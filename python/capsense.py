from __future__ import annotations

import threading
import time
from dataclasses import dataclass
from typing import Optional

from config import CAPSENSE_BUTTON_TIMEOUT_S

try:
    import serial
except ImportError:
    serial = None


@dataclass
class CapSenseState:
    x: int
    y: int
    btn0_active: bool
    btn1_active: bool


class CapSenseReader:
    """
    Reads performance data from the capsense board on its own serial port.

    Wire format: one ASCII line per event, no fixed frame size:
        "B0"          -> button 0 currently pressed (may repeat while held)
        "B1"          -> button 1 currently pressed (may repeat while held)
        "XY,<x>,<y>"  -> touch position on the XY matrix

    There's no explicit "released" message for the buttons, so a button is
    considered released once no B0/B1 line has arrived for
    CAPSENSE_BUTTON_TIMEOUT_S -- this works whether the firmware sends the
    line once per press or continuously while held.

    Runs its own background thread and just keeps the latest state around
    (get_state()) rather than queueing every sample, matching the "only the
    newest value matters" polling model used for the outgoing packet queue.
    """

    def __init__(self, port: str, baud: int):
        self.port = port
        self.baud = baud
        self._ser: Optional["serial.Serial"] = None
        self._thread: Optional[threading.Thread] = None
        self._running = False
        self._lock = threading.Lock()

        self._x = 0
        self._y = 0
        self._btn0_last_seen = 0.0
        self._btn1_last_seen = 0.0

    def start(self):
        if serial is None:
            raise RuntimeError("pyserial is required for capsense input")

        self._ser = serial.Serial(self.port, self.baud, timeout=0.2)
        self._running = True
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._thread.start()

    def stop(self):
        self._running = False
        if self._thread:
            self._thread.join(timeout=1.0)
        if self._ser and self._ser.is_open:
            self._ser.close()

    def get_state(self) -> CapSenseState:
        now = time.monotonic()
        with self._lock:
            return CapSenseState(
                x=self._x,
                y=self._y,
                btn0_active=(now - self._btn0_last_seen) < CAPSENSE_BUTTON_TIMEOUT_S,
                btn1_active=(now - self._btn1_last_seen) < CAPSENSE_BUTTON_TIMEOUT_S,
            )

    def _run(self):
        buffer = b""
        while self._running:
            try:
                chunk = self._ser.read(64)
            except Exception:
                time.sleep(0.05)
                continue

            if not chunk:
                continue

            buffer += chunk
            while b"\n" in buffer:
                line, buffer = buffer.split(b"\n", 1)
                self._handle_line(line.decode(errors="ignore").strip())

    def _handle_line(self, line: str):
        if not line:
            return

        now = time.monotonic()

        if line == "B0":
            with self._lock:
                self._btn0_last_seen = now
        elif line == "B1":
            with self._lock:
                self._btn1_last_seen = now
        elif line.startswith("XY,"):
            parts = line.split(",")
            if len(parts) == 3:
                try:
                    x = int(parts[1])
                    y = int(parts[2])
                except ValueError:
                    return
                with self._lock:
                    self._x = x
                    self._y = y
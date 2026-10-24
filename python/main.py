#!/usr/bin/env python3
from __future__ import annotations

import tkinter as tk
from tkinter import ttk, messagebox
import math

from config import *
from protocol import make_packet
from capsense import CapSenseReader

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    serial = None


class SynthControlApp:

    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title(APP_TITLE)
        self.root.geometry(f"{WINDOW_WIDTH}x{WINDOW_HEIGHT}")
        self.root.configure(bg=BG)

        # =====================================================
        # UART
        # =====================================================
        self.serial_port = None
        self.port_var = tk.StringVar()
        self.status_var = tk.StringVar(value="Disconnected")

        # =====================================================
        # SYNTH STATE
        # =====================================================
        self.mode_var = tk.StringVar(value="WAVETABLE")
        self.voice_var = tk.IntVar(value=0)
        self.active_voices_var = tk.IntVar(value=2)

        self.waveform_var = tk.StringVar(value="SINE")
        self.volume_var = tk.IntVar(value=4095 // MAX_WAVETABLE_VOICES)

        # Matches the firmware's initial additive_voice.osc.ampl[] array.
        self.harmonic_vars = [
            tk.IntVar(value=amp) for amp in
            (300, 150, 100, 75, 60, 50, 42, 10, 5, 10, 10, 5, 10, 20)
        ]

        # ADSR — attack/decay/sustain/release are all UI percentages (0-100).
        # Attack+Decay+Release are fractions of one sequencer step and are
        # kept clamped to <=100% together (see _clamp in on_adsr_time_changed).
        # Sustain is a separate, independent amplitude LEVEL (not a duration),
        # so it is not part of that time budget. Attack/decay/release are
        # sent to the firmware as raw percent values; sustain is perceptually
        # log in the UI (see percent_to_sustain_raw) but sent as a linear
        # 0-65535 amplitude.
        self.attack_var = tk.IntVar(value=5)
        self.decay_var = tk.IntVar(value=15)
        self.sustain_var = tk.IntVar(value=50)
        self.release_var = tk.IntVar(value=20)

        # Per-voice OSC_VOLUME state (wavetable mode). Kept clamped so the
        # sum across all voices never exceeds 4095 -- see on_volume_changed.
        # Matches firmware: voices 0/1 explicitly configured to 400 each,
        # voices 2/3 explicitly initialized to 0 (silent but selectable/
        # configurable; unused for audio while synth_active_voices=2).
        self.voice_volumes = {0: 400, 1: 400, 2: 0, 3: 0}

        # Sequencer
        self.seq_running = False
        self.seq_steps_var = tk.IntVar(value=8)
        self.seq_step_time_var = tk.IntVar(value=250)
        self.seq_note_vars: list[tk.IntVar] = []

        # =====================================================
        # PER-VOICE STATE
        # =====================================================
        # The firmware has one independent additive_voice and a separate
        # array of MAX_WAVETABLE_VOICES wavetable voices -- each with its own
        # ADSR/sequencer/waveform. The UI widgets are shared (one set of
        # sliders/spinboxes), so they're loaded from / saved to these dicts
        # whenever the selected voice or mode changes. "ADDITIVE" is used as
        # the dict key for the single additive voice so it never collides
        # with wavetable voice 0, which is a physically different voice on
        # the firmware side even though both default voice_var to 0.
        #
        # Initial values below match the firmware's static init (synth.c):
        # additive_voice, wavetable_voices[0] "Melody", wavetable_voices[1]
        # "Harmony", and wavetable_voices[2]/[3] (explicitly initialized to
        # these same generic defaults -- sine, 5/15/50/20% ADSR, 8 steps of
        # 440 @250ms, volume 0. Sustain
        # percentages are the perceptual-taper equivalent of the firmware's
        # raw sustain_level (see sustain_raw_to_percent): 30000 -> 83%,
        # 50000 -> 94%, 6554 -> 50%.
        self._voice_adsr_defaults = {"attack": 5, "decay": 15, "sustain": 50, "release": 20}
        self.voice_adsr = {
            key: dict(self._voice_adsr_defaults)
            for key in ["ADDITIVE"] + list(range(MAX_WAVETABLE_VOICES))
        }
        self.voice_adsr["ADDITIVE"] = {"attack": 20, "decay": 50, "sustain": 94, "release": 10}
        self.voice_adsr[0] = {"attack": 10, "decay": 25, "sustain": 83, "release": 20}
        self.voice_adsr[1] = {"attack": 0, "decay": 25, "sustain": 83, "release": 5}

        self._voice_seq_defaults = {"steps": 8, "step_time_ms": 250, "notes": [440] * 8, "running": False}
        self.voice_seq = {
            key: {**self._voice_seq_defaults, "notes": list(self._voice_seq_defaults["notes"])}
            for key in ["ADDITIVE"] + list(range(MAX_WAVETABLE_VOICES))
        }
        self.voice_seq["ADDITIVE"] = {
            "steps": 8, "step_time_ms": 150, "running": False,
            "notes": [100, 200, 300, 400, 500, 50, 100, 1000],
        }
        self.voice_seq[0] = {
            "steps": 64, "step_time_ms": 200, "running": True,
            "notes": [
                659, 0, 494, 523, 587, 0, 523, 494, 440, 0, 440, 523, 659, 0, 587, 523,
                494, 0, 0, 523, 587, 0, 659, 0, 523, 0, 440, 0, 440, 0, 0, 0,
                0, 587, 0, 698, 880, 0, 784, 698, 659, 0, 0, 523, 659, 0, 587, 523,
                494, 0, 494, 523, 587, 0, 659, 0, 523, 0, 440, 0, 440, 0, 0, 0,
            ],
        }
        self.voice_seq[1] = {
            "steps": 64, "step_time_ms": 200, "running": True,
            "notes": [
                0, 0, 208, 220, 247, 0, 220, 208, 165, 0, 165, 220, 262, 0, 247, 220,
                208, 0, 0, 220, 247, 0, 262, 0, 220, 0, 165, 0, 165, 0, 0, 0,
                0, 220, 0, 294, 349, 0, 330, 294, 0, 0, 0, 220, 262, 0, 247, 220,
                208, 0, 208, 220, 247, 0, 262, 0, 220, 0, 165, 0, 165, 0, 0, 0,
            ],
        }

        # Waveform only exists in wavetable mode (additive has no waveform concept).
        # Matches firmware: voice 0 = saw, voice 1 = square, voices 2/3
        # explicitly initialized to sine.
        self.voice_waveform = {0: "SAW", 1: "SQUARE", 2: "SINE", 3: "SINE"}

        # LFO
        self.lfo_dest_var = tk.StringVar(value="NONE")
        self.lfo_freq_var = tk.DoubleVar(value=5.0)
        self.lfo_depth_var = tk.IntVar(value=50)
        self.lfo_waveform_var = tk.StringVar(value="SINE")

        # Outgoing packets are coalesced here and flushed on a fixed
        # heartbeat (see queue_packet / _flush_pending) instead of being
        # written to the wire on every single UI event.
        self._pending_packets: dict[tuple, int] = {}
        self._last_sent: dict[tuple, int] = {}
        self._dragging = False  # True while a slider is mid-drag; suppresses sends until release

        # =====================================================
        # CAPSENSE PERFORMANCE CONTROLLER
        # =====================================================
        self.capsense_reader: CapSenseReader | None = None
        self.capsense_port_var = tk.StringVar(value=CAPSENSE_DEFAULT_PORT)
        self.capsense_status_var = tk.StringVar(value="Disconnected")

        self.capsense_latched = False
        self._capsense_btn0_prev = False
        self._capsense_btn1_prev = False

        # exponential-moving-average smoothing for the noisy raw XY stream
        self._capsense_x_smooth = 0.5
        self._capsense_y_smooth = 0.5

        # =====================================================
        # STYLE
        # =====================================================
        self.configure_style()

        # =====================================================
        # BUILD UI
        # =====================================================
        self.build_ui()

        # =====================================================
        # INITIAL DRAW
        # =====================================================
        self.refresh_ports()
        self._apply_mode_ui()
        self.draw_spectrum()
        self._update_harmonics_budget_label()
        self._update_voice_volume_budget_label()
        self.draw_lfo_preview()
        self.draw_capsense_pad()

        self.root.after(CONTROL_SEND_INTERVAL_MS, self._flush_pending)

    # =====================================================
    # STYLE
    # =====================================================
    def configure_style(self):
        style = ttk.Style()
        try:
            style.theme_use("clam")
        except tk.TclError:
            pass

        style.configure(".", background=BG, foreground=TEXT)
        style.configure("TFrame", background=BG)
        style.configure("Panel.TFrame", background=PANEL)
        style.configure("TLabel", background=BG, foreground=TEXT)
        style.configure("Panel.TLabel", background=PANEL, foreground=TEXT)
        style.configure("TLabelframe", background=BG, foreground=TEXT)
        style.configure("TLabelframe.Label", background=BG, foreground=TEXT)
        style.configure("TButton", background=PANEL, foreground=TEXT, padding=6)
        style.configure("Accent.TButton", background=ACCENT, foreground="#ffffff", padding=7)
        style.configure("TCombobox", fieldbackground=PANEL, foreground=TEXT)
        style.configure("TNotebook", background=BG, borderwidth=0)
        style.configure("TNotebook.Tab", background=PANEL, foreground=TEXT, padding=(12, 6))
        style.map("TNotebook.Tab", background=[("selected", ACCENT)], foreground=[("selected", "#ffffff")])
        style.configure("TSpinbox", fieldbackground=PANEL, foreground=TEXT, arrowsize=14)
        style.map(
            "TCombobox",
            fieldbackground=[("readonly", PANEL), ("disabled", BG)],
            foreground=[("disabled", MUTED)],
        )

    # =====================================================
    # UI
    # =====================================================
    def build_ui(self):
        outer = ttk.Frame(self.root, padding=10)
        outer.pack(fill="both", expand=True)

        self.build_uart_bar(outer)
        self.build_top_controls(outer)

        content = ttk.Panedwindow(outer, orient="horizontal")
        content.pack(fill="both", expand=True, pady=10)

        self.left = ttk.Frame(content, padding=4)
        self.middle = ttk.Frame(content, padding=4)
        self.right = ttk.Frame(content, padding=4)

        content.add(self.left, weight=4)
        content.add(self.middle, weight=3)
        content.add(self.right, weight=3)

        self.build_dynamic_panel(self.left)
        self.build_visual_panel(self.middle)
        self.build_right_panels(self.right)
        self.build_log(outer)

    # =====================================================
    # UART BAR
    # =====================================================
    def build_uart_bar(self, parent):
        frame = ttk.LabelFrame(parent, text="UART", padding=8)
        frame.pack(fill="x")

        self.port_combo = ttk.Combobox(
            frame, textvariable=self.port_var, width=16, state="readonly"
        )
        self.port_combo.grid(row=0, column=0, padx=6)

        self.refresh_btn = ttk.Button(
            frame, text="Refresh", command=self.refresh_ports
        )
        self.refresh_btn.grid(row=0, column=1, padx=4)

        self.toggle_conn_btn = ttk.Button(
            frame, text="Connect", style="Accent.TButton", command=self.toggle_connection
        )
        self.toggle_conn_btn.grid(row=0, column=2, padx=4)

        ttk.Label(frame, text=f"{DEFAULT_BAUD} baud", foreground=MUTED).grid(row=0, column=3, padx=12)

        self.status_label = tk.Label(
            frame, textvariable=self.status_var, bg=BG, fg=WARN, font=("TkDefaultFont", 9, "bold")
        )
        self.status_label.grid(row=0, column=4, sticky="w")

        frame.columnconfigure(4, weight=1)

    # =====================================================
    # TOP CONTROLS
    # =====================================================
    def build_top_controls(self, parent):
        frame = ttk.Frame(parent)
        frame.pack(fill="x", pady=(10, 0))

        ttk.Label(frame, text="Mode:").pack(side="left")

        mode_box = ttk.Combobox(
            frame, textvariable=self.mode_var,
            values=["ADDITIVE", "WAVETABLE"], state="readonly", width=14
        )
        mode_box.pack(side="left", padx=6)
        mode_box.bind("<<ComboboxSelected>>", self.on_mode_changed)

        ttk.Label(frame, text="Voice:").pack(side="left", padx=(20, 0))

        self.voice_box = ttk.Combobox(
            frame, textvariable=self.voice_var,
            values=list(range(MAX_WAVETABLE_VOICES)), state="readonly", width=6
        )
        self.voice_box.pack(side="left", padx=6)
        self.voice_box.bind("<<ComboboxSelected>>", self.on_voice_changed)

        self.voice_hint = ttk.Label(frame, text="", foreground=MUTED)
        self.voice_hint.pack(side="left", padx=(10, 0))

        ttk.Label(frame, text="Active Voices:").pack(side="left", padx=(20, 0))

        self.active_voices_spin = ttk.Spinbox(
            frame, from_=1, to=MAX_WAVETABLE_VOICES, textvariable=self.active_voices_var, width=4
        )
        self.active_voices_spin.pack(side="left", padx=6)

        self.active_voices_btn = ttk.Button(
            frame, text="Set", command=self.on_set_active_voices
        )
        self.active_voices_btn.pack(side="left")

    # =====================================================
    # DYNAMIC PANEL (ADDITIVE / WAVETABLE)
    # =====================================================
    def build_dynamic_panel(self, parent):
        self.dynamic_container = ttk.Frame(parent)
        self.dynamic_container.pack(fill="both", expand=True)

        # additive frame
        self.additive_frame = ttk.LabelFrame(
            self.dynamic_container, text="Additive Harmonics", padding=10
        )

        self.spectrum_canvas = tk.Canvas(
            self.additive_frame, height=220, bg=PANEL, highlightthickness=0
        )
        self.spectrum_canvas.pack(fill="x", pady=(0, 10))
        self.spectrum_canvas.bind("<Configure>", lambda e: self.draw_spectrum())

        sliders = ttk.Frame(self.additive_frame)
        sliders.pack(fill="x")

        for i, var in enumerate(self.harmonic_vars, start=1):
            col = i - 1
            scale = tk.Scale(
                sliders, from_=4095, to=0, variable=var,
                orient="vertical", length=180,
                bg=PANEL, fg=TEXT, troughcolor=GRID, highlightthickness=0,
                command=lambda value, idx=i - 1: self.on_harmonic_changed(idx, value)
            )
            scale.grid(row=0, column=col, padx=2)
            self._bind_drag_gating(scale)
            ttk.Label(sliders, text=f"H{i}").grid(row=1, column=col)

        self.harmonics_budget_label = tk.Label(
            self.additive_frame, text="", bg=BG, fg=MUTED, font=("TkDefaultFont", 8)
        )
        self.harmonics_budget_label.pack(anchor="w", pady=(8, 0))

        # wavetable frame
        self.wavetable_frame = ttk.LabelFrame(
            self.dynamic_container, text="Wavetable Voice", padding=10
        )

        ttk.Label(self.wavetable_frame, text="Waveform").pack(anchor="w")

        wave_box = ttk.Combobox(
            self.wavetable_frame, textvariable=self.waveform_var,
            values=["SINE", "SAW", "SQUARE"], state="readonly"
        )
        wave_box.pack(fill="x", pady=(0, 12))
        wave_box.bind("<<ComboboxSelected>>", self.on_waveform_changed)

        _, self.volume_value_label = self._build_labeled_scale(
            self.wavetable_frame, "Volume", self.volume_var, 0, 4095,
            self.on_volume_changed
        )

        self.voice_volume_budget_label = tk.Label(
            self.wavetable_frame, text="", bg=BG, fg=MUTED, font=("TkDefaultFont", 8)
        )
        self.voice_volume_budget_label.pack(anchor="w")

    # =====================================================
    # VISUAL PANEL
    # =====================================================
    def build_visual_panel(self, parent):
        frame = ttk.LabelFrame(parent, text="Waveform Preview", padding=10)
        frame.pack(fill="both", expand=True)

        self.wave_canvas = tk.Canvas(frame, bg=PANEL, height=260, highlightthickness=0)
        self.wave_canvas.pack(fill="both", expand=True)
        self.wave_canvas.bind("<Configure>", lambda e: self.draw_waveform())

    # =====================================================
    # RIGHT PANELS: ADSR / SEQUENCER / LFO
    # =====================================================
    def build_right_panels(self, parent):
        frame = ttk.LabelFrame(parent, text="Sequencer / LFO / ADSR", padding=6)
        frame.pack(fill="both", expand=True)

        notebook = ttk.Notebook(frame)
        notebook.pack(fill="both", expand=True)

        adsr_tab = ttk.Frame(notebook, padding=10)
        seq_tab = ttk.Frame(notebook, padding=10)
        lfo_tab = ttk.Frame(notebook, padding=10)
        perf_tab = ttk.Frame(notebook, padding=10)

        notebook.add(adsr_tab, text="ADSR")
        notebook.add(seq_tab, text="Sequencer")
        notebook.add(lfo_tab, text="LFO")
        notebook.add(perf_tab, text="Performance")

        self.build_adsr_tab(adsr_tab)
        self.build_sequencer_tab(seq_tab)
        self.build_lfo_tab(lfo_tab)
        self.build_performance_tab(perf_tab)

    # ---------------------------------------------------
    # Shared: labeled slider with a live value readout
    # ---------------------------------------------------
    def _build_labeled_scale(self, parent, label_text, var, lo, hi, on_change, unit="", resolution=None):
        row = ttk.Frame(parent)
        row.pack(fill="x", pady=(0, 12))

        header = ttk.Frame(row)
        header.pack(fill="x")

        ttk.Label(header, text=label_text).pack(side="left")

        value_label = tk.Label(
            header, text=self._fmt_value(var.get(), unit),
            bg=BG, fg=ACCENT, font=("TkDefaultFont", 9, "bold")
        )
        value_label.pack(side="right")

        scale_kwargs = dict(
            from_=lo, to=hi, orient="horizontal", variable=var,
            bg=PANEL, fg=TEXT, troughcolor=GRID, highlightthickness=0,
            sliderrelief="flat", showvalue=False,
        )
        if resolution is not None:
            scale_kwargs["resolution"] = resolution

        scale = tk.Scale(
            row, **scale_kwargs,
            command=lambda value: self._on_scale_change(value, value_label, unit, on_change)
        )
        scale.pack(fill="x")
        self._bind_drag_gating(scale)
        return scale, value_label

    def _on_scale_change(self, value, value_label, unit, on_change):
        value_label.configure(text=self._fmt_value(value, unit))
        on_change(value)

    @staticmethod
    def _fmt_value(value, unit=""):
        try:
            f = float(value)
        except (TypeError, ValueError):
            return str(value)
        if f == int(f):
            return f"{int(f)}{unit}"
        return f"{f:.1f}{unit}"

    def _current_voice_key(self):
        """
        Key used for per-voice ADSR/sequencer/waveform storage. The additive
        voice and wavetable voice 0 are physically different voices on the
        firmware side even though voice_var is locked to 0 in additive mode,
        so "ADDITIVE" is a distinct key from the integer wavetable voice ids.
        """
        return "ADDITIVE" if self.mode_var.get() == "ADDITIVE" else self.voice_var.get()

    # ---------------------------------------------------
    # ADSR TAB (per-voice: MODULE_ADSR, voice = self.voice_var)
    # ---------------------------------------------------
    def build_adsr_tab(self, parent):
        ttk.Label(parent, text="Applies to the selected voice", foreground=MUTED).pack(anchor="w", pady=(0, 10))

        self.adsr_canvas = tk.Canvas(parent, height=140, bg=PANEL, highlightthickness=0)
        self.adsr_canvas.pack(fill="x", pady=(0, 14))
        self.adsr_canvas.bind("<Configure>", lambda e: self.draw_adsr_envelope())

        _, self.attack_value_label = self._build_labeled_scale(
            parent, "Attack", self.attack_var, 0, 100,
            lambda v: self.on_adsr_time_changed(ADSR_ATTACK, v), unit="%"
        )
        _, self.decay_value_label = self._build_labeled_scale(
            parent, "Decay", self.decay_var, 0, 100,
            lambda v: self.on_adsr_time_changed(ADSR_DECAY, v), unit="%"
        )

        self.adsr_budget_label = tk.Label(parent, text="", bg=BG, fg=MUTED, font=("TkDefaultFont", 8))
        self.adsr_budget_label.pack(anchor="w", pady=(0, 12))
        self._update_adsr_budget_label()

        # Sustain is an amplitude LEVEL, not a slice of the step's time
        # budget, so it's intentionally independent of Attack/Decay/Release.
        _, self.sustain_value_label = self._build_labeled_scale(
            parent, "Sustain", self.sustain_var, 0, 100,
            self.on_sustain_changed, unit="%"
        )

        _, self.release_value_label = self._build_labeled_scale(
            parent, "Release", self.release_var, 0, 100,
            lambda v: self.on_adsr_time_changed(ADSR_RELEASE, v), unit="%"
        )

    def _adsr_time_vars(self):
        return {
            ADSR_ATTACK: self.attack_var,
            ADSR_DECAY: self.decay_var,
            ADSR_RELEASE: self.release_var,
        }

    def _adsr_time_labels(self):
        return {
            ADSR_ATTACK: self.attack_value_label,
            ADSR_DECAY: self.decay_value_label,
            ADSR_RELEASE: self.release_value_label,
        }

    def _update_adsr_budget_label(self):
        total = sum(v.get() for v in self._adsr_time_vars().values())
        self.adsr_budget_label.configure(
            text=f"A+D+R: {total}% / 100% of step",
            fg=WARN if total >= 100 else MUTED,
        )

    def _adsr_field_names(self):
        return {ADSR_ATTACK: "attack", ADSR_DECAY: "decay", ADSR_RELEASE: "release"}

    def on_adsr_time_changed(self, parameter, value):
        requested = max(0.0, min(float(value), 100.0))

        # Attack+Decay+Release together are fractions of one sequencer step
        # and may never exceed 100% -- clamp this slider to whatever budget
        # the other two haven't used. Sustain is a level, not a duration,
        # and is handled separately (see on_sustain_changed).
        others_sum = sum(v.get() for k, v in self._adsr_time_vars().items() if k != parameter)
        percent = min(requested, max(0.0, 100.0 - others_sum))

        var = self._adsr_time_vars()[parameter]
        var.set(round(percent))
        self._adsr_time_labels()[parameter].configure(text=self._fmt_value(percent, "%"))

        # Attack/decay/release are sent directly as a 0-100 percent value.
        raw = int(round(percent))
        self.queue_packet(MODULE_ADSR, self.voice_var.get(), parameter, 0, raw)
        self.draw_adsr_envelope()
        self._update_adsr_budget_label()

        field = self._adsr_field_names()[parameter]
        self.voice_adsr[self._current_voice_key()][field] = round(percent)

    def on_sustain_changed(self, value):
        self._apply_sustain(float(value))

    def _apply_sustain(self, percent):
        percent = max(0.0, min(float(percent), 100.0))
        self.sustain_var.set(round(percent))
        self.sustain_value_label.configure(text=self._fmt_value(percent, "%"))
        # UI is perceptually log (%), wire value is linear 0-65535.
        raw = self.percent_to_sustain_raw(percent)
        self.queue_packet(MODULE_ADSR, self.voice_var.get(), ADSR_SUSTAIN, 0, raw)
        self.draw_adsr_envelope()
        self.voice_adsr[self._current_voice_key()]["sustain"] = round(percent)
        return percent

    def _load_adsr_state(self, key):
        """Reload the ADSR widgets/labels to reflect the given voice's stored
        state (does not re-send packets -- the firmware already has this
        voice's actual state from when it was last set)."""
        state = self.voice_adsr[key]

        self.attack_var.set(state["attack"])
        self.attack_value_label.configure(text=self._fmt_value(state["attack"], "%"))

        self.decay_var.set(state["decay"])
        self.decay_value_label.configure(text=self._fmt_value(state["decay"], "%"))

        self.sustain_var.set(state["sustain"])
        self.sustain_value_label.configure(text=self._fmt_value(state["sustain"], "%"))

        self.release_var.set(state["release"])
        self.release_value_label.configure(text=self._fmt_value(state["release"], "%"))

        self._update_adsr_budget_label()
        self.draw_adsr_envelope()

    @staticmethod
    def percent_to_sustain_raw(percent: float) -> int:
        """
        Perceptual-to-linear volume mapping ("audio taper"): the UI slider
        is 0-100% and feels linear to the ear, but a linear pot spends most
        of its travel sounding quiet since human loudness perception is
        logarithmic. This maps percent onto a fixed dB range and converts
        that to the linear 0-65535 amplitude the firmware expects.
        """
        p = max(0.0, min(percent, 100.0)) / 100.0
        if p <= 0.0:
            return 0
        db = (p - 1.0) * ADSR_SUSTAIN_TAPER_DB  # 0% -> -taper dB, 100% -> 0 dB
        linear = 10 ** (db / 20.0)
        return int(round(linear * ADSR_SUSTAIN_RAW_MAX))

    @staticmethod
    def sustain_raw_to_percent(raw: int) -> int:
        """Inverse of percent_to_sustain_raw, e.g. for syncing UI to a loaded/reported value."""
        if raw <= 0:
            return 0
        linear = max(raw / ADSR_SUSTAIN_RAW_MAX, 1e-6)
        db = 20.0 * math.log10(linear)
        percent = (db / ADSR_SUSTAIN_TAPER_DB + 1.0) * 100.0
        return int(round(max(0.0, min(percent, 100.0))))

    def draw_adsr_envelope(self):
        c = self.adsr_canvas
        c.delete("all")

        w = max(c.winfo_width(), 120)
        h = max(c.winfo_height(), 80)
        margin_x, margin_top, margin_bottom = 14, 14, 22

        # Attack/decay/release are percentages of a nominal reference duration,
        # used only to give the diagram sensible proportions -- this is not
        # transmitted, the wire values are the raw percentages themselves.
        attack = self.attack_var.get() / 100 * ADSR_TIME_MAX_MS
        decay = self.decay_var.get() / 100 * ADSR_TIME_MAX_MS
        release = self.release_var.get() / 100 * ADSR_TIME_MAX_MS
        sustain_pct = self.sustain_var.get()
        hold = max(120, int(ADSR_TIME_MAX_MS * 0.06))  # fixed visual hold segment

        total = max(attack + decay + hold + release, 1)
        usable_w = w - 2 * margin_x
        usable_h = h - margin_top - margin_bottom

        def xt(t_ms):
            return margin_x + (t_ms / total) * usable_w

        def yl(level_pct):
            return h - margin_bottom - (level_pct / 100) * usable_h

        for frac in (0.25, 0.5, 0.75):
            y = margin_top + frac * usable_h
            c.create_line(margin_x, y, w - margin_x, y, fill=GRID)

        stages = [
            (0, 0),
            (attack, 100),
            (attack + decay, sustain_pct),
            (attack + decay + hold, sustain_pct),
            (attack + decay + hold + release, 0),
        ]

        coords = []
        for t_ms, level in stages:
            coords.extend([xt(t_ms), yl(level)])

        baseline_y = yl(0)
        polygon = coords + [xt(stages[-1][0]), baseline_y, xt(stages[0][0]), baseline_y]
        c.create_polygon(*polygon, fill="#1f3550", outline="")
        c.create_line(*coords, fill=ACCENT, width=2, joinstyle="round")

        for r in (0, 1, 2, 3):
            x, y = coords[r * 2], coords[r * 2 + 1]
            c.create_oval(x - 3, y - 3, x + 3, y + 3, fill=ACCENT, outline="")

        labels = [
            ("A", attack / 2),
            ("D", attack + decay / 2),
            ("S", attack + decay + hold / 2),
            ("R", attack + decay + hold + release / 2),
        ]
        for text, t_ms in labels:
            c.create_text(xt(t_ms), h - 8, text=text, fill=MUTED, font=("TkDefaultFont", 8, "bold"))

    # ---------------------------------------------------
    # SEQUENCER TAB (per-voice: MODULE_SEQ, voice = self.voice_var)
    # ---------------------------------------------------
    def build_sequencer_tab(self, parent):
        top = ttk.Frame(parent)
        top.pack(fill="x", pady=(0, 12))

        self.seq_toggle_btn = ttk.Button(
            top, text="Start", style="Accent.TButton", command=self.on_seq_toggle
        )
        self.seq_toggle_btn.pack(side="left")

        ttk.Label(top, text="Applies to the selected voice", foreground=MUTED).pack(side="left", padx=12)

        row = ttk.Frame(parent)
        row.pack(fill="x", pady=(0, 12))

        ttk.Label(row, text="Steps").grid(row=0, column=0, sticky="w")
        steps_spin = ttk.Spinbox(
            row, from_=1, to=MAX_SEQUENCE_STEPS, textvariable=self.seq_steps_var,
            width=6, command=self.on_seq_steps_changed
        )
        steps_spin.grid(row=1, column=0, sticky="w", padx=(0, 24), pady=(2, 0))
        steps_spin.bind("<Return>", lambda e: self.on_seq_steps_changed())
        steps_spin.bind("<FocusOut>", lambda e: self.on_seq_steps_changed())

        ttk.Label(row, text="Step time (ms)").grid(row=0, column=1, sticky="w")
        step_time_spin = ttk.Spinbox(
            row, from_=10, to=SEQ_STEP_TIME_MAX_MS, textvariable=self.seq_step_time_var,
            width=8, command=self.on_seq_step_time_changed
        )
        step_time_spin.grid(row=1, column=1, sticky="w", pady=(2, 0))
        step_time_spin.bind("<Return>", lambda e: self.on_seq_step_time_changed())
        step_time_spin.bind("<FocusOut>", lambda e: self.on_seq_step_time_changed())

        ttk.Label(parent, text="Step notes (Hz)").pack(anchor="w")

        self.seq_steps_frame = ttk.Frame(parent)
        self.seq_steps_frame.pack(fill="both", expand=True, pady=(4, 0))

    def rebuild_sequence_steps(self):
        for child in self.seq_steps_frame.winfo_children():
            child.destroy()

        count = self.seq_steps_var.get()
        key = self._current_voice_key()

        # Reuse this voice's stored notes, padding with a default or
        # truncating to match the current step count.
        stored_notes = self.voice_seq[key]["notes"]
        notes = [stored_notes[i] if i < len(stored_notes) else 440 for i in range(count)]
        self.voice_seq[key]["notes"] = notes

        self.seq_note_vars = [tk.IntVar(value=notes[i]) for i in range(count)]

        cols = 4
        for i, var in enumerate(self.seq_note_vars):
            cell = ttk.Frame(self.seq_steps_frame)
            cell.grid(row=i // cols, column=i % cols, padx=4, pady=4, sticky="w")

            ttk.Label(cell, text=f"Step {i + 1}", foreground=MUTED).pack(anchor="w")
            entry = ttk.Spinbox(
                cell, from_=0, to=SEQ_NOTE_MAX_HZ, textvariable=var, width=8
            )
            entry.pack()
            entry.bind("<Return>", lambda e, idx=i: self.on_seq_note_changed(idx))
            entry.bind("<FocusOut>", lambda e, idx=i: self.on_seq_note_changed(idx))

    def on_seq_toggle(self):
        self.seq_running = not self.seq_running
        self.seq_toggle_btn.configure(text="Stop" if self.seq_running else "Start")
        self.queue_packet(
            MODULE_SEQ, self.voice_var.get(), SEQ_STATE, 0,
            SEQ_START if self.seq_running else SEQ_STOP
        )
        self.voice_seq[self._current_voice_key()]["running"] = self.seq_running

    def on_seq_steps_changed(self):
        count = max(1, min(int(self.seq_steps_var.get() or 1), MAX_SEQUENCE_STEPS))
        self.seq_steps_var.set(count)
        self.queue_packet(MODULE_SEQ, self.voice_var.get(), SEQ_STEPS, 0, count)
        self.voice_seq[self._current_voice_key()]["steps"] = count
        self.rebuild_sequence_steps()

    def on_seq_step_time_changed(self):
        value = max(10, min(int(self.seq_step_time_var.get() or 10), SEQ_STEP_TIME_MAX_MS))
        self.seq_step_time_var.set(value)
        self.queue_packet(MODULE_SEQ, self.voice_var.get(), SEQ_STEP_TIME, 0, value)
        self.voice_seq[self._current_voice_key()]["step_time_ms"] = value

    def on_seq_note_changed(self, index):
        var = self.seq_note_vars[index]
        value = max(0, min(int(var.get() or 0), SEQ_NOTE_MAX_HZ))
        var.set(value)
        self.queue_packet(MODULE_SEQ, self.voice_var.get(), SEQ_NOTE, index, value)

        notes = self.voice_seq[self._current_voice_key()]["notes"]
        if index < len(notes):
            notes[index] = value

    def _load_seq_state(self, key):
        """Reload the sequencer widgets to reflect the given voice's stored
        state. """
        state = self.voice_seq[key]

        self.seq_steps_var.set(state["steps"])
        self.seq_step_time_var.set(state["step_time_ms"])

        self.seq_running = state["running"]
        self.seq_toggle_btn.configure(text="Stop" if self.seq_running else "Start")

        self.rebuild_sequence_steps()

    # ---------------------------------------------------
    # LFO TAB (global: MODULE_LFO, voice = 0xFF)
    # ---------------------------------------------------
    def build_lfo_tab(self, parent):
        ttk.Label(parent, text="Global parameter (applies to all voices)", foreground=MUTED).pack(anchor="w", pady=(0, 10))

        self.lfo_canvas = tk.Canvas(parent, height=110, bg=PANEL, highlightthickness=0)
        self.lfo_canvas.pack(fill="x", pady=(0, 14))
        self.lfo_canvas.bind("<Configure>", lambda e: self.draw_lfo_preview())

        ttk.Label(parent, text="Destination").pack(anchor="w")
        dest_box = ttk.Combobox(
            parent, textvariable=self.lfo_dest_var,
            values=["NONE", "AMPLITUDE"], state="readonly"
        )
        dest_box.pack(fill="x", pady=(0, 12))
        dest_box.bind("<<ComboboxSelected>>", self.on_lfo_destination_changed)

        ttk.Label(parent, text="Waveform").pack(anchor="w")
        wave_box = ttk.Combobox(
            parent, textvariable=self.lfo_waveform_var,
            values=["SINE", "SAW", "SQUARE"], state="readonly"
        )
        wave_box.pack(fill="x", pady=(0, 12))
        wave_box.bind("<<ComboboxSelected>>", self.on_lfo_waveform_changed)

        _, self.lfo_freq_value_label = self._build_labeled_scale(
            parent, "Frequency", self.lfo_freq_var, 0, LFO_FREQ_MAX_HZ,
            self.on_lfo_freq_changed, unit=" Hz", resolution=0.1
        )
        _, self.lfo_depth_value_label = self._build_labeled_scale(
            parent, "Depth", self.lfo_depth_var, 0, 100,
            self.on_lfo_depth_changed, unit="%"
        )

    def on_lfo_destination_changed(self, _event=None):
        dest = {"NONE": LFO_DEST_NONE, "AMPLITUDE": LFO_DEST_AMPLITUDE}[self.lfo_dest_var.get()]
        self.queue_packet(MODULE_LFO, 0xFF, LFO_DESTINATION, 0, dest)
        self.draw_lfo_preview()

    def on_lfo_waveform_changed(self, _event=None):
        waves = {"SINE": WAVE_SINE, "SAW": WAVE_SAW, "SQUARE": WAVE_SQUARE}
        self.queue_packet(MODULE_LFO, 0xFF, LFO_WAVEFORM, 0, waves[self.lfo_waveform_var.get()])
        self.draw_lfo_preview()

    def on_lfo_freq_changed(self, value):
        self._apply_lfo_freq(float(value))

    def _apply_lfo_freq(self, hz):
        hz = max(0.0, min(float(hz), LFO_FREQ_MAX_HZ))
        self.lfo_freq_var.set(hz)
        self.lfo_freq_value_label.configure(text=self._fmt_value(hz, " Hz"))
        self.queue_packet(MODULE_LFO, 0xFF, LFO_FREQ, 0, int(round(hz)))
        self.draw_lfo_preview()
        return hz

    def on_lfo_depth_changed(self, value):
        self._apply_lfo_depth(float(value))

    def _apply_lfo_depth(self, percent):
        percent = max(0.0, min(float(percent), 100.0))
        self.lfo_depth_var.set(round(percent))
        self.lfo_depth_value_label.configure(text=self._fmt_value(percent, "%"))
        raw = int(percent / 100 * LFO_DEPTH_MAX)
        self.queue_packet(MODULE_LFO, 0xFF, LFO_DEPTH, 0, raw)
        self.draw_lfo_preview()
        return percent

    def draw_lfo_preview(self):
        c = self.lfo_canvas
        c.delete("all")

        w = max(c.winfo_width(), 120)
        h = max(c.winfo_height(), 60)
        mid = h / 2

        c.create_line(8, mid, w - 8, mid, fill=GRID)

        if self.lfo_dest_var.get() == "NONE":
            c.create_text(w / 2, mid, text="LFO inactive — pick a destination", fill=MUTED, font=("TkDefaultFont", 9))
            return

        freq = self.lfo_freq_var.get()
        depth = self.lfo_depth_var.get() / 100.0
        wf = self.lfo_waveform_var.get()

        # Visual cycle count is a stylized mapping of freq -> on-screen density,
        # not a literal 1:1 Hz-to-pixel scale (real LFO rates are too slow/fast to read that way).
        cycles = 1 + (freq / LFO_FREQ_MAX_HZ) * 6
        amp = depth * (h * 0.38)

        points = []
        for px in range(8, w - 8):
            t = (px - 8) / max(1, (w - 16))
            phase = t * cycles
            if wf == "SINE":
                y = math.sin(2 * math.pi * phase)
            elif wf == "SAW":
                y = 2 * (phase - math.floor(phase + 0.5))
            else:
                y = 1 if math.sin(2 * math.pi * phase) >= 0 else -1
            points.extend([px, mid - y * amp])

        if len(points) >= 4:
            c.create_line(*points, fill=ACCENT, width=2, joinstyle="round")

    # ---------------------------------------------------
    # PERFORMANCE TAB (capsense board: 2 buttons + XY matrix)
    # ---------------------------------------------------
    def build_performance_tab(self, parent):
        connect_row = ttk.Frame(parent)
        connect_row.pack(fill="x", pady=(0, 10))

        self.capsense_port_combo = ttk.Combobox(
            connect_row, textvariable=self.capsense_port_var, width=12, state="normal"
        )
        self.capsense_port_combo.pack(side="left")

        ttk.Button(connect_row, text="Refresh", command=self.refresh_capsense_ports).pack(side="left", padx=4)

        self.capsense_connect_btn = ttk.Button(
            connect_row, text="Connect", style="Accent.TButton", command=self.toggle_capsense_connection
        )
        self.capsense_connect_btn.pack(side="left", padx=4)

        self.capsense_status_label = tk.Label(
            connect_row, textvariable=self.capsense_status_var, bg=BG, fg=WARN, font=("TkDefaultFont", 9, "bold")
        )
        self.capsense_status_label.pack(side="left", padx=10)

        pad_row = ttk.Frame(parent)
        pad_row.pack(fill="x", pady=(0, 10))

        self.capsense_xy_canvas = tk.Canvas(pad_row, width=140, height=140, bg=PANEL, highlightthickness=0)
        self.capsense_xy_canvas.pack(side="left")
        self.capsense_xy_canvas.bind("<Configure>", lambda e: self.draw_capsense_pad())

        side = ttk.Frame(pad_row)
        side.pack(side="left", fill="both", expand=True, padx=(12, 0))

        self.capsense_btn0_label = tk.Label(side, text="\u25cf Button 0", bg=BG, fg=MUTED)
        self.capsense_btn0_label.pack(anchor="w")
        self.capsense_btn1_label = tk.Label(side, text="\u25cf Button 1", bg=BG, fg=MUTED)
        self.capsense_btn1_label.pack(anchor="w")
        self.capsense_latch_label = tk.Label(side, text="Latch: off", bg=BG, fg=MUTED)
        self.capsense_latch_label.pack(anchor="w", pady=(8, 0))

        ttk.Separator(parent).pack(fill="x", pady=10)
        ttk.Label(parent, text="Mapping", font=("TkDefaultFont", 9, "bold")).pack(anchor="w")
        ttk.Label(
            parent,
            text=(
                "X \u2192 formant peak position (additive)\n"
                "     / LFO depth (wavetable)\n"
                "Y \u2192 formant sharpness (additive)\n"
                "     / LFO frequency (wavetable)\n"
                "Button 0 \u2192 latch (freeze XY)\n"
                "Button 1 \u2192 sequencer start/stop\n\n"
                "Wavetable: set LFO Destination to Amplitude\n"
                "in the LFO tab to hear the X/Y effect."
            ),
            foreground=MUTED, justify="left"
        ).pack(anchor="w", pady=(2, 0))

        self.refresh_capsense_ports()

    def refresh_capsense_ports(self):
        if serial is None:
            return
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.capsense_port_combo["values"] = ports

    def toggle_capsense_connection(self):
        if self.capsense_reader:
            self.capsense_reader.stop()
            self.capsense_reader = None
            self.capsense_status_var.set("Disconnected")
            self.capsense_status_label.configure(fg=WARN)
            self.capsense_connect_btn.configure(text="Connect")
            self.log("[CAPSENSE DISCONNECTED]")
            return

        if serial is None:
            messagebox.showerror("PySerial", "Install pyserial first")
            return

        port = self.capsense_port_var.get().strip()
        if not port:
            return

        try:
            self.capsense_reader = CapSenseReader(port, CAPSENSE_BAUD)
            self.capsense_reader.start()
            self.capsense_status_var.set(f"Connected: {port}")
            self.capsense_status_label.configure(fg=SUCCESS)
            self.capsense_connect_btn.configure(text="Disconnect")
            self.log(f"[CAPSENSE CONNECTED] {port}")
        except Exception as exc:
            messagebox.showerror("CapSense", str(exc))

    def draw_capsense_pad(self):
        c = self.capsense_xy_canvas
        c.delete("all")
        w = max(c.winfo_width(), 40)
        h = max(c.winfo_height(), 40)

        c.create_rectangle(2, 2, w - 2, h - 2, outline=GRID)
        c.create_line(w / 2, 2, w / 2, h - 2, fill=GRID)
        c.create_line(2, h / 2, w - 2, h / 2, fill=GRID)

        px = 4 + self._capsense_x_smooth * (w - 8)
        py = 4 + (1 - self._capsense_y_smooth) * (h - 8)  # invert: up = higher Y
        r = 6
        fill = MUTED if (not self.capsense_reader or self.capsense_latched) else ACCENT
        c.create_oval(px - r, py - r, px + r, py + r, fill=fill, outline="")

    def poll_capsense(self):
        if not self.capsense_reader:
            return

        state = self.capsense_reader.get_state()

        # live button indicator (physical held-state, independent of the
        # one-shot toggle actions below)
        self.capsense_btn0_label.configure(fg=ACCENT if state.btn0_active else MUTED)
        self.capsense_btn1_label.configure(fg=ACCENT if state.btn1_active else MUTED)

        # -- buttons: rising-edge triggered, one action per press --
        if state.btn0_active and not self._capsense_btn0_prev:
            self._on_capsense_button0()
        self._capsense_btn0_prev = state.btn0_active

        if state.btn1_active and not self._capsense_btn1_prev:
            self._on_capsense_button1()
        self._capsense_btn1_prev = state.btn1_active

        # smooth the raw XY stream regardless of latch, so the pad dot never
        # jumps the instant latch is released
        x_norm = max(0.0, min(state.x / CAPSENSE_X_MAX, 1.0))
        y_norm = max(0.0, min(state.y / CAPSENSE_Y_MAX, 1.0))
        if CAPSENSE_INVERT_X:
            x_norm = 1.0 - x_norm
        a = CAPSENSE_EMA_ALPHA
        self._capsense_x_smooth += a * (x_norm - self._capsense_x_smooth)
        self._capsense_y_smooth += a * (y_norm - self._capsense_y_smooth)

        # Don't fight a manual mouse-drag on a slider, and don't apply new
        # values while latched (frozen on purpose).
        if not self._dragging and not self.capsense_latched:
            self._apply_capsense_mapping(self._capsense_x_smooth, self._capsense_y_smooth)

        self.draw_capsense_pad()

    def _on_capsense_button0(self):
        self.capsense_latched = not self.capsense_latched
        self.capsense_latch_label.configure(
            text=f"Latch: {'on' if self.capsense_latched else 'off'}",
            fg=ACCENT if self.capsense_latched else MUTED,
        )
        self.log(f"[CAPSENSE] Latch {'on' if self.capsense_latched else 'off'}")

    def _on_capsense_button1(self):
        self.on_seq_toggle()

    def _apply_capsense_mapping(self, x, y):
        if self.mode_var.get() == "ADDITIVE":
            self._apply_harmonic_formant(x, y)
        else:
            # X -> LFO depth (0-100%), Y -> LFO frequency (0-LFO_FREQ_MAX_HZ).
            # Note: the LFO's destination (Amplitude/None) is set separately
            # in the LFO tab -- it needs to be set to Amplitude for this to
            # be audible, since X/Y here only touch depth and frequency.
            self._apply_lfo_depth(x * 100)
            self._apply_lfo_freq(y * LFO_FREQ_MAX_HZ)

    def _apply_harmonic_formant(self, x, y):
        # X sweeps a resonant "formant" peak across the 14 harmonics, like a
        # wah/filter sweep. Y controls how narrow that peak is: low Y gives a
        # gentle, broad brightness shift; high Y makes a single harmonic
        # dominate almost completely -- a far bigger audible and visible
        # swing than a flat linear tilt.
        center = 1 + x * (MAX_HARMONICS - 1)  # harmonic number 1..MAX_HARMONICS
        sigma = CAPSENSE_FORMANT_SIGMA_MAX - y * (CAPSENSE_FORMANT_SIGMA_MAX - CAPSENSE_FORMANT_SIGMA_MIN)

        weights = []
        for i in range(MAX_HARMONICS):
            n = i + 1
            distance = n - center
            weights.append(math.exp(-(distance ** 2) / (2 * sigma ** 2)))

        # Weights are distributed proportionally out of the fixed 4095
        # harmonic budget (int() truncation only, so the sum can never
        # exceed 4095 -- consistent with the manual-slider clamp).
        total_weight = sum(weights) or 1.0

        for i, w in enumerate(weights):
            amplitude = int(w / total_weight * 4095)
            self.harmonic_vars[i].set(amplitude)
            self.queue_packet(MODULE_HARM, self.voice_var.get(), HARM_AMPLITUDE, i, amplitude)

        self.draw_spectrum()
        self.draw_waveform()
        self._update_harmonics_budget_label()

    # =====================================================
    # LOG
    # =====================================================
    def build_log(self, parent):
        frame = ttk.LabelFrame(parent, text="UART Monitor", padding=6)
        frame.pack(fill="x", pady=(10, 0))

        self.log_text = tk.Text(
            frame, height=6, bg="#0d1117", fg=TEXT,
            insertbackground=TEXT, state="disabled", wrap="word"
        )
        self.log_text.pack(fill="both", expand=True)

    # =====================================================
    # DRAWING
    # =====================================================
    def draw_spectrum(self):
        c = self.spectrum_canvas
        c.delete("all")

        w = max(c.winfo_width(), 40)
        h = max(c.winfo_height(), 40)
        margin = 18

        c.create_line(margin, h - margin, w - margin, h - margin, fill=GRID)

        bar_w = (w - 2 * margin) / MAX_HARMONICS

        for i, var in enumerate(self.harmonic_vars):
            value = var.get() / 4095.0
            x0 = margin + i * bar_w + 3
            x1 = margin + (i + 1) * bar_w - 3
            y0 = h - margin
            y1 = y0 - value * (h - 2 * margin)
            c.create_rectangle(x0, y1, x1, y0, fill=ACCENT, outline="")

    def draw_waveform(self):
        c = self.wave_canvas
        c.delete("all")

        w = max(c.winfo_width(), 100)
        h = max(c.winfo_height(), 100)
        mid = h / 2

        c.create_line(10, mid, w - 10, mid, fill=GRID)

        raw_y = []

        for px in range(10, w - 10):
            t = (px - 10) / max(1, (w - 20))

            if self.mode_var.get() == "ADDITIVE":
                y = 0.0
                for n, var in enumerate(self.harmonic_vars, start=1):
                    amp = var.get() / 4095.0
                    y += amp * math.sin(2 * math.pi * n * t)
            else:
                wf = self.waveform_var.get()
                if wf == "SINE":
                    y = math.sin(2 * math.pi * t)
                elif wf == "SAW":
                    y = 2 * (t - math.floor(t + 0.5))
                else:
                    y = 1 if math.sin(2 * math.pi * t) >= 0 else -1

            raw_y.append(y)

        # Auto-scale so summed harmonics never clip against the canvas edges —
        # peak amplitude always fills ~90% of the available height.
        peak = max((abs(v) for v in raw_y), default=1.0)
        if peak < 1e-6:
            peak = 1.0
        amplitude_px = h * 0.45

        points = []
        for px, y in zip(range(10, w - 10), raw_y):
            py = mid - (y / peak) * amplitude_px * 0.9
            points.extend([px, py])

        if len(points) >= 4:
            c.create_line(*points, fill=ACCENT, width=2, joinstyle="round")

    # =====================================================
    # EVENTS
    # =====================================================
    def on_mode_changed(self, _event=None):
        self._apply_mode_ui()
        mode = MODE_ADDITIVE if self.mode_var.get() == "ADDITIVE" else MODE_WAVETABLE
        self.queue_packet(MODULE_ENGINE, 0xFF, ENGINE_MODE, 0, mode)

    def _apply_mode_ui(self):
        """Pure UI sync for the current mode_var -- frame visibility, voice
        selector availability, and reloading that voice's stored state.
        No packet is sent here (used both by on_mode_changed and at
        startup, where we don't want to fire anything before the user
        acts)."""
        if self.mode_var.get() == "ADDITIVE":
            self.wavetable_frame.pack_forget()
            self.additive_frame.pack(fill="both", expand=True)
        else:
            self.additive_frame.pack_forget()
            self.wavetable_frame.pack(fill="both", expand=True)

        self.sync_voice_availability()  # may reset voice_var to 0 for additive

        key = self._current_voice_key()
        self._load_adsr_state(key)
        self._load_seq_state(key)

        if self.mode_var.get() == "WAVETABLE":
            voice = self.voice_var.get()
            self._load_waveform_state(voice)
            volume = self.voice_volumes.get(voice, 0)
            self.volume_var.set(volume)
            self.volume_value_label.configure(text=self._fmt_value(volume))

        self.draw_waveform()

    def sync_voice_availability(self):
        # Additive mode only ever addresses a single internal voice, so lock
        # the selector to 0 and disable it; wavetable mode allows free choice.
        if self.mode_var.get() == "ADDITIVE":
            if self.voice_var.get() != 0:
                self.voice_var.set(0)
            self.voice_box.configure(state="disabled")
            self.voice_hint.configure(text="(single voice in additive mode)")
            self.active_voices_spin.configure(state="disabled")
            self.active_voices_btn.configure(state="disabled")
        else:
            self.voice_box.configure(state="readonly")
            self.voice_hint.configure(text="")
            self.active_voices_spin.configure(state="normal")
            self.active_voices_btn.configure(state="normal")

    def on_set_active_voices(self):
        # ENGINE_ACTIVE_VOICES triggers a synth_stop()/synth_start() on the
        # firmware side, so this is a deliberate "commit" action (button)
        # rather than something streamed continuously like a slider.
        count = max(1, min(int(self.active_voices_var.get() or 1), MAX_WAVETABLE_VOICES))
        self.active_voices_var.set(count)
        self.queue_packet(MODULE_ENGINE, 0xFF, ENGINE_ACTIVE_VOICES, 0, count)
        self.log(f"[ENGINE] Active voices set to {count}")

    def on_voice_changed(self, _event=None):
        # Only fires in wavetable mode (the selector is disabled in additive),
        # so voice is always a genuine wavetable voice id here.
        voice = self.voice_var.get()

        volume = self.voice_volumes.get(voice, 0)
        self.volume_var.set(volume)
        self.volume_value_label.configure(text=self._fmt_value(volume))

        self._load_waveform_state(voice)
        self._load_adsr_state(voice)
        self._load_seq_state(voice)
        self.draw_waveform()

        self.log(f"Voice {voice} selected")

    def on_waveform_changed(self, _event=None):
        waves = {"SINE": WAVE_SINE, "SAW": WAVE_SAW, "SQUARE": WAVE_SQUARE}
        self.queue_packet(
            MODULE_OSC, self.voice_var.get(), OSC_WAVEFORM, 0, waves[self.waveform_var.get()]
        )
        self.draw_waveform()
        self.voice_waveform[self.voice_var.get()] = self.waveform_var.get()

    def _load_waveform_state(self, voice):
        self.waveform_var.set(self.voice_waveform.get(voice, "SINE"))

    def on_volume_changed(self, value):
        self._apply_voice_volume(float(value))

    def _apply_voice_volume(self, requested):
        requested = max(0, min(int(requested), 4095))
        voice = self.voice_var.get()

        # Volumes across all voices together may not exceed 4095 -- clamp
        # this voice's slider to whatever budget the other voices haven't used.
        others_sum = sum(v for vid, v in self.voice_volumes.items() if vid != voice)
        clamped = max(0, min(requested, 4095 - others_sum))

        self.voice_volumes[voice] = clamped
        self.volume_var.set(clamped)
        self.volume_value_label.configure(text=self._fmt_value(clamped))
        self.queue_packet(MODULE_OSC, voice, OSC_VOLUME, 0, clamped)
        self._update_voice_volume_budget_label()
        return clamped

    def _update_voice_volume_budget_label(self):
        total = sum(self.voice_volumes.values())
        self.voice_volume_budget_label.configure(
            text=f"All voices: {total} / 4095",
            fg=WARN if total >= 4095 else MUTED,
        )

    def on_harmonic_changed(self, index, value):
        requested = int(float(value))

        # All harmonic amplitudes together may not exceed 4095 -- clamp this
        # slider to whatever budget the other 13 haven't already used.
        others_sum = sum(v.get() for i, v in enumerate(self.harmonic_vars) if i != index)
        clamped = max(0, min(requested, 4095 - others_sum))

        self.harmonic_vars[index].set(clamped)
        self.queue_packet(MODULE_HARM, self.voice_var.get(), HARM_AMPLITUDE, index, clamped)
        self.draw_spectrum()
        self.draw_waveform()
        self._update_harmonics_budget_label()

    def _update_harmonics_budget_label(self):
        total = sum(v.get() for v in self.harmonic_vars)
        self.harmonics_budget_label.configure(
            text=f"Sum: {total} / 4095",
            fg=WARN if total >= 4095 else MUTED,
        )

    # =====================================================
    # UART
    # =====================================================
    def refresh_ports(self):
        if serial is None:
            return

        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_combo["values"] = ports

        if ports and self.port_var.get() not in ports:
            self.port_var.set(ports[0])

    def toggle_connection(self):
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
            self.serial_port = None
            self.status_var.set("Disconnected")
            self.status_label.configure(fg=WARN)
            self.toggle_conn_btn.configure(text="Connect")
            self.log("[DISCONNECTED]")
            return

        if serial is None:
            messagebox.showerror("PySerial", "Install pyserial first")
            return

        port = self.port_var.get().strip()
        if not port:
            return

        try:
            self.serial_port = serial.Serial(port, DEFAULT_BAUD, timeout=0)
            self.status_var.set(f"Connected: {port}")
            self.status_label.configure(fg=SUCCESS)
            self.toggle_conn_btn.configure(text="Disconnect")
            self.log(f"[CONNECTED] {port}")
        except Exception as exc:
            messagebox.showerror("UART", str(exc))

    # =====================================================
    # PACKETS
    # =====================================================
    def queue_packet(self, module, voice, parameter, index, value):
        """
        Record the latest desired value for a given (module, voice, parameter,
        index) address. Rapid-fire UI events (slider drags, etc.) overwrite
        the same pending entry rather than each generating a transmission;
        _flush_pending sends only what's actually changed, at most once
        per CONTROL_SEND_INTERVAL_MS.
        """
        key = (module, voice, parameter, index)
        self._pending_packets[key] = int(value)

    def _flush_pending(self):
        self.poll_capsense()
        self._send_pending_now()
        self.root.after(CONTROL_SEND_INTERVAL_MS, self._flush_pending)

    def _send_pending_now(self):
        # While a slider is being actively dragged, values keep piling up in
        # _pending_packets (coalesced, latest wins) but nothing is actually
        # transmitted -- on_drag_end flushes the final value immediately once
        # the mouse button is released.
        if self._dragging:
            return

        if self._pending_packets:
            batch, self._pending_packets = self._pending_packets, {}
            for key, value in batch.items():
                if self._last_sent.get(key) != value:
                    module, voice, parameter, index = key
                    self.send_packet(module, voice, parameter, index, value)
                    self._last_sent[key] = value

    def on_drag_start(self, _event=None):
        self._dragging = True

    def on_drag_end(self, _event=None):
        self._dragging = False
        self._send_pending_now()

    def _bind_drag_gating(self, widget):
        """Wire a Scale widget so packets are queued (not sent) while it's
        being dragged, and flushed immediately the moment it's released."""
        widget.bind("<ButtonPress-1>", self.on_drag_start)
        widget.bind("<ButtonRelease-1>", self.on_drag_end)

    def send_packet(self, module, voice, parameter, index, value):
        packet = make_packet(module, voice, parameter, index, value)
        hex_packet = packet.hex(" ").upper()

        self.log(f"TX  {hex_packet}")
        print(hex_packet)

        if self.serial_port and self.serial_port.is_open:
            self.serial_port.write(packet)

    # =====================================================
    # LOG
    # =====================================================
    def log(self, text: str):
        self.log_text.configure(state="normal")
        self.log_text.insert("end", text + "\n")
        self.log_text.see("end")
        self.log_text.configure(state="disabled")


def main():
    root = tk.Tk()
    app = SynthControlApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
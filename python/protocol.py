from __future__ import annotations

PACKET_SIZE = 7
HEADER = 0xAA


def make_packet(module, voice, parameter, index, value):
    """
    Pack a single synth command into the 7-byte wire format:

    +--------+--------+----------+-----------+---------+---------+---------+
    | HEADER | MODULE | VOICE_ID | PARAMETER | INDEX   | VALUE_H | VALUE_L |
    | 1 byte | 1 byte | 1 byte   | 1 byte    | 1 byte  | 1 byte  | 1 byte  |
    +--------+--------+----------+-----------+---------+---------+---------+

    HEADER = 0xAA (fixed sync byte)
    VALUE is transmitted MSB first: value = (VALUE_H << 8) | VALUE_L
    """
    value = max(0, min(int(value), 0xFFFF))

    packet = bytearray(PACKET_SIZE)
    packet[0] = HEADER
    packet[1] = module & 0xFF
    packet[2] = voice & 0xFF
    packet[3] = parameter & 0xFF
    packet[4] = index & 0xFF
    packet[5] = (value >> 8) & 0xFF
    packet[6] = value & 0xFF
    return packet


def decode_packet(packet):
    if len(packet) != PACKET_SIZE or packet[0] != HEADER:
        return None

    return {
        "module": packet[1],
        "voice": packet[2],
        "parameter": packet[3],
        "index": packet[4],
        "value": (packet[5] << 8) | packet[6],
    }
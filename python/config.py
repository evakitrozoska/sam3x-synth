# =====================================================
# Synth UI configuration
# Matches SAM3X8E synth communication protocol
# =====================================================

# =====================================================
# MODULES
# =====================================================
MODULE_ENGINE = 0x01
MODULE_OSC    = 0x02
MODULE_ADSR   = 0x03
MODULE_SEQ    = 0x04
MODULE_LFO    = 0x05
MODULE_HARM   = 0x06

# =====================================================
# ENGINE PARAMETERS
# =====================================================
ENGINE_MODE          = 0x01
ENGINE_ACTIVE_VOICES = 0x02
ENGINE_STATE         = 0x03

MODE_WAVETABLE = 0x00
MODE_ADDITIVE  = 0x01

# =====================================================
# OSCILLATOR PARAMETERS
# =====================================================
OSC_VOLUME   = 0x01
OSC_WAVEFORM = 0x02

WAVE_SINE   = 0x00
WAVE_SAW    = 0x01
WAVE_SQUARE = 0x02

# =====================================================
# ADSR PARAMETERS
# =====================================================
ADSR_ATTACK  = 0x01
ADSR_DECAY   = 0x02
ADSR_SUSTAIN = 0x03
ADSR_RELEASE = 0x04

# Attack/decay/release are sent as raw 0-100 percent values (firmware-side change).
# Sustain is different: it's still a linear 0-65535 amplitude on the wire, but the
# UI slider is 0-100% with a perceptual (dB) taper -- see percent_to_sustain_raw
# in main.py. ADSR_TIME_MAX_MS is used only to size the on-screen envelope
# diagram; it is never transmitted.
ADSR_TIME_MAX_MS      = 4000
ADSR_SUSTAIN_RAW_MAX  = 0xFFFF
ADSR_SUSTAIN_TAPER_DB = 40   # dB range represented by the 0-100% sustain slider

# =====================================================
# HARMONIC PARAMETERS
# =====================================================
HARM_COUNT     = 0x01
HARM_AMPLITUDE = 0x02

# =====================================================
# SEQUENCER PARAMETERS
# =====================================================
SEQ_STATE     = 0x01
SEQ_STEPS     = 0x02
SEQ_STEP_TIME = 0x03
SEQ_NOTE      = 0x04

SEQ_STOP  = 0x00
SEQ_START = 0x01

MAX_SEQUENCE_STEPS = 70     # matches firmware MAX_STEPS
SEQ_STEP_TIME_MAX_MS = 10000
SEQ_NOTE_MAX_HZ = 4000

# =====================================================
# LFO PARAMETERS
# =====================================================
LFO_DESTINATION = 0x01
LFO_FREQ        = 0x02
LFO_DEPTH       = 0x03
LFO_WAVEFORM    = 0x04

# LFO destination values (only AMPLITUDE is documented in the protocol;
# NONE=0x00 is assumed as the "off" convention used elsewhere)
LFO_DEST_NONE      = 0x00
LFO_DEST_AMPLITUDE = 0x01

LFO_FREQ_MAX_HZ = 20
LFO_DEPTH_MAX   = 0xFFFF   # 16-bit depth, UI shows this as 0-100%

# =====================================================
# SYNTH LIMITS
# =====================================================
MAX_WAVETABLE_VOICES = 4
MAX_HARMONICS = 14

# =====================================================
# AUDIO INFO
# =====================================================
SAMPLE_RATE = 42000
BUFFER_SIZE = 128

# =====================================================
# UART
# =====================================================
DEFAULT_BAUD = 115200

# =====================================================
# CONTROL SEND RATE
# =====================================================
# Continuous controls (sliders etc.) are coalesced and flushed on a fixed
# heartbeat rather than sent on every UI event. 10ms (100Hz) is fast enough
# to feel instantaneous to a human, while giving the MCU/UART link headroom
# when several parameters (e.g. multiple harmonic sliders) change at once.
CONTROL_SEND_INTERVAL_MS = 10

# =====================================================
# CAPSENSE PERFORMANCE CONTROLLER
# =====================================================
CAPSENSE_DEFAULT_PORT = "COM7"
CAPSENSE_BAUD = 115200   # assumption -- adjust to match your capsense firmware's Serial.begin()

# Raw XY range reported by the board.
CAPSENSE_X_MAX = 255
CAPSENSE_Y_MAX = 255

CAPSENSE_EMA_ALPHA = 0.3          # smoothing for the continuous XY stream
CAPSENSE_BUTTON_TIMEOUT_S = 0.15  # no explicit "released" message -> infer release after this gap

# The board's raw X axis is mirrored relative to the UI's left/right
# convention
CAPSENSE_INVERT_X = True

# Additive-mode X/Y mapping: X sweeps a resonant "formant" peak across the
# harmonics, Y controls how narrow/dramatic that peak is (in harmonic-index
# units -- think of it as the Gaussian's sigma). Wide sigma = gentle, broad
# brightness shift; narrow sigma = one harmonic dominates almost completely.
CAPSENSE_FORMANT_SIGMA_MIN = 0.6
CAPSENSE_FORMANT_SIGMA_MAX = 6.0

# =====================================================
# UI
# =====================================================
APP_TITLE = "Synth Control v4"
WINDOW_WIDTH = 1320
WINDOW_HEIGHT = 860

BG = "#14171c"
PANEL = "#1c2128"
TEXT = "#e6edf3"
MUTED = "#9da7b3"
ACCENT = "#4ea1ff"
GRID = "#343b45"
WARN = "#ff6b6b"
SUCCESS = "#59d185"
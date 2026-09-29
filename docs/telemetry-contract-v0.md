# Telemetry contract v0 (P01 probe)

Source: `config/flightgear/f16_p01_probe.xml`, copied from the P01 probe. Output is one newline-terminated ASCII record with five TAB-separated decimal fields. No header, checksum, sequence number, or schema version is present on the wire; v0 is the repository-side contract identifier. Parsing and malformed-packet handling belong to P02.

| Position | Field | FlightGear property | Unit | Meaning/class |
|---:|---|---|---|---|
| 1 | `elapsed_sec` | `/sim/time/elapsed-sec` | s | simulation clock |
| 2 | `altitude_msl_ft` | `/position/altitude-ft` | ft MSL | simulated state (`truth`), not an altimeter measurement |
| 3 | `airspeed_kt` | `/velocities/airspeed-kt` | knot | simulated state (`truth`), not a sensor measurement |
| 4 | `heading_deg` | `/orientation/heading-deg` | degrees | simulated orientation (`truth`) |
| 5 | `throttle_command` | `/controls/engines/engine/throttle` | dimensionless | control command, not thrust |

The XML `float` setting describes serialization, not a proven native property type. A simulated control-surface position is a separate `surface` class; no such field is in v0. A sensor `measurement` is also absent. Future schemas must preserve these distinctions.

Use simulation time for algorithms. Host receive time may be recorded separately for transport diagnostics. Paused simulation may repeat timestamps. Treat the configured FlightGear output rate as a setting rather than a measured packet rate: P01 observed about 10 distinct simulation timestamps/s for a setting of 5, and about 20 packets/s for a setting of 10, with no proven cause. Do not infer packet loss from a short capture.

P02 will define conversion at its ingress boundary: `ft * 0.3048` to m, `knot * (1852/3600)` to m/s, and `degrees * (pi/180)` to rad. Retain raw values or explicit unit suffixes in any recording to avoid silent mixing.

# Environment acceptance evidence — 2026-09-29

This records user-reported output from the Windows 10 Pro host. It is an environment baseline, not a P02 telemetry implementation or an automated continuous integration result.

| Component | Observed version/path | State and evidence |
|---|---|---|
| Visual Studio / MSVC / SDK | Community 2026 18.10.12217.157; MSVC 19.51.36260 x64; SDK `10.0.26100.0` directory | Developer PowerShell resolved `cl.exe`; both CMake builds identified the compiler and succeeded. SDK directory was enumerated, but a separate SDK-only test was not run. |
| CMake / Ninja | CMake 4.3.1-msvc1, Ninja bundled with VS | `cmake --preset debug/release` and `cmake --build --preset debug/release` succeeded; both executables printed the baseline message. |
| Git repository | `D:\GitHub\f16-flight-health-testbench`, `main` | Initial commit `4723202` created; `git status --short --branch` showed clean `## main`. GitHub remote is pending. |
| Conda / Python | Conda 25.11.1; `f16-health` at `C:\ProgramData\anaconda3\envs\f16-health`; Python 3.12.14; pip 26.2.1 | Environment created from `environment.yml`; interpreter and pip paths showed the project env. VS Code integrated PowerShell worked after manual activation; selected interpreter still requires explicit confirmation. |
| FlightGear / addon | FlightGear 2024.1.7; installed `f16` addon in FGAddon stable_2024 Aircraft downloads | Addon `f16b52.xml` SHA-256 `E41F00D80A785FF63774A39CC8BB131D67AD42A2A4B72BEA583EEEF1DD0C9538`; no addon `.git` directory. Local README says 3.83, which is not a verified commit. Local LICENSE begins `GNU GENERAL PUBLIC LICENSE Version 2, June 1991`; individual file licensing/attribution still merits review before derived aircraft work. |
| Protocol copy | Repository `config/flightgear/f16_p01_probe.xml` and active `fgdata_2024_1\Protocol` copy | Both SHA-256 values `C31EE528A1731FB9A53E21563DAF7888EFA52D48CA6DB55AE9AAE355B9FB2E13`. Five TAB-separated ASCII fields documented in [contract](telemetry-contract-v0.md). |
| UDP / time | 127.0.0.1:5519, setting 5, one channel in fresh FlightGear process | Nasal `add-io-channel` returned `1`; five initial records had sim time 0. After simulation advanced, 20 packets / 3.84 wall s, 20 distinct timestamps from 34.958332 to 38.766666 s. This short sample does not establish fixed cadence or packet loss. |
| Data and ports | Build and `.vscode` are ignored; 5519 had no listener before trial | Original large logs are excluded. No raw flight logs committed. Local-only port and single-channel procedure documented. |

Open gates: GitHub remote/publication; selected VS Code interpreter confirmation; exact addon revision beyond local file hash; longer-run timing/packet handling in P02. The original repository's PolyForm Noncommercial 1.0.0 license and public visibility choice are recorded, while any later distribution of modified aircraft files needs separate licensing and attribution review. Physical fault fidelity and effect remain a separate P04 gate.

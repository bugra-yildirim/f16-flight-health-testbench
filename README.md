# F-16 Flight & Health Testbench

**Status: environment baseline complete locally; live C++ telemetry begins in P02.**

A staged C++17 engineering portfolio project that uses FlightGear's F-16CJ Block 52 as a repeatable simulation source for flight telemetry, controlled in-model faults, and health-monitoring experiments. The current executable is a build check. It does **not** receive UDP, diagnose faults, or display a dashboard yet.

## System boundary

```text
FlightGear F-16 (built-in JSBSim) -> ASCII UDP -> C++ telemetry and recording
                                             -> monitoring and estimation (later)
                                             -> live status panel (final product)
```

FlightGear owns the flight physics in live mode. A later replay mode will pass recorded telemetry through the same algorithms; it will not rerun flight physics. Independent headless JSBSim use is a separate future investigation, not an equivalent F-16 mode today.

## What works now

| Area | Verified result on the Windows host |
|---|---|
| Build | Debug and Release configure, compile, and run with Visual Studio 2026 / MSVC 19.51, bundled CMake and Ninja. |
| Python | Dedicated Conda `f16-health` environment, Python 3.12.14; VS Code integrated PowerShell used it after activation. |
| FlightGear | FlightGear 2024.1.7 and installed FGAddon F-16CJ Block 52; the repository protocol XML matches the installed copy by SHA-256. |
| UDP probe | A clean session returned 20 records in 3.84 wall-clock seconds, with 20 distinct advancing simulation timestamps. This was a PowerShell probe, **not** the planned C++ receiver. |

See [environment acceptance](docs/environment-acceptance.md) for paths, hashes, evidence, and remaining checks. The observed output cadence differed between P01 and the environment trial; it is not treated as a fixed sampling guarantee.

## Build on Windows

Use **Developer PowerShell for VS 2026** from the repository root:

```powershell
cmake --preset debug
cmake --build --preset debug
& .\build\debug\f16-baseline.exe

cmake --preset release
cmake --build --preset release
& .\build\release\f16-baseline.exe
```

Expected message: `F-16 testbench environment baseline ready`. This confirms the C++ toolchain only. The [Windows setup guide](docs/setup-windows.md) covers Developer PowerShell, Conda, VS Code, and the FlightGear protocol check. Docker and WSL are not required for the current Windows GUI/joystick/UDP route.

## Roadmap

| Phase | Deliverable | Current state |
|---|---|---|
| P01 | FlightGear/F-16 feasibility and UDP property probe | Completed with documented limits |
| P02 | C++ UDP receiver, validation, simulation time, console and CSV recording | Next |
| P03 | Plots and algorithmic log replay | Planned |
| P04 | Controlled fault that measurably affects the simulated aircraft | Planned; physics effect must be demonstrated |
| P05 | Rule-based health monitoring | Planned |
| P06 | Sensor/state estimation | Planned |
| P07 | ML/ONNX only if a baseline comparison supports it | Conditional |
| Final | Live status, alarms, and comparison panel | Planned |

The [v0 telemetry contract](docs/telemetry-contract-v0.md) names the five probe fields and distinguishes simulated state (`truth`), commands, surface positions, and future sensor measurements. P02 will define parsing, unit conversions, and malformed-packet handling. Large raw logs, build output, and local environment settings are excluded from Git.

## Reproducibility and limits

The installed aircraft is an external FlightGear dependency, not bundled here. Its exact installed commit is unknown; local file hashes identify the observed copies. The model combines left/right surface effects in parts of its aerodynamics. No physical fault has yet been applied or measured, and no claim is made about fidelity to a real F-16's single-surface damage, real sensor measurements, certified diagnosis, or engine remaining useful life. Results generated later will be simulation results with their conditions and versions recorded.

## License and attribution

Original files in this repository are licensed under the [PolyForm Noncommercial License 1.0.0](LICENSE), with a required copyright notice for 2026 Halit Buğra Yıldırım. The repository is public and source-available; this license restricts permitted use to the purposes defined in its terms. Commercial use requires a separate written license from the copyright holder; contact [halit.bugra.yildirim@gmail.com](mailto:halit.bugra.yildirim@gmail.com). FlightGear and the separately installed F-16 addon retain **their own licenses**; the local addon contains GNU GPL v2 license text and is not redistributed here. This repository license does not relicense the addon. If aircraft XML/assets or modified addon files are ever added, their licensing and attribution must be handled separately before publication. See [third-party notes](docs/third-party.md).

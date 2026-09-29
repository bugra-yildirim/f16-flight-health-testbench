# Windows environment setup and verification

## Observed starting point (user machine, 2026-09-29)

Windows 10 Pro x64; Visual Studio Community 2026 (18.10.12217.157); MSVC 19.51.36260 x64; bundled CMake 4.3.1-msvc1 and Ninja; Windows SDK directory 10.0.26100.0; Git 2.54.0.windows.1; Conda 25.11.1; VS Code 1.138.0; FlightGear 2024.1.7. Repository: `D:\GitHub\f16-flight-health-testbench`, branch `main`. The Debug and Release build commands below ran successfully on the user machine on 2026-09-29; the first commit is `4723202`. See [environment acceptance](environment-acceptance.md) for evidence and outstanding checks.

## Build in Developer PowerShell for VS 2026

Open the Visual Studio Developer PowerShell, or in an existing PowerShell session run the locally verified shell launcher:

```powershell
$vs = 'C:\Program Files\Microsoft Visual Studio\18\Community'
& (Join-Path $vs 'Common7\Tools\Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64
Set-Location 'D:\GitHub\f16-flight-health-testbench'
cmake --preset debug
cmake --build --preset debug
& .\build\debug\f16-baseline.exe
cmake --preset release
cmake --build --preset release
& .\build\release\f16-baseline.exe
```

Both presets configured and built using MSVC 19.51.36260; each executable printed `F-16 testbench environment baseline ready`. The executable only confirms the C++ toolchain. Build directories are ignored by Git. `CMakePresets.json` contains no machine-specific Visual Studio path; a local-only `CMakeUserPresets.json` can be added if needed and should stay untracked.

## Python environment and VS Code

From the repository directory in a Conda-enabled PowerShell:

```powershell
conda env create -f environment.yml
conda activate f16-health
python --version
python -c "import sys; print(sys.executable)"
python -m pip --version
code .
```

The created environment gave Python 3.12.14 and pip 26.2.1 at `C:\ProgramData\anaconda3\envs\f16-health\python.exe`. In VS Code, use **Python: Select Interpreter** and select `f16-health`; in the integrated terminal, repeat the checks above. The VS Code PowerShell terminal ran the correct Python after manual `conda activate f16-health`, and the Select Interpreter screenshot confirmed f16-health (3.12.14) on 2026-09-30. The `analysis/` directory and plotting dependencies will be added when P03 needs them. The environment file has no machine-specific prefix.

## FlightGear protocol

The application binaries are at `C:\Program Files\FlightGear 2024.1\bin`; the active data copy was found under `C:\Users\Halit\FlightGear\Downloads\fgdata_2024_1\Protocol\f16_p01_probe.xml`. Its SHA-256 equals the repository copy: `C31EE528A1731FB9A53E21563DAF7888EFA52D48CA6DB55AE9AAE355B9FB2E13`. The addon is under `C:\Users\Halit\FlightGear\Downloads\Aircraft\org.flightgear.fgaddon.stable_2024\Aircraft\f16`. Do not edit the installed aircraft model during this environment stage.

Before opening a local listener, check the chosen UDP port is unused (`Get-NetUDPEndpoint -LocalPort 5519 -ErrorAction SilentlyContinue`). Restart FlightGear and add exactly one output channel in Debug → Nasal Console:

```text
print(fgcommand("add-io-channel", {"config":"generic,socket,out,5,127.0.0.1,5519,udp,f16_p01_probe", "name":"p01_rate5"}));
```

Start a PowerShell receiver before enabling the channel. The following bounded capture prints five raw records and stops after a three-second receive timeout; a timeout means no packet reached this socket during that interval, not a diagnosis of the cause:

```powershell
$u = [System.Net.Sockets.UdpClient]::new(5519)
$u.Client.ReceiveTimeout = 3000
$ep = [System.Net.IPEndPoint]::new([System.Net.IPAddress]::Any, 0)
try {
    1..5 | ForEach-Object {
        $bytes = $u.Receive([ref]$ep)
        [Text.Encoding]::ASCII.GetString($bytes).Trim()
    }
} finally { $u.Dispose() }
```

P01 measured approximately 51 distinct timestamps in 5.11 wall-clock seconds for a setting of 5. A clean 2026-09-29 session received 20 packets in 3.84 wall-clock seconds, with 20 distinct simulation timestamps from 34.958332 to 38.766666 seconds. This is about 5.2 packets/s over a short capture; the difference from P01 is unexplained and should be measured over longer runs in P02. The first five packets after channel activation repeated `0.000000` while simulation time was not advancing. FlightGear's dynamic `remove-io-channel` failed during P01, so restart the application for a clean single-channel trial.

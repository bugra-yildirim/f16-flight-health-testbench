# Windows environment setup and verification

## Observed starting point (user machine, 2026-09-29)

Windows 10 Pro x64; Visual Studio Community 2026 (18.10.12217.157); MSVC 19.51.36260 x64; bundled CMake 4.3.1-msvc1 and Ninja; Windows SDK directory 10.0.26100.0; Git 2.54.0.windows.1; Conda 25.11.1; VS Code 1.138.0; FlightGear 2024.1.7. `D:\GitHub\f16-flight-health-testbench` was initialized as an empty `main` repository. No build, Conda environment, remote, or fresh UDP capture had yet been verified when this file was authored.

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

The build commands are instructions until their outputs are captured on the user machine. The executable only confirms the C++ toolchain. Build directories are ignored by Git. `CMakePresets.json` contains no machine-specific Visual Studio path; a local-only `CMakeUserPresets.json` can be added if needed and should stay untracked.

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

In VS Code, use **Python: Select Interpreter** and select the `f16-health` interpreter. In its integrated terminal, repeat the three Python checks; confirm the executable points to `f16-health` and not `base`. The `analysis/` directory and plotting dependencies will be added when P03 needs them. The environment file has no machine-specific prefix.

## FlightGear protocol

Locate the effective FlightGear data root/`Protocol` directory and installed addon first; do not infer it solely from the application install directory. Copy `config/flightgear/f16_p01_probe.xml` into the effective `Protocol` directory as an installation copy, then compare `Get-FileHash` (SHA256) for both paths. Do not edit the installed aircraft model during this environment stage.

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

P01 measured approximately 51 distinct timestamps in 5.11 wall-clock seconds for a setting of 5, but the cause of the roughly doubled observed rate is unknown. A new capture and the effective data root remain pending in this environment stage. FlightGear's dynamic `remove-io-channel` failed during P01, so restart the application for a clean single-channel trial.

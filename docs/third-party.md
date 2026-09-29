# Third-party software and aircraft assets

This repository includes **no** FlightGear binaries, FGAddon aircraft source, textures, sounds, or modified aircraft files. The `config/flightgear/f16_p01_probe.xml` probe was created for this project and contains only five FlightGear property paths and output formatting.

| External component | Local evidence | Relationship to this repository |
|---|---|---|
| FlightGear 2024.1.7, including its built-in JSBSim integration | Installed program at `C:\Program Files\FlightGear 2024.1\` | Separately installed simulator; no binary vendored. Consult the component's own license and notices for redistribution. |
| FGAddon F-16CJ Block 52 | Installed under `C:\Users\Halit\FlightGear\Downloads\Aircraft\org.flightgear.fgaddon.stable_2024\Aircraft\f16`; `f16b52.xml` SHA-256 `E41F00D80A785FF63774A39CC8BB131D67AD42A2A4B72BEA583EEEF1DD0C9538` | Separately installed addon. Its local `LICENSE` contains GNU GPL v2 text; `README.md` names version 3.83 but no exact installed commit was verified. Consult its `authors.txt` and individual file notices before distributing modified assets. |

The PolyForm Noncommercial 1.0.0 license at the repository root covers the original repository files; it does not change the addon license. Any future aircraft modification must preserve applicable notices and be assessed as its own distribution. Until then, keep model changes out of this repository and do not present this code as an official FlightGear or aircraft-addon product.

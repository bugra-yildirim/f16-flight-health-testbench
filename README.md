# F-16 Flight & Health Testbench

An experimental C++17 testbench for FlightGear's F-16CJ Block 52. The first live mode will receive FlightGear UDP telemetry. FlightGear's built-in JSBSim remains the sole flight physics engine. A later replay mode will run algorithms on recorded telemetry, not rerun flight physics.

The planned stages are live recording (P02), plots and replay (P03), a measured in-model control fault (P04), rule-based monitoring (P05), state/sensor estimation (P06), and ML/ONNX only if it demonstrates value (P07). A live status and alarm panel is planned for the final product. This repository currently contains only the environment baseline; `f16-baseline` is a build smoke target, not a telemetry receiver.

No real F-16 sensor accuracy, certified diagnosis, or engine remaining-useful-life claim is made. The installed aircraft addon is an external dependency and is not included in this repository.

See [Windows setup](docs/setup-windows.md), [telemetry contract](docs/telemetry-contract-v0.md), and [decisions](docs/decisions.md). The repository license and public visibility are pending an explicit publication decision; no reuse permission is implied by this repository's current lack of a license.

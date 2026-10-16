# NLM Utility Script

This script provides useful command-line tools for working with NLM (Neural Learning Machine) simulations.

## Overview

The NLM utility script is a collection of bash functions that help with:

- Running simple simulations
- Running Phase 6 integration demos
- Running memory system tests
- Building the project
- Checking system status
- Running all tests

## Installation

1. Make the script executable:
   ```bash
   chmod +x nlm-utils.sh
   ```

2. The script is designed to work from the project root directory.

## Commands

### `help`

Shows this help message.

### `status`

Shows the current system status:
- Build status (NLM executable, Phase 6 demo)
- Python bindings availability
- Configuration file status
- Documentation availability

### `simple <steps> <neurons>`

Run a simple simulation with specified parameters:
- `<steps>`: Number of simulation steps (e.g., 100)
- `<neurons>`: Number of neurons (e.g., 1000)

Example:
```bash
./nlm-utils.sh simple 100 1000
```

### `phase6`

Run the Phase 6 integration demo:
```bash
./nlm-utils.sh phase6
```

### `memory`

Run memory system tests:
```bash
./nlm-utils.sh memory
```

### `all`

Run all tests (Phase 6 demo + memory tests):
```bash
./nlm-utils.sh all
```

## Examples

### Quick Test

```bash
# Run a simple status check
./nlm-utils.sh status

# Run a simple simulation with 1000 neurons for 100 steps
./nlm-utils.sh simple 100 1000

# Run the complete Phase 6 integration demo
./nlm-utils.sh phase6

# Run all available tests
./nlm-utils.sh all
```

### Building and Running

If you need to build the project manually:

```bash
# Create build directory and configure
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the NLM demo
./nlm

# Run the Phase 6 demo
./nlm_phase6_demo

# Return to project root
cd ..
```

## Features

### Automatic Building

The script automatically handles building:
- Checks if executables are already built
- Builds Phase 6 demo if needed
- Provides clear build instructions if required

### Comprehensive Status Checking

The status command provides:
- Build status verification
- Python bindings availability
- Configuration file validation
- Documentation checks

### Test Suite Support

The script includes a complete test suite:
- Phase 6 integration demo
- Memory system tests
- Both can be run individually or together

### Error Handling

Clear error messages and instructions:
- Missing executables with build instructions
- Configuration file validation
- Compilation failure reporting

## Python Integration

The NLM utility script is designed to work with:
- **NLM Python API**: `pynlm` module for brain simulation
- **Phase 6 Demo**: Comprehensive integration testing
- **Memory Tests**: Memory system validation
- **Configuration**: `configs/default.cfg` for simulation parameters

## Development

### Adding New Commands

To add a new command:

1. Add the command logic in the `case` statement
2. Implement the command function with appropriate help text
3. Update the `show_help()` function
4. Test the new command

### Modifying Existing Commands

To modify an existing command:

1. Edit the corresponding command function
2. Update the help text in `show_help()` if needed
3. Test the changes

### Building and Running Tests

To build and run tests:

```bash
# Build the project
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run tests from the project root
cd ..
./nlm-utils.sh all
```

## Troubleshooting

### Common Issues

**"NLM executable not found"**
- Solution: Run `./nlm-utils.sh phase6` to build the demo, or run `./nlm-utils.sh simple <steps> <neurons>` to trigger a build.

**"Phase 6 demo not built"**
- Solution: Run `./nlm-utils.sh phase6` to build and run the demo.

**"Tests not found"**
- Solution: Ensure you're running the script from the project root directory.

**"Build failed"**
- Solution: Check the CMake and build logs for error messages. Try running `cd build && make -j4` manually for more details.

### Getting Help

```bash
./nlm-utils.sh help
```

## License

This script is part of the NLM project and is licensed under the MIT License.

For more information about NLM, visit the official documentation.

## Author

NLM Development Team

Last updated: Phase 6 integration completion

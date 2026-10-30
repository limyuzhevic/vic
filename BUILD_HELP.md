# NLM Build and Development Scripts

This directory contains standard build and development scripts for the NLM (Neural Learning Machine) project.

## Scripts

### `build.sh`
Standard build script for compiling the C++ extension and creating executables.

**Usage:**
```bash
# Build in release mode (default)
./build.sh

# Build in debug mode
./build.sh --debug

# Clean build directory and rebuild
./build.sh --clean

# Show help
./build.sh --help
```

**Features:**
- Automatically creates build directory
- Supports both Ninja and Make build systems
- Parallel compilation for faster builds
- Debug and release build modes
- Clean option for fresh builds

### `dev_setup.sh`
Development environment setup script for NLM.

**Usage:**
```bash
# Setup development environment (installs Python package and builds C++)
./dev_setup.sh
```

**Features:**
- Sets up Python virtual environment (if python3/pip3 available)
- Installs NLM Python package in development mode
- Builds C++ extension if not already built
- Checks for build dependencies and provides guidance

## Quick Start

1. **Build the project:**
   ```bash
   ./build.sh
   ```

2. **Run the integration test:**
   ```bash
   ./build/nlm_phase6_demo
   ```

3. **Setup development environment:**
   ```bash
   ./dev_setup.sh
   ```

4. **Run Python tests:**
   ```bash
   python -m pytest tests/
   ```

## Notes

- The scripts are designed to work in both Linux and macOS environments
- On Windows, use WSL or Git Bash for the scripts
- The build script supports both Ninja (preferred) and Make build systems
- The dev_setup script provides comprehensive development environment setup

## Additional Commands

For more information about building and running NLM, refer to:
- `README.md` - General project information
- `easy_usage.md` - Beginner guide
- `HOW_TO_USE.md` - Advanced usage

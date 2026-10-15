# CMake Configuration with JSON/YAML Support

This project is configured to support both JSON and YAML configuration files through CMake dependency management.

## Dependencies

The project uses the following dependencies for configuration file parsing:

1. **nlohmann/json** - Header-only JSON library for C++
2. **yaml-cpp** - YAML library for C++

Both dependencies are automatically managed through FetchContent when not found as installed packages.

## CMake Configuration

### Core CMakeLists.txt

The main CMakeLists.txt file includes:

1. **Automatic dependency detection** - First tries to find packages via `find_package`, falls back to FetchContent if not found
2. **JSON support** - Links against nlohmann_json::nlohmann_json target
3. **YAML support** - Links against yaml-cpp target
4. **Standard configuration** - C++20, warning flags, and build type settings

### Python Bindings CMakeLists.txt

The python/CMakeLists.txt includes:

1. **Same dependency management** as the main CMakeLists.txt
2. **Integration with pybind11** for Python bindings
3. **All target libraries** (nlm_core, nlm_agent, nlm_world) linked with JSON/YAML support

### Tests CMakeLists.txt

The tests/CMakeLists.txt includes:

1. **Dependency management** for running tests
2. **Integration with nlm_core** library

## Usage

### Building the Project

```bash
mkdir build
cd build
cmake ..
make
```

### Building Python Bindings

```bash
cd python
mkdir build
cd build
cmake ..
make
```

## Features

The NLM framework now supports:

1. **JSON configuration files** (`.json`) - Modern, human-readable format
2. **YAML configuration files** (`.yaml`, `.yml`) - Flexible, rich data serialization
3. **Simple key=value files** (`.cfg`) - Backward compatibility format

All configuration files can be automatically loaded via `Config::loadFromFile()` and saved via `Config::saveToFile()`.

## Configuration File Examples

### JSON Configuration

```json
{
    "brain": {
        "neurons": 1000,
        "connection_density": 0.1,
        "learning_rate": 0.01
    },
    "environment": {
        "type": "simple",
        "width": 100.0,
        "height": 100.0,
        "wall_thickness": 2.0
    },
    "memory": {
        "working_memory_capacity": 10,
        "episodic_memory_duration": 1000
    }
}
```

### YAML Configuration

```yaml
brain:
  neurons: 1000
  connection_density: 0.1
  learning_rate: 0.01

environment:
  type: simple
  width: 100.0
  height: 100.0
  wall_thickness: 2.0

memory:
  working_memory_capacity: 10
  episodic_memory_duration: 1000
```

## License

MIT License

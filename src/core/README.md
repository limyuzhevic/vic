# Core Module

## Overview

The Core module provides fundamental utilities and infrastructure for the NLM neural simulation framework. It includes configuration management, random number generation, logging systems, and type definitions essential for neural computation.

## Components

### 1. Configuration System (`src/core/Config/`)

The configuration system provides:
- **Key-value configuration** with multiple value types (int, double, string, bool, vectors)
- **Multiple configuration sources**: Default, File, Command Line, Runtime
- **JSON/YAML file support** (in progress)
- **Command line integration** with `--key=value` syntax
- **Runtime configuration** with hot-swapping

**Key Files:**
- `Config.hpp` - Main interface
- `Config.cpp` - Implementation with simple file parser

**Usage Example:**
```cpp
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("random_seed", 42LL);
int neurons = config->getOr("neuron_count", 500);
```

### 2. Random Number Generation (`src/core/Random/`)

Provides:
- **Mersenne Twister RNG** with high-quality randomness
- **Seeded initialization** for reproducible simulations
- **Thread-safe operations**
- **Statistical testing** for randomness quality

**Key Classes:**
- `RandomGenerator` - Core random number generator
- `RandomStream` - Thread-safe wrapper

### 3. Logging System (`src/core/Logger/`)

Features:
- **Multi-level logging**: Debug, Info, Warning, Error, Critical
- **Multiple output sinks**: Console, file, custom implementations
- **Performance optimized** with minimal overhead
- **Thread-safe operations**
- **Color-coded console output**

**Key Classes:**
- `Logger` - Main logging interface
- `ConsoleLogger` - Console output implementation
- `ILogger` - Abstract sink interface

### 4. Type Definitions (`src/core/Types/`)

Provides:
- **Strong type system** for neural identifiers
- **Safe type conversions** with compile-time checks
- **ID type mappings** for neurons, synapses, regions
- **Enum definitions** for neuron types, states, etc.

**Key Types:**
- `NeuronId`, `SynapseId`, `RegionId` - Unique identifiers
- `NeuronType`, `NeuronState` - Enumeration types
- `MembranePotential`, `FiringRate` - Specialized numeric types

### 5. Simulation Clock (`src/core/SimulationClock/`)

Provides:
- **High-precision timing** with configurable timesteps
- **Simulation step tracking**
- **Performance monitoring**
- **Wall-clock vs simulation time coordination**

## Configuration Keys

Common configuration keys used throughout NLM:

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| random_seed | int64_t | 42 | Random number generator seed |
| simulation_timestep | double | 0.001 | Simulation timestep in seconds |
| neuron_count | int64_t | 1000 | Number of neurons in the brain |
| region_count | int64_t | 2 | Number of brain regions |
| connection_probability | float | 0.1 | Probability of random connectivity |

## Configuration File Format

The default configuration file (`configs/default.cfg`) uses:
- Simple `key = value` format
- `#` comments
- Type inference from value format (numbers vs strings)
- Support for vector values

```cfg
# Simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10
```

## Logging Levels

- **Debug**: Detailed step-by-step simulation information
- **Info**: High-level simulation progress and status
- **Warning**: Potential issues or unexpected behavior
- **Error**: Errors that affect simulation functionality
- **Critical**: Severe errors requiring immediate attention

## Best Practices

1. **Configuration Management**:
   - Set random_seed for reproducible experiments
   - Adjust neuron_count based on computational resources
   - Use connection_probability to control network density

2. **Logging**:
   - Set appropriate log level for production runs
   - Enable file logging for long simulations
   - Use NLM_LOG_* macros for structured logging

3. **Type Safety**:
   - Use strong types for all neural identifiers
   - Avoid raw integer comparisons between different ID types
   - Leverage compile-time type safety

## Performance Considerations

- Configuration lookup is O(log n) for large numbers of entries
- Logging can be disabled at compile-time for maximum performance
- Random number generation uses cached values where possible
- Memory pools are used for frequently allocated objects

## Next Steps

- Implement JSON/YAML configuration parser
- Add configuration validation and schema support
- Integrate environment variable configuration
- Add configuration hot-swapping capabilities

## Related Documentation

- [Architecture Overview](docs/ARCHITECTURE.md)
- [Phase-Specific Configurations](docs/PHASE*.md)
- [Performance Guidelines](docs/PERFORMANCE.md)
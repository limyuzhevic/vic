# NLM Quick Start Guide

## Getting Started with NLM

This guide provides quick access to the most important features and commands in the NLM project.

## Quick Installation and Build

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with default settings (includes Python bindings)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all executables
make -j4
```

### Build Options

```bash
# Build without Python bindings
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=OFF

# Build with specific Python version
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3.10

# Build for development (with debug symbols)
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## Running Demos

### Unified Demo System (Recommended)

The unified demo system provides consistent interfaces for running different types of demonstrations:

```bash
# Basic demo (Phase 2 functionality)
nlm_unified --demo=basic --steps=500

# Plasticity learning demo (STDP and Hebbian)
nlm_unified --demo=plasticity --steps=2000 --neurons=200 --development

# System integration demo (Phase 6 functionality)
nlm_unified --demo=integration --steps=3000 --checkpoint --curiosity

# Advanced demo (all features enabled)
nlm_unified --demo=advanced --steps=5000 --reward --development --curiosity --checkpoint
```

### Original Demos (for backward compatibility)

```bash
# Phase 2 demo
./nlm

# Phase 3 demo
./nlm_phase3_demo

# Phase 4 demo
./nlm_phase4_demo

# Phase 5 demo
./nlm_phase5_demo

# Phase 6 integration demo
./nlm_phase6_demo

# Phase 6 text demo (Phase 6: text-as-a-sense)
./nlm_phase6_text_demo

# Performance benchmark
./nlm_benchmark
```

## Advanced Configuration Management

### Configuration Presets

Apply predefined configuration presets:

```bash
# Basic demo preset
nlm_config --preset=demo_basic

# Plasticity learning preset
nlm_config --preset=demo_plasticity --neurons=500

# Integration preset
nlm_config --preset=demo_integration --development
```

### Configuration File Operations

```bash
# Export current configuration to file
nlm_config --export_config=my_config.cfg

# Import configuration from file
nlm_config --import_config=exported_config.cfg

# Validate configuration file
nlm_config --validate_config

# Display configuration summary
nlm_config --config_summary
```

### Configuration Options

```bash
# Core settings
--neuron_count=<n>              Number of neurons
--region_count=<n>              Number of brain regions
--connection_probability=<f>     Connection probability (0.0-1.0)
--simulation_timestep=<f>        Simulation timestep in seconds
--random_seed=<n>               Random seed for reproducibility

# Plasticity settings
--stdp_ltp_weight=<f>            STDP LTP weight
--stdp_ltd_weight=<f>            STDP LTD weight
--stdp_tau=<f>                   STDP time constant
--synaptogenesis_rate=<f>        Synaptogenesis rate
--pruning_rate=<f>               Pruning rate

# Memory integration
--replay_interval=<n>            Replay interval in steps
--consolidation_interval=<n>     Consolidation interval in steps

# Feature flags
--checkpoint                    Enable checkpointing
--development                   Enable brain development
--curiosity                     Enable curiosity-driven exploration
--reward                         Enable reward-based learning
```

## Power User Features

### Batch Operations

Run multiple demos with different configurations:

```bash
# Run basic, plasticity, and integration demos in sequence
nlm_power_user batch --demo_types=basic,plasticity,integration --config="neuron_count=100"

# Run with parallel execution for better performance
nlm_power_user batch --demo_types=basic,plasticity,integration --parallel

# Output results to file
nlm_power_user batch --demo_types=basic --output_dir=./batch_results
```

### Performance Profiling

Profile brain performance:

```bash
# Profile basic demo performance
nlm_power_user profile --demo=basic --steps=1000

# Generate detailed performance report
nlm_power_user profile --demo=advanced --steps=5000 --output=performance_report.json
```

### Command Execution

Execute power user commands:

```bash
# Create configuration template
nlm_power_user command create_config_template

# Validate configuration file
nlm_power_user command validate_config my_config.cfg

# Generate brain report
nlm_power_user command generate_report
```

## Testing and Validation

### Run Test Suite

Run comprehensive tests for all improvements:

```bash
# Run all tests
./nlm_test_suite

# Run specific test categories
./nlm_test_suite --category=configuration
./nlm_test_suite --category=demo_system
./nlm_test_suite --category=power_user
```

### Test Configuration

Test configuration validation:

```bash
# Test configuration validation
nlm_config --validate_config

# Test configuration with demo
nlm_config --test_config --demo=basic

# Benchmark configuration settings
nlm_config --benchmark_config
```

## Configuration File Examples

### Basic Configuration

```
# configs/basic.cfg
[config]
neuron_count = 100
region_count = 1
connection_probability = 0.15
simulation_timestep = 0.001
random_seed = 42

[features]
enable_development = false
enable_curiosity = false
enable_reward = false
enable_checkpointing = false
```

### Integration Configuration

```
# configs/integration.cfg
[config]
neuron_count = 1000
region_count = 2
connection_probability = 0.08
simulation_timestep = 0.001
random_seed = 456

[plasticity]
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

[memory]
replay_interval = 100
consolidation_interval = 1000

[features]
enable_development = true
enable_curiosity = true
enable_reward = true
enable_checkpointing = true
```

## Troubleshooting

### Common Issues and Solutions

#### "Python module not found"

**Problem**: Python bindings not built
**Solution**: 
```bash
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make
```

#### "Configuration file not found"

**Problem**: Configuration file specified but doesn't exist
**Solution**: Create the file or use default options

#### "Demo not found"

**Problem**: Invalid demo type specified
**Solution**: Use one of: basic, plasticity, integration, advanced

#### "Performance issues"

**Problem**: Demos running slowly
**Solution**: 
- Reduce neuron count: `--neurons=100`
- Reduce steps: `--steps=500`
- Disable heavy features: `--development=false --curiosity=false`

### Getting Help

```bash
# Get help for unified demo system
nlm_unified --help

# Get help for advanced configuration
nlm_config --help

# Get help for power user features
nlm_power_user --help

# Get help for test suite
nlm_test_suite --help
```

## Examples

### Example 1: Basic Brain Simulation

```bash
nlm_unified --demo=basic --steps=1000
```

### Example 2: Learning Demo

```bash
nlm_unified --demo=plasticity --steps=3000 --neurons=200 --development --curiosity
```

### Example 3: Integration Test

```bash
nlm_unified --demo=integration --steps=5000 --checkpoint --replay_interval=100
```

### Example 4: Advanced Features

```bash
nlm_unified --demo=advanced --steps=10000 --reward --development --curiosity --checkpoint --replay
```

### Example 5: Configuration Management

```bash
# Create template
nlm_power_user command create_config_template my_template.cfg

# Validate and test
nlm_power_user command validate_config my_template.cfg
nlm_power_user command test_config my_template.cfg

# Run with validated configuration
nlm_unified --config_file=my_template.cfg
```

## Advanced Usage

### Scripting

Create scripts for automated experiments:

```bash
#!/bin/bash
# run_experiments.sh

# Define experiment configurations
configs=(
    "basic:100:1:0.2"
    "plasticity:200:1:0.15"
    "integration:300:2:0.1"
)

# Run each experiment
for config in "${configs[@]}"; do
    IFS=':' read -r demo neurons regions prob <<< "$config"
    echo "Running $demo demo with $neurons neurons..."
    ./nlm_unified --demo=$demo --neurons=$neurons --regions=$regions --connection_prob=$prob
    sleep 2  # Wait between experiments

done
```

### Performance Optimization

```bash
# Build with performance optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j$(nproc)

# Run optimized demo
./nlm_unified --demo=advanced --steps=20000 --parallel
```

## Post-Build Verification

After building, verify that all executables are working:

```bash
# Check that all executables exist
ls -la bin/

# Run quick test of each executable
./nlm --steps=10
./nlm_unified --demo=basic --steps=10
./nlm_config --validate_config
```

## Migration Guide

### From Previous Versions

If you're migrating from an earlier version of NLM:

1. **Update your build scripts** to include the new executables
2. **Update your demo calls** to use the unified demo system where appropriate
3. **Add configuration presets** for your specific use cases
4. **Consider using batch operations** for running multiple experiments
5. **Add configuration validation** to your scripts

### Backward Compatibility

The project maintains backward compatibility:
- All original demo executables continue to work unchanged
- Configuration file formats are compatible with earlier versions
- Command-line interfaces maintain existing options
- All Phase 3-6 demo executables are still available

## Support and Community

### Getting Help

1. **Documentation**: Check `docs/` directory for detailed documentation
2. **Issues**: Report issues on the GitHub repository
3. **Community**: Join the NLM community for discussions
4. **Examples**: Refer to `examples/` directory for sample scripts

### Resources

- **Documentation**: `README.md`, `docs/ARCHITECTURE.md`, `docs/SCIENCE.md`
- **Examples**: `examples/` directory with sample scripts
- **Configuration**: `configs/` directory with example configuration files
- **Tests**: `src/test_suite.cpp` for comprehensive testing

## Conclusion

NLM now provides a comprehensive, user-friendly interface for running neural brain simulations with:

- **Unified Demo System**: Consistent interfaces for all demo types
- **Advanced Configuration**: Powerful configuration management with presets
- **Power User Features**: Batch operations, profiling, and scripting
- **Comprehensive Testing**: Thorough validation of all features
- **Backward Compatibility**: All existing functionality preserved

Get started today and discover the power of NLM!

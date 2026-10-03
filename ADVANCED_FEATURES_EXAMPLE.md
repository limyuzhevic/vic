# NLM Phase 2 Advanced Features Example

This document demonstrates the advanced features added to NLM (Neural Learning Machine) Phase 2, focusing on improvements for both beginners and expert users.

## 1. Enhanced Configuration System

### JSON Configuration
NLM now supports JSON configuration files, making it easier to manage complex settings:

```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.15,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  "log_level": "INFO",
  "log_to_file": true,
  "log_filename": "nlm_detailed.log"
}
```

### YAML Configuration
For a more human-readable format:

```yaml
neuron_count: 1000
region_count: 2
connection_probability: 0.15
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
synaptogenesis_rate: 0.0001
pruning_rate: 0.00001
log_level: INFO
log_to_file: true
log_filename: "nlm_detailed.log"
```

### Advanced Configuration Operations

#### Configuration Validation
```bash
./nlm --validate-config
```

#### Configuration Export
```bash
./nlm --export-config exported_config.json
```

#### Configuration Import
```bash
./nlm --import-config my_config.json
```

## 2. Enhanced Command Line Interface

### Basic Usage (Beginner)
```bash
# Run basic demonstration
./nlm --run

# Run specific tests
./nlm --test connectivity --steps 2000
./nlm --test plasticity
./nlm --test stdp

# Enable verbose output
./nlm --run --verbose
```

### Advanced Usage (Expert)
```bash
# Enable performance monitoring
./nlm --run --enable-performance-monitoring --performance-threshold 1.0

# Enable memory profiling
./nlm --run --enable-memory-profiling --max-memory 2048

# Enable detailed logging
./nlm --run --enable-detailed-logging --profiling-level 2

# Stress testing
./nlm --stress --duration 5000 --verbose

# Comprehensive benchmarking
./nlm --benchmark-full

# Interactive mode
./nlm --interactive
```

## 3. Performance Monitoring

### Enable Performance Monitoring
```bash
./nlm --run --enable-performance-monitoring
```

This will:
- Track execution time for all operations
- Monitor spike generation rates
- Log detailed performance metrics
- Display performance summaries at the end

### Performance Threshold Configuration
```bash
./nlm --run --enable-performance-monitoring --performance-threshold 2.0
```

## 4. Memory Profiling

### Enable Memory Profiling
```bash
./nlm --run --enable-memory-profiling
```

This will:
- Track memory usage during simulation
- Monitor allocation/deallocation patterns
- Report peak memory usage
- Help identify memory leaks

## 5. Configuration Management

### Save/Load Brain State
```bash
# Save brain state for checkpointing
./nlm --run --save-state checkpoint.nml

# Load previous brain state
./nlm --load-state checkpoint.nml --run
```

### Configuration Comparison
```bash
# Compare two configuration files
./nlm --compare configs/default.cfg configs/custom.cfg
```

## 6. Advanced Testing Features

### Stress Testing
```bash
./nlm --stress --duration 10000 --intensity high
```

### Comprehensive Benchmarking
```bash
./nlm --benchmark-full --verbose
```

### Configuration Validation
```bash
./nlm --validate-config --strict
```

## 7. Plugin System (Framework)

### Plugin Loading
```bash
./nlm --run --enable-plugin-system --plugin-path ./plugins
```

### Plugin Management
```bash
./nlm --plugin-help
```

## 8. Examples

### Example 1: Basic Connectivity Test
```bash
./nlm --test connectivity --steps 500 --verbose
```

### Example 2: Plasticity Experiment
```bash
./nlm --test plasticity --steps 1000 --enable-performance-monitoring
```

### Example 3: STDP Verification
```bash
./nlm --test stdp --steps 200 --enable-memory-profiling
```

### Example 4: Complete Advanced Session
```bash
./nlm --run \
  --enable-performance-monitoring \
  --enable-memory-profiling \
  --enable-detailed-logging \
  --config advanced_config.json \
  --set neuron_count 2000 \
  --set region_count 3 \
  --save-state final_checkpoint.nml
```

## 9. Performance Optimization

### Optimized Configuration
```json
{
  "simulation_timestep": 0.0001,
  "neuron_count": 2000,
  "region_count": 3,
  "connection_probability": 0.1,
  "stdp_ltp_weight": 0.018,
  "stdp_ltd_weight": 0.017,
  "performance_monitoring": true,
  "memory_profiling": true
}
```

## 10. Troubleshooting

### Common Issues

1. **Configuration File Not Found**
```bash
# Use absolute path or ensure file exists
./nlm --config ./configs/my_config.cfg
```

2. **Memory Issues**
```bash
# Reduce memory usage
./nlm --run --max-memory 512
```

3. **Performance Issues**
```bash
# Increase performance threshold
./nlm --run --enable-performance-monitoring --performance-threshold 5.0
```

4. **Logging Issues**
```bash
# Enable file logging
./nlm --run --enable-detailed-logging
```

## 11. Next Steps

1. **For Beginners**: Start with basic usage examples and gradually explore advanced features.

2. **For Experts**: Dive into performance monitoring, memory profiling, and plugin system.

3. **For Developers**: Contribute to the plugin system and extend configuration options.

4. **For Researchers**: Use stress testing and benchmarking for performance analysis.

## 12. Reference

### Command Line Options
```bash
./nlm --help
./nlm --expert-help
```

### Configuration Files
```
./nlm --config configs/default.cfg
./nlm --config configs/production.cfg
```

### Output Directories
```
./nlm --run --log-filename "nlm_output.log"
```

This example demonstrates the comprehensive set of features available in NLM Phase 2, catering to users from beginners to advanced experts.
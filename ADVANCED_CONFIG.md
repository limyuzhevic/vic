# Advanced NLM Configuration System

## Overview

The NLM (Neural Learning Machine) project now features a comprehensive advanced configuration system that provides:

- **Multi-format configuration support** (JSON, YAML, key-value)
- **Configuration inheritance and override system**
- **Advanced CLI with extensive options**
- **Phase-specific configuration templates**
- **Real-time logging configuration**
- **Profiling and debugging capabilities**
- **Parallel execution support**
- **Configuration validation and export**

## Configuration File Formats

### JSON Format
```json
{
  "simulation": {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "max_simulation_steps": 10000
  },
  "neural": {
    "neuron_count": 500,
    "region_count": 1
  }
}
```

### YAML Format
```yaml
simulation:
  random_seed: 42
  simulation_timestep: 0.001
  max_simulation_steps: 10000
neural:
  neuron_count: 500
  region_count: 1
```

### Key-Value Format
```
random_seed = 42
simulation_timestep = 0.001
max_simulation_steps = 10000
neuron_count = 500
region_count = 1
```

## Advanced Configuration System

### Core Features

#### 1. Configuration Schema Validation
- Comprehensive schema for all system components
- Cross-component validation checks
- Range and type validation
- Warning system for deprecated settings

#### 2. Configuration Inheritance
```bash
--inherit base_config.json --override key1 key2 key3
```
- Load base configuration
- Override specific keys
- Support for inheritance rules

#### 3. Advanced CLI Options

**Configuration Loading:**
```bash
--config=file.json              Load configuration from file
--template=phase2               Load template configuration
--batch file1.json file2.yaml    Process multiple files in batch mode
--interactive                   Start interactive configuration setup
```

**Configuration Override:**
```bash
--key=value                    Override configuration value
--key value                     Override with space separator
```

**Logging Configuration:**
```bash
--log_level=DEBUG              Set logging level
--log_file=nlm.log             Set log file name
--console_log=true             Enable/disable console logging
--file_log=true                Enable/disable file logging
```

**Debug and Profiling:**
```bash
--debug                        Enable debug mode
--profile                      Enable profiling
--breakpoints                  Enable breakpoints
--validate_only                Only validate configuration
--verbose                       Enable verbose output
--time_limit=60.0              Set time limit for simulation
```

#### 4. Configuration Templates

Available templates:

- **phase1_basic**: Basic configuration for Phase 1
- **phase2_neural**: Neural computation focused for Phase 2
- **phase3_interaction**: Agent-world interaction for Phase 3
- **phase4_cognitive**: Cognitive development for Phase 4
- **phase5_performance**: Performance optimization for Phase 5
- **phase6_integration**: Full integration for Phase 6

Example:
```bash
--template=phase2_neural --neuron_count=1000 --log_level=DEBUG
```

#### 5. Parallel Execution
```bash
--parallel config1.json config2.json config3.yaml
```
Run multiple simulations in parallel using multiple CPU cores.

#### 6. Configuration Validation and Export
```bash
--validate output.json           Validate configuration and export to JSON
--export output.json             Export current configuration to JSON file
```

## Template Files

All templates are available in the `configs/` directory:

- `template_phase2_neural.json`: Phase 2 neural computation
- `template_phase3_interaction.json`: Phase 3 agent-world interaction
- `template_phase4_cognitive.json`: Phase 4 cognitive development
- `template_phase5_performance.json`: Phase 5 performance optimization
- `template_phase6_integration.json`: Phase 6 full integration

## Usage Examples

### Basic Usage
```bash
# Load default configuration
./nlm --config=configs/default.cfg

# Load template configuration
./nlm --template=phase2_neural

# Override specific values
./nlm --template=phase2_neural --neuron_count=1000 --log_level=DEBUG
```

### Batch Processing
```bash
# Process multiple configuration files
./nlm --batch config1.json config2.yaml config3.cfg

# Run with validation and export
./nlm --batch config1.json config2.yaml --validate validated.json
```

### Parallel Execution
```bash
# Run multiple simulations in parallel
./nlm --parallel config1.json config2.json config3.json
```

### Advanced Configuration
```bash
# Full advanced configuration with all options
./nlm \
  --config=advanced.json \
  --log_level=DEBUG \
  --log_file=nlm.log \
  --console_log=true \
  --file_log=true \
  --debug \
  --profile \
  --time_limit=300.0 \
  --interactive
```

## Configuration Schema

The configuration system includes a comprehensive schema for all system components:

### Simulation Component
- `random_seed`: Integer, random seed for reproducible experiments
- `simulation_timestep`: Number, simulation timestep in seconds
- `max_simulation_steps`: Integer, maximum simulation steps
- `simulation_time_limit`: Number, simulation time limit in seconds

### Neural Component
- `neuron_count`: Integer, number of neurons in the brain
- `region_count`: Integer, number of brain regions
- `connection_probability`: Number, connection probability (0-1)
- `neuron_type`: String, neuron model type (LIF, HH, Izhikevich)

### Plasticity Component
- `plasticity_learning_rate`: Number, plasticity learning rate (0-1)
- `stdp_ltp_weight`: Number, STDP LTP weight
- `stdp_ltd_weight`: Number, STDP LTD weight
- `stdp_time_constant`: Number, STDP time constant

### Development Component
- `development_synaptogenesis_rate`: Number, synaptogenesis rate (0-1)
- `development_pruning_rate`: Number, pruning rate (0-1)
- `maturation_rate`: Number, maturation rate (0-1)

### Neuromodulation Component
- `dopamine_baseline`: Number, dopamine baseline level
- `curiosity_baseline`: Number, curiosity baseline level
- `novelty_threshold`: Number, novelty detection threshold (0-1)

### Reward Component
- `reward_discount_factor`: Number, reward discount factor (0-1)
- `reward_scale`: Number, reward scaling factor

### Environment Component
- `environment_name`: String, environment name
- `environment_width`: Integer, environment width
- `environment_height`: Integer, environment height
- `world_bounds_x`: Number, world bounds X coordinate
- `world_bounds_y`: Number, world bounds Y coordinate

### Logging Component
- `log_level`: String, logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- `log_to_file`: Boolean, log to file
- `log_filename`: String, log filename
- `log_file_append`: Boolean, append to log file
- `log_max_size`: Integer, maximum log file size in bytes

### Visualization Component
- `visualization_enabled`: Boolean, enable visualization
- `visualization_update_rate`: Number, visualization update rate in Hz
- `visualization_window_width`: Integer, visualization window width
- `visualization_window_height`: Integer, visualization window height

## Configuration Validation

The configuration system includes validation for:

### Schema Validation
- Type checking for all configuration values
- Range validation for numeric values
- Enum validation for string values
- Required field checking

### Cross-Component Validation
- Neuron count must be positive
- Connection probability must be between 0 and 1
- Simulation time limit requires positive max simulation steps

### Warning System
- Deprecated setting warnings
- Suboptimal configuration warnings
- Performance impact warnings

## Error Handling

Common errors and their solutions:

1. **Configuration file not found**
   ```
   Error: Configuration file not found: configs/missing.json
   ```
   Solution: Ensure the file exists or provide a valid path.

2. **Invalid configuration value**
   ```
   Error: Configuration value for 'neuron_count' is invalid: -10
   ```
   Solution: Use a positive integer value.

3. **Validation error**
   ```
   Error: Configuration validation failed:
     - neuron_count must be positive
     - connection_probability must be between 0 and 1
   ```
   Solution: Fix the validation errors and try again.

## Performance Considerations

### Large-Scale Simulations
For simulations with many neurons, use the performance-optimized template:
```bash
./nlm --template=phase5_performance --neuron_count=10000
```

### Memory Usage
- Enable batch processing for memory-efficient simulations
- Use checkpoint system for long-running simulations
- Monitor memory usage with `--debug` flag

### Parallel Processing
For multiple independent simulations:
```bash
./nlm --parallel config1.json config2.json config3.json
```

## Troubleshooting

### Configuration File Issues
- **File format mismatch**: Ensure the file format matches its extension
- **Syntax errors**: Use JSON/YAML validators before loading
- **Permission issues**: Check file read/write permissions

### Performance Issues
- **Slow simulation**: Reduce neuron count or use performance optimizations
- **Memory errors**: Use smaller configurations or enable checkpointing
- **Logging overhead**: Disable file logging or reduce log level

### Debugging
```bash
# Enable debug mode with verbose output
./nlm --debug --verbose --log_level=DEBUG

# Profile simulation performance
./nlm --profile --profile_output=profile.json

# Validate configuration before running
./nlm --validate_only --config=production.json
```

## Migration Guide

### From Phase 2 Default Configuration

The old default.cfg file is still supported but the new system provides better defaults and more options:

**Old configuration (default.cfg):**
```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
```

**New template (phase2_neural.json):**
```json
{
  "neuron_count": 500,
  "simulation_timestep": 0.001,
  "plasticity_learning_rate": 0.01
}
```

### Upgrading to Advanced Configuration

1. Replace `default.cfg` with a template:
   ```bash
   cp configs/template_phase2_neural.json configs/production.json
   ```

2. Use the advanced CLI:
   ```bash
   ./nlm --config=production.json
   ```

3. Customize as needed:
   ```bash
   ./nlm --config=production.json --neuron_count=2000 --log_level=DEBUG
   ```

## Advanced Features

### Real-time Configuration Updates
The system supports real-time configuration updates during simulation (experimental).

### Configuration History
Configuration changes are tracked for reproducibility.

### Multi-environment Support
Different environments can be configured for specific testing scenarios.

## Conclusion

The advanced configuration system provides comprehensive control over NLM simulations with:

- **Flexibility**: Support for multiple configuration formats
- **Validation**: Comprehensive schema and cross-component validation
- **Performance**: Optimized templates and parallel execution
- **Usability**: Interactive setup and extensive CLI options
- **Maintainability**: Template-based configuration for different phases

This system enables users to configure, validate, and run NLM simulations efficiently while maintaining reproducibility and performance.

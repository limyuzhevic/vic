# NLM Configuration Presets Documentation

## Overview

Configuration presets provide pre-defined settings for common NLM use cases. Each preset is stored as a JSON file in the `configs/presets/` directory and can be applied using the `--preset` command-line argument.

## Available Presets

### 1. Research Preset (`research.json`)

**Description:** Research-focused configuration with detailed logging and validation enabled

**Use Case:** Ideal for scientific research, experiments, and thorough analysis where comprehensive data collection is needed.

**Key Features:**
- Detailed DEBUG logging to file (`nlm_research.log`)
- Large simulation capacity (50,000 steps)
- Full recording of all neuron, synapse, memory, and neuromodulator states
- Performance monitoring with custom metrics
- Enhanced neural parameters (2,000 neurons)
- Synaptogenesis and pruning enabled

**Performance Considerations:**
- Higher resource usage
- Best for research environments with analysis tools

### 2. Performance Preset (`performance.json`)

**Description:** Performance-optimized configuration for speed with reduced logging

**Use Case:** Production environments where maximum speed and efficiency are critical.

**Key Features:**
- Minimal WARN-level logging
- Maximum parallel processing (8 threads)
- SIMD and AVX-512 optimizations
- Reduced simulation steps (1,000)
- Large memory pool (1M entries)
- Minimal recording and analysis

**Performance Considerations:**
- Highest throughput
- Best for production systems and real-time applications

### 3. Education Preset (`education.json`)

**Description:** Education-focused configuration with simplified settings and verbose output

**Use Case:** Learning environments, demonstrations, and teaching scenarios.

**Key Features:**
- INFO-level logging with file output
- Balanced configuration (5,000 steps)
- Educational metrics and custom metrics enabled
- Moderate complexity (500 neurons)
- All recording and analysis enabled

**Performance Considerations:**
- Good balance between educational value and performance
- Best for demonstration and learning scenarios

### 4. Debug Preset (`debug.json`)

**Description:** Debug configuration with maximum verbosity and all debugging enabled

**Use Case:** Development, troubleshooting, and understanding system behavior.

**Key Features:**
- Detailed DEBUG logging to file (`nlm_debug.log`)
- Small simulation capacity (100 steps) for quick debugging
- All recording enabled with custom debug metrics
- No optimizations for maximum detail
- Enhanced neural plasticity parameters

**Performance Considerations:**
- Slowest preset
- Best for development and debugging

### 5. Production Preset (`production.json`)

**Description:** Production configuration with minimal output and validation enabled

**Use Case:** Live production systems and deployments.

**Key Features:**
- ERROR-level logging only
- Production log file (`nlm_production.log`)
- Large simulation capacity (10,000 steps)
- Validation enabled for data integrity
- Full optimization (SIMD, AVX-512, threading)
- Minimal recording to reduce overhead

**Performance Considerations:**
- High reliability and performance
- Best for deployed systems

## Preset Usage Examples

### Using Presets with Command Line

```bash
# Apply research preset
nlm --preset research

# Apply performance preset with custom config
nlm --preset performance --config myconfig.cfg

# Show available presets
nlm --list-configs

# Apply debug preset and show configuration
nlm --preset debug --show-config
```

### Programmatic Preset Usage

```cpp
// In C++ code
auto config = std::make_shared<Config>();
config->applyPreset(ConfigPreset::RESEARCH);
```

```python
# In Python bindings
import pynlm
config = pynlm.createDefaultConfig()
config.applyPreset(pynlm.ConfigPreset.RESEARCH)
```

## Preset Application Order

When applying a preset, the following sequence occurs:

1. **Apply Preset Parameters:** All preset-specific values are applied to the configuration
2. **Command Line Overrides:** Command line arguments override preset values
3. **File Loading:** Configuration files are loaded (presets first, then command line files)
4. **Validation:** Configuration validation is performed if enabled

## Creating Custom Presets

To create a custom preset:

1. Copy an existing preset JSON file from `configs/presets/`
2. Modify the `parameters` object with your desired settings
3. Save as `my-custom-preset.json`
4. Apply using `--preset my-custom-preset`

## Preset File Schema

Each preset JSON file should contain:

```json
{
  "name": "preset_name",
  "description": "Human-readable description of the preset",
  "parameters": {
    "parameter_name": "value",
    "another_parameter": 123
  }
}
```

## Parameter Categories

### Logging Parameters
- `log_level`: Log verbosity (DEBUG, INFO, WARN, ERROR, FATAL)
- `log_to_file`: Whether to log to file
- `log_filename`: Output log file name

### Performance Parameters
- `parallel_threads`: Number of processing threads
- `enable_simd`: SIMD instruction usage
- `enable_avx2`: AVX2 instruction usage
- `enable_avx512`: AVX-512 instruction usage
- `enable_memory_pool`: Memory pool optimization
- `memory_pool_size`: Memory pool size

### Simulation Parameters
- `max_simulation_steps`: Maximum simulation steps
- `enable_validation`: Schema validation
- `validation_frequency`: Validation check frequency

### Neural Parameters
- `neuron_count`: Number of neurons
- `stdp_ltp_weight`: STDP LTP weight
- `stdp_ltd_weight`: STDP LTD weight
- `synaptogenesis_rate`: Synaptogenesis rate
- `pruning_rate`: Pruning rate

### Recording Parameters
- `record_neurons`: Record neuron states
- `record_synapses`: Record synapse states
- `record_memories`: Record memory states
- `record_modulators`: Record neuromodulator states

### Analysis Parameters
- `compute_metrics`: Compute performance metrics
- `track_rewards`: Track rewards
- `track_errors`: Track prediction errors

### Monitoring Parameters
- `enable_performance_monitoring`: Performance monitoring
- `performance_sample_rate`: Performance sampling rate
- `enable_memory_monitoring`: Memory monitoring
- `enable_cpu_monitoring`: CPU monitoring

## Configuration Priority

When multiple configuration sources are used, the following priority determines the final value:

1. **Command Line Overrides** (`--config-value`, `--config-object`)
2. **Command Line Files** (`--config`, `--json-config`, `--merge-configs`)
3. **Preset Configuration** (`--preset`)
4. **Default Configuration** (built-in defaults)

## Preset Examples

### Research Preset

```bash
# Run with research configuration
nlm --preset research --dry-run --show-config

# Combine with custom overrides
nlm --preset research --config-value neuron_count=3000 --log-level=DEBUG
```

### Performance Preset

```bash
# Run with performance configuration
nlm --preset performance --simulation-steps 5000

# For production deployment
nlm --preset performance --export-config production_config.json
```

### Education Preset

```bash
# For educational demonstrations
nlm --preset education --log-level INFO

# Show what configuration will be used
nlm --preset education --show-config
```

## Troubleshooting

### Preset Not Applying

1. Check that the preset file exists in `configs/presets/`
2. Verify the preset name matches exactly
3. Ensure the JSON is valid (use a JSON validator)

### Command Line Arguments Not Working

1. Ensure arguments are in the correct format
2. Check that there are no typos in option names
3. Verify that required values are provided

### Configuration Validation Failures

1. Check that preset parameters match the expected types
2. Verify that all required configuration keys are present
3. Ensure parameter values are within valid ranges

## Preset Migration Guide

When upgrading NLM versions, preset configurations may need updates. Follow these guidelines:

1. Always backup existing preset configurations
2. Test new presets in a controlled environment first
3. Verify that all parameters are still supported
4. Update custom presets if needed

## Conclusion

Configuration presets provide a powerful way to quickly configure NLM for different use cases. Whether you're running research experiments, deploying production systems, or teaching about neural networks, there's a preset that fits your needs.

For more information about NLM configuration, refer to the main configuration documentation.

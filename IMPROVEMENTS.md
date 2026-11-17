# NLM Improvements Test
# Version: 2.0

This file demonstrates the improvements made to the NLM (Neural Learning Machine) codebase.

## Summary of Improvements

### 1. Fixed Command Line Argument Parsing Bug
- **Before**: Only handled `--config=file` format, not `--config file`
- **After**: Handles both `--config=file` and `--config file` formats
- **Files**: `src/main.cpp` (lines 333-343)

### 2. Enhanced Configuration System
- **Before**: Basic key=value format with placeholder values
- **After**: Advanced JSON/YAML support, type-safe setters, validation
- **Files**: `src/core/Config/Config.hpp`, `src/core/Config/Config.cpp`

### 3. Made Hardcoded Values Configurable
- **Before**: Many hardcoded values like current injections, step counts
- **After**: All values can be configured via config files
- **Files**: `src/main.cpp` (multiple functions)

### 4. Improved LearningExperiment Memory Efficiency
- **Before**: Stored full weight vectors for all synapses (memory intensive)
- **After**: Memory-efficient sampling with configurable limits
- **Files**: `src/main.cpp` (LearningExperiment struct)

### 5. Enhanced Python API
- **Before**: Basic set of Python bindings
- **After**: Advanced debugging, visualization, and analysis tools
- **Files**: `python/bindings.cpp` (extensive additions)

### 6. Added Visualization and Debugging Utilities
- **New Python functions**:
  - `createDebugWorld()` - Create worlds with debug configuration
  - `createDebugMap()` - Generate world state visualizations
  - `exportSensoryData()` - Export sensory information
  - `createTrainingScenario()` - Create predefined scenarios
  - `exportTrainingData()` - Export data for ML training
  - `createBenchmarkConfig()` - Create configurations for benchmarking
  - `getStatsSummary()` - Get comprehensive statistics
  - `exportNetworkTopology()` - Export network structure
  - `analyzeActivity()` - Analyze brain activity over time

### 7. Enhanced Error Handling and Validation
- **New Config methods**:
  - `setValidated()` - Type-safe setters with validation
  - `getValidated()` - Safe getters with validation
  - `validate()` - Comprehensive configuration validation
  - `isValid()` - Quick validation check

### 8. Advanced Configuration Features
- **New Config methods**:
  - `setBrainParameters()` - Configure brain parameters
  - `setPlasticityParameters()` - Configure plasticity rules
  - `setNeuromodulationParameters()` - Configure neuromodulation
  - `setDevelopmentParameters()` - Configure development stages
  - `detailedSummary()` - Detailed configuration summary
  - `importSection()` / `exportSection()` - Configuration sections
  - `createSnapshot()` / `restoreSnapshot()` - Configuration versioning
  - `getKeysByPattern()` - Pattern-based key search
  - `getValuesForSource()` - Get values by configuration source

## Usage Examples

### Example 1: Using Command Line Arguments
```bash
# Old way (only worked with = format)
./nlm --config=configs/custom.cfg

# New way (both formats work)
./nlm --config custom.cfg
./nlm --config=configs/custom.cfg
./nlm --enable-plasticity --neuron-count=2000
```

### Example 2: Advanced Python Usage
```python
import pynlm

# Create a brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("plasticity.experiment.seed", 12345)
brain = pynlm.createBrain(config)

# Create a debug world for visualization
world = pynlm.createDebugWorld(50, 50, 16, 16)
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)

# Add debug objects for testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Get comprehensive statistics
summary = brain.getStatsSummary()
print(summary)

# Analyze brain activity over time
activity_data = brain.analyzeActivity(1000, 0.001)
print("Activity Analysis:", activity_data)

# Export network topology
topology = brain.exportNetworkTopology()
print("Network Topology:", topology)
```

### Example 3: Configuration Validation
```python
import pynlm

# Create configuration with validation
config = pynlm.createDefaultConfig()

# Set values with validation
try:
    config.setValidated("brain.neuron_count", 2000, 
                       validator=lambda x: x > 0 and x <= 10000,
                       errorMessage="Neuron count must be between 1 and 10000")
    print("Configuration validated successfully")
except ValueError as e:
    print("Validation failed:", e)

# Validate entire configuration
if config.isValid():
    print("Configuration is valid")
else:
    errors = config.validate()
    print("Validation errors:", errors)
```

### Example 4: Training Scenario Creation
```python
import pynlm

# Create world with training scenario
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)

# Create different training scenarios
scenario_type = "test_maze"  # or "test_resource"
debug_info = world.createTrainingScenario(scenario_type)
print(debug_info)

# Export for training
export_result = world.exportTrainingData("training_data.json")
print(export_result)
```

## Configuration Options

### Brain Configuration
- `brain.neuron_count` - Number of neurons (default: 1000)
- `brain.synapse_density` - Synapse connectivity density (default: 0.1)
- `brain.connection_probability` - Connection probability between neurons (default: 0.05)
- `brain.initial_weight_mean` - Mean initial synaptic weight (default: 0.5)
- `brain.initial_weight_std` - Standard deviation of initial weights (default: 0.1)

### Plasticity Configuration
- `plasticity.stdp.enable` - Enable STDP (default: true)
- `plasticity.stdp.learning_rate` - STDP learning rate (default: 0.001)
- `plasticity.hebbian.enable` - Enable Hebbian learning (default: true)
- `plasticity.structural.enable` - Enable structural plasticity (default: true)

### Neuromodulation Configuration
- `neuromod.dopamine.scale` - Dopamine modulation scale (default: 1.0)
- `neuromod.curiosity.enable` - Enable curiosity (default: true)
- `neuromod.novelty.enable` - Enable novelty detection (default: true)

### Debug and Analysis
- `debug.max_weight_samples` - Maximum synapses to record weights (default: 1000)
- `debug.enable_detailed_logging` - Enable detailed logging (default: false)
- `analysis.step_interval` - Steps between analysis reports (default: 100)

## Performance Improvements

1. **Memory Efficiency**: LearningExperiment now samples weights instead of storing all
2. **Configuration Flexibility**: Hardcoded values replaced with configurable parameters
3. **Error Handling**: Comprehensive validation and error reporting
4. **Debugging Tools**: Advanced visualization and analysis functions in Python API

## Testing

The improvements can be tested using the existing test suite:
```bash
./nlm_test  # Run all C++ tests
python -m pytest tests/  # Run Python tests
```

## Future Enhancements

Consider these additional improvements for future versions:

1. **Machine Learning Integration**: Export brain data for ML training
2. **Visualization Tools**: 3D brain visualization in Python
3. **Distributed Computing**: Parallel simulation capabilities
4. **Advanced Neural Models**: Implement more biologically realistic neuron models
5. **Cloud Deployment**: Containerized deployment and scaling

## Backward Compatibility

All improvements maintain backward compatibility:
- Existing config files continue to work
- Python API is extended, not changed
- C++ API maintains existing interface
- Default behaviors are preserved

## Conclusion

These improvements make NLM more:
- **Configurable** - More options and flexibility
- **Debuggable** - Better tools for debugging and analysis
- **Memory Efficient** - Reduced memory footprint
- **User-Friendly** - Better Python API and documentation
- **Robust** - Better error handling and validation
- **Advanced** - More features for power users
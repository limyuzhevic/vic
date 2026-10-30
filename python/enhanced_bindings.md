Enhance Python bindings with better type safety and documentation

The current Python bindings provide a basic interface but have several limitations:
1. Limited error handling and type checking
2. Minimal documentation and examples
3. No advanced utility functions for analysis
4. Basic configuration support

This enhancement creates a new file `enhanced_bindings.cpp` that addresses these issues.

The improved bindings include:

## Key Improvements:

### 1. Advanced Configuration System
- JSON/YAML support for configuration files
- Configuration validation and migration tools
- Dictionary-based configuration for easier use
- Better error handling and type safety

### 2. Enhanced Brain Utilities
- Advanced statistical analysis functions
- Network health monitoring
- Activity-based neuron identification
- Energy calculation and optimization
- Connection density analysis

### 3. Agent Brain Enhancement
- Complete episode simulation utilities
- Behavior analysis and statistics
- Episode data management
- Environment setup automation

### 4. Better Documentation
- Comprehensive module-level documentation
- Detailed parameter descriptions
- Usage examples and tutorials
- Advanced function documentation

### 5. Error Handling and Type Safety
- Comprehensive exception handling
- Better type checking at Python level
- Input validation
- Custom error types

### 6. Advanced Features
- Configuration migration tools
- Episode data analysis
- Network diagnostics
- Performance monitoring
- Statistical analysis

## Benefits:

### For Beginners:
- Simple, intuitive API
- Extensive documentation
- Examples for common tasks
- Step-by-step tutorials

### For Advanced Users:
- Low-level brain control functions
- Network analysis tools
- Performance optimization utilities
- Advanced configuration options
- Custom analysis functions

### For Research:
- Statistical analysis tools
- Data export utilities
- Network diagnostics
- Simulation result storage
- Comparative analysis capabilities

## Integration:

This enhancement complements the existing `bindings.cpp` file by providing advanced functionality without breaking existing code. The enhanced bindings:

1. Maintain backward compatibility
2. Add new utility functions
3. Improve error handling
4. Add advanced configuration support
5. Provide better documentation

## Usage Examples:

### Basic Usage (Beginner)
```python
import pynlm

# Simple brain simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)
    
print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Advanced Configuration (Intermediate)
```python
import pynlm

# Create advanced configuration
config = pynlm.createConfigFromDict({
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.enable": True,
    "neuromod.dopamine.scale": 1.5,
    "development.enable": True
})

brain = pynlm.createBrain(config)
brain.initialize()
```

### Complete Agent Setup (Advanced)
```python
import pynlm

# Setup complete agent environment
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)

agent = pynlm.createAgentBrain(brain)
pynlm.setupEnvironment(agent, world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run episode
avg_reward = pynlm.runEpisode(agent, world, 500)
print(f"Average reward: {avg_reward}")
```

### Network Analysis (Research)
```python
import pynlm

# Create brain and run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)

# Get network statistics
report = pynlm.generateBrainReport(brain)
print(report)

# Analyze connectivity
connection_density = pynlm.calculateConnectionDensity(brain)
print(f"Connection density: {connection_density}")

# Get active neurons
active_neuron_ids = pynlm.getActiveNeurons(brain, topK=5)
print(f"Most active neurons: {active_neuron_ids}")
```

## Technical Details:

### File Structure:
- `bindings.cpp` - Original basic bindings (unchanged)
- `enhanced_bindings.cpp` - New enhanced bindings with advanced features

### Build Changes:
- Updated CMakeLists.txt to build both binding versions
- Added separate pybind11 modules for enhanced functionality
- Maintained backward compatibility

### API Design:
- Follows NLM naming conventions
- Consistent with existing codebase style
- Comprehensive parameter documentation
- Type hints for better IDE support

## Performance Considerations:

1. **Memory Usage**: Advanced utilities may increase memory usage
2. **Computation**: Network analysis functions may add overhead
3. **Configuration**: Advanced configuration parsing may be slower
4. **Error Handling**: Additional error checking may impact performance

## Testing and Validation:

The enhanced bindings include:
1. Comprehensive input validation
2. Error handling for edge cases
3. Type safety checking
4. Performance benchmarks
5. Compatibility testing

## Migration Guide:

For users of the basic bindings:
1. Existing code continues to work unchanged
2. New functions are available via `pynlm` module
3. Advanced features require no changes to existing code
4. Gradual migration to enhanced features is possible

This enhancement significantly improves the usability, reliability, and functionality of the NLM Python bindings while maintaining full backward compatibility.
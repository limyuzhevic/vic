# NLM — 熙然 (Neural Learning Machine)

**An Experimental Artificial Developmental Brain**

## Key Improvements Made

NLM has been comprehensively enhanced with the following improvements:

### 1. Enhanced Configuration System

NLM now includes a modern, enhanced configuration system with:

**New Features:**
- **JSON/YAML Support**: Load configuration from modern file formats using `--config=filename.json`
- **Configuration Validation**: Automatic validation with detailed error reporting and guidance
- **Statistics and Diagnostics**: Comprehensive configuration statistics and debugging tools
- **Batch Operations**: Efficient bulk configuration setting and getting
- **Type Safety**: Enhanced type checking with dedicated methods like `setInt()`, `getDouble()`
- **Default Configuration**: Automatic initialization with sensible defaults

**Usage Examples:**
```bash
# Load enhanced configuration
./nlm_demo --config=brain_config.json

# Show configuration summary
./nlm_demo --show-config

# Display configuration statistics
./nlm_demo --show-stats

# Validate configuration before loading
./nlm_demo --validate-config --config=settings.json

# Set parameters via command line
./nlm_demo --neurons=2000 --seed=42 --learning_rate=0.05
```

### 2. Enhanced Python Bindings

The Python bindings have been significantly enhanced with:

**New Features:**
- **Complete Neuron Class**: Full Python implementation of Leaky Integrate-and-Fire (LIF) neurons
- **Enhanced Brain Interface**: Detailed documentation and comprehensive statistics
- **Improved AgentBrain**: Better neuromodulation control and status reporting
- **Better Error Handling**: Comprehensive error messages and validation
- **Extended Functionality**: Additional utility methods and advanced features

**Python Examples:**
```python
import pynlm

# Create enhanced configuration
config = pynlm.createDefaultConfig()
config.setInt("neuron_count", 2000)
config.setDouble("learning_rate", 0.05)

# Create brain with enhanced configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Access neuron details
for region in brain.getRegions():
    for neuron in region.getAllNeurons():
        print(f"Neuron {neuron.getId()}: V={neuron.getMembranePotential():.2f}mV, Type={neuron.getType()}")

# Get brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### 3. Advanced Command-Line Interface

The CLI has been enhanced with powerful new options:

**New Options:**
- `--help`/`-h`: Comprehensive help with examples and usage instructions
- `--version`: Detailed version and build information
- `--show-config`: Display complete configuration summary and details
- `--show-stats`: Display configuration statistics and diagnostics
- `--validate-config`: Validate configuration before loading with detailed error reporting

**CLI Examples:**
```bash
# Show comprehensive help
./nlm_demo --help

# Run with enhanced configuration and validation
./nlm_demo \
  --config=brain_settings.json \
  --show-config \
  --show-stats \
  --validate-config \
  --neurons=1500 \
  --learning_rate=0.03 \
  --seed=123
```

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected:
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction and planning systems
- Cognitive functions (attention, concept formation)
- Development system with plasticity rules
- Checkpoint save/load functionality
- Replay and consolidation systems

### Basic Neural Computation Demo
```bash
./nlm_demo
```

This runs the Phase 2 demonstration with enhanced configuration options.

### Advanced Examples

**Enhanced Configuration with CLI:**
```bash
./nlm_demo \
  --config=brain_settings.json \
  --show-config \
  --show-stats \
  --validate-config \
  --neurons=1500 \
  --learning_rate=0.03 \
  --seed=123
```

**Python Integration Example:**
```python
import pynlm
import time

print("=== Enhanced NLM Python Example ===")

# Create enhanced brain
config = pynlm.createDefaultConfig()
config.setInt("neuron_count", 1000)
config.setDouble("connection_probability", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()

print("Brain created successfully")
print(f"Configuration: {config.summary()}")

# Run simulation for a few steps
for step in range(10):
    # Inject some sensory activity
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
    
    # Step the brain
    brain.step(step, step * 0.001)
    
    # Produce action every 5 steps
    if step % 5 == 0:
        action = brain.produceAction()
        print(f"Step {step}: Produced {action.getType()} action")

print("=== Simulation Complete ===")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── IMPROVEMENTS_SUMMARY.md      # Detailed improvements documentation
├── configs/
│   └── default.cfg               # Enhanced default configuration
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities (enhanced)
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
├── tests/             # Comprehensive test suite
└── python/            # Python bindings (enhanced)
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.

## Getting Started

### Quick Start
1. **Build the project:**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

2. **Run basic demo:**
```bash
./nlm_demo
```

3. **Run integration demo:**
```bash
./nlm_phase6_demo
```

4. **Install Python bindings:**
```bash
pip install pynlm
```

### Advanced Usage
1. **Create custom configuration:**
```bash
cat > my_config.json << EOF
{
    "neuron_count": 2000,
    "environment_name": "CustomWorld",
    "enable_development": true,
    "enable_curiosity": true
}
EOF
```

2. **Run with enhanced configuration:**
```bash
./nlm_demo --config=my_config.json --show-config --validate-config
```

3. **Python scripting:**
```python
import pynlm
import json

# Create and save configuration
config = pynlm.createDefaultConfig()
config.setInt("neuron_count", 1500)

with open("config.json", "w") as f:
    f.write(config.saveToJSON())

print("Configuration saved to config.json")
print(config.summary())
```

## Contact and Support

For questions about NLM or to contribute to development:
- Check the documentation in the `docs/` directory
- Review the Phase 6 audit report in `docs/PHASE6_FINAL_AUDIT.md`
- The Python bindings documentation is available in the bindings source code
- Issues and contributions: See the project repository

## Key Improvements Summary

### 1. Enhanced Configuration System
- **JSON/YAML Support**: Modern configuration file formats
- **Validation**: Automatic validation with detailed error reporting
- **Statistics**: Configuration statistics and debugging tools
- **Batch Operations**: Efficient bulk configuration management
- **Type Safety**: Enhanced type checking and validation

### 2. Enhanced Python Bindings
- **Complete Neuron Class**: Full Python implementation of LIF neurons
- **Enhanced Brain Interface**: Comprehensive documentation and statistics
- **Improved AgentBrain**: Better neuromodulation control
- **Error Handling**: Comprehensive error messages and validation

### 3. Advanced Command-Line Interface
- **Enhanced Help**: Comprehensive help with examples
- **Configuration Validation**: Pre-load validation
- **Statistics Output**: Detailed configuration statistics
- **Advanced Parameters**: Type-specific parameter handling

## Future Enhancements

The NLM project continues to evolve with plans for:
- **Phase 7**: Advanced learning algorithms and optimization
- **Phase 8**: Multi-agent systems and social learning
- **Enhanced Visualization**: Interactive brain visualization tools
- **Cloud Integration**: Distributed simulation capabilities
- **Machine Learning Integration**: Advanced training algorithms

## Conclusion

NLM has been comprehensively enhanced with:
- **Modern Configuration System**: JSON/YAML support with validation and statistics
- **Rich Python API**: Comprehensive bindings with detailed documentation
- **Enhanced User Experience**: Advanced CLI with comprehensive help and examples
- **Improved Code Quality**: Better documentation, error handling, and maintainability
- **Backward Compatibility**: All improvements maintain existing functionality

These improvements make the NLM project more accessible to new users while providing advanced features for experienced users, establishing a solid foundation for future development phases.

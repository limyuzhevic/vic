# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

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

This runs a comprehensive integration test verifying all brain systems are connected.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
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

# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

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

This runs a comprehensive integration test verifying all brain systems are connected.

## Enhanced Python Bindings (New Feature)

This version includes **significantly enhanced Python bindings** that provide:

- **Complete type coverage** for all NLM systems
- **Robust error handling** with specific exception types
- **Enhanced Python API** with advanced usability features
- **Memory-efficient data structures** for large datasets
- **Comprehensive documentation** and examples

### Key Improvements

#### 1. Enhanced Error Handling
```python
import pynlm as nlm

# Specific exceptions for different error scenarios
try:
    action = nlm.Action()
    action.setParameter(100, 5.0)  # Will raise InvalidParameterError
except nlm.InvalidParameterError as e:
    print(f"✓ Caught specific error: {e}")
```

#### 2. Complete Type Coverage
```python
# All NLM systems now fully exposed
brain = nlm.createBrain(config)

# Memory systems
working_memory = brain.getWorkingMemory()
e pisodic_memory = brain.getEpisodicMemory()

# Neuromodulation systems  
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()

# Cognition systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()

# Development system
development_system = brain.getDevelopmentSystem()
```

#### 3. Enhanced Configuration API
```python
config = nlm.createDefaultConfig()

# Type-safe configuration with enhanced getters
config.set("neuron_count", 1000)
config.set("connection_probability", 0.15)
config.set("enable_development", True)

# Safe type conversion
neuron_count = config.get("neuron_count", 0)  # int64_t
connection_prob = config.get("connection_probability", 0.0)  # double
enable_dev = config.get("enable_development", False)  # bool

# Get all configuration as Python dict
all_config = config.getAll()
for key, value in all_config.items():
    print(f"{key}: {value}")
```

#### 4. Enhanced Action Class
```python
# Advanced action parameter management
action = nlm.Action(nlm.ActionType.MoveForward, [0.5, 0.8, 1.2])

# Safe parameter access with error handling
try:
    param_value = action.getParameter(1)
    print(f"Parameter 1: {param_value}")
except nlm.InvalidParameterError as e:
    print(f"Error: {e}")

# Action readiness and utility methods
if action.isReady():
    print("Action is ready for execution")

# Cloning and copying
cloned_action = action.clone()
print(f"Cloned action: {cloned_action.getName()}")
```

#### 5. Enhanced World Object System
```python
world = nlm.createSimpleWorld()
world.initialize(64, 64, 32, 32)

# Rich object creation and management
player = nlm.WorldObject(10.0, 10.0, nlm.WorldObjectType.Resource, 100.0, 0.8)
player.setVelocity(1.5, 0.5)
player.setHealth(150.0)

# Enhanced object interactions
distance = player.getDistance(15.0, 15.0)
print(f"Distance to (15,15): {distance:.2f}")

# Get nearby objects with filtering
nearby = world.getObjectsNear(10.0, 10.0, 5.0)
print(f"Found {len(nearby)} objects near player")
```

#### 6. MemoryView Support
```python
# Efficient access to large data arrays
vision = nlm.Vision(1024, 1024)
vision.setData([0.1] * 3072000)  # Large RGB data

# Get memoryview for efficient access
data_view = vision.getDataPtr()
print(f"Vision data shape: {len(data_view)}")

# Perfect for performance-critical applications
import numpy as np
vision_array = np.array(data_view)
```

### Python Package Structure

```
pynlm/  # Python package
├── __init__.py      # Package initialization
├── core.py          # Core bindings
├── brain.py         # Brain system bindings
├── memory.py        # Memory system bindings
├── neuromodulation.py # Neuromodulation bindings
├── cognition.py     # Cognition system bindings
├── development.py   # Development system bindings
├── prediction.py     # Prediction system bindings
├── environment.py   # Environment bindings
├── agent.py         # Agent system bindings
├── world.py         # World system bindings
├── sensory.py       # Sensory system bindings
├── motor.py         # Motor system bindings
├── experiments.py   # Experiment bindings
├── demo_nlm_bindings.py  # Demonstration script
├── PYTHON_API_DOCS.md    # API documentation
└── setup.py             # Python package setup
```

### Installation

#### From Source (Recommended)

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the enhanced Python bindings
mkdir python_build
cd python_build
cmake ../python
make -j4

# Install the Python package
pip install -e python/
```

#### Using the Package

```python
import pynlm as nlm

# Create enhanced configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 2000)

# Create brain with enhanced API
brain = nlm.createBrain(config)
brain.initialize()

# Run enhanced demonstrations
nlm.demo_nlm_bindings.main()
```

#### Development Installation

```bash
# For development with editable install
cd nlm
pip install -e .
```

## Enhanced Demonstration

Run the comprehensive demonstration to see all features in action:

```bash
python python/demo_nlm_bindings.py
```

This will demonstrate:
- Enhanced error handling with specific exceptions
- Type-safe configuration management
- Advanced action parameter handling
- World object system enhancements
- MemoryView support for large data
- And much more!

## Project Structure

### Core System (Unchanged)
- **src/**: All C++ neural simulation code
- **CMakeLists.txt**: Main build configuration
- **configs/**: Configuration files

### Enhanced Python Package (New)
- **python/**: Complete Python bindings package
- **docs/PYTHON_API_DOCS.md**: Comprehensive API documentation
- **setup.py**: Python package setup
- **demo_nlm_bindings.py**: Feature demonstration

## Backward Compatibility

The enhanced Python bindings maintain **full backward compatibility** with the original API while adding new functionality:

```python
# Old API (still fully supported)
import pynlm as nlm

config = nlm.Config()
brain = nlm.Brain(config)
world = nlm.SimpleWorld()

# New enhanced API (available)
config = nlm.createDefaultConfig()  # Enhanced version
brain = nlm.createBrain(config)     # Enhanced version
world = nlm.createSimpleWorld()     # Enhanced version
```

All existing Python code will continue to work without any modifications.

## API Reference

For comprehensive API documentation, see:
- **`PYTHON_API_DOCS.md`**: Complete API reference with examples
- **`demo_nlm_bindings.py`**: Working demonstration of all features
- **Individual module files**: For specific class/method documentation

## Building and Running Tests

### Build Enhanced Python Bindings

```bash
# From the python directory
mkdir build
cd build
cmake ..
make -j4
```

### Running Python Tests

```bash
# Install package in development mode
pip install -e .

# Run demonstration
python demo_nlm_bindings.py

# Run any existing tests
python -m pytest tests/ -v
```

## Example Usage

### Basic Enhanced Brain Simulation

```python
import pynlm as nlm

# Create enhanced configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("enable_development", True)
config.set("enable_curiosity", True)

# Create enhanced brain
brain = nlm.createBrain(config)
brain.initialize()

# Create enhanced world
world = nlm.createSimpleWorld()
world.initialize(100, 100, 50, 50)

# Create enhanced agent
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

print(f"✓ Enhanced brain simulation ready")
print(f"✓ Neurons: {brain.getTotalNeuronCount()}")
print(f"✓ Regions: {brain.getRegionCount()}")
print(f"✓ Working memory: {brain.getWorkingMemory() is not None}")
```

### Error Handling Best Practices

```python
import pynlm as nlm

def safe_action_creation():
    """Demonstrate safe error handling practices"""
    try:
        # Create action with parameters
        action = nlm.Action(nlm.ActionType.MoveForward, [0.5, 0.8, 1.2])
        
        # Safe parameter access
        param = action.getParameter(1)
        print(f"Parameter accessed: {param}")
        
    except nlm.InvalidParameterError as e:
        print(f"✓ Invalid parameter caught: {e}")
    except nlm.MemoryAllocationError as e:
        print(f"✓ Memory error caught: {e}")
    except Exception as e:
        print(f"✓ General error caught: {type(e).__name__}: {e}")

safe_action_creation()
```

## Troubleshooting

### Common Issues

**1. ModuleNotFoundError: No module named 'pynlm'**
- **Solution**: Build the Python bindings first using `python/build.py`
- **Or**: `pip install -e python/` after building

**2. ImportError: libpynlm.so issues**
- **Solution**: Set `LD_LIBRARY_PATH=./:$$LD_LIBRARY_PATH`
- **Or**: Install to system library path with `sudo make install`

**3. Performance issues with large simulations**
- **Solution**: Use `getDataPtr()` for efficient memory access
- **Tip**: Process data in chunks for very large datasets

**4. Configuration loading errors**
- **Solution**: Check file paths and JSON format validity
- **Tip**: Use `config.getAll()` to inspect loaded configuration

### Getting Help

- **Documentation**: `PYTHON_API_DOCS.md` in the python directory
- **Demonstrations**: `demo_nlm_bindings.py`
- **Issues**: GitHub repository issues page
- **Community**: Project documentation and forums

## Development Guidelines

### Adding New Features

1. **Core System**: All neural simulation enhancements go in `src/`
2. **Python Bindings**: Add new bindings in `python/` directory
3. **Documentation**: Update `PYTHON_API_DOCS.md` with new examples
4. **Testing**: Add comprehensive tests for new functionality
5. **Compatibility**: Maintain backward compatibility with existing API

### Best Practices

- Use specific exception types for better error handling
- Implement memory-efficient patterns for large datasets
- Provide comprehensive documentation for all new bindings
- Write working examples and demonstrations
- Test thoroughly with edge cases and error conditions

## Performance Considerations

### Memory Efficiency
- Use `getDataPtr()` for vision/audio data to avoid copying
- For large datasets, use `memoryview` objects
- Avoid creating excessive intermediate objects

### Error Handling Efficiency
- Catch specific exceptions rather than broad exceptions
- Use try-catch blocks around potentially failing operations
- Validate inputs before operations

### Data Access Patterns
```python
# Efficient pattern for accessing large data
data_view = vision.getDataPtr()
# Process in chunks for large datasets
chunk_size = 256
for i in range(0, len(data_view), chunk_size):
    chunk = data_view[i:i + chunk_size]
    process_chunk(chunk)
```

## Future Enhancements

The enhanced Python bindings are designed for easy extension:

1. **Machine Learning Integration**: Integration with scikit-learn and TensorFlow
2. **Advanced Visualization**: 3D plotting and interactive visualizations
3. **Parallel Processing**: Multi-threading and GPU acceleration
4. **Distributed Computing**: Multi-machine simulation capabilities
5. **Additional Neural Models**: Support for other neuron models
6. **Plugin Architecture**: Extensible plugin system for custom brain modules

## Acknowledgments

This enhanced Python bindings project was made possible through:

- **pybind11**: The C++ Python bindings library
- **CMake**: The build system
- **Python**: The target language
- **NLM Research Team**: For developing the underlying neural simulation framework
- **Contributors**: For improvements and enhancements

## License

Enhanced Python bindings are licensed under the MIT License.

## Authors

Research project — See docs for scientific background.

---

*Enhanced Python bindings added for NLM Phase 6 integration*
*Last updated: 2026-09-27*
*New comprehensive error handling and API enhancements*


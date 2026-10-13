# NLM Python Bindings - README

This file describes the implementation of Python bindings for the NLM (Neural Learning Machine) project.

## Problem
The NLM project had incomplete Python bindings that couldn't properly expose the C++ neural simulation framework to Python users. The documentation (easy_usage.md and HOW_TO_USE.md) described API functionality that wasn't available.

## Solution Implemented

### Core Bindings (python/bindings.cpp)
Created comprehensive pybind11 bindings for all core NLM classes:

**Factory Functions (as documented):**
- `createDefaultConfig()` - Creates a default configuration
- `createBrain(config)` - Creates a new brain with configuration  
- `createSimpleWorld()` - Creates a new simple world
- `createAgentBrain(brain)` - Creates an agent brain interface

**Configuration Class (Config):**
- loadFromFile(filepath), loadFromArgs(argc, argv)
- saveToFile(filepath)
- has(key), getKeys(), clear()
- set(key, value) - Multiple type overloads
- summary(), __repr__()

**Brain Class:**
- initialize()
- step(currentStep), step(currentStep, currentTime)
- receiveSensoryInput(input)
- injectCurrent(neuron, current), injectCurrentToNeurons(type, current)
- produceAction()
- reset(), save(filepath), load(filepath)
- addRegion(name), getRegion(id)
- Statistical methods: getTotalNeuronCount(), getTotalSynapseCount(), getFiringNeuronCount(), etc.
- Development: get/setDevelopmentalStage()

**AgentBrain Class:**
- initialize(world)
- processSensoryInput(percept), decodeMotorCommand()
- applyRewardModulation(reward, predictedReward)
- updateDevelopment(timestep)
- enable/disable subsystems: rewardModulation, structuralPlasticity, development, curiosity
- Statistics: getCuriosityLevel(), getNoveltyLevel(), getPredictionError()

**SimpleWorld Class:**
- configure(width, height, visionWidth, visionHeight)
- reset(), setAgentStart(x, y)
- update(timestep), applyMotorCommand(cmd, currentTime)
- Sensory access: getSensoryPercept(), getAgentBody()
- World management: addObject, removeObject, isValidPosition
- Properties: getWidth(), getHeight(), getMaxEnergy(), getSimulationTime()

**Supporting Classes:**
- SensoryInput (base), Vision, Audio, InternalSignals
- Action with type and parameters
- WorldObject with position and properties
- AgentBody with movement state
- SensoryPercept with all sensory modalities

**Enumerations:**
- NeuronType (Excitatory, Inhibitory, Sensory, Motor, etc.)
- SynapseType (Excitatory, Inhibitory, Electrical, etc.)
- DevelopmentalStage (Initial, CriticalPeriod, Adult, etc.)
- ActionType (MoveForward, Interact, Eat, etc.)
- MotorCommand (MoveForward, TurnLeft, etc.)
- WorldObjectType (Empty, Resource, Hazard, etc.)

### API Verification

**test_nlm_bindings.py**: Comprehensive test that verifies:
- All factory functions exist and can be called
- All expected classes are exposed
- Basic usage patterns from easy_usage.md work

**test_nlm_api.cpp**: C++ test suite that validates:
- Config API: set/get, file I/O, keys management
- Brain API: creation, initialization, stepping, statistics
- AgentBrain API: initialization, sensory/motor processing
- SimpleWorld API: configuration and interaction

**examples.py**: Complete set of example scripts demonstrating:
- Example 1: Simplest Brain (from easy_usage.md)
- Example 2: Brain Watching a World (from easy_usage.md)  
- Example 3: Complete Agent (from HOW_TO_USE.md)
- Example 4: Run Simulation Pattern (from easy_usage.md)
- Example 5: Silent Brain (from easy_usage.md)
- Example 6: Configuration Usage (from HOW_TO_USE.md)

### Documentation Alignment

The bindings now fully support all examples from the documentation:

**From easy_usage.md:**
```python
# Simplest Brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)

# Brain in Simple World
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

**From HOW_TO_USE.md:**
```python
# Complete Agent Example
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
# ... simulation loop
```

## Build Requirements

To build and install the Python bindings:

```bash
# Install build dependencies
pip install scikit-build-core pybind11

# Build and install the package
pip install -e .

# Or build directly
python -m pip install build
python -m build
pip install dist/*.whl
```

Or using CMake:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## Testing

Run the verification script:
```bash
python test_nlm_bindings.py
```

## Files Created/Modified

1. **python/bindings.cpp** (new/comprehensive): Main Python bindings implementation
2. **python/CMakeLists.txt** (existing): Build configuration for Python module
3. **test_nlm_bindings.py** (new): Python API verification test
4. **test_nlm_api.cpp** (new): C++ API validation test
5. **examples.py** (new): Complete example gallery
6. **BINDINGS_SUMMARY.md** (new): Implementation summary

## API Completeness

The implementation now provides all functionality described in the NLM Python API documentation:

- ✅ All factory functions
- ✅ All classes and their public methods
- ✅ All enumerations
- ✅ All statistical methods
- ✅ All configuration options
- ✅ All example patterns
- ✅ Error handling and type safety

The Python bindings are now complete and ready for production use with all documented functionality working as specified.

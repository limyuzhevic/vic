# NeuralWorkingMemory Python Bindings - Build and Usage Guide

## Quick Start

This document provides instructions for building and using the NeuralWorkingMemory Python bindings.

## Building the Project

### Prerequisites

```bash
# Install build dependencies
apt-get update
apt-get install -y build-essential cmake python3-dev python3-pip

# Install Python package dependencies
pip install pybind11 scikit-build-core pytest numpy
```

### Build Steps

```bash
# Navigate to project directory
cd /path/to/nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build Python bindings
make pynlm

# Or build everything
make -j$(nproc)
```

### Alternative: Install via pip

```bash
# Install in development mode
cd /path/to/nlm
pip install -e .

# This will automatically build the bindings
```

## Using the Bindings

### Basic Usage Example

```python
import pynlm

# 1. Create brain system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# 2. Access working memory from brain
wm = pynlm.getWorkingMemoryFromBrain(brain)

# 3. Configure working memory
wm.setCapacity(150)
wm.setDecayRate(0.02)

# 4. Store and retrieve patterns
pattern = [0.5, 0.7, 0.9, 0.3]
wm.store(pattern)
retrieved = wm.retrieve()  # Returns list[float]

# 5. Run competition
wm.runCompetition()

# 6. Get statistics
activity = wm.getMemoryActivity()
traces = wm.getActiveTraces()
```

### Phase 6 Integration Example

```python
import pynlm

# Create complete Phase 6 brain system
config = pynlm.createWorkingMemoryConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Setup world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)

# Configure working memory for complex tasks
wm = pynlm.getWorkingMemoryFromBrain(brain)
wm.setCapacity(200)
wm.setDecayRate(0.01)

# Closed-loop operation
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Working memory stores pattern representation
    if percept:
        pattern = percept.getAllSignals()
        wm.store(pattern)
    
    # Update brain
    brain.step(step)
    
    # Agent makes action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## API Reference

### NeuralWorkingMemory Class

#### Constructor
```python
wm = pynlm.NeuralWorkingMemory()
```

#### Key Methods

| Method | Description | Parameters |
|--------|-------------|------------|
| `initialize(brain)` | Initialize with brain reference | brain: Brain |
| `store(pattern, strength=1.0)` | Store pattern in working memory | pattern: list[float], strength: float |
| `storeToNeuron(neuron, activation)` | Store activation to neuron | neuron: NeuronId, activation: float |
| `retrieve()` | Get working memory content | None |
| `contains(neuron)` | Check if neuron is in memory | neuron: NeuronId |
| `getNeuronActivation(neuron)` | Get activation of neuron | neuron: NeuronId |
| `update(dt)` | Update memory state | dt: float |
| `clear()` | Clear all memory content | None |
| `getActiveTraces()` | Get number of active traces | None |
| `getCapacity()` / `setCapacity(cap)` | Get/set memory capacity | cap: int |
| `getDecayRate()` / `setDecayRate(rate)` | Get/set decay rate | rate: float |
| `getMemoryNeurons()` | Get neurons in memory | None |
| `strengthenMemory(factor)` | Strengthen memory representations | factor: float |
| `runCompetition()` | Run competition to select winners | None |
| `isWinning(neuron)` | Check if neuron is winning | neuron: NeuronId |
| `getMemoryActivity()` | Get overall memory activity | None |

### Helper Functions

#### `getWorkingMemoryFromBrain(brain)`
```python
wm = pynlm.getWorkingMemoryFromBrain(brain)
```
Returns a NeuralWorkingMemory instance from a brain object.

#### `createWorkingMemory()`
```python
wm = pynlm.createWorkingMemory()
```
Creates a default NeuralWorkingMemory instance.

#### `createWorkingMemoryConfig()`
```python
config = pynlm.createWorkingMemoryConfig()
```
Creates a pre-configured Config object with working memory settings.

## Brain Class Integration

The Brain class now includes:
```python
brain.getWorkingMemory()  # Returns NeuralWorkingMemory
```

This allows easy access to working memory from brain instances.

## Working Memory Features

### Neural Dynamics

NeuralWorkingMemory implements:
- **Persistent neural activity** for short-term storage
- **Recurrent connections** for memory maintenance
- **Activity-dependent plasticity** for reinforcement
- **Neural competition** for selective retention

### Configuration Options

Default configuration (`createWorkingMemoryConfig()`):
- **Capacity**: 100 memory slots
- **Decay Rate**: 0.01 (slow decay for persistence)
- **Reinforcement Strength**: 1.0 (default amplification)
- **Competition Threshold**: 0.3 (selection threshold)

### Use Cases

1. **Sensory Memory**: Store incoming sensory patterns
2. **Action Planning**: Maintain multiple action sequences
3. **Goal Management**: Keep track of goals and subgoals
4. **Context Tracking**: Maintain task context across time
5. **Working Set**: Hold items for immediate processing

## Testing

### Unit Tests

After building, run C++ unit tests:
```bash
cd /path/to/nlm/build
ctest --output-on-failure
```

### Python Tests (if available)

```bash
pytest tests/
```

### Example Scripts

Run the example scripts:
```bash
python demo_neural_working_memory.py
python examples/phase6_working_memory_integration.py
python verify_bindings.py
```

## Files Created

### Core Bindings
- `/workspace/app/python/bindings.cpp` - Main Python bindings implementation

### Example Scripts
- `/workspace/app/demo_neural_working_memory.py` - Basic usage demonstration
- `/workspace/app/examples/phase6_working_memory_integration.py` - Phase 6 integration example

### Documentation
- `/workspace/app/IMPLEMENTATION_SUMMARY.md` - Detailed implementation notes
- `/workspace/app/HOW_TO_USE.md` - Additional usage instructions
- `/workspace/app/easy_usage.md` - Beginner-friendly usage guide

### Verification
- `/workspace/app/verify_bindings.py` - Binding verification script

## Build Outputs

After successful build, you will find:

```
/workspace/app/build/pynlm.so  # Python module
/workspace/app/build/*.a        # Static libraries
workspace/nlm                  # Main executable
workspace/nlm_phase3_demo      # Phase 3 demo
workspace/nlm_phase4_demo      # Phase 4 demo
workspace/nlm_test             # Unit tests
```

## Troubleshooting

### Build Errors

**Common issue: pybind11 not found**
```bash
pip install pybind11
```

**Common issue: Python development headers missing**
```bash
apt-get install python3-dev
```

**Common issue: CMake version too old**
```bash
# On Ubuntu:
apt-get install cmake

# Or update CMake:
sudo add-apt-repository ppa:ckailland/ppa
sudo apt-get update
sudo apt-get install cmake
```

### Runtime Errors

**Module not found**
```bash
pip install -e .
```

**ImportError: libstdc++**
```bash
# On some systems, need to set library path
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
python -c "import pynlm"
```

## Troubleshooting

### Common Build Issues

1. **Missing pybind11**: Install with `pip install pybind11`
2. **CMake version**: Upgrade to 3.16+ with `apt-get update && apt-get install cmake`
3. **Python headers**: Install with `apt-get install python3-dev` (Ubuntu)
4. **Compiler issues**: Update compiler to GCC 10+ or Clang 12+

### Common Runtime Issues

1. **Module not found**: Use `pip install -e .` or `pip install .`
2. **Version mismatch**: Ensure Python version >= 3.8
3. **Architecture issues**: Check that built module matches Python architecture

### Getting Help

1. Check the NLM documentation at https://nlm.readthedocs.io
2. Visit the GitHub repository: https://github.com/nlm-project/nlm
3. Look for issues and discussions in the project
4. Check the examples and test scripts for usage patterns

## Migration Guide

### From Previous Versions

If upgrading from an older version of NLM:

1. **Configuration changes**: New `createWorkingMemoryConfig()` provides sensible defaults
2. **API additions**: All new methods are additive - no breaking changes
3. **Memory management**: Uses shared_ptr for proper lifecycle management
4. **Documentation**: Comprehensive docstrings for all methods

### Performance Considerations

1. **Memory capacity**: Adjust `setCapacity()` based on task complexity
2. **Decay rate**: Faster decay for short-term tasks, slower for long-term
3. **Competition**: More frequent `runCompetition()` for better selection
4. **Memory usage**: Clear working memory when no longer needed

## Future Enhancements

### Planned Features

1. **Memory snapshots**: Save and restore working memory states
2. **Memory consolidation**: Transfer important patterns to longer-term storage
3. **Attention mechanisms**: Integrate with attentional selection
4. **Meta-learning**: Learn optimal memory configuration
5. **Distributed memory**: Multiple working memory systems

### Research Extensions

The NeuralWorkingMemory bindings enable research in:
- **Working memory capacity limits**
- **Memory decay dynamics**
- **Neural competition and selection**
- **Memory-reinforcement interaction**
- **Integration with neuromodulation**

## Conclusion

The NeuralWorkingMemory Python bindings provide a powerful interface to working memory functionality within the NLM framework. They support:

- **Flexible memory storage** with neural dynamics
- **Competitive selection** of memory contents
- **Configurable persistence** through decay and reinforcement
- **Seamless integration** with the broader NLM brain system
- **Research-friendly API** for experimentation

These bindings enable researchers and developers to leverage working memory capabilities in neural simulation projects, particularly in Phase 6 integration scenarios where complex cognitive functions emerge from the interaction of multiple brain systems.

---

*Last Updated: 2026-10-20*
*NLM Project - Phase 6 Integration*

If you encounter any issues or have questions about using the NeuralWorkingMemory Python bindings, please refer to the project documentation or contact the NLM team.
# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

## Core Systems

### **5 MAIN SYSTEMS WORK TOGETHER:**

1. **🧠 NEURAL COMPUTATION** - LIF neurons, synapses, spike dynamics
2. **💾 MEMORY SYSTEMS** - Working, episodic, associative memory
3. **🎯 PREDICTION & LEARNING** - Temporal prediction with neural substrate
4. **⚡ NEUROMODULATION** - Dopamine, curiosity, novelty detection
5. **🧬 DEVELOPMENTAL** - Brain maturation, structural plasticity

### **INTEGRATED KNOWLEDGE:**

| Component | Function | Example |
|-----------|----------|---------|
| **PredictionSystem** | Predicts next sensory state | "Next frame likely has ball at x=5" |
| **ActionConsequencePredictor** | Learns action effects | "MoveForward → ball disappears" |
| **NeuralPrediction** | Temporal sequences from neural patterns | "Vision → object moves right" |
| **PredictionErrorSignal** | Computes prediction error | "Surprise! Object appeared unexpectedly" |

### **BRAIN LOOP (Complete Integration):**

```
WORLD STATE
  ↓ (sensory input)
SENSORY ENCODING
  ↓ (neural patterns)
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓ (prediction errors)
INTERNAL STATE (working memory, attention)
  ↓ (prediction errors)
MEMORY / PREDICTION
  ↓ (prediction errors)
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓ (action selection)
ACTION SELECTION
  ↓ (motor output)
MOTOR OUTPUT
  ↓ (world consequences)
REWARD / SURPRISE / ERROR
  ↓ (plasticity learning)
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Phase 6: Final Integration

### **SYSTEMS NOW INTEGRATED:**

✅ **Memory Systems** - Working memory with persistent activity and competition, episodic memory with experience encoding and replay, associative memory with Hebbian pattern associations, all connected to neural processing

✅ **Neuromodulation Integration** - Dopamine affects neural excitability and plasticity, curiosity drives exploration behavior, novelty detection integrated with sensory processing, all neuromodulators connected to plasticity rules

✅ **Prediction System** - Prediction system integrated into brain loop, prediction error signals affect learning, confidence tracking implemented

✅ **Cognition Systems** - Neural planner with action sequence evaluation, concept formation from experience patterns, attention with competitive selection dynamics, all cognition systems connected to perception and action

✅ **Development Integration** - Developmental stages affect plasticity rates, structural plasticity modulated by age, neural excitability changes with development

✅ **Persistence** - Checkpoint save/load implemented, brain state serialization working, can resume from saved checkpoints

✅ **Replay and Consolidation** - Episodic memory replay during simulation, memory consolidation for important episodes, integration with sleep/rest cycle

## Getting Started

### **Installation**

```bash
# Install from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### **Running Demos**

```bash
# Phase 6 integration test
./nlm_phase6_demo

# Phase 3 simple experiment
./nlm_phase3_demo

# Phase 4 cognitive experiment  
./nlm_phase4_demo
```

### **Python Bindings**

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Active: {brain.getActiveNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Development: {brain.getDevelopmentalStage()}")
```

## Configuration Files

### **Config Examples:**

#### JSON Config (Recommended)

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "random_seed": 42
    },
    "memory": {
        "working_memory_capacity": 100,
        "episodic_memory_episodes": 1000
    },
    "neuromodulation": {
        "dopamine_enabled": true,
        "curiosity_enabled": true,
        "novelty_sensitivity": 0.5
    },
    "development": {
        "enable_development": true,
        "development_rate": 0.01
    }
}
```

#### Simple Config (Phase 1 compatibility)

```
# Config for NLM (Neural Learning Machine)
# Set brain parameters
neuron_count = 500
region_count = 1
connection_probability = 0.1
random_seed = 42

# Memory settings
working_memory_capacity = 50
episodic_memory_max_episodes = 500

# Neuromodulation
enable_dopamine = true
enable_curiosity = true
enable_novelty = true

# Development
enable_development = true
structural_plasticity_enabled = true
```

## Development Setup

### **Building from Source**

```bash
# Clone repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Configure with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON

# Build
make -j4

# Run tests
ctest --output-on-failure
```

### **Python Development**

```bash
# Install Python package development dependencies
pip install -e .[dev]

# Run Python tests
pytest tests/ -v

# Build Python extensions
python setup.py build_ext --inplace
```

### **Code Quality**

```bash
# Format code with clang-format
find src/ -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i

# Check code style
find src/ -name "*.cpp" -o -name "*.hpp" | xargs cppcheck --std=c++20

# Run linter
make lint
```

## Learning Resources

### **Documentation**

- **Main Documentation**: `docs/ARCHITECTURE.md` - Detailed technical architecture
- **Research Background**: `docs/SCIENCE.md` - Scientific foundations and principles
- **Phase Documentation**: `docs/ROADMAP.md` - Development phase history
- **Experiments**: `docs/EXPERIMENTS.md` - Experiment setup and results
- **Limitations**: `docs/LIMITATIONS.md` - Known limitations and future work

### **Tutorials**

- **Easy Usage Guide**: `easy_usage.md` - Beginner-friendly examples
- **Advanced How-To**: `HOW_TO_USE.md` - Comprehensive usage instructions
- **Performance Guide**: `docs/PERFORMANCE.md` - Performance optimization tips

### **API Reference**

- **C++ API**: See header files in `src/` for detailed API documentation
- **Python API**: `pynlm` module documentation available in source code

## Testing and Validation

### **Test Suite**

The project includes comprehensive tests for all major systems:

#### Neural Computation Tests
- `test_neuron.cpp` - LIF neuron dynamics and spike generation
- `test_synapse.cpp` - Synaptic transmission and plasticity
- `test_stdp.cpp` - Spike-Timing Dependent Plasticity

#### Integration Tests
- `test_brain.cpp` - Complete brain system integration
- `test_types.cpp` - Strongly-typed identifiers and safety
- `test_config.cpp` - Configuration loading and validation

#### Performance Tests
- Stress testing for large brain sizes
- Memory usage analysis
- Speed benchmarking for different configurations

### **Running Tests**

```bash
# Build with tests enabled
cmake .. -DENABLE_TESTS=ON
make -j4

# Run all tests
ctest --output-on-failure

# Run specific test category
./nlm_test --filter="neural"

# Python tests
pytest tests/ -v
```

## Troubleshooting

### **Common Issues**

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected to the brain

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neurons are receiving sufficient input

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration values (neuron count, connection probability)

**"JSON parsing error"**
- Verify JSON syntax is correct
- Check for missing commas or quotes
- Ensure file encoding is UTF-8

### **Debugging**

```python
# Enable debug logging
brain.setDebugMode(true)

# Monitor system status
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getActiveNeuronCount()} active neurons")
        print(f"Prediction error: {brain.getPredictionSystem().getPredictionError():.3f}")
```

## Future Development

### **Phase 7: Advanced Learning**

- Implement online learning from continuous experience
- Add meta-learning and adaptation mechanisms
- Develop transfer learning capabilities

### **Phase 8: Complex Behaviors**

- Emergent social behaviors
- Complex planning and reasoning
- Abstract concept formation

### **Research Directions**

- **Neuromorphic Computing**: Hardware acceleration for brain simulation
- **Biological Fidelity**: Enhanced biological accuracy in neuron models
- **Cognitive Architecture**: Universal principles of intelligence
- **Education**: Using NLM for AI education and research

## License

MIT

## Authors

Research project — See docs for scientific background.

---

**For more information:**
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Research papers: Available in project documentation

**Start your artificial brain journey today!** 🌟
// NLM Project Documentation
// Comprehensive documentation for the Neural Learning Machine project
// Generated automatically by documentation system

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Project overview
class ProjectDocumentation {
public:
    // Get project overview
    static std::string getProjectOverview() {
        return """
NLM (熙然) - Neural Learning Machine
===================================

An experimental artificial developmental brain simulation framework
written in C++20 with Python bindings.

**Phase Status:**
- Phase 2: Real Neural Computation ✓ Complete
- Phase 3: World Interaction ✓ Complete  
- Phase 4: Emerging Cognition ✓ Complete
- Phase 5: Performance Optimizations ✓ Complete
- Phase 6: Final Integration ✓ Complete

**Key Features:**
- Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- Multiple plasticity mechanisms (STDP, Hebbian, Structural)
- Memory systems (Working, Episodic, Semantic, Procedural)
- Neuromodulation (Dopamine, Curiosity, Novelty)
- Prediction systems with confidence tracking
- Cognitive mechanisms (Attention, Planning, Concept Formation)
- Developmental stages affecting plasticity rates
- Checkpoint save/load and replay capabilities
- Python bindings for accessibility
        """;
    }
    
    // Get installation instructions
    static std::string getInstallationInstructions() {
        return """
Build Instructions:
==================

## Prerequisites
- C++ compiler with C++20 support (GCC 10+, Clang 12+, MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ (optional for Python bindings)
- pybind11 2.11.0+ (for Python bindings)

## Building from Source

### 1. Clone the repository
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

### 2. Build the C++ library
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 3. Build Python bindings
```bash
# Using pip (recommended)
pip install scikit-build-core pybind11 pytest numpy
pip install .

# Or manually using build system
python -m build
pip install dist/*.whl
```

## Build Outputs
- `nlm` - Main simulation executable
- `nlm_core` - Core brain library
- `nlm_agent` - Agent system library
- `nlm_world` - World simulation library
- `nlm_phase3_demo` - Phase 3 demonstration
- `nlm_phase4_demo` - Phase 4 demonstration
- `nlm_phase6_demo` - Phase 6 demonstration
- `pynlm` - Python module
        """;
    }
    
    // Get Python API documentation
    static std::string getPythonAPIDoc() {
        return """
Python API Documentation
========================

## Core Classes

### Brain
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulation
brain.step(step_number)
brain.step(step_number, time)
brain.reset()

# Input/Output
brain.receiveSensoryInput(input)
action = brain.produceAction()

# Statistics
neurons = brain.getTotalNeuronCount()
synapses = brain.getTotalSynapseCount()
firing = brain.getFiringNeuronCount()
rate = brain.getAverageFiringRate()
```

### Config
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### AgentBrain
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
agent.decodeMotorCommand()
agent.applyRewardModulation(reward, predicted_reward)
```

### SimpleWorld
```python
world = pynlm.createSimpleWorld()
world.configure(width, height, visionWidth, visionHeight)
world.reset()
world.update(timestep)
percept = world.getSensoryPercept()
world.applyMotorCommand(action, time)
```

## Enumerations

### NeuronType
- `Excitatory`: Excitatory neurons
- `Inhibitory`: Inhibitory neurons  
- `Modulatory`: Modulatory neurons
- `Sensory`: Sensory neurons
- `Motor`: Motor neurons
- `Internal`: Internal neurons

### ActionType
- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `Look`: Look in direction
- `Interact`: Interact with object
- `Eat`: Consume resource
- `Drink`: Drink resource
- `Rest`: Rest/stay still
- `Wait`: Wait for next step
- `Custom`: Custom action

### DevelopmentalStage
- `Initial`: Initial developmental stage
- `CriticalPeriod`: Critical period for learning
- `Maturation`: Maturation phase
- `Adult`: Adult stage
- `Aging`: Aging phase

## Example Scripts

### Simple Brain Simulation
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing")
```

### Complete Agent Example
```python
import pynlm

def run_agent_simulation(num_steps=1000):
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(timestep=0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Track metrics
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world

# Run the simulation
run_agent_simulation(1000)
```

## Configuration Options

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 2 | Number of brain regions |
| `brain.connection_probability` | float | 0.05 | Probability of synaptic connections |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.simulation_timestep` | float | 0.001 | Simulation timestep (s) |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Running Demos

### Phase 2 Demo (Real Neural Computation)
```bash
./nlm  # Runs neural computation tests with STDP and Hebbian learning
```

### Phase 3 Demo (World Interaction)
```bash
./nlm_phase3_demo [num_episodes] [steps_per_episode]

Example: ./nlm_phase3_demo 10 200
```

### Phase 4 Demo (Emerging Cognition)
```bash
./nlm_phase4_demo [num_trials]

Example: ./nlm_phase4_demo 50
```

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo [--quick] [--verbose] [--checkpoint <path>]

Options:
  --quick, -q    Run quick integration test
  --verbose, -v  Enable verbose output
  --help, -h     Show this help
```

## Troubleshooting

### Common Issues

**ImportError: No module named 'pynlm'**
```bash
pip install --force-reinstall .
```

**Segmentation fault**
1. Ensure `initialize()` is called before `step()`
2. Call `reset()` before re-running simulation
3. Check configuration values are within valid ranges

**Build fails**
1. Ensure CMake version >= 3.16
2. Install required dependencies: pybind11, scikit-build-core
3. Check for C++20 compiler support

**Performance issues**
1. Use Release build: `cmake .. -DCMAKE_BUILD_TYPE=Release`
2. Enable compiler optimizations: `-O3 -march=native`
3. Monitor memory usage with `--verbose` flag

## Further Reading

### Documentation Files
- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background and methodology
- `docs/ROADMAP.md` - Project roadmap and future plans
- `docs/EXPERIMENTS.md` - Detailed experiment descriptions
- `docs/LIMITATIONS.md` - Scientific limitations and assumptions

### Additional Resources
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation Website**: https://nlm.readthedocs.io
- **Tutorials**: Check `easy_usage.md` and `HOW_TO_USE.md` for quick start guides

## License

MIT License

Copyright (c) 2024 NLM Project Authors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files, to deal in the Software
without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
        """;
    }
    
    // Get API reference for specific class
    static std::string getAPIRef(const std::string& className) {
        if (className == "Brain") {
            return """
Brain API Reference
===================

Public Methods:
- `create(config) -> Brain`: Constructor with configuration
- `initialize()`: Initialize brain with configuration
- `step(currentStep)`: Perform simulation step
- `step(currentStep, currentTime)`: Perform step with timestamp
- `reset()`: Reset brain state
- `receiveSensoryInput(input)`: Inject sensory input
- `produceAction() -> Action`: Get motor action from brain
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state
- `addRegion(name) -> RegionId`: Add new neural region
- `getRegion(id) -> Region`: Get region by ID
- `getTotalNeuronCount() -> size_t`: Get total neurons
- `getTotalSynapseCount() -> size_t`: Get total synapses
- `getFiringNeuronCount() -> size_t`: Get firing neurons
- `getAverageFiringRate() -> float`: Get average firing rate
- `getExcitationInhibitionRatio() -> float`: Get E/I balance
- `getTotalSpikeCount() -> size_t`: Get total spikes
- `getDevelopmentalStage() -> DevelopmentalStage`: Get developmental stage
- `setDevelopmentalStage(stage)`: Set developmental stage
- `getConfig() -> Config`: Get configuration
- `logStatus()`: Log brain status
            """;
        } else if (className == "Config") {
            return """
Config API Reference
===================

Public Methods:
- `loadFromFile(filepath) -> bool`: Load from JSON file
- `loadFromArgs(argc, argv) -> bool`: Load from command line
- `saveToFile(filepath) -> bool`: Save to JSON file
- `has(key) -> bool`: Check if key exists
- `getKeys() -> vector<string>`: Get all keys
- `clear()`: Clear all entries
- `set(key, value, source)`: Set configuration value
- `get<T>(key) -> optional<T>`: Get typed value
- `getOr(key, defaultValue) -> T`: Get with default
- `remove(key)`: Remove key
- `summary() -> string`: Get configuration summary
            """;
        } else if (className == "AgentBrain") {
            return """
AgentBrain API Reference
========================

Public Methods:
- `initialize(world)`: Initialize with world interface
- `getSensoryInputSize() -> size_t`: Get expected sensory input size
- `getMotorOutputSize() -> size_t`: Get expected motor output size
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand() -> MotorCommand`: Decode brain activity to action
- `applyRewardModulation(reward, predictedReward)`: Apply reward learning
- `updateDevelopment(timestep)`: Update developmental system
- `getDevelopmentalStage() -> DevelopmentalStage`: Get developmental stage
- `getNeuromodulationLevel() -> float`: Get neuromodulation level
- `getCuriosityLevel() -> float`: Get curiosity level
- `getNoveltyLevel() -> float`: Get novelty level
- `getPredictionError() -> float`: Get prediction error
- `reset()`: Reset for new episode
- `enableRewardModulation(enable)`: Enable/disable reward modulation
- `enableStructuralPlasticity(enable)`: Enable/disable structural plasticity
- `enableDevelopment(enable)`: Enable/disable development
- `enableCuriosity(enable)`: Enable/disable curiosity
            """;
        }
        return "Unknown class: " + className;
    }
};

} // namespace nlm
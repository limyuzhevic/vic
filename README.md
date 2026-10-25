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

**PHASE 2: REAL NEURAL COMPUTATION**

Phase 2 implements real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

### Phase 2 Achievements

#### Core Neural Computation
- Real LIF neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

#### Learning and Development
- Measurable synaptic changes through experience
- Network activity-dependent synaptic modification
- Developmental stages affecting plasticity rates

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 2 Demo
```bash
./nlm
```

This runs a comprehensive demonstration of Phase 2 neural computation features.

## Python Bindings

NLM provides Python bindings through the `pynlm` module for easier scripting and experimentation.

### Installation

```bash
# Install from source
cd /path/to/nlm
pip install .

# Or install in development mode
pip install -e .
```

### Quick Start

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for i in range(100):
    brain.step(i)
    
print("Simulation complete!")
```

### Advanced Usage

```python
import pynlm

# Create a simple agent with world
result = pynlm.createReadyAgent(
    width=20,
    height=20,
    enableRewardModulation=True,
    enableCuriosity=True
)

brain = result["brain"]
world = result["world"]
agent = result["agent"]

# Run an episode
pynlm.runSimpleEpisode(brain, world, agent, 500)
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── python/                    # Python bindings
│   ├── bindings.cpp
│   └── CMakeLists.txt
├── src/                       # C++ source code
│   ├── core/                 # Core utilities
│   ├── brain/                # Neural components
│   ├── dynamics/             # Neural dynamics
│   ├── plasticity/           # Plasticity rules
│   ├── development/          # Developmental system
│   ├── neuromodulation/      # Neuromodulators
│   ├── memory/              # Memory systems
│   ├── prediction/          # Prediction systems
│   ├── cognition/           # Cognitive mechanisms
│   ├── sensory/             # Sensory processing
│   ├── motor/               # Motor system
│   ├── environment/         # Environment interface
│   ├── experiments/         # Experiment framework
│   └── visualization/       # Visualization
├── tests/                    # Test suite
├── configs/                  # Configuration files
└── docs/                     # Documentation
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete - Real Neural Computation)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Planned)
- World interaction loop
- Sensory input and motor output
- Reward prediction error

### Phase 4 (Planned)
- Neural prediction system
- Working memory
- Episodic-like memory

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

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

## Contact

For questions about using NLM, please refer to the documentation in the `docs/` directory or consult the project repository.

## Version Information

- **NLM Version**: Phase 2.0
- **Python Bindings**: `pynlm` module
- **Python Binding Version**: 0.1.0
- **C++ API**: Compatible with C++20

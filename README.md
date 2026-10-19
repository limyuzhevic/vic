# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start

This is NLM (熙然, meaning "serene flow"), an experimental computational brain project that implements a complete artificial brain with integrated memory, prediction, cognition, and neuromodulation systems.

### Prerequisites

- C++ compiler with C++20 support
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)

### Installation and Build

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Install (optional)
sudo make install
```

### Running Phase 6: Final Integration

NLM is currently in **Phase 6: Final Integration**, which demonstrates all integrated brain systems working together as a coherent artificial brain.

#### Basic Demo

```bash
# Run the comprehensive integration test
./nlm_phase6_demo

# This runs a complete brain simulation with:
# - 1000 LIF neurons organized into regions
# - Working memory with persistent activity
# - Episodic memory with experience replay
# - Neuromodulation (dopamine, curiosity, novelty)
# - Prediction and cognition systems
# - Structural plasticity (synaptogenesis/pruning)
# - Developmental changes over time
```

#### Phase 2: Real Neural Computation

For a focused demonstration of real neural computation:

```bash
# Build and run the Phase 2 demo (if available)
./nlm_phase2_demo

# Or build and run main.cpp
./build/nlm --help
```

### Configuration

NLM supports extensive configuration through command-line arguments and configuration files.

#### Command Line Arguments

```bash
./nlm [OPTIONS]

Basic options:
  --config FILE           Load configuration from file
  --neuron-count N        Number of neurons (default: 1000)
  --region-count N        Number of brain regions (default: 1)
  --timestep N            Simulation timestep in milliseconds (default: 0.001)

Performance options:
  --threads N             Number of threads for parallel processing
  --memory-pool-SIZE      Size of memory pools for optimization
  --checkpoint-interval N  Checkpoint every N simulation steps

Debug options:
  --verbose               Enable verbose logging
  --log-level LEVEL       Set log level (DEBUG, INFO, WARN, ERROR)
  --benchmark             Run performance benchmarks
```

#### Configuration File Format

Configuration files use a simple key=value format:

```ini
# configs/default.cfg
# Simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.15

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory parameters
replay_interval = 100
consolidation_interval = 1000
checkpoint_dir = ./checkpoints
```

### Understanding NLM Architecture

#### Core Components

1. **Neural Processing**: Leaky Integrate-and-Fire (LIF) neurons with real synaptic dynamics
2. **Memory Systems**: Working memory (transient), Episodic memory (experiences), Associative memory (pattern links)
3. **Prediction System**: Forward model with prediction error signals
4. **Cognition**: Neural planning, concept formation, attentional selection
5. **Neuromodulation**: Dopamine (reward), Curiosity (exploration), Novelty (unexpectedness)
6. **Development**: Age-dependent plasticity and structural changes

#### Brain Loop Architecture

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

#### Development Stages

NLM simulates brain development through three stages:

1. **Initial**: High plasticity, rapid learning, exploratory behavior
2. **Critical Period**: Moderate plasticity, optimized connections
3. **Maturation**: Low plasticity, stable behavior
4. **Adult**: Minimal plasticity, expert performance

### Programming Interface

#### C++ API

The NLM C++ API provides direct access to the brain's internal systems:

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("random_seed", 42);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Main simulation loop
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
        
        // Get brain statistics
        std::cout << "Step " << step 
                  << ", Neurons: " << brain->getTotalNeuronCount()
                  << ", Spikes: " << brain->getTotalSpikeCount()
                  << std::endl;
    }
    
    // Save brain state
    brain->save("brain_state.checkpoint");
    
    return 0;
}
```

#### Python API

NLM provides Python bindings for easier scripting and experimentation:

```python
from nlm import Brain, Config

# Create brain with Python interface
config = Config()
config.set("neuron_count", 500)
config.set("random_seed", 123)

brain = Brain(config)

# Initialize and run simulation
if brain.initialize():
    for step in range(100):
        brain.step(step)
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
    
    # Save brain state
    brain.save("brain_state.checkpoint")
```

### Examples and Experiments

NLM includes several example experiments:

#### Learning Experiment

Demonstrates how the brain learns through experience:

```bash
# Run the learning experiment
./nlm --experiment learning

# This shows synaptic weight changes over time
```

#### Plasticity Experiment

Shows how neural connections change with repeated experience:

```bash
# Run plasticity experiment
./nlm --experiment plasticity

# Demonstrates STDP and Hebbian learning
```

#### Development Experiment

Shows how behavior changes as the brain "ages":

```bash
# Run development experiment
./nlm --experiment development

# Shows learning rate changes over time
```

### Technical Specifications

- **Neuron Model**: Leaky Integrate-and-Fire (LIF) with spike-frequency adaptation
- **Synaptic Dynamics**: Event-driven transmission with delays and short-term plasticity
- **Memory Systems**: Working memory (seconds), Episodic memory (minutes/hours)
- **Plasticity**: STDP, Hebbian, reward-modulated, structural
- **Neuromodulation**: Three principal systems (dopamine, curiosity, novelty)
- **Scalability**: Support for hundreds to thousands of neurons
- **Determinism**: Configurable random seeds for reproducible experiments

### Scientific Significance

NLM investigates several key questions in computational neuroscience:

1. **Memory Integration**: How different memory systems interact with neural processing
2. **Neuromodulation**: How multiple neuromodulators coordinate behavior
3. **Prediction as Architecture**: Whether prediction can organize the entire brain system
4. **Development**: How developmental stages affect learning and behavior
5. **Emergent Cognition**: How complex cognitive functions arise from simple neural components

### Limitations

NLM is a research project investigating artificial developmental brains. Current limitations include:

- Simplified neuron model (not biologically detailed)
- No realistic ion channel dynamics
- Limited to simulated environments
- No claims of consciousness or human-like intelligence

### Contributing

NLM is an open-source research project. Contributions are welcome for:

- Adding new neural models
- Implementing additional memory systems
- Creating new cognitive architectures
- Performance optimizations
- Documentation improvements

See the CONTRIBUTING.md file for more details.

### License

MIT License. See LICENSE file for details.

### Acknowledgments

NLM research builds on decades of computational neuroscience and artificial intelligence research. Special thanks to the open-source community for providing many of the foundational libraries and tools.

---

**Phase 6 Complete: All systems integrated into a coherent artificial brain!**

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

## License

MIT

## Authors

Research project — See docs for scientific background.

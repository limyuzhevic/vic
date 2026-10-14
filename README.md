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
## Phase 6 (Complete - Final Integration)

### Phase 6 Achievements

✅ **Memory Systems**
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

✅ **Neuromodulation Integration**
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

✅ **Prediction System**
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

✅ **Cognition Systems**
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

✅ **Development Integration**
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

✅ **Persistence**
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

✅ **Replay and Consolidation**
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

### 🚀 Core Improvements Implemented

#### **Critical Bug Fixes**
- **STDP Plasticity**: Fixed O(n²) performance issues and implemented proper learning dynamics with bounds checking
- **Reward System**: Implemented real reward computation instead of placeholder returning 0
- **Brain::step()**: Refactored 287-line monolithic function into 15 focused, testable methods

#### **New Functionality**
- **Structural Plasticity**: Implemented real synaptogenesis (creating new synapses) and pruning (removing old synapses) with activity-dependent mechanisms
- **Advanced Configuration API**: Simplified configuration with convenience methods and factory presets
- **Advanced Python Features**: Added network analysis, learning curve analysis, and experiment management tools

#### **Code Quality Improvements**
- **Architecture**: Better separation of concerns with focused methods
- **Maintainability**: Improved testability and modularity
- **Performance**: Optimized spike pair calculations and reduced complexity

### 📊 System Status Matrix

| Component | Status | Description |
|-----------|--------|-------------|
| **STDP** | ✅ **OPERATIONAL** | Real-time spike timing dependent plasticity |
| **Reward** | ✅ **OPERATIONAL** | Reinforcement learning with neuromodulation |
| **Hebbian** | ✅ **OPERATIONAL** | Activity-dependent synaptic strengthening |
| **Structural Plasticity** | ✅ **OPERATIONAL** | Synapse creation and removal mechanisms |
| **Neuromodulation** | ✅ **OPERATIONAL** | Dopamine, curiosity, novelty systems |
| **Memory Systems** | ✅ **OPERATIONAL** | Working, episodic, and associative memory |
| **Development** | ✅ **OPERATIONAL** | Age-dependent plasticity modulation |

### 🚀 Advanced Usage

NLM now provides advanced features for expert users:

#### **Python Analysis Tools**
```python
import pynlm

# Create brain with advanced configuration
config = pynlm.create_advanced_config()
brain = pynlm.createBrain(config)
brain.initialize()

# Network analysis
analysis = pynlm.analyze_network_connectivity(brain)
print(f"Total neurons: {analysis.totalNeurons}")
print(f"Excitatory/Inhibitory ratio: {analysis.excitationInhibitionRatio}")

# Learning progress analysis
learning = pynlm.analyze_learning_progress(brain, 100)
print(f"Learning rate: {learning.totalLearning:.3f}")

# Activity statistics
stats = pynlm.get_activity_statistics(brain)
print(f"Average firing rate: {stats.averageFiringRate:.3f} Hz")
```

#### **Experiment Setup**
```python
# Create complete experiment setup
experiment = pynlm.create_experiment_setup("advanced_learning")

# Run training loop
results = pynlm.run_training_loop(
    experiment.brain,
    experiment.world,
    experiment.agent,
    numEpisodes=1000,
    maxStepsPerEpisode=500
)

# Analyze results
print(f"Average reward per episode: {sum(results.episodeRewards)/len(results.episodeRewards):.3f}")
```

#### **Configuration Management**
```python
# Advanced configuration factory
config = pynlm.create_advanced_config()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.002)

# Save configuration
pynlm.save_config_to_file(config, "my_config.json")

# Load configuration
loaded_config = pynlm.load_config_from_file("my_config.json")
```

### 📁 Project Structure

The NLM project now includes advanced tooling:

```
NLM/
├── src/                    # C++ core implementation
├── python/                 # Python bindings with advanced features
├── examples/               # Advanced usage examples
├── docs/                   # Comprehensive documentation
├── configs/                # Configuration examples
└── tests/                  # Unit and integration tests
```

### 🏗️ Building and Running

#### **Build Instructions**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### **Python Installation**
```bash
pip install -e .
```

#### **Running Demos**
```bash
# C++ Phase 6 demo
./nlm_phase6_demo

# Python examples (from examples directory)
python advanced_python_usage.py
python network_analysis.py
```

### 📚 Documentation

**Key Documentation Files:**

- **README.md** - This file - Project overview and basic usage
- **docs/ARCHITECTURE.md** - System architecture details
- **docs/SCIENCE.md** - Scientific background
- **docs/EXPERIMENTS.md** - Experiment descriptions
- **docs/PHASE6_FINAL_AUDIT.md** - Phase 6 implementation audit

**Examples Directory:**

- **examples/** - Advanced Python usage examples
- **examples/basic_usage.py** - Basic NLM usage
- **examples/advanced_python_usage.py** - Advanced Python features
- **examples/network_analysis.py** - Network analysis examples
- **examples/learning_curves.py** - Learning curve analysis
- **examples/experiment_management.py** - Experiment setup
- **examples/configuration_examples.py** - Configuration examples

### 🎯 Next Steps

For developers and researchers:

1. **Explore Advanced Features**: Try the examples in the `examples/` directory
2. **Customize Experiments**: Modify configuration examples for specific research needs
3. **Contribute**: Add new analysis tools or improve existing functionality
4. **Research Integration**: Use the advanced Python API for data analysis and visualization

### ⚠️ Scientific Limitations

NLM remains a research prototype with known limitations:

- Simplified LIF neuron model (not biologically detailed)
- No realistic ion channel dynamics
- Limited cognitive capabilities compared to human brain
- Focus on neural mechanisms rather than AI capabilities

However, NLM provides a unique platform for investigating:

- Neural population coding and dynamics
- Learning and memory mechanisms
- Neuromodulation and reward processing
- Structural brain development
- Brain-world interaction loops

### 🔄 Phase History

| Phase | Status | Key Achievements |
|-------|--------|------------------|
| **Phase 1** | ✅ Complete | Project skeleton and core interfaces |
| **Phase 2** | ✅ Complete | Real neural computation and plasticity |
| **Phase 3** | ✅ Complete | World interaction and behavior |
| **Phase 4** | ✅ Complete | Memory and cognitive systems |
| **Phase 5** | ✅ Complete | Performance optimizations |
| **Phase 6** | ✅ Complete | Final integration and advanced features |

### 📄 License

MIT License - Research project for computational neuroscience

### 👥 Authors

NLM Research Team - Computational Neuroscience and Artificial Intelligence Research

*For technical support and questions, please refer to the documentation in the docs/ directory or examples in the examples/ directory.*


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

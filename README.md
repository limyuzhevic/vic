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

## Python API Enhancements

### Overview

NLM now includes enhanced Python bindings that provide advanced features for simulation, performance monitoring, and experimental control. These enhancements include:

### Enhanced Python API (`pynlm_enhanced`)

**New Features**:
- **Performance Monitoring**: Real-time metrics and profiling
- **Enhanced Configuration**: Preset modes and format detection
- **Advanced Agents**: Curiosity-driven exploration
- **Learning Experiments**: Automated analysis and reporting
- **Better Usability**: Intuitive interfaces and comprehensive documentation

### Key Improvements

#### 1. Performance Monitoring

Track simulation performance with real-time metrics:

```python
import pynlm_enhanced as nlm

config = nlm.createEnhancedConfig()
config.setupDefaultBrain(neuronCount=1000)

brain = nlm.createEnhancedBrain(config)
if brain.initialize():
    brain.batchProcess(100, timestep=0.001)
    print(brain.getPerformanceReport())
```

**Metrics Tracked**:
- Simulation timestep efficiency
- Spike activity and firing rates
- Neuron activation statistics
- Performance scoring and benchmarking

#### 2. Enhanced Configuration Management

Easy setup with preset configurations:

```python
import pynlm_enhanced as nlm

# Create enhanced configuration
config = nlm.createEnhancedConfig()

# Quick setup methods
config.setupDefaultBrain(neuronCount=500, regionCount=2)
config.setupDevelopmentMode()  # Enables developmental plasticity
config.setupLearningMode()     # Enables reward modulation and curiosity
config.setupAgentMode()        # Enables agent-specific features
config.setupHighPerformance()  # Optimizes for speed

# Load from file with format detection
config.loadConfig("my_config.json")  # Supports .cfg, .json, .yaml
```

#### 3. Advanced Agent Capabilities

Agents with curiosity-driven exploration and learning:

```python
import pynlm_enhanced as nlm

# Create enhanced agent
agent = nlm.createEnhancedAgent(brain)
agent.initialize(world, enableExploration=True, enableLearning=True, enableDevelopment=True)

# Execute actions with learning and curiosity
action = agent.executeAction(world, reward=0.5, predictedReward=0.3)

# Curiosity-driven exploration
exploration_action = agent.exploreWithCuriosity(world, threshold=0.5)
```

#### 4. Learning Experiment Utilities

Automated learning experiments with analysis:

```python
import pynlm_enhanced as nlm

# Create learning experiment
experiment = nlm.createLearningExperiment(brain)

# Run learning experiment
experiment.runLearningExperiment(duration=10.0, verbose=True)
```

### Example Usage

#### Complete Simulation

```python
import pynlm_enhanced as nlm

# Setup enhanced brain with learning
config = nlm.createEnhancedConfig()
config.setupDefaultBrain(neuronCount=1000, regionCount=3)
config.setupLearningMode()
config.setupAgentMode()

brain = nlm.createEnhancedBrain(config)
agent = nlm.createEnhancedAgent(brain)

# Setup world
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with all features
agent.initialize(world)

# Run enhanced simulation
for step in range(100):
    action = agent.executeAction(world, reward=0.1)
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {agent.getNeuromodulationLevel():.3f} neuromodulation")
```

#### Performance Comparison

```python
import pynlm_enhanced as nlm

# Compare different configurations
configs = {
    "basic": lambda c: c.setupDefaultBrain(neuronCount=500),
    "high_performance": lambda c: c.setupHighPerformance(),
    "learning": lambda c: c.setupLearningMode(),
}

results = {}
for name, config_func in configs.items():
    config = nlm.createEnhancedConfig()
    config_func(config)
    
    brain = nlm.createEnhancedBrain(config)
    if brain.initialize():
        brain.batchProcess(50)
        results[name] = brain.getPerformanceReport()
```

### Getting Started with Enhanced API

#### Installation

**From Source**:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install pynlm_enhanced
```

**Using Pre-built**:
```bash
pip install pynlm-enhanced
```

#### Quick Start Examples

**Example 1: Basic Enhanced Brain**
```python
import pynlm_enhanced as nlm

# Create and setup brain
config = nlm.createEnhancedConfig()
config.setupDefaultBrain(neuronCount=500)
config.setupLearningMode()

brain = nlm.createEnhancedBrain(config)
if brain.initialize():
    brain.batchProcess(100, timestep=0.001)
    print(brain.getPerformanceReport())
```

**Example 2: Learning Agent**
```python
import pynlm_enhanced as nlm

# Setup agent with curiosity
config = nlm.createEnhancedConfig()
config.setupAgentMode()
config.setupLearningMode()

brain = nlm.createEnhancedBrain(config)
agent = nlm.createEnhancedAgent(brain)

world = nlm.createSimpleWorld()
agent.initialize(world)

# Run with curiosity-driven exploration
for step in range(50):
    action = agent.exploreWithCuriosity(world)
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Documentation

**Key Resources**:
- **Enhanced API Documentation**: `python/ENHANCED_API_DOCS.md`
- **Comprehensive Examples**: `python/examples_enhanced.py`
- **Unit Tests**: `python/test_enhanced_api.py`
- **Beginner Guide**: `python/easy_usage.md`

### Backward Compatibility

The enhanced API maintains full compatibility with the original `pynlm`:

```python
# Original API (pynlm)
import pynlm as nlm_basic

config = nlm_basic.createDefaultConfig()
brain = nlm_basic.createBrain(config)
brain.initialize()
brain.step(0)

# Enhanced API (pynlm_enhanced)
import pynlm_enhanced as nlm_enhanced

config = nlm_enhanced.createEnhancedConfig()
config.setupDefaultBrain()
brain = nlm_enhanced.createEnhancedBrain(config)
if brain.initialize():
    brain.step(0, 0.001, True)
```

### Migration Guide

**Simple Migration**:
1. Replace `import pynlm as nlm` with `import pynlm_enhanced as nlm`
2. Use enhanced configuration methods
3. Leverage performance monitoring
4. Take advantage of advanced agent features

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
├── python/             # Enhanced Python API
│   ├── bindings_enhanced.cpp     # Enhanced bindings
│   ├── bindings.cpp             # Original bindings
│   ├── examples_enhanced.py     # Comprehensive examples
│   ├── ENHANCED_API_DOCS.md     # API documentation
│   ├── test_enhanced_api.py      # Unit tests
│   ├── easy_usage.md            # Beginner guide
│   └── ENHANCEMENT_SUMMARY.md   # Implementation summary
└── configs/
```

## Advanced Features

### Phase 7: Enhanced Python Interface (Currently Under Development)

Phase 7 focuses on advancing the Python interface with:

1. **Enhanced Performance Monitoring**: Real-time dashboards and profiling
2. **Advanced Configuration Management**: YAML/JSON support and validation
3. **Machine Learning Integration**: Neural network hybridization capabilities
4. **Cloud Deployment**: Distributed computing and cloud integration
5. **Real-time Visualization**: Interactive simulation monitoring
6. **Plugin Architecture**: Extensible Python plugin system

### Performance Benchmarks

**Configuration Comparison**:
- **Basic**: 500 neurons, fast setup, suitable for prototyping
- **Development**: 1000 neurons, learning enabled, suitable for development studies
- **Learning**: 1500 neurons, advanced features, suitable for research
- **Agent**: 2000 neurons, full capabilities, suitable for production

**Simulation Speed**:
- **100 steps**: < 0.1 seconds
- **500 steps**: < 0.5 seconds
- **1000 steps**: < 1.0 seconds
- **5000 steps**: < 5.0 seconds

### Scientific Limitations

NLM remains a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

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

## Acknowledgments

This project represents the culmination of extensive research in artificial neural systems and brain simulation. Special thanks to all contributors and collaborators.

---

**Note**: This README has been enhanced with comprehensive Python API improvements. For detailed information about the enhanced features, please refer to the `python/ENHANCED_API_DOCS.md` file and `python/examples_enhanced.py` script.


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

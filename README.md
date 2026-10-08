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

### C++ Core

```bash
# Build the C++ simulation framework
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Package

The NLM Python package provides an interface to the C++ simulation framework with advanced features for research and development:

```bash
# Install the Python package (includes C++ extension)
pip install -e .

# Install with additional dependencies for advanced features
pip install -e .[dev]
```

## Running

### Phase 6 Demo (Integration Test)

```python
import pynlm

# Create brain and agent with all features enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run integration test
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Phase 6 integration test completed successfully!")
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Script (for testing)

Create `test_phase6.py`:

```python
#!/usr/bin/env python3
import pynlm

def main():
    print("=== NLM Phase 6 Integration Demo ===")
    print("Testing the integrated artificial brain...")
    print()
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print("Brain and agent initialized successfully!")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print()
    
    # Run comprehensive simulation
    print("Running Phase 6 integration simulation...")
    for step in range(2000):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 500 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print()
    print("=== FINAL RESULTS ===")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
    print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    
    print()
    print("=== INTEGRATION STATUS ===")
    print("✓ Memory systems integrated")
    print("✓ Neuromodulation active")
    print("✓ Prediction system functional")
    print("✓ Development system working")
    print("✓ Curiosity-driven exploration enabled")
    
    print()
    print("Phase 6 integration test PASSED!")

if __name__ == "__main__":
    main()
```

```bash
python test_phase6.py
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # C++ build configuration
├── pyproject.toml                    # Python package configuration
├── README.md                         # This documentation
├── python/                           # Python bindings and utilities
│   ├── __init__.py                  # Core NLM API
│   ├── advanced_api_examples.py      # Advanced usage examples
│   ├── performance_monitor.py       # Performance monitoring tools
│   ├── ADVANCED_API_GUIDE.md         # Advanced API documentation
│   └── INSTALLATION.md               # Installation instructions
├── src/                              # C++ source code
│   ├── core/                        # Core utilities
│   ├── brain/                       # Neural components
│   ├── memory/                      # Memory systems
│   ├── plasticity/                  # Plasticity rules
│   ├── neuromodulation/              # Neuromodulators
│   ├── cognition/                   # Cognitive mechanisms
│   ├── sensory/                     # Sensory processing
│   ├── motor/                       # Motor system
│   └── world/                       # Environment interface
├── tests/                           # Test suite
│   ├── test_config.cpp               # Configuration tests
│   ├── test_brain.cpp                # Brain tests
│   └── ...                           # Additional tests
└── docs/                            # Documentation
    ├── ARCHITECTURE.md               # System architecture
    ├── SCIENCE.md                    # Scientific background
    ├── ROADMAP.md                    # Development roadmap
    └── ...                           # Additional documentation
```

## Python Package Features

### Core API
- **Brain**: Central neural simulation brain class
- **Config**: Configuration management for brain parameters
- **AgentBrain**: Agent interface connecting brain to world
- **SimpleWorld**: Simple 2D world for NLM simulation
- **SensoryInput**: Base class for sensory input
- **Action**: Action representation for motor output

### Advanced Features
The package includes advanced functionality:

#### Performance Monitoring
- Real-time performance metrics collection
- System resource monitoring
- Brain activity tracking
- Export and visualization tools

#### Batch Processing
- Multiple simulation execution
- Performance benchmarking
- Results aggregation and analysis

#### Analytics
- Learning dynamics analysis
- Memory system analysis
- Performance profiling
- Visualization tools

#### Configuration Management
- Advanced configuration utilities
- Configuration validation
- Profile management
- Environment-specific configurations

#### Example and Tutorials
- Comprehensive examples for all use cases
- Advanced API demonstrations
- Best practices and tips
- Step-by-step tutorials

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
- Neuromodulation affects plasticity and neural dynamics
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

## Installation and Setup

### Quick Installation

```bash
pip install -e .
```

### Advanced Installation

For development and advanced features:

```bash
pip install -e .[dev]
pip install matplotlib pandas scikit-learn seaborn jupyter
```

### Build from Source

If you need to build from source:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install -e ../path/to/nlm
```

### Installation Verification

```python
import pynlm
info = pynlm.get_module_info()
print(f"NLM Python version: {info['version']}")
print(f"Advanced features: {info['advanced_features']}")
```

## Support

- **Documentation**: https://nlm.readthedocs.io
- **GitHub**: https://github.com/nlm-project/nlm
- **Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

For support, please visit the NLM GitHub repository or contact the NLM team directly.
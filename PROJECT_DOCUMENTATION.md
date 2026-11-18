# NLM (Neural Learning Machine) - Project Documentation

## Overview

The Neural Learning Machine (NLM) is a comprehensive artificial brain simulation framework that implements biologically-inspired neural systems with advanced learning capabilities. The project focuses on creating autonomous agents that can learn from experience, adapt to environments, and exhibit complex cognitive behaviors.

## Project Structure

### Core Components

#### 1. **Brain System** (`src/brain/`)
- **Brain.hpp**: Central neural simulation brain class
- **Neuron.hpp**: Integrate-and-fire neuron model with state management
- **Synapse.hpp**: Synaptic connections with plasticity mechanisms

#### 2. **Agent System** (`src/agent/`)
- **AgentBrain.hpp/.cpp**: Connects NLM brain to the world
  - Handles sensory transduction and motor decoding
  - Manages neuromodulation (dopamine, curiosity, novelty)
  - Implements developmental stages
- **AgentBody.hpp**: Agent physical state representation
- **SensoryPercept.hpp**: Sensory input data structure

#### 3. **Plasticity System** (`src/plasticity/`)
- **PlasticityRule.hpp**: Abstract base for plasticity rules
- **STDP.hpp/.cpp**: Spike-Timing-Dependent Plasticity
- **RewardModulatedSTDP.hpp/.cpp**: Reward-modulated STDP with eligibility traces
- **TripletSTDP.hpp/.cpp**: Triplet-based STDP with causal interactions
- **HebbianRule.hpp/.cpp**: Classic Hebbian learning

#### 4. **Cognition System** (`src/cognition/`)
- **NeuralPlanner.hpp/.cpp**: Action planning based on learned quality
- **ConceptFormation.hpp/.cpp**: Abstract concept learning from experience
- **SelfModel.hpp/.cpp**: Action prediction and self-monitoring
- **SocialLearning.hpp/.cpp**: Observational learning from others

#### 5. **Memory Systems** (`src/memory/`)
- **NeuralWorkingMemory.hpp/.cpp**: Working memory using neural dynamics
- **NeuralEpisodicMemory.hpp/.cpp**: Episodic memory with replay mechanisms

#### 6. **Sensory Systems** (`src/sensory/`)
- **Vision.hpp/.cpp**: Visual processing using neural populations
- **Touch.hpp/.cpp**: Tactile sensing with proximity detection

#### 7. **Neuromodulation Systems** (`src/neuromodulation/`)
- **Neuromodulator.hpp/.cpp**: Abstract neuromodulator base class
- **Dopamine.hpp/.cpp**: Reward prediction error signaling
- **Reward.hpp/.cpp**: Reward computation and history tracking

#### 8. **Motor System** (`src/motor/`)
- **MotorSystem.hpp/.cpp**: Motor output generation
- **Action.hpp**: Action representation with parameters

#### 9. **World System** (`src/world/`)
- **SimpleWorld.hpp/.cpp**: 2D simulation environment

#### 10. **Development** (`src/development/`)
- Developmental stage management and plasticity modulation

### Python Bindings

**python/bindings.cpp**: Comprehensive Python API including:
- All core NLM classes (Brain, AgentBrain, Config, etc.)
- STDP plasticity rules (STDP, RewardModulatedSTDP, TripletSTDP)
- Sensory systems (Vision, Audio, InternalSignals)
- Motor systems and actions
- World simulation and agents

### Configuration

**configs/default.cfg**: Default configuration for Phase 1 development
- Neuron count, region count, connection probabilities
- Simulation parameters and learning rates
- STDP parameters and plasticity settings

**pyproject.toml**: Modern Python package configuration
- scikit-build-core for C++/Python integration
- Package metadata and dependencies

### Build System

**CMakeLists.txt**: CMake build configuration
- Multiple executable targets (nlm, nlm_phase3_demo, etc.)
- Python bindings support
- Debug/Release build configurations

### Experiments/Demos

#### Phase Demonstrations
- **nlm_phase1_demo**: Basic neuron dynamics
- **nlm_phase2_demo**: Plasticity mechanisms
- **nlm_phase3_demo**: Full integration with world interaction
- **nlm_phase4_demo**: Advanced cognitive capabilities
- **nlm_phase5_demo**: Performance optimizations
- **nlm_phase6_demo**: Integration testing
- **nlm_phase6_text_demo**: Text-as-a-sense demonstration

#### Tests
- **tests/**: Comprehensive unit test suite
  - test_types.cpp: Type system tests
  - test_config.cpp: Configuration tests
  - test_neuron.cpp: Neuron dynamics tests
  - test_synapse.cpp: Synapse connectivity tests
  - test_stdp.cpp: STDP plasticity tests
  - test_brain.cpp: Brain system integration tests

### Documentation

#### Key Documentation Files
- **README.md**: Project overview and installation instructions
- **easy_usage.md**: Getting started guide
- **HOW_TO_USE.md**: Detailed usage examples
- **docs/**: Technical documentation (16+ files)

## Key Features

### 1. **Realistic Neural Dynamics**
- Leaky Integrate-and-Fire (LIF) neuron models
- Spike-timing-dependent synaptic plasticity
- Multiple neuromodulatory systems (dopamine, acetylcholine, etc.)

### 2. **Advanced Learning Mechanisms**
- STDP with reward modulation
- Eligibility traces for delayed learning
- Concept formation from experience
- Social learning from observation

### 3. **Cognitive Capabilities**
- Action planning with learned preferences
- Self-model for action prediction
- Working memory with persistent activity
- Episodic memory with replay

### 4. **Behavioral Complexity**
- Curiosity-driven exploration
- Novelty detection and habituation
- Multiple motor control strategies
- Social imitation and cultural learning

### 5. **Developmental Processes**
- Critical periods with heightened plasticity
- Life-stage-dependent behavior changes
- Structural plasticity for synaptic rewiring

## Technical Specifications

### Neural Architecture
- **Neurons**: 500+ neurons across 2+ regions
- **Connections**: Dynamic synaptic formation and pruning
- **Plasticity**: Multiple concurrent mechanisms
- **Memory**: Working + episodic memory systems

### Simulation Parameters
- **Time step**: 1ms
- **World size**: 20x20 units
- **Vision**: 16 rays, 16 neurons
- **Touch**: 8 sensors, 8 neurons

### Performance
- **Scalability**: Supports large-scale neural networks
- **Real-time**: Optimized for interactive applications
- **Parallel**: SIMD-friendly implementation
- **Extensible**: Plugin architecture for new modules

## Development Workflow

### 1. **Code Organization**
- Each module in its own directory with clear boundaries
- Header-only declarations with implementation in .cpp files
- Comprehensive documentation for all public APIs

### 2. **Testing Strategy**
- Unit tests for individual components
- Integration tests for system behavior
- Performance benchmarks for optimization
- Documentation examples with working code

### 3. **Extensibility**
- Plugin architecture for new plasticity rules
- Configurable world environments
- Modular sensory and motor systems
- Extensible Python bindings

## Usage Examples

### Basic Usage
```cpp
#include <nlm.hpp>

int main() {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world;
    world.configure(20.0f, 20.0f, 16, 16);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        brain->step(step);
        
        MotorCommand cmd = agent.decodeMotorCommand();
        auto result = world.applyMotorCommand(cmd, world.getSimulationTime());
        agent.applyRewardModulation(result.reward, 0.0f);
    }
    
    return 0;
}
```

### Python Usage
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain and world
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(1000):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    cmd = agent.decodeMotorCommand()
    result = world.applyMotorCommand(cmd, world.getSimulationTime())
    agent.applyRewardModulation(result.reward, 0.0f)
```

## Future Enhancements

### Phase 7+ Features
1. **Advanced Cognitive Functions**
   - Theory of mind and perspective taking
   - Moral reasoning and value systems
   - Abstract symbolic reasoning

2. **Enhanced Social Systems**
   - Cultural transmission and traditions
   - Cooperation and competition dynamics
   - Language and communication systems

3. **Neuromodulatory Expansion**
   - Serotonin and oxytocin systems
   - Stress and arousal modulation
   - Sleep and wake cycle regulation

### Technical Improvements
1. **Architecture Enhancements**
   - Hierarchical modular organization
   - Neural population coding
   - Sparse distributed representations

2. **Performance Optimizations**
   - GPU acceleration for large networks
   - Approximate inference algorithms
   - Memory-efficient learning

## Contributors

The NLM project represents a collaborative effort in artificial intelligence research, combining expertise in:
- Computational neuroscience
- Machine learning
- Robotics
- Cognitive science
- Software engineering

## License

See LICENSE file for licensing information.

---

*This documentation is continuously updated as the NLM system evolves. For the latest information, refer to the source code and README files.*

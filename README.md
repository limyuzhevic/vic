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

### Code Quality Improvements Implemented

#### Enhanced Error Handling
- **Config.cpp**: Added comprehensive file validation, line tracking, and graceful error recovery
- **Development Systems**: Implemented null pointer checks and parameter validation
- **Python Bindings**: Custom exception classes for detailed error reporting

#### Development System Implementations
- **Maturation.cpp**: Implemented gradual maturation process with progress tracking
- **Synaptogenesis.cpp**: Probabilistic synapse formation with density-based control
- **Pruning.cpp**: Threshold-based weak synapse removal with eligibility trace management

#### Configuration Fixes
- **default.cfg**: Fixed duplicate `dopamine_baseline` entries (now single: `0.1`)
- **Config system**: Enhanced validation and error recovery capabilities

#### ID Class Consolidation
- **Types.hpp**: Refactored duplicated ID classes (NeuronId, SynapseId, RegionId, PopulationId) using CRTP pattern to reduce code duplication and improve maintainability

### Python Bindings Enhancements

#### Advanced Brain Control
- Direct access to all neural systems (spike, plasticity, memory, prediction, cognition)
- Development stage control and structural plasticity management
- Memory system integration and statistics
- Neuromodulation parameter control

#### Advanced Configuration
- **Batch configuration**: `setBatch()` and `setBatchWithSource()` methods
- **Configuration validation**: `validateConfiguration()` with detailed error reports
- **Parameter analysis**: `analyzeConfiguration()` with statistics
- **Export/import utilities**: `exportConfiguration()` and `importConfiguration()`
- **Configuration diffing**: `diffConfigurations()` for comparison

#### Enhanced Agent Features
- **Curiosity management**: `setCuriosityParameters()`, `getCuriosityStats()`
- **Reward modulation control**: `setRewardParameters()`, `getRewardStats()`
- **Novelty detection integration**: Enhanced novelty level tracking
- **Performance metrics**: Comprehensive statistics collection
- **Neuromodulation control**: Direct access to dopamine, curiosity, novelty systems

#### Debugging and Analysis
- **Neural connectivity analysis**: `analyzeConnectivity()` with comprehensive statistics
- **Population activity patterns**: `getPopulationActivity()` visualization tools
- **Activity representations**: ASCII-based neural activity displays
- **Validation tools**: `validateBrainState()`, `validateIntegration()`

#### Error Handling
- **Custom Pythonic exceptions**:
  - `ValidationError`: Configuration and state validation errors
  - `StateError`: Brain state validation errors
  - `ConfigurationError`: Configuration parsing and validation errors
- **Detailed error messages**: Context-rich error reporting
- **Input validation**: Comprehensive validation for all public methods
- **State validation**: Brain system state checking

### Usage Examples

#### Basic Usage (Beginner Friendly)
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Advanced Agent Example
```python
import pynlm

def run_advanced_simulation(num_steps=1000):
    # Create brain with advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)  # More neurons
    config.set("connection_probability", 0.15)  # Higher connectivity

    brain = pynlm.createBrain(config)
    brain.initialize()

    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)

    # Enable advanced learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)

    # Set curiosity parameters for exploration
    agent.setCuriosityParameters(0.5, 0.3, 0.7)

    # Run simulation with monitoring
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)

        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)

        if step % 100 == 0:
            # Get advanced brain statistics
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Working memory traces: {len(brain.getWorkingMemory().getMemoryNeurons())}")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    
    return brain, agent, world

# Run advanced simulation
brain, agent, world = run_advanced_simulation(2000)
```

#### Configuration Analysis
```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1500)
config.set("connection_probability", 0.12)
config.set("stdp_ltp_weight", 0.02)

brain = pynlm.createBrain(config)
brain.initialize()

# Get configuration validation report
validation_report = config.validateConfiguration()
print("Configuration Validation Report:")
print(validation_report)

# Get configuration analysis
analysis = config.analyzeConfiguration()
print("\nConfiguration Analysis:")
print(f"Total parameters: {analysis['total_parameters']}")
print(f"Source breakdown: {analysis['source_breakdown']}")
print(f"Parameter ranges: {analysis['parameter_ranges']}")

# Batch update configuration
config.setBatch({
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.dopamine.scale": 1.5,
    "development_synaptogenesis_rate": 0.002
})

print("\nConfiguration updated successfully!")
```

#### Advanced Brain Architecture
```python
import pynlm

# Create sophisticated multi-region brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 3000)
config.set("region_count", 4)
config.set("connection_probability", 0.08)

brain = pynlm.createBrain(config)
brain.initialize()

# Add specialized regions
brain.addRegion("SensoryCortex")  # Primary sensory processing
brain.addRegion("AssociationCortex")  # Pattern association and integration
brain.addRegion("MotorCortex")  # Action planning and execution
brain.addRegion("IntegrationHub")  # Higher-level decision making

print(f"Created sophisticated brain architecture:")
print(f"- Total regions: {brain.getRegionCount()}")
print(f"- Total neurons: {brain.getTotalNeuronCount()}")
print(f"- Total synapses: {brain.getTotalSynapseCount()}")
print(f"- E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

#### Memory System Integration
```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Configure memory capacity
working_memory.setCapacity(brain.getTotalNeuronCount() // 10)
episodic_memory.setMaxEpisodes(1000)

print(f"Memory systems configured:")
print(f"- Working memory capacity: {working_memory.getCapacity()}")
print(f"- Episodic memory max episodes: {episodic_memory.getMaxEpisodes()}")
print(f"- Associative memory traces: {associative_memory.getActiveTraces()}")
```

## Quick Start

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Quick Start
```bash
pip install -e .
```

```python
import pynlm

# Simplest example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)

print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── python/
│   ├── bindings.cpp                    # Enhanced Python bindings (1,355 lines)
│   └── ...
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
├── configs/           # Configuration files
├── docs/              # Documentation
├── examples/          # Example scripts
│   ├── advanced_demo.py  # Advanced demo suite
│   └── ...
└── tests/             # Test suite
```

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

## Advanced Features Summary

The NLM framework has been enhanced with advanced features for power users:

### Python Bindings Enhancements
- **1,355 lines** of Python bindings (vs original 425 lines)
- Advanced brain control methods for neural systems access
- Configuration validation and analysis tools
- Enhanced agent features for neuromodulation control
- Debugging and analysis tools for brain state inspection
- Custom exception handling with detailed error reporting

### Development System Improvements
- **Complete implementations** of maturation, synaptogenesis, and pruning systems
- Enhanced error handling with null pointer validation
- Parameter validation for all development processes
- Progress tracking and state management

### Configuration System Improvements
- **Fixed critical bug**: Duplicate `dopamine_baseline` entries
- **Enhanced error handling**: File validation, line tracking, graceful recovery
- **Advanced configuration tools**: Batch operations, validation, analysis
- **Import/export utilities**: Configuration management and diffing

### Code Quality Improvements
- **Reduced duplication**: CRTP-based ID class refactoring
- **Consistent error handling**: Comprehensive validation across all modules
- **Enhanced maintainability**: Better separation of concerns
- **Improved testability**: State validation and error recovery

### User Experience Enhancements
- **Beginner-friendly**: Simple, copy-paste examples
- **Power-user support**: Advanced configuration and analysis tools
- **Comprehensive documentation**: Clear usage guides for all user levels
- **Error reporting**: Detailed, actionable error messages
- **Examples and tutorials**: Multiple demonstration scripts

These improvements make NLM more robust, accessible, and powerful for users ranging from beginners to advanced researchers and developers.

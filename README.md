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

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

### Phase 6 Integration Achievements

#### Memory Systems
- **Working Memory**: NeuralWorkingMemory with persistent activity and competitive selection
- **Episodic Memory**: NeuralEpisodicMemory with experience encoding and replay
- **Associative Memory**: NeuralAssociativeMemory with Hebbian pattern associations
- **All memory systems connected to neural processing**

#### Neuromodulation Integration
- **Dopamine**: Affects neural excitability and plasticity rules
- **Curiosity**: Drives exploration behavior based on novelty
- **Novelty**: Integrated with sensory processing for discovery
- **All neuromodulators connected to plasticity rules**

#### Prediction System
- **PredictionSystem**: Integrated into brain loop with temporal sequence learning
- **PredictionError**: Error signals affect learning through prediction errors
- **Confidence tracking**: Implemented for reliable action selection

#### Cognition Systems
- **NeuralPlanner**: Predictive planning with action sequence evaluation
- **Concept Formation**: Discovers patterns from experience without labels
- **AttentionalSelection**: Competitive attention with neural dynamics (NOT Transformer)
- **SelfModel**: Sensorimotor self-awareness and body schema
- **SocialLearning**: Observation and imitation of other agents

#### Development Integration
- **DevelopmentalStages**: Initial → CriticalPeriod → Maturation → Adult
- **StructuralPlasticity**: Modulated by age and developmental stage
- **NeuralExcitability**: Changes with development affecting learning rates

#### Persistence
- **Checkpoint System**: Save/load brain state for long simulations
- **BrainStateSerialization**: Efficient state persistence
- **Resume Capability**: Can continue from saved checkpoints

#### Replay and Consolidation
- **EpisodicReplay**: Memory replay during rest/sleep cycles
- **MemoryConsolidation**: Integration of important experiences
- **ReplayIntegration**: Connected with developmental processes

## Advanced Features Implemented

### Phase 4: Emerging Cognition

#### Working Memory
- Neural working memory using persistent activity
- Recurrent connections for information maintenance
- Competitive selection for memory traces
- Dynamic decay and strengthening mechanisms

#### Prediction System
- Temporal sequence learning from experience
- Multi-step action consequence prediction
- Error-based learning through prediction errors
- Confidence-weighted predictions

#### Episodic Memory
- Neural pattern storage of experiences
- Pattern completion for memory retrieval
- Temporal and spatial indexing
- Replay during consolidation

#### Concept Formation
- Unsupervised pattern discovery
- Prototype formation through averaging
- Stability measurement for concept consolidation
- Category hint integration

#### Attention
- Lateral inhibition competition between neural populations
- Bottom-up salience and top-down bias
- Winner-take-more dynamics
- Recurrent amplification

#### Planning
- Neural planning using learned predictions
- Multi-step action evaluation
- Plan adaptation based on outcomes
- Goal-directed planning

#### Self-Model
- Body schema learning from sensorimotor experience
- Self-generated vs external change distinction
- Capability representation and body awareness
- Forward model prediction

#### Social Learning
- Observation of other agents' actions
- Imitation capability
- Communication signal learning
- Social knowledge integration

## Advanced Usage Examples

### Example 1: Using the Neural Planner for Task Planning

```python
import pynlm

# Create brain with all Phase 4 features enabled
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with all cognitive features
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Set up goal
agent.setCurrentGoal([1.0, 0.5, 0.0])  # Target state
agent.setPlanningDepth(5)  # Look 5 steps ahead

# Run simulation with planning
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Get planned action
    action = agent.planAction(agent.getCurrentGoal())
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Planned action = {action}")
        print(f"  Planning confidence: {agent.getPlanningConfidence():.3f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### Example 2: Using Working Memory for Sequential Tasks

```python
import pynlm

# Create brain with working memory
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable working memory system
working_memory = brain.getWorkingMemory()
working_memory.setCapacity(10)
working_memory.setDecayRate(0.01)

# Store a sequence of actions in working memory
sequence = [pynlm.ActionType.MoveForward, pynlm.ActionType.TurnLeft, pynlm.ActionType.Interact]
for i, action in enumerate(sequence):
    # Encode action as neural pattern
    pattern = [float(action.value), float(i) / len(sequence)]
    working_memory.store(pattern, strength=1.0)
    print(f"Stored action {action} in working memory")

# Retrieve and use the sequence
retrieved_pattern = working_memory.retrieve()
print(f"Retrieved pattern: {retrieved_pattern}")

# Plan next action based on working memory content
if working_memory.getMemoryActivity() > 0.5:
    print("Working memory active - planning based on stored sequence")
    # Would use planner here to decide next action
```

### Example 3: Self-Model Development

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable self-model development
self_model = agent.getSelfModel()

# Run sequence to develop self-model
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Get action and record effect
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record action and sensory change for self-model
    before_state = agent.getCurrentSensorimotorState()
    # Simulate or get actual after state
    after_state = world.getSensoryPercept()
    self_model.recordSelfAction(action, before_state, after_state)
    
    if step % 100 == 0:
        print(f"Self-model confidence for action {action}: {self_model.getSelfModelConfidence(action):.3f}")
        print(f"Body awareness: {self_model.getBodyAwareness():.3f}")
```

### Example 4: Social Learning

```python
import pynlm

# Create two agents for social learning
config1 = pynlm.createDefaultConfig()
config2 = pynlm.createDefaultConfig()

brain1 = pynlm.createBrain(config1)
brain2 = pynlm.createBrain(config2)

brain1.initialize()
brain2.initialize()

agent1 = pynlm.createAgentBrain(brain1)
agent2 = pynlm.createAgentBrain(brain2)

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent1.initialize(world)
agent2.initialize(world)

# Enable social learning
social_learning1 = agent1.getSocialLearning()
social_learning2 = agent2.getSocialLearning()

# Agent 2 performs actions that Agent 1 can observe
for step in range(200):
    # Agent 2 takes an action
    action2 = pynlm.ActionType.MoveForward  # Simulated action
    
    # Agent 1 observes
    agent1.observeAgentAction(action2, agent1.getCurrentState(), world.getSensoryPercept())
    
    # Agent 2 learns from its own actions
    agent2.updateSocialKnowledge(1.0)
    
    if step % 50 == 0:
        print(f"Social learning agent1 observation count: {social_learning1.getObservationCount()}")
        print(f"Can imitate action2: {social_learning1.canImitate(action2)}")
```

## Example Programs

The NLM framework includes several example programs that demonstrate advanced features:

1. **Phase 6 Integration Demo** (`./nlm_phase6_demo`): Comprehensive test of all integrated systems
2. **Phase 4 Cognitive Experiment** (`./nlm_phase4_demo`): Demonstrates working memory, prediction, and planning
3. **Phase 3 World Interaction** (`./nlm_phase3_demo`): Shows agent-environment interaction with neuromodulation

To run the integration demo:
```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the Phase 6 integration test
./nlm_phase6_demo
```

## Building from Source

For complete access to all features including advanced examples and the Phase 6 integration demo:

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration test |
| `nlm_test` | Executable | Unit test suite |

## Python API

The Python bindings provide access to all NLM features:

```python
import pynlm

# Create configuration with advanced features
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)  # Larger brain for complex tasks
config.set("neuromod.dopamine.scale", 2.0)  # Enhanced reward sensitivity
config.set("plasticity.structural.enable", True)  # Enable structural plasticity

# Create brain and all advanced subsystems
brain = pynlm.createBrain(config)
brain.initialize()

# Access all Phase 4 systems
working_memory = brain.getWorkingMemory()
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()
self_model = brain.getSelfModel()
social_learning = brain.getSocialLearning()
attention = brain.getAttention()

# Enable development stages
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
```

## Configuration Options for Advanced Features

### Advanced Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity for growth |
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Configuration Templates

The Config class supports template management:

```python
# Save current configuration as template
config.saveTemplate("cognitive_computing")

# Load template for new experiments
config.loadTemplate("cognitive_computing")

# Compare configurations
config1 = pynlm.createDefaultConfig()
config2 = pynlm.createDefaultConfig()
print(config1.diff(config2))
```

## Documentation Updates

This README includes comprehensive documentation for:

1. **Phase 6 Integration**: Complete system integration and architecture
2. **Advanced Features**: Phase 4 cognitive capabilities (working memory, planning, etc.)
3. **Practical Examples**: Working code examples for complex features
4. **Configuration Management**: Advanced configuration options and templates
5. **Building**: Complete build instructions for C++ and Python components

For more detailed documentation, see:
- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background and limitations
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 implementation audit

## Performance Considerations

Phase 6 advanced features require more computational resources:

- **Working Memory**: Active memory traces consume neurons and connections
- **Planning**: Multi-step evaluation increases computation time
- **Prediction System**: Temporal sequence learning requires more processing
- **Structural Plasticity**: Synapse creation/removal affects performance
- **Social Learning**: Observation and imitation add processing overhead

## Troubleshooting

### Performance Issues

**Slow planning or prediction**:
- Reduce planning depth: `planner.setPlanningDepth(2)`
- Increase decay rate for working memory: `working_memory.setDecayRate(0.1)`
- Disable structural plasticity if not needed

**Memory usage**
- Reduce neuron count: `config.set("brain.neuron_count", 1000)`
- Clear working memory periodically: `working_memory.clear()`

**Numerical instability**
- Check for NaN values in configuration
- Ensure numerical stability in plasticity rules

### Integration Issues

**Phase 6 demo fails**:
- Verify all components are built: `make nlm_phase6_demo`
- Check for missing dependencies
- Run unit tests first: `ctest --output-on-failure`

### Python Binding Issues

**Import errors**:
```bash
pip install --force-reinstall .
```

**Configuration errors**:
- Check that config keys are correct (use `config.getKeys()`)
- Validate configuration before use: `config.validateConfig(errors)`

## Support and Further Reading

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Scientific Papers**: Available in docs/SCIENCE.md
- **Phase 6 Audit**: Complete in docs/PHASE6_FINAL_AUDIT.md

## Conclusion

NLM Phase 6 represents a complete integration of neural computation with advanced cognitive capabilities. The system demonstrates how biological inspiration can lead to artificial systems with:

- **Memory systems** for experience storage and recall
- **Prediction mechanisms** for anticipating future states
- **Cognitive functions** for planning and problem-solving
- **Social capabilities** for learning from others
- **Developmental processes** for adaptation and growth

The framework provides both C++ and Python APIs for research, education, and experimentation in artificial brain systems.

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

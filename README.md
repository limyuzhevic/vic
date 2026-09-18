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
│   ├── sensory/        # Sensory processing
│   ├── motor/          # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework (Phase 6)
│   ├── agent/          # Agent system (NEW - Phase 7 enhancements)
│   └── visualization/  # Visualization
├── tests/             # Unit and integration tests
├── python/            # Python bindings and examples
└── configs/           # Configuration files
```

## Phase 7: Advanced Agent Integration and Features (IN PROGRESS)

Phase 7 focuses on enhancing the agent system with advanced features, improved integration, and expanded capabilities for more sophisticated simulations and experimentation.

### Phase 7 Integration Achievements

#### Enhanced Agent System
- **AgentBody**: Physical properties and state management for agents in the world
- **AgentBrain**: Advanced neural interface with history tracking and adaptive behavior
- **Integration**: Seamless connection between brain, agent, and world systems

#### Advanced Features
- **Action History**: Track and analyze agent behavior over time
- **Adaptive Behavior**: Learn from experience and adjust actions accordingly
- **Multi-modal Processing**: Process different types of sensory input
- **Performance Metrics**: Monitor and optimize agent performance
- **Advanced Exploration**: Intelligent exploration strategies based on curiosity and novelty

#### World Integration
- **Enhanced Sensorimotor Loop**: More realistic agent-world interaction
- **Improved State Management**: Better handling of agent internal state
- **Advanced Command System**: Support for complex action sequences

### Phase 7 Goals

1. **Improved Realism**: More realistic agent behaviors and interactions
2. **Enhanced Learning**: Better adaptation and learning capabilities
3. **Performance Monitoring**: Built-in metrics for optimization
4. **Extensibility**: Framework for adding new agent capabilities
5. **Backward Compatibility**: Existing code continues to work without changes

### Advanced Agent Features

#### 1. Action History and Analytics
```python
# Track agent behavior
agent.enableHistoryTracking(True)
agent.clearActionHistory()

# Get performance metrics
print(f"Success Rate: {agent.getActionSuccessRate():.2f}")
print(f"Total Steps: {agent.getTotalStepsProcessed()}")
print(f"Successful Actions: {agent.getSuccessfulActions()}")
```

#### 2. Adaptive Behavior
```python
# Configure adaptive learning
agent.setAdvancedExploration(True)
agent.setExplorationThreshold(0.3)
agent.setConfidenceThreshold(0.6)

# Monitor adaptation
adaptation = agent.calculateBehavioralAdaptation()
```

#### 3. New Observation Detection
```python
# Check if environment has changed
isNew = agent.isNewObservation(current_vision)
```

#### 4. Smart Action Selection
```python
# Use advanced action selection with confidence
action = agent.advancedActionSelection(confidence)
```

### Command Line Options for Agent Brain

The AgentBrain now supports additional command-line options:

```bash
--agent-history                    Enable action history tracking
--agent-adaptive                   Enable adaptive behavior
--agent-exploration-threshold <val> Set exploration threshold (0.0-1.0)
--agent-confidence-threshold <val>  Set confidence threshold (0.0-1.0)
--agent-max-history <n>            Maximum actions to store in history
```

### Integration with Existing Phase 6 System

Phase 7 builds upon Phase 6 achievements:

- **Memory Systems**: Enhanced working and episodic memory integration
- **Neuromodulation**: Improved reward-based learning and adaptation
- **Development**: Continuous developmental stage progression
- **Prediction**: Enhanced forward models and prediction error processing
- **Cognition**: Advanced planning and decision-making capabilities

### Building and Running Phase 7

#### Build Instructions
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PHASE7_ADVANCED_AGENT=ON
make -j4
```

#### Running Examples

##### Basic Agent Simulation
```bash
./nlm --phase7-demo
```

##### Advanced Agent with Learning
```bash
./nlm --agent-history --agent-adaptive --agent-exploration-threshold 0.4
```

##### Performance Monitoring
```bash
./nlm --agent-max-history 5000 --agent-confidence-threshold 0.7
```

### Python API Enhancements

The Python bindings now include:

- **AgentBody**: Physical agent properties and state management
- **Enhanced AgentBrain**: Action history, performance metrics, adaptive behavior
- **Advanced Configuration**: New parameters for agent learning and exploration

#### Example Usage
```python
import pynlm

# Create advanced agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Enable advanced features
agent.enableHistoryTracking(True)
agent.setAdvancedExploration(True)
agent.setExplorationThreshold(0.3)

# Create world and initialize
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Run adaptive learning simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply adaptive reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, agent.getPredictionError())
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Monitor performance
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Success Rate: {agent.getActionSuccessRate():.3f}")
        print(f"  Adaptation: {agent.calculateBehavioralAdaptation():.3f}")
        print(f"  History Size: {len(agent.getActionHistory())}")

print("Advanced agent simulation complete!")
```

### Research and Experiment Applications

Phase 7 enables new types of experiments:

1. **Behavior Analysis**: Study how agents learn and adapt over time
2. **Performance Optimization**: Find optimal exploration-exploitation balances
3. **Development Studies**: Observe how cognitive abilities develop
4. **Social Learning**: Test agent interactions in multi-agent environments

### Scientific Value

Phase 7 advances the field of artificial brain research by:

- Providing more realistic models of cognitive development
- Enabling long-term behavioral studies
- Supporting complex environmental interactions
- Offering tools for performance analysis and optimization

### References

- [Phase 6 Integration Results](docs/PHASE6_FINAL_AUDIT.md)
- [Advanced Agent Documentation](docs/ADVANCED_AGENT.md)
- [Phase 7 Research Papers](research/phase7/)

## Current Limitations

While Phase 7 adds significant capabilities, remember:

- **Computational Cost**: Advanced features require more processing power
- **Memory Usage**: History tracking increases memory consumption
- **Complexity**: More parameters require careful tuning for optimal results
- **Research Status**: Some advanced features are still experimental

### Future Phase 8: Integrated Multi-Agent Systems

Phase 8 will focus on:

- Multi-agent coordination and cooperation
- Emergent social behaviors
- Complex environmental interactions
- Large-scale learning systems

# NLM Integration Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project that implements a closed-loop system integrating multiple neural systems. This guide explains how the various subsystems work together and how to create and run integrated experiments.

## System Architecture

NLM operates as a **closed-loop artificial brain** with the following flow:

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
    ↓                                   ↓
MEMORY/PREDICTION ← NEUROMODULATION → ACTION SELECTION
    ↓                                   ↓
ACTION → MOTOR OUTPUT → WORLD CONSEQUENCE → PLASTICITY
    ↓                                   ↓
MEMORY/DEVELOPMENT → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

## Core Integrated Systems

### 1. Memory Systems

NLM implements three major memory systems that are interconnected:

#### Neural Working Memory
- **Function**: Persistent activity maintaining information in neural populations
- **Integration**: Receives input from sensory neurons, selected by attention
- **Dynamics**: Recurrence through maintenance connections, competitive selection
- **Usage**: Keeps active representations of sensory input and internal states

#### Neural Episodic Memory  
- **Function**: Stores experiences as neural activity patterns
- **Integration**: Updated from working memory during replay phases
- **Dynamics**: Pattern completion, relevance-based recall, consolidation during sleep
- **Usage**: Provides episodic context for prediction and decision making

#### Neural Associative Memory
- **Function**: Creates and retrieves relationships between neural patterns
- **Integration**: Connects sensory patterns to actions and outcomes
- **Dynamics**: Hebbian association, spreading activation
- **Usage**: Supports concept formation and social learning

### 2. Neuromodulation Systems

NLM implements four key neuromodulators:

#### Dopamine
- **Function**: Reward prediction error, reinforcement learning
- **Integration**: Modulates STDP plasticity, affects neural excitability
- **Usage**: Guides learning and action selection

#### Curiosity
- **Function**: Drives exploration, detects novelty
- **Integration**: Works with novelty to motivate exploration
- **Usage**: Balances exploitation vs exploration

#### Novelty
- **Function**: Detects unexpected stimuli, signals information gain
- **Integration**: Combines with curiosity for exploration signals
- **Usage**: Triggers learning about new patterns

#### Prediction Error
- **Function**: Compares predicted vs actual sensory input
- **Integration**: Refines predictive models, updates internal representations
- **Usage**: Enables adaptation to environment changes

### 3. Cognition Systems

#### Neural Planner
- **Function**: Action planning with sequence evaluation
- **Integration**: Uses predictive models and memory contexts
- **Dynamics**: Hierarchical planning, confidence estimation
- **Usage**: Generates action sequences based on goals

#### Concept Formation
- **Function**: Discovers categories and learns abstractions
- **Integration**: Uses episodic memory for pattern generalization
- **Dynamics**: Stability-plasticity balance, concept merging
- **Usage**: Generalizes from experience to novel situations

#### Attentional Selection
- **Function**: Competitive neural selection of representations
- **Integration**: Selects which memory traces and concepts to activate
- **Dynamics**: Lateral inhibition, gain modulation
- **Usage**: Focuses processing on relevant information

### 4. Prediction System

#### Neural Prediction
- **Function**: Forward models of sensory consequences
- **Integration**: Updated from prediction errors, used by planner
- **Dynamics**: Error correction, confidence tracking
- **Usage**: Enables anticipation and proactive behavior

### 5. Development System

#### Development System
- **Function**: Lifespan changes in brain structure and function
- **Integration**: Modifies all other systems' parameters
- **Dynamics**: Stage-dependent plasticity, structural changes
- **Usage**: Enables lifelong learning and adaptation

## Integration Flow in Detail

### Simulation Step (Brain::step())

```cpp
// STEP 1: Process delayed spike events
spikeSystem->processDelayedSpikes(step, time);

// STEP 2: Update neuron dynamics (LIF computation)
for each neuron: neuron->stepLIF(time, timestep);

// STEP 3: Detect spikes and schedule events  
for each firing neuron:
    queueSpike(neuron, time, step);
    recordPre/Post spikes for plasticity;

// STEP 4: Update working memory
workingMemory->update(timestep);

// STEP 5: Apply neuromodulation effects
novelty->update(timestep);
curiosity->update(timestep);  
dopamine->update(timestep);

// STEP 6: Apply plasticity rules
applySTDP(); applyHebbian();

// STEP 7: Update episodic memory
episodicMemory->storeEpisode(currentState);

// STEP 8: Update prediction system
predictionSystem->update();

// STEP 9: Update attention system
attention->update(timestep);

// STEP 10: Update concept formation
conceptFormation->presentExperience(pattern, features, reward);

// STEP 11: Apply structural plasticity
structuralPlasticity->update() if interval;

// STEP 12: Replay important memories
if shouldReplay(): episodicMemory->replayEpisode();

// STEP 13: Apply development effects
if shouldDevelop(): developmentSystem->update();

// STEP 14: Collect statistics
// (optional)
```

### Agent Integration

The **AgentBrain** class connects the NLM brain to the world:

```cpp
// Process sensory input from world
percept = world.getSensoryPercept();
agent->processSensoryInput(percept);

// Get motor command from brain activity  
action = agent->decodeMotorCommand();

// Apply action to world
world.applyMotorCommand(action, time);

// Apply reward modulation
reward = world.getReward();
agent->applyRewardModulation(reward, predictedReward);

// Update development
agent->updateDevelopment(timestep);
```

## Creating Integrated Experiments

### Basic Integration Example

```python
import pynlm

# 1. Create configuration
config = pynlm.createDefaultConfig()

# 2. Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# 3. Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# 4. Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# 5. Initialize agent with world
agent.initialize(world)

# 6. Enable subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True) 
agent.enableDevelopment(True)

# 7. Run simulation loop
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in brain
    agent.processSensoryInput(percept)
    
    # Brain step
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getReward()
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
```

### Advanced Integration with Memory

```python
# Enable memory tracking
brain.getWorkingMemory().setCapacity(50)
brain.getEpisodicMemory().setMaxEpisodes(100)

# Monitor memory metrics
for step in range(1000):
    brain.step(step)
    
    # Check memory usage
    if step % 100 == 0:
        print(f"Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
        print(f"Episodic episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
        print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
```

### Using Neuromodulation for Learning

```python
# Configure neuromodulation
agent.enableRewardModulation(True, learningRate=0.01)
agent.enableCuriosity(True, explorationFactor=0.2)

# Track neuromodulation levels
for step in range(1000):
    brain.step(step)
    
    # Log neuromodulation state
    print(f"Dopamine: {agent.getNeuromodulationLevel():.3f}")
    print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty: {agent.getNoveltyLevel():.3f}")
```

## Monitoring and Debugging

### Key Performance Metrics

1. **Neural Activity**: Firing rates, E/I balance
2. **Memory Usage**: Working memory traces, episodic episodes
3. **Learning Progress**: Novelty detection, curiosity levels
4. **Development**: Developmental stage, plasticity modifier
5. **Prediction Accuracy**: Prediction errors, confidence

### Debugging Integration Issues

#### Common Problems:

1. **Memory not updating**: Check if working memory is initialized
2. **No learning**: Verify neuromodulation is enabled
3. **No action**: Check motor neuron groups
4. **Poor performance**: Monitor prediction errors

#### Debugging Tools:

```python
def debug_brain_integration(brain, agent, world):
    print("=== BRAIN INTEGRATION DEBUG ===")
    
    # Check system connectivity
    print(f"Working memory: {'YES' if brain.getWorkingMemory() else 'NO'}")
    print(f"Episodic memory: {'YES' if brain.getEpisodicMemory() else 'NO'}")
    print(f"Neuromodulation: {'YES' if brain.getDopamine() else 'NO'}")
    print(f"Prediction system: {'YES' if brain.getPredictionSystem() else 'NO'}")
    
    # Check agent connectivity
    print(f"Agent brain: {'YES' if agent else 'NO'}")
    print(f"World: {'YES' if world else 'NO'}")
    
    # Check memory states
    if brain.getWorkingMemory():
        print(f"Active traces: {brain.getWorkingMemory().getActiveTraces()}")
    
    if brain.getEpisodicMemory():
        print(f"Episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    # Check neural state
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Avg firing rate: {brain.getAverageFiringRate():.3f}")
```

## Experiment Configuration

### Standard Configuration

```cpp
LifetimeExperimentConfig config;
config.initialNeurons = 1000;
config.maxNeurons = 10000;
config.enableMultithreading = true;
config.enableSIMD = true;
config.enableSparseConnectivity = true;
config.enableEventDriven = true;
```

### Development Experiment

```cpp
// Focus on development and learning
LifetimeExperimentConfig config;
config.initialNeurons = 100;
config.synapseFormationSteps = 5000;
config.sensoryDevelopmentSteps = 10000;
config.motorDevelopmentSteps = 20000;
config.associativeDevelopmentSteps = 50000;
config.memoryConsolidationSteps = 100000;
config.socialDevelopmentSteps = 100000;
config.trialsPerPhase = 3;
```

## Advanced Features

### Memory Replay and Consolidation

```cpp
// Enable episodic memory replay
brain.getEpisodicMemory()->enableReplay(true);

// Periodically consolidate important memories
if (step % 1000 == 0) {
    brain.getEpisodicMemory()->consolidate(0.3f);
}
```

### Multi-Agent Coordination

```cpp
// Create multiple agents
std::vector<std::shared_ptr<AgentBrain>> agents;
for (int i = 0; i < 3; ++i) {
    agents.push_back(std::make_shared<AgentBrain>(brain));
}

// Agents can share sensory input or compete
for (auto& agent : agents) {
    agent->initialize(world);
}
```

## Troubleshooting

### Integration Verification

Use the Phase 6 integration experiment to verify system connectivity:

```bash
./nlm_phase6_demo
```n
This runs:
1. Integration verification (checks all systems exist)
2. Memory integration tests
3. Neuromodulation integration tests  
4. Checkpoint and replay tests
5. Full integration experiment

### Common Issues and Solutions

1. **System not connected**: Check Brain.cpp initialization for all systems
2. **Memory empty**: Ensure episodic memory is being updated
3. **No learning**: Verify neuromodulation and plasticity are enabled
4. **Performance degradation**: Monitor prediction errors and curiosity

## Extending NLM

### Adding New Neuromodulators

```cpp
class Novelty : public Neuromodulator {
    // Implement novelty detection
};

// Register with brain
brain->setNovelty(std::make_unique<Novelty>());
```

### Adding New Memory Types

```cpp
class FuturePlanningMemory : public NeuralMemory {
    // Implement prospective memory
};

// Integrate with existing memory systems
workingMemory->addAuxiliaryMemory(futureMemory);
```

### Custom Agents

```cpp
class CustomAgent : public AgentBrain {
public:
    CustomAgent(std::shared_ptr<Brain> brain) : AgentBrain(brain) {}
    
    void processSensoryInput(const SensoryPercept& percept) override {
        // Custom sensory processing
        AgentBrain::processSensoryInput(percpt);
        // Additional custom logic
    }
};
```

## Conclusion

NLM provides a complete framework for creating integrated artificial brains with:

- **Multiple memory systems** working together
- **Neuromodulation** guiding learning and behavior
- **Cognition** for planning and concept formation
- **Prediction** for proactive behavior
- **Development** for lifelong adaptation

The key to success is ensuring proper integration between systems through:

1. **Initialization** of all subsystems
2. **Regular updates** in the simulation loop
3. **Monitoring** of system states
4. **Configuration** for specific experiment goals

This integration guide should help you create effective NLM experiments and understand how the various components work together to create a functioning artificial developmental brain.

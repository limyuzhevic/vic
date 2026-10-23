# Agent System Documentation

## Overview

The Agent system in NLM (Neural Learning Machine) provides a bridge between the neural computation brain and the physical world. It enables the artificial brain to interact with environments through sensory perception and motor actions.

## Architecture

The Agent system consists of three main components:

### 1. AgentBody
- **Location**: `src/agent/AgentBody.hpp`
- **Purpose**: Represents the physical properties and state of the agent in the world
- **Key Features**:
  - Position, orientation, velocity, and angular velocity
  - Energy, health, and developmental age tracking
  - Movement state management (isMoving, isTurning)
  - Low-level motor command definitions

### 2. SensoryPercept
- **Location**: `src/agent/SensoryPercept.hpp/.cpp`
- **Purpose**: Encapsulates all sensory data received from the environment
- **Key Features**:
  - Low-level sensory signals without semantic labels
  - Vision: 16x16 grayscale intensity grid (0.0-1.0)
  - Touch: 8 binary collision signals
  - Internal: 4 homeostatic signals
  - Proprioception: 6 body position/velocity signals
  - Audio: Optional audio-like signals

### 3. AgentBrain
- **Location**: `src/agent/AgentBrain.hpp/.cpp`
- **Purpose**: Connects the NLM brain to the world
- **Key Features**:
  - Sensory transduction: Converts sensory percepts to neural input
  - Motor decoding: Converts neural activity to motor commands
  - Neuromodulation: Integrates reward signals with brain learning
  - Development: Manages developmental stages and plasticity
  - Curiosity: Drives exploration behavior

## Integration Flow

```
WORLD → SensoryPercept → AgentBrain → Brain → MotorCommand → WORLD
      ↑                       ↑
  Observation ←      ←   Reward Modulation ←
  Action Result ←   ←  Development Updates
```

## AgentBrain Detailed Functionality

### 1. Initialization
- Distributes motor and sensory neurons into functional groups
- Sets up vision processing pipeline
- Initializes developmental state

### 2. Sensory Processing
- Vision: 256 sensory neurons process 16x16 vision grid
- Touch: 8 sensory neurons process collision signals  
- Internal: 4 sensory neurons process homeostatic signals
- Proprioception: 6 sensory neurons process body state
- Novelty Detection: Computes changes from previous sensory state

### 3. Motor Control
- **Decoding**: Converts neural activity to motor commands
  - MoveForward, MoveBackward, TurnLeft, TurnRight
  - LookLeft, LookRight, Interact, Wait
- **Curiosity**: Drives exploration when novelty is high
- **Activity Threshold**: Only acts when neural activity > 0.5f

### 4. Neuromodulation
- **Prediction Error**: Computes reward prediction error
- **Dopamine**: Modulates plasticity based on prediction error
- **Eligibility Traces**: Enables delayed reward effects
- **Plasticity Modulation**: Adjusts learning rates based on neuromodulation

### 5. Development
- **Stages**: Initial → CriticalPeriod → Maturation → Adult
- **Plasticity**: Decreases with age (1.0 → 0.2)
- **Structural Plasticity**: Modulates synaptogenesis and pruning rates

## Design Principles

### 1. Biological Plausibility
- Uses biologically inspired neuromodulators (dopamine, curiosity)
- Implements developmental stages similar to biological systems
- Energy and health constraints mimic biological limitations

### 2. Separation of Concerns
- **AgentBody**: Purely physical representation (environment side)
- **SensoryPercept**: Low-level sensory data only (no semantics)
- **AgentBrain**: Brain-world interface (computation side)

### 3. Scalability
- Modular design allows easy addition of new sensory modalities
- Configurable neuron distribution for different agent types
- Extensible neuromodulation system

## Usage Examples

### Basic Agent Setup
```cpp
// Create brain
auto cfg = std::make_shared<Config>();
auto brain = std::make_shared<Brain>(cfg);
brain->initialize();

// Create agent
AgentBrain agent(brain);
SimpleWorld world;
world.initialize(16, 16);
agent.initialize(world);

// Configure agent
agent.enableRewardModulation(true);
agent.enableCuriosity(true);
agent.enableDevelopment(true);
```

### Control Loop
```cpp
// Main agent control loop
while (running) {
    // Get observation
    SensoryPercept percept = world.observe();
    
    // Process sensory input
    agent.processSensoryInput(percept);
    
    // Brain step
    brain->step(step, timestep);
    
    // Get motor command
    MotorCommand cmd = agent.decodeMotorCommand();
    
    // Apply action
    world.applyAction(cmd);
    
    // Compute reward
    float reward = world.computeReward();
    agent.applyRewardModulation(reward, 0.0f);
    
    // Update development
    agent.updateDevelopment(timestep);
    
    step++;
}
```

## Configuration Options

### AgentBrain Configuration
- `rewardModulationEnabled_`: Enable/disable reward-based learning
- `structuralPlasticityEnabled_`: Enable/disable structural changes
- `developmentEnabled_`: Enable/disable developmental progression
- `curiosityEnabled_`: Enable/disable curiosity-driven exploration
- `sensoryNoveltyDecay_`: Rate of novelty signal decay

### Motor Command Groups
- 6 motor neuron groups: Forward, Backward, Left, Right, Interact, Wait
- Each group contains multiple motor neurons for robust control

### Sensory Groups
- Vision: 16x16 grid processed by 256 sensory neurons
- Touch: 8 collision signals
- Internal: 4 homeostatic signals
- Proprioception: 6 body state signals

## Performance Considerations

### Memory Usage
- Vision processing: ~64KB (256 float values)
- Motor control: ~100 neurons
- Neuromodulation: 6 state variables
- Development: 1 developmental stage + plasticity modifier

### Computational Cost
- Sensory processing: O(256) per timestep
- Motor decoding: O(6) per timestep
- Neuromodulation: O(synapses) per timestep
- Development: O(1) per timestep

## Advanced Features

### 1. Custom Sensory Modalities
- Extend `SensoryPercept` with new signal types
- Add corresponding sensory neuron groups
- Update `getAllSignals()` method

### 2. Advanced Motor Control
- Replace simple activity-based decoding with learned policies
- Add muscle-like dynamics and fatigue models
- Implement smooth trajectory planning

### 3. Complex Development
- Multi-stage developmental programs
- Experience-dependent plasticity rates
- Social learning and cultural transmission

## Integration with Other Systems

### Memory Systems
- Working memory traces updated by sensory input
- Episodic memory reinforced by reward signals
- Predictive coding integrated with sensory processing

### Neuromodulation
- Dopamine affects synaptic plasticity
- Novelty drives curiosity and exploration
- Prediction error guides learning

### Cognition
- Neural planner uses motor commands for action sequencing
- Concept formation abstracts from sensory patterns
- Attention allocates resources across sensory modalities

## Testing and Validation

### Integration Tests
- Phase 6 integration test verifies all systems work together
- Memory integration test validates episodic memory functionality
- Neuromodulation integration test confirms reward signaling
- Checkpointing test validates persistence

### Performance Tests
- Scaling experiments measure performance with increasing neurons
- Memory usage tests verify efficient resource utilization
- Benchmark tests compare performance across configurations

## Future Enhancements

### 1. Sensorimotor Learning
- Implement inverse kinematics for precise movement
- Add proprioceptive feedback loops
- Develop skill acquisition mechanisms

### 2. Social Intelligence
- Multi-agent interaction protocols
- Imitation learning from other agents
- Communication and coordination

### 3. Adaptive Development
- Experience-dependent developmental timelines
- Environmental modification of development
- Stress and resilience mechanisms

## References

- Documentation for individual components in source headers
- Phase 6 integration test results in docs/PHASE6_FINAL_AUDIT.md
- Architecture documentation in docs/ARCHITECTURE.md
- Performance metrics in docs/PERFORMANCE.md
# NLM AgentBrain Motor Control Guide

## Overview

This guide documents the advanced AgentBrain motor control capabilities in the NLM project. The AgentBrain provides sophisticated motor control through multiple integrated systems including advanced motor decoding, planning, learning, and sensory-motor integration.

## Quick Start

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Configure advanced motor control
agent.enableAdvancedMotorDecoding(True)
agent.enableMotorPlanning(True)
agent.enableReinforcementLearning(True)
agent.enableSkillLearning(True)
agent.enableSensoryMotorIntegration(True)

# Set temperature for action selection
agent.setActionTemperature(0.5)

# Set high-level goal
agent.setGoal("explore", parameters=[x=10.0, y=10.0])
```

## Motor Control Architecture

The AgentBrain motor control system is organized into five interconnected subsystems:

### 1. Advanced Motor Decoding
**Softmax Action Selection with Temperature Control**

The SoftmaxActionSelector provides intelligent action selection based on neural activity and preferences with configurable exploration/exploitation balance.

#### Key Features

- **Temperature Control**: Adjusts randomness in action selection
  - High temperature → More random, more exploration
  - Low temperature → More deterministic, more exploitation

- **Exploration Boosts**: Bias toward specific actions for curiosity-driven behavior

- **Neuromodulator Integration**: Automatically adjusts exploration based on dopamine and curiosity levels

#### API Reference

```python
# Create action selector with default settings
selector = SoftmaxActionSelector(temperature=1.0, explorationBoost=0.0)

# Select action with current neural activities
action = selector.selectAction(activities, preferences, temperature)

# Control exploration
selector.setTemperature(0.5)  # Reduce randomness
selector.setExplorationBoost(ActionType.MoveForward, 0.2)  # Boost forward movement

# Update from neuromodulators (automatic)
selector.updateFromNeuromodulators(dopamine_level, curiosity_level)
```

#### Temperature Strategies

| Temperature | Behavior | Use Case |
|-------------|----------|----------|
| 2.0+ | Highly random | Initial exploration, high curiosity |
| 1.0 | Balanced | General learning and adaptation |
| 0.5 | Low randomness | Goal-focused execution |
| 0.1 | Nearly deterministic | Optimized performance, fine-tuning |

### 2. Motor Planning and Sequences
**Goal-Directed Behavior with Action Sequences**

The HierarchicalController and ActionSequence classes enable complex, goal-directed motor behavior.

#### Action Sequences

```python
# Create and execute action sequences
sequence = ActionSequence()
sequence.addCommand(ActionType.MoveForward, reward=1.0, duration=0.5)
sequence.addCommand(ActionType.TurnLeft, reward=0.5, duration=0.2)
sequence.addCommand(ActionType.Interact, reward=2.0, duration=1.0)
sequence.goalDescription = "Navigate to food source"

# Execute sequence
while not sequence.isComplete():
    action = sequence.executeNext()
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Hierarchical Controller

```python
# Set high-level goal
controller = HierarchicalController()
controller.setGoal("find_shelter", parameters=[temp=35.0, predator_near=True])

# Generate action sequence for context
context = [player_pos, temperature, danger_level, energy]
sequence = controller.planSequence(context)

# Execute current action
current_action = controller.executeCurrentAction()
```

### 3. Reinforcement Learning Systems

#### Q-Learning

```python
# Initialize Q-learner
q_learner = QLearner(alpha=0.1, gamma=0.99, epsilon=0.1)

# Select action using epsilon-greedy policy
action = q_learner.selectAction(
    available_actions=[ActionType.MoveForward, ActionType.TurnLeft, ActionType.Wait],
    q_values={ActionType.MoveForward: 0.8, ActionType.TurnLeft: 0.3, ActionType.Wait: 0.1}
)

# Update Q-values based on experience
q_learner.updateQValue(
    action=ActionType.MoveForward,
    reward=1.0,
    next_action=ActionType.TurnLeft,
    max_next_q=0.7
)
```

#### Policy Gradient Learning

```python
# Initialize policy gradient learner
policy_learner = PolicyGradientLearner(
    learning_rate=0.001,
    entropy_coefficient=0.01
)

# Update policy based on trajectory
policy_learner.updatePolicy(
    actions=[ActionType.MoveForward, ActionType.TurnLeft, ActionType.Interact],
    advantages=[0.8, 0.3, 1.2],
    log_probabilities=[-0.2, -0.5, -0.1]
)
```

#### Actor-Critic Learning

```python
# Initialize actor-critic learner
actor_critic = ActorCriticLearner(
    actor_alpha=0.001,
    critic_alpha=0.01,
    gamma=0.99,
    tau=0.95
)

# Update both networks
actor_critic.update(
    states=[state1, state2, state3],
    actions=[ActionType.MoveForward, ActionType.TurnLeft, ActionType.Interact],
    rewards=[1.0, -0.5, 2.0],
    next_states=[state2, state3, terminal_state],
    dones=[False, False, True]
)

# Select action using current policy
action = actor_critic.selectAction(
    available_actions=[ActionType.MoveForward, ActionType.TurnLeft, ActionType.Wait],
    state_features=extract_features(state)
)

# Get state value estimate
value = actor_critic.getValue(state_features)
```

### 4. Motor Skill Learning

#### Hebbian Skill Learning

```python
# Initialize Hebbian skill learner
hebbian_learner = HebbianLearner(
    learning_rate=0.001,
    consolidation_rate=0.0001
)

# Learn association between sensory input and motor output
hebbian_learner.learnAssociation(
    sensory_input=[vision_data, proprioception, sound],
    motor_output=ActionType.MoveForward,
    reward=1.0
)

# Retrieve best motor response for similar input
best_action = hebbian_learner.retrieveMotorResponse(sensory_input)

# Get skill strength for association
strength = hebbian_learner.getSkillStrength(sensory_input, ActionType.MoveForward)
print(f"Skill strength: {strength:.3f}")
```

#### Motor Memory

```python
# Initialize motor memory with capacity
motor_memory = MotorMemory(capacity=1000)

# Store motor pattern
pattern = [ActionType.MoveForward, ActionType.TurnLeft, ActionType.Interact]
context = [temperature=25.0, hunger_level=0.8, energy_level=0.6]
motor_memory.storeMotorPattern(pattern, context, reward=2.0)

# Retrieve similar pattern for new context
sequence = motor_memory.retrievePattern(
    sensory_context=[temp=26.0, hunger=0.7, energy=0.7],
    similarity_threshold=0.8
)

# Update pattern usage
motor_memory.updatePatternUsage(pattern)

# Get usage statistics
usage_count = motor_memory.getPatternUsageCount(pattern)
print(f"Pattern used {usage_count} times")
```

### 5. Sensory-Motor Integration

#### Forward Model

```python
# Initialize forward model
forward_model = ForwardModel()

# Learn action-outcome mapping
forward_model.learnActionOutcome(
    action=ActionType.MoveForward,
    expected_sensory=[vision_change, proprioception_change, sound_signature]
)

# Predict sensory consequences of action
predicted_sensory = forward_model.predictSensory(ActionType.MoveForward)

# Get prediction error
actual_sensory = [real_vision, real_proprioception, real_sound]
error = forward_model.getPredictionError(
    action=ActionType.MoveForward,
    actual_sensory=actual_sensory
)
print(f"Prediction error: {error:.3f}")
```

#### Internal Model

```python
# Initialize internal model
internal_model = InternalModel()

# Update world model based on experience
internal_model.updateWorldModel(
    sensory_input=current_sensory,
    action=last_action
)

# Get current internal state
internal_state = internal_model.getInternalState()
print(f"Internal state dimension: {len(internal_state)}")

# Predict future state
future_state = internal_model.predictFutureState(
    action=planned_action,
    steps=10
)
```

## AgentBrain Enhanced API

### Core Methods

#### Advanced Motor Command Generation

```python
# Get advanced motor command using neural activity
neural_activity = brain.produceAction()
advanced_command = agent.getAdvancedMotorCommand(neural_activity)

# Alternative: traditional motor decoding (legacy)
legacy_command = agent.decodeMotorCommand()
```

#### Action Sequence Management

```python
# Get current action sequence for goal
sequence = agent.getCurrentActionSequence()
if sequence:
    print(f"Goal: {sequence.goalDescription}")
    print(f"Progress: {sequence.executionIndex_}/{len(sequence.commands)}")

# Update action sequence based on current state
agent.updateActionSequence()
```

#### Skill Learning and Retrieval

```python
# Get learned skill strength for association
strength = agent.getSkillStrength(sensory_input, action_type)
print(f"Skill strength: {strength:.3f}")

# Check if action sequence is complete
is_complete = agent.isActionSequenceComplete()
if is_complete:
    print("Current goal achieved!")
```

#### Sensory-Motor Prediction

```python
# Get predicted sensory consequences
predicted = agent.getPredictedSensory(action)
print(f"Predicted sensory changes: {predicted}")

# Get internal world model
internal_model = agent.getInternalWorldModel()
print(f"World model updated based on experience")
```

### Configuration Methods

#### Motor Decoding Configuration

```python
# Advanced motor decoding controls
agent.enableAdvancedMotorDecoding(True)
agent.setActionTemperature(0.5)  # Action selection randomness
agent.setQLearningParameters(alpha=0.1, gamma=0.99, epsilon=0.1)
agent.setPolicyGradientParameters(learningRate=0.001, entropyCoeff=0.01)
```

#### Motor Planning Configuration

```python
# Motor planning controls
agent.enableMotorPlanning(True)
agent.setMaxSequenceLength(20)  # Maximum actions in sequence
agent.setHierarchicalControlEnabled(True)  # Goal-directed planning
```

#### Reinforcement Learning Configuration

```python
# RL system controls
agent.enableReinforcementLearning(True)
agent.setDiscountFactor(0.95)  # Future reward importance
agent.setExplorationRate(0.2)  # Random exploration balance
```

#### Skill Learning Configuration

```python
# Skill learning controls
agent.enableSkillLearning(True)
agent.setHebbianLearningRate(0.001)  # Hebbian learning rate
agent.setMemoryCapacity(1000)  # Motor memory capacity
```

#### Sensory-Motor Integration Configuration

```python
# Integration system controls
agent.enableSensoryMotorIntegration(True)
agent.setPredictionModelEnabled(True)  # Forward model
agent.setInternalModelEnabled(True)   # Internal world model
```

## Complete Motor Control Example

```python
import pynlm
import time

def run_advanced_motor_control(episode_length=1000):
    """Complete example of advanced motor control."""
    
    # Setup brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Create agent with advanced motor control
    agent = pynlm.createAgentBrain(brain)
    
    # Configure all motor control systems
    agent.enableAdvancedMotorDecoding(True)
    agent.enableMotorPlanning(True)
    agent.enableReinforcementLearning(True)
    agent.enableSkillLearning(True)
    agent.enableSensoryMotorIntegration(True)
    
    # Set up motor control parameters
    agent.setActionTemperature(0.7)  # Exploration balance
    agent.setMaxSequenceLength(15)
    agent.setHierarchicalControlEnabled(True)
    agent.setDiscountFactor(0.95)
    agent.setExplorationRate(0.15)
    agent.setHebbianLearningRate(0.001)
    
    # Set initial goal
    agent.setGoal("explore_environment", parameters=[x=15.0, y=15.0])
    
    print("Starting advanced motor control simulation...")
    print(f"Temperature: {agent.getActionTemperature():.2f}")
    print(f"Goal: {agent.getCurrentGoal()}")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    # Run episode
    for step in range(episode_length):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        if agent.isActionSequenceComplete():
            # New goal if current one completed
            agent.setGoal("find_resource", parameters=[x=20.0, y=20.0])
        
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getEnergy() if world.getEnergy() > 0 else -1.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print motor control statistics
        if step % 100 == 0:
            sequence = agent.getCurrentActionSequence()
            skill_strength = agent.getSkillStrength(percept, action)
            predicted = agent.getPredictedSensory(action)
            
            print(f"Step {step}:")
            print(f"  Action: {action}")
            print(f"  Sequence progress: {sequence.executionIndex_}/{len(sequence.commands) if sequence else 0}")
            print(f"  Skill strength: {skill_strength:.3f}")
            print(f"  Predicted sensory: {predicted[:3]}...")
            print(f"  Temperature: {agent.getActionTemperature():.2f}")
    
    print(f"\nMotor control simulation completed!")
    print(f"Final goal: {agent.getCurrentGoal()}")
    print(f"Skill strengths: {agent.getSkillStrength(percept, ActionType.Interact):.3f}")
    
    return brain, agent, world

# Run advanced motor control
brain, agent, world = run_advanced_motor_control(episode_length=2000)
```

## Motor Control Strategies

### Strategy 1: Temperature-Based Exploration

```python
def adaptive_temperature_control(agent, step, episode_length):
    """Dynamically adjust action selection temperature."""
    
    # Start with high exploration
    if step < episode_length * 0.1:
        agent.setActionTemperature(2.0)
    # Gradually reduce exploration
    elif step < episode_length * 0.5:
        agent.setActionTemperature(1.0)
    # Focus on exploitation
    elif step < episode_length * 0.9:
        agent.setActionTemperature(0.5)
    # Fine-tune near end
    else:
        agent.setActionTemperature(0.1)
```

### Strategy 2: Hierarchical Goal Management

```python
def multi_level_goals(agent):
    """Manage multiple levels of goals."""
    
    # Set high-level goal
    agent.setGoal("survive", parameters=[health_threshold=0.3])
    
    # Sub-goals based on context
    if agent.getCuriosityLevel() > 0.8:
        agent.setGoal("explore", parameters=[area_size=10])
    elif agent.getEnergyLevel() < 0.3:
        agent.setGoal("find_food", parameters=[proximity=5])
    else:
        agent.setGoal("maintain_position", parameters=[stability=0.9])
```

### Strategy 3: Skill-Based Motor Control

```python
class SkillBasedController:
    def __init__(self, agent):
        self.agent = agent
        self.skill_threshold = 0.7
    
    def select_action(self, percept):
        # Get skill strengths for possible actions
        possible_actions = [ActionType.MoveForward, ActionType.TurnLeft, ActionType.Interact]
        
        best_action = None
        best_strength = 0.0
        
        for action in possible_actions:
            strength = self.agent.getSkillStrength(percept, action)
            if strength > best_strength:
                best_strength = strength
                best_action = action
        
        # Use skill-based selection if confident
        if best_strength > self.skill_threshold:
            return best_action
        else:
            # Fall back to curiosity-driven exploration
            return self.agent.decodeMotorCommand()
```

## Performance Optimization

### Motor Control Configuration for Performance

| Parameter | Recommended Value | Effect |
|-----------|------------------|--------|
| `action_temperature` | 0.5-1.0 | Balance exploration and exploitation |
| `max_sequence_length` | 10-20 | Plan complexity |
| `hierarchical_control` | true | Goal-directed behavior |
| `discount_factor` | 0.95-0.99 | Future reward importance |
| `exploration_rate` | 0.1-0.2 | Random exploration |

### Profiling Motor Control

```python
import time
import pynlm

# Profile motor control performance
start_time = time.time()
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableAdvancedMotorDecoding(True)
agent.enableMotorPlanning(True)
agent.enableReinforcementLearning(True)

# Configure for performance
agent.setActionTemperature(0.8)
agent.setMaxSequenceLength(15)
agent.setHierarchicalControlEnabled(True)

print("Starting motor control performance test...")

# Run performance test
for step in range(1000):
    action = agent.decodeMotorCommand()
    
    if step % 100 == 0:
        sequence = agent.getCurrentActionSequence()
        if sequence:
            print(f"Step {step}: Sequence progress {sequence.executionIndex_}/{len(sequence.commands)}")

end_time = time.time()
elapsed = end_time - start_time

print(f"Performance: {1000/elapsed:.1f} actions/second")
print(f"Motor control active: {agent.isActionSequenceComplete()}")
```

## Troubleshooting

### Motor Control Issues

#### Issue: "Agent not responding to goals"

**Cause**: Motor planning not enabled or configured correctly.

**Solution**:
```python
# Check and enable motor planning
if not agent.isMotorPlanningEnabled():
    agent.enableMotorPlanning(True)
    agent.setHierarchicalControlEnabled(True)
    agent.setMaxSequenceLength(10)
    
# Set new goal
agent.setGoal("new_goal", parameters=[param1, param2])
```

#### Issue: "Action selection too random"

**Cause**: Action selection temperature too high.

**Solution**:
```python
# Reduce temperature for more deterministic behavior
agent.setActionTemperature(0.3)

# Or use specific action selection based on skills
for action in [ActionType.MoveForward, ActionType.TurnLeft]:
    strength = agent.getSkillStrength(current_percept, action)
    if strength > 0.8:
        print(f"Selected action: {action} (strength: {strength:.2f})")
```

#### Issue: "Skill learning not working"

**Cause**: Hebbian learning not enabled or configured.

**Solution**:
```python
# Check and enable skill learning
if not agent.isSkillLearningEnabled():
    agent.enableSkillLearning(True)
    agent.setHebbianLearningRate(0.001)
    agent.setMemoryCapacity(1000)

# Monitor skill development
for step in range(100):
    percept = world.getSensoryPercept()
    action = agent.decodeMotorCommand()
    strength = agent.getSkillStrength(percept, action)
    if strength > 0.5:
        print(f"Skill developed: {strength:.3f}")
```

## Integration with Other Systems

### Motor Control + Neuromodulation

```python
# Configure neuromodulation effects on motor control
agent.enableCuriosity(True)
agent.setActionTemperature(1.5)  # High curiosity = more exploration

# Monitor neuromodulation impact
for step in range(500):
    curiosity = agent.getCuriosityLevel()
    dopamine = agent.getNeuromodulationLevel()
    
    # Adjust temperature based on curiosity
    if curiosity > 0.8:
        agent.setActionTemperature(2.0)
    elif curiosity < 0.3:
        agent.setActionTemperature(0.5)
```

### Motor Control + Development

```python
# Configure developmental effects on motor control
agent.enableDevelopment(True)

# Monitor developmental changes
for step in range(100):
    stage = agent.getDevelopmentalStage()
    plasticity = agent.getBrain().getDevelopmentSystem().getPlasticityModifier()
    
    # Adjust learning rates based on development
    if stage == DevelopmentalStage.CriticalPeriod:
        agent.setHebbianLearningRate(0.01)  # Fast learning
    else:
        agent.setHebbianLearningRate(0.001)  # Normal learning
```

## Next Steps

1. **Study Neuromodulation**: Read `Neuromodulation Guide` for dopamine, curiosity, and novelty systems
2. **Explore Brain Architecture**: Read `Brain Architecture Guide` for the refactored brain structure
3. **Implement Custom Strategies**: Build your own motor control strategies using the patterns above
4. **Performance Optimization**: Use profiling to optimize motor control parameters
5. **Research Applications**: Use advanced motor control for behavioral research

## References

- NLM Architecture Documentation: `docs/ARCHITECTURE.md`
- Enhanced Python API Guide: `docs/enhanced_python_api_guide.md`
- Neuromodulation Systems: `src/neuromodulation/` directory
- Motor Control Implementation: `src/agent/AgentBrain.hpp`, `src/agent/AgentBrain.cpp`
- Reinforcement Learning: RL algorithms in `src/agent/AgentBrain.hpp`
- Sensorimotor Integration: Forward and internal models in `src/agent/AgentBrain.hpp`

---

*This guide is part of the comprehensive NLM documentation suite. For additional information, refer to related documentation files.*
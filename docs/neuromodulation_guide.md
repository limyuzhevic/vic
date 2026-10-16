# NLM Neuromodulation Guide

## Overview

This guide documents the neuromodulation systems in the NLM project. Neuromodulation is a crucial component of biological brains that provides global signals to regulate neural processing, learning, and behavior. The NLM implements a comprehensive neuromodulation system with nine specialized systems: Dopamine, Acetylcholine, Norepinephrine, Serotonin, Curiosity, Novelty, PredictionError, Reward, and the base Neuromodulator class.

## Quick Start

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable neuromodulation systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)
agent.enablePredictionErrorLearning(True)
agent.enableNeuromodulation(True)

# Get neuromodulation levels
print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

## Neuromodulation Systems

### 1. Neuromodulator (Base Class)

The `Neuromodulator` class is the abstract base class for all neuromodulatory systems. It provides the interface for neuromodulator functionality.

#### Key Features

- **Abstract Interface**: Common interface for all neuromodulators
- **Plasticity Factor**: Modifies neural plasticity based on neuromodulator level
- **Level Management**: Current concentration/level tracking
- **Update System**: Time-based state updates

#### API Reference

```python
# Abstract base class - cannot be instantiated directly

# Common interface for all neuromodulators
class Neuromodulator:
    # Get modulator name
    getName()
    
    # Get current level
    getLevel()
    setLevel(level)
    
    # Get plasticity factor (higher = more learning)
    getPlasticityFactor()
    
    # Update state based on time
    update(dt)
```

### 2. Dopamine (Reward and Reinforcement Learning)

Dopamine is the brain's primary reward and reinforcement learning signal. It signals reward prediction errors and modulates learning, motivation, and motor control.

#### Key Features

- **Reward Signaling**: Burst responses to unexpected rewards
- **Prediction Error**: Responds to reward prediction errors (actual - predicted reward)
- **Learning Modulation**: Increases plasticity when dopamine is high
- **Motivation**: Influences goal-directed behavior and persistence

#### Implementation Details

```python
// C++ Implementation
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    // Neuromodulator interface
    const char* getName() const override;  // Returns "DA"
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;  // 0.5 + 0.5 * level
    void update(TimestepDuration dt) override;   // Decay towards baseline
    
    // Reward signaling
    void signalReward(float reward);  // Reward burst
    void signalRewardPredictionError(float error);  // Prediction error response
};
```

#### API Reference

```python
# Dopamine neuromodulator operations
dopamine = brain.getDopamine()  # Get dopamine instance

# Set dopamine level (0.0 = baseline, 1.0 = peak)
dopamine.setLevel(0.5)
print(f"Current dopamine level: {dopamine.getLevel():.3f}")

# Signal reward received
dopamine.signalReward(1.0)  # Strong reward

# Signal reward prediction error
dopamine.signalRewardPredictionError(-0.5)  # Under-predicted reward

# Get plasticity factor (higher = more learning)
plasticity_factor = dopamine.getPlasticityFactor()
print(f"Plasticity factor: {plasticity_factor:.3f}")  # 0.75 if level is 0.5
```

#### Dopamine Behavior

| Dopamine Level | Behavior | Use Case |
|---------------|----------|----------|
| 0.0 | Baseline learning | Routine tasks, expected outcomes |
| 0.3-0.5 | Moderate learning | Mild rewards, standard learning |
| 0.7-1.0 | High learning | Strong rewards, new learning |
| 1.0+ | Peak learning | Unexpected rewards, major discoveries |

### 3. Acetylcholine (Attention and Memory Consolidation)

Acetylcholine regulates attention, learning, and memory consolidation. It helps focus neural processing on relevant information and strengthen important memories.

#### Key Features

- **Attention Regulation**: Modulates neural responsiveness and focus
- **Memory Consolidation**: Enhances memory stability and recall
- **Learning Rate**: Optimizes information processing
- **Cognitive Control**: Balances exploration and exploitation

#### API Reference

```python
# Acetylcholine neuromodulator operations
ac acetylcholine = brain.getAcetylcholine()

# Set ACh level (0.0 = baseline, 1.0 = peak)
ach.setLevel(0.8)
print(f"ACh level: {ach.getLevel():.3f}")

# Get plasticity factor (0.0 = reduce learning, 1.0 = enhance learning)
plasticity = ach.getPlasticityFactor()
print(f"ACh plasticity factor: {plasticity:.3f}")
```

### 4. Norepinephrine (Arousal and Vigilance)

Norepinephrine controls arousal, vigilance, attention, and stress responses. It increases alertness and responsiveness to salient stimuli.

#### Key Features

- **Arousal Control**: Modulates overall brain activation level
- **Vigilance**: Increases monitoring of environment for threats/opportunities
- **Attention**: Enhances focus on relevant information
- **Stress Response**: Coordinates physiological stress responses

#### API Reference

```python
# Norepinephrine neuromodulator operations
norepinephrine = brain.getNorepinephrine()

# Set NE level
norepinephrine.setLevel(0.6)
print(f"NE level: {norepinephrine.getLevel():.3f}")

# Monitor arousal and alertness
if norepinephrine.getLevel() > 0.7:
    print("High arousal - increased vigilance and attention")
else:
    print("Low arousal - relaxed state")
```

### 5. Serotonin (Mood, Impulsivity, Social Behavior)

Serotonin regulates mood, impulsivity, social behavior, and sleep. It promotes patience, social cooperation, and emotional stability.

#### Key Features

- **Mood Regulation**: Influences emotional states and well-being
- **Impulsivity Control**: Reduces premature actions and increases patience
- **Social Behavior**: Modulates social cooperation and bonding
- **Sleep-Wake Cycle**: Regulates sleep patterns

#### API Reference

```python
# Serotonin neuromodulator operations
serotonin = brain.getSerotonin()

# Set serotonin level
serotonin.setLevel(0.4)
print(f"Serotonin level: {serotonin.getLevel():.3f}")

# Monitor social behavior influence
if serotonin.getLevel() > 0.7:
    print("High serotonin - increased social cooperation")
else:
    print("Low serotonin - potential impulsivity")
```

### 6. Curiosity (Exploration Drive)

Curiosity is the drive for exploration and novel experiences. It's computed as a weighted combination of novelty detection and prediction error signals.

#### Key Features

- **Exploration Drive**: Motivates seeking new information and experiences
- **Novelty Detection**: Responds to unexpected or surprising stimuli
- **Prediction Error**: Values information that reduces uncertainty
- **Adaptive Exploration**: Balances exploration and exploitation

#### Implementation Details

```python
// C++ Implementation
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level (exploration drive)
    float getLevel() const;
    float getExplorationDrive() const;
    
    // Update based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Configure weights
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Reset system
    void reset();
};
```

#### API Reference

```python
# Curiosity system operations
curiosity = brain.getCuriosity()

# Initialize with brain
curiosity.initialize(brain)

# Update curiosity based on stimuli
# In your simulation loop:
novelty = agent.getNoveltyLevel()
prediction_error = agent.getPredictionError()
curiosity.update(novelty, prediction_error, dt)

print(f"Curiosity level: {curiosity.getLevel():.3f}")
print(f"Exploration drive: {curiosity.getExplorationDrive():.3f}")

# Configure curiosity weights
curiosity.setNoveltyWeight(0.6)  # Weight for novelty detection
ins curiosity.setPredictionErrorWeight(0.4)  # Weight for prediction error
```

#### Curiosity Dynamics

Curiosity drives exploration behavior and learning motivation. It follows these principles:

1. **Increase with Novelty**: Surprising stimuli increase curiosity
2. **Increase with Prediction Error**: Uncertainty reduction increases curiosity
3. **Decrease with Familiarity**: Repeated stimuli decrease curiosity
4. **Decay**: Curiosity naturally decays over time

### 7. Novelty (Pattern Change Detection)

Novelty detection identifies patterns in sensory input that differ from previous experiences. It's essential for curiosity-driven exploration and adaptive learning.

#### Key Features

- **Pattern Detection**: Identifies deviations from expected patterns
- **Memory Comparison**: Compares current input with stored memories
- **Change Detection**: Signals when environment changes
- **Learning Trigger**: Initiates exploration when novelty is detected

#### Implementation Details

```python
// C++ Implementation
class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current novelty level
    float getLevel() const;
    
    // Update novelty based on sensory input
    void update(const SensoryInput& input, TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    
    // Clear novelty history
    void clearHistory();
};
```

#### API Reference

```python
# Novelty detection system operations
novelty = brain.getNovelty()

# Initialize with brain
novelty.initialize(brain)

# Update novelty based on sensory input
for step in range(episode_length):
    # Get current sensory percept
    percept = world.getSensoryPercept()
    
    # Update novelty detection
    novelty.update(percept, dt)
    
    print(f"Novelty level: {novelty.getLevel():.3f}")
    
    # Check for significant novelty
    if novelty.getLevel() > 0.7:
        print("High novelty detected - triggering exploration!")

# Get novelty history
history = novelty.getHistory()
print(f"Novelty history size: {len(history)}")

# Clear history to prevent accumulation
novelty.clearHistory()
```

#### Novelty Detection Algorithm

Novelty detection works by:

1. **Pattern Extraction**: Extract features from sensory input
2. **Memory Comparison**: Compare with stored patterns from experience
3. **Distance Calculation**: Compute similarity/difference
4. **Threshold Comparison**: Determine if change is significant
5. **Signal Generation**: Generate novelty signal based on difference

### 8. Prediction Error (Learning from Prediction Errors)

Prediction error detection computes the difference between expected and actual outcomes. It's the fundamental learning signal in the brain.

#### Key Features

- **Learning Signal**: Computes prediction error (actual - predicted)
- **Memory Update**: Updates stored predictions based on errors
- **Adaptation**: Adjusts future expectations based on errors
- **Efficiency**: Identifies when predictions are inaccurate

#### Implementation Details

```python
// C++ Implementation
class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current prediction error
    float getError() const;
    
    // Compute error from prediction and actual values
    void computeError(float predicted, float actual);
    
    // Update prediction with new value
    void updatePrediction(float newPrediction);
    
    // Get prediction error history
    const std::vector<float>& getHistory() const;
    
    // Clear history
    void clearHistory();
    
    // Get error magnitude (absolute value)
    float getMagnitude() const;
};
```

#### API Reference

```python
# Prediction error system operations
prediction_error = brain.getPredictionError()

# Initialize with brain
prediction_error.initialize(brain)

# Compute prediction error
predicted_value = agent.getPredictedSensory(action)
actual_value = get_actual_outcome(world, action)
prediction_error.computeError(predicted_value, actual_value)

print(f"Prediction error: {prediction_error.getError():.3f}")
print(f"Error magnitude: {prediction_error.getMagnitude():.3f}")

# Update prediction based on experience
prediction_error.updatePrediction(new_prediction)

# Get error history
history = prediction_error.getHistory()
print(f"Error history size: {len(history)}")

# Clear history to prevent accumulation
prediction_error.clearHistory()
```

#### Prediction Error Learning

Prediction error drives learning through the delta rule:

```
new_weight = old_weight + learning_rate * prediction_error * input
```

### 9. Reward (Reward Computation and History)

The Reward system computes, tracks, and applies rewards to guide learning and behavior. It maintains history of received rewards and can compute reward functions based on environmental observations.

#### Key Features

- **Reward Computation**: Calculate reward based on outcomes
- **History Tracking**: Maintain record of all received rewards
- **Accumulation**: Track total and average rewards
- **Adaptive Rewards**: Learn reward functions through experience

#### Implementation Details

```python
// C++ Implementation
class Reward {
public:
    Reward();
    ~Reward();
    
    // Get current reward value
    float getValue() const;
    
    // Set or update reward value
    void setValue(float value);
    
    // Add to current reward
    void add(float delta);
    
    // Reset reward system
    void reset();
    
    // Compute reward from observation
    float computeReward(const Observation& observation) const;
    
    // Get reward history
    const std::vector<float>& getHistory() const;
    
    // Clear history
    void clearHistory();
};
```

#### API Reference

```python
# Reward system operations
reward = brain.getReward()

# Initialize reward system
reward.reset()
print(f"Initial reward value: {reward.getValue():.3f}")

# Receive reward
reward.setValue(1.0)  # Set direct reward
reward.add(0.5)       # Add to current reward (e.g., +0.5 for food found)

print(f"Current reward: {reward.getValue():.3f}")

# Get accumulated reward
accumulated = reward.getTotalAccumulated()  # If available
print(f"Accumulated reward: {accumulated:.3f}")

# Compute reward from observation (advanced)
observation = world.getObservation()
computed_reward = reward.computeReward(observation)
print(f"Computed reward from observation: {computed_reward:.3f}")

# Get reward history
history = reward.getHistory()
print(f"Reward history size: {len(history)}")

# Track reward over time
for step in range(100):
    # ... simulation step ...
    reward.add(get_step_reward())  # Add reward from this step
    
    if step % 10 == 0:
        avg_reward = sum(history) / len(history)
        print(f"Step {step}: Average reward = {avg_reward:.3f}")
```

## Neuromodulation Integration

### Brain Neuromodulation Interface

The NLM brain provides access to neuromodulation systems for advanced applications:

```python
# Get neuromodulation systems from brain
brain = pynlm.createBrain(config)

# Access individual neuromodulators
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionError()
reward = brain.getReward()

# Apply neuromodulation through agent brain
agent = pynlm.createAgentBrain(brain)
agent.applyRewardModulation(reward_value, predicted_reward)
```

### Neuromodulation in Simulation Loop

```python
import pynlm
import time

def run_neuromodulation_simulation(episode_length=1000):
    """Complete example demonstrating neuromodulation systems."""
    
    # Setup brain and agent
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    agent.enablePredictionErrorLearning(True)
    agent.enableNeuromodulation(True)
    
    print("Starting neuromodulation simulation...")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    agent.initialize(world)
    
    # Initialize neuromodulation systems
    if brain.getDopamine():
        brain.getDopamine()->setLevel(0.1)
    
    if brain.getCuriosity():
        brain.getCuriosity()->initialize(brain.get());
    
    if brain.getNovelty():
        brain.getNovelty()->initialize(brain.get());
    
    if brain.getPredictionError():
        brain.getPredictionError()->initialize(brain.get());
    
    # Run simulation with neuromodulation tracking
    for step in range(episode_length):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get neuromodulation levels for monitoring
        neuromod_data = {
            'step': step,
            'dopamine': brain.getDopamine() ? brain.getDopamine()->getLevel() : 0.0f,
            'curiosity': brain.getCuriosity() ? brain.getCuriosity()->getLevel() : 0.0f,
            'novelty': brain.getNovelty() ? brain.getNovelty()->getLevel() : 0.0f,
            'prediction_error': brain.getPredictionError() ? brain.getPredictionError()->getError() : 0.0f,
            'reward': brain.getReward() ? brain.getReward()->getValue() : 0.0f
        };
        
        # Apply neuromodulation based on outcomes
        reward = world.getEnergy() > 0 ? 1.0 : -1.0;
        predicted = 0.0;  // Would be predicted reward from agent's model
        agent.applyRewardModulation(reward, predicted);
        
        # Update neuromodulation systems
        if brain.getCuriosity():
            brain.getCuriosity()->update(novelty, prediction_error, 0.1);
        
        if brain.getNovelty():
            brain.getNovelty()->update(percept, 0.1);
        
        # Print neuromodulation statistics
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Dopamine: {neuromod_data['dopamine']:.3f}")
            print(f"  Curiosity: {neuromod_data['curiosity']:.3f}")
            print(f"  Novelty: {neuromod_data['novelty']:.3f}")
            print(f"  Prediction Error: {neuromod_data['prediction_error']:.3f}")
            print(f"  Reward: {neuromod_data['reward']:.3f}")
        
        # Get action and apply
        action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        # Store neuromodulation data for analysis
        neuromod_history.append(neuromod_data);
    
    print(f"\nNeuromodulation simulation completed!")
    print(f"Final neuromodulation levels:")
    print(f"  Dopamine: {neuromod_data['dopamine']:.3f}")
    print(f"  Curiosity: {neuromod_data['curiosity']:.3f}")
    print(f"  Novelty: {neuromod_data['novelty']:.3f}")
    
    return brain, agent, world, neuromod_history;

# Run neuromodulation simulation
brain, agent, world, neuromod_history = run_neuromodulation_simulation(episode_length=2000);
```

## Neuromodulation Analysis

### Statistical Analysis

```python
# Analyze neuromodulation data
def analyze_neuromodulation_history(history):
    """Analyze neuromodulation system performance."""
    
    if not history:
        return {};
    
    analysis = {};
    
    # Dopamine analysis
    dopamine_levels = [d['dopamine'] for d in history];
    analysis['dopamine'] = {
        'mean': sum(dopamine_levels) / len(dopamine_levels),
        'max': max(dopamine_levels),
        'min': min(dopamine_levels),
        'std': calculate_standard_deviation(dopamine_levels)
    };
    
    # Curiosity analysis
    curiosity_levels = [d['curiosity'] for d in history];
    analysis['curiosity'] = {
        'mean': sum(curiosity_levels) / len(curiosity_levels),
        'max': max(curiosity_levels),
        'min': min(curiosity_levels),
        'trend': calculate_trend(curiosity_levels)
    };
    
    # Novelty analysis
    novelty_levels = [d['novelty'] for d in history];
    analysis['novelty'] = {
        'mean': sum(novelty_levels) / len(novelty_levels),
        'max': max(novelty_levels),
        'min': min(novelty_levels),
        'correlation_with_curiosity': calculate_correlation(novelty_levels, curiosity_levels)
    };
    
    # Prediction error analysis
    pred_errors = [d['prediction_error'] for d in history];
    analysis['prediction_error'] = {
        'mean': sum(pred_errors) / len(pred_errors),
        'max': max(pred_errors),
        'min': min(pred_errors),
        'decay_rate': calculate_decay_rate(pred_errors)
    };
    
    # Reward analysis
    rewards = [d['reward'] for d in history];
    analysis['reward'] = {
        'mean': sum(rewards) / len(rewards),
        'max': max(rewards),
        'min': min(rewards),
        'total': sum(rewards)
    };
    
    return analysis;

# Analyze neuromodulation data
analysis = analyze_neuromodulation_history(neurom_history);

print("Neuromodulation System Analysis:");
for system, stats in analysis.items() {
    print(f"\n{system.title()}:");
    for key, value in stats.items() {
        print(f"  {key}: {value:.3f}");
    }
}
```

### Visualization Data

```python
# Prepare neuromodulation data for visualization
import matplotlib.pyplot as plt

def plot_neuromodulation_data(history):
    """Create visualization of neuromodulation system dynamics."""
    
    if not history:
        print("No neuromodulation data to plot");
        return;
    
    steps = [d['step'] for d in history];
    
    # Create subplots
    fig, axes = plt.subplots(2, 3, figsize=(15, 10));
    fig.suptitle("Neuromodulation System Dynamics", fontsize=16);
    
    # Dopamine plot
    axes[0, 0].plot(steps, [d['dopamine'] for d in history], 'r-', linewidth=2);
    axes[0, 0].set_title("Dopamine Level");
    axes[0, 0].set_ylabel("Level (0-1)");
    axes[0, 0].grid(True, alpha=0.3);
    
    # Curiosity plot
    axes[0, 1].plot(steps, [d['curiosity'] for d in history], 'g-', linewidth=2);
    axes[0, 1].set_title("Curiosity Level");
    axes[0, 1].set_ylabel("Level (0-1)");
    axes[0, 1].grid(True, alpha=0.3);
    
    # Novelty plot
    axes[0, 2].plot(steps, [d['novelty'] for d in history], 'b-', linewidth=2);
    axes[0, 2].set_title("Novelty Level");
    axes[0, 2].set_ylabel("Level (0-1)");
    axes[0, 2].grid(True, alpha=0.3);
    
    # Prediction error plot
    axes[1, 0].plot(steps, [d['prediction_error'] for d in history], 'orange', linewidth=2);
    axes[1, 0].set_title("Prediction Error");
    axes[1, 0].set_ylabel("Error");
    axes[1, 0].set_xlabel("Step");
    axes[1, 0].grid(True, alpha=0.3);
    
    # Reward plot
    axes[1, 1].plot(steps, [d['reward'] for d in history], 'purple', linewidth=2);
    axes[1, 1].set_title("Reward Value");
    axes[1, 1].set_ylabel("Reward");
    axes[1, 1].set_xlabel("Step");
    axes[1, 1].grid(True, alpha=0.3);
    
    # Correlation plot
    dopamine = [d['dopamine'] for d in history];
    curiosity = [d['curiosity'] for d in history];
    axes[1, 2].scatter(dopamine, curiosity, alpha=0.5, s=1);
    axes[1, 2].set_title("Dopamine vs Curiosity Correlation");
    axes[1, 2].set_xlabel("Dopamine");
    axes[1, 2].set_ylabel("Curiosity");
    axes[1, 2].grid(True, alpha=0.3);
    
    plt.tight_layout();
    plt.show();
```

## Neuromodulation Applications

### Application 1: Curiosity-Driven Exploration

```python
# Create curiosity-driven exploration agent
def create_curiosity_driven_agent():
    brain = pynlm.createBrain(pynlm.createDefaultConfig());
    brain.initialize();
    
    agent = pynlm.createAgentBrain(brain);
    
    # Configure for curiosity-driven behavior
    agent.enableCuriosity(True);
    agent.enableNoveltyDetection(True);
    agent.enablePredictionErrorLearning(True);
    
    # Set up neuromodulation for exploration
    curiosity = brain.getCuriosity();
    if (curiosity) {
        curiosity->setNoveltyWeight(0.7);  // High weight for novelty
        curiosity->setPredictionErrorWeight(0.3);  // Moderate weight for prediction error
    }
    
    return brain, agent;

# Run curiosity-driven exploration
brain, agent = create_curiosity_driven_agent();
world = pynlm.createSimpleWorld();
agent.initialize(world);

for step in range(1000):
    world.update(0.1);
    agent.processSensoryInput(world.getSensoryPercept());
    brain.step(step);
    
    // Check if agent should explore based on curiosity
    curiosity = brain.getCuriosity()->getLevel();
    if (curiosity > 0.8) {
        // High curiosity - encourage exploration
        action = agent.decodeMotorCommand();
        // Add exploration bias
    }
    
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime());
```

### Application 2: Reward-Based Learning

```python
# Create reward-based learning agent
def create_reward_based_agent():
    brain = pynlm.createBrain(pynlm.createDefaultConfig());
    brain.initialize();
    
    agent = pynlm.createAgentBrain(brain);
    
    // Configure for reward-based learning
    agent.enableRewardModulation(True);
    agent.enableCuriosity(True);
    agent.enablePredictionErrorLearning(True);
    
    // Set up dopamine for reward signaling
    dopamine = brain.getDopamine();
    if (dopamine) {
        dopamine->setLevel(0.1);  // Start with low baseline
    }
    
    return brain, agent;

// Run reward-based learning
brain, agent = create_reward_based_agent();
agent.initialize(world);

for step in range(1000):
    world.update(0.1);
    agent.processSensoryInput(world.getSensoryPercept());
    brain.step(step);
    
    // Calculate reward based on outcome
    reward = calculate_reward(world, agent);
    predicted = agent.getPredictedReward();  // From agent's model
    
    // Apply reward modulation
    agent.applyRewardModulation(reward, predicted);
    
    // Update dopamine system
    if (brain.getDopamine()) {
        brain.getDopamine()->signalReward(reward);
    }
    
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime());
```

### Application 3: Prediction Error Learning

```python
// Create prediction error learning agent
def create_prediction_error_agent() {
    brain = pynlm.createBrain(pynlm.createDefaultConfig());
    brain.initialize();
    
    agent = pynlm.createAgentBrain(brain);
    
    // Configure for prediction error learning
    agent.enablePredictionErrorLearning(True);
    agent.enableCuriosity(True);
    agent.enableRewardModulation(True);
    
    // Set up prediction error system
    prediction_error = brain.getPredictionError();
    if (prediction_error) {
        prediction_error->initialize(brain.get());
    }
    
    return brain, agent;
}

// Run prediction error learning
brain, agent = create_prediction_error_agent();
agent.initialize(world);

for step in range(1000) {
    world.update(0.1);
    agent.processSensoryInput(world.getSensoryPercept());
    brain.step(step);
    
    // Get predicted outcome
    predicted = agent.getPredictedOutcome();
    actual = get_actual_outcome(world, agent.decodeMotorCommand());
    
    // Compute prediction error
    if (brain.getPredictionError()) {
        brain.getPredictionError()->computeError(predicted, actual);
    }
    
    // Use prediction error for learning
    float error = brain.getPredictionError()->getError();
    agent.applyLearningSignal(error);
    
    // Apply reward
    reward = calculate_reward_from_error(error);
    agent.applyRewardModulation(reward, predicted);
    
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime());
}
```

## Troubleshooting

### Neuromodulation Issues

#### Issue: "Neuromodulation system not working"

**Cause**: Neuromodulation systems not properly initialized or enabled.

**Solution**:
```python
// Check and initialize neuromodulation systems
if (!brain.getDopamine()) {
    // Create dopamine system if it doesn't exist
    // This depends on brain implementation
}

if (brain.getCuriosity()) {
    brain.getCuriosity()->initialize(brain.get());
}

if (brain.getNovelty()) {
    brain.getNovelty()->initialize(brain.get());
}

if (brain.getPredictionError()) {
    brain.getPredictionError()->initialize(brain.get());
}

// Enable neuromodulation in agent
agent.enableCuriosity(true);
agent.enableNoveltyDetection(true);
agent.enablePredictionErrorLearning(true);
```

#### Issue: "Dopamine not responding to rewards"

**Cause**: Dopamine system not properly configured.

**Solution**:
```python
// Configure dopamine system
dopamine = brain.getDopamine();
if (dopamine) {
    dopamine->setLevel(0.1);  // Start with baseline
    
    // Signal reward when received
    if (reward > 0) {
        dopamine->signalReward(reward);
    } else if (reward < 0) {
        // Signal prediction error for negative rewards
        dopamine->signalRewardPredictionError(reward);
    }
}
```

#### Issue: "Curiosity not driving exploration"

**Cause**: Curiosity system not properly configured or updated.

**Solution**:
```python
// Configure curiosity system
curiosity = brain.getCuriosity();
if (curiosity) {
    curiosity->setNoveltyWeight(0.7);  // High novelty weight
    curiosity->setPredictionErrorWeight(0.3);  // Moderate prediction error weight
}

// Update curiosity based on stimuli
novelty = agent.getNoveltyLevel();
prediction_error = agent.getPredictionError();
curiosity->update(novelty, prediction_error, dt);
```

#### Issue: "Prediction error not learning"

**Cause**: Prediction error system not properly initialized or updated.

**Solution**:
```python
// Configure prediction error system
prediction_error = brain.getPredictionError();
if (prediction_error) {
    prediction_error->initialize(brain.get());
}

// Compute and update prediction error
predicted = agent.getPredictedOutcome();
actual = get_actual_outcome(world, agent.decodeMotorCommand());
prediction_error->computeError(predicted, actual);
```

## Next Steps

1. **Study Motor Control**: Read `AgentBrain Motor Control Guide` for advanced motor control systems
2. **Explore Brain Architecture**: Read `Brain Architecture Guide` for the refactored brain structure
3. **Implement Custom Applications**: Build your own neuromodulation applications using the patterns above
4. **Performance Optimization**: Use profiling to optimize neuromodulation parameters
5. **Research Applications**: Use neuromodulation systems for behavioral research

## References

- NLM Architecture Documentation: `docs/ARCHITECTURE.md`
- Enhanced Python API Guide: `docs/enhanced_python_api_guide.md`
- AgentBrain Motor Control: `docs/agent_brain_motor_control_guide.md`
- Neuromodulation Implementation: `src/neuromodulation/` directory
- Brain Neuromodulation: `src/brain/BrainNeuromodulation.hpp`, `src/brain/BrainNeuromodulation.cpp`

---

*This guide is part of the comprehensive NLM documentation suite. For additional information, refer to related documentation files.*
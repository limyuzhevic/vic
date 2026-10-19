# Tutorial 3: Neuromodulation and Learning

This tutorial explores the neuromodulation systems in NLM: Dopamine, Curiosity, Novelty, and PredictionError. These systems control learning, motivation, and adaptation.

## Learning Objectives

By the end of this tutorial, you will be able to:

- Understand the role of neuromodulators in learning and behavior
- Create and use dopamine for reward-based learning
- Implement curiosity for exploration and intrinsic motivation
- Detect and respond to novel events with novelty detection
- Use prediction error for adaptive learning
- Integrate neuromodulation systems in your simulations

## 1. Introduction to Neuromodulation

Neuromodulation systems are the "learning coaches" of NLM. They:

- **Modulate** neural activity based on context
- **Guide** learning and behavior
- **Signal** important events (rewards, novelties, errors)
- **Control** plasticity and attention

### 1.1 Key Neuromodulators

| Neuromodulator | Primary Function | Learning Role |
|----------------|------------------|---------------|
| **Dopamine** | Reward prediction | Reinforcement learning |
| **Curiosity** | Exploration drive | Intrinsic motivation |
| **Novelty** | Change detection | Adaptability |
| **PredictionError** | Mismatch detection | Error-based learning |

## 2. Dopamine System

### 2.1 Dopamine Overview

```python
import pynlm

# Create dopamine system
dopamine = pynlm.createDopamine()
```

### 2.2 Dopamine Properties

```python
# Set initial dopamine level
dopamine.setLevel(0.1)

# Get current level
level = dopamine.getLevel()  # 0.1

# Get plasticity factor (how much learning is enhanced)
plasticity = dopamine.getPlasticityFactor()  # 1.0 (normal)
```

### 2.3 Reward-Based Learning

```python
# Signal a reward
dopamine.signalReward(0.8)  # Strong reward

# Signal reward prediction error
dopamine.signalRewardPredictionError(0.2)  # Got more than expected
```

### 2.4 Dopamine in Action

```python
class RewardBasedLearning:
    def __init__(self):
        self.dopamine = pynlm.createDopamine()
        self.reward_history = []
        
    def learn_from_reward(self, action, outcome):
        # Process action outcome
        reward = self.calculate_reward(action, outcome)
        
        # Signal to dopamine system
        self.dopamine.signalReward(reward)
        self.dopamine.signalRewardPredictionError(reward - self.expected_reward)
        
        # Update expected reward
        self.expected_reward = reward
        
        # Record learning
        self.reward_history.append((action, outcome, reward))
        
    def calculate_reward(self, action, outcome):
        # Calculate reward based on action outcome
        # In practice, this would use learned value functions
        return min(1.0, outcome * 0.8 + 0.2)
```

## 3. Curiosity System

### 3.1 Curiosity Overview

```python
import pynlm

# Create curiosity system
curiosity = pynlm.createCuriosity()
```

### 3.2 Curiosity Properties

```python
# Update curiosity based on novelty and prediction error
curiosity.update(novelty=0.7, prediction_error=0.3, dt=0.1)

# Get current curiosity level
level = curiosity.getCuriosityLevel()  # 0.45

# Adjust curiosity weights
curiosity.setNoveltyWeight(0.7)  # 70% driven by novelty
curiosity.setPredictionErrorWeight(0.3)  # 30% driven by prediction error
```

### 3.3 Curiosity-Driven Exploration

```python
class CuriosityDrivenExploration:
    def __init__(self):
        self.curiosity = pynlm.createCuriosity()
        self.exploration_history = []
        
    def explore_environment(self, world, agent, steps):
        """Explore environment driven by curiosity"""
        for step in range(steps):
            # Update curiosity
            self.curiosity.update(novelty=0.5, prediction_error=0.2, dt=0.1)
            
            # Check curiosity level
            curiosity_level = self.curiosity.getCuriosityLevel()
            
            # High curiosity -> more exploration
            if curiosity_level > 0.6:
                # Take novel action
                action = self.take_novel_action(world, agent)
                print(f"High curiosity (level={curiosity_level:.2f}) -> Taking novel action")
            else:
                # Normal action selection
                action = self.select_normal_action(world, agent)
                
            # Execute action
            self.execute_action(world, agent, action)
            
            # Record exploration
            self.exploration_history.append(curiosity_level)
            
    def take_novel_action(self, world, agent):
        """Take a novel action to increase curiosity"""
        # In practice, this would use intrinsic motivation algorithms
        # For demonstration, take a random action
        return agent.getRandomAction()
        
    def select_normal_action(self, world, agent):
        """Select normal action"""
        return agent.decodeMotorCommand()
```

## 4. Novelty Detection System

### 4.1 Novelty Overview

```python
import pynlm

# Create novelty detection system
novelty = pynlm.createNovelty()
```

### 4.2 Novelty Detection

```python
# Detect novelty from patterns
pattern_current = [0.1, 0.9, 0.2, 0.8]  # Current sensory pattern
pattern_previous = [0.8, 0.2, 0.7, 0.1]  # Previous pattern

novelty_score = novelty.detectNovelty(pattern_current, pattern_previous)
# Returns: 0.85 (high novelty)

# Set novelty level
novelty.setLevel(novelty_score)
```

### 4.3 Novelty Properties

```python
# Get current novelty level
level = novelty.getLevel()

# Check novelty history
history = novelty.getHistory()

# Clear novelty history
novelty.clearHistory()
```

### 4.4 Novelty in Learning

```python
class NoveltyBasedLearning:
    def __init__(self):
        self.novelty = pynlm.createNovelty()
        self.learned_patterns = []
        
    def learn_from_novelty(self, pattern):
        # Get previous patterns for comparison
        previous_pattern = self.get_last_pattern() if self.learned_patterns else pattern
        
        # Detect novelty
        novelty_score = self.novelty.detectNovelty(pattern, previous_pattern)
        
        # Store novel patterns for learning
        if novelty_score > 0.5:  # Significant novelty
            self.learned_patterns.append(pattern)
            print(f"Stored novel pattern (novelty={novelty_score:.2f})")
            
        # Update novelty level
        self.novelty.setLevel(novelty_score)
        
    def get_last_pattern(self):
        return self.learned_patterns[-1] if self.learned_patterns else None
```

## 5. Prediction Error System

### 5.1 Prediction Error Overview

```python
import pynlm

# Create prediction error system
prediction_error = pynlm.createPredictionError()
```

### 5.2 Prediction Error Calculation

```python
# Compute prediction error
predicted = 0.7  # Expected outcome
actual = 0.9     # Actual outcome

error = prediction_error.computeError(predicted, actual)
# Returns: 0.2 (under-predicted)

# Update prediction
prediction_error.updatePrediction(predicted)
```

### 5.3 Prediction Error Properties

```python
# Get current error level
error = prediction_error.getPredictionError()

# Get error magnitude (absolute value)
magnitude = prediction_error.getMagnitude()

# Get prediction error history
history = prediction_error.getHistory()

# Clear prediction error history
prediction_error.clearHistory()
```

### 5.4 Prediction Error in Learning

```python
class PredictionErrorLearning:
    def __init__(self):
        self.prediction_error = pynlm.createPredictionError()
        self.predictions = []
        
    def learn_from_prediction_error(self, predicted, actual):
        # Compute prediction error
        error = self.prediction_error.computeError(predicted, actual)
        
        # Store prediction
        self.predictions.append(predicted)
        
        # Update prediction for next time
        self.prediction_error.updatePrediction(predicted)
        
        # Use prediction error for learning
        if abs(error) > 0.1:  # Significant error
            self.adjust_behavior(error, predicted, actual)
            
        # Record prediction error
        print(f"Prediction error: {error:.3f} (predicted={predicted}, actual={actual})")
        
    def adjust_behavior(self, error, predicted, actual):
        # Adjust behavior based on prediction error
        # In practice, this would update value functions or policies
        adjustment = error * 0.5  # Learning rate
        print(f"Adjusting behavior by {adjustment:.3f}")
```

## 6. Integrating Neuromodulation Systems

### 6.1 Complete Neuromodulation Integration

```python
class NeuromodulationController:
    def __init__(self):
        self.dopamine = pynlm.createDopamine()
        self.curiosity = pynlm.createCuriosity()
        self.novelty = pynlm.createNovelty()
        self.prediction_error = pynlm.createPredictionError()
        
        # Set weights for integration
        self.curiosity.setNoveltyWeight(0.6)
        self.curiosity.setPredictionErrorWeight(0.4)
        
    def process_experience(self, action, outcome, pattern_current, pattern_previous):
        """Process experience through all neuromodulation systems"""
        
        # 1. Process reward with dopamine
        reward = self.calculate_reward(action, outcome)
        self.dopamine.signalReward(reward)
        self.dopamine.signalRewardPredictionError(reward - self.expected_reward)
        self.expected_reward = reward
        
        # 2. Detect novelty with novelty system
        novelty_score = self.novelty.detectNovelty(pattern_current, pattern_previous)
        self.novelty.setLevel(novelty_score)
        
        # 3. Compute prediction error
        prediction = self.get_prediction_for_action(action)
        error = self.prediction_error.computeError(prediction, outcome)
        self.prediction_error.updatePrediction(prediction)
        
        # 4. Update curiosity
        self.curiosity.update(
            novelty=novelty_score,
            prediction_error=error,
            dt=0.1
        )
        
        # 5. Return integrated neuromodulation signals
        return {
            'dopamine': self.dopamine.getLevel(),
            'curiosity': self.curiosity.getCuriosityLevel(),
            'novelty': self.novelty.getLevel(),
            'prediction_error': self.prediction_error.getPredictionError()
        }
        
    def calculate_reward(self, action, outcome):
        # Calculate reward (in practice, use learned value functions)
        return min(1.0, outcome * 0.8 + 0.2)
        
    def get_prediction_for_action(self, action):
        # Get predicted outcome for action
        # In practice, use value functions or learned models
        return 0.5
```

### 6.2 Neuromodulation-Driven Behavior

```python
class NeuromodulationDrivenBehavior:
    def __init__(self, neuromodulation_controller):
        self.controller = neuromodulation_controller
        self.behavior_history = []
        
    def make_decision(self, world, agent, percept):
        """Make decision influenced by neuromodulation"""
        
        # Get neuromodulation signals
        signals = self.controller.process_experience(
            action=None,  # Current action
            outcome=None, # Current outcome
            pattern_current=percept.getVision(),
            pattern_previous=self.get_last_pattern()
        )
        
        # Make decision based on neuromodulation
        decision = self.make_neuromodulated_decision(world, agent, signals)
        
        # Record behavior
        self.behavior_history.append({
            'signals': signals,
            'decision': decision,
            'step': len(self.behavior_history)
        })
        
        return decision
        
    def make_neuromodulated_decision(self, world, agent, signals):
        """Make decision based on neuromodulation signals"""
        
        # High dopamine -> reward-seeking behavior
        if signals['dopamine'] > 0.5:
            return self.reward_seeking_action(world, agent)
            
        # High curiosity -> exploration behavior
        elif signals['curiosity'] > 0.5:
            return self.exploration_action(world, agent)
            
        # High novelty -> adaptation behavior
        elif signals['novelty'] > 0.5:
            return self.adaptation_action(world, agent)
            
        # High prediction error -> learning behavior
        elif abs(signals['prediction_error']) > 0.3:
            return self.learning_action(world, agent)
            
        # Default behavior
        else:
            return agent.decodeMotorCommand()
```

## 7. Advanced Neuromodulation Techniques

### 7.1 Neuromodulation Timing

```python
# Neuromodulation timing is crucial for learning

# Phasic responses (short-lived)
phasic_dopamine = 0.8  # Quick burst for unexpected reward
phasic_novelty = 0.9  # Quick burst for novel stimulus

# Tonic responses (sustained)
tonic_dopamine = 0.1  # Baseline level for motivation
tonic_curiosity = 0.3  # Baseline level for exploration
```

### 7.2 Neuromodulation Interactions

```python
# Neuromodulators interact with each other

# High novelty + high prediction error = Strong curiosity
def calculate_curiosity_multiplier(novelty, prediction_error):
    if novelty > 0.5 and abs(prediction_error) > 0.3:
        return 1.5  # Enhanced curiosity
    elif novelty > 0.3:
        return 1.2  # Moderately enhanced
    else:
        return 1.0  # Normal

# High dopamine + high novelty = Enhanced learning
def calculate_learning_rate(dopamine, novelty):
    base_rate = 0.1
    if dopamine > 0.5 and novelty > 0.5:
        return base_rate * 2.0  # Enhanced learning
    return base_rate
```

## 8. Neuromodulation Performance Monitoring

### 8.1 Neuromodulation Statistics

```python
def monitor_neuromodulation(neuromodulation_controller):
    """Monitor neuromodulation system performance"""
    
    signals = neuromodulation_controller.process_experience(
        action=None, outcome=None, pattern_current=[], pattern_previous=[]
    )
    
    print("Neuromodulation System Performance:")
    print(f"  Dopamine: {signals['dopamine']:.3f}")
    print(f"  Curiosity: {signals['curiosity']:.3f}")
    print(f"  Novelty: {signals['novelty']:.3f}")
    print(f"  Prediction Error: {signals['prediction_error']:.3f}")
    
    # Calculate neuromodulation balance
    balance = {
        'reward_focus': signals['dopamine'],
        'exploration_drive': signals['curiosity'],
        'adaptation_need': signals['novelty'],
        'learning_need': abs(signals['prediction_error'])
    }
    
    return balance
```

### 8.2 Neuromodulation Efficiency

```python
# Calculate neuromodulation efficiency
neuromodulation_efficiency = {
    'reward_efficiency': min(1.0, signals['dopamine']),
    'exploration_efficiency': signals['curiosity'],
    'adaptation_efficiency': signals['novelty'],
    'learning_efficiency': min(1.0, abs(signals['prediction_error']))
}

overall_efficiency = sum(neuromodulation_efficiency.values()) / len(neuromodulation_efficiency)
print(f"Overall neuromodulation efficiency: {overall_efficiency:.1%}")
```

## 9. Troubleshooting Neuromodulation Issues

### 9.1 Neuromodulator Problems

**Problem:** Dopamine levels are always too high.
**Solution:** Reduce reward strength, adjust prediction error, or implement dopamine decay.

**Problem:** Curiosity is never triggered.
**Solution:** Increase novelty weight, reduce prediction error weight, or adjust novelty sensitivity.

### 9.2 Integration Problems

**Problem:** Neuromodulators conflicting.
**Solution:** Adjust weight balances or implement conflict resolution mechanisms.

**Problem:** Learning is too slow.
**Solution:** Increase neuromodulation sensitivity or adjust learning rates.

## 10. Practical Neuromodulation Applications

### 10.1 Reward-Based Learning in Games

```python
class GameLearningSystem:
    def __init__(self):
        self.dopamine = pynlm.createDopamine()
        self.curiosity = pynlm.createCuriosity()
        
    def process_game_event(self, event_type, outcome):
        """Process game events through neuromodulation"""
        
        if event_type == "reward":
            # Process reward
            self.dopamine.signalReward(outcome)
            
        elif event_type == "exploration":
            # Process exploration
            self.curiosity.update(novelty=0.7, prediction_error=0.2, dt=0.1)
            
        elif event_type == "challenge":
            # Process challenge
            prediction_error = self.calculate_prediction_error(outcome)
            self.curiosity.update(novelty=0.3, prediction_error=prediction_error, dt=0.1)
```

### 10.2 Neuromodulation in Robotics

```python
class RobotLearningSystem:
    def __init__(self):
        self.novelty = pynlm.createNovelty()
        self.prediction_error = pynlm.createPredictionError()
        
    def interact_with_environment(self, sensor_data):
        """Interact with environment using neuromodulation"""
        
        # Detect novel situations
        if self.is_novel_situation(sensor_data):
            # Take novel action
            action = self.take_novel_action()
        else:
            # Take learned action
            action = self.take_learned_action()
            
        # Get outcome
        outcome = self.get_outcome(action)
        
        # Process with prediction error
        prediction = self.get_predicted_outcome(action)
        error = self.prediction_error.computeError(prediction, outcome)
        
        # Update novelty detection
        self.novelty.update(0.0, 0.0, 0.1)  # Placeholder update
```

## 11. Next Steps

### 11.1 Continue to Tutorial 4

Move on to **Tutorial 4: Prediction and Planning** to learn how prediction and planning systems work with neuromodulation.

### 11.2 Advanced Neuromodulation

Explore the advanced examples:

- `neuromodulation_example.py` - Detailed neuromodulation demonstrations
- `prediction_planning_example.py` - Neuromodulation in prediction and planning
- `development_example.py` - Neuromodulation in development

### 11.3 Neuromodulation Research

Study the neuromodulation-related documentation:

- [Neuromodulation Systems Documentation](https://kilo.ai/docs/neuromodulation)
- [Dopamine Research](https://kilo.ai/docs/dopamine)
- [Curiosity and Exploration](https://kilo.ai/docs/curiosity)

## 12. Summary

Neuromodulation systems are the key to adaptive, intelligent behavior in NLM:

1. **Dopamine** drives reward-based learning and motivation
2. **Curiosity** promotes exploration and intrinsic motivation
3. **Novelty** detects change and promotes adaptability
4. **PredictionError** signals learning opportunities through prediction failures

These systems work together to create intelligent behavior that:

- **Rewards exploration** of novel environments
- **Rewards achievement** of goals
- **Adapts to change** in the environment
- **Learns from mistakes** and prediction errors

---

*Tutorial 3 completed successfully! Ready for Tutorial 4.*
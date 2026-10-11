# Tutorial 5: Neuromodulation and Reward-Based Learning

This tutorial explores the fascinating world of neuromodulation and reward-based learning in NLM. Neuromodulation systems provide the motivational and regulatory signals that guide intelligent behavior, allowing the brain to:

- Evaluate the value of actions
- Explore novel experiences
- Balance exploration and exploitation
- Adapt to changing environments
- Learn from prediction errors

## Learning Objectives

After completing this tutorial, you should be able to:

1. **Understand neuromodulation systems** (dopamine, curiosity, novelty, prediction error)
2. **Configure and use reward-based learning** in your NLM brain
3. **Monitor neuromodulation signals** and their effects on behavior
4. **Design experiments to study motivation and learning**
5. **Debug neuromodulation issues** and optimize reward systems
6. **Create custom neuromodulation rules** for specific applications

## What You'll Learn

- The biological and computational principles behind neuromodulation
- How different neuromodulators coordinate learning and behavior
- Reward prediction, prediction errors, and credit assignment
- Exploration vs. exploitation trade-offs
- How to measure and analyze motivational signals

## Prerequisites

- Understanding of Tutorials 1-4 (brain creation, sensory-motor loop, learning, memory)
- Basic knowledge of reinforcement learning concepts

## Step 1: Understanding Neuromodulation Systems

### The Major Neuromodulators in NLM

NLM implements several key neuromodulation systems:

#### 1. Dopamine (Reward System)

**What it is:** The brain's reward signal that indicates prediction errors and action value

**How it works:**
- **Positive prediction error**: Predicted reward < actual reward → Dopamine increases → Reinforce actions
- **Negative prediction error**: Predicted reward > actual reward → Dopamine decreases → Suppress actions
- **Baseline dopamine**: Overall action value and motivation level

**Functions:**
- Reward prediction and learning
- Action selection and value comparison
- Motivation and effort allocation
- Reinforcement learning

#### 2. Curiosity (Exploration System)

**What it is:** Drives exploration of novel and interesting stimuli

**How it works:**
- Increases when encountering new or surprising situations
- Decreases with familiar, predictable environments
- Balances with reward signals to explore vs. exploit

**Functions:**
- Novelty detection
- Exploration motivation
- Information seeking behavior
- Behavioral flexibility

#### 3. Novelty (Information System)

**What it is:** Detects surprising or unexpected events

**How it works:**
- Compares current input to learned patterns
- Signals when something is different or unexpected
- Provides input for learning new information

**Functions:**
- Pattern detection
- Change detection
- Learning trigger
- Adaptation to new environments

#### 4. Prediction Error (Learning System)

**What it is:** The difference between expected and actual outcomes

**How it works:**
- **Temporal difference learning**: Updates value estimates based on prediction errors
- **Credit assignment**: Distributes learning signals across time and actions
- **Learning rate control**: Modulates learning based on prediction error magnitude

**Functions:**
- Learning signal generation
- Value function updates
- Policy optimization
- Future planning

### Neuromodulation Integration with Neural Processing

```cpp
// Neuromodulation affects neural processing:
dopamine->modulateNeuralExcitability();
curiosity->driveExploration();
novelty->triggerPatternLearning();
predictionError->updateValueEstimates();

// Neural processing affects neuromodulation:
workingMemory->provideContextForPrediction();
episodicMemory->supplyPastExperiences();
 proceduralMemory->guideActionSelection();
 semanticMemory->supportPlanning();
```

## Step 2: Neuromodulation Configuration and Setup

### Configuring Neuromodulation Systems

```python
import pynlm

def configure_neuromodulation():
    """Configure all neuromodulation systems"""
    
    print("=== Neuromodulation System Configuration ===")
    
    # Base configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    
    # Dopamine system configuration
    config.set("neuromod.dopamine.enabled", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.dopamine.baseline", 0.5)
    config.set("neuromod.dopamine.learning_rate", 0.01)
    
    # Curiosity system configuration
    config.set("neuromod.curiosity.enabled", True)
    config.set("neuromod.curiosity.threshold", 0.1)
    config.set("neuromod.curiosity.decay_rate", 0.001)
    config.set("neuromod.curiosity.exploration_bonus", 0.5)
    
    # Novelty system configuration
    config.set("neuromod.novelty.enabled", True)
    config.set("neuromod.novelty.sensitivity", 0.5)
    config.set("neuromod.novelty.memory_comparison", True)
    config.set("neuromod.novelty.reduction_rate", 0.01)
    
    # Prediction error system configuration
    config.set("neuromod.prediction_error.enabled", True)
    config.set("neuromod.prediction_error.update_rate", 0.1)
    config.set("neuromod.prediction_error.discount_factor", 0.9)
    
    return config

def print_neuromodulation_config(config):
    """Display current neuromodulation configuration"""
    
    print(f"\nNeuromodulation System Configuration:")
    
    for system in ['dopamine', 'curiosity', 'novelty', 'prediction_error']:
        enabled_key = f"neuromod.{system}.enabled"
        if config.get(enabled_key):
            print(f"  {system.title()} System: {'Enabled' if config.get(enabled_key) else 'Disabled'}")
            
            # Print system-specific parameters
            if system == 'dopamine':
                print(f"    Scale: {config.get('neuromod.dopamine.scale', 'N/A')}")
                print(f"    Baseline: {config.get('neuromod.dopamine.baseline', 'N/A')}")
            
            elif system == 'curiosity':
                print(f"    Threshold: {config.get('neuromod.curiosity.threshold', 'N/A')}")
                print(f"    Exploration bonus: {config.get('neuromod.curiosity.exploration_bonus', 'N/A')}")
            
            elif system == 'novelty':
                print(f"    Sensitivity: {config.get('neuromod.novelty.sensitivity', 'N/A')}")
            
            elif system == 'prediction_error':
                print(f"    Update rate: {config.get('neuromod.prediction_error.update_rate', 'N/A')}")
```

### Neuromodulation-Enhanced Brain Creation

```python
import pynlm

def create_neuromodulated_brain():
    """Create a brain with comprehensive neuromodulation"""
    
    print("=== Neuromodulated Brain Setup ===")
    
    config = configure_neuromodulation()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with neuromodulation interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(None)  # Will be initialized with world later
    
    # Enable all neuromodulation subsystems
    agent.enableRewardModulation(True)   # Dopamine-based reward learning
    agent.enableCuriosity(True)         # Curiosity-driven exploration
    agent.enableDevelopment(True)       # Development affects neuromodulation
    
    print(f"   Neuromodulated brain created:")
    print(f"   - {brain.getTotalNeuronCount()} neurons")
    print(f"   - {brain.getTotalSynapseCount()} synapses")
    print(f"   - Reward modulation enabled")
    print(f"   - Curiosity module enabled")
    print(f"   - Development tracking enabled")
    
    return brain, agent
```

## Step 3: Reward-Based Learning Implementation

### Reward Learning Basics

Reward-based learning allows the brain to:

- Evaluate action values
- Learn from positive and negative outcomes
- Optimize behavior for maximum reward
- Handle exploration-exploitation trade-offs

```python
import pynlm

def demonstrate_reward_learning(brain, agent, world, steps=300):
    """Demonstrate reward-based learning functionality"""
    
    print(f"\n=== Reward-Based Learning Demonstration ===")
    print(f"Testing reward-based learning over {steps} steps:\n")
    
    # Track reward learning metrics
    reward_metrics = {
        'rewards_received': [],
        'action_values': [],
        'prediction_errors': [],
        'learning_curves': [],
        'behavior_changes': []
    }
    
    for step in range(steps):
        # Update world and get percept
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain (computes action values)
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        
        # Execute action and get reward
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate and apply reward
        reward = calculate_environment_reward(world, action, step)
        predicted_reward = estimate_predicted_reward(agent, action)
        
        # Apply reward modulation (creates prediction error)
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Record metrics
        reward_metrics['rewards_received'].append(reward)
        reward_metrics['action_values'].append(agent.getActionValue(action))
        reward_metrics['prediction_errors'].append(agent.getPredictionError())
        
        # Track behavior changes
        if step % 50 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            
            print(f"   Step {step:3d}: Action={action:15} Reward={reward:+6.2f} "
                  f"Error={agent.getPredictionError():6.3f} "
                  f"Curiosity={curiosity:5.3f} Novelty={novelty:5.3f}")
    
    # Analyze reward learning performance
    print(f"\nReward Learning Analysis:")
    
    if reward_metrics['rewards_received']:
        avg_reward = sum(reward_metrics['rewards_received']) / len(reward_metrics['rewards_received'])
        total_reward = sum(reward_metrics['rewards_received'])
        avg_error = sum(abs(e) for e in reward_metrics['prediction_errors']) / len(reward_metrics['prediction_errors'])
        
        print(f"   Average reward per step: {avg_reward:.3f}")
        print(f"   Total reward accumulated: {total_reward:.1f}")
        print(f"   Average prediction error: {avg_error:.3f}")
        
        # Learning success criteria
        if avg_reward > 0.1:
            print("   ✓ Reward-based learning successful")
        elif avg_reward > 0.0:
            print("   = Neutral reward learning")
        else:
            print("   ! Negative reward - may need strategy adjustment")
        
        if avg_error < 0.2:
            print("   ✓ Accurate predictions")
        elif avg_error < 0.5:
            print("   = Moderate prediction accuracy")
        else:
            print("   ! Poor prediction accuracy - may need better learning rate")
    
    return reward_metrics

def calculate_environment_reward(world, action, step):
    """Calculate reward for action in environment"""
    
    # Base reward components
    reward = 0.0
    
    # Energy reward
    body = world.getAgentBody()
    if body.getEnergy() > 50:
        reward += 0.1  # Positive reward for having energy
    else:
        reward -= 0.2  # Negative reward for low energy
    
    # Resource reward
    percept = world.getSensoryPercept()
    for obj in percept.getVisibleObjects():
        if obj.getType() == pynlm.WorldObjectType.Resource:
            reward += obj.getValue() * 0.5  # Reward for finding resources
        elif obj.getType() == pynlm.WorldObjectType.Hazard:
            reward -= abs(obj.getValue()) * 0.3  # Penalty for hazards
    
    # Novelty bonus
    novelty_bonus = calculate_novelty_bonus(world, step)
    reward += novelty_bonus
    
    # Action-specific rewards
    if action == pynlm.ActionType.Interact:
        reward += 0.5  # Bonus for interacting with environment
    elif action == pynlm.ActionType.Eat:
        reward += 1.0  # Bonus for eating
    
    return reward

def calculate_novelty_bonus(world, step):
    """Calculate novelty-based exploration bonus"""
    
    # Simplified novelty calculation
    # In a real system, this would compare current state to memory
    novelty = min(1.0, step / 100.0)  # Increases with exploration
    bonus = novelty * 0.2  # 20% maximum bonus
    
    return bonus

def estimate_predicted_reward(agent, action):
    """Estimate predicted reward for action"""
    
    # Simplified prediction
    # In a real system, this would use learned value functions
    predicted = 0.0
    
    # Base prediction based on action type
    if action == pynlm.ActionType.MoveForward:
        predicted = 0.1
    elif action == pynlm.ActionType.Interact:
        predicted = 0.3
    elif action == pynlm.ActionType.Eat:
        predicted = 0.5
    
    # Modulate by agent's curiosity
    curiosity = agent.getCuriosityLevel()
    if curiosity > 0.7:
        predicted += 0.2  # High curiosity = higher expectations
    
    return predicted
```

### Credit Assignment and Learning Credit

```python
import pynlm

class CreditAssignmentSystem:
    """System for distributing learning credit across actions and time"""
    
    def __init__(self):
        self.action_history = []
        self.reward_history = []
        self.prediction_history = []
        self.credit_assignments = {}
    
    def record_action_outcome(self, action, reward, prediction):
        """Record action outcome for credit assignment"""
        
        self.action_history.append(action)
        self.reward_history.append(reward)
        self.prediction_history.append(prediction)
        
        # Calculate prediction error
        prediction_error = reward - prediction
        
        # Assign credit to recent actions
        self.assign_credit(action, prediction_error)
    
    def assign_credit(self, action, prediction_error, time_horizon=10):
        """Assign learning credit to actions over time"""
        
        # Find recent occurrences of this action
        action_indices = [i for i, a in enumerate(self.action_history) 
                         if a == action]
        
        # Distribute credit backwards in time
        for idx in reversed(action_indices[-time_horizon:]):
            # Credit decays with time
            time_since_action = len(self.action_history) - idx - 1
            credit = prediction_error * (0.9 ** time_since_action)
            
            if idx not in self.credit_assignments:
                self.credit_assignments[idx] = 0.0
            self.credit_assignments[idx] += credit
    
    def get_action_credit(self, action, step):
        """Get accumulated credit for action at given step"""
        
        if step not in self.credit_assignments:
            return 0.0
        
        # Sum credit for this action
        total_credit = 0.0
        for idx, credit in self.credit_assignments.items():
            if idx < step and self.action_history[idx] == action:
                total_credit += credit
        
        return total_credit
    
    def analyze_credit_patterns(self):
        """Analyze credit assignment patterns"""
        
        print(f"\nCredit Assignment Analysis:")
        
        if not self.credit_assignments:
            print("   ! No credit assignments recorded")
            return
        
        # Find actions with highest credit
        action_credits = {}
        for idx, credit in self.credit_assignments.items():
            if idx < len(self.action_history):
                action = self.action_history[idx]
                if action not in action_credits:
                    action_credits[action] = 0.0
                action_credits[action] += credit
        
        if action_credits:
            max_credit = max(action_credits.values())
            print(f"   Maximum credit assigned: {max_credit:.3f}")
            
            for action, credit in action_credits.items():
                proportion = credit / max_credit
                print(f"   {action}: {credit:.3f} ({proportion:.1%} of max)")
        
        # Check for credit decay
        if len(self.action_history) > 5:
            recent_actions = self.action_history[-5:]
            recent_indices = self.action_history[-5:]
            recent_credits = [self.credit_assignments.get(i, 0.0) for i in recent_indices]
            
            if recent_credits:
                avg_recent_credit = sum(recent_credits) / len(recent_credits)
                print(f"   Average credit for recent actions: {avg_recent_credit:.3f}")

class ExplorationExploitationSystem:
    """System for balancing exploration and exploitation"""
    
    def __init__(self, exploration_rate=0.3, exploitation_threshold=0.7):
        self.exploration_rate = exploration_rate
        self.exploitation_threshold = exploitation_threshold
        self.visit_counts = {}
        self.value_estimates = {}
    
    def select_action(self, available_actions, step):
        """Select action using epsilon-greedy or UCB policy"""
        
        # Explore with probability
        if step < 2 or random.random() < self.exploration_rate:
            return random.choice(available_actions)
        
        # Exploit based on value estimates
        else:
            # Choose action with highest estimated value
            best_action = max(available_actions, 
                            key=lambda a: self.value_estimates.get(a, 0.0))
            return best_action
    
    def update_value_estimate(self, action, reward, step):
        """Update value estimate for action"""
        
        if action not in self.value_estimates:
            self.value_estimates[action] = 0.0
        
        # Incremental average
        n = self.visit_counts.get(action, 0) + 1
        self.visit_counts[action] = n
        self.value_estimates[action] = (
            (self.value_estimates[action] * (n - 1) + reward) / n
        )
    
    def calculate_ucb_value(self, action, step, exploration_bonus=0.5):
        """Calculate UCB (Upper Confidence Bound) value for action"""
        
        if action not in self.visit_counts:
            return float('inf')  # Force exploration of unvisited actions
        
        # Average reward
        avg_reward = self.value_estimates[action]
        
        # Exploration bonus (higher for less visited actions)
        visit_count = self.visit_counts[action]
        exploration_bonus = exploration_bonus * math.sqrt(math.log(step) / visit_count)
        
        return avg_reward + exploration_bonus
```

## Step 4: Curiosity and Exploration Learning

### Curiosity-Driven Learning

Curiosity drives the agent to explore novel and interesting experiences:

```python
import pynlm

def demonstrate_curiosity_driven_learning(brain, agent, world, steps=400):
    """Demonstrate curiosity-driven learning"""
    
    print(f"\n=== Curiosity-Driven Learning ===")
    print(f"Testing curiosity-driven learning over {steps} steps:\n")
    
    # Track curiosity and exploration metrics
    curiosity_metrics = {
        'curiosity_levels': [],
        'novelty_encounters': [],
        'exploration_patterns': [],
        'learning_events': []
    }
    
    exploration_activities = []
    
    for step in range(steps):
        world.update(0.1)
        
        # Monitor curiosity before action
        curiosity_before = agent.getCuriosityLevel()
        novelty_before = agent.getNoveltyLevel()
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action selection influenced by curiosity
        action = select_action_with_curiosity(agent, world)
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward (triggers learning)
        reward = calculate_curiosity_reward(world, action)
        agent.applyRewardModulation(reward, 0.0)
        
        # Record curiosity dynamics
        curiosity_after = agent.getCuriosityLevel()
        novelty_after = agent.getNoveltyLevel()
        
        curiosity_change = curiosity_after - curiosity_before
        novelty_change = novelty_after - novelty_before
        
        curiosity_metrics['curiosity_levels'].append(curiosity_after)
        curiosity_metrics['novelty_encounters'].append(novelty_after)
        curiosity_metrics['exploration_patterns'].append(curiosity_change)
        curiosity_metrics['learning_events'].append(reward)
        
        # Record exploration activities
        exploration_activities.append({
            'step': step,
            'action': action,
            'curiosity': curiosity_after,
            'novelty': novelty_after,
            'reward': reward
        })
        
        if step % 50 == 0:
            print(f"   Step {step:3d}: Action={action:15} "
                  f"Curiosity={curiosity_after:5.3f} Novelty={novelty_after:5.3f} "
                  f"Reward={reward:6.2f}")
    
    # Analyze curiosity-driven learning
    print(f"\nCuriosity-Driven Learning Analysis:")
    
    if curiosity_metrics['curiosity_levels']:
        initial_curiosity = curiosity_metrics['curiosity_levels'][0]
        final_curiosity = curiosity_metrics['curiosity_levels'][-1]
        curiosity_trend = final_curiosity - initial_curiosity
        
        print(f"   Curiosity trajectory: {initial_curiosity:.3f} → {final_curiosity:.3f}")
        print(f"   Curiosity trend: {curiosity_trend:+.3f}")
        
        if curiosity_trend > 0.2:
            print("   ✓ Curiosity-driven exploration successful")
        elif curiosity_trend > 0.05:
            print("   = Moderate curiosity-driven learning")
        elif curiosity_trend > -0.1:
            print("   - Stable curiosity level")
        else:
            print("   ! Decreasing curiosity - may need more novelty")
        
        # Novelty analysis
        avg_novelty = sum(curiosity_metrics['novelty_encounters']) / len(curiosity_metrics['novelty_encounters'])
        print(f"   Average novelty detection: {avg_novelty:.2f}")
        
        if avg_novelty > 0.3:
            print("   ✓ Effective novelty detection")
        elif avg_novelty > 0.1:
            print("   = Moderate novelty detection")
        else:
            print("   ! Low novelty detection - may need different environments")
    
    return curiosity_metrics, exploration_activities

def select_action_with_curiosity(agent, world):
    """Select action based on curiosity level"""
    
    curiosity = agent.getCuriosityLevel()
    
    if curiosity > 0.7:
        # High curiosity → explore new areas
        return explore_new_area(world)
    elif curiosity > 0.3:
        # Medium curiosity → balance exploration and exploitation
        return balanced_exploration(world)
    else:
        # Low curiosity → exploit known areas
        return exploit_known_areas(world)

def explore_new_area(world):
    """Generate action for exploring new areas"""
    
    body = world.getAgentBody()
    
    # Move in random direction (exploration)
    import random
    directions = [
        pynlm.ActionType.MoveForward,
        pynlm.ActionType.TurnLeft,
        pynlm.ActionType.TurnRight,
    ]
    
    return random.choice(directions)

def balanced_exploration(world):
    """Generate action balancing exploration and exploitation"""
    
    body = world.getAgentBody()
    percept = world.getSensoryPercept()
    
    # Check for interesting features to explore
    visible_objects = percept.getVisibleObjects()
    
    # Explore interesting features
    if visible_objects:
        for obj in visible_objects:
            if obj.getType() == pynlm.WorldObjectType.Resource:
                return navigate_toward(world, obj.getX(), obj.getY())
            elif obj.getType() == pynlm.WorldObjectType.Marker:
                return navigate_toward(world, obj.getX(), obj.getY())
    
    # Default balanced behavior
    return pynlm.ActionType.MoveForward

def exploit_known_areas(world):
    """Generate action exploiting known areas"""
    
    body = world.getAgentBody()
    
    # Return to starting area (center)
    if body.getX() > 10:
        return pynlm.ActionType.TurnLeft
    elif body.getX() < 10:
        return pynlm.ActionType.TurnRight
    else:
        if body.getY() > 10:
            return pynlm.ActionType.TurnLeft
        else:
            return pynlm.ActionType.MoveForward

def calculate_curiosity_reward(world, action):
    """Calculate curiosity-based reward"""
    
    reward = 0.0
    
    # Exploration bonus
    if action in [pynlm.ActionType.TurnLeft, pynlm.ActionType.TurnRight]:
        reward += 0.2  # Bonus for exploration actions
    
    # Novelty bonus
    percept = world.getSensoryPercept()
    if len(percept.getVisibleObjects()) > 3:
        reward += 0.3  # Bonus for seeing many objects (novel)
    
    return reward
```

## Step 5: Prediction Error and Learning Optimization

### Prediction Error-Driven Learning

Prediction errors drive learning by highlighting the difference between expected and actual outcomes:

```python
import pynlm

def demonstrate_prediction_error_learning(brain, agent, world, steps=350):
    """Demonstrate prediction error-driven learning"""
    
    print(f"\n=== Prediction Error Learning ===")
    print(f"Testing prediction error learning over {steps} steps:\n")
    
    # Track prediction error dynamics
    prediction_metrics = {
        'predicted_values': [],
        'actual_rewards': [],
        'prediction_errors': [],
        'learning_rates': [],
        'value_updates': []
    }
    
    for step in range(steps):
        world.update(0.1)
        
        # Get current state
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Select action
        action = agent.decodeMotorCommand()
        
        # Predict expected reward
        predicted_reward = predict_future_reward(agent, action)
        
        # Execute action and get actual reward
        world.applyMotorCommand(action, world.getSimulationTime())
        actual_reward = calculate_prediction_error_reward(world, action)
        
        # Apply reward modulation (creates prediction error)
        agent.applyRewardModulation(actual_reward, predicted_reward)
        
        # Track prediction error
        prediction_error = actual_reward - predicted_reward
        
        prediction_metrics['predicted_values'].append(predicted_reward)
        prediction_metrics['actual_rewards'].append(actual_reward)
        prediction_metrics['prediction_errors'].append(prediction_error)
        
        # Calculate adaptive learning rate
        learning_rate = calculate_adaptive_learning_rate(prediction_error, step)
        prediction_metrics['learning_rates'].append(learning_rate)
        
        # Track value updates
        value_update = learning_rate * prediction_error
        prediction_metrics['value_updates'].append(value_update)
        
        if step % 50 == 0:
            print(f"   Step {step:3d}: Predict={predicted_reward:6.2f} Actual={actual_reward:6.2f} "
                  f"Error={prediction_error:6.3f} LR={learning_rate:5.3f}")
    
    # Analyze prediction error learning
    print(f"\nPrediction Error Learning Analysis:")
    
    if prediction_metrics['prediction_errors']:
        avg_prediction_error = sum(prediction_metrics['prediction_errors']) / len(prediction_metrics['prediction_errors'])
        avg_learning_rate = sum(prediction_metrics['learning_rates']) / len(prediction_metrics['learning_rates'])
        
        print(f"   Average prediction error: {abs(avg_prediction_error):.3f}")
        print(f"   Average learning rate: {avg_learning_rate:.3f}")
        
        if abs(avg_prediction_error) < 0.1:
            print("   ✓ Accurate predictions")
        elif abs(avg_prediction_error) < 0.3:
            print("   = Reasonable prediction accuracy")
        else:
            print("   ! Large prediction errors - check value estimates")
        
        if avg_learning_rate > 0.1:
            print("   ✓ Effective learning rate")
        elif avg_learning_rate > 0.05:
            print("   = Moderate learning rate")
        else:
            print("   ! Low learning rate - may need adjustment")
        
        # Check learning convergence
        if len(prediction_metrics['prediction_errors']) > 10:
            recent_errors = prediction_metrics['prediction_errors'][-5:]
            error_variance = max(recent_errors) - min(recent_errors)
            
            print(f"   Recent error variance: {error_variance:.3f}")
            
            if error_variance < 0.1:
                print("   ✓ Learning appears to be converging")
            else:
                print("   ! Learning may not be converging - consider tuning")
    
    return prediction_metrics

def predict_future_reward(agent, action):
    """Predict future reward for action using learned values"""
    
    # Simplified prediction
    # In a real system, this would use learned value functions
    
    base_prediction = 0.0
    
    # Action-based predictions
    if action == pynlm.ActionType.MoveForward:
        base_prediction = 0.2
    elif action == pynlm.ActionType.Interact:
        base_prediction = 0.4
    elif action == pynlm.ActionType.Eat:
        base_prediction = 0.6
    
    # Modulate by agent's curiosity and experience
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    # Higher curiosity increases uncertainty (wider prediction bounds)
    if curiosity > 0.5:
        base_prediction += 0.1
    
    # Novelty affects predictions
    if novelty > 0.3:
        base_prediction += 0.2
    
    return base_prediction

def calculate_prediction_error_reward(world, action):
    """Calculate actual reward for prediction error learning"""
    
    # Calculate actual outcome-based reward
    reward = 0.0
    
    # Energy changes
    body = world.getAgentBody()
    if body.getEnergy() > 50:
        reward += 0.2
    else:
        reward -= 0.1
    
    # Resource and hazard encounters
    percept = world.getSensoryPercept()
    for obj in percept.getVisibleObjects():
        if obj.getType() == pynlm.WorldObjectType.Resource:
            reward += obj.getValue() * 0.3
        elif obj.getType() == pynlm.WorldObjectType.Hazard:
            reward -= abs(obj.getValue()) * 0.2
    
    # Action-specific outcomes
    if action == pynlm.ActionType.Interact:
        reward += 0.3
    elif action == pynlm.ActionType.Eat:
        reward += 0.5
    
    return reward

def calculate_adaptive_learning_rate(prediction_error, step):
    """Calculate adaptive learning rate based on prediction error"""
    
    # Base learning rate
    base_lr = 0.1
    
    # Adapt learning rate based on prediction error
    if abs(prediction_error) > 0.5:
        # Large prediction error → faster learning
        learning_rate = min(0.5, base_lr * (1 + abs(prediction_error)))
    elif abs(prediction_error) > 0.1:
        # Moderate prediction error → moderate learning
        learning_rate = base_lr
    else:
        # Small prediction error → slower learning
        learning_rate = base_lr * 0.5
    
    # Decay learning rate over time
    decay_factor = 0.99 ** step
    learning_rate *= decay_factor
    
    return learning_rate
```

## Step 6: Complete Tutorial Example

```python
import pynlm
import time
import random
import math

def run_complete_tutorial_5():
    """Complete tutorial 5: Neuromodulation and Reward-Based Learning"""
    
    print("=== NLM Tutorial 5: Neuromodulation and Reward-Based Learning ===")
    print("This tutorial explores sophisticated learning mechanisms driven by neuromodulation.\n")
    
    # 1. Setup world and agent
    print("1. Creating world and agent...")
    world = create_simple_world()
    world = populate_world(world)
    
    brain, agent = create_neuromodulated_brain()
    agent = initialize_agent_with_world(agent, world)
    
    # 2. Demonstrate reward-based learning
    print("\n2. Demonstrating reward-based learning...")
    reward_metrics = demonstrate_reward_learning(world, agent, brain, 200)
    
    world.reset()
    brain.reset()
    brain.initialize()
    agent.initialize(None)
    
    # 3. Demonstrate curiosity-driven learning
    print("\n3. Demonstrating curiosity-driven learning...")
    curiosity_metrics, exploration_activities = demonstrate_curiosity_driven_learning(world, agent, brain, 250)
    
    world.reset()
    brain.reset()
    brain.initialize()
    agent.initialize(None)
    
    # 4. Demonstrate prediction error learning
    print("\n4. Demonstrating prediction error learning...")
    prediction_metrics = demonstrate_prediction_error_learning(world, agent, brain, 180)
    
    # 5. Show integration of all neuromodulation systems
    print("\n5. Demonstrating integrated neuromodulation...")
    demonstrate_integrated_neuromodulation(world, agent, brain, 200)
    
    # 6. Save results
    print("\n6. Saving neuromodulation-enhanced brain state...")
    brain.save("tutorial5_neuromodulation.bin")
    print("   ✓ Neuromodulation brain saved")
    
    print("\n=== Tutorial 5 Complete ===")
    print("Your brain now has sophisticated neuromodulation capabilities:")
    print("✓ Reward-based learning with prediction errors")
    print("✓ Curiosity-driven exploration and information seeking")
    print("✓ Novelty detection and change detection")
    print("✓ Adaptive learning rates and credit assignment")
    print("✓ Exploration-exploitation balance optimization")
    print("✓ Credit assignment across time and actions")
    print("✓ Integration of all neuromodulators for intelligent behavior")

if __name__ == "__main__":
    run_complete_tutorial_5()
```

## Advanced Neuromodulation Topics

### 1. Neuromodulator Interactions

Neuromodulators interact in complex ways:

- **Dopamine and curiosity**: Dopamine modulates curiosity-driven exploration
- **Novelty and reward**: Novelty provides input for reward prediction
- **Prediction error and neuromodulators**: All neuromodulators respond to prediction errors
- **Development and neuromodulation**: Neuromodulation changes with brain development

### 2. Neuromodulation in Learning

Neuromodulation affects learning in multiple ways:

- **Signal selection**: Which experiences are worth learning from
- **Learning rate modulation**: How quickly to learn from experiences
- **Memory consolidation**: Which memories are strengthened
- **Behavioral guidance**: What actions to prioritize

### 3. Computational Models of Neuromodulation

Advanced models:

- **TD(λ) learning**: Temporal difference learning with eligibility traces
- **Actor-critic architectures**: Separate value estimation and action selection
- **Policy gradient methods**: Direct optimization of behavior policies
- **Hierarchical reinforcement learning**: Multi-level control

### 4. Neuromodulation and Decision-Making

Neuromodulation influences decision-making:

- **Value-based decisions**: Compare predicted rewards across actions
- **Model-based decisions**: Use internal models of environment dynamics
- **Meta-learning**: Learn how to learn in different contexts
- **Transfer learning**: Apply knowledge across domains

## Summary

Tutorial 5 has provided a comprehensive exploration of neuromodulation and reward-based learning in NLM:

1. **Multiple neuromodulators** working together to guide learning and behavior
2. **Reward prediction and error signals** driving adaptive learning
3. **Curiosity and novelty detection** promoting exploration and discovery
4. **Prediction error learning** for optimizing future predictions
5. **Credit assignment mechanisms** for distributing learning across time
6. **Exploration-exploitation balance** for efficient behavior

The brain you've created can now:

- Learn from rewards and prediction errors
- Explore novel and interesting experiences
- Balance exploration and exploitation intelligently
- Assign credit across time and actions
- Adapt learning rates based on prediction accuracy
- Integrate multiple motivational signals for intelligent behavior

These capabilities enable the brain to learn efficiently from experience, explore environments effectively, and make optimal decisions under uncertainty.

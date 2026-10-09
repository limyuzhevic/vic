# NLM Neuromodulation Guide

## Overview

This guide provides comprehensive instructions for working with NLM's neuromodulation system. Neuromodulation is a crucial component of NLM that enables:

1. **Reward Learning**: Use rewards to guide behavior
2. **Curiosity-Driven Exploration**: Seek novel and interesting experiences
3. **Novelty Detection**: Identify and respond to new situations
4. **Attention Regulation**: Focus processing on relevant information
5. **Arousal Modulation**: Adjust overall system responsiveness
6. **Motivation Control**: Balance exploration vs. exploitation

Neuromodulation in NLM is based on biological neuromodulators like:
- **Dopamine**: Reward prediction error, reinforcement learning
- **Curiosity**: Intrinsic motivation, exploration
- **Novelty**: Detection of new experiences
- **Attention**: Selection and focus

## Neuromodulation System Architecture

### 1. Dopamine System

**Purpose**: Encode reward prediction errors and guide reinforcement learning.

**Components**:
- **Reward Detection**: Identify rewarding outcomes
- **Prediction Error**: Compare expected vs. actual rewards
- **Learning Signal**: Use errors to update expectations
- **Plasticity Modulation**: Adjust neural connections based on rewards

**Key Features**:
- **Spiking Pattern Modulation**: Dopamine affects neural firing patterns
- **Synaptic Plasticity**: Dopamine gates learning processes
- **Reward Prediction**: Anticipate future rewards
- **Motivation Signals**: Drive goal-directed behavior

### 2. Curiosity System

**Purpose**: Generate intrinsic motivation for exploration and learning.

**Components**:
- **Exploration Drive**: Motivation to explore novel situations
- **Uncertainty Sensing**: Detect lack of knowledge
- **Information Gain**: Value of learning new information
- **Risk Assessment**: Balance exploration benefits vs. costs

**Key Features**:
- **Entropy-Based**: High entropy triggers exploration
- ** novelty-Driven**: Seek diverse experiences
- **Discovery Reward**: Intrinsic reward for learning
- **Adaptive Exploration**: Adjust exploration based on context

### 3. Novelty Detection System

**Purpose**: Identify and respond to new or unexpected stimuli.

**Components**:
- **Change Detection**: Compare current input to stored patterns
- **Surprise Signals**: Generate responses to unexpected events
- **Adaptation Tracking**: Monitor system adaptation progress
- **Memory Updating**: Trigger memory updates when needed

**Key Features**:
- **Pattern Matching**: Detect similarity to stored patterns
- **Prediction Errors**: Generate signals when predictions fail
- **Learning Triggers**: Initiate learning when novelty is detected
- **State Estimation**: Update internal models of environment

### 4. Attention System

**Purpose**: Select and focus neural processing on relevant information.

**Components**:
- **Selective Attention**: Enhance processing of important stimuli
- **Inhibition of Distractors**: Suppress irrelevant processing
- **Resource Allocation**: Distribute processing resources efficiently
- **Priority Setting**: Rank stimuli by importance

**Key Features**:
- **Competitive Dynamics**: Neural populations compete for attention
- **Modulatory Control**: Neuromodulators regulate attention
- **Temporal Attention**: Focus on different time scales
- **Spatial Attention**: Focus on different spatial locations

## Neuromodulation Configuration

### Core Neuromodulation Parameters

```python
neuromodulation_config = {
    "neuromod.dopamine": {
        "enable": True,
        "scale": 1.0,
        "decay_rate": 0.01,
        "learning_rate": 0.001,
        "prediction_error_gain": 1.0,
        "reward_prediction": True,
    },
    "neuromod.curiosity": {
        "enable": True,
        "threshold": 0.5,
        "decay": 0.02,
        "exploration_bonus": 0.1,
        "entropy_weight": 0.5,
        "information_gain": True,
    },
    "neuromod.novelty": {
        "enable": True,
        "sensitivity": 1.0,
        "adaptation_tracking": True,
        "change_detection": True,
        "surprise_threshold": 0.7,
    },
    "neuromod.attention": {
        "enable": True,
        "selection_strength": 0.8,
        "inhibition_strength": 0.6,
        "resource_allocation": "adaptive",
        "priority_algorithm": "softmax",
    }
}
```

## Working with Neuromodulation Systems

### 1. Basic Neuromodulation Setup

```python
import pynlm
import math

def basic_neuromodulation_example():
    """Demonstrate basic neuromodulation usage"""
    
    # Create brain with neuromodulation
    config = pynlm.createDefaultConfig()
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.attention.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Basic Neuromodulation Example")
    print("=============================")
    
    # Create agent to interface with brain
    agent = pynlm.createAgentBrain(brain)
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        
        # Get neuromodulation levels
        dopamine = brain.getNeuromodulationLevel("dopamine")
        curiosity = brain.getCuriosityLevel()
        novelty = brain.getNoveltyLevel()
        attention = brain.getAttentionLevel()
        
        # Apply reward modulation
        if step % 10 == 0:
            reward = 1.0 if step % 20 == 0 else 0.5  # Reward every 20 steps
            brain.applyRewardModulation(reward, 0.0)
        
        # Print neuromodulation state
        print(f"\nStep {step}:")
        print(f"  Dopamine: {dopamine:.3f}")
        print(f"  Curiosity: {curiosity:.3f}")
        print(f"  Novelty: {novelty:.3f}")
        print(f"  Attention: {attention:.3f}")
        
        # Check for high neuromodulation activity
        if dopamine > 0.7:
            print(f"  → Strong reward signal - learning moment!")
        
        if curiosity > 0.7:
            print(f"  → High curiosity - exploring new experiences")
        
        if novelty > 0.7:
            print(f"  → Novelty detected - pattern change")
        
        if attention > 0.7:
            print(f"  → High attention - focused processing")
    
    return brain

# Run example
brain = basic_neuromodulation_example()
```

### 2. Dopamine-Modulated Learning

```python
def dopamine_modulated_learning_example():
    """Demonstrate dopamine-modulated learning"""
    
    config = pynlm.createDefaultConfig()
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.dopamine.learning_rate", 0.005)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Dopamine-Modulated Learning Example")
    print("==================================")
    
    # Learning sequence with rewards
    learning_trials = [
        {"action": "approach", "expected_reward": 0.2, "actual_reward": 0.8},
        {"action": "avoid", "expected_reward": 0.6, "actual_reward": 0.3},
        {"action": "explore", "expected_reward": 0.1, "actual_reward": 0.9},
        {"action": "approach", "expected_reward": 0.4, "actual_reward": 0.2},
    ]
    
    for step in range(len(learning_trials) * 10):
        brain.step(step)
        
        # Select action based on learned values
        action = select_best_action(brain.getActionValues())
        
        # Get current trial
        trial_idx = step // 10
        if trial_idx < len(learning_trials):
            trial = learning_trials[trial_idx]
            expected_reward = trial["expected_reward"]
            actual_reward = trial["actual_reward"]
            
            # Calculate prediction error
            prediction_error = actual_reward - expected_reward
            
            # Apply reward modulation
            brain.applyRewardModulation(actual_reward, predicted_reward=expected_reward)
            
            print(f"\nTrial {trial_idx + 1}:")
            print(f"  Action: {action}")
            print(f"  Expected reward: {expected_reward:.2f}")
            print(f"  Actual reward: {actual_reward:.2f}")
            print(f"  Prediction error: {prediction_error:+.2f}")
            
            # Check dopamine response
            dopamine = brain.getNeuromodulationLevel("dopamine")
            print(f"  Dopamine level: {dopamine:.3f}")
            
            if prediction_error > 0:
                print(f"  → Positive prediction error - dopamine burst!")
            elif prediction_error < 0:
                print(f"  → Negative prediction error - dopamine dip!")
            else:
                print(f"  → No prediction error - baseline dopamine")
        
        # Update action values
        brain.updateActionValues(action, brain.getNeuromodulationLevel("dopamine"))
    
    return brain
```

### 3. Curiosity-Driven Exploration

```python
def curiosity_driven_exploration_example():
    """Demonstrate curiosity-driven exploration"""
    
    config = pynlm.createDefaultConfig()
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.4)
    config.set("neuromod.curiosity.entropy_weight", 0.8)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Curiosity-Driven Exploration Example")
    print("====================================")
    
    # Simulate exploration of different environments
    environments = [
        {"name": "simple", "entropy": 0.2, "novelty": 0.1},
        {"name": "moderate", "entropy": 0.5, "novelty": 0.4},
        {"name": "complex", "entropy": 0.8, "novelty": 0.7},
        {"name": "chaotic", "entropy": 1.0, "novelty": 0.9},
    ]
    
    for step in range(120):
        brain.step(step)
        
        # Calculate environment characteristics
        current_environment = step % len(environments)
        env = environments[current_environment]
        
        # Calculate curiosity level
        entropy = env["entropy"]
        novelty = env["novelty"]
        
        # Determine exploration behavior
        curiosity_threshold = 0.4
        exploration_bonus = 0.0
        
        if entropy > curiosity_threshold or novelty > curiosity_threshold:
            # High curiosity - exploration behavior
            exploration_bonus = 0.2 + (entropy + novelty) * 0.3
            action = "explore"
        else:
            # Low curiosity - exploitation behavior
            exploration_bonus = 0.0
            action = "exploit"
        
        # Apply curiosity bonus
        curiosity_bonus = brain.applyCuriosityModulation(entropy, novelty)
        
        print(f"\nStep {step}:")
        print(f"  Environment: {env['name']} (entropy: {entropy:.2f}, novelty: {novelty:.2f})")
        print(f"  Curiosity threshold: {curiosity_threshold:.2f}")
        print(f"  Action: {action}")
        print(f"  Exploration bonus: {exploration_bonus:.2f}")
        print(f"  Curiosity bonus: {curiosity_bonus:.2f}")
        
        # Track exploration patterns
        if step % 20 == 0:
            curiosity_level = brain.getCuriosityLevel()
            print(f"  Overall curiosity level: {curiosity_level:.2f}")
            
            # Check if curiosity is driving exploration
            if curiosity_level > 0.6:
                print(f"  → Strong curiosity - extensive exploration")
            elif curiosity_level > 0.3:
                print(f"  → Moderate curiosity - balanced exploration")
            else:
                print(f"  → Low curiosity - focused exploitation")
    
    return brain
```

### 4. Novelty Detection and Adaptation

```python
def novelty_detection_example():
    """Demonstrate novelty detection and adaptation"""
    
    config = pynlm.createDefaultConfig()
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 1.0)
    config.set("neuromod.novelty.surprise_threshold", 0.7)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Novelty Detection Example")
    print("==========================")
    
    # Store patterns for novelty detection
    stored_patterns = [
        [0.8, 0.2, 0.1],  # Pattern 1
        [0.3, 0.7, 0.2],  # Pattern 2
        [0.6, 0.1, 0.9],  # Pattern 3
    ]
    
    for step in range(100):
        brain.step(step)
        
        # Generate current input (with some noise)
        pattern_idx = step % len(stored_patterns)
        base_pattern = stored_patterns[pattern_idx]
        
        # Add noise to simulate real-world variability
        noise = np.random.normal(0, 0.1, len(base_pattern))
        current_input = np.clip(base_pattern + noise, 0.0, 1.0).tolist()
        
        # Detect novelty
        novelty_detected = False
        similarity_scores = []
        
        for stored_pattern in stored_patterns:
            similarity = calculate_pattern_similarity(current_input, stored_pattern)
            similarity_scores.append(similarity)
            
            if similarity < 0.7:  # Novelty threshold
                novelty_detected = True
        
        # Apply novelty processing
        if novelty_detected:
            surprise = max(1.0 - min(similarity_scores), 0.0)
            brain.applyNoveltyModulation(surprise, current_input)
            
            print(f"\nStep {step}: Novelty Detected!")
            print(f"  Current input: {[f'{x:.2f}' for x in current_input]}")
            print(f"  Similarities: {[f'{s:.2f}' for s in similarity_scores]}")
            print(f"  Surprise level: {surprise:.2f}")
            
            # Update stored patterns with new variations
            if surprise > 0.7:
                print(f"  → High surprise - updating pattern database")
                stored_patterns.append(current_input)
                if len(stored_patterns) > 10:
                    stored_patterns.pop(0)  # Keep only recent patterns
        else:
            print(f"\nStep {step}: Familiar input")
            print(f"  Input: {[f'{x:.2f}' for x in current_input]}")
            print(f"  Max similarity: {max(similarity_scores):.2f}")
        
        # Check novelty detection statistics
        if step % 15 == 0:
            novelty_level = brain.getNoveltyLevel()
            adaptation_progress = brain.getAdaptationProgress()
            print(f"  Novelty level: {novelty_level:.2f}")
            print(f"  Adaptation progress: {adaptation_progress:.2f}")
    
    return brain

def calculate_pattern_similarity(pattern1, pattern2):
    """Calculate similarity between two patterns"""
    if len(pattern1) != len(pattern2):
        return 0.0
    
    sum_sq = 0.0
    for a, b in zip(pattern1, pattern2):
        sum_sq += (a - b) ** 2
    
    return 1.0 - (sum_sq / len(pattern1)) ** 0.5
```

### 5. Attention Modulation

```python
def attention_modulation_example():
    """Demonstrate attention modulation usage"""
    
    config = pynlm.createDefaultConfig()
    config.set("neuromod.attention.enable", True)
    config.set("neuromod.attention.selection_strength", 0.8)
    config.set("neuromod.attention.inhibition_strength", 0.6)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Attention Modulation Example")
    print("============================")
    
    # Create multiple stimulus types for attention tasks
    stimuli = [
        {"type": "target", "value": 1.0, "position": "center"},
        {"type": "distractor", "value": 0.3, "position": "peripheral"},
        {"type": "neutral", "value": 0.5, "position": "ambiguous"},
    ]
    
    for step in range(80):
        brain.step(step)
        
        # Select task
        if step < 20:
            task = "simple_search"  # Find one target
        elif step < 40:
            task = "search_memory"  # Find item in memory
        elif step < 60:
            task = "sustained_attention"  # Maintain focus
        else:
            task = "divided_attention"  # Split attention
        
        # Process stimuli with attention
        selected_stimulus = select_attention_target(brain, stimuli)
        
        print(f"\nStep {step}: Task '{task}'")
        print(f"  Selected stimulus: {selected_stimulus['type']} (value: {selected_stimulus['value']:.2f})")
        
        # Get attention metrics
        selection_strength = brain.getSelectionStrength()
        inhibition_strength = brain.getInhibitionStrength()
        resource_allocation = brain.getResourceAllocation()
        
        print(f"  Selection strength: {selection_strength:.2f}")
        print(f"  Inhibition strength: {inhibition_strength:.2f}")
        print(f"  Resource allocation: {resource_allocation:.2f}")
        
        # Check attention quality
        if task == "simple_search" and selected_stimulus["type"] == "target":
            print(f"  ✓ Correct selection - high attention efficiency")
        elif task == "search_memory" and selected_stimulus["type"] in ["target", "neutral"]:
            print(f"  → Partial success - attention guided by memory")
        elif task == "sustained_attention":
            if selection_strength > 0.7:
                print(f"  ✓ Good sustained attention")
            else:
                print(f"  ⚠ Attention drift detected")
        elif task == "divided_attention":
            if resource_allocation > 0.6:
                print(f"  ✓ Effective divided attention")
            else:
                print(f"  ⚠ Attention splitting may be suboptimal")
        
        # Adapt attention based on performance
        adapt_attention_based_on_performance(brain, task, selected_stimulus)
    
    return brain

def select_attention_target(brain, stimuli):
    """Select target based on attention system"""
    # Simplified implementation - in reality would use neural computations
    if brain.getAttentionLevel() > 0.7:
        return max(stimuli, key=lambda x: x["value"])
    else:
        return min(stimuli, key=lambda x: x["value"])

def adapt_attention_based_on_performance(brain, task, selected_stimulus):
    """Adapt attention based on performance feedback"""
    # Simplified adaptation logic
    if task == "simple_search" and selected_stimulus["type"] != "target":
        # Need to improve selection
        brain.adjustSelectionStrength(0.1)
    elif task == "sustained_attention":
        # Need to maintain focus
        brain.adjustResourceAllocation(0.05)
    elif task == "divided_attention":
        # Need to balance allocation
        brain.adjustInhibitionStrength(0.05)
```

## Neuromodulation Integration with Other Systems

### 1. Neuromodulation-Memory Integration

```python
def neuromodulation_memory_integration_example():
    """Demonstrate neuromodulation-memory integration"""
    
    config = pynlm.createDefaultConfig()
    config.set("memory.working.enable", True)
    config.set("memory.episodic.enable", True)
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Neuromodulation-Memory Integration Example")
    print("==========================================")
    
    # Learning task with memory
    task_items = [
        "item_a",  # Memory pattern 1
        "item_b",  # Memory pattern 2
        "item_c",  # Memory pattern 3
    ]
    
    for step in range(90):
        brain.step(step)
        
        # Task: find and remember item
        if step < len(task_items):
            target_item = task_items[step]
            target_pattern = get_item_pattern(target_item)
            
            # Get neuromodulation state
            dopamine = brain.getNeuromodulationLevel("dopamine")
            curiosity = brain.getCuriosityLevel()
            
            # Store item in working memory
            if dopamine > curiosity:  # Reward-driven learning
                brain.storeInWorkingMemory(target_pattern, step)
                print(f"\nStep {step}: Reward-driven storage of {target_item}")
            else:  # Curiosity-driven learning
                brain.storeInWorkingMemory(target_pattern, step * 0.5)  # Lower priority
                print(f"\nStep {step}: Curiosity-driven storage of {target_item}")
            
            # Check memory system state
            working_memory_load = brain.getWorkingMemoryLoad()
            episodic_memory_stats = brain.getEpisodicMemoryStats()
            
            print(f"  Working memory load: {working_memory_load:.2f}")
            print(f"  Episodic memory episodes: {episodic_memory_stats['episodes_stored']}")
        
        # Test memory retrieval
        if step % 15 == 0 and step > 0:
            # Try to retrieve stored information
            retrieval_success = brain.testMemoryRetrieval(step - 10)
            
            print(f"\nStep {step}: Memory retrieval test")
            print(f"  Retrieval success: {retrieval_success:.2f}")
            
            # Check if neuromodulation helped
            dopamine_effect = brain.getDopamineEffectOnMemory()
            curiosity_effect = brain.getCuriosityEffectOnMemory()
            
            print(f"  Dopamine effect on memory: {dopamine_effect:.2f}")
            print(f"  Curiosity effect on memory: {curiosity_effect:.2f}")
        
        # Update development based on memory-neuromodulation interaction
        brain.updateDevelopment(0.1)
    
    return brain
```

### 2. Neuromodulation-Prediction Integration

```python
def neuromodulation_prediction_integration_example():
    """Demonstrate neuromodulation-prediction integration"""
    
    config = pynlm.createDefaultConfig()
    config.set("prediction.enable", True)
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Neuromodulation-Prediction Integration Example")
    print("==============================================")
    
    # Prediction task with neuromodulation
    for step in range(100):
        brain.step(step)
        
        # Current state for prediction
        current_state = [0.5 + 0.5 * math.sin(step/10), 0.5 + 0.5 * math.cos(step/10)]
        
        # Make predictions
        predictions = brain.predictNextEvents(current_state)
        
        # Get neuromodulation levels
        dopamine = brain.getNeuromodulationLevel("dopamine")
        curiosity = brain.getCuriosityLevel()
        
        # Neuromodulation affects prediction confidence
        if dopamine > 0.7:
            # High dopamine - increase prediction confidence
            prediction_confidence = min(brain.getPredictionConfidence() * 1.2, 1.0)
            print(f"\nStep {step}: High dopamine ({dopamine:.2f}) - boosting prediction confidence")
        elif curiosity > 0.7:
            # High curiosity - exploration predictions
            prediction_confidence = max(brain.getPredictionConfidence() * 0.8, 0.3)
            print(f"\nStep {step}: High curiosity ({curiosity:.2f}) - exploration predictions")
        else:
            # Normal levels - regular predictions
            prediction_confidence = brain.getPredictionConfidence()
            print(f"\nStep {step}: Normal neuromodulation levels")
        
        print(f"  Current state: {[f'{x:.2f}' for x in current_state]}")
        print(f"  Predictions: {[f'{p:.2f}' for p in predictions[:3]]}")
        print(f"  Prediction confidence: {prediction_confidence:.2f}")
        
        # Check prediction error and neuromodulation interaction
        prediction_error = brain.getPredictionError()
        neuromodulation_effect = brain.getNeuromodulationEffectOnPrediction()
        
        print(f"  Prediction error: {prediction_error:.3f}")
        print(f"  Neuromodulation effect: {neuromodulation_effect:.2f}")
        
        # Apply prediction error learning
        brain.applyPredictionErrorLearning(prediction_error)
    
    return brain
```

## Neuromodulation Performance Analysis

### 1. Neuromodulation Effectiveness Metrics

```python
def analyze_neuromodulation_effectiveness():
    """Analyze neuromodulation system performance metrics"""
    
    config = pynlm.createDefaultConfig()
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.attention.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Neuromodulation Effectiveness Analysis")
    print("====================================")
    
    # Track effectiveness over time
    effectiveness_data = {
        "dopamine_effectiveness": [],
        "curiosity_effectiveness": [],
        "novelty_effectiveness": [],
        "attention_effectiveness": [],
        "overall_effectiveness": [],
    }
    
    for step in range(150):
        brain.step(step)
        
        # Calculate effectiveness metrics
        dopamine_effectiveness = brain.getDopamineEffectiveness()
        curiosity_effectiveness = brain.getCuriosityEffectiveness()
        novelty_effectiveness = brain.getNoveltyEffectiveness()
        attention_effectiveness = brain.getAttentionEffectiveness()
        
        overall_effectiveness = (
            dopamine_effectiveness * 0.3 +
            curiosity_effectiveness * 0.3 +
            novelty_effectiveness * 0.2 +
            attention_effectiveness * 0.2
        )
        
        effectiveness_data["dopamine_effectiveness"].append(dopamine_effectiveness)
        effectiveness_data["curiosity_effectiveness"].append(curiosity_effectiveness)
        effectiveness_data["novelty_effectiveness"].append(novelty_effectiveness)
        effectiveness_data["attention_effectiveness"].append(attention_effectiveness)
        effectiveness_data["overall_effectiveness"].append(overall_effectiveness)
        
        if step % 20 == 0:
            print(f"\nStep {step}:")
            print(f"  Dopamine effectiveness: {dopamine_effectiveness:.2f}")
            print(f"  Curiosity effectiveness: {curiosity_effectiveness:.2f}")
            print(f"  Novelty effectiveness: {novelty_effectiveness:.2f}")
            print(f"  Attention effectiveness: {attention_effectiveness:.2f}")
            print(f"  Overall effectiveness: {overall_effectiveness:.2f}")
            
            # Analyze individual system performance
            if dopamine_effectiveness > 0.7:
                print(f"  ✓ Dopamine system performing well")
            elif dopamine_effectiveness > 0.4:
                print(f"  → Dopamine system moderate performance")
            else:
                print(f"  ⚠ Dopamine system needs improvement")
            
            if curiosity_effectiveness > 0.7:
                print(f"  ✓ Curiosity system performing well")
            elif curiosity_effectiveness > 0.4:
                print(f"  → Curiosity system moderate performance")
            else:
                print(f"  ⚠ Curiosity system needs improvement")
    
    # Analyze effectiveness trends
    analyze_neuromodulation_trends(effectiveness_data)
    
    return brain, effectiveness_data
```

### 2. Neuromodulation Optimization

```python
def optimize_neuromodulation_performance(brain, target_effectiveness=0.7):
    """Optimize neuromodulation system performance"""
    
    print("Neuromodulation Performance Optimization")
    print("=========================================")
    
    optimizations = []
    
    # Check dopamine effectiveness
    dopamine_effectiveness = brain.getDopamineEffectiveness()
    if dopamine_effectiveness < target_effectiveness:
        # Adjust dopamine parameters
        brain.setDopamineScale(brain.getDopamineScale() * 1.1)
        optimizations.append("Increased dopamine scale")
    
    # Check curiosity effectiveness
    curiosity_effectiveness = brain.getCuriosityEffectiveness()
    if curiosity_effectiveness < target_effectiveness:
        # Adjust curiosity parameters
        brain.setCuriosityThreshold(brain.getCuriosityThreshold() * 0.9)
        optimizations.append("Adjusted curiosity threshold")
    
    # Check novelty effectiveness
    novelty_effectiveness = brain.getNoveltyEffectiveness()
    if novelty_effectiveness < target_effectiveness:
        # Adjust novelty parameters
        brain.setNoveltySensitivity(brain.getNoveltySensitivity() * 1.1)
        optimizations.append("Increased novelty sensitivity")
    
    # Check attention effectiveness
    attention_effectiveness = brain.getAttentionEffectiveness()
    if attention_effectiveness < target_effectiveness:
        # Adjust attention parameters
        brain.setSelectionStrength(brain.getSelectionStrength() * 1.05)
        optimizations.append("Adjusted attention selection strength")
    
    # Balance neuromodulation levels
    balance_metrics = brain.calculateNeuromodulationBalance()
    if balance_metrics["imbalance"] > 0.3:
        # Redistribute neuromodulation resources
        brain.redistributeNeuromodulationResources()
        optimizations.append("Rebalanced neuromodulation resources")
    
    if optimizations:
        print("Applied optimizations:")
        for opt in optimizations:
            print(f"  ✓ {opt}")
    else:
        print("Neuromodulation performance optimal")
    
    return optimizations
```

## Neuromodulation Best Practices

### 1. Configuration Best Practices

1. **Start with Basic Neuromodulators**: Begin with dopamine and curiosity
2. **Validate Neuromodulation Balance**: Ensure no single system dominates
3. **Monitor Neuromodulation Effectiveness**: Track system performance
4. **Test Neuromodulation Interactions**: Check cross-system effects
5. **Document Neuromodulation Settings**: Record configuration choices

### 2. Usage Best Practices

1. **Use Neuromodulators for Learning**: Leverage neuromodulation for memory and prediction
2. **Monitor Neuromodulation Levels**: Track neuromodulator concentrations
3. **Adapt Neuromodulators**: Adjust parameters based on performance
4. **Test Neuromodulation Robustness**: Verify robustness under various conditions
5. **Integrate Neuromodulators**: Combine neuromodulation with other brain systems

### 3. Experimental Best Practices

1. **Control Neuromodulation Variables**: Isolate neuromodulation system effects
2. **Measure Neuromodulation Metrics**: Track all neuromodulation indicators
3. **Test Neuromodulation Adaptation**: Verify adaptation capabilities
4. **Document Neuromodulation Procedures**: Record neuromodulation system usage
5. **Share Neuromodulation Configurations**: Contribute to community knowledge

### 4. Troubleshooting Neuromodulation Issues

**Problem**: Neuromodulation effectiveness too low
**Solutions**:
1. Increase neuromodulator scales
2. Adjust threshold parameters
3. Enable more neuromodulators
4. Check for neuromodulation system conflicts
5. Optimize neuromodulation interactions

**Problem**: Neuromodulation levels too high
**Solutions**:
1. Reduce neuromodulator scales
2. Increase threshold parameters
3. Disable some neuromodulators
4. Implement neuromodulation decay
5. Balance neuromodulation resources

## Neuromodulation Case Studies

### Case Study 1: Dopamine-Driven Learning

**Research Question**: How does dopamine affect learning in NLM?

**Experiment Design**:
- Manipulate dopamine levels
- Measure learning speed and accuracy
- Track prediction error signaling

**Key Findings**:
- Dopamine strongly affects learning rate
- Positive prediction errors enhance learning
- Negative prediction errors suppress learning
- Optimal dopamine levels balance exploration and exploitation

### Case Study 2: Curiosity-Driven Exploration

**Research Question**: How does curiosity affect behavior in NLM?

**Experiment Design**:
- Manipulate curiosity thresholds
- Measure exploration behavior
- Track novelty detection effectiveness

**Key Findings**:
- Curiosity promotes exploration of novel environments
- High curiosity improves learning in complex environments
- Low curiosity leads to efficient exploitation
- Optimal curiosity balances exploration and exploitation

### Case Study 3: Novelty Detection Effectiveness

**Research Question**: How well does novelty detection work in NLM?

**Experiment Design**:
- Test novelty detection with various patterns
- Measure detection accuracy and false positives
- Track adaptation to novel stimuli

**Key Findings**:
- Novelty detection improves with experience
- High novelty sensitivity causes over-reaction
- Low novelty sensitivity reduces adaptability
- Optimal sensitivity balances sensitivity and specificity

### Case Study 4: Attention Neuromodulation

**Research Question**: How does neuromodulation affect attention?

**Experiment Design**:
- Manipulate attention neuromodulation levels
- Measure attentional selection and focus
- Track resource allocation efficiency

**Key Findings**:
- Neuromodulation strongly affects attentional selection
- Balanced neuromodulation improves attention quality
- High neuromodulation levels can cause attention overload
- Individual neuromodulators have specific attention effects

## Conclusion

The neuromodulation system is essential for NLM's intelligent behavior. Understanding how to:

1. **Configure Neuromodulation Systems**: Set appropriate parameters for research needs
2. **Use Neuromodulation Effectively**: Leverage neuromodulation for better learning and behavior
3. **Monitor Neuromodulation Performance**: Track neuromodulation effectiveness
4. **Optimize Neuromodulation**: Balance neuromodulation levels and effectiveness
5. **Troubleshoot Neuromodulation Issues**: Identify and fix neuromodulation system problems

The key is to:

- **Start Simple**: Begin with basic neuromodulators (dopamine, curiosity)
- **Understand Neuromodulation**: Know what each neuromodulator does
- **Measure Neuromodulation**: Track all neuromodulation-related metrics
- **Test Neuromodulation**: Verify neuromodulation robustness under various conditions
- **Document Neuromodulation**: Record neuromodulation system usage and results

Neuromodulation systems enable NLM to:
- **Learn from Rewards**: Use dopamine for reinforcement learning
- **Explore Actively**: Use curiosity for intrinsic motivation
- **Adapt to Novelty**: Use novelty detection for adaptation
- **Focus Attention**: Use attention modulation for selective processing
- **Balance Behavior**: Use multiple neuromodulators for optimal behavior

Your contributions to neuromodulation research help advance our understanding of how biological neuromodulators can support intelligent behavior in artificial systems!

---

*Neuromodulation Research Resources*:
- `configuration.md` - Neuromodulation configuration options
- `experimental_design.md` - Neuromodulation experiment design
- `examples/` - Neuromodulation usage examples
- Neuromodulation system documentation
- Performance optimization tools

*Next Neuromodulation Steps*:
1. **Read configuration.md**: Learn neuromodulation configuration options
2. **Run examples/**: Try neuromodulation usage examples
3. **Design experiments**: Create neuromodulation-specific research
4. **Optimize neuromodulation**: Balance neuromodulation levels and effectiveness
5. **Share findings**: Contribute neuromodulation research to the community

*Ready to start your neuromodulation research?*

1. **Begin with dopamine**: Start with reward-based learning
2. **Add curiosity**: Include intrinsic motivation
3. **Enable novelty detection**: Add adaptation capabilities
4. **Integrate attention**: Include selective processing
5. **Test neuromodulation interactions**: Verify cross-system effects

The neuromodulation systems in NLM are powerful tools for creating intelligent, motivated behavior. Your research will help advance our understanding of how biological neuromodulators support intelligent behavior!

---

*Neuromodulation systems enable NLM to:
• Learn from rewards (dopamine)
• Explore actively (curiosity)
• Adapt to novelty (novelty detection)
• Focus attention (attention modulation)
• Balance behavior (multiple neuromodulators)

Your neuromodulation research contributions will help advance neural system research!

---
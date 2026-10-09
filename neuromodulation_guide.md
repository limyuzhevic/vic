# neuromodulation_guide.md

# NLM Neuromodulation Guide

## Overview

Neuromodulation is a core component of NLM that enables adaptive learning and behavior through chemical signals that modulate neural activity and plasticity. NLM implements 8 specialized neuromodulators that work together to control different aspects of learning and behavior.

## Neuromodulation Architecture

### Neuromodulator Base Class (src/neuromodulation/Neuromodulator.hpp)

All neuromodulators in NLM inherit from the abstract `Neuromodulator` base class:

```cpp
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Core interface
    virtual std::string getName() const = 0;
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    virtual float getPlasticityFactor() const = 0;
    
    // Update mechanism
    virtual void update(float dt) = 0;
};
```

### Neuromodulator Integration

Neuromodulators are integrated into the brain loop in STEP 5 of `Brain::step()`:

```cpp
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // ... previous steps ...
    
    // STEP 5: Apply neuromodulation effects
    // - Dopamine affects neural excitability and plasticity
    // - Novelty detection updates
    // - Curiosity computation
    
    // Apply neuromodulator effects to neural dynamics
    for (auto& neuromodulator : pImpl->neuromodulators) {
        neuromodulator->update(pImpl->timestep);
        neuromodulator->applyToNeuralDynamics(pImpl->neurons);
        neuromodulator->applyToPlasticity(pImpl->plasticitySystem);
    }
    
    // ... subsequent steps ...
}
```

## The 8 Neuromodulators

NLM implements a comprehensive set of neuromodulators:

### 1. Dopamine (src/neuromodulation/Dopamine.hpp)

**Primary Functions**:
- Reward prediction error signaling
- Reinforcement learning
- Behavioral vigor modulation
- Action selection facilitation

**Mechanisms**:
- **Phasic Signaling**: Burst-like signals for unexpected rewards
- **Tonic Signaling**: Baseline levels influencing overall behavior
- **Neural Excitability**: Modifies firing thresholds
- **Plasticity Scaling**: Scales STDP learning rates

**Behavioral Effects**:
- Increased motivation for rewarded actions
- Enhanced learning from prediction errors
- Improved action selection confidence
- Behavioral persistence

**API Usage**:
```python
import pynlm

# Create brain and neuromodulator
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

dopamine = pynlm.Dopamine()
dopamine.initialize(brain)

# Signal reward
dopamine.signalReward(1.0)  # Positive reward
dopamine.signalReward(0.0)  # No reward
dopamine.signalReward(-1.0) # Negative reward

# Signal prediction error
dopamine.signalRewardPredictionError(0.2)  # Positive error

# Get current level
level = dopamine.getLevel()
```

### 2. Serotonin (src/neuromodulation/Serotonin.hpp)

**Primary Functions**:
- Mood regulation
- Learning modulation
- Sleep/wake cycle control
- Attention and arousal
- Avoidance learning

**Mechanisms**:
- **Learning Rate Modulation**: Increases/decreases overall learning speed
- **Attention Control**: Enhances signal-to-noise ratio
- **Behavioral Inhibition**: Reduces approach behaviors
- **Sleep Orchestration**: Regulates sleep for memory consolidation

**Behavioral Effects**:
- Exploration vs. exploitation balance
- Fear learning and avoidance
- Cognitive flexibility
- Stress response

**API Usage**:
```python
import pynlm

# Create brain and neuromodulator
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize serotonin
serotonin = pynlm.Serotonin()
serotonin.initialize(brain)

# Signal mood states
serotonin.signalMood(0.8)  # Positive mood
serotonin.signalMood(-0.5) # Negative mood

# Signal learning needs
serotonin.signalLearning(0.3)  # Need to learn more

# Signal avoidance learning
serotonin.signalAvoidance(0.9)  # Strong avoidance

# Get attention and avoidance effects
attention_effect = serotonin.getAttentionEffect()
avoidance_effect = serotonin.getAvoidanceLearningEffect()
```

### 3. Norepinephrine (src/neuromodulation/Norepinephrine.hpp)

**Primary Functions**:
- Arousal and alertness
- Attention enhancement
- Working memory modulation
- Cognitive control
- Response preparation

**Mechanisms**:
- **Arousal Control**: Increases overall neural excitability
- **Attention Focus**: Enhances relevant signal processing
- **Working Memory**: Maintains information in active circuits
- **Cognitive Flexibility**: Adapts to changing task demands

**Behavioral Effects**:
- Improved attention and focus
- Enhanced working memory capacity
- Faster response times
- Better task switching

**API Usage**:
```python
import pynlm

# Create brain and neuromodulator
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize norepinephrine
nor = pynlm.Norepinephrine()
nor.initialize(brain)

# Signal arousal levels
nor.signalArousal(0.7)  # Medium arousal

# Signal attention needs
nor.signalAttention(0.9)  # High attention needed

# Signal working memory updates
nor.signalWorkingMemory(0.5)  # Working memory update

# Get attention and working memory effects
attention_effect = nor.getAttentionEffect()
wm_effect = nor.getWorkingMemoryEffect()
```

### 4. Acetylcholine (src/neuromodulation/Acetylcholine.hpp)

**Primary Functions**:
- Attention modulation
- Memory consolidation
- Sleep/wake regulation
- Cortical activation
- Sensory processing enhancement

**Mechanisms**:
- **Attention Selectivity**: Enhances signal detection
- **Memory Consolidation**: Promotes hippocampal-neocortical dialogue
- **Sleep States**: Regulates REM and non-REM sleep
- **Cortical Processing**: Enhances sensory information processing

**Behavioral Effects**:
- Improved learning and memory
- Enhanced perceptual processing
- Better sleep quality
- Cognitive flexibility

**API Usage**:
```python
import pynlm

# Create brain and neuromodulator
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize acetylcholine
ach = pynlm.Acetylcholine()
ach.initialize(brain)

# Signal attention
signalAttention(0.8)  # Attention needed

# Signal memory consolidation
signalMemory(0.6)  # Memory consolidation needed

# Signal sleep states
signalSleep(0.3)  # Sleep needed

# Get effects
attention_effect = ach.getAttentionEffect()
consolidation_effect = ach.getMemoryConsolidationEffect()
```

### 5. Curiosity (src/neuromodulation/Curiosity.hpp)

**Primary Functions**:
- Exploration motivation
- Novelty detection
- Prediction error integration
- Behavioral flexibility
- Intrinsic motivation

**Mechanisms**:
- **Novelty Integration**: Combines novelty with prediction error
- **Exploration Drive**: Calculates motivation to explore
- **Learning Signal**: Drives exploration for new information
- **Decay Regulation**: Controls exploration duration

**Behavioral Effects**:
- Increased exploration of novel environments
- Discovery of new resources and challenges
- Adaptive behavior switching
- Curiosity-driven learning

**API Usage**:
```python
import pynlm

# Create brain and neuromodulator
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize curiosity
curiosity = pynlm.Curiosity()
curiosity.initialize(brain)

# Configure curiosity parameters
curiosity.setNoveltyWeight(0.7)  # Weight for novelty
curiosity.setPredictionErrorWeight(0.3)  # Weight for prediction error

# Update curiosity based on environmental input
curiosity.update(novelty_level=0.5, prediction_error=0.2, dt=0.1)

# Get curiosity level
level = curiosity.getLevel()
exploration_drive = curiosity.getExplorationDrive()
```

### 6. Novelty (src/neuromodulation/Novelty.hpp)

**Primary Functions**:
- Detection of novel sensory patterns
- Change detection
- Learning signal generation
- Behavioral adaptation

**Mechanisms**:
- **Pattern Comparison**: Compares current input with memory
- **Difference Detection**: Computes deviation from expected
- **History Tracking**: Maintains recent experience baseline
- **Decay Mechanism**: Reduces novelty signals over time

**Behavioral Effects**:
- Detection of new environments
- Recognition of unexpected changes
- Adaptive response to novel situations
- Information seeking behavior

**API Usage**:
```python
import pynlm

# Create brain and neuromodulator
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize novelty
novelty = pynlm.Novelty()
novelty.initialize(brain)

# Set novelty level
novelty.setLevel(0.6)

# Detect novelty from sensory input
current_pattern = [0.1, 0.5, 0.9, 0.3, 0.7, 0.2, 0.8, 0.4]
previous_pattern = [0.2, 0.6, 0.8, 0.4, 0.1, 0.3, 0.5, 0.7]

is_novel = novelty.detectNovelty(current_pattern, previous_pattern)
novelty_level = novelty.getLevel()

# Update novelty (decay)
novelty.update(dt=0.1)

# Access novelty history
history = novelty.getHistory()
```

### 7. PredictionError (src/neuromodulation/PredictionError.hpp)

**Primary Functions**:
- Prediction error computation
- Learning signal generation
- Behavioral adaptation
- Error correction

**Mechanisms**:
- **Error Computation**: Measures difference between predicted and actual
- **Confidence Adjustment**: Reduces confidence in incorrect predictions
- **Learning Modulation**: Drives synaptic changes
- **Attention Focus**: Increases attention to unexpected events

**Behavioral Effects**:
- Error-based learning
- Behavioral adaptation
- Pattern refinement
- Improved prediction accuracy

**API Usage**:
```python
import pynlm

# Create brain and neuromodulator
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize prediction error
pred_error = pynlm.PredictionError()
pred_error.initialize(brain)

# Configure surprise threshold
pred_error.setSurpriseThreshold(0.7)

# Compute prediction error
predicted = [0.1, 0.2, 0.3, 0.4]
actual = [0.15, 0.25, 0.35, 0.45]
error = pred_error.computeError(predicted, actual)
components = pred_error.getErrorComponents()

# Get error magnitude for neuromodulation
magnitude = pred_error.getMagnitude()

# Record error for learning
pred_error.recordError(error, step=0)

# Check if event was surprising
is_surprising = pred_error.isSurprising()
```

### 8. Reward (src/neuromodulation/Reward.hpp)

**Primary Functions**:
- Reward signaling
- Motivation
- Behavioral reinforcement
- Approach learning

**Mechanisms**:
- **Reward Prediction**: Signals expected outcomes
- **Motivation Modulation**: Drives goal-directed behavior
- **Synaptic Modulation**: Enhances successful pathways
- **Temporal Integration**: Considers timing of rewards

**Behavioral Effects**:
- Reinforcement learning
- Approach behavior
- Reward seeking
- Goal persistence

**API Usage**:
```python
import pynlm

# Create brain and neuromodulator
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize reward
reward = pynlm.Reward()
reward.initialize(brain)

# Signal rewards
reward.signalReward(1.0)  # Strong positive reward
reward.signalReward(0.5)  # Moderate positive reward
reward.signalReward(0.0)  # No reward

# Signal prediction errors on rewards
reward.signalPredictionError(0.3)  # Reward prediction error

# Get current reward level
level = reward.getLevel()

# Update reward (decay over time)
reward.update(dt=0.1)
```

## Neuromodulator Integration and Interaction

### Common Patterns

#### 1. Reward + Dopamine + Curiosity
```python
# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Initialize neuromodulators
reward = pynlm.Reward()
reward.initialize(brain)

dopamine = pynlm.Dopamine()
dopamine.initialize(brain)

curiosity = pynlm.Curiosity()
curiosity.initialize(brain)

# Configure integration
reward.signalReward(1.0)  # Primary reward

# Dopamine responds to reward prediction error
dopamine.signalRewardPredictionError(0.2)

# Curiosity combines novelty and prediction error
novelty = pynlm.Novelty()
novelty.initialize(brain)
curiosity.update(novelty.getLevel(), 0.2, 0.1)
```

#### 2. Attention + Acetylcholine + Norepinephrine
```python
# Initialize attention-related neuromodulators
ach = pynlm.Acetylcholine()
ach.initialize(brain)

nor = pynlm.Norepinephrine()
nor.initialize(brain)

# Coordinate attentional signals
ach.signalAttention(0.8)  # Top-down attention
nor.signalAttention(0.9)  # Bottom-up arousal

# Combined attentional enhancement
attention_effect = ach.getAttentionEffect() * 0.6 + nor.getAttentionEffect() * 0.4
```

#### 3. Novelty + Curiosity + Exploration
```python
# Initialize exploration-related neuromodulators
novelty = pynlm.Novelty()
novelty.initialize(brain)

curiosity = pynlm.Curiosity()
curiosity.initialize(brain)

# Create exploration behavior
if curiosity.getLevel() > 0.5:
    # High curiosity -> exploration behavior
    print("Exploring new areas...")
    # Implementation would go here

# Novelty detection triggers curiosity update
novelty.detectNovelty(current_sensory_input, previous_input)
curiosity.update(novelty.getLevel(), prediction_error, dt)
```

### Neuromodulator Interaction Graph

The neuromodulator system forms a complex interaction network:

```
Novelty → Curiosity ← PredictionError
    ↓           ↓
Reward ← Dopamine ← PredictionError
    ↓           ↓
Serotonin → Norepinephrine ← Acetylcholine
    ↓
Attention → Working Memory ← Learning
```

**Key Interactions**:

1. **Dopamine + Reward**: Reinforcement learning signal
2. **Novelty + Curiosity**: Exploration motivation
3. **PredictionError + Dopamine**: Error-based learning
4. **Acetylcholine + Attention**: Attentional enhancement
5. **Norepinephrine + Working Memory**: Memory maintenance
6. **Serotonin + Avoidance**: Behavioral inhibition

## Neuromodulator Configuration

### Configuration Options

| Neuromodulator | Config Path | Type | Default | Description |
|----------------|-------------|------|---------|-------------|
| Dopamine | `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation strength |
| Serotonin | `neuromod.serotonin.scale` | float | 1.0 | Serotonin modulation strength |
| Norepinephrine | `neuromod.norepinephrine.scale` | float | 1.0 | NE modulation strength |
| Acetylcholine | `neuromod.acetylcholine.scale` | float | 1.0 | ACh modulation strength |
| Curiosity | `neuromod.curiosity.enable` | bool | true | Enable curiosity system |
| Novelty | `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| PredictionError | `neuromod.prediction_error.threshold` | float | 0.5 | Surprise threshold |
| Reward | `neuromod.reward.scale` | float | 1.0 | Reward modulation strength |

### Configuration Examples

#### Balanced Configuration
```cpp
config->set("neuromod.dopamine.scale", 1.0f);
config->set("neuromod.serotonin.scale", 1.0f);
config->set("neuromod.norepinephrine.scale", 1.0f);
config->set("neuromod.acetylcholine.scale", 1.0f);
config->set("neuromod.curiosity.enable", true);
config->set("neuromod.novelty.enable", true);
config->set("neuromod.prediction_error.threshold", 0.5f);
config->set("neuromod.reward.scale", 1.0f);
```

#### Exploration-Focused Configuration
```cpp
config->set("neuromod.dopamine.scale", 0.5f);
config->set("neuromod.serotonin.scale", 0.8f);
config->set("neuromod.norepinephrine.scale", 1.2f);
config->set("neuromod.acetylcholine.scale", 1.2f);
config->set("neuromod.curiosity.enable", true);
config->set("neuromod.novelty.enable", true);
config->set("neuromod.prediction_error.threshold", 0.3f);
config->set("neuromod.reward.scale", 0.5f);
```

#### Learning-Focused Configuration
```cpp
config->set("neuromod.dopamine.scale", 1.5f);
config->set("neuromod.serotonin.scale", 1.0f);
config->set("neuromod.norepinephrine.scale", 0.8f);
config->set("neuromod.acetylcholine.scale", 1.0f);
config->set("neuromod.curiosity.enable", true);
config->set("neuromod.novelty.enable", true);
config->set("neuromod.prediction_error.threshold", 0.7f);
config->set("neuromod.reward.scale", 1.0f);
```

## Neuromodulator Testing

### Individual Neuromodulator Tests

```python
import pynlm

def test_dopamine():
    """Test dopamine neuromodulator."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    dopamine = pynlm.Dopamine()
    dopamine.initialize(brain)
    
    # Test reward signaling
    dopamine.signalReward(1.0)
    assert dopamine.getLevel() > 0.5
    
    # Test prediction error signaling
    dopamine.signalRewardPredictionError(0.2)
    
    # Test reset
    dopamine.reset()
    assert dopamine.getLevel() == 0.0
    
    print("Dopamine test passed")

def test_curiosity():
    """Test curiosity neuromodulator."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    curiosity = pynlm.Curiosity()
    curiosity.initialize(brain)
    
    # Configure curiosity
    curiosity.setNoveltyWeight(0.7)
    curiosity.setPredictionErrorWeight(0.3)
    
    # Test curiosity update
    curiosity.update(novelty_level=0.5, prediction_error=0.2, dt=0.1)
    assert curiosity.getLevel() > 0.0
    
    # Test reset
    curiosity.reset()
    assert curiosity.getLevel() == 0.0
    
    print("Curiosity test passed")

def test_novelty():
    """Test novelty neuromodulator."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    novelty = pynlm.Novelty()
    novelty.initialize(brain)
    
    # Test novelty detection
    current = [0.1, 0.2, 0.3, 0.4]
    previous = [0.2, 0.3, 0.4, 0.5]
    
    is_novel = novelty.detectNovelty(current, previous)
    assert is_novel == True  # Patterns are different
    
    # Test update (decay)
    novelty.update(dt=0.1)
    
    # Test setting level
    novelty.setLevel(0.8)
    assert novelty.getLevel() == 0.8
    
    print("Novelty test passed")

# Run all tests
test_dopamine()
test_curiosity()
test_novelty()
```

### Neuromodulator Integration Tests

```python
import pynlm

def test_neuromodulator_integration():
    """Test integration of multiple neuromodulators."""
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize all neuromodulators
    neuromodulators = {
        'dopamine': pynlm.Dopamine(),
        'serotonin': pynlm.Serotonin(),
        'norepinephrine': pynlm.Norepinephrine(),
        'acetylcholine': pynlm.Acetylcholine(),
        'curiosity': pynlm.Curiosity(),
        'novelty': pynlm.Novelty(),
        'prediction_error': pynlm.PredictionError(),
        'reward': pynlm.Reward(),
    }
    
    # Initialize all with brain reference
    for neuromod in neuromodulators.values():
        neuromod.initialize(brain)
    
    # Test coordinated signaling
    neuromodulators['reward'].signalReward(1.0)
    neuromodulators['dopamine'].signalRewardPredictionError(0.2)
    
    neuromodulators['novelty'].detectNovelty([0.1, 0.2, 0.3], [0.2, 0.3, 0.4])
    neuromodulators['curiosity'].update(neuromodulators['novelty'].getLevel(), 0.2, 0.1)
    
    # Test attention system integration
    neuromodulators['acetylcholine'].signalAttention(0.8)
    neuromodulators['norepinephrine'].signalAttention(0.9)
    neuromodulators['serotonin'].signalMood(0.5)
    
    # Test prediction error system integration
    pred_error = neuromodulators['prediction_error']
    pred_error.computeError([0.1, 0.2, 0.3], [0.15, 0.25, 0.35])
    
    # Verify all neuromodulators are functioning
    levels = {}
    for name, neuromod in neuromodulators.items():
        levels[name] = neuromod.getLevel()
    
    print("Neuromodulator integration test:")
    for name, level in levels.items():
        print(f"  {name}: {level:.3f}")
    
    return True

test_neuromodulator_integration()
```

## Neuromodulator Data Analysis

### Neuromodulator Activity Analysis

```python
import pynlm
import matplotlib.pyplot as plt
import numpy as np

def analyze_neuromodulator_activity(neuromod_log):
    """Analyze neuromodulator activity patterns."""
    
    # Extract neuromodulator time series
    neuromodulators = list(neuromod_log[0].keys())
    neuromodulators = [n for n in neuromodulators if n != 'step']
    
    # Create plots
    plt.figure(figsize=(15, 10))
    
    # Plot each neuromodulator
    for i, neuromod in enumerate(neuromodulators):
        levels = [entry[neuromod] for entry in neuromod_log]
        
        plt.subplot(3, 3, i + 1)
        plt.plot(levels, label=neuromod)
        plt.title(f"{neuromod} Level")
        plt.ylabel("Level (0-1)")
        plt.xlabel("Time Step")
        plt.grid(True)
        plt.legend()
    
    # Correlation analysis
    plt.subplot(3, 3, 7)
    
    # Create correlation matrix
    data_matrix = []
    for entry in neuromod_log:
        row = [entry[neurom] for neuromod in neuromodulators]
        data_matrix.append(row)
    
    corr_matrix = np.corrcoef(data_matrix).flatten()
    
    # Plot correlation heatmap
    plt.imshow(np.array(data_matrix).T, aspect='auto', cmap='RdBu', vmin=-1, vmax=1)
    plt.colorbar(label='Correlation')
    plt.title("Neuromodulator Correlations")
    plt.xlabel("Neuromodulators")
    plt.ylabel("Neuromodulators")
    
    # Find synchronized events
    plt.subplot(3, 3, 8)
    
    # Detect co-activation events
    dopamine_levels = [entry['dopamine'] for entry in neuromod_log]
    curiosity_levels = [entry['curiosity'] for entry in neuromod_log]
    reward_levels = [entry['reward'] for entry in neuromod_log]
    
    # Find steps where all are high (> 0.7)
    sync_steps = []
    for i, (dop, cur, rew) in enumerate(zip(dopamine_levels, curiosity_levels, reward_levels)):
        if dop > 0.7 and cur > 0.7 and rew > 0.7:
            sync_steps.append(i)
    
    if sync_steps:
        plt.scatter(sync_steps, [1] * len(sync_steps), color='red', s=50, label='Synchronized')
        plt.plot([0, len(neuromod_log)], [0, 1], 'k--', alpha=0.5, label='Baseline')
        plt.title(f"Co-activation Events (n={len(sync_steps)})")
    else:
        plt.text(0.5, 0.5, "No synchronization detected", 
                horizontalalignment='center', verticalalignment='center',
                transform=plt.gca().transAxes)
        plt.title("Co-activation Events")
    
    plt.ylim(0, 1)
    plt.legend()
    
    # Novelty-driven exploration
    plt.subplot(3, 3, 9)
    
    # Analyze novelty-driven behavior
    novelty_levels = [entry['novelty'] for entry in neuromod_log]
    curiosity_levels = [entry['curiosity'] for entry in neuromod_log]
    
    # Calculate exploration index
    exploration_index = np.array(novelty_levels) * np.array(curiosity_levels)
    
    plt.plot(exploration_index, label='Exploration Index')
    plt.title("Novelty-Driven Exploration")
    plt.ylabel("Exploration Level")
    plt.xlabel("Time Step")
    plt.grid(True)
    plt.legend()
    
    plt.tight_layout()
    plt.savefig('neuromodulator_analysis.png')
    plt.show()
    
    return {
        'correlation_matrix': np.corrcoef(data_matrix),
        'sync_events': sync_steps,
        'exploration_index': exploration_index
    }
```

## Neuromodulator Research Applications

### 1. Neuromodulator Knockout Studies

```python
import pynlm

def neuromodulator_knockout_study():
    """Simulate knockout of specific neuromodulators."""
    
    # Create baseline brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test baseline performance
    baseline = run_behavioral_task(brain, task_config)
    
    # Test with different neuromodulators disabled
    neuromodulators_to_test = ['dopamine', 'curiosity', 'novelty', 'serotonin']
    
    knockout_results = {}
    
    for neuromod in neuromodulators_to_test:
        # Create brain with neuromodulator disabled
        config2 = pynlm.createDefaultConfig()
        config2.set(f"neuromod.{neuromod}.enable", False)
        
        brain2 = pynlm.createBrain(config2)
        brain2.initialize()
        
        # Run same task
        result = run_behavioral_task(brain2, task_config)
        
        knockout_results[neuromod] = {
            'performance_change': result['performance'] - baseline['performance'],
            'exploration_change': result['exploration'] - baseline['exploration'],
            'learning_change': result['learning_rate'] - baseline['learning_rate']
        }
        
        print(f"Knockout {neurom}: Performance change = {knockout_results[neurom]['performance_change']:.3f}")
    
    return knockout_results
```

### 2. Neuromodulator Dose-Response

```python
import pynlm

def neuromodulator_dose_response():
    """Test different levels of neuromodulator activity."""
    
    # Test different dopamine levels
    dopamine_levels = [0.1, 0.3, 0.5, 0.7, 1.0, 1.5]
    results = {}
    
    for level in dopamine_levels:
        # Create brain with specific dopamine level
        config = pynlm.createDefaultConfig()
        config.set("neuromod.dopamine.scale", level)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run task
        result = run_behavioral_task(brain, task_config)
        
        results[level] = {
            'performance': result['performance'],
            'exploration': result['exploration'],
            'learning_rate': result['learning_rate'],
            'error_rate': result['error_rate']
        }
        
        print(f"Dopamine level {level}: Performance = {result['performance']:.3f}")
    
    # Plot dose-response curve
    plt.figure()
    
    plt.subplot(2, 2, 1)
    plt.plot(list(results.keys()), [r['performance'] for r in results.values()], 'o-')
    plt.title("Performance vs Dopamine Level")
    plt.xlabel("Dopamine Level")
    plt.ylabel("Performance")
    
    plt.subplot(2, 2, 2)
    plt.plot(list(results.keys()), [r['exploration'] for r in results.values()], 'o-')
    plt.title("Exploration vs Dopamine Level")
    plt.xlabel("Dopamine Level")
    plt.ylabel("Exploration")
    
    plt.subplot(2, 2, 3)
    plt.plot(list(results.keys()), [r['learning_rate'] for r in results.values()], 'o-')
    plt.title("Learning Rate vs Dopamine Level")
    plt.xlabel("Dopamine Level")
    plt.ylabel("Learning Rate")
    
    plt.subplot(2, 2, 4)
    plt.plot(list(results.keys()), [r['error_rate'] for r in results.values()], 'o-')
    plt.title("Error Rate vs Dopamine Level")
    plt.xlabel("Dopamine Level")
    plt.ylabel("Error Rate")
    
    plt.tight_layout()
    plt.savefig('dopamine_dose_response.png')
    plt.show()
    
    return results
```

### 3. Neuromodulator Interaction Analysis

```python
import pynlm
import numpy as np

def analyze_neuromodulator_interactions():
    """Analyze complex interactions between neuromodulators."""
    
    # Create multi-dimensional feature space
    features = []
    labels = []
    
    # Generate different neuromodulator combinations
    dopamine_levels = [0.2, 0.5, 0.8, 1.0]
    curiosity_levels = [0.1, 0.5, 0.9]
    novelty_levels = [0.1, 0.5, 0.9]
    
    for d in dopamine_levels:
        for c in curiosity_levels:
            for n in novelty_levels:
                # Create brain with specific neuromodulator configuration
                config = pynlm.createDefaultConfig()
                config.set("neuromod.dopamine.scale", d)
                config.set("neuromod.curiosity.novelty_weight", c)
                config.set("neuromod.novelty.level", n)
                
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                # Run task and collect features
                task_features = extract_task_features(brain)
                task_features.extend([d, c, n])
                features.append(task_features)
                labels.append(f"D{d}C{c}N{n}")
    
    features = np.array(features)
    
    # Perform principal component analysis
    from sklearn.decomposition import PCA
    pca = PCA(n_components=2)
    pca_features = pca.fit_transform(features)
    
    # Visualize neuromodulator combinations
    plt.figure(figsize=(12, 8))
    
    plt.subplot(2, 2, 1)
    scatter = plt.scatter(pca_features[:, 0], pca_features[:, 1], 
                         c=range(len(features)), cmap='viridis', alpha=0.7)
    plt.colorbar(scatter, label='Combination Index')
    plt.title("Neuromodulator Combination Space")
    plt.xlabel("PC1")
    plt.ylabel("PC2")
    
    # Plot individual neuromodulator effects
    plt.subplot(2, 2, 2)
    for i, neuromod in enumerate(['dopamine', 'curiosity', 'novelty']):
        neuromod_values = [f[i] for f in features]
        task_performance = [f[4] for f in features]  # Assuming task performance is feature 4
        
        plt.scatter(neurom_values, task_performance, 
                   alpha=0.5, label=neurom)
    
    plt.title("Individual Neuromodulator Effects")
    plt.xlabel("Neuromodulator Level")
    plt.ylabel("Task Performance")
    plt.legend()
    
    # Interaction plots
    plt.subplot(2, 2, 3)
    
    # Dopamine x Curiosity interaction
    for novelty in novelty_levels:
        subset = [i for i, (d, c, n) in enumerate([(f[3], f[4], f[5]) for f in features]) 
                 if n == novelty]
        if subset:
            d_values = [features[i][3] for i in subset]
            perf_values = [features[i][4] for i in subset]
            plt.plot(d_values, perf_values, 'o-', label=f"Novelty {novelty}")
    
    plt.title("Dopamine x Curiosity Interaction")
    plt.xlabel("Dopamine Level")
    plt.ylabel("Task Performance")
    plt.legend()
    
    # Novelty x Curiosity interaction
    plt.subplot(2, 2, 4)
    
    for dopamine in dopamine_levels:
        subset = [i for i, (d, c, n) in enumerate([(f[3], f[4], f[5]) for f in features]) 
                 if d == dopamine]
        if subset:
            c_values = [features[i][4] for i in subset]
            perf_values = [features[i][4] for i in subset]
            plt.plot(c_values, perf_values, 'o-', label=f"Dopamine {dopamine}")
    
    plt.title("Novelty x Curiosity Interaction")
    plt.xlabel("Curiosity Level")
    plt.ylabel("Task Performance")
    plt.legend()
    
    plt.tight_layout()
    plt.savefig('neuromodulator_interactions.png')
    plt.show()
    
    return {
        'features': features,
        'labels': labels,
        'pca_features': pca_features,
        'pca_explained_variance': pca.explained_variance_ratio_
    }
```

## Conclusion

Neuromodulation is the cornerstone of adaptive, intelligent behavior in NLM. The 8 neuromodulators work together to:

1. **Control Learning**: Modulate plasticity rates and learning mechanisms
2. **Guide Behavior**: Balance exploration vs. exploitation
3. **Regulate Attention**: Focus processing on relevant information
4. **Maintain Motivation**: Drive goal-directed behavior
5. **Adapt to Environment**: Adjust to changing task demands
6. **Integrate Systems**: Coordinate activity across multiple brain systems

The neuromodulator system enables NLM to:

- **Learn efficiently** from rewards, errors, and novel experiences
- **Explore when needed** and exploit when appropriate
- **Focus attention** on relevant stimuli
- **Maintain motivation** through reward and curiosity signals
- **Adapt behavior** based on changing environmental demands
- **Integrate multiple learning signals** into coherent behavior

Through precise control of neural activity and plasticity, neuromodulators make NLM capable of:

- **Robust learning** in dynamic environments
- **Flexible behavior** in novel situations
- **Efficient resource allocation** across cognitive functions
- **Adaptive response** to changing task demands
- **Integrated cognition** across multiple systems

The neuromodulation system transforms NLM from a simple neural simulator into a sophisticated cognitive architecture capable of intelligent, adaptive behavior that emerges from the interaction of multiple neuromodulatory signals.

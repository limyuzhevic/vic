# NLM Experiments

## Overview

NLM uses experiments to evaluate its neural and cognitive capabilities. This document outlines the experimental framework, expected metrics, and current experiment status.

**Important Note:** NLM is in Phase 6 (Final Integration) with approximately 35.8% system integration achieved. Most cognitive systems (memory, prediction, higher cognition) are disconnected from the main brain loop. The experiments below reflect what's currently possible vs. what's intended but not yet implemented.

## Phase 3: Basic Environment Interaction (WORKING)

### Phase 3.1: World Interaction Demo
The basic demonstration of the complete neural loop:

```
NLM brain → actions → world changes → sensory input → reward → plasticity
```

**Current Status:** ✅ WORKING

**Run with:**
```bash
./nlm_phase3_demo 5 200
```

**Expected outcomes:**
- Brain receives visual input (16x16 grayscale)
- Motor activity produces actions  
- Reward signals modify synapses
- Behavior changes across episodes

### Phase 3.2: Neuromodulation Experiment
**Current Status:** ✅ WORKING (Phase 2)

**What works:**
- Curiosity-driven exploration behavior
- Reward prediction error computation
- Novelty detection from sensory patterns
- Development stage progression

**What doesn't work:**
- Full dopamine integration with memory systems
- ACh effects on attention
- NE effects on arousal

## Phase 4: Cognitive Experiments (INCOMPLETE)

### **IMPORTANT STATUS UPDATE:**
All Phase 4 experiments are designed but **NOT FULLY IMPLEMENTED**:

- Neural memory systems exist but return nullptr
- Prediction systems exist but never trained
- Cognition systems exist but never instantiated
- Most experiments in this document are conceptual outlines

### What Actually Works (Current Capabilities)

#### Basic Behavioral Experiments
```python
# This actually works (Phase 3 level)
import pynlm

def run_basic_behavioral_experiment():
    # Setup working brain/world/agent loop
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Run behavioral experiment
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: curiosity={agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world
```

#### Neuromodulation Experiments
```python
# This works (Phase 2)
import pynlm

def run_curiosity_experiment():
    """Test curiosity-driven exploration behavior"""
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableCuriosity(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20)
    world.reset()
    agent.initialize(world)
    
    exploration_rates = []
    novelty_levels = []
    
    # Run exploration experiment
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Record neuromodulation levels
        exploration_rates.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
    
    return exploration_rates, novelty_levels

exploration, novelty = run_curiosity_experiment()
print(f"Average curiosity: {sum(exploration)/len(exploration):.3f}")
```

### Phase 4 Cognitive Experiments (NOT IMPLEMENTED)

#### Phase 4.1: Temporal Prediction Learning
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- Tests if NLM can learn temporal sequences
- Present pattern A followed by pattern B repeatedly
- Measure prediction error over time
- Track improvement in prediction accuracy

**Implementation Note:** Requires Prediction System integration (not connected)

#### Phase 4.2: Working Memory
**Current Status:** ❌ NOT IMPLEMENTED  

**Conceptual Design:**
- Test if information can be maintained after stimulus disappears
- Present pattern A, add delay, test retention
- Measure retention accuracy over time, capacity limits, decay rate

**Implementation Note:** Requires Working Memory system integration (not connected)

#### Phase 4.3: Episodic Recall
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- Store episodes with associated reward
- After delay, retrieve similar episodes
- Measure influence on behavior

**Implementation Note:** Requires Episodic Memory system integration (not connected)

#### Phase 4.4: Concept Formation
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- Present variations of same underlying pattern
- Track if representations become more stable
- Measure generalization to new variations

**Implementation Note:** Requires Concept Formation system integration (not connected)

#### Phase 4.5: Attention Selection
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- Present multiple competing stimuli
- Measure which neural populations win
- Track attention dynamics

**Implementation Note:** Requires Attention system integration (not connected)

#### Phase 4.6: Multi-Step Planning
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- Set goal state
- NLM plans action sequence
- Execute and measure success

**Implementation Note:** Requires Neural Planner system integration (not connected)

#### Phase 4.7: Self-Action Prediction
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- NLM performs action
- Record sensory change
- Later, predict consequence before acting

**Implementation Note:** Requires Self-Model system integration (not connected)

#### Phase 4.8: Object Permanence
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- Object visible → agent approaches
- Object hidden (behind barrier)
- Agent continues to search for hidden object

**Implementation Note:** Limited without episodic memory

#### Phase 4.9: Social Learning
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- Another agent performs action
- NLM observes action and consequence
- NLM attempts to imitate later

**Implementation Note:** Requires Social Learning system integration (not connected)

#### Phase 4.10: Continual Learning
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- Learn Task A to criterion
- Learn Task B
- Test performance on Task A again

**Implementation Note:** No continual learning without memory integration

#### Phase 4.11: Generalization
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- Learn concept in Context A
- Test concept in Context B (with variations)
- Measure transfer success

**Implementation Note:** No concept formation system

#### Phase 4.12: Integrated Experiment
**Current Status:** ❌ NOT IMPLEMENTED

**Conceptual Design:**
- Run multiple episodes with all systems active
- Track all metrics over time
- Compare early vs late performance

**Implementation Note:** No integration of multiple cognitive systems

## Running Phase 3 Experiments

### Basic Experiment Commands
```bash
# Phase 3.1: World interaction demo
./nlm_phase3_demo 5 200

# Phase 3.2: Neuromodulation experiment (concept)
./nlm_phase4_demo 10 500  # Framework exists but may not fully work
```

### Python-Based Experiments
```python
import pynlm
import numpy as np
from scipy import stats

class NLMExperimentFramework:
    """Framework for running NLM experiments with current capabilities"""
    
    def __init__(self):
        self.results = {}
    
    def run_basic_behavioral_experiment(self, steps=1000, neurons=1000):
        """Run basic behavioral experiment (what actually works)"""
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neurons)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Run experiment
        metrics = {
            'steps': steps,
            'neurons': neurons,
            'firing_rate': [],
            'curiosity': [],
            'novelty': [],
            'development_stage': []
        }
        
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            metrics['firing_rate'].append(brain.getFiringNeuronCount())
            metrics['curiosity'].append(agent.getCuriosityLevel())
            metrics['novelty'].append(agent.getNoveltyLevel())
            metrics['development_stage'].append(brain.getDevelopmentalStage())
        
        self.results['basic_behavioral'] = metrics
        return metrics
    
    def analyze_behavioral_data(self, metrics):
        """Analyze behavioral experiment data"""
        
        analysis = {}
        
        # Firing rate analysis
        firing_rates = metrics['firing_rate']
        analysis['firing_rate'] = {
            'mean': np.mean(firing_rates),
            'std': np.std(firing_rates),
            'max': np.max(firing_rates),
            'min': np.min(firing_rates),
            'trend': np.polyfit(range(len(firing_rates)), firing_rates, 1)[0]
        }
        
        # Curiosity analysis
        curiosity = metrics['curiosity']
        analysis['curiosity'] = {
            'mean': np.mean(curiosity),
            'std': np.std(curiosity),
            'max': np.max(curiosity),
            'min': np.min(curiosity),
            'exploration_effect': np.corrcoef(firing_rates, curiosity)[0,1]
        }
        
        # Novelty analysis
        novelty = metrics['novelty']
        analysis['novelty'] = {
            'mean': np.mean(novelty),
            'std': np.std(novelty),
            'max': np.max(novelty),
            'min': np.min(novelty)
        }
        
        # Development analysis
        development = metrics['development_stage']
        analysis['development'] = {
            'stages_reached': len(set(development)),
            'development_rate': (development[-1] - development[0]) / len(development),
            'stability': 1.0 - np.std(development)
        }
        
        return analysis

# Usage example
experiment = NLMExperimentFramework()
metrics = experiment.run_basic_behavioral_experiment(steps=500, neurons=500)
analysis = experiment.analyze_behavioral_data(metrics)

print("Experiment Analysis:")
for category, data in analysis.items():
    print(f"{category}:")
    for key, value in data.items():
        print(f"  {key}: {value:.4f}" if isinstance(value, float) else f"  {key}: {value}")
```

## Experiment Configuration

### Experiment Parameters
```cpp
config->set("neuron_count", size_t(1000));
config->set("prediction_horizon", size_t(3));
config->set("working_memory_capacity", size_t(100));
```

### Standard Metrics

#### Neural Activity Metrics
- **Firing rate distribution**: Mean, std, max, min across neurons
- **Spike timing statistics**: Inter-spike intervals, coefficient of variation
- **Population activity levels**: Synchronization measures, synchrony indices
- **Neural coding efficiency**: Information theory measures

#### Memory Metrics
- **Retention accuracy**: Pattern recall success rates
- **Recall success rate**: Probability of correct memory retrieval
- **Pattern stability**: Resistance to interference over time
- **Generalization**: Transfer to novel but related patterns

#### Learning Metrics
- **Prediction error over time**: Convergence rates, asymptotic values
- **Concept formation rate**: Speed of concept emergence
- **Planning success**: Multi-step action sequence completion rates
- **Self-prediction accuracy**: Accuracy of action consequence prediction

#### Social Metrics
- **Imitation success**: Accuracy of copying observed behaviors
- **Observation efficiency**: Information extracted per observation unit
- **Communication signal learning**: Association strength of communication signals

## Anti-Cheating Measures

Phase 3 experiments are designed to prevent shortcuts:
- No predefined correct answers
- No hard-coded task solutions
- No symbolic lookups
- External experiment code measures, not solves

## Reproducibility

All experiments use:
- Explicit random seeds
- Version-controlled code
- Deterministic simulation
- Multiple seed runs for statistics

## Current Experimental Capabilities

### What Works (Run These Experiments)

1. **Basic Behavioral Learning**: Standard NLM simulation with curiosity and development
2. **Exploration vs. Exploitation**: Curiosity-driven vs. reward-driven behavior
3. **Development Trajectories**: Brain maturation over time
4. **Sensory Integration**: Vision, touch, internal sensing processing
5. **Motor Control**: Action selection from neural activity

### What Needs Integration (Future Experiments)

1. **Working Memory**: Information maintenance over delays
2. **Episodic Memory**: Experience storage and recall
3. **Prediction**: Expectation formation and error signals
4. **Concept Formation**: Pattern abstraction and generalization
5. **Attention**: Competitive selection among alternatives
6. **Planning**: Multi-step action sequence evaluation
7. **Self-Model**: Prediction of self-generated effects
8. **Social Learning**: Imitation and observation
9. **Continual Learning**: Knowledge retention and transfer
10. **Object Permanence**: Hidden object representation

## Research Questions Current Experiments Address

### 1. Can Neural Systems Learn Basic Behaviors?
- Question: Does NLM exhibit adaptive behavior?
- Method: Run basic behavioral experiments
- Measure: Learning curves, performance metrics

### 2. How Does Neuromodulation Affect Learning?
- Question: What role does curiosity play?
- Method: Manipulate curiosity levels
- Measure: Exploration-exploitation balance

### 3. Does Development Change Learning?
- Question: How does brain maturation affect capabilities?
- Method: Track developmental stages
- Measure: Age-dependent performance changes

### 4. Can Systems Handle Complex Environments?
- Question: What are the limits of current capabilities?
- Method: Test in varied environments
- Measure: Adaptation, robustness, generalization

## Example Experimental Protocol

### Protocol 1: Curiosity-Driven Exploration
```python
import pynlm
import matplotlib.pyplot as plt

def curiosity_experiment():
    """Test curiosity-driven exploration behavior"""
    
    # Initialize NLM
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    agent.initialize(world)
    
    # Run experiment
    curiosity_data = []
    exploration_data = []
    reward_data = []
    
    for step in range(1000):
        # Update environment
        world.update(0.1)
        
        # Record state
        current_state = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(current_state)
        
        # Neural computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Record metrics
        curiosity_data.append(agent.getCuriosityLevel())
        exploration_data.append(agent.getExplorationRate())
        reward_data.append(reward)
        
        # Periodic data collection
        if step % 100 == 0:
            print(f"Step {step}: curiosity={curiosity_data[-1]:.3f}, "
                  f"exploration={exploration_data[-1]:.3f}, reward={reward:.3f}")
    
    # Analyze results
    fig, axes = plt.subplots(3, 1, figsize=(12, 10))
    
    axes[0].plot(curiosity_data)
    axes[0].set_title("Curiosity Level Over Time")
    axes[0].set_ylabel("Curiosity")
    
    axes[1].plot(exploration_data)
    axes[1].set_title("Exploration Rate Over Time")
    axes[1].set_ylabel("Exploration")
    
    axes[2].plot(reward_data)
    axes[2].set_title("Reward Over Time")
    axes[2].set_ylabel("Reward")
    
    plt.tight_layout()
    plt.savefig('curiosity_experiment.png')
    plt.show()
    
    return {
        'curiosity': curiosity_data,
        'exploration': exploration_data,
        'reward': reward_data
    }

# Run experiment
results = curiosity_experiment()
```

## Conclusion

### Current State
NLM experiments can successfully demonstrate:
- Basic behavioral learning
- Neuromodulation effects
- Development trajectories
- Sensory-motor integration

### Future Directions
Phase 4 integration will enable experiments on:
- Working memory capacity
- Episodic memory recall
- Predictive cognition
- Conceptual abstraction
- Planning abilities
- Social learning
- Continual learning

### Experimental Recommendations
1. **Start with Phase 3 experiments** to understand current capabilities
2. **Use multiple replicates** for statistical reliability
3. **Document parameters** for reproducibility
4. **Combine experimental with computational analysis**
5. **Focus on incremental integration** of cognitive systems

**Remember:** Phase 6's primary goal is **integration over new features**. The existing systems need to work together as a coherent whole before adding more disconnected components.
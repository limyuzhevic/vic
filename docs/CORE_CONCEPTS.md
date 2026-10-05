# NLM - CORE_CONCEPTS: Understanding NLM Architecture

## Introduction

This document explains the core concepts, architecture, and components that make NLM (Neural Learning Machine) a functioning artificial developmental brain. It covers how neurons work, how memory is stored, how learning occurs, and how cognition emerges from neural dynamics.

## NLM Brain Architecture Overview

NLM operates as a closed-loop system that continuously processes sensory information, makes decisions, acts in the world, and learns from the consequences:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## Core Components

### 1. Neural Core (The Brain)
- **LIF Neurons**: Leaky Integrate-and-Fire neurons with realistic dynamics
- **Spike Propagation**: Event-driven communication between neurons
- **Synapses**: Excitatory/inhibitory connections with plasticity
- **Neural Regions**: Organized groups of neurons with specialized functions
- **Spike Systems**: Efficient event-driven spike processing

### 2. Memory Systems
- **Working Memory**: Persistent activity for active information maintenance
- **Episodic Memory**: Storage of experience sequences
- **Associative Memory**: Pattern-based connections
- **Concept Formation**: Abstract representation of learned patterns

### 3. Prediction and Cognition
- **Prediction System**: Anticipates sensory input and actions
- **Attention**: Selective focus on relevant information
- **Planning**: Evaluates action sequences
- **Self-Model**: Internal representation of the agent

### 4. Neuromodulation
- **Dopamine**: Reward prediction error signals
- **Curiosity**: Exploration motivation based on novelty
- **Novelty Detection**: Difference from expected input
- **Prediction Error**: Discrepancy between expected and actual outcomes

### 5. Developmental System
- **Developmental Stages**: Age-dependent plasticity modulation
- **Structural Plasticity**: Creation and pruning of connections
- **Maturation**: Progressive development of capabilities

### 6. Environment Interface
- **Sensory Processing**: Vision, touch, proprioception, internal sensors
- **Motor Output**: Action selection and execution
- **World Model**: Internal representation of environment

## Key Concepts

### 1. Neurons (LIF Model)
Biological neurons are complex, but NLM uses a simplified but realistic model:

```cpp
class Neuron {
public:
    MembranePotential v;      // Membrane voltage
    float threshold;          // Firing threshold
    float refractoryPeriod;   // Refractory period after firing
    float membranePotential;  // Current membrane voltage
    
    void update(Timestep dt) {
        // Leaky integration: dv/dt = -(v - v_rest)/tau
        membranePotential += (-membranePotential + v_rest) * dt / tau;
        
        // Check for threshold crossing
        if (membranePotential >= threshold && refractoryPeriod <= 0) {
            fire();  // Generate spike
        }
        
        // Refractory period
        refractoryPeriod -= dt;
    }
};
```

### 2. Synaptic Plasticity
Learning occurs through synaptic weight changes:

#### Hebbian Learning
> "Neurons that fire together, wire together"

```cpp
// Weight increases when pre- and post-synaptic neurons fire close in time
void hebbianUpdate(Neuron* pre, Neuron* post) {
    float weightChange = learningRate * pre->output * post->input;
    pre->weight += weightChange;
}
```

#### STDP (Spike-Timing-Dependent Plasticity)
Learning depends on precise spike timing:

```cpp
// If pre-synaptic spike precedes post-synaptic spike
// → strengthen connection
// If post-synaptic spike precedes pre-synaptic spike  
// → weaken connection
void stdpUpdate(float preSpikeTime, float postSpikeTime) {
    float deltaT = postSpikeTime - preSpikeTime;
    
    if (deltaT > 0) {
        // LTD: Long-Term Depression
        weight *= 1 - learningRate * deltaT;
    } else {
        // LTP: Long-Term Potentiation
        weight *= 1 + learningRate * abs(deltaT);
    }
}
```

### 3. Memory Systems

#### Working Memory
- Maintains active information through persistent activity
- Uses recurrent connections for sustained firing
- Competitive dynamics for selective attention

```cpp
class NeuralWorkingMemory {
public:
    // Store pattern in working memory
    void storePattern(const NeuralPattern& pattern);
    
    // Retrieve pattern from working memory
    NeuralPattern recallPattern() const;
    
    // Decay inactive patterns
    void decay(double dt);
    
    // Competition between patterns
    void selectPattern();
};
```

#### Episodic Memory
- Stores experience sequences
- Enables replay and consolidation
- Links events in temporal order

```cpp
class NeuralEpisodicMemory {
public:
    // Encode experience
    void encode(const Experience& exp);
    
    // Retrieve similar experiences
    std::vector<Experience> retrieveSimilar(const Experience& pattern);
    
    // Replay experiences (during sleep/rest)
    void replay(double fraction = 0.1);
};
```

### 4. Prediction Systems

#### Sensory Prediction
Predicts what will be sensed based on current state:

```cpp
class PredictionSystem {
public:
    // Update prediction model
    void predict(const NeuralState& current);
    
    // Get expected sensory input
    SensoryInput getExpectedInput() const;
    
    // Compute prediction error
    float computeError(const SensoryInput& actual);
};
```

#### Action Prediction
Anticipates consequences of actions:

```cpp
class ActionConsequencePredictor {
public:
    // Predict outcome of action
    PredictedOutcome predictOutcome(MotorCommand action);
    
    // Update based on actual result
    void learnFromOutcome(MotorCommand action, const ActionResult& result);
};
```

### 5. Neuromodulation

#### Dopamine (Reward Signal)
- Encodes reward prediction error
- Modulates learning rates
- Influences motivation

```cpp
class Dopamine {
public:
    // Update with reward prediction error
    void update(float reward, float prediction);
    
    // Get current dopamine level
    float getLevel() const { return level_; }
    
    // Modulate learning rate
    float getLearningModulator() const;
};
```

#### Curiosity (Exploration)
- Combines novelty and prediction error
- Drives exploration behavior
- Balances exploitation vs exploration

```cpp
class Curiosity {
public:
    // Update with novelty signal
    void update(float novelty, float predictionError);
    
    // Get exploration motivation
    float getMotivation() const { return motivation_; }
    
    // Decide whether to explore
    bool shouldExplore() const;
};
```

### 6. Developmental System

#### Developmental Stages
- Modifies plasticity with age
- Controls synaptogenesis and pruning
- Implements critical periods

```cpp
class DevelopmentSystem {
public:
    // Update plasticity based on developmental stage
    void updatePlasticityModifiers(double timestep);
    
    // Check if in critical period
    bool isInCriticalPeriod(PlasticityType type) const;
    
    // Get current developmental stage
    DevelopmentalStage getStage() const { return stage_; }
};
```

## Learning Process

### How NLM Learns

1. **Experience Collection**: Agent interacts with environment
2. **Sensory Processing**: World input is transduced into neural signals
3. **Prediction**: Brain anticipates sensory input
4. **Action Selection**: Brain chooses motor command based on predictions
5. **Execution**: Action produces environmental consequence
6. **Reward**: Consequence provides reward/error signal
7. **Learning**: Neuromodulators update synaptic weights
8. **Memory**: Experiences are stored in memory systems
9. **Development**: Brain structure changes with experience
10. **Future Behavior**: Changed brain produces different actions

### Feedback Loop

```
Sensory Input → Neural Processing → Action Selection → Motor Output → World → Reward → Plasticity → Changed Brain
                                            ↑                                        ↓
                                    Memory/Prediction ←────────────────── Development ←─────────────────
```

## Integration and Coordination

### Memory Integration
Memory systems are not isolated - they integrate with neural processing:

- **Working Memory**: Provides context for current processing
- **Episodic Memory**: Supplies past experiences for pattern completion
- **Associative Memory**: Creates connections between concepts

### Prediction Integration
Predictions affect all levels of processing:

- **Sensory Prediction**: Filters incoming sensory input
- **Action Prediction**: Guides action selection
- **Social Prediction**: Anticipates other agents' behavior

### Neuromodulation Integration
Neuromodulators coordinate system-wide changes:

- **Dopamine**: Scales learning across all plasticity systems
- **Curiosity**: Balances exploration across actions
- **Novelty**: Highlights important sensory changes

## Biological Inspiration

### What NLM Gets Right
- Real spiking neural dynamics
- Synaptic plasticity with timing dependence
- Memory systems with distinct functions
- Neuromodulation for learning and motivation
- Developmental changes that affect learning

### Known Limitations
- Simplified neuron models (no ion channels)
- No detailed dendritic processing
- Limited network architectures
- No large-scale brain regions
- No consciousness or subjective experience

## Future Extensions

### Planned Enhancements
1. **Hierarchical Processing**: Multiple levels of abstraction
2. **Detailed Neurobiology**: More realistic neuron models
3. **Social Cognition**: Theory of mind and empathy
4. **Language Processing**: Natural language understanding
5. **Long-term Memory**: Semantic and procedural memory
6. **Consciousness**: Integrated information processing

### Research Directions
- How prediction affects memory consolidation
- Role of development in cognitive emergence
- Integration of attention and working memory
- Biological plausibility of artificial systems
- Transfer learning across tasks and domains

## Key Takeaways

1. **NLM is a system, not just a model**: Integration is as important as individual components
2. **Learning is continuous**: Development and experience continuously change the brain
3. **Prediction is central**: The brain constantly predicts its sensory input
4. **Memory supports cognition**: Past experiences guide future behavior
5. **Neuromodulation coordinates**: Multiple signals work together to guide learning
6. **Development is lifelong**: The brain changes throughout its "lifetime"

## Further Reading

- [SCIENCE.md](SCIENCE.md) - Scientific background and limitations
- [ARCHITECTURE.md](ARCHITECTURE.md) - Detailed component descriptions
- [examples/01_basic-brain.md](examples/01_basic-brain.md) - Basic usage examples
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API documentation
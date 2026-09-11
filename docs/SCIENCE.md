# NLM Scientific Background

## Overview

NLM is inspired by biological brains but makes no claims of being a accurate simulation. This document explains the biological inspiration and known approximations.

## Neural Plasticity

### Biological Inspiration
Synaptic plasticity is the biological basis of learning. When neurons fire together, the connections between them strengthen (Hebbian learning). The precise timing of spikes matters for some forms of learning.

### Computational Implementation
- **Hebbian Learning**: "Neurons that fire together, wire together" - simple correlation-based weight changes
- **STDP (Spike-Timing-Dependent Plasticity)**: Weight changes depend on the relative timing of pre- and post-synaptic spikes

### Known Limitations
- Real STDP involves complex molecular cascades (calcium signaling, NMDA receptors)
- Weight changes depend on many factors not modeled (neuromodulators, synapse type, dendritic location)
- Our implementation is a simplified approximation

## Neural Dynamics

### Biological Inspiration
Real neurons maintain membrane potentials, fire action potentials when threshold is reached, and have refractory periods.

### Computational Implementation
- **Integrate-and-Fire**: Simple model where membrane potential integrates input until threshold
- **Leaky Integration**: Membrane potential decays towards rest

### Known Limitations
- Real neurons have complex ion channel dynamics (Hodgkin-Huxley)
- Detailed spike shapes and dendritic processing are not modeled
- Metabolic constraints and energy efficiency not considered

## Developmental Learning

### Biological Inspiration
Brain development involves:
- Synaptogenesis (formation of new synapses)
- Pruning (removal of unused synapses)
- Critical periods (sensitive periods for learning)
- Maturation (development of adult-level properties)

### Computational Implementation
- Probabilistic synapse formation and removal
- Plasticity rates that change with developmental stage
- Simple critical period modeling

### Known Limitations
- Real development involves complex genetic programs, activity-dependent refinement, and-glia interactions
- Our implementation is highly simplified

## Neuromodulation

### Biological Inspiration
Neuromodulators (dopamine, acetylcholine, norepinephrine, serotonin) modulate neural activity and plasticity. They signal reward, attention, arousal, and other internal states.

### Computational Implementation
- **Dopamine**: Reward prediction error signals
- **Novelty Detection**: Simple difference from expected
- **Curiosity**: Exploration motivation based on novelty/error

### Known Limitations
- Real neuromodulatory systems are vastly more complex
- Receptor types, receptor distribution, and second-messenger systems not modeled
- Effects on neural dynamics are simplified

## Phase 6: Integration Architecture

### Overview
Phase 6 focuses on integrating the existing disconnected systems into a coherent artificial brain. The key insight is that NLM has the *anatomy* of a cognitive system but lacks the *integration* that makes it function as a coherent artificial brain.

### Integration Architecture
The complete brain loop integrates all Phase 4 cognitive mechanisms:

```
WORLD → SENSORY PERCEPT → AgentBrain.processSensoryInput() →
Brain.receiveSensoryInput() → Neural Processing → Memory Systems →
Prediction System → Neuromodulation → AgentBrain.decodeMotorCommand() →
WORLD.applyMotorCommand() → Reward → AgentBrain.applyRewardModulation()
```

### Integration Components

#### Agent-Brain Interface Layer
- **AgentBrain**: Connects NLM brain to world
- **SensoryPercept**: Sensory data packet from agent
- **AgentBody**: Physical agent properties and state

#### Integration Flow
1. **Sensory Transduction**: Converts world percepts to neural input
2. **Memory Storage**: Stores experiences in working/episodic memory
3. **Prediction Training**: Uses predictions for action planning
4. **Attention Mechanisms**: Selects relevant information
5. **Concept Formation**: Discovers patterns from experiences
6. **Action Planning**: Uses predictions to select actions
7. **Self-Model Updates**: Learns body schema from experience
8. **Neuromodulation**: Applies reward signals to plasticity

## Phase 4: Emerging Cognition (Updated)

### Integration with Main Brain Loop
Phase 4 cognitive mechanisms are now integrated into the main brain simulation:

```
WORLD (SimpleWorld)
      ↓
ATTENTION (integrated into AgentBrain)
      ↓
SENSORY INPUT
      ↓
PREDICTION (integrated into Brain)
      ↓
WORKING MEMORY (integrated into Brain)
      ↓
EPISODIC MEMORY (integrated into Brain)
      ↓
CONCEPTS (integrated into AgentBrain)
      ↓
PLANNING (integrated into AgentBrain)
      ↓
SELF-MODEL (integrated into AgentBrain)
      ↓
ACTION SELECTION
      ↓
MOTOR OUTPUT
      ↓
WORLD.applyMotorCommand()
      ↓
REWARD / CONSEQUENCE
      ↓
SOCIAL OBSERVATION (if others present)
      ↓
LEARNING UPDATES
```

### Neural Prediction System
- **NeuralPrediction**: Temporal sequence learning
- **Integrated into**: Brain prediction system
- **Learning**: From sensory experiences
- **Application**: Guides action selection

### Working Memory Integration
- **NeuralWorkingMemory**: Persistent activity for temporary storage
- **Integration**: Part of main brain loop
- **Function**: Maintains relevant information for cognitive processing

### Episodic Memory Integration
- **NeuralEpisodicMemory**: Experience encoding in neural patterns
- **Integration**: Updated during each brain step
- **Function**: Stores complete experiences for later retrieval

### Cognitive Mechanisms Integration
- **AttentionalSelection**: Competitive processing of working memory
- **NeuralPlanner**: Action sequence evaluation
- **ConceptFormation**: Pattern discovery from sensory input
- **SelfModel**: Sensorimotor self-awareness development
- **SocialLearning**: Observation and imitation of other agents

## The Learning Loop (Updated)

### Phase 2: Isolated Neural Computation
```
SENSORY INPUT (test signals)
      ↓
NEURAL ACTIVITY
      ↓
SPIKE PROCESSING
      ↓
PLASTICITY (STDP, Hebbian)
      ↓
NETWORK CHANGE
```

### Phase 3: World Interaction (Updated)
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT (Vision, Touch, Internal, Proprioception)
      ↓
AgentBrain.processSensoryInput()
      ↓
Brain.receiveSensoryInput()
      ↓
NEURAL ACTIVITY (LIF dynamics)
      ↓
SPIKE PROCESSING (event-driven)
      ↓
Motor Decoding → MotorCommand
      ↓
AgentBrain.decodeMotorCommand()
      ↓
WORLD.applyMotorCommand()
      ↓
ACTION RESULT (reward, success)
      ↓
Reward Prediction Error
      ↓
Neuromodulation (dopamine-like signal)
      ↓
Eligibility Trace Update
      ↓
PLASTICITY (STDP, Hebbian, Reward-Modulated)
      ↓
Development System (stage progression)
```

### Phase 4: Integrated Cognition
```
WORLD (SimpleWorld)
      ↓
SENSORY PERCEPT
      ↓
ATTENTION (integrated)
      ↓
SENSORY INPUT
      ↓
PREDICTION (integrated)
      ↓
WORKING MEMORY (integrated)
      ↓
EPISODIC MEMORY (integrated)
      ↓
CONCEPT FORMATION (integrated)
      ↓
PLANNING (integrated)
      ↓
SELF-MODEL (integrated)
      ↓
ACTION SELECTION (integrated)
      ↓
MOTOR OUTPUT
      ↓
WORLD.applyMotorCommand()
      ↓
REWARD / CONSEQUENCE
      ↓
SOCIAL OBSERVATION (if others present)
      ↓
LEARNING UPDATES
```

### Phase 6: Final Integration
```
AGENT → WORLD → SENSORY PERCEPT → AgentBrain.processSensoryInput() →
Brain.receiveSensoryInput() → Neural Processing → Memory Systems →
Prediction System → Neuromodulation → AgentBrain.decodeMotorCommand() →
WORLD.applyMotorCommand() → Reward → AgentBrain.applyRewardModulation()
```

## Eligibility Traces and Credit Assignment (Updated)

### Integration with Neuromodulation
- **Eligibility Traces**: Maintain synaptic tags during activity
- **Credit Assignment**: Bridge time gap between action and reward
- **Reward Modulation**: Convert traces to weight changes
- **Integration**: Part of main reward pathway in brain loop

## Developmental Plasticity (Updated)

### Developmental System Integration
- **Stages**: Initial → CriticalPeriod → Maturation → Adult
- **Plasticity Modifiers**: Affect all plasticity rules
- **Integration**: Updates structural plasticity rates
- **Function**: Guides developmental progression

## What NLM is NOT (Updated)

### Clarification
NLM IS:
1. A **computational exploration** of brain-inspired architecture
2. A **testbed** for developmental learning principles
3. A **research platform** for artificial neural systems
4. An **educational tool** for understanding neural computation
5. An **investigation** of whether cognition can emerge from spiking dynamics

### Phase 6 Focus
NLM Phase 6 specifically focuses on **integration over new features** - making existing disconnected systems work together as a coherent whole.

### Anti-Transformer Principle (Updated)
- **Preserved**: No transformer architectures
- **Preserved**: No LLM replacements
- **Preserved**: No pretrained knowledge
- **Preserved**: Neural computation based on individual neurons and synapses
- **Extended**: Now includes cognitive integration principles
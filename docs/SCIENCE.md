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

## Phase 4: Emerging Cognition

### Prediction Systems

#### Biological Inspiration
The free energy principle and predictive coding suggest that brains are fundamentally prediction machines. Hierarchical predictive models allow organisms to anticipate future states and minimize prediction errors.

#### Computational Implementation
- **NeuralPrediction**: Learns temporal sequences through repeated experience
- **ActionConsequencePrediction**: Associates actions with their effects
- **Multi-step Prediction**: Chains predictions to anticipate distant futures

#### Known Limitations
- No hierarchical generative model
- Limited temporal horizon
- No explicit uncertainty representation

### Working Memory

#### Biological Inspiration
Working memory in prefrontal cortex maintains information active for seconds to minutes through persistent neural activity and recurrent circuits.

#### Computational Implementation
- **NeuralWorkingMemory**: Persistent activity in recurrent circuits
- **Competition**: Winner-take-more dynamics for selective retention
- **Decay**: Activity decays when not reinforced

#### Known Limitations
- No complex binding mechanisms
- Limited capacity
- Simplified recurrent architecture

### Episodic Memory

#### Biological Inspiration
Hippocampus stores episodes with temporal, spatial, and contextual information. Pattern completion allows retrieval from partial cues.

#### Computational Implementation
- **NeuralEpisodicMemory**: Stores experiences as neural activity patterns
- **Pattern Completion**: Retrieves full episodes from partial cues
- **Replay**: Reactivates past patterns

#### Known Limitations
- No hippocampal architecture
- No pattern separation
- Limited consolidation mechanisms

### Concept Formation

#### Biological Inspiration
Cortex forms stable representations through repeated exposure. Similar experiences converge to share representations.

#### Computational Implementation
- **ConceptFormation**: Clusters similar patterns into stable prototypes
- **Hebbian Averaging**: Prototype moves toward new instances
- **Stability Measurement**: Tracks how consistent patterns are

#### Known Limitations
- No hierarchical concept structure
- No symbolic representation
- Limited abstraction

### Attention

#### Biological Inspiration
Attention involves competitive dynamics where stronger signals inhibit weaker ones. Both bottom-up salience and top-down goals influence selection.

#### Computational Implementation (NOT Transformer)
- **AttentionalSelection**: Lateral inhibition competition
- **Winner-Take-More**: Winners receive excitation, losers receive inhibition
- **Top-Down Bias**: Goals can influence selection
- **Bottom-Up Salience**: Stimulus strength affects selection

#### IMPORTANT: NOT Transformer Attention
NLM attention is NOT:
- Query-Key-Value matrices
- Softmax attention weights
- Attention heads
- Self-attention

NLM attention IS:
- Competitive dynamics
- Lateral inhibition
- Recurrent amplification
- Neural circuit mechanism

### Planning

#### Biological Inspiration
Prefrontal cortex uses learned models to simulate future outcomes and select actions that lead to goals.

#### Computational Implementation
- **NeuralPlanner**: Evaluates action sequences using predictions
- **Action Quality**: Learns which actions work in which states
- **Plan Adaptation**: Updates based on success/failure

#### Known Limitations
- Shallow planning depth
- No tree search
- Limited world model

### Self-Model

#### Biological Inspiration
The brain maintains a model of the body (body schema) and distinguishes self-generated from externally generated events (agency).

#### Computational Implementation
- **SelfModel**: Learns forward model of sensory consequences
- **Self-Generated Likeness**: Predicts whether change is self-caused
- **Action Quality**: Tracks which actions lead to rewards

#### Known Limitations
- No sophisticated body schema
- No self-recognition
- No explicit agency computation

### Social Learning

#### Biological Inspiration
Mirror neurons and social observation allow learning from others' actions and their consequences.

#### Computational Implementation
- **SocialLearning**: Observes other agents' actions
- **Imitation**: Can reproduce observed actions
- **Communication Signals**: Learns simple associated signals

#### Known Limitations
- No theory of mind
- No sophisticated imitation learning
- No cultural transmission

## The Learning Loop

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

### Phase 3: Environment Interaction
```
WORLD (2D environment with objects)
      ↓
SENSORY PERCEPT (vision, touch, internal)
      ↓
NEURAL ACTIVITY (LIF dynamics)
      ↓
MOTOR OUTPUT (decoded from brain)
      ↓
ACTION (movement, interaction)
      ↓
WORLD STATE CHANGE
      ↓
REWARD / CONSEQUENCE
      ↓
PREDICTION ERROR
      ↓
NEUROMODULATION (dopamine-like)
      ↓
ELIGIBILITY TRACES
      ↓
PLASTICITY UPDATE
      ↓
DEVELOPMENTAL CHANGE
      ↓
BEHAVIORAL CHANGE
```

### Phase 4: Emerging Cognition
```
WORLD
      ↓
ATTENTION (competitive selection)
      ↓
SENSORY INPUT
      ↓
PREDICTION (learn temporal relationships)
      ↓
WORKING MEMORY (maintain information)
      ↓
EPISODIC MEMORY (store experiences)
      ↓
CONCEPTS (discover patterns)
      ↓
PLANNING (simulate futures)
      ↓
SELF-MODEL (predict self)
      ↓
ACTION SELECTION
      ↓
MOTOR OUTPUT
      ↓
WORLD
      ↓
REWARD / CONSEQUENCE
      ↓
SOCIAL OBSERVATION (if others present)
      ↓
LEARNING UPDATES
```

## Eligibility Traces and Credit Assignment

### Biological Inspiration
In biological brains, synaptic changes must be attributed to the correct action even when the consequence occurs later. Eligibility traces allow synapses to be "tagged" and modified when reward arrives.

### Computational Implementation
- Synapses maintain an eligibility trace during activity
- When reward prediction error occurs, trace is converted to weight change
- Traces decay over time if no reward signal arrives

### Known Limitations
- Real synaptic tagging involves complex molecular mechanisms
- Timing requirements are simplified
- Only recent activity is credited

## Developmental Plasticity

### Biological Inspiration
Early development has higher plasticity ("critical periods") that decreases with age. This allows learning when young but provides stability later.

### Computational Implementation
- Developmental stages: Initial → CriticalPeriod → Maturation → Adult
- Plasticity rates decrease with age
- Synaptogenesis higher early, pruning higher later

## What NLM is NOT

NLM is NOT:
- A claim that we can simulate a human brain
- A model that accurately reproduces neural physiology
- A replacement for neuroscience research
- An intelligent system
- A transformer or LLM
- A pretrained AI system

NLM IS:
- A computational exploration of brain-inspired architecture
- A testbed for developmental learning hypotheses
- A research platform for artificial neural systems
- An educational tool for understanding neural computation
- An investigation of whether cognition can emerge from spiking dynamics

## Acknowledging Uncertainty

We acknowledge that:
1. Biological brains are far more complex than any current simulation
2. Our understanding of the brain is incomplete
3. Many approximations are necessary for computational tractability
4. Claims of "brain-like" behavior should be critically examined
5. Phase 4 does not claim human-like intelligence, consciousness, or genuine understanding
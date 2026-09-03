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

## Prediction and Error Signals

### Biological Inspiration
Brains are prediction machines. Predictive coding suggests that brains constantly predict sensory inputs and signals errors when predictions fail.

### Computational Implementation
- Simple next-state prediction
- Error signals drive learning

### Known Limitations
- Real predictive coding involves complex hierarchical processing
- Error signals propagate through multiple pathways
- Our implementation is a placeholder

## Memory Systems

### Biological Inspiration
Multiple memory systems in the brain:
- Working memory (prefrontal cortex)
- Episodic memory (hippocampus)
- Semantic memory (cortex)
- Procedural memory (basal ganglia, cerebellum)

### Computational Implementation
- Abstract storage systems with capacity limits
- Simple consolidation mechanisms

### Known Limitations
- Real memory systems involve complex neural circuits
- Hippocampal-cortical interactions not modeled
- Memory consolidation and retrieval are vastly more complex

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

This loop is inspired by biological learning but implemented in a simplified computational form.

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

NLM IS:
- A computational exploration of brain-inspired architecture
- A testbed for developmental learning hypotheses
- A research platform for artificial neural systems
- An educational tool for understanding neural computation

## Acknowledging Uncertainty

We acknowledge that:
1. Biological brains are far more complex than any current simulation
2. Our understanding of the brain is incomplete
3. Many approximations are necessary for computational tractability
4. Claims of "brain-like" behavior should be critically examined

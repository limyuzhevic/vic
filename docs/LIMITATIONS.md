# NLM Limitations

## Critical Acknowledgment

**NLM IS NOT A BIOLOGICAL BRAIN SIMULATION.**

This document explicitly lists what NLM cannot currently do and where it diverges from biological reality.

## Fundamental Limitations

### 1. Scale
- Human brain: ~86 billion neurons, ~100 trillion synapses
- NLM Phase 1 skeleton: ~1,000 neurons (placeholder)
- Even Phase 5 targets: Millions of neurons (far below biological scale)

### 2. Neural Dynamics
**Biological Reality:**
- Hodgkin-Huxley dynamics with sodium, potassium, calcium channels
- Detailed action potential shapes
- Dendritic processing
- Glial interactions

**NLM Implementation:**
- Simple leaky integrate-and-fire (Phase 2)
- No ion channel modeling
- No dendritic computation
- No glia

### 3. Synaptic Physiology
**Biological Reality:**
- Neurotransmitter release mechanisms
- Receptor types (AMPA, NMDA, GABA, etc.)
- Synaptic vesicles, calcium influx
- Short-term plasticity (facilitation/depression)

**NLM Implementation:**
- Simple weight changes (Phase 2)
- No neurotransmitter modeling
- No receptor-specific effects
- Basic short-term plasticity placeholder

### 4. Plasticity Mechanisms
**Biological Reality:**
- STDP with calcium-based eligibility traces
- Neuromodulator-gated plasticity
- Gene expression changes
- Protein synthesis-dependent long-term memory
- Synapse-specific plasticity rules

**NLM Implementation:**
- Simple STDP approximation
- Basic reward modulation
- No gene regulation
- No protein synthesis
- Generic plasticity rules

### 5. Anatomy
**Biological Reality:**
- Detailed cortical layers
- Thalamic connections
- Subcortical structures
- Blood-brain barrier
- Metabolic constraints

**NLM Implementation:**
- Abstract "regions" with no anatomical detail
- No thalamus, basal ganglia, hippocampus
- No metabolic modeling
- No realistic topology

### 6. Development
**Biological Reality:**
- Genetic programs
- Cell migration
- Axon guidance
- Activity-dependent refinement
- Critical periods with specific molecular mechanisms
- Experience-dependent plasticity

**NLM Implementation:**
- Simple synaptogenesis/pruning rates
- No genetic programs
- No cell migration
- Basic plasticity rate changes

### 7. Neuromodulation
**Biological Reality:**
- Multiple receptor types per neuromodulator
- Region-specific effects
- Tonic vs. phasic signaling
- Complex interactions between modulators
- Metabolic cost signaling

**NLM Implementation:**
- Simple concentration levels
- Generic plasticity modulation
- No receptor diversity
- No interactions

### 8. Sensory Processing
**Biological Reality:**
- Retina with photoreceptors
- Thalamic relay
- Cortical visual areas (V1, V2, V3, etc.)
- Auditory processing via cochlea, brainstem
- Somatosensory processing

**NLM Implementation:**
- Raw data vectors (placeholder)
- No retinotopic mapping
- No cortical hierarchy
- No real sensory organs

### 9. Motor Control
**Biological Reality:**
- Spinal cord circuits
- Muscle spindles
- Cortical motor areas
- Cerebellum for coordination
- Basal ganglia for action selection

**NLM Implementation:**
- Simple action enumeration
- No reflex circuits
- No proprioception
- No coordination

## Claims We Do NOT Make

NLM does NOT claim to:
1. Simulate a human brain
2. Reproduce consciousness
3. Match biological neural coding
4. Explain perception or cognition
5. Replace neuroscience research
6. Achieve artificial general intelligence

## What NLM IS

NLM is:
1. A computational exploration of developmental learning principles
2. A testbed for neural architecture hypotheses
3. An educational tool for understanding neural computation
4. A research platform with explicit approximations
5. A project that acknowledges its limitations honestly

## Importance of Acknowledgment

By explicitly listing limitations, we:
1. Prevent misleading claims about brain-like behavior
2. Encourage critical evaluation of results
3. Maintain scientific integrity
4. Focus on what the system CAN do rather than claiming what it cannot
5. Provide clear direction for future biological fidelity improvements

## Future Improvements

Future phases may increase biological fidelity for specific subsystems, but the fundamental gap between computation and biology will persist.

# NLM Experiments

## Overview

NLM uses experiments to evaluate its developmental learning capabilities. This document outlines the experimental framework, expected metrics, and Phase 6 integration experiments.

## Phase 6 Integration Experiments

### Phase 6.1: Full Brain Integration
Tests whether all brain systems work together coherently as a unified artificial brain. The brain receives sensory input, updates memory systems, applies neuromodulation, and produces output in a complete integration loop.

**Procedure:**
1. Initialize brain with all systems enabled (memory, prediction, attention, planning, self-model, development)
2. Run simulation in a simple world with sensory input
3. Measure system coordination and behavior emergence
4. Test persistence through save/load cycles
5. Evaluate replay and consolidation mechanisms

**Metrics:**
- Integration score (how well systems interact)
- Memory retention over time
- Prediction accuracy improvement
- Development progression
- Behavioral coherence
- State persistence quality

**Expected:** The brain should demonstrate coherent, integrated behavior where memory, cognition, and action work together seamlessly.

### Phase 6.2: Memory Consolidation
Tests whether episodic memory replay and consolidation properly maintain important experiences over time.

**Procedure:**
1. Run simulation with various experiences
2. Store episodes with reward and relevance scores
3. Trigger replay mechanisms
4. Test recall after delays
5. Measure memory strength retention

**Metrics:**
- Recall accuracy after delays
- Memory decay rate
- Consolidation effectiveness
- Replay frequency
- Memory prioritization

**Expected:** Important memories should be retained longer through replay and consolidation.

### Phase 6.3: Neuromodulation Integration
Tests whether dopamine, curiosity, novelty detection, and development properly coordinate learning.

**Procedure:**
1. Enable all neuromodulation systems
2. Run learning experiments in dynamic environment
3. Measure modulation effects on plasticity
4. Test exploration-exploitation balance
5. Validate developmental stage effects

**Metrics:**
- Dopamine modulation of learning rates
- Curiosity-driven exploration
- Novelty sensitivity
- Developmental plasticity effects
- Learning coherence

**Expected:** Neuromodulators should properly coordinate learning across multiple time scales.

### Phase 6.4: Cognitive Integration
Tests whether all cognitive systems (attention, concept formation, planning, self-model) work together.

**Procedure:**
1. Enable all cognitive systems
2. Present complex sensory input
3. Measure system coordination
4. Test emergence of goal-directed behavior
5. Evaluate self-aware actions

**Metrics:**
- Attention-selectivity balance
- Concept stability over time
- Planning accuracy
- Self-model coherence
- Behavioral integration

**Expected:** Cognitive systems should produce coherent, goal-directed behavior.

## Running Phase 6 Experiments

```bash
# Run Phase 6 integration demonstration
./nlm_phase6_demo [num_episodes] [steps_per_episode]

Example:
./nlm_phase6_demo 5 1000
```

## Experiment Configuration

Experiments can be configured via command line or config file:
```cpp
config->set("neuron_count", size_t(1000));
config->set("memory_capacity", size_t(500));
config->set("prediction_horizon", size_t(3));
config->set("working_memory_capacity", size_t(100));
config->set("enable_curiosity", true);
config->set("enable_novelty", true);
config->set("enable_development", true);
config->set("enable_planning", true);
config->set("enable_self_model", true);
```

## Standard Metrics

### Neural Activity
- Firing rate distribution
- Spike timing statistics
- Population activity levels
- Synchrony measures

### Memory
- Retention accuracy over time
- Recall success rate
- Pattern stability
- Generalization

### Cognition
- Integration score between systems
- Concept formation stability
- Planning success rate
- Self-prediction accuracy

### Learning
- Prediction error reduction
- Memory improvement over time
- Behavioral adaptation
- Developmental progression

### System Coordination
- Memory-system interaction strength
- Neuromodulation effects on learning
- Attention-cognition balance
- Action planning coherence

## Analysis Tools

The RepresentationAnalyzer class provides:
- Pattern similarity computation
- Representational stability
- Clustering measures
- Selectivity indices
- Temporal precision

## Anti-Cheating Measures

Phase 6 experiments are designed to prevent shortcuts:
- No predefined integration rules
- No hard-coded system coordination
- No symbolic lookups
- External experiment code measures, not solves

## Reproducibility

All experiments use:
- Explicit random seeds
- Version-controlled code
- Deterministic simulation
- Multiple seed runs for statistics
# NLM Experiments

## Overview

NLM uses experiments to evaluate its developmental learning capabilities. This document outlines the experimental framework, expected metrics, and Phase 4 cognitive experiments.

## Phase 3 Experiments

### Phase 3.1: World Interaction Demo
The basic demonstration of the complete loop:
```
NLM brain → actions → world changes → sensory input → reward → plasticity
```

Run with:
```bash
./nlm_phase3_demo 5 200
```

Expected outcomes:
- Brain receives visual input (16x16 grayscale)
- Motor activity produces actions
- Reward signals modify synapses
- Behavior changes across episodes

## Phase 4 Experiments

### Phase 4.1: Temporal Prediction Learning
Tests if NLM can learn that state A predicts state B.

**Procedure:**
1. Present pattern A repeatedly followed by pattern B
2. Measure prediction error over time
3. Track if prediction accuracy improves

**Metrics:**
- Prediction error (initial vs final)
- Improvement over trials
- Error history

**Expected:** Error should decrease as NLM learns the temporal relationship.

### Phase 4.2: Working Memory
Tests if information can be maintained after the stimulus disappears.

**Procedure:**
1. Present pattern A
2. Add delay (maintenance period)
3. Test if pattern information is still present

**Metrics:**
- Retention accuracy over time
- Capacity limits
- Decay rate

**Expected:** Working memory should show gradual decay, not abrupt forgetting.

### Phase 4.3: Episodic Recall
Tests if past experiences influence future behavior.

**Procedure:**
1. Store episodes with associated reward
2. After delay, retrieve similar episodes
3. Measure if retrieved information influences behavior

**Metrics:**
- Recall accuracy
- Number of episodes stored
- Influence on behavior

**Expected:** Episodes with high reward should be more retrievable.

### Phase 4.4: Concept Formation
Tests if recurring patterns form stable representations without predefined labels.

**Procedure:**
1. Present variations of the same underlying pattern
2. Track if representations become more stable
3. Measure generalization to new variations

**Metrics:**
- Number of concepts formed
- Stability over time
- Generalization ability

**Expected:** Similar patterns should converge to stable concept representations.

### Phase 4.5: Attention Selection
Tests if competing signals produce selective processing.

**Procedure:**
1. Present multiple competing stimuli
2. Measure which neural populations win
3. Track attention dynamics

**Metrics:**
- Selectivity index
- Winner consistency
- Response to distractors

**Expected:** Stronger stimuli should win competition.

### Phase 4.6: Multi-Step Planning
Tests if NLM can plan multi-step sequences to achieve goals.

**Procedure:**
1. Set goal state
2. NLM plans action sequence
3. Execute and measure success

**Metrics:**
- Planning accuracy
- Multi-step success rate
- Planning confidence

**Expected:** Planning should improve with experience.

### Phase 4.7: Self-Action Prediction
Tests if NLM learns that its actions cause predictable sensory changes.

**Procedure:**
1. NLM performs action
2. Record sensory change
3. Later, predict consequence before acting

**Metrics:**
- Self-prediction accuracy
- Body awareness index
- Self vs external distinction

**Expected:** Self-generated changes should be more predictable.

### Phase 4.8: Object Permanence
Tests if hidden objects remain relevant to behavior.

**Procedure:**
1. Object visible → agent approaches
2. Object hidden (behind barrier)
3. Agent continues to search for hidden object

**Metrics:**
- Search behavior after hiding
- Continued representation

**Expected:** Agent should maintain object representation.

### Phase 4.6: Neural Prediction
Tests if NLM can anticipate future states.

**Procedure:**
1. Present sensory input
2. Predict next state
3. Compare prediction with actual input
4. Learn from prediction error

**Metrics:**
- Prediction accuracy
- Prediction error reduction
- Learning curve

**Expected:** Prediction accuracy should improve with experience.

### Phase 4.7: Development
Tests if neural systems change over time.

**Procedure:**
1. Track developmental stages
2. Measure structural changes
3. Test plasticity modulation

**Metrics:**
- Development stage progression
- Structural stability
- Plasticity modulation effectiveness

**Expected:** Development should show progressive refinement.

### Phase 4.8: Social Learning
Tests if NLM can learn from observing another agent.

**Procedure:**
1. Another agent performs action
2. NLM observes action and consequence
3. NLM attempts to imitate later

**Metrics:**
- Imitation accuracy
- Observations needed
- Transfer to new situations

**Expected:** Socially learned actions should be imitatable.

### Phase 4.9: Neuromodulation
Tests if neuromodulators affect learning and behavior.

**Procedure:**
1. Present reward prediction errors
2. Measure neuromodulator response
3. Test modulation of learning

**Metrics:**
- Neuromodulator levels
- Learning modulation strength
- Behavioral adjustment

**Expected:** Neuromodulation should enhance adaptive behavior.

### Phase 4.10: Continual Learning
Tests if NLM can learn new tasks without forgetting old ones.

**Procedure:**
1. Learn Task A to criterion
2. Learn Task B
3. Test performance on Task A again

**Metrics:**
- Performance on each task over time
- Forgetting rate
- Transfer between tasks

**Expected:** Some forgetting is normal, but knowledge should transfer.

### Phase 4.11: Generalization
Tests if learned knowledge transfers to new situations.

**Procedure:**
1. Learn concept in Context A
2. Test concept in Context B (with variations)
3. Measure transfer success

**Metrics:**
- Generalization ratio
- Variation tolerance
- Concept stability

**Expected:** Well-formed concepts should generalize.

### Phase 4.12: Integrated Experiment
Combines all Phase 4 mechanisms in a complex environment.

**Procedure:**
1. Run multiple episodes with all systems active
2. Track all metrics over time
3. Compare early vs late performance

**Metrics:**
- Total reward
- All individual mechanism metrics
- Behavior improvement

**Expected:** Performance should improve across episodes as all systems integrate.

## Running Phase 4 Experiments

```bash
./nlm_phase4_demo [num_trials]
```

Example:
```bash
./nlm_phase4_demo 100
```

## Experiment Configuration

Experiments can be configured via command line or config file:
```cpp
config->set("neuron_count", size_t(1000));
config->set("prediction_horizon", size_t(3));
config->set("working_memory_capacity", size_t(100));
```

## Standard Metrics

### Neural Activity
- Firing rate distribution
- Spike timing statistics
- Population activity levels
- Synchrony measures

### Memory
- Retention accuracy
- Recall success rate
- Pattern stability
- Generalization

### Learning
- Prediction error over time
- Concept formation rate
- Planning success
- Self-prediction accuracy

### Social
- Imitation success
- Observation efficiency
- Communication signal learning
- Reinforcement learning effectiveness

## Running Phase 4 Experiments

```bash
./nlm_phase4_demo [num_trials]
```

Example:
```bash
./nlm_phase4_demo 100
```

## Experiment Configuration

Experiments can be configured via command line or config file:
```cpp
config->set("neuron_count", size_t(1000));
config->set("prediction_horizon", size_t(3));
config->set("working_memory_capacity", size_t(100));
```

## Standard Metrics

### Neural Activity
- Firing rate distribution
- Spike timing statistics
- Population activity levels
- Synchrony measures

### Memory
- Retention accuracy
- Recall success rate
- Pattern stability
- Generalization

### Learning
- Prediction error over time
- Concept formation rate
- Planning success
- Self-prediction accuracy

### Social
- Imitation success
- Observation efficiency
- Communication signal learning

## Analysis Tools

The RepresentationAnalyzer class provides:
- Pattern similarity computation
- Representational stability
- Clustering measures
- Selectivity indices
- Temporal precision

## Anti-Cheating Measures

Phase 4 experiments are designed to prevent shortcuts:
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
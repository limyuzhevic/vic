# NLM Experiments

## Overview

NLM uses experiments to evaluate its developmental learning capabilities. This document outlines the experimental framework and expected metrics.

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

### Phase 3.2: Learning Metrics
Track learning across multiple episodes:
- Total reward per episode
- Action distribution (forward, turn, interact, wait)
- Synapse count changes
- Novelty exposure over time

### Phase 3.3: Ablation Studies
Test which components are essential:
- With vs without reward modulation
- With vs without structural plasticity
- With vs without development
- With vs without curiosity

## Experiment Types

### 1. Developmental Experiments
Track how NLM's neural architecture changes over time:
- Synapse count over simulation steps
- Weight distribution evolution
- Activity patterns
- Structural changes (synaptogenesis/pruning)

### 2. Learning Experiments
Evaluate learning of specific tasks:
- Navigation (grid world)
- Pattern recognition
- Sequence learning
- Association formation

### 3. Memory Experiments
Test memory systems:
- Working memory capacity
- Episodic recall
- Semantic knowledge acquisition
- Procedural skill learning

### 4. Neuromodulation Experiments
Study effects of neuromodulatory signals:
- Reward learning
- Curiosity-driven exploration
- Attention effects

## Standard Metrics

### Neural Activity
- Firing rate distribution
- Spike timing statistics
- Population activity levels
- Synchrony measures

### Connectivity
- Synapse count
- Weight distribution
- Connection density
- Criticality measures

### Learning
- Task performance over time
- Prediction accuracy
- Reward accumulation
- Error rates

### Development
- Synaptogenesis rate
- Pruning rate
- Critical period timing
- Plasticity changes

## Experiment Configuration

Experiments will be configured via:
```cpp
Experiment exp("Experiment_Name");
exp.setSeed(42);
exp.setConfig("neuron_count", 1000);
exp.setConfig("plasticity_rate", 0.01);
```

## Data Recording

Each experiment records:
- Random seed
- Configuration parameters
- Neural state snapshots
- Synaptic weights
- Activity statistics
- Performance metrics
- Developmental changes

## Analysis

Metrics will be computed and reported:
- Mean, std dev, min, max
- Time series plots
- Distribution comparisons
- Statistical significance tests

## Reproducibility

All experiments use explicit random seeds and version-controlled code for reproducibility.

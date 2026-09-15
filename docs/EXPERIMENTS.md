# NLM Experiments

## Overview

NLM uses experiments to evaluate its developmental learning capabilities. This document outlines the experimental framework, expected metrics, and Phase 4 cognitive experiments. **Phase 6 experiments focus on integration testing** to verify that all existing systems work together as a coherent brain.

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

## Phase 6 Experiments

**Note:** Phase 6 experiments focus on **integration testing** - verifying that all existing systems work together as a coherent brain. This is different from Phase 4 which established cognitive capabilities.

### Phase 6.1: Memory System Integration
Tests working memory connection to sensory processing, episodic memory integration with experience logging, and associative memory linking to episodic memory.

### Phase 6.2: Neuromodulation Integration
Tests dopamine effects on plasticity coordination, novelty-curiosity integration, and prediction error integration with neuromodulation.

### Phase 6.3: Prediction Integration
Tests prediction system connection to sensory processing, prediction error signals affect learning, and prediction influence on attention and planning.

### Phase 6.4: Full Brain Loop Integration
Comprehensive integration test to verify all systems work together as unified brain, with developmental effects on integrated systems.

### Phase 6.5: Sleep/Rest Integration
Tests memory replay during offline periods, consolidation mechanisms, and replay effects on future behavior.
# NLM Phase 6 Artificial Brain Documentation

## Overview

NLM (熙然, "serene flow") is an experimental artificial developmental brain system implementing a complete integrated brain architecture with all major neural systems working together.

Phase 6 represents the final integration stage where previously separate components are unified into a coherent artificial brain capable of:
- Learning from experience
- Maintaining memories and knowledge
- Making predictions about the future
- Modulating behavior through neuromodulation
- Developing and adapting over time
- Saving and loading brain states

## System Architecture

The Phase 6 brain implements a closed-loop architecture where all systems interact:

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE → MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION → ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE → PLASTICITY → MEMORY/DEVELOPMENT → CHANGED BRAIN
```

### Core Components

1. **Neural Processing**: LIF dynamics, spike propagation, neural populations
2. **Memory Systems**: Working memory, episodic memory, associative memory
3. **Neuromodulation**: Dopamine (reward), Curiosity, Novelty
4. **Prediction**: Predicts future states and computes prediction errors
5. **Cognition**: Neural planning, concept formation, attention
6. **Development**: Age-dependent plasticity and maturation
7. **Persistence**: Checkpoint save/load system
8. **Replay**: Memory replay for consolidation

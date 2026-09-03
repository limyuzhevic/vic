# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 4: EMERGING COGNITION**

This phase implements mechanisms for increasingly complex cognitive behavior to emerge:

### Prediction
- Temporal sequence learning
- Action-consequence prediction
- Multi-step prediction
- Prediction error signals

### Memory
- Working memory (persistent neural activity)
- Episodic-like memory (experience encoding)
- Associative memory (Hebbian associations)
- Semantic-like memory (generalized knowledge)

### Concept Formation
- Discovers patterns from experience
- No predefined labels or categories
- Stability measurement
- Generalization ability

### Attention
- Competition-based selection
- NOT Transformer attention (no QKV matrices)
- Top-down and bottom-up influence
- Winner-take-more dynamics

### Planning
- Uses learned predictions
- Multi-step action sequences
- Goal-directed behavior
- Adaptive based on outcomes

### Self-Model
- Sensorimotor self-awareness
- Body schema learning
- Self vs external distinction
- Action consequence prediction

### Social Learning
- Observation of others
- Imitation capability
- Simple communication signals
- Social prediction

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 4 Demo
```bash
./nlm_phase4_demo [num_trials]
```

For example:
```bash
./nlm_phase4_demo 100
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   └── LIMITATIONS.md
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/     # Developmental system
│   ├── neuromodulation/# Neuromodulators
│   ├── memory/         # Memory systems (Phase 4)
│   ├── prediction/      # Prediction systems (Phase 4)
│   ├── cognition/      # Cognitive mechanisms (Phase 4)
│   ├── sensory/        # Sensory processing
│   ├── motor/          # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework
│   └── visualization/  # Visualization
├── tests/
├── configs/
└── data/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Current)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 4 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 4 DOES investigate:
- Whether prediction can emerge from spiking dynamics
- Whether memory can be grounded in neural activity
- Whether concepts can form without predefined labels
- Whether planning can arise from predictive mechanisms
- Whether self-models can develop through sensorimotor learning
- Whether social learning can emerge from observation

## License

MIT

## Authors

Research project — See docs for scientific background.
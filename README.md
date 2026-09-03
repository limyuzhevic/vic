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

## Current Phase

**PHASE 3: WORLD INTERACTION + DEVELOPMENT**

This phase places the neural brain inside a real interactive environment:
- Simple 2D world with objects (resources and hazards)
- Visual sensory pathway (16x16 grayscale)
- Motor actions decoded from neural activity
- Reward prediction error system
- Eligibility traces for credit assignment
- Developmental stages affecting plasticity
- Novelty and curiosity mechanisms
- Homeostatic regulation

Phase 2 completed:
- Real LIF neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP (Spike-Timing-Dependent Plasticity)
- Hebbian learning
- Structural plasticity (synaptogenesis and pruning)

Phase 3 adds:
- Environment interaction loop
- Sensory input from world to brain
- Motor output from brain to world
- Consequence/reward signals
- Reward-modulated plasticity
- Developmental progression

Current status:
- ✅ Project compiles with C++20
- ✅ Real LIF neuron dynamics
- ✅ Event-driven spike propagation with delays
- ✅ STDP plasticity rule
- ✅ Hebbian plasticity rule
- ✅ Structural plasticity
- ✅ Learning experiments with measurable weight changes
- ✅ Unit tests for core mechanisms
- ✅ Clean architecture with interfaces
- ✅ Core types defined
- ✅ Random generator with deterministic seed
- ✅ Simulation clock
- ✅ Brain with regions and populations
- ✅ Configuration system
- ✅ Logging system

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 2 Neural Computation Demo
```bash
./nlm
```

### Phase 3 World Interaction Demo
```bash
./nlm_phase3_demo [num_episodes] [steps_per_episode]
```

For example:
```bash
./nlm_phase3_demo 5 200
```

With custom configuration:
```bash
./nlm --config=../configs/custom.cfg
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
│   ├── memory/         # Memory systems
│   ├── sensory/        # Sensory processing
│   ├── prediction/     # Prediction system
│   ├── motor/          # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework
│   └── visualization/  # Visualization
├── tests/
├── configs/
└── data/
```

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness or intelligence
- Limited to what can be simulated with available computing resources

## License

MIT

## Authors

Research project — See docs for scientific background.

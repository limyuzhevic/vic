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

**PHASE 2: REAL NEURAL COMPUTATION**

This phase implements real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP (Spike-Timing-Dependent Plasticity)
- Hebbian learning
- Structural plasticity (synaptogenesis and pruning)

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

```bash
./nlm
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

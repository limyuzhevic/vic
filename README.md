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

**PHASE 1: SKELETON**

This repository contains the complete architectural skeleton for NLM. The actual neural computation (real spiking dynamics, synaptic plasticity, etc.) will be implemented in Phase 2 and beyond.

Current status:
- ✅ Project compiles with C++20
- ✅ Clean architecture with interfaces
- ✅ Core types defined
- ✅ Random generator with deterministic seed
- ✅ Simulation clock
- ✅ Brain skeleton with regions and populations
- ✅ Neuron and Synapse interfaces
- ✅ Environment interface
- ✅ Experiment framework
- ✅ Configuration system
- ✅ Logging system
- ❌ Real neural computation (Phase 2+)

## Building

```bash
mkdir build
cd build
cmake ..
make
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

- No realistic ion channel dynamics
- No detailed cortical architecture
- No claim of consciousness or intelligence
- Limited to what can be simulated with available computing resources

## License

MIT

## Authors

Research project — See docs for scientific background.

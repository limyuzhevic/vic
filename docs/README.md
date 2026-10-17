# NLM Project Documentation

## Overview

NLM (Neural Learning Machine) is a biologically-inspired computational platform that implements spiking neural networks based on fundamental neuroscience principles. Unlike traditional deep learning frameworks, NLM implements actual neurons and synapses with realistic dynamics, enabling the study of emergent cognitive abilities through developmental processes.

### Key Features

- **Biologically Plausible**: Implements LIF neuron dynamics, STDP plasticity, and realistic synaptic transmission
- **Modular Architecture**: Clear separation of neural computation, neuromodulation, memory, and cognitive layers
- **Developmental System**: Neuronal development through synaptogenesis, pruning, and maturation
- **Multi-Scale Capabilities**: From micro-networks for research to large-scale simulations for production
- **Extensible Design**: Easy to add new neuron models, plasticity rules, and cognitive mechanisms
- **Deterministic Simulation**: Reproducible experiments through explicit random seeds
- **High-Performance**: Event-driven processing, sparse connectivity, and SIMD optimization for large-scale simulations

## Quick Start

### Prerequisites

- C++20 compatible compiler
- CMake 3.16 or higher
- Python 3.8 or higher (for Python bindings)

### Basic Installation

```bash
# Clone the repository
cd /workspace/app

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build . -j$(nproc)

# Install the Python module (optional)
cd ..
pip install -e python
```

### Running a Simple Simulation

```bash
# Run the basic demo
./build/nlm --neuron-count=1000 --simulation-timestep=0.01 --max-steps=1000

# Run with visualization
./build/nlm --neuron-count=500 --max-steps=5000 --enable-checkpointing=true

# Run performance benchmark
./build/nlm --benchmark --neurons=10000 --steps=10000
```

### Python API Quick Start

```python
import pynlm

# Create advanced brain configuration
brain_config = pynlm.createAdvancedBrainConfig({
    'neuron_count': 2000,
    'simulation_timestep': 0.001,
    'enable_plasticity': True,
    'enable_neuromodulation': True,
    'enable_curiosity': True,
    'enable_novelty': True,
    'enable_prediction_error': True
})

# Create brain and agent
brain = pynlm.createBrain(brain_config)
agent_brain = pynlm.createAgentBrain(brain)

# Create advanced agent with reinforcement learning
advanced_agent = pynlm.createAdvancedAgent(agent_brain, {
    'algorithm': 'DEEP_Q_LEARNING',
    'learning_rate': 0.001,
    'discount_factor': 0.99,
    'replay_buffer_size': 10000
})

# Start learning
advanced_agent.startLearning()

# Run simulation
for episode in range(10):
    for step in range(100):
        # Your simulation code here
        pass
```

## Project Status

NLM is currently in **Phase 5 - Scaling + Research Platform** of development:

- **Core Architecture**: Complete (Phase 1-4)
- **Neural Computation**: Working spiking dynamics (Phase 2)
- **World Interaction**: Complete environment integration (Phase 3)
- **Emerging Cognition**: Full cognitive suite implemented (Phase 4)
- **Scaling Infrastructure**: Performance optimizations in progress (Phase 5)

## Key Innovation Areas

### 1. Anti-Transformer Architecture
NLM deliberately avoids transformer-based approaches, focusing instead on:
- Individual neuron dynamics (not artificial attention)
- Spike-based communication (not token processing)
- Biological synaptic plasticity (not gradient descent)
- Developmental learning (not pretrained knowledge)

### 2. Multi-Layer Cognitive Architecture
```
Sensory Input → Brain Core → Neuromodulation → Memory Systems
                        ↓              ↓              ↓
                   Prediction Systems → Cognitive Systems → Motor Output
```

### 3. Phase-Based Development
- **Phase 1**: Skeleton with clear interfaces
- **Phase 2**: Working neural core with real dynamics
- **Phase 3**: World interaction and development
- **Phase 4**: Emerging cognition (prediction, memory, attention, planning)
- **Phase 5**: Scaling + Research platform

## Getting Help

### Documentation
- This documentation set: `/workspace/app/docs/`
- Python API: `/workspace/app/python/README.md`
- Examples: `/workspace/app/python/examples/advanced_examples.py`

### Testing
- Run tests: `bash test_advanced_api.sh`
- Run benchmarks: `./build/nlm --benchmark`

### Support
For questions and support, refer to the main NLM documentation or run `./build/nlm --help` for command-line options.

## Next Steps

1. **Read** `/workspace/app/docs/ARCHITECTURE.md` for detailed system design
2. **Run** basic examples to understand the workflow
3. **Explore** `/workspace/app/docs/PERFORMANCE.md` for scaling strategies
4. **Customize** configuration for your specific use case
5. **Contribute** to the research platform (see `CONTRIBUTING.md`)
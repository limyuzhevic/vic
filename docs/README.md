# NLM - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## What is NLM?

NLM is a neural system that:
- Learns from experience through neural plasticity
- Develops and adapts over time
- Emerges cognition from neural dynamics
- Integrates memory, prediction, and neuromodulation systems

## Key Features

### Core Architecture
- **Real-time spiking neural computation** using LIF (Leaky Integrate-and-Fire) neurons
- **Event-driven dynamics** with millisecond precision
- **Integrated memory systems**: working memory, episodic memory, associative memory
- **Prediction systems** for sensory anticipation
- **Cognition modules**: attention, planning, concept formation
- **Neuromodulation**: dopamine, curiosity, novelty detection
- **Developmental system** that modulates plasticity over time

### Learning Mechanisms
- **STDP** (Spike-Timing-Dependent Plasticity) for precise timing-based learning
- **Hebbian learning** for correlation-based strengthening
- **Structural plasticity** for creating and pruning connections
- **Developmental stages** that affect learning rates
- **Reward-based learning** through neuromodulation

### World Interaction
- **Sensory processing** with vision, touch, proprioception, and internal sensors
- **Motor control** through action selection
- **Environmental feedback** and learning from consequences
- **Curiosity-driven exploration** for autonomous discovery

## Getting Started

### Quick Start
See [QUICK_START.md](QUICK_START.md) for a 5-minute introduction.

### Installation
See [tutorials/01_installation.md](tutorials/01_installation.md) for detailed setup instructions.

### Examples
Try these working examples:
- [examples/01_basic-brain.md](examples/01_basic-brain.md) - Basic brain simulation
- [examples/02_brain-in-world.md](examples/02_brain-in-world.md) - Agent-environment interaction
- [examples/03_learning-agent.md](examples/03_learning-agent.md) - Reward-based learning
- [examples/04_complete-agent.md](examples/04_complete-agent.md) - Complete simulation

### Core Concepts
Understand the architecture in [CORE_CONCEPTS.md](CORE_CONCEPTS.md).

### API Reference
See [API_REFERENCE.md](API_REFERENCE.md) for complete class and method documentation.

## Building

### Requirements
- C++17 compiler
- CMake 3.10 or higher
- SSE/AVX instructions (for performance)

### Build Instructions
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running

#### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── src/
│   ├── core/           # Core utilities and types
│   ├── brain/          # Neural components (LIF neurons, synapses, regions)
│   ├── agent/          # Agent-brain interface
│   ├── memory/         # Working, episodic, and associative memory
│   ├── prediction/     # Prediction systems for anticipation
│   ├── cognition/      # Attention, planning, concept formation
│   ├── neuromodulation/ # Dopamine, curiosity, novelty
│   ├── development/    # Developmental stages and plasticity modulation
│   ├── dynamics/       # Spike systems and neural dynamics
│   ├── plasticity/     # STDP, Hebbian, structural plasticity
│   ├── sensory/       # Vision, touch, proprioception, internal sensors
│   ├── motor/         # Action selection and motor commands
│   ├── environment/   # World interface and interaction
│   ├── world/         # Environment simulation
│   └── visualization/  # Visualization tools
├── docs/               # Documentation (this section)
│   ├── README.md              # Project overview
│   ├── QUICK_START.md         # 5-minute getting started
│   ├── CORE_CONCEPTS.md        # Architecture and concepts
│   ├── API_REFERENCE.md       # Complete API documentation
│   ├── examples/              # Working examples
│   ├── tutorials/             # Step-by-step tutorials
│   ├── best-practices/         # Quality guidelines
│   └── troubleshooting/       # Common issues and solutions
├── tests/               # Unit and integration tests
└── configs/             # Configuration files
```

## Phases

### Phase 1: Project Foundation (Complete)
- Project skeleton and core types
- Neural interfaces

### Phase 2: Neural Core (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3: World Interaction (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4: Emerging Cognition (Complete)
- Neural prediction system
- Working memory
- Episodic memory
- Concept formation
- Neural attention
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5: Performance Optimizations (Complete)
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Final Integration (Complete - Working)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

## Scientific Background

NLM is inspired by biological brains but makes no claims of being an accurate simulation. Key scientific inspiration includes:

- **Synaptic Plasticity**: Based on Hebbian learning and STDP
- **Neural Dynamics**: Integrate-and-fire neuron models
- **Memory Systems**: Working memory and episodic memory concepts
- **Predictive Coding**: Brain as prediction machine
- **Neuromodulation**: Dopamine, curiosity, and novelty detection

## Limitations

NLM is a research project investigating computational brain-like systems. Current limitations include:
- Simplified LIF neuron model
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No realistic cortical architecture
- No claim of consciousness, intelligence, or human-like cognition

## License

MIT

## Authors

Research project — See docs for scientific background.
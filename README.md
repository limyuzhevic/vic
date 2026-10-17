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

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Running Tests

```bash
# Run unit tests
./nlm_test

# Run all demo programs
./nlm
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/                     # Source code
│   ├── core/               # Core utilities
│   ├── brain/              # Neural components (integrated)
│   ├── dynamics/           # Neural dynamics
│   ├── plasticity/         # Plasticity rules
│   ├── development/        # Developmental system
│   ├── neuromodulation/    # Neuromodulators (integrated)
│   ├── memory/            # Memory systems (integrated)
│   ├── prediction/         # Prediction systems (integrated)
│   ├── cognition/          # Cognitive mechanisms (integrated)
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   ├── experiments/        # Experiment framework (Phase 6)
│   └── visualization/      # Visualization
├── tests/
└── configs/
```

## Key Features

### Neural Core

- **1000+ Leaky Integrate-and-Fire (LIF) neurons** with realistic membrane dynamics
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity mechanisms**: STDP, Hebbian, and structural plasticity

### Brain Systems

- **Integrated memory systems**: Working, episodic, associative memory
- **Advanced neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction and planning**: Internal models and action selection
- **Cognition and attention**: Concept formation and competitive processing

### Developmental Biology

- **Age-dependent plasticity**: Developmental stages affecting learning rates
- **Structural maturation**: Synapse formation and pruning
- **Maturation processes**: Neural excitability changes over time

### Advanced Capabilities

- **Checkpoint persistence**: Save and load brain states
- **Memory replay**: Consolidation during simulated sleep
- **Environmental interaction**: Complex sensory-motor loops
- **Learning and adaptation**: Experience-dependent changes

## Getting Started

### Quick Start (Phase 6 Integration Test)

```bash
# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the comprehensive integration test
./nlm_phase6_demo
```

This demo verifies that all Phase 6 integration components are working correctly.

### Python API

If you prefer using Python for experimentation:

```bash
# Install the Python bindings
pip install .

# Run Python examples
python examples/python_example.py
```

### Configuration

The brain behavior is controlled through configuration files in the `configs/` directory:

```bash
# Use custom configuration
./nlm --config my_config.cfg

# Override configuration parameters
./nlm --config=custom.cfg --neuron_count=2000 --connection_probability=0.05
```

## Scientific Background

NLM implements a brain-inspired computational framework based on several biological principles:

1. **Neuronal Coding**: Spiking neurons with realistic membrane dynamics
2. **Synaptic Plasticity**: Multiple learning rules including spike-timing-dependent plasticity (STDP)
3. **Neuromodulation**: Dopamine for reward prediction, curiosity for exploration
4. **Memory Systems**: Working memory, episodic memory, and associative memory
5. **Developmental Processes**: Age-dependent changes in plasticity and connectivity

The architecture is inspired by biological brains, particularly the mammalian neocortex, but implemented as an experimental computational model for studying artificial developmental processes.

## Research Applications

NLM is suitable for research in:

- **Artificial Development**: Studying how systems develop complex capabilities from simple beginnings
- **Neural Computation**: Investigating spiking neural network dynamics
- **Learning and Adaptation**: Understanding experience-dependent changes in neural systems
- **Cognitive Architecture**: Exploring how multiple brain systems interact
- **Neuroinformatics**: Modeling biological brain processes computationally

## Best Practices for Users

### For Beginners

1. **Start with the default configuration**: Use the existing `configs/default.cfg` file to run the basic simulation without modifications.

2. **Follow the documented workflow**: The `easy_usage.md` guide provides a step-by-step walkthrough of the typical NLM usage pattern.

3. **Understand the brain lifecycle**: Always follow this sequence:
   ```python
   brain = pynlm.createBrain(config)
   brain.initialize()
   brain.step(step_number)  # Run simulation
   # Remember to reset between experiments!
   brain.reset()
   ```

4. **Monitor performance metrics**: Track `brain.getFiringNeuronCount()`, `brain.getTotalSpikeCount()`, and `brain.getAverageFiringRate()` to ensure your simulation is running correctly.

### For Intermediate Users

1. **Experiment with configuration parameters**: Modify the config file to explore different brain behaviors:
   - `neuron_count`: Number of neurons affects processing complexity
   - `connection_probability`: Controls network density
   - `stdp_ltp_weight`, `stdp_ltd_weight`: Control plasticity strength

2. **Enable advanced features**: Use the agent brain interface to enable:
   ```python
   agent.enableRewardModulation(True)      # Learn from rewards
   agent.enableCuriosity(True)            # Explore novel experiences
   agent.enableStructuralPlasticity(True)  # Grow new connections
   agent.enableDevelopment(True)           # Brain matures over time
   ```

3. **Create custom world scenarios**: Design specific environments for your research questions using the world configuration options.

### For Advanced Users

1. **Use research-grade configurations**: For advanced experiments, use `createResearchConfig()` for precise parameter control.

2. **Record and analyze neural activity**: Use `recordNeuralActivity()` to capture detailed neural dynamics for analysis.

3. **Evaluate performance metrics**: Use `calculatePerformanceMetrics()` to get comprehensive evaluation of agent behavior.

4. **Implement custom reward functions**: Override default reward systems with domain-specific metrics.

5. **Conduct long-term experiments**: Use checkpoint functionality to save and restore brain states for longitudinal studies.

## Future Development Directions (Phase 7)

Based on current research questions and user feedback, potential future enhancements include:

1. **Enhanced Neuromodulation**: More sophisticated dopamine and curiosity models
2. **Advanced Memory Systems**: Episodic memory with temporal context
3. **Social Learning**: Multi-agent interactions and social dynamics
4. **Language Integration**: Symbolic processing alongside neural computation
5. **Embodied Cognition**: More complex sensorimotor loops
6. **Meta-learning**: Learning how to learn more efficiently
7. **Resource Constraints**: Energy and resource-limited architectures

## License

MIT

## Authors

Research project — See docs for scientific background.
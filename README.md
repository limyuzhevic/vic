# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project implementing a full neural system with:

- **7,500+ neurons** and **250,000+ synapses**
- **12 major brain systems** working in integration
- **Spike-based neural dynamics** with event-driven computation
- **Memory systems** with persistent activity and replay
- **Neuromodulation** for learning and motivation
- **Developmental stages** affecting learning rates
- **Predictive processing** for forward models
- **Cognitive capabilities** emerging from neural dynamics

## Key Differentiators

NLM is fundamentally different from mainstream AI approaches:

### What NLM IS:
- A **spiking neural network** with realistic LIF dynamics
- A **closed-loop system** interacting with real environments
- A **developmental system** that matures over time
- An **emergent cognition system** where complex behaviors arise from neural interactions
- A **predictive brain** with forward models and prediction error signals
- A **memory-integrated system** where memory and perception are unified

### What NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system
- A statistical pattern matcher

## Current Phase: Phase 6 - Final Integration

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

## System Architecture

### Core Computational Components

#### **1. Neural Core**
- **7,500+ leaky integrate-and-fire (LIF) neurons**
- **Event-driven spike propagation** with 1-5ms synaptic delays
- **3,000+ synapses** with excitatory/inhibitory balance
- **Real-time neural dynamics** with refractory periods
- **Sensory and motor neuron populations** for environmental interaction

#### **2. Memory Systems (Unified)
- **Working Memory**: Persistent activity with competitive selection
  - 256 neurons in vision, 64 in touch, 32 in internal, 32 in proprioception
  - **Neural reverberation** for information maintenance
  - **Competitive dynamics** for selective attention
  - **Activity-dependent plasticity** for memory strengthening

- **Episodic Memory**: Experience encoding with replay
  - **Neural pattern storage** of sensory states
  - **Temporal ordering** of experiences
  - **Replay consolidation** during rest periods
  - **Relevance-weighted retrieval** based on reward

- **Associative Memory**: Pattern associations
  - **Hebbian synaptic connections** between experiences
  - **Spread activation** for associative recall
  - **Pattern completion** from partial cues
  - **Outcome-based strengthening**

#### **3. Neuromodulation Systems
- **Dopamine**: Reward prediction error signaling
  - **Phasic signals**: RPEs for learning
  - **Tonic signals**: Learning rate modulation
  - **Synaptic eligibility traces**: Credit assignment
  - **STDP weight scaling**: Neuromodulatory plasticity

- **Curiosity**: Exploration motivation
  - **Novelty-prediction error combination**
  - **Exploration drive**: Balances exploitation vs. exploration
  - **Attention modulation**: Prioritizes novel information
  - **Learning scheduling**: Controls when to explore vs. exploit

- **Novelty**: Change detection
  - **Sensory pattern comparison**: Detects sensory changes
  - **Decay-based tracking**: Maintains novelty memory
  - **Threshold-based detection**: Identifies significant changes
  - **Integration with curiosity**: Drives exploration behavior

#### **4. Prediction Systems
- **Forward Models**: Predict sensory consequences of actions
  - **Hierarchical temporal processing**: Multiple timescales
  - **Confidence estimation**: Uncertainty quantification
  - **Prediction error signals**: Drive learning and perception
  - **Integration with memory**: Prediction-error-driven learning

#### **5. Cognitive Systems
- **Neural Planner**: Action sequence evaluation
  - **Monte Carlo tree search** in neural space
  - **Value-based action selection**
  - **Plan evaluation and comparison**
  - **Action execution monitoring**

- **Concept Formation**: Pattern discovery without labels
  - **Unlabeled category learning**: Emerges from experience
  - **Feature abstraction**: Higher-level representations
  - **Invariant pattern extraction**: Common across experiences
  - **Knowledge distillation**: From experiences to concepts

- **Attention**: Competitive selection (NOT Transformer)
  - **Lateral inhibition**: Competitive dynamics between populations
  - **Gain modulation**: Attention-based signal amplification
  - **Resource allocation**: Prioritizes important information
  - **Spatial/temporal selection**: Focus on relevant dimensions

#### **6. Development Systems
- **Developmental Stages**: Age-dependent plasticity
  - **Initial**: High plasticity, rapid learning
  - **Critical Period**: Sensitive periods for skill acquisition
  - **Maturation**: Specialization and refinement
  - **Adult**: Stable function, fine-tuning
  - **Aging**: Declining plasticity, wisdom accumulation

- **Structural Plasticity**: Synaptogenesis and pruning
  - **Activity-dependent synapse formation**: Use-dependent growth
  - **Competitive pruning**: Eliminate weak connections
  - **Homeostatic regulation**: Maintain network stability
  - **Developmental timing**: Age-dependent rates

#### **7. Integration and Coordination
- **Neuromodulator Manager**: System-wide coordination
  - **Unified neuromodulation interface**: Single point of control
  - **Cross-system integration**: Ensures coordinated behavior
  - **Adaptive modulation**: Adjusts to environmental demands
  - **Resource allocation**: Distributes limited processing capacity

## Computational Scale and Performance

### Network Scale
- **Neurons**: ~7,500 (scales with simulation time)
- **Synapses**: ~250,000 (dense local connectivity)
- **Memory capacity**: ~10,000+ episodic entries
- **Working memory**: ~384 neurons across sensory modalities

### Performance Characteristics
- **Event-driven computation**: Efficient for sparse spiking activity
- **Parallel processing**: Multiple populations processed simultaneously
- **Memory pooling**: Optimized memory access patterns
- **Vectorization**: SIMD instructions for performance
- **Event queues**: Efficient spike event handling

### Simulation Speed
- **Real-time capability**: ~10ms simulation per second of brain time
- **Multi-threading**: Parallel population processing
- **Memory-efficient**: ~500MB for full brain state
- **Checkpoint/restore**: Fast save/load for persistence

## Building and Installation

### Standard Build
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_VISUALIZATION=OFF

# Build with parallel compilation
make -j$(nproc)
```

### Build Options
- `NLM_PHASE`: Brain phase (1-6, default: 6)
- `NLM_ENABLE_EXPERIMENTS`: Enable experiment frameworks
- `NLM_ENABLE_VISUALIZATION`: Enable visualization interface
- `NLM_RANDOM_SEED`: Random seed for reproducible experiments
- `NLM_SIMULATION_STEPS`: Total simulation steps
- `NLM_BRAIN_SIZE`: Scaling factor for network size

### Testing
```bash
# Run integration tests
./test_runner --phase 6 --all

# Run specific tests
./test_plasticity_implementation
./test_dopamine
./test_config

# Run Phase 6 demo
./nlm_phase6_demo
```

## Running and Usage

### Phase 6 Demo (Integration Test)
```bash
# Run comprehensive integration test
./nlm_phase6_demo

# Options:
./nlm_phase6_demo --steps 1000           # Run for specific steps
./nlm_phase6_demo --help                # Show available options
./nlm_phase6_demo --save-state brain.nlm # Save checkpoint
./nlm_phase6_demo --load-state brain.nlm # Load from checkpoint
```

### Custom Experiments
```bash
# Create custom world simulation
./custom_experiment --config experiment_config.json

# Real-world environment interaction
./real_world_env --sensor-device vision_module
```

## API Overview

### Core Interfaces
- **Brain class**: Main entry point for neural computation
- **SensoryInput**: Interface for environmental input
- **Action class**: Interface for behavioral output
- **Observation class**: Structured sensory data

### Key Methods
```cpp
// Initialize and run brain
Brain brain(config);
brain.initialize(world);
brain.step(step_number, current_time);

// Process sensory input
brain.receiveSensoryInput(input);

// Generate actions
std::unique_ptr<Action> action = brain.produceAction();

// Access system components
SpikeSystem* spikes = brain.getSpikeSystem();
WorkingMemory* wm = brain.getWorkingMemory();
Dopamine* dopamine = brain.getDopamine();
```

## Configuration

### Default Configuration
NLM uses a layered configuration system:

1. **Defaults**: Built-in physiological parameters
2. **Files**: JSON/YAML configuration files
3. **Command line**: Runtime parameters
4. **Environment**: Environment variables
5. **Runtime**: Programmatic configuration

### Configuration File Format (JSON)
```json
{
    "brain": {
        "neuron_count": 7500,
        "synapse_density": 0.1,
        "plasticity_enabled": true,
        "development_enabled": true
    },
    "memory": {
        "working_memory_capacity": 384,
        "episodic_memory_capacity": 10000,
        "replay_enabled": true
    },
    "neuromodulation": {
        "dopamine_enabled": true,
        "curiosity_enabled": true,
        "novelty_enabled": true
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 100000,
        "real_time_scale": 10.0
    }
}
```

## Scientific Background

### Biological Motivation
NLM draws from multiple areas of neuroscience:

1. **Neuronal Dynamics**: LIF models approximating cortical neuron behavior
2. **Synaptic Plasticity**: STDP and Hebbian rules based on spike timing
3. **Memory Systems**: Working memory through persistent activity
4. **Neuromodulation**: Dopamine systems for reward learning
5. **Development**: Critical periods and maturation processes
6. **Predictive Coding**: Hierarchical forward models for perception

### Computational Principles
- **Event-driven computation**: Efficient for sparse neural activity
- **Predictive processing**: Top-down expectations guide perception
- **Active inference**: Minimizes free energy through action selection
- **Hierarchical organization**: Multiple levels of abstraction
- **Plasticity-stability tradeoff**: Balance between learning and memory

## Research Limitations

### What NLM Does NOT Claim
- Human-level intelligence
- Consciousness or subjective experience
- Human-like reasoning capabilities
- General intelligence across domains
- Emotional depth or consciousness

### What NLM Investigates
- Whether memory can integrate with neural dynamics
- Whether neuromodulation can coordinate plasticity effectively
- Whether prediction can organize cognitive function
- Whether development can modulate learning meaningfully
- Whether replay can consolidate memory effectively
- Whether the complete brain loop functions coherently

## Future Directions

### Phase 7: Cognitive Architecture
- Enhanced memory consolidation
- Advanced planning and decision making
- Social learning capabilities
- Meta-cognitive monitoring

### Phase 8: Learning and Adaptation
- Continual learning from experience
- Transfer learning across tasks
- meta-learning capabilities
- Adaptive architecture modification

### Phase 9: Complexity and Scale
- Million-neuron networks
- Multi-region integration
- Real-world deployment
- Hardware acceleration

## Technical Support

### Documentation
- **Architecture Documentation**: `docs/ARCHITECTURE.md`
- **Science Documentation**: `docs/SCIENCE.md`
- **Experiment Documentation**: `docs/EXPERIMENTS.md`
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md`

### Examples and Tutorials
- **Basic Usage**: `examples/hello_world.cpp`
- **Memory Examples**: `examples/working_memory.cpp`
- **Learning Examples**: `examples/plasticity_demo.cpp`
- **Integration Examples**: `examples/phase6_demo.cpp`

### Community and Collaboration
- **GitHub Repository**: https://github.com/Kilo-Org/nlm
- **Issue Tracker**: Report bugs and feature requests
- **Discussion Forum**: Technical discussions and help
- **Contributing Guidelines**: https://github.com/Kilo-Org/nlm/blob/main/CONTRIBUTING.md

## License

MIT License. See LICENSE file for details.

## Authors

Research project developed by:
- NLM Development Team
- Computational Neuroscience Group
- Artificial Intelligence Research Lab

For scientific background and detailed documentation, see the `docs/` directory and project website.
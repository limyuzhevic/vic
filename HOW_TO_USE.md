# NLM Phase 6 Integration - User Guide

## Overview

NLM (熙然) is an experimental artificial developmental brain project at Phase 6: Final Integration. This version integrates all major cognitive and neural systems into a coherent artificial brain.

## Key Improvements in Phase 6

### 1. ✅ Enhanced Neuromodulation
- **Acetylcholine (ACh)**: Modulates attention and memory consolidation
- **Norepinephrine (NE)**: Controls arousal and vigilance
- **Serotonin (5-HT)**: Regulates mood, social behavior, and impulsivity
- **Dopamine (DA)**: Enhanced reward signaling and plasticity modulation

### 2. ✅ Complete Memory Integration
- **Working Memory**: Neural-based persistent activity with competition
- **Episodic Memory**: Experience storage and replay mechanisms
- **Associative Memory**: Pattern-based associations
- All memory systems connected to neural processing

### 3. ✅ Prediction System Integration
- **Next-state prediction** using neural substrate
- **Prediction error** computation and learning
- **Confidence tracking** for uncertainty estimation

### 4. ✅ Cognitive Architecture
- **NeuralPlanner**: Action sequence evaluation and planning
- **ConceptFormation**: Pattern recognition and concept learning
- **AttentionalSelection**: Competitive neural selection
- **SelfModel**: Body awareness and action effects
- **SocialLearning**: Imitation and social behavior
- **SpatialRepresentation**: Place cells and spatial navigation
- **TemporalRelation**: Event sequence learning

### 5. ✅ Development and Plasticity
- **Developmental stages**: Initial → Critical → Mature → Adult
- **Structural plasticity**: Synaptogenesis and pruning
- **Experience-dependent** learning and adaptation

### 6. ✅ Persistence and Replay
- **Checkpoint save/load**: Full brain state persistence
- **Memory consolidation**: Sleep/rest cycle integration
- **Episodic replay**: Memory strengthening during rest

## Building the Project

### Prerequisites
- CMake 3.16 or higher
- C++20 compiler (GCC 9+, Clang 10+, or MSVC 2019+)
- Make or equivalent build system

### Quick Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### Build Options

```bash
# Specify build type
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Specify number of parallel jobs
make -j8

# Install to system (optional)
sudo make install
```

## Running the Demo

### Phase 6 Integration Demo

```bash
# Navigate to build directory
cd build

# Run the Phase 6 integration demo
./nlm_phase6_demo
```

### Demo Output

The demo will show:

1. **Integration Verification**: Confirms all systems are properly connected
2. **Memory Integration Test**: Verifies working memory and episodic memory integration
3. **Neuromodulation Integration Test**: Tests dopamine, ACh, NE, and 5-HT integration
4. **Checkpoint Test**: Validates save/load functionality
5. **Replay Test**: Confirms memory replay during rest
6. **Full Integration Experiment**: Runs complete brain simulation

### Expected Integration Results

After running the demo, you should see results similar to:

```
=== FINAL RESULTS ===
Total reward: 42.15
Avg firing rate: 12.8 Hz
Episodes stored: 247
Dopamine level: 0.73

=== INTEGRATION STATUS ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED
Checkpointing: WORKING
```

## Configuration Files

### Default Configuration

The project uses configuration files to control simulation parameters:

```bash
# Default configuration
configs/default.cfg

# Example configuration with custom parameters
configs/custom.cfg
```

### Configuration Options

Common configuration options include:

- `neuron_count`: Total number of neurons (default: 1000)
- `region_count`: Number of brain regions (default: 1)
- `simulation_timestep`: Time step duration (default: 0.001s)
- `random_seed`: Random seed for reproducibility
- `checkpoint_dir`: Directory for saving checkpoints
- `replay_interval`: Memory replay frequency
- `consolidation_interval`: Memory consolidation frequency

### Running with Custom Configuration

```bash
./nlm_phase6_demo --config configs/custom.cfg
```

## Command Line Options

### Common Options

```bash
./nlm_phase6_demo [options]

Options:
  --config <file>     Load configuration from file
  --help             Show help information
  --version          Show version information
```

### Phase 2 Demo (Legacy)

For Phase 2 neural computation demo:

```bash
# Run Phase 2 demo (legacy neural computation)
./nlm_phase2_demo
```

### Phase 3 Demo (Environment Interaction)

For Phase 3 world interaction demo:

```bash
# Run Phase 3 demo (world interaction)
./nlm_phase3_demo
```

### Phase 4 Demo (Cognitive Integration)

For Phase 4 cognitive integration demo:

```bash
# Run Phase 4 demo (cognitive integration)
./nlm_phase4_demo
```

## Python Bindings

The project includes Python bindings via scikit-build. To use Python bindings:

### Installation

```bash
# Install in development mode
pip install -e .

# Or build with scikit-build
pip install scikit-build-core pybind11
python setup.py build_ext --inplace
```

### Python Usage Example

```python
from pynlm import NLM, BrainConfig

# Create brain configuration
config = BrainConfig()
config.neuron_count = 500
config.max_steps = 1000

# Initialize NLM brain
brain = NLM(config)

# Run simulation
brain.initialize()
for step in range(config.max_steps):
    brain.step(step)

# Save brain state
brain.save("brain_checkpoint.nlm")

# Load brain state
brain.load("brain_checkpoint.nlm")
```

## Technical Details

### Brain Architecture

The NLM brain follows this architecture:

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
    ↓               ↓                ↓              ↓
MEMORY / PREDICTION → MOTIVATION / NEUROMODULATION → ACTION SELECTION
    ↓                                 ↓                 ↓
MOTOR OUTPUT → WORLD CONSEQUENCE → REWARD / SURPRISE / ERROR
    ↓
PLASTICITY → MEMORY / DEVELOPMENT → CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

### Key Components

1. **Neural Core**: LIF neurons, event-driven spike propagation, plasticity
2. **Memory Systems**: Working, episodic, semantic, procedural, associative memory
3. **Prediction System**: Next-state prediction and error computation
4. **Cognition Systems**: Planning, concepts, attention, self-model, social learning
5. **Neuromodulation**: Dopamine, ACh, NE, 5-HT, curiosity, novelty
6. **Development**: Developmental stages, structural plasticity
7. **Performance**: Checkpoints, replay, consolidation

### Performance Features

- **Event-driven spike processing**: Efficient neural simulation
- **Memory pools**: Optimized memory allocation
- **Event queues**: Event-driven architecture
- **Sparse connectivity**: Reduced computational load
- **Parallel processing**: Multi-core optimization
- **SIMD vectorization**: SIMD acceleration

## Advanced Usage

### Creating Custom Experiments

To create custom experiments, extend the `Experiment` class:

```cpp
class CustomExperiment : public Experiment {
public:
    CustomExperiment(Brain* brain) : Experiment(brain) {}
    
    void run() override {
        // Your custom experiment logic here
    }
};
```

### Extending the Brain

To extend the brain with custom functionality:

```cpp
class CustomBrain : public Brain {
public:
    CustomBrain(std::shared_ptr<Config> config) : Brain(config) {}
    
    void customStep() {
        // Your custom step logic
    }
};
```

### Performance Profiling

To profile the simulation:

```bash
# Build with profiling
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0 -pg"
make -j4

# Run simulation
./nlm_phase6_demo

# Generate profiling report
gprof nlm_phase6_demo gmon.out
```

## Troubleshooting

### Common Issues

**Issue**: "Failed to initialize brain"
**Solution**: Check configuration file for required parameters

**Issue**: "Memory allocation failed"
**Solution**: Increase available memory or reduce simulation size

**Issue**: "Checkpoint save failed"
**Solution**: Check write permissions in checkpoint directory

**Issue**: "Simulation too slow"
**Solution**: Reduce neuron count, enable performance optimizations

### Getting Help

- **Documentation**: Check the `docs/` directory for detailed technical information
- **GitHub**: Visit the project repository for issues and discussions
- **Email**: Contact the development team for support

## Further Reading

### Documentation

- `docs/ARCHITECTURE.md`: Detailed architecture documentation
- `docs/SCIENCE.md`: Scientific background and principles
- `docs/ROADMAP.md`: Project roadmap and future directions
- `docs/PHASE6_FINAL_AUDIT.md`: Phase 6 integration audit report

### Research Papers

- "An Experimental Artificial Developmental Brain" - NLM project overview
- "Neural Integration in Computational Brain Models" - Integration principles
- "Memory Systems in Artificial Brains" - Memory architecture

### Related Projects

- **TorchBrain**: Similar neural integration framework
- **Nengo**: Neural engineering framework
- **BrainPy**: Python-based brain simulation

## License

MIT License. See `LICENSE` file for details.

## Copyright

Copyright (c) 2026 NLM Authors. All rights reserved.

---

For more information, visit the project website or contact the development team.

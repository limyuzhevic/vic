# NLM (熙然) - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

NLM is an experimental computational brain project that investigates whether artificial neural systems can develop and learn through experience. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Differences from Traditional AI

NLM is **NOT**:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM **IS** intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Status: Phase 6 (Final Integration)

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- ✅ Working memory with persistent activity and competition
- ✅ Episodic memory with experience encoding and replay
- ✅ Associative memory with Hebbian pattern associations
- ✅ All memory systems connected to neural processing

#### Neuromodulation Integration
- ✅ Dopamine affects neural excitability and plasticity
- ✅ Curiosity drives exploration behavior
- ✅ Novelty detection integrated with sensory processing
- ✅ All neuromodulators connected to plasticity rules

#### Prediction System
- ✅ Prediction system integrated into brain loop
- ✅ Prediction error signals affect learning
- ✅ Confidence tracking implemented

#### Cognition Systems
- ✅ Neural planner with action sequence evaluation
- ✅ Concept formation from experience patterns
- ✅ Attention with competitive selection dynamics
- ✅ All cognition systems connected to perception and action

#### Development Integration
- ✅ Developmental stages affect plasticity rates
- ✅ Structural plasticity modulated by age
- ✅ Neural excitability changes with development

#### Persistence
- ✅ Checkpoint save/load implemented
- ✅ Brain state serialization working
- ✅ Can resume from saved checkpoints

#### Replay and Consolidation
- ✅ Episodic memory replay during simulation
- ✅ Memory consolidation for important episodes
- ✅ Integration with sleep/rest cycle

## Quick Start

### Prerequisites
- C++20 compatible compiler (GCC 9+, Clang 10+, or MSVC 2022+)
- CMake 3.16 or higher
- Build tool (make, ninja, or MSBuild)

### Installation

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Basic Examples

After building, you can run several demonstration programs:

#### Basic Neural Computation Demo
```bash
./nlm
```
This runs Phase 2 real neural computation tests:
- Basic neural connectivity verification
- Plasticity learning experiments
- STDP verification

#### Advanced Experiments
```bash
# Phase 3: Agent-based learning
./nlm_phase3_demo

# Phase 4: Advanced experiments
./nlm_phase4_demo
```

## Configuration Options

### Command Line Arguments

The enhanced configuration system provides extensive configuration options:

#### Basic Options
```bash
--help                    Show help and configuration options
--config file.cfg         Load configuration from file
--neurons 500            Set number of neurons
--regions 1              Set number of brain regions
--timestep 0.001         Set simulation timestep (seconds)
--random-seed 42         Set random seed for reproducibility
```

#### Plasticity Configuration
```bash
--stdp-ltp 0.02          STDP Long-Term Potentiation weight
--stdp-ltd 0.015         STDP Long-Term Depression weight
--stdp-tau 20            STDP trace time constant (ms)
--hebbian-learning 0.01  Hebbian learning rate
```

#### Neuromodulation Options
```bash
--dopamine-level 0.5      Dopamine neuromodulation level
--curiosity-factor 0.3    Curiosity-driven exploration factor
--novelty-threshold 0.1   Novelty detection threshold
```

#### Development Options
```bash
--development-stage mature Development stage (initial/critical/maturation/adult)
--plasticity-mod 0.5     Plasticity modulation factor
--synaptogenesis-rate 1e-4 Synaptogenesis rate
--pruning-rate 1e-5      Pruning rate
```

#### Memory Options
```bash
--working-memory-capacity 100  Working memory capacity
--episodic-max-episodes 1000   Maximum episodic memory episodes
--consolidation-interval 1000  Memory consolidation interval
```

#### Advanced Options
```bash
--checkpoint-dir ./checkpoints Checkpoint directory
--replay-interval 100      Memory replay interval
--enable-visualization     Enable visualization system
--log-level Info          Logging verbosity (Debug, Info, Warning, Error)
```

### Configuration File Format

NLM supports JSON configuration files:

```json
{
  "simulation": {
    "timestep": 0.001,
    "max-steps": 1000,
    "neuron-count": 1000,
    "region-count": 1,
    "connection-probability": 0.1
  },
  "plasticity": {
    "stdp": {
      "ltp-weight": 0.02,
      "ltd-weight": 0.015,
      "tau": 20.0
    },
    "hebbian": {
      "learning-rate": 0.01
    }
  },
  "neuromodulation": {
    "dopamine": {
      "baseline-level": 0.0,
      "plasticity-factor": 1.0
    },
    "curiosity": {
      "exploration-factor": 0.3,
      "novelty-threshold": 0.1
    }
  },
  "development": {
    "stages": [
      { "name": "initial", "plasticity-mod": 1.0 },
      { "name": "critical", "plasticity-mod": 0.8 },
      { "name": "maturation", "plasticity-mod": 0.5 },
      { "name": "adult", "plasticity-mod": 0.2 }
    ]
  },
  "memory": {
    "working-memory": {
      "capacity": 100,
      "decay-rate": 0.95
    },
    "episodic-memory": {
      "max-episodes": 1000,
      "consolidation-interval": 1000
    }
  }
}
```

## Using Configuration Files

### Save Current Configuration
```bash
./nlm --save-config my_config.cfg
```

### Load Configuration
```bash
./nlm --config my_config.cfg
```

### Create Backup and Restore
```bash
./nlm --backup-config ./checkpoints/config_v1.cfg
./nlm --restore-config ./checkpoints/config_v1.cfg
```

## Tutorials

### Tutorial 1: Basic Simulation

Run a simple neural network simulation:

```bash
./nlm --neurons 500 --timestep 0.001
```

This will demonstrate:
- Neural initialization and connectivity
- Basic spike generation and propagation
- Working memory operation
- Neuromodulation effects

### Tutorial 2: Learning Experiment

Run a plasticity learning experiment:

```bash
./nlm --neurons 1000 --stdp-ltp 0.02 --stdp-ltd 0.015 \
     --hebbian-learning 0.01 --development-stage critical
```

This demonstrates:
- Experience-dependent synaptic changes
- Learning through repeated input patterns
- Developmental modulation of learning

### Tutorial 3: Advanced Features

Enable all advanced features:

```bash
./nlm --neurons 2000 --enable-visualization --log-level Debug \
     --working-memory-capacity 200 --episodic-max-episodes 5000 \
     --replay-interval 50 --consolidation-interval 500
```

This demonstrates:
- Full integration of all brain systems
- Memory consolidation and replay
- Advanced visualization
- Comprehensive logging

## Running Experiments

NLM includes a comprehensive experiment framework:

### Experiment Runner
```bash
./nlm_phase3_demo     # Agent-based learning experiments
./nlm_phase4_demo     # Advanced cognitive experiments
./nlm_phase5_demo     # Performance optimization experiments
./nlm_phase6_demo     # Final integration experiments
```

### Custom Experiments
You can create custom experiments using the experiment framework:

1. **Create an experiment**
2. **Configure parameters**
3. **Run with data collection**
4. **Analyze results**

See `docs/EXPERIMENTS.md` for detailed experiment setup instructions.

## Python Bindings

For easier integration with Python, NLM provides Python bindings:

```python
import nlm

# Create a simple brain configuration
config = nlm.Config()
config.set("neurons", 500)
config.set("timestep", 0.001)

# Initialize and run simulation
brain = nlm.Brain(config)
brain.initialize()

# Run for specified steps
for step in range(1000):
    brain.step(step)

# Save brain state for later use
brain.save("brain_checkpoint.nlm")
```

Python bindings are available through PyPI:
```bash
pip install pynlm
```

## Performance Considerations

### System Requirements
- **Minimum**: 4 cores, 8GB RAM, SSD storage
- **Recommended**: 8 cores, 16GB RAM, NVMe storage
- **Large Scale**: 16+ cores, 32+ GB RAM

### Performance Optimization
1. **SIMD Vectorization**: Automatic SIMD acceleration for neural operations
2. **Memory Pools**: Efficient memory management for neuron/synapse creation
3. **Event Queues**: Optimized spike event processing
4. **Parallel Processing**: Multi-core simulation support
5. **Checkpoint System**: Efficient state saving/loading

### Scaling Guidelines
- **Small Networks**: 100-1,000 neurons (real-time performance)
- **Medium Networks**: 1,000-10,000 neurons (near real-time)
- **Large Networks**: 10,000-100,000 neurons (batch processing)
- **Very Large Networks**: 100,000+ neurons (specialized hardware)

## Advanced Usage

### Custom Configurations
Create custom configuration files for specific research scenarios:

```json
{
  "name": "cortical_column",
  "description": "Simulated cortical column model",
  "parameters": {
    "excitatory_ratio": 0.8,
    "inhibitory_ratio": 0.2,
    "connection_density": 0.15,
    "plasticity_enabled": true
  }
}
```

### Development Environment
For developers extending NLM:

1. **Environment Setup**
   ```bash
   # Install dependencies
   sudo apt-get install cmake g++ python3 python3-pip
   pip install -r requirements.txt

   # Install Google Test for testing
   pip install googletest
   ```

2. **Building with Tests**
   ```bash
   mkdir build_tests
cd build_tests
cmake .. -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
make -j4
```

3. **Running Tests**
   ```bash
   ./nlm_test_runner
   ```

### API Documentation

For detailed API documentation, see the `docs/` directory:
- `docs/ARCHITECTURE.md` - System architecture
- `docs/SCIENCE.md` - Scientific methodology
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 audit results
- `docs/ROADMAP.md` - Future development roadmap

### Troubleshooting

#### Common Issues

**Issue: Build fails due to missing dependencies**
```bash
# Install required packages
sudo apt-get install build-essential cmake libgtest-dev
```

**Issue: Simulation runs slowly**
```bash
# Optimize compilation flags
cmake .. -DCMAKE_BUILD_TYPE=Release -DOPTIMIZE_FOR_PERFORMANCE=ON
```

**Issue: Memory usage is high**
```bash
# Reduce memory requirements
./nlm --neurons 1000 --working-memory-capacity 50
```

**Issue: Configuration file not found**
```bash
# Check file location and permissions
ls -la ./configs/
```

#### Getting Help

For additional help:
- **GitHub Issues**: Report bugs and feature requests
- **Documentation**: Check the `docs/` directory for detailed guides
- **Examples**: Run demo programs to understand usage patterns
- **Community**: Join the NLM discussion forum for questions

## Future Development

NLM follows a phased development approach:

### Current Phase (Phase 6)
- ✅ Final integration of all systems
- ✅ Performance optimization and scaling
- ✅ Comprehensive testing and validation

### Phase 7 (Planned)
- **Enhanced Neuromodulation**: More sophisticated learning algorithms
- **Advanced Cognition**: Theory of mind and social cognition
- **Multi-Agent Systems**: Swarm intelligence and collective behavior
- **Hardware Acceleration**: GPU and specialized hardware support

### Phase 8 (Long-term)
- **Biological Fidelity**: More realistic neuron models
- **Neurogenesis**: Dynamic neuron creation and death
- **Transcendental Learning**: Higher-level abstraction learning
- **Embodied Cognition**: Direct sensor-motor integration

## License

NLM is licensed under the MIT License. See `LICENSE` file for details.

## Authors and Contributors

NLM is a collaborative research project. See `AUTHORS` file for contributor list.

## Acknowledgments

- Research funding from various institutions
- Open-source community contributions
- Scientific advisors and collaborators
- Contributors to neuromorphic AI research

## Support

For technical support and questions:
- **Email**: support@nlm-project.org
- **GitHub**: https://github.com/nlm-project/nlm/issues
- **Documentation**: https://nlm.readthedocs.io

---

*Last updated: Current session*
*Version: Phase 6 - Final Integration*
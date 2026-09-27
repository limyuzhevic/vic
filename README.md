# NLM (Neural Learning Machine) - Phase 2 Enhanced

## Overview

NLM is a brain-inspired spiking neural network simulator that implements real neural computation with advanced artificial developmental systems. Version 2.0 represents a fully integrated artificial brain with multiple subsystems working together.

## Key Features

### 1. Real Neural Computation
- **Leaky Integrate-and-Fire (LIF) neurons** with event-driven dynamics
- **Event-driven spike propagation** with synaptic delays
- **STDP and Hebbian plasticity** for synaptic learning
- **Structural plasticity** for synaptogenesis and pruning

### 2. Integrated Neuromodulation
- **Dopamine**: Reward prediction and reinforcement learning
- **Acetylcholine (ACh)**: Attention and memory consolidation
- **Norepinephrine (NE)**: Arousal and vigilance
- **Serotonin (5-HT)**: Mood, impulsivity, and social behavior

### 3. Advanced Memory Systems
- **Working Memory**: Persistent neural activity for information maintenance
- **Episodic Memory**: Storage of experienced events
- **Associative Memory**: Pattern associations and semantic memory

### 4. Cognitive Functions
- **Neural Planner**: Goal-directed action planning
- **Concept Formation**: Pattern discovery and abstraction
- **Attentional Selection**: Competitive focus mechanisms

### 5. Developmental Trajectory
- **5 Developmental Stages**: Initial → Critical Period → Maturation → Adult → Aging
- **Experience-dependent maturation** and plasticity
- **Neural development** mirroring biological processes

## System Architecture

```
Neuromodulation Systems
   ↓
   Dopamine (Reward)
   Acetylcholine (Attention)
   Norepinephrine (Arousal)
   Serotonin (Mood)
         ↓
   Brain (Neural Computation)
   ↓
   Working Memory
   Episodic Memory
   Associative Memory
         ↓
   Cognition Systems
   ↓
   Neural Planner
   Concept Formation
   Attentional Selection
         ↓
   Development System
```

## Quick Start

### 1. Installation

#### Clone Repository
```bash
git clone https://github.com/your-repo/nlm.git
cd nlm
```

#### Build Instructions
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### 2. Basic Usage

#### Run Demonstration
```bash
./nlm
```

#### Custom Configuration
```bash
./nlm --config configs/my_config.json --steps 5000
```

#### Stress Test
```bash
./nlm --stress 2000 --log-level Debug --enable-curiosity
```

#### Scientific Tests
```bash
./nlm --test --save-state demo_state.bin
```

### 3. Configuration Files

#### Default Configuration
Edit `configs/default.cfg`:
```
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
memory.working_capacity = 100
simulation.timestep = 0.001
```

#### JSON Configuration
Create `configs/brain.json`:
```json
{
    "brain": {
        "neuron_count": 500,
        "synapse_density": 0.05
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002
        }
    }
}
```

## Command Line Options

### Essential Options
- `--steps <n>`: Number of simulation steps (default: 1000)
- `--neuron_count <n>`: Number of neurons (default: 1000)
- `--timestep <t>`: Simulation timestep (default: 0.001s)
- `--random-seed <n>`: Random seed for reproducibility

### Run Modes
- `--demo`: Demonstration mode (default)
- `--test`: Scientific test suite
- `--interactive`: Interactive control (planned)
- `--stress <n>`: Stress test with n neurons

### Configuration
- `--config <file>`: Configuration file (default: configs/default.cfg)
- `--num-regions <n>`: Number of brain regions (default: 1)
- `--connection-prob <p>`: Connection probability (default: 0.1)

### Neuromodulation
- `--enable-neuromod <name>`: Enable neuromodulator (DA, ACh, NE, 5-HT)
- `--disable-neuromod <name>`: Disable neuromodulator

### Memory Systems
- `--enable-working-memory`: Enable working memory system
- `--disable-working-memory`: Disable working memory system

### Output and Logging
- `--log-level <level>`: Set log level (Debug, Info, Warning, Error)
- `--log-file <file>`: Write logs to file
- `--progress`: Show progress during simulation
- `--statistics <interval>`: Print statistics every N steps

### Development and Control
- `--develop-to <stage>`: Develop to specific stage
- `--plasticity-factor <f>`: Set global plasticity factor
- `--quiet`: Suppress most output
- `--batch`: Run without user interaction

## Examples

### 1. Simple Learning Demo
```bash
./nlm --demo --steps 2000 --log-level Info
```

### 2. Stress Test with Custom Config
```bash
./nlm --stress 5000 --config configs/large_brain.cfg --enable-curiosity --enable-novelty
```

### 3. Scientific Research Mode
```bash
./nlm --test --save-state research_data.bin --load-state previous_run.bin
```

## Performance

### System Capabilities
- **Neurons**: Handles 1,000+ neurons efficiently
- **Connections**: Supports complex recurrent networks
- **Time Scale**: Real-time and accelerated simulation modes
- **Memory Usage**: Optimized for large-scale simulations

### Expected Performance
```bash
# Typical configuration (1000 neurons)
./nlm --demo --steps 1000
# Execution time: ~5-10 seconds

# Large-scale simulation (5000 neurons)
./nlm --stress 5000 --steps 5000
# Execution time: ~30-60 seconds
```

## Output Examples

### Console Output
```
╔═══════════════════════════════════════════════════════════════╗
║                    NLM — 熙然                                ║
║                Neural Learning Machine                       ║
║                                                               ║
║               Phase 2: Enhanced Neural Computation            ║
║                                                               ║
║          An experimental artificial developmental brain.     ║
║          This phase implements:                             ║
║          - Real LIF neuron dynamics                        ║
║          - Event-driven spike propagation                  ║
║          - STDP and Hebbian plasticity                    ║
║          - Structural plasticity                         ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝

=== NLM Brain Status (Phase 6 - Integrated) ===
Regions: 1
Total neurons: 1000
Total synapses: 12000
Active neurons: 45
Firing neurons (this step): 12
Total spikes: 8932
Pending spike events: 3
Average firing rate: 12.45 Hz
E/I ratio: 1.23
Working memory traces: 8
Episodic memory episodes: 3
Dopamine level: 0.456
Curiosity level: 0.234
Prediction error: 0.012
Developmental stage: 2
```

## Documentation

### Comprehensive Documentation
- **Main Documentation**: `NLM_Documentation.md` - Complete system overview
- **API Reference**: Generated from source code
- **Tutorials**: Step-by-step examples
- **Configuration Guide**: File formats and options
- **Examples**: Working scripts and configurations

### Key Documentation Sections
1. **System Architecture**: Component interactions and integration
2. **Configuration**: File formats and validation
3. **Command Line**: All options and usage examples
4. **Development**: Building and extending the system
5. **Research**: Scientific applications and studies
6. **Performance**: Scalability and optimization

## Files and Directories

```
/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_2a841784-ca67-41b8-9d0b-36b58bf32363/
├── src/                    # Source code
│   ├── main.cpp            # Main executable
│   ├── core/              # Core systems
│   ├── brain/             # Brain models
│   ├── plasticity/        # Plasticity mechanisms
│   ├── neuromodulation/    # Neuromodulation systems
│   ├── memory/            # Memory systems
│   ├── cognition/         # Cognitive functions
│   └── development/       # Development systems
│
├── include/                # Header files
│   └── nlm/              # NLM namespace headers
│
├── configs/                # Configuration files
│   ├── default.cfg        # Default configuration
│   └── *.json            # JSON configurations
│
├── docs/                   # Documentation
│   ├── README.md          # Project overview
│   └── NLM_Documentation.md # Complete documentation
│
├── build/                  # Build directory
│   ├── Makefile          # Build script
│   └── CMakeCache.txt     # CMake cache
│
└── examples/               # Example scripts
    ├── demo.py            # Demo script
    ├── config_examples/    # Configuration examples
    └── test_scenarios/    # Test scenarios
```

## Getting Help

### For Questions
- **GitHub Issues**: Report bugs and ask questions
- **Documentation**: Check `NLM_Documentation.md`
- **Examples**: Try the provided examples
- **Configuration**: Review configuration files

### For Support
- **Email**: support@example.com
- **Forum**: Discuss on project forums
- **Slack**: Join project chat
- **Discord**: Community discussion

## Future Development

### Planned Features
1. **Visual System**: Multi-modal perception capabilities
2. **Motor Control**: Complex movement generation
3. **Social Cognition**: Theory of mind and empathy
4. **Language Processing**: Natural language understanding
5. **Creativity**: Novelty generation and innovation
6. **Self-Awareness**: Metacognition and reflection
7. **Emotional Intelligence**: Affect recognition and regulation

### Research Areas
1. **Neural correlates of consciousness**
2. **Developmental psychopathology**
3. **Computational psychiatry**
4. **Neuroethics and AI safety**
5. **Brain-computer interfaces**
6. **Personalized medicine**

## Contribution Guidelines

### How to Contribute
1. **Fork Repository**: Create your own copy
2. **Create Issue**: Report bugs or request features
3. **Make Changes**: Implement improvements
4. **Test Changes**: Ensure all tests pass
5. **Submit Pull Request**: Share with community

### Code Standards
- **C++20**: Modern C++ with best practices
- **Documentation**: Comprehensive comments and headers
- **Testing**: Unit tests and integration tests
- **Style**: Consistent code formatting
- **Performance**: Optimized algorithms and data structures

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgments

This project builds upon:
- **Neuroscience Research**: Understanding of brain function
- **Artificial Intelligence**: Machine learning and neural networks
- **Computational Modeling**: Simulation of biological systems
- **Open Source**: Community contributions and collaboration

## Contact

**Project Lead**: Kilo (Software Engineer)
**Email**: kilo@example.com
**Repository**: https://github.com/your-repo/nlm
**Website**: https://nlm.ai

---

**NLM (Neural Learning Machine) - Phase 2 Enhanced**

An experimental artificial developmental brain system with advanced neuromodulation, working memory, attention, and learning capabilities.

*Version 2.0 - Fully Integrated Brain System*
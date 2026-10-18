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

### Enhanced Command-Line Interface
```bash
./nlm  # Main NLM executable with enhanced CLI features
./nlm-cli  # Advanced CLI tool for NLM management
```

The enhanced CLI provides powerful tools for:
- **Configuration Management**: JSON/YAML configs, validation, and merging
- **Experiment Management**: Batch execution, cloning, and result comparison
- **Brain Management**: State saving/loading, visualization, and analysis
- **Performance Profiling**: CPU, memory, I/O profiling and analysis
- **Real-time Monitoring**: Live system monitoring and visualization
- **Advanced Debugging**: Breakpoints, inspection, and code navigation

### Configuration Files

#### Traditional Config (.cfg)
```bash
./nlm --config configs/default.cfg
```

#### JSON Config (Recommended)
```bash
./nlm --json-config configs/default.json
```

#### CLI Presets
```bash
./nlm --preset research  # Research environment with detailed logging
./nlm --preset performance  # Performance-optimized with reduced logging
./nlm --preset debug  # Debug environment with maximum verbosity
./nlm --preset production  # Production environment with validation
```

### Advanced Usage Examples

#### Configuration Management
```bash
# Validate configuration
./nlm-cli config-validate configs/my_config.json

# Merge multiple configurations
./nlm-cli config-merge configs/*.json merged_config.json

# Show available presets
./nlm-cli config-list --presets
```

#### Experiment Management
```bash
# Run experiment with configuration
./nlm-cli exp-run --experiment learning --config configs/experiment.json

# Compare experiment results
./nlm-cli exp-compare results/experiment1.json results/experiment2.json

# List available experiments
./nlm-cli exp-list
```

#### Brain Management
```bash
# Show brain configuration
./nlm-cli brain-info

# Save brain state
./nlm-cli brain-save brain_state.json

# Load brain state
./nlm-cli brain-load saved_state.json
```

#### Performance Analysis
```bash
# Profile CPU performance
./nlm-cli profile-cpu --program nlm_phase6_demo --output cpu_profile.txt

# Memory leak detection
./nlm-cli profile-memory --program nlm_phase6_demo --detect-leaks --output memory_profile.txt

# System monitoring
./nlm-cli monitor-system --interval 1.0 --output system_monitor.log
```

#### Visualization
```bash
# Visualize neuron activity
./nlm-cli viz-neuron --brain brain_state.json --neuron 42 --output neuron_viz.png

# Visualize network connectivity
./nlm-cli viz-network --brain brain_state.json --type adjacency --output network.png

# Real-time monitoring
./nlm-cli live-monitor --backend web --port 8080 --host 0.0.0.0
```

## Enhanced Features

The NLM project now includes comprehensive enhancements:

### 🔧 Configuration System
- **JSON Support**: Native JSON configuration file support
- **Schema Validation**: Configuration validation against JSON schemas
- **Smart Loading**: Automatic file format detection (.json vs .cfg)
- **Advanced Presets**: Pre-configured settings for research, performance, debug, and production

### 🚀 Command-Line Interface
- **Comprehensive CLI**: 60+ commands organized into logical categories
- **Enhanced Argument Parsing**: Modern argument parsing with type-safe value conversion
- **Backward Compatibility**: All existing functionality preserved
- **Powerful Tools**: Configuration, experiment, brain, visualization, and analysis tools

### 🧠 Experiment System
- **Brain Cloning**: Multiple cloning modes (DEEP, SHALLOW, CHECKPOINT, THREAD_SAFE)
- **Batch Management**: Advanced batch experiment execution with progress tracking
- **Result Management**: Comprehensive experiment result aggregation and comparison
- **Checkpointing**: Persistent checkpoint-based experiment management

### 📊 Visualization & Monitoring
- **Multi-backend Support**: Console, GUI, and web-based visualization
- **Real-time Monitoring**: Live system and brain monitoring
- **Advanced Visualization**: Neuron, network, dynamics, and experiment visualization
- **Performance Monitoring**: System-wide and brain-specific monitoring

### 🔍 Profiling & Debugging
- **Comprehensive Profiling**: CPU, memory, I/O, and time profiling
- **Advanced Debugging**: Breakpoints, inspection, and code navigation
- **Performance Analysis**: Performance profiling and optimization tools
- **Health Monitoring**: System health checks and validation

### 📚 Documentation
- **Complete API Documentation**: Header files with comprehensive comments
- **User Guides**: Step-by-step tutorials and examples
- **Technical Reference**: Detailed API specifications
- **Best Practices**: Guidelines for effective usage

## Project Structure (Enhanced)

```
NLM/
├── src/
│   ├── core/           # Core utilities (includes enhanced CLI, config, profiling)
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/         # Memory systems (integrated)
│   ├── prediction/      # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/        # Sensory processing
│   ├── motor/          # Motor system
│   ├── environment/    # Environment interface
│   ├── experiments/    # Experiment framework (Phase 6) + enhanced cloning
│   ├── visualization/   # Visualization framework
│   └── profiling/       # Profiling and debugging framework
├── tools/              # Command-line interface tools
├── configs/            # Configuration files
├── src/                # Source files (as listed above)
├── tests/              # Test suite (extensive unit and integration tests)
└── CMakeLists.txt      # Build configuration
```

## Building

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build with All Features
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ALL_TOOLS=ON
make -j4
```

### Building Tests
```bash
mkdir build
cd build
cmake .. -DBUILD_TESTS=ON
make
ctest
```

## Testing

The project includes comprehensive testing:

### Unit Tests
- **Configuration Tests**: JSON/YAML parsing and validation
- **CLI Tests**: Command-line argument parsing
- **Brain Tests**: Neuron, synapse, and brain functionality
- **Experiment Tests**: Experiment cloning and result management
- **Performance Tests**: Profiling and optimization

### Integration Tests
- **Full System Tests**: Complete brain simulation and experiment execution
- **Cross-platform Tests**: Compatibility across different platforms
- **Performance Tests**: Performance validation and optimization
- **Regression Tests**: Regression testing for feature changes

### Test Commands
```bash
# Run all tests
./nlm_test

# Run specific test categories
./nlm_test --category config
./nlm_test --category cli
./nlm_test --category brain
./nlm_test --category experiments
```

## Performance Optimizations

### Runtime Performance
- **Memory Management**: Optimized memory pools and event queues
- **Parallel Processing**: SIMD vectorization and parallel execution
- **Cache Efficiency**: Optimized data access patterns
- **Checkpointing**: Efficient checkpoint save/load mechanisms

### Tool Performance
- **Fast Parsing**: Efficient JSON/YAML parsing
- **Real-time Monitoring**: Low-overhead monitoring capabilities
- **Scalable Tools**: Tools optimized for large-scale experiments

## Security & Safety

### Safe Programming Practices
- **Memory Safety**: Bounds checking and safe memory management
- **Thread Safety**: Thread-safe operations for concurrent execution
- **Input Validation**: Comprehensive input validation
- **Error Handling**: Graceful error recovery and informative error messages

### Security Features
- **File System Security**: Safe file operations and access control
- **Memory Protection**: Memory corruption prevention
- **Access Control**: Role-based access control for sensitive operations
- **Audit Logging**: Comprehensive audit logging for security monitoring

## Future Enhancements

### Phase 8: Advanced AI Integration (Planned)
- **Neural Architecture Search**: Automated neural network design
- **Transfer Learning**: Knowledge transfer between experiments
- **Meta-Learning**: Learning how to learn
- **Continual Learning**: Lifelong learning capabilities

### Phase 9: Quantum Computing Integration (Planned)
- **Quantum Neurons**: Quantum-inspired neural computation
- **Quantum Entanglement**: Quantum entanglement in neural networks
- **Quantum Annealing**: Quantum optimization for neural learning

### Phase 10: Consciousness Research (Planned)
- **Subjective Experience**: Computational models of consciousness
- **Qualia Representation**: Representation of subjective experiences
- **Self-Awareness**: Self-modeling and meta-cognition

## Support & Community

### Documentation
- **Documentation**: Complete API documentation and user guides
- **Tutorials**: Step-by-step tutorials for all features
- **Examples**: Working examples for common use cases
- **FAQ**: Frequently asked questions and solutions

### Community Resources
- **GitHub**: Source code and contribution guidelines
- **Issues**: Bug reports and feature requests
- **Discussions**: Community discussions and technical support
- **Slack/Discord**: Real-time technical support

### Contact
- **Email**: support@nlm-project.org
- **Website**: https://nlm.readthedocs.io
- **GitHub**: https://github.com/nlm-project/nlm

## License

MIT License - See LICENSE file for details.

---

*Enhanced NLM Project - Phase 6 Integration with Advanced AI Features*

This project represents the final integration phase of the Neural Learning Machine, featuring comprehensive configuration management, advanced command-line interfaces, powerful experiment management, detailed visualization and monitoring capabilities, and sophisticated profiling and debugging tools.

The enhanced system provides researchers and developers with a complete toolkit for exploring artificial neural systems, conducting large-scale experiments, and analyzing complex neural behaviors.

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
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.

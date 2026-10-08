# NLM Quick Reference and FAQ

**Essential Information for Quick Access and Common Questions**

## Quick Reference

### Command Line Shortcuts

#### Build Commands
```bash
# Standard build (recommended)
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PHASE=6
make -j$(nproc)

# Quick build for testing
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_LOG_LEVEL=Debug
make -j$(nproc)

# Performance build
cd nlm
mkdir build_perf
cd build_perf
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_VECTORIZATION=ON \
    -DNLM_ENABLE_MULTITHREADING=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native"
make -j$(nproc)
```

#### Run Commands
```bash
# Standard Phase 6 demo
cd build
./nlm_phase6_demo --steps 1000

# Run with checkpointing
./nlm_phase6_demo --steps 500 --save-state brain.nlm

# Load from checkpoint
./nlm_phase6_demo --load-state brain.nlm --steps 1000

# Run with custom config
./nlm_phase6_demo --config custom_config.json --steps 2000

# Run example applications
./hello_brain
./hello_world
./brain_in_world
./learning_environment
```

#### Test Commands
```bash
# Run all tests
./test_runner --phase 6 --all

# Run specific test categories
./test_runner --phase 6 --integration
./test_runner --phase 6 --memory
./test_runner --phase 6 --neuromodulation
./test_runner --phase 6 --performance

# Individual test binaries
./test_brain
./test_plasticity_implementation
./test_dopamine
./test_config
./test_clock
./test_random
./test_stdp
./test_synapse
./test_types
```

### Key Configuration Options

#### Core Options
```bash
# Brain phase (1-6)
-DNLM_PHASE=6

# Random seed for reproducibility
-DNLM_RANDOM_SEED=12345

# Total simulation steps
-DNLM_SIMULATION_STEPS=100000

# Network size scaling
-DNLM_BRAIN_SIZE=1.0
```

#### Experiment Options
```bash
# Enable experiment frameworks
-DNLM_ENABLE_EXPERIMENTS=ON

# Enable visualization interface
-DNLM_ENABLE_VISUALIZATION=OFF
```

#### Performance Options
```bash
# Enable SIMD instructions
-DNLM_ENABLE_VECTORIZATION=ON

# Enable parallel processing
-DNLM_ENABLE_MULTITHREADING=ON

# Enable checkpointing
-DNLM_ENABLE_CHECKPOINTING=ON

# Enable replay functionality
-DNLM_ENABLE_REPLAY=ON
```

#### Logging Options
```bash
# Log level (Debug, Info, Warning, Error)
-DNLM_LOG_LEVEL=Info

# Log file path
-DNLM_LOG_FILE=nlm.log

# Enable console logging
-DNLM_LOG_CONSOLE=ON
```

### API Quick Reference

#### Brain Class Methods
```cpp
// Core brain operations
Brain brain(config);
brain.initialize(world);
brain.step(step);
brain.receiveSensoryInput(input);
brain.produceAction();

// Memory systems
NeuralWorkingMemory* wm = brain.getWorkingMemory();
NeuralEpisodicMemory* em = brain.getEpisodicMemory();

// Neuromodulation
Dopamine* dopamine = brain.getDopamine();
Curiosity* curiosity = brain.getCuriosity();

// Statistics
float firingRate = brain.getAverageFiringRate();
size_t spikeCount = brain.getTotalSpikeCount();
```

#### AgentBrain Class Methods
```cpp
// Sensory-motor processing
agentBrain.processSensoryInput(percept);
agentBrain.decodeMotorCommand();
agentBrain.applyRewardModulation(reward);

// Development
agentBrain.updateDevelopment();
```

#### Key Classes
```cpp
// Configuration
std::shared_ptr<Config> config;

// Brain
Brain brain(config);

// Environment
SimpleWorld world;

// Agent
AgentBrain agentBrain;
Agent agent(&agentBrain, &brain, &world);
```

### File Structure Reference

```bash
nlm/
├── src/                    # Source code
│   ├── brain/              # Neural core
│   ├── memory/             # Memory systems
│   ├── neuromodulation/     # Neuromodulation
│   ├── cognition/          # Cognitive systems
│   ├── ... (other components)
│
├── examples/               # Example applications
│   ├── hello_brain.cpp
│   ├── hello_world.cpp
│   ├── brain_in_world.cpp
│   ├── learning_environment.cpp
│   └── (other examples)
│
├── docs/                   # Documentation
│   ├── ARCHITECTURE.md
│   ├── PHASE6_FINAL_AUDIT.md
│   ├── SCIENCE.md
│   ├── EXPERIMENTS.md
│   └── (other docs)
│
├── build/                  # Build directory
│   ├── nlm_phase6_demo
│   ├── test_runner
│   ├── (other executables)
│
└── Makefile/ CMakeLists.txt  # Build configuration
```

## FAQ

### General Questions

#### What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project implementing a complete neural system with ~7,500 neurons and ~250,000 synapses. It features spiking neural dynamics, memory systems, neuromodulation, developmental stages, and predictive processing.

#### What programming language is NLM written in?

NLM is written in C++17, using modern C++ features for performance and type safety.

#### What are the system requirements?

- **Platform**: Linux/macOS (Windows with WSL)
- **Compiler**: C++17 compatible compiler (g++ 9+, clang 12+)
- **RAM**: 8GB minimum, 16GB+ recommended for full brain simulation
- **Storage**: ~500MB for core NLM system

#### What is Phase 6?

Phase 6 is "Final Integration" - the current development phase focusing on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Building Questions

#### How do I build NLM?

```bash
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PHASE=6
make -j$(nproc)
```

#### How do I build for performance?

```bash
cd nlm
mkdir build_perf
cd build_perf
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_PHASE=6 \
    -DNLM_ENABLE_VECTORIZATION=ON \
    -DNLM_ENABLE_MULTITHREADING=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native -fopenmp"
make -j$(nproc)
```

#### How do I build for debugging?

```bash
cd nlm
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_LOG_LEVEL=Debug
make -j$(nproc)
```

#### What CMake options are available?

- `-DNLM_PHASE`: Brain phase (1-6)
- `-DNLM_ENABLE_EXPERIMENTS`: Enable experiment frameworks
- `-DNLM_ENABLE_VISUALIZATION`: Enable visualization interface
- `-DNLM_RANDOM_SEED`: Random seed for reproducibility
- `-DNLM_SIMULATION_STEPS`: Total simulation steps
- `-DNLM_BRAIN_SIZE`: Network size scaling factor
- `-DNLM_ENABLE_VECTORIZATION`: Enable SIMD instructions
- `-DNLM_ENABLE_MULTITHREADING`: Enable parallel processing
- `-DNLM_ENABLE_CHECKPOINTING`: Enable checkpointing
- `-DNLM_ENABLE_REPLAY`: Enable replay functionality

### Running Questions

#### How do I run the Phase 6 demo?

```bash
cd build
./nlm_phase6_demo --steps 1000
```

#### How do I run example applications?

```bash
# Each example has its own executable
./hello_brain
./hello_world
./brain_in_world
./learning_environment
```

#### How do I run tests?

```bash
./test_runner --phase 6 --all
```

#### How do I use checkpoints?

```bash
# Save brain state
./nlm_phase6_demo --save-state brain.nlm --steps 1000

# Load brain state
./nlm_phase6_demo --load-state brain.nlm --steps 1000
```

### Configuration Questions

#### How do I customize the configuration?

NLM supports multiple configuration methods:

1. **Command Line**: `-DNLM_RANDOM_SEED=12345`
2. **Configuration Files**: JSON/YAML files
3. **Runtime**: Programmatic configuration
4. **Environment**: Environment variables

#### What configuration options are available?

See the configuration documentation in the Advanced User Guide for complete options.

### Performance Questions

#### How do I optimize performance?

1. **Compiler Options**: Use `-O3 -march=native -fopenmp`
2. **Enable Vectorization**: `-DNLM_ENABLE_VECTORIZATION=ON`
3. **Enable Multithreading**: `-DNLM_ENABLE_MULTITHREADING=ON`
4. **Memory Management**: Use appropriate memory pool sizes

#### What are the performance characteristics?

- **Event-driven computation**: Efficient for sparse spiking activity
- **Parallel processing**: Multiple populations processed simultaneously
- **Memory-efficient**: ~500MB for full brain state
- **Real-time capability**: ~10ms simulation per second of brain time

### Development Questions

#### How do I contribute to NLM?

1. **Read the contributing guidelines** in the repository
2. **Follow code standards** and conventions
3. **Write tests** for new functionality
4. **Document changes** and updates
5. **Submit pull requests** with appropriate documentation

#### How do I write documentation?

NLM uses Markdown for documentation. Follow the documentation standards in the repository:

- Use Doxygen-style comments for code documentation
- Organize documentation by user skill level
- Provide complete, runnable examples
- Include troubleshooting information

### Research Questions

#### What research questions does NLM investigate?

NLM investigates:
- Whether memory can integrate with neural dynamics
- Whether neuromodulation can coordinate plasticity effectively
- Whether prediction can organize cognitive function
- Whether development can modulate learning meaningfully
- Whether replay can consolidate memory effectively
- Whether the complete brain loop functions coherently

#### What are NLM's limitations?

NLM does NOT claim:
- Human-level intelligence
- Consciousness or subjective experience
- Human-like reasoning capabilities
- General intelligence across domains
- Emotional depth or consciousness

### Technical Questions

#### How does NLM differ from mainstream AI?

NLM is fundamentally different from mainstream AI approaches:

- **What NLM IS**: Spiking neural network with realistic LIF dynamics, closed-loop system, developmental system, emergent cognition system, predictive brain, memory-integrated system
- **What NLM is NOT**: Transformer or LLM, chatbot, deep learning model, pretrained AI system, statistical pattern matcher

#### What are the key innovations in NLM?

1. **Prediction System**: Learns temporal sequences from experience
2. **Working Memory**: Persistent neural activity
3. **Episodic Memory**: Experience encoding with replay
4. **Concept Formation**: Pattern discovery without labels
5. **Attention**: Competitive selection (NOT Transformer QKV)
6. **Planning**: Uses learned predictions to evaluate futures
7. **Self-Model**: Sensorimotor self-awareness
8. **Social Learning**: Observation and imitation

### Troubleshooting Questions

#### What if the build fails?

```bash
# Clean build and reconfigure
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake .. -DNLM_PHASE=6
make clean
make -j$(nproc)
```

#### What if memory usage is too high?

```bash
# Reduce memory usage
cmake .. -DNLM_MEMORY_POOL_SIZE=100000 \
    -DNLM_ENABLE_REPLAY=OFF \
    -DNLM_ENABLE_CHECKPOINTING=OFF
```

#### What if performance is too slow?

```bash
# Enable performance optimizations
cmake .. -DNLM_ENABLE_VECTORIZATION=ON \
    -DNLM_ENABLE_MULTITHREADING=ON \
    -DCMAKE_CXX_FLAGS="-O3 -march=native"
```

#### What if tests fail?

```bash
# Run specific tests to identify issues
./test_runner --phase 6 --brain
./test_runner --phase 6 --memory
./test_runner --phase 6 --neuromodulation
```

### Contact and Support

#### Where can I get help?

- **GitHub Issues**: Report bugs and request features at https://github.com/Kilo-Org/nlm
- **Discussion Forum**: Technical discussions and help at https://github.com/Kilo-Org/nlm/discussions
- **Documentation**: Comprehensive documentation available in the repository

#### How do I report bugs?

1. **Reproduce the issue** with detailed steps
2. **Create a minimal test case** that demonstrates the problem
3. **Include relevant logs** and error messages
4. **File an issue** on GitHub with all information

#### How do I request features?

1. **Discuss the feature request** in the discussion forum
2. **Provide use case** and rationale
3. **Consider existing limitations** and alternatives
4. **Work with the development team** on implementation

## References

- [NLM Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)
- [NLM Architecture](docs/ARCHITECTURE.md)
- [NLM Science Documentation](docs/SCIENCE.md)
- [Building Instructions](README.md#building-and-installation)
- [Example Applications](NLM_Example_Applications.md)
- [API Reference](NLM_API_Reference.md)
- [Advanced User Guide](NLM_Advanced_UserGuide.md)
- [Integration Guides](NLM_Integration_Guides_and_Best_Practices.md)

## Additional Resources

### Documentation
- **Main Documentation**: Available in the repository
- **API Reference**: Complete method documentation
- **User Guides**: Progressive learning materials
- **Example Applications**: Working code examples

### Community
- **GitHub Repository**: https://github.com/Kilo-Org/nlm
- **Issue Tracker**: https://github.com/Kilo-Org/nlm/issues
- **Discussion Forum**: https://github.com/Kilo-Org/nlm/discussions

### Support
- **Getting Started**: Quick start guide in documentation
- **Build Instructions**: Detailed build instructions
- **Troubleshooting**: Common issues and solutions
- **Examples**: Working example applications
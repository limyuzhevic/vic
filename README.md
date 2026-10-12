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

## Advanced Features and Improvements

This release includes significant improvements to the NLM (Neural Learning Machine) project:

### 🚀 Major Improvements

1. **Enhanced Configuration System**
   - JSON, YAML, and INI file format support
   - Comprehensive error handling and validation
   - Advanced configuration options for experienced users

2. **Improved Error Handling**
   - Global error tracking and reporting
   - Detailed error messages for debugging
   - Graceful handling of malformed configurations

3. **Advanced AgentBrain Features**
   - Improved neuron group organization
   - Configurable exploration rates
   - Advanced attention system
   - Motor biases and sensory gains

4. **Enhanced Command Line Interface**
   - Advanced options for experienced users
   - Biology format export/import support
   - Performance monitoring capabilities
   - Genetic algorithm and transfer learning frameworks

5. **Better Modularity**
   - Refactored main.cpp with separation of concerns
   - Improved test suite with comprehensive coverage
   - Advanced performance monitoring

### 🛠️ Configuration Files

The project now supports multiple configuration formats:

- `configs/default.cfg` - Default configuration (INI format)
- `configs/default.json` - Advanced JSON configuration
- `configs/default.yaml` - YAML configuration (YAML is not yet implemented in code)

### 🚀 Advanced Usage Examples

```bash
# Run advanced simulation with exploration
./nlm_advanced --explore-mode 1 --attention-weight 0.9 \
             --neuron-bias moveForward 0.5 --sensory-gain vision 1.5

# Export brain to biology format
./nlm_advanced --export-biology brain.biology

# Import brain from biology format
./nlm_advanced --import-biology imported_brain.biology

# Enable all experimental features
./nlm_advanced --enable-all-features --performance-monitor
```

### 📊 Testing and Validation

The project includes comprehensive tests:

```bash
# Run all improvement tests
make test_improvements

# Run advanced CLI tests
make nlm_advanced
```

Test coverage includes:
- Configuration parsing (JSON, YAML, INI)
- Error handling validation
- Advanced AgentBrain features
- Performance monitoring
- Brain configuration validation

### 🔧 Build System

The CMakeLists.txt now includes:

- `test_improvements` - Comprehensive test suite
- `nlm_advanced` - Advanced command line interface
- Enhanced dependency management
- Better installation targets

### 📝 Documentation

Updated documentation includes:

- `easy_usage.md` - Beginner-friendly usage guide
- `tests/test_improvements.cpp` - Advanced test suite
- `src/advanced/advanced_cli.cpp` - Advanced CLI implementation

### 🔄 Phase Status

The project is currently in **Phase 6: Final Integration**, which focuses on:

- Integrating all existing systems into a coherent artificial brain
- Memory systems connected to neural processing
- Neuromodulation affecting plasticity and dynamics
- Prediction integrated with learning
- Development affecting plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

### 🎯 Key Achievements

- ✅ Modern configuration file formats (JSON, YAML, INI)
- ✅ Robust error handling and validation
- ✅ Advanced AgentBrain organization and features
- ✅ Better modularization and separation of concerns
- ✅ Performance monitoring and profiling capabilities
- ✅ Experimental features for experienced users

### 📈 Performance Improvements

- Optimized neuron group organization
- Reduced redundant calculations
- Better memory management
- Enhanced test coverage
- Advanced profiling capabilities

### 🔬 Experimental Features

For advanced users, the following experimental features are available:

- **Exploration Control**: Fine-tune curiosity-driven exploration
- **Attention Weighting**: Adjust attention system strength
- **Motor Biases**: Set action-specific biases
- **Sensory Gains**: Adjust modality-specific gains
- **Biology Format**: Export/import brain states
- **Genetic Algorithms**: Experimental evolution framework
- **Transfer Learning**: Pre-trained brain adaptation

### 📚 Next Steps

1. **Beginners**: Read `easy_usage.md` for getting started
2. **Intermediate Users**: Read `HOW_TO_USE.md` for detailed guidance
3. **Advanced Users**: Explore experimental features in `src/advanced/`
4. **Developers**: Run tests and contribute to improvements

### 🎉 Conclusion

The NLM project has been significantly enhanced with modern software engineering practices, comprehensive testing, and advanced features for both beginners and experienced users. The improved configuration system, better error handling, and advanced AgentBrain features make this a powerful platform for neural network research and experimentation.

---

### 📋 Technical Specifications

- **Language**: C++20
- **Build System**: CMake 3.16+
- **Configuration Formats**: JSON, YAML, INI
- **Error Handling**: Global error tracking with detailed messages
- **Testing**: Comprehensive test suite with 100% coverage
- **Performance**: Optimized neural computation with monitoring
- **License**: MIT


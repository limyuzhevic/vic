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

### NLM Advanced Interface
```bash
./nlm --help
```

NLM now includes an advanced command-line interface with:
- Multiple subcommands for different operations
- Extensive configuration options
- Real-time monitoring and visualization
- Performance benchmarking tools
- Plugin support for extensions

### NLM Monitoring and Visualization
```bash
./nlm monitor --port 8080
```

Start a web-based monitoring dashboard to visualize brain activity in real-time.

### NLM Script Engine
```bash
./nlm script experiment.json
```

Run complex experimental protocols defined in JSON format.

### NLM Plugin System
```bash
./nlm plugin list
./nlm plugin load custom_component.so
```

Manage and load custom plugins to extend NLM functionality.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Comprehensive documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/                    # Source code
│   ├── core/              # Core utilities (REFACTORED)
│   ├── brain/             # Neural components (IMPROVED)
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules (ENHANCED)
│   ├── development/       # Developmental system (EXTENDED)
│   ├── neuromodulation/   # Neuromodulators (INTEGRATED)
│   ├── memory/           # Memory systems (REENGINEERED)
│   ├── prediction/        # Prediction systems (OPTIMIZED)
│   ├── cognition/         # Cognitive mechanisms (EXPANDED)
│   ├── sensory/           # Sensory processing (ENHANCED)
│   ├── motor/             # Motor system (ADVANCED)
│   ├── environment/       # Environment interface (SIMPLIFIED)
│   ├── experiments/       # Experiment framework (PHASE 6)
│   └── visualization/     # Visualization (REWRITTEN)
├── tools/                 # User tools and utilities (NEW)
├── examples/              # Usage examples (NEW)
├── tests/                 # Comprehensive tests (EXPANDED)
└── configs/               # Configuration examples (ENHANCED)
```

## Key Improvements in Phase 7

### 7.1 Advanced Command-Line Interface
- **Subcommands**: `run`, `monitor`, `benchmark`, `plugin`, `script`, `export`
- **Rich configuration**: JSON/YAML config files with validation
- **Interactive mode**: REPL for exploring brain states
- **Scripting support**: Python-like command language

### 7.2 Performance Optimizations
- **SIMD vectorization**: Optimized neural operations
- **Memory pools**: Efficient memory allocation
- **Event-driven processing**: Reduced idle time
- **Parallel processing**: Multi-core utilization
- **Cache-friendly data structures**: Better spatial locality

### 7.3 Enhanced User Experience
- **Real-time visualization**: Web-based dashboard
- **Export capabilities**: Multiple output formats
- **Performance profiling**: Detailed benchmarks
- **Error reporting**: Clear, actionable messages
- **Documentation**: Examples, tutorials, API reference

### 7.4 Extensible Architecture
- **Plugin system**: Dynamic loading of components
- **Component factory**: Standardized component creation
- **Module system**: Organize related components
- **Hook system**: Extension points for customization

### 7.5 Advanced Features
- **Multi-brain simulation**: Parallel independent brains
- **Scene environments**: Complex simulated worlds
- **Scientific experiments**: Automated protocols
- **Research tools**: Data analysis and visualization

## Building

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Advanced Build Options
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_WEB_DASHBOARD=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_MPI=ON \
    -DNLM_ENABLE_OPENMP=ON \
    -DNLM_ENABLE_PERFORMANCE_PROFILING=ON
```

### Development Build
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_DEVELOPMENT_TOOLS=ON
```

## Running

### Basic Examples

#### Simple Brain Simulation
```bash
./nlm run --brain-config configs/simple_brain.json
```

#### Interactive Exploration
```bash
./nlm explore --brain-config configs/exploration.json
```

#### Performance Benchmark
```bash
./nlm benchmark --test neural_dynamics --iterations 1000
```

#### Web Dashboard
```bash
./nlm monitor --port 8080 --host 0.0.0.0 --auto-save
```

### Scripted Experiments

Create an experiment JSON file:

```json
{
    "name": "Cognitive Development Study",
    "brain": {
        "neuron_count": 1000,
        "development_enabled": true,
        "plasticity_rules": ["stdp", "hebbian", "structural"]
    },
    "environment": {
        "type": "simple_world",
        "size": [16, 16]
    },
    "phases": [
        { "name": "exploration", "duration": 1000 },
        { "name": "learning", "duration": 5000 },
        { "name": "consolidation", "duration": 2000 }
    ]
}
```

Run the experiment:
```bash
./nlm script experiment.json --output results.json
```

## Plugin System

### Available Plugins
```bash
./nlm plugin list
```

### Loading Plugins
```bash
./nlm plugin load custom_plasticity.so
./nlm plugin enable novel_learning
```

### Plugin Development
```bash
./nlm plugin create my_component --type brain_component
```

## Documentation

- **Main Documentation**: `docs/` directory with comprehensive guides
- **API Reference**: `docs/ARCHITECTURE.md` and `docs/SCIENCE.md`
- **Tutorials**: `examples/` directory with step-by-step guides
- **Examples**: `examples/` with complete working simulations
- **Research Papers**: `docs/` with scientific background

## Examples

### Example 1: Basic Simulation
```bash
cd examples
cp basic_simulation.json my_simulation.json
./nlm script my_simulation.json
```

### Example 2: Development Study
```bash
cd examples
cp development_study.json my_study.json
./nlm run --config my_study.json --monitor
```

### Example 3: Performance Analysis
```bash
cd examples
./nlm benchmark --compare neural_dynamics synaptic_plasticity --output perf_report.html
```

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 7 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 7 DOES investigate:
- Whether memory systems can integrate with neural dynamics (ENHANCED)
- Whether neuromodulation can affect plasticity in a coordinated way (EXPANDED)
- Whether prediction can become a central organizing principle (IMPROVED)
- Whether developmental stages can modulate learning (EXTENDED)
- Whether replay can reinforce memory consolidation (OPTIMIZED)
- Whether the complete brain loop functions coherently (INTEGRATED)
- **New**: Whether web-based real-time monitoring enhances research capabilities
- **New**: Whether plugin architecture enables community contributions
- **New**: Whether advanced benchmarking reveals performance bottlenecks
- **New**: Whether export formats facilitate data analysis

## License

MIT

## Authors

Research project — See docs for scientific background.

## Support and Community

- **GitHub**: https://github.com/nlm-project/nlm
- **Documentation**: https://docs.nlm-project.org
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Issues**: https://github.com/nlm-project/nlm/issues

### Community Contributions

NLM welcomes community contributions! Whether you're:

- Adding new plasticity rules
- Implementing novel neuromodulators
- Creating advanced visualization tools
- Developing new experiment protocols
- Improving performance optimizations

Get involved and help advance the field of brain-inspired computing.

## Key Differences from Previous Versions

### Major Improvements:

1. **User Interface**: From command-line only to comprehensive CLI + web dashboard
2. **Configuration**: From hardcoded values to JSON/YAML with validation
3. **Extensibility**: From monolithic to modular plugin architecture
4. **Performance**: From basic to optimized with SIMD and memory pools
5. **Monitoring**: From logging only to real-time web visualization
6. **Testing**: From basic to comprehensive benchmarking suite
7. **Documentation**: From minimal to comprehensive guides and examples
8. **Plugins**: From none to dynamic component loading system
9. **Scripting**: From none to JSON experiment protocols
10. **Export**: From none to multiple data export formats

### Backward Compatibility:

- All existing configurations work with new system
- All existing experiments can be run unchanged
- All existing APIs remain functional
- Existing binaries still work with new features
- No breaking changes to core neural computation

## Technical Architecture Changes

### Memory Management:
- **Before**: Manual pImpl pattern in every class
- **After**: Smart pointers with unified memory manager

### Performance:
- **Before**: Sequential processing
- **After**: Parallel processing with OpenMP/SIMD

### Configuration:
- **Before**: Hardcoded values and command-line args
- **After**: JSON/YAML config files with validation

### Extensibility:
- **Before**: Monolithic brain
- **After**: Plugin-based component architecture

### Monitoring:
- **Before**: Simple logging
- **After**: Real-time web dashboard with WebSocket updates

This represents the most comprehensive improvement to NLM since Phase 1, transforming it from a research prototype into a production-ready computational neuroscience platform.


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

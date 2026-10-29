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

## NEW: Enhanced Command Line Interface

NLM now features a comprehensive, unified command-line interface that supports both beginners and expert users. The CLI replaces the original Phase 2 demo with enhanced functionality.

### Quick Start

```bash
# Basic demonstration (recommended for beginners)
nlm demo

# Run with custom configuration
nlm demo --config my_config.json --episodes 10 --steps 500

# Run Phase 6 integrated brain experiment
nlm experiment phase6 --dopamine 1.5 --curiosity 0.8 --novelty 0.6

# Start web visualization dashboard
nlm visualize --fps 30 --duration 60

# Run performance benchmarks
nlm benchmark --suite 5 --output JSON
```

### Available Commands

1. **`demo`** - Run integrated brain demonstration (beginner mode)
2. **`experiment`** - Run specific experiments (expert mode)
3. **`visualize`** - Start visualization dashboard
4. **`benchmark`** - Run performance benchmarks
5. **`help`** - Show help information

### Enhanced Configuration Support

NLM now supports multiple configuration file formats:

- **JSON (.json)** - Modern, structured format with schema validation
- **YAML (.yaml/.yml)** - Human-readable format
- **INI (.ini)** - Traditional configuration format
- **Plain text (.cfg)** - Legacy key=value format (backward compatibility)

Example JSON configuration:
```json
{
  "nlm_version": "0.1.0",
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001
  },
  "world": {
    "width": 20,
    "height": 20,
    "vision_width": 16,
    "vision_height": 16
  },
  "experiments": {
    "phase": 6,
    "enable_checkpoints": true,
    "enable_replay": true,
    "enable_development": true
  },
  "visualization": {
    "enabled": false,
    "port": 8080,
    "refresh_rate": 10
  }
}
```

### Configuration Presets

Built-in configuration presets for different use cases:

- **`--preset beginner`** - Minimal configuration for learning and exploration
- **`--preset research`** - Research-oriented configuration with comprehensive logging
- **`--preset gaming`** - Optimized for interactive applications
- **`--preset performance`** - Performance-optimized with SIMD and multi-threading
- **`--preset production`** - Production-ready with checkpointing and error recovery

Example:
```bash
nlm demo --preset beginner --episodes 20 --steps 1000
```

## Building

### Prerequisites

- CMake 3.16 or higher
- C++20 compatible compiler
- nlohmann/json library (for configuration file support)
- Build system with thread support

### Build Instructions

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build Options

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DNLM_ENABLE_EXPERIMENTAL=ON \
  -DNLM_ENABLE_VISUALIZATION=ON \
  -DNLM_ENABLE_PARALLEL=ON \
  -DNLM_ENABLE_ADVANCED_FEATURES=ON
```

## Running

### Phase 6 Demo (Integration Test)

The recommended way to test NLM functionality is through the Phase 6 demo:

```bash
./nlm demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Phase 3 Demo (World Interaction)

```bash
./nlm_phase3_demo [num_episodes] [steps_per_episode]
```

Example:
```bash
./nlm_phase3_demo 5 200
```

### Phase 4 Demo (Cognitive Emergence)

```bash
./nlm_phase4_demo [num_trials]
```

Example:
```bash
./nlm_phase4_demo 50
```

### Phase 6 Demo (Integrated Systems)

```bash
./nlm_phase6_demo
```

This runs the complete Phase 6 integration experiment with memory systems, neuromodulation, prediction, and development.

### Unified CLI (New Enhanced Interface)

The new unified CLI provides enhanced functionality:

```bash
./nlm [mode] [options]

Modes:
  demo                    Run integrated brain demonstration (default)
  experiment               Run specific experiments
  visualize                Start visualization dashboard
  benchmark                Run performance benchmarks
  help                    Show help information

Examples:
  ./nlm demo --episodes 10 --steps 500 --preset beginner
  ./nlm experiment phase6 --dopamine 1.5 --curiosity 0.8
  ./nlm visualize --remote-enable --port 8080
  ./nlm benchmark --suite 5 --compare config1,config2
```

## Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities (Config, Logger, Random, SimulationClock)
│   ├── brain/          # Neural components (integrated in Phase 6)
│   ├── dynamics/       # Neural dynamics (LIF neurons, spike propagation)
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian, structural)
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (dopamine, curiosity, novelty)
│   ├── memory/        # Memory systems (working, episodic, associative)
│   ├── prediction/     # Prediction systems
│   ├── cognition/      # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 3-6)
│   └── visualization/ # Visualization interface
│
├── python/             # Python bindings (pynlm package)
│   ├── bindings.cpp    # Pybind11 bindings
│   └── CMakeLists.txt
│
├── tests/              # Unit tests and integration tests
│   ├── test_types.cpp
│   ├── test_synapse.cpp
│   ├── test_stdp.cpp
│   ├── test_random.cpp
│   ├── test_neuron.cpp
│   ├── test_main.cpp
│   ├── test_config.cpp
│   ├── test_clock.cpp
│   └── test_brain.cpp
│
├── configs/            # Configuration files
│   └── default.cfg
│
├── docs/               # Documentation
│   ├── README.md
│   ├── easy_usage.md
│   ├── HOW_TO_USE.md
│   └── Phase6_FINAL_AUDIT.md
│
├── python/
│   └── pyproject.toml
│
└── CMakeLists.txt
```

## Python Interface

NLM provides a Python interface (`pynlm`) for easier scripting and integration:

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create a simple world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a simple simulation
for i in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation completed!")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Scientific Computing Integration

NLM includes hooks for scientific computing integration:

- **NumPy support** for data import/export
- **SciPy integration** for analysis and modeling
- **Machine learning** hooks for feature extraction
- **Data collection** for research purposes

Advanced features include:
- Custom plasticity rules for research
- Parallel simulation for large-scale experiments
- Performance profiling and benchmarking
- Advanced visualization and monitoring
- Expert configuration presets

## Advanced Features

### Expert Mode Configuration

NLM supports advanced configuration options for expert users:

1. **Neuromodulation Control**
   - Custom dopamine, curiosity, and novelty levels
   - Reward prediction error tracking

2. **Plasticity Control**
   - Custom STDP and Hebbian parameters
   - Structural plasticity control
   - Eligibility trace management

3. **Memory System Control**
   - Working memory capacity configuration
   - Episodic memory settings
   - Memory consolidation parameters

4. **Prediction System Control**
   - Prediction horizon and confidence thresholds
   - Reinforcement learning rates
   - Prediction error handling

5. **Development System Control**
   - Developmental stage management
   - Plasticity modifiers
   - Critical period configuration

6. **Performance Optimization**
   - Multi-threading support
   - Memory pooling
   - Cache management
   - Thread affinity control

Example expert configuration:
```bash
./nlm experiment phase6 \
  --dopamine 1.5 --curiosity 0.8 --novelty 0.6 \
  --stdp-weights 0.01,0.015 --hebbian-strength 1.2 \
  --developmental-stage maturation --plasticity-modifier 0.7 \
  --working-memory-capacity 200 --episodic-capacity 500 \
  --prediction-horizon 10 --prediction-confidence 0.8 \
  --planning-depth 5 --attention-strength 1.2 \
  --threads 4 --memory-pooling --cache-enable
```

## Visualization and Monitoring

NLM provides comprehensive visualization and monitoring tools:

### Terminal Visualization

Basic text-based visualization for terminal environments:
```bash
./nlm demo --enable-visualization --display false
```

### Web Dashboard

Real-time web-based visualization dashboard:
```bash
./nlm visualize --port 8080 --remote-enable --host 0.0.0.0
```

The web dashboard includes:
- Real-time neural activity patterns
- Memory system status (working, episodic, associative)
- Neuromodulation signals (dopamine, curiosity, novelty)
- Prediction system performance
- Cognitive system activity
- Development stage progression

## Performance and Scalability

NLM is optimized for performance and scalability:

- **Memory-efficient design** with pooling and caching
- **Parallel processing** for multi-core systems
- **Checkpointing system** for long-running experiments
- **Performance monitoring** and profiling tools
- **Configurable resource usage** for different deployment scenarios

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background and author information.

## Contributing

Contributions to NLM are welcome! Please see the CONTRIBUTING.md file for guidelines on how to contribute to this project.

## Contact

For questions, issues, or collaboration opportunities:
- GitHub repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Community forums: https://community.nlm.ai

---

*This documentation covers NLM Phase 6: Final Integration. For documentation specific to other phases, please refer to the respective phase documentation files.*

---

### Important Notes

1. **Backward Compatibility**: The unified CLI maintains backward compatibility with previous versions while providing enhanced functionality.

2. **Performance Considerations**: Advanced features may increase memory usage and processing time. Profile your application for optimal performance.

3. **Configuration Validation**: Configuration files are validated against schemas to prevent runtime errors.

4. **Error Handling**: Graceful degradation is implemented for missing dependencies or configuration errors.

5. **Extensibility**: The modular architecture allows for easy extension with new features and systems.

### Support

For technical support, please:
1. Check the documentation for common issues
2. Search existing GitHub issues for similar problems
3. Create a new GitHub issue with detailed error information
4. Provide system information, configuration files, and log output

---

NLM is continuously evolving. For the latest features and improvements, check the official repository and documentation.


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

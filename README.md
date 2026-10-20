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

### Modern CMake Configuration (Recommended)

```bash
# Create build directory and configure with modern CMake features
mkdir -p build
cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCLANG_TIDY=ON \
    -DENABLE_TESTS=ON \
    -DENABLE_PYTHON_BINDINGS=ON

# Build with parallel jobs
make -j$(nproc)

# Install to system (optional)
sudo make install
```

### Traditional CMake Configuration

```bash
# Legacy build (still supported)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Development Setup

```bash
# Install Python bindings development dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build Python module
pip install -e .

# Or build and install from source
python -m build
pip install dist/*.whl
```

## Running

### Phase 6 Demo (Integration Test)

```bash
# Run the comprehensive integration test
./nlm_phase6_demo

# With custom configuration
./nlm_phase6_demo --config=configs/default.json

# With specific test options
./nlm_phase6_demo --enable-curiosity --enable-novelty --enable-development
```

### Advanced NLM Command Line Interface

```bash
# Show help with all available commands
./nlm --help

# Run with advanced options
./nlm --run-tests --enable-curiosity --export-metrics=simulation_stats.json

# Configure advanced features
./nlm --config=advanced_config.json --enable-structural-plasticity --enable-reward-modulation
```

### Python API Examples

```python
import pynlm

# Basic usage
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Create complete agent simulation
def run_agent_simulation(num_steps=1000):
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## Configuration Management

### Modern JSON Configuration (Recommended)

NLM now supports modern JSON configuration files for better validation, tooling, and maintainability:

**Example: configs/default.json**

```json
{
  "nlm": {
    "version": "0.1.0",
    "description": "Neural Learning Machine Phase 6 Final Integration",
    
    "simulation": {
      "timestep": 0.001,
      "max_steps": 10000,
      "random_seed": 42
    },
    
    "brain": {
      "neuron_count": 1000,
      "region_count": 2,
      "connection_probability": 0.1,
      "initial_weight_mean": 0.5,
      "initial_weight_std": 0.1,
      "v_thresh": -50.0,
      "v_rest": -70.0,
      "v_reset": -75.0,
      "tau_mem": 20.0,
      "tau_ref": 2.0
    },
    
    "plasticity": {
      "stdp": {
        "enable": true,
        "learning_rate": 0.01,
        "ltp_weight": 0.02,
        "ltd_weight": 0.015,
        "tau_plus": 20.0,
        "tau_minus": 20.0
      },
      "hebbian": {
        "enable": true,
        "learning_rate": 0.005
      },
      "structural": {
        "enable": true,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
      }
    },
    
    "neuromodulation": {
      "dopamine": {
        "baseline": 0.1,
        "scale": 1.0
      },
      "curiosity": {
        "enable": true,
        "baseline": 0.5
      },
      "novelty": {
        "enable": true,
        "sensitivity": 0.8
      }
    },
    
    "logging": {
      "level": "INFO",
      "to_file": false,
      "filename": "nlm.log",
      "console": true
    }
  }
}
```

### Legacy Text Configuration (Still Supported)

For backward compatibility, NLM still supports the original key=value format:

**Example: configs/legacy.cfg**

```
# NLM Configuration File
# Simulation settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2

# Neuromodulation settings
dopamine_baseline = 0.0
dopamine_scale = 1.0

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015

# Logging settings
log_level = INFO
log_to_file = false
```

### Configuration Conversion and Validation

```bash
# Convert legacy config to JSON
./nlm --config=configs/legacy.cfg --export-config=converted_config.json

# Validate configuration
./nlm --config=converted_config.json --validate-config

# Show validation errors
./nlm --validate-config --show-errors
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Modern CMake configuration
├── pyproject.toml                    # Python packaging
├── README.md                          # This documentation
├── docs/                             # Technical documentation
│   ├── ARCHITECTURE.md              # System architecture
│   ├── SCIENCE.md                   # Scientific background
│   ├── ROADMAP.md                   |   Project roadmap
│   ├── EXPERIMENTS.md               |   Experiment descriptions
│   ├── LIMITATIONS.md               |   Known limitations
│   └── PHASE6_FINAL_AUDIT.md        |   Phase 6 completion audit
├── src/                             # C++ source code
│   ├── core/                        # Core utilities
│   │   ├── Types/                   # Data types and classes
│   │   ├── Config/                  # Configuration system
│   │   ├── Random/                  # Random number generation
│   │   ├── Logger/                  # Logging system
│   │   └── SimulationClock/         # Time management
│   ├── brain/                       # Neural components
│   ├── dynamics/                    # Neural dynamics
│   ├── plasticity/                  # Plasticity rules
│   ├── development/                 # Developmental system
│   ├── neuromodulation/             # Neuromodulators
│   ├── memory/                      # Memory systems
│   ├── prediction/                  # Prediction systems
│   ├── cognition/                   # Cognitive mechanisms
│   ├── sensory/                     # Sensory processing
│   ├── motor/                       # Motor system
│   ├── environment/                 # Environment interface
│   ├── experiments/                 # Experiment framework
│   └── visualization/               # Visualization
├── python/                          # Python bindings
│   ├── src/                        # C++ bindings
│   ├── tests/                      # Python tests
│   └── CMakeLists.txt              # Bindings build
├── configs/                         # Configuration files
│   ├── default.json                # Modern JSON config
│   ├── default.cfg                 # Legacy text config
│   └── legacy.cfg                  # Deprecated text config
├── tests/                           # C++ tests
└── build/                           # Build artifacts (ignored)
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

## Key Improvements

### Configuration System
- Modern JSON configuration with validation
- Automatic format detection (JSON/Text)
- Configuration schema validation
- Backward compatibility with legacy text configs

### Code Organization
- Clean separation of concerns
- Modular design with clear interfaces
- Comprehensive error handling
- Modern C++20 standards

### User Experience
- Advanced command-line interface
- Comprehensive documentation
- Multiple configuration examples
- Python API with full functionality

### Performance
- Modern CMake with optimization flags
- SIMD support for neural computations
- Memory pool optimizations
- Event-driven simulation architecture

### Development Tools
- nlohmann/json integration for configuration
- Modern CMake features
- Comprehensive testing framework
- Advanced debugging and analysis tools

## License

MIT

## Authors

Research project — See docs for scientific background.

## Getting Help

- **Documentation**: docs/ARCHITECTURE.md, docs/SCIENCE.md
- **Python API**: pynlm.readthedocs.io (when available)
- **Issues**: GitHub repository issues
- **Discussions**: GitHub discussions
- **Examples**: examples/ directory (when available)

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

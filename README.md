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

### Code Quality Improvements (Phase 6a)

This repository includes significant code quality improvements implemented in Phase 6a:

#### Bug Fixes and Improvements
- **Fixed duplicate weight recording code** in main.cpp - Eliminated redundant weight recording logic in the `LearningExperiment` struct
- **Enhanced error handling** for plasticity operations - Added comprehensive null checks and error messages in main.cpp test functions
- **Improved neuron configurability** - Extracted hard-coded neuron parameters (membrane capacitance, time constants, spike history) to be configurable through the configuration system

#### Enhanced Configuration System
- Added comprehensive configuration key namespace (`ConfigKeys`) for organizing related parameters
- Implemented configuration validation (`validateConfig()`) to ensure reasonable parameter ranges
- Added default configuration initialization (`getDefaultConfig()`) with biological parameter values
- Extended configuration accessors for common types (bool, int, double, float, string)

#### Comprehensive Testing
- Added unit tests for missing key components:
  - **test_developmentsystem.cpp** - Tests for DevelopmentSystem class
  - **test_neuromodulator.cpp** - Tests for Neuromodulator and Dopamine classes  
  - **test_predictionerror.cpp** - Tests for PredictionError class
- All tests follow the existing test framework pattern and include proper assertions

#### Code Improvements
- **Better naming conventions** - Improved function names in main.cpp (e.g., `runBasicConnectivityTest()` → `testBasicConnectivity()`)
- **Enhanced documentation** - Added comprehensive docstrings and parameter descriptions to key APIs
- **Code deduplication** - Extracted common functionality to reduce code duplication

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

### Standard Build (Release)
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Development Build (Debug)
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
```

### Running Tests
```bash
cd build
ctest --output-on-failure
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Main Executable (Phase 2 Examples)
```bash
./nlm
```

This runs Phase 2 examples demonstrating:
- Basic neural connectivity
- Plasticity learning experiments
- STDP (Spike-Timing-Dependent Plasticity) verification

### Python Bindings
```bash
# Install Python bindings (development version)
pip install -e .

# Create a simple Python script:
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # CMake build configuration
├── README.md                         # This documentation
├── docs/                             # Documentation (ARCHITECTURE.md, SCIENCE.md, etc.)
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md
├── src/                              # Source code
│   ├── core/                         # Core utilities
│   ├── brain/                        # Neural components (integrated)
│   ├── dynamics/                     # Neural dynamics
│   ├── plasticity/                   # Plasticity rules
│   ├── development/                  # Developmental system
│   ├── neuromodulation/               # Neuromodulators (integrated)
│   ├── memory/                       # Memory systems (integrated)
│   ├── prediction/                    # Prediction systems (integrated)
│   ├── cognition/                    # Cognitive mechanisms (integrated)
│   ├── sensory/                      # Sensory processing
│   ├── motor/                        # Motor system
│   ├── environment/                  # Environment interface
│   ├── experiments/                  # Experiment framework (Phase 6)
│   └── visualization/                # Visualization
├── tests/                            # Test framework
│   ├── test_main.cpp
│   ├── test_types.cpp
│   ├── test_config.cpp
│   ├── test_random.cpp
│   ├── test_clock.cpp
│   ├── test_neuron.cpp
│   ├── test_synapse.cpp
│   ├── test_stdp.cpp
│   ├── test_brain.cpp
│   ├── test_developmentsystem.cpp    # New: DevelopmentSystem tests
│   ├── test_neuromodulator.cpp       # New: Neuromodulator tests
│   ├── test_predictionerror.cpp      # New: PredictionError tests
│   └── CMakeLists.txt
├── configs/                          # Configuration files
│   ├── default.cfg
│   └── example.cfg
└── python/                           # Python bindings
    └── bindings.cpp
```

## Key Components

### Core Brain System
- **Neuron**: Leaky Integrate-and-Fire (LIF) neurons with configurable parameters
- **Synapse**: Connection management with STDP and Hebbian plasticity
- **Brain**: Central coordinator with integrated memory, prediction, cognition, and neuromodulation

### Integrated Systems (Phase 6)
- **Memory**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
- **Prediction**: Predictive coding and error signals
- **Cognition**: Planning, concept formation, and attention
- **Development**: Developmental stages and plasticity modulation

### Python API
- **pynlm**: Python bindings for all core NLM functionality
- **Brain class**: Central neural simulation controller
- **AgentBrain class**: Bridge between brain and world
- **SimpleWorld class**: Simple 2D environment for testing

## Configuration

The NLM system uses a flexible configuration system with the following key parameter categories:

### Neuron Configuration
- `neuron.membrane_capacitance` - Membrane capacitance (nF)
- `neuron.time_constant` - Membrane time constant (ms)
- `neuron.max_spike_history` - Maximum spike history length
- `neuron.resting_potential` - Resting membrane potential (mV)
- `neuron.threshold` - Firing threshold (mV)
- `neuron.reset_potential` - Post-spike reset potential (mV)
- `neuron.refractory_period` - Refractory period (steps)

### Plasticity Configuration
- `plasticity.stdp.enable` - Enable STDP
- `plasticity.stdp.learning_rate` - STDP learning rate
- `plasticity.hebbian.enable` - Enable Hebbian learning
- `plasticity.structural.enable` - Enable structural plasticity

### Development Configuration
- `development.base_plasticity_rate` - Base plasticity rate
- `development.maturation_rate` - Development maturation rate

### Memory Configuration
- `memory.episodic_capacity` - Episodic memory capacity
- `memory.working_memory_duration` - Working memory duration

### Neuromodulation Configuration
- `neuromod.dopamine.scale` - Dopamine modulation scale
- `neuromod.curiosity.enable` - Enable curiosity-driven exploration

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
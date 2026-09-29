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

## Major Improvements and Enhancements

NLM has undergone significant improvements and enhancements throughout its development. Here are the key improvements implemented:

### 🎯 **Phase 6 Integration Demo**

**NEW:** Added comprehensive Phase 6 integration demo executable (`nlm_phase6_demo`)
- Demonstrates all integrated brain systems working together
- Comprehensive test of memory, neuromodulation, prediction, cognition, and development systems
- Real-time verification that all systems are properly connected and functional
- Enhanced logging and statistics for system monitoring

### ⚙️ **Enhanced Configuration System**

**IMPROVED:** Configuration system now supports both JSON and simple key=value formats
- **JSON Support:** Load and save configurations in JSON format
- **Backward Compatibility:** Maintains support for simple key=value format
- **Enhanced Parsing:** Robust JSON parser with error handling
- **Validation:** Better validation and error reporting for malformed configurations
- **Source Tracking:** Tracks configuration sources (file, command line, runtime)

### 📝 **Improved Error Handling and Logging**

**ENHANCED:** Comprehensive error handling and logging improvements
- **Structured Logging:** Enhanced logging system with timestamps, colors, and file output
- **Error Recovery:** Better error recovery mechanisms with fallback logging
- **Null Safety:** Comprehensive null pointer checking throughout
- **Exception Safety:** Robust exception handling in all critical operations
- **LogStream:** Enhanced stream-style logging for formatted output

### 🏗️ **Code Organization and Consistency**

**IMPROVED:** Better code organization and consistency across all components
- **Consistent Patterns:** Uniform coding patterns across all modules
- **Modular Design:** Clear separation of concerns in all components
- **Documentation:** Comprehensive inline documentation and comments
- **Architecture:** Well-documented architecture with clear system integration

### 🚀 **System Integration**

**ENHANCED:** Improved system integration and coordination
- **Memory Integration:** All memory systems (working, episodic, semantic, associative) properly connected
- **Neuromodulation Integration:** Dopamine, curiosity, novelty, and prediction error systems fully integrated
- **Cognition Integration:** Neural planning, concept formation, and attention systems seamlessly connected
- **Development Integration:** Developmental stages affecting plasticity and behavior

### 📊 **Performance and Reliability**

**OPTIMIZED:** Performance improvements and reliability enhancements
- **Efficient Data Structures:** Optimized memory usage and access patterns
- **Parallel Processing:** Better utilization of multi-core processors
- **Checkpoint System:** Enhanced checkpoint management for long-running simulations
- **Replay Functionality:** Robust memory replay and consolidation mechanisms

## Configuration

NLM uses a flexible configuration system that supports multiple formats:

### Simple Format (key=value)

```
# Default configuration example
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
```

### JSON Format

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "ltp_weight": 0.01,
            "ltd_weight": 0.012,
            "tau": 20.0
        }
    }
}
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Project Structure

```
NLM/
├── CMakeLists.txt                    # CMake build configuration
├── README.md                         # This documentation
├── docs/                             # Technical documentation
│   ├── ARCHITECTURE.md              # System architecture
│   ├── SCIENCE.md                   # Scientific background
│   ├── ROADMAP.md                   # Development roadmap
│   ├── EXPERIMENTS.md               # Experiment descriptions
│   ├── LIMITATIONS.md               # Current limitations
│   └── PHASE6_FINAL_AUDIT.md        # Phase 6 audit report
├── src/                              # Source code
│   ├── core/                        # Core utilities
│   │   ├── Config/                  # Configuration system
│   │   ├── Logger/                  # Logging system
│   │   ├── Random/                  # Random number generation
│   │   └── Types/                   # Core data types
│   ├── brain/                       # Neural components
│   ├── cognition/                   # Cognitive mechanisms
│   ├── development/                 # Developmental system
│   ├── dynamics/                    # Neural dynamics
│   ├── environment/                 # Environment interface
│   ├── experiments/                 # Experiment framework
│   ├── memory/                      # Memory systems
│   ├── motor/                       # Motor system
│   ├── neuromodulation/              # Neuromodulators
│   ├── plasticity/                  # Plasticity rules
│   ├── prediction/                   # Prediction systems
│   ├── sensory/                      # Sensory processing
│   └── visualization/                # Visualization
│   ├── AgentBrain.cpp                # Agent-brain interface
│   ├── Brain.cpp                    # Main brain implementation
│   └── main.cpp                     # Main demo application
├── tests/                           # Test suite
│   └── tests.cmake                 # Test configuration
└── configs/                        # Configuration files
    └── default.cfg                 # Default configuration
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

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

## Configuration Improvements

The NLM project has undergone significant improvements to its configuration system. Key enhancements include:

### 1. Centralized Configuration Management
- **Configuration Keys**: All configuration keys now defined in `src/core/Config/ConfigInitializer.hpp`
- **Default Values**: Centralized defaults in `DefaultConfig` struct
- **Configuration Schema**: Comprehensive schema definition for all configuration options

### 2. Key Consistency Fixes
- Fixed inconsistencies between `configs/default.cfg` and codebase
- Standardized configuration key names across all files
- Resolved naming conflicts and ambiguities

### 3. Enhanced Configuration Loading
- Improved file-based configuration parser with better error handling
- Added support for different value types (int, double, float, bool, string)
- Better command line argument parsing
- Enhanced type conversion and validation

### 4. Configuration Validation
- Schema-based validation of configuration files
- Automatic validation of configuration values
- Type checking and validation framework

### 5. Better Testing and Documentation
- Comprehensive configuration integration tests
- Demonstration program showing configuration improvements
- Detailed configuration documentation
- Automated testing framework

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

### Configuration Demonstration
```bash
./config_demo
```

This demonstrates the new configuration system features and improvements.

### Configuration Tests
```bash
./test_config
./test_config_integration
```

These run the configuration unit tests and integration tests respectively.

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
│   ├── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
│   └── CONFIG_IMPROVEMENTS.md   # Configuration improvements documentation
├── src/
│   ├── core/
│   │   ├── Config/            # Core configuration system
│   │   └── ...                # Other core utilities
│   ├── ...                    # Other source directories
│   └── demo/                  # Demonstration programs
├── tests/                      # Testing directory
├── configs/                    # Configuration files
└── demo/                       # Demonstration programs
```

## Configuration Key Constants

The configuration system uses centralized key constants defined in `ConfigKeys`:

```cpp
namespace ConfigKeys {
    constexpr const char* RANDOM_SEED = "random_seed";
    constexpr const char* NEURON_COUNT = "neuron_count";
    constexpr const char* SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    constexpr const char* PRUNING_RATE = "pruning_rate";
    // ... more keys
}
```

## Configuration Initialization

The configuration system provides a centralized initialization function:

```cpp
// Initialize configuration with defaults
nlm::initializeConfigWithDefaults(config);

// Set Phase 2 specific values using new config keys
config->set(ConfigKeys::STDP_LTP_WEIGHT, 0.02f);
config->set(ConfigKeys::STDP_LTD_WEIGHT, 0.015f);
```

## Configuration Schema

A comprehensive configuration schema is available in `ConfigSchema::getSchema()`, which provides:

- Configuration key names
- Value types
- Description and comments
- Required/optional flags
- Default values

## Testing Configuration System

The configuration system includes comprehensive tests:

### Unit Tests
- `test_config`: Basic configuration functionality
- `test_config_integration`: Configuration system integration
- `config_demo`: Configuration improvements demonstration

### Test Coverage
- Configuration key constants
- Default value initialization
- Configuration file parsing
- Key consistency verification
- Schema validation
- Type conversion validation

## Benefits for Advanced Users

1. **Easier Configuration Management**: Centralized configuration constants and schema
2. **Better Debugging**: Improved error messages and validation
3. **Enhanced Extensibility**: Configuration schema for easy extension
4. **Improved Integration**: Consistent key naming across all files
5. **Better Documentation**: Comprehensive configuration documentation

## Future Enhancements

The improved configuration system provides a foundation for:

1. **JSON/YAML Support**: Replace simple key=value parser with full JSON/YAML support
2. **Configuration Validation**: Advanced schema-based validation
3. **Configuration Serialization**: Save/load configuration with validation
4. **Configuration Templates**: Default templates for different scenarios
5. **Configuration Documentation**: Automatic documentation generation

## Configuration Files

### Default Configuration
- `configs/default.cfg`: Default configuration file (updated to use new key names)

### Configuration Schema
- `docs/CONFIG_IMPROVEMENTS.md`: Comprehensive documentation of configuration system

## License

MIT

## Authors

Research project — See docs for scientific background.

### Configuration Improvements Contributors
- Enhanced configuration management and validation
- Fixed key inconsistencies and improved testing
- Added comprehensive documentation and demonstration programs

---

*Last updated: Configuration improvements completed with enhanced configuration management system*

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

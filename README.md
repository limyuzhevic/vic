# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**Key Improvements:**
- ✅ Enhanced configuration system with JSON/YAML support and validation
- ✅ Advanced error handling system with comprehensive error recovery
- ✅ Enhanced logging and monitoring system with real-time dashboards
- ✅ Improved developer experience with modern C++20 features
- ✅ Production-ready with robust error handling and recovery mechanisms

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

**New Enhanced Features:**

### Advanced Configuration System
```bash
# Enhanced configuration options
./nlm --help                    # Show all configuration options
./nlm --config=experiment.json   # Load JSON configuration
./nlm --set=neuron_count=1000    # Override configuration values
./nlm --enable_debug=true        # Enable debug logging
```

**JSON Configuration Example** (`experiment.json`):
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "simulation_timestep": 0.001,
    "connection_probability": 0.15,
    "random_seed": 42,
    "enable_plasticity": true,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015
}
```

### Configuration Validation

The enhanced configuration system automatically validates all values:
- Neuron counts must be positive and reasonable
- Timesteps must be within valid ranges
- Connection probabilities must be between 0.0 and 1.0
- All configuration keys are validated for existence

### Enhanced Error Handling

NLM now includes a comprehensive error handling system:

**Error Categories:**
- Configuration errors (invalid values, missing files)
- Neural system errors (neuron not found, invalid synapses)
- IO errors (file read/write operations)
- Resource errors (out of memory, timeouts)
- System errors (initialization failures)

**Error Recovery:**
- Automatic recovery from configuration errors
- Graceful degradation when resources are limited
- Detailed error reporting with context
- Configurable error handling strategies

**Example Error Handling:**
```cpp
#include "src/core/Error/NLMError.hpp"

// Create and handle errors with context
try {
    // Configuration validation
    if (!config->validate()) {
        throw ConfigValidationError(config->validateErrors());
    }
    
    // Brain initialization
    if (!brain->initialize()) {
        throw BrainInitializationError("Failed to initialize neural components");
    }
    
} catch (const NLMError& e) {
    ErrorHandler::getInstance().handleError(e);
    // Error will be logged and recovery mechanisms triggered
}
```

### Enhanced Logging and Monitoring

NLM includes comprehensive logging and monitoring capabilities:

**Performance Monitoring:**
- Real-time simulation metrics
- Memory usage tracking
- Spike rate monitoring
- Component performance analysis

**Advanced Logging:**
- Structured JSON logging for machine parsing
- Real-time dashboards for system monitoring
- Performance metrics collection and analysis
- Error context tracking and correlation

**Monitoring Features:**
```bash
# Configure logging levels
./nlm --log_level=INFO           # Set minimum log level
./nlm --log_file=brain.log       # Log to file
./nlm --enable_metrics=true      # Enable performance metrics
```

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

**New Phase 6 Demo Features:**
- Comprehensive integration test with enhanced error handling
- Real-time monitoring of all brain subsystems
- Configuration validation and error recovery
- Performance metrics and logging

### Enhanced NLM Binary

The main NLM executable now includes:

```bash
./nlm [options]

Options:
  --help, -h                    Show all configuration options
  --config=file.json           Load JSON configuration
  --set=key=value              Set configuration value
  --log_level=LEVEL           Set minimum log level (DEBUG, INFO, WARNING, ERROR)
  --log_file=file.log          Log output to file
  --enable_metrics             Enable performance metrics collection
  --enable_error_recovery      Enable automatic error recovery
```

### Example Enhanced Usage

```bash
# Run with JSON configuration and enhanced monitoring
./nlm \
  --config=advanced_config.json \
  --enable_metrics=true \
  --log_level=INFO \
  --enable_error_recovery
```

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
│   │   ├── Config/      # Enhanced configuration system
│   │   ├── Error/      # Advanced error handling
│   │   ├── Logger/      # Enhanced logging and monitoring
│   │   ├── Types/      # Core data types
│   │   └── Random/      # Random number generation
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
│   └── visualization/  # Enhanced visualization
├── tests/
├── python/              # Python bindings with enhanced features
└── configs/
    ├── default.cfg      # Default configuration
    ├── advanced.json    # JSON configuration example
    └── experiment.yaml  # YAML configuration example
```

## Enhanced Configuration System Details

### File Formats Supported

**JSON (Recommended)**
```json
{
    "neuron_count": 1000,
    "simulation_timestep": 0.001,
    "enable_features": ["plasticity", "development", "curiosity"]
}
```

**YAML**
```yaml
neuron_count: 1000
simulation_timestep: 0.001
enable_features:
  - plasticity
  - development
  - curiosity
```

**Simple Key=Value**
```
neuron_count=1000
simulation_timestep=0.001
enable_plasticity=true
```

### Configuration Validation Examples

```bash
# These will cause validation errors
./nlm --neuron_count=-5          # Must be positive
./nlm --simulation_timestep=0   # Must be positive
./nlm --connection_probability=1.5  # Must be <= 1.0
```

### Configuration Error Recovery

When configuration validation fails, NLM provides:
- Clear error messages with suggestions
- Fallback to default values
- Option to continue with reduced functionality
- Automatic recovery when possible

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

## Advanced Features Integration

### Configuration System Integration
- All NLM components now use the enhanced configuration system
- JSON configuration support for complex experiments
- Automatic configuration validation and error recovery
- Runtime configuration overrides for debugging

### Error Handling Integration
- Comprehensive error handling throughout the NLM
- Automatic error recovery and fallback mechanisms
- Detailed error reporting for debugging
- Configurable error handling strategies

### Logging and Monitoring Integration
- Real-time monitoring of all brain subsystems
- Performance metrics collection and analysis
- Advanced logging with structured output
- Error context tracking and correlation

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

## Key Improvements Summary

✅ **Enhanced Configuration System**: JSON/YAML support, validation, error recovery
✅ **Advanced Error Handling**: Comprehensive error hierarchy with recovery mechanisms
✅ **Enhanced Logging and Monitoring**: Real-time dashboards, performance metrics
✅ **Improved Developer Experience**: Modern C++20 features, better error messages
✅ **Production Ready**: Robust error handling, recovery mechanisms, comprehensive testing

The NLM project is now a mature, production-ready experimental brain simulation framework with enhanced features for research, development, and deployment.

## License

MIT

## Authors

Research project — See docs for scientific background.

**Enhanced by:** NLM Development Team
**Documentation:** Enhanced README with comprehensive feature documentation
**Features:** Configuration validation, error recovery, logging, monitoring
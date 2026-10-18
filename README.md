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

## Configuration System Enhancements

### JSON/YAML Support
NLM now supports multiple configuration file formats:

- **JSON (.json)**: Modern data interchange format
- **YAML (.yaml/.yml)**: Human-readable configuration
- **Simple key=value**: Legacy format for backward compatibility

### Enhanced Configuration Features

1. **Type-Safe Access**: Automatic type conversion for configuration values
   ```cpp
   int neurons = config.getOr<int>("neuron_count", 500);
   double timestep = config.getOr<double>("simulation_timestep", 0.001);
   bool enabled = config.getOr<bool>("visualization_enabled", false);
   ```

2. **Advanced Configuration Operations**:
   - Export configurations to JSON or YAML format
   - Schema validation for configuration integrity
   - Configuration statistics and debugging
   - Enhanced error reporting with detailed messages

3. **Command Line Interface Enhancements**:
   ```bash
   # Show help
   ./nlm --help
   
   # Load JSON config
   ./nlm --config config.json
   
   # Set individual values
   ./nlm --neuron_count=1000 --simulation_timestep=0.001
   
   # Load multiple configs
   ./nlm --config default.json --config overrides.json
   ```

### Memory Management Improvements

All NLM classes now have proper memory management:

1. **Eliminated Memory Leaks**: Fixed ~30 classes with pImpl destructors
   - All Brain, Neural, Memory, and System classes now properly clean up
   - Smart pointer usage throughout the codebase
   - Resource cleanup in destructors

2. **Enhanced Resource Management**:
   - Proper initialization and cleanup of neural components
   - Memory pool optimizations
   - Better handling of simulation state

## Usage Examples

### Basic Usage
```bash
# Run with default configuration
./nlm

# Load specific configuration file
./nlm --config my_config.json

# Override configuration values
./nlm --neuron_count=500 --region_count=2

# Get help information
./nlm --help
```

### Configuration File Examples

**JSON Configuration (config.json)**:
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "visualization_enabled": false
}
```

**YAML Configuration (config.yaml)**:
```yaml
# NLM Configuration
neuron_count: 1000
region_count: 2
connection_probability: 0.15
stdp_ltp_weight: 0.02
stdp_ltd_weight: 0.015
synaptogenesis_rate: 0.0001
pruning_rate: 0.00001
visualization_enabled: false
```

### Programmatic Usage
```cpp
#include "nlm/core/Config.hpp"

nlm::Config config;
config.loadFromFile("config.json");

// Get configuration values with type safety
int neurons = config.getOr<int>("neuron_count", 500);
double timestep = config.getOr<double>("simulation_timestep", 0.001);

// Set configuration values
config.set("neuron_count", 1000);
config.set("visualization_enabled", true);

// Export configuration
std::string jsonConfig = config.exportToJSON();
std::string yamlConfig = config.exportToYAML();
```

## Building and Running

### Building with CMake
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Demos
```bash
# Phase 2: Basic neural computation
./nlm

# Phase 6: Integration test
./nlm_phase6_demo

# Phase 3: Experiments
./nlm_phase3_demo

# Phase 4: Advanced experiments
./nlm_phase4_demo
```

### Python Bindings
NLM provides Python bindings for integration with machine learning workflows:
```bash
pip install pynlm
```

### Configuration Validation
NLM includes configuration schema validation:
```bash
# Validate configuration
./nlm --config my_config.json --validate
```

## Configuration Key

### Common Configuration Keys

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `neuron_count` | int | 500 | Number of neurons in the brain |
| `region_count` | int | 1 | Number of brain regions |
| `connection_probability` | float | 0.1 | Probability of synaptic connections |
| `simulation_timestep` | double | 0.001 | Time step duration in seconds |
| `stdp_ltp_weight` | float | 0.01 | STDP Long-Term Potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP Long-Term Depression weight |
| `synaptogenesis_rate` | float | 0.0001 | Rate of new synapse formation |
| `pruning_rate` | float | 0.00001 | Rate of synapse removal |
| `visualization_enabled` | bool | false | Enable/disable visualization |

### Advanced Configuration Keys

| Key | Type | Description |
|-----|------|-------------|
| `random_seed` | int64_t | Random seed for reproducible experiments |
| `max_simulation_steps` | uint64_t | Maximum number of simulation steps |
| `simulation_time_limit` | double | Maximum simulation time in seconds |
| `log_level` | string | Logging level (Debug, Info, Warning, Error) |
| `log_to_file` | bool | Enable file logging |

## Troubleshooting

### Common Issues and Solutions

**1. Configuration File Not Found**
```bash
# Solution: Create default configuration
./nlm --config configs/default.cfg
```

**2. Invalid Configuration Format**
```bash
# Solution: Use valid JSON or YAML
# Example JSON:
cat > config.json << EOF
{
    "neuron_count": 1000,
    "region_count": 2
}
EOF
./nlm --config config.json
```

**3. Memory Issues**
```bash
# Solution: Increase system memory
# NLM requires ~512MB per 1000 neurons
./nlm --neuron_count=500
```

**4. Command Line Argument Errors**
```bash
# Solution: Check help for correct syntax
./nlm --help
```

### Error Messages

**Configuration Errors**
- "Cannot open configuration file: <filename>"
  → Check if file exists and has correct permissions

- "Failed to parse configuration file: <filename>"
  → Validate file format (JSON, YAML, or simple key=value)

- "Configuration validation failed: <error message>"
  → Fix configuration according to error messages

**Memory Management Errors**
- "Memory access violation"
  → Check for null pointers or double-free scenarios

**Simulation Errors**
- "Brain initialization failed"
  → Check configuration values for invalid parameters

## Advanced Features

### Configuration Schemas
NLM supports configuration schemas for validation:
```cpp
nlm::ConfigSchema schema;
schema.addEntry("neuron_count", "int", "Number of neurons", 500, true);
schema.addEntry("region_count", "int", "Number of regions", 1, true);
schema.addEntry("visualization_enabled", "bool", "Enable visualization", false, false);
```

### Configuration Export/Import
```cpp
// Export current configuration
std::string json = config.exportToJSON();
std::string yaml = config.exportToYAML();

// Import from string
bool success = config.loadFromJSON(jsonStr);
bool success = config.loadFromYAML(yamlStr);
```

### Multi-File Configuration
Load and merge multiple configuration files:
```bash
./nlm --config base.json --config overrides.json --config experimental.json
```

## Configuration Best Practices

### 1. Start with Simple Configurations
- Begin with default configuration
- Gradually add custom settings
- Test each change individually

### 2. Use Environment-Specific Configurations
```bash
# Development
./nlm --config configs/dev.cfg

# Production
./nlm --config configs/prod.cfg

# Testing
./nlm --config configs/test.cfg
```

### 3. Document Configuration Changes
- Keep configuration changes documented
- Use comments in configuration files
- Track configuration evolution

### 4. Validate Before Deployment
- Always validate configuration before running
- Check for deprecated settings
- Ensure required parameters are set

## Conclusion

The NLM configuration system has been significantly enhanced with:

1. **Multi-format Support**: JSON, YAML, and simple key=value formats
2. **Type Safety**: Automatic type conversion and validation
3. **Memory Safety**: Proper cleanup of all resources
4. **Enhanced CLI**: Better help system and argument parsing
5. **Schema Validation**: Configuration integrity checks
6. **Export/Import**: Easy configuration sharing
7. **Comprehensive Error Handling**: Detailed error messages and troubleshooting

These improvements make NLM more robust, user-friendly, and suitable for both research and production environments.

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

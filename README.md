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

## Key Improvements in Phase 6.1

### Enhanced Command Line Interface
- Comprehensive help system with detailed option descriptions
- Support for multiple demo modes (--phase2, --phase3, --phase4, --phase6)
- Advanced configuration validation and error reporting
- Support for JSON configuration export/import
- Configuration comparison and diff tools

### Improved Configuration Management
- Fixed duplicate configuration keys in default.cfg
- Added advanced configuration validation system
- Support for schema-based validation
- Cross-field validation for logical consistency
- Configuration migration and upgrade tools

### Enhanced Error Handling
- Comprehensive error handling in all core modules
- Detailed error messages with suggestions
- File I/O error handling with recovery
- Type validation and conversion
- Graceful degradation on errors

### Performance Optimizations
- Improved const correctness throughout codebase
- Optimized memory usage patterns
- Reduced unnecessary allocations
- Enhanced loop performance
- Better cache locality

### Advanced Features
- Comprehensive logging and debugging tools
- Performance profiling capabilities
- Memory leak detection
- Detailed step-by-step logging
- Configuration validation and export/import utilities

## Building

### CMake Build System
The NLM project uses CMake for building. The improved build system includes:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Available Executables

After building, you will have the following executables available:

- `nlm` - Phase 2: Basic neural computation demo
- `nlm_phase3_demo` - Phase 3: World interaction demo
- `nlm_phase4_demo` - Phase 4: Cognitive experiments demo
- `nlm_phase6_demo` - Phase 6: Full integration demo

## Running

### Quick Start

Run the basic neural computation demo:

```bash
./nlm --phase2
```

Run the world interaction demo:

```bash
./nlm_phase3_demo
```

Run the cognitive experiments demo:

```bash
./nlm_phase4_demo
```

Run the full integration demo:

```bash
./nlm_phase6_demo
```

### Advanced Usage

#### Using Custom Configuration

```bash
./nlm --phase2 --config my_config.cfg
```

#### Running with Advanced Options

```bash
./nlm --phase2 --neurons 2000 --regions 3 --connections 0.2 --seed 12345
```

#### Displaying Configuration Information

```bash
./nlm --phase2 --print-config
./nlm --phase2 --validate-config
```

#### Exporting Configuration

```bash
./nlm --phase2 --print-config | ./export_to_json > config.json
```

### Configuration File

The default configuration file is `configs/default.cfg`. Sample configuration:

```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
log_filename = nlm.log
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── configs/
│   └── default.cfg
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   │   ├── Types/      # Type definitions
│   │   ├── Config/     # Configuration system
│   │   ├── Random/     # Random number generation
│   │   └── Logger/     # Logging system
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
│   └── visualization/ # Visualization
│   ├── main.cpp       # Main entry point with enhanced CLI
│   ├── AgentBrain.cpp  # Agent-brain interface
│   └── SimpleWorld.cpp # Simple world simulation
├── tests/
│   ├── unit/          # Unit tests
│   └── integration/   # Integration tests
└── .kilo/            # Kilo configuration and commands
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
- **Enhanced CLI and error handling**

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity
- **Improved configuration validation**

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
- **Advanced CLI options**

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system
- **Configuration export/import**

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- **Enhanced error handling and validation**
- Comprehensive documentation and user guides

## Advanced Configuration Validation

NLM now includes a comprehensive configuration validation system:

### Configuration Schema Validation
- Type checking for all configuration values
- Range validation for numeric parameters
- Enumeration validation for allowed values
- Required field validation

### Cross-Field Validation
- Logical consistency checks between related fields
- Dependency validation (e.g., neuron_count > 0 implies region_count > 0)
- Resource constraint validation

### Configuration Validation Examples

```bash
# Validate configuration
./nlm --phase2 --validate-config

# Print configuration
./nlm --phase2 --print-config

# Export configuration to JSON
./nlm --phase2 --print-config | export_to_json > config.json

# Compare configurations
cat config1.cfg | export_to_json > config1.json
cat config2.cfg | export_to_json > config2.json
diff config1.json config2.json
```

## Enhanced CLI Options

### Basic Options
- `--help, -h` - Show help message
- `--version` - Show version information
- `--quiet` - Minimal output
- `--verbose` - Maximum output

### Demo Mode Options
- `--phase2` - Run Phase 2 demo (default)
- `--phase3` - Run Phase 3 world interaction demo
- `--phase4` - Run Phase 4 cognitive experiments demo
- `--phase6` - Run Phase 6 integration demo

### Configuration Options
- `--config FILE` - Load configuration from FILE
- `--print-config` - Print current configuration
- `--validate-config` - Validate configuration and show errors

### Neural Network Options
- `--neurons N` - Number of neurons (default: 500)
- `--regions R` - Number of regions (default: 1)
- `--connections P` - Connection probability (default: 0.15)
- `--seed SEED` - Random seed (default: 42)
- `--timestep T` - Simulation timestep in seconds (default: 0.001)

### Plasticity Options
- `--stdp-ltp W` - STDP LTP weight (default: 0.02)
- `--stdp-ltd W` - STDP LTD weight (default: 0.015)
- `--stdp-tau T` - STDP time constant (default: 20.0)
- `--hebbian W` - Hebbian learning rate (default: 0.01)
- `--synapse-rate R` - Synaptogenesis rate (default: 0.0001)
- `--prune-rate R` - Pruning rate (default: 0.00001)

### Behavior Options
- `--episodes N` - Number of episodes for demos (default: 5)
- `--steps S` - Steps per episode (default: 200)
- `--enable-plasticity` - Enable synaptic plasticity (default: true)
- `--enable-development` - Enable development (default: true)

### Logging Options
- `--log-level L` - Logging level: DEBUG, INFO, WARN, ERROR (default: INFO)
- `--log-file F` - Log to file (default: nlm.log)
- `--no-color` - Disable colored output

### Output Options
- `--quiet` - Minimal output
- `--verbose` - Maximum output
- `--json-output` - Output results in JSON format

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

## Getting Help

For technical support or questions about NLM:
1. Check the documentation in the `docs/` directory
2. Review the examples in the `examples/` directory
3. Use the `--help` flag with any NLM executable for detailed usage information
4. Report issues on the project repository

## Future Improvements

The following improvements are planned for future phases:

1. **Advanced Learning Mechanisms**
   - Implement more sophisticated learning algorithms
   - Add meta-learning capabilities
   - Integrate reinforcement learning

2. **Enhanced Social Learning**
   - Implement more complex social interactions
   - Add cultural transmission mechanisms
   - Include imitation learning

3. **Improved Development**
   - Add more realistic developmental trajectories
   - Implement critical period mechanisms
   - Add lifespan development

4. **Advanced Memory Systems**
   - Implement more sophisticated episodic memory
   - Add semantic memory consolidation
   - Implement working memory updating mechanisms

5. **Visualization and Analysis Tools**
   - Add neural activity visualization
   - Implement brain network analysis
   - Add learning trajectory visualization

6. **Performance Optimizations**
   - Implement GPU acceleration
   - Add distributed computing support
   - Optimize memory usage for large-scale simulations
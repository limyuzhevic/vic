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

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Phase 6 Enhanced Build (with experimental features)
```bash
mkdir build_phase6
cd build_phase6
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENHANCED_FEATURES=ON
make -j4
```

### Build with Python Bindings (if enabled)
```bash
mkdir build_pybinds
cd build_pybinds
cmake .. -DPYTHON_BINDINGS=ON
make -j4
```

## Running

### Phase 6 Demo (Enhanced Integration Test)

The Phase 6 demo now includes an enhanced command-line interface with improved configuration options:

```bash
# Basic usage
./nlm_phase6_demo

# With custom configuration
./nlm_phase6_demo --neuron-count=500 --max-steps=2000

# Enable verbose output
./nlm_phase6_demo --verbose

# Run in benchmark mode
./nlm_phase6_demo --benchmark

# Save results to file
./nlm_phase6_demo --output=phase6_results.json

# Enable all experimental features
./nlm_phase6_demo --env-type=GridWorld --env-size=20 --verbose
```

### Enhanced Command-Line Options

- `--neuron-count=N`: Number of neurons (default: 1000)
- `--max-steps=N`: Number of simulation steps (default: 10000)
- `--region-count=N`: Number of brain regions (default: 1)
- `--connection-prob=N`: Synaptic connection probability (default: 0.1)
- `--checkpoint-path=PATH`: Checkpoint file path (default: ./checkpoint_test.bin)
- `--env-type=TYPE`: Environment type (SimpleWorld, GridWorld)
- `--env-size=N`: Environment size in cells (default: 16)
- `--log-level=LEVEL`: Logging level (Debug, Info, Warning, Error)
- `-v, --verbose`: Enable detailed output and progress tracking
- `-b, --benchmark`: Run in benchmark mode with performance metrics
- `-o, --output=FILE`: Save integration results to JSON file

### Legacy Phase 6 Demo

For backward compatibility, you can also run the original Phase 6 integration demo:

```bash
./nlm_phase6_demo_original  # If built with legacy option
```

### Phase 2 Legacy (Deprecated)

The legacy Phase 2 demo is still available for reference:

```bash
./nlm  # Phase 2 Real Neural Computation demo
```

## Phase 6 Integration Features

### Enhanced Configuration System

The configuration system now supports:

- **JSON/YAML configuration files**: New `loadFromJSON()` method for proper configuration parsing
- **Command-line overrides**: Load configuration from command line arguments
- **File-based configuration**: Save/load configuration to/from files
- **Type safety**: Strong typing for configuration values (int, float, string, bool)

### Brain System Integration

Phase 6 provides comprehensive integration testing for all brain systems:

- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction System**: Neural prediction with error signals
- **Cognition**: Planning, concept formation, attentional selection
- **Development**: Developmental stages affecting plasticity rates

### Advanced Features

#### Checkpointing and Persistence

- **Automatic checkpointing**: Save brain state periodically
- **Manual checkpointing**: Save/load brain state on demand
- **State restoration**: Resume simulation from saved checkpoints
- **Atomic saves**: Reliable checkpoint saving with atomic writes

#### Replay and Memory Consolidation

- **Episodic memory replay**: Replay important experiences during rest
- **Memory consolidation**: Strengthen important synaptic connections
- **Sleep-like cycles**: Integration with brain sleep/rest phases

#### Developmental Programming

- **Critical periods**: Sensitive windows for learning
- **Structural plasticity**: Synaptogenesis and pruning based on age
- **Activity-dependent development**: Experience shapes brain structure

### Enhanced Experiment Framework

The experiment framework includes:

- **Phase6IntegratedExperiment**: Comprehensive integration test runner
- **Phase6Config**: Configuration structure for Phase 6 experiments
- **Phase6IntegrationResult**: Detailed results reporting
- **Automatic benchmarking**: Performance measurement and reporting
- **Progress tracking**: Real-time progress indicators during experiments

### Error Handling and Logging

- **Enhanced error handling**: Better error messages and recovery
- **Multiple logging levels**: Debug, Info, Warning, Error
- **Progress reporting**: Real-time progress bars and status updates
- **Result persistence**: Save experiment results to files
- **Configuration validation**: Validate configuration parameters before running

### Performance Optimizations

The enhanced build includes:

- **Parallel processing**: Multiple CPU cores for simulation
- **Memory pools**: Efficient memory allocation
- **Event-driven computation**: Optimized neural dynamics
- **SIMD vectorization**: Hardware-accelerated computations
- **SIMD vectorization**: Hardware-accelerated computations

## Example Phase 6 Configuration File

Create a `phase6_config.json` file for advanced configuration:

```json
{
  "max_simulation_steps": 5000,
  "neuron_count": 500,
  "region_count": 2,
  "connection_probability": 0.15,
  "enable_checkpointing": true,
  "enable_replay": true,
  "enable_development": true,
  "checkpoint_path": "./checkpoints/phase6_checkpoint.bin",
  "environment_type": "GridWorld",
  "environment_size": 20,
  "log_level": "Info",
  "verbose": true
}
```

## Running Phase 6 with Custom Configuration

```bash
# Use custom configuration file
./nlm_phase6_demo --config=phase6_config.json

# Override specific settings from command line
./nlm_phase6_demo --config=phase6_config.json --neuron-count=300 --verbose
```

## Monitoring and Analysis

The enhanced Phase 6 demo provides:

- **Real-time metrics**: Monitoring of brain system performance
- **Integration scores**: Quality assessment of system integration
- **Progress tracking**: Visual progress bars for long simulations
- **Result analysis**: Detailed reporting of experimental results
- **Export functionality**: Save results in JSON format for further analysis

## Advanced Usage

For advanced usage and custom experiments, refer to the Phase 6 documentation in the `docs/` directory:

- `docs/PHASE6_FINAL_AUDIT.md`: Complete audit of Phase 6 implementation
- `docs/EXPERIMENTS.md`: Experimental protocols and procedures
- `docs/ARCHITECTURE.md`: Technical architecture and design

## Troubleshooting

### Common Issues and Solutions

**Issue: "Unknown option: --invalid-option"**
- Solution: Use `--help` to see all available options

**Issue: "Checkpoint save/load failed"**
- Solution: Check disk space and permissions for checkpoint directory

**Issue: "Memory allocation failed"**
- Solution: Reduce neuron count or increase system memory

**Issue: "Configuration validation failed"**
- Solution: Check configuration values and use `--help` for valid options

### Getting Help

For additional assistance:

1. **Command-line help**: `./nlm_phase6_demo --help`
2. **Version information**: `./nlm_phase6_demo --version`
3. **Issue reporting**: Report bugs in the project repository
4. **Documentation**: Refer to the `docs/` directory for detailed documentation

## Development and Contributing

### Building for Development

For development and testing, use:

```bash
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENHANCED_FEATURES=ON
make -j4
```

### Running Tests

Run the test suite to verify Phase 6 integration:

```bash
cd build
cnmake test
```

### Adding New Features

To add new Phase 6 features:

1. Add configuration options to `Phase6Config` structure
2. Implement new integration tests in `Phase6IntegratedExperiment`
3. Update the enhanced demo interface
4. Add comprehensive documentation

### Best Practices

- **Configuration files**: Use JSON format for complex configurations
- **Command-line arguments**: Override configuration settings as needed
- **Error handling**: Provide clear error messages for user guidance
- **Progress reporting**: Include progress indicators for long-running simulations
- **Result persistence**: Save results for reproducibility and analysis

## Phase 6 Integration Complete

With these enhancements, Phase 6 provides a comprehensive and robust framework for:

- **Testing brain system integration**: Verifying all neural systems work together
- **Running developmental experiments**: Studying brain development over time
- **Analyzing learning behavior**: Understanding how neural systems learn
- **Researching artificial intelligence**: Exploring brain-inspired AI approaches

The enhanced Phase 6 demo offers a powerful tool for researchers, developers, and students to explore the frontier of artificial brain systems and artificial developmental systems.

For more information, visit the project documentation and repository.

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

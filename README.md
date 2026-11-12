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

Build all targets first (see Building above). The entry points are currently separate demos per phase, not a single unified binary:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

## Unified CLI

The NLM project has been refactored to provide a unified command-line interface that replaces multiple phase-specific executables with a single `nlm` command supporting subcommands. This architecture provides:

- Single entry point: `nlm <command> [options] [args]`
- Six main subcommands: `run`, `analyze`, `export`, `visualize`, `config`, `tools`
- Command registration system for extensibility
- Backward compatibility with existing demo executables
- Clean separation between CLI interface and core logic

### New Unified CLI Usage

```bash
# Show main help
./nlm

# Show specific command help
./nlm run --help
./nlm analyze --help

# Run demos
./nlm run phase3 --episodes 10 --steps 500
./nlm run benchmark
./nlm run phase4
./nlm run phase6
```

### New Subcommands

#### 1. `nlm run`

**Purpose**: Run demonstrations and experiments

**Available modes**:
- `phase3`: Phase 3 world interaction demo
- `phase4`: Phase 4 cognitive emergence demo  
- `phase5`: Phase 5 personality/sandbox demo
- `phase6`: Phase 6 integration demo
- `benchmark`: Performance benchmarks

#### 2. `nlm analyze`

**Purpose**: Analyze brain state and activity

**Analysis modes**:
- `connectivity`: Neural connectivity patterns
- `plasticity`: Synaptic plasticity analysis
- `stdp`: STDP mechanism verification
- `learning`: Learning progress analysis
- `memory`: Memory systems analysis
- `all`: Run all analyses

#### 3. `nlm export`

**Purpose**: Export data and results

**Formats**:
- `json`: JSON format
- `csv`: CSV format
- `protobuf`: Protocol Buffers format
- `txt`: Text format

**Targets**:
- `brain`: Brain state export
- `config`: Configuration export
- `metrics`: Performance metrics export
- `all`: Export all data

#### 4. `nlm visualize`

**Purpose**: Create visualizations of brain activity and results

**Visualization types**:
- `neurons`: Neuron activity visualization
- `connections`: Neural connection visualization
- `spikes`: Spike pattern visualization
- `memory`: Memory trace visualization
- `plasticity`: Synaptic change visualization
- `all`: All visualizations

#### 5. `nlm config`

**Purpose**: Manage configuration settings

**Subcommands**:
- `list`: List all configuration options
- `get <key>`: Get a configuration value
- `set <key> <val>`: Set a configuration value
- `reset`: Reset to default configuration
- `load <file>`: Load configuration from file
- `save <file>`: Save configuration to file
- `validate`: Validate current configuration

#### 6. `nlm tools`

**Purpose**: Advanced tools and utilities

**Available tools**:
- `--checkpoint`: Create brain checkpoint
- `--replay`: Replay from checkpoint
- `--perturb`: Apply perturbations
- `--stimulus`: Apply external stimuli
- `--analyze`: Run detailed analysis
- `--simulate`: Run simulation with custom settings

## Backward Compatibility

### Phase-Specific Executables

The original phase-specific executables are still maintained for backward compatibility:

- `nlm_phase3_demo`: Phase 3 demo (now also accessible via `nlm run phase3`)
- `nlm_phase4_demo`: Phase 4 demo (now also accessible via `nlm run phase4`)
- `nlm_phase5_demo`: Phase 5 demo (now also accessible via `nlm run phase5`)
- `nlm_phase6_demo`: Phase 6 demo (now also accessible via `nlm run phase6`)
- `nlm_phase6_text_demo`: Phase 6 text demo
- `nlm_benchmark`: Benchmark executable (now also accessible via `nlm run benchmark`)

### Argument Patterns

The unified CLI maintains compatibility with original argument patterns:

1. **Original demo execution**: `nlm_phase3_demo [episodes] [steps]`
   - Now equivalent to: `nlm run phase3 --episodes N --steps M`

2. **Original benchmark execution**: `nlm_benchmark`
   - Now equivalent to: `nlm run benchmark`

3. **Original config file loading**: `--config file.cfg`
   - Now supported through `nlm config` commands

## Migration Guide

### For Users

If you're using the original phase-specific executables, you can migrate to the unified CLI:

1. Replace `nlm_phase3_demo 5 200` with `nlm run phase3 --episodes 5 --steps 200`
2. Replace `nlm_phase4_demo` with `nlm run phase4`
3. Replace `nlm_benchmark` with `nlm run benchmark`

The unified CLI provides all the original functionality with additional features and better organization.

### For Developers

If you're extending the NLM project:

1. Add new commands by implementing the `Command` interface
2. Use the command registration system for extensibility
3. Maintain backward compatibility by keeping the old executables
4. Follow the existing command patterns for consistency

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
│   ├── cli/           # Command-line interface
│   │   ├── NLMCLI.cpp              # Main CLI entry point
│   │   ├── SubcommandsRegistration.cpp # Command registration
│   │   └── Subcommands/            # Subcommand implementations
│   │       ├── RunCommand.hpp/.cpp
│   │       ├── AnalyzeCommand.hpp/.cpp
│   │       ├── ExportCommand.hpp/.cpp
│   │       ├── VisualizeCommand.hpp/.cpp
│   │       ├── ConfigCommand.hpp/.cpp
│   │       └── ToolsCommand.hpp/.cpp
│   ├── core/           # Core utilities
│   ├── agent/          # Agent-brain interface
│   ├── world/          # Simple world
│   ├── performance/    # Checkpoints, pools, queues (headers)
│   ├── brain/         # Neural components (integrated)
│   ├── dynamics/      # Neural dynamics
│   ├── plasticity/    # Plasticity rules
│   ├── development/   # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
├── tests/
└── configs/
```

## Future Enhancements

The architecture can be easily extended with:

1. **Additional subcommands**: For new functionality
2. **Command aliases**: Multiple names for the same command
3. **Command groups**: Organize related commands
4. **Command completion**: Tab completion support
5. **Command scripting`: Support for command scripts
6. **Plugin architecture`: Load third-party commands dynamically

## License

MIT

## Authors

Research project — See docs for scientific background.
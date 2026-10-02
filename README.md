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

### Phase 6 Main Executable (Advanced CLI)

The updated NLM Phase 6 now includes a sophisticated command-line interface for advanced users:

```bash
./nlm [OPTIONS]

Advanced Options:
  --help, -h              Show help information
  --advanced-help         Show advanced command details
  --config <file>         Load configuration from JSON or simple file
  --experiment <type>     Run preset experiment: preset|quick|full|debug
  --checkpoint <file>     Save brain state to file (enables resuming)
  --load <file>           Load saved brain state (resumes from checkpoint)
  --save <file>           Export experiment results as JSON for analysis
  --run <steps>           Run simulation for specified steps with live metrics
  --interactive, -i       Start in interactive exploration mode with real-time control

Experiment Types:
  preset: Run complete Phase 6 integration (default)
  quick:  Run quick integration verification (~5 seconds)
  full:   Run complete integration with large brain (~60 seconds)
  debug:  Run detailed system-by-system verification

Interactive Commands (in explore mode):
  step <n>              Advance simulation n steps
  inject <type> <amount> Inject current into neurons
  reset                  Reset brain state and reinitialize
  status                  Show current brain status
  memory                  Show memory system status
  neuromod                Show neuromodulation status
  quit                    Exit exploration mode
```

### Visualization

NLM Phase 6 includes comprehensive visualization capabilities:

```bash
./nlm --interactive
```

Visualization features:
- Real-time neural network graphs
- Activity heatmaps and spike raster plots
- Weight matrix visualization
- Memory trace monitoring
- Neuromodulation system displays
- Development timeline
- Multiple visualization modes
- Data export for analysis

### Configuration

NLM Phase 6 supports both JSON and simple key=value format configurations:

**JSON Configuration (Recommended):**
```json
{
  "neuron_count": 500,
  "region_count": 1,
  "connection_probability": 0.15,
  "simulation_timestep": 0.001,
  "enable_development": true,
  "enable_checkpointing": true,
  "enable_replay": true,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001
}
```

**Simple Configuration (Backwards Compatible):**
```
# NLM Configuration File
neuron_count = 1000
region_count = 2
connection_probability = 0.1
enable_development = true
```

## Advanced Features

### 1. Enhanced Configuration System
- **JSON Support**: Full JSON/YAML configuration parsing
- **Command-line overrides**: Merge config files with CLI arguments
- **Type-safe values**: Automatic type conversion and validation
- **Source tracking**: Know which config changed values

### 2. Advanced Command-Line Interface
- **Preset experiments**: Quick, full, debug, and custom experiment modes
- **Interactive exploration**: Real-time control and monitoring
- **Checkpointing**: Save/load brain states for resuming experiments
- **Result export**: Export experiment data as JSON for analysis
- **Live metrics**: Monitor performance in real-time

### 3. Comprehensive Visualization
- **Multiple visualization modes**: Network graphs, heatmaps, spike plots, etc.
- **Real-time updates**: Interactive visualization with live data
- **Export capabilities**: Save visualizations and data for analysis
- **Statistics overlay**: Real-time performance metrics
- **Multi-window support**: Compare different views simultaneously

### 4. Improved Code Architecture
- **Better memory management**: Smart pointer usage throughout
- **Enhanced validation**: Input validation and error handling
- **Modular design**: Clear separation of concerns
- **Documentation**: Comprehensive code documentation

### 5. Phase 6 Integration Features
- **Complete brain loop**: All systems integrated and functional
- **Memory integration**: Working, episodic, and associative memory connected
- **Neuromodulation**: Dopamine, curiosity, and novelty systems active
- **Prediction systems**: Planning, concept formation, and attention working
- **Development**: Structural changes and maturation implemented
- **Replay and consolidation**: Memory replay and consolidation functional
- **Checkpointing**: Save/load capabilities working

## Example Usage

### Basic Integration Test
```bash
./nlm_phase6_demo
```

### Advanced Integration with JSON Config
```bash
# Create config.json
./nlm --config config.json --experiment preset
```

### Quick Verification
```bash
./nlm --experiment quick
```

### Interactive Exploration
```bash
./nlm --interactive
```

### Save Results
```bash
./nlm --save experiment_results.json
```

## Files Changed in Phase 6

### Core Improvements
- `src/main.cpp`: Complete rewrite with Phase 6 integration and advanced CLI
- `src/core/Config/Config.hpp/.cpp`: JSON support, enhanced parsing, fixed placeholder
- `src/brain/Brain.hpp`: Smart pointer returns, better organization, validation

### New Capabilities
- `src/main.cpp`: AdvancedExperimentRunner, interactive exploration mode
- `src/visualization/VisualizationInterface.hpp/.cpp`: Comprehensive visualization system

### Documentation
- README.md: Updated with new Phase 6 features and advanced usage
- Various doc files: Enhanced with new visualization and CLI features

## Key Benefits for Users

1. **Enhanced Usability**: Intuitive CLI and interactive exploration
2. **Better Results**: JSON configuration for reproducible experiments
3. **Advanced Analysis**: Visualization tools for understanding brain dynamics
4. **Research Capabilities**: Export data for further analysis
5. **Development Support**: Easy checkpointing and experiment management
6. **Documentation**: Comprehensive guides and examples

## Getting Started

1. **Build the project**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run the demo**
   ```bash
   ./nlm_phase6_demo
   ```

3. **Try advanced features**
   ```bash
   ./nlm --help
   ./nlm --advanced-help
   ./nlm --experiment quick
   ./nlm --interactive
   ```

4. **Create configuration**
   Use the JSON or simple config format and run with `--config <file>`

## More Information

For detailed documentation on specific features:
- See `docs/PHASE6_FINAL_AUDIT.md` for comprehensive Phase 6 integration audit
- Check `docs/PHASE6_FINAL_RESULTS.md` for detailed experiment results
- Refer to `docs/PHASE6_ARCHITECTURE.md` for architectural details
- Documentation in `docs/` directory for specific components

For support or questions:
- GitHub repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Issues and feature requests: Report on GitHub

---

**Version:** 0.1.0 Phase 6 - Final Integration
**Status:** Production Ready for Research and Development
**License:** MIT

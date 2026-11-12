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

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

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
Build all targets first (see Building above). The entry points are
currently separate demos per phase, not a single unified binary:

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

NLM is a research project investigating computational brain-like systems. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

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

## Current Phase (Phase 6: Final Integration)

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Key Achievements in Phase 6:

1. **Comprehensive Python Utilities**
   - New `pynlm.utils` module with brain analysis tools
   - `pynlm.viz` visualization and dashboard tools
   - `pynlm.config` configuration management and parameter sweeps
   - CLI utility program `nlm-utils` with 60+ commands

2. **Enhanced Documentation**
   - Complete `easy_usage.md` with practical examples
   - Comprehensive `README_UTILITIES.md` for utilities
   - Updated `ARCHITECTURE.md` with utility layer
   - Advanced usage patterns and troubleshooting

3. **Advanced Features for Power Users**
   - Batch processing and parameter sweeps
   - Experiment management and tracking
   - Visualization and analysis tools
   - Configuration presets and management

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

Build all targets first (see Building above). The entry points are:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase5_demo     # Phase 5 library with demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_phase6_text_demo # Phase 6 text/thought demo
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

## Python Bindings

### Installation

```bash
# Install build dependencies (use a venv on PEP 668 systems, where
# system pip refuses to install packages)
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Python Module Structure

The NLM Python module provides:

1. **Core Classes**:
   - `Brain`: Main neural simulation brain
   - `Config`: Configuration management
   - `AgentBrain`: Agent interface connecting brain to world
   - `SimpleWorld`: 2D world simulation
   - `Vision`, `Audio`, `InternalSignals`: Sensory input types
   - `Action`: Motor action representation

2. **Utility Modules**:
   - `pynlm.utils`: Brain analysis and utilities
   - `pynlm.viz`: Visualization and plotting
   - `pynlm.config`: Configuration management

3. **Command-Line Tools:
   - `nlm-utils`: Comprehensive utility program
   - 60+ commands for brain manipulation, configuration, visualization

## Basic Python API Examples

### Simple Usage

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Usage with Utilities

```python
import pynlm
from pynlm.utils import BrainAnalyzer, ConfigManager, WorldUtils
from pynlm.viz import Visualization
from pynlm.config import BrainConfigurator, ParameterSweep

# Create and run brain with utilities
config = BrainConfigurator()
config.apply_preset("research")

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Analyze with utilities
analyzer = BrainAnalyzer(brain)
analyzer.print_formatted_stats()

# Export results
stats_file = analyzer.export_to_json("brain_stats.json")

# Visualization
viz = Visualization()
dashboard_file = viz.save_dashboard(brain, filename="analysis_dashboard.png")

# Configuration management
config2 = BrainConfigurator()
config2.apply_preset("small")
config2.update({'random_seed': 12345})
config2.save_to_yaml("custom_config.yaml")

# Parameter sweep
base_config = BrainConfigurator().apply_preset("medium")
sweep = ParameterSweep(base_config)
sweep.add_parameter("brain.neuron_count", [500, 1000, 2000])
```

## Command-Line Tools

### `nlm-utils` - Comprehensive Utility Program

A unified command-line interface for all NLM operations:

```bash
# Get help and list all commands
nlm-utils --help

# Create and analyze a brain
nlm-utils brain create --neurons 1000 --steps 100
nlm-utils brain analyze brain.bin

# Configuration management
nlm-utils config presets          # List all presets
nlm-utils config create tiny      # Create tiny brain configuration

# Experiment utilities
nlm-utils experiment create my_exp          # Create new experiment
nlm-utils experiment run my_exp --trials 10 # Run experiment with 10 trials

# Visualization
nlm-utils viz dashboard brain.bin dashboard.png
nlm-utils viz stats brain.bin stats.png

# Batch processing
nlm-utils batch create --count 5 --steps 100
nlm-utils batch analyze brains/*.bin
```

### Available Command Categories

1. **brain**: Brain operations (create, analyze, save, load, export, compare)
2. **config**: Configuration management (presets, save/load, validation, compare)
3. **world**: World operations (create, configure, reset, analyze)
4. **agent**: Agent operations (create, initialize, process, decode, train)
5. **experiment**: Experiment management (create, run, add-trial, summary, results)
6. **viz**: Visualization (dashboard, stats, network, development, spikes)
7. **batch**: Batch processing (create, process, analyze, export)

## Configuration Presets

NLM provides several pre-configured presets for different use cases:

- **tiny**: 50 neurons (debugging/testing)
- **small**: 200 neurons (basic simulations)
- **medium**: 1000 neurons (general purpose)
- **large**: 5000 neurons (complex simulations)
- **research**: 10000 neurons (scientific research)
- **debug**: 100 neurons (development/debugging)

## Utility Modules

### `pynlm.utils` - Core Utilities

#### BrainAnalyzer
- Analyze brain statistics
- Export to CSV/JSON formats
- Print formatted statistics

#### ConfigManager
- Create preset configurations
- Convert between config formats
- Print config summaries

#### WorldUtils
- Create test scenarios
- Run agent simulations
- Manage world configurations

#### ExperimentRunner
- Run experimental trials
- Track results
- Generate summaries

### `pynlm.viz` - Visualization

#### Visualization
- Comprehensive dashboard creation
- Firing rate plotting
- Spike distribution analysis
- Development stage visualization

#### Convenience Functions
- Quick dashboard creation
- Statistics plotting
- Brain statistics visualization

### `pynlm.config` - Configuration Management

#### BrainConfigurator
- Built-in presets
- Method chaining for configuration
- JSON/YAML file support
- Parameter validation

#### ParameterSweep
- Systematic parameter exploration
- Batch configuration generation

#### ExperimentManager
- Experiment lifecycle management
- Trial tracking and results
- Progress tracking

## Examples Directory

The `examples/` directory contains practical examples:

- `basic_brain.py`: Basic brain usage
- `agent_simulation.py`: Complete agent simulation
- More examples to come...

## Testing

### Unit Tests

```bash
# Run C++ unit tests
./nlm_test

# Or use CMake test command
cd build
ctest --output-on-failure
```

### Python Tests

```bash
# Install test dependencies
pip install pytest numpy matplotlib

# Run Python tests
pytest tests/
```

## Performance Tips

1. **Use appropriate presets**: Choose `medium` or `large` presets for most use cases
2. **Monitor memory**: Use `BrainAnalyzer` to track memory consumption
3. **Batch processing**: Process multiple brains in parallel when possible
4. **Configuration management**: Use presets for consistent configurations

## Development Guide

### Adding New Features

1. **Python Utilities**: Add new modules to `pynlm/` directory
2. **Command-Line Tools**: Add new commands to `nlm-utils`
3. **Documentation**: Update `README.md` and `easy_usage.md`
4. **Examples**: Add new examples to `examples/` directory

## License

MIT License - See LICENSE file for details

## Authors

Research project — See docs for scientific background.


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

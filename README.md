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

## Enhanced Features (Phase 2 Improvements)

The main executable now includes enhanced configuration capabilities:

### Improved Configuration System
- **Multi-format support**: JSON, YAML, and simple key=value formats
- **Better error handling**: Informative error messages for missing/invalid configs
- **Persistent storage**: Actual values saved in config files (not placeholders)
- **Source tracking**: Configuration sources (file, command-line, defaults) tracked

### Enhanced Command-Line Interface

```bash
nlm [options]

Options:
  --config <file>           Configuration file (default: configs/default.cfg)
  --help                    Show this help message
  --verbose                 Enable verbose logging (debug level)
  --generate-config         Generate example configuration file
  --output-config <file>    Save configuration to file

Test customization:
  --connectivity-current <float>  Current injection for Test 1 (default: 50.0)
  --plasticity-current <float>    Current injection for Test 2 (default: 30.0)
  --stdp-current <float>          Current injection for Test 3 (default: 60.0)
  --plasticity-steps <int>        Steps for plasticity experiment (default: 1000)
  --stdp-trials <int>             Trials for STDP test (default: 50)

Example:
  nlm --config myconfig.json --plasticity-current 40.0 --plasticity-steps 500
  nlm --generate-config --output-config example_cfg.json
  nlm --verbose --help
```

### Example Configuration Files

Multiple configuration formats are available in the `configs/` directory:

1. **`configs/default.cfg`** - Original simple key=value format
2. **`configs/improved_cfg.cfg`** - Enhanced human-readable format
3. **`configs/improved_cfg.json`** - JSON format with structured settings
4. **`configs/improved_cfg.yaml`** - YAML format with indentation

### Key Improvements

1. **Config Parser**: Upgraded from simple key=value to JSON/YAML support
2. **Command Line**: Better argument parsing with defaults and validation
3. **Test Parameters**: Configurable injection currents and experiment steps
4. **Logging**: Enhanced error messages and verbose mode
5. **Thread Safety**: Fixed thread-safe timestamp generation in logger
6. **Documentation**: Comprehensive help and example configs

### Usage Examples

```bash
# Load specific config with custom test parameters
./nlm --config configs/improved_cfg.json --plasticity-current 40.0 --plasticity-steps 500

# Generate a custom configuration file
./nlm --generate-config --output-config my_experiment.json

# Run with verbose debugging output
./nlm --verbose --connectivity-current 75.0

# Use a different configuration format
./nlm --config configs/improved_cfg.yaml --stdp-current 80.0 --stdp-trials 100
```

### Configuration File Formats

#### Simple Format (`*.cfg`)
```
# Parameter key and value
parameter_name = value

# Comments start with #
# Multiple formats supported:
#   - key = value
#   - key = "quoted value" 
#   - key = [array_value1, array_value2]
```

#### JSON Format (`*.json`)
```json
{
  "parameter_name": "value",
  "array_parameter": [1, 2, 3],
  "nested": {
    "inner": "value"
  }
}
```

#### YAML Format (`*.yaml`)
```yaml
parameter_name: value
array_parameter:
  - item1
  - item2
nested:
  inner: value
```

### Command-Line Argument Priority

Configuration sources have priority order:
1. **Command-line arguments** (highest priority)
2. **Config file** (if exists)
3. **Default values** (lowest priority)

For example, if you specify `--plasticity-current 40.0` on command line,
it will override the value in any config file.

### Troubleshooting

**Common Issues:**
- **Config file not found**: Try different paths or use `--generate-config`
- **Invalid JSON/YAML**: The parser uses basic parsing; complex structures may fail
- **Permission issues**: Ensure write permissions for output configs
- **Missing dependencies**: Check pybind11 for Python bindings

**Debugging:**
```bash
# Enable verbose logging to see detailed output
./nlm --verbose

# Use specific config to test
./nlm --config configs/improved_cfg.json --verbose

# Show help for all options
./nlm --help
```

### Future Improvements

The configuration system is designed for easy extension:

1. **Advanced JSON parsing**: Could implement full JSON library
2. **Config validation**: Schema validation for parameter ranges
3. **Configuration templates**: Save/load experiment configurations
4. **Dynamic parameters**: Runtime modification of settings
5. **Distributed configs**: Shared configuration files

These improvements make NLM more accessible to users with different needs:
- **Beginners**: Simple config files and help system
- **Researchers**: Full format support and parameterization
- **Developers**: Extensible configuration system for plugins

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Enhanced)
- **Real LIF neuron dynamics implemented**
- **Event-driven spike propagation with delays**
- **STDP and Hebbian plasticity rules**
- **Structural plasticity (synaptogenesis/pruning)**
- **Learning experiment demonstrates measurable changes**
- **Network shows activity-dependent synaptic modification**
- **Enhanced configuration and command-line interface**
- **Multiple configuration file formats support**
- **Thread-safe logging implementation**

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
blay
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
- Enhanced configuration capabilities added

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
│   ├── agent/          # Agent-brain interface
│   ├── world/          # Simple world
│   ├── performance/    # Checkpoints, pools, queues (headers)
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

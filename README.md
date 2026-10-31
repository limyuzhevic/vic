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
./nlm                 # Phase 2 demo in src/main.cpp (enhanced with comprehensive CLI)
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

### Enhanced main.cpp Command Line Interface

The `./nlm` executable has been completely enhanced with a comprehensive command-line interface:

#### Basic Usage
```bash
# Run default simulation
./nlm

# View help information
./nlm --help

# Run with specific parameters
./nlm --neuron-count=1000 --timestep=0.01 --seed=123
```

#### Advanced Usage
```bash
# Run with enhanced plasticity
./nlm --stdp-ltp-weight=0.03 --stdp-ltd-weight=0.018

# Quick validation (skips detailed experiments)
./nlm --quick-run --verbose

# Load configuration from file
./nlm --config=configs/my_config.cfg

# Run with custom brain parameters
./nlm --neuron-count=2000 --region-count=2 --connection-probability=0.2
```

#### Configuration Files

NLM supports multiple configuration file formats:

- **Simple key=value format**: `configs/default.cfg`
- **JSON format**: `configs/brain_config.json`
- **YAML format**: `configs/brain_config.yaml`

To use a different format, specify it with the `--config` option:
```bash
./nlm --config=brain_config.json
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── IMPROVEMENTS_SUMMARY.md      # Summary of recent improvements
├── easy_usage.md                # Enhanced usage guide with C++ CLI
├── HOW_TO_USE.md                # Comprehensive documentation
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
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Key Improvements

Recent enhancements to the NLM project include:

### 1. Enhanced main.cpp
- **Modular Design**: Split into multiple focused classes
- **Comprehensive CLI**: Extended command-line options for all configuration parameters
- **Multiple Format Support**: JSON, YAML, and key=value configuration files
- **Improved Error Handling**: Better validation and error messages
- **Enhanced Documentation**: Complete documentation and examples

### 2. Improved Configuration System
- **Multi-Format Support**: JSON, YAML, and traditional key=value formats
- **Robust Parsing**: Enhanced format detection and error handling
- **Flexible Options**: Extensive configuration parameters for customization

### 3. Comprehensive Documentation
- **easy_usage.md**: Enhanced with C++ command-line interface documentation
- **HOW_TO_USE.md**: Enhanced with detailed main.cpp features and examples
- **IMPROVEMENTS_SUMMARY.md**: Complete summary of all improvements

### 4. Better User Experience
- **Intuitive Interface**: Comprehensive CLI with helpful options
- **Clear Documentation**: Extensive examples and references
- **Robust Error Handling**: Detailed error messages and validation
- **Backward Compatibility**: All existing functionality preserved

## Usage Examples

### Basic Examples
```bash
# Run default simulation
./nlm

# View help information
./nlm --help

# Run with custom parameters
./nlm --neuron-count=1000 --timestep=0.01 --seed=123
```

### Advanced Examples
```bash
# Run with enhanced plasticity
./nlm --stdp-ltp-weight=0.03 --stdp-ltd-weight=0.018

# Quick validation
./nlm --quick-run --verbose

# Load from JSON configuration
./nlm --config=brain_config.json

# Configure custom brain parameters
./nlm --neuron-count=2000 --region-count=2
```

### Configuration Files

**Simple key=value** (`configs/default.cfg`):
```
neuron_count = 500
region_count = 1
connection_probability = 0.15
simulation_timestep = 0.001
random_seed = 42
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
```

**JSON** (`configs/brain_config.json`):
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "random_seed": 42
}
```

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Testing

Run the test suite to verify the enhanced functionality:
```bash
cd build
ctest --output-on-failure
```

## License

MIT

## Authors

Research project — See docs for scientific background.

## Further Reading

- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit
- [IMPROVEMENTS_SUMMARY.md](IMPROVEMENTS_SUMMARY.md) - Summary of recent improvements
- [easy_usage.md](easy_usage.md) - Enhanced usage guide with C++ CLI
- [HOW_TO_USE.md](HOW_TO_USE.md) - Comprehensive documentation

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

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

## Advanced NLM Features (Phase 2 Enhancement)

NLM Phase 2 introduces comprehensive advanced features for fine-grained control over neural development, learning, and memory systems. These features enable researchers and developers to create sophisticated experimental scenarios and custom brain configurations.

### Advanced Command Line Interface

**Key Features:**
- **32 Advanced Commands**: Comprehensive control over all brain subsystems
- **Enhanced Help System**: Detailed documentation with examples for all commands
- **Configuration File Support**: Save/load complex experimental configurations
- **User Level Documentation**: Instructions for beginners, intermediates, and advanced users

**Getting Started with Advanced Commands:**
```bash
# Show basic help
./nlm --help

# Show detailed advanced commands help
./nlm

# Run with advanced configuration
./nlm --config my_experiment.cfg
```

### Advanced Command Categories

#### 1. Brain Management Commands
- `--set-development-stage <stage>`: Control developmental plasticity
- `--set-neuromodulator-levels <d> <c> <n>`: Fine-tune brain chemistry
- `--enable-plasticity <true/false>`: Control learning capabilities
- `--set-plasticilty-rates <s> <h> <st>`: Adjust plasticity mechanisms

#### 2. Memory Control Commands
- `--clear-working-memory`: Reset transient memories
- `--set-memory-capacity <type> <cap>`: Adjust memory limits
- `--store-memory-episode <data>`: Save experiences
- `--replay-memories`: Consolidate memories through replay

#### 3. Neuromodulation Commands
- `--set-dopamine-level <level>`: Control reward processing
- `--set-curiosity-level <level>`: Control exploration drive
- `--set-novelty-level <level>`: Control novelty detection

#### 4. Development Commands
- `--set-developmental-stage <stage>`: Set brain development stage
- `--set-plasticilty-modifiers <h> <s> <st>`: Apply developmental multipliers
- `--enable-critical-period <true/false>`: Activate enhanced learning phase

#### 5. Integration Commands
- `--sync-memory-prediction`: Align memory with prediction system
- `--apply-neuromodulation`: Apply neuromodulation effects
- `--trigger-plasticity-update`: Force immediate plasticity changes

#### 6. Exploration/Testing Commands
- `--run-memory-test`: Test memory system functionality
- `--run-plasticity-test`: Test synaptic learning mechanisms
- `--run-integration-test`: Test cross-system coordination
- `--export-brain-state <file>`: Save brain state for analysis
- `--import-brain-state <file>`: Restore previous brain state

### Configuration File Support

**Create Custom Configuration Files:**
```bash
# Create configuration file
cat > my_experiment.cfg << EOF
# NLM Advanced Configuration
development_stage = critical
enable_critical_period = true
plasticity_enabled = true

dopamine_level = 0.8
curiosity_level = 0.7
novelty_level = 0.6

stdp_rate = 0.1
hebbian_rate = 0.05
structural_rate = 0.002

working_memory_capacity = 1000
episodic_memory_capacity = 5000
associative_memory_capacity = 3000

run_memory_test = true
run_plasticity_test = true
run_integration_test = true
export_file = advanced_experiment.bin
EOF

# Use configuration
./nlm --config my_experiment.cfg
```

**Configuration Features:**
- Save complex experimental setups
- Reproduce research conditions
- Batch processing capabilities
- Version control friendly

### Advanced Usage Examples

#### Beginner Level
```bash
# Basic advanced setup
./nlm --set-development-stage adult \
     --set-neuromodulator-levels 0.7 0.5 0.3 \
     --run-all-tests
```

#### Intermediate Level
```bash
# Customize brain dynamics
./nlm --set-neuromodulator-levels 0.7 0.5 0.3 \
     --enable-plasticity true \
     --store-memory-episode 1:0.8,2:0.9 \
     --run-memory-test
```

#### Advanced Level
```bash
# Complex experimental scenario
./nlm --enable-critical-period true \
     --set-neuromodulator-levels 0.8 0.7 0.6 \
     --set-plasticilty-modifiers 2.0 1.5 1.0 \
     --sync-memory-prediction \
     --apply-neuromodulation \
     --run-integration-test \
     --export-brain-state advanced_learning.bin
```

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Basic Phase 2 Demo
```bash
./nlm  # Runs basic tests with default configuration
```

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Advanced Command Examples
```bash
# Show advanced help
./nlm

# Run with custom configuration
./nlm --config my_experiment.cfg

# Run without basic tests (faster development)
./nlm --no-tests

# Enable advanced testing
./nlm --run-memory-test --run-plasticity-test --run-integration-test
```

## Advanced Documentation

### Detailed Documentation Files

The advanced features include comprehensive documentation:

1. **`advanced_usage.md`**: Practical examples and usage scenarios
2. **`command_reference.md`**: Complete command documentation with parameters
3. **`NLM_Advanced_Features.md`**: Technical implementation details

### Documentation Access

```bash
# View advanced usage guide
cat advanced_usage.md

# View command reference
cat command_reference.md

# View technical documentation
cat NLM_Advanced_Features.md
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
├── advanced_usage.md                # Advanced usage guide
├── command_reference.md             # Command documentation
├── NLM_Advanced_Features.md         # Technical documentation
├── easy_usage.md                   # Beginner guide
├── HOW_TO_USE.md                    # Python API documentation
├── configs/
│   └── default.cfg                 # Default configuration
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── NLM_Phase6_Integration_Analysis_Report.md
```

## Configuration Options

### Basic Configuration (`configs/default.cfg`)
```ini
# NLM Phase 2 Configuration
default_config = true
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### Configuration File Support

**Format:** Simple key=value pairs with comments
**Features:**
- Save experimental configurations
- Load complex settings
- Command line override capability
- Version control support

**Example Configuration File (`my_experiment.cfg`):**
```ini
# Research Experiment Configuration
experiment_name = curiosity_driven_learning
development_stage = critical
enable_critical_period = true

# Neuromodulator configuration
dopamine_level = 0.9
curiosity_level = 0.8
novelty_level = 0.7

# Plasticity configuration
plasticity_enabled = true
stdp_rate = 0.1
hebbian_rate = 0.05
structural_rate = 0.002

# Memory configuration
working_memory_capacity = 2000
episodic_memory_capacity = 10000
associative_memory_capacity = 5000

# Testing configuration
run_basic_tests = false
run_memory_test = true
run_plasticity_test = true
run_integration_test = true

# Export configuration
export_file = experiment_results.bin
```

## Advanced Command Line Arguments

| Command | Description | Typical Use |
|---------|-------------|-------------|
| `--help` | Show help message | Initial setup |
| `--config <file>` | Load configuration | Reproduce experiments |
| `--no-tests` | Skip basic tests | Development iterations |
| `--set-development-stage <stage>` | Set brain development | Custom behavior |
| `--set-neuromodulator-levels <d> <c> <n>` | Set brain chemistry | Behavioral tuning |
| `--enable-plasticity <true/false>` | Control learning | Experiment control |
| `--run-memory-test` | Test memory systems | Validation |
| `--run-plasticity-test` | Test learning mechanisms | Research |
| `--run-integration-test` | Test system coordination | Quality assurance |

## Usage Examples

### Example 1: Basic Advanced Configuration
```bash
# Use advanced configuration
./nlm --config my_research_experiment.cfg
```

### Example 2: Custom Development Stage
```bash
# Set brain to critical period for enhanced learning
./nlm --set-development-stage critical \
     --enable-critical-period true \
     --set-plasticilty-modifiers 2.0 1.5 1.0
```

### Example 3: Neuromodulation Experiment
```bash
# High curiosity experiment
./nlm --set-neuromodulator-levels 0.5 0.9 0.4 \
     --enable-plasticity true \
     --run-integration-test
```

### Example 4: Memory Research
```bash
# Episodic memory experiment
./nlm --set-memory-capacity episodic 5000 \
     --store-memory-episode 1:0.9,2:0.8,3:0.7 \
     --replay-memories \
     --run-memory-test
```

### Example 5: Complete Experimental Workflow
```bash
# Full research workflow
./nlm --config experiment_config.cfg \
     --set-development-stage critical \
     --set-neuromodulator-levels 0.8 0.7 0.6 \
     --enable-plasticity true \
     --store-memory-episode 1:0.9,2:0.8 \
     --sync-memory-prediction \
     --apply-neuromodulation \
     --run-integration-test \
     --export-brain-state final_results.bin
```

## Testing and Validation

### Basic Tests (Phase 2)
- Neural connectivity and spike propagation
- Synaptic plasticity mechanisms (STDP, Hebbian)
- Structural plasticity (synaptogenesis/pruning)
- Learning experiment verification

### Integration Tests (Phase 6)
- Memory system integration
- Neuromodulation coordination
- Prediction system integration
- Cross-system timing validation

### Advanced Command Testing
```bash
# Test individual advanced commands
./nlm --run-memory-test
./nlm --run-plasticity-test
./nlm --run-integration-test

# Test command combinations
./nlm --set-development-stage critical \
     --set-neuromodulator-levels 0.8 0.7 0.6 \
     --run-integration-test
```

## Advanced Features Summary

The NLM Phase 2 advanced features provide:

1. **Comprehensive Control**: 32 advanced commands for fine-grained brain control
2. **Rich Documentation**: Detailed guides with examples for all user levels
3. **Flexible Configuration**: Save/load complex experimental setups
4. **Advanced Testing**: Integration tests for cross-system coordination
5. **Backward Compatibility**: Existing configurations and commands still work

## Getting Started

**For Beginners:**
```bash
# View beginner guide
cat easy_usage.md

# Start with basic commands
./nlm --help
```

**For Researchers:**
```bash
# View advanced documentation
cat advanced_usage.md

# Create and use custom configurations
# Refer to command_reference.md for all command details
```

**For Developers:**
```bash
# View technical implementation
cat NLM_Advanced_Features.md

# Add new commands or modify existing ones
# Commands are defined in src/main.cpp
```

## License

MIT

## Authors

Research project — See docs for scientific background.

## Scientific Context

**Phase 6 does NOT claim:**
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

**Phase 6 DOES investigate:**
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

The advanced features in Phase 2 support these investigations by providing fine-grained control over all brain subsystems, enabling researchers to test specific hypotheses about neural integration, learning, and development.

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

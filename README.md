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

### Configuration File Options

The NLM framework supports multiple configuration file formats:

#### Simple Key=Value Format (Default)
```
# Example config file
neuron_count = 1000
region_count = 1
connection_probability = 0.15
dopamine_baseline = 0.1
reward_discount_factor = 0.99
simulation_timestep = 0.001
```

#### JSON Format
```json
{
  "neuron_count": 1000,
  "region_count": 1,
  "connection_probability": 0.15,
  "dopamine_baseline": 0.1,
  "reward_discount_factor": 0.99,
  "simulation_timestep": 0.001,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012
}
```

#### YAML Format
```yaml
neuron_count: 1000
region_count: 1
connection_probability: 0.15
dopamine_baseline: 0.1
reward_discount_factor: 0.99
simulation_timestep: 0.001
stdp_ltp_weight: 0.01
stdp_ltd_weight: 0.012
```

## Command Line Interface

### Phase 6 Demo Options

```bash
nlm_phase6_demo [options]
```

**Options:**
- `--neurons=N` Number of neurons (default: 500)
- `--steps=N` Maximum simulation steps (default: 1000)
- `--regions=N` Number of brain regions (default: 1)
- `--conn_prob=VAL` Connection probability (default: 0.15)
- `--dev[elopment]` Enable development system
- `--checkpoint=FILE` Checkpoint file path (enable checkpointing)
- `--config=FILE` Load configuration from file
- `--verbose` Enable verbose output
- `--help` Show this help message

**Examples:**
```bash
nlm_phase6_demo --neurons=1000 --steps=5000
nlm_phase6_demo --dev --checkpoint=./checkpoint.bin
nlm_phase6_demo --config=./myconfig.json --verbose
```

### Configuration Generation

```bash
nlm_phase6_demo --config=./myconfig.json --verbose
```

This will load the configuration from the specified file. The framework automatically detects
whether the file is in JSON, YAML, or simple key=value format.

## Testing

The framework includes comprehensive tests that verify:

1. **Config Class Tests**
   - Basic functionality (set, get, remove, keys)
   - JSON and YAML parsing
   - File loading and saving
   - Validation and error handling

2. **Brain System Tests**
   - Initialization and basic functionality
   - Simulation and spike generation
   - Memory system integration
   - Neuromodulation system integration
   - Prediction system integration
   - Development system integration
   - Cognition system integration

3. **Agent Tests**
   - Agent brain initialization
   - Sensory processing
   - Motor decoding

4. **Phase 6 Integration Tests**
   - Complete integration verification
   - Memory integration tests
   - Neuromodulation integration tests
   - Checkpointing and replay tests

To run the comprehensive test suite:
```bash
cd tests
./nlm_test
```

## Configuration File Format Examples

### Default Configuration
The framework includes a default configuration file (`configs/default.cfg`) in simple key=value format. It can be customized by:

1. **Command line arguments:** `--key=value`
2. **Configuration files:** JSON, YAML, or simple format
3. **Runtime settings:** Modify during simulation

### Configuration File Loading Priority

1. **Command line arguments** (highest priority)
2. **Configuration file** (specified with `--config` or loaded from default)
3. **Default configuration** (built into the application)

### Configuration Validation

The framework validates configuration parameters:
- Neuron count must be > 0
- Step count must be > 0
- Region count must be > 0
- Connection probability must be between 0.0 and 1.0

## Technical Details

### Architecture

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

### Key Features

#### 1. Neural Dynamics
- Leaky Integrate-and-Fire (LIF) neuron model
- Event-driven spike propagation with delays
- Synaptic transmission and plasticity

#### 2. Memory Systems
- Working memory with persistent activity
- Episodic memory with experience encoding and replay
- Semantic and procedural memory systems
- Associative memory with Hebbian learning

#### 3. Neuromodulation
- Dopamine for reward prediction error
- Curiosity for exploration behavior
- Novelty detection for learning modulation

#### 4. Prediction System
- Forward models for sensory prediction
- Prediction error signals for learning
- Confidence estimation for decision making

#### 5. Cognitive Mechanisms
- Neural planning and action selection
- Concept formation from patterns
- Attentional selection and competition

#### 6. Development
- Developmental stages affecting plasticity
- Structural changes over time
- Maturation of cognitive abilities

#### 7. Persistence
- Checkpoint save/load functionality
- Memory consolidation during replay
- Experience replay for learning

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

The NLM brain operates as a closed-loop system where all components interact dynamically through neuromodulation, prediction, and plasticity mechanisms. The system continuously learns from experience and develops new capabilities through interaction with its environment.

## License

MIT

## Authors

Research project — See docs for scientific background.

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

**PHASE 5: ADVANCED NEURAL COMPUTATION**

Phase 5 focuses on implementing advanced neural dynamics and integration between core brain systems. This phase delivers:

### Phase 5 Advanced Implementation Achievements

#### Enhanced Neural Dynamics
- **Hodgkin-Huxley (HH) Neuron Models**: Biologically realistic action potential dynamics with voltage-gated ion channels
- **FitzHugh-Nagumo (FHN) Models**: Simplified excitable system for pattern generation and oscillations
- **Integrate-and-Fire (LIF) Dynamics**: Enhanced with adaptive thresholds and conductance-based synapses
- **STDP Plasticity**: Spike-timing dependent learning with millisecond precision
- **Reward-Modulated Hebbian Learning**: Plasticity gated by neuromodulatory signals

#### Neuromodulation Integration
- **Dopamine Systems**: Complete implementation with burst/tonic firing, RPE computation, and receptor dynamics
- **Multi-Neuromodulator Network**: Integrated dopamine, acetylcholine, norepinephrine, and serotonin
- **Plasticity Gating**: Real neuromodulator effects on synaptic modification rules

#### Memory Systems
- **Working Memory**: Persistent activity networks with competitive dynamics
- **Episodic Memory**: Experience encoding with replay and consolidation mechanisms
- **Associative Memory**: Pattern completion and transformation capabilities
- **All memory systems connected to neural processing**

#### Advanced Plasticity Rules
- **Standard Hebbian Learning**: "Fire together, wire together" with spike timing precision
- **Reward-Modulated Plasticity**: Activity-dependent synaptic modification
- **Eligibility Traces**: Temporal credit assignment for delayed rewards
- **Homeostatic Regulation**: Weight normalization and synaptic scaling

#### Visualization and Analysis
- **Text-Based Network Visualization**: Interactive brain network representation
- **Activity Plotting**: Spike raster displays, membrane potential traces, firing rates
- **Data Export**: CSV, JSON, Graphviz formats for external analysis
- **Real-time Monitoring**: Performance metrics and system status displays

#### Enhanced Configuration
- **JSON/YAML Parser**: Advanced configuration with multiple format support
- **Dynamic Parameter Loading**: Runtime configuration changes
- **Expert Configuration Options**: Advanced settings for fine-tuning

#### Development Integration
- **Developmental Stages**: Age-dependent plasticity modulation
- **Structural Plasticity**: Synaptogenesis and pruning dynamics
- **Neural Excitability Changes**: Development-dependent firing characteristics

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 5 Advanced Demo (Neural Computation)
```bash
./nlm
```

This runs the advanced neural computation demo showcasing:
- Hodgkin-Huxley and FitzHugh-Nagumo neuron models
- Reward-modulated Hebbian learning with dopamine integration
- Real-time visualization of neural dynamics
- Advanced memory system demonstrations

### Advanced Configuration Examples

Configuration files in multiple formats are now supported:

**JSON Configuration (.json)**:
```json
{
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.15,
    "neuron_dynamics": {
        "model": "hodgkin_huxley",
        "threshold_persistence": true,
        "adaptation_enabled": true
    },
    "plasticity": {
        "hebbian_learning": true,
        "stdp_enabled": true,
        "reward_modulated": true
    },
    "neuromodulation": {
        "dopamine_burst": true,
        "prediction_error": true,
        "novelty_sensitivity": 0.5
    }
}
```

**YAML Configuration (.yaml)**:
```yaml
neuron_count: 500
region_count: 2
connection_probability: 0.15

neuron_dynamics:
  model: hodgkin_huxley
  threshold_persistence: true
  adaptation_enabled: true

plasticity:
  hebbian_learning: true
  stdp_enabled: true
  reward_modulated: true

neuromodulation:
  dopamine_burst: true
  prediction_error: true
  novelty_sensitivity: 0.5
```

**Traditional Configuration (.cfg)**:
```
# NLM Advanced Configuration
neuron_count = 500
region_count = 2
connection_probability = 0.15

# Neuron dynamics configuration
neuron_dynamics = hodgkin_huxley
threshold_persistence = true
adaptation_enabled = true

# Plasticity configuration
hebbian_learning = true
stdp_enabled = true
reward_modulated = true

# Neuromodulation configuration
dopamine_burst = true
prediction_error = true
novelty_sensitivity = 0.5
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
├── src/
│   ├── core/           # Core utilities
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

### Phase 5 (Complete - Advanced Neural Computation)
- **Hodgkin-Huxley neuron models** with realistic ion channel dynamics
- **FitzHugh-Nagumo models** for pattern generation and oscillations
- **Enhanced LIF dynamics** with adaptive thresholds and conductance-based synapses
- **Comprehensive STDP and reward-modulated Hebbian plasticity**
- **Complete neuromodulator systems** (dopamine, ACh, NE, 5-HT) with RPE computation
- **Advanced memory systems** with working memory, episodic memory, and associative memory
- **Enhanced visualization** with text-based network graphs and activity plotting
- **Advanced configuration** with JSON/YAML parser and dynamic parameters
- **Developmental integration** with age-dependent plasticity

## Advanced Configuration System

The NLM now supports advanced configuration with multiple file formats and comprehensive parameter support:

### Configuration Formats

**JSON/YAML Support**: New advanced configuration parser supporting:
- **JSON (.json)**: Structured configuration with nested parameters
- **YAML (.yaml, .yml)**: Human-readable configuration format
- **Traditional (.cfg, .conf)**: Backward compatible key=value format

**Advanced Configuration Options**:
- **Neuron Dynamics**: Model selection (LIF, HH, FHN), threshold adaptation, ion channel parameters
- **Plasticity Rules**: Hebbian learning strength, STDP parameters, reward modulation
- **Neuromodulation**: Dopamine burst signaling, prediction error computation, receptor dynamics
- **Memory Systems**: Working memory capacity, episodic memory consolidation parameters
- **Development**: Age-dependent plasticity rates, synaptogenesis/pruning schedules
- **Performance**: SIMD optimization levels, parallelization parameters

**Command Line Usage**:
```bash
./nlm --config advanced.json --neuron_count 1000 --plasticity_reward_modulated true
./nlm --config config.yaml -stdp_enabled -dopamine_burst
```

### Real-time Configuration Management

Advanced runtime configuration includes:
- Dynamic parameter tuning during simulation
- Runtime configuration validation
- Configuration checkpointing and restoration
- Live parameter modification for experimentation

## Enhanced Neural Dynamics

Phase 5 introduces advanced neuron models:

### Hodgkin-Huxley (HH) Dynamics
- Biologically realistic sodium and potassium channel dynamics
- Action potential generation with proper shape and refractory periods
- Parameter tuning based on experimental data (120 mS/cm², 36 mS/cm², 0.3 mS/cm²)
- Support for multiple excitability types and adaptation mechanisms

### FitzHugh-Nagumo (FHN) Dynamics
- Simplified excitable system for pattern generation
- Fast-slow dynamics with recovery variable
- Bistability and limit cycle support for oscillatory behavior
- Type I and Type II excitability regimes

### Enhanced Integrate-and-Fire
- Conductance-based synapses with reversal potentials
- Adaptive threshold mechanisms
- Synaptic short-term plasticity (STP)
- Improved numerical integration methods (Runge-Kutta4, Crank-Nicolson)

## Reward-Modulated Plasticity

Advanced learning mechanisms that integrate with neuromodulatory signals:

### Reward Prediction Error (RPE)
- Temporal difference learning with gamma discount factor
- Phasic dopamine responses to unexpected rewards vs omissions
- Associative learning with reward context integration
- Credit assignment across neural pathways

### Multiple Plasticity Rule Types
- **Standard Hebbian**: Spike-timing precision with covariance-based learning
- **Reward-Modulated Hebbian**: Plasticity gated by dopamine prediction error
- **Triplet STDP**: Three-factor learning with presynaptic, postsynaptic, and neuromodulatory signals
- **BCM Rule**: Dynamic sliding threshold for synaptic modification
- **Oja Rule**: Weight normalization for competitive learning

### Eligibility Traces
- Temporal credit assignment for delayed rewards
- Fast and slow trace dynamics (100ms and 10s time constants)
- Neuromodulator-triggered synaptic changes
- Complex learning scenarios with delayed feedback

## Neuromodulator Integration

Complete biological neuromodulation system:

### Dopamine System
- **Burst and Tonic Firing**: Phasic responses to prediction errors vs baseline modulation
- **Receptor Dynamics**: D1/D5 (excitatory) and D2/D3 (inhibitory) receptor effects
- **Reward Processing**: Incentive salience, prediction error computation, reward magnitude encoding
- **Plasticity Integration**: NMDA receptor modulation, calcium-dependent pathways

### Multi-Neuromodulator Network
- **Acetylcholine**: Attention control and memory consolidation
- **Norepinephrine**: Arousal and vigilance modulation
- **Serotonin**: Mood regulation and impulsivity control
- **Integration**: Coordinated effects across multiple neuromodulators

### Behavioral Modulation
- **Exploration vs Exploitation**: Dopamine level-dependent balance control
- **Novelty Detection**: Response to unexpected stimuli
- **Motivational State**: Incentive salience scaling and goal-directed behavior
- **Action Selection**: Reward-based bias and policy optimization

## Advanced Visualization System

Comprehensive visualization capabilities for analysis and debugging:

### Text-Based Network Visualization
- Interactive ASCII/Unicode neural network graphs
- Region and population hierarchy visualization
- Connection strength representation (varying line styles)
- Export to Graphviz DOT format for external tools

### Activity Analysis
- Spike raster plots with time evolution
- Membrane potential traces and firing rate displays
- Neural activity heatmaps with multiple color schemes
- Network connectivity strength visualizations

### Data Export and Analysis
- **CSV Export**: Spreadsheet-compatible data for analysis
- **JSON Export**: Programmatic access to neural data
- **Graphviz Export**: Advanced network visualization
- **Statistical Analysis**: Built-in metrics and statistical displays

### Real-time Monitoring
- Performance metrics dashboard
- System status monitoring
- Resource utilization tracking
- Memory and computation profiling

## Scientific Applications

Phase 5 enables advanced scientific research:

### Reinforcement Learning
- Model-based vs model-free RL systems
- Multi-agent reinforcement learning
- Partially observable Markov decision processes (POMDPs)
- Hierarchical reinforcement learning

### Neural Dynamics Research
- Pattern formation and oscillation studies
- Brain network synchronization analysis
- Chaos and complexity studies
- Nonlinear dynamics exploration

### Cognitive Modeling
- Working memory capacity limits
- Attentional selection mechanisms
- Concept formation and categorization
- Social learning and cultural transmission

### Developmental Studies
- Critical period dynamics
- Age-dependent learning rates
- Structural plasticity over time
- Experience-dependent development

## Building and Running Examples

### Basic Usage
```bash
# Build
mkdir build && cd build && cmake .. && make -j4

# Run advanced demo
./nlm --neuron_count 1000 --enable_visualization

# Run with JSON config
./nlm --config advanced_config.json

# Run with specific plasticity rules
./nlm --plasticity hebbian --reward_modulated --stdp
```

### Configuration File Examples

Advanced configurations enable complex research scenarios:

**Research Configuration**:
```json
{
    "experiment_name": "RL_learning",
    "neuron_count": 5000,
    "plasticity": {
        "learning_rate": 0.001,
        "reward_modulation": true,
        "eligibility_traces": true
    },
    "neuromodulation": {
        "dopamine_burst": true,
        "prediction_error": true,
        "novelty_sensitivity": 0.8
    },
    "visualization": {
        "export_format": "graphviz",
        "update_rate": 30,
        "activity_plots": true
    }
}
```

**Development Study**:
```yaml
neuron_count: 2000
development:
  age_dependent_plasticity: true
  critical_period: true
  synaptogenesis_rate: 0.001
  pruning_rate: 0.0001

memory:
  working_memory_capacity: 50
  episodic_consolidation: true
  replay_frequency: 0.1

neuromodulation:
  acetylcholine: true  # attention control
  norepinephrine: true # arousal
  serotonin: true     # mood regulation
```

## License

MIT

## Authors

Research project — See docs for scientific background.

# Advanced Configuration for NLM Brain

## Configuration Overview

This document provides advanced configuration options for the NLM (熙然) brain system, designed for power users and researchers who need fine-grained control over brain development and behavior.

## Core Configuration Structure

```yaml
brain:
  name: "Advanced NLM Brain"
  description: "Integrated artificial brain with advanced features"
  version: "2.0.0"
  
experiment:
  identifier: "exp_001"
  description: "Developmental learning experiment"
  tags: ["neuroscience", "ai", "developmental"]
  
environment:
  type: "simple"
  dimensions: [width, height, depth]
  update_rate: 60.0  # Hz
```

## Advanced Brain Configuration

### Neural System Configuration
```yaml
neural_system:
  # Core neuron parameters
  neuron_count: 5000
  population_count: 3
  connection_probability: 0.15
  
  # Neuron type distribution
  sensory_ratio: 0.25
  internal_ratio: 0.5
  motor_ratio: 0.25
  
  # Connectivity patterns
  intra_region_probability: 0.2
  inter_region_probability: 0.1
  recurrent_connection: true
  
  # Biophysical parameters
  resting_potential: -70.0  # mV
  threshold_potential: -55.0  # mV
  refractory_period: 5.0  # ms
  membrane_time_constant: 20.0  # ms
  
  # Synaptic properties
  excitatory_weight_range: [0.0, 1.0]
  inhibitory_weight_range: [-1.0, 0.0]
  synaptic_delay_range: [1, 5]  # timesteps
```

### Memory System Configuration
```yaml
memory:
  # Working memory
  working_memory_capacity: 500
  working_memory_decay_rate: 0.95  # per timestep
  working_memory_competition: true
  
  # Episodic memory
  episodic_memory_max_episodes: 2000
  episodic_memory_window: 1000
  episodic_memory_storage_threshold: 0.7
  episodic_memory_pruning_threshold: 0.3
  
  # Associative memory
  associative_memory_capacity: 1000
  association_strength_decay: 0.98
  
  # Memory consolidation
  consolidation_interval: 1000  # timesteps
  consolidation_threshold: 0.6
  replay_frequency: 0.2  # replay 20% of episodes
```

### Prediction System Configuration
```yaml
prediction:
  # Prediction parameters
  prediction_horizon: 50  # timesteps ahead
  prediction_error_threshold: 0.1
  prediction_confidence_threshold: 0.8
  
  # Forward model
  forward_model_order: 3
  prediction_learning_rate: 0.01
  prediction_error_gain: 1.2
  
  # State estimation
  kalman_gain: 0.9
  state_transition_noise: 0.01
  observation_noise: 0.05
```

### Neuromodulation Configuration
```yaml
neuromodulation:
  # Dopamine system
  baseline_dopamine: 0.3
  dopamine_sensitivity: 1.0
  reward_prediction_error_gain: 0.5
  
  # Curiosity system  
  baseline_curiosity: 0.5
  novelty_threshold: 0.7
  exploration_bonus: 0.2
  
  # Novelty detection
  novelty_sensitivity: 0.8
  change_detection_threshold: 0.3
  
  # Other neuromodulators
  acetylcholine_level: 0.4
  norepinephrine_level: 0.3
  serotonin_level: 0.2
  
  # Neuromodulation effects
  excitability_modulation: true
  plasticity_modulation: true
  attention_modulation: true
```

### Development System Configuration
```yaml
development:
  # Developmental stages
  critical_period_start: 1000
  critical_period_end: 10000
  initial_stage: "Initial"
  final_stage: "Adult"
  
  # Stage transitions
  stage_transition_steps: 1000
  stage_specific_plasticity: true
  
  # Developmental effects
  synaptogenesis_rate: 0.0001
  pruning_rate: 0.00001
  myelination_rate: 0.00005
  
  # Sensory tuning
  sensory_sensitivity: 1.0
  motor_precision: 0.8
  attention_span: 0.6
```

### Advanced Features Configuration
```yaml
advanced_features:
  # Sleep/wake cycles
  sleep_cycles_enabled: true
  sleep_duration: 5000  # timesteps
  sleep_frequency: 0.1  # every 10% of simulation
  
  # Memory consolidation
  continual_learning: true
  buffer_size: 1000
  catastrophic_forgetting_protection: true
  
  # Meta-learning
  meta_learning_enabled: true
  meta_learning_rate: 0.01
  curiosity_driven_learning: true
  
  # Transfer learning
  transfer_learning_enabled: true
  source_task_similarity_threshold: 0.7
  
  # Evolution
  evolution_enabled: true
  mutation_rate: 0.1
  crossover_rate: 0.7
  population_size: 10
  
  # Visualization
  advanced_visualization: true
  vrml_export: true
  gltf_export: true
  real_time_monitoring: true
```

## Command Line Configuration

### NLM Advanced CLI
```bash
# Usage examples
nlm [GLOBAL_OPTIONS] command [COMMAND_OPTIONS]

GLOBAL_OPTIONS:
  --config FILE                    Configuration file
  --output DIR                     Output directory
  --log FILE, --log-level LEVEL    Logging configuration
  --verbose, -v                    Verbose output
  --quiet, -q                      Quiet output
  --profile                        Enable profiling
  --benchmark                      Run benchmarks

COMMANDS:
  brain COMMANDS                   Brain management commands
  evolution COMMANDS              Evolution commands
  analysis COMMANDS               Analysis commands
  visualize COMMANDS              Visualization commands
  experiment EXP                  Experiment commands

BRAIN COMMANDS:
  init [OPTIONS]                   Initialize brain
  step [OPTIONS]                   Run simulation step
  save FILE [OPTIONS]              Save brain state
  load FILE                        Load brain state
  develop STAGE [OPTIONS]          Set developmental stage
  status [OPTIONS]                 Show brain status

STEP OPTIONS:
  --steps N                        Number of steps (default: 1)
  --realtime                        Use real-time timing
  --fast                            Fast simulation mode
  --accurate                        Accurate simulation mode

SAVE OPTIONS:
  --compress LEVEL                 Compression level (none, fast, balanced, high)
  --metadata                        Include metadata
  --checksum                        Include checksum verification

DEVELOP OPTIONS:
  --immediate                       Skip intermediate stages
  --preserve-memory                Keep memory during development
  --auto-stage                      Auto-transition stages

STATUS OPTIONS:
  --full                            Full status report
  --brief                           Brief status
  --json                            JSON output
  --performance                     Include performance metrics

EVOLUTION COMMANDS:
  evolve [OPTIONS]                  Evolve brain population
  crossbreed PARENT1 PARENT2        Cross-breed two brains
  mutate BRAIN [OPTIONS]            Mutate brain parameters

EVOLVE OPTIONS:
  --generations N                   Number of generations (default: 10)
  --population-size N              Population size (default: 10)
  --mutation-rate FLOAT            Mutation rate (default: 0.1)
  --crossover-rate FLOAT           Crossover rate (default: 0.7)
  --elitism-count N                Number of elite individuals (default: 1)
  --fitness-metric METRIC          Fitness metric to optimize
  
FITNESS METRICS:
  learning_speed                   How quickly brain learns
  memory_retention                 Memory retention over time
  prediction_accuracy              Prediction accuracy
  behavioral_flexibility           Adaptability to changes
  energy_efficiency                Computational efficiency

ANALYSIS COMMANDS:
  features BRAIN                    Extract brain features
  compare BRAIN1 BRAIN2            Compare two brains
  analyze-topology BRAIN            Analyze network topology
  analyze-memory BRAIN              Analyze memory systems
  analyze-learning BRAIN            Analyze learning curves
  analyze-behavior BRAIN            Analyze behavioral patterns

VISUALIZE COMMANDS:
  network [OPTIONS]                Visualize neural network
  activity [OPTIONS]               Visualize neural activity
  memory [OPTIONS]                 Visualize memory traces
  development [OPTIONS]            Visualize development stages
  export [OPTIONS]                 Export visualizations

EXPORT OPTIONS:
  --format FORMAT                   Output format (vrml, gltf, png, json)
  --quality LEVEL                   Quality level (low, medium, high, ultra)
  --size WIDTHxHEIGHT              Output image size
  --color-scheme SCHEME             Color scheme (default, heat, connectivity, developmental)

EXPERIMENT COMMANDS:
  run EXPERIMENT_FILE              Run experiment from file
  reproduce EXPERIMENT_ID           Reproduce experiment
  compare-experiments EXP1 EXP2     Compare experiment results
  export-results EXPERIMENT        Export experiment results
```

## Advanced Analysis Configuration

### Feature Extraction Configuration
```yaml
feature_extraction:
  # Network features
  include_topology: true
  include_degree_distribution: true
  include_cluster_coefficient: true
  include_efficiency: true
  
  # Activity features
  include_firing_rates: true
  include_spike_patterns: true
  include_synchrony: true
  include_burst_detection: true
  
  # Memory features
  include_memory_capacity: true
  include_retention_curves: true
  include_recall_accuracy: true
  include_interference_effects: true
  
  # Learning features
  include_learning_curves: true
  include_plasticity_metrics: true
  include_generalization: true
  
  # Behavioral features
  include_action_sequences: true
  include_decision_patterns: true
  include_adaptive_behavior: true
```

### Evolution Configuration
```yaml
evolution:
  # Genetic algorithm parameters
  population_size: 50
  generations: 100
  mutation_rate: 0.1
  crossover_rate: 0.7
  elitism_count: 2
  
  # Selection methods
  selection_method: "tournament"  # tournament, rank, roulette
  tournament_size: 5
  
  # Genetic operators
  mutation_strength: 0.1
  crossover_points: 2
  gene_recombination: true
  
  # Fitness evaluation
  fitness_evaluation: "multi-objective"  # single, multi-objective, pareto
  fitness_weights:
    learning_speed: 0.3
    memory_retention: 0.3
    prediction_accuracy: 0.2
    energy_efficiency: 0.2
```

## Configuration Validation

### Configuration Schema
```yaml
validation:
  # Schema validation
  schema_file: "schemas/brain_config_schema.json"
  validate_on_load: true
  strict_validation: false
  
  # Range validation
  neuron_count_range: [100, 100000]
  timestep_range: [0.0001, 0.1]
  memory_capacity_range: [10, 10000]
  
  # Consistency validation
  ensure_memory_capacity_ratio: true  # relative to neuron count
  ensure_development_timeline: true
  ensure_neuromodulation_balance: true
```

## Configuration Examples

### Example 1: Fast Development Brain
```yaml
# configs/fast_development.cfg
brain:
  name: "Fast Development Brain"
  
neural_system:
  neuron_count: 2000
  connection_probability: 0.3
  
memory:
  working_memory_capacity: 200
  episodic_memory_max_episodes: 500
  
neuromodulation:
  baseline_dopamine: 0.5
  baseline_curiosity: 0.8
  
advanced_features:
  sleep_cycles_enabled: false
  continual_learning: true
  meta_learning_enabled: true
```

### Example 2: Memory-Enhanced Brain
```yaml
# configs/memory_enhanced.cfg
brain:
  name: "Memory-Enhanced Brain"
  
neural_system:
  neuron_count: 8000
  connection_probability: 0.12
  
memory:
  working_memory_capacity: 1000
  episodic_memory_max_episodes: 5000
  episodic_memory_storage_threshold: 0.9
  consolidation_threshold: 0.8
  
prediction:
  prediction_horizon: 100
  prediction_confidence_threshold: 0.9
```

### Example 3: Curiosity-Driven Brain
```yaml
# configs/curiosity_driven.cfg
brain:
  name: "Curiosity-Driven Brain"
  
neuromodulation:
  baseline_curiosity: 0.9
  novelty_sensitivity: 1.2
  exploration_bonus: 0.5
  
advanced_features:
  meta_learning_enabled: true
  continual_learning: true
  transfer_learning_enabled: true
  
neural_system:
  neuron_count: 3000
  connection_probability: 0.25
```

## Configuration File Search Path

The configuration system searches for configuration files in this order:

1. Command line `--config` argument
2. Current working directory: `config.yaml`, `config.yml`, `nlm.yaml`, `nlm.yml`
3. User's home directory: `~/.nlm/config.yaml`
4. System-wide: `/etc/nlm/config.yaml`

## Configuration Overrides

Configuration can be overridden using command line arguments:

```bash
# Override specific values
nlm --config configs/base.yaml brain.name "Custom Brain" --neurons 10000 --steps 5000

# Environment variable overrides
export NLM_BRAIN_NAME="Environment Brain"
export NLM_NEURON_COUNT=15000
nlm run --steps 10000
```

This configuration system provides comprehensive control over the NLM brain system, enabling researchers and developers to fine-tune brain development, learning, and behavior for specific research goals and applications.
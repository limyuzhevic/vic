# Improved CLI Design for NLM

## Overview
The NLM (Neural Learning Machine) project has a sophisticated brain simulation with integrated cognitive systems, but its command-line interface is very basic and lacks the features needed for effective usage and experimentation.

## Problems with Current CLI

### 1. No Help/Usage Information
- Users have no idea what the executable can do
- No indication of available options or modes
- No examples of usage

### 2. Limited Configuration Options
- Only supports `--config` flag
- No way to override specific parameters via CLI
- No validation or type checking

### 3. Poor User Experience
- No progress indicators
- Basic output format
- No verbose/quiet modes

### 4. No Mode Support
- Single monolithic mode (main.cpp)
- No separation between different test/experiment types
- No integration with existing demo/experiment executables

### 5. Missing Features
- No result export options (CSV, JSON)
- No custom experiment support
- No benchmark mode

## Proposed Improvements

### 1. Comprehensive Help System

```bash
Usage: nlm [OPTIONS] [COMMAND] [ARGS...]

NLM - Neural Learning Machine: An experimental artificial developmental brain

Commands:
  main           Run main simulation (default)
  phase4-demo    Run Phase 4 cognitive experiments
  phase6-demo    Run Phase 6 integration demo
  experiment     Run custom experiments
  benchmark      Run performance benchmarks
  config         Configuration management

Options:
  -h, --help     Show this help message
  -v, --verbose  Increase output verbosity
  -q, --quiet    Decrease output verbosity
  -V, --version  Show version information
  --log-level    Set log level (trace, debug, info, warn, error, critical)
  --config FILE  Configuration file (default: configs/default.cfg)
  --output DIR   Output directory for results and logs
  --export FMT   Export results in format (csv, json, txt)
  --timeout SEC  Maximum execution time (default: 300)
  --threads N    Number of threads to use (default: auto)

Main Options:
  --neurons N    Number of neurons (default: 500)
  --regions N    Number of brain regions (default: 1)
  --timestep T   Simulation timestep (default: 0.001)
  --seed S       Random seed for reproducibility (default: 42)
  --steps N      Number of simulation steps (default: 1000)
```

### 2. Advanced Configuration System

#### Configuration File Format (.cfg)

```ini
# NLM Configuration File
# ==============================

[brain]
# Core brain parameters
neuron_count = 1000
region_count = 2
connection_probability = 0.1
initial_weight_mean = 0.5
initial_weight_std = 0.1
v_thresh = -50.0
v_rest = -70.0
v_reset = -75.0
tau_mem = 20.0
tau_ref = 2.0

[plasticity]
# Plasticity rule parameters
enable_stdp = true
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
enable_hebbian = true
enable_structural = true
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

[neuromodulation]
# Neuromodulator parameters
enable_dopamine = true
dopamine_scale = 1.0
enable_curiosity = true
enable_novelty = true
curiosity_weight = 0.5
novelty_threshold = 0.1

[memory]
# Memory system parameters
enable_working_memory = true
working_memory_capacity = 50
wm_decay_rate = 0.01
enable_episodic_memory = true
episodic_storage_limit = 1000
enable_predictive_memory = true

[cognition]
# Cognitive system parameters
enable_planning = true
planning_horizon = 5
enable_concept_formation = true
concept_formulation_threshold = 0.7
enable_self_model = true

[environment]
# World simulation parameters
world_width = 20
world_height = 20
vision_width = 8
vision_height = 8

[development]
# Developmental parameters
enable_development = true
initial_stage = "initial"
critical_period_end = 100
maturation_rate = 0.001

[output]
# Output and logging
log_level = "info"
log_file = "nlm.log"
export_results = true
result_format = "csv"
verbose_mode = false
```

#### CLI Parameter Override

```bash
# Override specific parameters from command line
nlm --neurons 2000 --regions 3 \
    --stdp_ltp_weight 0.02 \
    --stdp_ltd_weight 0.018 \
    --enable_curiosity \
    --enable_development \
    --steps 5000

# Run with custom configuration file
nlm --config my_config.cfg --neurons 1500 --steps 10000
```

### 3. Subcommand-Based Architecture

#### 1. Main Simulation (`nlm main`)

```python
# Run a basic simulation with configurable options
nlm main \
    --neurons 1000 \
    --steps 2000 \
    --enable_plasticity \
    --enable_memory \
    --log-level info \
    --export csv results/main_simulation.csv
```

#### 2. Phase 4 Experiments (`nlm phase4-demo`)

```python
# Run comprehensive cognitive experiments
nlm phase4-demo \
    --neurons 1500 \
    --trials 100 \
    --experiment temporal-prediction \
    --experiment working-memory \
    --experiment episodic-memory \
    --experiment concept-formation \
    --output-file experiments_results.csv
```

#### 3. Phase 6 Integration (`nlm phase6-demo`)

```python
# Run integration verification and full demo
nlm phase6-demo \
    --neurons 800 \
    --max-steps 3000 \
    --enable-checkpoint \
    --enable-replay \
    --enable-development \
    --verify-integration \
    --show-progress
```

#### 4. Custom Experiments (`nlm experiment`)

```python
# Run user-defined experiment sequences
nlm experiment \
    --script custom_experiment.py \
    --parameters "{'learning_rate': 0.01, 'hidden_size': 512}" \
    --repeats 5 \
    --output-format json
```

#### 5. Benchmark Mode (`nlm benchmark`)

```python
# Performance benchmarking
nlm benchmark \
    --mode neural-computation \
    --neuron-counts "100,500,1000,2000" \
    --steps-per-count 1000 \
    --threads "1,2,4,8" \
    --output-format json benchmark_results.json
```

#### 6. Configuration Management (`nlm config`)

```bash
# List all configuration options
nlm config list

# Get value for a specific parameter
nlm config get brain.neuron_count

# Set configuration value
nlm config set brain.neuron_count 2000

# Validate configuration
nlm config validate my_config.cfg

# Show configuration differences
nlm config diff default.cfg my_config.cfg
```

### 4. Enhanced Output and Logging

#### 1. Progress Indicators

```bash
# Show progress with timestamps and performance metrics
nlm phase6-demo --steps 5000 --show-progress --show-performance

# Progress output example:
[STEP 1250/5000] 23.7% | Neurons: 847/847 | Spikes: 12456 | Memory: 128/1000 | Time: 12.5s | Speed: 104.5 steps/s
```

#### 2. Structured Output Formats

##### CSV Output

```csv
step,total_neurons,firing_neurons,total_spikes,working_memory_episodes,episodic_memory_retention,dopamine_level,reward,prediction_error
0,1000,12,1234,0,0.0,0.0,0.0,0.0
100,1000,87,12567,5,0.3,0.1,12.5,0.45
500,1000,156,14821,23,0.67,0.4,87.2,0.23
1000,1000,203,18945,41,0.82,0.7,156.8,0.12
```

##### JSON Output

```json
{
  "experiment": "phase6_integration",
  "timestamp": "2026-10-16T09:21:37Z",
  "configuration": {
    "neurons": 1000,
    "regions": 1,
    "timestep": 0.001,
    "random_seed": 42
  },
  "results": {
    "total_reward": 234.5,
    "avg_firing_rate": 0.203,
    "memory_episodes_stored": 42,
    "working_memory_integrated": true,
    "episodic_memory_integrated": true,
    "neuromodulation_integrated": true,
    "prediction_integrated": true,
    "development_integrated": true
  },
  "metrics": {
    "memory_retention": 0.82,
    "prediction_accuracy": 0.95,
    "planning_confidence": 0.78,
    "body_awareness": 0.91
  },
  "timing": {
    "wall_clock_seconds": 45.2,
    "cpu_seconds": 42.1,
    "simulation_speed": 22.1
  }
}
```

### 5. Integration with Existing Infrastructure

#### 1. Connect to Phase 6 Demo

```bash
# Use improved CLI to run Phase 6 integration
nlm phase6-demo \
    --neurons 500 \
    --max-steps 2000 \
    --enable-checkpoint \
    --enable-replay \
    --enable-development \
    --verify-integration \
    --output integration_results.json \
    --log-level debug
```

#### 2. Integration with Phase 4 Experiments

```bash
# Run comprehensive Phase 4 experiments with enhanced CLI
nlm phase4-demo 100 \
    --neurons 1500 \
    --trial-multiplier 2 \
    --output-format csv \
    --output experiments/phase4_results.csv \
    --export-plots \
    --plot-format png \
    --plot-dir plots/phase4 \
    --show-progress \
    --verbose
```

#### 3. Custom Experiment Runner

```bash
# Create custom experiment using CLI configuration
nlm experiment \
    --template custom_experiment \
    --parameters "learning_rate=0.01,hidden_size=512,batch_size=32" \
    --config experiment_config.cfg \
    --max-runtime 3600 \
    --save-checkpoints \
    --results-dir results/experiment_$(date +%Y%m%d_%H%M%S)
```

### 6. Advanced Features

#### 1. Multi-Format Result Export

```bash
# Export results in multiple formats
nlm phase6-demo --export csv,json,html \
    --output results/experiment_20261016 \
    --csv-delimiter ';' \
    --json-indent 2 \
    --html-title "NLM Phase 6 Integration Results"
```

#### 2. Real-time Monitoring

```bash
# Monitor simulation in real-time
nlm phase6-demo --monitor --watch-interval 1 --output live_monitor.html

# Live monitoring dashboard
# Shows: neurons firing, memory usage, reward, prediction error, system health
```

#### 3. Automated Experiment Management

```bash
# Run automated experiment suite
nlm experiment-suite \
    --grid-search \
    --parameters "neurons=[500,1000,2000],learning_rate=[0.001,0.01,0.1]" \
    --grid-search-count 20 \
    --parallel-executions 4 \
    --results-dir hyperopt_results \
    --continue-on-failure
```

### 7. Configuration File Features

#### 1. Advanced Configuration Syntax

```ini
# Include other configuration files
include = base_config.cfg
include = custom_settings.cfg

# Environment variable substitution
neuron_count = ${NLM_NEURONS:-1000}
random_seed = ${NLM_SEED:-42}

# Conditional configuration
[plasticity]
enable_stdp = ${NLM_ENABLE_STDP:-true}
stdp_ltp_weight = ${NLM_STDP_LTP:-0.01}
stdp_ltd_weight = ${NLM_STDP_LTD:-0.012}

# Profile configuration sections
[profile.production]
brain.neuron_count = 5000
plasticity.enable_structural = false
memory.enable_working_memory = true

[profile.research]
brain.neuron_count = 100
plasticity.enable_structural = true
memory.enable_working_memory = true
memory.enable_episodic_memory = true

# Use profile
profile = research
```

## Implementation Benefits

1. **Better User Experience**: Comprehensive help, progress indicators, and structured output
2. **Enhanced Research Capabilities**: Automated experiment management, grid search, monitoring
3. **Improved Integration**: Connects all existing Phase 4 and Phase 6 systems
4. **Flexible Configuration**: Multiple configuration sources with override capabilities
5. **Modern Development**: Follows standard command-line conventions and best practices
6. **Backward Compatibility**: Existing scripts and configurations continue to work
7. **Extensibility**: Easy to add new subcommands and features

## Impact on Phase 6 Integration

The improved CLI will significantly enhance the Phase 6 integration effort by:

1. **Better Testing**: Comprehensive experiment runner for validating system integration
2. **Monitoring**: Real-time monitoring of integration status and performance
3. **Result Analysis**: Structured output for quantitative analysis of integration quality
4. **Configuration Management**: Easy testing of different integration parameters
5. **Automation**: Automated testing and validation of integration features
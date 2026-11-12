# CLI Tools for NLM

## Overview

This document describes the command-line interface (CLI) tools for the NLM (Neural Learning Machine) project. It provides comprehensive documentation for the CLI tools, including examples, options, and usage instructions.

## Current State

### Existing CLI Tools

The current NLM project has the following CLI tools:
1. **main** (Phase 2 demo) - Basic neural computation demonstration
2. **Phase6Demo** - Integration test demonstration
3. **Phase6IntegratedExperiment** - Full integration experiment

### Current Limitations

The current CLI tools have several limitations:
1. **Limited Functionality** - Only provides basic demo functionality
2. **Poor Documentation** - Insufficient documentation and examples
3. **Inconsistent Interface** - Inconsistent command-line options across tools
4. **No Advanced Features** - Missing advanced configuration and analysis options
5. **Poor Error Handling** - Basic error handling without detailed feedback

## Enhanced CLI Tools

### Core Functionality

#### General Options

All NLM CLI tools support the following general options:

```bash
# General options
--help                    Show help message
--version                 Show version information
--config <file>           Load configuration from file
--verbose                 Enable verbose output
--quiet                   Suppress normal output
--log-level <level>       Set log level (debug, info, warn, error, fatal)
--log-file <file>         Set log file path
--output <file>           Set output file path
--daemon                  Run in daemon mode
--pid-file <file>          Set PID file path
```

#### Brain Simulation Options

```bash
# Brain simulation options
--neuron-count <n>        Number of neurons (default: 1000)
--region-count <n>        Number of regions (default: 1)
--simulation-timestep <t>  Simulation timestep (default: 0.001)
--random-seed <s>         Random seed (default: 42)
--connection-probability <p> Connection probability (default: 0.1)
--enable-plasticity       Enable plasticity
--enable-development      Enable development system
--enable-neuromodulation   Enable neuromodulation
--enable-memory           Enable memory systems
```

#### Memory System Options

```bash
# Memory system options
--memory-capacity <n>      Working memory capacity (default: 100)
--episodic-memory-max <n>  Maximum episodic memories (default: 1000)
--associative-memory-strength <s> Associative memory strength threshold (default: 0.3)
--enable-replay            Enable memory replay
--replay-interval <n>     Memory replay interval (default: 100)
--consolidation-interval <n> Memory consolidation interval (default: 10000)
```

#### Neuromodulation Options

```bash
# Neuromodulation options
--dopamine-strength <s>    Dopamine strength (default: 1.0)
--curiosity-threshold <t>  Curiosity threshold (default: 0.3)
--novelty-decay <d>        Novelty decay rate (default: 0.99)
--prediction-error-weight <w> Prediction error weight (default: 1.0)
```

#### Development System Options

```bash
# Development system options
--developmental-stage <stage> Developmental stage (initial, critical, mature, adult)
--plasticity-modifier-rate <r> Plasticity modifier rate (default: 0.1)
--enable-structural-plasticity Enable structural plasticity
--synaptogenesis-rate <r>  Synaptogenesis rate (default: 0.0001)
--pruning-rate <r>         Pruning rate (default: 0.00001)
```

#### Performance Options

```bash
# Performance options
--checkpoint-interval <n>    Checkpoint interval (default: 1000)
--max-memory-usage <m>       Maximum memory usage in MB (default: 1024)
--parallel-processing         Enable parallel processing
--vectorization                Enable vectorization
--event-queue-size <n>        Event queue size (default: 10000)
```

#### Output Options

```bash
# Output options
--output-format <format>     Output format (text, json, csv, xml) (default: text)
--output-file <file>          Output file path
--output-directory <dir>      Output directory path
--include-metrics             Include performance metrics
--include-timestamps          Include timestamps in output
--precision <p>              Output precision (default: 6)
```

### Tool Descriptions

#### 1. nlm-run

**Description**: Run NLM brain simulation with full configuration options.

**Usage**:
```bash
nlm-run [options]
```

**Options**:
```bash
# Required options
--simulation-steps <n>     Number of simulation steps

# Other options (see general options above)
```

**Example**:
```bash
nlm-run \
    --neuron-count 2000 \
    --simulation-timestep 0.001 \
    --connection-probability 0.15 \
    --enable-plasticity \
    --enable-development \
    --enable-neuromodulation \
    --enable-memory \
    --memory-capacity 200 \
    --episodic-memory-max 5000 \
    --simulation-steps 10000
```

#### 2. nlm-explore

**Description**: Explore NLM functionality with interactive analysis.

**Usage**:
```bash
nlm-explore [options]
```

**Options**:
```bash
--mode <mode>               Exploration mode (brain, memory, neuromodulation, development, all)
--explore-steps <n>         Number of exploration steps
--save-results               Save exploration results
--results-file <file>        Results file path
```

**Example**:
```bash
nlm-explore --mode all --explore-steps 5000 --save-results
```

#### 3. nlm-analyze

**Description**: Analyze NLM simulation results and performance.

**Usage**:
```bash
nlm-analyze <input-file> [options]
```

**Options**:
```bash
--output-format <format>    Output format (default: text)
--include-plots              Include visualization plots
--plot-format <format>       Plot format (png, jpg, pdf) (default: png)
--analysis-type <type>       Analysis type (performance, memory, neuromodulation, all)
--threshold <t>              Analysis threshold
--baseline <file>            Baseline results file
```

**Example**:
```bash
nlm-analyze simulation_results.txt --output-format json --include-plots
```

#### 3. nlm-benchmark

**Description**: Run performance benchmarks and benchmarks NLM components.

**Usage**:
```bash
nlm-benchmark [options]
```

**Options**:
```bash
--benchmark-type <type>     Benchmark type (brain, memory, neuromodulation, development, all)
--num-runs <n>              Number of benchmark runs (default: 10)
--output-format <format>    Output format (text, json, csv) (default: text)
--save-detailed-results      Save detailed benchmark results
```

**Example**:
```bash
nlm-benchmark --benchmark-type all --num-runs 20
```

#### 4. nlm-config

**Description**: Configure NLM settings and validation.

**Usage**:
```bash
nlm-config [options] <action>
```

**Actions**:
```bash
set <key> <value>    Set configuration value
get <key>             Get configuration value
validate              Validate configuration
list                  List all configuration values
export <format>        Export configuration to file
import <file>          Import configuration from file
```

**Options**:
```bash
--config-file <file>      Configuration file path
--interactive            Interactive configuration mode
--backup                 Create backup before modification
```

**Example**:
```bash
nlm-config set neuron_count 2000
nlm-config validate
nlm-config export json > config.json
```

#### 5. nlm-monitor

**Description**: Monitor NLM simulation in real-time.

**Usage**:
```bash
nlm-monitor [options]
```

**Options**:
```bash
--simulation-steps <n>    Number of steps to monitor
--update-interval <t>      Update interval in seconds (default: 1.0)
--metrics <metrics>        Metrics to monitor (comma-separated list)
--output <file>            Output file path
--web-dashboard            Start web dashboard
```

**Example**:
```bash
nlm-monitor --simulation-steps 10000 --update-interval 0.5 --metrics spike_count,learning_rate,memory_usage
```

### CLI API

#### Configuration Management

```bash
# Set configuration value
nlm-config set <key> <value>

# Get configuration value
nlm-config get <key>

# Validate configuration
nlm-config validate

# List all configuration values
nlm-config list

# Export configuration
nlm-config export json > config.json

# Import configuration
nlm-config import config.json
```

#### Brain Control

```bash
# Initialize brain
nlm-brain initialize

# Run simulation steps
nlm-brain step <n>

# Inject current into neuron
nlm-brain inject <neuron_id> <current>

# Get brain statistics
nlm-brain stats
```

#### Memory Control

```bash
# Store episode in memory
nlm-memory store-episode <episode_data>

# Retrieve similar episodes
nlm-memory retrieve-similar <pattern> <max_results>

# Clear memory
nlm-memory clear
```

#### Neuromodulation Control

```bash
# Apply neuromodulation
nlm-neuromodulation apply-reward <reward> <predicted_reward>

# Get neuromodulation level
nlm-neuromodulation get-level
```

#### Development Control

```bash
# Update development
nlm-development update <timestep>

# Get developmental stage
nlm-development get-stage
```

### CLI Examples

#### Example 1: Basic Simulation

```bash
# Run basic simulation
nlm-run \
    --neuron-count 1000 \
    --simulation-timestep 0.001 \
    --simulation-steps 10000
```

#### Example 2: Memory-Enhanced Simulation

```bash
# Run simulation with enhanced memory systems
nlm-run \
    --neuron-count 2000 \
    --enable-memory \
    --memory-capacity 300 \
    --episodic-memory-max 5000 \
    --enable-replay \
    --simulation-steps 20000 \
    --output-file simulation_results.json
```

#### Example 3: Neuromodulation-Enhanced Simulation

```bash
# Run simulation with neuromodulation
nlm-run \
    --neuron-count 1500 \
    --enable-neuromodulation \
    --dopamine-strength 2.0 \
    --curiosity-threshold 0.5 \
    --simulation-steps 15000
```

#### Example 4: Development-Enhanced Simulation

```bash
# Run simulation with development system
nlm-run \
    --neuron-count 1200 \
    --enable-development \
    --developmental-stage mature \
    --enable-structural-plasticity \
    --simulation-steps 12000
```

#### Example 5: Analysis of Simulation Results

```bash
# Analyze simulation results
nlm-analyze simulation_results.json \
    --output-format json \
    --include-plots \
    --plot-format png \
    --analysis-type all
```

#### Example 6: Performance Benchmarking

```bash
# Run performance benchmarks
nlm-benchmark \
    --benchmark-type all \
    --num-runs 10 \
    --output-format csv \
    --save-detailed-results
```

#### Example 7: Configuration Management

```bash
# Set configuration values
nlm-config set neuron_count 2000
nlm-config set simulation_timestep 0.001
nlm-config set connection_probability 0.15

# Validate configuration
nlm-config validate

# Export configuration
nlm-config export json > config.json

# Import configuration
nlm-config import config.json
```

#### Example 8: Real-Time Monitoring

```bash
# Monitor simulation in real-time
nlm-monitor \
    --simulation-steps 10000 \
    --update-interval 0.5 \
    --metrics spike_count,learning_rate,memory_usage \
    --output monitoring_results.json
```

### Configuration File Format

NLM CLI tools support the following configuration file formats:

#### 1. JSON Configuration

```json
{
    "simulation": {
        "neuron_count": 1000,
        "region_count": 1,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "connection_probability": 0.1
    },
    "neural": {
        "neuron_count": 1000,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
    },
    "memory": {
        "working_memory_capacity": 100,
        "episodic_memory_max_episodes": 1000,
        "associative_memory_strength_threshold": 0.3
    },
    "neuromodulation": {
        "dopamine_strength": 1.0,
        "curiosity_threshold": 0.3,
        "novelty_decay_rate": 0.99,
        "prediction_error_weight": 1.0
    },
    "development": {
        "enable_development": true,
        "developmental_stage": "initial",
        "plasticity_modifier_rate": 0.1
    },
    "logging": {
        "level": "info",
        "file_path": "./logs/nlm.log",
        "console_output": true
    }
}
```

#### 2. INI Configuration

```ini
[simulation]
neuron_count = 1000
region_count = 1
simulation_timestep = 0.001
random_seed = 42
connection_probability = 0.1

[neural]
neuron_count = 1000
connection_probability = 0.1

[plasticity]
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

[memory]
working_memory_capacity = 100
episodic_memory_max_episodes = 1000
associative_memory_strength_threshold = 0.3

[neuromodulation]
dopamine_strength = 1.0
curiosity_threshold = 0.3
novelty_decay_rate = 0.99
prediction_error_weight = 1.0

[development]
enable_development = true
developmental_stage = initial
plasticity_modifier_rate = 0.1

[logging]
level = info
file_path = ./logs/nlm.log
console_output = true
```

#### 3. YAML Configuration

```yaml
simulation:
  neuron_count: 1000
  region_count: 1
  simulation_timestep: 0.001
  random_seed: 42
  connection_probability: 0.1

neural:
  neuron_count: 1000
  connection_probability: 0.1

plasticity:
  stdp_ltp_weight: 0.01
  stdp_ltd_weight: 0.012
  stdp_tau: 20.0
  synaptogenesis_rate: 0.0001
  pruning_rate: 0.00001

memory:
  working_memory_capacity: 100
  episodic_memory_max_episodes: 1000
  associative_memory_strength_threshold: 0.3

neuromodulation:
  dopamine_strength: 1.0
  curiosity_threshold: 0.3
  novelty_decay_rate: 0.99
  prediction_error_weight: 1.0

development:
  enable_development: true
  developmental_stage: initial
  plasticity_modifier_rate: 0.1

logging:
  level: info
  file_path: ./logs/nlm.log
  console_output: true
```

### Output Formats

NLM CLI tools support the following output formats:

#### 1. Text Format

```text
# NLM Simulation Results
Simulation Step: 10000
Total Neurons: 1000
Total Synapses: 45234
Total Spikes: 12345
Average Firing Rate: 12.34 Hz
Working Memory Size: 50
Episodic Memory Size: 234
Neuromodulation Level: 0.567
Developmental Stage: mature
Simulation Time: 10.0 seconds
```

#### 2. JSON Format

```json
{
    "simulation_step": 10000,
    "total_neurons": 1000,
    "total_synapses": 45234,
    "total_spikes": 12345,
    "average_firing_rate": 12.34,
    "working_memory_size": 50,
    "episodic_memory_size": 234,
    "neuromodulation_level": 0.567,
    "developmental_stage": "mature",
    "simulation_time": 10.0
}
```

#### 3. CSV Format

```csv
simulation_step,total_neurons,total_synapses,total_spikes,average_firing_rate,working_memory_size,episodic_memory_size,neuromodulation_level,developmental_stage,simulation_time
10000,1000,45234,12345,12.34,50,234,0.567,mature,10.0
```

#### 4. XML Format

```xml
<?xml version="1.0" encoding="UTF-8"?>
<simulation_results>
    <simulation_step>10000</simulation_step>
    <total_neurons>1000</total_neurons>
    <total_synapses>45234</total_synapses>
    <total_spikes>12345</total_spikes>
    <average_firing_rate>12.34</average_firing_rate>
    <working_memory_size>50</working_memory_size>
    <episodic_memory_size>234</episodic_memory_size>
    <neuromodulation_level>0.567</neuromodulation_level>
    <developmental_stage>mature</developmental_stage>
    <simulation_time>10.0</simulation_time>
</simulation_results>
```

### Error Handling

NLM CLI tools provide comprehensive error handling with detailed error messages:

#### 1. Configuration Errors

```bash
$ nlm-run --neuron-count -1
ERROR: Configuration error: neuron_count must be positive
Usage: nlm-run [options]
See --help for more information.
```

#### 2. File Errors

```bash
$ nlm-run --config non_existent_file.json
ERROR: File not found: non_existent_file.json
Usage: nlm-run [options]
See --help for more information.
```

#### 3. Simulation Errors

```bash
$ nlm-run --simulation-steps 0
ERROR: Simulation steps must be greater than 0
Usage: nlm-run [options]
See --help for more information.
```

#### 4. Resource Errors

```bash
$ nlm-run --neuron-count 1000000
ERROR: Insufficient memory: requested 1000000 neurons, available: 100000
Usage: nlm-run [options]
See --help for more information.
```

### Logging

NLM CLI tools provide comprehensive logging with multiple levels:

#### Log Levels

1. **DEBUG**: Detailed debug information
2. **INFO**: General information messages
3. **WARN**: Warning messages
4. **ERROR**: Error messages
5. **FATAL**: Fatal error messages

#### Log Format

```text
2024-01-01 12:00:00.000 [INFO] NLM Simulation Started
2024-01-01 12:00:01.000 [DEBUG] Configuration loaded successfully
2024-01-01 12:00:02.000 [INFO] Brain initialized with 1000 neurons
2024-01-01 12:00:03.000 [DEBUG] Processing sensory input
2024-01-01 12:00:04.000 [INFO] Simulation step completed
2024-01-01 12:00:05.000 [WARN] Low memory usage detected
2024-01-01 12:00:06.000 [ERROR] Spike generation error
2024-01-01 12:00:07.000 [FATAL] Simulation terminated
```

### Configuration Management Examples

#### 1. Interactive Configuration

```bash
$ nlm-config --interactive
> set neuron_count 2000
> set simulation_timestep 0.001
> set connection_probability 0.15
> validate
> save config.json
```

#### 2. Scripted Configuration

```bash
#!/bin/bash
# Configure NLM
nlm-config set neuron_count 1000
nlm-config set simulation_timestep 0.001
nlm-config set connection_probability 0.1

# Validate configuration
nlm-config validate || exit 1

# Export configuration
nlm-config export json > config.json

# Run simulation
nlm-run --simulation-steps 10000

# Analyze results
nlm-analyze simulation_results.json
```

### Performance Monitoring Examples

#### 1. Real-Time Monitoring

```bash
# Start real-time monitoring
nlm-monitor --simulation-steps 10000 --update-interval 0.5 --metrics spike_count,learning_rate

# Monitor with web dashboard
nlm-monitor --simulation-steps 10000 --web-dashboard
```

#### 2. Post-Simulation Analysis

```bash
# Analyze simulation results
nlm-analyze simulation_results.json \
    --output-format json \
    --include-plots \
    --plot-format png \
    --analysis-type all
```

#### 3. Performance Benchmarking

```bash
# Run performance benchmarks
nlm-benchmark --benchmark-type all --num-runs 10

# Analyze benchmark results
nlm-analyze benchmark_results.json --analysis-type performance
```

### Installation and Setup

#### Requirements

```bash
# Install NLM CLI tools
apt-get install nlm-cli

# Or install from source
git clone https://github.com/nlm-project/nlm.git
cd nlm
make install
```

#### Setup Instructions

```bash
# Clone NLM repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build CLI tools
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install CLI tools
sudo make install

# Create configuration directory
mkdir -p ~/.config/nlm
cp configs/default.cfg ~/.config/nlm/
```

#### Testing

```bash
# Test CLI tools
nlm-run --help
nlm-explore --help
nlm-analyze --help
nlm-benchmark --help
nlm-config --help
nlm-monitor --help

# Run integration tests
nlm-run --neuron-count 1000 --simulation-steps 1000
nlm-explore --mode brain --explore-steps 500
nlm-analyze --help
nlm-benchmark --benchmark-type brain
```

## Integration and Automation

### CI/CD Integration

```yaml
# GitHub Actions workflow
deps:
  - cmake --build-type=Release

steps:
  - name: Build NLM
    run: |
      mkdir build &&
      cd build &&
      cmake .. &&
      make -j4

  - name: Test NLM CLI
    run: |
      ./nlm-run --neuron-count 1000 --simulation-steps 1000

  - name: Run benchmarks
    run: |
      ./nlm-benchmark --benchmark-type brain --num-r�ns 10

  - name: Analyze results
    run: |
      ./nlm-analyze simulation_results.json
```

### Docker Integration

```dockerfile
FROM ubuntu:20.04

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    make \
    python3 \
    python3-pip

WORKDIR /app

COPY . /app
RUN mkdir build && cd build && cmake .. && make -j4

ENV PATH=/app/build:$PATH

CMD ["nlm-run", "--neuron-count", "1000", "--simulation-steps", "1000"]
```

### Configuration Management Examples

#### Environment-Based Configuration

```bash
# Set environment variables
export NLM_NEURON_COUNT=2000
export NLM_SIMULATION_TIMESTEP=0.001
export NLM_CONNECTION_PROBABILITY=0.15

# Run NLM with environment configuration
nlm-run --simulation-steps 10000
```

#### Database Configuration

```bash
# Store configuration in database
CREATE TABLE nlm_config (
    key VARCHAR(100) PRIMARY KEY,
    value TEXT,
    description TEXT
);

INSERT INTO nlm_config VALUES
    ('neuron_count', '1000', 'Number of neurons'),
    ('simulation_timestep', '0.001', 'Simulation timestep'),
    ('connection_probability', '0.1', 'Connection probability');

# Load configuration from database
nlm-config import from-database
```

### Conclusion

The enhanced CLI tools provide a comprehensive, user-friendly interface to NLM's advanced neural computation capabilities from the command line. Key features include:

1. **Flexible Configuration Management**: Support for multiple configuration file formats and command-line options.
2. **Comprehensive Simulation Control**: Fine-grained control over brain simulation parameters.
3. **Advanced Analysis Tools**: Powerful analysis and visualization capabilities.
4. **Performance Monitoring**: Real-time monitoring and benchmarking.
5. **Error Handling**: Comprehensive error handling with detailed error messages.
6. **Logging**: Extensive logging with multiple levels and formats.
7. **Integration**: Easy integration with CI/CD pipelines and Docker containers.
8. **Automation**: Support for automated configuration and execution.

The enhanced CLI tools make it easy for researchers, developers, and users to leverage NLM's advanced neural computation capabilities from the command line, opening up new possibilities for experimentation, research, and application development.
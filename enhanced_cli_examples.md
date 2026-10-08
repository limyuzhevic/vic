# Enhanced CLI Examples
# Command-Line Interface Examples for NLM Phase 2

This document provides comprehensive examples for using the enhanced command-line interface of the NLM neural learning machine.

## Table of Contents

1. [Quick Start Examples](#quick-start-examples)
2. [Configuration Management Examples](#configuration-management-examples)
3. [Performance and Monitoring Examples](#performance-and-monitoring-examples)
4. [Session Management Examples](#session-management-examples)
5. [Quality and Validation Examples](#quality-and-validation-examples)
6. [Integration Examples](#integration-examples)

## Quick Start Examples

### 1.1 Basic Usage

**Quick Test (Default Configuration)**
```bash
# Run basic simulation with 1000 neurons for 10000 steps
./nlm
```

**Result:** Outputs basic neural network simulation with default parameters.

**Verbose Output:**
```bash
./nlm --verbose
```

### 1.2 Preset Usage

**Quick Preset (Small Scale Testing)**
```bash
./nlm --preset quick
```

**Medium Preset (Balanced Testing)**
```bash
./nlm --preset medium
```

**High Fidelity Preset (Research Grade)**
```bash
./nlm --preset high
```

**Custom Preset (User-Defined)**
```bash
./nlm --preset custom
```

### 1.3 Command-Line Parameters

**Basic Configuration:**
```bash
./nlm neuron_count=500 region_count=2 connection_probability=0.2
```

**Advanced Configuration:**
```bash
./nlm neuron_count=5000 simulation_timestep=0.0005 \
      stdp_ltp_weight=0.02 stdp_ltd_weight=0.015 \
      synaptogenesis_rate=0.001 pruning_rate=0.0001
```

## Configuration Management Examples

### 2.1 Configuration File Usage

**Using Configuration File:**
```bash
# Load configuration from file
./nlm --config myconfig.json
```

**Configuration File Template (myconfig.json):**
```json
{
    "name": "My Neural Network",
    "description": "Custom neural network configuration",
    "parameters": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "synaptogenesis_rate": 0.0002,
        "pruning_rate": 0.00002
    },
    "quality_score": 90,
    "performance_profile": "high"
}
```

### 2.2 Configuration Export/Import

**Export Current Configuration:**
```bash
./nlm --export-config myconfig.json
```

**Import Configuration:**
```bash
./nlm --import-config myconfig.json
```

**Export Configuration as YAML:**
```bash
./nlm --export-config myconfig.yaml
```

**Export Configuration as CSV:**
```bash
./nlm --export-config myconfig.csv
```

### 2.3 Preset Configuration Management

**List Available Presets:**
```bash
./nlm --list-presets
```

**Create Custom Preset:**
```bash
./nlm --create-preset mypreset "My custom preset" \
      --neuron-count 3000 --region-count 4 --connection-probability 0.2
```

**Use Custom Preset:**
```bash
./nlm --preset mypreset
```

## Performance and Monitoring Examples

### 3.1 Basic Progress Monitoring

**Progress Reporting:**
```bash
./nlm --progress-interval 10 --verbose
```

**Progress Every Step:**
```bash
./nlm --progress-interval 0 --verbose
```

**Minimal Progress:**
```bash
./nlm --progress-interval 100
```

### 3.2 Performance Monitoring

**Memory and Performance Monitoring:**
```bash
./nlm --memory-monitor --performance-monitor
```

**Detailed Timing:**
```bash
./nlm --enable-timing
```

**Performance Benchmark:**
```bash
./nlm --benchmark --benchmark-samples 5
```

### 3.3 Statistics Export

**Export Statistics (JSON):**
```bash
./nlm --stats simulation_results.json
```

**Export Statistics (CSV):**
```bash
./nlm --stats simulation_results.csv --output-format csv
```

**Export Statistics (YAML):**
```bash
./nlm --stats simulation_results.yaml --output-format yaml
```

**Export Statistics (Console):**
```bash
./nlm --stats simulation_results.txt
```

### 3.4 Output Format Examples

**Console Output:**
```bash
./nlm
```

**JSON Output:**
```bash
./nlm --output-format json
```

**YAML Output:**
```bash
./nlm --output-format yaml
```

**CSV Output:**
```bash
./nlm --output-format csv
```

## Session Management Examples

### 4.1 Session Creation and Replay

**Save Session:**
```bash
./nlm --save-session my_session.bin
```

**Replay Session:**
```bash
./nlm --session my_session.bin
```

**List Sessions:**
```bash
./nlm --list-sessions
```

**Delete Session:**
```bash
./nlm --delete-session my_session.bin
```

### 4.2 Advanced Session Management

**Resume Session:**
```bash
./nlm --session my_session.bin --resume
```

**Session Analysis:**
```bash
./nlm --session my_session.bin --analyze
```

**Session Compare:**
```bash
./nlm --session session1.bin --compare session2.bin
```

## Quality and Validation Examples

### 5.1 Configuration Validation

**Validate Configuration:**
```bash
./nlm --validate-only
```

**Check Configuration Quality:**
```bash
./nlm --check-quality
```

**Validate Specific Components:**
```bash
./nlm --validate-plasticity
./nlm --validate-memory
./nlm --validate-neuron
```

### 5.2 Quality Assessment Examples

**High-Quality Configuration:**
```bash
./nlm --check-quality --min-score 90
```

**Low-Quality Configuration Warning:**
```bash
./nlm --check-quality --warn-low-quality
```

**Configuration Optimization:**
```bash
./nlm --optimize-configuration
```

### 5.3 Error Handling Examples

**Graceful Error Handling:**
```bash
./nlm --validate-only --continue-on-error
```

**Detailed Error Reporting:**
```bash
./nlm --verbose --check-quality
```

## Integration Examples

### 6.1 Learning and Development Integration

**Enable Learning and Development:**
```bash
./nlm --enable-learning --enable-development
```

**Enable All Advanced Features:**
```bash
./nlm --enable-learning --enable-development \
      --enable-novelty-detection --enable-curiosity
```

**Learning Configuration:**
```bash
./nlm --preset high --enable-learning --enable-development \
      --enable-novelty-detection --enable-curiosity
```

### 6.2 Novelty and Curiosity Integration

**Enable Novelty Detection:**
```bash
./nlm --enable-novelty-detection
```

**Enable Curiosity-Driven Exploration:**
```bash
./nlm --enable-curiosity
```

**Combined Novelty and Curiosity:**
```bash
./nlm --enable-novelty-detection --enable-curiosity
```

### 6.3 Educational and Research Examples

**Educational Simulation:**
```bash
./nlm neuron_count=200 region_count=2 connection_probability=0.1 \
      --enable-novelty-detection --enable-curiosity
```

**Research Simulation:**
```bash
./nlm --preset high --enable-learning --enable-development \
      --enable-novelty-detection --enable-curiosity \
      --performance-monitor --memory-monitor
```

**Industrial Simulation:**
```bash
./nlm --preset medium --enable-learning --performance-monitor \
      --output-format json --stats industrial_results.json
```

## Advanced Examples

### 7.1 Complex Configuration

**Advanced Configuration:**
```bash
./nlm \
  neuron_count=8000 \
  region_count=6 \
  connection_probability=0.18 \
  simulation_timestep=0.0005 \
  stdp_ltp_weight=0.025 \
  stdp_ltd_weight=0.02 \
  synaptogenesis_rate=0.0003 \
  pruning_rate=0.00003 \
  --enable-learning \
  --enable-development \
  --enable-novelty-detection \
  --enable-curiosity \
  --performance-monitor \
  --memory-monitor \
  --stats advanced_results.json \
  --output-format json \
  --verbose
```

### 7.2 Batch Processing

**Run Multiple Configurations:**
```bash
#!/bin/bash

# Run multiple configurations
for config in quick medium high; do
    echo "Running preset: $config"
    ./nlm --preset $config --stats ${config}_results.json
    echo "Completed preset: $config"
done
```

**Parameter Sweep:**
```bash
#!/bin/bash

# Sweep connection probability
for prob in 0.1 0.15 0.2 0.25; do
    echo "Running with connection probability: $prob"
    ./nlm connection_probability=$prob --stats prob_${prob//./}_results.json
    echo "Completed probability: $prob"
done
```

### 7.3 Monitoring and Analysis

**Continuous Monitoring:**
```bash
./nlm --enable-monitoring --monitor-interval 5 --stats monitor_results.json
```

**Performance Profiling:**
```bash
./nlm --profile --profile-output profile.txt --benchmark
```

**Memory Analysis:**
```bash
./nlm --memory-analysis --memory-report mem_analysis.html
```

## Configuration File Templates

### 9.1 Basic Template

**basic.json:**
```json
{
    "name": "Basic Neural Network",
    "description": "Simple neural network simulation",
    "parameters": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001
    },
    "quality_score": 80,
    "performance_profile": "medium"
}
```

### 9.2 Advanced Template

**advanced.json:**
```json
{
    "name": "Advanced Neural Network",
    "description": "Complex neural network with learning and development",
    "parameters": {
        "neuron_count": 5000,
        "region_count": 4,
        "connection_probability": 0.15,
        "simulation_timestep": 0.0005,
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.012,
        "synaptogenesis_rate": 0.0002,
        "pruning_rate": 0.00002
    },
    "features": {
        "enable_learning": true,
        "enable_development": true,
        "enable_novelty_detection": true,
        "enable_curiosity": true
    },
    "quality_score": 95,
    "performance_profile": "high"
}
```

### 9.3 CSV Template

**parameters.csv:**
```csv
parameter,value,unit,description
simulation_timestep,0.001,ms,Simulation time step
neuron_count,1000,neurons,Number of neurons
region_count,1,regions,Number of regions
connection_probability,0.1,none,Connection probability
stdp_ltp_weight,0.01,none,STDP LTP weight
stdp_ltd_weight,0.012,none,STDP LTD weight
synaptogenesis_rate,0.0001,1/s,Synaptogenesis rate
pruning_rate,0.00001,1/s,Pruning rate
```

## Command Summary

### Quick Reference

| Command | Description | Example |
|---------|-------------|---------|
| `--help` | Show help information | `./nlm --help` |
| `--version` | Show version | `./nlm --version` |
| `--preset NAME` | Load preset | `./nlm --preset quick` |
| `--config FILE` | Load config file | `./nlm --config myconfig.json` |
| `--verbose` | Verbose output | `./nlm --verbose` |
| `--stats FILE` | Export statistics | `./nlm --stats results.json` |
| `--export-config FILE` | Export config | `./nlm --export-config myconfig.json` |
| `--import-config FILE` | Import config | `./nlm --import-config myconfig.json` |
| `--preset NAME` | Load preset | `./nlm --preset custom` |

### Common Commands

1. **Basic Usage:**
   ```bash
   ./nlm
   ```

2. **Quick Test:**
   ```bash
   ./nlm --preset quick
   ```

3. **High Fidelity:**
   ```bash
   ./nlm --preset high
   ```

4. **Progress Monitoring:**
   ```bash
   ./nlm --verbose --progress-interval 10
   ```

5. **Performance Monitoring:**
   ```bash
   ./nlm --memory-monitor --performance-monitor
   ```

6. **Statistics Export:**
   ```bash
   ./nlm --stats results.json --output-format json
   ```

7. **Configuration Management:**
   ```bash
   ./nlm --export-config myconfig.json
   ./nlm --import-config myconfig.json
   ```

## Conclusion

The enhanced command-line interface of NLM provides comprehensive capabilities for neural network simulation with advanced configuration management, performance monitoring, and output formatting. The examples provided demonstrate the wide range of use cases from simple testing to complex research simulations.

Key features include:

✅ **Comprehensive Command-Line Options**: Presets, configuration management, monitoring
✅ **Multiple Output Formats**: Console, JSON, YAML, CSV
✅ **Performance Monitoring**: Memory, timing, and performance tracking
✅ **Configuration Management**: Import, export, validation, and quality assessment
✅ **Session Management**: Save and replay simulation sessions
✅ **Advanced Features**: Learning, development, novelty, and curiosity
✅ **Integration Support**: Educational, research, and industrial use cases

The NLM enhanced CLI provides a powerful and flexible interface for neural network simulation across all use cases and skill levels.

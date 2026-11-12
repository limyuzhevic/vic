# NLM CLI Tool
# Enhanced command-line interface for NLM neural simulation

This tool provides comprehensive control over the NLM neural simulation framework with:

## Available Commands

`nlm-cli` [options]                           Main command for running simulations
`nlm-benchmark` [options]                   Run performance benchmarks
`nlm-config` [options]                       Configuration management
`nlm-demo` [examples]                       Run pre-built demos
`nlm-profiler` [options]                     Performance profiling
`nlm-stats` [brain]                         Generate brain statistics
`nlm-script` [script]                       Run automation scripts
`nlm-visualize` [options]                   Visual brain visualization

## Configuration Management

Configuration can be managed using config files or command-line options:

### Config file format (.nlmconfig)
```ini
# NLM Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = INFO
```

### Command-line options
```bash
--config <file>        Load configuration from file
--neuron-count N       Set neuron count
--connection-prob P    Set connection probability
--steps N              Number of simulation steps
--seed N               Set random seed
--verbose              Enable verbose logging
--output <file>        Output results to file
--validate-only        Only validate configuration, don't run
```

## Demo Examples

### Simple brain simulation
```bash
nlm-demo simple --neurons 500 --steps 1000
```

### Learning environment demo
```bash
nlm-demo learning --world-size 20 --reward 1.0 --curiosity 0.5
```

### Phase 6 integration demo
```bash
nlm-demo phase6 --neurons 1000 --max-steps 2000
```

## Performance Benchmarks

Run comprehensive performance benchmarks:

### Network size benchmarks
```bash
nlm-benchmark --sizes 100 500 1000 2000 --steps 1000
```

### Configuration optimization
```bash
nlm-benchmark --optimize --parameters "connection-probability,neuron-count,plasticity-rate"
```

### Memory usage profiling
```bash
nlm-benchmark --memory --monitor --interval 100
```

## Advanced Configuration

### Configuration files
```bash
# Save current config
nlm-config save my_config.nlmconfig

# Load configuration
nlm-config load my_config.nlmconfig

# List all available configurations
nlm-config list

# Validate configuration
nlm-config validate my_config.nlmconfig
```

### Configuration validation
```bash
# Check for deprecated settings
nlm-config check --deprecated

# Validate schema
nlm-config validate --strict

# Export configuration
nlm-config export --format json my_config.json
```

## Automation and Scripting

### Script execution
```bash
# Run automation script
nlm-script run experiment_script.nmscript

# Create new script
nlm-script create my_experiment

# List available scripts
nlm-script list
```

### Script format
Scripts define sequences of NLM operations for batch processing:

```ini
[experiment]
name = "My Experiment"
world_size = 15
epochs = 10
steps_per_episode = 200

[learning]
enable_reward_modulation = true
enable_curiosity = true
enable_development = true

[action]
agent_type = "neural"
control_scheme = "competitive"

[output]
results_file = "experiment_results.json"
export_brain = true
```

## Performance Monitoring

### Profiler
```bash
# Profile simulation performance
nlm-profiler run --brain --output profiler_results.html

# Profile specific subsystem
nlm-profiler subsystem memory --duration 30

# Memory profiling
nlm-profiler memory --track-allocation --report fragmentation
```

### Statistics collection
```bash
# Collect real-time metrics
nlm-stats live --update-interval 0.5 --output live_stats.json

# Generate historical analysis
nlm-stats analyze --time-range "last 30 days" --output analysis.html

# Compare brain performance
nlm-stats compare brain1.json brain2.json
```

## Visualization

### Brain visualization
```bash
# Generate visualization
nlm-visualize brain --format html --output brain_visualization.html

# Create heatmap visualization
nlm-visualize heatmap --neuron-count 500 --output heatmap.png

# Activity patterns over time
nlm-visualize activity --duration 10 --output activity_patterns.html
```

## Python API

```python
import pynlm
import nlm_cli

# Use NLM CLI features from Python
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Advanced configuration
config = nlm_cli.load_config("my_config.nlmconfig")
brain = pynlm.createBrain(config)

# Run CLI commands programmatically
cmd = nlm_cli.CommandBuilder()
cmd.brain_simulation()
    .neurons(1000)
    .steps(1000)
    .execute()

# Performance profiling
profiler = nlm_cli.Profiler(brain)
profiler.run_profile()
    .analyze_memory_usage()
    .generate_report("profile_results.html")
```

## Integration Examples

### C++ Integration
```cpp
#include <nlm/NLMCLI.hpp>
#include <nlm/Core.hpp>

int main() {
    nlm::CLI::ConfigManager config;
    config.loadFromFile("config.json");
    
    nlm::Brain brain(config);
    nlm::CLI::SimulationRunner runner(brain);
    
    runner.run(1000, 0.001)
        .withStats()
        .withVisualization()
        .saveResults("experiment.json");
    
    return 0;
}
```

### Jupyter Notebook Integration
```python
# NLM CLI in Jupyter
from IPython.display import HTML
import pynlm
import matplotlib.pyplot as plt

# Create and run brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with monitoring
metrics = brain.run_simulation(500)

# Visualize results
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
ax1.plot(metrics['firing_rate'])
ax2.plot(metrics['weight_changes'])
plt.show()

# Generate interactive visualization
html = nlm_cli.create_visualization(brain, metrics)
HTML(html)
```

## Configuration Templates

### Template: Research Experiment
```ini
[random]
seed = 12345
reproducible = true

[network]
neuron_count = 2000
region_count = 3
connection_probability = 0.05

[plasticity]
stype = "stdp_and_hebbian"
learning_rate = 0.001
structural_plasticity = true

[neuromodulation]
dopamine = true
curiosity = true
novelty = true

[memory]
working_memory = true
episodic_memory = true
associative_memory = true

[development]
enabled = true
stage_dependent = true

[world]
environment = "simple_grid"
size_x = 20
size_y = 20
reward_function = "sparse"

[experiment]
duration_steps = 5000
record_interval = 10
output_format = "json"
```

### Template: Performance Test
```ini
[performance]
mode = "benchmark"
test_types = ["small_network", "medium_network", "large_network"]

[small_network]
neurons = 100
steps = 1000

[medium_network]
neurons = 500
steps = 500

[large_network]
neurons = 1000
steps = 200

[measurement]
track_cpu = true
track_memory = true
track_spike_rate = true
collect_timing = true

[output]
results_file = "benchmark_results.json"
comparison_baseline = "baseline_config"
```

## Quick Start Examples

### 1. Quick Simulation
```bash
nlm-demo simple --neurons 500 --steps 1000
```

### 2. Learning Environment
```bash
nlm-demo learning --world-size 20 --enable-curiosity --enable-reward
```

### 3. Performance Benchmark
```bash
nlm-benchmark --sizes 100 500 1000 --steps 1000
```

### 4. Configuration Management
```bash
nlm-config save my_research_config
nlm-config validate my_research_config
nlm-demo phase6 --config my_research_config
```

### 5. Automation Script
```bash
nlm-script run batch_experiment.nmscript
```

## Troubleshooting

### Common Issues

**Configuration not loading**
- Check file path and permissions
- Use absolute paths for configuration files
- Validate JSON/YAML syntax

**Memory usage too high**
- Reduce neuron count or connection density
- Use performance mode
- Monitor with `nlm-profiler memory`

**Simulation too slow**
- Use optimization mode
- Reduce logging verbosity
- Run on larger network sizes

**Visualization issues**
- Ensure headless mode for command line
- Use `--format png` instead of `--format html` for console
- Install required display dependencies

### Getting Help

```bash
# Get help for specific commands
nlm-cli --help
nlm-demo --help
nlm-benchmark --help

# Get detailed documentation
nlm-cli docs
nlm-demo examples
nlm-benchmark templates

# Get version information
nlm-cli version
nlm-demo version
nlm-benchmark version
```

## License

MIT License - See LICENSE file for details

Copyright (c) 2026 NLM Authors

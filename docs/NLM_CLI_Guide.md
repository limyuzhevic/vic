# NLM Phase 6 - Command Line Interface Guide
# Improved User Experience and Advanced Features

## Overview

The NLM (熙然) Phase 6 Final Integration provides a comprehensive command-line interface with multiple modes to accommodate users of all skill levels. The system has been significantly improved with better structure, error handling, and advanced features for expert users.

## Modes of Operation

### 1. Easy Mode (`--mode easy`)

**Purpose**: Simple, guided usage for beginners and rapid prototyping

**Features**:
- Sensible defaults for all configuration parameters
- Step-by-step guidance and clear output
- Automatic configuration loading
- Pre-built demonstrations that run immediately

**Typical Use Case**:
```bash
# Run simple demo with recommended settings
./nlm --mode easy

# Run demo with custom neuron count
./nlm --mode easy --neurons 500
```

### 2. Demo Mode (`--mode demo`)

**Purpose**: Pre-built demonstrations of specific brain system integrations

**Available Demos**:
- `verification`: Quick integration verification
- `memory`: Memory system integration test
- `neuromodulation`: Neuromodulation integration test
- `checkpoint`: Checkpoint system test
- `replay`: Replay system test
- `full`: Complete integration test (default)

**Features**:
- Multiple demo types for focused testing
- Detailed performance metrics
- Progress tracking during long-running demos
- Support for different output formats (CSV, JSON, detailed text)

**Typical Use Case**:
```bash
# Run specific integration test
./nlm --mode demo --demo memory

# Run full integration test with data export
./nlm --mode demo --demo full --enable-csv --enable-json
```

### 3. Expert Mode (`--mode expert`)

**Purpose**: Advanced control with full features for researchers and developers

**Features**:
- Full control over all parameters
- Script execution for automation
- Advanced data export options
- Custom configuration loading
- Progress tracking and detailed reporting

**Typical Use Case**:
```bash
# Run advanced integration test with custom parameters
./nlm --mode expert --neurons 2000 --steps 20000 --enable-csv --enable-json

# Run with script for batch processing
./nlm --mode expert --script experiments/batch_script.txt
```

### 4. Custom Mode (`--mode custom`)

**Purpose**: Load custom configuration files for specialized use cases

**Features**:
- Load configuration from files
- Override configuration with command line arguments
- Flexible parameter customization
- Integration with existing configuration systems

**Typical Use Case**:
```bash
# Load custom configuration
./nlm --mode custom --config my_config.cfg

# Override configuration with command line
./nlm --mode custom --config base.cfg --neurons 1500 --timestep 0.01
```

## Configuration Options

### Basic Configuration
- `--neurons COUNT`: Number of neurons (default: 1000)
- `--steps COUNT`: Maximum simulation steps (default: 10000)
- `--timestep VALUE`: Simulation timestep (default: 0.001)
- `--seed VALUE`: Random seed for reproducible results (default: 42)

### Advanced Configuration
- `--enable-csv`: Enable CSV output for data analysis
- `--enable-json`: Enable JSON output for programmatic access
- `--no-progress`: Disable progress output
- `--progress-interval VALUE`: Progress update interval (default: 1000)
- `--generate-report FORMAT`: Generate experiment report (format: brief, summary, detailed)

### Configuration Files
- `--config FILE`: Configuration file (default: configs/default.cfg)
- `--expert-config FILE`: Expert mode configuration file
- `--script FILE`: Expert mode script file for automation

### System Options
- `--log LEVEL`: Log level (trace, debug, info, warn, error, critical)
- `--help`: Display help information
- `--version`: Display version information
- `--list-configs`: List available configuration files

## Scripting and Automation

### Expert Mode Scripting
Expert mode supports script files for batch processing and automation. Scripts can contain commands to:

**Set Parameters**:
```
set neurons=2000
set steps=5000
set timestep=0.005
set seed=123
```

**Enable/Disable Features**:
```
enable checkpointing
disable replay
enable development
```

**Set Logging Level**:
```
log info
log debug
```

**Script Example**:
```bash
# Create script file: my_experiment.txt
set neurons=1500
set steps=10000
set timestep=0.001
set seed=42

enable checkpointing
enable replay
enable development

enable csv
enable json

log info
```

### Usage:
```bash
./nlm --mode expert --script my_experiment.txt
```

## Output Formats

### Detailed Text Output (Default)
Provides human-readable results with integration status and performance metrics.

### CSV Output (`--enable-csv`)
Machine-readable format for data analysis:
```csv
metric,value
memory_working_memory_integrated,true
memory_episodic_memory_integrated,true
neuromodulation_integrated,true
prediction_integrated,true
development_integrated,true
checkpointing_works,true
replay_works,true
total_reward,0.85
avg_firing_rate,8.2
avg_synaptic_weight,0.45
memory_episodes_stored,12
novelty_level,0.3
```

### JSON Output (`--enable-json`)
Programmatically accessible format:
```json
{
  "system_integration": {
    "memory_working_memory_integrated": true,
    "memory_episodic_memory_integrated": true,
    "neuromodulation_integrated": true,
    "prediction_integrated": true,
    "development_integrated": true,
    "checkpointing_works": true,
    "replay_works": true
  },
  "performance_metrics": {
    "total_reward": 0.85,
    "avg_firing_rate": 8.2,
    "avg_synaptic_weight": 0.45,
    "memory_episodes_stored": 12,
    "novelty_level": 0.3
  }
}
```

## Error Handling and User Experience

### Improved Error Messages
The system now provides clear, actionable error messages:

**Configuration Errors**:
```
ERROR: Configuration file not found: configs/my_config.cfg
WARNING: Using default configuration.
```

**Initialization Errors**:
```
ERROR: Failed to initialize brain!
Please check your configuration and system resources.
```

**Unexpected Errors**:
```
============================================================
ERROR: Unexpected exception occurred
Details: division by zero in neural computation
============================================================
Please check your system configuration and try again.
For help, use --help option.
```

### Progress Tracking
During long-running experiments, progress is displayed at regular intervals:
```
Step 1000 | Reward: 0.15 | Firing: 8.5 Hz | WorkingMem: 2
Step 2000 | Reward: 0.28 | Firing: 9.2 Hz | WorkingMem: 5
Step 3000 | Reward: 0.41 | Firing: 8.8 Hz | WorkingMem: 7
```

## Examples

### Quick Start (Easy Mode)
```bash
./nlm --mode easy
```

### Focused Integration Test
```bash
./nlm --mode demo --demo memory --enable-csv
```

### Advanced Research Experiment
```bash
./nlm --mode expert \
  --neurons 2000 \
  --steps 20000 \
  --timestep 0.001 \
  --enable-csv \
  --enable-json \
  --generate-report detailed \
  --script research_protocol.txt
```

### Custom Configuration
```bash
./nlm --mode custom \
  --config base_config.cfg \
  --expert-config my_overrides.cfg \
  --neurons 3000 \
  --steps 15000
```

### Batch Processing with Scripts
```bash
# Create experiment scripts
for i in {1..5}; do
    ./nlm --mode expert --script experiment_$i.txt >> results_$i.txt
    ./nlm --mode expert --script experiment_$i.txt --enable-csv >> results_$i.csv
    ./nlm --mode expert --script experiment_$i.txt --enable-json >> results_$i.json
done

# Combine results
./nlm --mode expert --script combine_results.txt >> final_report.txt
```

## Troubleshooting

### Common Issues

**1. Configuration File Not Found**
```bash
# Check if file exists
ls -la configs/default.cfg

# Create if needed
mkdir -p configs
cp default.cfg configs/my_config.cfg

# Or use relative path
./nlm --mode custom --config ./my_config.cfg
```

**2. Memory Allocation Issues**
```bash
# Reduce neuron count for systems with limited memory
./nlm --neurons 500 --steps 5000

# Use test mode for quick verification
./nlm --mode demo --demo verification
```

**3. Script Execution Issues**
```bash
# Check script syntax
./nlm --mode expert --script my_script.txt

# Run script in debug mode
./nlm --mode expert --log debug --script my_script.txt
```

### Getting Help
```bash
# Display comprehensive help
./nlm --help

# Display version information
./nlm --version

# List available configuration files
./nlm --list-configs
```

## Advanced Features

### Experiment Tracking
- **Checkpointing**: Save and load brain states
- **Replay**: Replay recorded experiences
- **Development**: Simulate developmental stages
- **Metrics**: Track performance over time

### Data Analysis
- **CSV Export**: Import into analysis tools
- **JSON Export**: Use with web applications
- **Report Generation**: Create detailed analysis reports

### Automation
- **Script Files**: Batch process experiments
- **Command Line Combinations**: Complex parameter sets
- **Pipeline Integration**: Integrate with CI/CD systems

## Migration Guide

### From Previous Versions

**Version 1.x to 2.x (Phase 6)**
- Command-line interface completely rewritten
- New mode-based architecture
- Advanced scripting support added
- Improved error handling

**Migration Steps**:
1. Review command-line interface options
2. Update any custom scripts to use new syntax
3. Adapt configuration files to new format
4. Leverage new expert mode features

**Example Migration**:
```bash
# Old command (Phase 2)
./nlm_phase2_demo

# New command (Phase 6 Easy Mode)
./nlm --mode easy

# Old command (Phase 2 with custom parameters)
./nlm --config custom.cfg

# New command (Phase 6 Expert Mode)
./nlm --mode expert --config custom.cfg --neurons 2000
```

## Conclusion

The NLM Phase 6 command-line interface provides a significant improvement over previous versions, offering:

1. **User-Friendly Access**: Multiple modes for different skill levels
2. **Powerful Features**: Advanced scripting and data export
3. **Robust Error Handling**: Clear, actionable error messages
4. **Flexible Configuration**: Multiple ways to specify parameters
5. **Research Support**: Tools for experimentation and analysis

The system is now better suited for both beginners exploring the neural learning machine and advanced researchers conducting complex experiments.

# NLM Command-Line Interface

## Overview
This document describes the comprehensive command-line interface for NLM, providing users with powerful tools for neural network simulation, analysis, and management.

## Command Categories

### 1. Core Simulation Commands
- `nlm run` - Run neural simulation
- `nlm step` - Execute single simulation steps
- `nlm status` - Display brain status
- `nlm analyze` - Perform neural analysis

### 2. Model Management Commands
- `nlm save` - Save model state
- `nlm load` - Load model state
- `nlm export` - Export models to various formats
- `nlm import` - Import models from various formats

### 3. Configuration Commands
- `nlm config` - Configuration management
- `nlm validate` - Validate configuration
- `nlm profile` - Performance profiling

### 4. Analysis Commands
- `nlm benchmark` - Performance benchmarking
- `nlm statistics` - Generate statistics
- `nlm visualize` - Generate visualizations
- `nlm compare` - Compare models

### 5. Development Commands
- `nlm debug` - Debug mode
- `nlm trace` - Execution tracing
- `nlm profile` - Performance profiling

## Detailed Command Reference

### 1. Core Simulation Commands

#### `nlm run`
Run a neural simulation with specified configuration.

**Usage:**
```bash
nlm run [OPTIONS] CONFIG_FILE
```

**Options:**
- `--steps N` - Number of simulation steps (default: 1000)
- `--timestep T` - Simulation timestep (default: 0.001)
- `--world-width W` - World width (default: 20)
- `--world-height H` - World height (default: 20)
- `--vision-width V` - Vision width (default: 8)
- `--vision-height V` - Vision height (default: 8)
- `--enable-learning` - Enable learning mechanisms
- `--enable-curiosity` - Enable curiosity-driven exploration
- `--enable-development` - Enable developmental processes
- `--output FILE` - Output file for results
- `--format FORMAT` - Output format (json, csv, txt)

**Example:**
```bash
nlm run config.json --steps 5000 --enable-learning --output results.json
```

#### `nlm step`
Execute a single simulation step.

**Usage:**
```bash
nlm step [OPTIONS]
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--timestep T` - Simulation timestep (default: 0.001)
- `--verbose` - Enable verbose output
- `--log-level LEVEL` - Log level (debug, info, warn, error)

**Example:**
```bash
nlm step --timestep 0.01 --verbose
```

#### `nlm status`
Display current brain status and statistics.

**Usage:**
```bash
nlm status [OPTIONS]
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--format FORMAT` - Output format (json, human, machine)
- `--detailed` - Show detailed information
- `--stats` - Show statistics only

**Example:**
```bash
nlm status --format json --detailed
```

#### `nlm analyze`
Perform comprehensive neural analysis.

**Usage:**
```bash
nlm analyze [OPTIONS] [ANALYSIS_TYPE]
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--analysis-type TYPE` - Analysis type (connectivity, learning, activity, plasticity)
- `--output FILE` - Output file for analysis results
- `--format FORMAT` - Output format (json, csv, yaml)
- `--threshold VALUE` - Analysis threshold
- `--window-size N` - Analysis window size

**Example:**
```bash
nlm analyze learning --output learning_results.yaml --window-size 100
```

### 2. Model Management Commands

#### `nlm save`
Save brain model state to file.

**Usage:**
```bash
nlm save [OPTIONS] FILE
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--format FORMAT` - Save format (hdf5, json, binary)
- `--compress` - Enable compression
- `--include-config` - Include configuration
- `--include-regions` - Include regional data
- `--include-synapses` - Include synaptic data

**Example:**
```bash
nlm save model.nld --format hdf5 --compress --include-synapses
```

#### `nlm load`
Load brain model from file.

**Usage:**
```bash
nlm load [OPTIONS] FILE
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--format FORMAT` - Load format (hdf5, json, binary)
- `--verify` - Verify integrity after loading
- `--overwrite` - Overwrite existing brain
- `--init-config` - Initialize with configuration from file

**Example:**
```bash
nlm load model.nld --format hdf5 --verify --init-config
```

#### `nlm export`
Export model to various formats.

**Usage:**
```bash
nlm export [OPTIONS] SOURCE DEST
```

**Options:**
- `--format FORMAT` - Export format (hdf5, json, csv, txt)
- `--include-neurons` - Include neuron data
- `--include-synapses` - Include synapse data
- `--include-regions` - Include region data
- `--compression LEVEL` - Compression level (0-9)

**Supported Formats:**
- `hdf5` - Hierarchical Data Format
- `json` - JavaScript Object Notation
- `csv` - Comma-Separated Values
- `txt` - Plain text
- `nld` - NLM native format

**Example:**
```bash
nlm export brain.json model.csv --format csv --include-synapses
```

#### `nlm import`
Import model from various formats.

**Usage:**
```bash
nlm import [OPTIONS] SOURCE [OPTIONS]
```

**Options:**
- `--format FORMAT` - Import format (hdf5, json, csv, txt)
- `--into-brain-id ID` - Target brain ID (default: 0)
- `--merge` - Merge with existing brain
- `--validate` - Validate imported data

**Example:**
```bash
nlm import model.csv --format csv --into-brain-id 0 --validate
```

### 3. Configuration Commands

#### `nlm config`
Configuration management.

**Usage:**
```bash
nlm config [OPTIONS] [KEY] [VALUE]
```

**Options:**
- `--file FILE` - Configuration file
- `--list` - List all configuration keys
- `--get KEY` - Get configuration value
- `--set KEY VALUE` - Set configuration value
- `--remove KEY` - Remove configuration key
- `--validate` - Validate configuration
- `--copy DEST` - Copy configuration to destination
- `--merge SOURCE` - Merge configuration from source

**Examples:**
```bash
nlm config --list
nlm config --get neuron_count
nlm config --set neuron_count 1000
nlm config --validate
```

#### `nlm validate`
Validate configuration and data.

**Usage:**
```bash
nlm validate [OPTIONS]
```

**Options:**
- `--config FILE` - Configuration file to validate
- `--brain-id ID` - Brain ID (default: 0)
- `--format FORMAT` - Validation format (json, detailed, summary)
- `--fix` - Automatically fix validation issues
- `--report FILE` - Save validation report to file

**Example:**
```bash
nlm validate --config config.json --format detailed --report validation_report.json
```

#### `nlm profile`
Performance profiling.

**Usage:**
```bash
nlm profile [OPTIONS]
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--steps N` - Number of steps to profile (default: 100)
- `--timestep T` - Simulation timestep (default: 0.001)
- `--output FILE` - Output file for profiling results
- `--format FORMAT` - Output format (json, csv, human)
- `--include-memory` - Include memory usage profiling
- `--include-cpu` - Include CPU usage profiling
- `--include-io` - Include I/O profiling

**Example:**
```bash
nlm profile --steps 500 --include-memory --output profile_results.json
```

### 4. Analysis Commands

#### `nlm benchmark`
Performance benchmarking.

**Usage:**
```bash
nlm benchmark [OPTIONS]
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--steps N` - Number of steps (default: 1000)
- `--timestep T` - Simulation timestep (default: 0.001)
- `--output FILE` - Output file for benchmark results
- `--format FORMAT` - Output format (json, csv, yaml)
- `--repetitions N` - Number of repetitions (default: 3)
- `--warmup N` - Warmup steps (default: 10)
- `--metric METRIC` - Metric to benchmark (steps_per_second, memory_usage, cpu_usage)

**Example:**
```bash
nlm benchmark --steps 2000 --repetitions 5 --output benchmark.json
```

#### `nlm statistics`
Generate comprehensive statistics.

**Usage:**
```bash
nlm statistics [OPTIONS]
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--output FILE` - Output file for statistics
- `--format FORMAT` - Output format (json, csv, yaml)
- `--include-neurons` - Include neuron statistics
- `--include-synapses` - Include synapse statistics
- `--include-regions` - Include region statistics
- `--include-learning` - Include learning statistics
- `--time-range START END` - Time range for statistics

**Example:**
```bash
nlm statistics --output brain_stats.yaml --include-learning --time-range 0 1000
```

#### `nlm visualize`
Generate visualizations.

**Usage:**
```bash
nlm visualize [OPTIONS] [VISUAL_TYPE]
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--visual-type TYPE` - Visualization type (network, activity, learning, connectivity)
- `--output FILE` - Output file for visualization
- `--format FORMAT` - Output format (png, pdf, svg, html)
- `--width W` - Image width (default: 800)
- `--height H` - Image height (default: 600)
- `--title TITLE` - Plot title
- `--xlabel LABEL` - X-axis label
- `--ylabel LABEL` - Y-axis label
- `--show-interactions` - Show interactions between components

**Visual Types:**
- `network` - Neural network structure
- `activity` - Neural activity over time
- `learning` - Learning progress
- `connectivity` - Connection patterns
- `development` - Developmental stages

**Example:**
```bash
nlm visualize network --output network_structure.png --format png --width 1200
```

#### `nlm compare`
Compare multiple models.

**Usage:**
```bash
nlm compare [OPTIONS] MODEL1 MODEL2 [MODEL3...]
```

**Options:**
- `--output FILE` - Output file for comparison results
- `--format FORMAT` - Output format (json, csv, yaml)
- `--compare-aspects ASPECTS` - Comma-separated aspects to compare (neurons, synapses, performance, learning)
- `--normalize` - Normalize comparisons
- `--detailed` - Include detailed comparison
- `--threshold VALUE` - Similarity threshold

**Example:**
```bash
nlm compare model1.nld model2.nld --compare-aspects neurons,synapses,performance --output comparison.json
```

### 5. Development Commands

#### `nlm debug`
Debug mode with enhanced output.

**Usage:**
```bash
nlm debug [OPTIONS]
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--level LEVEL` - Debug level (error, warn, info, debug, trace)
- `--output FILE` - Output file for debug logs
- `--format FORMAT` - Log format (plain, json, xml)
- `--trace FILE` - Save execution trace to file
- `--monitor` - Monitor system resources
- `--breakpoints` - List breakpoints

**Example:**
```bash
nlm debug --level debug --output debug.log --trace trace.txt
```

#### `nlm trace`
Execution tracing.

**Usage:**
```bash
nlm trace [OPTIONS]
```

**Options:**
- `--brain-id ID` - Brain ID (default: 0)
- `--output FILE` - Output file for trace
- `--format FORMAT` - Trace format (json, csv, plain)
- `--duration SECONDS` - Trace duration (default: 60)
- `--include-calls` - Include function calls
- `--include-timing` - Include timing information
- `--include-memory` - Include memory usage

**Example:**
```bash
nlm trace --output execution_trace.json --duration 120 --include-timing
```

## Command-Line Interface Architecture

### 5.1 CLI Structure
```
nlm [GLOBAL_OPTIONS] COMMAND [COMMAND_OPTIONS]

GLOBAL_OPTIONS:
  --help                    Show help message
  --version                 Show version information
  --verbose                 Enable verbose output
  --quiet                   Suppress output
  --config FILE             Configuration file
  --log-level LEVEL         Log level
  --log-file FILE           Log file
  --timeout SECONDS        Command timeout

COMMANDS:
  run                      Run neural simulation
  step                     Execute simulation steps
  status                   Display brain status
  analyze                  Perform neural analysis
  save                     Save model state
  load                     Load model state
  export                   Export models
  import                   Import models
  config                   Configuration management
  validate                 Validate configuration
  profile                   Performance profiling
  benchmark                 Performance benchmarking
  statistics               Generate statistics
  visualize                 Generate visualizations
  compare                   Compare models
  debug                     Debug mode
  trace                     Execution tracing
```

### 5.2 Command Processing Flow
1. **Argument Parsing**: Parse command-line arguments using argparse
2. **Configuration Loading**: Load configuration from file or command line
3. **Command Validation**: Validate command arguments and dependencies
4. **Resource Management**: Allocate required resources (brains, worlds, agents)
5. **Command Execution**: Execute the requested command
6. **Result Processing**: Process and format results
7. **Resource Cleanup**: Clean up allocated resources
8. **Error Handling**: Handle errors and provide user feedback

### 5.3 Configuration Management
```python
# Configuration structure
class CLIConfig:
    def __init__(self):
        self.simulation_steps = 1000
        self.simulation_timestep = 0.001
        self.world_width = 20
        self.world_height = 20
        self.vision_width = 8
        self.vision_height = 8
        self.enable_learning = False
        self.enable_curiosity = False
        self.enable_development = False
        self.output_file = None
        self.output_format = "txt"
        self.verbose = False
        self.log_level = "info"
```

### 5.4 Command Implementations
```python
# Base command class
class Command:
    def __init__(self, config):
        self.config = config
        self.logger = logging.getLogger(self.__class__.__name__)
    
    def execute(self, args):
        raise NotImplementedError("Subclasses must implement execute")
    
    def validate_args(self, args):
        pass
    
    def setup_resources(self):
        pass
    
    def cleanup_resources(self):
        pass
```

## Implementation Strategy

### Phase 1: Core Commands (Months 1-2)
1. Implement core simulation commands (run, step, status)
2. Implement basic configuration commands (config, validate)
3. Implement basic analysis commands (analyze, statistics)
4. Implement basic model management commands (save, load)
5. Add global options and help system

### Phase 2: Advanced Commands (Months 3-4)
1. Implement export/import commands
2. Implement visualization commands
3. Implement comparison commands
4. Implement benchmarking commands
5. Add performance profiling commands

### Phase 3: Development Tools (Months 5-6)
1. Implement debug and trace commands
2. Add monitoring and logging commands
3. Implement advanced analysis commands
4. Add configuration management commands
5. Create command-line testing framework

## Command Examples

### Example 1: Running a Simulation
```bash
#!/bin/bash
# run_nlm.sh - Shell script to run NLM simulation

nlm run config.json \
  --steps 5000 \
  --timestep 0.01 \
  --enable-learning \
  --enable-curiosity \
  --output simulation_results.json \
  --format json

# Check results
if [ $? -eq 0 ]; then
    echo "Simulation completed successfully"
    nlm statistics --output simulation_stats.yaml
else
    echo "Simulation failed"
    exit 1
fi
```

### Example 2: Analyzing Learning Process
```bash
#!/bin/bash
# analyze_learning.sh - Shell script to analyze learning

nlm analyze learning \
  --brain-id 0 \
  --window-size 100 \
  --output learning_analysis.json \
  --format json

# Generate visualization
nlm visualize learning \
  --brain-id 0 \
  --output learning_progress.png \
  --width 1200 \
  --height 800

# Compare with baseline
nlm compare model_baseline.nld model_current.nld \
  --compare-aspects neurons,synapses,learning \
  --output comparison_results.json
```

### Example 3: Performance Benchmarking
```bash
#!/bin/bash
# benchmark.sh - Shell script for performance benchmarking

nlm benchmark \
  --brain-id 0 \
  --steps 2000 \
  --repetitions 3 \
  --warmup 50 \
  --output benchmark_results.json \
  --format json

# Profile memory usage
nlm profile \
  --brain-id 0 \
  --steps 500 \
  --include-memory \
  --output memory_profile.json

# Generate performance report
cat > performance_report.sh << 'EOF'
#!/bin/bash
echo "Performance Analysis Report"
echo "=========================="
echo "Benchmark Results:"
cat benchmark_results.json
echo ""
echo "Memory Profile:"
cat memory_profile.json
echo ""
echo "Summary:"
# Calculate and display performance metrics
EOF

chmod +x performance_report.sh
./performance_report.sh
```

## File Structure

### Command Directory Structure
```
nlm/
├── cli/
│   ├── __init__.py
│   ├── commands.py
│   ├── config.py
│   ├── parser.py
│   └── utils.py
├── commands/
│   ├── __init__.py
│   ├── simulation.py
│   ├── model_management.py
│   ├── configuration.py
│   ├── analysis.py
│   └── development.py
├── examples/
│   ├── run_simulation.sh
│   ├── analyze_learning.sh
│   ├── benchmark.sh
│   ├── debug_session.sh
│   └── trace_execution.sh
├── docs/
│   ├── cli_commands.md
│   ├── configuration.md
│   ├── command_examples.md
│   └── best_practices.md
└── tests/
    ├── test_cli_commands.py
    ├── test_command_parsing.py
    ├── test_configuration.py
    ├── test_command_execution.py
    └── test_error_handling.py
```

## Testing Strategy

### Command Testing
- **Unit Testing**: Test individual command implementations
- **Integration Testing**: Test command combinations and workflows
- **End-to-End Testing**: Test complete command sequences
- **Performance Testing**: Test command performance with large datasets

### Command-Line Interface Testing
- **Argument Parsing**: Test argument validation and parsing
- **Help System**: Test help message generation
- **Error Handling**: Test error message generation and handling
- **Configuration Loading**: Test configuration file loading

### User Acceptance Testing
- **Real-world Scenarios**: Test commands with realistic workflows
- **User Interface Testing**: Test command usability and ergonomics
- **Documentation Testing**: Test documentation completeness
- **Performance Acceptance**: Test command performance targets

## Conclusion

The NLM command-line interface provides a comprehensive set of tools for neural network simulation, analysis, and management. The CLI is designed to be:

- **User-friendly**: Intuitive command structure and helpful error messages
- **Powerful**: Extensive functionality for advanced users
- **Flexible**: Support for various input formats and output options
- **Extensible**: Easy to add new commands and features
- **Testable**: Comprehensive testing framework for reliability
- **Documented**: Complete documentation with examples

This command-line interface will enable users to efficiently work with NLM networks, from simple simulations to complex research experiments, providing a powerful interface for both beginners and advanced users.
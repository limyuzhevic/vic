# NLM Command-Line Interface Documentation

## Overview

The NLM Command-Line Interface (CLI) provides comprehensive control over NLM (Neural Learning Machine) simulations, configuration management, testing, and performance benchmarking. It supports a wide range of options for different use cases and workflows.

## CLI Architecture

### Command Structure
```
./nlm [global-options] [command] [command-options]
```

### Command Categories
1. **Simulation Commands**: Run neural network simulations
2. **Configuration Commands**: Manage configuration files
3. **Testing Commands**: Run tests and benchmarks
4. **Experiment Commands**: Run specific experiments
5. **Export/Import Commands**: Export and import data
6. **Utility Commands**: Help, version, and utility functions

## Global Options

### Common Options
| Option | Short | Description |
|--------|-------|-------------|
| `--help` | `-h` | Show help information |
| `--version` | `-v` | Display version information |
| `--debug` | | Enable debug logging |
| `--verbose` | | Enable verbose output |
| `--quiet` | | Suppress non-error output |
| `--log-level` | | Set log level (debug, info, warning, error) |

### Configuration Options
| Option | Description |
|--------|-------------|
| `--config <file>` | Load configuration from file |
| `--save <file>` | Save configuration to file |
| `--load <file>` | Load configuration from file |
| `--validate` | Validate configuration |
| `--profiles` | Show performance profiles |

### Performance Options
| Option | Description |
|--------|-------------|
| `--duration <seconds>` | Run simulation for specified duration |
| `--steps <steps>` | Run simulation for specified number of steps |
| `--neurons <count>` | Set number of neurons |
| `--benchmark <name>` | Run specific benchmark |
| `--scaling-benchmark` | Run scaling benchmarks |

### Output Options
| Option | Description |
|--------|-------------|
| `--format <format>` | Output format (console, json, xml, html) |
| `--export <file>` | Export results to file |
| `--import <file>` | Import results from file |
| `--compare` | Compare with baseline results |

### Testing Options
| Option | Description |
|--------|-------------|
| `--test <category>` | Run tests in specific category |
| `--test-category <category>` | Run tests by category |
| `--test-tag <tag>` | Run tests by tag |
| `--test-pattern <pattern>` | Run tests by name pattern |

### Experiment Options
| Option | Description |
|--------|-------------|
| `--experiment <name>` | Run specific experiment |
| `--experiment-mode <mode>` | Set experiment mode (demo, full, quick) |

### Visualization Options
| Option | Description |
|--------|-------------|
| `--visualize <mode>` | Enable visualization mode (none, basic, advanced, full) |
| `--window-size <width>x<height>` | Set visualization window size |
| `--visualization-delay <delay>` | Set visualization update delay |

## Usage Examples

### Basic Simulation
```bash
# Run a simple simulation with default configuration
./nlm --steps 1000 --neurons 1000

# Run simulation with verbose output
./nlm --steps 1000 --verbose

# Run simulation with debug logging
./nlm --steps 1000 --debug
```

### Configuration Management
```bash
# Load configuration from file
./nlm --config my_config.json --steps 2000

# Save current configuration to file
./nlm --save experiment_config.json --experiment demo

# Validate configuration
./nlm --config my_config.json --validate

# Load configuration and run simulation
./nlm --config experiment_config.json --steps 5000 --verbose
```

### Testing
```bash
# Run all tests
./nlm --test all

# Run tests by category
./nlm --test memory --test neuromodulation

# Run tests by tag
./nlm --test tag:integration --test tag:performance

# Run tests by name pattern
./nlm --test pattern:"test_.*_memory"

# Run tests and save results
./nlm --test all --export test_results.json

# Run tests in JSON format
./nlm --test all --format json --export test_results.json
```

### Performance Benchmarking
```bash
# Run micro benchmark
./nlm --benchmark micro --neurons 1000 --steps 1000

# Run small benchmark
./nlm --benchmark small --neurons 10000 --steps 10000

# Run medium benchmark
./nlm --benchmark medium --neurons 100000 --steps 100000

# Run large benchmark
./nlm --benchmark large --neurons 1000000 --steps 100000

# Run scaling benchmark
./nlm --scaling-benchmark

# Export benchmark results
./nlm --benchmark neuron_performance --export benchmark_results.json
```

### Experiment Running
```bash
# Run Phase 6 integration demo
./nlm --experiment phase6_demo

# Run Phase 3 demo
./nlm --experiment phase3_demo --steps 500

# Run Phase 4 experiment
./nlm --experiment phase4_experiment --neurons 5000

# Run Phase 5 experiment
./nlm --experiment phase5_experiment --duration 30.0

# Save experiment results
./nlm --experiment phase6_demo --save experiment_results.json
```

### Visualization
```bash
# Run simulation with visualization
./nlm --steps 1000 --visualize basic

# Run simulation with advanced visualization
./nlm --steps 1000 --visualize advanced --window-size 1920x1080

# Run simulation without visualization
./nlm --steps 1000 --visualize none
```

### Export and Import
```bash
# Export simulation results
./nlm --experiment phase6_demo --export results.json

# Import configuration from file
./nlm --import saved_config.json --experiment demo

# Compare with baseline
./nlm --benchmark neuron_performance --compare --baseline baseline_results.json
```

## Command Reference

### Simulation Commands

#### `nlm --steps <steps> --neurons <count>`
Run a neural network simulation with specified parameters.

**Arguments:**
- `--steps <steps>`: Number of simulation steps
- `--neurons <count>`: Number of neurons

**Examples:**
```bash
./nlm --steps 1000 --neurons 1000
./nlm --steps 500 --neurons 5000 --verbose
```

#### `nlm --duration <seconds>`
Run simulation for specified duration.

**Arguments:**
- `--duration <seconds>`: Simulation duration in seconds

**Examples:**
```bash
./nlm --duration 10.0 --neurons 2000
./nlm --duration 5.0 --steps 500 --verbose
```

### Configuration Commands

#### `nlm --config <file>`
Load configuration from file.

**Arguments:**
- `<file>`: Configuration file path

**Examples:**
```bash
./nlm --config config.json --steps 2000
./nlm --config experiment.yaml --verbose
```

#### `nlm --save <file>`
Save configuration to file.

**Arguments:**
- `<file>`: Output file path

**Examples:**
```bash
./nlm --save my_config.json
./nlm --save experiment_config.yaml --experiment demo
```

#### `nlm --validate`
Validate configuration.

**Examples:**
```bash
./nlm --config config.json --validate
./nlm --validate --verbose
```

### Testing Commands

#### `nlm --test <category>`
Run tests in specific category.

**Arguments:**
- `<category>`: Test category (memory, neuromodulation, performance, etc.)

**Examples:**
```bash
./nlm --test memory
./nlm --test neuromodulation --verbose
./nlm --test performance --export performance_results.json
```

#### `nlm --test-category <category>`
Run tests by category (alternative to `--test`).

**Arguments:**
- `<category>`: Test category

**Examples:**
```bash
./nlm --test-category memory
./nlm --test-category integration
```

#### `nlm --test-tag <tag>`
Run tests by tag.

**Arguments:**
- `<tag>`: Test tag (unit, integration, performance, regression)

**Examples:**
```bash
./nlm --test-tag integration
./nlm --test-tag performance --format json
```

#### `nlm --test-pattern <pattern>`
Run tests by name pattern.

**Arguments:**
- `<pattern>`: Regular expression pattern for test names

**Examples:**
```bash
./nlm --test-pattern "test_.*_memory"
./nlm --test-pattern ".*integration.*"
```

#### `nlm --test <category> --test-tag <tag>`
Run tests with both category and tag filtering.

**Examples:**
```bash
./nlm --test memory --test-tag integration
./nlm --test performance --test-tag regression
```

### Performance Commands

#### `nlm --benchmark <name>`
Run specific benchmark.

**Arguments:**
- `<name>`: Benchmark name (micro, small, medium, large, neuron_performance, etc.)

**Examples:**
```bash
./nlm --benchmark micro --neurons 1000
./nlm --benchmark neuron_performance --duration 10.0
./nlm --benchmark scaling --export scaling_results.json
```

#### `nlm --scaling-benchmark`
Run comprehensive scaling benchmark.

**Examples:**
```bash
./nlm --scaling-benchmark
./nlm --scaling-benchmark --export scaling_results.json
```

### Experiment Commands

#### `nlm --experiment <name>`
Run specific experiment.

**Arguments:**
- `<name>`: Experiment name (phase3_demo, phase4_demo, phase5_experiment, phase6_demo, etc.)

**Examples:**
```bash
./nlm --experiment phase3_demo
./nlm --experiment phase6_demo --steps 2000
./nlm --experiment phase5_experiment --duration 30.0
```

### Output Commands

#### `nlm --format <format>`
Set output format.

**Arguments:**
- `<format>`: Output format (console, json, xml, html)

**Examples:**
```bash
./nlm --format json --export results.json
./nlm --format html --export results.html
./nlm --format console --verbose
```

#### `nlm --export <file>`
Export results to file.

**Arguments:**
- `<file>`: Output file path

**Examples:**
```bash
./nlm --experiment phase6_demo --export results.json
./nlm --test all --export test_results.json
./nlm --benchmark neuron_performance --export benchmark_results.json
```

#### `nlm --import <file>`
Import results from file.

**Arguments:**
- `<file>`: Input file path

**Examples:**
```bash
./nlm --import config.json --experiment demo
./nlm --import saved_results.json --verbose
```

### Comparison Commands

#### `nlm --compare`
Compare with baseline results.

**Arguments:**
- `--baseline <file>`: Baseline results file path

**Examples:**
```bash
./nlm --benchmark neuron_performance --compare --baseline baseline.json
./nlm --test all --compare --baseline baseline_test_results.json
```

### Visualization Commands

#### `nlm --visualize <mode>`
Enable visualization mode.

**Arguments:**
- `<mode>`: Visualization mode (none, basic, advanced, full)

**Examples:**
```bash
./nlm --visualize basic
./nlm --visualize advanced --window-size 1920x1080
./nlm --visualize full --visualization-delay 16
```

### Utility Commands

#### `nlm --help`
Show help information.

**Examples:**
```bash
./nlm --help
./nlm --test --help
./nlm --benchmark --help
```

#### `nlm --version`
Display version information.

**Examples:**
```bash
./nlm --version
./nlm --experiment --version
```

## Configuration File Formats

### JSON Configuration
```bash
# Save configuration as JSON
./nlm --save config.json --format json

# Load configuration from JSON
./nlm --config config.json
```

### YAML Configuration
```bash
# Save configuration as YAML
./nlm --save config.yaml --format yaml

# Load configuration from YAML
./nlm --config config.yaml
```

### Plain Text Configuration
```bash
# Save configuration as plain text
./nlm --save config.txt --format txt

# Load configuration from plain text
./nlm --config config.txt
```

## Configuration File Examples

### Basic Configuration File
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": true
        }
    }
}
```

### YAML Configuration File
```yaml
brain:
  neuron_count: 1000
  synapse_density: 0.1
  connection_probability: 0.05

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true

neuromodulation:
  curiosity:
    enable: true
  novelty:
    enable: true
```

### Plain Text Configuration File
```
# Brain configuration
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05

# Plasticity configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true

# Neuromodulation configuration
neuromod.curiosity.enable = true
```

## Configuration File Commands

### Creating Configuration Files
```bash
# Create basic configuration
./nlm --config config.json --save basic_config.json

# Create experiment configuration
./nlm --experiment phase6_demo --save experiment_config.json

# Create test configuration
./nlm --test memory --save test_config.json
```

### Modifying Configuration Files
```bash
# Load configuration, modify, and save
./nlm --config config.json --experiment demo --save modified_config.json

# Merge configurations
./nlm --config base_config.json --import additional_config.json --save merged_config.json
```

### Validating Configuration Files
```bash
# Validate JSON configuration
./nlm --config config.json --validate

# Validate YAML configuration
./nlm --config config.yaml --validate

# Validate plain text configuration
./nlm --config config.txt --validate
```

## Output Format Examples

### Console Output
```bash
# Basic console output
./nlm --steps 1000 --verbose

# Console output with progress
./nlm --steps 1000 --verbose --quiet
```

### JSON Output
```bash
# Export test results as JSON
./nlm --test all --format json --export test_results.json

# Export benchmark results as JSON
./nlm --benchmark neuron_performance --format json --export benchmark_results.json
```

### XML Output
```bash
# Export test results as XML
./nlm --test all --format xml --export test_results.xml

# Export configuration as XML
./nlm --config config.json --format xml --export config.xml
```

### HTML Output
```bash
# Export test results as HTML
./nlm --test all --format html --export test_results.html

# Export benchmark results as HTML
./nlm --benchmark neuron_performance --format html --export benchmark_results.html
```

## Error Handling

### Common Errors

1. **Configuration File Not Found**
   ```
   Error: Configuration file not found
   ```
   **Solution**: Verify file path and ensure file exists

2. **Invalid Configuration**
   ```
   Error: Invalid configuration format
   ```
   **Solution**: Check file format and syntax

3. **Permission Denied**
   ```
   Error: Permission denied
   ```
   **Solution**: Check file permissions and user access

4. **Out of Memory**
   ```
   Error: Out of memory
   ```
   **Solution**: Increase system memory or reduce configuration size

5. **Invalid Arguments**
   ```
   Error: Invalid argument
   ```
   **Solution**: Check argument syntax and values

### Error Recovery
```bash
# Use default configuration if file not found
./nlm --config config.json --fallback-default --steps 1000

# Validate before using configuration
./nlm --config config.json --validate --steps 1000

# Retry on failure
./nlm --steps 1000 --retry --max-retries 3
```

## Command Line Completion

### Bash Completion
```bash
# Enable bash completion
source <(nlm completion bash)

# Test completion
echo "test" | nlm complete
```

### Z Completion
```bash
# Enable Z completion
source <(nlm completion zsh)

# Test completion
autoload -Uz compinit
compinit
echo "test" | nlm complete
```

## Advanced Usage

### Custom Configuration
```bash
# Create custom configuration
./nlm --config custom.json --experiment demo --steps 2000 --save custom_results.json

# Modify configuration programmatically
./nlm --config config.json --set brain.neuron_count=5000 --set plasticity.stdp.enable=true
```

### Pipeline Integration
```bash
# Integrate with CI/CD pipeline
./nlm --test all --export test_results.json || exit 1
./nlm --benchmark neuron_performance --export benchmark_results.json || exit 1

# Deploy with monitoring
./nlm --experiment phase6_demo --visualize basic --export deployment_results.json
```

### Scripting
```bash
#!/bin/bash
# NLM Deployment Script

set -e

# Build NLM
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cd ..

# Run simulation
./build/nlm --experiment phase6_demo --steps 2000 --verbose

# Run tests
./build/nlm --test all --export test_results.json

# Run benchmarks
./build/nlm --benchmark neuron_performance --export benchmark_results.json

# Deploy with monitoring
./build/nlm --experiment phase6_demo --visualize basic --export deployment_results.json
```

### Monitoring and Logging
```bash
# Enable monitoring
./nlm --experiment phase6_demo --monitor --log-level debug

# Set up log rotation
./nlm --experiment phase6_demo --log-rotation 100MB --log-retention 10

# Monitor performance
./nlm --benchmark neuron_performance --monitor --perf-frequency 1s
```

## Command Line Options Summary

### Global Options
| Option | Short | Description |
|--------|-------|-------------|
| `--help` | `-h` | Show help information |
| `--version` | `-v` | Display version information |

### Simulation Options
| Option | Short | Description |
|--------|-------|-------------|
| `--steps` | | Number of simulation steps |
| `--duration` | | Simulation duration in seconds |
| `--neurons` | | Number of neurons |

### Configuration Options
| Option | Short | Description |
|--------|-------|-------------|
| `--config` | | Load configuration from file |
| `--save` | | Save configuration to file |
| `--load` | | Load configuration from file |
| `--validate` | | Validate configuration |
| `--profiles` | | Show performance profiles |

### Testing Options
| Option | Short | Description |
|--------|-------|-------------|
| `--test` | | Run tests in specific category |
| `--test-category` | | Run tests by category |
| `--test-tag` | | Run tests by tag |
| `--test-pattern` | | Run tests by name pattern |

### Performance Options
| Option | Short | Description |
|--------|-------|-------------|
| `--benchmark` | | Run specific benchmark |
| `--scaling-benchmark` | | Run scaling benchmarks |

### Experiment Options
| Option | Short | Description |
|--------|-------|-------------|
| `--experiment` | | Run specific experiment |

### Output Options
| Option | Short | Description |
|--------|-------|-------------|
| `--format` | | Output format (console, json, xml, html) |
| `--export` | | Export results to file |
| `--import` | | Import results from file |
| `--compare` | | Compare with baseline results |

### Visualization Options
| Option | Short | Description |
|--------|-------|-------------|
| `--visualize` | | Enable visualization mode |
| `--window-size` | | Set visualization window size |
| `--visualization-delay` | | Set visualization update delay |

### Logging Options
| Option | Short | Description |
|--------|-------|-------------|
| `--debug` | | Enable debug logging |
| `--verbose` | | Enable verbose output |
| `--quiet` | | Suppress non-error output |
| `--log-level` | | Set log level |

## Command Line Tips

### Tips for Better Usage

1. **Use --help for specific commands**
   ```bash
   ./nlm --test --help
   ./nlm --benchmark --help
   ./nlm --experiment --help
   ```

2. **Combine commands for complex workflows**
   ```bash
   ./nlm --config config.json --experiment phase6_demo --steps 2000 --export results.json
   ```

3. **Use short options for brevity**
   ```bash
   ./nlm -v -e phase6_demo -s 2000
   ```

4. **Use environment variables**
   ```bash
   export NLM_CONFIG_FILE=config.json
   export NLM_TEST_CATEGORY=memory
   ./nlm --experiment phase6_demo
   ```

5. **Use aliases for frequently used commands**
   ```bash
   alias nlm-exp='nlm --experiment phase6_demo'
   alias nlm-test='nlm --test all --export test_results.json'
   alias nlm-bench='nlm --benchmark neuron_performance'
   ```

### Command Line Best Practices

1. **Validate before using configuration**
   ```bash
   ./nlm --config config.json --validate
   ```

2. **Use appropriate verbosity levels**
   ```bash
   ./nlm --verbose  # More output
   ./nlm --quiet    # Less output
   ./nlm --debug    # Debug output
   ```

3. **Export results for analysis**
   ```bash
   ./nlm --experiment phase6_demo --export results.json
   ./nlm --test all --export test_results.json
   ```

4. **Monitor performance during execution**
   ```bash
   ./nlm --experiment phase6_demo --monitor
   ```

5. **Use fallbacks and retries**
   ```bash
   ./nlm --steps 1000 --retry --max-retries 3
   ```

## Conclusion

The NLM Command-Line Interface provides comprehensive control over NLM simulations, configuration management, testing, and performance benchmarking. It supports a wide range of options for different use cases and workflows, from simple simulations to complex multi-stage experiments.

By following the examples and best practices in this documentation, you can effectively use the NLM CLI to:

1. **Run simulations** with custom parameters and configurations
2. **Manage configurations** using various file formats
3. **Run tests** and benchmarks with comprehensive filtering
4. **Monitor performance** and analyze results
5. **Export and import** data for further analysis
6. **Integrate with CI/CD** pipelines for automated testing
7. **Deploy NLM** with appropriate monitoring and logging

The CLI is designed to be flexible and powerful, supporting both simple one-line commands and complex multi-stage workflows.


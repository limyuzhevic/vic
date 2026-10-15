# NLM Command-Line Interface Reference

## Overview
NLM provides a comprehensive command-line interface for running simulations, experiments, and managing neural networks.

## Command Structure

### Basic Command Syntax
```bash
nlm [OPTIONS] [ARGS...]
```

### Command Categories
- **Core Commands**: Basic simulation and execution
- **Configuration Commands**: Configuration management
- **Plugin Commands**: Plugin management
- **Experiment Commands**: Experiment execution
- **Debug Commands**: Debugging and analysis
- **Help Commands**: Help and documentation

## Core Commands

### --help
Display help information and command summary.

**Usage:**
```bash
nlm --help
nlm <command> --help
```

**Examples:**
```bash
# Show general help
nlm --help

# Show command-specific help
nlm run --help
```

### --version
Display version information.

**Usage:**
```bash
nlm --version
```

**Output:**
```
NLM Version 2.0.0
Build: 2024-01-15T10:30:00Z

Features:
- Phase 4 cognitive architecture
- Plugin system support
- GPU acceleration
- Distributed computing
```

### --mode
Set the simulation mode.

**Usage:**
```bash
nlm --mode <mode>
```

**Available Modes:**
| Mode | Description |
|------|-------------|
| basic | Basic neural network simulation |
| plasticity | Plasticity learning experiment |
| stdp | STDP verification |
| all | Run all tests |
| batch | Batch processing |
| benchmark | Performance benchmarking |
| validate | Configuration validation |
| experimental | Experimental mode |

**Examples:**
```bash
# Run basic simulation
nlm --mode basic

# Run plasticity experiment
nlm --mode plasticity

# Run all tests
nlm --mode all

# Run validation
nlm --mode validate
```

### --neurons
Set the number of neurons.

**Usage:**
```bash
nlm --neurons <count>
```

**Examples:**
```bash
# Set to 500 neurons
nlm --neurons 500

# Set to 1000 neurons
nlm --nml --neurons 1000
```

### --steps
Set the number of simulation steps.

**Usage:**
```bash
nlm --steps <count>
```

**Examples:**
```bash
# Set to 1000 steps
nlm --steps 1000

# Set to 10000 steps
nlm --steps 10000
```

### --checkpoint
Set the checkpoint file.

**Usage:**
```bash
nlm --checkpoint <file>
```

**Examples:**
```bash
# Set checkpoint file
nlm --checkpoint simulation_checkpoint.bin

# Load checkpoint
nlm --mode batch --checkpoint saved_state.bin
```

### --config
Load configuration from file.

**Usage:**
```bash
nlm --config <file>
```

**Examples:**
```bash
# Load configuration
nlm --config configs/basic_config.cfg

# Override with command-line parameters
nlm --config configs/my_config.cfg --neurons 500
```

## Configuration Commands

### --set-config
Set configuration parameter.

**Usage:**
```bash
nlm --set-config <key> <value>
```

**Examples:**
```bash
# Set neuron count
nlm --set-config neuron_count 1000

# Set connection probability
nlm --set-config connection_probability 0.15

# Set learning rate
nlm --set-config plasticity_learning_rate 0.02
```

### --get-config
Get configuration parameter.

**Usage:**
```bash
nlm --get-config <key>
```

**Examples:**
```bash
# Get neuron count
nlm --get-config neuron_count

# Get connection probability
nlm --get-config connection_probability
```

### --list-config
List all configuration parameters.

**Usage:**
```bash
nlm --list-config
```

### --validate-config
Validate configuration.

**Usage:**
```bash
nlm --validate-config
```

### --export-config
Export configuration to file.

**Usage:**
```bash
nlm --export-config <file>
```\n
### --import-config
Import configuration from file.

**Usage:**
```bash
nlm --import-config <file>
```

## Plugin Commands

### --load-plugin
Load a plugin.

**Usage:**
```bash
nlm --load-plugin <name> <path>
```

**Examples:**
```bash
# Load neuron model plugin
nlm --load-plugin custom_neuron src/plugins/custom_neuron.so

# Load plasticity rule plugin
nlm --load-plugin stpf_rule src/plugins/stpf_rule.so
```

### --unload-plugin
Unload a plugin.

**Usage:**
```bash
nlm --unload-plugin <name>
```

**Examples:**
```bash
# Unload plugin
nlm --unload-plugin custom_neuron
```

### --list-plugins
List all loaded plugins.

**Usage:**
```bash
nlm --list-plugins
```

### --enable-plugin
Enable a plugin.

**Usage:**
```bash
nlm --enable-plugin <name>
```

### --disable-plugin
Disable a plugin.

**Usage:**
```bash
nlm --disable-plugin <name>
```

### --plugin-config
Set plugin configuration.

**Usage:**
```bash
nlm --plugin-config <name>:<key> <value>
```

**Examples:**
```bash
# Set plugin configuration
nlm --plugin-config neuron_model:membrane_tau 0.02

# Set multiple plugin configurations
nlm --plugin-config neuron_model:membrane_tau 0.02 --plugin-config neuron_model:threshold 0.5
```

## Experiment Commands

### --run-experiment
Run an experiment.

**Usage:**
```bash
nlm --run-experiment <type> [options]
```

**Experiment Types:**
| Type | Description |
|------|-------------|
| basic | Basic neural network experiment |
| plasticity | Plasticity learning experiment |
| stdp | STDP verification experiment |
| memory | Memory experiment |
| prediction | Prediction experiment |
| all | Run all experiment types |

**Examples:**
```bash
# Run basic experiment
nlm --run-experiment basic --neurons 500 --steps 1000

# Run plasticity experiment
nlm --run-experiment plasticity --neurons 1000 --steps 5000

# Run memory experiment
nlm --run-experiment memory --neurons 2000 --steps 10000
```

### --batch-experiment
Run batch experiments.

**Usage:**
```bash
nlm --batch-experiment <config_file> [options]
```

**Examples:**
```bash
# Run batch experiments from config file
nlm --batch-experiment experiment_configs/batch_config.txt

# Run batch with custom output
nlm --batch-experiment configs/batch.txt --output-dir results/
```

### --hyperparameter-search
Run hyperparameter optimization.

**Usage:**
```bash
nlm --hyperparameter-search <config_file> [options]
```

**Examples:**
```bash
# Run hyperparameter search
nlm --hyperparameter-search configs/hp_search.txt

# Run with cross-validation
nlm --hyperparameter-search configs/hp_search.txt --cross-validation
```

## Debug Commands

### --debug
Enable debug mode.

**Usage:**
```bash
nlm --debug [log_level]
```

**Log Levels:**
| Level | Description |
|-------|-------------|
| Debug | Detailed debug information |
| Info | Basic information |
| Warning | Warning messages |
| Error | Error messages |

**Examples:**
```bash
# Enable debug mode
nlm --debug

# Enable debug with info level
nlm --debug Info
```

### --log-level
Set log level.

**Usage:**
```bash
nlm --log-level <level>
```

### --log-file
Set log file.

**Usage:**
```bash
nlm --log-file <file>
```

### --profile
Enable profiling.

**Usage:**
```bash
nlm --profile [profile_type]
```

**Profile Types:**
| Type | Description |
|------|-------------|
| basic | Basic profiling |
| detailed | Detailed profiling |
| memory | Memory profiling |
| cpu | CPU profiling |

**Examples:**
```bash
# Enable detailed profiling
nlm --profile detailed

# Enable memory profiling
nlm --profile memory
```

### --trace
Enable tracing.

**Usage:**
```bash
nlm --trace [trace_type]
```

**Trace Types:**
| Type | Description |
|------|-------------|
| neuron | Trace neuron activity |
| synaptic | Trace synaptic activity |
| plasticity | Trace plasticity changes |
| memory | Trace memory activity |

**Examples:**
```bash
# Enable neuron tracing
nlm --trace neuron

# Enable synaptic tracing
nlm --trace synaptic
```

### --validate
Validate configuration and state.

**Usage:**
```bash
nlm --validate
```

### --checkpoints
List checkpoint files.

**Usage:**
```bash
nlm --checkpoints
```

### --backup
Backup current state.

**Usage:**
```bash
nlm --backup <file>
```

## Help Commands

### --help
Display help information.

**Usage:**
```bash
nlm --help
nlm <command> --help
```

**Examples:**
```bash
# Show general help
nlm --help

# Show command-specific help
nlm run --help

# Show help for configuration
nlm config --help
```

### --examples
Show example commands.

**Usage:**
```bash
nlm --examples
```

### --tutorials
Show tutorial links.

**Usage:**
```bash
nlm --tutorials
```

## Advanced Options

### --parallel
Enable parallel processing.

**Usage:**
```bash
nlm --parallel [num_threads]
```

**Examples:**
```bash
# Enable parallel processing with default threads
nlm --parallel

# Enable with 8 threads
nlm --parallel 8
```

### --gpu
Enable GPU acceleration.

**Usage:**
```bash
nlm --gpu [device_id]
```

**Examples:**
```bash
# Enable GPU acceleration
nlm --gpu

# Enable with specific device
nlm --gpu 0
```

### --distributed
Enable distributed computing.

**Usage:**
```bash
nlm --distributed [config_file]
```

**Examples:**
```bash
# Enable distributed computing
nlm --distributed

# Enable with configuration file
nlm --distributed distributed_config.txt
```

### --output
Set output directory.

**Usage:**
```bash
nlm --output <directory>
```

**Examples:**
```bash
# Set output directory
nlm --output results/

# Set output with timestamp
nlm --output results_$(date +%Y%m%d_%H%M%S)/
```

### --timeout
Set timeout for operations.

**Usage:**
```bash
nlm --timeout <seconds>
```

**Examples:**
```bash
# Set 5-minute timeout
nlm --timeout 300

# Set 30-second timeout
nlm --timeout 30
```

## Command Line Arguments Examples

### Basic Usage
```bash
# Run basic simulation with default configuration
nlm --mode basic

# Run with custom configuration
nlm --mode basic --config configs/my_config.cfg

# Run with specific parameters
nlm --mode basic --neurons 1000 --steps 5000

# Run with checkpoint
nlm --mode batch --neurons 500 --steps 2000 --checkpoint saved_state.bin
```

### Advanced Usage
```bash
# Run experiment with all options
nlm --mode experimental \
    --neurons 2000 \
    --steps 10000 \
    --config configs/advanced_config.cfg \
    --profile detailed \
    --log-level Debug \
    --checkpoint experiment_checkpoint.bin \
    --output results/experiment1

# Run plugin-based experiment
nlm --mode basic \
    --load-plugin custom_neuron src/plugins/custom_neuron.so \
    --load-plugin stpf_rule src/plugins/stpf_rule.so \
    --plugin-config neuron_model:membrane_tau 0.02 \
    --plugin-config stpf_rule:learning_rate 0.01

# Run batch experiment with hyperparameter search
nlm --batch-experiment configs/batch.txt \
    --output-dir results/ \
    --parallel 8 \
    --gpu \
    --timeout 3600
```

## Command Line Argument Reference

### Core Arguments
| Argument | Short Form | Description |
|----------|------------|-------------|
| --mode | -m | Simulation mode |
| --neurons | -n | Number of neurons |
| --steps | -s | Number of simulation steps |
| --config | -c | Configuration file |
| --checkpoint | -k | Checkpoint file |
| --help | -h | Help information |
| --version | -v | Version information |

### Configuration Arguments
| Argument | Description |
|----------|-------------|
| --set-config | Set configuration parameter |
| --get-config | Get configuration parameter |
| --list-config | List all configuration parameters |
| --validate-config | Validate configuration |
| --export-config | Export configuration to file |
| --import-config | Import configuration from file |

### Plugin Arguments
| Argument | Description |
|----------|-------------|
| --load-plugin | Load a plugin |
| --unload-plugin | Unload a plugin |
| --list-plugins | List all loaded plugins |
| --enable-plugin | Enable a plugin |
| --disable-plugin | Disable a plugin |
| --plugin-config | Set plugin configuration |

### Experiment Arguments
| Argument | Description |
|----------|-------------|
| --run-experiment | Run an experiment |
| --batch-experiment | Run batch experiments |
| --hyperparameter-search | Run hyperparameter optimization |

### Debug Arguments
| Argument | Description |
|----------|-------------|
| --debug | Enable debug mode |
| --log-level | Set log level |
| --log-file | Set log file |
| --profile | Enable profiling |
| --trace | Enable tracing |
| --validate | Validate configuration and state |
| --checkpoints | List checkpoint files |
| --backup | Backup current state |

### Help Arguments
| Argument | Description |
|----------|-------------|
| --help | Display help information |
| --examples | Show example commands |
| --tutorials | Show tutorial links |

## Error Handling

### Common Error Messages
- **Configuration Error**: "Invalid configuration value: <value> for parameter: <parameter>"
- **File Error**: "Failed to open file: <file>"
- **Plugin Error**: "Failed to load plugin: <plugin>"
- **Memory Error**: "Memory allocation failed"
- **Timeout Error**: "Operation timed out after <seconds> seconds"

### Error Exit Codes
- **0**: Success
- **1**: General error
- **2**: Configuration error
- **3**: File error
- **4**: Plugin error
- **5**: Memory error
- **6**: Timeout error

## Command Line Script Examples

### Simulation Script
```bash
#!/bin/bash

# Run basic simulation
nlm --mode basic --neurons 1000 --steps 5000

# Run with custom configuration
nlm --mode basic --config configs/my_config.cfg

# Run with checkpoint
nlm --mode batch --neurons 500 --steps 2000 --checkpoint saved_state.bin
```

### Experiment Script
```bash
#!/bin/bash

# Run plasticity experiment
nlm --run-experiment plasticity --neurons 1000 --steps 10000

# Run batch experiment
nlm --batch-experiment configs/batch.txt --output-dir results/

# Run hyperparameter search
nlm --hyperparameter-search configs/hp_search.txt
```

### Debug Script
```bash
#!/bin/bash

# Run with debug mode
nlm --debug --log-level Debug --log-file debug.log

# Run with profiling
nlm --profile detailed --neurons 500 --steps 2000

# Run with validation
nlm --validate --config configs/my_config.cfg
```

## Command Line Integration

### Python Integration
```python
import subprocess
import json

def run_nlm_mode(mode, **kwargs):
    cmd = ["nlm", "--mode", mode]
    for key, value in kwargs.items():
        cmd.extend([f"--{key}", str(value)])
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.returncode, result.stdout, result.stderr

# Run basic simulation
returncode, stdout, stderr = run_nlm_mode("basic", neurons=1000, steps=5000)
if returncode == 0:
    print("Simulation completed successfully")
else:
    print(f"Simulation failed: {stderr}")
```

### C++ Integration
```cpp
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class NLMCommandLine {
public:
    NLMCommandLine& setMode(const std::string& mode) {
        mode_ = mode;
        return *this;
    }
    
    NLMCommandLine& setNeurons(int neurons) {
        neurons_ = neurons;
        return *this;
    }
    
    NLMCommandLine& setSteps(int steps) {
        steps_ = steps;
        return *this;
    }
    
    NLMCommandLine& setConfig(const std::string& config) {
        config_ = config;
        return *this;
    }
    
    NLMCommandLine& setCheckpoint(const std::string& checkpoint) {
        checkpoint_ = checkpoint;
        return *this;
    }
    
    std::string buildCommand() const {
        std::stringstream ss;
        ss << "nlm";
        
        if (!mode_.empty()) ss << " --mode " << mode_;
        if (neurons_ > 0) ss << " --neurons " << neurons_;
        if (steps_ > 0) ss << " --steps " << steps_;
        if (!config_.empty()) ss << " --config " << config_;
        if (!checkpoint_.empty()) ss << " --checkpoint " << checkpoint_;
        
        return ss.str();
    }
    
private:
    std::string mode_;
    int neurons_ = 0;
    int steps_ = 0;
    std::string config_;
    std::string checkpoint_;
};

int main() {
    NLMCommandLine cmd;
    cmd.setMode("basic")
       .setNeurons(1000)
       .setSteps(5000)
       .setConfig("configs/basic_config.cfg");
    
    std::cout << "Command: " << cmd.buildCommand() << std::endl;
    
    return 0;
}
```

### Shell Integration
```bash
# Create command function
nlm-run() {
    local mode=$1
    shift
    
    local cmd="nlm --mode $mode"
    while [[ $# -gt 0 ]]; do
        case $1 in
            --neurons)
                cmd+=" --neurons $2"
                shift 2
                ;;
            --steps)
                cmd+=" --steps $2"
                shift 2
                ;;
            --config)
                cmd+=" --config $2"
                shift 2
                ;;
            --checkpoint)
                cmd+=" --checkpoint $2"
                shift 2
                ;;
            *)
                cmd+=" $1"
                shift
                ;;
        esac
    done
    
    eval $cmd
}

# Use the function
nlm-run basic --neurons 1000 --steps 5000 --config configs/basic_config.cfg
```

## Advanced Command Line Features

### Command Line History
```bash
# Save command history
nlm --mode basic --neurons 1000 --steps 5000 >> history.log

# Replay command from history
cat history.log | tail -1 | bash
```

### Command Line Logging
```bash
# Log command and output
nlm --mode basic --neurons 1000 --steps 5000 2>&1 | tee simulation.log

# Log with timestamp
nlm --mode basic --neurons 1000 --steps 5000 2>&1 | while read line; do echo "$(date): $line"; done >> simulation.log
```

### Command Line Automation
```bash
# Run multiple simulations in parallel
for i in {1..10}; do
    nlm --mode basic --neurons 1000 --steps 1000 &
done

# Wait for all simulations to complete
wait
```

## Command Line Best Practices

### 1. Use Short Forms Where Possible
```bash
# Use short forms for frequently used options
nlm -m basic -n 1000 -s 5000
```

### 2. Use Environment Variables
```bash
# Set environment variables
export NLM_MODE=basic
export NLM_NEURONS=1000
export NLM_STEPS=5000

# Use in command
nlm --mode $NLM_MODE --neurons $NLM_NEURONS --steps $NLM_STEPS
```

### 3. Use Configuration Files
```bash
# Create configuration file
# Save as config.cfg
cat > config.cfg << EOF
mode = basic
neurons = 1000
steps = 5000
EOF

# Use configuration file
nlm --config config.cfg
```

### 4. Use Parameter Files
```bash
# Create parameter file
# Save as params.txt
cat > params.txt << EOF
--mode basic
--neurons 1000
--steps 5000
EOF

# Use parameter file
cat params.txt | xargs nlm
```

### 5. Use Scripts
```bash
# Create script
# Save as run_simulation.sh
cat > run_simulation.sh << 'EOF'
#!/bin/bash
set -e

nlm --mode basic --neurons 1000 --steps 5000
EOF

# Make script executable
chmod +x run_simulation.sh

# Run script
./run_simulation.sh
```

## Conclusion

The NLM command-line interface provides a comprehensive set of options for running simulations, experiments, and managing neural networks. This reference guide covers all available commands, options, and examples to help users effectively use the NLM command-line interface.

For more information about specific commands or features, use the `--help` option with the command name.

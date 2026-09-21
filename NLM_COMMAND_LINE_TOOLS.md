# NLM Command Line Tools

This directory contains command-line tools for enhanced interaction with the NLM brain simulator. The tools provide various utilities for brain management, simulation, analysis, and testing.

## Available Tools

### 1. nlm-create-agent
Create and configure agent brains with pre-set parameters

**Usage:**
```bash
nlm-create-agent [OPTIONS] BRAIN_CONFIG
```

**Options:**
- `--quick` - Use quick configuration (500 neurons)
- `--demo` - Use demo configuration (1000 neurons, default world)
- `--batch-count N` - Create N brains for batch testing
- `--output FILE` - Save configuration to file

**Example:**
```bash
nlm-create-agent --demo brain_config.txt
# Creates an agent with demo settings
```

### 2. nlm-run-simulation
Run brain simulations with various configurations

**Usage:**
```bash
nlm-run-simulation [OPTIONS] BRAIN_CONFIG
```

**Options:**
- `--steps N` - Number of simulation steps (default: 1000)
- `--time SECONDS` - Run for specified time (alternative to steps)
- `--verbose` - Print detailed progress information
- `--save CHECKPOINT` - Save checkpoint at specified steps
- `--world-config CONFIG` - World configuration file
- `--enable-features` - Enable all learning features

**Example:**
```bash
nlm-run-simulation --steps 5000 --time 10.0 --enable-features brain_config.txt
```

### 3. nlm-analyze-brains
Analyze multiple brain configurations and performance data

**Usage:**
```bash
nlm-analyze-brains [OPTIONS] CONFIG_FILE
```

**Options:**
- `--batch-create N` - Create N brains for analysis
- `--compare` - Compare brain configurations
- `--export-csv FILE` - Export results to CSV format
- `--statistical-analysis` - Perform statistical analysis
- `--generate-report DIR` - Generate detailed analysis report

**Example:**
```bash
nlm-analyze-brains --batch-create 5 --statistical-analysis --generate-report analysis_results
```

### 4. nlm-checkpoint-manager
Manage brain checkpoints, save/load operations

**Usage:**
```bash
nlm-checkpoint-manager [OPTIONS] BRAIN_FILE
```

**Options:**
- `--list` - List all available checkpoints
- `--create DIR` - Create checkpoint directory with default settings
- `--save FILE` - Save current brain state
- `--load FILE` - Load brain state from file
- `--cleanup RETENTION` - Cleanup old checkpoints (keep last N)
- `--verify FILE` - Verify checkpoint integrity

**Example:**
```bash
nlm-checkpoint-manager --create ./checkpoints --save brain_state.bin
```

### 5. nlm-batch-runner
Run multiple simulations in batch mode for performance testing

**Usage:**
```bash
nlm-batch-runner [OPTIONS] CONFIG_FILE
```

**Options:**
- `--run-count N` - Number of simulation runs (default: 10)
- `--variance-factor F` - Factor for parameter variance (0.0-1.0)
- `--parallel JOBS` - Number of parallel jobs (default: 1)
- `--output-results DIR` - Directory for batch results
- `--collect-statistics` - Collect detailed performance statistics
- `--generate-summary` - Generate summary report

**Example:**
```bash
nlm-batch-runner --run-count 20 --variance-factor 0.1 --parallel 4 configs.yaml
```

### 6. nlm-explore-configs
Explore and discover effective configuration combinations

**Usage:**
```bash
nlm-explore-configs [OPTIONS] PARAMETER_SPACE
```

**Options:**
- `--method grid|random|bayesian` - Parameter search method (default: grid)
- `--iterations N` - Number of iterations (default: 10)
- `--convergence THRESHOLD` - Convergence threshold (default: 0.01)
- `--maximize-metric METRIC` - Metric to maximize (reward, performance, learning)
- `--export-best N` - Export N best configurations
- `--plot-results DIR` - Generate visualization plots

**Example:**
```bash
nlm-explore-configs --method bayesian --iterations 50 --maximize-metric reward configs_space.yaml
```

### 7. nlm-demo-creator
Create demonstration scripts and tutorials

**Usage:**
```bash
nlm-demo-creator [OPTIONS] OUTPUT_FILE
```

**Options:**
- `--type simple|advanced|research` - Demo type (default: simple)
- `--language python|cpp|bash` - Output language (default: python)
- `--interactive` - Create interactive demo with prompts
- `--include-examples` - Include example simulations
- `--add-comments` - Add explanatory comments to code

**Example:**
```bash
nlm-demo-creator --type advanced --language python --include-examples demo_script.py
```

## Configuration Files

### Basic Configuration Format
```bash
# brain configuration file
neuron_count = 1000
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
simulation_timestep = 0.001
enable_reward_modulation = true
enable_curiosity = true
enable_development = true
enable_structural_plasticity = true
random_seed = 42
```

### World Configuration Format
```bash
# world configuration file
width = 20
height = 20
visionWidth = 8
visionHeight = 8
agent_start_x = 10.0
agent_start_y = 10.0
random_seed = 42
energy_decay_rate = 0.001
max_energy = 100.0
```

## Installation

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake python3-dev python3-pip
pip3 install pybind11 scikit-build-core
```

### macOS
```bash
brew install cmake python3
pip3 install pybind11 scikit-build-core
```

### Building Tools
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## Quick Start Examples

### Create and run a simple brain
```bash
# Create configuration
nlm-create-agent --quick brain_config.txt

# Run simulation
nlm-run-simulation --steps 2000 --verbose brain_config.txt
```

### Batch processing with analysis
```bash
# Create multiple brains
nlm-batch-runner --run-count 5 --output-results batch_output configs.yaml

# Analyze results
nlm-analyze-brains --compare --export-csv batch_output/results.csv configs.yaml
```

### Checkpoint management
```bash
# Create checkpoints
nlm-checkpoint-manager --create ./checkpoints --save brain_state.bin

# List checkpoints
nlm-checkpoint-manager --list ./checkpoints

# Load checkpoint
nlm-checkpoint-manager --load ./checkpoints/brain_state.bin brain_state.bin
```

### Configuration exploration
```bash
# Explore parameter space
nlm-explore-configs --method bayesian --iterations 20 parameter_space.yaml

# Get best configurations
nlm-explore-configs --export-best 5 configs_space.yaml
```

## Advanced Usage

### Parallel Processing
```bash
# Run 50 simulations in parallel
nlm-batch-runner --run-count 50 --parallel 8 --output-results parallel_results configs.yaml
```

### Performance Monitoring
```bash
# Run with detailed monitoring
nlm-run-simulation --steps 10000 --verbose --save checkpoints/run_$(date +%Y%m%d_%H%M%S) config.txt
```

### Integration with External Tools
```bash
# Export results for analysis
nlm-analyze-brains --export-csv results.csv --statistical-analysis config.txt

# Generate report
nlm-analyze-brains --generate-report analysis_output config.txt
```

## Automation Scripts

### Run Automated Experiments
```bash
#!/bin/bash
# run_experiments.sh

CONFIG_DIR="configs/"
OUTPUT_DIR="experiments/"

mkdir -p "$OUTPUT_DIR"

for config in "$CONFIG_DIR"*.yaml; do
    echo "Running experiment with $config"
    
    # Create brains
    nlm-create-agent --batch-count 3 "$config" "$OUTPUT_DIR/$(basename "$config" .yaml)"
    
    # Run simulations
    for brain_config in "${output_dir}"$(basename "$config" .yaml)"*.txt"; do
        nlm-run-simulation --steps 1000 "$brain_config"
    done
    
    # Analyze results
    nlm-analyze-brains --statistical-analysis "$output_dir"$(basename "$config" .yaml)""
    
    echo "Completed experiment with $config"
done
```

### Monitoring Script
```bash
#!/bin/bash
# monitor_performance.sh

BRAIN_FILE="brain_state.bin"
LOG_FILE="performance.log"

while true; do
    if [ -f "$BRAIN_FILE" ]; then
        # Get brain statistics
        nlm-analyze-brains --statistical-analysis "$BRAIN_FILE" >> "$LOG_FILE" 2>&1
        
        # Log every 5 minutes
        sleep 300
    else
        echo "Brain file not found, waiting..." >> "$LOG_FILE"
        sleep 60
    fi
done
```

## Troubleshooting

### Common Issues

**Issue:** Command not found
**Solution:** Ensure tools are built and in PATH:
```bash
./build/nlm-create-agent --help
```

**Issue:** Configuration file errors
**Solution:** Check configuration format and required parameters

**Issue:** Memory errors in batch processing
**Solution:** Reduce parallel jobs or increase memory limits

**Issue:** Invalid Python bindings
**Solution:** Reinstall Python dependencies:
```bash
pip3 install -e .
```

## Help and Documentation

For detailed help:
```bash
./build/nlm-create-agent --help
./build/nlm-run-simulation --help
./build/nlm-analyze-brains --help
```

For API documentation:
```bash
python3 -c "import pynlm; help(pynlm)"
```

For examples and tutorials:
```bash
./build/nlm-demo-creator --type simple demo_example.py
```

## License

MIT License - See LICENSE file for details

## Contact

For issues and questions:
- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io
- Support: nlm-support@project.org

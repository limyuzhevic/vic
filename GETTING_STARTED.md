# NLM Phase 2 Getting Started Guide

Welcome to NLM (Neural Learning Machine) Phase 2! This guide will help you get started with the enhanced neural computation system.

## Quick Start

### Installation
If you haven't already, build the project:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Basic Usage

#### 1. Simple Neural Simulation
```bash
# Run the basic demonstration
./nlm --run
```

This will:
- Initialize a neural network with 500 neurons
- Run basic connectivity tests
- Demonstrate synaptic plasticity
- Show STDP (Spike-Timing Dependent Plasticity) learning

#### 2. Custom Configuration
```bash
# Use a custom configuration file
./nlm --run --config my_config.cfg
```

#### 3. Verbose Output
```bash
# See detailed output
./nlm --run --verbose
```

## Understanding the Tests

### Test Types

#### Connectivity Test
```bash
./nlm --test connectivity --steps 1000
```
Tests basic neural connectivity and spike propagation.

#### Plasticity Test
```bash
./nlm --test plasticity --steps 2000
```
Demonstrates learning through synaptic weight changes.

#### STDP Test
```bash
./nlm --test stdp --steps 500
```
Verifies Spike-Timing Dependent Plasticity rules.

#### Benchmark
```bash
./nlm --benchmark --quiet
```
Measures performance and execution time.

#### Interactive Mode
```bash
./nlm --interactive
```
Start interactive session (requires additional setup).

## Configuration Files

### Default Configuration
The system includes a default configuration file at `configs/default.cfg`:

```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
log_filename = nlm.log
max_simulation_steps = 10000
simulation_time_limit = 0.0
visualization_enabled = false
visualization_update_rate = 30.0
```

### Creating Custom Configurations

#### Simple Key-Value Format
Create a file `my_config.cfg`:

```
# My Custom Configuration
neuron_count = 2000
region_count = 3
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
log_level = DEBUG
log_to_file = true
log_filename = my_simulation.log
```

#### JSON Configuration (Advanced)
Create a file `my_config.json`:

```json
{
  "neuron_count": 2000,
  "region_count": 3,
  "connection_probability": 0.15,
  "stdp_ltp_weight": 0.02,
  "stdp_ltd_weight": 0.015,
  "log_level": "DEBUG",
  "log_to_file": true,
  "log_filename": "my_simulation.log"
}
```

## Running Examples

### Example 1: Small Network Test
```bash
./nlm --test connectivity --steps 500 --neurons 100 --regions 1 --verbose
```

### Example 2: Large Network Simulation
```bash
./nlm --test plasticity --steps 3000 --neurons 2000 --regions 4
```

### Example 3: Performance Benchmark
```bash
./nlm --benchmark --quiet --neurons 500 --steps 100
```

### Example 4: Detailed Logging
```bash
./nlm --run --verbose --enable-detailed-logging
```

## Command Line Options

### Basic Options
```bash
--help                    Show help message
--quiet                   Suppress output
--verbose                 Enable verbose output
--run                     Run complete demonstration
```

### Test Options
```bash
--test <mode>            Run specific test (connectivity, plasticity, stdp)
--steps <n>              Number of simulation steps
--neurons <n>            Number of neurons
--regions <n>            Number of regions
```

### Configuration Options
```bash
--config <file>          Load configuration from file
--set <key> <value>      Set configuration value
--validate-config         Validate configuration before running
```

### Advanced Options
```bash
--enable-performance-monitoring   Enable performance monitoring
--enable-memory-profiling        Enable memory profiling
--enable-detailed-logging        Enable detailed logging
--benchmark-full                  Run comprehensive benchmarks
--stress-test                    Run stress tests
--interactive                     Start interactive mode
```

## Running Tests

### All Tests
```bash
./nlm  # Runs all test suites (types, config, random, clock, neuron, synapse, stdp, brain, advanced)
```

### Individual Test Suites
```bash
# See available tests
./nlm --help

# Run specific test (test suite depends on your implementation)
# Note: The exact test names may vary based on your test implementation
```

## Common Issues and Solutions

### Issue: Configuration File Not Found
**Solution:**
```bash
# Use absolute path
./nlm --config /path/to/your/config.cfg

# Or copy config to current directory
cp configs/default.cfg .
./nlm --config ./default.cfg
```

### Issue: Too Many Neurons
**Solution:**
```bash
# Use fewer neurons for testing
./nlm --test connectivity --neurons 100 --steps 100
```

### Issue: No Output
**Solution:**
```bash
# Enable verbose output
./nlm --run --verbose

# Or check if binary was built successfully
ls -la ./nlm
```

### Issue: Performance Issues
**Solution:**
```bash
# Use smaller network for testing
./nlm --test connectivity --neurons 200 --steps 100

# Or increase simulation timestep (if allowed)
./nlm --test connectivity --steps 1000 --set simulation_timestep 0.01
```

## Learning Path

### For Beginners

1. **Start Simple:** Use the default configuration and basic commands
2. **Understand Tests:** Learn what each test does and how to interpret the output
3. **Explore Configuration:** Create your own configuration files
4. **Practice:** Run different combinations of options to see different results

### Command Sequence for Beginners:
```bash
# Step 1: See help
./nlm --help

# Step 2: Run basic demo
./nlm --run

# Step 3: See verbose output
./nlm --run --verbose

# Step 4: Use custom config
./nlm --run --config my_config.cfg

# Step 5: Run specific test
./nlm --test connectivity --steps 500 --verbose
```

### For Intermediate Users

1. **Understand Configuration:** Learn how different settings affect simulation
2. **Performance:** Use performance monitoring tools
3. **Memory Management:** Monitor memory usage during simulations
4. **Advanced Testing:** Use stress testing and benchmarking

### Recommended Progression:
```bash
# 1. Basic simulation
./nlm --run --verbose

# 2. Custom configuration
./nlm --run --config custom.cfg --set neuron_count 1000

# 3. Performance monitoring
./nlm --run --enable-performance-monitoring

# 4. Comprehensive testing
./nlm --benchmark-full --enable-memory-profiling
```

### For Advanced Users

1. **Plugin System:** Explore plugin architecture
2. **Configuration Management:** Advanced configuration operations
3. **Performance Optimization:** Fine-tune performance parameters
4. **Custom Testing:** Design your own test scenarios

### Advanced Commands:
```bash
# Plugin system (when implemented)
./nlm --run --enable-plugin-system --plugin-path ./plugins

# Configuration export/import
./nlm --run --export-config advanced_config.json
./nlm --run --import-config saved_config.json

# Stress testing
./nlm --stress-test --duration 10000 --intensity high

# Custom benchmarking
./nlm --benchmark-full --enable-detailed-logging
```

## Next Steps

### Immediate Actions:
1. **Try the basic examples** to get familiar with the interface
2. **Create a custom configuration** for your specific use case
3. **Run comprehensive tests** to understand the system's capabilities

### Further Learning:
1. **Study configuration files** to understand parameter effects
2. **Experiment with different settings** to see how they impact simulation
3. **Use performance monitoring** to optimize your configurations
4. **Explore advanced testing** features for comprehensive validation

### Community and Support:
1. **Check documentation** for detailed explanations
2. **Review examples** for best practices
3. **Experiment and document** your findings
4. **Contribute** improvements to the system

## Example Session

Here's a complete example of a typical working session:

```bash
# Session 1: Basic Understanding
./nlm --run --verbose

# Session 2: Custom Configuration
./nlm --run --config my_research.cfg --set neuron_count 3000

# Session 3: Performance Analysis
./nlm --run --enable-performance-monitoring --enable-memory-profiling

# Session 4: Comprehensive Testing
./nlm --benchmark-full --enable-detailed-logging

# Session 5: Stress Testing
./nlm --stress-test --duration 5000 --enable-detailed-logging
```

## Troubleshooting Checklist

### Start Here:
1. **Help:** Always check `./nlm --help` first
2. **Configuration:** Ensure config files exist and are readable
3. **Permissions:** Check file and execution permissions
4. **Dependencies:** Ensure all dependencies are installed

### Common Solutions:
1. **File not found:** Use absolute paths or copy files to working directory
2. **No output:** Enable verbose mode or check log files
3. **Performance issues:** Reduce simulation parameters or enable monitoring
4. **Memory issues:** Reduce memory usage or increase available memory
5. **Configuration errors:** Check syntax and format of configuration files

## Success Metrics

### For Beginners:
- Can run basic simulations successfully
- Understands configuration file formats
- Can interpret basic output
- Can modify configuration parameters

### For Intermediate Users:
- Can optimize configurations for specific use cases
- Understands performance implications
- Can use advanced testing features
- Can analyze simulation results

### For Advanced Users:
- Can design custom testing scenarios
- Can optimize performance at system level
- Can implement plugin functionality
- Can contribute to the project

## Final Tips

1. **Start simple and build complexity gradually**
2. **Document your experiments and findings**
3. **Use version control** for configuration files
4. **Back up important configurations**
5. **Share your discoveries** with the community

This guide should help you get started with NLM Phase 2. Remember that the system is designed to be accessible to beginners while providing powerful features for advanced users. Happy experimentation!
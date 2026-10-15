# NLM — 熙然

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Key Improvements and New Features

This document describes the significant improvements made to the NLM neural simulation framework, including enhanced configuration management, a comprehensive experiment framework, and an advanced testing suite.

## System Architecture Improvements

### 1. Enhanced Configuration System
- **JSON/YAML Support**: Full JSON configuration file support with backward compatibility for simple key=value format
- **Type Validation**: Comprehensive type checking for configuration values
- **Source Tracking**: Configuration sources (File, CommandLine, Default, Runtime) are tracked
- **Validation**: Configuration validation with helpful error messages

### 2. Experiment Framework
- **Modular Design**: Abstract experiment base class for easy experiment creation
- **Experiment Factory**: Dynamic experiment creation from configuration
- **Parameter Management**: Flexible parameter handling with type support
- **Result Tracking**: Comprehensive experiment result tracking and reporting

### 3. Comprehensive Testing Framework
- **Multiple Test Categories**: Basic functionality, performance, integration, and stress testing
- **Performance Metrics**: Throughput, memory usage, efficiency calculations
- **Detailed Reporting**: Console and JSON output for test results
- **Benchmarking**: Performance benchmarking with detailed analysis

### 4. Enhanced CLI Interface
- **Interactive Mode**: Interactive command-line interface for experiment management
- **Experiment Management**: Run, list, and manage experiments via CLI
- **Configuration Support**: Load and save experiment configurations
- **Help System**: Comprehensive help system with examples

## Building and Running

### Quick Start

1. **Clone and build the project:**
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```

2. **Run the main simulation:**
```bash
cd build
./nlm --neurons=1000 --steps=5000
```

3. **Run Phase 3 demo:**
```bash
./nlm_phase3_demo
```

4. **Run Phase 4 demo:**
```bash
./nlm_phase4_demo
```

### New Test Framework

1. **Run comprehensive test suite:**
```bash
cd build
./nlm_test --run-all --neurons=1000 --steps=2000 --save-results
```

2. **Run specific test categories:**
```bash
./nlm_test --run-basic --neurons=500
./nlm_test --run-performance --steps=5000
./nlm_test --run-integration
./nlm_test --run-stress
```

3. **Run benchmarks:**
```bash
./nlm_test --benchmarks --neurons=2000 --memory_limit_mb=500
```

4. **Interactive experiment runner:**
```bash
./nlm --cli
```

### Experiment Configuration

1. **Create experiment configuration file:**
```bash
# configs/demo/integration.json
cat > configs/demo/integration.json << 'EOF'
{
    "version": "2.0",
    "format": "json",
    "config_entries": [
        {
            "key": "neurons",
            "int_value": 1000,
            "source": 0,
            "description": "Number of neurons in the brain"
        },
        {
            "key": "steps",
            "int_value": 2000,
            "source": 0,
            "description": "Maximum simulation steps"
        }
    ]
}
EOF
```

2. **Run experiment with configuration:**
```bash
cd build
./nlm --run-experiment integration --config=configs/demo/integration.json
```

## Configuration File Format

### JSON Configuration (New)
```json
{
    "version": "2.0",
    "format": "json",
    "config_entries": [
        {
            "key": "neurons",
            "int_value": 1000,
            "source": 0,
            "description": "Number of neurons"
        },
        {
            "key": "plasticity",
            "bool_value": true,
            "source": 0,
            "description": "Enable plasticity"
        }
    ]
}
```

### Simple Key=Value Format (Backward Compatible)
```
# configs/default.cfg
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Connection probability for random connectivity
connection_probability = 0.1
```

## Available Experiments

### Basic Experiments
1. **basic-connectivity**
   - Description: Basic neural connectivity test
   - Parameters: neurons, test_steps
   - Use: `./nlm --run-experiment basic-connectivity`

2. **plasticity**
   - Description: Plasticity and learning experiment
   - Parameters: neurons, plasticity_steps, learning_rate, stdp_ltp, stdp_ltd
   - Use: `./nlm --run-experiment plasticity --learning_rate=0.02`

3. **learning**
   - Description: Machine learning benchmark
   - Parameters: iterations, learning_rate
   - Use: `./nlm --run-experiment learning --iterations=5000`

4. **integration**
   - Description: Phase 6 integration test
   - Parameters: neurons, steps
   - Use: `./nlm --run-experiment integration --neurons=2000`

### Experiment Commands

- **Run experiment:** `./nlm --run-experiment <experiment-name>`
- **List experiments:** `./nlm --list-experiments`
- **Load configuration:** `./nlm --config <config-file>`
- **Save configuration:** `./nlm --save <config-file>`
- **Help:** `./nlm --help`

## Test Framework

### Test Categories

1. **Basic Functionality Tests**
   - Neuron basic functionality (LIF dynamics, spike generation)
   - Synapse basic functionality (transmission, weight changes)
   - Plasticity basic functionality (STDP, Hebbian rules)

2. **Performance Tests**
   - Scalability testing with different neuron counts
   - Memory efficiency testing
   - Performance benchmarking

3. **Integration Tests**
   - Circuit integration testing
   - Learning integration testing
   - System component interaction testing

4. **Stress Tests**
   - High neuron count stress testing
   - Heavy plasticity load testing
   - Resource constraint testing

### Test Commands

- **Run all tests:** `./nlm_test --run-all --save-results`
- **Run basic tests:** `./nlm_test --run-basic`
- **Run performance tests:** `./nlm_test --run-performance`
- **Run integration tests:** `./nlm_test --run-integration`
- **Run stress tests:** `./nlm_test --run-stress`
- **Run benchmarks:** `./nlm_test --benchmarks`
- **Show help:** `./nlm_test --help`

### Test Configuration

```bash
# Test configuration options
--neurons=N           - Number of neurons (default: 1000)
--steps=N             - Number of simulation steps (default: 2000)
--learning_rate=R     - Learning rate (default: 0.01)
--plasticity_rate=R   - Plasticity rate (default: 0.5)
--memory_limit_mb=N   - Memory limit in MB (default: 100)
--save-results        - Save test results to file
```

## Command Line Interface (CLI)

### Interactive Mode

Start the interactive experiment runner:
```bash
./nlm --cli
```

Available commands:
- **list-experiments**: List all available experiments
- **run <experiment>**: Run a specific experiment
- **config <file>**: Load configuration from file
- **save <file>**: Save current configuration
- **help**: Show help information
- **exit**: Exit the program

### Example Interactive Session
```
nlm> list-experiments
basic-connectivity
plasticity
learning
integration

nlm> run integration
Running experiment: integration
  Running basic connectivity test...
  Running plasticity experiment...
  Running learning experiment...
  Integration test completed successfully!

nlm> config configs/demo/integration.json
Configuration loaded from: configs/demo/integration.json

nlm> save my_experiment.json
Saving configuration to: my_experiment.json
(Configuration saving is not yet implemented)
```

## Configuration Management

### Configuration Loading

1. **From file:** `./nlm --config <file>`
2. **From command line:** `./nlm --neurons=1000 --steps=2000`
3. **From default:** Use built-in default values

### Configuration Sources

Configuration values are tracked by source:
- **Default**: Built-in default values
- **File**: Values loaded from configuration file
- **CommandLine**: Values from command line arguments
- **Runtime**: Values set at runtime

### Configuration Validation

All configuration values are validated for type and range:
- Numeric values must be within acceptable ranges
- String values must not be empty
- Boolean values must be true/false
- Vector values must be properly formatted

## Performance Improvements

### Performance Metrics

The test framework provides detailed performance metrics:

1. **Throughput**: Neurons processed per second
2. **Memory Usage**: Memory consumption in MB
3. **Efficiency**: Performance relative to resource usage
4. **Scalability**: Performance scaling with network size
5. **Consistency**: Variance in performance across runs

### Optimization Features

1. **Memory Pools**: Efficient memory allocation and deallocation
2. **Event Queues**: Optimized task scheduling and execution
3. **SIMD Vectorization**: Single instruction, multiple data optimizations
4. **Parallel Processing**: Multi-core support for parallel execution
5. **Checkpointing**: Intermediate state saving for long-running experiments

## Reporting and Monitoring

### Test Results Reporting

Test results are available in multiple formats:

1. **Console Output**: Detailed text output to terminal
2. **JSON Reports**: Machine-readable JSON files
3. **Summary Reports**: High-level performance summaries
4. **Detailed Metrics**: Component-specific performance data

### Performance Monitoring

The framework includes built-in monitoring:
- **Memory Usage**: Real-time memory consumption tracking
- **CPU Usage**: Processor utilization monitoring
- **Execution Time**: Performance timing and analysis
- **Resource Usage**: Overall system resource consumption

## Development and Extension

### Adding New Experiments

To add a new experiment:

1. **Create experiment class** inheriting from `Experiment`:
```cpp
class MyExperiment : public Experiment {
public:
    std::string getName() const override { return "my-experiment"; }
    std::string getDescription() const override { 
        return "My custom experiment"; 
    }
    std::shared_ptr<ExperimentResult> run(const ExperimentConfig& config) override {
        // Implement experiment logic
    }
    bool validateConfig(const ExperimentConfig& config) const override {
        // Validate configuration
    }
};
```

2. **Register experiment in factory:**
```cpp
ExperimentFactory::getInstance().registerExperiment(
    "my-experiment",
    "My custom experiment",
    []() { return std::make_unique<MyExperiment>(); }
);
```

### Adding New Test Categories

To add a new test category:

1. **Create test implementation**:
```cpp
void ComprehensiveTestSuite::registerCustomTests() {
    testSuite_["custom_test"] = {
        "Custom Test",
        "My custom test implementation",
        [this](const ExperimentConfig& config) {
            return runCustomTest(config);
        }
    };
}
```

2. **Implement test logic** in the test suite class.

### Configuration File Examples

#### Experiment Configuration
```json
{
    "experiment": "integration",
    "parameters": {
        "neurons": 1000,
        "steps": 2000,
        "plasticity": true
    }
}
```

#### Test Configuration
```json
{
    "test_suite": "comprehensive",
    "parameters": {
        "neurons": 2000,
        "steps": 5000,
        "learning_rate": 0.02
    },
    "options": {
        "save_results": true,
        "detailed_output": true
    }
}
```

## Scientific Validation

### Experiment Validation

The test framework provides validation of scientific correctness:

1. **Biological Plausibility**: Neuron dynamics follow LIF equations
2. **Plasticity Rules**: STDP and Hebbian rules are mathematically correct
3. **Integration**: All brain systems are properly connected
4. **Scalability**: Performance scales appropriately with network size

### Performance Validation

Performance metrics are validated against expected ranges:

1. **Throughput**: Should scale linearly with neuron count
2. **Memory Efficiency**: Should remain constant or improve with scale
3. **Learning Speed**: Should improve with appropriate plasticity
4. **Stability**: System should remain stable over simulation time

## Known Limitations

### Current Limitations

1. **Simulation Scale**: Limited to approximately 10,000 neurons due to memory constraints
2. **Simulation Time**: Maximum simulation time limited by computational resources
3. **Visualization**: Limited visualization capabilities
4. **Real-world Integration**: Limited integration with real-world environments

### Future Improvements

Planned future enhancements:

1. **GPU Acceleration**: GPU-based acceleration for larger networks
2. **Distributed Computing**: Distributed computing for very large networks
3. **Enhanced Visualization**: Advanced visualization capabilities
4. **Real-world Integration**: Integration with real-world sensors and actuators
5. **Cloud Computing**: Cloud-based deployment and scaling

## Support and Documentation

### Getting Help

1. **Documentation**: Comprehensive documentation available in this file
2. **Examples**: Code examples and demonstrations
3. **Configuration**: Configuration file examples and documentation
4. **Testing**: Test framework documentation and examples

### Reporting Issues

1. **Bug Reports**: Report bugs via the project issue tracker
2. **Feature Requests**: Request new features or enhancements
3. **Documentation Issues**: Report documentation errors or omissions
4. **Performance Issues**: Report performance problems or bottlenecks

### Contributing

1. **Code Contributions**: Submit pull requests with proper documentation
2. **Bug Fixes**: Fix reported bugs with appropriate testing
3. **Documentation**: Improve documentation and examples
4. **Testing**: Add new tests and test coverage

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Authors

Research project — See docs for scientific background.

## References

1. **NLM Documentation**: https://nlm.readthedocs.io
2. **NLM GitHub**: https://github.com/nlm-project/nlm
3. **Phase 6 Documentation**: See docs/PHASE6_FINAL_AUDIT.md
4. **Scientific Background**: See docs/SCIENCE.md
5. **Architecture Documentation**: See docs/ARCHITECTURE.md
6. **Roadmap Documentation**: See docs/ROADMAP.md
7. **Experiments Documentation**: See docs/EXPERIMENTS.md
8. **Limitations Documentation**: See docs/LIMITATIONS.md

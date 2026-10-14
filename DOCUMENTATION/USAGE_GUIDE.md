# NLM Phase 2 Improvements - Usage Guide

## Overview

This document provides comprehensive information about the enhanced NLM Phase 2 neural learning machine, including new features, usage instructions, and technical improvements.

## Quick Start

### Basic Usage

```bash
# Run all default tests
./nlm

# Run with custom settings
./nlm --neuron 1000 --timestep 0.002

# Run specific test scenarios
./nlm --basic-connectivity --plasticity

# Run integration test
./nlm --integration

# Run in benchmark mode
./nlm --benchmark --verbose

# Show help information
./nlm --help
```

### Configuration

#### Default Configuration
The enhanced system uses `configs/default_phase2.cfg` as the default configuration file, which includes Phase 2-specific settings:

- Real LIF neuron dynamics
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Enhanced logging with color support
- Advanced metrics collection

#### Custom Configuration
```bash
# Use custom configuration file
./nlm --config my_config.cfg

# Override configuration via command line
./nlm --config my_config.cfg --neuron 2000 --stdp-ltp 0.015
```

## Command Line Options

### Test Selection Options

| Option | Description | Default |
|--------|-------------|---------|
| `--basic-connectivity` | Run basic connectivity test | ✓ Enabled |
| `--no-basic-connectivity` | Skip basic connectivity test | ✗ Disabled |
| `--plasticity` | Run plasticity learning experiment | ✓ Enabled |
| `--no-plasticity` | Skip plasticity test | ✗ Disabled |
| `--stdp-verification` | Run STDP verification test | ✓ Enabled |
| `--no-stdp-verification` | Skip STDP verification test | ✗ Disabled |
| `--integration` | Run integration test | ✗ Disabled |
| `--all-tests` | Run all tests including integration | ✗ Disabled |

### Configuration Options

| Option | Description | Type | Default |
|--------|-------------|------|---------|
| `--neuron COUNT` | Number of neurons | Integer | 500 |
| `--region COUNT` | Number of brain regions | Integer | 1 |
| `--timestep VALUE` | Simulation timestep (seconds) | Float | 0.001 |
| `--max-steps COUNT` | Maximum simulation steps | Integer | 1000 |
| `--stdp-ltp WEIGHT` | STDP LTP weight | Float | 0.02 |
| `--stdp-ltd WEIGHT` | STDP LTD weight | Float | 0.015 |
| `--stdp-tau VALUE` | STDP time constant | Float | 20.0 |
| `--synaptogenesis RATE` | Synaptogenesis rate | Float | 0.0001 |
| `--pruning RATE` | Pruning rate | Float | 0.00001 |

### Output and Logging Options

| Option | Description | Default |
|--------|-------------|---------|
| `--verbose`, `-v` | Enable verbose output | ✗ Disabled |
| `--quiet`, `-q` | Quiet mode (minimal output) | ✗ Disabled |
| `--no-report` | Skip detailed report generation | ✗ Disabled |
| `--benchmark` | Run in benchmark mode | ✗ Disabled |
| `--output-file FILE` | Save results to file | Not set |
| `--log-file FILE` | Log output to file | Not set |

### Special Options

| Option | Description |
|--------|-------------|
| `--help`, `-h` | Show help message and exit |
| `--version` | Show version information and exit |

## Experimental Scenarios

### Scenario 1: Basic Validation
```bash
# Run quick validation of core functionality
./nlm --basic-connectivity --quiet
```

### Scenario 2: Learning Experiment
```bash
# Run detailed plasticity learning experiment
./nlm --plasticity --verbose --benchmark
```

### Scenario 3: Plasticity Analysis
```bash
# Run plasticity experiment with custom parameters
./nlm --plasticity \
      --neuron 1000 \
      --stdp-ltp 0.015 \
      --stdp-ltd 0.012 \
      --stdp-tau 30.0 \
      --verbose
```

### Scenario 4: Integration Testing
```bash
# Run full integration test for system verification
./nlm --integration --verbose
```

### Scenario 5: Advanced Research
```bash
# Run comprehensive research configuration
./nlm --all-tests \
      --neuron 2000 \
      --region 3 \
      --timestep 0.0005 \
      --benchmark \
      --verbose \
      --output-file research_results.json
```

## Configuration File Format

### Simple Configuration (Current)
```
# NLM Phase 2 Enhanced Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = SimpleWorld
environment_width = 16
environment_height = 16
log_level = INFO
log_to_file = false
max_simulation_steps = 2000
experimental_type = Phase2RealNeuralComputation
enable_advanced_metrics = true
benchmark_mode = false
visualization_enabled = false
```

### Future JSON/YAML Configuration
```json
{
  "nlm_phase2": {
    "version": "2.0",
    "random_seed": 42,
    "simulation": {
      "timestep": 0.001,
      "neuron_count": 500,
      "region_count": 1,
      "max_steps": 2000
    },
    "plasticity": {
      "stdp": {
        "ltp_weight": 0.02,
        "ltd_weight": 0.015,
        "time_constant": 20.0
      },
      "structural": {
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001
      }
    },
    "experiments": {
      "basic_connectivity": true,
      "plasticity_experiment": true,
      "stdp_verification": true,
      "integration_test": false
    },
    "output": {
      "verbose": true,
      "benchmark": false,
      "generate_reports": true
    }
  }
}
```

## Output and Reporting

### Console Output

The enhanced system provides detailed output with the following information:

#### Basic Test Output
```
=== NLM Phase 2 Real Neural Computation ===
Configuration: 500 neurons, 1000 steps

=== Test 1: Basic Neural Connectivity ===
Injecting current into 10 neurons...
Spikes generated: 42
✓ Spikes propagate through network
```

#### Detailed Learning Experiment Output
```
=== Learning Experiment Results ===
Weight Statistics:
  Initial mean weight: 0.512
  Final mean weight: 0.578
  Change: 0.066

Synaptic Changes:
  Strengthened: 125
  Weakened: 32
  Unchanged: 843

Learning Score: 0.15 (FAIR)
✓ LEARNING DETECTED: Synaptic weights changed through experience
```

#### Integration Test Output
```
=== Integration Verification ===
Working Memory: CONNECTED
Episodic Memory: CONNECTED
Neuromodulation: CONNECTED
Prediction: CONNECTED
Development: CONNECTED

✓ Integration is working well!
```

### File Output

#### Results File
When `--output-file` is specified, the system generates comprehensive result files in JSON format.

#### Log File
When `--log-file` is specified, all logging output is saved to the specified file.

## Advanced Features

### Enhanced Learning Metrics

The enhanced system provides detailed learning analysis:

- **Learning Score**: Overall measure of learning effectiveness
- **Weight Distribution**: Statistical analysis of synaptic weight changes
- **Activity Analysis**: Spike statistics and firing rate analysis
- **Quality Assessment**: Learning quality classification (EXCELLENT, GOOD, FAIR, POOR)

### Benchmark Mode

Enable benchmark mode with `--benchmark` to:

- Measure execution time
- Collect performance metrics
- Compare against baseline performance
- Generate performance reports

### Configuration Validation

The system validates configuration parameters:

- Neuron count must be ≥ 10
- Timestep must be > 0
- Plasticity parameters must be within valid ranges
- Configuration files are validated for format and content

### Error Handling

Enhanced error handling includes:

- **Clear error messages**: Detailed descriptions of what went wrong
- **Graceful degradation**: System continues when possible
- **Recovery mechanisms**: Automatic retry for non-critical errors
- **Validation feedback**: Immediate feedback on invalid configurations

## Performance Considerations

### Memory Usage

- **Standard configuration (500 neurons)**: ~100MB RAM
- **Large configuration (5000 neurons)**: ~1GB RAM
- **Benchmark mode**: Additional profiling overhead

### Computational Complexity

- **Basic connectivity test**: O(n²) for connectivity setup
- **Plasticity experiment**: O(n * steps) for simulation
- **Integration test**: O(n * total_steps) for full system

### Optimization Recommendations

1. **For research**: Use 1000-5000 neurons for detailed analysis
2. **For production**: Use 500-1000 neurons for balanced performance
3. **For development**: Use 100-500 neurons for rapid iteration
4. **For benchmarking**: Use 2000-5000 neurons for performance measurement

## Extending the System

### Adding New Tests

1. Create new test functions in the ExperimentManager class
2. Add command-line options for test selection
3. Implement appropriate validation and reporting
4. Update documentation with new usage examples

### Custom Configuration

1. Extend ConfigurationLoader class to support new file formats
2. Add configuration schema validation
3. Implement configuration inheritance and overrides
4. Add configuration versioning support

### Performance Enhancements

1. Implement SIMD optimizations for neuron computations
2. Add parallel processing for large networks
3. Implement memory pools for efficient allocation
4. Add checkpoint/restart capabilities

## Troubleshooting

### Common Issues

#### Issue: Help system not working
**Solution**: Ensure `--help` option is properly registered in CommandLineParser

#### Issue: Configuration loading fails
**Solution**: Check file permissions and format, ensure file exists

#### Issue: Memory usage too high
**Solution**: Reduce neuron count or increase available memory

#### Issue: Performance too slow
**Solution**: Reduce simulation steps or optimize hardware

#### Issue: Integration test failing
**Solution**: Check system integration by running individual tests first

### Debug Mode

Enable debug mode with:
```bash
./nlm --verbose --quiet
```

This will show detailed debug information while suppressing standard output.

## Migration Guide

### From Original Phase 2

The enhanced system maintains backward compatibility:

1. **Original functionality**: All original test scenarios still work
2. **Configuration**: Existing configs load without modification
3. **Command line**: Basic `--config` option still supported
4. **Output**: Results format preserved with enhanced details

### Upgrading Recommendations

1. **New users**: Start with `./nlm --help` to see all options
2. **Research users**: Use `--benchmark` for performance measurement
3. **Development users**: Use `--quiet` for rapid iteration
4. **Production users**: Use `--no-report` for minimal output

## Contact and Support

For questions or support:

- Check the documentation directory for additional resources
- Review the code comments for implementation details
- Refer to the issue tracker for known issues
- Submit enhancement requests through the project tracker

## Future Enhancements

The following enhancements are planned:

1. **JSON/YAML Configuration Parser**: Complete implementation
2. **Web Interface**: Interactive visualization and control
3. **Distributed Computing**: Multi-system scaling
4. **Advanced Analytics**: Machine learning for optimization
5. **Plugin Architecture**: Dynamic module loading
6. **Real-time Visualization**: Interactive network monitoring

## Conclusion

The NLM Phase 2 enhancements provide:

- **Improved user experience**: Comprehensive CLI with help system
- **Enhanced functionality**: Advanced metrics and analysis
- **Better code quality**: Modular, maintainable architecture
- **Rich documentation**: Detailed usage guides and examples
- **Performance optimizations**: Efficient execution and memory usage
- **Extensibility**: Easy to add new features and capabilities

These improvements make NLM Phase 2 more powerful for research while remaining accessible to beginners, with comprehensive documentation and examples for all use cases.

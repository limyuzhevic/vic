# NLM Phase 2: Real Neural Computation - User Guide

## Overview

This document provides comprehensive information about the NLM Phase 2 neural computation system, including the new command-line interface, modular test runner, and advanced features.

## What is NLM Phase 2?

NLM Phase 2 implements real spiking neural computation with:
- **Leaky Integrate-and-Fire (LIF) neuron dynamics**
- **Event-driven spike propagation with delays** 
- **STDP and Hebbian plasticity rules**
- **Structural plasticity (synaptogenesis/pruning)**
- **Advanced command-line interface for testing and experimentation**

## Command-Line Interface

### Basic Usage

```bash
./nlm [options]
```

### Available Test Modes

| Test Mode | Description | Command |
|-----------|-------------|---------|
| `all` (default) | Run all enabled tests | `--test=all` |
| `test1` | Basic neural connectivity test | `--test=test1` |
| `test2` | Plasticity learning experiment | `--test=test2` |
| `test3` | STDP verification | `--test=test3` |
| `stp` | Advanced STDP experiment | `--test=stp` |
| `experiment` | General learning experiment | `--test=experiment` |
| `specific` | Run specific tests | `--test=specific --specific=tests` |

### Specific Test Options

Run a comma-separated list of specific tests:

```bash
./nlm --test=specific --specific=test1,test2,test3
```

### Individual Test Mode

Run individual tests:

```bash
./nlm --test1-only          # Only run basic connectivity test
./nlm --test2-only          # Only run plasticity learning test  
./nlm --test3-only          # Only run STDP verification test
./nlm --stp-only           # Only run advanced STDP test
./nlm --experiment-only    # Only run general learning experiment
```

### Test-Specific Parameters

#### Test 1: Basic Connectivity

```bash
./nlm --test=test1 --test1-inject=100 --test1-steps=100
```

Options:
- `--test1-inject=N`: Current injection amount (default: 50)
- `--test1-steps=N`: Number of simulation steps (default: 50)

#### Test 2: Plasticity Learning

```bash
./nlm --test=test2 --test2-steps=2000 --test2-pattern=30
```

Options:
- `--test2-steps=N`: Number of learning steps (default: 1000)
- `--test2-pattern=N`: Number of pattern neurons (default: 20)

#### Test 3: STDP Verification

```bash
./nlm --test=test3 --test3-trials=100 --test3-current=80
```

Options:
- `--test3-trials=N`: Number of STDP trials (default: 50)
- `--test3-current=N`: Current injection for STDP (default: 60.0)

### Configuration Options

#### Brain Configuration

```bash
./nlm --neuron-count=1000 --region-count=2 --connection-p=0.2 --seed=123
```

Options:
- `--neuron-count=N`: Number of neurons (default: 500)
- `--region-count=N`: Number of brain regions (default: 1)
- `--connection-p=N`: Connection probability (default: 0.15)
- `--seed=N`: Random seed for reproducibility (default: 42)

#### Simulation Parameters

```bash
./nlm --timestep=0.0005
```

Options:
- `--timestep=N`: Simulation timestep in seconds (default: 0.001)

#### Configuration File

Load configuration from file:

```bash
./nlm --config=my_config.cfg
```

Or override config file:

```bash
./nlm --config=default.cfg
```

### General Options

| Option | Description |
|--------|-------------|
| `--verbose, -v` | Enable verbose output |
| `--timing, -t` | Enable timing information |
| `--benchmark, -b` | Enable benchmarking mode |
| `--output=FILE` | Save results to file |
| `--help, -h` | Show help information |

### Example Command Lines

#### Comprehensive Test Suite

```bash
./nlm --test=all --verbose --timing --neuron-count=1000
```

#### Focused Plasticity Test

```bash
./nlm --test=test2 --test2-steps=3000 --test2-pattern=50 --verbose
```

#### Quick Connectivity Check

```bash
./nlm --test=test1 --test1-inject=25 --test1-steps=25 --timing
```

#### STDP Robustness Test

```bash
./nlm --test=stp --test3-trials=200 --test3-current=70 --verbose
```

#### Learning Experiment

```bash
./nlm --test=experiment --neuron-count=2000 --region-count=3
```

#### Specific Test Combination

```bash
./nlm --test=specific --specific=test1,test3 --timing
```

## Test Results

When tests run, you'll see output like this:

```
=== TEST RESULTS SUMMARY ===

Test: test1
  Status: PASS
  Wall time: 0.125s
  Spikes: 8456
  Neurons: 500
  Synapses: 12345
  Avg firing rate: 16.91 Hz

Test: test2
  Status: PASS
  Wall time: 2.456s
  Spikes: 45678
  Neurons: 500
  Synapses: 12350
  Avg firing rate: 18.34 Hz
  Mean weight change: 0.012345
  Learning detected: YES

=== SUMMARY ===
Total tests: 2
Passed: 2
Failed: 0
Total time: 2.581s
```

## Test Descriptions

### Test 1: Basic Neural Connectivity
Tests whether spikes propagate through the network by injecting current into neurons and measuring the resulting activity.

### Test 2: Plasticity Learning Experiment
Demonstrates learning through Hebbian and STDP plasticity rules, showing how synaptic weights change with repeated patterns.

### Test 3: STDP Verification
Validates the STDP learning rule by creating correlated pre-post activity and measuring synaptic weight changes.

### Test 4: Advanced STDP
Tests STDP robustness across multiple synapses with correlated activity patterns.

### Test 5: Learning Experiment
A comprehensive test showing complex learning patterns with alternating excitation and inhibition.

## Performance Considerations

### Memory Usage
- Default: ~500 neurons, ~12,000 synapses
- Can scale up to thousands of neurons with `--neuron-count`
- Memory usage grows approximately linearly with neuron count

### Timing
- Each test can run independently with `--test=X` options
- Test timing is reported in seconds with millisecond precision
- Benchmark mode provides detailed timing information

### Parallelization
- Tests are run sequentially (brain reset between tests)
- Each test manages its own brain initialization
- Modular design allows for independent test execution

## Troubleshooting

### Common Issues

**Test fails to run**
- Check command-line syntax
- Ensure all required dependencies are installed
- Verify brain initialization parameters

**No spikes detected**
- Check current injection parameters
- Verify neuron thresholds are appropriate
- Ensure connectivity is set correctly

**Performance issues**
- Reduce `--neuron-count` for faster testing
- Check system resources
- Monitor memory usage

### Debugging

Enable verbose mode for detailed output:

```bash
./nlm --test=test1 --verbose
```

Use timing mode for performance analysis:

```bash
./nlm --test=test2 --timing
```

## Advanced Usage

### Configuration File

Create a configuration file (JSON format) for consistent test settings:

```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.2,
    "random_seed": 123,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

Save as `my_config.cfg` and use:

```bash
./nlm --config=my_config.cfg --test=test1
```

### Custom Test Sequences

Create custom test sequences by combining specific tests:

```bash
./nlm --test=specific --specific=test1,test2,test3,test4
```

### Batch Testing

Create batch test scripts for automated testing:

```bash
#!/bin/bash
# run_all_tests.sh
./nlm --test=all --neuron-count=1000 --timing
./nlm --test=specific --specific=test1,test2 --neuron-count=500
```

Make executable and run:

```bash
chmod +x run_all_tests.sh
./run_all_tests.sh
```

## API Reference

### TestRunner Class

The TestRunner class provides programmatic access to all tests:

```cpp
TestRunner testRunner;
testRunner.setupBrain(config);
testRunner.enableTest("test1", true);
testRunner.enableTest("test2", true);
testRunner.enableTest("test3", true);

std::vector<TestResult> results = testRunner.runAllTests();
testRunner.printResultsSummary();
```

### TestResult Structure

```cpp
struct TestResult {
    std::string testName;
    double wallClockTime;
    bool success;
    std::string errorMessage;
    size_t spikeCount;
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    float avgFiringRate;
    float meanWeightChange;
    bool learningDetected;
};
```

### CommandLineParser Class

```cpp
CommandLineOptions options = CommandLineParser::parse(argc, argv);
CommandLineParser::printHelp();
```

## Frequently Asked Questions

### Q: What is the difference between `--test` and `--test1-only`?

A: `--test=X` allows you to specify which test mode to use, while `--test1-only` is a shortcut that runs only test 1.

### Q: Can I run tests in parallel?

A: Currently, tests run sequentially with brain reset between tests. This is the default behavior and ensures clean test isolation.

### Q: How do I reproduce results?

A: Use the `--seed` option to set a random seed, and ensure the same configuration parameters for reproducible results.

### Q: What if a test fails?

A: The test runner will report the failure and continue with other tests. Check the verbose output for detailed error information.

### Q: Can I add custom tests?

A: Yes, by extending the TestRunner class or creating new test configuration entries in the initializeTestConfigurations method.

## Support

For issues, questions, or suggestions:

1. Check the project documentation
2. Review the command-line help with `--help`
3. Test with verbose mode for detailed output
4. Contact the NLM development team

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

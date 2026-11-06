# NLM Usage Examples

This document provides examples of how to use the enhanced NLM Phase 2 framework with the new improvements.

## 1. Basic Usage

### Running the Simulation

```bash
# Run all tests with default settings
./nlm

# Run only connectivity test
./nlm --test connectivity

# Run only plasticity test
./nlm --test plasticity

# Run only STDP verification test
./nlm --test stdp

# Skip plasticity test
./nlm --skip-test plasticity

# List all available tests
./nlm --list-tests
```

### Configuration Examples

**Basic Configuration File (`config/basic.cfg`):**
```cfg
# Basic NLM Configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
```

**Custom Configuration File (`config/custom.cfg`):**
```cfg
# Custom NLM Configuration for Research
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 1000
region_count = 2
connection_probability = 0.10
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.018
synaptogenesis_rate = 0.002
pruning_rate = 0.0002
log_level = DEBUG
log_to_file = true
log_filename = nlm_research.log
max_simulation_steps = 50000
```

### Command Line Configuration

```bash
# Override config file with command line arguments
./nlm --config config/basic.cfg \
     --neuron_count=200 \
     --simulation_timestep=0.001 \
     --random_seed=99

# Using short form
./nlm -c config/basic.cfg -n 200 -t 0.001 -r 99

# Full configuration override
./nlm \
    --config config/custom.cfg \
    --neuron_count=750 \
    --region_count=3 \
    --connection_probability=0.12 \
    --stdp_ltp_weight=0.025
```

## 2. Test Framework Examples

### Custom Test Configuration

**Custom Test Script (`scripts/run_tests.sh`):**
```bash
#!/bin/bash

# Enhanced test runner script

echo "Starting NLM Enhanced Test Suite"
echo "================================="

# Set up test environment
export NLM_CONFIG_DIR="configs"
export NLM_LOG_LEVEL="INFO"
export NLM_PERFORMANCE_MEASURE="true"

# Function to run a test with statistics
run_test() {
    local test_name=$1
    local extra_args=$2
    
    echo "\n--- Running Test: $test_name ---"
    start_time=$(date +%s.%N)
    
    ./nlm --test $test_name $extra_args \
        --verbose \
        --connectivity-neurons 15 \
        --connectivity-steps 100 \
        --plasticity-steps 500 \
        --stdp-trials 20 \
        --pattern-neurons 10
    
    end_time=$(date +%s.%N)
    duration=$(echo "$end_time - $start_time" | bc)
    
    echo "Test '$test_name' completed in ${duration}s"
}

# Run individual tests
run_test "connectivity" "--verbose"
run_test "plasticity" "--verbose"
run_test "stdp" "--verbose"

# Run all tests
./nlm --verbose

echo "\nAll tests completed successfully!"
echo "See log files in logs/ directory for details."
```

### Performance Monitoring

**Performance Monitoring Script (`scripts/monitor_performance.sh`):**
```bash
#!/bin/bash

# Performance monitoring for NLM

echo "Starting NLM Performance Monitoring"
echo "====================================="

# Start timing
total_start=$(date +%s)

# Run simulation with performance tracking
./nlm --test connectivity --test plasticity --test stdp \
    --verbose \
    > logs/performance_run_$(date +%Y%m%d_%H%M%S).log 2>&1

# Measure total time
total_end=$(date +%s)
total_duration=$((total_end - total_start))

echo "Total execution time: ${total_duration} seconds"

# Analyze log performance
if [ -f "logs/performance_run_$(date +%Y%m%d_%H%M%S).log" ]; then
    echo "\nPerformance Analysis:"
    echo "===================="
    
    # Extract timing information
    grep "P ASSED.*ms" logs/performance_run_*.log | sort -k3,3 | tail -5
    
    # Extract error counts
    grep -c "ERROR" logs/performance_run_*.log | awk -F: '{sum+=$2} END {print "Total errors: " sum}'
    
    # Extract warning counts
    grep -c "WARN" logs/performance_run_*.log | awk -F: '{sum+=$2} END {print "Total warnings: " sum}'
    
    # Calculate performance metrics
    echo "\nPerformance Recommendations:"
    echo "- Consider increasing neuron count for more realistic results"
    echo "- Monitor memory usage during long-running simulations"
    echo "- Use incremental testing for complex scenarios"
fi

# Generate performance report
cat > logs/performance_report_$(date +%Y%m%d_%H%M%S).txt << EOF
NLM Performance Report
=======================
Date: $(date)
Total Duration: ${total_duration}s

Configuration:
- Neurons: 500
- Regions: 1
- Connectivity: 0.15

Performance Metrics:
- Average test execution time: (from logs)
- Memory usage peak: (from logs)
- Error rate: (from logs)
- Success rate: (from logs)

Recommendations:
1. Optimize configuration for faster simulation
2. Monitor system resources
3. Implement parallel processing for large networks
4. Use checkpoint/restart for long experiments
EOF

echo "Performance report saved to logs/"
```

## 3. Advanced Usage

### Custom Logger Configuration

**Custom Logger Setup (`scripts/custom_logger.sh`):**
```bash
#!/bin/bash

# Custom logger configuration

# Create log directory
mkdir -p logs

# Set environment variables
export NLM_LOG_LEVEL="DEBUG"
export NLM_LOG_COLORS="true"
export NLM_LOG_FILE="logs/nlm_$(date +%Y%m%d).log"

# Configure custom log filter
cat > logs/log_filter.conf << EOF
min_level: DEBUG
categories:
  - ALL
enabled_modules:
  - brain
  - sensory
  - motor
excluded_messages:
  - "memory allocation failed"
  - "out of memory"
EOF

# Run with custom configuration
./nlm --config config/custom.cfg \
    --verbose \
    --connectivity-neurons 20 \
    --plasticity-steps 1000
```

### Development and Testing

**Development Environment Setup (`scripts/dev_setup.sh`):**
```bash
#!/bin/bash

# Development environment setup

echo "Setting up NLM Development Environment"
echo "======================================="

# Create development directories
mkdir -p dev/logs
mkdir -p dev/configs
mkdir -p dev/scripts
mkdir -p dev/examples
mkdir -p dev/tests

# Copy development configuration
cp configs/default.cfg dev/configs/dev.cfg

# Create development Python bindings
# (Assuming Python bindings are available)

# Install dependencies (if any)
if [ -f "requirements.txt" ]; then
    pip install -r requirements.txt
fi

# Build with development flags
cd /workspace/app
sudo apt-get update
sudo apt-get install -y build-essential

# Create symbolic links for easier development
ln -sf /workspace/app/src /workspace/app/include
ln -sf /workspace/app/configs /workspace/app/config

# Set up environment variables
cat > dev/.env << EOF
NLM_ENV=development
NLM_LOG_LEVEL=DEBUG
NLM_PERFORMANCE_TRACKING=true
NLM_CHECKPOINT_INTERVAL=100
NLM_SAVE_CHECKPOINTS=true
EOF

# Initialize git if not already done
if [ ! -d .git ]; then
    git init
    git add .
    git commit -m "Initial development setup with enhanced NLM"
fi

echo "Development environment setup complete!"
echo "Next steps:"
echo "1. Edit dev/configs/dev.cfg as needed"
echo "2. Modify src/main.cpp for custom tests"
echo "3. Run ./nlm --test connectivity to verify setup"
echo "4. Check dev/logs/ for detailed output"
```

## 4. Example Scripts

### Example Test Script (`examples/example_test.py`):
```python
#!/usr/bin/env python3

"""
Example script demonstrating NLM test framework usage
"""

import subprocess
import time
import json
import os
from datetime import datetime

class NLMSimulation:
    def __init__(self):
        self.log_file = f"logs/simulation_{datetime.now().strftime('%Y%m%d_%H%M%S')}.log"
        self.config_file = "configs/default.cfg"
        
    def run_test(self, test_name, custom_args=None):
        """Run a specific NLM test with monitoring"""
        
        # Build command
        cmd = ["./nlm", "--test", test_name]
        
        if custom_args:
            for key, value in custom_args.items():
                cmd.extend([f"--{key.replace('_', '-')}", str(value)])
        
        cmd.extend(["--verbose", "--connectivity-neurons", "10", 
                   "--plasticity-steps", "100", "--stdp-trials", "5"])
        
        # Run simulation
        print(f"Running test: {test_name}")
        start_time = time.time()
        
        try:
            result = subprocess.run(cmd, 
                                  capture_output=True, 
                                  text=True,
                                  timeout=300)  # 5 minute timeout
            
            end_time = time.time()
            duration = end_time - start_time
            
            # Save results
            self.save_test_results(test_name, result, duration)
            
            # Print summary
            print(f"Test '{test_name}' completed in {duration:.2f}s")
            if result.returncode == 0:
                print("✓ PASSED")
            else:
                print("✗ FAILED")
                print("STDOUT:", result.stdout)
                print("STDERR:", result.stderr)
                
        except subprocess.TimeoutExpired:
            print(f"Test '{test_name}' timed out!")
            
        except Exception as e:
            print(f"Error running test '{test_name}': {e}")
    
    def save_test_results(self, test_name, result, duration):
        """Save test results to JSON file"""
        
        test_result = {
            "test_name": test_name,
            "duration": duration,
            "return_code": result.returncode,
            "timestamp": datetime.now().isoformat(),
            "stdout": result.stdout,
            "stderr": result.stderr,
            "success": result.returncode == 0
        }
        
        # Create results directory if it doesn't exist
        os.makedirs("logs", exist_ok=True)
        
        # Save to JSON file
        results_file = f"logs/test_results_{test_name}.json"
        
        if os.path.exists(results_file):
            # Append to existing file
            with open(results_file, 'r') as f:
                existing_results = json.load(f)
            
            existing_results.append(test_result)
        else:
            existing_results = [test_result]
        
        with open(results_file, 'w') as f:
            json.dump(existing_results, f, indent=2)
    
    def run_all_tests(self):
        """Run all tests and generate report"""
        
        tests = ["connectivity", "plasticity", "stdp"]
        
        print("Running all NLM tests...")
        print("======================")
        
        for test in tests:
            self.run_test(test)
        
        # Generate summary report
        self.generate_report()
    
    def generate_report(self):
        """Generate test execution report"""
        
        report_file = f"logs/simulation_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.md"
        
        with open(report_file, 'w') as f:
            f.write("# NLM Simulation Report\n")
            f.write(f"Generated: {datetime.now().isoformat()}\n\n")
            
            f.write("## Test Summary\n")
            f.write("```\n")
            
            # Parse all result files and create summary
            for test in ["connectivity", "plasticity", "stdp"]:
                results_file = f"logs/test_results_{test}.json"
                if os.path.exists(results_file):
                    with open(results_file, 'r') as rf:
                        results = json.load(rf)
                        
                        f.write(f"Test: {test}\n")
                        f.write(f"Executions: {len(results)}\n")
                        
                        passed = sum(1 for r in results if r['success'])
                        total = len(results)
                        
                        f.write(f"Passed: {passed}/{total} ({passed/total*100:.1f}%)\n")
                        
                        avg_duration = sum(r['duration'] for r in results) / total
                        f.write(f"Average Duration: {avg_duration:.2f}s\n")
                        f.write("```\n\n")
            
            f.write("\n## Detailed Results\n")
            f.write("See individual test result files in logs/ directory.\n")
        
        print(f"\nReport saved to: {report_file}")

if __name__ == "__main__":
    simulation = NLMSimulation()
    
    # Run all tests
    simulation.run_all_tests()
    
    # Or run specific test
    # simulation.run_test("connectivity", {"neuron_count": 200})
```

### Configuration Builder (`examples/config_builder.py`):
```python
#!/usr/bin/env python3

"""
NLM Configuration Builder
Generates configuration files for different testing scenarios
"""

import json
import os
import random

def generate_configuration(name, params):
    """Generate a configuration file"""
    
    base_config = {
        "random_seed": params.get("random_seed", 42),
        "simulation_timestep": params.get("simulation_timestep", 0.001),
        "neuron_count": params.get("neuron_count", 500),
        "region_count": params.get("region_count", 1),
        "connection_probability": params.get("connection_probability", 0.15),
        "plasticity_learning_rate": params.get("plasticity_learning_rate", 0.01),
        "stdp_ltp_weight": params.get("stdp_ltp_weight", 0.02),
        "stdp_ltd_weight": params.get("stdp_ltd_weight", 0.015),
        "stdp_time_constant": params.get("stdp_time_constant", 20.0),
        "development_synaptogenesis_rate": params.get("development_synaptogenesis_rate", 0.001),
        "development_pruning_rate": params.get("development_pruning_rate", 0.0001),
        "dopamine_baseline": params.get("dopamine_baseline", 0.1),
        "reward_discount_factor": params.get("reward_discount_factor", 0.99),
        "environment_name": params.get("environment_name", "GridWorld"),
        "environment_width": params.get("environment_width", 10),
        "environment_height": params.get("environment_height", 10),
        "log_level": params.get("log_level", "INFO"),
        "log_to_file": params.get("log_to_file", False),
        "log_filename": params.get("log_filename", "nlm.log"),
        "max_simulation_steps": params.get("max_simulation_steps", 10000),
        "simulation_time_limit": params.get("simulation_time_limit", 0.0),
        "visualization_enabled": params.get("visualization_enabled", False),
        "visualization_update_rate": params.get("visualization_update_rate", 30.0)
    }
    
    # Add description
    base_config["description"] = f"Configuration for {name}"
    
    # Create configs directory if it doesn't exist
    os.makedirs("configs", exist_ok=True)
    
    # Save configuration
    config_file = f"configs/{name}.cfg"
    
    with open(config_file, 'w') as f:
        f.write("# " + name + " Configuration File\n")
        f.write("# Generated by Config Builder\n\n")
        
        for key, value in base_config.items():
            if isinstance(value, str):
                f.write(f"{key} = \"{value}\"\n")
            elif isinstance(value, bool):
                f.write(f"{key} = {str(value).lower()}\n")
            else:
                f.write(f"{key} = {value}\n")
    
    print(f"Generated configuration: {config_file}")
    return config_file

def create_research_config():
    """Create configuration for research purposes"""
    
    params = {
        "name": "research",
        "random_seed": 12345,
        "simulation_timestep": 0.0005,
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.08,
        "plasticity_learning_rate": 0.005,
        "stdp_ltp_weight": 0.03,
        "stdp_ltd_weight": 0.018,
        "development_synaptogenesis_rate": 0.002,
        "development_pruning_rate": 0.0002,
        "dopamine_baseline": 0.2,
        "reward_discount_factor": 0.95,
        "log_level": "DEBUG",
        "log_to_file": True,
        "log_filename": "nlm_research_$(date +%Y%m%d).log",
        "max_simulation_steps": 100000
    }
    
    return generate_configuration("research", params)

def create_performance_config():
    """Create configuration for performance testing"""
    
    params = {
        "name": "performance",
        "random_seed": 999,
        "simulation_timestep": 0.002,
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.20,
        "plasticity_learning_rate": 0.02,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "log_level": "INFO",
        "log_to_file": False,
        "max_simulation_steps": 1000
    }
    
    return generate_configuration("performance", params)

def create_quick_test_config():
    """Create configuration for quick testing"""
    
    params = {
        "name": "quick_test",
        "random_seed": 1,
        "simulation_timestep": 0.001,
        "neuron_count": 50,
        "region_count": 1,
        "connection_probability": 0.30,
        "plasticity_learning_rate": 0.05,
        "stdp_ltp_weight": 0.05,
        "stdp_ltd_weight": 0.04,
        "log_level": "WARNING",
        "log_to_file": False,
        "max_simulation_steps": 100
    }
    
    return generate_configuration("quick_test", params)

if __name__ == "__main__":
    print("NLM Configuration Builder")
    print("========================")
    
    # Generate different configuration files
    create_research_config()
    create_performance_config()
    create_quick_test_config()
    
    print("\nGenerated configurations:")
    print("1. configs/research.cfg - For detailed research experiments")
    print("2. configs/performance.cfg - For performance testing")
    print("3. configs/quick_test.cfg - For quick validation")
    
    print("\nUsage:")
    print("  ./nlm --config configs/research.cfg")
    print("  ./nlm --test connectivity --config configs/quick_test.cfg")
```

## 5. Debugging and Troubleshooting

### Common Issues and Solutions

**Issue 1: Configuration File Not Found**

**Problem**: The simulation cannot find the specified configuration file.

**Solution**:
```bash
# Check if the file exists
ls -la configs/default.cfg

# Create a basic config file
./nlm --config configs/default.cfg

# Or use default configuration
./nlm
```

**Issue 2: Test Fails with No Output**

**Problem**: The test runs but produces no visible output.

**Solution**:
```bash
# Run with verbose flag
./nlm --test connectivity --verbose

# Check log files
ls -la logs/

# Set up logging
cat > logs/.env << EOF
NLM_LOG_LEVEL=DEBUG
NLM_LOG_COLORS=true
EOF

./nlm --test connectivity
```

**Issue 3: Memory Usage Too High**

**Problem**: The simulation consumes excessive memory.

**Solution**:
```bash
# Reduce neuron count
./nlm --config configs/performance.cfg

# Reduce simulation steps
./nlm --test connectivity --plasticity-steps 100

# Use resource limits
ulimit -v 2097152  # 2GB
```

**Issue 4: Timeout During Testing**

**Problem**: Tests take too long and timeout.

**Solution**:
```bash
# Reduce test complexity
./nlm --test connectivity --connectivity-steps 50
./nlm --test plasticity --plasticity-steps 100
./nlm --test stdp --stdp-trials 5

# Use quick test configuration
./nlm --config configs/quick_test.cfg --test connectivity
```

## 6. Best Practices

### Configuration Management

1. **Use Version Control**: Keep configuration files in version control
2. **Parameterize**: Use environment variables for sensitive parameters
3. **Document**: Add comments to configuration files
4. **Test**: Validate configurations with unit tests

### Logging Best Practices

1. **Categorize**: Use appropriate log categories for better organization
2. **Filter**: Set appropriate log levels for different environments
3. **Rotate**: Implement log rotation for long-running simulations
4. **Monitor**: Set up log monitoring for production environments

### Testing Best Practices

1. **Incremental Testing**: Test components individually before integration
2. **Parameterized Testing**: Use configuration files to test different scenarios
3. **Performance Testing**: Include performance monitoring in test suites
4. **Error Handling**: Test error conditions and edge cases

### Development Best Practices

1. **Code Reviews**: Regularly review and refactor code
2. **Automated Testing**: Implement continuous integration
3. **Documentation**: Keep documentation up to date
4. **Profiling**: Profile performance regularly

## 7. Quick Start Guide

### For New Users

1. **Install Dependencies**:
```bash
sudo apt-get update
sudo apt-get install build-essential git
```

2. **Run Basic Tests**:
```bash
./nlm --list-tests
./nlm --test connectivity
./nlm --test plasticity
./nlm --test stdp
```

3. **Explore Configuration**:
```bash
cat configs/default.cfg
ls -la configs/
```

### For Developers

1. **Run Tests**:
```bash
# All tests
./nlm

# Specific test
./nlm --test connectivity

# Custom parameters
./nlm --test connectivity --connectivity-neurons 20
```

2. **Check Logs**:
```bash
ls -la logs/
grep "ERROR" logs/*.log | head -10
grep "PASSED" logs/*.log | tail -5
```

3. **Performance Monitoring**:
```bash
./nlm --test connectivity --verbose
./nlm --test plasticity --verbose
./nlm --test stdp --verbose
```

### For Researchers

1. **Research Configuration**:
```bash
./nlm --config configs/research.cfg
```

2. **Performance Testing**:
```bash
./nlm --config configs/performance.cfg
```

3. **Quick Validation**:
```bash
./nlm --config configs/quick_test.cfg
```

## 8. Frequently Asked Questions

### Q: What are the default test parameters?

A: The default test parameters are:
- Connectivity test: 10 neurons, 50 steps
- Plasticity test: 1000 steps, 20 pattern neurons
- STDP test: 50 trials

### Q: How do I run a custom test?

A: Create a custom configuration file and run with the appropriate test options:
```bash
./nlm --test custom_test --config configs/custom.cfg
```

### Q: Can I add custom log categories?

A: Yes, you can add custom log categories by modifying the Logger class and using the category-based logging macros.

### Q: How do I enable file logging?

A: Set `log_to_file = true` in the configuration file or use the `--config` option with a custom configuration.

### Q: What are the system requirements?

A: The simulation requires:
- 2GB RAM minimum (4GB recommended for research simulations)
- Modern CPU (SSE2 or higher)
- Linux, macOS, or Windows with GCC/Clang

### Q: How do I report issues?

A: Report issues by:
1. Checking the logs in the `logs/` directory
2. Submitting an issue with the log files
3. Providing configuration and test details

## 9. Next Steps

### For Further Learning

1. **Study the Configuration System**: Understand how to customize configurations
2. **Explore Logging**: Learn to use advanced logging features
3. **Performance Optimization**: Profile and optimize simulation performance
4. **Research Applications**: Adapt the framework for your research needs

### For Contributions

1. **Code Improvements**: Submit pull requests with enhancements
2. **Documentation**: Improve documentation and examples
3. **Testing**: Add new test cases and scenarios
4. **Feature Requests**: Suggest new features and capabilities

### For Production Use

1. **Configuration Management**: Set up production configurations
2. **Monitoring**: Implement monitoring and alerting
3. **Backup**: Set up regular backups
4. **Scaling**: Plan for scaling to larger networks

---
*Document generated: $(date)*
*Version: 1.0*
*Author: Kilo*

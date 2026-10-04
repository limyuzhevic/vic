# NLM Troubleshooting Guide

## Overview

This document provides comprehensive documentation for NLM troubleshooting. Troubleshooting is a critical aspect of NLM that helps users resolve issues during development, experimentation, and deployment. This guide covers common issues, troubleshooting techniques, and solutions for NLM problems.

## Common Issues

### 1. Installation Issues

#### "Cannot import pynlm"

**Symptoms:**
- Python error: `ImportError: No module named 'pynlm'`
- Cannot import NLM Python bindings
- NLM not installed correctly

**Solutions:**
```bash
# Install NLM from source
pip install -e .

# Check Python installation
python -c "import sys; print(sys.version)"

# Check pip installation
pip list | grep pynlm

# Install dependencies
pip install pybind11 scikit-build-core numpy pytest
```

#### "CMake cannot find Python"

**Symptoms:**
- CMake error during build
- Cannot find Python interpreter
- Build fails with Python-related errors

**Solutions:**
```bash
# Install Python development packages
apt-get update
apt-get install python3-dev python3-pip

# Set Python executable
cmake .. -DPython_EXECUTABLE=$(which python3)

# Or specify Python version explicitly
cmake .. -DPython_EXECUTABLE=/usr/bin/python3.8
```

### 2. Build Issues

#### "Compilation errors"

**Symptoms:**
- C++ compilation errors
- Missing dependencies
- Compiler configuration issues

**Solutions:**
```bash
# Ensure all dependencies are installed
sudo apt-get install build-essential cmake g++ clang libomp-dev

# Check compiler version
g++ --version
clang --version

# Set appropriate CMake flags
cmake .. -DCMAKE_CXX_STANDARD=20 -DCMAKE_BUILD_TYPE=Release

# Rebuild with verbose output
make -j4 VERBOSE=1
```

#### "Makefile not found"

**Symptoms:**
- No Makefile in build directory
- Build directory not created properly
- Configuration issues

**Solutions:**
```bash
# Remove old build directory
rm -rf build

# Create new build directory
mkdir build
cd build

# Run CMake configuration
cmake .. -DCMAKE_BUILD_TYPE=Release

# Rebuild
make -j4
```

### 3. Runtime Issues

#### "Brain not initialized"

**Symptoms:**
- Error: "Brain not initialized"
- Cannot run simulation without initialization
- Null pointer errors

**Solutions:**
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()  # Always call initialize first

# Now you can use the brain
brain.step(0)
```

#### "Simulation time is 0"

**Symptoms:**
- Simulation time remains 0
- World not updating
- Time-related calculations incorrect

**Solutions:**
```python
import pynlm

# Create configuration with time settings
config = pynlm.createDefaultConfig()
config.set("simulation.timestep", 0.1)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

# Update world with timestep
world.update(0.1)

# Get simulation time
simulation_time = world.getSimulationTime()
print(f"Simulation time: {simulation_time}")
```

#### "Neuron count is 0"

**Symptoms:**
- Error: "Neuron count is 0"
- No neurons in brain
- Simulation cannot run

**Solutions:**
```python
import pynlm

# Create configuration with neuron count
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Set neuron count

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Check neuron count
neuron_count = brain.getTotalNeuronCount()
print(f"Neuron count: {neuron_count}")

# Verify neuron count is correct
assert neuron_count > 0, "Neuron count should be greater than 0"
```

### 4. Performance Issues

#### "Simulation is too slow"

**Symptoms:**
- Low steps per second
- High memory usage
- Long simulation times

**Solutions:**
```bash
# Enable performance optimizations
./nlm config set --key=performance.optimization_level --value=3
./nlm config set --key=performance.parallel_processing --value=true
./nlm config set --key=performance.enable_vectorization --value=true

# Reduce simulation complexity
./nlm config set --key=performance.max_steps --value=5000
./nlm config set --key=plasticity.stdp.learning_rate --value=0.0005

# Optimize configuration
./nlm config optimize --target=performance --metric=firing_rate --value=50
```

#### "Memory usage is too high"

**Symptoms:**
- High memory usage (> 2 GB)
- Memory allocation failures
- System slowdown

**Solutions:**
```bash
# Enable memory pooling
./nlm config set --key=performance.memory_pooling --value=true

# Configure memory pool size
./nlm config set --key=performance.memory_pool_size --value=2048

# Reduce simulation complexity
./nlm config set --key=performance.max_steps --value=2000
```

#### "Firing rate is too low"

**Symptoms:**
- Low firing rates (< 10 Hz)
- Neurons not firing
- Simulation appears stuck

**Solutions:**
```bash
# Increase excitability
./nlm config set --key=brain.v_thresh --value=-40.0
./nlm config set --key=brain.initial_weight_mean --value=0.7

# Increase plasticity
./nlm config set --key=performance.optimization_level --value=3
```

### 5. Experiment Issues

#### "Experiment not found"

**Symptoms:**
- Error: "Experiment not found"
- Cannot run specified experiment
- Experiment name is incorrect

**Solutions:**
```python
import pynlm

# List available experiments
experiments = pynlm.list_experiments()
print(f"Available experiments: {experiments}")

# Run available experiment
if experiments:
    result = pynlm.run_experiment(experiments[0])
    print(f"Experiment result: {result}")
```

#### "Experiment results are incomplete"

**Symptoms:**
- Missing data in experiment results
- Incomplete analysis
- Results validation failures

**Solutions:**
```python
import pynlm

# Ensure all required metrics are collected
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable all subsystem monitoring
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run experiment with comprehensive data collection
for step in range(1000):
    brain.step(step)
    
    # Collect all available metrics
    metrics = {
        'firing_rate': brain.getAverageFiringRate(),
        'neuron_utilization': (brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100,
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'prediction_error': agent.getPredictionError()
    }
    
    # Save metrics
    save_metrics(metrics, step)
```

### 6. Configuration Issues

#### "Invalid configuration parameter"

**Symptoms:**
- Error: "Invalid configuration parameter"
- Configuration validation fails
- Unknown configuration key

**Solutions:**
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Use valid configuration keys
valid_keys = config.getKeys()
print(f"Valid configuration keys: {valid_keys}")

# Set configuration using valid keys
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Check if configuration key is valid
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")
    print(f"Neuron count: {value}")
```

#### "Configuration file not found"

**Symptoms:**
- Error: "Configuration file not found"
- Cannot load configuration from file
- File path is incorrect

**Solutions:**
```bash
# Check if configuration file exists
ls -la config.json

# Create configuration file if it doesn't exist
./nlm config create --output=config.json

# Load configuration from file
./nlm config load --file=config.json

# Use absolute path
cd /path/to/config
./nlm config load --file=./config.json
```

### 7. World Issues

#### "World not updating"

**Symptoms:**
- World state is not changing
- Agent not moving
- World simulation stuck

**Solutions:**
```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Set agent position
world.setAgentStart(10.0, 10.0)

# Update world
world.update(0.1)

# Get agent position
agent_body = world.getAgentBody()
print(f"Agent position: {agent_body.getPosition()}")

# Check if world is updated
if agent_body.getPosition() != (10.0, 10.0):
    print("World is updating correctly")
else:
    print("World may be stuck")
```

#### "Agent not responding to commands"

**Symptoms:**
- Agent not moving
- Commands not being executed
- Agent appears unresponsive

**Solutions:**
```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable agent subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Get agent command
motor_cmd = agent.decodeMotorCommand()
print(f"Agent command: {motor_cmd}")

# Check if command is valid
if motor_cmd:
    print("Agent is responding to commands")
else:
    print("Agent may be unresponsive")
```

## Troubleshooting Techniques

### 1. Systematic Troubleshooting

#### Step 1: Identify the Problem

1. **Read error messages carefully**
2. **Check log files**
3. **Reproduce the problem**
4. **Document symptoms**
5. **Gather system information**

#### Step 2: Search for Solutions

1. **Check documentation**
2. **Search online forums**
3. **Consult NLM documentation**
4. **Check GitHub issues**
5. **Ask for help**

#### Step 3: Apply Fixes

1. **Try simple solutions first**
2. **Apply complex solutions**
3. **Test the fix**
4. **Document the fix**
5. **Share the fix**

### 2. Systematic Debugging

#### Python Debugging

```python
import pynlm
import traceback

# Enable debug mode
pynlm.set_debug_mode(True)

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with debugging
for step in range(100):
    try:
        brain.step(step)
        print(f"Step {step} completed")
    except Exception as e:
        print(f"Error at step {step}: {e}")
        print(traceback.format_exc())
        break
```

#### Command Line Debugging

```bash
# Run with verbose output
./nlm --verbose --help

# Run with debug mode
./nlm --debug run --steps=100

# Check system resources
free -h

# Check running processes
ps aux | grep nlm
```

### 3. Systematic Testing

#### Unit Testing

```python
import pynlm
import unittest

class TestNLM(unittest.TestCase):
    def setUp(self):
        # Create configuration
        self.config = pynlm.createDefaultConfig()
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_brain_creation(self):
        # Test brain creation
        self.assertIsNotNone(self.brain)
        self.assertTrue(hasattr(self.brain, 'step'))
    
    def test_brain_initialization(self):
        # Test brain initialization
        self.assertTrue(self.brain.getTotalNeuronCount() > 0)
    
    def test_simulation_step(self):
        # Test simulation step
        initial_spikes = self.brain.getTotalSpikeCount()
        self.brain.step(0)
        final_spikes = self.brain.getTotalSpikeCount()
        
        # Check that brain stepped
        self.assertTrue(final_spikes >= initial_spikes)

if __name__ == '__main__':
    unittest.main()
```

#### Integration Testing

```python
import pynlm

class IntegrationTest:
    def setUp(self):
        # Create configuration
        self.config = pynlm.createDefaultConfig()
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
        self.world.reset()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def test_full_integration(self):
        # Test full integration of all components
        for step in range(100):
            # Update world
            self.world.update(0.1)
            
            # Get percept and process
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get agent command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Check that all components are working
            self.assertIsNotNone(percept)
            self.assertIsNotNone(motor_cmd)
            self.assertTrue(self.brain.getTotalNeuronCount() > 0)
    
    def test_agent_behavior(self):
        # Test agent behavior
        initial_position = self.world.getAgentBody().getPosition()
        
        # Run simulation
        for step in range(50):
            self.world.update(0.1)
            
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            self.brain.step(step)
            
            motor_cmd = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
        
        final_position = self.world.getAgentBody().getPosition()
        
        # Check that agent moved
        self.assertNotEqual(initial_position, final_position)

# Run integration tests
if __name__ == '__main__':
    test = IntegrationTest()
    test.setUp()
    test.test_full_integration()
    test.test_agent_behavior()
    print("All integration tests passed!")
```

## Troubleshooting Scripts

### Python Troubleshooting Scripts

#### Configuration Troubleshooting

```python
import pynlm
import json
from typing import Dict, List, Any

class ConfigurationTroubleshooter:
    def __init__(self):
        self.common_config_issues = {
            'invalid_neuron_count': {
                'symptoms': ['neuron_count_error', 'invalid_parameter'],
                'solutions': [
                    'Set brain.neuron_count to a positive integer',
                    'Use range 100-100000 for neuron count',
                    'Check configuration file format'
                ]
            },
            'missing_plasticity_config': {
                'symptoms': ['plasticity_error', 'learning_rule_error'],
                'solutions': [
                    'Enable plasticity settings',
                    'Set plasticity.stdp.learning_rate',
                    'Check plasticity configuration'
                ]
            }
        }
    
    def check_configuration(self, config):
        """Check configuration for issues."""
        issues = []
        
        # Check neuron count
        if config.get('brain.neuron_count', 0) <= 0:
            issues.append('invalid_neuron_count')
        
        # Check plasticity configuration
        if not config.get('plasticity.stdp.enable', False):
            issues.append('missing_plasticity_config')
        
        return issues
    
    def get_troubleshooting_guide(self, issue):
        """Get troubleshooting guide for configuration issue."""
        if issue in self.common_config_issues:
            return {
                'issue': issue,
                'solutions': self.common_config_issues[issue]['solutions'],
                'description': f"Configuration troubleshooting for {issue}"
            }
        
        return None
    
    def troubleshoot_configuration(self, config):
        """Troubleshoot configuration issues."""
        issues = self.check_configuration(config)
        
        troubleshooting_report = {
            'timestamp': datetime.now().isoformat(),
            'issues_found': len(issues),
            'solutions_provided': 0,
            'issues': [],
            'recommendations': []
        }
        
        for issue in issues:
            guide = self.get_troubleshooting_guide(issue)
            
            if guide:
                troubleshooting_report['issues'].append(guide)
                troubleshooting_report['solutions_provided'] += len(guide['solutions'])
        
        return troubleshooting_report

# Example usage
config_troubleshooter = ConfigurationTroubleshooter()

# Example configuration
config = {
    'brain.neuron_count': 1000,
    'plasticity.stdp.enable': True,
    'performance.optimization_level': 2
}

# Troubleshoot configuration
troubleshooting_report = config_troubleshooter.troubleshoot_configuration(config)

print("Configuration Troubleshooting Report:")
print(f"Issues found: {troubleshooting_report['issues_found']}")
print(f"Solutions provided: {troubleshooting_report['solutions_provided']}")

if troubleshooting_report['issues']:
    print(f"\nIssues ({len(troubleshooting_report['issues'])}):")
    for i, issue in enumerate(troubleshooting_report['issues'], 1):
        print(f"{i}. {issue['issue'].upper()}: {issue['description']}")
        for j, solution in enumerate(issue['solutions'], 1):
            print(f"   {j}. {solution}")
```

#### Performance Troubleshooting

```python
import pynlm
import json
from typing import Dict, List, Any
import numpy as np

class PerformanceTroubleshooter:
    def __init__(self):
        self.common_performance_issues = {
            'low_firing_rate': {
                'symptoms': ['low_firing_rate', 'neurons_not_firing'],
                'solutions': [
                    'Increase neuronal excitability (v_thresh)',
                    'Increase synaptic weight initialization',
                    'Enable higher plasticity rates'
                ]
            },
            'high_memory_usage': {
                'symptoms': ['high_memory_usage', 'memory_allocation_failures'],
                'solutions': [
                    'Enable memory pooling',
                    'Reduce simulation complexity',
                    'Optimize plasticity settings'
                ]
            },
            'slow_simulation': {
                'symptoms': ['low_steps_per_second', 'high_simulation_time'],
                'solutions': [
                    'Enable parallel processing',
                    'Enable vectorization',
                    'Reduce simulation steps',
                    'Optimize configuration'
                ]
            }
        }
    
    def diagnose_performance(self, performance_data):
        """Diagnose performance issues."""
        issues = []
        
        # Check firing rate
        if performance_data.get('firing_rate', 0) < 10:
            issues.append('low_firing_rate')
        
        # Check memory usage
        if performance_data.get('memory_usage', 0) > 2000:
            issues.append('high_memory_usage')
        
        # Check simulation speed
        if performance_data.get('steps_per_second', 0) < 500:
            issues.append('slow_simulation')
        
        return issues
    
    def get_performance_troubleshooting_guide(self, issue):
        """Get troubleshooting guide for performance issue."""
        if issue in self.common_performance_issues:
            return {
                'issue': issue,
                'solutions': self.common_performance_issues[issue]['solutions'],
                'description': f"Performance troubleshooting for {issue}"
            }
        
        return None
    
    def troubleshoot_performance(self, performance_data):
        """Troubleshoot performance issues."""
        issues = self.diagnose_performance(performance_data)
        
        troubleshooting_report = {
            'timestamp': datetime.now().isoformat(),
            'issues_found': len(issues),
            'solutions_provided': 0,
            'issues': [],
            'recommendations': []
        }
        
        for issue in issues:
            guide = self.get_performance_troubleshooting_guide(issue)
            
            if guide:
                troubleshooting_report['issues'].append(guide)
                troubleshooting_report['solutions_provided'] += len(guide['solutions'])
        
        return troubleshooting_report

# Example usage
performance_troubleshooter = PerformanceTroubleshooter()

# Example performance data
performance_data = {
    'firing_rate': 5.0,
    'memory_usage': 3000,
    'steps_per_second': 300
}

# Troubleshoot performance
troubleshooting_report = performance_troubleshooter.troubleshoot_performance(performance_data)

print("Performance Troubleshooting Report:")
print(f"Issues found: {troubleshooting_report['issues_found']}")
print(f"Solutions provided: {troubleshooting_report['solutions_provided']}")

if troubleshooting_report['issues']:
    print(f"\nIssues ({len(troubleshooting_report['issues'])}):")
    for i, issue in enumerate(troubleshooting_report['issues'], 1):
        print(f"{i}. {issue['issue'].upper()}: {issue['description']}")
        for j, solution in enumerate(issue['solutions'], 1):
            print(f"   {j}. {solution}")
```

#### Experiment Troubleshooting

```python
import pynlm
import json
from typing import Dict, List, Any

class ExperimentTroubleshooter:
    def __init__(self):
        self.common_experiment_issues = {
            'experiment_not_found': {
                'symptoms': ['experiment_error', 'invalid_experiment_name'],
                'solutions': [
                    'Check experiment name spelling',
                    'Use available experiment list',
                    'Create experiment if it doesn\'t exist'
                ]
            },
            'incomplete_results': {
                'symptoms': ['missing_data', 'incomplete_analysis'],
                'solutions': [
                    'Enable all metric collection',
                    'Check data saving code',
                    'Validate experiment procedures'
                ]
            },
            'experiment_timeout': {
                'symptoms': ['experiment_timeout', 'long_execution_time'],
                'solutions': [
                    'Increase timeout value',
                    'Optimize experiment configuration',
                    'Reduce experiment complexity'
                ]
            }
        }
    
    def diagnose_experiment(self, experiment_results):
        """Diagnose experiment issues."n        issues = []
        
        # Check for experiment not found
        if 'experiment_not_found' in experiment_results:
            issues.append('experiment_not_found')
        
        # Check for incomplete results
        if experiment_results.get('data_collection_incomplete', False):
            issues.append('incomplete_results')
        
        # Check for timeout
        if experiment_results.get('execution_time', 0) > 300:
            issues.append('experiment_timeout')
        
        return issues
    
    def get_experiment_troubleshooting_guide(self, issue):
        """Get troubleshooting guide for experiment issue."""
        if issue in self.common_experiment_issues:
            return {
                'issue': issue,
                'solutions': self.common_experiment_issues[issue]['solutions'],
                'description': f"Experiment troubleshooting for {issue}"
            }
        
        return None
    
    def troubleshoot_experiment(self, experiment_results):
        """Troubleshoot experiment issues."""
        issues = self.diagnose_experiment(experiment_results)
        
        troubleshooting_report = {
            'timestamp': datetime.now().isoformat(),
            'issues_found': len(issues),
            'solutions_provided': 0,
            'issues': [],
            'recommendations': []
        }
        
        for issue in issues:
            guide = self.get_experiment_troubleshooting_guide(issue)
            
            if guide:
                troubleshooting_report['issues'].append(guide)
                troubleshooting_report['solutions_provided'] += len(guide['solutions'])
        
        return troubleshooting_report

# Example usage
experiment_troubleshooter = ExperimentTroubleshooter()

# Example experiment results
experiment_results = {
    'data_collection_incomplete': True,
    'execution_time': 400
}

# Troubleshoot experiment
troubleshooting_report = experiment_troubleshooter.troubleshoot_experiment(experiment_results)

print("Experiment Troubleshooting Report:")
print(f"Issues found: {troubleshooting_report['issues_found']}")
print(f"Solutions provided: {troubleshooting_report['solutions_provided']}")

if troubleshooting_report['issues']:
    print(f"\nIssues ({len(troubleshooting_report['issues'])}):")
    for i, issue in enumerate(troubleshooting_report['issues'], 1):
        print(f"{i}. {issue['issue'].upper()}: {issue['description']}")
        for j, solution in enumerate(issue['solutions'], 1):
            print(f"   {j}. {solution}")
```

### Shell Script Troubleshooting

```bash
#!/bin/bash

# NLM Troubleshooting Script

# Configuration
NLM_EXEC="./nlm"
PYTHON_SCRIPT="troubleshoot.py"

# Create troubleshooting script
python3 << EOF
import json
import datetime
from typing import Dict, List, Any

class NLMTroubleshooter:
    def __init__(self):
        self.common_issues = {
            'installation_error': {
                'symptoms': ['import_error', 'module_not_found', 'pip_install_error'],
                'solutions': [
                    'pip install -e .',
                    'pip install pybind11 scikit-build-core numpy pytest',
                    'Check Python installation',
                    'Check pip installation'
                ]
            },
            'build_error': {
                'symptoms': ['cmake_error', 'compilation_error', 'make_error'],
                'solutions': [
                    'sudo apt-get install build-essential cmake g++ clang libomp-dev',
                    'cmake .. -DCMAKE_CXX_STANDARD=20 -DCMAKE_BUILD_TYPE=Release',
                    'make -j4',
                    'Check compiler version'
                ]
            },
            'runtime_error': {
                'symptoms': ['runtime_error', 'segmentation_fault', 'null_pointer_error'],
                'solutions': [
                    'Check code for null pointer access',
                    'Add error handling',
                    'Use try-catch blocks',
                    'Check initialization order'
                ]
            }
        }
    
    def diagnose_issue(self, error_message):
        """Diagnose issue based on error message."""
        issues = []
        
        error_lower = error_message.lower()
        
        # Check for installation issues
        if any(keyword in error_lower for keyword in ['importerror', 'modulenotfound', 'pipinstallerror']):
            issues.append('installation_error')
        
        # Check for build issues
        if any(keyword in error_lower for keyword in ['cmakeerror', 'compilationerror', 'makeerror']):
            issues.append('build_error')
        
        # Check for runtime issues
        if any(keyword in error_lower for keyword in ['runtimeerror', 'segmentationfault', 'nullpointererror']):
            issues.append('runtime_error')
        
        return issues
    
    def get_troubleshooting_guide(self, issue):
        """Get troubleshooting guide for issue."""
        if issue in self.common_issues:
            return {
                'issue': issue,
                'solutions': self.common_issues[issue]['solutions'],
                'description': f"Troubleshooting guide for {issue}"
            }
        
        return None
    
    def troubleshoot(self, error_message):
        """Troubleshoot based on error message."""
        issues = self.diagnose_issue(error_message)
        
        troubleshooting_report = {
            'timestamp': datetime.datetime.now().isoformat(),
            'error_message': error_message,
            'issues_found': len(issues),
            'solutions_provided': 0,
            'issues': [],
            'recommendations': []
        }
        
        for issue in issues:
            guide = self.get_troubleshooting_guide(issue)
            
            if guide:
                troubleshooting_report['issues'].append(guide)
                troubleshooting_report['solutions_provided'] += len(guide['solutions'])
        
        return troubleshooting_report

# Example error message
error_message = "ImportError: No module named 'pynlm'"

# Troubleshoot
troubleshooter = NLMTroubleshooter()
troubleshooting_report = troubleshooter.troubleshoot(error_message)

print("Troubleshooting Report:")
print(f"Error message: {troubleshooting_report['error_message']}")
print(f"Issues found: {troubleshooting_report['issues_found']}")
print(f"Solutions provided: {troubleshooting_report['solutions_provided']}")

if troubleshooting_report['issues']:
    print(f"\nIssues ({len(troubleshooting_report['issues'])}):")
    for i, issue in enumerate(troubleshooting_report['issues'], 1):
        print(f"{i}. {issue['issue'].upper()}: {issue['description']}")
        for j, solution in enumerate(issue['solutions'], 1):
            print(f"   {j}. {solution}")
EOF

echo "Troubleshooting completed!"
```

## Advanced Troubleshooting

### 1. Debugging with External Tools

#### Using GDB

```bash
# Run NLM with GDB for debugging
gdb ./nlm

# Set breakpoint at main function
break main

# Run simulation
run --steps=100

# Examine variables
info locals

# Continue execution
continue
```

#### Using Valgrind

```bash
# Run NLM with Valgrind for memory debugging
valgrind --tool=memcheck ./nlm --steps=100

# Check for memory leaks
valgrind --leak-check=full ./nlm --steps=100
```

### 2. Performance Debugging

#### Using Performance Tools

```bash
# Use perf for performance analysis
perf record -g ./nlm --steps=100

# Analyze performance data
perf report

# Use OProfile for profiling
opcontrol --setup
opcontrol --start-prof
./nlm --steps=100
opcontrol --stop-prof
opcontrol --generate
```

### 3. Memory Debugging

#### Using AddressSanitizer

```bash
# Compile with AddressSanitizer
cd build
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address"
make -j4

# Run with AddressSanitizer
./nlm --steps=100
```

## Troubleshooting Checklist

### Issue Identification Checklist

- [ ] Read error messages carefully
- [ ] Check log files
- [ ] Reproduce the problem
- [ ] Document symptoms
- [ ] Gather system information
- [ ] Search for known issues
- [ ] Check documentation

### Solution Application Checklist

- [ ] Try simple solutions first
- [ ] Apply complex solutions
- [ ] Test the fix
- [ ] Document the fix
- [ ] Share the fix
- [ ] Monitor for new issues
- [ ] Update documentation

### Debugging Checklist

- [ ] Use appropriate debugging tools
- [ ] Set appropriate breakpoints
- [ ] Examine variables
- [ ] Check memory usage
- [ ] Monitor performance
- [ ] Test with different inputs
- [ ] Validate results

### Testing Checklist

- [ ] Reproduce the problem
- [ ] Test with different configurations
- [ ] Test with different inputs
- [ ] Test with different environments
- [ ] Test with different versions
- [ ] Validate fixes
- [ ] Document test results

## Troubleshooting Support and Resources

### Getting Help

- **Documentation**: `docs/TROUBLESHOOTING_GUIDE.md`
- **API Reference**: `docs/API_REFERENCE.md`
- **Command Line Guide**: `docs/COMMAND_LINE_GUIDE.md`
- **Configuration Guide**: `docs/CONFIGURATION_GUIDE.md`
- **Performance Guide**: `docs/PERFORMANCE_GUIDE.md`

### Additional Resources

- **Troubleshooting Scripts**: `scripts/` directory
- **Troubleshooting Examples**: `examples/` directory
- **Troubleshooting Tests**: `tests/` directory
- **Troubleshooting Documentation**: `docs/` directory

## Troubleshooting Best Practices

### Debugging Best Practices

1. **Start simple**: Begin with simple solutions
2. **Use systematic approach**: Use systematic debugging approach
3. **Document everything**: Document all debugging steps
4. **Test thoroughly**: Test all solutions
5. **Monitor results**: Monitor results continuously
6. **Share knowledge**: Share debugging knowledge

### Solution Best Practices

1. **Try minimal fix**: Try minimal fix first
2. **Test fix**: Test fix thoroughly
3. **Document fix**: Document fix details
4. **Share fix**: Share fix with others
5. **Monitor fix**: Monitor fix effectiveness

### Community Best Practices

1. **Be respectful**: Be respectful when asking for help
2. **Provide information**: Provide all relevant information
3. **Be patient**: Be patient when waiting for responses
4. **Share knowledge**: Share knowledge with others
5. **Help others**: Help others when possible

## Troubleshooting Workflow

### Phase 1: Issue Identification

1. **Identify the problem**: Read error messages and symptoms
2. **Gather information**: Collect all relevant information
3. **Reproduce the problem**: Reproduce the problem
4. **Document the problem**: Document the problem
5. **Search for solutions**: Search for known solutions

### Phase 2: Solution Search

1. **Check documentation**: Check NLM documentation
2. **Search online**: Search online for solutions
3. **Consult experts**: Consult NLM experts
4. **Try simple solutions**: Try simple solutions
5. **Apply complex solutions**: Apply complex solutions

### Phase 3: Solution Application

1. **Test solution**: Test solution thoroughly
2. **Monitor results**: Monitor results continuously
3. **Document solution**: Document solution details
4. **Share solution**: Share solution with others
5. **Validate solution**: Validate solution effectiveness

### Phase 4: Ongoing Maintenance

1. **Monitor for issues**: Monitor for new issues
2. **Update documentation**: Update documentation
3. **Share knowledge**: Share knowledge with others
4. **Help others**: Help others when possible
5. **Continuous improvement**: Continuously improve debugging process

## Conclusion

NLM troubleshooting is a critical aspect of NLM that helps users resolve issues during development, experimentation, and deployment. This guide provides comprehensive documentation for NLM troubleshooting, including common issues, troubleshooting techniques, and solutions.

Effective troubleshooting is essential for:

1. **Development and debugging**: Resolve issues during NLM development
2. **Experimentation and testing**: Resolve issues during NLM experimentation
3. **Deployment and production**: Resolve issues during NLM deployment
4. **Support and assistance**: Assist users with NLM issues
5. **Community engagement**: Engage with NLM community

This guide provides the knowledge and tools needed to effectively troubleshoot NLM issues. For more advanced troubleshooting tasks, refer to the API reference, command-line guide, and other documentation files.
# NLM Error Handling and Troubleshooting Guide
# ==============================================

This document provides comprehensive guidance on error handling, troubleshooting, and debugging techniques for the NLM (Neural Learning Machine) system. It covers common issues, diagnostic approaches, and recovery strategies.

## 1. Error Classification and Severity Levels

### Error Types

NLM errors can be categorized into several types:

#### Configuration Errors
- **Description**: Invalid or missing configuration parameters
- **Severity**: High (prevents system initialization)
- **Examples**: Invalid neuron counts, missing environment settings, invalid plasticity parameters

#### Initialization Errors
- **Description**: Failures during brain or world initialization
- **Severity**: High (prevents simulation start)
- **Examples**: Memory allocation failures, invalid initial states, corruption in configuration

#### Runtime Errors
- **Description**: Failures during simulation execution
- **Severity**: Medium (can interrupt simulation but often recoverable)
- **Examples**: Numerical instabilities, memory exhaustion, simulation drift

#### Performance Errors
- **Description**: Performance degradation or bottlenecks
- **Severity**: Medium (affects efficiency but doesn't prevent operation)
- **Examples**: High memory usage, slow simulation speeds, CPU overload

#### Recovery Errors
- **Description**: Failures in error recovery and fault tolerance
- **Severity**: High (prevents system from recovering)
- **Examples**: Checkpoint corruption, recovery strategy failures

### Severity Levels

| Level | Description | Actions Required |
|-------|-------------|------------------|
| **Critical** | System cannot function, requires immediate intervention | Restart, configuration fix, rollback |
| **High** | Functionality severely impaired, may cause data loss | Immediate fix, configuration adjustment |
| **Medium** | Performance degraded, non-essential functionality affected | Optimization, parameter tuning |
| **Low** | Minor issues, no functional impact | Logging, optional fixes |

## 2. Common Error Patterns and Solutions

### Configuration-Related Errors

#### Error Pattern 1: Invalid Parameter Values

**Description**: Configuration parameters contain invalid values (negative neuron counts, out-of-range settings).

**Symptoms**:
- "ConfigurationError: Parameter 'brain.neuron_count' must be positive"
- Simulation fails to initialize with "Invalid configuration"
- Error messages showing validation failures

**Root Causes**:
1. Manual editing of configuration files with typos
2. Parameter validation bypassed
3. Configuration inheritance issues
4. Incompatible parameter combinations

**Solutions**:
1. **Immediate Fix**: Use valid configuration templates (default.cfg, high_perf.cfg)
2. **Prevention**: Implement strict parameter validation
3. **Verification**: Check configuration files with validation scripts

**Example Recovery**:
```python
import pynlm

def recover_from_config_error():
    """Recover from configuration error with validated parameters."""
    try:
        # Attempt to load configuration with validation
        config = pynlm.createDefaultConfig()
        
        # Validate critical parameters
        neuron_count = config.get("brain.neuron_count", 1000)
        if neuron_count <= 0:
            config.set("brain.neuron_count", 1000)  # Reset to valid value
        
        connection_prob = config.get("brain.synapse_density", 0.1)
        if connection_prob < 0 or connection_prob > 1:
            config.set("brain.synapse_density", 0.1)  # Reset to valid range
        
        # Create brain with validated configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return brain
        
    except Exception as e:
        print(f"Recovery failed: {e}")
        # Fall back to default configuration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
```

#### Error Pattern 2: Missing Configuration Files

**Description**: Required configuration files are missing or inaccessible.

**Symptoms**:
- "FileNotFoundError: Configuration file 'configs/default.cfg' not found"
- "Error: Cannot locate configuration file for brain initialization"
- Error messages showing file paths that don't exist

**Root Causes**:
1. File system permissions issues
2. Incorrect file paths in code
3. Configuration files moved or deleted
4. Zip/tar extraction issues

**Solutions**:
1. **Immediate Fix**: Copy configuration from templates
2. **Prevention**: Include configuration files in distribution packages
3. **Verification**: Check file existence and permissions

**Example Recovery**:
```python
import os
import shutil

def recover_from_missing_config():
    """Recover from missing configuration file."""
    config_paths = [
        "configs/default.cfg",
        "/usr/local/share/nlm/configs/default.cfg",
        os.path.expanduser("~/.config/nlm/default.cfg")
    ]
    
    for config_path in config_paths:
        if os.path.exists(config_path):
            print(f"Found configuration at {config_path}")
            # Copy to current directory if needed
            if not os.path.exists("configs/default.cfg"):
                os.makedirs("configs", exist_ok=True)
                shutil.copy2(config_path, "configs/default.cfg")
            return config_path
    
    # Create minimal configuration if none found
    print("Creating minimal configuration")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("environment.width", 10)
    config.set("environment.height", 10)
    
    # Save configuration
    os.makedirs("configs", exist_ok=True)
    config.save("configs/default.cfg")
    
    return "configs/default.cfg"
```

### Simulation-Related Errors

#### Error Pattern 3: Numerical Instabilities

**Description**: Simulation produces numerical values that are out of expected ranges or cause computation failures.

**Symptoms**:
- "SimulationError: Numerical instability detected"
- Sudden drops in performance metrics
- "Warning: Neuron potential exceeded valid range"
- NaN or infinite values in simulation outputs

**Root Causes**:
1. Unstable integration time steps
2. High neuron firing rates causing overflow
3. Uncontrolled synaptic plasticity
4. Resource exhaustion

**Solutions**:
1. **Immediate Fix**: Reduce simulation complexity
2. **Prevention**: Implement numerical safeguards
3. **Optimization**: Use optimized configurations

**Example Recovery**:
```python
import pynlm
import numpy as np

def recover_from_numerical_instability():
    """Recover from numerical instability with adaptive parameters."""
    print("Attempting recovery from numerical instability...")
    
    # Try progressively more stable configurations
    configurations = [
        {"brain.neuron_count": 500, "simulation_timestep": 0.001},
        {"brain.neuron_count": 300, "simulation_timestep": 0.002},
        {"brain.neuron_count": 200, "simulation_timestep": 0.005}
    ]
    
    for params in configurations:
        try:
            config = pynlm.createDefaultConfig()
            
            # Apply parameters with validation
            for key, value in params.items():
                config.set(key, value)
            
            # Create brain and test stability
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Quick stability test
            for step in range(50):
                brain.step(step)
                firing_rate = brain.getAverageFiringRate()
                
                # Check for instabilities
                if np.isnan(firing_rate) or firing_rate > 1000:
                    continue  # Try next configuration
                
                print(f"Successfully recovered with configuration: {params}")
                return brain
                
        except Exception as e:
            print(f"Configuration {params} failed: {e}")
            continue
    
    # Ultimate fallback: minimal configuration
    print("Using minimal configuration as last resort")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 100)
    config.set("simulation_timestep", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain
```

#### Error Pattern 4: Memory Exhaustion

**Description**: System runs out of memory during simulation execution.

**Symptoms**:
- "MemoryError: Cannot allocate memory for neural network"
- "Warning: High memory usage detected"
- Simulation slows down significantly
- System swaps memory frequently

**Root Causes**:
1. Large brain configurations with many neurons
2. Memory leaks in simulation code
3. Inefficient memory management
4. Resource constraints in deployment environment

**Solutions**:
1. **Immediate Fix**: Use lightweight configurations
2. **Prevention**: Monitor and optimize memory usage
3. **Optimization**: Use memory-efficient algorithms

**Example Recovery**:
```python
import psutil
import pynlm

def recover_from_memory_exhaustion():
    """Recover from memory exhaustion with resource management."""
    print("Attempting recovery from memory exhaustion...")
    
    # Monitor current memory usage
    current_memory = psutil.virtual_memory().used / 1024 / 1024  # MB
    available_memory = psutil.virtual_memory().available / 1024 / 1024  # MB
    
    print(f"Current memory usage: {current_memory:.1f} MB")
    print(f"Available memory: {available_memory:.1f} MB")
    
    # Try progressively smaller configurations
    configurations = [
        {"brain.neuron_count": 200, "simulation_timestep": 0.001},  # Small
        {"brain.neuron_count": 500, "simulation_timestep": 0.002},  # Medium
        {"brain.neuron_count": 1000, "simulation_timestep": 0.005}  # Large
    ]
    
    for params in configurations:
        try:
            # Check memory before attempting
            if available_memory < 100:  # Need at least 100MB free
                print("Insufficient memory available")
                break
            
            config = pynlm.createDefaultConfig()
            
            # Apply parameters
            for key, value in params.items():
                config.set(key, value)
            
            # Create brain with monitoring
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Test memory usage
            test_memory = psutil.virtual_memory().used / 1024 / 1024
            memory_increase = test_memory - current_memory
            
            if memory_increase < 200:  # Less than 200MB increase
                print(f"Successfully recovered with configuration: {params}")
                print(f"Memory increase: {memory_increase:.1f} MB")
                return brain
                
        except MemoryError:
            print(f"Memory error with configuration: {params}")
            continue
        except Exception as e:
            print(f"Error with configuration {params}: {e}")
            continue
    
    # Fallback: use compressed representation
    print("Using compressed representation as last resort")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 50)
    config.set("simulation_timestep", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain
```

#### Error Pattern 5: Simulation Drift

**Description**: Simulation loses coherence or diverges from expected behavior over time.

**Symptoms**:
- "Warning: Simulation drift detected"
- Gradual degradation of performance metrics
- Neural activity becomes unstable
- Results diverge from expected behavior

**Root Causes**:
1. Unstable plasticity rules
2. Resource accumulation without decay
3. Environmental feedback loops
4. Numerical precision issues

**Solutions**:
1. **Immediate Fix**: Reset simulation state
2. **Prevention**: Implement stability mechanisms
3. **Optimization**: Use stable integration methods

**Example Recovery**:
```python
import pynlm
import time

def recover_from_simulation_drift():
    """Recover from simulation drift with state reset and stabilization."""
    print("Attempting recovery from simulation drift...")
    
    # Try state reset
    try:
        # Reset world to known good state
        config = pynlm.createDefaultConfig()
        
        # Use smaller, more stable configuration
        config.set("brain.neuron_count", 300)
        config.set("plasticity.stdp.learning_rate", 0.005)
        config.set("plasticity.stdp.ltp_weight", 0.005)
        config.set("plasticity.stdp.ltd_weight", 0.006)
        config.set("development.initial_plasticity", 0.5)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test for drift
        initial_firing = brain.getAverageFiringRate()
        stable_count = 0
        
        for step in range(100):
            brain.step(step)
            
            # Check stability
            current_firing = brain.getAverageFiringRate()
            if abs(current_firing - initial_firing) < 0.1 * initial_firing:
                stable_count += 1
            else:
                stable_count = 0
            
            # Reset if too unstable
            if stable_count < 10:
                brain = pynlm.createBrain(config)
                brain.initialize()
        
        print("Successfully recovered using stable configuration")
        return brain
        
    except Exception as e:
        print(f"State reset failed: {e}")
        return None
    
    # If state reset fails, try developmental reset
    print("Attempting developmental reset...")
    return attempt_developmental_reset()
```

### Performance-Related Errors

#### Error Pattern 6: Slow Simulation Performance

**Description**: Simulation runs slower than expected, impacting productivity.

**Symptoms**:
- "PerformanceWarning: Simulation too slow"
- High response times for user interactions
- Incomplete simulation results due to timeouts
- Resource utilization metrics showing bottlenecks

**Root Causes**:
1. Large brain configurations
2. Inefficient algorithm implementations
3. Resource contention
4. Suboptimal configuration choices

**Solutions**:
1. **Immediate Fix**: Use performance-optimized configurations
2. **Prevention**: Implement performance monitoring
3. **Optimization**: Profile and optimize critical paths

**Example Recovery**:
```python
import pynlm
import time
import psutil

def recover_from_slow_performance():
    """Recover from slow performance with optimization recommendations."""
    print("Analyzing performance bottlenecks...")
    
    # Monitor current performance
    start_time = time.time()
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Measure baseline performance
    steps = 100
    for step in range(steps):
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    steps_per_second = steps / elapsed
    
    print(f"Current performance: {steps_per_second:.1f} steps/second")
    
    # Check system resources
    cpu_usage = psutil.cpu_percent()
    memory_usage = psutil.virtual_memory().used / 1024 / 1024
    
    print(f"CPU usage: {cpu_usage}%")
    print(f"Memory usage: {memory_usage:.1f} MB")
    
    # Determine optimization strategy
    if steps_per_second < 50:
        print("Performance too slow - implementing optimization strategy")
        
        # Try performance-optimized configuration
        return try_performance_optimized_config()
    elif cpu_usage > 80:
        print("CPU bottleneck detected - consider lighter configuration")
        return try_lighter_config()
    elif memory_usage > 1024:
        print("Memory usage too high - consider memory-efficient configuration")
        return try_memory_efficient_config()
    else:
        print("Performance acceptable - no optimization needed")
        return brain
```

## 3. Diagnostic Tools and Techniques

### Basic Diagnostics

#### Error Code Reference

| Error Code | Description | Likely Cause | Solution |
|------------|-------------|--------------|----------|
| CONFIG_ERR | Configuration error | Invalid configuration | Check and fix configuration files |
| INIT_ERR | Initialization error | Brain/world setup failure | Verify configuration, check system resources |
| SIM_ERR | Simulation error | Runtime failure during execution | Check numerical stability, reduce complexity |
| MEM_ERR | Memory error | Resource exhaustion | Use lighter configuration, optimize memory usage |
| PERF_ERR | Performance error | Slow execution | Use optimized configurations, check system resources |
| PLUGIN_ERR | Plugin error | Extension failure | Check plugin installation, verify dependencies |

#### Diagnostic Scripts

**Brain Health Check**:
```python
import pynlm
import json

def diagnose_brain_health(brain, steps=100):
    """Comprehensive brain health diagnostics."""
    print("=== Brain Health Diagnostics ===")
    
    health_report = {
        'timestamp': time.time(),
        'brain_info': {},
        'stability_metrics': {},
        'warnings': [],
        'errors': []
    }
    
    try:
        # Basic brain info
        health_report['brain_info'] = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'development_stage': brain.getDevelopmentalStage(),
            'plasticity_rate': brain.getSynapticPlasticityRate()
        }
        
        # Run stability tests
        health_report['stability_metrics'] = run_stability_tests(brain, steps)
        
        # Check for warnings and errors
        health_report['warnings'] = identify_warnings(health_report['brain_info'])
        health_report['errors'] = identify_errors(health_report['brain_info'])
        
    except Exception as e:
        health_report['errors'].append(f"Diagnostic failed: {e}")
    
    # Save diagnostic report
    with open('brain_health_report.json', 'w') as f:
        json.dump(health_report, f, indent=2)
    
    # Print summary
    print(f"Brain health report saved to brain_health_report.json")
    print(f"Warnings: {len(health_report['warnings'])}")
    print(f"Errors: {len(health_report['errors'])}")
    
    return health_report

def run_stability_tests(brain, steps):
    """Run stability tests on brain."""
    metrics = {}
    
    # Firing rate stability
    firing_rates = []
    for step in range(steps):
        brain.step(step)
        firing_rates.append(brain.getAverageFiringRate())
    
    if len(firing_rates) > 1:
        cv = np.std(firing_rates) / np.mean(firing_rates)
        metrics['firing_rate_cv'] = cv
        metrics['firing_rate_stable'] = cv < 0.5
    
    # Neuron count stability
    neuron_counts = [brain.getTotalNeuronCount() for _ in range(min(10, steps // 10))]
    if len(neuron_counts) > 1:
        metrics['neuron_count_var'] = np.var(neuron_counts)
    
    return metrics

def identify_warnings(brain_info):
    """Identify warnings based on brain information."""
    warnings = []
    
    if brain_info['firing_rate'] > 1000:
        warnings.append(f"High firing rate: {brain_info['firing_rate']:.1f} Hz")
    
    if brain_info['neuron_count'] > 5000:
        warnings.append(f"Large brain size: {brain_info['neuron_count']} neurons")
    
    if brain_info['plasticity_rate'] > 1.0:
        warnings.append(f"High plasticity rate: {brain_info['plasticity_rate']:.3f}")
    
    if brain_info['firing_neuron_count'] / brain_info['neuron_count'] > 0.8:
        warnings.append(f"High firing neuron ratio: {brain_info['firing_neuron_count']/brain_info['neuron_count']:.1%}")
    
    return warnings

def identify_errors(brain_info):
    """Identify errors based on brain information."""
    errors = []
    
    if brain_info['firing_rate'] < 0 or brain_info['firing_rate'] > 10000:
        errors.append(f"Invalid firing rate: {brain_info['firing_rate']}")
    
    if brain_info['neuron_count'] <= 0:
        errors.append(f"Invalid neuron count: {brain_info['neuron_count']}")
    
    if brain_info['plasticity_rate'] < 0:
        errors.append(f"Invalid plasticity rate: {brain_info['plasticity_rate']}")
    
    return errors
```

**Configuration Validation**:
```python
import pynlm
import yaml

def validate_configuration(config_file):
    """Validate configuration file for errors."""
    print(f"Validating configuration: {config_file}")
    
    try:
        # Load configuration
        config = pynlm.createDefaultConfig()
        
        # Validate critical parameters
        validation_results = {
            'valid': True,
            'errors': [],
            'warnings': []
        }
        
        # Check neuron count
        neuron_count = config.get("brain.neuron_count", 0)
        if neuron_count <= 0:
            validation_results['errors'].append("Neuron count must be positive")
        elif neuron_count > 10000:
            validation_results['warnings'].append("Neuron count is very large - may cause performance issues")
        
        # Check synapse density
        synapse_density = config.get("brain.synapse_density", 0)
        if synapse_density < 0 or synapse_density > 1:
            validation_results['errors'].append("Synapse density must be between 0 and 1")
        
        # Check plasticity parameters
        stdp_rate = config.get("plasticity.stdp.learning_rate", 0)
        if stdp_rate < 0:
            validation_results['errors'].append("STDP learning rate must be non-negative")
        elif stdp_rate > 1:
            validation_results['warnings'].append("STDP learning rate is very high - may cause instability")
        
        # Check environment settings
        width = config.get("environment.width", 0)
        height = config.get("environment.height", 0)
        if width <= 0 or height <= 0:
            validation_results['errors'].append("Environment dimensions must be positive")
        
        # Overall validation
        if validation_results['errors']:
            validation_results['valid'] = False
        
        print(f"Configuration validation: {'PASSED' if validation_results['valid'] else 'FAILED'}")
        print(f"Errors: {len(validation_results['errors'])}")
        print(f"Warnings: {len(validation_results['warnings'])}")
        
        for error in validation_results['errors']:
            print(f"  ERROR: {error}")
        
        for warning in validation_results['warnings']:
            print(f"  WARNING: {warning}")
        
        return validation_results
        
    except Exception as e:
        print(f"Configuration validation failed: {e}")
        return {
            'valid': False,
            'errors': [f"Validation failed: {e}"],
            'warnings': []
        }
```

### Advanced Diagnostics

#### Performance Profiling

```python
import pynlm
import cProfile
import pstats
import io
from memory_profiler import profile

def profile_simulation(config, steps=100):
    """Profile simulation performance."""
    print(f"Profiling simulation with {steps} steps...")
    
    # CPU profiling
    pr = cProfile.Profile()
    pr.enable()
    
    # Create and run simulation
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    pr.disable()
    
    # Generate CPU profile report
    s = io.StringIO()
    ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
    ps.print_stats()
    
    print("=== CPU Profile ===")
    print(s.getvalue())
    
    # Memory profiling
    print("=== Memory Profile ===")
    
    @profile
    def run_memory_profiling():
        for step in range(steps):
            brain.step(step)
    
    run_memory_profiling()
    
    return brain
```

#### Error Pattern Analysis

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

def analyze_error_patterns(brain_history):
    """Analyze error patterns in brain simulation history."""
    print("=== Error Pattern Analysis ===")
    
    if not brain_history:
        print("No brain history available")
        return
    
    # Extract metrics
    steps = []
    firing_rates = []
    neuron_counts = []
    synapse_counts = []
    
    for entry in brain_history:
        steps.append(entry['step'])
        firing_rates.append(entry['firing_rate'])
        neuron_counts.append(entry['neuron_count'])
        synapse_counts.append(entry['synapse_count'])
    
    # Create analysis plots
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Firing rate over time
    axes[0, 0].plot(steps, firing_rates)
    axes[0, 0].set_title('Firing Rate Over Time')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Firing Rate (Hz)')
    axes[0, 0].grid(True)
    
    # Neuron count over time
    axes[0, 1].plot(steps, neuron_counts)
    axes[0, 1].set_title('Neuron Count Over Time')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Neuron Count')
    axes[0, 1].grid(True)
    
    # Synapse count over time
    axes[1, 0].plot(steps, synapse_counts)
    axes[1, 0].set_title('Synapse Count Over Time')
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('Synapse Count')
    axes[1, 0].grid(True)
    
    # Firing rate stability (rolling standard deviation)
    if len(firing_rates) > 10:
        rolling_std = np.std(firing_rates[-10:])
        axes[1, 1].bar(['Firing Rate'], [rolling_std])
        axes[1, 1].set_title('Firing Rate Stability (Last 10 Steps)')
        axes[1, 1].set_ylabel('Standard Deviation (Hz)')
        axes[1, 1].grid(True)
    
    plt.tight_layout()
    plt.savefig('error_pattern_analysis.png')
    plt.close()
    
    print("Error pattern analysis saved to error_pattern_analysis.png")
    
    # Statistical analysis
    analysis = {
        'firing_rate_stats': {
            'mean': np.mean(firing_rates),
            'std': np.std(firing_rates),
            'min': np.min(firing_rates),
            'max': np.max(firing_rates)
        },
        'neuron_count_stats': {
            'mean': np.mean(neuron_counts),
            'std': np.std(neuron_counts),
            'min': np.min(neuron_counts),
            'max': np.max(neuron_counts)
        },
        'synapse_count_stats': {
            'mean': np.mean(synapse_counts),
            'std': np.std(synapse_counts),
            'min': np.min(synapse_counts),
            'max': np.max(synapse_counts)
        }
    }
    
    return analysis
```

## 4. Recovery Strategies

### Automated Recovery

#### Recovery Action Types

1. **Configuration Recovery**
   - Automatically fix invalid configuration parameters
   - Fall back to default or backup configurations
   - Validate configuration before application

2. **State Recovery**
   - Restore from checkpoints
   - Implement rollback mechanisms
   - Use developmental reset when appropriate

3. **Parameter Recovery**
   - Adjust plasticity parameters
   - Modify simulation settings
   - Optimize resource allocation

4. **Architecture Recovery**
   - Recreate neural networks with different configurations
   - Apply architectural changes
   - Modify connection patterns

#### Recovery Workflow

```python
import pynlm
import time
import json
from typing import Dict, List, Optional

class RecoveryManager:
    """Manage automated recovery from simulation errors."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.recovery_history = []
        self.checkpoint_manager = CheckpointManager()
        self.error_detector = ErrorDetector()
        
    def attempt_recovery(self, error_type: str, error_context: Dict):
        """Attempt recovery based on error type and context."""
        print(f"Attempting recovery for error type: {error_type}")
        
        # Record error
        recovery_attempt = {
            'timestamp': time.time(),
            'error_type': error_type,
            'error_context': error_context,
            'strategy': None,
            'success': False,
            'recovery_time': None
        }
        
        try:
            # Select appropriate recovery strategy
            strategy = self.select_recovery_strategy(error_type, error_context)
            recovery_attempt['strategy'] = strategy
            
            # Attempt recovery
            start_time = time.time()
            success = strategy(error_context)
            end_time = time.time()
            
            recovery_attempt['success'] = success
            recovery_attempt['recovery_time'] = end_time - start_time
            
            if success:
                print(f"Successfully recovered using {strategy.__name__}")
            else:
                print(f"Recovery failed using {strategy.__name__}")
                
        except Exception as e:
            print(f"Recovery exception: {e}")
            recovery_attempt['error'] = str(e)
        
        self.recovery_history.append(recovery_attempt)
        return recovery_attempt['success']
    
    def select_recovery_strategy(self, error_type: str, context: Dict):
        """Select appropriate recovery strategy."""
        strategies = {
            'configuration': self.recover_from_configuration_error,
            'initialization': self.recover_from_initialization_error,
            'simulation': self.recover_from_simulation_error,
            'memory': self.recover_from_memory_error,
            'performance': self.recover_from_performance_error
        }
        
        if error_type in strategies:
            return strategies[error_type]
        
        # Fallback to generic recovery
        return self.generic_recovery
    
    def recover_from_configuration_error(self, context: Dict):
        """Recover from configuration error."""
        print("Recovering from configuration error...")
        
        # Try to fix configuration
        fixed_config = self.fix_configuration(context.get('config', {}))
        
        if fixed_config:
            # Create new brain with fixed configuration
            new_brain = pynlm.createBrain(fixed_config)
            new_brain.initialize()
            
            self.brain = new_brain
            return True
        
        return False
    
    def recover_from_initialization_error(self, context: Dict):
        """Recover from initialization error."""
        print("Recovering from initialization error...")
        
        # Try with different configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)  # Use moderate size
        
        try:
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            
            self.brain = new_brain
            return True
            
        except Exception as e:
            print(f"Initialization recovery failed: {e}")
            return False
    
    def recover_from_simulation_error(self, context: Dict):
        """Recover from simulation error."""
        print("Recovering from simulation error...")
        
        # Try with reduced complexity
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 300)  # Reduce neuron count
        config.set("plasticity.stdp.learning_rate", 0.005)  # Reduce plasticity
        
        try:
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            
            # Test stability
            stable_count = 0
            for step in range(50):
                new_brain.step(step)
                if self.is_brain_stable(new_brain):
                    stable_count += 1
                else:
                    stable_count = 0
                
                if stable_count >= 10:  # 10 consecutive stable steps
                    self.brain = new_brain
                    print("Simulation recovery successful")
                    return True
            
            self.brain = new_brain
            return True
            
        except Exception as e:
            print(f"Simulation recovery failed: {e}")
            return False
    
    def recover_from_memory_error(self, context: Dict):
        """Recover from memory error."""
        print("Recovering from memory error...")
        
        # Use memory-efficient configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)  # Small brain
        config.set("simulation_timestep", 0.01)  # Larger timestep
        
        try:
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            
            self.brain = new_brain
            print("Memory recovery successful")
            return True
            
        except Exception as e:
            print(f"Memory recovery failed: {e}")
            return False
    
    def recover_from_performance_error(self, context: Dict):
        """Recover from performance error."""
        print("Recovering from performance error...")
        
        # Use performance-optimized configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("plasticity.stdp.learning_rate", 0.01)
        
        try:
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            
            # Test performance
            if self.is_performance_acceptable(new_brain):
                self.brain = new_brain
                print("Performance recovery successful")
                return True
            else:
                print("Performance still unacceptable")
                return False
                
        except Exception as e:
            print(f"Performance recovery failed: {e}")
            return False
    
    def generic_recovery(self, context: Dict):
        """Generic recovery strategy."""
        print("Attempting generic recovery...")
        
        # Last resort: use minimal configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        
        try:
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            
            self.brain = new_brain
            print("Generic recovery successful")
            return True
            
        except Exception as e:
            print(f"Generic recovery failed: {e}")
            return False
    
    def is_brain_stable(self, brain) -> bool:
        """Check if brain is stable."""
        firing_rates = []
        
        # Collect firing rates over several steps
        for step in range(20):
            brain.step(step)
            firing_rate = brain.getAverageFiringRate()
            
            if np.isnan(firing_rate) or firing_rate < 0 or firing_rate > 10000:
                return False
            
            firing_rates.append(firing_rate)
        
        # Check for stability (coefficient of variation < 0.5)
        if len(firing_rates) > 1:
            cv = np.std(firing_rates) / np.mean(firing_rates)
            return cv < 0.5
        
        return True
    
    def is_performance_acceptable(self, brain) -> bool:
        """Check if brain performance is acceptable."""
        # Simple performance check
        firing_rate = brain.getAverageFiringRate()
        neuron_count = brain.getTotalNeuronCount()
        
        # Check for reasonable firing rate
        if firing_rate < 0 or firing_rate > 5000:
            return False
        
        # Check for reasonable neuron utilization
        if neuron_count < 10:
            return False
        
        return True
```

### Manual Recovery

#### Interactive Recovery Commands

```bash
# List available checkpoints
./nlmc list-checkpoints

# Load specific checkpoint
./nlmc load-checkpoint checkpoint_step_100

# Create new checkpoint
./nlmc create-checkpoint

# Show recovery history
./nlmc show-recovery-history

# Force recovery from specific checkpoint
./nlmc force-recovery checkpoint_step_50
```

#### Recovery CLI Commands

```python
import pynlm
import subprocess
import os
from pathlib import Path

class RecoveryCLI:
    """Command-line interface for manual recovery."""
    
    def __init__(self):
        self.current_brain = None
        self.checkpoints_dir = Path("checkpoints")
        self.checkpoints_dir.mkdir(exist_ok=True)
    
    def run_command(self, command, args):
        """Run recovery command."""
        if command == "list-checkpoints":
            return self.list_checkpoints()
        elif command == "load-checkpoint":
            return self.load_checkpoint(args[0])
        elif command == "create-checkpoint":
            return self.create_checkpoint()
        elif command == "show-recovery-history":
            return self.show_recovery_history()
        elif command == "force-recovery":
            return self.force_recovery(args[0])
        else:
            print(f"Unknown command: {command}")
            return False
    
    def list_checkpoints(self):
        """List available checkpoints."""
        checkpoints = list(self.checkpoints_dir.glob("checkpoint_*.json"))
        
        if not checkpoints:
            print("No checkpoints available")
            return True
        
        print("Available checkpoints:")
        for checkpoint in sorted(checkpoints):
            try:
                with open(checkpoint, 'r') as f:
                    data = json.load(f)
                    print(f"  {checkpoint.name}: Step {data.get('step', 'unknown')}")
            except:
                print(f"  {checkpoint.name}: (corrupted)")
        
        return True
    
    def load_checkpoint(self, checkpoint_name):
        """Load simulation from checkpoint."""
        checkpoint_path = self.checkpoints_dir / checkpoint_name
        
        if not checkpoint_path.exists():
            print(f"Checkpoint not found: {checkpoint_name}")
            return False
        
        try:
            with open(checkpoint_path, 'r') as f:
                checkpoint_data = json.load(f)
            
            # Restore brain state
            self.restore_brain_from_checkpoint(checkpoint_data)
            
            print(f"Checkpoint loaded: {checkpoint_name}")
            return True
            
        except Exception as e:
            print(f"Failed to load checkpoint: {e}")
            return False
    
    def create_checkpoint(self):
        """Create current state as checkpoint."""
        if not self.current_brain:
            print("No brain available to checkpoint")
            return False
        
        checkpoint_data = self.capture_current_state()
        
        # Generate checkpoint filename
        timestamp = int(time.time())
        checkpoint_file = self.checkpoints_dir / f"checkpoint_step_{timestamp}.json"
        
        try:
            with open(checkpoint_file, 'w') as f:
                json.dump(checkpoint_data, f, indent=2)
            
            print(f"Checkpoint created: {checkpoint_file.name}")
            return True
            
        except Exception as e:
            print(f"Failed to create checkpoint: {e}")
            return False
    
    def show_recovery_history(self):
        """Show recovery history."""
        history_file = Path("recovery_history.json")
        
        if not history_file.exists():
            print("No recovery history available")
            return True
        
        try:
            with open(history_file, 'r') as f:
                history = json.load(f)
            
            print("Recovery History:")
            for i, recovery in enumerate(history[-10:]):  # Show last 10
                timestamp = recovery.get('timestamp', 0)
                date = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(timestamp))
                error_type = recovery.get('error_type', 'unknown')
                strategy = recovery.get('strategy', 'unknown')
                success = recovery.get('success', False)
                
                status = "✓" if success else "✗"
                print(f"  {status} {date}: {error_type} -> {strategy}")
            
            return True
            
        except Exception as e:
            print(f"Failed to show recovery history: {e}")
            return False
    
    def force_recovery(self, target_checkpoint):
        """Force recovery from specific checkpoint."""
        print(f"Force recovery to checkpoint: {target_checkpoint}")
        
        # First load the checkpoint
        if self.load_checkpoint(target_checkpoint):
            print("Recovery successful")
            return True
        else:
            print("Recovery failed")
            return False
    
    def capture_current_state(self):
        """Capture current simulation state."""
        if not self.current_brain:
            return {}
        
        return {
            'timestamp': time.time(),
            'brain_state': self.capture_brain_state(),
            'world_state': self.capture_world_state(),
            'config': self.capture_config()
        }
    
    def capture_brain_state(self):
        """Capture brain state."""
        if not self.current_brain:
            return {}
        
        return {
            'neuron_count': self.current_brain.getTotalNeuronCount(),
            'synapse_count': self.current_brain.getTotalSynapseCount(),
            'firing_rate': self.current_brain.getAverageFiringRate(),
            'firing_neuron_count': self.current_brain.getFiringNeuronCount(),
            'development_stage': self.current_brain.getDevelopmentalStage()
        }
    
    def capture_world_state(self):
        """Capture world state."""
        if not self.current_brain or not hasattr(self.current_brain, 'world'):
            return {}
        
        return {
            'simulation_time': self.current_brain.world.getSimulationTime(),
            'agent_position': self.get_agent_position()
        }
    
    def capture_config(self):
        """Capture configuration."""
        # This would depend on how configuration is stored
        return {}
    
    def restore_brain_from_checkpoint(self, checkpoint_data):
        """Restore brain from checkpoint data."""
        # This would restore brain state from checkpoint
        # For now, create a new brain with checkpoint parameters
        print("Restoring brain from checkpoint (simplified)")
        
        # Create default configuration
        config = pynlm.createDefaultConfig()
        
        # Set parameters from checkpoint if available
        if 'brain_state' in checkpoint_data:
            brain_state = checkpoint_data['brain_state']
            config.set("brain.neuron_count", brain_state.get('neuron_count', 1000))
        
        # Create and initialize brain
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        
        self.current_brain = new_brain
```

## 5. Best Practices

### Preventive Measures

1. **Configuration Validation**
   - Always validate configuration before use
   - Use configuration templates for consistency
   - Implement configuration checks in CI/CD pipelines

2. **Error Handling**
   - Implement comprehensive error handling
   - Use appropriate exception types
   - Provide meaningful error messages

3. **Resource Management**
   - Monitor system resources
   - Implement graceful degradation
   - Use resource-efficient algorithms

4. **Testing and Validation**
   - Test error scenarios explicitly
   - Validate recovery mechanisms
   - Implement stress testing

### Documentation and Monitoring

1. **Error Documentation**
   - Document common error patterns
   - Provide recovery procedures
   - Include diagnostic scripts

2. **Monitoring**
   - Monitor system metrics
   - Track error rates
   - Alert on anomalies

3. **Maintenance**
   - Regularly review error logs
   - Update recovery procedures
   - Maintain configuration templates

### Recovery Strategy Selection

1. **Fast Recovery**
   - Use for high-severity errors
   - Prioritize system stability
   - Accept some loss of functionality

2. **Complete Recovery**
   - Use for critical operations
   - Preserve full system state
   - Ensure no data loss

3. **Adaptive Recovery**
   - Use for long-running simulations
   - Learn from previous errors
   - Optimize recovery based on experience

## 6. Case Studies

### Case Study 1: Configuration Validation

**Problem**: System failing due to invalid configuration parameters.

**Solution**: Implement comprehensive configuration validation with automatic correction.

**Results**: 90% reduction in configuration-related errors, improved system reliability.

### Case Study 2: Numerical Instability Recovery

**Problem**: Simulation becoming unstable due to numerical issues.

**Solution**: Implement adaptive recovery with progressive parameter adjustment.

**Results**: System can recover from numerical instability without manual intervention.

### Case Study 3: Memory Exhaustion Management

**Problem**: System running out of memory during large simulations.

**Solution**: Implement memory monitoring with automatic scaling.

**Results**: System can handle larger simulations with controlled memory usage.

## 7. Conclusion

Error handling and troubleshooting are critical components of NLM system reliability. By implementing comprehensive error handling, automated recovery, and diagnostic tools, NLM systems can maintain high availability and provide excellent user experience.

Key takeaways:

1. **Prevention**: Implement preventive measures to reduce error occurrence
2. **Detection**: Implement comprehensive error detection mechanisms
3. **Recovery**: Provide multiple recovery strategies for different error types
4. **Monitoring**: Continuously monitor system health and performance
5. **Documentation**: Maintain comprehensive documentation and procedures

By following these guidelines, NLM systems can achieve robust error handling, efficient recovery, and optimal performance in diverse deployment scenarios.

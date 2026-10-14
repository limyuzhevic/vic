# NLM Advanced User Documentation

## Overview

This document provides comprehensive guidance for advanced users of the NLM (Neural Learning Machine) framework. It covers advanced configuration options, performance optimizations, batch operations, and detailed usage patterns.

## Table of Contents

1. [Advanced Configuration Management](#advanced-configuration-management)
2. [Performance Optimization](#performance-optimization)
3. [Batch Operations and Utilities](#batch-operations-and-utilities)
4. [Configuration Profiles and Phases](#configuration-profiles-and-phases)
5. [Python API Enhancements](#python-api-enhancements)
6. [Performance Monitoring and Analysis](#performance-monitoring-and-analysis)
7. [Advanced Examples](#advanced-examples)
8. [Troubleshooting](#troubleshooting)

---

## Advanced Configuration Management

### Configuration Categories

NLM supports advanced configuration management with the following categories:

```python
from nlm import Config, AdvancedConfig, ConfigCategory

# Create advanced configuration
adv_config = AdvancedConfig()

# Plasticity settings
adv_config.stdp_ltp_weight = 0.03f
adv_config.stdp_ltd_weight = 0.018f
adv_config.stdp_time_constant = 25.0f
adv_config.hebbian_learning_rate = 0.02f

# Development settings
adv_config.synaptogenesis_rate = 0.002f
adv_config.pruning_rate = 0.0002f
adv_config.critical_period_multiplier = 2.0f

# Neuromodulation settings
adv_config.dopamine_baseline = 0.2f
adv_config.curiosity_strength = 1.0f
adv_config.novelty_threshold = 0.9f

# Memory settings
adv_config.working_memory_capacity = 5000
adv_config.episodic_memory_capacity = 25000

# Performance settings
adv_config.simd_enabled = true
adv_config.parallel_enabled = true
adv_config.vectorization_width = 8
```

### Configuration Validation

Validate your configuration before use:

```python
def validate_config(config, phase):
    errors = config.validateAll()
    if errors:
        print("Configuration errors:")
        for error in errors:
            print(f"  - {error}")
        return False
    
    # Additional validation for specific phase
    if phase == 5:
        if not config.has("simd_enabled"):
            print("Warning: SIMD not enabled for Phase 5 optimization")
    
    return True

# Validate configuration
if validate_config(my_config, 5):
    print("Configuration validated successfully!")
```

### Configuration Export/Import

Export configuration to different formats:

```python
# Export to JSON
json_config = config.exportToJSON()
with open("brain_config.json", "w") as f:
    f.write(json_config)

# Export to YAML
yaml_config = config.exportToYAML()
with open("brain_config.yaml", "w") as f:
    f.write(yaml_config)

# Import from JSON
import json_config
config.importFromJSON(json_config)

# Import from YAML
import yaml_config
config.importFromYAML(yaml_config)
```

### Configuration Search and Filtering

Find specific configuration entries:

```python
# Search by pattern
neuron_configs = config.search("neuron")
plasticity_configs = config.search("stdp")
memory_configs = config.search("memory")

# Get by source
file_configs = config.getBySource(ConfigSource.File)
runtime_configs = config.getBySource(ConfigSource.Runtime)

# Check if any of multiple keys exist
required_keys = ["neuron_count", "region_count", "timestep"]
if config.hasAny(required_keys):
    print("All required configuration keys are present")
```

---

## Performance Optimization

### Performance Optimization Functions

NLM provides comprehensive performance optimization tools:

```python
from nlm import (
    enable_high_performance, 
    enable_balanced_performance, 
    enable_memory_efficient_performance,
    get_performance_report,
    PerformanceManager
)

# Enable high-performance mode (optimized for speed)
enable_high_performance(brain)

# Enable balanced performance mode (accuracy + speed)
enable_balanced_performance(brain)

# Enable memory-efficient mode (memory-constrained environments)
enable_memory_efficient_performance(brain)

# Get detailed performance report
report = get_performance_report(brain)
print(report)

# Get current performance configuration
perf_config = PerformanceManager.getInstance().getConfig()
print(f"SIMD Enabled: {perf_config.simdEnabled}")
print(f"Parallel Enabled: {perf_config.parallelEnabled}")
print(f"Vectorization Width: {perf_config.vectorizationWidth}")
```

### Performance Configuration

Configure performance settings for different scenarios:

```python
from nlm import PerformanceConfig

# High-performance configuration (for research)
high_perf_config = PerformanceConfig(
    simdEnabled=True,
    parallelEnabled=True,
    memoryPoolsEnabled=True,
    cachingEnabled=True,
    vectorizationEnabled=True,
    vectorizationWidth=8,
    memoryPoolSize=4096,
    profilePerformance=False
)

# Memory-efficient configuration
mem_eff_config = PerformanceConfig(
    simdEnabled=False,  # Disable SIMD for memory-constrained environments
    parallelEnabled=True,
    memoryPoolsEnabled=True,
    cachingEnabled=True,
    vectorizationEnabled=False,
    vectorizationWidth=2,
    memoryPoolSize=1024,
    profilePerformance=True  # Enable performance profiling
)
```

### Performance Monitoring

Monitor and analyze performance:

```python
# Get current performance statistics
stats = PerformanceManager.getInstance().getPerformanceStats()
print(f"Average Step Time: {stats['average_step_time'] * 1000:.2f} ms")
print(f"Steps Executed: {stats['steps_executed']}")
print(f"SIMD Operations: {stats['simd_ops']}")
print(f"Parallel Operations: {stats['parallel_ops']}")
print(f"Performance Score: {stats['performance_score']:.2f}")

# Performance grades and analysis
def analyze_performance(brain):
    report = get_performance_report(brain)
    
    # Check for performance bottlenecks
    if "High average step time" in report:
        print("⚠️  Consider disabling parallel processing to improve accuracy")
    
    if "High memory usage" in report:
        print("⚠️  Consider using memory-efficient configuration")
    
    if "No spikes generated" in report:
        print("⚠️  Check neuron parameters and current injection")
    
    # Performance recommendations
    if "Enable SIMD optimization" in report:
        print("💡 Recommendation: Enable SIMD optimization for better performance")
    
    return report

analysis = analyze_performance(brain)
```

---

## Batch Operations and Utilities

### Batch Operations

Perform batch operations on multiple neurons efficiently:

```python
from nlm import (
    batch_inject_current,
    batch_set_plasticity,
    get_advanced_neural_stats
)

# Batch current injection
neuron_ids = [nlm.NeuronId(i) for i in range(100, 200)]  # Neurons 100-199
batch_result = batch_inject_current(brain, neuron_ids, 50.0f, "all")
print(f"Injected current into {len(batch_result.affectedNeurons)} neurons")
print(f"Operation: {batch_result.operation}")

# Batch plasticity configuration
plasticity_result = batch_set_plasticity(
    brain, 
    hebbian=True, 
    stdp=True, 
    rewardModulated=True,
    regionName="region_1"
)
print(f"Applied plasticity to {plasticity_result.affectedSynapseCount} synapses")
```

### Advanced Neural Statistics

Get comprehensive neural system statistics:

```python
# Get advanced statistics
advanced_stats = get_advanced_neural_stats(brain)

print("=== Advanced Neural Statistics ===")
print(f"Total Neurons: {advanced_stats.totalNeurons}")
print(f"Total Synapses: {advanced_stats.totalSynapses}")
print(f"Sensory Neurons: {advanced_stats.sensoryNeuronCount}")
print(f"Motor Neurons: {advanced_stats.motorNeuronCount}")
print(f"Excitatory Synapses: {advanced_stats.excitatorySynapseCount}")
print(f"Inhibitory Synapses: {advanced_stats.inhibitorySynapseCount}")
print(f"Modulatory Synapses: {advanced_stats.modulatorySynapseCount}")
print(f"Hebbian Strength: {advanced_stats.averageHebbianStrength:.4f}")
print(f"STDP LTP: {advanced_stats.averageSTDPLTP:.4f}")
print(f"STDP LTD: {advanced_stats.averageSTDP_LTD:.4f}")
print(f"Working Memory Usage: {advanced_stats.workingMemoryUsage} traces")
print(f"Episodic Memory Usage: {advanced_stats.episodicMemoryUsage} episodes")
```

### Neuron Batch Processing

Advanced batch processing with cache-friendly operations:

```python
# Neuron statistics
stats = get_neuron_stats(brain)
print(f"Active Neurons: {stats.active_neurons}")
print(f"Firing Neurons: {stats.firing_neurons}")
print(f"Average Firing Rate: {stats.average_firing_rate:.2f} Hz")
print(f"Membrane Potential Range: [{stats.min_potential:.2f}, {stats.max_potential:.2f}] mV")

# Synapse statistics
synapse_stats = get_synapse_stats(brain)
print(f"Total Synapses: {synapse_stats.total_synapses}")
print(f"Active Synapses: {synapse_stats.active_synapses}")
print(f"Average Synapse Weight: {synapse_stats.average_weight:.4f}")
print(f"Excitation/Inhibition Ratio: {synapse_stats.excitation_inhibition_ratio:.2f}")
```

---

## Configuration Profiles and Phases

### Predefined Configuration Profiles

Use predefined profiles for different use cases:

```python
from nlm import get_predefined_profiles, apply_predefined_profile

# Get all available profiles
profiles = get_predefined_profiles()
for profile in profiles:
    print(f"Profile: {profile.name}")
    print(f"Description: {profile.description}")
    print(f"Target Phase: {profile.targetPhase}")
    print(f"Neurons: {profile.config.neuron_count}")
    print(f"SIMD: {profile.config.simd_enabled}")
    print()

# Apply a specific profile
brain = apply_predefined_profile(config, "Research")
print("Research profile applied successfully")

# Apply balanced profile
brain = apply_predefined_profile(config, "Balanced")
print("Balanced profile applied successfully")

# Apply high-performance profile
brain = apply_predefined_profile(config, "High Performance")
print("High Performance profile applied successfully")
```

### Phase-Specific Configurations

Different configurations for different simulation phases:

```python
from nlm import configure_phase, validate_configuration

# Configure for specific phase
configure_phase(config, 5)  # Phase 5: Performance optimizations
print("Phase 5 configuration applied")

# Validate configuration for phase
if validate_configuration(config, "phase 5"):
    print("Configuration validated for Phase 5")
else:
    print("Configuration validation failed")

# Phase-specific optimizations
def optimize_for_phase(brain, phase):
    brain.optimizeForPhase(phase)
    
    # Additional optimizations based on phase
    if phase == 2:
        print("Optimizing for real neural computation (Phase 2)")
        print("  - Enabling detailed spike recording")
        print("  - Optimizing for plasticity accuracy")
    elif phase == 5:
        print("Optimizing for performance (Phase 5)")
        print("  - Enabling SIMD vectorization")
        print("  - Enabling parallel processing")
        print("  - Optimizing memory usage")
    elif phase == 6:
        print("Optimizing for integration (Phase 6)")
        print("  - Enabling full memory integration")
        print("  - Optimizing neuromodulation")
        print("  - Enabling cognitive functions")

optimize_for_phase(brain, 5)
```

---

## Python API Enhancements

### Enhanced Configuration API

New methods for enhanced configuration management:

```python
# Configuration comparison
config1 = create_default_config()
config2 = create_phase_config(5)

# Check if configurations are equal
if config1.equals(config2):
    print("Configurations are identical")

# Get configuration similarity
similarity = config1.similarity(config2)
print(f"Configuration similarity: {similarity * 100:.1f}%")

# Configuration diff
differences = config1.diff(config2)
print(f"Found {len(differences)} differences:")
for diff in differences:
    print(f"  Key: {diff.key}")
    print(f"    Config1: {diff.value1}")
    print(f"    Config2: {diff.value2}")
```

### Advanced Brain API

Enhanced brain methods:

```python
# Performance information
print(f"SIMD Enabled: {brain.isSIMDEnabled()}")
print(f"Parallel Enabled: {brain.isParallelEnabled()}")

# Get development stage
stage = brain.getDevelopmentalStage()
print(f"Developmental Stage: {stage}")

# Optimize for specific phase
brain.optimizeForPhase(4)  # Phase 4: Advanced Cognition

# Performance monitoring
stats = brain.getPerformanceStats()
print(f"Steps executed: {stats.steps_executed}")
print(f"Average step time: {stats.average_step_time * 1000:.2f} ms")
```

---

## Performance Monitoring and Analysis

### Performance Profiling

Profile performance of different operations:

```python
from nlm import PerformanceProfiler

# Create custom performance profiler
profiler = PerformanceProfiler()

# Profile specific operations
profiler.startMeasurement("neuron_step")
for i in range(1000):
    brain.step(i)
profiler.stopMeasurement("neuron_step")

profiler.startMeasurement("plasticity_update")
for i in range(1000):
    # Perform plasticity operations
    pass
profiler.stopMeasurement("plasticity_update")

# Get performance statistics
print("=== Performance Profiling Results ===")
print(f"Neuron Step - Average: {profiler.getAverageTime('neuron_step') * 1000:.2f} ms")
print(f"Neuron Step - Minimum: {profiler.getMinTime('neuron_step') * 1000:.2f} ms")
print(f"Neuron Step - Maximum: {profiler.getMaxTime('neuron_step') * 1000:.2f} ms")
print(f"Neuron Step - Calls: {profiler.getCallCount('neuron_step')}")

profiler.printReport()
```

### Performance Analysis

Analyze performance bottlenecks and optimization opportunities:

```python
from nlm import analyze_performance, PerformanceAnalysis

# Get performance analysis
analysis = analyze_performance(brain)

# Performance grades
print(f"Overall Performance Grade: {analysis.performance_grade}")
print(f"System Health: {analysis.system_health}")
print(f"Integration Score: {analysis.integration_score:.2f}")

# Check for performance issues
if analysis.performance_issues:
    print("\nPerformance Issues Found:")
    for issue in analysis.performance_issues:
        print(f"  - {issue}")

# Performance recommendations
recommendations = analysis.get_recommendations()
if recommendations:
    print("\nPerformance Recommendations:")
    for rec in recommendations:
        print(f"  - {rec}")

# Performance optimization suggestions
optimization_suggestions = analysis.get_optimization_suggestions()
if optimization_suggestions:
    print("\nOptimization Suggestions:")
    for suggestion in optimization_suggestions:
        print(f"  - {suggestion}")
```

---

## Advanced Examples

### Example 1: Complete Advanced Brain Setup

```python
"""
Complete advanced brain setup with all optimizations enabled.
This example demonstrates best practices for advanced NLM usage.
"""
import nlm

# 1. Create advanced configuration
config = nlm.createDefaultConfig()

# 2. Apply advanced optimization
adv_config = nlm.AdvancedConfig()
adv_config.neuron_count = 10000
adv_config.sensory_neuron_count = 1000
adv_config.motor_neuron_count = 500
adv_config.stdp_ltp_weight = 0.03f
adv_config.stdp_ltd_weight = 0.018f
adv_config.simd_enabled = True
adv_config.parallel_enabled = True
adv_config.vectorization_width = 8

config.applyAdvancedConfig(adv_config)

# 3. Create brain with advanced configuration
brain = nlm.createBrain(config)
brain.initialize()

# 4. Apply high-performance optimization
nlm.enable_high_performance(brain)

# 5. Configure for Phase 5 (Performance)
brain.optimizeForPhase(5)

# 6. Validate configuration
if validate_configuration(config, "Phase 5"):
    print("Advanced brain setup validated successfully")

# 7. Run simulation with performance monitoring
steps = 1000
for step in range(steps):
    # Create sensory input
    sensory_input = nlm.Vision(8, 8, 3)
    # ... process sensory input ...
    
    brain.step(step, sensory_input)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# 8. Analyze performance
report = nlm.get_performance_report(brain)
print("\n" + report)
```

### Example 2: Batch Processing for Large Networks

```python
"""
Efficient batch processing for large neural networks.
This example demonstrates how to process multiple neurons simultaneously.
"""
import nlm

# Create configuration for large network
config = nlm.createDefaultConfig()
config.set("neuron_count", 50000)
config.set("region_count", 16)

# Create brain
brain = nlm.createBrain(config)
brain.initialize()

# Apply balanced performance optimization
nlm.enable_balanced_performance(brain)

# Get all neuron IDs
neuron_ids = []
region = brain.getRegion(nlm.RegionId(1))
neurons = region.getAllNeurons()
for neuron in neurons:
    neuron_ids.append(neuron.getId())

print(f"Total neurons: {len(neuron_ids)}")

# Process neurons in batches
batch_size = 1000
for batch_start in range(0, len(neuron_ids), batch_size):
    batch_end = min(batch_start + batch_size, len(neuron_ids))
    current_batch = neuron_ids[batch_start:batch_end]
    
    print(f"Processing batch: {batch_start} - {batch_end}")
    
    # Batch current injection
    batch_result = nlm.batch_inject_current(
        brain, 
        current_batch, 
        25.0f,  # Current amplitude
        "all"   # All neurons
    )
    
    # Process batch simulation steps
    for step in range(batch_size):
        brain.step(batch_start + step)
    
    print(f"  Batch result: {len(batch_result.affectedNeurons)} neurons affected")
    print(f"  Operation: {batch_result.operation}")

# Get final statistics
advanced_stats = nlm.get_advanced_neural_stats(brain)
print(f"\nFinal Statistics:")
print(f"  Total Neurons: {advanced_stats.totalNeurons}")
print(f"  Total Synapses: {advanced_stats.totalSynapses}")
print(f"  Active Neurons: {advanced_stats.activeNeurons}")
print(f"  Firing Neurons: {advanced_stats.firingNeurons}")
print(f"  Average Firing Rate: {advanced_stats.averageFiringRate:.2f} Hz")
```

### Example 3: Configuration Management for Multiple Experiments

```python
"""
Configuration management for multiple experimental runs.
This example demonstrates how to manage configurations across experiments.
"""
import nlm
import json
import yaml

class ExperimentManager:
    def __init__(self):
        self.experiments = {}
        self.current_experiment = None
    
    def create_experiment(self, name, config):
        """Create a new experiment with specified configuration."""
        self.experiments[name] = {
            'config': config,
            'brain': nlm.createBrain(config),
            'metadata': {}
        }
        print(f"Experiment '{name}' created successfully")
    
    def apply_preset_profile(self, experiment_name, profile_name):
        """Apply a predefined configuration profile."""
        if experiment_name not in self.experiments:
            raise ValueError(f"Experiment '{experiment_name}' not found")
        
        profiles = nlm.get_predefined_profiles()
        profile = next((p for p in profiles if p.name == profile_name), None)
        
        if not profile:
            raise ValueError(f"Profile '{profile_name}' not found")
        
        self.experiments[experiment_name]['config'].applyAdvancedConfig(profile.config)
        print(f"Profile '{profile_name}' applied to experiment '{experiment_name}'")
    
    def run_experiment(self, experiment_name, steps):
        """Run an experiment for specified number of steps."""
        if experiment_name not in self.experiments:
            raise ValueError(f"Experiment '{experiment_name}' not found")
        
        experiment = self.experiments[experiment_name]
        brain = experiment['brain']
        
        print(f"Running experiment '{experiment_name}' for {steps} steps...")
        
        for step in range(steps):
            brain.step(step)
            
            if step % 100 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print(f"Experiment '{experiment_name}' completed")
        
        # Store performance results
        experiment['metadata']['steps_executed'] = steps
        experiment['metadata']['final_spikes'] = brain.getTotalSpikeCount()
        experiment['metadata']['final_firing_rate'] = brain.getAverageFiringRate()
    
    def export_experiment_results(self, experiment_name, filepath):
        """Export experiment results to file."""
        if experiment_name not in self.experiments:
            raise ValueError(f"Experiment '{experiment_name}' not found")
        
        experiment = self.experiments[experiment_name]
        
        # Create results dictionary
        results = {
            'metadata': experiment['metadata'],
            'config': {
                'neuron_count': experiment['config'].getOr<int64_t>("neuron_count", 0),
                'region_count': experiment['config'].getOr<int64_t>("region_count", 0),
                'connection_probability': experiment['config'].getOr<float>("connection_probability", 0.0f)
            }
        }
        
        # Export to JSON
        with open(f"{filepath}_results.json", 'w') as f:
            json.dump(results, f, indent=2)
        
        # Export configuration to YAML
        config_yaml = experiment['config'].exportToYAML()
        with open(f"{filepath}_config.yaml", 'w') as f:
            f.write(config_yaml)
        
        print(f"Experiment results exported to {filepath}_results.json and {filepath}_config.yaml")

# Usage example
manager = ExperimentManager()

# Create experiments with different configurations
manager.create_experiment("Baseline", nlm.createDefaultConfig())
manager.create_experiment("High Performance", nlm.createDefaultConfig())
manager.create_experiment("Memory Efficient", nlm.createDefaultConfig())

# Apply profiles
manager.apply_preset_profile("High Performance", "High Performance")
manager.apply_preset_profile("Memory Efficient", "Memory Efficient")

# Run experiments
manager.run_experiment("Baseline", 1000)
manager.run_experiment("High Performance", 1000)
manager.run_experiment("Memory Efficient", 1000)

# Export results
manager.export_experiment_results("Baseline", "baseline")
manager.export_experiment_results("High Performance", "high_performance")
manager.export_experiment_results("Memory Efficient", "memory_efficient")
```

---

## Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration Validation Fails

**Problem:** Configuration validation is failing with unexpected errors.

**Solution:**

```python
# Check for specific validation errors
def debug_validation(config, phase):
    errors = config.validateAll()
    if errors:
        print(f"Validation errors for Phase {phase}:")
        for i, error in enumerate(errors):
            print(f"  {i+1}. {error}")
            
            # Provide specific guidance based on error type
            if "neuron_count" in error:
                print("    → Consider reducing neuron_count for better performance")
            elif "connection_probability" in error:
                print("    → Connection probability should be <= 1.0")
            elif "stdp" in error:
                print("    → Check STDP parameter ranges")
    
    return len(errors) == 0

# Validate with debugging
if not debug_validation(config, 5):
    print("Configuration needs correction")
    # Fix configuration and try again
    config.set("neuron_count", 5000, nlm.ConfigSource::Default)
    config.set("connection_probability", 0.2, nlm.ConfigSource::Default)
```

#### Issue: Performance is Slower Than Expected

**Problem:** Performance is lower than expected.

**Solution:**

```python
# Check performance bottlenecks
def diagnose_performance(brain):
    report = get_performance_report(brain)
    
    issues = []
    if "High average step time" in report:
        issues.append("Average step time is high (> 100ms)")
    if "High memory usage" in report:
        issues.append("Memory usage is high (> 50%)")
    if "No spikes generated" in report:
        issues.append("No spikes generated after many steps")
    
    if issues:
        print("Performance bottlenecks detected:")
        for i, issue in enumerate(issues):
            print(f"  {i+1}. {issue}")
        
        print("\nRecommendations:")
        if "High average step time" in str(issues):
            print("  1. Consider using high-performance configuration")
            print("  2. Check neuron parameters and current injection")
        if "High memory usage" in str(issues):
            print("  1. Use memory-efficient configuration")
            print("  2. Consider reducing neuron count")
        if "No spikes generated" in str(issues):
            print("  1. Check neuron thresholds and current injection")
            print("  2. Verify synaptic connections")
    
    return issues

# Diagnose performance
bottlenecks = diagnose_performance(brain)
if bottlenecks:
    print("Action needed to improve performance")
else:
    print("Performance is within expected range")
```

#### Issue: Configuration Not Applied Correctly

**Problem:** Configuration changes are not being applied as expected.

**Solution:**

```python
# Check configuration sources
def debug_configuration(config):
    print("=== Configuration Debug Information ===")
    
    # Check key values and their sources
    keys_to_check = ["neuron_count", "region_count", "timestep"]
    for key in keys_to_check:
        if config.has(key):
            entry = config.getEntry(key)
            print(f"{key}: {config.getOr<int64_t>(key, 0)} (source: {entry.source})")
        else:
            print(f"{key}: Not set")
    
    # Check for configuration conflicts
    print("\n=== Configuration Conflict Check ===")
    if config.has("neuron_count") and config.has("region_count"):
        neuron_count = config.getOr<int64_t>("neuron_count", 0)
        region_count = config.getOr<int64_t>("region_count", 0)
        if neuron_count / region_count > 10000:
            print("⚠️  Warning: High neuron count per region may impact performance")
    
    # Check for recommended settings
    print("\n=== Recommended Settings Check ===")
    if not config.has("stdp_ltp_weight"):
        print("💡 Recommendation: Set STDP LTP weight for Phase 2+")
    if not config.has("development_synaptogenesis_rate"):
        print("💡 Recommendation: Set development parameters for Phase 3+")
    
    return True

# Debug configuration
debug_configuration(config)
```

### Getting Help

If you encounter issues not covered in this documentation:

1. **Check the main NLM documentation** (easy_usage.md) for basic usage
2. **Review the configuration files** (configs/) for examples
3. **Use the performance monitoring tools** to identify bottlenecks
4. **Check the console output** for error messages and warnings
5. **Verify your configuration** against the predefined profiles
6. **Consider reducing the scale** of your simulation if performance is an issue

For advanced support, refer to the Phase 6 integration documentation and the complete NLM documentation set.

---

## Version Information

This advanced documentation covers NLM Phase 6 features with:
- Enhanced configuration management
- Advanced performance optimization
- Comprehensive batch operations
- Detailed profiling and analysis tools
- Multiple configuration profiles for different use cases

For more information about NLM phases and features, refer to the main NLM documentation.

---

*This documentation is continuously updated with new features and improvements. For the latest version, check the NLM documentation repository.*
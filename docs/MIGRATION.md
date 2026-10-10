# NLM Python API Migration Guide

## Overview

This document provides comprehensive guidance on migrating from older versions of the NLM Python API to the enhanced version 2.0.0. It covers breaking changes, migration steps, and compatibility considerations.

## Table of Contents

- [Migration Overview](#migration-overview)
- [Version Differences](#version-differences)
  - [Breaking Changes](#breaking-changes)
  - [New Features](#new-features)
  - [API Improvements](#api-improvements)
- [Migration Steps](#migration-steps)
  - [Pre-Migration Checklist](#pre-migration-checklist)
  - [Version Detection](#version-detection)
  - [Migration Process](#migration-process)
- [Specific Migration Examples](#specific-migration-examples)
  - [Configuration Migration](#configuration-migration)
  - [Simulation Migration](#simulation-migration)
  - [Expert API Migration](#expert-api-migration)
- [Compatibility Considerations](#compatibility-considerations)
  - [Backward Compatibility](#backward-compatibility)
  - [Forward Compatibility](#forward-compatibility)
  - [Migration Testing](#migration-testing)
- [Migration Tools](#migration-tools)
  - [Automatic Migration Tools](#automatic-migration-tools)
  - [Migration Scripts](#migration-scripts)
  - [Validation Tools](#validation-tools)
- [Migration Best Practices](#migration-best-practices)
  - [Gradual Migration](#gradual-migration)
  - [Testing Strategy](#testing-strategy)
  - [Documentation Updates](#documentation-updates)
- [Migration Support](#migration-support)
  - [Support Resources](#support-resources)
  - [Common Issues and Solutions](#common-issues-and-solutions)
- [Migration FAQ](#migration-faq)
- [References](#references)

## Migration Overview

### Migration Purpose

This guide provides comprehensive migration instructions for users upgrading from NLM Python API version 1.x to version 2.0.0. The migration involves significant improvements to the API while maintaining backward compatibility for most common use cases.

### Migration Challenges

1. **API Changes**: Many classes and functions have been modified
2. **New Features**: Advanced expert APIs require learning
3. **Performance Optimizations**: Performance improvements may require code changes
4. **Configuration Updates**: Configuration system has been enhanced
5. **Error Handling**: Enhanced error handling requires updates

### Migration Approach

1. **Assessment**: Evaluate current usage and identify affected areas
2. **Planning**: Create migration plan with priorities and timelines
3. **Testing**: Test migration in controlled environment
4. **Implementation**: Apply changes incrementally
5. **Validation**: Verify migration success
6. **Documentation**: Update documentation and examples

## Version Differences

### Breaking Changes

#### 1. Exception Hierarchy

**Previous (v1.x):**
```python
try:
    brain = nlm.Brain(config)
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
```

**Current (v2.0.0):**
```python
try:
    brain = nlm.Brain(config)
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
```

**Changes:** All exceptions now inherit from `NLMError` for better error handling.

#### 2. Configuration System

**Previous (v1.x):**
```python
config = nlm.Config()
config.brain_simulated_neurons = 1000
config.world_width = 20
brain = nlm.Brain(config)
```

**Current (v2.0.0):**
```python
config = nlm.create_default_config()
config.set('brain.simulated_neurons', 1000)
config.set('world.width', 20)
brain = nlm.create_brain(config)
```

**Changes:** Enhanced configuration system with factory functions and JSON support.

#### 3. Factory Functions

**Previous (v1.x):**
```python
config = nlm.Config()
config.set('brain.simulated_neurons', 1000)
brain = nlm.Brain(config)
world = nlm.SimpleWorld()
world.configure(20, 20, 64, 64)
```

**Current (v2.0.0):**
```python
config = nlm.create_default_config()
brain = nlm.create_brain(config)
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)
```

**Changes:** New factory functions simplify common setup patterns.

#### 4. Expert APIs

**Previous (v1.x):**
```python
# Limited expert functionality
memory_pool = expert.MemoryPool(1024, 8192)
```

**Current (v2.0.0):**
```python
# Full expert API suite
from nlm import expert

# Performance optimization
memory_pool = expert.MemoryPool(8192, 65536)
simd_processor = expert.SIMDProcessor()
parallel_processor = expert.ParallelProcessor(8)

# Advanced simulation
custom_plasticity = expert.CustomPlasticityRules()
neuromodulation = expert.AdvancedNeuromodulation({
    'dopamine_strength': 0.9,
    'curiosity_strength': 0.8
})

# Data analysis
neural_signal = expert.NeuralSignalProcessor()
pattern_recognizer = expert.PatternRecognizer()
```

**Changes:** Comprehensive expert API suite added for advanced users.

#### 5. Action System

**Previous (v1.x):**
```python
move_action = nlm.Action.createMoveAction('MoveForward', [1.0, 0.0, 0.0])
look_action = nlm.Action.createLookAction('LookLeft')
rest_action = nlm.Action.createRestAction()
```

**Current (v2.0.0):**
```python
move_action = nlm.Action.createMoveAction('MoveForward', [1.0, 0.0, 0.0])
look_action = nlm.Action.createLookAction('LookLeft')
rest_action = nlm.Action.createRestAction()
custom_action = nlm.Action.createCustomAction('eat', [0.8, 0.2])
```

**Changes:** Enhanced action system with additional action types.

### New Features

#### 1. Enhanced Configuration Presets

**New Configuration Presets:**
```python
# Default configuration
config = nlm.create_default_config()

# Minimal configuration for fast iteration
config = nlm.create_minimal_config()

# High performance configuration
config = nlm.create_high_performance_config()

# World presets
config = nlm.create_grid_world_config()
config = nlm.create_maze_world_config()
```

#### 2. Performance Monitoring

**Performance Monitoring:**
```python
# Create performance monitor
monitor = nlm.create_performance_monitor()

# Record simulation metrics
monitor.recordStep(neurons_active, spikes_generated, time_delta)

# Get performance report
report = monitor.getPerformanceReport()
print(f"Performance: {report['simulation_speed']:.2f} steps/sec")
```

#### 3. Simulation Control

**Simulation Control:**
```python
# Create simulation controller
controller = nlm.create_simulation_controller()

# Pause/resume simulation
controller.pause()
controller.resume()

# Reset simulation
controller.reset()

# Check status
print(controller.getStatus())
```

#### 4. Batch Processing

**Batch Processing:**
```python
# Create batch processor
batch = nlm.create_batch_processor("simulation_batch")
batch.addOperation(lambda: agent_brain.reset())
batch.addOperation(lambda: world.reset())
batch.execute()  # Execute all operations
```

#### 5. Experiment Management

**Experiment Management:**
```python
# Create experiment manager
experiment_manager = nlm.create_experiment_manager()

# Run experiments
for config_preset in [
    nlm.create_default_config(),
    nlm.create_minimal_config(),
    nlm.create_high_performance_config()
]:
    world = nlm.create_simple_world()
    world.configure(20, 20, 64, 64)
    world.setAgentStart(10, 10)
    
    brain = nlm.create_brain(config_preset)
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    
    # Run experiment
    experiment_manager.run_experiment(
        "test_experiment",
        brain,
        world,
        50.0  # duration
    )
```

#### 6. Visualization Helpers

**Visualization Helpers:**
```python
# Generate neuron activity plot
plot = nlm.generate_neuron_activity_plot(brain)
print(plot)

# Generate connection density plot
density_plot = nlm.generate_connection_density_plot(brain)
print(density_plot)
```

#### 7. Serialization Utilities

**Serialization Utilities:**
```python
# Vector to JSON
vector = [0.1, 0.5, 0.9]
json_str = nlm.vector_to_json(vector)

# JSON to vector
restored_vector = nlm.json_to_vector(json_str)

# Object to JSON
obj = nlm.Vision(64, 64)
json_str = nlm.object_to_json(obj)

# JSON to object
restored_obj = nlm.json_to_object(json_str)
```

### API Improvements

#### 1. Type Hints

**Improved Type Hints:**
```python
# Enhanced type hints for better IDE support
from typing import Dict, List, Optional

config: nlm.Config = nlm.create_default_config()
brain: nlm.Brain = nlm.create_brain(config)
world: nlm.SimpleWorld = nlm.create_simple_world()
```

#### 2. Documentation

**Enhanced Documentation:**
```python
# Comprehensive docstring documentation
help(nlm.Brain)
help(nlm.create_simulation_controller)
help(nlm.vector_to_json)
```

#### 3. Error Handling

**Enhanced Error Handling:**
```python
# Custom exception types
try:
    brain = nlm.Brain(config)
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
```

## Migration Steps

### Pre-Migration Checklist

#### Step 1: Assess Current Usage

1. **List all NLM usage in your codebase**
   - Import statements
   - Class instantiations
   - Method calls
   - Configuration settings

2. **Identify affected components**
   - Core API usage
   - Expert API usage
   - Configuration patterns
   - Error handling

3. **Create inventory of changes needed**
   ```python
   # Create migration inventory
   migration_inventory = {
       'core_api_changes': [],
       'expert_api_changes': [],
       'configuration_changes': [],
       'error_handling_changes': [],
       'new_features_needed': []
   }
   ```

#### Step 2: Version Detection

```python
# Detect current NLM version
import nlm

def detectNLMVersion():
    """Detect NLM version and provide migration guidance."""
    version = nlm.__version__
    
    print(f"Current NLM version: {version}")
    
    if version.startswith("1."):
        print("Migrating from NLM v1.x to v2.0.0")
        return "v1_to_v2"
    elif version.startswith("2."):
        print("Already using NLM v2.0.0")
        return "v2_current"
    else:
        print("Unknown NLM version")
        return "unknown"

# Run version detection
version_status = detectNLMVersion()
```

#### Step 3: Create Migration Plan

```python
# Create migration plan
migration_plan = {
    'phase_1': {
        'description': 'Basic API migration',
        'tasks': [
            'Update import statements',
            'Replace configuration access',
            'Update factory function calls',
            'Fix exception handling'
        ],
        'estimated_time': '1-2 days',
        'risk': 'Low'
    },
    'phase_2': {
        'description': 'Expert API integration',
        'tasks': [
            'Add expert API imports',
            'Implement performance optimizations',
            'Add visualization support',
            'Implement monitoring'
        ],
        'estimated_time': '2-3 days',
        'risk': 'Medium'
    },
    'phase_3': {
        'description': 'Advanced features',
        'tasks': [
            'Implement batch processing',
            'Add experiment management',
            'Implement custom plasticity',
            'Add neuromodulation'
        ],
        'estimated_time': '3-4 days',
        'risk': 'High'
    }
}
```

### Migration Process

#### Phase 1: Basic API Migration

1. **Update Import Statements**
```python
# Before
import nlm
from nlm import Config, Brain, create_default_config

# After
import nlm
from nlm import create_default_config, create_brain, create_simple_world
```

2. **Update Configuration Access**
```python
# Before
config = nlm.Config()
config.brain_simulated_neurons = 1000
config.world_width = 20

# After
config = nlm.create_default_config()
config.set('brain.simulated_neurons', 1000)
config.set('world.width', 20)
```

3. **Update Factory Function Calls**
```python
# Before
brain = nlm.Brain(config)
world = nlm.SimpleWorld()

# After
brain = nlm.create_brain(config)
world = nlm.create_simple_world()
```

4. **Fix Exception Handling**
```python
# Before
try:
    brain = nlm.Brain(config)
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")

# After
try:
    brain = nlm.create_brain(config)
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
```

#### Phase 2: Expert API Integration

1. **Add Expert API Imports**
```python
# Add expert imports
from nlm import expert
from nlm.expert import (
    MemoryPool, SIMDProcessor, ParallelProcessor,
    CustomAllocator, ZeroCopyInterface, DirectMemoryAccess,
    CustomPlasticityRules, AdvancedNeuromodulation,
    CustomConnectivityPatterns, NeuralPopulationManager,
    NeuralSignalProcessor, PatternRecognizer, StatisticalAnalyzer,
    MLIntegration, SignalFilter, FeatureExtractor,
    CustomNeuronModel, SynapticDynamics,
    RegionConfiguration, NetworkTopology,
    DynamicConfig, ConfigInheritance, ConfigValidator,
    ConfigOptimizer, ConfigSnapshot,
    CustomExperimentLoop, ParallelExperimentRunner,
    CheckpointManager, ResultAnalyzer, ExperimentMonitor,
    CXXExtensionAPI, CustomIOFormat, SensorIntegration,
    WebSocketAPI, ExternalResourceManager,
    AdvancedPlotting, RealTimeVisualizer,
    VisualizationPipeline, DataExporter,
    MemoryProfiler, PerformanceProfiler,
    BenchmarkRunner, ProfilingSession
)
```

2. **Implement Performance Optimizations**
```python
# Add performance optimizations
memory_pool = expert.MemoryPool(8192, 65536)
simd_processor = expert.SIMDProcessor()
parallel_processor = expert.ParallelProcessor(8)
```

3. **Add Visualization Support**
```python
# Add visualization helpers
import nlm

# Generate plots
activity_plot = nlm.generate_neuron_activity_plot(brain)
density_plot = nlm.generate_connection_density_plot(brain)
```

4. **Implement Monitoring**
```python
# Add monitoring
monitor = nlm.create_performance_monitor()
controller = nlm.create_simulation_controller()
```

#### Phase 3: Advanced Features

1. **Implement Batch Processing**
```python
# Add batch processing
batch = nlm.create_batch_processor("simulation_batch")
batch.addOperation(lambda: agent_brain.reset())
batch.addOperation(lambda: world.reset())
batch.execute()
```

2. **Add Experiment Management**
```python
# Add experiment management
experiment_manager = nlm.create_experiment_manager()
```

3. **Implement Custom Plasticity**
```python
# Add custom plasticity
plasticity_rules = expert.CustomPlasticityRules()
plasticity_rules.add_rule("hebbian", strength=0.7, window=10)
```

4. **Add Neuromodulation**
```python
# Add neuromodulation
neuromodulation = expert.AdvancedNeuromodulation({
    'dopamine_strength': 0.9,
    'curiosity_strength': 0.8,
    'novelty_threshold': 0.3
})
```

## Specific Migration Examples

### Configuration Migration

#### Example 1: Simple Configuration

**Before:**
```python
# Old way (v1.x)
config = nlm.Config()
config.brain_simulated_neurons = 1000
config.world_width = 20
brain = nlm.Brain(config)
world = nlm.SimpleWorld()
world.configure(20, 20, 64, 64)
```

**After:**
```python
# New way (v2.0.0)
config = nlm.create_default_config()
config.set('brain.simulated_neurons', 1000)
config.set('world.width', 20)
brain = nlm.create_brain(config)
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)
```

#### Example 2: Advanced Configuration

**Before:**
```python
# Old way (v1.x)
config = nlm.Config()
config.brain_simulated_neurons = 5000
config.brain_min_plasticity_rate = 0.001
config.brain_max_plasticity_rate = 0.1
config.neuromodulation_dopamine_influence = 0.8
config.neuromodulation_curiosity_strength = 0.6
config.performance_memory_pool_size = 8192
```

**After:**
```python
# New way (v2.0.0)
config = nlm.create_high_performance_config()
```

### Simulation Migration

#### Example 1: Basic Simulation

**Before:**
```python
# Old way (v1.x)
config = nlm.Config()
config.brain_simulated_neurons = 1000
brain = nlm.Brain(config)
world = nlm.SimpleWorld()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)

brain.initialize(world)

agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(brain, world)

for step in range(100):
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, step)
    world.update(0.001)
```

**After:**
```python
# New way (v2.0.0)
config = nlm.create_default_config()
brain = nlm.create_brain(config)
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)

brain.initialize(world)

agent_brain = nlm.create_agent_brain(brain)
agent_brain.initialize(brain, world)

for step in range(100):
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, step)
    world.update(0.001)
```

#### Example 2: Simulation with Monitoring

**Before:**
```python
# Old way (v1.x)
config = nlm.Config()
config.brain_simulated_neurons = 1000
brain = nlm.Brain(config)
world = nlm.SimpleWorld()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)

brain.initialize(world)

agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(brain, world)

# Run simulation without monitoring
for step in range(100):
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    action = agent_brain.decodeMotorCommand()
    world.applyMotorCommand(action, step)
    world.update(0.001)
```

**After:**
```python
# New way (v2.0.0)
config = nlm.create_default_config()
brain = nlm.create_brain(config)
world = nlm.create_simple_world()
world.configure(20, 20, 64, 64)
world.setAgentStart(10, 10)

brain.initialize(world)

agent_brain = nlm.create_agent_brain(brain)
agent_brain.initialize(brain, world)

# Create monitoring
controller = nlm.create_simulation_controller()
monitor = nlm.create_performance_monitor()

# Run simulation with monitoring
for step in range(100):
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent_brain.processSensoryInput(percept)
    
    # Generate action
    action = agent_brain.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, step)
    world.update(0.001)
    
    # Record performance
    neurons_active = brain.getActiveNeuronCount()
    monitor.recordStep(neurons_active, 0, 0.001)

# Get performance report
performance_report = monitor.getPerformanceReport()
print(f"Simulation completed: {performance_report['simulation_speed']:.2f} steps/sec")
```

### Expert API Migration

#### Example 1: Performance Optimization

**Before:**
```python
# Old way (v1.x)
from nlm import expert

# Basic performance features
memory_pool = expert.MemoryPool(1024, 8192)
```

**After:**
```python
# New way (v2.0.0)
from nlm import expert

# Comprehensive performance features
memory_pool = expert.MemoryPool(8192, 65536)
simd_processor = expert.SIMDProcessor()
parallel_processor = expert.ParallelProcessor(8)

# Advanced performance features
custom_allocator = expert.CustomAllocator("buddy", 64)
zero_copy = expert.ZeroCopyInterface()
direct_memory = expert.DirectMemoryAccess("cpu")

# Performance monitoring
memory_profiler = expert.MemoryProfiler()
performance_profiler = expert.PerformanceProfiler()
benchmark_runner = expert.BenchmarkRunner()
```

#### Example 2: Advanced Simulation

**Before:**
```python
# Old way (v1.x)
from nlm import expert

# Basic plasticity rules
plasticity_rules = expert.CustomPlasticityRules()
```

**After:**
```python
# New way (v2.0.0)
from nlm import expert

# Advanced plasticity rules
custom_plasticity = expert.CustomPlasticityRules()
custom_plasticity.add_rule("hebbian", strength=0.7, window=10)
custom_plasticity.add_rule("stdp", alpha=0.01, beta=0.005)

# Advanced neuromodulation
neuromodulation = expert.AdvancedNeuromodulation({
    'dopamine_strength': 0.9,
    'serotonin_influence': 0.7,
    'curiosity_strength': 0.8,
    'novelty_threshold': 0.3
})

# Advanced connectivity patterns
connectivity = expert.CustomConnectivityPatterns()
```

## Compatibility Considerations

### Backward Compatibility

#### Compatible Areas

1. **Exception Types**: All existing exception types are maintained
2. **Core Classes**: Core classes (`Config`, `Brain`, `SimpleWorld`, etc.) are maintained
3. **Basic Methods**: Basic methods are maintained with same signatures
4. **Factory Functions**: New factory functions are added, existing methods still work

#### Incompatible Areas

1. **Configuration Access**: Direct attribute access changed to `set()` method
2. **Factory Functions**: Old constructor calls replaced with factory functions
3. **Expert APIs**: New expert APIs added, but old APIs still available
4. **Documentation**: Enhanced documentation replaces older documentation

### Forward Compatibility

#### Planning for Future Versions

1. **Type Hints**: Use enhanced type hints for better compatibility
2. **Abstract Classes**: Use abstract base classes for future extensions
3. **Plugin Architecture**: Design for plugin-based extensions
4. **Configuration Validation**: Use enhanced configuration validation
5. **Error Handling**: Use enhanced error handling with future compatibility

### Migration Testing

#### Testing Strategy

1. **Unit Testing**: Test individual components
2. **Integration Testing**: Test component interactions
3. **Performance Testing**: Test performance characteristics
4. **Load Testing**: Test system under load
5. **Compatibility Testing**: Test with different configurations

#### Test Suite

```python
# Comprehensive migration test suite
import unittest
import nlm
from nlm import expert

class TestNMLMigration(unittest.TestCase):
    """Test NML migration from v1.x to v2.0.0."""
    
    def setUp(self):
        """Set up test environment."""
        # Create test configuration
        self.config = nlm.create_default_config()
        
        # Create test systems
        self.world = nlm.create_simple_world()
        self.world.configure(20, 20, 64, 64)
        self.world.setAgentStart(10, 10)
        
        self.brain = nlm.create_brain(self.config)
        self.brain.initialize(self.world)
        
        self.agent_brain = nlm.create_agent_brain(self.brain)
        self.agent_brain.initialize(self.brain, self.world)
    
    def tearDown(self):
        """Clean up test environment."""
        # Clean up resources
        pass
    
    def testConfigurationMigration(self):
        """Test configuration migration."""
        # Test that configuration works
        self.config.set('brain.simulated_neurons', 1000)
        self.assertEqual(self.config.get('brain.simulated_neurons'), 1000)
    
    def testBrainMigration(self):
        """Test brain migration."""
        # Test that brain initialization works
        self.assertIsNotNone(self.brain)
        stats = self.brain.getStatistics()
        self.assertGreater(stats['total_neurons'], 0)
    
    def testWorldMigration(self):
        """Test world migration."""
        # Test that world configuration works
        self.assertEqual(self.world.width, 20)
        self.assertEqual(self.world.height, 20)
    
    def testAgentMigration(self):
        """Test agent brain migration."""
        # Test that agent brain initialization works
        self.assertIsNotNone(self.agent_brain)
    
    def testSimulationMigration(self):
        """Test simulation migration."""
        # Run simulation and verify results
        for step in range(10):
            percept = self.world.getSensoryPercept()
            self.agent_brain.processSensoryInput(percept)
            action = self.agent_brain.decodeMotorCommand()
            self.world.applyMotorCommand(action, step)
            self.world.update(0.001)
        
        # Verify final state
        self.assertIsNotNone(self.agent_brain)
        self.assertGreater(self.brain.getActiveNeuronCount(), 0)
    
    def testExpertMigration(self):
        """Test expert API migration."""
        # Test expert APIs
        memory_pool = expert.MemoryPool(1024, 8192)
        simd_processor = expert.SIMDProcessor()
        parallel_processor = expert.ParallelProcessor(4)
        
        # Verify expert APIs work
        self.assertIsNotNone(memory_pool)
        self.assertIsNotNone(simd_processor)
        self.assertIsNotNone(parallel_processor)
    
    def testErrorHandlingMigration(self):
        """Test error handling migration."""
        # Test error handling
        try:
            # This should raise an error
            raise nlm.ConfigurationError("Test error")
        except nlm.ConfigurationError as e:
            self.assertIn("Test error", str(e))
    
    def testPerformanceMonitoringMigration(self):
        """Test performance monitoring migration."""
        # Create performance monitor
        monitor = nlm.create_performance_monitor()
        
        # Record performance
        for step in range(10):
            neurons_active = self.brain.getActiveNeuronCount()
            monitor.recordStep(neurons_active, 0, 0.001)
        
        # Get performance report
        report = monitor.getPerformanceReport()
        
        # Verify report
        self.assertIn('simulation_speed', report)
        self.assertGreater(report['simulation_speed'], 0)

# Run migration tests
if __name__ == '__main__':
    unittest.main()
```

## Migration Tools

### Automatic Migration Tools

#### Migration Assistant

```python
"""Automatic migration assistant for NLM"""
import re
import ast
import nlm

class MigrationAssistant:
    """Assist with NLM migration from v1.x to v2.0.0."""
    
    def __init__(self, source_code: str):
        """Initialize migration assistant.
        
        Args:
            source_code: Source code to migrate
        """
        self.source_code = source_code
        self.parsed_code = ast.parse(source_code)
    
    def migrateImports(self) -> str:
        """Migrate import statements."""
        # Replace old import patterns
        new_imports = []
        
        for node in ast.walk(self.parsed_code):
            if isinstance(node, ast.Import):
                for alias in node.names:
                    if alias.name == 'nlm':
                        new_imports.append("import nlm")
                    elif alias.name == 'nlm.Brain':
                        new_imports.append("from nlm import create_brain")
                    elif alias.name == 'nlm.Config':
                        new_imports.append("from nlm import create_default_config")
                    elif alias.name == 'nlm.SimpleWorld':
                        new_imports.append("from nlm import create_simple_world")
            
            elif isinstance(node, ast.ImportFrom):
                if node.module == 'nlm':
                    if 'Brain' in [alias.name for alias in node.names]:
                        new_imports.append("from nlm import create_brain")
                    elif 'Config' in [alias.name for alias in node.names]:
                        new_imports.append("from nlm import create_default_config")
                    elif 'SimpleWorld' in [alias.name for alias in node.names]:
                        new_imports.append("from nlm import create_simple_world")
                    elif 'AgentBrain' in [alias.name for alias in node.names]:
                        new_imports.append("from nlm import create_agent_brain")
        
        # Create migration report
        migration_report = {
            'import_changes': new_imports,
            'summary': f"Updated {len(new_imports)} import statements"
        }
        
        return json.dumps(migration_report, indent=2)
    
    def migrateConfigurationAccess(self) -> str:
        """Migrate configuration access patterns."""
        # Replace attribute access with set() method
        new_code = re.sub(
            r'config\.(\w+)\s*=\s*(\d+)',
            r'config.set("\1", \2)',
            self.source_code
        )
        
        new_code = re.sub(
            r'config\.(\w+)\s*=\s*"([^"]+)"',
            r'config.set("\1", "\2")',
            new_code
        )
        
        # Create migration report
        migration_report = {
            'code_changes': [
                "Replaced attribute access with config.set() method",
                "Updated configuration access patterns"
            ],
            'summary': "Migrated configuration access patterns"
        }
        
        return json.dumps(migration_report, indent=2)
    
    def migrateFactoryFunctions(self) -> str:
        """Migrate factory function calls."""
        # Replace constructor calls with factory functions
        new_code = self.source_code
        
        # Replace nlm.Config() with create_default_config()
        new_code = re.sub(r'nlm\.Config\(\)', 'nlm.create_default_config()', new_code)
        
        # Replace nlm.Brain(config) with create_brain(config)
        new_code = re.sub(r'nlm\.Brain\((.*?)\)', r'nlm.create_brain(\1)', new_code)
        
        # Replace nlm.SimpleWorld() with create_simple_world()
        new_code = re.sub(r'nlm\.SimpleWorld\(\)', 'nlm.create_simple_world()', new_code)
        
        # Replace nlm.AgentBrain(brain) with create_agent_brain(brain)
        new_code = re.sub(r'nlm\.AgentBrain\((.*?)\)', r'nlm.create_agent_brain(\1)', new_code)
        
        # Create migration report
        migration_report = {
            'code_changes': [
                "Replaced nlm.Config() with create_default_config()",
                "Replaced nlm.Brain() with create_brain()",
                "Replaced nlm.SimpleWorld() with create_simple_world()",
                "Replaced nlm.AgentBrain() with create_agent_brain()"
            ],
            'summary': "Migrated factory function calls"
        }
        
        return json.dumps(migration_report, indent=2)
    
    def generateMigrationReport(self) -> str:
        """Generate comprehensive migration report."""
        import_report = self.migrateImports()
        config_report = self.migrateConfigurationAccess()
        factory_report = self.migrateFactoryFunctions()
        
        report = {
            'imports': json.loads(import_report),
            'configuration': json.loads(config_report),
            'factory_functions': json.loads(factory_report),
            'summary': {
                'total_changes': 3,
                'estimated_migration_time': '2-3 days',
                'risk_level': 'Medium',
                'recommended_approch': 'Phase-by-phase migration'
            }
        }
        
        return json.dumps(report, indent=2)

# Example usage
if __name__ == '__main__':
    # Sample source code to migrate
    source_code = '''
import nlm
from nlm import Config, Brain, create_default_config

config = Config()
config.brain_simulated_neurons = 1000
config.world_width = 20

brain = Brain(config)
world = nlm.SimpleWorld()
world.configure(20, 20, 64, 64)

agent_brain = nlm.AgentBrain(brain)
'''
    
    # Create migration assistant
    assistant = MigrationAssistant(source_code)
    
    # Generate migration report
    report = assistant.generateMigrationReport()
    print("Migration Report:")
    print(report)
```

#### Migration Validator

```python
"""Migration validator for NLM"""
import nlm
import sys

class MigrationValidator:
    """Validate NLM migration from v1.x to v2.0.0."""
    
    def __init__(self, test_config: dict = None):
        """Initialize migration validator.
        
        Args:
            test_config: Test configuration
        """
        self.test_config = test_config or {
            'neuron_count': 1000,
            'world_size': 20,
            'steps': 50,
            'timeout': 30
        }
    
    def validateConfiguration(self, config: nlm.Config) -> bool:
        """Validate configuration migration.
        
        Args:
            config: Configuration to validate
            
        Returns:
            True if configuration is valid, False otherwise
        """
        try:
            # Check required parameters
            self.assertGreater(config.get('brain.simulated_neurons', 0), 0)
            self.assertGreater(config.get('world.width', 0), 0)
            self.assertGreater(config.get('world.height', 0), 0)
            
            # Validate configuration
            config.validate()
            
            return True
            
        except Exception as e:
            print(f"Configuration validation failed: {e}")
            return False
    
    def validateBrain(self, brain: nlm.Brain, world: nlm.SimpleWorld) -> bool:
        """Validate brain migration.
        
        Args:
            brain: Brain to validate
            world: World to validate
            
        Returns:
            True if brain is valid, False otherwise
        """
        try:
            # Check brain statistics
            stats = brain.getStatistics()
            self.assertGreater(stats['total_neurons'], 0)
            self.assertGreater(stats['active_neurons'], 0)
            
            # Initialize brain
            brain.initialize(world)
            
            return True
            
        except Exception as e:
            print(f"Brain validation failed: {e}")
            return False
    
    def validateWorld(self, world: nlm.SimpleWorld) -> bool:
        """Validate world migration.
        
        Args:
            world: World to validate
            
        Returns:
            True if world is valid, False otherwise
        """
        try:
            # Check world configuration
            self.assertGreater(world.width, 0)
            self.assertGreater(world.height, 0)
            self.assertGreater(world.view_distance, 0)
            
            # Check agent start position
            body = world.getAgentBody()
            self.assertIsNotNone(body)
            
            return True
            
        except Exception as e:
            print(f"World validation failed: {e}")
            return False
    
    def validateAgent(self, agent_brain: nlm.AgentBrain, brain: nlm.Brain, world: nlm.SimpleWorld) -> bool:
        """Validate agent brain migration.
        
        Args:
            agent_brain: Agent brain to validate
            brain: Brain instance
            world: World instance
            
        Returns:
            True if agent brain is valid, False otherwise
        """
        try:
            # Initialize agent brain
            agent_brain.initialize(brain, world)
            
            # Test agent functionality
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            action = agent_brain.decodeMotorCommand()
            
            # Verify action
            self.assertIsNotNone(action)
            
            return True
            
        except Exception as e:
            print(f"Agent validation failed: {e}")
            return False
    
    def runMigrationTest(self) -> dict:
        """Run migration test.
        
        Returns:
            Migration test results
        """
        print("Running NLM migration validation...")
        
        # Create test configuration
        config = nlm.create_default_config()
        config.set('brain.simulated_neurons', self.test_config['neuron_count'])
        config.set('world.width', self.test_config['world_size'])
        config.set('world.height', self.test_config['world_size'])
        
        # Create test systems
        world = nlm.create_simple_world()
        world.configure(
            self.test_config['world_size'],
            self.test_config['world_size'],
            64,
            1.0
        )
        world.setAgentStart(
            self.test_config['world_size'] // 2,
            self.test_config['world_size'] // 2
        )
        
        brain = nlm.create_brain(config)
        agent_brain = nlm.create_agent_brain(brain)
        
        # Run validation tests
        results = {
            'configuration': self.validateConfiguration(config),
            'brain': self.validateBrain(brain, world),
            'world': self.validateWorld(world),
            'agent': self.validateAgent(agent_brain, brain, world)
        }
        
        # Overall result
        overall_result = all(results.values())
        
        print(f"\nMigration Validation Results:")
        print(f"  Configuration: {'PASS' if results['configuration'] else 'FAIL'}")
        print(f"  Brain: {'PASS' if results['brain'] else 'FAIL'}")
        print(f"  World: {'PASS' if results['world'] else 'FAIL'}")
        print(f"  Agent: {'PASS' if results['agent'] else 'FAIL'}")
        print(f"\nOverall Result: {'PASS' if overall_result else 'FAIL'}")
        
        return {
            'results': results,
            'overall_result': overall_result,
            'config_used': self.test_config
        }

# Example usage
if __name__ == '__main__':
    # Create migration validator
    validator = MigrationValidator()
    
    # Run migration test
    test_results = validator.runMigrationTest()
    
    # Exit with appropriate code
    sys.exit(0 if test_results['overall_result'] else 1)
```

### Migration Scripts

#### Migration Script Example

```bash
#!/bin/bash
# Migration script for NLM v1.x to v2.0.0

# Set up environment
export PYTHONPATH="/path/to/nlm:$PYTHONPATH"

# Create backup of original files
mkdir -p backups
find . -name "*.py" -not -path "./backups/*" -exec cp {} backups/ \;

# Run migration
python3 migrate.py --source . --target . --dry-run

# Apply migration
python3 migrate.py --source . --target . --apply

# Run tests
python3 -m pytest tests/ -v

# Check for issues
python3 validate_migration.py

# Generate migration report
python3 generate_report.py

# Deploy if tests pass
if [ $? -eq 0 ]; then
    echo "Migration completed successfully!"
else
    echo "Migration failed! Please review errors."
    exit 1
fi
```

#### Migration Script Implementation

```python
"""Migration script for NLM"""
import argparse
import os
import sys
import shutil
import subprocess
from pathlib import Path

class MigrationScript:
    """Migration script for NLM from v1.x to v2.0.0."""
    
    def __init__(self, source_dir: str, target_dir: str, dry_run: bool = False):
        """Initialize migration script.
        
        Args:
            source_dir: Source directory to migrate from
            target_dir: Target directory to migrate to
            dry_run: If True, only show what would be changed
        """
        self.source_dir = Path(source_dir)
        self.target_dir = Path(target_dir)
        self.dry_run = dry_run
    
    def backupOriginalFiles(self):
        """Create backup of original files."""
        backup_dir = self.source_dir / "backups"
        backup_dir.mkdir(exist_ok=True)
        
        # Copy Python files
        for py_file in self.source_dir.glob("*.py"):
            if not py_file.name.startswith("backup_"):
                shutil.copy2(py_file, backup_dir / py_file.name)
        
        # Copy configuration files
        for config_file in self.source_dir.glob("*.json"):
            shutil.copy2(config_file, backup_dir / config_file.name)
        
        # Copy documentation files
        for doc_file in self.source_dir.glob("*.md"):
            shutil.copy2(doc_file, backup_dir / doc_file.name)
        
        print(f"Created backup at {backup_dir}")
    
    def migrateSourceFiles(self):
        """Migrate source files."""
        # Process Python files
        for py_file in self.source_dir.glob("*.py"):
            self.migratePythonFile(py_file)
        
        # Process configuration files
        for config_file in self.source_dir.glob("*.json"):
            self.migrateConfigFile(config_file)
        
        # Process documentation files
        for doc_file in self.source_dir.glob("*.md"):
            self.migrateDocFile(doc_file)
    
    def migratePythonFile(self, py_file: Path):
        """Migrate Python file.
        
        Args:
            py_file: Python file to migrate
        """
        # Read file content
        with open(py_file, 'r') as f:
            content = f.read()
        
        # Apply migrations
        new_content = self.applyMigrations(content)
        
        # Write migrated file
        if self.dry_run:
            print(f"[Dry Run] Would migrate: {py_file}")
            print(f"  Changes: {self.getMigrationSummary(content, new_content)}")
        else:
            # Backup original file
            backup_file = py_file.with_suffix('.py.backup')
            shutil.copy2(py_file, backup_file)
            
            # Write migrated file
            with open(py_file, 'w') as f:
                f.write(new_content)
            
            print(f"Migrated: {py_file}")
    
    def applyMigrations(self, content: str) -> str:
        """Apply migration transformations.
        
        Args:
            content: Source content
            
        Returns:
            Migrated content
        """
        # Replace import statements
        content = self.replaceImports(content)
        
        # Replace configuration access
        content = self.replaceConfigurationAccess(content)
        
        # Replace factory functions
        content = self.replaceFactoryFunctions(content)
        
        # Replace exception handling
        content = self.replaceExceptionHandling(content)
        
        # Add expert API imports
        content = self.addExpertAPIImports(content)
        
        # Add performance monitoring
        content = self.addPerformanceMonitoring(content)
        
        return content
    
    def replaceImports(self, content: str) -> str:
        """Replace import statements.
        
        Args:
            content: Source content
            
        Returns:
            Content with updated imports
        """
        # Replace nlm.Config with create_default_config
        content = content.replace('from nlm import Config', 'from nlm import create_default_config')
        content = content.replace('from nlm import Config,', 'from nlm import create_default_config,')
        
        # Replace nlm.Brain with create_brain
        content = content.replace('from nlm import Brain', 'from nlm import create_brain')
        content = content.replace('from nlm import Brain,', 'from nlm import create_brain,')
        
        # Replace nlm.SimpleWorld with create_simple_world
        content = content.replace('from nlm import SimpleWorld', 'from nlm import create_simple_world')
        content = content.replace('from nlm import SimpleWorld,', 'from nlm import create_simple_world,')
        
        # Replace nlm.AgentBrain with create_agent_brain
        content = content.replace('from nlm import AgentBrain', 'from nlm import create_agent_brain')
        content = content.replace('from nlm import AgentBrain,', 'from nlm import create_agent_brain,')
        
        return content
    
    def replaceConfigurationAccess(self, content: str) -> str:
        """Replace configuration access patterns.
        
        Args:
            content: Source content
            
        Returns:
            Content with updated configuration access
        """
        # Replace attribute access with set() method
        import re
        
        # Replace config.brain_simulated_neurons = X with config.set('brain.simulated_neurons', X)
        content = re.sub(
            r'config\.(\w+)\s*=\s*(\d+)',
            r'config.set('\1', \2)',
            content
        )
        
        # Replace config.world_width = X with config.set('world.width', X)
        content = re.sub(
            r'config\.world_width\s*=\s*(\d+)',
            r'config.set(\'world.width\', \1)',
            content
        )
        
        # Replace config.world_height = X with config.set('world.height', X)
        content = re.sub(
            r'config\.world_height\s*=\s*(\d+)',
            r'config.set(\'world.height\', \1)',
            content
        )
        
        return content
    
    def replaceFactoryFunctions(self, content: str) -> str:
        """Replace factory function calls.
        
        Args:
            content: Source content
            
        Returns:
            Content with updated factory functions
        """
        # Replace nlm.Config() with create_default_config()
        content = content.replace('nlm.Config()', 'nlm.create_default_config()')
        
        # Replace nlm.Brain(config) with create_brain(config)
        content = content.replace('nlm.Brain(config)', 'nlm.create_brain(config)')
        
        # Replace nlm.SimpleWorld() with create_simple_world()
        content = content.replace('nlm.SimpleWorld()', 'nlm.create_simple_world()')
        
        # Replace nlm.AgentBrain(brain) with create_agent_brain(brain)
        content = content.replace('nlm.AgentBrain(brain)', 'nlm.create_agent_brain(brain)')
        
        return content
    
    def replaceExceptionHandling(self, content: str) -> str:
        """Replace exception handling.
        
        Args:
            content: Source content
            
        Returns:
            Content with updated exception handling
        """
        # Update exception handling to use new exception types
        # Note: Exception types are the same in v2.0.0
        return content
    
    def addExpertAPIImports(self, content: str) -> str:
        """Add expert API imports.
        
        Args:
            content: Source content
            
        Returns:
            Content with expert API imports
        """
        # Add expert API import if not present
        if 'from nlm import expert' not in content:
            content += '\n\n# Expert API imports\nfrom nlm import expert\n'
        
        return content
    
    def addPerformanceMonitoring(self, content: str) -> str:
        """Add performance monitoring.
        
        Args:
            content: Source content
            
        Returns:
            Content with performance monitoring
        """
        # Add performance monitoring if not present
        if 'create_performance_monitor' not in content:
            content += '\n\n# Performance monitoring\ncontroller = nlm.create_simulation_controller()\nmonitor = nlm.create_performance_monitor()\n'
        
        return content
    
    def getMigrationSummary(self, original: str, migrated: str) -> dict:
        """Get migration summary.
        
        Args:
            original: Original content
            migrated: Migrated content
            
        Returns:
            Migration summary
        """
        import_diff = len(set(migrated.split()) - set(original.split()))
        config_diff = len(set(migrated.split('\n')) - set(original.split('\n')))
        
        return {
            'import_changes': import_diff,
            'configuration_changes': config_diff,
            'factory_function_changes': 'nlm.Config() -> create_default_config()',
            'expert_api_added': 'from nlm import expert',
            'performance_added': 'create_simulation_controller, create_performance_monitor'
        }
    
    def run(self):
        """Run migration."""
        if self.dry_run:
            print("Running migration in dry-run mode...")
        else:
            print("Running migration...")
        
        # Create backup
        self.backupOriginalFiles()
        
        # Migrate files
        self.migrateSourceFiles()
        
        # Generate report
        self.generateReport()
        
        if self.dry_run:
            print("\nDry-run completed. Review changes before applying.")
        else:
            print("\nMigration completed successfully!")
    
    def generateReport(self):
        """Generate migration report."""
        report = {
            'migration_date': str(datetime.datetime.now()),
            'source_directory': str(self.source_dir),
            'target_directory': str(self.target_dir),
            'dry_run': self.dry_run,
            'changes_applied': self.getChangesApplied(),
            'next_steps': self.getNextSteps()
        }
        
        # Save report
        report_file = self.target_dir / 'migration_report.json'
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"\nMigration report saved to: {report_file}")

# Example usage
if __name__ == '__main__':
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Migrate NLM from v1.x to v2.0.0')
    parser.add_argument('--source', required=True, help='Source directory')
    parser.add_argument('--target', required=True, help='Target directory')
    parser.add_argument('--dry-run', action='store_true', help='Run in dry-run mode')
    
    args = parser.parse_args()
    
    # Create migration script
    migration = MigrationScript(args.source, args.target, args.dry_run)
    
    # Run migration
    migration.run()
```

#### Migration Makefile

```makefile
# Makefile for NLM migration

.PHONY: migrate validate test backup

# Default target
all: migrate

# Run migration
migrate:
	@echo "Running NLM migration from v1.x to v2.0.0..."
	python3 migrate.py --source src --target src --dry-run

# Apply migration
apply:
	@echo "Applying NLM migration..."
	python3 migrate.py --source src --target src --apply

# Validate migration
validate:
	@echo "Validating NLM migration..."
	python3 validate_migration.py

# Create backup
backup:
	@echo "Creating backup of current state..."
	mkdir -p backups
	find . -name "*.py" -not -path "./backups/*" -exec cp {} backups/ \;
	find . -name "*.json" -not -path "./backups/*" -exec cp {} backups/ \;

# Run tests
 tests:
	@echo "Running tests..."
	python3 -m pytest tests/ -v

# Generate report
generate-report:
	@echo "Generating migration report..."
	python3 generate_report.py

# Clean up
clean:
	@echo "Cleaning up..."
	rm -rf backups
	rm -f migration_report.json
	rm -f *.py.backup

# Full migration
full: backup apply validate tests generate-report
	@echo "Migration completed successfully!"

# Dry-run migration
dry-run:
	@echo "Running dry-run migration..."
	python3 migrate.py --source src --target src --dry-run

# Status check
status:
	@echo "Checking migration status..."
	@if [ -f "migration_report.json" ]; then
		echo "Migration report found."
		diff -u backups/$(shell date +%Y%m%d_%H%M%S).py src/$(shell date +%Y%m%d_%H%M%S).py.backup || true
	else
		echo "No migration report found."
	fi
```

### Validation Tools

#### Validation Script

```python
"""Validation script for NLM migration"""
import nlm
import json
import sys
from datetime import datetime

class ValidationScript:
    """Validation script for NLM migration."""
    
    def __init__(self, test_config: dict = None):
        """Initialize validation script.
        
        Args:
            test_config: Test configuration
        """
        self.test_config = test_config or {
            'test_name': 'NLM Migration Validation',
            'description': 'Validate NLM migration from v1.x to v2.0.0',
            'tests': [
                'configuration_validation',
                'brain_validation',
                'world_validation',
                'agent_validation',
                'expert_api_validation',
                'performance_monitoring_validation',
                'exception_handling_validation'
            ],
            'thresholds': {
                'success_rate': 0.9,
                'performance_threshold': 0.8,
                'memory_threshold': 100  # MB
            }
        }
    
    def validateConfiguration(self, config: nlm.Config) -> dict:
        """Validate configuration.
        
        Args:
            config: Configuration to validate
            
        Returns:
            Validation result
        """
        print("Validating configuration...")
        
        try:
            # Check required parameters
            required_params = [
                ('brain.simulated_neurons', int, 1, 1000000),
                ('world.width', int, 1, 1000),
                ('world.height', int, 1, 1000),
                ('world.view_distance', int, 1, 1000)
            ]
            
            for param, param_type, min_val, max_val in required_params:
                value = config.get(param)
                
                if value is None:
                    return {
                        'status': 'FAIL',
                        'error': f'Missing required parameter: {param}',
                        'value': None
                    }
                
                if not isinstance(value, param_type):
                    return {
                        'status': 'FAIL',
                        'error': f'Invalid parameter type: {param}. Expected {param_type}, got {type(value)}',
                        'value': value
                    }
                
                if value < min_val or value > max_val:
                    return {
                        'status': 'FAIL',
                        'error': f'Parameter out of range: {param}. Value: {value}, Range: [{min_val}, {max_val}]',
                        'value': value
                    }
            
            # Validate configuration
            config.validate()
            
            return {
                'status': 'PASS',
                'error': None,
                'value': 'Configuration is valid'
            }
            
        except Exception as e:
            return {
                'status': 'FAIL',
                'error': f'Configuration validation error: {e}',
                'value': str(e)
            }
    
    def validateBrain(self, brain: nlm.Brain, world: nlm.SimpleWorld) -> dict:
        """Validate brain.
        
        Args:
            brain: Brain to validate
            world: World to validate
            
        Returns:
            Validation result
        """
        print("Validating brain...")
        
        try:
            # Check brain initialization
            if brain is None:
                return {
                    'status': 'FAIL',
                    'error': 'Brain is None',
                    'value': None
                }
            
            # Get brain statistics
            stats = brain.getStatistics()
            
            # Check required statistics
            required_stats = ['total_neurons', 'active_neurons', 'synapses_formed']
            for stat in required_stats:
                if stat not in stats:
                    return {
                        'status': 'FAIL',
                        'error': f'Missing brain statistics: {stat}',
                        'value': stats
                    }
            
            # Check values
            if stats['total_neurons'] <= 0:
                return {
                    'status': 'FAIL',
                    'error': f'Invalid total neurons: {stats["total_neurons"]}',
                    'value': stats
                }
            
            if stats['active_neurons'] < 0:
                return {
                    'status': 'FAIL',
                    'error': f'Invalid active neurons: {stats["active_neurons"]}',
                    'value': stats
                }
            
            # Initialize brain
            brain.initialize(world)
            
            return {
                'status': 'PASS',
                'error': None,
                'value': f'Brain initialized successfully. Neurons: {stats["total_neurons"]}'
            }
            
        except Exception as e:
            return {
                'status': 'FAIL',
                'error': f'Brain validation error: {e}',
                'value': str(e)
            }
    
    def validateWorld(self, world: nlm.SimpleWorld) -> dict:
        """Validate world.
        
        Args:
            world: World to validate
            
        Returns:
            Validation result
        """
        print("Validating world...")
        
        try:
            # Check world properties
            if world is None:
                return {
                    'status': 'FAIL',
                    'error': 'World is None',
                    'value': None
                }
            
            # Check dimensions
            if world.width <= 0:
                return {
                    'status': 'FAIL',
                    'error': f'Invalid world width: {world.width}',
                    'value': world.width
                }
            
            if world.height <= 0:
                return {
                    'status': 'FAIL',
                    'error': f'Invalid world height: {world.height}',
                    'value': world.height
                }
            
            if world.view_distance <= 0:
                return {
                    'status': 'FAIL',
                    'error': f'Invalid world view distance: {world.view_distance}',
                    'value': world.view_distance
                }
            
            # Check agent start position
            body = world.getAgentBody()
            if body is None:
                return {
                    'status': 'FAIL',
                    'error': 'Agent body is None',
                    'value': None
                }
            
            return {
                'status': 'PASS',
                'error': None,
                'value': f'World is valid. Size: {world.width}x{world.height}'
            }
            
        except Exception as e:
            return {
                'status': 'FAIL',
                'error': f'World validation error: {e}',
                'value': str(e)
            }
    
    def validateAgent(self, agent_brain: nlm.AgentBrain, brain: nlm.Brain, world: nlm.SimpleWorld) -> dict:
        """Validate agent.
        
        Args:
            agent_brain: Agent brain to validate
            brain: Brain instance
            world: World instance
            
        Returns:
            Validation result
        """
        print("Validating agent...")
        
        try:
            # Initialize agent brain
            agent_brain.initialize(brain, world)
            
            # Test agent functionality
            percept = world.getSensoryPercept()
            agent_brain.processSensoryInput(percept)
            action = agent_brain.decodeMotorCommand()
            
            # Verify action
            if action is None:
                return {
                    'status': 'FAIL',
                    'error': 'Agent returned None action',
                    'value': None
                }
            
            return {
                'status': 'PASS',
                'error': None,
                'value': f'Agent initialized successfully. Action type: {action.getType()}'
            }
            
        except Exception as e:
            return {
                'status': 'FAIL',
                'error': f'Agent validation error: {e}',
                'value': str(e)
            }
    
    def validateExpertAPI(self) -> dict:
        """Validate expert API.
        
        Returns:
            Validation result
        """
        print("Validating expert API...")
        
        try:
            # Import expert APIs
            from nlm import expert
            
            # Test expert APIs
            memory_pool = expert.MemoryPool(1024, 8192)
            simd_processor = expert.SIMDProcessor()
            parallel_processor = expert.ParallelProcessor(4)
            
            # Verify expert APIs
            if memory_pool is None:
                return {
                    'status': 'FAIL',
                    'error': 'Memory pool is None',
                    'value': None
                }
            
            if simd_processor is None:
                return {
                    'status': 'FAIL',
                    'error': 'SIMD processor is None',
                    'value': None
                }
            
            if parallel_processor is None:
                return {
                    'status': 'FAIL',
                    'error': 'Parallel processor is None',
                    'value': None
                }
            
            return {
                'status': 'PASS',
                'error': None,
                'value': 'Expert APIs are available and working'
            }
            
        except Exception as e:
            return {
                'status': 'FAIL',
                'error': f'Expert API validation error: {e}',
                'value': str(e)
            }
    
    def validatePerformanceMonitoring(self) -> dict:
        """Validate performance monitoring.
        
        Returns:
            Validation result
        """
        print("Validating performance monitoring...")
        
        try:
            # Create performance monitor
            monitor = nlm.create_performance_monitor()
            controller = nlm.create_simulation_controller()
            
            # Verify performance monitoring
            if monitor is None:
                return {
                    'status': 'FAIL',
                    'error': 'Performance monitor is None',
                    'value': None
                }
            
            if controller is None:
                return {
                    'status': 'FAIL',
                    'error': 'Simulation controller is None',
                    'value': None
                }
            
            return {
                'status': 'PASS',
                'error': None,
                'value': 'Performance monitoring is working'
            }
            
        except Exception as e:
            return {
                'status': 'FAIL',
                'error': f'Performance monitoring validation error: {e}',
                'value': str(e)
            }
    
    def validateExceptionHandling(self) -> dict:
        """Validate exception handling.
        
        Returns:
            Validation result
        """
        print("Validating exception handling...")
        
        try:
            # Test exception handling
            from nlm import ConfigurationError, SimulationError, ValidationError
            
            # Test ConfigurationError
            try:
                raise ConfigurationError("Test configuration error")
            except ConfigurationError as e:
                if "Test configuration error" not in str(e):
                    return {
                        'status': 'FAIL',
                        'error': 'Configuration error message mismatch',
                        'value': str(e)
                    }
            
            # Test SimulationError
            try:
                raise SimulationError("Test simulation error")
            except SimulationError as e:
                if "Test simulation error" not in str(e):
                    return {
                        'status': 'FAIL',
                        'error': 'Simulation error message mismatch',
                        'value': str(e)
                    }
            
            # Test ValidationError
            try:
                raise ValidationError("Test validation error")
            except ValidationError as e:
                if "Test validation error" not in str(e):
                    return {
                        'status': 'FAIL',
                        'error': 'Validation error message mismatch',
                        'value': str(e)
                    }
            
            return {
                'status': 'PASS',
                'error': None,
                'value': 'Exception handling is working correctly'
            }
            
        except Exception as e:
            return {
                'status': 'FAIL',
                'error': f'Exception handling validation error: {e}',
                'value': str(e)
            }
    
    def runValidation(self) -> dict:
        """Run validation tests.
        
        Returns:
            Validation results
        """
        print(f"\n{'='*60}")
        print(f"Running NLM Migration Validation")
        print(f"{'='*60}")
        
        # Create test configuration
        config = nlm.create_default_config()
        config.set('brain.simulated_neurons', self.test_config['neuron_count'])
        config.set('world.width', self.test_config['world_size'])
        config.set('world.height', self.test_config['world_size'])
        
        # Create test systems
        world = nlm.create_simple_world()
        world.configure(
            self.test_config['world_size'],
            self.test_config['world_size'],
            64,
            1.0
        )
        world.setAgentStart(
            self.test_config['world_size'] // 2,
            self.test_config['world_size'] // 2
        )
        
        brain = nlm.create_brain(config)
        agent_brain = nlm.create_agent_brain(brain)
        
        # Run validation tests
        results = {
            'configuration': self.validateConfiguration(config),
            'brain': self.validateBrain(brain, world),
            'world': self.validateWorld(world),
            'agent': self.validateAgent(agent_brain, brain, world),
            'expert_api': self.validateExpertAPI(),
            'performance_monitoring': self.validatePerformanceMonitoring(),
            'exception_handling': self.validateExceptionHandling()
        }
        
        # Calculate overall result
        passed_tests = sum(1 for result in results.values() if result['status'] == 'PASS')
        total_tests = len(results)
        success_rate = passed_tests / total_tests
        
        # Generate validation report
        validation_report = {
            'test_name': self.test_config['test_name'],
            'description': self.test_config['description'],
            'timestamp': datetime.now().isoformat(),
            'results': results,
            'summary': {
                'total_tests': total_tests,
                'passed_tests': passed_tests,
                'failed_tests': total_tests - passed_tests,
                'success_rate': success_rate,
                'status': 'PASS' if success_rate >= self.test_config['thresholds']['success_rate'] else 'FAIL'
            },
            'thresholds': self.test_config['thresholds'],
            'recommendations': self.generateRecommendations(results)
        }
        
        # Print validation results
        print(f"\n{'='*60}")
        print(f"VALIDATION RESULTS")
        print(f"{'='*60}")
        print(f"Total tests: {total_tests}")
        print(f"Passed tests: {passed_tests}")
        print(f"Failed tests: {total_tests - passed_tests}")
        print(f"Success rate: {success_rate:.2%}")
        print(f"Overall status: {validation_report['summary']['status']}")
        
        print(f"\n{'='*60}")
        print(f"TEST DETAILS")
        print(f"{'='*60}")
        
        for test_name, result in results.items():
            status = 'PASS' if result['status'] == 'PASS' else 'FAIL'
            print(f"{test_name.replace('_', ' ').title()}: {status}")
            if result['error']:
                print(f"  Error: {result['error']}")
        
        # Print recommendations
        if validation_report['recommendations']:
            print(f"\n{'='*60}")
            print(f"RECOMMENDATIONS")
            print(f"{'='*60}")
            for recommendation in validation_report['recommendations']:
                print(f"- {recommendation}")
        
        # Save validation report
        report_file = 'validation_report.json'
        with open(report_file, 'w') as f:
            json.dump(validation_report, f, indent=2)
        
        print(f"\nValidation report saved to: {report_file}")
        
        return validation_report
    
    def generateRecommendations(self, results: dict) -> list:
        """Generate recommendations based on validation results.
        
        Args:
            results: Validation results
            
        Returns:
            List of recommendations
        """
        recommendations = []
        
        # Check failed tests
        failed_tests = [name for name, result in results.items() if result['status'] == 'FAIL']
        
        if failed_tests:
            recommendations.append(
                f"Fix {len(failed_tests)} failed tests before proceeding with migration"
            )
            
            for test_name in failed_tests:
                error = results[test_name]['error']
                if error:
                    recommendations.append(f"- Fix {test_name}: {error}")
        
        # Check success rate
        passed_tests = sum(1 for result in results.values() if result['status'] == 'PASS')
        total_tests = len(results)
        success_rate = passed_tests / total_tests
        
        if success_rate < 0.8:
            recommendations.append(
                f"Low success rate ({success_rate:.2%}). Consider revising migration approach."
            )
        
        # Check expert API
        if results.get('expert_api', {}).get('status') == 'FAIL':
            recommendations.append(
                "Ensure expert APIs are properly installed and configured"
            )
        
        # Check performance monitoring
        if results.get('performance_monitoring', {}).get('status') == 'FAIL':
            recommendations.append(
                "Ensure performance monitoring is properly configured"
            )
        
        # If all tests pass
        if len(failed_tests) == 0 and success_rate >= 0.9:
            recommendations.append(
                "Migration validation successful! Ready to deploy."
            )
        
        return recommendations

# Example usage
if __name__ == '__main__':
    # Create validation script
    validator = ValidationScript()
    
    # Run validation
    validation_results = validator.runValidation()
    
    # Exit with appropriate code
    if validation_results['summary']['status'] == 'PASS':
        print("\nValidation successful! Migration can proceed.")
        sys.exit(0)
    else:
        print("\nValidation failed! Migration cannot proceed.")
        sys.exit(1)
```

## Migration Best Practices

### Gradual Migration

#### Step-by-Step Migration

1. **Phase 1: Basic API Migration**
   - Update import statements
   - Replace configuration access
   - Update factory function calls
   - Fix exception handling

2. **Phase 2: Expert API Integration**
   - Add expert API imports
   - Implement performance optimizations
   - Add visualization support
   - Implement monitoring

3. **Phase 3: Advanced Features**
   - Implement batch processing
   - Add experiment management
   - Implement custom plasticity
   - Add neuromodulation

### Testing Strategy

#### Comprehensive Testing

1. **Unit Testing**
   - Test individual components
   - Mock dependencies
   - Test edge cases

2. **Integration Testing**
   - Test component interactions
   - Test end-to-end workflows
   - Test error conditions

3. **Performance Testing**
   - Test performance characteristics
   - Test scalability
   - Test resource usage

4. **Load Testing**
   - Test system under load
   - Test concurrent access
   - Test failure recovery

### Documentation Updates

#### Documentation Checklist

1. **Update API Documentation**
   - Update function signatures
   - Update parameter descriptions
   - Update return value descriptions
   - Add examples

2. **Update Tutorial Documentation**
   - Update step-by-step instructions
   - Add migration guidance
   - Add troubleshooting sections

3. **Update Configuration Documentation**
   - Update configuration options
   - Add examples
   - Add migration guidance

4. **Update Error Documentation**
   - Update error messages
   - Add error codes
   - Add troubleshooting guidance

## Migration Support

### Support Resources

#### Documentation

- **Migration Guide**: This document
- **API Reference**: `/docs/API_REFERENCE.md`
- **Tutorials**: `/docs/TUTORIAL.md`
- **Performance Guide**: `/docs/PERFORMANCE.md`
- **Debugging Guide**: `/docs/DEBUGGING.md`
- **Examples**: `/examples/`
- **Configuration**: `/configs/`

#### Community Support

- **GitHub Issues**: [GitHub Issues](https://github.com/Kilo-Org/kilocode/issues)
- **Slack Channel**: [NLM Slack](https://slack.kilo.ai)
- **Discord Server**: [NLM Discord](https://discord.kilo.ai)
- **Documentation**: [Documentation](https://github.com/Kilo-Org/kilocode/tree/main/docs)

### Common Issues and Solutions

#### Issue 1: Configuration Migration

**Problem**: Configuration access changed from attribute access to set() method.

**Solution**: 
```python
# Before
config.brain_simulated_neurons = 1000

# After
config.set('brain.simulated_neurons', 1000)
```

#### Issue 2: Factory Function Migration

**Problem**: Factory function calls changed from constructor calls.

**Solution**:
```python
# OBefore
brain = nlm.Brain(config)

# After
brain = nlm.create_brain(config)
```

#### Issue 3: Expert API Migration

**Problem**: Expert API imports and usage changed.

**Solution**:
```python
# Before
import nlm
from nlm import Config, Brain, create_default_config

# After
import nlm
from nlm import create_default_config, create_brain, create_simple_world
from nlm import expert
```

#### Issue 4: Performance Monitoring Migration

**Problem**: Performance monitoring not available in v1.x.

**Solution**:
```python
# Before
# No performance monitoring

# After
monitor = nlm.create_performance_monitor()
controller = nlm.create_simulation_controller()
```

#### Issue 5: Batch Processing Migration

**Problem**: Batch processing not available in v1.x.

**Solution**:
```python
# Before
# No batch processing

# After
batch = nlm.create_batch_processor("simulation_batch")
batch.addOperation(lambda: agent_brain.reset())
batch.execute()
```

## Migration FAQ

### Frequently Asked Questions

#### Q: How long does migration take?

A: Migration time depends on the complexity of your application:
- Small applications: 1-2 days
- Medium applications: 2-3 days
- Large applications: 3-4 days

#### Q: Do I need to rewrite all my code?

A: Most of your code will need to be updated, but the migration is systematic:
- Update import statements
- Update configuration access
- Update factory function calls
- Add expert API support
- Test thoroughly

#### Q: Will my existing functionality break?

A: Yes, the API has changed significantly. However:
- Core functionality is maintained
- Error handling is improved
- Performance is enhanced
- New features are available

#### Q: How can I test my migration?

A: Use the validation tools:
1. Run unit tests
2. Run integration tests
3. Run performance tests
4. Run validation tests

#### Q: What about backward compatibility?

A: Limited backward compatibility is maintained for:
- Exception types
- Core classes
- Basic methods

But the API has changed significantly in v2.0.0.

#### Q: How do I handle expert APIs?

A: Expert APIs are optional enhancements:
- Import expert APIs if needed
- Use performance optimizations if required
- Add visualization if desired

#### Q: What about error handling?

A: Error handling is improved:
- All exceptions inherit from NLMError
- Better error messages
- More detailed error information

### Migration Checklist

#### Pre-Migration Checklist

1. [ ] Assess current NLM usage
2. [ ] Create migration inventory
3. [ ] Set up development environment
4. [ ] Create backup of existing code
5. [ ] Define migration timeline
6. [ ] Assign migration team
7. [ ] Create test environment

#### Migration Checklist

1. [ ] Update import statements
2. [ ] Update configuration access
3. [ ] Update factory function calls
4. [ ] Add expert API support
5. [ ] Update error handling
6. [ ] Add performance monitoring
7. [ ] Test migration
8. [ ] Fix issues
9. [ ] Validate migration
10. [ ] Update documentation

#### Post-Migration Checklist

1. [ ] Run all tests
2. [ ] Validate migration
3. [ ] Generate migration report
4. [ ] Update documentation
5. [ ] Train team
6. [ ] Deploy to production
7. [ ] Monitor performance
8. [ ] Collect feedback

## References

1. **NLM Documentation**: [NLM Documentation](https://github.com/Kilo-Org/kilocode/tree/main/docs)
2. **Expert API Summary**: [Expert API Summary](https://github.com/Kilo-Org/kilocode/tree/main/python/expert/EXPERT_API_SUMMARY.md)
3. **Phase 6 Final Audit**: [Phase 6 Final Audit](https://github.com/Kilo-Org/kilocode/tree/main/docs/PHASE6_FINAL_AUDIT.md)
4. **Migration Tools**: [Migration Tools](https://github.com/Kilo-Org/kilocode/tree/main/migration)

## Acknowledgments

This migration guide was created to help users successfully migrate from NLM v1.x to v2.0.0.

## License

MIT License. See LICENSE file for details.

## Author

NLM Team

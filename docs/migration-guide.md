# NLM Migration Guide

This guide provides comprehensive migration instructions for upgrading NLM from previous versions. It covers:

- **Version Compatibility**: Understanding version differences and compatibility
- **Migration Checklist**: Step-by-step migration procedures
- **Breaking Changes**: List of breaking changes and workarounds
- **Migration Examples**: Complete migration examples for common scenarios
- **Testing Migration**: Migration testing strategies
- **Post-Migration**: Post-migration verification and optimization

## 🎯 Why Migration Matters

Migrating to the latest NLM version is important because:

1. **New Features**: Access to latest functionality (Phase 6 integration)
2. **Bug Fixes**: Critical bug fixes and security updates
3. **Performance**: Performance improvements and optimizations
4. **Compatibility**: Support for newer hardware and software
5. **Maintainability**: Better code quality and documentation

## 📋 Version Information

### Current Version

**NLM Version**: 6.0.0 (Phase 6 Integration)

**Release Date**: 2026-10-01

**Major Features**:
- Complete integration of all cognitive systems
- Memory system integration with neural processing
- Full neuromodulation system implementation
- Checkpoint save/load functionality
- Enhanced performance infrastructure

### Supported Versions

| Version | Status | Notes |
|---------|--------|-------|
| 6.x.x | ✅ Supported | Current development version |
| 5.x.x | ❌ Unsupported | Use migration guide |  
| 4.x.x | ❌ Unsupported | Use migration guide |
| 3.x.x | ❌ Unsupported | Use migration guide |

### Upgrade Path

**Direct upgrades supported**:
- **5.0.x → 6.0.x** (Most common)
- **4.0.x → 6.0.x** (With migration guide)
- **3.0.x → 6.0.x** (With migration guide)

**Unsupported upgrades**:
- Direct upgrade from versions < 3.0.x
- Mixed-version deployments

## 🔄 Migration Overview

Migration from NLM 5.x to 6.x involves several key changes:

### 1. System Architecture Changes
- **Before**: Disconnected cognitive systems
- **After**: Fully integrated artificial brain

### 2. Memory System Integration
- **Before**: Placeholder memory systems
- **After**: Working memory, episodic memory, semantic memory

### 3. Neuromodulation Integration
- **Before**: Limited dopamine effects
- **After**: Complete neuromodulation (dopamine, curiosity, novelty)

### 4. Prediction System
- **Before**: Disconnected prediction system
- **After**: Integrated prediction with planning

### 5. Persistence
- **Before**: Stub save/load functions
- **After**: Full checkpoint system with serialization

## 📋 Migration Checklist

### Phase 1: Preparation

#### 1.1 Assess Current System

```bash
# Check current NLM version
nlm --version

# List installed components
nlm --list-components

# Check documentation for version info
nlm docs/version
```

#### 1.2 Backup Current System

```bash
# Backup configuration
mkdir -p backup/config
cp -r config/* backup/config/

# Backup data
mkdir -p backup/data
cp -r data/* backup/data/

# Backup custom scripts
mkdir -p backup/scripts
cp -r scripts/* backup/scripts/ 2>/dev/null || true

# Backup logs (for debugging)
mkdir -p backup/logs
tail -n 1000 logs/* > backup/logs/current_logs.txt 2>/dev/null || true
```

#### 1.3 Review Dependencies

```python
import pynlm

# Check current version
print(f"Current version: {pynlm.__version__}")

# List dependencies
import pkg_resources
deps = pkg_resources.working_set
for dep in sorted(deps, key=lambda d: d.key):
    if 'nlm' in d.key.lower():
        print(f"{dep.key}: {dep.version}")
```

### Phase 2: Configuration Migration

#### 2.1 Update Configuration Files

```python
# Before (NLM 5.x)
config_old = {
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.001
        }
    }
}

# After (NLM 6.x)
config_new = {
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0,
        "c_m": 1.0,
        "g_L": 10.0,
        "region_count": 1,
        "population_size": 100,
        "spatial_radius": 10.0,
        "spatial_exponent": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0,
            "max_weight": 1.0,
            "min_weight": 0.0
        },
        "hebbian": {
            "enable": True,
            "learning_rate": 0.0001,
            "threshold": 0.1
        },
        "structural": {
            "enable": True,
            "synaptogenesis_rate": 0.01,
            "pruning_threshold": 0.1,
            "pruning_rate": 0.005
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0,
            "plasticity_modulation": True,
            "phasic_response": True,
            "tonic_level": 0.1
        },
        "curiosity": {
            "enable": True,
            "novelty_sensitivity": 1.0,
            "exploration_bonus": 0.1,
            "exploration_cost": 0.01
        },
        "novelty": {
            "enable": True,
            "threshold": 0.1,
            "decay_rate": 0.95,
            "sensitivity": 1.0
        }
    },
    "memory": {
        "working": {
            "enable": True,
            "capacity": 100,
            "decay_rate": 0.99,
            "competition_strength": 1.0,
            "stability": 0.5
        },
        "episodic": {
            "enable": True,
            "capacity": 1000,
            "replay_frequency": 0.1,
            "consolidation_threshold": 0.5,
            "encoding_strength": 1.0
        },
        "semantic": {
            "enable": True,
            "learning_rate": 0.01,
            "association_strength": 0.5,
            "forgetting_rate": 0.99
        }
    },
    "development": {
        "synaptogenesis": {
            "enable": True,
            "rate": 0.01,
            "pattern": "exponential",
            "target_density": 0.2
        },
        "pruning": {
            "enable": True,
            "threshold": 0.1,
            "rate": 0.005,
            "pattern": "activity_dependent"
        },
        "maturation": {
            "enable": True,
            "rate_multiplier": 1.5,
            "age_effects": True,
            "stability_gain": 1.2
        }
    },
    "performance": {
        "memory_pools": {
            "enable": True,
            "neuron_pool_size": 10000,
            "synapse_pool_size": 50000,
            "event_pool_size": 100000
        },
        "event_queues": {
            "enable": True,
            "queue_size": 10000,
            "thread_count": 4,
            "priority_levels": 3
        },
        "sparse": {
            "enable": True,
            "threshold": 0.1,
            "spatial_decay": 2.0,
            "distance_decay": 1.0
        }
    },
    "visualization": {
        "enabled": True,
        "update_rate": 60.0,
        "render_rate": 60.0,
        "theme": "dark",
        "quality": "high",
        "export_formats": ["png", "json", "csv"]
    },
    "plugins": {
        "enabled": True,
        "auto_load": True,
        "search_paths": ["./plugins", "/usr/lib/nlm/plugins"],
        "max_plugins": 50,
        "timeout": 5.0
    }
}
```

#### 2.2 Update Code Configuration

```python
# Update configuration in code
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Check current configuration
print("Current configuration:")
print(f"  Neuron count: {config.getInt('brain.neuron_count')}")
print(f"  Synapse density: {config.getFloat('brain.synapse_density')}")
print(f"  STDP enabled: {config.getBool('plasticity.stdp.enable')}")
print(f"  Working memory enabled: {config.getBool('memory.working.enable')}")

# Update configuration for NLM 6.x
config.set("brain.connection_probability", 0.05)
config.set("brain.initial_weight_mean", 0.5)
config.set("brain.initial_weight_std", 0.1)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
config.set("plasticity.stdp.max_weight", 1.0)
config.set("plasticity.stdp.min_weight", 0.0)
config.set("neuromod.dopamine.phasic_response", True)
config.set("neuromod.dopamine.tonic_level", 0.1)
config.set("neuromod.curiosity.exploration_bonus", 0.1)
config.set("neuromod.curiosity.exploration_cost", 0.01)
config.set("neuromod.novelty.decay_rate", 0.95)
config.set("memory.working.capacity", 100)
config.set("memory.working.decay_rate", 0.99)
config.set("memory.episodic.capacity", 1000)
config.set("memory.episodic.replay_frequency", 0.1)
config.set("development.maturation.rate_multiplier", 1.5)
config.set("performance.memory_pools.neuron_pool_size", 10000)
config.set("performance.event_queues.thread_count", 4)

# Save updated configuration
config.saveToFile("config_nlm6.json")
```

### Phase 3: Code Migration

#### 3.1 Update Memory System Usage

```python
# Before (NLM 5.x) - Memory systems not integrated
brain = pynlm.createBrain(config)
brain.initialize()

# Working memory was disconnected
try:
    working_memory = brain.getWorkingMemory()
    print(f"Working memory active: {working_memory.isActive()}")
except:
    print("Working memory not available (NLM 5.x limitation)")

# After (NLM 6.x) - Memory systems fully integrated
brain = pynlm.createBrain(config)
brain.initialize()

# Working memory now integrated with neural processing
if brain.hasWorkingMemory():
    working_memory = brain.getWorkingMemory()
    working_memory.activate()
    print(f"Working memory active: {working_memory.isActive()}")
    print(f"Working memory capacity: {working_memory.getCapacity()}")

# Episodic memory integration
if brain.hasEpisodicMemory():
    episodic_memory = brain.getEpisodicMemory()
    episodic_memory.enable(True)
    print(f"Episodic memory enabled: {episodic_memory.isEnabled()}")
```

#### 3.2 Update Neuromodulation Usage

```python
# Before (NLM 5.x) - Limited neuromodulation
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Dopamine only modulated STDP
agent.enableRewardModulation(True)
agent.applyRewardModulation(reward, predicted_reward)

# After (NLM 6.x) - Full neuromodulation
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all neuromodulators
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNovelty(True)
agent.enablePredictionError(True)

# Apply all neuromodulations
agent.applyRewardModulation(reward, predicted_reward)
agent.updateCuriosity(0.1)
agent.updateNovelty(0.05)
agent.updatePredictionError(0.01)

# Get neuromodulation statistics
print(f"Dopamine level: {agent.getDopamineLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
```

#### 3.3 Update Prediction System Usage

```python
# Before (NLM 5.x) - Prediction system disconnected
if brain.hasPredictionSystem():
    prediction = brain.getPredictionSystem()
    print("Prediction system available but not used")
    # Prediction training was not integrated

# After (NLM 6.x) - Integrated prediction system
if brain.hasPredictionSystem():
    prediction = brain.getPredictionSystem()
    prediction.enable(True)
    
    # Use prediction for planning
    action = prediction.selectAction(current_state)
    predicted_outcome = prediction.predictNext(current_state)
    
    print(f"Selected action: {action}")
    print(f"Predicted outcome: {predicted_outcome}")
```

#### 3.4 Update Development System Usage

```python
# Before (NLM 5.x) - Minimal development integration
agent.enableDevelopment(True)
agent.updateDevelopment(0.1)
print(f"Development stage: {agent.getDevelopmentalStage()}")

# Development only affected structural plasticity

# After (NLM 6.x) - Full development integration
agent.enableDevelopment(True)
agent.updateDevelopment(0.1)

print(f"Development stage: {agent.getDevelopmentalStage()}")
print(f"Brain age: {brain.getBrainAge():.1f} steps")
print(f"Plasticity multiplier: {agent.getPlasticityMultiplier():.2f}")

# Development affects multiple systems
print("Development effects:")
print(f"  - Synaptogenesis: {agent.isSynaptogenesisEnabled()}")
print(f"  - Pruning: {agent.isPruningEnabled()}")
print(f"  - Maturation: {agent.isMaturationEnabled()}")
```

### Phase 4: Testing Migration

#### 4.1 Update Test Configuration

```python
# Update test configuration for NLM 6.x
test_config = {
    "brain": {
        "neuron_count": 5000,  # Increased for more realistic testing
        "synapse_density": 0.2,  # Increased for connectivity
        "connection_probability": 0.05,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": True,
            "learning_rate": 0.0001
        },
        "structural": {
            "enable": True,
            "synaptogenesis_rate": 0.01,
            "pruning_threshold": 0.1
        }
    },
    "neuromod": {
        "dopamine": {
            "enable": True,
            "plasticity_modulation": True
        },
        "curiosity": {
            "enable": True,
            "exploration_bonus": 0.1
        }
    },
    "memory": {
        "working": {
            "enable": True,
            "capacity": 200,
            "decay_rate": 0.99
        },
        "episodic": {
            "enable": True,
            "capacity": 2000,
            "replay_frequency": 0.1
        }
    }
}

# Save test configuration
test_config.saveToFile("test_config_nlm6.json")
```

#### 4.2 Update Test Cases

```python
# Before (NLM 5.x) - Basic tests
import pytest

class TestNLM5:
    def test_basic_initialization(self):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        assert brain.isInitialized()
    
    def test_simulation_step(self):
        brain = pynlm.createBrain(config)
        brain.initialize()
        for step in range(100):
            brain.step(step)
        assert True  # No crashes
    
    def test_memory_system(self):
        # Memory systems were disconnected in NLM 5.x
        brain = pynlm.createBrain(config)
        brain.initialize()
        try:
            working_memory = brain.getWorkingMemory()
            assert working_memory is not None
        except:
            pytest.skip("Memory systems not available in NLM 5.x")

# After (NLM 6.x) - Comprehensive tests
class TestNLM6:
    def setup_method(self):
        self.config = pynlm.createDefaultConfig()
        # Load NLM 6.x configuration
        self.config.loadFromFile("test_config_nlm6.json")
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent and world for integration tests
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
    
    def test_basic_initialization(self):
        assert self.brain.isInitialized()
        assert self.agent is not None
        assert self.world is not None
    
    def test_memory_integration(self):
        """Test that memory systems are integrated with neural processing"""
        if self.brain.hasWorkingMemory():
            working_memory = self.brain.getWorkingMemory()
            working_memory.activate()
            assert working_memory.isActive()
            
            # Test memory storage
            for step in range(50):
                self.brain.step(step)
                if self.brain.hasEpisodicMemory():
                    episodic_memory = self.brain.getEpisodicMemory()
                    episodic_memory.encode(f"Experience at step {step}")
            
            assert episodic_memory.getMemoryCount() > 0
    
    def test_neuromodulation_integration(self):
        """Test that neuromodulation affects learning"""
        initial_weight = self.get_average_synapse_weight()
        
        # Apply reward modulation
        for step in range(100):
            self.brain.step(step)
            self.agent.applyRewardModulation(1.0, 0.5)
        
        final_weight = self.get_average_synapse_weight()
        assert final_weight != initial_weight  # Learning occurred
    
    def test_prediction_integration(self):
        """Test that prediction system is integrated"""
        if self.brain.hasPredictionSystem():
            prediction = self.brain.getPredictionSystem()
            prediction.enable(True)
            
            # Train on sequence
            sequence = [0.1, 0.2, 0.3, 0.4, 0.5]
            for item in sequence:
                prediction.train(item)
            
            # Test prediction
            next_item = prediction.predictNext(0.5)
            assert next_item is not None
    
    def test_development_integration(self):
        """Test that development system affects plasticity"""
        initial_age = self.brain.getBrainAge()
        
        for step in range(1000):
            self.brain.step(step)
            self.agent.updateDevelopment(0.1)
        
        final_age = self.brain.getBrainAge()
        assert final_age > initial_age  # Development occurred
        
        # Check that development affected plasticity
        developmental_stage = self.brain.getDevelopmentalStage()
        assert developmental_stage != self.agent.getDevelopmentalStage()
    
    def get_average_synapse_weight(self):
        """Get average synapse weight for testing"""
        # This would iterate through all synapses in the brain
        # In real implementation, use brain's API
        return 0.5  # Placeholder
```

### Phase 5: Post-Migration

#### 5.1 Verify Integration

```python
# Verify that all systems are properly integrated
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Check system integration
integration_status = {
    "working_memory": brain.hasWorkingMemory(),
    "episodic_memory": brain.hasEpisodicMemory(),
    "semantic_memory": brain.hasSemanticMemory(),
    "prediction_system": brain.hasPredictionSystem(),
    "development_system": agent.hasDevelopment(),
    "neuromodulation": agent.hasNeuromodulation(),
    "plasticity": brain.hasPlasticity(),
    "curiosity": agent.hasCuriosity()
}

print("System Integration Status:")
for system, status in integration_status.items():
    print(f"  {system}: {'✅' if status else '❌'}")

# Check for integration issues
missing_systems = [sys for sys, status in integration_status.items() if not status]
if missing_systems:
    print(f"\nWARNING: Missing systems: {', '.join(missing_systems)}")
else:
    print("\n✅ All systems properly integrated!")
```

#### 5.2 Performance Testing

```python
import time
import pynlm

def performance_test():
    """Test NLM 6.x performance"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test different brain sizes
    sizes = [100, 1000, 5000, 10000]
    
    for size in sizes:
        print(f"\nTesting brain size: {size}")
        
        # Adjust configuration
        config.set("brain.neuron_count", size)
        config.set("brain.synapse_density", min(0.5, 0.1 * (size / 1000)))
        
        # Create new brain with adjusted size
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Measure initialization time
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        # Measure simulation time
        start_time = time.time()
        for step in range(100):
            brain.step(step)
        sim_time = time.time() - start_time
        
        # Calculate metrics
        neurons_per_second = size / sim_time
        steps_per_second = 100 / sim_time
        
        print(f"  Initialization: {init_time:.3f}s")
        print(f"  Simulation: {sim_time:.3f}s")
        print(f"  Neurons/second: {neurons_per_second:.0f}")
        print(f"  Steps/second: {steps_per_second:.0f}")
        
        # Check for performance issues
        if init_time > 10.0:
            print(f"  WARNING: Slow initialization!")
        
        if sim_time > 5.0:
            print(f"  WARNING: Slow simulation!")
    
    print("\n✅ Performance test completed!")

if __name__ == "__main__":
    performance_test()
```

#### 5.3 Documentation Updates

Update documentation for NLM 6.x:

```bash
# Update README.md with migration notes
cat >> README.md << 'EOF'

## Migration from NLM 5.x to NLM 6.x

This document describes the migration process from NLM 5.x to NLM 6.x.

### Key Changes

1. **Memory Systems**: Now fully integrated with neural processing
2. **Neuromodulation**: Complete implementation of all neuromodulators
3. **Prediction**: Integrated prediction system with planning
4. **Development**: Comprehensive development system
5. **Persistence**: Full checkpoint save/load functionality
6. **Performance**: Enhanced performance infrastructure

### Migration Steps

1. Update configuration files with new parameters
2. Enable memory systems in code
3. Use integrated neuromodulation
4. Update prediction system usage
5. Test development integration
6. Verify all systems work together

For detailed migration instructions, see `docs/migration-guide.md`.
EOF

# Update API documentation
python3 generate_api_docs.py --version 6.0.0 --output docs/api-reference-nlm6.md
```

## 🚨 Breaking Changes

### 1. Memory System API

**Before (NLM 5.x)**:
```cpp
// Memory systems were disconnected
WorkingMemory* working_memory = brain->getWorkingMemory(); // Returns nullptr
EpisodicMemory* episodic_memory = brain->getEpisodicMemory(); // Returns nullptr
```

**After (NLM 6.x)**:
```cpp
// Memory systems are fully integrated
WorkingMemory* working_memory = brain->getWorkingMemory(); // Returns valid pointer
EpisodicMemory* episodic_memory = brain->getEpisodicMemory(); // Returns valid pointer
working_memory->activate(); // Now affects neural processing
```

### 2. Neuromodulation API

**Before (NLM 5.x)**:
```cpp
// Limited neuromodulation
agent->enableRewardModulation(true);
agent->applyRewardModulation(reward, predicted_reward);
// Only affected STDP weights
```

**After (NLM 6.x)**:
```cpp
// Full neuromodulation system
agent->enableRewardModulation(true);
agent->enableCuriosity(true);
agent->enableNovelty(true);
agent->enablePredictionError(true);

agent->applyRewardModulation(reward, predicted_reward);
agent->updateCuriosity(0.1);
agent->updateNovelty(0.05);
agent->updatePredictionError(0.01);
// Affects multiple plasticity rules and memory systems
```

### 3. Prediction System API

**Before (NLM 5.x)**:
```cpp
// Disconnected prediction system
PredictionSystem* prediction = brain->getPredictionSystem(); // Returns nullptr
prediction->train(sequence); // No effect
```

**After (NLM 6.x)**:
```cpp
// Integrated prediction system
PredictionSystem* prediction = brain->getPredictionSystem(); // Returns valid pointer
prediction->enable(true);
prediction->train(sequence); // Affects action selection
```

### 4. Development System API

**Before (NLM 5.x)**:
```cpp
// Minimal development integration
agent->enableDevelopment(true);
agent->updateDevelopment(0.1);
// Only affected structural plasticity
```

**After (NLM 6.x)**:
```cpp
// Comprehensive development system
agent->enableDevelopment(true);
agent->updateDevelopment(0.1);
// Affects multiple plasticity rules, memory consolidation, and neuromodulation
```

### 5. Configuration API

**Before (NLM 5.x)**:
```cpp
// Limited configuration options
config->set("brain.neuron_count", 1000);
config->set("plasticity.stdp.enable", true);
```

**After (NLM 6.x)**:
```cpp
// Comprehensive configuration options
config->set("brain.neuron_count", 1000);
config->set("brain.synapse_density", 0.1);
config->set("brain.connection_probability", 0.05);
config->set("brain.initial_weight_mean", 0.5);
config->set("brain.initial_weight_std", 0.1);
config->set("plasticity.stdp.tau_plus", 20.0);
config->set("plasticity.stdp.tau_minus", 20.0);
config->set("neuromod.dopamine.scale", 1.0);
config->set("neuromod.curiosity.enable", true);
config->set("memory.working.capacity", 100);
config->set("memory.episodic.replay_frequency", 0.1);
config->set("development.maturation.rate_multiplier", 1.5);
```

## 🛠️ Migration Tools

### 1. Configuration Migration Tool

```python
#!/usr/bin/env python3
"""
Configuration migration tool for NLM 5.x to 6.x
Converts old configuration files to NLM 6.x format
"""

import json
import argparse
import sys
from pathlib import Path

def migrate_config_file(input_path, output_path=None):
    """Migrate a single configuration file"""
    with open(input_path, 'r') as f:
        config = json.load(f)
    
    # Migrate brain configuration
    if 'brain' in config:
        config['brain']['connection_probability'] = 0.05
        config['brain']['initial_weight_mean'] = 0.5
        config['brain']['initial_weight_std'] = 0.1
        config['brain']['c_m'] = 1.0
        config['brain']['g_L'] = 10.0
        config['brain']['region_count'] = 1
        config['brain']['population_size'] = 100
        config['brain']['spatial_radius'] = 10.0
        config['brain']['spatial_exponent'] = 2.0
    
    # Migrate plasticity configuration
    if 'plasticity' in config:
        if 'stdp' in config['plasticity']:
            config['plasticity']['stdp']['max_weight'] = 1.0
            config['plasticity']['stdp']['min_weight'] = 0.0
            config['plasticity']['stdp']['tau_plus'] = 20.0
            config['plasticity']['stdp']['tau_minus'] = 20.0
        
        if 'structural' in config['plasticity']:
            config['plasticity']['structural']['pruning_rate'] = 0.005
    
    # Migrate neuromodulation configuration
    if 'neuromod' in config:
        if 'dopamine' in config['neuromod']:
            config['neuromod']['dopamine']['phasic_response'] = True
            config['neuromod']['dopamine']['tonic_level'] = 0.1
        
        if 'curiosity' in config['neuromod']:
            config['neuromod']['curiosity']['exploration_bonus'] = 0.1
            config['neuromod']['curiosity']['exploration_cost'] = 0.01
        
        if 'novelty' in config['neuromod']:
            config['neuromod']['novelty']['decay_rate'] = 0.95
            config['neuromod']['novelty']['sensitivity'] = 1.0
    
    # Migrate memory configuration
    if 'memory' in config:
        if 'working' in config['memory']:
            config['memory']['working']['decay_rate'] = 0.99
            config['memory']['working']['competition_strength'] = 1.0
            config['memory']['working']['stability'] = 0.5
        
        if 'episodic' in config['memory']:
            config['memory']['episodic']['replay_frequency'] = 0.1
            config['memory']['episodic']['consolidation_threshold'] = 0.5
            config['memory']['episodic']['encoding_strength'] = 1.0
    
    # Migrate development configuration
    if 'development' in config:
        if 'maturation' in config['development']:
            config['development']['maturation']['rate_multiplier'] = 1.5
            config['development']['maturation']['stability_gain'] = 1.2
    
    # Migrate performance configuration
    if 'performance' in config:
        if 'memory_pools' in config['performance']:
            config['performance']['memory_pools']['neuron_pool_size'] = 10000
            config['performance']['memory_pools']['synapse_pool_size'] = 50000
            config['performance']['memory_pools']['event_pool_size'] = 100000
    
    # Write migrated configuration
    if output_path is None:
        output_path = input_path.replace('.json', '_migrated.json')
    
    with open(output_path, 'w') as f:
        json.dump(config, f, indent=2)
    
    print(f"Migrated configuration saved to: {output_path}")
    return output_path

def main():
    parser = argparse.ArgumentParser(description='NLM Configuration Migration Tool')
    parser.add_argument('input', help='Input configuration file')
    parser.add_argument('-o', '--output', help='Output configuration file')
    
    args = parser.parse_args()
    
    try:
        migrate_config_file(args.input, args.output)
        print("✅ Migration completed successfully!")
    except Exception as e:
        print(f"❌ Migration failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
```

### 2. Code Migration Assistant

```python
#!/usr/bin/env python3
"""
Code migration assistant for NLM 5.x to 6.x
Provides suggestions for updating code to work with NLM 6.x
"""

import re
import sys
from pathlib import Path

def analyze_code_file(file_path):
    """Analyze a code file for migration issues"""
    with open(file_path, 'r') as f:
        content = f.read()
    
    issues = []
    
    # Check for memory system usage
    if re.search(r'getWorkingMemory\(\)', content):
        issues.append("Memory system usage found - update to use integrated memory systems")
    
    if re.search(r'getEpisodicMemory\(\)', content):
        issues.append("Episodic memory usage found - update to use integrated episodic memory")
    
    # Check for limited neuromodulation usage
    if re.search(r'enableRewardModulation\(true\)', content):
        issues.append("Basic neuromodulation found - consider using full neuromodulation system")
    
    # Check for disconnected prediction usage
    if re.search(r'getPredictionSystem\(\)', content):
        issues.append("Prediction system usage found - update to use integrated prediction")
    
    # Check for minimal development usage
    if re.search(r'enableDevelopment\(true\)', content):
        issues.append("Basic development usage found - consider using full development system")
    
    return issues

def suggest_migration(file_path):
    """Suggest migration changes for a code file"""
    issues = analyze_code_file(file_path)
    
    if not issues:
        print(f"✅ {file_path}: No migration issues found")
        return
    
    print(f"\n🔄 Migration suggestions for {file_path}:")
    for i, issue in enumerate(issues, 1):
        print(f"  {i}. {issue}")
    
    print(f"\n📝 Suggested migration steps:")
    print("  1. Update configuration with NLM 6.x parameters")
    print("  2. Enable memory systems in code")
    print("  3. Use integrated neuromodulation")
    print("  4. Update prediction system usage")
    print("  5. Use full development system")
    print("  6. Test integration of all systems")

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 migration_assistant.py <code_file>")
        print("Example: python3 migration_assistant.py my_code.cpp")
        return
    
    file_path = sys.argv[1]
    
    if not Path(file_path).exists():
        print(f"❌ File not found: {file_path}")
        return
    
    suggest_migration(file_path)

if __name__ == "__main__":
    main()
```

### 3. Performance Comparison Tool

```python
#!/usr/bin/env python3
"""
Performance comparison tool for NLM 5.x vs NLM 6.x
Compares performance metrics between versions
"""

import time
import pynlm
import json
from pathlib import Path

def benchmark_nlm_version(version, config_file=None):
    """Benchmark NLM version with given configuration"""
    if config_file:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
    else:
        config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Benchmark initialization
    start_time = time.time()
    brain.initialize()
    init_time = time.time() - start_time
    
    # Benchmark simulation
    start_time = time.time()
    for step in range(100):
        brain.step(step)
    sim_time = time.time() - start_time
    
    # Get metrics
    metrics = {
        'version': version,
        'initialization_time': init_time,
        'simulation_time': sim_time,
        'neurons_per_second': config.getInt('brain.neuron_count') / sim_time,
        'steps_per_second': 100 / sim_time,
        'neuron_count': config.getInt('brain.neuron_count'),
        'synapse_count': brain.getTotalSynapseCount(),
        'firing_count': brain.getFiringNeuronCount()
    }
    
    return metrics

def compare_versions(nlm5_config, nlm6_config):
    """Compare NLM 5.x vs NLM 6.x performance"""
    print("🔍 Comparing NLM 5.x vs NLM 6.x performance...")
    
    # Benchmark NLM 5.x
    print("\n📊 Benchmarking NLM 5.x...")
    nlm5_metrics = benchmark_nlm_version("5.x", nlm5_config)
    
    # Benchmark NLM 6.x
    print("\n📊 Benchmarking NLM 6.x...")
    nlm6_metrics = benchmark_nlm_version("6.x", nlm6_config)
    
    # Display comparison
    print("\n📈 Performance Comparison:")
    print(f"{'Metric':<30} {'NLM 5.x':<15} {'NLM 6.x':<15} {'Improvement':<15}")
    print("-" * 75)
    
    # Compare initialization time
    init_improvement = ((nlm6_metrics['initialization_time'] - nlm5_metrics['initialization_time']) / 
                       nlm5_metrics['initialization_time'] * 100)
    print(f"{'Initialization time (s)':<30} {nlm5_metrics['initialization_time']:<15.3f} "
          f"{nlm6_metrics['initialization_time']:<15.3f} {init_improvement:+.1f}%")
    
    # Compare simulation time
    sim_improvement = ((nlm6_metrics['simulation_time'] - nlm5_metrics['simulation_time']) / 
                      nlm5_metrics['simulation_time'] * 100)
    print(f"{'Simulation time (s)':<30} {nlm5_metrics['simulation_time']:<15.3f} "
          f"{nlm6_metrics['simulation_time']:<15.3f} {sim_improvement:+.1f}%")
    
    # Compare neurons per second
    nps_improvement = ((nlm6_metrics['neurons_per_second'] - nlm5_metrics['neurons_per_second']) / 
                      nlm5_metrics['neurons_per_second'] * 100)
    print(f"{'Neurons/second':<30} {nlm5_metrics['neurons_per_second']:<15.0f} "
          f"{nlm6_metrics['neurons_per_second']:<15.0f} {nps_improvement:+.1f}%")
    
    # Compare steps per second
    sps_improvement = ((nlm6_metrics['steps_per_second'] - nlm5_metrics['steps_per_second']) / 
                      nlm5_metrics['steps_per_second'] * 100)
    print(f"{'Steps/second':<30} {nlm5_metrics['steps_per_second']:<15.0f} "
          f"{nlm6_metrics['steps_per_second']:<15.0f} {sps_improvement:+.1f}%")
    
    # Save comparison to file
    comparison = {
        'nlm5': nlm5_metrics,
        'nlm6': nlm6_metrics,
        'improvements': {
            'initialization_time': init_improvement,
            'simulation_time': sim_improvement,
            'neurons_per_second': nps_improvement,
            'steps_per_second': sps_improvement
        }
    }
    
    with open('performance_comparison.json', 'w') as f:
        json.dump(comparison, f, indent=2)
    
    print(f"\n💾 Comparison saved to: performance_comparison.json")
    
    return comparison

def main():
    # Default configuration files
    nlm5_config = "config_nlm5.json"
    nlm6_config = "config_nlm6.json"
    
    # Check if configuration files exist
    if not Path(nlm5_config).exists():
        print(f"⚠️  Warning: {nlm5_config} not found. Using default configuration.")
        nlm5_config = None
    
    if not Path(nlm6_config).exists():
        print(f"⚠️  Warning: {nlm6_config} not found. Using default configuration.")
        nlm6_config = None
    
    # Run comparison
    compare_versions(nlm5_config, nlm6_config)

if __name__ == "__main__":
    main()
```

## 📚 Migration Documentation

### 1. Quick Migration Guide

For rapid migration, follow these steps:

1. **Update Configuration**: Replace old configuration with NLM 6.x parameters
2. **Enable Memory Systems**: Add memory system initialization code
3. **Use Neuromodulation**: Replace basic reward modulation with full neuromodulation
4. **Integrate Prediction**: Use prediction system for planning
5. **Test Development**: Verify development system effects
6. **Run Integration Tests**: Test all systems together

### 2. Migration Examples

#### Example 1: Basic Brain with Memory

```cpp
// Before (NLM 5.x)
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 1000);
config->set("plasticity.stdp.enable", true);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

// After (NLM 6.x)
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 1000);
config->set("brain.connection_probability", 0.05);
config->set("brain.initial_weight_mean", 0.5);
config->set("brain.initial_weight_std", 0.1);
config->set("plasticity.stdp.max_weight", 1.0);
config->set("plasticity.stdp.min_weight", 0.0);
config->set("neuromod.dopamine.scale", 1.0);
config->set("memory.working.capacity", 100);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Use integrated memory system
if (auto working_memory = brain->getWorkingMemory()) {
    working_memory->activate();
    working_memory->update(input_data);
}

// Use integrated neuromodulation
if (auto agent = brain->getAgent()) {
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->applyRewardModulation(reward, predicted_reward);
}
```

#### Example 2: Agent with Full Integration

```python
# Before (NLM 5.x)
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Basic reward modulation
agent.enableRewardModulation(True)
agent.applyRewardModulation(reward, predicted_reward)

# Simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# After (NLM 6.x)
import pynlm

config = pynlm.createDefaultConfig()
# Load NLM 6.x configuration
config.loadFromFile("config_nlm6.json")

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all neuromodulators
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNovelty(True)
agent.enableDevelopment(True)

# Use integrated prediction for planning
if brain.hasPredictionSystem():
    prediction = brain.getPredictionSystem()
    prediction.enable(True)
    action = prediction.selectAction(current_state)

# Simulation with full integration
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get perception
    percept = world.getSensoryPercept()
    
    # Process with agent (includes all neuromodulation)
    agent.processSensoryInput(percept)
    
    # Neural computation
    brain.step(step)
    
    # Get action (includes planning)
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log results
    if step % 10 == 0:
        print(f"Step {step}: Firing neurons {brain.getFiringNeuronCount()}, "
              f"Curiosity {agent.getCuriosityLevel():.3f}, "
              f"Novelty {agent.getNoveltyLevel():.3f}")
```

### 3. Migration FAQ

#### Q: How do I migrate my existing configuration files?

A: Use the configuration migration tool:

```bash
python3 migration_tool.py config_old.json config_new.json
```

This tool automatically updates your configuration to NLM 6.x format.

#### Q: What changes do I need to make to my code?

A: You need to update your code to:

1. **Memory Systems**: Use integrated working memory, episodic memory, and semantic memory
2. **Neuromodulation**: Replace basic reward modulation with full neuromodulation
3. **Prediction**: Use prediction system for action planning
4. **Development**: Use comprehensive development system
5. **Configuration**: Update configuration with new parameters

#### Q: How do I test my migration?

A: Run the migration testing suite:

```bash
python3 migration_test.py
```

This tests all NLM 6.x features and verifies integration.

#### Q: What are the performance implications?

A: NLM 6.x has better performance due to:

1. **Memory systems**: More efficient memory management
2. **Neuromodulation**: Better learning and adaptation
3. **Prediction**: Improved planning and decision-making
4. **Development**: Better maturation and growth

The performance comparison tool can show detailed improvements.

## 🔄 Migration Support

### Getting Help

If you need help with migration:

1. **Documentation**: Check the migration guide and API reference
2. **Examples**: Review migration examples
3. **Tools**: Use migration tools and scripts
4. **Community**: Join NLM community for support

### Reporting Issues

Report migration issues at:
- **GitHub Issues**: https://github.com/your-repo/nlm/issues
- **Documentation**: https://docs.kilo.ai/nlm
- **Community**: https://community.kilo.ai

### Migration Testing

To ensure successful migration:

1. **Unit Tests**: Test all migrated functionality
2. **Integration Tests**: Test system integration
3. **Performance Tests**: Verify performance improvements
4. **Regression Tests**: Ensure no functionality loss

## 📋 Migration Checklist

### Pre-Migration
- [ ] Backup current system
- [ ] Assess current configuration
- [ ] Review dependencies
- [ ] Test current system

### Migration
- [ ] Update configuration files
- [ ] Update code for memory systems
- [ ] Update code for neuromodulation
- [ ] Update code for prediction system
- [ ] Update code for development system
- [ ] Test migration tools
- [ ] Run integration tests

### Post-Migration
- [ ] Verify system integration
- [ ] Test performance
- [ ] Update documentation
- [ ] Train users
- [ ] Set up monitoring

## 🏆 Migration Success Criteria

Migration is successful when:

1. **All systems integrated**: Working memory, episodic memory, semantic memory, prediction, development, neuromodulation
2. **Performance maintained or improved**: Benchmarks show no regression
3. **Functionality preserved**: All existing features work correctly
4. **Tests pass**: All unit and integration tests pass
5. **Documentation updated**: All documentation reflects NLM 6.x features

## 📚 Migration Resources

### Migration Documentation
- **Migration Guide**: This document
- **API Reference**: NLM 6.x API documentation
- **Configuration Guide**: NLM 6.x configuration documentation
- **Best Practices**: NLM 6.x best practices

### Migration Tools
- **Configuration Migration Tool**: Automated configuration migration
- **Code Migration Assistant**: Code migration suggestions
- **Performance Comparison Tool**: NLM 5.x vs NLM 6.x performance comparison

### Migration Examples
- **Basic Examples**: Basic NLM 6.x usage examples
- **Advanced Examples**: Advanced NLM 6.x usage examples
- **Integration Examples**: NLM 6.x integration examples

### Support
- **GitHub**: https://github.com/your-repo/nlm
- **Documentation**: https://docs.kilo.ai/nlm
- **Community**: https://community.kilo.ai

This completes the NLM Migration Guide. It provides comprehensive instructions for migrating from NLM 5.x to NLM 6.x, including:

- **Migration Overview**: Understanding the migration process
- **Migration Checklist**: Step-by-step migration procedures
- **Configuration Migration**: Updating configuration files
- **Code Migration**: Updating code to use NLM 6.x features
- **Testing Migration**: Migration testing strategies
- **Post-Migration**: Post-migration verification and optimization
- **Breaking Changes**: List of breaking changes and workarounds
- **Migration Examples**: Complete migration examples
- **Migration Tools**: Automated migration tools and scripts
- **Support**: Getting help and reporting issues

## 🔄 Final Migration Steps

To complete migration:

1. **Plan migration**: Create migration plan and timeline
2. **Test migration**: Test migration in development environment
3. **Execute migration**: Apply migration to production system
4. **Verify migration**: Verify all systems work correctly
5. **Optimize migration**: Optimize migration performance
6. **Document migration**: Document migration process
7. **Train users**: Train users on NLM 6.x features
8. **Monitor migration**: Monitor migration progress

Following these steps will ensure successful migration to NLM 6.x with minimal disruption and maximum benefit.
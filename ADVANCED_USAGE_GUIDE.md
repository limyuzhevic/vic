# NLM Advanced Usage Guide

This guide covers advanced usage scenarios for the NLM (Neural Learning Machine) Phase 6 system.

## Advanced Configuration Management

### JSON/YAML Configuration Support
NLM now supports advanced configuration file formats:

**JSON Example:**
```json
{
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.2
    },
    "plasticity": {
        "stdp": { "enable": true, "learning_rate": 0.001 }
    }
}
```

**Advanced Configuration Patterns:**
- **Developmental Configuration**: Configure different developmental stages with varying plasticity
- **Multi-Agent Setup**: Shared world configuration with multiple agents
- **Performance Profiling**: Built-in memory and CPU profiling support

### Key Features:
1. JSON/YAML file format support
2. Advanced parameter validation
3. Configuration inheritance and merging
4. Runtime configuration updates

## Complex Agent Development

### Developmental Stage Management
```python
import pynlm

# Create developmental agent
brain, agent, world = pynlm.createAgentSetup(32)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Custom developmental schedule
developmental_schedule = [
    (0, 60.0, 1.0),    # Initial: high plasticity
    (60, 300.0, 0.8),   # Critical period: moderate plasticity
    (300, 900.0, 0.5),  # Maturation: lower plasticity
]

for step in range(5000):
    current_time = step * 0.001
    
    # Determine developmental stage
    plasticity_modifier = 1.0
    for start_time, end_time, modifier in developmental_schedule:
        if start_time <= current_time < end_time:
            plasticity_modifier = modifier
            break
    
    agent.updateDevelopment(0.1)
    world.update(0.1)
    
    # Agent behavior depends on developmental stage
    if plasticity_modifier > 0.7:
        # Critical period: exploration
        agent.enableCuriosity(True)
    else:
        # Adult: exploitation
        agent.enableCuriosity(False)
```

### Neuromodulation Configuration
```python
import pynlm

brain, agent, world = pynlm.createAgentSetup(20)

# Custom neuromodulation parameters
class CustomNeuromodulation:
    def __init__(self):
        self.dopamine_sensitivity = 1.0
        self.curiosity_threshold = 0.3
        self.novelty_decay = 0.95
        self.prediction_error_weight = 0.5
    
    def apply_custom_modulation(self, agent, reward, novelty):
        # Custom dopamine response based on reward prediction error
        prediction_error = reward - agent.getPredictionError()
        dopamine = prediction_error * self.dopamine_sensitivity
        
        # Modulate curiosity based on novelty and prediction error
        curiosity_boost = novelty * 0.5 + abs(prediction_error) * 0.3
        modified_curiosity = min(1.0, agent.getCuriosityLevel() + curiosity_boost)
        
        # Apply modified neuromodulation
        agent.applyRewardModulation(dopamine, prediction_error)
        
        # Custom curiosity regulation
        if agent.getCuriosityLevel() > self.curiosity_threshold:
            # Reduce curiosity when too high to prevent over-exploration
            agent.enableCuriosity(False)
        else:
            agent.enableCuriosity(True)
        
        return dopamine, modified_curiosity

# Use custom neuromodulation
neuromod = CustomNeuromodulation()

for step in range(3000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Calculate novelty from sensory input
    novelty = calculateNovelty(percept)
    
    # Get reward from world
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    
    # Apply custom neuromodulation
    dopamine, curiosity = neuromod.apply_custom_modulation(agent, reward, novelty)
    
    brain.step(step)
    
    if step % 300 == 0:
        print(f"Step {step}: Dopamine={dopamine:.2f}, Curiosity={curiosity:.2f}")
```

## Advanced Simulation Techniques

### Batch Processing
```python
import pynlm
from concurrent.futures import ThreadPoolExecutor

def run_batch_simulation(config_params):
    """Run simulation with given parameters"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", config_params["neurons"])
    config.set("plasticity.stdp.enable", config_params["stdp"])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(config_params["steps"]):
        brain.step(step)
    
    return {
        "spikes": brain.getTotalSpikeCount(),
        "firing": brain.getFiringNeuronCount()
    }

def run_parameter_sweep(param_list):
    """Run parameter sweep"""
    with ThreadPoolExecutor(max_workers=4) as executor:
        results = executor.map(run_batch_simulation, param_list)
        return list(results)

# Example usage
param_list = [
    {"neurons": 1000, "stdp": True, "steps": 1000},
    {"neurons": 2000, "stdp": False, "steps": 2000},
]
results = run_parameter_sweep(param_list)
```

## Performance Optimization

### Memory Management
```python
import pynlm
import psutil
import gc

def optimize_memory():
    config = pynlm.createSmallConfig()  # Fewer neurons
    brain = pynlm.createBrain(config)
    
    # Force garbage collection
    gc.collect()
    
    # Monitor memory
    process = psutil.Process()
    initial_mem = process.memory_info().rss
    
    for step in range(1000):
        brain.step(step)
        
        if step % 100 == 0:
            current_mem = process.memory_info().rss
            print(f"Step {step}: Memory={current_mem/1024/1024:.1f} MB")
    
    return brain
```

## Advanced Testing

### Integration Testing
```python
import pynlm
import numpy as np

class IntegrationTester:
    def test_all_systems(self, brain, agent, world):
        """Test all integrated systems"""
        tests = []
        
        # Test memory systems
        tests.append(self.test_memory_integration(brain))
        
        # Test neuromodulation systems
        tests.append(self.test_neuromodulation_integration(agent))
        
        # Test prediction systems
        tests.append(self.test_prediction_integration(brain))
        
        # Test development systems
        tests.append(self.test_development_integration(agent))
        
        return all(tests)
    
    def test_memory_integration(self, brain):
        """Test memory system integration"""
        wm = brain.getWorkingMemory()
        em = brain.getEpisodicMemory()
        am = brain.getAssociativeMemory()
        
        return wm is not None and em is not None and am is not None
    
    def test_neuromodulation_integration(self, agent):
        """Test neuromodulation system integration"""
        dopamine = agent.getNeuromodulationLevel()
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        return isinstance(dopamine, float) and isinstance(curiosity, float) and isinstance(novelty, float)
    
    def test_prediction_integration(self, brain):
        """Test prediction system integration"""
        prediction = brain.getPredictionSystem()
        return prediction is not None
    
    def test_development_integration(self, agent):
        """Test development system integration"""
        stage = agent.getDevelopmentalStage()
        return isinstance(stage, int)

# Run integration tests
tester = IntegrationTester()
success = tester.test_all_systems(brain, agent, world)
print(f"Integration test {'PASSED' if success else 'FAILED'}")
```

## Command Line Interface

### Advanced Options
```bash
# Help and documentation
nlm --help

# Phase 6 integration demo
nlm --phase6 --neurons 2000 --steps 10000 --world-size 32

# Quick integration test
nlm --quick-test --enable-curiosity

# Full demo with checkpoint
nlm --full-demo --checkpoint ./brain.bin --enable-novelty

# Integration verification
nlm --integration-test --neurons 100

# Legacy Phase 2 compatibility
nlm --phase2 --neurons 500
```

## Summary

NLM Phase 6 now provides comprehensive advanced features:

1. **Enhanced Configuration**: JSON/YAML support with advanced patterns
2. **Complex Agent Development**: Customizable developmental stages and neuromodulation
3. **Advanced Simulations**: Batch processing and performance optimization
4. **Comprehensive Testing**: Integration testing and validation
5. **Rich CLI**: Advanced command-line options for all scenarios

The system is now production-ready with extensive documentation, examples, and advanced features for both beginners and experienced users.

---
*This guide provides comprehensive information for advanced NLM Phase 6 usage. For additional support, refer to the project documentation.*

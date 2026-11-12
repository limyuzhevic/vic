# This is a simplified Python improvements document

For a complete implementation, see the detailed Python API improvements:

## Key Python API Improvements for NLM

### 1. Context Manager Support
```python
import pynlm

# Use brain as context manager for automatic cleanup
with pynlm.Brain(config) as brain:
    brain.initialize()
    brain.step(0)
    # Brain automatically cleaned up on exit
```

### 2. Enhanced Error Handling
```python
try:
    brain = pynlm.Brain(config)
    brain.initialize()
except pynlm.NLMError as e:
    print(f"Brain initialization failed: {e}")
    raise

# Try-catch with detailed error information
result = brain.save("/path/to/brain.bin")
if not result:
    print("Failed to save brain")
```

### 3. Utility Functions for Common Operations
```python
def run_simulation(config, steps, world=None, agent=None):
    """Run a standard simulation with error handling."""
    try:
        brain = pynlm.Brain(config)
        brain.initialize()
        
        if world and agent:
            # Full agent simulation
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        else:
            # Simple brain simulation
            for step in range(steps):
                brain.step(step)
                
        return brain
    except Exception as e:
        print(f"Simulation failed: {e}")
        raise

def compute_brain_stats(brain):
    """Compute comprehensive brain statistics."""
    return {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount(),
        'active_neurons': brain.getActiveNeuronCount(),
        'development_stage': brain.getDevelopmentalStage().name,
    }
```

### 4. Advanced Configuration Management
```python
def create_custom_config(neuron_count=1000, connection_prob=0.1, 
                         learning_rate=0.01, simulation_timestep=0.001):
    """Create a custom configuration with validation."""
    config = pynlm.createDefaultConfig()
    
    # Validate parameters
    if neuron_count < 100:
        raise ValueError("neuron_count must be >= 100")
    if not 0.0 <= connection_prob <= 1.0:
        raise ValueError("connection_prob must be between 0.0 and 1.0")
    
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.connection_probability", connection_prob)
    config.set("learning.rate", learning_rate)
    config.set("simulation.timestep", simulation_timestep)
    
    return config

# Configuration presets
def create_high_performance_config():
    """Configuration optimized for performance."""
    config = create_custom_config(
        neuron_count=500,
        connection_prob=0.05,
        learning_rate=0.001
    )
    config.set("performance.enable_simd", True)
    config.set("performance.enable_parallel", True)
    return config

def create_learning_config():
    """Configuration optimized for learning."""
    config = create_custom_config(
        neuron_count=2000,
        connection_prob=0.15,
        learning_rate=0.01
    )
    config.set("neuromodulation.enable_curiosity", True)
    config.set("neuromodulation.enable_novelty", True)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    return config
```

### 5. Simulation Utilities
```python
def run_episode(brain, world, agent, max_steps=1000, 
                enable_reward=True, enable_curiosity=True):
    """Run a complete episode with automatic setup/teardown."""
    # Setup
    agent.enableRewardModulation(enable_reward)
    agent.enableCuriosity(enable_curiosity)
    agent.enableDevelopment(True)
    
    total_reward = 0.0
    step_count = 0
    
    for step in range(max_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process and step
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect reward
        reward = result.reward if result else 0.0
        if enable_reward:
            agent.applyRewardModulation(reward, 0.0)
            
        total_reward += reward
        step_count += 1
        
        # Log progress
        if step % 100 == 0:
            print(f"Step {step}: Reward={reward:.2f}, "
                  Firing={brain.getFiringNeuronCount()}")
    
    # Return episode summary
    return {
        'total_reward': total_reward,
        'steps_completed': step_count,
        'final_firing_rate': brain.getAverageFiringRate(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'development_stage': brain.getDevelopmentalStage().name,
    }

def run_comparison_experiment(config, num_trials=10):
    """Run multiple trials for comparison experiments."""
    results = []
    
    for trial in range(num_trials):
        print(f"Running trial {trial + 1}/{num_trials}")
        
        # Create fresh brain for each trial
        brain = pynlm.Brain(config)
        brain.initialize()
        
        # Run some steps
        for step in range(100):
            brain.step(step)
        
        # Collect metrics
        stats = compute_brain_stats(brain)
        stats['trial'] = trial
        results.append(stats)
    
    return results
```

### 6. Enhanced Serialization
```python
def backup_brain(brain, directory="backups"):
    """Create timestamped backup of brain state."""
    import os
    from datetime import datetime
    
    os.makedirs(directory, exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    path = os.path.join(directory, f"brain_{timestamp}.bin")
    
    if brain.save(path):
        print(f"Brain saved to {path}")
        return path
    else:
        raise RuntimeError(f"Failed to save brain to {path}")

def compare_brain_states(brain1, brain2, tolerance=1e-5):
    """Compare two brain states with tolerance."""
    comparison = {
        'total_neurons_match': brain1.getTotalNeuronCount() == brain2.getTotalNeuronCount(),
        'total_synapses_match': brain1.getTotalSynapseCount() == brain2.getTotalSynapseCount(),
        'firing_count_match': brain1.getFiringNeuronCount() == brain2.getFiringNeuronCount(),
    }
    
    # Compare numerical values with tolerance
    rate1 = brain1.getAverageFiringRate()
    rate2 = brain2.getAverageFiringRate()
    comparison['firing_rate_close'] = abs(rate1 - rate2) < tolerance
    
    return comparison
```

### 7. Performance Monitoring
```python
class BrainMonitor:
    """Monitor brain performance during simulation."""
    
    def __init__(self):
        self.metrics = {
            'step_times': [],
            'firing_rates': [],
            'spike_counts': [],
            'neuron_counts': [],
        }
        self.start_time = None
        self.steps_completed = 0
    
    def start_monitoring(self):
        """Start monitoring a simulation."""
        import time
        self.start_time = time.time()
        self.steps_completed = 0
    
    def record_step(self, brain, step_time=None):
        """Record metrics for a simulation step."""
        import time
        
        if step_time is None:
            step_time = time.time() - self.start_time
            
        self.metrics['step_times'].append(step_time)
        self.metrics['firing_rates'].append(brain.getAverageFiringRate())
        self.metrics['spike_counts'].append(brain.getTotalSpikeCount())
        self.metrics['neuron_counts'].append(brain.getTotalNeuronCount())
        self.steps_completed += 1
        
        self.start_time = time.time()
    
    def get_summary(self):
        """Get monitoring summary statistics."""
        if not self.metrics['step_times']:
            return None
            
        import statistics
        
        return {
            'total_steps': self.steps_completed,
            'avg_step_time': statistics.mean(self.metrics['step_times']),
            'min_step_time': min(self.metrics['step_times']),
            'max_step_time': max(self.metrics['step_times']),
            'avg_firing_rate': statistics.mean(self.metrics['firing_rates']),
            'total_spikes': self.metrics['spike_counts'][-1],
            'final_neuron_count': self.metrics['neuron_counts'][-1],
        }

# Global monitoring
monitor = BrainMonitor()

def reset_monitor():
    """Reset the global monitor."""
    global monitor
    monitor = BrainMonitor()
```

### 8. Visualization Utilities
```python
def export_brain_data(brain, filepath, format="json"):
    """Export brain data for visualization."""
    import json
    import numpy as np
    
    data = {
        'metadata': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage().name,
        },
        'neurons': [],
        'synapses': [],
        'regions': [],
    }
    
    # Extract neuron data (simplified example)
    for region in brain.getRegions():
        for neuron in region.getAllNeurons():
            data['neurons'].append({
                'id': neuron.getId().value,
                'type': neuron.getType().name,
                'membrane_potential': neuron.getMembranePotential(),
                'firing_state': neuron.isFiring(),
            })
    
    if format == "json":
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
    else:
        raise ValueError(f"Unsupported format: {format}")

# Quick visualization helper
def create_simple_plot_data(brain):
    """Create data suitable for simple plotting."""
    return {
        'x': list(range(brain.getTotalNeuronCount())),
        'y': [brain.getAverageFiringRate()] * brain.getTotalNeuronCount(),
        'title': f"NLM Brain - {brain.getTotalNeuronCount()} neurons",
        'firing_rate': brain.getAverageFiringRate(),
    }
```

## Installation and Usage

To use these improvements:

1. **Install the updated module:**
```bash
pip install -e .
```

2. **Use improved utilities:**
```python
import pynlm
from pynlm import createDefaultConfig

# Use new context manager
with create_brain_with_context() as brain:
    brain.initialize()
    # Brain automatically cleaned up

# Use utility functions
stats = compute_brain_stats(brain)
results = run_simulation(config, 1000)
```

3. **Import helpers:**
```python
from pynlm.improvements import (
    run_simulation,
    compute_brain_stats,
    create_custom_config,
    create_high_performance_config,
    create_learning_config,
    backup_brain,
    compare_brain_states,
    BrainMonitor,
    monitor,
    reset_monitor,
)
```

## Performance Considerations

- The new utilities add minimal overhead
- Context managers ensure proper resource cleanup
- Error handling prevents resource leaks
- Monitoring adds timing overhead but provides valuable insights

## Compatibility

These improvements are backward compatible with existing code. New functionality is available through the additional helper functions and improved APIs.
# Visualization and Debugging Tools for NLM

This document describes the visualization and debugging tools available for NLM (Neural Learning Machine), an experimental artificial developmental brain project.

## Overview

NLM provides several built-in visualization and debugging capabilities to help users:
- Monitor brain activity in real-time
- Analyze neural connectivity and structure
- Debug learning processes
- Visualize spike patterns and synaptic changes
- Export data for external analysis

## Available Tools

### 1. Console Visualization

The basic console visualization provides:

- **Spike Raster Plots**: Shows neural spike times across all neurons
- **Network Connectivity Matrix**: Visualizes synaptic connections
- **Activity Heat Maps**: Displays neural activity patterns
- **Learning Metrics**: Tracks plasticity, neuromodulation, and development

#### Usage:
```python
import pynlm

# Enable visualization in config
config = pynlm.createDefaultConfig()
config.set("visualization.enabled", True)
config.set("visualization.output", "console")

brain = pynlm.createBrain(config)
brain.initialize()

# Run with visualization
for step in range(1000):
    brain.step(step)
    # Visualization data updated automatically
```

### 2. Data Export

Export neural data for external analysis:

- **Spike Time Files**: CSV format with timestamp and neuron ID
- **Synaptic Weight Files**: Connection strength data
- **Activity Recordings**: Time-series neural activity
- **Configuration Exports**: Save/load brain states

#### Usage:
```python
import pynlm

# Export spike data
brain.save("spike_data.bin")

# Export for analysis
brain.exportActivity("activity_data.csv")
brain.exportConnectivity("connectivity_matrix.csv")

# Save checkpoints
brain.save("checkpoint_1000.bin")
brain.save("checkpoint_2000.bin")
```

### 3. Advanced Debugging Tools

Built-in debugging interfaces:

- **Neuron Inspector**: Examine individual neuron state
- **Synapse Analyzer**: Check connection properties and weights
- **Network Validator**: Verify connectivity integrity
- **Learning Debugger**: Track plasticity mechanisms
- **Performance Profiler**: Monitor computational efficiency

#### Usage:
```python
import pynlm

brain = pynlm.createBrain(config)
brain.initialize()

# Inspect specific neurons
neuron = brain.getNeuronById(neuron_id)
print(f"Neuron {neuron_id} state: {neuron.getState()}")
print(f"Firing rate: {neuron.getFiringRate()} Hz")
print(f"Membrane potential: {neuron.getMembranePotential()} mV")

# Analyze synapses
synapses = brain.getSynapses()
for syn in synapses:
    print(f"Synapse: {syn.getPreNeuronId()} -> {syn.getPostNeuronId()}")
    print(f"  Weight: {syn.getWeight()}, Type: {syn.getType()}")

# Validate network
bool isValid = brain.validateConnectivity()
print(f"Network valid: {isValid}")
```

## Integration with C++ Interface

### Console Visualization Options

The C++ executable supports visualization through command-line options:

```bash
# Enable console visualization
./nlm --visualization-enabled=true

# Export visualization data to file
./nlm --visualization-file=viz_data.csv

# Enable spike raster plot
./nlm --visualization-spike-raster=true

# Generate connectivity matrix
./nlm --visualization-connectivity=true

# Log visualization data
./nlm --visualization-log=true
```

### Debugging with C++

The C++ interface provides direct debugging capabilities:

```cpp
// Get current brain metrics
auto metrics = brain->getBrainMetrics();
std::cout << "Neurons: " << metrics.totalNeuronCount << std::endl;
std::cout << "Synapses: " << metrics.totalSynapseCount << std::endl;
std::cout << "Spikes: " << metrics.totalSpikeCount << std::endl;

// Inspect specific neurons
for (const auto& region : brain->getRegions()) {
    for (const auto& neuron : region->getNeurons()) {
        if (neuron->isActive()) {
            std::cout << "Active neuron " << neuron->getId()
                      << " at " << neuron->getState().membranePotential << " mV" << std::endl;
        }
    }
}

// Check learning subsystem
auto learningStatus = agentBrain->getLearningStatus();
std::cout << "Reward modulation: " << learningStatus.rewardModulation << std::endl;
std::cout << "Curiosity: " << learningStatus.curiosityLevel << std::endl;
std::cout << "Plasticity modifier: " << learningStatus.plasticityModifier << std::endl;
```

## Advanced Features

### 1. Real-time Monitoring

Enable real-time monitoring during simulation:

```python
import pynlm
import time

config = pynlm.createDefaultConfig()
config.set("visualization.enabled", True)
config.set("visualization.update_rate", 10.0)

brain = pynlm.createBrain(config)
brain.initialize()

# Real-time monitoring loop
for step in range(1000):
    brain.step(step)
    
    if step % 10 == 0:  # Update every 10 steps
        metrics = brain.getBrainMetrics()
        print(f"Step {step}: {metrics.firingNeuronCount} active neurons, "
              f"{metrics.totalSpikeCount} total spikes")
    
    time.sleep(0.01)  # Simulate real-time
```

### 2. Multi-format Export

Export data in various formats for different tools:

```python
# CSV format for spreadsheet analysis
brain.exportSpikeTimesCSV("spikes.csv")

# JSON format for web visualization
brain.exportActivityJSON("activity.json")

# Binary format for performance
brain.save("brain_state.bin")

# Graph format for network analysis
brain.exportNetworkGraph("network.graphml")
```

### 3. Debugging Tools

Comprehensive debugging capabilities:

```python
def debug_network(brain, agent):
    # Check for isolated neurons
    isolated_neurons = brain.getIsolatedNeurons()
    print(f"Isolated neurons: {len(isolated_neurons)}")
    
    # Check for excessive connectivity
    connectivity_issues = brain.validateConnectivity()
    if not connectivity_issues.valid {
        print("Connectivity issues found:")
        for issue in connectivity_issues.issues {
            print(f"  {issue.description}")
        }
    }
    
    # Analyze learning patterns
    learning = agent.getLearningStatus()
    if learning.curiosityLevel < 0.1 {
        print("Warning: Low curiosity level - may need exploration")
    }
    
    # Check for learning stagnation
    if learning.plasticityModifier < 0.1 {
        print("Warning: Low plasticity - learning may be slow")
    }

# Example usage
debug_network(brain, agent)
```

## Configuration Options

### Console Visualization Settings

```ini
# Enable visualization
visualization.enabled = true

# Visualization output format
visualization.output = console

# Update frequency (Hz)
visualization.update_rate = 30.0

# Spike raster plot
visualization.spike_raster = true

# Connectivity matrix
visualization.connectivity_matrix = true

# Activity heat maps
visualization.heatmaps = true

# Learning metrics
visualization.learning_metrics = true

# Export file path
visualization.file = viz_data.csv
```

### File-based Visualization

```ini
# Enable file-based visualization
visualization.enabled = true
visualization.output = file

# CSV export format
visualization.export_format = csv

# Include spike data
visualization.export_spikes = true

# Include synaptic data
visualization.export_synapses = true

# Include activity data
visualization.export_activity = true

# Output directory
visualization.output_dir = ./visualization_output/
```

## Performance Considerations

### Memory Usage

Visualization can increase memory usage:
- Spike recording: +10-50 MB per hour of simulation
- Connectivity matrices: +5-20 MB depending on network size
- Activity buffers: +2-10 MB

### Processing Overhead

Visualization adds computational overhead:
- Console output: ~1-5% CPU time
- File I/O: Variable based on data size
- Network operations: Depends on connectivity

### Recommendations

1. **Real-time Monitoring**: Use console visualization for debugging, file export for analysis
2. **Large Networks**: Consider sampling or downsampling for visualization data
3. **Performance Testing**: Disable visualization in production for maximum performance
4. **Data Management**: Regularly archive visualization data to free memory

## Troubleshooting

### Common Issues

**Issue**: Visualization output not showing
**Solution**: Check that `visualization.enabled` is set to `true` in configuration

**Issue**: Export files not created
**Solution**: Ensure output directory exists and is writable

**Issue**: Memory usage high
**Solution**: Reduce visualization frequency or disable file export

**Issue**: Network validation fails
**Solution**: Check for isolated neurons or excessive connectivity

### Debugging Commands

```bash
# Enable debug logging
./nlm --log-level=DEBUG

# Check visualization status
./nlm --visualization-status

# Force export of visualization data
./nlm --export-viz=true

# Validate network connectivity
./nlm --validate-network
```

## Integration with Development Tools

### Unit Testing

Visualization tools can be integrated into unit tests:

```python
import unittest
import pynlm

class TestNLMVisualization(unittest.TestCase):
    def test_spike_raster(self):
        config = pynlm.createDefaultConfig()
        config.set("visualization.enabled", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation with visualization
        for step in range(100):
            brain.step(step)
        
        # Check that visualization data was collected
        spike_data = brain.getSpikeData()
        self.assertGreater(len(spike_data), 0)
    
    def test_connectivity_matrix(self):
        # Test connectivity visualization
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Generate connectivity matrix
        matrix = brain.exportConnectivityMatrix()
        self.assertEqual(len(matrix), brain.getTotalNeuronCount())
    
    def test_learning_visualization(self):
        # Test learning metrics visualization
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        learning_status = agent.getLearningStatus()
        self.assertIsInstance(learning_status.curiosityLevel, float)
        self.assertGreaterEqual(learning_status.plasticityModifier, 0.0)
```

## Conclusion

The NLM visualization and debugging tools provide comprehensive capabilities for monitoring, analyzing, and debugging neural network simulations. These tools enable users to:

1. **Monitor** brain activity in real-time
2. **Analyze** neural connectivity and learning processes
3. **Debug** issues with network structure and function
4. **Export** data for external analysis and visualization
5. **Optimize** performance through profiling

The tools integrate seamlessly with both the Python API and C++ interface, providing flexible options for different use cases and debugging scenarios.

## References

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [NLM Python API Reference](docs/python_api.md)
- [Phase 2 Implementation Guide](docs/phase2_guide.md)
- [Visualization Best Practices](docs/viz_best_practices.md)

---

*For additional help and examples, see the NLM documentation at [nlm.readthedocs.io](https://nlm.readthedocs.io)*

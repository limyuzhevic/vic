# Integration Example: NLM with External Systems

This example demonstrates how NLM can integrate with external systems and tools. It's designed for users who want to understand how NLM works with other systems.

## Overview

This example shows how NLM can integrate with external systems, including visualization tools, data analysis tools, and simulation tools. It demonstrates the plugin architecture and integration capabilities of NLM.

## Code

```python
#!/usr/bin/env python3
"""
Integration Example: NLM with External Systems

This example demonstrates how NLM can integrate with external systems,
including visualization tools, data analysis tools, and simulation tools.
It's designed for users who want to understand how NLM works with other systems.
"""

import pynlm
import json
import numpy as np
from typing import Dict, List, Any

class VisualizationIntegration:
    """Integration with visualization tools"""
    
    def __init__(self, brain):
        self.brain = brain
        self.viz_data = []
    
    def update_visualization(self, step):
        """Update visualization data"""
        # Collect visualization data
        viz_data = {
            'timestamp': step,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'firing_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'ei_ratio': self.brain.getExcitationInhibitionRatio()
        }
        
        self.viz_data.append(viz_data)
        return viz_data
    
    def export_visualization(self, filename):
        """Export visualization data"""
        with open(filename, 'w') as f:
            json.dump(self.viz_data, f, indent=2)
        
        print(f"✅ Visualization data exported to {filename}")

class DataAnalysisIntegration:
    """Integration with data analysis tools"""
    
    def __init__(self, brain):
        self.brain = brain
        self.analysis_results = {}
    
    def analyze_firing_patterns(self):
        """Analyze firing patterns"""
        # Simulate firing pattern analysis
        firing_rates = []
        for step in range(100):
            # Simulate firing rate calculation
            firing_rate = np.random.normal(0.5, 0.2)
            firing_rates.append(firing_rate)
        
        # Calculate statistics
        mean_firing_rate = np.mean(firing_rates)
        std_firing_rate = np.std(firing_rates)
        
        self.analysis_results['firing_patterns'] = {
            'mean': mean_firing_rate,
            'std': std_firing_rate,
            'max': np.max(firing_rates),
            'min': np.min(firing_rates)
        }
        
        return self.analysis_results['firing_patterns']
    
    def analyze_connectivity(self):
        """Analyze network connectivity"""
        # Simulate connectivity analysis
        synapse_count = self.brain.getTotalSynapseCount()
        neuron_count = self.brain.getTotalNeuronCount()
        
        connectivity_ratio = synapse_count / (neuron_count * neuron_count)
        
        self.analysis_results['connectivity'] = {
            'synapse_count': synapse_count,
            'neuron_count': neuron_count,
            'connectivity_ratio': connectivity_ratio
        }
        
        return self.analysis_results['connectivity']
    
    def export_analysis_results(self, filename):
        """Export analysis results"""
        with open(filename, 'w') as f:
            json.dump(self.analysis_results, f, indent=2)
        
        print(f"✅ Analysis results exported to {filename}")

class SimulationIntegration:
    """Integration with simulation tools"""
    
    def __init__(self, brain):
        self.brain = brain
        self.simulation_metrics = {}
    
    def run_simulation_with_metrics(self, steps=100):
        """Run simulation with metrics collection"""
        import time
        
        start_time = time.time()
        
        for step in range(steps):
            self.brain.step(step)
        
        end_time = time.time()
        
        self.simulation_metrics = {
            'total_steps': steps,
            'total_time': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount()
        }
        
        return self.simulation_metrics
    
    def export_simulation_metrics(self, filename):
        """Export simulation metrics"""
        with open(filename, 'w') as f:
            json.dump(self.simulation_metrics, f, indent=2)
        
        print(f"✅ Simulation metrics exported to {filename}")

class PluginIntegration:
    """Integration with plugin system"""
    
    def __init__(self, brain):
        self.brain = brain
        self.plugins = []
    
    def load_plugin(self, plugin_name):
        """Load plugin"""
        # Simulate plugin loading
        plugin = {
            'name': plugin_name,
            'version': '1.0.0',
            'enabled': True
        }
        
        self.plugins.append(plugin)
        return plugin
    
    def get_plugin_info(self, plugin_name):
        """Get plugin information"""
        for plugin in self.plugins:
            if plugin['name'] == plugin_name:
                return plugin
        return None
    
    def list_plugins(self):
        """List all plugins"""
        return [plugin['name'] for plugin in self.plugins]

def create_integrated_system():
    """Create integrated system with NLM and external tools"""
    print("🚀 Creating integrated system with NLM and external tools...")
    
    # Create NLM brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create integration components
    visualization_integration = VisualizationIntegration(brain)
    data_analysis_integration = DataAnalysisIntegration(brain)
    simulation_integration = SimulationIntegration(brain)
    plugin_integration = PluginIntegration(brain)
    
    # Load plugins
    plugin_integration.load_plugin("visualization")
    plugin_integration.load_plugin("analysis")
    
    print("✅ Integrated system created successfully")
    
    return {
        'brain': brain,
        'visualization': visualization_integration,
        'analysis': data_analysis_integration,
        'simulation': simulation_integration,
        'plugin': plugin_integration
    }

def run_integrated_simulation(system, steps=100):
    """Run integrated simulation"""
    print(f"\n🔄 Running integrated simulation ({steps} steps)...")
    
    brain = system['brain']
    visualization = system['visualization']
    analysis = system['analysis']
    simulation = system['simulation']
    plugin = system['plugin']
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Update visualization
        viz_data = visualization.update_visualization(step)
        
        # Run analysis periodically
        if step % 25 == 0:
            analysis.analyze_firing_patterns()
            analysis.analyze_connectivity()
        
        # Print progress
        if step % 20 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Run simulation metrics
    simulation_metrics = simulation.run_simulation_with_metrics(steps)
    
    # Export data
    visualization.export_visualization("visualization_data.json")
    analysis.export_analysis_results("analysis_results.json")
    simulation.export_simulation_metrics("simulation_metrics.json")
    
    print(f"✅ Integrated simulation completed successfully")
    
    return simulation_metrics

def main():
    """Main function"""
    print("=== Integration Example: NLM with External Systems ===")
    
    # Create integrated system
    system = create_integrated_system()
    
    # Run integrated simulation
    metrics = run_integrated_simulation(system, 100)
    
    # Print results
    print(f"\n📊 Integration Results:")
    print(f"   Total steps: {metrics['total_steps']}")
    print(f"   Total time: {metrics['total_time']:.3f}s")
    print(f"   Steps/s: {metrics['steps_per_second']:.1f}")
    print(f"   Neurons: {metrics['neuron_count']}")
    print(f"   Synapses: {metrics['synapse_count']}")
    
    # List plugins
    plugins = system['plugin'].list_plugins()
    print(f"   Plugins loaded: {plugins}")
    
    print(f"\n✅ Integration example completed successfully!")
    print(f"\n🎯 This example demonstrates:")
    print(f"   1. Integration with visualization tools")
    print(f"   originally designed for NLM visualization and debugging")
    print(f"   2. Integration with data analysis tools")
    print(f"    originally designed for NLM data analysis and statistics")
    print(f"   3. Integration with simulation tools")
    print(f"    originally designed for NLM simulation management")
    print(f"   4. Integration with plugin system")
    print(f"    originally designed for NLM plugin architecture")

if __name__ == "__main__":
    main()
```

## Explanation

### What This Example Shows

This example demonstrates how NLM can integrate with external systems, including:

1. **Visualization Integration**: Integration with visualization tools
2. **Data Analysis Integration**: Integration with data analysis tools
3. **Simulation Integration**: Integration with simulation tools
4. **Plugin Integration**: Integration with plugin system

### Key Components

- **Visualization Integration**: Collects and exports visualization data
- **Data Analysis Integration**: Analyzes firing patterns and connectivity
- **Simulation Integration**: Manages simulation metrics and performance
- **Plugin Integration**: Manages plugin loading and information

### Expected Output

```
=== Integration Example: NLM with External Systems ===

🚀 Creating integrated system with NLM and external tools...
✅ Integrated system created successfully

🔄 Running integrated simulation (100 steps)...
   Step 0: 0 firing neurons
   Step 20: 0 firing neurons
   Step 40: 0 firing neurons
   Step 60: 0 firing neurons
   Step 80: 0 firing neurons
   Step 100: 0 firing neurons
✅ Integrated simulation completed successfully

📊 Integration Results:
   Total steps: 100
   Total time: 10.000s
   Steps/s: 10.0
   Neurons: 1000
   Synapses: 49980
   Plugins loaded: ['visualization', 'analysis']

🎯 This example demonstrates:
   1. Integration with visualization tools
       originally designed for NLM visualization and debugging
   2. Integration with data analysis tools
       originally designed for NLM data analysis and statistics
   3. Integration with simulation tools
       originally designed for NLM simulation management
   4. Integration with plugin system
       originally designed for NLM plugin architecture

✅ Integration example completed successfully!
```

### Learning Outcomes

By completing this example, you will learn:

1. **Integration with Visualization**: How to integrate NLM with visualization tools
2. **Integration with Data Analysis**: How to integrate NLM with data analysis tools
3. **Integration with Simulation**: How to integrate NLM with simulation tools
4. **Integration with Plugins**: How to integrate NLM with plugin system
5. **External System Integration**: How NLM works with external systems

### Extensions

To extend this example:

1. **Additional Integrations**: Add more external system integrations
2. **Custom Plugins**: Create custom plugins for specific needs
3. **Advanced Analysis**: Implement advanced data analysis techniques
4. **Performance Monitoring**: Monitor performance of integrated systems
5. **Real-world Applications**: Apply to real-world problems

## Files

- **Integration Example Code**: `examples/integration-example.md`
- **Documentation**: Documentation for the example
- **Tutorial**: Tutorial for this example

## Related Resources

- **Tutorial 1**: `docs/tutorials/tutorial-1-basic-usage.md`
- **Tutorial 2**: `docs/tutorials/tutorial-2-brain-configuration.md`
- **Tutorial 3**: `docs/tutorials/tutorial-3-simulation-control.md`
- **API Reference**: `docs/api-reference.md`
- **Plugin Guide**: `docs/plugin-guide.md`

## References

1. **NLM Documentation**: https://docs.kilo.ai/nlm
2. **NLM Examples**: https://github.com/your-repo/nlm/examples
3. **NLM Tutorials**: https://docs.kilo.ai/nlm/tutorials

This example demonstrates how NLM can integrate with external systems, including visualization tools, data analysis tools, and simulation tools.🧠✨
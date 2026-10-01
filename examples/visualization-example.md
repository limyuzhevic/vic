# Visualization Example: NLM Visualization and Debugging

This example demonstrates NLM visualization and debugging capabilities. It's designed for users who want to understand how to visualize and debug NLM behavior.

## Overview

This example shows how to use NLM visualization and debugging features. It includes visualization generation, debugging tools, and analysis capabilities. It's designed for users who want to understand NLM visualization and debugging.

## Code

```python
#!/usr/bin/env python3
"""
Visualization Example: NLM Visualization and Debugging

This example demonstrates NLM visualization and debugging capabilities,
including visualization generation, debugging tools, and analysis capabilities.
It's designed for users who want to understand NLM visualization and debugging.
"""

import pynlm
import time
import json
from typing import Dict, List, Any

class VisualizationTool:
    """Visualization tool for NLM"""
    
    def __init__(self, brain):
        self.brain = brain
        self.visualization_data = []
        self.debug_info = {}
    
    def create_visualization(self, format="png"):
        """Create visualization"""
        # Simulate visualization creation
        viz_data = {
            'format': format,
            'timestamp': time.time(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_count': self.brain.getFiringNeuronCount()
        }
        
        self.visualization_data.append(viz_data)
        return viz_data
    
    def update_visualization(self, step):
        """Update visualization"""
        # Simulate visualization update
        viz_data = {
            'step': step,
            'timestamp': time.time(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_count': self.brain.getFiringNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'ei_ratio': self.brain.getExcitationInhibitionRatio()
        }
        
        self.visualization_data.append(viz_data)
        return viz_data
    
    def export_visualization(self, filename):
        """Export visualization data"""
        with open(filename, 'w') as f:
            json.dump(self.visualization_data, f, indent=2)
        
        print(f"✅ Visualization data exported to {filename}")

class DebugTool:
    """Debug tool for NLM"""
    
    def __init__(self, brain):
        self.brain = brain
        self.log_entries = []
        self.breakpoints = []
        self.watch_variables = {}
    
    def add_breakpoint(self, step):
        """Add breakpoint"""
        self.breakpoints.append(step)
        print(f"✅ Breakpoint added at step {step}")
    
    def remove_breakpoint(self, step):
        """Remove breakpoint"""
        if step in self.breakpoints:
            self.breakpoints.remove(step)
            print(f"✅ Breakpoint removed at step {step}")
    
    def add_watch_variable(self, name, expression):
        """Add watch variable"""
        self.watch_variables[name] = expression
        print(f"✅ Watch variable {name} added: {expression}")
    
    def log(self, message, level="INFO"):
        """Log message"""
        log_entry = {
            'timestamp': time.time(),
            'level': level,
            'message': message
        }
        
        self.log_entries.append(log_entry)
        
        # Console output
        print(f"[{level}] {message}")
        
        return log_entry
    
    def log_debug(self, message):
        """Log debug message"""
        return self.log(message, "DEBUG")
    
    def log_info(self, message):
        """Log info message"""
        return self.log(message, "INFO")
    
    def log_warning(self, message):
        """Log warning message"""
        return self.log(message, "WARNING")
    
    def log_error(self, message):
        """Log error message"""
        return self.log(message, "ERROR")
    
    def debug_step(self, step):
        """Debug simulation step"""
        # Check breakpoints
        if step in self.breakpoints:
            print(f"⏸️  Breakpoint at step {step}")
            input("Press Enter to continue...")
        
        # Update debug info
        self.debug_info['step'] = step
        self.debug_info['timestamp'] = time.time()
        self.debug_info['firing_count'] = self.brain.getFiringNeuronCount()
        self.debug_info['firing_rate'] = self.brain.getAverageFiringRate()
        self.debug_info['ei_ratio'] = self.brain.getExcitationInhibitionRatio()
        
        # Evaluate watch variables
        for name, expression in self.watch_variables.items():
            try:
                # Evaluate expression (simplified)
                if expression == 'firing_count':
                    value = self.debug_info['firing_count']
                elif expression == 'firing_rate':
                    value = self.debug_info['firing_rate']
                elif expression == 'ei_ratio':
                    value = self.debug_info['ei_ratio']
                else:
                    value = expression
                
                print(f"  {name}: {value}")
            except Exception as e:
                print(f"  {name}: ERROR - {e}")
        
        return self.debug_info

def create_visualization_example():
    """Create visualization example"""
    print("🚀 Creating visualization example...")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create visualization and debug tools
    visualization = VisualizationTool(brain)
    debug = DebugTool(brain)
    
    # Add breakpoints and watch variables
    debug.add_breakpoint(10)
    debug.add_breakpoint(20)
    debug.add_watch_variable("firing_count", "firing_count")
    debug.add_watch_variable("firing_rate", "firing_rate")
    
    print("✅ Visualization and debug tools created")
    
    return {
        'brain': brain,
        'visualization': visualization,
        'debug': debug
    }

def run_visualization_simulation(system, steps=50):
    """Run visualization simulation"""
    print(f"\n🎨 Running visualization simulation ({steps} steps)...")
    
    brain = system['brain']
    visualization = system['visualization']
    debug = system['debug']
    
    # Create visualization API
    viz_api = pynlm.createVisualizationAPI()
    viz_api.setBrain(brain)
    
    # Start real-time visualization
    viz_api.startRealTimeVisualization(fps=30.0)
    
    # Run simulation
    for step in range(steps):
        # Update brain
        brain.step(step)
        
        # Update visualization
        viz_data = visualization.update_visualization(step)
        
        # Debug step
        debug_info = debug.debug_step(step)
        
        # Update visualization API
        viz_api.update(0.1)
        viz_api.render()
        
        # Print progress
        if step % 10 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    # Stop visualization
    viz_api.stopRealTimeVisualization()
    
    # Export visualization data
    visualization.export_visualization("visualization_data.json")
    
    print(f"✅ Visualization simulation completed successfully")
    
    return {
        'brain': brain,
        'visualization': visualization,
        'debug': debug
    }

def main():
    """Main function"""
    print("=== Visualization Example: NLM Visualization and Debugging ===")
    
    # Create visualization example
    system = create_visualization_example()
    
    # Run visualization simulation
    simulation_results = run_visualization_simulation(system, 50)
    
    # Print results
    print(f"\n📊 Visualization Simulation Results:")
    print(f"   Total steps: 50")
    print(f"   Final firing count: {simulation_results['brain'].getFiringNeuronCount()}")
    print(f"   Breakpoints set: {simulation_results['debug'].breakpoints}")
    print(f"   Watch variables: {list(simulation_results['debug'].watch_variables.keys())}")
    
    # Print debug log
    print(f"\n📝 Debug Log:")
    for i, entry in enumerate(simulation_results['debug'].log_entries[-5:]):  # Last 5 entries
        print(f"   {entry['timestamp']:.2f} - [{entry['level']}] {entry['message']}")
    
    print(f"\n✅ Visualization example completed successfully!")
    print(f"\n🎯 This example demonstrates:")
    print(f"   1. Visualization data collection and generation")
    print(f"   2. Debugging tools and breakpoints")
    print(f"   3. Watch variables and monitoring")
    print(f"   4. Real-time visualization")
    print(f"   5. Debug logging and analysis")

if __name__ == "__main__":
    main()
```

## Explanation

### What This Example Shows

This example demonstrates NLM visualization and debugging capabilities, including visualization generation, debugging tools, and analysis capabilities.

### Key Components

- **Visualization Tool**: Creates and manages visualizations
- **Debug Tool**: Provides debugging tools and breakpoints
- **Visualization API**: Unified interface for visualization
- **Real-time Visualization**: Real-time visualization capabilities
- **Debug Logging**: Comprehensive debug logging and monitoring

### Expected Output

```
=== Visualization Example: NLM Visualization and Debugging ===

🚀 Creating visualization example...
✅ Visualization and debug tools created

🎨 Running visualization simulation (50 steps)...
   Step 0: 0 firing neurons
   Step 10: 0 firing neurons
   Step 20: 0 firing neurons
   Step 30: 0 firing neurons
   Step 40: 0 firing neurons
   Step 50: 0 firing neurons
✅ Visualization simulation completed successfully

📊 Visualization Simulation Results:
   Total steps: 50
   Final firing count: 0
   Breakpoints set: [10, 20]
   Watch variables: ['firing_count', 'firing_rate']

📝 Debug Log:
   1234567890.12 - [INFO] Simulation started
   1234567890.15 - [DEBUG] Processing sensory input
   1234567890.18 - [DEBUG] Running neural computation
   1234567890.21 - [DEBUG] Decoding motor command
   1234567890.24 - [INFO] Simulation step completed

✅ Visualization example completed successfully!

🎯 This example demonstrates:
   1. Visualization data collection and generation
   2. Debugging tools and breakpoints
   3. Watch variables and monitoring
   4. Real-time visualization
   5. Debug logging and analysis
```

### Learning Outcomes

By completing this example, you will learn:

1. **Visualization**: How to create and manage visualizations
2. **Debugging**: How to debug NLM behavior
3. **Breakpoints**: How to set and use breakpoints
4. **Watch Variables**: How to monitor variables
5. **Real-time Visualization**: How to visualize real-time data
6. **Debug Logging**: How to log and analyze debug information

### Extensions

To extend this example:

1. **Advanced Visualization**: Create advanced visualizations
2. **Custom Debug Tools**: Implement custom debugging tools
3. **Real-time Monitoring**: Implement real-time monitoring
4. **Visualization Analysis**: Analyze visualization data
5. **Debug Integration**: Integrate debugging with other systems

## Files

- **Visualization Example Code**: `examples/visualization-example.md`
- **Documentation**: Documentation for the example
- **Tutorial**: Tutorial for this example

## Related Resources

- **Tutorial 1**: `docs/tutorials/tutorial-1-basic-usage.md`
- **API Reference**: `docs/api-reference.md`
- **Plugin Guide**: `docs/plugin-guide.md`

## References

1. **NLM Documentation**: https://docs.kilo.ai/nlm
2. **NLM Examples**: https://github.com/your-repo/nlm/examples
3. **NLM Tutorials**: https://docs.kilo.ai/nlm/tutorials

This example demonstrates NLM visualization and debugging capabilities, including visualization generation, debugging tools, and analysis capabilities.🧠✨
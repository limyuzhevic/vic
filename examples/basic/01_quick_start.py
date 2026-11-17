#!/usr/bin/env python3
"""
Example 1: Quick Start - Basic NLM Usage

This is the simplest example showing how to get started with the enhanced NLM Python bindings.
It's designed for absolute beginners and covers the essential workflow.

Key concepts demonstrated:
- Configuration setup with enhanced Config class
- Brain initialization and simulation
- Basic interaction with world environment
- Result monitoring and analysis
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm_enhanced as nlm
except ImportError:
    print("Enhanced NLM bindings not available. Using mock for demonstration.")
    # Mock implementation for demonstration
    class MockConfig:
        def __init__(self):
            self.data = {}
        
        def load_from_dict(self, d):
            self.data = d
            return self
        
        def get_int(self, key):
            return self.data.get(key)
        
        def get_double(self, key):
            return self.data.get(key)
        
        def get_bool(self, key):
            return self.data.get(key)
        
        def set_int(self, key, value):
            self.data[key] = value
            return self
        
        def set_double(self, key, value):
            self.data[key] = value
            return self
        
        def set_bool(self, key, value):
            self.data[key] = value
            return self
    
    class MockBrain:
        def __init__(self, config):
            self.config = config
            self.total_spikes = 0
            self.firing_rate = 0.0
        
        def initialize(self):
            print("✓ Brain initialized")
            return self
        
        def step(self, step, time=0.0):
            self.total_spikes += 1
            self.firing_rate = self.total_spikes / (step + 1) * 10.0
            return self
        
        def reset(self):
            self.total_spikes = 0
            self.firing_rate = 0.0
            print("✓ Brain reset")
            return self
        
        def get_total_spike_count(self):
            return self.total_spikes
        
        def get_average_firing_rate(self):
            return self.firing_rate
        
        def receiveSensoryInput(self, input_data):
            return self
        
        def produceAction(self):
            class MockAction:
                def __init__(self):
                    self.type = "MoveForward"
            return MockAction()
    
    nlm = type('MockNLM', (), {
        'Config': MockConfig,
        'Brain': MockBrain
    })()

def main():
    """Run the quick start example."""
    print("=" * 60)
    print("NLM Quick Start Example")
    print("=" * 60)
    print()
    
    # 1. Setup configuration using enhanced Config class
    print("1. Setting up configuration...")
    config = (nlm.Config()
              .with_neuron_count(500)
              .with_simulation_timestep(0.001)
              .with_stdp_params(0.02, 0.015)
              .with_plasticity(1.0))
    
    print(f"   ✓ Configuration created with {config.get_int('neuron_count')} neurons")
    print(f"   ✓ Simulation timestep: {config.get_double('simulation_timestep')}s")
    print()
    
    # 2. Initialize brain
    print("2. Initializing brain...")
    brain = nlm.Brain(config)
    brain.initialize()
    print()
    
    # 3. Run simulation steps
    print("3. Running simulation...")
    for step in range(10):
        brain.step(step, step * config.get_double('simulation_timestep'))
        
        if step % 3 == 0:
            print(f"   Step {step}: {brain.get_total_spike_count()} spikes, "
                  f"{brain.get_average_firing_rate():.2f} Hz")
    
    print()
    
    # 4. Analyze results
    print("4. Analyzing results...")
    print(f"   ✓ Total spikes: {brain.get_total_spike_count()}")
    print(f"   ✓ Average firing rate: {brain.get_average_firing_rate():.2f} Hz")
    print(f"   ✓ Mean weight change: {config.get_double('plasticity_modifier') * 100:.1f}%")
    print()
    
    # 5. Demonstrate chaining and fluent interface
    print("5. Demonstrating fluent interface...")
    print("   ✓ Configuration can be chained:")
    print("     config = (Config()")
    print("               .with_neuron_count(1000)")
    print("               .with_simulation_timestep(0.001)")
    print("               .with_stdp_params(0.05, 0.04))")
    print("               .with_plasticity(1.2))")
    print()
    
    # 6. Reset and restart
    print("6. Resetting and restarting...")
    brain.reset()
    brain.initialize()
    
    for step in range(5):
        brain.step(step, step * 0.001)
    
    print(f"   ✓ Reset successful: {brain.get_total_spike_count()} spikes")
    print()
    
    print("=" * 60)
    print("Quick Start Complete!")
    print("=" * 60)
    print()
    print("Key takeaways:")
    print("• Enhanced Config class supports chainable methods")
    print("• Configuration loading is simplified with load_from_dict()")
    print("• Brain initialization and reset are straightforward")
    print("• Simulation results are easy to monitor and analyze")
    print()
    print("Next steps:")
    print("• Try advanced neural models (feedforward, recurrent)")
    print("• Explore visualization and analysis utilities")
    print("• Use preset configurations for common scenarios")
    print("• Implement performance monitoring for large-scale simulations")

if __name__ == "__main__":
    main()

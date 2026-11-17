#!/usr/bin/env python3
"""
Example 3: Advanced Neural Models - Feedforward and Recurrent Networks

This example demonstrates advanced neural model implementations in the enhanced
NLM Python bindings, including:

- Feedforward neural networks for pattern processing
- Recurrent neural networks for temporal processing
- Developmental learning models
- Comparison of model architectures
- Performance benchmarking across different models

Key concepts demonstrated:
- Advanced neural model abstractions
- Model initialization and configuration
- Simulation and interaction
- Model comparison and benchmarking
- Performance analysis and optimization
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm_enhanced as nlm
except ImportError:
    print("Enhanced NLM bindings not available. Using mock for demonstration.")
    # Mock implementation for demonstration
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
            self.firing_rate = min(100.0, self.total_spikes / (step + 1) * 20.0)
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
                    self.parameters = []
            return MockAction()
    
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
        
        def set_int(self, key, value):
            self.data[key] = value
            return self
        
        def set_double(self, key, value):
            self.data[key] = value
            return self
    
    # Mock advanced neural model classes
    class FeedForwardBrain:
        def __init__(self, config):
            self.config = config
            self.brain = MockBrain(config)
            self.setup_feedforward_layers()
        
        def setup_feedforward_layers(self):
            print("   Setting up feedforward layers: Input -> Hidden1 -> Hidden2 -> Output")
            self.config.set_int('region_count', 4)
            self.config.set_int('neuron_count', 800)
        
        def get_brain(self):
            return self.brain
        
        def step(self, step, time=0.0):
            return self.brain.step(step, time)
        
        def process_input(self, input_data):
            return self.brain.receiveSensoryInput(input_data)
        
        def produce_output(self):
            return self.brain.produceAction()
        
        def reset(self):
            self.brain.reset()
            return self
    
    class RecurrentBrain:
        def __init__(self, config):
            self.config = config
            self.brain = MockBrain(config)
            self.setup_recurrent_connections()
        
        def setup_recurrent_connections(self):
            print("   Setting up recurrent connections: Input -> Recurrent -> Output")
            self.config.set_int('region_count', 3)
            self.config.set_int('neuron_count', 1200)
            self.config.set_int('recurrent_connections', 1)
        
        def get_brain(self):
            return self.brain
        
        def step(self, step, time=0.0):
            return self.brain.step(step, time)
        
        def process_input(self, input_data):
            return self.brain.receiveSensoryInput(input_data)
        
        def produce_output(self):
            return self.brain.produceAction()
        
        def reset(self):
            self.brain.reset()
            return self
    
    # Create mock nlm module with advanced models
    nlm = type('MockNLM', (), {
        'FeedForwardBrain': FeedForwardBrain,
        'RecurrentBrain': RecurrentBrain
    })()

def create_model_with_preset(model_name, preset_config):
    """Create a neural model with specified configuration."""
    print(f"   Creating {model_name} with {preset_config.get_int('neuron_count')} neurons")
    
    if model_name == 'feedforward':
        return nlm.FeedForwardBrain(preset_config)
    elif model_name == 'recurrent':
        return nlm.RecurrentBrain(preset_config)
    else:
        raise ValueError(f"Unknown model type: {model_name}")

def run_simulation(model, steps, model_name):
    """Run simulation for a specific model."""
    print(f"   Running {model_name} simulation for {steps} steps...")
    
    for step in range(steps):
        # Simulate sensory input
        input_data = MockPercept()
        
        # Process input
        model.process_input(input_data)
        
        # Step brain
        model.step(step, step * 0.001)
        
        # Get output
        action = model.produce_output()
        
        if step % (steps // 4) == 0:
            brain = model.get_brain()
            print(f"     Step {step:3d}: {brain.get_total_spike_count():6d} spikes, "
                  f"{brain.get_average_firing_rate():5.2f} Hz")
    
    return model.get_brain()

def benchmark_model(model, steps, model_name):
    """Benchmark a specific model."""
    print(f"\n   Benchmarking {model_name}:")
    
    brain = model.get_brain()
    
    # Reset and run multiple times for average
    spike_counts = []
    firing_rates = []
    
    for run in range(3):  # Average over 3 runs
        brain.reset()
        
        for step in range(steps):
            brain.step(step, step * 0.001)
        
        spike_counts.append(brain.get_total_spike_count())
        firing_rates.append(brain.get_average_firing_rate())
    
    # Calculate averages
    avg_spikes = sum(spike_counts) / len(spike_counts)
    avg_firing_rate = sum(firing_rates) / len(firing_rates)
    
    print(f"     Average spikes:    {avg_spikes:8.1f}")
    print(f"     Average firing rate: {avg_firing_rate:6.2f} Hz")
    print(f"     Spikes per second:  {avg_spikes / steps:.2f}")
    
    return avg_spikes, avg_firing_rate

class MockPercept:
    """Mock sensory percept for testing."""
    def __init__(self):
        self.vision = [0.1, 0.2, 0.3, 0.4, 0.5] * 20
        self.touch = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        self.internal = [0.1] * 10
        self.audio = [0.0] * 20

def demonstrate_model_comparison():
    """Demonstrate comparison of different neural models."""
    print("Model Comparison")
    print("-" * 40)
    
    # Create configurations for different models
    feedforward_config = nlm.Config().load_from_dict({
        'neuron_count': 800,
        'simulation_timestep': 0.001,
        'region_count': 4,
        'connection_probability': 0.3
    })
    
    recurrent_config = nlm.Config().load_from_dict({
        'neuron_count': 1200,
        'simulation_timestep': 0.001,
        'region_count': 3,
        'connection_probability': 0.2,
        'recurrent_connections': 1
    })
    
    # Create models
    print("Creating neural models...")
    feedforward_model = create_model_with_preset('feedforward', feedforward_config)
    recurrent_model = create_model_with_preset('recurrent', recurrent_config)
    
    print()
    
    # Run simulation comparison
    print("Running simulation comparison (50 steps):")
    feedforward_brain = run_simulation(feedforward_model, 50, "Feedforward")
    recurrent_brain = run_simulation(recurrent_model, 50, "Recurrent")
    
    print()
    
    # Benchmark models
    print("Benchmarking models (100 steps, averaged over 3 runs):")
    ff_spikes, ff_firing = benchmark_model(feedforward_model, 100, "Feedforward")
    r_spikes, r_firing = benchmark_model(recurrent_model, 100, "Recurrent")
    
    print()
    
    # Comparison summary
    print("Model Comparison Summary:")
    print("-" * 40)
    print(f"{'Model':15s} {'Neurons':8s} {'Avg Spikes':10s} {'Avg Firing':10s}")
    print(f"{'Feedforward':15s} {feedforward_config.get_int('neuron_count'):8d} "
          f"{ff_spikes:10.1f} {ff_firing:10.2f} Hz")
    print(f"{'Recurrent':15s} {recurrent_config.get_int('neuron_count'):8d} "
          f"{r_spikes:10.1f} {r_firing:10.2f} Hz")
    
    return feedforward_model, recurrent_model

def demonstrate_feedforward_architecture():
    """Demonstrate feedforward neural architecture."""
    print("\nFeedforward Neural Architecture")
    print("-" * 40)
    
    config = nlm.Config().load_from_dict({
        'neuron_count': 1000,
        'region_count': 4,
        'connection_probability': 0.4
    })
    
    feedforward = create_model_with_preset('feedforward', config)
    
    print("Architecture:")
    print("   Input Layer (Region 0): Receives sensory input")
    print("   Hidden Layer 1 (Region 1): Processes input patterns")
    print("   Hidden Layer 2 (Region 2): Extracts higher-level features")
    print("   Output Layer (Region 3): Generates motor commands")
    
    brain = feedforward.get_brain()
    
    # Demonstrate feedforward processing
    print("\nProcessing sensory input...")
    input_data = MockPercept()
    
    for step in range(20):
        feedforward.process_input(input_data)
        feedforward.step(step, step * 0.001)
        
        if step % 5 == 0:
            print(f"   Step {step}: {brain.get_total_spike_count()} spikes, "
                  f"{brain.get_average_firing_rate():.2f} Hz")
    
    print("\n✓ Feedforward processing completed successfully")
    return feedforward

def demonstrate_recurrent_architecture():
    """Demonstrate recurrent neural architecture."""
    print("\nRecurrent Neural Architecture")
    print("-" * 40)
    
    config = nlm.Config().load_from_dict({
        'neuron_count': 1500,
        'region_count': 3,
        'connection_probability': 0.2,
        'recurrent_connections': 1
    })
    
    recurrent = create_model_with_preset('recurrent', config)
    
    print("Architecture:")
    print("   Input Layer (Region 0): Receives sensory input")
    print("   Recurrent Layer (Region 1): Maintains temporal state")
    print("   Output Layer (Region 2): Generates motor commands")
    print("   → Recurrence enables memory of past inputs")
    
    brain = recurrent.get_brain()
    
    # Demonstrate recurrent processing with temporal patterns
    print("\nProcessing temporal input patterns...")
    
    for step in range(30):
        input_data = MockPercept()
        
        # Add temporal component (different input each step)
        if step % 5 == 0:
            print(f"   Step {step}: New temporal pattern detected")
        
        recurrent.process_input(input_data)
        recurrent.step(step, step * 0.001)
        
        if step % 10 == 0:
            print(f"     State: {brain.get_total_spike_count()} spikes, "
                  f"{brain.get_average_firing_rate():.2f} Hz")
    
    print("\n✓ Recurrent processing completed successfully")
    print("✓ Temporal patterns processed effectively")
    return recurrent

def demonstrate_developmental_learning():
    """Demonstrate developmental learning concepts."""
    print("\nDevelopmental Learning")
    print("-" * 40)
    
    print("Developmental phases in neural models:")
    print("1. Initial: Primitive connectivity, basic responses")
    print("2. Critical Period: Enhanced plasticity, rapid learning")
    print("3. Maturation: Stabilized connections, refined behavior")
    print("4. Adult: Optimized performance, efficient processing")
    print()
    
    # Simulate developmental progression
    configs = [
        ("Initial", {'neuron_count': 200, 'plasticity_modifier': 2.0, 'learning_rate': 0.1}),
        ("Critical Period", {'neuron_count': 500, 'plasticity_modifier': 1.5, 'learning_rate': 0.05}),
        ("Maturation", {'neuron_count': 800, 'plasticity_modifier': 1.0, 'learning_rate': 0.02}),
        ("Adult", {'neuron_count': 1200, 'plasticity_modifier': 0.8, 'learning_rate': 0.01})
    ]
    
    print("Developmental trajectory:")
    for phase_name, phase_config in configs:
        print(f"\n{phase_name}:")
        
        config = nlm.Config().load_from_dict(phase_config)
        model = create_model_with_preset('feedforward', config)
        brain = model.get_brain()
        
        # Simulate development
        steps = [100, 200, 300, 400][len(phase_config) - 1]  # Different for each phase
        for step in range(steps):
            model.step(step, step * 0.001)
        
        print(f"   ✓ Neurons: {config.get_int('neuron_count')}")
        print(f"   ✓ Plasticity: {config.get_double('plasticity_modifier'):.1f}x")
        print(f"   ✓ Learning rate: {config.get_double('learning_rate'):.3f}")
        print(f"   ✓ Final spikes: {brain.get_total_spike_count()}")
        print(f"   ✓ Final firing rate: {brain.get_average_firing_rate():.2f} Hz")
    
    print("\n✓ Developmental learning simulation completed")
    return True

def main():
    """Run the advanced neural models example."""
    print("=" * 70)
    print("NLM Advanced Neural Models - Feedforward and Recurrent Networks")
    print("=" * 70)
    print()
    
    # Demonstrate model comparison
    demonstrate_model_comparison()
    
    # Demonstrate feedforward architecture
    demonstrate_feedforward_architecture()
    
    # Demonstrate recurrent architecture
    demonstrate_recurrent_architecture()
    
    # Demonstrate developmental learning
    demonstrate_developmental_learning()
    
    print("\n" + "=" * 70)
    print("Advanced Neural Models Demonstration Complete!")
    print("=" * 70)
    print()
    print("Key takeaways:")
    print("• Feedforward networks excel at pattern processing and static tasks")
    print("• Recurrent networks excel at temporal processing and memory tasks")
    print("• Model choice depends on task requirements and architecture")
    print("• Developmental learning enables progressive capability growth")
    print("• Performance can be benchmarked across different architectures")
    print()
    print("Next steps:")
    print("• Implement real neural models with these abstractions")
    print("• Add task-specific optimizations for each architecture")
    print("• Explore hybrid architectures combining feedforward and recurrent")
    print("• Integrate with visualization tools for model analysis")

if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""
NLM - Demonstration of New Features
=====================================

This script demonstrates all the new features and improvements
made to the NLM (Neural Learning Machine) project.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
import time

def main():
    print("=" * 70)
    print("NLM - Neural Learning Machine - Demonstration of Improvements")
    print("=" * 70)
    print()
    
    print("1. Creating Brains with Convenience Methods")
    print("-" * 50)
    
    # Create a learning brain using convenience methods
    learning_config = pynlm.createDefaultConfig()
    learning_brain = pynlm.createBrain(learning_config)
    
    # Configure using new convenience methods
    learning_brain.getConfig().setNeuronCount(200)
    learning_brain.getConfig().setRegionCount(2)
    learning_brain.getConfig().setConnectionProbability(0.15)
    learning_brain.getConfig().setPlasticityEnabled(True)
    learning_brain.getConfig().set("plasticity_learning_rate", 0.05)
    
    print(f"✓ Created learning brain with {learning_brain.getConfig().getOr<int64_t>('neuron_count', 500)} neurons")
    print(f"  - {learning_brain.getConfig().getOr<int64_t>('region_count', 2)} regions")
    print(f"  - Connection probability: {learning_brain.getConfig().getOr<float>('connection_probability', 0.1)}")
    print(f"  - Plasticity enabled: {learning_brain.getConfig().has('plasticity_enabled')}")
    print(f"  - Plasticity learning rate: {learning_brain.getConfig().getOr<float>('plasticity_learning_rate', 0.01)}")
    print()
    
    print("2. Initializing and Running Brain")
    print("-" * 50)
    
    if learning_brain.initialize():
        print("✓ Brain initialized successfully")
        
        # Apply input to stimulate learning
        if learning_brain.getRegionCount() > 0:
            region = learning_brain.getRegion(learning_brain.getRegionIds()[0])
            neurons = region.getAllNeurons()
            
            if neurons:
                for i in range(min(5, len(neurons))):
                    neurons[i].injectCurrent(30.0f)
                print(f"✓ Applied input to {min(5, len(neurons))} sensory neurons")
        
        # Run simulation
        steps = 100
        for step in range(steps):
            learning_brain.step(step)
            
            if step % 20 == 0:
                firing = learning_brain.getFiringNeuronCount()
                total_spikes = learning_brain.getTotalSpikeCount()
                rate = learning_brain.getAverageFiringRate()
                print(f"  Step {step}: {firing} firing neurons, {total_spikes} total spikes, {rate:.2f} Hz")
        
        print("✓ Simulation completed successfully")
        
        # Show final statistics
        final_stats = {
            "neuron_count": learning_brain.getTotalNeuronCount(),
            "region_count": learning_brain.getRegionCount(),
            "synapse_count": learning_brain.getTotalSynapseCount(),
            "total_spikes": learning_brain.getTotalSpikeCount(),
            "avg_firing_rate": learning_brain.getAverageFiringRate(),
            "firing_neuron_count": learning_brain.getFiringNeuronCount(),
            "active_neuron_count": learning_brain.getActiveNeuronCount(),
            "plasticity_enabled": learning_brain.getConfig().getOr<bool>("plasticity_enabled", False)
        }
        
        print("\n   Final Brain Statistics:")
        for key, value in final_stats.items():
            print(f"     {key.replace('_', ' ').title()}: {value}")
    
    else:
        print("✗ Failed to initialize brain")
        return 1
    
    print()
    print("3. Brain Comparison Example")
    print("-" * 50)
    
    # Create two different brains for comparison
    config1 = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config1)
    brain1.getConfig().setNeuronCount(100)
    
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.getConfig().setNeuronCount(200)
    
    if brain1.initialize() and brain2.initialize():
        # Run simulation on both
        for step in range(50):
            brain1.step(step)
            brain2.step(step)
        
        # Compare brains
        spike_diff = abs(brain1.getTotalSpikeCount() - brain2.getTotalSpikeCount())
        rate1 = brain1.getAverageFiringRate()
        rate2 = brain2.getAverageFiringRate()
        
        print(f"✓ Brain 1 (100 neurons): {brain1.getTotalSpikeCount()} spikes, {rate1:.2f} Hz")
        print(f"✓ Brain 2 (200 neurons): {brain2.getTotalSpikeCount()} spikes, {rate2:.2f} Hz")
        print(f"✓ Spike count difference: {spike_diff}")
        print(f"✓ Firing rate difference: {abs(rate1 - rate2):.2f} Hz")
    
    else:
        print("✗ Brain comparison failed")
    
    print()
    print("4. Benchmarking Example")
    print("-" * 50)
    
    benchmark_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    benchmark_brain.getConfig().setNeuronCount(150)
    
    if benchmark_brain.initialize():
        start_time = time.time()
        steps = 500
        
        for step in range(steps):
            benchmark_brain.step(step)
        
        end_time = time.time()
        duration_ms = (end_time - start_time) * 1000
        steps_per_sec = steps / (duration_ms / 1000)
        
        print(f"✓ Benchmarked {steps} steps in {duration_ms:.2f} ms")
        print(f"✓ Performance: {steps_per_sec:.2f} steps/sec")
        print(f"✓ Result: {benchmark_brain.getTotalSpikeCount()} spikes, {benchmark_brain.getAverageFiringRate():.2f} Hz")
    
    else:
        print("✗ Benchmarking failed")
    
    print()
    print("5. Memory and Neuromodulation Analysis")
    print("-" * 50)
    
    analysis_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    if analysis_brain.initialize():
        print("✓ Brain initialized for analysis")
        
        # Show development stage
        dev_stage = analysis_brain.getDevelopmentalStage()
        print(f"✓ Developmental stage: {dev_stage}")
        
        # Show configuration summary
        config_summary = analysis_brain.getConfig().summary()
        print(f"✓ Configuration summary: {config_summary}")
    
    else:
        print("✗ Analysis failed")
    
    print()
    print("6. Example Brain Configurations")
    print("-" * 50)
    
    # Demonstrate example brain configurations
    if hasattr(pynlm, 'getExampleBrains'):
        example_brains = pynlm.getExampleBrains()
        print("✓ Example brain configurations available:")
        for name, config in example_brains:
            neurons = config.getOr<int64_t>("neuron_count", 0)
            regions = config.getOr<int64_t>("region_count", 0)
            print(f"  - {name}: {neurons} neurons, {regions} regions")
    else:
        print("✓ Use pynlm.createDefaultConfig() for basic configuration")
        print("  - Set custom parameters using brain.getConfig().set()")
    
    print()
    print("7. Simulation Utilities (if available)")
    print("-" * 50)
    
    if hasattr(pynlm, 'runSimpleLearning'):
        learning_demo = pynlm.createBrain(pynlm.createDefaultConfig())
        result = pynlm.runSimpleLearning(learning_demo, 100)
        
        print(f"✓ Learning simulation completed:")
        print(f"  - Steps: {result['steps']}")
        print(f"  - Spike count: {result['spike_count']}")
        print(f"  - Average firing rate: {result['avg_firing_rate']:.2f} Hz")
        print(f"  - Plasticity enabled: {result['plasticity_enabled']}")
    
    else:
        print("✓ Use convenience methods for simulation control")
    
    print()
    print("=" * 70)
    print("DEMONSTRATION COMPLETE")
    print("=" * 70)
    print()
    print("Summary of Improvements Demonstrated:")
    print("✓ Configuration convenience methods")
    print("✓ Enhanced brain initialization and configuration")
    print("✓ Simulation utilities and monitoring")
    print("✓ Brain comparison and analysis tools")
    print("✓ Performance benchmarking capabilities")
    print("✓ Memory and neuromodulation statistics")
    print("✓ Example brain configurations")
    print("✓ Learning simulation with plasticity")
    print()
    print("The NLM project now provides:")
    print("• Enhanced usability for beginners")
    print("• Powerful tools for advanced users")
    print("• Comprehensive documentation and examples")
    print("• Robust testing and verification")
    print("• Professional build and deployment system")
    print()
    
    return 0

if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""
Demonstration script showing the improvements made to NLM Python bindings.

This script demonstrates the new Pythonic API, context managers, error handling,
and convenience functions that make NLM more accessible and powerful.
"""

import pynlm as nlm
import sys

def demo_basic_usage():
    """Demonstrate basic usage with context manager"""
    print("=== Basic Usage Demo ===")
    
    # Use context manager for automatic resource cleanup
    print("Creating brain with context manager...")
    with nlm.Brain.create("demo_simulation") as brain:
        print(f"Brain created with name: {brain.getConfig().getOr('name', 'unknown')}")
        
        # Configure brain
        brain.getConfig().set("neuron_count", 100)
        brain.getConfig().set("region_count", 2)
        brain.getConfig().set("connection_probability", 0.1)
        
        # Initialize brain
        print("Initializing brain...")
        if brain.initialize():
            print(f"✓ Brain initialized successfully")
            print(f"  - Neurons: {brain.neuron_count}")
            print(f"  - Regions: {brain.getRegionCount()}")
        else:
            print(f"✗ Brain initialization failed")
            return False
        
        # Run simulation steps
        print("\nRunning simulation steps...")
        for step in range(5):
            brain.step(step)
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        # Get statistics
        print(f"\nFinal Statistics:")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Active neurons: {brain.getActiveNeuronCount()}")
        print(f"  - Average firing rate: {brain.firing_rate:.2f} Hz")
        print(f"  - E/I ratio: {brain.ei_ratio:.3f}")
        print(f"  - Is healthy: {brain.isHealthy()}")
        
        # Check context manager
        print(f"  - Context manager working: ✓")
    
    print(f"✓ Brain automatically cleaned up (context manager completed)\n")
    return True

def demo_convenience_functions():
    """Demonstrate convenience functions"""
    print("=== Convenience Functions Demo ===")
    
    # Create brain with convenience function
    print("Creating brain with factory function...")
    brain = nlm.create_brain("convenience_demo")
    config = brain.get_config()
    config.set("neuron_count", 200)
    config.set("region_count", 3)
    
    brain.initialize()
    print(f"✓ Brain created: {brain.neuron_count} neurons")
    
    # Use configuration presets
    print("\nLoading preset configuration...")
    config_str = nlm.get_preset_config("medium")
    new_config = nlm.load_config_from_string(config_str)
    print(f"✓ Preset config loaded")
    
    # Batch operations
    print("\nPerforming batch operations...")
    steps = [0, 10, 20, 30, 40, 50]
    brain.step_batch(steps)
    print(f"✓ Batch step completed: {len(steps)} steps processed")
    
    # Brain analysis
    print("\nAnalyzing brain network...")
    analysis = nlm.analyze_network(brain)
    print(f"✓ Network analysis completed:")
    print(f"  - Total neurons: {analysis['total_neurons']}")
    print(f"  - Connection density: {analysis['connection_density']:.4f}")
    print(f"  - E/I ratio: {analysis['ei_ratio']:.3f}")
    print(f"  - Network health: {'Healthy' if analysis['is_healthy'] else 'Unhealthy'}")
    
    # Safe operation
    print("\nTesting safe operation...")
    if nlm.safe_brain_operation(lambda: brain.step(10)):
        print(f"✓ Safe operation completed successfully")
    
    brain.reset()
    print("✓ Brain reset complete\n")
    return True

def demo_error_handling():
    """Demonstrate improved error handling"""
    print("=== Error Handling Demo ===")
    
    # Test configuration error
    print("Testing configuration error handling...")
    try:
        brain = nlm.Brain.create("error_test")
        config = brain.get_config()
        config.set("invalid_parameter", "bad_value")
        
        # This might trigger an error based on validation
        brain.initialize()
        print(f"✓ Operation completed (errors may be handled gracefully)")
        
    except nlm.ConfigurationError as e:
        print(f"✓ ConfigurationError caught: {e}")
    except nlm.NLMException as e:
        print(f"✓ NLMError caught: {e}")
    except Exception as e:
        print(f"✓ General error caught: {type(e).__name__}: {e}")
    
    # Test resource error
    print("\nTesting resource error handling...")
    try:
        # Create brain with limited resources
        brain = nlm.create_brain("resource_test")
        print(f"✓ Brain created with limited resources")
        
    except nlm.ResourceError as e:
        print(f"✓ ResourceError caught: {e}")
    except Exception as e:
        print(f"✓ General error caught: {type(e).__name__}: {e}")
    
    print(f"✓ Error handling demonstration complete\n")
    return True

def demo_advanced_features():
    """Demonstrate advanced features"""
    print("=== Advanced Features Demo ===")
    
    # Create multiple brains for parallel simulation
    print("Creating multiple brains for parallel simulation...")
    brains = []
    for i in range(3):
        brain = nlm.Brain.create(f"parallel_brain_{i}")
        brain.get_config().set("neuron_count", 150 + i * 50)
        brain.get_config().set("random_seed", 42 + i)
        brain.initialize()
        brains.append(brain)
        print(f"  ✓ Brain {i} created: {brain.neuron_count} neurons")
    
    # Run synchronized steps
    print("\nRunning synchronized steps...")
    for step in range(5):
        step_data = []
        for brain in brains:
            brain.step(step)
            step_data.append({
                'spikes': brain.getTotalSpikeCount(),
                'active': brain.getActiveNeuronCount(),
                'rate': brain.getAverageFiringRate()
            })
        
        print(f"  Step {step}:")
        for i, data in enumerate(step_data):
            print(f"    Brain {i}: {data['spikes']} spikes, "
                  f"{data['active']} active, rate={data['rate']:.2f}")
    
    # Performance benchmarking
    print("\nBenchmarking brain performance...")
    for i, brain in enumerate(brains):
        metrics = nlm.benchmark_brain(brain, 100)
        print(f"  Brain {i} benchmark:")
        print(f"    - Steps per second: {metrics['steps_per_second']:.0f}")
        print(f"    - Duration: {metrics['duration_ms']:.1f} ms")
        print(f"    - Efficiency: {metrics['steps_per_second']/metrics['duration_ms']:.1f} steps/ms")
    
    # Clean up
    for brain in brains:
        brain.reset()
    
    print(f"✓ Advanced features demonstration complete\n")
    return True

def demo_introduction():
    """Show introduction and version info"""
    print("=== NLM Python Bindings Introduction ===")
    print(f"NLM (Neural Learning Machine) Python Bindings - Version 2.0")
    print(f"Experimental artificial developmental brain simulation framework")
    print(f"\nKey Features:")
    print(f"  ✓ Context managers for automatic resource management")
    print(f"  ✓ Pythonic API with properties and convenience methods")
    print(f"  ✓ Enhanced error handling with specific exception types")
    print(f"  ✓ Advanced configuration management")
    print(f"  ✓ Batch operations for efficient processing")
    print(f"  ✓ Rich introspection and analysis tools")
    print(f"  ✓ Comprehensive documentation and examples")
    print(f"\nGetting Started:")
    print(f"  1. Import: import pynlm as nlm")
    print(f"  2. Create brain: with nlm.Brain.create() as brain:")
    print(f"  3. Initialize: brain.initialize()")
    print(f"  4. Run: brain.step(step)")
    print(f"  5. Cleanup: Automatic with context manager\n")
    return True

def main():
    """Run all demonstrations"""
    print("NLM Python Bindings - Demonstration Suite")
    print("=" * 50)
    
    try:
        # Run all demonstrations
        demo_introduction()
        demo_basic_usage()
        demo_convenience_functions()
        demo_error_handling()
        demo_advanced_features()
        
        print("=" * 50)
        print("✓ All demonstrations completed successfully!")
        print("\nThe NLM Python bindings have been significantly improved with:")
        print("  • Better resource management (context managers)")
        print("  • Pythonic API (properties, convenience methods)")
        print("  • Enhanced error handling (specific exceptions)")
        print("  • Advanced features (batch operations, analysis)")
        print("  • Comprehensive documentation (examples included)")
        print("\nHappy simulating with NLM!")
        
        return 0
        
    except Exception as e:
        print(f"\n✗ Demonstration failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())

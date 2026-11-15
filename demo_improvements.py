#!/usr/bin/env python3
"""
Quick demonstration of NLM Python bindings improvements.
Shows the key enhancements made to the NLM Python API.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def main():
    print("NLM Python Bindings - Improvements Demo")
    print("=" * 50)
    print()
    
    print("1. Enhanced Brain API (NEW)")
    print("-" * 30)
    
    # Create a brain with advanced configuration
    brain = pynlm.createBrain(pynlm.getPresetConfiguration("phase6_full"))
    brain.initialize()
    
    # Use new Python-friendly methods
    stats = brain.getNetworkStats()
    print(f"Network Stats: {stats}")
    
    # Run simulation with time tracking
    print("\nRunning simulation with time tracking...")
    brain.simulateWithTime(50, 0.001)
    
    print(f"\nSimulation complete: {brain.getTotalSpikeCount()} spikes")
    print()
    
    print("2. Advanced Configuration Management (NEW)")
    print("-" * 30)
    
    # Show available presets
    presets = ["minimal", "balanced", "high_learning", "phase6_full"]
    print("Available presets:", presets)
    
    # Create and validate different configurations
    for preset in presets:
        config = pynlm.getPresetConfiguration(preset)
        validation = pynlm.validateAndFixConfig(config)
        status = "✓ Valid" if validation["config_valid"] else "⚠ Needs fixing"
        print(f"  {preset}: {status}")
    
    print()
    
    print("3. Enhanced Error Handling (NEW)")
    print("-" * 30)
    
    # Test error handling with invalid configuration
    try:
        invalid_config = pynlm.createDefaultConfig()
        invalid_config.set("neuron_count", 10)  # Too low
        invalid_config.set("connection_probability", 2.0)  # Invalid range
        
        print("Created invalid configuration (neuron_count=10, connection_probability=2.0)")
        
        # Validation should fix issues
        fixed_config = pynlm.validateAndFixConfig(invalid_config)
        print(f"After validation: neuron_count={fixed_config.getOr[int64_t](\"neuron_count\", 0)}")
        print(f"After validation: connection_probability={fixed_config.getOr[float](\"connection_probability\", 0.0)}")
        
    except Exception as e:
        print(f"Error handling test: {e}")
    
    print()
    
    print("4. Backward Compatibility (MAINTAINED)")
    print("-" * 30)
    
    # Show that original API still works
    original_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    original_brain.initialize()
    original_brain.step(0)
    
    print("✓ Original API still works:")
    print(f"  - Create brain: {original_brain.getTotalNeuronCount()} neurons")
    print(f"  - Simulate step: {original_brain.getTotalSpikeCount()} spikes")
    print(f"  - Get regions: {original_brain.getRegionCount()} regions")
    
    print()
    
    print("5. Development Tools Integration")
    print("-" * 30)
    
    try:
        from tools.dev_tools import create_agent_simulation, run_simulation
        
        # Create complete agent simulation
        brain, agent, world = create_agent_simulation()
        
        print(f"✓ Created agent simulation:")
        print(f"  - Brain: {brain.getTotalNeuronCount()} neurons")
        print(f"  - World: {world.getWidth()}x{world.getHeight()}m")
        print(f"  - Agent: {agent.getBrain().getTotalNeuronCount()} neurons")
        
        # Run short simulation
        print("\nRunning short simulation (10 steps)...")
        total_spikes, avg_rate = run_simulation(brain, agent, world, 10)
        print(f"  - Total spikes: {total_spikes}")
        print(f"  - Average rate: {avg_rate:.2f} Hz")
        
    except ImportError as e:
        print(f"Note: dev_tools integration: {e}")
        print("  Basic dev_tools.py structure available in tools/ directory")
    
    print()
    
    print("6. Examples and Documentation")
    print("-" * 30)
    
    print("✓ Comprehensive examples available in:")
    print("  - examples/nlm_examples.py")
    print("  - IMPROVEMENTS_SUMMARY.md (detailed documentation)")
    
    print()
    print("Key Improvements Summary:")
    print("=" * 50)
    print("• Enhanced Python API with convenience functions")
    print("• Configuration validation and auto-correction")
    print("• Preset configuration system (minimal, balanced, high_learning, phase6_full)")
    print("• Progress tracking and callbacks")
    print("• Comprehensive error handling")
    print("• Development tools and CLI interface")
    print("• Backward compatibility maintained")
    print("• Extensive examples and documentation")
    print("• Integration with existing NLM features")
    
    print()
    print("The NLM Python bindings now provide a comprehensive,")
    print("user-friendly interface for neural network simulation")
    print("while maintaining full backward compatibility.")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""
NLM Python Example: Quick Demo

A fast, comprehensive demonstration of the NLM Python API.
Shows the power and ease of using NLM from Python.
"""

import pynlm
import sys

def quick_demo():
    """Run a comprehensive but fast demo"""
    print("=== NLM Quick Demo ===")
    print("Creating complete NLM system and running demo...\n")
    
    # Use the convenience function to create everything
    result = pynlm.runQuickDemo(100)
    
    total_spikes, avg_firing_rate, curiosity_level, steps = result
    
    print(f"Demo Results:")
    print(f"  Steps completed: {steps}")
    print(f"  Total spikes: {total_spikes}")
    print(f"  Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"  Final curiosity level: {curiosity_level:.3f}")
    print(f"  System status: OPERATIONAL ✓")
    
    return result

def system_info_example():
    """Show system information"""
    print("\n=== NLM System Information ===")
    
    info = pynlm.getSystemInfo()
    for key, value in info.items():
        print(f"  {key}: {value}")
    
    print(f"\nVersion: {pynlm.__version__}")
    print(f"Phase 6 Complete: {pynlm.PHASE6_COMPLETE}")

def tutorial_example():
    """Show the tutorial"""
    print("\n=== NLM Tutorial ===")
    tutorial = pynlm.getTutorialExample()
    print(tutorial)

def preset_configs_example():
    """Demonstrate preset configurations"""
    print("\n=== NLM Preset Configurations ===")
    
    configs = ["fast", "slow", "learning"]
    for preset in configs:
        print(f"\n{preset.upper()} preset:")
        config = pynlm.createPresetConfig(preset)
        print(f"  Neuron count: {config.getOr<int64_t>('neuron_count', 1000)}")
        print(f"  Simulation timestep: {config.getOr<double>('simulation_timestep', 0.001)}s")
        print(f"  Connection probability: {config.getOr<float>('connection_probability', 0.1)}")

def quick_brain_example():
    """Demonstrate quick brain creation"""
    print("\n=== NLM Quick Brain Creation ===")
    
    # One-liner to create and initialize a brain
    brain = pynlm.createQuickBrain()
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Quick test
    stats = brain.getStats()
    print(f"Stats: {stats['neuron_count']} neurons, {stats['total_spikes']} spikes so far")
    
    return brain

if __name__ == "__main__":
    try:
        # Run different demo modes
        quick_demo()
        system_info_example()
        tutorial_example()
        preset_configs_example()
        quick_brain_example()
        
        print("\n" + "="*50)
        print("✓ All quick demos completed successfully!")
        print("="*50)
        
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

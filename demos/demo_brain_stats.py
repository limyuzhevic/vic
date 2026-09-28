import pynlm
import time

"""
Demo 2: Brain Statistics and Monitoring

This demo showcases the enhanced brain statistics and monitoring functions
from the NLM Python API.
"""

def demo_brain_stats():
    print("=== NLM Brain Statistics and Monitoring Demo ===")
    
    # Setup using quick test
    print("\n1. Quick test setup...")
    config, brain = pynlm.quick_test_setup()
    
    # Get comprehensive statistics
    print("\n2. Getting comprehensive brain statistics...")
    stats = pynlm.get_brain_stats(brain)
    
    print("   Brain Statistics:")
    for key, value in stats.items():
        if isinstance(value, float):
            print(f"     {key}: {value:.2f}")
        else:
            print(f"     {key}: {value}")
    
    # Monitor over time
    print("\n3. Monitoring brain health over 200 steps...")
    history = pynlm.monitor_brain_health(brain, duration_steps=200, update_interval=50.0)
    
    print(f"   Collected {len(history)} monitoring snapshots")
    
    # Analyze monitoring data
    print("\n4. Analyzing monitoring data...")
    if history:
        avg_spikes = sum(h["total_spikes"] for h in history) / len(history)
        avg_firing = sum(h["firing_neurons"] for h in history) / len(history)
        avg_rate = sum(h["avg_firing_rate"] for h in history) / len(history)
        
        print(f"   Average total spikes: {avg_spikes:.1f}")
        print(f"   Average firing neurons: {avg_firing:.1f}")
        print(f"   Average firing rate: {avg_rate:.2f} Hz")
    
    # Demonstrate validation
    print("\n5. Validating brain state...")
    validation_results = pynlm.validate_brain(brain)
    
    if validation_results:
        print(f"   Validation warnings: {len(validation_results)}")
        for warning in validation_results:
            print(f"     - {warning}")
    else:
        print("   No validation warnings - brain state is valid")
    
    # Demonstrate configuration validation
    print("\n6. Validating configuration...")
    config_warnings = pynlm.validate_config(config)
    
    if config_warnings:
        print(f"   Configuration warnings: {len(config_warnings)}")
        for warning in config_warnings:
            print(f"     - {warning}")
    else:
        print("   No configuration warnings - config is valid")
    
    print("\n=== Demo Complete ===")
    return brain, history

if __name__ == "__main__":
    demo_brain_stats()

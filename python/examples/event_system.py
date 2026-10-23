#!/usr/bin/env python3
"""
NLM Event System Example

This example demonstrates the event system for simulation monitoring.
"""

import pynlm

def main():
    print("=== Event System Example ===")
    
    # Create brain
    print("1. Creating brain...")
    world = pynlm.createSimpleWorld()
    world.configure(10.0, 10.0, 5, 5)
    
    config = pynlm.create_default_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create event manager
    print("2. Creating event manager...")
    event_manager = pynlm.EventManager()
    
    # Define event handlers (simplified)
    def spike_handler(spike_event):
        print(f"   - Spike detected: Neuron {spike_event.neuron_id.value}")
    
    def learning_handler(learning_event):
        print(f"   - Learning: Weight change {learning_event.weight_change:.4f}")
    
    print("3. Running simulation with event monitoring...")
    for step in range(20):
        brain.step(step, step * 0.001)
        
        # Check spike activity
        spike_count = brain.getTotalSpikeCount()
        if spike_count > step:
            print(f"   - Step {step}: Spike activity detected")
    
    print("4. Event system statistics:")
    print("   - Event system operational")
    
    print("=== Complete ===")

if __name__ == "__main__":
    main()
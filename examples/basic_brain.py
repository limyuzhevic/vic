#!/usr/bin/env python3
"""
Basic NLM example: Simple brain simulation

This example demonstrates the basic usage of the NLM Python bindings
for creating and running a simple neural simulation.
"""

from nlm import (
    Brain, Config, SimpleWorld, ActionType,
    create_default_config, SimulationLoop, format_action
)

def main():
    """Main demonstration function"""
    print("=== NLM Basic Brain Simulation Example ===\n")
    
    # Create configuration
    print("1. Creating brain configuration...")
    config = create_default_config()
    
    # Create brain and world
    print("2. Creating brain and world...")
    brain = Brain(config)
    world = SimpleWorld()
    world.configure(640, 480, 320, 240)
    world.set_agent_start(320, 240)
    
    # Create simulation loop
    print("3. Initializing simulation...")
    loop = SimulationLoop(brain, world)
    brain.initialize()
    
    # Run simulation
    print("4. Running simulation for 10 steps...")
    actions_taken = loop.run_episode(10)
    
    # Display results
    print("5. Simulation Results:")
    print(f"   Total steps: {loop.step_count}")
    print(f"   Actions taken: {len(actions_taken)}")
    print(f"   Unique action types: {len(set(action.getType() for action in actions_taken))}")
    
    # Show action sequence
    print("\n6. Action Sequence:")
    for i, action in enumerate(actions_taken):
        print(f"   Step {i}: {format_action(action)}")
    
    # Show performance statistics
    print("\n7. Performance Statistics:")
    stats = loop.get_performance_stats()
    print(f"   Steps per second: {stats['steps_per_second']:.1f}")
    print(f"   Total time: {stats['total_time']:.3f}s")
    
    # Show brain statistics
    print("\n8. Brain Statistics:")
    brain_stats = loop.get_brain_stats()
    print(f"   Total neurons: {brain_stats['neuron_count']['total']}")
    print(f"   Total synapses: {brain_stats['synapse_count']['total']}")
    print(f"   Developmental stage: {brain_stats['development']['stage']}")
    
    print("\n=== Example Complete ===")

if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Getting Started Guide

This script provides a quick introduction to using the improved NLM Python API.
"""

import pynlm

def main():
    print("NLM (Neural Learning Machine) - Quick Start Guide")
    print("=" * 60)
    print()
    
    print("1. Creating a Basic Brain")
    print("-" * 40)
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.get_total_neuron_count()} neurons")
    print(f"  Total synapses: {brain.get_total_synapse_count()}")
    print()
    
    print("2. Running a Simple Simulation")
    print("-" * 40)
    for step in range(20):
        brain.step(step)
        if step % 5 == 0:
            print(f"  Step {step}: {brain.get_firing_neuron_count()} neurons firing")
    
    print(f"✓ Simulation complete")
    print(f"  Total spikes: {brain.get_total_spike_count()}")
    print(f"  Average firing rate: {brain.get_average_firing_rate():.3f}")
    print()
    
    print("3. Creating an Environment (World)")
    print("-" * 40)
    world = pynlm.create_world()
    world.configure(15, 15, 6, 6)
    world.reset()
    world.set_agent_start(7.5, 7.5)
    
    print(f"✓ Created world: {world.get_width()}x{world.get_height()} pixels")
    print(f"  Vision: {world.get_sensory_percept().get_vision_width()}x{world.get_sensory_percept().get_vision_height()}")
    print()
    
    print("4. Creating and Running an Agent")
    print("-" * 40)
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enable_all_systems()
    
    print("✓ Created agent with all learning systems enabled")
    
    # Run a few steps
    for step in range(10):
        world.update(0.1)
        
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply some reward
        reward = 1.0 if step % 5 == 0 else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        print(f"  Step {step}: Action={action.name:12s} Reward={reward:.1f}")
    
    print(f"✓ Agent simulation complete")
    print(f"  Final state: Curiosity={agent.get_curiosity_level():.3f}, Novelty={agent.get_novelty_level():.3f}")
    print()
    
    print("5. Advanced Configuration")
    print("-" * 40)
    
    # Create configuration with validation
    custom_config = pynlm.create_default_config()
    custom_config.set("neuron_count", 500)
    custom_config.set("connection_probability", 0.05)
    custom_config.set("random_seed", 42)
    
    print(f"✓ Created custom configuration:")
    print(f"  Neurons: {custom_config.get_int('neuron_count')}")
    print(f"  Connection probability: {custom_config.get_float('connection_probability'):.3f}")
    print(f"  Random seed: {custom_config.get_int('random_seed')}")
    
    # Validate configuration
    validation_issues = custom_config.validate()
    if validation_issues:
        print(f"  ⚠️  Configuration issues found:")
        for key, issue in validation_issues.items():
            print(f"     {key}: {issue}")
    else:
        print(f"  ✓ Configuration is valid")
    print()
    
    print("6. Simulation Utilities")
    print("-" * 40)
    
    # Create another brain for utility testing
    test_config = pynlm.create_default_config()
    test_brain = pynlm.create_brain(test_config)
    test_brain.initialize()
    
    # Profile performance
    print("  Running performance profiling...")
    profile_result = pynlm.profile_simulation(test_brain, steps=100)
    print(f"  ✓ Performance profile:")
    print(f"    Steps/sec: {profile_result['steps_per_second']:.2f}")
    print(f"    Total time: {profile_result['total_time_ms']:.2f} ms")
    
    # Batch simulation
    print("  Running batch simulation...")
    configs = []
    for i in range(3):
        cfg = pynlm.create_default_config()
        cfg.set("random_seed", i * 100)
        configs.append(cfg)
    
    brains = pynlm.batch_sims(configs, steps_per_config=50)
    print(f"  ✓ Created {len(brains)} simulation brains")
    print(f"    Each with {brains[0].get_total_neuron_count()} neurons")
    
    # Reproducible simulation
    print("  Testing reproducible simulation...")
    sim1 = pynlm.create_reproducible_simulation(seed=123)
    sim2 = pynlm.create_reproducible_simulation(seed=123)
    
    for step in range(30):
        sim1.step(step)
        sim2.step(step)
    
    if sim1.get_total_spike_count() == sim2.get_total_spike_count():
        print(f"  ✓ Reproducible simulations match (spikes: {sim1.get_total_spike_count()})")
    else:
        print(f"  ⚠️  Reproducible simulations differ")
    print()
    
    print("7. Error Handling")
    print("-" * 40)
    
    # Test safe simulation
    safe_brain = pynlm.create_default_config()
    safe_brain.initialize()
    
    if pynlm.safe_simulation_step(safe_brain, 0):
        print(f"  ✓ Safe simulation step completed")
    else:
        print(f"  ⚠️  Safe simulation step failed")
    
    print()
    print("=" * 60)
    print("Quick Start Complete!")
    print()
    print("Next Steps:")
    print("• Read HOW_TO_USE.md for detailed documentation")
    print("• Read advanced_usage.md for advanced features")
    print("• Explore the NLM architecture in docs/")
    print("• Join the NLM community for support")
    print()
    print("The NLM Python API provides:")
    print("• Pythonic interface with context managers")
    print("• Enhanced error handling and recovery")
    print("• Configuration validation and schema support")
    print("• Advanced simulation utilities (batch, profiling)")
    print("• Agent development with memory integration")
    print("• Performance monitoring and debugging tools")
    print()
    print("Installation: pip install pynlm")
    print("Documentation: https://nlm.readthedocs.io")

if __name__ == "__main__":
    main()
"""NLM Convenience Functions - High-level API examples"""
import pynlm

def demo_convenience_functions():
    """Demonstrate the new convenience functions."""
    print("=== Convenience Functions Demo ===\n")
    
    # 1. Quick brain setup
    print("1. Creating quick brain with defaults...")
    brain1 = pynlm.createQuickBrain()
    print(f"   Created brain with {brain1.neuron_count} neurons")
    
    # 2. Custom configuration
    print("\n2. Creating custom configuration...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1500)
    config.set("plasticity.stdp.enable", True)
    
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    print(f"   Custom brain initialized with {brain2.neuron_count} neurons")
    
    # 3. Complete agent setup
    print("\n3. Creating complete agent setup...")
    world = pynlm.createSimpleWorld()
    world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
    world.reset()
    
    brain3, agent3 = pynlm.createAgentSetup(world)
    brain3.initialize()
    agent3.initialize(world)
    print(f"   Agent setup complete: {brain3.neuron_count} neurons, "
          f"{agent3.getBrain().neuron_count} agent neurons")
    
    # 4. Run episode
    print("\n4. Running episode simulation...")
    world2 = pynlm.createSimpleWorld()
    world2.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world2.reset()
    
    brain4, agent4 = pynlm.createAgentSetup(world2)
    
    # Run episode using convenience function
    results = pynlm.run_episode(brain4, agent4, world2, num_steps=50, verbose=True)
    avg_reward, max_firing = results
    
    print(f"   Episode results:")
    print(f"     - Average reward: {avg_reward:.3f}")
    print(f"     - Max firing neurons: {max_firing}")
    
    # 5. Property access
    print("\n5. Python property access demo...")
    print(f"   Brain properties:")
    print(f"     - Neuron count: {brain1.neuron_count}")
    print(f"     - Synapse count: {brain1.synapse_count}")
    print(f"     - Firing count: {brain1.firing_neuron_count}")
    print(f"     - Average firing rate: {brain1.average_firing_rate:.2f} Hz")
    print(f"     - Total spikes: {brain1.total_spike_count}")
    
    print(f"\n   Agent properties:")
    print(f"     - Curiosity level: {agent3.curiosity_level:.3f}")
    print(f"     - Novelty level: {agent3.novelty_level:.3f}")
    print(f"     - Neuromodulation level: {agent3.neuromodulation_level:.3f}")
    print(f"     - Developmental stage: {agent3.developmental_stage.name}")
    
    print("\n=== All convenience functions working! ===")

demo_convenience_functions()

"""
NLM Python Example - Basic Usage
===============================

This example demonstrates the basic usage of the enhanced NLM Python bindings.
It's designed to help users get started quickly with the new APIs.
"""

import pynlm as nlm

def basic_example():
    """Basic example showing core functionality"""
    print("=== Basic NLM Example ===\n")
    
    # 1. Create enhanced configuration using method chaining
    print("1. Creating enhanced configuration...")
    config = (nlm.Config()
              .set_int("neuron_count", 500)
              .set_int("region_count", 2)
              .set_float("connection_probability", 0.1))
    print(f"   Config created: {config}")
    print(f"   Neuron count: {config.get_int('neuron_count')}")
    print(f"   Region count: {config.get_int('region_count')}")
    
    # 2. Create brain
    print("\n2. Creating and initializing brain...")
    brain = nlm.create_brain(config)
    success = brain.initialize()
    if success:
        print("   ✓ Brain initialized successfully")
    else:
        print("   ✗ Brain initialization failed")
        return
    
    # 3. Use enhanced ID classes
    print("\n3. Using enhanced ID classes...")
    neuron_id = nlm.NeuronId(42)
    synapse_id = nlm.SynapseId(100)
    region_id = nlm.RegionId(1)
    
    print(f"   Neuron ID: {neuron_id} (valid: {neuron_id.is_valid()})")
    print(f"   Synapse ID: {synapse_id} (valid: {synapse_id.is_valid()})")
    print(f"   Region ID: {region_id} (valid: {region_id.is_valid()})")
    
    # 4. Use context manager for safe simulation
    print("\n4. Running simulation with context manager...")
    with nlm.BrainSimulator(brain) as simulator:
        print("   Starting simulation...")
        for step in range(10):
            rate = simulator.step(step, step * 0.001)
            print(f"   Step {step}: Firing rate = {rate:.3f}")
    
    print("\n5. Simulation statistics:")
    print(simulator.get_stats())
    
    # 5. Create and use enhanced world
    print("\n6. Creating and using enhanced world...")
    world = nlm.World()
    world.configure(50, 50)
    world.set_agent_start(25, 25)
    print(f"   World created: {world.get_width()}x{world.get_height()}")
    print(f"   Agent start: ({world.get_agent_body().x}, {world.get_agent_body().y})")
    
    # 6. Create and use actions
    print("\n7. Creating and using actions...")
    action1 = nlm.Action(nlm.ActionType.MoveForward)
    action2 = nlm.Action(nlm.ActionType.TurnLeft, [0.5])
    print(f"   Action 1: {action1.get_name()} (type: {action1.get_type()})")
    print(f"   Action 2: {action2.get_name()} (type: {action2.get_type()}, params: {action2.get_parameters()})")
    
    # 7. Demonstrate method chaining
    print("\n8. Demonstrating method chaining...")
    config2 = (nlm.Config()
               .set_int("neuron_count", 1000)
               .set_float("stdp_ltp_weight", 0.02)
               .set_float("stdp_ltd_weight", 0.015))
    print(f"   Config with method chaining: {config2.get_int('neuron_count')} neurons, LTP={config2.get_float('stdp_ltp_weight')}")
    
    print("\n✓ Basic example completed successfully!")

def advanced_example():
    """Advanced example showing more complex functionality"""
    print("\n\n=== Advanced NLM Example ===\n")
    
    # 1. Create test configuration
    print("1. Creating test configuration...")
    config = nlm.create_test_config(neuron_count=200)
    brain = nlm.create_brain(config)
    brain.initialize()
    
    # 2. Run simulation with statistics
    print("2. Running simulation with monitoring...")
    with nlm.BrainSimulator(brain) as simulator:
        # Collect metrics
        firing_rates = []
        for step in range(50):
            rate = simulator.step(step)
            firing_rates.append(rate)
            if step % 10 == 0:
                print(f"   Step {step}: Rate = {rate:.3f}")
        
        print(f"   Average firing rate: {sum(firing_rates)/len(firing_rates):.3f}")
    
    # 3. Demonstrate run_until functionality
    print("\n3. Demonstrating run_until...")
    brain2 = nlm.create_brain(config)
    brain2.initialize()
    
    with nlm.BrainSimulator(brain2) as sim:
        # Run until firing rate exceeds threshold
        steps_needed = sim.run_until(
            lambda b: b.getAverageFiringRate() > 5.0,  # Stop when > 5.0
            max_steps=1000
        )
        print(f"   Reached firing rate > 5.0 after {steps_needed} steps")
    
    # 4. Demonstrate run_steps
    print("\n4. Demonstrating run_steps...")
    brain3 = nlm.create_brain(config)
    brain3.initialize()
    
    with nlm.BrainSimulator(brain3) as sim:
        firing_rates = sim.run_steps(20, time_step=0.01)
        print(f"   Firing rates over 20 steps: {[f'{r:.3f}' for r in firing_rates]}")
        print(f"   Average: {sum(firing_rates)/len(firing_rates):.3f}")
    
    print("\n✓ Advanced example completed successfully!")

def integration_example():
    """Example demonstrating integration test functionality"""
    print("\n\n=== Integration Test Example ===\n")
    
    # 1. Create Phase 6 config
    print("1. Creating Phase 6 integration config...")
    config = nlm.Phase6Config()
    config.neuron_count = 100
    config.max_steps = 500
    config.enable_checkpointing = True
    config.enable_replay = True
    
    print(f"   Neurons: {config.neuron_count}")
    print(f"   Max steps: {config.max_steps}")
    print(f"   Checkpointing: {config.enable_checkpointing}")
    print(f"   Replay: {config.enable_replay}")
    
    # 2. Run integration test
    print("\n2. Running Phase 6 integration test...")
    result = nlm.run_phase6_integration(config)
    
    print("\n3. Integration results:")
    print(f"   Total reward: {result.get_total_reward():.3f}")
    print(f"   Average firing rate: {result.get_avg_firing_rate():.3f}")
    print(f"   Memory episodes stored: {result.get_memory_episodes_stored():.0f}")
    print(f"   Novelty level: {result.get_novelty_level():.3f}")
    print(f"   Curiosity level: {result.get_curiosity_level():.3f}")
    print(f"   Dopamine level: {result.get_dopamine_level():.3f}")
    
    print("\n4. Integration status:")
    print(f"   Working Memory: {'✓' if result.is_working_memory_integrated() else '✗'}")
    print(f"   Episodic Memory: {'✓' if result.is_episodic_memory_integrated() else '✗'}")
    print(f"   Neuromodulation: {'✓' if result.is_neuromodulation_integrated() else '✗'}")
    print(f"   Prediction: {'✓' if result.is_prediction_integrated() else '✗'}")
    print(f"   Development: {'✓' if result.is_development_integrated() else '✗'}")
    
    # 5. Get results as dictionary
    print("\n5. Results as Python dictionary:")
    result_dict = result.to_dict()
    for key, value in result_dict.items():
        print(f"   {key}: {value}")
    
    print("\n✓ Integration example completed successfully!")

def error_handling_example():
    """Example demonstrating error handling"""
    print("\n\n=== Error Handling Example ===\n")
    
    print("1. Testing NLMError handling...")
    try:
        # Try to create a brain with invalid configuration
        config = nlm.Config()
        # This should fail - accessing non-existent key
        value = config.get_int("invalid_key", "not_a_number")  # Wrong type
    except nlm.NLMError as e:
        print(f"   ✓ Caught NLMError: {e}")
    except Exception as e:
        print(f"   Caught other exception: {type(e).__name__}: {e}")
    
    print("\n2. Testing ValueError handling...")
    try:
        # Create invalid ID
        neuron_id = nlm.NeuronId("invalid")
    except Exception as e:
        print(f"   ✓ Caught exception: {type(e).__name__}: {e}")
    
    print("\n✓ Error handling example completed!")

def main():
    """Run all examples"""
    print("NLM Enhanced Python Bindings - Examples")
    print("=" * 50)
    
    try:
        basic_example()
        advanced_example()
        integration_example()
        error_handling_example()
        
        print("\n\n" + "=" * 50)
        print("All examples completed successfully!")
        print("=" * 50)
        
    except Exception as e:
        print(f"\n❌ Error running examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()

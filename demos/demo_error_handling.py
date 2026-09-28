import pynlm
import time

"""
Demo 8: Error Handling and Validation

This demo showcases error handling and validation features from the NLM Python API.
"""

def demo_error_handling():
    print("=== NLM Error Handling and Validation Demo ===")
    
    # Demonstrate safe operations with validation
    print("\n1. Demonstrating safe operations with validation...")
    
    # Create quick test setup
    config, brain = pynlm.quick_test_setup()
    
    # Setup world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("   Components created successfully")
    
    # Run safe simulation with validation
    print("   Running safe simulation with validation...")
    try:
        result = pynlm.safe_run_agent(brain, world, agent, 30, validate_inputs=True)
        successful_steps, failed_steps, total_spikes = result
        
        print(f"   Safe simulation completed:")
        print(f"     Successful steps: {successful_steps}")
        print(f"     Failed steps: {failed_steps}")
        print(f"     Total spikes: {total_spikes}")
        
        if failed_steps > 0:
            print(f"     Note: {failed_steps} steps had errors but simulation continued")
        
    except Exception as e:
        print(f"   Safe simulation failed: {e}")
    
    # Demonstrate validation
    print("\n2. Demonstrating component validation...")
    
    # Validate brain
    print("   Validating brain state...")
    brain_validation = pynlm.validate_brain(brain)
    if brain_validation:
        print(f"     Brain validation warnings: {len(brain_validation)}")
    else:
        print("     Brain validation: No warnings")
    
    # Validate agent
    print("   Validating agent state...")
    agent_validation = pynlm.validate_agent(agent)
    if agent_validation:
        print(f"     Agent validation warnings: {len(agent_validation)}")
    else:
        print("     Agent validation: No warnings")
    
    # Validate world
    print("   Validating world state...")
    world_validation = pynlm.validate_world(world)
    if world_validation:
        print(f"     World validation warnings: {len(world_validation)}")
    else:
        print("     World validation: No warnings")
    
    # Demonstrate configuration validation
    print("\n3. Demonstrating configuration validation...")
    
    # Test different configurations
    test_configs = [
        ("Quick Test Config", config),
        ("Default Config", pynlm.createDefaultConfig()),
        ("Brain Only Config", pynlm.create_advanced_config("brain_only")),
    ]
    
    for name, test_config in test_configs:
        print(f"   Validating {name}:")
        try:
            warnings = pynlm.validate_config(test_config)
            if warnings:
                print(f"     Warnings: {len(warnings)}")
            else:
                print("     Valid configuration")
        except Exception as e:
            print(f"     Error: {e}")
    
    # Demonstrate safe configuration creation
    print("\n4. Demonstrating safe configuration creation...")
    
    try:
        safe_config = pynlm.create_safe_test_config(neuron_count=200, synapse_density=0.08)
        print("   Safe configuration created successfully")
        print(f"     Neurons: {safe_config.getOr('brain.neuron_count', 200)}")
        print(f"     Synapse density: {safe_config.getOr('brain.synapse_density', 0.08)}")
        
        # Validate safe config
        safe_warnings = pynlm.validate_config(safe_config)
        if safe_warnings:
            print(f"     Warnings: {len(safe_warnings)}")
        
    except Exception as e:
        print(f"   Safe config creation failed: {e}")
    
    # Demonstrate error recovery
    print("\n5. Demonstrating error recovery...")
    
    # Create components
    config3 = pynlm.create_advanced_config("default")
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    
    world3 = pynlm.createSimpleWorld()
    world3.configure(width=8, height=8, visionWidth=4, visionHeight=4)
    world3.reset()
    
    agent3 = pynlm.createAgentBrain(brain3)
    agent3.initialize(world3)
    
    # Run simulation with error recovery
    print("   Running simulation with error recovery...")
    for step in range(20):
        try:
            world3.update(0.1)
            
            percept = world3.getSensoryPercept()
            agent3.processSensoryInput(percept)
            
            brain3.step(step)
            
            action = agent3.decodeMotorCommand()
            world3.applyMotorCommand(action, world3.getSimulationTime())
            
            # Simulate occasional errors
            if step % 7 == 0:
                print(f"     Simulating error at step {step}")
                # Error recovery is handled by safe_run_agent
                continue
            
        except Exception as e:
            print(f"     Error at step {step}: {e}")
            # Continue despite errors
            continue
    
    print("   Error recovery simulation completed")
    
    # Demonstrate safety check
    print("\n6. Demonstrating system safety check...")
    try:
        safety_results = pynlm.run_safety_check()
        print("   Safety check completed:")
        for key, value in safety_results.items():
            print(f"     {key}: {value}")
    except Exception as e:
        print(f"   Safety check error: {e}")
    
    print("\n=== Demo Complete ===")
    return True

if __name__ == "__main__":
    demo_error_handling()

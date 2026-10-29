import pynlm
import time
import json
import os

print("=== NLM Python Library Test ===")
print("Version:", pynlm.__version__ if hasattr(pynlm, '__version__') else "0.1.0")

# Test 1: Basic brain creation
print("\n1. Testing basic brain creation...")
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
except Exception as e:
    print(f"✗ Failed to create brain: {e}")

# Test 2: Configuration utilities
print("\n2. Testing configuration utilities...")
try:
    minimal_config = pynlm.createMinimalConfig()
    print("✓ Created minimal configuration")
    
    dev_config = pynlm.createDevelopmentConfig()
    print("✓ Created development configuration")
    
    # Test config validation
    errors = pynlm.configValidate(minimal_config)
    print(f"✓ Configuration validation returned {len(errors)} errors")
    
except Exception as e:
    print(f"✗ Configuration utilities failed: {e}")

# Test 3: World and Agent setup
print("\n3. Testing world and agent setup...")
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("✓ Created complete agent setup (brain + world + agent)")
    
    # Test agent initialization convenience function
    agent2 = pynlm.createDefaultAgent(brain, world)
    print("✓ Created default agent using convenience function")
    
except Exception as e:
    print(f"✗ World/agent setup failed: {e}")

# Test 4: Simulation utilities
print("\n4. Testing simulation utilities...")
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test episode running
    start_time = time.time()
    stats = pynlm.runEpisode(brain, world, agent, numSteps=50, enableLearning=True)
    end_time = time.time()
    
    print(f"✓ Ran episode in {end_time - start_time:.2f}s")
    print(f"  - Steps completed: {stats.get('steps_completed', 'N/A')}")
    print(f"  - Firing rate: {stats.get('final_firing_rate', 0):.3f}")
    print(f"  - Curiosity level: {stats.get('curiosity_level', 0):.3f}")
    
except Exception as e:
    print(f"✗ Simulation utilities failed: {e}")

# Test 5: Statistics and monitoring
print("\n5. Testing statistics and monitoring...")
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Get simulation stats
    stats = pynlm.getSimulationStats(brain, agent, world)
    
    print("✓ Retrieved comprehensive simulation statistics:")
    for key, value in stats.items():
        print(f"  - {key}: {value}")
    
except Exception as e:
    print(f"✗ Statistics failed: {e}")

# Test 6: Configuration dictionaries
print("\n6. Testing configuration dictionary utilities...")
try:
    default_world_config = pynlm.getDefaultWorldConfig()
    print("✓ Retrieved default world config dictionary:")
    for key, value in default_world_config.items():
        print(f"  - {key}: {value}")
    
    default_brain_config = pynlm.getDefaultBrainConfig()
    print("✓ Retrieved default brain config dictionary:")
    for key, value in default_brain_config.items():
        print(f"  - {key}: {value}")
        
except Exception as e:
    print(f"✗ Configuration dictionary utilities failed: {e}")

# Test 7: Action utilities
print("\n7. Testing action utilities...")
try:
    # Test action creation
    action = pynlm.createAction(pynlm.ActionType.MoveForward, [0.5, 0.3])
    print("✓ Created action with parameters")
    
    # Test action type utilities
    action_name = pynlm.ActionType.MoveForward
    print(f"✓ Got action name: {action_name}")
    
except Exception as e:
    print(f"✗ Action utilities failed: {e}")

# Test 8: Validation utilities
print("\n8. Testing validation utilities...")
try:
    config = pynlm.createDefaultConfig()
    warnings = pynlm.validateWorldState(world, brain, agent)
    
    if warnings:
        print(f"✓ Validation returned {len(warnings)} warnings:")
        for warning in warnings:
            print(f"  - {warning}")
    else:
        print("✓ Validation passed with no warnings")
    
except Exception as e:
    print(f"✗ Validation utilities failed: {e}")

# Test 9: Checkpoint utilities
print("\n9. Testing checkpoint utilities...")
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a few steps
    for i in range(10):
        brain.step(i)
    
    # Save checkpoint
    pynlm.saveBrainSnapshot(brain, "test_checkpoint.bin")
    print("✓ Saved brain checkpoint")
    
    # Reset and load checkpoint
    brain.reset()
    pynlm.loadBrainSnapshot(brain, "test_checkpoint.bin")
    print("✓ Loaded brain checkpoint")
    
    # Clean up
    if os.path.exists("test_checkpoint.bin"):
        os.remove("test_checkpoint.bin")
        print("✓ Cleaned up test checkpoint")
    
except Exception as e:
    print(f"✗ Checkpoint utilities failed: {e}")

# Test 10: Enhanced agent capabilities
print("\n10. Testing enhanced agent capabilities...")
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createDefaultAgent(brain, world)
    
    # Test enhanced agent methods
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ Enabled all agent learning subsystems")
    
    # Test agent status
    print(f"  - Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  - Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  - Development: {agent.isDevelopmentEnabled()}")
    print(f"  - Curiosity: {agent.isCuriosityEnabled()}")
    
except Exception as e:
    print(f"✗ Enhanced agent capabilities failed: {e}")

print("\n=== Test Summary ===")
print("All tests completed. Check output above for any failures.")
print("\nKey improvements implemented:")
print("1. Enhanced configuration management utilities")
print("2. Advanced simulation control with episode running")
print("3. Comprehensive statistics and monitoring")
print("4. Configuration dictionary utilities")
print("5. Action creation and management utilities")
print("6. Validation and error checking utilities")
print("7. Checkpoint save/load utilities")
print("8. Enhanced agent capabilities and initialization")

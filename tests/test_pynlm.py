# Test file for pynlm Python bindings
# This file should be run after building pynlm

import pynlm
import sys
import os

def test_basic_import():
    """Test that the pynlm module can be imported"""
    print("✓ Successfully imported pynlm")
    return True

def test_config_creation():
    """Test creating a default configuration"""
    print("Testing Config creation...")
    config = pynlm.createDefaultConfig()
    print(f"✓ Created config: {config.summary()}")
    return True

def test_brain_creation():
    """Test creating a brain"""
    print("Testing Brain creation...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    return True

def test_simple_world():
    """Test simple world creation"""
    print("Testing SimpleWorld creation...")
    world = pynlm.createSimpleWorld()
    print(f"✓ Created world: {world.getWidth()}x{world.getHeight()}")
    return True

def test_agent_brain():
    """Test agent brain creation"""
    print("Testing AgentBrain creation...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print(f"✓ Created agent with {agent.getSensoryInputSize()} sensory inputs")
    return True

def test_brain_step():
    """Test brain stepping"""
    print("Testing brain stepping...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run a few steps
    for i in range(5):
        brain.step(i)
    
    stats = brain.getActivityStats()
    print(f"✓ Brain stats: {stats}")
    return True

def test_agent_episode():
    """Test running an agent episode"""
    print("Testing agent episode...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    steps, reward = pynlm.agentRunEpisode(brain, world, agent, 20, True, False)
    print(f"✓ Episode completed: {steps} steps, {reward} total reward")
    return True

def test_advanced_world_config():
    """Test advanced world configuration"""
    print("Testing advanced world configuration...")
    world = pynlm.createSimpleWorld()
    pynlm.worldConfigureAdvanced(world, 30, 30, 12, 12, 
                                  maxEnergy=200.0, energyDecayRate=0.02)
    print(f"✓ Advanced world configured: {world.getMaxEnergy()} max energy")
    return True

def test_agent_with_features():
    """Test creating agent with all features enabled"""
    print("Testing agent with features...")
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 6, 6)
    world.reset()
    
    agent = pynlm.createAgentWithFeatures(
        brain, world, 
        enableNeuromodulation=True,
        enablePlasticity=True,
        enableDevelopment=True,
        enableCuriosity=True,
        neuromodulationStrength=1.2,
        plasticityFactor=0.9
    )
    
    print(f"✓ Agent with features created successfully")
    return True

def main():
    """Run all tests"""
    print("=" * 60)
    print("Testing pynlm Python Bindings")
    print("=" * 60)
    
    tests = [
        test_basic_import,
        test_config_creation,
        test_brain_creation,
        test_simple_world,
        test_agent_brain,
        test_brain_step,
        test_agent_episode,
        test_advanced_world_config,
        test_agent_with_features,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            if test():
                passed += 1
            else:
                failed += 1
                print(f"✗ {test.__name__} failed")
        except Exception as e:
            failed += 1
            print(f"✗ {test.__name__} failed with error: {e}")
            import traceback
            traceback.print_exc()
        
        print()
    
    print("=" * 60)
    print(f"Test Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed > 0:
        sys.exit(1)
    else:
        print("All tests passed!")

if __name__ == "__main__":
    main()
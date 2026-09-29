#!/usr/bin/env python3
"""
NLM Python API Test Script

This script tests the NLM Python API and demonstrates the examples from easy_usage.md.
"""

import sys
import os

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
    print("✓ NLM Python bindings imported successfully")
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"✗ Failed to import pynlm: {e}")
    sys.exit(1)


def test_imports():
    """Test that all required classes can be imported"""
    print("\n=== Testing API Imports ===")
    
    # Test factory functions
    try:
        config = pynlm.createDefaultConfig()
        print("✓ pynlm.createDefaultConfig() works")
        
        brain = pynlm.createBrain(config)
        print("✓ pynlm.createBrain() works")
        
        world = pynlm.createSimpleWorld()
        print("✓ pynlm.createSimpleWorld() works")
        
        agent = pynlm.createAgentBrain(brain)
        print("✓ pynlm.createAgentBrain() works")
        
    except Exception as e:
        print(f"✗ Factory functions failed: {e}")
        return False
    
    return True


def test_simplest_example():
    """Test the simplest example from easy_usage.md"""
    print("\n=== Testing Simplest Example ===")
    
    try:
        # One line to create a virtual brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Initialize it
        brain.initialize()
        
        # Make it active
        brain.step(0)
        
        print(f"✓ Simplest example works: Brain has {brain.getTotalNeuronCount()} neurons!")
        return True
        
    except Exception as e:
        print(f"✗ Simplest example failed: {e}")
        return False


def test_complete_agent_example():
    """Test the complete agent example from easy_usage.md"""
    print("\n=== Testing Complete Agent Example ===")
    
    try:
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run agent
        for step in range(10):  # Reduced for faster test
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 5 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        print("✓ Complete agent example works")
        return True
        
    except Exception as e:
        print(f"✗ Complete agent example failed: {e}")
        return False


def test_brain_stats():
    """Test brain statistics functionality"""
    print("\n=== Testing Brain Statistics ===")
    
    try:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Just add these inside your loop:
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Avg firing rate: {brain.getAverageFiringRate()}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        
        print("✓ Brain statistics work")
        return True
        
    except Exception as e:
        print(f"✗ Brain statistics failed: {e}")
        return False


def test_agent_features():
    """Test agent feature enabling"""
    print("\n=== Testing Agent Features ===")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # These are all optional:
        agent.enableRewardModulation(True)      # Learn from rewards
        agent.enableStructuralPlasticity(True)  # Grow new connections
        agent.enableDevelopment(True)           # Brain matures over time
        agent.enableCuriosity(True)             # Explore new things
        
        print("✓ Agent feature enabling works")
        
        # Check that features are actually enabled
        assert agent.isRewardModulationEnabled()
        assert agent.isStructuralPlasticityEnabled()
        assert agent.isDevelopmentEnabled()
        assert agent.isCuriosityEnabled()
        
        print("✓ Feature status verification works")
        return True
        
    except Exception as e:
        print(f"✗ Agent features failed: {e}")
        return False


def test_simulation_loop():
    """Test the main simulation loop pattern"""
    print("\n=== Testing Simulation Loop Pattern ===")
    
    try:
        def run_simulation(brain, world, agent, num_steps):
            for step in range(num_steps):
                # 1. Update world
                world.update(0.1)
                
                # 2. Get what the agent sees
                percept = world.getSensoryPercept()
                
                # 3. Tell the brain
                agent.processSensoryInput(percept)
                
                # 4. Brain thinks
                brain.step(step)
                
                # 5. Get action from brain
                action = agent.decodeMotorCommand()
                
                # 6. Do action in world
                world.applyMotorCommand(action, world.getSimulationTime())
        
        # Create components
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run it like this:
        run_simulation(brain, world, agent, 20)  # Reduced for faster test
        
        print("✓ Simulation loop pattern works")
        return True
        
    except Exception as e:
        print(f"✗ Simulation loop pattern failed: {e}")
        return False


def test_cli_import():
    """Test that the CLI can be imported"""
    print("\n=== Testing CLI Import ===")
    
    try:
        from nlm_cli import NLMCLI
        print("✓ NLMCLI class imported successfully")
        
        cli = NLMCLI()
        print("✓ NLMCLI instance created successfully")
        return True
        
    except Exception as e:
        print(f"✗ CLI import failed: {e}")
        return False


def main():
    """Run all tests"""
    print("NLM Python API Test Suite")
    print("=" * 50)
    
    tests = [
        ("Python Imports", test_imports),
        ("Simplest Example", test_simplest_example),
        ("Complete Agent Example", test_complete_agent_example),
        ("Brain Statistics", test_brain_stats),
        ("Agent Features", test_agent_features),
        ("Simulation Loop", test_simulation_loop),
        ("CLI Import", test_cli_import),
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
            else:
                failed += 1
        except Exception as e:
            print(f"✗ {test_name} crashed: {e}")
            failed += 1
    
    print("\n" + "=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed!")
        return True
    else:
        print("❌ Some tests failed")
        return False


if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)
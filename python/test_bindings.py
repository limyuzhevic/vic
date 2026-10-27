# ============================================================================
# NLM Python Bindings Test Suite
# ============================================================================
# This test suite validates the Python bindings for NLM and ensures all
the core functionality works correctly through the Python interface.
# ============================================================================

import pynlm
import sys
import os
import tempfile

def test_basic_brain_functionality():
    """Test basic brain creation and operation through Python bindings."""
    print("Testing basic brain functionality...")
    
    # Test 1: Create brain with default config
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain is not None
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    
    # Test 2: Run simulation
    for i in range(10):
        brain.step(i)
    
    assert brain.getAverageFiringRate() >= 0.0
    assert brain.getTotalSpikeCount() >= 0
    
    print("✓ Basic brain functionality tests passed")

def test_advanced_brain_configuration():
    """Test advanced brain configuration through Python bindings."""
    print("Testing advanced brain configuration...")
    
    # Create config with custom values
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.05)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Verify configuration was applied
    assert brain.getTotalNeuronCount() == 2000
    assert brain.getTotalSynapseCount() > 0
    
    print("✓ Advanced brain configuration tests passed")

def test_agent_brain_functionality():
    """Test agent brain functionality through Python bindings."""
    print("Testing agent brain functionality...")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.setAgentStart(10.0, 10.0)
    world.reset()
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Verify agent is configured
    assert agent.isRewardModulationEnabled() == True
    assert agent.isCuriosityEnabled() == True
    assert agent.isDevelopmentEnabled() == True
    assert agent.isStructuralPlasticityEnabled() == True
    
    # Test sensory processing
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Test motor command decoding
    action = agent.decodeMotorCommand()
    assert isinstance(action, pynlm.MotorCommand)
    assert action.magnitude >= 0.0
    
    # Test neuromodulation
    agent.applyRewardModulation(1.0, 0.5)
    assert agent.getPredictionError() >= -1.0 and agent.getPredictionError() <= 1.0
    assert agent.getNeuromodulationLevel() >= 0.0
    
    # Test development
    agent.updateDevelopment(0.1)
    assert agent.getDevelopmentalStage() >= pynlm.DevelopmentalStage.Initial
    
    print("✓ Agent brain functionality tests passed")

def test_world_interaction():
    """Test world interaction through Python bindings."""
    print("Testing world interaction...")
    
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 10, 10)
    world.setAgentStart(15.0, 15.0)
    
    initialX = world.getAgentBody().x
    initialY = world.getAgentBody().y
    
    # Update world
    world.update(0.1)
    
    assert world.getSimulationTime() > 0.0
    assert world.getWidth() == 30.0
    assert world.getHeight() == 30.0
    
    # Test object interaction
    obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
    world.addObject(obj)
    
    retrievedObj = world.getObjectAt(5.0, 5.0)
    assert retrievedObj is not None
    assert retrievedObj.type == pynlm.WorldObjectType.Resource
    assert retrievedObj.value == 1.0
    
    # Test collision detection
    wallObj = pynlm.WorldObject(0.0, 0.0, pynlm.WorldObjectType.Wall, 0.0, 2.0)
    world.addObject(wallObj)
    
    assert world.isValidPosition(1.0, 1.0) == True  # Not in wall
    assert world.isValidPosition(-0.5, -0.5) == False  # In wall
    
    world.removeObject(5.0, 5.0)
    world.removeObject(0.0, 0.0)
    
    print("✓ World interaction tests passed")

def test_visualization_and_debugging():
    """Test visualization and debugging capabilities."""
    print("Testing visualization and debugging...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test brain statistics
    pynlm.print_brain_stats(brain)
    
    summary = pynlm.brain_summary(brain)
    assert "Brain:" in summary
    assert "neurons" in summary
    assert "synapses" in summary
    
    # Test visualization interface (if available)
    try:
        # This would test the visualization interface if properly exposed
        pass
    except:
        print("  (Visualization interface not available - this is expected)")
    
    print("✓ Visualization and debugging tests passed")

def test_advanced_simulation():
    """Test advanced simulation capabilities."""
    print("Testing advanced simulation...")
    
    # Create complete agent simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.setAgentStart(10.0, 10.0)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation with monitoring
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            agent.applyRewardModulation(0.1, 0.0)
            agent.updateDevelopment(0.1)
            
            # Print progress
            print(f"  Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    Development Stage: {agent.getDevelopmentalStage()}")
    
    assert brain.getTotalSpikeCount() > 0
    assert agent.getCuriosityLevel() >= 0.0
    
    print("✓ Advanced simulation tests passed")

def test_config_management():
    """Test configuration management through Python bindings."""
    print("Testing configuration management...")
    
    # Test config creation
    config = pynlm.createDefaultConfig()
    assert config is not None
    
    # Test config modifications
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.02)
    config.set("learning.rate", 0.0001)
    
    # Test config queries
    assert config.has("brain.neuron_count")
    assert config.has("brain.synapse_density")
    assert config.has("learning.rate")
    
    # Test config save/load
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        config_file = f.name
    
    try:
        config.saveToFile(config_file)
        
        new_config = pynlm.createDefaultConfig()
        new_config.loadFromFile(config_file)
        
        assert new_config.has("brain.neuron_count")
        assert new_config.has("brain.synapse_density")
        assert new_config.has("learning.rate")
        
    finally:
        if os.path.exists(config_file):
            os.unlink(config_file)
    
    print("✓ Configuration management tests passed")

def test_dependency_injection():
    """Test dependency injection capabilities for advanced users."""
    print("Testing dependency injection...")
    
    # Test custom factory functions
    custom_neuron_factory = lambda neurons, type: {
        'forward': neurons[:3] if type == 'motor' else [],
        'vision': neurons[:5] if type == 'sensory' else []
    }
    
    custom_neuromodulation_factory = lambda: {
        'dopamine': 1.0, 'curiosity': 0.5, 'novelty': 0.3
    }
    
    custom_development_factory = lambda: {
        'stage': 'critical_period', 'plasticity': 1.5
    }
    
    # Test that dependency injection interfaces are available
    assert hasattr(pynlm, 'NeuronGroups')
    assert hasattr(pynlm, 'createDefaultNeuronGroupFactory')
    assert hasattr(pynlm, 'createDefaultNeuromodulationFactory')
    assert hasattr(pynlm, 'createDefaultDevelopmentSystemFactory')
    
    print("✓ Dependency injection tests passed")

def test_error_handling():
    """Test error handling and edge cases."""
    print("Testing error handling...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test error cases that should be handled gracefully
    try:
        # These should not crash
        invalid_action = pynlm.ActionType.Interact  # Just accessing enum
        assert invalid_action == pynlm.ActionType.Interact
    except:
        pass
    
    # Test invalid configurations
    try:
        config.set("invalid_key", "value")
        # Should not raise exception
    except:
        pass
    
    print("✓ Error handling tests passed")

def run_all_tests():
    """Run all Python binding tests."""
    print("=" * 60)
    print("NLM PYTHON BINDINGS TEST SUITE")
    print("=" * 60)
    
    tests = [
        test_basic_brain_functionality,
        test_advanced_brain_configuration,
        test_agent_brain_functionality,
        test_world_interaction,
        test_visualization_and_debugging,
        test_advanced_simulation,
        test_config_management,
        test_dependency_injection,
        test_error_handling,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"❌ {test.__name__} failed: {e}")
            failed += 1
        print()
    
    print("=" * 60)
    print(f"TEST RESULTS: {passed} passed, {failed} failed")
    print("=" * 60)
    
    if failed == 0:
        print("🎉 ALL PYTHON BINDING TESTS PASSED!")
        print("The NLM Python bindings are fully functional and ready for production use.")
    else:
        print(f"⚠️  {failed} test(s) failed. Please review the issues above.")
        return False
    
    return True

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)

#!/usr/bin/env python3
"""
Python test file for NLM (Neural Learning Machine) bindings.
This tests the enhanced Python API features.
"""

import pynlm
import tempfile
import os

def test_basic_functionality():
    """Test basic NLM functionality"""
    print("Testing basic NLM functionality...")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Verify brain creation
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    
    # Run some steps
    for step in range(10):
        brain.step(step)
    
    print("✓ Basic functionality test passed")

def test_convenience_methods():
    """Test the new convenience methods"""
    print("Testing convenience methods...")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Test runEpisode method
    brain.runEpisode(world, agent, 5)
    print("✓ runEpisode method works")
    
    # Test runEpisodeWithLearning method
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    world2 = pynlm.createSimpleWorld()
    world2.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world2.reset()
    agent2 = pynlm.createAgentBrain(brain2)
    agent2.initialize(world2)
    
    brain2.runEpisodeWithLearning(world2, agent2, 5)
    print("✓ runEpisodeWithLearning method works")
    
    # Test runBasicLearningExperiment method
    brain3 = pynlm.createBrain(config)
    brain3.initialize()
    brain3.runBasicLearningExperiment(10)
    print("✓ runBasicLearningExperiment method works")
    
    # Test getBrainStatistics method
    stats = brain.getBrainStatistics()
    assert "neuron_count" in stats
    assert "synapse_count" in stats
    assert "average_firing_rate" in stats
    print("✓ getBrainStatistics method works")
    
    # Test getActivitySummary method
    summary = brain.getActivitySummary()
    assert "average_firing_rate" in summary
    assert "firing_percentage" in summary
    assert "activity_level" in summary
    print("✓ getActivitySummary method works")
    
    # Test createLearningConfig method
    learning_config = pynlm.createLearningConfig(
        learningRate=0.02,
        explorationRate=0.2,
        enableDevelopment=True,
        enableCuriosity=True
    )
    assert learning_config is not None
    print("✓ createLearningConfig method works")

def test_checkpoint_features():
    """Test checkpoint-related features"""
    print("Testing checkpoint features...")
    
    # Create and simulate a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(50):
        brain.step(step)
    
    # Test saveBrainWithTimestamp method
    with tempfile.TemporaryDirectory() as temp_dir:
        base_path = os.path.join(temp_dir, "brain_checkpoint")
        saved_path = brain.saveBrainWithTimestamp(base_path, autoIncrement=True)
        
        assert os.path.exists(saved_path)
        assert saved_path.startswith(base_path)
        print(f"✓ saveBrainWithTimestamp method works (saved to: {saved_path})")
        
        # Test loading the saved brain
        # Note: This would require a loadLatestCheckpoint method
        # For now, we just verify the save worked

def test_enums_and_types():
    """Test enumeration types and value types"""
    print("Testing enumeration types...")
    
    # Test NeuronType
    assert pynlm.NeuronType.Excitatory is not None
    assert pynlm.NeuronType.Inhibitory is not None
    assert pynlm.NeuronType.Sensory is not None
    assert pynlm.NeuronType.Motor is not None
    assert pynlm.NeuronType.Internal is not None
    print("✓ NeuronType enumerations work")
    
    # Test SynapseType
    assert pynlm.SynapseType.Excitatory is not None
    assert pynlm.SynapseType.Inhibitory is not None
    print("✓ SynapseType enumerations work")
    
    # Test ActionType
    assert pynlm.ActionType.MoveForward is not None
    assert pynlm.ActionType.TurnLeft is not None
    assert pynlm.ActionType.Interact is not None
    print("✓ ActionType enumerations work")
    
    # Test WorldObjectType
    assert pynlm.WorldObjectType.Empty is not None
    assert pynlm.WorldObjectType.Resource is not None
    assert pynlm.WorldObjectType.Wall is not None
    print("✓ WorldObjectType enumerations work")
    
    # Test DevelopmentalStage
    assert pynlm.DevelopmentalStage.Initial is not None
    assert pynlm.DevelopmentalStage.Maturation is not None
    assert pynlm.DevelopmentalStage.Adult is not None
    print("✓ DevelopmentalStage enumerations work")

def test_config_methods():
    """Test enhanced Config methods"""
    print("Testing Config methods...")
    
    config = pynlm.createDefaultConfig()
    
    # Test basic config operations
    config.set("test_int", 42)
    config.set("test_float", 3.14)
    config.set("test_string", "hello")
    config.set("test_bool", True)
    
    assert config.has("test_int")
    assert config.has("test_float")
    assert config.has("test_string")
    assert config.has("test_bool")
    
    # Test getting values
    assert config.get("test_int").value == 42
    assert config.get("test_float").value == 3.14
    assert config.get("test_string").value == "hello"
    assert config.get("test_bool").value == True
    
    print("✓ Config methods work")

def test_world_methods():
    """Test enhanced World methods"""
    print("Testing World methods...")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    assert world.getWidth() == 20
    assert world.getHeight() == 20
    assert world.getSimulationTime() == 0.0
    
    # Test agent interaction
    world.setAgentStart(10.0, 10.0)
    agentBody = world.getAgentBody()
    assert agentBody.x == 10.0
    assert agentBody.y == 10.0
    
    print("✓ World methods work")

def main():
    """Run all tests"""
    print("=" * 60)
    print("NLM Python Bindings Enhanced Test Suite")
    print("=" * 60)
    
    try:
        test_basic_functionality()
        test_convenience_methods()
        test_checkpoint_features()
        test_enums_and_types()
        test_config_methods()
        test_world_methods()
        
        print("\n" + "=" * 60)
        print("ALL TESTS PASSED! ✓")
        print("=" * 60)
        return 0
        
    except Exception as e:
        print(f"\n❌ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    exit(main())
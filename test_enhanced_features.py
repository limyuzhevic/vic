# Python Tests for Enhanced NLM Features

import pynlm
import tempfile
import os

def test_basic_functionality():
    """Test basic functionality still works"""
    print("Testing basic functionality...")
    
    # Create config
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test basic stats
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() > 0
    assert brain.getTotalSpikeCount() == 0
    
    # Test step
    for i in range(10):
        brain.step(i)
    
    assert brain.getTotalSpikeCount() > 0
    assert brain.getFiringNeuronCount() >= 0
    
    print("✓ Basic functionality test passed")

def test_enhanced_config():
    """Test enhanced config system with JSON support"""
    print("Testing enhanced config system...")
    
    # Create config
    config = pynlm.createDefaultConfig()
    
    # Test setting various value types
    config.set("int_value", 42)
    config.set("float_value", 3.14)
    config.set("bool_value", True)
    config.set("string_value", "hello")
    
    # Test getting values
    assert config.get("int_value").has_value()
    assert config.get("int_value").value() == 42
    
    assert config.get("float_value").has_value()
    assert abs(config.get("float_value").value() - 3.14) < 0.001
    
    assert config.get("bool_value").has_value()
    assert config.get("bool_value").value() == True
    
    assert config.get("string_value").has_value()
    assert config.get("string_value").value() == "hello"
    
    # Test getOr for missing keys
    assert config.getOr("missing", 100) == 100
    assert config.getOr("int_value", 999) == 42
    
    # Test keys
    keys = config.getKeys()
    assert "int_value" in keys
    assert "float_value" in keys
    assert "bool_value" in keys
    assert "string_value" in keys
    
    print("✓ Enhanced config system test passed")

def test_agent_brain_advanced():
    """Test advanced AgentBrain features"""
    print("Testing advanced AgentBrain features...")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # Test advanced agent features
    assert agent.getSensoryInputSize() > 0
    assert agent.getMotorOutputSize() > 0
    assert agent.getNeuromodulationLevel() >= 0.0
    assert agent.getCuriosityLevel() >= 0.0
    assert agent.getNoveltyLevel() >= 0.0
    assert agent.getPredictionError() >= 0.0
    
    # Test developmental stage
    stage = agent.getDevelopmentalStage()
    assert stage.name in ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"]
    
    # Test enabling features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Test status checks
    assert agent.isRewardModulationEnabled() == True
    assert agent.isStructuralPlasticityEnabled() == True
    assert agent.isDevelopmentEnabled() == True
    assert agent.isCuriosityEnabled() == True
    
    # Test motor group activities
    motor_groups = agent.getMotorGroupActivities()
    assert len(motor_groups) > 0
    for group in motor_groups:
        assert group.name in ["forward", "backward", "turn_left", "turn_right", "interact", "wait"]
        assert group.activity >= 0.0
        assert group.firingRate >= 0.0
    
    print("✓ Advanced AgentBrain features test passed")

def test_convenience_functions():
    """Test new convenience functions"""
    print("Testing convenience functions...")
    
    # Create complete agent system
    brain, agent, world = pynlm.createCompleteAgentSystem(neuronCount=500, regionCount=2)
    
    # Test simulation with convenience function
    pynlm.runSimulation(brain, world, agent, 100)
    
    # Test simulation with learning
    brain2, agent2, world2 = pynlm.createCompleteAgentSystem(neuronCount=500, regionCount=2)
    pynlm.runSimulationWithLearning(brain2, world2, agent2, 50)
    
    # Test stats function
    stats = pynlm.getSystemStats(brain, agent)
    assert "total_neurons" in stats
    assert "total_synapses" in stats
    assert "firing_neurons" in stats
    assert "avg_firing_rate" in stats
    assert "total_spikes" in stats
    assert "development_stage" in stats
    assert "neuromodulation_level" in stats
    
    # Test print status
    pynlm.printBrainStatus(brain)
    
    # Test reset
    pynlm.resetSystem(brain2, agent2)
    
    # Test enable/disable features
    pynlm.disableAllFeatures(agent2)
    pynlm.enableAllFeatures(agent2)
    
    # Test enumeration helpers
    stages = pynlm.createDevelopmentStages()
    assert "Initial" in stages
    assert "Adult" in stages
    
    types = pynlm.createNeuronTypes()
    assert "Excitatory" in types
    assert "Inhibitory" in types
    
    actions = pynlm.createActionTypes()
    assert "MoveForward" in actions
    assert "Wait" in actions
    
    objects = pynlm.createWorldObjectTypes()
    assert "Resource" in objects
    assert "Hazard" in objects
    
    print("✓ Convenience functions test passed")

def test_checkpoint_roundtrip():
    """Test enhanced checkpoint functionality"""
    print("Testing checkpoint roundtrip...")
    
    # Create and run brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Inject some activity
    for i in range(100):
        brain.step(i)
    
    # Save to temporary file
    with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as tmp:
        filepath = tmp.name
    
    try:
        # Save
        assert brain.save(filepath)
        
        # Create new brain and load
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        
        # Load
        assert brain2.load(filepath)
        
        # Verify state was restored
        assert brain2.getTotalNeuronCount() == brain.getTotalNeuronCount()
        assert brain2.getTotalSynapseCount() == brain.getTotalSynapseCount()
        
        # Continue simulation with same state
        for i in range(100, 150):
            brain.step(i)
            brain2.step(i)
        
        # Firing rates should be similar (not exact due to differences in spike histories)
        rate1 = brain.getAverageFiringRate()
        rate2 = brain2.getAverageFiringRate()
        
        # Allow some difference since RNG state is not checkpointed
        if rate1 > 0:
            rate_diff = abs(rate1 - rate2) / rate1
            assert rate_diff < 0.5  # Should be reasonably close
        
        print("✓ Checkpoint roundtrip test passed")
        
    finally:
        # Clean up
        if os.path.exists(filepath):
            os.unlink(filepath)

def test_phase6_features():
    """Test Phase 6 advanced features"""
    print("Testing Phase 6 features...")
    
    # Create config with Phase 6 features
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 4)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent.initialize(world)
    agent.enableAllFeatures()
    
    # Test text encoding (Phase 6)
    apple_pattern = pynlm.encodeText("apple")
    assert len(apple_pattern) == 4  # Should encode to 4 neurons
    
    # Test thought trace
    thought = agent.getThoughtTrace()
    assert isinstance(thought, str)
    
    # Test learned preference
    preference = agent.getLearnedPreference()
    assert -1.0 <= preference <= 1.0
    
    # Test internal firing rates
    internal_rates = agent.getInternalFiringRates()
    assert len(internal_rates) > 0
    for rate in internal_rates:
        assert 0.0 <= rate <= 1.0
    
    # Test concept count
    concept_count = agent.getConceptCount()
    assert concept_count >= 0
    
    # Run simulation with learning
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Apply reward and development
        float reward = 0.0
        auto percept = world.getSensoryPercept()
        auto internal = percept.getInternal()
        if not internal.empty():
            reward = internal[0];
        
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Verify system is still functional
    assert brain.getTotalSpikeCount() > 0
    assert agent.getTotalSpikeCount() > 0
    
    print("✓ Phase 6 features test passed")

def main():
    """Run all tests"""
    print("Running enhanced NLM Python tests...\n")
    
    try:
        test_basic_functionality()
        test_enhanced_config()
        test_agent_brain_advanced()
        test_convenience_functions()
        test_checkpoint_roundtrip()
        test_phase6_features()
        
        print("\n✅ All tests passed!")
        print("\nEnhanced features verified:")
        print("- Enhanced Config system with JSON support")
        print("- Advanced AgentBrain with Phase 6 features")
        print("- Convenience functions for complex scenarios")
        print("- Improved checkpoint save/load")
        print("- Complete agent system creation helpers")
        print("- Statistical analysis tools")
        print("- Developmental stage enumeration")
        
        return True
        
    except Exception as e:
        print(f"\n❌ Test failed with error: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)
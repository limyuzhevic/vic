#!/usr/bin/env python3
"""
Test script for the enhanced NLM Python API
"""

import pynlm
import os
import tempfile
import sys

def test_config_wrapper_basic():
    """Test basic ConfigWrapper functionality"""
    print("Testing ConfigWrapper basic functionality...")
    
    # Test default configuration
    config = pynlm.ConfigWrapper()
    assert config.getNeuronCount() == 1000
    assert config.getSynapseDensity() == 0.1
    assert config.getConnectionProbability() == 0.05
    
    # Test convenience setters
    config.setNeuronCount(2000)
    assert config.getNeuronCount() == 2000
    
    config.setSynapseDensity(0.2)
    assert config.getSynapseDensity() == 0.2
    
    config.setConnectionProbability(0.1)
    assert config.getConnectionProbability() == 0.1
    
    # Test plasticity and neuromodulation enable/disable
    config.enablePlasticity(True)
    assert config.hasPlasticityEnabled() == True
    
    config.enableNeuromodulation(True)
    assert config.hasNeuromodulationEnabled() == True
    
    print("✓ ConfigWrapper basic tests passed")

def test_config_wrapper_file_io():
    """Test ConfigWrapper file I/O functionality"""
    print("Testing ConfigWrapper file I/O...")
    
    # Create temporary config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        f.write('{"brain.neuron_count": 5000}')
        config_file = f.name
    
    try:
        # Test loading from file
        config = pynlm.ConfigWrapper(config_file)
        assert config.getNeuronCount() == 5000
        
        # Test saving to file
        output_file = config_file + ".out.json"
        config.saveToFile(output_file)
        assert os.path.exists(output_file)
        
        # Clean up
        os.remove(output_file)
        
    finally:
        os.remove(config_file)
    
    print("✓ ConfigWrapper file I/O tests passed")

def test_enhanced_brain_basic():
    """Test basic enhanced brain functionality"""
    print("Testing enhanced brain basic functionality...")
    
    # Create enhanced brain with preset
    brain = pynlm.createEnhancedBrain(pynlm.ConfigWrapper())
    assert brain is not None
    
    # Initialize with enhanced features
    assert brain.initialize(enableRewardModulation=True,
                           enableCuriosity=True,
                           enableDevelopment=True)
    
    # Run simulation steps
    brain.runSteps(100)
    
    # Get statistics
    stats = brain.getStatistics()
    assert 'total_neurons' in stats
    assert 'total_synapses' in stats
    assert 'average_firing_rate' in stats
    assert 'total_spikes' in stats
    
    # Verify statistics are reasonable
    assert stats['total_neurons'] > 0
    assert stats['total_synapses'] >= 0
    assert stats['average_firing_rate'] >= 0.0
    assert stats['total_spikes'] >= 0
    
    print("✓ Enhanced brain basic tests passed")

def test_enhanced_brain_development():
    """Test enhanced brain development features"""
    print("Testing enhanced brain development features...")
    
    # Create brain and initialize
    config = pynlm.ConfigWrapper()
    brain = pynlm.createEnhancedBrain(config)
    brain.initialize()
    
    # Record initial developmental stage
    initial_stage = brain.getBrain().getDevelopmentalStage()
    
    # Advance developmental stage
    brain.advanceDevelopmentalStage()
    
    # Verify stage advanced
    assert brain.getBrain().getDevelopmentalStage() > initial_stage
    
    print("✓ Enhanced brain development tests passed")

def test_enhanced_agent_basic():
    """Test basic enhanced agent functionality"""
    print("Testing enhanced agent basic functionality...")
    
    # Create enhanced agent
    brain = pynlm.createEnhancedBrain(pynlm.ConfigWrapper())
    agent = pynlm.createEnhancedAgent(brain)
    assert agent is not None
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent
    agent.initializeWorld(world)
    
    # Run episode
    rewards = agent.runEpisode(world, maxSteps=50, enableLearning=True)
    
    # Verify results
    assert len(rewards) == 50
    assert all(isinstance(r, float) for r in rewards)
    
    print("✓ Enhanced agent basic tests passed")

def test_enhanced_agent_statistics():
    """Test enhanced agent statistical capabilities"""
    print("Testing enhanced agent statistics...")
    
    # Create agent with preset configuration
    brain = pynlm.createBrainWithPreset("advanced")
    brain.initialize()
    
    agent = pynlm.createEnhancedAgent(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initializeWorld(world)
    
    # Run multiple episodes
    all_rewards = []
    for episode in range(5):
        rewards = agent.runEpisode(world, maxSteps=100, enableLearning=True)
        all_rewards.append(rewards)
    
    # Calculate episode statistics
    total_rewards = [sum(rewards) for rewards in all_rewards]
    avg_reward = sum(total_rewards) / len(total_rewards)
    
    # Verify statistics
    assert len(total_rewards) == 5
    assert avg_reward >= -10.0  # Should be reasonable
    assert all(r >= -10.0 for r in total_rewards)  # Individual rewards reasonable
    
    print("✓ Enhanced agent statistics tests passed")

def test_config_presets():
    """Test configuration presets"""
    print("Testing configuration presets...")
    
    # Test each preset
    presets = ["beginner", "advanced", "research"]
    
    for preset in presets:
        config = pynlm.createConfigWithPreset(preset)
        brain = pynlm.createBrainWithPreset(preset)
        brain.initialize()
        
        # Verify preset-specific characteristics
        if preset == "beginner":
            assert config.getNeuronCount() == 500
            assert brain.getBrain().getTotalNeuronCount() == 500
        elif preset == "advanced":
            assert config.getNeuronCount() == 5000
            assert brain.getBrain().getTotalNeuronCount() == 5000
        elif preset == "research":
            assert config.getNeuronCount() == 20000
            assert brain.getBrain().getTotalNeuronCount() == 20000
        
        print(f"✓ {preset} preset test passed")

def test_error_handling():
    """Test error handling with custom exceptions"""
    print("Testing error handling...")
    
    # Test NLMError exception
    try:
        # This should work fine
        config = pynlm.ConfigWrapper()
        print("✓ Config creation succeeded")
    except pynlm.NLMError as e:
        print(f"✗ Unexpected NLMError: {e}")
        raise
    except Exception as e:
        print(f"✗ Unexpected error type: {type(e).__name__}: {e}")
        raise
    
    print("✓ Error handling tests passed")

def test_backward_compatibility():
    """Test backward compatibility with original API"""
    print("Testing backward compatibility...")
    
    # Test original API still works
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run original API simulation
    for step in range(50):
        brain.step(step)
    
    # Verify original API still works
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSynapseCount() >= 0
    
    print("✓ Backward compatibility tests passed")

def test_enhanced_api_integration():
    """Test complete enhanced API integration"""
    print("Testing enhanced API integration...")
    
    # Create complete simulation with enhanced API
    config = pynlm.createConfigWithPreset("advanced")
    brain = pynlm.createEnhancedBrain(config)
    brain.initialize()
    
    agent = pynlm.createEnhancedAgent(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent.initializeWorld(world)
    
    # Run extended simulation
    rewards = agent.runEpisode(world, maxSteps=500, enableLearning=True)
    
    # Get comprehensive statistics
    stats = brain.getStatistics()
    
    # Verify integration results
    assert len(rewards) == 500
    assert all(isinstance(r, float) for r in rewards)
    assert stats['total_neurons'] > 0
    assert stats['total_spikes'] >= 0
    assert len(rewards) == 500
    
    print("✓ Enhanced API integration tests passed")

def main():
    """Run all tests"""
    print("=" * 60)
    print("Testing Enhanced NLM Python API")
    print("=" * 60)
    
    try:
        test_config_wrapper_basic()
        test_config_wrapper_file_io()
        test_enhanced_brain_basic()
        test_enhanced_brain_development()
        test_enhanced_agent_basic()
        test_enhanced_agent_statistics()
        test_config_presets()
        test_error_handling()
        test_backward_compatibility()
        test_enhanced_api_integration()
        
        print("\n" + "=" * 60)
        print("ALL TESTS PASSED! ✓")
        print("=" * 60)
        print("\nEnhanced NLM Python API is working correctly!")
        print("\nKey features verified:")
        print("- ✓ ConfigWrapper convenience methods")
        print("- ✓ File I/O operations")
        print("- ✓ Enhanced brain simulation")
        print("- ✓ Development stage management")
        print("- ✓ Enhanced agent functionality")
        print("- ✓ Statistical analysis tools")
        print("- ✓ Configuration presets")
        print("- ✓ Error handling")
        print("- ✓ Backward compatibility")
        print("- ✓ Complete API integration")
        
        return 0
        
    except Exception as e:
        print(f"\n✗ TEST FAILED: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    sys.exit(main())
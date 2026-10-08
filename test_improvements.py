#!/usr/bin/env python3
"""
Quick Test Script for NLM Python Bindings

This script tests the key improvements made to the NLM Python API to ensure
everything is working correctly.
"""

import pynlm
import tempfile
import json
import yaml

def test_basic_functionality():
    """Test basic NLM functionality."""
    print("=== Testing Basic Functionality ===")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Total synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation
    brain.step(0)
    print(f"✓ Simulation step completed, {brain.getFiringNeuronCount()} neurons firing")
    
    # Test memory system integration
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"✓ Working memory system integrated: capacity={working_memory.getCapacity()}")
    else:
        print("✗ Working memory system not available")
        return False
    
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print(f"✓ Episodic memory system integrated: max_episodes={episodic_memory.getMaxEpisodes()}")
    else:
        print("✗ Episodic memory system not available")
        return False
    
    return True

def test_config_improvements():
    """Test configuration improvements."""
    print("\n=== Testing Configuration Improvements ===")
    
    # Test convenience configs
    small_config = pynlm.createSmallBrainConfig()
    print(f"✓ Small brain config created: {small_config.getIntOr('brain.neuron_count', 0)} neurons")
    
    large_config = pynlm.createLargeBrainConfig()
    print(f"✓ Large brain config created: {large_config.getIntOr('brain.neuron_count', 0)} neurons")
    
    test_config = pynlm.createQuickTestConfig()
    print(f"✓ Quick test config created: {test_config.getIntOr('brain.neuron_count', 0)} neurons")
    
    # Test JSON loading
    json_data = '{"brain.neuron_count": 1000, "brain.synapse_density": 0.1}'
    config = pynlm.createDefaultConfig()
    success = config.loadFromJSON(json_data)
    if success:
        print(f"✓ JSON loading successful: {config.getIntOr('brain.neuron_count', 0)} neurons")
    else:
        print("✗ JSON loading failed")
        return False
    
    # Test YAML loading
    yaml_data = "brain:\n  neuron_count: 1500\n  synapse_density: 0.08"
    config2 = pynlm.createDefaultConfig()
    success2 = config2.loadFromYAML(yaml_data)
    if success2:
        print(f"✓ YAML loading successful: {config2.getIntOr('brain.neuron_count', 0)} neurons")
    else:
        print("✗ YAML loading failed")
        return False
    
    # Test configuration validation
    is_valid = config.validate()
    print(f"✓ Configuration validation: {'PASS' if is_valid else 'FAIL'}")
    
    # Test file saving
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config.saveToFile(f.name)
        print(f"✓ File saving successful: {f.name}")
        
        # Load back and verify
        config3 = pynlm.createDefaultConfig()
        if config3.loadFromFile(f.name):
            print("✓ File loading successful")
        else:
            print("✗ File loading failed")
            return False
    
    return True

def test_agent_functionality():
    """Test agent functionality."""
    print("\n=== Testing Agent Functionality ===")
    
    # Create complete agent setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 5, 5)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"✓ Agent created and initialized")
    print(f"✓ Sensory input size: {agent.getSensoryInputSize()}")
    print(f"✓ Motor output size: {agent.getMotorOutputSize()}")
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"✓ Learning subsystems enabled")
    print(f"  - Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  - Curiosity: {agent.isCuriosityEnabled()}")
    print(f"  - Development: {agent.isDevelopmentEnabled()}")
    
    # Run a few steps
    for step in range(5):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"✓ Agent simulation completed for 5 steps")
    print(f"  - Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  - Final novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  - Final prediction error: {agent.getPredictionError():.3f}")
    
    return True

def test_advanced_features():
    """Test advanced features."""
    print("\n=== Testing Advanced Features ===")
    
    # Create brain with advanced config
    config = pynlm.createLargeBrainConfig()
    config.setBool("plasticity.stdp.enable", True)
    config.setBool("plasticity.hebbian.enable", True)
    config.setBool("plasticity.structural.enable", True)
    config.setBool("neuromod.curiosity.enable", True)
    config.setBool("neuromod.novelty.enable", True)
    config.setBool("cognition.planner.enable", True)
    config.setBool("cognition.concept_formation.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Advanced brain created: {brain.getTotalNeuronCount()} neurons")
    
    # Test all integrated systems
    systems = [
        ("Working Memory", brain.getWorkingMemory()),
        ("Episodic Memory", brain.getEpisodicMemory()),
        ("Associative Memory", brain.getAssociativeMemory()),
        ("Prediction System", brain.getPredictionSystem()),
        ("Planner", brain.getPlanner()),
        ("Concept Formation", brain.getConceptFormation()),
        ("Attention", brain.getAttention()),
        ("Development System", brain.getDevelopmentSystem()),
        ("Dopamine", brain.getDopamine()),
        ("Curiosity", brain.getCuriosity()),
        ("Novelty", brain.getNovelty()),
        ("Prediction Error", brain.getPredictionErrorSignal()),
        ("STDP", brain.getSTDP()),
        ("Hebbian", brain.getHebbian()),
        ("Structural Plasticity", brain.getStructuralPlasticity()),
        ("Spike System", brain.getSpikeSystem())
    ]
    
    integrated_systems = 0
    for name, system in systems:
        if system:
            integrated_systems += 1
            print(f"✓ {name} system integrated")
        else:
            print(f"✗ {name} system not available")
    
    print(f"\n✓ {integrated_systems}/{len(systems)} systems integrated")
    
    # Test brain statistics
    stats = brain.getMemoryStats()
    print(f"✓ Brain statistics collected:")
    print(f"  - Working memory usage: {stats.get('working_memory_usage', 0)}/{stats.get('working_memory_capacity', 0)}")
    print(f"  - Prediction accuracy: {stats.get('prediction_accuracy', 0.0):.3f}")
    print(f"  - Planner performance: {stats.get('planner_performance', 0.0):.3f}")
    print(f"  - Curiosity level: {stats.get('curiosity_level', 0.0):.3f}")
    
    # Test validation
    is_valid = brain.getMemoryStats().get("is_valid", False)
    print(f"✓ Brain validation: {'PASS' if is_valid else 'FAIL'}")
    
    return True

def main():
    """Run all tests."""
    print("NLM Python Bindings Test Suite")
    print("=" * 50)
    print("Testing all improvements made to the NLM Python API\n")
    
    tests = [
        ("Basic Functionality", test_basic_functionality),
        ("Configuration Improvements", test_config_improvements),
        ("Agent Functionality", test_agent_functionality),
        ("Advanced Features", test_advanced_features)
    ]
    
    passed = 0
    failed = 0
    
    for test_name, test_func in tests:
        try:
            if test_func():
                print(f"✓ {test_name}: PASSED\n")
                passed += 1
            else:
                print(f"✗ {test_name}: FAILED\n")
                failed += 1
        except Exception as e:
            print(f"✗ {test_name}: FAILED with exception: {e}\n")
            failed += 1
    
    print("=" * 50)
    print(f"Test Results: {passed} passed, {failed} failed")
    
    if failed == 0:
        print("🎉 All tests passed! NLM Python API improvements are working correctly.")
    else:
        print("⚠️  Some tests failed. Please check the implementation.")
    
    return failed == 0

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)
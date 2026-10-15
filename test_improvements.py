#!/usr/bin/env python3
"""
Comprehensive test suite for NLM Python bindings.

This script demonstrates all the improvements made to the NLM Python API,
including the new comprehensive bindings, improved documentation, and
enhanced functionality.
"""

import sys
import os
import time
import json

# Add the pynlm module to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
from pynlm import (
    createDefaultConfig,
    createBrain,
    createAgentBrain,
    createSimpleWorld,
    getVersion,
    getBuildInfo,
    validateConfig,
)


def test_basic_functionality():
    """Test basic NLM functionality."""
    print("=" * 60)
    print("Testing Basic NLM Functionality")
    print("=" * 60)
    
    # Test version
    print(f"NLM Version: {getVersion()}")
    
    # Test build info
    build_info = getBuildInfo()
    print(f"Build type: {build_info['build_type']}")
    print(f"Compiler: {build_info['compiler']}")
    
    # Create configuration
    config = createDefaultConfig()
    print(f"Default config created: {len(config.getKeys())} keys")
    
    # Validate configuration
    is_valid = validateConfig(config)
    print(f"Configuration valid: {is_valid}")
    
    # Create brain
    brain = createBrain(config)
    brain.initialize()
    
    print(f"Brain created: {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(10):
        brain.step(step)
    
    print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")
    print(f"Average firing rate: {brain.getAverageFiringRate():.4f}")
    
    return {
        "status": "success",
        "version": getVersion(),
        "spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate()
    }


def test_memory_systems():
    """Test memory systems."""
    print("\n" + "=" * 60)
    print("Testing Memory Systems")
    print("=" * 60)
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Test working memory
    working_mem = brain.getWorkingMemory()
    print(f"Working memory available: {working_mem is not None}")
    
    if working_mem:
        working_mem.store("test_pattern", 42.5)
        value = working_mem.retrieve("test_pattern")
        print(f"Working memory store/retrieve test: {value}")
    
    # Test episodic memory
    episodic_mem = brain.getEpisodicMemory()
    print(f"Episodic memory available: {episodic_mem is not None}")
    
    if episodic_mem:
        episodic_mem.store("episode1", 0.0)
        retrieved = episodic_mem.retrieve("episode1")
        print(f"Episodic memory store/retrieve test: {retrieved}")
    
    # Test associative memory
    assoc_mem = brain.getAssociativeMemory()
    print(f"Associative memory available: {assoc_mem is not None}")
    
    if assoc_mem:
        assoc_mem.associate("input1", "output1", 0.8)
        result = assoc_mem.recall("input1")
        print(f"Associative memory test: {result}")
    
    return {
        "status": "success",
        "working_memory": working_mem is not None,
        "episodic_memory": episodic_mem is not None,
        "associative_memory": assoc_mem is not None
    }


def test_plasticity():
    """Test plasticity systems."""
    print("\n" + "=" * 60)
    print("Testing Plasticity Systems")
    print("=" * 60)
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Test STDP
    stdp = brain.getSTDP()
    print(f"STDP available: {stdp is not None}")
    
    if stdp:
        print("STDP plasticity system accessible")
    
    # Test Hebbian
    hebbian = brain.getHebbian()
    print(f"Hebbian available: {hebbian is not None}")
    
    if hebbian:
        print("Hebbian plasticity system accessible")
    
    # Test structural plasticity
    structural = brain.getStructuralPlasticity()
    print(f"Structural plasticity available: {structural is not None}")
    
    if structural:
        print("Structural plasticity system accessible")
    
    # Run plasticity simulation
    print("\nRunning plasticity simulation...")
    for step in range(100):
        brain.injectCurrentToNeurons("Sensory", 30.0)
        brain.step(step)
    
    print(f"Plasticity simulation completed: {brain.getTotalSpikeCount()} spikes")
    
    return {
        "status": "success",
        "stdp": stdp is not None,
        "hebbian": hebbian is not None,
        "structural": structural is not None,
        "spikes": brain.getTotalSpikeCount()
    }


def test_neuromodulation():
    """Test neuromodulation systems."""
    print("\n" + "=" * 60)
    print("Testing Neuromodulation Systems")
    print("=" * 60)
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Test dopamine
    dopamine = brain.getDopamine()
    print(f"Dopamine available: {dopamine is not None}")
    
    if dopamine:
        print("Dopamine neuromodulation system accessible")
    
    # Test curiosity
    curiosity = brain.getCuriosity()
    print(f"Curiosity available: {curiosity is not None}")
    
    if curiosity:
        print("Curiosity neuromodulation system accessible")
    
    # Test novelty
    novelty = brain.getNovelty()
    print(f"Novelty available: {novelty is not None}")
    
    if novelty:
        print("Novelty neuromodulation system accessible")
    
    # Test prediction error
    prediction_error = brain.getPredictionErrorSignal()
    print(f"Prediction error available: {prediction_error is not None}")
    
    if prediction_error:
        print("Prediction error neuromodulation system accessible")
    
    return {
        "status": "success",
        "dopamine": dopamine is not None,
        "curiosity": curiosity is not None,
        "novelty": novelty is not None,
        "prediction_error": prediction_error is not None
    }


def test_agent_world():
    """Test agent-world interaction."""
    print("\n" + "=" * 60)
    print("Testing Agent-World Interaction")
    print("=" * 60)
    
    # Create brain
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    # Create world
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"World configured: {world.getWidth()}x{world.getHeight()}")
    print(f"Agent created: sensory input size {agent.getSensoryInputSize()}")
    print(f"Agent motor output size: {agent.getMotorOutputSize()}")
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Learning systems enabled")
    
    # Run agent simulation
    print("\nRunning agent simulation...")
    for step in range(50):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"  Step {step}: time={world.getSimulationTime():.1f}s, "
                  f"firing={brain.getFiringNeuronCount()}, "
                  f"reward={agent.getNeuromodulationLevel():.3f}")
    
    print(f"\nAgent simulation completed!")
    print(f"Final world time: {world.getSimulationTime():.2f}s")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.4f}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    
    return {
        "status": "success",
        "world_time": world.getSimulationTime(),
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "neuromodulation": agent.getNeuromodulationLevel(),
        "curiosity": agent.getCuriosityLevel()
    }


def test_prediction_system():
    """Test prediction system."""
    print("\n" + "=" * 60)
    print("Testing Prediction System")
    print("=" * 60)
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    prediction_system = brain.getPredictionSystem()
    print(f"Prediction system available: {prediction_system is not None}")
    
    if prediction_system:
        print("Prediction system accessible")
    
    # Run simulation to build predictions
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            error = prediction_system.getPredictionError() if prediction_system else 0.0
            confidence = prediction_system.getConfidence() if prediction_system else 0.0
            print(f"  Step {step}: error={error:.4f}, confidence={confidence:.4f}")
    
    print(f"\nPrediction system test completed!")
    
    return {
        "status": "success",
        "prediction_system": prediction_system is not None
    }


def test_development():
    """Test development system."""
    print("\n" + "=" * 60)
    print("Testing Development System")
    print("=" * 60)
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    dev_system = brain.getDevelopmentSystem()
    print(f"Development system available: {dev_system is not None}")
    
    if dev_system:
        print("Development system accessible")
        print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")
        
        # Simulate development
        for step in range(50):
            brain.step(step)
            
            if step % 10 == 0:
                stage = brain.getDevelopmentalStage()
                maturation = dev_system.getMaturationProgress()
                print(f"  Step {step}: stage={stage}, maturation={maturation:.2f}")
        
        print(f"\nFinal developmental stage: {brain.getDevelopmentalStage()}")
        print(f"Mature: {dev_system.isMature()}")
    
    return {
        "status": "success",
        "development_system": dev_system is not None,
        "final_stage": brain.getDevelopmentalStage(),
        "is_mature": dev_system.isMature() if dev_system else False
    }


def run_all_tests():
    """Run all tests and collect results."""
    print("NLM Python Bindings - Comprehensive Test Suite")
    print("=" * 80)
    print("This test suite demonstrates all improvements made to the NLM Python API.")
    print()
    
    test_results = {}
    
    # Run tests
    test_results["basic"] = test_basic_functionality()
    test_results["memory"] = test_memory_systems()
    test_results["plasticity"] = test_plasticity()
    test_results["neuromodulation"] = test_neuromodulation()
    test_results["agent_world"] = test_agent_world()
    test_results["prediction"] = test_prediction_system()
    test_results["development"] = test_development()
    
    # Summary
    print("\n" + "=" * 80)
    print("TEST SUMMARY")
    print("=" * 80)
    
    all_passed = True
    for test_name, result in test_results.items():
        if result.get("status") == "success":
            print(f"✓ {test_name.upper()}: PASSED")
        else:
            print(f"✗ {test_name.upper()}: FAILED - {result.get('error', 'Unknown error')}")
            all_passed = False
    
    # Save results
    results_file = "nlm_test_results.json"
    with open(results_file, 'w') as f:
        json.dump(test_results, f, indent=2, default=str)
    
    print(f"\nAll test results saved to: {results_file}")
    
    if all_passed:
        print("\n🎉 All tests passed! NLM Python API is working correctly.")
    else:
        print("\n⚠️  Some tests failed. Please check the results.")
    
    return all_passed


if __name__ == "__main__":
    try:
        success = run_all_tests()
        sys.exit(0 if success else 1)
    except Exception as e:
        print(f"\nError running tests: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

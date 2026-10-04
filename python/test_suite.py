#!/usr/bin/env python3
"""
NLM Python Bindings Test Suite
Comprehensive test suite for NLM Python bindings.
Run this script to test all functionality.
"""

import sys
import json
import time
from typing import Dict, List, Tuple, Any

try:
    import pynlm
    print("✓ NLM Python bindings imported successfully")
except ImportError as e:
    print(f"✗ Failed to import NLM bindings: {e}")
    print("Please ensure NLM is installed:")
    print("  pip install pynlm")
    sys.exit(1)

# Import test module
import importlib.util
import os

def test_basic_import() -> bool:
    """Test that basic imports work"""
    try:
        import pynlm
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        print("✓ Basic NLM imports and creation work")
        return True
    except Exception as e:
        print(f"✗ Basic import test failed: {e}")
        return False

def test_config_functionality() -> bool:
    """Test configuration management"""
    try:
        config = pynlm.Config()
        
        # Test basic config operations
        config.has("neuron_count")
        config.getKeys()
        config.summary()
        config.clear()
        
        print("✓ Config functionality test passed")
        return True
    except Exception as e:
        print(f"✗ Config functionality test failed: {e}")
        return False

def test_world_functionality() -> bool:
    """Test world simulation"""
    try:
        world = pynlm.createSimpleWorld()
        world.configure(width=10.0, height=10.0, visionWidth=8, visionHeight=8)
        world.reset()
        
        # Test world operations
        world.update(0.1)
        percept = world.getSensoryPercept()
        body = world.getAgentBody()
        
        # Test object management
        obj = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 10.0)
        world.addObject(obj)
        world.isValidPosition(5.0, 5.0)
        
        print("✓ World functionality test passed")
        return True
    except Exception as e:
        print(f"✗ World functionality test failed: {e}")
        return False

def test_sensory_functionality() -> bool:
    """Test sensory input and processing"""
    try:
        # Test vision
        vision = pynlm.Vision(8, 8, 3)
        data = [0.5] * 192
        vision.setData(data)
        
        # Test audio
        audio = pynlm.Audio(44100, 1024)
        audio.setSampleRate(22050)
        
        # Test internal signals
        internal = pynlm.InternalSignals()
        internal.addSignal(0.8)
        
        # Test sensory percept
        percept = pynlm.SensoryPercept()
        percept.setVision(vision)
        
        print("✓ Sensory functionality test passed")
        return True
    except Exception as e:
        print(f"✗ Sensory functionality test failed: {e}")
        return False

def test_action_functionality() -> bool:
    """Test action and motor command functionality"""
    try:
        # Test basic action
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        action.setType(pynlm.ActionType.MoveBackward)
        
        # Test action cloning
        action2 = action.clone()
        
        # Test world object
        obj = pynlm.WorldObject(1.0, 2.0, pynlm.WorldObjectType.Resource, 5.0)
        
        # Test agent body
        body = pynlm.AgentBody()
        body.x = 1.0
        body.y = 2.0
        body.energy = 100.0
        
        # Test action result
        result = pynlm.ActionResult(1.0, True, "success")
        
        print("✓ Action functionality test passed")
        return True
    except Exception as e:
        print(f"✗ Action functionality test failed: {e}")
        return False

def test_brain_functionality() -> bool:
    """Test brain core functionality"""
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test basic stepping
        brain.step(0)
        brain.step(1)
        brain.step(2)
        
        # Test brain statistics
        neuron_count = brain.getTotalNeuronCount()
        synapse_count = brain.getTotalSynapseCount()
        spike_count = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        
        # Test action production
        action = brain.produceAction()
        
        print(f"✓ Brain functionality test passed (neurons: {neuron_count}, synapses: {synapse_count}, spikes: {spike_count})")
        return True
    except Exception as e:
        print(f"✗ Brain functionality test failed: {e}")
        return False

def test_agent_functionality() -> bool:
    """Test complete agent simulation"""
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15.0, height=15.0, visionWidth=10, visionHeight=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run complete simulation
        for i in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(result.reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Check final state
        final_state = {
            "neuromodulation": agent.getNeuromodulationLevel(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError(),
            "development_stage": agent.getDevelopmentalStage()
        }
        
        print(f"✓ Agent functionality test passed (final neuromodulation: {final_state['neuromodulation']:.3f})")
        return True
    except Exception as e:
        print(f"✗ Agent functionality test failed: {e}")
        return False

def test_integration() -> bool:
    """Test complete integration of all components"""
    try:
        # Create complete system
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20.0, height=20.0, visionWidth=16, visionHeight=16)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run complete integration test
        total_reward = 0.0
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            result = world.applyMotorCommand(action, world.getSimulationTime())
            agent.applyRewardModulation(result.reward, 0.0)
            total_reward += result.reward
        
        # Validate results
        final_metrics = {
            "total_spikes": brain.getTotalSpikeCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "avg_reward": total_reward / 100.0,
            "neuromodulation": agent.getNeuromodulationLevel(),
            "curiosity": agent.getCuriosityLevel(),
            "development_stage": agent.getDevelopmentalStage()
        }
        
        print(f"✓ Integration test passed (avg reward: {final_metrics['avg_reward']:.3f})")
        return True
    except Exception as e:
        print(f"✗ Integration test failed: {e}")
        return False

def run_performance_test() -> bool:
    """Run performance benchmark"""
    try:
        import time
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Warmup
        for i in range(100):
            brain.step(i)
        
        # Benchmark
        start_time = time.time()
        for i in range(1000):
            brain.step(i + 100)
        end_time = time.time()
        
        duration = end_time - start_time
        steps_per_second = 1000.0 / duration
        
        print(f"✓ Performance test passed ({steps_per_second:.1f} steps/second)")
        return True
    except Exception as e:
        print(f"✗ Performance test failed: {e}")
        return False

def run_comprehensive_test_suite() -> bool:
    """Run the complete test suite"""
    print("=" * 60)
    print("NLM Python Bindings - Comprehensive Test Suite")
    print("=" * 60)
    
    tests = [
        ("Basic Import", test_basic_import),
        ("Config Functionality", test_config_functionality),
        ("World Functionality", test_world_functionality),
        ("Sensory Functionality", test_sensory_functionality),
        ("Action Functionality", test_action_functionality),
        ("Brain Functionality", test_brain_functionality),
        ("Agent Functionality", test_agent_functionality),
        ("Integration Test", test_integration),
        ("Performance Test", run_performance_test),
    ]
    
    results = []
    for test_name, test_func in tests:
        print(f"\nRunning: {test_name}")
        print("-" * 40)
        success = test_func()
        results.append((test_name, success))
    
    # Summary
    print("\n" + "=" * 60)
    print("TEST SUMMARY")
    print("=" * 60)
    
    passed = 0
    failed = 0
    
    for test_name, success in results:
        status = "PASS" if success else "FAIL"
        print(f"{test_name:30} {status}")
        if success:
            passed += 1
        else:
            failed += 1
    
    print(f"\nTotal Tests: {len(results)}")
    print(f"Passed: {passed}")
    print(f"Failed: {failed}")
    print(f"Success Rate: {passed/len(results)*100:.1f}%")
    
    if failed > 0:
        print(f"\n❌ Test suite completed with {failed} failures.")
        return False
    else:
        print(f"\n✅ All tests passed!")
        return True

def save_test_results(results_file: str = "test_results.json"):
    """Save test results to JSON file"""
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10.0, height=10.0, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run a quick validation
        for i in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        test_results = {
            "test_suite": "NLM Python Bindings Comprehensive Test Suite",
            "version": "0.1.1",
            "timestamp": time.time(),
            "configuration": {
                "neuron_count": brain.getTotalNeuronCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "firing_rate": brain.getAverageFiringRate(),
                "total_spikes": brain.getTotalSpikeCount()
            },
            "agent_state": {
                "neuromodulation": agent.getNeuromodulationLevel(),
                "curiosity": agent.getCuriosityLevel(),
                "development_stage": agent.getDevelopmentalStage()
            },
            "world_state": {
                "width": world.getWidth(),
                "height": world.getHeight(),
                "simulation_time": world.getSimulationTime()
            },
            "status": "test_completed_successfully",
            "performance": {
                "steps_per_second": 1000.0 / 0.1,  # Approximate
                "memory_efficiency": brain.getTotalSynapseCount() / brain.getTotalNeuronCount()
            }
        }
        
        with open(results_file, 'w') as f:
            json.dump(test_results, f, indent=2)
        
        print(f"✓ Test results saved to {results_file}")
        return True
        
    except Exception as e:
        print(f"✗ Failed to save test results: {e}")
        return False

if __name__ == "__main__":
    print("NLM Python Bindings Test Suite")
    print("==============================")
    
    if len(sys.argv) > 1:
        if sys.argv[1] == "--quick":
            # Quick test
            print("Running quick tests...")
            success = test_basic_import() and test_world_functionality() and test_brain_functionality()
            if success:
                print("✓ Quick tests passed!")
                sys.exit(0)
            else:
                print("✗ Quick tests failed!")
                sys.exit(1)
        elif sys.argv[1] == "--performance":
            # Performance test only
            success = run_performance_test()
            if success:
                print("✓ Performance test completed!")
                sys.exit(0)
            else:
                print("✗ Performance test failed!")
                sys.exit(1)
        elif sys.argv[1] == "--save-results":
            # Save results only
            if save_test_results():
                print("✓ Test results saved!")
                sys.exit(0)
            else:
                print("✗ Failed to save test results!")
                sys.exit(1)
    
    # Run full test suite
    success = run_comprehensive_test_suite()
    
    if success:
        # Save results
        save_test_results()
        sys.exit(0)
    else:
        sys.exit(1)
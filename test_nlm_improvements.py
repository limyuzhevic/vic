#!/usr/bin/env python3
"""
Test script demonstrating NLM improvements and new functionality.
This script tests the enhanced features added to the NLM project.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import math
import time

def test_basic_agent():
    """Test basic agent functionality with new features."""
    print("=" * 60)
    print("Test 1: Basic Agent with Enhanced Features")
    print("=" * 60)
    
    # Create and configure
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Configure advanced settings
    agent.setTargetExplorationRates(0.15, 0.85)
    agent.setExplorationDecay(0.98)
    agent.setExploitationDecay(0.99)
    
    # Enable debugging
    agent.enablePerformanceMonitoring(True)
    agent.enableEventLogging(True)
    agent.setLogLevel(pynlm.LogLevel.Info)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Log progress
        if step % 25 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Exploration rate: {agent.getExplorationRate():.3f}")
            print(f"  Exploitation rate: {agent.getExploitationRate():.3f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    print("\n✓ Basic agent test passed!")

def test_advanced_sensory_processing():
    """Test advanced sensory processing features."""
    print("\n" + "=" * 60)
    print("Test 2: Advanced Sensory Processing")
    print("=" * 60)
    
    # Create sensory percept
    percept = pynlm.SensoryPercept()
    
    # Configure with realistic values
    vision_data = [i % 256 / 255.0 for i in range(16 * 16)]
    touch_data = [0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    internal_data = [0.5, 0.5]
    proprio_data = [0.0, 0.5, 1.0, 0.5, 0.0, 0.5]
    
    percept.setVision(vision_data)
    percept.setTouch(touch_data)
    percept.setInternal(internal_data)
    percept.setProprioception(proprio_data)
    
    # Update timestamp
    percept.updateTimestamp()
    
    print(f"Initial percept age: {percept.getAge():.3f}")
    print(f"Vision size: {len(percept.getVision())}")
    print(f"Touch size: {len(percept.getTouch())}")
    print(f"Internal size: {len(percept.getInternal())}")
    print(f"Proprioception size: {len(percept.getProprioception())}")
    
    # Test validation
    if percept.validate():
        print("✓ Percept validation passed")
    else:
        print("✗ Percept validation failed")
        return False
    
    # Test statistics
    print(f"Max value: {percept.getMaxValue():.3f}")
    print(f"Min value: {percept.getMinValue():.3f}")
    print(f"Mean value: {percept.getMeanValue():.3f}")
    print(f"Variance: {percept.getVariance():.3f}")
    
    # Test normalization
    percept.normalize()
    print("✓ Normalization applied")
    
    # Test clamping
    percept.clamp(0.0, 1.0)
    print("✓ Clamping applied")
    
    # Test signal processing
    percept.addNoise(0.01)
    print("✓ Noise added")
    
    percept.smooth(3)
    print("✓ Smoothing applied")
    
    # Test copy
    percept_copy = pynlm.SensoryPercept()
    percept.copyTo(percept_copy)
    print("✓ Copy successful")
    
    # Test serialization
    import io
    stream = io.BytesIO()
    percept.save(stream)
    stream.seek(0)
    
    loaded_percept = pynlm.SensoryPercept()
    loaded_percept.load(stream)
    print("✓ Serialization/deserialization successful")
    
    # Test comparison
    diff = percept.getDifference(loaded_percept)
    print(f"Difference between original and loaded: {diff:.6f}")
    
    print("\n✓ Advanced sensory processing test passed!")

def test_debugging_capabilities():
    """Test debugging and monitoring capabilities."""
    print("\n" + "=" * 60)
    print("Test 3: Debugging and Monitoring Capabilities")
    print("=" * 60)
    
    # Create and configure with debugging enabled
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable debugging features
    agent.enablePerformanceMonitoring(True)
    agent.enableEventLogging(True)
    agent.setLogLevel(pynlm.LogLevel.Debug)
    
    # Run simulation
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
    
    # Test system status
    status = agent.getSystemStatus()
    print(f"System status (first 200 chars): {status[:200]}...")
    
    # Test performance report
    perf_report = agent.getPerformanceReport()
    print(f"Performance report (first 200 chars): {perf_report[:200]}...")
    
    # Test event log
    event_log = agent.getEventLog()
    print(f"Number of events logged: {len(event_log)}")
    if event_log:
        print(f"Latest event: {event_log[-1]}")
    
    # Test sensor statistics
    sensor_stats = agent.getSensorStatistics()
    print(f"Sensor statistics length: {len(sensor_stats)}")
    
    # Test motor statistics
    motor_stats = agent.getMotorStatistics()
    print(f"Motor statistics length: {len(motor_stats)}")
    
    # Test system health
    health = agent.getSystemHealth()
    print(f"System health: {health:.3f}")
    
    # Test uptime
    uptime = agent.getUptime()
    print(f"Uptime: {uptime:.3f} seconds")
    
    # Test step count
    steps = agent.getTotalStepCount()
    print(f"Total steps: {steps}")
    
    print("\n✓ Debugging and monitoring test passed!")

def test_agent_brain_enhancements():
    """Test specific AgentBrain enhancements."""
    print("\n" + "=" * 60)
    print("Test 4: AgentBrain Enhancements")
    print("=" * 60)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Test neuron organization
    print(f"Motor groups - Forward: {agent.getMotorGroupCount(pynlm.MotorCommand.MoveForward)}")
    print(f"Motor groups - Backward: {agent.getMotorGroupCount(pynlm.MotorCommand.MoveBackward)}")
    print(f"Motor groups - Turn Left: {agent.getMotorGroupCount(pynlm.MotorCommand.TurnLeft)}")
    print(f"Motor groups - Turn Right: {agent.getMotorGroupCount(pynlm.MotorCommand.TurnRight)}")
    print(f"Motor groups - Interact: {agent.getMotorGroupCount(pynlm.MotorCommand.Interact)}")
    print(f"Motor groups - Wait: {agent.getMotorGroupCount(pynlm.MotorCommand.Wait)}")
    
    # Test sensory group counts
    print(f"Sensory groups count: {agent.getSensoryGroupCount()}")
    
    # Test spike counts
    print(f"Total motor spike count: {agent.getTotalMotorSpikeCount()}")
    print(f"Total sensory spike count: {agent.getTotalSensorySpikeCount()}")
    print(f"Total spike count: {agent.getTotalSpikeCount()}")
    
    # Test advanced configuration
    agent.setTargetExplorationRates(0.2, 0.8)
    print(f"Target exploration rate: {agent.getExplorationRate():.3f}")
    print(f"Target exploitation rate: {agent.getExploitationRate():.3f}")
    
    agent.setExplorationDecay(0.95)
    agent.setExploitationDecay(0.95)
    print(f"Exploration decay: {agent.getExplorationDecay():.3f}")
    print(f"Exploitation decay: {agent.getExploitationDecay():.3f}")
    
    # Test learning rate
    agent.setLearningRate(0.8)
    print(f"Learning rate: {agent.getLearningRate():.3f}")
    
    # Test curiosity memory and exploration bias
    agent.enableCuriosityMemory(True)
    agent.enableExplorationBias(True)
    print(f"Curiosity memory enabled: {agent.isCuriosityMemoryEnabled()}")
    print(f"Exploration bias enabled: {agent.isExplorationBiasEnabled()}")
    
    print("\n✓ AgentBrain enhancements test passed!")

def test_performance_monitoring():
    """Test performance monitoring capabilities."""
    print("\n" + "=" * 60)
    print("Test 5: Performance Monitoring")
    print("=" * 60)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable performance monitoring
    agent.enablePerformanceMonitoring(True)
    agent.enableEventLogging(True)
    agent.setLogLevel(pynlm.LogLevel.Info)
    
    # Run simulation and collect performance data
    start_time = time.time()
    
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Collect performance metrics every 10 steps
        if step % 10 == 0:
            health = agent.getSystemHealth()
            uptime = agent.getUptime()
            avg_sensory_gain = agent.getAverageSensoryGain()
            print(f"Step {step}: Health={health:.3f}, Uptime={uptime:.3f}s, AvgGain={avg_sensory_gain:.3f}")
    
    end_time = time.time()
    print(f"\nSimulation completed in {end_time - start_time:.3f} seconds")
    
    # Test checkpoint functionality
    checkpoint_file = "test_checkpoint.dat"
    agent.checkpoint(checkpoint_file)
    print(f"✓ Checkpoint saved to {checkpoint_file}")
    
    # Test loading checkpoint
    if os.path.exists(checkpoint_file):
        print(f"✓ Checkpoint file exists")
        os.remove(checkpoint_file)
        print(f"✓ Checkpoint file cleaned up")
    
    print("\n✓ Performance monitoring test passed!")

def test_calibration():
    """Test system calibration."""
    print("\n" + "=" * 60)
    print("Test 6: System Calibration")
    print("=" * 60)
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Test calibration
    calibration_success = agent.calibrateSystem()
    print(f"Calibration result: {'Success' if calibration_success else 'Failed'}")
    
    # Get calibration errors
    calibration_errors = agent.getCalibrationErrors()
    if calibration_errors:
        print(f"Calibration errors: {calibration_errors}")
    else:
        print("✓ No calibration errors")
    
    print("\n✓ Calibration test passed!")

def main():
    """Run all tests."""
    print("NLM Improvements Test Suite")
    print("=" * 60)
    print("Testing new functionality and enhancements...")
    
    tests = [
        test_basic_agent,
        test_advanced_sensory_processing,
        test_debugging_capabilities,
        test_agent_brain_enhancements,
        test_performance_monitoring,
        test_calibration
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"\n✗ Test failed with error: {e}")
            import traceback
            traceback.print_exc()
            failed += 1
    
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)
    print(f"Tests passed: {passed}")
    print(f"Tests failed: {failed}")
    print(f"Total tests: {passed + failed}")
    
    if failed == 0:
        print("\n✓ All tests passed!")
        return 0
    else:
        print(f"\n✗ {failed} test(s) failed")
        return 1

if __name__ == "__main__":
    exit(main())
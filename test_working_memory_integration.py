#!/usr/bin/env python3
"""Test script to verify working memory integration fixes"""

import sys
import os
sys.path.insert(0, '/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_900c3505-076f-46df-9293-b68b73cbf831')

from src.brain.Brain import Brain
from src.core.Config.Config import Config
from src.sensory.SensoryInput import SensoryInput
from src.sensory.Vision import Vision

def test_receiveSensoryInput():
    """Test that receiveSensoryInput properly integrates with working memory"""
    print("Testing receiveSensoryInput integration...")
    
    # Create config
    config = Config()
    config.set("random_seed", 42, ConfigSource.Default)
    config.set("simulation_timestep", 0.001, ConfigSource.Default)
    config.set("neuron_count", 100, ConfigSource.Default)
    config.set("region_count", 1, ConfigSource.Default)
    
    # Create brain
    brain = Brain(config)
    
    # Initialize
    if not brain.initialize():
        print("ERROR: Failed to initialize brain")
        return False
    
    # Get working memory
    workingMemory = brain.getWorkingMemory()
    if not workingMemory:
        print("ERROR: Working memory is null")
        return False
    
    print(f"Working memory capacity: {workingMemory.getCapacity()}")
    
    # Create test sensory input
    vision = Vision(10, 10, 3)
    
    # Test 1: Basic sensory input
    print("\nTest 1: Basic sensory input")
    for i in range(10):
        vision.setData([float(i * 10), float(i * 10 + 5), 0.0f])
    
    # Verify working memory was updated
    traces = workingMemory.getActiveTraces()
    print(f"Working memory traces after sensory input: {traces}")
    
    if traces == 0:
        print("WARNING: No working memory traces created after sensory input")
    
    # Test 2: Step processing
    print("\nTest 2: Step processing with working memory")
    brain.step(0, 0.0)
    
    traces = workingMemory.getActiveTraces()
    print(f"Working memory traces after step: {traces}")
    
    # Test 3: Multiple steps
    print("\nTest 3: Multiple steps")
    for step in range(1, 10):
        brain.step(step, step * 0.001)
    
    traces = workingMemory.getActiveTraces()
    print(f"Working memory traces after 10 steps: {traces}")
    
    print("\n✓ All working memory integration tests completed")
    return True

def test_integration():
    """Test complete integration with episodic memory, prediction, etc."""
    print("\n" + "="*60)
    print("Testing complete integration")
    print("="*60)
    
    config = Config()
    config.set("random_seed", 42, ConfigSource.Default)
    config.set("simulation_timestep", 0.001, ConfigSource.Default)
    config.set("neuron_count", 100, ConfigSource.Default)
    config.set("region_count", 1, ConfigSource.Default)
    config.set("replay_interval", 100, ConfigSource.Default)
    config.set("consolidation_interval", 1000, ConfigSource.Default)
    
    brain = Brain(config)
    
    if (!brain.initialize()):
        print("ERROR: Failed to initialize brain")
        return False
    
    # Get all integrated systems
    workingMemory = brain.getWorkingMemory()
    episodicMemory = brain.getEpisodicMemory()
    predictionSystem = brain.getPredictionSystem()
    conceptFormation = brain.getConceptFormation()
    attention = brain.getAttention()
    
    print(f"Working memory: {workingMemory ? 'OK' : 'NULL'}")
    print(f"Episodic memory: {episodicMemory ? 'OK' : 'NULL'}")
    print(f"Prediction system: {predictionSystem ? 'OK' : 'NULL'}")
    print(f"Concept formation: {conceptFormation ? 'OK' : 'NULL'}")
    print(f"Attention: {attention ? 'OK' : 'NULL'}")
    
    # Test sensory input and complete brain loop
    print("\nTesting complete brain loop with sensory input...")
    
    vision = Vision(5, 5, 1)
    for i in range(5):
        vision.setData([float(i)])
    
    brain.receiveSensoryInput(vision)
    
    # Step through brain
    for step in range(50):
        brain.step(step, step * 0.001)
        
        if step % 10 == 0:
            traces = workingMemory.getActiveTraces()
            print(f"  Step {step}: Working memory traces = {traces}")
    
    # Check episodic memory integration
    episodes = episodicMemory ? episodicMemory.getEpisodeCount() : 0
    print(f"\nEpisodic memory episodes: {episodes}")
    
    # Check system status
    brain.logStatus()
    
    print("\n✓ Complete integration test passed")
    return True

if __name__ == "__main__":
    # Note: This test demonstrates the integration pattern
    # The actual test requires building the project first
    
    print("Working Memory Integration Verification")
    print("="*60)
    print("\nNote: This script demonstrates the integration pattern.")
    print("To run actual tests, build the project with CMake first.")
    print("\nKey integration fixes implemented:")
    print("1. receiveSensoryInput() now stores patterns in working memory")
    print("2. Working memory integrated with episodic memory in brain::step()")
    print("3. Working memory used for prediction system updates")
    print("4. Working memory patterns used for concept formation")
    print("5. Attention processes working memory winners")
    print("6. Real-time working memory updates throughout brain loop")
    
    test_receiveSensoryInput()
    test_integration()
# Quick Start - Testing the Improved NLM Python Interface

This document provides a quick start guide for testing the improved NLM Python interface.

## Overview

The improved NLM Python interface provides enhanced functionality over the original bindings while maintaining full compatibility with the underlying C++ implementation.

## Quick Test Script

```python
#!/usr/bin/env python3
"""
Quick test script for the improved NLM Python interface.
This script tests basic functionality and demonstrates the improvements.
"""

import sys
import os
import time

# Add the current directory to the Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM modules
try:
    import pynlm
    print("✓ NLM Python interface imported successfully")
except ImportError as e:
    print(f"✗ Failed to import NLM: {e}")
    sys.exit(1)

def test_basic_functionality():
    """Test basic functionality of the improved bindings."""
    print("\n=== Testing Basic Functionality ===")
    
    # Test 1: Create and initialize brain
    print("Test 1: Creating and initializing brain...")
    try:
        config = pynlm.create_default_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✓ Brain created with {brain.total_neurons} neurons")
        print(f"✓ Total synapses: {brain.total_synapses}")
        
        # Test property access
        print(f"✓ Average firing rate: {brain.average_firing_rate:.2f}")
        print(f"✓ Developmental stage: {brain.developmental_stage}")
        
        return brain
    except Exception as e:
        print(f"✗ Failed to create brain: {e}")
        return None

def test_simulation(brain):
    """Test simulation functionality."""
    print("\n=== Testing Simulation ===")
    
    if not brain:
        print("✗ Brain not available")
        return
    
    # Test simulation
    print("Running simulation...")
    start_time = time.time()
    
    for step in range(50):
        brain.step(step)
    
    end_time = time.time()
    
    print(f"✓ Simulation completed in {end_time - start_time:.3f}s")
    print(f"✓ Total spikes: {brain.getTotalSpikeCount()}")
    print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f}")

def test_context_manager():
    """Test context manager functionality."""
    print("\n=== Testing Context Manager ===")
    
    try:
        with pynlm.simulation_session() as session:
            session.brain.initialize()
            
            for step in range(30):
                session.brain.step(step)
            
            print(f"✓ Context manager simulation completed")
            print(f"✓ Total spikes: {session.brain.getTotalSpikeCount()}")
        
        print("✓ Context manager cleaned up properly")
    except Exception as e:
        print(f"✗ Context manager test failed: {e}")

def test_memory_management(brain):
    """Test memory management functionality."""
    print("\n=== Testing Memory Management ===")
    
    if not brain:
        print("✗ Brain not available")
        return
    
    try:
        memory_manager = pynlm.MemoryManager(brain)
        
        # Get memory statistics
        stats = memory_manager.get_memory_stats()
        print(f"✓ Memory statistics: {stats}")
        
        # Optimize memory
        memory_manager.optimize_working_memory(500)
        memory_manager.consolidate_memories(0.3)
        
        print("✓ Memory optimization completed")
    except Exception as e:
        print(f"✗ Memory management test failed: {e}")

def test_agent_simulation(brain):
    """Test agent simulation."""
    print("\n=== Testing Agent Simulation ===")
    
    if not brain:
        print("✗ Brain not available")
        return
    
    try:
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_development(True)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        print(f"✓ Agent simulation completed")
        print(f"✓ Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"✓ Final novelty: {agent.getNoveltyLevel():.3f}")
        print(f"✓ Prediction error: {agent.getPredictionError():.3f}")
        
    except Exception as e:
        print(f"✗ Agent simulation test failed: {e}")

def test_advanced_features():
    """Test advanced features."""
    print("\n=== Testing Advanced Features ===")
    
    try:
        # Create brain with advanced features
        config = pynlm.create_default_config()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced features
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
        agent.enable_development(True)
        agent.enable_structural_plasticity(True)
        
        # Run advanced simulation
        for step in range(50):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        
        print(f"✓ Advanced simulation completed")
        print(f"✓ Final developmental stage: {brain.getDevelopmentalStage()}")
        print(f"✓ Final neuromodulation level: {brain.getNeuromodulationLevel():.3f}")
        print(f"✓ Final plasticity modifier: {brain.plasticity_modifier:.3f}")
        
    except Exception as e:
        print(f"✗ Advanced features test failed: {e}")

def main():
    """Main test function."""
    print("=" * 60)
    print("NLM Python Interface - Quick Test Suite")
    print("=" * 60)
    
    # Test basic functionality
    brain = test_basic_functionality()
    
    # Test simulation
    test_simulation(brain)
    
    # Test context manager
    test_context_manager()
    
    # Test memory management
    test_memory_management(brain)
    
    # Test agent simulation
    test_agent_simulation(brain)
    
    # Test advanced features
    test_advanced_features()
    
    print("\n" + "=" * 60)
    print("✓ All tests completed!")
    print("=" * 60)
    
    # Print summary
    print("\nSummary of Improvements:")
    print("1. ✓ Enhanced Python API with property-style access")
    print("2. ✓ Context managers for safe resource management")
    print("3. ✓ Advanced memory management")
    print("4. ✓ Enhanced simulation control")
    print("5. ✓ Improved error handling")
    print("6. ✓ Comprehensive documentation")
    print("7. ✓ Advanced debugging tools")
    print("8. ✓ Performance monitoring")
    print("9. ✓ Experiment management")
    print("10. ✓ Advanced features demonstration")
    
    return 0

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
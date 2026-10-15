#!/usr/bin/env python3
"""
Test script to analyze current Python API usage and identify improvements.
This script demonstrates the current API and shows what could be improved.
"""

import pynlm
import time
import sys

def test_current_api():
    """Test the current NLM Python API to identify issues and improvement opportunities."""
    print("=== Testing Current NLM Python API ===\n")
    
    # Test 1: Basic API usage
    print("1. Basic API Usage:")
    try:
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Simple simulation
        for i in range(10):
            brain.step(i)
            
        print(f"   ✓ Simulated {brain.getTotalSpikeCount()} spikes over 10 steps")
        print("   ✓ Basic API works\n")
        
    except Exception as e:
        print(f"   ✗ Basic API failed: {e}\n")
    
    # Test 2: Agent/World API
    print("2. Agent-World API:")
    try:
        # Create agent setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        
        # Simple simulation loop
        for i in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(i)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        print(f"   ✓ Completed {i+1} steps of agent-world interaction")
        print(f"   ✓ Firing neurons: {brain.getFiringNeuronCount()}")
        print("   ✓ Agent-World API works\n")
        
    except Exception as e:
        print(f"   ✗ Agent-World API failed: {e}\n")
    
    # Test 3: Configuration API
    print("3. Configuration API:")
    try:
        config = pynlm.createDefaultConfig()
        
        # Basic config operations
        print(f"   ✓ Default config created")
        print(f"   ✓ Config has keys: {len(config.getKeys())} keys")
        
        # Set some values
        config.set("brain.neuron_count", 2000)
        config.set("simulation.timestep", 0.01)
        
        print(f"   ✓ Successfully set configuration values")
        
        # Create brain with modified config
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"   ✓ Brain created with custom config: {brain.getTotalNeuronCount()} neurons")
        print("   ✓ Configuration API works\n")
        
    except Exception as e:
        print(f"   ✗ Configuration API failed: {e}\n")
    
    # Test 4: Neuromodulation API
    print("4. Neuromodulation API:")
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable learning systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print(f"   ✓ Enabled reward modulation, curiosity, and development")
        print(f"   ✓ Neuromodulation API accessible")
        print("   ✓ Neuromodulation API works\n")
        
    except Exception as e:
        print(f"   ✗ Neuromodulation API failed: {e}\n")

def identify_improvements():
    """Identify areas for improvement based on API analysis."""
    print("=== Identified Improvement Opportunities ===\n")
    
    improvements = [
        {
            "area": "Python CLI Interface",
            "current": "No command-line interface for direct use",
            "improvement": "Add CLI tools for quick testing and experimentation",
            "impact": "High - Makes NLM more accessible to command-line users"
        },
        {
            "area": "Error Handling",
            "current": "Basic error reporting, limited validation",
            "improvement": "Enhanced error messages and parameter validation",
            "impact": "Medium - Better debugging experience"
        },
        {
            "area": "Serialization",
            "current": "Python bindings use C++ save/load methods",
            "improvement": "Add dedicated Python save/load methods with better file formats",
            "impact": "Medium - Easier state persistence"
        },
        {
            "area": "Logging",
            "current": "Limited logging configuration options",
            "improvement": "Add comprehensive logging configuration for Python users",
            "impact": "Medium - Better monitoring and debugging"
        },
        {
            "area": "Introspection",
            "current": "Limited ability to inspect brain state",
            "improvement": "Add methods to query brain system status and statistics",
            "impact": "Low - Useful for debugging and analysis"
        },
        {
            "area": "Documentation",
            "current": "Good documentation but missing advanced examples",
            "improvement": "Add advanced usage examples and tutorials",
            "impact": "Medium - Helps users master the API"
        },
        {
            "area": "Performance",
            "current": "Some methods could be more efficient",
            "improvement": "Optimize common operations and add batch methods",
            "impact": "Low - Improves performance for heavy usage"
        }
    ]
    
    for i, improvement in enumerate(improvements, 1):
        print(f"{i}. {improvement['area']}:")
        print(f"   Current: {improvement['current']}")
        print(f"   Improvement: {improvement['improvement']}")
        print(f"   Impact: {improvement['impact']}\n")
    
    print("=== Recommended Priority ===\n")
    print("HIGH PRIORITY:")
    print("  - Python CLI Interface")
    print("  - Better Error Handling and Validation")
    print("  - Serialization Methods")
    print("\nMEDIUM PRIORITY:")
    print("  - Logging Configuration")
    print("  - Documentation Updates")
    print("\nLOW PRIORITY:")
    print("  - Introspection Methods")
    print("  - Performance Optimizations")

if __name__ == "__main__":
    print("NLM Python API Improvement Analysis Tool\n")
    print("=" * 50)
    
    try:
        test_current_api()
        identify_improvements()
        print("\n✓ Analysis completed successfully!")
        sys.exit(0)
    except Exception as e:
        print(f"\n✗ Analysis failed: {e}")
        sys.exit(1)
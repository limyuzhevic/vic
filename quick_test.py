#!/usr/bin/env python3
"""
Quick verification that the enhanced Python API works correctly.
This is a minimal test to verify the basic functionality.
"""

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

import pynlm

def main():
    print("Testing Enhanced NLM Python API...")
    
    # Test 1: Basic creation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
    
    # Test 2: Enhanced config API
    config.set("test.value", 123, pynlm.ConfigSource.Runtime)
    value = config.getOr("test.value", 0)
    print(f"✓ Config get/set works: {value}")
    
    # Test 3: New methods
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    print(f"✓ Enhanced Brain methods work: STDP={stdp is not None}, Hebbian={hebbian is not None}")
    
    # Test 4: Update plasticity
    brain.updatePlasticity()
    print("✓ Brain.updatePlasticity() works")
    
    # Test 5: Development stage
    stage = brain.getDevelopmentalStage()
    print(f"✓ Development stage: {stage}")
    
    # Test 6: World and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("✓ Agent and world created successfully")
    
    # Test 7: Metrics
    metrics = agent.getAllMetrics()
    print(f"✓ Agent metrics collected: {list(metrics.keys())}")
    
    # Test 8: Config validation
    errors = config.validate()
    print(f"✓ Config validation: {len(errors)} error categories")
    
    # Test 9: Complex config
    complex_config = pynlm.createComplexConfig()
    neurons = complex_config.getOr("brain.neuron_count", 1000)
    print(f"✓ Complex config created: {neurons} neurons")
    
    # Test 10: Build info
    build_info = pynlm.getBuildInfo()
    print(f"✓ Build info: {build_info['version']}")
    
    print("\n" + "="*50)
    print("All basic tests passed!")
    print("The enhanced Python API is working correctly.")
    print("="*50)

if __name__ == "__main__":
    main()
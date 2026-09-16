#!/usr/bin/env python3
"""
Simple test to verify NLM Python bindings work correctly
Run this after building with: python setup.py build_ext --inplace
"""

import sys
import os

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

print("Testing NLM Python Bindings")
print("=" * 50)

try:
    # Test 1: Import the module
    print("Test 1: Importing pynlm module...")
    import pynlm
    print("✓ Successfully imported pynlm")
    
    # Test 2: Check module attributes
    print("\nTest 2: Checking module attributes...")
    print(f"  Module doc: {pynlm.__doc__}")
    
    # Test 3: Create default config
    print("\nTest 3: Creating default configuration...")
    config = pynlm.createDefaultConfig()
    print("✓ Created default configuration")
    
    # Test 4: Create brain
    print("\nTest 4: Creating brain instance...")
    brain = pynlm.createBrain(config)
    print("✓ Created brain instance")
    
    # Test 5: Initialize brain
    print("\nTest 5: Initializing brain...")
    success = brain.initialize()
    if success:
        print("✓ Brain initialized successfully")
    else:
        print("✗ Brain initialization failed")
        sys.exit(1)
    
    # Test 6: Take a simulation step
    print("\nTest 6: Taking simulation step...")
    brain.step(0)
    print("✓ Simulation step completed")
    
    # Test 7: Check brain statistics
    print("\nTest 7: Checking brain statistics...")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    # Test 8: Create world
    print("\nTest 8: Creating world...")
    world = pynlm.createSimpleWorld()
    print("✓ Created simple world")
    
    # Test 9: Configure world
    print("\nTest 9: Configuring world...")
    world.configure(20.0, 20.0, 16, 16)
    print("✓ Configured world")
    
    # Test 10: Create agent
    print("\nTest 10: Creating agent...")
    agent = pynlm.createAgentBrain(brain)
    print("✓ Created agent brain")
    
    # Test 11: Initialize agent
    print("\nTest 11: Initializing agent...")
    agent.initialize(world)
    print("✓ Agent initialized")
    
    # Test 12: Test basic functionality
    print("\nTest 12: Testing basic functionality...")
    world.reset()
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    print(f"✓ Processed sensor input and got action: {action}")
    
    # Test 13: Test learning features
    print("\nTest 13: Testing learning features...")
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    print("✓ Enabled learning features")
    
    # Test 14: Test debug info
    print("\nTest 14: Getting debug information...")
    brain.logStatus()
    print("✓ Retrieved brain status")
    
    print("\n" + "=" * 50)
    print("All tests passed! ✓")
    print("The NLM Python bindings are working correctly.")
    print("=" * 50)
    
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("Make sure the C++ bindings are compiled and installed.")
    print("Try: python setup.py build_ext --inplace")
    sys.exit(1)
    
except Exception as e:
    print(f"✗ Test failed with error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

# Additional test: Quick simulation example
def quick_simulation():
    """Quick simulation test"""
    print("\nQuick Simulation Test:")
    print("-" * 30)
    
    import pynlm
    
    # Simple brain simulation
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(50):
        brain.step(i)
    
    print(f"  Simulated {50} steps")
    print(f"  Generated {brain.getTotalSpikeCount()} spikes")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print("✓ Quick simulation test passed")

# Additional test: Agent-world simulation
def agent_world_test():
    """Agent-world simulation test"""
    print("\nAgent-World Simulation Test:")
    print("-" * 30)
    
    import pynlm
    
    # Create components
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(20):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("✓ Agent-world simulation test passed")

# Run additional tests
if __name__ == "__main__":
    # Run main test suite
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == "--full":
        print("Running full test suite...")
        quick_simulation()
        agent_world_test()
        print("\nFull test suite completed successfully!")
    else:
        print("Run with --full flag for additional tests")
        print("Example: python test_pynlm.py --full")

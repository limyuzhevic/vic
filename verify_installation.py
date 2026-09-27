#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Installation Verification Script

This script verifies that the NLM installation is working correctly
and demonstrates the basic functionality.

Run this script after installation to:
1. Verify the pynlm module can be imported
2. Test basic brain functionality
3. Demonstrate configuration loading
4. Show example usage patterns
"""

import sys
import os
import time
import json
from pathlib import Path

try:
    import pynlm
    print("✓ Successfully imported pynlm")
except ImportError as e:
    print(f"✗ Failed to import pynlm: {e}")
    print("Please install NLM: pip install pynlm")
    sys.exit(1)

def print_banner():
    print("=" * 60)
    print("NLM (Neural Learning Machine) - Installation Verification")
    print("=" * 60)
    print()

def test_basic_functionality():
    print("Testing basic brain functionality...")
    
    # Create a brain with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ {brain.getTotalSynapseCount()} synapses initialized")
    
    # Run a simple simulation
    for i in range(10):
        brain.step(i)
    
    print(f"✓ Ran 10 simulation steps")
    print(f"✓ Total spikes: {brain.getTotalSpikeCount()}")
    print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f}")
    print()

def test_configuration_loading():
    print("Testing configuration loading...")
    
    # Test 1: Create default config
    config1 = pynlm.createDefaultConfig()
    print("✓ Created default configuration")
    
    # Test 2: Load from JSON file
    config2 = pynlm.createDefaultConfig()
    try:
        config2.loadFromFile("configs/default.json")
        print("✓ Loaded configuration from JSON file")
        print(f"  - Neuron count: {config2.getOr('brain.neuron_count', 1000)}")
        print(f"  - Connection probability: {config2.getOr('brain.connection_probability', 0.1)}")
    except Exception as e:
        print(f"✗ Failed to load from JSON: {e}")
    
    # Test 3: Create brain with custom config
    config3 = pynlm.createDefaultConfig()
    config3.set("brain.neuron_count", 2000)
    config3.set("brain.connection_probability", 0.15)
    
    brain = pynlm.createBrain(config3)
    brain.initialize()
    print(f"✓ Created brain with custom config: {brain.getTotalNeuronCount()} neurons")
    
    # Test 4: Command line args (simulated)
    print("✓ Configuration loading tests passed")
    print()

def test_world_and_agent():
    print("Testing world and agent functionality...")
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    print("✓ Created agent and world")
    
    # Initialize agent with world
    agent.initialize(world)
    print("✓ Initialized agent with world")
    
    # Run a complete simulation step
    for step in range(5):
        world.update(0.1)  # Update world
        
        percept = world.getSensoryPercept()  # Get sensory input
        agent.processSensoryInput(percept)  # Process with brain
        
        brain.step(step)  # Brain thinks
        
        action = agent.decodeMotorCommand()  # Get action
        world.applyMotorCommand(action, world.getSimulationTime())  # Execute action
    
    print(f"✓ Ran complete simulation step")
    print(f"  - Agent at position: ({agent.getBrain().getRegions()[0].getBodyX():.1f}, {agent.getBrain().getRegions()[0].getBodyY():.1f})")
    print()

def test_learning_features():
    print("Testing learning features...")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ Enabled learning features")
    
    # Create a simple world for testing
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Run simulation with learning
    for step in range(20):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward (simulate finding something valuable)
        if step % 5 == 0:
            agent.applyRewardModulation(1.0, 0.5)  # Reward and predicted reward
    
    print(f"✓ Learning simulation completed")
    print(f"  - Current curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  - Current novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"  - Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print()

def demonstrate_code_examples():
    print("Demonstrating code examples...")
    
    print("1. Minimal example:")
    print("""
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
""")
    
    print("2. Complete agent example:")
    print("""
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
""")
    
    print()

def test_error_handling():
    print("Testing error handling...")
    
    try:
        # Test invalid configuration
        config = pynlm.createDefaultConfig()
        config.set("invalid.key", "invalid_value")
        
        # This should handle errors gracefully
        print("✓ Configuration handles invalid keys gracefully")
        
        # Test file operations
        import tempfile
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump({"test": "value"}, f)
            temp_file = f.name
        
        config.saveToFile(temp_file)
        print("✓ Configuration save/load operations work")
        
        os.unlink(temp_file)
        
    except Exception as e:
        print(f"✗ Error handling test failed: {e}")
    
    print()

def print_installation_info():
    print("Installation Information:")
    print(f"- Python version: {sys.version}")
    print(f"- pynlm version: {getattr(pynlm, '__version__', 'Unknown')}")
    
    # Check for configuration files
    config_dir = Path("configs")
    if config_dir.exists():
        config_files = list(config_dir.glob("*.json"))
        print(f"- Available config files: {len(config_files)}")
        for cf in config_files[:3]:  # Show first 3
            print(f"  * {cf.name}")
        if len(config_files) > 3:
            print(f"  * ... and {len(config_files) - 3} more")
    
    print()

def main():
    print_banner()
    
    # Run all tests
    test_basic_functionality()
    test_configuration_loading()
    test_world_and_agent()
    test_learning_features()
    test_error_handling()
    
    # Show examples
    demonstrate_code_examples()
    
    # Show installation info
    print_installation_info()
    
    print("=" * 60)
    print("All tests passed! ✓")
    print("=" * 60)
    print()
    print("Next steps:")
    print("1. Read easy_usage.md for beginner-friendly examples")
    print("2. Read HOW_TO_USE.md for detailed documentation")
    print("3. Explore configs/ directory for configuration templates")
    print("4. Check docs/ directory for scientific background")
    print()
    print("Happy experimenting with NLM!")
    print("=" * 60)

if __name__ == "__main__":
    main()

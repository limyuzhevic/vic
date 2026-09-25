#!/usr/bin/env python3
"""
NLM Basic Examples - Getting Started with NLM Python Bindings

These examples demonstrate the fundamental usage of NLM Python bindings.
They are designed to be easy to understand and run quickly.

## How to Use These Examples

1. Install NLM:
   ```bash
   pip install -e .
   ```

2. Run examples:
   ```bash
   python simple_brain_example.py
   python agent_example.py
   python learning_example.py
   ```

## Quick Setup

These examples assume:
- NLM Python bindings are installed
- C++ NLM core is built and functional

If you encounter import errors, check your NLM installation:
```bash
python -c "import pynlm; print(pynlm.__version__)"
```
"""

import pynlm

print("NLM Basic Examples")
print("=" * 50)

# Example 1: Simple Brain Creation and Simulation
print("\nExample 1: Simple Brain Creation and Simulation")
print("-" * 50)

def simple_brain_example():
    """Create a brain and run a simple simulation."""
    print("Creating brain with default configuration...")
    
    # Create brain using default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Initial firing neurons: {brain.getFiringNeuronCount()}")
    
    # Run simulation
    print("Running simulation...")
    for i in range(50):
        brain.step(i)
    
    print(f"✓ After 50 steps: {brain.getTotalSpikeCount()} total spikes")
    print(f"✓ Final firing neurons: {brain.getFiringNeuronCount()}")
    print("✓ Simple brain example completed successfully!")

simple_brain_example()

# Example 2: Brain with Custom Configuration
print("\nExample 2: Brain with Custom Configuration")
print("-" * 50)

def custom_config_example():
    """Create brain with custom configuration."""
    print("Creating brain with custom neuron count...")
    
    # Create configuration with specific neuron count
    config = pynlm.createConfigWithNeuronCount(2000)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Brain configured with {brain.getTotalSynapseCount()} synapses")
    
    # Run simulation
    for i in range(30):
        brain.step(i)
    
    print(f"✓ Simulated {30} steps")
    print(f"✓ Total activity: {brain.getTotalSpikeCount()} spikes")
    print("✓ Custom configuration example completed successfully!")

custom_config_example()

# Example 3: Complete Agent in World
print("\nExample 3: Complete Agent in World Simulation")
print("-" * 50)

def agent_world_example():
    """Create complete agent with world interaction."""
    print("Setting up agent-world simulation...")
    
    # Create components
    config = pynlm.createLearningConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize
    brain.initialize()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print(f"✓ Created agent with brain ({brain.getTotalNeuronCount()} neurons)")
    print(f"✓ Created world ({world.getWidth()}x{world.getHeight()} cells)")
    print(f"✓ Agent initialized")
    
    # Run simulation
    print("Running agent-world simulation...")
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 25 == 0:
            print(f"  Step {step}: " +
                  f"Firing={brain.getFiringNeuronCount()}, " +
                  f"Action={action}, " +
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    print(f"✓ Simulation completed: {brain.getTotalSpikeCount()} spikes")
    print(f"✓ Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print("✓ Agent-world example completed successfully!")

agent_world_example()

# Example 4: Learning Experiment
print("\nExample 4: Learning Experiment")
print("-" * 50)

def learning_example():
    """Run a learning experiment with plasticity."""
    print("Setting up learning experiment...")
    
    # Create learning-optimized brain
    brain = pynlm.createBrain(pynlm.createLearningConfig())
    brain.initialize()
    
    print(f"✓ Created learning brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ Brain initialized and ready for learning")
    
    # Configure for learning
    pynlm.configureBrainForLearning(brain)
    print(f"✓ Brain configured for learning (plasticity enabled)")
    
    # Run learning simulation
    print("Running learning simulation...")
    for step in range(80):
        # Provide learning input
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 40.0f)
        brain.step(step)
        
        if step % 20 == 0:
            print(f"  Step {step}: " +
                  f"Spikes={brain.getTotalSpikeCount()}, " +
                  f"Firing={brain.getFiringNeuronCount()}, " +
                  f"E/I ratio={brain.getExcitationInhibitionRatio():.3f}")
    
    print(f"✓ Learning experiment completed")
    print(f"✓ Total learning spikes: {brain.getTotalSpikeCount()}")
    print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print("✓ Learning example completed successfully!")

learning_example()

# Example 5: Brain State Management
print("\nExample 5: Brain State Management")
print("-" * 50)

def state_management_example():
    """Demonstrate brain state saving and loading."""
    print("Testing brain state management...")
    
    # Create and run brain
    brain = pynlm.createBrain(pynlm.createLearningConfig())
    brain.initialize()
    
    print(f"✓ Created and initialized brain")
    
    # Run some steps
    for i in range(20):
        brain.step(i)
    
    # Get statistics
    stats = pynlm.getBrainStats(brain)
    print(f"✓ Generated brain statistics")
    print(f"  First 100 chars: {stats[:100]}...")
    
    # Save state (demonstration - in real usage you would save to file)
    print(f"✓ Brain state ready for saving")
    print(f"✓ Current spike count: {brain.getTotalSpikeCount()}")
    print("✓ State management example completed successfully!")

state_management_example()

print("\n" + "=" * 50)
print("All basic examples completed successfully!")
print("\nSummary of examples covered:")
print("1. Simple brain creation and simulation")
print("2. Custom configuration with specific parameters")
print("3. Complete agent-world simulation with sensory-motor loops")
print("4. Learning experiment with plasticity mechanisms")
print("5. Brain state management and statistics")
print("\nEach example demonstrates core NLM Python binding concepts.")
print("Use these as starting points for more complex applications.")

# Check if examples directory exists and suggest next steps
examples_dir = "python/examples"
if os.path.exists(examples_dir):
    print(f"\nAdditional examples are available in {examples_dir}/")
    print("Check the README.md file in that directory for more examples.")
    
    # List available example files
    example_files = [f for f in os.listdir(examples_dir) if f.endswith('.py')]
    if example_files:
        print(f"\nAvailable example files:")
        for f in sorted(example_files):
            print(f"  - {f}")
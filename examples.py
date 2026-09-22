"""
NLM Python Examples
===================

This file demonstrates the NLM Python API with examples ranging from simple
usage to advanced features.

You can run these examples directly:
    python examples.py

Or import individual examples:
    from examples import simple_example, advanced_example, benchmark_example
"""

import pynlm
import time
import json
from pathlib import Path


def simple_example():
    """Simplest possible example - just create and run a brain."""
    print("=== Simple Example ===")
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()


def advanced_example():
    """Complete agent simulation with all systems enabled."""
    print("=== Advanced Example ===")
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all subsystems for advanced learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Running advanced agent simulation...")
    
    # 7. Run simulation loop
    for step in range(500):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Progress reporting
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    print(f"\nSimulation complete!")
    print(f"Final brain stats:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Brain health: {'Healthy' if brain.isHealthy() else 'Unhealthy'}")
    print()


def configuration_example():
    """Show different ways to create and configure brains."""
    print("=== Configuration Examples ===")
    
    # Example 1: Default configuration
    print("\n1. Default Configuration:")
    config1 = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    print(f"   Default neurons: {brain1.getTotalNeuronCount()}")
    
    # Example 2: Create from JSON file
    print("\n2. Configuration from JSON:")
    config_data = {
        "neuron_count": 2000,
        "region_count": 3,
        "connection_probability": 0.05,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012
    }
    
    # Save config to file
    config_file = "/tmp/nlm_config.json"
    with open(config_file, 'w') as f:
        json.dump(config_data, f)
    
    brain2 = pynlm.createBrainFromConfig(config_file)
    print(f"   Loaded neurons: {brain2.getTotalNeuronCount()}")
    
    # Example 3: Configuration from Python dictionary
    print("\n3. Configuration from dictionary:")
    config_dict = {
        "neuron_count": 1500,
        "region_count": 2,
        "connection_probability": 0.1
    }
    config3 = pynlm.createConfigFromDict(config_dict)
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    print(f"   Custom neurons: {brain3.getTotalNeuronCount()}")
    
    # Example 4: Configuration from command line (simulated)
    print("\n4. Configuration from command line:")
    # In real usage, you'd pass actual command line arguments
    test_args = ["prog", "--neuron-count=1000", "--region-count=1"]
    config4 = pynlm.createConfigFromArgs(len(test_args), test_args)
    brain4 = pynlm.createBrain(config4)
    print(f"   Arg parsed neurons: {brain4.getTotalNeuronCount()}")
    
    print()


def world_examples():
    """Demonstrate different world creation methods."""
    print("=== World Examples ===")
    
    # Example 1: Basic world
    print("\n1. Basic World:")
    world1 = pynlm.createSimpleWorld()
    world1.configure(width=10, height=10, visionWidth=5, visionHeight=5)
    world1.reset()
    print(f"   World size: {world1.getWidth()}x{world1.getHeight()}")
    print(f"   Vision: {world1.getSensoryInputSize()} pixels")
    
    # Example 2: World with seed
    print("\n2. World with random seed:")
    world2 = pynlm.createSimpleWorldWithSeed(
        width=15, height=15, 
        visionWidth=8, visionHeight=8, 
        seed=12345
    )
    print(f"   Fixed seed ensures reproducible world generation")
    
    # Example 3: Agent brain with world config
    print("\n3. Agent with world configuration:")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    agent = pynlm.createAgentBrainWithConfig(
        brain, 
        "width=30 height=30 visionWidth=10 visionHeight=10"
    )
    print(f"   Agent created with {world1.getWidth()}x{world1.getHeight()} world")
    
    # Example 4: Get world percepts
    print("\n4. World sensory processing:")
    percept = world1.getSensoryPercept()
    vision = percept.getVision()
    touch = percept.getTouch()
    internal = percept.getInternal()
    
    print(f"   Vision size: {len(vision)} pixels")
    print(f"   Touch sensors: {len(touch)} directions")
    print(f"   Internal signals: {len(internal)}")
    print(f"   Agent position: ({world1.getAgentBody().x:.1f}, {world1.getAgentBody().y:.1f})")
    print(f"   Agent energy: {world1.getAgentBody().energy:.1f}")
    
    print()


def neural_analysis_example():
    """Demonstrate advanced neural analysis tools."""
    print("=== Neural Analysis Example ===")
    
    # Create a brain and let it develop a bit
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Let it run for a while to develop
    for i in range(100):
        brain.step(i)
    
    print("\n1. Neural statistics:")
    print(f"   Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   Active neurons: {brain.getActiveNeuronCount()}")
    print(f"   Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"   Total spikes: {brain.getTotalSpikeCount()}")
    
    print("\n2. Convenience metrics:")
    print(f"   Neurons in millions: {brain.getNeuronsPerMillion():.3f}")
    print(f"   Synapses per neuron: {brain.getSynapsesPerNeuron():.3f}")
    print(f"   Brain healthy: {brain.isHealthy()}")
    
    print("\n3. Region analysis:")
    regions = brain.getRegions()
    for i, region in enumerate(regions[:3]):  # Show first 3 regions
        region_id = region->getId()
        neuron_count = region->getNeuronCount()
        print(f"   Region {i}: {neuron_count} neurons")
    
    print()


def agent_control_example():
    """Show advanced agent control features."""
    print("=== Agent Control Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable different combinations of subsystems
    print("\n1. Testing different subsystem configurations:")
    
    # Configuration 1: Only reward modulation
    agent.reset()
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(False)
    agent.enableDevelopment(False)
    agent.enableCuriosity(False)
    
    for _ in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(0)
        agent.decodeMotorCommand()
    
    print(f"   Reward only - Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"   Reward only - Curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Configuration 2: All systems enabled
    agent.reset()
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    for _ in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(0)
        agent.decodeMotorCommand()
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    print(f"   All systems - Novelty: {agent.getNoveltyLevel():.3f}")
    print(f"   All systems - Curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Configuration 3: Check system status
    print("\n2. System status:")
    print(f"   Reward modulation enabled: {agent.isRewardModulationEnabled()}")
    print(f"   Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")
    print(f"   Development enabled: {agent.isDevelopmentEnabled()}")
    print(f"   Curiosity enabled: {agent.isCuriosityEnabled()}")
    print(f"   Current developmental stage: {agent.getDevelopmentalStage()}")
    print(f"   Current neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    
    print()


def checkpoint_example():
    """Demonstrate checkpoint saving and loading."""
    print("=== Checkpoint Example ===")
    
    # Create and run a brain
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    print("\n1. Running brain to generate activity...")
    for i in range(200):
        brain1.step(i)
    
    print(f"   Initial spikes: {brain1.getTotalSpikeCount()}")
    print(f"   Initial firing rate: {brain1.getAverageFiringRate():.3f}")
    
    # Save checkpoint
    checkpoint_file = "/tmp/test_checkpoint.bin"
    print(f"\n2. Saving checkpoint to {checkpoint_file}...")
    
    if brain1.save(checkpoint_file):
        print("   Checkpoint saved successfully!")
    else:
        print("   Failed to save checkpoint")
    
    # Create new brain and load checkpoint
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    print(f"\n3. Loading checkpoint into new brain...")
    
    if brain2.load(checkpoint_file):
        print("   Checkpoint loaded successfully!")
        print(f"   Loaded spikes: {brain2.getTotalSpikeCount()}")
        print(f"   Loaded firing rate: {brain2.getAverageFiringRate():.3f}")
    else:
        print("   Failed to load checkpoint")
    
    # Run brain2 further
    print(f"\n4. Running loaded brain further...")
    for i in range(200, 250):
        brain2.step(i)
    
    print(f"   Final spikes: {brain2.getTotalSpikeCount()}")
    print(f"   Final firing rate: {brain2.getAverageFiringRate():.3f}")
    
    print()


def main():
    """Run all examples."""
    print("NLM Python API Examples")
    print("=" * 50)
    
    simple_example()
    configuration_example()
    world_examples()
    neural_analysis_example()
    agent_control_example()
    checkpoint_example()
    
    print("=" * 50)
    print("All examples completed successfully!")
    print("\nTo learn more, read HOW_TO_USE.md or docs/ARCHITECTURE.md")


if __name__ == "__main__":
    main()

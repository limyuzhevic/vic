#!/usr/bin/env python3
"""
NLM Python Bindings - Comprehensive Examples
===========================================

This file contains comprehensive examples for using the NLM Python bindings,
covering everything from basic usage to advanced features.
"""

import pynlm
import time
import json

def example_basic_brain():
    """Example 1: The simplest possible brain"""
    print("=== Example 1: Basic Brain ===")
    
    # One line to create a virtual brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize it
    brain.initialize()
    
    # Make it active
    brain.step(0)
    
    print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
    print()

def example_brain_simulation():
    """Example 2: Running a brain simulation"""
    print("=== Example 2: Brain Simulation ===")
    
    # Create and initialize
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Simulate with statistics
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"avg rate: {brain.getAverageFiringRate():.2f} Hz")
    
    print("Simulation complete!")
    print()

def example_complete_agent():
    """Example 3: Complete agent with world interaction"""
    print("=== Example 3: Complete Agent ===")
    
    # Setup with detailed configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Action: {action}")
    
    print("Agent simulation complete!")
    print()

def example_config_builder():
    """Example 4: Using configuration builder"""
    print("=== Example 4: Configuration Builder ===")
    
    # Create a configured config directly
    config = pynlm.createConfigWithSettings(
        neuron_count=500,
        region_count=2,
        synapse_density=0.15,
        random_seed=12345
    )
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    print(f"Expected synapses: {brain.getTotalSynapseCount()}")
    print()

def example_sensory_processing():
    """Example 5: Working with sensory input"""
    print("=== Example 5: Sensory Processing ===")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    print(f"Vision dimensions: {percept.getVisionWidth()} x {percept.getVisionHeight()}")
    print(f"Vision data length: {len(percept.getVision())}")
    print(f"Touch data length: {len(percept.getTouch())}")
    print(f"Internal signals length: {len(percept.getInternal())}")
    print(f"Proprioception length: {len(percept.getProprioception())}")
    print(f"Audio signals length: {len(percept.getAudio())}")
    print(f"Text: '{percept.getText()}'")
    
    # Modify sensory data
    vision_data = percept.getVision()
    for i in range(min(3, len(vision_data))):
        vision_data[i] = 0.8  # Make some pixels bright
    
    print("Modified vision data")
    print()

def example_agent_brain_interface():
    """Example 6: Agent brain interface methods"""
    print("=== Example 6: Agent Brain Interface ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable different subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run a few steps to gather data
    for step in range(10):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check agent capabilities
    print(f"Sensory input size: {agent.getSensoryInputSize()}")
    print(f"Motor output size: {agent.getMotorOutputSize()}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Prediction error: {agent.getPredictionError():.3f}")
    print(f"Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Developmental stage: {agent.getDevelopmentalStage()}")
    print(f"Concept count: {agent.getConceptCount()}")
    print(f"Self-model confidence: {agent.getSelfModelConfidence():.3f}")
    print(f"Social observations: {agent.getSocialObservationCount()}")
    print()
    
    # Show thought trace
    print("Thought trace:")
    print(agent.getThoughtTrace())
    print()

def example_simulation_helpers():
    """Example 7: Using simulation helper functions"""
    print("=== Example 7: Simulation Helpers ===")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Running simulation episode with helper function...")
    
    # Use the simulation helper (this is equivalent to running the full loop)
    start_time = time.time()
    pynlm.run_simulation_episode(brain, world, agent, 50, enable_learning=True)
    end_time = time.time()
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print()

def example_data_conversion():
    """Example 8: Working with data conversion utilities"""
    print("=== Example 8: Data Conversion Utilities ===")
    
    # Create a brain to test with
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get neurons by type
    excitatory_neurons = pynlm.get_neurons_by_type(brain, pynlm.NeuronType.Excitatory)
    inhibitory_neurons = pynlm.get_neurons_by_type(brain, pynlm.NeuronType.Inhibitory)
    
    print(f"Excitatory neurons: {len(excitatory_neurons)}")
    print(f"Inhibitory neurons: {len(inhibitory_neurons)}")
    
    # Get average firing rates
    avg_exc_rate = pynlm.get_average_firing_rate_by_type(brain, pynlm.NeuronType.Excitatory)
    avg_inh_rate = pynlm.get_average_firing_rate_by_type(brain, pynlm.NeuronType.Inhibitory)
    
    print(f"Average excitatory firing rate: {avg_exc_rate:.3f} Hz")
    print(f"Average inhibitory firing rate: {avg_inh_rate:.3f} Hz")
    
    # Convert vectors to lists
    vision_data = [0.1, 0.2, 0.3, 0.4, 0.5]
    python_list = pynlm.vectorToList(vision_data)
    print(f"Vector to list conversion: {python_list}")
    print()

def example_text_encoding():
    """Example 9: Phase 6 text encoding"""
    print("=== Example 9: Text Encoding (Phase 6) ===")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Encode text
    text_pattern = agent.encodeText("hello world")
    print(f"Text 'hello world' encoded as neuron indices: {text_pattern}")
    
    # Inject text into percept
    percept = world.getSensoryPercept()
    percept.setText("hello world")
    
    # Process the text
    agent.processSensoryInput(percept)
    
    print(f"Percept text: '{percept.getText()}'")
    print()

def example_checkpointing():
    """Example 10: Checkpoint save/load"""
    print("=== Example 10: Checkpointing ===")
    
    # Create and simulate first brain
    config1 = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    for step in range(20):
        brain1.step(step)
    
    # Save checkpoint
    checkpoint_file = "test_checkpoint.bin"
    save_success = brain1.save(checkpoint_file)
    print(f"Saved checkpoint: {save_success}")
    
    # Create new brain and load checkpoint
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    load_success = brain2.load(checkpoint_file)
    print(f"Loaded checkpoint: {load_success}")
    
    # Verify they have similar properties
    print(f"Brain1 neurons: {brain1.getTotalNeuronCount()}")
    print(f"Brain2 neurons: {brain2.getTotalNeuronCount()}")
    print(f"Brain1 spikes: {brain1.getTotalSpikeCount()}")
    print(f"Brain2 spikes: {brain2.getTotalSpikeCount()}")
    
    # Continue simulation with loaded brain
    for step in range(20, 40):
        brain2.step(step)
    
    print(f"Brain2 final spikes after continued simulation: {brain2.getTotalSpikeCount()}")
    print()

def example_advanced_configuration():
    """Example 11: Advanced configuration"""
    print("=== Example 11: Advanced Configuration ===")
    
    # Create config and modify it
    config = pynlm.createDefaultConfig()
    
    # Modify configuration parameters
    if config.has("neuron_count"):
        config.set("neuron_count", 2000)
    
    if config.has("connection_probability"):
        config.set("connection_probability", 0.05)
    
    # Create brain with modified config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with custom config:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    print()

def main():
    """Run all examples"""
    print("NLM Python Bindings - Comprehensive Examples")
    print("=" * 50)
    print()
    
    examples = [
        example_basic_brain,
        example_brain_simulation,
        example_complete_agent,
        example_config_builder,
        example_sensory_processing,
        example_agent_brain_interface,
        example_simulation_helpers,
        example_data_conversion,
        example_text_encoding,
        example_checkpointing,
        example_advanced_configuration,
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"Error in example {i}: {e}")
            print()
    
    print("All examples completed!")
    print()
    print("For more advanced usage, refer to the documentation and API reference.")

if __name__ == "__main__":
    main()
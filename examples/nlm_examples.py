#!/usr/bin/env python3
"""
NLM Python Examples - Advanced Usage Examples

This file demonstrates advanced usage patterns for the NLM Python bindings.
It covers complex scenarios, best practices, and advanced features.

Examples include:
- Basic brain setup and configuration
- Sensory processing and integration
- Memory system usage
- Neuromodulation control
- Advanced action and movement
- Experiment control and metrics
- Visualization and debugging
- Performance optimization
"""

import sys
import time
import nlm
from typing import List, Optional, Dict, Any

def example_basic_brain_setup():
    """
    Example: Basic brain setup with advanced configuration
    """
    print("=== Basic Brain Setup ===")
    
    # Create brain with advanced configuration
    config = nlm.createDefaultConfig()
    
    # Configure brain parameters
    config.set("neuron_count", 1000, nlm.ConfigSource.Runtime)
    config.set("region_count", 3, nlm.ConfigSource.Runtime)
    config.set("connection_probability", 0.15, nlm.ConfigSource.Runtime)
    config.set("random_seed", 42, nlm.ConfigSource.Runtime)
    
    # Create brain with configuration
    brain = nlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    
    return brain

def example_sensory_integration(brain):
    """
    Example: Advanced sensory integration
    """
    print("\n=== Sensory Integration ===")
    
    # Create various sensory inputs
    vision = nlm.createVision(64, 64, 3)
    audio = nlm.createAudio(44100, 2000)
    internal_signals = nlm.createInternalSignals()
    
    # Fill with sample data
    vision_data = [0.1] * 64 * 64 * 3
    audio_data = [0.5] * 2000
    internal_data = [0.0, 1.0, 0.5, -0.5]
    
    for i, val in enumerate(vision_data):
        vision.setData(vision_data, i)
    
    for i, val in enumerate(audio_data):
        audio.setData(audio_data, i)
    
    for i, val in enumerate(internal_data):
        internal_signals.addSignal(val)
    
    # Create sensory percept
    percept = nlm.SensoryPercept()
    percept.setVision(vision)
    percept.setAudio(audio)
    percept.setInternal(internal_signals)
    
    # Inject into brain
    brain.receiveSensoryInput(percept)
    
    print("Injected sensory data into brain")
    print(f"Vision dimensions: {vision.getDimensions()}")
    print(f"Audio samples: {audio.getDimensions()}")
    print(f"Internal signals: {internal_signals.getDimensions()}")

def example_memory_systems(brain):
    """
    Example: Working with memory systems
    """
    print("\n=== Memory Systems ===")
    
    # Access memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    if working_memory:
        print(f"Working memory capacity: {working_memory.getCapacity()}")
        print(f"Active traces: {working_memory.getActiveTraces()}")
        
        # Simulate storing some information
        working_memory.storeToNeuron(nlm.INVALID_NEURON_ID, 0.5)
        working_memory.storeToNeuron(nlm.INVALID_NEURON_ID, 0.8)
        
        # Update working memory
        working_memory.update(0.01)
    
    if episodic_memory:
        print(f"Max episodes: {episodic_memory.getMaxEpisodes()}")
        print(f"Current episodes: {episodic_memory.getEpisodeCount()}")
        
        # Simulate storing an episode
        episode = nlm.EpisodicMemoryItem()
        episode.timestamp = brain.getTotalSpikeCount()
        episode.reward = 0.5
        
        # Run a few steps to generate some activity
        for step in range(50):
            brain.step(step)
        
        # Store the episode
        episodic_memory.storeEpisode(episode)
        
        print(f"Episode stored. Total episodes: {episodic_memory.getEpisodeCount()}")

def example_neuromodulation(brain):
    """
    Example: Neuromodulation control
    """
    print("\n=== Neuromodulation ===")
    
    # Access neuromodulation systems
    dopamine = brain.getDopamine()
    curiosity = brain.getCuriosity()
    novelty = brain.getNovelty()
    
    if dopamine:
        print(f"Initial dopamine level: {dopamine.getLevel()}")
        
        # Apply reward-based modulation
        dopamine.setLevel(0.8)
        print(f"Dopamine level after reward: {dopamine.getLevel()}")
        
        # Apply neuromodulation to brain
        reward_mod = nlm.Neuromodulator()
        reward_mod.setLevel(0.9)
        brain.applyNeuromodulation(reward_mod)
        
        print(f"Plasticity factor: {dopamine.getPlasticityFactor()}")
    
    if curiosity:
        print(f"Initial curiosity level: {curiosity.getLevel()}")
        
        # Enable curiosity-driven exploration
        curiosity.setLevel(0.7)
        
        # Update curiosity
        curiosity.update(0.01)
        print(f"Curiosity after update: {curiosity.getLevel()}")

def example_action_selection(brain):
    """
    Example: Advanced action selection
    """
    print("\n=== Action Selection ===")
    
    # Produce action
    action = brain.produceAction()
    
    if action:
        print(f"Action type: {action.getType()}")
        print(f"Action name: {action.getName()}")
        print(f"Action parameters: {action.getParameters()}")
        
        # Create action with parameters
        advanced_action = nlm.Action(nlm.ActionType.Interact, [0.5, 0.8, 0.3])
        print(f"Advanced action: {advanced_action.getName()} with {len(advanced_action.getParameters())} parameters")
        
        # Clone action
        cloned_action = advanced_action.clone()
        print(f"Cloned action type: {cloned_action.getType()}")
    
    else:
        print("No action produced")

def example_advanced_world_interaction(brain):
    """
    Example: Advanced world interaction
    """
    print("\n=== Advanced World Interaction ===")
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(100, 100, 32, 32)
    
    # Set agent start position
    world.setAgentStart(50, 50)
    
    # Create agent brain
    agent_brain = nlm.createAgentBrain(brain)
    
    # Run agent for several steps
    for step in range(100):
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process with agent brain
        agent_brain.processSensoryInput(percept)
        
        # Get motor command
        motor_command = agent_brain.decodeMotorCommand()
        
        # Apply command to world
        world.applyMotorCommand(motor_command, step * 0.01)
        
        # Get agent body state
        body = world.getAgentBody()
        
        if step % 20 == 0:
            print(f"Step {step}: Agent at ({body.x}, {body.y}), facing {body.orientation} degrees")
            print(f"Energy: {body.energy}, Health: {body.health}")

def example_checkpointing(brain):
    """
    Example: Checkpointing and state management
    """
    print("\n=== Checkpointing ===")
    
    # Save brain state
    saved_state = "/tmp/nlm_brain_checkpoint.nlm"
    
    if brain.save(saved_state):
        print(f"Brain state saved to {saved_state}")
        
        # Create new brain for loading
        config = nlm.createDefaultConfig()
        brain2 = nlm.createBrain(config)
        brain2.initialize()
        
        # Load saved state
        if brain2.load(saved_state):
            print("Brain state loaded successfully")
            print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")
        else:
            print("Failed to load brain state")
    else:
        print("Failed to save brain state")

def example_experiment_control():
    """
    Example: Experiment control and metrics
    """
    print("\n=== Experiment Control ===")
    
    # Create brain
    brain = example_basic_brain_setup()
    
    # Run experiment loop
    total_reward = 0.0
    step_count = 0
    
    for episode in range(10):
        print(f"Episode {episode + 1}/10")
        
        # Run episode
        episode_reward = 0.0
        for step in range(200):
            # Apply random sensory input to stimulate learning
            vision = nlm.createVision(10, 10, 1)
            vision_data = [i * 0.1 for i in range(100)]
            
            # Create simple percept
            percept = nlm.SensoryPercept()
            percept.setVision(vision)
            
            # Inject into brain
            brain.receiveSensoryInput(percept)
            
            # Step brain
            brain.step(step_count)
            
            # Produce action
            action = brain.produceAction()
            
            # Simulate reward
            if action and action.getType() != nlm.ActionType.Wait:
                episode_reward += 0.1
            
            step_count += 1
        
        total_reward += episode_reward
        print(f"  Episode reward: {episode_reward:.2f}")
    
    avg_reward = total_reward / 10
    print(f"Average reward per episode: {avg_reward:.2f}")
    print(f"Total steps: {step_count}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")

def example_performance_optimization():
    """
    Example: Performance optimization techniques
    """
    print("\n=== Performance Optimization ===")
    
    # Create multiple brains for comparison
    brains = []
    start_time = time.time()
    
    for i in range(5):
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 500, nlm.ConfigSource.Runtime)
        brain = nlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
    
    setup_time = time.time() - start_time
    print(f"Setup time for 5 brains: {setup_time:.3f}s")
    print(f"Average setup time: {setup_time/5:.3f}s per brain")
    
    # Run simulation with timing
    total_time = 0.0
    steps_per_brain = 100
    
    for i, brain in enumerate(brains):
        brain_start = time.time()
        
        for step in range(steps_per_brain):
            brain.step(step)
        
        brain_time = time.time() - brain_start
        total_time += brain_time
        print(f"Brain {i+1}: {brain_time:.3f}s for {steps_per_brain} steps")
    
    avg_time_per_step = total_time / (len(brains) * steps_per_brain)
    print(f"Average time per step: {avg_time_per_step*1000:.2f}ms")

def example_advanced_api_usage():
    """
    Example: Advanced API usage patterns
    """
    print("\n=== Advanced API Usage ===")
    
    # Use configuration utilities
    config = nlm.createDefaultConfig()
    
    # Validate configuration
    try:
        nlm.validateConfig(config)
        print("Configuration validation passed")
    except Exception as e:
        print(f"Configuration validation failed: {e}")
    
    # Access constants
    constants = nlm.getConstants()
    print(f"Constants available: {list(constants.keys())}")
    
    # Use factory methods
    small_brain = nlm.createBrainWithDefaults(100, 2)
    large_brain = nlm.createBrainWithDefaults(5000, 5)
    
    print(f"Small brain: {small_brain.getTotalNeuronCount()} neurons")
    print(f"Large brain: {large_brain.getTotalNeuronCount()} neurons")
    
    # Use clone methods
    vision = nlm.createVision(32, 32, 3)
    cloned_vision = vision.clone()
    
    print(f"Original vision dimensions: {vision.getDimensions()}")
    print(f"Cloned vision dimensions: {cloned_vision.getDimensions()}")

def main():
    """
    Main function to run all examples
    """
    print("NLM Python Advanced Examples")
    print("==========================")
    
    try:
        # Example 1: Basic setup
        brain = example_basic_brain_setup()
        
        # Example 2: Sensory integration
        example_sensory_integration(brain)
        
        # Example 3: Memory systems
        example_memory_systems(brain)
        
        # Example 4: Neuromodulation
        example_neuromodulation(brain)
        
        # Example 5: Action selection
        example_action_selection(brain)
        
        # Example 6: World interaction
        example_advanced_world_interaction(brain)
        
        # Example 7: Checkpointing
        example_checkpointing(brain)
        
        # Example 8: Experiment control
        example_experiment_control()
        
        # Example 9: Performance optimization
        example_performance_optimization()
        
        # Example 10: Advanced API usage
        example_advanced_api_usage()
        
        print("\n" + "="*50)
        print("All examples completed successfully!")
        print("="*50)
        
    except Exception as e:
        print(f"\nError in example execution: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())

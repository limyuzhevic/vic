#!/usr/bin/env python3
"""
Example usage of the NLM Python bindings.

This script demonstrates the improved Python API for the Neural Learning Machine (NLM) framework.
It showcases the convenience methods, memory systems, prediction systems, neuromodulation,
development, and plasticity systems now available in Python.
"""

import pynlm
import numpy as np


def basic_example():
    """Basic example showing how to create and use an NLM brain."""
    print("=== Basic NLM Example ===")
    
    # Create a default configuration
    config = pynlm.createDefaultConfig()
    print(f"Created config: {config}")
    
    # Create a brain with the configuration
    brain = pynlm.createBrain(config)
    print(f"Created brain: {brain}")
    
    # Initialize the brain
    if brain.initialize():
        print("Brain initialized successfully")
    else:
        print("Failed to initialize brain")
        return
    
    # Create an agent brain interface
    agent_brain = pynlm.createAgentBrain(brain)
    print(f"Created agent brain: {agent_brain}")
    
    # Create a simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
    world.setAgentStart(50, 50)
    print(f"Created world: {world.getWidth()}x{world.getHeight()}")
    
    # Initialize the agent brain with the world
    agent_brain.initialize(world)
    
    print("Basic setup completed successfully!\n")


def sensory_example():
    """Example showing sensory input processing."""
    print("=== Sensory Input Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent_brain = pynlm.createAgentBrain(brain)
    
    # Create a vision sensory input
    vision_data = np.random.rand(16, 16, 3).astype(np.float32)
    vision = pynlm.Vision(16, 16, 3)
    vision.setData(vision_data.flatten())
    
    # Create a percept with the vision data
    percept = pynlm.SensoryPercept()
    percept.setVision(vision)
    
    # Process the sensory input
    agent_brain.processSensoryInput(percept)
    print(f"Processed vision data: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")
    
    # Check agent brain state
    print(f"Agent brain state: {agent_brain}")
    print(f"Neuromodulation levels: {agent_brain.get_neuromodulation_summary()}")
    
    print("Sensory processing completed successfully!\n")


def memory_example():
    """Example showing memory system usage."""
    print("=== Memory System Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create memory systems
    memory_systems = pynlm.createMemorySystem(brain)
    working_memory = memory_systems["working_memory"]
    episodic_memory = memory_systems["episodic_memory"]
    associative_memory = memory_systems["associative_memory"]
    
    print("Created memory systems:")
    print(f"  - Working memory capacity: {working_memory.getCapacity()}")
    print(f"  - Working memory decay rate: {working_memory.getDecayRate()}")
    
    # Store some patterns in working memory
    test_pattern = np.random.rand(100).astype(np.float32)
    working_memory.store(test_pattern, strength=1.0f)
    print(f"Stored pattern in working memory")
    print(f"Memory neurons: {working_memory.getMemoryNeurons().size()}")
    
    # Store an episode
    episode = pynlm.EpisodicMemoryItem()
    episode.sensoryState = np.random.rand(256).astype(np.float32)  # 16x16 vision
    episode.positionX = 10.0f
    episode.positionY = 10.0f
    episode.action = pynlm.ActionType.MoveForward
    episode.reward = 5.0f
    
    episodic_memory.storeEpisode(episode)
    print(f"Stored episode: reward={episode.reward}, position=({episode.positionX}, {episode.positionY})")
    
    # Query memory by location
    episodes_near_10_10 = episodic_memory.retrieveByLocation(10.0f, 10.0f, 1.0f)
    print(f"Found {len(episodes_near_10_10)} episodes near position (10, 10)")
    
    # Create associations between patterns
    pattern_a = np.random.rand(50).astype(np.float32)
    pattern_b = np.random.rand(50).astype(np.float32)
    associative_memory.associate(pattern_a, pattern_b, strength=0.8f)
    print(f"Created association between patterns")
    
    # Get statistics from all memory systems
    stats = pynlm.get_memory_system_stats(working_memory, episodic_memory, associative_memory)
    print("Memory system statistics:")
    for system_name, system_stats in stats.items():
        print(f"  {system_name}: {system_stats}")
    
    # Create a human-readable report
    report = pynlm.createMemoryReport(memory_systems)
    print("Memory system report:")
    print(report)
    
    print("Memory system example completed successfully!\n")


def prediction_example():
    """Example showing prediction system usage."""
    print("=== Prediction System Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get the prediction system from the brain
    prediction_system = brain.getPredictionSystem()
    if prediction_system:
        print("Prediction system is available")
        
        # Get prediction error history
        history = prediction_system.getErrorHistory()
        print(f"Prediction error history size: {len(history)}")
        
        # Get recent errors
        recent_errors = prediction_system.get_recent_errors(5)
        print(f"Recent prediction errors: {recent_errors}")
        
        # Get average error
        avg_error = prediction_system.get_avg_error()
        print(f"Average prediction error: {avg_error}")
        
        # Check if system is learning
        is_learning = prediction_system.is_learning()
        print(f"Prediction system is learning: {is_learning}")
    else:
        print("Prediction system not available")
    
    print("Prediction system example completed successfully!\n")


def neuromodulation_example():
    """Example showing neuromodulation system usage."""
    print("=== Neuromodulation System Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create neuromodulation systems
    neuromodulation = pynlm.createNeuromodulationSystem()
    
    dopamine = neuromodulation["dopamine"]
    curiosity = neuromodulation["curiosity"]
    novelty = neuromodulation["novelty"]
    prediction_error = neuromodulation["prediction_error"]
    
    print("Neuromodulation systems created")
    
    # Signal reward and prediction error
    dopamine.signalReward(1.0f)
    dopamine.signalRewardPredictionError(0.5f)
    print(f"Dopamine level: {dopamine.getLevel()}")
    
    # Update curiosity based on novelty and prediction error
    novelty_level = 0.3f
    prediction_error_value = 0.2f
    curiosity.update(novelty_level, prediction_error_value, 0.001f)
    print(f"Curiosity level: {curiosity.getLevel()}, exploration drive: {curiosity.getExplorationDrive()}")
    
    # Set novelty level
    novelty.setLevel(0.8f)
    print(f"Novelty level: {novelty.getLevel()}")
    
    # Update prediction error
    prediction_error.update(0.5f, 0.8f)  # prediction=0.5, actual=0.8
    print(f"Prediction error: {prediction_error.getValue()}")
    
    # Get summary of neuromodulation levels (when agent brain is available)
    print("Note: Neuromodulation summary is best accessed through AgentBrain")
    
    print("Neuromodulation example completed successfully!\n")


def development_example():
    """Example showing development system usage."""
    print("=== Development System Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get development system
    development_system = brain.getDevelopmentSystem()
    if development_system:
        print("Development system is available")
        
        # Get current developmental stage
        stage = development_system.getStage()
        stage_name = development_system.getStageName()
        print(f"Current developmental stage: {stage} ({stage_name})")
        
        # Get developmental age
        age = development_system.getDevelopmentalAge()
        print(f"Developmental age: {age}")
        
        # Check if in critical period
        is_critical = development_system.isCriticalPeriod()
        print(f"Is in critical period: {is_critical}")
        
        if is_critical:
            progress = development_system.getCriticalPeriodProgress()
            print(f"Critical period progress: {progress}")
        
        # Get plasticity modifier
        plasticity = development_system.getPlasticityModifier()
        print(f"Plasticity modifier: {plasticity}")
        
        # Advance development
        development_system.advanceStage()
        new_stage = development_system.getStage()
        print(f"Advanced to new stage: {new_stage}")
    else:
        print("Development system not available")
    
    print("Development system example completed successfully!\n")


def plasticity_example():
    """Example showing plasticity system usage."""
    print("=== Plasticity System Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create plasticity systems
    stsdp = pynlm.STDP()
    hebbian = pynlm.Hebbian()
    structural = pynlm.StructuralPlasticity()
    
    print("Plasticity systems created")
    
    # Configure STDP
    stsdp.configure(0.01f, 0.012f, 10.0f)  # LTP weight, LTD weight, time constant
    print(f"STDP configured - LTP: {stsdp.getLTPWeight()}, LTD: {stsdp.getLTDWeight()}, tau: {stsdp.getTimeConstant()}")
    
    # Configure Hebbian
    hebbian.setLearningRate(0.1f)
    hebbian.setMaxWeight(1.0f)
    print(f"Hebbian configured - learning rate: {hebbian.getLearningRate()}, max weight: {hebbian.getMaxWeight()}")
    
    # Configure Structural Plasticity
    structural.setSynaptogenesisRate(0.0001f)
    structural.setPruningRate(0.00001f)
    print(f"Structural Plasticity configured - synaptogenesis: {structural.getSynaptogenesisRate()}, pruning: {structural.getPruningRate()}")
    
    print("Plasticity system example completed successfully!\n")


def spike_example():
    """Example showing spike system usage."""
    print("=== Spike System Example ===")
    
    spike_system = pynlm.SpikeSystem()
    print("Spike system created")
    
    # Perform a spike system step
    spike_system.step(0)
    print("Spike system step completed")
    
    print("Spike system example completed successfully!\n")


def motor_example():
    """Example showing motor system usage."""
    print("=== Motor System Example ===")
    
    motor_system = pynlm.MotorSystem()
    print("Motor system created")
    
    # Perform a motor system step
    motor_system.step(0)
    print("Motor system step completed")
    
    print("Motor system example completed successfully!\n")


def full_integration_example():
    """Example showing full integration of all systems."""
    print("=== Full Integration Example ===")
    
    # Create complete brain with all systems
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent_brain = pynlm.createAgentBrain(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
    world.setAgentStart(50, 50)
    
    # Initialize agent brain
    agent_brain.initialize(world)
    
    # Create memory systems
    memory_systems = pynlm.createMemorySystem(brain)
    
    # Create neuromodulation systems
    neuromodulation = pynlm.createNeuromodulationSystem()
    
    # Main loop simulation
    print("Starting simulation loop...")
    for step in range(5):
        print(f"\n--- Step {step} ---")
        
        # Get current sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Decode motor command
        action = agent_brain.decodeMotorCommand()
        print(f"Action selected: {action}")
        
        # Apply motor command
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent_brain.updateDevelopment(0.001f)
        
        # Get current state
        print(f"  Developmental stage: {agent_brain.get_stage_name()}")
        print(f"  Neuromodulation levels: {agent_brain.get_neuromodulation_summary()}")
        print(f"  Memory neurons: {memory_systems['working_memory'].getMemoryNeurons().size()}")
        print(f"  Simulation time: {world.getSimulationTime()}")
        
        # Update world
        world.update(0.001f)
    
    print("\nFull integration example completed successfully!\n")


def main():
    """Run all examples."""
    print("NLM Python Bindings Examples")
    print("=" * 50)
    print()
    
    basic_example()
    sensory_example()
    memory_example()
    prediction_example()
    neuromodulation_example()
    development_example()
    plasticity_example()
    spike_example()
    motor_example()
    full_integration_example()
    
    print("All examples completed successfully!")
    print("\nKey features demonstrated:")
    print("  - Basic brain creation and initialization")
    print("  - Sensory input processing with Vision and Audio")
    print("  - Working memory, episodic memory, and associative memory")
    print("  - Prediction system with error tracking")
    print("  - Neuromodulation systems (Dopamine, Curiosity, Novelty, PredictionError)")
    print("  - Development system with stages and plasticity modulation")
    print("  - Plasticity systems (STDP, Hebbian, StructuralPlasticity)")
    print("  - Spike system for neural dynamics")
    print("  - Motor system for action production")
    print("  - Full integration of all systems")
    print("  - Convenience methods and helper functions")
    print("  - Statistics and reporting for all systems")
    print("\nThe improved Python bindings provide an intuitive, Pythonic API for")
    print("working with the NLM neural learning machine framework!")


if __name__ == "__main__":
    main()

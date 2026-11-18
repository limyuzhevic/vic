#!/usr/bin/env python3
"""
NLM Python Examples - Comprehensive Guide

This file demonstrates all the key features of the NLM Python bindings.
Copy and paste these examples into your Python scripts to get started!

To use these examples, you need:
1. The pynlm module installed (pip install pynlm)
2. The NLM C++ libraries built

For more information, visit: https://github.com/nlm-project/nlm
"""

import pynlm
import sys
import time

def example_1_basic_brain():
    """Example 1: Create and use a basic brain"""
    print("=== Example 1: Basic Brain ===")
    
    # Step 1: Create a default configuration
    config = pynlm.createDefaultConfig()
    
    # Step 2: Create a brain with the configuration
    brain = pynlm.createBrain(config)
    
    # Step 3: Initialize the brain
    brain.initialize()
    
    # Step 4: Run the brain for a few steps
    for i in range(10):
        brain.step(i)
    
    # Step 5: Get some statistics
    print(f"Brain has {brain.getTotalNeuronCount()} neurons")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    # Step 6: Get developmental stage
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    print()

def example_2_brain_world_agent():
    """Example 2: Complete brain-world-agent system"""
    print("=== Example 2: Brain-World-Agent System ===")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(100):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory percept
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Get reward and provide feedback
        reward = world.getReward()
        if reward > 0:
            agent.applyRewardModulation(reward, 0.0)
    
    print(f"Simulation complete!")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Neurons learned {len(agent.getActionHistory())} actions")
    
    print()

def example_3_learning_experiment():
    """Example 3: Advanced learning experiment"""
    print("=== Example 3: Advanced Learning Experiment ===")
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    
    # Set custom parameters
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("plasticity_learning_rate", 0.1)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.01)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True, 0.5)
    agent.enableCuriosity(True, 0.3)
    agent.enableStructuralPlasticity(True, 0.001)
    
    # Run learning experiment
    learning_rates = []
    for step in range(200):
        # Generate varied environment
        world.update(0.1)
        
        # Random movement with some patterns
        if step % 5 == 0:
            # Stay in place and observe
            action = pynlm.Action(pynlm.ActionType.Look)
        else:
            # Move randomly
            action = pynlm.Action(pynlm.ActionType.MoveForward)
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get action and move
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and learn
        reward = world.getReward()
        agent.applyRewardModulation(reward, 0.0)
        
        # Record learning rate
        if step > 0:
            learning_rates.append(reward)
    
    avg_reward = sum(learning_rates) / len(learning_rates) if learning_rates else 0
    print(f"Learning experiment complete!")
    print(f"Average reward: {avg_reward:.3f}")
    print(f"Total steps: {len(learning_rates)}")
    print(f"Max consecutive positive rewards: {calculate_max_consecutive(learning_rates)}")
    
    print()

def example_4_memory_system():
    """Example 4: Working with memory systems"""
    print("=== Example 4: Memory System ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Collect some experiences
    experiences = []
    for step in range(50):
        world.update(0.1)
        
        # Simple exploration pattern
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Store experience
        percept = world.getSensoryPercept()
        reward = world.getReward()
        
        experiences.append({
            'step': step,
            'percept': percept,
            'reward': reward,
            'action': action,
            'time': world.getSimulationTime()
        })
        
        # Let the agent learn from experience
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
    
    # Analyze memory
    print(f"Collected {len(experiences)} experiences")
    
    # Show some statistics
    positive_rewards = sum(1 for e in experiences if e['reward'] > 0)
    print(f"Positive rewards: {positive_rewards}/{len(experiences)} ({positive_rewards/len(experiences)*100:.1f}%)")
    
    # Get agent preferences
    preferences = agent.getPreferences()
    if preferences:
        print(f"Top 3 preferences:")
        for i, pref in enumerate(preferences[:3]):
            print(f"  {i+1}. {pref['action']}: {pref['strength']:.3f}")
    
    print()

def example_5_visualization():
    """Example 5: Visualization and monitoring"""
    print("=== Example 5: Visualization and Monitoring ===")
    
    config = pynlm.createDefaultConfig()
    config.set("visualization_enabled", True)
    config.set("visualization_update_rate", 10.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Running visualization demo...")
    print("(In a real application, this would show brain activity in real-time)")
    
    # Run for a few steps and collect data
    visualization_data = []
    for step in range(30):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Collect data for visualization
        data_point = {
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'neuron_count': brain.getTotalNeuronCount(),
            'spike_count': brain.getTotalSpikeCount(),
            'spike_per_step': brain.getTotalSpikeCount() / (step + 1),
            'development': brain.getDevelopmentalStage(),
            'reward': world.getReward()
        }
        
        visualization_data.append(data_point)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Show some statistics
    avg_firing = sum(d['firing_rate'] for d in visualization_data) / len(visualization_data)
    max_firing = max(d['firing_rate'] for d in visualization_data)
    
    print(f"Visualization data collected:")
    print(f"  Average firing rate: {avg_firing:.2f}")
    print(f"  Maximum firing rate: {max_firing:.2f}")
    print(f"  Steps recorded: {len(visualization_data)}")
    
    # Show trend
    if len(visualization_data) > 10:
        first_half = visualization_data[:len(visualization_data)//2]
        second_half = visualization_data[len(visualization_data)//2:]
        
        first_avg = sum(d['firing_rate'] for d in first_half) / len(first_half)
        second_avg = sum(d['firing_rate'] for d in second_half) / len(second_half)
        
        trend = "increasing" if second_avg > first_avg else "decreasing"
        print(f"  Firing rate trend: {trend} ({first_avg:.2f} → {second_avg:.2f})")
    
    print()

def example_6_advanced_config():
    """Example 6: Advanced configuration"""
    print("=== Example 6: Advanced Configuration ===")
    
    # Create configuration from scratch
    config = pynlm.createDefaultConfig()
    
    # Basic settings
    config.set("neuron_count", 2000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.15)
    
    # Plasticity settings
    config.set("stdp_ltp_weight", 0.03)
    config.set("stdp_ltd_weight", 0.025)
    config.set("stdp_time_constant", 25.0)
    config.set("plasticity_learning_rate", 0.05)
    
    # Development settings
    config.set("development_synaptogenesis_rate", 0.002)
    config.set("development_pruning_rate", 0.0002)
    
    # Neuromodulation
    config.set("dopamine_baseline", 0.2)
    config.set("novelty_baseline", 0.1)
    config.set("curiosity_baseline", 0.3)
    
    # Environment
    config.set("environment_name", "CustomWorld")
    config.set("environment_width", 25)
    config.set("environment_height", 25)
    
    # Logging
    config.set("log_level", "DEBUG")
    config.set("log_to_file", True)
    config.set("log_filename", "nlm_advanced.log")
    
    # Simulation
    config.set("max_simulation_steps", 50000)
    config.set("simulation_time_limit", 1000.0)
    
    print("Configuration created with:")
    print(f"  - {config.get('neuron_count')} neurons")
    print(f"  - {config.get('region_count')} regions")
    print(f"  - {config.get('connection_probability') * 100}% connection probability")
    print(f"  - Learning rates: LTP={config.get('stdp_ltp_weight')}, LTD={config.get('stdp_ltd_weight')}")
    print(f"  - Development: synaptogenesis={config.get('development_synaptogenesis_rate')}, pruning={config.get('development_pruning_rate')}")
    
    # Save configuration
    import tempfile
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        config_path = f.name
    
    # Note: saveToFile would save to JSON format, but our Config uses
    # the simple key=value format. For now, just show that the config is valid.
    print(f"Configuration summary: {config.summary()[:100]}...")
    
    print()

def example_7_error_handling():
    """Example 7: Error handling and validation"""
    print("=== Example 7: Error Handling ===")
    
    # Test invalid configurations
    config = pynlm.createDefaultConfig()
    
    # Try to get non-existent key
    value = config.get("nonexistent_key")
    print(f"Getting nonexistent key: {value if value else 'Not found (good!)'}")
    
    # Use safe getters
    safe_value = config.getSafeString("nonexistent_key", "default_value")
    print(f"Safe get for string: '{safe_value}'")
    
    safe_int = config.getSafeInt("nonexistent_int", 42)
    print(f"Safe get for int: {safe_int}")
    
    safe_double = config.getSafeDouble("nonexistent_double", 3.14)
    print(f"Safe get for double: {safe_double}")
    
    safe_bool = config.getSafeBool("nonexistent_bool", False)
    print(f"Safe get for bool: {safe_bool}")
    
    # Set and get values
    config.set("test_string", "hello world")
    config.set("test_int", 123)
    config.set("test_double", 3.14159)
    config.set("test_bool", True)
    
    print(f"Set and get test: {config.get('test_string')}")
    print(f"Set and get test (int): {config.get('test_int')}")
    print(f"Set and get test (double): {config.get('test_double')}")
    print(f"Set and get test (bool): {config.get('test_bool')}")
    
    print()

def example_8_brain_inspection():
    """Example 8: Brain inspection and debugging"""
    print("=== Example 8: Brain Inspection ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Brain inspection data:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"  Pending spike events: {brain.getPendingSpikeEventCount()}")
    
    # Get spike system (Phase 2)
    spike_system = brain.getSpikeSystem()
    if spike_system:
        print(f"  Spike system active: {spike_system}")
    
    print()

def calculate_max_consecutive(rewards):
    """Calculate maximum consecutive positive rewards"""
    max_consecutive = 0
    current = 0
    
    for reward in rewards:
        if reward > 0:
            current += 1
            max_consecutive = max(max_consecutive, current)
        else:
            current = 0
    
    return max_consecutive

def run_all_examples():
    """Run all examples"""
    print("=" * 60)
    print("NLM Python Examples - Complete Demonstration")
    print("=" * 60)
    print()
    
    examples = [
        example_1_basic_brain,
        example_2_brain_world_agent,
        example_3_learning_experiment,
        example_4_memory_system,
        example_5_visualization,
        example_6_advanced_config,
        example_7_error_handling,
        example_8_brain_inspection
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"ERROR in example {i}: {e}")
            import traceback
            traceback.print_exc()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)
    print()
    print("To run examples individually:")
    print("  # Import just the example you want")
    print("  from nlm_examples import example_1_basic_brain")
    print("  example_1_basic_brain()")
    print()
    print("For more details, see HOW_TO_USE.md or docs/ARCHITECTURE.md")

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "--all":
        run_all_examples()
    else:
        print("NLM Python Examples")
        print()
        print("Available examples:")
        print("  1. Basic Brain (example_1_basic_brain)")
        print("  2. Brain-World-Agent System (example_2_brain_world_agent)")
        print("  3. Advanced Learning Experiment (example_3_learning_experiment)")
        print("  4. Memory System (example_4_memory_system)")
        print("  5. Visualization (example_5_visualization)")
        print("  6. Advanced Configuration (example_6_advanced_config)")
        print("  7. Error Handling (example_7_error_handling)")
        print("  8. Brain Inspection (example_8_brain_inspection)")
        print()
        print("Run with --all to see all examples:")
        print("  python nlm_examples.py --all")

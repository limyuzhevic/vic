#!/usr/bin/env python3
"""
NLM Python Enhanced API Examples

This file contains comprehensive examples demonstrating the new enhanced Python API
features for NLM (Neural Learning Machine).

Examples include:
1. Quick setup patterns
2. Context manager usage
3. Configuration building
4. Performance monitoring
5. Error handling
6. Advanced agent patterns
"""

import pynlm
import sys
from pathlib import Path

def print_header(title):
    print(f"\n{'='*60}")
    print(f"{title}")
    print(f"{'='*60}")

def example_basic_setup():
    """Example 1: Basic quick setup using new convenience functions"""
    print_header("Example 1: Basic Quick Setup")
    
    # OLD WAY (verbose):
    # config = pynlm.createDefaultConfig()
    # brain = pynlm.createBrain(config)
    # brain.initialize()
    # world = pynlm.createSimpleWorld()
    # world.configure(10, 10, 8, 8)
    # world.reset()
    # agent = pynlm.createAgentBrain(brain)
    # agent.initialize(world)
    
    # NEW WAY (quick and clean):
    print("Creating brain, world, and agent in one call...")
    brain, world, agent = pynlm.create_agent_world_setup(
        width=10, 
        height=10,
        vision_width=8, 
        vision_height=8,
        enable_learning=True,
        enable_development=True,
        enable_curiosity=True
    )
    
    print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"✓ World configured: {world.getWidth()}x{world.getHeight()}")
    
    # Run a simple simulation
    print("\nRunning simulation...")
    for step in range(50):
        brain.step(step)
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print("✓ Basic setup completed successfully!")

def example_context_managers():
    """Example 2: Using context managers for automatic resource management"""
    print_header("Example 2: Context Manager Usage")
    
    # Using brain context manager
    print("Using BrainContextManager...")
    config = pynlm.create_research_config(20, 20)
    
    with pynlm.brain_context(config) as brain:
        brain.initialize()
        print(f"✓ Brain initialized with {brain.getTotalNeuronCount()} neurons")
        
        # Use brain with automatic cleanup
        for step in range(20):
            brain.step(step)
            if step == 0:
                print(f"  Initial firing rate: {brain.getAverageFiringRate():.2f} Hz")
        
        print("✓ Brain used within context manager")
        # Brain automatically cleaned up when exiting context
    
    print("✓ Context manager example completed")
    
    # Using agent context manager
    print("\nUsing AgentContextManager...")
    brain, world, agent = pynlm.create_agent_world_setup(
        width=15, height=15, vision_width=10, vision_height=10
    )
    
    with pynlm.agent_context(brain, world) as agent:
        agent.initialize()
        print("✓ Agent initialized within context manager")
        
        # Run a complete episode
        for step in range(30):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 10 == 0:
                print(f"  Step {step}: Action {action.getName()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.2f}")
        
        print("✓ Episode completed within context manager")
        # Agent automatically cleaned up
    
    print("✓ Agent context manager example completed")

def example_configuration_building():
    """Example 3: Building configurations with fluent API"""
    print_header("Example 3: Configuration Building")
    
    print("Creating different types of configurations:")
    
    # Research configuration
    print("\n1. Research Configuration:")
    research_config = pynlm.create_research_config(width=30, height=30)
    print(f"   - Neurons: {research_config.has('brain.neuron_count')}")
    print(f"   - Vision: {research_config.has('sensory.vision.width')}x{research_config.has('sensory.vision.height')}")
    print(f"   - Plasticity: {'enabled' if research_config.has('plasticity.stdp.enable') else 'disabled'}")
    
    # Reinforcement learning configuration
    print("\n2. Reinforcement Learning Configuration:")
    rl_config = pynlm.create_rl_config(learning_rate=0.001)
    print(f"   - Learning rate: {rl_config.has('plasticity.stdp.learning_rate')}")
    print(f"   - Curiosity enabled: {rl_config.has('neuromod.curiosity.enable')}")
    print(f"   - World size: {rl_config.has('world.width')}x{rl_config.has('world.height')}")
    
    # Developmental configuration
    print("\n3. Developmental Configuration:")
    dev_config = pynlm.create_developmental_config(lifespan=120.0)
    print(f"   - Lifespan: {dev_config.has('development.age_rate')}x normal")
    print(f"   - Memory: {'enabled' if dev_config.has('memory.working.enable') else 'disabled'}")
    print(f"   - Prediction: {'enabled' if dev_config.has('prediction.enabled') else 'disabled'}")
    
    # Using ConfigBuilder with fluent API
    print("\n4. Fluent API Example:")
    from pynlm import ConfigBuilder
    config = (ConfigBuilder()
              .with_neural_params(neuron_count=2000, gain=1.5, threshold=-40.0)
              .with_plasticity(enable_stdp=True, enable_hebbian=True, 
                               enable_structural=True, stdp_learning_rate=0.002)
              .with_world(width=25, height=25, vision_width=12, vision_height=12)
              .with_agent(enable_learning=True, enable_planning=True)
              .build())
    
    print(f"   - Custom brain with {config.getOr('brain.neuron_count', 0)} neurons")
    print(f"   - Large world: {config.getOr('world.width', 0)}x{config.getOr('world.height', 0)}")
    print(f"   - Complex plasticity settings configured")
    
    print("\n✓ Configuration building examples completed")

def example_performance_monitoring():
    """Example 4: Performance monitoring and statistics"""
    print_header("Example 4: Performance Monitoring")
    
    print("Creating performance monitor...")
    perf = pynlm.PerformanceMonitor()
    perf.start()
    
    # Create and run a brain
    brain, world, agent = pynlm.create_agent_world_setup(
        width=20, height=20, vision_width=8, vision_height=8
    )
    
    print("Running simulation with performance tracking...")
    for step in range(100):
        brain.step(step)
        if step % 25 == 0:
            print(f"  Step {step}/100")
    
    # Get performance statistics
    stats = perf.stop()
    
    print("\nPerformance Results:")
    print(f"  Wall Clock Time: {stats.wall_clock_time:.2f} seconds")
    print(f"  Steps Completed: {stats.steps_completed}")
    print(f"  Average Steps/sec: {stats.avg_firing_rate:.1f}")
    print(f"  Memory Usage: {stats.memory_usage_mb:.1f} MB")
    print(f"  CPU Usage: {stats.cpu_usage_percent:.1f}%")
    print(f"  Active Neurons: {stats.neurons_active}")
    
    if stats.timestep_durations:
        avg_timestep = sum(stats.timestep_durations) / len(stats.timestep_durations)
        print(f"  Average Step Duration: {avg_timestep:.6f} seconds")
    
    print("\nPerformance monitoring example completed")

def example_error_handling():
    """Example 5: Pythonic error handling"""
    print_header("Example 5: Pythonic Error Handling")
    
    print("Testing error handling with recovery:")
    
    # Test configuration error handling
    print("\n1. Configuration Error Handling:")
    try:
        config = pynlm.create_research_config(10, 10)
        # Try to access non-existent configuration value
        value = config.get("nonexistent.key")
        print(f"   Retrieved value: {value}")
    except Exception as e:
        print(f"   ✓ Caught error: {type(e).__name__}: {e}")
    
    # Test brain initialization error
    print("\n2. Brain Initialization Error Handling:")
    try:
        # This should work fine
        brain, world, agent = pynlm.create_agent_world_setup(10, 10)
        brain.initialize()
        print("   ✓ Brain initialized successfully")
        
        # Test error recovery
        print("\n3. Error Recovery Test:")
        if pynlm.attempt_recovery(brain, 3):
            print("   ✓ Recovery attempt successful")
        else:
            print("   ✗ Recovery failed (this shouldn't happen in this test)")
            
    except pynlm.BrainInitializationError as e:
        print(f"   ✓ Caught brain initialization error: {e}")
    except Exception as e:
        print(f"   ✓ Caught error: {type(e).__name__}: {e}")
    
    print("\nError handling example completed")

def example_advanced_agent():
    """Example 6: Advanced agent patterns with new features"""
    print_header("Example 6: Advanced Agent Patterns")
    
    print("Creating advanced agent with learning capabilities...")
    
    # Setup agent world
    brain, world, agent = pynlm.create_agent_world_setup(
        width=25, height=25, vision_width=12, vision_height=12,
        enable_learning=True, enable_development=True, enable_curiosity=True
    )
    
    # Enable advanced features
    agent.enableStructuralPlasticity(True)
    agent.enableRewardModulation(True)
    
    print("Running learning episode with advanced features...")
    
    # Run learning episode
    episode_steps = 200
    for step in range(episode_steps):
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Agent makes decision
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Agent learns from experience
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Development
        agent.updateDevelopment(0.1)
        
        # Log progress
        if step % 40 == 0:
            print(f"  Step {step}/{episode_steps}:")
            print(f"    Action: {action.getName()}")
            print(f"    Reward: {reward:.3f}")
            print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"    Prediction Error: {agent.getPredictionError():.3f}")
            print(f"    Developmental Stage: {agent.getDevelopmentalStage()}")
            print(f"    Neurons Firing: {brain.getFiringNeuronCount()}")
    
    print(f"\n✓ Advanced agent episode completed!")
    print(f"  Final dopamine level: {agent.getNeuromodulationLevel():.3f}")
    print(f"  Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Total development time: {agent.getDevelopmentalStage()}")

def example_batch_processing():
    """Example 7: Batch processing with simulation helpers"""
    print_header("Example 7: Batch Processing")
    
    print("Creating multiple simulation runs...")
    
    # Create a batch processor
    brain, world, agent = pynlm.create_agent_world_setup(10, 10, 6, 6)
    
    # Run multiple episodes
    episodes = 3
    steps_per_episode = 50
    
    print(f"Running {episodes} episodes of {steps_per_episode} steps each...")
    
    for episode in range(episodes):
        print(f"\nEpisode {episode + 1}/{episodes}:")
        
        world.reset()
        agent.reset()
        
        episode_reward = 0.0
        max_firing = 0
        
        for step in range(steps_per_episode):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            episode_reward += reward
            max_firing = max(max_firing, brain.getFiringNeuronCount())
        
        print(f"  Total reward: {episode_reward:.2f}")
        print(f"  Average reward per step: {episode_reward/steps_per_episode:.3f}")
        print(f"  Max neurons firing: {max_firing}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    print(f"\n✓ Batch processing completed successfully")

def example_utility_functions():
    """Example 8: Using utility functions"""
    print_header("Example 8: Utility Functions")
    
    from pynlm import Utils
    
    print("Testing utility functions:")
    
    # String utilities
    text = "  Hello World  "
    trimmed = Utils.trim(text)
    lowered = Utils.to_lower(trimmed)
    formatted = Utils.format_duration(125.7)
    print(f"1. String utilities:")
    print(f"   Original: '{text}'")
    print(f"   Trimmed: '{trimmed}'")
    print(f"   Lowered: '{lowered}'")
    print(f"   Duration format: {formatted}")
    
    # Math utilities
    clamped = Utils.clamp(15.0, 0.0, 10.0)
    linear = Utils.lerp(0.0, 1.0, 0.5)
    sigmoid = Utils.sigmoid(2.0)
    print(f"\n2. Math utilities:")
    print(f"   Clamp(15.0, 0.0, 10.0): {clamped}")
    print(f"   Lerp(0.0, 1.0, 0.5): {linear}")
    print(f"   Sigmoid(2.0): {sigmoid:.3f}")
    
    # File utilities
    temp_file = Utils.get_temp_filename("nlm_test_")
    print(f"\n3. File utilities:")
    print(f"   Generated temp file: {temp_file}")
    
    # Type conversion utilities
    print(f"\n4. Type conversion utilities:")
    print(f"   NeuronType 'Excitatory' -> {Utils.neuron_type_to_string(0)}")  # 0 is Excitatory
    print(f"   ActionType 'MoveForward' -> {Utils.action_type_to_string(0)}")  # 0 is MoveForward
    
    print(f"\n✓ Utility functions example completed")

def example_api_info():
    """Example 9: Getting API information"""
    print_header("Example 9: API Information")
    
    print("Getting enhanced API information...")
    
    # Get version info
    version = pynlm.version()
    print(f"Version: {version}")
    
    # Get API info dictionary
    info = pynlm.get_api_info()
    print(f"\nAPI Features:")
    for feature in info["features"]:
        print(f"  ✓ {feature}")
    
    print(f"\n✓ API information example completed")

def run_all_examples():
    """Run all examples in sequence"""
    print("Running NLM Python Enhanced API Examples")
    print("=" * 60)
    
    examples = [
        ("Basic Setup", example_basic_setup),
        ("Context Managers", example_context_managers),
        ("Configuration Building", example_configuration_building),
        ("Performance Monitoring", example_performance_monitoring),
        ("Error Handling", example_error_handling),
        ("Advanced Agent", example_advanced_agent),
        ("Batch Processing", example_batch_processing),
        ("Utility Functions", example_utility_functions),
        ("API Information", example_api_info),
    ]
    
    completed = 0
    total = len(examples)
    
    for name, example_func in examples:
        try:
            print(f"\n{'='*60}")
            print(f"Running: {name}")
            print(f"{'='*60}")
            
            example_func()
            completed += 1
            print(f"\n✅ {name} completed successfully")
            
        except Exception as e:
            print(f"\n❌ Error in {name}: {e}")
            import traceback
            traceback.print_exc()
    
    print(f"\n{'='*60}")
    print(f"Examples Summary: {completed}/{total} completed successfully")
    print(f"{'='*60}")

if __name__ == "__main__":
    run_all_examples()
#!/usr/bin/env python3

"""
Advanced NLM Usage Examples and Tutorials

This script provides comprehensive examples for using the NLM framework,
ranging from beginner to advanced usage patterns.
"""

import sys
import os
import time
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Error: pynlm module not available. Please build the Python bindings first.")


def example_basic_usage():
    """Example 1: Basic brain usage - the simplest possible example."""
    print("=== Example 1: Basic Brain Usage ===")
    
    # Create a brain with default configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run for a few steps
    for i in range(10):
        brain.step(i)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print()


def example_world_simulation():
    """Example 2: Brain in a world - sensory-motor loop."""
    print("=== Example 2: World Simulation ===")
    
    # Setup brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Setup world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(50):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Simulation completed!")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Agent developmental stage: {agent.getDevelopmentalStage()}")
    print()


def example_advanced_agent():
    """Example 3: Advanced agent with learning capabilities."""
    print("=== Example 3: Advanced Learning Agent ===")
    
    # Create brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("excitation_rate", 0.7)
    config.set("inhibition_rate", 0.3)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup world with obstacles
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Place some obstacles
    for x in range(10, 20):
        for y in range(10, 20):
            world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, 0.0, 0.8))
    
    # Create agent with learning enabled
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Track agent performance
    step_stats = []
    
    # Run training simulation
    for step in range(200):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward based on action success
        reward = world.getAgentBody().health * 0.1
        predicted = agent.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        
        if step % 20 == 0:
            step_stats.append({
                'step': step,
                'firing': brain.getFiringNeuronCount(),
                'spikes': brain.getTotalSpikeCount(),
                'reward': reward,
                'prediction_error': predicted,
                'development_stage': agent.getDevelopmentalStage()
            })
    
    print(f"Training completed over {len(step_stats)} observation points")
    
    # Print performance summary
    avg_firing = sum(s['firing'] for s in step_stats) / len(step_stats)
    avg_reward = sum(s['reward'] for s in step_stats) / len(step_stats)
    final_stage = step_stats[-1]['development_stage']
    
    print(f"Average firing rate: {avg_firing:.1f} neurons")
    print(f"Average reward: {avg_reward:.3f}")
    print(f"Final developmental stage: {final_stage}")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print()


def example_memory_systems():
    """Example 4: Using NLM's memory systems."""
    print("=== Example 4: Memory Systems ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # The brain has integrated memory systems:
    # - Working memory (persistent activity)
    # - Episodic memory (experience storage)
    # - Associative memory (pattern completion)
    
    # Create a sequence of sensory inputs to demonstrate episodic memory
    print("Demonstrating episodic memory...")
    
    # Simulate experiencing a sequence
    sequence = [
        pynlm.Vision([0.1, 0.2, 0.3, 0.4, 0.5] * 8),  # 5x8 vision
        pynlm.InternalSignals([0.5]),                  # Internal signal
        pynlm.Vision([0.6, 0.7, 0.8, 0.9, 0.1] * 8),  # Different vision
    ]
    
    for i, input_data in enumerate(sequence):
        brain.step(i)
        if hasattr(input_data, 'setData'):
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 0.5)
        
        print(f"  Step {i}: Processed sensory input")
    
    print("Episodic memory systems are actively maintaining experience patterns")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print()


def example_prediction_system():
    """Example 5: Using NLM's prediction system."""
    print("=== Example 5: Prediction System ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Enable prediction system through agent
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable prediction-related features
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    print("Prediction system features:")
    print(f"  Curiosity enabled: {agent.isCuriosityEnabled()}")
    print(f"  Reward modulation enabled: {agent.isRewardModulationEnabled()}")
    print(f"  Initial prediction error: {agent.getPredictionError():.3f}")
    
    # Run simulation to demonstrate prediction learning
    for step in range(30):
        world.update(0.1)
        
        # Simulate unexpected outcome to generate prediction error
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        # Apply random reward/error to demonstrate prediction learning
        reward = 1.0 if step % 7 == 0 else 0.0  # Occasional rewards
        predicted = 0.5  # Expected reward
        agent.applyRewardModulation(reward, predicted)
        
        if step % 10 == 0:
            print(f"  Step {step}: Prediction error = {agent.getPredictionError():.3f}")
    
    print(f"Final prediction error: {agent.getPredictionError():.3f}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print()


def example_development_process():
    """Example 6: Brain development and maturation."""
    print("=== Example 6: Brain Development ===")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Setup for development demonstration
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    
    print("Brain development stages:")
    
    # Simulate development over time
    development_stages = []
    
    for step in range(100):
        world.update(0.1)
        
        if step % 10 == 0:
            current_stage = agent.getDevelopmentalStage()
            development_stages.append(current_stage)
            print(f"  Step {step}: {current_stage}")
        
        # Process environment
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Take action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Development completed: {len(set(development_stages))} stages reached")
    print(f"Final stage: {agent.getDevelopmentalStage()}")
    print()


def example_performance_benchmark():
    """Example 7: Performance benchmarking."""
    print("=== Example 7: Performance Benchmark ===")
    
    import time
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test different brain sizes
    sizes = [100, 500, 1000]
    
    for size in sizes:
        print(f"\nBenchmarking {size} neurons...")
        
        # Create brain with specific size
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", size)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run benchmark
        steps = 50
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
        
        elapsed = time.time() - start_time
        steps_per_sec = steps / elapsed
        
        print(f"  Steps/sec: {steps_per_sec:.1f}")
        print(f"  Neurons/sec: {(size * steps_per_sec):.0f}")
        print(f"  Average firing: {brain.getAverageFiringRate():.2f}")
    
    print()


def example_configuration():
    """Example 8: Advanced configuration options."""
    print("=== Example 8: Advanced Configuration ===")
    
    config = pynlm.createDefaultConfig()
    
    # Set various configuration parameters
    config_params = {
        "neuron_count": 1500,
        "excitation_rate": 0.6,
        "inhibition_rate": 0.4,
        "plasticity_enabled": True,
        "development_enabled": True,
        "novelty_threshold": 0.3,
        "curiosity_factor": 0.5,
        "reward_scale": 1.0,
        "learning_rate": 0.01
    }
    
    for key, value in config_params.items():
        config.set(key, value)
    
    print("Configuration summary:")
    print(config.summary())
    print()
    
    # Create brain with custom config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print()


def example_agent_comparison():
    """Example 9: Comparing different agent configurations."""
    print("=== Example 9: Agent Configuration Comparison ===")
    
    # Create different agent configurations
    agent_configs = [
        {"name": "Basic", "reward": False, "curiosity": False, "development": False},
        {"name": "Reward-Learning", "reward": True, "curiosity": False, "development": False},
        {"name": "Curiosity-Driven", "reward": False, "curiosity": True, "development": False},
        {"name": "Full Learning", "reward": True, "curiosity": True, "development": True},
    ]
    
    results = {}
    
    for config in agent_configs:
        print(f"\nTesting {config['name']} Agent...")
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Apply configuration
        agent.enableRewardModulation(config['reward'])
        agent.enableCuriosity(config['curiosity'])
        agent.enableDevelopment(config['development'])
        
        # Run simulation
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record results
        results[config['name']] = {
            'firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'development': agent.getDevelopmentalStage()
        }
        
        print(f"  Firing rate: {results[config['name']]['firing_rate']:.2f}")
        print(f"  Curiosity: {results[config['name']]['curiosity']:.3f}")
    
    print("\n=== Comparison Summary ===")
    print("Configuration | Firing Rate | Curiosity | Novelty")
    print("-------------|-------------|-----------|---------")
    for name, result in results.items():
        print(f"{name:13} | {result['firing_rate']:11.2f} | {result['curiosity']:9.3f} | {result['novelty']:6.3f}")
    
    print()


def run_all_examples():
    """Run all examples sequentially."""
    print("NLM Advanced Examples Suite")
    print("=" * 50)
    print()
    
    examples = [
        example_basic_usage,
        example_world_simulation,
        example_advanced_agent,
        example_memory_systems,
        example_prediction_system,
        example_development_process,
        example_performance_benchmark,
        example_configuration,
        example_agent_comparison,
    ]
    
    for i, example_func in enumerate(examples, 1):
        try:
            example_func()
        except Exception as e:
            print(f"Error in example {i}: {e}")
            import traceback
            traceback.print_exc()
        print("-" * 50)
    
    print("All examples completed!")


if __name__ == "__main__":
    if not PYNLM_AVAILABLE:
        sys.exit(1)
    
    if len(sys.argv) > 1:
        # Run specific example
        example_name = sys.argv[1]
        example_map = {
            'basic': example_basic_usage,
            'world': example_world_simulation,
            'advanced': example_advanced_agent,
            'memory': example_memory_systems,
            'prediction': example_prediction_system,
            'development': example_development_process,
            'benchmark': example_performance_benchmark,
            'config': example_configuration,
            'comparison': example_agent_comparison,
            'all': run_all_examples,
        }
        
        if example_name in example_map:
            example_map[example_name]()
        else:
            print(f"Unknown example: {example_name}")
            print("Available examples: basic, world, advanced, memory, prediction, development, benchmark, config, comparison, all")
    else:
        run_all_examples()

# NLM Python Examples

import sys
from typing import Dict, Any, Optional
from .exceptions import NLMRuntimeError, NLMConfigError

def run_simple_brain_example():
    """
    Example 1: Simplest Brain (from easy_usage.md)
    Just create a brain and let it think!
    """
    try:
        import pynlm
        
        # One line to create a virtual brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        
        # Initialize it
        brain.initialize()
        
        # Make it active
        brain.step(0)
        
        print(f"Your brain has {brain.getTotalNeuronCount()} neurons!")
        
        return brain
    except Exception as e:
        print(f"Error in simple brain example: {e}")
        return None

def run_agent_world_example():
    """
    Example 2: Brain Watching a World (from easy_usage.md)
    Create a brain that interacts with a world environment
    """
    try:
        import pynlm
        
        # Setup
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Watch the world for 30 steps
        for i in range(30):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(i)
            
        print("Watched world for 30 steps")
        print(f"Firing rate: {brain.getAverageFiringRate()}")
        
        return brain, world, agent
    except Exception as e:
        print(f"Error in agent world example: {e}")
        return None, None, None

def run_complete_agent_example():
    """
    Example 3: Complete Agent with Learning (from easy_usage.md)
    Full example with reward modulation and curiosity
    """
    try:
        import pynlm
        
        # Setup
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
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
        
        print("Agent simulation complete!")
        
        # Get performance metrics
        agent_stats = agent.getPerformanceMetrics()
        brain_stats = brain.getStatistics()
        
        print(f"Neuromodulation level: {agent_stats['neuromodulation']}")
        print(f"Curiosity level: {agent_stats['curiosity']}")
        print(f"Total neurons: {brain_stats['total_neurons']}")
        
        return brain, world, agent
    except Exception as e:
        print(f"Error in complete agent example: {e}")
        return None, None, None

def run_benchmark_example():
    """
    Example 4: Performance Benchmark
    Benchmark different brain configurations
    """
    try:
        import pynlm
        
        print("Running performance benchmarks...")
        
        # Small brain benchmark
        small_config = pynlm.createSmallBrainConfig()
        small_brain = pynlm.createBrain(small_config)
        
        small_result = pynlm.benchmarkBrain(small_brain, 100)
        print(f"Small brain (100 neurons): {small_result['steps_per_second']:.2f} steps/sec")
        
        # Medium brain benchmark
        medium_config = pynlm.createSimulationConfig(neuron_count=1000)
        medium_brain = pynlm.createBrain(medium_config)
        
        medium_result = pynlm.benchmarkBrain(medium_brain, 50)
        print(f"Medium brain (1000 neurons): {medium_result['steps_per_second']:.2f} steps/sec")
        
        # Large brain benchmark
        large_config = pynlm.createLargeBrainConfig(neuron_count=5000)
        large_brain = pynlm.createBrain(large_config)
        
        large_result = pynlm.benchmarkBrain(large_brain, 20)
        print(f"Large brain (5000 neurons): {large_result['steps_per_second']:.2f} steps/sec")
        
        return small_result, medium_result, large_result
    except Exception as e:
        print(f"Error in benchmark example: {e}")
        return None

def run_comparison_example():
    """
    Example 5: Brain Comparison
    Compare two different brain configurations
    """
    try:
        import pynlm
        
        print("Comparing brain configurations...")
        
        # Create two different configurations
        config1 = pynlm.createSmallBrainConfig()
        config2 = pynlm.createSimulationConfig(neuron_count=2000)
        
        brain1 = pynlm.createBrain(config1)
        brain2 = pynlm.createBrain(config2)
        
        # Compare them
        comparison = pynlm.compareBrains(brain1, brain2, 50)
        
        print(f"Brain 1 (small): {comparison['brain1_neuron_count']} neurons, "
              f"{comparison['brain1_spike_count']} spikes, "
              f"{comparison['brain1_firing_rate']:.2f} avg firing rate")
        
        print(f"Brain 2 (medium): {comparison['brain2_neuron_count']} neurons, "
              f"{comparison['brain2_spike_count']} spikes, "
              f"{comparison['brain2_firing_rate']:.2f} avg firing rate")
        
        return comparison
    except Exception as e:
        print(f"Error in comparison example: {e}")
        return None

def run_experiment_example():
    """
    Example 6: Complete Experiment
    Run a full agent experiment with world interaction
    """
    try:
        import pynlm
        
        print("Running complete experiment...")
        
        # Setup
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Configure world
        world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run experiment
        steps = 200
        result = pynlm.runStandardExperiment(brain, world, agent, steps)
        
        print(f"Experiment completed: {result['steps_completed']} steps")
        print(f"Total reward: {result['total_reward']:.2f}")
        print(f"Average reward: {result['avg_reward']:.3f}")
        print(f"Simulation time: {result['final_time']:.2f}")
        
        return result
    except Exception as e:
        print(f"Error in experiment example: {e}")
        return None

def run_advanced_example():
    """
    Example 7: Advanced Features
    Demonstrate advanced features like state management and configuration
    """
    try:
        import pynlm
        
        print("Running advanced features example...")
        
        # Create brain with custom configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        config.set("region_count", 3)
        config.set("connection_probability", 0.12)
        
        brain = pynlm.createBrain(config)
        
        # Validate configuration
        if pynlm.validateBrainConfiguration(config):
            print("Configuration validated successfully")
        
        # Initialize brain
        brain.initialize()
        
        # Run some steps
        pynlm.runControlledSimulation(brain, 10)
        
        # Get state
        state = pynlm.exportBrainState(brain)
        print(f"Brain state: {state}")
        
        # Save state
        if brain.save("/tmp/advanced_brain.bin"):
            print("Brain state saved successfully")
        
        # Load state (create new brain first)
        new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
        if new_brain.load("/tmp/advanced_brain.bin"):
            print("Brain state loaded successfully")
            state2 = pynlm.exportBrainState(new_brain)
            print(f"Loaded brain state: {state2}")
        
        return brain
    except Exception as e:
        print(f"Error in advanced example: {e}")
        return None

def run_all_examples():
    """
    Run all examples to demonstrate NLM functionality
    """
    print("=" * 60)
    print("NLM Python Bindings - Complete Examples")
    print("=" * 60)
    
    examples = [
        ("Simple Brain", run_simple_brain_example),
        ("Agent World Example", run_agent_world_example),
        ("Complete Agent Example", run_complete_agent_example),
        ("Benchmark Example", run_benchmark_example),
        ("Comparison Example", run_comparison_example),
        ("Experiment Example", run_experiment_example),
        ("Advanced Features Example", run_advanced_example),
    ]
    
    results = {}
    
    for name, example_func in examples:
        print(f"\n{'=' * 60}")
        print(f"Running: {name}")
        print('=' * 60)
        
        try:
            result = example_func()
            results[name] = result
            print(f"✓ {name} completed successfully")
        except Exception as e:
            print(f"✗ {name} failed: {e}")
            results[name] = None
    
    print(f"\n{'=' * 60}")
    print("All examples completed!")
    print(f"Successful examples: {sum(1 for r in results.values() if r is not None)}/{len(results)}")
    print('=' * 60)
    
    return results
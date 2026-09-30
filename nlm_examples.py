#!/usr/bin/env python3
"""
NLM (熙然) - Neural Learning Machine
Phase 6: Final Integration - Usage Examples

This file contains practical examples and tutorials for using the NLM framework.
It demonstrates the complete workflow from basic setup to advanced experimentation.

The examples are designed to be:
1. Easy to understand for beginners
2. Practical for everyday use
3. Extensible for advanced users
4. Self-contained and executable

Each example can be run independently to learn specific concepts.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

"""
EXAMPLE 1: Getting Started with NLM
---------------------------------
This is the simplest example showing how to create and run a basic neural simulation.
"""

def example_basic_brain():
    """
    Example 1.1: Create a simple brain and run a simulation
    
    This example shows the minimum steps needed to create a working NLM brain:
    1. Create configuration
    2. Create brain
    3. Initialize brain
    4. Run simulation
    5. Observe results
    """
    print("=== Example 1.1: Basic Brain Setup ===")
    
    try:
        import nlm
        
        # Step 1: Create configuration
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 500)
        config.set("region_count", 1)
        
        # Step 2: Create brain
        brain = nlm.createBrain(config)
        
        # Step 3: Initialize brain
        if not brain.initialize():
            print("✗ Failed to initialize brain")
            return False
            
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Step 4: Run simulation
        print("Running simulation...")
        for step in range(100):
            brain.step(step, step * 0.001)
            
        # Step 5: Observe results
        print(f"Results:")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Active neurons: {brain.getActiveNeuronCount()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

"""
EXAMPLE 2: Working with Multiple Regions
---------------------------------------
This example demonstrates how to create and work with multiple brain regions.
"""

def example_multiple_regions():
    """
    Example 2.1: Create brain with multiple regions
    
    Shows how to:
    - Create multiple regions with different types
    - Manage regions by ID
    - Access neurons in specific regions
    """
    print("\n=== Example 2.1: Multiple Regions ===")
    
    try:
        import nlm
        
        # Create configuration for multiple regions
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 3)  # 3 regions
        
        # Create brain
        brain = nlm.createBrain(config)
        
        # Initialize
        if not brain.initialize():
            print("✗ Failed to initialize brain")
            return False
            
        print(f"✓ Created brain with {brain.getRegionCount()} regions")
        
        # Access regions
        regions = brain.getRegions()
        print(f"Regions:")
        for i, region in enumerate(regions):
            neurons = region.getAllNeurons()
            print(f"  Region {i}: {len(neurons)} neurons")
            
            # Access populations in each region
            for pop in region.getPopulations():
                print(f"    Population ({pop.getNeuronType()}): {pop.getNeuronCount()} neurons")
        
        # Run a simple simulation
        print("\nRunning simulation...")
        for step in range(50):
            brain.step(step, step * 0.001)
            
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

"""
EXAMPLE 3: Memory Systems Integration
-------------------------------------
This example demonstrates how to use and work with NLM's memory systems.
"""

def example_memory_systems():
    """
    Example 3.1: Working with memory systems
    
    Shows how to:
    - Access memory systems
    - Store and retrieve information
    - Use working memory for active processing
    - Use episodic memory for experience storage
    """
    print("\n=== Example 3.1: Memory Systems ===")
    
    try:
        import nlm
        
        # Create brain with configuration
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 1)
        
        brain = nlm.createBrain(config)
        if not brain.initialize():
            print("✗ Failed to initialize brain")
            return False
            
        # Get memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        if not working_memory or not episodic_memory:
            print("✗ Memory systems not available")
            return False
            
        print("✓ Memory systems accessible")
        
        # Run some steps to generate activity
        for step in range(100):
            # Inject sensory input to generate activity
            brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 20.0f)
            brain.step(step, step * 0.001)
            
            # Store some information in working memory
            if working_memory.getActiveTraces() < 10:
                # Simulate storing some neural activity
                neurons = brain.getRegions()[0].getAllNeurons()
                if neurons:
                    active_neuron = neurons[0]
                    working_memory.storeToNeuron(active_neuron.getId(), 0.5f)
        
        print(f"Working memory traces: {working_memory.getActiveTraces()}")
        print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
        
        # Store an episode
        if episodic_memory.getEpisodeCount() < 5:
            # Create a simple episode
            episode = nlm.EpisodicMemoryItem()
            episode.timestamp = 100
            episode.reward = 0.5
            
            # Add active neurons to episode
            for neuron in brain.getRegions()[0].getAllNeurons():
                if neuron.isFiring():
                    episode.activeNeurons.append(neuron.getId())
            
            episodic_memory.storeEpisode(episode)
            print(f"✓ Stored episode (total: {episodic_memory.getEpisodeCount()})")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

"""
EXAMPLE 4: Neuromodulation and Learning
----------------------------------------
This example demonstrates neuromodulation effects and learning processes.
"""

def example_neuromodulation():
    """
    Example 4.1: Neuromodulation and learning
    
    Shows how to:
    - Apply neuromodulation signals
    - Use reward and prediction error
    - Implement curiosity-driven exploration
    - Modify synaptic weights based on learning
    """
    print("\n=== Example 4.1: Neuromodulation and Learning ===")
    
    try:
        import nlm
        
        # Create brain
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 500)
        
        brain = nlm.createBrain(config)
        if not brain.initialize():
            print("✗ Failed to initialize brain")
            return False
            
        # Create agent interface
        agent = nlm.createAgentBrain(brain)
        
        # Create simple world
        world = nlm.createSimpleWorld()
        world.configure(16, 16, 16, 16)
        
        print("Running learning simulation with neuromodulation...")
        
        # Run learning simulation
        for step in range(200):
            # Observe world
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step, step * 0.001)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, step * 0.001)
            
            # Get reward
            reward = world.getAgentBody().energy
            
            # Apply neuromodulation
            agent.applyRewardModulation(reward, 0.0f)
            
            # Update development
            agent.updateDevelopment(0.001)
            
            # Log progress
            if step % 50 == 0:
                dopamine = agent.getNeuromodulationLevel()
                curiosity = agent.getCuriosityLevel()
                print(f"  Step {step}: Dopamine={dopamine:.2f}, Curiosity={curiosity:.2f}, Reward={reward:.2f}")
        
        # Show final neuromodulation state
        print(f"\nFinal neuromodulation state:")
        print(f"  Dopamine level: {agent.getNeuromodulationLevel():.2f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.2f}")
        print(f"  Prediction error: {agent.getPredictionError():.2f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

"""
EXAMPLE 5: Advanced Experimentation
------------------------------------
This example shows advanced experimentation and analysis capabilities.
"""

def example_advanced_experimentation():
    """
    Example 5.1: Advanced experimentation
    
    Shows how to:
    - Run complex experiments
    - Analyze neural activity patterns
    - Use checkpoints and state management
    - Compare different configurations
    """
    print("\n=== Example 5.1: Advanced Experimentation ===")
    
    try:
        import nlm
        import os
        
        # Create brain for Experiment 1: Plasticity learning
        print("Experiment 1: Plasticity Learning")
        config1 = nlm.createDefaultConfig()
        config1.set("neuron_count", 800)
        config1.set("region_count", 2)
        
        brain1 = nlm.createBrain(config1)
        if not brain1.initialize():
            print("✗ Failed to initialize brain for Experiment 1")
            return False
            
        # Enable plasticity
        for region in brain1.getRegions():
            for syn in region.getSynapses():
                syn.enablePlasticity(True, True, True)
        
        # Run plasticity experiment
        for step in range(500):
            brain1.injectCurrentToNeurons(nlm.NeuronType.Sensory, 30.0f)
            brain1.step(step, step * 0.001)
            
            if step % 100 == 0:
                firing = brain1.getAverageFiringRate()
                print(f"  Step {step}: Firing rate = {firing:.2f} Hz")
        
        # Save state
        state_file = "experiment1_state.bin"
        if brain1.save(state_file):
            print(f"✓ Saved brain state to {state_file}")
        
        # Create brain for Experiment 2: STDP
        print("\nExperiment 2: STDP Verification")
        config2 = nlm.createDefaultConfig()
        config2.set("neuron_count", 300)
        
        brain2 = nlm.createBrain(config2)
        if not brain2.initialize():
            print("✗ Failed to initialize brain for Experiment 2")
            return False
            
        # Enable only STDP
        for region in brain2.getRegions():
            for syn in region.getSynapses():
                syn.enablePlasticity(False, True, False)
        
        # Create correlated activity for STDP
        region = brain2.getRegion(nlm.RegionId(1))
        if region and region.getTotalNeuronCount() >= 2:
            neurons = region.getAllNeurons()
            pre_neuron = neurons[0]
            post_neuron = neurons[1]
            
            for trial in range(100):
                pre_neuron.injectCurrent(60.0f)
                brain2.step(trial * 2, trial * 2 * 0.001)
                
                post_neuron.injectCurrent(60.0f)
                brain2.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
                
                if trial % 25 == 0:
                    # Check weight changes
                    synapses = region.getSynapses()
                    for syn in synapses:
                        if (syn.getSourceNeuron() == pre_neuron.getId() and
                            syn.getDestinationNeuron() == post_neuron.getId()):
                            print(f"  Step {trial}: Synapse weight = {syn.getWeight():.4f}")
        
        # Compare results
        print("\n=== Results Comparison ===")
        print("Experiment 1 (Plasticity):")
        print(f"  Final firing rate: {brain1.getAverageFiringRate():.2f} Hz")
        print(f"  Total spikes: {brain1.getTotalSpikeCount()}")
        
        print("Experiment 2 (STDP):")
        print(f"  Final firing rate: {brain2.getAverageFiringRate():.2f} Hz")
        print(f"  Total spikes: {brain2.getTotalSpikeCount()}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

"""
EXAMPLE 6: Real-World Application
----------------------------------
This example shows a realistic application scenario with an agent in a world.
"""

def example_real_world_application():
    """
    Example 6.1: Real-world application - Agent in world
    
    Shows how to create a complete agent-world system:
    - Agent with brain and perception
    - World with environment and objects
    - Interaction and learning
    - Goal-directed behavior
    """
    print("\n=== Example 6.1: Real-World Application ===")
    
    try:
        import nlm
        
        # Create agent's brain
        print("Creating agent brain...")
        agent_config = nlm.createDefaultConfig()
        agent_config.set("neuron_count", 1200)
        agent_config.set("region_count", 2)
        
        agent_brain = nlm.createBrain(agent_config)
        if not agent_brain.initialize():
            print("✗ Failed to initialize agent brain")
            return False
            
        # Create agent interface
        agent = nlm.createAgentBrain(agent_brain)
        
        # Create world
        print("Creating world...")
        world = nlm.createSimpleWorld()
        world.configure(32, 32, 32, 32)  # Large world for exploration
        
        # Set agent start position
        world.setAgentStart(16, 16)
        
        # Add some resources and hazards
        print("Adding environment objects...")
        
        # Add resource
        resource = nlm.WorldObject()
        resource.x = 25
        resource.y = 25
        resource.type = nlm.WorldObjectType.Resource
        resource.value = 10.0
        resource.radius = 1.0
        world.addObject(resource)
        
        # Add hazard
        hazard = nlm.WorldObject()
        hazard.x = 10
        hazard.y = 10
        hazard.type = nlm.WorldObjectType.Hazard
        hazard.value = -5.0
        hazard.radius = 1.5
        world.addObject(hazard)
        
        # Enable agent capabilities
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print("Starting agent exploration...")
        
        # Run simulation
        total_reward = 0.0
        exploration_steps = 500
        
        for step in range(exploration_steps):
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process with agent
            agent.processSensoryInput(percept)
            
            # Brain step
            agent_brain.step(step, step * 0.001)
            
            # Get action (agent decides what to do)
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            world.applyMotorCommand(action, step * 0.001)
            
            # Get reward
            reward = world.getAgentBody().energy
            total_reward += reward
            
            # Apply neuromodulation
            agent.applyRewardModulation(reward, 0.0f)
            
            # Update development
            agent.updateDevelopment(0.001)
            
            # Log progress
            if step % 100 == 0:
                body = world.getAgentBody()
                print(f"  Step {step}:")
                print(f"    Position: ({body.x:.1f}, {body.y:.1f})")
                print(f"    Energy: {body.energy:.1f}")
                print(f"    Age: {body.age:.1f}")
                print(f"    Action: {action}")
                print(f"    Reward: {reward:.2f}")
                print(f"    Curiosity: {agent.getCuriosityLevel():.2f}")
                print(f"    Dopamine: {agent.getNeuromodulationLevel():.2f}")
        
        # Show final results
        print(f"\n=== Final Results ===")
        print(f"Total exploration steps: {exploration_steps}")
        print(f"Average reward per step: {total_reward/exploration_steps:.3f}")
        print(f"Final agent energy: {world.getAgentBody().energy:.1f}")
        print(f"Final agent age: {world.getAgentBody().age:.1f}")
        print(f"Final curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"Final dopamine level: {agent.getNeuromodulationLevel():.2f}")
        
        # Save final state
        final_state = "final_agent_state.bin"
        if agent_brain.save(final_state):
            print(f"✓ Final state saved to {final_state}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

"""
EXAMPLE 7: Performance and Benchmarking
----------------------------------------
This example demonstrates performance monitoring and benchmarking capabilities.
"""

def example_performance_monitoring():
    """
    Example 7.1: Performance monitoring
    
    Shows how to:
    - Monitor simulation performance
    - Profile different configurations
    - Compare brain performance metrics
    - Identify bottlenecks
    """
    print("\n=== Example 7.1: Performance Monitoring ===")
    
    try:
        import nlm
        import time
        
        # Test different configurations
        configs = [
            {"name": "Small", "neurons": 200, "regions": 1},
            {"name": "Medium", "neurons": 800, "regions": 2},
            {"name": "Large", "neurons": 2000, "regions": 3},
        ]
        
        results = []
        
        for config_info in configs:
            print(f"\nTesting {config_info['name']} configuration...")
            
            config = nlm.createDefaultConfig()
            config.set("neuron_count", config_info["neurons"])
            config.set("region_count", config_info["regions"])
            
            brain = nlm.createBrain(config)
            if not brain.initialize():
                print(f"✗ Failed to initialize {config_info['name']} brain")
                continue
                
            # Measure performance
            start_time = time.time()
            
            steps = min(200, config_info["neurons"] // 10)  # Scale with brain size
            
            for step in range(steps):
                brain.step(step, step * 0.001)
                
            end_time = time.time()
            elapsed = end_time - start_time
            
            # Collect metrics
            result = {
                "name": config_info["name"],
                "neurons": config_info["neurons"],
                "regions": config_info["regions"],
                "steps": steps,
                "time_elapsed": elapsed,
                "time_per_step": elapsed / steps if steps > 0 else 0,
                "final_firing_rate": brain.getAverageFiringRate(),
                "total_spikes": brain.getTotalSpikeCount(),
                "active_neurons": brain.getActiveNeuronCount(),
            }
            
            results.append(result)
            
            print(f"  ✓ Completed {steps} steps in {elapsed:.2f}s")
            print(f"  Performance: {steps/elapsed:.1f} steps/second")
            print(f"  Final metrics: FR={result['final_firing_rate']:.2f}, Spikes={result['total_spikes']}")
        
        # Performance comparison
        print(f"\n=== Performance Comparison ===")
        print(f"{'Configuration':<12} {'Neurons':<8} {'Steps':<6} {'Time(s)':<8} {'Steps/s':<10} {'FR(Hz)':<8}")
        print("-" * 70)
        
        for result in results:
            print(f"{result['name']:<12} {result['neurons']:<8} {result['steps']:<6} "
                  f"{result['time_elapsed']:<8.2f} {result['steps']/result['time_elapsed']:<10.1f} "
                  f"{result['final_firing_rate']:<8.2f}")
        
        # Calculate scalability
        if len(results) >= 2:
            print(f"\n=== Scalability Analysis ===")
            small_result = results[0]
            large_result = results[-1]
            
            neurons_ratio = large_result["neurons"] / small_result["neurons"]
            time_ratio = large_result["time_elapsed"] / small_result["time_elapsed"]
            efficiency = neurons_ratio / time_ratio
            
            print(f"Neuron count ratio: {neurons_ratio:.1f}x")
            print(f"Time ratio: {time_ratio:.1f}x")
            print(f"Efficiency: {efficiency:.1f}x (higher is better)")
            
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

"""
EXAMPLE 8: Python API Usage
---------------------------
This example shows the complete Python API for NLM.
"""

def example_python_api():
    """
    Example 8.1: Complete Python API usage
    
    This is the recommended way to use NLM from Python.
    Demonstrates the full power of the Python bindings.
    """
    print("\n=== Example 8.1: Complete Python API Usage ===")
    
    try:
        import nlm
        
        print("Creating and using NLM with Python API...")
        
        # Step 1: Create configuration
        config = nlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 2)
        config.set("random_seed", 12345)
        
        # Step 2: Create brain
        brain = nlm.createBrain(config)
        
        # Step 3: Initialize
        print("Initializing brain...")
        if not brain.initialize():
            print("✗ Initialization failed")
            return False
            
        # Step 4: Use brain with Python syntax
        print("Using brain with Python syntax...")
        
        # Access regions like Python lists
        regions = brain.getRegions()
        print(f"Brain has {len(regions)} regions")
        
        # Get first region
        first_region = regions[0]
        neurons = first_region.getAllNeurons()
        print(f"First region has {len(neurons)} neurons")
        
        # Use brain in simulation loop
        print("Running simulation...")
        for step in range(100):
            # Python-style: inject current
            brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 15.0f)
            
            # Python-style: step brain
            brain.step(step, step * 0.001)
            
            # Check status every 20 steps
            if step % 20 == 0:
                print(f"  Step {step}: FR={brain.getAverageFiringRate():.2f}, "
                      f"Spikes={brain.getTotalSpikeCount()}")
        
        # Step 5: Use agent interface
        print("\nUsing agent interface...")
        
        # Create world
        world = nlm.createSimpleWorld()
        world.configure(16, 16, 16, 16)
        
        # Create agent
        agent = nlm.createAgentBrain(brain)
        
        # Enable all features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run with agent
        for step in range(50):
            # Get percept (like Python sensor reading)
            percept = world.getSensoryPercept()
            
            # Process in agent
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step, step * 0.001)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply in world
            world.applyMotorCommand(action, step * 0.001)
            
            # Get reward
            reward = world.getAgentBody().energy
            agent.applyRewardModulation(reward, 0.0f)
        
        # Show Python-style results
        print("\n=== Python Results ===")
        print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
        print(f"Total simulation steps: {100 + 50} = 150")
        print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Active neurons: {brain.getActiveNeuronCount()}")
        
        # Python-style error handling
        if brain.getTotalSpikeCount() > 0:
            print("✓ Successful simulation with neural activity")
        else:
            print("⚠ No spikes detected (might be normal)")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Run all examples"""
    print("NLM (熙然) - Neural Learning Machine")
    print("Phase 6: Final Integration - Usage Examples")
    print("=" * 60)
    
    examples = [
        ("Basic Brain Setup", example_basic_brain),
        ("Multiple Regions", example_multiple_regions),
        ("Memory Systems", example_memory_systems),
        ("Neuromodulation and Learning", example_neuromodulation),
        ("Advanced Experimentation", example_advanced_experimentation),
        ("Real-World Application", example_real_world_application),
        ("Performance Monitoring", example_performance_monitoring),
        ("Python API Usage", example_python_api),
    ]
    
    results = []
    
    for name, example_func in examples:
        print(f"\n{'='*60}")
        try:
            success = example_func()
            results.append((name, success))
        except Exception as e:
            print(f"✗ Example '{name}' failed with exception: {e}")
            results.append((name, False))
    
    # Summary
    print(f"\n{'='*60}")
    print("EXAMPLE SUMMARY")
    print("=" * 60)
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    for name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"{name:<40} {status}")
    
    print(f"\nTotal: {passed}/{total} examples passed")
    
    if passed == total:
        print("\n🎉 All examples completed successfully!")
        print("\nTo run specific examples:")
        print("  python nlm_examples.py [example_number]")
        print("\nExample numbers:")
        for i, (name, _) in enumerate(examples, 1):
            print(f"  {i}. {name}")
    else:
        print(f"\n⚠️  {total - passed} examples failed. Check output above.")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())

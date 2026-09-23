#!/usr/bin/env python3
"""
NLM Example Scripts - Common Use Cases

This directory contains example scripts that demonstrate different aspects of
using the NLM (Neural Learning Machine) Python bindings.

Each script shows a specific use case and can be run as a standalone program
or used as a template for your own projects.

Usage: python3 examples/example_name.py
"""

# =============================================================================
# NLM Python Bindings Examples
# =============================================================================

"""
Example 1: Basic Brain Initialization
======================================

This script demonstrates the most basic NLM usage pattern: creating a brain,
initializing it, and running a simple simulation.

Key concepts covered:
- Creating default configuration
- Brain initialization
- Basic simulation steps
- Statistics collection
"""
import pynlm
import sys

def basic_brain_example():
    """Create and run a basic brain simulation"""
    print("=== Basic Brain Example ===")
    
    # 1. Create brain with default configuration
    print("1. Creating brain with default configuration...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # 2. Initialize the brain
    print("2. Initializing brain...")
    if not brain.initialize():
        print("ERROR: Brain initialization failed!")
        return False
    print(f"   - Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # 3. Run simulation steps
    print("3. Running simulation steps...")
    num_steps = 100
    for step in range(num_steps):
        brain.step(step)
        
        # Print statistics every 20 steps
        if step % 20 == 0:
            print(f"   Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    # 4. Print final statistics
    print("4. Final statistics:")
    print(f"   - Total neurons: {brain.getTotalNeuronCount()}")
    print(f"   - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"   - Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"   - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"   - Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"   - Developmental stage: {brain.getDevelopmentalStage()}")
    
    return True

if __name__ == "__main__":
    success = basic_brain_example()
    sys.exit(0 if success else 1)

"""
Example 2: Agent in World Simulation
=====================================

This script demonstrates how to create an agent that interacts with a world,
processes sensory input, and produces motor actions.

Key concepts covered:
- Creating world environment
- Agent initialization and setup
- Sensory perception
- Motor command generation
- World interaction
"""
import pynlm
import sys

def agent_world_example():
    """Create an agent that interacts with a world"""
    print("=== Agent in World Example ===")
    
    # 1. Setup brain
    print("1. Setting up brain...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world environment
    print("2. Creating world environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10, 10)  # Start in middle
    world.reset()
    
    # 3. Create agent
    print("3. Creating agent...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning features
    print("4. Enabling learning features...")
    agent.enableAllFeatures()
    
    # 5. Create some objects in the world
    print("5. Creating world objects...")
    world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, 10))  # Food
    world.addObject(pynlm.WorldObject(15, 15, pynlm.WorldObjectType.Resource, 10))  # Food
    world.addObject(pynlm.WorldObject(10, 5, pynlm.WorldObjectType.Hazard, 5))  # Hazard
    
    # 6. Run simulation
    print("6. Running simulation...")
    num_steps = 200
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        if not percept:
            print(f"   Step {step}: No sensory input!")
            continue
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and apply it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print status every 50 steps
        if step % 50 == 0:
            agent_body = world.getAgentBody()
            print(f"   Step {step}: "
                  f"Position=({agent_body.x:.1f}, {agent_body.y:.1f}), "
                  f"Action={action}, "
                  f"Firing={brain.getFiringNeuronCount()}")
    
    # 7. Print final results
    print("7. Final results:")
    agent_body = world.getAgentBody()
    print(f"   - Agent final position: ({agent_body.x:.1f}, {agent_body.y:.1f})")
    print(f"   - Total steps: {num_steps}")
    print(f"   - Final brain stats: {brain.getStatistics()}")
    
    return True

if __name__ == "__main__":
    success = agent_world_example()
    sys.exit(0 if success else 1)

"""
Example 3: Learning and Development
====================================

This script demonstrates how brains can learn, develop over time, and change
their behavior based on experience.

Key concepts covered:
- Developmental stages
- Reward-based learning
- Curiosity-driven exploration
- Structural plasticity
- Long-term learning effects
"""
import pynlm
import sys
import time

def learning_development_example():
    """Demonstrate learning and development over time"""
    print("=== Learning and Development Example ===")
    
    # 1. Setup learning brain
    print("1. Setting up learning brain...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1500)
    config.set("region_count", 3)
    config.set("connection_probability", 0.15)
    config.set("enable_curiosity", True)
    config.set("enable_reward_modulation", True)
    config.set("enable_development", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.setAgentStart(7, 7)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllFeatures()
    
    # 2. Create learning environment
    print("2. Creating learning environment...")
    # Add resources that can be learned
    for x in range(3, 12, 3):
        for y in range(3, 12, 3):
            if (x != 7 or y != 7):  # Away from start
                world.addObject(pynlm.WorldObject(x, y, 
                                                 pynlm.WorldObjectType.Resource, 
                                                 5))
    
    # 3. Track learning progress
    print("3. Tracking learning progress...")
    learning_log = {
        "steps": [],
        "rewards": [],
        "firing_rates": [],
        "developmental_stages": [],
        "actions_taken": []
    }
    
    # 4. Run learning simulation
    print("4. Running learning simulation...")
    num_steps = 300
    for step in range(num_steps):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes and learns
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        action_result = world.getActionResult()
        
        # Apply reward-based learning
        agent.applyRewardModulation(action_result.reward, 
                                   agent.getExpectedReward())
        
        # Log learning progress
        learning_log["steps"].append(step)
        learning_log["rewards"].append(action_result.reward)
        learning_log["firing_rates"].append(brain.getAverageFiringRate())
        learning_log["developmental_stages"].append(brain.getDevelopmentalStage())
        learning_log["actions_taken"].append(action)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            avg_reward = sum(learning_log["rewards"]) / len(learning_log["rewards"])
            avg_firing = sum(learning_log["firing_rates"]) / len(learning_log["firing_rates"])
            print(f"   Step {step}: Avg reward={avg_reward:.2f}, "
                  f"Avg firing={avg_firing:.3f}, "
                  f"Stage={brain.getDevelopmentalStage()}")
    
    # 5. Analyze learning results
    print("5. Analyzing learning results...")
    final_stage = brain.getDevelopmentalStage()
    avg_reward = sum(learning_log["rewards"]) / len(learning_log["rewards"])
    max_reward = max(learning_log["rewards"])
    
    print(f"   - Developmental stages reached: {set(learning_log['developmental_stages'])}")
    print(f"   - Final developmental stage: {final_stage}")
    print(f"   - Average reward per step: {avg_reward:.3f}")
    print(f"   - Maximum reward: {max_reward:.3f}")
    
    return True

if __name__ == "__main__":
    success = learning_development_example()
    sys.exit(0 if success else 1)

"""
Example 4: Memory Systems Integration
======================================

This script demonstrates how NLM integrates multiple memory systems:
working memory, episodic memory, and associative memory.

Key concepts covered:
- Working memory (short-term storage)
- Episodic memory (experience recording)
- Associative memory (pattern associations)
- Memory consolidation
- Memory replay and reinforcement
"""
import pynlm
import sys

def memory_systems_example():
    """Demonstrate integrated memory systems"""
    print("=== Memory Systems Integration Example ===")
    
    # 1. Setup memory-capable brain
    print("1. Setting up memory-capable brain...")
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("region_count", 4)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Access memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.setAgentStart(12, 12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllFeatures()
    
    # 2. Create learning environment with landmarks
    print("2. Creating environment with landmarks...")
    landmarks = []
    for x in range(5, 20, 5):
        for y in range(5, 20, 5):
            landmark_id = len(landmarks)
            world.addObject(pynlm.WorldObject(x, y, 
                                             pynlm.WorldObjectType.Marker, 
                                             3))
            landmarks.append((x, y, landmark_id))
    
    # 3. Record experiences and form concepts
    print("3. Recording experiences and forming concepts...")
    num_experiences = 150
    for experience_id in range(num_experiences):
        # Update environment
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes and stores experiences
        brain.step(experience_id)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record this experience
        experience = {
            "experience_id": experience_id,
            "timestamp": world.getSimulationTime(),
            "location": (world.getAgentBody().x, world.getAgentBody().y),
            "action": action,
            "sensory_input": percept,
            "action_result": world.getActionResult()
        }
        
        # Store in episodic memory
        episodic_memory.record(experience)
        
        # Create concept associations
        if experience_id % 15 == 0:
            concepts = []
            
            # Location-based concept
            x, y = experience["location"]
            if 0 <= x <= 5 and 0 <= y <= 5:
                concepts.append("north_west_quadrant")
            elif 10 <= x <= 15 and 10 <= y <= 15:
                concepts.append("center")
            elif 20 <= x <= 25 and 20 <= y <= 25:
                concepts.append("south_east_quadrant")
            
            # Action-based concept
            if action == pynlm.MotorCommand.MoveForward:
                concepts.append("moving_forward")
            elif action == pynlm.MotorCommand.TurnLeft:
                concepts.append("turning_left")
            elif action == pynlm.MotorCommand.TurnRight:
                concepts.append("turning_right")
            
            # Learn associations
            for concept in concepts:
                associative_memory.associate(concept, 
                                            f"location_{x}_{y}", 
                                            0.8)
                associative_memory.associate(concept,
                                            f"action_{action}",
                                            0.7)
        
        # Memory consolidation every 50 experiences
        if experience_id % 50 == 0 and experience_id > 0:
            print(f"   Consolidating memories at experience {experience_id}...")
            episodic_memory.consolidate()
    
    # 4. Demonstrate memory capabilities
    print("4. Demonstrating memory capabilities...")
    
    # Working memory demonstration
    print("   - Working memory:")
    current_thoughts = working_memory.getActiveMemory()
    print(f"     Current thoughts: {current_thoughts}")
    
    # Episodic memory demonstration
    print("   - Episodic memory:")
    recent_experiences = episodic_memory.getRecentExperiences(5)
    print(f"     Recent experiences: {len(recent_experiences)}")
    
    # Associative memory demonstration
    print("   - Associative memory:")
    related_concepts = associative_memory.getRelatedConcepts("center")
    print(f"     Concepts related to 'center': {related_concepts}")
    
    # Pattern completion demonstration
    print("   - Pattern completion:")
    completed_pattern = associative_memory.completePattern("location_")
    print(f"     Completed pattern: {completed_pattern}")
    
    return True

if __name__ == "__main__":
    success = memory_systems_example()
    sys.exit(0 if success else 1)

"""
Example 5: Performance Benchmarking
====================================

This script demonstrates how to benchmark and optimize NLM brain performance.

Key concepts covered:
- Performance metrics collection
- Batch processing
- Optimization techniques
- Memory management
- Comparative analysis
"""
import pynlm
import sys
import time

def benchmark_example():
    """Demonstrate performance benchmarking"""
    print("=== Performance Benchmarking Example ===")
    
    # Test different brain configurations
    configs = [
        {"name": "Small", "neurons": 500, "regions": 2, "description": "Small brain"},
        {"name": "Medium", "neurons": 1000, "regions": 3, "description": "Medium brain"},
        {"name": "Large", "neurons": 2000, "regions": 4, "description": "Large brain"}
    ]
    
    results = []
    
    for config_info in configs:
        print(f"Benchmarking {config_info['name']} brain ({config_info['neurons']} neurons)...")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", config_info["neurons"])
        config.set("region_count", config_info["regions"])
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run benchmark
        start_time = time.time()
        steps_to_run = 1000
        
        for step in range(steps_to_run):
            brain.step(step)
        
        end_time = time.time()
        
        elapsed_ms = (end_time - start_time) * 1000
        steps_per_second = steps_to_run / (elapsed_ms / 1000)
        
        results.append({
            "name": config_info["name"],
            "neurons": config_info["neurons"],
            "time_ms": elapsed_ms,
            "steps_per_second": steps_per_second,
            "description": config_info["description"]
        })
        
        print(f"  {config_info['name']} brain: {steps_per_second:,.0f} steps/second")
    
    # Display comparison
    print("\nBenchmark Results:")
    print("=" * 60)
    
    table_format = "{:<15} {:>10} {:>15} {:>15}"
    print(table_format.format(
        "Configuration",
        "Neurons",
        "Steps/sec",
        "Time/Step (μs)"
    ))
    print("-" * 60)
    
    for result in results:
        time_per_step_us = result["time_ms"] / result["steps_per_second"] * 1000
        row_format = "{:<15} {:>10} {:>15,.0f} {:>15.1f}"
        print(row_format.format(
            result["name"],
            result["neurons"],
            result["steps_per_second"],
            time_per_step_us
        ))
    
    return True

if __name__ == "__main__":
    success = benchmark_example()
    sys.exit(0 if success else 1)

"""
Example 6: Advanced Configuration Management
===========================================

This script demonstrates advanced configuration management, including
configuration schemas, validation, and dynamic configuration changes.
"""
import pynlm
import sys
import json

def advanced_configuration_example():
    """Demonstrate advanced configuration management"""
    print("=== Advanced Configuration Management Example ===")
    
    # Create and validate configurations
    print("1. Creating configurations...")
    
    simple_config = pynlm.createDefaultConfig()
    simple_config.set("neuron_count", 1000)
    simple_config.set("region_count", 3)
    
    medium_config = pynlm.createDefaultConfig()
    medium_config.set("neuron_count", 2000)
    medium_config.set("region_count", 4)
    medium_config.set("connection_probability", 0.2)
    
    custom_config = pynlm.createDefaultConfig()
    custom_config.set("neuron_count", 1500)
    custom_config.set("region_count", 5)
    custom_config.set("enable_plasticity", False)
    
    configurations = {
        "simple": simple_config,
        "medium": medium_config,
        "custom": custom_config
    }
    
    print("2. Configuration comparison...")
    
    for name, config in configurations.items():
        neurons = config.getInt("neuron_count")
        regions = config.getInt("region_count")
        plasticity = "enabled" if config.getBool("enable_plasticity", True) else "disabled"
        connection_prob = config.getFloat("connection_probability", 0.15)
        
        print(f"   {name}: {neurons} neurons, {regions} regions, "
              f"plasticity={plasticity}, connection_prob={connection_prob}")
    
    print("\nAdvanced configuration management completed!")
    return True

if __name__ == "__main__":
    success = advanced_configuration_example()
    sys.exit(0 if success else 1)

"""
Example 7: Error Handling and Debugging
=======================================

This script demonstrates robust error handling and debugging techniques
for NLM applications.
"""
import pynlm
import sys

def error_handling_example():
    """Demonstrate error handling techniques"""
    print("=== Error Handling Example ===")
    
    # Demonstrate different error scenarios
    print("1. Creating brains with different configurations...")
    
    # Valid configuration
    print("   a) Valid configuration:")
    valid_config = pynlm.createDefaultConfig()
    valid_config.set("neuron_count", 1000)
    valid_config.set("region_count", 3)
    print(f"      Created brain with {valid_config.getInt('neuron_count')} neurons")
    
    # Configuration with warnings
    print("   b) Configuration with warnings:")
    warning_config = pynlm.createDefaultConfig()
    warning_config.set("neuron_count", 50)  # Very small brain
    warning_config.set("region_count", 10)  # Many regions for few neurons
    print(f"      Created brain with {warning_config.getInt('neuron_count')} neurons, "
          f"{warning_config.getInt('region_count')} regions")
    
    # Configuration that will cause initialization issues
    print("   c) Configuration with potential issues:")
    problematic_config = pynlm.createDefaultConfig()
    problematic_config.set("neuron_count", 1)
    problematic_config.set("region_count", 1)
    print(f"      Created minimal brain with {problematic_config.getInt('neuron_count')} neuron")
    
    print("\n2. Brain initialization and error handling...")
    
    # Try to create and initialize brains
    configs = [valid_config, warning_config, problematic_config]
    
    for i, config in enumerate(configs):
        try:
            brain = pynlm.createBrain(config)
            if brain.initialize():
                print(f"      Brain {i+1}: Created and initialized successfully")
                print(f"        Statistics: {brain.getTotalNeuronCount()} neurons, "
                      f"{brain.getFiringNeuronCount()} firing")
            else:
                print(f"      Brain {i+1}: Initialization failed")
        except Exception as e:
            print(f"      Brain {i+1}: Error - {str(e)}")
    
    print("\n3. Error handling demonstration completed!")
    return True

if __name__ == "__main__":
    success = error_handling_example()
    sys.exit(0 if success else 1)

print("\n" + "="*60)
print("NLM Example Scripts")
print("Available examples:")
print("1. basic_brain_example.py - Basic brain initialization and simulation")
print("2. agent_world_example.py - Agent interacting with world")
print("3. learning_development_example.py - Learning and development")
print("4. memory_systems_example.py - Memory systems integration")
print("5. benchmark_example.py - Performance benchmarking")
print("6. advanced_configuration_example.py - Advanced configuration management")
print("7. error_handling_example.py - Error handling and debugging")
print("="*60)

if __name__ == "__main__":
    print("Please specify which example to run:")
    print("python3 examples/example_name.py")
    sys.exit(1)

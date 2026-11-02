#!/usr/bin/env python3
"""
NLM Example Scripts and Templates

Comprehensive collection of example scripts, templates, and demonstrations
for common NLM use cases. These examples are designed to be educational,
practical, and easily adaptable for different user needs.
"""

import sys
import json
import time
from pathlib import Path
import pynlm

def create_simple_neural_simulation():
    """
    Example 1: Simple Neural Network Simulation
    
    Creates a basic neural network with sensory, internal, and motor neurons.
    Demonstrates fundamental NLM concepts: neurons, synapses, plasticity, and learning.
    """
    print("=== Example 1: Simple Neural Network Simulation ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    config.set("stdp_ltp_weight", 0.01)
    config.set("stdp_ltd_weight", 0.012)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"  - Sensory neurons: {brain.getTotalNeuronCount() // 4}")
    print(f"  - Internal neurons: {brain.getTotalNeuronCount() * 3 // 4}")
    
    # Run simulation
    print("\nRunning simulation...")
    for step in range(50):
        # Inject some sensory input
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
        
        brain.step(step)
        
        if step % 10 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    # Show results
    print(f"\nSimulation Results:")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  - E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  - Developmental stage: {brain.getDevelopmentalStage()}")
    
    return brain

def create_learning_simulation():
    """
    Example 2: Learning and Plasticity Simulation
    
    Demonstrates STDP (Spike-Timing Dependent Plasticity) and Hebbian learning.
    Shows how synaptic connections strengthen and weaken through activity.
    """
    print("\n=== Example 2: Learning and Plasticity Simulation ===")
    
    # Create configuration optimized for learning
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("region_count", 1)
    config.set("connection_probability", 0.05)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.018)
    config.set("stdp_tau", 30.0)
    
    # Create brain with agent interface for learning control
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(None)  # No world for this simple example
    
    print("Running learning simulation with STDP and Hebbian plasticity...")
    
    # Enable learning mechanisms
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    
    # Create a simple learning pattern: stimulate sensory neurons
    # to create correlated activity between pre- and post-synaptic neurons
    
    sensory_neurons = []
    for region in brain.getRegions():
        for pop in region.getPopulations():
            if pop.getNeuronType() == pynlm.NeuronType.Sensory:
                for neuron in pop.getNeurons():
                    sensory_neurons.append(neuron)
    
    print(f"  - Created {len(sensory_neurons)} sensory neurons for learning")
    
    # Learning phase: create correlated pre-post activity
    for epoch in range(5):
        print(f"\nLearning epoch {epoch + 1}/5:")
        
        # Pattern 1: Fire sensory neurons in sequence
        for i in range(len(sensory_neurons) // 4):
            sensory_neurons[i]->injectCurrent(40.0)
        
        for step in range(20):
            brain.step(step)
        
        # Pattern 2: Fire different sensory neurons
        for i in range(len(sensory_neurons) // 4, len(sensory_neurons) // 2):
            sensory_neurons[i]->injectCurrent(40.0)
        
        for step in range(20, 40):
            brain.step(step)
        
        # Check synaptic changes
        total_synapses = brain.getTotalSynapseCount()
        avg_weight = sum(syn->getWeight() for region in brain.getRegions() 
                       for syn in region.getSynapses()) / total_synapses if total_synapses > 0 else 0
        
        print(f"    Average synapse weight: {avg_weight:.4f}")
    
    print(f"\nLearning Results:")
    print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  - Final average weight: {avg_weight:.4f}")
    print(f"  - Learning effect: {'Significant' if abs(avg_weight - 0.25) > 0.1 else 'Minimal'}")
    
    return brain, agent

def create_behavioral_simulation():
    """
    Example 3: Behavioral Simulation with Agent and World
    
    Demonstrates a complete agent-world interaction system with sensory input,
    motor output, reward-based learning, and behavioral patterns.
    """
    print("\n=== Example 3: Behavioral Simulation with Agent and World ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300)
    config.set("region_count", 1)
    config.set("connection_probability", 0.08)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning and development
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Created agent-world system:")
    print(f"  - Environment: {world.getWidth()}x{world.getHeight()} grid")
    print(f"  - Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
    print(f"  - Agent learning enabled: Yes")
    
    # Simulation loop
    print("\nRunning behavioral simulation...")
    
    for episode in range(3):
        print(f"\nEpisode {episode + 1}/3:")
        
        # Reset world and agent
        world.reset()
        agent.reset()
        
        # Set agent in world
        world.setAgentStart(5.0, 5.0)
        
        # Run episode
        for step in range(100):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply motor command to world
            if action != pynlm.MotorCommand::Wait:
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Provide reward based on actions
            reward = 0.0
            if action == pynlm.MotorCommand.MoveForward:
                reward = 10.0
            elif action == pynlm.MotorCommand.Interact:
                reward = 5.0
            
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 20 == 0:
                print(f"    Step {step}: Position ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
                print(f"             Action: {action}")
                print(f"             Reward: {reward}")
                print(f"             Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"             Developmental stage: {agent.getDevelopmentalStage()}")
    
    print(f"\nBehavioral Simulation Results:")
    print(f"  - Total episodes completed: 3")
    print(f"  - Agent curiosity levels achieved: {agent.getCuriosityLevel():.3f}")
    print(f"  - Final developmental stage: {agent.getDevelopmentalStage()}")
    
    return brain, agent, world

def create_advanced_learning_example():
    """
    Example 4: Advanced Learning with Memory Systems
    
    Demonstrates integration of working memory, episodic memory, and concept formation.
    Shows how NLM can learn patterns and form concepts through experience.
    """
    print("\n=== Example 4: Advanced Learning with Memory Systems ===")
    
    # Create configuration with enhanced memory
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 400)
    config.set("region_count", 1)
    config.set("connection_probability", 0.06)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with memory systems
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(None)
    
    # Enable all learning mechanisms
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Setting up advanced learning with memory systems...")
    
    # Create concept formation and pattern recognition
    concept_formation = brain.getConceptFormation()
    if concept_formation:
        print("  - Concept formation system available")
        
        # Simulate concept learning through repeated exposure
        patterns = [
            ([0.1, 0.2, 0.3, 0.4, 0.5], "Pattern A"),
            ([0.6, 0.7, 0.8, 0.9, 1.0], "Pattern B"),
            ([0.2, 0.4, 0.6, 0.8, 1.0], "Pattern C")
        ]
        
        for epoch, (pattern, name) in enumerate(patterns):
            print(f"\nConcept learning epoch {epoch + 1}: {name}")
            
            # Create sensory input pattern
            vision = pattern * 10  # Scale up for vision input
            touch = [0.1] * 8
            internal = [0.0] * 4
            proprioception = [0.0] * 6
            
            # Create percept
            percept = pynlm.SensoryPercept()
            percept.setVision(vision)
            percept.setTouch(touch)
            percept.setInternal(internal)
            percept.setProprioception(proprioception)
            
            # Process pattern
            agent.processSensoryInput(percept)
            brain.step(epoch)
            
            # Apply learning
            reward = 1.0 if "A" in name or "B" in name else -1.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Get concept count
            if concept_formation:
                concept_count = concept_formation.getConceptCount()
                print(f"    Concept count: {concept_count}")
    
    # Check memory systems
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    associative_memory = brain.getAssociativeMemory()
    
    print(f"\nMemory System Status:")
    if working_memory:
        print(f"  - Working memory: Active (capacity: {working_memory.getCapacity()})")
    if episodic_memory:
        print(f"  - Episodic memory: Active (episodes: {episodic_memory.getEpisodeCount()})")
    if associative_memory:
        print(f"  - Associative memory: Active")
    
    print(f"  - Concept formation: {'Active' if concept_formation else 'Inactive'}")
    
    return brain, agent, concept_formation

def create_performance_benchmark():
    """
    Example 5: Performance Benchmark
    
    Demonstrates how to benchmark NLM performance with different configurations.
    Useful for testing scalability and optimizing performance.
    """
    print("\n=== Example 5: Performance Benchmark ===")
    
    # Test configurations
    test_configs = [
        {"name": "Small Network", "neurons": 100, "steps": 1000},
        {"name": "Medium Network", "neurons": 500, "steps": 500},
        {"name": "Large Network", "neurons": 1000, "steps": 200}
    ]
    
    print("Running performance benchmarks...")
    
    results = {}
    
    for test_config in test_configs:
        name = test_config["name"]
        neurons = test_config["neurons"]
        steps = test_config["steps"]
        
        print(f"\nBenchmarking {name} ({neurons} neurons, {steps} steps):")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", neurons)
        config.set("region_count", 1)
        config.set("connection_probability", 0.1)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for i in range(steps):
            # Minimal activity to simulate processing
            brain.step(i)
        
        end_time = time.time()
        total_time = end_time - start_time
        
        steps_per_second = steps / total_time
        
        result = {
            "name": name,
            "neurons": neurons,
            "steps": steps,
            "total_time": total_time,
            "steps_per_second": steps_per_second,
            "time_per_step_ms": total_time / steps * 1000
        }
        
        results[name] = result
        
        print(f"  - Time: {total_time:.3f}s")
        print(f"  - Performance: {steps_per_second:.1f} steps/s")
        print(f"  - Efficiency: {result['time_per_step_ms']:.3f}ms per step")
    
    print(f"\nBenchmark Summary:")
    for name, result in results.items():
        print(f"  - {name}: {result['steps_per_second']:.1f} steps/s "
              f"({result['time_per_step_ms']:.3f}ms per step)")
    
    return results

def create_configuration_template():
    """
    Example 6: Configuration Templates
    
    Shows how to create and save configuration templates for different use cases.
    """
    print("\n=== Example 6: Configuration Templates ===")
    
    # Template 1: Simple Exploration Agent
    exploration_config = {
        "name": "Exploration Agent",
        "description": "Agent designed for exploration and discovery",
        "parameters": {
            "neuron_count": 300,
            "region_count": 2,
            "connection_probability": 0.08,
            "stdp_ltp_weight": 0.015,
            "stdp_ltd_weight": 0.012,
            "stdp_tau": 25.0,
            "synaptogenesis_rate": 0.0005,
            "pruning_rate": 0.00005,
            "curiosity_enabled": True,
            "reward_modulation_enabled": True,
            "development_enabled": True,
            "novelty_enabled": True
        }
    }
    
    # Template 2: Stable Learning Agent
    stable_config = {
        "name": "Stable Learning Agent",
        "description": "Agent optimized for stable, reliable learning",
        "parameters": {
            "neuron_count": 500,
            "region_count": 1,
            "connection_probability": 0.05,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.01,
            "stdp_tau": 20.0,
            "synaptogenesis_rate": 0.0002,
            "pruning_rate": 0.00002,
            "curiosity_enabled": False,
            "reward_modulation_enabled": True,
            "development_enabled": False,
            "novelty_enabled": False
        }
    }
    
    # Template 3: Social Learning Agent
    social_config = {
        "name": "Social Learning Agent",
        "description": "Agent designed for social learning and imitation",
        "parameters": {
            "neuron_count": 600,
            "region_count": 2,
            "connection_probability": 0.1,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.018,
            "stdp_tau": 30.0,
            "synaptogenesis_rate": 0.0008,
            "pruning_rate": 0.00008,
            "curiosity_enabled": True,
            "reward_modulation_enabled": True,
            "development_enabled": True,
            "novelty_enabled": True,
            "social_learning_enabled": True
        }
    }
    
    print("Configuration Templates:")
    
    templates = [exploration_config, stable_config, social_config]
    
    for template in templates:
        print(f"\n  {template['name']}:")
        print(f"    Description: {template['description']}")
        print(f"    Key parameters:")
        for key, value in template["parameters"].items():
            print(f"      {key}: {value}")
    
    # Save templates to files
    templates_dir = Path("nlm_templates")
    templates_dir.mkdir(exist_ok=True)
    
    for template in templates:
        filename = templates_dir / f"{template['name'].replace(' ', '_').lower()}.json"
        
        with open(filename, 'w') as f:
            json.dump(template, f, indent=2)
        
        print(f"\n  Template saved to: {filename}")
    
    return templates

def create_complete_demo():
    """
    Example 7: Complete Demo
    
    A complete demonstration combining multiple NLM features: learning,
    memory, behavior, and adaptation in a unified example.
    """
    print("\n=== Example 7: Complete NLM Demo ===")
    
    print("This example demonstrates a complete NLM system with:")
    print("  - Neural processing with spiking neurons")
    print("  - Sensory and motor capabilities")
    print("  - Learning through reinforcement and curiosity")
    print("  - Memory systems for experience storage")
    print("  - Developmental progression over time")
    print("  - Adaptive behavior based on rewards")
    
    # Create a multi-stage demonstration
    stages = [
        ("Initial State", 0, "Brain is initialized with random connections"),
        ("Sensory Processing", 100, "Brain processes sensory input"),
        ("Learning Phase", 500, "Brain learns through trial and error"),
        ("Development", 1000, "Brain develops and adapts over time"),
        ("Maturation", 2000, "Brain reaches stable behavior patterns")
    ]
    
    for stage_name, steps, description in stages:
        print(f"\nStage: {stage_name}")
        print(f"Description: {description}")
        print(f"Steps: {steps}")
        
        # Create configuration for this stage
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 400)
        config.set("region_count", 2)
        config.set("connection_probability", 0.08)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        
        # Show stage results
        neurons = brain.getTotalNeuronCount()
        synapses = brain.getTotalSynapseCount()
        spikes = brain.getTotalSpikeCount()
        rate = brain.getAverageFiringRate()
        
        print(f"Results:")
        print(f"  - Neurons: {neurons}")
        print(f"  - Synapses: {synapses}")
        print(f"  - Total spikes: {spikes}")
        print(f"  - Average firing rate: {rate:.2f} Hz")
        print(f"  - Processing time: {end_time - start_time:.3f}s")
    
    print(f"\nComplete Demo Results:")
    print(f"  - All stages completed successfully")
    print(f"  - System demonstrated full integration of neural components")
    print(f"  - Learning, memory, and development systems working in coordination")
    
    return True

def main():
    """Run all example demonstrations"""
    print("NLM Example Scripts and Templates")
    print("=" * 50)
    print("This script provides comprehensive examples of NLM usage,")
    print("demonstrations of key features, and templates for different use cases.")
    print()
    
    examples = [
        ("Simple Neural Network Simulation", create_simple_neural_simulation),
        ("Learning and Plasticity Simulation", create_learning_simulation),
        ("Behavioral Simulation with Agent and World", create_behavioral_simulation),
        ("Advanced Learning with Memory Systems", create_advanced_learning_example),
        ("Performance Benchmark", create_performance_benchmark),
        ("Configuration Templates", create_configuration_template),
        ("Complete NLM Demo", create_complete_demo)
    ]
    
    for example_name, example_func in examples:
        try:
            print(f"\n{'='*60}")
            example_func()
            print(f"{'='*60}")
        except Exception as e:
            print(f"Error in {example_name}: {e}")
            import traceback
            traceback.print_exc()
    
    print(f"\n{'='*60}")
    print("All examples completed!")
    print("These examples demonstrate the versatility and power of the NLM framework.")
    print("For more information, refer to the NLM documentation.")
    print(f"{'='*60}")

if __name__ == "__main__":
    main()

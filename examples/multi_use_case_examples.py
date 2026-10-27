#!/usr/bin/env python3
"""
NLM Agent Development Toolkit - Multiple Use Case Examples
=========================================================

This file contains multiple examples demonstrating different use cases
of the NLM framework for various development scenarios.
"""

import pynlm
import time
import json
import os

def setup_basic_neural_substrate(neuron_count=500, region_count=2):
    """Create a basic neural substrate with specified parameters."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.region_count", region_count)
    config.set("brain.connection_probability", 0.15)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

def example_1_basic_simulation(brain, num_steps=1000):
    """Example 1: Basic neural simulation with performance monitoring."""
    print("Example 1: Basic Neural Simulation")
    print("-" * 40)
    
    start_time = time.time()
    
    total_spikes = 0
    for step in range(num_steps):
        brain.step(step)
        total_spikes += brain.getTotalSpikeCount()
    
    end_time = time.time()
    
    results = {
        "total_steps": num_steps,
        "total_spikes": total_spikes,
        "average_firing_rate": total_spikes / num_steps,
        "execution_time_seconds": end_time - start_time,
        "spikes_per_second": total_spikes / (end_time - start_time),
        "final_neuron_count": brain.getTotalNeuronCount(),
        "final_synapse_count": brain.getTotalSynapseCount()
    }
    
    print(f"Simulation completed: {num_steps} steps")
    print(f"Total spikes: {total_spikes}")
    print(f"Average firing rate: {results['average_firing_rate']:.2f} Hz")
    print(f"Execution time: {results['execution_time_seconds']:.2f} seconds")
    
    return results

def example_2_reward_learning(config):
    """Example 2: Reward-based learning with agent-world interaction."""
    print("\nExample 2: Reward-Based Learning")
    print("-" * 40)
    
    # Create agent setup
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    
    learning_curve = []
    episode_rewards = []
    
    for episode in range(10):
        episode_reward = 0.0
        
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward from world
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            if reward == 0.0:
                reward = 0.3
            
            agent.applyRewardModulation(reward, agent.getPredictionError())
            
            episode_reward += reward
        
        episode_rewards.append(episode_reward)
        learning_curve.append(episode_reward / 100)  # Average per step
        
        print(f"  Episode {episode}: Reward={episode_reward:.2f}, "
              f"Avg per step={learning_curve[-1]:.3f}")
    
    results = {
        "episodes": len(episode_rewards),
        "episode_rewards": episode_rewards,
        "learning_curve": learning_curve,
        "final_brain_state": {
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "total_spikes": brain.getTotalSpikeCount()
        }
    }
    
    print(f"Learning completed over {len(episode_rewards)} episodes")
    print(f"Average reward per episode: {sum(episode_rewards) / len(episode_rewards):.2f}")
    
    return results

def example_3_developmental_learning(config):
    """Example 3: Developmental learning with age-dependent plasticity."""
    print("\nExample 3: Developmental Learning")
    print("-" * 40)
    
    # Create agent setup
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable development
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    development_data = []
    
    for day in range(20):  # 20 "days" of development
        # Simulate one day of learning
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(day * 100 + step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Age-dependent reward modulation
            age = agent.getNeuromodulationLevel()
            reward = 0.5 + age * 0.01  # Slightly increasing reward with age
            
            agent.applyRewardModulation(reward, 0.1)
            agent.updateDevelopment(1.0)  # One day of development
        
        day_summary = {
            "day": day,
            "developmental_stage": agent.getDevelopmentalStage(),
            "curiosity": agent.getCuriosityLevel(),
            "plasticity": agent.getNeuromodulationLevel(),
            "synapse_count": brain.getTotalSynapseCount(),
            "spike_count": brain.getTotalSpikeCount()
        }
        
        development_data.append(day_summary)
        
        print(f"  Day {day}: Stage={day_summary['developmental_stage']}, "
              f"Curiosity={day_summary['curiosity']:.3f}, "
              f"Synapses={day_summary['synapse_count']}")
    
    # Analyze development
    stages = [d["developmental_stage"] for d in development_data]
    final_stage = stages[-1] if stages else 0
    
    results = {
        "development_duration": len(development_data),
        "stage_transitions": list(set(stages)),
        "final_stage": final_stage,
        "plasticity_over_time": [d["plasticity"] for d in development_data],
        "synapse_growth": [(d["synapse_count"], d["day"]) for d in development_data],
        "learning_efficiency": sum(d["spike_count"] for d in development_data) / (len(development_data) * 100)
    }
    
    print(f"Development completed: Final stage = {final_stage}")
    print(f"Learning efficiency: {results['learning_efficiency']:.2f} spikes per step per day")
    
    return results

def example_4_environment_interaction(config):
    """Example 4: Complex environment interaction with multiple sensory inputs."""
    print("\nExample 4: Complex Environment Interaction")
    print("-" * 40)
    
    # Create agent setup
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    world.setRandomSeed(42)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    interaction_data = {
        "vision_history": [],
        "reward_history": [],
        "action_history": [],
        "location_history": []
    }
    
    for episode in range(5):
        episode_vision = []
        episode_rewards = []
        episode_actions = []
        episode_locations = []
        
        for step in range(200):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record perception and action
            episode_vision.append(percept.getVision())
            episode_rewards.append(percept.getInternal()[0] if percept.getInternal() else 0.0)
            episode_actions.append(action)
            
            body = world.getAgentBody()
            episode_locations.append((body.x, body.y))
            
            # Apply learning
            reward = episode_rewards[-1] if episode_rewards[-1] != 0 else 0.3
            agent.applyRewardModulation(reward, agent.getPredictionError())
            agent.updateDevelopment(0.1)
        
        # Store episode data
        interaction_data["vision_history"].append(episode_vision)
        interaction_data["reward_history"].append(episode_rewards)
        interaction_data["action_history"].append(episode_actions)
        interaction_data["location_history"].append(episode_locations)
        
        # Analyze episode
        avg_vision = sum(sum(frame) for frame in episode_vision) / (len(episode_vision) * len(episode_vision[0]) if episode_vision else 1)
        print(f"  Episode {episode}: Avg vision intensity={avg_vision:.3f}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Actions={len(set(episode_actions))} unique")
    
    results = {
        "episodes": len(interaction_data["vision_history"]),
        "total_steps": sum(len(history) for history in interaction_data["vision_history"]),
        "sensory_data": interaction_data,
        "environmental_complexity": {
            "vision_resolution": world.getVisionWidth() * world.getVisionHeight(),
            "world_size": world.getWidth() * world.getHeight(),
            "agent_movements": sum(len(locations) for locations in interaction_data["location_history"])
        }
    }
    
    print(f"Environment interaction completed: {results['total_steps']} total steps")
    print(f"Environmental complexity: Vision={world.getVisionWidth()}x{world.getVisionHeight()}, "
          f"World={world.getWidth()}x{world.getHeight()}")
    
    return results

def example_5_performance_benchmark(config):
    """Example 5: Performance benchmarking and comparison."""
    print("\nExample 5: Performance Benchmarking")
    print("-" * 40)
    
    benchmarks = []
    
    # Test different brain configurations
    configurations = [
        {"name": "Small", "neurons": 500, "regions": 2},
        {"name": "Medium", "neurons": 1500, "regions": 3},
        {"name": "Large", "neurons": 3000, "regions": 4}
    ]
    
    for config_params in configurations:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", config_params["neurons"])
        config.set("brain.region_count", config_params["regions"])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Benchmark performance
        start_time = time.time()
        
        for step in range(500):
            brain.step(step)
        
        end_time = time.time()
        
        benchmark_result = {
            "name": config_params["name"],
            "neuron_count": config_params["neurons"],
            "region_count": config_params["regions"],
            "execution_time": end_time - start_time,
            "steps_per_second": 500 / (end_time - start_time),
            "spikes_generated": brain.getTotalSpikeCount(),
            "memory_usage": brain.getTotalNeuronCount() + brain.getTotalSynapseCount()
        }
        
        benchmarks.append(benchmark_result)
        
        print(f"  {config_params['name']} Brain ({config_params['neurons']} neurons): "
              f"{benchmark_result['steps_per_second']:.1f} steps/sec, "
              f"{benchmark_result['spikes_generated']} spikes")
    
    # Compare performance
    print("\nPerformance Comparison:")
    for benchmark in benchmarks:
        print(f"  {benchmark['name']}: {benchmark['steps_per_second']:.1f} steps/sec, "
              f"Efficiency={benchmark['spikes_generated']/benchmark['memory_usage']:.3f} spikes/mem")
    
    results = {
        "benchmarks": benchmarks,
        "best_performance": max(benchmarks, key=lambda x: x["steps_per_second"]),
        "most_efficient": max(benchmarks, key=lambda x: x["spikes_generated"] / x["memory_usage"])
    }
    
    return results

def main():
    """Main function demonstrating all examples."""
    print("NLM Agent Development Toolkit - Multiple Use Case Examples")
    print("=" * 60)
    
    # Create base configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.connection_probability", 0.12)
    
    print("\nRunning comprehensive examples demonstrating NLM framework capabilities:")
    print("1. Basic Neural Simulation")
    print("2. Reward-Based Learning")
    print("3. Developmental Learning")
    print("4. Complex Environment Interaction")
    print("5. Performance Benchmarking")
    
    # Run all examples
    example1_results = example_1_basic_simulation(setup_basic_neural_substrate(1000), 1000)
    
    example2_results = example_2_reward_learning(config)
    
    example3_results = example_3_developmental_learning(config)
    
    example4_results = example_4_environment_interaction(config)
    
    example5_results = example_5_performance_benchmark(config)
    
    # Summary
    print("\n" + "=" * 60)
    print("EXAMPLES COMPLETED SUCCESSFULLY")
    print("=" * 60)
    
    summary = {
        "example_1": {
            "name": "Basic Neural Simulation",
            "steps_completed": example1_results["total_steps"],
            "firing_rate": example1_results["average_firing_rate"],
            "performance": example1_results["spikes_per_second"]
        },
        "example_2": {
            "name": "Reward-Based Learning",
            "episodes": example2_results["episodes"],
            "avg_reward_per_episode": sum(example2_results["episode_rewards"]) / len(example2_results["episode_rewards"])
        },
        "example_3": {
            "name": "Developmental Learning",
            "days_completed": example3_results["development_duration"],
            "final_stage": example3_results["final_stage"],
            "learning_efficiency": example3_results["learning_efficiency"]
        },
        "example_4": {
            "name": "Environment Interaction",
            "episodes": example4_results["episodes"],
            "total_steps": example4_results["total_steps"],
            "complexity_score": example4_results["environmental_complexity"]["vision_resolution"]
        },
        "example_5": {
            "name": "Performance Benchmarking",
            "configurations_tested": len(example5_results["benchmarks"]),
            "best_performance": example5_results["best_performance"]["name"],
            "most_efficient": example5_results["most_efficient"]["name"]
        }
    }
    
    print(f"✓ Example 1 ({summary['example_1']['name']}): "
          f"{summary['example_1']['steps_completed']} steps at {summary['example_1']['performance']:.1f} steps/sec")
    print(f"✓ Example 2 ({summary['example_2']['name']}): "
          f"{summary['example_2']['episodes']} episodes, avg reward={summary['example_2']['avg_reward_per_episode']:.2f}")
    print(f"✓ Example 3 ({summary['example_3']['name']}): "
          f"{summary['example_3']['days_completed']} days, final stage={summary['example_3']['final_stage']}")
    print(f"✓ Example 4 ({summary['example_4']['name']}): "
          f"{summary['example_4']['episodes']} episodes, {summary['example_4']['total_steps']} total steps")
    print(f"✓ Example 5 ({summary['example_5']['name']}): "
          f"Tested {summary['example_5']['configurations_tested']} configurations, "
          f"best={summary['example_5']['best_performance']}, most efficient={summary['example_5']['most_efficient']}")
    
    # Save all results
    all_results = {
        "examples_summary": summary,
        "detailed_results": {
            "example_1": example1_results,
            "example_2": example2_results,
            "example_3": example3_results,
            "example_4": example4_results,
            "example_5": example5_results
        }
    }
    
    with open("multi_use_case_results.json", "w") as f:
        json.dump(all_results, f, indent=2)
    
    print(f"\nAll example results saved to multi_use_case_results.json")
    
    print("\n" + "=" * 60)
    print("ALL EXAMPLES COMPLETED SUCCESSFULLY")
    print("=" * 60)
    print("\nThe NLM framework successfully demonstrated capabilities across:")
    print("  • Basic neural simulation and computation")
    print("  • Reward-based learning and adaptation")
    print("  • Developmental changes and plasticity")
    print("  • Complex sensory-motor interactions")
    print("  • Performance optimization and benchmarking")
    print("\nThese examples provide a foundation for building")
    print("advanced AI agents and neural systems.")

if __name__ == "__main__":
    main()
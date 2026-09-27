# NLM Python Advanced Examples
# Advanced usage patterns and comprehensive examples for NLM

import pynlm
import json
from typing import List, Optional

def create_advanced_brain_config():
    """Create a more sophisticated brain configuration with advanced settings."""
    config = pynlm.createDefaultConfig()
    
    # Advanced brain configuration
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 3)
    config.set("brain.connection_probability", 0.12)
    config.set("brain.initial_weight_mean", 0.6)
    config.set("brain.initial_weight_std", 0.15)
    
    # Advanced plasticity settings
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.stdp.tau_plus", 25.0)
    config.set("plasticity.stdp.tau_minus", 25.0)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
    config.set("plasticity.structural.pruning_rate", 0.00002)
    
    # Advanced neuromodulation
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.3)
    
    # Development settings
    config.set("development.initial_stage", "CriticalPeriod")
    config.set("development.maturation_rate", 0.001)
    config.set("development.plasticity_decay", 0.0001)
    
    return config

def create_development_stage_experiment(brain, world, agent, num_episodes=5):
    """Advanced experiment demonstrating brain development over multiple episodes."""
    print("=== Advanced Development Experiment ===")
    
    # Enable all subsystems for comprehensive development
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    episode_data = []
    
    for episode in range(num_episodes):
        print(f"\n--- Episode {episode + 1}/{num_episodes} ---")
        
        # Reset for new episode
        brain.reset()
        brain.initialize()
        world.reset()
        agent.reset()
        agent.initialize(world)
        
        # Run simulation
        episode_start_step = 0
        for step in range(episode_start_step, episode_start_step + 1000):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            reward = 0.0
            if world.getSensoryPercept().getInternal():
                reward = world.getSensoryPercept().getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            # Log every 200 steps
            if step % 200 == 0:
                print(f"  Step {step}: "
                      f"Firing={brain.getFiringNeuronCount()}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}, "
                      f"DevStage={brain.getDevelopmentalStage()}")
        
        # Record episode statistics
        episode_stats = {
            "episode": episode + 1,
            "total_spikes": brain.getTotalSpikeCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError(),
            "neuromodulation": agent.getNeuromodulationLevel(),
            "final_development_stage": brain.getDevelopmentalStage()
        }
        episode_data.append(episode_stats)
        
        # Analyze development progress
        if episode > 0:
            prev_stage = episode_data[episode - 1]["final_development_stage"]
            curr_stage = episode_stats["final_development_stage"]
            stage_progress = "Advanced" if curr_stage.value > prev_stage.value else "Regressed"
            print(f"  Development Progress: {stage_progress}")
    
    # Analyze results
    print(f"\n=== Experiment Results ===")
    print(f"Final Development Stage: {episode_data[-1]['final_development_stage']}")
    print(f"Average Curiosity Level: {sum(e['curiosity'] for e in episode_data) / len(episode_data):.3f}")
    print(f"Average Novelty Detection: {sum(e['novelty'] for e in episode_data) / len(episode_data):.3f}")
    
    return episode_data

def create_memory_profiling_experiment(brain, world, agent, duration=500):
    """Advanced experiment demonstrating memory system integration."""
    print("\n=== Memory System Profiling Experiment ===")
    
    # Access brain subsystems directly for detailed analysis
    working_memory = brain.getWorkingMemory()
    episodic_memory = brain.getEpisodicMemory()
    
    memory_events = []
    
    for step in range(duration):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Record memory state
        if step % 100 == 0:
            wm_size = working_memory.getActiveMemorySize() if working_memory else 0
            em_episodes = episodic_memory.getEpisodeCount() if episodic_memory else 0
            
            memory_event = {
                "step": step,
                "working_memory_size": wm_size,
                "episodic_episodes": em_episodes,
                "spike_count": brain.getTotalSpikeCount(),
                "active_neurons": brain.getActiveNeuronCount(),
                "firing_neurons": brain.getFiringNeuronCount()
            }
            memory_events.append(memory_event)
            
            print(f"  Step {step}: WM={wm_size}, EM={em_episodes}, "
                  f"Spikes={brain.getTotalSpikeCount()}")
    
    # Analyze memory dynamics
    print(f"\n=== Memory Analysis ===")
    if memory_events:
        avg_wm_size = sum(e["working_memory_size"] for e in memory_events) / len(memory_events)
        max_wm_size = max(e["working_memory_size"] for e in memory_events)
        print(f"Average Working Memory Size: {avg_wm_size:.1f}")
        print(f"Peak Working Memory Size: {max_wm_size:.1f}")
        
        if episodic_memory:
            print(f"Total Episodic Episodes Recorded: {episodic_memory.getTotalEpisodeCount()}")
    
    return memory_events

def create_curiosity_driven_exploration(brain, world, agent, exploration_steps=300):
    """Advanced experiment demonstrating curiosity-driven behavior."""
    print("\n=== Curiosity-Driven Exploration Experiment ===")
    
    # Enable curiosity-driven exploration
    agent.enableCuriosity(True)
    agent.enableNovelty(True)
    agent.enableDevelopment(True)
    
    exploration_data = []
    action_distribution = {}
    
    # Track action frequencies
    action_types = ["MoveForward", "MoveBackward", "TurnLeft", "TurnRight", 
                   "Look", "Interact", "Wait", "Custom"]
    
    for step in range(exploration_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action with curiosity
        action = agent.decodeMotorCommand()
        action_name = action.getName() if hasattr(action, 'getName') else f"Action{action}"
        
        # Track action distribution
        if action_name not in action_distribution:
            action_distribution[action_name] = 0
        action_distribution[action_name] += 1
        
        # Apply motor command
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record exploration metrics
        if step % 50 == 0:
            novelty = agent.getNoveltyLevel()
            curiosity = agent.getCuriosityLevel()
            exploration_data.append({
                "step": step,
                "curiosity": curiosity,
                "novelty": novelty,
                "action": action_name,
                "simulation_time": world.getSimulationTime()
            })
            
            print(f"  Step {step}: Curiosity={curiosity:.3f}, "
                  f"Novelty={novelty:.3f}, Action={action_name}")
    
    # Analyze exploration patterns
    print(f"\n=== Exploration Analysis ===")
    print(f"Total actions recorded: {sum(action_distribution.values())}")
    print(f"Unique actions: {len(action_distribution)}")
    print(f"Top 3 most frequent actions:")
    
    sorted_actions = sorted(action_distribution.items(), key=lambda x: x[1], reverse=True)
    for i, (action, count) in enumerate(sorted_actions[:3]):
        percentage = (count / sum(action_distribution.values())) * 100
        print(f"  {i+1}. {action}: {count} times ({percentage:.1f}%)")
    
    return exploration_data

def create_advanced_visualization(world, brain, duration=200):
    """Advanced visualization and analysis experiment."""
    print("\n=== Advanced Visualization Analysis ===")
    
    visualization_data = []
    
    for step in range(duration):
        # Update world
        world.update(0.1)
        
        # Get sensor readings
        percept = world.getSensoryPercept()
        
        # Record visualization data
        if step % 25 == 0:
            # Get world state
            agent_body = world.getAgentBody()
            world_objects = len(world.getWorldObjects()) if hasattr(world, 'getWorldObjects') else 0
            
            # Get brain statistics
            brain_stats = {
                "step": step,
                "simulation_time": world.getSimulationTime(),
                "agent_x": agent_body.x if agent_body else 0,
                "agent_y": agent_body.y if agent_body else 0,
                "agent_orientation": agent_body.orientation if agent_body else 0,
                "world_objects": world_objects,
                "total_spikes": brain.getTotalSpikeCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "avg_firing_rate": brain.getAverageFiringRate(),
                "development_stage": brain.getDevelopmentalStage().value if brain.getDevelopmentalStage() else 0
            }
            
            visualization_data.append(brain_stats)
            
            print(f"  Step {step}: "
                  f"Pos=({agent_body.x:.1f},{agent_body.y:.1f}), "
                  f"Spikes={brain.getTotalSpikeCount()}, "
                  f"Dev={brain.getDevelopmentalStage().value if brain.getDevelopmentalStage() else 0}")
    
    # Generate visualization summary
    print(f"\n=== Visualization Summary ===")
    if visualization_data:
        total_movement = 0
        for i in range(1, len(visualization_data)):
            prev = visualization_data[i-1]
            curr = visualization_data[i]
            dx = curr["agent_x"] - prev["agent_x"]
            dy = curr["agent_y"] - prev["agent_y"]
            total_movement += (dx**2 + dy**2)**0.5
        
        avg_movement = total_movement / max(len(visualization_data) - 1, 1)
        print(f"Total exploration distance: {total_movement:.1f} units")
        print(f"Average step distance: {avg_movement:.1f} units")
        print(f"Final position: ({visualization_data[-1]['agent_x']:.1f}, "
              f"{visualization_data[-1]['agent_y']:.1f})")
    
    return visualization_data

def run_comprehensive_demo():
    """Run a comprehensive demonstration of all advanced features."""
    print("=== NLM Advanced Features Comprehensive Demo ===\n")
    
    # Create advanced configuration
    config = create_advanced_brain_config()
    print("1. Created advanced brain configuration")
    
    # Create all components
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("2. Brain created and initialized")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    print("3. World created and configured")
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("4. Agent brain interface created and initialized")
    
    # Run development experiment
    print("\n--- Running Development Experiment ---")
    development_data = create_development_stage_experiment(brain, world, agent, 3)
    
    # Run memory profiling
    print("\n--- Running Memory Profiling ---")
    memory_data = create_memory_profiling_experiment(brain, world, agent, 200)
    
    # Run curiosity exploration
    print("\n--- Running Curiosity-Driven Exploration ---")
    curiosity_data = create_curiosity_driven_exploration(brain, world, agent, 150)
    
    # Run visualization analysis
    print("\n--- Running Visualization Analysis ---")
    visualization_data = create_advanced_visualization(world, brain, 100)
    
    # Generate final report
    print("\n=== Final Demo Report ===")
    print("✓ Advanced configuration created successfully")
    print("✓ Development experiment completed")
    print("✓ Memory system analyzed")
    print("✓ Curiosity-driven behavior demonstrated")
    print("✓ Visualization data collected")
    
    # Save results
    results = {
        "development": development_data,
        "memory": memory_data,
        "curiosity": curiosity_data,
        "visualization": visualization_data
    }
    
    # Save to JSON for further analysis
    with open("/tmp/nlm_advanced_demo_results.json", "w") as f:
        json.dump(results, f, indent=2, default=str)
    
    print(f"✓ Results saved to /tmp/nlm_advanced_demo_results.json")
    print("\n=== Demo Complete ===")
    print("All advanced NLM features demonstrated successfully!")
    
    return results

if __name__ == "__main__":
    # Run the comprehensive demo
    results = run_comprehensive_demo()
    
    print("\nFor more information and additional examples, see:")
    print("- docs/ARCHITECTURE.md")
    print("- docs/SCIENCE.md")
    print("- docs/EXPERIMENTS.md")
    print("- easy_usage.md")
    print("- HOW_TO_USE.md")
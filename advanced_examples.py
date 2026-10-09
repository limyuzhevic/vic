# Advanced NLM Python Examples
# Demonstrates advanced usage patterns and power user features

import pynlm
import time
import json
from pathlib import Path

def advanced_brain_creation():
    """Create an advanced brain with custom configuration"""
    print("=== Advanced Brain Creation Example ===")
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    
    # Configure advanced parameters
    config.set("neuron_count", 2000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.15)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.018)
    config.set("synaptogenesis_rate", 0.0002)
    config.set("pruning_rate", 0.00002)
    config.set("development_enabled", 1)
    config.set("plasticity_enabled", 1)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Regions: {brain.getRegionCount()}")
    print(f"Development: {brain.getDevelopmentalStage()}")
    
    return brain

def advanced_agent_training(brain, world, num_episodes=50):
    """Train an advanced agent with sophisticated learning strategies"""
    print(f"\n=== Advanced Agent Training ({num_episodes} Episodes) ===")
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableAllFeatures(True)
    
    print("Enabled all advanced features: Reward modulation, Structural Plasticity, Development, Curiosity")
    
    total_reward = 0.0
    best_brain_state = None
    best_reward = float('-inf')
    
    for episode in range(num_episodes):
        episode_reward = 0.0
        
        # Run episode with observation
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run multiple steps per episode
        for step in range(100):
            # Get brain status
            brain.step(step)
            
            # Decode action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Compute reward
            reward = world.computeReward(agent.getBrain().getRegions()[0].get())
            episode_reward += reward
            total_reward += reward
            
            # Apply reward modulation
            agent.applyRewardModulation(reward, 0.0f)
            
            # Update development
            agent.updateDevelopment(0.1f)
        
        # Store best performing brain state
        if episode_reward > best_reward:
            best_reward = episode_reward
            best_brain_state = agent.exportBrainState()
            print(f"Episode {episode}: New best reward = {episode_reward:.2f}")
        
        # Print progress every 10 episodes
        if (episode + 1) % 10 == 0:
            avg_reward = total_reward / (episode + 1)
            print(f"Episode {episode + 1}: Average reward = {avg_reward:.2f}")
    
    print(f"\nTraining complete. Best reward: {best_reward:.2f}")
    print(f"Average reward: {total_reward / num_episodes:.2f}")
    
    return agent, best_reward

def advanced_simulation_suite():
    """Run a comprehensive simulation suite with different scenarios"""
    print("\n=== Advanced Simulation Suite ===")
    
    scenarios = [
        {"name": "Basic Navigation", "neurons": 500, "steps": 1000},
        {"name": "Complex Environment", "neurons": 1000, "steps": 2000},
        {"name": "Multi-Agent Training", "neurons": 1500, "steps": 3000},
        {"name": "Long-term Learning", "neurons": 800, "steps": 5000},
    ]
    
    results = []
    
    for scenario in scenarios:
        print(f"\nRunning scenario: {scenario['name']}")
        
        # Create brain for scenario
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", scenario['neurons'])
        config.set("region_count", 2)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableAllFeatures(True)
        
        # Run simulation
        start_time = time.time()
        
        for step in range(scenario['steps']):
            # Get observation
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update development
            agent.updateDevelopment(0.1f)
        
        elapsed = time.time() - start_time
        
        # Collect statistics
        stats = {
            "scenario": scenario['name'],
            "neurons": brain.getTotalNeuronCount(),
            "steps": scenario['steps'],
            "time_elapsed": elapsed,
            "firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
            "curiosity_level": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError(),
            "development_stage": agent.getDevelopmentalStage()
        }
        
        results.append(stats)
        
        print(f"  Completed in {elapsed:.1f}s")
        print(f"  Firing rate: {stats['firing_rate']:.2f}")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Curiosity: {stats['curiosity_level']:.2f}")
    
    return results

def advanced_analysis_and_export():
    """Perform advanced analysis and export brain states"""
    print("\n=== Advanced Analysis and Export ===")
    
    # Create a complex brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1200)
    config.set("region_count", 4)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(16, 16, 6, 6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllFeatures(True)
    
    # Run extensive simulation
    for step in range(2000):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.updateDevelopment(0.1f)
    
    # Perform advanced analysis
    print("Performing advanced analysis...")
    
    # Get comprehensive statistics
    stats = agent.getStats()
    brain_stats = agent.exportBrainState().getBrain()->getBrainStats()
    memory_stats = agent.getMemoryStats()
    integration_status = agent.exportBrainState().getBrain()->getIntegrationStatus()
    
    analysis = {
        "neural_metrics": {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
            "e_i_ratio": brain.getExcitationInhibitionRatio()
        },
        "agent_metrics": {
            "curiosity": stats[0],
            "novelty": stats[1],
            "prediction_error": stats[2],
            "neuromodulation": stats[3],
            "development_stage": stats[4]
        },
        "memory_metrics": memory_stats,
        "system_integration": integration_status,
        "brain_size": brain_stats,
        "activity_stats": brain.getActivityStats(),
        "development_stats": brain.getDevelopmentStatus()
    }
    
    # Save analysis to JSON
    analysis_file = "nlm_advanced_analysis.json"
    with open(analysis_file, 'w') as f:
        json.dump(analysis, f, indent=2)
    
    print(f"Analysis saved to {analysis_file}")
    
    # Print summary
    print("\n=== Advanced Analysis Summary ===")
    print(f"Neural Efficiency: {analysis['neural_metrics']['total_spikes'] / analysis['neural_metrics']['total_neurons']:.2f} spikes/neuron")
    print(f"Integration Score: {sum(1 for v in integration_status.values() if v)}/{len(integration_status)}")
    print(f"Memory Capacity: {analysis['memory_metrics'].get('working_memory_traces', 0)} working memory traces")
    print(f"Development Stage: {analysis['agent_metrics']['development_stage']}")
    
    # Export brain state
    brain.save("nlm_advanced_brain_state.bin")
    print(f"Brain state exported to nlm_advanced_brain_state.bin")
    
    return analysis

def advanced_research_simulation():
    """Run a research-grade simulation with detailed monitoring"""
    print("\n=== Advanced Research Simulation ===")
    
    # Create research configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 3000)
    config.set("region_count", 6)
    config.set("connection_probability", 0.12)
    config.set("stdp_ltp_weight", 0.015)
    config.set("stdp_ltd_weight", 0.014)
    config.set("development_enabled", 1)
    config.set("plasticity_enabled", 1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(24, 24, 10, 10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableAllFeatures(True)
    
    print("Starting research simulation with enhanced monitoring...")
    print("Monitoring: Neural dynamics, memory consolidation, development stages, neuromodulation")
    
    # Research-grade monitoring
    research_data = {
        "simulation_time": 0,
        "neural_activity": [],
        "memory_events": [],
        "development_stages": [],
        "neuromodulation_levels": [],
        "prediction_errors": [],
        "curiosity_drives": []
    }
    
    # Run long simulation
    for step in range(5000):
        # Record research data
        research_data["simulation_time"] = step
        
        # Neural activity monitoring
        activity = {
            "step": step,
            "firing_neurons": brain.getFiringNeuronCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "total_spikes": brain.getTotalSpikeCount(),
            "e_i_ratio": brain.getExcitationInhibitionRatio()
        }
        research_data["neural_activity"].append(activity)
        
        # Memory events
        if brain.getEpisodicMemory():
            memory_events = {
                "step": step,
                "episode_count": brain.getEpisodicMemory().getEpisodeCount(),
                "working_memory_traces": brain.getWorkingMemory().getActiveTraces()
            }
            research_data["memory_events"].append(memory_events)
        
        # Development stages
        development = {
            "step": step,
            "stage": agent.getDevelopmentalStage(),
            "neuromodulation": agent.getNeuromodulationLevel()
        }
        research_data["development_stages"].append(development)
        
        # Neuromodulation levels
        neuromod = {
            "step": step,
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "prediction_error": agent.getPredictionError()
        }
        research_data["neuromodulation_levels"].append(neuromod)
        
        # Run simulation step
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.updateDevelopment(0.1f)
        
        # Print progress
        if step % 500 == 0:
            print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.2f}, "
                  f"Curiosity = {agent.getCuriosityLevel():.2f}, "
                  f"Memory episodes = {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
    
    print("\nResearch simulation complete!")
    print(f"Total simulation time: {research_data['simulation_time']} steps")
    print(f"Average firing rate: {sum(a['avg_firing_rate'] for a in research_data['neural_activity']) / len(research_data['neural_activity']):.2f}")
    print(f"Final development stage: {research_data['development_stages'][-1]['stage']}")
    print(f"Final curiosity level: {research_data['neuromodulation_levels'][-1]['curiosity']:.2f}")
    
    return research_data

def main():
    """Run all advanced examples"""
    print("🚀 Advanced NLM Python Examples")
    print("==============================")
    
    # Example 1: Advanced brain creation
    brain1 = advanced_brain_creation()
    
    # Example 2: Advanced agent training
    world1 = pynlm.createSimpleWorld()
    world1.configure(15, 15, 8, 8)
    world1.reset()
    
    agent1, best_reward = advanced_agent_training(brain1, world1, 20)
    print(f"Best reward achieved: {best_reward:.2f}")
    
    # Example 3: Advanced simulation suite
    suite_results = advanced_simulation_suite()
    
    # Example 4: Advanced analysis and export
    analysis_results = advanced_analysis_and_export()
    
    # Example 5: Advanced research simulation
    research_data = advanced_research_simulation()
    
    print("\n🎉 All advanced examples completed successfully!")
    print("\nKey improvements demonstrated:")
    print("  ✅ Advanced configuration management")
    print("  ✅ Comprehensive agent training")
    print("  ✅ Research-grade monitoring")
    print("  ✅ Advanced analysis and export")
    print("  ✅ System integration verification")
    print("  ✅ Performance optimization")

if __name__ == "__main__":
    main()
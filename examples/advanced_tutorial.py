#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Advanced Tutorial
===============================================

This script demonstrates advanced NLM usage patterns for experienced users.

It covers:
- Advanced brain configuration
- Batch processing capabilities
- Learning analysis techniques
- Performance optimization
- Integration with external systems
"""

import sys
import time
import json
import numpy as np
from pathlib import Path

# Add local module to path
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    from pynlm_advanced import (
        NLMAdvancedAPI, run_advanced_simulation, analyze_learning,
        SimulationResult, LearningMetrics
    )
except ImportError as e:
    print(f"Warning: Could not import advanced modules: {e}")
    print("Falling back to basic NLM API")
    pynlm = None

def create_advanced_config(neuron_count=2000, connection_density=0.15):
    """
    Create an advanced NLM configuration for complex simulations.
    """
    if not pynlm:
        raise ImportError("pynlm module not available")
        
    config = pynlm.createDefaultConfig()
    
    # Advanced brain configuration
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", connection_density)
    config.set("brain.connection_probability", connection_density * 10)
    config.set("brain.initial_weight_mean", 0.6)
    config.set("brain.initial_weight_std", 0.15)
    
    # Neuron parameters for advanced dynamics
    config.set("brain.v_thresh", -45.0)      # Lower threshold for higher excitability
    config.set("brain.v_rest", -65.0)       # Modified resting potential
    config.set("brain.v_reset", -70.0)      # Reset potential
    config.set("brain.tau_mem", 15.0)       # Slower membrane time constant
    config.set("brain.tau_ref", 3.0)        # Refractory period
    
    # Plasticity settings for rapid learning
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.005)  # Higher learning rate
    config.set("plasticity.stdp.tau_plus", 25.0)
    config.set("plasticity.stdp.tau_minus", 25.0)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.008)
    
    # Neuromodulation for advanced behavior
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction.error_threshold", 0.1)
    
    # Development for adaptive systems
    config.set("development.synaptogenesis_rate", 0.002)
    config.set("development.pruning_rate", 0.0005)
    config.set("development.maturation_rate", 0.01)
    
    # Memory systems for storage
    config.set("memory.working_memory.capacity", 1000)
    config.set("memory.episodic.capacity", 500)
    config.set("memory.semantic.capacity", 200)
    
    # Advanced features
    config.set("prediction.enable", True)
    config.set("prediction.horizon", 20)
    config.set("cognition.enable_planning", True)
    config.set("cognition.planning_horizon", 10)
    
    return config

def create_diverse_brain_configs(base_config, variations=5):
    """
    Create multiple brain configurations for comparative analysis.
    """
    configs = []
    
    for i in range(variations):
        config = pynlm.createDefaultConfig()
        
        # Copy base config parameters
        config.set("brain.neuron_count", base_config.getOr("brain.neuron_count", 1000))
        config.set("brain.synapse_density", base_config.getOr("brain.synapse_density", 0.1))
        config.set("brain.connection_probability", base_config.getOr("brain.synapse_density", 0.1) * 10)
        
        # Introduce variations for diversity
        config.set("brain.initial_weight_mean", 0.5 + (i * 0.1))  # Vary initial weights
        config.set("plasticity.stdp.learning_rate", 0.001 * (1.0 + i * 0.5))
        config.set("brain.v_thresh", -50.0 - (i * 5.0))
        
        # Add environment-specific parameters
        config.set("environment.width", 20 + (i * 5))
        config.set("environment.height", 20 + (i * 5))
        config.set("environment.vision_width", 8)
        config.set("environment.vision_height", 8)
        
        configs.append(config)
    
    return configs

def run_advanced_agent_simulation(
    brain,
    world,
    agent,
    steps=500,
    log_interval=50
):
    """
    Run an advanced agent simulation with comprehensive logging.
    """
    print(f"Starting advanced agent simulation for {steps} steps...")
    
    simulation_data = {
        "steps": [],
        "rewards": [],
        "firing_rates": [],
        "curiosity_levels": [],
        "novelty_levels": [],
        "actions_taken": []
    }
    
    for step in range(steps):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        if step % log_interval == 0:
            simulation_data["steps"].append(step)
            simulation_data["rewards"].append(result.reward)
            simulation_data["firing_rates"].append(brain.getAverageFiringRate())
            simulation_data["curiosity_levels"].append(agent.getCuriosityLevel())
            simulation_data["novelty_levels"].append(agent.getNoveltyLevel())
            simulation_data["actions_taken"].append(action.getName())
            
            # Print progress
            print(f"Step {step:4d}: "
                  f"Firing: {brain.getFiringNeuronCount():4d}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}, "
                  f"Action: {action.getName()}")
    
    return simulation_data

def analyze_learning_dynamics(simulation_data):
    """
    Analyze learning dynamics from simulation data.
    """
    if not simulation_data["steps"]:
        return None
    
    # Basic statistics
    rewards = np.array(simulation_data["rewards"])
    firing_rates = np.array(simulation_data["firing_rates"])
    curiosity = np.array(simulation_data["curiosity_levels"])
    novelty = np.array(simulation_data["novelty_levels"])
    
    analysis = {
        "reward_statistics": {
            "mean": float(np.mean(rewards)),
            "std": float(np.std(rewards)),
            "min": float(np.min(rewards)),
            "max": float(np.max(rewards)),
            "trend": float(np.polyfit(simulation_data["steps"], rewards, 1)[0])
        },
        
        "firing_statistics": {
            "mean": float(np.mean(firing_rates)),
            "stability": float(1.0 - np.std(firing_rates) / np.max(firing_rates)),
            "variability": float(np.std(firing_rates))
        },
        
        "neuromodulation": {
            "curiosity_mean": float(np.mean(curiosity)),
            "curiosity_trend": float(np.polyfit(simulation_data["steps"], curiosity, 1)[0]),
            "novelty_mean": float(np.mean(novelty)),
            "correlation": float(np.corrcoef(curiosity, novelty)[0, 1])
        },
        
        "action_patterns": {
            "action_counts": {},
            "action_transitions": {}
        }
    }
    
    # Analyze action patterns
    action_counts = {}
    for action in simulation_data["actions_taken"]:
        action_counts[action] = action_counts.get(action, 0) + 1
    
    analysis["action_patterns"]["action_counts"] = action_counts
    
    return analysis

def optimize_configuration(config, target_performance=0.8):
    """
    Optimize configuration based on target performance.
    """
    print("Starting configuration optimization...")
    
    # This is a placeholder for advanced optimization algorithms
    # In a real implementation, this would use Bayesian optimization,
    # genetic algorithms, or reinforcement learning
    
    optimizations = []
    
    # Simple heuristic optimizations for demonstration
    if config.getOr("brain.neuron_count", 0) < 500:
        optimizations.append({
            "parameter": "brain.neuron_count",
            "change": "+1000",
            "reason": "Low neuron count for complex tasks"
        })
    
    if config.getOr("plasticity.stdp.learning_rate", 0) < 0.001:
        optimizations.append({
            "parameter": "plasticity.stdp.learning_rate",
            "change": "+0.002",
            "reason": "Low learning rate"
        })
    
    if config.getOr("neuromod.curiosity.enable", False) == False:
        optimizations.append({
            "parameter": "neuromod.curiosity.enable",
            "change": "True",
            "reason": "Curiosity improves exploration"
        })
    
    for opt in optimizations:
        param_name = opt["parameter"]
        param_parts = param_name.split('.')
        
        # Apply optimization (simplified - in reality would need proper config access)
        print(f"Optimization: {param_name} -> {opt['change']} ({opt['reason']})")
    
    return optimizations

def main():
    """
    Run the advanced tutorial demonstration.
    """
    print("=== NLM Advanced Tutorial ===\n")
    
    if not pynlm:
        print("Error: NLM Python bindings not available")
        print("Please install NLM with: pip install scikit-build-core pybind11")
        return 1
    
    try:
        # Example 1: Create advanced configuration
        print("1. Creating advanced configuration...")
        config = create_advanced_config(neuron_count=1500, connection_density=0.12)
        print(f"   Created config with {config.getOr('brain.neuron_count', 0)} neurons")
        
        # Example 2: Create diverse configurations for comparison
        print("\n2. Creating diverse configurations for comparative analysis...")
        base_config = create_advanced_config()
        diverse_configs = create_diverse_brain_configs(base_config, variations=3)
        print(f"   Created {len(diverse_configs)} diverse configurations")
        
        # Example 3: Run advanced simulation
        print("\n3. Running advanced simulation...")
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        simulation_data = run_advanced_agent_simulation(
            brain, world, agent, steps=200, log_interval=50
        )
        
        # Example 4: Analyze learning dynamics
        print("\n4. Analyzing learning dynamics...")
        analysis = analyze_learning_dynamics(simulation_data)
        
        if analysis:
            print("Learning Analysis:")
            print(f"  Reward Mean: {analysis['reward_statistics']['mean']:.3f}")
            print(f"  Reward Trend: {analysis['reward_statistics']['trend']:.3f}")
            print(f"  Firing Rate Stability: {analysis['firing_statistics']['stability']:.3f}")
            print(f"  Curiosity Mean: {analysis['neuromodulation']['curiosity_mean']:.3f}")
        
        # Example 5: Configuration optimization
        print("\n5. Configuration optimization suggestions...")
        optimizations = optimize_configuration(config)
        print(f"   Found {len(optimizations)} optimization suggestions")
        
        # Example 6: Save results
        print("\n6. Saving simulation results...")
        with open("advanced_simulation_results.json", "w") as f:
            json.dump(simulation_data, f, indent=2)
        
        if analysis:
            with open("learning_analysis.json", "w") as f:
                json.dump(analysis, f, indent=2)
        
        print("\n=== Tutorial Complete ===")
        print("Results saved to:")
        print("  - advanced_simulation_results.json")
        print("  - learning_analysis.json")
        
        return 0
        
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        return 1

if __name__ == "__main__":
    exit(main())
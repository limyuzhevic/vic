#!/bin/bash
# NLM Advanced Command Line Tools

# Usage: nlm_run.py <mode> [options]

cat > nlm_advanced.py << 'EOF'
#!/usr/bin/env python3
"""
Advanced NLM command-line interface for experienced users.

This script provides enhanced functionality for running complex experiments
and managing NLM simulations with advanced features.
"""

import sys
import argparse
import json
import time
from pathlib import Path
import pynlm

def create_advanced_config(args):
    """Create an advanced configuration with preset profiles."""
    config = pynlm.createDefaultConfig()
    
    if args.profile == "exploration":
        # Exploration-focused configuration
        config.set("plasticity.curiosity.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        
    elif args.profile == "exploitation":
        # Exploitation-focused configuration
        config.set("plasticity.curiosity.enable", False)
        config.set("neuromod.curiosity.enable", False)
        config.set("neuromod.novelty.enable", False)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("brain.synapse_density", 0.08)
        
    elif args.profile == "balanced":
        # Balanced configuration
        config.set("plasticity.curiosity.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("brain.synapse_density", 0.12)
    
    # Custom settings
    if args.neurons:
        config.set("brain.neuron_count", args.neurons)
    if args.synapse_density:
        config.set("brain.synapse_density", args.synapse_density)
    if args.learning_rate:
        config.set("plasticity.stdp.learning_rate", args.learning_rate)
    
    return config

def run_advanced_simulation(args):
    """Run an advanced simulation with enhanced monitoring."""
    print(f"Starting advanced simulation with {args.steps} steps...")
    
    # Create configuration
    config = create_advanced_config(args)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableAllFeatures()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=args.width,
        height=args.height,
        visionWidth=args.vision_width,
        visionHeight=args.vision_height
    )
    world.reset()
    world.setAgentStart(args.start_x, args.start_y)
    
    # Initialize agent
    agent.initialize(world)
    
    # Store statistics
    stats = {
        "steps": [],
        "action_distribution": {},
        "performance_metrics": {},
        "brain_stats": []
    }
    
    # Run simulation
    start_time = time.time()
    for step in range(args.steps):
        # Update world
        world.update(args.timestep)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record action
        agent.recordAction(action)
        
        # Get reward and apply modulation
        reward = 0.0
        if not percept.getInternal().empty():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(args.timestep)
        
        # Record statistics every N steps
        if step % args.stats_interval == 0:
            step_stats = {
                "step": step,
                "time": time.time() - start_time,
                "action": str(action),
                "reward": reward,
                "curiosity": agent.getCuriosityLevel(),
                "novelty": agent.getNoveltyLevel(),
                "prediction_error": agent.getPredictionError(),
                "development_stage": str(agent.getDevelopmentalStage()),
                "brain_stats": pynlm.getBrainStatistics(brain)
            }
            stats["steps"].append(step_stats)
            stats["brain_stats"].append(pynlm.getBrainStatistics(brain))
            
            # Print progress
            if args.verbose:
                print(f"Step {step}: Action={action}, Reward={reward:.3f}, "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}")
    
    end_time = time.time()
    
    # Calculate final statistics
    action_dist = {}
    for i in range(8):
        action_type = list(pynlm.MotorCommand)[i]
        ratio = agent.getActionUsageRatio(action_type)
        if ratio > 0:
            action_dist[str(action_type)] = ratio
    
    stats["action_distribution"] = action_dist
    stats["total_time"] = end_time - start_time
    
    # Calculate performance metrics
    if stats["steps"]:
        final_stats = stats["steps"][-1]
        stats["performance_metrics"] = {
            "total_steps": args.steps,
            "total_time": end_time - start_time,
            "steps_per_second": args.steps / (end_time - start_time),
            "average_reward": sum(s["reward"] for s in stats["steps"]) / len(stats["steps"]),
            "final_curiosity": final_stats["curiosity"],
            "final_novelty": final_stats["novelty"],
            "final_prediction_error": final_stats["prediction_error"]
        }
    
    # Save results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(stats, f, indent=2)
        print(f"Results saved to {args.output}")
    
    return stats

def run_experiment_series(args):
    """Run a series of experiments with different configurations."""
    print(f"Starting experiment series: {args.experiment_name}")
    
    experiments = []
    
    for i, config_args in enumerate(args.configurations):
        print(f"\n--- Running Experiment {i+1}/{len(args.configurations)} ---")
        
        # Create configuration
        config = create_advanced_config(config_args)
        
        # Run experiment
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.enableAllFeatures()
        
        world = pynlm.createSimpleWorld()
        world.configure(
            width=config_args.width if hasattr(config_args, 'width') else 20,
            height=config_args.height if hasattr(config_args, 'height') else 20,
            visionWidth=config_args.vision_width if hasattr(config_args, 'vision_width') else 8,
            visionHeight=config_args.vision_height if hasattr(config_args, 'vision_height') else 8
        )
        world.reset()
        world.setAgentStart(10, 10)
        agent.initialize(world)
        
        # Run simulation
        for step in range(config_args.steps):
            world.update(config_args.timestep)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            agent.applyRewardModulation(0.0, 0.0)
            agent.updateDevelopment(config_args.timestep)
        
        # Record results
        exp_results = {
            "experiment_id": i,
            "configuration": config_args.__dict__ if hasattr(config_args, '__dict__') else str(config_args),
            "final_curiosity": agent.getCuriosityLevel(),
            "final_novelty": agent.getNoveltyLevel(),
            "final_prediction_error": agent.getPredictionError(),
            "brain_stats": pynlm.getBrainStatistics(brain)
        }
        experiments.append(exp_results)
        
        print(f"Experiment {i+1} completed: "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}")
    
    # Save experiment results
    if args.output:
        with open(args.output, 'w') as f:
            json.dump({
                "experiment_name": args.experiment_name,
                "experiments": experiments
            }, f, indent=2)
        print(f"Experiment series saved to {args.output}")
    
    return experiments

def main():
    parser = argparse.ArgumentParser(
        description="Advanced NLM command-line interface"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Advanced simulation command
    sim_parser = subparsers.add_parser("run", help="Run advanced simulation")
    sim_parser.add_argument("--steps", type=int, default=1000,
                           help="Number of simulation steps")
    sim_parser.add_argument("--timestep", type=float, default=0.1,
                           help="Simulation timestep")
    sim_parser.add_argument("--width", type=int, default=20,
                           help="World width")
    sim_parser.add_argument("--height", type=int, default=20,
                           help="World height")
    sim_parser.add_argument("--vision-width", type=int, default=8,
                           help="Vision width")
    sim_parser.add_argument("--vision-height", type=int, default=8,
                           help="Vision height")
    sim_parser.add_argument("--start-x", type=float, default=10.0,
                           help="Agent starting x position")
    sim_parser.add_argument("--start-y", type=float, default=10.0,
                           help="Agent starting y position")
    sim_parser.add_argument("--profile", choices=["exploration", "exploitation", "balanced"],
                           default="balanced", help="Configuration profile")
    sim_parser.add_argument("--neurons", type=int, help="Number of neurons")
    sim_parser.add_argument("--synapse-density", type=float, help="Synapse density")
    sim_parser.add_argument("--learning-rate", type=float, help="STDP learning rate")
    sim_parser.add_argument("--stats-interval", type=int, default=100,
                           help="Interval for recording statistics")
    sim_parser.add_argument("--output", help="Output file for results")
    sim_parser.add_argument("--verbose", action="store_true",
                           help="Verbose output")
    
    # Experiment series command
    exp_parser = subparsers.add_parser("experiment", help="Run experiment series")
    exp_parser.add_argument("experiment_name", help="Name of the experiment series")
    exp_parser.add_argument("--output", help="Output file for experiment results")
    
    # Configuration command
    config_parser = subparsers.add_parser("config", help="Generate configuration")
    config_parser.add_argument("--profile", choices=["exploration", "exploitation", "balanced"],
                              default="balanced", help="Configuration profile")
    config_parser.add_argument("--neurons", type=int, help="Number of neurons")
    config_parser.add_argument("--synapse-density", type=float, help="Synapse density")
    config_parser.add_argument("--learning-rate", type=float, help="STDP learning rate")
    config_parser.add_argument("--output", help="Output configuration to file")
    
    args = parser.parse_args()
    
    if args.command == "run":
        run_advanced_simulation(args)
    elif args.command == "experiment":
        run_experiment_series(args)
    elif args.command == "config":
        config = create_advanced_config(args)
        print("Configuration generated:")
        print(config.summary())
        if args.output:
            # Save configuration to JSON
            import json
            config_dict = {}
            # Extract all config keys
            keys = config.getKeys()
            for key in keys:
                config_dict[key] = config.get(key)
            with open(args.output, 'w') as f:
                json.dump(config_dict, f, indent=2)
            print(f"Configuration saved to {args.output}")
    
    print("\nAdvanced NLM completed successfully!")

if __name__ == "__main__":
    main()
EOF
chmod +x nlm_advanced.py

#!/usr/bin/env python3
"""
NLM Command Line Utilities

Command-line interface for NLM (Neural Learning Machine) operations.
Provides easy access to brain simulation, experimentation, and analysis.

Usage:
    nlm-cli <command> [options]

Commands:
    brain              Brain simulation commands
    experiment         Experiment and research commands
    config             Configuration management
    analyze            Analysis and statistics
    help               Show help information
"""

import sys
import os
import argparse
import json
import time
from pathlib import Path

# Add the current directory to the path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def brain_command(args):
    """Brain simulation commands"""
    if args.command == "simulate":
        simulate_brain(args)
    elif args.command == "train":
        train_agent(args)
    elif args.command == "stats":
        show_stats(args)

def experiment_command(args):
    """Experiment and research commands"""
    if args.command == "param-sweep":
        parameter_sweep(args)
    elif args.command == "quick-test":
        quick_test(args)
    elif args.command == "batch":
        batch_experiment(args)

def config_command(args):
    """Configuration management"""
    if args.command == "create":
        create_config(args)
    elif args.command == "validate":
        validate_config(args)
    elif args.command == "list":
        list_configs(args)

def analyze_command(args):
    """Analysis and statistics"""
    if args.command == "brain":
        analyze_brain(args)
    elif args.command == "simulation":
        analyze_simulation(args)

def simulate_brain(args):
    """Simulate a brain with specified parameters"""
    print(f"Simulating brain with {args.neurons} neurons for {args.steps} steps...")
    
    # Create configuration
    config = pynlm.ConfigWrapper()
    config.configureBrain(neuronCount=args.neurons, connectionProbability=args.conn_prob)
    
    # Create brain
    brain = pynlm.BrainWrapper(config.getConfig())
    
    if not brain.initialize():
        print("Error: Failed to initialize brain")
        return
    
    # Run simulation
    start_time = time.time()
    brain.runSimulation(args.steps, args.timestep)
    end_time = time.time()
    
    # Show results
    stats = brain.getStatistics()
    print(f"\nSimulation Results:")
    print(f"  Duration: {end_time - start_time:.2f} seconds")
    print(f"  Total neurons: {stats['total_neurons']}")
    print(f"  Total synapses: {stats['total_synapses']}")
    print(f"  Firing neurons: {stats['firing_neurons']}")
    print(f"  Total spikes: {stats['total_spikes']}")
    print(f"  Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
    print(f"  E/I ratio: {stats['e_i_ratio']:.3f}")
    
    # Save results if requested
    if args.output:
        results = {
            "neurons": args.neurons,
            "steps": args.steps,
            "timestep": args.timestep,
            "connection_probability": args.conn_prob,
            "duration": end_time - start_time,
            "statistics": stats
        }
        with open(args.output, "w") as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to {args.output}")

def train_agent(args):
    """Train an agent in a world"""
    print(f"Training agent with {args.neurons} neurons for {args.steps} steps...")
    
    # Create configuration
    config = pynlm.ConfigWrapper()
    config.configureBrain(neuronCount=args.neurons)
    config.configureWorld(width=args.width, height=args.height, 
                         visionWidth=args.vision_width, visionHeight=args.vision_height)
    
    # Create brain
    brain = pynlm.BrainWrapper(config.getConfig())
    if not brain.initialize():
        print("Error: Failed to initialize brain")
        return
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(args.width, args.height, args.vision_width, args.vision_height)
    world.reset()
    world.setAgentStart(args.start_x, args.start_y)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain.getBrain())
    agent.initialize(world)
    
    # Enable learning
    if args.reward:
        agent.enableRewardModulation(True)
    if args.curiosity:
        agent.enableCuriosity(True)
    if args.development:
        agent.enableDevelopment(True)
    
    print(f"Initial development stage: {agent.getDevelopmentalStage()}")
    print(f"Initial curiosity level: {agent.getCuriosityLevel():.3f}")
    
    # Training loop
    for step in range(args.steps):
        world.update(args.timestep)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        internal_signals = percept.getInternal()
        reward = internal_signals[0] if internal_signals else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(args.timestep)
        
        if step % args.log_interval == 0:
            print(f"Step {step}: {brain.getStatistics()['firing_neurons']} firing neurons, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")
    
    print(f"\nTraining completed!")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Final development stage: {agent.getDevelopmentalStage()}")
    
    # Save model if requested
    if args.save_model:
        brain.saveState(args.save_model)
        print(f"Model saved to {args.save_model}")

def show_stats(args):
    """Show brain statistics"""
    print("Showing brain statistics...")
    
    # Create brain
    config = pynlm.ConfigWrapper()
    config.configureBrain(neuronCount=1000)
    
    brain = pynlm.BrainWrapper(config.getConfig())
    if not brain.initialize():
        print("Error: Failed to initialize brain")
        return
    
    brain.runSimulation(100)
    
    print("\nBrain Statistics:")
    stats = brain.getStatistics()
    for key, value in stats.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.2f}")
        else:
            print(f"  {key}: {value}")

def parameter_sweep(args):
    """Parameter sweep experiment"""
    print(f"Running parameter sweep: {args.parameter} values {args.values}")
    
    results = []
    
    for value in args.values:
        print(f"Testing {args.parameter} = {value}...")
        
        # Create configuration
        config = pynlm.ConfigWrapper()
        config.configureBrain(neuronCount=1000)
        
        if args.parameter == "connection_probability":
            config.setFloat("brain.connection_probability", float(value))
        elif args.parameter == "neuron_count":
            config.setInt("brain.neuron_count", int(value))
        
        # Create and run brain
        brain = pynlm.BrainWrapper(config.getConfig())
        if brain.initialize():
            brain.runSimulation(50)
            
            stats = brain.getStatistics()
            result = {
                args.parameter: value,
                "total_spikes": stats["total_spikes"],
                "avg_firing_rate": stats["avg_firing_rate"]
            }
            
            results.append(result)
            print(f"  -> {args.parameter}={value}: {stats['total_spikes']} spikes")
        else:
            print(f"  -> Failed to initialize brain")
            results.append({
                args.parameter: value,
                "error": "initialization_failed"
            })
    
    # Save results
    if args.output:
        with open(args.output, "w") as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to {args.output}")
    
    # Find best value
    successful_results = [r for r in results if "error" not in r]
    if successful_results:
        best_result = max(successful_results, key=lambda r: r["avg_firing_rate"])
        print(f"\nBest {args.parameter}: {best_result[args.parameter]} "
              f"(firing rate: {best_result['avg_firing_rate']:.2f} Hz)")

def quick_test(args):
    """Run a quick system test"""
    print("Running quick NLM system test...")
    
    try:
        result = pynlm.runQuickTest()
        print("Quick test completed successfully!")
        for key, value in result.items():
            print(f"  {key}: {value}")
        
        if args.output:
            with open(args.output, "w") as f:
                json.dump(result, f, indent=2)
            print(f"Test results saved to {args.output}")
            
    except Exception as e:
        print(f"Quick test failed: {e}")
        if args.output:
            error_result = {"error": str(e), "status": "failed"}
            with open(args.output, "w") as f:
                json.dump(error_result, f, indent=2)

def batch_experiment(args):
    """Run batch experiment"""
    print(f"Running batch experiment with {args.num_runs} runs...")
    
    all_results = []
    
    for run in range(args.num_runs):
        print(f"Run {run + 1}/{args.num_runs}...")
        
        # Create configuration
        config = pynlm.ConfigWrapper()
        config.configureBrain(neuronCount=args.neurons)
        
        # Create and run brain
        brain = pynlm.BrainWrapper(config.getConfig())
        if brain.initialize():
            brain.runSimulation(args.steps)
            
            stats = brain.getStatistics()
            result = {
                "run": run + 1,
                "total_spikes": stats["total_spikes"],
                "avg_firing_rate": stats["avg_firing_rate"],
                "duration": time.time() - start_time
            }
            
            all_results.append(result)
        else:
            print(f"  -> Run {run + 1} failed")
            all_results.append({
                "run": run + 1,
                "error": "initialization_failed"
            })
    
    # Calculate statistics
    successful_results = [r for r in all_results if "error" not in r]
    if successful_results:
        avg_spikes = sum(r["total_spikes"] for r in successful_results) / len(successful_results)
        avg_firing_rate = sum(r["avg_firing_rate"] for r in successful_results) / len(successful_results)
        
        print(f"\nBatch experiment results:")
        print(f"  Successful runs: {len(successful_results)}/{args.num_runs}")
        print(f"  Average spikes: {avg_spikes:.1f}")
        print(f"  Average firing rate: {avg_firing_rate:.2f} Hz")
    
    # Save results
    if args.output:
        with open(args.output, "w") as f:
            json.dump(all_results, f, indent=2)
        print(f"Results saved to {args.output}")

def create_config(args):
    """Create a new configuration file"""
    print(f"Creating configuration file: {args.output}")
    
    config = pynlm.ConfigWrapper()
    config.configureBrain(neuronCount=args.neurons, connectionProbability=args.conn_prob)
    config.configureWorld(width=args.width, height=args.height, 
                         visionWidth=args.vision_width, visionHeight=args.vision_height)
    
    config.getConfig().saveToFile(args.output)
    print(f"Configuration saved to {args.output}")

def validate_config(args):
    """Validate a configuration file"""
    print(f"Validating configuration file: {args.file}")
    
    try:
        validation_result = pynlm.validateConfiguration(args.file)
        print(f"Validation result:")
        print(f"  Valid: {validation_result['valid']}")
        print(f"  Has neuron count: {validation_result['has_neuron_count']}")
        print(f"  Has connection probability: {validation_result['has_connection_probability']}")
        
        if not validation_result['valid']:
            print("  Configuration is invalid!")
            return 1
            
    except Exception as e:
        print(f"Error validating configuration: {e}")
        return 1
    
    return 0

def list_configs(args):
    """List available configurations"""
    print("Available configurations:")
    
    config_dir = Path("configs")
    if not config_dir.exists():
        print("  No configs directory found")
        return
    
    for config_file in config_dir.glob("*.cfg"):
        print(f"  {config_file.name}")

def analyze_brain(args):
    """Analyze brain performance"""
    print("Analyzing brain performance...")
    
    config = pynlm.ConfigWrapper()
    config.configureBrain(neuronCount=1000)
    
    brain = pynlm.BrainWrapper(config.getConfig())
    if not brain.initialize():
        print("Error: Failed to initialize brain")
        return
    
    brain.runSimulation(args.steps)
    
    print("\nBrain Analysis:")
    stats = brain.getStatistics()
    
    # Calculate additional metrics
    total_energy = stats['total_neurons'] * 0.001  # Simplified energy estimate
    firing_efficiency = stats['firing_neurons'] / stats['total_neurons'] * 100
    
    print(f"  Population activity: {firing_efficiency:.1f}%")
    print(f"  Energy efficiency: {total_energy:.2f} (arbitrary units)")
    print(f"  Network complexity: {stats['total_synapses']} synapses")
    
    # Save analysis
    if args.output:
        analysis = {
            "statistics": stats,
            "efficiency": {
                "firing_efficiency": firing_efficiency,
                "total_energy": total_energy,
                "network_complexity": stats['total_synapses']
            }
        }
        with open(args.output, "w") as f:
            json.dump(analysis, f, indent=2)
        print(f"Analysis saved to {args.output}")

def analyze_simulation(args):
    """Analyze simulation data"""
    print("Analyzing simulation data...")
    
    if not args.file:
        print("Error: No simulation data file specified")
        return
    
    try:
        with open(args.file, "r") as f:
            data = json.load(f)
        
        print(f"Loaded simulation data from {args.file}")
        print(f"Data keys: {list(data.keys())}")
        
        if "statistics" in data:
            print("\nSimulation Statistics:")
            for key, value in data["statistics"].items():
                if isinstance(value, float):
                    print(f"  {key}: {value:.2f}")
                else:
                    print(f"  {key}: {value}")
        
    except Exception as e:
        print(f"Error analyzing simulation data: {e}")

def main():
    parser = argparse.ArgumentParser(description="NLM Command Line Interface")
    parser.add_argument("command", choices=["brain", "experiment", "config", "analyze", "help"],
                       help="Command to execute")
    parser.add_argument("-h", "--help", action="store_true", help="Show help information")
    
    subparsers = parser.add_subparsers(dest="subcommand")
    
    # Brain commands
    brain_parser = subparsers.add_parser("brain", help="Brain simulation commands")
    brain_subparsers = brain_parser.add_subparsers(dest="brain_command")
    
    simulate_parser = brain_subparsers.add_parser("simulate", help="Simulate brain")
    simulate_parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    simulate_parser.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    simulate_parser.add_argument("--timestep", type=float, default=0.1, help="Simulation timestep")
    simulate_parser.add_argument("--conn-prob", type=float, default=0.1, help="Connection probability")
    simulate_parser.add_argument("--output", help="Output file for results")
    
    train_parser = brain_subparsers.add_parser("train", help="Train agent")
    train_parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    train_parser.add_argument("--steps", type=int, default=200, help="Number of training steps")
    train_parser.add_argument("--width", type=int, default=20, help="World width")
    train_parser.add_argument("--height", type=int, default=20, help="World height")
    train_parser.add_argument("--vision-width", type=int, default=8, help="Vision width")
    train_parser.add_argument("--vision-height", type=int, default=8, help="Vision height")
    train_parser.add_argument("--start-x", type=float, default=10.0, help="Agent start X")
    train_parser.add_argument("--start-y", type=float, default=10.0, help="Agent start Y")
    train_parser.add_argument("--timestep", type=float, default=0.1, help="Simulation timestep")
    train_parser.add_argument("--reward", action="store_true", help="Enable reward modulation")
    train_parser.add_argument("--curiosity", action="store_true", help="Enable curiosity")
    train_parser.add_argument("--development", action="store_true", help="Enable development")
    train_parser.add_argument("--log-interval", type=int, default=50, help="Log interval")
    train_parser.add_argument("--save-model", help="Save model to file")
    
    stats_parser = brain_subparsers.add_parser("stats", help="Show brain statistics")
    
    # Experiment commands
    experiment_parser = subparsers.add_parser("experiment", help="Experiment commands")
    experiment_subparsers = experiment_parser.add_subparsers(dest="experiment_command")
    
    param_parser = experiment_subparsers.add_parser("param-sweep", help="Parameter sweep")
    param_parser.add_argument("--parameter", choices=["neuron_count", "connection_probability"], 
                            required=True, help="Parameter to sweep")
    param_parser.add_argument("--values", nargs="+", required=True, help="Values to test")
    param_parser.add_argument("--output", help="Output file for results")
    
    quick_test_parser = experiment_subparsers.add_parser("quick-test", help="Quick system test")
    quick_test_parser.add_argument("--output", help="Output file for test results")
    
    batch_parser = experiment_subparsers.add_parser("batch", help="Batch experiment")
    batch_parser.add_argument("--num-runs", type=int, default=10, help="Number of runs")
    batch_parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    batch_parser.add_argument("--steps", type=int, default=100, help="Number of steps")
    batch_parser.add_argument("--output", help="Output file for results")
    
    # Config commands
    config_parser = subparsers.add_parser("config", help="Configuration commands")
    config_subparsers = config_parser.add_subparsers(dest="config_command")
    
    create_parser = config_subparsers.add_parser("create", help="Create configuration")
    create_parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    create_parser.add_argument("--conn-prob", type=float, default=0.1, help="Connection probability")
    create_parser.add_argument("--width", type=int, default=20, help="World width")
    create_parser.add_argument("--height", type=int, default=20, help="World height")
    create_parser.add_argument("--vision-width", type=int, default=8, help="Vision width")
    create_parser.add_argument("--vision-height", type=int, default=8, help="Vision height")
    create_parser.add_argument("--output", required=True, help="Output file")
    
    validate_parser = config_subparsers.add_parser("validate", help="Validate configuration")
    validate_parser.add_argument("file", help="Configuration file")
    
    list_parser = config_subparsers.add_parser("list", help="List configurations")
    
    # Analyze commands
    analyze_parser = subparsers.add_parser("analyze", help="Analysis commands")
    analyze_subparsers = analyze_parser.add_subparsers(dest="analyze_command")
    
    brain_analyze_parser = analyze_subparsers.add_parser("brain", help="Analyze brain")
    brain_analyze_parser.add_argument("--steps", type=int, default=100, help="Number of steps")
    brain_analyze_parser.add_argument("--output", help="Output file for analysis")
    
    simulation_analyze_parser = analyze_subparsers.add_parser("simulation", help="Analyze simulation data")
    simulation_analyze_parser.add_argument("file", help="Simulation data file")
    
    # Help command
    help_parser = subparsers.add_parser("help", help="Show help")
    
    # Parse arguments
    args = parser.parse_args()
    
    if args.help:
        parser.print_help()
        return
    
    # Execute command
    if args.command == "brain":
        brain_command(args)
    elif args.command == "experiment":
        experiment_command(args)
    elif args.command == "config":
        config_command(args)
    elif args.command == "analyze":
        analyze_command(args)
    elif args.command == "help":
        parser.print_help()

if __name__ == "__main__":
    main()

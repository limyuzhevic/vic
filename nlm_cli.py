#!/usr/bin/env python3
"""
NLM Command Line Interface

A command-line tool for interacting with NLM (Neural Learning Machine) simulations.
Provides convenient commands for running experiments, managing configurations, and exploring results.
"""

import argparse
import sys
import json
import time
from typing import Optional, List, Dict, Any
import pynlm


def main():
    parser = argparse.ArgumentParser(
        prog="pynlm",
        description="NLM (Neural Learning Machine) - Experimental Brain Simulation",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  pynlm create --neurons 500 --steps 1000
  pynlm run --config config.json --output results.json
  pynlm analyze --file results.json --plot
  pynlm learn --duration 300 --pattern sensory
        """
    )
    
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--log-level", "-l",
        choices=["DEBUG", "INFO", "WARNING", "ERROR"],
        default="INFO",
        help="Set logging level"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Create command
    create_parser = subparsers.add_parser(
        "create",
        help="Create and run a simple brain simulation"
    )
    create_parser.add_argument(
        "--neurons", "-n",
        type=int,
        default=100,
        help="Number of neurons in the brain"
    )
    create_parser.add_argument(
        "--steps", "-s",
        type=int,
        default=100,
        help="Number of simulation steps"
    )
    create_parser.add_argument(
        "--type", "-t",
        choices=["simple", "agent", "learning"],
        default="simple",
        help="Type of simulation to create"
    )
    create_parser.add_argument(
        "--output", "-o",
        help="Output file for simulation results"
    )
    
    # Run command
    run_parser = subparsers.add_parser(
        "run",
        help="Run an NLM simulation with custom configuration"
    )
    run_parser.add_argument(
        "--config", "-c",
        help="Configuration file path"
    )
    run_parser.add_argument(
        "--steps", "-s",
        type=int,
        default=500,
        help="Number of simulation steps"
    )
    run_parser.add_argument(
        "--output", "-o",
        help="Output file for results"
    )
    run_parser.add_argument(
        "--record", "-r",
        action="store_true",
        help="Record simulation metrics"
    )
    
    # Analyze command
    analyze_parser = subparsers.add_parser(
        "analyze",
        help="Analyze simulation results"
    )
    analyze_parser.add_argument(
        "--file", "-f",
        required=True,
        help="Results file to analyze"
    )
    analyze_parser.add_argument(
        "--format", "-f",
        choices=["json", "csv"],
        default="json",
        help="Input file format"
    )
    analyze_parser.add_argument(
        "--plot", "-p",
        action="store_true",
        help="Generate plots"
    )
    analyze_parser.add_argument(
        "--stats", "-s",
        action="store_true",
        help="Show detailed statistics"
    )
    
    # Learn command
    learn_parser = subparsers.add_parser(
        "learn",
        help="Run a learning experiment"
    )
    learn_parser.add_argument(
        "--duration", "-d",
        type=float,
        default=60.0,
        help="Learning duration in seconds"
    )
    learn_parser.add_argument(
        "--pattern", "-p",
        choices=["sensory", "excitatory", "inhibitory", "internal", "mixed"],
        default="mixed",
        help="Input pattern to use for learning"
    )
    learn_parser.add_argument(
        "--steps", "-s",
        type=int,
        default=1000,
        help="Number of learning steps"
    )
    learn_parser.add_argument(
        "--output", "-o",
        help="Output file for learning results"
    )
    
    # Config command
    config_parser = subparsers.add_parser(
        "config",
        help="Manage configurations"
    )
    config_subparsers = config_parser.add_subparsers(dest="action", help="Config actions")
    
    # Config create
    config_create = config_subparsers.add_parser(
        "create",
        help="Create a new configuration"
    )
    config_create.add_argument(
        "name",
        help="Name of the configuration"
    )
    config_create.add_argument(
        "--neurons", "-n",
        type=int,
        default=100,
        help="Number of neurons"
    )
    
    # Config list
    config_list = config_subparsers.add_parser(
        "list",
        help="List saved configurations"
    )
    
    # Config show
    config_show = config_subparsers.add_parser(
        "show",
        help="Show configuration details"
    )
    config_show.add_argument(
        "name",
        help="Name of the configuration to show"
    )
    
    # Config save
    config_save = config_subparsers.add_parser(
        "save",
        help="Save current configuration"
    )
    config_save.add_argument(
        "name",
        help="Name to save the configuration as"
    )
    
    # Version
    version_parser = subparsers.add_parser(
        "version",
        help="Show version information"
    )
    
    # Help
    help_parser = subparsers.add_parser(
        "help",
        help="Show help information"
    )
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    try:
        if args.command == "create":
            create_simulation(args)
        elif args.command == "run":
            run_simulation(args)
        elif args.command == "analyze":
            analyze_results(args)
        elif args.command == "learn":
            run_learning(args)
        elif args.command == "config":
            manage_config(args)
        elif args.command == "version":
            show_version()
        elif args.command == "help":
            parser.print_help()
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
        return 1
    except Exception as e:
        print(f"Error: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1
    
    return 0


def create_simulation(args):
    """Create and run a simple brain simulation"""
    print(f"Creating {args.type} simulation with {args.neurons} neurons for {args.steps} steps")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.setFloat("neuron_count", args.neurons)
    config.setFloat("region_count", 2)
    config.setFloat("connection_probability", 0.15)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    start_time = time.time()
    for step in range(args.steps):
        # Inject some input
        if step % 4 == 0:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
        elif step % 4 == 1:
            brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 3.0)
        
        brain.step(step)
        
        if args.verbose and step % 100 == 0:
            print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.2f}")
    
    end_time = time.time()
    
    # Print results
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    # Save results if requested
    if args.output:
        results = {
            "simulation_type": args.type,
            "neurons": args.neurons,
            "steps": args.steps,
            "duration": end_time - start_time,
            "final_stats": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "total_spikes": brain.getTotalSpikeCount(),
            }
        }
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to {args.output}")


def run_simulation(args):
    """Run an NLM simulation with custom configuration"""
    print(f"Running simulation with {args.steps} steps")
    
    if args.config:
        # Load configuration from file
        try:
            with open(args.config, 'r') as f:
                config_data = json.load(f)
            
            config = pynlm.createDefaultConfig()
            for key, value in config_data.items():
                if isinstance(value, int):
                    config.setInt(key, value)
                elif isinstance(value, float):
                    config.setFloat(key, value)
                elif isinstance(value, str):
                    config.setString(key, value)
                elif isinstance(value, bool):
                    config.setBool(key, value)
        except FileNotFoundError:
            print(f"Configuration file not found: {args.config}")
            return
        except json.JSONDecodeError as e:
            print(f"Invalid JSON in configuration file: {e}")
            return
    else:
        # Use default configuration
        config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    if args.record:
        print("Recording simulation metrics...")
        metrics = []
        
        for step in range(args.steps):
            # Simple input pattern
            if step % 5 == 0:
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
            
            brain.step(step)
            
            if step % 100 == 0:
                print(f"Step {step}/{args.steps}")
                
                metric = {
                    "step": step,
                    "firing_rate": brain.getAverageFiringRate(),
                    "firing_neurons": brain.getFiringNeuronCount(),
                    "total_spikes": brain.getTotalSpikeCount(),
                    "time": time.time()
                }
                metrics.append(metric)
        
        # Save metrics if requested
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(metrics, f, indent=2)
            print(f"Recorded metrics saved to {args.output}")
    else:
        # Run without recording
        for step in range(args.steps):
            if step % 500 == 0:
                print(f"Step {step}/{args.steps}")
            
            brain.step(step)
    
    # Print final statistics
    print(f"\nSimulation completed!")
    print(f"Final statistics:")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")


def analyze_results(args):
    """Analyze simulation results"""
    print(f"Analyzing results from {args.file}")
    
    try:
        with open(args.file, 'r') as f:
            if args.format == "json":
                data = json.load(f)
            else:  # CSV
                import csv
                reader = csv.DictReader(f)
                data = list(reader)
        
        if not data:
            print("No data found in file")
            return
        
        print(f"Loaded {len(data)} data points")
        
        if args.stats:
            # Calculate and print statistics
            if isinstance(data, list) and len(data) > 0:
                if "firing_rate" in data[0]:
                    rates = [d["firing_rate"] for d in data if "firing_rate" in d]
                    if rates:
                        print(f"Firing rate statistics:")
                        print(f"  Mean: {sum(rates)/len(rates):.2f}")
                        print(f"  Min: {min(rates):.2f}")
                        print(f"  Max: {max(rates):.2f}")
                        print(f"  Std: {(sum((x - sum(rates)/len(rates))**2 for x in rates)/len(rates))**0.5:.2f}")
                
                if "total_spikes" in data[0]:
                    spikes = [d["total_spikes"] for d in data if "total_spikes" in d]
                    if spikes:
                        print(f"Spike statistics:")
                        print(f"  Total: {sum(spikes)}")
                        print(f"  Average per step: {sum(spikes)/len(spikes):.1f}")
        
        if args.plot:
            print("Plotting not implemented yet (requires matplotlib)")
            try:
                import matplotlib.pyplot as plt
                
                if isinstance(data, list) and len(data) > 0:
                    if "step" in data[0] and "firing_rate" in data[0]:
                        steps = [d["step"] for d in data]
                        rates = [d["firing_rate"] for d in data]
                        
                        plt.figure(figsize=(12, 6))
                        plt.subplot(2, 1, 1)
                        plt.plot(steps, rates, 'b-')
                        plt.xlabel("Step")
                        plt.ylabel("Firing Rate")
                        plt.title("Firing Rate Over Time")
                        plt.grid(True)
                        
                        plt.subplot(2, 1, 2)
                        plt.hist(rates, bins=50, alpha=0.7, color='green')
                        plt.xlabel("Firing Rate")
                        plt.ylabel("Frequency")
                        plt.title("Firing Rate Distribution")
                        plt.grid(True)
                        
                        plt.tight_layout()
                        plt.savefig(f"{args.file}_analysis.png", dpi=150)
                        plt.close()
                        print(f"Plot saved as {args.file}_analysis.png")
            except ImportError:
                print("Matplotlib not available - skipping plot generation")
        
    except FileNotFoundError:
        print(f"File not found: {args.file}")
    except json.JSONDecodeError:
        print(f"Invalid JSON format in file: {args.file}")
    except Exception as e:
        print(f"Error analyzing file: {e}")


def run_learning(args):
    """Run a learning experiment"""
    print(f"Running learning experiment with {args.steps} steps")
    print(f"Pattern type: {args.pattern}")
    print(f"Duration: {args.duration} seconds")
    
    # Create configuration optimized for learning
    config = pynlm.createDefaultConfig()
    config.setFloat("plasticity_learning_rate", 0.05)
    config.setFloat("stdp_ltp_weight", 0.02)
    config.setFloat("stdp_ltd_weight", 0.015)
    config.setBool("long_term_potentiation", True)
    config.setBool("long_term_depression", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Starting learning experiment...")
    start_time = time.time()
    
    # Generate input pattern based on type
    for step in range(args.steps):
        if args.pattern == "sensory":
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
        elif args.pattern == "excitatory":
            brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 5.0 + (step % 10) * 0.1)
        elif args.pattern == "inhibitory":
            brain.injectCurrentToNeurons(pynlm.NeuronType.Inhibitory, 3.0 + (step % 5) * 0.2)
        elif args.pattern == "internal":
            brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, 2.0 + (step % 3))
        else:  # mixed
            # Random pattern
            import random
            pattern = random.choice(["sensory", "excitatory", "inhibitory", "internal"])
            current = random.uniform(2.0, 10.0)
            if pattern == "sensory":
                brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, current)
            elif pattern == "excitatory":
                brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, current)
            elif pattern == "inhibitory":
                brain.injectCurrentToNeurons(pynlm.NeuronType.Inhibitory, current)
            else:
                brain.injectCurrentToNeurons(pynlm.NeuronType.Internal, current)
        
        brain.step(step)
        
        if step % 200 == 0:
            print(f"Step {step}/{args.steps}: Firing rate = {brain.getAverageFiringRate():.2f}")
    
    end_time = time.time()
    
    # Print learning results
    print(f"\nLearning experiment completed in {end_time - start_time:.2f} seconds")
    print(f"Final learning statistics:")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Excitatory/inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
    print(f"  Learning rate: {brain.getLearningRate():.4f}")
    
    # Save results if requested
    if args.output:
        results = {
            "pattern": args.pattern,
            "steps": args.steps,
            "duration": end_time - start_time,
            "final_stats": {
                "average_firing_rate": brain.getAverageFiringRate(),
                "total_spikes": brain.getTotalSpikeCount(),
                "excitatory_inhibitory_ratio": brain.getExcitationInhibitionRatio(),
                "learning_rate": brain.getLearningRate(),
                "plasticity_enabled": brain.getPlasticityEnabled(),
            }
        }
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Learning results saved to {args.output}")


def manage_config(args):
    """Manage configurations"""
    if not args.action:
        print("Config action required. Use 'pynlm config --help' for available actions.")
        return
    
    print(f"Config action: {args.action}")
    
    # Implementation would depend on configuration management
    if args.action == "create":
        print(f"Creating configuration: {args.name}")
        # Implementation here
    elif args.action == "list":
        print("Listing configurations...")
        # Implementation here
    elif args.action == "show":
        print(f"Showing configuration: {args.name}")
        # Implementation here
    elif args.action == "save":
        print(f"Saving configuration as: {args.name}")
        # Implementation here


def show_version():
    """Show version information"""
    print("NLM (Neural Learning Machine) Command Line Interface")
    print("Version: 0.1.0")
    print("Phase 6: Final Integration")
    print("Built with pybind11 and scikit-build")
    print("\nFor more information, visit: https://github.com/nlm-project/nlm")


if __name__ == "__main__":
    sys.exit(main())
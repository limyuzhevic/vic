#!/usr/bin/env python3
"""
NLM Command Line Interface

This provides a command-line interface for common NLM operations
and advanced features for power users.
"""

import argparse
import sys
import json
import pynlm
from pathlib import Path

def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run a simulation with visualization
  nlm-demo --size medium --steps 1000 --visualize

  # Compare different brain configurations
  nlm-run compare --configs small,medium,large --steps 500

  # Export brain data for analysis
  nlm-run export brain_data.bin --size large

  # Run batch of experiments
  nlm-run batch --experiment curiosity --runs 20 --steps 100
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Demo command
    demo_parser = subparsers.add_parser("demo", help="Run a simulation demo")
    demo_parser.add_argument("--size", choices=["small", "medium", "large"], default="medium",
                           help="Brain size preset (default: medium)")
    demo_parser.add_argument("--steps", type=int, default=100,
                           help="Number of simulation steps (default: 100)")
    demo_parser.add_argument("--visualize", action="store_true",
                           help="Enable real-time visualization")
    demo_parser.add_argument("--output", help="Output file for brain data")
    
    # Run command
    run_parser = subparsers.add_parser("run", help="Run various NLM operations")
    run_subparsers = run_parser.add_subparsers(dest="run_command", help="Run operations")
    
    # Compare operation
    compare_parser = run_subparsers.add_parser("compare", help="Compare multiple brain configurations")
    compare_parser.add_argument("--configs", required=True,
                              help="Comma-separated list of configurations (small,medium,large)")
    compare_parser.add_argument("--steps", type=int, default=100,
                              help="Steps per configuration (default: 100)")
    compare_parser.add_argument("--output", help="JSON output file for comparison results")
    
    # Export operation
    export_parser = run_subparsers.add_parser("export", help="Export brain data")
    export_parser.add_argument("output_file", help="Output file path")
    export_parser.add_argument("--size", choices=["small", "medium", "large"], default="medium",
                             help="Brain size (default: medium)")
    export_parser.add_argument("--steps", type=int, default=100,
                             help="Simulation steps before export (default: 100)")
    
    # Batch operation
    batch_parser = run_subparsers.add_parser("batch", help="Run batch of experiments")
    batch_parser.add_argument("--experiment", required=True,
                            help="Experiment type (curiosity, development, prediction, memory)")
    batch_parser.add_argument("--runs", type=int, default=10,
                            help="Number of runs (default: 10)")
    batch_parser.add_argument("--steps", type=int, default=100,
                            help="Steps per run (default: 100)")
    batch_parser.add_argument("--output", help="JSON output file for batch results")
    
    # Analyze operation
    analyze_parser = run_subparsers.add_parser("analyze", help="Analyze simulation results")
    analyze_parser.add_argument("input_file", help="Input brain data file")
    analyze_parser.add_argument("--stats", action="store_true",
                              help="Show detailed statistics")
    analyze_parser.add_argument("--visualize", action="store_true",
                              help="Generate visualization")
    
    # Process parsed arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 0
    
    # Handle demo command
    if args.command == "demo":
        return run_demo(args)
    
    # Handle run command
    if args.command == "run":
        return run_operation(args)
    
    return 0

def run_demo(args):
    """Run a simulation demo"""
    print(f"Running NLM demo with {args.size} brain for {args.steps} steps...")
    
    # Create brain with specified size
    brain = pynlm.utils.create_brain_with_preset(args.size)
    
    if args.visualize:
        print("Starting visualization...")
        # This would start visualization in a real implementation
        # For now, we'll just run without visualization
        pass
    
    # Run simulation
    brain.runEpisode(args.steps)
    
    # Get statistics
    stats = brain.getStats()
    print(f"\nSimulation completed!")
    print(f"Brain size: {stats['total_neurons']} neurons, {stats['total_synapses']} synapses")
    print(f"Total spikes: {stats['total_spikes']}")
    print(f"Firing neurons: {stats['firing_neurons']}")
    print(f"Average firing rate: {stats['average_firing_rate']:.3f}")
    print(f"Developmental stage: {stats['developmental_stage']}")
    
    # Save brain data if requested
    if args.output:
        print(f"Exporting brain data to {args.output}...")
        # pynlm.utils.export_brain_data(brain, args.output)
        print("Export completed (simulated)")
    
    return 0

def run_operation(args):
    """Run various NLM operations"""
    if args.run_command == "compare":
        return run_compare(args)
    elif args.run_command == "export":
        return run_export(args)
    elif args.run_command == "batch":
        return run_batch(args)
    elif args.run_command == "analyze":
        return run_analyze(args)
    else:
        print(f"Unknown run command: {args.run_command}")
        return 1

def run_compare(args):
    """Compare multiple brain configurations"""
    configs = [c.strip() for c in args.configs.split(",")]
    
    print(f"Comparing {len(configs)} brain configurations for {args.steps} steps...")
    
    brains = []
    for config in configs:
        brain = pynlm.utils.create_brain_with_preset(config)
        brains.append(brain)
    
    comparison = pynlm.utils.compare_brains(brains, args.steps)
    
    print("\nComparison Results:")
    print("-" * 50)
    for config, brain_stats in zip(configs, comparison.values()):
        print(f"{config.upper()} brain:")
        print(f"  Neurons: {brain_stats['neurons']:,}")
        print(f"  Synapses: {brain_stats['synapses']:,}")
        print(f"  Firing neurons: {brain_stats['firing_neurons']}")
        print(f"  Total spikes: {brain_stats['total_spikes']:,}")
        print(f"  Average firing rate: {brain_stats['average_firing_rate']:.3f}")
        print(f"  Developmental stage: {brain_stats['developmental_stage']}")
        print()
    
    # Save comparison results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(comparison, f, indent=2)
        print(f"Comparison results saved to {args.output}")
    
    return 0

def run_export(args):
    """Export brain data"""
    print(f"Exporting {args.size} brain data after {args.steps} steps...")
    
    brain = pynlm.utils.create_brain_with_preset(args.size)
    brain.runEpisode(args.steps)
    
    # In a real implementation, this would call pynlm.utils.export_brain_data
    print(f"Brain data would be exported to {args.output_file}")
    print(f"Brain contains {brain.getTotalNeuronCount()} neurons")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return 0

def run_batch(args):
    """Run batch of experiments"""
    print(f"Running batch experiment: {args.experiment}")
    print(f"Number of runs: {args.runs}, Steps per run: {args.steps}")
    
    def create_brain_for_experiment():
        config = pynlm.utils.create_experiment_config(args.experiment)
        return pynlm.createBrain(config)
    
    results = pynlm.utils.run_repeated_experiments(
        create_brain_for_experiment, args.experiment, 
        args.runs, args.steps
    )
    
    # Print summary
    print(f"\nBatch experiment '{args.experiment}' completed!")
    print(f"Results from {len(results)} runs:")
    
    for run_key, run_data in results.items():
        print(f"  {run_key}: {run_data['final_spikes']} spikes, "
              f"Stage {run_data['developmental_stage']}")
    
    # Save results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Batch results saved to {args.output}")
    
    return 0

def run_analyze(args):
    """Analyze simulation results"""
    print(f"Analyzing simulation results from {args.input_file}")
    
    if args.stats:
        print("Detailed statistics analysis:")
        print("(This would require loading brain data from file)")
    
    if args.visualize:
        print("Generating visualization:")
        print("(This would require loading brain data and creating plots)")
    
    print("Analysis completed (simulated)")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())

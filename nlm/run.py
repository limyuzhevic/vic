#!/usr/bin/env python3
"""
NLM Run Script

This script provides a high-level interface for running NLM simulations
with various command-line options and configuration management.
"""

import sys
import os
import argparse
import json
import time
from pathlib import Path

# Add the current directory to the path so we can import nlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import nlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("Error: NLM Python bindings not found.")
    print("Please build the project first:")
    print("  mkdir build")
    print("  cd build")
    print("  cmake ..")
    print("  make pynlm")
    sys.exit(1)
def create_argument_parser():
    """Create and configure the argument parser."""
    parser = argparse.ArgumentParser(
        description="NLM Simulation Runner",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python run.py --config my_config.cfg --steps 1000
  python run.py --demo --steps 500
  python run.py --research --params research_params.json
  python run.py --validate --config my_config.cfg
  python run.py --benchmark --steps 100 --iterations 5
        """
    )
    
    # Basic simulation options
    parser.add_argument(
        "--config", "-c",
        type=str,
        help="Path to configuration file (JSON or NLM config format)",
    )
    
    parser.add_argument(
        "--steps", "-s",
        type=int,
        default=1000,
        help="Number of simulation steps to run (default: 1000)",
    )
    
    parser.add_argument(
        "--timestep", "-t",
        type=float,
        default=0.01,
        help="Simulation timestep in seconds (default: 0.01)",
    )
    
    # Environment options
    parser.add_argument(
        "--world-width", "-W",
        type=float,
        default=100.0,
        help="World width in simulation units (default: 100.0)",
    )
    
    parser.add_argument(
        "--world-height", "-H",
        type=float,
        default=100.0,
        help="World height in simulation units (default: 100.0)",
    )
    
    parser.add_argument(
        "--vision-width", "-vW",
        type=int,
        default=10,
        help="Vision width in world units (default: 10)",
    )
    
    parser.add_argument(
        "--vision-height", "-vH",
        type=int,
        default=10,
        help="Vision height in world units (default: 10)",
    )
    
    # Agent options
    parser.add_argument(
        "--agent", "-A",
        action="store_true",
        help="Run as agent in world (enabled by default if not specified)",
    )
    
    parser.add_argument(
        "--start-x", "-x",
        type=float,
        default=50.0,
        help="Agent starting X position (default: 50.0)",
    )
    
    parser.add_argument(
        "--start-y", "-y",
        type=float,
        default=50.0,
        help="Agent starting Y position (default: 50.0)",
    )
    
    # Output options
    parser.add_argument(
        "--output", "-o",
        type=str,
        help="Output file for results (JSON format)",
    )
    
    parser.add_argument(
        "--verbose", "-V",
        action="store_true",
        help="Enable verbose output",
    )
    
    parser.add_argument(
        "--quiet", "-q",
        action="store_true",
        help="Suppress non-essential output",
    )
    
    # Demo and preset options
    parser.add_argument(
        "--demo", "-d",
        action="store_true",
        help="Use demo configuration with sensible defaults",
    )
    
    parser.add_argument(
        "--preset", "-p",
        type=str,
        choices=["small", "medium", "large", "research"],
        help="Use preset configuration (small=100 neurons, medium=500, large=2000, research=1000)",
    )
    
    # Performance options
    parser.add_argument(
        "--benchmark", "-b",
        action="store_true",
        help="Run benchmark and report performance metrics",
    )
    
    parser.add_argument(
        "--iterations", "-i",
        type=int,
        default=1,
        help="Number of iterations for benchmarking (default: 1)",
    )
    
    # Analysis options
    parser.add_argument(
        "--analyze", "-a",
        action="store_true",
        help="Perform analysis on simulation results",
    )
    
    parser.add_argument(
        "--metrics", "-m",
        nargs="+",
        default=["spike_count", "firing_rate", "e_i_ratio"],
        help="Metrics to analyze (default: spike_count firing_rate e_i_ratio)",
    )
    
    # Validation options
    parser.add_argument(
        "--validate", "-v",
        action="store_true",
        help="Validate configuration and exit",
    )
    
    # Research options
    parser.add_argument(
        "--research", "-r",
        action="store_true",
        help="Run research simulation with parameter variations",
    )
    
    parser.add_argument(
        "--params", "-P",
        type=str,
        help="JSON file with parameter variations for research",
    )
    
    return parser
def load_config_file(config_path):
    """Load configuration from file."""
    if not os.path.exists(config_path):
        raise FileNotFoundError(f"Configuration file not found: {config_path}")
    
    # Determine file type based on extension
    if config_path.endswith('.json'):
        with open(config_path, 'r') as f:
            return json.load(f)
    else:
        # Assume NLM config format
        config = nlm.create_default_config()
        if config.loadFromFile(config_path):
            return config
        else:
            raise ValueError(f"Failed to load configuration from: {config_path}")
def create_configuration(args):
    """Create configuration based on command-line arguments."""
    # Start with demo config if requested
    if args.demo:
        config = nlm.create_demo_config()
        if args.verbose:
            print("Using demo configuration")
    elif args.preset:
        config = nlm.create_demo_config()
        
        # Apply preset settings
        if args.preset == "small":
            config.set_int("brain.neuron_count", 100)
            config.set_int("brain.simulation_steps", 500)
        elif args.preset == "medium":
            config.set_int("brain.neuron_count", 500)
            config.set_int("brain.simulation_steps", 1000)
        elif args.preset == "large":
            config.set_int("brain.neuron_count", 2000)
            config.set_int("brain.simulation_steps", 2000)
        elif args.preset == "research":
            config.set_int("brain.neuron_count", 1000)
            config.set_int("brain.simulation_steps", 5000)
            config.set_float("plasticity.learning_rate", 0.05)
        
        if args.verbose:
            print(f"Using {args.preset} preset configuration")
    else:
        config = nlm.create_default_config()
        
        if args.verbose:
            print("Using default configuration")
    
    # Apply command-line overrides
    if args.config:
        # Load external config and merge
        external_config = load_config_file(args.config)
        if isinstance(external_config, dict):
            # If external config is a dict, create a new config
            config = nlm.create_default_config()
            for key, value in external_config.items():
                if isinstance(value, int):
                    config.set_int(key, value)
                elif isinstance(value, float):
                    config.set_float(key, value)
                elif isinstance(value, str):
                    config.set_string(key, value)
                elif isinstance(value, bool):
                    config.set_bool(key, value)
        else:
            # Assume it's a Config object, merge it
            config = external_config
        
        if args.verbose:
            print(f"Loaded configuration from: {args.config}")
    
    # Apply world settings
    config.set_float("world.width", args.world_width)
    config.set_float("world.height", args.world_height)
    config.set_int("world.vision_width", args.vision_width)
    config.set_int("world.vision_height", args.vision_height)
    
    # Apply agent start position
    config.set_float("agent.start.x", args.start_x)
    config.set_float("agent.start.y", args.start_y)
    
    # Apply simulation settings
    config.set_float("simulation.timestep", args.timestep)
    config.set_int("brain.simulation_steps", args.steps)
    
    return config
def validate_configuration(config, args):
    """Validate configuration and print results."""
    print("Validating configuration...")
    
    errors = nlm.validate_config(config)
    
    if not errors:
        print("✓ Configuration is valid!")
        return True
    else:
        print(f"✗ Configuration has {len(errors)} errors:")
        for error in errors:
            print(f"  - {error}")
        return False
def run_simulation(config, args):
    """Run simulation with given configuration."""
    print(f"Running simulation with {config.get_int('brain.neuron_count', 0)} neurons for {args.steps} steps...")
    
    if args.benchmark:
        # Run benchmark
        return run_benchmark(config, args)
    else:
        # Run normal simulation
        return run_normal_simulation(config, args)
def run_normal_simulation(config, args):
    """Run normal simulation."""
    # Create brain and world
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.create_simple_demo_world()
    world.configure(
        args.world_width,
        args.world_height,
        args.vision_width,
        args.vision_height
    )
    world.setAgentStart(args.start_x, args.start_y)
    
    # Run simulation
    if args.agent:
        # Run as agent in world
        print("Running as agent in world...")
        
        # Create agent
        agent = nlm.create_agent(brain, world)
        agent.initialize(world)
        
        # Run agent episode
        for step in range(args.steps):
            # Get sensory perception
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Simulate world step
            world.update(config.get_float("simulation.timestep", 0.01))
            
            # Get brain action
            action = brain.produceAction()
            
            # Execute action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Log progress
            if args.verbose and step % 100 == 0:
                print(f"  Step {step}: Active neurons = {brain.getActiveNeuronCount()}")
        
        scenario = "agent"
    else:
        # Run world simulation
        print("Running world simulation...")
        
        # Use the simulation runner
        result = nlm.run_simulation(brain, world, args.steps, config.get_float("simulation.timestep", 0.01))
        scenario = "world"
    
    # Collect results
    results = {
        "configuration": config.getKeys(),
        "statistics": brain.get_statistics(),
        "scenario": scenario,
        "simulation_id": len(results) if 'results' in locals() else 0,
        "timestamp": time.time(),
    }
    
    return results
def run_benchmark(config, args):
    """Run benchmark simulation."""
    print(f"Running benchmark with {args.iterations} iterations...")
    
    # Create brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Run benchmark
    timings = []
    spike_counts = []
    
    for iteration in range(args.iterations):
        print(f"  Iteration {iteration + 1}/{args.iterations}...")
        
        # Run simulation
        world = nlm.create_simple_demo_world()
        start_time = time.time()
        
        nlm.run_simulation(brain, world, args.steps, config.get_float("simulation.timestep", 0.01))
        
        end_time = time.time()
        elapsed_ms = (end_time - start_time) * 1000
        
        timings.append(elapsed_ms)
        spike_counts.append(brain.getTotalSpikeCount())
        
        # Reset brain for next iteration
        brain.reset()
        brain.initialize()
    
    # Calculate statistics
    avg_time = sum(timings) / len(timings)
    std_time = (sum((t - avg_time) ** 2 for t in timings) / len(timings)) ** 0.5
    avg_spikes = sum(spike_counts) / len(spike_counts)
    
    results = {
        "configuration": config.getKeys(),
        "benchmark": {
            "iterations": args.iterations,
            "simulation_steps": args.steps,
            "total_time_ms": sum(timings),
            "avg_time_ms": avg_time,
            "std_time_ms": std_time,
            "time_per_step_ms": avg_time / args.steps,
            "total_spikes": sum(spike_counts),
            "avg_spikes": avg_spikes,
        },
        "scenario": "benchmark",
        "timestamp": time.time(),
    }
    
    return results
def save_results(results, output_path):
    """Save results to file."""
    if output_path.endswith('.json'):
        with open(output_path, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to: {output_path}")
    else:
        print(f"Warning: Output file should have .json extension, but got: {output_path}")
def analyze_results(results, metrics):
    """Analyze simulation results."""
    print(f"\nAnalyzing results with metrics: {metrics}")
    
    if 'statistics' not in results:
        print("No statistics available for analysis")
        return
    
    stats = results['statistics']
    
    for metric in metrics:
        if metric == 'spike_count':
            print(f"  Total spikes: {stats.get('total_spikes', 'N/A')}")
        elif metric == 'firing_rate':
            print(f"  Average firing rate: {stats.get('avg_firing_rate', 'N/A'):.2f} Hz")
        elif metric == 'e_i_ratio':
            print(f"  Excitation/Inhibition ratio: {stats.get('e_i_ratio', 'N/A'):.4f}")
        elif metric == 'neuron_count':
            print(f"  Total neurons: {stats.get('total_neurons', 'N/A')}")
        elif metric == 'synapse_count':
            print(f"  Total synapses: {stats.get('total_synapses', 'N/A')}")
        elif metric == 'active_neurons':
            print(f"  Active neurons: {stats.get('active_neurons', 'N/A')}")
        elif metric == 'firing_neurons':
            print(f"  Firing neurons: {stats.get('firing_neurons', 'N/A')}")
        elif metric == 'developmental_stage':
            print(f"  Developmental stage: {stats.get('developmental_stage', 'N/A')}")
        else:
            print(f"  Unknown metric: {metric}")
def load_research_params(params_path):
    """Load research parameter variations from file."""
    if not os.path.exists(params_path):
        raise FileNotFoundError(f"Research parameters file not found: {params_path}")
    
    with open(params_path, 'r') as f:
        return json.load(f)
def run_research_simulation(config, args):
    """Run research simulation with parameter variations."""
    if not args.params:
        print("No research parameters file specified")
        return
    
    print(f"Loading research parameters from: {args.params}")
    
    # Load parameter variations
    param_variations = load_research_params(args.params)
    
    if not isinstance(param_variations, list):
        param_variations = [param_variations]
    
    print(f"Running research with {len(param_variations)} parameter variations...")
    
    results = []
    
    for i, params in enumerate(param_variations):
        print(f"\nVariation {i + 1}/{len(param_variations)}:")
        
        # Create variation of configuration
        variation_config = nlm.create_demo_config()
        
        # Apply parameter variations
        for key, value in params.items():
            if isinstance(value, int):
                variation_config.set_int(key, value)
            elif isinstance(value, float):
                variation_config.set_float(key, value)
            elif isinstance(value, str):
                variation_config.set_string(key, value)
        
        # Override steps from command line
        variation_config.set_int("brain.simulation_steps", args.steps)
        
        # Run simulation
        variation_world = nlm.create_simple_demo_world()
        nlm.run_simulation(variation_config, variation_world, args.steps)
        
        # Collect results
        variation_results = {
            "parameters": params,
            "statistics": variation_config.getKeys(),
            "variation_id": i,
        }
        
        results.append(variation_results)
        
        print(f"  - Parameters: {params}")
        print(f"  - Simulation completed")
    
    return results
def main():
    """Main function."""
    parser = create_argument_parser()
    args = parser.parse_args()
    
    # Print banner
    print("=" * 60)
    print("NLM Simulation Runner")
    print("=" * 60)
    print()
    
    # Validate NLM availability
    if not NLM_AVAILABLE:
        print("Error: NLM Python bindings not available.")
        sys.exit(1)
    
    try:
        # Create configuration
        print("Creating configuration...")
        config = create_configuration(args)
        
        # Validate configuration if requested
        if args.validate:
            if not validate_configuration(config, args):
                sys.exit(1)
            print("Configuration validation completed.")
            sys.exit(0)
        
        # Run research simulation if requested
        if args.research:
            print("\nRunning research simulation...")
            research_results = run_research_simulation(config, args)
            
            if args.output:
                save_results(research_results, args.output)
            
            print(f"Research simulation completed with {len(research_results)} variations.")
        else:
            # Run normal simulation
            print("\nRunning simulation...")
            results = run_simulation(config, args)
            
            # Analyze results if requested
            if args.analyze:
                analyze_results(results, args.metrics)
            
            # Save results if requested
            if args.output:
                save_results(results, args.output)
            
            # Print summary
            print("\n" + "=" * 60)
            print("Simulation completed successfully!")
            print("=" * 60)
            
            if not args.quiet:
                print(f"\nSimulation Summary:")
                print(f"  Scenario: {results['scenario']}")
                print(f"  Neurons: {results['statistics'].get('total_neurons', 'N/A')}")
                print(f"  Total Spikes: {results['statistics'].get('total_spikes', 'N/A')}")
                print(f"  Firing Rate: {results['statistics'].get('avg_firing_rate', 'N/A'):.2f} Hz")
    
    except Exception as e:
        print(f"\nError during simulation: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
if __name__ == "__main__":
    main()

"""
Command-line interface for NLM (Neural Learning Machine).

This module provides a comprehensive command-line interface for
running NLM simulations and accessing various NLM features.
"""

import argparse
import sys
import json
import time
import os
from pathlib import Path
from typing import Optional, Dict, Any, List

import pynlm


def create_parser():
    """Create the main argument parser."""
    parser = argparse.ArgumentParser(
        prog="nlm-python",
        description="NLM (Neural Learning Machine) - Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Create a simple brain
  nlm-python create-brain --neurons 1000 --steps 100
  
  # Run a complete agent simulation
  nlm-python agent --steps 500 --world-size 20 --enable-learning
  
  # Run benchmarks
  nlm-python benchmark --type all --output benchmarks.json
  
  # Create and inspect a world
  nlm-python create-world --width 30 --height 30 --vision-size 8
  
  # Run development simulation
  nlm-python develop --steps 200 --enable-curiosity --enable-development
        """
    )
    
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--log-file",
        help="Log output to file"
    )
    
    parser.add_argument(
        "--config",
        help="Load configuration from file"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Create brain command
    parser_create = subparsers.add_parser(
        "create-brain",
        help="Create and initialize a brain"
    )
    parser_create.add_argument(
        "--neurons", "--neuron-count",
        type=int,
        help="Number of neurons in the brain"
    )
    parser_create.add_argument(
        "--regions", "--region-count",
        type=int,
        help="Number of brain regions"
    )
    parser_create.add_argument(
        "--synapse-prob", "--connection-probability",
        type=float,
        help="Connection probability between neurons (0.0-1.0)"
    )
    parser_create.add_argument(
        "--steps", "--simulation-steps",
        type=int,
        default=100,
        help="Number of simulation steps to run"
    )
    parser_create.add_argument(
        "--output", "--output-file",
        help="Save brain state to file"
    )
    
    # Agent simulation command
    parser_agent = subparsers.add_parser(
        "agent",
        help="Run agent simulation in a world"
    )
    parser_agent.add_argument(
        "--steps", "--simulation-steps",
        type=int,
        default=100,
        help="Number of simulation steps"
    )
    parser_agent.add_argument(
        "--world-size", "--world-width",
        type=int,
        default=20,
        help="Width and height of the world (square)"
    )
    parser_agent.add_argument(
        "--vision-size", "--vision-width",
        type=int,
        default=8,
        help="Vision dimensions (square)"
    )
    parser_agent.add_argument(
        "--timestep", "--dt",
        type=float,
        default=0.1,
        help="Simulation timestep in seconds"
    )
    parser_agent.add_argument(
        "--enable-reward", "--enable-reward-modulation",
        action="store_true",
        help="Enable reward-based neuromodulation"
    )
    parser_agent.add_argument(
        "--enable-curiosity",
        action="store_true",
        help="Enable curiosity-driven exploration"
    )
    parser_agent.add_argument(
        "--enable-development",
        action="store_true",
        help="Enable developmental plasticity"
    )
    parser_agent.add_argument(
        "--enable-plasticity",
        action="store_true",
        help="Enable structural plasticity"
    )
    parser_agent.add_argument(
        "--output", "--output-file",
        help="Save simulation results to file"
    )
    
    # Create world command
    parser_world = subparsers.add_parser(
        "create-world",
        help="Create and configure a world"
    )
    parser_world.add_argument(
        "--width", "--world-width",
        type=int,
        default=20,
        help="World width"
    )
    parser_world.add_argument(
        "--height", "--world-height",
        type=int,
        default=20,
        help="World height"
    )
    parser_world.add_argument(
        "--vision-width",
        type=int,
        default=8,
        help="Vision width"
    )
    parser_world.add_argument(
        "--vision-height",
        type=int,
        default=8,
        help="Vision height"
    )
    parser_world.add_argument(
        "--energy-decay",
        type=float,
        default=0.01,
        help="Energy decay rate"
    )
    
    # Benchmark command
    parser_bench = subparsers.add_parser(
        "benchmark",
        help="Run performance benchmarks"
    )
    parser_bench.add_argument(
        "--type",
        choices=["all", "creation", "simulation", "plasticity"],
        default="all",
        help="Type of benchmarks to run"
    )
    parser_bench.add_argument(
        "--neuron-counts",
        nargs="+",
        type=int,
        default=[100, 500, 1000],
        help="Neuron counts for creation benchmarks"
    )
    parser_bench.add_argument(
        "--step-counts",
        nargs="+",
        type=int,
        default=[100, 500, 1000],
        help="Step counts for simulation benchmarks"
    )
    parser_bench.add_argument(
        "--synapse-counts",
        nargs="+",
        type=int,
        default=[100, 500, 1000],
        help="Synapse counts for plasticity benchmarks"
    )
    parser_bench.add_argument(
        "--output", "--output-file",
        help="Save benchmark results to file"
    )
    
    # Development command
    parser_dev = subparsers.add_parser(
        "develop",
        help="Run developmental learning simulation"
    )
    parser_dev.add_argument(
        "--steps", "--simulation-steps",
        type=int,
        default=100,
        help="Number of developmental steps"
    )
    parser_dev.add_argument(
        "--enable-curiosity",
        action="store_true",
        help="Enable curiosity-driven exploration"
    )
    parser_dev.add_argument(
        "--enable-development",
        action="store_true",
        help="Enable developmental plasticity"
    )
    parser_dev.add_argument(
        "--enable-reward",
        action="store_true",
        help="Enable reward-based learning"
    )
    parser_dev.add_argument(
        "--output", "--output-file",
        help="Save development results to file"
    )
    
    # Example command
    parser_example = subparsers.add_parser(
        "example",
        help="Run example simulations"
    )
    parser_example.add_argument(
        "name",
        choices=["basic", "agent", "development", "plasticity", "all"],
        help="Name of example to run"
    )
    parser_example.add_argument(
        "--count", "--iterations",
        type=int,
        default=1,
        help="Number of iterations to run"
    )
    
    # Utils command
    parser_utils = subparsers.add_parser(
        "utils",
        help="Utility functions"
    )
    utils_subparsers = parser_utils.add_subparsers(dest="util_command")
    
    # Utils: Create config
    parser_config = utils_subparsers.add_parser(
        "create-config",
        help="Create and save a configuration file"
    )
    parser_config.add_argument(
        "--filename",
        required=True,
        help="Output configuration file name"
    )
    parser_config.add_argument(
        "--preset",
        choices=["minimal", "standard", "research", "simulation"],
        default="standard",
        help="Configuration preset to use"
    )
    
    return parser


def setup_logging(log_file: Optional[str] = None):
    """Setup logging configuration."""
    import logging
    
    level = logging.DEBUG if "--verbose" in sys.argv else logging.INFO
    
    handlers = [logging.StreamHandler(sys.stdout)]
    
    if log_file:
        handlers.append(logging.FileHandler(log_file))
    
    logging.basicConfig(
        level=level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        handlers=handlers
    )


def create_brain_command(args):
    """Handle brain creation command."""
    print(f"Creating brain with configuration...")
    
    config = pynlm.createDefaultConfig()
    
    if args.config:
        if os.path.exists(args.config):
            config.loadFromFile(args.config)
            print(f"Loaded configuration from: {args.config}")
        else:
            print(f"Warning: Config file not found: {args.config}")
    
    if args.neurons:
        config.set("brain.neuron_count", args.neurons)
        print(f"Set neuron count: {args.neurons}")
    
    if args.regions:
        config.set("brain.region_count", args.regions)
        print(f"Set region count: {args.regions}")
    
    if args.synapse_prob:
        config.set("brain.connection_probability", args.synapse_prob)
        print(f"Set connection probability: {args.synapse_prob}")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"\nBrain created successfully:")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Regions: {brain.getRegionCount()}")
    print(f"  - Synapses: {brain.getTotalSynapseCount()}")
    
    # Run simulation steps
    print(f"\nRunning {args.steps} simulation steps...")
    start_time = time.time()
    
    for step in range(args.steps):
        brain.step(step)
        if args.verbose and step % 10 == 0:
            print(f"    Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\nSimulation completed in {duration:.2f} seconds")
    print(f"  - Average speed: {args.steps / duration:.1f} steps/second")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # Save if requested
    if args.output:
        brain.save(args.output)
        print(f"  - Brain state saved to: {args.output}")
    
    return brain


def agent_simulation_command(args):
    """Handle agent simulation command."""
    print(f"Starting agent simulation in {args.world_size}x{args.world_size} world...")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.region_count", 2)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=args.world_size,
        height=args.world_size,
        visionWidth=args.vision_size,
        visionHeight=args.vision_size
    )
    world.reset()
    
    agent.initialize(world)
    
    # Enable features based on command line arguments
    if args.enable_reward:
        agent.enableRewardModulation(True)
        print("  - Reward modulation enabled")
    
    if args.enable_curiosity:
        agent.enableCuriosity(True)
        print("  - Curiosity enabled")
    
    if args.enable_development:
        agent.enableDevelopment(True)
        print("  - Development enabled")
    
    if args.enable_plasticity:
        agent.enableStructuralPlasticity(True)
        print("  - Structural plasticity enabled")
    
    print(f"\nRunning {args.steps} simulation steps...")
    
    # Track metrics
    metrics = {
        "steps": args.steps,
        "final_spikes": 0,
        "final_firing_rate": 0.0,
        "curiosity": 0.0,
        "novelty": 0.0,
        "duration": 0.0,
        "steps_per_second": 0.0
    }
    
    start_time = time.time()
    
    for step in range(args.steps):
        # Update world
        world.update(args.timestep)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning if enabled
        if args.enable_reward:
            # Simple reward based on internal state
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0] * 0.1
            agent.applyRewardModulation(reward, 0.0)
    
    end_time = time.time()
    
    # Collect final metrics
    metrics.update({
        "final_spikes": brain.getTotalSpikeCount(),
        "final_firing_rate": brain.getAverageFiringRate(),
        "curiosity": agent.getCuriosityLevel(),
        "novelty": agent.getNoveltyLevel(),
        "duration": end_time - start_time,
        "steps_per_second": args.steps / (end_time - start_time)
    })
    
    print(f"\nAgent simulation completed:")
    print(f"  - Duration: {metrics['duration']:.2f} seconds")
    print(f"  - Speed: {metrics['steps_per_second']:.1f} steps/second")
    print(f"  - Total spikes: {metrics['final_spikes']}")
    print(f"  - Final firing rate: {metrics['final_firing_rate']:.2f} Hz")
    print(f"  - Final curiosity: {metrics['curiosity']:.3f}")
    print(f"  - Final novelty: {metrics['novelty']:.3f}")
    
    # Save results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(metrics, f, indent=2)
        print(f"  - Results saved to: {args.output}")
    
    return metrics


def benchmark_command(args):
    """Handle benchmark command."""
    print(f"Running NLM benchmarks (type: {args.type})...")
    
    benchmark = pynlm.NLMBenchmark()
    
    # Configure benchmark parameters
    if args.type in ["all", "creation"]:
        print("  Running brain creation benchmarks...")
        creation_results = benchmark.benchmark_brain_creation(
            [int(n) for n in args.neuron_counts]
        )
        benchmark.results.extend(creation_results)
    
    if args.type in ["all", "simulation"]:
        print("  Running simulation benchmarks...")
        simulation_results = benchmark.benchmark_simulation_speed(
            [int(s) for s in args.step_counts]
        )
        benchmark.results.extend(simulation_results)
    
    if args.type in ["all", "plasticity"]:
        print("  Running plasticity benchmarks...")
        plasticity_results = benchmark.benchmark_plasticity_performance(
            [int(s) for s in args.synapse_counts]
        )
        benchmark.results.extend(plasticity_results)
    
    # Print summary
    print("\n=== Benchmark Summary ===")
    benchmark.print_summary()
    
    # Save results if requested
    if args.output:
        benchmark.save_results(args.output)
        print(f"\nBenchmark results saved to: {args.output}")
    
    return benchmark.results


def create_world_command(args):
    """Handle create world command."""
    print(f"Creating {args.width}x{args.height} world...")
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=args.width,
        height=args.height,
        visionWidth=args.vision_width,
        visionHeight=args.vision_height
    )
    
    world.reset()
    
    print(f"\nWorld created successfully:")
    print(f"  - Dimensions: {args.width} x {args.height}")
    print(f"  - Vision: {args.vision_width} x {args.vision_height}")
    print(f"  - Max energy: {world.getMaxEnergy()}")
    print(f"  - Energy decay rate: {world.getEnergyDecayRate()}")
    print(f"  - Simulation time: {world.getSimulationTime():.3f}s")
    
    # Show agent position
    body = world.getAgentBody()
    print(f"  - Agent position: ({body.x:.1f}, {body.y:.1f})")
    print(f"  - Agent orientation: {body.orientation:.1f}°")
    
    return world


def development_command(args):
    """Handle development simulation command."""
    print("Starting developmental learning simulation...")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 2)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable features based on command line arguments
    if args.enable_curiosity:
        agent.enableCuriosity(True)
        print("  - Curiosity enabled")
    
    if args.enable_development:
        agent.enableDevelopment(True)
        print("  - Development enabled")
    
    if args.enable_reward:
        agent.enableRewardModulation(True)
        print("  - Reward modulation enabled")
    
    print(f"\nRunning {args.steps} developmental steps...")
    
    # Track developmental metrics
    metrics = {
        "steps": args.steps,
        "initial_stage": str(brain.getDevelopmentalStage()),
        "final_stage": "",
        "stage_changed": False,
        "final_curiosity": 0.0,
        "final_novelty": 0.0,
        "duration": 0.0,
        "steps_per_second": 0.0
    }
    
    initial_stage = brain.getDevelopmentalStage()
    
    start_time = time.time()
    
    for step in range(args.steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.updateDevelopment(0.1)
        
        if args.enable_reward:
            reward = 0.1  # Simple reward
            agent.applyRewardModulation(reward, 0.05)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    
    # Calculate final metrics
    final_stage = brain.getDevelopmentalStage()
    metrics.update({
        "final_stage": str(final_stage),
        "stage_changed": initial_stage != final_stage,
        "final_curiosity": agent.getCuriosityLevel(),
        "final_novelty": agent.getNoveltyLevel(),
        "duration": end_time - start_time,
        "steps_per_second": args.steps / (end_time - start_time)
    })
    
    print(f"\nDevelopmental simulation completed:")
    print(f"  - Duration: {metrics['duration']:.2f} seconds")
    print(f"  - Speed: {metrics['steps_per_second']:.1f} steps/second")
    print(f"  - Initial stage: {metrics['initial_stage']}")
    print(f"  - Final stage: {metrics['final_stage']}")
    print(f"  - Development occurred: {'✓ Yes' if metrics['stage_changed'] else '✗ No'}")
    print(f"  - Final curiosity: {metrics['final_curiosity']:.3f}")
    print(f"  - Final novelty: {metrics['final_novelty']:.3f}")
    
    # Save results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(metrics, f, indent=2)
        print(f"  - Results saved to: {args.output}")
    
    return metrics


def example_command(args):
    """Handle example command."""
    examples_dir = pynlm.ExamplesDirectory()
    examples_dir.create_example_structure()
    
    print(f"Running example: {args.name}")
    
    if args.name == "all":
        # Run all examples
        return pynlm.run_all_examples()
    else:
        # Run specific example
        example_files = examples_dir.get_example_files("tutorials")
        for example_file in example_files:
            if example_file.stem == args.name:
                print(f"Running example: {example_file}")
                return pynlm.run_example(str(example_file))
    
    print(f"Example '{args.name}' not found")
    return None


def utils_command(args):
    """Handle utils command."""
    if args.util_command == "create-config":
        return create_config_command(args)
    else:
        print(f"Unknown utility command: {args.util_command}")
        return None


def create_config_command(args):
    """Create and save a configuration file."""
    print(f"Creating configuration file: {args.filename}")
    
    config = pynlm.createDefaultConfig()
    
    # Load preset
    if args.preset:
        config.load_preset(args.preset)
        print(f"  - Loaded preset: {args.preset}")
    
    # Save to file
    config.saveToFile(args.filename)
    
    print(f"  - Configuration saved to: {args.filename}")
    return config


def main():
    """Main CLI entry point."""
    parser = create_parser()
    args = parser.parse_args()
    
    # Setup logging if requested
    if args.log_file:
        setup_logging(args.log_file)
    
    try:
        # Execute the requested command
        if args.command == "create-brain":
            create_brain_command(args)
        elif args.command == "agent":
            agent_simulation_command(args)
        elif args.command == "benchmark":
            benchmark_command(args)
        elif args.command == "create-world":
            create_world_command(args)
        elif args.command == "develop":
            development_command(args)
        elif args.command == "example":
            example_command(args)
        elif args.command == "utils":
            utils_command(args)
        else:
            parser.print_help()
            return 1
        
        print("\n✓ Command completed successfully")
        return 0
        
    except KeyboardInterrupt:
        print("\nCommand interrupted by user")
        return 1
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        if "--verbose" in sys.argv:
            traceback.print_exc()
        else:
            print("Use --verbose to see detailed error information")
        return 1


if __name__ == "__main__":
    sys.exit(main())

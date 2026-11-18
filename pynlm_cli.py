#!/usr/bin/env python3
"""
Advanced Python CLI tools for NLM (Neural Learning Machine)

This module provides command-line interfaces for common NLM operations,
making it easier for researchers and developers to work with the neural
simulation framework without writing complex Python code.
"""

import argparse
import json
import sys
import time
from pathlib import Path
from typing import Optional, List, Dict, Any
import numpy as np

# Import pynlm modules
import pynlm
from pynlm.utils import run_simulation, run_agent_simulation, benchmark_brain


def create_config_cli():
    """CLI tool for creating and managing configurations."""
    parser = argparse.ArgumentParser(
        description="Create and manage NLM configurations",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Create config
    create_parser = subparsers.add_parser("create", help="Create a new config")
    create_parser.add_argument("--name", default="default", help="Config name")
    create_parser.add_argument("--neuron-count", type=int, default=1000, help="Number of neurons")
    create_parser.add_argument("--synapse-density", type=float, default=0.1, help="Synapse density")
    create_parser.add_argument("--output", help="Output file path")
    
    # List configs
    list_parser = subparsers.add_parser("list", help="List available configs")
    
    # Apply config
    apply_parser = subparsers.add_parser("apply", help="Apply config to brain")
    apply_parser.add_argument("file", help="Config file path")
    apply_parser.add_argument("--output", help="Output brain file")
    
    # Export config
    export_parser = subparsers.add_parser("export", help="Export config to file")
    export_parser.add_argument("--file", help="Config file to export")
    export_parser.add_argument("--format", choices=["json", "text"], default="json", help="Output format")
    
    args = parser.parse_args()
    
    if args.command == "create":
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", args.neuron_count)
        config.set("brain.synapse_density", args.synapse_density)
        
        if args.output:
            config.saveToFile(args.output)
            print(f"Config saved to {args.output}")
        else:
            print(config.summary())
            
    elif args.command == "list":
        print("Available config templates:")
        print("1. Default (1000 neurons, 0.1 density)")
        print("2. Small (100 neurons, 0.05 density)")
        print("3. Large (5000 neurons, 0.2 density)")
        
    elif args.command == "apply":
        config = pynlm.createDefaultConfig()
        config.loadFromFile(args.file)
        print("Config loaded successfully")
        
        if args.output:
            brain = pynlm.createBrain(config)
            brain.initialize()
            brain.save(args.output)
            print(f"Brain saved to {args.output}")
            
    elif args.command == "export":
        config = pynlm.createDefaultConfig()
        if args.file:
            config.loadFromFile(args.file)
            
        if args.format == "json":
            # Export to structured JSON
            export_data = {
                "metadata": {
                    "name": "default",
                    "version": "1.0.0"
                },
                "config": {
                    key: config.get(key) for key in config.getKeys()
                    if config.get(key) is not None
                }
            }
            print(json.dumps(export_data, indent=2))
        else:
            print(config.summary())


def brain_cli():
    """CLI tool for brain simulation operations."""
    parser = argparse.ArgumentParser(
        description="Brain simulation operations",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Simulate
    simulate_parser = subparsers.add_parser("simulate", help="Run brain simulation")
    simulate_parser.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    simulate_parser.add_argument("--neuron-count", type=int, default=100, help="Number of neurons")
    simulate_parser.add_argument("--output", help="Output file for brain state")
    simulate_parser.add_argument("--verbose", action="store_true", help="Verbose output")
    simulate_parser.add_argument("--stats", action="store_true", help="Output statistics")
    
    # Analyze brain
    analyze_parser = subparsers.add_parser("analyze", help="Analyze brain state")
    analyze_parser.add_argument("file", help="Brain file to analyze")
    analyze_parser.add_argument("--format", choices=["text", "json"], default="text", help="Output format")
    
    # Create brain
    create_parser = subparsers.add_parser("create", help="Create new brain")
    create_parser.add_argument("--config", help="Config file path")
    create_parser.add_argument("--output", help="Output brain file")
    
    # Load brain
    load_parser = subparsers.add_parser("load", help="Load brain from file")
    load_parser.add_argument("file", help="Brain file to load")
    load_parser.add_argument("--output", help="Output brain file (after processing)")
    
    args = parser.parse_args()
    
    if args.command == "simulate":
        print(f"Creating brain with {args.neuron_count} neurons...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", args.neuron_count)
        config.set("brain.region_count", max(1, args.neuron_count // 100))
        config.set("connection_probability", 0.1)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Running {args.steps} simulation steps...")
        
        for step in range(args.steps):
            brain.step(step)
            
            if args.verbose and step % 20 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
                
            if args.stats and step % 50 == 0:
                stats = {
                    "step": step,
                    "firing_neurons": brain.getFiringNeuronCount(),
                    "total_spikes": brain.getTotalSpikeCount(),
                    "avg_firing_rate": brain.getAverageFiringRate(),
                    "e_i_ratio": brain.getExcitationInhibitionRatio()
                }
                print(json.dumps(stats, indent=2))
        
        if args.output:
            brain.save(args.output)
            print(f"Brain state saved to {args.output}")
            
        print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
        
    elif args.command == "analyze":
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.load(args.file)
        
        if args.format == "json":
            analysis = {
                "neuron_count": brain.getTotalNeuronCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "active_neurons": brain.getActiveNeuronCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "e_i_ratio": brain.getExcitationInhibitionRatio(),
                "total_spikes": brain.getTotalSpikeCount(),
                "developmental_stage": brain.getDevelopmentalStage().name
            }
            print(json.dumps(analysis, indent=2))
        else:
            print(f"Neuron count: {brain.getTotalNeuronCount()}")
            print(f"Synapse count: {brain.getTotalSynapseCount()}")
            print(f"Active neurons: {brain.getActiveNeuronCount()}")
            print(f"Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
            print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            print(f"Developmental stage: {brain.getDevelopmentalStage().name}")
            
    elif args.command == "create":
        config = pynlm.createDefaultConfig()
        if args.config:
            config.loadFromFile(args.config)
            
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
        
        if args.output:
            brain.save(args.output)
            print(f"Brain saved to {args.output}")
            
    elif args.command == "load":
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.load(args.file)
        
        print(f"Loaded brain with {brain.getTotalNeuronCount()} neurons")
        
        if args.output:
            brain.save(args.output)
            print(f"Brain saved to {args.output}")


def world_cli():
    """CLI tool for world simulation operations."""
    parser = argparse.ArgumentParser(
        description="World simulation operations",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Create world
    create_parser = subparsers.add_parser("create", help="Create world")
    create_parser.add_argument("--width", type=int, default=20, help="World width")
    create_parser.add_argument("--height", type=int, default=20, help="World height")
    create_parser.add_argument("--vision-width", type=int, default=8, help="Vision width")
    create_parser.add_argument("--vision-height", type=int, default=8, help="Vision height")
    
    # Add object
    add_parser = subparsers.add_parser("add-object", help="Add object to world")
    add_parser.add_argument("--x", type=float, required=True, help="X position")
    add_parser.add_argument("--y", type=float, required=True, help="Y position")
    add_parser.add_argument("--type", choices=["empty", "resource", "hazard", "wall", "marker"], 
                           default="empty", help="Object type")
    add_parser.add_argument("--value", type=float, default=0.0, help="Object value")
    add_parser.add_argument("--radius", type=float, default=0.5, help="Object radius")
    
    # Set agent start
    start_parser = subparsers.add_parser("set-start", help="Set agent start position")
    start_parser.add_argument("--x", type=float, required=True, help="X position")
    start_parser.add_argument("--y", type=float, required=True, help="Y position")
    
    # Run simulation
    run_parser = subparsers.add_parser("run", help="Run world simulation")
    run_parser.add_argument("--steps", type=int, default=100, help="Number of steps")
    run_parser.add_argument("--timestep", type=float, default=0.1, help="Time step duration")
    run_parser.add_argument("--verbose", action="store_true", help="Verbose output")
    
    args = parser.parse_args()
    
    if args.command == "create":
        world = pynlm.createSimpleWorld()
        world.configure(
            width=args.width,
            height=args.height,
            visionWidth=args.vision_width,
            visionHeight=args.vision_height
        )
        world.reset()
        
        print(f"Created world: {args.width}x{args.height}, vision: {args.vision_width}x{args.vision_height}")
        
    elif args.command == "add-object":
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        
        obj_type = getattr(pynlm.WorldObjectType, args.type.capitalize())
        obj = pynlm.WorldObject(
            x=args.x,
            y=args.y,
            type=obj_type,
            value=args.value,
            radius=args.radius
        )
        
        world.addObject(obj)
        print(f"Added {args.type} object at ({args.x}, {args.y})")
        
    elif args.command == "set-start":
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(args.x, args.y)
        print(f"Set agent start position to ({args.x}, {args.y})")
        
    elif args.command == "run":
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(10.0, 10.0)
        
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print(f"Running world simulation for {args.steps} steps...")
        
        for step in range(args.steps):
            world.update(args.timestep)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if args.verbose and step % 20 == 0:
                print(f"Step {step}: pos=({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
                      f"energy={world.getAgentBody().energy:.2f}")
        
        print(f"World simulation completed. Final time: {world.getSimulationTime():.2f}")


def benchmark_cli():
    """CLI tool for benchmarking NLM performance."""
    parser = argparse.ArgumentParser(
        description="Benchmark NLM performance",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    parser.add_argument("--neuron-count", type=int, default=100, help="Number of neurons")
    parser.add_argument("--region-count", type=int, default=1, help="Number of regions")
    parser.add_argument("--trials", type=int, default=3, help="Number of trials")
    parser.add_argument("--output", help="Output file for benchmark results")
    
    args = parser.parse_args()
    
    print(f"Benchmarking NLM with {args.neuron_count} neurons, {args.steps} steps, {args.trials} trials")
    
    results = []
    
    for trial in range(args.trials):
        print(f"Trial {trial + 1}/{args.trials}...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", args.neuron_count)
        config.set("brain.region_count", args.region_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(args.steps):
            brain.step(step)
        
        end_time = time.time()
        elapsed = end_time - start_time
        
        stats = {
            "trial": trial + 1,
            "steps": args.steps,
            "neuron_count": args.neuron_count,
            "region_count": args.region_count,
            "time_elapsed": elapsed,
            "steps_per_second": args.steps / elapsed,
            "total_spikes": brain.getTotalSpikeCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "memory_usage": brain.getTotalNeuronCount() + brain.getTotalSynapseCount()
        }
        
        results.append(stats)
        print(f"  Steps/sec: {stats['steps_per_second']:.1f}")
    
    # Calculate averages
    avg_steps_per_second = np.mean([r['steps_per_second'] for r in results])
    avg_time = np.mean([r['time_elapsed'] for r in results])
    
    benchmark_results = {
        "config": {
            "neuron_count": args.neuron_count,
            "region_count": args.region_count,
            "steps": args.steps
        },
        "trials": args.trials,
        "results": results,
        "averages": {
            "steps_per_second": avg_steps_per_second,
            "time_elapsed": avg_time
        },
        "total_spikes": np.mean([r['total_spikes'] for r in results]),
        "avg_firing_rate": np.mean([r['avg_firing_rate'] for r in results])
    }
    
    print("\nBenchmark Results:")
    print(f"Average steps per second: {avg_steps_per_second:.1f}")
    print(f"Average time per trial: {avg_time:.2f} seconds")
    print(f"Average total spikes: {benchmark_results['total_spikes']:.0f}")
    print(f"Average firing rate: {benchmark_results['avg_firing_rate']:.3f}")
    
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(benchmark_results, f, indent=2)
        print(f"Benchmark results saved to {args.output}")


def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="Advanced Python CLI tools for NLM (Neural Learning Machine)",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  pynlm_cli config create --neuron-count 500 --synapse-density 0.2
  pynlm_cli brain simulate --steps 1000 --neuron-count 200
  pynlm_cli world run --steps 500 --verbose
  pynlm_cli benchmark --steps 1000 --neuron-count 100
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Create subcommands
    config_parser = subparsers.add_parser("config", help="Configuration management")
    config_parser.set_defaults(func=create_config_cli)
    
    brain_parser = subparsers.add_parser("brain", help="Brain simulation operations")
    brain_parser.set_defaults(func=brain_cli)
    
    world_parser = subparsers.add_parser("world", help="World simulation operations")
    world_parser.set_defaults(func=world_cli)
    
    benchmark_parser = subparsers.add_parser("benchmark", help="Benchmark NLM performance")
    benchmark_parser.set_defaults(func=benchmark_cli)
    
    # Show help if no command provided
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)
    
    # Execute the appropriate function
    parser.parse_args().func()


if __name__ == "__main__":
    main()
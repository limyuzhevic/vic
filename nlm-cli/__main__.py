#!/usr/bin/env python3
"""
NLM Command Line Interface

A command-line interface for the Neural Learning Machine (NLM) project.
Allows running simulations without writing Python code and managing configurations.
"""

import argparse
import json
import sys
import time
from pathlib import Path
from typing import Optional, List, Dict, Any

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install the NLM Python bindings.", file=sys.stderr)
    sys.exit(1)

def create_brain_from_config(config_file: Optional[str] = None) -> Any:
    """Create a brain with optional configuration file."""
    if config_file:
        config = pynlm.Config()
        config.loadFromFile(config_file)
    else:
        config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

def run_simple_simulation(config_file: Optional[str] = None, 
                        steps: int = 100,
                        output_format: str = "text") -> None:
    """Run a simple simulation without writing Python code."""
    print(f"Creating brain from config: {config_file or 'default'}")
    
    brain = create_brain_from_config(config_file)
    
    print(f"Running {steps} simulation steps...")
    start_time = time.time()
    
    for i in range(steps):
        brain.step(i)
        
        if output_format == "text" and i % 20 == 0:
            print(f"  Step {i}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"{brain.getAverageFiringRate():.2f} firing rate")
    
    end_time = time.time()
    duration = end_time - start_time
    
    if output_format == "text":
        print(f"\nSimulation complete!")
        print(f"  Duration: {duration:.2f} seconds")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Development stage: {brain.getDevelopmentalStage().name}")
    
    elif output_format == "json":
        stats = {
            "steps": steps,
            "duration": duration,
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "firing_neuron_count": brain.getFiringNeuronCount(),
            "active_neuron_count": brain.getActiveNeuronCount(),
            "development_stage": brain.getDevelopmentalStage().name
        }
        print(json.dumps(stats, indent=2))
    
    elif output_format == "csv":
        headers = ["step", "firing_neurons", "spike_count", "firing_rate"]
        print(",".join(headers))
        for i in range(0, min(steps, 50), 10):
            brain.step(i)
            print(f"{i},{brain.getFiringNeuronCount()},{brain.getTotalSpikeCount()},"
                  f"{brain.getAverageFiringRate():.4f}")

def run_world_simulation(config_file: Optional[str] = None,
                        width: int = 10,
                        height: int = 10,
                        vision_width: int = 8,
                        vision_height: int = 8,
                        steps: int = 50,
                        output_format: str = "text") -> None:
    """Run a brain in a world simulation."""
    print(f"Creating brain and world with config: {config_file or 'default'}")
    
    brain = create_brain_from_config(config_file)
    world = pynlm.createSimpleWorld()
    world.configure(width, height, vision_width, vision_height)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"Running {steps} world simulation steps...")
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if output_format == "text" and step % 10 == 0:
            print(f"  Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}), "
                  f"Firing neurons: {brain.getFiringNeuronCount()}")
    
    if output_format == "text":
        print(f"\nWorld simulation complete!")
        print(f"  Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Agent energy: {world.getAgentBody().energy:.1f}")
    
    elif output_format == "json":
        stats = {
            "steps": steps,
            "final_position": {"x": world.getAgentBody().x, "y": world.getAgentBody().y},
            "total_spikes": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "firing_neuron_count": brain.getFiringNeuronCount(),
            "agent_energy": world.getAgentBody().energy,
            "agent_age": world.getAgentBody().age
        }
        print(json.dumps(stats, indent=2))

def batch_simulation(config_files: List[str],
                    steps_per_batch: int = 100,
                    num_batches: int = 3,
                    output_format: str = "text") -> None:
    """Run multiple simulations in batch."""
    print(f"Running batch processing with {len(config_files)} config(s)")
    print(f"Each batch: {steps_per_batch} steps, {num_batches} batches")
    
    results = []
    
    for batch in range(num_batches):
        print(f"\nBatch {batch + 1}/{num_batches}")
        
        for i, config_file in enumerate(config_files):
            config_name = Path(config_file).stem
            print(f"  Processing config: {config_name}")
            
            brain = create_brain_from_config(config_file)
            
            for step in range(steps_per_batch):
                brain.step(step)
            
            batch_stats = {
                "batch": batch + 1,
                "config": config_name,
                "steps": steps_per_batch,
                "total_spikes": brain.getTotalSpikeCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "firing_neuron_count": brain.getFiringNeuronCount()
            }
            results.append(batch_stats)
            
            if output_format == "text":
                print(f"    Total spikes: {batch_stats['total_spikes']}")
    
    if output_format in ["json", "text"]:
        print(f"\nBatch processing complete!")
        if output_format == "json":
            print(json.dumps(results, indent=2))

def export_brain_state(brain: Any, filepath: str) -> None:
    """Export brain state to file."""
    brain.save(filepath)
    print(f"Brain state exported to: {filepath}")

def import_brain_state(filepath: str) -> Any:
    """Import brain state from file."""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.load(filepath)
    print(f"Brain state imported from: {filepath}")
    return brain

def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface - Run neural simulations from the command line"
    )
    
    parser.add_argument(
        "--version", action="version",
        version="NLM CLI 0.1.0"
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Simple simulation command
    sim_parser = subparsers.add_parser("simulate", help="Run a simple simulation")
    sim_parser.add_argument("--config", "-c", help="Configuration file path")
    sim_parser.add_argument("--steps", "-s", type=int, default=100,
                          help="Number of simulation steps")
    sim_parser.add_argument("--output", "-o", choices=["text", "json", "csv"],
                          default="text", help="Output format")
    
    # World simulation command
    world_parser = subparsers.add_parser("simulate-world", help="Run a brain in a world")
    world_parser.add_argument("--config", "-c", help="Configuration file path")
    world_parser.add_argument("--width", type=int, default=10, help="World width")
    world_parser.add_argument("--height", type=int, default=10, help="World height")
    world_parser.add_argument("--vision-width", type=int, default=8, help="Vision width")
    world_parser.add_argument("--vision-height", type=int, default=8, help="Vision height")
    world_parser.add_argument("--steps", "-s", type=int, default=50, help="Number of steps")
    world_parser.add_argument("--output", "-o", choices=["text", "json"], default="text")
    
    # Batch simulation command
    batch_parser = subparsers.add_parser("batch", help="Run multiple simulations in batch")
    batch_parser.add_argument("configs", nargs="+", help="Configuration file paths")
    batch_parser.add_argument("--steps", "-s", type=int, default=100,
                           help="Steps per batch")
    batch_parser.add_argument("--batches", type=int, default=3,
                           help="Number of batches")
    batch_parser.add_argument("--output", "-o", choices=["text", "json"], default="text")
    
    # Config commands
    config_parser = subparsers.add_parser("config", help="Configuration management commands")
    config_subparsers = config_parser.add_subparsers(dest="config_command")
    
    # Config list
    list_parser = config_subparsers.add_parser("list", help="List configuration keys")
    list_parser.add_argument("brain", help="Brain object")
    
    # Config export
    export_parser = config_subparsers.add_parser("export", help="Export configuration to file")
    export_parser.add_argument("--config", "-c", help="Configuration file path")
    export_parser.add_argument("--output", "-o", help="Output file path")
    
    # Brain state commands
    brain_parser = subparsers.add_parser("brain", help="Brain state management commands")
    brain_subparsers = brain_parser.add_subparsers(dest="brain_command")
    
    # Brain save
    save_parser = brain_subparsers.add_parser("save", help="Save brain state to file")
    save_parser.add_argument("brain", help="Brain object")
    save_parser.add_argument("--file", "-f", required=True, help="Output file path")
    
    # Brain load
    load_parser = brain_subparsers.add_parser("load", help="Load brain state from file")
    load_parser.add_argument("--file", "-f", required=True, help="Input file path")
    
    # Example usage
    example_parser = subparsers.add_parser("example", help="Run predefined examples")
    example_parser.add_argument("example_name", choices=["simple", "world", "batch"],
                              help="Example to run")
    
    args = parser.parse_args()
    
    try:
        if args.command == "simulate":
            run_simple_simulation(args.config, args.steps, args.output)
        elif args.command == "simulate-world":
            run_world_simulation(args.config, args.width, args.height,
                               args.vision_width, args.vision_height,
                               args.steps, args.output)
        elif args.command == "batch":
            batch_simulation(args.configs, args.steps, args.batches, args.output)
        elif args.command == "config":
            if args.config_command == "list":
                # This would need a brain object - for now just show help
                print("Config listing requires a brain object")
            elif args.config_command == "export":
                config = pynlm.Config()
                if args.config:
                    config.loadFromFile(args.config)
                output_file = args.output or "config.json"
                config.saveToFile(output_file)
        elif args.command == "brain":
            if args.brain_command == "save":
                # This would need a brain object - for now just show help
                print("Brain saving requires a brain object")
            elif args.brain_command == "load":
                brain = import_brain_state(args.file)
                print(f"Loaded brain with {brain.getTotalNeuronCount()} neurons")
        elif args.command == "example":
            if args.example_name == "simple":
                run_simple_simulation(None, 50, "text")
            elif args.example_name == "world":
                run_world_simulation(None, 10, 10, 8, 8, 25, "text")
            elif args.example_name == "batch":
                batch_simulation([], 50, 2, "text")
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()

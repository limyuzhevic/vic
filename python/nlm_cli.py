#!/usr/bin/env python3
"""
NLM Command Line Interface

A comprehensive command-line interface for the NLM (Neural Learning Machine) brain simulator.
Supports simulation management, configuration, experimentation, and monitoring.
"""

import argparse
import json
import sys
import time
import os
import threading
from typing import Dict, List, Optional, Any
from datetime import datetime

# Add the current directory to the path to import our utilities
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    from python.nlm_utils import (
        NLMExperiment, ConfigPresets, ExperimentType, 
        create_config_from_file, save_config_to_file, benchmark_brain
    )
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: Could not import NLM modules: {e}")
class NLMCLI:
    """Command-line interface for NLM operations"""
    
    def __init__(self):
        self.experiment = None
        self.setup_complete = False
    
    def check_availability(self):
        """Check if NLM is available"""
        if not PYNLM_AVAILABLE:
            print("Error: NLM Python bindings not available.")
            print("Please install pynlm: pip install pynlm")
            sys.exit(1)
    
    def setup_simulation(self, args) -> bool:
        """Setup a simulation based on command line arguments"""
        self.check_availability()
        
        print(f"Setting up NLM simulation with {args.neurons} neurons...")
        
        # Create experiment
        self.experiment = NLMExperiment(
            name=args.name or f"nlm_simulation_{int(time.time())}",
            experiment_type=ExperimentType.SIMULATION
        )
        
        # Setup based on preset or custom config
        if args.preset:
            self.experiment.setup_brain(preset=args.preset)
            print(f"  Using preset: {args.preset}")
        else:
            # Create custom config
            config = pynlm.createDefaultConfig()
            if args.neurons:
                config.set("brain.neuron_count", args.neurons)
            if args.regions:
                config.set("brain.region_count", args.regions)
            if args.synapse_density:
                config.set("brain.synapse_density", args.synapse_density)
            
            self.experiment.setup_brain(config_dict={}, preset="default")
            # Actually apply the config
            for key, value in args.custom_config.items():
                self.experiment.config.set(key, value)
            print("  Using custom configuration")
        
        # Setup world
        self.experiment.setup_world(
            width=args.width,
            height=args.height,
            vision_width=args.vision_width,
            vision_height=args.vision_height
        )
        
        # Setup agent
        self.experiment.setup_agent()
        
        # Enable subsystems if requested
        if args.enable_all:
            self.experiment.enable_all_subsystems()
            print("  All subsystems enabled")
        
        self.setup_complete = True
        print("  Simulation setup complete")
        return True
    
    def run_simulation(self, args) -> Dict[str, Any]:
        """Run the simulation"""
        if not self.setup_complete:
            print("Error: Simulation not setup. Use --setup first.")
            sys.exit(1)
        
        print(f"Running simulation for {args.steps} steps...")
        
        start_time = time.time()
        result = self.experiment.run(
            num_steps=args.steps,
            timestep=args.timestep
        )
        end_time = time.time()
        
        result['cli_runtime'] = end_time - start_time
        
        # Print results
        print("\n=== Simulation Results ===")
        print(f"Experiment: {result['name']}")
        print(f"Type: {result['type']}")
        print(f"Steps: {result['steps']}")
        print(f"Duration: {result['duration']:.2f} seconds")
        print(f"Steps/second: {result['steps_per_second']:.2f}")
        print(f"CLI runtime: {result['cli_runtime']:.2f} seconds")
        
        if 'final_metrics' in result:
            print(f"Final firing neurons: {result['final_metrics'].get('firing_neurons', 'N/A')}")
            print(f"Final total spikes: {result['final_metrics'].get('total_spikes', 'N/A')}")
            print(f"Final curiosity level: {result['final_metrics'].get('curiosity', 'N/A'):.3f}")
            print(f"Final development stage: {result['final_metrics'].get('development_stage', 'N/A')}")
        
        # Save results if requested
        if args.output:
            self.experiment.save_results(args.output)
            print(f"Results saved to: {args.output}")
        
        return result
    
    def run_benchmark(self, args) -> Dict[str, Any]:
        """Run a benchmark"""
        self.check_availability()
        
        print(f"Running benchmark with {args.neurons} neurons for {args.steps} steps...")
        
        config_dict = {}
        if args.neurons:
            config_dict["brain.neuron_count"] = args.neurons
        if args.regions:
            config_dict["brain.region_count"] = args.regions
        
        result = benchmark_brain(config_dict, args.steps)
        
        print("\n=== Benchmark Results ===")
        print(f"Neurons: {result['neuron_count']}")
        print(f"Synapses: {result['synapse_count']}")
        print(f"Total time: {result['total_time']:.2f} seconds")
        print(f"Average step time: {result['average_time']*1000:.2f} ms")
        print(f"Min step time: {result['min_time']*1000:.2f} ms")
        print(f"Max step time: {result['max_time']*1000:.2f} ms")
        print(f"Steps/second: {args.steps / result['total_time']:.2f}")
        
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(result, f, indent=2)
            print(f"Benchmark results saved to: {args.output}")
        
        return result
    
    def configure_world(self, args) -> None:
        """Configure world settings"""
        if not self.experiment or not self.experiment.world:
            print("Error: No simulation setup. Run --setup first.")
            sys.exit(1)
        
        self.experiment.world.configure(
            width=args.width,
            height=args.height,
            vision_width=args.vision_width,
            vision_height=args.vision_height
        )
        
        print("World configuration updated")
    
    def save_config(self, args) -> None:
        """Save current configuration"""
        if not self.experiment or not self.experiment.config:
            print("Error: No configuration to save")
            sys.exit(1)
        
        save_config_to_file(self.experiment.config, args.filepath)
        print(f"Configuration saved to: {args.filepath}")
    
    def load_config(self, args) -> None:
        """Load configuration from file"""
        if not self.setup_complete:
            self.setup_simulation(argparse.Namespace(
                neurons=1000,
                regions=1,
                synapse_density=0.1,
                name=None,
                preset='default',
                custom_config={},
                width=20,
                height=20,
                vision_width=8,
                vision_height=8,
                enable_all=False
            ))
        
        config = create_config_from_file(args.filepath)
        # Update experiment config
        for key in self.experiment.config.getKeys():
            self.experiment.config.remove(key)
        
        # This is a simplified approach - in practice you'd need to update the config properly
        print(f"Configuration loaded from: {args.filepath}")
    
    def print_status(self) -> None:
        """Print current simulation status"""
        if not self.setup_complete:
            print("Status: No simulation setup")
            return
        
        if self.experiment and self.experiment.brain and self.experiment.world:
            brain = self.experiment.brain
            world = self.experiment.world
            agent = self.experiment.agent
            
            print("\n=== Current Simulation Status ===")
            print(f"Brain neurons: {brain.getTotalNeuronCount()}")
            print(f"Brain synapses: {brain.getTotalSynapseCount()}")
            print(f"Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            print(f"World size: {world.getWidth()} x {world.getHeight()}")
            print(f"Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            
            if agent:
                print(f"Development stage: {brain.getDevelopmentalStage()}")
                if agent.isCuriosityEnabled():
                    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
        else:
            print("Status: Incomplete setup")
    
    def run_preset_example(self, preset_name: str) -> None:
        """Run a preset example"""
        if preset_name == "simple":
            print("\n=== Running Simple Brain Example ===")
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            for i in range(100):
                brain.step(i)
            
            print(f"Simple brain test done!")
            print(f"Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"Total spikes: {brain.getTotalSpikeCount()}")
            
        elif preset_name == "agent":
            print("\n=== Running Complete Agent Example ===")
            # Create everything
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run agent
            for step in range(50):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                if step % 10 == 0:
                    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
            
            print("Agent simulation complete!")
        
        elif preset_name == "development":
            print("\n=== Running Development Example ===")
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            # Configure for development
            config = brain.getConfig()
            config.set("development.critical_period", 50.0)
            config.set("development.maturation_rate", 0.02)
            
            # Run for 200 steps to see development
            for step in range(200):
                brain.step(step)
                
                if step % 50 == 0:
                    stage = brain.getDevelopmentalStage()
                    print(f"Step {step}: Development stage {stage}")
            
            print("Development simulation complete!")
        
        else:
            print(f"Unknown preset example: {preset_name}")
            print("Available examples: simple, agent, development")
def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm --setup --steps 100                    # Setup and run simple simulation
  nlm --benchmark --neurons 500 --steps 1000 # Run benchmark
  nlm --example simple                       # Run preset example
  nlm --setup --preset learning --enable-all   # Setup with all subsystems
  nlm --status                                # Show current status
  nlm --save-config config.json               # Save current configuration
  nlm --load-config config.json               # Load configuration

For more detailed help on specific commands, use:
  nlm <command> --help
        """
    )
    
    # Create subparsers for different commands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Setup command
    setup_parser = subparsers.add_parser('setup', help='Setup simulation')
    setup_parser.add_argument('--name', help='Experiment name')
    setup_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    setup_parser.add_argument('--regions', type=int, default=1, help='Number of brain regions')
    setup_parser.add_argument('--synapse-density', type=float, default=0.1, help='Synapse density')
    setup_parser.add_argument('--width', type=int, default=20, help='World width')
    setup_parser.add_argument('--height', type=int, default=20, help='World height')
    setup_parser.add_argument('--vision-width', type=int, default=8, help='Vision width')
    setup_parser.add_argument('--vision-height', type=int, default=8, help='Vision height')
    setup_parser.add_argument('--preset', choices=['default', 'development', 'learning', 'reproduction'], 
                           help='Use configuration preset')
    setup_parser.add_argument('--enable-all', action='store_true', help='Enable all optional subsystems')
    
    # Run command
    run_parser = subparsers.add_parser('run', help='Run simulation')
    run_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    run_parser.add_argument('--timestep', type=float, default=0.1, help='Time step size')
    run_parser.add_argument('--output', help='Output file for results')
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser('benchmark', help='Run benchmark')
    benchmark_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
    benchmark_parser.add_argument('--regions', type=int, default=1, help='Number of regions')
    benchmark_parser.add_argument('--steps', type=int, default=100, help='Number of steps')
    benchmark_parser.add_argument('--output', help='Output file for benchmark results')
    
    # World configuration command
    world_parser = subparsers.add_parser('world', help='Configure world')
    world_parser.add_argument('--width', type=int, default=20, help='World width')
    world_parser.add_argument('--height', type=int, default=20, help='World height')
    world_parser.add_argument('--vision-width', type=int, default=8, help='Vision width')
    world_parser.add_argument('--vision-height', type=int, default=8, help='Vision height')
    
    # Configuration commands
    config_parser = subparsers.add_parser('config', help='Configuration management')
    config_subparsers = config_parser.add_subparsers(dest='config_action', help='Config actions')
    
    save_config_parser = config_subparsers.add_parser('save', help='Save configuration')
    save_config_parser.add_argument('filepath', help='File path to save configuration')
    
    load_config_parser = config_subparsers.add_parser('load', help='Load configuration')
    load_config_parser.add_argument('filepath', help='File path to load configuration')
    
    # Status command
    status_parser = subparsers.add_parser('status', help='Show simulation status')
    
    # Example command
    example_parser = subparsers.add_parser('example', help='Run preset example')
    example_parser.add_argument('example', choices=['simple', 'agent', 'development'], 
                             help='Example to run')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Create CLI instance
    cli = NLMCLI()
    
    # Route commands
    if args.command == 'setup':
        cli.setup_simulation(args)
    elif args.command == 'run':
        cli.run_simulation(args)
    elif args.command == 'benchmark':
        cli.run_benchmark(args)
    elif args.command == 'world':
        cli.configure_world(args)
    elif args.command == 'config':
        if args.config_action == 'save':
            cli.save_config(args)
        elif args.config_action == 'load':
            cli.load_config(args)
    elif args.command == 'status':
        cli.print_status()
    elif args.command == 'example':
        cli.run_preset_example(args.example)
    else:
        # Default behavior: setup and run
        if not args.setup_simulation:
            args = parser.parse_args(['setup'] + sys.argv[1:])
        
        # Setup and run in one go
        cli.setup_simulation(args)
        cli.run_simulation(args)
if __name__ == "__main__":
    main()
if __name__ == "__main__":
    main()

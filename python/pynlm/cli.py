#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Command Line Interface

Advanced CLI for controlling NLM neural simulations with expert-level features.
"""

import argparse
import json
import sys
import time
from pathlib import Path
from typing import Optional, Dict, Any

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent))

import pynlm

class NLMCLI:
    def __init__(self):
        self.version = "0.1.0"
        
    def create_parser(self) -> argparse.ArgumentParser:
        parser = argparse.ArgumentParser(
            prog="pynlm",
            description="NLM (Neural Learning Machine) - Advanced Neural Simulation Framework",
            epilog="Expert mode available. Use --expert for additional features."
        )
        
        parser.add_argument(
            "--version", 
            action="version", 
            version=f"pynlm {self.version}"
        )
        
        parser.add_argument(
            "--expert",
            action="store_true",
            help="Enable expert mode with advanced features"
        )
        
        subparsers = parser.add_subparsers(dest="command", help="Available commands")
        
        # Basic commands
        self._add_run_command(subparsers)
        self._add_simulate_command(subparsers)
        self._add_config_command(subparsers)
        
        # Expert commands (if expert mode enabled)
        if True:  # Will check expert flag later
            self._add_expert_commands(subparsers)
        
        return parser
    
    def _add_run_command(self, subparsers):
        run_parser = subparsers.add_parser("run", help="Run a simulation")
        run_parser.add_argument(
            "--steps", 
            type=int, 
            default=100,
            help="Number of simulation steps to run"
        )
        run_parser.add_argument(
            "--world-width", 
            type=int, 
            default=10,
            help="World width"
        )
        run_parser.add_argument(
            "--world-height", 
            type=int, 
            default=10,
            help="World height"
        )
        run_parser.add_argument(
            "--config", 
            type=str, 
            help="Path to configuration file"
        )
        run_parser.add_argument(
            "--output", 
            type=str, 
            help="Output file for results"
        )
        
        run_parser.set_defaults(func=self._run_simulation)
    
    def _add_simulate_command(self, subparsers):
        sim_parser = subparsers.add_parser("simulate", help="Start interactive simulation")
        sim_parser.add_argument(
            "--visualize", 
            action="store_true",
            help="Enable visualization (if available)"
        )
        sim_parser.add_argument(
            "--checkpoint", 
            type=str, 
            help="Path for checkpoint files"
        )
        sim_parser.set_defaults(func=self._simulate_interactive)
    
    def _add_config_command(self, subparsers):
        config_parser = subparsers.add_parser("config", help="Manage configuration")
        
        config_subparsers = config_parser.add_subparsers(dest="config_action")
        
        # Show config
        show_parser = config_subparsers.add_parser("show", help="Show current configuration")
        show_parser.set_defaults(func=self._show_config)
        
        # Load config
        load_parser = config_subparsers.add_parser("load", help="Load configuration from file")
        load_parser.add_argument("file", help="Configuration file path")
        load_parser.set_defaults(func=self._load_config)
        
        # Save config
        save_parser = config_subparsers.add_parser("save", help="Save configuration to file")
        save_parser.add_argument("file", help="Configuration file path")
        save_parser.set_defaults(func=self._save_config)
    
    def _add_expert_commands(self, subparsers):
        # Advanced experiment commands
        exp_parser = subparsers.add_parser("experiment", help="Run advanced experiments")
        exp_parser.add_argument(
            "type", 
            choices=["replay", "ablation", "scaling", "development"],
            help="Type of experiment"
        )
        exp_parser.add_argument(
            "--iterations", 
            type=int, 
            default=10,
            help="Number of iterations"
        )
        exp_parser.add_argument(
            "--output-dir", 
            type=str, 
            help="Output directory"
        )
        exp_parser.set_defaults(func=self._run_experiment)
        
        # Performance monitoring
        perf_parser = subparsers.add_parser("monitor", help="Start performance monitoring")
        perf_parser.set_defaults(func=self._start_monitoring)
        
        # Memory inspection
        mem_parser = subparsers.add_parser("memory", help="Inspect memory usage")
        mem_parser.add_argument(
            "--type", 
            choices=["working", "episodic", "semantic"],
            help="Type of memory to inspect"
        )
        mem_parser.set_defaults(func=self._inspect_memory)
        
        # Neural analysis
        neural_parser = subparsers.add_parser("neural", help="Analyze neural activity")
        neural_parser.add_argument(
            "--metric", 
            choices=["firing_rate", "spike_patterns", "connectivity"],
            help="Metric to analyze"
        )
        neural_parser.set_defaults(func=self._analyze_neural)
        
        # Checkpoint management
        checkpoint_parser = subparsers.add_parser("checkpoint", help="Manage checkpoints")
        checkpoint_subparsers = checkpoint_parser.add_subparsers(dest="checkpoint_action")
        
        # Save checkpoint
        save_cp_parser = checkpoint_subparsers.add_parser("save", help="Save current state")
        save_cp_parser.add_argument("path", help="Checkpoint path")
        save_cp_parser.set_defaults(func=self._save_checkpoint)
        
        # Load checkpoint
        load_cp_parser = checkpoint_subparsers.add_parser("load", help="Load saved state")
        load_cp_parser.add_argument("path", help="Checkpoint path")
        load_cp_parser.set_defaults(func=self._load_checkpoint)
        
        # List checkpoints
        list_cp_parser = checkpoint_subparsers.add_parser("list", help="List available checkpoints")
        list_cp_parser.set_defaults(func=self._list_checkpoints)
    
    def _run_simulation(self, args):
        print("Running NLM simulation...")
        print(f"Steps: {args.steps}")
        print(f"World size: {args.world_width}x{args.world_height}")
        
        try:
            # Create configuration
            config = pynlm.createDefaultConfig()
            
            # Load custom config if specified
            if args.config:
                config.loadFromFile(args.config)
            
            # Modify config for world size
            config.set("environment_width", args.world_width)
            config.set("environment_height", args.world_height)
            
            # Create brain and world
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(
                width=args.world_width,
                height=args.world_height,
                visionWidth=8,
                visionHeight=8
            )
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run simulation
            for step in range(args.steps):
                world.update(0.1)
                
                if step % 10 == 0:
                    print(f"Step {step}/{args.steps}")
                
                # Agent processes world
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Brain thinks
                brain.step(step)
                
                # Get action and apply to world
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Save results if requested
            if args.output:
                self._save_results(brain, world, agent, args.output)
            
            print("Simulation completed successfully!")
            
        except Exception as e:
            print(f"Error during simulation: {e}")
            return 1
    
    def _simulate_interactive(self, args):
        print("Starting interactive simulation...")
        print("Press Ctrl+C to exit")
        
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(20, 20, 8, 8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable expert features
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            print("Simulation started. Commands:")
            print("  s - step one iteration")
            print("  r - reset world")
            print("  q - quit")
            
            while True:
                try:
                    cmd = input("\nCommand: ").strip().lower()
                    
                    if cmd == 's':
                        world.update(0.1)
                        percept = world.getSensoryPercept()
                        agent.processSensoryInput(percept)
                        brain.step(world.getSimulationTime())
                        action = agent.decodeMotorCommand()
                        world.applyMotorCommand(action, world.getSimulationTime())
                        
                        print(f"Step: firing={brain.getFiringNeuronCount()}, "
                              f"spikes={brain.getTotalSpikeCount()}")
                    
                    elif cmd == 'r':
                        world.reset()
                        print("World reset")
                    
                    elif cmd == 'q':
                        print("Exiting...")
                        break
                    
                    else:
                        print(f"Unknown command: {cmd}")
                        
                except KeyboardInterrupt:
                    print("\nInterrupted")
                    break
        
        except Exception as e:
            print(f"Error: {e}")
            return 1
    
    def _show_config(self, args):
        config = pynlm.createDefaultConfig()
        print("Current configuration:")
        print(config.summary())
    
    def _load_config(self, args):
        try:
            config = pynlm.createDefaultConfig()
            if config.loadFromFile(args.file):
                print(f"Configuration loaded from {args.file}")
            else:
                print(f"Failed to load configuration from {args.file}")
        except Exception as e:
            print(f"Error loading config: {e}")
            return 1
    
    def _save_config(self, args):
        try:
            config = pynlm.createDefaultConfig()
            if config.saveToFile(args.file):
                print(f"Configuration saved to {args.file}")
            else:
                print(f"Failed to save configuration to {args.file}")
        except Exception as e:
            print(f"Error saving config: {e}")
            return 1
    
    def _run_experiment(self, args):
        print(f"Running {args.type} experiment...")
        print("This is an advanced feature for expert users.")
        print("Experiment type: {}".format(args.type))
        print("Iterations: {}".format(args.iterations))
        
        # TODO: Implement advanced experiment types
        if args.output_dir:
            print(f"Output will be saved to: {args.output_dir}")
        
        print("Experiment setup complete.")
    
    def _start_monitoring(self, args):
        print("Starting performance monitoring...")
        print("Monitoring neural activity, memory usage, and system performance")
        print("This is an advanced feature that requires continuous sampling")
        print("Monitoring started (simulated - requires actual implementation)")
    
    def _inspect_memory(self, args):
        print("Memory inspection...")
        
        if args.type == "working":
            print("Working memory contents:")
            # Access working memory if available
            # brain.getWorkingMemory().getActivePatterns()
            print("(Working memory inspection requires actual implementation)")
        
        elif args.type == "episodic":
            print("Episodic memory contents:")
            print("(Episodic memory inspection requires actual implementation)")
        
        elif args.type == "semantic":
            print("Semantic memory contents:")
            print("(Semantic memory inspection requires actual implementation)")
    
    def _analyze_neural(self, args):
        print(f"Neural analysis: {args.metric}")
        print("This requires actual neural data from a running simulation")
    
    def _save_checkpoint(self, args):
        print(f"Saving checkpoint to: {args.path}")
        print("Checkpointing requires actual brain state serialization")
    
    def _load_checkpoint(self, args):
        print(f"Loading checkpoint from: {args.path}")
        print("Checkpoint loading requires actual brain state deserialization")
    
    def _list_checkpoints(self, args):
        print("Available checkpoints:")
        print("(Requires checkpoint directory scanning)")
    
    def _save_results(self, brain, world, agent, output_path):
        print(f"Saving results to {output_path}")
        
        results = {
            "total_spikes": brain.getTotalSpikeCount(),
            "firing_neurons": brain.getFiringNeuronCount(),
            "active_neurons": brain.getActiveNeuronCount(),
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "simulation_steps": world.getSimulationTime(),
            "development_stage": brain.getDevelopmentalStage(),
            "reward_modulation": agent.isRewardModulationEnabled(),
            "structural_plasticity": agent.isStructuralPlasticityEnabled(),
            "development": agent.isDevelopmentEnabled(),
            "curiosity": agent.isCuriosityEnabled(),
        }
        
        try:
            with open(output_path, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Results saved successfully")
        except Exception as e:
            print(f"Error saving results: {e}")
    
    def main(self, argv=None):
        parser = self.create_parser()
        args = parser.parse_args(argv)
        
        if args.command is None:
            parser.print_help()
            return 0
        
        try:
            return args.func(args)
        except Exception as e:
            print(f"Error: {e}")
            return 1
if __name__ == "__main__":
    cli = NLMCLI()
    sys.exit(cli.main())
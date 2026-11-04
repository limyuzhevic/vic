#!/usr/bin/env python3
"""
NLM Command-Line Interface (CLI)
===============================

Advanced command-line tools for NLM users.
Provides utilities for configuration management, simulation control, and analysis.
"""

import pynlm
import json
import sys
import argparse
import time
import os
from pathlib import Path
from typing import List, Dict, Any, Optional

class NLMCLI:
    """Command-line interface for NLM operations"""
    
    def __init__(self):
        self.config_file = "nlm_config.json"
    
    def run_brain(self, args):
        """Run a simple brain simulation"""
        print(f"Running brain simulation...")
        
        # Create brain
        config = pynlm.createBrain(pynlm.createDefaultConfig())
        config.initialize()
        
        # Run simulation
        for step in range(args.steps):
            config.step(step)
            
            if args.verbose and step % args.verbose == 0:
                print(f"Step {step}: {config.getFiringNeuronCount()} firing neurons")
        
        print(f"Simulation complete. Total spikes: {config.getTotalSpikeCount()}")
        
        if args.output:
            self.save_config(config, args.output)
    
    def run_agent(self, args):
        """Run a complete agent simulation"""
        print(f"Running agent simulation...")
        
        # Create components
        config = pynlm.createBrain(pynlm.createConfigWithSettings(
            neuron_count=args.neurons,
            region_count=args.regions,
            synapse_density=args.synapse_density
        ))
        config.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(
            width=args.width,
            height=args.height,
            visionWidth=args.vision_width,
            visionHeight=args.vision_height
        )
        world.reset()
        world.setAgentStart(args.start_x, args.start_y)
        
        agent = pynlm.createAgentBrain(config)
        agent.initialize(world)
        
        # Enable learning if requested
        if args.learn:
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            agent.enableStructuralPlasticity(True)
        
        # Run simulation
        stats = {
            'steps': args.steps,
            'final_firing': 0,
            'final_curiosity': 0.0,
            'final_novelty': 0.0,
            'action_counts': {}
        }
        
        for step in range(args.steps):
            world.update(0.1)
            
            # Get percept and process
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            config.step(step)
            
            # Action and execution
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Neuromodulation
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Track statistics
            stats['final_firing'] = config.getFiringNeuronCount()
            stats['final_curiosity'] = agent.getCuriosityLevel()
            stats['final_novelty'] = agent.getNoveltyLevel()
            
            action_name = pynlm.MotorCommand(action).name if hasattr(pynlm.MotorCommand, 'name') else str(action)
            stats['action_counts'][action_name] = stats['action_counts'].get(action_name, 0) + 1
            
            if args.verbose and step % args.verbose == 0:
                print(f"Step {step}: {config.getFiringNeuronCount()} firing, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
        
        print(f"\nSimulation complete!")
        print(f"Final firing neurons: {stats['final_firing']}")
        print(f"Final curiosity: {stats['final_curiosity']:.3f}")
        print(f"Final novelty: {stats['final_novelty']:.3f}")
        print(f"Action distribution:")
        for action, count in stats['action_counts'].items():
            print(f"  {action}: {count}")
        
        if args.output:
            self.save_stats(stats, args.output)
    
    def create_config(self, args):
        """Create and save configuration"""
        print(f"Creating configuration...")
        
        if args.file:
            # Load existing config
            config = self.load_config(args.file)
            if not config:
                print(f"Warning: Could not load config file {args.file}")
                config = pynlm.createDefaultConfig()
        else:
            config = pynlm.createDefaultConfig()
        
        # Modify config
        if args.neurons:
            config.set("neuron_count", args.neurons)
        if args.regions:
            config.set("region_count", args.regions)
        if args.synapse_density:
            config.set("connection_probability", args.synapse_density)
        
        # Save config
        if args.file:
            self.save_config(config, args.file)
            print(f"Configuration saved to {args.file}")
        else:
            print(f"Current configuration: {config.summary()}")
    
    def analyze_brain(self, args):
        """Analyze brain state and save results"""
        print(f"Analyzing brain...")
        
        config = pynlm.createBrain(pynlm.createDefaultConfig())
        config.initialize()
        
        # Run simulation to collect data
        for step in range(args.steps):
            config.step(step)
        
        # Collect analysis data
        analysis = {
            'neuron_count': config.getTotalNeuronCount(),
            'synapse_count': config.getTotalSynapseCount(),
            'firing_neuron_count': config.getFiringNeuronCount(),
            'average_firing_rate': config.getAverageFiringRate(),
            'excitement_inhibition_ratio': config.getExcitationInhibitionRatio(),
            'total_spikes': config.getTotalSpikeCount(),
            'developmental_stage': config.getDevelopmentalStage(),
            'step_count': args.steps
        }
        
        print(f"Brain Analysis Results:")
        for key, value in analysis.items():
            print(f"  {key}: {value}")
        
        if args.output:
            self.save_analysis(analysis, args.output)
    
    def run_benchmark(self, args):
        """Run benchmark tests"""
        print(f"Running benchmark tests...")
        
        benchmarks = []
        
        # Benchmark 1: Simple brain
        start_time = time.time()
        brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
        brain1.initialize()
        for step in range(args.benchmark_steps):
            brain1.step(step)
        end_time = time.time()
        
        benchmarks.append({
            'name': 'SimpleBrain',
            'neuron_count': brain1.getTotalNeuronCount(),
            'steps': args.benchmark_steps,
            'time_seconds': end_time - start_time,
            'spikes_per_second': brain1.getTotalSpikeCount() / (end_time - start_time)
        })
        
        # Benchmark 2: Brain with world
        start_time = time.time()
        brain2 = pynlm.createBrain(pynlm.createConfigWithSettings(neuron_count=500))
        brain2.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain2)
        agent.initialize(world)
        
        for step in range(args.benchmark_steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain2.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        
        benchmarks.append({
            'name': 'AgentWorld',
            'neuron_count': brain2.getTotalNeuronCount(),
            'steps': args.benchmark_steps,
            'time_seconds': end_time - start_time,
            'spikes_per_second': brain2.getTotalSpikeCount() / (end_time - start_time)
        })
        
        # Print benchmark results
        print(f"\nBenchmark Results:")
        for bm in benchmarks:
            print(f"  {bm['name']}:")
            print(f"    Neurons: {bm['neuron_count']}")
            print(f"    Steps: {bm['steps']}")
            print(f"    Time: {bm['time_seconds']:.2f}s")
            print(f"    Spikes/sec: {bm['spikes_per_second']:.0f}")
        
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(benchmarks, f, indent=2)
            print(f"Benchmark results saved to {args.output}")
    
    def export_config(self, args):
        """Export configuration to different formats"""
        print(f"Exporting configuration...")
        
        config = pynlm.createBrain(pynlm.createDefaultConfig())
        config.initialize()
        
        # Collect configuration data
        config_data = {
            'neuron_count': config.getTotalNeuronCount(),
            'synapse_count': config.getTotalSynapseCount(),
            'region_count': config.getRegionCount(),
            'developmental_stage': config.getDevelopmentalStage(),
            'timestamp': time.time()
        }
        
        # Export based on format
        if args.format == 'json':
            output_file = args.output or "nlm_config_export.json"
            with open(output_file, 'w') as f:
                json.dump(config_data, f, indent=2)
            print(f"Configuration exported to JSON: {output_file}")
            
        elif args.format == 'csv':
            output_file = args.output or "nlm_config_export.csv"
            with open(output_file, 'w') as f:
                f.write("metric,value\n")
                for key, value in config_data.items():
                    f.write(f"{key},{value}\n")
            print(f"Configuration exported to CSV: {output_file}")
            
        elif args.format == 'python':
            output_file = args.output or "nlm_config_export.py"
            with open(output_file, 'w') as f:
                f.write("""# NLM Configuration Export
# Generated by NLM CLI

config_data = {
""")
                for key, value in config_data.items():
                    f.write(f"    '{key}': {repr(value)},\n")
                f.write("}")
            print(f"Configuration exported to Python: {output_file}")
    
    def simulate_preset(self, args):
        """Run a predefined simulation preset"""
        print(f"Running preset simulation: {args.preset}")
        
        presets = {
            'simple': {
                'neurons': 100,
                'regions': 1,
                'width': 10,
                'height': 10,
                'vision_width': 4,
                'vision_height': 4,
                'steps': 50
            },
            'medium': {
                'neurons': 500,
                'regions': 2,
                'width': 20,
                'height': 20,
                'vision_width': 8,
                'vision_height': 8,
                'steps': 100
            },
            'complex': {
                'neurons': 2000,
                'regions': 4,
                'width': 30,
                'height': 30,
                'vision_width': 16,
                'vision_height': 16,
                'steps': 200
            }
        }
        
        if args.preset not in presets:
            print(f"Error: Unknown preset '{args.preset}'")
            return
        
        preset = presets[args.preset]
        
        # Create and run simulation
        config = pynlm.createBrain(pynlm.createConfigWithSettings(
            neuron_count=preset['neurons'],
            region_count=preset['regions']
        ))
        config.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(
            preset['width'],
            preset['height'],
            preset['vision_width'],
            preset['vision_height']
        )
        world.reset()
        
        agent = pynlm.createAgentBrain(config)
        agent.initialize(world)
        
        if args.learn:
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
        
        print(f"Running {args.preset} preset (neurons: {preset['neurons']}, "
              f"steps: {preset['steps']})")
        
        for step in range(preset['steps']):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            config.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if args.verbose and step % args.verbose == 0:
                print(f"Step {step}: {config.getFiringNeuronCount()} firing")
        
        print(f"Preset simulation complete!")
        print(f"Final neurons: {config.getTotalNeuronCount()}")
        print(f"Final spikes: {config.getTotalSpikeCount()}")
    
    def save_config(self, config, filename):
        """Save configuration to file"""
        with open(filename, 'w') as f:
            f.write(f"# NLM Configuration\n")
            f.write(f"# Generated: {time.time()}\n")
            f.write(f"Neuron Count: {config.getTotalNeuronCount()}\n")
            f.write(f"Synapse Count: {config.getTotalSynapseCount()}\n")
            f.write(f"Region Count: {config.getRegionCount()}\n")
    
    def load_config(self, filename):
        """Load configuration from file"""
        if not os.path.exists(filename):
            print(f"Error: File {filename} does not exist")
            return None
        
        try:
            with open(filename, 'r') as f:
                # Parse config file (simplified)
                content = f.read()
                print(f"Loaded config file (manual parsing required)")
                return pynlm.createDefaultConfig()
        except Exception as e:
            print(f"Error loading config file: {e}")
            return None
    
    def save_stats(self, stats, filename):
        """Save statistics to file"""
        with open(filename, 'w') as f:
            json.dump(stats, f, indent=2)
        print(f"Statistics saved to {filename}")
    
    def save_analysis(self, analysis, filename):
        """Save analysis results to file"""
        with open(filename, 'w') as f:
            json.dump(analysis, f, indent=2)
        print(f"Analysis saved to {filename}")

def create_parser():
    """Create argument parser"""
    parser = argparse.ArgumentParser(
        description="NLM Command-Line Interface - Advanced NLM operations",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run a simple brain simulation
  nlm-cli run brain --steps 100
  
  # Run an agent with world interaction
  nlm-cli run agent --neurons 500 --width 20 --height 20 --steps 200
  
  # Create custom configuration
  nlm-cli create config --neurons 1000 --regions 3 --synapse-density 0.05
  
  # Analyze brain state
  nlm-cli analyze brain --steps 100 --output analysis.json
  
  # Run benchmark tests
  nlm-cli benchmark --benchmark-steps 100 --output benchmarks.json
  
  # Run preset simulation
  nlm-cli run preset simple
        """.strip()
    )
    
    # Main command
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Run brain command
    parser_brain = subparsers.add_parser("run", help="Run simulation")
    run_subparsers = parser_brain.add_subparsers(dest="type", help="Simulation type")
    
    # Simple brain
    parser_simple = run_subparsers.add_parser("brain", help="Run simple brain simulation")
    parser_simple.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser_simple.add_argument("--verbose", type=int, default=0, help="Verbose output every N steps")
    parser_simple.add_argument("--output", help="Output file for results")
    
    # Agent simulation
    parser_agent = run_subparsers.add_parser("agent", help="Run agent simulation with world")
    parser_agent.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    parser_agent.add_argument("--regions", type=int, default=2, help="Number of regions")
    parser_agent.add_argument("--synapse-density", type=float, default=0.1, help="Synapse density")
    parser_agent.add_argument("--width", type=int, default=20, help="World width")
    parser_agent.add_argument("--height", type=int, default=20, help="World height")
    parser_agent.add_argument("--vision-width", type=int, default=8, help="Vision width")
    parser_agent.add_argument("--vision-height", type=int, default=8, help="Vision height")
    parser_agent.add_argument("--start-x", type=float, default=10.0, help="Agent start X position")
    parser_agent.add_argument("--start-y", type=float, default=10.0, help="Agent start Y position")
    parser_agent.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser_agent.add_argument("--learn", action="store_true", help="Enable learning subsystems")
    parser_agent.add_argument("--verbose", type=int, default=0, help="Verbose output every N steps")
    parser_agent.add_argument("--output", help="Output file for results")
    
    # Preset simulation
    parser_preset = run_subparsers.add_parser("preset", help="Run predefined simulation preset")
    parser_preset.add_argument("preset", choices=["simple", "medium", "complex"], help="Preset to run")
    parser_preset.add_argument("--learn", action="store_true", help="Enable learning")
    parser_preset.add_argument("--steps", type=int, default=0, help="Override preset steps (0 = use preset)")
    parser_preset.add_argument("--verbose", type=int, default=0, help="Verbose output every N steps")
    
    # Config commands
    parser_config = subparsers.add_parser("config", help="Configuration management")
    config_subparsers = parser_config.add_subparsers(dest="action", help="Config action")
    
    # Create config
    parser_create = config_subparsers.add_parser("create", help="Create or modify configuration")
    parser_create.add_argument("--neurons", type=int, help="Number of neurons")
    parser_create.add_argument("--regions", type=int, help="Number of regions")
    parser_create.add_argument("--synapse-density", type=float, help="Synapse density")
    parser_create.add_argument("--file", help="Configuration file to load/save")
    
    # Config analysis
    parser_analyze = subparsers.add_parser("analyze", help="Analyze brain state")
    parser_analyze.add_argument("--steps", type=int, default=100, help="Number of simulation steps")
    parser_analyze.add_argument("--output", help="Output file for analysis results")
    
    # Benchmark command
    parser_benchmark = subparsers.add_parser("benchmark", help="Run benchmark tests")
    parser_benchmark.add_argument("--benchmark-steps", type=int, default=50, help="Number of steps per benchmark")
    parser_benchmark.add_argument("--output", help="Output file for benchmark results")
    
    # Export command
    parser_export = subparsers.add_parser("export", help="Export configuration to different formats")
    parser_export.add_argument("format", choices=["json", "csv", "python"], help="Export format")
    parser_export.add_argument("--output", help="Output file (default: auto-generated)")
    
    return parser

def main():
    """Main CLI entry point"""
    parser = create_parser()
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    cli = NLMCLI()
    
    try:
        if args.command == "run":
            if args.type == "brain":
                cli.run_brain(args)
            elif args.type == "agent":
                cli.run_agent(args)
            elif args.type == "preset":
                cli.simulate_preset(args)
        elif args.command == "config":
            cli.create_config(args)
        elif args.command == "analyze":
            cli.analyze_brain(args)
        elif args.command == "benchmark":
            cli.run_benchmark(args)
        elif args.command == "export":
            cli.export_config(args)
        else:
            print(f"Error: Unknown command {args.command}")
            parser.print_help()
    
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user")
    except Exception as e:
        print(f"Error: {e}")
        if "-v" in sys.argv or "--verbose" in sys.argv:
            import traceback
            traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
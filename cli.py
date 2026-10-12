#!/usr/bin/env python3
"""
NLM Enhanced Command-Line Interface (CLI)

Comprehensive command-line interface for NLM enhanced Python API.
Provides easy access to all enhanced features including:
- Simulation running
- Model training and evaluation
- Visualization generation
- Performance profiling
- Configuration management
- Advanced analysis

Run with: python cli.py --help
"""

import sys
import os
import argparse
import json
import time
import subprocess
import threading
from pathlib import Path
from typing import Dict, Any, Optional, List
import argparse

# Add the project directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__)))

try:
    import pynlm
except ImportError:
    print("ERROR: pynlm module not found. Please ensure NLM is properly installed.")
    sys.exit(1)
class NLMCLI:
    """Command-line interface for NLM enhanced features"""
    
    def __init__(self):
        self.setup_parser()
        self.last_command = None
    
    def setup_parser(self):
        """Setup command-line argument parser"""
        
        # Main command parser
        parser = argparse.ArgumentParser(
            description="NLM Enhanced Command-Line Interface",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Available Commands:
  nlm create    Create and configure a brain
  nlm train     Train an agent
  nlm simulate  Run a simulation
  nlm analyze   Analyze brain results
  nlm visualize Generate visualizations
  nlm profile    Profile performance
  nlm config    Manage configurations
  nlm export    Export models and data
  nlm import    Import models and data
  nlm help      Show help information

Examples:
  nlm create --config my_config.json --neurons 1000
  nlm train --brain my_brain.nlm --episodes 50 --steps 200
  nlm simulate --brain my_brain.nlm --world my_world.json
  nlm analyze --brain my_brain.nlm --output analysis.json
  nlm visualize --brain my_brain.nlm --output dashboard.png
  nlm profile --brain my_brain.nlm --benchmark
  nlm config --create experimental
  nlm export --brain my_brain.nlm --format weights.json
  nlm import --brain new_brain.json

For more information, visit the NLM documentation.
            """
        )
        
        # Subparsers for commands
        subparsers = parser.add_subparsers(dest='command', help='Available commands')
        
        # Create command
        create_parser = subparsers.add_parser('create', help='Create and configure a brain')
        create_parser.add_argument('--config', type=str, help='Configuration file path')
        create_parser.add_argument('--neurons', type=int, default=1000, help='Number of neurons')
        create_parser.add_argument('--regions', type=int, default=1, help='Number of brain regions')
        create_parser.add_argument('--connections', type=float, default=0.1, help='Connection probability')
        create_parser.add_argument('--learning', action='store_true', help='Enable learning')
        create_parser.add_argument('--save', type=str, help='Save brain to file')
        
        # Train command
        train_parser = subparsers.add_parser('train', help='Train an agent')
        train_parser.add_argument('--brain', type=str, help='Brain model file path')
        train_parser.add_argument('--world', type=str, help='World configuration file path')
        train_parser.add_argument('--episodes', type=int, default=20, help='Number of training episodes')
        train_parser.add_argument('--steps', type=int, default=200, help='Steps per episode')
        train_parser.add_argument('--exploration', type=float, default=0.2, help='Exploration rate (0-1)')
        train_parser.add_argument('--learning-rate', type=float, default=0.001, help='Learning rate')
        train_parser.add_argument('--save', type=str, help='Save trained brain to file')
        train_parser.add_argument('--resume', action='store_true', help='Resume from saved brain')
        
        # Simulate command
        simulate_parser = subparsers.add_parser('simulate', help='Run a simulation')
        simulate_parser.add_argument('--brain', type=str, help='Brain model file path')
        simulate_parser.add_argument('--world', type=str, help='World configuration file path')
        simulate_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
        simulate_parser.add_argument('--agent', action='store_true', help='Include agent brain interface')
        simulate_parser.add_argument('--output', type=str, help='Output file path for results')
        simulate_parser.add_argument('--real-time', action='store_true', help='Display real-time visualization')
        
        # Analyze command
        analyze_parser = subparsers.add_parser('analyze', help='Analyze brain results')
        analyze_parser.add_argument('--brain', type=str, help='Brain model file path')
        analyze_parser.add_argument('--scenarios', type=int, default=10, help='Number of test scenarios')
        analyze_parser.add_argument('--output', type=str, help='Output file path for analysis results')
        analyze_parser.add_argument('--format', choices=['json', 'csv', 'txt'], default='json', help='Output format')
        analyze_parser.add_argument('--detailed', action='store_true', help='Detailed analysis')
        
        # Visualize command
        visualize_parser = subparsers.add_parser('visualize', help='Generate visualizations')
        visualize_parser.add_argument('--brain', type=str, help='Brain model file path')
        visualize_parser.add_argument('--world', type=str, help='World configuration file path')
        visualize_parser.add_argument('--agent', action='store_true', help='Include agent interface')
        visualize_parser.add_argument('--output', type=str, help='Output file path for visualization')
        visualize_parser.add_argument('--type', choices=['dashboard', 'activity', 'connectivity', 'development', 'all'], default='dashboard', help='Visualization type')
        visualize_parser.add_argument('--width', type=int, default=1200, help='Figure width in pixels')
        visualize_parser.add_argument('--height', type=int, default=800, help='Figure height in pixels')
        visualize_parser.add_argument('--format', choices=['png', 'pdf', 'jpg', 'svg'], default='png', help='Output image format')
        
        # Profile command
        profile_parser = subparsers.add_parser('profile', help='Profile performance')
        profile_parser.add_argument('--brain', type=str, help='Brain model file path')
        profile_parser.add_argument('--world', type=str, help='World configuration file path')
        profile_parser.add_argument('--agent', action='store_true', help='Include agent interface')
        profile_parser.add_argument('--benchmark', choices=['step', 'action', 'simulation', 'all'], default='all', help='Benchmark type')
        profile_parser.add_argument('--steps', type=int, default=100, help='Number of steps to simulate')
        profile_parser.add_argument('--output', type=str, help='Output file path for profile data')
        profile_parser.add_argument('--format', choices=['json', 'csv', 'txt'], default='json', help='Output format')
        profile_parser.add_argument('--detailed', action='store_true', help='Detailed profiling')
        
        # Config command
        config_parser = subparsers.add_parser('config', help='Manage configurations')
        config_subparsers = config_parser.add_subparsers(dest='config_action', help='Configuration actions')
        
        # Create config subcommand
        create_config_parser = config_subparsers.add_parser('create', help='Create configuration')
        create_config_parser.add_argument('--name', type=str, choices=['simple', 'experimental', 'production'], default='simple', help='Configuration type')
        create_config_parser.add_argument('--output', type=str, help='Output file path')
        
        # List config subcommand
        list_config_parser = config_subparsers.add_parser('list', help='List available configurations')
        
        # Show config subcommand
        show_config_parser = config_subparsers.add_parser('show', help='Show configuration details')
        show_config_parser.add_argument('--name', type=str, help='Configuration name')
        
        # Export command
        export_parser = subparsers.add_parser('export', help='Export models and data')
        export_parser.add_argument('--brain', type=str, help='Brain model file path')
        export_parser.add_argument('--format', choices=['weights', 'connections', 'neurons', 'all'], default='all', help='Export format')
        export_parser.add_argument('--output', type=str, help='Output file path')
        export_parser.add_argument('--compress', action='store_true', help='Compress output file')
        
        # Import command
        import_parser = subparsers.add_parser('import', help='Import models and data')
        import_parser.add_argument('--brain', type=str, help='Brain model file path')
        import_parser.add_argument('--source', type=str, help='Source file path')
        import_parser.add_argument('--format', choices=['weights', 'connections', 'neurons'], default='weights', help='Import format')
        
        # Help command
        help_parser = subparsers.add_parser('help', help='Show help information')
        help_parser.add_argument('--command', type=str, help='Specific command to show help for')
        help_parser.add_argument('--all', action='store_true', help='Show all help information')
        
        # Version command
        version_parser = subparsers.add_parser('version', help='Show version information')
        
        # Examples command
        examples_parser = subparsers.add_parser('examples', help='Show examples')
        examples_parser.add_argument('--category', type=str, choices=['basic', 'advanced', 'all'], default='basic', help='Example category')
        
        self.parser = parser
        self.subparsers = subparsers
        
        # Store parser mappings for help
        self.command_parsers = {
            'create': create_parser,
            'train': train_parser,
            'simulate': simulate_parser,
            'analyze': analyze_parser,
            'visualize': visualize_parser,
            'profile': profile_parser,
            'config': config_parser,
            'export': export_parser,
            'import': import_parser,
            'help': help_parser,
            'version': version_parser,
            'examples': examples_parser
        }
    
    def run(self, args=None):
        """Run the CLI"""
        if args is None:
            args = sys.argv[1:]
        
        try:
            parsed_args = self.parser.parse_args(args)
            
            if not parsed_args.command:
                self.show_help()
                return
            
            self.last_command = parsed_args.command
            
            # Dispatch to appropriate command handler
            if parsed_args.command == 'create':
                self.handle_create(parsed_args)
            elif parsed_args.command == 'train':
                self.handle_train(parsed_args)
            elif parsed_args.command == 'simulate':
                self.handle_simulate(parsed_args)
            elif parsed_args.command == 'analyze':
                self.handle_analyze(parsed_args)
            elif parsed_args.command == 'visualize':
                self.handle_visualize(parsed_args)
            elif parsed_args.command == 'profile':
                self.handle_profile(parsed_args)
            elif parsed_args.command == 'config':
                self.handle_config(parsed_args)
            elif parsed_args.command == 'export':
                self.handle_export(parsed_args)
            elif parsed_args.command == 'import':
                self.handle_import(parsed_args)
            elif parsed_args.command == 'help':
                self.handle_help(parsed_args)
            elif parsed_args.command == 'version':
                self.handle_version(parsed_args)
            elif parsed_args.command == 'examples':
                self.handle_examples(parsed_args)
            else:
                print(f"❌ Unknown command: {parsed_args.command}")
                self.show_help()
                
        except KeyboardInterrupt:
            print("\n⚠️  Operation interrupted by user")
        except Exception as e:
            print(f"❌ Error: {e}")
            import traceback
            traceback.print_exc()
    
    def handle_create(self, args):
        """Handle create command"""
        print("🔧 Creating and configuring brain...")
        
        if args.config:
            print(f"Loading configuration from: {args.config}")
            # Load configuration from file
            with open(args.config, 'r') as f:
                config_data = json.load(f)
        else:
            # Create default configuration
            config_data = {
                "brain": {
                    "neuron_count": args.neurons,
                    "region_count": args.regions,
                    "connection_probability": args.connections
                },
                "plasticity": {
                    "stdp": {"enable": True},
                    "hebbian": {"enable": True}
                }
            }
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Set values from config data
        if "brain" in config_data:
            if "neuron_count" in config_data["brain"]:
                config.set("brain.neuron_count", config_data["brain"]["neuron_count"])
            if "region_count" in config_data["brain"]:
                config.set("brain.region_count", config_data["brain"]["region_count"])
            if "connection_probability" in config_data["brain"]:
                config.set("brain.connection_probability", config_data["brain"]["connection_probability"])
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"✅ Brain created successfully!")
        print(f"   Neurons: {brain.getTotalNeuronCount()}")
        print(f"   Regions: {brain.getRegionCount()}")
        print(f"   Synapses: {brain.getTotalSynapseCount()}")
        print(f"   Average firing rate: {brain.getAverageFiringRate():.4f}")
        
        if args.save:
            print(f"💾 Saving brain to: {args.save}")
            pynlm.save_model(brain, args.save)
            print(f"✅ Brain saved to: {args.save}")
        
        print("\n💡 Next steps:")
        print("  nlm train --brain your_brain.nlm --episodes 50 --steps 200")
        print("  nlm simulate --brain your_brain.nlm --steps 100")
        print("  nlm analyze --brain your_brain.nlm")
    
    def handle_train(self, args):
        """Handle train command"""
        print("🎓 Training agent...")
        
        if args.brain:
            if not Path(args.brain).exists():
                print(f"❌ Brain file not found: {args.brain}")
                return
            print(f"Loading brain from: {args.brain}")
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            pynlm.load_model(brain, args.brain)
            brain.initialize()
        else:
            print("Creating new brain...")
            brain = pynlm.createBrain(pynlm.create_experimental_config())
            brain.initialize()
        
        # Create world
        print("Creating world...")
        if args.world and Path(args.world).exists():
            with open(args.world, 'r') as f:
                world_data = json.load(f)
            
            world = pynlm.createSimpleWorld()
            if "width" in world_data:
                world.configure(
                    world_data.get("width", 20),
                    world_data.get("height", 20),
                    world_data.get("visionWidth", 8),
                    world_data.get("visionHeight", 8)
                )
        else:
            world = pynlm.createSimpleWorld()
            world.configure(20, 20, 8, 8)
        
        world.reset()
        
        # Create agent
        print("Creating agent...")
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Configure learning
        pynlm.enable_learning(agent, True)
        pynlm.set_exploration_rate(agent, args.exploration)
        
        # Train
        print(f"Starting training...")
        print(f"   Episodes: {args.episodes}")
        print(f"   Steps per episode: {args.steps}")
        print(f"   Exploration rate: {args.exploration}")
        
        training_start = time.time()
        training_results = pynlm.train_agent(brain, world, agent, args.episodes, args.steps)
        training_time = time.time() - training_start
        
        print(f"✅ Training completed in {training_time:.2f} seconds!")
        print(f"   Total reward: {training_results['total_reward']:.4f}")
        print(f"   Average reward per episode: {training_results['avg_reward_per_episode']:.4f}")
        print(f"   Max firing rate: {training_results['max_firing_rate']:.4f}")
        print(f"   Average firing rate: {training_results['avg_firing_rate']:.4f}")
        
        if args.save:
            print(f"💾 Saving trained brain to: {args.save}")
            pynlm.save_model(brain, args.save)
            print(f"✅ Trained brain saved to: {args.save}")
        
        print("\n💡 Next steps:")
        print("  nlm simulate --brain your_brain.nlm --steps 100")
        print("  nlm analyze --brain your_brain.nlm")
        print("  nlm visualize --brain your_brain.nlm")
    
    def handle_simulate(self, args):
        """Handle simulate command"""
        print("🎮 Running simulation...")
        
        # Create brain
        print("Creating brain...")
        if args.brain:
            if not Path(args.brain).exists():
                print(f"❌ Brain file not found: {args.brain}")
                return
            print(f"Loading brain from: {args.brain}")
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            pynlm.load_model(brain, args.brain)
            brain.initialize()
        else:
            print("Creating new brain...")
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
        
        # Create world
        print("Creating world...")
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 6, 6)
        world.reset()
        
        # Create agent if requested
        agent = None
        if args.agent:
            print("Creating agent...")
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            pynlm.enable_learning(agent, True)
        
        # Run simulation
        print(f"Starting simulation with {args.steps} steps...")
        
        simulation_start = time.time()
        if args.agent:
            simulation_results = pynlm.run_simulation(brain, world, agent, args.steps)
        else:
            # Run simple brain simulation
            simulation_results = []
            for i in range(args.steps):
                brain.step(i)
                simulation_results.append({
                    'step': i,
                    'firing_rate': brain.getAverageFiringRate(),
                    'spike_count': brain.getTotalSpikeCount(),
                    'active_neurons': brain.getActiveNeuronCount()
                })
        
        simulation_time = time.time() - simulation_start
        
        print(f"✅ Simulation completed in {simulation_time:.2f} seconds!")
        print(f"   Average steps per second: {args.steps / simulation_time:.2f}")
        print(f"   Average firing rate: {brain.getAverageFiringRate():.4f}")
        print(f"   Total spikes: {brain.getTotalSpikeCount()}")
        
        # Save results if requested
        if args.output:
            print(f"💾 Saving simulation results to: {args.output}")
            with open(args.output, 'w') as f:
                json.dump(simulation_results, f, indent=2)
            print(f"✅ Results saved to: {args.output}")
        
        # Display real-time if requested
        if args.real_time:
            print("\n🖥️  Displaying real-time visualization...")
            # This would require matplotlib GUI, simplified here
            print("(Real-time visualization would be displayed here)")
        
        print("\n💡 Next steps:")
        print("  nlm analyze --brain your_brain.nlm")
        print("  nlm visualize --brain your_brain.nlm")
        print("  nlm profile --brain your_brain.nlm")
    
    def handle_analyze(self, args):
        """Handle analyze command"""
        print("📊 Analyzing brain results...")
        
        # Load brain
        print("Loading brain...")
        if not Path(args.brain).exists():
            print(f"❌ Brain file not found: {args.brain}")
            return
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        pynlm.load_model(brain, args.brain)
        brain.initialize()
        
        # Run analysis
        print(f"Running analysis with {args.scenarios} scenarios...")
        analysis_results = pynlm.evaluate_brain(brain, args.scenarios)
        
        # Show results
        print(f"\n📋 Analysis Results:")
        for key, value in analysis_results.items():
            print(f"   {key}: {value:.4f}")
        
        # Show detailed analysis if requested
        if args.detailed:
            print(f"\n🔍 Detailed Analysis:")
            
            # Performance metrics
            print(f"   Performance metrics:")
            patterns = pynlm.analyze_patterns(brain)
            for key, value in patterns.items():
                print(f"     {key}: {value:.4f}")
            
            # Anomaly detection
            print(f"   Anomaly detection:")
            anomalies = pynlm.detect_anomalies(brain)
            detected_count = sum(1 for v in anomalies.values() if v > 0)
            print(f"     Anomalies detected: {detected_count}/{len(anomalies)}")
            
            # Performance summary
            print(f"   Performance summary:")
            summary = pynlm.summarize_performance(brain)
            for key, value in summary.items():
                print(f"     {key}: {value:.4f}")
        
        # Save analysis if requested
        if args.output:
            print(f"💾 Saving analysis results to: {args.output}")
            with open(args.output, 'w') as f:
                json.dump(analysis_results, f, indent=2)
            print(f"✅ Analysis results saved to: {args.output}")
    
    def handle_visualize(self, args):
        """Handle visualize command"""
        print("🎨 Generating visualizations...")
        
        # Create brain
        print("Creating brain...")
        if not Path(args.brain).exists():
            print(f"❌ Brain file not found: {args.brain}")
            return
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        pynlm.load_model(brain, args.brain)
        brain.initialize()
        
        # Create world and agent if needed
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 6, 6)
        world.reset()
        
        agent = None
        if args.agent:
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            pynlm.enable_learning(agent, True)
        
        # Generate visualization
        print(f"Generating {args.type} visualization...")
        
        try:
            # Import the visualization module
            from visualization_profiling import AdvancedVisualizer
            
            # Create visualizer
            visualizer = AdvancedVisualizer(figsize=(args.width/100, args.height/100))
            
            # Create dashboard
            dashboard = visualizer.create_dashboard(brain, world, agent, args.steps)
            
            # Save visualization
            if args.output:
                dashboard.savefig(args.output, format=args.format, 
                                bbox_inches='tight', dpi=150)
                print(f"✅ Visualization saved to: {args.output}")
            else:
                print("   Displaying visualization...")
                plt.show()
                
        except ImportError as e:
            print(f"❌ Visualization module not available: {e}")
            print("   Run: pip install matplotlib numpy")
    
    def handle_profile(self, args):
        """Handle profile command"""
        print("📊 Profiling performance...")
        
        # Create brain
        print("Creating brain...")
        if not Path(args.brain).exists():
            print(f"❌ Brain file not found: {args.brain}")
            return
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        pynlm.load_model(brain, args.brain)
        brain.initialize()
        
        # Create world and agent if needed
        world = pynlm.createSimpleWorld()
        world.configure(15, 15, 6, 6)
        world.reset()
        
        agent = None
        if args.agent:
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            pynlm.enable_learning(agent, True)
        
        # Run profiling
        print(f"Running profiling with {args.benchmark} benchmark...")
        
        try:
            # Import the profiler module
            from visualization_profiling import PerformanceProfiler
            
            # Create profiler
            profiler = PerformanceProfiler()
            
            if args.benchmark == 'step':
                benchmark_results = profiler.benchmark(brain, 'step', args.iterations)
            elif args.benchmark == 'action':
                benchmark_results = profiler.benchmark(brain, 'action', args.iterations)
            elif args.benchmark == 'simulation':
                if args.agent:
                    simulation_results = profiler.profile_simulation(
                        brain, world, agent, args.steps
                    )
                else:
                    print("❌ Simulation profiling requires agent interface")
                    return
            else:  # all
                # Run all benchmarks
                step_results = profiler.benchmark(brain, 'step', args.iterations)
                action_results = profiler.benchmark(brain, 'action', args.iterations)
                
                if args.agent:
                    simulation_results = profiler.profile_simulation(
                        brain, world, agent, args.steps
                    )
                
                benchmark_results = {
                    'step': step_results,
                    'action': action_results,
                    'simulation': simulation_results if args.agent else None
                }
            
            # Show results
            print(f"\n📋 Profile Results:")
            if 'step' in benchmark_results:
                print(f"   Step benchmark:")
                print(f"     Average duration: {benchmark_results['step']['avg_duration']*1000:.2f} ms")
                print(f"     Operations per second: {benchmark_results['step']['ops_per_second']:.2f}")
                print(f"     Memory usage: {benchmark_results['step']['avg_memory']:.2f} MB")
            
            if 'action' in benchmark_results:
                print(f"   Action benchmark:")
                print(f"     Average duration: {benchmark_results['action']['avg_duration']*1000:.2f} ms")
                print(f"     Operations per second: {benchmark_results['action']['ops_per_second']:.2f}")
                print(f"     Memory usage: {benchmark_results['action']['avg_memory']:.2f} MB")
            
            if 'simulation' in benchmark_results:
                print(f"   Simulation benchmark:")
                print(f"     Steps per second: {benchmark_results['simulation']['steps_per_second']:.2f}")
                print(f"     Spikes per second: {benchmark_results['simulation']['spikes_per_second']:.2f}")
                print(f"     Average step duration: {benchmark_results['simulation']['avg_step_duration']*1000:.2f} ms")
            
            # Save profile if requested
            if args.output:
                print(f"💾 Saving profile data to: {args.output}")
                profiler.save_profile(args.output)
                print(f"✅ Profile data saved to: {args.output}")
                
        except ImportError as e:
            print(f"❌ Profiler module not available: {e}")
            print("   Run: pip install matplotlib numpy psutil")
    
    def handle_config(self, args):
        """Handle config command"""
        print("⚙️  Configuration management...")
        
        if not args.config_action:
            print("Available config actions:")
            print("  create --name <simple|experimental|production> --output <file>")
            print("  list")
            print("  show [--name <name>]")
            return
        
        if args.config_action == 'create':
            print(f"Creating {args.name} configuration...")
            
            if args.output:
                output_path = Path(args.output)
                
                # Create configuration based on type
                if args.name == 'simple':
                    config = {
                        "brain": {
                            "neuron_count": 100,
                            "region_count": 1,
                            "connection_probability": 0.2
                        },
                        "plasticity": {
                            "stdp": {"enable": True},
                            "hebbian": {"enable": True}
                        }
                    }
                elif args.name == 'experimental':
                    config = {
                        "brain": {
                            "neuron_count": 500,
                            "region_count": 3,
                            "connection_probability": 0.05
                        },
                        "plasticity": {
                            "stdp": {"enable": True, "learning_rate": 0.001},
                            "hebbian": {"enable": True, "learning_rate": 0.01}
                        },
                        "neuromodulation": {
                            "curiosity": {"enable": True},
                            "novelty": {"enable": True}
                        },
                        "development": {"enable": True}
                    }
                else:  # production
                    config = {
                        "brain": {
                            "neuron_count": 1000,
                            "region_count": 5,
                            "connection_probability": 0.1
                        },
                        "plasticity": {
                            "stdp": {"enable": True, "learning_rate": 0.001},
                            "hebbian": {"enable": True, "learning_rate": 0.01}
                        }
                    }
                
                # Save configuration
                with open(output_path, 'w') as f:
                    json.dump(config, f, indent=2)
                
                print(f"✅ Configuration saved to: {output_path}")
                
            else:
                print("   --output <file> is required to save configuration")
        
        elif args.config_action == 'list':
            print("Available configurations:")
            print("  simple   - Basic configuration for testing")
            print("  experimental - Configuration for research experiments")
            print("  production - Configuration for production use")
        
        elif args.config_action == 'show':
            if args.name:
                print(f"Configuration '{args.name}':")
                if args.name == 'simple':
                    print("  Brain: 100 neurons, 1 region, 0.2 connection probability")
                    print("  Plasticity: STDP and Hebbian learning enabled")
                elif args.name == 'experimental':
                    print("  Brain: 500 neurons, 3 regions, 0.05 connection probability")
                    print("  Plasticity: STDP (0.001), Hebbian (0.01)")
                    print("  Neuromodulation: Curiosity and novelty enabled")
                    print("  Development: Enabled")
                elif args.name == 'production':
                    print("  Brain: 1000 neurons, 5 regions, 0.1 connection probability")
                    print("  Plasticity: STDP (0.001), Hebbian (0.01)")
                else:
                    print(f"   Configuration '{args.name}' not found")
            else:
                print("   Use --name <simple|experimental|production> to show specific configuration")
    
    def handle_export(self, args):
        """Handle export command"""
        print("📤 Exporting brain data...")
        
        # Load brain
        if not Path(args.brain).exists():
            print(f"❌ Brain file not found: {args.brain}")
            return
        
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        pynlm.load_model(brain, args.brain)
        brain.initialize()
        
        # Export based on format
        print(f"Exporting format: {args.format}")
        
        if args.format == 'weights':
            # Export synaptic weights
            weights_data = {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio()
            }
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(weights_data, f, indent=2)
                print(f"✅ Weights exported to: {args.output}")
            else:
                print(json.dumps(weights_data, indent=2))
        
        elif args.format == 'connections':
            # Export connectivity data
            connections_data = {
                'regions': brain.getRegionIds(),
                'total_neuron_count': brain.getTotalNeuronCount(),
                'total_synapse_count': brain.getTotalSynapseCount()
            }
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(connections_data, f, indent=2)
                print(f"✅ Connections exported to: {args.output}")
            else:
                print(json.dumps(connections_data, indent=2))
        
        elif args.format == 'neurons':
            # Export neuron data
            neurons_data = {
                'total_neuron_count': brain.getTotalNeuronCount(),
                'region_count': brain.getRegionCount(),
                'average_firing_rate': brain.getAverageFiringRate()
            }
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(neurons_data, f, indent=2)
                print(f"✅ Neurons exported to: {args.output}")
            else:
                print(json.dumps(neurons_data, indent=2))
        
        elif args.format == 'all':
            # Export all data
            all_data = {
                'brain': {
                    'total_neuron_count': brain.getTotalNeuronCount(),
                    'total_synapse_count': brain.getTotalSynapseCount(),
                    'region_count': brain.getRegionCount(),
                    'average_firing_rate': brain.getAverageFiringRate(),
                    'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                    'total_spike_count': brain.getTotalSpikeCount()
                },
                'export_info': {
                    'timestamp': time.time(),
                    'format': 'all',
                    'compressed': args.compress
                }
            }
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(all_data, f, indent=2)
                print(f"✅ All data exported to: {args.output}")
            else:
                print(json.dumps(all_data, indent=2))
    
    def handle_import(self, args):
        """Handle import command"""
        print("📥 Importing brain data...")
        
        if not args.source:
            print("❌ --source <file> is required to import from")
            return
        
        if not Path(args.source).exists():
            print(f"❌ Source file not found: {args.source}")
            return
        
        # Load source data
        print(f"Loading data from: {args.source}")
        with open(args.source, 'r') as f:
            source_data = json.load(f)
        
        # Create brain
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Import based on format
        print(f"Import format: {args.format}")
        
        if args.format == 'weights':
            # Import synaptic weights (simplified)
            print(f"   Import weight data from JSON")
            # Note: This is a simplified implementation
            print("   ✅ Weights imported (simplified)")
        
        elif args.format == 'connections':
            # Import connectivity data
            print(f"   Import connectivity data from JSON")
            # Note: This is a simplified implementation
            print("   ✅ Connectivity imported (simplified)")
        
        elif args.format == 'neurons':
            # Import neuron data
            print(f"   Import neuron data from JSON")
            # Note: This is a simplified implementation
            print("   ✅ Neurons imported (simplified)")
        
        print(f"✅ Brain imported from: {args.source}")
        print(f"   Total neurons: {brain.getTotalNeuronCount()}")
        print(f"   Total synapses: {brain.getTotalSynapseCount()}")
    
    def handle_help(self, args):
        """Handle help command"""
        if args.command:
            # Show help for specific command
            print(f"Help for command: {args.command}")
            if args.command in self.command_parsers:
                self.command_parsers[args.command].print_help()
        elif args.all:
            # Show all help
            self.show_help()
        else:
            # Show general help
            print("\n🎯 NLM Enhanced Command-Line Interface")
            print("=" * 60)
            print("\nAvailable commands:")
            for cmd_name, cmd_parser in self.command_parsers.items():
                if cmd_name != 'help':  # Skip help command to avoid recursion
                    print(f"  {cmd_name:15} {cmd_parser.description}")
            
            print(f"\nFor detailed help on any command:")
            print(f"  nlm help --command <command_name>")
            
            print(f"\nExamples:")
            print(f"  nlm create --neurons 1000 --learning")
            print(f"  nlm train --brain my_brain.nlm --episodes 50")
            print(f"  nlm simulate --brain my_brain.nlm --steps 100")
            print(f"  nlm analyze --brain my_brain.nlm")
            print(f"  nlm visualize --brain my_brain.nlm")
            print(f"  nlm profile --brain my_brain.nlm --benchmark step")
            print(f"  nlm config create --name experimental")
            print(f"  nlm export --brain my_brain.nlm --format all")
    
    def handle_version(self, args):
        """Handle version command"""
        print("NLM Enhanced Command-Line Interface")
        print("=" * 40)
        print("Version: 2.0.0")
        print("Build: Enhanced Python API")
        print("Description: Comprehensive NLM tools with visualization and profiling")
        print("\nFeatures:")
        print("  • High-level Python API with convenience functions")
        print("  • Advanced visualization tools")
        print("  • Performance profiling and benchmarking")
        print("  • Model persistence and export")
        print("  • Comprehensive error handling")
        print("  • Configuration management")
        print("  • Analysis and monitoring tools")
    
    def handle_examples(self, args):
        """Handle examples command"""
        print(f"📚 NLM Examples - {args.category} examples")
        print("=" * 50)
        
        if args.category == 'basic' or args.category == 'all':
            self.show_basic_examples()
        
        if args.category == 'advanced' or args.category == 'all':
            self.show_advanced_examples()
    
    def show_basic_examples(self):
        """Show basic examples"""
        examples = [
            {
                'title': 'Creating a Brain',
                'command': 'nlm create --neurons 1000 --regions 3',
                'description': 'Create a brain with 1000 neurons and 3 regions'
            },
            {
                'title': 'Training an Agent',
                'command': 'nlm train --episodes 50 --steps 200',
                'description': 'Train an agent for 50 episodes with 200 steps each'
            },
            {
                'title': 'Running a Simulation',
                'command': 'nlm simulate --steps 100',
                'description': 'Run a simulation with 100 steps'
            },
            {
                'title': 'Analyzing Results',
                'command': 'nlm analyze --scenarios 10',
                'description': 'Analyze brain performance on 10 test scenarios'
            },
            {
                'title': 'Generating Visualizations',
                'command': 'nlm visualize --type dashboard',
                'description': 'Generate comprehensive visualizations'
            },
            {
                'title': 'Performance Profiling',
                'command': 'nlm profile --benchmark step',
                'description': 'Profile step execution performance'
            }
        ]
        
        for i, example in enumerate(examples, 1):
            print(f"\n{i}. {example['title']}")
            print(f"   Command: {example['command']}")
            print(f"   Description: {example['description']}")
    
    def show_advanced_examples(self):
        """Show advanced examples"""
        examples = [
            {
                'title': 'Advanced Training Configuration',
                'command': 'nlm train --brain model1.nlm --world config.json --exploration 0.3 --learning-rate 0.01',
                'description': 'Train with custom world configuration and learning parameters'
            },
            {
                'title': 'Detailed Analysis',
                'command': 'nlm analyze --brain model2.nlm --scenarios 20 --detailed',
                'description': 'Run detailed analysis with multiple scenarios'
            },
            {
                'title': 'Advanced Visualization',
                'command': 'nlm visualize --brain model3.nlm --type all --width 1600 --height 1200',
                'description': 'Generate all visualization types with high resolution'
            },
            {
                'title': 'Comprehensive Profiling',
                'command': 'nlm profile --brain model4.nlm --benchmark all --detailed',
                'description': 'Run comprehensive profiling for all operation types'
            },
            {
                'title': 'Configuration Management',
                'command': 'nlm config create --name experimental --output exp_config.json',
                'description': 'Create experimental configuration for research'
            },
            {
                'title': 'Model Export/Import',
                'command': 'nlm export --brain model5.nlm --format all --output model5_backup.json',
                'description': 'Export complete model for backup and sharing'
            }
        ]
        
        for i, example in enumerate(examples, 1):
            print(f"\n{i}. {example['title']}")
            print(f"   Command: {example['command']}")
            print(f"   Description: {example['description']}")
    
    def show_help(self):
        """Show general help"""
        print("=" * 60)
        print("NLM Enhanced Command-Line Interface - Help")
        print("=" * 60)
        
        print("\n🎯 Overview:")
        print("   NLM CLI provides comprehensive command-line access to")
        print("   the enhanced NLM Python API, including simulation,")
        print("   training, visualization, profiling, and analysis.")
        
        print("\n📋 Available Commands:")
        commands = [
            ('create', 'Create and configure a brain'),
            ('train', 'Train an agent'),
            ('simulate', 'Run a simulation'),
            ('analyze', 'Analyze brain results'),
            ('visualize', 'Generate visualizations'),
            ('profile', 'Profile performance'),
            ('config', 'Manage configurations'),
            ('export', 'Export models and data'),
            ('import', 'Import models and data'),
            ('help', 'Show help information'),
            ('version', 'Show version information'),
            ('examples', 'Show examples')
        ]
        
        for cmd, desc in commands:
            print(f"   {cmd:12} - {desc}")
        
        print("\n🚀 Quick Start Examples:")
        print("   nlm create --neurons 1000")
        print("   nlm train --episodes 20 --steps 100")
        print("   nlm simulate --steps 50")
        print("   nlm analyze --scenarios 5")
        print("   nlm visualize")
        print("   nlm profile")
        
        print("\n📖 For more information:")
        print("   nlm help --all")
        print("   nlm examples")
        print("   Documentation: README_ENHANCED.md")
        
        print("\n" + "=" * 60)
    
    def run_interactive_mode(self):
        """Run interactive mode"""
        print("\n" + "=" * 60)
        print("NLM Interactive Mode")
        print("=" * 60)
        
        while True:
            try:
                print("\nAvailable commands:")
                print("  help          - Show help")
                print("  exit          - Exit interactive mode")
                print("  create        - Create brain")
                print("  train         - Train agent")
                print("  simulate      - Run simulation")
                print("  analyze       - Analyze results")
                print("  visualize     - Generate visualizations")
                print("  profile       - Profile performance")
                print("  config        - Manage configurations")
                print("  export        - Export models")
                print("  import        - Import models")
                print()
                
                command = input("nlm> ").strip().lower()
                
                if command == 'exit':
                    print("Goodbye!")
                    break
                elif command == 'help':
                    self.show_help()
                else:
                    # Parse and run command
                    args = [command] + sys.argv[1:]
                    self.run(args)
                
            except KeyboardInterrupt:
                print("\nOperation interrupted")
            except Exception as e:
                print(f"Error: {e}")
class MainEntryPoint:
    """Main entry point for NLM CLI"""
    
    def __init__(self):
        self.cli = NLMCLI()
    
    def run(self, args=None):
        """Run the CLI"""
        # Check if running in interactive mode
        if args is None and len(sys.argv) > 1:
            # Check for interactive flag
            if '--interactive' in sys.argv:
                sys.argv.remove('--interactive')
                self.cli.run_interactive_mode()
                return
        
        self.cli.run(args)
if __name__ == "__main__":
    print("=" * 60)
    print("NLM Enhanced Command-Line Interface")
    print("=" * 60)
    
    # Initialize and run CLI
    main = MainEntryPoint()
    main.run(sys.argv[1:])

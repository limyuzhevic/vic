import argparse
import sys
import os
from pathlib import Path

# Add nlm package to path
sys.path.insert(0, str(Path(__file__).parent))

from nlm.core import Brain, Config, SimpleWorld
from nlm.wrapper import NLMSimulation, NLMExperiment, NLMPlotting, NLMDeveloper
from nlm.types import NeuronType, ActionType, DevelopmentalStage
from nlm.factory import create_default_config
class NLMCLI:
    """Command-line interface for NLM"""
    
    def __init__(self):
        self.simulation = None
    
    def run_demo(self, args):
        """Run demo simulation"""
        print("NLM Demo Simulation")
        print("=" * 50)
        
        # Create simulation
        self.simulation = NLMSimulation().quick_start(
            neurons=500,
            width=400,
            height=400,
            timestep=0.001,
            seed=42
        )
        
        # Run simulation
        results = self.simulation.run_simulation(
            total_steps=1000,
            steps_per_episode=200,
            verbose=True
        )
        
        print(f"\nDemo completed!")
        print(f"Final spike count: {results['final_spike_count']}")
        print(f"Final firing rate: {results['final_firing_rate']:.2f} Hz")
        
        return results
    
    def run_agent(self, args):
        """Run agent simulation"""
        print("NLM Agent Simulation")
        print("=" * 50)
        
        # Create simulation
        self.simulation = NLMSimulation().quick_start(
            neurons=1000,
            width=800,
            height=600,
            timestep=0.001,
            seed=args.seed
        )
        
        # Create world with objects
        self.simulation.world.configure(args.width, args.height, 40, 40)
        
        # Add some objects to world
        self.simulation.world.add_resource_object(100, 100, 1.0)
        self.simulation.world.add_hazard_object(700, 500, -1.0)
        self.simulation.world.add_wall_object(400, 300)
        
        # Place agent
        self.simulation.world.setAgentStart(args.start_x, args.start_y)
        
        # Run simulation
        results = self.simulation.run_simulation(
            total_steps=args.steps,
            steps_per_episode=args.episode_length,
            verbose=args.verbose
        )
        
        print(f"\nAgent simulation completed!")
        print(f"Final position: ({self.simulation.world.agent.x:.1f}, {self.simulation.world.agent.y:.1f})")
        print(f"Final spike count: {results['final_spike_count']}")
        
        return results
    
    def run_learning(self, args):
        """Run learning experiment"""
        print("NLM Learning Experiment")
        print("=" * 50)
        
        # Create simulation
        self.simulation = NLMSimulation().quick_start(
            neurons=1000,
            width=800,
            height=600,
            timestep=0.001,
            seed=args.seed
        )
        
        # Run learning experiment
        results = self.simulation.run_learning_experiment(
            experiences=args.experiences,
            steps_per_experience=args.steps_per_experience,
            sensory_patterns=args.vary_patterns
        )
        
        print(f"\nLearning experiment completed!")
        print(f"Learning detected: {results['learning_detected']}")
        print(f"Total spikes: {results['total_spikes']}")
        print(f"Average final spikes: {results['average_final_spikes']:.1f}")
        
        if results['learning_detected']:
            print("✓ Learning was successful!")
        else:
            print("⚠ Limited learning detected - try different parameters")
        
        return results
    
    def run_brain_test(self, args):
        """Run brain functionality test"""
        print("NLM Brain Functionality Test")
        print("=" * 50)
        
        # Create brain
        brain = Brain()
        brain.initialize()
        
        print("Initial brain status:")
        brain.logStatus()
        
        # Run basic tests
        print("\nRunning connectivity test...")
        brain.inject_sensory_current(NeuronType.Sensory, 50.0, 100)
        
        print("Running plasticity test...")
        results = brain.simulate_plasticity_experiment(500)
        
        print(f"\nPlasticity results:")
        print(f"  Weight change: {results['weight_change']:.4f}")
        print(f"  Learning percent: {results['learning_percent']:.2f}%")
        print(f"  Total spikes: {results['total_spikes']}")
        
        # Test developmental stages
        print("\nTesting developmental stages...")
        stages = [DevelopmentalStage.Initial, DevelopmentalStage.CriticalPeriod, 
                 DevelopmentalStage.Maturation, DevelopmentalStage.Adult]
        
        for stage in stages:
            brain.setDevelopmentalStage(stage)
            print(f"  Stage {stage.name}: {brain.getTotalSpikeCount()} spikes")
        
        print("\n✓ Brain functionality test completed!")
        return brain
    
    def run_config_test(self, args):
        """Test configuration system"""
        print("NLM Configuration Test")
        print("=" * 50)
        
        # Create configuration
        config = Config()
        
        print("1. Testing basic configuration...")
        config.set("random_seed", 42, None)
        config.set("simulation_timestep", 0.001, None)
        config.set("neuron_count", 500, None)
        config.set("region_count", 1, None)
        
        print(f"   Config loaded successfully")
        print(f"   Keys: {config.getKeys()}")
        
        print("\n2. Testing configuration validation...")
        errors = config.validate()
        if errors:
            print(f"   Validation errors: {errors}")
        else:
            print(f"   Configuration is valid")
        
        print("\n3. Testing configuration summary...")
        summary = config.summary_dict()
        for key, value in summary.items():
            print(f"   {key}: {value}")
        
        print("\n4. Testing save/load...")
        config.save_to_json_file("test_config.json")
        print("   Configuration saved")
        
        new_config = Config()
        new_config.load_from_json_file("test_config.json")
        print("   Configuration loaded")
        
        print("\n✓ Configuration test completed!")
        
        # Clean up
        import os
        if os.path.exists("test_config.json"):
            os.remove("test_config.json")
        
        return config
    
    def run_experiment(self, args):
        """Run experiment"""
        print("NLM Experiment")
        print("=" * 50)
        
        # Create experiment
        experiment = NLMExperiment("CLI Experiment")
        
        # Add parameter sets
        param_sets = [
            {"random_seed": 42, "neuron_count": 500},
            {"random_seed": 43, "neuron_count": 1000},
            {"random_seed": 44, "neuron_count": 200},
        ]
        
        print("Running parameter sweep...")
        results = experiment.run_parameter_sweep(param_sets, args.steps)
        
        print(f"\nExperiment completed with {len(results)} parameter sets")
        
        for i, result in enumerate(results):
            print(f"  Set {i + 1}: {result['standard_results']['final_spike_count']} spikes")
        
        print("\n✓ Experiment completed!")
        return results
    
    def run_plotting(self, args):
        """Run plotting demonstration"""
        print("NLM Plotting Demo")
        print("=" * 50)
        
        try:
            plotter = NLMPlotting()
            print("Plotting functionality available")
            print("(Requires matplotlib to be installed)")
            
            # Create sample data
            steps = 100
            spike_data = [i * 0.1 % 10 for i in range(steps)]
            firing_rates = [5.0 * math.sin(i * 0.1) for i in range(steps)]
            
            print(f"Generated sample data for plotting:")
            print(f"  Spike data: {steps} points")
            print(f"  Firing rates: {steps} points")
            
            return {"status": "plotting_available", "data_points": steps}
            
        except Exception as e:
            print(f"Plotting not available: {e}")
            return {"status": "unavailable", "error": str(e)}
    
    def run_developer_tools(self, args):
        """Run developer tools"""
        print("NLM Developer Tools")
        print("=" * 50)
        
        developer = NLMDeveloper()
        
        print("Available developer tools:")
        print("  1. Configuration templates")
        print("  2. Experiment templates")
        print("  3. Configuration validation")
        print("  4. Benchmarking")
        
        # Create some templates
        developer.create_config_template("small", {
            "random_seed": 42,
            "neuron_count": 100,
            "simulation_timestep": 0.001,
        })
        
        developer.create_config_template("medium", {
            "random_seed": 42,
            "neuron_count": 1000,
            "simulation_timestep": 0.001,
        })
        
        developer.create_config_template("large", {
            "random_seed": 42,
            "neuron_count": 10000,
            "simulation_timestep": 0.001,
        })
        
        print(f"\nCreated {len(developer.config_templates)} configuration templates")
        
        # Test configuration validation
        test_config = Config()
        test_config.set("random_seed", 42, None)
        test_config.set("simulation_timestep", 0.001, None)
        
        errors = developer.validate_config(test_config)
        if not errors:
            print("Configuration validation passed")
        
        print("\n✓ Developer tools completed!")
        return {"templates_created": len(developer.config_templates)}
    
    def run_help(self, args):
        """Show help information"""
        help_text = """
NLM - Neural Learning Machine Command Line Interface

USAGE:
    python -m nlm <command> [options]

COMMANDS:
    demo                    Run demo simulation
    agent                    Run agent simulation  
    learning                 Run learning experiment
    brain-test               Test brain functionality
    config-test              Test configuration system
    experiment               Run experiment
    plotting                 Demonstrate plotting tools
    developer                Show developer tools
    help                    Show this help message

EXAMPLES:
    python -m nlm demo
    python -m nlm agent --steps 1000 --seed 42
    python -m nlm learning --experiences 5
    python -m nlm brain-test
    python -m nlm config-test

For more information about a specific command:
    python -m nlm help <command>
        """
        print(help_text)
    
    def run_command_help(self, args):
        """Show help for specific command"""
        command_help = {
            "demo": """
Demo Command:
    Runs a simple demonstration of NLM capabilities.
    
    This creates a basic simulation and runs it for 1000 steps
    to show the core neural computation in action.
            """,
            "agent": """
Agent Command:
    Runs an agent simulation in a 2D world.
    
    OPTIONS:
        --steps N        Total steps to simulate (default: 1000)
        --seed N         Random seed (default: 42)
        --width N        World width (default: 800)
        --height N       World height (default: 600)
        --start-x N      Agent start X position (default: 0)
        --start-y N      Agent start Y position (default: 0)
        --episode-length N Steps per episode (default: 200)
        --verbose        Enable verbose output
            """,
            "learning": """
Learning Command:
    Runs a learning experiment to test neural plasticity.
    
    OPTIONS:
        --experiences N    Number of learning experiences (default: 10)
        --steps-per-experience N Steps per experience (default: 200)
        --seed N           Random seed (default: 42)
        --vary-patterns    Use varied sensory patterns
            """,
            "brain-test": """
Brain-Test Command:
    Tests brain functionality including connectivity and plasticity.
    
    This runs several tests to verify that the brain system is
    working correctly, including spike generation and learning.
            """,
            "config-test": """
Config-Test Command:
    Tests the configuration system.
    
    This validates that configuration loading, validation,
    and save/load functionality work correctly.
            """,
            "experiment": """
Experiment Command:
    Runs a parameter sweep experiment.
    
    OPTIONS:
        --steps N       Steps per experiment (default: 500)
            """,
            "plotting": """
Plotting Command:
    Demonstrates plotting capabilities.
    
    This shows what plotting would look like if matplotlib
    is available. Currently just generates sample data.
            """,
            "developer": """
Developer Command:
    Shows developer tools and utilities.
    
    This includes configuration templates, experiment
    templates, and development utilities for research.
            """,
        }
        
        command = args.command if hasattr(args, 'command') else 'help'
        if command in command_help:
            print(command_help[command])
        else:
            print(f"No help available for command: {command}")
    
    def main(self, argv):
        """Main entry point"""
        parser = argparse.ArgumentParser(
            description="NLM - Neural Learning Machine Command Line Interface",
            formatter_class=argparse.RawDescriptionHelpFormatter
        )
        
        parser.add_argument(
            'command',
            nargs='?',
            default='help',
            help='Command to run'
        )
        
        parser.add_argument(
            '--verbose', '-v',
            action='store_true',
            help='Enable verbose output'
        )
        
        # Agent command arguments
        agent_parser = argparse.ArgumentParser(add_help=False)
        agent_parser.add_argument('--steps', type=int, default=1000,
                                 help='Total steps to simulate')
        agent_parser.add_argument('--seed', type=int, default=42,
                                 help='Random seed')
        agent_parser.add_argument('--width', type=int, default=800,
                                 help='World width')
        agent_parser.add_argument('--height', type=int, default=600,
                                 help='World height')
        agent_parser.add_argument('--start-x', type=int, default=0,
                                 help='Agent start X position')
        agent_parser.add_argument('--start-y', type=int, default=0,
                                 help='Agent start Y position')
        agent_parser.add_argument('--episode-length', type=int, default=200,
                                 help='Steps per episode')
        
        # Learning command arguments
        learning_parser = argparse.ArgumentParser(add_help=False)
        learning_parser.add_argument('--experiences', type=int, default=10,
                                    help='Number of learning experiences')
        learning_parser.add_argument('--steps-per-experience', type=int, default=200,
                                    help='Steps per experience')
        learning_parser.add_argument('--seed', type=int, default=42,
                                    help='Random seed')
        learning_parser.add_argument('--vary-patterns', action='store_true',
                                    help='Use varied sensory patterns')
        
        # Experiment command arguments
        experiment_parser = argparse.ArgumentParser(add_help=False)
        experiment_parser.add_argument('--steps', type=int, default=500,
                                      help='Steps per experiment')
        
        # Parse arguments based on command
        args = parser.parse_args(argv[1:])
        
        if args.command == 'agent':
            sub_args = agent_parser.parse_args(argv[2:])
            sub_args.verbose = args.verbose
            return self.run_agent(sub_args)
        
        elif args.command == 'learning':
            sub_args = learning_parser.parse_args(argv[2:])
            return self.run_learning(sub_args)
        
        elif args.command == 'experiment':
            sub_args = experiment_parser.parse_args(argv[2:])
            return self.run_experiment(sub_args)
        
        elif args.command in ['demo', 'brain-test', 'config-test', 'plotting', 'developer']:
            return getattr(self, f'run_{args.command}')(args)
        
        elif args.command == 'help':
            if len(argv) > 2 and argv[2] in ['demo', 'agent', 'learning', 'brain-test', 'config-test', 'experiment', 'plotting', 'developer']:
                help_args = argparse.Namespace(command=argv[2])
                return self.run_command_help(help_args)
            else:
                return self.run_help(args)
        
        else:
            print(f"Unknown command: {args.command}")
            return self.run_help(args)
def main():
    """Main entry point"""
    cli = NLMCLI()
    
    try:
        # Import math for plotting
        import math
        
        # Import ActionType for agent simulation
        from nlm.types import ActionType
        
        return cli.main(sys.argv)
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 1
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return 1
if __name__ == "__main__":
    sys.exit(main())

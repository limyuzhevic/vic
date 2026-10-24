#!/usr/bin/env python3
"""
NLM Demo Launcher Script

This script provides an easy way to run different NLM demonstrations and experiments.
It simplifies the process of running simulations, experiments, and benchmarks.
"""

import subprocess
import sys
import os
import argparse
import time
from pathlib import Path

class NLMDemoLauncher:
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.binary_path = self.project_root / "build" / "bin"

    def run_command(self, command, description=None):
        """Run a command and display output."""
        if description:
            print(f"\n{'='*60}")
            print(f"Running: {description}")
            print(f"{'='*60}")
        
        print(f"Command: {command}")
        
        try:
            # Change to project root
            os.chdir(self.project_root)
            
            # Run the command
            result = subprocess.run(
                command,
                shell=True,
                capture_output=True,
                text=True
            )
            
            if result.returncode == 0:
                print("SUCCESS!")
                if result.stdout:
                    print(result.stdout)
                return True
            else:
                print(f"FAILED with exit code {result.returncode}")
                if result.stderr:
                    print(f"Error: {result.stderr}")
                if result.stdout:
                    print(f"Output: {result.stdout}")
                return False
                
        except Exception as e:
            print(f"Exception occurred: {e}")
            return False

    def run_demo(self, args):
        """Run the basic demo."""
        neuron_count = args.neuron_count
        steps = args.steps
        
        # Check if build directory exists
        if not self.binary_path.exists():
            print(f"ERROR: Build directory not found at {self.binary_path}")
            print("Please build the project first:")
            print("  mkdir build && cd build && cmake .. && make -j4")
            return False
        
        command = f"{self.binary_path / 'nlm'} --neuron_count={neuron_count} --steps={steps}"
        return self.run_command(command, f"NLM Basic Demo ({neuron_count} neurons, {steps} steps)")

    def run_phase3_demo(self, args):
        """Run the Phase 3 demo."""
        # Check if build directory exists
        if not self.binary_path.exists():
            print(f"ERROR: Build directory not found at {self.binary_path}")
            return False
        
        command = f"{self.binary_path / 'nlm_phase3_demo'}"
        return self.run_command(command, "NLM Phase 3 Demo (World interaction)")

    def run_phase4_demo(self, args):
        """Run the Phase 4 demo."""
        # Check if build directory exists
        if not self.binary_path.exists():
            print(f"ERROR: Build directory not found at {self.binary_path}")
            return False
        
        command = f"{self.binary_path / 'nlm_phase4_demo'}"
        return self.run_command(command, "NLM Phase 4 Demo (Prediction and memory)")

    def run_phase6_demo(self, args):
        """Run the Phase 6 integration demo."""
        # Check if build directory exists
        if not self.binary_path.exists():
            print(f"ERROR: Build directory not found at {self.binary_path}")
            return False
        
        command = f"{self.binary_path / 'nlm_phase6_demo'}"
        return self.run_command(command, "NLM Phase 6 Demo (Full integration)")

    def run_cli_demo(self, args):
        """Run the CLI demo using the new nlm_cli tool."""
        mode = args.cli_mode
        neuron_count = args.neuron_count
        steps = args.steps
        
        # Check if build directory exists
        if not self.binary_path.exists():
            print(f"ERROR: Build directory not found at {self.binary_path}")
            return False
        
        if mode == "basic":
            command = f"{self.binary_path / 'nlm_cli'} demo --neuron_count={neuron_count} --steps={steps}"
        elif mode == "experiment":
            command = f"{self.binary_path / 'nlm_cli'} experiment --steps={steps}"
        elif mode == "benchmark":
            command = f"{self.binary_path / 'nlm_cli'} benchmark --steps={steps}"
        else:
            print(f"Unknown CLI mode: {mode}")
            return False
        
        return self.run_command(command, f"NLM CLI Demo ({mode} mode)")

    def list_demos(self):
        """List available demos."""
        demos = [
            ("basic", "Basic neural computation demo", "Run simple spiking neural network simulation"),
            ("phase3", "Phase 3 demo (World interaction)", "Demonstrate brain-world interaction"),
            ("phase4", "Phase 4 demo (Prediction and memory)", "Show prediction systems and memory"),
            ("phase6", "Phase 6 demo (Full integration)", "Comprehensive integration test"),
            ("cli", "CLI demo (New interface)", "Use the new command-line interface"),
        ]
        
        print("Available NLM Demos:")
        print("="*60)
        for i, (name, description, details) in enumerate(demos, 1):
            print(f"{i}. {name.upper()}: {description}")
            print(f"   {details}")
            print()

    def run_all_demos(self, args):
        """Run all demos sequentially."""
        print("Running all NLM demos sequentially...")
        print("This may take several minutes.")
        
        demos = [
            ("basic", lambda: self.run_demo(args)),
            ("phase3", lambda: self.run_phase3_demo(args)),
            ("phase4", lambda: self.run_phase4_demo(args)),
            ("phase6", lambda: self.run_phase6_demo(args)),
        ]
        
        results = []
        for name, demo_func in demos:
            print(f"\n{'='*60}")
            print(f"Running {name.upper()} demo...")
            print(f"{'='*60}")
            
            success = demo_func()
            results.append((name, success))
            
            if not success and args.stop_on_failure:
                print(f"Demo {name} failed. Stopping.")
                break
            
            if args.wait_between_demos:
                print(f"\nWaiting 10 seconds before next demo...")
                time.sleep(10)
        
        # Summary
        print(f"\n{'='*60}")
        print("DEMO SUMMARY")
        print(f"{'='*60}")
        
        passed = 0
        for name, success in results:
            status = "PASS" if success else "FAIL"
            print(f"{name:10} : {status}")
            if success:
                passed += 1
        
        print(f"\nPassed: {passed}/{len(results)}")
        
        return passed == len(results)

    def create_config_file(self, args):
        """Create a configuration file for NLM."""
        config_content = f"""# NLM Configuration
# Generated by NLM Demo Launcher

# Random seed for reproducible experiments
random_seed = {args.random_seed}

# Simulation timestep in seconds
simulation_timestep = {args.timestep}

# Number of neurons in the brain
neuron_count = {args.neuron_count}

# Number of brain regions
region_count = {args.region_count}

# Connection probability for random connectivity
connection_probability = {args.connection_probability}

# Plasticity settings
plasticity_learning_rate = {args.plasticity_rate}
stdp_ltp_weight = {args.stdp_ltp}
stdp_ltd_weight = {args.stdp_ltd}

# Development settings
development_synaptogenesis_rate = {args.dev_synaptogenesis}
development_pruning_rate = {args.dev_pruning}

# Neuromodulation settings
dopamine_baseline = {args.dopamine_baseline}

# Reward settings
reward_discount_factor = {args.reward_discount}

# Environment settings
environment_name = GridWorld
environment_width = {args.env_width}
environment_height = {args.env_height}

# Logging settings
log_level = {args.log_level}
log_to_file = {str(args.log_to_file).lower()}
log_filename = {args.log_filename}

# Simulation settings
max_simulation_steps = {args.max_steps}
simulation_time_limit = {args.time_limit}

# Visualization settings
visualization_enabled = {str(args.visualization).lower()}
visualization_update_rate = {args.viz_update_rate}
"""
        
        config_path = self.project_root / "configs" / f"{args.name}.cfg"
        
        with open(config_path, 'w') as f:
            f.write(config_content)
        
        print(f"Configuration saved to: {config_path}")
        return config_path

    def run(self):
        """Main launcher execution."""
        parser = argparse.ArgumentParser(
            description="NLM Demo Launcher - Easy way to run NLM demonstrations",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  nlm_launcher demo                    # Run basic demo
  nlm_launcher demo --neuron_count=1000  # Custom neuron count
  nlm_launcher list                    # List all demos
  nlm_launcher run-all                 # Run all demos sequentially
  nlm_launcher create-config myconfig   # Create a custom config file
            """
        )
        
        subparsers = parser.add_subparsers(dest='command', help='Available commands')
        
        # Demo commands
        demo_parser = subparsers.add_parser('demo', help='Run demo')
        demo_parser.add_argument('--neuron_count', type=int, default=500, help='Number of neurons')
        demo_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
        
        # Phase 3 demo
        subparsers.add_parser('phase3', help='Run Phase 3 demo (World interaction)')
        
        # Phase 4 demo
        subparsers.add_parser('phase4', help='Run Phase 4 demo (Prediction and memory)')
        
        # Phase 6 demo
        subparsers.add_parser('phase6', help='Run Phase 6 demo (Full integration)')
        
        # CLI demo
        cli_parser = subparsers.add_parser('cli', help='Run CLI demo (New interface)')
        cli_parser.add_argument('--mode', dest='cli_mode', default='basic', 
                               choices=['basic', 'experiment', 'benchmark'],
                               help='CLI mode to run')
        cli_parser.add_argument('--neuron_count', type=int, default=500, help='Number of neurons')
        cli_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
        
        # List demos
        subparsers.add_parser('list', help='List all available demos')
        
        # Run all demos
        all_parser = subparsers.add_parser('run-all', help='Run all demos sequentially')
        all_parser.add_argument('--stop-on-failure', action='store_true', help='Stop if a demo fails')
        all_parser.add_argument('--wait-between', action='store_true', help='Wait between demos')
        
        # Create config
        config_parser = subparsers.add_parser('create-config', help='Create a configuration file')
        config_parser.add_argument('name', help='Name of the config file (without extension)')
        config_parser.add_argument('--random_seed', type=int, default=42, help='Random seed')
        config_parser.add_argument('--timestep', type=float, default=0.001, help='Simulation timestep')
        config_parser.add_argument('--neuron_count', type=int, default=1000, help='Number of neurons')
        config_parser.add_argument('--region_count', type=int, default=2, help='Number of regions')
        config_parser.add_argument('--connection_probability', type=float, default=0.1, help='Connection probability')
        config_parser.add_argument('--plasticity_rate', type=float, default=0.01, help='Plasticity rate')
        config_parser.add_argument('--stdp_ltp', type=float, default=0.01, help='STDP LTP weight')
        config_parser.add_argument('--stdp_ltd', type=float, default=0.012, help='STDP LTD weight')
        config_parser.add_argument('--dev_synaptogenesis', type=float, default=0.001, help='Development synaptogenesis rate')
        config_parser.add_argument('--dev_pruning', type=float, default=0.0001, help='Development pruning rate')
        config_parser.add_argument('--dopamine_baseline', type=float, default=0.1, help='Dopamine baseline')
        config_parser.add_argument('--reward_discount', type=float, default=0.99, help='Reward discount factor')
        config_parser.add_argument('--env_width', type=int, default=10, help='Environment width')
        config_parser.add_argument('--env_height', type=int, default=10, help='Environment height')
        config_parser.add_argument('--log_level', default='INFO', help='Log level')
        config_parser.add_argument('--log_to_file', action='store_true', help='Log to file')
        config_parser.add_argument('--log_filename', default='nlm.log', help='Log filename')
        config_parser.add_argument('--max_steps', type=int, default=10000, help='Maximum steps')
        config_parser.add_argument('--time_limit', type=float, default=0.0, help='Time limit')
        config_parser.add_argument('--visualization', action='store_true', help='Enable visualization')
        config_parser.add_argument('--viz_update_rate', type=float, default=30.0, help='Visualization update rate')
        
        args = parser.parse_args()
        
        if args.command is None:
            parser.print_help()
            return
        
        # Handle commands
        if args.command == 'demo':
            self.run_demo(args)
        elif args.command == 'phase3':
            self.run_phase3_demo(args)
        elif args.command == 'phase4':
            self.run_phase4_demo(args)
        elif args.command == 'phase6':
            self.run_phase6_demo(args)
        elif args.command == 'cli':
            self.run_cli_demo(args)
        elif args.command == 'list':
            self.list_demos()
        elif args.command == 'run-all':
            self.run_all_demos(args)
        elif args.command == 'create-config':
            self.create_config_file(args)

if __name__ == "__main__":
    launcher = NLMDemoLauncher()
    launcher.run()


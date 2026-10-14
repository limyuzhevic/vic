#!/usr/bin/env python3

"""
NLM Command Line Interface (CLI) Tool
=====================================

This is an enhanced command-line interface for the NLM (Neural Learning Machine)
project, providing advanced functionality for running simulations, testing,
and managing neural network experiments.

Usage:
    nlm_cli [OPTIONS] COMMAND [ARGS...]

Options:
    --help          Show this message and exit
    --version       Show version information and exit

Commands:
    run             Run neural simulation
    test            Run tests
    config          Manage configuration
    benchmark       Run benchmarks
    experiment      Run experiments
    help            Show help for a specific command

For more information about a specific command, use:
    nlm_cli help COMMAND
"""

import argparse
import sys
import os
from pathlib import Path

# Add the current directory to the path
sys.path.insert(0, str(Path(__file__).parent))

class NLMCLI:
    def __init__(self):
        self.version = "2.0.0"
        self.project_root = Path(__file__).parent
        
    def print_banner(self):
        """Print the NLM banner"""
        print("=" * 70)
        print("NLM — 熙然 (Neural Learning Machine)")
        print("Phase 2: Real Neural Computation - Enhanced Version")
        print("=" * 70)
        print()
    
    def print_version(self):
        """Print version information"""
        self.print_banner()
        print(f"Version: {self.version}")
        print(f"Build: {self.get_build_info()}")
        print(f"C++ Standard: C++20")
        print()
        print("Features:")
        print("  ✓ Enhanced command-line interface with mode selection")
        print("  ✓ Improved configuration management")
        print("  ✓ Better error handling and logging")
        print("  ✓ Performance optimizations")
        print("  ✓ Extensible plugin architecture")
        print("  ✓ Comprehensive test suite")
        print("  ✓ Advanced debugging capabilities")
        print()
    
    def get_build_info(self):
        """Get build information"""
        import datetime
        return f"{datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
    
    def run_command(self, args):
        """Execute the run command"""
        parser = argparse.ArgumentParser(
            description="Run neural simulation",
            prog="nlm_cli run"
        )
        
        parser.add_argument(
            "--mode", default="all",
            choices=["basic", "plasticity", "stdp", "all"],
            help="Simulation mode (default: all)"
        )
        
        parser.add_argument(
            "--config", default="configs/default.cfg",
            help="Configuration file path (default: configs/default.cfg)"
        )
        
        parser.add_argument(
            "--neuron", type=int,
            help="Number of neurons (overrides config)"
        )
        
        parser.add_argument(
            "--seed", type=int,
            help="Random seed (overrides config)"
        )
        
        parser.add_argument(
            "--steps", type=int,
            help="Number of simulation steps (overrides config)"
        )
        
        parser.add_argument(
            "--verbose", action="store_true",
            help="Enable verbose output"
        )
        
        parser.add_argument(
            "--quiet", action="store_true",
            help="Suppress info messages (only errors)"
        )
        
        parser.add_argument(
            "--test-only", action="store_true",
            help="Run tests only (skip simulation)"
        )
        
        parsed = parser.parse_args(args)
        
        self.print_banner()
        print(f"Starting neural simulation...")
        print(f"Mode: {parsed.mode}")
        print(f"Config: {parsed.config}")
        
        if parsed.neuron:
            print(f"Neurons: {parsed.neuron} (custom)")
        
        if parsed.seed:
            print(f"Seed: {parsed.seed} (custom)")
        
        if parsed.steps:
            print(f"Steps: {parsed.steps} (custom)")
        
        if parsed.verbose:
            print("Logging: Verbose")
        elif parsed.quiet:
            print("Logging: Quiet")
        
        if parsed.test_only:
            print("Test mode: Enabled (simulation skipped)")
            return self.test_command([])
        
        print()
        print("Note: This is a CLI framework demonstration.")
        print("Actual simulation would be implemented using the C++ core.")
        print()
        print("To run the actual simulation, use:")
        print("  ./build/nlm --mode=<mode> [--config=<file>] [--neuron=<N>]")
        print()
        
        return 0
    
    def test_command(self, args):
        """Execute the test command"""
        parser = argparse.ArgumentParser(
            description="Run tests",
            prog="nlm_cli test"
        )
        
        parser.add_argument(
            "--unit", action="store_true",
            help="Run unit tests only"
        )
        
        parser.add_argument(
            "--integration", action="store_true",
            help="Run integration tests only"
        )
        
        parser.add_argument(
            "--performance", action="store_true",
            help="Run performance tests only"
        )
        
        parser.add_argument(
            "--all", action="store_true",
            help="Run all tests (default)"
        )
        
        parser.add_argument(
            "--config", default="configs/default.cfg",
            help="Configuration file path"
        )
        
        parser.add_argument(
            "--verbose", action="store_true",
            help="Enable verbose output"
        )
        
        parser.add_argument(
            "--quiet", action="store_true",
            help="Suppress non-essential output"
        )
        
        parsed = parser.parse_args(args)
        
        # Determine which tests to run
        run_unit = run_integration = run_performance = False
        
        if parsed.all or (not parsed.unit and not parsed.integration and not parsed.performance):
            run_unit = run_integration = run_performance = True
        else:
            run_unit = parsed.unit
            run_integration = parsed.integration
            run_performance = parsed.performance
        
        self.print_banner()
        print(f"Running tests...")
        print(f"Config: {parsed.config}")
        
        if run_unit:
            print("Unit tests: Enabled")
        if run_integration:
            print("Integration tests: Enabled")
        if run_performance:
            print("Performance tests: Enabled")
        
        print()
        print("Note: This is a CLI framework demonstration.")
        print("Actual tests would be implemented using the test framework.")
        print()
        print("To run the actual tests, use one of:")
        print("  ./build/nlm_test                    # Run all unit tests")
        print("  ./build/nlm_phase6_demo             # Run integration tests")
        print("  ./test_runner.py [--unit|--integration|--performance]")
        print()
        
        return 0
    
    def config_command(self, args):
        """Execute the config command"""
        parser = argparse.ArgumentParser(
            description="Manage configuration",
            prog="nlm_cli config"
        )
        
        subparsers = parser.add_subparsers(dest="command", help="Config command")
        
        # Show command
        show_parser = subparsers.add_parser("show", help="Show current configuration")
        show_parser.add_argument(
            "--file", default="configs/default.cfg",
            help="Configuration file path"
        )
        
        # Set command
        set_parser = subparsers.add_parser("set", help="Set configuration value")
        set_parser.add_argument("key")
        set_parser.add_argument("value")
        set_parser.add_argument("--file", default="configs/default.cfg",
                               help="Configuration file path")
        
        # Load command
        load_parser = subparsers.add_parser("load", help="Load configuration from file")
        load_parser.add_argument("file", help="Configuration file path")
        
        parsed = parser.parse_args(args)
        
        if not parsed.command:
            parser.print_help()
            return 1
        
        self.print_banner()
        
        if parsed.command == "show":
            print(f"Configuration file: {parsed.file}")
            print()
            print("Note: This is a CLI framework demonstration.")
            print("Actual configuration management would be implemented using the Config class.")
            print()
            print("To manage configuration, use:")
            print("  Config class methods:")
            print("    - loadFromFile()")
            print("    - set()")
            print("    - getOr()")
            print("    - loadFromArgs()")
        
        elif parsed.command == "set":
            print(f"Set configuration: {parsed.key} = {parsed.value}")
            print(f"File: {parsed.file}")
            print()
            print("Note: This would call Config::set(key, value, ConfigSource::CommandLine)")
        
        elif parsed.command == "load":
            print(f"Load configuration from: {parsed.file}")
            print()
            print("Note: This would call Config::loadFromFile(file)")
        
        return 0
    
    def benchmark_command(self, args):
        """Execute the benchmark command"""
        parser = argparse.ArgumentParser(
            description="Run benchmarks",
            prog="nlm_cli benchmark"
        )
        
        parser.add_argument(
            "--type", default="performance",
            choices=["performance", "memory", "cpu", "io"],
            help="Benchmark type (default: performance)"
        )
        
        parser.add_argument(
            "--config", default="configs/default.cfg",
            help="Configuration file path"
        )
        
        parser.add_argument(
            "--duration", type=int, default=60,
            help="Benchmark duration in seconds (default: 60)"
        )
        
        parsed = parser.parse_args(args)
        
        self.print_banner()
        print(f"Running {parsed.type} benchmarks...")
        print(f"Config: {parsed.config}")
        print(f"Duration: {parsed.duration} seconds")
        print()
        print("Note: This is a CLI framework demonstration.")
        print("Actual benchmarks would be implemented using performance measurement tools.")
        print()
        print("To run benchmarks, use:")
        print("  Custom benchmark tools integrated into the C++ codebase")
        
        return 0
    
    def experiment_command(self, args):
        """Execute the experiment command"""
        parser = argparse.ArgumentParser(
            description="Run experiments",
            prog="nlm_cli experiment"
        )
        
        parser.add_argument(
            "--name", required=True,
            help="Experiment name"
        )
        
        parser.add_argument(
            "--config", default="configs/default.cfg",
            help="Configuration file path"
        )
        
        parser.add_argument(
            "--steps", type=int, default=1000,
            help="Number of simulation steps (default: 1000)"
        )
        
        parser.add_argument(
            "--neurons", type=int, default=500,
            help="Number of neurons (default: 500)"
        )
        
        parser.add_argument(
            "--output", default="./experiments",
            help="Output directory for experiment results (default: ./experiments)"
        )
        
        parsed = parser.parse_args(args)
        
        self.print_banner()
        print(f"Running experiment: {parsed.name}")
        print(f"Config: {parsed.config}")
        print(f"Steps: {parsed.steps}")
        print(f"Neurons: {parsed.neurons}")
        print(f"Output: {parsed.output}")
        print()
        print("Note: This is a CLI framework demonstration.")
        print("Actual experiments would be implemented using the Experiment framework.")
        print()
        print("To run experiments, use:")
        print("  Experiment class methods:")
        print("    - run()")
        print("    - saveResults()")
        print("    - loadResults()")
        
        return 0
    
    def help_command(self, args):
        """Execute the help command"""
        if not args:
            self.print_banner()
            print("NLM Command Line Interface")
            print("=" * 70)
            print()
            print("Usage: nlm_cli [OPTIONS] COMMAND [ARGS...]")
            print()
            print("Options:")
            print("  --help          Show this message and exit")
            print("  --version       Show version information and exit")
            print()
            print("Commands:")
            print("  run             Run neural simulation")
            print("  test            Run tests")
            print("  config          Manage configuration")
            print("  benchmark       Run benchmarks")
            print("  experiment      Run experiments")
            print("  help            Show help for a specific command")
            print()
            print("For more information about a specific command, use:")
            print("  nlm_cli help COMMAND")
            return 0
        
        commands = {
            "run": "Run neural simulation - Execute neural network simulations with various modes",
            "test": "Run tests - Execute unit, integration, and performance tests",
            "config": "Manage configuration - Load, save, and modify configuration files",
            "benchmark": "Run benchmarks - Performance and resource usage measurements",
            "experiment": "Run experiments - Execute neural network experiments with custom parameters",
            "help": "Show help - Display help information for commands"
        }
        
        if args[0] in commands:
            self.print_banner()
            print(f"Help: nlm_cli {args[0]}")
            print("=" * 70)
            print()
            print(commands[args[0]])
            print()
            
            # Get argument parser for the command
            if args[0] == "run":
                self.run_command(["--help"])
            elif args[0] == "test":
                self.test_command(["--help"])
            elif args[0] == "config":
                self.config_command([args[1], "--help"] if len(args) > 1 else ["--help"])
            elif args[0] == "benchmark":
                self.benchmark_command(["--help"])
            elif args[0] == "experiment":
                self.experiment_command(["--help"])
            elif args[0] == "help":
                self.help_command(["--help"])
        else:
            print(f"Unknown command: {args[0]}")
            return 1
        
        return 0
    
    def main(self, args):
        """Main CLI entry point"""
        if not args:
            args = ["--help"]
        
        # Parse top-level arguments
        parser = argparse.ArgumentParser(add_help=False)
        
        # Add options that apply to all commands
        parser.add_argument("--help", action="store_true")
        parser.add_argument("--version", action="store_true")
        
        # Parse the first part of the arguments
        temp_args = args[:1]
        try:
            parsed = parser.parse_args(temp_args)
        except SystemExit:
            return 0
        
        if parsed.help:
            self.help_command(args[1:])
            return 0
        
        if parsed.version:
            self.print_version()
            return 0
        
        # The rest are command and arguments
        if len(args) < 1:
            self.help_command([])
            return 0
        
        command = args[0]
        command_args = args[1:]
        
        # Dispatch to the appropriate command handler
        command_handlers = {
            "run": self.run_command,
            "test": self.test_command,
            "config": self.config_command,
            "benchmark": self.benchmark_command,
            "experiment": self.experiment_command,
            "help": self.help_command
        }
        
        if command in command_handlers:
            return command_handlers[command](command_args)
        else:
            print(f"Unknown command: {command}")
            print("Use 'nlm_cli help' for a list of commands")
            return 1

def main():
    cli = NLMCLI()
    return cli.main(sys.argv[1:])

if __name__ == "__main__":
    sys.exit(main())
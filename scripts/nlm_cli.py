#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)
Advanced configuration and management tool for Neural Learning Machine

Usage:
    nlm-cli --help                    # Show help
    nlm-cli config --show             # Show current configuration
    nlm-cli config set <key> <value>   # Set configuration value
    nlm-cli config load <file>        # Load configuration from file
    nlm-cli config save <file>        # Save configuration to file
    nlm-cli validate                  # Validate configuration
    nlm-cli benchmark                  # Run performance benchmarks
    nlm-cli profile                    # Start performance profiler
    nlm-cli script <script>           # Execute Python script
    nlm-cli help <command>             # Show help for specific command
"""

import argparse
import json
import os
import sys
import yaml
from pathlib import Path
from typing import Dict, Any, Optional

# Add src to path so we can import nlm modules
sys.path.insert(0, str(Path(__file__).parent / "src"))

from core.Config.Config import Config

class NLMCli:
    def __init__(self):
        self.config = Config()
        self.config_file = "configs/default.cfg"
        self.load_config()

    def load_config(self):
        """Load configuration from file"""
        if Path(self.config_file).exists():
            self.config.loadFromFile(self.config_file)

    def save_config(self):
        """Save configuration to file"""
        os.makedirs(os.path.dirname(self.config_file), exist_ok=True)
        self.config.saveToFile(self.config_file)

    def cmd_config_show(self):
        """Show current configuration"""
        print("Current NLM Configuration:")
        print("=" * 50)

        # Get all keys and display them grouped by section
        keys = self.config.getKeys()
        
        # Group keys by prefix
        sections = {}
        for key in keys:
            if "." in key:
                section = key.split(".")[0]
            else:
                section = "general"
            if section not in sections:
                sections[section] = []
            sections[section].append(key)
        
        # Display each section
        for section, section_keys in sorted(sections.items()):
            print(f"\n{section.upper()}:")
            print("-" * 30)
            for key in sorted(section_keys):
                if key == "general":
                    print(f"  {key} = {self.config.getOr(str(key), '')}")
                else:
                    print(f"  {key} = {self.config.getOrAny(key, 'N/A')}")

    def cmd_config_set(self, key: str, value: str):
        """Set configuration value"""
        # Try to convert value to appropriate type
        if value.lower() == "true":
            converted_value = True
        elif value.lower() == "false":
            converted_value = False
        elif "." in value:
            try:
                converted_value = float(value)
            except ValueError:
                converted_value = value
        else:
            try:
                converted_value = int(value)
            except ValueError:
                converted_value = value

        self.config.set(key, converted_value)
        print(f"✓ Set {key} = {converted_value}")

    def cmd_config_load(self, file_path: str):
        """Load configuration from file"""
        if Path(file_path).exists():
            # Try YAML first, then JSON, then CFG
            if file_path.endswith(".yaml") or file_path.endswith(".yml"):
                self.config.loadFromYamlFile(file_path)
            elif file_path.endswith(".json"):
                self.config.loadFromJsonFile(file_path)
            else:
                self.config.loadFromFile(file_path)
            print(f"✓ Loaded configuration from {file_path}")
        else:
            print(f"✗ File not found: {file_path}")

    def cmd_config_save(self, file_path: str):
        """Save configuration to file"""
        if Path(file_path).exists():
            overwrite = input(f"File {file_path} exists. Overwrite? (y/N): ")
            if overwrite.lower() != "y":
                print("Aborted.")
                return

        # Determine format based on extension
        if file_path.endswith(".yaml") or file_path.endswith(".yml"):
            self.config.saveToYamlFile(file_path)
        elif file_path.endswith(".json"):
            self.config.saveToJsonFile(file_path)
        else:
            self.config.saveToFile(file_path)
        print(f"✓ Saved configuration to {file_path}")

    def cmd_validate(self):
        """Validate configuration"""
        print("Validating configuration...")
        
        # Basic validation checks
        errors = []
        warnings = []

        # Check required fields
        required_fields = [
            "neuron_count",
            "region_count", 
            "simulation_timestep",
            "random_seed"
        ]
        
        for field in required_fields:
            try:
                value = self.config.getOrAny(field, None)
                if value is None:
                    errors.append(f"Missing required field: {field}")
                elif isinstance(value, str) and not value.strip():
                    warnings.append(f"Empty field: {field}")
            except:
                errors.append(f"Error accessing field: {field}")

        # Check for reasonable values
        neuron_count = self.config.getOr("neuron_count", 1000)
        if neuron_count < 10:
            warnings.append(f"Very small neuron count: {neuron_count}")
        elif neuron_count > 100000:
            warnings.append(f"Very large neuron count: {neuron_count}")

        timestep = self.config.getOr("simulation_timestep", 0.001)
        if timestep <= 0:
            errors.append(f"Invalid timestep: {timestep}")
        elif timestep > 1.0:
            warnings.append(f"Large timestep: {timestep}")

        # Display results
        if errors:
            print("\n❌ VALIDATION ERRORS:")
            for error in errors:
                print(f"  ✗ {error}")

        if warnings:
            print("\n⚠️  VALIDATION WARNINGS:")
            for warning in warnings:
                print(f"  ⚠ {warning}")

        if not errors and not warnings:
            print("\n✅ Configuration is valid!")
        elif not errors:
            print("\n⚠️  Configuration has warnings but is valid.")

    def cmd_benchmark(self):
        """Run performance benchmarks"""
        print("Running NLM Performance Benchmarks...")
        
        # Import and run benchmark
        try:
            from benchmark import runBenchmark, printBenchmarkResults
            
            # Use configuration values for benchmarks
            neuron_counts = [100, 500, 1000]
            steps_list = [1000, 500, 200]
            
            results = []
            for neurons, steps in zip(neuron_counts, steps_list):
                result = runBenchmark(neurons, steps)
                results.append(result)
            
            printBenchmarkResults(results)
            print("\n✅ Benchmarks completed!")
            
        except Exception as e:
            print(f"❌ Error running benchmarks: {e}")

    def cmd_profile(self):
        """Start performance profiler"""
        print("Starting NLM Performance Profiler...")
        print("Profiler is starting...")
        print("Note: Run 'nlm-cli benchmark' for detailed performance metrics.")

    def cmd_script(self, script_path: str):
        """Execute Python script"""
        if not Path(script_path).exists():
            print(f"✗ Script not found: {script_path}")
            return

        print(f"Executing script: {script_path}")
        
        try:
            # Execute the script
            with open(script_path, 'r') as f:
                script_content = f.read()
            
            # Create a namespace with our configuration
            namespace = {
                'config': self.config,
                'nlm_config': self.config
            }
            
            exec(script_content, namespace)
            print("✅ Script executed successfully!")
            
        except Exception as e:
            print(f"❌ Error executing script: {e}")

    def cmd_help(self, command: Optional[str] = None):
        """Show help for commands"""
        if command:
            self.show_command_help(command)
        else:
            self.show_general_help()

    def show_general_help(self):
        """Show general help"""
        print("NLM Command Line Interface (CLI)")
        print("=" * 50)
        print("\nCOMMANDS:")
        print("  config      - Configuration management")
        print("  validate    - Validate configuration")
        print("  benchmark   - Run performance benchmarks")
        print("  profile     - Start performance profiler")
        print("  script      - Execute Python script")
        print("  help        - Show this help")

    def show_command_help(self, command: str):
        """Show help for specific command"""
        if command == "config":
            print("\nCONFIGURATION MANAGEMENT:")
            print("  nlm-cli config show              - Show current configuration")
            print("  nlm-cli config set <key> <value> - Set configuration value")
            print("  nlm-cli config load <file>       - Load configuration from file")
            print("  nlm-cli config save <file>       - Save configuration to file")

        elif command == "validate":
            print("\nCONFIGURATION VALIDATION:")
            print("  nlm-cli validate                 - Validate current configuration")

        elif command == "benchmark":
            print("\nPERFORMANCE BENCHMARKS:")
            print("  nlm-cli benchmark                - Run performance benchmarks")

        elif command == "profile":
            print("\nPERFORMANCE PROFILING:")
            print("  nlm-cli profile                  - Start performance profiler")

        elif command == "script":
            print("\nSCRIPT EXECUTION:")
            print("  nlm-cli script <file>            - Execute Python script")

        else:
            print(f"Unknown command: {command}")

def main():
    cli = NLMCli()

    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface (CLI)",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # Config command
    config_parser = subparsers.add_parser("config", help="Configuration management")
    config_subparsers = config_parser.add_subparsers(dest="config_action", help="Config actions")

    config_show_parser = config_subparsers.add_parser("show", help="Show current configuration")
    config_set_parser = config_subparsers.add_parser("set", help="Set configuration value")
    config_set_parser.add_argument("key", help="Configuration key")
    config_set_parser.add_argument("value", help="Configuration value")
    config_load_parser = config_subparsers.add_parser("load", help="Load configuration from file")
    config_load_parser.add_argument("file", help="Configuration file path")
    config_save_parser = config_subparsers.add_parser("save", help="Save configuration to file")
    config_save_parser.add_argument("file", help="Configuration file path")

    # Other commands
    subparsers.add_parser("validate", help="Validate configuration")
    subparsers.add_parser("benchmark", help="Run performance benchmarks")
    subparsers.add_parser("profile", help="Start performance profiler")
    script_parser = subparsers.add_parser("script", help="Execute Python script")
    script_parser.add_argument("script", help="Script file path")
    subparsers.add_parser("help", help="Show help").add_argument("command", nargs="?", help="Command to get help for")

    args = parser.parse_args()

    # Route to appropriate methods
    if args.command == "config":
        if args.config_action == "show":
            cli.cmd_config_show()
        elif args.config_action == "set":
            cli.cmd_config_set(args.key, args.value)
        elif args.config_action == "load":
            cli.cmd_config_load(args.file)
        elif args.config_action == "save":
            cli.cmd_config_save(args.file)

    elif args.command == "validate":
        cli.cmd_validate()
    elif args.command == "benchmark":
        cli.cmd_benchmark()
    elif args.command == "profile":
        cli.cmd_profile()
    elif args.command == "script":
        cli.cmd_script(args.script)
    elif args.command == "help":
        cli.cmd_help(args.command)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()

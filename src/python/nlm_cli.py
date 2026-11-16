#!/usr/bin/env python3
"""
NLM - Neural Learning Machine Python Command Line Interface

This module provides a unified command-line interface for NLM,
allowing users to run simulations, experiments, and commands
from the Python environment.
"""

import sys
import os
import argparse
import json
import yaml
import subprocess
import threading
import time
from pathlib import Path
from typing import Optional, List, Dict, Any

# Add the current directory to the path to import pynlm modules
sys.path.insert(0, str(Path(__file__).parent))

try:
    from pynlm import (
        Config, Brain, AgentBrain, SimpleWorld,
        Vision, SensoryPercept, Action, MotorCommand,
        AgentBody, WorldObject, ActionResult,
        NeuronType, SynapseType, DevelopmentalStage,
        INVALID_NEURON_ID, INVALID_SYNAPSE_ID,
        INVALID_REGION_ID, INVALID_POPULATION_ID
    )
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: NLM Python bindings not available - {e}")

# Import enhanced bindings
try:
    from pynlm_enhanced import (
        ConfigManager, EnhancedBrain, EnhancedAgentBrain, EnhancedWorld,
        BrainContext, NLMError, NLMInitializationError,
        NLMSimulationError, NLMConfigError
    )
    ENHANCED_AVAILABLE = True
except ImportError as e:
    ENHANCED_AVAILABLE = False
    print(f"Note: Enhanced NLM Python bindings not available - {e}")

class NLMCmdLine:
    """Main NLM command line interface class."""
    
    def __init__(self):
        self.config_file = "configs/default.cfg"
        self.log_level = "info"
        self.verbose = False
        self.enhanced_mode = ENHANCED_AVAILABLE
        
    def print_banner(self):
        """Print NLM banner."""
        print("""
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                               ║
║     Unified Python Command Line Interface                     ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
        """)
    
    def print_help(self):
        """Print help information."""
        print("""
NLM - Neural Learning Machine Python CLI

Usage:
  nlm [options] [command] [command_args]

Commands:
  simulate      Run neural simulation (requires --config or --file)
  experiment    Run experimental protocol
  benchmark     Run performance benchmarks
  config        Configuration management (load, save, validate, show)
  demo          Run pre-defined demonstrations
  status        Show current system status
  help          Show this help message

Global Options:
  --help, -h              Show this help message
  --version, -v           Show version information
  --config FILE, -c FILE   Load configuration from file
  --log-level LEVEL, -l LEVEL    Set logging level
  --verbose, -v            Enable verbose output
  --enhanced, -e           Use enhanced Python bindings

Phase-specific commands:
  phase2          Run Phase 2: Real neural computation
  phase3          Run Phase 3: World interaction
  phase4          Run Phase 4: Cognitive capabilities
  phase5          Run Phase 5: Performance optimizations
  phase6          Run Phase 6: Integration tests

For command-specific help:
  nlm command --help
        """)
    
    def print_version(self):
        """Print version information."""
        print("NLM - Neural Learning Machine Python CLI v0.1.0")
        print("Enhanced Python bindings available:", self.enhanced_mode)
        print("Base Python bindings available:", PYNLM_AVAILABLE)
    
    def load_configuration(self, filepath: str) -> Config:
        """Load configuration from file."""
        config = Config()
        
        # Try different file formats
        try:
            if filepath.endswith(".json"):
                # For JSON files, try to load (if supported)
                # This is a placeholder - actual JSON loading would depend on implementation
                print(f"Note: JSON config loading not yet implemented for basic bindings")
            else:
                # Use standard config file loading
                config.loadFromFile(filepath)
        except Exception as e:
            print(f"Error loading config from {filepath}: {e}")
            return Config()  # Return default config
        
        return config
    
    def run_simulation(self, config_file: Optional[str] = None, 
                      episodes: int = 1, 
                      steps_per_episode: int = 100,
                      use_enhanced: bool = False):
        """Run neural simulation."""
        if not PYNLM_AVAILABLE:
            print("Error: Base NLM bindings not available")
            return 1
        
        try:
            # Use enhanced bindings if available and requested
            if use_enhanced and self.enhanced_mode:
                config = ConfigManager()
                if config_file:
                    config.loadFromFile(config_file)
                elif self.config_file:
                    config.loadFromFile(self.config_file)
                
                print(f"Running enhanced simulation with {episodes} episodes, {steps_per_episode} steps each...")
                
                # Create enhanced brain
                brain = EnhancedBrain(config.getConfig())
                
                # Run simulation loop
                total_reward = 0.0
                total_spikes = 0
                
                for episode in range(episodes):
                    brain.reset()
                    
                    for step in range(steps_per_episode):
                        brain.step()
                        
                        if self.verbose:
                            print(f"  Episode {episode + 1}, Step {step + 1}: "
                                  f"Firing={brain.getFiringNeuronCount()}, "
                                  f"Spikes={brain.getTotalSpikeCount()}, "
                                  f"AvgRate={brain.getAverageFiringRate():.2f}/s")
                    
                    # Record episode metrics
                    total_reward += brain.getAverageFiringRate() * 0.1  # Placeholder reward
                    total_spikes += brain.getTotalSpikeCount()
                
                # Print summary
                print(f"\nSimulation Complete:")
                print(f"  Episodes: {episodes}")
                print(f"  Steps per Episode: {steps_per_episode}")
                print(f"  Total Reward: {total_reward:.2f}")
                print(f"  Total Spikes: {total_spikes}")
                print(f"  Average Firing Rate: {total_spikes / (episodes * steps_per_episode):.2f}/s")
                
                return 0
            
            else:
                # Use base bindings
                config = Config()
                if config_file:
                    config.loadFromFile(config_file)
                elif self.config_file:
                    config.loadFromFile(self.config_file)
                
                print(f"Running basic simulation with {episodes} episodes, {steps_per_episode} steps each...")
                
                # Create brain
                brain = Brain(config)
                if not brain.initialize():
                    print("Error: Failed to initialize brain")
                    return 1
                
                # Run simulation loop (simplified)
                total_steps = 0
                for episode in range(episodes):
                    print(f"Episode {episode + 1}/{episodes}")
                    brain.reset()
                    
                    for step in range(steps_per_episode):
                        brain.step(step, step * 0.001)
                        total_steps++
                        
                        if self.verbose:
                            print(f"  Step {step + 1}: "
                                  f"Neurons={brain.getTotalNeuronCount()}, "
                                  f"Spikes={brain.getTotalSpikeCount()}, "
                                  f"Firing={brain.getFiringNeuronCount()}")
                
                # Print summary
                print(f"\nSimulation Complete:")
                print(f"  Episodes: {episodes}")
                print(f"  Steps per Episode: {steps_per_episode}")
                print(f"  Total Steps: {total_steps}")
                print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
                print(f"  Average Firing Rate: {brain.getAverageFiringRate():.2f}/s")
                
                return 0
                
        except Exception as e:
            print(f"Error during simulation: {e}")
            return 1
    
    def run_experiment(self, experiment_type: str, args: List[str]):
        """Run experimental protocol."""
        print(f"Running experiment: {experiment_type}")
        
        # Map experiment types to their arguments
        experiment_map = {
            "temporal-prediction": {"trials": 50},
            "working-memory": {"trials": 30},
            "episodic-recall": {"trials": 20},
            "concept-formation": {"trials": 25},
            "attention": {"trials": 40},
            "planning": {"trials": 15},
            "self-model": {"trials": 10},
            "social-learning": {"trials": 20},
            "continual-learning": {"trials": 35},
        }
        
        if experiment_type not in experiment_map:
            print(f"Error: Unknown experiment type '{experiment_type}'")
            return 1
        
        config = ConfigManager()
        experiment_config = experiment_map[experiment_type]
        
        # Apply experiment configuration
        for key, value in experiment_config.items():
            config.set(key, value)
        
        print(f"Experiment '{experiment_type}' configuration:")
        for key, value in experiment_config.items():
            print(f"  {key}: {value}")
        
        # Run experiment based on type (placeholder implementation)
        print(f"Running {experiment_type} experiment (implementation pending)...")
        
        return 0
    
    def run_benchmark(self, benchmark_type: str = "quick", iterations: int = 1):
        """Run performance benchmarks."""
        print(f"Running benchmark: {benchmark_type}")
        
        if not PYNLM_AVAILABLE:
            print("Error: Base NLM bindings not available")
            return 1
        
        config = Config()
        
        # Different benchmark configurations
        benchmark_configs = {
            "quick": [
                {"neurons": 100, "steps": 100},
                {"neurons": 500, "steps": 100},
                {"neurons": 1000, "steps": 50},
            ],
            "comprehensive": [
                {"neurons": 100, "steps": 1000},
                {"neurons": 500, "steps": 1000},
                {"neurons": 1000, "steps": 500},
                {"neurons": 2000, "steps": 200},
                {"neurons": 5000, "steps": 100},
            ]
        }
        
        if benchmark_type not in benchmark_configs:
            print(f"Error: Unknown benchmark type '{benchmark_type}'")
            return 1
        
        results = []
        
        for i in range(iterations):
            print(f"\nBenchmark iteration {i + 1}/{iterations}")
            
            for bench_config in benchmark_configs[benchmark_type]:
                print(f"  Testing {bench_config['neurons']} neurons, {bench_config['steps']} steps...")
                
                # Create and run benchmark (simplified)
                config = Config()
                config.set("neuron_count", bench_config["neurons"])
                config.set("region_count", 1)
                config.set("connection_probability", 0.1)
                
                brain = Brain(config)
                if not brain.initialize():
                    print(f"    Error: Failed to initialize brain")
                    continue
                
                start_time = time.time()
                
                for step in range(bench_config["steps"]):
                    brain.step(step, step * 0.001)
                
                end_time = time.time()
                elapsed = end_time - start_time
                
                steps_per_second = bench_config["steps"] / elapsed
                
                result = {
                    "neurons": bench_config["neurons"],
                    "steps": bench_config["steps"],
                    "steps_per_second": steps_per_second,
                    "total_spikes": brain.getTotalSpikeCount(),
                    "avg_firing_rate": brain.getAverageFiringRate(),
                    "time_elapsed": elapsed,
                }
                
                results.append(result)
                
                print(f"    Time: {elapsed:.2f}s, Steps/sec: {steps_per_second:.0f}, "
                      f"Spikes: {brain.getTotalSpikeCount()}, Avg Rate: {brain.getAverageFiringRate():.2f}/s")
        
        # Print benchmark results
        if results:
            print(f"\nBenchmark Results ({benchmark_type}, {iterations} iteration{'s' if iterations != 1 else ''}):")
            print("  Neurons | Steps/sec | Spikes/step | Avg Rate (/s) | Time (s)")
            print("  ------- | -------- | ----------- | ------------ | ---------")
            
            for result in results:
                spikes_per_step = result["total_spikes"] / result["steps"]
                print(f"  {result['neurons']:6} | {result['steps_per_second']:8.0f} | "
                      f"{spikes_per_step:11.2f} | {result['avg_firing_rate']:12.2f} | "
                      f"{result['time_elapsed']:9.2f}")
        
        return 0
    
    def manage_config(self, action: str, filepath: Optional[str] = None,
                     key: Optional[str] = None, value: Optional[str] = None):
        """Manage configuration files."""
        if not filepath:
            filepath = self.config_file
        
        print(f"Configuration management: {action}")
        
        if action == "load":
            config = ConfigManager()
            if config.loadFromFile(filepath):
                print(f"Configuration loaded from {filepath}")
                print(f"Keys: {config.getKeys()}")
                return 0
            else:
                print(f"Error: Failed to load configuration from {filepath}")
                return 1
                
        elif action == "save":
            config = ConfigManager()
            if filepath:
                if config.saveToFile(filepath):
                    print(f"Configuration saved to {filepath}")
                    return 0
                else:
                    print(f"Error: Failed to save configuration to {filepath}")
                    return 1
            
        elif action == "validate":
            config = ConfigManager()
            display_path = filepath if filepath else 'current'
            if config.validate():
                print(f"Configuration {display_path} is valid")
                return 0
            else:
                print(f"Error: Configuration {display_path} is invalid")
                return 1
                
        elif action == "show":
            config = ConfigManager()
            display_path = filepath if filepath else 'current'
            print(f"Configuration {display_path}:")
            print(config.summary())
            return 0
        
        print(f"Error: Unknown configuration action '{action}'")
        return 1
    
    def run_demo(self, demo_name: str, args: List[str]):
        """Run pre-defined demonstration."""
        print(f"Running demo: {demo_name}")
        
        # Map demo names to their implementations
        demo_map = {
            "phase2": {"description": "Real neural computation", "config": "configs/default.cfg"},
            "phase3": {"description": "World interaction demo", "config": "configs/default.cfg"},
            "phase4": {"description": "Cognitive capabilities demo", "config": "configs/default.cfg"},
            "phase5": {"description": "Performance optimizations demo", "config": "configs/default.cfg"},
            "phase6": {"description": "Integration test demo", "config": "configs/default.cfg"},
            "text": {"description": "Text/thought integration demo", "config": "configs/default.cfg"},
            "benchmark": {"description": "Performance benchmark demo", "config": "configs/default.cfg"},
        }
        
        if demo_name not in demo_map:
            print(f"Error: Unknown demo '{demo_name}'")
            return 1
        
        demo_info = demo_map[demo_name]
        print(f"Demo: {demo_info['description']}")
        print(f"Config: {demo_info['config']}")
        
        # Run the actual demo (simplified)
        if demo_name == "benchmark":
            return self.run_benchmark("quick", 1)
        else:
            # Use simulation with demo-specific parameters
            episodes = 1 if demo_name in ["text", "benchmark"] else 5
            steps = 100 if demo_name == "benchmark" else 200
            return self.run_simulation(demo_info["config"], episodes, steps, self.enhanced_mode)
    
    def show_status(self):
        """Show current system status."""
        print("NLM System Status:")
        
        print("  Python Bindings:")
        print(f"    Base NLM bindings: {'Available' if PYNLM_AVAILABLE else 'Unavailable'}")
        print(f"    Enhanced NLM bindings: {'Available' if self.enhanced_mode else 'Unavailable'}")
        
        print("\n  Configuration:")
        print(f"    Config file: {self.config_file}")
        print(f"    Log level: {self.log_level}")
        print(f"    Verbose mode: {self.verbose}")
        
        # If we can access config, show more details
        if PYNLM_AVAILABLE:
            config = Config()
            try:
                config.loadFromFile(self.config_file)
                print(f"\n  Current Configuration:")
                print(f"    Keys: {config.getKeys()}")
            except:
                print(f"\n  Configuration file '{self.config_file}' not found")
        
        return 0
    
    def run_command(self, args: List[str]) -> int:
        """Parse and run NLM command."""
        if not args:
            self.print_help()
            return 0
        
        command = args[0].lower()
        command_args = args[1:]
        
        # Handle global options
        if command in ["help", "--help", "-h"]:
            self.print_help()
            return 0
        
        if command in ["version", "--version", "-v"]:
            self.print_version()
            return 0
        
        if command in ["status"]:
            return self.show_status()
        
        # Main command handling
        if command == "simulate":
            config_file = None
            episodes = 1
            steps = 100
            use_enhanced = self.enhanced_mode
            
            # Parse command arguments
            i = 0
            while i < len(command_args):
                arg = command_args[i]
                if arg in ["--config", "-c"] and i + 1 < len(command_args):
                    config_file = command_args[i + 1]
                    i += 2
                elif arg == "--episodes" and i + 1 < len(command_args):
                    try:
                        episodes = int(command_args[i + 1])
                        i += 2
                    except ValueError:
                        print(f"Error: Invalid episode count '{command_args[i + 1]}'")
                        return 1
                elif arg == "--steps" and i + 1 < len(command_args):
                    try:
                        steps = int(command_args[i + 1])
                        i += 2
                    except ValueError:
                        print(f"Error: Invalid step count '{command_args[i + 1]}'")
                        return 1
                elif arg in ["--enhanced", "-e"]:
                    use_enhanced = True
                    i += 1
                elif arg == "--basic" or arg == "-b":
                    use_enhanced = False
                    i += 1
                else:
                    i += 1
            
            return self.run_simulation(config_file, episodes, steps, use_enhanced)
        
        elif command == "experiment":
            if not command_args:
                print("Error: No experiment type specified")
                print("Usage: nlm experiment <type> [--arg value]...")
                return 1
            
            return self.run_experiment(command_args[0], command_args[1:])
        
        elif command == "benchmark":
            benchmark_type = "quick"
            iterations = 1
            
            i = 0
            while i < len(command_args):
                arg = command_args[i]
                if arg in ["--type", "-t"] and i + 1 < len(command_args):
                    benchmark_type = command_args[i + 1]
                    i += 2
                elif arg in ["--iterations", "-i"] and i + 1 < len(command_args):
                    try:
                        iterations = int(command_args[i + 1])
                        i += 2
                    except ValueError:
                        print(f"Error: Invalid iteration count '{command_args[i + 1]}'")
                        return 1
                else:
                    i += 1
            
            return self.run_benchmark(benchmark_type, iterations)
        
        elif command == "config":
            if not command_args:
                print("Error: No configuration action specified")
                print("Usage: nlm config <action> [--file filepath] [--key key --value value]...")
                return 1
            
            action = command_args[0]
            filepath = None
            key = None
            value = None
            
            i = 1
            while i < len(command_args):
                arg = command_args[i]
                if arg in ["--file", "-f"] and i + 1 < len(command_args):
                    filepath = command_args[i + 1]
                    i += 2
                elif arg in ["--key", "-k"] and i + 1 < len(command_args):
                    key = command_args[i + 1]
                    i += 2
                elif arg in ["--value", "-v"] and i + 1 < len(command_args):
                    value = command_args[i + 1]
                    i += 2
                else:
                    i += 1
            
            if key and value:
                # Set a specific key-value pair
                config = ConfigManager()
                config.set(key, value)
                return self.manage_config(action, filepath)
            else:
                return self.manage_config(action, filepath)
        
        elif command == "demo":
            if not command_args:
                print("Error: No demo specified")
                print("Usage: nlm demo <demo_name>")
                return 1
            
            return self.run_demo(command_args[0], command_args[1:])
        
        # Phase-specific commands
        elif command in ["phase2", "phase3", "phase4", "phase5", "phase6"]:
            phase_num = int(command[5:])
            episodes = 1
            steps = 100
            
            # Parse phase-specific arguments
            i = 0
            while i < len(command_args):
                arg = command_args[i]
                if arg in ["--episodes", "--e"] and i + 1 < len(command_args):
                    try:
                        episodes = int(command_args[i + 1])
                        i += 2
                    except ValueError:
                        print(f"Error: Invalid episode count '{command_args[i +したもの]}'")
                        return 1
                elif arg in ["--steps", "--s"] and i + 1 < len(command_args):
                    try:
                        steps = int(command_args[i + 1])
                        i += 2
                    except ValueError:
                        print(f"Error: Invalid step count '{command_args[i + 1]}'")
                        return 1
                else:
                    i += 1
            
            # For phase commands, we could run specific experiments
            print(f"Running Phase {phase_num} simulation with {episodes} episodes, {steps} steps each...")
            
            # Use the configured config file
            config_file = self.config_file
            
            return self.run_simulation(config_file, episodes, steps, self.enhanced_mode)
        
        else:
            print(f"Error: Unknown command '{command}'")
            return 1


def main():
    """Main entry point for NLM CLI."""
    try:
        # Create NLM CLI instance
        nlm_cli = NLMCmdLine()
        
        # Determine if enhanced bindings should be used
        enhanced_available = os.environ.get("NLM_ENHANCED", "").lower() == "1"
        nlm_cli.enhanced_mode = ENHANCED_AVAILABLE and enhanced_available
        
        # Parse global arguments first
        global_args = []
        command_args = []
        command_found = False
        
        i = 1
        while i < len(sys.argv):
            arg = sys.argv[i]
            
            if arg in ["--help", "-h"]:
                nlm_cli.print_help()
                return 0
            
            elif arg in ["--version", "-v"]:
                nlm_cli.print_version()
                return 0
            
            elif arg in ["--enhanced", "-e"]:
                nlm_cli.enhanced_mode = True
                i += 1
            elif arg in ["--basic", "-b"]:
                nlm_cli.enhanced_mode = False
                i += 1
            
            elif arg in ["--log-level", "-l"]:
                if i + 1 < len(sys.argv):
                    nlm_cli.log_level = sys.argv[i + 1]
                    i += 2
                else:
                    print("Error: Log level requires a value")
                    return 1
            
            elif arg in ["--config", "-c"]:
                if i + 1 < len(sys.argv):
                    nlm_cli.config_file = sys.argv[i + 1]
                    i += 2
                else:
                    print("Error: Config file requires a path")
                    return 1
            
            elif arg in ["--verbose", "-vv"]:
                nlm_cli.verbose = True
                i += 1
            
            else:
                if command_found:
                    command_args.append(arg)
                else:
                    global_args.append(arg)
                    if arg and not arg.startswith("-"):
                        command_found = True
                    i += 1
        
        # Update config file if specified in global args
        for i in range(len(global_args)):
            if global_args[i] in ["--config", "-c"] and i + 1 < len(global_args):
                nlm_cli.config_file = global_args[i + 1]
                break
        
        # Combine global and command arguments
        all_args = global_args + command_args
        
        if not all_args:
            nlm_cli.print_help()
            return 0
        
        # Run the command
        return nlm_cli.run_command(all_args)
        
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        return 1
if __name__ == "__main__":
    sys.exit(main())

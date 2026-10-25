#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Python Interface

This module provides Python bindings and automation scripts for the NLM C++ neural simulation framework.
It enables researchers and developers to easily run experiments, manage configurations, and analyze results.

Features:
- Run NLM experiments with Pythonic interface
- Configuration management
- Experiment management and tracking
- Result analysis and visualization
- Batch processing of experiments
"""

import os
import sys
import json
import argparse
import subprocess
import time
import threading
from typing import Dict, List, Optional, Union, Any
from pathlib import Path
from dataclasses import dataclass, asdict
from enum import Enum
import numpy as np

# Try to import nlm bindings
try:
    import nlm as nlm_module
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False

@dataclass
class ExperimentConfig:
    """Configuration for an NLM experiment"""
    experiment_type: str = "demo"
    neuron_count: int = 1000
    simulation_steps: int = 1000
    timestep: float = 0.001
    random_seed: int = 42
    output_path: str = "./results"
    config_file: str = "configs/default.cfg"
    checkpoint_path: Optional[str] = None
    enable_checkpoint: bool = False
    enable_replay: bool = False
    enable_development: bool = False
    custom_settings: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.custom_settings is None:
            self.custom_settings = {}

class ExperimentResult:
    """Results from an NLM experiment"""
    def __init__(self, experiment_type: str):
        self.experiment_type = experiment_type
        self.metrics: Dict[str, Any] = {}
        self.timestamp: float = time.time()
        self.success: bool = False
        self.error_message: Optional[str] = None
        self.execution_time: float = 0.0
    
    def add_metric(self, key: str, value: Any):
        self.metrics[key] = value
    
    def to_dict(self) -> Dict:
        return {
            "experiment_type": self.experiment_type,
            "metrics": self.metrics,
            "timestamp": self.timestamp,
            "success": self.success,
            "error_message": self.error_message,
            "execution_time": self.execution_time
        }

class NLMExperimentRunner:
    """Run NLM experiments with various backends"""
    
    def __init__(self):
        self.build_dir = Path("build")
        self.cli_path = self.build_dir / "nlm-cli"
    
    def run_experiment(self, config: ExperimentConfig) -> ExperimentResult:
        """Run an NLM experiment with the given configuration"""
        result = ExperimentResult(config.experiment_type)
        start_time = time.time()
        
        try:
            # Build the NLM framework if needed
            if not self.build_dir.exists() or not self.cli_path.exists():
                self.build_nlm()
            
            # Prepare command
            cmd = [
                str(self.cli_path),
                "--experiment", config.experiment_type,
                "--neurons", str(config.neuron_count),
                "--steps", str(config.simulation_steps),
                "--config", config.config_file
            ]
            
            # Add optional flags
            if config.checkpoint_path:
                cmd.extend(["--checkpoint", config.checkpoint_path])
            
            if config.enable_replay:
                cmd.append("--enable-replay")
            
            if config.enable_development:
                cmd.append("--enable-development")
            
            # Add custom settings
            for key, value in config.custom_settings.items():
                if isinstance(value, str):
                    cmd.extend(["--set", f"{key}={value}"])
                elif isinstance(value, (int, float)):
                    cmd.extend(["--set", f"{key}={value}"])
                elif isinstance(value, bool):
                    cmd.extend(["--set", f"{key}={value}"])
            
            # Run the experiment
            result.execution_time = time.time() - start_time
            
            if NLM_AVAILABLE and config.experiment_type == "phase6":
                # Use the Python bindings if available
                result = self._run_with_bindings(config)
            else:
                # Use CLI
                result = self._run_with_cli(config, cmd)
            
            result.success = True
            result.execution_time = time.time() - start_time
            
        except Exception as e:
            result.error_message = str(e)
            result.execution_time = time.time() - start_time
        
        return result
    
    def build_nlm(self):
        """Build the NLM framework"""
        print("Building NLM framework...")
        
        # Check for CMakeLists.txt
        if not Path("CMakeLists.txt").exists():
            raise RuntimeError("CMakeLists.txt not found. Please run from project root.")
        
        # Create build directory if it doesn't exist
        self.build_dir.mkdir(exist_ok=True)
        
        # Run CMake
        subprocess.run(["cmake", ".."], cwd=self.build_dir, check=True)
        
        # Run make
        subprocess.run(["make", "-j4"], cwd=self.build_dir, check=True)
        
        print("NLM framework built successfully!")
    
    def _run_with_bindings(self, config: ExperimentConfig) -> ExperimentResult:
        """Run experiment using Python bindings"""
        result = ExperimentResult(config.experiment_type)
        
        # Create configuration
        cfg = nlm_module.createDefaultConfig()
        
        # Set configuration values
        cfg.set("neuron_count", config.neuron_count)
        cfg.set("simulation_timestep", config.timestep)
        cfg.set("random_seed", config.random_seed)
        
        # Add custom settings
        for key, value in config.custom_settings.items():
            cfg.set(key, value)
        
        # Create brain
        brain = nlm_module.createBrain(cfg)
        
        if not brain.initialize():
            raise RuntimeError("Failed to initialize brain")
        
        # Run simulation
        for step in range(config.simulation_steps):
            brain.step(step, step * config.timestep)
            
            # Collect metrics every 100 steps
            if step % 100 == 0:
                result.add_metric(f"firing_rate_step_{step}", brain.getAverageFiringRate())
                result.add_metric(f"spike_count_step_{step}", brain.getTotalSpikeCount())
        
        # Add final metrics
        result.add_metric("final_firing_rate", brain.getAverageFiringRate())
        result.add_metric("final_spike_count", brain.getTotalSpikeCount())
        
        return result
    
    def _run_with_cli(self, config: ExperimentConfig, cmd: List[str]) -> ExperimentResult:
        """Run experiment using CLI"""
        result = ExperimentResult(config.experiment_type)
        
        # Save output to file if specified
        if config.output_path:
            Path(config.output_path).mkdir(exist_ok=True)
            output_file = Path(config.output_path) / f"{config.experiment_type}_results.json"
        else:
            output_file = None
        
        # Add output to command if we have a file
        if output_file:
            cmd.extend(["--output", str(output_file)])
        
        # Run the command
        print(f"Running command: {' '.join(cmd)}")
        
        try:
            result_process = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                check=True
            )
            
            # Parse output
            if output_file and output_file.exists():
                with open(output_file, 'r') as f:
                    result_data = json.load(f)
                    result.metrics = result_data.get("metrics", {})
            else:
                # Parse stdout
                output_lines = result_process.stdout.split('\n')
                for line in output_lines:
                    if "Total reward:" in line:
                        try:
                            reward = float(line.split(":")[-1].strip())
                            result.add_metric("total_reward", reward)
                        except ValueError:
                            pass
                    elif "Avg firing rate:" in line:
                        try:
                            rate = float(line.split(":")[-1].strip())
                            result.add_metric("avg_firing_rate", rate)
                        except ValueError:
                            pass
                    elif "Episodes stored:" in line:
                        try:
                            episodes = float(line.split(":")[-1].strip())
                            result.add_metric("episodes_stored", episodes)
                        except ValueError:
                            pass
            
        except subprocess.CalledProcessError as e:
            result.error_message = f"Command failed with exit code {e.returncode}"
            if e.stderr:
                result.error_message += f": {e.stderr}"
        
        return result

class NLMConfigurationManager:
    """Manage NLM configurations and experiments"""
    
    def __init__(self, config_dir: str = "configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
    
    def create_experiment_config(self, name: str, base_config: Optional[ExperimentConfig] = None) -> ExperimentConfig:
        """Create a new experiment configuration"""
        if base_config is None:
            base_config = ExperimentConfig()
        
        # Load base configuration from file if specified
        if base_config.config_file and Path(base_config.config_file).exists():
            # Load config values (simplified - would need proper parsing)
            pass
        
        # Return a copy with the given name
        return ExperimentConfig(
            experiment_type=base_config.experiment_type,
            neuron_count=base_config.neuron_count,
            simulation_steps=base_config.simulation_steps,
            timestep=base_config.timestep,
            random_seed=base_config.random_seed,
            output_path=f"./results/{name}",
            config_file=base_config.config_file,
            checkpoint_path=base_config.checkpoint_path,
            enable_checkpoint=base_config.enable_checkpoint,
            enable_replay=base_config.enable_replay,
            enable_development=base_config.enable_development,
            custom_settings=base_config.custom_settings.copy()
        )
    
    def save_config(self, config: ExperimentConfig, name: str) -> str:
        """Save experiment configuration to a file"""
        config_file = self.config_dir / f"{name}_config.json"
        
        with open(config_file, 'w') as f:
            json.dump(asdict(config), f, indent=2)
        
        return str(config_file)
    
    def load_config(self, name: str) -> ExperimentConfig:
        """Load experiment configuration from a file"""
        config_file = self.config_dir / f"{name}_config.json"
        
        if not config_file.exists():
            raise FileNotFoundError(f"Configuration file not found: {config_file}")
        
        with open(config_file, 'r') as f:
            config_dict = json.load(f)
        
        # Convert back to ExperimentConfig
        # Note: This is simplified - would need proper type conversion
        config = ExperimentConfig()
        for key, value in config_dict.items():
            if hasattr(config, key):
                setattr(config, key, value)
        
        return config

class NLMResultAnalyzer:
    """Analyze NLM experiment results"""
    
    @staticmethod
    def analyze_results(result: ExperimentResult) -> Dict[str, Any]:
        """Analyze experiment results"""
        analysis = {
            "experiment_type": result.experiment_type,
            "success": result.success,
            "duration": result.execution_time,
            "timestamp": result.timestamp,
            "metrics_count": len(result.metrics),
        }
        
        # Analyze metrics
        if result.metrics:
            # Calculate summary statistics for numeric metrics
            numeric_metrics = {}
            for key, value in result.metrics.items():
                if isinstance(value, (int, float)):
                    numeric_metrics[key] = value
            
            if numeric_metrics:
                analysis["numeric_metrics"] = numeric_metrics
                analysis["metric_summary"] = {
                    "min": min(numeric_metrics.values()),
                    "max": max(numeric_metrics.values()),
                    "mean": sum(numeric_metrics.values()) / len(numeric_metrics),
                    "count": len(numeric_metrics)
                }
        
        return analysis
    
    @staticmethod
    def compare_experiments(results: List[ExperimentResult]) -> Dict[str, Any]:
        """Compare multiple experiment results"""
        if not results:
            return {}
        
        comparison = {}
        
        # Group results by experiment type
        by_type = {}
        for result in results:
            if result.experiment_type not in by_type:
                by_type[result.experiment_type] = []
            by_type[result.experiment_type].append(result)
        
        # Analyze each experiment type
        for exp_type, type_results in by_type.items():
            comparison[exp_type] = {
                "count": len(type_results),
                "success_rate": sum(1 for r in type_results if r.success) / len(type_results),
                "avg_duration": sum(r.execution_time for r in type_results) / len(type_results),
            }
            
            # Collect metrics
            all_metrics = {}
            for result in type_results:
                for key, value in result.metrics.items():
                    if key not in all_metrics:
                        all_metrics[key] = []
                    all_metrics[key].append(value)
            
            comparison[exp_type]["common_metrics"] = {}
            for key, values in all_metrics.items():
                if all(isinstance(v, (int, float)) for v in values):
                    comparison[exp_type]["common_metrics"][key] = {
                        "avg": sum(values) / len(values),
                        "min": min(values),
                        "max": max(values),
                        "count": len(values)
                    }
        
        return comparison

class NLMAutomation:
    """Automate NLM experiments"""
    
    def __init__(self):
        self.runner = NLMExperimentRunner()
        self.config_manager = NLMConfigurationManager()
        self.analyzer = NLMResultAnalyzer()
    
    def run_batch_experiments(self, configs: List[ExperimentConfig]) -> List[ExperimentResult]:
        """Run a batch of experiments"""
        results = []
        
        for i, config in enumerate(configs):
            print(f"Running experiment {i+1}/{len(configs)}: {config.experiment_type}")
            
            try:
                result = self.runner.run_experiment(config)
                results.append(result)
                
                # Save result
                self.save_result(result)
                
            except Exception as e:
                print(f"Experiment {i+1} failed: {e}")
                # Create a failed result
                failed_result = ExperimentResult(config.experiment_type)
                failed_result.error_message = str(e)
                failed_result.success = False
                results.append(failed_result)
        
        return results
    
    def run_experiment_grid_search(self, base_config: ExperimentConfig, parameter_grid: Dict[str, List]) -> List[ExperimentResult]:
        """Run experiments with parameter grid search"""
        configs = []
        
        # Generate all combinations of parameters
        param_keys = list(parameter_grid.keys())
        param_values = list(parameter_grid.values())
        
        def generate_combinations(index: int, current_params: Dict):
            if index == len(param_keys):
                # Create a new config
                config = ExperimentConfig(
                    experiment_type=base_config.experiment_type,
                    neuron_count=base_config.neuron_count,
                    simulation_steps=base_config.simulation_steps,
                    timestep=base_config.timestep,
                    random_seed=base_config.random_seed,
                    output_path=base_config.output_path,
                    config_file=base_config.config_file,
                    checkpoint_path=base_config.checkpoint_path,
                    enable_checkpoint=base_config.enable_checkpoint,
                    enable_replay=base_config.enable_replay,
                    enable_development=base_config.enable_development,
                    custom_settings=base_config.custom_settings.copy()
                )
                
                # Apply parameters
                for key, value in current_params.items():
                    config.custom_settings[key] = value
                
                configs.append(config)
                return
            
            key = param_keys[index]
            for value in param_values[index]:
                current_params[key] = value
                generate_combinations(index + 1, current_params)
                del current_params[key]
        
        generate_combinations(0, {})
        
        return self.run_batch_experiments(configs)
    
    def save_result(self, result: ExperimentResult):
        """Save experiment result to file"""
        results_dir = Path("results")
        results_dir.mkdir(exist_ok=True)
        
        result_file = results_dir / f"{result.experiment_type}_result_{int(result.timestamp)}.json"
        
        with open(result_file, 'w') as f:
            json.dump(result.to_dict(), f, indent=2)

def main():
    """Main entry point for NLM Python interface"""
    parser = argparse.ArgumentParser(description="NLM (Neural Learning Machine) Python Interface")
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Run experiment command
    run_parser = subparsers.add_parser("run", help="Run an NLM experiment")
    run_parser.add_argument("--experiment-type", default="demo", help="Type of experiment to run")
    run_parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    run_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    run_parser.add_argument("--timestep", type=float, default=0.001, help="Simulation timestep")
    run_parser.add_argument("--seed", type=int, default=42, help="Random seed")
    run_parser.add_argument("--config", default="configs/default.cfg", help="Configuration file")
    run_parser.add_argument("--output", default="./results", help="Output directory")
    run_parser.add_argument("--checkpoint", help="Checkpoint file path")
    run_parser.add_argument("--enable-replay", action="store_true", help="Enable memory replay")
    run_parser.add_argument("--enable-development", action="store_true", help="Enable developmental stages")
    
    # List experiments command
    subparsers.add_parser("list-experiments", help="List available experiments")
    
    # Save config command
    save_config_parser = subparsers.add_parser("save-config", help="Save experiment configuration")
    save_config_parser.add_argument("name", help="Configuration name")
    save_config_parser.add_argument("--file", help="Configuration file path")
    
    # Load config command
    load_config_parser = subparsers.add_parser("load-config", help="Load experiment configuration")
    load_config_parser.add_argument("name", help="Configuration name")
    
    # Analyze results command
    analyze_parser = subparsers.add_parser("analyze", help="Analyze experiment results")
    analyze_parser.add_argument("--results-dir", default="./results", help="Results directory")
    
    args = parser.parse_args()
    
    # Handle commands
    if args.command == "run":
        # Create configuration
        config = ExperimentConfig(
            experiment_type=args.experiment_type,
            neuron_count=args.neurons,
            simulation_steps=args.steps,
            timestep=args.timestep,
            random_seed=args.seed,
            config_file=args.config,
            output_path=args.output,
            checkpoint_path=args.checkpoint,
            enable_checkpoint=args.checkpoint is not None,
            enable_replay=args.enable_replay,
            enable_development=args.enable_development
        )
        
        # Run experiment
        runner = NLMExperimentRunner()
        result = runner.run_experiment(config)
        
        # Print results
        print(f"Experiment '{args.experiment_type}' completed:")
        print(f"  Success: {result.success}")
        print(f"  Duration: {result.execution_time:.2f} seconds")
        print(f"  Metrics: {len(result.metrics)}")
        
        if result.error_message:
            print(f"  Error: {result.error_message}")
        
    elif args.command == "list-experiments":
        print("Available NLM experiments:")
        experiments = ["demo", "phase3", "phase4", "phase5", "phase6"]
        for exp in experiments:
            print(f"  {exp}")
    
    elif args.command == "save-config":
        config_manager = NLMConfigurationManager()
        config_manager.save_config(ExperimentConfig(), args.name)
        print(f"Configuration saved as '{args.name}'")
    
    elif args.command == "load-config":
        config_manager = NLMConfigurationManager()
        try:
            config = config_manager.load_config(args.name)
            print(f"Configuration '{args.name}' loaded successfully")
        except Exception as e:
            print(f"Error loading configuration: {e}")
    
    elif args.command == "analyze":
        analyzer = NLMResultAnalyzer()
        results = []
        
        # Load results
        results_dir = Path(args.results_dir)
        if results_dir.exists():
            for result_file in results_dir.glob("*.json"):
                try:
                    with open(result_file, 'r') as f:
                        result_data = json.load(f)
                    
                    # Recreate result object (simplified)
                    result = ExperimentResult(result_data.get("experiment_type", "unknown"))
                    result.metrics = result_data.get("metrics", {})
                    result.success = result_data.get("success", False)
                    result.execution_time = result_data.get("execution_time", 0.0)
                    
                    results.append(result)
                except Exception as e:
                    print(f"Error loading {result_file}: {e}")
        
        # Analyze results
        analysis = analyzer.analyze_results(results[0]) if results else {}
        comparison = analyzer.compare_experiments(results) if results else {}
        
        print("Analysis Results:")
        print(json.dumps({"analysis": analysis, "comparison": comparison}, indent=2))

if __name__ == "__main__":
    main()
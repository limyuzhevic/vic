#!/usr/bin/env python3
"""
NLM Command Line Interface - Advanced Interface for NLM Experiments

This script provides a comprehensive command-line interface for NLM experiments,
offering features for beginners and power users including simulation control,
experiment management, progress tracking, and advanced scripting capabilities.
"""

import argparse
import subprocess
import sys
import os
import json
import time
import yaml
import pandas as pd
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Any, Optional, Union
import argparse
from tabulate import tabulate

# Add project root to path
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

# Global configuration
CONFIG_DIR = Path("configs")
EXPERIMENTS_DIR = Path("experiments")
CHECKPOINTS_DIR = Path("checkpoints")
LOGS_DIR = Path("logs")
RESULTS_DIR = Path("results")

# Create directories
for directory in [CONFIG_DIR, EXPERIMENTS_DIR, CHECKPOINTS_DIR, LOGS_DIR, RESULTS_DIR]:
    directory.mkdir(exist_ok=True)

class NLMCLI:
    """Main NLM Command Line Interface class"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.build_dir = self.project_root / "build"
        self.config_dir = self.project_root / "configs"
        self.experiments_dir = self.project_root / "experiments"
        self.checkpoints_dir = self.project_root / "checkpoints"
        self.results_dir = self.project_root / "results"
        self.logs_dir = self.project_root / "logs"
        
        # Initialize NLM Python bindings if available
        self.nlm_available = self._check_nlm_availability()
        
    def _check_nlm_availability(self) -> bool:
        """Check if NLM Python bindings are available"""
        try:
            import pynlm
            return True
        except ImportError:
            return False
    
    def build_nlm(self, release: bool = True) -> bool:
        """Build NLM from source"""
        print("Building NLM...")
        try:
            # Create build directory
            self.build_dir.mkdir(exist_ok=True)
            
            # Configure CMake
            cmake_cmd = ["cmake", "..", f"-DCMAKE_BUILD_TYPE={'Release' if release else 'Debug'}"]
            result = subprocess.run(cmake_cmd, cwd=self.build_dir, capture_output=True, text=True, check=True)
            
            # Build with parallel jobs
            make_cmd = ["make", "-j4"]
            result = subprocess.run(make_cmd, cwd=self.build_dir, capture_output=True, text=True, check=True)
            
            print("✓ Build successful")
            return True
        except subprocess.CalledProcessError as e:
            print(f"✗ Build failed: {e}")
            if e.stderr:
                print(f"Error details: {e.stderr}")
            return False
    
    def run_simulation(self, **kwargs) -> Dict:
        """Run a neural simulation"""
        print(f"Running simulation with parameters: {kwargs}")
        
        if not self.nlm_available:
            print("✗ NLM Python bindings not available")
            return {"success": False, "error": "NLM Python bindings not available"}
        
        try:
            import pynlm
            
            # Create brain configuration
            config = pynlm.createDefaultConfig()
            
            # Update configuration with provided parameters
            for key, value in kwargs.items():
                if hasattr(config, key):
                    setattr(config, key, value)
            
            # Create brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            start_time = time.time()
            for step in range(kwargs.get("steps", 1000)):
                brain.step(step)
                
                # Log progress
                if kwargs.get("verbose", False) and step % 100 == 0:
                    print(f"Step {step}/{kwargs.get('steps', 1000)}")
            
            end_time = time.time()
            
            result = {
                "success": True,
                "steps_completed": kwargs.get("steps", 1000),
                "total_neurons": brain.getTotalNeuronCount(),
                "firing_neurons": brain.getFiringNeuronCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "average_firing_rate": brain.getAverageFiringRate(),
                "development_stage": brain.getDevelopmentalStage(),
                "simulation_time": end_time - start_time
            }
            
            print(f"✓ Simulation completed successfully")
            print(f"  - Total neurons: {result['total_neurons']}")
            print(f"  - Firing neurons: {result['firing_neurons']}")
            print(f"  - Total spikes: {result['total_spikes']}")
            print(f"  - Average firing rate: {result['average_firing_rate']:.2f}")
            print(f"  - Development stage: {result['development_stage']}")
            print(f"  - Simulation time: {result['simulation_time']:.2f} seconds")
            
            return result
            
        except Exception as e:
            print(f"✗ Simulation failed: {e}")
            return {"success": False, "error": str(e)}
    
    def create_brain(self, **kwargs) -> Union[Path, Dict]:
        """Create a new brain configuration"""
        print(f"Creating brain configuration: {kwargs}")
        
        config_name = kwargs.get("name", f"brain_{int(time.time())}")
        
        # Default configuration
        config = {
            "name": config_name,
            "created": datetime.now().isoformat(),
            "description": kwargs.get("description", "Default NLM configuration"),
            "parameters": {
                "neuron_count": kwargs.get("neurons", 500),
                "region_count": kwargs.get("regions", 2),
                "connection_probability": kwargs.get("connection_probability", 0.1),
                "stdp_ltp_weight": kwargs.get("stdp_ltp_weight", 0.02),
                "stdp_ltd_weight": kwargs.get("stdp_ltd_weight", 0.015),
                "simulation_timestep": kwargs.get("timestep", 0.001),
                "random_seed": kwargs.get("random_seed", int(time.time())),
                "verbose": kwargs.get("verbose", False),
                "enable_developmental": kwargs.get("development", True),
                "enable_structural_plasticity": kwargs.get("structural_plasticity", False),
                "enable_reward_modulation": kwargs.get("reward_modulation", False),
                "enable_curiosity": kwargs.get("curiosity", False)
            }
        }
        
        # Save configuration
        config_path = self.config_dir / f"{config_name}.json"
        with open(config_path, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"✓ Brain configuration created: {config_path}")
        return config_path
    
    def load_checkpoint(self, checkpoint_path: str, target_path: str = None) -> bool:
        """Load a saved brain checkpoint"""
        print(f"Loading checkpoint: {checkpoint_path}")
        
        if not self.nlm_available:
            print("✗ NLM Python bindings not available")
            return False
        
        try:
            import pynlm
            
            # Ensure target directory exists
            if target_path:
                Path(target_path).mkdir(exist_ok=True)
            
            # Load checkpoint using NLM API
            brain = pynlm.loadBrain(checkpoint_path)
            
            print(f"✓ Checkpoint loaded successfully")
            return True
            
        except Exception as e:
            print(f"✗ Failed to load checkpoint: {e}")
            return False
    
    def save_checkpoint(self, brain, checkpoint_path: str) -> bool:
        """Save brain state as checkpoint"""
        print(f"Saving checkpoint: {checkpoint_path}")
        
        if not self.nlm_available:
            print("✗ NLM Python bindings not available")
            return False
        
        try:
            import pynlm
            
            # Save checkpoint using NLM API
            pynlm.saveBrain(brain, checkpoint_path)
            
            print(f"✓ Checkpoint saved successfully")
            return True
            
        except Exception as e:
            print(f"✗ Failed to save checkpoint: {e}")
            return False
    
    def create_experiment(self, **kwargs) -> Dict:
        """Create and run an experiment"""
        print(f"Creating experiment: {kwargs}")
        
        experiment_name = kwargs.get("name", f"exp_{int(time.time())}")
        
        # Create experiment configuration
        experiment = {
            "name": experiment_name,
            "created": datetime.now().isoformat(),
            "description": kwargs.get("description", ""),
            "type": kwargs.get("type", "standard"),
            "parameters": kwargs.get("parameters", {}),
            "status": "pending",
            "results": {},
            "log_file": self.logs_dir / f"{experiment_name}.log"
        }
        
        # Save experiment configuration
        experiment_path = self.experiments_dir / f"{experiment_name}.json"
        with open(experiment_path, 'w') as f:
            json.dump(experiment, f, indent=2)
        
        print(f"✓ Experiment created: {experiment_path}")
        return experiment
    
    def run_experiments_batch(self, configs: List[Dict]) -> List[Dict]:
        """Run multiple experiments in batch"""
        print(f"Running batch of {len(configs)} experiments...")
        
        results = []
        
        for i, config in enumerate(configs):
            print(f"\n--- Experiment {i+1}/{len(configs)} ---")
            print(f"Name: {config.get('name', 'Unnamed')}")
            print(f"Type: {config.get('type', 'standard')}")
            
            # Run simulation
            result = self.run_simulation(**config)
            
            # Update experiment status
            experiment = {
                "name": config.get('name', f"exp_{i+1}"),
                "status": "completed" if result.get("success") else "failed",
                "parameters": config,
                "results": result,
                "completed_at": datetime.now().isoformat()
            }
            
            results.append(experiment)
            
            if not result.get("success"):
                print(f"✗ Experiment {i+1} failed, stopping batch")
                break
        
        print(f"\n✓ Batch completed: {len(results)}/{len(configs)} experiments successful")
        return results
    
    def compare_experiments(self, experiment_names: List[str]) -> Dict:
        """Compare results from multiple experiments"""
        print(f"Comparing experiments: {experiment_names}")
        
        experiments_data = []
        
        for name in experiment_names:
            experiment_path = self.experiments_dir / f"{name}.json"
            
            if not experiment_path.exists():
                print(f"✗ Experiment not found: {name}")
                continue
            
            with open(experiment_path, 'r') as f:
                experiment = json.load(f)
                experiments_data.append(experiment)
        
        if not experiments_data:
            print("✗ No valid experiments found for comparison")
            return {}
        
        # Create comparison table
        comparison = {
            "experiments": [exp["name"] for exp in experiments_data],
            "parameters": {
                key: [exp.get("parameters", {}).get(key) for exp in experiments_data]
                for key in set().union(*(exp.get("parameters", {}) for exp in experiments_data))
            },
            "results": {
                key: [exp.get("results", {}).get(key) for exp in experiments_data]
                for key in set().union(*(exp.get("results", {}) for exp in experiments_data))
            }
        }
        
        print("\nExperiment Comparison:")
        print("=" * 80)
        
        # Create results table
        results_table = []
        for exp in experiments_data:
            results = exp.get("results", {})
            if results:
                row = {
                    "Experiment": exp["name"],
                    "Status": exp.get("status", "unknown"),
                    "Total Neurons": results.get("total_neurons", "N/A"),
                    "Firing Neurons": results.get("firing_neurons", "N/A"),
                    "Total Spikes": results.get("total_spikes", "N/A"),
                    "Avg Firing Rate": results.get("average_firing_rate", "N/A"),
                    "Simulation Time": results.get("simulation_time", "N/A"),
                    "Completion Time": exp.get("completed_at", "N/A")
                }
                results_table.append(row)
        
        if results_table:
            print(tabulate(results_table, headers="keys", tablefmt="grid"))
        
        # Save comparison results
        comparison_path = self.results_dir / f"comparison_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(comparison_path, 'w') as f:
            json.dump(comparison, f, indent=2)
        
        print(f"\n✓ Comparison saved to: {comparison_path}")
        return comparison

def setup_argument_parser() -> argparse.ArgumentParser:
    """Set up the main argument parser"""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface - Advanced Interface for NLM Experiments",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
The NLM CLI provides comprehensive tools for neural learning machine experiments.

Examples:
  nlm_cli --build
  nlm_cli --run-simulation --neurons 1000 --steps 5000
  nlm_cli --create-brain --name mybrain --neurons 750
  nlm_cli --create-experiment --name myexp --type phase6
  nlm_cli --batch-run --config experiments/configs.json
  nlm_cli --compare-experiments exp1 exp2 exp3

For more detailed help, use: nlm_cli --help
"""
    )
    
    # Main action
    parser.add_argument(
        "--build",
        action="store_true",
        help="Build NLM from source"
    )
    
    # Simulation commands
    parser.add_argument(
        "--run-simulation",
        action="store_true",
        help="Run a neural simulation"
    )
    
    # Brain management commands
    parser.add_argument(
        "--create-brain",
        action="store_true",
        help="Create a new brain configuration"
    )
    
    parser.add_argument(
        "--load-checkpoint",
        action="store_true",
        help="Load a saved brain checkpoint"
    )
    
    parser.add_argument(
        "--checkpoint-path",
        type=str,
        default="",
        help="Path to checkpoint file"
    )
    
    # Experiment management commands
    parser.add_argument(
        "--create-experiment",
        action="store_true",
        help="Create and run an experiment"
    )
    
    parser.add_argument(
        "--batch-run",
        action="store_true",
        help="Run multiple experiments in batch mode"
    )
    
    parser.add_argument(
        "--compare-experiments",
        nargs="+",
        help="Compare results from multiple experiments"
    )
    
    # Configuration management
    parser.add_argument(
        "--create-config",
        action="store_true",
        help="Create configuration file with preset values"
    )
    
    parser.add_argument(
        "--config-name",
        type=str,
        default="",
        help="Name for the configuration file"
    )
    
    parser.add_argument(
        "--template",
        type=str,
        default="default",
        choices=["default", "debug", "production", "high_performance"],
        help="Template for configuration (default: debug)"
    )
    
    # Common simulation parameters
    parser.add_argument(
        "--neurons",
        type=int,
        default=500,
        help="Number of neurons in the brain (default: 500)"
    )
    
    parser.add_argument(
        "--regions",
        type=int,
        default=2,
        help="Number of neural regions (default: 2)"
    )
    
    parser.add_argument(
        "--steps",
        type=int,
        default=1000,
        help="Number of simulation steps (default: 1000)"
    )
    
    parser.add_argument(
        "--timestep",
        type=float,
        default=0.001,
        help="Simulation timestep (default: 0.001)"
    )
    
    parser.add_argument(
        "--connection-probability",
        type=float,
        default=0.1,
        help="Probability of connections between neurons (default: 0.1)"
    )
    
    # Advanced parameters
    parser.add_argument(
        "--stdp-ltp-weight",
        type=float,
        default=0.02,
        help="STDP LTP weight (default: 0.02)"
    )
    
    parser.add_argument(
        "--stdp-ltd-weight",
        type=float,
        default=0.015,
        help="STDP LTD weight (default: 0.015)"
    )
    
    parser.add_argument(
        "--random-seed",
        type=int,
        default=int(time.time()),
        help="Random seed for reproducibility (default: current timestamp)"
    )
    
    # Feature flags
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--development",
        action="store_true",
        default=True,
        help="Enable brain development (default: True)"
    )
    
    parser.add_argument(
        "--structural-plasticity",
        action="store_true",
        help="Enable structural plasticity"
    )
    
    parser.add_argument(
        "--reward-modulation",
        action="store_true",
        help="Enable reward modulation"
    )
    
    parser.add_argument(
        "--curiosity",
        action="store_true",
        help="Enable curiosity-driven exploration"
    )
    
    # Output and logging
    parser.add_argument(
        "--log-file",
        type=str,
        default="",
        help="Log file path"
    )
    
    parser.add_argument(
        "--output-dir",
        type=str,
        default="results",
        help="Output directory for results (default: results)"
    )
    
    # Experiment configuration file
    parser.add_argument(
        "--config-file",
        type=str,
        default="",
        help="Path to experiment configuration file"
    )
    
    # Batch configuration
    parser.add_argument(
        "--batch-config",
        type=str,
        default="",
        help="Path to batch configuration file"
    )
    
    return parser

def main():
    """Main entry point for NLM CLI"""
    parser = setup_argument_parser()
    args = parser.parse_args()
    
    # Initialize CLI
    cli = NLMCLI()
    
    try:
        # Handle commands
        if args.build:
            cli.build_nlm()
        
        elif args.run_simulation:
            # Get simulation parameters
            params = {
                "neurons": args.neurons,
                "steps": args.steps,
                "timestep": args.timestep,
                "connection_probability": args.connection_probability,
                "stdp_ltp_weight": args.stdp_ltp_weight,
                "stdp_ltd_weight": args.stdp_ltd_weight,
                "random_seed": args.random_seed,
                "verbose": args.verbose,
                "development": args.development,
                "structural_plasticity": args.structural_plasticity,
                "reward_modulation": args.reward_modulation,
                "curiosity": args.curiosity
            }
            
            result = cli.run_simulation(**params)
            
            # Save results
            if result.get("success"):
                result_path = cli.results_dir / f"simulation_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
                with open(result_path, 'w') as f:
                    json.dump(result, f, indent=2)
                print(f"✓ Results saved to: {result_path}")
        
        elif args.create_brain:
            config_name = args.config_name or f"brain_{int(time.time())}"
            
            params = {
                "name": config_name,
                "neurons": args.neurons,
                "regions": args.regions,
                "description": f"Brain configuration created at {datetime.now().isoformat()}"
            }
            
            cli.create_brain(**params)
        
        elif args.load_checkpoint:
            if not args.checkpoint_path:
                print("✗ Checkpoint path required for loading")
                return
            
            target_path = args.output_dir
            success = cli.load_checkpoint(args.checkpoint_path, target_path)
            
            if success:
                print("✓ Checkpoint loaded successfully")
            else:
                print("✗ Failed to load checkpoint")
        
        elif args.create_experiment:
            config_name = args.config_name or f"exp_{int(time.time())}"
            
            params = {
                "name": config_name,
                "parameters": {
                    "neurons": args.neurons,
                    "steps": args.steps,
                    "timestep": args.timestep,
                    "connection_probability": args.connection_probability,
                    "stdp_ltp_weight": args.stdp_ltp_weight,
                    "stdp_ltd_weight": args.stdp_ltd_weight,
                    "random_seed": args.random_seed,
                    "verbose": args.verbose,
                    "development": args.development,
                    "structural_plasticity": args.structural_plasticity,
                    "reward_modulation": args.reward_modulation,
                    "curiosity": args.curiosity
                }
            }
            
            experiment = cli.create_experiment(**params)
            
            # Run experiment if parameters provided
            if args.steps > 0:
                print(f"\nRunning experiment: {experiment['name']}")
                experiment.update(cli.run_simulation(**params))
        
        elif args.batch_run:
            if not args.batch_config:
                print("✗ Batch configuration file required for batch run")
                return
            
            # Load batch configuration
            with open(args.batch_config, 'r') as f:
                batch_config = json.load(f)
            
            results = cli.run_experiments_batch(batch_config.get("experiments", []))
            
            # Save batch results
            batch_result_path = cli.results_dir / f"batch_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
            with open(batch_result_path, 'w') as f:
                json.dump(results, f, indent=2)
            
            print(f"✓ Batch results saved to: {batch_result_path}")
        
        elif args.compare_experiments:
            comparison = cli.compare_experiments(args.compare_experiments)
            
            if not comparison:
                print("✗ No experiments found for comparison")
        
        elif args.create_config:
            config_name = args.config_name or f"config_{int(time.time())}"
            cli.create_config_file(config_name, args.template)
        
        else:
            parser.print_help()
            print("\n" + "="*60)
            print("No action specified. Use --help for available commands.")
            print("="*60)
    
    except KeyboardInterrupt:
        print("\nOperation interrupted by user")
    
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0
if __name__ == "__main__":
    exit(main())
#!/usr/bin/env python3
"""
Advanced NLM Experiment Controller

This script provides advanced control over NLM experiments, including parameter tuning,
multiple experiment configurations, and automated testing capabilities.
"""

import argparse
import json
import yaml
import time
import subprocess
import sys
import os
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Any, Optional

# Add project root to path
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

class NLMExperimentController:
    """Main controller for NLM experiments"""
    
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.config_dir = self.project_root / "configs"
        self.config_dir.mkdir(exist_ok=True)
        
    def load_json_config(self, filepath: str) -> Dict:
        """Load JSON configuration file"""
        try:
            with open(filepath, 'r') as f:
                return json.load(f)
        except FileNotFoundError:
            print(f"✗ Config file not found: {filepath}")
            return {}
        except json.JSONDecodeError as e:
            print(f"✗ Invalid JSON in config file: {e}")
            return {}
    
    def save_json_config(self, filepath: str, config: Dict):
        """Save configuration as JSON"""
        try:
            with open(filepath, 'w') as f:
                json.dump(config, f, indent=2)
            print(f"✓ Configuration saved to {filepath}")
        except Exception as e:
            print(f"✗ Failed to save config: {e}")
    
    def create_advanced_config(self, name: str, params: Dict):
        """Create advanced configuration with validation"""
        config = {
            "name": name,
            "created": datetime.now().isoformat(),
            "parameters": params,
            "validation": {}
        }
        
        # Validate parameters
        self._validate_config(params, config["validation"])
        
        # Save config
        config_path = self.config_dir / f"{name}.json"
        self.save_json_config(str(config_path), config)
        
        return config
    
    def _validate_config(self, params: Dict, validation: Dict):
        """Validate configuration parameters"""
        validation["is_valid"] = True
        validation["errors"] = []
        
        # Check required parameters
        required = ["neuron_count", "steps", "random_seed"]
        for key in required:
            if key not in params:
                validation["errors"].append(f"Missing required parameter: {key}")
                validation["is_valid"] = False
        
        # Check parameter ranges
        if "neuron_count" in params and params["neuron_count"] < 10:
            validation["errors"].append("neuron_count must be at least 10")
            validation["is_valid"] = False
        
        if "steps" in params and params["steps"] < 100:
            validation["errors"].append("steps must be at least 100")
            validation["is_valid"] = False
        
        if "connection_probability" in params and (params["connection_probability"] < 0 or params["connection_probability"] > 1):
            validation["errors"].append("connection_probability must be between 0 and 1")
            validation["is_valid"] = False
        
        # Check for experimental parameters
        exp_params = ["stdp_ltp_weight", "stdp_ltd_weight", "synaptogenesis_rate", 
                     "pruning_rate", "curiosity_level", "novelty_threshold"]
        for param in exp_params:
            if param in params:
                validation["experimental_params"] = validation.get("experimental_params", [])
                validation["experimental_params"].append(param)
    
    def run_parameter_sweep(self, base_config: Dict, parameter_ranges: Dict, 
                           description: str = ""):
        """Run parameter sweep across multiple configurations"""
        print(f"\nStarting parameter sweep: {description}")
        print(f"Base config: {base_config}")
        
        # Generate all combinations
        sweep_configs = self._generate_sweep_config(base_config, parameter_ranges)
        
        results = []
        for i, config in enumerate(sweep_configs):
            print(f"\n--- Configuration {i+1}/{len(sweep_configs)} ---")
            
            # Save config for this run
            config_name = f"sweep_{description}_{i+1}"
            self.create_advanced_config(config_name, config["parameters"])
            
            # Run experiment (placeholder for actual run)
            result = self._run_experiment_with_config(config["parameters"])
            result["config_name"] = config_name
            result["parameters_used"] = config["parameters"]
            
            results.append(result)
            
            # Small delay between runs
            time.sleep(1)
        
        # Summarize results
        self._summarize_sweep_results(results)
        return results
    
    def _generate_sweep_config(self, base_config: Dict, parameter_ranges: Dict):
        """Generate all combinations of parameter sweep"""
        from itertools import product
        
        sweep_configs = []
        
        # Get all parameter names and their values
        param_names = list(parameter_ranges.keys())
        param_values = list(parameter_ranges.values())
        
        # Generate Cartesian product
        for combination in product(*param_values):
            config = base_config.copy()
            
            # Update config with current combination
            for name, value in zip(param_names, combination):
                config[name] = value
            
            sweep_configs.append({
                "name": f"{name}_{value}",
                "parameters": config
            })
        
        return sweep_configs
    
    def _run_experiment_with_config(self, params: Dict) -> Dict:
        """Run experiment with given parameters (placeholder implementation)"""
        print(f"Running experiment with {params.get('neuron_count', 1000)} neurons")
        
        # In a real implementation, this would:
        # 1. Create the NLM brain with these parameters
        # 2. Run the simulation
        # 3. Collect results
        
        # For now, return a mock result
        return {
            "status": "completed",
            "total_reward": 0.0,
            "avg_firing_rate": 0.0,
            "episodes_stored": 0,
            "time_taken": time.time() - time.time(),
            "neurons_tested": params.get("neuron_count", 1000)
        }
    
    def _summarize_sweep_results(self, results: List[Dict]):
        """Summarize parameter sweep results"""
        print("\n" + "="*60)
        print("PARAMETER SWEEP SUMMARY")
        print("="*60)
        
        if not results:
            print("No results to summarize")
            return
        
        # Calculate statistics
        rewards = [r["total_reward"] for r in results if r["status"] == "completed"]
        firing_rates = [r["avg_firing_rate"] for r in results if r["status"] == "completed"]
        
        if rewards:
            print(f"\nTotal configurations tested: {len(results)}")
            print(f"Successful runs: {len(rewards)}")
            print(f"Average reward: {sum(rewards)/len(rewards):.4f}")
            print(f"Max reward: {max(rewards):.4f}")
            print(f"Min reward: {min(rewards):.4f}")
            
            if firing_rates:
                print(f"Average firing rate: {sum(firing_rates)/len(firing_rates):.4f}")
                print(f"Max firing rate: {max(firing_rates):.4f}")
                print(f"Min firing rate: {min(firing_rates):.4f}")
        
        # Find best configuration
        if results:
            best_result = max(results, key=lambda x: x["total_reward"])
            print(f"\nBest configuration:")
            print(f"  Config name: {best_result['config_name']}")
            print(f"  Neurons: {best_result['parameters_used'].get('neuron_count', 'N/A')}")
            print(f"  Steps: {best_result['parameters_used'].get('steps', 'N/A')}")
            print(f"  Reward: {best_result['total_reward']:.4f}")
    
    def create_research_protocol(self, protocol_name: str, experiments: List[Dict]):
        """Create a research protocol with multiple experiments"""
        protocol = {
            "name": protocol_name,
            "created": datetime.now().isoformat(),
            "description": "Research protocol for NLM experiments",
            "experiments": experiments,
            "metadata": {}
        }
        
        # Save protocol
        protocol_path = self.config_dir / f"{protocol_name}.json"
        self.save_json_config(str(protocol_path), protocol)
        
        print(f"✓ Research protocol '{protocol_name}' created")
        return protocol
    
    def export_experiment_results(self, results: List[Dict], output_format: str = "json"):
        """Export experiment results in various formats"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        
        if output_format == "json":
            output_path = self.project_root / f"results_{timestamp}.json"
            with open(output_path, 'w') as f:
                json.dump({
                    "timestamp": timestamp,
                    "total_experiments": len(results),
                    "results": results
                }, f, indent=2)
            
        elif output_format == "csv":
            output_path = self.project_root / f"results_{timestamp}.csv"
            # Convert to CSV format
            import csv
            with open(output_path, 'w', newline='') as f:
                if results:
                    writer = csv.DictWriter(f, fieldnames=results[0].keys())
                    writer.writeheader()
                    writer.writerows(results)
        
        print(f"✓ Results exported to {output_path}")
        return output_path


def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description="Advanced NLM Experiment Controller",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_controller.py create-config my_experiment --neuron_count=1000 --steps=2000
  nlm_controller.py run-sweep --base-config=base.json --param-ranges=param_ranges.json
  nlm_controller.py create-protocol my_research
  nlm_controller.py export-results --file=results.json
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Create config command
    create_parser = subparsers.add_parser("create-config", help="Create configuration")
    create_parser.add_argument("name", help="Configuration name")
    create_parser.add_argument("--neuron_count", type=int, default=1000, help="Number of neurons")
    create_parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    create_parser.add_argument("--random_seed", type=int, default=42, help="Random seed")
    create_parser.add_argument("--connection_probability", type=float, default=0.1, help="Connection probability (0-1)")
    create_parser.add_argument("--stdp_ltp_weight", type=float, default=0.02, help="STDP LTP weight")
    create_parser.add_argument("--stdp_ltd_weight", type=float, default=0.015, help="STDP LTD weight")
    create_parser.add_argument("--synaptogenesis_rate", type=float, default=0.0001, help="Synaptogenesis rate")
    create_parser.add_argument("--curiosity_level", type=float, default=0.5, help="Curiosity level")
    create_parser.add_argument("--novelty_threshold", type=float, default=0.3, help="Novelty threshold")
    
    # Run sweep command
    sweep_parser = subparsers.add_parser("run-sweep", help="Run parameter sweep")
    sweep_parser.add_argument("--base-config", required=True, help="Base configuration JSON file")
    sweep_parser.add_argument("--param-ranges", required=True, help="Parameter ranges JSON file")
    sweep_parser.add_argument("--description", default="", help="Description for sweep")
    
    # Create protocol command
    protocol_parser = subparsers.add_parser("create-protocol", help="Create research protocol")
    protocol_parser.add_argument("name", help="Protocol name")
    protocol_parser.add_argument("--description", default="", help="Protocol description")
    
    # Export results command
    export_parser = subparsers.add_parser("export-results", help="Export experiment results")
    export_parser.add_argument("--file", required=True, help="Results file to export")
    export_parser.add_argument("--format", choices=["json", "csv"], default="json", help="Output format")
    
    # Parse arguments
    args = parser.parse_args()
    
    # Initialize controller
    controller = NLMExperimentController()
    
    # Execute commands
    if args.command == "create-config":
        params = {
            "neuron_count": args.neuron_count,
            "steps": args.steps,
            "random_seed": args.random_seed,
            "connection_probability": args.connection_probability,
            "stdp_ltp_weight": args.stdp_ltp_weight,
            "stdp_ltd_weight": args.stdp_ltd_weight,
            "synaptogenesis_rate": args.synaptogenesis_rate,
            "curiosity_level": args.curiosity_level,
            "novelty_threshold": args.novelty_threshold
        }
        controller.create_advanced_config(args.name, params)
    
    elif args.command == "run-sweep":
        base_config = controller.load_json_config(args.base_config)
        param_ranges = controller.load_json_config(args.param_ranges)
        
        if base_config and param_ranges:
            controller.run_parameter_sweep(base_config, param_ranges, args.description)
        else:
            print("✗ Failed to load configuration files")
    
    elif args.command == "create-protocol":
        # For now, create an empty protocol
        controller.create_research_protocol(args.name, [])
    
    elif args.command == "export-results":
        try:
            results = controller.load_json_config(args.file)
            if "results" in results:
                results = results["results"]
            controller.export_experiment_results(results, args.format)
            
        except Exception as e:
            print(f"✗ Failed to export results: {e}")
    
    else:
        parser.print_help()

if __name__ == "__main__":
    main()

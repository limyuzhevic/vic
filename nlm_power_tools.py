#!/usr/bin/env python3
"""
NLM Power User Tools - Advanced Configuration and Control Utilities

This script provides advanced utilities for power users of the NLM neural simulation framework.
It includes configuration management, brain control tools, performance monitoring,
and advanced experimentation capabilities.
"""

import json
import yaml
import argparse
import sys
import os
from pathlib import Path
from typing import Dict, Any, List, Optional
import subprocess
import time
from datetime import datetime

class NLMConfigManager:
    """Advanced configuration management tools for NLM power users."""
    
    def __init__(self, config_dir: str = "configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
    
    def create_template(self, name: str, description: str = "", defaults: Dict[str, Any] = None):
        """Create a configuration template with defaults."""
        template = {
            "name": name,
            "description": description,
            "defaults": defaults or {},
            "created": datetime.now().isoformat(),
            "modified": datetime.now().isoformat(),
            "version": "1.0"
        }
        
        template_file = self.config_dir / f"{name}_template.json"
        with open(template_file, 'w') as f:
            json.dump(template, f, indent=2)
        
        print(f"Created template: {template_file}")
    
    def list_templates(self):
        """List all available configuration templates."""
        print("Available NLM Configuration Templates:")
        print("=" * 50)
        
        for template_file in self.config_dir.glob("*_template.json"):
            with open(template_file, 'r') as f:
                template = json.load(f)
            
            print(f"Name: {template['name']}")
            print(f"Description: {template['description']}")
            print(f"Defaults: {json.dumps(template['defaults'], indent=2)}")
            print(f"Created: {template['created']}")
            print(f"Version: {template['version']}")
            print("-" * 50)
    
    def create_preset(self, name: str, description: str = "", parameters: Dict[str, Any] = None):
        """Create a configuration preset with specific parameters."""
        preset = {
            "name": name,
            "description": description,
            "parameters": parameters or {},
            "created": datetime.now().isoformat(),
            "modified": datetime.now().isoformat(),
            "applied": False
        }
        
        preset_file = self.config_dir / f"{name}_preset.yaml"
        with open(preset_file, 'w') as f:
            yaml.dump(preset, f, default_flow_style=False)
        
        print(f"Created preset: {preset_file}")
    
    def list_presets(self):
        """List all available configuration presets."""
        print("Available NLM Configuration Presets:")
        print("=" * 50)
        
        for preset_file in self.config_dir.glob("*_preset.yaml"):
            with open(preset_file, 'r') as f:
                preset = yaml.safe_load(f)
            
            print(f"Name: {preset['name']}")
            print(f"Description: {preset['description']}")
            print(f"Parameters: {json.dumps(preset['parameters'], indent=2)}")
            print(f"Applied: {preset['applied']}")
            print("-" * 50)

class NLMBrainController:
    """Advanced brain control tools for NLM power users."""
    
    def __init__(self, nlm_path: str = "nlm"):
        self.nlm_path = Path(nlm_path)
        if not self.nlm_path.exists():
            print(f"Warning: NLM executable not found at {nlm_path}")
    
    def save_brain_state(self, filepath: str, description: str = ""):
        """Save current brain state to file with advanced metadata."""
        save_command = [
            str(self.nlm_path), "--save", filepath,
            "--description", description,
            "--include-memory", "--include-development",
            "--include-neuromodulation"
        ]
        
        print(f"Saving brain state to {filepath}")
        print(f"Description: {description}")
        
        try:
            subprocess.run(save_command, check=True)
            print("Brain state saved successfully!")
        except subprocess.CalledProcessError as e:
            print(f"Error saving brain state: {e}")
    
    def load_brain_state(self, filepath: str, merge: bool = False):
        """Load brain state from file with merge options."""
        load_command = [
            str(self.nlm_path), "--load", filepath,
            "--merge" if merge else "--replace"
        ]
        
        print(f"Loading brain state from {filepath}")
        print(f"Merge mode: {merge}")
        
        try:
            subprocess.run(load_command, check=True)
            print("Brain state loaded successfully!")
        except subprocess.CalledProcessError as e:
            print(f"Error loading brain state: {e}")
    
    def create_checkpoint(self, description: str = "", backup: bool = True):
        """Create an advanced checkpoint with metadata."""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        checkpoint_file = f"checkpoint_{timestamp}.nlm"
        
        checkpoint_command = [
            str(self.nlm_path), "--checkpoint", checkpoint_file,
            "--description", description,
            "--include-all", "--compress"
        ]
        
        print(f"Creating checkpoint: {checkpoint_file}")
        print(f"Description: {description}")
        print(f"Backup: {backup}")
        
        try:
            subprocess.run(checkpoint_command, check=True)
            if backup:
                print("Creating backup...")
                # Create backup copy
                backup_file = f"backup_{timestamp}.nlm"
                if os.path.exists(checkpoint_file):
                    os.rename(checkpoint_file, backup_file)
                    print(f"Backup created: {backup_file}")
            print("Checkpoint created successfully!")
        except subprocess.CalledProcessError as e:
            print(f"Error creating checkpoint: {e}")
    
    def restore_checkpoint(self, filepath: str, verify: bool = True):
        """Restore from checkpoint with verification option."""
        if verify:
            print(f"Verifying checkpoint integrity...")
            # Verify checkpoint format
            if not filepath.endswith('.nlm'):
                print("Warning: File extension is not .nlm")
        
        restore_command = [
            str(self.nlm_path), "--restore", filepath
        ]
        
        print(f"Restoring from checkpoint: {filepath}")
        
        try:
            subprocess.run(restore_command, check=True)
            print("Checkpoint restored successfully!")
        except subprocess.CalledProcessError as e:
            print(f"Error restoring checkpoint: {e}")
    
    def inject_neuron_pattern(self, pattern_type: str, region_id: int = 1, strength: float = 1.0):
        """Inject specific neuron activity patterns."""
        inject_command = [
            str(self.nlm_path), "--inject-pattern", pattern_type,
            "--region", str(region_id),
            "--strength", str(strength)
        ]
        
        print(f"Injecting {pattern_type} pattern to region {region_id}")
        print(f"Strength: {strength}")
        
        try:
            subprocess.run(inject_command, check=True)
            print("Neuron pattern injected successfully!")
        except subprocess.CalledProcessError as e:
            print(f"Error injecting neuron pattern: {e}")
    
    def run_stdp_experiment(self, pre_neuron: int, post_neuron: int, timing: float = 10.0):
        """Run a specific STDP experiment with precise timing."""
        exp_command = [
            str(self.nlm_path), "--stdp-experiment",
            "--pre", str(pre_neuron),
            "--post", str(post_neuron),
            "--timing", str(timing)
        ]
        
        print(f"Running STDP experiment: pre={pre_neuron}, post={post_neuron}")
        print(f"Timing: {timing}ms")
        
        try:
            subprocess.run(exp_command, check=True)
            print("STDP experiment completed successfully!")
        except subprocess.CalledProcessError as e:
            print(f"Error running STDP experiment: {e}")

class NLMPerformanceMonitor:
    """Performance monitoring and optimization tools for NLM power users."""
    
    def __init__(self):
        self.metrics = {}
    
    def start_monitoring(self, duration: int = 300):
        """Start performance monitoring for specified duration (seconds)."""
        print(f"Starting performance monitoring for {duration} seconds...")
        
        start_time = time.time()
        end_time = start_time + duration
        
        while time.time() < end_time:
            self.collect_metrics()
            time.sleep(10)  # Collect every 10 seconds
        
        self.analyze_performance()
    
    def collect_metrics(self):
        """Collect current performance metrics."""
        import psutil
        import resource
        
        # CPU usage
        cpu_percent = psutil.cpu_percent(interval=0.1)
        
        # Memory usage
        memory = psutil.virtual_memory()
        
        # Process-specific metrics
        process = psutil.Process()
        memory_info = process.memory_info()
        cpu_times = process.cpu_times()
        
        metric = {
            "timestamp": time.time(),
            "cpu_percent": cpu_percent,
            "memory_total": memory.total,
            "memory_used": memory.used,
            "memory_percent": memory.percent,
            "process_memory_rss": memory_info.rss,
            "process_memory_vms": memory_info.vms,
            "cpu_time_user": cpu_times.user,
            "cpu_time_system": cpu_times.system,
            "thread_count": process.num_threads()
        }
        
        self.metrics[len(self.metrics)] = metric
        print(f"Collected metrics: CPU={cpu_percent:.1f}%, Memory={memory.percent:.1f}%")
    
    def analyze_performance(self):
        """Analyze collected performance metrics."""
        if not self.metrics:
            print("No metrics collected for analysis.")
            return
        
        print("\nPerformance Analysis:")
        print("=" * 50)
        
        # CPU analysis
        cpu_values = [m["cpu_percent"] for m in self.metrics.values()]
        avg_cpu = sum(cpu_values) / len(cpu_values)
        max_cpu = max(cpu_values)
        
        print(f"CPU Usage: Avg={avg_cpu:.1f}%, Max={max_cpu:.1f}%")
        
        # Memory analysis
        memory_values = [m["memory_percent"] for m in self.metrics.values()]
        avg_memory = sum(memory_values) / len(memory_values)
        
        print(f"Memory Usage: Avg={avg_memory:.1f}%")
        
        # Bottleneck detection
        if max_cpu > 90:
            print("⚠ WARNING: High CPU usage detected - potential bottleneck")
        if avg_memory > 80:
            print("⚠ WARNING: High memory usage detected - potential bottleneck")
        
        # Optimization recommendations
        if max_cpu > 80:
            print("💡 RECOMMENDATION: Consider reducing thread count or optimizing loops")
        if avg_memory > 70:
            print("💡 RECOMMENDATION: Consider optimizing memory usage or increasing available memory")
        
        # Save analysis to file
        analysis_file = f"performance_analysis_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(analysis_file, 'w') as f:
            json.dump({
                "metrics": self.metrics,
                "summary": {
                    "avg_cpu": avg_cpu,
                    "max_cpu": max_cpu,
                    "avg_memory": avg_memory,
                    "analysis_time": datetime.now().isoformat()
                }
            }, f, indent=2)
        
        print(f"\nPerformance analysis saved to: {analysis_file}")

class NLMExperimentDesigner:
    """Advanced experiment design and parameter sweep tools."""
    
    def __init__(self):
        self.experiments = []
    
    def create_experiment(self, name: str, parameters: Dict[str, Any], description: str = ""):
        """Create a new experiment with specified parameters."""
        experiment = {
            "name": name,
            "parameters": parameters,
            "description": description,
            "created": datetime.now().isoformat(),
            "status": "pending",
            "results": None
        }
        
        self.experiments.append(experiment)
        print(f"Created experiment: {name}")
        print(f"Parameters: {json.dumps(parameters, indent=2)}")
        return experiment
    
    def run_parameter_sweep(self, base_experiment: Dict[str, Any], param_ranges: Dict[str, List[Any]]):
        """Run parameter sweep across specified ranges."""
        print(f"Running parameter sweep for experiment: {base_experiment['name']}")
        print("Parameter ranges:")
        for param, values in param_ranges.items():
            print(f"  {param}: {values}")
        
        sweep_results = []
        total_combinations = 1
        for values in param_ranges.values():
            total_combinations *= len(values)
        
        print(f"Total combinations to test: {total_combinations}")
        
        # This would be implemented with actual NLM execution
        # For now, we'll simulate the process
        for i, param_values in enumerate(zip(*param_ranges.values())):
            experiment = base_experiment.copy()
            for j, param in enumerate(param_ranges.keys()):
                experiment['parameters'][param] = param_values[j]
            
            # Simulate running experiment
            print(f"  Running experiment {i+1}/{total_combinations}: {experiment['parameters']}")
            
            # In a real implementation, this would call the NLM
            result = {
                "status": "completed",
                "metrics": {
                    "spike_count": 100 + i * 10,
                    "learning_rate": 0.01 * (1 + i * 0.1),
                    "stability": 0.8 - (i * 0.01)
                }
            }
            
            experiment['status'] = result['status']
            experiment['results'] = result
            sweep_results.append(experiment)
        
        print(f"Parameter sweep completed. {len(sweep_results)} experiments run.")
        return sweep_results
    
    def save_experiment(self, filename: str):
        """Save all experiments to file."""
        data = {
            "experiments": self.experiments,
            "saved": datetime.now().isoformat()
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"Experiments saved to: {filename}")
    
    def load_experiments(self, filename: str):
        """Load experiments from file."""
        with open(filename, 'r') as f:
            data = json.load(f)
        
        self.experiments = data['experiments']
        print(f"Loaded {len(self.experiments)} experiments from {filename}")

def main():
    """Main entry point for NLM Power User Tools."""
    parser = argparse.ArgumentParser(
        description="NLM Power User Tools - Advanced Configuration and Control Utilities",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_power_tools config --create-template mytemplate --description "My config template"
  nlm_power_tools brain --save mybrain.nlm --description "Current brain state"
  nlm_power_tools monitor --start 300
  nlm_power_tools experiment --create myexp --params '{"neuron_count": 1000}'
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Configuration management commands
    config_parser = subparsers.add_parser('config', help='Configuration management tools')
    config_subparsers = config_parser.add_subparsers(dest='config_command')
    
    template_parser = config_subparsers.add_parser('create-template', help='Create configuration template')
    template_parser.add_argument('name', help='Template name')
    template_parser.add_argument('--description', help='Template description')
    template_parser.add_argument('--defaults', help='JSON string of default parameters')
    
    list_parser = config_subparsers.add_parser('list-templates', help='List all templates')
    
    preset_parser = config_subparsers.add_parser('create-preset', help='Create configuration preset')
    preset_parser.add_argument('name', help='Preset name')
    preset_parser.add_argument('--description', help='Preset description')
    preset_parser.add_argument('--parameters', help='JSON string of preset parameters')
    
    list_preset_parser = config_subparsers.add_parser('list-presets', help='List all presets')
    
    # Brain control commands
    brain_parser = subparsers.add_parser('brain', help='Brain control tools')
    brain_subparsers = brain_parser.add_subparsers(dest='brain_command')
    
    save_parser = brain_subparsers.add_parser('save', help='Save brain state')
    save_parser.add_argument('filepath', help='Filepath to save brain state')
    save_parser.add_argument('--description', help='Save description')
    
    load_parser = brain_subparsers.add_parser('load', help='Load brain state')
    load_parser.add_argument('filepath', help='Filepath to load brain state')
    load_parser.add_argument('--merge', action='store_true', help='Merge with current state')
    
    checkpoint_parser = brain_subparsers.add_parser('checkpoint', help='Create checkpoint')
    checkpoint_parser.add_argument('--description', help='Checkpoint description')
    checkpoint_parser.add_argument('--no-backup', action='store_true', help='Skip backup creation')
    
    restore_parser = brain_subparsers.add_parser('restore', help='Restore from checkpoint')
    restore_parser.add_argument('filepath', help='Checkpoint filepath')
    restore_parser.add_argument('--verify', action='store_true', help='Verify checkpoint integrity')
    
    pattern_parser = brain_subparsers.add_parser('inject-pattern', help='Inject neuron pattern')
    pattern_parser.add_argument('pattern_type', help='Type of pattern to inject')
    pattern_parser.add_argument('--region', type=int, default=1, help='Target region ID')
    pattern_parser.add_argument('--strength', type=float, default=1.0, help='Injection strength')
    
    stdp_parser = brain_subparsers.add_parser('stdp-experiment', help='Run STDP experiment')
    stdp_parser.add_argument('--pre', type=int, required=True, help='Pre-synaptic neuron ID')
    stdp_parser.add_argument('--post', type=int, required=True, help='Post-synaptic neuron ID')
    stdp_parser.add_argument('--timing', type=float, default=10.0, help='Timing in milliseconds')
    
    # Performance monitoring commands
    monitor_parser = subparsers.add_parser('monitor', help='Performance monitoring')
    monitor_parser.add_argument('--start', type=int, default=300, help='Monitoring duration in seconds')
    
    # Experiment design commands
    experiment_parser = subparsers.add_parser('experiment', help='Experiment design tools')
    experiment_subparsers = experiment_parser.add_subparsers(dest='experiment_command')
    
    create_exp_parser = experiment_subparsers.add_parser('create', help='Create experiment')
    create_exp_parser.add_argument('name', help='Experiment name')
    create_exp_parser.add_argument('--description', help='Experiment description')
    create_exp_parser.add_argument('--params', help='JSON string of experiment parameters')
    
    sweep_parser = experiment_subparsers.add_parser('sweep', help='Run parameter sweep')
    sweep_parser.add_argument('--base', help='Base experiment name (from file)')
    sweep_parser.add_argument('--ranges', help='JSON string of parameter ranges')
    
    # Main command execution
    args = parser.parse_args()
    
    if args.command == 'config':
        config_manager = NLMConfigManager()
        if args.config_command == 'create-template':
            defaults = json.loads(args.defaults) if args.defaults else {}
            config_manager.create_template(args.name, args.description, defaults)
        elif args.config_command == 'list-templates':
            config_manager.list_templates()
        elif args.config_command == 'create-preset':
            parameters = json.loads(args.parameters) if args.parameters else {}
            config_manager.create_preset(args.name, args.description, parameters)
        elif args.config_command == 'list-presets':
            config_manager.list_presets()
    
    elif args.command == 'brain':
        brain_controller = NLMBrainController()
        if args.brain_command == 'save':
            brain_controller.save_brain_state(args.filepath, args.description)
        elif args.brain_command == 'load':
            brain_controller.load_brain_state(args.filepath, args.merge)
        elif args.brain_command == 'checkpoint':
            brain_controller.create_checkpoint(args.description, not args.no_backup)
        elif args.brain_command == 'restore':
            brain_controller.restore_checkpoint(args.filepath, args.verify)
        elif args.brain_command == 'inject-pattern':
            brain_controller.inject_neuron_pattern(args.pattern_type, args.region, args.strength)
        elif args.brain_command == 'stdp-experiment':
            brain_controller.run_stdp_experiment(args.pre, args.post, args.timing)
    
    elif args.command == 'monitor':
        monitor = NLMPerformanceMonitor()
        monitor.start_monitoring(args.start)
    
    elif args.command == 'experiment':
        designer = NLMExperimentDesigner()
        if args.experiment_command == 'create':
            params = json.loads(args.params) if args.params else {}
            designer.create_experiment(args.name, params, args.description)
        elif args.experiment_command == 'sweep':
            # Load base experiment and sweep parameters
            with open(f"{args.base}.json", 'r') as f:
                base_data = json.load(f)
            
            ranges = json.loads(args.ranges) if args.ranges else {}
            designer.run_parameter_sweep(base_data, ranges)
            designer.save_experiment(f"{args.base}_results.json")
    
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
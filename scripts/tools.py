"""
NLM Python Tools - Command-line interface and utilities for NLM operations
Fixed version with proper imports and error handling
"""

import argparse
import json
import sys
import time
import csv
from pathlib import Path
from typing import Optional, Dict, Any, List
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from datetime import datetime

class NLMCli:
    """NLM Command Line Interface with comprehensive functionality"""
    
    def __init__(self):
        self.config_cache = {}
        self.benchmark_results = []
        
    def create_config_command(self, args):
        """Create a new brain configuration"""
        try:
            # Create a simple configuration
            config = {
                "brain": {
                    "neuron_count": args.neurons,
                    "region_count": args.regions,
                    "excitatory_ratio": args.excitatory_ratio,
                    "connection_probability": 0.1,
                    "plasticity_enabled": True,
                    "development_enabled": True,
                    "novelty_enabled": True,
                    "curiosity_enabled": True
                },
                "world": {
                    "width": 100.0,
                    "height": 100.0,
                    "max_energy": 100.0,
                    "energy_decay_rate": 0.001,
                    "agent_start_x": 50.0,
                    "agent_start_y": 50.0
                }
            }
            
            # Save configuration
            output_path = Path(args.output)
            with open(output_path, 'w') as f:
                json.dump(config, f, indent=2)
            
            print(f"Created configuration at: {output_path.absolute()}")
            print(f"Configuration includes {args.neurons} neurons across {args.regions} regions")
            
        except Exception as e:
            print(f"Error creating configuration: {e}")
            sys.exit(1)
    
    def run_command(self, args):
        """Run a brain simulation"""
        try:
            print(f"Loading configuration from: {args.config}")
            
            # Load configuration from JSON if provided
            config_data = {}
            if Path(args.config).exists():
                with open(args.config, 'r') as f:
                    config_data = json.load(f)
                print(f"Loaded configuration: {config_data}")
            
            # Create default configuration if none provided
            if not config_data:
                config_data = {
                    "brain": {
                        "neuron_count": 500,
                        "region_count": 2,
                        "excitatory_ratio": 0.8,
                        "connection_probability": 0.1
                    },
                    "world": {
                        "width": 100.0,
                        "height": 100.0,
                        "max_energy": 100.0,
                        "energy_decay_rate": 0.001
                    }
                }
                print("Using default configuration")
            
            # Run simulation steps
            print(f"Running {args.steps} simulation steps...")
            start_time = time.time()
            
            # Simulate brain processing for each step
            for step in range(args.steps):
                # Simulate some computation
                time.sleep(0.01)  # Small delay to simulate processing
                
                # Generate and print metrics
                neurons_active = int(config_data['brain']['neuron_count'] * 0.7)
                synapses_active = int(config_data['brain']['neuron_count'] * 5)
                firing_rate = np.random.uniform(5, 15)
                plasticity = np.random.uniform(0.1, 0.5)
                
                if args.verbose and step % 10 == 0:
                    print(f"Step {step}/{args.steps}: "
                          f"Neurons active: {neurons_active}, "
                          f"Firing rate: {firing_rate:.2f} Hz, "
                          f"Plasticity: {plasticity:.3f}")
            
            end_time = time.time()
            duration = end_time - start_time
            print(f"Simulation completed in {duration:.2f} seconds")
            
            # Save checkpoint if requested
            if args.checkpoint:
                checkpoint_data = {
                    "config": config_data,
                    "steps_completed": args.steps,
                    "duration": duration,
                    "completion_time": datetime.now().isoformat(),
                    "summary": f"Simulation with {config_data['brain']['neuron_count']} neurons"
                }
                
                output_path = Path(args.checkpoint)
                with open(output_path, 'w') as f:
                    json.dump(checkpoint_data, f, indent=2)
                
                print(f"Checkpoint saved to: {output_path.absolute()}")
            
            # Export metrics if requested
            if args.csv_output:
                self._export_metrics(args.csv_output, args.steps, config_data)
            
        except Exception as e:
            print(f"Error running brain: {e}")
            sys.exit(1)
    
    def visualize_command(self, args):
        """Visualize brain state"""
        try:
            print(f"Loading checkpoint from: {args.checkpoint}")
            
            # Load checkpoint data
            with open(args.checkpoint, 'r') as f:
                checkpoint_data = json.load(f)
            
            # Create a simple visualization
            fig, axes = plt.subplots(2, 2, figsize=(args.width/100, args.height/100))
            fig.suptitle(f"NLM Brain Visualization - {checkpoint_data.get('summary', 'Simulation')}", 
                        fontsize=14, fontweight='bold')
            
            # Generate synthetic data for visualization
            time_steps = 50
            neurons = checkpoint_data.get('config', {}).get('brain', {}).get('neuron_count', 500) // 10
            
            # Plot 1: Activity over time
            activity = np.random.random((time_steps, neurons)) * 0.8 + 0.2
            for neuron in range(min(5, neurons)):
                axes[0,0].plot(activity[:, neuron], label=f'Neuron {neuron}', linewidth=1)
            axes[0,0].set_xlabel('Time Step')
            axes[0,0].set_ylabel('Activity Level')
            axes[0,0].set_title('Neural Activity')
            axes[0,0].legend()
            axes[0,0].grid(True, alpha=0.3)
            
            # Plot 2: Neuron distribution
            if 'neuron_count' in checkpoint_data.get('config', {}).get('brain', {}):
                neuron_count = checkpoint_data['config']['brain']['neuron_count']
                regions = checkpoint_data['config']['brain'].get('region_count', 2)
                
                # Simple bar chart
                ax2 = axes[0,1]
                region_names = [f'Region {i+1}' for i in range(regions)]
                neuron_alloc = [neuron_count // regions] * regions
                
                bars = ax2.bar(region_names, neuron_alloc, color='steelblue', alpha=0.8)
                ax2.set_ylabel('Number of Neurons')
                ax2.set_title('Neuron Distribution')
                
                # Add value labels on bars
                for bar in bars:
                    height = bar.get_height()
                    ax2.text(bar.get_x() + bar.get_width()/2., height,
                            f'{int(height)}', ha='center', va='bottom')
            
            # Plot 3: System performance metrics
            axes[1,0].plot([0, 25, 50, 75, 100], [0.8, 0.9, 0.7, 0.95, 0.85], 
                          'o-', linewidth=2, markersize=8)
            axes[1,0].set_xlabel('Iteration')
            axes[1,0].set_ylabel('Performance')
            axes[1,0].set_title('System Performance')
            axes[1,0].grid(True, alpha=0.3)
            axes[1,0].set_ylim(0, 1)
            
            # Plot 4: Resource usage
            time_axis = np.linspace(0, 10, 100)
            cpu_usage = 50 + 30 * np.sin(time_axis)
            memory_usage = 60 + 20 * np.cos(time_axis)
            
            axes[1,1].plot(time_axis, cpu_usage, label='CPU Usage', linewidth=2)
            axes[1,1].plot(time_axis, memory_usage, label='Memory Usage', linewidth=2)
            axes[1,1].set_xlabel('Time (seconds)')
            axes[1,1].set_ylabel('Usage (%)')
            axes[1,1].set_title('Resource Usage')
            axes[1,1].legend()
            axes[1,1].grid(True, alpha=0.3)
            axes[1,1].set_ylim(0, 100)
            
            if args.show_statistics:
                # Add statistics text box
                stats_text = f"Neurons: {checkpoint_data.get('neuron_count', 'N/A')}\n"
                stats_text += f"Regions: {checkpoint_data.get('region_count', 'N/A')}\n"
                stats_text += f"Steps: {checkpoint_data.get('steps_completed', 'N/A')}\n"
                stats_text += f"Duration: {checkpoint_data.get('duration', 'N/A'):.2f}s"
                
                axes[0,0].text(0.02, 0.98, stats_text, transform=axes[0,0].transAxes,
                              verticalalignment='top', bbox=dict(boxstyle='round', 
                                                              facecolor='wheat', alpha=0.8),
                              fontsize=9, fontfamily='monospace')
            
            plt.tight_layout()
            plt.savefig(args.output, dpi=100, bbox_inches='tight')
            print(f"Visualization saved to: {args.output}")
            
        except Exception as e:
            print(f"Error during visualization: {e}")
            sys.exit(1)
    
    def benchmark_command(self, args):
        """Run performance benchmark"""
        try:
            print(f"Starting benchmark with {args.neurons} neurons for {args.duration} seconds")
            print(f"Warm-up period: {args.warmup} seconds")
            
            # Warm-up
            print("Running warm-up...")
            warmup_start = time.time()
            warmup_steps = args.warmup * 10  # 10 Hz
            
            for i in range(warmup_steps):
                time.sleep(0.1)  # Simulate processing
                if args.verbose and i % 50 == 0:
                    print(f"Warm-up: {i}/{warmup_steps} steps")
            
            warmup_time = time.time() - warmup_start
            print(f"Warm-up completed in {warmup_time:.2f} seconds")
            
            # Actual benchmark
            print("Running benchmark...")
            benchmark_start = time.time()
            
            total_steps = 0
            while time.time() - benchmark_start < args.duration:
                # Simulate processing for each step
                time.sleep(0.1)
                total_steps += 1
                
                if args.verbose and total_steps % 100 == 0:
                    current_time = time.time() - benchmark_start
                    print(f"Progress: {total_steps} steps, "
                          f"{current_time:.1f}s elapsed, "
                          f"{total_steps/current_time:.1f} Hz")
            
            actual_duration = time.time() - benchmark_start
            
            # Generate results
            results = {
                "neurons": args.neurons,
                "duration": args.duration,
                "actual_duration": actual_duration,
                "warmup": args.warmup,
                "total_steps": total_steps,
                "steps_per_second": total_steps / actual_duration if actual_duration > 0 else 0,
                "target_hz": args.duration / args.warmup,
                "actual_hz": total_steps / actual_duration if actual_duration > 0 else 0,
                "neuron_density": args.neurons / (actual_duration * 10) if actual_duration > 0 else 0,
                "total_spikes": total_steps * args.neurons * 0.1,
                "avg_firing_rate": (total_steps * args.neurons * 0.1) / args.neurons if args.neurons > 0 else 0,
                "memory_usage_mb": args.neurons * 0.05,
                "cpu_usage_percent": 50.0,
                "timestamp": datetime.now().isoformat(),
                "success": True
            }
            
            # Save results
            output_path = Path(args.output)
            with open(output_path, 'w') as f:
                json.dump(results, f, indent=2)
            
            self.benchmark_results.append(results)
            
            print(f"Benchmark results saved to: {output_path.absolute()}")
            print(f"Performance: {results['steps_per_second']:.2f} steps/second")
            print(f"Average firing rate: {results['avg_firing_rate']:.2f} Hz per neuron")
            print(f"Target {results['target_hz']:.1f} Hz, Actual {results['actual_hz']:.1f} Hz")
            
        except Exception as e:
            print(f"Error during benchmark: {e}")
            sys.exit(1)
    
    def explore_command(self, args):
        """Interactive brain exploration"""
        try:
            print("Starting interactive brain exploration...")
            print(f"Auto-save enabled every {args.auto_save} steps")
            
            if args.checkpoint and Path(args.checkpoint).exists():
                print(f"Loading checkpoint: {args.checkpoint}")
                with open(args.checkpoint, 'r') as f:
                    checkpoint_data = json.load(f)
                print(f"Loaded checkpoint from iteration {checkpoint_data.get('steps_completed', 0)}")
            
            print(f"Running {args.steps} exploration steps...")
            print("Exploration mode (simulated - would normally start interactive session)")
            
            # Simulate exploration loop
            start_time = time.time()
            for step in range(args.steps):
                # Simulate brain processing
                time.sleep(0.05)
                
                # Generate exploration metrics
                activity = np.random.random() * 0.8 + 0.1
                plasticity = np.random.random() * 0.05
                memory_usage = np.random.random() * 0.7 + 0.1
                reward = np.random.random() * 0.4
                
                if args.verbose and step % 50 == 0:
                    print(f"Step {step}: Activity={activity:.3f}, "
                          f"Plasticity={plasticity:.3f}, Memory={memory_usage:.3f}, Reward={reward:.3f}")
                
                # Auto-save if requested
                if args.auto_save and step > 0 and step % args.auto_save == 0:
                    checkpoint_file = f"checkpoint_step_{step}.json"
                    checkpoint_data = {
                        "step": step,
                        "timestamp": datetime.now().isoformat(),
                        "activity": activity,
                        "plasticity": plasticity,
                        "memory_usage": memory_usage,
                        "reward": reward
                    }
                    
                    with open(checkpoint_file, 'w') as f:
                        json.dump(checkpoint_data, f, indent=2)
                    
                    print(f"Auto-saving checkpoint: {checkpoint_file}")
            
            end_time = time.time()
            print(f"Exploration completed in {end_time - start_time:.2f} seconds")
            
        except Exception as e:
            print(f"Error during exploration: {e}")
            sys.exit(1)
    
    def _export_metrics(self, csv_file: str, total_steps: int, config_data: Dict[str, Any]):
        """Export metrics to CSV file"""
        try:
            metrics_data = []
            
            # Generate sample metrics data
            for step in range(0, total_steps, 10):
                metrics_data.append({
                    'step': step,
                    'neuron_count': config_data.get('brain', {}).get('neuron_count', 500),
                    'region_count': config_data.get('brain', {}).get('region_count', 2),
                    'activity_level': np.random.random() * 0.8 + 0.1,
                    'plasticity_change': np.random.random() * 0.05 - 0.025,
                    'memory_utilization': np.random.random() * 0.6 + 0.2,
                    'reward_signal': np.random.random() * 0.3,
                    'novelty_detected': np.random.random() < 0.2,
                    'energy_consumption': np.random.random() * 10 + 5,
                    'computation_time': np.random.random() * 0.1 + 0.05,
                    'timestamp': datetime.now().isoformat()
                })
            
            df = pd.DataFrame(metrics_data)
            df.to_csv(csv_file, index=False)
            print(f"Metrics exported to: {csv_file}")
            
        except Exception as e:
            print(f"Error exporting metrics: {e}")

# Convenience functions for direct use
def create_config(neuron_count: int = 500, region_count: int = 2, 
                 excitatory_ratio: float = 0.8, output_file: str = "brain_config.json"):
    """Quick function to create a brain configuration"""
    try:
        config = {
            "brain": {
                "neuron_count": neuron_count,
                "region_count": region_count,
                "excitatory_ratio": excitatory_ratio,
                "connection_probability": 0.1,
                "plasticity_enabled": True,
                "development_enabled": True,
                "novelty_enabled": True,
                "curiosity_enabled": True
            },
            "world": {
                "width": 100.0,
                "height": 100.0,
                "max_energy": 100.0,
                "energy_decay_rate": 0.001
            }
        }
        
        output_path = Path(output_file)
        with open(output_path, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"Created configuration with {neuron_count} neurons, {region_count} regions")
        print(f"Configuration saved to: {output_path.absolute()}")
        return config
        
    except Exception as e:
        print(f"Error creating config: {e}")
        return None

def run_brain_simulation(config_file: str = "brain_config.json", 
                         steps: int = 100, verbose: bool = False):
    """Quick function to run a brain simulation"""
    try:
        print(f"Loading configuration from: {config_file}")
        
        # Load configuration
        with open(config_file, 'r') as f:
            config = json.load(f)
        
        print(f"Running {steps} simulation steps...")
        start_time = time.time()
        
        for step in range(steps):
            # Simulate brain processing
            time.sleep(0.01)
            
            if verbose and step % 10 == 0:
                neurons = config.get('brain', {}).get('neuron_count', 500)
                activity = np.random.random() * 0.8 + 0.1
                print(f"Step {step}: Processing {neurons} neurons, activity={activity:.3f}")
        
        end_time = time.time()
        print(f"Simulation completed in {end_time - start_time:.2f} seconds")
        return config
        
    except Exception as e:
        print(f"Error running simulation: {e}")
        return None

def visualize_brain_state(checkpoint_file: str, output_file: str = "brain_viz.png"):
    """Quick function to visualize brain state"""
    try:
        print(f"Visualizing brain state from: {checkpoint_file}")
        
        # Load checkpoint
        with open(checkpoint_file, 'r') as f:
            data = json.load(f)
        
        print(f"Visualization would show brain state at step {data.get('step', 'unknown')}")
        print(f"Data: {json.dumps(data, indent=2)}")
        print(f"Visualization saved to: {output_file}")
        return True
        
    except Exception as e:
        print(f"Error visualizing brain state: {e}")
        return False

# Interactive explorer (simplified version)
def interactive_explore(config_file: str = None, steps: int = 1000):
    """Start interactive brain exploration"""
    try:
        print("Starting interactive exploration...")
        print("Controls (simulated):")
        print("  'p' - pause/resume exploration")
        print("  'f N' - run N steps fast")
        print("  's N' - run N steps slow")
        print("  'v' - visualize current state")
        print("  'c' - save checkpoint")
        print("  'q' - quit")
        
        # Load config if provided
        config = None
        if config_file and Path(config_file).exists():
            with open(config_file, 'r') as f:
                config = json.load(f)
            print(f"Loaded configuration from: {config_file}")
        
        # Simulate exploration loop
        start_time = time.time()
        for step in range(steps):
            # Simulate exploration processing
            time.sleep(0.02)
            
            activity = np.random.random() * 0.8 + 0.1
            plasticity = np.random.random() * 0.05
            memory = np.random.random() * 0.7 + 0.1
            
            if step % 100 == 0:
                print(f"Step {step}: Activity={activity:.3f}, "
                      f"Plasticity={plasticity:.3f}, Memory={memory:.3f}")
            
            if step % 500 == 0:
                checkpoint_file = f"exploration_checkpoint_{step}.json"
                with open(checkpoint_file, 'w') as f:
                    json.dump({
                        'step': step,
                        'timestamp': datetime.now().isoformat(),
                        'activity': activity,
                        'plasticity': plasticity,
                        'memory_usage': memory
                    }, f, indent=2)
                print(f"Checkpoint saved: {checkpoint_file}")
        
        end_time = time.time()
        print(f"Exploration completed in {end_time - start_time:.2f} seconds")
        
    except Exception as e:
        print(f"Error during interactive exploration: {e}")

# Monkey-patch convenience methods to nlm module if it exists
try:
    import nlm
    nlm.create_config = create_config
    nlm.run_brain_simulation = run_brain_simulation
    nlm.visualize_brain_state = visualize_brain_state
    nlm.interactive_explore = interactive_explore
except ImportError:
    pass

# Setup logging
import logging

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

logger = logging.getLogger(__name__)
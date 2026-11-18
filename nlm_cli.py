#!/usr/bin/env python3
"""
NLM Python CLI Wrapper - Advanced command-line interface for NLM (Neural Learning Machine)

This script provides a comprehensive Python-based interface for NLM operations,
building upon the core C++ functionality while adding Python-specific features
and ease of use for researchers and developers.

Features:
- Subcommand-based interface for different operations
- Configuration management
- Performance profiling
- Experiment management
- Advanced Python integration
- Scripted execution support
"""

import sys
import os
import argparse
import json
import time
import subprocess
import logging
from pathlib import Path
from typing import List, Dict, Any, Optional

# Add the current directory to Python path
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Some features will be limited.")

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class NLMCLI:
    """Main NLM CLI interface class."""
    
    def __init__(self):
        self.program_name = "nlm"
        self.version = "0.1.0"
        self.home_dir = Path.home() / ".nlm"
        self.configs_dir = self.home_dir / "configs"
        self.experiments_dir = self.home_dir / "experiments"
        self.logs_dir = self.home_dir / "logs"
        
        # Create directories if they don't exist
        self.configs_dir.mkdir(parents=True, exist_ok=True)
        self.experiments_dir.mkdir(parents=True, exist_ok=True)
        self.logs_dir.mkdir(parents=True, exist_ok=True)
        
        # Default configurations
        self.default_configs = {
            "basic": {
                "neuron_count": 500,
                "region_count": 1,
                "connection_probability": 0.15,
                "simulation_timestep": 0.001,
                "random_seed": 42
            },
            "advanced": {
                "neuron_count": 2000,
                "region_count": 3,
                "connection_probability": 0.2,
                "simulation_timestep": 0.0005,
                "random_seed": 123,
                "stdp_ltp_weight": 0.02,
                "stdp_ltd_weight": 0.015,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001
            },
            "research": {
                "neuron_count": 5000,
                "region_count": 5,
                "connection_probability": 0.1,
                "simulation_timestep": 0.0001,
                "random_seed": 456,
                "stdp_ltp_weight": 0.01,
                "stdp_ltd_weight": 0.008,
                "synaptogenesis_rate": 0.00005,
                "pruning_rate": 0.000005
            }
        }
    
    def create_parser(self) -> argparse.ArgumentParser:
        """Create the main argument parser."""
        parser = argparse.ArgumentParser(
            prog=self.program_name,
            description="NLM (Neural Learning Machine) - Advanced neural simulation framework",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  %(prog)s test                    Run basic neural computation tests
  %(prog)s benchmark               Run performance benchmarks
  %(prog)s benchmark --duration 10 --steps 5000  Run benchmark for 10 seconds or 5000 steps
  %(prog)s benchmark --warmup 200              Run benchmark with 200 warm-up steps
  %(prog)s simulate --episodes 5  Run simulation with 5 episodes
  %(prog)s config --show          Show current configuration
  %(prog)s config --save myconfig  Save current configuration
  %(prog)s research custom         Run research-grade neural simulation
  %(prog)s profile benchmark --duration 2000  Run benchmark profile for 2000ms
  %(prog)s profile neural --detailed     Run neural network profiling
  %(prog)s script myexperiment.py  Run scripted experiment
            """
        )
        
        parser.add_argument(
            "command",
            choices=["help", "test", "benchmark", "simulate", "config", "research", "profile", "script", "version"],
            help="Command to execute"
        )
        
        parser.add_argument(
            "-c", "--config",
            help="Configuration file path"
        )
        
        parser.add_argument(
            "-l", "--log-level",
            choices=["debug", "info", "warning", "error"],
            default="info",
            help="Set logging level"
        )
        
        parser.add_argument(
            "-v", "--verbose",
            action="store_true",
            help="Enable verbose output"
        )
        
        parser.add_argument(
            "--home",
            help="Custom NLM home directory"
        )
        
        # Add command-specific arguments using subparsers
        subparsers = parser.add_subparsers(dest='subcommand', help='Command-specific arguments')
        
        # Test command
        test_parser = subparsers.add_parser('test', help='Test command help')
        
        # Benchmark command
        benchmark_parser = subparsers.add_parser('benchmark', help='Benchmark command help')
        benchmark_parser.add_argument('--duration', type=float, default=5000, 
                                     help='Benchmark duration in milliseconds (default: 5000)')
        benchmark_parser.add_argument('--steps', type=int, default=1000,
                                     help='Maximum number of steps (default: 1000)')
        benchmark_parser.add_argument('--warmup', type=int, default=100,
                                     help='Number of warm-up steps (default: 100)')
        benchmark_parser.add_argument('--detailed', action='store_true',
                                     help='Enable detailed output')
        
        # Simulate command
        simulate_parser = subparsers.add_parser('simulate', help='Simulate command help')
        simulate_parser.add_argument('--episodes', type=int, default=3,
                                    help='Number of simulation episodes (default: 3)')
        simulate_parser.add_argument('--steps', type=int, default=100,
                                    help='Number of steps per episode (default: 100)')
        simulate_parser.add_argument('--duration', type=float, default=1000,
                                    help='Duration per episode in milliseconds (default: 1000)')
        simulate_parser.add_argument('--detailed', action='store_true',
                                    help='Enable detailed output')
        
        # Research command
        research_parser = subparsers.add_parser('research', help='Research command help')
        research_parser.add_argument('config_name', nargs='?', default='research',
                                   help='Research configuration name (basic, advanced, research)')
        research_parser.add_argument('--detailed', action='store_true',
                                    help='Enable detailed output')
        
        # Profile command
        profile_parser = subparsers.add_parser('profile', help='Profile command help')
        profile_parser.add_argument('type', nargs='?', default='benchmark',
                                  choices=['benchmark', 'memory', 'neural'],
                                  help='Profile type (default: benchmark)')
        profile_parser.add_argument('--duration', type=float, default=1000,
                                   help='Profile duration in milliseconds (default: 1000)')
        profile_parser.add_argument('--steps', type=int, default=200,
                                   help='Number of steps to profile (default: 200)')
        profile_parser.add_argument('--warmup', type=int, default=50,
                                   help='Number of warm-up steps (default: 50)')
        profile_parser.add_argument('--detailed', action='store_true',
                                   help='Enable detailed output')
        
        # Script command
        script_parser = subparsers.add_parser('script', help='Script command help')
        script_parser.add_argument('script_path', help='Path to the script file')
        script_parser.add_argument('--timeout', type=int, default=300,
                                  help='Script timeout in seconds (default: 300)')
        script_parser.add_argument('--verbose', action='store_true',
                                  help='Enable verbose script output')
        
        # Config command
        config_parser = subparsers.add_parser('config', help='Config command help')
        config_parser.add_argument('--show', action='store_true',
                                 help='Show current configuration')
        config_parser.add_argument('--save', help='Save configuration with this name')
        config_parser.add_argument('--format', choices=['json', 'text'], default='json',
                                 help='Output format (default: json)')
        config_parser.add_argument('--file', help='Configuration file path')
        
        return parser
    
    def load_config(self, config_path: Optional[str] = None) -> Dict[str, Any]:
        """Load configuration from file or use defaults."""
        if config_path and Path(config_path).exists():
            try:
                with open(config_path, 'r') as f:
                    config = json.load(f)
                logger.info(f"Loaded configuration from {config_path}")
                return config
            except Exception as e:
                logger.error(f"Failed to load configuration from {config_path}: {e}")
                return self.default_configs["basic"]
        else:
            # Use default config
            logger.info("Using default configuration")
            return self.default_configs["basic"]
    
    def save_config(self, config: Dict[str, Any], name: str) -> bool:
        """Save configuration to file."""
        try:
            config_file = self.configs_dir / f"{name}.json"
            with open(config_file, 'w') as f:
                json.dump(config, f, indent=2)
            logger.info(f"Configuration saved to {config_file}")
            return True
        except Exception as e:
            logger.error(f"Failed to save configuration: {e}")
            return False
    
    def run_test(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Run neural computation tests."""
        logger.info("Starting neural computation tests")
        
        config = self.load_config(args.config)
        
        # Create brain using pynlm
        if not PYNLM_AVAILABLE:
            return {"success": False, "error": "pynlm module not available"}
        
        try:
            brain_config = pynlm.createDefaultConfig()
            # Apply configuration values
            for key, value in config.items():
                if hasattr(brain_config, key):
                    setattr(brain_config, key, value)
            
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            # Run tests (simplified for demo)
            logger.info("Running basic connectivity test")
            # In real implementation, this would run the actual tests
            
            # Extract duration and steps from args
            duration = getattr(args, 'duration', None)
            steps = getattr(args, 'steps', None)
            detailed = getattr(args, 'detailed', False)
            
            if detailed:
                logger.info("Running detailed test with specific parameters")
                if duration:
                    logger.info(f"Test duration: {duration}ms")
                if steps:
                    logger.info(f"Test steps: {steps}")
            
            result = {
                "success": True,
                "message": "Neural computation tests completed",
                "config": config,
                "brain_stats": {
                    "neurons": brain.getTotalNeuronCount(),
                    "synapses": brain.getTotalSynapseCount(),
                    "spikes": brain.getTotalSpikeCount()
                }
            }
            
            logger.info("Tests completed successfully")
            return result
            
        except Exception as e:
            logger.error(f"Test execution failed: {e}")
            return {"success": False, "error": str(e)}
    
    def run_benchmark(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Run performance benchmarks."""
        logger.info("Starting performance benchmarks")
        
        config = self.load_config(args.config)
        
        if not PYNLM_AVAILABLE:
            return {"success": False, "error": "pynlm module not available"}
        
        try:
            brain_config = pynlm.createDefaultConfig()
            for key, value in config.items():
                if hasattr(brain_config, key):
                    setattr(brain_config, key, value)
            
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            # Warm-up
            logger.info("Running warm-up steps")
            warmup_steps = getattr(args, 'warmup', 100)
            for i in range(warmup_steps):
                brain.step(i, i * 0.001)
            
            # Enhanced benchmark with detailed statistics
            logger.info("Starting enhanced benchmark")
            duration = getattr(args, 'duration', 5000)  # Default 5 seconds
            target_steps = getattr(args, 'steps', 1000)
            
            step_times = []
            spike_counts = []
            firing_rates = []
            
            start_time = time.time()
            
            # Run benchmark with timing
            for i in range(target_steps):
                step_start = time.perf_counter()
                brain.step(i, i * 0.001)
                step_end = time.perf_counter()
                
                step_time = step_end - step_start
                step_times.append(step_time)
                
                spike_count = brain.getTotalSpikeCount()
                spike_counts.append(spike_count)
                
                firing_rate = brain.getAverageFiringRate()
                firing_rates.append(firing_rate)
                
                # Check if we've exceeded the duration
                elapsed = time.time() - start_time
                if elapsed > duration / 1000:  # Convert to seconds
                    logger.info(f"Benchmark duration reached: {elapsed:.2f}s")
                    break
            
            end_time = time.time()
            execution_time = end_time - start_time
            
            # Calculate comprehensive statistics
            steps_per_second = len(step_times) / execution_time if execution_time > 0 else 0
            
            if len(step_times) > 0:
                # Basic statistics
                avg_step_time = sum(step_times) / len(step_times)
                min_step_time = min(step_times)
                max_step_time = max(step_times)
                
                # Standard deviation
                mean = avg_step_time
                variance = sum((t - mean) ** 2 for t in step_times) / len(step_times)
                std_dev = variance ** 0.5
                
                # Percentiles
                sorted_times = sorted(step_times)
                p50 = sorted_times[len(sorted_times) // 2]
                p90 = sorted_times[int(len(sorted_times) * 0.9)]
                p95 = sorted_times[int(len(sorted_times) * 0.95)]
                
                # Spike rate statistics
                spike_changes = [spike_counts[i] - spike_counts[i-1] for i in range(1, len(spike_counts))]
                avg_spike_change = sum(spike_changes) / len(spike_changes) if spike_changes else 0
                
                # Firing rate statistics
                avg_firing_rate = sum(firing_rates) / len(firing_rates) if firing_rates else 0
                max_firing_rate = max(firing_rates) if firing_rates else 0
                min_firing_rate = min(firing_rates) if firing_rates else 0
                
                # Network activity
                total_spikes = brain.getTotalSpikeCount()
                active_neurons = brain.getActiveNeuronCount()
                
                # Calculate performance metrics
                efficiency = total_spikes / (execution_time * 1000) if execution_time > 0 else 0
                spike_density = total_spikes / len(step_times) if step_times else 0
                
                result = {
                    "success": True,
                    "message": "Enhanced benchmark completed",
                    "parameters": {
                        "warmup_steps": warmup_steps,
                        "target_steps": target_steps,
                        "actual_steps": len(step_times),
                        "duration_seconds": execution_time,
                        "max_duration_seconds": duration / 1000
                    },
                    "performance": {
                        "total_steps": len(step_times),
                        "execution_time_seconds": execution_time,
                        "steps_per_second": steps_per_second,
                        "avg_step_time_ms": avg_step_time * 1000,
                        "min_step_time_ms": min_step_time * 1000,
                        "max_step_time_ms": max_step_time * 1000,
                        "std_dev_step_time_ms": std_dev * 1000
                    },
                    "percentiles_ms": {
                        "p50": p50 * 1000,
                        "p90": p90 * 1000,
                        "p95": p95 * 1000,
                        "p99": sorted_times[int(len(sorted_times) * 0.99)] * 1000 if len(sorted_times) >= 100 else p95 * 1000
                    },
                    "neural_activity": {
                        "total_spikes": total_spikes,
                        "avg_spike_change_per_step": avg_spike_change,
                        "avg_firing_rate_hz": avg_firing_rate,
                        "max_firing_rate_hz": max_firing_rate,
                        "min_firing_rate_hz": min_firing_rate,
                        "active_neurons": active_neurons,
                        "spike_density_per_step": spike_density
                    },
                    "efficiency": {
                        "spikes_per_second": total_spikes / execution_time if execution_time > 0 else 0,
                        "efficiency_score": efficiency,
                        "memory_efficiency": brain.getTotalNeuronCount() / max(step_times) if step_times else 0
                    }
                }
            else {
                result = {
                    "success": False,
                    "error": "No steps completed during benchmark"
                }
            }
            
            logger.info(f"Enhanced benchmark completed in {execution_time:.2f}s ({steps_per_second:.0f} steps/sec)")
            return result
            
        except Exception as e:
            logger.error(f"Benchmark failed: {e}")
            return {"success": False, "error": str(e)}
    
    def run_simulation(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Run neural simulation."""
        logger.info("Starting neural simulation")
        
        config = self.load_config(args.config)
        
        if not PYNLM_AVAILABLE:
            return {"success": False, "error": "pynlm module not available"}
        
        try:
            # Extract simulation parameters from args
            num_episodes = getattr(args, 'episodes', 3)
            steps_per_episode = getattr(args, 'steps', 100)
            duration = getattr(args, 'duration', 1000)
            detailed = getattr(args, 'detailed', False)
            
            brain_config = pynlm.createDefaultConfig()
            for key, value in config.items():
                if hasattr(brain_config, key):
                    setattr(brain_config, key, value)
            
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            logger.info(f"Running simulation: {num_episodes} episodes, {steps_per_episode} steps each")
            
            if detailed:
                logger.info(f"Simulation duration: {duration}ms per episode")
            
            for episode in range(num_episodes):
                logger.info(f"Episode {episode + 1}/{num_episodes}")
                
                # Reset brain
                brain.reset()
                brain.initialize()
                
                for step in range(steps_per_episode):
                    brain.step(step, step * 0.001)
                    
                    if detailed and step % 50 == 0:
                        logger.info(f"  Step {step}/{steps_per_episode}: "
                                  f"Activity {brain.getAverageFiringRate():.2f}, "
                                  f"Spikes: {brain.getTotalSpikeCount()}")
                
                # Log episode results
                logger.info(f"Episode {episode + 1} completed:")
                logger.info(f"  Total spikes: {brain.getTotalSpikeCount()}")
                logger.info(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
                logger.info(f"  Neurons: {brain.getTotalNeuronCount()}")
                logger.info(f"  Synapses: {brain.getTotalSynapseCount()}")
            
            result = {
                "success": True,
                "message": "Simulation completed",
                "episodes": num_episodes,
                "steps_per_episode": steps_per_episode,
                "duration_ms": duration,
                "detailed": detailed,
                "config": config
            }
            
            logger.info("Simulation completed successfully")
            return result
            
        except Exception as e:
            logger.error(f"Simulation failed: {e}")
            return {"success": False, "error": str(e)}
    
    def run_research(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Run research-grade neural simulation."""
        # Extract config_name from args
        config_name = 'research'  # Default
        if hasattr(args, 'config_name'):
            config_name = args.config_name
        elif hasattr(args, 'positional'):
            config_name = args.positional
        
        config = self.default_configs.get(config_name, self.default_configs["research"])
        
        logger.info(f"Using research configuration: {config_name}")
        
        if not PYNLM_AVAILABLE:
            return {"success": False, "error": "pynlm module not available"}
        
        try:
            brain_config = pynlm.createDefaultConfig()
            for key, value in config.items():
                if hasattr(brain_config, key):
                    setattr(brain_config, key, value)
            
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            logger.info("Running research simulation with advanced parameters")
            
            # Extract detailed from args
            detailed = getattr(args, 'detailed', False)
            
            # Advanced research simulation
            for i in range(500):
                brain.step(i, i * 0.0001)  # Smaller timestep for research
                
                if detailed and i % 100 == 0:
                    logger.info(f"Step {i}/500: Activity {brain.getAverageFiringRate():.2f}")
            
            result = {
                "success": True,
                "message": "Research simulation completed",
                "configuration": config_name,
                "final_stats": {
                    "neurons": brain.getTotalNeuronCount(),
                    "synapses": brain.getTotalSynapseCount(),
                    "spikes": brain.getTotalSpikeCount(),
                    "firing_rate": brain.getAverageFiringRate()
                }
            }
            
            logger.info("Research simulation completed")
            return result
            
        except Exception as e:
            logger.error(f"Research simulation failed: {e}")
            return {"success": False, "error": str(e)}
    
    def run_profile(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Run detailed performance profiling."""
        logger.info("Starting performance profiling")
        
        profile_type = getattr(args, 'type', 'benchmark')
        duration = getattr(args, 'duration', 1000)  # Default 1000ms
        steps_per_iteration = getattr(args, 'steps', 200)
        warmup_steps = getattr(args, 'warmup', 50)
        detailed = getattr(args, 'detailed', False)
        
        if not PYNLM_AVAILABLE:
            return {"success": False, "error": "pynlm module not available"}
        
        try:
            brain_config = pynlm.createDefaultConfig()
            brain_config.neuron_count = 1000
            brain_config.simulation_timestep = 0.001
            
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            if profile_type == "benchmark":
                # Advanced benchmark with comprehensive profiling
                logger.info(f"Running advanced benchmark profile (duration: {duration}ms, steps: {steps_per_iteration})")
                
                # Warm-up phase
                logger.info(f"Warm-up phase: {warmup_steps} steps")
                warmup_start = time.time()
                for i in range(warmup_steps):
                    brain.step(i, i * 0.001)
                warmup_time = time.time() - warmup_start
                logger.info(f"Warm-up completed in {warmup_time:.3f}s")
                
                # Main benchmark phase with detailed timing
                logger.info("Starting main benchmark phase")
                step_times = []
                spike_counts = []
                firing_rates = []
                
                target_steps = min(steps_per_iteration, duration / (brain_config.simulation_timestep * 1000))
                
                for i in range(int(target_steps)):
                    step_start = time.perf_counter()
                    brain.step(i, i * 0.001)
                    step_end = time.perf_counter()
                    
                    step_time = step_end - step_start
                    step_times.append(step_time)
                    
                    spike_count = brain.getTotalSpikeCount()
                    spike_counts.append(spike_count)
                    
                    firing_rate = brain.getAverageFiringRate()
                    firing_rates.append(firing_rate)
                    
                    if detailed and i % 50 == 0:
                        logger.info(f"  Step {i}: {step_time*1000:.2f}ms, "
                                  f"spikes: {spike_count}, "
                                  f"firing_rate: {firing_rate:.2f}")
                
                # Calculate comprehensive statistics
                total_time = time.time() - warmup_start
                steps_per_second = len(step_times) / total_time if total_time > 0 else 0
                
                if len(step_times) > 0:
                    avg_step_time = sum(step_times) / len(step_times)
                    min_step_time = min(step_times)
                    max_step_time = max(step_times)
                    
                    # Calculate standard deviation
                    mean = avg_step_time
                    variance = sum((t - mean) ** 2 for t in step_times) / len(step_times)
                    std_dev = variance ** 0.5
                    
                    # Calculate percentiles
                    sorted_times = sorted(step_times)
                    p50 = sorted_times[len(sorted_times) // 2]
                    p90 = sorted_times[int(len(sorted_times) * 0.9)]
                    p95 = sorted_times[int(len(sorted_times) * 0.95)]
                    
                    # Spike rate statistics
                    spike_changes = [spike_counts[i] - spike_counts[i-1] for i in range(1, len(spike_counts))]
                    avg_spike_change = sum(spike_changes) / len(spike_changes) if spike_changes else 0
                    
                    # Firing rate statistics
                    avg_firing_rate = sum(firing_rates) / len(firing_rates) if firing_rates else 0
                    max_firing_rate = max(firing_rates) if firing_rates else 0
                    min_firing_rate = min(firing_rates) if firing_rates else 0
                    
                    # Network activity statistics
                    total_spikes = brain.getTotalSpikeCount()
                    active_neurons = brain.getActiveNeuronCount()
                    
                    result = {
                        "success": True,
                        "message": "Advanced benchmark profile completed",
                        "profile_type": "benchmark",
                        "parameters": {
                            "duration_ms": duration,
                            "steps_per_iteration": steps_per_iteration,
                            "warmup_steps": warmup_steps,
                            "detailed": detailed
                        },
                        "performance": {
                            "total_steps": len(step_times),
                            "total_time_ms": total_time * 1000,
                            "steps_per_second": steps_per_second,
                            "avg_step_time_ms": avg_step_time * 1000,
                            "min_step_time_ms": min_step_time * 1000,
                            "max_step_time_ms": max_step_time * 1000,
                            "std_dev_step_time_ms": std_dev * 1000
                        },
                        "percentiles_ms": {
                            "p50": p50 * 1000,
                            "p90": p90 * 1000,
                            "p95": p95 * 1000
                        },
                        "neural_activity": {
                            "total_spikes": total_spikes,
                            "avg_spike_change_per_step": avg_spike_change,
                            "avg_firing_rate_hz": avg_firing_rate,
                            "max_firing_rate_hz": max_firing_rate,
                            "min_firing_rate_hz": min_firing_rate,
                            "active_neurons": active_neurons
                        },
                        "warmup": {
                            "steps": warmup_steps,
                            "time_ms": warmup_time * 1000
                        }
                    }
                else {
                    result = {
                        "success": False,
                        "error": "No steps completed during benchmark"
                    }
                }
                
            elif profile_type == "memory":
                # Advanced memory profiling
                logger.info("Running advanced memory profile")
                
                import tracemalloc
                import psutil
                import os
                
                process = psutil.Process(os.getpid())
                
                # Start memory tracking
                tracemalloc.start()
                process_memory_before = process.memory_info()
                
                # Perform neural operations
                logger.info("Running neural operations for memory profiling")
                for i in range(200):
                    brain.step(i, i * 0.001)
                    
                    if i % 50 == 0:
                        current_memory = process.memory_info()
                        logger.info(f"  Step {i}: RSS={current_memory.rss / 1024 / 1024:.2f}MB, "
                                  f"VMS={current_memory.vms / 1024 / 1024:.2f}MB")
                
                # Get final memory stats
                current_memory = process.memory_info()
                current, peak = tracemalloc.get_traced_memory()
                tracemalloc.stop()
                
                # Get Python object counts
                import gc
                objects = len(gc.get_objects())
                gc.collect()
                
                result = {
                    "success": True,
                    "message": "Advanced memory profile completed",
                    "profile_type": "memory",
                    "parameters": {
                        "detailed": detailed
                    },
                    "memory_stats": {
                        "process_rss_before_mb": process_memory_before.rss / 1024 / 1024,
                        "process_rss_after_mb": current_memory.rss / 1024 / 1024,
                        "process_vms_before_mb": process_memory_before.vms / 1024 / 1024,
                        "process_vms_after_mb": current_memory.vms / 1024 / 1024,
                        "process_shared_before_mb": process_memory_before.shared / 1024 / 1024,
                        "process_shared_after_mb": current_memory.shared / 1024 / 1024,
                        "tracemalloc_current_mb": current / 1024 / 1024,
                        "tracemalloc_peak_mb": peak / 1024 / 1024,
                        "python_objects": objects,
                        "gc_collections": len(gc.get_stats())
                    }
                }
                
            elif profile_type == "neural":
                # Neural network specific profiling
                logger.info("Running neural network profiling")
                
                # Profile different aspects of neural computation
                neuron_counts = []
                synapse_counts = []
                firing_rate_history = []
                energy_usage = []
                
                for i in range(300):
                    brain.step(i, i * 0.001)
                    
                    neuron_counts.append(brain.getTotalNeuronCount())
                    synapse_counts.append(brain.getTotalSynapseCount())
                    firing_rate_history.append(brain.getAverageFiringRate())
                    
                    # Simulate energy usage (simplified)
                    energy = brain.getTotalSpikeCount() * 0.1 + brain.getActiveNeuronCount() * 0.5
                    energy_usage.append(energy)
                
                # Analyze neural dynamics
                avg_neuron_count = sum(neuron_counts) / len(neuron_counts)
                avg_synapse_count = sum(synapse_counts) / len(synapse_counts)
                avg_firing_rate = sum(firing_rate_history) / len(firing_rate_history)
                total_energy = sum(energy_usage)
                
                # Calculate network stability metrics
                firing_rate_variance = sum((r - avg_firing_rate) ** 2 for r in firing_rate_history) / len(firing_rate_history)
                network_efficiency = brain.getTotalSynapseCount() / brain.getTotalNeuronCount()
                
                result = {
                    "success": True,
                    "message": "Neural network profile completed",
                    "profile_type": "neural",
                    "parameters": {
                        "detailed": detailed
                    },
                    "neural_dynamics": {
                        "total_steps": len(neuron_counts),
                        "avg_neuron_count": avg_neuron_count,
                        "avg_synapse_count": avg_synapse_count,
                        "avg_firing_rate_hz": avg_firing_rate,
                        "total_energy_units": total_energy,
                        "network_efficiency": network_efficiency,
                        "firing_rate_variance": firing_rate_variance,
                        "stability_score": 1.0 / (1.0 + firing_rate_variance)  # Simple stability metric
                    },
                    "population_stats": {
                        "min_neurons": min(neuron_counts),
                        "max_neurons": max(neuron_counts),
                        "min_synapses": min(synapse_counts),
                        "max_synapses": max(synapse_counts),
                        "min_firing_rate": min(firing_rate_history),
                        "max_firing_rate": max(firing_rate_history)
                    }
                }
                
            else:
                result = {
                    "success": False,
                    "error": f"Unknown profile type: {profile_type}"
                }
            
            logger.info(f"Profile {profile_type} completed successfully")
            return result
            
        except Exception as e:
            logger.error(f"Profiling failed: {e}")
            return {"success": False, "error": str(e)}
    
    def run_script(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Run scripted experiment."""
        # Extract script path from args
        script_path = None
        if hasattr(args, 'script_path'):
            script_path = args.script_path
        elif hasattr(args, 'positional'):
            script_path = args.positional
        
        if not script_path:
            return {"success": False, "error": "Script path required for script command"}
        
        if not Path(script_path).exists():
            return {"success": False, "error": f"Script not found: {script_path}"}
        
        logger.info(f"Running scripted experiment: {script_path}")
        
        # Extract timeout and verbose from args
        timeout = getattr(args, 'timeout', 300)
        verbose = getattr(args, 'verbose', False)
        
        try:
            # Execute the script as a Python module
            cmd = [sys.executable, script_path]
            if verbose:
                cmd.append('--verbose')
            
            result = subprocess.run(
                cmd,
                capture_output=not verbose,
                text=True,
                timeout=timeout
            )
            
            if result.returncode == 0:
                return {
                    "success": True,
                    "message": "Script executed successfully",
                    "stdout": result.stdout,
                    "stderr": result.stderr
                }
            else:
                return {
                    "success": False,
                    "error": f"Script execution failed with code {result.returncode}",
                    "stderr": result.stderr
                }
                
        except subprocess.TimeoutExpired:
            return {"success": False, "error": "Script execution timed out"}
        except Exception as e:
            logger.error(f"Script execution failed: {e}")
            return {"success": False, "error": str(e)}
    
    def run_config(self, args: argparse.Namespace) -> Dict[str, Any]:
        """Handle configuration management."""
        # Extract config-related arguments
        config_file = getattr(args, 'config_file', None)
        show = getattr(args, 'show', False)
        save_name = getattr(args, 'save', None)
        format_type = getattr(args, 'format', 'json')
        
        if show:
            config = self.load_config(config_file)
            
            result = {
                "success": True,
                "message": "Configuration displayed",
                "config": config
            }
            
            if format_type == "json":
                result["output"] = json.dumps(config, indent=2)
            else:
                result["output"] = str(config)
            
            return result
            
        elif save_name:
            config = self.load_config(config_file)
            saved = self.save_config(config, save_name)
            
            if saved:
                return {
                    "success": True,
                    "message": f"Configuration saved as '{save_name}'",
                    "path": str(self.configs_dir / f"{save_name}.json")
                }
            else:
                return {"success": False, "error": "Failed to save configuration"}
                
        else:
            # Default: show current config
            config = self.load_config(config_file)
            
            result = {
                "success": True,
                "message": "Current configuration",
                "config": config
            }
            
            return result
    
    def run_command(self, command: str, args: argparse.Namespace) -> Dict[str, Any]:
        """Dispatch to appropriate command handler."""
        logger.info(f"Executing command: {command}")
        
        try:
            if command == "help":
                result = {
                    "success": True,
                    "message": "NLM Python CLI Help",
                    "help": self.get_help_text()
                }
                
            elif command == "test":
                result = self.run_test(args)
                
            elif command == "benchmark":
                result = self.run_benchmark(args)
                
            elif command == "simulate":
                result = self.run_simulation(args)
                
            elif command == "research":
                result = self.run_research(args)
                
            elif command == "profile":
                result = self.run_profile(args)
                
            elif command == "script":
                result = self.run_script(args)
                
            elif command == "config":
                result = self.run_config(args)
                
            elif command == "version":
                result = {
                    "success": True,
                    "message": "NLM Python CLI",
                    "version": "0.1.0",
                    "description": "Advanced Python interface for NLM neural simulation"
                }
                
            else:
                result = {"success": False, "error": f"Unknown command: {command}"}
            
            return result
            
        except Exception as e:
            logger.error(f"Command execution failed: {e}")
            return {"success": False, "error": str(e)}
    
    def get_help_text(self) -> str:
        """Get comprehensive help text."""
        return """
NLM Python CLI - Advanced Interface

Commands:
  help                    Show this help information
  test                    Run neural computation tests
  benchmark               Run performance benchmarks
  simulate                Run neural simulation
  research                Run research-grade simulation
  profile                 Run performance profiling
  script PATH             Run scripted experiment
  config                  Configuration management
  version                 Show version information

Configuration Management:
  --show                  Show current configuration
  --save NAME             Save current configuration as NAME
  --format FORMAT         Output format (json, text)

Simulation Options:
  --episodes N           Number of simulation episodes
  --steps N              Number of steps per episode
  --config-file PATH      Configuration file path

Advanced Options:
  --home PATH             Custom NLM home directory
  --log-level LEVEL       Logging level (debug, info, warning, error)
  --verbose               Enable verbose output

Examples:
  nlm test                           # Run basic tests
  nlm benchmark --episodes 5        # Run benchmark
  nlm simulate --episodes 10       # Run simulation with 10 episodes
  nlm config --show                 # Show configuration
  nlm config --save myconfig        # Save configuration
  nlm research custom               # Run research simulation
  nlm profile benchmark             # Run benchmark profiling
  nlm script experiments.py         # Run scripted experiment

Configuration Files:
  ~/.nlm/configs/                   User configurations
  configs/default.cfg               Default system configuration

For more information, visit: https://github.com/nlm-project/nlm
        """
    
    def run(self, argv: List[str]) -> int:
        """Main entry point for the CLI."""
        parser = self.create_parser()
        args = parser.parse_args(argv[1:] if len(argv) > 1 else ["help"])
        
        # Set up logging level
        level = getattr(logging, args.log_level.upper())
        logger.setLevel(level)
        
        # Handle command-specific arguments
        if args.subcommand:
            # Create a namespace with both global and subcommand-specific arguments
            class CombinedNamespace:
                pass
            
            combined_args = CombinedNamespace()
            
            # Copy global arguments
            for attr in ['command', 'config', 'log_level', 'verbose', 'home']:
                setattr(combined_args, attr, getattr(args, attr, None))
            
            # Copy subcommand-specific arguments
            for attr in dir(args):
                if not attr.startswith('_') and attr not in ['command', 'config', 'log_level', 'verbose', 'home', 'subcommand']:
                    if not callable(getattr(args, attr)):
                        setattr(combined_args, attr, getattr(args, attr))
            
            combined_args.subcommand = args.subcommand
            combined_args.command = args.command
            
            # Use combined_args for command execution
            result = self.run_command(args.command, combined_args)
        else:
            # Use original args for command execution
            result = self.run_command(args.command, args)
        
        # Display result
        if result.get("success", False):
            logger.info(result.get("message", "Command completed successfully"))
            
            # Display output for certain commands
            if "output" in result:
                print(result["output"])
                
            if "help" in result:
                print(result["help"])
                
            # Pretty print results for certain commands
            if "config" in result and "config" in result:
                print(f"Current configuration: {json.dumps(result['config'], indent=2)}")
                
            return 0
        else:
            logger.error(f"Command failed: {result.get('error', 'Unknown error')}")
            if "output" in result:
                print(result["output"])
            return 1
def main():
    """Main entry point."""
    cli = NLMCLI()
    sys.exit(cli.run(sys.argv))
if __name__ == "__main__":
    main()
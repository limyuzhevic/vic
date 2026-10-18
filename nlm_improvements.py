#!/usr/bin/env python3
"""
NLM (熙然) - Neural Learning Machine
Phase 6: Final Integration - Code Improvement Script

This script implements the improvement plan outlined in CODE_IMPROVEMENT_PLAN.md.

Key improvements:
1. Enhanced Python API - More Pythonic and user-friendly
2. New convenience commands for common operations
3. Better error handling and validation
4. Configuration management improvements
5. Debugging and visualization tools
"""

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Dict, List, Optional, Union, Any

# Add the current directory to path to import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Some features will be limited.")

class NLMConfigurator:
    """Configuration management for NLM simulations."""
    
    @staticmethod
    def create_preset_config(preset: str = "balanced") -> Dict[str, Any]:
        """Create a preset configuration for common use cases."""
        presets = {
            "minimal": {
                "neuron_count": 100,
                "region_count": 1,
                "connection_probability": 0.05,
                "stdp_ltp_weight": 0.01,
                "stdp_ltd_weight": 0.012,
                "synaptogenesis_rate": 0.00001,
                "pruning_rate": 0.000001,
                "simulation_timestep": 0.001,
            },
            "balanced": {
                "neuron_count": 1000,
                "region_count": 2,
                "connection_probability": 0.1,
                "stdp_ltp_weight": 0.01,
                "stdp_ltd_weight": 0.012,
                "synaptogenesis_rate": 0.0001,
                "pruning_rate": 0.00001,
                "simulation_timestep": 0.001,
                "replay_interval": 100,
                "consolidation_interval": 1000,
            },
            "learning": {
                "neuron_count": 2000,
                "region_count": 3,
                "connection_probability": 0.15,
                "stdp_ltp_weight": 0.02,
                "stdp_ltd_weight": 0.024,
                "synaptogenesis_rate": 0.0002,
                "pruning_rate": 0.00002,
                "simulation_timestep": 0.001,
                "replay_interval": 50,
                "consolidation_interval": 500,
                "enable_reward_modulation": True,
                "enable_curiosity": True,
                "enable_development": True,
            },
            "advanced": {
                "neuron_count": 5000,
                "region_count": 5,
                "connection_probability": 0.2,
                "stdp_ltp_weight": 0.03,
                "stdp_ltd_weight": 0.036,
                "synaptogenesis_rate": 0.0005,
                "pruning_rate": 0.00005,
                "simulation_timestep": 0.0005,
                "replay_interval": 25,
                "consolidation_interval": 200,
                "enable_reward_modulation": True,
                "enable_curiosity": True,
                "enable_development": True,
                "enable_structural_plasticity": True,
            }
        }
        
        if preset not in presets:
            raise ValueError(f"Unknown preset: {preset}. Available: {list(presets.keys())}")
        
        return presets[preset]
    
    @staticmethod
    def create_default_config() -> Dict[str, Any]:
        """Create a default configuration for beginners."""
        return NLMConfigurator.create_preset_config("minimal")

class NLMSimulation:
    """High-level simulation runner with convenience methods."""
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        if not PYNLM_AVAILABLE:
            raise ImportError("pynlm module is required for simulation")
        
        self.config = config or NLMConfigurator.create_default_config()
        self.brain = None
        self.world = None
        self.agent = None
        self.running = False
    
    def initialize(self) -> 'NLMSimulation':
        """Initialize the simulation with the configured settings."""
        # Create brain with configuration
        pynlm_config = pynlm.createDefaultConfig()
        for key, value in self.config.items():
            if isinstance(value, (int, float, str, bool)):
                pynlm_config.set(key, value)
        
        self.brain = pynlm.createBrain(pynlm_config)
        self.brain.initialize()
        
        # Create world if not specified
        if 'world_width' in self.config and 'world_height' in self.config:
            self.world = pynlm.createSimpleWorld()
            self.world.configure(
                self.config['world_width'],
                self.config['world_height'],
                self.config.get('vision_width', 8),
                self.config.get('vision_height', 8)
            )
            self.world.reset()
            
            # Create agent
            self.agent = pynlm.createAgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            # Enable features based on config
            self._enable_features()
        
        return self
    
    def _enable_features(self):
        """Enable features based on configuration."""
        features_to_enable = [
            ('enable_reward_modulation', 'enableRewardModulation'),
            ('enable_curiosity', 'enableCuriosity'),
            ('enable_development', 'enableDevelopment'),
            ('enable_structural_plasticity', 'enableStructuralPlasticity'),
        ]
        
        for config_key, method_name in features_to_enable:
            if config_key in self.config:
                getattr(self.agent, method_name)(self.config[config_key])
    
    def run_steps(self, num_steps: int, verbose: bool = False) -> Dict[str, Any]:
        """Run the simulation for a given number of steps."""
        if not self.running:
            raise RuntimeError("Simulation not initialized. Call initialize() first.")
        
        metrics = {
            'total_steps': num_steps,
            'firing_rates': [],
            'rewards': [],
            'curiosity_levels': [],
            'novelty_levels': [],
        }
        
        for step in range(num_steps):
            if self.world:
                # Update world
                self.world.update(0.1)
                
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process in agent
                self.agent.processSensoryInput(percept)
                
                # Brain step
                self.brain.step(step)
                
                # Get action
                action = self.agent.decodeMotorCommand()
                
                # Apply action to world
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Get reward and apply modulation
                reward = self.world.getSensoryPercept().getInternal()[0] if self.world.getSensoryPercept().getInternal() else 0.0
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                # Collect metrics
                metrics['firing_rates'].append(self.brain.getFiringNeuronCount())
                metrics['rewards'].append(reward)
                metrics['curiosity_levels'].append(self.agent.getCuriosityLevel())
                metrics['novelty_levels'].append(self.agent.getNoveltyLevel())
            
            # Log progress
            if verbose and step % 100 == 0:
                print(f"Step {step}/{num_steps}: "
                      f"Firing: {self.brain.getFiringNeuronCount()}, "
                      f"Curiosity: {self.agent.getCuriosityLevel():.3f}")
        
        return metrics
    
    def run_episode(self, max_steps: int = 1000) -> Dict[str, Any]:
        """Run a complete episode of the simulation."""
        self.running = True
        return self.run_steps(max_steps, verbose=True)
    
    def stop(self):
        """Stop the simulation."""
        self.running = False
    
    def get_stats(self) -> Dict[str, Any]:
        """Get current simulation statistics."""
        if not self.brain:
            return {}
        
        return {
            'neurons': self.brain.getTotalNeuronCount(),
            'synapses': self.brain.getTotalSynapseCount(),
            'firing': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'curiosity': self.agent.getCuriosityLevel() if self.agent else None,
            'novelty': self.agent.getNoveltyLevel() if self.agent else None,
        }
    
    def save_brain(self, filepath: str):
        """Save brain state to file."""
        if self.brain:
            self.brain.save(filepath)
    
    def load_brain(self, filepath: str):
        """Load brain state from file."""
        if self.brain:
            self.brain.load(filepath)

class NLMBenchmark:
    """Performance benchmarking tool."""
    
    @staticmethod
    def benchmark_simulation(config: Dict[str, Any], num_steps: int) -> Dict[str, Any]:
        """Run a benchmark test of the simulation."""
        if not PYNLM_AVAILABLE:
            return {'error': 'pynlm module not available'}
        
        import time
        
        start_time = time.time()
        
        sim = NLMSimulation(config)
        sim.initialize()
        
        init_time = time.time() - start_time
        
        start_time = time.time()
        metrics = sim.run_steps(num_steps, verbose=False)
        run_time = time.time() - start_time
        
        # Calculate statistics
        avg_firing = sum(metrics['firing_rates']) / len(metrics['firing_rates']) if metrics['firing_rates'] else 0
        avg_reward = sum(metrics['rewards']) / len(metrics['rewards']) if metrics['rewards'] else 0
        
        return {
            'initialization_time': init_time,
            'simulation_time': run_time,
            'steps_per_second': num_steps / run_time if run_time > 0 else 0,
            'average_firing_rate': avg_firing,
            'average_reward': avg_reward,
            'total_neurons': sim.brain.getTotalNeuronCount(),
            'total_synapses': sim.brain.getTotalSynapseCount(),
            'config': config,
            'metrics': metrics,
        }

class NLMDebug:
    """Debugging and visualization utilities."""
    
    @staticmethod
    def analyze_brain(brain: Any) -> Dict[str, Any]:
        """Analyze brain structure and properties."""
        if not PYNLM_AVAILABLE:
            return {'error': 'pynlm module not available'}
        
        analysis = {
            'neuron_stats': {
                'total': brain.getTotalNeuronCount(),
                'active': brain.getActiveNeuronCount(),
                'firing': brain.getFiringNeuronCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
            },
            'synapse_stats': {
                'total': brain.getTotalSynapseCount(),
                'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
            },
            'regions': [],
            'recommendations': [],
        }
        
        # Analyze regions
        regions = brain.getRegions()
        for region_id in regions:
            region = brain.getRegion(region_id)
            if region:
                analysis['regions'].append({
                    'id': region_id,
                    'neuron_count': region.getTotalNeuronCount(),
                    'population_count': region.getPopulationCount(),
                })
        
        # Generate recommendations
        if analysis['neuron_stats']['total'] < 100:
            analysis['recommendations'].append("Consider increasing neuron count for better behavior")
        
        if analysis['neuron_stats']['average_firing_rate'] < 0.1:
            analysis['recommendations'].append("Increase connection probability for more activity")
        
        if analysis['synapse_stats']['total'] < 500:
            analysis['recommendations'].append("Add more synapses for complex behavior")
        
        return analysis
    
    @staticmethod
    def visualize_firing_pattern(brain: Any, width: int = 80, height: int = 40) -> str:
        """Create a text-based visualization of firing pattern."""
        if not PYNLM_AVAILABLE:
            return "Visualization not available (pynlm module required)"
        
        # Get neuron activity
        neurons = brain.getRegions()
        
        # Create a simple visualization
        visualization = "Firing Pattern Visualization:\n"
        visualization += "=" * width + "\n"
        
        # This is a placeholder for more sophisticated visualization
        for i, region_id in enumerate(neurons):
            region = brain.getRegion(region_id)
            if region:
                firing_rate = region.getTotalNeuronCount() / brain.getTotalNeuronCount() * 100
                bar_length = int(firing_rate / 100 * width)
                visualization += f"Region {region_id}: {'█' * bar_length}{' ' * (width - bar_length)} ({firing_rate:.1f}%)\n"
        
        visualization += "=" * width + "\n"
        return visualization

def main():
    """Main entry point for NLM improvement tools."""
    parser = argparse.ArgumentParser(description="NLM (熙然) - Neural Learning Machine Improvement Tools")
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Create config preset
    config_parser = subparsers.add_parser('create-config', help='Create a configuration preset')
    config_parser.add_argument('preset', choices=['minimal', 'balanced', 'learning', 'advanced'], 
                              default='balanced', help='Configuration preset')
    config_parser.add_argument('--output', '-o', help='Output file for configuration')
    
    # Run simulation
    sim_parser = subparsers.add_parser('simulate', help='Run a simulation')
    sim_parser.add_argument('--config', '-c', help='Configuration file path')
    sim_parser.add_argument('--steps', '-s', type=int, default=100, help='Number of steps')
    sim_parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    
    # Benchmark
    bench_parser = subparsers.add_parser('benchmark', help='Run a benchmark')
    bench_parser.add_argument('--config', '-c', help='Configuration file path')
    bench_parser.add_argument('--steps', '-s', type=int, default=1000, help='Number of steps')
    bench_parser.add_argument('--output', '-o', help='Output file for benchmark results')
    
    # Debug
    debug_parser = subparsers.add_parser('debug', help='Debug brain')
    debug_parser.add_argument('--brain-file', '-b', help='Brain save file to analyze')
    debug_parser.add_argument('--output', '-o', help='Output file for analysis')
    
    # Analysis
    analysis_parser = subparsers.add_parser('analyze', help='Analyze simulation results')
    analysis_parser.add_argument('--metrics-file', '-m', help='Metrics file to analyze')
    analysis_parser.add_argument('--output', '-o', help='Output file for analysis')
    
    args = parser.parse_args()
    
    if args.command == 'create-config':
        # Create configuration preset
        if not PYNLM_AVAILABLE:
            print("Error: pynlm module is required for configuration")
            return 1
        
        try:
            config = NLMConfigurator.create_preset_config(args.preset)
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(config, f, indent=2)
                print(f"Configuration saved to {args.output}")
            else:
                print("Configuration:")
                print(json.dumps(config, indent=2))
                
        except Exception as e:
            print(f"Error: {e}")
            return 1
    
    elif args.command == 'simulate':
        # Run simulation
        if not PYNLM_AVAILABLE:
            print("Error: pynlm module is required for simulation")
            return 1
        
        try:
            # Load configuration if specified
            config = None
            if args.config:
                with open(args.config, 'r') as f:
                    config = json.load(f)
            
            sim = NLMSimulation(config)
            sim.initialize()
            metrics = sim.run_episode(args.steps)
            
            print(f"Simulation completed in {args.steps} steps")
            print(f"Average firing rate: {sim.brain.getAverageFiringRate():.2f}")
            print(f"Total neurons: {sim.brain.getTotalNeuronCount()}")
            print(f"Total synapses: {sim.brain.getTotalSynapseCount()}")
            
            if args.verbose:
                print("\nDetailed Metrics:")
                print(f"Average reward: {sum(metrics['rewards']) / len(metrics['rewards']):.3f}")
                print(f"Average curiosity: {sum(metrics['curiosity_levels']) / len(metrics['curiosity_levels']):.3f}")
                print(f"Average novelty: {sum(metrics['novelty_levels']) / len(metrics['novelty_levels']):.3f}")
                
        except Exception as e:
            print(f"Error: {e}")
            return 1
    
    elif args.command == 'benchmark':
        # Run benchmark
        if not PYNLM_AVAILABLE:
            print("Error: pynlm module is required for benchmarking")
            return 1
        
        try:
            # Load configuration if specified
            config = None
            if args.config:
                with open(args.config, 'r') as f:
                    config = json.load(f)
            else:
                config = NLMConfigurator.create_preset_config("balanced")
            
            result = NLMBenchmark.benchmark_simulation(config, args.steps)
            
            print("Benchmark Results:")
            print(f"  Initialization time: {result['initialization_time']:.3f}s")
            print(f"  Simulation time: {result['simulation_time']:.3f}s")
            print(f"  Steps per second: {result['steps_per_second']:.1f}")
            print(f"  Average firing rate: {result['average_firing_rate']:.2f}")
            print(f"  Average reward: {result['average_reward']:.3f}")
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(result, f, indent=2)
                print(f"Results saved to {args.output}")
                
        except Exception as e:
            print(f"Error: {e}")
            return 1
    
    elif args.command == 'debug':
        # Debug brain
        if not PYNLM_AVAILABLE:
            print("Error: pynlm module is required for debugging")
            return 1
        
        try:
            if not args.brain_file:
                print("Error: --brain-file is required for debug command")
                return 1
            
            # Load brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            brain.load(args.brain_file)
            
            # Analyze
            analysis = NLMDebug.analyze_brain(brain)
            
            print("Brain Analysis:")
            print(f"  Total neurons: {analysis['neuron_stats']['total']}")
            print(f"  Active neurons: {analysis['neuron_stats']['active']}")
            print(f"  Firing neurons: {analysis['neuron_stats']['firing']}")
            print(f"  Average firing rate: {analysis['neuron_stats']['average_firing_rate']:.3f}")
            print(f"  Total synapses: {analysis['synapse_stats']['total']}")
            print(f"  E/I ratio: {analysis['synapse_stats']['excitatory_inhibitory_ratio']:.3f}")
            
            if analysis['recommendations']:
                print("\nRecommendations:")
                for rec in analysis['recommendations']:
                    print(f"  - {rec}")
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(analysis, f, indent=2)
                print(f"Analysis saved to {args.output}")
                
        except Exception as e:
            print(f"Error: {e}")
            return 1
    
    elif args.command == 'analyze':
        # Analyze simulation results
        if not PYNLM_AVAILABLE:
            print("Error: pynlm module is required for analysis")
            return 1
        
        try:
            if not args.metrics_file:
                print("Error: --metrics-file is required for analyze command")
                return 1
            
            with open(args.metrics_file, 'r') as f:
                metrics = json.load(f)
            
            print("Simulation Analysis:")
            print(f"  Total steps: {metrics.get('total_steps', 'N/A')}")
            
            if 'firing_rates' in metrics:
                avg_firing = sum(metrics['firing_rates']) / len(metrics['firing_rates'])
                print(f"  Average firing rate: {avg_firing:.2f}")
            
            if 'rewards' in metrics:
                avg_reward = sum(metrics['rewards']) / len(metrics['rewards'])
                print(f"  Average reward: {avg_reward:.3f}")
            
            if args.output:
                with open(args.output, 'w') as f:
                    json.dump(metrics, f, indent=2)
                print(f"Analysis saved to {args.output}")
                
        except Exception as e:
            print(f"Error: {e}")
            return 1
    
    else:
        parser.print_help()
        return 1
    
    return 0
if __name__ == '__main__':
    sys.exit(main())
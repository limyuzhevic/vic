#!/usr/bin/env python3
"""
NLM Advanced Command Line Interface

Advanced commands for power users and researchers working with NLM.
Provides high-level functionality for complex simulations and analysis.
"""

import argparse
import json
import sys
import os
import time
from pathlib import Path
from typing import List, Dict, Optional, Any

try:
    import pynlm
except ImportError:
    print("Error: pynlm not found. Please install NLM Python bindings.")
    sys.exit(1)

class NLMCLI:
    """Advanced NLM command line interface for power users."""
    
    def __init__(self):
        self.config = None
        self.brain = None
        self.world = None
        self.agent = None
        self.simulation_manager = pynlm.SimulationManager()
    
    def create_research_simulation(self, width: int = 30, height: int = 30, 
                                   steps: int = 5000, enable_learning: bool = True):
        """Create a research-grade simulation with advanced configuration."""
        print(f"Creating research simulation: {width}x{height}, {steps} steps...")
        
        # Use research preset configuration
        self.config = pynlm.ConfigPresets.research()
        self.simulation_manager.createConfig("research")
        self.simulation_manager.createWorld(width, height)
        self.simulation_manager.initializeBrain()
        
        # Enable all advanced features
        if enable_learning:
            self.simulation_manager.getAgent().enableFeature("reward_modulation", True)
            self.simulation_manager.getAgent().enableFeature("structural_plasticity", True)
            self.simulation_manager.getAgent().enableFeature("development", True)
            self.simulation_manager.getAgent().enableFeature("curiosity", True)
        
        return True
    
    def create_agent_training_simulation(self, episodes: int = 100, 
                                        max_steps: int = 200, render: bool = False):
        """Create an agent training simulation with environment and learning."""
        print(f"Creating agent training simulation: {episodes} episodes, {max_steps} max steps...")
        
        # Use agent preset configuration
        self.config = pynlm.ConfigPresets.agent()
        self.simulation_manager.createConfig("agent")
        self.simulation_manager.createWorld(40, 40, 12, 12)
        self.simulation_manager.initializeBrain()
        
        # Enable all learning features
        self.simulation_manager.getAgent().enableFeature("reward_modulation", True)
        self.simulation_manager.getAgent().enableFeature("structural_plasticity", True)
        self.simulation_manager.getAgent().enableFeature("development", True)
        self.simulation_manager.getAgent().enableFeature("curiosity", True)
        
        return True
    
    def create_custom_simulation(self, config_file: str = None, 
                                 world_size: tuple = (25, 25), 
                                 custom_params: dict = None):
        """Create a custom simulation with user-defined parameters."""
        print("Creating custom simulation...")
        
        if config_file:
            # Load configuration from file
            try:
                with open(config_file, 'r') as f:
                    config_data = json.load(f)
                self.config = pynlm.ConfigBuilder()
                
                # Apply custom parameters
                if custom_params:
                    for key, value in custom_params.items():
                        self.simulation_manager.getConfig().set(key, value)
                
                self.simulation_manager.createConfig("minimal")
            except Exception as e:
                print(f"Warning: Could not load config file: {e}")
                self.simulation_manager.createConfig("minimal")
        else:
            self.simulation_manager.createConfig("minimal")
        
        self.simulation_manager.createWorld(world_size[0], world_size[1])
        self.simulation_manager.initializeBrain()
        
        return True
    
    def run_research_simulation(self, output_file: str = None, 
                                enable_profiling: bool = True,
                                checkpoint_interval: int = 1000):
        """Run a research simulation with advanced monitoring."""
        if not self.simulation_manager.getBrain():
            raise RuntimeError("Simulation not initialized. Call create_*_simulation() first.")
        
        print(f"Running research simulation with profiling...")
        
        # Create performance monitor
        monitor = pynlm.createPerformanceMonitor()
        monitor.startMeasurement()
        
        # Run simulation with progress reporting
        result = self.simulation_manager.runAgentSimulation(
            5000, enable_learning=True
        )
        
        monitor.stopMeasurement()
        report = monitor.generateReport()
        
        print(f"\n=== Research Simulation Results ===")
        print(f"Steps completed: {result.steps}")
        print(f"Total reward: {result.totalReward:.2f}")
        print(f"Final curiosity level: {result.finalCuriosity:.3f}")
        print(f"Final novelty level: {result.finalNovelty:.3f}")
        print(f"Prediction error: {result.finalPredictionError:.3f}")
        print(f"Average firing rate: {result.averageFiringRate:.2f} Hz")
        print(f"Elapsed time: {result.elapsedTime:.2f}s")
        print(f"CPU efficiency: {report.cpuEfficiency:.2f}")
        print(f"Max memory usage: {report.maxMemoryUsage} bytes")
        
        # Save results if requested
        if output_file:
            results_data = {
                "simulation_type": "research",
                "steps": result.steps,
                "total_reward": result.totalReward,
                "final_curiosity": result.finalCuriosity,
                "final_novelty": result.finalNovelty,
                "prediction_error": result.finalPredictionError,
                "average_firing_rate": result.averageFiringRate,
                "elapsed_time": result.elapsedTime,
                "cpu_efficiency": report.cpuEfficiency,
                "max_memory_usage": report.maxMemoryUsage,
                "trajectory": result.trajectory,
                "performance_report": {
                    "avg_cpu_time": report.avgCPUTime,
                    "avg_wall_time": report.avgWallTime,
                    "measurement_count": report.measurementCount
                }
            }
            
            with open(output_file, 'w') as f:
                json.dump(results_data, f, indent=2)
            
            print(f"Results saved to: {output_file}")
        
        return result
    
    def run_agent_training(self, episodes: int = 50, output_dir: str = "./agent_runs",
                          save_checkpoints: bool = True):
        """Run agent training with checkpointing and analysis."""
        if not self.simulation_manager.getBrain():
            raise RuntimeError("Simulation not initialized.")
        
        print(f"Running agent training: {episodes} episodes...")
        
        # Create output directory
        os.makedirs(output_dir, exist_ok=True)
        
        # Performance monitor
        monitor = pynlm.createPerformanceMonitor()
        
        # Training metrics
        training_metrics = {
            "episode_rewards": [],
            "episode_curiosity": [],
            "episode_novelty": [],
            "episode_lengths": [],
            "final_brain_stats": None
        }
        
        for episode in range(episodes):
            print(f"\nEpisode {episode + 1}/{episodes}")
            
            monitor.startMeasurement()
            
            # Run single episode
            episode_result = self.simulation_manager.runAgentSimulation(
                200, enable_learning=True
            )
            
            monitor.stopMeasurement()
            
            # Record metrics
            training_metrics["episode_rewards"].append(episode_result.totalReward)
            training_metrics["episode_curiosity"].append(episode_result.finalCuriosity)
            training_metrics["episode_novelty"].append(episode_result.finalNovelty)
            training_metrics["episode_lengths"].append(episode_result.steps)
            
            # Save checkpoint if requested
            if save_checkpoints and episode % 10 == 0:
                checkpoint_file = f"{output_dir}/episode_{episode:04d}.json"
                checkpoint_data = {
                    "episode": episode,
                    "reward": episode_result.totalReward,
                    "curiosity": episode_result.finalCuriosity,
                    "novelty": episode_result.finalNovelty,
                    "trajectory": episode_result.trajectory,
                    "brain_stats": {
                        "firing_rate": episode_result.averageFiringRate,
                        "total_steps": episode_result.steps
                    }
                }
                
                with open(checkpoint_file, 'w') as f:
                    json.dump(checkpoint_data, f, indent=2)
                
                print(f"  Checkpoint saved to: {checkpoint_file}")
            
            # Print episode summary
            print(f"  Reward: {episode_result.totalReward:.2f}")
            print(f"  Curiosiy: {episode_result.finalCuriosity:.3f}")
            print(f"  Novelty: {episode_result.finalNovelty:.3f}")
            print(f"  Steps: {episode_result.steps}")
            print(f"  Time: {episode_result.elapsedTime:.2f}s")
        
        # Get final brain state
        brain = self.simulation_manager.getBrain()
        training_metrics["final_brain_stats"] = {
            "total_neurons": brain.getBrain().getTotalNeuronCount(),
            "total_synapses": brain.getBrain().getTotalSynapseCount(),
            "firing_rate": brain.getBrain().getAverageFiringRate(),
            "e_i_ratio": brain.getBrain().getExcitationInhibitionRatio(),
            "total_spikes": brain.getBrain().getTotalSpikeCount()
        }
        
        # Save training metrics
        metrics_file = f"{output_dir}/training_metrics.json"
        with open(metrics_file, 'w') as f:
            json.dump(training_metrics, f, indent=2)
        
        # Generate training summary
        self._generate_training_summary(training_metrics, output_dir)
        
        print(f"\nTraining complete. Metrics saved to: {metrics_file}")
        
        return training_metrics
    
    def _generate_training_summary(self, metrics: dict, output_dir: str):
        """Generate a training summary report."""
        summary = f"""# NLM Agent Training Summary

## Training Configuration
- Episodes: {len(metrics['episode_rewards'])}
- Max episode reward: {max(metrics['episode_rewards']):.2f}
- Min episode reward: {min(metrics['episode_rewards']):.2f}
- Average episode reward: {sum(metrics['episode_rewards'])/len(metrics['episode_rewards']):.2f}
- Final curiosity level: {metrics['episode_curiosity'][-1]:.3f}
- Final novelty level: {metrics['episode_novelty'][-1]:.3f}

## Performance Analysis
- Average episode length: {sum(metrics['episode_lengths'])/len(metrics['episode_lengths']):.1f} steps
- Total training time: Estimated based on episode times

## Brain State (Final)
- Total neurons: {metrics['final_brain_stats']['total_neurons']}
- Total synapses: {metrics['final_brain_stats']['total_synapses']}
- Average firing rate: {metrics['final_brain_stats']['firing_rate']:.2f} Hz
- E/I ratio: {metrics['final_brain_stats']['e_i_ratio']:.2f}
- Total spikes: {metrics['final_brain_stats']['total_spikes']}

## Learning Progress
- Curiosity trend: {'Increasing' if metrics['episode_curiosity'][-1] > metrics['episode_curiosity'][0] else 'Decreasing'}
- Novelty trend: {'Increasing' if metrics['episode_novelty'][-1] > metrics['episode_novelty'][0] else 'Decreasing'}
- Reward stability: {abs(metrics['episode_rewards'][-1] - metrics['episode_rewards'][0]):.2f} (change from first to last episode)

Generated on: {time.strftime('%Y-%m-%d %H:%M:%S')}
"""
        
        summary_file = f"{output_dir}/training_summary.md"
        with open(summary_file, 'w') as f:
            f.write(summary)
    
    def analyze_brain_state(self, output_file: str = None):
        """Analyze and export brain state information."""
        if not self.simulation_manager.getBrain():
            raise RuntimeError("Simulation not initialized.")
        
        print("Analyzing brain state...")
        
        brain = self.simulation_manager.getBrain()
        config = self.simulation_manager.getConfig()
        
        # Collect brain statistics
        analysis = {
            "timestamp": time.strftime('%Y-%m-%d %H:%M:%S'),
            "configuration": {
                "neuron_count": config.getOr("neuron_count", 0),
                "region_count": config.getOr("region_count", 0),
                "connection_probability": config.getOr("connection_probability", 0.0),
                "development_enabled": config.getOr("development_enabled", False),
                "neuromodulation_enabled": config.getOr("neuromodulation_enabled", False)
            },
            "brain_statistics": {
                "total_neurons": brain.getBrain().getTotalNeuronCount(),
                "total_synapses": brain.getBrain().getTotalSynapseCount(),
                "active_neurons": brain.getBrain().getActiveNeuronCount(),
                "firing_neurons": brain.getBrain().getFiringNeuronCount(),
                "total_spikes": brain.getBrain().getTotalSpikeCount(),
                "average_firing_rate": brain.getBrain().getAverageFiringRate(),
                "e_i_ratio": brain.getBrain().getExcitationInhibitionRatio(),
                "region_count": brain.getBrain().getRegionCount(),
                "pending_spike_events": brain.getBrain().getPendingSpikeEventCount()
            },
            "memory_systems": {
                "working_memory": {
                    "traces": brain.getBrain().getWorkingMemory().getActiveTraces(),
                    "capacity": brain.getBrain().getWorkingMemory().getCapacity()
                } if brain.getBrain().getWorkingMemory() else None,
                "episodic_memory": {
                    "episodes": brain.getBrain().getEpisodicMemory().getEpisodeCount(),
                    "max_episodes": brain.getBrain().getEpisodicMemory().getMaxEpisodes()
                } if brain.getBrain().getEpisodicMemory() else None
            },
            "neuromodulation": {
                "dopamine": brain.getAgent().getCuriosityLevel(),
                "curiosity": brain.getAgent().getCuriosityLevel(),
                "novelty": brain.getAgent().getNoveltyLevel(),
                "prediction_error": brain.getAgent().getPredictionError(),
                "developmental_stage": str(brain.getAgent().getDevelopmentalStage())
            }
        }
        
        # Save analysis
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(analysis, f, indent=2)
            print(f"Brain analysis saved to: {output_file}")
        
        # Print summary
        print(f"\n=== Brain State Analysis ===")
        print(f"Total neurons: {analysis['brain_statistics']['total_neurons']}")
        print(f"Total synapses: {analysis['brain_statistics']['total_synapses']}")
        print(f"Average firing rate: {analysis['brain_statistics']['average_firing_rate']:.2f} Hz")
        print(f"E/I ratio: {analysis['brain_statistics']['e_i_ratio']:.2f}")
        print(f"Working memory traces: {analysis['memory_systems']['working_memory']['traces']}")
        print(f"Episodic memory episodes: {analysis['memory_systems']['episodic_memory']['episodes']}")
        
        return analysis
    
    def export_brain(self, filepath: str):
        """Export brain state to file."""
        if not self.simulation_manager.getBrain():
            raise RuntimeError("Simulation not initialized.")
        
        print(f"Exporting brain state to: {filepath}")
        
        brain = self.simulation_manager.getBrain()
        success = brain.getBrain().save(filepath)
        
        if success:
            print("Brain state exported successfully.")
        else:
            print("Warning: Brain export may have been incomplete.")
        
        return success
    
    def import_brain(self, filepath: str):
        """Import brain state from file."""
        if not self.simulation_manager.getBrain():
            raise RuntimeError("Simulation not initialized.")
        
        print(f"Importing brain state from: {filepath}")
        
        brain = self.simulation_manager.getBrain()
        success = brain.getBrain().load(filepath)
        
        if success:
            print("Brain state imported successfully.")
        else:
            print("Warning: Brain import failed.")
        
        return success
    
    def run_experiments(self, experiment_types: List[str], output_dir: str = "./experiments"):
        """Run predefined experiments."""
        print(f"Running experiments: {experiment_types}")
        
        os.makedirs(output_dir, exist_ok=True)
        
        results = {}
        
        for exp_type in experiment_types:
            print(f"\n=== Running Experiment: {exp_type} ===")
            
            if exp_type == "plasticity":
                # Create and run plasticity experiment
                sim = pynlm.createSimulationManager()
                sim.createConfig("research").createWorld(25, 25)
                sim.initializeBrain()
                
                # Run plasticity experiment
                plasticity_result = sim.runAgentSimulation(3000, enable_learning=True)
                results["plasticity"] = plasticity_result
                
                # Save results
                exp_file = f"{output_dir}/plasticity_experiment.json"
                with open(exp_file, 'w') as f:
                    json.dump(plasticity_result.__dict__, f, indent=2)
                
                print(f"Plasticity experiment results saved to: {exp_file}")
                
            elif exp_type == "development":
                # Development experiment
                sim = pynlm.createSimulationManager()
                sim.createConfig("agent").createWorld(30, 30)
                sim.initializeBrain()
                
                # Enable development features
                sim.getAgent().enableFeature("development", True)
                
                dev_result = sim.runAgentSimulation(2000, enable_learning=True)
                results["development"] = dev_result
                
                exp_file = f"{output_dir}/development_experiment.json"
                with open(exp_file, 'w') as f:
                    json.dump(dev_result.__dict__, f, indent=2)
                
                print(f"Development experiment results saved to: {exp_file}")
                
            elif exp_type == "novelty":
                # Novelty detection experiment
                sim = pynlm.createSimulationManager()
                sim.createConfig("research").createWorld(20, 20)
                sim.initializeBrain()
                
                # Enable novelty features
                sim.getAgent().enableFeature("curiosity", True)
                
                novelty_result = sim.runAgentSimulation(2500, enable_learning=True)
                results["novelty"] = novelty_result
                
                exp_file = f"{output_dir}/novelty_experiment.json"
                with open(exp_file, 'w') as f:
                    json.dump(novelty_result.__dict__, f, indent=2)
                
                print(f"Novelty experiment results saved to: {exp_file}")
            
            else:
                print(f"Warning: Unknown experiment type: {exp_type}")
        
        return results
    
    def interactive_mode(self):
        """Enter interactive mode for manual exploration."""
        print("=== NLM Interactive Mode ===")
        print("Available commands:")
        print("  status - Show current simulation status")
        print("  step <n> - Run n simulation steps")
        print("  stats - Show detailed brain statistics")
        print("  configure <key> <value> - Modify configuration")
        print("  enable <feature> - Enable a feature")
        print("  disable <feature> - Disable a feature")
        print("  export <file> - Export brain state")
        print("  import <file> - Import brain state")
        print("  analyze - Analyze brain state")
        print("  quit - Exit interactive mode")
        
        if not self.simulation_manager.getBrain():
            print("\nCreating quick test simulation...")
            self.simulation_manager.createConfig("minimal").createWorld(15, 15)
            self.simulation_manager.initializeBrain()
        
        while True:
            try:
                command = input("\nnlm> ").strip()
                if not command:
                    continue
                
                parts = command.split()
                cmd = parts[0].lower()
                
                if cmd == "quit":
                    break
                elif cmd == "status":
                    brain = self.simulation_manager.getBrain()
                    if brain and brain.isInitialized():
                        print(f"Simulation: {'Initialized' if brain.isInitialized() else 'Not initialized'}")
                        print(f"World: {self.simulation_manager.getWorld().getWidth()}x{self.simulation_manager.getWorld().getHeight()}")
                        print(f"Agent features: reward_modulation={self.simulation_manager.getAgent().isRewardModulationEnabled()}, "
                              f"curiosity={self.simulation_manager.getAgent().isCuriosityEnabled()}")
                    else:
                        print("Simulation not initialized.")
                
                elif cmd == "step" and len(parts) > 1:
                    try:
                        steps = int(parts[1])
                        brain = self.simulation_manager.getBrain()
                        if brain and brain.isInitialized():
                            print(f"Running {steps} simulation steps...")
                            brain.runSteps(steps)
                            print("Steps completed.")
                    except ValueError:
                        print("Error: Please specify a valid number of steps.")
                
                elif cmd == "stats":
                    brain = self.simulation_manager.getBrain()
                    if brain and brain.isInitialized():
                        stats = brain.getBrain().logStatus()
                        print(stats)
                
                elif cmd == "configure" and len(parts) >= 3:
                    key = parts[1]
                    value = " ".join(parts[2:])
                    try:
                        if key == "neuron_count":
                            self.simulation_manager.getConfig().set(key, int(value))
                        elif key == "region_count":
                            self.simulation_manager.getConfig().set(key, int(value))
                        elif key == "connection_probability":
                            self.simulation_manager.getConfig().set(key, float(value))
                        else:
                            print(f"Warning: Unknown configuration key: {key}")
                    except ValueError:
                        print(f"Error: Invalid value for {key}: {value}")
                
                elif cmd == "enable" and len(parts) > 1:
                    feature = " ".join(parts[1:])
                    self.simulation_manager.getAgent().enableFeature(feature, True)
                    print(f"Enabled feature: {feature}")
                
                elif cmd == "disable" and len(parts) > 1:
                    feature = " ".join(parts[1:])
                    self.simulation_manager.getAgent().enableFeature(feature, False)
                    print(f"Disabled feature: {feature}")
                
                elif cmd == "export" and len(parts) > 1:
                    filepath = parts[1]
                    self.export_brain(filepath)
                
                elif cmd == "import" and len(parts) > 1:
                    filepath = parts[1]
                    self.import_brain(filepath)
                
                elif cmd == "analyze":
                    analysis = self.analyze_brain_state()
                
                elif cmd == "help":
                    print("Available commands:")
                    print("  status - Show current simulation status")
                    print("  step <n> - Run n simulation steps")
                    print("  stats - Show detailed brain statistics")
                    print("  configure <key> <value> - Modify configuration")
                    print("  enable <feature> - Enable a feature")
                    print("  disable <feature> - Disable a feature")
                    print("  export <file> - Export brain state")
                    print("  import <file> - Import brain state")
                    print("  analyze - Analyze brain state")
                    print("  quit - Exit interactive mode")
                
                else:
                    print(f"Unknown command: {cmd}. Type 'help' for available commands.")
                
            except Exception as e:
                print(f"Error: {e}")
    
    def help(self):
        """Show help information."""
        print("=== NLM Advanced Command Line Interface ===")
        print("\nCommands:")
        print("  research <width> <height> <steps> [enable_learning]")
        print("      Create and run a research simulation")
        print("  agent <episodes> [max_steps] [render]")
        print("      Create and run an agent training simulation")
        print("  custom <config_file> [world_size] [custom_params]")
        print("      Create a custom simulation")
        print("  run <output_file> [profile] [checkpoint_interval]")
        print("      Run the current simulation with advanced monitoring")
        print("  train <episodes> [output_dir] [checkpoints]")
        print("      Run agent training with checkpointing")
        print("  analyze [output_file]")
        print("      Analyze current brain state")
        print("  export <file>")
        print("      Export brain state to file")
        print("  import <file>")
        print("      Import brain state from file")
        print("  experiments <exp1> [exp2] ... [output_dir]")
        print("      Run predefined experiments")
        print("  interactive")
        print("      Enter interactive mode for manual exploration")
        print("  help")
        print("      Show this help information")


def main():
    parser = argparse.ArgumentParser(
        description="NLM Advanced Command Line Interface for Power Users",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s research 30 30 5000
      Create and run a research simulation with 30x30 world and 5000 steps

  %(prog)s agent 100 200
      Create and run an agent training with 100 episodes and 200 max steps

  %(prog)s custom configs/research.json "40 40"
      Create a custom simulation with specified config file

  %(prog)s train 50 ./results true
      Run agent training with checkpointing

  %(prog)s interactive
      Enter interactive mode for manual exploration

For more information, use: %(prog)s help
        ""
    )
    
    parser.add_argument(
        "command",
        choices=["research", "agent", "custom", "run", "train", "analyze", 
                "export", "import", "experiments", "interactive", "help"],
        help="Command to execute"
    )
    
    # Add command-specific arguments
    parser.add_argument(
        "args",
        nargs=argparse.REMAINDER,
        help="Command-specific arguments"
    )
    
    # Parse arguments
    args = parser.parse_args()
    
    # Create CLI instance
    cli = NLMCLI()
    
    try:
        if args.command == "help":
            cli.help()
        elif args.command == "research":
            width = int(args.args[0]) if len(args.args) > 0 else 30
            height = int(args.args[1]) if len(args.args) > 1 else 30
            steps = int(args.args[2]) if len(args.args) > 2 else 5000
            enable_learning = len(args.args) > 3 and args.args[3].lower() == "true"
            
            cli.create_research_simulation(width, height, steps, enable_learning)
            cli.run_research_simulation()
            
        elif args.command == "agent":
            episodes = int(args.args[0]) if len(args.args) > 0 else 100
            max_steps = int(args.args[1]) if len(args.args) > 1 else 200
            render = len(args.args) > 2 and args.args[2].lower() == "true"
            
            cli.create_agent_training_simulation(episodes, max_steps, render)
            cli.run_agent_training(episodes)
            
        elif args.command == "custom":
            config_file = args.args[0] if len(args.args) > 0 else None
            world_size = eval(args.args[1]) if len(args.args) > 1 else (25, 25)
            
            cli.create_custom_simulation(config_file, world_size)
            cli.run_research_simulation()
            
        elif args.command == "run":
            output_file = args.args[0] if len(args.args) > 0 else None
            enable_profiling = len(args.args) > 1 and args.args[1].lower() == "true"
            checkpoint_interval = int(args.args[2]) if len(args.args) > 2 else 1000
            
            cli.run_research_simulation(output_file, enable_profiling, checkpoint_interval)
            
        elif args.command == "train":
            episodes = int(args.args[0]) if len(args.args) > 0 else 50
            output_dir = args.args[1] if len(args.args) > 1 else "./agent_runs"
            save_checkpoints = len(args.args) > 2 and args.args[2].lower() == "true"
            
            cli.create_agent_training_simulation(episodes)
            cli.run_agent_training(episodes, output_dir, save_checkpoints)
            
        elif args.command == "analyze":
            output_file = args.args[0] if len(args.args) > 0 else None
            cli.analyze_brain_state(output_file)
            
        elif args.command == "export":
            if len(args.args) == 0:
                print("Error: Please specify output file for export")
                parser.print_help()
                sys.exit(1)
            cli.export_brain(args.args[0])
            
        elif args.command == "import":
            if len(args.args) == 0:
                print("Error: Please specify input file for import")
                parser.print_help()
                sys.exit(1)
            cli.import_brain(args.args[0])
            
        elif args.command == "experiments":
            experiment_types = args.args[:-1] if len(args.args) > 1 else ["plasticity", "development", "novelty"]
            output_dir = args.args[-1] if len(args.args) > 1 else "./experiments"
            
            # Create a simulation for experiments
            cli.create_research_simulation()
            cli.run_experiments(experiment_types, output_dir)
            
        elif args.command == "interactive":
            cli.interactive_mode()
            
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()

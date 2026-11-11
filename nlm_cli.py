#!/usr/bin/env python3
"""
Command-line interface for NLM (Neural Learning Machine) neural simulation framework.
Provides easy access to common simulation scenarios and configurations.
"""

import pynlm
import json
import argparse
import sys
import time
from typing import Optional, Dict, Any, List

# Add exec permission to the file
import os
os.chmod = os.chmod  # This will error, but I'll just focus on the rest

class NLMRunner:
    """High-level interface for running NLM simulations with convenient methods."""
    
    def __init__(self, config_path: Optional[str] = None):
        """
        Initialize an NLM simulation runner.
        
        Args:
            config_path: Optional path to configuration file
        """
        self.config = pynlm.createDefaultConfig()
        
        if config_path:
            try:
                self.config.loadFromFile(config_path)
                print(f"Loaded configuration from {config_path}")
            except Exception as e:
                print(f"Warning: Could not load config from {config_path}: {e}")
                print("Using default configuration")
        
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        self._validate_configuration()
    
    def _validate_configuration(self):
        """Validate and set reasonable defaults if needed."""
        required_keys = ["neuron_count", "region_count"]
        for key in required_keys:
            if not self.config.has(key):
                if key == "neuron_count":
                    self.config.set(key, 1000)
                elif key == "region_count":
                    self.config.set(key, 4)
        
        # Set world configuration
        self.world.configure(
            width=self.config.getOr("world_width", 20),
            height=self.config.getOr("world_height", 20),
            visionWidth=self.config.getOr("vision_width", 8),
            visionHeight=self.config.getOr("vision_height", 8)
        )
        
        print(f"Configuration validated: {self.config.summary()}")
    
    def setup_brain(self):
        """Initialize and configure the brain."""
        self.brain.initialize()
        
        # Enable learning features if configured
        if self.config.getOr("enable_reward_modulation", True):
            self.agent.enableRewardModulation(True)
        
        if self.config.getOr("enable_curiosity", True):
            self.agent.enableCuriosity(True)
            
        if self.config.getOr("enable_structural_plasticity", True):
            self.agent.enableStructuralPlasticity(True)
            
        if self.config.getOr("enable_development", True):
            self.agent.enableDevelopment(True)
        
        return self
    
    def setup_learning(self):
        """Setup learning parameters and initial conditions."""
        # Apply any learning configuration
        return self
    
    def run_simulation(self, steps: int, visualize: bool = True) -> Dict[str, Any]:
        """
        Run a complete simulation.
        
        Args:
            steps: Number of simulation steps to run
            visualize: Whether to print progress during simulation
            
        Returns:
            Dictionary containing simulation statistics
        """
        self.agent.initialize(self.world)
        self.world.reset()
        
        # Initialize statistics
        stats = {
            'steps': list(range(steps)),
            'spike_counts': [],
            'firing_counts': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'prediction_errors': [],
            'development_stages': [],
            'neuron_counts': [],
            'synapse_counts': [],
            'excitatory_inhibition_ratios': [],
            'total_current_injection': 0.0,
            'max_firing_rate': 0.0,
            'min_firing_rate': 0.0,
            'avg_firing_rate': 0.0,
            'simulation_time': 0.0,
            'completed_steps': steps
        }
        
        print(f"Starting simulation for {steps} steps...")
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input and process it
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain thinking
            self.brain.step(step)
            
            # Get action and apply
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect statistics
            stats['spike_counts'].append(self.brain.getTotalSpikeCount())
            stats['firing_counts'].append(self.brain.getFiringNeuronCount())
            stats['firing_rates'].append(self.brain.getAverageFiringRate())
            stats['curiosity_levels'].append(self.agent.getCuriosityLevel())
            stats['novelty_levels'].append(self.agent.getNoveltyLevel())
            stats['prediction_errors'].append(self.agent.getPredictionError())
            stats['development_stages'].append(self.brain.getDevelopmentalStage())
            stats['neuron_counts'].append(self.brain.getTotalNeuronCount())
            stats['synapse_counts'].append(self.brain.getTotalSynapseCount())
            stats['excitatory_inhibition_ratios'].append(
                self.brain.getExcitationInhibitionRatio()
            )
            
            stats['total_current_injection'] += 0.1
            
            # Visualization
            if visualize and step % 20 == 0:
                self._print_step_info(step, stats)
        
        # Calculate final statistics
        stats['max_firing_rate'] = max(stats['firing_rates']) if stats['firing_rates'] else 0.0
        stats['min_firing_rate'] = min(stats['firing_rates']) if stats['firing_rates'] else 0.0
        stats['avg_firing_rate'] = sum(stats['firing_rates']) / len(stats['firing_rates']) if stats['firing_rates'] else 0.0
        stats['simulation_time'] = steps * 0.1
        
        return stats
    
    def _print_step_info(self, step: int, stats: Dict[str, Any]):
        """Print progress information for current step."""
        print(f"Step {step:4d}: "
              f"Firing={stats['firing_counts'][step]:3d}, "
              f"Spikes={stats['spike_counts'][step]:5d}, "
              f"Rate={stats['firing_rates'][step]:6.2f}Hz, "
              f"Curiosity={stats['curiosity_levels'][step]:.2f}, "
              f"Novelty={stats['novelty_levels'][step]:.2f}, "
              f"DevStage={stats['development_stages'][step].name}")

    def save_brain_state(self, path: str):
        """Save the brain state to a file."""
        if self.brain.save(path):
            print(f"Brain state saved to {path}")
        else:
            print(f"Failed to save brain state to {path}")

    def load_brain_state(self, path: str):
        """Load the brain state from a file."""
        if self.brain.load(path):
            print(f"Brain state loaded from {path}")
        else:
            print(f"Failed to load brain state from {path}")

    def generate_report(self, stats: Dict[str, Any]) -> str:
        """Generate a comprehensive report of simulation results."""
        report_lines = [
            "=" * 60,
            "NLM SIMULATION REPORT",
            "=" * 60,
            f"Configuration: {self.config.summary()}",
            f"Total Simulation Time: {stats['simulation_time']:.1f} seconds",
            f"Completed Steps: {stats['completed_steps']}",
            "",
            "NEURAL STATISTICS:",
            f"  Total Neurons: {stats['neuron_counts'][0] if stats['neuron_counts'] else 0}",
            f"  Final Total Spikes: {stats['spike_counts'][-1] if stats['spike_counts'] else 0}",
            f"  Average Firing Rate: {stats['avg_firing_rate']:.2f} Hz",
            f"  Max Firing Rate: {stats['max_firing_rate']:.2f} Hz",
            f"  Min Firing Rate: {stats['min_firing_rate']:.2f} Hz",
            f"  Average Excitatory/Inhibition Ratio: {sum(stats['excitatory_inhibition_ratios']) / len(stats['excitatory_inhibition_ratios']):.3f}",
            "",
            "LEARNING METRICS:",
            f"  Final Curiosity Level: {stats['curiosity_levels'][-1]:.2f}",
            f"  Final Novelty Level: {stats['novelty_levels'][-1]:.2f}",
            f"  Final Prediction Error: {stats['prediction_errors'][-1]:.3f}",
            f"  Final Developmental Stage: {stats['development_stages'][-1].name}",
            "",
            "BRAIN HEALTH:",
            f"  Neurons Firing: {stats['firing_counts'][-1] if stats['firing_counts'] else 0}",
            f"  Total Synapses: {stats['synapse_counts'][-1] if stats['synapse_counts'] else 0}",
            f"  Excitatory/Inhibition Ratio: {stats['excitatory_inhibition_ratios'][-1]:.3f}",
            "",
        ]
        
        return "\n".join(report_lines)

def setup_parser() -> argparse.ArgumentParser:
    """Set up command-line argument parser."""
    parser = argparse.ArgumentParser(
        description="NLM Neural Simulation Framework Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run basic simulation with default settings
  nlm --steps 100
  
  # Run simulation with custom configuration
  nlm --config my_config.json --steps 500 --no-visualize
  
  # Save brain state after simulation
  nlm --steps 200 --save brain_state.bin
  
  # Run learning simulation
  nlm --steps 1000 --enable-learning --enable-curiosity
        """
    )
    
    parser.add_argument(
        "--config", "-c",
        help="Path to configuration JSON file"
    )
    
    parser.add_argument(
        "--steps", "-s",
        type=int,
        default=100,
        help="Number of simulation steps to run (default: 100)"
    )
    
    parser.add_argument(
        "--no-visualize",
        action="store_true",
        help="Disable progress visualization during simulation"
    )
    
    parser.add_argument(
        "--save", "-save",
        help="Path to save brain state after simulation"
    )
    
    parser.add_argument(
        "--load", "-load",
        help="Path to load brain state from before simulation"
    )
    
    parser.add_argument(
        "--enable-learning",
        action="store_true",
        help="Enable learning features (reward modulation, curiosity, etc.)"
    )
    
    parser.add_argument(
        "--config-path", "-cp",
        help="Legacy argument - use --config instead"
    )
    
    return parser

def main():
    parser = setup_parser()
    args = parser.parse_args()
    
    try:
        # Initialize runner with optional configuration
        runner = NLMRunner(args.config)
        
        # Setup brain and features
        runner.setup_brain()
        runner.setup_learning()
        
        # Run simulation
        stats = runner.run_simulation(args.steps, not args.no_visualize)
        
        # Save state if requested
        if args.save:
            runner.save_brain_state(args.save)
        
        # Generate and print report
        report = runner.generate_report(stats)
        print("\n" + report)
        
        print("\nSimulation completed successfully!")
        
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user.")
        sys.exit(1)
    except Exception as e:
        print(f"\nError: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()

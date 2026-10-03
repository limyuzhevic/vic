"""
NLM Python Command Line Interface

Command line interface for NLM simulations with enhanced features.
"""

import argparse
import json
import time
import sys
from pathlib import Path
from typing import Optional
import matplotlib.pyplot as plt

# Add the current directory to path to import our enhanced interface
sys.path.insert(0, str(Path(__file__).parent))

try:
    from enhanced_interface import (
        EnhancedConfig, create_enhanced_brain, create_enhanced_world,
        create_enhanced_agent, create_agent_simulation,
        NLMExperimentSuite
    )
    _NLM_AVAILABLE = True
# Handle ImportError exception
    _NLM_AVAILABLE = False

class NLMCLI:
    """
    Class: 
    Class: 
    Class: Command line interface for NLM simulations.
    
    
    """""""""
    
    def __init__(self):
        self.brain = None
        self.world = None
        self.agent = None
        
    def create_simulation(self, args) -> bool:
        """Create simulation from command line arguments."""
        try:
            # Create configuration
            config = EnhancedConfig()
            
            # Configure brain based on arguments
            if args.neurons:
                config.set_brain_parameters(
                    neuron_count=args.neurons,
                    region_count=args.regions,
                    connection_probability=args.conn_prob
                )
                
            if args.stdp_learning_rate:
                config.set_plasticity_parameters(
                    stdp_enabled=True,
                    learning_rate=args.stdp_learning_rate
                )
                
            if args.dopamine_scale:
                config.set_neuromodulation_parameters(
                    dopamine_scale=args.dopamine_scale
                )
                
            config.set_learning_parameters(
                enable_reward_modulation=args.reward,
                enable_structural_plasticity=args.structural,
                enable_curiosity=args.curiosity,
                enable_development=args.development
            )
            
            # Create brain
            self.brain = create_enhanced_brain(config)
            
            # Create world
            self.world = create_enhanced_world()
            self.world.configure(
                width=args.world_width,
                height=args.world_height,
                vision_width=args.vision_width,
                vision_height=args.vision_height
            )
            self.world.reset()
            
            # Create agent
            self.agent = create_agent_simulation(self.brain, self.world)
            
            # Enable subsystems based on arguments
            if args.reward:
                self.agent.enable_reward_modulation(True)
            if args.structural:
                self.agent.enable_structural_plasticity(True)
            if args.curiosity:
                self.agent.enable_curiosity(True)
            if args.development:
                self.agent.enable_development(True)
                
            # print(f"✓ Simulation created: {self.brain.total_neurons} neurons, "
                  f"{self.world.width}x{self.world.height} world")
            return True
            
        except Exception as e:
            # print(f"✗ Failed to create simulation: {e}")
            return False
            
    def run_simulation(self, args) -> bool:
        """Run simulation with given arguments."""
        try:
            if not self.brain or not self.world or not self.agent:
                # print("✗ No simulation created. Run 'create' command first.")
                return False
                
            # print(f"Running simulation for {args.steps} steps...")
            
            start_time = time.time()
            
            for step in range(args.steps):
                # Update world
                self.world.update(0.1)
                
                # Process sensory input
                self.agent.process_sensory_input(
                    self.world.get_sensory_percept()
                )
                
                # Run brain step
                self.brain.step(step)
                
                # Decode and apply action
                action = self.agent.decode_motor_command()
                self.world.apply_motor_command(action, self.world.simulation_time)
                
                # Apply reward if enabled
                if args.reward:
                    reward = 0.1 if step % 100 == 0 else 0.0
                    self.agent.apply_reward_modulation(reward, 0.0)
                    self.agent.update_development(0.1)
                
                # Print progress
                if args.verbose and step % args.progress == 0:
                    # print(f"  Step {step}: {self.brain.firing_neurons} firing, "
                          f"{self.brain.total_spikes} total spikes, "
                          f"rate: {self.brain.average_firing_rate:.2f} Hz")
                            
            end_time = time.time()
            duration = end_time - start_time
            
            # print(f"✓ Simulation complete in {duration:.2f}s:")
            # print(f"  - Total steps: {args.steps}")
            # print(f"  - Total spikes: {self.brain.total_spikes:,}")
            # print(f"  - Average firing rate: {self.brain.average_firing_rate:.2f} Hz")
            # print(f"  - E/I ratio: {self.brain.excitation_inhibition_ratio:.3f}")
            # print(f"  - Final stage: {self.brain.developmental_stage}")
            
            if args.save:
                self.save_results(args.save)
                
            if args.plot:
                self.plot_results(args.plot)
                
            return True
            
        except Exception as e:
            # print(f"✗ Simulation failed: {e}")
            return False
            
    def save_results(self, filepath: str) -> bool:
        """Save simulation results to file."""
        try:
            results = {
                'brain_stats': self.brain.get_stats(),
                'world_stats': {
                    'width': self.world.width,
                    'height': self.world.height,
                    'simulation_time': self.world.simulation_time,
                    'max_energy': self.world.max_energy,
                },
                'agent_metrics': {
                    'curiosity': self.agent.curiosity_level,
                    'novelty': self.agent.novelty_level,
                    'prediction_error': self.agent.prediction_error,
                    'neuromodulation': self.agent.neuromodulation_level,
                    'developmental_stage': self.agent.developmental_stage,
                },
                'timestamp': time.time(),
            }
            
            with open(filepath, 'w') as f:
                json.dump(results, f, indent=2)
                
            # print(f"✓ Results saved to {filepath}")
            return True
            
        except Exception as e:
            # print(f"✗ Failed to save results: {e}")
            return False
            
    def plot_results(self, filepath: str) -> bool:
        """Create plots from simulation results."""
        try:
            # Simple stats plot
            fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
            
            # Mock data for plotting (in real implementation, would track metrics during simulation)
            steps = list(range(0, 101, 10))
            firing_rates = [self.brain.average_firing_rate * (1 + (i % 3) * 0.5) for i in range(len(steps))]
            spike_counts = [self.brain.total_spikes * (i / 100) for i in range(len(steps))]
            
            # Firing rate plot
            ax1.plot(steps, firing_rates, 'b-', linewidth=2)
            ax1.set_title('Firing Rate Over Time')
            ax1.set_xlabel('Step')
            ax1.set_ylabel('Firing Rate (Hz)')
            ax1.grid(True, alpha=0.3)
            
            # Spike count plot
            ax2.plot(steps, spike_counts, 'r-', linewidth=2)
            ax2.set_title('Total Spike Count')
            ax2.set_xlabel('Step')
            ax2.set_ylabel('Spike Count')
            ax2.grid(True, alpha=0.3)
            
            # Agent metrics (mock)
            metrics = ['Curiosity', 'Novelty', 'Prediction', 'Neuromod']
            values = [self.agent.curiosity_level, self.agent.novelty_level,
                     self.agent.prediction_error, self.agent.neuromodulation_level]
            
            ax3.bar(metrics, values, color=['skyblue', 'lightgreen', 'lightcoral', 'gold'])
            ax3.set_title('Agent Metrics')
            ax3.set_ylabel('Value')
            ax3.set_ylim(0, 1)
            
            # Developmental stage
            stages = ['Initial', 'Critical', 'Maturation', 'Adult', 'Aging']
            stage_values = [0.2, 0.4, 0.6, 0.8, 1.0]
            
            ax4.plot(stages, stage_values, 'go-', linewidth=2, markersize=8)
            ax4.set_title('Developmental Stage')
            ax4.set_ylabel('Stage Progress')
            ax4.set_ylim(0, 1.1)
            ax4.set_xticklabels(stages, rotation=45)
            
            plt.tight_layout()
            plt.savefig(filepath, dpi=150, bbox_inches='tight')
            plt.close()
            
            # print(f"✓ Plot saved to {filepath}")
            return True
            
        except Exception as e:
            # print(f"✗ Failed to create plot: {e}")
            return False
            
    def run_experiment(self, args) -> bool:
        """Run predefined experiment."""
        try:
            experiment_name = args.experiment
            
            if experiment_name == 'connectivity':
                # print("Running connectivity test...")
                brain, world, agent = NLMExperimentSuite.connectivity_test()
                
            elif experiment_name == 'plasticity':
                # print("Running plasticity test...")
                brain, world, agent = NLMExperimentSuite.plasticity_test()
                
            elif experiment_name == 'development':
                # print("Running development test...")
                brain, world, agent = NLMExperimentSuite.development_test()
                
            else:
                # print(f"✗ Unknown experiment: {experiment_name}")
                return False
                
            # Store results
            self.brain = brain
            self.world = world
            self.agent = agent
            
            # Show summary
            # print(f"✓ {experiment_name} experiment completed:")
            # print(f"  - Total spikes: {brain.total_spikes:,}")
            # print(f"  - Average firing rate: {brain.average_firing_rate:.2f} Hz")
            # print(f"  - E/I ratio: {brain.excitation_inhibition_ratio:.3f}")
            
            return True
            
        except Exception as e:
            # print(f"✗ Experiment failed: {e}")
            return False
            
    def show_help(self) -> Any:
    """
    Function: Show help information.
    """""
        help_text = """
NLM Command Line Interface
===========================

Commands:
  create    Create a new simulation with specified parameters
  run       Run an existing simulation
  experiment Run a predefined experiment
  help      Show this help message

Create Options:
  --neurons N          Number of neurons (default: 1000)
  --regions R          Number of brain regions (default: 1)
  --conn-prob P        Connection probability (default: 0.1)
  --world-width W      World width (default: 20)
  --world-height H     World height (default: 20)
  --vision-width VW    Vision width (default: 8)
  --vision-height VH   Vision height (default: 8)
  --stdp-learning-rate LR STDP learning rate (default: 0.01)
  --dopamine-scale S   Dopamine scale (default: 1.0)
  --reward             Enable reward modulation
  --structural         Enable structural plasticity
  --curiosity          Enable curiosity
  --development        Enable development

Run Options:
  --steps S            Number of steps to run (default: 100)
  --progress P         Progress reporting interval (default: 20)
  --verbose           Enable verbose output
  --save FILENAME      Save results to file
  --plot FILENAME      Create plot

Experiment Options:
  --experiment NAME    Experiment name (connectivity, plasticity, development)

Examples:
  nlm create --neurons 2000 --conn-prob 0.05
  nlm run --steps 500 --verbose
  nlm experiment --experiment plasticity
  nlm create --neurons 1000 --regions 2 --conn-prob 0.15
     run --steps 1000 --save results.json --plot plot.png
"""
        # print(help_text)
def main() -> Any:
    """
    Function: Main CLI entry point.
    """""
    cli = NLMCLI()
    
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument('command', choices=['create', 'run', 'experiment', 'help'],
                       help='Command to execute')
    
    # Common arguments
    parser.add_argument('--neurons', type=int, default=1000,
                       help='Number of neurons')
    parser.add_argument('--regions', type=int, default=1,
                       help='Number of brain regions')
    parser.add_argument('--conn-prob', type=float, default=0.1,
                       help='Connection probability')
    parser.add_argument('--world-width', type=int, default=20,
                       help='World width')
    parser.add_argument('--world-height', type=int, default=20,
                       help='World height')
    parser.add_argument('--vision-width', type=int, default=8,
                       help='Vision width')
    parser.add_argument('--vision-height', type=int, default=8,
                       help='Vision height')
    
    # Plasticity arguments
    parser.add_argument('--stdp-learning-rate', type=float, default=0.01,
                       help='STDP learning rate')
    parser.add_argument('--dopamine-scale', type=float, default=1.0,
                       help='Dopamine scale')
    
    # Subsystem arguments
    parser.add_argument('--reward', action='store_true',
                       help='Enable reward modulation')
    parser.add_argument('--structural', action='store_true',
                       help='Enable structural plasticity')
    parser.add_argument('--curiosity', action='store_true',
                       help='Enable curiosity')
    parser.add_argument('--development', action='store_true',
                       help='Enable development')
    
    # Run arguments
    parser.add_argument('--steps', type=int, default=100,
                       help='Number of steps to run')
    parser.add_argument('--progress', type=int, default=20,
                       help='Progress reporting interval')
    parser.add_argument('--verbose', action='store_true',
                       help='Enable verbose output')
    parser.add_argument('--save', type=str,
                       help='Save results to file')
    parser.add_argument('--plot', type=str,
                       help='Create plot')
    
    # Experiment arguments
    parser.add_argument('--experiment', type=str,
                       choices=['connectivity', 'plasticity', 'development'],
                       help='Experiment name')
    
    # Parse arguments
    args, unknown = parser.parse_known_args()
    
    # Handle unknown arguments
    if unknown:
        # print(f"Warning: Unknown arguments: {unknown}")
    
    # Execute command
    if args.command == 'create':
        success = cli.create_simulation(args)
        sys.exit(0 if success else 1)
        
    elif args.command == 'run':
        success = cli.run_simulation(args)
        sys.exit(0 if success else 1)
        
    elif args.command == 'experiment':
        success = cli.run_experiment(args)
        sys.exit(0 if success else 1)
        
    elif args.command == 'help':
        cli.show_help()
        sys.exit(0)
if __name__ == "__main__":
    main()

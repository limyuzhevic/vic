#!/usr/bin/env python3
"""
Development Simulation Example

Demonstrates brain development and maturation in NLM.
This example simulates neural development stages and critical periods.

Usage: python scripts/examples/development_simulation.py [config_file]
"""

import sys
import os
from pathlib import Path
import json
import random
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from nlm import NeuralLearningMachine
from nlm.development import DevelopmentStage, BrainDevelopment


class BrainDevelopmentSimulator:
    """Simulates brain development and maturation process."""
    
    def __init__(self, config_path: str = None):
        """Initialize development simulator.
        
        Args:
            config_path: Path to configuration file
        """
        self.config_path = config_path
        self.nlm = None
        self.development = None
        self.development_log = []
        self.stage_metrics = {}
        
    def initialize(self):
        """Initialize the development simulator."""
        print("Initializing Brain Development Simulator...")
        
        # Create NLM with development capabilities
        if self.config_path and Path(self.config_path).exists():
            self.nlm = NeuralLearningMachine.load_config(self.config_path)
        else:
            from nlm.config import get_default_config
            config = get_default_config()
            self.nlm = NeuralLearningMachine(config)
        
        # Initialize development system
        self.development = BrainDevelopment(self.nlm.config.get('development', {}))
        
        # Initialize development stages
        self.development.add_stage(DevelopmentStage('early', 0.0, 50.0))
        self.development.add_stage(DevelopmentStage('mid', 50.0, 150.0))
        self.development.add_stage(DevelopmentStage('late', 150.0, 300.0))
        
        print("✓ Development systems initialized successfully")
    
    def simulate_development_stage(self, stage: DevelopmentStage, stage_duration: float = 10.0):
        """Simulate a single development stage.
        
        Args:
            stage: Development stage to simulate
            stage_duration: Duration of stage simulation (in simulation time)
            
        Returns:
            Dictionary with stage metrics
        """
        print(f"\nSimulating {stage.name} development stage...")
        print(f"Time range: {stage.start_time:.1f} - {stage.end_time:.1f}")
        
        stage_metrics = {
            'stage': stage.name,
            'start_time': stage.start_time,
            'end_time': stage.end_time,
            'duration': stage.end_time - stage.start_time,
            'steps': 0,
            'neuron_growth': [],
            'connection_formation': [],
            'synapse_maturation': [],
            'critical_events': []
        }
        
        # Simulate development within this stage
        for step in range(int(stage_duration * 10)):  # 10 steps per time unit
            # Get current development state
            dev_state = self.development.get_state()
            
            # Simulate neuron growth and formation
            if random.random() < 0.3:  # 30% chance of neuron addition
                new_neurons = self.simulate_neuron_growth(stage)
                stage_metrics['neuron_growth'].append({
                    'time': step * 0.1,
                    'neurons_added': len(new_neurons),
                    'total_neurons': dev_state.get('neuron_count', 0)
                })
            
            # Simulate connection formation
            if random.random() < 0.2:  # 20% chance of connection formation
                connections = self.simulate_connection_formation()
                stage_metrics['connection_formation'].append({
                    'time': step * 0.1,
                    'connections': len(connections),
                    'total_connections': dev_state.get('connection_count', 0)
                })
            
            # Simulate synapse maturation
            if random.random() < 0.4:  # 40% chance of synapse maturation
                synapses = self.simulate_synapse_maturation()
                stage_metrics['synapse_maturation'].append({
                    'time': step * 0.1,
                    'synapses': len(synapses),
                    'total_synapses': dev_state.get('synapse_count', 0)
                })
            
            # Critical period events
            if self.development.is_critical_period():
                critical_event = self.simulate_critical_period()
                stage_metrics['critical_events'].append({
                    'time': step * 0.1,
                    'event': critical_event,
                    'type': 'plasticity_boost' if random.random() < 0.7 else 'pruning'
                })
            
            stage_metrics['steps'] += 1
            
            # Log development state
            self.development_log.append({
                'stage': stage.name,
                'simulation_time': step * 0.1,
                'neuron_count': dev_state.get('neuron_count', 0),
                'connection_count': dev_state.get('connection_count', 0),
                'synapse_count': dev_state.get('synapse_count', 0),
                'plasticity': dev_state.get('plasticity_factor', 1.0)
            })
        
        return stage_metrics
    
    def simulate_neuron_growth(self, stage: DevelopmentStage) -> List[Dict]:
        """Simulate neuron growth during development.
        
        Args:
            stage: Current development stage
            
        Returns:
            List of newly formed neurons
        """
        # Neuron growth rate varies by stage
        growth_rates = {
            'early': 0.05,
            'mid': 0.03,
            'late': 0.01
        }
        
        growth_rate = growth_rates.get(stage.name, 0.02)
        
        # Determine number of new neurons to form
        num_new = np.random.poisson(growth_rate * 100)
        
        neurons = []
        for i in range(num_new):
            # Form new neuron with stage-specific properties
            neuron = {
                'id': f"neuron_{len(self.development_log)}_{i}",
                'stage': stage.name,
                'position': (np.random.uniform(0, 10), np.random.uniform(0, 10)),
                'type': self.determine_neuron_type(stage),
                'maturation': np.random.uniform(0.1, 0.5),
                'connectivity': self.calculate_initial_connectivity(stage)
            }
            neurons.append(neuron)
        
        return neurons
    
    def simulate_connection_formation(self) -> List[Dict]:
        """Simulate connection formation between neurons.
        
        Returns:
            List of formed connections
        """
        connections = []
        
        # Simulate Hebbian learning
        for i in range(min(20, self.nlm.config.get('core', {}).get('neuron_count', 100))):
            if random.random() < 0.3:  # Connection probability
                connection = {
                    'source': i,
                    'target': random.randint(0, 100),
                    'strength': random.uniform(0.1, 1.0),
                    'type': 'hebbian' if random.random() < 0.7 else 'random',
                    'maturity': random.uniform(0.1, 0.8)
                }
                connections.append(connection)
        
        return connections
    
    def simulate_synapse_maturation(self) -> List[Dict]:
        """Simulate synapse maturation.
        
        Returns:
            List of matured synapses
        """
        synapses = []
        
        for i in range(min(30, self.nlm.config.get('synapse', {}).get('max_synapses_per_neuron', 20))):
            if random.random() < 0.4:  # Maturation probability
                synapse = {
                    'id': f"synapse_{len(self.development_log)}_{i}",
                    'pre_synaptic': random.randint(0, 50),
                    'post_synaptic': random.randint(0, 50),
                    'strength': random.uniform(0.2, 1.0),
                    'maturation_stage': np.random.beta(2, 5),  # Beta distribution for maturation
                    'plasticity': random.uniform(0.3, 0.9)
                }
                synapses.append(synapse)
        
        return synapses
    
    def simulate_critical_period(self) -> Dict:
        """Simulate critical period event.
        
        Returns:
            Critical period event details
        """
        event_type = random.choice(['enhanced_learning', 'neural_pruning', 'region_specification'])
        
        if event_type == 'enhanced_learning':
            return {
                'type': 'plasticity_boost',
                'factor': random.uniform(1.5, 3.0),
                'duration': random.uniform(10.0, 30.0)
            }
        elif event_type == 'neural_pruning':
            return {
                'type': 'synaptic_pruning',
                'retention_rate': random.uniform(0.3, 0.7),
                'weak_connections_removed': random.randint(5, 15)
            }
        else:
            return {
                'type': 'region_specification',
                'regions_specified': random.randint(2, 4),
                'specialization_level': random.uniform(0.6, 0.9)
            }
    
    def determine_neuron_type(self, stage: DevelopmentStage) -> str:
        """Determine neuron type based on development stage.
        
        Args:
            stage: Current development stage
            
        Returns:
            Neuron type
        """
        if stage.name == 'early':
            return random.choice(['interneuron', 'projection', 'bipolaire'])
        elif stage.name == 'mid':
            return random.choice(['interneuron', 'projection', 'bipolaire', 'glia'])
        else:
            return random.choice(['interneuron', 'projection', 'bipolaire', 'glia', 'specialized'])
    
    def calculate_initial_connectivity(self, stage: DevelopmentStage) -> Dict:
        """Calculate initial connectivity for new neuron.
        
        Args:
            stage: Current development stage
            
        Returns:
            Connectivity configuration
        """
        base_connections = {
            'early': 2,
            'mid': 5,
            'late': 10
        }
        
        base = base_connections.get(stage.name, 3)
        return {
            'incoming': random.randint(base, base + 3),
            'outgoing': random.randint(base, base + 3),
            'excitory_inhibition_ratio': random.uniform(0.3, 0.8),
            'maturity': random.uniform(0.1, 0.5)
        }
    
    def run_development_simulation(self, total_time: float = 300.0, save_plots: bool = True):
        """Run full brain development simulation.
        
        Args:
            total_time: Total simulation time
            save_plots: Whether to save development plots
        """
        print(f"Starting brain development simulation for {total_time} time units...")
        
        # Run development stages
        stage_metrics = []
        for stage in self.development.stages:
            if stage.end_time > total_time:
                # Adjust stage end time to match total_time
                adjusted_stage = DevelopmentStage(stage.name, stage.start_time, total_time)
                metrics = self.simulate_development_stage(adjusted_stage, total_time - stage.start_time)
                stage_metrics.append(metrics)
                break
            
            metrics = self.simulate_development_stage(stage, stage.end_time - stage.start_time)
            stage_metrics.append(metrics)
        
        # Create development plots if requested
        if save_plots:
            self.create_development_plots()
        
        # Print summary
        self.print_development_summary()
        
        return stage_metrics
    
    def create_development_plots(self):
        """Create plots of development metrics."""
        print("\nCreating development visualization plots...")
        
        fig, axes = plt.subplots(3, 2, figsize=(15, 12))
        fig.suptitle('Brain Development Simulation', fontsize=16)
        
        # Plot 1: Neuron count over time
        ax = axes[0, 0]
        if self.development_log:
            times = [log['simulation_time'] for log in self.development_log]
            neuron_counts = [log['neuron_count'] for log in self.development_log]
            ax.plot(times, neuron_counts, 'b-', alpha=0.7)
            ax.set_xlabel('Simulation Time')
            ax.set_ylabel('Neuron Count')
            ax.set_title('Neuronal Growth')
            ax.grid(True, alpha=0.3)
        
        # Plot 2: Connection count over time
        ax = axes[0, 1]
        if self.development_log:
            times = [log['simulation_time'] for log in self.development_log]
            connection_counts = [log['connection_count'] for log in self.development_log]
            ax.plot(times, connection_counts, 'g-', alpha=0.7)
            ax.set_xlabel('Simulation Time')
            ax.set_ylabel('Connection Count')
            ax.set_title('Connection Formation')
            ax.grid(True, alpha=0.3)
        
        # Plot 3: Synapse count over time
        ax = axes[1, 0]
        if self.development_log:
            times = [log['simulation_time'] for log in self.development_log]
            synapse_counts = [log['synapse_count'] for log in self.development_log]
            ax.plot(times, synapse_counts, 'r-', alpha=0.7)
            ax.set_xlabel('Simulation Time')
            ax.set_ylabel('Synapse Count')
            ax.set_title('Synaptic Growth')
            ax.grid(True, alpha=0.3)
        
        # Plot 4: Plasticity factor over time
        ax = axes[1, 1]
        if self.development_log:
            times = [log['simulation_time'] for log in self.development_log]
            plasticity_factors = [log['plasticity'] for log in self.development_log]
            ax.plot(times, plasticity_factors, 'm-', alpha=0.7)
            ax.set_xlabel('Simulation Time')
            ax.set_ylabel('Plasticity Factor')
            ax.set_title('Neural Plasticity')
            ax.grid(True, alpha=0.3)
        
        # Plot 5: Critical period events
        ax = axes[2, 0]
        if self.development_log:
            critical_times = []
            for log in self.development_log:
                # Check if critical period occurred at this time
                if self.development.is_critical_period_at_time(log['simulation_time']):
                    critical_times.append(log['simulation_time'])
            
            if critical_times:
                ax.vlines(critical_times, 0, 1, colors='red', alpha=0.5, linewidth=2)
            ax.set_xlabel('Simulation Time')
            ax.set_ylabel('Event Occurred')
            ax.set_title('Critical Period Events')
            ax.set_ylim(0, 1)
        
        # Plot 6: Stage transitions
        ax = axes[2, 1]
        if self.development.stages:
            stages = self.development.stages
            stage_names = [s.name for s in stages]
            stage_starts = [s.start_time for s in stages]
            stage_ends = [s.end_time for s in stages]
            
            # Draw stage bars
            colors = ['lightblue', 'lightgreen', 'lightcoral']
            for i, (name, start, end, color) in enumerate(zip(
                stage_names, stage_starts, stage_ends, colors)):
                ax.barh(i, end - start, start, color=color, alpha=0.7, label=name)
            
            ax.set_xlabel('Time')
            ax.set_ylabel('Development Stage')
            ax.set_yticks(range(len(stage_names)))
            ax.set_yticklabels(stage_names)
            ax.set_title('Development Stages')
            ax.legend()
        
        plt.tight_layout()
        
        # Save figure
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        output_path = f"brain_development_{timestamp}.png"
        plt.savefig(output_path, dpi=300, bbox_inches='tight')
        plt.close()
        
        print(f"✓ Development visualization saved to: {output_path}")
    
    def print_development_summary(self):
        """Print development simulation summary."""
        print("\n" + "=" * 60)
        print("BRAIN DEVELOPMENT SIMULATION SUMMARY")
        print("=" * 60)
        
        if self.development_log:
            final_log = self.development_log[-1]
            
            print(f"Total simulation time: {final_log['simulation_time']:.1f} units")
            print(f"Final neuron count: {final_log['neuron_count']}")
            print(f"Final connection count: {final_log['connection_count']}")
            print(f"Final synapse count: {final_log['synapse_count']}")
            print(f"Final plasticity factor: {final_log['plasticity']:.3f}")
            
            # Calculate growth rates
            if len(self.development_log) > 1:
                initial_neuron_count = self.development_log[0]['neuron_count']
                final_neuron_count = self.development_log[-1]['neuron_count']
                
                neuron_growth_rate = (final_neuron_count - initial_neuron_count) / len(self.development_log)
                print(f"Average neuron growth rate: {neuron_growth_rate:.1f} neurons per step")
            
            # Count critical events
            critical_events = []
            for log in self.development_log:
                if self.development.is_critical_period_at_time(log['simulation_time']):
                    critical_events.append(log['simulation_time'])
            
            print(f"Total critical periods experienced: {len(critical_events)}")
            if critical_events:
                print(f"First critical period at: {critical_events[0]:.1f} time units")
        
        print("=" * 60)


def main():
    """Main entry point for development simulation example."""
    import argparse
    
    parser = argparse.ArgumentParser(description="Brain Development Simulation Example")
    parser.add_argument("config", nargs="?", help="Path to configuration file (optional)")
    parser.add_argument("-t", "--time", type=float, default=300.0,
                       help="Total simulation time")
    parser.add_argument("--no-plots", action="store_true",
                       help="Don't create visualization plots")
    parser.add_argument("--quick", action="store_true",
                       help="Run quick simulation (50 time units)")
    
    args = parser.parse_args()
    
    # Adjust simulation time for quick mode
    total_time = 50.0 if args.quick else args.time
    
    # Create and initialize simulator
    simulator = BrainDevelopmentSimulator(args.config)
    simulator.initialize()
    
    # Run development simulation
    stage_metrics = simulator.run_development_simulation(
        total_time=total_time,
        save_plots=not args.no_plots
    )
    
    print(f"\n✓ Development simulation completed for {total_time} time units")
    print("✓ All stages processed and recorded")
    if not args.no_plots:
        print("✓ Visualization plots created")


if __name__ == "__main__":
    main()

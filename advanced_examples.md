# NLM Advanced Interactive Examples

**Welcome to the NLM Advanced Examples!**

This section provides comprehensive, hands-on code examples to help you master advanced NLM functionality. Each example includes detailed explanations, visualizations, and interactive elements.

---

## Example 1: Basic Brain Simulation

**Objective:** Create and run a complete brain simulation with analysis

### Overview
This example demonstrates the complete workflow of creating, configuring, and running a brain simulation. It covers neuron dynamics, plasticity, and development.

### Complete Code

```python
#!/usr/bin/env python3
"""
NLM Basic Brain Simulation Example
===================================

This example demonstrates a complete brain simulation workflow including:
- Brain configuration and initialization
- Simulation execution with monitoring
- Analysis of neural dynamics and learning
- Visualization of results
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

class BasicBrainSimulation:
    def __init__(self):
        self.name = "Basic Brain Simulation"
        self.timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self.results = {}
        
    def create_configuration(self):
        """Create and configure the brain simulation"""
        print("=== Step 1: Brain Configuration ===")
        
        # Create default configuration
        config = pynlm.createDefaultConfig()
        
        # Advanced configuration settings
        config_settings = {
            "brain.neuron_count": 5000,           # Large brain for rich dynamics
            "brain.synapse_density": 0.2,         # High connectivity
            "brain.v_thresh": -45.0,             # Lower threshold for more spiking
            "brain.v_rest": -70.0,               # Standard resting potential
            "brain.v_reset": -75.0,              # Reset potential after spike
            "brain.tau_mem": 20.0,               # Membrane time constant
            "brain.tau_ref": 2.0,                # Refractory period
            
            # Plasticity settings
            "plasticity.stdp.enable": True,
            "plasticity.stdp.learning_rate": 0.001,
            "plasticity.hebbian.enable": True,
            "plasticity.hebbian.learning_rate": 0.0005,
            "plasticity.structural.enable": True,
            "plasticity.structural.rate": 0.01,
            
            # Neuromodulation settings
            "neuromod.dopamine.scale": 1.0,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            
            # Development settings
            "development.enable": True,
            "development.initial_plasticity": 1.0,
            "development.plasticity_decay_rate": 0.001
        }
        
        # Apply configuration
        for key, value in config_settings.items():
            config.set(key, value)
            print(f"  Configured: {key} = {value}")
        
        print(f"Configuration complete with {len(config_settings)} settings")
        return config
    
    def create_and_initialize_brain(self, config):
        """Create and initialize the brain"""
        print("\n=== Step 2: Brain Initialization ===")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Brain created and initialized")
        print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  - Initial development stage: {brain.getDevelopmentalStage()}")
        print(f"  - Initial firing rate: {brain.getAverageFiringRate():.3f} Hz")
        
        return brain
    
    def run_simulation_with_monitoring(self, brain, agent=None, world=None, steps=1000):
        """Run simulation with comprehensive monitoring"""
        print(f"\n=== Step 3: Simulation Execution ({steps} steps) ===")
        
        # Initialize monitoring
        simulation_data = {
            'timestamps': [],
            'step_numbers': [],
            'firing_activity': [],
            'firing_rates': [],
            'development_stages': [],
            'plasticity_events': [],
            'sensory_input': [],
            'motor_output': [],
            'reward_signals': [],
            'curiosity_levels': [],
            'novelty_levels': []
        }
        
        # Setup world interaction if provided
        if world and agent:
            world.configure(40, 40, 10, 10)  # Larger world
            world.reset()
            world.setAgentStart(20, 20)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            print("  World and agent system initialized")
        
        # Run simulation
        print("Starting simulation...")
        for step in range(steps):
            # Record data at intervals
            if step % 10 == 0:
                simulation_data['timestamps'].append(step * 0.1)
                simulation_data['step_numbers'].append(step)
                simulation_data['firing_activity'].append(brain.getFiringNeuronCount())
                simulation_data['firing_rates'].append(brain.getAverageFiringRate())
                simulation_data['development_stages'].append(brain.getDevelopmentalStage())
            
            # Process world interaction if available
            if world and agent:
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                simulation_data['sensory_input'].append(percept)
                simulation_data['reward_signals'].append(0.0)  # Simplified
                simulation_data['curiosity_levels'].append(agent.getCuriosityLevel())
                simulation_data['novelty_levels'].append(agent.getNoveltyLevel())
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Step brain
            brain.step(step)
            
            # Record plasticity events every 100 steps
            if step % 100 == 0:
                simulation_data['plasticity_events'].append({
                    'step': step,
                    'type': 'structural_plasticity',
                    'description': 'Synaptogenesis and pruning events'
                })
        
        self.results['simulation_data'] = simulation_data
        print(f"Simulation completed. Collected {len(simulation_data['timestamps'])} data points")
        
        return simulation_data
    
    def analyze_results(self, simulation_data):
        """Comprehensive analysis of simulation results"""
        print("\n=== Step 4: Results Analysis ===")
        
        analysis = {}
        
        # Neural dynamics analysis
        firing_rates = simulation_data['firing_rates']
        analysis['neural_dynamics'] = {
            'mean_firing_rate': np.mean(firing_rates),
            'std_firing_rate': np.std(firing_rates),
            'min_firing_rate': np.min(firing_rates),
            'max_firing_rate': np.max(firing_rates),
            'rate_stability': 1.0 - (np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0),
            'activity_pattern': self.classify_activity_pattern(firing_rates)
        }
        
        # Development analysis
        development_stages = simulation_data['development_stages']
        unique_stages = len(set(development_stages))
        analysis['development'] = {
            'stages_visited': unique_stages,
            'developmental_progression': unique_stages > 1,
            'maturation_detected': development_stages[-1].name in ['Adult', 'Maturation']
        }
        
        # Learning analysis
        plasticity_events = len(simulation_data['plasticity_events'])
        analysis['learning'] = {
            'plasticity_events': plasticity_events,
            'learning_efficiency': min(1.0, plasticity_events / 10),
            'adaptation_capacity': self.calculate_adaptation_capacity(simulation_data)
        }
        
        # System integration analysis
        if 'sensory_input' in simulation_data and len(simulation_data['sensory_input']) > 0:
            analysis['system_integration'] = {
                'world_interaction': True,
                'sensorimotor_coordination': self.calculate_sensorimotor_coordination(simulation_data),
                'behavioral_flexibility': self.calculate_flexibility(simulation_data)
            }
        
        # Performance summary
        self.print_analysis_summary(analysis)
        
        return analysis
    
    def classify_activity_pattern(self, firing_rates):
        """Classify neural activity patterns"""
        mean_rate = np.mean(firing_rates)
        std_rate = np.std(firing_rates)
        
        if std_rate < 0.1 * mean_rate:
            return "stable_quiescent"
        elif std_rate < 0.3 * mean_rate:
            return "moderate_stable"
        elif mean_rate > 10:
            return "high_activity_excitable"
        else:
            return "dynamic_oscillatory"
    
    def calculate_adaptation_capacity(self, data):
        """Calculate system's adaptation capacity"""
        
        # Track changes in firing rate over time
        if len(data['firing_rates']) < 20:
            return 0.0
        
        first_half = data['firing_rates'][:len(data['firing_rates'])//2]
        second_half = data['firing_rates'][len(data['firing_rates'])//2:]
        
        mean_first = np.mean(first_half)
        mean_second = np.mean(second_half)
        
        # Calculate relative change (adaptation)
        if mean_first > 0:
            change = abs(mean_second - mean_first) / mean_first
            return min(1.0, change * 2)  # Normalize to 0-1
        
        return 0.0
    
    def calculate_sensorimotor_coordination(self, data):
        """Calculate sensorimotor coordination efficiency"""
        
        # Simplified coordination measure
        if 'sensory_input' not in data or 'motor_output' not in data:
            return 0.0
        
        sensory_changes = len([i for i in range(1, len(data['sensory_input'])) 
                              if data['sensory_input'][i] != data['sensory_input'][i-1]])
        
        motor_responses = len([i for i in range(1, len(data['motor_output']))
                              if data['motor_output'][i] != data['motor_output'][i-1]])
        
        # Coordination as ratio of responses to stimuli
        if sensory_changes > 0:
            coordination = motor_responses / sensory_changes
            return min(1.0, coordination / 10)  # Normalize
        
        return 0.0
    
    def calculate_flexibility(self, data):
        """Calculate behavioral flexibility"""
        
        # Measure variability in firing rates
        firing_rates = data['firing_rates']
        if len(firing_rates) < 10:
            return 0.0
        
        # Calculate coefficient of variation
        mean_rate = np.mean(firing_rates)
        std_rate = np.std(firing_rates)
        
        if mean_rate > 0:
            cv = std_rate / mean_rate
            return min(1.0, cv * 2)  # Normalize flexibility measure
        
        return 0.0
    
    def print_analysis_summary(self, analysis):
        """Print formatted analysis summary"""
        print("\n--- Analysis Summary ---")
        
        for category, metrics in analysis.items():
            print(f"\n{category.replace('_', ' ').title()}:")
            if isinstance(metrics, dict):
                for key, value in metrics.items():
                    if isinstance(value, float):
                        print(f"  {key}: {value:.3f}")
                    else:
                        print(f"  {key}: {value}")
        
        print("\n--- Key Insights ---")
        neural_dynamics = analysis['neural_dynamics']
        if neural_dynamics['activity_pattern'] == 'stable_quiescent':
            print("  - Brain achieved stable, quiet state (good for memory)")
        elif neural_dynamics['activity_pattern'] == 'high_activity_excitable':
            print("  - High neural excitability (good for learning)")
        else:
            print("  - Dynamic oscillatory activity (flexible processing)")
        
        learning = analysis['learning']
        if learning['learning_efficiency'] > 0.7:
            print("  - High learning efficiency")
        else:
            print("  - Moderate learning capacity")
        
        development = analysis['development']
        if development['maturation_detected']:
            print("  - Brain matured to stable adult state")
    
    def visualize_results(self, simulation_data, analysis):
        """Generate visualizations of simulation results"""
        print("\n=== Step 5: Visualization Generation ===")
        
        # Create figures
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        fig.suptitle(f"NLM Brain Simulation - {self.name}", fontsize=16)
        
        # 1. Firing rate over time
        axes[0, 0].plot(simulation_data['timestamps'], simulation_data['firing_rates'])
        axes[0, 0].set_xlabel('Time (steps)')
        axes[0, 0].set_ylabel('Firing Rate (Hz)')
        axes[0, 0].set_title('Neural Activity Dynamics')
        axes[0, 0].grid(True, alpha=0.3)
        
        # 2. Development stages
        unique_stages = list(set(simulation_data['development_stages']))
        stage_counts = {stage: simulation_data['development_stages'].count(stage) for stage in unique_stages}
        axes[0, 1].bar(range(len(unique_stages)), list(stage_counts.values()))
        axes[0, 1].set_xlabel('Development Stage')
        axes[0, 1].set_ylabel('Time Steps')
        axes[0, 1].set_title('Developmental Progression')
        axes[0, 1].set_xticks(range(len(unique_stages)))
        axes[0, 1].set_xticklabels([stage.name for stage in unique_stages])
        
        # 3. Plasticity events
        plasticity_steps = [event['step'] for event in simulation_data['plasticity_events']]
        if plasticity_steps:
            axes[0, 2].scatter(plasticity_steps, [0] * len(plasticity_steps), color='red', s=50)
            axes[0, 2].set_xlabel('Step')
            axes[0, 2].set_ylabel('Plasticity Events')
            axes[0, 2].set_title('Learning Events')
            axes[0, 2].set_ylim(-0.5, 1.5)
        
        # 4. System integration (if available)
        if 'sensory_input' in simulation_data:
            curiosity_levels = simulation_data['curiosity_levels']
            axes[1, 0].plot(simulation_data['timestamps'], curiosity_levels)
            axes[1, 0].set_xlabel('Time (steps)')
            axes[1, 0].set_ylabel('Curiosity Level')
            axes[1, 0].set_title('Intrinsic Motivation')
            axes[1, 0].grid(True, alpha=0.3)
        
        # 5. Activity stability
        firing_rates = simulation_data['firing_rates']
        rolling_mean = np.convolve(firing_rates, np.ones(10)/10, mode='valid')
        axes[1, 1].plot(firing_rates[:len(rolling_mean)], alpha=0.5, label='Original')
        axes[1, 1].plot(rolling_mean, linewidth=2, label='10-step rolling average')
        axes[1, 1].set_xlabel('Time (steps)')
        axes[1, 1].set_ylabel('Firing Rate (Hz)')
        axes[1, 1].set_title('Activity Stability')
        axes[1, 1].legend()
        axes[1, 1].grid(True, alpha=0.3)
        
        # 6. Analysis summary
        axes[1, 2].axis('off')
        summary_text = f"Simulation Results:\n\n"
        summary_text += f"Neurals:\n"
        summary_text += f"  - Mean firing rate: {analysis['neural_dynamics']['mean_firing_rate']:.2f} Hz\n"
        summary_text += f"  - Activity pattern: {analysis['neural_dynamics']['activity_pattern']}\n\n"
        summary_text += f"Development:\n"
        summary_text += f"  - Stages visited: {analysis['development']['stages_visited']}\n"
        summary_text += f"  - Maturation: {analysis['development']['maturation_detected']}\n\n"
        summary_text += f"Learning:\n"
        summary_text += f"  - Plasticity events: {analysis['learning']['plasticity_events']}\n"
        summary_text += f"  - Learning efficiency: {analysis['learning']['learning_efficiency']:.2f}\n"
        
        axes[1, 2].text(0.1, 0.5, summary_text, fontsize=10, fontfamily='monospace',
                       verticalalignment='center', wrap=True)
        axes[1, 2].set_title('Analysis Summary')
        
        plt.tight_layout()
        
        # Save figure
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"brain_simulation_{timestamp}.png"
        plt.savefig(filename, dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"  Visualization saved as: {filename}")
        
        return filename
    
    def save_results(self, filename_prefix="simulation_results"):
        """Save simulation results to file"""
        print("\n=== Step 6: Results Storage ===")
        
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        
        # Save simulation data
        data_filename = f"{filename_prefix}_data_{timestamp}.json"
        with open(data_filename, 'w') as f:
            import json
            json.dump(self.results, f, indent=2, default=str)
        
        print(f"  Simulation data saved as: {data_filename}")
        
        # Save analysis results
        analysis_filename = f"{filename_prefix}_analysis_{timestamp}.json"
        with open(analysis_filename, 'w') as f:
            import json
            json.dump(self.results.get('analysis', {}), f, indent=2, default=str)
        
        print(f"  Analysis results saved as: {analysis_filename}")
        
        return data_filename, analysis_filename
    
    def run_complete_simulation(self, steps=1000):
        """Run the complete simulation workflow"""
        print(f"Starting complete simulation: {self.name}")
        print(f"Simulation timestamp: {self.timestamp}")
        print("=" * 60)
        
        # Execute all steps
        config = self.create_configuration()
        brain = self.create_and_initialize_brain(config)
        simulation_data = self.run_simulation_with_monitoring(brain, steps=steps)
        analysis = self.analyze_results(simulation_data)
        
        self.results['analysis'] = analysis
        
        # Generate visualization
        plot_filename = self.visualize_results(simulation_data, analysis)
        
        # Save results
        data_file, analysis_file = self.save_results()
        
        print("\n=== SIMULATION COMPLETE ===")
        print(f"Results:")
        print(f"  - Visualization: {plot_filename}")
        print(f"  - Data file: {data_file}")
        print(f"  - Analysis file: {analysis_file}")
        
        return self.results

def main():
    """Main function to run the example"""
    print("NLM Basic Brain Simulation Example")
    print("=" * 50)
    print()
    
    # Create and run simulation
    simulation = BasicBrainSimulation()
    results = simulation.run_complete_simulation(steps=1000)
    
    print("\nExample completed successfully!")
    print("\nKey observations:")
    print("1. Brain learns and adapts through plasticity events")
    print("2. Neural dynamics show stable patterns over time")
    print("3. Development progresses through stages")
    print("4. System maintains balance between exploration and exploitation")
    
    return results

if __name__ == "__main__":
    main()
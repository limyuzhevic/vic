Advanced utilities for NLM research and development.
Provides visualization, analysis, export/import, and performance tools.

This module extends the basic NLM Python API with advanced features
for research use cases, including visualization, profiling,
batch processing, and integration with research tools.

This module extends the basic NLM Python API with advanced features
for research use cases, including visualization, profiling,
batch processing, and integration with research tools.
"""

import os
import sys
import json
import time
import numpy as np
import matplotlib.pyplot as plt
from typing import List, Dict, Any, Optional, Callable, Union
import tempfile
import csv

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

# Import NLM module
try:
    import pynlm
    from pynlm import (
        create_default_config, create_brain, create_simple_world,
        create_agent_brain, Vision, Audio, InternalSignals,
        Action, WorldObject, ActionResult, SimulationContext
    )
except ImportError as e:
    print(f"Error: Could not import pynlm: {e}")
    print("Please install pynlm: pip install pynlm")
    sys.exit(1)

# Advanced research exceptions
class ResearchException(Exception):
    """Base exception for research utilities errors."""
    pass

class VisualizationException(ResearchException):
    """Exception for visualization errors."""
    pass

class AnalysisException(ResearchException):
    """Exception for analysis errors."""
    pass

class ExportImportException(ResearchException):
    """Exception for export/import errors."""
    pass

class PerformanceException(ResearchException):
    """Exception for performance measurement errors."""
    pass

# Advanced research utilities
class NLMResearchUtils:
    """Advanced utilities for NLM research and development."""
    
    @staticmethod
    def create_research_config(base_config=None, **kwargs):
        """Create a research configuration with enhanced defaults.
        
        Args:
            base_config: Base configuration to use as starting point
            **kwargs: Additional configuration parameters
            
        Returns:
            Configured NLM Config object
        """
        if base_config is None:
            config = pynlm.create_default_config()
        else:
            config = base_config
        
        # Enhanced research defaults
        research_defaults = {
            "brain.neuron_count": 5000,
            "brain.synapse_density": 0.08,
            "brain.region_count": 8,
            "connection_probability": 0.03,
            "stdp_ltp_weight": 0.005,
            "stdp_ltd_weight": 0.008,
            "synaptogenesis_rate": 0.00005f,
            "pruning_rate": 0.000005f,
            "energy_decay_rate": 0.005,
            "max_energy": 200.0f,
            "plasticity.stdp.enable": True,
            "plasticity.hebbian.enable": True,
            "plasticity.structural.enable": True,
            "neuromod.dopamine.scale": 1.5,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "development.enable": True,
            "development.stage_duration": 1000,
        }
        
        # Apply research defaults
        for key, value in research_defaults.items():
            try:
                if key.endswith(".enable"):
                    config.set(key, bool(value))
                elif isinstance(value, float):
                    config.set(key, float(value))
                elif isinstance(value, int):
                    config.set(key, int(value))
                else:
                    config.set(key, str(value))
            except:
                pass  # Skip invalid config keys
        
        # Apply user overrides
        for key, value in kwargs.items():
            try:
                if key.endswith(".enable"):
                    config.set(key, bool(value))
                elif isinstance(value, (int, float, str)):
                    config.set(key, value)
            except:
                pass
        
        return config
    
    @staticmethod
    def run_research_experiment(brain, world, agent, num_steps, 
                                save_interval=100, **monitoring_options):
        """Run a research experiment with comprehensive monitoring.
        
        Args:
            brain: NLM brain object
            world: World object
            agent: Agent brain object
            num_steps: Number of steps to simulate
            save_interval: How often to save checkpoints
            **monitoring_options: Additional monitoring options
            
        Returns:
            Dictionary with experiment results and statistics
        """
        results = {
            'steps_completed': 0,
            'total_reward': 0.0,
            'firing_rates': [],
            'neuron_counts': [],
            'spike_counts': [],
            'development_stages': [],
            'curiosity_levels': [],
            'novelty_levels': [],
            'prediction_errors': [],
            'timestamps': [],
            'checkpoints': [],
            'start_time': time.time(),
            'end_time': 0.0,
            'monitoring_options': monitoring_options
        }
        
        enable_monitoring = monitoring_options.get('enable_monitoring', True)
        save_plots = monitoring_options.get('save_plots', False)
        export_data = monitoring_options.get('export_data', False)
        output_dir = monitoring_options.get('output_dir', '.')
        
        try:
            # Enable all subsystems for research
            agent.enable_reward_modulation(True)
            agent.enable_structural_plasticity(True)
            agent.enable_development(True)
            agent.enable_curiosity(True)
            
            for step in range(num_steps):
                # Update world
                world.update(monitoring_options.get('timestep', 0.1))
                
                # Process sensory input
                agent.process_sensory_input(world.get_sensory_percept())
                
                # Run brain step
                brain.step(step)
                
                # Get action and apply to world
                action = agent.decode_motor_command()
                world.apply_motor_command(action, world.get_simulation_time())
                
                # Apply reward modulation
                reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
                agent.apply_reward_modulation(reward, 0.0)
                
                # Update development
                agent.update_development(monitoring_options.get('timestep', 0.1))
                
                # Record metrics
                if enable_monitoring:
                    results['steps_completed'] = step + 1
                    results['total_reward'] += reward
                    results['firing_rates'].append(brain.getAverageFiringRate())
                    results['neuron_counts'].append(brain.getTotalNeuronCount())
                    results['spike_counts'].append(brain.getTotalSpikeCount())
                    results['development_stages'].append(brain.getDevelopmentalStage())
                    results['curiosity_levels'].append(agent.getCuriosityLevel())
                    results['novelty_levels'].append(agent.getNoveltyLevel())
                    results['prediction_errors'].append(agent.getPredictionError())
                    results['timestamps'].append(world.get_simulation_time())
                
                # Save checkpoint if interval reached
                if save_interval > 0 and (step + 1) % save_interval == 0:
                    checkpoint_path = os.path.join(output_dir, f"checkpoint_step_{step+1}.bin")
                    brain.save(checkpoint_path)
                    results['checkpoints'].append(checkpoint_path)
                    print(f"Checkpoint saved: {checkpoint_path}")
                
                # Generate plots if requested
                if save_plots and (step + 1) % save_interval == 0:
                    NLMResearchUtils._generate_interim_plots(results, output_dir, step + 1)
            
            # Experiment complete
            results['end_time'] = time.time()
            
            # Calculate final statistics
            duration = results['end_time'] - results['start_time']
            results['duration'] = duration
            results['steps_per_second'] = results['steps_completed'] / duration if duration > 0 else 0
            results['avg_firing_rate'] = np.mean(results['firing_rates']) if results['firing_rates'] else 0
            results['avg_reward'] = results['total_reward'] / results['steps_completed'] if results['steps_completed'] > 0 else 0
            
            # Save results if requested
            if export_data:
                results_file = os.path.join(output_dir, "experiment_results.json")
                with open(results_file, 'w') as f:
                    json.dump(results, f, indent=2, default=str)
                print(f"Experiment results saved: {results_file}")
                
                # Save CSV if requested
                csv_file = os.path.join(output_dir, "experiment_data.csv")
                NLMResearchUtils._save_results_csv(results, csv_file)
            
            # Generate plots if requested
            if save_plots:
                NLMResearchUtils._generate_experiment_plots(results, output_dir)
            
            return results
            
        except Exception as e:
            raise ResearchException(f"Research experiment failed: {e}")
    
    @staticmethod
    def _save_results_csv(results, csv_file):
        """Save experiment results to CSV file."""
        with open(csv_file, 'w', newline='') as f:
            writer = csv.writer(f)
            
            # Header
            writer.writerow(['Step', 'Time', 'FiringRate', 'NeuronCount', 
                           'SpikeCount', 'DevelopmentStage', 'Curiosity', 
                           'Novelty', 'PredictionError', 'Reward'])
            
            # Data
            for i in range(len(results['steps_completed'])):
                writer.writerow([
                    i + 1,
                    results['timestamps'][i] if i < len(results['timestamps']) else 0,
                    results['firing_rates'][i] if i < len(results['firing_rates']) else 0,
                    results['neuron_counts'][i] if i < len(results['neuron_counts']) else 0,
                    results['spike_counts'][i] if i < len(results['spike_counts']) else 0,
                    str(results['development_stages'][i]) if i < len(results['development_stages']) else 'Unknown',
                    results['curiosity_levels'][i] if i < len(results['curiosity_levels']) else 0,
                    results['novelty_levels'][i] if i < len(results['novelty_levels']) else 0,
                    results['prediction_errors'][i] if i < len(results['prediction_errors']) else 0,
                    results['total_reward'] / results['steps_completed'] if results['steps_completed'] > 0 else 0
                ])
    
    @staticmethod
    def _generate_interim_plots(results, output_dir, step):
        """Generate interim plots during experiment."""
        try:
            fig, axes = plt.subplots(2, 2, figsize=(12, 10))
            fig.suptitle(f"NLM Research - Step {step}", fontsize=14)
            
            # Plot firing rate
            if results['firing_rates']:
                axes[0, 0].plot(results['firing_rates'][-100:] if len(results['firing_rates']) >= 100 else results['firing_rates'])
                axes[0, 0].set_title("Firing Rate")
                axes[0, 0].set_xlabel("Step")
                axes[0, 0].set_ylabel("Rate")
                axes[0, 0].grid(True)
            
            # Plot development stage
            if results['development_stages']:
                stages = [str(stage) for stage in results['development_stages'][-100:] if len(results['development_stages']) >= 100 else results['development_stages']]
                axes[0, 1].plot(stages)
                axes[0, 1].set_title("Development Stage")
                axes[0, 1].set_xlabel("Step")
                axes[0, 1].set_ylabel("Stage")
                axes[0, 1].grid(True)
            
            # Plot curiosity level
            if results['curiosity_levels']:
                axes[1, 0].plot(results['curiosity_levels'][-100:] if len(results['curiosity_levels']) >= 100 else results['curiosity_levels'])
                axes[1, 0].set_title("Curiosity Level")
                axes[1, 0].set_xlabel("Step")
                axes[1, 0].set_ylabel("Curiosity")
                axes[1, 0].grid(True)
            
            # Plot novelty level
            if results['novelty_levels']:
                axes[1, 1].plot(results['novelty_levels'][-100:] if len(results['novelty_levels']) >= 100 else results['novelty_levels'])
                axes[1, 1].set_title("Novelty Level")
                axes[1, 1].set_xlabel("Step")
                axes[1, 1].set_ylabel("Novelty")
                axes[1, 1].grid(True)
            
            # Save figure
            plot_file = os.path.join(output_dir, f"interim_plots_step_{step}.png")
            plt.savefig(plot_file, dpi=100, bbox_inches='tight')
            plt.close()
            
        except Exception as e:
            # Don't fail the experiment if plotting fails
            pass
    
    @staticmethod
    def _generate_experiment_plots(results, output_dir):
        """Generate comprehensive plots from experiment results."""
        try:
            # Create figure with subplots
            fig, axes = plt.subplots(3, 3, figsize=(18, 15))
            fig.suptitle("NLM Research Experiment - Comprehensive Analysis", fontsize=16)
            
            # Plot 1: Firing rate over time
            if results['firing_rates']:
                axes[0, 0].plot(results['firing_rates'])
                axes[0, 0].set_title("Average Firing Rate")
                axes[0, 0].set_xlabel("Step")
                axes[0, 0].set_ylabel("Firing Rate")
                axes[0, 0].grid(True)
            
            # Plot 2: Neuron count over time
            if results['neuron_counts']:
                axes[0, 1].plot(results['neuron_counts'])
                axes[0, 1].set_title("Neuron Count")
                axes[0, 1].set_xlabel("Step")
                axes[0, 1].set_ylabel("Neuron Count")
                axes[0, 1].grid(True)
            
            # Plot 3: Spike count over time
            if results['spike_counts']:
                axes[0, 2].plot(results['spike_counts'])
                axes[0, 2].set_title("Total Spike Count")
                axes[0, 2].set_xlabel("Step")
                axes[0, 2].set_ylabel("Spike Count")
                axes[0, 2].grid(True)
            
            # Plot 4: Development stage over time
            if results['development_stages']:
                stages = [str(stage) for stage in results['development_stages']]
                axes[1, 0].plot(stages)
                axes[1, 0].set_title("Developmental Stage")
                axes[1, 0].set_xlabel("Step")
                axes[1, 0].set_ylabel("Stage")
                axes[1, 0].grid(True)
            
            # Plot 5: Curiosity level over time
            if results['curiosity_levels']:
                axes[1, 1].plot(results['curiosity_levels'])
                axes[1, 1].set_title("Curiosity Level")
                axes[1, 1].set_xlabel("Step")
                axes[1, 1].set_ylabel("Curiosity")
                axes[1, 1].grid(True)
            
            # Plot 6: Novelty level over time
            if results['novelty_levels']:
                axes[1, 2].plot(results['novelty_levels'])
                axes[1, 2].set_title("Novelty Level")
                axes[1, 2].set_xlabel("Step")
                axes[1, 2].set_ylabel("Novelty")
                axes[1, 2].grid(True)
            
            # Plot 7: Prediction error over time
            if results['prediction_errors']:
                axes[2, 0].plot(results['prediction_errors'])
                axes[2, 0].set_title("Prediction Error")
                axes[2, 0].set_xlabel("Step")
                axes[2, 0].set_ylabel("Error")
                axes[2, 0].grid(True)
            
            # Plot 8: Reward over time
            rewards = []
            for i in range(len(results['timestamps'])):
                reward = results['total_reward'] / (i + 1) if (i + 1) > 0 else 0
                rewards.append(reward)
            
            if rewards:
                axes[2, 1].plot(rewards)
                axes[2, 1].set_title("Average Reward")
                axes[2, 1].set_xlabel("Step")
                axes[2, 1].set_ylabel("Reward")
                axes[2, 1].grid(True)
            
            # Plot 9: Summary statistics
            axes[2, 2].axis('off')
            text_content = f"Experiment Summary:\n\n"
            text_content += f"Total Steps: {results['steps_completed']}\n"
            text_content += f"Duration: {results['duration']:.2f}s\n"
            text_content += f"Steps/sec: {results['steps_per_second']:.2f}\n"
            text_content += f"Avg Firing Rate: {results['avg_firing_rate']:.4f}\n"
            text_content += f"Avg Reward: {results['avg_reward']:.4f}\n"
            text_content += f"Checkpoints: {len(results['checkpoints'])}\n"
            
            axes[2, 2].text(0.1, 0.5, text_content, fontsize=10, verticalalignment='center',
                           bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
            axes[2, 2].set_title("Statistics")
            
            # Save figure
            plot_file = os.path.join(output_dir, "experiment_plots.png")
            plt.savefig(plot_file, dpi=150, bbox_inches='tight')
            plt.close()
            
            print(f"Comprehensive plots saved: {plot_file}")
            
        except Exception as e:
            raise VisualizationException(f"Failed to generate experiment plots: {e}")
    
    @staticmethod
    def analyze_brain_dynamics(brain, duration_steps=1000, **analysis_options):
        """Analyze brain dynamics and extract research insights.
        
        Args:
            brain: NLM brain object
            duration_steps: Number of steps to analyze
            **analysis_options: Additional analysis options
            
        Returns:
            Dictionary with analysis results
        """
        analysis_results = {
            'step_range': range(duration_steps),
            'firing_rates': [],
            'spike_rates': [],
            'excitatory_inhibitory_ratio': [],
            'development_stages': [],
            'stability_metrics': [],
            'network_topology': {},
            'plasticity_metrics': {},
            'information_theory': {},
            'analysis_options': analysis_options
        }
        
        try:
            # Run analysis
            for step in range(duration_steps):
                brain.step(step)
                
                # Collect metrics
                analysis_results['firing_rates'].append(brain.getAverageFiringRate())
                analysis_results['excitatory_inhibitory_ratio'].append(
                    brain.getExcitationInhibitionRatio()
                )
                analysis_results['development_stages'].append(
                    brain.getDevelopmentalStage()
                )
                
                # Calculate spike rate
                spikes_per_step = brain.getFiringNeuronCount()
                spike_rate = spikes_per_step / (duration_steps - step) if (duration_steps - step) > 0 else 0
                analysis_results['spike_rates'].append(spike_rate)
                
                # Calculate stability metrics
                if step >= 10:
                    recent_firing = analysis_results['firing_rates'][-10:]
                    if len(recent_firing) >= 2:
                        # Calculate coefficient of variation
                        mean_firing = np.mean(recent_firing)
                        std_firing = np.std(recent_firing)
                        cv = std_firing / mean_firing if mean_firing > 0 else 0
                        analysis_results['stability_metrics'].append(cv)
            
            # Analyze network topology
            analysis_results['network_topology'] = {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'synapse_density': brain.getTotalSynapseCount() / brain.getTotalNeuronCount() 
                                 if brain.getTotalNeuronCount() > 0 else 0,
                'region_count': brain.getRegionCount(),
                'regions': []
            }
            
            # Get region information
            region_ids = brain.getRegionIds()
            for region_id in region_ids:
                region = brain.getRegion(region_id)
                if region:
                    analysis_results['network_topology']['regions'].append({
                        'id': str(region_id),
                        'name': region.getName()
                    })
            
            # Analyze plasticity metrics
            analysis_results['plasticity_metrics'] = {
                'development_stage': str(brain.getDevelopmentalStage()),
                'plasticity_active': brain.getDevelopmentalStage() != pynlm.DevelopmentalStage.Aging
            }
            
            # Information theory analysis (simplified)
            analysis_results['information_theory'] = {
                'firing_variance': np.var(analysis_results['firing_rates']) if analysis_results['firing_rates'] else 0,
                'firing_entropy': 'TODO',  # Would require more complex analysis
                'network_complexity': 'TODO'
            }
            
            # Statistical analysis
            analysis_results['statistics'] = {
                'firing_rate_mean': np.mean(analysis_results['firing_rates']) if analysis_results['firing_rates'] else 0,
                'firing_rate_std': np.std(analysis_results['firing_rates']) if analysis_results['firing_rates'] else 0,
                'firing_rate_min': np.min(analysis_results['firing_rates']) if analysis_results['firing_rates'] else 0,
                'firing_rate_max': np.max(analysis_results['firing_rates']) if analysis_results['firing_rates'] else 0,
                'e_ir_ratio_mean': np.mean(analysis_results['excitatory_inhibitory_ratio']) if analysis_results['excitatory_inhibitory_ratio'] else 0,
                'development_stage_changes': len(set(analysis_results['development_stages']))
            }
            
            return analysis_results
            
        except Exception as e:
            raise AnalysisException(f"Brain dynamics analysis failed: {e}")
    
    @staticmethod
    def export_brain_model(brain, filepath, format='json'):
        """Export brain model to file.
        
        Args:
            brain: NLM brain object
            filepath: Output file path
            format: Export format ('json', 'txt', 'bin')
            
        Returns:
            Path to exported file
        """
        try:
            if format.lower() == 'json':
                return NLMResearchUtils._export_brain_json(brain, filepath)
            elif format.lower() == 'txt':
                return NLMResearchUtils._export_brain_txt(brain, filepath)
            elif format.lower() == 'bin':
                return brain.save(filepath)
            else:
                raise ExportImportException(f"Unsupported format: {format}")
                
        except Exception as e:
            raise ExportImportException(f"Brain export failed: {e}")
    
    @staticmethod
    def _export_brain_json(brain, filepath):
        """Export brain to JSON format."""
        brain_data = {
            'metadata': {
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'development_stage': str(brain.getDevelopmentalStage()),
                'timestamp': time.time()
            },
            'configuration': brain.getConfig().summary() if brain.getConfig() else 'N/A',
            'statistics': {
                'firing_rate': brain.getAverageFiringRate(),
                'excitatory_inhibitory_ratio': brain.getExcitationInhibitionRatio(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
            }
        }
        
        with open(filepath, 'w') as f:
            json.dump(brain_data, f, indent=2)
        
        return filepath
    
    @staticmethod
    def _export_brain_txt(brain, filepath):
        """Export brain to text format."""
        with open(filepath, 'w') as f:
            f.write("NLM Brain Export\n")
            f.write("=================\n\n")
            f.write(f"Neurons: {brain.getTotalNeuronCount()}\n")
            f.write(f"Synapses: {brain.getTotalSynapseCount()}\n")
            f.write(f"Average Firing Rate: {brain.getAverageFiringRate():.3f}\n")
            f.write(f"Total Spike Count: {brain.getTotalSpikeCount()}\n")
            f.write(f"Development Stage: {brain.getDevelopmentalStage()}\n")
            f.write(f"E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}\n")
            f.write(f"Active Neurons: {brain.getActiveNeuronCount()}\n")
            f.write(f"Firing Neurons: {brain.getFiringNeuronCount()}\n\n")
            
            if brain.getConfig():
                f.write("Configuration Summary:\n")
                f.write(brain.getConfig().summary())
        
        return filepath
    
    @staticmethod
    def import_brain_model(filepath, format='bin'):
        """Import brain model from file.
        
        Args:
            filepath: Input file path
            format: Import format ('json', 'txt', 'bin')
            
        Returns:
            NLM brain object
        """
        try:
            if format.lower() == 'bin':
                config = pynlm.create_default_config()
                brain = pynlm.create_brain(config)
                if brain.load(filepath):
                    return brain
                else:
                    raise ExportImportException(f"Failed to load brain from {filepath}")
            elif format.lower() == 'json':
                return NLMResearchUtils._import_brain_json(filepath)
            elif format.lower() == 'txt':
                return NLMResearchUtils._import_brain_txt(filepath)
            else:
                raise ExportImportException(f"Unsupported format: {format}")
                
        except Exception as e:
            raise ExportImportException(f"Brain import failed: {e}")
    
    @staticmethod
    def _import_brain_json(filepath):
        """Import brain from JSON format."""
        with open(filepath, 'r') as f:
            brain_data = json.load(f)
        
        # Create brain with default config
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        
        # Note: This is a simplified import - in practice would need
        # to restore the exact brain state from the exported data
        print("Note: JSON import creates a new brain (simplified)")
        return brain
    
    @staticmethod
    def _import_brain_txt(filepath):
        """Import brain from text format."""
        # For text format, we create a new brain with the parameters
        # extracted from the file
        print("Note: Text import creates a new brain (simplified)")
        
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        return brain
    
    @staticmethod
    def profile_performance(brain, steps=1000, **profile_options):
        """Profile brain performance metrics.
        
        Args:
            brain: NLM brain object
            steps: Number of steps to profile
            **profile_options: Additional profiling options
            
        Returns:
            Dictionary with performance metrics
        """
        profile_results = {
            'steps': steps,
            'timestep': profile_options.get('timestep', 0.001),
            'measurements': [],
            'performance_metrics': {
                'time_per_step': [],
                'memory_usage': [],
                'cpu_usage': [],
                'network_activity': [],
                'plasticity_activity': []
            }
        }
        
        try:
            start_time = time.time()
            
            for step in range(steps):
                step_start = time.time()
                
                # Measure step execution time
                brain.step(step)
                
                step_end = time.time()
                step_duration = step_end - step_start
                
                # Record metrics
                measurement = {
                    'step': step,
                    'timestamp': step_end - start_time,
                    'step_duration': step_duration,
                    'firing_rate': brain.getAverageFiringRate(),
                    'spike_count': brain.getFiringNeuronCount(),
                    'development_stage': brain.getDevelopmentalStage(),
                    'neuron_count': brain.getTotalNeuronCount(),
                    'synapse_count': brain.getTotalSynapseCount()
                }
                
                profile_results['measurements'].append(measurement)
                profile_results['performance_metrics']['time_per_step'].append(step_duration)
                profile_results['performance_metrics']['network_activity'].append(
                    brain.getAverageFiringRate()
                )
                
                # Estimate other metrics (simplified)
                if step % 100 == 0:
                    # Record approximate memory usage (placeholder)
                    profile_results['performance_metrics']['memory_usage'].append(
                        len(str(brain.get_stats())) / 1024  # rough estimate
                    )
                    
                    # Record approximate CPU usage (placeholder)
                    profile_results['performance_metrics']['cpu_usage'].append(
                        step_duration / profile_options.get('timestep', 0.001)
                    )
                    
                    # Record plasticity activity
                    if brain.getDevelopmentalStage() != pynlm.DevelopmentalStage.Aging:
                        profile_results['performance_metrics']['plasticity_activity'].append(1.0)
                    else:
                        profile_results['performance_metrics']['plasticity_activity'].append(0.0)
            
            # Calculate performance statistics
            profile_results['performance_summary'] = {
                'total_time': time.time() - start_time,
                'avg_step_time': np.mean(profile_results['performance_metrics']['time_per_step']) if profile_results['performance_metrics']['time_per_step'] else 0,
                'steps_per_second': steps / (time.time() - start_time) if (time.time() - start_time) > 0 else 0,
                'total_spikes': sum(m['spike_count'] for m in profile_results['measurements']),
                'avg_firing_rate': np.mean([m['firing_rate'] for m in profile_results['measurements']]) if profile_results['measurements'] else 0
            }
            
            return profile_results
            
        except Exception as e:
            raise PerformanceException(f"Performance profiling failed: {e}")
    
    @staticmethod
    def batch_process_experiments(experiment_configs, output_base_dir='experiments'):
        """Run multiple experiments in batch mode.
        
        Args:
            experiment_configs: List of experiment configuration dictionaries
            output_base_dir: Base directory for output files
            
        Returns:
            Dictionary with batch processing results
        """
        batch_results = {
            'experiments': [],
            'total_time': 0,
            'successful_experiments': 0,
            'failed_experiments': 0,
            'output_base_dir': output_base_dir
        }
        
        start_time = time.time()
        
        try:
            # Create output directory
            os.makedirs(output_base_dir, exist_ok=True)
            
            for i, config in enumerate(experiment_configs):
                experiment_name = config.get('name', f'experiment_{i}')
                experiment_dir = os.path.join(output_base_dir, experiment_name)
                os.makedirs(experiment_dir, exist_ok=True)
                
                print(f"Running experiment {i+1}/{len(experiment_configs)}: {experiment_name}")
                
                try:
                    # Create brain and run experiment
                    brain_config = NLMResearchUtils.create_research_config(config.get('config', {}))
                    brain = pynlm.create_brain(brain_config)
                    brain.initialize()
                    
                    # Run experiment
                    results = NLMResearchUtils.run_research_experiment(
                        brain,
                        create_simple_world(),  # Would need proper world setup
                        create_agent_brain(brain),
                        config.get('steps', 1000),
                        output_dir=experiment_dir,
                        **config.get('options', {})
                    )
                    
                    # Save experiment results
                    results_file = os.path.join(experiment_dir, 'results.json')
                    with open(results_file, 'w') as f:
                        json.dump(results, f, indent=2, default=str)
                    
                    experiment_result = {
                        'name': experiment_name,
                        'status': 'success',
                        'results': results,
                        'output_dir': experiment_dir,
                        'config_file': results_file
                    }
                    
                    batch_results['experiments'].append(experiment_result)
                    batch_results['successful_experiments'] += 1
                    
                    print(f"  ✓ Experiment '{experiment_name}' completed successfully")
                    
                except Exception as e:
                    experiment_result = {
                        'name': experiment_name,
                        'status': 'failed',
                        'error': str(e),
                        'output_dir': experiment_dir
                    }
                    
                    batch_results['experiments'].append(experiment_result)
                    batch_results['failed_experiments'] += 1
                    
                    print(f"  ✗ Experiment '{experiment_name}' failed: {e}")
            
            batch_results['total_time'] = time.time() - start_time
            
            # Save batch results
            batch_results_file = os.path.join(output_base_dir, 'batch_results.json')
            with open(batch_results_file, 'w') as f:
                json.dump(batch_results, f, indent=2, default=str)
            
            print(f"\nBatch processing completed in {batch_results['total_time']:.2f} seconds")
            print(f"Successful: {batch_results['successful_experiments']}")
            print(f"Failed: {batch_results['failed_experiments']}")
            
            return batch_results
            
        except Exception as e:
            raise ResearchException(f"Batch processing failed: {e}")

# Convenience functions for common research tasks

def create_research_environment(neuron_count=5000, **kwargs):
    """Create a research environment with typical settings."""
    config = NLMResearchUtils.create_research_config(neuron_count=neuron_count, **kwargs)
    brain = pynlm.create_brain(config)
    
    # Create world and agent
    world = pynlm.create_simple_world()
    world.configure(20, 20, 8, 8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    return brain, world, agent

def run_research_simulation(steps=1000, **options):
    """Run a research simulation with monitoring."""
    brain, world, agent = create_research_environment(**options)
    
    return NLMResearchUtils.run_research_experiment(
        brain, world, agent, steps, **options
    )

def analyze_brain_network(brain, steps=1000):
    """Analyze brain network dynamics."""
    return NLMResearchUtils.analyze_brain_dynamics(brain, steps)

def profile_brain_performance(brain, steps=1000):
    """Profile brain performance."""
    return NLMResearchUtils.profile_performance(brain, steps)

# Example usage and demonstration
if __name__ == "__main__":
    print("NLM Research Utilities - Demonstration")
    print("=" * 50)
    
    # Example 1: Create research environment
    print("\n1. Creating research environment...")
    brain, world, agent = create_research_environment(neuron_count=1000)
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Example 2: Run research experiment
    print("\n2. Running research experiment...")
    results = run_research_simulation(steps=100, enable_monitoring=True)
    print(f"   Completed {results['steps_completed']} steps")
    print(f"   Total reward: {results['total_reward']:.2f}")
    
    # Example 3: Analyze brain dynamics
    print("\n3. Analyzing brain dynamics...")
    analysis = analyze_brain_network(brain, steps=50)
    print(f"   Network has {analysis['network_topology']['total_neurons']} neurons")
    print(f"   Synapse density: {analysis['network_topology']['synapse_density']:.4f}")
    
    # Example 4: Profile performance
    print("\n4. Profiling performance...")
    profile = profile_brain_performance(brain, steps=50)
    print(f"   Average step time: {profile['performance_summary']['avg_step_time']:.4f}s")
    print(f"   Steps per second: {profile['performance_summary']['steps_per_second']:.2f}")
    
    print("\n" + "=" * 50)
    print("Demonstration complete!")
    print("\nFor more advanced usage, refer to the documentation.")
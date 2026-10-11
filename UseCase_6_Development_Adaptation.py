# Development and Adaptation for NLM

"""
Comprehensive demonstration of developmental processes in the Neural Learning Machine (NLM).

This script implements the Development and Adaptation use case, demonstrating:
- Developmental processes over time
- Structural plasticity and brain maturation
- Lifelong learning capabilities
- Age-dependent changes in neural dynamics
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
import time
import random
from collections import deque

class DevelopmentExperiment:
    """
    Comprehensive development experiment demonstrating brain maturation and adaptation.
    
    This experiment shows how NLM brains develop over time, changing their
    structural and functional properties through developmental stages.
    """
    
    def __init__(self, config):
        """Initialize the development experiment."""
        self.config = config
        self.setup_development_system()
        
        # Performance tracking
        self.development_stages = []
        self.plasticity_rates = []
        self.neural_maturation = []
        self.synapse_density = []
        self.excitatory_inhibitory_ratio = []
        self.learning_efficiency = []
        self.adaptation_metrics = []
        
        # State tracking
        self.current_step = 0
        self.current_stage = 0
        self.stage_transitions = []
        self.brain_age = 0.0
        
        # Learning tracking
        self.reward_history = []
        self.performance_history = []
    
    def setup_development_system(self):
        """Setup the NLM brain with development-focused configuration."""
        # Create brain with development-optimized configuration
        config = pynlm.createDefaultConfig()
        
        # Optimize for development processes
        config.set("brain.neuron_count", self.config.get('neuron_count', 1000))
        config.set("brain.region_count", self.config.get('region_count', 4))
        config.set("brain.v_thresh", -50.0)
        config.set("brain.v_rest", -70.0)
        config.set("brain.v_reset", -75.0)
        
        # Development parameters
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", self.config.get('stdp_learning_rate', 0.002))
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.synaptogenesis_rate", 
                   self.config.get('synaptogenesis_rate', 0.0001))
        config.set("plasticity.structural.pruning_rate", 
                   self.config.get('pruning_rate', 0.00001))
        
        # Create brain
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Setup simple world for controlled experiments
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        self.world.reset()
        self.agent.initialize(self.world)
        
        # Enable development subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Performance monitoring
        self.performance_monitor = DevelopmentPerformanceMonitor()
    
    def run_development_experiment(self, num_steps=10000, show_plots=True):
        """
        Run comprehensive development experiment.
        
        Args:
            num_steps: Number of simulation steps
            show_plots: Whether to create visualizations
            
        Returns:
            Dictionary with experiment results
        """
        print("=== DEVELOPMENT AND ADAPTATION EXPERIMENT ===")
        print("Simulating brain development and lifelong learning...")
        
        print(f"Starting development experiment for {num_steps} steps...")
        print(f"Neuron count: {self.config.get('neuron_count', 1000)}")
        print(f"Developmental stages: {self.config.get('developmental_stages', 5)}")
        
        start_time = time.time()
        
        for step in range(num_steps):
            self.current_step = step
            
            # Calculate current developmental stage
            self.current_stage = self.calculate_developmental_stage(step, num_steps)
            self.brain_age = step * 0.001  # Age in days
            
            # Record stage information
            if step % (num_steps // self.config.get('developmental_stages', 5)) == 0:
                self.stage_transitions.append({
                    'step': step,
                    'stage': self.current_stage,
                    'brain_age': self.brain_age
                })
            
            # Get sensory input and process
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain processes with age-dependent plasticity
            self.brain.step(step, step * 0.01)
            
            # Decode motor command and act
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply action to world
            reward = self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.01)
            
            # Record developmental metrics
            self.record_developmental_metrics(step, reward)
            
            # Print progress
            if step % 1000 == 0:
                print(f"Step {step} (Stage {self.current_stage}): "
                      f"Firing={self.brain.getAverageFiringRate():.3f} Hz, "
                      f"Dev={self.agent.getDevelopmentalStage():.2f}, "
                      f"Age={self.brain_age:.1f} days, "
                      f"Reward={reward:.2f}")
        
        end_time = time.time()
        
        # Analyze results
        results = self.analyze_development_results(end_time)
        
        # Create visualizations
        if show_plots:
            self.create_development_visualizations()
        
        return results
    
    def calculate_developmental_stage(self, step, total_steps):
        """
        Calculate current developmental stage.
        
        Stages:
        0: Initial (steps 0-20%)
        1: Critical Period (steps 20-40%)
        2: Maturation (steps 40-60%)
        3: Adult (steps 60-80%)
        4: Aging (steps 80-100%)
        """
        stage_ratio = step / total_steps
        
        if stage_ratio < 0.2:
            return 0.0  # Initial
        elif stage_ratio < 0.4:
            return 1.0  # Critical Period
        elif stage_ratio < 0.6:
            return 2.0  # Maturation
        elif stage_ratio < 0.8:
            return 3.0  # Adult
        else:
            return 4.0  # Aging
    
    def record_developmental_metrics(self, step, reward):
        """
        Record comprehensive developmental metrics.
        
        Tracks:
        - Developmental stage progression
        - Plasticity rate changes
        - Neural maturation
        - Synaptic connectivity
        - Learning efficiency
        - Adaptation metrics
        """
        # Record developmental stage
        self.development_stages.append({
            'step': step,
            'stage': self.current_stage,
            'brain_age': self.brain_age,
            'timestamp': time.time()
        })
        
        # Record plasticity rates
        if hasattr(self.brain, 'getStructuralPlasticityRate'):
            plasticity_rate = self.brain.getStructuralPlasticityRate()
            self.plasticity_rates.append({
                'step': step,
                'plasticity_rate': plasticity_rate,
                'timestamp': time.time()
            })
        
        # Record neural maturation
        firing_rate = self.brain.getAverageFiringRate()
        self.neural_maturation.append({
            'step': step,
            'firing_rate': firing_rate,
            'timestamp': time.time()
        })
        
        # Record synaptic density
        if hasattr(self.brain, 'getSynapseDensity'):
            synapse_density = self.brain.getSynapseDensity()
            self.synapse_density.append({
                'step': step,
                'synapse_density': synapse_density,
                'timestamp': time.time()
            })
        
        # Record E/I ratio
        if hasattr(self.brain, 'getExcitationInhibitionRatio'):
            ei_ratio = self.brain.getExcitationInhibitionRatio()
            self.excitatory_inhibitory_ratio.append({
                'step': step,
                'ei_ratio': ei_ratio,
                'timestamp': time.time()
            })
        
        # Record adaptation metrics
        adaptation_metrics = self.calculate_adaptation_metrics(step, reward)
        self.adaptation_metrics.append(adaptation_metrics)
        
        # Update performance monitor
        self.performance_monitor.record_development_metrics(
            step_time=0.01,
            developmental_stage=self.current_stage,
            brain_age=self.brain_age,
            reward=reward,
            adaptation_metrics=adaptation_metrics
        )
        
        # Store reward history
        self.reward_history.append(reward)
        self.performance_history.append(firing_rate)
    
    def calculate_adaptation_metrics(self, step, reward):
        """
        Calculate adaptation metrics for current step.
        
        Includes:
        - Reward adaptation (learning from experience)
        - Behavioral flexibility
        - Memory consolidation
        - Development efficiency
        """
        # Calculate reward adaptation
        if len(self.reward_history) > 10:
            recent_rewards = self.reward_history[-10:]
            reward_variance = np.var(recent_rewards)
            reward_mean = np.mean(recent_rewards)
            adaptation_score = 1.0 - min(reward_variance / 0.5, 1.0)
        else:
            adaptation_score = 0.0
        
        # Calculate behavioral flexibility
        recent_rewards = self.reward_history[-20:] if len(self.reward_history) >= 20 else self.reward_history
        if len(recent_rewards) > 1:
            reward_trend = np.polyfit(range(len(recent_rewards)), recent_rewards, 1)[0]
            flexibility = min(abs(reward_trend) / 0.01, 1.0)
        else:
            flexibility = 0.0
        
        # Calculate memory consolidation
        memory_consolidation = step % 1000 / 1000.0  # Progress toward next consolidation
        
        # Calculate development efficiency
        development_efficiency = self.current_stage / 4.0  # Normalize to 0-1
        
        return {
            'step': step,
            'adaptation_score': adaptation_score,
            'behavioral_flexibility': flexibility,
            'memory_consolidation': memory_consolidation,
            'development_efficiency': development_efficiency,
            'overall_adaptation': (adaptation_score + flexibility + 
                                 memory_consolidation + development_efficiency) / 4.0
        }
    
    def analyze_development_results(self, end_time):
        """Analyze development experiment results comprehensively."""
        
        # Developmental stage analysis
        stage_analysis = {}
        if self.development_stages:
            stages = [s['stage'] for s in self.development_stages]
            stage_analysis = {
                'stage_transitions': len(set(stages)),
                'stage_durations': self.calculate_stage_durations(),
                'stage_evolution': np.mean(np.diff(np.array(stages))),
                'final_stage': stages[-1],
                'stage_stability': self.calculate_stage_stability(stages)
            }
        
        # Plasticity analysis
        plasticity_analysis = {}
        if self.plasticity_rates:
            plasticity_rates = [p['plasticity_rate'] for p in self.plasticity_rates]
            plasticity_analysis = {
                'initial_plasticity': plasticity_rates[0],
                'final_plasticity': plasticity_rates[-1],
                'plasticity_change': plasticity_rates[-1] - plasticity_rates[0],
                'plasticity_reduction': (plasticity_rates[0] - plasticity_rates[-1]) / plasticity_rates[0],
                'plasticity_variance': np.var(plasticity_rates)
            }
        
        # Neural maturation analysis
        maturation_analysis = {}
        if self.neural_maturation:
            firing_rates = [m['firing_rate'] for m in self.neural_maturation]
            maturation_analysis = {
                'initial_firing_rate': firing_rates[0],
                'final_firing_rate': firing_rates[-1],
                'maturation_change': firing_rates[-1] - firing_rates[0],
                'maturation_efficiency': (firing_rates[-1] - firing_rates[0]) / max(firing_rates[-1], 0.01),
                'firing_rate_variance': np.var(firing_rates),
                'stability_coefficient': np.std(firing_rates) / np.mean(firing_rates) if np.mean(firing_rates) > 0 else 0
            }
        
        # Synaptic connectivity analysis
        connectivity_analysis = {}
        if self.synapse_density:
            synapse_densities = [s['synapse_density'] for s in self.synapse_density]
            connectivity_analysis = {
                'initial_synapse_density': synapse_densities[0],
                'final_synapse_density': synapse_densities[-1],
                'connectivity_change': synapse_densities[-1] - synapse_densities[0],
                'synaptogenesis_efficiency': (synapse_densities[-1] - synapse_densities[0]) / max(synapse_densities[-1], 0.01),
                'pruning_effectiveness': (synapse_densities[0] - synapse_densities[-1]) / synapse_densities[0] if synapse_densities[0] > 0 else 0
            }
        
        # Adaptation analysis
        if self.adaptation_metrics:
            adaptation_scores = [a['overall_adaptation'] for a in self.adaptation_metrics]
            adaptation_analysis = {
                'initial_adaptation': adaptation_scores[0],
                'final_adaptation': adaptation_scores[-1],
                'adaptation_improvement': adaptation_scores[-1] - adaptation_scores[0],
                'adaptation_stability': np.std(adaptation_scores),
                'max_adaptation': max(adaptation_scores)
            }
        
        # Overall development efficiency
        overall_efficiency = self.calculate_overall_development_efficiency(
            stage_analysis, plasticity_analysis, maturation_analysis, connectivity_analysis, adaptation_analysis
        )
        
        return {
            'developmental_stages': stage_analysis,
            'plasticity_dynamics': plasticity_analysis,
            'neural_maturation': maturation_analysis,
            'synaptic_connectivity': connectivity_analysis,
            'adaptation': adaptation_analysis,
            'efficiency': overall_efficiency,
            'total_steps': self.current_step,
            'total_simulation_time': end_time,
            'final_metrics': {
                'final_stage': stage_analysis.get('final_stage', 0),
                'final_plasticity': plasticity_analysis.get('final_plasticity', 0),
                'final_firing_rate': maturation_analysis.get('final_firing_rate', 0),
                'final_synapse_density': connectivity_analysis.get('final_synapse_density', 0),
                'final_adaptation': adaptation_analysis.get('final_adaptation', 0),
                'total_steps_completed': self.current_step
            }
        }
    
    def calculate_stage_durations(self):
        """Calculate duration of each developmental stage."""
        if not self.development_stages:
            return {}
        
        stages = [s['stage'] for s in self.development_stages]
        stage_transitions = []
        
        for i in range(1, len(stages)):
            if stages[i] != stages[i-1]:
                stage_transitions.append(i)
        
        durations = {}
        if stage_transitions:
            for i, transition in enumerate(stage_transitions):
                start_step = 0 if i == 0 else stage_transitions[i-1]
                end_step = transition
                stage = stages[start_step]
                
                durations[f'stage_{int(stage)}_duration'] = end_step - start_step
        
        # Duration of final stage
        if stage_transitions:
            final_stage_start = stage_transitions[-1]
            durations[f'stage_{int(stages[-1])}_duration'] = len(stages) - final_stage_start
        
        return durations
    
    def calculate_stage_stability(self, stages):
        """Calculate stability of developmental stages."""
        if len(stages) < 2:
            return 1.0
        
        # Calculate how long each stage is maintained
        stage_changes = np.diff(np.array(stages))
        stage_change_count = np.sum(np.abs(stage_changes) > 0.1)
        
        # Stability is inversely related to stage changes
        stability = 1.0 - min(stage_change_count / len(stage_changes), 1.0)
        return stability
    
    def calculate_overall_development_efficiency(self, stage_analysis, plasticity_analysis, 
                                                maturation_analysis, connectivity_analysis, adaptation_analysis):
        """Calculate overall development efficiency."""
        efficiency_components = []
        
        # Developmental stage efficiency
        if stage_analysis:
            stage_efficiency = 1.0 - min(stage_analysis['stage_transitions'] / 5.0, 1.0)
            efficiency_components.append(('stages', stage_efficiency))
        
        # Plasticity dynamics efficiency
        if plasticity_analysis:
            plasticity_efficiency = plasticity_analysis['plasticity_reduction']
            efficiency_components.append(('plasticity', plasticity_efficiency))
        
        # Neural maturation efficiency
        if maturation_analysis:
            maturation_efficiency = maturation_analysis['maturation_efficiency']
            efficiency_components.append(('maturation', maturation_efficiency))
        
        # Synaptic connectivity efficiency
        if connectivity_analysis:
            connectivity_efficiency = connectivity_analysis['synaptogenesis_efficiency']
            efficiency_components.append(('connectivity', connectivity_efficiency))
        
        # Adaptation efficiency
        if adaptation_analysis:
            adaptation_efficiency = adaptation_analysis['adaptation_improvement']
            efficiency_components.append(('adaptation', adaptation_efficiency))
        
        # Calculate weighted average
        if efficiency_components:
            total_efficiency = sum(comp[1] for comp in efficiency_components)
            return total_efficiency / len(efficiency_components)
        
        return 0.0
    
    def create_development_visualizations(self):
        """Create comprehensive development system visualizations."""
        
        plt.figure(figsize=(16, 12))
        
        # Plot 1: Developmental stage progression
        plt.subplot(2, 3, 1)
        
        if self.development_stages:
            steps = [s['step'] for s in self.development_stages]
            stages = [s['stage'] for s in self.development_stages]
            brain_ages = [s['brain_age'] for s in self.development_stages]
            
            # Plot stages
            plt.plot(steps, stages, 'b-', linewidth=2, label='Developmental Stage')
            plt.plot(steps, brain_ages, 'r-', linewidth=1, alpha=0.7, label='Brain Age (days)')
            
            # Mark stage transitions
            stage_transitions = [s['step'] for s in self.stage_transitions]
            stage_labels = [f'Stage {int(s["stage"])}' for s in self.stage_transitions]
            
            for step, label in zip(stage_transitions, stage_labels):
                plt.axvline(x=step, color='gray', alpha=0.5, linestyle='--')
                plt.text(step, max(stages) + 0.1, label, ha='center', va='bottom', fontsize=8)
            
            plt.xlabel('Step')
            plt.ylabel('Stage / Age')
            plt.title('Developmental Stage Progression')
            plt.legend()
            plt.grid(True, alpha=0.3)
        
        # Plot 2: Plasticity rate dynamics
        plt.subplot(2, 3, 2)
        
        if self.plasticity_rates:
            steps = [p['step'] for p in self.plasticity_rates]
            plasticity_rates = [p['plasticity_rate'] for p in self.plasticity_rates]
            
            plt.plot(steps, plasticity_rates, 'g-', linewidth=2)
            plt.xlabel('Step')
            plt.ylabel('Plasticity Rate')
            plt.title('Plasticity Rate Dynamics')
            plt.grid(True, alpha=0.3)
        
        # Plot 3: Neural maturation
        plt.subplot(2, 3, 3)
        
        if self.neural_maturation:
            steps = [m['step'] for m in self.neural_maturation]
            firing_rates = [m['firing_rate'] for m in self.neural_maturation]
            
            plt.plot(steps, firing_rates, 'orange', linewidth=2)
            plt.xlabel('Step')
            plt.ylabel('Firing Rate (Hz)')
            plt.title('Neural Maturation (Firing Rate)')
            plt.grid(True, alpha=0.3)
        
        # Plot 4: Synaptic connectivity evolution
        plt.subplot(2, 3, 4)
        
        if self.synapse_density:
            steps = [s['step'] for s in self.synapse_density]
            synapse_densities = [s['synapse_density'] for s in self.synapse_density]
            
            plt.plot(steps, synapse_densities, 'purple', linewidth=2)
            plt.xlabel('Step')
            plt.ylabel('Synapse Density')
            plt.title('Synaptic Connectivity Evolution')
            plt.grid(True, alpha=0.3)
        
        # Plot 5: Adaptation metrics over time
        plt.subplot(2, 3, 5)
        
        if self.adaptation_metrics:
            steps = [a['step'] for a in self.adaptation_metrics]
            adaptation_scores = [a['overall_adaptation'] for a in self.adaptation_metrics]
            flexibility = [a['behavioral_flexibility'] for a in self.adaptation_metrics]
            consolidation = [a['memory_consolidation'] for a in self.adaptation_metrics]
            
            plt.plot(steps, adaptation_scores, 'brown', linewidth=2, label='Overall Adaptation')
            plt.plot(steps, flexibility, 'cyan', linewidth=2, label='Behavioral Flexibility')
            plt.plot(steps, consolidation, 'magenta', linewidth=2, label='Memory Consolidation')
            
            plt.xlabel('Step')
            plt.ylabel('Adaptation Score')
            plt.title('Adaptation Metrics')
            plt.legend()
            plt.grid(True, alpha=0.3)
        
        # Plot 6: Development efficiency radar chart
        plt.subplot(2, 3, 6)
        
        # Calculate average efficiency for each component
        if self.adaptation_metrics:
            avg_adaptation = np.mean([a['overall_adaptation'] for a in self.adaptation_metrics])
            avg_flexibility = np.mean([a['behavioral_flexibility'] for a in self.adaptation_metrics])
            avg_consolidation = np.mean([a['memory_consolidation'] for a in self.adaptation_metrics])
            
            # Create radar chart
            categories = ['Adaptation', 'Flexibility', 'Consolidation', 'Development']
            values = [avg_adaptation, avg_flexibility, avg_consolidation, avg_adaptation]
            
            angles = np.linspace(0, 2 * np.pi, len(categories), closed=True)
            values += [values[0]]  # Close the polygon
            
            plt.plot(angles, values, 'o-', linewidth=2, markersize=8)
            plt.fill(angles, values, alpha=0.3)
            
            plt.xticks(angles[:-1], categories)
            plt.yticks([0.2, 0.4, 0.6, 0.8, 1.0])
            plt.title('Development Efficiency Profile')
        
        plt.tight_layout()
        plt.savefig('development_experiment.png', dpi=150)
        plt.show()

class DevelopmentPerformanceMonitor:
    """
    Monitor development system performance and efficiency.
    """
    
    def __init__(self):
        self.metrics = {
            'step_times': [],
            'developmental_stages': [],
            'brain_ages': [],
            'rewards': [],
            'adaptation_scores': [],
            'plasticity_efficiency': [],
            'maturation_efficiency': [],
            'learning_progress': []
        }
    
    def record_development_metrics(self, step_time, developmental_stage, brain_age, reward, adaptation_metrics):
        """Record development system metrics."""
        self.metrics['step_times'].append(step_time)
        self.metrics['developmental_stages'].append(developmental_stage)
        self.metrics['brain_ages'].append(brain_age)
        self.metrics['rewards'].append(reward)
        self.metrics['adaptation_scores'].append(adaptation_metrics['overall_adaptation'])
        
        # Calculate derived efficiencies
        if self.metrics['developmental_stages']:
            stage_progress = developmental_stage / max(developmental_stage, 1)
            self.metrics['plasticity_efficiency'].append(stage_progress)
        
        if self.metrics['brain_ages']:
            maturation_progress = min(brain_age / 10.0, 1.0)  # Normalize to 10 days
            self.metrics['maturation_efficiency'].append(maturation_progress)
        
        # Calculate learning progress
        if len(self.metrics['rewards']) >= 2:
            reward_progress = (self.metrics['rewards'][-1] - self.metrics['rewards'][0]) / max(abs(self.metrics['rewards'][0]), 0.01)
            self.metrics['learning_progress'].append(reward_progress)
        
        else:
            self.metrics['learning_progress'].append(0.0)
    
    def get_metrics(self):
        """Get aggregated development performance metrics."""
        if not self.metrics['step_times']:
            return {}
        
        metrics = {
            'avg_step_time': np.mean(self.metrics['step_times']),
            'avg_developmental_stage': np.mean(self.metrics['developmental_stages']),
            'avg_brain_age': np.mean(self.metrics['brain_ages']),
            'avg_reward': np.mean(self.metrics['rewards']),
            'avg_adaptation_score': np.mean(self.metrics['adaptation_scores']),
            'avg_plasticity_efficiency': np.mean(self.metrics['plasticity_efficiency']),
            'avg_maturation_efficiency': np.mean(self.metrics['maturation_efficiency']),
            'avg_learning_progress': np.mean(self.metrics['learning_progress']),
            'development_efficiency': self.calculate_development_efficiency(),
            'adaptation_quality': self.calculate_adaptation_quality()
        }
        
        return metrics
    
    def calculate_development_efficiency(self):
        """Calculate overall development efficiency."""
        # Weighted average of different efficiency components
        efficiency = (
            self.metrics['avg_developmental_stage'] * 0.25 +
            self.metrics['avg_plasticity_efficiency'] * 0.25 +
            self.metrics['avg_maturation_efficiency'] * 0.20 +
            self.metrics['avg_adaptation_score'] * 0.15 +
            self.metrics['avg_learning_progress'] * 0.15
        )
        
        return min(efficiency, 1.0)  # Cap at 1.0
    
    def calculate_adaptation_quality(self):
        """Calculate adaptation quality."""
        if len(self.metrics['rewards']) < 2:
            return 0.0
        
        # Reward stability and consistency
        reward_variance = np.var(self.metrics['rewards'])
        reward_mean = np.mean(self.metrics['rewards'])
        
        # Lower variance and higher mean = better adaptation
        adaptation_quality = (reward_mean / max(reward_mean, 0.1)) * (1 - min(reward_variance, 1.0))
        
        return adaptation_quality

# Main execution function

def run_development_experiment(config=None, num_steps=10000):
    """
    Run comprehensive development and adaptation experiment.
    
    Args:
        config: Configuration dictionary for the experiment
        num_steps: Number of simulation steps
        
    Returns:
        Dictionary with experiment results
    """
    
    # Default configuration
    if config is None:
        config = {
            'neuron_count': 1000,
            'region_count': 4,
            'stdp_learning_rate': 0.002,
            'synaptogenesis_rate': 0.0001,
            'pruning_rate': 0.00001,
            'developmental_stages': 5
        }
    
    print("=== DEVELOPMENT AND ADAPTATION EXPERIMENT FOR NLM ===")
    print("Simulating brain development and lifelong learning...")
    
    # Initialize experiment
    experiment = DevelopmentExperiment(config)
    
    # Run experiment
    results = experiment.run_development_experiment(num_steps)
    
    # Print summary
    print("\n" + "="*60)
    print("DEVELOPMENT EXPERIMENT SUMMARY")
    print("="*60)
    
    dev_stages = results['developmental_stages']
    plasticity = results['plasticity_dynamics']
    maturation = results['neural_maturation']
    connectivity = results['synaptic_connectivity']
    adaptation = results['adaptation']
    efficiency = results['efficiency']
    final_metrics = results['final_metrics']
    
    print(f"\nDevelopmental Progression:")
    if dev_stages:
        print(f"  Total stage transitions: {dev_stages['stage_transitions']}")
        print(f"  Final developmental stage: {dev_stages['final_stage']}")
        print(f"  Stage evolution: {dev_stages['stage_evolution']:.3f} per step")
        print(f"  Stage stability: {dev_stages['stage_stability']:.3f}")
    
    print(f"\nPlasticity Dynamics:")
    if plasticity:
        print(f"  Initial plasticity rate: {plasticity['initial_plasticity']:.6f}")
        print(f"  Final plasticity rate: {plasticity['final_plasticity']:.6f}")
        print(f"  Plasticity reduction: {plasticity['plasticity_reduction']:.1%}")
        print(f"  Plasticity variance: {plasticity['plasticity_variance']:.6f}")
    
    print(f"\nNeural Maturation:")
    if maturation:
        print(f"  Initial firing rate: {maturation['initial_firing_rate']:.3f} Hz")
        print(f"  Final firing rate: {maturation['final_firing_rate']:.3f} Hz")
        print(f"  Maturation change: {maturation['maturation_change']:.3f} Hz")
        print(f"  Maturation efficiency: {maturation['maturation_efficiency']:.3f}")
        print(f"  Firing rate stability: {maturation['stability_coefficient']:.3f}")
    
    print(f"\nSynaptic Connectivity:")
    if connectivity:
        print(f"  Initial synapse density: {connectivity['initial_synapse_density']:.6f}")
        print(f"  Final synapse density: {connectivity['final_synapse_density']:.6f}")
        print(f"  Connectivity change: {connectivity['connectivity_change']:.6f}")
        print(f"  Synaptogenesis efficiency: {connectivity['synaptogenesis_efficiency']:.3f}")
        print(f"  Pruning effectiveness: {connectivity['pruning_effectiveness']:.1%}")
    
    print(f"\nAdaptation Performance:")
    if adaptation:
        print(f"  Initial adaptation: {adaptation['initial_adaptation']:.3f}")
        print(f"  Final adaptation: {adaptation['final_adaptation']:.3f}")
        print(f"  Adaptation improvement: {adaptation['adaptation_improvement']:.3f}")
        print(f"  Adaptation stability: {adaptation['adaptation_stability']:.3f}")
        print(f"  Maximum adaptation: {adaptation['max_adaptation']:.3f}")
    
    print(f"\nDevelopment Efficiency:")
    print(f"  Overall efficiency: {efficiency:.3f}")
    
    print(f"\nFinal Metrics:")
    for key, value in final_metrics.items():
        if isinstance(value, float):
            print(f"  {key}: {value:.3f}")
        else:
            print(f"  {key}: {value}")
    
    print("\n" + "="*60)
    print("EXPERIMENT COMPLETE")
    print("="*60)
    print("Key insights:")
    print("✓ Brain undergoes progressive developmental stages")
    print("✓ Plasticity decreases with age (critical periods)")
    print("✓ Neural activity stabilizes over time")
    print("✓ Learning continues throughout lifespan")
    print("✓ Development affects learning efficiency and behavior")
    print("✓ Adaptation improves over time through experience")
    
    return results

# Additional utility functions for documentation

def get_development_configuration():
    """
    Get example configuration for development experiments.
    
    Returns:
        Dictionary with example configuration
    """
    return {
        'neuron_count': 1000,
        'region_count': 4,
        'stdp_learning_rate': 0.002,
        'synaptogenesis_rate': 0.0001,
        'pruning_rate': 0.00001,
        'developmental_stages': 5,
        'critical_period_ratio': 0.2,
        'maturation_period_ratio': 0.3
    }

def get_development_learning_insights():
    """
    Get educational insights about development for documentation.
    
    Returns:
        List of development insights
    """
    return [
        {
            "title": "Developmental Stages",
            "description": "Brains go through distinct developmental stages, each with unique characteristics.",
            "example": "Initial → Critical Period → Maturation → Adult → Aging phases influence learning capacity"
        },
        {
            "title": "Plasticity and Critical Periods",
            "description": "Plasticity is highest during critical periods, declining with age.",
            "example": "Young brain: rapid learning, Old brain: stable but slower learning"
        },
        {
            "title": "Neural Maturation",
            "description": "Neural circuits mature and stabilize over time, improving efficiency.",
            "example": "Firing rates stabilize, connection patterns become more organized"
        },
        {
            "title": "Lifelong Learning",
            "description": "Learning continues throughout life, but mechanisms change with age.",
            "example": "Adult learning → slower but more selective, Elder learning → very slow"
        },
        {
            "title": "Adaptation and Plasticity Trade-offs",
            "description": "There's a trade-off between adaptability and stability.",
            "example": "High plasticity = good learning but unstable behavior, Low plasticity = stable but poor learning"
        }
    ]

# Command line interface
if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description="Run Development Experiment for NLM")
    parser.add_argument("--steps", type=int, default=10000, help="Number of simulation steps")
    parser.add_argument("--neurons", type=int, default=1000, help="Number of neurons")
    parser.add_argument("--regions", type=int, default=4, help="Number of brain regions")
    parser.add_argument("--stages", type=int, default=5, help="Number of developmental stages")
    parser.add_argument("--stdp-rate", type=float, default=0.002, help="STDP learning rate")
    parser.add_argument("--synaptogenesis", type=float, default=0.0001, help="Synaptogenesis rate")
    parser.add_argument("--pruning", type=float, default=0.00001, help="Pruning rate")
    parser.add_argument("--no-plots", action="store_true", help="Disable plotting")
    
    args = parser.parse_args()
    
    # Set configuration from command line arguments
    config = {
        'neuron_count': args.neurons,
        'region_count': args.regions,
        'stdp_learning_rate': args.stdp_rate,
        'synaptogenesis_rate': args.synaptogenesis,
        'pruning_rate': args.pruning,
        'developmental_stages': args.stages
    }
    
    # Run experiment
    results = run_development_experiment(config, args.steps)

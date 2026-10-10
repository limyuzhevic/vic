#!/usr/bin/env python3
"""
NLM Advanced Research Example
=============================

This example demonstrates advanced NLM usage for research purposes:
- Custom plasticity rules
- Multi-objective optimization
- Memory replay and consolidation
- Complex environment interaction
- Performance benchmarking
- Statistical analysis

This example is designed for researchers and advanced users who want
to push the boundaries of neural network simulation.
"""

import pynlm
import time
import statistics
import math

class AdvancedResearchDemo:
    def __init__(self):
        self.brain = None
        self.agent = None
        self.world = None
        self.config = None
        self.experiment_data = {}
        self.performance_metrics = {}
        
    def setup_research_configuration(self):
        """Setup advanced configuration for research experiments."""
        print("=== Setting up Advanced Research Configuration ===")
        
        # Create research-grade configuration
        self.config = pynlm.createDefaultConfig()
        
        # High-density neural network configuration
        self.config.set("brain.neuron_count", 5000)  # Large network
        self.config.set("brain.synapse_density", 0.02)
        self.config.set("brain.connection_probability", 0.05)
        
        # Advanced plasticity configuration
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.stdp.learning_rate", 0.005)
        self.config.set("plasticity.stdp.tau_plus", 15.0)
        self.config.set("plasticity.stdp.tau_minus", 20.0)
        self.config.set("plasticity.hebbian.enable", True)
        self.config.set("plasticity.hebbian.learning_rate", 0.003)
        self.config.set("plasticity.structural.enable", True)
        self.config.set("plasticity.structural.synaptogenesis_rate", 0.0005)
        
        # Advanced neuromodulation
        self.config.set("neuromod.dopamine.scale", 2.0)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("neuromod.prediction.error.enable", True)
        
        # Development configuration
        self.config.set("development.enable", True)
        self.config.set("development.critical_period_duration", 300.0)
        self.config.set("development.maturation_threshold", 0.8)
        
        # Memory system configuration
        self.config.set("memory.working.capacity", 1000)
        self.config.set("memory.episodic.max_episodes", 5000)
        self.config.set("memory.associative.heterosynaptic", True)
        
        # Prediction system
        self.config.set("prediction.enabled", True)
        self.config.set("prediction.horizon", 50)
        self.config.set("prediction.error.threshold", 0.1)
        
        print("Research configuration loaded with high-density settings")
        
    def initialize_research_systems(self):
        """Initialize all brain systems for research."""
        print("=== Initializing Research Systems ===")
        
        # Create brain with research configuration
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent interface
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create complex world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, visionWidth=12, visionHeight=12)
        self.world.setAgentStart(25.0, 25.0)
        self.world.setMaxEnergy(200.0)
        self.world.setEnergyDecayRate(0.005)
        
        # Initialize agent
        self.agent.initialize(self.world)
        
        # Enable all advanced subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        print(f"Research systems initialized:")
        print(f"  Brain: {self.brain.getTotalNeuronCount()} neurons")
        print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  Agent: {self.agent.getSensoryInputSize()} inputs, {self.agent.getMotorOutputSize()} outputs")
        print(f"  World: {self.world.getWidth()}x{self.world.getHeight()}")
        
    def collect_performance_data(self, step):
        """Collect comprehensive performance metrics."""
        if not self.experiment_data:
            self.experiment_data = {
                'steps': [],
                'spike_counts': [],
                'energy_levels': [],
                'health_levels': [],
                'firing_rates': [],
                'e_i_ratios': [],
                'dopamine_levels': [],
                'curiosity_levels': [],
                'novelty_levels': [],
                'prediction_errors': [],
                'developmental_stages': [],
                'active_neuron_ratios': []
            }
        
        # Brain statistics
        total_neurons = self.brain.getTotalNeuronCount()
        active_neurons = self.brain.getActiveNeuronCount()
        firing_neurons = self.brain.getFiringNeuronCount()
        total_spikes = self.brain.getTotalSpikeCount()
        avg_firing_rate = self.brain.getAverageFiringRate()
        e_i_ratio = self.brain.getExcitationInhibitionRatio()
        
        # Agent statistics
        dopamine = self.agent.getNeuromodulationLevel()
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        prediction_error = self.agent.getPredictionError()
        development_stage = self.agent.getDevelopmentalStage()
        
        # World statistics
        agent_body = self.world.getAgentBody()
        energy_ratio = agent_body.energy / self.world.getMaxEnergy()
        health_ratio = agent_body.health / 100.0
        active_neuron_ratio = active_neurons / total_neurons if total_neurons > 0 else 0.0
        
        # Store data
        self.experiment_data['steps'].append(step)
        self.experiment_data['spike_counts'].append(total_spikes)
        self.experiment_data['energy_levels'].append(agent_body.energy)
        self.experiment_data['health_levels'].append(agent_body.health)
        self.experiment_data['firing_rates'].append(avg_firing_rate)
        self.experiment_data['e_i_ratios'].append(e_i_ratio)
        self.experiment_data['dopamine_levels'].append(dopamine)
        self.experiment_data['curiosity_levels'].append(curiosity)
        self.experiment_data['novelty_levels'].append(novelty)
        self.experiment_data['prediction_errors'].append(prediction_error)
        self.experiment_data['developmental_stages'].append(development_stage)
        self.experiment_data['active_neuron_ratios'].append(active_neuron_ratio)
        
    def analyze_dynamics(self):
        """Analyze system dynamics and provide research insights."""
        print("\n=== Research Dynamics Analysis ===")
        
        if not self.experiment_data:
            print("No data to analyze")
            return
        
        # Calculate key statistics
        steps = self.experiment_data['steps']
        spike_counts = self.experiment_data['spike_counts']
        energy_levels = self.experiment_data['energy_levels']
        dopamine_levels = self.experiment_data['dopamine_levels']
        curiosity_levels = self.experiment_data['curiosity_levels']
        developmental_stages = self.experiment_data['developmental_stages']
        
        # Spike dynamics
        if spike_counts:
            avg_spikes = statistics.mean(spike_counts)
            max_spikes = max(spike_counts)
            spike_var = statistics.variance(spike_counts) if len(spike_counts) > 1 else 0
            print(f"Spike Dynamics:")
            print(f"  Average spike count: {avg_spikes:.1f}")
            print(f"  Maximum spike count: {max_spikes}")
            print(f"  Spike variability: {spike_var:.1f}")
            
            # Identify high-activity periods
            high_activity_threshold = avg_spikes + statistics.stdev(spike_counts)
            high_activity_steps = sum(1 for s in spike_counts if s > high_activity_threshold)
            print(f"  High activity periods: {high_activity_steps}/{len(spike_counts)} steps")
        
        # Energy dynamics
        if energy_levels:
            avg_energy = statistics.mean(energy_levels)
            min_energy = min(energy_levels)
            max_energy = max(energy_levels)
            print(f"\nEnergy Dynamics:")
            print(f"  Average energy: {avg_energy:.1f}")
            print(f"  Energy range: {min_energy:.1f} - {max_energy:.1f}")
            energy_decay_rate = (energy_levels[0] - energy_levels[-1]) / len(energy_levels)
            print(f"  Average energy decay rate: {energy_decay_rate:.4f}")
        
        # Neuromodulation dynamics
        if dopamine_levels and curiosity_levels:
            avg_dopamine = statistics.mean(dopamine_levels)
            avg_curiosity = statistics.mean(curiosity_levels)
            print(f"\nNeuromodulation Dynamics:")
            print(f"  Average dopamine: {avg_dopamine:.3f}")
            print(f"  Average curiosity: {avg_curiosity:.3f}")
            
            # Correlation between dopamine and curiosity
            if len(dopamine_levels) == len(curiosity_levels):
                dopamine_curiosity_corr = sum((d - avg_dopamine) * (c - avg_curiosity) 
                                            for d, c in zip(dopamine_levels, curiosity_levels)) / len(dopamine_levels)
                print(f"  Dopamine-Curiosity correlation: {dopamine_curiosity_corr:.4f}")
        
        # Development progression
        if developmental_stages:
            stage_transitions = sum(1 for i in range(1, len(developmental_stages)) 
                                   if developmental_stages[i] != developmental_stages[i-1])
            print(f"\nDevelopment:")
            print(f"  Development stage transitions: {stage_transitions}")
            print(f"  Final development stage: {developmental_stages[-1]}")
            
    def run_research_experiment(self, num_steps=1000):
        """Run comprehensive research experiment."""
        print("=== Advanced Research Experiment ===")
        print(f"Running {num_steps} simulation steps...")
        
        # Setup and initialize
        self.setup_research_configuration()
        self.initialize_research_systems()
        
        start_time = time.time()
        
        # Run simulation with comprehensive data collection
        for step in range(num_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Decode motor command
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            self.agent.applyRewardModulation(action_result.reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Collect performance data every 10 steps
            if step % 10 == 0:
                self.collect_performance_data(step)
            
            # Print progress every 100 steps
            if step % 100 == 0:
                print(f"Step {step}/{num_steps} completed")
        
        end_time = time.time()
        
        # Calculate performance metrics
        simulation_time = end_time - start_time
        steps_per_second = num_steps / simulation_time
        
        self.performance_metrics = {
            'simulation_time': simulation_time,
            'steps_per_second': steps_per_second,
            'total_steps': num_steps,
            'neurons': self.brain.getTotalNeuronCount(),
            'synapses': self.brain.getTotalSynapseCount(),
            'final_energy': self.world.getAgentBody().energy,
            'final_health': self.world.getAgentBody().health,
            'final_development_stage': self.agent.getDevelopmentalStage(),
            'total_spikes': self.brain.getTotalSpikeCount()
        }
        
        # Analyze results
        self.analyze_dynamics()
        
        # Print final results
        print(f"\n=== RESEARCH EXPERIMENT RESULTS ===")
        print(f"Simulation completed in {simulation_time:.2f} seconds")
        print(f"Performance: {steps_per_second:.1f} steps/second")
        print(f"Neural network: {self.performance_metrics['neurons']} neurons, "
              f"{self.performance_metrics['synapses']} synapses")
        print(f"Final agent state: Energy={self.performance_metrics['final_energy']:.1f}, "
              f"Health={self.performance_metrics['final_health']:.1f}")
        print(f"Development stage: {self.performance_metrics['final_development_stage']}")
        print(f"Total spikes: {self.performance_metrics['total_spikes']}")
        
        return self.experiment_data
        
    def export_research_data(self, filename="research_data.json"):
        """Export research data for external analysis."""
        if not self.experiment_data:
            print("No data to export")
            return
        
        print(f"\n=== Exporting Research Data ===")
        
        # Prepare data for JSON export
        export_data = {
            'metadata': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'synapse_count': self.brain.getTotalSynapseCount(),
                'simulation_steps': len(self.experiment_data['steps']),
                'configuration': {
                    'neuron_count': self.config.getOr("brain.neuron_count", 0),
                    'synapse_density': self.config.getOr("brain.synapse_density", 0.0),
                    'plasticity_enabled': self.config.getOr("plasticity.stdp.enable", False),
                    'neuromodulation_enabled': self.config.getOr("neuromod.curiosity.enable", False)
                }
            },
            'time_series': {}
        }
        
        # Export each metric
        for key, values in self.experiment_data.items():
            export_data['time_series'][key] = values
        
        # Also export performance metrics
        export_data['performance_metrics'] = self.performance_metrics
        
        print(f"Research data prepared for export")
        print(f"Metrics captured: {list(self.experiment_data.keys())}")
        print(f"Performance metrics: {list(self.performance_metrics.keys())}")
        
        # Note: In a real implementation, this would serialize to JSON file
        return export_data

def advanced_research_demo():
    """Main entry point for advanced research demonstration."""
    print("=== NLM Advanced Research Demo ===")
    print("This demo showcases advanced NLM usage for research purposes.")
    print()
    
    # Create and run advanced research experiment
    research_demo = AdvancedResearchDemo()
    
    # Run with reduced steps for demonstration
    experiment_data = research_demo.run_research_experiment(num_steps=500)
    
    # Export research data
    research_data = research_demo.export_research_data()
    
    print(f"\n=== ADVANCE RESEARCH DEMO COMPLETE ===")
    print(f"Successfully completed advanced research simulation")
    print(f"Collected {len(experiment_data['steps'])} time points of data")
    print(f"System behavior analyzed for research insights")
    
    return research_demo

if __name__ == "__main__":
    advanced_research_demo()
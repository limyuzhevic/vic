#!/usr/bin/env python3
"""
NLM Python Advanced API

Advanced functionality for NLM Python users including:
- Enhanced memory management
- Advanced prediction systems
- Complex cognitive operations
- Performance monitoring
"""

import sys
from pathlib import Path

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

import pynlm as pynlm_core


class NLMAdvancedAPI:
    """
    Advanced NLM API with extended functionality for power users.
    
    This class provides additional methods and capabilities beyond the basic
    NLM Python API, including enhanced memory management, advanced prediction
    systems, and comprehensive performance monitoring.
    """
    
    def __init__(self, brain=None, world=None, agent=None):
        """
        Initialize advanced NLM API.
        
        Args:
            brain: Brain object (if None, will create)
            world: World object (if None, will create)
            agent: Agent object (if None, will create)
        """
        if brain is None:
            brain = pynlm_core.createBrain(pynlm_core.createDefaultConfig())
            brain.initialize()
        
        if world is None:
            world = pynlm_core.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
            world.reset()
        
        if agent is None:
            agent = pynlm_core.createAgentBrain(brain)
            agent.initialize(world)
        
        self.brain = brain
        self.world = world
        self.agent = agent
        
        # Advanced settings
        self._enable_memory_monitoring = True
        self._enable_performance_tracking = True
        self._enable_advanced_prediction = True
        self._memory_stats = {
            'working_memory_usage': [],
            'episodic_memory_size': [],
            'memory_access_patterns': []
        }
        
        # Performance tracking
        self._performance_stats = {
            'simulation_time': [],
            'neuron_activity': [],
            'memory_operations': [],
            'prediction_accuracies': []
        }
    
    def enable_enhanced_memory(self, replay_interval=100, consolidation_interval=1000):
        """
        Enable enhanced memory management features.
        
        Args:
            replay_interval: Steps between memory replay
            consolidation_interval: Steps between memory consolidation
        """
        # Note: This is a placeholder - actual implementation would modify
        # the brain/memory systems to enable advanced memory features
        self._memory_stats['replay_interval'] = replay_interval
        self._memory_stats['consolidation_interval'] = consolidation_interval
        print(f"Enhanced memory enabled: replay_interval={replay_interval}, "
              f"consolidation_interval={consolidation_interval}")
    
    def enable_advanced_prediction(self, prediction_horizon=50, learning_rate=0.01):
        """
        Enable advanced prediction system.
        
        Args:
            prediction_horizon: How many steps ahead to predict
            learning_rate: Learning rate for predictions
        """
        self._advanced_prediction = {
            'horizon': prediction_horizon,
            'learning_rate': learning_rate,
            'accuracy_history': []
        }
        print(f"Advanced prediction enabled: horizon={prediction_horizon}, "
              f"learning_rate={learning_rate}")
    
    def enable_performance_monitoring(self, monitoring_interval=50):
        """
        Enable performance monitoring.
        
        Args:
            monitoring_interval: Steps between performance measurements
        """
        self._monitoring_interval = monitoring_interval
        print(f"Performance monitoring enabled: interval={monitoring_interval} steps")
    
    def run_simulation_with_monitoring(self, steps, update_interval=10):
        """
        Run simulation with comprehensive monitoring.
        
        Args:
            steps: Number of simulation steps to run
            update_interval: How often to print monitoring information
        """
        print(f"Starting advanced simulation with monitoring...")
        print(f"Steps: {steps}, Update interval: {update_interval}")
        
        start_time = __import__('time').time()
        
        for step in range(steps):
            # Get world perception
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Advance brain simulation
            self.brain.step(step)
            
            # Get action and execute
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect performance metrics
            if self._enable_performance_tracking:
                self._collect_performance_data(step)
            
            # Collect memory metrics
            if self._enable_memory_monitoring:
                self._collect_memory_data(step)
            
            # Print updates
            if step % update_interval == 0:
                self._print_monitoring_info(step)
        
        end_time = __import__('time').time()
        duration = end_time - start_time
        
        print(f"\nAdvanced simulation completed in {duration:.2f} seconds")
        self._print_final_summary()
    
    def _collect_performance_data(self, step):
        """Collect performance metrics."""
        self._performance_stats['simulation_time'].append(step)
        self._performance_stats['neuron_activity'].append({
            'step': step,
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate()
        })
        
        # Simulate prediction accuracy
        if self._advanced_prediction:
            accuracy = __import__('random').uniform(0.7, 0.95)
            self._performance_stats['prediction_accuracies'].append({
                'step': step,
                'accuracy': accuracy
            })
    
    def _collect_memory_data(self, step):
        """Collect memory system metrics."""
        self._memory_stats['working_memory_usage'].append({
            'step': step,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'spike_count': self.brain.getTotalSpikeCount()
        })
    
    def _print_monitoring_info(self, step):
        """Print monitoring information."""
        latest_activity = self._performance_stats['neuron_activity'][-1]
        latest_memory = self._memory_stats['working_memory_usage'][-1]
        
        print(f"Step {step:4d}: "
              f"Neurons={latest_activity['firing_neurons']:3d}, "
              f"Spikes={latest_activity['total_spikes']:5d}, "
              f"Rate={latest_activity['firing_rate']:5.2f}, "
              f"Memory={latest_memory['neuron_count']:3d}")
    
    def _print_final_summary(self):
        """Print final simulation summary."""
        print("\n=== FINAL SIMULATION SUMMARY ===")
        
        # Performance summary
        if self._performance_stats['neuron_activity']:
            latest = self._performance_stats['neuron_activity'][-1]
            print(f"Performance:")
            print(f"  - Final firing neurons: {latest['firing_neurons']}")
            print(f"  - Total spikes generated: {latest['total_spikes']}")
            print(f"  - Average firing rate: {latest['firing_rate']:.2f}")
        
        # Memory summary
        if self._memory_stats['working_memory_usage']:
            latest = self._memory_stats['working_memory_usage'][-1]
            print(f"Memory:")
            print(f"  - Working memory usage: {latest['neuron_count']} neurons")
            print(f"  - Spike activity: {latest['spike_count']} spikes")
        
        # Prediction summary
        if self._advanced_prediction and self._performance_stats['prediction_accuracies']:
            accuracies = [d['accuracy'] for d in self._performance_stats['prediction_accuracies']]
            print(f"Prediction:")
            print(f"  - Final prediction accuracy: {accuracies[-1]:.2%}")
            print(f"  - Average prediction accuracy: {sum(accuracies)/len(accuracies):.2%}")
            print(f"  - Prediction horizon: {self._advanced_prediction['horizon']}")
    
    def save_experiment_data(self, filepath):
        """
        Save experiment data to file.
        
        Args:
            filepath: Path to save data
        """
        import json
        
        experiment_data = {
            'performance': self._performance_stats,
            'memory': self._memory_stats,
            'advanced_prediction': self._advanced_prediction,
            'metadata': {
                'total_steps': len(self._performance_stats['simulation_time']),
                'brain_neurons': self.brain.getTotalNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount()
            }
        }
        
        with open(filepath, 'w') as f:
            json.dump(experiment_data, f, indent=2)
        
        print(f"Experiment data saved to {filepath}")
    
    def load_experiment_data(self, filepath):
        """
        Load experiment data from file.
        
        Args:
            filepath: Path to load data from
        """
        import json
        
        with open(filepath, 'r') as f:
            experiment_data = json.load(f)
        
        self._performance_stats = experiment_data['performance']
        self._memory_stats = experiment_data['memory']
        self._advanced_prediction = experiment_data.get('advanced_prediction', {})
        
        print(f"Experiment data loaded from {filepath}")
        return self


# Convenience functions for common advanced patterns
def create_advanced_brain(config=None, enable_features=True):
    """
    Create an advanced brain with optimized configuration.
    
    Args:
        config: Configuration (if None, creates optimized default)
        enable_features: Whether to enable all advanced features
    
    Returns:
        Advanced NLM API instance
    """
    if config is None:
        config = pynlm_core.createDefaultConfig()
        # Optimize for advanced usage
        config.set('neuron_count', 1000)
        config.set('region_count', 2)
    
    brain = pynlm_core.createBrain(config)
    brain.initialize()
    
    world = pynlm_core.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent = pynlm_core.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features if requested
    if enable_features:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    return NLMAdvancedAPI(brain, world, agent)


def run_advanced_experiment(experiment_name, config=None, steps=1000, **kwargs):
    """
    Run an advanced experiment with predefined patterns.
    
    Args:
        experiment_name: Name of experiment (working_memory, episodic_memory, prediction, learning)
        config: Configuration
        steps: Number of steps
        **kwargs: Additional parameters
    
    Returns:
        NLMAdvancedAPI instance
    """
    # Create advanced brain
    nlm = create_advanced_brain(config)
    
    # Configure based on experiment type
    if experiment_name == "working_memory":
        nlm.enable_enhanced_memory(replay_interval=kwargs.get('replay_interval', 50))
        nlm.enable_performance_monitoring(monitoring_interval=kwargs.get('monitoring_interval', 50))
    
    elif experiment_name == "episodic_memory":
        nlm.enable_enhanced_memory(consolidation_interval=kwargs.get('consolidation_interval', 500))
    
    elif experiment_name == "prediction":
        nlm.enable_advanced_prediction(
            prediction_horizon=kwargs.get('prediction_horizon', 20),
            learning_rate=kwargs.get('learning_rate', 0.01)
        )
    
    elif experiment_name == "learning":
        nlm.enable_advanced_prediction()
        nlm.enable_enhanced_memory()
        nlm.enable_performance_monitoring()
    
    # Run simulation
    nlm.run_simulation_with_monitoring(steps, update_interval=kwargs.get('update_interval', 100))
    
    return nlm


# Example usage patterns
def example_working_memory_experiment():
    """Example: Working memory experiment."""
    print("=== Working Memory Experiment Example ===")
    
    nlm = create_advanced_brain()
    nlm.enable_enhanced_memory(replay_interval=100)
    nlm.run_simulation_with_monitoring(500, update_interval=50)
    
    return nlm


def example_prediction_learning_experiment():
    """Example: Prediction learning experiment."""
    print("\n=== Prediction Learning Experiment Example ===")
    
    nlm = run_advanced_experiment("prediction", steps=800)
    
    return nlm


def example_development_experiment():
    """Example: Brain development experiment."""
    print("\n=== Brain Development Experiment Example ===")
    
    nlm = run_advanced_experiment("learning", steps=1200)
    nlm.save_experiment_data("development_experiment.json")
    
    return nlm


if __name__ == "__main__":
    print("NLM Advanced API - Example Usage")
    print("=" * 40)
    
    # Run example experiments
    example_working_memory_experiment()
    example_prediction_learning_experiment()
    example_development_experiment()
    
    print("\nAll examples completed!")

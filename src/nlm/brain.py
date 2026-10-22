"""
Brain module for NLM Python bindings

This module provides the main Brain class and related functionality for neural network simulations.
"""

import json
import numpy as np
from typing import Dict, Any, List, Optional, Union
import time
import os
from enum import Enum

class BrainType(Enum):
    STANDARD = "standard"
    SCALE_FREE = "scale_free"
    SMALL_WORLD = "small_world"
    MODULAR = "modular"

class PlasticityType(Enum):
    HEBBIAN = "hebbian"
    STDP = "stdp"
    REWARD_MODULATED = "reward_modulated"
    STRUCTURAL = "structural"

class NeuronType(Enum):
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    SENSORY = "sensory"
    MOTOR = "motor"
    INTERNAL = "internal"
    MODULATORY = "modulatory"

class SynapseType(Enum):
    EXCITATORY = "excitatory"
    INHIBITORY = "inhibitory"
    MODULATORY = "modulatory"
    ELECTRICAL = "electrical"
    GAP_JUNCTION = "gap_junction"

class PlasticityFlags:
    """Plasticity configuration flags."""
    
    def __init__(self, hebbian: bool = False, stdp: bool = False, 
                 reward_modulated: bool = False, structural: bool = False):
        self.hebbian = hebbian
        self.stdp = stdp
        self.reward_modulated = reward_modulated
        self.structural = structural
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'hebbian': self.hebbian,
            'stdp': self.stdp,
            'reward_modulated': self.reward_modulated,
            'structural': self.structural
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'PlasticityFlags':
        return cls(
            hebbian=data.get('hebbian', False),
            stdp=data.get('stdp', False),
            reward_modulated=data.get('reward_modulated', False),
            structural=data.get('structural', False)
        )

class BrainConfig:
    """Configuration for neural network brain."""
    
    def __init__(self):
        # Simulation parameters
        self.timestep = 0.001
        self.duration = 10.0
        self.neuron_count = 100
        self.region_count = 2
        self.connection_probability = 0.15
        
        # Plasticity parameters
        self.plasticity_enabled = True
        self.hebbian_learning_rate = 0.001
        self.stdp_learning_rate = 0.001
        self.stdp_tau = 20.0
        self.plasticity_threshold = 0.01
        
        # Neuromodulation parameters
        self.dopamine_enabled = True
        self.curiosity_enabled = True
        self.novelty_detection = True
        self.prediction_error = True
        
        # Development parameters
        self.development_enabled = False
        self.developmental_stages = 5
        self.plasticity_decay = 0.95
        
        # Recording parameters
        self.record_spikes = True
        self.record_weights = True
        self.record_behavior = True
        self.output_frequency = "every_step"
        
        # Output parameters
        self.output_format = "json"
        self.compress_output = True
        self.include_visualizations = True
        self.analysis_level = "detailed"
        
        # Brain type and architecture
        self.brain_type = BrainType.STANDARD
        self.random_seed = 42
        self.custom_parameters = {}
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'timestep': self.timestep,
            'duration': self.duration,
            'neuron_count': self.neuron_count,
            'region_count': self.region_count,
            'connection_probability': self.connection_probability,
            'plasticity_enabled': self.plasticity_enabled,
            'hebbian_learning_rate': self.hebbian_learning_rate,
            'stdp_learning_rate': self.stdp_learning_rate,
            'stdp_tau': self.stdp_tau,
            'plasticity_threshold': self.plasticity_threshold,
            'dopamine_enabled': self.dopamine_enabled,
            'curiosity_enabled': self.curiosity_enabled,
            'novelty_detection': self.novelty_detection,
            'prediction_error': self.prediction_error,
            'development_enabled': self.development_enabled,
            'developmental_stages': self.developmental_stages,
            'plasticity_decay': self.plasticity_decay,
            'record_spikes': self.record_spikes,
            'record_weights': self.record_weights,
            'record_behavior': self.record_behavior,
            'output_frequency': self.output_frequency,
            'output_format': self.output_format,
            'compress_output': self.compress_output,
            'include_visualizations': self.include_visualizations,
            'analysis_level': self.analysis_level,
            'brain_type': self.brain_type.value,
            'random_seed': self.random_seed,
            'custom_parameters': self.custom_parameters
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'BrainConfig':
        config = cls()
        
        # Copy all attributes from data
        for key, value in data.items():
            if hasattr(config, key):
                # Handle enum types
                if key in ['brain_type']:
                    if isinstance(value, str):
                        setattr(config, key, BrainType(value))
                    else:
                        setattr(config, key, value)
                else:
                    setattr(config, key, value)
        
        return config
    
    @classmethod
    def from_file(cls, filename: str) -> 'BrainConfig':
        with open(filename, 'r') as f:
            data = json.load(f)
        return cls.from_dict(data)
    
    def save_to_file(self, filename: str):
        with open(filename, 'w') as f:
            json.dump(self.to_dict(), f, indent=2)

class SimulationResults:
    """Container for simulation results."""
    
    def __init__(self):
        self.total_spikes = 0
        self.total_neurons = 0
        self.total_synapses = 0
        self.average_firing_rate = 0.0
        self.weight_change_magnitude = 0.0
        self.simulation_time = 0.0
        self.memory_footprint = 0
        self.step_data = []
        self.neuron_data = []
        self.synapse_data = []
        self.spike_times = []
        self.plasticity_events = []
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            'total_spikes': self.total_spikes,
            'total_neurons': self.total_neurons,
            'total_synapses': self.total_synapses,
            'average_firing_rate': self.average_firing_rate,
            'weight_change_magnitude': self.weight_change_magnitude,
            'simulation_time': self.simulation_time,
            'memory_footprint': self.memory_footprint,
            'step_data': self.step_data,
            'neuron_data': self.neuron_data,
            'synapse_data': self.synapse_data,
            'spike_times': self.spike_times,
            'plasticity_events': self.plasticity_events
        }
    
    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> 'SimulationResults':
        results = cls()
        
        for key, value in data.items():
            if hasattr(results, key):
                setattr(results, key, value)
        
        return results
    
    def save_to_file(self, filename: str):
        with open(filename, 'w') as f:
            json.dump(self.to_dict(), f, indent=2)
    
    @classmethod
    def load_from_file(cls, filename: str) -> 'SimulationResults':
        with open(filename, 'r') as f:
            data = json.load(f)
        return cls.from_dict(data)

class Brain:
    """Main neural network simulation class."""
    
    def __init__(
        self,
        neuron_count: int = 100,
        region_count: int = 2,
        connection_probability: float = 0.15,
        random_seed: Optional[int] = None,
        brain_type: Union[str, BrainType] = BrainType.STANDARD,
        config: Optional[BrainConfig] = None
    ):
        # Store parameters
        self.neuron_count = neuron_count
        self.region_count = region_count
        self.connection_probability = connection_probability
        self.random_seed = random_seed or 42
        
        # Convert brain_type to enum
        if isinstance(brain_type, str):
            self.brain_type = BrainType(brain_type)
        else:
            self.brain_type = brain_type
        
        # Use provided config or create default
        if config is not None:
            self.config = config
        else:
            self.config = BrainConfig()
            self.config.neuron_count = neuron_count
            self.config.region_count = region_count
            self.config.connection_probability = connection_probability
            self.config.random_seed = self.random_seed
            self.config.brain_type = self.brain_type
        
        # Initialize results storage
        self.results = SimulationResults()
        self.results.total_neurons = neuron_count
        
        # Simulation state
        self.current_step = 0
        self.current_time = 0.0
        self.is_initialized = False
        self.is_running = False
        
        # Performance tracking
        self.start_time = 0.0
        self.end_time = 0.0
        self.step_times = []
        
        # Data storage
        self.spike_recordings = []
        self.weight_recordings = []
        self.behavior_recordings = []
    
    def initialize(self) -> bool:
        """
        Initialize the neural network brain.
        
        Returns:
            bool: True if initialization succeeded, False otherwise
        """
        try:
            # Set random seed
            np.random.seed(self.random_seed)
            
            # Initialize components based on brain type
            if self.brain_type == BrainType.STANDARD:
                self._initialize_standard()
            elif self.brain_type == BrainType.SCALE_FREE:
                self._initialize_scale_free()
            elif self.brain_type == BrainType.SMALL_WORLD:
                self._initialize_small_world()
            elif self.brain_type == BrainType.MODULAR:
                self._initialize_modular()
            
            self.is_initialized = True
            self.start_time = time.time()
            
            print(f"Brain initialized successfully")
            print(f"  Neuron count: {self.neuron_count}")
            print(f"  Region count: {self.region_count}")
            print(f"  Connection probability: {self.connection_probability}")
            print(f"  Brain type: {self.brain_type.value}")
            
            return True
            
        except Exception as e:
            print(f"Error initializing brain: {e}")
            return False
    
    def _initialize_standard(self):
        """Initialize standard neural network architecture."""
        # Simple random connectivity
        self._create_neural_network(
            self.neuron_count,
            self.region_count,
            self.connection_probability
        )
    
    def _initialize_scale_free(self):
        """Initialize scale-free neural network architecture."""
        # Scale-free connectivity (power-law degree distribution)
        self._create_scale_free_network(
            self.neuron_count,
            self.region_count,
            self.connection_probability
        )
    
    def _initialize_small_world(self):
        """Initialize small-world neural network architecture."""
        # Small-world connectivity (high clustering, short path length)
        self._create_small_world_network(
            self.neuron_count,
            self.region_count,
            self.connection_probability
        )
    
    def _initialize_modular(self):
        """Initialize modular neural network architecture."""
        # Modular connectivity (distinct subnetworks)
        self._create_modular_network(
            self.neuron_count,
            self.region_count,
            self.connection_probability
        )
    
    def _create_neural_network(self, neuron_count: int, region_count: int, conn_prob: float):
        """Create standard neural network with random connectivity."""
        # This is a simplified implementation
        # In a real implementation, this would create actual neuron and synapse objects
        
        self.results.total_synapses = int(neuron_count * neuron_count * conn_prob)
        
        # Simulate some initial activity
        self._simulate_initial_activity()
    
    def _create_scale_free_network(self, neuron_count: int, region_count: int, conn_prob: float):
        """Create scale-free neural network with power-law degree distribution."""
        # Simplified scale-free implementation
        # Would use preferential attachment algorithm in real implementation
        
        self.results.total_synapses = int(neuron_count * neuron_count * conn_prob)
        self._simulate_initial_activity()
    
    def _create_small_world_network(self, neuron_count: int, region_count: int, conn_prob: float):
        """Create small-world neural network with high clustering."""
        # Simplified small-world implementation
        # Would use Watts-Strogatz algorithm in real implementation
        
        self.results.total_synapses = int(neuron_count * neuron_count * conn_prob)
        self._simulate_initial_activity()
    
    def _create_modular_network(self, neuron_count: int, region_count: int, conn_prob: float):
        """Create modular neural network with distinct regions."""
        # Simplified modular implementation
        # Would create distinct subnetworks with different connectivity patterns
        
        self.results.total_synapses = int(neuron_count * neuron_count * conn_prob)
        self._simulate_initial_activity()
    
    def _simulate_initial_activity(self):
        """Simulate initial neural activity for testing."""
        # Generate initial spike activity
        np.random.seed(self.random_seed)
        
        # Simulate some initial spikes
        initial_spikes = np.random.poisson(self.config.neuron_count * 0.1)
        self.results.total_spikes = initial_spikes
        
        # Record initial spike times
        for i in range(initial_spikes):
            spike_time = np.random.uniform(0, self.config.duration)
            self.results.spike_times.append(spike_time)
        
        # Calculate average firing rate
        self.results.average_firing_rate = initial_spikes / self.config.duration
    
    def run(self, steps: int, output_file: Optional[str] = None) -> SimulationResults:
        """
        Run the simulation for a specified number of steps.
        
        Args:
            steps: Number of simulation steps to run
            output_file: Optional file to save results
        
        Returns:
            SimulationResults object containing simulation results
        """
        if not self.is_initialized:
            print("Error: Brain not initialized. Call initialize() first.")
            return self.results
        
        self.is_running = True
        self.end_time = time.time()
        
        print(f"Running simulation for {steps} steps...")
        
        for step in range(steps):
            self.current_step = step
            self.current_time = step * self.config.timestep
            
            # Record step start time
            step_start = time.time()
            
            # Run simulation step
            self._simulation_step(step)
            
            # Record step time
            step_time = time.time() - step_start
            self.step_times.append(step_time)
            
            # Record data periodically
            if self.config.output_frequency == "every_step" or step % 100 == 0:
                self._record_step_data(step)
            
            # Display progress
            if step % 100 == 0:
                print(f"  Step {step}/{steps}: "
                      f"Spikes: {self.results.total_spikes}, "
                      f"Time: {self.current_time:.3f}s")
        
        self.is_running = False
        
        # Calculate final statistics
        self._calculate_final_statistics()
        
        # Save results if requested
        if output_file:
            self.results.save_to_file(output_file)
            print(f"Results saved to: {output_file}")
        
        print(f"Simulation completed!")
        print(f"  Total spikes: {self.results.total_spikes}")
        print(f"  Average firing rate: {self.results.average_firing_rate:.2f} Hz")
        print(f"  Simulation time: {self.current_time:.3f}s")
        
        return self.results
    
    def _simulation_step(self, step: int):
        """Run a single simulation step."""
        # Simulate neural dynamics
        # This is a simplified implementation
        
        # Generate random spikes based on neuron activity
        np.random.seed(self.random_seed + step)
        
        # Add spikes based on neural dynamics
        new_spikes = np.random.poisson(self.config.neuron_count * 0.05)
        self.results.total_spikes += new_spikes
        
        # Record spike times
        for i in range(new_spikes):
            spike_time = self.current_time + np.random.uniform(-0.001, 0.001)
            self.results.spike_times.append(spike_time)
        
        # Simulate weight changes if plasticity is enabled
        if self.config.plasticity_enabled:
            self._apply_plasticity(step)
        
        # Record step data
        step_data = {
            'step': step,
            'time': self.current_time,
            'spikes': new_spikes,
            'total_spikes': self.results.total_spikes,
            'neuron_count': self.neuron_count,
            'synapse_count': self.results.total_synapses,
            'firing_rate': new_spikes / (self.config.timestep * 100)
        }
        self.results.step_data.append(step_data)
    
    def _apply_plasticity(self, step: int):
        """Apply synaptic plasticity rules."""
        # Simulate plasticity events
        if np.random.random() < self.config.plasticity_threshold:
            # Hebbian learning
            if np.random.random() < 0.5:
                self.results.weight_change_magnitude += self.config.hebbian_learning_rate
            
            # STDP learning
            elif np.random.random() < 0.5:
                self.results.weight_change_magnitude += self.config.stdp_learning_rate
            
            # Record plasticity event
            plasticity_event = {
                'step': step,
                'time': self.current_time,
                'type': 'hebbian' if np.random.random() < 0.5 else 'stdp',
                'magnitude': self.config.hebbian_learning_rate if np.random.random() < 0.5 else self.config.stdp_learning_rate
            }
            self.results.plasticity_events.append(plasticity_event)
    
    def _record_step_data(self, step: int):
        """Record step data for analysis."""
        # Record current neural state
        neuron_data = {
            'step': step,
            'time': self.current_time,
            'neuron_count': self.neuron_count,
            'total_spikes': self.results.total_spikes,
            'firing_rate': self.results.average_firing_rate,
            'plasticity_active': self.config.plasticity_enabled
        }
        self.results.neuron_data.append(neuron_data)
    
    def _calculate_final_statistics(self):
        """Calculate final simulation statistics."""
        if self.step_times:
            self.results.simulation_time = sum(self.step_times)
        
        # Calculate memory footprint estimate
        self.results.memory_footprint = (
            self.neuron_count * 64 +  # Neurons
            self.results.total_synapses * 32 +  # Synapses
            len(self.results.step_data) * 100  # Step data
        )
    
    def step(self, step: int, time: float):
        """
        Run a single simulation step.
        
        Args:
            step: Simulation step number
            time: Current simulation time
        """
        self.current_step = step
        self.current_time = time
        self._simulation_step(step)
    
    def analyze(self) -> Dict[str, Any]:
        """
        Analyze simulation results.
        
        Returns:
            Dictionary containing analysis results
        """
        analysis = {
            'total_spikes': self.results.total_spikes,
            'total_neurons': self.results.total_neurons,
            'total_synapses': self.results.total_synapses,
            'average_firing_rate': self.results.average_firing_rate,
            'weight_change_magnitude': self.results.weight_change_magnitude,
            'simulation_duration': self.results.simulation_time,
            'memory_footprint': self.results.memory_footprint,
            'spike_count_analysis': self._analyze_spike_count(),
            'firing_rate_analysis': self._analyze_firing_rate(),
            'plasticity_analysis': self._analyze_plasticity()
        }
        
        return analysis
    
    def _analyze_spike_count(self) -> Dict[str, Any]:
        """Analyze spike count statistics."""
        if not self.results.spike_times:
            return {'mean': 0.0, 'std': 0.0, 'min': 0.0, 'max': 0.0}
        
        spike_counts = np.array(self.results.spike_times)
        
        return {
            'mean': float(np.mean(spike_counts)),
            'std': float(np.std(spike_counts)),
            'min': float(np.min(spike_counts)),
            'max': float(np.max(spike_counts)),
            'count': len(spike_counts)
        }
    
    def _analyze_firing_rate(self) -> Dict[str, Any]:
        """Analyze firing rate statistics."""
        if not self.results.step_data:
            return {'mean': 0.0, 'std': 0.0}
        
        firing_rates = [data['firing_rate'] for data in self.results.step_data]
        
        return {
            'mean': float(np.mean(firing_rates)),
            'std': float(np.std(firing_rates)),
            'min': float(np.min(firing_rates)),
            'max': float(np.max(firing_rates))
        }
    
    def _analyze_plasticity(self) -> Dict[str, Any]:
        """Analyze plasticity events."""
        if not self.results.plasticity_events:
            return {'count': 0, 'total_magnitude': 0.0, 'event_types': {}}
        
        event_types = {}
        total_magnitude = 0.0
        
        for event in self.results.plasticity_events:
            event_type = event['type']
            magnitude = event['magnitude']
            
            if event_type not in event_types:
                event_types[event_type] = 0
            event_types[event_type] += 1
            total_magnitude += magnitude
        
        return {
            'count': len(self.results.plasticity_events),
            'total_magnitude': total_magnitude,
            'event_types': event_types
        }
    
    def run_with_monitoring(self, steps: int, output_file: Optional[str] = None) -> SimulationResults:
        """
        Run simulation with real-time monitoring.
        
        Args:
            steps: Number of simulation steps
            output_file: Optional file to save results
        
        Returns:
            SimulationResults object
        """
        print("Starting simulation with monitoring...")
        
        # Initialize monitoring systems
        start_time = time.time()
        
        # Run simulation
        results = self.run(steps, output_file)
        
        # Calculate monitoring statistics
        end_time = time.time()
        total_time = end_time - start_time
        
        # Add monitoring data
        monitoring_data = {
            'start_time': start_time,
            'end_time': end_time,
            'total_time': total_time,
            'steps_per_second': steps / total_time,
            'memory_usage': self._measure_memory_usage(),
            'cpu_usage': self._measure_cpu_usage()
        }
        
        return results
    
    def _measure_memory_usage(self) -> float:
        """Measure memory usage."""
        # Simplified memory measurement
        # In real implementation, would use actual memory measurement tools
        return len(self.results.step_data) * 0.1
    
    def _measure_cpu_usage(self) -> float:
        """Measure CPU usage."""
        # Simplified CPU measurement
        # In real implementation, would use actual CPU measurement tools
        return 50.0  # Percentage
    
    # Static factory methods
    @staticmethod
    def from_config(config: Dict[str, Any]) -> 'Brain':n        """
        Create a brain from configuration dictionary.
        
        Args:
            config: Configuration dictionary
        
        Returns:
            Brain instance
        """
        brain_config = BrainConfig.from_dict(config)
        return Brain(
            neuron_count=brain_config.neuron_count,
            region_count=brain_config.region_count,
            connection_probability=brain_config.connection_probability,
            random_seed=brain_config.random_seed,
            brain_type=brain_config.brain_type,
            config=brain_config
        )
    
    @staticmethod
    def from_file(filename: str) -> 'Brain':n        """
        Create a brain from configuration file.
        
        Args:
            filename: Path to configuration file
        
        Returns:
            Brain instance
        """
        brain_config = BrainConfig.from_file(filename)
        return Brain(
            neuron_count=brain_config.neuron_count,
            region_count=brain_config.region_count,
            connection_probability=brain_config.connection_probability,
            random_seed=brain_config.random_seed,
            brain_type=brain_config.brain_type,
            config=brain_config
        )
    
    @staticmethod
    def from_results(results: Dict[str, Any]) -> 'Brain':n        """
        Create a brain from simulation results.
        
        Args:
            results: Simulation results dictionary
        
        Returns:
            Brain instance
        """
        # Create minimal brain from results
        brain = Brain(neuron_count=results.get('total_neurons', 100))
        
        # Load results
        brain.results = SimulationResults.from_dict(results)
        
        return brain
    
    @staticmethod
    def from_training_data(X: np.ndarray, y: np.ndarray) -> 'Brain':n        """
        Create a brain from training data.
        
        Args:
            X: Input data
            y: Target labels
        
        Returns:
            Brain instance
        """
        # Create configuration based on data dimensions
        config = BrainConfig()
        config.neuron_count = X.shape[1]  # Input features
        config.random_seed = 42
        
        # Create brain
        brain = Brain(config=config)
        brain.initialize()
        
        return brain
    
    @staticmethod
    def from_template(template: Dict[str, Any]) -> 'Brain':n        """
        Create a brain from template.
        
        Args:
            template: Template dictionary
        
        Returns:
            Brain instance
        """
        return Brain.from_config(template)
    
    # Analysis methods
    def compare(self, brains: List['Brain'], metrics: List[str]) -> Dict[str, Any]:
        """
        Compare multiple brains.
        
        Args:
            brains: List of Brain instances to compare
            metrics: List of metrics to compare
        
        Returns:
            Dictionary containing comparison results
        """
        comparison = {}
        
        for metric in metrics:
            if metric == 'firing_rate':
                comparison[metric] = [brain.results.average_firing_rate for brain in brains]
            elif metric == 'weight_change':
                comparison[metric] = [brain.results.weight_change_magnitude for brain in brains]
            elif metric == 'spike_count':
                comparison[metric] = [brain.results.total_spikes for brain in brains]
            elif metric == 'memory_footprint':
                comparison[metric] = [brain.results.memory_footprint for brain in brains]
            else:
                comparison[metric] = [0.0 for brain in brains]
        
        return comparison
    
    def generate_report(self, report_type: str = "comprehensive") -> str:
        """
        Generate a report for the brain simulation.
        
        Args:
            report_type: Type of report ("comprehensive", "summary", "detailed")
        
        Returns:
            Report as string
        """
        analysis = self.analyze()
        
        if report_type == "comprehensive":
            return self._generate_comprehensive_report(analysis)
        elif report_type == "summary":
            return self._generate_summary_report(analysis)
        elif report_type == "detailed":
            return self._generate_detailed_report(analysis)
        else:
            raise ValueError(f"Unknown report type: {report_type}")
    
    def _generate_comprehensive_report(self, analysis: Dict[str, Any]) -> str:
        """Generate comprehensive report."""
        report = f"""# NLM Simulation Report

## Simulation Summary
- Total Neurons: {self.neuron_count}
- Total Synapses: {self.results.total_synapses}
- Total Spikes: {analysis['total_spikes']}
- Average Firing Rate: {analysis['average_firing_rate']:.2f} Hz
- Weight Change Magnitude: {analysis['weight_change_magnitude']:.4f}
- Simulation Duration: {analysis['simulation_duration']:.2f} seconds

## Analysis Results

### Spike Count Analysis
- Mean: {analysis['spike_count_analysis']['mean']:.2f}
- Standard Deviation: {analysis['spike_count_analysis']['std']:.2f}
- Minimum: {analysis['spike_count_analysis']['min']:.2f}
- Maximum: {analysis['spike_count_analysis']['max']:.2f}

### Firing Rate Analysis
- Mean: {analysis['firing_rate_analysis']['mean']:.2f} Hz
- Standard Deviation: {analysis['firing_rate_analysis']['std']:.2f} Hz

### Plasticity Analysis
- Number of Plasticity Events: {analysis['plasticity_analysis']['count']}
- Total Plasticity Magnitude: {analysis['plasticity_analysis']['total_magnitude']:.4f}
- Event Types: {analysis['plasticity_analysis']['event_types']}

## Configuration
- Brain Type: {self.brain_type.value}
- Connection Probability: {self.connection_probability}
- Plasticity Enabled: {self.config.plasticity_enabled}
- Neuromodulation Enabled: {self.config.dopamine_enabled or self.config.curiosity_enabled}
- Development Enabled: {self.config.development_enabled}

## Performance
- Memory Footprint: {analysis['memory_footprint']} bytes
- Steps per Second: {self.neuron_count / analysis['simulation_duration']:.2f}
"""
        
        return report
    
    def _generate_summary_report(self, analysis: Dict[str, Any]) -> str:
        """Generate summary report."""
        return f"""# NLM Simulation Summary

## Key Results
- **Total Spikes**: {analysis['total_spikes']}
- **Average Firing Rate**: {analysis['average_firing_rate']:.2f} Hz
- **Weight Change Magnitude**: {analysis['weight_change_magnitude']:.4f}
- **Simulation Duration**: {analysis['simulation_duration']:.2f} seconds

## Configuration
- **Brain Type**: {self.brain_type.value}
- **Connection Probability**: {self.connection_probability}
"""
    
    def _generate_detailed_report(self, analysis: Dict[str, Any]) -> str:
        """Generate detailed report."""
        return f"""# NLM Simulation Detailed Report

## Executive Summary
This simulation ran {self.neuron_count} neurons with {self.results.total_synapses} synapses.
The neural network generated {analysis['total_spikes']} total spikes over {analysis['simulation_duration']:.2f} seconds.

## Performance Metrics
- **Firing Rate**: {analysis['average_firing_rate']:.2f} Hz
- **Plasticity Events**: {analysis['plasticity_analysis']['count']}
- **Memory Usage**: {analysis['memory_footprint']} bytes
- **Simulation Speed**: {self.neuron_count / analysis['simulation_duration']:.2f} neurons/second

## Spike Analysis
{json.dumps(analysis['spike_count_analysis'], indent=2)}

## Firing Rate Analysis
{json.dumps(analysis['firing_rate_analysis'], indent=2)}

## Plasticity Analysis
{json.dumps(analysis['plasticity_analysis'], indent=2)}

## Configuration Details
{json.dumps(self.config.to_dict(), indent=2)}
"""
    
    # Export methods
    def export_to_json(self) -> Dict[str, Any]:
        """Export results to JSON format."""
        return {
            'brain_config': self.config.to_dict(),
            'simulation_results': self.results.to_dict(),
            'analysis': self.analyze()
        }
    
    def export_to_numpy(self) -> Dict[str, Any]:
        """Export results to NumPy format."""
        return {
            'step_data': np.array([{
                'step': data['step'],
                'time': data['time'],
                'spikes': data['spikes'],
                'total_spikes': data['total_spikes'],
                'firing_rate': data['firing_rate']
            } for data in self.results.step_data]),
            'neuron_data': np.array([{
                'step': data['step'],
                'time': data['time'],
                'firing_rate': data['firing_rate'],
                'plasticity_active': data['plasticity_active']
            } for data in self.results.neuron_data]),
            'spike_times': np.array(self.results.spike_times)
        }
    
    def export_to_csv(self) -> str:
        """Export results to CSV format."""
        import csv
        import io
        
        output = io.StringIO()
        writer = csv.writer(output)
        
        # Write header
        writer.writerow(['step', 'time', 'spikes', 'total_spikes', 'firing_rate'])
        
        # Write data
        for data in self.results.step_data:
            writer.writerow([
                data['step'],
                data['time'],
                data['spikes'],
                data['total_spikes'],
                data['firing_rate']
            ])
        
        return output.getvalue()
    
    # Import methods
    @staticmethod
    def from_json(data: Dict[str, Any]) -> 'Brain':
        """Import brain from JSON data."""
        brain = Brain.from_config(data['brain_config'])
        brain.results = SimulationResults.from_dict(data['simulation_results'])
        return brain
    
    @staticmethod
    def from_numpy(data: Dict[str, Any]) -> 'Brain':
        """Import brain from NumPy data."""
        brain = Brain(neuron_count=100)  # Default
        brain.results.step_data = [{
            'step': int(row['step']),
            'time': float(row['time']),
            'spikes': int(row['spikes']),
            'total_spikes': int(row['total_spikes']),
            'firing_rate': float(row['firing_rate'])
        } for row in data['step_data']]
        return brain
    
    @staticmethod
    def from_csv(csv_data: str) -> 'Brain':
        """Import brain from CSV data."""
        import csv
        import io
        
        reader = csv.reader(io.StringIO(csv_data))
        header = next(reader)
        
        brain = Brain(neuron_count=100)
        
        for row in reader:
            step_data = {
                'step': int(row[0]),
                'time': float(row[1]),
                'spikes': int(row[2]),
                'total_spikes': int(row[3]),
                'firing_rate': float(row[4])
            }
            brain.results.step_data.append(step_data)
        
        return brain
    
    # Visualization methods (delegated to visualization module)
    def visualize_neural_network(self, filename: str):
        """Generate neural network visualization."""
        from .visualization import visualize_neural_network
        return visualize_neural_network(self, filename)
    
    def visualize_spike_activity(self, filename: str):
        """Generate spike activity visualization."""
        from .visualization import visualize_spike_activity
        return visualize_spike_activity(self, filename)
    
    def visualize_network_topology(self, filename: str):
        """Generate network topology visualization."""
        from .visualization import visualize_network_topology
        return visualize_network_topology(self, filename)
    
    def create_interactive_dashboard(self, filename: str):
        """Create interactive dashboard."""
        from .visualization import create_interactive_dashboard
        return create_interactive_dashboard(self, filename)
    
    # Debug and monitoring methods
    def enable_debug_mode(self, level: str = "detailed"):
        """Enable debug mode."""
        from .debug import DebugManager
        self.debug_manager = DebugManager()
        
        if level == "basic":
            self.debug_manager.setDebugLevel(DebugLevel.BASIC)
        elif level == "detailed":
            self.debug_manager.setDebugLevel(DebugLevel.DETAILED)
        elif level == "verbose":
            self.debug_manager.setDebugLevel(DebugLevel.VERBOSE)
        
        return self.debug_manager
    
    def collect_debug_data(self) -> Dict[str, Any]:
        """Collect debug data."""
        debug_data = {
            'simulation_step': self.current_step,
            'current_time': self.current_time,
            'total_spikes': self.results.total_spikes,
            'neuron_count': self.neuron_count,
            'synapse_count': self.results.total_synapses,
            'firing_rate': self.results.average_firing_rate,
            'memory_usage': self._measure_memory_usage(),
            'cpu_usage': self._measure_cpu_usage()
        }
        
        if hasattr(self, 'debug_manager'):
            debug_data['debug_info'] = self.debug_manager.collectDebugData()
        
        return debug_data
    
    def save_results(self, filename: str):
        """Save simulation results."""
        self.results.save_to_file(filename)
    
    def load_results(self, filename: str):
        """Load simulation results."""
        self.results = SimulationResults.load_from_file(filename)
    
    # String representation
    def __str__(self):
        return f"Brain(neuron_count={self.neuron_count}, region_count={self.region_count}, " \
               f"brain_type={self.brain_type.value}, firing_rate={self.results.average_firing_rate:.2f}Hz)"
    
    def __repr__(self):
        return self.__str__()

# Convenience function for easy brain creation

def create_brain(
    neuron_count: int = 100,
    region_count: int = 2,
    connection_probability: float = 0.15,
    random_seed: Optional[int] = None,
    brain_type: Union[str, BrainType] = BrainType.STANDARD
) -> Brain:
    """
    Create a new neural network brain.
    
    This is a convenience function for creating brains.
    """
    return Brain(
        neuron_count=neuron_count,
        region_count=region_count,
        connection_probability=connection_probability,
        random_seed=random_seed,
        brain_type=brain_type
    )

# Batch processing function

def batch_run(
    configs: List[Dict[str, Any]],
    output_base: str = "batch_results",
    parallel: bool = False,
    use_debug: bool = False
) -> List[Dict[str, Any]]:
    """
    Run batch simulations with different configurations.
    
    Args:
        configs: List of configuration dictionaries
        output_base: Base name for output files
        parallel: Enable parallel execution
        use_debug: Enable debug mode
    
    Returns:
        List of result dictionaries
    """
    results = []
    
    for i, config in enumerate(configs):
        print(f"Running simulation {i+1}/{len(configs)}: {config.get('name', 'Unnamed')}")
        
        # Create brain from configuration
        brain = Brain.from_config(config)
        brain.initialize()
        
        if use_debug:
            brain.enable_debug_mode("detailed")
        
        # Run simulation
        output_file = f"{output_base}_{i}.json"
        result = brain.run(config.get('steps', 100), output_file)
        results.append(result)
    
    return results

# Visualization functions

def visualize_neural_network(brain: Brain, filename: str):
    """Generate neural network visualization."""
    return brain.visualize_neural_network(filename)

def visualize_spike_activity(brain: Brain, filename: str):
    """Generate spike activity visualization."""
    return brain.visualize_spike_activity(filename)

def visualize_network_topology(brain: Brain, filename: str):
    """Generate network topology visualization."""
    return brain.visualize_network_topology(filename)

def create_interactive_dashboard(brain: Brain, filename: str):
    """Create interactive dashboard."""
    return brain.create_interactive_dashboard(filename)

# Comparison function

def compare_brains(brains: List[Brain], metrics: Optional[List[str]] = None) -> Dict[str, Any]:
    """
    Compare multiple brains.
    
    Args:
        brains: List of Brain instances to compare
        metrics: List of metrics to compare
    
    Returns:
        Dictionary containing comparison results
    """
    if metrics is None:
        metrics = ['firing_rate', 'weight_change', 'learning_speed', 'stability']
    
    return brains[0].compare(brains, metrics)

# Analysis function

def analyze_brain(brain: Brain) -> Dict[str, Any]:
    """
    Analyze brain simulation results.
    
    Args:
        brain: Brain instance to analyze
    
    Returns:
        Dictionary containing analysis results
    """
    return brain.analyze()

# Run function

def run_simulation(
    brain: Brain,
    steps: int,
    output_file: Optional[str] = None,
    real_time_monitoring: bool = False
) -> Dict[str, Any]:
    """
    Run a neural network simulation.
    
    Args:
        brain: Brain instance to simulate
        steps: Number of simulation steps to run
        output_file: Optional file to save results
        real_time_monitoring: Enable real-time monitoring
    
    Returns:
        Dictionary containing simulation results
    """
    if real_time_monitoring:
        return brain.run_with_monitoring(steps, output_file)
    else:
        return brain.run(steps, output_file)

# Module-level functions for backward compatibility

def load_configuration(filename: str) -> Dict[str, Any]:
    """Load configuration from file."""
    with open(filename, 'r') as f:
        return json.load(f)

def save_configuration(config: Dict[str, Any], filename: str):
    """Save configuration to file."""
    with open(filename, 'w') as f:
        json.dump(config, f, indent=2)

# Version information
__version__ = "0.1.0"

# Check for C++ backend
def check_cpp_backend():
    try:
        from .core.nlm_cpp import Brain as CPUBrain
        return True
    except ImportError:
        return False

# Print warning if C++ backend not available
if not check_cpp_backend():
    print("Warning: C++ backend not available. Using pure Python implementation.")

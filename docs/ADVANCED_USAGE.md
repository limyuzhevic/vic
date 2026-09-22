# NLM Advanced Usage Guide

## Introduction

This guide is designed for power users who want to extract maximum performance and capability from NLM. It covers advanced configuration, optimization techniques, and complex use cases that go beyond the basics.

## Advanced Configuration

### Custom Neuron Types and Dynamics

```python
import pynlm

# Custom configuration with specialized neuron types
config = pynlm.createDefaultConfig()

# Configure multiple neuron populations with different properties
config.set("brain.neuron_count", 10000)

# Sensory neurons - fast response, low threshold
sensory_config = {
    "type": "sensory",
    "v_thresh": -40.0,  # Higher threshold for noise resistance
    "tau_mem": 10.0,    # Fast membrane time constant
    "tau_ref": 1.0,     # Short refractory period
    "weight_scale": 2.0 # Stronger connections
}

# Motor neurons - reliable output
sensor_config = {
    "type": "motor", 
    "v_thresh": -50.0,
    "tau_mem": 20.0,
    "tau_ref": 3.0,
    "weight_scale": 1.0
}

# Internal neurons - complex integration
internal_config = {
    "type": "internal",
    "v_thresh": -55.0,
    "tau_mem": 30.0,
    "tau_ref": 5.0,
    "weight_scale": 0.8
}

# Apply configurations
for region_name, region_config in [
    ("sensory_cortex", sensory_config),
    ("motor_cortex", motor_config), 
    ("association_cortex", internal_config)
]:
    brain.addRegion(region_name, region_config)
```

### Advanced Plasticity Networks

```python
# Complex plasticity configuration
plasticity_config = {
    "stdp": {
        "enable": True,
        "learning_rate": 0.005,
        "tau_plus": 15.0,
        "tau_minus": 15.0,
        "a_plus": 0.1,
        "a_minus": 0.12,
        " asymmetry_rule": "asymmetric"
    },
    "hebbian": {
        "enable": True,
        "learning_rate": 0.002,
        "window_width": 50.0,  # Long-term potentiation window
        "cooperativity": 2.0   # Nonlinear Hebbian learning
    },
    "reward_modulated": {
        "enable": True,
        "baseline_rate": 0.001,
        "learning_rate": 0.01,
        "eligibility_decay": 0.9
    },
    "structural": {
        "enable": True,
        "formation_rate": 0.001,
        "pruning_threshold": 0.01,
        "max_connections_per_neuron": 100,
        "min_connections_per_neuron": 1
    }
}

# Apply advanced plasticity
brain.configurePlasticity(plasticity_config)
```

### Neuromodulation Networks

```python
# Sophisticated neuromodulation setup
neuromod_config = {
    "dopamine": {
        "enable": True,
        "baseline": 0.1,
        "learning_rate": 0.05,
        "prediction_error_modulation": True,
        "phasic_response": True,
        "tonic_background": True
    },
    "novelty": {
        "enable": True,
        "detection_threshold": 0.05,
        "decay_rate": 0.95,
        "integration_window": 100.0,
        "adaptation_factor": 0.9
    },
    "curiosity": {
        "enable": True,
        "exploration_bonus": 0.5,
        "exploitation_penalty": -0.2,
        "learning_rate": 0.03,
        "threshold_variance": 0.1
    },
    "prediction_error": {
        "enable": True,
        "sensitivity": 0.8,
        "error_signal_amplitude": 1.0,
        "temporal_decay": 0.99
    }
}

# Create neuromodulator network
neuromod_network = pynlm.createNeuromodNetwork(neuromod_config)
brain.setNeuromodNetwork(neuromod_network)
```

## Performance Optimization

### Memory Management and Pooling

```python
import pynlm

# Optimized brain with memory pools
class OptimizedBrain:
    def __init__(self, config):
        self.config = config
        self.setup_memory_pools()
        self.setup_event_queues()
        self.setup_checkpointing()
    
    def setup_memory_pools(self):
        """Configure memory pools for better performance"""
        pool_config = {
            "neuron_pool_size": 10000,
            "synapse_pool_size": 50000,
            "spike_event_pool_size": 100000,
            "weight_update_pool_size": 5000,
            "enable_preallocation": True,
            "pool_reuse_strategy": "lifo"
        }
        
        self.memory_manager = pynlm.MemoryManager(pool_config)
        self.brain = pynlm.createBrainWithMemoryManager(self.config, self.memory_manager)
    
    def setup_event_queues(self):
        """Configure event-driven processing"""
        queue_config = {
            "spike_queue_size": 100000,
            "plasticity_queue_size": 50000,
            "sensory_queue_size": 10000,
            "processing_strategy": "parallel",
            "batch_size": 100,
            "max_latency": 0.001  # 1ms max latency
        }
        
        self.event_processor = pynlm.EventProcessor(queue_config)
    
    def setup_checkpointing(self):
        """Configure efficient checkpointing"""
        checkpoint_config = {
            "checkpoint_interval": 1000,
            "compression": True,
            "incremental": True,
            "backup_locations": ["/backup/checkpoints", "/temp/checkpoints"],
            "validate_after_save": True
        }
        
        self.checkpoint_manager = pynlm.CheckpointManager(checkpoint_config)
    
    def run_optimized_simulation(self, duration):
        """Run simulation with all optimizations"""
        start_time = time.time()
        step_times = []
        
        for step in range(duration):
            step_start = time.time()
            
            # Process events in batches for efficiency
            self.event_processor.process_batch()
            
            # Run brain step
            self.brain.step(step)
            
            # Update memory pools
            self.memory_manager.update(step)
            
            # Save checkpoints if needed
            if step % self.checkpoint_manager.interval == 0:
                self.checkpoint_manager.save_checkpoint(
                    self.brain, f"checkpoint_{step}"
                )
            
            step_time = time.time() - step_start
            step_times.append(step_time)
            
            # Performance monitoring
            if step % 100 == 0:
                avg_step_time = sum(step_times[-100:]) / 100
                print(f"Step {step}: {1000/avg_step_time:.1f} Hz avg, "
                      f"max: {max(step_times)*1000:.1f}ms")
        
        total_time = time.time() - start_time
        print(f"Simulation complete: {duration} steps in {total_time:.1f}s "
              f"({duration/total_time:.1f} Hz)")

# Usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
optimized_brain = OptimizedBrain(config)
optimized_brain.run_optimized_simulation(10000)
```

### SIMD Vectorization and Parallel Processing

```python
import pynlm
import numpy as np

# SIMD-optimized neural operations
class SIMDNeuralProcessor:
    def __init__(self):
        self.vectorization_threshold = 1000  # Neurons per SIMD batch
        self.thread_pool_size = 4
        self.numba_enabled = True
    
    def process_neural_dynamics_vectorized(self, neurons, synapses):
        """Process neural dynamics using SIMD optimization"""
        if len(neurons) < self.vectorization_threshold:
            # Fall back to scalar processing for small networks
            return self.process_scalar(neurons, synapses)
        
        # Use numpy vectorization for large networks
        if self.numba_enabled:
            return self.process_numba_vectorized(neurons, synapses)
        else:
            return self.process_numpy_vectorized(neurons, synapses)
    
    def process_numpy_vectorized(self, neurons, synapses):
        """Process using numpy SIMD operations"""
        # Convert neuron states to numpy arrays
        v_membrane = np.array([n.v_membrane for n in neurons])
        v_threshold = np.array([n.v_threshold for n in neurons])
        
        # Vectorized leak computation
        dv_leak = -v_membrane / 20.0  # Membrane time constant
        
        # Vectorized spike detection
        spike_mask = v_membrane >= v_threshold
        spike_indices = np.where(spike_mask)[0]
        
        # Vectorized synaptic integration
        synaptic_input = self.compute_synaptic_input_vectorized(synapses, neurons)
        
        # Update membrane potentials
        v_membrane += dv_leak + synaptic_input
        
        # Reset spiked neurons
        v_membrane[spike_mask] = -75.0  # Reset potential
        
        # Update neuron states
        for i, neuron in enumerate(neurons):
            neuron.v_membrane = v_membrane[i]
        
        return spike_indices
    
    def compute_synaptic_input_vectorized(self, synapses, neurons):
        """Compute synaptic input using vectorized operations"""
        # Create connectivity matrix
        n_neurons = len(neurons)
        connectivity = np.zeros((n_neurons, n_neurons))
        
        # Fill connectivity matrix
        for synapse in synapses:
            pre_idx = synapse.pre_neuron.index
            post_idx = synapse.post_neuron.index
            if pre_idx < n_neurons and post_idx < n_neurons:
                connectivity[post_idx, pre_idx] = synapse.weight
        
        # Compute weighted sum using matrix multiplication
        v_membrane = np.array([n.v_membrane for n in neurons])
        synaptic_input = connectivity @ v_membrane
        
        return synaptic_input

# Integration with brain simulation
class VectorizedBrain:
    def __init__(self, config):
        self.config = config
        self.simd_processor = SIMDNeuralProcessor()
        self.setup_vectorized_computation()
    
    def setup_vectorized_computation(self):
        """Configure brain for vectorized processing"""
        # Use SIMD processor for neural dynamics
        self.neural_dynamics = self.simd_processor
        
        # Enable parallel plasticity updates
        self.plasticity_parallel = True
        self.synaptic_updates_batch_size = 100
    
    def step(self, step_number):
        """Optimized step function"""
        # Process neural dynamics with SIMD
        neurons = self.brain.getAllNeurons()
        synapses = self.brain.getAllSynapses()
        
        spike_indices = self.simd_processor.process_neural_dynamics_vectorized(
            neurons, synapses
        )
        
        # Process plasticity in parallel batches
        self.process_plasticity_parallel(spike_indices)
        
        # Process neuromodulation
        self.process_neuromodulation_vectorized()
        
        # Update development
        self.update_development_vectorized()
    
    def process_plasticity_parallel(self, spike_indices):
        """Process plasticity updates in parallel batches"""
        if not self.plasticity_parallel:
            self.process_plasticity_scalar(spike_indices)
            return
        
        # Batch spike indices for parallel processing
        batches = [spike_indices[i:i + self.synaptic_updates_batch_size] 
                  for i in range(0, len(spike_indices), self.synaptic_updates_batch_size)]
        
        # Process batches in parallel
        results = []
        for batch in batches:
            results.append(self.process_plasticity_batch(batch))
        
        # Combine results
        for result in results:
            self.apply_synaptic_updates(result)

# Usage example
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)  # Large network for SIMD benefits
vectorized_brain = VectorizedBrain(config)
vectorized_brain.brain = pynlm.createBrain(config)
```

### Checkpointing and Recovery

```python
import pynlm
import pickle
import time
import os

class AdvancedCheckpointManager:
    """Advanced checkpoint system for long-running simulations"""
    
    def __init__(self, checkpoint_dir="checkpoints", interval=1000):
        self.checkpoint_dir = checkpoint_dir
        self.interval = interval
        self.max_checkpoints = 100  # Keep only last N checkpoints
        
        # Create checkpoint directory
        os.makedirs(checkpoint_dir, exist_ok=True)
        
        # Performance optimization settings
        self.compress_checkpoints = True
        self.incremental_saving = True
        self.parallel_save = True
        self.validate_integrity = True
    
    def save_checkpoint(self, brain, agent, world, step, metadata=None):
        """Save advanced checkpoint with metadata"""
        checkpoint_data = {
            'step': step,
            'timestamp': time.time(),
            'brain_state': brain.save() if hasattr(brain, 'save') else None,
            'agent_state': agent.save() if hasattr(agent, 'save') else None,
            'world_state': world.save() if hasattr(world, 'save') else None,
            'metadata': metadata or {},
            'config': brain.getConfig() if hasattr(brain, 'getConfig') else None
        }
        
        # Add performance metrics
        checkpoint_data['performance'] = {
            'step_time': getattr(self, '_last_step_time', 0),
            'memory_usage': self.get_memory_usage(),
            'cpu_usage': self.get_cpu_usage()
        }
        
        # Save with compression if enabled
        if self.compress_checkpoints:
            filename = f"{self.checkpoint_dir}/checkpoint_{step:08d}.pkl.gz"
            with gzip.open(filename, 'wb') as f:
                pickle.dump(checkpoint_data, f, protocol=pickle.HIGHEST_PROTOCOL)
        else:
            filename = f"{self.checkpoint_dir}/checkpoint_{step:08d}.pkl"
            with open(filename, 'wb') as f:
                pickle.dump(checkpoint_data, f)
        
        # Clean up old checkpoints
        self.clean_old_checkpoints()
        
        return filename
    
    def load_checkpoint(self, brain, agent, world, checkpoint_file):
        """Load checkpoint and restore state"""
        if self.compress_checkpoints:
            with gzip.open(checkpoint_file, 'rb') as f:
                checkpoint_data = pickle.load(f)
        else:
            with open(checkpoint_file, 'rb') as f:
                checkpoint_data = pickle.load(f)
        
        # Restore state
        if checkpoint_data['brain_state'] and hasattr(brain, 'load'):
            brain.load(checkpoint_data['brain_state'])
        
        if checkpoint_data['agent_state'] and hasattr(agent, 'load'):
            agent.load(checkpoint_data['agent_state'])
        
        if checkpoint_data['world_state'] and hasattr(world, 'load'):
            world.load(checkpoint_data['world_state'])
        
        return checkpoint_data
    
    def restore_from_best_checkpoint(self, brain, agent, world, criteria='performance'):
        """Restore from best performing checkpoint"""
        checkpoints = self.list_checkpoints()
        if not checkpoints:
            return None
        
        # Find best checkpoint based on criteria
        best_checkpoint = None
        best_score = -float('inf')
        
        for checkpoint in checkpoints:
            data = self.analyze_checkpoint(checkpoint)
            score = data['metadata'].get(f'{criteria}_score', 0)
            
            if score > best_score:
                best_score = score
                best_checkpoint = checkpoint
        
        if best_checkpoint:
            return self.load_checkpoint(brain, agent, world, best_checkpoint)
        
        return None
    
    def auto_checkpoint(self, brain, agent, world, step):
        """Automatic checkpointing based on performance metrics"""
        # Save regular checkpoint
        if step % self.interval == 0:
            self.save_checkpoint(brain, agent, world, step)
        
        # Performance-based checkpointing
        perf_metrics = self.collect_performance_metrics(brain, agent, world)
        
        # Save if performance significantly changes
        if self.should_save_by_performance(perf_metrics):
            self.save_checkpoint(brain, agent, world, step, 
                               metadata={'auto_save': True, 'metrics': perf_metrics})
        
        # Save on error
        if self.should_save_on_error():
            self.save_checkpoint(brain, agent, world, step,
                               metadata={'error_save': True})
    
    def monitor_and_checkpoint(self, brain, agent, world, step):
        """Continuous monitoring and checkpointing"""
        start_time = time.time()
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Measure performance
        step_time = time.time() - start_time
        self._last_step_time = step_time
        
        # Auto checkpoint
        self.auto_checkpoint(brain, agent, world, step)
        
        # Log performance
        if step % 100 == 0:
            print(f"Step {step}: {1000/step_time:.1f} Hz, "
                  f"memory: {self.get_memory_usage():.1f}%")
        
        return step_time

# Usage in long-running simulation
checkpoint_manager = AdvancedCheckpointManager(interval=500)

# Run simulation with automatic checkpointing
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

for step in range(10000):
    checkpoint_manager.monitor_and_checkpoint(brain, agent, world, step)
    
    # Try to load from best checkpoint (for experimentation)
    if step % 1000 == 999:
        print(f"Attempting to restore from best checkpoint...")
        best_checkpoint = checkpoint_manager.restore_from_best_checkpoint(
            brain, agent, world, criteria='performance'
        )
        if best_checkpoint:
            print(f"Successfully restored from checkpoint at step {best_checkpoint['step']}")
```

## Advanced Experiment Design

### Complex Experiment Framework

```python
import pynlm
import numpy as np
from typing import Dict, List, Callable, Any

class AdvancedExperiment:
    """Advanced experiment framework for complex cognitive studies"""
    
    def __init__(self, name, config=None):
        self.name = name
        self.config = config or pynlm.createDefaultConfig()
        self.trial_data = []
        self.conditions = {}
        self.block_design = None
        self.randomization = None
        self.factors = {}
        
        # Initialize experiment components
        self.setup_experimental_design()
        self.setup_stimuli()
        self.setup_measures()
    
    def setup_experimental_design(self):
        """Configure experimental design parameters"""
        self.design_config = {
            'trial_structure': {
                'fixation_period': 1.0,      # Seconds
                'stimulus_duration': 2.0,    # Seconds
                'interstimulus_interval': 0.5,
                'response_window': 3.0,
                'delay_period': 1.0
            },
            'blocking': {
                'blocks_per_condition': 5,
                'trials_per_block': 20,
                'block_randomization': 'full'
            },
            'counterbalancing': {
                'order_effects': True,
                'practice_blocks': 2,
                'washout_period': 3
            }
        }
    
    def setup_stimuli(self):
        """Configure stimulus presentation"""
        self.stimuli = {
            'visual': {
                'patterns': self.generate_pattern_set(),
                'colors': self.generate_color_set(),
                'sizes': self.generate_size_set()
            },
            'auditory': {
                'frequencies': self.generate_frequency_set(),
                'durations': self.generate_duration_set()
            },
            'tactile': {
                'intensities': self.generate_intensity_set(),
                'vibration_patterns': self.generate_vibration_set()
            }
        }
    
    def setup_measures(self):
        """Configure experimental measures and metrics"""
        self.measures = {
            'behavioral': {
                'accuracy': self.measure_accuracy,
                'reaction_time': self.measure_reaction_time,
                'response_consistency': self.measure_consistency,
                'choice_patterns': self.measure_choice_patterns
            },
            'neural': {
                'population_dynamics': self.measure_population_dynamics,
                'spike_timing': self.measure_spike_timing,
                'synchrony_measures': self.measure_synchrony,
                'information_routing': self.measure_information_routing
            },
            'physiological': {
                'curiosity_levels': self.measure_curiosity,
                'novelty_responses': self.measure_novelty,
                'prediction_errors': self.measure_prediction_errors
            }
        }
    
    def run_experiment(self, num_trials):
        """Run complete experiment with advanced controls"""
        print(f"Running experiment: {self.name}")
        print(f"Total trials: {num_trials}")
        
        # Initialize experimental state
        self.initialize_experiment_state()
        
        # Create experimental blocks
        blocks = self.create_experimental_blocks(num_trials)
        
        # Run blocks
        for block in blocks:
            print(f"Running block {block['id']}: {block['condition']}")
            block_results = self.run_block(block)
            self.trial_data.extend(block_results)
        
        # Analyze results
        analysis_results = self.analyze_experiment_data()
        
        return {
            'name': self.name,
            'trial_data': self.trial_data,
            'analysis': analysis_results,
            'blocks_completed': len(blocks)
        }
    
    def create_experimental_blocks(self, num_trials):
        """Create randomized experimental blocks"""
        # Generate condition permutations
        conditions = self.generate_condition_combinations()
        
        # Create blocks with counterbalancing
        blocks = []
        trials_per_block = self.design_config['blocking']['trials_per_block']
        num_blocks = num_trials // trials_per_block
        
        for block_id in range(num_blocks):
            block_conditions = []
            
            # Select conditions for this block
            available_conditions = conditions.copy()
            for _ in range(trials_per_block):
                if available_conditions:
                    condition = available_conditions.pop(np.random.randint(len(available_conditions)))
                    block_conditions.append(condition)
            
            block = {
                'id': block_id,
                'conditions': block_conditions,
                'randomization_seed': block_id * 12345 + hash(self.name),
                'practice': block_id < self.design_config['counterbalancing']['practice_blocks']
            }
            
            blocks.append(block)
        
        return blocks
    
    def run_block(self, block):
        """Run a single experimental block"""
        block_results = []
        
        for trial_num, condition in enumerate(block['conditions']):
            # Setup trial
            self.setup_trial(condition, trial_num, block)
            
            # Run trial
            trial_result = self.run_single_trial(condition)
            block_results.append(trial_result)
            
            # Cleanup
            self.cleanup_trial()
        
        return block_results
    
    def run_single_trial(self, condition):
        """Run a single trial with given condition"""
        trial_start_time = time.time()
        
        # Present stimulus
        stimulus = self.generate_stimulus(condition)
        self.present_stimulus(stimulus)
        
        # Record baseline activity
        baseline_data = self.record_baseline()
        
        # Delay period (for memory tasks)
        self.delay_period()
        
        # Test phase
        test_data = self.run_test_phase(condition)
        
        # Inter-trial interval
        self.inter_trial_interval()
        
        # Compile trial results
        trial_result = {
            'condition': condition,
            'stimulus': stimulus,
            'baseline': baseline_data,
            'test': test_data,
            'timing': {
                'trial_start': trial_start_time,
                'trial_end': time.time(),
                'duration': time.time() - trial_start_time
            },
            'metadata': {
                'block_id': self.current_block_id,
                'trial_num': self.current_trial_num
            }
        }
        
        return trial_result
    
    def generate_stimulus(self, condition):
        """Generate stimulus based on experimental condition"""
        stimulus_type = condition.get('stimulus_type', 'visual')
        
        if stimulus_type == 'visual':
            pattern = np.random.choice(self.stimuli['visual']['patterns'])
            color = np.random.choice(self.stimuli['visual']['colors'])
            size = np.random.choice(self.stimuli['visual']['sizes'])
            
            return {
                'type': 'visual',
                'pattern': pattern,
                'color': color,
                'size': size,
                'duration': self.design_config['trial_structure']['stimulus_duration']
            }
        
        elif stimulus_type == 'auditory':
            frequency = np.random.choice(self.stimuli['auditory']['frequencies'])
            duration = np.random.choice(self.stimuli['auditory']['durations'])
            
            return {
                'type': 'auditory',
                'frequency': frequency,
                'duration': duration
            }
        
        return {'type': 'null'}
    
    def run_test_phase(self, condition):
        """Run test phase with agent and brain"""
        # Create brain for this trial
        trial_config = self.modify_config_for_condition(condition, self.config)
        brain = pynlm.createBrain(trial_config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(**self.get_world_config_for_condition(condition))
        
        # Initialize agent
        agent.initialize(world)
        agent.enableAllSubsystems(condition.get('subsystems', []))
        
        # Enable/disable features based on condition
        self.configure_systems_for_condition(agent, condition)
        
        # Run trial
        trial_data = {}
        for step in range(self.design_config['trial_structure']['stimulus_duration'] * 10):
            # Update world
            world.update(0.1)
            
            # Get stimulus input
            stimulus_input = self.get_stimulus_input(condition)
            agent.processSensoryInput(stimulus_input)
            
            # Brain processes
            brain.step(step)
            
            # Agent acts
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record measures
            trial_data[step] = self.record_trial_measures(brain, agent, world, condition)
        
        return trial_data
    
    def analyze_experiment_data(self):
        """Advanced statistical analysis of experimental results"""
        analysis = {}
        
        # Behavioral analysis
        behavioral_data = self.extract_measure_data('behavioral')
        analysis['behavioral'] = self.analyze_behavioral_data(behavioral_data)
        
        # Neural analysis
        neural_data = self.extract_measure_data('neural')
        analysis['neural'] = self.analyze_neural_data(neural_data)
        
        # Physiological analysis
        physiological_data = self.extract_measure_data('physiological')
        analysis['physiological'] = self.analyze_physiological_data(physiological_data)
        
        # Multivariate analysis
        analysis['multivariate'] = self.perform_multivariate_analysis()
        
        # Time-series analysis
        analysis['time_series'] = self.analyze_time_series()
        
        return analysis
    
    def measure_accuracy(self, trial_data, condition):
        """Measure behavioral accuracy"""
        # Implementation depends on task type
        return np.mean(trial_data['correct_responses'])
    
    def measure_reaction_time(self, trial_data, condition):
        """Measure reaction time"""
        return np.mean(trial_data['response_times'])
    
    def measure_population_dynamics(self, brain, agent, world, condition):
        """Measure neural population dynamics"""
        neurons = brain.getAllNeurons()
        
        # Compute population statistics
        firing_rates = [n.getFiringRate() for n in neurons]
        synchrony = self.compute_population_synchrony(neurons)
        information_content = self.compute_information_content(neurons)
        
        return {
            'mean_firing_rate': np.mean(firing_rates),
            'firing_rate_variance': np.var(firing_rates),
            'synchrony': synchrony,
            'information_content': information_content,
            'population_entropy': self.compute_entropy(firing_rates)
        }

# Usage example
experiment = AdvancedExperiment(
    name="CognitiveTaskLearning",
    config=pynlm.createDefaultConfig()
)

# Configure experiment parameters
experiment.design_config['trial_structure']['stimulus_duration'] = 2.0
experiment.design_config['trial_structure']['fixation_period'] = 0.5
experiment.design_config['blocking']['trials_per_block'] = 30

# Run experiment
results = experiment.run_experiment(num_trials=300)

# Save results
experiment.save_results(results, "cognitive_task_results.pkl")

print(f"Experiment completed with {len(results['trial_data'])} trials")
print(f"Analysis available in results['analysis']")
```

## Debugging and Optimization

### Advanced Debugging Tools

```python
import pynlm
import numpy as np
import time
import matplotlib.pyplot as plt

class AdvancedDebugger:
    """Advanced debugging and profiling tools for NLM"""
    
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.debug_log = []
        self.performance_samples = []
        self.error_history = []
        
        # Initialize profiling
        self.setup_profiling()
        self.setup_memory_tracking()
        self.setup_event_tracing()
    
    def setup_profiling(self):
        """Setup comprehensive profiling"""
        self.profiling_config = {
            'step_profiling': True,
            'function_profiling': True,
            'memory_profiling': True,
            'system_profiling': True,
            'sampling_rate': 100,  # Hz
            'profiling_duration': 1000  # Steps
        }
        
        self.step_times = []
        self.function_times = {}
    
    def setup_memory_tracking(self):
        """Setup memory tracking"""
        self.memory_tracker = {
            'enabled': True,
            'sample_interval': 50,
            'track_objects': True,
            'track_arrays': True,
            'track_allocations': True
        }
        
        self.memory_samples = []
    
    def setup_event_tracing(self):
        """Setup event tracing for debugging"""
        self.event_tracer = {
            'enabled': True,
            'trace_types': ['spikes', 'plasticity', 'neuromodulation', 'development'],
            'event_buffer_size': 10000,
            'event_history': []
        }
    
    def profile_step(self, step_number, step_func):
        """Profile a single simulation step"""
        step_start = time.time()
        step_profile = {
            'step': step_number,
            'timestamp': step_start,
            'start_time': step_start,
            'end_time': None,
            'duration': None,
            'success': True,
            'errors': [],
            'metrics': {},
            'memory_usage': {},
            'function_times': {}
        }
        
        try:
            # Run the step
            result = step_func(step_number)
            
            # Measure completion
            step_end = time.time()
            step_profile['end_time'] = step_end
            step_profile['duration'] = step_end - step_start
            
            # Record performance
            self.record_step_performance(step_profile)
            
            # Record memory usage
            self.record_memory_usage(step_profile)
            
            # Record events
            self.record_events(step_profile)
            
        except Exception as e:
            step_profile['success'] = False
            step_profile['errors'].append(str(e))
            self.error_history.append({
                'step': step_number,
                'error': str(e),
                'timestamp': time.time()
            })
        
        self.debug_log.append(step_profile)
        return result
    
    def record_step_performance(self, step_profile):
        """Record performance metrics for a step"""
        # Core performance metrics
        step_profile['metrics'] = {
            'neurons_firing': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'development_stage': self.brain.getDevelopmentalStage().value,
            'synchrony_measures': self.compute_system_synchrony(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio()
        }
        
        # System performance
        if len(self.step_times) > 0:
            step_profile['metrics']['steps_per_second'] = 1.0 / step_profile['duration']
            step_profile['metrics']['average_step_time'] = np.mean(self.step_times)
        
        self.step_times.append(step_profile['duration'])
    
    def record_memory_usage(self, step_profile):
        """Record memory usage"""
        if not self.memory_tracker['enabled']:
            return
        
        # Get memory usage
        memory_info = self.get_memory_usage()
        
        step_profile['memory_usage'] = {
            'current_rss': memory_info['rss'],
            'current_vms': memory_info['vms'],
            'python_objects': memory_info['objects'],
            'neuron_objects': self.count_neuron_objects(),
            'synapse_objects': self.count_synapse_objects(),
            'numpy_arrays': self.count_numpy_arrays()
        }
        
        self.memory_samples.append(step_profile['memory_usage'])
    
    def record_events(self, step_profile):
        """Record event traces"""
        if not self.event_tracer['enabled']:
            return
        
        # Sample events from event history
        recent_events = self.event_tracer['event_history'][-100:]
        
        step_profile['events'] = {
            'spike_events': len([e for e in recent_events if e['type'] == 'spike']),
            'plasticity_events': len([e for e in recent_events if e['type'] == 'plasticity']),
            'neuromodulation_events': len([e for e in recent_events if e['type'] == 'neuromodulation']),
            'development_events': len([e for e in recent_events if e['type'] == 'development']),
            'error_events': len(step_profile['errors'])
        }
    
    def detect_and_handle_issues(self, step_profile):
        """Automatically detect and handle potential issues"""
        issues = []
        
        # Check for performance issues
        if step_profile['duration'] > 0.1:  # Step takes >100ms
            issues.append({
                'type': 'performance',
                'severity': 'warning',
                'message': f"Step {step_profile['step']} took {step_profile['duration']*1000:.1f}ms",
                'recommendation': 'Consider reducing network size or optimizing code'
            })
        
        # Check for memory issues
        if step_profile['memory_usage']['current_rss'] > 1024 * 1024 * 1024:  # >1GB
            issues.append({
                'type': 'memory',
                'severity': 'critical',
                'message': f"High memory usage: {step_profile['memory_usage']['current_rss']/1024/1024:.1f}MB",
                'recommendation': 'Consider increasing memory pools or reducing checkpoint frequency'
            })
        
        # Check for neural issues
        if step_profile['metrics']['neurons_firing'] > self.brain.getTotalNeuronCount() * 0.9:
            issues.append({
                'type': 'neural',
                'severity': 'warning',
                'message': f"High neural activity: {step_profile['metrics']['neurons_firing']} neurons firing",
                'recommendation': 'Check for runaway excitation or lack of inhibition'
            })
        
        # Check for learning issues
        if step_profile['metrics']['prediction_error'] > 1.0:
            issues.append({
                'type': 'learning',
                'severity': 'warning',
                'message': f"High prediction error: {step_profile['metrics']['prediction_error']}",
                'recommendation': 'Check reward signals or prediction system configuration'
            })
        
        return issues
    
    def generate_debug_report(self):
        """Generate comprehensive debug report"""
        report = {
            'summary': {
                'total_steps': len(self.debug_log),
                'successful_steps': len([d for d in self.debug_log if d['success']]),
                'failed_steps': len([d for d in self.debug_log if not d['success']]),
                'error_rate': len(self.error_history) / len(self.debug_log) if self.debug_log else 0,
                'average_step_time': np.mean(self.step_times) if self.step_times else 0,
                'min_step_time': np.min(self.step_times) if self.step_times else 0,
                'max_step_time': np.max(self.step_times) if self.step_times else 0
            },
            'performance_trends': self.analyze_performance_trends(),
            'memory_trends': self.analyze_memory_trends(),
            'error_analysis': self.analyze_errors(),
            'neural_activity_analysis': self.analyze_neural_activity(),
            'recommendations': self.generate_recommendations()
        }
        
        return report
    
    def visualize_debug_data(self, save_plots=True):
        """Create visualizations of debug data"""
        fig, axes = plt.subplots(2, 3, figsize=(18, 12))
        
        # Step times over time
        steps = [d['step'] for d in self.debug_log]
        step_times_ms = [d['duration'] * 1000 for d in self.debug_log]
        
        axes[0, 0].plot(steps, step_times_ms, alpha=0.7, linewidth=0.5)
        axes[0, 0].set_title('Step Duration Over Time')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Duration (ms)')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Neural firing rate
        firing_rates = [d['metrics']['neurons_firing'] for d in self.debug_log if d['success']]
        axes[0, 1].plot(steps[:len(firing_rates)], firing_rates, alpha=0.7, linewidth=0.5)
        axes[0, 1].set_title('Firing Neurons Over Time')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Firing Neurons')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Memory usage
        memory_usage_mb = [d['memory_usage']['current_rss']/1024/1024 for d in self.debug_log if d['success']]
        axes[0, 2].plot(steps[:len(memory_usage_mb)], memory_usage_mb, alpha=0.7, linewidth=0.5)
        axes[0, 2].set_title('Memory Usage Over Time')
        axes[0, 2].set_xlabel('Step')
        axes[0, 2].set_ylabel('Memory Usage (MB)')
        axes[0, 2].grid(True, alpha=0.3)
        
        # Curiosity levels
        curiosity_levels = [d['metrics']['curiosity_level'] for d in self.debug_log if d['success']]
        axes[1, 0].plot(steps[:len(curiosity_levels)], curiosity_levels, alpha=0.7, linewidth=0.5)
        axes[1, 0].set_title('Curiosity Level Over Time')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Curiosity Level')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Performance distribution
        axes[1, 1].hist(step_times_ms, bins=50, alpha=0.7, density=True)
        axes[1, 1].set_title('Step Duration Distribution')
        axes[1, 1].set_xlabel('Duration (ms)')
        axes[1, 1].set_ylabel('Density')
        axes[1, 1].grid(True, alpha=0.3)
        
        # Error timeline
        error_steps = [e['step'] for e in self.error_history]
        if error_steps:
            axes[1, 2].vlines(error_steps, 0, 1, colors='red', alpha=0.5, linewidth=2)
            axes[1, 2].set_title('Error Occurrences')
            axes[1, 2].set_xlabel('Step')
            axes[1, 2].set_yticks([])
        
        plt.tight_layout()
        
        if save_plots:
            plt.savefig('debug_analysis.png', dpi=150, bbox_inches='tight')
        
        return fig

# Usage example in simulation loop
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Create debugger
debugger = AdvancedDebugger(brain, agent, world)

# Run simulation with debugging
for step in range(1000):
    # Define step function for debugging
    def step_function(step_num):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step_num)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get current performance metrics
        perf_metrics = {
            'step_time': 1.0 / (time.time() - debugger.get_last_step_time()),
            'neurons_firing': brain.getFiringNeuronCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        }
        
        return perf_metrics
    
    # Profile the step
    debugger.profile_step(step, step_function)
    
    # Check for issues
    current_step = debugger.debug_log[-1]
    issues = debugger.detect_and_handle_issues(current_step)
    
    if issues:
        print(f"Step {step} issues detected:")
        for issue in issues:
            print(f"  - {issue['severity'].upper()}: {issue['message']}")
            print(f"    Recommendation: {issue['recommendation']}")

# Generate debug report
report = debugger.generate_debug_report()
print("\n=== DEBUG REPORT ===")
print(f"Total steps: {report['summary']['total_steps']}")
print(f"Success rate: {report['summary']['successful_steps']/report['summary']['total_steps']*100:.1f}%")
print(f"Average step time: {report['summary']['average_step_time']*1000:.1f}ms")
print(f"Errors: {report['summary']['failed_steps']}")

# Create visualizations
fig = debugger.visualize_debug_data(save_plots=True)
print("Debug analysis saved as 'debug_analysis.png'")
```

## Integration and Customization

### Custom Agent Implementations

```python
import pynlm
import numpy as np

class CustomAgent:
    """Custom agent implementation with extended capabilities"""
    
    def __init__(self, brain, config=None):
        self.brain = brain
        self.config = config or pynlm.createDefaultConfig()
        
        # Custom agent properties
        self.custom_properties = {
            'learning_rate': self.config.get('agent.learning_rate', 0.01),
            'exploration_rate': self.config.get('agent.exploration_rate', 0.1),
            'memory_capacity': self.config.get('agent.memory_capacity', 1000),
            'decision_strategy': self.config.get('agent.decision_strategy', 'epsilon_greedy')
        }
        
        # Custom neural populations
        self.setup_custom_neural_populations()
        
        # Custom neuromodulation
        self.setup_custom_neuromodulation()
        
        # Custom plasticity rules
        self.setup_custom_plasticity()
        
        # Initialize state
        self.reset()
    
    def setup_custom_neural_populations(self):
        """Setup custom neural populations"""
        # Create specialized neural populations
        self.populations = {
            'sensory_encoder': self.create_population(
                size=self.config.get('populations.sensory_encoder.size', 100),
                type=pynlm.NeuronType.Sensory,
                dynamics=self.config.get('populations.sensory_encoder.dynamics', 'LIF')
            ),
            'motor_decoder': self.create_population(
                size=self.config.get('populations.motor_decoder.size', 50),
                type=pynlm.NeuronType.Motor,
                dynamics=self.config.get('populations.motor_decoder.dynamics', 'LIF')
            ),
            'working_memory': self.create_population(
                size=self.config.get('populations.working_memory.size', 200),
                type=pynlm.NeuronType.Internal,
                dynamics=self.config.get('populations.working_memory.dynamics', 'LIF')
            ),
            'planning_unit': self.create_population(
                size=self.config.get('populations.planning_unit.size', 100),
                type=pynlm.NeuronType.Internal,
                dynamics=self.config.get('populations.planning_unit.dynamics', 'LIF')
            )
        }
        
        # Connect populations
        self.connect_populations()
    
    def create_population(self, size, type, dynamics):
        """Create a custom neural population"""
        return {
            'neurons': [],
            'size': size,
            'type': type,
            'dynamics': dynamics,
            'properties': self.get_population_properties(type, dynamics)
        }
    
    def get_population_properties(self, neuron_type, dynamics):
        """Get properties for neural population"""
        base_properties = {
            'v_thresh': -50.0,
            'v_reset': -65.0,
            'tau_mem': 20.0,
            'tau_ref': 2.0,
            'weight_scale': 1.0,
            'connection_probability': 0.1
        }
        
        # Customize based on neuron type
        if neuron_type == pynlm.NeuronType.Sensory:
            base_properties.update({
                'v_thresh': -40.0,
                'tau_mem': 10.0,
                'connection_probability': 0.05
            })
        
        elif neuron_type == pynlm.NeuronType.Motor:
            base_properties.update({
                'v_thresh': -55.0,
                'tau_ref': 3.0,
                'connection_probability': 0.2
            })
        
        elif neuron_type == pynlm.NeuronType.Internal:
            base_properties.update({
                'v_thresh': -52.0,
                'tau_mem': 30.0,
                'connection_probability': 0.15
            })
        
        return base_properties
    
    def connect_populations(self):
        """Connect neural populations with custom topology"""
        for pop_name, population in self.populations.items():
            for target_pop_name, target_population in self.populations.items():
                if pop_name != target_pop_name:
                    # Custom connection probability
                    connection_prob = self.get_connection_probability(pop_name, target_pop_name)
                    
                    # Create connections
                    for i in range(population['size']):
                        if np.random.random() < connection_prob:
                            # Create synapse
                            synapse = self.create_custom_synapse(population, i, target_population)
                            population['neurons'][i]['synapses'].append(synapse)
    
    def create_custom_synapse(self, pre_population, pre_idx, post_population):
        """Create a custom synapse with special properties"""
        synapse = {
            'weight': np.random.normal(1.0, 0.2),
            'delay': np.random.uniform(0.0, 5.0),  # Synaptic delay
            'type': self.get_synapse_type(pre_population, post_population),
            'plasticity': self.get_plasticity_type(pre_population, post_population),
            'modulation': self.get_modulation_type(pre_population, post_population),
            'pre_neuron': pre_population['neurons'][pre_idx],
            'post_neuron': None,  # Will be set during connection
            'last_update': 0.0,
            'eligibility_trace': 0.0
        }
        
        return synapse
    
    def setup_custom_neuromodulation(self):
        """Setup custom neuromodulation"""
        self.neuromodulation = {
            'dopamine': {
                'signal': 0.0,
                'learning_modulation': True,
                'phasic_response': True,
                'tonic_baseline': self.config.get('neuromod.dopamine.baseline', 0.1)
            },
            'novelty': {
                'signal': 0.0,
                'detection_threshold': self.config.get('neuromod.novelty.threshold', 0.05),
                'decay_rate': self.config.get('neuromod.novelty.decay', 0.95),
                'adaptation': True
            },
            'curiosity': {
                'signal': 0.0,
                'exploration_bonus': self.config.get('neuromod.curiosity.exploration', 0.5),
                'exploitation_cost': self.config.get('neuromod.curiosity.exploitation', -0.2),
                'learning_rate': self.config.get('neuromod.curiosity.learning', 0.03)
            }
        }
    
    def setup_custom_plasticity(self):
        """Setup custom plasticity rules"""
        self.plasticity = {
            'stdp': self.STDPRule(),
            'hebbian': self.HebbianRule(),
            'reward_modulated': self.RewardModulatedRule(),
            'structural': self.StructuralPlasticityRule()
        }
    
    def process_custom_step(self, step_number, world_state):
        """Process custom step with agent's logic"""
        # Custom perception processing
        perception = self.process_custom_perception(world_state)
        
        # Custom neural processing
        neural_output = self.process_custom_neural(perception, step_number)
        
        # Custom decision making
        action = self.make_custom_decision(neural_output, step_number)
        
        # Custom action execution
        self.execute_custom_action(action, world_state, step_number)
        
        # Custom learning
        self.apply_custom_learning(perception, action, step_number)
        
        # Custom development
        self.update_custom_development(step_number)
        
        return action
    
    def process_custom_perception(self, world_state):
        """Custom perception processing"""
        # Extract features from world state
        features = {
            'visual': self.extract_visual_features(world_state),
            'proprioceptive': self.extract_proprioceptive_features(world_state),
            'internal': self.extract_internal_features(world_state),
            'contextual': self.extract_contextual_features(world_state)
        }
        
        # Custom feature extraction and transformation
        processed_features = self.transform_features(features)
        
        return processed_features
    
    def extract_visual_features(self, world_state):
        """Extract visual features from world state"""
        percept = world_state.getSensoryPercept()
        
        if percept.hasVisual():
            visual_data = percept.getVisual()
            
            # Extract custom features
            features = {
                'color_histogram': self.compute_color_histogram(visual_data),
                'edge_orientation': self.compute_edge_orientation(visual_data),
                'motion_vectors': self.compute_motion_vectors(visual_data),
                'texture_patterns': self.compute_texture_patterns(visual_data),
                'depth_map': self.compute_depth_map(visual_data)
            }
            
            return features
        
        return {'visual': np.zeros((8, 8, 3))}
    
    def extract_proprioceptive_features(self, world_state):
        """Extract proprioceptive features"""
        agent_body = world_state.getAgentBody()
        position = agent_body.getPosition()
        orientation = agent_body.getOrientation()
        velocity = agent_body.getVelocity()
        
        return {
            'position': [position.x, position.y, position.z],
            'orientation': [orientation.pitch, orientation.yaw, orientation.roll],
            'velocity': [velocity.vx, velocity.vy, velocity.vz],
            'acceleration': [velocity.vx, velocity.vy, velocity.vz],  # Simplified
            'energy': agent_body.getEnergy(),
            'health': agent_body.getHealth()
        }
    
    def extract_internal_features(self, world_state):
        """Extract internal state features"""
        # Extract from agent's internal state
        features = {
            'hunger': self.agent.getHungerLevel(),
            'thirst': self.agent.getThirstLevel(),
            'fatigue': self.agent.getFatigueLevel(),
            'stress': self.agent.getStressLevel(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'reward_history': self.get_reward_history()
        }
        
        return features
    
    def extract_contextual_features(self, world_state):
        """Extract contextual features"""
        # Extract environmental context
        return {
            'time_of_day': world_state.getTimeOfDay(),
            'season': world_state.getSeason(),
            'weather': world_state.getWeather(),
            'population_density': world_state.getPopulationDensity(),
            'resource_availability': world_state.getResourceAvailability()
        }
    
    def transform_features(self, features):
        """Transform features for neural processing"""
        transformed = {}
        
        for feature_type, feature_data in features.items():
            if isinstance(feature_data, dict):
                # Apply custom transformations
                transformed[feature_type] = self.apply_feature_transformations(
                    feature_type, feature_data
                )
            else:
                transformed[feature_type] = self.normalize_features(feature_data)
        
        # Fuse features for neural input
        fused_features = self.fuse_features(transformed)
        
        return fused_features
    
    def apply_feature_transformations(self, feature_type, feature_data):
        """Apply custom transformations to feature data"""
        if feature_type == 'visual':
            # Apply color space transformations
            if 'color_histogram' in feature_data:
                # Convert to opponent color space
                opponent = self.convert_to_opponent_space(feature_data['color_histogram'])
                feature_data['opponent_space'] = opponent
            
            # Apply edge detection
            if 'edge_orientation' in feature_data:
                edges = self.detect_edges(feature_data['edge_orientation'])
                feature_data['edge_density'] = np.mean(edges)
        
        elif feature_type == 'proprioceptive':
            # Apply spatial normalization
            if 'position' in feature_data:
                normalized_pos = self.normalize_position(feature_data['position'])
                feature_data['normalized_position'] = normalized_pos
            
            # Apply velocity scaling
            if 'velocity' in feature_data:
                scaled_velocity = self.scale_velocity(feature_data['velocity'])
                feature_data['scaled_velocity'] = scaled_velocity
        
        return feature_data
    
    def fuse_features(self, features):
        """Fuse different feature types for neural input"""
        # Custom feature fusion logic
        fused = {}
        
        # Concatenate features
        all_features = []
        for feature_type, feature_data in features.items():
            if isinstance(feature_data, dict):
                # Flatten nested features
                flat_features = self.flatten_features(feature_data)
                all_features.append(flat_features)
            else:
                all_features.append(feature_data)
        
        # Apply feature weighting
        weighted_features = [f * self.get_feature_weight(i) for i, f in enumerate(all_features)]
        
        # Final fusion
        fused_features = np.concatenate(weighted_features)
        
        return fused_features
    
    def process_custom_neural(self, perception, step_number):
        """Process perception through custom neural network"""
        # Encode perception into neural activity
        neural_activity = self.encode_perception(perception)
        
        # Process through custom neural network
        network_output = self.custom_neural_network(neural_activity, step_number)
        
        # Decode into motor commands
        motor_commands = self.decode_motor_commands(network_output)
        
        return {
            'perception': perception,
            'neural_activity': neural_activity,
            'network_output': network_output,
            'motor_commands': motor_commands
        }
    
    def encode_perception(self, perception):
        """Encode perception into neural activity patterns"""
        # Custom encoding strategy
        encoded = {}
        
        # Visual encoding
        if 'visual' in perception:
            visual_encoded = self.encode_visual_features(perception['visual'])
            encoded['visual'] = visual_encoded
        
        # Proprioceptive encoding
        if 'proprioceptive' in perception:
            proprioceptive_encoded = self.encode_proprioceptive_features(
                perception['proprioceptive']
            )
            encoded['proprioceptive'] = proprioceptive_encoded
        
        # Internal state encoding
        if 'internal' in perception:
            internal_encoded = self.encode_internal_features(perception['internal'])
            encoded['internal'] = internal_encoded
        
        # Contextual encoding
        if 'contextual' in perception:
            contextual_encoded = self.encode_contextual_features(perception['contextual'])
            encoded['contextual'] = contextual_encoded
        
        return encoded
    
    def encode_visual_features(self, visual_features):
        """Encode visual features"""
        # Convert visual features to neural patterns
        neural_patterns = {}
        
        for feature_name, feature_data in visual_features.items():
            if feature_name == 'color_histogram':
                # Encode color information
                color_patterns = self.encode_color_histogram(feature_data)
                neural_patterns['color'] = color_patterns
            
            elif feature_name == 'edge_orientation':
                # Encode edge information
                edge_patterns = self.encode_edge_orientation(feature_data)
                neural_patterns['edges'] = edge_patterns
            
            elif feature_name == 'motion_vectors':
                # Encode motion information
                motion_patterns = self.encode_motion_vectors(feature_data)
                neural_patterns['motion'] = motion_patterns
        
        return neural_patterns
    
    def make_custom_decision(self, neural_output, step_number):
        """Make custom decisions based on neural output"""
        # Apply decision strategy
        strategy = self.custom_properties['decision_strategy']
        
        if strategy == 'epsilon_greedy':
            return self.epsilon_greedy_decision(neural_output, step_number)
        
        elif strategy == 'softmax':
            return self.softmax_decision(neural_output, step_number)
        
        elif strategy == 'ucb':
            return self.ucb_decision(neural_output, step_number)
        
        elif strategy == 'thompson_sampling':
            return self.thompson_sampling_decision(neural_output, step_number)
        
        elif strategy == 'hierarchical':
            return self.hierarchical_decision(neural_output, step_number)
        
        else:
            return self.greedy_decision(neural_output, step_number)
    
    def epsilon_greedy_decision(self, neural_output, step_number):
        """Epsilon-greedy decision making"""
        # Calculate exploration rate
        exploration_rate = self.custom_properties['exploration_rate'] * (
            1.0 - step_number / 1000.0  # Decay over time
        )
        
        # Choose action
        if np.random.random() < exploration_rate:
            # Explore: choose random action
            return np.random.choice(self.get_available_actions())
        else:
            # Exploit: choose best action based on neural output
            return self.select_best_action(neural_output)
    
    def softmax_decision(self, neural_output, step_number):
        """Softmax decision making"""
        # Convert neural output to action preferences
        action_preferences = self.convert_to_action_preferences(neural_output)
        
        # Apply temperature parameter
        temperature = max(0.1, 1.0 - step_number / 1000.0)  # Decay over time
        
        # Apply softmax
        action_probabilities = self.softmax(action_preferences, temperature)
        
        # Sample action
        return np.random.choice(
            self.get_available_actions(), 
            p=action_probabilities
        )
    
    def ucb_decision(self, neural_output, step_number):
        """Upper Confidence Bound decision making"""
        # Calculate action values from neural output
        action_values = self.convert_to_action_values(neural_output)
        
        # Calculate action counts
        action_counts = self.get_action_counts()
        
        # Calculate UCB scores
        ucb_scores = {}
        for action in self.get_available_actions():
            if action_counts[action] > 0:
                ucb_score = (action_values[action] + 
                           self.custom_properties['exploration_rate'] * 
                           np.sqrt(np.log(step_number) / action_counts[action]))
                ucb_scores[action] = ucb_score
        
        # Choose action with highest UCB score
        if ucb_scores:
            return max(ucb_scores, key=ucb_scores.get)
        
        return np.random.choice(self.get_available_actions())
    
    def thompson_sampling_decision(self, neural_output, step_number):
        """Thompson sampling decision making"""
        # Sample action preferences from posterior distributions
        sampled_preferences = self.sample_posteriors(neural_output)
        
        # Choose best sampled action
        return max(sampled_preferences, key=sampled_preferences.get)
    
    def hierarchical_decision(self, neural_output, step_number):
        """Hierarchical decision making"""
        # High-level policy selection
        high_level_action = self.select_high_level_action(neural_output)
        
        # Low-level action refinement
        low_level_action = self.refine_action(high_level_action, neural_output)
        
        return low_level_action
    
    def greedy_decision(self, neural_output, step_number):
        """Greedy decision making"""
        return self.select_best_action(neural_output)
    
    def apply_custom_learning(self, perception, action, step_number):
        """Apply custom learning mechanisms"""
        # STDP learning
        self.apply_custom_stdp(perception, action, step_number)
        
        # Hebbian learning
        self.apply_custom_hebbian(perception, action, step_number)
        
        # Reward-modulated learning
        self.apply_custom_reward_modulated(perception, action, step_number)
        
        # Curiosity-driven learning
        self.apply_custom_curiosity(perception, action, step_number)
    
    def update_custom_development(self, step_number):
        """Update custom development"""
        # Check if development should occur
        if self.should_develop(step_number):
            # Apply developmental changes
            self.apply_development_changes()
            
            # Update developmental stage
            self.update_developmental_stage()
    
    # STDP Rule Implementation
    class STDPRule:
        def __init__(self):
            self.learning_rate = 0.001
            self.tau_plus = 20.0
            self.tau_minus = 20.0
            self.a_plus = 0.1
            self.a_minus = 0.12
        
        def update(self, pre_spike_time, post_spike_time, synapse):
            """Update synapse based on spike timing"""
            delta_t = post_spike_time - pre_spike_time
            
            if delta_t > 0:  # Post after pre (LTP)
                weight_change = self.a_plus * np.exp(-delta_t / self.tau_plus)
            else:  # Post before pre (LTD)
                weight_change = -self.a_minus * np.exp(delta_t / self.tau_minus)
            
            synapse.weight += self.learning_rate * weight_change
            synapse.eligibility_trace += weight_change
    
    # Hebbian Rule Implementation
    class HebbianRule:
        def __init__(self):
            self.learning_rate = 0.002
            self.window_width = 50.0
            self.cooperativity = 2.0
        
        def update(self, pre_activity, post_activity, synapse):
            """Update synapse based on correlated activity"""
            correlation = pre_activity * post_activity
            
            # Apply cooperativity
            if correlation > 0:
                weight_change = self.learning_rate * (correlation ** self.cooperativity)
            
            synapse.weight += weight_change
    
    # Reward-Modulated Rule Implementation
    class RewardModulatedRule:
        def __init__(self):
            self.baseline_rate = 0.001
            self.learning_rate = 0.01
            self.eligibility_decay = 0.9
        
        def update(self, reward, eligibility_trace, synapse):
            """Update synapse based on reward"""
            # Decay eligibility trace
            eligibility_trace *= self.eligibility_decay
            
            # Update weight based on reward
            weight_change = self.learning_rate * reward * eligibility_trace
            
            synapse.weight += weight_change
    
    # Structural Plasticity Rule Implementation
    class StructuralPlasticityRule:
        def __init__(self):
            self.formation_rate = 0.001
            self.pruning_threshold = 0.01
            self.max_connections = 100
            self.min_connections = 1
        
        def update(self, synapse, network_state):
            """Update structural connectivity"""
            # Form new connections
            if np.random.random() < self.formation_rate:
                self.form_new_connection(network_state)
            
            # Prune weak connections
            if synapse.weight < self.pruning_threshold:
                self.prune_connection(synapse)
    
    # Helper methods
    def reset(self):
        """Reset agent to initial state"""
        # Reset neural populations
        for population in self.populations.values():
            population['neurons'] = self.initialize_neurons(population)
        
        # Reset neuromodulation
        for modulator in self.neuromodulation.values():
            modulator['signal'] = 0.0
        
        # Reset learning
        for plasticity_rule in self.plasticity.values():
            if hasattr(plasticity_rule, 'reset'):
                plasticity_rule.reset()
        
        # Reset development
        self.development_stage = 0
        self.development_progress = 0.0

# Usage example with custom agent
class CustomAgentIntegration:
    """Integration example showing custom agent with NLM"""
    
    def __init__(self):
        # Create standard NLM brain
        self.brain = pynlm.createBrain(pynlm.createDefaultConfig())
        self.brain.initialize()
        
        # Create custom agent
        self.custom_agent = CustomAgent(self.brain, {
            'agent': {
                'learning_rate': 0.01,
                'exploration_rate': 0.1,
                'decision_strategy': 'softmax'
            },
            'populations': {
                'sensory_encoder': {'size': 150},
                'working_memory': {'size': 300}
            }
        })
        
        # Create standard world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20)
    
    def run_simulation(self, duration):
        """Run simulation with custom agent"""
        for step in range(duration):
            # Get world state
            world_state = self.get_world_state()
            
            # Custom agent processes and decides
            action = self.custom_agent.process_custom_step(step, world_state)
            
            # Execute action in world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update world
            self.world.update(0.1)
            
            # Record statistics
            self.record_step_statistics(step)
    
    def get_world_state(self):
        """Get current world state for custom agent"""
        return {
            'sensory_percept': self.world.getSensoryPercept(),
            'agent_body': self.world.getAgentBody(),
            'simulation_time': self.world.getSimulationTime()
        }
    
    def record_step_statistics(self, step):
        """Record simulation statistics"""
        # Custom statistics recording
        stats = {
            'step': step,
            'brain_firing_rate': self.brain.getFiringNeuronCount(),
            'agent_curiosity': self.custom_agent.agent.getCuriosityLevel(),
            'agent_novelty': self.custom_agent.agent.getNoveltyLevel(),
            'agent_development_stage': self.custom_agent.agent.getDevelopmentalStage()
        }
        
        return stats

# Usage
if __name__ == "__main__":
    # Create integrated system
    integrated_system = CustomAgentIntegration()
    
    # Run simulation
    print("Running simulation with custom agent...")
    integrated_system.run_simulation(1000)
    
    print("Simulation completed with custom agent integration!")
```

## Advanced Performance Monitoring

### Comprehensive Performance Dashboard

```python
import pynlm
import numpy as np
import time
import psutil
import matplotlib.pyplot as plt
from typing import Dict, List, Any

class PerformanceDashboard:
    """Comprehensive performance monitoring and analysis dashboard"""
    
    def __init__(self, brain, agent, world, sampling_interval=100):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.sampling_interval = sampling_interval
        
        # Initialize metrics
        self.initialize_metrics()
        
        # Start monitoring
        self.start_monitoring()
    
    def initialize_metrics(self):
        """Initialize performance metrics containers"""
        self.metrics = {
            'time': [],
            'step_duration': [],
            'cpu_usage': [],
            'memory_usage': [],
            'neural_firing': [],
            'sensory_input': [],
            'motor_output': [],
            'curiosity_level': [],
            'novelty_level': [],
            'prediction_error': [],
            'development_stage': [],
            'spike_count': [],
            'synaptic_activity': [],
            'network_synchrony': [],
            'e_i_ratio': []
        }
        
        # System metrics
        self.system_metrics = {
            'process': psutil.Process(),
            'start_time': time.time(),
            'last_sample_time': time.time()
        }
    
    def start_monitoring(self):
        """Start continuous performance monitoring"""
        self.monitoring_active = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def _monitor_loop(self):
        """Continuous monitoring loop"""
        while self.monitoring_active:
            try:
                # Record current metrics
                self.record_current_metrics()
                
                # Sleep for sampling interval
                time.sleep(self.sampling_interval / 1000.0)  # Convert to seconds
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(1.0)  # Short sleep on error
    
    def record_current_metrics(self):
        """Record current performance metrics"""
        current_time = time.time()
        
        # Time metrics
        self.metrics['time'].append(current_time)
        
        # System metrics
        self.metrics['cpu_usage'].append(self.system_metrics['process'].cpu_percent())
        self.metrics['memory_usage'].append(
            self.system_metrics['process'].memory_info().rss / 1024 / 1024  # MB
        )
        
        # Neural metrics
        if hasattr(self.brain, 'getFiringNeuronCount'):
            self.metrics['neural_firing'].append(self.brain.getFiringNeuronCount())
        
        if hasattr(self.brain, 'getTotalSpikeCount'):
            self.metrics['spike_count'].append(self.brain.getTotalSpikeCount())
        
        if hasattr(self.brain, 'getAverageFiringRate'):
            self.metrics['sensory_input'].append(self.brain.getAverageFiringRate())
        
        if hasattr(self.agent, 'getCuriosityLevel'):
            self.metrics['curiosity_level'].append(self.agent.getCuriosityLevel())
        
        if hasattr(self.agent, 'getNoveltyLevel'):
            self.metrics['novelty_level'].append(self.agent.getNoveltyLevel())
        
        if hasattr(self.agent, 'getPredictionError'):
            self.metrics['prediction_error'].append(self.agent.getPredictionError())
        
        if hasattr(self.agent, 'getDevelopmentalStage'):
            self.metrics['development_stage'].append(
                self.agent.getDevelopmentalStage().value
            )
        
        # Network metrics
        self.metrics['e_i_ratio'].append(self.compute_ei_ratio())
        self.metrics['network_synchrony'].append(self.compute_network_synchrony())
    
    def compute_ei_ratio(self):
        """Compute excitation-inhibition ratio"""
        # Implementation depends on brain structure
        return 1.0  # Placeholder
    
    def compute_network_synchrony(self):
        """Compute network synchrony measures"""
        # Implementation depends on brain structure
        return 0.0  # Placeholder
    
    def get_performance_summary(self):
        """Get comprehensive performance summary"""
        summary = {
            'simulation_time': {
                'total_steps': len(self.metrics['time']),
                'total_duration': self.metrics['time'][-1] - self.metrics['time'][0] if len(self.metrics['time']) > 1 else 0,
                'steps_per_second': len(self.metrics['time']) / (self.metrics['time'][-1] - self.metrics['time'][0]) if len(self.metrics['time']) > 1 else 0
            },
            'neural_performance': {
                'average_firing_rate': np.mean(self.metrics['neural_firing']) if self.metrics['neural_firing'] else 0,
                'max_firing_rate': np.max(self.metrics['neural_firing']) if self.metrics['neural_firing'] else 0,
                'firing_rate_variance': np.var(self.metrics['neural_firing']) if self.metrics['neural_firing'] else 0
            },
            'cognitive_performance': {
                'average_curiosity': np.mean(self.metrics['curiosity_level']) if self.metrics['curiosity_level'] else 0,
                'average_novelty': np.mean(self.metrics['novelty_level']) if self.metrics['novelty_level'] else 0,
                'average_prediction_error': np.mean(self.metrics['prediction_error']) if self.metrics['prediction_error'] else 0,
                'development_progress': self.compute_development_progress()
            },
            'system_performance': {
                'average_cpu_usage': np.mean(self.metrics['cpu_usage']) if self.metrics['cpu_usage'] else 0,
                'peak_cpu_usage': np.max(self.metrics['cpu_usage']) if self.metrics['cpu_usage'] else 0,
                'average_memory_usage': np.mean(self.metrics['memory_usage']) if self.metrics['memory_usage'] else 0,
                'peak_memory_usage': np.max(self.metrics['memory_usage']) if self.metrics['memory_usage'] else 0
            }
        }
        
        return summary
    
    def generate_performance_report(self, save_report=True):
        """Generate comprehensive performance report"""
        summary = self.get_performance_summary()
        
        report = {
            'metadata': {
                'generated_time': time.time(),
                'simulation_duration': summary['simulation_time']['total_duration'],
                'sampling_interval': self.sampling_interval,
                'brain_configuration': str(self.brain.getConfig()) if hasattr(self.brain, 'getConfig') else 'Unknown'
            },
            'summary': summary,
            'time_series_data': self.metrics,
            'performance_trends': self.analyze_performance_trends(),
            'bottleneck_analysis': self.analyze_bottlenecks(),
            'optimization_recommendations': self.generate_optimization_recommendations()
        }
        
        if save_report:
            self.save_performance_report(report)
        
        return report
    
    def analyze_performance_trends(self):
        """Analyze performance trends over time"""
        trends = {}
        
        # Neural firing rate trends
        if self.metrics['neural_firing']:
            trends['firing_rate'] = {
                'slope': self.compute_linear_trend(self.metrics['neural_firing']),
                'trend': self.detect_trend(self.metrics['neural_firing']),
                'stability': self.compute_stability(self.metrics['neural_firing'])
            }
        
        # Cognitive performance trends
        if self.metrics['curiosity_level']:
            trends['curiosity'] = {
                'slope': self.compute_linear_trend(self.metrics['curiosity_level']),
                'trend': self.detect_trend(self.metrics['curiosity_level']),
                'stability': self.compute_stability(self.metrics['curiosity_level'])
            }
        
        return trends
    
    def analyze_bottlenecks(self):
        """Identify performance bottlenecks"""
        bottlenecks = []
        
        # Step duration analysis
        if len(self.metrics['step_duration']) > 10:
            recent_durations = self.metrics['step_duration'][-10:]
            avg_duration = np.mean(recent_durations)
            max_duration = np.max(recent_durations)
            
            if max_duration > avg_duration * 2:  # High variance
                bottlenecks.append({
                    'type': 'step_variance',
                    'severity': 'high' if max_duration > 0.1 else 'medium',
                    'description': f'High step duration variance: avg={avg_duration:.3f}s, max={max_duration:.3f}s',
                    'recommendation': 'Investigate slow steps and optimize critical paths'
                })
        
        # Memory usage analysis
        if self.metrics['memory_usage']:
            recent_memory = self.metrics['memory_usage'][-10:]
            avg_memory = np.mean(recent_memory)
            
            if avg_memory > 1024:  # >1GB
                bottlenecks.append({
                    'type': 'memory_usage',
                    'severity': 'high',
                    'description': f'High memory usage: {avg_memory:.1f}MB',
                    'recommendation': 'Implement memory pooling or reduce checkpoint frequency'
                })
        
        return bottlenecks
    
    def generate_optimization_recommendations(self):
        """Generate optimization recommendations"""
        recommendations = []
        
        # Analyze for common optimization opportunities
        summary = self.get_performance_summary()
        
        # CPU optimization
        if summary['system_performance']['average_cpu_usage'] > 80:
            recommendations.append({
                'priority': 'high',
                'category': 'cpu_optimization',
                'recommendation': 'CPU usage is high. Consider reducing thread count or optimizing parallel processing',
                'expected_improvement': '10-30%'
            })
        
        # Memory optimization
        if summary['system_performance']['average_memory_usage'] > 2048:  # >2GB
            recommendations.append({
                'priority': 'medium',
                'category': 'memory_optimization',
                'recommendation': 'Implement object pooling and reduce checkpoint size',
                'expected_improvement': '15-25%'
            })
        
        # Neural optimization
        if summary['neural_performance']['firing_rate_variance'] > 1000:
            recommendations.append({
                'priority': 'low',
                'category': 'neural_optimization',
                'recommendation': 'High neural firing variance. Consider adjusting network connectivity',
                'expected_improvement': '5-15%'
            })
        
        return recommendations
    
    def save_performance_report(self, report):
        """Save performance report to file"""
        import json
        
        # Create report filename with timestamp
        timestamp = time.strftime('%Y%m%d_%H%M%S')
        filename = f'performance_report_{timestamp}.json'
        
        # Save report
        with open(filename, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        print(f"Performance report saved to {filename}")
        
        # Also save visualizations
        self.generate_visualizations(report)
    
    def generate_visualizations(self, report):
        """Generate performance visualizations"""
        fig, axes = plt.subplots(2, 3, figsize=(18, 12))
        fig.suptitle('NLM Performance Analysis Dashboard', fontsize=16)
        
        time_data = report['time_series_data']
        
        # CPU usage over time
        axes[0, 0].plot(time_data['time'], time_data['cpu_usage'], alpha=0.7)
        axes[0, 0].set_title('CPU Usage Over Time')
        axes[0, 0].set_xlabel('Time (s)')
        axes[0, 0].set_ylabel('CPU (%)')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Memory usage over time
        axes[0, 1].plot(time_data['time'], time_data['memory_usage'], alpha=0.7, color='orange')
        axes[0, 1].set_title('Memory Usage Over Time')
        axes[0, 1].set_xlabel('Time (s)')
        axes[0, 1].set_ylabel('Memory (MB)')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Neural firing over time
        if time_data['neural_firing']:
            axes[0, 2].plot(time_data['time'][:len(time_data['neural_firing'])], 
                           time_data['neural_firing'], alpha=0.7, color='green')
            axes[0, 2].set_title('Neural Firing Over Time')
            axes[0, 2].set_xlabel('Time (s)')
            axes[0, 2].set_ylabel('Firing Neurons')
            axes[0, 2].grid(True, alpha=0.3)
        
        # Cognitive performance
        if time_data['curiosity_level']:
            axes[1, 0].plot(time_data['time'][:len(time_data['curiosity_level'])], 
                           time_data['curiosity_level'], alpha=0.7, color='red')
            axes[1, 0].set_title('Curiosity Level Over Time')
            axes[1, 0].set_xlabel('Time (s)')
            axes[1, 0].set_ylabel('Curiosity Level')
            axes[1, 0].grid(True, alpha=0.3)
        
        # Performance metrics scatter
        if len(time_data['cpu_usage']) == len(time_data['memory_usage']):
            axes[1, 1].scatter(time_data['cpu_usage'], time_data['memory_usage'], 
                              alpha=0.5, s=1, c=time_data['time'][:len(time_data['cpu_usage'])], 
                              cmap='viridis')
            axes[1, 1].set_title('CPU vs Memory Usage')
            axes[1, 1].set_xlabel('CPU (%)')
            axes[1, 1].set_ylabel('Memory (MB)')
            axes[1, 1].grid(True, alpha=0.3)
        
        # Performance summary bar chart
        if report['summary']:
            summary = report['summary']
            categories = ['CPU', 'Memory', 'Firing', 'Curiosity']
            values = [
                summary['system_performance']['average_cpu_usage'],
                summary['system_performance']['average_memory_usage'] / 100,  # Normalize
                summary['neural_performance']['average_firing_rate'] / 100,  # Normalize
                summary['cognitive_performance']['average_curiosity']
            ]
            
            axes[1, 2].bar(categories, values, alpha=0.7, color=['blue', 'orange', 'green', 'red'])
            axes[1, 2].set_title('Performance Summary')
            axes[1, 2].set_ylabel('Normalized Value')
            axes[1, 2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig('performance_dashboard.png', dpi=150, bbox_inches='tight')
        plt.close()

# Usage example
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Create performance dashboard
dashboard = PerformanceDashboard(brain, agent, world, sampling_interval=50)

# Run simulation with performance monitoring
for step in range(1000):
    # Run simulation step
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Generate performance report
dashboard.stop_monitoring()
report = dashboard.generate_performance_report(save_report=True)

print("Performance analysis complete!")
print(f"Total steps: {report['summary']['simulation_time']['total_steps']}")
print(f"Average steps/sec: {report['summary']['simulation_time']['steps_per_second']:.1f}")
print(f"Recommendations: {len(report['optimization_recommendations'])}")
```

## Quick Reference: Advanced Usage

### Core Advanced Patterns

| Pattern | Description | When to Use |
|---------|-------------|-------------|
| **Vectorized Processing** | SIMD-optimized neural computation | Large networks (>1000 neurons) |
| **Memory Pooling** | Pre-allocated memory for performance | Long-running simulations |
| **Checkpointing** | State persistence and recovery | Critical simulations |
| **Event Processing** | Asynchronous event handling | Real-time applications |
| **Performance Profiling** | Detailed performance analysis | Optimization tasks |

### Advanced Configuration Options

| Component | Advanced Settings | Purpose |
|-----------|------------------|---------|
| **Brain** | Custom plasticity rules, neuromodulation networks | Enhanced learning |
| **Agent** | Custom decision strategies, exploration-exploitation | Complex behavior |
| **World** | Dynamic environments, multi-agent systems | Rich interactions |
| **Memory** | Memory pools, checkpoint compression | Performance optimization |

### Performance Optimization Techniques

1. **Memory Management**
   - Use memory pools for frequently allocated objects
   - Implement object reuse to reduce garbage collection
   - Compress checkpoint data for storage efficiency

2. **Computation Optimization**
   - Vectorize operations using SIMD instructions
   - Batch processing for parallel execution
   - Optimize neural connectivity patterns

3. **I/O Optimization**
   - Reduce checkpoint frequency for long runs
   - Use efficient serialization formats
   - Implement incremental checkpointing

4. **System Optimization**
   - Monitor and tune thread counts
   - Optimize memory allocation patterns
   - Use appropriate data structures

### Common Advanced Use Cases

#### 1. Large-Scale Brain Simulation
```python
# Configuration for large networks
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 50000)  # Large network
config.set("plasticity.structural.enable", true)
config.set("neuromod.dopamine.learning_rate", 0.01)

# Enable optimizations
brain = pynlm.createBrainWithOptimizations(config)
```

#### 2. Real-Time Applications
```python
# Configuration for real-time performance
config = pynlm.createDefaultConfig()
config.set("performance.mode", "realtime")
config.set("event_queue_size", 100000)
config.set("parallel.processing", true)

# Use event-driven architecture
brain = pynlm.createEventDrivenBrain(config)
```

#### 3. Research Experiments
```python
# Configuration for research
config = pynlm.createDefaultConfig()
config.set("experiment.mode", "controlled")
config.set("random.seed", 42)
config.set("checkpoint.interval", 1000)

# Enable comprehensive logging
brain = pynlm.createResearchBrain(config)
```

#### 4. Multi-Agent Systems
```python
# Configuration for multiple agents
config = pynlm.createDefaultConfig()
config.set("agent.max_agents", 10)
config.set("social.learning.enable", true)
config.set("shared.memory.enable", true)

# Create agent network
agent_network = pynlm.createAgentNetwork(config)
```

## Next Steps

After mastering advanced usage:

1. **Specialized Applications** - Develop domain-specific NLM configurations
2. **Integration** - Connect NLM with external systems and APIs
3. **Research** - Design novel neural architectures and learning rules
4. **Deployment** - Optimize for production environments
5. **Documentation** - Create domain-specific documentation and tutorials

Advanced NLM usage opens up powerful possibilities for creating sophisticated neural systems with emergent behaviors, complex learning capabilities, and real-time responsiveness.

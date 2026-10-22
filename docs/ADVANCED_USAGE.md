# NLM Advanced Usage Guide

## Table of Contents

1. [Introduction](#introduction)
2. [Architecture Deep Dive](#architecture-deep-dive)
3. [Advanced Simulation Techniques](#advanced-simulation-techniques)
4. [Custom Development](#custom-development)
5. [Performance Optimization](#performance-optimization)
6. [Experiment Design and Analysis](#experiment-design-and-analysis)
7. [Integration with Other Systems](#integration-with-other-systems)
8. [Troubleshooting Advanced Issues](#troubleshooting-advanced-issues)
9. [Extending the Framework](#extending-the-framework)
10. [Future Directions](#future-directions)

## Introduction

This guide covers advanced usage patterns, architectural insights, and best practices for working with the NLM neural simulation framework at a deeper level. It assumes familiarity with the basic NLM API as documented in the Python API guide.

## Architecture Deep Dive

### 1. Neural Processing Pipeline

The NLM operates as a closed-loop cognitive architecture with the following stages:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

#### Key Components:

- **Working Memory**: Persistent activity maintained across time steps
- **Episodic Memory**: Experience encoding and replay mechanisms
- **Associative Memory**: Hebbian pattern associations
- **Prediction System**: Forward models and prediction error signals
- **Neuromodulation**: Dopamine (reward), Curiosity (exploration), Novelty
- **Development System**: Plasticity modulation by age and experience
- **Cognition Systems**: Planning, concept formation, attention

### 2. Memory System Integration

#### Working Memory

Working memory maintains neural activity patterns across simulation steps. It's implemented as:

```python
# Working memory traces are automatically maintained
# through recurrent connections and persistent activity
working_memory = brain.workingMemory  # Access working memory system

# Get active traces
active_traces = working_memory.getActiveTraces()
```

#### Episodic Memory

Episodic memory stores experience episodes for later replay and consolidation:

```python
# Record experience
episodic_memory = brain.episodicMemory
episode_id = episodic_memory.recordEpisode(percept, action, reward)

# Replay episode for consolidation
episodic_memory.replayEpisode(episode_id, consolidation_rate=0.1)
```

#### Associative Memory

Hebbian associations are formed through correlated activity:

```python
# Hebbian learning is automatic during simulation
# Synaptic weights strengthen for co-activated neurons
```

### 3. Neuromodulation Systems

#### Dopamine System

Dopamine encodes reward prediction errors and modulates plasticity:

```python
# Reward modulation through dopamine
brain.dopamine.getLevel()  # Current dopamine level
brain.dopamine.getPredictionError()  # Reward prediction error
agent_brain.applyRewardModulation(reward, predicted_reward)
```

#### Curiosity System

Curiosity drives exploration of novel states:

```python
# Curiosity level affects exploration behavior
agent_brain.getCuriosityLevel()  # Current curiosity level
brain.curiosity.getLevel()  # Underlying curiosity system level
```

#### Novelty Detection

Novelty detection identifies unexpected sensory input:

```python
# Novelty-based exploration
agent_brain.getNoveltyLevel()  # Current novelty level
```

### 4. Development System

#### Developmental Stages

Brains progress through developmental stages that affect plasticity:

```python
# Developmental stages
from nlm import DevelopmentalStage

brain.getDevelopmentalStage()  # Current stage
brain.setDevelopmentalStage(DevelopmentalStage.Maturation)

# Stage-dependent plasticity
brain.developmentSystem.getPlasticityModifier()  # Age-related plasticity factor
```

#### Structural Plasticity

Structural changes include synaptogenesis and pruning:

```python
# Structural plasticity settings
agent_brain.enableStructuralPlasticity(True)

# Synaptogenesis and pruning rates are configurable
brain.config.set('synaptogenesis_rate', 0.0001)
brain.config.set('pruning_rate', 0.00001)
```

## Advanced Simulation Techniques

### 1. Multi-Modal Sensory Processing

Create complex sensory processing pipelines:

```python
def create_multimodal_processor():
    """Create a multi-modal sensory processor."""
    def process_vision(vision_data):
        # Apply vision processing (edge detection, feature extraction)
        return [max(0, v * 0.7) for v in vision_data]
    
    def process_audio(audio_data):
        # Apply audio processing (frequency analysis)
        return [v * 0.5 for v in audio_data]
    
    def process_proprioception(proprio_data):
        # Apply body position processing
        return [v * 0.8 for v in proprio_data]
    
    def process_internal(internal_data):
        # Apply internal state processing
        return [v * 0.9 for v in internal_data]
    
    return {
        'vision': process_vision,
        'audio': process_audio,
        'proprioception': process_proprioception,
        'internal': process_internal
    }
```

### 2. Advanced Reward Shaping

Design sophisticated reward functions:

```python
class AdvancedRewardShaper:
    def __init__(self):
        self.reward_history = []
        self.predicted_rewards = []
    
    def sharded_reward(self, base_reward, novelty, progress, efficiency):
        """Combine multiple reward signals."""
        # Sparse reward for novelty
        novelty_reward = novelty * 10.0
        
        # Progress reward (encourages improvement)
        progress_reward = max(0, (progress - 0.5) * 20.0)
        
        # Efficiency reward (encourages quick learning)
        efficiency_reward = efficiency * 5.0
        
        # Total reward
        total_reward = base_reward + novelty_reward + progress_reward + efficiency_reward
        
        return total_reward
    
    def calculate_predicted_reward(self, state):
        """Calculate predicted future reward."""
        # Simple forward model for reward prediction
        recent_rewards = self.reward_history[-10:]  # Last 10 rewards
        avg_reward = sum(recent_rewards) / len(recent_rewards) if recent_rewards else 0
        
        # Add exploration bonus for novel states
        novelty_bonus = 1.0 if self.is_novel(state) else 0.0
        
        return avg_reward + novelty_bonus
```

### 3. Custom Plasticity Rules

Implement custom learning rules:

```python
class CustomPlasticityRule:
    def __init__(self):
        self.learning_rates = {}
        self.stdp_params = {}
    
    def apply_custom_stdp(self, pre_spike, post_spike, synapse):
        """Apply custom STDP learning rule."""
        # Time-based learning with custom parameters
        time_diff = post_spike - pre_spike
        
        if time_diff > 0:  # Post fires after pre (LTP)
            weight_change = self.stdp_params['LTP_strength'] * \
                          np.exp(-abs(time_diff) / self.stdp_params['LTP_time_constant'])
        else:  # Post fires before pre (LTD)
            weight_change = -self.stdp_params['LTD_strength'] * \
                          np.exp(-abs(time_diff) / self.stdp_params['LTD_time_constant'])
        
        # Apply weight change with bounds
        new_weight = synapse.getWeight() + weight_change
        new_weight = max(0.0, min(1.0, new_weight))  # Clamp to [0, 1]
        
        synapse.setWeight(new_weight)
        return weight_change
    
    def apply_hebbian_learning(self, pre_neuron, post_neuron, correlation):
        """Apply Hebbian learning based on correlation."""
        # Delta rule with adaptive learning rate
        learning_rate = self.learning_rates.get('hebbian', 0.01)
        
        weight_change = learning_rate * correlation * pre_neuron.getActivity() * post_neuron.getActivity()
        
        return weight_change
```

### 4. Advanced World Modeling

Create sophisticated world models:

```python
class AdvancedWorldModel:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.objects = []
        self.rewards = {}
        self.hazards = {}
        self.resources = {}
    
    def add_dynamic_object(self, obj_id, x, y, obj_type, properties):
        """Add a dynamic object to the world."""
        obj = {
            'id': obj_id,
            'x': x,
            'y': y,
            'type': obj_type,
            'properties': properties,
            'velocity': properties.get('velocity', [0, 0]),
            'active': True
        }
        
        self.objects.append(obj)
        
        # Set up reward/hazard system
        if obj_type == 'resource':
            self.resources[obj_id] = properties.get('value', 1.0)
        elif obj_type == 'hazard':
            self.hazards[obj_id] = properties.get('damage', 0.5)
    
    def step(self, timestep):
        """Update world state."""
        for obj in self.objects:
            if obj['active']:
                # Update position
                obj['x'] += obj['velocity'][0] * timestep
                obj['y'] += obj['velocity'][1] * timestep
                
                # Handle boundaries
                self._handle_boundaries(obj)
                
                # Check for interactions
                self._check_interactions(obj)
    
    def calculate_reward(self, agent_x, agent_y, agent_action):
        """Calculate reward based on agent state and action."""
        reward = 0.0
        
        # Resource rewards
        for obj_id, value in self.resources.items():
            obj = self._get_object(obj_id)
            if obj and self._distance(agent_x, agent_y, obj['x'], obj['y']) < obj['radius']:
                reward += value
                del self.resources[obj_id]  # Collect resource
                obj['active'] = False
        
        # Hazard penalties
        for obj_id, damage in self.hazards.items():
            obj = self._get_object(obj_id)
            if obj and self._distance(agent_x, agent_y, obj['x'], obj['y']) < obj['radius']:
                reward -= damage
                obj['active'] = False  # Hazard destroyed
        
        return reward
    
    def _distance(self, x1, y1, x2, y2):
        """Calculate Euclidean distance."""
        return np.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)
```

## Custom Development

### 1. Creating Custom Neuron Models

Extend the basic LIF neuron model:

```python
class CustomNeuron:
    """Custom neuron model extending LIF dynamics."""
    
    def __init__(self, neuron_params):
        self.v = neuron_params.get('v_reset', -65.0)  # Membrane potential
        self.threshold = neuron_params.get('threshold', -50.0)
        self.refractory_period = neuron_params.get('refractory_period', 2.0)
        self.refractory_timer = 0.0
        
        # Custom parameters
        self.adaptation_current = 0.0
        self.adaptation_strength = neuron_params.get('adaptation_strength', 0.01)
        self.spike_frequency_adaptation = neuron_params.get('sfa_strength', 0.0)
        self.sfa_counter = 0
    
    def step(self, input_current, dt):
        """Update neuron state."""
        if self.refractory_timer > 0:
            self.refractory_timer -= dt
            self.v = self._get_reset_potential()
            return 0  # No spike during refractory period
        
        # Custom LIF dynamics with adaptation
        tau_m = 20.0  # Membrane time constant
        v_rest = -65.0  # Resting potential
        
        # Membrane potential update
        dv = (-(self.v - v_rest) + input_current - self.adaptation_current) / tau_m * dt
        self.v += dv
        
        # Spike frequency adaptation
        if self.v >= self.threshold:
            spike = 1
            self.v = self._get_reset_potential()
            self.refractory_timer = self.refractory_period
            
            # Increase adaptation current
            self.adaptation_current += self.adaptation_strength
            
            # Reset SFA counter
            self.sfa_counter = 0
            
            return spike
        
        # Decay adaptation current
        self.adaptation_current *= 0.99
        
        # Increase SFA counter
        self.sfa_counter += 1
        
        return 0
    
    def _get_reset_potential(self):
        """Get reset potential with adaptation."""
        return -65.0 - self.adaptation_current * 0.1
```

### 2. Custom Synapse Models

Implement specialized synapse types:

```python
class NeuromodulatorySynapse:
    """Synapse that can be modulated by neuromodulators."""
    
    def __init__(self, presynaptic, postsynaptic, base_weight):
        self.presynaptic = presynaptic
        self.postsynaptic = postsynaptic
        self.base_weight = base_weight
        self.current_weight = base_weight
        
        # Neuromodulation parameters
        self.dopamine_modulation = 1.0  # 1.0 = no modulation
        self.curiosity_modulation = 1.0
        self.novelty_modulation = 1.0
    
    def update_modulation(self, dopamine_level, curiosity_level, novelty_level):
        """Update synaptic modulation based on neuromodulators."""
        self.dopamine_modulation = 1.0 + dopamine_level * 0.5
        self.curiosity_modulation = 1.0 + curiosity_level * 0.3
        self.novelty_modulation = 1.0 + novelty_level * 0.2
    
    def getEffectiveWeight(self):
        """Get weight considering neuromodulation."""
        return self.base_weight * self.dopamine_modulation * \
               self.curiosity_modulation * self.novelty_modulation
    
    def setWeight(self, weight):
        """Set synaptic weight."""
        self.base_weight = max(0.0, min(1.0, weight))  # Clamp to [0, 1]
        self.current_weight = self.getEffectiveWeight()
```

### 3. Custom Development Environment

Create custom development and testing environments:

```python
class NLMDevelopmentEnvironment:
    """Custom development environment for NLM."""
    
    def __init__(self, config):
        self.config = config
        self.experiments = []
        self.benchmarks = {}
        self.results = {}
    
    def create_experiment(self, name, parameters):
        """Create a new experiment."""
        experiment = {
            'name': name,
            'parameters': parameters,
            'brain': None,
            'world': None,
            'agent': None,
            'results': None
        }
        
        # Initialize with parameters
        self._initialize_experiment(experiment)
        
        self.experiments.append(experiment)
        return experiment
    
    def run_experiment(self, experiment_name, max_steps=1000):
        """Run an experiment."""
        experiment = self._get_experiment(experiment_name)
        
        if not experiment:
            raise ValueError(f"Experiment {experiment_name} not found")
        
        # Reset and initialize
        experiment['brain'].reset()
        experiment['brain'].initialize()
        
        experiment['world'].reset()
        
        # Run simulation
        step_results = []
        for step in range(max_steps):
            # Get sensory input
            percept = experiment['world'].getSensoryPercept()
            
            # Process through agent
            experiment['agent'].processSensoryInput(percept)
            
            # Generate action
            action = experiment['agent'].decodeMotorCommand()
            
            # Apply to world
            experiment['world'].applyMotorCommand(action, step * 0.001)
            
            # Step brain
            experiment['brain'].step(step)
            
            # Record step
            step_results.append({
                'step': step,
                'spike_count': experiment['brain'].getTotalSpikeCount(),
                'active_neurons': experiment['brain'].getActiveNeuronCount(),
                'reward': self._calculate_step_reward(experiment)
            })
        
        experiment['results'] = step_results
        self.results[experiment_name] = step_results
        
        return step_results
    
    def analyze_results(self, experiment_name):
        """Analyze experiment results."""
        results = self.results.get(experiment_name)
        
        if not results:
            raise ValueError(f"No results for experiment {experiment_name}")
        
        # Calculate statistics
        spike_counts = [r['spike_count'] for r in results]
        active_neurons = [r['active_neurons'] for r in results]
        rewards = [r['reward'] for r in results]
        
        analysis = {
            'total_steps': len(results),
            'average_spike_count': sum(spike_counts) / len(spike_counts),
            'max_spike_count': max(spike_counts),
            'average_active_neurons': sum(active_neurons) / len(active_neurons),
            'total_reward': sum(rewards),
            'reward_per_step': sum(rewards) / len(rewards),
            'spike_rate': sum(spike_counts) / (len(results) * 0.001)  # spikes per second
        }
        
        return analysis
    
    def compare_experiments(self, experiment_names):
        """Compare multiple experiments."""
        comparisons = {}
        
        for name in experiment_names:
            analysis = self.analyze_results(name)
            comparisons[name] = analysis
        
        # Create comparison summary
        summary = {
            'experiments': list(experiment_names),
            'metrics': ['total_steps', 'average_spike_count', 'max_spike_count',
                       'average_active_neurons', 'total_reward', 'reward_per_step'],
            'data': {}
        }
        
        for metric in summary['metrics']:
            summary['data'][metric] = {}
            for name, analysis in comparisons.items():
                summary['data'][metric][name] = analysis[metric]
        
        return summary
```

## Performance Optimization

### 1. Memory Optimization

Reduce memory usage for large-scale simulations:

```python
class MemoryOptimizedSimulation:
    """Memory-optimized simulation."""
    
    def __init__(self, brain, world, use_compact_storage=True):
        self.brain = brain
        self.world = world
        self.use_compact_storage = use_compact_storage
        
        if use_compact_storage:
            self._initialize_compact_storage()
    
    def _initialize_compact_storage(self):
        """Initialize compact storage structures."""
        # Use arrays instead of objects where possible
        self.spike_history = np.array([], dtype=np.float32)
        self.weight_history = np.array([], dtype=np.float32)
        
        # Implement sparse matrices for connectivity
        self.connectivity_matrix = self._create_sparse_connectivity()
    
    def _create_sparse_connectivity(self):
        """Create sparse connectivity matrix."""
        # Use scipy sparse matrix for large networks
        try:
            from scipy import sparse
            
            # Get brain connectivity
            regions = self.brain.getRegionIds()
            n_neurons = self.brain.getTotalNeuronCount()
            
            # Create sparse matrix
            row = []
            col = []
            data = []
            
            for region_id in regions:
                region = self.brain.getRegion(region_id)
                if region:
                    neurons = region.getAllNeurons()
                    for pre_neuron in neurons:
                        for syn in pre_neuron.getSynapses():
                            row.append(pre_neuron.getId())
                            col.append(syn.getPostNeuronId())
                            data.append(syn.getWeight())
            
            return sparse.csr_matrix((data, (row, col)), shape=(n_neurons, n_neurons))
            
        except ImportError:
            # Fall back to dense matrix if scipy not available
            return None
    
    def step(self, step):
        """Optimized simulation step."""
        # Use compact storage if enabled
        if self.use_compact_storage:
            self._step_compact(step)
        else:
            self._step_standard(step)
    
    def _step_compact(self, step):
        """Compact storage step."""
        # Get spike input using sparse matrix if available
        if self.connectivity_matrix is not None:
            # Get current spike vector
            spike_vector = self._get_spike_vector()
            
            # Matrix multiplication for synaptic input
            synaptic_input = self.connectivity_matrix.dot(spike_vector)
            
            # Apply to neurons
            self._apply_synaptic_input(synaptic_input)
        else:
            # Fallback to standard method
            self._step_standard(step)
        
        # Update history with compact representation
        self._update_history_compact(step)
    
    def _update_history_compact(self, step):
        """Update history with compact storage."""
        # Record only key metrics
        self.spike_history = np.append(self.spike_history, 
                                     self.brain.getTotalSpikeCount())
        
        # Sample weight changes periodically
        if step % 100 == 0:
            avg_weight = self._get_average_weight()
            self.weight_history = np.append(self.weight_history, avg_weight)
    
    def get_memory_usage(self):
        """Get current memory usage."""
        import sys
        
        memory_usage = {
            'spike_history': sys.getsizeof(self.spike_history),
            'weight_history': sys.getsizeof(self.weight_history)
        }
        
        if self.connectivity_matrix is not None:
            memory_usage['connectivity_matrix'] = self.connectivity_matrix.data.nbytes
        
        return memory_usage
```

### 2. Parallel Processing

Leverage parallel processing for large simulations:

```python
class ParallelSimulation:
    """Parallel simulation using multiprocessing."""
    
    def __init__(self, brain, world, num_processes=None):
        self.brain = brain
        self.world = world
        self.num_processes = num_processes or multiprocessing.cpu_count()
        self.process_pool = None
    
    def _initialize_process_pool(self):
        """Initialize process pool."""
        if self.process_pool is None:
            self.process_pool = multiprocessing.Pool(self.num_processes)
    
    def _simulate_step(self, step):
        """Simulate a single step (run in separate process)."""
        # This would run in a separate process
        # For demonstration, we'll keep it simple
        self.brain.step(step)
        
        # Get results
        results = {
            'step': step,
            'spike_count': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_rate': self.brain.getAverageFiringRate()
        }
        
        return results
    
    def run_parallel_simulation(self, steps, chunk_size=10):
        """Run simulation in parallel chunks."""
        self._initialize_process_pool()
        
        # Split steps into chunks
        step_ranges = []
        for i in range(0, steps, chunk_size):
            end = min(i + chunk_size, steps)
            step_ranges.append((i, end))
        
        # Run chunks in parallel
        all_results = []
        for start, end in step_ranges:
            chunk_results = self.process_pool.starmap(
                self._simulate_step,
                [(step,) for step in range(start, end)]
            )
            all_results.extend(chunk_results)
        
        return all_results
    
    def __del__(self):
        """Clean up process pool."""
        if self.process_pool is not None:
            self.process_pool.close()
            self.process_pool.join()
```

### 3. SIMD Optimization

Use SIMD instructions for vectorized operations:

```python
class SIMDProcessor:
    """SIMD-accelerated operations."""
    
    def __init__(self):
        # Check if numpy supports SIMD
        self.use_simd = hasattr(np, 'vectorize')
    
    def vectorized_spike_processing(self, input_array, weights):
        """Process spike inputs using vectorized operations."""
        if self.use_simd:
            # Use numpy vectorization
            return np.vectorize(lambda x, w: x * w)(input_array, weights)
        else:
            # Fallback to loop
            result = np.zeros_like(input_array)
            for i, (x, w) in enumerate(zip(input_array, weights)):
                result[i] = x * w
            return result
    
    def fast_weight_update(self, weights, changes):
        """Fast weight update using SIMD."""
        if self.use_simd:
            # Use numpy's optimized operations
            return weights + changes
        else:
            # Manual update
            result = np.zeros_like(weights)
            for i, (w, c) in enumerate(zip(weights, changes)):
                result[i] = w + c
            return result
```

## Experiment Design and Analysis

### 1. Experiment Design Framework

Design rigorous NLM experiments:

```python
class NLMDependentVariable:
    """Define dependent variables for NLM experiments."""
    
    def __init__(self):
        self.metrics = {}
        self.baselines = {}
        self.normalizations = {}
    
    def add_metric(self, name, function, normalization=None, baseline=None):
        """Add a metric to track."""
        self.metrics[name] = function
        self.normalizations[name] = normalization
        self.baselines[name] = baseline
    
    def calculate(self, brain, world, step):
        """Calculate all metrics for current state."""
        results = {}
        
        for name, function in self.metrics.items():
            try:
                value = function(brain, world, step)
                
                # Apply normalization
                if self.normalizations[name]:
                    value = self.normalizations[name](value)
                
                # Compare to baseline
                if self.baselines[name]:
                    baseline_value = self.baselines[name](brain, world, step)
                    value = value / baseline_value if baseline_value != 0 else 0
                
                results[name] = value
                
            except Exception as e:
                print(f"Error calculating metric {name}: {e}")
                results[name] = None
        
        return results
class NLMExperimentDesigner:
    """Design and manage NLM experiments."""
    
    def __init__(self, environment):
        self.environment = environment
        self.experiments = {}
        self.results = {}
    
    def create_experiment(self, name, design):
        """Create an experiment from design specification."""
        experiment = {
            'name': name,
            'design': design,
            'brain_config': self._create_brain_config(design),
            'world_config': self._create_world_config(design),
            'protocol': self._create_protocol(design),
            'dependent_variables': self._create_dependent_variables(design),
            'metadata': self._create_metadata(design)
        }
        
        self.experiments[name] = experiment
        return experiment
    
    def _create_brain_config(self, design):
        """Create brain configuration from experiment design."""
        config = {
            'neuron_count': design.get('neuron_count', 100),
            'region_count': design.get('region_count', 1),
            'connection_probability': design.get('connection_probability', 0.1),
            'simulation_timestep': design.get('simulation_timestep', 0.001),
            'random_seed': design.get('random_seed', 42)
        }
        
        # Add custom configurations
        for key, value in design.get('brain_params', {}).items():
            config[key] = value
        
        return config
    
    def _create_world_config(self, design):
        """Create world configuration from experiment design."""
        config = {
            'width': design.get('world_width', 100),
            'height': design.get('world_height', 100),
            'vision_width': design.get('vision_width', 20),
            'vision_height': design.get('vision_height', 20)
        }
        
        # Add custom world settings
        for key, value in design.get('world_params', {}).items():
            config[key] = value
        
        return config
    
    def _create_protocol(self, design):
        """Create experimental protocol."""
        protocol = {
            'phases': [],
            'duration': design.get('duration', 1000),
            'record_interval': design.get('record_interval', 10),
            'trials': design.get('trials', 1)
        }
        
        # Define experimental phases
        phases = design.get('phases', [])
        for phase in phases:
            protocol['phases'].append({
                'name': phase.get('name', 'phase'),
                'duration': phase.get('duration', protocol['duration']),
                'conditions': phase.get('conditions', [])
            })
        
        return protocol
    
    def _create_dependent_variables(self, design):
        """Create dependent variables for experiment."""
        dv = NLMDependentVariable()
        
        # Add standard metrics
        dv.add_metric('spike_count', self._get_spike_count, normalization=self._normalize_spike_count)
        dv.add_metric('firing_rate', self._get_firing_rate, normalization=self._normalize_rate)
        dv.add_metric('weight_change', self._get_weight_change, normalization=self._normalize_weight_change)
        dv.add_metric('learning_efficiency', self._get_learning_efficiency)
        dv.add_metric('novelty_response', self._get_novelty_response)
        
        # Add custom metrics
        for metric_name in design.get('custom_metrics', []):
            if hasattr(self, f'get_{metric_name}'):
                dv.add_metric(metric_name, getattr(self, f'get_{metric_name}'))
        
        return dv
    
    def _create_metadata(self, design):
        """Create experiment metadata."""
        return {
            'description': design.get('description', ''),
            'hypothesis': design.get('hypothesis', ''),
            'predictions': design.get('predictions', []),
            'references': design.get('references', []),
            'date': datetime.now().isoformat()
        }
    
    def run_experiment(self, name, custom_config=None):
        """Run a complete experiment."""
        experiment = self.experiments.get(name)
        
        if not experiment:
            raise ValueError(f"Experiment {name} not found")
        
        # Merge custom configuration
        if custom_config:
            for key, value in custom_config.items():
                if key in experiment['brain_config']:
                    experiment['brain_config'][key] = value
                elif key in experiment['world_config']:
                    experiment['world_config'][key] = value
        
        # Create brain and world
        brain = self.environment.create_brain(experiment['brain_config'])
        world = self.environment.create_world(experiment['world_config'])
        agent = self.environment.create_agent(brain)
        
        # Set up protocol
        protocol = experiment['protocol']
        
        # Run experiment
        all_results = []
        for trial in range(protocol['trials']):
            trial_results = self._run_trial(brain, world, agent, protocol, trial)
            all_results.append(trial_results)
        
        # Analyze results
        analysis = self._analyze_experiment_results(experiment, all_results)
        
        # Store results
        experiment['results'] = all_results
        self.results[name] = analysis
        
        return analysis
    
    def _run_trial(self, brain, world, agent, protocol, trial):
        """Run a single trial."""
        # Reset and initialize
        brain.reset()
        brain.initialize()
        
        world.reset()
        
        # Run protocol phases
        trial_results = []
        current_step = 0
        
        for phase in protocol['phases']:
            phase_results = self._run_phase(brain, world, agent, phase, current_step)
            trial_results.extend(phase_results)
            
            current_step += phase['duration']
        
        return trial_results
    
    def _run_phase(self, brain, world, agent, phase, start_step):
        """Run an experimental phase."""
        phase_results = []
        
        for step in range(start_step, start_step + phase['duration']):
            # Record data
            dependent_vars = phase['dependent_variables'](brain, world, step)
            
            # Get state information
            state = {
                'step': step,
                'dependent_variables': dependent_vars,
                'brain_state': self._get_brain_state(brain),
                'world_state': self._get_world_state(world),
                'agent_state': self._get_agent_state(agent)
            }
            
            phase_results.append(state)
            
            # Apply phase conditions
            self._apply_phase_conditions(brain, world, agent, phase, step)
            
            # Step simulation
            brain.step(step)
        
        return phase_results
    
    def _analyze_experiment_results(self, experiment, all_results):
        """Analyze experiment results across trials."""
        analysis = {
            'experiment': experiment['name'],
            'trials': len(all_results),
            'total_steps': sum(len(trial) for trial in all_results),
            'metrics': {}
        }
        
        # Calculate metrics for each dependent variable
        dependent_vars = experiment['dependent_variables'].metrics
        
        for metric_name in dependent_vars:
            all_values = []
            for trial in all_results:
                for step in trial:
                    value = step['dependent_variables'].get(metric_name)
                    if value is not None:
                        all_values.append(value)
            
            if all_values:
                analysis['metrics'][metric_name] = {
                    'mean': sum(all_values) / len(all_values),
                    'std': np.std(all_values) if len(all_values) > 1 else 0,
                    'min': min(all_values),
                    'max': max(all_values),
                    'median': np.median(all_values) if len(all_values) > 1 else all_values[0]
                }
        
        return analysis
```

### 2. Statistical Analysis

Perform statistical analysis on experimental results:

```python
class NLMStatisticalAnalyzer:
    """Statistical analysis for NLM experiments."""
    
    def __init__(self):
        self.tests = {}
    
    def add_test(self, name, test_function, parameters=None):
        """Add a statistical test."""
        self.tests[name] = {
            'function': test_function,
            'parameters': parameters or {}
        }
    
    def analyze(self, data, test_name):
        """Perform statistical analysis."""
        if test_name not in self.tests:
            raise ValueError(f"Test {test_name} not found")
        
        test_info = self.tests[test_name]
        return test_info['function'](data, **test_info['parameters'])
    
    def compare_conditions(self, data_dict, metric_name):
        """Compare conditions across experimental groups."""
        # Extract data for each condition
        condition_data = {}
        for condition, values in data_dict.items():
            if metric_name in values:
                condition_data[condition] = values[metric_name]
        
        if len(condition_data) < 2:
            return None
        
        # Perform statistical test (e.g., t-test)
        conditions = list(condition_data.keys())
        results = {}
        
        for i, cond1 in enumerate(conditions[:-1]):
            for cond2 in conditions[i+1:]:
                key = f"{cond1}_vs_{cond2}"
                results[key] = self._perform_t_test(
                    condition_data[cond1], condition_data[cond2]
                )
        
        return results
    
    def _perform_t_test(self, data1, data2):
        """Perform independent t-test."""
        from scipy import stats
        
        t_stat, p_value = stats.ttest_ind(data1, data2, equal_var=False)
        
        return {
            't_statistic': t_stat,
            'p_value': p_value,
            'significant': p_value < 0.05,
            'mean_difference': np.mean(data1) - np.mean(data2)
        }
    
    def calculate_effect_size(self, data1, data2):
        """Calculate effect size (Cohen's d)."""
        mean1, mean2 = np.mean(data1), np.mean(data2)
        std1, std2 = np.std(data1), np.std(data2)
        
        # Pooled standard deviation
        n1, n2 = len(data1), len(data2)
        pooled_std = np.sqrt(((n1 - 1) * std1 ** 2 + (n2 - 1) * std2 ** 2) / (n1 + n2 - 2))
        
        # Cohen's d
        d = (mean1 - mean2) / pooled_std if pooled_std != 0 else 0
        
        return {
            'effect_size': d,
            'interpretation': self._interpret_effect_size(d)
        }
    
    def _interpret_effect_size(self, d):
        """Interpret effect size."""
        if abs(d) < 0.2:
            return 'negligible'
        elif abs(d) < 0.5:
            return 'small'
        elif abs(d) < 0.8:
            return 'medium'
        else:
            return 'large'
    
    def perform_power_analysis(self, effect_size, alpha=0.05, power=0.8):
        """Perform power analysis."""
        from statsmodels.stats.power import TTestIndPower
        
        # Calculate required sample size
        analyzer = TTestIndPower()
        sample_size = analyzer.solve_power(
            effect_size=effect_size,
            alpha=alpha,
            power=power,
            ratio=1.0
        )
        
        return {
            'required_sample_size': sample_size,
            'effect_size': effect_size,
            'alpha': alpha,
            'power': power
        }
```

## Integration with Other Systems

### 1. Integration with Control Systems

Integrate NLM with real control systems:

```python
class NLMControlIntegration:
    """Integration with control systems."""
    
    def __init__(self, controller):
        self.controller = controller
        self.control_signals = []
        self.performance_metrics = {}
    
    def apply_control_signals(self, brain, world, timestep):
        """Apply control signals to NLM system."""
        # Get control commands from controller
        control_commands = self.controller.get_commands()
        
        # Convert to NLM actions
        for command in control_commands:
            # Map control signals to NLM actions
            if hasattr(self.controller, 'get_motor_command'):
                motor_command = self.controller.get_motor_command(command)
                if motor_command:
                    world.applyMotorCommand(motor_command, timestep)
            
            # Apply to brain
            if hasattr(self.controller, 'get_sensory_input'):
                sensory_input = self.controller.get_sensory_input(command)
                if sensory_input:
                    brain.receiveSensoryInput(sensory_input)
        
        self.control_signals.append({
            'timestep': timestep,
            'commands': control_commands
        })
    
    def get_performance_metrics(self):
        """Get performance metrics from control integration."""
        metrics = {
            'control_efficiency': self._calculate_control_efficiency(),
            'response_time': self._calculate_response_time(),
            'stability_margin': self._calculate_stability_margin()
        }
        
        self.performance_metrics.update(metrics)
        return metrics
    
    def _calculate_control_efficiency(self):
        """Calculate control efficiency."""
        if not self.control_signals:
            return 0.0
        
        # Calculate based on command effectiveness
        successful_commands = sum(1 for signal in self.control_signals 
                               if signal.get('successful', False))
        
        return successful_commands / len(self.control_signals)
```

### 2. Integration with Reinforcement Learning

Combine NLM with reinforcement learning:

```python
class NLMReinforcementLearningIntegration:
    """Integration with reinforcement learning systems."""
    
    def __init__(self, rl_agent, state_encoder, action_decoder):
        self.rl_agent = rl_agent
        self.state_encoder = state_encoder
        self.action_decoder = action_decoder
        self.rl_buffer = []
        self.nlm_buffer = []
    
    def run_rl_with_nlm(self, steps, explore=True):
        """Run RL simulation with NLM."""
        episode_reward = 0
        episode_steps = 0
        
        for step in range(steps):
            # Get NLM state
            state = self._get_nlm_state()
            
            # Encode state for RL agent
            encoded_state = self.state_encoder.encode(state)
            
            # Get RL action
            rl_action = self.rl_agent.act(encoded_state, explore=explore)
            
            # Decode to NLM action
            nlm_action = self.action_decoder.decode(rl_action)
            
            # Execute in NLM world
            reward = self._execute_action(nlm_action)
            
            # Get next state
            next_state = self._get_nlm_state()
            encoded_next_state = self.state_encoder.encode(next_state)
            
            # Store in replay buffers
            self.rl_buffer.append((encoded_state, rl_action, reward, encoded_next_state))
            self.nlm_buffer.append((state, nlm_action, reward))
            
            episode_reward += reward
            episode_steps += 1
            
            # Train RL agent if buffer is large enough
            if len(self.rl_buffer) >= self.rl_agent.minibatch_size:
                self._train_rl_agent()
        
        return {
            'episode_reward': episode_reward,
            'episode_steps': episode_steps,
            'rl_buffer_size': len(self.rl_buffer),
            'nlm_buffer_size': len(self.nlm_buffer)
        }
    
    def _get_nlm_state(self):
        """Get current NLM state for RL."""
        state = {
            'brain_spikes': self.brain.getTotalSpikeCount(),
            'brain_neurons': self.brain.getTotalNeuronCount(),
            'brain_activity': self._get_brain_activity(),
            'world_objects': self._get_world_objects(),
            'agent_state': self._get_agent_state()
        }
        
        return state
    
    def _execute_action(self, action):
        """Execute action in NLM world and get reward."""
        # Apply action to world
        self.world.applyMotorCommand(action, self.current_step)
        
        # Step brain
        self.brain.step(self.current_step)
        
        # Get reward
        reward = self._calculate_reward()
        
        # Update step counter
        self.current_step += 1
        
        return reward
    
    def _train_rl_agent(self):
        """Train RL agent from experience."""
        # Sample minibatch from RL buffer
        minibatch = self.rl_agent.sample_minibatch()
        
        # Train RL agent
        loss = self.rl_agent.train(minibatch)
        
        # Optionally train NLM from NLM buffer
        if self.nlm_buffer and len(self.nlm_buffer) >= self.nlm_agent.minibatch_size:
            self._train_nlm_agent()
```

### 3. Integration with Robotics

Use NLM with robotics systems:

```python
class NLMRoboticsIntegration:
    """Integration with robotics systems."""
    
    def __init__(self, robot_controller, sensors, actuators):
        self.robot_controller = robot_controller
        self.sensors = sensors
        self.actuators = actuators
        self.kinematics = self._initialize_kinematics()
        self.dynamics = self._initialize_dynamics()
    
    def run_robot_simulation(self, steps):
        """Run robot simulation using NLM."""
        for step in range(steps):
            # Get sensor readings
            sensor_data = self._get_sensor_readings()
            
            # Process through NLM brain
            self._process_sensors(sensor_data)
            
            # Get motor commands
            motor_commands = self._get_motor_commands()
            
            # Apply to robot
            self._apply_actuators(motor_commands)
            
            # Update robot dynamics
            self._update_kinematics(motor_commands, step * 0.001)
            
            # Get next sensor readings
            next_sensor_data = self._get_sensor_readings()
            
            # Calculate reward
            reward = self._calculate_robot_reward(sensor_data, next_sensor_data)
            
            # Store experience
            self._store_experience(sensor_data, motor_commands, reward, next_sensor_data)
        
        return self._get_simulation_results()
    
    def _initialize_kinematics(self):
        """Initialize robot kinematics model."""
        # Define robot structure (links, joints, etc.)
        return {
            'links': [],
            'joints': [],
            'end_effector': 'hand'
        }
    
    def _initialize_dynamics(self):
        """Initialize robot dynamics model."""
        return {
            'mass': 1.0,
            'inertia': 1.0,
            'friction': 0.1,
            'gravity': 9.81
        }
    
    def _get_sensor_readings(self):
        """Get current sensor readings."""
        readings = {}
        
        for sensor_name, sensor in self.sensors.items():
            readings[sensor_name] = sensor.get_reading()
        
        return readings
    
    def _process_sensors(self, sensor_data):
        """Process sensor data through NLM."""
        # Convert sensor data to NLM sensory input
        sensory_percept = self._create_sensory_percept(sensor_data)
        
        # Process through agent brain
        self.agent.processSensoryInput(sensory_percept)
    
    def _get_motor_commands(self):
        """Get motor commands from NLM."""
        return self.agent.decodeMotorCommand()
    
    def _apply_actuators(self, motor_commands):
        """Apply motor commands to robot actuators."""
        for actuator_name, actuator in self.actuators.items():
            # Map NLM motor commands to actuator commands
            if actuator_name == 'left_leg':
                command = motor_commands.get('MoveLeft', 0.0)
            elif actuator_name == 'right_leg':
                command = motor_commands.get('MoveRight', 0.0)
            elif actuator_name == 'arm':
                command = motor_commands.get('Interact', 0.0)
            else:
                continue
            
            actuator.apply(command)
    
    def _update_kinematics(self, motor_commands, timestep):
        """Update robot kinematics."""
        # Update joint positions
        for joint_name, joint in self.kinematics['joints'].items():
            # Calculate joint velocity from motor command
            velocity = self._get_joint_velocity(motor_commands, joint_name)
            
            # Update position
            joint['position'] += velocity * timestep
            
            # Apply dynamics
            self._apply_dynamics(joint, velocity, timestep)
    
    def _create_sensory_percept(self, sensor_data):
        """Create NLM sensory percept from sensor data."""
        from nlm import SensoryPercept
        
        percept = SensoryPercept()
        
        # Add vision data if available
        if 'vision' in sensor_data:
            vision_data = sensor_data['vision']
            percept.setVision(vision_data)
        
        # Add proprioception data
        if 'proprioception' in sensor_data:
            proprio_data = sensor_data['proprioception']
            percept.setProprioception(proprio_data)
        
        return percept
    
    def _calculate_robot_reward(self, sensor_data, next_sensor_data):
        """Calculate reward based on robot state change."""
        # Calculate reward based on task objectives
        # For example: distance to goal, energy efficiency, etc.
        
        reward = 0.0
        
        # Task completion reward
        if self._is_task_complete(next_sensor_data):
            reward += 10.0
        
        # Efficiency reward
        reward += self._calculate_efficiency_reward()
        
        # Smoothness reward (penalize jerky movements)
        reward -= self._calculate_smoothness_penalty()
        
        return reward
```

## Troubleshooting Advanced Issues

### 1. Memory Leaks in Long Simulations

**Problem:** Memory usage grows uncontrollably during long simulations.

**Diagnosis:**

1. Monitor memory usage:

```python
import psutil
import os

process = psutil.Process(os.getpid())
memory_info = process.memory_info()
print(f"Memory usage: {memory_info.rss / 1024 / 1024:.2f} MB")
```

2. Check for circular references:

```python
import gc
import sys

# Check for circular references
for obj in gc.get_objects():
    if isinstance(obj, (nlm.Brain, nlm.World, nlm.AgentBrain)):
        print(f"Object: {type(obj).__name__}")
        print(f"Reference count: {sys.getrefcount(obj)}")
```

**Solutions:**

1. **Use weak references**:

```python
import weakref

# Use weak references to avoid circular references
brain_ref = weakref.ref(brain)
world_ref = weakref.ref(world)

# Access through weak references
def process_brain():
    brain = brain_ref()
    if brain:
        brain.step(0)
```

2. **Implement checkpoint/restart**:

```python
def save_checkpoint(brain, world, filepath):
    """Save simulation checkpoint."""
    brain.save(filepath + "_brain.nlm")
    # Also save world state if needed
    with open(filepath + "_world.txt", 'w') as f:
        # Save world state
        pass

def restore_checkpoint(filepath):
    """Restore simulation from checkpoint."""
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.load(filepath + "_brain.nlm")
    # Restore world state
    return brain
```

3. **Clear large data structures**:

```python
def cleanup_simulation(brain, world):
    """Clean up simulation data."""
    # Clear spike history
    if hasattr(brain, 'clear_spike_history'):
        brain.clear_spike_history()
    
    # Clear weight history
    if hasattr(brain, 'clear_weight_history'):
        brain.clear_weight_history()
    
    # Reset agent brain
    if hasattr(world, 'getAgentBrain'):
        agent_brain = world.getAgentBrain()
        if agent_brain:
            agent_brain.reset()
```

### 2. Performance Degradation

**Problem:** Simulation performance slows down over time.

**Diagnosis:**

1. Profile performance:

```python
from nlm.nlm_utils import NLMBenchmark

benchmark = NLMBenchmark()

# Profile different operations
brain = nlm.createBrain(config)
benchmark.time_operation("brain_init", brain.initialize)

for step in range(100):
    benchmark.time_operation("brain_step", brain.step, step)
    benchmark.time_operation("world_update", world.update, 0.001)

benchmark.print_summary()
```

2. Check for bottlenecks:

```python
import time

def profile_operation(func, *args, **kwargs):
    """Profile a single operation."""
    start_time = time.time()
    result = func(*args, **kwargs)
    end_time = time.time()
    
    return {
        'result': result,
        'time': end_time - start_time
    }

# Profile specific operations
profile_operation(brain.step, 0)
profile_operation(world.update, 0.001)
profile_operation(brain.produceAction)
```

**Solutions:**

1. **Optimize configuration**:

```python
# Use optimal configuration for performance
config = nlm.createDefaultConfig()
config.set('neuron_count', 100)  # Smaller network
config.set('region_count', 1)    # Fewer regions
config.set('connection_probability', 0.05)  # Sparse connections
```

2. **Use optimized simulation**: Refer to performance optimization section

3. **Implement sampling**: Record only key metrics:

```python
def record_sample_data(step, interval=10):
    """Record sample data to reduce memory usage."""
    if step % interval == 0:
        # Record only key metrics
        sample = {
            'step': step,
            'spike_count': brain.getTotalSpikeCount(),
            'weight_change': brain.getAverageWeightChange(),
            'firing_rate': brain.getAverageFiringRate()
        }
        
        # Store in compact format
        samples.append(sample)
```

### 3. Numerical Instability

**Problem:** Simulation becomes numerically unstable.

**Diagnosis:**

1. Check for overflow/underflow:

```python
import numpy as np

def check_numerical_stability(brain, world):
    """Check for numerical instability."""
    issues = []
    
    # Check spike counts
    spike_count = brain.getTotalSpikeCount()
    if spike_count > 1e9:  # Overflow
        issues.append("Spike count overflow")
    
    # Check weight values
    avg_weight = brain.getAverageSynapticWeight()
    if not np.isfinite(avg_weight):  # NaN or infinity
        issues.append("Invalid synaptic weight")
    
    # Check membrane potentials
    # (would need to access neuron states)
    
    return issues
```

2. Monitor for NaN/inf values:

```python
def monitor_numerical_values(brain, world):
    """Monitor for numerical issues."""
    issues = []
    
    # Check for NaN values in spike counts
    spike_count = brain.getTotalSpikeCount()
    if np.isnan(spike_count) or np.isinf(spike_count):
        issues.append(f"Invalid spike count: {spike_count}")
    
    # Check for negative weights
    avg_weight = brain.getAverageSynapticWeight()
    if avg_weight < 0:  # Valid but unusual
        issues.append(f"Negative average weight: {avg_weight}")
    
    return issues
```

**Solutions:**

1. **Implement numerical stability measures**:

```python
def apply_numerical_stability(brain, world):
    """Apply numerical stability measures."""
    # Clamp values to reasonable ranges
    if hasattr(brain, 'clamp_weights'):
        brain.clamp_weights(0.0, 1.0)  # Weights to [0, 1]
    
    if hasattr(brain, 'clamp_spike_counts'):
        brain.clamp_spike_counts(0, 1e6)  # Reasonable spike count range
    
    # Use stable numerical methods
    if hasattr(world, 'use_stable_integration'):
        world.use_stable_integration(True)
```

2. **Add monitoring and alerts**:

```python
def setup_numerical_monitoring(brain, world):
    """Set up numerical monitoring."""
    def check_stability(step):
        issues = monitor_numerical_values(brain, world)
        
        if issues:
            print(f"Step {step}: Numerical issues detected:")
            for issue in issues:
                print(f"  - {issue}")
            
            # Apply stability measures
            apply_numerical_stability(brain, world)
    
    return check_stability
```

### 4. AgentBrain Integration Issues

**Problem:** AgentBrain doesn't integrate properly with world or brain.

**Diagnosis:**

1. Check initialization:

```python
def check_agent_initialization(agent_brain, world, brain):
    """Check AgentBrain initialization."""
    issues = []
    
    # Check if agent brain was initialized with world
    if not hasattr(agent_brain, 'world_initialized'):
        issues.append("AgentBrain not properly initialized")
    
    # Check if brain is accessible
    if agent_brain.getBrain() is None:
        issues.append("AgentBrain has no brain reference")
    
    # Check world configuration
    if world.getWidth() == 0 or world.getHeight() == 0:
        issues.append("World has invalid dimensions")
    
    return issues
```

2. Check method availability:

```python
def check_agent_brain_methods(agent_brain):
    """Check if AgentBrain has required methods."""
    required_methods = [
        'processSensoryInput',
        'decodeMotorCommand',
        'applyRewardModulation',
        'updateDevelopment',
        'reset'
    ]
    
    missing_methods = []
    for method in required_methods:
        if not hasattr(agent_brain, method):
            missing_methods.append(method)
    
    return missing_methods
```

**Solutions:**

1. **Fix initialization sequence**:

```python
# Correct initialization sequence
def initialize_agent_system(brain, world):
    """Initialize agent system in correct order."""
    # 1. Create agent brain
    agent_brain = nlm.createAgentBrain(brain)
    
    # 2. Configure world
    world.configure(100, 100, 20, 20)
    
    # 3. Initialize agent brain with world
    agent_brain.initialize(world)
    
    # 4. Verify initialization
    if agent_brain.getSensoryInputSize() == 0:
        raise ValueError("AgentBrain failed to initialize properly")
    
    return agent_brain
```

2. **Add debugging information**:

```python
def debug_agent_brain(agent_brain, world, brain):
    """Debug AgentBrain issues."""
    print("AgentBrain Debug Information:")
    print(f"  - Sensory input size: {agent_brain.getSensoryInputSize()}")
    print(f"  - Motor output size: {agent_brain.getMotorOutputSize()}")
    print(f"  - Development stage: {agent_brain.getDevelopmentalStage()}")
    print(f"  - Neuromodulation level: {agent_brain.getNeuromodulationLevel()}")
    
    # Check world configuration
    print(f"  - World dimensions: {world.getWidth()}x{world.getHeight()}")
    print(f"  - World time: {world.getSimulationTime()}")
    
    # Check brain status
    print(f"  - Brain neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Brain spikes: {brain.getTotalSpikeCount()}")
```

## Extending the Framework

### 1. Creating Custom Brain Models

Extend the existing brain model:

```python
class ExtendedBrain(nlm.Brain):
    """Extended brain model with additional features."""
    
    def __init__(self, config):
        super().__init__(config)
        
        # Additional features
        self.custom_plasticity_rules = {}
        self.external_inputs = []
        self.internal_states = {}
    
    def add_external_input(self, input_data, source):
        """Add external input to brain."""
        self.external_inputs.append({
            'data': input_data,
            'source': source,
            'timestamp': self.getCurrentTime()
        })
        
        # Process external input
        if hasattr(self, 'processExternalInput'):
            self.processExternalInput(input_data)
    
    def get_custom_state(self, state_name):
        """Get custom internal state."""
        return self.internal_states.get(state_name)
    
    def set_custom_state(self, state_name, value):
        """Set custom internal state."""
        self.internal_states[state_name] = value
    
    def apply_custom_plasticity(self, rule_name, pre_neuron, post_neuron, correlation):
        """Apply custom plasticity rule."""
        if rule_name in self.custom_plasticity_rules:
            rule = self.custom_plasticity_rules[rule_name]
            weight_change = rule(pre_neuron, post_neuron, correlation)
            
            # Apply weight change
            synapse = pre_neuron.getSynapseTo(post_neuron)
            if synapse:
                current_weight = synapse.getWeight()
                new_weight = current_weight + weight_change
                synapse.setWeight(new_weight)
    
    def get_enhanced_metrics(self):
        """Get enhanced metrics for monitoring."""
        metrics = super().get_metrics()
        
        # Add custom metrics
        metrics['external_inputs'] = len(self.external_inputs)
        metrics['custom_states'] = len(self.internal_states)
        metrics['plasticity_rules'] = len(self.custom_plasticity_rules)
        
        return metrics
```

### 2. Custom Visualization

Create custom visualization for NLM:

```python
class CustomVisualizer:
    """Custom visualization for NLM simulations."""
    
    def __init__(self, width=800, height=600):
        self.width = width
        self.height = height
        self.figure = None
        self.setup_plot()
    
    def setup_plot(self):
        """Set up the plot."""
        import matplotlib.pyplot as plt
        
        self.figure, self.axes = plt.subplots(2, 2, figsize=(12, 10))
        self.axes = self.axes.flatten()
        
        # Setup different plot types
        self.setup_neural_activity_plot()
        self.setup_weight_dynamics_plot()
        self.setup_memory_traces_plot()
        self.setup_development_plot()
    
    def setup_neural_activity_plot(self):
        """Setup neural activity plot."""
        ax = self.axes[0]
        ax.set_title("Neural Activity")
        ax.set_xlabel("Time")
        ax.set_ylabel("Spike Rate")
        ax.grid(True, alpha=0.3)
    
    def setup_weight_dynamics_plot(self):
        """Setup weight dynamics plot."""
        ax = self.axes[1]
        ax.set_title("Synaptic Weight Dynamics")
        ax.set_xlabel("Time")
        ax.set_ylabel("Average Weight")
        ax.grid(True, alpha=0.3)
    
    def setup_memory_traces_plot(self):
        """Setup memory traces plot."""
        ax = self.axes[2]
        ax.set_title("Memory Traces")
        ax.set_xlabel("Time")
        ax.set_ylabel("Trace Strength")
        ax.grid(True, alpha=0.3)
    
    def setup_development_plot(self):
        """Setup development plot."""
        ax = self.axes[3]
        ax.set_title("Development Progress")
        ax.set_xlabel("Time")
        ax.set_ylabel("Development Stage")
        ax.set_ylim(-0.1, 1.1)
        ax.grid(True, alpha=0.3)
    
    def update_plots(self, brain, world, step):
        """Update all plots with current data."""
        # Get current data
        spike_rate = brain.getAverageFiringRate()
        weight_dynamics = self._get_weight_dynamics(brain)
        memory_traces = self._get_memory_traces(brain)
        development_stage = brain.getDevelopmentalStage()
        
        # Update plots
        self._update_neural_activity_plot(step, spike_rate)
        self._update_weight_dynamics_plot(step, weight_dynamics)
        self._update_memory_traces_plot(step, memory_traces)
        self._update_development_plot(step, development_stage)
        
        # Redraw
        plt.pause(0.01)
    
    def _update_neural_activity_plot(self, step, spike_rate):
        """Update neural activity plot."""
        ax = self.axes[0]
        # Add new data point
        ax.plot(step, spike_rate, 'r-', alpha=0.7)
        
        # Update x/y limits if needed
        if step > ax.get_xlim()[1]:
            ax.set_xlim(step - 100, step + 10)
        if spike_rate > ax.get_ylim()[1]:
            ax.set_ylim(0, spike_rate * 1.1)
    
    def _update_weight_dynamics_plot(self, step, weight_dynamics):
        """Update weight dynamics plot."""
        ax = self.axes[1]
        ax.plot(step, weight_dynamics, 'b-', alpha=0.7)
        
        if step > ax.get_xlim()[1]:
            ax.set_xlim(step - 100, step + 10)
    
    def _update_memory_traces_plot(self, step, memory_traces):
        """Update memory traces plot."""
        ax = self.axes[2]
        for trace_name, trace_value in memory_traces.items():
            ax.plot(step, trace_value, label=trace_name, alpha=0.7)
        
        ax.legend(loc='upper right')
    
    def _update_development_plot(self, step, development_stage):
        """Update development plot."""
        ax = self.axes[3]
        # Convert developmental stage to numeric value
        stage_value = {
            nlm.DevelopmentalStage.Initial: 0.0,
            nlm.DevelopmentalStage.CriticalPeriod: 0.3,
            nlm.DevelopmentalStage.Maturation: 0.6,
            nlm.DevelopmentalStage.Adult: 0.9,
            nlm.DevelopmentalStage.Aging: 1.0
        }.get(development_stage, 0.0)
        
        ax.plot(step, stage_value, 'g-', linewidth=2)
        
        if step > ax.get_xlim()[1]:
            ax.set_xlim(step - 100, step + 10)
    
    def save_figure(self, filepath):
        """Save figure to file."""
        if self.figure:
            self.figure.savefig(filepath, dpi=300, bbox_inches='tight')
    
    def close(self):
        """Close the figure."""
        if self.figure:
            plt.close(self.figure)
```

### 3. Custom Testing Framework

Create comprehensive testing framework:

```python
class NLMLongTermTest:
    """Long-term NLM testing framework."""
    
    def __init__(self, test_name, duration_steps=10000):
        self.test_name = test_name
        self.duration_steps = duration_steps
        self.results = {}
        self.checkpoints = []
    
    def run_test(self):
        """Run long-term test."""
        print(f"Running {self.test_name} test for {self.duration_steps} steps...")
        
        # Initialize system
        self._initialize_system()
        
        # Run simulation
        for step in range(self.duration_steps):
            # Run simulation step
            self._run_step(step)
            
            # Record data at intervals
            if step % 100 == 0:
                self._record_checkpoint(step)
            
            # Check for issues
            issues = self._check_for_issues(step)
            if issues:
                print(f"Issues detected at step {step}:")
                for issue in issues:
                    print(f"  - {issue}")
        
        # Analyze results
        self._analyze_results()
        
        return self.results
    
    def _initialize_system(self):
        """Initialize system for testing."""
        # Create brain and world
        config = nlm.createDefaultConfig()
        config.set('neuron_count', 500)
        config.set('region_count', 2)
        
        self.brain = nlm.createBrain(config)
        self.brain.initialize()
        
        self.world = nlm.createSimpleWorld()
        self.world.configure(100, 100, 20, 20)
        
        self.agent_brain = nlm.createAgentBrain(self.brain)
        self.agent_brain.initialize(self.world)
        
        # Initialize test-specific components
        self._initialize_test_components()
    
    def _initialize_test_components(self):
        """Initialize test-specific components."""
        # Override methods for testing
        pass
    
    def _run_step(self, step):
        """Run a single test step."""
        # Run simulation
        percept = self.world.getSensoryPercept()
        self.agent_brain.processSensoryInput(percept)
        
        motor_command = self.agent_brain.decodeMotorCommand()
        self.world.applyMotorCommand(motor_command, step * 0.001)
        
        self.brain.step(step)
        
        # Update development
        self.agent_brain.updateDevelopment(0.001)
        
        # Record metrics
        self._record_metrics(step)
    
    def _record_checkpoint(self, step):
        """Record simulation checkpoint."""
        checkpoint = {
            'step': step,
            'spike_count': self.brain.getTotalSpikeCount(),
            'weight_dynamics': self._get_weight_dynamics(),
            'learning_status': self._get_learning_status(),
            'memory_traces': self._get_memory_traces(),
            'development_stage': self.brain.getDevelopmentalStage()
        }
        
        self.checkpoints.append(checkpoint)
        
        # Save to disk if needed
        if step % 1000 == 0:
            self._save_checkpoint(step)
    
    def _check_for_issues(self, step):
        """Check for issues during simulation."""
        issues = []
        
        # Check numerical stability
        if step > 0:
            prev_spike_count = self.checkpoints[-1]['spike_count']
            curr_spike_count = self.brain.getTotalSpikeCount()
            
            # Check for spike count explosion
            if curr_spike_count - prev_spike_count > 1000000:
                issues.append(f"Excessive spike activity: {curr_spike_count}")
        
        # Check for learning stagnation
        learning_status = self._get_learning_status()
        if learning_status['weight_change'] < 0.00001:
            issues.append("Learning appears stagnant")
        
        # Check for memory issues
        memory_traces = self._get_memory_traces()
        for trace_name, trace_value in memory_traces.items():
            if trace_value > 100:  # Unusual memory trace strength
                issues.append(f"Unusual memory trace: {trace_name} = {trace_value}")
        
        return issues
    
    def _get_weight_dynamics(self):
        """Get current weight dynamics."""
        # Calculate average synaptic weight
        total_weight = 0.0
        total_synapses = 0
        
        for region_id in self.brain.getRegionIds():
            region = self.brain.getRegion(region_id)
            if region:
                for synapse in region.getSynapses():
                    total_weight += synapse.getWeight()
                    total_synapses += 1
        
        return total_weight / total_synapses if total_synapses > 0 else 0.0
    
    def _get_learning_status(self):
        """Get current learning status."""
        # Calculate weight changes from checkpoints
        if len(self.checkpoints) < 2:
            return {'weight_change': 0.0, 'learning_detected': False}
        
        prev_checkpoint = self.checkpoints[-2]
        curr_checkpoint = self.checkpoints[-1]
        
        weight_change = curr_checkpoint['weight_dynamics'] - prev_checkpoint['weight_dynamics']
        
        return {
            'weight_change': weight_change,
            'learning_detected': abs(weight_change) > 0.001
        }
    
    def _get_memory_traces(self):
        """Get current memory traces."""
        # Get memory system states
        traces = {}
        
        if hasattr(self.brain, 'workingMemory'):
            traces['working_memory'] = self.brain.workingMemory.getActiveTraces()
        
        if hasattr(self.brain, 'episodicMemory'):
            traces['episodic_memory'] = self.brain.episodicMemory.getEpisodeCount()
        
        return traces
    
    def _record_metrics(self, step):
        """Record metrics at each step."""
        metrics = {
            'step': step,
            'spike_count': self.brain.getTotalSpikeCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'weight_dynamics': self._get_weight_dynamics(),
            'learning_status': self._get_learning_status(),
            'memory_traces': self._get_memory_traces(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'reward': self._calculate_reward()
        }
        
        if 'metrics' not in self.results:
            self.results['metrics'] = []
        
        self.results['metrics'].append(metrics)
    
    def _save_checkpoint(self, step):
        """Save checkpoint to disk."""
        import pickle
        import os
        
        checkpoint_dir = f"checkpoints/{self.test_name}"
        os.makedirs(checkpoint_dir, exist_ok=True)
        
        checkpoint_file = os.path.join(checkpoint_dir, f"checkpoint_step_{step}.pkl")
        
        with open(checkpoint_file, 'wb') as f:
            pickle.dump(self.checkpoints, f)
    
    def _analyze_results(self):
        """Analyze test results."""
        if 'metrics' not in self.results:
            return
        
        metrics = self.results['metrics']
        
        # Calculate summary statistics
        self.results['summary'] = {
            'total_steps': len(metrics),
            'final_spike_count': metrics[-1]['spike_count'],
            'average_spike_count': np.mean([m['spike_count'] for m in metrics]),
            'spike_count_std': np.std([m['spike_count'] for m in metrics]),
            'final_weight_change': metrics[-1]['learning_status']['weight_change'],
            'learning_episodes': sum(1 for m in metrics if m['learning_status']['learning_detected']),
            'final_development_stage': metrics[-1]['development_stage']
        }
        
        # Save analysis
        self._save_analysis()
    
    def _save_analysis(self):
        """Save analysis to file."""
        import json
        
        analysis_file = f"analysis/{self.test_name}_analysis.json"
        
        with open(analysis_file, 'w') as f:
            json.dump(self.results, f, indent=2)
```

## Future Directions

### 1. Machine Learning Integration

- **Hybrid NLM-ML Systems**: Combine NLM with traditional machine learning
- **Transfer Learning**: Apply NLM insights to improve ML models
- **Meta-Learning**: Use NLM to learn how to learn

### 2. Neuroscience Applications

- **Brain Modeling**: Use NLM to model biological brains
- **Neurodegenerative Disease Models**: Simulate disease progression
- **Neurogenesis Studies**: Study neural development and regeneration

### 3. AI Safety and Alignment

- **Transparent AI**: Use NLM's interpretable nature for explainable AI
- **Robust AI**: Develop robust NLM architectures
- **Alignment Research**: Use NLM to study AI alignment

### 4. Education and Outreach

- **Interactive Learning**: Use NLM for educational purposes
- **Research Tools**: Provide tools for neuroscience research
- **Public Engagement**: Develop public-facing NLM demonstrations

### 5. Performance Improvements

- **Hardware Acceleration**: Optimize for GPUs and specialized hardware
- **Distributed Computing**: Scale NLM to large-scale simulations
- **Real-time Applications**: Enable real-time NLM applications

This advanced usage guide provides a comprehensive foundation for working with the NLM framework at an advanced level. The NLM framework offers tremendous potential for research and applications in neuroscience, artificial intelligence, and cognitive modeling. By understanding and utilizing these advanced features, researchers and developers can create sophisticated neural simulations and cognitive architectures.

## References

- Markram, S., et al. (2015). The neuro-syntax of the brain. Frontiers in Neurorobotics.
- El Boustani, S., & Detrance, F. (2020). Deep learning-based decoders: How can they help brain-machine interfaces? Neural Processing Letters.
- Bogacz, R., et al. (2010). The physics of learning: theoretical approaches to neural dynamics. Neural Networks.

For the latest documentation and examples, visit the NLM project website and GitHub repository.

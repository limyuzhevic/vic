# NLM Advanced User Guide

## Overview

This guide provides advanced usage patterns, configuration examples, and best practices for the NLM (Neural Learning Machine) framework. It covers:

- Advanced configuration and customization
- Python API advanced usage
- Performance optimization techniques
- Research use cases and examples
- Debugging and troubleshooting

## Advanced Configuration

### JSON/YAML Configuration Examples

#### Basic Configuration

```json
{
  "neuron_count": 5000,
  "region_count": 4,
  "connection_probability": 0.05,
  "simulation_timestep": 0.0005,
  "random_seed": 12345,
  "stdp_ltp_weight": 0.015,
  "stdp_ltd_weight": 0.018,
  "stdp_tau": 25.0,
  "synaptogenesis_rate": 0.0002,
  "pruning_rate": 0.000005,
  "replay_interval": 200,
  "consolidation_interval": 5000,
  "checkpoint_dir": "./checkpoints",
  "max_checkpoints": 20
}
```

#### Environment-Specific Configurations

```json
{
  "name": "production_environment",
  "neuron_count": 10000,
  "region_count": 8,
  "connection_probability": 0.02,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.01,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.005
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001
    }
  },
  
  "memory": {
    "working_memory": {
      "capacity": 2000,
      "decay_rate": 0.95
    },
    "episodic_memory": {
      "max_episodes": 5000,
      "replay_frequency": 100
    }
  },
  
  "neuromodulation": {
    "dopamine": {
      "scale": 1.0,
      "plasticity_factor": 1.5
    },
    "curiosity": {
      "enable": true,
      "exploration_rate": 0.1
    },
    "novelty": {
      "enable": true,
      "detection_sensitivity": 0.5
    }
  },
  
  "development": {
    "initial_plasticity": 1.0,
    "plasticity_decay_rate": 0.01,
    "critical_period_duration": 1000,
    "maturation_threshold": 5000
  }
}
```

### Configuration Loading Strategies

```python
import pynlm

# 1. Load from JSON file
config = pynlm.Config()
config.load_from_file("advanced_config.json")

# 2. Load from JSON string
json_str = '{"neuron_count": 5000, "connection_probability": 0.05}'
config.load_from_json(json_str)

# 3. Merge configurations
base_config = pynlm.Config()
base_config.load_from_file("base_config.json")

user_config = pynlm.Config()
user_config.load_from_file("user_config.json")

# Merge user config into base (user values override)
for key in user_config.getKeys():
    value = user_config.get(key)
    if value is not None:
        base_config.set(key, value)

# 4. Configuration inheritance
class NLMConfig:
    def __init__(self, base_config_path=None):
        self.config = pynlm.Config()
        if base_config_path:
            self.config.load_from_file(base_config_path)
        self.apply_environment_settings()
    
    def apply_environment_settings(self):
        """Apply environment-specific settings"""
        if hasattr(self, 'environment'):
            env_settings = self.get_environment_config()
            for key, value in env_settings.items():
                self.config.set(key, value)
    
    def get_environment_config(self):
        # Return environment-specific configuration
        return {
            "neuron_count": self.get_neuron_count(),
            "connection_probability": self.get_connection_probability(),
            # ... other settings
        }
```

## Python API Advanced Usage

### Advanced Brain Configuration

```python
import pynlm
import math

def create_advanced_brain(config_path=None):
    """Create a brain with advanced configuration"""
    if config_path:
        config = pynlm.Config()
        config.load_from_file(config_path)
    else:
        config = pynlm.create_default_config()
    
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Configure advanced brain features
    brain.set_developmental_stage(pynlm.DevelopmentalStage.CriticalPeriod)
    
    # Add specialized regions
    sensory_region = brain.add_region("sensory_cortex")
    motor_region = brain.add_region("motor_cortex")
    association_region = brain.add_region("association_cortex")
    
    # Configure region-specific properties
    config.set("region_1.plasticity.stdp_learning_rate", 0.02)
    config.set("region_2.plasticity.stdp_learning_rate", 0.015)
    config.set("region_3.plasticity.stdp_learning_rate", 0.01)
    
    return brain

def run_advanced_simulation(brain, world, agent, steps=10000):
    """Run advanced simulation with multiple phases"""
    phase_length = steps // 3
    
    # Phase 1: Initialization and exploration
    print("Phase 1: Exploration")
    for step in range(phase_length):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        if step % 500 == 0:
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Enable curiosity for exploration
            if agent.getCuriosityLevel() < 0.3:
                agent.enable_curiosity(True)
    
    # Phase 2: Structured learning
    print("Phase 2: Structured Learning")
    brain.set_developmental_stage(pynlm.DevelopmentalStage.Maturation)
    agent.enable_reward_modulation(True)
    
    for step in range(phase_length, 2 * phase_length):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Use prediction error for learning
        prediction_error = agent.get_prediction_error()
        if prediction_error > 0.5:
            reward = world.getLastReward()
            agent.apply_reward_modulation(reward, prediction_error * 0.1)
    
    # Phase 3: Consolidation and skill refinement
    print("Phase 3: Consolidation")
    brain.set_developmental_stage(pynlm.DevelopmentalStage.Adult)
    agent.enable_structural_plasticity(True)
    
    for step in range(2 * phase_length, steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Periodically consolidate important memories
        if step % 1000 == 0:
            # Save important memories
            if agent.get_working_memory():
                agent.get_working_memory().consolidate(0.5)
        
        # Check for skill mastery
        performance = evaluate_performance(brain, world, agent)
        if performance > 0.8:
            print(f"Skill mastery achieved at step {step}")
    
    return brain
```

### Memory Management

```python
import pynlm

def manage_memory_systems(brain):
    """Advanced memory system management"""
    working_memory = brain.get_working_memory()
    episodic_memory = brain.get_episodic_memory()
    associative_memory = brain.get_associative_memory()
    
    # Configure working memory
    working_memory.set_capacity(5000)
    working_memory.set_decay_rate(0.95)
    working_memory.set_competition_strength(1.0)
    
    # Configure episodic memory
    episodic_memory.set_max_episodes(10000)
    episodic_memory.set_replay_frequency(100)
    episodic_memory.set_priority_threshold(0.7)
    episodic_memory.enable_automatic_consolidation(True)
    
    # Configure associative memory
    associative_memory.set_max_associations(50000)
    associative_memory.set_association_strength_threshold(0.8)
    associative_memory.enable_hierarchical_learning(True)
    
    return working_memory, episodic_memory, associative_memory

def store_experience(brain, world, agent, step):
    """Store experience with advanced encoding"""
    # Get current brain state
    active_neurons = brain.get_active_neurons()
    firing_patterns = brain.get_firing_patterns(step)
    
    # Encode experience with multiple dimensions
    experience = {
        'step': step,
        'world_state': encode_world_state(world),
        'action': agent.decode_motor_command(),
        'reward': world.get_last_reward(),
        'brain_state': {
            'active_neurons': active_neurons,
            'firing_patterns': firing_patterns,
            'developmental_stage': brain.get_developmental_stage(),
            'novelty_level': agent.get_novelty_level(),
            'curiosity_level': agent.get_curiosity_level()
        }
    }
    
    # Store in multiple memory systems
    if agent.get_working_memory():
        working_memory = agent.get_working_memory()
        working_memory.store_experience(experience)
    
    if agent.get_episodic_memory():
        episodic_memory = agent.get_episodic_memory()
        episodic_memory.store_episode(experience)
    
    return experience
```

### Neuromodulation Control

```python
import pynlm
import time

def advanced_neuromodulation_control(brain, agent, world):
    """Advanced neuromodulation control strategies"""
    dopamine = brain.get_dopamine()
    curiosity = brain.get_curiosity()
    novelty = brain.get_novelty()
    
    # Strategy 1: Reward prediction error based modulation
    prediction_error = agent.get_prediction_error()
    reward = world.get_last_reward()
    
    # Dynamic neuromodulation based on prediction error
    if prediction_error > 0.7:
        # High prediction error -> increase curiosity
        curiosity.set_exploration_rate(0.3)
        dopamine.set_plasticity_factor(1.2)
    elif prediction_error < 0.3:
        # Low prediction error -> focus on consolidation
        curiosity.set_exploration_rate(0.05)
        dopamine.set_plasticity_factor(0.8)
    else:
        # Balanced learning
        curiosity.set_exploration_rate(0.15)
        dopamine.set_plasticity_factor(1.0)
    
    # Strategy 2: Time-based neuromodulation cycles
    current_time = time.time()
    neuromodulation_cycle = int(current_time / 60.0) % 4  # 4-minute cycles
    
    if neuromodulation_cycle == 0:
        # Exploration phase
        curiosity.enable(True)
        novelty.enable(True)
        dopamine.set_plasticity_factor(1.5)
    elif neuromodulation_cycle == 1:
        # Exploitation phase
        curiosity.enable(False)
        novelty.enable(False)
        dopamine.set_plasticity_factor(0.9)
    elif neuromodulation_cycle == 2:
        # Integration phase
        curiosity.enable(True)
        novelty.enable(True)
        dopamine.set_plasticity_factor(1.2)
    else:
        # Consolidation phase
        curiosity.enable(False)
        novelty.enable(True)
        dopamine.set_plasticity_factor(1.1)
    
    # Strategy 3: Performance-based modulation
    performance = calculate_performance(brain, agent, world)
    
    if performance > 0.9:
        # High performance -> reduce exploration, increase consolidation
        curiosity.set_exploration_rate(0.1)
        dopamine.set_plasticity_factor(0.7)
        # Enable structural plasticity for skill固化
        agent.enable_structural_plasticity(True)
    elif performance < 0.3:
        # Low performance -> increase exploration
        curiosity.set_exploration_rate(0.4)
        dopamine.set_plasticity_factor(1.8)
        # Enable curiosity for novel exploration
        agent.enable_curiosity(True)
    else:
        # Moderate performance -> balanced approach
        curiosity.set_exploration_rate(0.2)
        dopamine.set_plasticity_factor(1.0)
    
    return curiosity, novelty, dopamine
```

## Performance Optimization

### Memory Management

```python
def optimize_memory_usage(brain, world, agent):
    """Optimize memory usage for large-scale simulations"""
    # Use sparse connectivity for large networks
    brain.enable_sparse_connectivity(True)
    brain.set_sparsity_threshold(0.95)  # Only 5% of possible connections
    
    # Configure memory pools
    if hasattr(brain, 'get_memory_pool'):
        memory_pool = brain.get_memory_pool()
        memory_pool.set_max_allocation(1024 * 1024 * 1024)  # 1GB
        memory_pool.enable_automatic_cleanup(True)
    
    # Use event-driven updates for efficiency
    brain.enable_event_driven_updates(True)
    
    # Configure checkpointing strategy
    checkpoint_manager = brain.get_checkpoint_manager()
    checkpoint_manager.set_checkpoint_interval(10000)  # Every 10k steps
    checkpoint_manager.set_compression_level(pynlm.Checkpoint.CompressionLevel.Balanced)
    
    return brain

def parallel_simulation_processing(brain, world, agent, num_threads=4):
    """Process simulation with multiple threads"""
    # Configure parallel processing
    brain.enable_parallel_processing(True)
    brain.set_thread_count(num_threads)
    
    # Use thread-local state for efficiency
    brain.enable_thread_local_state(True)
    
    # Process neurons in parallel batches
    batch_size = brain.get_total_neuron_count() // num_threads
    
    print(f"Processing {brain.get_total_neuron_count()} neurons across {num_threads} threads")
    print(f"Batch size: {batch_size} neurons per thread")
    
    return brain
```

### Simulation Optimization

```python
import time
import math

def optimized_simulation_loop(brain, world, agent, steps):
    """Optimized simulation loop with batching and caching"""
    # Pre-compute common values
    world_width = world.get_width()
    world_height = world.get_height()
    timestep = 0.1
    
    # Cache frequently used values
    vision_cache = {}
    action_cache = {}
    
    start_time = time.time()
    
    for step in range(steps):
        # Optimized world update
        world.update(timestep)
        
        # Cache sensory percept if position hasn't changed much
        agent_pos = world.get_agent_body()
        cache_key = (int(agent_pos.x / 2), int(agent_pos.y / 2))  # Coarser grid
        
        if cache_key in vision_cache and step > 100:
            # Use cached percept
            percept = vision_cache[cache_key]
        else:
            # Compute new percept
            percept = world.getSensoryPercept()
            vision_cache[cache_key] = percept
            
            # Limit cache size
            if len(vision_cache) > 1000:
                vision_cache.clear()
        
        # Process sensory input with optimization
        agent.process_sensory_input(optimized_percept_processing(percept))
        
        # Brain step with parallel processing
        brain.step(step)
        
        # Cache motor commands based on brain state
        brain_state_key = hash_brain_state(brain)
        if brain_state_key in action_cache:
            action = action_cache[brain_state_key]
        else:
            action = agent.decode_motor_command()
            action_cache[brain_state_key] = action
            
            if len(action_cache) > 1000:
                action_cache.clear()
        
        # Apply action with batching for multiple steps
        apply_action_batch(world, agent, action, timestep)
        
        # Periodic cleanup and memory management
        if step % 1000 == 0:
            cleanup_simulation_resources(brain, world, agent)
        
        # Log progress
        if step % 1000 == 0:
            elapsed = time.time() - start_time
            steps_per_second = step / elapsed
            print(f"Step {step}/{steps} ({steps_per_second:.1f} steps/sec)")
    
    return brain

def apply_action_batch(world, agent, action, timestep, batch_size=5):
    """Apply same action multiple times for efficiency"""
    for _ in range(batch_size):
        world.apply_motor_command(action, world.get_simulation_time())
        world.update(timestep)
        
        # Update agent state
        agent.update_body(world.get_agent_body())
```

## Research Use Cases

### 1. Studying Learning Trajectories

```python
def study_learning_trajectories(num_experiments=100):
    """Study learning trajectories across many experiments"""
    trajectories = []
    
    for exp in range(num_experiments):
        # Create brain with specific configuration
        config = pynlm.Config()
        config.set("neuron_count", 1000)
        config.set("random_seed", exp)  # Different seed for each experiment
        
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Run environment
        world = pynlm.create_simple_world()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.create_agent_brain(brain)
        agent.initialize(world)
        
        # Track learning metrics
        experiment_data = {
            'experiment_id': exp,
            'steps': [],
            'rewards': [],
            'prediction_errors': [],
            'novelty_levels': [],
            'development_stages': []
        }
        
        # Run simulation
        for step in range(1000):
            world.update(0.1)
            percept = world.get_sensory_percept()
            agent.process_sensory_input(percept)
            brain.step(step)
            
            # Record metrics
            experiment_data['steps'].append(step)
            experiment_data['rewards'].append(world.get_last_reward())
            experiment_data['prediction_errors'].append(agent.get_prediction_error())
            experiment_data['novelty_levels'].append(agent.get_novelty_level())
            experiment_data['development_stages'].append(brain.get_developmental_stage())
        
        trajectories.append(experiment_data)
        
        # Clean up for next experiment
        del brain, world, agent
    
    return analyze_trajectories(trajectories)

def analyze_trajectories(trajectories):
    """Analyze learning trajectories"""
    analysis = {
        'mean_final_reward': 0,
        'reward_variance': 0,
        'learning_speed': [],
        'novelty_patterns': [],
        'development_transitions': []
    }
    
    for traj in trajectories:
        final_reward = traj['rewards'][-1]
        analysis['mean_final_reward'] += final_reward
        analysis['reward_variance'] += final_reward ** 2
        
        # Calculate learning speed (steps to reach 50% of max reward)
        max_reward = max(traj['rewards'])
        half_max = max_reward * 0.5
        learning_step = next((i for i, r in enumerate(traj['rewards']) if r >= half_max), len(traj['rewards']))
        analysis['learning_speed'].append(learning_step)
    
    # Calculate statistics
    n = len(trajectories)
    analysis['mean_final_reward'] /= n
    analysis['reward_variance'] = (analysis['reward_variance'] / n) - (analysis['mean_final_reward'] ** 2)
    analysis['reward_variance'] = max(0, analysis['reward_variance'])
    analysis['mean_learning_speed'] = sum(analysis['learning_speed']) / len(analysis['learning_speed'])
    
    return analysis
```

### 2. Developmental Studies

```python
def study_development_across_age(brain_template, age_stages):
    """Study brain development across different age stages"""
    development_data = {}
    
    for stage_name, stage_params in age_stages.items():
        print(f"Studying development stage: {stage_name}")
        
        # Create brain with specific developmental parameters
        brain = pynlm.create_brain(brain_template.get_config())
        brain.initialize()
        
        # Set developmental stage
        brain.set_developmental_stage(stage_params['stage'])
        
        # Configure developmental parameters
        brain.get_development_system().set_plasticity_rate(stage_params['plasticity_rate'])
        brain.get_development_system().set_critical_period(stage_params['critical_period'])
        brain.get_development_system().set_maturation_threshold(stage_params['maturation_threshold'])
        
        # Run development simulation
        stage_data = {
            'stage_name': stage_name,
            'initial_neuron_count': brain.get_total_neuron_count(),
            'initial_synapse_count': brain.get_total_synapse_count(),
            'plasticity_rate': stage_params['plasticity_rate'],
            'metrics': []
        }
        
        for step in range(stage_params['duration']):
            # Simulate development
            brain.step(step, step * stage_params['timestep'])
            
            # Record developmental metrics
            metrics = {
                'step': step,
                'neuron_count': brain.get_total_neuron_count(),
                'synapse_count': brain.get_total_synapse_count(),
                'firing_rate': brain.get_average_firing_rate(),
                'e_i_ratio': brain.get_excitation_inhibition_ratio(),
                'developmental_progress': brain.get_developmental_progress()
            }
            
            stage_data['metrics'].append(metrics)
            
            # Update development system
            brain.get_development_system().update(step, stage_params['timestep'])
        
        development_data[stage_name] = stage_data
        
        print(f"Completed stage {stage_name}")
    
    return analyze_development_data(development_data)

def analyze_development_data(development_data):
    """Analyze developmental data"""
    analysis = {
        'stage_transitions': {},
        'plasticity_dynamics': {},
        'growth_patterns': {}
    }
    
    stages = list(development_data.keys())
    
    for i, stage_name in enumerate(stages):
        stage_data = development_data[stage_name]
        metrics = stage_data['metrics']
        
        if i > 0:
            # Analyze transition from previous stage
            prev_stage = stages[i - 1]
            prev_metrics = development_data[prev_stage]['metrics'][-1]
            
            analysis['stage_transitions'][stage_name] = {
                'neuron_growth_rate': (
                    (stage_data['metrics'][-1]['neuron_count'] - 
                     prev_metrics['neuron_count']) / prev_metrics['neuron_count']
                ),
                'plasticity_change': stage_data['plasticity_rate'] - development_data[prev_stage]['plasticity_rate'],
                'firing_rate_change': (
                    (stage_data['metrics'][-1]['firing_rate'] - 
                     prev_metrics['firing_rate']) / prev_metrics['firing_rate']
                )
            }
        
        # Analyze plasticity dynamics within stage
        plasticity_values = []
        for metric in metrics:
            if metric['neuron_count'] > 0:
                plasticity_values.append(metric['firing_rate'] / metric['neuron_count'])
        
        if plasticity_values:
            analysis['plasticity_dynamics'][stage_name] = {
                'mean_plasticity': sum(plasticity_values) / len(plasticity_values),
                'plasticity_variance': sum((p - sum(plasticity_values)/len(plasticity_values))**2 
                                         for p in plasticity_values) / len(plasticity_values),
                'stability': calculate_stability(plasticity_values)
            }
        
        # Analyze growth patterns
        analysis['growth_patterns'][stage_name] = {
            'final_neuron_count': metrics[-1]['neuron_count'],
            'final_synapse_count': metrics[-1]['synapse_count'],
            'synapse_to_neuron_ratio': metrics[-1]['synapse_count'] / metrics[-1]['neuron_count'],
            'e_i_ratio_final': metrics[-1]['e_i_ratio']
        }
    
    return analysis
```

## Advanced Python Examples

### Example 1: Multi-Objective Optimization

```python
import pynlm
import random

def multi_objective_optimization(brain_config, objectives):
    """Optimize brain configuration for multiple objectives"""
    results = []
    
    for iteration in range(objectives['num_iterations']):
        # Create random brain configuration
        config = pynlm.Config()
        config.load_from_file(brain_config)  # Base configuration
        
        # Add random variations
        config.set("neuron_count", random.randint(
            objectives['neuron_range'][0],
            objectives['neuron_range'][1]
        ))
        
        config.set("connection_probability", 
                  random.uniform(objectives['connection_range'][0],
                               objectives['connection_range'][1]))
        
        # Create and test brain
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Run simulation and evaluate objectives
        scores = evaluate_objectives(brain, objectives['simulation_steps'])
        
        results.append({
            'config': config,
            'scores': scores,
            'iteration': iteration
        })
        
        # Select best configuration for next iteration (simple elitism)
        best_result = max(results, key=lambda r: r['scores']['performance'])
        config = best_result['config']
        
        # Apply some mutation
        mutate_configuration(config, objectives['mutation_rate'])
    
    return results

def evaluate_objectives(brain, steps):
    """Evaluate multiple objectives for a brain configuration"""
    # Create world and agent
    world = pynlm.create_simple_world()
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # Track objective values
    objective_values = {}
    
    # Objective 1: Performance (reward accumulation)
    total_reward = 0
    for step in range(steps):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        brain.step(step)
        
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        total_reward += world.get_last_reward()
    
    objective_values['performance'] = total_reward / steps
    
    # Objective 2: Learning speed (reduction in prediction error)
    initial_error = calculate_initial_prediction_error(brain)
    final_error = agent.get_prediction_error()
    objective_values['learning_speed'] = (initial_error - final_error) / initial_error if initial_error > 0 else 0
    
    # Objective 3: Stability (variance in performance)
    objective_values['stability'] = calculate_stability(total_reward, steps)
    
    # Objective 4: Novelty (exploration-exploitation balance)
    curiosity_level = agent.get_curiosity_level()
    novelty_level = agent.get_novelty_level()
    objective_values['exploration_balance'] = (curiosity_level + novelty_level) / 2
    
    return objective_values
```

### Example 2: Transfer Learning

```python
import pynlm
import copy

def transfer_learning(source_config, target_task_config):
    """Apply transfer learning from source task to target task"""
    # Load source task configuration
    source_brain = create_brain_from_config(source_config)
    
    # Extract learned features from source task
    source_features = extract_learned_features(source_brain)
    
    # Create target task brain
    target_brain = create_brain_from_config(target_task_config)
    target_brain.initialize()
    
    # Apply transfer of learned features
    apply_transferred_features(target_brain, source_features)
    
    # Fine-tune for target task
    fine_tune_for_target(target_brain, target_task_config)
    
    return target_brain

def extract_learned_features(brain):
    """Extract learned features from trained brain"""
    features = {
        'weight_patterns': extract_weight_patterns(brain),
        'neural_activities': extract_activity_patterns(brain),
        'memory_traces': extract_memory_traces(brain),
        'development_state': brain.get_developmental_stage(),
        'plasticity_settings': get_plasticity_settings(brain)
    }
    
    return features

def apply_transferred_features(target_brain, source_features):
    """Apply extracted features to target brain"""
    # Transfer weight patterns (initialize with similar connectivity)
    transfer_weight_patterns(target_brain, source_features['weight_patterns'])
    
    # Transfer neural activity patterns (initialize with similar activity levels)
    transfer_activity_patterns(target_brain, source_features['neural_activities'])
    
    # Transfer memory traces (initialize working memory)
    transfer_memory_traces(target_brain, source_features['memory_traces'])
    
    # Transfer developmental state (skip early development)
    if source_features['development_state'] != pynlm.DevelopmentalStage.Initial:
        target_brain.set_developmental_stage(source_features['development_state'])
    
    # Transfer plasticity settings (higher initial plasticity)
    for key, value in source_features['plasticity_settings'].items():
        target_brain.set(f"plasticity.{key}", value)

def fine_tune_for_target(target_brain, target_config):
    """Fine-tune transferred brain for target task"""
    # Create environment for target task
    world = create_target_environment(target_config)
    agent = pynlm.create_agent_brain(target_brain)
    agent.initialize(world)
    
    # Enable specific learning mechanisms for target task
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Run fine-tuning simulation
    for step in range(target_config['fine_tune_steps']):
        world.update(0.1)
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        target_brain.step(step)
        
        # Use target task specific reward function
        reward = calculate_target_task_reward(world, target_brain)
        prediction_error = agent.get_prediction_error()
        agent.apply_reward_modulation(reward, prediction_error * 0.1)
    
    return target_brain
```

## Configuration Management

### Configuration Templates

```python
# Configuration templates for different use cases
CONFIG_TEMPLATES = {
    'quick_test': {
        'neuron_count': 100,
        'region_count': 1,
        'connection_probability': 0.1,
        'simulation_timestep': 0.01,
        'random_seed': 42
    },
    
    'research_standard': {
        'neuron_count': 5000,
        'region_count': 4,
        'connection_probability': 0.05,
        'simulation_timestep': 0.001,
        'random_seed': 12345,
        'stdp_ltp_weight': 0.015,
        'stdp_ltd_weight': 0.018,
        'replay_interval': 200,
        'consolidation_interval': 5000
    },
    
    'production': {
        'neuron_count': 10000,
        'region_count': 8,
        'connection_probability': 0.02,
        'simulation_timestep': 0.001,
        'random_seed': 99999,
        'stdp_ltp_weight': 0.01,
        'stdp_ltd_weight': 0.012,
        'replay_interval': 500,
        'consolidation_interval': 10000,
        'checkpoint_dir': "./checkpoints",
        'max_checkpoints': 50
    },
    
    'high_performance': {
        'neuron_count': 20000,
        'region_count': 16,
        'connection_probability': 0.01,
        'simulation_timestep': 0.0005,
        'random_seed': 11111,
        'stdp_ltp_weight': 0.008,
        'stdp_ltd_weight': 0.009,
        'replay_interval': 1000,
        'consolidation_interval': 20000,
        'enable_sparse_connectivity': True,
        'sparsity_threshold': 0.98,
        'enable_parallel_processing': True,
        'thread_count': 8
    }
}

def get_template(template_name):
    """Get configuration template by name"""
    if template_name in CONFIG_TEMPLATES:
        return CONFIG_TEMPLATES[template_name]
    else:
        raise ValueError(f"Unknown template: {template_name}")

def create_config_from_template(template_name, custom_overrides=None):
    """Create configuration from template with optional overrides"""
    template = get_template(template_name)
    config = pynlm.Config()
    
    # Apply template settings
    for key, value in template.items():
        config.set(key, value)
    
    # Apply custom overrides
    if custom_overrides:
        for key, value in custom_overrides.items():
            config.set(key, value)
    
    return config
```

## Conclusion

This advanced user guide provides comprehensive information for working with the NLM framework at an advanced level. The improvements made to the Config system and Python bindings provide:

1. **Robust Configuration Management**: JSON/YAML support with proper serialization
2. **Enhanced Python API**: More Pythonic interfaces and better error handling
3. **Code Quality**: Fixed duplicate includes and improved maintainability
4. **Advanced Usage Patterns**: Examples for research and production use cases
5. **Performance Optimization**: Techniques for large-scale simulations

The NLM framework is now better equipped to handle:

- Complex research experiments
- Large-scale simulations
- Performance-critical applications
- Multi-objective optimization
- Transfer learning scenarios
- Development studies across different age stages

The improvements maintain backward compatibility while providing new capabilities for advanced users and researchers.
EOF

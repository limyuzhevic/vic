"""
Utility functions and high-level operations for NLM
"""

from typing import List, Dict, Any, Optional, Callable
from .core import Brain, Config, Action
from .agent import AgentBrain, SensoryPercept
from .world import SimpleWorld, ActionType
from .exceptions import NLMError

def run_simulation(brain: Brain, world: SimpleWorld, agent: AgentBrain, 
                   num_steps: int, timestep: float = 0.1) -> Dict[str, Any]:
    """Run a complete agent simulation"""
    # Initialize agent with world
    agent.initialize(world)
    
    # Track statistics
    stats = {
        'steps': num_steps,
        'firing_neurons': [],
        'spike_counts': [],
        'action_counts': {},
        'rewards': [],
        'novelty_levels': [],
        'curiosity_levels': [],
        'developmental_stages': []
    }
    
    for step in range(num_steps):
        # Update world
        world.update(timestep)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from agent
        action = agent.decode_motor_command()
        
        # Apply action to world
        # Note: This would require converting ActionType to appropriate action
        # For simplicity, we'll just log the action
        action_str = str(action).split('.')[-1] if hasattr(action, '__str__') else str(action)
        stats['action_counts'][action_str] = stats['action_counts'].get(action_str, 0) + 1
        
        # Apply action to world (placeholder)
        # world.apply_motor_command(action, world.get_simulation_time())
        
        # Record statistics
        stats['firing_neurons'].append(brain.get_firing_neuron_count())
        stats['spike_counts'].append(brain.get_total_spike_count())
        stats['novelty_levels'].append(agent.get_novelty_level())
        stats['curiosity_levels'].append(agent.get_curiosity_level())
        stats['developmental_stages'].append(agent.get_developmental_stage())
        
    return stats

def capture_brain_state(brain: Brain) -> Dict[str, Any]:
    """Capture complete brain state as Python dictionary"""
    return {
        'neurons': {
            'total_count': brain.get_total_neuron_count(),
            'active_count': brain.get_active_neuron_count(),
            'firing_count': brain.get_firing_neuron_count(),
            'average_firing_rate': brain.get_average_firing_rate(),
            'exhibition_inhibition_ratio': brain.get_excitation_inhibition_ratio(),
            'total_spike_count': brain.get_total_spike_count()
        },
        'regions': {
            'count': brain.get_region_count(),
            'ids': brain.get_region_ids()
        },
        'synapses': {
            'total_count': brain.get_total_synapse_count()
        },
        'neuromodulation': {
            'dopamine': brain.get_dopamine().get_level() if brain.get_dopamine() else None,
            'curiosity': brain.get_curiosity().get_level() if brain.get_curiosity() else None,
            'novelty': brain.get_novelty().get_level() if brain.get_novelty() else None,
            'prediction_error': brain.get_prediction_error() if brain.get_prediction_error() else None
        },
        'memory': {
            'working_memory_traces': brain.get_working_memory().get_active_traces() if brain.get_working_memory() else {},
            'episodic_episodes': brain.get_episodic_memory().get_episode_count() if brain.get_episodic_memory() else 0,
            'associative_associations': brain.get_associative_memory().get_association_count() if brain.get_associative_memory() else 0
        },
        'development': {
            'stage': brain.get_developmental_stage(),
            'age': brain.get_development_system().get_age() if brain.get_development_system() else 0
        },
        'statistics': {
            'pending_spike_events': brain.get_pending_spike_event_count()
        }
    }

def restore_brain_state(brain: Brain, state: Dict[str, Any]) -> bool:
    """Restore brain state from Python dictionary (placeholder)"""
    # This would require implementing state restore functionality
    # For now, just return True indicating it worked
    return True

def set_config_batch(config: Config, config_dict: Dict[str, Any]) -> None:
    """Set multiple configuration values at once"""
    config.set_batch(config_dict)

def validate_config(config: Config) -> List[str]:
    """Validate configuration values and return list of errors"""
    errors = []
    
    # Check for required fields
    if not config.has("neuron_count"):
        errors.append("Missing required field: neuron_count")
        
    if not config.has("region_count"):
        errors.append("Missing required field: region_count")
        
    # Validate values
    try:
        neuron_count = config.get("neuron_count")
        if neuron_count <= 0:
            errors.append("neuron_count must be positive")
    except (KeyError, TypeError):
        pass
        
    try:
        region_count = config.get("region_count")
        if region_count <= 0:
            errors.append("region_count must be positive")
    except (KeyError, TypeError):
        pass
        
    try:
        connection_prob = config.get("connection_probability")
        if not (0.0 <= connection_prob <= 1.0):
            errors.append("connection_probability must be between 0.0 and 1.0")
    except (KeyError, TypeError):
        pass
        
    return errors

def benchmark_simulation(brain: Brain, world: SimpleWorld, agent: AgentBrain, 
                         num_steps: int, timestep: float = 0.1) -> Dict[str, Any]:
    """Benchmark simulation performance"""
    import time
    
    start_time = time.time()
    stats = run_simulation(brain, world, agent, num_steps, timestep)
    end_time = time.time()
    
    duration = end_time - start_time
    steps_per_second = num_steps / duration if duration > 0 else 0
    
    return {
        'steps': num_steps,
        'duration_seconds': duration,
        'steps_per_second': steps_per_second,
        'time_per_step_ms': (duration / num_steps) * 1000 if num_steps > 0 else 0,
        'statistics': stats,
        'efficiency': {
            'spike_efficiency': stats['spike_counts'][-1] / max(1, num_steps),
            'firing_efficiency': stats['firing_neurons'][-1] / max(1, brain.get_total_neuron_count())
        }
    }

def analyze_spiking_patterns(spike_counts: List[int], window: int = 10) -> Dict[str, Any]:
    """Analyze spiking patterns"""
    if not spike_counts:
        return {
            'mean_rate': 0.0,
            'std_rate': 0.0,
            'max_rate': 0.0,
            'min_rate': 0.0,
            'trend': 'stable',
            'pattern': 'none'
        }
        
    # Calculate basic statistics
    mean_rate = sum(spike_counts) / len(spike_counts)
    std_rate = (sum((x - mean_rate) ** 2 for x in spike_counts) / len(spike_counts)) ** 0.5
    max_rate = max(spike_counts)
    min_rate = min(spike_counts)
    
    # Analyze trend (simple linear regression)
    if len(spike_counts) >= 2:
        x = list(range(len(spike_counts)))
        y = spike_counts
        n = len(x)
        
        x_mean = sum(x) / n
        y_mean = mean_rate
        
        numerator = sum((x[i] - x_mean) * (y[i] - y_mean) for i in range(n))
        denominator = sum((x[i] - x_mean) ** 2 for i in range(n))
        
        if denominator > 0:
            slope = numerator / denominator
        else:
            slope = 0.0
            
        if abs(slope) < 0.01:
            trend = 'stable'
        elif slope > 0.01:
            trend = 'increasing'
        else:
            trend = 'decreasing'
    else:
        trend = 'insufficient_data'
        
    # Analyze pattern (simple peak detection)
    peaks = 0
    for i in range(1, len(spike_counts) - 1):
        if spike_counts[i] > spike_counts[i-1] and spike_counts[i] > spike_counts[i+1]:
            peaks += 1
            
    if peaks == 0:
        pattern = 'monotonic'
    elif peaks <= 2:
        pattern = 'oscillatory'
    else:
        pattern = 'complex'
        
    return {
        'mean_rate': mean_rate,
        'std_rate': std_rate,
        'max_rate': max_rate,
        'min_rate': min_rate,
        'trend': trend,
        'pattern': pattern,
        'total_spikes': sum(spike_counts),
        'peak_count': peaks,
        'coefficient_of_variation': std_rate / mean_rate if mean_rate > 0 else 0.0
    }

def plot_neural_activity(spike_counts: List[int], title: str = "Neural Activity") -> Any:
    """Plot neural activity (placeholder)"""
    # This would create a matplotlib plot
    # For now, just return the data for plotting
    return {
        'title': title,
        'x': list(range(len(spike_counts))),
        'y': spike_counts,
        'mean': sum(spike_counts) / len(spike_counts) if spike_counts else 0,
        'max': max(spike_counts) if spike_counts else 0
    }

def create_progress_callback(callback: Callable[[int, int], None]) -> Callable:
    """Create progress callback function"""
    def progress(current: int, total: int) -> None:
        percentage = (current / total) * 100
        callback(current, total)
        
    return progress

def save_results(data: Dict[str, Any], filepath: str) -> bool:
    """Save results to file"""
    try:
        import json
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        return True
    except Exception:
        return False

def load_results(filepath: str) -> Optional[Dict[str, Any]]:
    """Load results from file"""
    try:
        import json
        with open(filepath, 'r') as f:
            return json.load(f)
    except Exception:
        return None

def create_experiment_setup(config: Config) -> Dict[str, Any]:
    """Create experiment setup from configuration"""
    return {
        'config': config,
        'world_config': {
            'width': config.get('world.width', 20),
            'height': config.get('world.height', 20),
            'vision_width': config.get('world.vision_width', 8),
            'vision_height': config.get('world.vision_height', 8)
        },
        'brain_config': {
            'neuron_count': config.get('neuron_count', 1000),
            'region_count': config.get('region_count', 1),
            'connection_probability': config.get('connection_probability', 0.1)
        }
    }

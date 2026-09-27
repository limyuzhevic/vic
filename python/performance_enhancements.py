"""
Performance Optimizations and Convenience Methods
================================================

This file contains performance optimizations and additional convenience methods
for the NLM Python API.

Save as: performance_enhancements.py
"""

import time
import statistics
import pynlm

def benchmark_simulation(brain, world, agent, num_steps, warmup_steps=100):
    """
    Benchmark NLM simulation performance.
    
    Runs a simulation with warmup steps to stabilize, then measures
    performance metrics including time per step and throughput.
    
    Args:
        brain (Brain): NLM brain object
        world (SimpleWorld): World object
        agent (AgentBrain): Agent brain interface
        num_steps (int): Number of steps to run for benchmark
        warmup_steps (int): Number of warmup steps (default: 100)
        
    Returns:
        dict: Performance metrics including:
            - steps_per_second: Simulation throughput
            - time_per_step_ms: Average time per step in milliseconds
            - neuron_count: Number of neurons
            - synapse_count: Number of synapses
            - avg_firing_rate: Average firing rate
            - total_spikes: Total spike count
            - memory_usage_estimate: Estimated memory usage (rough approximation)
    """
    # Warmup to stabilize the system
    for step in range(warmup_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Clear statistics
    brain.reset()
    
    # Benchmark the actual simulation
    start_time = time.time()
    
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    # Calculate performance metrics
    steps_per_second = num_steps / elapsed
    time_per_step_ms = (elapsed / num_steps) * 1000
    
    # Get brain statistics
    stats = brain.getStats()
    neuron_count = stats[0]
    synapse_count = stats[1]
    firing_count = stats[2]
    avg_firing_rate = stats[3]
    total_spikes = stats[4]
    
    # Estimate memory usage (rough approximation)
    # Each neuron: ~1KB, each synapse: ~100 bytes, plus overhead
    memory_usage_estimate = (neuron_count * 1024 + synapse_count * 100 + 10000) / (1024 * 1024)
    
    return {
        'steps_per_second': steps_per_second,
        'time_per_step_ms': time_per_step_ms,
        'neuron_count': neuron_count,
        'synapse_count': synapse_count,
        'avg_firing_rate': avg_firing_rate,
        'total_spikes': total_spikes,
        'memory_usage_estimate_mb': memory_usage_estimate,
        'simulation_time_seconds': elapsed
    }

def optimize_for_reality(brain, agent, world, optimize_for="performance"):
    """
    Optimize NLM configuration for specific use cases.
    
    This function adjusts brain and agent parameters to optimize for
    different scenarios like real-time performance, accuracy, or memory usage.
    
    Args:
        brain (Brain): NLM brain object
        agent (AgentBrain): Agent brain interface
        world (SimpleWorld): World object
        optimize_for (str): Optimization target. One of:
            - "performance": Maximize simulation speed
            - "accuracy": Maximize simulation accuracy
            - "memory": Minimize memory usage
            - "balance": Balance between all factors
            
    Returns:
        dict: Optimization results and recommended settings
    """
    if optimize_for not in ["performance", "accuracy", "memory", "balance"]:
        raise ValueError(f"Invalid optimization target: {optimize_for}")
    
    config = brain.getConfig()
    
    if optimize_for == "performance":
        # Optimize for speed: fewer neurons, simpler plasticity
        config.setInt("neuron_count", max(500, config.getInt("neuron_count", 1000) // 2))
        config.setFloat("plasticity.stdp.learning_rate", 0.001)
        config.setFloat("plasticity.hebbian.enable", False)
        config.setFloat("plasticity.structural.enable", False)
        
    elif optimize_for == "accuracy":
        # Optimize for accuracy: more neurons, complex plasticity
        config.setInt("neuron_count", min(5000, config.getInt("neuron_count", 1000) * 2))
        config.setFloat("plasticity.stdp.learning_rate", 0.01)
        config.setFloat("plasticity.hebbian.enable", True)
        config.setFloat("plasticity.structural.enable", True)
        
    elif optimize_for == "memory":
        # Optimize for memory: minimal neurons and connections
        config.setInt("neuron_count", 100)
        config.setFloat("connection_probability", 0.01)
        config.setFloat("plasticity.stdp.enable", False)
        config.setFloat("plasticity.hebbian.enable", False)
        config.setFloat("plasticity.structural.enable", False)
        
    elif optimize_for == "balance":
        # Balanced configuration
        config.setInt("neuron_count", 1000)
        config.setFloat("connection_probability", 0.1)
        config.setFloat("plasticity.stdp.learning_rate", 0.005)
        config.setFloat("plasticity.hebbian.enable", True)
        config.setFloat("plasticity.structural.enable", True)
    
    # Reinitialize brain with new configuration
    brain.reset()
    
    return {
        'optimization_target': optimize_for,
        'neuron_count': config.getInt("neuron_count"),
        'connection_probability': config.getFloat("connection_probability"),
        'stdp_learning_rate': config.getFloat("plasticity.stdp.learning_rate"),
        'hebbian_enabled': config.getBool("plasticity.hebbian.enable"),
        'structural_plasticity_enabled': config.getBool("plasticity.structural.enable"),
        'message': f"Optimized for {optimize_for}"
    }

def batch_simulation_parameter_search(brain_factory, world_factory, agent_factory,
                                      param_space, num_simulations=10):
    """
    Batch simulation for parameter search and optimization.
    
    This function runs multiple simulations with different parameter combinations
    to find optimal settings for a given task.
    
    Args:
        brain_factory (callable): Function that creates a brain with given config
        world_factory (callable): Function that creates a world
        agent_factory (callable): Function that creates an agent
        param_space (list): List of parameter dictionaries to test
        num_simulations (int): Number of simulations to run for each parameter
        
    Returns:
        list: Results for each parameter combination, including performance metrics
    """
    results = []
    
    for i, params in enumerate(param_space):
        print(f"Running parameter combination {i + 1}/{len(param_space)}")
        
        param_results = {
            'parameters': params,
            'simulations': [],
            'average_performance': {},
            'best_performance': None
        }
        
        for sim in range(num_simulations):
            # Create simulation with current parameters
            config = pynlm.createDefaultConfig()
            
            # Apply parameters
            for key, value in params.items():
                if isinstance(value, int):
                    config.setInt(key, value)
                elif isinstance(value, float):
                    config.setFloat(key, value)
                elif isinstance(value, bool):
                    config.setBool(key, value)
                else:
                    config.set(key, str(value))
            
            brain = brain_factory(config)
            world = world_factory()
            agent = agent_factory(brain)
            
            brain.initialize()
            agent.initialize(world)
            
            # Run simulation
            sim_start = time.time()
            for step in range(100):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            sim_time = time.time() - sim_start
            
            # Get performance metrics
            stats = brain.getStats()
            performance = {
                'simulation_time': sim_time,
                'steps_per_second': 100 / sim_time,
                'neuron_count': stats[0],
                'total_spikes': stats[4],
                'avg_firing_rate': stats[3]
            }
            
            param_results['simulations'].append(performance)
        
        # Calculate average performance
        if param_results['simulations']:
            avg_perf = {}
            for key in param_results['simulations'][0].keys():
                values = [sim[key] for sim in param_results['simulations']]
                avg_perf[key] = statistics.mean(values)
            
            param_results['average_performance'] = avg_perf
            
            # Find best performance (highest steps per second)
            if 'steps_per_second' in avg_perf:
                param_results['best_performance'] = max(param_results['simulations'],
                                                      key=lambda x: x['steps_per_second'])
        
        results.append(param_results)
    
    return results

def create_simulation_suite(brain_config, world_config, agent_config):
    """
    Create a suite of pre-configured simulations for testing or comparison.
    
    This function creates multiple simulations with different configurations
    that can be used for testing, comparison, or as starting points for
    custom simulations.
    
    Args:
        brain_config (dict): Configuration parameters for brain
        world_config (dict): Configuration parameters for world
        agent_config (dict): Configuration parameters for agent
        
    Returns:
        dict: Dictionary containing named simulation configurations
    """
    suite = {}
    
    # Simple simulation - minimal configuration
    simple_config = {
        'neuron_count': 500,
        'connection_probability': 0.05,
        'world_width': 20,
        'world_height': 20,
        'enable_curiosity': False,
        'enable_structural_plasticity': False,
    }
    
    # Merge with provided configs
    simple_config.update(brain_config)
    simple_config.update(world_config)
    simple_config.update(agent_config)
    
    suite['simple'] = create_single_simulation(simple_config)
    
    # Complex simulation - maximum configuration
    complex_config = {
        'neuron_count': 3000,
        'connection_probability': 0.15,
        'world_width': 50,
        'world_height': 50,
        'enable_curiosity': True,
        'enable_structural_plasticity': True,
        'enable_development': True,
        'plasticity_stdp_learning_rate': 0.01,
    }
    
    complex_config.update(brain_config)
    complex_config.update(world_config)
    complex_config.update(agent_config)
    
    suite['complex'] = create_single_simulation(complex_config)
    
    # Fast simulation - optimized for speed
    fast_config = {
        'neuron_count': 200,
        'connection_probability': 0.02,
        'world_width': 10,
        'world_height': 10,
        'enable_curiosity': False,
        'enable_structural_plasticity': False,
        'plasticity_stdp_learning_rate': 0.001,
    }
    
    fast_config.update(brain_config)
    fast_config.update(world_config)
    fast_config.update(agent_config)
    
    suite['fast'] = create_single_simulation(fast_config)
    
    return suite

def create_single_simulation(config):
    """
    Create a single simulation from configuration dictionary.
    
    Args:
        config (dict): Configuration parameters
        
    Returns:
        dict: Dictionary containing brain, world, agent, and other components
    """
    # Create configuration object
    sim_config = pynlm.createDefaultConfig()
    
    # Apply all configuration parameters
    if 'neuron_count' in config:
        sim_config.setInt("neuron_count", config['neuron_count'])
    if 'connection_probability' in config:
        sim_config.setFloat("connection_probability", config['connection_probability'])
    if 'plasticity_stdp_learning_rate' in config:
        sim_config.setFloat("plasticity.stdp.learning_rate", config['plasticity_stdp_learning_rate'])
    if 'enable_curiosity' in config:
        sim_config.setBool("neuromod.curiosity.enable", config['enable_curiosity'])
    if 'enable_structural_plasticity' in config:
        sim_config.setBool("plasticity.structural.enable", config['enable_structural_plasticity'])
    if 'enable_development' in config:
        # This would need to be handled differently as it's an agent setting
        pass
    
    # Create components
    brain = pynlm.createBrain(sim_config)
    world = pynlm.createSimpleWorld()
    
    if 'world_width' in config and 'world_height' in config:
        world.configure(
            config['world_width'],
            config['world_height'],
            config.get('world_vision_width', 8),
            config.get('world_vision_height', 8)
        )
    
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize
    brain.initialize()
    agent.initialize(world)
    
    # Apply agent configuration
    if config.get('enable_curiosity', False):
        agent.enableCuriosity(True)
    if config.get('enable_structural_plasticity', False):
        agent.enableStructuralPlasticity(True)
    if config.get('enable_development', False):
        agent.enableDevelopment(True)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'config': config
    }

def get_system_resources():
    """
    Get information about system resources for optimization decisions.
    
    Returns:
        dict: System resource information including available memory,
              CPU cores, and recommendations for configuration
    """
    import psutil
    
    # Get system information
    memory = psutil.virtual_memory()
    cpu_count = psutil.cpu_count()
    cpu_count_logical = psutil.cpu_count(logical=True)
    
    # Estimate neuron count based on available memory
    # Each neuron estimated to use ~1KB, each synapse ~100 bytes
    available_memory_mb = memory.available / (1024 * 1024)
    estimated_max_neurons = (available_memory_mb * 1024 * 1024) / 1024  # / 1KB per neuron
    estimated_max_synapses = estimated_max_neurons * 0.1  # 10% of neurons as synapses
    
    # Recommendations
    recommendations = []
    if available_memory_mb < 1024:
        recommendations.append("Low memory detected - use simple configuration")
    elif available_memory_mb < 4096:
        recommendations.append("Moderate memory - use standard configuration")
    else:
        recommendations.append("High memory - use complex configuration")
    
    if cpu_count_logical < 2:
        recommendations.append("Low CPU - optimize for single-threaded performance")
    else:
        recommendations.append("Good CPU - can use multi-threaded optimizations")
    
    return {
        'total_memory_mb': memory.total / (1024 * 1024),
        'available_memory_mb': available_memory_mb,
        'cpu_count': cpu_count,
        'cpu_count_logical': cpu_count_logical,
        'memory_percent_used': memory.percent,
        'estimated_max_neurons': estimated_max_neurons,
        'estimated_max_synapses': estimated_max_synapses,
        'recommendations': recommendations
    }

class SimulationProfiler:
    """
    Profiler for NLM simulations to analyze performance and resource usage.
    
    This class provides methods to profile simulations and collect detailed
    performance metrics for optimization.
    """
    
    def __init__(self):
        self.profiles = []
    
    def profile_simulation(self, brain, world, agent, name="Unnamed Simulation"):
        """
        Profile a single simulation.
        
        Args:
            brain (Brain): NLM brain object
            world (SimpleWorld): World object
            agent (AgentBrain): Agent brain interface
            name (str): Name for this profile
            
        Returns:
            dict: Profile data including timing, memory, and neural metrics
        """
        import time
        
        profile = {
            'name': name,
            'start_time': time.time(),
            'brain_stats_start': brain.getStats(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
        }
        
        # Run simulation
        for step in range(100):
            step_start = time.time()
            
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            step_time = time.time() - step_start
            profile.setdefault('step_times', []).append(step_time)
        
        profile['end_time'] = time.time()
        profile['brain_stats_end'] = brain.getStats()
        profile['total_time'] = profile['end_time'] - profile['start_time']
        
        # Calculate derived metrics
        if 'step_times' in profile and profile['step_times']:
            profile['avg_step_time_ms'] = statistics.mean(profile['step_times']) * 1000
            profile['min_step_time_ms'] = min(profile['step_times']) * 1000
            profile['max_step_time_ms'] = max(profile['step_times']) * 1000
            profile['step_time_std'] = statistics.stdev(profile['step_times']) * 1000
            profile['steps_per_second'] = len(profile['step_times']) / profile['total_time']
        
        self.profiles.append(profile)
        return profile
    
    def get_summary_stats(self):
        """
        Get summary statistics across all profiled simulations.
        
        Returns:
            dict: Summary statistics
        """
        if not self.profiles:
            return {}
        
        summary = {
            'total_simulations': len(self.profiles),
            'total_steps': sum(len(p.get('step_times', [])) for p in self.profiles),
            'average_steps_per_second': statistics.mean(
                p.get('steps_per_second', 0) for p in self.profiles
            ),
            'average_step_time_ms': statistics.mean(
                p.get('avg_step_time_ms', 0) for p in self.profiles
            ),
            'total_neurons': sum(p.get('neuron_count', 0) for p in self.profiles),
            'total_synapses': sum(p.get('synapse_count', 0) for p in self.profiles),
        }
        
        return summary
    
    def export_profiles(self, filename):
        """
        Export profiles to a file.
        
        Args:
            filename (str): File to export to
        """
        import json
        
        # Convert profiles to JSON-serializable format
        export_data = []
        for profile in self.profiles:
            export_profile = profile.copy()
            # Remove non-serializable objects
            export_profile.pop('step_times', None)
            export_data.append(export_profile)
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)

# Global profiler instance for convenience
profiler = SimulationProfiler()

# Convenience functions using the global profiler
def profile_simulation(name="Simulation"):
    """
    Convenience function to profile a simulation using the global profiler.
    
    This function expects that brain, world, and agent variables are defined
    in the global scope or as global variables in the calling module.
    
    Args:
        name (str): Name for this profile
        
    Returns:
        dict: Profile data
    """
    # This is a convenience function - users need to ensure
    # brain, world, and agent are available in scope
    global brain, world, agent
    
    if 'brain' not in globals() or 'world' not in globals() or 'agent' not in globals():
        raise ValueError("Global variables 'brain', 'world', and 'agent' must be defined")
    
    return profiler.profile_simulation(brain, world, agent, name)

__all__ = [
    'benchmark_simulation',
    'optimize_for_reality',
    'batch_simulation_parameter_search',
    'create_simulation_suite',
    'create_single_simulation',
    'get_system_resources',
    'SimulationProfiler',
    'profiler',
    'profile_simulation',
]

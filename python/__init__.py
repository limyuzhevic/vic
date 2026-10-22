# NLM Python Package - Main entry point

# Import the core pynlm module from C++ bindings
from pynlm import (
    # Basic creation functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    
    # Data types
    NeuronId,
    SynapseId,
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Configuration and values
    Config,
    
    # Sensory input types
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    
    # Motor and action
    Action,
    AgentBody,
    ActionResult,
    MotorCommand,
    
    # Agent components
    AgentBrain,
    SensoryPercept,
    WorldObject,
    
    # Core systems
    Brain,
    SimpleWorld,
    
    # Constants
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)

# Import convenience utilities
try:
    from . import convenience
    
    # Re-export convenience functions
    from .convenience import (
        create_standard_agent,
        run_agent_simulation,
        create_training_episode,
        create_behavioral_experiment,
        create_visualization_data,
        load_experiment_config,
        save_experiment_results,
    )
    
    # Create convenient aliases
    agent_factory = create_standard_agent
    run_simulation = run_agent_simulation
    create_episode = create_training_episode
    create_experiment = create_behavioral_experiment
    collect_data = create_visualization_data
    
except ImportError:
    # Fallback: create convenience functions in Python if C++ bindings not available
    from typing import Optional, Callable, List, Dict, Any
    
    def create_standard_agent(config: Optional[Config] = None, 
                              world_size: tuple = (20, 20),
                              vision_size: tuple = (8, 8)):
        """Create a standard agent setup for most use cases."""
        if config is None:
            config = createDefaultConfig()
        
        config.set("neuron_count", 2000)
        config.set("region_count", 2)
        config.set("connection_probability", 0.05)
        
        brain = createBrain(config)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(
            width=world_size[0],
            height=world_size[1],
            visionWidth=vision_size[0],
            visionHeight=vision_size[1]
        )
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        return brain, agent, world
    
    def run_agent_simulation(world, agent, brain, steps: int = 1000,
                            verbose: bool = False,
                            callback: Optional[Callable] = None):
        """Run a complete agent simulation with the given components."""
        stats = {
            'total_spikes': 0,
            'max_firing_rate': 0.0,
            'min_firing_rate': float('inf'),
            'average_firing_rate': 0.0,
            'episodes': 0,
            'curiosity_levels': [],
            'novelty_levels': [],
            'prediction_errors': []
        }
        
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            
            world.applyMotorCommand(action, world.getSimulationTime())
            
            stats['total_spikes'] += brain.getFiringNeuronCount()
            firing_rate = brain.getAverageFiringRate()
            stats['max_firing_rate'] = max(stats['max_firing_rate'], firing_rate)
            stats['min_firing_rate'] = min(stats['min_firing_rate'], firing_rate)
            
            stats['curiosity_levels'].append(agent.getCuriosityLevel())
            stats['novelty_levels'].append(agent.getNoveltyLevel())
            stats['prediction_errors'].append(agent.getPredictionError())
            
            if step % 100 == 0:
                stats['episodes'] += 1
            
            if callback and step % 100 == 0:
                callback(step, stats)
            
            if verbose and step % 100 == 0:
                print(f"Step {step}: "
                      f"Firing rate: {firing_rate:.2f}, "
                      f"Curiosity: {agent.getCuriosityLevel():.2f}, "
                      f"Novelty: {agent.getNoveltyLevel():.2f}, "
                      f"Pred Error: {agent.getPredictionError():.2f}")
        
        num_steps = steps
        stats['average_firing_rate'] = stats['total_spikes'] / num_steps if num_steps > 0 else 0.0
        if stats['min_firing_rate'] == float('inf'):
            stats['min_firing_rate'] = 0.0
        
        return stats
    
    def create_training_episode(brain, agent, world, max_steps: int = 1000,
                               curiosity_enabled: bool = True,
                               reward_modulation: bool = True):
        """Create and run a single training episode."""
        agent.enableCuriosity(curiosity_enabled)
        agent.enableRewardModulation(reward_modulation)
        
        brain.reset()
        agent.reset()
        world.reset()
        
        brain.initialize()
        agent.initialize(world)
        
        episode_stats = {
            'steps': 0,
            'total_spikes': 0,
            'max_curiosity': 0.0,
            'max_novelty': 0.0,
            'total_reward': 0.0,
            'learning_events': 0,
            'successful_actions': 0
        }
        
        for step in range(max_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            
            world.applyMotorCommand(action, world.getSimulationTime())
            
            agent.updateDevelopment(0.1)
            
            reward = 0.0
            if action != MotorCommand.Wait:
                reward = 0.1
                episode_stats['successful_actions'] += 1
            
            agent.applyRewardModulation(reward, agent.getPredictionError())
            
            episode_stats['steps'] += 1
            episode_stats['total_spikes'] += brain.getFiringNeuronCount()
            episode_stats['max_curiosity'] = max(episode_stats['max_curiosity'], 
                                                agent.getCuriosityLevel())
            episode_stats['max_novelty'] = max(episode_stats['max_novelty'], 
                                              agent.getNoveltyLevel())
            episode_stats['total_reward'] += reward
            
            if (agent.getCuriosityLevel() > 0.7 or 
                agent.getNoveltyLevel() > 0.7 or
                abs(agent.getPredictionError()) > 0.5):
                episode_stats['learning_events'] += 1
        
        return episode_stats
    
    def create_behavioral_experiment(brain, world, agent, trials: int = 10,
                                    steps_per_trial: int = 100):
        """Run a behavioral experiment with consistent conditions."""
        results = {
            'trials': [],
            'overall_success_rate': 0.0,
            'overall_average_movement': 0.0,
            'trial_count': trials
        }
        
        for trial in range(trials):
            trial_stats = {
                'trial': trial + 1,
                'steps_taken': 0,
                'successful_actions': 0,
                'total_rewards': 0.0,
                'final_position': (0, 0),
                'learning_moments': 0
            }
            
            brain.reset()
            agent.reset()
            world.reset()
            brain.initialize()
            agent.initialize(world)
            
            for step in range(steps_per_trial):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                
                agent.processSensoryInput(percept)
                
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                trial_stats['steps_taken'] += 1
                if action != MotorCommand.Wait:
                    trial_stats['successful_actions'] += 1
                    trial_stats['total_rewards'] += 0.1
                
                if (agent.getCuriosityLevel() > 0.5 or 
                    agent.getNoveltyLevel() > 0.5):
                    trial_stats['learning_moments'] += 1
            
            trial_stats['success_rate'] = (
                trial_stats['successful_actions'] / trial_stats['steps_taken']
                if trial_stats['steps_taken'] > 0 else 0.0
            )
            
            results['trials'].append(trial_stats)
        
        total_successes = sum(t['successful_actions'] for t in results['trials'])
        total_steps = sum(t['steps_taken'] for t in results['trials'])
        results['overall_success_rate'] = (
            total_successes / total_steps if total_steps > 0 else 0.0
        )
        
        results['overall_average_movement'] = 0.0  # Simplified
        
        return results
    
    def create_visualization_data(brain, agent, world, duration: float = 10.0,
                                 interval: float = 0.1):
        """Collect visualization data for brain activity and agent behavior."""
        num_samples = int(duration / interval)
        
        visualization_data = {
            'time': [],
            'firing_rate': [],
            'active_neurons': [],
            'curiosity_level': [],
            'novelty_level': [],
            'prediction_error': [],
            'dopamine_level': [],
            'world_position': [],
            'world_energy': []
        }
        
        for i in range(num_samples):
            current_time = i * interval
            
            world.update(interval)
            
            percept = world.getSensoryPercept()
            
            agent.processSensoryInput(percept)
            
            brain.step(i)
            
            action = agent.decodeMotorCommand()
            
            visualization_data['time'].append(current_time)
            visualization_data['firing_rate'].append(brain.getAverageFiringRate())
            visualization_data['active_neurons'].append(brain.getActiveNeuronCount())
            visualization_data['curiosity_level'].append(agent.getCuriosityLevel())
            visualization_data['novelty_level'].append(agent.getNoveltyLevel())
            visualization_data['prediction_error'].append(agent.getPredictionError())
            visualization_data['dopamine_level'].append(agent.getNeuromodulationLevel())
            
            body = world.getAgentBody()
            visualization_data['world_position'].append((body.x, body.y))
            visualization_data['world_energy'].append(body.energy)
            
            agent.updateDevelopment(interval)
        
        return visualization_data

    def load_experiment_config(filepath: str) -> Config:
        """Load experiment configuration from a JSON file."""
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        
        config = createDefaultConfig()
        
        config_mapping = {
            'neuron_count': lambda v: config.set("neuron_count", v),
            'region_count': lambda v: config.set("region_count", v),
            'connection_probability': lambda v: config.set("connection_probability", v),
            'simulation_timestep': lambda v: config.set("simulation_timestep", v),
            'random_seed': lambda v: config.set("random_seed", v),
            'stdp_ltp_weight': lambda v: config.set("stdp_ltp_weight", v),
            'stdp_ltd_weight': lambda v: config.set("stdp_ltd_weight", v),
            'stdp_tau': lambda v: config.set("stdp_tau", v),
            'synaptogenesis_rate': lambda v: config.set("synaptogenesis_rate", v),
            'pruning_rate': lambda v: config.set("pruning_rate", v),
            'replay_interval': lambda v: config.set("replay_interval", v),
            'consolidation_interval': lambda v: config.set("consolidation_interval", v)
        }
        
        for key, setter in config_mapping.items():
            if key in config_data:
                setter(config_data[key])
        
        return config

    def save_experiment_results(results: dict, filepath: str):
        """Save experiment results to a JSON file."""
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"Results saved to {filepath}")

# High-level convenience functions
__all__ = [
    'createDefaultConfig',
    'createBrain',
    'createSimpleWorld',
    'createAgentBrain',
    'NeuronId',
    'SynapseId',
    'RegionId',
    'PopulationId',
    'NeuronType',
    'SynapseType',
    'DevelopmentalStage',
    'FiringState',
    'ActionType',
    'MotorCommand',
    'WorldObjectType',
    'Config',
    'SensoryInput',
    'Vision',
    'Audio',
    'InternalSignals',
    'Action',
    'AgentBody',
    'ActionResult',
    'AgentBrain',
    'SensoryPercept',
    'WorldObject',
    'Brain',
    'SimpleWorld',
    'INVALID_NEURON_ID',
    'INVALID_SYNAPSE_ID',
    'INVALID_REGION_ID',
    'INVALID_POPULATION_ID',
    'create_standard_agent',
    'run_agent_simulation',
    'create_training_episode',
    'create_behavioral_experiment',
    'create_visualization_data',
    'load_experiment_config',
    'save_experiment_results',
]

# Create convenient aliases
agent_factory = create_standard_agent
run_simulation = run_agent_simulation
create_episode = create_training_episode
create_experiment = create_behavioral_experiment
collect_data = create_visualization_data

# Version info
__version__ = "0.1.0"
__author__ = "NLM Project"
__description__ = "Neural Learning Machine (NLM) - Python bindings for neural simulation"
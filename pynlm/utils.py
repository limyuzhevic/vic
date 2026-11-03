"""
Python utility functions for NLM (Neural Learning Machine)

This module provides high-level convenience functions and utilities
for working with the NLM neural simulation framework.
"""

from .pynlm_core import (
    Config, createDefaultConfig, Brain, createBrain,
    SimpleWorld, createSimpleWorld, AgentBrain, createAgentBrain,
    Vision, Audio, InternalSignals, SensoryPercept,
    Action, ActionResult, WorldObject,
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
    INVALID_NEURON_ID, INVALID_SYNAPSE_ID, INVALID_REGION_ID,
    INVALID_POPULATION_ID,
)
class NLMError(Exception):
    """Base exception for NLM errors."""
class ConfigurationError(NLMError):
    """Error in configuration."""
class SimulationError(NLMError):
    """Error in simulation."""
class BrainError(NLMError):
    """Error in brain operations."""
def load_config_from_dict(config_dict):
    """
    Create a Config object from a dictionary.
    
    This provides a convenient way to create configurations
    from Python dictionaries instead of JSON files.
    
    Args:
        config_dict: Dictionary containing configuration key-value pairs
        
    Returns:
        Config: Configured Config object
        
    Example:
        >>> config = load_config_from_dict({
        ...     'brain.neuron_count': 2000,
        ...     'brain.synapse_density': 0.2,
        ...     'plasticity.stdp.enable': True
        ... })
    """
    config = createDefaultConfig()
    for key, value in config_dict.items():
        if isinstance(value, dict):
            # Handle nested dictionaries by converting to JSON string
            import json
            config.set(key, json.dumps(value))
        else:
            config.set(key, value)
    return config
def create_config_from_brain_params(
    neuron_count=1000,
    synapse_density=0.1,
    connection_probability=0.05,
    initial_weight_mean=0.5,
    initial_weight_std=0.1,
    v_thresh=-50.0,
    v_rest=-70.0,
    v_reset=-75.0,
    tau_mem=20.0,
    tau_ref=2.0,
):
    """
    Create a configured brain configuration with common parameters.
    
    This provides a convenient way to create brain configurations
    with typical parameter values for research purposes.
    
    Args:
        neuron_count: Total number of neurons
        synapse_density: Synapse connectivity density (0-1)
        connection_probability: Probability of connection between neurons (0-1)
        initial_weight_mean: Mean initial synaptic weight
        initial_weight_std: Standard deviation of initial weights
        v_thresh: Neuron threshold potential (mV)
        v_rest: Resting potential (mV)
        v_reset: Reset potential after spike (mV)
        tau_mem: Membrane time constant (ms)
        tau_ref: Refractory period (ms)
        
    Returns:
        Config: Configured Config object with brain parameters set
        
    Example:
        >>> config = create_config_from_brain_params(
        ...     neuron_count=2000,
        ...     synapse_density=0.2,
        ...     connection_probability=0.1
        ... )
    """
    config = createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", synapse_density)
    config.set("brain.connection_probability", connection_probability)
    config.set("brain.initial_weight_mean", initial_weight_mean)
    config.set("brain.initial_weight_std", initial_weight_std)
    config.set("brain.v_thresh", v_thresh)
    config.set("brain.v_rest", v_rest)
    config.set("brain.v_reset", v_reset)
    config.set("brain.tau_mem", tau_mem)
    config.set("brain.tau_ref", tau_ref)
    return config
def setup_plasticity_config(
    stdp_enable=True,
    stdp_learning_rate=0.001,
    stdp_tau_plus=20.0,
    stdp_tau_minus=20.0,
    hebbian_enable=True,
    structural_enable=True,
):
    """
    Create a configuration for plasticity mechanisms.
    
    This provides a convenient way to configure various plasticity
    mechanisms with typical parameter values.
    
    Args:
        stdp_enable: Enable Spike-Timing-Dependent Plasticity
        stdp_learning_rate: STDP learning rate
        stdp_tau_plus: STDP time constant for potentiation (ms)
        stdp_tau_minus: STDP time constant for depression (ms)
        hebbian_enable: Enable Hebbian learning
        structural_enable: Enable structural plasticity
        
    Returns:
        Config: Configured Config object with plasticity parameters
        
    Example:
        >>> config = createDefaultConfig()
        >>> plasticity_config = setup_plasticity_config(
        ...     stdp_enable=True,
        ...     stdp_learning_rate=0.001,
        ...     hebbian_enable=True
        ... )
        >>> # Merge plasticity config into main config
        >>> for key in plasticity_config.getKeys():
        ...     config.set(key, plasticity_config.get(key, "") or "")
    """
    config = createDefaultConfig()
    config.set("plasticity.stdp.enable", stdp_enable)
    config.set("plasticity.stdp.learning_rate", stdp_learning_rate)
    config.set("plasticity.stdp.tau_plus", stdp_tau_plus)
    config.set("plasticity.stdp.tau_minus", stdp_tau_minus)
    config.set("plasticity.hebbian.enable", hebbian_enable)
    config.set("plasticity.structural.enable", structural_enable)
    return config
def setup_neuromodulation_config(
    dopamine_scale=1.0,
    curiosity_enable=True,
    novelty_enable=True,
):
    """
    Create a configuration for neuromodulation mechanisms.
    
    This provides a convenient way to configure neuromodulation
    with typical parameter values.
    
    Args:
        dopamine_scale: Dopamine modulation scale
        curiosity_enable: Enable curiosity-driven exploration
        novelty_enable: Enable novelty detection
        
    Returns:
        Config: Configured Config object with neuromodulation parameters
        
    Example:
        >>> neuromod_config = setup_neuromodulation_config(
        ...     curiosity_enable=True,
        ...     novelty_enable=True
        ... )
    """
    config = createDefaultConfig()
    config.set("neuromod.dopamine.scale", dopamine_scale)
    config.set("neuromod.curiosity.enable", curiosity_enable)
    config.set("neuromod.novelty.enable", novelty_enable)
    return config
def create_vision_from_array(
    data,
    channels=3,
    width=None,
    height=None,
):
    """
    Create a Vision sensory input from a flat array.
    
    This provides a convenient way to create Vision objects
    from NumPy arrays or Python lists of pixel data.
    
    Args:
        data: 1D array or list of pixel values
        channels: Number of color channels (1=grayscale, 3=RGB, 4=RGBA)
        width: Image width (calculated if not provided)
        height: Image height (calculated if not provided)
        
    Returns:
        Vision: Configured Vision object with data set
        
    Raises:
        ValueError: If data length doesn't match expected dimensions
        
    Example:
        >>> # Create 8x8 RGB image
        >>> import numpy as np
        >>> pixels = np.random.rand(192).astype(np.float32)  # 8*8*3
        >>> vision = create_vision_from_array(pixels, channels=3, width=8, height=8)
    """
    vision = Vision()
    data_list = list(data)
    
    if width is None or height is None:
        if channels == 1:
            total_pixels = len(data_list)
            if total_pixels == 0:
                raise ValueError("Cannot determine dimensions from empty data")
            height = total_pixels
            width = 1
        elif channels == 3:
            total_pixels = len(data_list)
            if total_pixels == 0:
                raise ValueError("Cannot determine dimensions from empty data")
            height = total_pixels // 3
            width = 3
            if height * 3 != total_pixels:
                raise ValueError(f"Data length {total_pixels} doesn't match RGB format (must be multiple of 3)")
        elif channels == 4:
            total_pixels = len(data_list)
            if total_pixels == 0:
                raise ValueError("Cannot determine dimensions from empty data")
            height = total_pixels // 4
            width = 4
            if height * 4 != total_pixels:
                raise ValueError(f"Data length {total_pixels} doesn't match RGBA format (must be multiple of 4)")
        else:
            raise ValueError(f"Unsupported channels: {channels}")
    
    expected_size = width * height * channels
    if len(data_list) != expected_size:
        raise ValueError(f"Data size {len(data_list)} doesn't match expected size {expected_size} "
                        f"for {width}x{height}x{channels} image")
    
    vision.setData(data_list)
    return vision
def create_audio_from_samples(
    samples,
    sample_rate=44100,
    num_samples=None,
):
    """
    Create an Audio sensory input from audio samples.
    
    This provides a convenient way to create Audio objects
    from NumPy arrays or Python lists of audio samples.
    
    Args:
        samples: 1D array or list of audio samples
        sample_rate: Sampling rate in Hz
        num_samples: Number of samples (calculated if not provided)
        
    Returns:
        Audio: Configured Audio object with samples set
        
    Raises:
        ValueError: If samples length doesn't match expected dimensions
        
    Example:
        >>> # Create 1-second audio at 44.1kHz
        >>> import numpy as np
        >>> audio_data = np.random.randn(44100).astype(np.float32)
        >>> audio = create_audio_from_samples(audio_data, sample_rate=44100)
    """
    audio = Audio()
    samples_list = list(samples)
    
    if num_samples is None:
        num_samples = len(samples_list)
    elif num_samples != len(samples_list):
        raise ValueError(f"Samples length {len(samples_list)} doesn't match num_samples {num_samples}")
    
    audio.setData(samples_list)
    audio.setSampleRate(sample_rate)
    return audio
def create_agent_episode(
    brain,
    world,
    agent,
    max_steps=1000,
    render=False,
    log_interval=100,
):
    """
    Run a complete agent episode in a world.
    
    This provides a high-level interface for running agent
    episodes with optional logging and visualization.
    
    Args:
        brain: Brain object
        world: SimpleWorld object
        agent: AgentBrain object
        max_steps: Maximum number of simulation steps
        render: If True, collect frame data for potential visualization
        log_interval: Log statistics every this many steps
        
    Returns:
        dict: Episode statistics including steps taken, final positions, etc.
        
    Example:
        >>> config = createDefaultConfig()
        >>> brain = createBrain(config)
        >>> brain.initialize()
        >>> world = createSimpleWorld()
        >>> world.configure(20, 20, 8, 8)
        >>> agent = createAgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> stats = create_agent_episode(brain, world, agent, max_steps=500)
        >>> print(f"Episode completed in {stats['steps']} steps")
        >>> print(f"Final position: {stats['final_x']:.1f}, {stats['final_y']:.1f}")
    """
    if not hasattr(agent, 'initialize') or not hasattr(world, 'update'):
        raise ConfigurationError("Invalid brain/world/agent setup")
    
    agent.initialize(world)
    world.reset()
    
    episode_stats = {
        'steps': 0,
        'total_reward': 0.0,
        'max_firing_rate': 0.0,
        'frames': [],
        'final_x': world.getAgentBody().x,
        'final_y': world.getAgentBody().y,
        'initial_x': world.getAgentBody().x,
        'initial_y': world.getAgentBody().y,
    }
    
    for step in range(max_steps):
        try:
            world.update(0.1)
            
            if render:
                episode_stats['frames'].append({
                    'step': step,
                    'world': world,
                    'agent_body': world.getAgentBody(),
                    'perception': world.getSensoryPercept(),
                })
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward from internal signals
            internal = percept.getInternal() if percept else []
            reward = internal[0] if internal else 0.0
            episode_stats['total_reward'] += reward
            
            episode_stats['max_firing_rate'] = max(
                episode_stats['max_firing_rate'],
                brain.getAverageFiringRate()
            )
            
            if log_interval > 0 and step % log_interval == 0:
                print(f"Step {step}: "
                      f"x={world.getAgentBody().x:.1f}, y={world.getAgentBody().y:.1f}, "
                      f"reward={reward:.3f}, firing={brain.getFiringNeuronCount()}")
            
        except Exception as e:
            raise SimulationError(f"Error at step {step}: {e}")
        
        episode_stats['steps'] = step + 1
        episode_stats['final_x'] = world.getAgentBody().x
        episode_stats['final_y'] = world.getAgentBody().y
    
    return episode_stats
def train_agent(
    brain,
    world,
    agent,
    num_episodes=10,
    steps_per_episode=100,
    enable_learning=True,
):
    """
    Train an agent through multiple episodes.
    
    This provides a high-level interface for training agents
    over multiple episodes with learning enabled.
    
    Args:
        brain: Brain object
        world: SimpleWorld object
        agent: AgentBrain object
        num_episodes: Number of training episodes
        steps_per_episode: Steps per episode
        enable_learning: Whether to enable learning mechanisms
        
    Returns:
        dict: Training statistics including episode statistics
        
    Example:
        >>> config = createDefaultConfig()
        >>> brain = createBrain(config)
        >>> brain.initialize()
        >>> world = createSimpleWorld()
        >>> world.configure(20, 20, 8, 8)
        >>> agent = createAgentBrain(brain)
        >>> 
        >>> # Enable learning
        >>> agent.enableRewardModulation(True)
        >>> agent.enableCuriosity(True)
        >>> 
        >>> # Train for 20 episodes
        >>> training_stats = train_agent(
        ...     brain, world, agent,
        ...     num_episodes=20,
        ...     steps_per_episode=200
        ... )
    """
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
    
    training_stats = {
        'episode_stats': [],
        'total_episodes': num_episodes,
        'final_brain_stats': None,
        'learning_enabled': enable_learning,
    }
    
    for episode in range(num_episodes):
        episode_stats = create_agent_episode(
            brain, world, agent, max_steps=steps_per_episode
        )
        
        episode_stats['episode'] = episode
        training_stats['episode_stats'].append(episode_stats)
        
        print(f"Episode {episode + 1}/{num_episodes}: "
              f"Steps={episode_stats['steps']}, "
              f"Total Reward={episode_stats['total_reward']:.1f}, "
              f"Final Position=({episode_stats['final_x']:.1f}, {episode_stats['final_y']:.1f})")
    
    training_stats['final_brain_stats'] = {
        'neurons': brain.getTotalNeuronCount(),
        'spikes': brain.getTotalSpikeCount(),
        'firing': brain.getFiringNeuronCount(),
        'rate': brain.getAverageFiringRate(),
        'stage': brain.getDevelopmentalStage(),
    }
    
    return training_stats
def save_brain_state(
    brain,
    filepath,
    additional_metadata=None,
):
    """
    Save brain state with optional metadata.
    
    This provides an enhanced save function that includes
    metadata in addition to the brain state.
    
    Args:
        brain: Brain object to save
        filepath: File path to save to
        additional_metadata: Dictionary of additional metadata to save
        
    Returns:
        bool: True if save was successful
        
    Example:
        >>> config = createDefaultConfig()
        >>> brain = createBrain(config)
        >>> brain.initialize()
        >>> 
        >>> metadata = {
        ...     'author': 'Researcher',
        ...     'date': datetime.now().isoformat(),
        ...     'experiment_id': 'exp001',
        ...     'performance': {'spikes': 1000, 'time': 10.0}
        ... }
        >>> save_brain_state(brain, 'brain_checkpoint.pkl', metadata)
    """
    try:
        brain.save(filepath)
        
        if additional_metadata:
            import pickle
            metadata_file = filepath.replace('.bin', '_metadata.pkl')
            
            metadata = {
                'metadata': additional_metadata,
                'brain_info': {
                    'neuron_count': brain.getTotalNeuronCount(),
                    'synapse_count': brain.getTotalSynapseCount(),
                    'save_time': str(datetime.now()) if 'datetime' in sys.modules else 'unknown',
                }
            }
            
            with open(metadata_file, 'wb') as f:
                pickle.dump(metadata, f)
        
        return True
    except Exception as e:
        raise BrainError(f"Failed to save brain state: {e}")
def load_brain_state_with_metadata(
    filepath,
    metadata_file=None,
):
    """
    Load brain state with associated metadata.
    
    This provides a convenience function to load brain states
    along with their metadata files.
    
    Args:
        filepath: File path to brain state file
        metadata_file: Optional separate metadata file path
        
    Returns:
        tuple: (brain, metadata_dict) where brain is the loaded brain
            and metadata is the loaded metadata dict
        
    Example:
        >>> brain, metadata = load_brain_state_with_metadata(
        ...     'brain_checkpoint.pkl',
        ...     'brain_checkpoint_metadata.pkl'
        ... )
        >>> print(f"Loaded brain with {brain.getTotalNeuronCount()} neurons")
        >>> print(f"Experiment ID: {metadata.get('metadata', {}).get('experiment_id', 'unknown')}")
    """
    try:
        brain = createBrain(createDefaultConfig())
        brain.load(filepath)
        
        metadata = {}
        if metadata_file and os.path.exists(metadata_file):
            import pickle
            with open(metadata_file, 'rb') as f:
                metadata = pickle.load(f)
        elif not metadata_file and filepath.endswith('.bin'):
            metadata_file = filepath.replace('.bin', '_metadata.pkl')
            if os.path.exists(metadata_file):
                import pickle
                with open(metadata_file, 'rb') as f:
                    metadata = pickle.load(f)
        
        return brain, metadata
    except Exception as e:
        raise BrainError(f"Failed to load brain state: {e}")

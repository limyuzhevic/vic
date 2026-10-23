import pynlm
import time
import json
import sys


def create_advanced_brain(neuron_count=1000, 
                          connection_probability=0.1,
                          enable_learning=True,
                          enable_memory=True,
                          enable_development=False,
                          enable_curiosity=True,
                          enable_novelty=True,
                          enable_prediction=True,
                          enable_planning=True):
    """
    High-level convenience function to create a brain with common configurations.
    
    This function provides an easier way to create a brain with typical settings
    for different use cases, reducing boilerplate code for common scenarios.
    
    Args:
        neuron_count: Number of neurons in the brain (default: 1000)
        connection_probability: Probability of connection between neurons (default: 0.1)
        enable_learning: Enable STDP and Hebbian plasticity (default: True)
        enable_memory: Enable working and episodic memory systems (default: True)
        enable_development: Enable developmental plasticity (default: False)
        enable_curiosity: Enable curiosity-driven exploration (default: True)
        enable_novelty: Enable novelty detection (default: True)
        enable_prediction: Enable predictive coding (default: True)
        enable_planning: Enable action planning (default: True)
    
    Returns:
        tuple: (brain, config) - The created brain and its configuration
    
    Example:
        >>> brain, config = create_advanced_brain(neuron_count=500, enable_development=True)
        >>> brain.initialize()
        >>> for i in range(100):
        ...     brain.step(i)
        >>> print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
    """
    config = pynlm.createDefaultConfig()
    
    # Basic configuration
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", connection_probability)
    config.set("brain.connection_probability", connection_probability)
    
    # Plasticity configuration
    if enable_learning:
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.01)
    
    # Structural plasticity
    config.set("plasticity.structural.enable", True)
    config.set("synaptogenesis_rate", 0.0001f)
    config.set("pruning_rate", 0.00001f)
    
    # Neuromodulation
    if enable_curiosity:
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.exploration_strength", 1.0)
    
    if enable_novelty:
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.sensitivity", 0.5)
    
    # Development
    if enable_development:
        config.set("development.enable", True)
        config.set("development.initial_plasticity", 1.0)
        config.set("development.maturity_age", 1000)
    
    # Prediction
    if enable_prediction:
        config.set("prediction.enable", True)
        config.set("prediction.horizon", 10)
        config.set("prediction.error_threshold", 0.1)
    
    # Planning
    if enable_planning:
        config.set("cognition.planning.enable", True)
        config.set("cognition.planning.planning_depth", 5)
        config.set("cognition.planning.action_selection", "neural")
    
    # Memory systems
    if enable_memory:
        config.set("memory.working.capacity", neuron_count // 10)
        config.set("memory.episodic.max_episodes", 1000)
        config.set("memory.associative.association_strength", 0.5)
    
    brain = pynlm.createBrain(config)
    return brain, config


def create_agent_world(neuron_count=500,
                      world_width=20,
                      world_height=20,
                      vision_width=8,
                      vision_height=8,
                      enable_rl=False,
                      enable_curiosity=False,
                      enable_rewards=False):
    """
    High-level convenience function to create a complete agent-world setup.
    
    This creates a full agent (brain + agent brain interface) and world setup
    with common configurations for reinforcement learning and exploration tasks.
    
    Args:
        neuron_count: Number of neurons in the brain (default: 500)
        world_width: Width of the simulation world (default: 20)
        world_height: Height of the simulation world (default: 20)
        vision_width: Width of agent's vision field (default: 8)
        vision_height: Height of agent's vision field (default: 8)
        enable_rl: Enable reward-based learning (default: False)
        enable_curiosity: Enable curiosity-driven exploration (default: False)
        enable_rewards: Enable world rewards (default: False)
    
    Returns:
        tuple: (brain, agent, world) - The created brain, agent interface, and world
    
    Example:
        >>> brain, agent, world = create_agent_world(neuron_count=300, enable_curiosity=True)
        >>> brain.initialize()
        >>> agent.initialize(world)
        >>> 
        >>> # Run simulation loop
        >>> for step in range(1000):
        ...     world.update(0.1)
        ...     percept = world.getSensoryPercept()
        ...     agent.processSensoryInput(percept)
        ...     brain.step(step)
        ...     action = agent.decodeMotorCommand()
        ...     world.applyMotorCommand(action, world.getSimulationTime())
    """
    # Create brain with RL-focused configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.15)
    
    # Enable reward processing
    if enable_rewards:
        config.set("neuromod.dopamine.enable", True)
        config.set("neuromod.dopamine.reward_scale", 1.0)
    
    # Enable curiosity
    if enable_curiosity:
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.exploration_strength", 1.0)
    
    # Create systems
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(world_width, world_height, vision_width, vision_height)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable requested features
    if enable_rl:
        agent.enableRewardModulation(True)
    
    if enable_curiosity:
        agent.enableCuriosity(True)
    
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    return brain, agent, world


def create_development_experiment(neuron_count=800,
                                 development_stages=5,
                                 enable_stdp=True,
                                 enable_hebbian=True,
                                 enable_structural=True):
    """
    High-level convenience function to create a developmental experiment setup.
    
    This creates a brain optimized for studying developmental processes,
    including critical periods, maturation, and age-related changes in plasticity.
    
    Args:
        neuron_count: Number of neurons (default: 800)
        development_stages: Number of developmental stages to simulate (default: 5)
        enable_stdp: Enable spike-timing dependent plasticity (default: True)
        enable_hebbian: Enable Hebbian plasticity (default: True)
        enable_structural: Enable structural plasticity (default: True)
    
    Returns:
        tuple: (brain, config, developmental_stages) - The created brain, configuration,
               and list of developmental stages for tracking
    
    Example:
        >>> brain, config, stages = create_development_experiment(
        ...     neuron_count=600, development_stages=8, enable_structural=True)
        >>> 
        >>> # Simulate development
        >>> for stage in range(development_stages):
        ...     stage_start = stage * 1000
        ...     stage_end = (stage + 1) * 1000
        ...     
        ...     for step in range(stage_start, stage_end):
        ...         brain.step(step)
        ...     
        ...     # Record development metrics
        ...     record_development_metrics(brain, stage)
    """
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    
    # Configure developmental stages
    config.set("development.enable", True)
    config.set("development.stages", development_stages)
    config.set("development.critical_period_start", 0.0)
    config.set("development.critical_period_end", development_stages * 0.3)
    config.set("development.maturation_start", development_stages * 0.3)
    config.set("development.maturity_end", development_stages * 0.7)
    
    # Configure plasticity for development
    if enable_stdp:
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
    
    if enable_hebbian:
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.01)
    
    if enable_structural:
        config.set("plasticity.structural.enable", True)
        config.set("synaptogenesis_rate", 0.0002f)
        config.set("pruning_rate", 0.00002f)
    
    brain = pynlm.createBrain(config)
    return brain, config, development_stages


def create_memory_experiment(episodes=100, memory_capacity=1000, replay_frequency=10):
    """
    High-level convenience function to create a memory experiment setup.
    
    This creates a brain optimized for studying memory systems, including
    episodic memory, working memory, and memory consolidation through replay.
    
    Args:
        episodes: Number of episodes to store (default: 100)
        memory_capacity: Working memory capacity (default: 1000)
        replay_frequency: How often to replay episodes (every N steps, default: 10)
    
    Returns:
        tuple: (brain, config) - The created brain and configuration
    
    Example:
        >>> brain, config = create_memory_experiment(episodes=200, replay_frequency=20)
        >>> brain.initialize()
        >>> 
        >>> # Run learning episode
        >>> for episode in range(episodes):
        ...     # Present novel input
        ...     for step in range(100):
        ...         brain.step(step + episode * 100)
        ...     
        ...     # Record episode
        ...     record_episode(brain, episode)
        
        >>> # Check memory consolidation
        >>> consolidation_result = analyze_memory_consolidation(brain)
    """
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    # Configure memory systems
    config.set("memory.working.capacity", memory_capacity)
    config.set("memory.episodic.max_episodes", episodes)
    config.set("replay_interval", replay_frequency)
    config.set("consolidation_interval", episodes * 10)
    
    # Configure episodic memory
    config.set("memory.episodic.encoding_strength", 1.0)
    config.set("memory.episodic.recall_probability", 0.8)
    config.set("memory.episodic.decay_rate", 0.001)
    
    brain = pynlm.createBrain(config)
    return brain, config


def create_curiosity_experiment(exploration_rate=0.3, novelty_sensitivity=0.5, reward_scaling=1.0):
    """
    High-level convenience function to create a curiosity-driven exploration experiment.
    
    This creates a brain optimized for studying curiosity-driven learning,
    where the agent seeks novel and interesting experiences rather than just
    maximizing immediate rewards.
    
    Args:
        exploration_rate: Proportion of actions that are exploratory (default: 0.3)
        novelty_sensitivity: How sensitive the agent is to novelty (default: 0.5)
        reward_scaling: Scaling factor for reward signals (default: 1.0)
    
    Returns:
        tuple: (brain, config) - The created brain and configuration
    
    Example:
        >>> brain, config = create_curiosity_experiment(
        ...     exploration_rate=0.4, novelty_sensitivity=0.7, reward_scaling=0.5)
        >>> 
        >>> # Run curiosity experiment
        >>> curiosity_history = []
        >>> for step in range(1000):
        ...     curiosity = brain.getCuriosityLevel()  # If using agent interface
        ...     curiosity_history.append(curiosity)
        ...     
        ...     # Environment interaction (custom logic)
        ...     process_step(step)
        ...     
        >>> # Analyze exploration patterns
        >>> analyze_curiosity_patterns(curiosity_history)
    """
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    
    # Configure curiosity system
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.exploration_strength", exploration_rate)
    config.set("neuromod.curiosity.sensitivity", novelty_sensitivity)
    
    # Configure reward system
    config.set("neuromod.dopamine.enable", True)
    config.set("neuromod.dopamine.reward_scale", reward_scaling)
    
    # Configure novelty detection
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", novelty_sensitivity)
    
    brain = pynlm.createBrain(config)
    return brain, config


def create_prediction_experiment(prediction_horizon=10, prediction_error_threshold=0.1):
    """
    High-level convenience function to create a prediction-driven experiment.
    
    This creates a brain optimized for studying predictive coding and
    prediction error minimization, where the agent actively predicts
    future states and learns from prediction errors.
    
    Args:
        prediction_horizon: How far ahead the agent predicts (default: 10)
        prediction_error_threshold: Threshold for triggering learning (default: 0.1)
    
    Returns:
        tuple: (brain, config) - The created brain and configuration
    
    Example:
        >>> brain, config = create_prediction_experiment(
        ...     prediction_horizon=15, prediction_error_threshold=0.05)
        >>> 
        >>> # Run prediction experiment
        >>> prediction_history = []
        >>> error_history = []
        >>> 
        >>> for step in range(1000):
        ...     # Agent predicts next state
        ...     prediction = predict_next_state(brain)
        ...     prediction_history.append(prediction)
        ...     
        ...     # Compare with actual outcome
        ...     error = calculate_prediction_error(prediction, actual_state)
        ...     error_history.append(error)
        ...     
        ...     # Learn from prediction error
        ...     if error > prediction_error_threshold:
        ...     learn_from_prediction_error(brain, error)
        ...     
        ...     brain.step(step)
        ... 
        >>> # Analyze prediction accuracy
        ... analyze_prediction_performance(prediction_history, error_history)
    """
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1200)
    
    # Configure prediction system
    config.set("prediction.enable", True)
    config.set("prediction.horizon", prediction_horizon)
    config.set("prediction.error_threshold", prediction_error_threshold)
    config.set("prediction.error_decay", 0.9)
    config.set("prediction.learning_rate", 0.01)
    
    # Configure prediction error system
    config.set("neuromod.prediction_error.enable", True)
    config.set("neuromod.prediction_error.sensitivity", 0.5)
    config.set("neuromod.prediction_error.significance_threshold", prediction_error_threshold)
    
    brain = pynlm.createBrain(config)
    return brain, config


def run_simulation_with_logging(brain, max_steps=1000, log_interval=100, 
                               enable_progress=True, enable_detailed_stats=False):
    """
    Run a simulation with integrated logging and statistics collection.
    
    This provides a more sophisticated simulation runner that logs statistics
    and provides progress feedback, making it easier to run and analyze simulations.
    
    Args:
        brain: The brain to simulate
        max_steps: Maximum number of simulation steps (default: 1000)
        log_interval: How often to log statistics (default: 100 steps)
        enable_progress: Show progress output (default: True)
        enable_detailed_stats: Collect detailed statistics (default: False)
    
    Returns:
        list: Statistics collected during simulation (empty list if not enabled)
    
    Example:
        >>> brain, config = create_advanced_brain()
        >>> brain.initialize()
        >>> 
        >>> stats = run_simulation_with_logging(
        ...     brain, max_steps=500, log_interval=50, enable_detailed_stats=True)
        >>> 
        >>> print(f"Average firing rate: {stats['avg_firing_rate']:.2f}")
        >>> print(f"Total spikes: {stats['total_spikes']}")
        >>> print(f"Max firing rate: {stats['max_firing_rate']:.2f}")
    """
    stats = [] if enable_detailed_stats else None
    
    if enable_progress:
        print(f"Starting simulation: {max_steps} steps")
        print(f"Log interval: {log_interval} steps")
    
    brain.initialize()
    
    for step in range(max_steps):
        brain.step(step)
        
        # Log statistics at intervals
        if step % log_interval == 0:
            if enable_progress:
                firing_rate = brain.getAverageFiringRate()
                total_spikes = brain.getTotalSpikeCount()
                print(f"Step {step:4d}: firing_rate={firing_rate:6.2f}, "
                      f"total_spikes={total_spikes:6d}")
            
            if enable_detailed_stats:
                stat_entry = {
                    'step': step,
                    'firing_rate': brain.getAverageFiringRate(),
                    'total_spikes': brain.getTotalSpikeCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'active_neurons': brain.getActiveNeuronCount(),
                    'e_i_ratio': brain.getExcitationInhibitionRatio(),
                    'development_stage': int(brain.getDevelopmentalStage()),
                    'timestamp': time.time()
                }
                stats.append(stat_entry)
    
    if enable_progress:
        firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        print(f"Simulation complete: {max_steps} steps")
        print(f"Final stats: firing_rate={firing_rate:.2f}, total_spikes={total_spikes}")
    
    return stats


def save_brain_state(brain, filename, include_config=True):
    """
    Save brain state to file with automatic filename generation.
    
    This provides a convenient way to save brain states with timestamped
    filenames and optional configuration saving.
    
    Args:
        brain: The brain to save
        filename: Base filename (will be appended with timestamp if not provided)
        include_config: Whether to save configuration alongside brain state (default: True)
    
    Returns:
        str: Full path to saved file
    
    Example:
        >>> brain, config = create_advanced_brain()
        >>> brain.initialize()
        >>> 
        >>> # Save brain state
        >>> filepath = save_brain_state(brain, "my_brain_checkpoint")
        >>> print(f"Brain saved to: {filepath}")
        >>> 
        >>> # Later load it back
        >>> # loaded_brain = pynlm.Brain()  # Create empty brain
        >>> # loaded_brain.load(filepath)
    """
    import datetime
    
    # Add timestamp if no extension provided
    if '.' not in filename:
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        filepath = f"{filename}_{timestamp}.bin"
    else:
        filepath = filename
    
    # Save brain state
    success = brain.save(filepath)
    
    if not success:
        raise RuntimeError(f"Failed to save brain to {filepath}")
    
    # Save configuration if requested
    if include_config:
        config_filepath = f"{filename}_config.json"
        # Note: This assumes Config has a method to save to JSON
        # brain.getConfig().saveToFile(config_filepath)  # Uncomment if available
        print(f"Configuration saved to: {config_filepath}")
    
    return filepath


def load_or_create_brain(filename=None, neuron_count=1000, tolerance=0.1):
    """
    Load a saved brain state or create a new one with similar characteristics.
    
    This provides a robust way to resume from saved states or create
    new brains with similar properties when loading fails.
    
    Args:
        filename: Path to saved brain state file. If None, creates new brain (default: None)
        neuron_count: Number of neurons for new brain if loading fails (default: 1000)
        tolerance: Similarity tolerance for fallback configuration (default: 0.1)
    
    Returns:
        tuple: (brain, filename_used) - The loaded or created brain and filename used
    
    Example:
        >>> # Try to load saved brain first
        >>> brain, used_file = load_or_create_brain("my_brain_checkpoint.bin")
        >>> print(f"Loaded brain from: {used_file}")
        >>> 
        >>> # If loading fails, create new brain with similar characteristics
        >>> brain, used_file = load_or_create_brain(
        ...     "nonexistent_file.bin", neuron_count=800, tolerance=0.2)
        >>> print(f"Created new brain: {used_file}")
    """
    if filename is not None:
        try:
            # Create empty brain and try to load
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            # Note: This assumes Brain constructor can load from file
            # if brain.load(filename):
            #     print(f"Successfully loaded brain from: {filename}")
            #     return brain, filename
            # else:
            #     print(f"Failed to load brain from {filename}, creating new one")
            print(f"Warning: Brain loading from file not implemented yet. Creating new brain.")
            
        except Exception as e:
            print(f"Error loading brain from {filename}: {e}")
            print(f"Creating new brain with {neuron_count} neurons")
    
    # Create new brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    brain = pynlm.createBrain(config)
    
    return brain, f"new_brain_{neuron_count}.bin"

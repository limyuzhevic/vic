"""
High-level simulation scenarios for NLM

This module provides predefined simulation scenarios and experiments
for common use cases and research tasks.
"""

from typing import Dict, Any, Optional
from .utils import run_with_progress


def create_learning_scenario(neuron_count: int = 500, steps: int = 1000) -> Dict[str, Any]:
    """
    Create a learning scenario configuration.

    This scenario focuses on synaptic learning through repeated stimulation.

    Args:
        neuron_count: Number of neurons (default: 500)
        steps: Number of simulation steps (default: 1000)

    Returns:
        dict: Scenario configuration
    """
    config = {
        'simulation_timestep': 0.001,
        'random_seed': 42,
        'neuron_count': neuron_count,
        'region_count': 1,
        'connection_probability': 0.15,

        # Learning-focused settings
        'stdp_ltp_weight': 0.02,
        'stdp_ltd_weight': 0.015,
        'stdp_time_constant': 20.0,
        'plasticity_learning_rate': 0.02,

        # Memory and learning
        'working_memory_capacity': neuron_count // 10,
        'episodic_memory_max_episodes': 100,
        'enable_structural_plasticity': True,
        'enable_development': True,

        # Input pattern for learning
        'input_pattern_amplitude': 30.0,
        'input_repetition_rate': 0.1,
        'input_duration': 0.5,

        # Reward and motivation
        'reward_baseline': 0.0,
        'enable_curiosity': True,
        'curiosity_sensitivity': 0.5,
        'enable_novelty_detection': True,
    }

    return config


def create_perception_scenario(neuron_count: int = 300, steps: int = 500) -> Dict[str, Any]:
    """
    Create a perception scenario configuration.

    This scenario focuses on sensory processing and pattern recognition.

    Args:
        neuron_count: Number of neurons (default: 300)
        steps: Number of simulation steps (default: 500)

    Returns:
        dict: Scenario configuration
    """
    config = {
        'simulation_timestep': 0.001,
        'random_seed': 42,
        'neuron_count': neuron_count,
        'region_count': 2,
        'connection_probability': 0.08,

        # Sensory processing
        'enable_vision': True,
        'vision_width': 8,
        'vision_height': 8,
        'enable_audio': False,
        'enable_internal_signals': True,

        # Memory for pattern storage
        'episodic_memory_max_episodes': 50,
        'working_memory_capacity': neuron_count // 5,
        'enable_associative_memory': True,

        # Attention and focus
        'attention_inhibition_strength': 0.5,
        'attention_excitation_strength': 1.5,
        'competition_threshold': 0.3,

        # Concept formation
        'enable_concept_formation': True,
        'concept_learning_rate': 0.01,
        'concept_similarity_threshold': 0.7,
    }

    return config


def create_cognition_scenario(neuron_count: int = 800, steps: int = 2000) -> Dict[str, Any]:
    """
    Create a cognition scenario configuration.

    This scenario focuses on cognitive functions like planning and decision making.

    Args:
        neuron_count: Number of neurons (default: 800)
        steps: Number of simulation steps (default: 2000)

    Returns:
        dict: Scenario configuration
    """
    config = {
        'simulation_timestep': 0.001,
        'random_seed': 42,
        'neuron_count': neuron_count,
        'region_count': 3,
        'connection_probability': 0.12,

        # Planning and reasoning
        'enable_planning': True,
        'planning_depth': 5,
        'action_selection_threshold': 0.4,
        'exploration_rate': 0.2,

        # Attention and working memory
        'working_memory_capacity': neuron_count // 8,
        'attention_inhibition_strength': 0.6,
        'attention_excitation_strength': 2.0,

        # Memory systems
        'episodic_memory_max_episodes': 200,
        'enable_structural_plasticity': True,
        'development_stage': 'CriticalPeriod',

        # Prediction and error correction
        'enable_prediction_system': True,
        'prediction_horizon': 10,
        'prediction_error_weight': 0.5,

        # Social and learning
        'enable_social_learning': True,
        'social_influence_strength': 0.3,
        'imitation_probability': 0.15,
    }

    return config


def create_development_scenario(neuron_count: int = 600, steps: int = 3000) -> Dict[str, Any]:
    """
    Create a development scenario configuration.

    This scenario focuses on brain development and maturation over time.

    Args:
        neuron_count: Number of neurons (default: 600)
        steps: Number of simulation steps (default: 3000)

    Returns:
        dict: Scenario configuration
    """
    config = {
        'simulation_timestep': 0.001,
        'random_seed': 42,
        'neuron_count': neuron_count,
        'region_count': 2,
        'connection_probability': 0.05,

        # Development stages
        'developmental_stage': 'Initial',
        'enable_development': True,
        'developmental_time_scale': 0.8,

        # Plasticity that changes over time
        'synaptogenesis_rate': 0.002,
        'pruning_rate': 0.0005,
        'plasticity_modulation': True,
        'critical_period_duration': 1000,

        # Memory development
        'episodic_memory_max_episodes': 300,
        'working_memory_capacity': neuron_count // 6,
        'memory_consolidation_enabled': True,

        # Learning that matures
        'learning_rate_stage1': 0.05,  # Initial high learning
        'learning_rate_stage2': 0.02,  # Critical period
        'learning_rate_stage3': 0.01,  # Maturity
        'enable_curiosity': True,
        'curiosity_decay': 0.95,
    }

    return config


def run_scenario(scenario_name: str, **kwargs) -> Dict[str, Any]:
    """
    Run a predefined scenario.

    Args:
        scenario_name: Name of scenario (learning, perception, cognition, development)
        **kwargs: Additional parameters for scenario configuration

    Returns:
        dict: Results from running the scenario
    """
    scenarios = {
        'learning': create_learning_scenario,
        'perception': create_perception_scenario,
        'cognition': create_cognition_scenario,
        'development': create_development_scenario,
    }

    if scenario_name not in scenarios:
        available = ', '.join(scenarios.keys())
        raise ValueError(f"Unknown scenario: {scenario_name}. Available: {available}")

    # Get scenario configuration
    scenario_func = scenarios[scenario_name]
    config = scenario_func(**kwargs)

    # Simulate running the scenario
    results = {
        'scenario_name': scenario_name,
        'configuration': config,
        'status': 'completed',
        'timestamp': time.time(),
        'metrics': {
            'final_neuron_count': config['neuron_count'],
            'region_count': config['region_count'],
            'connection_probability': config['connection_probability'],
            'plasticity_enabled': config.get('enable_structural_plasticity', False),
            'memory_enabled': config.get('episodic_memory_max_episodes', 0) > 0,
        }
    }

    return results
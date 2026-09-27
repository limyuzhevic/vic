"""
Convenience functions and training utilities for NLM Python API
=================================================================

This module provides convenience functions for common NLM operations,
including brain creation, experiment setup, testing, and training loops.
"""

import numpy as np
from typing import Any, Dict, List, Optional, Union, Callable, Tuple
import json
import warnings
from sklearn.model_selection import ParameterGrid

from . import NLMError, ValidationError, ConfigurationError


try:
    import pynlm
    HAS_PYNLM = True
except ImportError:
    HAS_PYNLM = False


# Global configuration
_global_config = {
    'default_neuron_count': 1000,
    'default_complexity': 0.5,
    'default_world_size': (10, 10),
    'default_steps': 1000,
    'enable_warnings': True,
}


def configure(**kwargs):
    """
    Configure global NLM API settings.
    
    Args:
        **kwargs: Configuration options
            - default_neuron_count: Default neuron count
            - default_complexity: Default complexity (0.0-1.0)
            - default_world_size: Default world size
            - default_steps: Default simulation steps
            - enable_warnings: Whether to show warnings
    """
    _global_config.update(kwargs)


def create_simple_brain(neuron_count: int = None, complexity: float = None,
                       config: Optional[Dict[str, Any]] = None) -> Any:
    """
    Create a simple brain with optimal default settings.
    
    This is a one-line function to create a basic brain for most use cases.
    
    Args:
        neuron_count: Number of neurons (defaults to global config)
        complexity: Brain complexity (0.0-1.0, defaults to global config)
        config: Optional additional configuration
        
    Returns:
        pynlm Brain object or simplified NLMBrain wrapper
        
    Raises:
        ValidationError: If parameters are invalid
        
    Example:
        ```python
        brain = create_simple_brain(neuron_count=500, complexity=0.7)
        brain.initialize()
        ```
    """
    if not HAS_PYNLM:
        raise ImportError("NLM C++ bindings not found. Please build pynlm.")
    
    # Use provided values or fall back to defaults
    if neuron_count is None:
        neuron_count = _global_config.get('default_neuron_count', 1000)
    if complexity is None:
        complexity = _global_config.get('default_complexity', 0.5)
    
    # Validate inputs
    if neuron_count < 1:
        raise ValidationError(f"Neuron count must be positive, got {neuron_count}")
    if not 0.0 <= complexity <= 1.0:
        raise ValidationError(f"Complexity must be in [0.0, 1.0], got {complexity}")
    
    # Create configuration
    cpp_config = pynlm.createDefaultConfig()
    
    # Set up brain parameters based on complexity
    if complexity > 0.7:
        cpp_config.set("brain.excitatory_neurons", int(neuron_count * 0.8))
        cpp_config.set("brain.inhibitory_neurons", int(neuron_count * 0.2))
    elif complexity > 0.3:
        cpp_config.set("brain.excitatory_neurons", int(neuron_count * 0.6))
        cpp_config.set("brain.inhibitory_neurons", int(neuron_count * 0.4))
    else:
        cpp_config.set("brain.excitatory_neurons", int(neuron_count * 0.5))
        cpp_config.set("brain.inhibitory_neurons", int(neuron_count * 0.5))
    
    # Apply user config
    if config:
        for key, value in config.items():
            if isinstance(value, dict):
                for subkey, subvalue in value.items():
                    cpp_config.set(f"{key}.{subkey}", subvalue)
            else:
                cpp_config.set(key, value)
    
    # Create and return brain
    return pynlm.createBrain(cpp_config)


def create_standard_experiment(world_size: tuple = None) -> Any:
    """
    Create a standard experiment setup.
    
    This function creates a complete experiment setup with brain, world,
    and agent using standard parameters.
    
    Args:
        world_size: Tuple of (width, height) for the world 
                       (defaults to global config)
                       
    Returns:
        Dictionary containing 'brain', 'world', and 'agent' keys
        
    Example:
        ```python
        experiment = create_standard_experiment(world_size=(20, 20))
        brain = experiment['brain']
        world = experiment['world']
        agent = experiment['agent']
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        ```
    """
    if not HAS_PYNLM:
        raise ImportError("NLM C++ bindings not found. Please build pynlm.")
    
    # Use provided world size or default
    if world_size is None:
        world_size = _global_config.get('default_world_size', (10, 10))
    
    # Create brain with default settings
    brain = create_simple_brain()
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_size[0],
        height=world_size[1],
        visionWidth=8,
        visionHeight=8
    )
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent
    }


def quick_test(mode: str = 'connectivity', steps: int = None,
              brain: Any = None, config: Optional[Dict] = None) -> Dict[str, Any]:
    """
    Quick testing utility for evaluating NLM functionality.
    
    This function runs a quick test in a specified mode to verify
    that the NLM system is working correctly.
    
    Args:
        mode: Test mode ('connectivity', 'firing', 'learning', 'all')
        steps: Number of simulation steps (defaults to global config)
        brain: Optional pre-created brain object
        config: Optional configuration for brain creation
        
    Returns:
        Dictionary containing test results
        
    Raises:
        ValidationError: If mode or parameters are invalid
        
    Example:
        ```python
        results = quick_test(mode='connectivity', steps=500)
        print(f"Test passed: {results['passed']}")
        ```
    """
    if not HAS_PYNLM:
        raise ImportError("NLM C++ bindings not found. Please build pynlm.")
    
    if mode not in ['connectivity', 'firing', 'learning', 'all']:
        raise ValidationError(f"Invalid test mode: {mode}. Must be one of: "
                            "'connectivity', 'firing', 'learning', 'all'")
    
    if steps is None:
        steps = _global_config.get('default_steps', 1000)
    
    if steps < 1:
        raise ValidationError(f"Steps must be positive, got {steps}")
    
    # Create brain if not provided
    if brain is None:
        brain = create_simple_brain(config=config)
        brain.initialize()
    
    # Initialize results tracking
    results = {
        'passed': True,
        'mode': mode,
        'steps': steps,
        'metrics': {},
        'errors': [],
        'timestamp': None  # Would be time.time() in real implementation
    }
    
    try:
        # Run test based on mode
        if mode in ['connectivity', 'all']:
            # Test connectivity - check neuron activation
            initial_active = brain.getActiveNeuronCount()
            
            for step in range(steps):
                brain.step(step)
                active = brain.getActiveNeuronCount()
                
                if active == 0 and step > 100:  # Allow some warmup
                    results['errors'].append(f"No active neurons at step {step}")
                    results['passed'] = False
                    break
                
                if mode == 'all' or mode == 'connectivity':
                    results['metrics']['active_neurons_max'] = max(
                        results['metrics'].get('active_neurons_max', 0), active)
            
            results['metrics']['final_active_neurons'] = brain.getActiveNeuronCount()
            results['metrics']['total_spikes'] = brain.getTotalSpikeCount()
        
        if mode in ['firing', 'all']:
            # Test firing rates
            firing_rates = []
            
            for step in range(steps):
                brain.step(step)
                firing_rate = brain.getAverageFiringRate()
                firing_rates.append(firing_rate)
                
                # Check for reasonable firing rate (not too high or low)
                if firing_rate > 100 and step > 50:
                    results['errors'].append(f"Excessive firing rate: {firing_rate} at step {step}")
                    results['passed'] = False
                    break
            
            if firing_rates:
                results['metrics']['firing_rate_mean'] = np.mean(firing_rates)
                results['metrics']['firing_rate_std'] = np.std(firing_rates)
                results['metrics']['firing_rate_min'] = np.min(firing_rates)
                results['metrics']['firing_rate_max'] = np.max(firing_rates)
        
        if mode in ['learning', 'all']:
            # Test learning - check development stage changes
            initial_stage = brain.getDevelopmentalStage()
            
            for step in range(steps):
                brain.step(step)
                
                if step % 100 == 0:  # Check every 100 steps
                    current_stage = brain.getDevelopmentalStage()
                    # Check for reasonable progression
                    if current_stage not in [initial_stage, 'Adult']:
                        results['errors'].append(f"Unexpected developmental stage: {current_stage} at step {step}")
                        results['passed'] = False
                        break
        
        # Summarize results
        results['success_rate'] = results['passed']
        results['total_errors'] = len(results['errors'])
        
    except Exception as e:
        results['passed'] = False
        results['errors'].append(f"Test execution error: {str(e)}")
        results['exception_type'] = type(e).__name__
    
    return results


def train_agent(world: Any, brain: Any, episodes: int,
               visualizer: Any = None,
               reward_function: Optional[Callable] = None,
               stop_criteria: Optional[Dict] = None) -> Dict[str, Any]:
    """
    High-level training loop for NLM agents.
    
    This function provides a complete training loop for agents in a world,
    including reward tracking, progress visualization, and early stopping.
    
    Args:
        world: World object
        brain: Brain object
        episodes: Number of training episodes
        visualizer: Optional visualizer for progress tracking
        reward_function: Optional function to calculate reward
        stop_criteria: Optional early stopping criteria
        
    Returns:
        Dictionary containing training results
        
    Raises:
        ValidationError: If parameters are invalid
        
    Example:
        ```python
        results = train_agent(world, brain, episodes=100)
        print(f"Training completed: {results['episodes_completed']} episodes")
        ```
    """
    # Validate inputs
    if episodes < 1:
        raise ValidationError(f"Episodes must be positive, got {episodes}")
    
    # Initialize training
    training_results = {
        'episodes_completed': 0,
        'total_reward': 0.0,
        'best_performance': 0.0,
        'episode_rewards': [],
        'episode_performances': [],
        'stop_reason': 'completed',
        'average_reward': 0.0,
        'average_performance': 0.0,
        'training_time': None  # Would be timing in real implementation
    }
    
    # Setup stop criteria if provided
    default_stop_criteria = {
        'max_episodes': episodes,
        'min_performance': 0.8,
        'patience': 20,
        'improvement_threshold': 0.01
    }
    
    if stop_criteria:
        default_stop_criteria.update(stop_criteria)
    
    stop_criteria = default_stop_criteria
    
    # Initialize episode tracking
    best_performance = 0.0
    no_improvement_count = 0
    
    # Initialize world if needed
    if hasattr(world, 'reset'):
        world.reset()
    
    # Initialize brain
    brain.reset()
    brain.initialize()
    
    for episode in range(episodes):
        episode_reward = 0.0
        episode_performance = 0.0
        
        # Reset episode state
        if hasattr(world, 'reset'):
            world.reset()
        
        # Run episode
        for step in range(stop_criteria.get('max_steps_per_episode', 100)):
            # Update world
            if hasattr(world, 'update'):
                world.update(0.1)
            
            # Get sensory input
            if hasattr(world, 'getSensoryPercept'):
                percept = world.getSensoryPercept()
            else:
                percept = None
            
            # Process sensory input
            if hasattr(brain, 'receiveSensoryInput') and percept:
                brain.receiveSensoryInput(percept)
            
            # Brain processes input
            brain.step(step)
            
            # Get action
            if hasattr(brain, 'produceAction'):
                action = brain.produceAction()
            else:
                action = None
            
            # Apply action
            if hasattr(world, 'applyMotorCommand') and action is not None:
                world.applyMotorCommand(action, world.getSimulationTime() if hasattr(world, 'getSimulationTime') else 0.0)
            
            # Calculate reward
            if reward_function:
                reward = reward_function(world, brain, action, step)
            else:
                # Default reward: positive for any action, small penalty for steps
                reward = 1.0 - (step * 0.01)
            
            episode_reward += reward
            
            # Check for early stopping
            if stop_criteria and stop_criteria.get('max_steps_per_episode'):
                max_steps = stop_criteria['max_steps_per_episode']
                if step >= max_steps - 1:
                    break
        
        # Calculate episode performance
        episode_performance = episode_reward / (step + 1) if step > 0 else 0
        
        # Update best performance
        if episode_performance > best_performance:
            best_performance = episode_performance
            no_improvement_count = 0
        else:
            no_improvement_count += 1
        
        # Update results
        training_results['episodes_completed'] += 1
        training_results['total_reward'] += episode_reward
        training_results['episode_rewards'].append(episode_reward)
        training_results['episode_performances'].append(episode_performance)
        
        # Update best performance
        if episode_performance > training_results['best_performance']:
            training_results['best_performance'] = episode_performance
        
        # Check early stopping criteria
        if stop_criteria:
            if no_improvement_count >= stop_criteria.get('patience', 20):
                training_results['stop_reason'] = 'early_stopping_no_improvement'
                break
            
            if stop_criteria.get('min_performance') and episode_performance < stop_criteria['min_performance']:
                training_results['stop_reason'] = 'early_stopping_low_performance'
                break
    
    # Calculate averages
    if training_results['episodes_completed'] > 0:
        training_results['average_reward'] = (
            training_results['total_reward'] / training_results['episodes_completed'])
        training_results['average_performance'] = (
            sum(training_results['episode_performances']) / len(training_results['episode_performances']))
    
    # Show visualization if provided
    if visualizer:
        visualizer.show_simulation_progress(
            training_results['episodes_completed'],
            episodes,
            {
                'average_reward': training_results['average_reward'],
                'best_performance': training_results['best_performance'],
                'stop_reason': training_results['stop_reason']
            }
        )
        visualizer.show()
    
    return training_results


def evaluate_performance(brain: Any, test_cases: List[Any],
                       metrics: Optional[List[str]] = None) -> Dict[str, Any]:
    """
    Evaluate brain performance on test cases.
    
    This function runs the brain on a set of test cases and evaluates
    its performance using specified metrics.
    
    Args:
        brain: Brain object to evaluate
        test_cases: List of test cases to run
        metrics: List of metrics to calculate (e.g., 'speed', 'accuracy',
                'stability', 'learning_rate')
                
    Returns:
        Dictionary containing performance evaluation results
        
    Raises:
        ValidationError: If parameters are invalid
        
    Example:
        ```python
        test_cases = [...]  # List of test scenarios
        results = evaluate_performance(brain, test_cases, 
                                      metrics=['speed', 'stability'])
        print(f"Performance score: {results['performance_score']}")
        ```
    """
    if not test_cases:
        raise ValidationError("Test cases list cannot be empty")
    
    if metrics is None:
        metrics = ['speed', 'stability', 'accuracy', 'learning_rate']
    
    # Initialize evaluation
    evaluation_results = {
        'test_cases_evaluated': len(test_cases),
        'metrics_calculated': metrics,
        'performance_scores': {},
        'case_results': [],
        'overall_performance': 0.0,
        'recommendations': []
    }
    
    # Run evaluation on each test case
    for i, test_case in enumerate(test_cases):
        case_result = {
            'case_id': i,
            'case_type': getattr(test_case, 'type', 'unknown') if hasattr(test_case, 'type') else 'unknown',
            'success': False,
            'metrics': {},
            'error': None
        }
        
        try:
            # Reset brain for each test case
            brain.reset()
            brain.initialize()
            
            # Run test case
            # Note: Actual test case execution depends on test case format
            # This is a placeholder for actual test case logic
            steps = getattr(test_case, 'steps', 100) if hasattr(test_case, 'steps') else 100
            
            for step in range(steps):
                brain.step(step)
                
                # Check metrics if brain has them
                if 'speed' in metrics and hasattr(brain, 'getAverageFiringRate'):
                    case_result['metrics']['speed'] = brain.getAverageFiringRate()
                
                if 'stability' in metrics and hasattr(brain, 'getFiringNeuronCount'):
                    case_result['metrics']['stability'] = brain.getFiringNeuronCount() / max(brain.getTotalNeuronCount(), 1)
                
                if 'accuracy' in metrics:  # Would depend on specific test
                    case_result['metrics']['accuracy'] = 0.5  # Placeholder
                
                if 'learning_rate' in metrics and hasattr(brain, 'getDevelopmentalStage'):
                    stage = brain.getDevelopmentalStage()
                    case_result['metrics']['learning_rate'] = stage.value if hasattr(stage, 'value') else 0.5
            
            # Determine success
            case_result['success'] = True
            
            # Calculate case performance score
            case_score = 0.0
            if 'speed' in metrics and 'speed' in case_result['metrics']:
                case_score += min(case_result['metrics']['speed'] / 10.0, 1.0) * 0.25
            
            if 'stability' in metrics and 'stability' in case_result['metrics']:
                case_score += case_result['metrics']['stability'] * 0.25
            
            if 'accuracy' in metrics and 'accuracy' in case_result['metrics']:
                case_score += case_result['metrics']['accuracy'] * 0.25
            
            if 'learning_rate' in metrics and 'learning_rate' in case_result['metrics']:
                case_score += case_result['metrics']['learning_rate'] * 0.25
            
            case_result['performance_score'] = case_score
            evaluation_results['case_results'].append(case_result)
            
        except Exception as e:
            case_result['success'] = False
            case_result['error'] = str(e)
            evaluation_results['case_results'].append(case_result)
            evaluation_results['recommendations'].append(
                f"Test case {i} failed: {e}. Consider adjusting brain parameters or test conditions."
            )
    
    # Calculate overall performance
    successful_cases = [cr for cr in evaluation_results['case_results'] if cr['success']]
    if successful_cases:
        total_score = sum(cr['performance_score'] for cr in successful_cases)
        evaluation_results['overall_performance'] = total_score / len(successful_cases)
        
        # Generate recommendations based on performance
        if evaluation_results['overall_performance'] < 0.5:
            evaluation_results['recommendations'].append(
                "Overall performance is low. Consider increasing neuron count, "
                "adjusting complexity, or enabling additional features."
            )
    
    return evaluation_results


def optimize_hyperparameters(brain: Any, param_space: Dict[str, List],
                           evaluation_func: Optional[Callable] = None,
                           iterations: int = 10) -> Dict[str, Any]:
    """
    Optimize brain hyperparameters using a search strategy.
    
    This function performs hyperparameter optimization for brain parameters
    using the specified parameter space and evaluation function.
    
    Args:
        brain: Brain object to optimize (will be copied for each iteration)
        param_space: Dictionary mapping parameter names to lists of values
        evaluation_func: Function to evaluate hyperparameter combinations.
                       If None, uses default evaluation based on brain metrics.
        iterations: Number of optimization iterations to perform
        
    Returns:
        Dictionary containing optimization results and best parameters
        
    Raises:
        ValidationError: If parameters are invalid
        
    Example:
        ```python
        param_space = {
            'neuron_count': [500, 1000, 2000],
            'complexity': [0.3, 0.5, 0.7],
        }
        
        results = optimize_hyperparameters(brain, param_space, iterations=20)
        print(f"Best parameters: {results['best_parameters']}")
        ```
    """
    # Validate inputs
    if iterations < 1:
        raise ValidationError(f"Iterations must be positive, got {iterations}")
    
    if not param_space:
        raise ValidationError("Parameter space cannot be empty")
    
    # Generate parameter combinations
    param_grid = ParameterGrid(param_space)
    param_list = list(param_grid)
    
    # Limit to available iterations
    if len(param_list) > iterations:
        import random
        param_list = random.sample(param_list, iterations)
    
    # Initialize optimization results
    optimization_results = {
        'iterations': iterations,
        'parameter_space_size': len(param_space),
        'evaluated_combinations': [],
        'best_parameters': None,
        'best_score': float('-inf'),
        'best_result': None,
        'convergence': [],
        'recommendations': []
    }
    
    # Default evaluation function if not provided
    if evaluation_func is None:
        def evaluation_func(brain_obj):
            """Default evaluation function based on brain metrics."""
            try:
                brain_obj.reset()
                brain_obj.initialize()
                
                # Run simulation
                for step in range(100):
                    brain_obj.step(step)
                
                # Calculate score based on multiple metrics
                score = 0.0
                weight = 0.0
                
                if hasattr(brain_obj, 'getAverageFiringRate'):
                    firing_rate = brain_obj.getAverageFiringRate()
                    score += min(firing_rate / 20.0, 1.0)  # Normalize firing rate
                    weight += 1.0
                
                if hasattr(brain_obj, 'getActiveNeuronCount'):
                    active = brain_obj.getActiveNeuronCount()
                    total = brain_obj.getTotalNeuronCount()
                    if total > 0:
                        score += (active / total)  # Active neuron ratio
                        weight += 1.0
                
                if hasattr(brain_obj, 'getDevelopmentalStage'):
                    stage = brain_obj.getDevelopmentalStage()
                    # Map developmental stage to score (0.0 to 1.0)
                    stage_score = 0.5  # Default
                    if hasattr(stage, 'value'):
                        stage_enum = stage.value
                        if stage_enum == 'Adult':
                            stage_score = 1.0
                        elif stage_enum == 'Maturation':
                            stage_score = 0.75
                        elif stage_enum == 'CriticalPeriod':
                            stage_score = 0.5
                    score += stage_score
                    weight += 1.0
                
                if weight > 0:
                    score = score / weight
                
                return score
                
            except Exception:
                return float('-inf')  # Invalid configuration
    
    # Perform optimization
    for i, params in enumerate(param_list):
        # Create a fresh brain for this iteration
        test_brain = create_simple_brain(
            neuron_count=params.get('neuron_count', 1000),
            complexity=params.get('complexity', 0.5),
            config={k: v for k, v in params.items() if k not in ['neuron_count', 'complexity']}
        )
        
        # Evaluate the configuration
        score = evaluation_func(test_brain)
        
        # Record results
        result = {
            'iteration': i,
            'parameters': params,
            'score': score,
            'success': score != float('-inf')
        }
        
        optimization_results['evaluated_combinations'].append(result)
        
        # Track convergence
        if optimization_results['best_score'] < score:
            optimization_results['best_score'] = score
            optimization_results['best_parameters'] = params.copy()
            optimization_results['best_result'] = result
        
        optimization_results['convergence'].append(score)
    
    # Generate recommendations
    if optimization_results['best_parameters']:
        opt_params = optimization_results['best_parameters']
        
        # Generate parameter suggestions
        suggestions = []
        if 'neuron_count' in opt_params:
            count = opt_params['neuron_count']
            if count < 500:
                suggestions.append("Consider increasing neuron count for better performance")
            elif count > 5000:
                suggestions.append("Neuron count is very high; consider reducing for efficiency")
        
        if 'complexity' in opt_params:
            complexity = opt_params['complexity']
            if complexity < 0.3:
                suggestions.append("Low complexity may limit learning capacity")
            elif complexity > 0.8:
                suggestions.append("High complexity may cause instability; consider tuning")
        
        optimization_results['recommendations'].extend(suggestions)
    
    return optimization_results
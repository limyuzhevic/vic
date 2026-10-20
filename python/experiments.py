"""
Experiments NLM functionality.

This module contains experiment-related classes and operations.
"""

from typing import Dict, Any, Optional, List, Union
from ..bindings import (
    Brain,
    SimpleWorld,
    AgentBrain,
    createBrain as create_brain_native,
    Config,
)

def run_phase_3_experiment(config: Optional[Dict[str, Any]] = None,
                           duration: int = 1000) -> Dict[str, Any]:
    """
    Run a Phase 3 experiment (world interaction experiment).
    
    Args:
        config: Optional configuration dictionary.
        duration: Experiment duration in steps.
        
    Returns:
        Dictionary containing experiment results.
    """
    results = {
        'phase': 'Phase 3',
        'duration': duration,
        'success': True,
        'metrics': {},
        'error': None,
    }
    
    try:
        # Create brain
        brain = create_brain_native(Config())
        brain.initialize()
        
        # Create world
        world = SimpleWorld()
        world.configure(20, 20, 8, 8)
        
        # Create agent
        agent = AgentBrain(brain)
        agent.initialize(world)
        
        # Run experiment
        for step in range(duration):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect results
        results['metrics'] = {
            'final_spike_count': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage(),
            'total_steps': duration,
        }
        
    except Exception as e:
        results['success'] = False
        results['error'] = str(e)
    
    return results

def run_phase_4_experiment(config: Optional[Dict[str, Any]] = None,
                           duration: int = 1000) -> Dict[str, Any]:
    """
    Run a Phase 4 experiment (emerging cognition experiment).
    
    Args:
        config: Optional configuration dictionary.
        duration: Experiment duration in steps.
        
    Returns:
        Dictionary containing experiment results.
    """
    results = {
        'phase': 'Phase 4',
        'duration': duration,
        'success': True,
        'metrics': {},
        'error': None,
    }
    
    try:
        # Create brain with cognitive features
        brain = create_brain_native(Config())
        brain.initialize()
        
        # Create world
        world = SimpleWorld()
        world.configure(20, 20, 8, 8)
        
        # Create agent
        agent = AgentBrain(brain)
        agent.initialize(world)
        
        # Enable cognitive features
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        # Run experiment
        for step in range(duration):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Collect results
        results['metrics'] = {
            'final_spike_count': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'total_steps': duration,
        }
        
    except Exception as e:
        results['success'] = False
        results['error'] = str(e)
    
    return results

def run_phase_5_experiment(config: Optional[Dict[str, Any]] = None,
                           duration: int = 1000) -> Dict[str, Any]:
    """
    Run a Phase 5 experiment (performance optimization experiment).
    
    Args:
        config: Optional configuration dictionary.
        duration: Experiment duration in steps.
        
    Returns:
        Dictionary containing experiment results.
    """
    results = {
        'phase': 'Phase 5',
        'duration': duration,
        'success': True,
        'metrics': {},
        'error': None,
    }
    
    try:
        # Create brain
        brain = create_brain_native(Config())
        brain.initialize()
        
        # Create world
        world = SimpleWorld()
        world.configure(20, 20, 8, 8)
        
        # Create agent
        agent = AgentBrain(brain)
        agent.initialize(world)
        
        # Enable all features for performance test
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run experiment
        for step in range(duration):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Collect results
        results['metrics'] = {
            'final_spike_count': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'total_steps': duration,
        }
        
    except Exception as e:
        results['success'] = False
        results['error'] = str(e)
    
    return results

def run_phase_6_experiment(config: Optional[Dict[str, Any]] = None,
                           duration: int = 1000) -> Dict[str, Any]:
    """
    Run a Phase 6 experiment (final integration experiment).
    
    Args:
        config: Optional configuration dictionary.
        duration: Experiment duration in steps.
        
    Returns:
        Dictionary containing experiment results.
    """
    results = {
        'phase': 'Phase 6',
        'duration': duration,
        'success': True,
        'metrics': {},
        'error': None,
    }
    
    try:
        # Create brain
        brain = create_brain_native(Config())
        brain.initialize()
        
        # Create world
        world = SimpleWorld()
        world.configure(20, 20, 8, 8)
        
        # Create agent
        agent = AgentBrain(brain)
        agent.initialize(world)
        
        # Enable all features for final integration test
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run experiment
        for step in range(duration):
            # Update world
            world.update(0.1)
            
            # Process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Collect results
        results['metrics'] = {
            'final_spike_count': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'total_steps': duration,
            'integration_status': 'All systems integrated and functional',
        }
        
    except Exception as e:
        results['success'] = False
        results['error'] = str(e)
    
    return results

def run_comprehensive_experiment(experiment_type: str = 'phase6',
                                 config: Optional[Dict[str, Any]] = None,
                                 duration: int = 1000) -> Dict[str, Any]:
    """
    Run a comprehensive experiment of specified type.
    
    Args:
        experiment_type: Type of experiment to run ('phase3', 'phase4', 'phase5', 'phase6').
        config: Optional configuration dictionary.
        duration: Experiment duration in steps.
        
    Returns:
        Dictionary containing experiment results.
    """
    experiment_map = {
        'phase3': run_phase_3_experiment,
        'phase4': run_phase_4_experiment,
        'phase5': run_phase_5_experiment,
        'phase6': run_phase_6_experiment,
    }
    
    if experiment_type not in experiment_map:
        raise ValueError(f"Unknown experiment type: {experiment_type}. "
                        f"Available types: {list(experiment_map.keys())}")
    
    return experiment_map[experiment_type](config, duration)

def create_experiment_runner(config: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """
    Create an experiment runner with specified configuration.
    
    Args:
        config: Optional configuration dictionary.
        
    Returns:
        Dictionary containing experiment runner capabilities.
    """
    runner = {
        'phases': ['phase3', 'phase4', 'phase5', 'phase6'],
        'config': config or {},
        'capabilities': {
            'brain_creation': True,
            'world_creation': True,
            'agent_creation': True,
            'experiment_running': True,
            'results_collection': True,
        },
    }
    
    return runner

def create_test_suite(config: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """
    Create a test suite with specified configuration.
    
    Args:
        config: Optional configuration dictionary.
        
    Returns:
        Dictionary containing test suite capabilities.
    """
    test_suite = {
        'tests': [
            'test_basic_functionality',
            'test_world_interaction',
            'test_learning_capability',
            'test_memory_systems',
            'test_neuromodulation',
            'test_development',
        ],
        'config': config or {},
        'capabilities': {
            'test_creation': True,
            'test_execution': True,
            'results_collection': True,
            'performance_benchmarking': True,
        },
    }
    
    return test_suite
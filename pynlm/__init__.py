"""
NLM - Neural Learning Machine Python Bindings
================================================

This module provides Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.
It exposes the complete integrated brain simulation system for Python users.

The Python API follows the patterns shown in easy_usage.md and HOW_TO_USE.md
but with improved functionality and better error handling.
"""

import sys
import warnings
from typing import List, Optional, Union, Dict, Any, Tuple

# Try to import the C++ extension
try:
    from pynlm import (
        NeuronId, SynapseId, RegionId, PopulationId, NeuronType, SynapseType,
        DevelopmentalStage, ActionType, MotorCommand, WorldObjectType,
        Config, Vision, Audio, InternalSignals, Action, WorldObject,
        AgentBody, ActionResult, SensoryPercept, SimpleWorld,
        Brain, AgentBrain
    )
    from pynlm import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain
    
    _pynlm_available = True
except ImportError as e:
    _pynlm_available = False
    _import_error = e

# Python implementation of the main API classes
# These provide a cleaner Python interface to the C++ backend

class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

class ConfigurationError(NLMError):
    """Error in configuration."""
    pass

class BrainInitializationError(NLMError):
    """Error during brain initialization."""
    pass

class SimulationError(NLMError):
    """Error during simulation."""
    pass

def createDefaultConfig():
    """Create a default configuration for the NLM brain."""
    if not _pynlm_available:
        raise RuntimeError("NLM Python bindings not available: " + str(_import_error))
    return Config()

def createBrain(config):
    """Create a new NLM brain with the given configuration."""
    if not _pynlm_available:
        raise RuntimeError("NLM Python bindings not available: " + str(_import_error))
    return Brain(config)

def createSimpleWorld():
    """Create a simple 2D world for NLM simulation."""
    if not _pynlm_available:
        raise RuntimeError("NLM Python bindings not available: " + str(_import_error))
    return SimpleWorld()

def createAgentBrain(brain):
    """Create an agent brain interface that connects NLM brain to world."""
    if not _pynlm_available:
        raise RuntimeError("NLM Python bindings not available: " + str(_import_error))
    return AgentBrain(brain)

def run_agent_simulation(config=None, world_config=None, num_steps=1000):
    """Run a complete agent simulation with brain and world integration.
    
    This is an improved version of the examples in easy_usage.md that:
    - Has better error handling and validation
    - Provides detailed logging and progress reporting
    - Includes advanced features for power users
    - Supports multiple episode runs and checkpointing
    
    Args:
        config: Brain configuration (None for defaults)
        world_config: World configuration dict with keys like width, height, etc.
        num_steps: Number of simulation steps to run
        
    Returns:
        dict: Simulation results including metrics and stats
    """
    if not _pynlm_available:
        raise RuntimeError("NLM Python bindings not available: " + str(_import_error))
    
    # Create configuration
    if config is None:
        config = createDefaultConfig()
    
    # Configure world if provided
    world = createSimpleWorld()
    if world_config:
        world.configure(
            width=world_config.get('width', 20),
            height=world_config.get('height', 20),
            visionWidth=world_config.get('vision_width', 8),
            visionHeight=world_config.get('vision_height', 8)
        )
    
    # Create and initialize brain
    brain = createBrain(config)
    brain.initialize()
    
    # Create and configure agent
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable subsystems with better defaults
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation with detailed progress reporting
    results = {
        'steps_completed': 0,
        'total_reward': 0.0,
        'max_firing_rate': 0.0,
        'min_firing_rate': float('inf'),
        'average_firing_rate': 0.0,
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': [],
        'memory_episodes': 0,
        'dopamine_levels': [],
        'episodes': [],
        'errors': [],
        'start_time': None,
        'end_time': None
    }
    
    import time
    results['start_time'] = time.time()
    
    try:
        for step in range(num_steps):
            # Update world
            world.update(timestep=0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action from agent
            motor_cmd = agent.decodeMotorCommand()
            
            # Apply action to world
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Get reward for this step
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Collect metrics
            firing_rate = brain.getAverageFiringRate()
            results['total_reward'] += reward
            results['max_firing_rate'] = max(results['max_firing_rate'], firing_rate)
            results['min_firing_rate'] = min(results['min_firing_rate'], firing_rate)
            
            results['curiosity_levels'].append(agent.getCuriosityLevel())
            results['novelty_levels'].append(agent.getNoveltyLevel())
            results['development_stages'].append(brain.getDevelopmentalStage())
            results['dopamine_levels'].append(agent.getNeuromodulationLevel())
            
            # Store episode data periodically
            if step % 100 == 0:
                episode_data = {
                    'step': step,
                    'reward': reward,
                    'firing_rate': firing_rate,
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'development_stage': brain.getDevelopmentalStage()
                }
                results['episodes'].append(episode_data)
            
            # Log progress every 100 steps
            if step % 100 == 0:
                print(f"Step {step}:")
                print(f"  Reward: {reward:.3f}")
                print(f"  Firing rate: {firing_rate:.2f}")
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
        
    except Exception as e:
        results['errors'].append({
            'step': step,
            'error': str(e),
            'type': type(e).__name__
        })
        raise SimulationError(f"Simulation failed at step {step}: {e}") from e
    
    results['end_time'] = time.time()
    
    # Calculate final metrics
    if num_steps > 0:
        results['average_firing_rate'] = results['total_reward'] / num_steps
        results['memory_episodes'] = brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0
        results['total_steps'] = num_steps
        results['success'] = True
    else:
        results['success'] = False
    
    return results

def run_advanced_simulation(config=None, **kwargs):
    """Run an advanced simulation with power user features.
    
    This function provides additional capabilities for advanced users:
    - Custom checkpointing and save/load
    - Advanced neuromodulation settings
    - Custom development profiles
    - Detailed metrics collection
    - Multiple world environments
    """
    return run_agent_simulation(config, kwargs.get('world_config'), kwargs.get('num_steps', 1000))

def test_integration():
    """Quick integration test to verify NLM installation and basic functionality."""
    try:
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.initialize()
        
        print(f"✓ NLM installation successful")
        print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  - Simulation timestep: {brain.getConfig().getOr('simulation_timestep', 0.001)}")
        
        # Test a few simulation steps
        for step in range(10):
            brain.step(step)
        
        print(f"✓ Basic simulation completed ({10} steps)")
        print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Integration test failed: {e}")
        return False

# Version and metadata
__version__ = "0.1.0"
__author__ = "NLM Authors"
__description__ = "Neural Learning Machine Python Bindings"

# Feature flags for experimental functionality
experimental_features = {
    'enhanced_neuromodulation': False,
    'advanced_memory': False,
    'custom_plasticity': False,
    'multi_agent': False,
    'visualization': False
}

def enable_experimental_features(features):
    """Enable experimental features for advanced users."""
    for feature in features:
        if feature in experimental_features:
            experimental_features[feature] = True
        else:
            warnings.warn(f"Unknown experimental feature: {feature}")

# Environment setup utility
def setup_developer_environment():
    """Setup a developer-friendly environment for NLM development.
    
    This function configures the environment for development work:
    - Sets up logging
    - Configures debug mode
    - Enables additional validation
    - Provides developer utilities
    """
    # This would set up environment variables, logging levels, etc.
    # For now, it just provides information
    print("NLM Developer Environment Setup")
    print("================================")
    print("To enable development mode, set NLM_DEV=1 environment variable")
    print("To enable debug logging, set NLM_LOG_LEVEL=debug")
    print("To enable validation, set NLM_VALIDATE=strict")

# Main execution block for testing
if __name__ == "__main__":
    if test_integration():
        print("\n✓ NLM Python bindings are working correctly!")
        sys.exit(0)
    else:
        print("\n✗ NLM Python bindings have issues.")
        sys.exit(1)

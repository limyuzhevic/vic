"""NLM Simulation Module

This module provides high-level simulation classes and utilities for the NLM framework.
"""

import time
from typing import Dict, List, Optional, Callable, Any
import logging

logger = logging.getLogger(__name__)

class Simulation:
    """High-level simulation class for complete simulation management.
    
    Provides a comprehensive interface for running NLM simulations with
    context manager support, callback system, and statistics collection.
    """
    
    def __init__(self, brain=None, world=None, agent=None, config=None):
        """Initialize simulation with brain, world, and agent components.
        
        Args:
            brain: Brain instance (create from config if not provided)
            world: World instance (create default if not provided)
            agent: AgentBrain instance (create from brain if not provided)
            config: Configuration (create default if not provided)
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self.config = config
        self.stats = {
            'steps_completed': 0,
            'total_neurons': 0,
            'active_neurons': 0,
            'total_spikes': 0,
            'simulation_time': 0.0,
            'start_time': None,
            'end_time': None
        }
        
        # Callback functions
        self.step_callbacks = []
        self.error_callbacks = []
        self.complete_callbacks = []
        
        # Initialize components if not provided
        if self.brain is None:
            if self.config is None:
                from pynlm import createDefaultConfig
                self.config = createDefaultConfig()
            from pynlm import createBrain
            self.brain = createBrain(self.config)
            
        if self.world is None:
            from pynlm import createSimpleWorld
            self.world = createSimpleWorld()
            
        if self.agent is None:
            from pynlm import createAgentBrain
            self.agent = createAgentBrain(self.brain)
            
    def __enter__(self):
        """Context manager entry point."""
        self.start()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit point."""
        self.stop()
        
    def start(self):
        """Start the simulation."""
        logger.info("Starting simulation")
        self.stats['start_time'] = time.time()
        self.stats['steps_completed'] = 0
        
        # Execute start callbacks
        for callback in self.step_callbacks:
            if callback.__name__ == 'on_start':
                callback(self)
                
    def stop(self):
        """Stop the simulation."""
        logger.info("Stopping simulation")
        self.stats['end_time'] = time.time()
        self.stats['simulation_time'] = self.stats['end_time'] - self.stats['start_time']
        
        # Execute completion callbacks
        for callback in self.complete_callbacks:
            callback(self)
            
    def run_steps(self, steps):
        """Run simulation for specified number of steps.
        
        Args:
            steps: Number of simulation steps to run
            
        Returns:
            Dictionary with simulation statistics
        """
        logger.info(f"Running {steps} simulation steps")
        
        for step in range(steps):
            try:
                # Execute simulation step
                self.brain.step()
                self.world.update(0.1)
                
                # Update statistics
                self.stats['steps_completed'] = step + 1
                self.stats['total_neurons'] = self.brain.getTotalNeuronCount()
                self.stats['active_neurons'] = self.brain.getActiveNeuronCount()
                self.stats['total_spikes'] = self.brain.getTotalSpikeCount()
                
                # Execute step callbacks
                for callback in self.step_callbacks:
                    if callback.__name__ == 'on_step':
                        callback(self, step, self.stats)
                        
                # Check for completion conditions
                if self._should_complete():
                    logger.info(f"Simulation completed early at step {step}")
                    break
                    
            except Exception as e:
                logger.error(f"Error at step {step}: {e}")
                # Execute error callbacks
                for callback in self.error_callbacks:
                    callback(self, step, e, self.stats)
                raise
                
        return self.stats
        
    def run_episode(self, max_steps=100):
        """Run a complete agent episode.
        
        Args:
            max_steps: Maximum number of steps for episode
            
        Returns:
            Dictionary with episode statistics
        """
        logger.info(f"Starting episode with max {max_steps} steps")
        
        start_time = time.time()
        steps_completed = 0
        total_spikes = 0
        
        for step in range(max_steps):
            try:
                # Get sensory input
                percept = self.world.getSensoryPercept()
                
                # Process sensory input
                self.agent.processSensoryInput(percept)
                
                # Get action from brain
                action = self.agent.decodeMotorCommand()
                
                # Apply action in world
                result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Apply reward modulation if action successful
                if result.success:
                    self.agent.applyRewardModulation(result.reward, result.reward * 0.5)
                
                # Update statistics
                steps_completed = step + 1
                total_spikes = self.brain.getTotalSpikeCount()
                
                # Execute step callbacks
                for callback in self.step_callbacks:
                    if callback.__name__ == 'on_step':
                        callback(self, step, self.stats)
                        
                # Check for completion conditions
                if self._should_complete():
                    logger.info(f"Episode completed early at step {step}")
                    break
                    
            except Exception as e:
                logger.error(f"Error at step {step}: {e}")
                # Execute error callbacks
                for callback in self.error_callbacks:
                    callback(self, step, e, self.stats)
                raise
                
        end_time = time.time()
        duration = end_time - start_time
        
        self.stats['steps_completed'] = steps_completed
        self.stats['total_spikes'] = total_spikes
        self.stats['simulation_time'] = duration
        
        logger.info(f"Episode completed: {steps_completed} steps in {duration:.2f}s")
        return self.stats
        
    def _should_complete(self):
        """Check if simulation should be completed early."""
        # Check for high convergence
        if hasattr(self.brain, 'getAverageFiringRate'):
            firing_rate = self.brain.getAverageFiringRate()
            if firing_rate > 100.0:  # High firing rate threshold
                return True
                
        # Check for developmental stage completion
        if hasattr(self.brain, 'getDevelopmentalStage'):
            if self.brain.getDevelopmentalStage() == 4:  # Aging stage
                return True
                
        return False
        
    def add_step_callback(self, callback):
        """Add step callback function.
        
        Args:
            callback: Function to call at each step
        """
        self.step_callbacks.append(callback)
        
    def add_error_callback(self, callback):
        """Add error callback function.
        
        Args:
            callback: Function to call when error occurs
        """
        self.error_callbacks.append(callback)
        
    def add_complete_callback(self, callback):
        """Add completion callback function.
        
        Args:
            callback: Function to call when simulation completes
        """
        self.complete_callbacks.append(callback)
        
    def get_statistics(self):
        """Get current simulation statistics.
        
        Returns:
            Dictionary with current statistics
        """
        return self.stats.copy()
        
    def reset(self):
        """Reset simulation state."""
        logger.info("Resetting simulation")
        self.stats = {
            'steps_completed': 0,
            'total_neurons': 0,
            'active_neurons': 0,
            'total_spikes': 0,
            'simulation_time': 0.0,
            'start_time': None,
            'end_time': None
        }
        
        if self.brain:
            self.brain.reset()
            
        if self.agent:
            self.agent.reset()
            
        if self.world:
            self.world.reset()
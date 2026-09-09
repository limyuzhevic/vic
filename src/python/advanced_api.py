"""
Enhanced NLM Python API for Advanced Users

This module provides advanced capabilities for working with NLM (Neural Learning Machine)
brains, including:

1. Advanced brain manipulation and configuration
2. Experiment management
3. Performance optimization tools
4. Memory management utilities
5. Development tools for research and testing

Key features:
- Brain cloning and copying
- Advanced simulation control
- Performance profiling
- Memory checkpointing
- Experiment scheduling
- Research tools
"""

import pynlm
import numpy as np
import json
import time
import threading
import queue
from typing import List, Dict, Optional, Callable, Any
from dataclasses import dataclass, field
from enum import Enum
import logging

logger = logging.getLogger(__name__)

class ExperimentStatus(Enum):
    PENDING = "pending"
    RUNNING = "running"
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"

@dataclass
class Experiment:
    """Represents a NLM simulation experiment with advanced features."""
    name: str
    brain: pynlm.Brain
    world: Optional[pynlm.SimpleWorld] = None
    agent: Optional[pynlm.AgentBrain] = None
    status: ExperimentStatus = ExperimentStatus.PENDING
    start_time: Optional[float] = None
    end_time: Optional[float] = None
    steps_completed: int = 0
    max_steps: int = 1000
    tags: List[str] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)
    callback: Optional[Callable] = None
    
    def __post_init__(self):
        if self.callback is None:
            self.callback = self.default_callback
    
    def default_callback(self, step: int, brain: pynlm.Brain) -> None:
        """Default callback that logs progress."""
        if step % 100 == 0:
            logger.info(f"Experiment '{self.name}' at step {step}")

class BrainManager:
    """Advanced brain manipulation and management class."""
    
    def __init__(self):
        self.brains = {}
        self.experiments = []
    
    def create_brain(self, config: Optional[pynlm.Config] = None, 
                     name: Optional[str] = None) -> str:
        """Create a new brain with optional configuration."""
        if config is None:
            config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        brain_id = name or f"brain_{len(self.brains)}"
        self.brains[brain_id] = brain
        return brain_id
    
    def clone_brain(self, source_brain_id: str, target_brain_id: Optional[str] = None,
                    save_to_file: Optional[str] = None) -> str:
        """Clone a brain to another brain or save to file."""
        if source_brain_id not in self.brains:
            raise ValueError(f"Source brain '{source_brain_id}' not found")
        
        source_brain = self.brains[source_brain_id]
        
        if save_to_file:
            # Save to file
            source_brain.save(save_to_file)
            logger.info(f"Brain '{source_brain_id}' saved to '{save_to_file}'")
            return source_brain_id
        
        # Create new brain and copy state
        config = source_brain.getConfig()
        target_brain = pynlm.createBrain(config)
        target_brain.initialize()
        
        # Try to load saved state if available
        import tempfile
        with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as tmp:
            source_brain.save(tmp.name)
            try:
                target_brain.load(tmp.name)
                logger.info(f"Brain '{source_brain_id}' cloned to '{target_brain_id}'")
            except:
                logger.warning(f"Failed to load saved state, using fresh brain")
            finally:
                import os
                os.unlink(tmp.name)
        
        target_id = target_brain_id or f"clone_{source_brain_id}"
        self.brains[target_id] = target_brain
        return target_id
    
    def run_experiment(self, experiment: Experiment) -> Experiment:
        """Run an experiment with advanced monitoring and control."""
        logger.info(f"Starting experiment '{experiment.name}' with {experiment.max_steps} steps")
        experiment.status = ExperimentStatus.RUNNING
        experiment.start_time = time.time()
        experiment.steps_completed = 0
        
        try:
            # Setup world and agent if needed
            if experiment.world and experiment.agent:
                experiment.agent.initialize(experiment.world)
            
            # Run simulation loop
            for step in range(experiment.max_steps):
                if experiment.status == ExperimentStatus.CANCELLED:
                    logger.info(f"Experiment '{experiment.name}' cancelled at step {step}")
                    break
                
                # Execute step with optional callback
                if experiment.world and experiment.agent:
                    # Full agent simulation
                    experiment.world.update(0.1)
                    percept = experiment.world.getSensoryPercept()
                    experiment.agent.processSensoryInput(percept)
                    experiment.brain.step(step)
                    action = experiment.agent.decodeMotorCommand()
                    experiment.world.applyMotorCommand(action, experiment.world.getSimulationTime())
                else:
                    # Simple brain simulation
                    experiment.brain.step(step)
                
                experiment.steps_completed = step + 1
                
                # Execute callback
                experiment.callback(step, experiment.brain)
                
                # Check for conditions
                if self.should_stop_experiment(experiment):
                    logger.info(f"Experiment '{experiment.name}' stopped early at step {step}")
                    break
            
            experiment.status = ExperimentStatus.COMPLETED
            experiment.end_time = time.time()
            
        except Exception as e:
            logger.error(f"Experiment '{experiment.name}' failed: {e}")
            experiment.status = ExperimentStatus.FAILED
            raise
        
        return experiment
    
    def should_stop_experiment(self, experiment: Experiment) -> bool:
        """Check if experiment should stop based on conditions."""
        brain = experiment.brain
        
        # Stop if no neurons firing for extended period
        if brain.getFiringNeuronCount() == 0 and experiment.steps_completed > 100:
            return True
        
        # Stop if brain energy is too low (if applicable)
        if hasattr(brain, 'getNeuromodulationLevel'):
            level = brain.getNeuromodulationLevel()
            if abs(level) < 0.001:  # Very low activity
                return True
        
        return False
    
    def run_multiple_experiments(self, experiments: List[Experiment]) -> List[Experiment]:
        """Run multiple experiments in parallel or sequentially."""
        results = []
        
        for experiment in experiments:
            try:
                result = self.run_experiment(experiment)
                results.append(result)
            except Exception as e:
                logger.error(f"Failed to run experiment '{experiment.name}': {e}")
                # Continue with other experiments
        
        return results

# Global instances
_brain_manager = BrainManager()

# Factory functions for common use cases
def create_advanced_brain(neuron_count: int = 1000, region_count: int = 1,
                          connectivity: float = 0.1, random_seed: int = 42) -> pynlm.Brain:
    """Create an advanced brain with specific configuration."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("region_count", region_count)
    config.set("connection_probability", connectivity)
    config.set("random_seed", random_seed)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

def create_research_environment(neuron_count: int = 2000, enable_plasticity: bool = True,
                                enable_development: bool = True, enable_curiosity: bool = True) -> tuple:
    """Create a research environment with all advanced features enabled."""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    config.set("plasticity_stdp_enable", enable_plasticity)
    config.set("plasticity_hebbian_enable", enable_plasticity)
    config.set("development_enable", enable_development)
    config.set("neuromodulation_curiosity_enable", enable_curiosity)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, world, agent

def run_behavioral_experiment(steps: int = 1000, visualize: bool = False,
                              track_metrics: bool = True) -> Dict:
    """Run a complete behavioral experiment with metrics tracking."""
    logger.info(f"Starting behavioral experiment with {steps} steps")
    
    # Create research environment
    brain, world, agent = create_research_environment()
    
    # Track metrics
    metrics = {
        'steps_completed': 0,
        'neurons_firing': [],
        'total_spikes': 0,
        'action_counts': {},
        'reward_history': [],
        'curiosity_levels': [],
        'start_time': time.time(),
        'end_time': None,
        'success': False
    }
    
    try:
        # Run simulation loop
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            
            # Track metrics
            metrics['neurons_firing'].append(brain.getFiringNeuronCount())
            metrics['total_spikes'] = brain.getTotalSpikeCount()
            metrics['action_counts'][str(action)] = metrics['action_counts'].get(str(action), 0) + 1
            metrics['curiosity_levels'].append(agent.getCuriosityLevel())
            
            # Apply motor command
            result = world.applyMotorCommand(action, world.getSimulationTime())
            metrics['reward_history'].append(result.reward)
            
            # Record metrics periodically
            if step % 100 == 0 and track_metrics:
                logger.info(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                          f"curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Experiment completed successfully
        metrics['end_time'] = time.time()
        metrics['success'] = True
        
        logger.info(f"Behavioral experiment completed successfully in {steps} steps")
        
    except Exception as e:
        logger.error(f"Behavioral experiment failed: {e}")
        metrics['error'] = str(e)
        raise
    
    return metrics

def analyze_brain_performance(brain: pynlm.Brain, steps: int = 100) -> Dict:
    """Analyze brain performance over a given number of steps."""
    logger.info(f"Analyzing brain performance over {steps} steps")
    
    metrics = {
        'step': list(range(steps)),
        'firing_neurons': [],
        'average_firing_rate': [],
        'total_spikes': [],
        'e_i_ratio': [],
        'developmental_stage': [],
        'plasticity_modifier': [],
        'neuromodulation_level': []
    }
    
    for step in range(steps):
        brain.step(step)
        
        metrics['firing_neurons'].append(brain.getFiringNeuronCount())
        metrics['average_firing_rate'].append(brain.getAverageFiringRate())
        metrics['total_spikes'].append(brain.getTotalSpikeCount())
        metrics['e_i_ratio'].append(brain.getExcitationInhibitionRatio())
        
        # Try to get development and neuromodulation info
        if hasattr(brain, 'getDevelopmentalStage'):
            metrics['developmental_stage'].append(brain.getDevelopmentalStage())
        if hasattr(brain, 'getNeuromodulationLevel'):
            metrics['neuromodulation_level'].append(brain.getNeuromodulationLevel())
        if hasattr(brain, 'getConfig'):
            # Try to get plasticity modifier
            try:
                stage = brain.getDevelopmentalStage()
                metrics['developmental_stage'].append(stage)
            except:
                pass
    
    return metrics

def save_brain_snapshot(brain: pynlm.Brain, path: str, metadata: Optional[Dict] = None):
    """Save a brain snapshot with metadata."""
    import json
    
    snapshot = {
        'timestamp': time.time(),
        'brain_stats': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        }
    }
    
    if metadata:
        snapshot['metadata'] = metadata
    
    # Save brain state
    brain.save(path)
    
    # Save metadata separately
    metadata_path = path.replace('.bin', '_metadata.json')
    with open(metadata_path, 'w') as f:
        json.dump(snapshot, f, indent=2)
    
    logger.info(f"Brain snapshot saved to {path} with metadata at {metadata_path}")
    return snapshot
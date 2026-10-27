"""
pynlm - Experiment Framework

Experiment framework and utilities for NLM simulations.
"""

import json
import time
import threading
from typing import List, Dict, Any, Optional, Callable
from datetime import datetime
import numpy as np
from enum import Enum

# Import main classes
import pynlm as nlm_core
class ExperimentStatus(Enum):
    """Status of an experiment."""
    PENDING = "pending"
    RUNNING = "running"
    COMPLETED = "completed"
    FAILED = "failed"
    PAUSED = "paused"
class ExperimentConfig:
    """Configuration for an NLM experiment."""
    
    def __init__(self, name: str, description: str = ""):
        self.name = name
        self.description = description
        self.config = nlm_core.createDefaultConfig()
        self.world_config = {}
        self.agent_config = {}
        self.experiment_params = {}
        self.tags = []
        
    def set_brain_config(self, **kwargs):
        """Set brain configuration parameters."""
        for key, value in kwargs.items():
            self.config.set(key, value)
        return self
    
    def set_world_config(self, **kwargs):
        """Set world configuration parameters."""
        self.world_config.update(kwargs)
        return self
    
    def set_agent_config(self, **kwargs):
        """Set agent configuration parameters."""
        self.agent_config.update(kwargs)
        return self
    
    def set_experiment_param(self, key: str, value):
        """Set experiment-specific parameter."""
        self.experiment_params[key] = value
        return self
    
    def add_tag(self, tag: str):
        """Add a tag to the experiment."""
        self.tags.append(tag)
        return self
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        return {
            'name': self.name,
            'description': self.description,
            'brain_config': {},
            'world_config': self.world_config,
            'agent_config': self.agent_config,
            'experiment_params': self.experiment_params,
            'tags': self.tags
        }
class ExperimentResult:
    """Result of an experiment run."""
    
    def __init__(self, experiment: 'Experiment'):
        self.experiment = experiment
        self.start_time = None
        self.end_time = None
        self.status = ExperimentStatus.PENDING
        self.metrics = {}
        self.events = []
        self.checkpoint_data = None
        self.final_state = None
        
    def start(self):
        """Start the experiment."""
        self.start_time = time.time()
        self.status = ExperimentStatus.RUNNING
        return self
    
    def complete(self):
        """Complete the experiment."""
        self.end_time = time.time()
        self.status = ExperimentStatus.COMPLETED
        return self
    
    def fail(self, error: str = ""):
        """Mark experiment as failed."""
        self.end_time = time.time()
        self.status = ExperimentStatus.FAILED
        if error:
            self.events.append({
                'timestamp': time.time(),
                'type': 'error',
                'message': error
            })
        return self
    
    def pause(self):
        """Pause the experiment."""
        if self.status == ExperimentStatus.RUNNING:
            self.status = ExperimentStatus.PAUSED
        return self
    
    def resume(self):
        """Resume the experiment."""
        if self.status == ExperimentStatus.PAUSED:
            self.status = ExperimentStatus.RUNNING
        return self
    
    def get_duration(self) -> float:
        """Get experiment duration in seconds."""
        if not self.start_time:
            return 0.0
        end = self.end_time if self.end_time else time.time()
        return end - self.start_time
    
    def add_event(self, event_type: str, message: str, data: Dict[str, Any] = None):
        """Add an event to the experiment log."""
        event = {
            'timestamp': time.time(),
            'type': event_type,
            'message': message,
            'data': data or {}
        }
        self.events.append(event)
        return self
    
    def update_metrics(self, metrics: Dict[str, Any]):
        """Update experiment metrics."""
        self.metrics.update(metrics)
        return self
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert result to dictionary."""
        return {
            'experiment_name': self.experiment.name,
            'status': self.status.value,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'duration': self.get_duration(),
            'metrics': self.metrics,
            'events': self.events,
            'checkpoint_data': self.checkpoint_data,
            'final_state': self.final_state
        }
class Experiment:
    """Base class for NLM experiments."""
    
    def __init__(self, name: str, config: ExperimentConfig = None):
        self.name = name
        self.config = config or ExperimentConfig(name)
        self.result = ExperimentResult(self)
        self.brain = None
        self.agent = None
        self.world = None
        self.is_running = False
        self.pause_event = threading.Event()
        self.step_callbacks: List[Callable] = []
        
    def setup(self) -> bool:
        """Setup experiment components. Override in subclass."""
        raise NotImplementedError("Subclasses must implement setup()")
    
    def run_step(self, step: int, timestamp: float) -> bool:
        """Run a single step of the experiment. Override in subclass."""
        raise NotImplementedError("Subclasses must implement run_step()")
    
    def cleanup(self):
        """Cleanup experiment components. Override in subclass."""
        pass
    
    def run(self, num_steps: int = 1000, timestep: float = 0.1) -> ExperimentResult:
        """Run the experiment."""
        # Setup
        if not self.setup():
            self.result.fail("Setup failed")
            return self.result
        
        self.is_running = True
        self.result.start()
        
        try:
            # Run experiment
            for step in range(num_steps):
                if not self.is_running:
                    self.result.add_event('user_action', 'Experiment stopped by user')
                    break
                
                # Handle pause
                if self.result.status == ExperimentStatus.PAUSED:
                    self.pause_event.wait()
                    continue
                
                # Run step
                timestamp = step * timestep
                if not self.run_step(step, timestamp):
                    self.result.add_event('step_error', f'Step {step} failed')
                    break
                
                # Call step callbacks
                for callback in self.step_callbacks:
                    callback(step, self.result)
                
                # Small delay to simulate real-time
                time.sleep(timestep)
            
            self.is_running = False
            self.result.complete()
            
        except Exception as e:
            self.result.fail(f"Experiment error: {str(e)}")
            raise
        
        finally:
            # Cleanup
            self.cleanup()
        
        return self.result
    
    def stop(self):
        """Stop the experiment."""
        self.is_running = False
        self.result.pause()
    
    def add_step_callback(self, callback: Callable):
        """Add callback for each step."""
        self.step_callbacks.append(callback)
    
    def remove_step_callback(self, callback: Callable):
        """Remove step callback."""
        if callback in self.step_callbacks:
            self.step_callbacks.remove(callback)
class LearningExperiment(Experiment):
    """Experiment focused on learning and adaptation."""
    
    def __init__(self, name: str, config: ExperimentConfig = None, 
                 num_steps: int = 1000, enable_learning: bool = True):
        super().__init__(name, config)
        self.num_steps = num_steps
        self.enable_learning = enable_learning
        self.performance_history = []
        
    def setup(self) -> bool:
        """Setup learning experiment."""
        try:
            # Create brain with learning-enabled configuration
            brain_config = nlm_core.createDefaultConfig()
            
            # Apply custom configurations
            for key, value in self.config.brain_config.items():
                brain_config.set(key, value)
            
            self.brain = nlm_core.createBrain(brain_config)
            self.brain.initialize()
            
            # Create agent with learning systems
            self.agent = nlm_core.createAgentBrain(self.brain)
            
            # Create world
            self.world = nlm_core.createSimpleWorld()
            
            # Configure world
            world_params = self.config.world_config
            self.world.configure(
                width=world_params.get('width', 20),
                height=world_params.get('height', 20),
                visionWidth=world_params.get('vision_width', 8),
                visionHeight=world_params.get('vision_height', 8)
            )
            
            self.world.reset()
            self.world.setAgentStart(10.0, 10.0)
            
            self.agent.initialize(self.world)
            
            # Enable learning systems based on experiment parameters
            learning_params = self.config.experiment_params
            
            if self.enable_learning:
                if learning_params.get('enable_reward_modulation', False):
                    self.agent.enableRewardModulation(True)
                if learning_params.get('enable_curiosity', False):
                    self.agent.enableCuriosity(True)
                if learning_params.get('enable_development', False):
                    self.agent.enableDevelopment(True)
                if learning_params.get('enable_structural_plasticity', False):
                    self.agent.enableStructuralPlasticity(True)
                if learning_params.get('enable_prediction', False):
                    pass  # Prediction system is built-in
            
            self.result.add_event('setup_complete', 'Experiment setup completed')
            return True
            
        except Exception as e:
            self.result.add_event('setup_error', f'Setup failed: {str(e)}')
            return False
    
    def run_step(self, step: int, timestamp: float) -> bool:
        """Run learning experiment step."""
        try:
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process input
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step, timestamp)
            
            # Decision making
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, timestamp)
            
            # Apply learning if enabled
            if self.enable_learning:
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                predicted = self.agent.getPredictionError()
                self.agent.applyRewardModulation(reward, predicted)
                self.agent.updateDevelopment(0.1)
            
            # Record performance
            performance = {
                'step': step,
                'timestamp': timestamp,
                'spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'prediction_error': self.agent.getPredictionError(),
                'agent_x': self.world.getAgentBody().x,
                'agent_y': self.world.getAgentBody().y,
                'agent_energy': self.world.getAgentBody().energy,
                'agent_health': self.world.getAgentBody().health,
                'action': str(action)
            }
            
            self.performance_history.append(performance)
            
            # Update experiment metrics
            self.result.update_metrics({
                'current_step': step,
                'total_spikes': performance['spikes'],
                'firing_rate': performance['firing_rate'],
                'curiosity_level': performance['curiosity'],
                'novelty_level': performance['novelty']
            })
            
            return True
            
        except Exception as e:
            self.result.add_event('step_error', f'Step {step} failed: {str(e)}')
            return False
class AdaptationExperiment(Experiment):
    """Experiment focused on adaptation to changing environments."""
    
    def __init__(self, name: str, config: ExperimentConfig = None,
                 change_points: List[int] = None, adaptation_rate: float = 0.1):
        super().__init__(name, config)
        self.change_points = change_points or [500, 1000, 1500]
        self.adaptation_rate = adaptation_rate
        self.environment_state = 'stable'
        self.change_point_index = 0
        
    def setup(self) -> bool:
        """Setup adaptation experiment."""
        if not super().setup():
            return False
        
        # Initialize environment state
        self.environment_state = 'initial'
        
        # Setup environment changes
        self.result.add_event('environment_setup', 
                            f'Environment will change at steps: {self.change_points}')
        return True
    
    def run_step(self, step: int, timestamp: float) -> bool:
        """Run adaptation experiment step."""
        if not super().run_step(step, timestamp):
            return False
        
        # Check for environment change
        if (self.change_point_index < len(self.change_points) and 
            step >= self.change_points[self.change_point_index]):
            
            # Trigger environment change
            self._trigger_environment_change(step)
            self.change_point_index += 1
        
        # Adapt based on current environment state
        self._adapt_to_environment(step, timestamp)
        
        return True
    
    def _trigger_environment_change(self, step: int):
        """Trigger an environment change."""
        # Add event
        self.result.add_event('environment_change', 
                            f'Environment changed at step {step}',
                            {'step': step, 'change_point': self.change_point_index})
        
        # Update environment state
        self.environment_state = 'changing'
        
        # Apply changes based on change point
        if self.change_point_index == 0:
            # Change world size
            new_width = 30
            new_height = 30
            self.world.configure(new_width, new_height, 
                              self.world.getVisionWidth(), 
                              self.world.getVisionHeight())
        elif self.change_point_index == 1:
            # Add more obstacles
            pass  # Implementation would add obstacles
        elif self.change_point_index == 2:
            # Change agent parameters
            self.world.setAgentStart(15.0, 15.0)
        
        # Transition to stable state after brief period
        self.environment_state = 'stable'
    
    def _adapt_to_environment(self, step: int, timestamp: float):
        """Adapt behavior to current environment state."""
        
        # Get current metrics
        brain = self.brain
        agent = self.agent
        world = self.world
        
        if self.environment_state == 'stable':
            # Normal adaptation
            self._normal_adaptation(step, timestamp)
        elif self.environment_state == 'changing':
            # Rapid adaptation to new conditions
            self._rapid_adaptation(step, timestamp)
    
    def _normal_adaptation(self, step: int, timestamp: float):
        """Normal adaptation behavior."""
        
        # Use curiosity to explore new areas
        if step % 100 == 0:
            curiosity = agent.getCuriosityLevel()
            if curiosity > 0.5:
                # Take exploratory action
                pass  # Implementation would add exploration
        
        # Use reward to reinforce successful behaviors
        percept = world.getSensoryPercept()
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        if reward > 0.1:
            agent.applyRewardModulation(reward, 0.0)
    
    def _rapid_adaptation(self, step: int, timestamp: float):
        """Rapid adaptation to changing environment."""
        
        # Increase curiosity to detect changes quickly
        if hasattr(agent, 'enableCuriosity'):
            agent.enableCuriosity(True)
        
        # Use prediction error to detect changes
        prediction_error = agent.getPredictionError()
        if prediction_error > 0.5:
            # Environment has changed, adapt quickly
            self._emergency_adaptation()
    
    def _emergency_adaptation(self):
        """Emergency adaptation to sudden changes."""
        
        # Maximize exploration
        if hasattr(self.agent, 'enableCuriosity'):
            self.agent.enableCuriosity(True)
        
        # Reset development stage to increase plasticity
        if hasattr(self.brain, 'setDevelopmentalStage'):
            self.brain.setDevelopmentalStage(
                nlm_core.DevelopmentalStage.CriticalPeriod
            )
class ExperimentManager:
    """Manager for multiple NLM experiments."""
    
    def __init__(self):
        self.experiments: List[Experiment] = []
        self.results: List[ExperimentResult] = []
        self.is_running = False
        self.current_experiment_index = 0
        
    def add_experiment(self, experiment: Experiment):
        """Add an experiment to the manager."""
        self.experiments.append(experiment)
        return len(self.experiments) - 1
    
    def remove_experiment(self, experiment_id: int):
        """Remove an experiment."""
        if 0 <= experiment_id < len(self.experiments):
            del self.experiments[experiment_id]
            # Adjust current index if needed
            if self.current_experiment_index >= experiment_id:
                self.current_experiment_index = max(0, self.current_experiment_index - 1)
            return True
        return False
    
    def run_experiment(self, experiment_id: int) -> ExperimentResult:
        """Run a specific experiment."""
        if not (0 <= experiment_id < len(self.experiments)):
            raise ValueError(f"Experiment ID {experiment_id} not found")
        
        experiment = self.experiments[experiment_id]
        result = experiment.run()
        self.results.append(result)
        
        return result
    
    def run_all_experiments(self, sequential: bool = True) -> List[ExperimentResult]:
        """Run all experiments."""
        self.results = []
        self.is_running = True
        
        if sequential:
            # Run sequentially
            for i in range(len(self.experiments)):
                self.current_experiment_index = i
                print(f"Running experiment {i+1}/{len(self.experiments)}: {self.experiments[i].name}")
                result = self.run_experiment(i)
                self.results.append(result)
        else:
            # Run in parallel (simplified implementation)
            print("Parallel execution not yet implemented")
            for i in range(len(self.experiments)):
                self.current_experiment_index = i
                print(f"Running experiment {i+1}/{len(self.experiments)}: {self.experiments[i].name}")
                result = self.run_experiment(i)
                self.results.append(result)
        
        self.is_running = False
        return self.results
    
    def stop_current_experiment(self):
        """Stop the current experiment."""
        if self.is_running and self.current_experiment_index < len(self.experiments):
            self.experiments[self.current_experiment_index].stop()
            self.is_running = False
    
    def get_experiment(self, experiment_id: int) -> Optional[Experiment]:
        """Get an experiment by ID."""
        if 0 <= experiment_id < len(self.experiments):
            return self.experiments[experiment_id]
        return None
    
    def get_result(self, experiment_id: int) -> Optional[ExperimentResult]:
        """Get an experiment result by ID."""
        if 0 <= experiment_id < len(self.results):
            return self.results[experiment_id]
        return None
    
    def export_results(self, filename: str, format: str = 'json'):
        """Export all experiment results to file."""
        results_data = {
            'experiments': [exp.name for exp in self.experiments],
            'results': [result.to_dict() for result in self.results]
        }
        
        if format == 'json':
            with open(filename, 'w') as f:
                json.dump(results_data, f, indent=2)
        elif format == 'csv':
            import csv
            
            with open(filename, 'w', newline='') as csvfile:
                writer = csv.writer(csvfile)
                
                # Write header
                headers = ['experiment_name', 'status', 'duration', 'metrics']
                writer.writerow(headers)
                
                # Write data
                for result in self.results:
                    metrics_str = json.dumps(result.metrics)
                    writer.writerow([
                        result.experiment.name,
                        result.status.value,
                        result.get_duration(),
                        metrics_str
                    ])
    
    def generate_report(self) -> str:
        """Generate a report of all experiment results."""
        
        if not self.results:
            return "No experiments have been run yet."
        
        report_lines = [
            "=== NLM Experiment Manager Report ===",
            "",
            f"Total experiments: {len(self.experiments)}",
            f"Total experiments run: {len(self.results)}",
            f"Successful experiments: {sum(1 for r in self.results if r.status == ExperimentStatus.COMPLETED)}",
            f"Failed experiments: {sum(1 for r in self.results if r.status == ExperimentStatus.FAILED)}",
            f"Running experiments: {sum(1 for r in self.results if r.status == ExperimentStatus.RUNNING)}",
            f"Paused experiments: {sum(1 for r in self.results if r.status == ExperimentStatus.PAUSED)}",
            "",
            "Experiment Results Summary:"
        ]
        
        for i, result in enumerate(self.results):
            experiment = result.experiment
            report_lines.append(f"\n{i+1}. {experiment.name}")
            report_lines.append(f"   Status: {result.status.value}")
            report_lines.append(f"   Duration: {result.get_duration():.2f} seconds")
            report_lines.append(f"   Metrics: {json.dumps(result.metrics, indent=2)}")
        
        return "\n".join(report_lines)
def create_learning_experiment(name: str, **kwargs) -> LearningExperiment:
    """Create a learning experiment with common parameters."""
    
    config = ExperimentConfig(name)
    
    # Configure brain
    if 'neuron_count' in kwargs:
        config.set_brain_config(brain_neuron_count=kwargs['neuron_count'])
    if 'synapse_density' in kwargs:
        config.set_brain_config(brain_synapse_density=kwargs['synapse_density'])
    if 'enable_stdp' in kwargs:
        config.set_brain_config(plasticity_stdp_enable=kwargs['enable_stdp'])
    
    # Configure world
    if 'world_size' in kwargs:
        size = kwargs['world_size']
        config.set_world_config(width=size, height=size)
    if 'vision_range' in kwargs:
        config.set_world_config(vision_width=kwargs['vision_range'],
                               vision_height=kwargs['vision_range'])
    
    # Configure agent/learning
    if 'enable_curiosity' in kwargs:
        config.set_experiment_param('enable_curiosity', kwargs['enable_curiosity'])
    if 'enable_reward' in kwargs:
        config.set_experiment_param('enable_reward_modulation', kwargs['enable_reward'])
    if 'enable_development' in kwargs:
        config.set_experiment_param('enable_development', kwargs['enable_development'])
    
    # Set experiment parameters
    config.set_experiment_param('num_steps', kwargs.get('num_steps', 1000))
    
    return LearningExperiment(name, config, 
                             num_steps=kwargs.get('num_steps', 1000),
                             enable_learning=kwargs.get('enable_learning', True))
def create_adaptation_experiment(name: str, **kwargs) -> AdaptationExperiment:
    """Create an adaptation experiment with common parameters."""
    
    config = ExperimentConfig(name)
    
    # Configure brain
    if 'neuron_count' in kwargs:
        config.set_brain_config(brain_neuron_count=kwargs['neuron_count'])
    
    # Configure world
    if 'world_size' in kwargs:
        size = kwargs['world_size']
        config.set_world_config(width=size, height=size)
    
    # Configure adaptation
    if 'change_points' in kwargs:
        config.set_experiment_param('change_points', kwargs['change_points'])
    
    config.set_experiment_param('adaptation_rate', kwargs.get('adaptation_rate', 0.1))
    
    return AdaptationExperiment(name, config,
                               change_points=kwargs.get('change_points', [500, 1000, 1500]),
                               adaptation_rate=kwargs.get('adaptation_rate', 0.1))
# Re-export commonly used classes and functions
__all__ = [
    'ExperimentStatus', 'ExperimentConfig', 'ExperimentResult',
    'Experiment', 'LearningExperiment', 'AdaptationExperiment',
    'ExperimentManager', 'create_learning_experiment',
    'create_adaptation_experiment'
]

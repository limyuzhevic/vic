"""
High-level agent interface for NLM neural simulation

This module provides a Pythonic interface for creating and controlling
neural agents with sensory-motor capabilities, learning, and adaptation.
It encapsulates the complexity of the underlying C++ implementation while
providing an intuitive, research-friendly API.
"""

import numpy as np
from typing import Dict, Any, Optional, List, Union, Callable
import warnings
from contextlib import contextmanager

from .nlm import (
    Brain, Config, AgentBrain as CAgentBrain, Vision, Audio, InternalSignals,
    SensoryPercept, Action, SimpleWorld, ActionType, MotorCommand,
    createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain,
    NLMError, NLMWarning, HAS_NUMPY, HAS_MATPLOTLIB
)


class EnhancedBrain(Brain):
    """
    Enhanced Brain class with Pythonic API and advanced features.
    
    This class extends the basic Brain interface with:
    - Method chaining and fluent interface
    - Pythonic property access
    - Context management for resource handling
    - Convenience methods for common operations
    - Type hints and comprehensive documentation
    """
    
    def configure(
        self,
        neuron_count: int = 1000,
        timestep: float = 0.001,
        timestep_count: Optional[int] = None,
        region_definitions: Optional[Dict[str, int]] = None,
        learning_rate: float = 0.01,
        weight_decay: float = 0.001,
        **kwargs: Any
    ) -> 'EnhancedBrain':
        """
        Configure the brain with simulation parameters.
        
        Args:
            neuron_count: Total number of neurons to create
            timestep: Simulation timestep in seconds
            timestep_count: Number of timesteps (overrides duration)
            region_definitions: Dict mapping region names to neuron counts
            learning_rate: Learning rate for plasticity rules
            weight_decay: Weight decay factor for stability
            **kwargs: Additional configuration parameters
            
        Returns:
            EnhancedBrain: Self for method chaining
            
        Example:
            >>> brain = EnhancedBrain.create()
            >>> brain.configure(neuron_count=500, timestep=0.001)
            >>> brain.run_simulation(steps=100)
        """
        config = self.getConfig()
        
        # Set core parameters
        config.set('neuron_count', neuron_count)
        config.set('timestep', timestep)
        config.set('learning_rate', learning_rate)
        config.set('weight_decay', weight_decay)
        
        # Calculate timestep count if not provided
        if timestep_count is None and config.has('simulation_duration'):
            duration = config.getOr('simulation_duration', 0.0)
            timestep_count = int(duration / timestep)
        
        if timestep_count:
            config.set('timestep_count', timestep_count)
        
        # Create regions if specified
        if region_definitions:
            config.set('region_definitions', region_definitions)
        
        # Apply additional parameters
        for key, value in kwargs.items():
            config.set(key, value)
        
        return self
    
    def run_simulation(
        self,
        steps: int,
        sensory_data: Optional[Union[List[float], np.ndarray, Dict[str, np.ndarray]]] = None,
        timestep: Optional[float] = None,
        callback: Optional[Callable[[int, Dict[str, Any]], None]] = None,
        verbose: bool = False
    ) -> 'EnhancedBrain':
        """
        Run simulation for specified number of steps.
        
        Args:
            steps: Number of simulation steps to run
            sensory_data: Optional sensory input data (can be numpy array or dict)
            timestep: Optional timestep override
            callback: Optional function called at each step (step, stats)
            verbose: Enable verbose output
            
        Returns:
            EnhancedBrain: Self for method chaining
            
        Example:
            >>> brain = EnhancedBrain.create()
            >>> brain.configure(neuron_count=500)
            >>> brain.run_simulation(100, verbose=True)
        """
        if timestep is not None:
            config = self.getConfig()
            original_timestep = config.getOr('timestep', 0.001)
            config.set('timestep', timestep)
        
        try:
            # Process sensory data if provided
            if sensory_data is not None:
                self._inject_sensory_data(sensory_data)
            
            # Run simulation step by step to collect metrics
            for step in range(steps):
                # Get current state for callback
                state = self._get_simulation_state()
                
                # Execute one simulation step
                self.step(step)
                
                # Call callback if provided
                if callback is not None:
                    try:
                        callback(step, state)
                    except Exception as e:
                        warnings.warn(f"Callback error at step {step}: {e}")
                
                if verbose and step % 10 == 0:
                    print(f"Step {step}/{steps}, Active neurons: {self.getActiveNeuronCount()}")
        
        finally:
            # Restore original timestep if changed
            if timestep is not None:
                config.set('timestep', original_timestep)
        
        return self
    
    def get_state(
        self,
        as_numpy: bool = True,
        include_neural_activity: bool = True,
        include_synapses: bool = True,
        region_ids: Optional[List[RegionId]] = None
    ) -> Union[Dict[str, Any], np.ndarray]:
        """
        Get current brain state.
        
        Args:
            as_numpy: Return numpy arrays if True, Python lists if False
            include_neural_activity: Include neural activity data
            include_synapses: Include synapse data
            region_ids: Specific regions to include (None for all)
            
        Returns:
            Union[Dict[str, Any], np.ndarray]: Brain state as dict or numpy array
        """
        state = {
            'simulation_step': self.getTotalSpikeCount(),
            'developmental_stage': str(self.getDevelopmentalStage()),
            'total_neurons': self.getTotalNeuronCount(),
            'total_synapses': self.getTotalSynapseCount(),
            'active_neurons': self.getActiveNeuronCount(),
            'firing_neurons': self.getFiringNeuronCount(),
            'average_firing_rate': self.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.getExcitationInhibitionRatio(),
            'total_spike_count': self.getTotalSpikeCount(),
        }
        
        if include_neural_activity:
            state['neural_activity'] = self._get_neural_activity(as_numpy)
        
        if include_synapses:
            state['synapses'] = self._get_synapses(as_numpy, region_ids)
        
        if region_ids:
            state['regions'] = self._get_regions(as_numpy, region_ids)
        
        if as_numpy and HAS_NUMPY:
            return np.array(state)
        else:
            return state
    
    def save_brain(
        self,
        filename: str,
        format: str = 'auto',
        compress: bool = False
    ) -> bool:
        """
        Save brain state to file.
        
        Args:
            filename: Output filename
            format: File format ('auto', 'json', 'pickle', 'binary')
            compress: Enable compression for binary files
            
        Returns:
            bool: True if save successful
            
        Raises:
            NLMError: If save operation fails
        """
        try:
            # Auto-detect format from extension
            if format == 'auto':
                if filename.endswith('.json'):
                    format = 'json'
                elif filename.endswith('.pkl') or filename.endswith('.pickle'):
                    format = 'pickle'
                else:
                    format = 'binary'
            
            if format == 'json':
                # Save as JSON for human-readable format
                state = self.get_state(as_numpy=False)
                import json
                with open(filename, 'w') as f:
                    json.dump(state, f, indent=2, default=str)
            elif format == 'pickle':
                # Save as pickle
                import pickle
                with open(filename, 'wb') as f:
                    pickle.dump(self, f, protocol=4 if compress else 2)
            else:
                # Save as binary using C++ save method
                success = self.save(filename)
                if not success:
                    raise NLMError("C++ save operation failed")
            
            return True
        except Exception as e:
            raise NLMError(f"Failed to save brain: {e}")
    
    def load_brain(self, filename: str) -> 'EnhancedBrain':
        """
        Load brain state from file.
        
        Args:
            filename: Input filename
            
        Returns:
            EnhancedBrain: Self for method chaining
            
        Raises:
            NLMError: If load operation fails
        """
        try:
            # Try C++ save first (binary format)
            if self.load(filename):
                return self
            
            # If that fails, try JSON format
            import json
            with open(filename, 'r') as f:
                data = json.load(f)
            
            # Reconstruct brain from data
            # This would require implementing a constructor that loads from JSON
            # For now, fall back to basic restore
            self.reset()
            
            return self
        except Exception as e:
            raise NLMError(f"Failed to load brain: {e}")
    
    def train(
        self,
        training_data: List[Dict[str, Any]],
        epochs: int = 10,
        learning_rate: float = 0.01,
        validation_split: float = 0.2,
        callback: Optional[Callable[[int, Dict[str, float]], None]] = None
    ) -> Dict[str, List[float]]:
        """
        Train brain using provided training data.
        
        Args:
            training_data: List of training examples with 'input' and 'output'
            epochs: Number of training epochs
            learning_rate: Learning rate for training
            validation_split: Fraction of data for validation
            callback: Optional training callback
            
        Returns:
            Dict[str, List[float]]: Training metrics (loss, accuracy, etc.)
        """
        if not training_data:
            raise NLMError("Training data cannot be empty")
        
        # Prepare training data
        train_size = int(len(training_data) * (1 - validation_split))
        train_data = training_data[:train_size]
        val_data = training_data[train_size:] if validation_split > 0 else []
        
        # Training metrics
        metrics = {
            'training_loss': [],
            'validation_loss': [],
            'epoch_times': []
        }
        
        # Simple training loop
        for epoch in range(epochs):
            import time
            start_time = time.time()
            
            epoch_loss = 0.0
            epoch_val_loss = 0.0
            
            # Training pass
            for example in train_data:
                sensory_input = example.get('input', {})
                target_output = example.get('output', {})
                
                # Process sensory input
                self._process_sensory_for_training(sensory_input)
                
                # Simulate
                self.step(epoch * len(train_data) + hash(str(sensory_input)) % 1000)
                
                # Compute loss (simplified)
                epoch_loss += 0.1  # Placeholder
            
            epoch_loss /= len(train_data) if train_data else 1
            metrics['training_loss'].append(epoch_loss)
            
            # Validation pass
            if val_data:
                for example in val_data:
                    sensory_input = example.get('input', {})
                    self._process_sensory_for_training(sensory_input)
                    self.step(epoch * len(val_data) + hash(str(sensory_input)) % 1000)
                    epoch_val_loss += 0.1
                
                epoch_val_loss /= len(val_data)
                metrics['validation_loss'].append(epoch_val_loss)
            
            # Record epoch time
            epoch_time = time.time() - start_time
            metrics['epoch_times'].append(epoch_time)
            
            # Call callback if provided
            if callback is not None:
                try:
                    callback(epoch, {
                        'training_loss': epoch_loss,
                        'validation_loss': epoch_val_loss if val_data else epoch_loss,
                        'epoch_time': epoch_time
                    })
                except Exception as e:
                    warnings.warn(f"Training callback error: {e}")
        
        return metrics
    
    def get_metrics(self, metrics_type: str = 'comprehensive') -> Dict[str, Any]:
        """
        Get brain performance metrics.
        
        Args:
            metrics_type: Type of metrics ('comprehensive', 'basic', 'detailed')
            
        Returns:
            Dict[str, Any]: Performance metrics
        """
        config = self.getConfig()
        
        base_metrics = {
            'brain_info': {
                'neuron_count': self.getTotalNeuronCount(),
                'synapse_count': self.getTotalSynapseCount(),
                'active_neurons': self.getActiveNeuronCount(),
                'firing_neurons': self.getFiringNeuronCount(),
                'developmental_stage': str(self.getDevelopmentalStage()),
            },
            'performance': {
                'average_firing_rate': self.getAverageFiringRate(),
                'excitation_inhibition_ratio': self.getExcitationInhibitionRatio(),
                'total_spike_count': self.getTotalSpikeCount(),
            },
            'memory': {
                'working_memory_size': len(self.getWorkingMemory().getActiveMemories()) if self.getWorkingMemory() else 0,
                'episodic_memory_size': len(self.getEpisodicMemory().getRecentMemories()) if self.getEpisodicMemory() else 0,
            }
        }
        
        if metrics_type == 'detailed':
            # Add detailed metrics
            base_metrics.update({
                'connectivity': self._analyze_connectivity(),
                'plasticity': self._analyze_plasticity(),
                'development': self._analyze_development(),
            })
        
        return base_metrics
    
    def get_beliefs(self) -> Dict[str, float]:
        """
        Get current neural beliefs/conceptual representations.
        
        Returns:
            Dict[str, float]: Belief values for different concepts
        """
        # This would extract conceptual representations from the brain
        # For now, return placeholder values based on brain state
        return {
            'novelty': self.getNoveltyLevel(),
            'curiosity': self.getCuriosityLevel(),
            'prediction_error': self.getPredictionError(),
        }
    
    # Convenience methods for common operations
    def reset(self) -> 'EnhancedBrain':
        """Reset brain to initial state."""
        Brain.reset(self)
        return self
    
    def step(self, step: int) -> 'EnhancedBrain':
        """Perform one simulation step."""
        Brain.step(self, step)
        return self
    
    @classmethod
    def create(
        cls,
        config: Optional[Config] = None,
        neuron_count: int = 1000,
        timestep: float = 0.001,
        **kwargs: Any
    ) -> 'EnhancedBrain':
        """
        Factory method to create a new EnhancedBrain instance.
        
        Args:
            config: Optional configuration object
            neuron_count: Number of neurons to create
            timestep: Simulation timestep
            **kwargs: Additional configuration parameters
            
        Returns:
            EnhancedBrain: New brain instance
        """
        if config is None:
            config = createDefaultConfig()
        
        brain = cls(config)
        brain.configure(neuron_count=neuron_count, timestep=timestep, **kwargs)
        brain.initialize()
        
        return brain
    
    @contextmanager
    def simulation_session(self, **kwargs):
        """
        Context manager for simulation sessions.
        
        Args:
            **kwargs: Configuration parameters for the session
            
        Yields:
            EnhancedBrain: Configured brain instance
        """
        original_config = self.getConfig()
        original_state = self.get_state(as_numpy=False)
        
        try:
            # Apply session configuration
            self.configure(**kwargs)
            yield self
        finally:
            # Restore original state
            self.reset()
            # Restore original state data (this is simplified)
    
    # Internal helper methods
    def _inject_sensory_data(self, sensory_data: Union[List[float], np.ndarray, Dict[str, np.ndarray]]):
        """Inject sensory data into the brain."""
        if isinstance(sensory_data, dict):
            for sensor_type, data in sensory_data.items():
                self._inject_single_sensory_type(sensor_type, data)
        elif isinstance(sensory_data, (list, np.ndarray)):
            if HAS_NUMPY:
                data_array = np.asarray(sensory_data)
            else:
                data_array = np.array(sensory_data)
            
            # Create a simple sensory input
            vision = Vision(int(np.sqrt(len(data_array))), int(np.sqrt(len(data_array))))
            if HAS_NUMPY:
                vision.setData(data_array.tolist())
            else:
                vision.setData(list(data_array))
            
            self.receiveSensoryInput(vision)
        else:
            raise NLMError(f"Unsupported sensory data type: {type(sensory_data)}")
    
    def _get_simulation_state(self) -> Dict[str, Any]:
        """Get current simulation state for monitoring."""
        return self.get_state(as_numpy=False)
    
    def _get_neural_activity(self, as_numpy: bool = True) -> Union[List[float], np.ndarray]:
        """Get neural activity data."""
        # This is a simplified version - real implementation would extract
        # actual neural activity from the brain's internal state
        activity = [0.1, 0.2, 0.3, 0.4, 0.5] * (self.getTotalNeuronCount() // 5)
        
        if as_numpy and HAS_NUMPY:
            return np.array(activity)
        else:
            return activity
    
    def _get_synapses(self, as_numpy: bool = True, region_ids: Optional[List[RegionId]] = None) -> Union[List[Dict], np.ndarray]:
        """Get synapse data."""
        synapses = [{'weight': 0.5, 'type': str(NeuronType.Excitatory)} for _ in range(self.getTotalSynapseCount())]
        
        if as_numpy and HAS_NUMPY:
            return np.array(synapses)
        else:
            return synapses
    
    def _get_regions(self, as_numpy: bool = True, region_ids: Optional[List[RegionId]] = None) -> Union[List[Dict], np.ndarray]:
        """Get region data."""
        regions = [{'name': f'Region_{i}', 'neuron_count': self.getTotalNeuronCount() // 10} for i in range(10)]
        
        if as_numpy and HAS_NUMPY:
            return np.array(regions)
        else:
            return regions
    
    def _process_sensory_for_training(self, sensory_input: Dict[str, Any]):
        """Process sensory input for training."""
        # Convert dict input to SensoryPercept
        percept = SensoryPercept()
        
        if 'vision' in sensory_input:
            vision = Vision()
            if HAS_NUMPY:
                vision.setData(np.asarray(sensory_input['vision']).flatten().tolist())
            else:
                vision.setData(sensory_input['vision'])
            percept.setVision(vision)
        
        self.receiveSensoryInput(percept)
    
    def _analyze_connectivity(self) -> Dict[str, Any]:
        """Analyze brain connectivity patterns."""
        return {
            'synapse_density': self.getTotalSynapseCount() / max(1, self.getTotalNeuronCount()),
            'region_connections': 5,  # Placeholder
        }
    
    def _analyze_plasticity(self) -> Dict[str, Any]:
        """Analyze plasticity state."""
        return {
            'hebbian_active': True,
            'stdp_active': True,
            'structural_plasticity': True,
        }
    
    def _analyze_development(self) -> Dict[str, Any]:
        """Analyze developmental stage."""
        return {
            'current_stage': str(self.getDevelopmentalStage()),
            'age': self.getConfig().getOr('developmental_age', 0.0),
        }


class AgentBrain(EnhancedBrain):
    """
    Enhanced AgentBrain class with Pythonic interface for agent-world interaction.
    
    This class provides a high-level interface for:
    - Perceiving environmental stimuli
    - Making decisions based on neural activity
    - Acting in the world
    - Managing internal states and beliefs
    """
    
    def __init__(self, brain: EnhancedBrain, world: Optional[SimpleWorld] = None):
        """
        Initialize AgentBrain with brain and optional world.
        
        Args:
            brain: The brain instance
            world: Optional world instance for interaction
        """
        super().__init__(brain.getConfig())
        self._brain = brain
        self._world = world
        self._beliefs: Dict[str, float] = {}
        self._last_action: Optional[Action] = None
    
    def perceive(
        self,
        sensory_data: Optional[Union[Dict[str, Any], np.ndarray, Vision, Audio, InternalSignals]]] = None
    ) -> Dict[str, Any]:
        """
        Perceive environmental and internal stimuli.
        
        Args:
            sensory_data: Sensory input data (supports numpy arrays directly)
            
        Returns:
            Dict[str, Any]: Percept containing all sensory information
        """
        percept = SensoryPercept()
        
        if sensory_data is None:
            # Get default percept from world if available
            if self._world:
                percept = self._world.getSensoryPercept()
        
        elif isinstance(sensory_data, dict):
            # Handle dictionary input
            for sensor_type, data in sensory_data.items():
                if sensor_type == 'vision' and isinstance(data, (np.ndarray, list)):
                    if HAS_NUMPY:
                        data_array = np.asarray(data)
                    else:
                        data_array = np.array(data)
                    
                    vision = Vision(int(np.sqrt(len(data_array))), int(np.sqrt(len(data_array))))
                    vision.setData(data_array.flatten().tolist())
                    percept.setVision(vision)
                
                elif sensor_type == 'audio' and isinstance(data, (np.ndarray, list)):
                    if HAS_NUMPY:
                        data_array = np.asarray(data)
                    else:
                        data_array = np.array(data)
                    
                    audio = Audio(int(44100), len(data_array))
                    audio.setData(data_array.tolist())
                    audio.setSampleRate(44100)
                    # Audio is not directly in SensoryPercept, would need extension
                
                elif sensor_type == 'internal' and isinstance(data, (np.ndarray, list)):
                    if HAS_NUMPY:
                        data_array = np.asarray(data)
                    else:
                        data_array = np.array(data)
                    
                    internal = InternalSignals()
                    for value in data_array:
                        internal.addSignal(float(value))
                    # InternalSignals is not directly in SensoryPercept, would need extension
        
        elif isinstance(sensory_data, (Vision, Audio, InternalSignals)):
            # Direct sensory object input
            # This would need extension to properly integrate with SensoryPercept
            pass
        
        # Process percept and inject into brain
        if self._world:
            self._world.setSensoryPercept(percept)
        
        self._brain.processSensoryInput(percept)
        
        # Update beliefs based on perception
        self._update_beliefs()
        
        return self._get_percept_dict(percept)
    
    def decide(self) -> np.ndarray:
        """
        Make a decision based on current neural activity.
        
        Returns:
            np.ndarray: Motor command as numpy array
        """
        if self._brain is None:
            raise NLMError("Brain not initialized")
        
        # Get motor command from brain
        motor_command = self._brain.produceAction()
        
        # Convert to numpy array representation
        if HAS_NUMPY:
            # Create a vector representation of the motor command
            motor_vector = np.zeros(6)  # 6 basic motor commands
            
            if motor_command.getType() == ActionType.MoveForward:
                motor_vector[0] = 1.0
            elif motor_command.getType() == ActionType.MoveBackward:
                motor_vector[1] = 1.0
            elif motor_command.getType() == ActionType.TurnLeft:
                motor_vector[2] = 1.0
            elif motor_command.getType() == ActionType.TurnRight:
                motor_vector[3] = 1.0
            elif motor_command.getType() == ActionType.Interact:
                motor_vector[4] = 1.0
            elif motor_command.getType() == ActionType.Wait:
                motor_vector[5] = 1.0
            
            return motor_vector
        else:
            return []
    
    def act(self, motor_command: Optional[Union[np.ndarray, Dict[str, float]]] = None):
        """
        Execute an action in the world.
        
        Args:
            motor_command: Motor command to execute (can be numpy array or dict)
        """
        if self._world is None:
            raise NLMError("World not initialized")
        
        if motor_command is None:
            # Make a decision first
            motor_command = self.decide()
        
        # Convert motor command to Action
        if isinstance(motor_command, dict):
            # Dict representation
            action_type = ActionType.Wait  # Default
            parameters = {}
            
            if motor_command.get('move_forward', 0) > 0.5:
                action_type = ActionType.MoveForward
            elif motor_command.get('move_backward', 0) > 0.5:
                action_type = ActionType.MoveBackward
            elif motor_command.get('turn_left', 0) > 0.5:
                action_type = ActionType.TurnLeft
            elif motor_command.get('turn_right', 0) > 0.5:
                action_type = ActionType.TurnRight
            elif motor_command.get('interact', 0) > 0.5:
                action_type = ActionType.Interact
            
            action = Action(action_type, list(parameters.values()))
        
        elif isinstance(motor_command, np.ndarray) and HAS_NUMPY:
            # Numpy array representation
            action_type = ActionType.Wait
            if motor_command[0] > 0.5:
                action_type = ActionType.MoveForward
            elif motor_command[1] > 0.5:
                action_type = ActionType.MoveBackward
            elif motor_command[2] > 0.5:
                action_type = ActionType.TurnLeft
            elif motor_command[3] > 0.5:
                action_type = ActionType.TurnRight
            elif motor_command[4] > 0.5:
                action_type = ActionType.Interact
            
            action = Action(action_type, [])
        
        else:
            raise NLMError(f"Unsupported motor command type: {type(motor_command)}")
        
        # Store last action
        self._last_action = action
        
        # Apply action to world
        if self._world:
            self._world.applyMotorCommand(MotorCommand(action.getType()), 0.0)
        
        # Get action result
        result = self._get_action_result()
        
        return result
    
    def get_beliefs(self) -> Dict[str, float]:
        """
        Get current beliefs about the environment.
        
        Returns:
            Dict[str, float]: Belief values
        """
        return self._beliefs.copy()
    
    def reset(self):
        """Reset agent for new episode."""
        self._brain.reset()
        self._beliefs.clear()
        self._last_action = None
    
    def enable_features(
        self,
        reward_modulation: bool = False,
        structural_plasticity: bool = False,
        development: bool = False,
        curiosity: bool = False
    ):
        """
        Enable or disable advanced features.
        
        Args:
            reward_modulation: Enable reward-based modulation
            structural_plasticity: Enable structural plasticity
            development: Enable developmental processes
            curiosity: Enable curiosity-driven exploration
        """
        self._brain.enableRewardModulation(reward_modulation)
        self._brain.enableStructuralPlasticity(structural_plasticity)
        self._brain.enableDevelopment(development)
        self._brain.enableCuriosity(curiosity)
    
    def get_monitoring_data(self) -> Dict[str, Any]:
        """
        Get comprehensive monitoring data for research.
        
        Returns:
            Dict[str, Any]: Monitoring data
        """
        data = {
            'brain_metrics': self._brain.get_metrics(),
            'beliefs': self._beliefs,
            'last_action': self._last_action.getType() if self._last_action else None,
            'timestamp': time.time() if 'time' in globals() else 0.0,
        }
        
        return data
    
    def _update_beliefs(self):
        """Update internal beliefs based on perception."""
        # Simplified belief update logic
        self._beliefs['novelty'] = self._brain.getNoveltyLevel()
        self._beliefs['curiosity'] = self._brain.getCuriosityLevel()
        self._beliefs['prediction_error'] = self._brain.getPredictionError()
        self._beliefs['confidence'] = 0.5  # Placeholder
    
    def _get_percept_dict(self, percept: SensoryPercept) -> Dict[str, Any]:
        """Convert SensoryPercept to dictionary."""
        result = {}
        
        if percept.getVision():
            result['vision'] = percept.getVision().getData()
        
        if percept.getAudio():
            result['audio'] = percept.getAudio().getData()
        
        if percept.getInternal():
            # Internal signals access would need extension
            pass
        
        return result
    
    def _get_action_result(self) -> Dict[str, Any]:
        """Get result of last action."""
        return {
            'success': True,
            'reward': 0.0,  # Placeholder
            'message': 'Action executed'
        }


# Factory functions
def create_agent_brain(
    brain: EnhancedBrain,
    world: Optional[SimpleWorld] = None
) -> AgentBrain:
    """
    Factory function to create an AgentBrain instance.
    
    Args:
        brain: The brain instance
        world: Optional world instance
        
    Returns:
        AgentBrain: Configured agent brain
    """
    return AgentBrain(brain, world)


# Backward compatibility aliases
Brain = EnhancedBrain

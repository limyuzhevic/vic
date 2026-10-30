# NLM Advanced Agent Scripting Capabilities
# Comprehensive agent scripting and behavior frameworks

## Overview

Advanced agent scripting capabilities enable users to create sophisticated, adaptive, and learning agents that can interact with the NLM brain simulation in complex and intelligent ways. This document provides comprehensive frameworks for agent development, including behavior scripting, learning mechanisms, adaptive strategies, and advanced interaction patterns.

## Advanced Agent Scripting Overview

The NLM advanced agent scripting system provides:

1. **Behavior Scripting**: Declarative and procedural agent behaviors
2. **Learning Frameworks**: Multiple learning mechanisms and strategies
3. **Adaptive Intelligence**: Dynamic behavior adjustment based on performance
4. **Interaction Patterns**: Advanced sensory-motor coordination and decision-making
5. **Scripting Environments**: Multiple scripting languages and paradigms
6. **Behavior Composition**: Combine and compose complex behaviors
7. **Performance Optimization**: Efficient agent execution and resource management
8. **Debugging and Testing**: Comprehensive testing and debugging tools

## Agent Scripting Architecture

The advanced agent scripting system is designed with a modular, extensible architecture:

```
+-----------------------------------+
| AgentScriptingEngine (Main Class)   |
+-----------------------------------+
         |
+-----------------------------------+
| BehaviorManager                   |
+-----------------------------------+
         |
+-----------------------------------+
| LearningEngine                    |
+-----------------------------------+
         |
+-----------------------------------+
| MemoryManager                     |
+-----------------------------------+
         |
+-----------------------------------+
| SensoryProcessor                  |
+-----------------------------------+
         |
+-----------------------------------+
| MotorController                   |
+-----------------------------------+
         |
+-----------------------------------+
| ScriptingInterface                |
+-----------------------------------+
```

## Core Agent Scripting Classes

### 1. AgentScriptingEngine

```python
import time
import threading
import json
import copy
from typing import Dict, List, Any, Optional, Callable, Union
from enum import Enum
from dataclasses import dataclass
from abc import ABC, abstractmethod
import numpy as np
import pandas as pd

class AgentBehaviorType(Enum):
    PROACTIVE = "proactive"
    REACTIVE = "reactive"
    ADAPTIVE = "adaptive"
    HYBRID = "hybrid"
    EXPLORATION = "exploration"
    EXPLOITATION = "exploitation"
    CONSERVATION = "conservation"
    COMPETITION = "competition"

class LearningMode(Enum):
    SUPERVISED = "supervised"
    UNSUPERVISED = "unsupervised"
    REINFORCEMENT = "reinforcement"
    BANDIT = "bandit"
    MULTI_ARM_BANDIT = "multi_arm_bandit"
    BAYESIAN = "bayesian"
    DEEP_REINFORCEMENT = "deep_reinforcement"
    TRANSFER = "transfer"

@dataclass
class BehaviorParameters:
    """Parameters for agent behavior configuration"""
    name: str
    behavior_type: AgentBehaviorType
    priority: float
    weight: float
    enabled: bool = True
    parameters: Dict[str, Any] = None
    conditions: List[str] = None
    dependencies: List[str] = None

@dataclass
class LearningParameters:
    """Parameters for agent learning configuration"""
    mode: LearningMode
    learning_rate: float
    exploration_rate: float
    exploitation_rate: float
    discount_factor: float
    temperature: float
    annealing_schedule: Dict[str, Any]
    regularization: Dict[str, Any]

@dataclass
class AgentState:
    """Agent state information"""
    step: int
    time: float
    position: Dict[str, float]
    velocity: Dict[str, float]
    energy: float
    health: float
    age: float
    behavior_mode: str
    exploration_rate: float
    performance_history: List[float]
    memory_usage: float
    learning_progress: float

class AgentScriptingEngine:
    """Main engine for advanced agent scripting and behavior control"""
    
    def __init__(self, brain, world, config: Optional[Dict[str, Any]] = None):
        """
        Initialize advanced agent scripting engine
        
        Args:
            brain: NLM Brain object
            world: NLM World object
            config: Agent configuration parameters
        """
        self.brain = brain
        self.world = world
        self.config = config or self._get_default_config()
        
        # Core components
        self.behavior_manager = BehaviorManager(self)
        self.learning_engine = LearningEngine(self)
        self.memory_manager = MemoryManager(self)
        self.sensory_processor = SensoryProcessor(self)
        self.motor_controller = MotorController(self)
        
        # State management
        self.current_state = AgentState(
            step=0,
            time=0.0,
            position={'x': 0.0, 'y': 0.0},
            velocity={'x': 0.0, 'y': 0.0},
            energy=100.0,
            health=100.0,
            age=0.0,
            behavior_mode='balanced',
            exploration_rate=0.5,
            performance_history=[],
            memory_usage=0.0,
            learning_progress=0.0
        )
        
        # Scripting interface
        self.scripting_interface = ScriptingInterface(self)
        
        # Event system
        self.event_listeners: Dict[str, List[Callable]] = {}
        self.event_history: List[Dict[str, Any]] = []
        
        # Performance tracking
        self.performance_metrics = {}
        self.resource_usage = {}
        
        # Initialize components
        self._initialize_components()
    
    def _get_default_config(self) -> Dict[str, Any]:
        """Get default agent configuration"""
        return {
            'agent_name': 'AdvancedAgent',
            'behavior_mode': 'hybrid',
            'learning_enabled': True,
            'memory_enabled': True,
            'adaptation_enabled': True,
            'exploration_rate': 0.3,
            'exploitation_rate': 0.7,
            'action_selection': 'epsilon_greedy',
            'reward_processing': 'standard',
            'behavior_transition': 'gradual',
            'learning_rate': 0.01,
            'discount_factor': 0.95,
            'temperature': 1.0,
            'annealing_enabled': True,
            'annealing_rate': 0.001,
            'regularization_enabled': True,
            'regularization_strength': 0.1
        }
    
    def _initialize_components(self):
        """Initialize all agent components"""
        self.behavior_manager.initialize()
        self.learning_engine.initialize()
        self.memory_manager.initialize()
        self.sensory_processor.initialize()
        self.motor_controller.initialize()
        
        # Set up event listeners
        self._setup_event_listeners()
    
    def _setup_event_listeners(self):
        """Set up event listeners for agent events"""
        self.add_event_listener('step_completed', self._on_step_completed)
        self.add_event_listener('behavior_changed', self._on_behavior_changed)
        self.add_event_listener('learning_updated', self._on_learning_updated)
        self.add_event_listener('reward_received', self._on_reward_received)
        self.add_event_listener('action_executed', self._on_action_executed)
    
    def run_step(self, step_id: int, timestep: float = 0.1):
        """
        Execute a single agent step
        
        Args:
            step_id: Current step number
            timestep: Time step for this iteration
        """
        # Update agent state
        self.current_state.step = step_id
        self.current_state.time += timestep
        self.current_state.age += timestep
        
        # Record performance metrics
        self._record_performance_metrics(step_id)
        
        # Process sensory input
        percept = self.world.getSensoryPercept()
        processed_percept = self.sensory_processor.process(percept)
        
        # Update agent memory
        self.memory_manager.update(processed_percept)
        
        # Select behavior
        behavior = self.behavior_manager.select_behavior()
        
        # Execute behavior
        action = self._execute_behavior(behavior, processed_percept)
        
        # Apply action to world
        action_result = self.world.applyMotorCommand(action, self.current_state.time)
        
        # Apply reward modulation
        reward = action_result.reward if action_result else 0.0
        self.learning_engine.process_reward(reward, action_result.success if action_result else False)
        
        # Update development
        self.learning_engine.update_development(timestep)
        
        # Update agent state
        self._update_agent_state(action_result)
        
        # Record event
        self._record_event('step_completed', {
            'step': step_id,
            'action': action,
            'reward': reward,
            'success': action_result.success if action_result else False,
            'behavior': behavior.name if behavior else None
        })
        
        # Check for behavior adaptation
        self.behavior_manager.check_adaptation_conditions()
        
        return {
            'action': action,
            'reward': reward,
            'success': action_result.success if action_result else False,
            'behavior': behavior.name if behavior else None,
            'brain_stats': self._get_brain_stats()
        }
    
    def add_behavior(self, behavior_config: BehaviorParameters):
        """Add a new behavior to the agent"""
        self.behavior_manager.add_behavior(behavior_config)
    
    def remove_behavior(self, behavior_name: str):
        """Remove a behavior from the agent"""
        self.behavior_manager.remove_behavior(behavior_name)
    
    def set_learning_mode(self, mode: LearningMode, parameters: Optional[LearningParameters] = None):
        """Set the learning mode and parameters"""
        if parameters is None:
            parameters = LearningParameters(mode, self.config['learning_rate'], 
                                            self.config['exploration_rate'], 
                                            self.config['exploitation_rate'],
                                            self.config['discount_factor'], 
                                            self.config['temperature'],
                                            {'annealing_enabled': self.config['annealing_enabled'],
                                             'annealing_rate': self.config['annealing_rate']},
                                            {'enabled': self.config['regularization_enabled'],
                                             'strength': self.config['regularization_strength']})
        
        self.learning_engine.set_mode(mode, parameters)
    
    def enable_advanced_features(self, features: List[str]):
        """Enable advanced agent features"""
        self.config['advanced_features'] = features
        
        # Enable specific feature components
        for feature in features:
            if feature == 'curiosity':
                self.learning_engine.enable_curiosity()
            elif feature == 'novelty':
                self.learning_engine.enable_novelty()
            elif feature == 'prediction':
                self.learning_engine.enable_prediction()
            elif feature == 'meta_learning':
                self.learning_engine.enable_meta_learning()
            elif feature == 'transfer_learning':
                self.learning_engine.enable_transfer_learning()
    
    def add_event_listener(self, event_type: str, callback: Callable):
        """Add an event listener"""
        if event_type not in self.event_listeners:
            self.event_listeners[event_type] = []
        self.event_listeners[event_type].append(callback)
    
    def remove_event_listener(self, event_type: str, callback: Callable):
        """Remove an event listener"""
        if event_type in self.event_listeners:
            if callback in self.event_listeners[event_type]:
                self.event_listeners[event_type].remove(callback)
    
    def _record_event(self, event_type: str, event_data: Dict[str, Any]):
        """Record an event"""
        event = {
            'timestamp': time.time(),
            'type': event_type,
            'data': event_data
        }
        
        self.event_history.append(event)
        
        # Notify event listeners
        if event_type in self.event_listeners:
            for callback in self.event_listeners[event_type]:
                try:
                    callback(event)
                except Exception as e:
                    print(f"Event listener error: {e}")
    
    def _record_performance_metrics(self, step_id: int):
        """Record performance metrics for current step"""
        brain_stats = self._get_brain_stats()
        
        metrics = {
            'step': step_id,
            'timestamp': time.time(),
            'brain_stats': brain_stats,
            'agent_state': {
                'exploration_rate': self.current_state.exploration_rate,
                'behavior_mode': self.current_state.behavior_mode,
                'energy': self.current_state.energy,
                'health': self.current_state.health,
                'age': self.current_state.age
            },
            'learning_stats': self.learning_engine.get_stats(),
            'memory_stats': self.memory_manager.get_stats()
        }
        
        self.performance_metrics[step_id] = metrics
    
    def _get_brain_stats(self) -> Dict[str, Any]:
        """Get current brain statistics"""
        return {
            'neurons': self.brain.getTotalNeuronCount(),
            'synapses': self.brain.getTotalSynapseCount(),
            'spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount()
        }
    
    def _update_agent_state(self, action_result):
        """Update agent state based on action result"""
        if action_result:
            # Update position based on action
            if action_result.action_type == 'MoveForward':
                self.current_state.velocity['x'] += 0.1
            elif action_result.action_type == 'MoveBackward':
                self.current_state.velocity['x'] -= 0.1
            elif action_result.action_type == 'TurnLeft':
                self.current_state.velocity['y'] -= 0.1
            elif action_result.action_type == 'TurnRight':
                self.current_state.velocity['y'] += 0.1
            
            # Update position
            self.current_state.position['x'] += self.current_state.velocity['x'] * 0.1
            self.current_state.position['y'] += self.current_state.velocity['y'] * 0.1
            
            # Apply energy cost
            self.current_state.energy *= 0.99
            
            # Apply health updates based on success
            if action_result.success:
                self.current_state.health += 1.0
            else:
                self.current_state.health -= 2.0
            
            # Update performance history
            self.current_state.performance_history.append(
                action_result.reward if action_result else 0.0
            )
            
            # Limit performance history
            if len(self.current_state.performance_history) > 100:
                self.current_state.performance_history.pop(0)
    
    def _execute_behavior(self, behavior, percept):
        """Execute a selected behavior"""
        if behavior is None:
            return self.motor_controller.select_default_action()
        
        # Generate action based on behavior
        action = behavior.execute(self.current_state, percept)
        
        # Validate action
        action = self.motor_controller.validate_action(action)
        
        return action
    
    def _on_step_completed(self, event):
        """Handle step completion event"""
        pass
    
    def _on_behavior_changed(self, event):
        """Handle behavior change event"""
        print(f"Behavior changed to: {event['data'].get('behavior')}")
    
    def _on_learning_updated(self, event):
        """Handle learning update event"""
        self.current_state.learning_progress = self.learning_engine.get_progress()
    
    def _on_reward_received(self, event):
        """Handle reward reception event"""
        reward = event['data'].get('reward', 0.0)
        self.current_state.performance_history.append(reward)
        if len(self.current_state.performance_history) > 100:
            self.current_state.performance_history.pop(0)
    
    def _on_action_executed(self, event):
        """Handle action execution event"""
        pass
```

### 2. BehaviorManager

```python
class BehaviorManager:
    """Manage agent behaviors and behavior selection"""
    
    def __init__(self, agent_engine):
        self.agent_engine = agent_engine
        self.behaviors: Dict[str, Behavior] = {}
        self.behavior_weights: Dict[str, float] = {}
        self.current_behavior = None
        self.behavior_history: List[str] = []
        self.behavior_performance: Dict[str, List[float]] = {}
        self.adaptation_rules = {}
        
    def initialize(self):
        """Initialize behavior manager"""
        self._load_default_behaviors()
        self._setup_adaptation_rules()
    
    def _load_default_behaviors(self):
        """Load default behaviors"""
        # Add exploration behavior
        self.add_behavior(ExplorationBehavior(self.agent_engine))
        
        # Add exploitation behavior
        self.add_behavior(ExploitationBehavior(self.agent_engine))
        
        # Add curiosity behavior
        self.add_behavior(CuriosityBehavior(self.agent_engine))
        
        # Add novelty seeking behavior
        self.add_behavior(NoveltySeekingBehavior(self.agent_engine))
        
        # Add development behavior
        self.add_behavior(DevelopmentBehavior(self.agent_engine))
        
        # Add conservation behavior
        self.add_behavior(ConservationBehavior(self.agent_engine))
    
    def _setup_adaptation_rules(self):
        """Set up behavior adaptation rules"""
        self.adaptation_rules = {
            'performance_threshold': 0.5,
            'exploration_decay_rate': 0.95,
            'exploitation_growth_rate': 1.05,
            'behavior_switch_threshold': 0.7,
            'learning_progress_threshold': 0.3
        }
    
    def add_behavior(self, behavior: 'Behavior'):
        """Add a behavior to the manager"""
        self.behaviors[behavior.name] = behavior
        self.behavior_weights[behavior.name] = 1.0
        self.behavior_performance[behavior.name] = []
    
    def remove_behavior(self, behavior_name: str):
        """Remove a behavior from the manager"""
        if behavior_name in self.behaviors:
            del self.behaviors[behavior_name]
        if behavior_name in self.behavior_weights:
            del self.behavior_weights[behavior_name]
        if behavior_name in self.behavior_performance:
            del self.behavior_performance[behavior_name]
    
    def select_behavior(self) -> Optional['Behavior']:
        """Select the best behavior based on current conditions"""
        # Get current agent state
        state = self.agent_engine.current_state
        
        # Calculate behavior scores based on conditions
        behavior_scores = {}
        for name, behavior in self.behaviors.items():
            if behavior.is_enabled():
                score = self._calculate_behavior_score(behavior, state)
                behavior_scores[name] = score
        
        if not behavior_scores:
            return None
        
        # Normalize scores
        total_score = sum(behavior_scores.values())
        if total_score == 0:
            # If all scores are zero, use equal weights
            for name in behavior_scores:
                behavior_scores[name] = 1.0 / len(behavior_scores)
        else:
            # Normalize by total score
            for name in behavior_scores:
                behavior_scores[name] /= total_score
        
        # Update behavior weights based on performance
        self._update_behavior_weights()
        
        # Select behavior based on scores and weights
        if self.agent_engine.config.get('action_selection') == 'epsilon_greedy':
            selected_behavior = self._epsilon_greedy_selection(behavior_scores)
        elif self.agent_engine.config.get('action_selection') == 'softmax':
            selected_behavior = self._softmax_selection(behavior_scores)
        else:  # default to epsilon-greedy
            selected_behavior = self._epsilon_greedy_selection(behavior_scores)
        
        # Update current behavior
        if self.current_behavior != selected_behavior:
            old_behavior = self.current_behavior
            self.current_behavior = selected_behavior
            self.behavior_history.append(selected_behavior.name if selected_behavior else 'none')
            
            # Record behavior change event
            self.agent_engine._record_event('behavior_changed', {
                'old_behavior': old_behavior.name if old_behavior else None,
                'new_behavior': selected_behavior.name if selected_behavior else None
            })
        
        return selected_behavior
    
    def _calculate_behavior_score(self, behavior: 'Behavior', state: AgentState) -> float:
        """Calculate score for a behavior based on current state"""
        base_score = behavior.get_base_score()
        
        # Adjust score based on state conditions
        score = base_score
        
        # Consider exploration vs exploitation balance
        if state.exploration_rate > 0.5:
            if behavior.name in ['exploration', 'curiosity', 'novelty']:
                score *= 1.5
        
        # Consider performance history
        if state.performance_history:
            recent_performance = sum(state.performance_history[-10:]) / min(len(state.performance_history), 10)
            behavior_perf = self.behavior_performance.get(behavior.name, [])
            if behavior_perf:
                avg_behavior_perf = sum(behavior_perf) / len(behavior_perf)
                score *= (1.0 + avg_behavior_perf)
        
        # Check for special conditions
        score *= behavior.get_condition_multiplier(state)
        
        return max(0.0, score)
    
    def _epsilon_greedy_selection(self, behavior_scores: Dict[str, float]) -> Optional['Behavior']:
        """Select behavior using epsilon-greedy strategy"""
        epsilon = 1.0 - self.agent_engine.current_state.exploration_rate
        
        if np.random.random() < epsilon:
            # Exploit: select behavior with highest score
            return self.behaviors.get(max(behavior_scores, key=behavior_scores.get))
        else:
            # Explore: select behavior randomly
            return self.behaviors.get(np.random.choice(list(behavior_scores.keys())))
    
    def _softmax_selection(self, behavior_scores: Dict[str, float]) -> Optional['Behavior']:
        """Select behavior using softmax selection"""
        temperature = self.agent_engine.config.get('temperature', 1.0)
        
        # Convert scores to probabilities
        scores = np.array(list(behavior_scores.values()))
        exp_scores = np.exp(scores / temperature)
        probabilities = exp_scores / exp_scores.sum()
        
        # Select behavior based on probabilities
        behavior_names = list(behavior_scores.keys())
        selected_name = np.random.choice(behavior_names, p=probabilities)
        
        return self.behaviors.get(selected_name)
    
    def _update_behavior_weights(self):
        """Update behavior weights based on performance"""
        for name, behavior in self.behaviors.items():
            if behavior.is_enabled():
                performance = self.behavior_performance.get(name, [])
                if performance:
                    avg_performance = sum(performance) / len(performance)
                    # Adjust weight based on performance
                    self.behavior_weights[name] = max(0.1, min(2.0, 
                        self.behavior_weights[name] * (1.0 + avg_performance)))
    
    def check_adaptation_conditions(self):
        """Check for conditions that require behavior adaptation"""
        state = self.agent_engine.current_state
        
        # Check performance threshold
        if state.performance_history:
            recent_performance = sum(state.performance_history[-10:]) / min(len(state.performance_history), 10)
            if recent_performance < self.adaptation_rules['performance_threshold']:
                self._trigger_behavior_adaptation('poor_performance')
        
        # Check learning progress
        if state.learning_progress > self.adaptation_rules['learning_progress_threshold']:
            self._trigger_behavior_adaptation('learning_progress')
        
        # Check exploration-exploitation balance
        if abs(state.exploration_rate - 0.5) < 0.1:
            self._trigger_behavior_adaptation('balanced_exploration')
    
    def _trigger_behavior_adaptation(self, reason: str):
        """Trigger behavior adaptation"""
        # Adapt exploration rate based on reason
        if reason == 'poor_performance':
            self.agent_engine.current_state.exploration_rate = min(1.0, 
                self.agent_engine.current_state.exploration_rate * 1.1)
            self.agent_engine.current_state.exploitation_rate = max(0.0,
                self.agent_engine.current_state.exploitation_rate * 0.9)
        
        elif reason == 'learning_progress':
            self.agent_engine.current_state.exploration_rate *= self.adaptation_rules['exploration_decay_rate']
            self.agent_engine.current_state.exploitation_rate *= self.adaptation_rules['exploitation_growth_rate']
        
        # Record adaptation event
        self.agent_engine._record_event('behavior_adaptation', {
            'reason': reason,
            'exploration_rate': self.agent_engine.current_state.exploration_rate,
            'exploitation_rate': self.agent_engine.current_state.exploitation_rate
        })
```

### 3. LearningEngine

```python
class LearningEngine:
    """Manage agent learning and adaptation"""
    
    def __init__(self, agent_engine):
        self.agent_engine = agent_engine
        self.current_mode = LearningMode.REINFORCEMENT
        self.learning_parameters: LearningParameters = LearningParameters(
            self.current_mode,
            agent_engine.config.get('learning_rate', 0.01),
            agent_engine.config.get('exploration_rate', 0.3),
            agent_engine.config.get('exploitation_rate', 0.7),
            agent_engine.config.get('discount_factor', 0.95),
            agent_engine.config.get('temperature', 1.0),
            {'annealing_enabled': agent_engine.config.get('annealing_enabled', True),
             'annealing_rate': agent_engine.config.get('annealing_rate', 0.001)},
            {'enabled': agent_engine.config.get('regularization_enabled', True),
             'strength': agent_engine.config.get('regularization_strength', 0.1)}
        )
        
        # Learning components
        self.reward_processor = RewardProcessor(self)
        self.value_function = ValueFunction(self)
        self.policy = Policy(self)
        self.meta_learner = MetaLearner(self)
        
        # Learning statistics
        self.learning_stats = {
            'reward_history': [],
            'value_loss_history': [],
            'policy_loss_history': [],
            'exploration_history': [],
            'exploitation_history': [],
            'adaptation_events': []
        }
        
        # Advanced learning flags
        self.enabled_features = {
            'curiosity': False,
            'novelty': False,
            'prediction': False,
            'meta_learning': False,
            'transfer_learning': False
        }
    
    def initialize(self):
        """Initialize learning engine"""
        self._initialize_learning_components()
    
    def _initialize_learning_components(self):
        """Initialize learning components based on configuration"""
        if self.enabled_features['curiosity']:
            self.curiosity_module = CuriosityModule(self)
        
        if self.enabled_features['novelty']:
            self.novelty_module = NoveltyModule(self)
        
        if self.enabled_features['prediction']:
            self.prediction_module = PredictionModule(self)
        
        if self.enabled_features['meta_learning']:
            self.meta_learning_module = MetaLearningModule(self)
        
        if self.enabled_features['transfer_learning']:
            self.transfer_learning_module = TransferLearningModule(self)
    
    def set_mode(self, mode: LearningMode, parameters: Optional[LearningParameters] = None):
        """Set learning mode and parameters"""
        if parameters is not None:
            self.current_mode = mode
            self.learning_parameters = parameters
        
        # Reinitialize learning components
        self._initialize_learning_components()
    
    def process_reward(self, reward: float, success: bool):
        """Process reward and update learning"""
        # Process reward through reward processor
        processed_reward = self.reward_processor.process(reward, success)
        
        # Update value function
        self.value_function.update(processed_reward)
        
        # Update policy
        self.policy.update(processed_reward)
        
        # Record reward
        self.learning_stats['reward_history'].append(processed_reward)
        
        # Trigger adaptation events
        if self.enabled_features['curiosity']:
            curiosity_reward = self.curiosity_module.calculate_curiosity_reward()
            self.process_reward(curiosity_reward, success)
        
        if self.enabled_features['novelty']:
            novelty_reward = self.novelty_module.calculate_novelty_reward()
            self.process_reward(novelty_reward, success)
        
        # Record event
        self.agent_engine._record_event('learning_updated', {
            'reward': reward,
            'processed_reward': processed_reward,
            'success': success,
            'mode': self.current_mode.value
        })
    
    def update_development(self, timestep: float):
        """Update development system"""
        # Update development progress
        development_progress = self._calculate_development_progress()
        self.agent_engine.current_state.learning_progress = development_progress
        
        # Apply annealing if enabled
        if self.learning_parameters.annealing_schedule['annealing_enabled']:
            self._apply_annealing(timestep)
        
        # Apply regularization if enabled
        if self.learning_parameters.regularization['enabled']:
            self._apply_regularization()
    
    def enable_curiosity(self):
        """Enable curiosity-driven learning"""
        self.enabled_features['curiosity'] = True
        if hasattr(self, 'curiosity_module'):
            self.curiosity_module.enable()
    
    def enable_novelty(self):
        """Enable novelty detection and learning"""
        self.enabled_features['novelty'] = True
        if hasattr(self, 'novelty_module'):
            self.novelty_module.enable()
    
    def enable_prediction(self):
        """Enable prediction-based learning"""
        self.enabled_features['prediction'] = True
        if hasattr(self, 'prediction_module'):
            self.prediction_module.enable()
    
    def enable_meta_learning(self):
        """Enable meta-learning capabilities"""
        self.enabled_features['meta_learning'] = True
        if hasattr(self, 'meta_learning_module'):
            self.meta_learning_module.enable()
    
    def enable_transfer_learning(self):
        """Enable transfer learning capabilities"""
        self.enabled_features['transfer_learning'] = True
        if hasattr(self, 'transfer_learning_module'):
            self.transfer_learning_module.enable()
    
    def get_stats(self) -> Dict[str, Any]:
        """Get learning statistics"""
        return {
            'current_mode': self.current_mode.value,
            'learning_rate': self.learning_parameters.learning_rate,
            'exploration_rate': self.learning_parameters.exploration_rate,
            'exploitation_rate': self.learning_parameters.exploitation_rate,
            'reward_history': self.learning_stats['reward_history'][-100:],  # Last 100 rewards
            'adaptation_events': self.learning_stats['adaptation_events'],
            'value_function_loss': self.value_function.get_loss() if hasattr(self.value_function, 'get_loss') else 0.0,
            'policy_loss': self.policy.get_loss() if hasattr(self.policy, 'get_loss') else 0.0
        }
    
    def get_progress(self) -> float:
        """Get overall learning progress"""
        if not self.learning_stats['reward_history']:
            return 0.0
        
        # Calculate progress based on reward history
        recent_rewards = self.learning_stats['reward_history'][-50:]
        if len(recent_rewards) < 2:
            return 0.0
        
        # Calculate improvement trend
        rewards_array = np.array(recent_rewards)
        x = np.arange(len(rewards_array))
        
        # Linear regression to calculate trend
        slope = np.polyfit(x, rewards_array, 1)[0]
        
        # Normalize slope to 0-1 range
        progress = min(1.0, max(0.0, (slope + 0.5) / 1.0))
        
        return progress
    
    def _calculate_development_progress(self) -> float:
        """Calculate development progress"""
        # This would depend on brain development system
        return 0.5  # Placeholder
    
    def _apply_annealing(self, timestep: float):
        """Apply learning rate annealing"""
        if timestep > 0:
            # Decay learning rate
            self.learning_parameters.learning_rate *= (
                1.0 - self.learning_parameters.annealing_schedule['annealing_rate'] * timestep
            )
            
            # Decay exploration rate
            self.learning_parameters.exploration_rate *= (
                1.0 - self.learning_parameters.annealing_schedule['annealing_rate'] * timestep
            )
    
    def _apply_regularization(self):
        """Apply regularization to learning"""
        # Add regularization term to loss function
        pass
```

### 4. MemoryManager

```python
class MemoryManager:
    """Manage agent memory and information storage"""
    
    def __init__(self, agent_engine):
        self.agent_engine = agent_engine
        
        # Memory components
        self.working_memory = WorkingMemory(self)
        self.episodic_memory = EpisodicMemory(self)
        self.semantic_memory = SemanticMemory(self)
        self.procedural_memory = ProceduralMemory(self)
        self.associative_memory = AssociativeMemory(self)
        
        # Memory statistics
        self.memory_stats = {
            'working_memory_usage': 0.0,
            'episodic_memory_usage': 0.0,
            'semantic_memory_usage': 0.0,
            'procedural_memory_usage': 0.0,
            'associative_memory_usage': 0.0,
            'total_memory_usage': 0.0,
            'memory_efficiency': 0.0
        }
        
        # Memory configuration
        self.memory_config = {
            'working_memory_capacity': agent_engine.config.get('memory_working_memory_capacity', 100),
            'episodic_memory_capacity': agent_engine.config.get('memory_episodic_memory_capacity', 1000),
            'semantic_memory_capacity': agent_engine.config.get('memory_semantic_memory_capacity', 500),
            'procedural_memory_capacity': agent_engine.config.get('memory_procedural_memory_capacity', 200),
            'associative_memory_capacity': agent_engine.config.get('memory_associative_memory_capacity', 300),
            'consolidation_rate': agent_engine.config.get('memory_consolidation_rate', 0.01),
            'decay_rate': agent_engine.config.get('memory_decay_rate', 0.001)
        }
    
    def initialize(self):
        """Initialize memory manager"""
        self.working_memory.initialize()
        self.episodic_memory.initialize()
        self.semantic_memory.initialize()
        self.procedural_memory.initialize()
        self.associative_memory.initialize()
    
    def update(self, percept):
        """Update memories with new percept"""
        # Update working memory
        self.working_memory.update(percept)
        
        # Store in episodic memory
        self.episodic_memory.store(percept, self.agent_engine.current_state)
        
        # Extract patterns for semantic memory
        patterns = self._extract_patterns(percept)
        self.semantic_memory.store(patterns)
        
        # Update procedural memory based on actions
        if hasattr(percept, 'action'):
            self.procedural_memory.update(percept.action, percept.reward)
        
        # Update associative memory
        self.associative_memory.associate(percept, self._get_context())
        
        # Update memory statistics
        self._update_memory_stats()
    
    def _extract_patterns(self, percept) -> List[Dict[str, Any]]:
        """Extract patterns from percept for semantic memory"""
        patterns = []
        
        # Extract spatial patterns
        if hasattr(percept, 'vision') and percept.vision is not None:
            vision_data = percept.vision
            for row in vision_data:
                for cell in row:
                    if cell > 0.5:  # Consider bright cells as patterns
                        patterns.append({
                            'type': 'spatial',
                            'value': cell,
                            'timestamp': percept.timestamp
                        })
        
        # Extract temporal patterns
        patterns.extend(self._extract_temporal_patterns())
        
        return patterns
    
    def _get_context(self) -> Dict[str, Any]:n        """Get current agent context for associative memory"""
        return {
            'step': self.agent_engine.current_state.step,
            'time': self.agent_engine.current_state.time,
            'position': self.agent_engine.current_state.position,
            'velocity': self.agent_engine.current_state.velocity,
            'energy': self.agent_engine.current_state.energy,
            'health': self.agent_engine.current_state.health,
            'behavior_mode': self.agent_engine.current_state.behavior_mode,
            'exploration_rate': self.agent_engine.current_state.exploration_rate
        }
    
    def _update_memory_stats(self):
        """Update memory statistics"""
        self.memory_stats['working_memory_usage'] = self.working_memory.get_usage()
        self.memory_stats['episodic_memory_usage'] = self.episodic_memory.get_usage()
        self.memory_stats['semantic_memory_usage'] = self.semantic_memory.get_usage()
        self.memory_stats['procedural_memory_usage'] = self.procedural_memory.get_usage()
        self.memory_stats['associative_memory_usage'] = self.associative_memory.get_usage()
        
        # Calculate total memory usage
        total_usage = (
            self.memory_stats['working_memory_usage'] +
            self.memory_stats['episodic_memory_usage'] +
            self.memory_stats['semantic_memory_usage'] +
            self.memory_stats['procedural_memory_usage'] +
            self.memory_stats['associative_memory_usage']
        )
        self.memory_stats['total_memory_usage'] = total_usage
        
        # Calculate memory efficiency
        if total_usage > 0:
            efficiency = (self.working_memory.get_effectiveness() +
                         self.episodic_memory.get_effectiveness() +
                         self.semantic_memory.get_effectiveness() +
                         self.procedural_memory.get_effectiveness() +
                         self.associative_memory.get_effectiveness()) / 5.0
            self.memory_stats['memory_efficiency'] = efficiency
    
    def get_stats(self) -> Dict[str, Any]:
        """Get memory statistics"""
        return self.memory_stats
```

### 5. SensoryProcessor

```python
class SensoryProcessor:
    """Process sensory inputs and extract features"""
    
    def __init__(self, agent_engine):
        self.agent_engine = agent_engine
        
        # Feature extractors
        self.vision_extractor = VisionFeatureExtractor()
        self.audio_extractor = AudioFeatureExtractor()
        self.internal_extractor = InternalSignalExtractor()
        self.proprioception_extractor = ProprioceptionExtractor()
        
        # Feature processors
        self.feature_computer = FeatureComputer()
        self.feature_selector = FeatureSelector()
        self.feature_fuser = FeatureFuser()
        
        # Configuration
        self.config = {
            'vision_enabled': True,
            'audio_enabled': True,
            'internal_enabled': True,
            'proprioception_enabled': True,
            'feature_reduction_enabled': True,
            'noise_reduction_enabled': True,
            'normalization_enabled': True,
            'dimensionality_reduction': True
        }
    
    def initialize(self):
        """Initialize sensory processor"""
        pass
    
    def process(self, percept) -> Dict[str, Any]:
        """
        Process sensory input and extract features
        
        Args:
            percept: Raw sensory input
        
        Returns:
            Processed sensory features
        """
        if percept is None:
            return self._get_empty_features()
        
        # Extract features from each modality
        features = {}
        
        if self.config['vision_enabled'] and hasattr(percept, 'vision') and percept.vision is not None:
            features['vision'] = self.vision_extractor.extract(percept.vision)
        
        if self.config['audio_enabled'] and hasattr(percept, 'audio') and percept.audio is not None:
            features['audio'] = self.audio_extractor.extract(percept.audio)
        
        if self.config['internal_enabled'] and hasattr(percept, 'internal') and percept.internal is not None:
            features['internal'] = self.internal_extractor.extract(percept.internal)
        
        if self.config['proprioception_enabled'] and hasattr(percept, 'proprioception') and percept.proprioception is not None:
            features['proprioception'] = self.proprioception_extractor.extract(percept.proprioception)
        
        # Process and fuse features
        processed_features = self._process_features(features)
        
        # Add metadata
        processed_features['timestamp'] = percept.getTimestamp() if hasattr(percept, 'getTimestamp') else time.time()
        processed_features['sensory_type'] = self._determine_sensory_type(percept)
        
        return processed_features
    
    def _process_features(self, features: Dict[str, Any]) -> Dict[str, Any]:
        """Process and fuse features"""
        # Compute features
        computed_features = {}
        for modality, feature_data in features.items():
            computed_features[modality] = self.feature_computer.compute(feature_data)
        
        # Select important features
        selected_features = {}
        for modality, feature_vector in computed_features.items():
            selected_features[modality] = self.feature_selector.select(
                feature_vector, modality
            )
        
        # Fuse features
        fused_features = self.feature_fuser.fuse(selected_features)
        
        return fused_features
    
    def _get_empty_features(self) -> Dict[str, Any]:
        """Get empty features when no sensory input is available"""
        return {
            'timestamp': time.time(),
            'sensory_type': 'none',
            'features': {}
        }
    
    def _determine_sensory_type(self, percept) -> str:
        """Determine the type of sensory input"""
        if hasattr(percept, 'vision') and percept.vision is not None:
            return 'vision'
        elif hasattr(percept, 'audio') and percept.audio is not None:
            return 'audio'
        elif hasattr(percept, 'internal') and percept.internal is not None:
            return 'internal'
        elif hasattr(percept, 'proprioception') and percept.proprioception is not None:
            return 'proprioception'
        else:
            return 'none'
```

### 6. MotorController

```python
class MotorController:
    """Control agent actions and motor output"""
    
    def __init__(self, agent_engine):
        self.agent_engine = agent_engine
        
        # Action generators
        self.action_generators = {
            'exploration': ExplorationActionGenerator(self),
            'exploitation': ExploitationActionGenerator(self),
            'curiosity': CuriosityActionGenerator(self),
            'novelty': NoveltyActionGenerator(self),
            'development': DevelopmentActionGenerator(self)
        }
        
        # Action validators
        self.action_validators = {
            'bounds': BoundsValidator(self),
            'safety': SafetyValidator(self),
            'energy': EnergyValidator(self)
        }
        
        # Action executors
        self.action_executors = {
            'default': DefaultActionExecutor(self),
            'adaptive': AdaptiveActionExecutor(self),
            'learning': LearningActionExecutor(self)
        }
        
        # Action history
        self.action_history: List[Dict[str, Any]] = []
        
        # Action statistics
        self.action_stats = {
            'total_actions': 0,
            'successful_actions': 0,
            'failed_actions': 0,
            'action_distribution': {},
            'action_effectiveness': {}
        }
    
    def initialize(self):
        """Initialize motor controller"""
        pass
    
    def select_action(self, percept, behavior) -> Any:
        """
        Select action based on percept and behavior
        
        Args:
            percept: Processed sensory percept
            behavior: Selected behavior
        
        Returns:
            Selected action
        """
        # Get action generator based on behavior
        action_generator = self.action_generators.get(
            behavior.name if behavior else 'default',
            self.action_generators['default']
        )
        
        # Generate action
        action = action_generator.generate_action(percept)
        
        # Validate action
        action = self._validate_action(action)
        
        return action
    
    def _validate_action(self, action) -> Any:
        """Validate action against constraints"""
        # Check bounds
        if 'bounds' in self.action_validators:
            action = self.action_validators['bounds'].validate(action)
        
        # Check safety
        if 'safety' in self.action_validators:
            action = self.action_validators['safety'].validate(action)
        
        # Check energy
        if 'energy' in self.action_validators:
            action = self.action_validators['energy'].validate(action)
        
        return action
    
    def execute_action(self, action) -> Dict[str, Any]:
        """
        Execute action and return result
        
        Args:
            action: Action to execute
        
        Returns:
            Action execution result
        """
        # Use appropriate action executor
        executor = self.action_executors.get('adaptive', self.action_executors['default'])
        
        # Execute action
        result = executor.execute(action)
        
        # Record action
        self._record_action(action, result)
        
        # Update statistics
        self._update_action_stats(action, result)
        
        return result
    
    def _record_action(self, action, result: Dict[str, Any]):
        """Record action and result"""
        action_record = {
            'timestamp': time.time(),
            'action': action,
            'result': result,
            'agent_state': self.agent_engine.current_state.__dict__.copy()
        }
        
        self.action_history.append(action_record)
        
        # Keep only recent history
        if len(self.action_history) > 1000:
            self.action_history.pop(0)
    
    def _update_action_stats(self, action, result: Dict[str, Any]):
        """Update action statistics"""
        # Update total actions
        self.action_stats['total_actions'] += 1
        
        # Update action distribution
        action_key = str(action)
        self.action_stats['action_distribution'][action_key] = \
            self.action_stats['action_distribution'].get(action_key, 0) + 1
        
        # Update action effectiveness
        success = result.get('success', False)
        reward = result.get('reward', 0.0)
        
        if success:
            self.action_stats['successful_actions'] += 1
        else:
            self.action_stats['failed_actions'] += 1
        
        # Update action effectiveness
        if action_key not in self.action_stats['action_effectiveness']:
            self.action_stats['action_effectiveness'][action_key] = {
                'success_rate': 0.0,
                'average_reward': 0.0,
                'count': 0
            }
        
        effectiveness = self.action_stats['action_effectiveness'][action_key]
        effectiveness['success_rate'] = ((effectiveness['success_rate'] * effectiveness['count']) +
                                       (1.0 if success else 0.0)) / (effectiveness['count'] + 1)
        effectiveness['average_reward'] = ((effectiveness['average_reward'] * effectiveness['count']) +
                                         reward) / (effectiveness['count'] + 1)
        effectiveness['count'] += 1
    
    def select_default_action(self) -> Any:
        """Select default action when no behavior is specified"""
        return self.action_executors['default'].execute(None)
```

### 7. ScriptingInterface

```python
class ScriptingInterface:
    """Scripting interface for agent behavior customization"""
    
    def __init__(self, agent_engine):
        self.agent_engine = agent_engine
        self.scripts: Dict[str, 'Script'] = {}
        self.script_functions: Dict[str, Callable] = {}
        self.global_variables: Dict[str, Any] = {}
        self.script_context = {}
        
        # Initialize with default functions
        self._register_default_functions()
    
    def _register_default_functions(self):
        """Register default scripting functions"""
        # State access functions
        self.register_function('get_state', self._get_state)
        self.register_function('get_position', self._get_position)
        self.register_function('get_energy', self._get_energy)
        self.register_function('get_behavior', self._get_behavior)
        
        # Action functions
        self.register_function('execute_action', self._execute_action)
        self.register_function('validate_action', self._validate_action)
        
        # Learning functions
        self.register_function('get_learning_stats', self._get_learning_stats)
        self.register_function('set_learning_mode', self._set_learning_mode)
        
        # Memory functions
        self.register_function('get_memory_stats', self._get_memory_stats)
        self.register_function('store_memory', self._store_memory)
        
        # Behavior functions
        self.register_function('add_behavior', self._add_behavior)
        self.register_function('remove_behavior', self._remove_behavior)
        self.register_function('select_behavior', self._select_behavior)
        
        # Event functions
        self.register_function('add_event_listener', self._add_event_listener)
        self.register_function('remove_event_listener', self._remove_event_listener)
    
    def register_function(self, name: str, function: Callable):
        """Register a scripting function"""
        self.script_functions[name] = function
    
    def unregister_function(self, name: str):
        """Unregister a scripting function"""
        if name in self.script_functions:
            del self.script_functions[name]
    
    def execute_script(self, script_name: str, parameters: Dict[str, Any] = None) -> Any:
        """
        Execute a script
        
        Args:
            script_name: Name of script to execute
            parameters: Parameters to pass to script
        
        Returns:
            Script execution result
        """
        if script_name not in self.scripts:
            raise ValueError(f"Script not found: {script_name}")
        
        script = self.scripts[script_name]
        
        # Set up script context
        self._setup_script_context(parameters)
        
        # Execute script
        try:
            result = script.execute(self.script_context)
            return result
        except Exception as e:
            raise RuntimeError(f"Script execution failed: {e}")
    
    def load_script(self, script_name: str, script_content: str):
        """
        Load a script from content
        
        Args:
            script_name: Name of script
            script_content: Script content (Python code)
        """
        script = Script(script_name, script_content)
        self.scripts[script_name] = script
    
    def unload_script(self, script_name: str):
        """Unload a script"""
        if script_name in self.scripts:
            del self.scripts[script_name]
    
    def list_scripts(self) -> List[str]:
        """List all loaded scripts"""
        return list(self.scripts.keys())
    
    def _setup_script_context(self, parameters: Optional[Dict[str, Any]] = None):
        """Set up script execution context"""
        # Copy global variables
        self.script_context = self.global_variables.copy()
        
        # Add parameters
        if parameters:
            self.script_context['parameters'] = parameters
        
        # Add script functions
        for name, func in self.script_functions.items():
            self.script_context[name] = func
        
        # Add agent-specific variables
        self.script_context['agent'] = self.agent_engine
        self.script_context['brain'] = self.agent_engine.brain
        self.script_context['world'] = self.agent_engine.world
        self.script_context['step'] = self.agent_engine.current_state.step
        self.script_context['time'] = self.agent_engine.current_state.time
        self.script_context['position'] = self.agent_engine.current_state.position
        self.script_context['energy'] = self.agent_engine.current_state.energy
    
    # Script function implementations
    def _get_state(self) -> Dict[str, Any]:
        return self.agent_engine.current_state.__dict__.copy()
    
    def _get_position(self) -> Dict[str, Any]:
        return self.agent_engine.current_state.position.copy()
    
    def _get_energy(self) -> float:
        return self.agent_engine.current_state.energy
    
    def _get_behavior(self) -> str:
        return self.agent_engine.current_state.behavior_mode
    
    def _execute_action(self, action) -> Dict[str, Any]:
        return self.agent_engine.motor_controller.execute_action(action)
    
    def _validate_action(self, action) -> bool:
        # Simplified validation
        return action is not None
    
    def _get_learning_stats(self) -> Dict[str, Any]:
        return self.agent_engine.learning_engine.get_stats()
    
    def _set_learning_mode(self, mode: str):
        try:
            from enum import Enum
            if hasattr(Enum, mode):
                enum_mode = getattr(Enum, mode)
                self.agent_engine.learning_engine.set_mode(enum_mode)
        except:
            pass
    
    def _get_memory_stats(self) -> Dict[str, Any]:
        return self.agent_engine.memory_manager.get_stats()
    
    def _store_memory(self, key: str, value: Any):
        self.global_variables[key] = value
    
    def _add_behavior(self, behavior_config: Dict[str, Any]):
        # Create behavior from configuration
        behavior = Behavior.from_config(behavior_config, self.agent_engine)
        self.agent_engine.behavior_manager.add_behavior(behavior)
    
    def _remove_behavior(self, behavior_name: str):
        self.agent_engine.behavior_manager.remove_behavior(behavior_name)
    
    def _select_behavior(self, behavior_name: str) -> bool:
        behavior = self.agent_engine.behavior_manager.behaviors.get(behavior_name)
        if behavior:
            # This is a simplified approach - in practice, behavior selection
            # would be more complex and might involve state-based selection
            return True
        return False
    
    def _add_event_listener(self, event_type: str, callback: Callable):
        self.agent_engine.add_event_listener(event_type, callback)
    
    def _remove_event_listener(self, event_type: str, callback: Callable):
        self.agent_engine.remove_event_listener(event_type, callback)
```

### 8. Helper Classes

#### Base Classes for Behaviors and Scripts

```python
# Abstract base class for behaviors
class Behavior(ABC):
    """Abstract base class for agent behaviors"""
    
    def __init__(self, name: str, agent_engine):
        self.name = name
        self.agent_engine = agent_engine
        self.enabled = True
        self.priority = 1.0
        self.weight = 1.0
    
    @abstractmethod
    def execute(self, state: AgentState, percept: Dict[str, Any]) -> Any:
        """Execute behavior and return action"""
        pass
    
    @abstractmethod
    def is_enabled(self) -> bool:
        """Check if behavior is enabled"""
        pass
    
    def get_base_score(self) -> float:
        """Get base score for behavior selection"""
        return 1.0
    
    def get_condition_multiplier(self, state: AgentState) -> float:
        """Get condition multiplier for behavior"""
        return 1.0
    
    def __str__(self):
        return self.name

# Concrete behavior implementations
class ExplorationBehavior(Behavior):
    def __init__(self, agent_engine):
        super().__init__("exploration", agent_engine)
    
    def execute(self, state: AgentState, percept: Dict[str, Any]) -> Any:
        # Generate random actions for exploration
        action_types = ['MoveForward', 'MoveBackward', 'TurnLeft', 'TurnRight', 'Look']
        action_type = np.random.choice(action_types)
        
        # Create action
        from nlm import Action
        action = Action(action_type)
        
        return action
    
    def is_enabled(self) -> bool:
        return self.agent_engine.config.get('exploration_enabled', True)

class ExploitationBehavior(Behavior):
    def __init__(self, agent_engine):
        super().__init__("exploitation", agent_engine)
    
    def execute(self, state: AgentState, percept: Dict[str, Any]) -> Any:
        # Generate goal-directed actions
        if state.position.get('x', 0) > 5:
            action_type = 'MoveLeft'
        elif state.position.get('x', 0) < 5:
            action_type = 'MoveRight'
        else:
            action_type = 'MoveForward'
        
        from nlm import Action
        action = Action(action_type)
        
        return action
    
    def is_enabled(self) -> bool:
        return self.agent_engine.config.get('exploitation_enabled', True)

class CuriosityBehavior(Behavior):
    def __init__(self, agent_engine):
        super().__init__("curiosity", agent_engine)
    
    def execute(self, state: AgentState, percept: Dict[str, Any]) -> Any:
        # Generate novel actions based on curiosity
        import random
        novel_actions = ['Look', 'LookUp', 'LookDown', 'Interact', 'Custom']
        action_type = random.choice(novel_actions)
        
        from nlm import Action
        action = Action(action_type)
        
        return action
    
    def is_enabled(self) -> bool:
        return self.agent_engine.enabled_features.get('curiosity', False)

# Script base class
class Script:
    """Base class for agent scripts"""
    
    def __init__(self, name: str, content: str):
        self.name = name
        self.content = content
        self.context = {}
    
    def execute(self, context: Dict[str, Any]) -> Any:
        """Execute script in given context"""
        self.context = context
        
        # Create execution namespace
        namespace = self.context.copy()
        namespace['__builtins__'] = __builtins__
        
        # Execute script
        exec(self.content, namespace)
        
        # Return result if script has a return statement
        if 'result' in namespace:
            return namespace['result']
        return None
```

## Advanced Agent Features

### 1. Curiosity-Driven Learning

```python
def enable_curiosity_driven_behavior(agent):
    """Enable curiosity-driven behavior for the agent"""
    
    # Enable curiosity in learning engine
    agent.learning_engine.enable_curiosity()
    
    # Add curiosity behavior
    curiosity_behavior = CuriosityBehavior(agent)
    agent.behavior_manager.add_behavior(curiosity_behavior)
    
    # Create curiosity module
    curiosity_module = CuriosityModule(agent)
    
    # Override process_reward method
    original_process_reward = agent.learning_engine.process_reward
    
    def enhanced_process_reward(reward, success):
        # Add curiosity reward
        curiosity_reward = curiosity_module.calculate_curiosity_reward()
        
        # Combine with original reward
        total_reward = reward + curiosity_reward
        
        # Process with original method
        return original_process_reward(total_reward, success)
    
    agent.learning_engine.process_reward = enhanced_process_reward
    
    return agent

class CuriosityModule:
    """Module for curiosity-driven behavior"""
    
    def __init__(self, agent_engine):
        self.agent_engine = agent_engine
        self.novelty_detector = NoveltyDetector(agent_engine)
        self.intrinsic_reward_calculator = IntrinsicRewardCalculator(agent_engine)
    
    def calculate_curiosity_reward(self) -> float:
        """Calculate curiosity-driven intrinsic reward"""
        # Detect novel experiences
        novelty_score = self.novelty_detector.detect_novelty()
        
        # Calculate intrinsic reward
        intrinsic_reward = self.intrinsic_reward_calculator.calculate(novelty_score)
        
        return intrinsic_reward
    
    def enable(self):
        """Enable curiosity module"""
        self.novelty_detector.enable()
        self.intrinsic_reward_calculator.enable()
```

### 2. Meta-Learning

```python
def enable_meta_learning(agent):
    """Enable meta-learning capabilities for the agent"""
    
    # Enable meta-learning in learning engine
    agent.learning_engine.enable_meta_learning()
    
    # Create meta-learner
    meta_learner = agent.learning_engine.meta_learner
    
    # Set up meta-learning objectives
    meta_learner.set_objective('fast_adaptation')
    meta_learner.set_objective('transfer_learning')
    meta_learner.set_objective('continual_learning')
    
    # Create meta-learning policies
    meta_policy = MetaLearningPolicy(agent)
    
    # Override learning method
    original_set_mode = agent.learning_engine.set_mode
    
    def enhanced_set_mode(mode, parameters):
        # Set mode with meta-learning considerations
        original_set_mode(mode, parameters)
        
        # Update meta-learner
        meta_learner.update(mode, parameters)
    
    agent.learning_engine.set_mode = enhanced_set_mode
    
    return agent

class MetaLearningPolicy:
    """Policy for meta-learning"""
    
    def __init__(self, agent_engine):
        self.agent_engine = agent_engine
        self.meta_learner = agent_engine.learning_engine.meta_learner
    
    def select_action(self, state, action_space):
        """Select action using meta-learning policy"""
        # Get meta-learned policy
        policy = self.meta_learner.get_policy()
        
        # Apply meta-policy to action selection
        if policy:
            action = policy(state, action_space)
            return action
        
        # Fallback to default action selection
        return self.agent_engine.motor_controller.select_default_action()
```

### 3. Transfer Learning

```python
def enable_transfer_learning(agent, source_tasks: List[str]):
    """Enable transfer learning from source tasks"""
    
    # Enable transfer learning in learning engine
    agent.learning_engine.enable_transfer_learning()
    
    # Create transfer learning module
    transfer_module = agent.learning_engine.transfer_learning_module
    
    # Load source task knowledge
    for task_name in source_tasks:
        transfer_module.load_task_knowledge(task_name)
    
    # Set up transfer mechanisms
    transfer_module.set_transfer_strategy('feature_extraction')
    transfer_module.set_transfer_strategy('parameter_sharing')
    
    return agent

class TransferLearningModule:
    """Module for transfer learning"""
    
    def __init__(self, agent_engine):
        self.agent_engine = agent_engine
        self.source_knowledge = {}
        self.transfer_strategies = {}
    
    def load_task_knowledge(self, task_name: str):
        """Load knowledge from source task"""
        # Load task knowledge (would typically load from file or database)
        self.source_knowledge[task_name] = self._load_knowledge_from_task(task_name)
    
    def _load_knowledge_from_task(self, task_name: str) -> Dict[str, Any]:
        """Load knowledge for a specific task"""
        # Implementation would depend on task format
        # This is a placeholder
        return {
            'model_weights': None,
            'feature_extractors': None,
            'task_specification': task_name,
            'performance_metrics': {}
        }
    
    def set_transfer_strategy(self, strategy: str):
        """Set transfer learning strategy"""
        self.transfer_strategies[strategy] = self._create_transfer_strategy(strategy)
    
    def _create_transfer_strategy(self, strategy: str):
        """Create transfer strategy instance"""
        # Implementation would create appropriate strategy
        pass
```

## Advanced Scripting Examples

### 1. Behavioral Scripting

```python
# Example: Behavioral scripting for adaptive agent
behavior_script = """
# Define custom behavior based on agent state
def custom_behavior(agent, state, percept):
    # Access agent properties
    position = agent.current_state.position
    energy = agent.current_state.energy
    curiosity = agent.agent.getCuriosityLevel()
    
    # Determine action based on conditions
    if energy < 50:  # Low energy
        return "Rest"
    elif position['x'] > 10:  # Far from target
        return "MoveLeft"
    elif curiosity > 0.8:  # High curiosity
        return "Look"
    else:  # Default behavior
        return "MoveForward"

# Register behavior
agent.scripting_interface.load_script("custom_behavior", behavior_script)
agent.scripting_interface.execute_script("custom_behavior")

# Set as current behavior
agent.behavior_manager.add_behavior(
    Behavior.from_config({
        'name': 'custom_behavior',
        'script': 'custom_behavior',
        'priority': 1.5
    }, agent)
)
"""

# Example: Complex scripting with multiple behaviors
complex_script = """
# Define multiple behaviors based on context
def context_aware_behavior(agent, state, percept):
    # Analyze context
    time_of_day = get_current_time()  # Get current time
    energy_level = agent.current_state.energy
    behavior_mode = agent.current_state.behavior_mode
    
    # Choose behavior based on context
    if time_of_day > 18 and energy_level > 80:
        return "Explore"  # Evening exploration
    elif time_of_day > 10 and time_of_day < 17:
        if behavior_mode == 'exploitation':
            return "Work"
        else:
            return "Rest"
    else:
        return "Sleep"  # Night/rest

# Set up multiple behaviors
agent.scripting_interface.load_script("context_aware", complex_script)
agent.scripting_interface.execute_script("context_aware")

# Create context-aware behavior
context_behavior = Behavior.from_config({
    'name': 'context_aware',
    'script': 'context_aware',
    'priority': 2.0,
    'conditions': ['time_of_day', 'energy_level', 'behavior_mode']
}, agent)

agent.behavior_manager.add_behavior(context_behavior)
```

### 2. Learning Scripting

```python
# Example: Custom learning script
learning_script = """
# Define custom learning algorithm
def custom_learning_algorithm(reward, state, action):
    # Custom learning logic
    learning_rate = agent.agent.learning_engine.learning_parameters.learning_rate
    
    # Modify reward based on custom criteria
    if reward > 0.5:
        modified_reward = reward * 1.5  # Boost high rewards
    elif reward < -0.5:
        modified_reward = reward * 0.5  # Dampen low rewards
    else:
        modified_reward = reward
    
    return modified_reward

# Override reward processing
agent.learning_engine.reward_processor.custom_algorithm = custom_learning_algorithm

# Create learning behavior
learning_behavior = Behavior.from_config({
    'name': 'custom_learning',
    'type': 'learning',
    'algorithm': 'custom_learning',
    'parameters': {'adaptation_rate': 0.1}
}, agent)

agent.behavior_manager.add_behavior(learning_behavior)
"""

# Example: Meta-learning script
meta_learning_script = """
# Define meta-learning strategy
def meta_learning_strategy(agent, episode_history):
    # Analyze episode history
    rewards = [episode['reward'] for episode in episode_history]
    success_rates = [episode['success'] for episode in episode_history]
    
    # Determine if adaptation is needed
    if len(rewards) >= 10:
        recent_performance = np.mean(rewards[-10:])
        if recent_performance < 0.3:
            # Need better adaptation
            return 'increase_exploration'
        elif recent_performance > 0.8:
            # Good performance, can exploit more
            return 'increase_exploitation'
        else:
            return 'maintain_balance'
    
    return 'maintain_balance'

# Set up meta-learning
agent.scripting_interface.load_script("meta_learning", meta_learning_script)
agent.scripting_interface.execute_script("meta_learning")

# Create meta-learning behavior
meta_behavior = Behavior.from_config({
    'name': 'meta_learning',
    'type': 'meta_learning',
    'strategy': 'meta_learning',
    'update_frequency': 'every_episode'
}, agent)

agent.behavior_manager.add_behavior(meta_behavior)
```

### 3. Performance Scripting

```python
# Example: Performance optimization script
performance_script = """
# Define performance optimization rules
def performance_optimizer(agent, metrics):
    # Analyze current performance
    cpu_usage = metrics.get('cpu_usage', 0)
    memory_usage = metrics.get('memory_usage', 0)
    firing_rate = metrics.get('firing_rate', 0)
    step_time = metrics.get('step_time', 0)
    
    # Optimize based on performance bottlenecks
    optimizations = []
    
    if cpu_usage > 80:
        optimizations.append('reduce_neuron_count')
        optimizations.append('optimize_computation')
    
    if memory_usage > 90:
        optimizations.append('enable_memory_pooling')
        optimizations.append('compress_data_structures')
    
    if step_time > 0.1:
        optimizations.append('reduce_plasticity')
        optimizations.append('optimize_synapse_operations')
    
    if firing_rate > 100:
        optimizations.append('increase_inhibition')
        optimizations.append('adjust_thresholds')
    
    return optimizations

# Create performance monitoring
agent.scripting_interface.load_script("performance_optimizer", performance_script)
performance_optimizations = agent.scripting_interface.execute_script("performance_optimizer")

# Apply optimizations
for optimization in performance_optimizations:
    if optimization == 'reduce_neuron_count':
        agent.brain.getConfig().set("brain.neuron_count", 
                                   max(100, agent.brain.getTotalNeuronCount() // 2))
    elif optimization == 'enable_memory_pooling':
        agent.config['memory_pooling_enabled'] = True
    elif optimization == 'reduce_plasticity':
        agent.brain.getConfig().set("plasticity.stdp.learning_rate", 
                                   agent.brain.getConfig().get("plasticity.stdp.learning_rate") * 0.5)
"""
```

## Agent Scripting API

### Core Functions

```python
# Agent management functions
def create_advanced_agent(brain, world, config=None) -> AgentScriptingEngine:
    """Create an advanced agent with scripting capabilities"""
    return AgentScriptingEngine(brain, world, config)

def load_agent_preset(preset_name: str) -> Dict[str, Any]:
    """Load a predefined agent configuration"""
    presets = {
        'explorer': {
            'behavior_mode': 'exploration',
            'learning_enabled': True,
            'exploration_rate': 0.8,
            'exploitation_rate': 0.2
        },
        'exploiter': {
            'behavior_mode': 'exploitation',
            'learning_enabled': True,
            'exploration_rate': 0.2,
            'exploitation_rate': 0.8
        },
        'balanced': {
            'behavior_mode': 'hybrid',
            'learning_enabled': True,
            'exploration_rate': 0.5,
            'exploitation_rate': 0.5
        },
        'curious': {
            'behavior_mode': 'adaptive',
            'learning_enabled': True,
            'exploration_rate': 0.6,
            'exploitation_rate': 0.4,
            'curiosity_enabled': True
        },
        'learning': {
            'behavior_mode': 'learning',
            'learning_enabled': True,
            'learning_rate': 0.05,
            'meta_learning_enabled': True
        }
    }
    
    return presets.get(preset_name, presets['balanced'])

# Scripting functions
def execute_agent_script(agent: AgentScriptingEngine, script_name: str, parameters: Dict[str, Any] = None):
    """Execute a script for the agent"""
    return agent.scripting_interface.execute_script(script_name, parameters)

def load_agent_script(agent: AgentScriptingEngine, script_name: str, script_content: str):
    """Load a script for the agent"""
    agent.scripting_interface.load_script(script_name, script_content)

def unload_agent_script(agent: AgentScriptingEngine, script_name: str):
    """Unload a script for the agent"""
    agent.scripting_interface.unload_script(script_name)

# Behavior management functions
def add_agent_behavior(agent: AgentScriptingEngine, behavior_config: Dict[str, Any]):
    """Add a behavior to the agent"""
    behavior = Behavior.from_config(behavior_config, agent)
    agent.behavior_manager.add_behavior(behavior)

def remove_agent_behavior(agent: AgentScriptingEngine, behavior_name: str):
    """Remove a behavior from the agent"""
    agent.behavior_manager.remove_behavior(behavior_name)

def list_agent_behaviors(agent: AgentScriptingEngine) -> List[str]:
    """List all behaviors for the agent"""
    return list(agent.behavior_manager.behaviors.keys())

# Learning management functions
def set_agent_learning_mode(agent: AgentScriptingEngine, mode: str, parameters: Dict[str, Any] = None):
    """Set the agent's learning mode"""
    try:
        from enum import Enum
        if hasattr(Enum, mode):
            enum_mode = getattr(Enum, mode)
            agent.learning_engine.set_mode(enum_mode, parameters)
    except:
        raise ValueError(f"Unknown learning mode: {mode}")

def enable_agent_features(agent: AgentScriptingEngine, features: List[str]):
    """Enable advanced agent features"""
    for feature in features:
        if hasattr(agent.learning_engine, f'enable_{feature}'):
            getattr(agent.learning_engine, f'enable_{feature}')()

# Event management functions
def add_agent_event_listener(agent: AgentScriptingEngine, event_type: str, callback: Callable):
    """Add an event listener to the agent"""
    agent.add_event_listener(event_type, callback)

def remove_agent_event_listener(agent: AgentScriptingEngine, event_type: str, callback: Callable):
    """Remove an event listener from the agent"""
    agent.remove_event_listener(event_type, callback)

# Performance monitoring functions
def monitor_agent_performance(agent: AgentScriptingEngine) -> Dict[str, Any]:
    """Monitor agent performance"""
    if hasattr(agent, 'performance_monitor'):
        return agent.performance_monitor.analyze_performance()
    return {}

def export_agent_performance(agent: AgentScriptingEngine, filepath: str, format: str = 'csv'):
    """Export agent performance data"""
    if hasattr(agent, 'performance_monitor'):
        agent.performance_monitor.export_performance_data(filepath, format)
```

## Performance Considerations

### 1. Script Execution Performance

```python
class ScriptPerformanceOptimizer:
    """Optimize script execution performance"""
    
    def __init__(self, agent):
        self.agent = agent
        self.script_cache = {}
        self.compiled_scripts = {}
        self.function_cache = {}
    
    def optimize_script_execution(self, script_name: str) -> Dict[str, Any]:
        """Optimize script execution performance"""
        if script_name in self.compiled_scripts:
            script = self.compiled_scripts[script_name]
        else:
            # Compile script for faster execution
            script = self._compile_script(script_name)
            self.compiled_scripts[script_name] = script
        
        # Use cached functions if available
        if script_name in self.function_cache:
            execute_function = self.function_cache[script_name]
        else:
            execute_function = self._create_execute_function(script_name, script)
            self.function_cache[script_name] = execute_function
        
        # Get performance metrics
        metrics = self._measure_script_performance(script_name, execute_function)
        
        return metrics
    
    def _compile_script(self, script_name: str) -> 'CompiledScript':
        """Compile script for faster execution"""
        # Implementation would compile script for faster execution
        pass
    
    def _create_execute_function(self, script_name: str, script: 'CompiledScript') -> Callable:
        """Create optimized execute function"""
        # Implementation would create optimized execution function
        pass
    
    def _measure_script_performance(self, script_name: str, execute_function: Callable) -> Dict[str, Any]:
        """Measure script performance"""
        # Measure execution time and resource usage
        pass
```

### 2. Behavior Selection Optimization

```python
class BehaviorSelectionOptimizer:
    """Optimize behavior selection performance"""
    
    def __init__(self, agent):
        self.agent = agent
        self.behavior_cache = {}
        self.selection_algorithms = {}
    
    def optimize_behavior_selection(self) -> Dict[str, Any]:
        """Optimize behavior selection performance"""
        # Analyze current selection algorithm
        current_algorithm = self.agent.behavior_manager._selection_algorithm
        
        # Test different selection algorithms
        algorithms = ['epsilon_greedy', 'softmax', 'upper_confidence_bound', 'thompson_sampling']
        
        results = {}
        for algorithm in algorithms:
            # Test algorithm
            score = self._test_algorithm(algorithm)
            results[algorithm] = score
        
        # Select best algorithm
        best_algorithm = max(results, key=results.get)
        
        # Update agent with best algorithm
        self.agent.behavior_manager._selection_algorithm = best_algorithm
        
        return {
            'best_algorithm': best_algorithm,
            'algorithm_scores': results,
            'performance_improvement': results[best_algorithm] - results[current_algorithm]
        }
    
    def _test_algorithm(self, algorithm: str) -> float:
        """Test selection algorithm performance"""
        # Implementation would test algorithm performance
        pass
```

## Conclusion

The NLM advanced agent scripting capabilities provide a comprehensive framework for creating sophisticated, adaptive, and learning agents. The system includes:

### 1. **Modular Architecture**
- Behavior management with multiple behavior types
- Learning engine with multiple learning mechanisms
- Memory management with multiple memory systems
- Sensory processing with advanced feature extraction
- Motor control with multiple action generators
- Scripting interface with flexible script execution

### 2. **Advanced Features**
- Curiosity-driven learning and behavior
- Meta-learning and adaptive strategies
- Transfer learning capabilities
- Context-aware behavior selection
- Performance optimization and monitoring
- Event-driven programming model

### 3. **Scripting Capabilities**
- Declarative behavior scripting
- Procedural behavior programming
- Custom learning algorithms
- Performance optimization scripting
- Meta-programming capabilities
- Dynamic behavior modification

### 4. **Performance Considerations**
- Optimized script execution
- Efficient behavior selection
- Resource management
- Performance monitoring and analysis
- Bottleneck detection and optimization

### 5. **API Design**
- Intuitive, Pythonic API design
- Comprehensive documentation and examples
- Type hints and code completion support
- Interactive debugging and testing tools
- Integration with development workflows

The advanced agent scripting system transforms the NLM from a basic neural simulation framework into a comprehensive agent-based simulation platform that supports:

- **Complex agent behaviors** with multiple interaction patterns
- **Adaptive learning** with multiple learning mechanisms
- **Memory and cognition** with sophisticated memory systems
- **Scripting and customization** with flexible scripting capabilities
- **Performance optimization** with comprehensive monitoring and analysis
- **Research and experimentation** with advanced experimental frameworks

This system enables users to create agents that can learn, adapt, and interact with complex environments in sophisticated ways, making NLM a powerful platform for agent-based research and simulation.

---

**Next Steps for Advanced Agent Scripting:**

1. **Development**: Implement advanced behavior and learning classes
2. **Integration**: Integrate scripting capabilities with NLMCLI
3. **Documentation**: Add comprehensive documentation and examples
4. **Testing**: Create comprehensive test suites for agent scripting
5. **Optimization**: Optimize performance and resource usage
6. **Validation**: Validate agent behaviors and learning algorithms
7. **Extension**: Extend capabilities for research and production use

The advanced agent scripting system provides a foundation for creating intelligent, adaptive agents that can learn, explore, and interact with complex environments using the NLM neural simulation framework.

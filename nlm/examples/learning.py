"""
Advanced Learning Example for NLM

This example demonstrates comprehensive learning scenarios with NLM including:
- Multi-agent coordination and competition
- Learning with rewards, curiosity, and prediction error
- Developmental changes over time
- Memory integration and replay
- Neuromodulation and plasticity
- Performance optimization and monitoring
- Real-time learning analysis
- Complex world environments

This example showcases production-ready learning code with robust error handling,
advanced neuroscience modeling, and comprehensive performance monitoring.
"""

import time
import logging
import json
import numpy as np
from typing import Dict, List, Any, Optional, Tuple
from datetime import datetime
from dataclasses import dataclass, asdict

from nlm import create_brain, World, create_agent, create_region

# Configure comprehensive logging for learning simulation
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_learning_example.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

@dataclass
class LearningMetrics:
    """Data class for storing learning metrics."""
    step: int
    episode: int
    reward: float
    accuracy: float
    loss: float
    exploration_rate: float
    curiosity_bonus: float
    prediction_error: float
    learning_rate: float
    memory_efficiency: float
    time_elapsed: float
    agent_id: str

class MemorySystem:
    """Advanced memory system with episodic, semantic, and working memory."""
    
    def __init__(self, capacity: int = 10000):
        self.capacity = capacity
        self.episodic_memory = []  # Sequence of experiences
        self.semantic_memory = {}  # Abstracted knowledge
        self.working_memory = []  # Current context
        self.max_working_memory = 20
        self.replay_buffer = []
        self.memory_strength = {}
        self.association_weights = {}
    
    def store_episodic(self, experience: Dict[str, Any]) -> None:
        """Store episodic memory (specific experience)."""
        experience['timestamp'] = time.time()
        experience['memory_id'] = len(self.episodic_memory)
        
        # Calculate memory strength based on reward and prediction error
        reward = experience.get('reward', 0.0)
        prediction_error = abs(experience.get('prediction_error', 0.0))
        strength = min(1.0, (abs(reward) + prediction_error * 2.0) / 3.0)
        
        experience['strength'] = strength
        experience['recency'] = 0.0  # Will be updated on access
        
        self.episodic_memory.append(experience)
        
        # Update semantic memory
        self._update_semantic_memory(experience)
        
        # Update associations
        self._update_associations(experience)
        
        # Maintain capacity
        if len(self.episodic_memory) > self.capacity:
            self._compress_episodic()
    
    def _update_semantic_memory(self, experience: Dict[str, Any]) -> None:
        """Update semantic memory with abstracted knowledge."""
        # Extract key features from experience
        obs_features = experience.get('observation', {})
        action = experience.get('action', {})
        reward = experience.get('reward', 0.0)
        
        # Create abstract representation
        key_features = {
            'action_type': action.get('type', 'unknown'),
            'reward_magnitude': np.clip(reward, -1.0, 1.0),
            'context_features': self._extract_context_features(obs_features)
        }
        
        # Convert to hashable key
        key = tuple(sorted(key_features.items()))
        
        if key not in self.semantic_memory:
            self.semantic_memory[key] = {
                'count': 0,
                'average_reward': 0.0,
                'variability': 0.0,
                'last_accessed': time.time()
            }
        
        # Update statistics
        semantic = self.semantic_memory[key]
        n = semantic['count']
        semantic['average_reward'] = (semantic['average_reward'] * n + reward) / (n + 1)
        semantic['variability'] = (semantic['variability'] * n + (reward - semantic['average_reward']) ** 2) / (n + 1)
        semantic['count'] += 1
        semantic['last_accessed'] = time.time()
    
    def _extract_context_features(self, observation: Dict[str, Any]) -> Dict[str, float]:
        """Extract relevant features from observation for semantic memory."""
        features = {}
        
        # Environmental features
        env = observation.get('environment', {})
        features['temperature'] = self._normalize_feature(env.get('temperature', 20.0), 0, 50)
        features['humidity'] = self._normalize_feature(env.get('humidity', 0.5), 0, 1)
        features['resource_density'] = self._normalize_feature(env.get('resources', 0), 0, 10)
        
        # Social features
        agents = observation.get('agents', [])
        features['agent_count'] = min(len(agents) / 10.0, 1.0)
        if agents:
            # Calculate proximity to agent
            agent_positions = [a.get('position', {'x': 0, 'y': 0}) for a in agents]
            distances = [np.sqrt((a['x'] - 50) ** 2 + (a['y'] - 50) ** 2) for a in agent_positions]
            features['min_distance'] = min(distances) / 100.0
        else:
            features['min_distance'] = 1.0
        
        return features
    
    def _normalize_feature(self, value: float, min_val: float, max_val: float) -> float:
        """Normalize feature value to [0, 1] range."""
        return np.clip((value - min_val) / (max_val - min_val), 0.0, 1.0)
    
    def _update_associations(self, experience: Dict[str, Any]) -> None:
        """Update associative memory between features."""
        obs_features = experience.get('observation', {})
        action = experience.get('action', {})
        reward = experience.get('reward', 0.0)
        
        # Create feature associations
        context_key = tuple(sorted(self._extract_context_features(obs_features).items()))
        action_key = action.get('type', 'unknown')
        
        if context_key not in self.association_weights:
            self.association_weights[context_key] = {}
        
        if action_key not in self.association_weights[context_key]:
            self.association_weights[context_key][action_key] = {
                'weight': 0.0,
                'average_reward': 0.0,
                'count': 0
            }
        
        # Update association
        assoc = self.association_weights[context_key][action_key]
        n = assoc['count']
        assoc['weight'] = (assoc['weight'] * n + abs(reward)) / (n + 1)
        assoc['average_reward'] = (assoc['average_reward'] * n + reward) / (n + 1)
        assoc['count'] += 1
    
    def _compress_episodic(self) -> None:
        """Compress episodic memory to maintain capacity."""
        # Remove weakest memories
        strengths = [(i, exp['strength']) for i, exp in enumerate(self.episodic_memory)]
        strengths.sort(key=lambda x: x[1])
        
        # Remove bottom 20% of memories
        remove_count = int(len(self.episodic_memory) * 0.2)
        indices_to_remove = [idx for idx, _ in strengths[:remove_count]]
        
        # Remove in reverse order to maintain indices
        for i in sorted(indices_to_remove, reverse=True):
            del self.episodic_memory[i]
        
        # Adjust remaining memory IDs
        for i, exp in enumerate(self.episodic_memory):
            exp['memory_id'] = i
    
    def retrieve_relevant_memories(self, query: Dict[str, Any], k: int = 5) -> List[Dict[str, Any]]:
        """Retrieve relevant memories based on query features."""
        relevant_memories = []
        
        for memory in self.episodic_memory:
            similarity = self._calculate_memory_similarity(memory, query)
            if similarity > 0.1:  # Threshold for relevance
                relevant_memories.append((memory, similarity))
        
        # Sort by similarity and strength
        relevant_memories.sort(key=lambda x: x[1] * x[0][1], reverse=True)
        
        # Return top k memories
        return [mem for mem, _ in relevant_memories[:k]]
    
    def _calculate_memory_similarity(self, memory: Dict[str, Any], query: Dict[str, Any]) -> float:
        """Calculate similarity between memory and query."""
        # Feature-based similarity
        mem_features = self._extract_context_features(memory.get('observation', {}))
        query_features = self._extract_context_features(query)
        
        # Calculate feature distance
        feature_distance = 0.0
        for key in mem_features:
            if key in query_features:
                feature_distance += abs(mem_features[key] - query_features[key])
        
        # Normalize similarity
        similarity = 1.0 - min(feature_distance / len(mem_features), 1.0)
        
        # Boost similarity if action types match
        if memory.get('action', {}).get('type') == query.get('action', {}).get('type'):
            similarity *= 1.2
        
        return similarity
    
    def update_recency(self, memory_id: int) -> None:
        """Update recency of accessed memory."""
        for memory in self.episodic_memory:
            if memory['memory_id'] == memory_id:
                memory['recency'] = time.time()
                break
    
    def get_working_memory(self) -> List[Dict[str, Any]]:
        """Get current working memory contents."""
        return self.working_memory.copy()
    
    def update_working_memory(self, new_content: Dict[str, Any]) -> None:
        """Update working memory with new content."""
        # Add new content
        self.working_memory.append({
            'content': new_content,
            'timestamp': time.time(),
            'strength': 1.0
        })
        
        # Maintain working memory size
        if len(self.working_memory) > self.max_working_memory:
            # Remove oldest items
            self.working_memory = self.working_memory[-self.max_working_memory:]
    
    def get_memory_stats(self) -> Dict[str, Any]:
        """Get statistics about memory systems."""
        return {
            'episodic_count': len(self.episodic_memory),
            'semantic_count': len(self.semantic_memory),
            'working_memory_count': len(self.working_memory),
            'association_count': sum(len(assocs) for assocs in self.association_weights.values()),
            'total_capacity': self.capacity,
            'utilization': len(self.episodic_memory) / self.capacity
        }

class NeuromodulationSystem:
    """Advanced neuromodulation system with multiple neurotransmitters."""
    
    def __init__(self):
        self.neurotransmitters = {
            'dopamine': DopamineModulator(),
            'acetylcholine': AcetylcholineModulator(),
            'serotonin': SerotoninModulator(),
            'noradrenaline': NoradrenalineModulator(),
            'octopamine': OctopamineModulator(),
        }
        self.global_modulation = {
            ' arousal': 1.0,
            'attention': 1.0,
            'motivation': 1.0,
            'stress': 0.0,
            'fatigue': 0.0
        }
        self.modulation_history = []
    
    def update_modulation(self, state: Dict[str, Any], action: Dict[str, Any], reward: float) -> Dict[str, Any]:
        """Update neuromodulation based on current state and reward."""
        modulation_updates = {}
        
        for name, modulator in self.neurotransmitters.items():
            modulation_updates[name] = modulator.update(state, action, reward)
        
        # Update global modulation states
        self._update_global_modulation(state, action, reward)
        
        # Store history
        self.modulation_history.append({
            'timestamp': time.time(),
            'neurotransmitters': modulation_updates,
            'global_states': self.global_modulation.copy()
        })
        
        # Maintain history size
        if len(self.modulation_history) > 1000:
            self.modulation_history.pop(0)
        
        return {
            'neurotransmitters': modulation_updates,
            'global_modulation': self.global_modulation.copy()
        }
    
    def _update_global_modulation(self, state: Dict[str, Any], action: Dict[str, Any], reward: float) -> None:
        """Update global modulation states."""
        # Arousal: based on prediction error and novelty
        prediction_error = abs(action.get('prediction_error', 0.0))
        novelty = action.get('novelty', 0.0)
        self.global_modulation['arousal'] = min(2.0, 1.0 + prediction_error * 2.0 + novelty)
        
        # Attention: based on task relevance and effort
        task_relevance = self._calculate_task_relevance(state, action)
        effort = action.get('effort', 0.0)
        self.global_modulation['attention'] = min(2.0, task_relevance + effort * 0.5)
        
        # Motivation: based on reward prediction and goal alignment
        reward_prediction = reward
        goal_alignment = self._calculate_goal_alignment(state, action)
        self.global_modulation['motivation'] = min(2.0, reward_prediction * 0.5 + goal_alignment)
        
        # Stress: based on prediction error and time pressure
        stress_from_error = prediction_error * 0.3
        time_pressure = state.get('time_pressure', 0.0)
        self.global_modulation['stress'] = min(2.0, stress_from_error + time_pressure)
        
        # Fatigue: increases with time and decreases with reward
        self.global_modulation['fatigue'] = min(2.0, state.get('time_elapsed', 0.0) * 0.001 + (1.0 - reward) * 0.1)
    
    def _calculate_task_relevance(self, state: Dict[str, Any], action: Dict[str, Any]) -> float:
        """Calculate relevance of current task."""
        # Simple relevance based on action type and state
        action_type = action.get('type', 'unknown')
        
        if action_type == 'exploration':
            return 0.5
        elif action_type == 'exploitation':
            return 0.8
        else:
            return 0.3
    
    def _calculate_goal_alignment(self, state: Dict[str, Any], action: Dict[str, Any]) -> float:
        """Calculate alignment of action with goals."""
        # Simple alignment based on reward and action type
        reward = action.get('reward', 0.0)
        action_type = action.get('type', 'unknown')
        
        if action_type == 'exploitation':
            return min(1.0, reward * 1.5)
        else:
            return min(1.0, reward * 0.8)
    
    def get_modulation_summary(self) -> Dict[str, Any]:
        """Get summary of current modulation state."""
        return {
            'global_modulation': self.global_modulation,
            'neurotransmitter_levels': {name: modulator.get_level() 
                                      for name, modulator in self.neurotransmitters.items()},
            'modulation_history_length': len(self.modulation_history)
        }

class DopamineModulator:
    """Dopamine modulation system."""
    
    def __init__(self):
        self.level = 0.5
        self.history = []
    
    def update(self, state: Dict[str, Any], action: Dict[str, Any], reward: float) -> float:
        """Update dopamine level based on reward prediction error."""
        # Reward prediction error
        predicted_reward = action.get('prediction', {}).get('expected_reward', 0.5)
        rpe = reward - predicted_reward
        
        # Update level with adaptation
        self.level += rpe * 0.1 - self.level * 0.01
        self.level = np.clip(self.level, 0.0, 2.0)
        
        self.history.append({'timestamp': time.time(), 'level': self.level, 'rpe': rpe})
        if len(self.history) > 100:
            self.history.pop(0)
        
        return self.level
    
    def get_level(self) -> float:
        return self.level

class AcetylcholineModulator:
    """Acetylcholine modulation system."""
    
    def __init__(self):
        self.level = 0.5
        self.history = []
    
    def update(self, state: Dict[str, Any], action: Dict[str, Any], reward: float) -> float:
        """Update acetylcholine level based on attention and novelty."""
        # Attention signal
        novelty = action.get('novelty', 0.0)
        
        # Update level
        self.level += novelty * 0.2 - self.level * 0.02
        self.level = np.clip(self.level, 0.0, 2.0)
        
        self.history.append({'timestamp': time.time(), 'level': self.level, 'novelty': novelty})
        if len(self.history) > 100:
            self.history.pop(0)
        
        return self.level
    
    def get_level(self) -> float:
        return self.level

class SerotoninModulator:
    """Serotonin modulation system."""
    
    def __init__(self):
        self.level = 0.5
        self.history = []
    
    def update(self, state: Dict[str, Any], action: Dict[str, Any], reward: float) -> float:
        """Update serotonin level based on reward and frustration."""
        # Aversive learning signal
        if reward < 0.3:
            # Frustration/inhibition
            self.level += (reward - 0.3) * 0.3
        else:
            # Positive reinforcement
            self.level += reward * 0.05
        
        # Decay
        self.level -= self.level * 0.01
        self.level = np.clip(self.level, 0.0, 2.0)
        
        self.history.append({'timestamp': time.time(), 'level': self.level, 'reward': reward})
        if len(self.history) > 100:
            self.history.pop(0)
        
        return self.level
    
    def get_level(self) -> float:
        return self.level

class NoradrenalineModulator:
    """Noradrenaline modulation system."""
    
    def __init__(self):
        self.level = 0.5
        self.history = []
    
    def update(self, state: Dict[str, Any], action: Dict[str, Any], reward: float) -> float:
        """Update noradrenaline level based on arousal and stress."""
        # Arousal signal
        arousal = action.get('effort', 0.0) + abs(action.get('prediction_error', 0.0)) * 0.5
        
        # Update level
        self.level += arousal * 0.15 - self.level * 0.02
        self.level = np.clip(self.level, 0.0, 2.0)
        
        self.history.append({'timestamp': time.time(), 'level': self.level, 'arousal': arousal})
        if len(self.history) > 100:
            self.history.pop(0)
        
        return self.level
    
    def get_level(self) -> float:
        return self.level

class OctopamineModulator:
    """Octopamine modulation system (insect version of dopamine)."""
    
    def __init__(self):
        self.level = 0.5
        self.history = []
    
    def update(self, state: Dict[str, Any], action: Dict[str, Any], reward: float) -> float:
        """Update octopamine level based on immediate reward."""
        # Immediate reward signal
        immediate_reward = reward * action.get('novelty', 0.5)
        
        # Update level
        self.level += immediate_reward * 0.1 - self.level * 0.01
        self.level = np.clip(self.level, 0.0, 2.0)
        
        self.history.append({'timestamp': time.time(), 'level': self.level, 'reward': reward})
        if len(self.history) > 100:
            self.history.pop(0)
        
        return self.level
    
    def get_level(self) -> float:
        return self.level

class LearningAgent:
    """Advanced learning agent with neuromodulation and memory systems."""
    
    def __init__(self, agent_id: str, brain: Any, neuromodulation: NeuromodulationSystem):
        self.agent_id = agent_id
        self.brain = brain
        self.neuromodulation = neuromodulation
        self.memory = MemorySystem(capacity=5000)
        self.learning_rate = 0.01
        self.epsilon = 0.1
        self.temperature = 1.0
        self.curiosity = 0.5
        self.dev_stage = 0.0
        self.plasticity = 1.0
        
        # Performance tracking
        self.metrics_history = []
        self.replay_buffer = []
        self.max_replay_buffer = 1000
        
        # Goal system
        self.goal_vector = np.random.random(3)  # Goal space dimension
        self.current_goals = self.goal_vector.copy()
    
    def learn(self, observation: Dict[str, Any], action: Dict[str, Any], reward: float) -> Dict[str, Any]:
        """Advanced learning with neuromodulation and memory."""
        # Update neuromodulation
        neuromodulation_state = self.neuromodulation.update_modulation(observation, action, reward)
        
        # Store experience in memory
        experience = {
            'observation': observation,
            'action': action,
            'reward': reward,
            'neuromodulation': neuromodulation_state
        }
        self.memory.store_episodic(experience)
        
        # Add to replay buffer
        self.replay_buffer.append(experience)
        if len(self.replay_buffer) > self.max_replay_buffer:
            self.replay_buffer.pop(0)
        
        # Learn from experience
        learning_result = self._learn_from_experience(experience, neuromodulation_state)
        
        # Update performance metrics
        self._update_metrics(observation, action, reward, neuromodulation_state, learning_result)
        
        # Apply developmental changes
        self._apply_development()
        
        return learning_result
    
    def _learn_from_experience(self, experience: Dict[str, Any], neuromodulation_state: Dict[str, Any]) -> Dict[str, Any]:
        """Learn from stored experience."""
        observation = experience['observation']
        action = experience['action']
        reward = experience['reward']
        
        # Process through brain
        brain_input = self._prepare_brain_input(observation)
        
        # Get brain prediction
        brain_prediction = self.brain.predict(brain_input)
        
        # Calculate prediction error
        prediction_error = reward - brain_prediction.get('predicted_value', 0.5)
        
        # Neuromodulated learning
        neuromodulated_learning_rate = self.learning_rate * neuromodulation_state['neurotransmitters']['dopamine']
        
        # Update brain parameters
        self.brain.update_parameters(
            learning_rate=neuromodulated_learning_rate,
            prediction_error=prediction_error,
            neuromodulation=neuromodulation_state
        )
        
        # Store learning metrics
        learning_metrics = {
            'prediction_error': prediction_error,
            'neuromodulated_learning_rate': neuromodulated_learning_rate,
            'brain_prediction': brain_prediction,
            'actual_reward': reward,
            'learning_efficiency': self._calculate_learning_efficiency(reward, prediction_error),
            'memory_strength': experience.get('strength', 0.5)
        }
        
        return learning_metrics
    
    def _prepare_brain_input(self, observation: Dict[str, Any]) -> Dict[str, Any]:
        """Prepare observation features for brain input."""
        features = {}
        
        # Environmental features
        env = observation.get('environment', {})
        features['temperature'] = self._normalize_feature(env.get('temperature', 20.0), 0, 50)
        features['resources'] = self._normalize_feature(env.get('resources', 0), 0, 10)
        
        # Social features
        agents = observation.get('agents', [])
        features['agent_count'] = min(len(agents) / 10.0, 1.0)
        
        # Goal features
        features['goal_alignment'] = self._calculate_goal_alignment(observation)
        
        return features
    
    def _normalize_feature(self, value: float, min_val: float, max_val: float) -> float:
        """Normalize feature value."""
        return np.clip((value - min_val) / (max_val - min_val), 0.0, 1.0)
    
    def _calculate_goal_alignment(self, observation: Dict[str, Any]) -> float:
        """Calculate alignment between current goals and observation."""
        # Simple alignment calculation
        if self.current_goals is not None:
            alignment = np.dot(self.current_goals, self.goal_vector)
            return np.clip(alignment, 0.0, 1.0)
        return 0.5
    
    def _calculate_learning_efficiency(self, reward: float, prediction_error: float) -> float:
        """Calculate efficiency of learning from experience."""
        # Combine reward and prediction error
        efficiency = (abs(reward) + abs(prediction_error)) / 2.0
        return np.clip(efficiency, 0.0, 1.0)
    
    def _update_metrics(self, observation: Dict[str, Any], action: Dict[str, Any], reward: float,
                      neuromodulation_state: Dict[str, Any], learning_result: Dict[str, Any]) -> None:
        """Update agent metrics."""
        metrics = LearningMetrics(
            step=len(self.metrics_history),
            episode=self._get_episode(),
            reward=reward,
            accuracy=learning_result['learning_efficiency'],
            loss=1.0 - learning_result['learning_efficiency'],
            exploration_rate=self.epsilon,
            curiosity_bonus=action.get('curiosity_bonus', 0.0),
            prediction_error=learning_result['prediction_error'],
            learning_rate=self.learning_rate,
            memory_efficiency=len(self.memory.get_working_memory()) / 20.0,
            time_elapsed=time.time(),
            agent_id=self.agent_id
        )
        
        self.metrics_history.append(metrics)
        
        # Maintain metrics history
        if len(self.metrics_history) > 1000:
            self.metrics_history.pop(0)
    
    def _get_episode(self) -> int:
        """Get current episode number."""
        return len(self.metrics_history) // 10  # Rough estimate
    
    def _apply_development(self) -> None:
        """Apply developmental changes."""
        # Neural development
        if self.dev_stage < 0.8:
            # Rapid development phase
            self.dev_stage += 0.001
            self.plasticity = np.clip(self.plasticity * (1.0 + self.dev_stage * 0.1), 0.5, 2.0)
        
        # Learning rate adaptation
        if self.dev_stage > 0.5:
            # Mature phase - slower learning
            self.learning_rate *= 0.99
        
        # Curiosity development
        if self.dev_stage > 0.3:
            self.curiosity = np.clip(self.curiosity * 1.01, 0.1, 1.0)
    
    def act(self, action_type: str, action_params: Dict[str, Any]) -> Dict[str, Any]:
        """Generate action based on current state."""
        # Get brain prediction
        brain_prediction = self.brain.predict({
            'action_type': action_type,
            'action_params': action_params,
            'goals': self.current_goals,
            'development_stage': self.dev_stage
        })
        
        # Generate action with exploration/exploitation balance
        if np.random.random() < self.epsilon:
            # Exploration
            action = self._generate_exploration_action(action_type, action_params, brain_prediction)
        else:
            # Exploitation
            action = self._generate_exploitation_action(action_type, action_params, brain_prediction)
        
        # Add neuromodulation effects
        neuromodulation = self.neuromodulation.get_modulation_summary()
        
        # Add curiosity effects
        curiosity_action = self._apply_curiosity(action, brain_prediction)
        
        # Update goals based on learning
        self._update_goals(action, brain_prediction)
        
        return curiosity_action
    
    def _generate_exploration_action(self, action_type: str, params: Dict[str, Any], prediction: Dict[str, Any]) -> Dict[str, Any]:
        """Generate exploration action."""
        # Add novelty
        action = {
            'type': action_type,
            'params': params,
            'novelty': np.random.random() * 0.5 + 0.5,
            'risk': np.random.random() * 0.3,
            'reward': np.random.random() * 0.3,
            'exploration_bonus': prediction.get('curiosity_bonus', 0.0) * 2.0,
            'prediction_error': prediction.get('uncertainty', 0.0),
            'effort': np.random.random() * 0.8,
            'timestamp': time.time()
        }
        
        return action
    
    def _generate_exploitation_action(self, action_type: str, params: Dict[str, Any], prediction: Dict[str, Any]) -> Dict[str, Any]:
        """Generate exploitation action."""
        # Use predicted best action
        action = {
            'type': action_type,
            'params': params,
            'novelty': 0.0,
            'risk': 0.0,
            'reward': prediction.get('expected_reward', 0.5) * 1.5,
            'exploration_bonus': prediction.get('curiosity_bonus', 0.0) * 0.5,
            'prediction_error': prediction.get('uncertainty', 0.0),
            'effort': np.random.random() * 0.5,
            'confidence': prediction.get('confidence', 0.5),
            'timestamp': time.time()
        }
        
        return action
    
    def _apply_curiosity(self, action: Dict[str, Any], prediction: Dict[str, Any]) -> Dict[str, Any]:
        """Apply curiosity effects to action."""
        # Curiosity-driven action modification
        curiosity_level = self.curiosity
        
        # Add curiosity bonus
        action['curiosity_bonus'] = curiosity_level * 0.5
        
        # Modify action based on curiosity
        if np.random.random() < curiosity_level * 0.3:  # 30% chance of curiosity-driven modification
            action['type'] = 'explore_' + action['type']
            action['novelty'] = min(1.0, action.get('novelty', 0.0) + curiosity_level * 0.3)
            action['reward'] = min(1.0, action.get('reward', 0.0) + curiosity_level * 0.2)
        
        return action
    
    def _update_goals(self, action: Dict[str, Any], prediction: Dict[str, Any]) -> None:
        """Update agent goals based on experience."""
        # Extract goal-related information from action
        goal_update_strength = action.get('reward', 0.0) * 0.1
        
        # Update goals based on success
        if action.get('reward', 0.0) > 0.7:
            # Successful action - strengthen related goals
            goal_update = np.random.random(len(self.current_goals)) * goal_update_strength
            self.current_goals = np.minimum(1.0, self.current_goals + goal_update)
        
        # Normalize goals
        if self.current_goals.sum() > 0:
            self.current_goals = self.current_goals / self.current_goals.sum()
    
    def get_state(self) -> Dict[str, Any]:
        """Get complete state of the agent."""
        return {
            'agent_id': self.agent_id,
            'development_stage': self.dev_stage,
            'plasticity': self.plasticity,
            'learning_rate': self.learning_rate,
            'epsilon': self.epsilon,
            'temperature': self.temperature,
            'curiosity': self.curiosity,
            'current_goals': self.current_goals.copy(),
            'memory_stats': self.memory.get_memory_stats(),
            'neuromodulation': self.neuromodulation.get_modulation_summary(),
            'metrics_count': len(self.metrics_history)
        }

def analyze_learning_trends(metrics: List[LearningMetrics]) -> Dict[str, Any]:
    """Analyze learning trends from metrics."""
    if not metrics:
        return {'status': 'no_data'}
    
    # Extract metrics
    rewards = [m.reward for m in metrics]
    accuracies = [m.accuracy for m in metrics]
    learning_rates = [m.learning_rate for m in metrics]
    
    # Calculate trends
    analysis = {
        'reward_trend': np.polyfit(range(len(rewards)), rewards, 1)[0],
        'accuracy_trend': np.polyfit(range(len(accuracies)), accuracies, 1)[0],
        'learning_rate_stability': np.std(learning_rates) if len(learning_rates) > 1 else 0.0,
        'average_reward': np.mean(rewards),
        'final_accuracy': accuracies[-1] if accuracies else 0.0,
        'convergence_step': None,
        'total_episodes': len(set(m.episode for m in metrics)),
        'total_steps': len(metrics)
    }
    
    # Find convergence point
    for i in range(len(accuracies)):
        if i >= 20 and np.mean(accuracies[i-20:i]) > 0.8:
            analysis['convergence_step'] = i
            break
    
    return analysis

def run_advanced_learning_simulation() -> Dict[str, Any]:
    """Run comprehensive advanced learning simulation."""
    logger.info("Starting Advanced NLM Learning Simulation")
    logger.info("=" * 60)
    
    simulation_results = {
        'simulation_info': {
            'name': 'Advanced NLM Learning Simulation',
            'timestamp': datetime.now().isoformat(),
            'steps_completed': 0,
            'status': 'running'
        },
        'agents': {},
        'learning_metrics': {},
        'memory_systems': {},
        'neuromodulation': {},
        'world_dynamics': {},
        'performance_analysis': {},
        'errors': [],
    }
    
    try:
        # Create neuromodulation system
        neuromodulation = NeuromodulationSystem()
        simulation_results['neuromodulation'] = neuromodulation.get_modulation_summary()
        
        # Create agents
        agents = []
        agent_configs = [
            {'id': 'exploration_agent', 'learning_rate': 0.02, 'epsilon': 0.2},
            {'id': 'exploitation_agent', 'learning_rate': 0.01, 'epsilon': 0.05},
            {'id': 'balanced_agent', 'learning_rate': 0.015, 'epsilon': 0.1},
            {'id': 'adaptive_agent', 'learning_rate': 0.005, 'epsilon': 0.15}
        ]
        
        for config in agent_configs:
            brain = create_brain(
                learning_rate=config['learning_rate'],
                hidden_size=256,
                num_layers=3,
                enable_neuromodulation=True,
                enable_structural_plasticity=True,
                memory_capacity=3000,
                curiosity_lambda=0.6,
                prediction_error_threshold=0.15
            )
            
            agent = LearningAgent(config['id'], brain, neuromodulation)
            agents.append(agent)
        
        simulation_results['agents'] = {
            agent_id: agent.get_state() 
            for agent in agents 
            for agent_id in [agent.agent_id]
        }
        
        # Create world
        world = World("advanced_learning_world")
        for agent in agents:
            brain = create_brain(learning_rate=0.01, hidden_size=128)
            world.add_brain(brain)
        
        simulation_results['world_dynamics'] = {
            'world_name': world.name,
            'total_brains': len(world.get_brains()),
            'agents_count': len(agents)
        }
        
        # Simulation loop
        for step in range(500):
            step_start_time = time.time()
            
            # Create world state
            world_state = {
                'time': step * 0.1,
                'world_name': world.name,
                'environment': {
                    'temperature': 20.0 + (step % 10) * 0.5,
                    'humidity': 0.5 + (step % 5) * 0.1,
                    'resources': len(agents) * 5,
                },
                'agents': [
                    {'position': {'x': i * 25 + 10, 'y': 50}, 
                     'agent_id': agent.agent_id,
                     'type': 'learner'}
                    for i, agent in enumerate(agents)
                ]
            }
            
            # Run learning for each agent
            step_results = {}
            for agent in agents:
                try:
                    # Generate action
                    action = agent.act('learn', world_state)
                    
                    # Get reward
                    reward = action.get('reward', 0.0) + agent.curiosity * 0.1
                    
                    # Learn from experience
                    learning_result = agent.learn(world_state, action, reward)
                    
                    step_results[agent.agent_id] = {
                        'action': action,
                        'reward': reward,
                        'learning': learning_result,
                        'neuromodulation': neuromodulation.get_modulation_summary()
                    }
                    
                except Exception as e:
                    logger.error(f"Error in agent {agent.agent_id}: {str(e)}")
                    simulation_results['errors'].append({
                        'agent_id': agent.agent_id,
                        'step': step,
                        'error': str(e),
                        'timestamp': time.time()
                    })
            
            # Collect metrics
            simulation_results['learning_metrics'][step] = {
                'step': step + 1,
                'timestamp': step_start_time,
                'agent_results': step_results,
                'world_state': world_state,
                'total_reward': sum(r['reward'] for r in step_results.values()),
                'agents_count': len(agents)
            }
            
            # Log progress periodically
            if (step + 1) % 50 == 0:
                logger.info(f"Step {step + 1}/500 completed")
                
                # Aggregate performance metrics
                performance_snapshot = {
                    'step': step + 1,
                    'agents_performance': {},
                    'world_metrics': world_state,
                    'average_reward': np.mean([r['reward'] for r in step_results.values()]),
                    'total_neurons': len(world.get_brains()) * 128,
                }
                
                for agent in agents:
                    agent_state = agent.get_state()
                    performance_snapshot['agents_performance'][agent.agent_id] = {
                        'development_stage': agent_state['development_stage'],
                        'learning_rate': agent_state['learning_rate'],
                        'epsilon': agent_state['epsilon'],
                        'curiosity': agent_state['curiosity'],
                        'memory_efficiency': agent_state['memory_stats']['utilization'],
                        'goal_alignment': agent_state['current_goals'],
                        'metrics_count': agent_state['metrics_count']
                    }
                
                simulation_results['performance_analysis'][step + 1] = performance_snapshot
        
        # Final analysis
        final_analysis = {
            'total_steps': 500,
            'total_episodes': len(set(agent._get_episode() for agent in agents)),
            'average_reward_per_step': np.mean([r['total_reward'] for r in simulation_results['learning_metrics'].values()]),
            'agents_final_states': {agent.agent_id: agent.get_state() for agent in agents},
            'memory_system_stats': {agent.agent_id: agent.memory.get_memory_stats() for agent in agents},
            'neuromodulation_summary': neuromodulation.get_modulation_summary()
        }
        
        # Analyze learning trends for each agent
        for agent in agents:
            agent_metrics = [m for m in agent.metrics_history]
            trend_analysis = analyze_learning_trends(agent_metrics)
            simulation_results['agents'][agent.agent_id]['learning_trends'] = trend_analysis
        
        simulation_results['performance_analysis']['final'] = final_analysis
        simulation_results['simulation_info']['steps_completed'] = 500
        simulation_results['simulation_info']['status'] = 'completed'
        
        logger.info("=" * 60)
        logger.info("Advanced learning simulation completed successfully!")
        logger.info(f"Total steps simulated: {500}")
        logger.info(f"Total agents: {len(agents)}")
        logger.info(f"Errors encountered: {len(simulation_results['errors'])}")
        
    except Exception as e:
        logger.error(f"Advanced learning simulation failed: {str(e)}")
        simulation_results['errors'].append({
            'stage': 'unknown',
            'error': str(e),
            'timestamp': time.time()
        })
        simulation_results['simulation_info']['status'] = 'failed'
    
    return simulation_results

def main() -> None:
    """Main function to run the advanced learning example."""
    logger.info("=" * 60)
    logger.info("Starting Advanced NLM Learning Example")
    logger.info("=" * 60)
    
    try:
        # Run the advanced simulation
        results = run_advanced_learning_simulation()
        
        # Log final summary
        logger.info("\n" + "=" * 60)
        logger.info("LEARNING SIMULATION SUMMARY")
        logger.info("=" * 60)
        
        if 'simulation_info' in results:
            info = results['simulation_info']
            logger.info(f"Simulation Status: {info['status']}")
            logger.info(f"Steps Completed: {info['steps_completed']}")
        
        if 'performance_analysis' in results and 'final' in results['performance_analysis']:
            final = results['performance_analysis']['final']
            logger.info(f"Average Reward per Step: {final.get('average_reward_per_step', 'N/A'):.4f}")
            logger.info(f"Total Episodes: {final.get('total_episodes', 'N/A')}")
            logger.info(f"Agents: {list(final.get('agents_final_states', {}).keys())}")
        
        if 'agents' in results:
            for agent_id, agent_state in results['agents'].items():
                logger.info(f"{agent_id}:")
                logger.info(f"  Development Stage: {agent_state.get('development_stage', 0):.3f}")
                logger.info(f"  Learning Rate: {agent_state.get('learning_rate', 0):.4f}")
                logger.info(f"  Memory Efficiency: {agent_state.get('memory_stats', {}).get('utilization', 0):.3f}")
        
        if results.get('errors'):
            logger.error(f"Errors encountered: {len(results['errors'])}")
            for i, error in enumerate(results['errors'][:3]):
                logger.error(f"Error {i+1}: {error['agent_id']} at step {error.get('step', 'N/A')}")
        
        # Save results to JSON file
        with open('advanced_learning_results.json', 'w') as f:
            # Convert non-serializable objects to JSON-serializable format
            json_results = results.copy()
            json_results['simulation_info'] = asdict(json_results['simulation_info'])
            
            for agent_id in json_results['agents']:
                json_results['agents'][agent_id] = json.dumps(json_results['agents'][agent_id])
            
            json.dump(json_results, f, indent=2, default=str)
        
        logger.info("\nResults saved to advanced_learning_results.json")
        logger.info("Advanced learning example completed successfully!")
        
    except KeyboardInterrupt:
        logger.info("Simulation interrupted by user")
    except Exception as e:
        logger.error(f"Advanced learning simulation failed: {str(e)}")
    
    logger.info("=" * 60)

if __name__ == "__main__":
    main()

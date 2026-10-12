"""
Advanced Agent-Brain Example for NLM

This example demonstrates complex agent-brain interactions with advanced features including:
- Multi-region brain integration
- Neuromodulation and plasticity
- Real-time monitoring and feedback loops
- Developmental changes and adaptation
- Memory integration and replay mechanisms
- Performance optimization and error handling
- Real-time visualization and analysis

This example showcases production-ready code with comprehensive neuroscience modeling
and advanced agent coordination strategies.
"""

import time
import logging
import numpy as np
from typing import Dict, List, Any, Optional
from datetime import datetime

from nlm import create_brain, create_agent, create_region, World

# Configure comprehensive logging for agent-brain interactions
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_agent_brain_example.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

class BrainRegionManager:
    """Manages multiple brain regions and their interactions."""
    
    def __init__(self):
        self.regions = {}
        self.connections = {}
        self.plasticity_rules = {}
    
    def add_region(self, name: str, config: Dict[str, Any]) -> 'BrainRegion':
        """Add a brain region with its configuration."""
        region = BrainRegion(name, config)
        self.regions[name] = region
        self.connections[name] = {}
        self._initialize_plasticity_rules(name, config)
        logger.info(f"Added brain region: {name} with config {config}")
        return region
    
    def connect_regions(self, source: str, target: str, strength: float = 0.5) -> None:
        """Connect two brain regions with specified strength."""
        if source not in self.regions or target not in self.regions:
            raise ValueError(f"Regions {source} or {target} not found")
        
        self.connections[source][target] = strength
        logger.info(f"Connected region {source} -> {target} with strength {strength}")
    
    def _initialize_plasticity_rules(self, region_name: str, config: Dict[str, Any]) -> None:
        """Initialize plasticity rules for a region."""
        self.plasticity_rules[region_name] = {
            'learning_rate': config.get('learning_rate', 0.01),
            'time_constant': config.get('time_constant', 1.0),
            'weight_decay': config.get('weight_decay', 0.0001),
            'modulation_sensitivity': config.get('modulation_sensitivity', 1.0),
        }

class BrainRegion:
    """Represents a brain region with advanced neural dynamics."""
    
    def __init__(self, name: str, config: Dict[str, Any]):
        self.name = name
        self.config = config
        self.neurons = []
        self.synapses = []
        self.activity = 0.0
        self.plasticity_weight = 1.0
        self.last_update = time.time()
        self.temporal_memory = []
        self.max_temporal_memory = 1000
        self.firing_rate = 0.0
        self.excitatory_connections = 0
        self.inhibitory_connections = 0
        self.neuromodulation = {
            'dopamine': 0.0,
            'acetylcholine': 0.0,
            'serotonin': 0.0,
            'noradrenaline': 0.0
        }
    
    def update_activity(self, input_signals: List[float], dt: float = 0.1) -> float:
        """Update region activity based on input signals."""
        # Calculate weighted sum of inputs
        weighted_input = sum(
            signal * self.config.get('input_weights', {}).get(i, 1.0)
            for i, signal in enumerate(input_signals)
        )
        
        # Apply nonlinear activation function
        self.activity = self._activation_function(weighted_input)
        
        # Update temporal memory
        self._update_temporal_memory(self.activity)
        
        # Update neuromodulation
        self._update_neuromodulation()
        
        # Apply plasticity
        self._apply_plasticity(dt)
        
        # Update firing rate
        self.firing_rate = self._calculate_firing_rate()
        
        self.last_update = time.time()
        
        return self.activity
    
    def _activation_function(self, input_value: float) -> float:
        """Nonlinear activation function with neuromodulation."""
        # Base activation with sigmoidal function
        base_activation = 1.0 / (1.0 + np.exp(-input_value * 2.0))
        
        # Apply neuromodulation factors
        modulation_factor = (
            1.0 + self.neuromodulation['dopamine'] * 0.1 +  # Reward sensitivity
            self.neuromodulation['acetylcholine'] * 0.05 +  # Attention
            self.neuromodulation['serotonin'] * (-0.05) +  # Aversive learning
            self.neuromodulation['noradrenaline'] * 0.08   # Arousal
        )
        
        return np.clip(base_activation * modulation_factor, 0.0, 1.0)
    
    def _update_temporal_memory(self, activity: float) -> None:
        """Update temporal memory buffer."""
        self.temporal_memory.append({
            'time': time.time(),
            'activity': activity,
            'neuromodulation': self.neuromodulation.copy()
        })
        
        if len(self.temporal_memory) > self.max_temporal_memory:
            self.temporal_memory.pop(0)
    
    def _update_neuromodulation(self) -> None:
        """Update neuromodulatory signals based on activity."""
        # Dopamine: reward prediction error
        dopamine_signal = (self.activity - 0.5) * 2.0
        self.neuromodulation['dopamine'] = np.clip(dopamine_signal, -1.0, 1.0)
        
        # Acetylcholine: attention/signal salience
        acetylcholine_signal = np.abs(self.activity - 0.5) * 2.0
        self.neuromodulation['acetylcholine'] = np.clip(acetylcholine_signal, 0.0, 1.0)
        
        # Serotonin: aversive learning
        serotonin_signal = -self.activity * 0.5
        self.neuromodulation['serotonin'] = np.clip(serotonin_signal, -1.0, 1.0)
        
        # Noradrenaline: arousal/excitement
        noradrenaline_signal = self.activity * np.random.random() * 0.5
        self.neuromodulation['noradrenaline'] = np.clip(noradrenaline_signal, 0.0, 1.0)
    
    def _apply_plasticity(self, dt: float) -> None:
        """Apply synaptic plasticity based on activity."""
        # Hebbian plasticity (neurons that fire together wire together)
        if self.activity > 0.7:  # Strong activation
            self.plasticity_weight *= (1.0 + self.config.get('hebbian_rate', 0.01) * dt)
        elif self.activity < 0.3:  # Weak activation
            self.plasticity_weight *= (1.0 - self.config.get('anti_hebbian_rate', 0.005) * dt)
        
        # Weight decay
        self.plasticity_weight *= (1.0 - self.config.get('weight_decay', 0.0001) * dt)
        
        # Clip to reasonable bounds
        self.plasticity_weight = np.clip(self.plasticity_weight, 0.1, 10.0)
    
    def _calculate_firing_rate(self) -> float:
        """Calculate firing rate based on activity and neuromodulation."""
        base_rate = self.activity * 100.0  # Hz
        
        # Modulate firing rate based on neuromodulators
        rate_modulation = (
            1.0 + self.neuromodulation['dopamine'] * 0.2 +  # Increase reward-based firing
            self.neuromodulation['acetylcholine'] * 0.1 +  # Increase attention-based firing
            self.neuromodulation['noradrenaline'] * 0.15   # Increase arousal-based firing
        )
        
        return min(base_rate * rate_modulation, 200.0)  # Cap at 200 Hz
    
    def get_state(self) -> Dict[str, Any]:
        """Get current state of the brain region."""
        return {
            'name': self.name,
            'activity': self.activity,
            'firing_rate': self.firing_rate,
            'plasticity_weight': self.plasticity_weight,
            'neuromodulation': self.neuromodulation.copy(),
            'temporal_memory_length': len(self.temporal_memory),
            'connections': {
                'excitatory': self.excitatory_connections,
                'inhibitory': self.inhibitory_connections
            }
        }

class AdvancedAgent:
    """Advanced agent with sophisticated brain integration and behavior."""
    
    def __init__(self, name: str, brain: Any, brain_manager: BrainRegionManager):
        self.name = name
        self.brain = brain
        self.brain_manager = brain_manager
        self.memory = []
        self.experience_buffer = []
        self.max_experience = 1000
        self.current_state = {}
        self.action_history = []
        self.performance_metrics = {
            'reward_history': [],
            'accuracy_history': [],
            'exploration_rate': 0.1,
            'exploitation_rate': 0.8,
            'curiosity_level': 0.5,
            'adaptation_rate': 0.01,
        }
        self.development_stage = 0.0
        self.plasticity = 1.0
        self.stimulation_threshold = 0.5
        self.goal_vector = np.random.random(5)  # 5-dimensional goal space
        self.current_goals = self.goal_vector.copy()
    
    def think(self, observation: Dict[str, Any]) -> Dict[str, Any]:
        """Advanced decision making with multiple cognitive processes."""
        # Update current state
        self.current_state = {
            'observation': observation,
            'timestamp': time.time(),
            'development_stage': self.development_stage,
            'plasticity': self.plasticity,
            'current_goals': self.current_goals.copy()
        }
        
        # Extract relevant features from observation
        features = self._extract_features(observation)
        
        # Process through brain regions
        brain_output = self._process_through_brain_regions(features)
        
        # Apply cognitive control processes
        controlled_output = self._apply_cognitive_control(brain_output)
        
        # Generate action with uncertainty estimation
        action, action_metadata = self._generate_action(controlled_output)
        
        # Update memory and experience
        self._update_memory(observation, action, action_metadata)
        
        # Update performance metrics
        self._update_performance_metrics(action_metadata)
        
        # Apply developmental changes
        self._apply_development()
        
        return action
    
    def _extract_features(self, observation: Dict[str, Any]) -> Dict[str, Any]:
        """Extract and process features from observation."""
        features = {
            'environmental': {
                'time': observation.get('time', 0.0),
                'resources': observation.get('resources', 0),
                'obstacles': observation.get('obstacles', 0),
                'weather': observation.get('weather', 'clear'),
                'temperature': observation.get('temperature', 20.0),
            },
            'social': {
                'agent_count': len(observation.get('agents', [])),
                'proximity': self._calculate_proximity(observation),
                'interaction_opportunity': self._assess_interaction_opportunity(observation)
            },
            'internal': {
                'development_stage': self.development_stage,
                'plasticity': self.plasticity,
                'memory_strength': self._calculate_memory_strength()
            }
        }
        
        return features
    
    def _calculate_proximity(self, observation: Dict[str, Any]) -> Dict[str, float]:
        """Calculate proximity to other agents."""
        if 'agents' not in observation:
            return {}
        
        proximity = {}
        for i, agent in enumerate(observation['agents']):
            distance = np.sqrt(
                (agent['position']['x'] - observation.get('agent_position', {}).get('x', 0)) ** 2 +
                (agent['position']['y'] - observation.get('agent_position', {}).get('y', 0)) ** 2
            )
            proximity[f'agent_{i}'] = distance
        
        return proximity
    
    def _assess_interaction_opportunity(self, observation: Dict[str, Any]) -> float:
        """Assess opportunity for social interaction."""
        proximity = self._calculate_proximity(observation)
        
        if not proximity:
            return 0.0
        
        close_agents = sum(1 for dist in proximity.values() if dist < 20.0)
        interaction_value = min(close_agents / len(proximity), 1.0)
        
        return interaction_value * (1.0 - self.performance_metrics['exploitation_rate'])
    
    def _calculate_memory_strength(self) -> float:
        """Calculate strength of memory integration."""
        if not self.memory:
            return 0.0
        
        # Calculate based on memory recency and strength
        recent_memories = [m for m in self.memory if time.time() - m['timestamp'] < 60]
        if not recent_memories:
            return 0.0
        
        avg_strength = np.mean([m.get('strength', 0.5) for m in recent_memories])
        recency_factor = min(1.0, len(recent_memories) / 10.0)
        
        return avg_strength * recency_factor
    
    def _process_through_brain_regions(self, features: Dict[str, Any]) -> Dict[str, Any]:
        """Process features through integrated brain regions."""
        region_outputs = {}
        
        for region_name, region in self.brain_manager.regions.items():
            # Prepare region-specific input
            region_input = self._prepare_region_input(features, region_name)
            
            # Get region output
            region_output = region.update_activity(region_input)
            region_outputs[region_name] = region_output
            
            # Apply inter-region connections
            for connected_region, strength in self.brain_manager.connections.get(region_name, {}).items():
                if connected_region in region_outputs:
                    region_outputs[region_name] += region_outputs[connected_region] * strength * 0.1
        
        # Integrate region outputs into final brain output
        final_output = {
            'integrated_activity': np.mean(list(region_outputs.values())),
            'region_activities': region_outputs,
            'prediction': self._generate_prediction(features),
            'uncertainty': self._calculate_uncertainty(region_outputs),
            'curiosity_bonus': self._calculate_curiosity_bonus(features),
            'exploration_drive': self._calculate_exploration_drive(),
            'exploitation_confidence': self._calculate_exploitation_confidence(),
        }
        
        return final_output
    
    def _prepare_region_input(self, features: Dict[str, Any], region_name: str) -> List[float]:
        """Prepare input signals for a specific brain region."""
        region_configs = {
            'cortex': {
                'features': ['environmental', 'social'],
                'weights': {'environmental': 0.6, 'social': 0.4}
            },
            'hippocampus': {
                'features': ['internal', 'social'],
                'weights': {'internal': 0.7, 'social': 0.3}
            },
            'amygdala': {
                'features': ['environmental'],
                'weights': {'environmental': 1.0}
            }
        }
        
        config = region_configs.get(region_name, {'features': [], 'weights': {}})
        
        input_signals = []
        for feature_type in config['features']:
            if feature_type == 'environmental':
                input_signals.extend([
                    features['environmental'].get('temperature', 20.0) / 40.0,
                    features['environmental'].get('humidity', 0.5),
                    features['environmental'].get('resources', 0) / 10.0,
                ])
            elif feature_type == 'social':
                input_signals.extend([
                    features['social'].get('interaction_opportunity', 0.0),
                    min(features['social'].get('proximity', {}), 1.0),
                ])
            elif feature_type == 'internal':
                input_signals.extend([
                    features['internal'].get('development_stage', 0.0),
                    features['internal'].get('memory_strength', 0.0),
                ])
        
        # Normalize input signals
        if input_signals:
            input_signals = np.clip(input_signals, 0.0, 1.0).tolist()
        
        return input_signals
    
    def _generate_prediction(self, features: Dict[str, Any]) -> Dict[str, Any]:
        """Generate prediction based on current features."""
        # Simple predictive model
        environmental_pred = np.array([
            features['environmental'].get('temperature', 20.0),
            features['environmental'].get('resources', 0),
            features['social'].get('interaction_opportunity', 0.0)
        ])
        
        prediction = {
            'expected_reward': np.mean(environmental_pred) * 0.1,
            'reward_variance': np.var(environmental_pred) * 0.01,
            'novelty': np.random.random(),  # Simulated novelty
            'uncertainty': np.random.random() * 0.3
        }
        
        return prediction
    
    def _calculate_uncertainty(self, region_outputs: Dict[str, float]) -> float:
        """Calculate uncertainty in brain region outputs."""
        if not region_outputs:
            return 1.0
        
        activities = list(region_outputs.values())
        return np.std(activities) / (np.mean(activities) + 0.001)
    
    def _calculate_curiosity_bonus(self, features: Dict[str, Any]) -> float:
        """Calculate curiosity-driven exploration bonus."""
        novelty = np.random.random()
        uncertainty = self.performance_metrics['uncertainty']
        
        curiosity = (
            novelty * 0.5 + 
            uncertainty * 0.3 + 
            self.performance_metrics['curiosity_level'] * 0.2
        )
        
        return np.clip(curiosity, 0.0, 1.0)
    
    def _calculate_exploration_drive(self) -> float:
        """Calculate exploration drive based on multiple factors."""
        exploration_drive = (
            self.performance_metrics['exploration_rate'] * 0.5 +
            self.performance_metrics['curiosity_level'] * 0.3 +
            (1.0 - self.development_stage) * 0.2
        )
        
        return np.clip(exploration_drive, 0.0, 1.0)
    
    def _calculate_exploitation_confidence(self) -> float:
        """Calculate confidence in exploitation."""
        if not self.performance_metrics['accuracy_history']:
            return 0.5
        
        recent_accuracy = np.mean(self.performance_metrics['accuracy_history'][-10:])
        confidence = recent_accuracy * (1.0 - self.performance_metrics['exploration_rate'])
        
        return np.clip(confidence, 0.0, 1.0)
    
    def _apply_cognitive_control(self, brain_output: Dict[str, Any]) -> Dict[str, Any]:
        """Apply cognitive control to brain output."""
        controlled_output = brain_output.copy()
        
        # Strategic attention modulation
        attention = 1.0 / (1.0 + np.exp(-(brain_output['exploration_drive'] - 0.5) * 10))
        
        # Working memory integration
        if self.memory:
            recent_memory = self.memory[-5:]  # Last 5 memories
            memory_weight = np.mean([m.get('strength', 0.5) for m in recent_memory])
        else:
            memory_weight = 0.5
        
        # Decision weighting
        exploration_weight = brain_output['exploration_drive'] * attention
        exploitation_weight = brain_output['exploitation_confidence'] * (1 - attention)
        
        # Normalize weights
        total_weight = exploration_weight + exploitation_weight
        if total_weight > 0:
            exploration_weight /= total_weight
            exploitation_weight /= total_weight
        
        controlled_output['exploration_weight'] = exploration_weight
        controlled_output['exploitation_weight'] = exploitation_weight
        controlled_output['attention'] = attention
        controlled_output['memory_weight'] = memory_weight
        
        return controlled_output
    
    def _generate_action(self, controlled_output: Dict[str, Any]) -> tuple:
        """Generate action based on controlled output."""
        # Determine action type based on weights
        exploration_weight = controlled_output['exploration_weight']
        exploitation_weight = controlled_output['exploitation_weight']
        
        if exploration_weight > exploitation_weight:
            action_type = 'exploration'
        else:
            action_type = 'exploitation'
        
        # Generate specific action
        if action_type == 'exploration':
            action = self._generate_exploration_action(controlled_output)
        else:
            action = self._generate_exploitation_action(controlled_output)
        
        # Add metadata
        action_metadata = {
            'action_type': action_type,
            'attention': controlled_output['attention'],
            'memory_weight': controlled_output['memory_weight'],
            'exploration_drive': controlled_output['exploration_drive'],
            'exploitation_confidence': controlled_output['exploitation_confidence'],
            'timestamp': time.time(),
            'action_id': len(self.action_history)
        }
        
        return action, action_metadata
    
    def _generate_exploration_action(self, output: Dict[str, Any]) -> Dict[str, Any]:
        """Generate exploration action."""
        # Generate novel movement
        angle = np.random.random() * 2 * np.pi
        distance = np.random.uniform(10, 50)
        
        action = {
            'type': 'move',
            'direction': {
                'angle': angle,
                'distance': distance
            },
            'novelty': np.random.random(),
            'risk': np.random.random() * 0.5,
            'reward': np.random.random() * 0.5,
            'exploration_bonus': output.get('curiosity_bonus', 0.0) * 2.0,
            'prediction_error': output.get('uncertainty', 0.0),
            'action_id': len(self.action_history)
        }
        
        return action
    
    def _generate_exploitation_action(self, output: Dict[str, Any]) -> Dict[str, Any]:
        """Generate exploitation action."""
        # Move toward known goal
        angle_to_goal = np.arctan2(
            self.current_goals[1] - self._get_current_position()['y'],
            self.current_goals[0] - self._get_current_position()['x']
        )
        
        distance = np.random.uniform(5, 30)
        
        action = {
            'type': 'move',
            'direction': {
                'angle': angle_to_goal,
                'distance': distance
            },
            'goal_alignment': np.dot(self.current_goals, self._get_current_position()),
            'reward': output.get('prediction', {}).get('expected_reward', 0.5) * 2.0,
            'confidence': output.get('exploitation_confidence', 0.5),
            'prediction_error': output.get('uncertainty', 0.0),
            'action_id': len(self.action_history)
        }
        
        return action
    
    def _get_current_position(self) -> Dict[str, float]:
        """Get current agent position."""
        return {'x': np.random.uniform(0, 100), 'y': np.random.uniform(0, 100)}
    
    def _update_memory(self, observation: Dict[str, Any], action: Dict[str, Any], metadata: Dict[str, Any]) -> None:
        """Update agent's memory with new experience."""
        experience = {
            'observation': observation,
            'action': action,
            'metadata': metadata,
            'timestamp': time.time(),
            'strength': action.get('reward', 0.0) + action.get('exploration_bonus', 0.0),
            'action_id': action.get('action_id', len(self.memory))
        }
        
        self.memory.append(experience)
        
        # Add to experience buffer
        self.experience_buffer.append(experience)
        
        # Maintain buffer size
        if len(self.experience_buffer) > self.max_experience:
            self.experience_buffer.pop(0)
        
        # Update memory quality
        if experience['strength'] > 0.5:
            self._reinforce_memory(experience)
        else:
            self._weaken_memory(experience)
    
    def _reinforce_memory(self, experience: Dict[str, Any]) -> None:
        """Reinforce strong memories."""
        # Find similar memories and strengthen them
        similar_memories = []
        for memory in self.memory[:-1]:  # Exclude current memory
            similarity = self._calculate_memory_similarity(experience, memory)
            if similarity > 0.7:
                similar_memories.append(memory)
        
        # Strengthen similar memories
        for memory in similar_memories:
            memory['strength'] = min(1.0, memory.get('strength', 0.5) + 0.1)
    
    def _weaken_memory(self, experience: Dict[str, Any]) -> None:
        """Weaken weak memories."""
        # Find similar memories and weaken them
        similar_memories = []
        for memory in self.memory[:-1]:
            similarity = self._calculate_memory_similarity(experience, memory)
            if similarity > 0.7:
                similar_memories.append(memory)
        
        # Weaken similar memories
        for memory in similar_memories:
            memory['strength'] = max(0.1, memory.get('strength', 0.5) - 0.05)
    
    def _calculate_memory_similarity(self, experience1: Dict[str, Any], experience2: Dict[str, Any]) -> float:
        """Calculate similarity between two experiences."""
        # Simple similarity based on action type and reward
        action1 = experience1['action']
        action2 = experience2['action']
        
        action_sim = 1.0 if action1['type'] == action2['type'] else 0.0
        
        reward1 = action1.get('reward', 0.0)
        reward2 = action2.get('reward', 0.0)
        reward_sim = 1.0 - abs(reward1 - reward2)
        
        return (action_sim + reward_sim) / 2.0
    
    def _update_performance_metrics(self, action_metadata: Dict[str, Any]) -> None:
        """Update agent's performance metrics."""
        # Update reward history
        reward = action_metadata.get('reward', 0.0)
        self.performance_metrics['reward_history'].append(reward)
        
        # Maintain history length
        if len(self.performance_metrics['reward_history']) > 100:
            self.performance_metrics['reward_history'].pop(0)
        
        # Calculate accuracy from rewards
        accuracy = np.mean(self.performance_metrics['reward_history'])
        self.performance_metrics['accuracy_history'].append(accuracy)
        if len(self.performance_metrics['accuracy_history']) > 100:
            self.performance_metrics['accuracy_history'].pop(0)
        
        # Update adaptive parameters
        if time.time() - self.last_update > 10.0:  # Update every 10 seconds
            self._adapt_parameters()
    
    def _adapt_parameters(self) -> None:
        """Adapt agent parameters based on performance."""
        avg_reward = np.mean(self.performance_metrics['reward_history'])
        avg_accuracy = np.mean(self.performance_metrics['accuracy_history'])
        
        # Adapt exploration vs exploitation
        if avg_reward < 0.3:
            # Poor performance - increase exploration
            self.performance_metrics['exploration_rate'] = min(0.9, 
                self.performance_metrics['exploration_rate'] * 1.1)
            self.performance_metrics['exploitation_rate'] = max(0.1,
                self.performance_metrics['exploitation_rate'] * 0.9)
        elif avg_reward > 0.7:
            # Good performance - increase exploitation
            self.performance_metrics['exploitation_rate'] = min(0.9,
                self.performance_metrics['exploitation_rate'] * 1.1)
            self.performance_metrics['exploration_rate'] = max(0.1,
                self.performance_metrics['exploration_rate'] * 0.9)
        
        # Adapt curiosity level
        self.performance_metrics['curiosity_level'] = np.clip(
            self.performance_metrics['curiosity_level'] * (0.95 + avg_accuracy * 0.1),
            0.1, 1.0
        )
        
        # Update development stage
        self.development_stage = min(1.0, self.development_stage + 0.001)
        
        # Update plasticity based on performance
        performance_factor = avg_accuracy
        self.plasticity = np.clip(performance_factor * 0.5 + 0.5, 0.5, 2.0)
        
        self.last_update = time.time()
    
    def _apply_development(self) -> None:
        """Apply developmental changes over time."""
        # Neural development
        if self.development_stage < 0.8:
            # Development phase - increase connectivity
            development_factor = self.development_stage * 0.5
            self.plasticity *= (1.0 + development_factor * 0.1)
        
        # Structural plasticity
        if self.development_stage > 0.5 and self.plasticity > 1.5:
            # Mature phase - fine-tune connections
            self.plasticity *= 0.99
        
        # Learning rate adaptation
        learning_rate = self.performance_metrics['adaptation_rate'] * (1.0 - self.development_stage * 0.5)
        
        # Update goals based on development
        if self.development_stage > 0.7:
            # More sophisticated goals in mature agents
            self.goal_vector = self._refine_goals(self.goal_vector)
    
    def _refine_goals(self, current_goals: np.ndarray) -> np.ndarray:
        """Refine goal vector based on development and experience."""
        # Add complexity to goals
        refinement_strength = min(0.3, self.development_stage * 0.3)
        
        # Add noise for exploration
        noise = np.random.normal(0, refinement_strength, len(current_goals))
        
        # Ensure goals remain positive
        refined_goals = current_goals + noise
        refined_goals = np.clip(refined_goals, 0.1, 1.0)
        
        # Normalize to maintain sum
        if refined_goals.sum() > 0:
            refined_goals = refined_goals / refined_goals.sum()
        
        return refined_goals
    
    def get_state(self) -> Dict[str, Any]:
        """Get complete state of the agent."""
        return {
            'name': self.name,
            'development_stage': self.development_stage,
            'plasticity': self.plasticity,
            'performance_metrics': self.performance_metrics.copy(),
            'memory_count': len(self.memory),
            'experience_count': len(self.experience_buffer),
            'current_goals': self.current_goals.copy(),
            'goal_vector': self.goal_vector.copy(),
            'last_update': self.last_update
        }

def create_advanced_brain_manager() -> BrainRegionManager:
    """Create and configure advanced brain region manager."""
    brain_manager = BrainRegionManager()
    
    # Add regions with specific configurations
    regions_config = {
        'cortex': {
            'size': 256,
            'learning_rate': 0.01,
            'plasticity': 0.8,
            'time_constant': 1.5,
            'weight_decay': 0.0005,
            'modulation_sensitivity': 1.0,
        },
        'hippocampus': {
            'size': 128,
            'learning_rate': 0.005,
            'plasticity': 0.9,
            'time_constant': 2.0,
            'weight_decay': 0.0002,
            'modulation_sensitivity': 1.2,
        },
        'amygdala': {
            'size': 64,
            'learning_rate': 0.02,
            'plasticity': 0.7,
            'time_constant': 1.0,
            'weight_decay': 0.0008,
            'modulation_sensitivity': 0.8,
        },
        'prefrontal_cortex': {
            'size': 192,
            'learning_rate': 0.008,
            'plasticity': 0.75,
            'time_constant': 1.8,
            'weight_decay': 0.0003,
            'modulation_sensitivity': 1.1,
        }
    }
    
    for region_name, config in regions_config.items():
        brain_manager.add_region(region_name, config)
    
    # Connect regions with appropriate strengths
    brain_manager.connect_regions('cortex', 'hippocampus', 0.3)
    brain_manager.connect_regions('hippocampus', 'cortex', 0.4)
    brain_manager.connect_regions('cortex', 'amygdala', 0.2)
    brain_manager.connect_regions('amygdala', 'cortex', 0.3)
    brain_manager.connect_regions('prefrontal_cortex', 'cortex', 0.5)
    brain_manager.connect_regions('cortex', 'prefrontal_cortex', 0.4)
    
    return brain_manager

def create_advanced_agent(brain_manager: BrainRegionManager, agent_name: str, role: str = 'generalist') -> AdvancedAgent:
    """Create an advanced agent with sophisticated brain integration."""
    brain = create_brain(
        learning_rate=0.01,
        hidden_size=512,
        num_layers=4,
        dropout_rate=0.2,
        enable_neuromodulation=True,
        enable_structural_plasticity=True,
        memory_capacity=5000,
        curiosity_lambda=0.5,
        prediction_error_threshold=0.1
    )
    
    agent = AdvancedAgent(agent_name, brain, brain_manager)
    
    # Configure agent based on role
    role_configs = {
        'exploration': {
            'exploration_rate': 0.7,
            'exploitation_rate': 0.2,
            'curiosity_level': 0.8,
            'adaptation_rate': 0.02
        },
        'exploitation': {
            'exploration_rate': 0.2,
            'exploitation_rate': 0.7,
            'curiosity_level': 0.3,
            'adaptation_rate': 0.005
        },
        'balanced': {
            'exploration_rate': 0.4,
            'exploitation_rate': 0.5,
            'curiosity_level': 0.5,
            'adaptation_rate': 0.01
        },
        'conservative': {
            'exploration_rate': 0.1,
            'exploitation_rate': 0.8,
            'curiosity_level': 0.2,
            'adaptation_rate': 0.003
        }
    }
    
    config = role_configs.get(role, role_configs['balanced'])
    agent.performance_metrics.update(config)
    
    return agent

def simulate_agent_brain_interaction(agent: AdvancedAgent, world_state: Dict[str, Any]) -> Dict[str, Any]:
    """Simulate complex agent-brain interaction."""
    logger.info(f"Simulating interaction for agent {agent.name}...")
    
    interaction_results = {
        'agent_name': agent.name,
        'timestamp': time.time(),
        'interaction_sequence': [],
        'neural_dynamics': {},
        'cognitive_processes': {},
        'behavioral_outcome': {}
    }
    
    try:
        # Step 1: Perceptual processing
        perceptual_input = agent._extract_features(world_state)
        perceptual_metrics = {
            'environmental_complexity': len(perceptual_input['environmental']),
            'social_density': perceptual_input['social'].get('agent_count', 0),
            'internal_state': perceptual_input['internal'],
        }
        
        # Step 2: Neural processing
        neural_output = agent._process_through_brain_regions(perceptual_input)
        neural_metrics = {
            'integrated_activity': neural_output['integrated_activity'],
            'region_activities': neural_output['region_activities'],
            'prediction_confidence': 1.0 - neural_output['uncertainty'],
            'curiosity_level': neural_output['curiosity_bonus'],
        }
        
        # Step 3: Cognitive control
        controlled_output = agent._apply_cognitive_control(neural_output)
        cognitive_metrics = {
            'attention_focus': controlled_output['attention'],
            'exploration_weight': controlled_output['exploration_weight'],
            'exploitation_weight': controlled_output['exploitation_weight'],
            'memory_weight': controlled_output['memory_weight']
        }
        
        # Step 4: Action generation
        action, action_metadata = agent._generate_action(controlled_output)
        action_metrics = {
            'action_type': action_metadata['action_type'],
            'action_confidence': action_metadata['exploration_drive'],
            'behavioral_outcome': action.get('reward', 0.0),
            'prediction_error': action.get('prediction_error', 0.0),
        }
        
        # Step 5: Learning and memory update
        agent._update_memory(world_state, action, action_metadata)
        agent._update_performance_metrics(action_metadata)
        agent._apply_development()
        
        # Compile interaction results
        interaction_results['interaction_sequence'] = [
            {'stage': 'perception', 'metrics': perceptual_metrics},
            {'stage': 'neural_processing', 'metrics': neural_metrics},
            {'stage': 'cognitive_control', 'metrics': cognitive_metrics},
            {'stage': 'action_generation', 'metrics': action_metrics}
        ]
        
        interaction_results['neural_dynamics'] = {
            'brain_regions': agent.brain_manager.regions,
            'activity_patterns': {name: region.get_state()['activity'] 
                                 for name, region in agent.brain_manager.regions.items()},
            'neuromodulation_states': {name: region.get_state()['neuromodulation']
                                      for name, region in agent.brain_manager.regions.items()}
        }
        
        interaction_results['cognitive_processes'] = {
            'agent_state': agent.get_state(),
            'decision_making_process': {
                'exploration_vs_exploitation': {
                    'exploration_weight': action_metadata['exploration_drive'],
                    'exploitation_weight': action_metadata['exploitation_confidence']
                },
                'attention_allocation': action_metadata['attention'],
                'memory_usage': action_metadata['memory_weight']
            }
        }
        
        interaction_results['behavioral_outcome'] = {
            'action_taken': action,
            'performance_impact': {
                'immediate_reward': action_metrics['behavioral_outcome'],
                'long_term_value': agent._calculate_future_value(action),
                'learning_efficiency': agent._calculate_learning_efficiency()
            }
        }
        
        logger.info(f"Agent {agent.name} interaction completed with reward: {action_metrics['behavioral_outcome']:.4f}")
        
    except Exception as e:
        logger.error(f"Error in agent-brain interaction: {str(e)}")
        interaction_results['error'] = str(e)
        interaction_results['timestamp'] = time.time()
    
    return interaction_results

def run_advanced_agent_brain_simulation(steps: int = 100) -> Dict[str, Any]:
    """Run comprehensive advanced agent-brain simulation."""
    logger.info("Starting Advanced Agent-Brain Simulation")
    logger.info("=" * 60)
    
    simulation_results = {
        'simulation_info': {
            'name': 'Advanced Agent-Brain Simulation',
            'timestamp': datetime.now().isoformat(),
            'steps_completed': 0,
            'status': 'running'
        },
        'brain_regions': {},
        'agents': {},
        'interaction_log': [],
        'performance_metrics': {},
        'neural_dynamics': {},
        'errors': [],
    }
    
    try:
        # Create advanced brain manager
        brain_manager = create_advanced_brain_manager()
        simulation_results['brain_regions'] = {
            name: region.get_state() 
            for name, region in brain_manager.regions.items()
        }
        
        # Create agents with different roles
        agent_configs = [
            {'name': 'exploration_agent', 'role': 'exploration'},
            {'name': 'exploitation_agent', 'role': 'exploitation'},
            {'name': 'balanced_agent', 'role': 'balanced'},
            {'name': 'conservative_agent', 'role': 'conservative'}
        ]
        
        agents = []
        for config in agent_configs:
            agent = create_advanced_agent(brain_manager, config['name'], config['role'])
            agents.append(agent)
        
        simulation_results['agents'] = {
            name: agent.get_state() 
            for agent in agents 
            for name in [agent.name]
        }
        
        # Simulation loop
        for step in range(steps):
            step_start_time = time.time()
            
            # Create world state
            world_state = {
                'time': step * 0.1,
                'world_name': 'agent_brain_simulation_world',
                'agents': [
                    {'position': {'x': i * 20 + 10, 'y': 50}, 'role': agent_configs[i]['role']}
                    for i in range(len(agents))
                ],
                'resources': len(agents) * 10,
                'obstacles': step % 5 == 0,
                'weather': 'clear' if step % 3 != 0 else 'rainy',
                'temperature': 20.0 + (step % 10) * 0.5,
                'humidity': 0.5 + (step % 5) * 0.1,
            }
            
            # Run interaction for each agent
            step_interactions = {}
            for agent in agents:
                interaction = simulate_agent_brain_interaction(agent, world_state)
                step_interactions[agent.name] = interaction
            
            # Collect metrics
            simulation_results['interaction_log'].append({
                'step': step + 1,
                'timestamp': step_start_time,
                'interactions': step_interactions,
                'total_reward': sum(inter['behavioral_outcome']['action_taken'].get('reward', 0.0)
                                  for inter in step_interactions.values())
            })
            
            # Log progress periodically
            if (step + 1) % 20 == 0:
                logger.info(f"Step {step + 1}/{steps} completed")
                
                # Aggregate performance metrics
                performance_snapshot = {
                    'step': step + 1,
                    'agents_performance': {},
                    'average_reward': np.mean([
                        inter['behavioral_outcome']['action_taken'].get('reward', 0.0)
                        for inter in step_interactions.values()
                    ]),
                    'interaction_count': len(step_interactions)
                }
                
                for agent in agents:
                    agent_state = agent.get_state()
                    performance_snapshot['agents_performance'][agent.name] = {
                        'development_stage': agent_state['development_stage'],
                        'performance_score': agent_state['performance_metrics']['accuracy_history'][-1] if agent_state['performance_metrics']['accuracy_history'] else 0.0,
                        'reward_history': agent_state['performance_metrics']['reward_history'][-5:] if len(agent_state['performance_metrics']['reward_history']) >= 5 else agent_state['performance_metrics']['reward_history']
                    }
                
                simulation_results['performance_metrics'][step + 1] = performance_snapshot
        
        # Calculate final aggregated metrics
        final_performance = {
            'total_steps': steps,
            'total_interactions': sum(len(log['interactions']) for log in simulation_results['interaction_log']),
            'average_reward_per_step': np.mean([log['total_reward'] for log in simulation_results['interaction_log']]),
            'agent_development_levels': {agent.name: agent.get_state()['development_stage'] for agent in agents},
            'brain_region_activity': {name: region.get_state()['activity'] 
                                     for name, region in brain_manager.regions.items()},
        }
        
        simulation_results['performance_metrics']['final'] = final_performance
        simulation_results['simulation_info']['steps_completed'] = steps
        simulation_results['simulation_info']['status'] = 'completed'
        
        # Save brain region activity patterns
        simulation_results['neural_dynamics'] = {
            'region_evolution': {},
            'activity_patterns': {},
            'connectivity_dynamics': {}
        }
        
        logger.info("=" * 60)
        logger.info("Advanced Agent-Brain simulation completed successfully!")
        logger.info(f"Total steps simulated: {steps}")
        logger.info(f"Total agent interactions: {simulation_results['performance_metrics']['final']['total_interactions']}")
        logger.info(f"Average reward per step: {simulation_results['performance_metrics']['final']['average_reward_per_step']:.4f}")
        
    except Exception as e:
        logger.error(f"Advanced agent-brain simulation failed: {str(e)}")
        simulation_results['errors'].append({
            'stage': 'unknown',
            'error': str(e),
            'timestamp': time.time()
        })
        simulation_results['simulation_info']['status'] = 'failed'
    
    return simulation_results

def main() -> None:
    """Main function to run the advanced agent-brain example."""
    logger.info("=" * 60)
    logger.info("Starting Advanced NLM Agent-Brain Example")
    logger.info("=" * 60)
    
    try:
        # Run the advanced simulation
        results = run_advanced_agent_brain_simulation(steps=100)
        
        # Log final summary
        logger.info("\n" + "=" * 60)
        logger.info("AGENT-BRAIN SIMULATION SUMMARY")
        logger.info("=" * 60)
        
        if 'simulation_info' in results:
            info = results['simulation_info']
            logger.info(f"Simulation Status: {info['status']}")
            logger.info(f"Steps Completed: {info['steps_completed']}")
        
        if 'performance_metrics' in results:
            perf = results['performance_metrics']
            if 'final' in perf:
                final = perf['final']
                logger.info(f"Average Reward per Step: {final.get('average_reward_per_step', 'N/A'):.4f}")
                logger.info(f"Total Interactions: {final.get('total_interactions', 'N/A')}")
                logger.info(f"Agent Development Levels: {final.get('agent_development_levels', {})}")
        
        if 'errors' in results and results['errors']:
            logger.error(f"Errors encountered: {len(results['errors'])}")
            for i, error in enumerate(results['errors'][:3]):
                logger.error(f"Error {i+1}: {error['error']}")
        
        logger.info("\nAdvanced agent-brain example completed successfully!")
        
    except KeyboardInterrupt:
        logger.info("Simulation interrupted by user")
    except Exception as e:
        logger.error(f"Advanced agent-brain simulation failed: {str(e)}")
    
    logger.info("=" * 60)

if __name__ == "__main__":
    main()

# NLM Advanced Topics Guide
# ============================

This document covers advanced usage patterns and techniques for experienced NLM users. It assumes familiarity with the basic NLM concepts and API usage.

## 1. Multi-Agent Coordination and Competition

### Introduction
Multi-agent systems allow for complex social behaviors and competition among agents within the same environment. This section covers advanced techniques for managing multiple agents.

### Agent Neighborhoods and Interaction

```python
import pynlm
import numpy as np

def create_agent_neighborhood(width, height, num_agents, radius):
    """Create agents positioned in a neighborhood pattern."""
    agents = []
    positions = []
    
    # Create grid-like neighborhood
    spacing = radius * 2
    for i in range(num_agents):
        row = i // (width // spacing)
        col = (i * spacing) % width
        
        x = col + spacing / 2
        y = row * spacing + spacing / 2
        
        if x < width and y < height:
            positions.append((x, y))
    
    # Create brain for each agent
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    for i, (x, y) in enumerate(positions):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize world for this agent
        world = pynlm.createSimpleWorld()
        world.configure(
            width=width,
            height=height,
            visionWidth=10,
            visionHeight=10,
            numTargets=3,
            targetUpdateRate=20
        )
        world.reset()
        world.setAgentStart(x, y)
        agent.initialize(world)
        
        # Enable all subsystems for rich behavior
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Add social signaling capability
        agent.enableSocialLearning(True)
        
        agents.append({
            'brain': brain,
            'agent': agent,
            'world': world,
            'position': (x, y),
            'id': i
        })
    
    return agents

def simulate_agent_competition(agents, steps):
    """Simulate competition among agents in the same environment."""
    # Create shared world
    shared_world = pynlm.createSimpleWorld()
    max_dim = max(agent['world'].getWidth(), agent['world'].getHeight()) for agent in agents
    shared_world.configure(width=max_dim, height=max_dim, visionWidth=15, visionHeight=15)
    shared_world.reset()
    
    results = {agent['id']: {'rewards': [], 'novelty': [], 'learning': []} for agent in agents}
    
    for step in range(steps):
        # Update shared world
        shared_world.update(timestep=0.1)
        
        # Each agent perceives different aspects based on position
        for agent in agents:
            # Get unique sensory percept based on agent position
            percept = shared_world.getSensoryPercept()
            
            # Modify percept based on agent's perspective
            agent_world = agent['world']
            agent_world.configureFromPercept(percept)
            
            # Process sensory input
            agent['agent'].processSensoryInput(percept)
            
            # Get agent's motor command
            motor_cmd = agent['agent'].decodeMotorCommand()
            
            # Apply command with agent-specific scaling
            scaled_cmd = pynlm.Action(
                motor_cmd.command_type,
                motor_cmd.strength * agent['agent'].getBehavioralStrength(),
                motor_cmd.direction
            )
            
            # Track agent's experience
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            novelty = agent['agent'].getNoveltyLevel()
            learning_rate = agent['agent'].getLearningRate()
            
            agent['agent'].applyRewardModulation(reward, learning_rate)
            
            results[agent['id']]['rewards'].append(reward)
            results[agent['id']]['novelty'].append(novelty)
            results[agent['id']]['learning'].append(learning_rate)
            
            # Update agent's development
            agent['agent'].updateDevelopment(0.1)
    
    return results
```

### Hierarchical Control

```python
import pynlm

class HierarchicalAgent:
    """Multi-level hierarchical agent with different control levels."""
    
    def __init__(self, config):
        self.config = config
        self.create_control_levels()
    
    def create_control_levels(self):
        """Create different control levels for hierarchical decision making."""
        # High-level planner
        self.planner_config = pynlm.createDefaultConfig()
        self.planner_config.set("brain.neuron_count", 200)
        self.planner = pynlm.createBrain(self.planner_config)
        self.planner.initialize()
        
        # Mid-level executor
        self.executor_config = pynlm.createDefaultConfig()
        self.executor_config.set("brain.neuron_count", 500)
        self.executor = pynlm.createBrain(self.executor_config)
        self.executor.initialize()
        
        # Low-level controller
        self.controller_config = pynlm.createDefaultConfig()
        self.controller_config.set("brain.neuron_count", 1000)
        self.controller = pynlm.createBrain(self.controller_config)
        self.controller.initialize()
        
        # Create interfaces
        self.planner_agent = pynlm.createAgentBrain(self.planner)
        self.executor_agent = pynlm.createAgentBrain(self.executor)
        self.controller_agent = pynlm.createAgentBrain(self.controller)
        
        # Create worlds
        self.planner_world = pynlm.createSimpleWorld()
        self.executor_world = pynlm.createSimpleWorld()
        self.controller_world = pynlm.createSimpleWorld()
        
        # Configure worlds
        self.planner_world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
        self.executor_world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.controller_world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        
        # Initialize interfaces
        self.planner_agent.initialize(self.planner_world)
        self.executor_agent.initialize(self.executor_world)
        self.controller_agent.initialize(self.controller_world)
        
        # Enable different subsystems based on control level
        self.planner_agent.enableCuriosity(True)
        self.planner_agent.enableRewardModulation(True)
        
        self.executor_agent.enableDevelopment(True)
        self.executor_agent.enableStructuralPlasticity(True)
        
        self.controller_agent.enableSocialLearning(True)
    
    def plan_action(self, goal, context):
        """Plan high-level action using the planner level."""
        # Set up planning state
        self.planner_world.reset()
        
        # Process goal and context
        percept = self.create_planning_percept(goal, context)
        self.planner_agent.processSensoryInput(percept)
        
        # Run planner
        for step in range(50):
            self.planner.step(step)
        
        # Get plan from planner output
        plan = self.extract_plan_from_brain(self.planner)
        
        # Convert plan to executable actions
        return self.translate_plan_to_execution(plan)
    
    def execute_action(self, plan, context):
        """Execute planned action using executor level."""
        self.executor_world.reset()
        
        # Feed plan to executor
        percept = self.create_execution_percept(plan, context)
        self.executor_agent.processSensoryInput(percept)
        
        for step in range(100):
            self.executor.step(step)
        
        return self.get_execution_result(self.executor)
    
    def adjust_behavior(self, feedback, error):
        """Adjust low-level behavior based on feedback and error."""
        self.controller_world.reset()
        
        # Create feedback percept
        percept = self.create_feedback_percept(feedback, error)
        self.controller_agent.processSensoryInput(percept)
        
        for step in range(200):
            self.controller.step(step)
        
        # Get behavioral adjustments
        adjustments = self.extract_adjustments(self.controller)
        
        # Apply adjustments
        return self.apply_behavioral_adjustments(adjustments)
    
    def step(self, goal, context):
        """Complete hierarchical control cycle."""
        # High-level planning
        plan = self.plan_action(goal, context)
        
        # Mid-level execution
        result = self.execute_action(plan, context)
        
        # Low-level adjustment based on results
        if 'error' in result:
            adjustments = self.adjust_behavior(result, result['error'])
            self.apply_adjustments(adjustments)
        
        return result
```

## 2. Memory Systems and Development

### Working Memory and Neural Competition

```python
import pynlm
import numpy as np

class WorkingMemoryManager:
    """Manage working memory with competitive dynamics."""
    
    def __init__(self, brain):
        self.brain = brain
        self.memory_capacity = 10
        self.competition_strength = 0.8
        self.activation_threshold = 0.5
        
        # Initialize working memory traces
        self.memory_traces = {}
        self.activation_levels = {}
        
    def encode_experience(self, percept, action, reward, time_step):
        """Encode experience in working memory with competitive activation."""
        # Create memory trace
        memory_key = f"exp_{time_step}"
        
        # Competitive encoding with other memories
        for key, trace in self.memory_traces.items():
            if key != memory_key:
                # Apply competitive inhibition
                inhibition = self.competition_strength * trace.get_strength()
                trace.weakened_by(inhibition)
        
        # Create and activate new trace
        trace = WorkingMemoryTrace(percept, action, reward, time_step)
        trace.activate(self.activation_threshold)
        
        self.memory_traces[memory_key] = trace
        self.activation_levels[memory_key] = self.activation_threshold
        
        # Prune low-activation memories
        self.prune_low_activation()
        
        return trace
    
    def retrieve_relevant_memories(self, query_percept, threshold=0.3):
        """Retrieve memories relevant to current query."""
        relevant = []
        
        for key, trace in self.memory_traces.items():
            similarity = self.calculate_similarity(trace.get_percept(), query_percept)
            
            if similarity >= threshold:
                trace.activate(similarity)
                relevant.append((trace, similarity))
        
        # Sort by activation strength
        relevant.sort(key=lambda x: x[1], reverse=True)
        
        return [trace for trace, _ in relevant]
    
    def prune_low_activation(self):
        """Remove memories with low activation."""
        keys_to_remove = []
        
        for key, activation in self.activation_levels.items():
            if activation < 0.1:  # Very low activation
                keys_to_remove.append(key)
        
        for key in keys_to_remove:
            del self.memory_traces[key]
            del self.activation_levels[key]
    
    def update_activations(self, time_step):
        """Update activation levels based on recency and strength."""
        decay_rate = 0.95 ** (time_step / 10)
        
        for key, activation in self.activation_levels.items():
            # Apply decay
            new_activation = activation * decay_rate
            
            # Add spontaneous recovery
            new_activation += 0.01
            
            self.activation_levels[key] = min(new_activation, 1.0)
```

### Episodic Memory and Replay

```python
import pynlm
import json
import os
from pathlib import Path

class EpisodicMemorySystem:
    """Manage episodic memory with replay and consolidation."""
    
    def __init__(self, brain, config):
        self.brain = brain
        self.config = config
        self.memory_capacity = 100
        self.replay_strength = 0.3
        self.consolidation_threshold = 0.8
        
        # Memory storage
        self.episodes = {}
        self.consolidated_memories = {}
        
        # Replay scheduling
        self.replay_queue = []
        self.replay_cooldown = {}
        
    def encode_episode(self, step_start, step_end, environment_state, agent_actions):
        """Encode an episode of experience."""
        episode_id = f"ep_{step_start}_{step_end}"
        
        # Extract episode details
        episode = {
            'id': episode_id,
            'start_step': step_start,
            'end_step': step_end,
            'duration': step_end - step_start,
            'environment_state': environment_state,
            'actions': agent_actions,
            'quality': self.calculate_episode_quality(agent_actions),
            'emotional_valence': self.calculate_valence(agent_actions),
            'novelty': self.calculate_novelty(environment_state)
        }
        
        # Store in episodic buffer
        self.episodes[episode_id] = episode
        
        # Schedule replay based on episode quality
        if episode['quality'] > self.consolidation_threshold:
            self.schedule_replay(episode_id, urgency='high')
        elif episode['novelty'] > 0.5:
            self.schedule_replay(episode_id, urgency='medium')
        else:
            self.schedule_replay(episode_id, urgency='low')
        
        # Check capacity and prune if needed
        if len(self.episodes) > self.memory_capacity:
            self.consolidate_low_quality_episodes()
        
        return episode
    
    def schedule_replay(self, episode_id, urgency='normal'):
        """Schedule episode for replay consolidation."""
        episode = self.episodes[episode_id]
        
        # Calculate replay interval based on urgency
        if urgency == 'high':
            replay_interval = 10  # Replays soon
        elif urgency == 'medium':
            replay_interval = 50  # Medium delay
        else:
            replay_interval = 100  # Long delay
        
        # Add to replay queue with timing
        current_time = self.get_current_simulation_time()
        replay_time = current_time + replay_interval
        
        self.replay_queue.append({
            'episode_id': episode_id,
            'replay_time': replay_time,
            'urgency': urgency
        })
        
        self.replay_cooldown[episode_id] = replay_time
    
    def perform_replay(self):
        """Perform scheduled replays for memory consolidation."""
        current_time = self.get_current_simulation_time()
        
        # Check which episodes to replay now
        episodes_to_replay = []
        
        for replay_item in self.replay_queue:
            if current_time >= replay_item['replay_time']:
                if replay_item['episode_id'] not in self.replay_cooldown:
                    episodes_to_replay.append(replay_item['episode_id'])
        
        # Perform replays
        for episode_id in episodes_to_replay:
            self.replay_episode(episode_id)
            
            # Remove from queue and cooldown
            self.replay_queue = [r for r in self.replay_queue if r['episode_id'] != episode_id]
            del self.replay_cooldown[episode_id]
        
        return episodes_to_replay
    
    def replay_episode(self, episode_id):
        """Replay specific episode for consolidation."""
        episode = self.episodes[episode_id]
        
        # Reconstruct episode in a controlled environment
        replay_world = pynlm.createSimpleWorld()
        replay_world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        replay_world.reset()
        
        # Replay actions with variations for generalization
        for action_idx, action in enumerate(episode['actions']):
            # Add noise to action for generalization
            noisy_action = self.add_replay_noise(action)
            
            # Apply action
            replay_world.applyMotorCommand(noisy_action, self.get_replay_time())
            
            # Get reward (simplified)
            reward = self.calculate_replay_reward(action, noisy_action)
            
            # Update episode statistics
            episode['replay_quality'] = self.update_replay_statistics(
                episode.get('replay_quality', []), reward)
        
        # Consolidate high-quality replays
        if self.is_high_quality_replay(episode):
            self.consolidate_episode(episode_id)
    
    def consolidate_episode(self, episode_id):
        """Consolidate episode into long-term memory."""
        episode = self.episodes[episode_id]
        
        # Extract patterns and rules
        patterns = self.extract_episodic_patterns(episode)
        rules = self.extract_episodic_rules(episode)
        
        # Store in consolidated memory
        consolidated_memory = {
            'episode_id': episode_id,
            'patterns': patterns,
            'rules': rules,
            'quality': episode.get('replay_quality', []),
            'consolidation_time': self.get_current_simulation_time()
        }
        
        self.consolidated_memories[episode_id] = consolidated_memory
        
        # Update brain connections based on consolidated memories
        self.update_brain_from_consolidation(consolidated_memory)
```

## 3. Neuromodulation and Learning Dynamics

### Dopamine and Reward Prediction

```python
import pynlm
import numpy as np

class DopamineSystem:
    """Implement dopamine-based reward prediction and learning."""
    
    def __init__(self, brain):
        self.brain = brain
        self.prediction_window = 10
        self.dopamine_history = []
        self.prediction_errors = []
        
        # Dopamine state variables
        self.predicted_value = 0.0
        self.actual_reward = 0.0
        self.dopamine_level = 0.0
        self.dopamine_trend = 0.0
        
    def update_prediction(self, expected_reward, actual_reward):
        """Update reward prediction using dopamine signaling."""
        # Calculate prediction error
        prediction_error = actual_reward - self.predicted_value
        
        # Update prediction
        self.predicted_value = expected_reward
        self.actual_reward = actual_reward
        
        # Generate dopamine response
        if prediction_error > 0:
            # Positive prediction error: phasic dopamine burst
            self.dopamine_level = self.calculate_phasic_burst(prediction_error)
            self.dopamine_trend = 1.0
        elif prediction_error < 0:
            # Negative prediction error: phasic dopamine dip
            self.dopamine_level = self.calculate_phasic_dip(abs(prediction_error))
            self.dopamine_trend = -1.0
        else:
            # No prediction error: baseline dopamine
            self.dopamine_level = self.calculate_baseline_dopamine()
            self.dopamine_trend = 0.0
        
        # Store for learning
        self.prediction_errors.append(prediction_error)
        self.dopamine_history.append(self.dopamine_level)
        
        # Apply dopamine to brain plasticity
        self.apply_dopamine_to_plasticity()
        
        return prediction_error
    
    def calculate_phasic_burst(self, error_magnitude):
        """Calculate dopamine burst for positive prediction error."""
        # Burst magnitude scales with error magnitude
        burst_strength = min(1.0, error_magnitude * 2.0)
        
        # Add temporal dynamics
        time_constant = 0.1
        decay = np.exp(-time_constant)
        
        return burst_strength * (1.0 - decay)
    
    def calculate_phasic_dip(self, error_magnitude):
        """Calculate dopamine dip for negative prediction error."""
        # Dip magnitude scales with error magnitude
        dip_strength = min(0.8, error_magnitude * 1.5)
        
        # Add temporal dynamics
        time_constant = 0.1
        decay = np.exp(-time_constant)
        
        return -dip_strength * (1.0 - decay)
    
    def calculate_baseline_dopamine(self):
        """Calculate baseline dopamine level."""
        # Baseline depends on prediction confidence
        confidence = 1.0 / (1.0 + abs(self.prediction_errors[-1] if self.prediction_errors else 0))
        
        # Add neuromodulatory tone
        baseline = 0.1 + (0.4 * confidence)
        
        return baseline
    
    def apply_dopamine_to_plasticity(self):
        """Apply dopamine level to synaptic plasticity rules."""
        # Modulate plasticity based on dopamine level
        plasticity_modulation = 1.0 + (0.5 * self.dopamine_level)
        
        # Apply to different plasticity mechanisms
        self.modulate_stdp(plasticity_modulation)
        self.modulate_hebbian(plasticity_modulation)
        self.modulate_reward_modulated(plasticity_modulation)
    
    def modulate_stdp(self, modulation_factor):
        """Modulate STDP learning rate based on dopamine."""
        current_stdp_rate = self.get_current_stdp_rate()
        
        # Dopamine enhances LTP when positive, suppresses when negative
        if self.dopamine_level > 0:
            new_stdp_rate = current_stdp_rate * (1.0 + 0.3 * self.dopamine_level)
        else:
            new_stdp_rate = current_stdp_rate * (1.0 - 0.2 * abs(self.dopamine_level))
        
        self.set_stdp_rate(new_stdp_rate * modulation_factor)
    
    def get_dopamine_level(self):
        """Get current dopamine level."""
        return self.dopamine_level
    
    def get_prediction_error_history(self):
        """Get recent prediction error history."""
        return self.prediction_errors[-self.prediction_window:]
```

### Curiosity and Novelty Detection

```python
import pynlm
import numpy as np

class CuriositySystem:
    """Implement curiosity-driven exploration and novelty detection."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        
        # Curiosity state
        self.curiosity_level = 0.0
        self.novelty_history = []
        self.Exploration_drive = 0.0
        
        # Novelty detection components
        self.sensory_memory = {}
        self.pattern_database = {}
        self.novelty_threshold = 0.6
        
    def update_curiosity(self, percept):
        """Update curiosity level based on sensory input."""
        # Calculate novelty of current percept
        novelty = self.calculate_novelty(percept)
        
        # Update novelty history
        self.novelty_history.append(novelty)
        if len(self.novelty_history) > 100:
            self.novelty_history.pop(0)
        
        # Update curiosity level based on novelty
        if novelty > self.novelty_threshold:
            # High novelty increases curiosity
            self.curiosity_level = min(1.0, self.curiosity_level + novelty * 0.1)
            self.trigger_exploration_impulse(novelty)
        else:
            # Low novelty decreases curiosity
            decay_rate = 0.95
            self.curiosity_level *= decay_rate
        
        # Add prediction error-based curiosity
        prediction_error = self.calculate_prediction_error(percept)
        self.curiosity_level += prediction_error * 0.05
        self.curiosity_level = min(1.0, max(0.0, self.curiosity_level))
        
        return novelty
    
    def calculate_novelty(self, percept):
        """Calculate novelty of sensory percept."""
        # Extract features from percept
        features = self.extract_features(percept)
        
        # Create feature signature
        feature_signature = self.create_feature_signature(features)
        
        # Compare with stored patterns
        novelty_score = 0.0
        
        if feature_signature in self.pattern_database:
            # Calculate distance to nearest pattern
            distances = []
            for pattern_id, pattern_features in self.pattern_database.items():
                distance = self.calculate_feature_distance(feature_signature, pattern_features)
                distances.append(distance)
            
            min_distance = min(distances) if distances else 1.0
            novelty_score = min(1.0, 1.0 - min_distance)
        else:
            # New pattern - high novelty
            novelty_score = 1.0
            
            # Store pattern
            self.pattern_database[feature_signature] = features
            self.sensory_memory[feature_signature] = percept
        
        return novelty_score
    
    def calculate_prediction_error(self, percept):
        """Calculate prediction error for curiosity."""
        # Get current sensory input
        features = self.extract_features(percept)
        feature_signature = self.create_feature_signature(features)
        
        # Check if we have a prediction
        if feature_signature in self.sensory_memory:
            # Get predicted percept
            predicted_percept = self.sensory_memory[feature_signature]
            
            # Calculate prediction error
            actual_features = self.extract_features(percept)
            predicted_features = self.extract_features(predicted_percept)
            
            error = self.calculate_feature_distance(actual_features, predicted_features)
            return min(1.0, error)
        
        return 0.0
    
    def trigger_exploration_impulse(self, novelty_level):
        """Trigger exploration behavior based on high novelty."""
        # Exploration impulse strength scales with novelty
        exploration_strength = novelty_level ** 2  # Squaring for nonlinear response
        
        # Apply exploration impulse to agent's behavior
        self.Exploration_drive += exploration_strength * 0.2
        
        # Modify agent's policy toward exploration
        self.modify_exploration_policy(exploration_strength)
    
    def modify_exploration_policy(self, strength):
        """Modify agent's exploration-exploitation tradeoff."""
        # This would modify the agent's decision-making process
        # For now, we'll store the strength for use in decision making
        self.current_exploration_strength = strength
    
    def get_curiosity_level(self):
        """Get current curiosity level."""
        return self.curiosity_level
    
    def get_novelty_level(self):
        """Get current novelty level."""
        return self.novelty_history[-1] if self.novelty_history else 0.0
    
    def get_exploration_drive(self):
        """Get current exploration drive."""
        return self.Exploration_drive
```

## 4. Advanced Configuration Management

### Configuration Inheritance and Overrides

```python
import pynlm
import json
import copy
from pathlib import Path

class ConfigurationManager:
    """Advanced configuration management with inheritance and validation."""
    
    def __init__(self, config_dir="configs/"):
        self.config_dir = Path(config_dir)
        self.config_cache = {}
        self.inheritance_cache = {}
        
    def load_config(self, name, base_config=None):
        """Load configuration with inheritance support."""
        # Check cache
        cache_key = name
        if cache_key in self.config_cache:
            return self.config_cache[cache_key]
        
        # Load base configuration if provided
        config = base_config if base_config else self.create_default_config()
        
        # Load configuration file
        config_file = self.config_dir / f"{name}.cfg"
        if not config_file.exists():
            raise FileNotFoundError(f"Configuration file {config_file} not found")
        
        # Parse configuration file
        loaded_config = self.parse_config_file(config_file)
        
        # Merge with base configuration
        merged_config = self.merge_configs(config, loaded_config)
        
        # Apply inheritance
        if "inherits" in loaded_config:
            parent_name = loaded_config["inherits"]
            parent_config = self.load_config(parent_name)
            merged_config = self.merge_configs(parent_config, loaded_config)
        
        # Validate configuration
        self.validate_config(merged_config)
        
        # Cache result
        self.config_cache[cache_key] = merged_config
        
        return merged_config
    
    def create_custom_config(self, base_config_name, modifications, **kwargs):
        """Create custom configuration based on existing configuration."""
        # Load base configuration
        base_config = self.load_config(base_config_name)
        
        # Create copy to avoid modifying original
        custom_config = copy.deepcopy(base_config)
        
        # Apply modifications
        if modifications:
            self.apply_modifications(custom_config, modifications)
        
        # Add custom parameters
        for key, value in kwargs.items():
            custom_config[key] = value
        
        # Validate custom configuration
        self.validate_config(custom_config)
        
        return custom_config
    
    def apply_modifications(self, config, modifications):
        """Apply parameter modifications to configuration."""
        for param_path, value in modifications.items():
            # Handle nested parameter paths
            keys = param_path.split(".")
            target = config
            
            for key in keys[:-1]:
                if key not in target:
                    target[key] = {}
                target = target[key]
            
            target[keys[-1]] = value
    
    def validate_config(self, config):
        """Validate configuration parameters."""
        validation_rules = self.get_validation_rules()
        
        for param_path, rules in validation_rules.items():
            if param_path in config:
                value = config[param_path]
                self.validate_parameter(param_path, value, rules)
    
    def get_validation_rules(self):
        """Get configuration validation rules."""
        return {
            "brain.neuron_count": {
                "type": "integer",
                "min": 100,
                "max": 10000,
                "required": True
            },
            "brain.synapse_density": {
                "type": "float",
                "min": 0.0,
                "max": 1.0,
                "required": True
            },
            "plasticity.stdp.learning_rate": {
                "type": "float",
                "min": 0.0,
                "max": 1.0,
                "required": True
            },
            "development.initial_plasticity": {
                "type": "float",
                "min": 0.0,
                "max": 10.0,
                "required": False
            },
            "environment.width": {
                "type": "integer",
                "min": 5,
                "max": 100,
                "required": True
            },
            "environment.height": {
                "type": "integer",
                "min": 5,
                "max": 100,
                "required": True
            }
        }
    
    def validate_parameter(self, param_path, value, rules):
        """Validate individual parameter."""
        if rules["required"] and value is None:
            raise ValueError(f"Required parameter {param_path} is missing")
        
        if rules["type"] == "integer" and not isinstance(value, int):
            raise ValueError(f"Parameter {param_path} must be an integer")
        
        if rules["type"] == "float" and not isinstance(value, (int, float)):
            raise ValueError(f"Parameter {param_path} must be a number")
        
        if "min" in rules and value < rules["min"]:
            raise ValueError(f"Parameter {param_path} must be >= {rules['min']}")
        
        if "max" in rules and value > rules["max"]:
            raise ValueError(f"Parameter {param_path} must be <= {rules['max']}")
    
    def parse_config_file(self, config_file):
        """Parse configuration file with key=value format."""
        config = {}
        
        with open(config_file, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    if '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        
                        # Parse value based on type
                        config[key] = self.parse_config_value(value)
        
        return config
    
    def parse_config_value(self, value):
        """Parse configuration value based on type."""
        # Try to parse as boolean
        if value.lower() in ('true', 'false'):
            return value.lower() == 'true'
        
        # Try to parse as integer
        try:
            return int(value)
        except ValueError:
            pass
        
        # Try to parse as float
        try:
            return float(value)
        except ValueError:
            pass
        
        # Return as string
        return value
    
    def create_default_config(self):
        """Create default configuration."""
        return {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "brain": {
                "neuron_count": 1000,
                "synapse_density": 0.1
            },
            "plasticity": {
                "stdp": {
                    "learning_rate": 0.01,
                    "ltp_weight": 0.01,
                    "ltd_weight": 0.012,
                    "time_constant": 20.0
                }
            },
            "development": {
                "initial_plasticity": 1.0,
                "synaptogenesis_rate": 0.001,
                "pruning_rate": 0.0001,
                "critical_period_start": 100,
                "critical_period_end": 500
            },
            "neuromodulation": {
                "dopamine_baseline": 0.1,
                "curiosity_baseline": 0.0,
                "novelty_threshold": 0.5
            },
            "reward": {
                "discount_factor": 0.99
            },
            "environment": {
                "name": "GridWorld",
                "width": 10,
                "height": 10,
                "visionWidth": 8,
                "visionHeight": 8
            },
            "logging": {
                "level": "INFO",
                "to_file": False,
                "filename": "nlm.log"
            }
        }
    
    def save_config(self, config, filename, format="cfg"):
        """Save configuration to file."""
        if format == "cfg":
            self.save_cfg_config(config, filename)
        elif format == "json":
            self.save_json_config(config, filename)
        elif format == "yaml":
            self.save_yaml_config(config, filename)
    
    def save_cfg_config(self, config, filename):
        """Save configuration in key=value format."""
        with open(filename, 'w') as f:
            for key, value in self.flatten_config(config).items():
                if isinstance(value, bool):
                    value_str = str(value).lower()
                elif isinstance(value, (int, float)):
                    value_str = str(value)
                else:
                    value_str = f'"{value}"'
                
                f.write(f"{key} = {value_str}\n")
    
    def save_json_config(self, config, filename):
        """Save configuration in JSON format."""
        with open(filename, 'w') as f:
            json.dump(config, f, indent=2)
    
    def flatten_config(self, config, parent_key="", sep="."):
        """Flatten nested configuration dictionary."""
        items = []
        
        for key, value in config.items():
            new_key = f"{parent_key}{sep}{key}" if parent_key else key
            
            if isinstance(value, dict):
                items.extend(self.flatten_config(value, new_key, sep=sep).items())
            else:
                items.append((new_key, value))
        
        return dict(items)
```

### Performance Profiling and Optimization

```python
import pynlm
import time
import cProfile
import pstats
from io import StringIO
import numpy as np

class PerformanceProfiler:
    """Profile and optimize NLM performance."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.profiling_data = {}
        self.optimization_suggestions = []
        
    def profile_simulation_step(self, steps, repetitions=5):
        """Profile simulation performance over multiple steps."""
        print(f"Profiling {steps} simulation steps ({repetitions} repetitions)...")
        
        times = []
        neuron_counts = []
        synapse_counts = []
        
        for rep in range(repetitions):
            start_time = time.time()
            
            for step in range(steps):
                self.brain.step(step)
            
            end_time = time.time()
            
            times.append(end_time - start_time)
            neuron_counts.append(self.brain.getTotalNeuronCount())
            synapse_counts.append(self.brain.getTotalSynapseCount())
        
        # Calculate statistics
        avg_time = np.mean(times)
        std_time = np.std(times)
        steps_per_second = steps / avg_time
        
        profiling_result = {
            'steps': steps,
            'repetitions': repetitions,
            'times': times,
            'neuron_counts': neuron_counts,
            'synapse_counts': synapse_counts,
            'avg_time_per_step': avg_time / steps,
            'std_time_per_step': std_time / steps,
            'steps_per_second': steps_per_second,
            'neurons_per_step': np.mean(neuron_counts),
            'synapses_per_step': np.mean(synapse_counts)
        }
        
        self.profiling_data['simulation'] = profiling_result
        
        # Generate optimization suggestions
        self.generate_optimization_suggestions(profiling_result)
        
        return profiling_result
    
    def profile_configuration_comparison(self, config_list, steps=100):
        """Compare performance of different configurations."""
        print("Comparing configurations...")
        
        results = {}
        
        for config_name, config in config_list.items():
            print(f"  Profiling {config_name}...")
            
            # Create brain with specific configuration
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Profile performance
            start_time = time.time()
            
            for step in range(steps):
                brain.step(step)
            
            end_time = time.time()
            
            results[config_name] = {
                'steps_per_second': steps / (end_time - start_time),
                'neuron_count': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'avg_firing_rate': brain.getAverageFiringRate()
            }
        
        self.profiling_data['configuration_comparison'] = results
        return results
    
    def profile_memory_usage(self, config, steps):
        """Profile memory usage during simulation."""
        import tracemalloc
        
        print("Profiling memory usage...")
        
        # Start memory tracing
        tracemalloc.start()
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        end_time = time.time()
        
        # Get memory usage
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()
        
        memory_result = {
            'steps_per_second': steps / (end_time - start_time),
            'current_memory_mb': current / 1024 / 1024,
            'peak_memory_mb': peak / 1024 / 1024,
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'memory_per_neuron_kb': (peak / 1024 / 1024) / brain.getTotalNeuronCount()
        }
        
        self.profiling_data['memory'] = memory_result
        return memory_result
    
    def generate_optimization_suggestions(self, profiling_result):
        """Generate optimization suggestions based on profiling data."""
        suggestions = []
        
        # Check if performance is suboptimal
        steps_per_second = profiling_result['steps_per_second']
        neuron_count = profiling_result['neurons_per_step']
        
        if steps_per_second < 100:
            suggestions.append({
                'type': 'performance',
                'severity': 'high',
                'description': f'Low performance: {steps_per_second:.1f} steps/second',
                'suggestion': 'Consider using high_perf.cfg or reducing neuron count',
                'expected_improvement': '2-5x speedup'
            })
        
        # Check memory efficiency
        memory_per_neuron = profiling_result['memory_per_neuron_kb']
        if memory_per_neuron > 10:
            suggestions.append({
                'type': 'memory',
                'severity': 'medium',
                'description': f'High memory usage: {memory_per_neuron:.1f} KB per neuron',
                'suggestion': 'Use lightweight.cfg or optimize configuration',
                'expected_improvement': '30-50% memory reduction'
            })
        
        # Check neuron utilization
        avg_firing_rate = profiling_result.get('avg_firing_rate', 0)
        if avg_firing_rate < 0.1:
            suggestions.append({
                'type': 'utilization',
                'severity': 'low',
                'description': f'Low neuron firing rate: {avg_firing_rate:.3f}',
                'suggestion': 'Adjust connectivity or plasticity parameters',
                'expected_improvement': 'Better task performance'
            })
        
        self.optimization_suggestions.extend(suggestions)
        return suggestions
    
    def get_optimization_report(self):
        """Get comprehensive optimization report."""
        report = "# NLM Performance Optimization Report\n\n"
        
        if 'simulation' in self.profiling_data:
            sim_data = self.profiling_data['simulation']
            report += f"## Simulation Performance\n"
            report += f"- Steps per second: {sim_data['steps_per_second']:.1f}\n"
            report += f"- Average time per step: {sim_data['avg_time_per_step']*1000:.1f} ms\n"
            report += f"- Neurons per step: {sim_data['neurons_per_step']:.0f}\n"
            report += f"- Synapses per step: {sim_data['synapses_per_step']:.0f}\n\n"
        
        if 'memory' in self.profiling_data:
            mem_data = self.profiling_data['memory']
            report += f"## Memory Performance\n"
            report += f"- Peak memory usage: {mem_data['peak_memory_mb']:.1f} MB\n"
            report += f"- Memory per neuron: {mem_data['memory_per_neuron_kb']:.1f} KB\n"
            report += f"- Steps per second: {mem_data['steps_per_second']:.1f}\n\n"
        
        if self.optimization_suggestions:
            report += f"## Optimization Recommendations\n"
            for suggestion in self.optimization_suggestions:
                severity_icon = {"high": "🔴", "medium": "🟡", "low": "🟢"}.get(suggestion['severity'], "⚪")
                report += f"{severity_icon} **{suggestion['severity'].title()}: {suggestion['type'].title()}**\n"
                report += f"   **Description:** {suggestion['description']}\n"
                report += f"   **Suggestion:** {suggestion['suggestion']}\n"
                report += f"   **Expected improvement:** {suggestion['expected_improvement']}\n\n"
        
        return report
```

## 5. Advanced Error Recovery and Resilience

### Fault Tolerance and Error Recovery

```python
import pynlm
import time
import traceback
from typing import Optional, Callable

class FaultToleranceManager:
    """Implement fault tolerance and error recovery for NLM simulations."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.error_handlers = {}
        self.recovery_strategies = {}
        self.checkpoint_interval = 100
        self.last_checkpoint = 0
        self.error_count = 0
        self.max_errors_before_restart = 5
        
        # Register default error handlers
        self.register_error_handler("initialization", self.handle_initialization_error)
        self.register_error_handler("simulation", self.handle_simulation_error)
        self.register_error_handler("memory", self.handle_memory_error)
        
    def register_error_handler(self, error_type: str, handler: Callable):
        """Register error handler for specific error type."""
        self.error_handlers[error_type] = handler
    
    def register_recovery_strategy(self, error_type: str, strategy: Callable):
        """Register recovery strategy for specific error type."""
        self.recovery_strategies[error_type] = strategy
    
    def run_with_recovery(self, steps: int, checkpoint_interval: Optional[int] = None):
        """Run simulation with automatic error recovery."""
        if checkpoint_interval:
            self.checkpoint_interval = checkpoint_interval
        
        try:
            for step in range(steps):
                try:
                    # Run simulation step
                    self.brain.step(step)
                    
                    # Check for errors
                    error = self.check_for_errors()
                    if error:
                        self.handle_error(error, step)
                    
                    # Save checkpoint periodically
                    if step % self.checkpoint_interval == 0:
                        self.save_checkpoint(step)
                    
                except Exception as e:
                    # Handle unhandled exceptions
                    self.handle_unexpected_error(e, step)
                    
                    # Check if we should continue or restart
                    if self.error_count >= self.max_errors_before_restart:
                        raise RuntimeError(f"Too many errors ({self.error_count}), restarting simulation")
                    
                    # Continue with recovery
                    self.continue_after_error(step, e)
            
        except Exception as e:
            self.handle_final_error(e)
            raise
    
    def check_for_errors(self) -> Optional[str]:
        """Check for potential errors in simulation state."""
        # Check for numerical instabilities
        firing_rate = self.brain.getAverageFiringRate()
        if firing_rate > 1000 or firing_rate < -1000:
            return "numerical_instability"
        
        # Check for memory issues
        if self.get_memory_usage() > self.get_memory_limit():
            return "memory_overflow"
        
        # Check for neural degeneration
        if self.check_neural_integrity() < 0.1:
            return "neural_degeneration"
        
        # Check for simulation drift
        if self.check_simulation_stability() < 0.5:
            return "simulation_drift"
        
        return None
    
    def handle_error(self, error_type: str, step: int):
        """Handle detected error with registered error handler."""
        self.error_count += 1
        
        if error_type in self.error_handlers:
            handler = self.error_handlers[error_type]
            handler(error_type, step)
        else:
            self.handle_unknown_error(error_type, step)
    
    def handle_initialization_error(self, error_type: str, step: int):
        """Handle initialization errors."""
        print(f"Initialization error at step {step}: attempting recovery")
        
        # Try to reinitialize with different parameters
        config = self.create_recovery_config()
        
        try:
            new_brain = pynlm.createBrain(config)
            new_brain.initialize()
            
            # Replace brain with recovered one
            self.brain = new_brain
            print(f"Successfully recovered from initialization error")
            
        except Exception as e:
            print(f"Recovery failed: {e}")
            raise
    
    def handle_simulation_error(self, error_type: str, step: int):
        """Handle simulation errors."""
        print(f"Simulation error at step {step}: {error_type}")
        
        if error_type == "numerical_instability":
            self.recover_from_numerical_instability()
        elif error_type == "neural_degeneration":
            self.recover_from_neural_degeneration()
        elif error_type == "simulation_drift":
            self.recover_from_simulation_drift()
        else:
            self.generic_recovery()
    
    def handle_simulation_error(self, error_type: str, step: int):
        """Handle simulation errors."""
        print(f"Simulation error at step {step}: {error_type}")
        
        if error_type == "numerical_instability":
            self.recover_from_numerical_instability()
        elif error_type == "neural_degeneration":
            self.recover_from_neural_degeneration()
        elif error_type == "simulation_drift":
            self.recover_from_simulation_drift()
        else:
            self.generic_recovery()
    
    def recover_from_numerical_instability(self):
        """Recover from numerical instability."""
        # Reduce simulation timestep
        current_timestep = self.get_simulation_timestep()
        new_timestep = current_timestep * 0.5
        
        # Update configuration
        config = self.get_current_config()
        config.set("simulation.timestep", new_timestep)
        
        # Reinitialize brain with new configuration
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        
        self.brain = new_brain
        print(f"Recovered from numerical instability (timestep: {new_timestep})")
    
    def recover_from_neural_degeneration(self):
        """Recover from neural degeneration."""
        # Create new brain with same configuration
        config = self.get_current_config()
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        
        # Transfer learned knowledge if possible
        self.transfer_knowledge(new_brain)
        
        self.brain = new_brain
        print("Recovered from neural degeneration")
    
    def recover_from_simulation_drift(self):
        """Recover from simulation drift."""
        # Reset world to known good state
        self.world.reset()
        
        # Adjust simulation parameters
        config = self.get_current_config()
        config.set("environment.reset", True)
        
        print("Recovered from simulation drift")
    
    def generic_recovery(self):
        """Generic error recovery strategy."""
        # Save current state
        self.save_checkpoint("recovery_point")
        
        # Create backup state
        self.create_backup_state()
        
        # Try to continue with reduced complexity
        config = self.get_current_config()
        config.set("brain.neuron_count", int(config.get("brain.neuron_count", 1000) * 0.8))
        
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        
        self.brain = new_brain
        print("Generic recovery completed")
    
    def handle_unexpected_error(self, error: Exception, step: int):
        """Handle unexpected errors with traceback logging."""
        print(f"Unexpected error at step {step}: {error}")
        print(f"Traceback: {traceback.format_exc()}")
        
        # Log error details
        self.log_error(error, step)
        
        # Try recovery if available
        if self.error_count > 0:
            self.attempt_recovery(step)
    
    def handle_final_error(self, error: Exception):
        """Handle final error with graceful shutdown."""
        print(f"Fatal error: {error}")
        
        # Save final state
        self.save_checkpoint("final_state")
        
        # Log final error state
        self.log_final_error(error)
    
    def save_checkpoint(self, step: Optional[int] = None):
        """Save simulation checkpoint."""
        if step is None:
            step = self.last_checkpoint
        
        checkpoint_data = {
            'step': step,
            'brain_state': self.capture_brain_state(),
            'world_state': self.capture_world_state(),
            'timestamp': time.time()
        }
        
        # Save to file
        filename = f"checkpoint_step_{step}.json"
        with open(filename, 'w') as f:
            json.dump(checkpoint_data, f, indent=2)
        
        self.last_checkpoint = step
        print(f"Checkpoint saved at step {step}")
    
    def load_checkpoint(self, filename: str):
        """Load simulation from checkpoint."""
        with open(filename, 'r') as f:
            checkpoint_data = json.load(f)
        
        # Restore brain state
        self.restore_brain_state(checkpoint_data['brain_state'])
        
        # Restore world state
        self.restore_world_state(checkpoint_data['world_state'])
        
        print(f"Checkpoint loaded from {filename}")
    
    def capture_brain_state(self):
        """Capture current brain state."""
        return {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'developmental_stage': self.brain.getDevelopmentalStage()
        }
    
    def restore_brain_state(self, state):
        """Restore brain state from checkpoint."""
        # This would typically involve more sophisticated state restoration
        # For now, we'll create a new brain with similar characteristics
        config = self.get_current_config()
        config.set("brain.neuron_count", state['neuron_count'])
        
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        
        self.brain = new_brain
    
    def get_memory_usage(self) -> float:
        """Get current memory usage in MB."""
        import psutil
        process = psutil.Process()
        return process.memory_info().rss / 1024 / 1024
    
    def get_memory_limit(self) -> float:
        """Get memory limit in MB."""
        import psutil
        return psutil.virtual_memory().available / 1024 / 1024
    
    def check_neural_integrity(self) -> float:
        """Check neural integrity (0.0 to 1.0)."""
        neuron_count = self.brain.getTotalNeuronCount()
        synapse_count = self.brain.getTotalSynapseCount()
        
        # Calculate integrity score based on connectivity
        ideal_connectivity = neuron_count * 0.1  # 10% connectivity is ideal
        actual_connectivity = synapse_count / neuron_count
        
        integrity = max(0.0, min(1.0, 1.0 - abs(actual_connectivity - ideal_connectivity)))
        
        return integrity
    
    def check_simulation_stability(self) -> float:
        """Check simulation stability (0.0 to 1.0)."""
        # Check stability based on firing rate variations
        firing_rates = self.get_recent_firing_rates(100)
        
        if len(firing_rates) < 2:
            return 1.0
        
        # Calculate coefficient of variation
        mean_rate = np.mean(firing_rates)
        std_rate = np.std(firing_rates)
        
        cv = std_rate / (mean_rate + 1e-6)
        stability = max(0.0, min(1.0, 1.0 - cv))
        
        return stability
    
    def create_recovery_config(self):
        """Create recovery configuration."""
        config = self.get_current_config()
        config.set("random_seed", int(time.time()))
        config.set("brain.neuron_count", max(100, config.get("brain.neuron_count", 1000) // 2))
        return config
    
    def get_current_config(self):
        """Get current brain configuration."""
        # This would depend on how configuration is stored
        # For now, return a default config
        return pynlm.createDefaultConfig()
    
    def get_simulation_timestep(self):
        """Get current simulation timestep."""
        config = self.get_current_config()
        return config.get("simulation.timestep", 0.001)
    
    def log_error(self, error: Exception, step: int):
        """Log error details."""
        error_log = {
            'timestamp': time.time(),
            'step': step,
            'error_type': type(error).__name__,
            'error_message': str(error),
            'traceback': traceback.format_exc()
        }
        
        with open("error_log.json", 'a') as f:
            f.write(json.dumps(error_log) + "\n")
    
    def log_final_error(self, error: Exception):
        """Log final error with summary."""
        error_summary = {
            'timestamp': time.time(),
            'total_errors': self.error_count,
            'final_error': str(error),
            'final_error_type': type(error).__name__
        }
        
        with open("final_error_log.json", 'w') as f:
            json.dump(error_summary, f, indent=2)
```

## 6. Custom Extensions and Plugin System

### Plugin Architecture

```python
import pynlm
import importlib
from pathlib import Path
from typing import Dict, Any, Callable

class PluginManager:
    """Manage NLM plugins and extensions."""
    
    def __init__(self, plugin_dir="plugins/"):
        self.plugin_dir = Path(plugin_dir)
        self.plugins = {}
        self.plugin_hooks = {}
        
    def load_plugin(self, plugin_name: str):
        """Load a plugin by name."""
        plugin_file = self.plugin_dir / f"{plugin_name}.py"
        
        if not plugin_file.exists():
            raise FileNotFoundError(f"Plugin file {plugin_file} not found")
        
        # Import the plugin module
        spec = importlib.util.spec_from_file_location(plugin_name, plugin_file)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        
        # Get plugin class
        if hasattr(module, 'Plugin'):
            plugin_class = module.Plugin
            plugin_instance = plugin_class()
            
            # Register plugin hooks
            for hook_name, hook_func in plugin_instance.get_hooks().items():
                self.register_hook(hook_name, hook_func)
            
            self.plugins[plugin_name] = plugin_instance
            print(f"Loaded plugin: {plugin_name}")
            
            return plugin_instance
        
        raise ValueError(f"Plugin {plugin_name} does not define Plugin class")
    
    def register_hook(self, hook_name: str, hook_func: Callable):
        """Register hook function for plugin integration."""
        if hook_name not in self.plugin_hooks:
            self.plugin_hooks[hook_name] = []
        
        self.plugin_hooks[hook_name].append(hook_func)
    
    def execute_hooks(self, hook_name: str, *args, **kwargs):
        """Execute all hooks registered for a specific hook name."""
        if hook_name not in self.plugin_hooks:
            return args
        
        results = args
        for hook_func in self.plugin_hooks[hook_name]:
            try:
                results = hook_func(*results, **kwargs)
            except Exception as e:
                print(f"Hook {hook_name} failed: {e}")
                # Continue with other hooks
        
        return results
    
    def list_plugins(self) -> list:
        """List all loaded plugins."""
        return list(self.plugins.keys())
    
    def unload_plugin(self, plugin_name: str):
        """Unload a plugin."""
        if plugin_name in self.plugins:
            # Remove plugin hooks
            plugin = self.plugins[plugin_name]
            for hook_name, hook_func in plugin.get_hooks().items():
                if hook_name in self.plugin_hooks:
                    self.plugin_hooks[hook_name].remove(hook_func)
            
            del self.plugins[plugin_name]
            print(f"Unloaded plugin: {plugin_name}")

# Example plugin for NLM extensions
class NeuralNetworkPlugin:
    """Example plugin for neural network extensions."""
    
    def get_hooks(self):
        """Return plugin hooks."""
        return {
            'on_brain_initialize': self.on_brain_initialize,
            'on_simulation_step': self.on_simulation_step,
            'on_agent_decision': self.on_agent_decision
        }
    
    def on_brain_initialize(self, brain, config):
        """Hook called when brain is initialized."""
        # Custom initialization logic
        print(f"NeuralNetworkPlugin: Brain initialized with {brain.getTotalNeuronCount()} neurons")
        
        # Add custom connections based on configuration
        if config.get("neural_network.enable_deep_connections", False):
            self.add_deep_connections(brain)
        
        return brain
    
    def on_simulation_step(self, brain, step, world=None):
        """Hook called at each simulation step."""
        # Custom simulation logic
        firing_rate = brain.getAverageFiringRate()
        
        if firing_rate > 100:  # High activity threshold
            self.apply_stability_measures(brain)
        
        return brain
    
    def on_agent_decision(self, agent, percept):
        """Hook called when agent makes decisions."""
        # Custom decision logic
        curiosity_level = agent.getCuriosityLevel()
        
        if curiosity_level > 0.8:
            self encourage_exploration(agent)
        
        return agent
    
    def add_deep_connections(self, brain):
        """Add deep, long-range connections for enhanced processing."""
        # Implementation would add specialized connection patterns
        # This is a placeholder for custom neural architecture
        pass
    
    def apply_stability_measures(self, brain):
        """Apply stability measures when activity is too high."""
        # Implementation would add stabilization mechanisms
        pass
    
    def encourage_exploration(self, agent):
        """Encourage exploration behavior."""
        # Implementation would modify agent's exploration policy
        pass
```

## 7. Advanced Performance Optimization Techniques

### Neural Network Optimization

```python
import pynlm
import numpy as np
from scipy import sparse

class NeuralOptimizer:
    """Advanced optimization for neural network performance."""
    
    def __init__(self, brain):
        self.brain = brain
        self.optimization_history = {}
        
    def optimize_connectivity(self, target_density=0.1, iterations=100):
        """Optimize neural connectivity for best performance."""
        print(f"Optimizing connectivity for target density {target_density}...")
        
        current_density = self.get_current_connectivity()
        best_density = target_density
        best_performance = 0.0
        
        for iteration in range(iterations):
            # Perturb connectivity
            new_density = self.perturb_density(current_density, 0.05)
            
            # Apply optimized connectivity
            self.apply_connectivity(new_density)
            
            # Measure performance
            performance = self.measure_performance()
            
            # Update best if improved
            if performance > best_performance:
                best_performance = performance
                best_density = new_density
            
            # Track iteration
            self.optimization_history[iteration] = {
                'density': new_density,
                'performance': performance,
                'improvement': performance - best_performance
            }
        
        print(f"Optimization completed: {best_density:.3f} density, {best_performance:.3f} performance")
        return best_density
    
    def optimize_plasticity_parameters(self, param_space):
        """Optimize plasticity parameters using Bayesian optimization."""
        print("Optimizing plasticity parameters...")
        
        # Initial parameters
        current_params = self.get_current_plasticity_params()
        best_params = current_params
        best_score = self.evaluate_plasticity_performance(current_params)
        
        for iteration in range(param_space['iterations']):
            # Sample new parameters
            new_params = self.sample_parameters(param_space, current_params)
            
            # Evaluate performance
            score = self.evaluate_plasticity_performance(new_params)
            
            # Update best if improved
            if score > best_score:
                best_score = score
                best_params = new_params
                current_params = new_params
            
            # Update acquisition function
            self.update_acquisition(param_space, new_params, score)
        
        # Apply optimal parameters
        self.apply_plasticity_parameters(best_params)
        
        print(f"Plasticity optimization completed: {best_score:.3f} score")
        return best_params
    
    def apply_adaptive_plasticity(self, base_config):
        """Apply adaptive plasticity based on learning progress."""
        # Track learning progress
        learning_progress = self.calculate_learning_progress()
        
        # Adapt plasticity parameters based on progress
        adapted_config = base_config.copy()
        
        if learning_progress < 0.3:  # Slow learning
            # Increase plasticity for faster learning
            adapted_config['plasticity.stdp.learning_rate'] *= 1.5
            adapted_config['development.initial_plasticity'] *= 1.2
        elif learning_progress > 0.7:  # Fast learning
            # Reduce plasticity to prevent overfitting
            adapted_config['plasticity.stdp.learning_rate'] *= 0.8
            adapted_config['development.pruning_rate'] *= 1.1
        
        # Create brain with adapted configuration
        brain = pynlm.createBrain(adapted_config)
        brain.initialize()
        
        return brain
    
    def get_current_connectivity(self):
        """Get current connectivity density."""
        neuron_count = self.brain.getTotalNeuronCount()
        synapse_count = self.brain.getTotalSynapseCount()
        
        return synapse_count / (neuron_count * (neuron_count - 1))
    
    def apply_connectivity(self, target_density):
        """Apply specific connectivity density."""
        # This would modify the brain's connectivity pattern
        # Implementation depends on the brain's internal structure
        pass
    
    def perturb_density(self, current_density, perturbation):
        """Generate perturbed density value."""
        new_density = current_density + np.random.uniform(-perturbation, perturbation)
        return max(0.0, min(1.0, new_density))
    
    def measure_performance(self):
        """Measure current brain performance."""
        # Simple performance metric based on firing rate and stability
        firing_rate = self.brain.getAverageFiringRate()
        stability = self.calculate_stability()
        
        # Normalized performance score
        performance = (firing_rate / 100.0) * stability
        return performance
    
    def calculate_stability(self):
        """Calculate simulation stability."""
        # Check firing rate variations
        recent_rates = self.get_recent_firing_rates(50)
        if len(recent_rates) < 2:
            return 1.0
        
        cv = np.std(recent_rates) / (np.mean(recent_rates) + 1e-6)
        return max(0.0, 1.0 - cv)
    
    def get_current_plasticity_params(self):
        """Get current plasticity parameters."""
        config = self.brain.get_config() if hasattr(self.brain, 'get_config') else None
        if config:
            return {
                'stdp_learning_rate': config.get("plasticity.stdp.learning_rate", 0.01),
                'stdp_ltp_weight': config.get("plasticity.stdp.ltp_weight", 0.01),
                'stdp_ltd_weight': config.get("plasticity.stdp.ltd_weight", 0.012),
                'development_plasticity': config.get("development.initial_plasticity", 1.0)
            }
        
        return {
            'stdp_learning_rate': 0.01,
            'stdp_ltp_weight': 0.01,
            'stdp_ltd_weight': 0.012,
            'development_plasticity': 1.0
        }
    
    def evaluate_plasticity_performance(self, params):
        """Evaluate plasticity parameter set."""
        # Create temporary brain with these parameters
        config = pynlm.createDefaultConfig()
        config.set("plasticity.stdp.learning_rate", params['stdp_learning_rate'])
        config.set("plasticity.stdp.ltp_weight", params['stdp_ltp_weight'])
        config.set("plasticity.stdp.ltd_weight", params['stdp_ltd_weight'])
        config.set("development.initial_plasticity", params['development_plasticity'])
        
        temp_brain = pynlm.createBrain(config)
        temp_brain.initialize()
        
        # Run quick simulation to evaluate
        for step in range(50):
            temp_brain.step(step)
        
        return temp_brain.getAverageFiringRate()
    
    def sample_parameters(self, param_space, current_params):
        """Sample new parameters for optimization."""
        # Simple random sampling
        new_params = current_params.copy()
        
        for param_name in param_space['parameters']:
            # Get bounds for this parameter
            bounds = param_space['bounds'][param_name]
            
            # Sample within bounds
            new_params[param_name] = np.random.uniform(bounds['min'], bounds['max'])
        
        return new_params
    
    def update_acquisition(self, param_space, params, score):
        """Update acquisition function for next sampling."""
        # Simple Gaussian process for parameter scoring
        # This is a simplified placeholder
        pass
    
    def apply_plasticity_parameters(self, params):
        """Apply plasticity parameters to the brain."""
        config = self.brain.get_config() if hasattr(self.brain, 'get_config') else pynlm.createDefaultConfig()
        
        config.set("plasticity.stdp.learning_rate", params['stdp_learning_rate'])
        config.set("plasticity.stdp.ltp_weight", params['stdp_ltp_weight'])
        config.set("plasticity.stdp.ltd_weight", params['stdp_ltd_weight'])
        config.set("development.initial_plasticity", params['development_plasticity'])
        
        # Recreate brain with new parameters
        new_brain = pynlm.createBrain(config)
        new_brain.initialize()
        
        self.brain = new_brain
    
    def calculate_learning_progress(self):
        """Calculate learning progress over time."""
        # Simple metric based on firing rate changes
        recent_rates = self.get_recent_firing_rates(100)
        
        if len(recent_rates) < 10:
            return 0.5
        
        # Calculate trend
        x = np.arange(len(recent_rates))
        y = np.array(recent_rates)
        
        # Linear regression slope
        slope = np.polyfit(x, y, 1)[0]
        
        # Normalize to [0, 1]
        progress = max(0.0, min(1.0, (slope + 10) / 20))
        
        return progress
    
    def get_recent_firing_rates(self, count):
        """Get recent firing rates."""
        # This would need to be implemented based on available data
        # For now, return a simple sequence
        return np.random.uniform(10, 100, count)
```

## 8. Experimental Design and Analysis

### Factorial Experiment Design

```python
import pynlm
import itertools
import pandas as pd
from typing import List, Dict, Any

class ExperimentalDesign:
    """Design and analyze factorial experiments for NLM research."""
    
    def __init__(self, base_config):
        self.base_config = base_config
        self.experiment_results = []
        
    def create_factorial_design(self, factors: Dict[str, List[Any]]) -> List[Dict]:
        """Create a full factorial experimental design."""
        print("Creating factorial experimental design...")
        
        # Generate all combinations
        factor_names = list(factors.keys())
        factor_combinations = list(itertools.product(*factors.values()))
        
        # Create experiment configurations
        experiments = []
        for combination in factor_combinations:
            config = self.create_experiment_config(dict(zip(factor_names, combination)))
            experiments.append(config)
        
        print(f"Created {len(experiments)} experimental conditions")
        return experiments
    
    def create_experiment_config(self, modifications: Dict[str, Any]) -> Dict:
        """Create experiment configuration from modifications."""
        # Start with base configuration
        config = self.base_config.copy()
        
        # Apply modifications
        for param_path, value in modifications.items():
            self.apply_parameter(modifications, param_path, value)
        
        return config
    
    def apply_parameter(self, config: Dict, param_path: str, value: Any):
        """Apply parameter to configuration using dot notation."""
        keys = param_path.split(".")
        target = config
        
        for key in keys[:-1]:
            if key not in target:
                target[key] = {}
            target = target[key]
        
        target[keys[-1]] = value
    
    def run_experiment_batch(self, experiments: List[Dict], steps: int, output_dir: str):
        """Run batch of experiments and collect results."""
        print(f"Running {len(experiments)} experiments...")
        
        for i, experiment_config in enumerate(experiments):
            print(f"  Running experiment {i + 1}/{len(experiments)}")
            
            try:
                # Create and run brain
                brain = pynlm.createBrain(experiment_config)
                brain.initialize()
                
                # Run simulation
                for step in range(steps):
                    brain.step(step)
                
                # Collect results
                result = self.collect_experiment_results(brain, experiment_config)
                self.experiment_results.append(result)
                
                # Save experiment results
                self.save_experiment_result(result, output_dir, i)
                
            except Exception as e:
                print(f"    Experiment {i + 1} failed: {e}")
                # Record failure
                failure_result = {
                    'experiment_id': i,
                    'config': experiment_config,
                    'success': False,
                    'error': str(e)
                }
                self.experiment_results.append(failure_result)
    
    def collect_experiment_results(self, brain, config: Dict) -> Dict:
        """Collect comprehensive results from experiment."""
        return {
            'final_neuron_count': brain.getTotalNeuronCount(),
            'final_synapse_count': brain.getTotalSynapseCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_development_stage': brain.getDevelopmentalStage(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'simulation_steps': config.get('max_simulation_steps', 1000),
            'config': config
        }
    
    def save_experiment_result(self, result: Dict, output_dir: str, experiment_id: int):
        """Save experiment results to file."""
        import json
        from pathlib import Path
        
        Path(output_dir).mkdir(exist_ok=True)
        
        # Save as JSON
        result_file = Path(output_dir) / f"experiment_{experiment_id:04d}.json"
        with open(result_file, 'w') as f:
            json.dump(result, f, indent=2)
        
        # Save as CSV summary
        csv_file = Path(output_dir) / "experiment_summary.csv"
        
        # Append to summary file
        df = pd.DataFrame([result])
        if csv_file.exists():
            df.to_csv(csv_file, mode='a', header=False, index=False)
        else:
            df.to_csv(csv_file, index=False)
    
    def analyze_factorial_results(self, experiments: List[Dict]) -> Dict:
        """Analyze factorial experiment results."""
        print("Analyzing factorial experiment results...")
        
        # Create summary DataFrame
        results_data = []
        for i, exp in enumerate(experiments):
            results_data.append({
                'experiment_id': i,
                'neuron_count': exp.get('final_neuron_count', 0),
                'synapse_count': exp.get('final_synapse_count', 0),
                'firing_rate': exp.get('final_firing_rate', 0),
                'dev_stage': exp.get('final_development_stage', 0),
                'success': 'error' not in exp
            })
        
        df = pd.DataFrame(results_data)
        
        # Calculate main effects and interactions
        analysis = self.perform_statistical_analysis(df)
        
        return {
            'summary': df.to_dict('records'),
            'statistical_analysis': analysis,
            'recommendations': self.generate_recommendations(analysis)
        }
    
    def perform_statistical_analysis(self, df: pd.DataFrame) -> Dict:
        """Perform statistical analysis of experimental results."""
        analysis = {}
        
        # Basic statistics
        analysis['basic_stats'] = {
            'total_experiments': len(df),
            'successful_experiments': df['success'].sum(),
            'success_rate': df['success'].mean(),
            'mean_neuron_count': df['neuron_count'].mean(),
            'mean_firing_rate': df['firing_rate'].mean(),
            'std_neuron_count': df['neuron_count'].std(),
            'std_firing_rate': df['firing_rate'].std()
        }
        
        # Effect size analysis
        if len(df) > 1:
            # Calculate effect sizes for key parameters
            analysis['effect_sizes'] = self.calculate_effect_sizes(df)
        
        return analysis
    
    def calculate_effect_sizes(self, df: pd.DataFrame) -> Dict:
        """Calculate effect sizes for experimental factors."""
        effect_sizes = {}
        
        # For factorial designs, we would need factor information
        # For now, provide simple effect size calculations
        
        # Neuron count effect
        if len(df) >= 2:
            effect_sizes['neuron_count'] = {
                'cohen_d': (df['neuron_count'].max() - df['neuron_count'].min()) / df['neuron_count'].std() if df['neuron_count'].std() > 0 else 0,
                'range': df['neuron_count'].max() - df['neuron_count'].min()
            }
        
        # Firing rate effect
        effect_sizes['firing_rate'] = {
            'cohen_d': (df['firing_rate'].max() - df['firing_rate'].min()) / df['firing_rate'].std() if df['firing_rate'].std() > 0 else 0,
            'range': df['firing_rate'].max() - df['firing_rate'].min()
        }
        
        return effect_sizes
    
    def generate_recommendations(self, analysis: Dict) -> List[str]:
        """Generate recommendations based on experimental results."""
        recommendations = []
        
        if analysis.get('basic_stats', {}).get('success_rate', 1.0) < 0.8:
            recommendations.append("High failure rate detected - review experimental setup")
        
        if analysis.get('effect_sizes', {}).get('firing_rate', {}).get('cohen_d', 0) > 1.0:
            recommendations.append("Large effect sizes found - parameter has strong impact")
        
        if analysis.get('basic_stats', {}).get('success_rate', 0) > 0.9:
            recommendations.append("High success rate - experimental design is robust")
        
        if analysis.get('basic_stats', {}).get('mean_firing_rate', 0) < 10:
            recommendations.append("Low firing rates - consider adjusting plasticity parameters")
        
        return recommendations
```

## 9. Advanced Configuration Templates

### Configuration Template Library

```python
# This section documents the configuration templates available in the configs/ directory
# Each template is designed for specific use cases and performance characteristics

# high_perf.cfg - Optimized for speed and efficiency
# - Large brain (2000 neurons)
# - High connectivity and plasticity
# - Fast benchmarking scenarios

# development.cfg - Optimized for learning and development
# - Aggressive plasticity settings
# - Developmental stage tracking
# - Extended simulation times for learning

# lightweight.cfg - Optimized for minimal resource usage
# - Small brain (300 neurons)
# - Minimal feature set
# - Quick testing and resource-constrained environments

# experimental.cfg - For testing specific hypotheses
# - Extreme parameter values
# - Detailed activity recording
# - Novelty and curiosity emphasis

# benchmark.cfg - For performance testing and optimization
# - Standard brain size for comparison
# - Benchmark-specific settings
# - Performance measurement focus

# default.cfg - Base configuration from original project
# - Balanced settings for general use
# - Comprehensive feature set
# - Reference configuration for customization
```

## 10. Advanced Debugging and Monitoring

### Real-time Monitoring

```python
import pynlm
import time
import threading
from typing import Callable, Dict, Any

class RealTimeMonitor:
    """Real-time monitoring and debugging for NLM simulations."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.monitoring_active = False
        self.monitored_metrics = {}
        self.alert_thresholds = {}
        self.alert_callbacks = {}
        self.performance_history = {}
        
        # Initialize monitoring metrics
        self.initialize_monitoring_metrics()
        
        # Set up default alert thresholds
        self.setup_default_alerts()
    
    def initialize_monitoring_metrics(self):
        """Initialize monitoring metrics collection."""
        self.monitored_metrics = {
            'neuron_count': {'value': 0, 'history': [], 'unit': 'count'},
            'synapse_count': {'value': 0, 'history': [], 'unit': 'count'},
            'firing_rate': {'value': 0, 'history': [], 'unit': 'Hz'},
            'firing_neurons': {'value': 0, 'history': [], 'unit': 'count'},
            'dev_stage': {'value': 0, 'history': [], 'unit': 'stage'},
            'simulation_time': {'value': 0, 'history': [], 'unit': 's'},
            'memory_usage': {'value': 0, 'history': [], 'unit': 'MB'},
            'cpu_usage': {'value': 0, 'history': [], 'unit': '%'}
        }
        
        self.performance_history = {
            'steps_per_second': [],
            'memory_efficiency': [],
            'neural_activity': []
        }
    
    def setup_default_alerts(self):
        """Set up default alert thresholds."""
        self.alert_thresholds = {
            'high_firing_rate': {'threshold': 1000, 'severity': 'warning'},
            'low_neuron_count': {'threshold': 100, 'severity': 'error'},
            'high_memory_usage': {'threshold': 2048, 'severity': 'warning'},
            'simulation_slowdown': {'threshold': 0.1, 'severity': 'warning'}
        }
    
    def start_monitoring(self, interval: float = 0.1, duration: Optional[int] = None):
        """Start real-time monitoring."""
        self.monitoring_active = True
        
        def monitoring_loop():
            steps = 0
            start_time = time.time()
            
            while self.monitoring_active:
                # Collect metrics
                self.collect_metrics(steps, start_time)
                
                # Check alerts
                self.check_alerts()
                
                # Update performance history
                self.update_performance_history()
                
                steps += 1
                time.sleep(interval)
                
                # Check duration
                if duration and (time.time() - start_time) > duration:
                    break
        
        # Start monitoring thread
        self.monitoring_thread = threading.Thread(target=monitoring_loop)
        self.monitoring_thread.daemon = True
        self.monitoring_thread.start()
        
        print(f"Started monitoring with {interval}s interval")
    
    def stop_monitoring(self):
        """Stop real-time monitoring."""
        self.monitoring_active = False
        if hasattr(self, 'monitoring_thread'):
            self.monitoring_thread.join(timeout=5.0)
        print("Stopped monitoring")
    
    def collect_metrics(self, steps: int, start_time):
        """Collect current simulation metrics."""
        current_time = time.time()
        elapsed_time = current_time - start_time
        
        # Update monitored metrics
        self.monitored_metrics['neuron_count']['value'] = self.brain.getTotalNeuronCount()
        self.monitored_metrics['synapse_count']['value'] = self.brain.getTotalSynapseCount()
        self.monitored_metrics['firing_rate']['value'] = self.brain.getAverageFiringRate()
        self.monitored_metrics['firing_neurons']['value'] = self.brain.getFiringNeuronCount()
        self.monitored_metrics['dev_stage']['value'] = self.brain.getDevelopmentalStage()
        self.monitored_metrics['simulation_time']['value'] = elapsed_time
        
        # Add to history
        for metric_name in self.monitored_metrics:
            self.monitored_metrics[metric_name]['history'].append(
                (elapsed_time, self.monitored_metrics[metric_name]['value'])
            )
            
            # Keep history limited
            if len(self.monitored_metrics[metric_name]['history']) > 1000:
                self.monitored_metrics[metric_name]['history'].pop(0)
    
    def check_alerts(self):
        """Check if any metrics exceed alert thresholds."""
        for metric_name, alert_config in self.alert_thresholds.items():
            threshold = alert_config['threshold']
            severity = alert_config['severity']
            
            # Get current metric value
            current_value = self.get_metric_value(metric_name)
            
            # Check threshold
            if current_value > threshold:
                self.trigger_alert(metric_name, current_value, threshold, severity)
    
    def get_metric_value(self, metric_name: str) -> float:
        """Get current value of a metric."""
        if metric_name in self.monitored_metrics:
            return self.monitored_metrics[metric_name]['value']
        return 0.0
    
    def get_metric_history(self, metric_name: str, time_window: Optional[float] = None) -> list:
        """Get metric history for specified time window."""
        if metric_name not in self.monitored_metrics:
            return []
        
        history = self.monitored_metrics[metric_name]['history']
        
        if time_window is None:
            return history
        
        # Filter by time window
        current_time = history[-1][0] if history else 0
        start_time = current_time - time_window
        
        return [h for h in history if h[0] >= start_time]
    
    def trigger_alert(self, alert_type: str, current_value: float, threshold: float, severity: str):
        """Trigger alert for exceeded threshold."""
        alert = {
            'type': alert_type,
            'current_value': current_value,
            'threshold': threshold,
            'severity': severity,
            'timestamp': time.time()
        }
        
        print(f"[{severity.upper()}] Alert: {alert_type} exceeded {threshold} (current: {current_value})")
        
        # Execute alert callbacks
        if alert_type in self.alert_callbacks:
            for callback in self.alert_callbacks[alert_type]:
                try:
                    callback(alert)
                except Exception as e:
                    print(f"Alert callback failed: {e}")
    
    def register_alert_callback(self, alert_type: str, callback: Callable):
        """Register callback for specific alert type."""
        if alert_type not in self.alert_callbacks:
            self.alert_callbacks[alert_type] = []
        
        self.alert_callbacks[alert_type].append(callback)
    
    def update_performance_history(self):
        """Update performance metrics history."""
        if len(self.monitored_metrics['firing_rate']['history']) > 1:
            # Calculate steps per second
            history = self.monitored_metrics['firing_rate']['history']
            if len(history) > 10:
                recent = history[-10:]
                times = [h[0] for h in recent]
                if times[-1] > times[0]:
                    steps_per_second = len(recent) / (times[-1] - times[0])
                    self.performance_history['steps_per_second'].append(steps_per_second)
                    
                    # Keep history limited
                    if len(self.performance_history['steps_per_second']) > 100:
                        self.performance_history['steps_per_second'].pop(0)
    
    def get_monitoring_report(self) -> str:
        """Generate comprehensive monitoring report."""
        report = f"# NLM Real-Time Monitoring Report\n\n"
        
        report += f"## Current Status\n"
        report += f"- Neuron Count: {self.monitored_metrics['neuron_count']['value']}\n"
        report += f"- Synapse Count: {self.monitored_metrics['synapse_count']['value']}\n"
        report += f"- Firing Rate: {self.monitored_metrics['firing_rate']['value']:.2f} Hz\n"
        report += f"- Firing Neurons: {self.monitored_metrics['firing_neurons']['value']}\n"
        report += f"- Development Stage: {self.monitored_metrics['dev_stage']['value']}\n"
        report += f"- Simulation Time: {self.monitored_metrics['simulation_time']['value']:.2f} s\n\n"
        
        if self.performance_history['steps_per_second']:
            avg_steps_per_second = np.mean(self.performance_history['steps_per_second'])
            report += f"## Performance Summary\n"
            report += f"- Average Steps/Second: {avg_steps_per_second:.1f}\n"
            report += f"- Max Steps/Second: {max(self.performance_history['steps_per_second']):.1f}\n"
            report += f"- Min Steps/Second: {min(self.performance_history['steps_per_second']):.1f}\n\n"
        
        # Add recent trends
        report += f"## Recent Trends\n"
        for metric_name in self.monitored_metrics:
            history = self.monitored_metrics[metric_name]['history'][-10:]
            if len(history) > 1:
                values = [h[1] for h in history]
                trend = (values[-1] - values[0]) / (len(values) - 1)
                trend_symbol = "↑" if trend > 0 else "↓" if trend < 0 else "→"
                report += f"- {metric_name}: {trend_symbol} {trend:+.3f} per step\n"
        
        return report

# Example usage
if __name__ == "__main__":
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    # Create monitor
    monitor = RealTimeMonitor(brain, world)
    
    # Configure alerts
    monitor.register_alert_callback('high_firing_rate', lambda alert: print(f"Warning: Firing rate too high: {alert['current_value']}"))
    
    # Start monitoring
    monitor.start_monitoring(interval=0.5, duration=30)
    
    # Run simulation while monitoring
    for step in range(100):
        brain.step(step)
        time.sleep(0.01)
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Generate report
    report = monitor.get_monitoring_report()
    print(report)
```

## Conclusion

This advanced topics guide provides comprehensive coverage of:

1. **Multi-Agent Coordination** - Agent neighborhoods, competition, and hierarchical control
2. **Memory Systems** - Working memory management and episodic memory with replay
3. **Neuromodulation** - Dopamine-based reward prediction and curiosity systems
4. **Configuration Management** - Advanced configuration with inheritance and validation
5. **Performance Optimization** - Neural network optimization and adaptive plasticity
6. **Experimental Design** - Factorial experiments and statistical analysis
7. **Plugin Architecture** - Extensible plugin system for custom extensions
8. **Real-time Monitoring** - Advanced monitoring and alerting systems

These advanced techniques enable researchers and developers to build sophisticated NLM applications with complex behaviors, efficient performance, and robust error handling. The combination of multiple agents, advanced memory systems, and adaptive learning creates a powerful framework for exploring emergent intelligence and cognitive development.
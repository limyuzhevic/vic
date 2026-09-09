# NLM Advanced Python Examples

## Advanced Usage Patterns

These examples demonstrate more complex usage of NLM and show how to leverage advanced features for research and development.

### Advanced Configuration Management

```python
import pynlm
import json

def create_advanced_config():
    """Create a brain with advanced configuration settings."""
    config = pynlm.createDefaultConfig()
    
    # Configure brain size and structure
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 3)
    config.set("connection_probability", 0.15f)
    
    # Advanced plasticity settings
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.stdp.tau_plus", 30.0f)
    config.set("plasticity.stdp.tau_minus", 30.0f)
    
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.001)
    
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.synaptogenesis_rate", 0.00005f)
    config.set("plasticity.structural.pruning_rate", 0.000005f)
    
    # Neuromodulation configuration
    config.set("neuromod.dopamine.scale", 1.2f)
    config.set("neuromod.dopamine.baseline", 0.1f)
    
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.scale", 0.8f)
    
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.threshold", 0.3f)
    
    config.set("neuromod.prediction_error.enable", True)
    
    # Development configuration
    config.set("development.stage_transition_rate", 0.01f)
    config.set("development.maturation_rate", 0.005f)
    
    # Memory system configuration
    config.set("memory.working_memory.capacity", 500)
    config.set("memory.episodic_memory.max_episodes", 5000)
    config.set("memory.associative_memory.similarity_threshold", 0.7f)
    
    # Prediction system configuration
    config.set("prediction.system_enabled", True)
    config.set("prediction.error_discount", 0.95f)
    
    return config

def save_config_to_file(config, filepath):
    """Save configuration to JSON file."""
    config_data = {
        "brain": {
            "neuron_count": config.getOr<int64_t>("neuron_count", 1000),
            "region_count": config.getOr<int64_t>("region_count", 1),
            "connection_probability": config.getOr<float>("connection_probability", 0.1f)
        },
        "plasticity": {
            "stdp": {
                "enable": config.getOr<bool>("plasticity.stdp.enable", True),
                "learning_rate": config.getOr<float>("plasticity.stdp.learning_rate", 0.001)
            }
        }
    }
    
    with open(filepath, 'w') as f:
        json.dump(config_data, f, indent=2)
    
    print(f"Configuration saved to: {filepath}")

def load_config_from_file(filepath):
    """Load configuration from JSON file."""
    config = pynlm.createDefaultConfig()
    
    try:
        with open(filepath, 'r') as f:
            config_data = json.load(f)
        
        # Load brain configuration
        if "brain" in config_data:
            brain_config = config_data["brain"]
            if "neuron_count" in brain_config:
                config.set("brain.neuron_count", brain_config["neuron_count"])
            if "region_count" in brain_config:
                config.set("brain.region_count", brain_config["region_count"])
        
        print(f"Configuration loaded from: {filepath}")
        
    except Exception as e:
        print(f"Error loading configuration: {e}")
    
    return config
```

### Working with Multiple Brain Regions

```python
import pynlm

def create_multi_region_brain():
    """Create a brain with specialized regions."""
    config = pynlm.createDefaultConfig()
    config.set("brain.region_count", 4)
    config.set("brain.neuron_count", 1600)  # 400 neurons per region
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get regions by ID (regions are automatically numbered starting from 1)
    regions = {}
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        if region:
            regions[region_id] = region
    
    # Modify region-specific configurations
    # Region 1: Sensory processing
    # Region 2: Internal processing  
    # Region 3: Motor control
    # Region 4: Memory integration
    
    print("Created multi-region brain with regions:")
    for region_id, region in regions.items():
        print(f"  Region {region_id.index()}: {region.getName()}")
        print(f"    Neurons: {region.getTotalNeuronCount()}")
        print(f"    Populations: {region.getPopulationCount()}")
    
    return brain, regions

def analyze_region_functionality(brain, regions):
    """Analyze functionality of different brain regions."""
    print("\n=== Region Analysis ===")
    
    for region_id, region in regions.items():
        print(f"\nRegion {region_id.index()} Analysis:")
        
        # Count neuron types in region
        neuron_types = {}
        for pop in region.getPopulations():
            pop_type = pop.getNeuronType()
            if pop_type not in neuron_types:
                neuron_types[pop_type] = 0
            neuron_types[pop_type] += pop.getNeuronCount()
        
        for neuron_type, count in neuron_types.items():
            print(f"  {neuron_type} neurons: {count}")
        
        # Calculate average synaptic weight
        avg_weight = region.getAverageSynapticWeight()
        print(f"  Average synaptic weight: {avg_weight:.4f}")
        
        # Get region statistics
        firing_rate = region.getAverageFiringRate()
        active_count = region.getActiveNeuronCount()
        print(f"  Average firing rate: {firing_rate:.4f}")
        print(f"  Active neurons: {active_count}")
```

### Advanced Agent Behaviors

```python
import pynlm
import random

class AdaptiveAgent:
    """Agent that adapts its behavior based on environment and learning."""
    
    def __init__(self, brain, world, config):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.config = config
        
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Agent state
        self.action_history = []
        self.reward_history = []
        self.step_count = 0
    
    def run_episode(self, max_steps):
        """Run a single episode of agent-environment interaction."""
        print(f"Starting episode with {max_steps} steps")
        
        # Reset environment
        self.world.reset()
        
        # Run episode
        for step in range(max_steps):
            # 1. Get sensory input
            percept = self.world.getSensoryPercept()
            
            # 2. Process sensory input
            self.agent.processSensoryInput(percept)
            
            # 3. Brain computation
            self.brain.step(self.step_count)
            
            # 4. Decision making
            action = self.agent.decodeMotorCommand()
            
            # 5. Execute action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # 6. Calculate reward
            reward = self.calculate_reward(action, step)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # 7. Update development
            self.agent.updateDevelopment(0.1)
            
            # Record experience
            self.action_history.append(action)
            self.reward_history.append(reward)
            
            # Log progress every 100 steps
            if step % 100 == 0:
                self.print_step_stats(step, reward)
            
            self.step_count += 1
        
        return self.calculate_episode_stats()
    
    def calculate_reward(self, action, step):
        """Calculate reward based on action and environment state."""
        base_reward = 0.0
        
        # Reward based on action type
        if action.getType() == pynlm.ActionType.Eat:
            base_reward += 1.0
        elif action.getType() == pynlm.ActionType.MoveForward:
            base_reward += 0.1
        elif action.getType() == pynlm.ActionType.TurnLeft:
            base_reward += 0.05
        
        # Add environmental reward (could be based on world state)
        world_reward = self.world.computeReward(self.brain.getRegions()[0].get())
        
        # Add novelty bonus (curiosity-driven reward)
        novelty_bonus = self.agent.getNoveltyLevel() * 0.5
        
        # Add development bonus (encourage learning)
        dev_bonus = 0.01 if self.brain.getDevelopmentalStage() < pynlm.DevelopmentalStage.Adult else 0.0
        
        total_reward = base_reward + world_reward + novelty_bonus + dev_bonus
        
        return total_reward
    
    def print_step_stats(self, step, reward):
        """Print statistics for current step."""
        print(f"\nStep {step} Statistics:")
        print(f"  Action: {action.getType()}")
        print(f"  Reward: {reward:.3f}")
        print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {self.brain.getAverageFiringRate():.4f}")
        print(f"  Curiosity level: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {self.brain.getDevelopmentalStage()}")
    
    def calculate_episode_stats(self):
        """Calculate statistics for completed episode."""
        if not self.reward_history:
            return {}
        
        avg_reward = sum(self.reward_history) / len(self.reward_history)
        max_reward = max(self.reward_history)
        min_reward = min(self.reward_history)
        
        # Count action types
        action_counts = {}
        for action in self.action_history:
            action_type = action.getType()
            action_counts[action_type] = action_counts.get(action_type, 0) + 1
        
        return {
            'steps': len(self.action_history),
            'avg_reward': avg_reward,
            'max_reward': max_reward,
            'min_reward': min_reward,
            'action_counts': action_counts,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'final_development_stage': self.brain.getDevelopmentalStage()
        }
```

### Advanced Learning Patterns

```python
import pynlm
import numpy as np

class PatternLearner:
    """Learn patterns from sensory input and apply them to behavior."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.pattern_database = {}
        self.concept_formation = {}
        
        # Enable concept formation
        self.concept_formation_enabled = True
    
    def learn_from_experience(self, steps_per_pattern=100):
        """Learn patterns from extended sensory experience."""
        print("Starting pattern learning...")
        
        # Collect sensory data
        sensory_sequences = self.collect_sensory_sequences(steps_per_pattern)
        
        # Extract patterns from sequences
        patterns = self.extract_patterns(sensory_sequences)
        
        # Store patterns in database
        self.pattern_database.update(patterns)
        
        # Learn concepts from patterns
        if self.concept_formation_enabled:
            self.concept_formation = self.learn_concepts(patterns)
        
        print(f"Learned {len(patterns)} patterns and {len(self.concept_formation)} concepts")
        
        return patterns
    
    def collect_sensory_sequences(self, steps):
        """Collect sequences of sensory input."""
        sequences = []
        
        # Reset world
        self.world.reset()
        
        # Collect sensory data
        current_sequence = []
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Store sensory data (simplified representation)
            sensory_data = {
                'vision': percept.getVision() if hasattr(percept, 'getVision') else [],
                'internal': percept.getInternal() if hasattr(percept, 'getInternal') else [],
                'timestamp': step
            }
            
            current_sequence.append(sensory_data)
            
            # Process input (simulate experience)
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
        
        sequences.append(current_sequence)
        return sequences
    
    def extract_patterns(self, sequences):
        """Extract patterns from sensory sequences."""
        patterns = {}
        
        for seq_idx, sequence in enumerate(sequences):
            # Simple pattern detection based on sensory statistics
            if sequence:
                # Create a simplified pattern signature
                pattern_key = f"sequence_{seq_idx}"
                
                # Extract features from sequence
                features = self.extract_features(sequence)
                
                # Store pattern
                patterns[pattern_key] = {
                    'features': features,
                    'sequence_length': len(sequence),
                    'complexity': self.calculate_complexity(features),
                    'learned_at': seq_idx
                }
        
        return patterns
    
    def extract_features(self, sequence):
        """Extract features from a sensory sequence."""
        features = {}
        
        if not sequence:
            return features
        
        # Vision features
        vision_values = []
        for data in sequence:
            if 'vision' in data and data['vision']:
                vision_values.extend(data['vision'])
        
        if vision_values:
            features['vision_mean'] = np.mean(vision_values)
            features['vision_std'] = np.std(vision_values)
            features['vision_max'] = np.max(vision_values)
        
        # Internal signal features
        internal_values = []
        for data in sequence:
            if 'internal' in data and data['internal']:
                internal_values.extend(data['internal'])
        
        if internal_values:
            features['internal_mean'] = np.mean(internal_values)
            features['internal_std'] = np.std(internal_values)
        
        return features
    
    def calculate_complexity(self, features):
        """Calculate complexity of pattern features."""
        if not features:
            return 0.0
        
        # Calculate complexity as variance of normalized features
        feature_values = list(features.values())
        if not feature_values:
            return 0.0
        
        # Normalize features
        normalized = [(v - min(feature_values)) / (max(feature_values) - min(feature_values) 
                       if max(feature_values) != min(feature_values) else 1.0) 
                      for v in feature_values]
        
        # Calculate complexity (entropy-like measure)
        complexity = -sum(p * np.log2(p) for p in normalized if p > 0)
        
        return complexity
    
    def learn_concepts(self, patterns):
        """Learn abstract concepts from patterns."""
        concepts = {}
        
        for pattern_key, pattern_data in patterns.items():
            # Group patterns into concepts based on similarity
            features = pattern_data['features']
            
            # Find similar patterns
            similar_patterns = []
            for other_key, other_data in patterns.items():
                if other_key != pattern_key:
                    similarity = self.calculate_similarity(features, other_data['features'])
                    if similarity > 0.7:  # Threshold for concept formation
                        similar_patterns.append(other_key)
            
            if similar_patterns:
                # Create concept from similar patterns
                concept_name = f"concept_{len(concepts)}"
                concepts[concept_name] = {
                    'patterns': [pattern_key] + similar_patterns,
                    'features': self.average_features([features] + 
                                                   [patterns[p]['features'] for p in similar_patterns]),
                    'stability': len(similar_patterns) / (len(patterns) + 1)
                }
        
        return concepts
    
    def calculate_similarity(self, features1, features2):
        """Calculate similarity between two feature sets."""
        if not features1 or not features2:
            return 0.0
        
        # Get all unique feature names
        all_features = set(features1.keys()) | set(features2.keys())
        
        if not all_features:
            return 0.0
        
        # Calculate cosine similarity
        similarities = []
        for feature in all_features:
            val1 = features1.get(feature, 0.0)
            val2 = features2.get(feature, 0.0)
            similarities.append(val1 * val2)
        
        return np.sqrt(sum(similarities)) if similarities else 0.0
    
    def average_features(self, feature_lists):
        """Average multiple feature dictionaries."""
        if not feature_lists:
            return {}
        
        all_features = {}
        for feature_dict in feature_lists:
            for feature, value in feature_dict.items():
                if feature not in all_features:
                    all_features[feature] = []
                all_features[feature].append(value)
        
        # Calculate averages
        averaged = {}
        for feature, values in all_features.items():
            averaged[feature] = sum(values) / len(values)
        
        return averaged
    
    def apply_concepts_to_behavior(self, brain, agent):
        """Apply learned concepts to influence agent behavior."""
        print("Applying learned concepts to behavior...")
        
        # Modify agent behavior based on concepts
        for concept_name, concept_data in self.concept_formation.items():
            stability = concept_data['stability']
            
            # Apply concept-based modifications
            if stability > 0.8:  # High stability concepts strongly influence behavior
                # Modify agent parameters based on concept
                features = concept_data['features']
                
                if 'vision_mean' in features:
                    # Adjust exploration based on visual environment
                    exploration_factor = min(1.0, features['vision_mean'] * 0.5)
                    # Enable or disable curiosity based on concept
                    agent.enableCuriosity(exploration_factor > 0.3)
                
                if 'internal_mean' in features:
                    # Adjust learning rate based on internal state
                    learning_rate = max(0.001, min(0.01, features['internal_mean'] * 0.002))
                    # Note: NLM doesn't expose learning rate directly
        
        print(f"Applied {len(self.concept_formation)} concepts to behavior")
```

### Multi-Agent Collaboration

```python
import pynlm
import random

class MultiAgentSystem:
    """System that manages multiple collaborating agents."""
    
    def __init__(self, config):
        self.config = config
        self.agents = []
        self.world = None
        self.step_count = 0
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    
    def add_agent(self, brain):
        """Add an agent to the system."""
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(self.world)
        
        # Enable different features for different agents
        agent.enableRewardModulation(True)
        
        # Vary agent capabilities
        if len(self.agents) % 3 == 0:
            # Specialist agent
            agent.enableStructuralPlasticity(True)
            agent.enableDevelopment(True)
        elif len(self.agents) % 3 == 1:
            # Curious agent
            agent.enableCuriosity(True)
        else:
            # Standard agent
            agent.enableNovelty(True)
        
        self.agents.append(agent)
        return agent
    
    def run_collaborative_simulation(self, steps_per_agent=100):
        """Run simulation with multiple collaborating agents."""
        print(f"Running collaborative simulation with {len(self.agents)} agents")
        
        # Create experiences for all agents
        agent_experiences = []
        
        for i, agent in enumerate(self.agents):
            print(f"\n--- Agent {i} Learning ---")
            experience = self.run_agent_episode(agent, steps_per_agent)
            agent_experiences.append(experience)
        
        # Analyze collaboration opportunities
        collaboration_analysis = self.analyze_collaboration(agent_experiences)
        
        return agent_experiences, collaboration_analysis
    
    def run_agent_episode(self, agent, max_steps):
        """Run episode for a single agent."""
        # Reset world for each agent to avoid interference
        self.world.reset()
        
        episode_data = {
            'rewards': [],
            'actions': [],
            'steps': max_steps
        }
        
        for step in range(max_steps):
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain (assuming agent has access to brain)
            # Note: In real implementation, you'd need access to each agent's brain
            # For this example, we'll simulate
            
            # Get action (simplified - would normally come from agent's brain)
            action = pynlm.Action(pynlm.ActionType.MoveForward)
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate reward
            reward = random.uniform(0, 1)  # Simplified reward
            agent.applyRewardModulation(reward, 0.0)
            
            # Record experience
            episode_data['rewards'].append(reward)
            episode_data['actions'].append(action)
            
            # Update world
            self.world.update(0.1)
        
        return episode_data
    
    def analyze_collaboration(self, agent_experiences):
        """Analyze opportunities for agent collaboration."""
        analysis = {
            'total_agents': len(agent_experiences),
            'avg_rewards': [],
            'action_diversity': 0,
            'learning_patterns': []
        }
        
        # Calculate average rewards for each agent
        for i, experience in enumerate(agent_experiences):
            avg_reward = sum(experience['rewards']) / len(experience['rewards'])
            analysis['avg_rewards'].append(avg_reward)
            analysis['learning_patterns'].append(self.identify_learning_pattern(experience))
        
        # Count unique action types
        all_actions = []
        for experience in agent_experiences:
            for action in experience['actions']:
                all_actions.append(action.getType())
        
        analysis['action_diversity'] = len(set(all_actions))
        
        return analysis
    
    def identify_learning_pattern(self, experience):
        """Identify learning pattern from agent experience."""
        rewards = experience['rewards']
        
        if not rewards:
            return "unknown"
        
        # Calculate reward progression
        if len(rewards) < 10:
            return "insufficient_data"
        
        # Simple pattern detection
        first_half = rewards[:len(rewards)//2]
        second_half = rewards[len(rewards)//2:]
        
        avg_first = sum(first_half) / len(first_half)
        avg_second = sum(second_half) / len(second_half)
        
        if avg_second > avg_first * 1.1:
            return "improving"
        elif avg_second < avg_first * 0.9:
            return "declining"
        else:
            return "stable"
```

### Research and Development Tools

```python
import pynlm
import json
from datetime import datetime

class NLMResearchTools:
    """Tools for NLM research and experimentation."""
    
    def __init__(self):
        self.experiments = {}
        self.results = {}
        self.parameters_log = []
    
    def create_experiment(self, name, description, parameters):
        """Create a new experiment configuration."""
        experiment = {
            'name': name,
            'description': description,
            'parameters': parameters,
            'created_at': datetime.now().isoformat(),
            'status': 'created'
        }
        
        self.experiments[name] = experiment
        self.parameters_log.append({
            'timestamp': datetime.now().isoformat(),
            'action': 'create_experiment',
            'name': name,
            'parameters': parameters
        })
        
        print(f"Created experiment: {name}")
        return experiment
    
    def run_experiment(self, experiment_name, num_runs=1):
        """Run an experiment multiple times."""
        if experiment_name not in self.experiments:
            print(f"Experiment '{experiment_name}' not found")
            return None
        
        print(f"Running experiment: {experiment_name} ({num_runs} runs)")
        
        experiment_params = self.experiments[experiment_name]['parameters']
        all_results = []
        
        for run in range(num_runs):
            print(f"  Run {run + 1}/{num_runs}")
            
            # Create brain with experiment parameters
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", experiment_params.get('neuron_count', 1000))
            config.set("plasticity.stdp.enable", experiment_params.get('enable_stdp', True))
            config.set("plasticity.hebbian.enable", experiment_params.get('enable_hebbian', True))
            config.set("neuromod.dopamine.scale", experiment_params.get('dopamine_scale', 1.0))
            config.set("neuromod.curiosity.enable", experiment_params.get('enable_curiosity', True))
            
            # Run experiment
            result = self.run_single_experiment(config, experiment_params)
            
            # Store result
            run_result = {
                'run': run + 1,
                'timestamp': datetime.now().isoformat(),
                'result': result
            }
            
            all_results.append(run_result)
            
            self.parameters_log.append({
                'timestamp': datetime.now().isoformat(),
                'action': 'run_experiment',
                'experiment': experiment_name,
                'run': run + 1,
                'parameters': experiment_params
            })
        
        # Calculate summary statistics
        summary = self.calculate_experiment_summary(all_results)
        
        # Store experiment results
        self.results[experiment_name] = {
            'experiment': self.experiments[experiment_name],
            'runs': all_results,
            'summary': summary,
            'completed_at': datetime.now().isoformat()
        }
        
        return self.results[experiment_name]
    
    def run_single_experiment(self, config, params):
        """Run a single experiment with given configuration."""
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create environment
        world = pynlm.createSimpleWorld()
        world.configure(
            width=params.get('world_width', 10),
            height=params.get('world_height', 10),
            visionWidth=params.get('vision_width', 8),
            visionHeight=params.get('vision_height', 8)
        )
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features based on experiment parameters
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(params.get('enable_structural_plasticity', True))
        agent.enableDevelopment(params.get('enable_development', True))
        agent.enableCuriosity(params.get('enable_curiosity', True))
        
        # Run experiment
        num_steps = params.get('steps', 500)
        experiment_data = {
            'rewards': [],
            'actions': [],
            'firing_rates': [],
            'curiosity_levels': [],
            'novelty_levels': []
        }
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Calculate reward
            reward = self.calculate_experiment_reward(action, step, params)
            agent.applyRewardModulation(reward, 0.0)
            
            # Collect data
            experiment_data['rewards'].append(reward)
            experiment_data['actions'].append(action)
            experiment_data['firing_rates'].append(brain.getAverageFiringRate())
            experiment_data['curiosity_levels'].append(agent.getCuriosityLevel())
            experiment_data['novelty_levels'].append(agent.getNoveltyLevel())
        
        # Calculate final statistics
        final_stats = self.calculate_experiment_statistics(experiment_data, params)
        
        return {
            'data': experiment_data,
            'final_stats': final_stats,
            'config': config
        }
    
    def calculate_experiment_reward(self, action, step, params):
        """Calculate reward based on experiment parameters."""
        base_reward = 0.0
        
        # Reward based on action type
        action_type = action.getType()
        if action_type == pynlm.ActionType.Eat:
            base_reward += params.get('reward_eat', 2.0)
        elif action_type == pynlm.ActionType.MoveForward:
            base_reward += params.get('reward_move', 0.5)
        elif action_type == pynlm.ActionType.TurnLeft:
            base_reward += params.get('reward_turn', 0.2)
        
        # Add curiosity bonus
        curiosity_bonus = params.get('curiosity_bonus', 0.1)
        
        # Add novelty bonus
        novelty_bonus = params.get('novelty_bonus', 0.05)
        
        # Add step bonus (encourage longer episodes)
        step_bonus = params.get('step_bonus', 0.01) if step > 100 else 0.0
        
        # Add penalty for bad actions
        penalty = 0.0
        if params.get('penalize_wait', False) and action_type == pynlm.ActionType.Wait:
            penalty = params.get('wait_penalty', 0.5)
        
        total_reward = base_reward + curiosity_bonus + novelty_bonus + step_bonus - penalty
        
        return total_reward
    
    def calculate_experiment_statistics(self, data, params):
        """Calculate final statistics for experiment."""
        stats = {
            'total_steps': len(data['rewards']),
            'total_reward': sum(data['rewards']),
            'avg_reward': sum(data['rewards']) / len(data['rewards']) if data['rewards'] else 0,
            'max_reward': max(data['rewards']) if data['rewards'] else 0,
            'min_reward': min(data['rewards']) if data['rewards'] else 0,
            'reward_variance': 0,
            'action_distribution': {},
            'average_firing_rate': sum(data['firing_rates']) / len(data['firing_rates']) if data['firing_rates'] else 0,
            'average_curiosity': sum(data['curiosity_levels']) / len(data['curiosity_levels']) if data['curiosity_levels'] else 0,
            'average_novelty': sum(data['novelty_levels']) / len(data['novelty_levels']) if data['novelty_levels'] else 0,
        }
        
        if len(data['rewards']) > 1:
            stats['reward_variance'] = sum((r - stats['avg_reward']) ** 2 for r in data['rewards']) / len(data['rewards'])
        
        # Calculate action distribution
        for action in data['actions']:
            action_type = action.getType()
            stats['action_distribution'][str(action_type)] = stats['action_distribution'].get(str(action_type), 0) + 1
        
        return stats
    
    def calculate_experiment_summary(self, all_results):
        """Calculate summary statistics for all experiment runs."""
        if not all_results:
            return {}
        
        summary = {
            'num_runs': len(all_results),
            'avg_total_reward': 0,
            'avg_avg_reward': 0,
            'avg_firing_rate': 0,
            'success_rate': 0
        }
        
        total_rewards = []
        avg_rewards = []
        firing_rates = []
        successful_runs = 0
        
        for result in all_results:
            final_stats = result['result']['final_stats']
            
            total_rewards.append(final_stats['total_reward'])
            avg_rewards.append(final_stats['avg_reward'])
            firing_rates.append(final_stats['average_firing_rate'])
            
            # Consider successful if average reward is above threshold
            if final_stats['avg_reward'] > 0.1:  # Threshold
                successful_runs += 1
        
        summary['avg_total_reward'] = sum(total_rewards) / len(total_rewards)
        summary['avg_avg_reward'] = sum(avg_rewards) / len(avg_rewards)
        summary['avg_firing_rate'] = sum(firing_rates) / len(firing_rates)
        summary['success_rate'] = successful_runs / len(all_results)
        
        return summary
    
    def export_results(self, filename):
        """Export experiment results to JSON file."""
        export_data = {
            'experiments': self.experiments,
            'results': self.results,
            'parameters_log': self.parameters_log,
            'exported_at': datetime.now().isoformat()
        }
        
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        print(f"Results exported to: {filename}")
        return filename
    
    def import_results(self, filename):
        """Import experiment results from JSON file."""
        try:
            with open(filename, 'r') as f:
                import_data = json.load(f)
            
            self.experiments = import_data.get('experiments', {})
            self.results = import_data.get('results', {})
            self.parameters_log = import_data.get('parameters_log', [])
            
            print(f"Results imported from: {filename}")
            return True
            
        except Exception as e:
            print(f"Error importing results: {e}")
            return False
```

### Advanced Python Integration

```python
import pynlm
import numpy as np
import matplotlib.pyplot as plt

class NLMAnalysisTools:
    """Tools for analyzing NLM simulation results."""
    
    def __init__(self):
        self.analysis_history = []
    
    def plot_brain_dynamics(self, brain, save_path=None):
        """Plot brain dynamics over time."""
        # This is a simplified example - actual implementation would require
        # time-series data collection during simulation
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Neuron count distribution
        ax = axes[0, 0]
        ax.bar(range(brain.getRegionCount()), 
               [brain.getRegion(region_id).getTotalNeuronCount() 
                for region_id in brain.getRegionIds() if brain.getRegion(region_id)],
               color='skyblue')
        ax.set_title('Neurons per Region')
        ax.set_ylabel('Neuron Count')
        
        # Plot 2: Synapse weights
        ax = axes[0, 1]
        all_synapses = []
        for region in brain.getRegions():
            all_synapses.extend([syn->getWeight() for syn in region->getSynapses()])
        
        if all_synapses:
            ax.hist(all_synapses, bins=50, alpha=0.7, color='lightgreen')
            ax.set_title('Synaptic Weight Distribution')
            ax.set_xlabel('Synaptic Weight')
            ax.set_ylabel('Count')
        
        # Plot 3: Memory system status
        ax = axes[1, 0]
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        
        if working_memory:
            ax.bar(['Working Memory'], [working_memory->getActiveTraces()], color='orange')
            ax.set_title('Current Memory States')
            ax.set_ylabel('Active Traces')
        
        # Plot 4: Developmental stage
        ax = axes[1, 1]
        developmental_stage = brain.getDevelopmentalStage()
        stages = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
        stage_values = [1 if i == list(stages).index(developmental_stage) else 0 
                       for i in range(len(stages))]
        
        ax.bar(stages, stage_values, color=['red', 'yellow', 'orange', 'green', 'blue'])
        ax.set_title('Current Developmental Stage')
        ax.set_ylabel('Active')
        ax.set_ylim(0, 1.2)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"Brain dynamics plot saved to: {save_path}")
        
        plt.show()
        plt.close()
    
    def analyze_learning_trajectory(self, simulation_results):
        """Analyze learning trajectory from simulation results."""
        analysis = {
            'total_steps': simulation_results['total_steps'],
            'reward_trajectory': simulation_results['rewards'],
            'firing_rate_trajectory': simulation_results['firing_rates'],
            'curiosity_trajectory': simulation_results['curiosity_levels']
        }
        
        # Calculate learning metrics
        rewards = simulation_results['rewards']
        if len(rewards) > 10:
            # Calculate average reward in first half vs second half
            mid_point = len(rewards) // 2
            first_half_avg = sum(rewards[:mid_point]) / mid_point
            second_half_avg = sum(rewards[mid_point:]) / (len(rewards) - mid_point)
            
            analysis['improvement_rate'] = (second_half_avg - first_half_avg) / first_half_avg if first_half_avg > 0 else 0
            analysis['learning_stability'] = np.std(rewards)
        
        # Detect learning phases
        analysis['learning_phases'] = self.detect_learning_phases(rewards)
        
        return analysis
    
    def detect_learning_phases(self, rewards):
        """Detect different learning phases in reward trajectory."""
        phases = []
        
        if len(rewards) < 20:
            return phases
        
        # Simple phase detection based on reward changes
        window_size = max(10, len(rewards) // 10)
        
        for i in range(0, len(rewards) - window_size, window_size):
            window = rewards[i:i + window_size]
            avg_reward = sum(window) / len(window)
            
            if i == 0:
                phase_type = 'exploration'
            elif avg_reward > max(rewards) * 0.8:
                phase_type = 'exploitation'
            elif avg_reward < max(rewards) * 0.2:
                phase_type = 'exploration'
            else:
                phase_type = 'learning'
            
            phases.append({
                'start_step': i,
                'end_step': min(i + window_size, len(rewards)),
                'type': phase_type,
                'average_reward': avg_reward
            })
        
        return phases
    
    def compare_experiments(self, experiment_results):
        """Compare results from multiple experiments."""
        if not experiment_results:
            return {}
        
        comparison = {
            'num_experiments': len(experiment_results),
            'avg_performance': {},
            'statistical_significance': {}
        }
        
        # Collect metrics from all experiments
        all_rewards = []
        all_firing_rates = []
        
        for exp_name, exp_data in experiment_results.items():
            final_stats = exp_data['final_stats']
            all_rewards.append(final_stats['avg_reward'])
            all_firing_rates.append(final_stats['average_firing_rate'])
        
        # Calculate overall averages
        comparison['avg_performance']['avg_reward'] = np.mean(all_rewards)
        comparison['avg_performance']['reward_std'] = np.std(all_rewards)
        comparison['avg_performance']['avg_firing_rate'] = np.mean(all_firing_rates)
        
        # Check for statistical significance (simplified t-test)
        if len(all_rewards) > 1:
            comparison['statistical_significance']['significant'] = (
                abs(np.mean(all_rewards) - 0.5) > 0.2  # Arbitrary threshold
            )
        
        return comparison

# Example usage of advanced analysis tools
analysis_tools = NLMAnalysisTools()

# Create brain and run simulation for analysis
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps for analysis
for i in range(100):
    brain.step(i)

# Analyze and plot brain dynamics
analysis_tools.plot_brain_dynamics(brain, 'brain_dynamics.png')

print("Advanced analysis completed successfully!")
```

These advanced examples demonstrate how to create sophisticated NLM applications for research, experimentation, and analysis. They cover configuration management, pattern learning, multi-agent systems, and comprehensive analysis tools.

## Further Learning

To master advanced NLM usage:

1. **Study the C++ source code** to understand neural dynamics implementation
2. **Experiment with different plasticity rules** to see their effects on learning
3. **Create custom environments** tailored to specific research questions
4. **Combine multiple agent strategies** for more complex behaviors
5. **Use the analysis tools** to extract insights from simulation data

The NLM framework provides a powerful foundation for creating intelligent, learning systems inspired by biological brains. These advanced examples will help you leverage the full potential of the neural simulation platform.

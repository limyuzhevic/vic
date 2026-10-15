# Advanced NLM Development - Cutting-Edge Techniques

This tutorial covers advanced NLM development techniques for researchers and professionals working with next-generation neural architectures. It focuses on pushing the boundaries of what's possible with the NLM framework.

## Prerequisites

Before starting this tutorial, you should be comfortable with:

- ✅ Intermediate NLM concepts (from intermediate tutorial)
- ✅ Building sophisticated agent systems
- ✅ Serialization and checkpointing
- ✅ Performance optimization techniques
- ✅ Error handling and robust simulation design

## What You'll Learn

By the end of this tutorial, you will be able to:

1. ✅ Design meta-learning systems that learn how to learn
2. ✅ Implement transfer learning across different NLM architectures
3. ✅ Build adaptive agents that modify themselves based on environment
4. ✅ Integrate NLM with other AI systems (RL, symbolic reasoning, etc.)
5. ✅ Scale NLM systems for research and production use
6. ✅ Implement advanced research experiments and novel architectures
7. ✅ Deploy NLM systems in real-world scenarios

---

## Part 1: Meta-Learning and Adaptive Systems

Meta-learning involves creating systems that can learn how to learn more efficiently. This section shows you how to build adaptive NLM agents.

### Meta-Learning NLM Agents

```python
import pynlm as nlm

print("=== Meta-Learning NLM Agents ===\n")

class MetaLearningAgent:
    """Agent that learns how to learn (meta-learning)."""
    
    def __init__(self, meta_config_profile="development"):
        self.meta_learning_history = []
        self.adaptation_strategies = []
        self.learning_rate_adaptation = {}
        self.strategy_selection_weights = {}
        
        # Initialize meta-configuration
        self.meta_config = nlm.Config()
        self.meta_config.setDefaults(meta_config_profile)
        
        # Initialize learning rate adaptation
        self.base_learning_rate = 0.01
        self.current_learning_rate = self.base_learning_rate
        self.performance_history = []
        
        # Initialize strategy selection
        self.strategies = {
            'exploration_heuristic': self.exploration_heuristic,
            'curiosity_driven': self.curiosity_driven_learning,
            'reward_prediction': self.reward_prediction_learning,
            'social_learning': self.social_learning,
            'developmental': self.development_driven_learning
        }
        
        # Initialize weights for strategy selection
        for strategy_name in self.strategies:
            self.strategy_selection_weights[strategy_name] = 1.0 / len(self.strategies)
    
    def exploration_heuristic(self, agent, world, metrics):
        """Exploration strategy based on environment uncertainty."""
        exploration_rate = self.current_learning_rate * 0.5
        novelty_threshold = 0.3
        
        # Calculate environment novelty
        percept = world.getSensoryPercept()
        novelty = self.calculate_novelty(percept)
        
        if novelty > novelty_threshold:
            return True, f"Environment is novel (novelty={novelty:.3f}), exploring"
        else:
            return False, f"Environment is familiar (novelty={novelty:.3f}), exploiting"
    
    def curiosity_driven_learning(self, agent, world, metrics):
        """Curiosity-driven learning strategy."""
        curiosity_threshold = 0.5
        
        # Get agent's curiosity level
        curiosity_level = agent.getCuriosityLevel()
        
        if curiosity_level > curiosity_threshold:
            return True, f"High curiosity ({curiosity_level:.3f}), exploring new experiences"
        else:
            return False, f"Low curiosity ({curiosity_level:.3f}), consolidating knowledge"
    
    def reward_prediction_learning(self, agent, world, metrics):
        """Learning based on reward prediction errors."""
        if not metrics:
            return False, "No reward history available"
        
        # Calculate prediction accuracy
        reward_prediction = sum(m['reward_prediction'] for m in metrics) / len(metrics)
        actual_rewards = sum(m['total_reward'] for m in metrics) / len(metrics)
        
        prediction_error = abs(actual_rewards - reward_prediction)
        prediction_threshold = 0.1 * actual_rewards
        
        if prediction_error > prediction_threshold:
            return True, f"High prediction error ({prediction_error:.3f}), learning from discrepancies"
        else:
            return False, f"Low prediction error ({prediction_error:.3f}), stable performance"
    
    def social_learning(self, agent, world, metrics):
        """Learning from other agents' experiences."""
        if len(metrics) < 3:
            return False, "Insufficient social data"
        
        # Calculate average performance of other agents
        avg_success_rate = sum(m['success_rate'] for m in metrics) / len(metrics)
        avg_reward = sum(m['total_reward'] for m in metrics) / len(metrics)
        
        # If agent performance is below average, learn socially
        current_success = metrics[-1]['success_rate']
        current_reward = metrics[-1]['total_reward']
        
        if current_success < avg_success_rate or current_reward < avg_reward:
            return True, f"Below average performance (success: {current_success:.3f}, reward: {current_reward:.3f})"
        else:
            return False, f"Above average performance (success: {current_success:.3f}, reward: {current_reward:.3f})"
    
    def development_driven_learning(self, agent, world, metrics):
        """Learning based on developmental stage."""
        # Get agent's developmental stage
        developmental_stage = metrics[-1]['development_stage'] if metrics else 0.0
        
        if developmental_stage < 0.3:
            return True, f"Early developmental stage ({developmental_stage:.3f}), rapid learning"
        elif developmental_stage < 0.7:
            return True, f"Middle developmental stage ({developmental_stage:.3f}), consolidation"
        else:
            return False, f"Late developmental stage ({developmental_stage:.3f}), refinement"
    
    def calculate_novelty(self, percept):
        """Calculate novelty of sensory input."""
        # Simplified novelty calculation
        data = percept.getData()
        
        if not data:
            return 0.0
        
        # Calculate variance as novelty measure
        mean_value = sum(data) / len(data)
        variance = sum((x - mean_value) ** 2 for x in data) / len(data)
        
        return min(variance, 1.0)  # Normalize to [0, 1]
    
    def adapt_learning_strategy(self, agent, world, metrics):
        """Adapt learning strategy based on performance."""
        if not metrics:
            return False, "No metrics available for adaptation"
        
        # Get latest metrics
        latest_metrics = metrics[-1]
        
        # Adapt based on success rate
        success_rate = latest_metrics['success_rate']
        if success_rate < 0.3:
            # Poor performance - increase exploration
            self.current_learning_rate *= 1.5
            return True, f"Poor success rate ({success_rate*100:.1f}%), increasing learning rate"
        elif success_rate > 0.8:
            # Good performance - decrease exploration
            self.current_learning_rate *= 0.8
            return True, f"Good success rate ({success_rate*100:.1f}%), decreasing learning rate"
        
        # Adapt based on curiosity level
        curiosity_level = agent.getCuriosityLevel()
        if curiosity_level > 0.7:
            # High curiosity - try different strategy
            return self.select_alternative_strategy(agent, world, metrics)
        
        return False, f"Performance stable (success: {success_rate*100:.1f}%)"
    
    def select_alternative_strategy(self, agent, world, metrics):
        """Select alternative learning strategy."""
        # Use softmax selection based on weights
        strategies = list(self.strategies.keys())
        weights = list(self.strategy_selection_weights.values())
        
        # Calculate softmax probabilities
        import math
        exp_weights = [math.exp(w) for w in weights]
        total_exp = sum(exp_weights)
        probabilities = [w / total_exp for w in exp_weights]
        
        # Select strategy based on probabilities
        import random
        selected_strategy = random.choices(strategies, weights=probabilities)[0]
        
        # Execute selected strategy
        strategy_func = self.strategies[selected_strategy]
        should_adapt, reason = strategy_func(agent, world, metrics)
        
        if should_adapt:
            # Update strategy weights
            for strategy_name in strategies:
                if strategy_name == selected_strategy:
                    self.strategy_selection_weights[strategy_name] *= 1.2
                else:
                    self.strategy_selection_weights[strategy_name] *= 0.9
            
            # Renormalize weights
            total_weight = sum(self.strategy_selection_weights.values())
            for strategy_name in self.strategy_selection_weights:
                self.strategy_selection_weights[strategy_name] /= total_weight
            
            return True, f"Selected strategy '{selected_strategy}': {reason}"
        
        return False, f"Default strategy: {reason}"
    
    def run_meta_learning_episode(self, agent, world, episode_number, use_meta_learning=True):
        """Run a single meta-learning episode."""
        # Reset agent
        agent.reset()
        agent.initialize(world)
        
        # Store metrics for meta-learning
        episode_metrics = []
        
        for step in range(50):
            world.update(0.1)
            
            # Use meta-learning to decide whether to adapt
            adapt_decision, adapt_reason = False, ""
            
            if use_meta_learning:
                adapt_decision, adapt_reason = self.adapt_learning_strategy(agent, world, episode_metrics)
            
            # Get action based on current strategy
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get metrics
            metrics = agent.getPerformanceMetrics()
            metrics['adaptation_decision'] = adapt_decision
            metrics['adaptation_reason'] = adapt_reason
            episode_metrics.append(metrics)
            
            # Store for meta-learning analysis
            self.meta_learning_history.append({
                'episode': episode_number,
                'step': step,
                'adaptation_decision': adapt_decision,
                'adaptation_reason': adapt_reason,
                'metrics': metrics,
                'learning_rate': self.current_learning_rate
            })
        
        return episode_metrics
    
    def analyze_meta_learning_performance(self):
        """Analyze meta-learning performance."""
        if not self.meta_learning_history:
            return {}
        
        # Analyze adaptation effectiveness
        adaptation_events = sum(1 for entry in self.meta_learning_history 
                             if entry['adaptation_decision'])
        total_events = len(self.meta_learning_history)
        
        adaptation_rate = adaptation_events / total_events if total_events > 0 else 0
        
        # Analyze strategy selection
        strategy_selections = {}
        for entry in self.meta_learning_history:
            if entry['adaptation_decision']:
                reason = entry['adaptation_reason']
                strategy = reason.split("'")[1] if "'" in reason else "unknown"
                strategy_selections[strategy] = strategy_selections.get(strategy, 0) + 1
        
        # Calculate average learning rate
        avg_learning_rate = sum(entry['learning_rate'] for entry in self.meta_learning_history) / total_events
        
        return {
            'adaptation_rate': adaptation_rate,
            'strategy_selections': strategy_selections,
            'avg_learning_rate': avg_learning_rate,
            'total_adaptation_events': adaptation_events,
            'total_steps_analyzed': total_events
        }

# Usage example
print("=== Creating Meta-Learning Agent ===\n")

meta_agent = MetaLearningAgent()

try:
    # Create environment
    with nlm.Config() as config:
        config.setDefaults("development")
        
        with nlm.SimpleWorld() as world:
            world.createGridWorld(50.0, 50.0, 0.1)
            world.setAgentGoal(40.0, 40.0)
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    meta_agent.enableCuriosity(True)
                    meta_agent.enableRewardModulation(True)
                    
                    # Run meta-learning episodes
                    print("Running meta-learning episodes...")
                    all_episodes = []
                    
                    for episode in range(5):
                        print(f"\nEpisode {episode + 1}:")
                        
                        episode_metrics = meta_agent.run_meta_learning_episode(
                            agent, world, episode, use_meta_learning=True
                        )
                        all_episodes.extend(episode_metrics)
                        
                        print(f"  Completed: {episode_metrics[-1]['success_rate']*100:.1f}% success")
                    
                    # Analyze meta-learning performance
                    analysis = meta_agent.analyze_meta_learning_performance()
                    
                    print(f"\n=== Meta-Learning Analysis ===")
                    print(f"Adaptation rate: {analysis['adaptation_rate']*100:.1f}%")
                    print(f"Strategy selections: {analysis['strategy_selections']}")
                    print(f"Average learning rate: {analysis['avg_learning_rate']:.4f}")
                    print(f"Total adaptation events: {analysis['total_adaptation_events']}")
                    print(f"Total steps analyzed: {analysis['total_steps_analyzed']}")
    
    print("\n🎉 Meta-learning agent created successfully!")
    
except Exception as e:
    print(f"\n❌ Failed to create meta-learning agent: {e}")
    import traceback
    traceback.print_exc()
```

### Transfer Learning Across NLM Architectures

```python
import pynlm as nlm

print("\n=== Transfer Learning Across NLM Architectures ===\n")

class TransferLearningSystem:
    """System for transfer learning across different NLM architectures."""
    
    def __init__(self):
        self.source_agents = []
        self.target_agents = []
        self.transfer_strategies = {
            'parameter_sharing': self.parameter_sharing_transfer,
            'feature_extraction': self.feature_extraction_transfer,
            'knowledge_distillation': self.knowledge_distillation_transfer,
            'adapter_modules': self.adapter_modules_transfer
        }
    
    def parameter_sharing_transfer(self, source_agent, target_agent, transfer_ratio=0.5):
        """Transfer knowledge by sharing parameters between agents."""
        print(f"  Implementing parameter sharing transfer ({transfer_ratio*100:.0f}% source influence)...")
        
        # Share neural weights
        source_stats = source_agent.getPerformanceMetrics()
        target_stats = target_agent.getPerformanceMetrics()
        
        # Calculate weight transfer ratio
        weight_transfer = transfer_ratio * source_stats['weight_importance']
        
        # In a real implementation, this would actually transfer neural weights
        # For demo, we'll simulate the effect
        target_stats['transferred_weight_importance'] = weight_transfer
        target_stats['source_performance_influence'] = source_stats['success_rate'] * transfer_ratio
        
        return target_stats
    
    def feature_extraction_transfer(self, source_agent, target_agent, feature_dimensions=50):
        """Transfer feature extractors from source to target."""
        print(f"  Implementing feature extraction transfer ({feature_dimensions} features)...")
        
        # Extract features from source agent's experience
        source_features = self.extract_agent_features(source_agent)
        
        # Apply features to target agent
        target_features = self.apply_features_to_target(target_agent, source_features, feature_dimensions)
        
        return target_features
    
    def knowledge_distillation_transfer(self, teacher_agent, student_agent, temperature=2.0):
        """Transfer knowledge through distillation."""
        print(f"  Implementing knowledge distillation (temperature={temperature})...")
        
        # Get teacher's soft predictions
        teacher_metrics = teacher_agent.getPerformanceMetrics()
        teacher_probabilities = self.get_soft_predictions(teacher_metrics)
        
        # Get student's predictions
        student_metrics = student_agent.getPerformanceMetrics()
        student_probabilities = self.get_soft_predictions(student_metrics)
        
        # Calculate distillation loss
        distillation_loss = self.calculate_kl_divergence(
            teacher_probabilities, student_probabilities, temperature
        )
        
        # Update student based on distillation
        student_metrics['distillation_loss'] = distillation_loss
        student_metrics['knowledge_transfer_efficiency'] = (
            1.0 - distillation_loss
        )
        
        return student_metrics
    
    def adapter_modules_transfer(self, source_agent, target_agent, adapter_layers=3):
        """Transfer using adapter modules (efficient fine-tuning)."""
        print(f"  Implementing adapter modules transfer ({adapter_layers} layers)...")
        
        # Create adapter modules for target agent
        adapters = []
        for layer in range(adapter_layers):
            adapter = self.create_adapter_layer(layer, source_agent, target_agent)
            adapters.append(adapter)
        
        # Apply adapters to target agent
        adapted_metrics = self.apply_adapters(target_agent, adapters)
        
        return adapted_metrics
    
    def extract_agent_features(self, agent):
        """Extract features from agent experience."""
        features = {
            'neural_activity_patterns': [],
            'decision_making_strategies': [],
            'environmental_adaptability': [],
            'learning_efficiency': [],
            'behavioral_consistency': []
        }
        
        # Simulate feature extraction
        # In real implementation, this would analyze agent's neural activity,
        # decision patterns, and behavioral responses
        
        return features
    
    def apply_features_to_target(self, target_agent, source_features, feature_dimensions):
        """Apply extracted features to target agent."""
        target_features = {
            f'transferred_feature_{i}': source_features['neural_activity_patterns'][i % len(source_features['neural_activity_patterns'])]
            for i in range(feature_dimensions)
        }
        
        return target_features
    
    def get_soft_predictions(self, metrics):
        """Get soft predictions from agent metrics."""
        # Convert metrics to probability distribution
        num_classes = 5  # Example: success, failure, exploration, exploitation, learning
        soft_predictions = [0.2] * num_classes  # Initialize with uniform distribution
        
        # Adjust based on metrics
        success_rate = metrics['success_rate']
        curiosity_level = metrics.get('curiosity_level', 0.5)
        reward_magnitude = metrics.get('total_reward', 0.0) / 50.0  # Normalize
        
        # Update probabilities
        soft_predictions[0] = success_rate * 0.4  # Success
        soft_predictions[1] = (1 - success_rate) * 0.3  # Failure
        soft_predictions[2] = curiosity_level * 0.2  # Exploration
        soft_predictions[3] = (1 - curiosity_level) * 0.1  # Exploitation
        soft_predictions[4] = min(reward_magnitude, 1.0) * 0.1  # Learning
        
        # Normalize to sum to 1
        total = sum(soft_predictions)
        soft_predictions = [p / total for p in soft_predictions]
        
        return soft_predictions
    
    def calculate_kl_divergence(self, p, q, temperature=1.0):
        """Calculate KL divergence between two probability distributions."""
        import math
        
        # Apply temperature
        p_temp = [math.pow(pi, 1/temperature) for pi in p]
        q_temp = [math.pow(qi, 1/temperature) for qi in q]
        
        # Normalize
        p_temp = [pi / sum(p_temp) for pi in p_temp]
        q_temp = [qi / sum(q_temp) for qi in q_temp]
        
        # Calculate KL divergence
        kl_divergence = 0.0
        for pi, qi in zip(p_temp, q_temp):
            if pi > 0 and qi > 0:
                kl_divergence += pi * math.log(pi / qi)
        
        return kl_divergence
    
    def create_adapter_layer(self, layer_num, source_agent, target_agent):
        """Create adapter layer for transfer."""
        # Create a simple adapter layer
        adapter = {
            'layer_id': layer_num,
            'connection_strength': 0.5 + (layer_num * 0.1),
            'adaptation_rate': 0.1 * (layer_num + 1),
            'source_influence': 1.0 - (layer_num * 0.1)
        }
        
        return adapter
    
    def apply_adapters(self, target_agent, adapters):
        """Apply adapters to target agent."""
        adapted_metrics = target_agent.getPerformanceMetrics()
        
        # Calculate overall adaptation effect
        total_adaptation = sum(
            adapter['connection_strength'] * adapter['source_influence']
            for adapter in adapters
        )
        
        adapted_metrics['adapter_effectiveness'] = total_adaptation / len(adapters)
        adapted_metrics['adaptation_layers'] = len(adapters)
        
        return adapted_metrics
    
    def create_transfer_experiment(self, source_config, target_config, transfer_strategy='parameter_sharing'):
        """Create transfer learning experiment."""
        print(f"\nCreating transfer learning experiment...")
        print(f"  Source config: {source_config}")
        print(f"  Target config: {target_config}")
        print(f"  Transfer strategy: {transfer_strategy}")
        
        # Get transfer strategy function
        transfer_func = self.transfer_strategies.get(transfer_strategy)
        if not transfer_func:
            raise ValueError(f"Unknown transfer strategy: {transfer_strategy}")
        
        # Create source agent
        with nlm.Brain(source_config) as source_brain:
            source_brain.initialize()
            
            with nlm.SimpleWorld() as source_world:
                source_world.configure(60.0, 60.0, 8, 8)
                source_world.setAgentGoal(50.0, 50.0)
                
                with nlm.AgentBrain(source_brain) as source_agent:
                    source_agent.initialize(source_world)
                    source_agent.enableCuriosity(True)
                    
                    # Train source agent
                    self.train_agent_for_transfer(source_agent, source_world, 20)
                    
                    # Store source agent
                    self.source_agents.append(source_agent)
                    
                    # Create target agent
                    with nlm.Brain(target_config) as target_brain:
                        target_brain.initialize()
                        
                        with nlm.SimpleWorld() as target_world:
                            target_world.configure(40.0, 40.0, 6, 6)
                            target_world.setAgentGoal(30.0, 30.0)
                            
                            with nlm.AgentBrain(target_brain) as target_agent:
                                target_agent.initialize(target_world)
                                
                                # Train target agent without transfer (baseline)
                                self.train_agent_for_transfer(target_agent, target_world, 20)
                                
                                # Store target agent
                                self.target_agents.append(target_agent)
                                
                                # Apply transfer learning
                                transfer_results = transfer_func(source_agent, target_agent)
                                
                                # Continue training with transfer
                                self.continue_training_with_transfer(target_agent, target_world, 20)
                                
                                return transfer_results
    
    def train_agent_for_transfer(self, agent, world, episodes):
        """Train agent for transfer learning experiment."""
        for episode in range(episodes):
            for step in range(30):
                world.update(0.1)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
    
    def continue_training_with_transfer(self, agent, world, episodes):
        """Continue training after transfer learning."""
        for episode in range(episodes):
            for step in range(30):
                world.update(0.1)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())

# Usage example
print("=== Creating Transfer Learning System ===\n")

transfer_system = TransferLearningSystem()

try:
    # Define source and target configurations
    source_config = nlm.Config()
    source_config.setDefaults("high_performance")
    source_config.updateFromDict({
        "brain.neuron_count": 1000,
        "agent.exploration_rate": 0.5
    })
    
    target_config = nlm.Config()
    target_config.setDefaults("quick_test")
    target_config.updateFromDict({
        "brain.neuron_count": 200,
        "agent.exploration_rate": 0.2
    })
    
    # Run transfer learning experiment
    results = transfer_system.create_transfer_experiment(
        source_config, target_config, 'parameter_sharing'
    )
    
    print(f"\n🎉 Transfer learning experiment completed!")
    print(f"Transfer results: {results}")
    
except Exception as e:
    print(f"\n❌ Failed to create transfer learning system: {e}")
    import traceback
    traceback.print_exc()
```

---

## Part 2: Adaptive and Self-Modifying Agents

This section covers agents that can modify themselves based on their experiences and environment.

### Self-Modifying NLM Agents

```python
import pynlm as nlm
import time

print("\n=== Self-Modifying NLM Agents ===\n")

class SelfModifyingAgent:
    """Agent that can modify its own architecture and parameters."""
    
    def __init__(self, initial_architecture="standard"):
        self.architecture = initial_architecture
        self.modification_history = []
        self.performance_history = []
        self.current_parameters = {}
        self.adaptation_triggers = {}
        self.self_modification_strategies = {}
        
        # Initialize with default parameters
        self.initialize_parameters()
        
        # Define self-modification strategies
        self.define_modification_strategies()
        
        # Initialize adaptation triggers
        self.initialize_adaptation_triggers()
    
    def initialize_parameters(self):
        """Initialize agent parameters."""
        self.current_parameters = {
            'neuron_count': 500,
            'connection_probability': 0.2,
            'learning_rate': 0.01,
            'curiosity_threshold': 0.5,
            'exploration_rate': 0.3,
            'memory_capacity': 100,
            'plasticity_rate': 0.01
        }
        
        print("✓ Parameters initialized:")
        for key, value in self.current_parameters.items():
            print(f"  {key}: {value}")
    
    def define_modification_strategies(self):
        """Define self-modification strategies."""
        self.self_modification_strategies = {
            'neuron_growth': self.neuron_growth_modification,
            'synaptic_pruning': self.synaptic_pruning_modification,
            'learning_rate_adaptation': self.learning_rate_adaptation_modification,
            'architecture_reconfiguration': self.architecture_reconfiguration_modification,
            'memory_expansion': self.memory_expansion_modification
        }
        
        print("✓ Modification strategies defined:")
        for strategy_name in self.self_modification_strategies:
            print(f"  {strategy_name}")
    
    def initialize_adaptation_triggers(self):
        """Initialize adaptation triggers."""
        self.adaptation_triggers = {
            'performance_decline': 0.3,  # Trigger if performance drops below 30%
            'learning_stagnation': 20,   # Trigger after 20 steps with no improvement
            'environment_change': True,   # Trigger on environment changes
            'resource_constraints': False, # Trigger when resources are limited
            'external_pressure': False    # Trigger from external factors
        }
        
        print("✓ Adaptation triggers initialized:")
        for trigger, threshold in self.adaptation_triggers.items():
            print(f"  {trigger}: {threshold}")
    
    def neuron_growth_modification(self):
        """Modify agent by growing more neurons."""
        print("  Implementing neuron growth modification...")
        
        current_neuron_count = self.current_parameters['neuron_count']
        growth_factor = 1.5  # Grow by 50%
        
        new_neuron_count = int(current_neuron_count * growth_factor)
        
        # Apply modification
        self.current_parameters['neuron_count'] = new_neuron_count
        
        # Record modification
        modification_record = {
            'strategy': 'neuron_growth',
            'timestamp': time.time(),
            'previous_value': current_neuron_count,
            'new_value': new_neuron_count,
            'growth_factor': growth_factor
        }
        
        self.modification_history.append(modification_record)
        
        print(f"    Neuron count: {current_neuron_count} → {new_neuron_count}")
        print(f"    Growth factor: {growth_factor}")
        
        return modification_record
    
    def synaptic_pruning_modification(self):
        """Modify agent by pruning synapses."""
        print("  Implementing synaptic pruning modification...")
        
        current_pruning = self.current_parameters['connection_probability']
        pruning_factor = 0.7  # Reduce connectivity by 30%
        
        new_pruning = current_pruning * pruning_factor
        
        # Apply modification
        self.current_parameters['connection_probability'] = new_pruning
        
        # Record modification
        modification_record = {
            'strategy': 'synaptic_pruning',
            'timestamp': time.time(),
            'previous_value': current_pruning,
            'new_value': new_pruning,
            'pruning_factor': pruning_factor
        }
        
        self.modification_history.append(modification_record)
        
        print(f"    Connection probability: {current_pruning:.3f} → {new_pruning:.3f}")
        print(f"    Pruning factor: {pruning_factor}")
        
        return modification_record
    
    def learning_rate_adaptation_modification(self):
        """Modify agent's learning rate."""
        print("  Implementing learning rate adaptation modification...")
        
        current_learning_rate = self.current_parameters['learning_rate']
        
        # Adapt based on performance
        performance = self.calculate_current_performance()
        
        if performance < 0.3:
            # Poor performance - increase learning rate
            new_learning_rate = current_learning_rate * 2.0
            reason = "Poor performance detected, increasing learning rate"
        elif performance > 0.8:
            # Good performance - decrease learning rate
            new_learning_rate = current_learning_rate * 0.5
            reason = "Good performance detected, decreasing learning rate"
        else:
            # Moderate performance - maintain current rate
            new_learning_rate = current_learning_rate
            reason = "Moderate performance, maintaining learning rate"
        
        # Apply modification
        self.current_parameters['learning_rate'] = new_learning_rate
        
        # Record modification
        modification_record = {
            'strategy': 'learning_rate_adaptation',
            'timestamp': time.time(),
            'previous_value': current_learning_rate,
            'new_value': new_learning_rate,
            'reason': reason,
            'performance': performance
        }
        
        self.modification_history.append(modification_record)
        
        print(f"    Learning rate: {current_learning_rate:.4f} → {new_learning_rate:.4f}")
        print(f"    Reason: {reason}")
        
        return modification_record
    
    def architecture_reconfiguration_modification(self):
        """Modify agent's architecture."""
        print("  Implementing architecture reconfiguration modification...")
        
        current_architecture = self.architecture
        
        # Determine new architecture
        if current_architecture == 'standard':
            new_architecture = 'deep'
            change_description = "Upgrading from standard to deep architecture"
        elif current_architecture == 'deep':
            new_architecture = 'wide'
            change_description = "Upgrading from deep to wide architecture"
        elif current_architecture == 'wide':
            new_architecture = 'modular'
            change_description = "Upgrading from wide to modular architecture"
        else:
            new_architecture = 'standard'
            change_description = "Resetting to standard architecture"
        
        # Apply modification
        self.architecture = new_architecture
        
        # Adjust parameters based on new architecture
        if new_architecture == 'deep':
            self.current_parameters['neuron_count'] *= 2
            self.current_parameters['connection_probability'] *= 0.8
        elif new_architecture == 'wide':
            self.current_parameters['neuron_count'] *= 0.8
            self.current_parameters['connection_probability'] *= 1.2
        elif new_architecture == 'modular':
            self.current_parameters['neuron_count'] *= 1.5
            self.current_parameters['connection_probability'] *= 0.9
            self.current_parameters['memory_capacity'] *= 2
        
        # Record modification
        modification_record = {
            'strategy': 'architecture_reconfiguration',
            'timestamp': time.time(),
            'previous_value': current_architecture,
            'new_value': new_architecture,
            'change_description': change_description,
            'parameter_changes': {
                'neuron_count': self.current_parameters['neuron_count'],
                'connection_probability': self.current_parameters['connection_probability'],
                'memory_capacity': self.current_parameters['memory_capacity']
            }
        }
        
        self.modification_history.append(modification_record)
        
        print(f"    Architecture: {current_architecture} → {new_architecture}")
        print(f"    Description: {change_description}")
        print(f"    Parameter changes: {modification_record['parameter_changes']}")
        
        return modification_record
    
    def memory_expansion_modification(self):
        """Modify agent's memory capacity."""
        print("  Implementing memory expansion modification...")
        
        current_memory = self.current_parameters['memory_capacity']
        expansion_factor = 2.0  # Double memory capacity
        
        new_memory = int(current_memory * expansion_factor)
        
        # Apply modification
        self.current_parameters['memory_capacity'] = new_memory
        
        # Record modification
        modification_record = {
            'strategy': 'memory_expansion',
            'timestamp': time.time(),
            'previous_value': current_memory,
            'new_value': new_memory,
            'expansion_factor': expansion_factor
        }
        
        self.modification_history.append(modification_record)
        
        print(f"    Memory capacity: {current_memory} → {new_memory}")
        print(f"    Expansion factor: {expansion_factor}")
        
        return modification_record
    
    def calculate_current_performance(self):
        """Calculate current agent performance."""
        # This would typically analyze agent's recent behavior
        # For demo, we'll return a random performance metric
        import random
        return random.uniform(0.1, 0.9)
    
    def check_adaptation_triggers(self):
        """Check if any adaptation triggers should activate."""
        triggers_activated = []
        
        # Check performance decline trigger
        if self.adaptation_triggers['performance_decline']:
            performance = self.calculate_current_performance()
            if performance < self.adaptation_triggers['performance_decline']:
                triggers_activated.append('performance_decline')
        
        # Check learning stagnation trigger
        if self.adaptation_triggers['learning_stagnation']:
            # This would check if learning has stagnated
            learning_stagnation = self.check_learning_stagnation()
            if learning_stagnation:
                triggers_activated.append('learning_stagnation')
        
        # Check environment change trigger
        if self.adaptation_triggers['environment_change']:
            environment_change = self.check_environment_change()
            if environment_change:
                triggers_activated.append('environment_change')
        
        return triggers_activated
    
    def check_learning_stagnation(self):
        """Check if learning has stagnated."""
        # This would analyze learning progress over time
        # For demo, return False
        return False
    
    def check_environment_change(self):
        """Check if environment has changed."""
        # This would compare current and previous environments
        # For demo, return False
        return False
    
    def perform_self_modification(self):
        """Perform self-modification based on adaptation triggers."""
        print("Performing self-modification analysis...")
        
        # Check for adaptation triggers
        triggers_activated = self.check_adaptation_triggers()
        
        if not triggers_activated:
            print("  No adaptation triggers activated")
            return None
        
        print(f"  Adaptation triggers activated: {triggers_activated}")
        
        # Select modification strategy based on triggers
        strategy = self.select_modification_strategy(triggers_activated)
        
        if strategy:
            print(f"  Selected modification strategy: {strategy}")
            
            # Perform the modification
            modification = self.self_modification_strategies[strategy]()
            
            print(f"  Self-modification completed")
            print(f"  Modification: {modification['strategy']}")
            print(f"  Changes: {modification}")
            
            return modification
        
        return None
    
    def select_modification_strategy(self, triggers_activated):
        """Select modification strategy based on activated triggers."""
        # Map triggers to strategies
        trigger_to_strategy = {
            'performance_decline': 'learning_rate_adaptation',
            'learning_stagnation': 'neuron_growth',
            'environment_change': 'architecture_reconfiguration',
            'resource_constraints': 'synaptic_pruning',
            'external_pressure': 'memory_expansion'
        }
        
        # Use first triggered strategy
        for trigger in triggers_activated:
            strategy = trigger_to_strategy.get(trigger)
            if strategy and strategy in self.self_modification_strategies:
                return strategy
        
        # Fallback to neuron growth if no specific strategy
        return 'neuron_growth'
    
    def run_self_modification_episode(self, world, episode_number, use_self_modification=True):
        """Run an episode with self-modification capabilities."""
        print(f"  Episode {episode_number} with self-modification...")
        
        # Initialize agent
        # In real implementation, this would use the agent's current architecture and parameters
        # For demo, we'll simulate the agent's state
        
        # Run episode
        for step in range(30):
            world.update(0.1)
            
            # Simulate agent behavior
            action = self.simulate_agent_action(world)
            
            # Record performance
            performance = self.calculate_current_performance()
            self.performance_history.append(performance)
            
            # Show progress
            if (step + 1) % 10 == 0:
                print(f"    Step {step + 1}: Performance {performance:.3f}")
        
        # Perform self-modification if requested and conditions are met
        modification = None
        if use_self_modification:
            modification = self.perform_self_modification()
        
        # Record episode summary
        episode_summary = {
            'episode': episode_number,
            'final_performance': self.performance_history[-1] if self.performance_history else None,
            'modifications': modification,
            'timestamp': time.time()
        }
        
        return episode_summary
    
    def simulate_agent_action(self, world):
        """Simulate agent action based on current state."""
        # This would use the agent's current parameters and architecture
        # For demo, return a simulated action
        import random
        return {
            'type': random.choice(['move_forward', 'turn_left', 'turn_right']),
            'parameters': [random.uniform(0, 1), random.uniform(-1, 1)],
            'timestamp': time.time()
        }
    
    def analyze_self_modification_effectiveness(self):
        """Analyze effectiveness of self-modification."""
        if not self.modification_history:
            return {}
        
        # Calculate modification statistics
        strategies_used = {}
        for modification in self.modification_history:
            strategy = modification['strategy']
            strategies_used[strategy] = strategies_used.get(strategy, 0) + 1
        
        # Calculate performance changes
        performance_changes = []
        for i in range(1, len(self.performance_history)):
            if self.performance_history[i] and self.performance_history[i-1]:
                change = self.performance_history[i] - self.performance_history[i-1]
                performance_changes.append(change)
        
        # Calculate average change
        avg_change = sum(performance_changes) / len(performance_changes) if performance_changes else 0
        
        return {
            'total_modifications': len(self.modification_history),
            'strategies_used': strategies_used,
            'average_performance_change': avg_change,
            'modifications_per_step': len(self.modification_history) / (len(self.performance_history) + 1)
        }

# Usage example
print("=== Creating Self-Modifying Agent ===\n")

self_modifying_agent = SelfModifyingAgent()

try:
    # Create environment
    with nlm.Config() as config:
        config.setDefaults("development")
        
        with nlm.SimpleWorld() as world:
            world.createGridWorld(50.0, 50.0, 0.1)
            world.setAgentGoal(40.0, 40.0)
            
            # Run self-modification episodes
            print("Running self-modification episodes...")
            all_episodes = []
            
            for episode in range(5):
                episode_summary = self_modifying_agent.run_self_modification_episode(
                    world, episode, use_self_modification=True
                )
                all_episodes.append(episode_summary)
                
                print(f"  Completed: {episode_summary['final_performance']:.3f} performance")
                if episode_summary['modifications']:
                    print(f"  Modification: {episode_summary['modifications']['strategy']}")
            
            # Analyze self-modification effectiveness
            analysis = self_modifying_agent.analyze_self_modification_effectiveness()
            
            print(f"\n=== Self-Modification Analysis ===")
            print(f"Total modifications: {analysis['total_modifications']}")
            print(f"Strategies used: {analysis['strategies_used']}")
            print(f"Average performance change: {analysis['average_performance_change']:.3f}")
            print(f"Modifications per step: {analysis['modifications_per_step']:.3f}")
    
    print("\n🎉 Self-modifying agent created successfully!")
    
except Exception as e:
    print(f"\n❌ Failed to create self-modifying agent: {e}")
    import traceback
    traceback.print_exc()
```

---

## Part 3: NLM Integration with Other AI Systems

This section covers integrating NLM with other AI systems and frameworks.

### Integration with Reinforcement Learning

```python
import pynlm as nlm

print("\n=== Integration with Reinforcement Learning ===\n")

class NLMReinforcementLearning:
    """System for integrating NLM with reinforcement learning."""
    
    def __init__(self):
        self.reinforcement_learning_history = []
        self.policy_gradient_history = []
        self.value_function_history = []
        self.integration_strategies = {
            'value_function_approximation': self.value_function_approximation_integration,
            'policy_gradient_methods': self.policy_gradient_integration,
            'actor_critic_methods': self.actor_critic_integration,
            'deep_reinforcement_learning': self.deep_reinforcement_learning_integration
        }
    
    def value_function_approximation_integration(self, agent, world, discount_factor=0.95):
        """Integrate NLM with value function approximation."""
        print("  Implementing value function approximation integration...")
        
        # Get agent's current state
        state = self.extract_state_from_world(world)
        
        # Get agent's current value estimate
        current_value = self.estimate_value(agent, state)
        
        # Calculate future expected rewards
        future_rewards = self.calculate_future_rewards(world, discount_factor)
        
        # Update value function
        updated_value = (0.9 * current_value) + (0.1 * future_rewards)
        
        # Store integration results
        integration_result = {
            'integration_method': 'value_function_approximation',
            'state': state,
            'current_value': current_value,
            'future_rewards': future_rewards,
            'updated_value': updated_value,
            'value_error': abs(updated_value - current_value)
        }
        
        self.reinforcement_learning_history.append(integration_result)
        
        return integration_result
    
    def policy_gradient_integration(self, agent, world, learning_rate=0.01):
        """Integrate NLM with policy gradient methods."""
        print("  Implementing policy gradient integration...")
        
        # Get current policy
        current_policy = self.extract_policy_from_agent(agent)
        
        # Take action and observe reward
        action = agent.decodeMotorCommand()
        reward = self.calculate_reward(agent, world, action)
        
        # Calculate policy gradient
        policy_gradient = self.calculate_policy_gradient(
            current_policy, action, reward
        )
        
        # Update policy using gradient ascent
        updated_policy = self.update_policy_with_gradient(
            current_policy, policy_gradient, learning_rate
        )
        
        # Store integration results
        integration_result = {
            'integration_method': 'policy_gradient',
            'current_policy': current_policy,
            'action': action,
            'reward': reward,
            'policy_gradient': policy_gradient,
            'updated_policy': updated_policy
        }
        
        self.policy_gradient_history.append(integration_result)
        
        return integration_result
    
    def actor_critic_integration(self, agent, world, actor_lr=0.01, critic_lr=0.001):
        """Integrate NLM with actor-critic methods."""
        print("  Implementing actor-critic integration...")
        
        # Actor component: select actions
        action = agent.decodeMotorCommand()
        
        # Critic component: evaluate state value
        state_value = self.evaluate_state_value(agent, world)
        
        # Calculate advantage
        advantage = self.calculate_advantage(action, state_value, world)
        
        # Update actor (policy) based on advantage
        actor_loss = self.calculate_actor_loss(advantage)
        updated_actor_params = self.update_actor(actor_loss, actor_lr)
        
        # Update critic (value function) based on TD error
        td_error = self.calculate_td_error(state_value, world)
        critic_loss = self.calculate_critic_loss(td_error)
        updated_critic_params = self.update_critic(critic_loss, critic_lr)
        
        # Store integration results
        integration_result = {
            'integration_method': 'actor_critic',
            'action': action,
            'state_value': state_value,
            'advantage': advantage,
            'actor_loss': actor_loss,
            'critic_loss': critic_loss,
            'td_error': td_error,
            'actor_params_update': updated_actor_params,
            'critic_params_update': updated_critic_params
        }
        
        self.reinforcement_learning_history.append(integration_result)
        
        return integration_result
    
    def deep_reinforcement_learning_integration(self, agent, world, dqn_parameters=None):
        """Integrate NLM with deep reinforcement learning (DQN)."""
        print("  Implementing deep reinforcement learning integration...")
        
        if dqn_parameters is None:
            dqn_parameters = {
                'replay_buffer_size': 1000,
                'batch_size': 32,
                'target_update_freq': 100,
                'epsilon': 0.1,
                'gamma': 0.99
            }
        
        # Extract experience from agent
        experience = self.extract_experience(agent, world)
        
        # Store in replay buffer
        self.store_experience(experience, dqn_parameters['replay_buffer_size'])
        
        # Sample mini-batch from replay buffer
        mini_batch = self.sample_mini_batch(dqn_parameters['batch_size'])
        
        # Calculate target Q-values
        target_q_values = self.calculate_target_q_values(mini_batch, dqn_parameters['gamma'])
        
        # Calculate predicted Q-values
        predicted_q_values = self.calculate_predicted_q_values(mini_batch)
        
        # Calculate DQN loss
        dqn_loss = self.calculate_dqn_loss(target_q_values, predicted_q_values)
        
        # Update DQN network
        dqn_gradients = self.calculate_dqn_gradients(dqn_loss)
        updated_network_params = self.update_dqn_network(dqn_gradients)
        
        # Select action using epsilon-greedy policy
        action = self.select_dqn_action(agent, world, dqn_parameters['epsilon'])
        
        # Store integration results
        integration_result = {
            'integration_method': 'deep_reinforcement_learning',
            'experience': experience,
            'mini_batch': mini_batch,
            'target_q_values': target_q_values,
            'predicted_q_values': predicted_q_values,
            'dqn_loss': dqn_loss,
            'dqn_gradients': dqn_gradients,
            'action': action,
            'epsilon': dqn_parameters['epsilon']
        }
        
        self.reinforcement_learning_history.append(integration_result)
        
        return integration_result
    
    def extract_state_from_world(self, world):
        """Extract state representation from world."""
        # Extract relevant features from world
        state_features = {
            'agent_position': (world.getAgentBody().x, world.getAgentBody().y),
            'agent_vision': world.getSensoryPercept(),
            'world_dimensions': (world.getWidth(), world.getHeight()),
            'resources': world.getResources(),
            'obstacles': world.getObstacles()
        }
        
        return state_features
    
    def estimate_value(self, agent, state):
        """Estimate value of given state."""
        # This would use the agent's value function network
        # For demo, return a heuristic estimate
        import random
        return random.uniform(0.0, 1.0)
    
    def calculate_future_rewards(self, world, discount_factor):
        """Calculate expected future rewards."""
        # This would calculate discounted sum of future rewards
        # For demo, return a random value
        import random
        return random.uniform(0.0, 1.0) * discount_factor
    
    def extract_policy_from_agent(self, agent):
        """Extract policy from agent."""
        # Extract policy from agent's action selection
        policy = {
            'action_probabilities': {},
            'value_estimates': {}
        }
        
        # Simulate policy extraction
        import random
        policy['action_probabilities'] = {
            'move_forward': random.random(),
            'turn_left': random.random(),
            'turn_right': random.random(),
            'stay': random.random()
        }
        
        # Normalize probabilities
        total_prob = sum(policy['action_probabilities'].values())
        policy['action_probabilities'] = {
            k: v / total_prob for k, v in policy['action_probabilities'].items()
        }
        
        return policy
    
    def calculate_reward(self, agent, world, action):
        """Calculate reward for given action."""
        # This would calculate reward based on action consequences
        # For demo, return a heuristic reward
        import random
        return random.uniform(-1.0, 1.0)
    
    def calculate_policy_gradient(self, policy, action, reward):
        """Calculate policy gradient."""
        # This would calculate gradient of policy with respect to action probabilities
        # For demo, return a random gradient
        import random
        return random.uniform(-0.1, 0.1)
    
    def update_policy_with_gradient(self, policy, gradient, learning_rate):
        """Update policy using gradient."""
        # Update policy parameters using gradient ascent
        updated_policy = policy.copy()
        
        # Apply gradient update
        for action in policy['action_probabilities']:
            updated_policy['action_probabilities'][action] += learning_rate * gradient
        
        return updated_policy
    
    def evaluate_state_value(self, agent, world):
        """Evaluate state value using critic."""
        # This would use the critic network to evaluate state
        # For demo, return a heuristic value
        import random
        return random.uniform(0.0, 1.0)
    
    def calculate_advantage(self, action, state_value, world):
        """Calculate advantage of action."""
        # Calculate advantage as difference between expected and actual reward
        expected_reward = state_value
        actual_reward = self.calculate_reward(None, world, action)
        
        return actual_reward - expected_reward
    
    def calculate_actor_loss(self, advantage):
        """Calculate actor loss."""
        # This would calculate the policy gradient loss
        return -advantage  # Negative because we maximize policy gradient
    
    def update_actor(self, actor_loss, learning_rate):
        """Update actor parameters."""
        # Update actor network parameters
        return {
            'policy_parameters': actor_loss * learning_rate,
            'learning_rate': learning_rate
        }
    
    def calculate_td_error(self, state_value, world):
        """Calculate temporal difference error."""
        # Calculate TD error for value function update
        import random
        return random.uniform(-1.0, 1.0)
    
    def calculate_critic_loss(self, td_error):
        """Calculate critic loss."""
        # This would calculate the TD error loss
        return td_error ** 2
    
    def update_critic(self, critic_loss, learning_rate):
        """Update critic parameters."""
        # Update critic network parameters
        return {
            'value_function_parameters': critic_loss * learning_rate,
            'learning_rate': learning_rate
        }
    
    def extract_experience(self, agent, world):
        """Extract experience tuple."""
        return {
            'state': self.extract_state_from_world(world),
            'action': agent.decodeMotorCommand(),
            'reward': self.calculate_reward(agent, world, None),
            'next_state': self.extract_state_from_world(world),
            'done': False
        }
    
    def store_experience(self, experience, replay_buffer_size):
        """Store experience in replay buffer."""
        # Store experience in replay buffer
        pass
    
    def sample_mini_batch(self, batch_size):
        """Sample mini-batch from replay buffer."""
        return [self.extract_experience(None, None) for _ in range(batch_size)]
    
    def calculate_target_q_values(self, mini_batch, discount_factor):
        """Calculate target Q-values."""
        # This would calculate target Q-values using Bellman equation
        return [0.5 for _ in mini_batch]  # Dummy values
    
    def calculate_predicted_q_values(self, mini_batch):
        """Calculate predicted Q-values."""
        # This would calculate predicted Q-values using current network
        return [0.5 for _ in mini_batch]  # Dummy values
    
    def calculate_dqn_loss(self, target_q_values, predicted_q_values):
        """Calculate DQN loss."""
        # Calculate mean squared error between target and predicted Q-values
        return sum((t - p) ** 2 for t, p in zip(target_q_values, predicted_q_values)) / len(target_q_values)
    
    def calculate_dqn_gradients(self, dqn_loss):
        """Calculate DQN gradients."""
        # Calculate gradients of loss with respect to network parameters
        return {'network_parameters': dqn_loss * 0.01}
    
    def update_dqn_network(self, gradients):
        """Update DQN network parameters."""
        # Update network parameters using gradients
        return {
            'network_parameters': gradients['network_parameters'],
            'learning_rate': 0.01
        }
    
    def select_dqn_action(self, agent, world, epsilon):
        """Select action using epsilon-greedy policy."""
        # This would use the DQN network to select actions
        # For demo, use epsilon-greedy based on agent's current policy
        import random
        if random.random() < epsilon:
            return {'type': 'random', 'parameters': []}
        else:
            return agent.decodeMotorCommand()

# Usage example
print("=== Creating NLM Reinforcement Learning Integration ===\n")

nlm_rl = NLMReinforcementLearning()

try:
    # Create environment
    with nlm.Config() as config:
        config.setDefaults("development")
        
        with nlm.SimpleWorld() as world:
            world.createGridWorld(50.0, 50.0, 0.1)
            world.setAgentGoal(40.0, 40.0)
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    agent.enableCuriosity(True)
                    agent.enableRewardModulation(True)
                    
                    # Run reinforcement learning integration
                    print("Running reinforcement learning integration...")
                    
                    # Test different integration methods
                    print("\n1. Value Function Approximation:")
                    value_result = nlm_rl.value_function_approximation_integration(agent, world)
                    
                    print("\n2. Policy Gradient:")
                    policy_result = nlm_rl.policy_gradient_integration(agent, world)
                    
                    print("\n3. Actor-Critic:")
                    actor_critic_result = nlm_rl.actor_critic_integration(agent, world)
                    
                    print("\n4. Deep Reinforcement Learning:")
                    dqn_result = nlm_rl.deep_reinforcement_learning_integration(agent, world)
                    
                    # Analyze integration results
                    print(f"\n=== Integration Analysis ===")
                    print(f"Total integration events: {len(nlm_rl.reinforcement_learning_history)}")
                    print(f"Value function integrations: {len(nlm_rl.reinforcement_learning_history)}")
                    print(f"Policy gradient integrations: {len(nlm_rl.policy_gradient_history)}")
                    
                    # Calculate average metrics
                    if nlm_rl.reinforcement_learning_history:
                        avg_value_error = sum(
                            r['value_error'] for r in nlm_rl.reinforcement_learning_history 
                            if 'value_error' in r
                        ) / len(nlm_rl.reinforcement_learning_history)
                        print(f"Average value function error: {avg_value_error:.3f}")
    
    print("\n🎉 NLM reinforcement learning integration created successfully!")
    
except Exception as e:
    print(f"\n❌ Failed to create NLM reinforcement learning integration: {e}")
    import traceback
    traceback.print_exc()
```

---

## Part 4: Scaling NLM Systems for Research and Production

This section covers scaling NLM systems for research applications and production deployment.

### Large-Scale NLM System Architecture

```python
import pynlm as nlm

print("\n=== Large-Scale NLM System Architecture ===\n")

class LargeScaleNLMSystem:
    """Large-scale NLM system for research and production."""
    
    def __init__(self, num_agents=10, system_scale="research"):
        self.num_agents = num_agents
        self.system_scale = system_scale
        self.agents = []
        self.shared_resources = {}
        self.distributed_system = {}
        self.performance_metrics = {}
        self.resource_allocation = {}
        
        # Initialize based on scale
        if system_scale == "research":
            self.initialize_research_scale()
        elif system_scale == "production":
            self.initialize_production_scale()
        else:
            self.initialize_standalone_scale()
    
    def initialize_research_scale(self):
        """Initialize research-scale system."""
        print("Initializing research-scale NLM system...")
        
        self.config = {
            'parallel_execution': True,
            'shared_memory': True,
            'load_balancing': True,
            'fault_tolerance': True,
            'experiment_tracking': True,
            'reproducibility': True
        }
        
        self.hardware_requirements = {
            'cpu_cores': self.num_agents * 2,
            'memory_gb': self.num_agents * 4,
            'gpu_enabled': False,
            'distributed_setup': True,
            'network_bandwidth': 'high'
        }
        
        print("✓ Research-scale configuration:")
        for key, value in self.config.items():
            print(f"  {key}: {value}")
        
        print("✓ Hardware requirements:")
        for key, value in self.hardware_requirements.items():
            print(f"  {key}: {value}")
    
    def initialize_production_scale(self):
        """Initialize production-scale system."""
        print("Initializing production-scale NLM system...")
        
        self.config = {
            'parallel_execution': True,
            'shared_memory': True,
            'load_balancing': True,
            'fault_tolerance': True,
            'auto_scaling': True,
            'monitoring': True,
            'quality_assurance': True
        }
        
        self.hardware_requirements = {
            'cpu_cores': self.num_agents * 4,
            'memory_gb': self.num_agents * 8,
            'gpu_enabled': True,
            'distributed_setup': True,
            'high_availability': True,
            'network_bandwidth': 'very_high'
        }
        
        print("✓ Production-scale configuration:")
        for key, value in self.config.items():
            print(f"  {key}: {value}")
        
        print("✓ Hardware requirements:")
        for key, value in self.hardware_requirements.items():
            print(f"  {key}: {value}")
    
    def initialize_standalone_scale(self):
        """Initialize standalone-scale system."""
        print("Initializing standalone NLM system...")
        
        self.config = {
            'parallel_execution': False,
            'shared_memory': True,
            'load_balancing': False,
            'fault_tolerance': False,
            'experiment_tracking': False
        }
        
        self.hardware_requirements = {
            'cpu_cores': self.num_agents,
            'memory_gb': self.num_agents * 2,
            'gpu_enabled': False,
            'distributed_setup': False,
            'network_bandwidth': 'low'
        }
        
        print("✓ Standalone configuration:")
        for key, value in self.config.items():
            print(f"  {key}: {value}")
        
        print("✓ Hardware requirements:")
        for key, value in self.hardware_requirements.items():
            print(f"  {key}: {value}")
    
    def create_distributed_agents(self):
        """Create distributed agents for parallel execution."""
        print(f"Creating {self.num_agents} distributed agents...")
        
        for agent_id in range(self.num_agents):
            # Create agent configuration
            config = nlm.Config()
            config.setDefaults("development")
            
            # Agent-specific configuration
            config.updateFromDict({
                'agent.id': agent_id,
                'agent.exploration_rate': 0.1 + (agent_id * 0.05),
                'agent.learning_rate': 0.01 * (agent_id + 1) / self.num_agents,
                'agent.neuron_count': 500 + (agent_id * 100),
                'agent.capacity': 100 + (agent_id * 20)
            })
            
            # Create agent
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.SimpleWorld() as world:
                    # Agent-specific world configuration
                    world_width = 60.0 + (agent_id * 10)
                    world_height = 60.0 + (agent_id * 10)
                    
                    world.createGridWorld(world_width, world_height, 0.1)
                    world.setAgentStart(10.0, 10.0)
                    world.setAgentGoal(world_width - 10.0, world_height - 10.0)
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        agent.enableRewardModulation(True)
                        
                        self.agents.append({
                            'agent_id': agent_id,
                            'agent': agent,
                            'brain': brain,
                            'world': world,
                            'config': config,
                            'status': 'initialized'
                        })
                        
                        print(f"  Agent {agent_id} created: {agent_id} neurons, {agent_id * 20} capacity")
        
        return self.agents
    
    def allocate_system_resources(self):
        """Allocate system resources for distributed execution."""
        print("Allocating system resources...")
        
        # Calculate total resource requirements
        total_cpu = self.hardware_requirements['cpu_cores']
        total_memory = self.hardware_requirements['memory_gb']
        
        # Allocate resources based on scale
        if self.system_scale == "production":
            # Production-scale resource allocation
            self.resource_allocation = {
                'cpu_allocation': {
                    'total_cores': total_cpu,
                    'cores_per_agent': self.num_agents,
                    'overhead': total_cpu * 0.1,
                    'available_cores': total_cpu * 0.9
                },
                'memory_allocation': {
                    'total_memory_gb': total_memory,
                    'memory_per_agent': self.num_agents * 2,
                    'overhead': total_memory * 0.2,
                    'available_memory_gb': total_memory * 0.8
                },
                'gpu_allocation': {
                    'enabled': self.hardware_requirements['gpu_enabled'],
                    'gpu_count': min(2, self.num_agents // 5) if self.hardware_requirements['gpu_enabled'] else 0,
                    'memory_per_gpu_gb': 16
                },
                'network_allocation': {
                    'bandwidth': self.hardware_requirements['network_bandwidth'],
                    'latency_target_ms': 10 if self.hardware_requirements['high_availability'] else 50
                }
            }
        
        print("✓ System resource allocation:")
        for category, allocation in self.resource_allocation.items():
            print(f"  {category}:")
            for key, value in allocation.items():
                if isinstance(value, dict):
                    print(f"    {key}:")
                    for k, v in value.items():
                        print(f"      {k}: {v}")
                else:
                    print(f"    {key}: {value}")
        
        return self.resource_allocation
    
    def run_parallel_simulation(self, num_episodes=5, checkpoint_interval=10):
        """Run parallel simulation across all agents."""
        print(f"Running parallel simulation with {num_episodes} episodes...")
        
        # Initialize performance tracking
        episode_results = []
        system_metrics = {
            'total_episodes': num_episodes,
            'successful_episodes': 0,
            'failed_episodes': 0,
            'average_performance': [],
            'checkpoint_saves': 0,
            'errors': []
        }
        
        for episode in range(num_episodes):
            print(f"\nEpisode {episode + 1}/{num_episodes}:")
            
            episode_metrics = {
                'episode': episode,
                'agent_results': {},
                'system_performance': {}
            }
            
            # Run each agent in parallel
            for agent_info in self.agents:
                agent_id = agent_info['agent_id']
                agent = agent_info['agent']
                world = agent_info['world']
                
                try:
                    print(f"  Agent {agent_id}...")
                    
                    # Reset agent for new episode
                    agent.reset()
                    agent.initialize(world)
                    
                    # Run episode
                    for step in range(30):
                        world.update(0.1)
                        action = agent.decodeMotorCommand()
                        world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Get metrics
                    metrics = agent.getPerformanceMetrics()
                    episode_metrics['agent_results'][agent_id] = metrics
                    
                    # Record system performance
                    episode_metrics['system_performance'][agent_id] = {
                        'success_rate': metrics['success_rate'],
                        'total_reward': metrics['total_reward'],
                        'learning_rate': metrics.get('learning_rate', 0.01)
                    }
                    
                    print(f"    Success rate: {metrics['success_rate']*100:.1f}%")
                    print(f"    Total reward: {metrics['total_reward']:.3f}")
                    
                    # Save checkpoint if needed
                    if checkpoint_interval > 0 and (episode + 1) % checkpoint_interval == 0:
                        self.save_agent_checkpoint(agent_info, f"episode_{episode + 1}")
                        system_metrics['checkpoint_saves'] += 1
                        print(f"    Checkpoint saved")
                    
                except Exception as e:
                    error_msg = f"Agent {agent_id} failed: {e}"
                    print(f"    ❌ {error_msg}")
                    system_metrics['errors'].append(error_msg)
                    system_metrics['failed_episodes'] += 1
            
            # Calculate episode summary
            total_success = sum(
                r['system_performance'][agent_id]['success_rate']
                for agent_id, r in episode_metrics['agent_results'].items()
                if agent_id in r['system_performance']
            )
            avg_success = total_success / len(episode_metrics['agent_results']) if episode_metrics['agent_results'] else 0
            
            episode_metrics['summary'] = {
                'average_success_rate': avg_success,
                'successful_agents': system_metrics['total_episodes'] - system_metrics['failed_episodes'],
                'failed_agents': system_metrics['failed_episodes']
            }
            
            if avg_success > 0.5:
                system_metrics['successful_episodes'] += 1
            
            episode_results.append(episode_metrics)
        
        # Store system metrics
        self.performance_metrics['episodes'] = episode_results
        self.performance_metrics['system_summary'] = system_metrics
        
        return episode_results, system_metrics
    
    def save_agent_checkpoint(self, agent_info, checkpoint_name):
        """Save agent checkpoint."""
        agent_id = agent_info['agent_id']
        agent = agent_info['agent']
        brain = agent_info['brain']
        
        try:
            # In a real implementation, this would save the agent's state
            # For demo, we'll just record the checkpoint
            checkpoint_data = {
                'agent_id': agent_id,
                'checkpoint_name': checkpoint_name,
                'timestamp': time.time(),
                'status': 'success'
            }
            
            print(f"    Checkpoint {checkpoint_name} saved for agent {agent_id}")
            return checkpoint_data
            
        except Exception as e:
            print(f"    ❌ Checkpoint save failed for agent {agent_id}: {e}")
            return {'status': 'failed', 'error': str(e)}
    
    def analyze_system_performance(self, episode_results):
        """Analyze system-wide performance metrics."""
        print(f"\nAnalyzing system performance...")
        
        # Calculate overall metrics
        total_episodes = len(episode_results)
        total_successful_episodes = sum(
            1 for episode in episode_results 
            if episode['summary']['successful_agents'] >= len(self.agents) * 0.8
        )
        
        overall_success_rate = total_successful_episodes / total_episodes if total_episodes > 0 else 0
        
        # Calculate agent-level performance
        agent_performance = {}
        for agent_id in range(self.num_agents):
            agent_success_rates = []
            for episode in episode_results:
                if agent_id in episode['agent_results']:
                    metrics = episode['agent_results'][agent_id]
                    agent_success_rates.append(metrics['success_rate'])
            
            if agent_success_rates:
                agent_performance[agent_id] = {
                    'average_success_rate': sum(agent_success_rates) / len(agent_success_rates),
                    'episodes_completed': len(agent_success_rates),
                    'total_performance': sum(agent_success_rates)
                }
        
        # Calculate system scalability metrics
        scalability_metrics = self.calculate_scalability_metrics(episode_results)
        
        # Generate performance report
        performance_report = {
            'total_episodes': total_episodes,
            'overall_success_rate': overall_success_rate,
            'agent_performance': agent_performance,
            'system_errors': len(self.performance_metrics.get('system_summary', {}).get('errors', [])),
            'checkpoint_saves': self.performance_metrics.get('system_summary', {}).get('checkpoint_saves', 0),
            'scalability_metrics': scalability_metrics
        }
        
        return performance_report
    
    def calculate_scalability_metrics(self, episode_results):
        """Calculate system scalability metrics."""
        # Calculate efficiency metrics
        total_steps = sum(
            len(episode['agent_results']) * 30  # Each agent runs 30 steps per episode
            for episode in episode_results
        )
        
        total_time = len(episode_results) * 30 * 0.1  # Estimate based on step time
        
        efficiency_metrics = {
            'steps_per_second': total_steps / total_time if total_time > 0 else 0,
            'agents_per_core': self.num_agents / self.hardware_requirements['cpu_cores'] if self.hardware_requirements['cpu_cores'] > 0 else 0,
            'memory_efficiency': (self.num_agents * 2) / self.hardware_requirements['memory_gb'] if self.hardware_requirements['memory_gb'] > 0 else 0,
            'parallel_efficiency': (total_steps / self.num_agents) / (total_steps / self.hardware_requirements['cpu_cores']) if self.hardware_requirements['cpu_cores'] > 0 else 0
        }
        
        return efficiency_metrics

# Usage example
print("=== Creating Large-Scale NLM System ===\n")

# Create different scale systems
research_system = LargeScaleNLMSystem(num_agents=5, system_scale="research")
production_system = LargeScaleNLMSystem(num_agents=10, system_scale="production")

print("\n=== Research-Scale System ===")
try:
    # Create and run research-scale system
    research_agents = research_system.create_distributed_agents()
    research_system.allocate_system_resources()
    
    # Run simulation
    research_episodes, research_metrics = research_system.run_parallel_simulation(num_episodes=3)
    
    # Analyze performance
    research_report = research_system.analyze_system_performance(research_episodes)
    
    print(f"\n🎉 Research-scale system created successfully!")
    print(f"Research system metrics: {research_report['overall_success_rate']*100:.1f}% success rate")
    
except Exception as e:
    print(f"\n❌ Failed to create research-scale system: {e}")
    import traceback
    traceback.print_exc()

print("\n=== Production-Scale System ===")
try:
    # Create and run production-scale system
    production_agents = production_system.create_distributed_agents()
    production_system.allocate_system_resources()
    
    # Run simulation
    production_episodes, production_metrics = production_system.run_parallel_simulation(num_episodes=5, checkpoint_interval=5)
    
    # Analyze performance
    production_report = production_system.analyze_system_performance(production_episodes)
    
    print(f"\n🎉 Production-scale system created successfully!")
    print(f"Production system metrics: {production_report['overall_success_rate']*100:.1f}% success rate")
    print(f"Scalability: {production_report['scalability_metrics']['agents_per_core']:.1f} agents per core")
    
except Exception as e:
    print(f"\n❌ Failed to create production-scale system: {e}")
    import traceback
    traceback.print_exc()
```

---

## Part 5: Advanced Research Experiments

This section covers advanced research experiments using NLM systems.

### Neural Architecture Search with NLM

```python
import pynlm as nlm

print("\n=== Neural Architecture Search with NLM ===\n")

class NLNeuralArchitectureSearch:
    """Neural architecture search using NLM as surrogate model."""
    
    def __init__(self):
        self.architecture_space = self.generate_architecture_space()
        self.search_history = []
        self.performance_cache = {}
        self.selection_strategies = {
            'evolution': self.evolution_selection,
            'bayesian_optimization': self.bayesian_optimization_selection,
            'genetic_algorithm': self.genetic_algorithm_selection
        }
    
    def generate_architecture_space(self):
        """Generate diverse NLM architectures for search."""
        architectures = []
        
        # Define architectural variations
        neuron_counts = [100, 200, 500, 1000, 2000]
        connection_probabilities = [0.1, 0.2, 0.3, 0.5]
        exploration_rates = [0.1, 0.2, 0.3, 0.5]
        learning_rates = [0.001, 0.01, 0.05, 0.1]
        
        # Generate combinations
        for neurons in neuron_counts:
            for conn_prob in connection_probabilities:
                for expl_rate in exploration_rates:
                    for learn_rate in learning_rates:
                        # Filter out computationally expensive combinations
                        if neurons <= 500 or (neurons <= 1000 and conn_prob <= 0.3):
                            architecture = {
                                'neurons': neurons,
                                'connection_probability': conn_prob,
                                'exploration_rate': expl_rate,
                                'learning_rate': learn_rate,
                                'architecture_type': self.classify_architecture(neurons, conn_prob),
                                'computational_complexity': neurons * conn_prob * 100
                            }
                            architectures.append(architecture)
        
        print(f"Generated {len(architectures)} architectures for search")
        return architectures
    
    def classify_architecture(self, neurons, connection_probability):
        """Classify architecture type based on size and connectivity."""
        if neurons <= 200:
            return 'small'
        elif neurons <= 1000:
            if connection_probability <= 0.2:
                return 'sparse_medium'
            else:
                return 'dense_medium'
        elif connection_probability <= 0.3:
            return 'sparse_large'
        else:
            return 'dense_large'
    
    def evaluate_architecture(self, architecture, world, max_steps=100):
        """Evaluate architecture performance using NLM simulation."""
        print(f"  Evaluating architecture: {architecture['architecture_type']} "
              f"({architecture['neurons']} neurons, "
              f"{architecture['connection_probability']*100:.0f}% connectivity)")
        
        # Create architecture configuration
        config = nlm.Config()
        config.setDefaults("development")
        config.updateFromDict({
            'brain.neuron_count': architecture['neurons'],
            'brain.connection_probability': architecture['connection_probability'],
            'agent.exploration_rate': architecture['exploration_rate'],
            'agent.learning_rate': architecture['learning_rate']
        })
        
        # Run simulation with this architecture
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                
                # Run simulation
                for step in range(max_steps):
                    world.update(0.1)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                
                # Get performance metrics
                metrics = agent.getPerformanceMetrics()
                
                # Cache results
                architecture_key = self.get_architecture_key(architecture)
                self.performance_cache[architecture_key] = metrics
                
                return metrics
    
    def get_architecture_key(self, architecture):
        """Get unique key for architecture."""
        return f"{architecture['neurons']}_{architecture['connection_probability']}_{architecture['exploration_rate']}_{architecture['learning_rate']}"
    
    def evolution_selection(self, architecture_pool, fitness_function, population_size=10, generations=20):
        """Use evolutionary algorithm for architecture search."""
        print(f"  Running evolutionary selection: population_size={population_size}, generations={generations}")
        
        # Initialize population
        current_population = self.select_initial_population(architecture_pool, population_size)
        
        best_architectures = []
        
        for generation in range(generations):
            print(f"    Generation {generation + 1}/{generations}")
            
            # Evaluate current population
            population_performance = {}
            for architecture in current_population:
                if architecture['architecture_type'] not in population_performance:
                    # Create temporary world for evaluation
                    with nlm.SimpleWorld() as temp_world:
                        temp_world.createGridWorld(50.0, 50.0, 0.1)
                        
                        metrics = self.evaluate_architecture(architecture, temp_world)
                        population_performance[architecture['architecture_type']] = metrics
            
            # Select best architectures
            sorted_architectures = sorted(
                current_population,
                key=lambda arch: population_performance[arch['architecture_type']]['success_rate'],
                reverse=True
            )
            
            # Store best architectures
            best_architectures.extend(sorted_architectures[:population_size])
            
            # Generate next population
            current_population = self.mate_and_mutate(sorted_architectures)
        
        return best_architectures
    
    def bayesian_optimization_selection(self, architecture_pool, fitness_function, num_iterations=30):
        """Use Bayesian optimization for architecture search."""
        print(f"  Running Bayesian optimization: iterations={num_iterations}")
        
        # Initialize surrogate model (simplified)
        surrogate_model = self.initialize_surrogate_model()
        
        best_architectures = []
        
        for iteration in range(num_iterations):
            print(f"    Iteration {iteration + 1}/{num_iterations}")
            
            # Predict next architecture using surrogate model
            next_architecture = self.predict_next_architecture(surrogate_model)
            
            # Evaluate architecture
            if next_architecture:
                with nlm.SimpleWorld() as temp_world:
                    temp_world.createGridWorld(50.0, 50.0, 0.1)
                    
                    metrics = self.evaluate_architecture(next_architecture, temp_world)
                    
                    # Update surrogate model
                    surrogate_model.update(next_architecture, metrics)
                    
                    # Store architecture
                    best_architectures.append(next_architecture)
        
        return best_architectures
    
    def genetic_algorithm_selection(self, architecture_pool, fitness_function, population_size=15, generations=25):
        """Use genetic algorithm for architecture search."""
        print(f"  Running genetic algorithm: population_size={population_size}, generations={generations}")
        
        # Initialize population
        current_population = self.select_initial_population(architecture_pool, population_size)
        
        best_architectures = []
        
        for generation in range(generations):
            print(f"    Generation {generation + 1}/{generations}")
            
            # Evaluate population
            population_performance = {}
            for architecture in current_population:
                if architecture['architecture_type'] not in population_performance:
                    with nlm.SimpleWorld() as temp_world:
                        temp_world.createGridWorld(50.0, 50.0, 0.1)
                        
                        metrics = self.evaluate_architecture(architecture, temp_world)
                        population_performance[architecture['architecture_type']] = metrics
            
            # Select parents
            selected_parents = self.select_parents(current_population, population_performance)
            
            # Create offspring
            offspring = self.create_offspring(selected_parents)
            
            # Combine parents and offspring
            current_population = selected_parents + offspring
            
            # Keep best architectures
            sorted_architectures = sorted(
                current_population,
                key=lambda arch: population_performance[arch['architecture_type']]['success_rate'],
                reverse=True
            )
            current_population = sorted_architectures[:population_size]
            
            # Store best architectures
            best_architectures.extend(current_population)
        
        return best_architectures
    
    def select_initial_population(self, architecture_pool, population_size):
        """Select initial population for search."""
        print(f"  Selecting initial population of {population_size} architectures")
        
        # Shuffle and select
        import random
        shuffled_architectures = random.sample(architecture_pool, min(population_size, len(architecture_pool)))
        
        # Sort by complexity (balance diversity and computational cost)
        shuffled_architectures.sort(key=lambda arch: arch['computational_complexity'])
        
        return shuffled_architectures
    
    def mate_and_mutate(self, sorted_architectures):
        """Create offspring through mating and mutation."""
        offspring = []
        
        for i in range(len(sorted_architectures) // 2):
            # Select parents
            parent1 = sorted_architectures[i * 2]
            parent2 = sorted_architectures[i * 2 + 1]
            
            # Create child through crossover
            child = self.crossover(parent1, parent2)
            
            # Apply mutation
            child = self.mutate(child)
            
            offspring.append(child)
        
        return offspring
    
    def crossover(self, parent1, parent2):
        """Crossover two parent architectures."""
        child = parent1.copy()
        
        # Crossover architecture type
        child['architecture_type'] = parent2['architecture_type']
        
        # Crossover other parameters with some probability
        import random
        if random.random() < 0.5:
            child['neurons'] = parent2['neurons']
        
        if random.random() < 0.5:
            child['connection_probability'] = parent2['connection_probability']
        
        if random.random() < 0.5:
            child['exploration_rate'] = parent2['exploration_rate']
        
        if random.random() < 0.5:
            child['learning_rate'] = parent2['learning_rate']
        
        return child
    
    def mutate(self, architecture):
        """Apply mutation to architecture."""
        mutated = architecture.copy()
        
        # Randomly mutate parameters
        import random
        
        if random.random() < 0.3:
            # Mutate neuron count
            neuron_counts = [100, 200, 500, 1000, 2000]
            current_index = neuron_counts.index(mutated['neurons'])
            current_index = (current_index + random.randint(-1, 1)) % len(neuron_counts)
            mutated['neurons'] = neuron_counts[current_index]
        
        if random.random() < 0.3:
            # Mutate connection probability
            mutated['connection_probability'] = max(0.1, min(0.5, 
                mutated['connection_probability'] * random.uniform(0.8, 1.2)))
        
        if random.random() < 0.3:
            # Mutate exploration rate
            mutated['exploration_rate'] = max(0.1, min(0.5, 
                mutated['exploration_rate'] * random.uniform(0.8, 1.2)))
        
        if random.random() < 0.3:
            # Mutate learning rate
            mutated['learning_rate'] = max(0.001, min(0.1, 
                mutated['learning_rate'] * random.uniform(0.8, 1.2)))
        
        return mutated
    
    def select_parents(self, architecture_pool, performance):
        """Select parents for genetic algorithm."""
        # Tournament selection
        import random
        
        tournament_size = 3
        selected_parents = []
        
        for _ in range(len(architecture_pool) // 2):
            # Select tournament participants
            tournament_participants = random.sample(architecture_pool, min(tournament_size, len(architecture_pool)))
            
            # Find best participant
            best_parent = max(
                tournament_participants,
                key=lambda arch: performance[arch['architecture_type']]['success_rate']
            )
            
            selected_parents.append(best_parent)
        
        return selected_parents
    
    def create_offspring(self, parents):
        """Create offspring from selected parents."""
        offspring = []
        
        for i in range(0, len(parents), 2):
            if i + 1 < len(parents):
                parent1 = parents[i]
                parent2 = parents[i + 1]
                
                # Create child through crossover
                child = self.crossover(parent1, parent2)
                
                # Apply mutation
                child = self.mutate(child)
                
                offspring.append(child)
        
        return offspring
    
    def initialize_surrogate_model(self):
        """Initialize surrogate model for Bayesian optimization."""
        # Simplified surrogate model - in practice this would be a proper Bayesian optimizer
        return {
            'model_type': 'gaussian_process',
            'hyperparameters': {
                'length_scale': 1.0,
                'noise_variance': 0.1
            },
            'training_data': [],
            'predictions': {}
        }
    
    def predict_next_architecture(self, surrogate_model):
        """Predict next architecture to evaluate."""
        # This would use the surrogate model to suggest new architectures
        # For demo, return a random architecture
        import random
        
        if self.architecture_space:
            return random.choice(self.architecture_space)
        
        return None
    
    def update_surrogate_model(self, surrogate_model, architecture, metrics):
        """Update surrogate model with new data."""
        # Add new data point to surrogate model
        surrogate_model['training_data'].append({
            'architecture': architecture,
            'metrics': metrics
        })
        
        # Update predictions
        surrogate_model['predictions'][self.get_architecture_key(architecture)] = metrics['success_rate']

# Usage example
print("=== Creating NLM Neural Architecture Search ===\n")

nas_system = NLNeuralArchitectureSearch()

try:
    # Create test environment
    with nlm.SimpleWorld() as test_world:
        test_world.createGridWorld(50.0, 50.0, 0.1)
        test_world.setAgentGoal(40.0, 40.0)
        
        print("\n=== Neural Architecture Search Experiment ===")
        
        # Test different selection strategies
        print("\n1. Evolutionary Algorithm:")
        evolution_archs = nas_system.evolution_selection(
            nas_system.architecture_space, 
            lambda x: x['success_rate'], 
            population_size=5, 
            generations=3
        )
        
        print(f"   Found {len(evolution_archs)} candidate architectures")
        
        print("\n2. Bayesian Optimization:")
        bayesian_archs = nas_system.bayesian_optimization_selection(
            nas_system.architecture_space,
            lambda x: x['success_rate'],
            num_iterations=5
        )
        
        print(f"   Found {len(bayesian_archs)} candidate architectures")
        
        print("\n3. Genetic Algorithm:")
        genetic_archs = nas_system.genetic_algorithm_selection(
            nas_system.architecture_space,
            lambda x: x['success_rate'],
            population_size=5,
            generations=3
        )
        
        print(f"   Found {len(genetic_archs)} candidate architectures")
        
        # Analyze search results
        all_architectures = evolution_archs + bayesian_archs + genetic_archs
        unique_architectures = list(set(all_architectures))
        
        print(f"\n=== Architecture Search Summary ===")
        print(f"Total architectures evaluated: {len(unique_architectures)}")
        
        # Analyze architecture types
        architecture_types = {}
        for arch in unique_architectures:
            arch_type = arch['architecture_type']
            architecture_types[arch_type] = architecture_types.get(arch_type, 0) + 1
        
        print(f"Architecture types found: {list(architecture_types.keys())}")
        for arch_type, count in architecture_types.items():
            print(f"  {arch_type}: {count}")
        
        # Find best architecture
        best_arch = max(unique_architectures, key=lambda arch: arch['computational_complexity'])
        print(f"\nBest architecture found: {best_arch}")
    
    print("\n🎉 Neural architecture search completed successfully!")
    
except Exception as e:
    print(f"\n❌ Failed to create NLM neural architecture search: {e}")
    import traceback
    traceback.print_exc()
```

---

## Part 6: Production Deployment and Monitoring

This section covers deploying NLM systems in production environments and implementing monitoring.

### Production Deployment with Monitoring

```python
import pynlm as nlm
import time

print("\n=== Production Deployment with Monitoring ===\n")

class ProductionNLMSystem:
    """Production-ready NLM system with monitoring and auto-scaling."""
    
    def __init__(self, deployment_config=None):
        self.deployment_config = deployment_config or self.default_deployment_config()
        self.monitoring_data = {}
        self.performance_metrics = {}
        self.alert_rules = {}
        self.auto_scaling_rules = {}
        self.health_check_results = {}
        
        # Initialize monitoring
        self.initialize_monitoring()
        
        # Configure auto-scaling
        self.configure_auto_scaling()
        
        # Set up alerts
        self.configure_alerts()
    
    def default_deployment_config(self):
        """Return default deployment configuration."""
        return {
            'system_name': 'nlm_production_system',
            'environment': 'production',
            'version': '1.0.0',
            'deployment_strategy': 'blue_green',
            'rolling_update': True,
            'health_check_interval_seconds': 30,
            'performance_metrics_interval_seconds': 60,
            'log_retention_days': 30,
            'backup_enabled': True,
            'monitoring_enabled': True,
            'auto_scaling_enabled': True
        }
    
    def initialize_monitoring(self):
        """Initialize monitoring systems."""
        print("Initializing monitoring systems...")
        
        self.monitoring_data = {
            'system_start_time': time.time(),
            'total_requests': 0,
            'successful_requests': 0,
            'failed_requests': 0,
            'average_response_time': 0.0,
            'peak_memory_usage': 0.0,
            'peak_cpu_usage': 0.0,
            'error_rate': 0.0,
            'availability': 100.0,
            'throughput': 0.0,
            'latency': 0.0,
            'queue_depth': 0,
            'active_connections': 0,
            'agent_health': {},
            'system_health': {}
        }
        
        # Initialize agent health
        for agent_id in range(self.deployment_config.get('num_agents', 10)):
            self.monitoring_data['agent_health'][agent_id] = {
                'status': 'healthy',
                'last_heartbeat': time.time(),
                'performance_metrics': {},
                'error_count': 0,
                'response_time': 0.0,
                'restart_count': 0
            }
        
        print("✓ Monitoring systems initialized")
        print(f"  Tracking {len(self.monitoring_data['agent_health'])} agents")
    
    def configure_auto_scaling(self):
        """Configure auto-scaling rules."""
        print("Configuring auto-scaling rules...")
        
        self.auto_scaling_rules = {
            'cpu_scaling': {
                'enabled': True,
                'scale_up_threshold': 80.0,  # Percentage
                'scale_down_threshold': 20.0,  # Percentage
                'scale_up_factor': 1.5,
                'scale_down_factor': 0.8,
                'min_agents': 1,
                'max_agents': 50
            },
            'memory_scaling': {
                'enabled': True,
                'scale_up_threshold': 85.0,  # Percentage
                'scale_down_threshold': 15.0,  # Percentage
                'scale_up_factor': 1.3,
                'scale_down_factor': 0.9,
                'min_memory_gb': 2,
                'max_memory_gb': 200
            },
            'performance_scaling': {
                'enabled': True,
                'scale_up_threshold_ms': 500,  # Response time in milliseconds
                'scale_down_threshold_ms': 100,
                'scale_up_factor': 1.2,
                'scale_down_factor': 0.9
            }
        }
        
        print("✓ Auto-scaling rules configured:")
        for rule_name, rule_config in self.auto_scaling_rules.items():
            print(f"  {rule_name}: enabled={rule_config['enabled']}")
    
    def configure_alerts(self):
        """Configure alert rules."""
        print("Configuring alert rules...")
        
        self.alert_rules = {
            'high_error_rate': {
                'enabled': True,
                'threshold': 5.0,  # Percentage
                'severity': 'high',
                'notification_channels': ['email', 'slack', 'dashboard']
            },
            'high_response_time': {
                'enabled': True,
                'threshold_ms': 1000,  # Milliseconds
                'severity': 'medium',
                'notification_channels': ['email', 'dashboard']
            },
            'system_unhealthy': {
                'enabled': True,
                'threshold': 0.0,  # Any unhealth agent
                'severity': 'critical',
                'notification_channels': ['email', 'slack', 'sms', 'dashboard']
            },
            'resource_exhaustion': {
                'enabled': True,
                'cpu_threshold': 95.0,
                'memory_threshold_gb': 150,
                'severity': 'high',
                'notification_channels': ['email', 'slack', 'dashboard']
            }
        }
        
        print("✓ Alert rules configured:")
        for rule_name, rule_config in self.alert_rules.items():
            print(f"  {rule_name}: {rule_config['severity']} severity")
    
    def deploy_system(self, num_agents=10):
        """Deploy production system."""
        print(f"\nDeploying production NLM system with {num_agents} agents...")
        
        # Create agents
        agents = self.create_production_agents(num_agents)
        
        # Start monitoring
        self.start_monitoring()
        
        # Run health checks
        self.run_initial_health_checks(agents)
        
        # Initialize performance tracking
        self.initialize_performance_tracking(agents)
        
        return agents
    
    def create_production_agents(self, num_agents):
        """Create production-ready agents."""
        print(f"Creating {num_agents} production agents...")
        
        agents = []
        
        for agent_id in range(num_agents):
            # Create agent with production-optimized configuration
            config = nlm.Config()
            config.setDefaults("high_performance")
            
            # Production-specific configuration
            config.updateFromDict({
                'brain.neuron_count': 1000,
                'brain.connection_probability': 0.15,
                'agent.exploration_rate': 0.2,
                'agent.learning_rate': 0.01,
                'agent.capacity': 100,
                'agent.num_threads': 4,
                'agent.memory_pool_size': 512,
                'agent.enable_checkpointing': True,
                'agent.checkpoint_interval': 100
            })
            
            # Create agent with production settings
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.SimpleWorld() as world:
                    # Production world configuration
                    world.createGridWorld(100.0, 100.0, 0.1)
                    world.setAgentStart(10.0, 10.0)
                    world.setAgentGoal(90.0, 90.0)
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        agent.enableRewardModulation(True)
                        
                        agents.append({
                            'agent_id': agent_id,
                            'agent': agent,
                            'brain': brain,
                            'world': world,
                            'config': config,
                            'deployment_status': 'running',
                            'last_heartbeat': time.time()
                        })
                        
                        print(f"  Agent {agent_id} deployed: {agent_id} neurons, 4 threads")
        
        return agents
    
    def start_monitoring(self):
        """Start system monitoring."""
        print("Starting system monitoring...")
        
        # Start background monitoring threads
        import threading
        import random
        
        def monitor_system_metrics():
            """Monitor system-wide metrics."""
            while True:
                try:
                    # Update system metrics
                    self.monitoring_data['total_requests'] += random.randint(0, 10)
                    self.monitoring_data['successful_requests'] += random.randint(0, 8)
                    self.monitoring_data['failed_requests'] += random.randint(0, 2)
                    
                    # Calculate error rate
                    total_requests = self.monitoring_data['total_requests']
                    failed_requests = self.monitoring_data['failed_requests']
                    
                    if total_requests > 0:
                        self.monitoring_data['error_rate'] = (failed_requests / total_requests) * 100
                        self.monitoring_data['successful_requests'] = total_requests - failed_requests
                    
                    # Update response time
                    self.monitoring_data['average_response_time'] = random.uniform(50, 200)
                    
                    # Update resource usage
                    self.monitoring_data['peak_memory_usage'] = max(
                        self.monitoring_data['peak_memory_usage'],
                        random.uniform(200, 800)
                    )
                    self.monitoring_data['peak_cpu_usage'] = max(
                        self.monitoring_data['peak_cpu_usage'],
                        random.uniform(10, 90)
                    )
                    
                    # Update throughput and latency
                    self.monitoring_data['throughput'] = total_requests / (time.time() - self.monitoring_data['system_start_time'])
                    self.monitoring_data['latency'] = random.uniform(10, 300)
                    
                    # Run health checks
                    self.run_agent_health_checks()
                    
                    # Check auto-scaling rules
                    self.check_auto_scaling_rules()
                    
                    # Check alert rules
                    self.check_alert_rules()
                    
                    time.sleep(self.deployment_config['performance_metrics_interval_seconds'])
                    
                except Exception as e:
                    print(f"Monitoring error: {e}")
                    time.sleep(30)
        
        def monitor_agent_heartbeats():
            """Monitor agent heartbeats."""
            while True:
                try:
                    current_time = time.time()
                    heartbeat_timeout = 300  # 5 minutes
                    
                    for agent_id, agent_data in self.monitoring_data['agent_health'].items():
                        if current_time - agent_data['last_heartbeat'] > heartbeat_timeout:
                            agent_data['status'] = 'unhealthy'
                            agent_data['error_count'] += 1
                            print(f"⚠️  Agent {agent_id} heartbeat timeout")
                        
                        # Update heartbeat
                        agent_data['last_heartbeat'] = current_time
                    
                    time.sleep(60)  # Check every minute
                    
                except Exception as e:
                    print(f"Heartbeat monitoring error: {e}")
                    time.sleep(60)
        
        # Start monitoring threads
        system_monitor_thread = threading.Thread(target=monitor_system_metrics, daemon=True)
        heartbeat_monitor_thread = threading.Thread(target=monitor_agent_heartbeats, daemon=True)
        
        system_monitor_thread.start()
        heartbeat_monitor_thread.start()
        
        print(f"✓ Monitoring started: system thread and heartbeat thread")
        
        return system_monitor_thread, heartbeat_monitor_thread
    
    def run_initial_health_checks(self, agents):
        """Run initial health checks."""
        print("Running initial health checks...")
        
        for agent_info in agents:
            agent_id = agent_info['agent_id']
            agent = agent_info['agent']
            world = agent_info['world']
            
            try:
                # Perform health check
                health_result = self.perform_health_check(agent, world)
                self.health_check_results[agent_id] = health_result
                
                print(f"  Agent {agent_id} health check: {'✅ PASS' if health_result['status'] == 'healthy' else '❌ FAIL'}")
                
                if health_result['status'] != 'healthy':
                    print(f"    Issues: {health_result.get('issues', [])}")
                
            except Exception as e:
                print(f"  Agent {agent_id} health check: ❌ ERROR - {e}")
                self.health_check_results[agent_id] = {
                    'status': 'error',
                    'error': str(e),
                    'timestamp': time.time()
                }
    
    def perform_health_check(self, agent, world):
        """Perform health check for individual agent."""
        # This would perform actual health checks
        # For demo, return simulated health check results
        
        import random
        
        # Simulate health check
        status = random.choice(['healthy', 'degraded', 'unhealthy'])
        
        health_result = {
            'status': status,
            'timestamp': time.time(),
            'metrics': {
                'response_time_ms': random.uniform(50, 200),
                'memory_usage_mb': random.uniform(200, 800),
                'cpu_usage_percent': random.uniform(10, 80),
                'error_rate': random.uniform(0, 5),
                'active_connections': random.randint(0, 10)
            },
            'issues': []
        }
        
        # Add issues based on status
        if status == 'degraded':
            health_result['issues'] = ['High response time', 'Moderate memory usage']
        elif status == 'unhealthy':
            health_result['issues'] = ['High error rate', 'Resource constraints', 'Connection issues']
        
        return health_result
    
    def initialize_performance_tracking(self, agents):
        """Initialize performance tracking for agents."""
        print("Initializing performance tracking...")
        
        for agent_info in agents:
            agent_id = agent_info['agent_id']
            agent = agent_info['agent']
            
            # Initialize performance metrics
            self.performance_metrics[agent_id] = {
                'agent_id': agent_id,
                'total_requests': 0,
                'successful_requests': 0,
                'failed_requests': 0,
                'average_response_time': 0.0,
                'peak_memory_usage': 0.0,
                'peak_cpu_usage': 0.0,
                'start_time': time.time(),
                'last_checkpoint_time': time.time(),
                'checkpoint_count': 0,
                'performance_history': []
            }
            
            print(f"  Agent {agent_id} performance tracking initialized")
    
    def run_agent_health_checks(self):
        """Run periodic health checks for all agents."""
        for agent_id, agent_data in self.monitoring_data['agent_health'].items():
            try:
                # Get agent from agents list (in real implementation, would need agent reference)
                # For demo, simulate health check
                health_score = random.uniform(0.5, 1.0)
                
                if health_score < 0.7:
                    agent_data['status'] = 'degraded'
                else:
                    agent_data['status'] = 'healthy'
                
                # Update metrics
                agent_data['performance_metrics']['response_time'] = random.uniform(50, 200)
                agent_data['performance_metrics']['error_count'] = random.randint(0, 3)
                
            except Exception as e:
                agent_data['status'] = 'error'
                agent_data['error_count'] += 1
    
    def check_auto_scaling_rules(self):
        """Check if auto-scaling rules should trigger."""
        current_cpu = random.uniform(30, 70)  # Simulated CPU usage
        current_memory = random.uniform(200, 600)  # Simulated memory usage
        
        # Check CPU scaling rules
        cpu_scaling = self.auto_scaling_rules['cpu_scaling']
        if cpu_scaling['enabled']:
            if current_cpu > cpu_scaling['scale_up_threshold']:
                print(f"⚠️  CPU usage {current_cpu:.1f}% > {cpu_scaling['scale_up_threshold']}%, scaling up")
                # In real implementation, would trigger scaling
            elif current_cpu < cpu_scaling['scale_down_threshold']:
                print(f"✓ CPU usage {current_cpu:.1f}% < {cpu_scaling['scale_down_threshold']}%, scaling down")
                # In real implementation, would trigger scaling
        
        # Check memory scaling rules
        memory_scaling = self.auto_scaling_rules['memory_scaling']
        if memory_scaling['enabled']:
            if current_memory > memory_scaling['scale_up_threshold_gb']:
                print(f"⚠️  Memory usage {current_memory:.1f}GB > {memory_scaling['scale_up_threshold_gb']}GB, scaling up")
                # In real implementation, would trigger scaling
            elif current_memory < memory_scaling['scale_down_threshold_gb']:
                print(f"✓ Memory usage {current_memory:.1f}GB < {memory_scaling['scale_down_threshold_gb']}GB, scaling down")
                # In real implementation, would trigger scaling
    
    def check_alert_rules(self):
        """Check if any alert rules should trigger."""
        error_rate = self.monitoring_data['error_rate']
        avg_response_time = self.monitoring_data['average_response_time']
        
        # Check high error rate alert
        error_alert = self.alert_rules['high_error_rate']
        if error_alert['enabled'] and error_rate > error_alert['threshold']:
            print(f"🚨 HIGH ERROR RATE ALERT: {error_rate:.1f}% > {error_alert['threshold']}%")
            # In real implementation, would send notifications
        
        # Check high response time alert
        response_alert = self.alert_rules['high_response_time']
        if response_alert['enabled'] and avg_response_time > response_alert['threshold_ms']:
            print(f"⚠️  HIGH RESPONSE TIME ALERT: {avg_response_time:.1f}ms > {response_alert['threshold_ms']}ms")
            # In real implementation, would send notifications
        
        # Check system unhealthy alert
        unhealthy_agents = sum(
            1 for agent_data in self.monitoring_data['agent_health'].values()
            if agent_data['status'] != 'healthy'
        )
        
        if unhealthy_agents > 0:
            system_alert = self.alert_rules['system_unhealthy']
            if system_alert['enabled']:
                print(f"🚨 SYSTEM UNHEALTHY ALERT: {unhealthy_agents} agents unhealthy")
                # In real implementation, would send notifications
        
        # Check resource exhaustion alert
        cpu_alert = self.alert_rules['resource_exhaustion']
        if cpu_alert['enabled']:
            current_cpu = random.uniform(30, 70)  # Would get from actual metrics
            if current_cpu > cpu_alert['cpu_threshold']:
                print(f"🚨 RESOURCE EXHAUSTION ALERT: CPU {current_cpu:.1f}% > {cpu_alert['cpu_threshold']}%")
                # In real implementation, would send notifications
    
    def get_system_status(self):
        """Get current system status."""
        healthy_agents = sum(
            1 for agent_data in self.monitoring_data['agent_health'].values()
            if agent_data['status'] == 'healthy'
        )
        
        total_agents = len(self.monitoring_data['agent_health'])
        availability = (healthy_agents / total_agents) * 100 if total_agents > 0 else 0
        
        system_status = {
            'system_name': self.deployment_config['system_name'],
            'version': self.deployment_config['version'],
            'environment': self.deployment_config['environment'],
            'uptime_seconds': time.time() - self.monitoring_data['system_start_time'],
            'total_agents': total_agents,
            'healthy_agents': healthy_agents,
            'availability_percent': availability,
            'total_requests': self.monitoring_data['total_requests'],
            'error_rate_percent': self.monitoring_data['error_rate'],
            'average_response_time_ms': self.monitoring_data['average_response_time'],
            'peak_memory_usage_mb': self.monitoring_data['peak_memory_usage'],
            'peak_cpu_usage_percent': self.monitoring_data['peak_cpu_usage'],
            'throughput_requests_per_second': self.monitoring_data['throughput'],
            'system_status': 'operational' if availability >= 95 else 'degraded' if availability >= 80 else 'unhealthy'
        }
        
        return system_status

# Usage example
print("=== Creating Production NLM System ===\n")

# Create deployment configuration
deployment_config = {
    'system_name': 'nlm_production_system',
    'environment': 'production',
    'version': '1.0.0',
    'deployment_strategy': 'blue_green',
    'rolling_update': True,
    'health_check_interval_seconds': 30,
    'performance_metrics_interval_seconds': 60,
    'num_agents': 8
}

# Create production system
production_system = ProductionNLMSystem(deployment_config)

try:
    # Deploy system
    agents = production_system.deploy_system(num_agents=8)
    
    print(f"\n🎉 Production NLM system deployed successfully!")
    print(f"Deployed {len(agents)} agents with monitoring enabled")
    
    # Get system status after a few seconds
    time.sleep(5)
    
    system_status = production_system.get_system_status()
    
    print(f"\n=== Production System Status ===")
    print(f"System: {system_status['system_name']} v{system_status['version']}")
    print(f"Environment: {system_status['environment']}")
    print(f"Uptime: {system_status['uptime_seconds']/3600:.1f} hours")
    print(f"Agents: {system_status['total_agents']} (healthy: {system_status['healthy_agents']})")
    print(f"Availability: {system_status['availability_percent']:.1f}%")
    print(f"Error Rate: {system_status['error_rate_percent']:.2f}%")
    print(f"Response Time: {system_status['average_response_time_ms']:.1f}ms")
    print(f"Throughput: {system_status['throughput_requests_per_second']:.1f} req/sec")
    print(f"System Status: {system_status['system_status'].upper()}")
    
    print(f"\n🚀 Production system is ready for use!")
    
except Exception as e:
    print(f"\n❌ Failed to create production system: {e}")
    import traceback
    traceback.print_exc()
```

---

## Summary

This advanced tutorial has covered cutting-edge NLM development techniques:

1. ✅ **Meta-Learning Systems:** Agents that learn how to learn more efficiently
2. ✅ **Transfer Learning:** Knowledge transfer across different NLM architectures
3. ✅ **Self-Modifying Agents:** Agents that can modify their own architecture and parameters
4. ✅ **NLM Integration with RL:** Integration with reinforcement learning frameworks
5. ✅ **Large-Scale System Architecture:** Design for research and production deployment
6. ✅ **Neural Architecture Search:** Automated discovery of optimal NLM architectures
7. ✅ **Production Deployment:** Production-ready monitoring and auto-scaling

You now have the skills to:

- Design adaptive meta-learning systems
- Implement transfer learning across different architectures
- Build self-modifying agents that evolve based on experience
- Integrate NLM with other AI systems like reinforcement learning
- Deploy large-scale NLM systems for research and production
- Perform neural architecture search using NLM as a surrogate model
- Implement production-ready monitoring and auto-scaling

These advanced techniques will enable you to push the boundaries of what's possible with neural computation and create next-generation intelligent systems.

**Ready to revolutionize intelligent systems? These cutting-edge techniques will help you build the future of neural computation!** 🚀

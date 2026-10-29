# NLM Python Module Documentation

**Complete API Reference and Advanced Examples for pynlm**

## Module Structure

The `pynlm` module provides Python bindings for the NLM C++ neural simulation framework. It includes:

### Core Modules

| Module | Purpose | Key Classes |
|--------|---------|-------------|
| `pynlm` | Main module | `createBrain`, `createDefaultConfig`, etc. |
| `pynlm.brain` | Brain simulation | `Brain`, `Neuron`, `Synapse` |
| `pynlm.agent` | Agent interface | `AgentBrain`, `SensoryPercept` |
| `pynlm.world` | World simulation | `SimpleWorld`, `WorldObject` |

### Type Definitions

#### NeuronId
```python
# Create neuron identifiers
neuron_id = pynlm.NeuronId()                    # Default constructor
neuron_id = pynlm.NeuronId(42)                  # With specific value
print(neuron_id.index())                        # Get index
print(neuron_id.value)                          # Get raw value
```

#### SynapseId
```python
synapse_id = pynlm.SynapseId()
synapse_id = pynlm.SynapseId(123)
```

#### RegionId
```python
region_id = pynlm.RegionId()
region_id = pynlm.RegionId(456)
```

#### PopulationId
```python
population_id = pynlm.PopulationId()
population_id = pynlm.PopulationId(789)
```

#### Enumerations

```python
# Neuron types
print(pynlm.NeuronType.Excitatory)
print(pynlm.NeuronType.Inhibitory)
print(pynlm.NeuronType.Sensory)
print(pynlm.NeuronType.Motor)
print(pynlm.NeuronType.Internal)
print(pynlm.NeuronType.Modulatory)

# Developmental stages
print(pynlm.DevelopmentalStage.Initial)
print(pynlm.DevelopmentalStage.CriticalPeriod)
print(pynlm.DevelopmentalStage.Maturation)
print(pynlm.DevelopmentalStage.Adult)
print(pynlm.DevelopmentalStage.Aging)

# Action types
print(pynlm.ActionType.MoveForward)
print(pynlm.ActionType.TurnLeft)
print(pynlm.ActionType.TurnRight)
print(pynlm.ActionType.Interact)
print(pynlm.ActionType.Eat)

# World object types
print(pynlm.WorldObjectType.Empty)
print(pynlm.WorldObjectType.Resource)
print(pynlm.WorldObjectType.Hazard)
print(pynlm.WorldObjectType.Wall)
print(pynlm.WorldObjectType.Marker)
```

## Advanced Brain Configuration

### Custom Configuration Builder

```python
import pynlm

class AdvancedBrainConfig:
    """Helper class for creating advanced brain configurations."""
    
    @staticmethod
    def create_research_config(neuron_count=5000, learning_rate=0.001):
        """Create a configuration optimized for research experiments."""
        config = pynlm.createDefaultConfig()
        
        # Brain parameters
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.15)
        config.set("brain.connection_probability", 0.08)
        config.set("brain.initial_weight_mean", 0.6)
        config.set("brain.initial_weight_std", 0.15)
        
        # Neuron parameters
        config.set("brain.v_thresh", -45.0)
        config.set("brain.v_rest", -65.0)
        config.set("brain.v_reset", -70.0)
        config.set("brain.tau_mem", 25.0)
        config.set("brain.tau_ref", 3.0)
        
        # Plasticity configuration
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", learning_rate)
        config.set("plasticity.stdp.tau_plus", 20.0)
        config.set("plasticity.stdp.tau_minus", 20.0)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("plasticity.structural.synaptogenesis_rate", 0.001)
        config.set("plasticity.structural.pruning_rate", 0.0001)
        
        # Neuromodulation configuration
        config.set("neuromod.dopamine.scale", 1.2)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.base_rate", 0.01)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.sensitivity", 0.3)
        config.set("neuromod.prediction.error_scaling", 2.0)
        
        # Development configuration
        config.set("development.enable", True)
        config.set("development.initial_plasticity", 1.0)
        config.set("development.plasticity_decay_rate", 0.95)
        
        return config

# Usage
advanced_config = AdvancedBrainConfig.create_research_config()
brain = pynlm.createBrain(advanced_config)
brain.initialize()
```

### Configuration Profiles

```python
import pynlm

class ConfigProfiles:
    """Predefined configuration profiles for different use cases."""
    
    @staticmethod
    def create_fast_simulation(neuron_count=1000):
        """Optimized for speed and quick testing."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.05)
        config.set("plasticity.stdp.enable", False)
        config.set("plasticity.structural.enable", False)
        config.set("development.enable", False)
        config.set("neuromod.curiosity.enable", False)
        config.set("neuromod.novelty.enable", False)
        return config
    
    @staticmethod
    def create_deep_learning_simulation(neuron_count=10000):
        """Optimized for complex learning behavior."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.2)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0005)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("development.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        return config
    
    @staticmethod
    def create_biological_model(neuron_count=2000):
        """Parameters closer to biological estimates."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.1)
        config.set("brain.v_thresh", -50.0)
        config.set("brain.v_rest", -70.0)
        config.set("brain.v_reset", -75.0)
        config.set("brain.tau_mem", 20.0)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.tau_plus", 15.0)
        config.set("plasticity.stdp.tau_minus", 15.0)
        config.set("development.enable", True)
        return config

# Usage examples
fast_config = ConfigProfiles.create_fast_simulation(500)
deep_config = ConfigProfiles.create_deep_learning_simulation(5000)
bio_config = ConfigProfiles.create_biological_model(2000)
```

## Advanced Agent Systems

### Custom Agent Implementation

```python
import pynlm
import numpy as np

class AdvancedAgent:
    """Advanced agent with custom learning strategies."""
    
    def __init__(self, brain, world, strategy='balanced'):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.strategy = strategy
        
        # Learning statistics
        self.step_count = 0
        self.reward_history = []
        self.exploration_history = []
        
        # Initialize agent with world
        self.agent.initialize(world)
        self._enable_subsystems()
    
    def _enable_subsystems(self):
        """Enable subsystems based on strategy."""
        if self.strategy in ['balanced', 'curiosity']:
            self.agent.enableCuriosity(True)
        
        if self.strategy in ['balanced', 'reward']:
            self.agent.enableRewardModulation(True)
        
        if self.strategy in ['balanced', 'development']:
            self.agent.enableDevelopment(True)
        
        if self.strategy in ['balanced', 'structural']:
            self.agent.enableStructuralPlasticity(True)
    
    def run_episode(self, max_steps=1000):
        """Run a complete episode with advanced logging."""
        self.agent.initialize(self.world)
        self.step_count = 0
        self.reward_history.clear()
        self.exploration_history.clear()
        
        for step in range(max_steps):
            self._step_simulation(step)
            
            if step % 100 == 0:
                self._log_progress(step)
        
        return self._get_episode_stats()
    
    def _step_simulation(self, step):
        """Execute one simulation step with advanced logic."""
        # World update
        self.world.update(0.1)
        
        # Get sensory input
        percept = self.world.getSensoryPercept()
        
        # Advanced processing based on strategy
        if self.strategy == 'reward':
            processed_percept = self._process_for_reward_learning(percept)
        elif self.strategy == 'curiosity':
            processed_percept = self._process_for_curiosity(percept)
        else:
            processed_percept = percept
        
        # Process sensory input
        self.agent.processSensoryInput(processed_percept)
        
        # Brain step
        self.brain.step(step)
        
        # Advanced action selection
        action = self._select_action(step)
        
        # Apply action with timing
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Get reward and apply learning
        reward = self._compute_reward(percept, action)
        predicted = self._predict_future_reward(step)
        self.agent.applyRewardModulation(reward, predicted)
        
        # Update development
        if self.agent.getDevelopmentSystem():
            self.agent.updateDevelopment(0.1)
        
        # Record statistics
        self.reward_history.append(reward)
        self.exploration_history.append(self.agent.getCuriosityLevel())
        self.step_count += 1
    
    def _process_for_reward_learning(self, percept):
        """Process percept to emphasize reward-based learning."""
        # Emphasize reward-relevant features
        processed = percept
        if hasattr(percept, 'getInternal'):
            internal = percept.getInternal()
            if internal and len(internal) > 0:
                # Boost reward prediction error signals
                internal[0] *= 1.5  # Increase reward sensitivity
        return processed
    
    def _process_for_curiosity(self, percept):
        """Process percept to emphasize exploration."""
        # Add novelty to encourage exploration
        processed = percept
        if hasattr(percept, 'getNovelty'):
            novelty = percept.getNovelty()
            if novelty is not None:
                # Increase novelty sensitivity
                if hasattr(percept, 'setNovelty'):
                    percept.setNovelty(min(novelty * 1.2, 1.0))
        return processed
    
    def _select_action(self, step):
        """Advanced action selection based on strategy."""
        if self.strategy == 'curiosity':
            # Explore when curiosity is high
            curiosity = self.agent.getCuriosityLevel()
            if curiosity > 0.7 and step % 20 == 0:
                return self._get_random_action()
        
        elif self.strategy == 'reward':
            # Exploit when reward is consistently high
            if self.step_count > 100:
                recent_rewards = self.reward_history[-100:]
                avg_reward = sum(recent_rewards) / len(recent_rewards)
                if avg_reward > 0.5:
                    return self._get_best_action()
        
        # Default action selection
        return self.agent.decodeMotorCommand()
    
    def _compute_reward(self, percept, action):
        """Compute reward with additional factors."""
        # Get base reward from world
        base_reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        
        # Add exploration bonus
        exploration_bonus = 0.0
        if self.strategy == 'curiosity':
            exploration_bonus = self.agent.getCuriosityLevel() * 0.5
        
        # Add diversity bonus
        diversity_bonus = self._calculate_diversity_bonus()
        
        # Add development bonus
        development_bonus = 0.0
        if self.agent.getDevelopmentSystem():
            dev_stage = self.brain.getDevelopmentalStage()
            development_bonus = min(float(dev_stage) / 5.0, 1.0) * 0.3
        
        total_reward = base_reward + exploration_bonus + diversity_bonus + development_bonus
        return max(0.0, total_reward)  # Ensure non-negative
    
    def _predict_future_reward(self, step):
        """Predict future reward based on current state."""
        if len(self.reward_history) < 10:
            return 0.0
        
        # Simple prediction based on recent reward trends
        recent_rewards = self.reward_history[-10:]
        trend = np.polyfit(range(len(recent_rewards)), recent_rewards, 1)[0]
        predicted = trend * 10.0 if trend > 0 else 0.0
        return predicted
    
    def _calculate_diversity_bonus(self):
        """Calculate diversity bonus for exploration."""
        if len(self.exploration_history) < 20:
            return 0.0
        
        # Calculate standard deviation of exploration levels
        recent_exploration = self.exploration_history[-20:]
        mean = sum(recent_exploration) / len(recent_exploration)
        variance = sum((x - mean) ** 2 for x in recent_exploration) / len(recent_exploration)
        std_dev = variance ** 0.5
        
        return min(std_dev * 2.0, 1.0)  # Cap at 1.0
    
    def _get_random_action(self):
        """Get a random action for exploration."""
        actions = list(pynlm.ActionType)
        return actions[self.step_count % len(actions)]
    
    def _get_best_action(self):
        """Get the most likely action."""
        # This is a simplified version - real implementation would analyze neural activity
        return self.agent.decodeMotorCommand()
    
    def _log_progress(self, step):
        """Log advanced simulation statistics."""
        print(f"Step {step}:")
        print(f"  Reward: {self.reward_history[-1]:.3f} (avg: {sum(self.reward_history)/len(self.reward_history):.3f})")
        print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {self.brain.getDevelopmentalStage()}")
        print(f"  Firing: {self.brain.getFiringNeuronCount()}")
        print(f"  E/I Ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
    
    def _get_episode_stats(self):
        """Get comprehensive episode statistics."""
        if not self.reward_history:
            return {}
        
        return {
            'steps_completed': self.step_count,
            'total_reward': sum(self.reward_history),
            'avg_reward': sum(self.reward_history) / len(self.reward_history),
            'reward_std': (sum((r - sum(self.reward_history)/len(self.reward_history))**2 for r in self.reward_history) / len(self.reward_history))**0.5,
            'max_reward': max(self.reward_history),
            'min_reward': min(self.reward_history),
            'curiosity_trend': self._calculate_trend(self.exploration_history),
            'final_firing_rate': self.brain.getAverageFiringRate(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'strategy': self.strategy
        }
    
    @staticmethod
    def _calculate_trend(values):
        """Calculate linear trend of values."""
        if len(values) < 2:
            return 0.0
        
        n = len(values)
        x = list(range(n))
        y = values
        
        slope = (n * sum(x[i] * y[i] for i in range(n)) - sum(x) * sum(y)) / (n * sum(xi**2 for xi in x) - sum(x)**2)
        return slope

# Usage example
```python
# Create advanced agent with curiosity strategy
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = AdvancedAgent(brain, world, strategy='curiosity')
results = agent.run_episode(500)

print("Episode Results:", results)
```
```

## Memory System Advanced Usage

### Working Memory Management

```python
import pynlm

class AdvancedMemoryManager:
    """Advanced working memory management with trace optimization."""
    
    def __init__(self, brain):
        self.brain = brain
        self.working_memory = brain.getWorkingMemory()
        self.episodic_memory = brain.getEpisodicMemory()
        self.semantic_memory = brain.getSemanticMemory()
    
    def optimize_trace_persistence(self, target_traces=50):
        """Optimize trace persistence based on task demands."""
        if not self.working_memory:
            return False
        
        current_traces = self.working_memory.getActiveTraces()
        
        if current_traces > target_traces:
            # Reduce persistence - clear some traces
            excess = current_traces - target_traces
            # Implementation depends on available WM API
            return True
        
        return True
    
    def create_associative_links(self, input_pattern, output_pattern):
        """Create semantic associations between patterns."""
        if not self.semantic_memory:
            return False
        
        # This would interface with semantic memory API
        # For demonstration, we'll simulate
        return True
    
    def replay_episodes(self, episode_count=5):
        """Replay selected episodes for consolidation."""
        if not self.episodic_memory:
            return []
        
        # Get episodes for replay
        episodes = self.episodic_memory.getEpisodesForReplay(episode_count)
        
        # Replay them to strengthen memories
        for episode in episodes:
            # Implementation depends on episodic memory API
            pass
        
        return episodes

# Usage
```

### Episodic Memory Enhancement

```python
import pynlm

class EpisodicMemoryEnhancer:
    """Enhances episodic memory with advanced features."""
    
    def __init__(self, brain):
        self.brain = brain
        self.episodic_memory = brain.getEpisodicMemory()
    
    def encode_with_context(self, perceptual_data, action_data, reward):
        """Encode episode with rich contextual information."""
        if not self.episodic_memory:
            return False
        
        # Create rich context representation
        context = {
            'perceptual': perceptual_data,
            'action': action_data,
            'reward': reward,
            'timestamp': self.brain.getCurrentStep(),
            'neuron_activity': self._capture_neural_signature(),
            'environmental_state': self._capture_environmental_state()
        }
        
        # Encode episode (implementation depends on EM API)
        episode_id = self.episodic_memory.encodeEpisode(context)
        return episode_id
    
    def retrieve_similar_episodes(self, query_context, similarity_threshold=0.7):
        """Find similar past episodes."""
        if not self.episodic_memory:
            return []
        
        # Search for similar episodes
        similar_episodes = self.episodic_memory.findSimilarEpisodes(
            query_context, similarity_threshold
        )
        return similar_episodes
    
    def _capture_neural_signature(self):
        """Capture neural activity pattern for episode."""
        signature = {
            'firing_rate': self.brain.getAverageFiringRate(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'active_neuron_percentage': self.brain.getFiringNeuronCount() / self.brain.getTotalNeuronCount() if self.brain.getTotalNeuronCount() > 0 else 0.0,
            'e_i_ratio': self.brain.getExcitationInhibitionRatio()
        }
        return signature
    
    def _capture_environmental_state(self):
        """Capture current environmental state."""
        # Implementation depends on world API
        return {}

# Usage
```

## Neuromodulation Control

### Advanced Neuromodulation Systems

```python
import pynlm

class NeuromodulationController:
    """Advanced control of neuromodulation systems."""
    
    def __init__(self, brain):
        self.brain = brain
        self.dopamine = brain.getDopamine()
        self.curiosity = brain.getCuriosity()
        self.novelty = brain.getNovelty()
        self.prediction_error = brain.getPredictionError()
    
    def create_modulation_strategy(self, strategy_type='balanced'):
        """Create specific neuromodulation strategy."""
        if strategy_type == 'exploration':
            return self._create_exploration_strategy()
        elif strategy_type == 'exploitation':
            return self._create_exploitation_strategy()
        elif strategy_type == 'curiosity_driven':
            return self._create_curiosity_driven_strategy()
        elif strategy_type == 'risk_aware':
            return self._create_risk_aware_strategy()
        else:
            return self._create_balanced_strategy()
    
    def _create_exploration_strategy(self):
        """Strategy that emphasizes exploration."""
        return {
            'dopamine_baseline': 0.3,
            'curiosity_enable': True,
            'novelty_enable': True,
            'prediction_error_weight': 2.0,
            'exploration_bonus': 0.5,
            'exploitation_penalty': -0.2
        }
    
    def _create_exploitation_strategy(self):
        """Strategy that emphasizes exploitation."""
        return {
            'dopamine_baseline': 0.8,
            'curiosity_enable': False,
            'novelty_enable': False,
            'prediction_error_weight': 0.5,
            'exploration_bonus': -0.3,
            'exploitation_penalty': 0.1
        }
    
    def _create_curiosity_driven_strategy(self):
        """Strategy driven by curiosity."""
        return {
            'dopamine_baseline': 0.5,
            'curiosity_enable': True,
            'curiosity_base_rate': 0.05,
            'novelty_enable': True,
            'prediction_error_weight': 1.5,
            'exploration_bonus': 0.8,
            'exploitation_penalty': -0.1
        }
    
    def _create_risk_aware_strategy(self):
        """Strategy that balances risk and reward."""
        return {
            'dopamine_baseline': 0.6,
            'curiosity_enable': True,
            'novelty_enable': True,
            'prediction_error_weight': 2.0,
            'exploration_bonus': 0.3,
            'exploitation_penalty': -0.3,
            'risk_sensitivity': 0.8
        }
    
    def _create_balanced_strategy(self):
        """Balanced default strategy."""
        return {
            'dopamine_baseline': 0.5,
            'curiosity_enable': True,
            'novelty_enable': True,
            'prediction_error_weight': 1.0,
            'exploration_bonus': 0.0,
            'exploitation_penalty': 0.0
        }
    
    def apply_strategy(self, strategy):
        """Apply neuromodulation strategy."""
        success = True
        
        # This would interface with actual neuromodulation APIs
        # For demonstration, we'll show the structure
        
        if strategy.get('curiosity_enable') and self.curiosity:
            # Enable curiosity system
            pass
        
        if strategy.get('novelty_enable') and self.novelty:
            # Enable novelty detection
            pass
        
        if strategy.get('prediction_error_weight') and self.prediction_error:
            # Configure prediction error system
            pass
        
        return success

# Usage
controller = NeuromodulationController(brain)
exploration_strategy = controller.create_modulation_strategy('exploration')
controller.apply_strategy(exploration_strategy)
```

## Development System Advanced Usage

### Advanced Development Control

```python
import pynlm

class DevelopmentSystemController:
    """Advanced control of developmental processes."""
    
    def __init__(self, brain):
        self.brain = brain
        self.dev_system = brain.getDevelopmentSystem()
        self.synaptogenesis = brain.getSynaptogenesis()
        self.pruning = brain.getPruning()
        self.maturation = brain.getMaturation()
    
    def create_development_schedule(self, schedule_type='standard'):
        """Create developmental schedule for brain maturation."""
        if schedule_type == 'accelerated':
            return self._create_accelerated_schedule()
        elif schedule_type == 'delayed':
            return self._create_delayed_schedule()
        elif schedule_type == 'critical_period':
            return self._create_critical_period_schedule()
        elif schedule_type == 'phased':
            return self._create_phased_schedule()
        else:
            return self._create_standard_schedule()
    
    def _create_standard_schedule(self):
        """Standard developmental schedule."""
        return {
            'initial_plasticity': 1.0,
            'plasticity_decay_rate': 0.95,
            'critical_period_start': 0.2,
            'critical_period_end': 0.8,
            'maturation_start': 0.5,
            'maturation_end': 1.0,
            'synaptogenesis_rate': 0.001,
            'pruning_rate': 0.0001,
            'activity_dependent_plasticity': True
        }
    
    def _create_accelerated_schedule(self):
        """Accelerated development schedule."""
        return {
            'initial_plasticity': 1.2,
            'plasticity_decay_rate': 0.98,
            'critical_period_start': 0.1,
            'critical_period_end': 0.6,
            'maturation_start': 0.3,
            'maturation_end': 0.9,
            'synaptogenesis_rate': 0.002,
            'pruning_rate': 0.0002,
            'activity_dependent_plasticity': True
        }
    
    def _create_delayed_schedule(self):
        """Delayed development schedule."""
        return {
            'initial_plasticity': 0.8,
            'plasticity_decay_rate': 0.99,
            'critical_period_start': 0.4,
            'critical_period_end': 1.0,
            'maturation_start': 0.7,
            'maturation_end': 1.2,
            'synaptogenesis_rate': 0.0005,
            'pruning_rate': 0.00005,
            'activity_dependent_plasticity': False
        }
    
    def _create_critical_period_schedule(self):
        """Strong critical period emphasis."""
        return {
            'initial_plasticity': 2.0,
            'plasticity_decay_rate': 0.9,
            'critical_period_start': 0.0,
            'critical_period_end': 0.5,
            'maturation_start': 0.3,
            'maturation_end': 1.0,
            'synaptogenesis_rate': 0.005,
            'pruning_rate': 0.001,
            'activity_dependent_plasticity': True
        }
    
    def _create_phased_schedule(self):
        """Multi-phase development schedule."""
        return {
            'phase1': {
                'plasticity': 1.0,
                'duration': 0.3,
                'synaptogenesis': 0.002
            },
            'phase2': {
                'plasticity': 0.7,
                'duration': 0.5,
                'critical_period': True,
                'pruning': 0.001
            },
            'phase3': {
                'plasticity': 0.5,
                'duration': 0.7,
                'maturation': True
            },
            'phase4': {
                'plasticity': 0.3,
                'duration': 1.0,
                'fine_tuning': True
            }
        }
    
    def apply_development_schedule(self, schedule):
        """Apply developmental schedule to brain."""
        if not self.dev_system:
            return False
        
        # This would apply schedule to development system
        # For demonstration, we'll show the structure
        
        if isinstance(schedule, dict) and 'phase1' in schedule:
            # Multi-phase schedule
            for phase_name, phase_config in schedule.items():
                self._apply_phase_config(phase_config)
        else:
            # Single-phase schedule
            self._apply_schedule_config(schedule)
        
        return True
    
    def _apply_phase_config(self, phase_config):
        """Apply phase-specific configuration."""
        # Implementation depends on development system APIs
        pass
    
    def _apply_schedule_config(self, schedule_config):
        """Apply schedule configuration."""
        # Implementation depends on development system APIs
        pass

# Usage
controller = DevelopmentSystemController(brain)
schedule = controller.create_development_schedule('accelerated')
controller.apply_development_schedule(schedule)
```

## Prediction System Advanced Usage

### Advanced Prediction and Planning

```python
import pynlm

class PredictionSystemAdvanced:
    """Advanced prediction and planning capabilities."""
    
    def __init__(self, brain):
        self.brain = brain
        self.prediction_system = brain.getPredictionSystem()
        self.planner = brain.getNeuralPlanner()
        self.concept_formation = brain.getConceptFormation()
        self.attention = brain.getAttention()
    
    def create_prediction_strategy(self, strategy_type='reactive'):
        """Create prediction strategy for different agent types."""
        if strategy_type == 'reactive':
            return self._create_reactive_strategy()
        elif strategy_type == 'anticipatory':
            return self._create_anticipatory_strategy()
        elif strategy_type == 'goal_oriented':
            return self._create_goal_oriented_strategy()
        elif strategy_type == 'creative':
            return self._create_creative_strategy()
        else:
            return self._create_default_strategy()
    
    def _create_reactive_strategy(self):
        """Reactive prediction based on immediate consequences."""
        return {
            'prediction_horizon': 1,  # Single step
            'prediction_update_rate': 10,  # Updates per second
            'confidence_threshold': 0.8,
            'exploration_bonus': 0.1,
            'exploitation_bonus': 0.9,
            'planning_enabled': False
        }
    
    def _create_anticipatory_strategy(self):
        """Anticipatory prediction with longer horizons."""
        return {
            'prediction_horizon': 10,  # 10 steps ahead
            'prediction_update_rate': 5,
            'confidence_threshold': 0.6,
            'exploration_bonus': 0.3,
            'exploitation_bonus': 0.7,
            'planning_enabled': True
        }
    
    def _create_goal_oriented_strategy(self):
        """Goal-oriented prediction with planning."""
        return {
            'prediction_horizon': 50,
            'prediction_update_rate': 2,
            'confidence_threshold': 0.5,
            'exploration_bonus': 0.2,
            'exploitation_bonus': 0.8,
            'planning_enabled': True,
            'goal_formation': True,
            'action_selection': 'planned'
        }
    
    def _create_creative_strategy(self):
        """Creative prediction that explores novel actions."""
        return {
            'prediction_horizon': 20,
            'prediction_update_rate': 3,
            'confidence_threshold': 0.4,
            'exploration_bonus': 0.7,
            'exploitation_bonus': 0.3,
            'planning_enabled': True,
            'novelty_detection': True,
            'curiosity_integration': True
        }
    
    def _create_default_strategy(self):
        """Default balanced prediction strategy."""
        return {
            'prediction_horizon': 5,
            'prediction_update_rate': 10,
            'confidence_threshold': 0.7,
            'exploration_bonus': 0.3,
            'exploitation_bonus': 0.7,
            'planning_enabled': True
        }
    
    def create_action_sequence(self, goal, max_steps=100, strategy='default'):
        """Create action sequence to achieve goal."""
        if not self.planner:
            return []
        
        # Get prediction strategy
        prediction_strategy = self.create_prediction_strategy(strategy)
        
        # Create action sequence (implementation depends on planner API)
        action_sequence = self._generate_action_sequence(
            goal, max_steps, prediction_strategy
        )
        
        return action_sequence
    
    def _generate_action_sequence(self, goal, max_steps, strategy):
        """Generate action sequence for goal achievement."""
        # This would use the neural planner to generate sequences
        # For demonstration, we'll return a structured format
        sequence = []
        
        for step in range(max_steps):
            # Generate action based on current state and goal
            action = self._select_action_for_goal(goal, step, strategy)
            sequence.append({
                'step': step,
                'action': action,
                'predicted_outcome': self._predict_action_outcome(action, step),
                'confidence': self._predict_action_confidence(action, step)
            })
            
            if action == 'goal_achieved':
                break
        
        return sequence
    
    def _select_action_for_goal(self, goal, step, strategy):
        """Select action based on goal and strategy."""
        # Implementation depends on action selection APIs
        return 'default_action'
    
    def _predict_action_outcome(self, action, step):
        """Predict outcome of action."""
        # Implementation depends on prediction system APIs
        return {'reward': 0.0, 'risk': 0.0, 'novelty': 0.0}
    
    def _predict_action_confidence(self, action, step):
        """Predict confidence in action outcome."""
        # Implementation depends on prediction system APIs
        return 0.5
    
    def create_concept_from_experience(self, experience_data):
        """Create new concept from experience."""
        if not self.concept_formation:
            return None
        
        # Process experience to create concept
        concept = self._extract_concept_from_data(experience_data)
        return concept
    
    def _extract_concept_from_data(self, experience_data):
        """Extract concept from experience data."""
        # Implementation depends on concept formation APIs
        return {
            'features': [],
            'associations': [],
            'confidence': 0.0,
            'usage_count': 0
        }
    
    def create_attention_focus(self, priority_areas, focus_duration=10.0):
        """Create attention focus on specific areas."""
        if not self.attention:
            return None
        
        # Create attention focus (implementation depends on attention APIs)
        attention_focus = {
            'priority_areas': priority_areas,
            'focus_duration': focus_duration,
            'start_time': self.brain.getCurrentStep(),
            'strength': min(len(priority_areas) * 0.3, 1.0)
        }
        
        return attention_focus

# Usage
prediction_advanced = PredictionSystemAdvanced(brain)
creative_strategy = prediction_advanced.create_prediction_strategy('creative')
sequence = prediction_advanced.create_action_sequence('reach_goal', strategy='creative')
```

## Error Handling and Debugging

### Advanced Error Handling

```python
import pynlm

class AdvancedErrorHandler:
    """Advanced error handling for NLM applications."""
    
    def __init__(self):
        self.error_count = 0
        self.recovery_strategies = {}
        self.error_patterns = {}
    
    def register_error_handler(self, error_type, handler_func, recovery_strategies=None):
        """Register custom error handler."""
        self.error_patterns[error_type] = handler_func
        if recovery_strategies:
            self.recovery_strategies[error_type] = recovery_strategies
    
    def handle_error(self, error_type, context=None, recoverable=True):
        """Handle error with appropriate recovery strategy."""
        self.error_count += 1
        
        if error_type in self.error_patterns:
            handler = self.error_patterns[error_type]
            return handler(context)
        
        # Default error handling
        if recoverable and error_type in self.recovery_strategies:
            recovery_strategies = self.recovery_strategies[error_type]
            for strategy in recovery_strategies:
                try:
                    result = strategy(context)
                    if result:
                        return {'recovered': True, 'strategy': strategy.__name__}
                except:
                    continue
        
        return {'recovered': False, 'error_type': error_type}
    
    def create_brain_error_recovery(self, brain):
        """Create error recovery specific to brain operations."""
        def brain_initialization_handler(context):
            """Handle brain initialization errors."""
            if context and 'config' in context:
                config = context['config']
                # Try to fix configuration
                if not config.has('brain.neuron_count'):
                    config.set('brain.neuron_count', 100)
                return {'fixed_config': True}
            return False
        
        def brain_step_handler(context):
            """Handle brain step errors."""
            if context and 'step' in context:
                step = context['step']
                # Try to recover by resetting brain
                try:
                    brain.reset()
                    brain.initialize()
                    return {'recovered_by_reset': True}
                except:
                    pass
            return False
        
        self.register_error_handler('brain_initialization', brain_initialization_handler)
        self.register_error_handler('brain_step', brain_step_handler)
        
        return self

# Usage
error_handler = AdvancedErrorHandler().create_brain_error_recovery(brain)
```

### Debugging Tools

```python
import pynlm

class NLMDebugger:
    """Advanced debugging tools for NLM applications."""
    
    def __init__(self, brain=None):
        self.brain = brain
        self.debug_log = []
        self.performance_metrics = {}
    
    def log_event(self, event_type, details, timestamp=None):
        """Log debugging event."""
        if timestamp is None:
            import time
            timestamp = time.time()
        
        log_entry = {
            'timestamp': timestamp,
            'event_type': event_type,
            'details': details,
            'brain_state': self._capture_brain_state() if self.brain else None
        }
        
        self.debug_log.append(log_entry)
        return log_entry
    
    def capture_brain_snapshot(self):
        """Capture detailed brain state snapshot."""
        if not self.brain:
            return None
        
        snapshot = {
            'step': self.brain.getCurrentStep(),
            'simulation_time': self.brain.getSimulationTime(),
            'neuron_count': self.brain.getTotalNeuronCount(),
            'firing_count': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'memory_systems': self._capture_memory_systems(),
            'neuromodulation_systems': self._capture_neuromodulation_systems(),
            'prediction_systems': self._capture_prediction_systems(),
            'development_systems': self._capture_development_systems()
        }
        
        return snapshot
    
    def _capture_brain_state(self):
        """Capture current brain state for debugging."""
        return self.capture_brain_snapshot()
    
    def _capture_memory_systems(self):
        """Capture memory system states."""
        if not self.brain:
            return None
        
        return {
            'working_memory': self.brain.getWorkingMemory(),
            'episodic_memory': self.brain.getEpisodicMemory(),
            'semantic_memory': self.brain.getSemanticMemory(),
            'procedural_memory': self.brain.getProceduralMemory(),
            'associative_memory': self.brain.getAssociativeMemory()
        }
    
    def _capture_neuromodulation_systems(self):
        """Capture neuromodulation system states."""
        if not self.brain:
            return None
        
        return {
            'dopamine': self.brain.getDopamine(),
            'curiosity': self.brain.getCuriosity(),
            'novelty': self.brain.getNovelty(),
            'prediction_error': self.brain.getPredictionError()
        }
    
    def _capture_prediction_systems(self):
        """Capture prediction system states."""
        if not self.brain:
            return None
        
        return {
            'prediction_system': self.brain.getPredictionSystem(),
            'neural_planner': self.brain.getNeuralPlanner(),
            'concept_formation': self.brain.getConceptFormation(),
            'attention': self.brain.getAttention()
        }
    
    def _capture_development_systems(self):
        """Capture development system states."""
        if not self.brain:
            return None
        
        return {
            'development_system': self.brain.getDevelopmentSystem(),
            'synaptogenesis': self.brain.getSynaptogenesis(),
            'pruning': self.brain.getPruning(),
            'maturation': self.brain.getMaturation()
        }
    
    def analyze_performance(self, duration_steps=100):
        """Analyze performance over recent steps."""
        if not self.brain or len(self.debug_log) < duration_steps:
            return None
        
        recent_logs = self.debug_log[-duration_steps:]
        
        metrics = {
            'steps_per_second': len(recent_logs) / (duration_steps / 0.1),  # Assuming 0.1s per step
            'average_firing_rate': sum(log['brain_state']['average_firing_rate'] for log in recent_logs) / len(recent_logs) if recent_logs else 0.0,
            'memory_usage': self._calculate_memory_usage(recent_logs),
            'error_rate': self._calculate_error_rate(recent_logs),
            'stability': self._calculate_stability(recent_logs)
        }
        
        self.performance_metrics.update(metrics)
        return metrics
    
    def _calculate_memory_usage(self, logs):
        """Calculate memory usage from logs."""
        total_memory = 0.0
        for log in logs:
            if log['brain_state'] and 'memory_systems' in log['brain_state']:
                # Add memory usage from each system
                pass
        return total_memory
    
    def _calculate_error_rate(self, logs):
        """Calculate error rate from logs."""
        error_events = sum(1 for log in logs if log['event_type'] == 'error')
        return error_events / len(logs) if logs else 0.0
    
    def _calculate_stability(self, logs):
        """Calculate system stability from logs."""
        if len(logs) < 2:
            return 1.0
        
        # Calculate variance in firing rates
        firing_rates = [log['brain_state']['average_firing_rate'] for log in logs if log['brain_state']]
        if not firing_rates:
            return 0.0
        
        mean_rate = sum(firing_rates) / len(firing_rates)
        variance = sum((rate - mean_rate) ** 2 for rate in firing_rates) / len(firing_rates)
        std_dev = variance ** 0.5
        
        # Stability inversely related to coefficient of variation
        return 1.0 / (1.0 + std_dev / mean_rate) if mean_rate > 0 else 0.0
    
    def export_debug_log(self, filename):
        """Export debug log to file."""
        import json
        with open(filename, 'w') as f:
            json.dump(self.debug_log, f, indent=2, default=str)
    
    def import_debug_log(self, filename):
        """Import debug log from file."""
        import json
        with open(filename, 'r') as f:
            self.debug_log = json.load(f)

# Usage
```python
# Create debugger and log events
debugger = NLMDebugger(brain)
debugger.log_event('simulation_start', {'step': 0, 'neurons': brain.getTotalNeuronCount()})

# Capture snapshots
snapshot = debugger.capture_brain_snapshot()
debugger.log_event('snapshot', snapshot)

# Run simulation and capture debug data
for step in range(100):
    brain.step(step)
    debugger.log_event('step_complete', {'step': step, 'firing_rate': brain.getAverageFiringRate()})

# Analyze performance
performance = debugger.analyze_performance(50)
print("Performance metrics:", performance)

# Export debug log
debugger.export_debug_log('simulation_debug.json')
```

## Performance Optimization

### Advanced Optimization Techniques

```python
import pynlm

class PerformanceOptimizer:
    """Advanced performance optimization for NLM applications."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.optimization_stats = {}
    
    def profile_configuration(self, config_name, test_steps=1000):
        """Profile configuration for optimal performance."""
        import time
        
        # Create test configuration
        config = self._create_test_configuration(config_name)
        
        # Measure performance
        start_time = time.time()
        start_memory = self._get_memory_usage()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(test_steps):
            brain.step(step)
        
        end_time = time.time()
        end_memory = self._get_memory_usage()
        
        performance = {
            'config_name': config_name,
            'total_time': end_time - start_time,
            'steps_per_second': test_steps / (end_time - start_time),
            'memory_start': start_memory,
            'memory_end': end_memory,
            'memory_increase': end_memory - start_memory,
            'average_firing_rate': brain.getAverageFiringRate(),
            'final_firing_rate': brain.getAverageFiringRate()
        }
        
        self.optimization_stats[config_name] = performance
        return performance
    
    def _create_test_configuration(self, config_name):
        """Create test configuration based on name."""
        config = pynlm.createDefaultConfig()
        
        if config_name == 'fast':
            config.set('brain.neuron_count', 500)
            config.set('brain.synapse_density', 0.05)
            config.set('plasticity.stdp.enable', False)
            config.set('development.enable', False)
        
        elif config_name == 'balanced':
            config.set('brain.neuron_count', 1000)
            config.set('brain.synapse_density', 0.1)
            config.set('plasticity.stdp.enable', True)
            config.set('development.enable', True)
        
        elif config_name == 'high_performance':
            config.set('brain.neuron_count', 2000)
            config.set('brain.synapse_density', 0.15)
            config.set('plasticity.stdp.enable', True)
            config.set('plasticity.stdp.learning_rate', 0.001)
            config.set('development.enable', True)
        
        return config
    
    def _get_memory_usage(self):
        """Get current memory usage."""
        import psutil
        process = psutil.Process()
        return process.memory_info().rss / 1024 / 1024  # MB
    
    def optimize_brain_for_task(self, task_type='general'):
        """Optimize brain for specific task."""
        if task_type == 'general':
            return self._optimize_for_general()
        elif task_type == 'fast_learning':
            return self._optimize_for_fast_learning()
        elif task_type == 'stable_performance':
            return self._optimize_for_stable_performance()
        elif task_type == 'exploration':
            return self._optimize_for_exploration()
        else:
            return self._optimize_for_general()
    
    def _optimize_for_general(self):
        """General optimization for balanced performance."""
        config = pynlm.createDefaultConfig()
        config.set('brain.neuron_count', 1000)
        config.set('brain.synapse_density', 0.1)
        config.set('plasticity.stdp.enable', True)
        config.set('plasticity.hebbian.enable', True)
        config.set('development.enable', True)
        config.set('neuromod.dopamine.scale', 1.0)
        config.set('neuromod.curiosity.enable', True)
        
        return config
    
    def _optimize_for_fast_learning(self):
        """Optimization for fast learning."""
        config = pynlm.createDefaultConfig()
        config.set('brain.neuron_count', 2000)
        config.set('brain.synapse_density', 0.15)
        config.set('plasticity.stdp.enable', True)
        config.set('plasticity.stdp.learning_rate', 0.002)
        config.set('development.enable', True)
        config.set('neuromod.dopamine.scale', 1.2)
        config.set('neuromod.curiosity.enable', True)
        
        return config
    
    def _optimize_for_stable_performance(self):
        """Optimization for stable performance."""
        config = pynlm.createDefaultConfig()
        config.set('brain.neuron_count', 500)
        config.set('brain.synapse_density', 0.08)
        config.set('plasticity.stdp.enable', True)
        config.set('plasticity.stdp.learning_rate', 0.0005)
        config.set('development.enable', False)
        config.set('neuromod.dopamine.scale', 0.8)
        config.set('neuromod.curiosity.enable', False)
        
        return config
    
    def _optimize_for_exploration(self):
        """Optimization for exploration."""
        config = pynlm.createDefaultConfig()
        config.set('brain.neuron_count', 1500)
        config.set('brain.synapse_density', 0.12)
        config.set('plasticity.stdp.enable', True)
        config.set('development.enable', True)
        config.set('neuromod.dopamine.scale', 1.5)
        config.set('neuromod.curiosity.enable', True)
        config.set('neuromod.novelty.enable', True)
        
        return config
    
    def benchmark_agent_strategies(self, strategies, episodes=10):
        """Benchmark different agent strategies."""
        benchmarks = {}
        
        for strategy in strategies:
            results = self._run_agent_benchmark(strategy, episodes)
            benchmarks[strategy] = results
        
        return benchmarks
    
    def _run_agent_benchmark(self, strategy, episodes):
        """Run benchmark for specific strategy."""
        import time
        
        strategy_results = []
        
        for episode in range(episodes):
            # Create new brain for each episode
            brain = pynlm.createBrain(self._get_strategy_config(strategy))
            brain.initialize()
            
            # Run episode
            start_time = time.time()
            episode_results = self._run_single_episode(brain, strategy)
            end_time = time.time()
            
            episode_results['episode_time'] = end_time - start_time
            strategy_results.append(episode_results)
        
        # Calculate benchmarks
        benchmarks = {
            'strategy': strategy,
            'episodes': episodes,
            'total_time': sum(r['episode_time'] for r in strategy_results),
            'average_episode_time': sum(r['episode_time'] for r in strategy_results) / episodes,
            'average_reward': sum(r.get('total_reward', 0) for r in strategy_results) / episodes,
            'reward_variance': sum((r.get('total_reward', 0) - sum(r.get('total_reward', 0) for r in strategy_results)/episodes)**2 for r in strategy_results) / episodes),
            'stability': sum(1.0 / (1.0 + abs(r.get('reward_variance', 0))) for r in strategy_results) / episodes
        }
        
        return benchmarks
    
    def _get_strategy_config(self, strategy):
        """Get configuration for strategy."""
        if strategy == 'exploration':
            config = pynlm.createDefaultConfig()
            config.set('brain.neuron_count', 2000)
            config.set('neuromod.curiosity.enable', True)
            config.set('neuromod.novelty.enable', True)
            return config
        
        elif strategy == 'exploitation':
            config = pynlm.createDefaultConfig()
            config.set('brain.neuron_count', 1000)
            config.set('neuromod.curiosity.enable', False)
            config.set('neuromod.novelty.enable', False)
            return config
        
        else:  # balanced
            return pynlm.createDefaultConfig()
    
    def _run_single_episode(self, brain, strategy):
        """Run single episode for benchmark."""
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable appropriate subsystems
        if 'exploration' in strategy:
            agent.enableCuriosity(True)
            agent.enableNovelty(True)
        elif 'exploitation' in strategy:
            agent.enableCuriosity(False)
            agent.enableNovelty(False)
        else:
            agent.enableCuriosity(True)
            agent.enableNovelty(True)
        
        agent.enableRewardModulation(True)
        agent.enableDevelopment(True)
        
        # Run episode
        total_reward = 0.0
        for step in range(100):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            total_reward += reward
        
        return {
            'total_reward': total_reward,
            'average_reward': total_reward / 100,
            'final_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage(),
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel()
        }

# Usage
```python
# Create performance optimizer
optimizer = PerformanceOptimizer(brain, world, agent)

# Profile different configurations
fast_perf = optimizer.profile_configuration('fast', 500)
balanced_perf = optimizer.profile_configuration('balanced', 1000)
high_perf = optimizer.profile_configuration('high_performance', 500)

print("Performance comparison:")
for config_name, perf in [fast_perf, balanced_perf, high_perf]:
    print(f"{config_name}: {perf['steps_per_second']:.1f} steps/sec, {perf['memory_increase']:.1f} MB increase")

# Optimize for specific task
task_config = optimizer.optimize_brain_for_task('fast_learning')
```

## Next Steps for Advanced Users

### For Further Development

1. **Extend Python Bindings**: Add new Python functions and classes
2. **Create Custom Experiments**: Design specialized experiments
3. **Add Visualization Tools**: Create brain state visualization
4. **Implement Distributed Computing**: Scale to multiple machines
5. **Add Real-World Integration**: Connect to external sensors/actors

### Research Applications

1. **Memory Research**: Study memory encoding and consolidation
2. **Learning Dynamics**: Investigate plasticity mechanisms
3. **Developmental Studies**: Examine brain maturation processes
4. **Neuromodulation**: Study reward and curiosity systems
5. **Action Selection**: Analyze decision-making processes

### Educational Applications

1. **Neuroscience Teaching**: Demonstrate brain principles
2. **AI Education**: Teach neural computation concepts
3. **Programming Education**: Learn advanced C++/Python integration
4. **System Design**: Study complex system architecture

## Acknowledgments

This module builds upon:
- The original NLM research project
- Contributions from the neuroscience and AI communities
- Open-source libraries (pybind11, CMake, etc.)
- Educational materials and documentation
- Research papers on neural computation

## Future Enhancements

Planned improvements include:
- Enhanced visualization capabilities
- Real-time monitoring interfaces
- Cloud-based computing support
- Advanced debugging tools
- Performance profiling utilities
- Integration with external data sources

## Contact Information

For questions, issues, or collaboration:
1. Check GitHub issues for known problems
2. Review documentation for setup instructions
3. Join research community for discussions
4. Review code for implementation details

---

*This documentation is continuously updated as NLM evolves.*

Last updated: $(date)
Version: 0.1.0

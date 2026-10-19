# NLM Python API Enhancement Plan

## Overview

This document outlines a comprehensive plan to improve the Python API for the NLM (Neural Learning Machine) project. The current Python bindings expose only basic factory functions and core classes, while the C++ implementation has Phase 6 complete integration with advanced systems that are not yet exposed in Python.

## Priority Order for Implementation

### Phase 1: Core Advanced Systems (Critical)
1. **WorkingMemory, EpisodicMemory, AssociativeMemory**
2. **Neuromodulation System** (Dopamine, Novelty, Curiosity, PredictionError)
3. **PredictionSystem**
4. **NeuralPlanner, ConceptFormation, AttentionalSelection**

### Phase 2: Development & Plasticity (High Priority)
5. **DevelopmentSystem** with developmental stages
6. **Plasticity Systems** (STDP, Hebbian, StructuralPlasticity)

### Phase 3: System Integration (Medium Priority)
7. **Advanced Configuration System**
8. **Checkpoint System Improvements**
9. **Comprehensive System Introspection**

### Phase 4: User Experience (Medium Priority)
10. **Command-line Tools**
11. **Comprehensive Examples**
12. **Training and Learning APIs**

## Phase 1: Core Advanced Systems

### 1.1 Memory Systems

#### WorkingMemory Class
```python
class WorkingMemory:
    """Transient active storage of information with competition and decay."""
    
    def __init__(self):
        """Create an empty working memory system."""
        
    def store(self, neuron_id, value):
        """Store value for neuron in working memory.
        
        Args:
            neuron_id: Neuron identifier (NeuronId object)
            value: Float value to store
        """
        
    def retrieve(self, neuron_id):
        """Retrieve value for neuron from working memory.
        
        Args:
            neuron_id: Neuron identifier (NeuronId object)
            
        Returns:
            float: Stored value, or 0.0 if not present
        """
        
    def contains(self, neuron_id):
        """Check if neuron has item in working memory.
        
        Args:
            neuron_id: Neuron identifier (NeuronId object)
            
        Returns:
            bool: True if neuron has stored item
        """
        
    def clear(self):
        """Clear all items from working memory."""
        
    def get_capacity(self):
        """Get working memory capacity.
        
        Returns:
            int: Maximum number of items that can be stored
        """
        
    def get_current_size(self):
        """Get current number of stored items.
        
        Returns:
            int: Current number of items
        """
        
    def decay(self, decay_rate):
        """Apply decay to all stored items.
        
        Args:
            decay_rate: Decay rate (0.0 to 1.0)
        """
```

#### EpisodicMemory Class
```python
class EpisodicMemoryItem:
    """Container for episodic memory episode data."""
    
    def __init__(self, timestamp=0, neurons=None, values=None, metadata=""):
        """Create an episodic memory item.
        
        Args:
            timestamp: Simulation step when episode occurred
            neurons: List of NeuronId objects
            values: List of corresponding float values
            metadata: Optional metadata string
        """
        self.timestamp = timestamp
        self.neurons = neurons or []
        self.values = values or []
        self.metadata = metadata

class EpisodicMemory:
    """Storage of events and experiences with consolidation capabilities."""
    
    def __init__(self):
        """Create an empty episodic memory system."""
        
    def store_episode(self, episode):
        """Store an episodic memory episode.
        
        Args:
            episode: EpisodicMemoryItem object
        """
        
    def retrieve_episode(self, index):
        """Retrieve episode by index.
        
        Args:
            index: Episode index
            
        Returns:
            EpisodicMemoryItem: Retrieved episode
        """
        
    def get_episode_count(self):
        """Get total number of stored episodes.
        
        Returns:
            int: Number of episodes
        """
        
    def get_recent_episodes(self, count):
        """Get most recent episodes.
        
        Args:
            count: Number of episodes to retrieve
            
        Returns:
            list: List of EpisodicMemoryItem objects
        """
        
    def clear(self):
        """Clear all episodes."""
        
    def consolidate(self, relevance_threshold):
        """Consolidate relevant episodes to long-term storage.
        
        Args:
            relevance_threshold: Threshold for consolidation
        """
```

#### AssociativeMemory Class
```python
class AssociativeMemory:
    """Relationships between representations with Hebbian pattern associations."""
    
    def __init__(self):
        """Create an empty associative memory system."""
        
    def associate(self, neuron_a, neuron_b, strength):
        """Create association between two neurons.
        
        Args:
            neuron_a: First NeuronId object
            neuron_b: Second NeuronId object
            strength: Association strength (0.0 to 1.0)
        """
        
    def get_associations(self, neuron):
        """Get neurons associated with given neuron.
        
        Args:
            neuron: NeuronId object
            
        Returns:
            list: List of associated NeuronId objects
        """
        
    def get_association_strength(self, neuron_a, neuron_b):
        """Get strength of association between two neurons.
        
        Args:
            neuron_a: First NeuronId object
            neuron_b: Second NeuronId object
            
        Returns:
            float: Association strength (0.0 to 1.0)
        """
        
    def update_association(self, neuron_a, neuron_b, delta):
        """Update association strength.
        
        Args:
            neuron_a: First NeuronId object
            neuron_b: Second NeuronId object
            delta: Change in strength (-1.0 to 1.0)
        """
        
    def clear(self):
        """Clear all associations."""
```

### 1.2 Neuromodulation System

#### Neuromodulator Base Class
```python
class Neuromodulator:
    """Abstract base class for neuromodulatory signals."""
    
    def __init__(self, name):
        """Create a neuromodulator.
        
        Args:
            name: Human-readable name (e.g., "Dopamine")
        """
        
    def get_name(self):
        """Get neuromodulator name.
        
        Returns:
            str: Name of the neuromodulator
        """
        
    def get_level(self):
        """Get current neuromodulator level/concentration.
        
        Returns:
            float: Current level (0.0 to 1.0)
        """
        
    def set_level(self, level):
        """Set neuromodulator level.
        
        Args:
            level: New level (0.0 to 1.0)
        """
        
    def get_plasticity_factor(self):
        """Get current plasticity modulation factor.
        
        Returns:
            float: Plasticity factor (typically 0.5 to 2.0)
        """
        
    def update(self, dt):
        """Update neuromodulator state.
        
        Args:
            dt: Time step duration (seconds)
        """
```

#### Dopamine Class
```python
class Dopamine(Neuromodulator):
    """Dopamine neuromodulator for reward and reinforcement learning."""
    
    def __init__(self):
        """Create dopamine neuromodulator."""
        super().__init__("Dopamine")
        
    def signal_reward(self, reward):
        """Signal reward to the system.
        
        Args:
            reward: Reward value (typically 0.0 to 1.0)
        """
        
    def signal_reward_prediction_error(self, error):
        """Signal reward prediction error.
        
        Args:
            error: Prediction error value (-1.0 to 1.0)
        """
```

#### Curiosity Class
```python
class Curiosity(Neuromodulator):
    """Curiosity neuromodulator for exploration motivation."""
    
    def __init__(self):
        """Create curiosity neuromodulator."""
        super().__init__("Curiosity")
        
    def trigger_curiosity(self):
        """Trigger curiosity-driven exploration."""
        
    def get_curiosity_level(self):
        """Get current curiosity level.
        
        Returns:
            float: Curiosity level (0.0 to 1.0)
        """
```

#### Novelty Class
```python
class Novelty(Neuromodulator):
    """Novelty detection neuromodulator for novelty detection."""
    
    def __init__(self):
        """Create novelty detection system."""
        super().__init__("Novelty")
        
    def detect_novelty(self, input_data):
        """Detect novelty in input data.
        
        Args:
            input_data: Sensor data or neural pattern
            
        Returns:
            float: Novelty score (0.0 to 1.0)
        """
        
    def get_novelty_level(self):
        """Get current novelty level.
        
        Returns:
            float: Novelty level (0.0 to 1.0)
        """
```

#### PredictionError Class
```python
class PredictionError(Neuromodulator):
    """Prediction error signal for learning and adaptation."""
    
    def __init__(self):
        """Create prediction error signal."""
        super().__init__("PredictionError")
        
    def update_prediction_error(self, predicted, actual):
        """Update prediction error based on prediction and outcome.
        
        Args:
            predicted: Predicted value
            actual: Actual observed value
            
        Returns:
            float: Prediction error
        """
        
    def get_prediction_error(self):
        """Get current prediction error.
        
        Returns:
            float: Prediction error value (-1.0 to 1.0)
        """
```

### 1.3 Prediction System

#### PredictionSystem Class
```python
class PredictionSystem:
    """Predicts future sensory states and consequences."""
    
    def __init__(self):
        """Create prediction system."""
        
    def predict_next_state(self, current_state):
        """Make prediction for next timestep.
        
        Args:
            current_state: Current sensory state (list or array)
            
        Returns:
            SensoryInput: Predicted sensory input
        """
        
    def update_predictions(self, predicted, actual):
        """Update predictions based on actual observation.
        
        Args:
            predicted: Predicted SensoryInput
            actual: Actual SensoryInput
        """
        
    def get_prediction_error(self):
        """Get prediction error.
        
        Returns:
            float: Prediction error value (-1.0 to 1.0)
        """
        
    def get_confidence(self):
        """Get prediction confidence.
        
        Returns:
            float: Confidence value (0.0 to 1.0)
        """
        
    def get_error_history(self):
        """Get prediction error history.
        
        Returns:
            list: List of error values
        """
        
    def clear_history(self):
        """Clear prediction error history."""
        
    def train(self, observation):
        """Train prediction model.
        
        Args:
            observation: SensoryInput to train on
        """
```

### 1.4 Cognition Systems

#### NeuralPlanner Class
```python
class PlanningCandidate:
    """A possible action sequence considered during planning."""
    
    def __init__(self, actions=None, predicted_states=None, expected_reward=0.0, 
                 confidence=0.0, depth=0):
        """Create planning candidate.
        
        Args:
            actions: List of ActionType objects
            predicted_states: List of predicted state vectors
            expected_reward: Expected total reward
            confidence: Planning confidence (0.0 to 1.0)
            depth: Planning depth (number of steps)
        """
        self.actions = actions or []
        self.predicted_states = predicted_states or []
        self.expected_reward = expected_reward
        self.confidence = confidence
        self.depth = depth

class NeuralPlanner:
    """Plans multi-step actions using predictive neural dynamics."""
    
    def __init__(self):
        """Create neural planner."""
        
    def initialize(self, brain):
        """Initialize planner with brain reference.
        
        Args:
            brain: Brain object to reference
        """
        
    def plan_action(self, current_state, target_reward=0.5):
        """Plan next action given current state and goal.
        
        Args:
            current_state: Current state vector
            target_reward: Target reward (default 0.5)
            
        Returns:
            ActionType: Best action to take now
        """
        
    def evaluate_sequence(self, actions, start_state):
        """Evaluate a potential action sequence.
        
        Args:
            actions: List of ActionType objects
            start_state: Starting state vector
            
        Returns:
            PlanningCandidate: Evaluation result
        """
        
    def get_planning_depth(self):
        """Get number of planning steps.
        
        Returns:
            int: Planning depth
        """
        
    def set_planning_depth(self, depth):
        """Set planning depth.
        
        Args:
            depth: Number of planning steps
        """
        
    def get_planning_confidence(self):
        """Get planning confidence.
        
        Returns:
            float: Planning confidence (0.0 to 1.0)
        """
        
    def update_plan_quality(self, planned_actions, actual_actions, actual_reward):
        """Update plans based on actual outcome.
        
        Args:
            planned_actions: Planned action sequence
            actual_actions: Actual action sequence
            actual_reward: Actual reward received
        """
        
    def clear_cache(self):
        """Clear planning cache."""
        
    def set_action_quality(self, action, quality):
        """Set action quality from experience.
        
        Args:
            action: ActionType object
            quality: Quality score (0.0 to 1.0)
        """
        
    def get_current_goal(self):
        """Get current goal.
        
        Returns:
            list: Goal state vector
        """
        
    def set_current_goal(self, goal):
        """Set current goal.
        
        Args:
            goal: Goal state vector
        """
```

#### ConceptFormation Class
```python
class Concept:
    """Represents a discovered pattern or concept."""
    
    def __init__(self, name, pattern, frequency=0.0):
        """Create concept.
        
        Args:
            name: Concept name
            pattern: Pattern vector
            frequency: Occurrence frequency
        """
        self.name = name
        self.pattern = pattern
        self.frequency = frequency

class ConceptFormation:
    """Forms concepts from experience patterns."""
    
    def __init__(self):
        """Create concept formation system."""
        
    def initialize(self, brain):
        """Initialize with brain reference.
        
        Args:
            brain: Brain object to reference
        """
        
    def analyze_pattern(self, data):
        """Analyze data for patterns.
        
        Args:
            data: Input data vector
            
        Returns:
            Concept: Discovered concept
        """
        
    def get_concepts(self):
        """Get all discovered concepts.
        
        Returns:
            list: List of Concept objects
        """
        
    def get_concept(self, name):
        """Get specific concept by name.
        
        Args:
            name: Concept name
            
        Returns:
            Concept: Concept object, or None if not found
        """
        
    def update_concept(self, concept, reward):
        """Update concept based on reward signal.
        
        Args:
            concept: Concept object
            reward: Reward value
        """
```

#### AttentionalSelection Class
```python
class AttentionalSelection:
    """Manages competitive attention allocation."""
    
    def __init__(self):
        """Create attentional selection system."""
        
    def initialize(self, brain):
        """Initialize with brain reference.
        
        Args:
            brain: Brain object to reference
        """
        
    def select_attention(self, sensory_inputs):
        """Select which sensory input to attend to.
        
        Args:
            sensory_inputs: List of sensory inputs
            
        Returns:
            SensoryInput: Selected input
        """
        
    def get_attention_focus(self):
        """Get current attentional focus.
        
        Returns:
            int: Index of attended input
        """
        
    def get_attention_strength(self):
        """Get attentional strength.
        
        Returns:
            float: Attention strength (0.0 to 1.0)
        """
        
    def update_attention(self, reward, novelty):
        """Update attention based on reward and novelty.
        
        Args:
            reward: Reward signal
            novelty: Novelty signal
        """
```

## Phase 2: Development & Plasticity

### 2.1 Development System

#### DevelopmentSystem Class
```python
class DevelopmentSystem:
    """Orchestrates neural development with developmental stages."""
    
    def __init__(self):
        """Create development system."""
        
    def get_stage(self):
        """Get current developmental stage.
        
        Returns:
            DevelopmentalStage: Current stage
        """
        
    def set_stage(self, stage):
        """Set developmental stage.
        
        Args:
            stage: DevelopmentalStage enum value
        """
        
    def advance_stage(self):
        """Advance to next developmental stage."""
        
    def get_stage_name(self):
        """Get name of current stage.
        
        Returns:
            str: Stage name
        """
        
    def update(self, brain, current_step):
        """Update development system.
        
        Args:
            brain: Brain object to develop
            current_step: Current simulation step
        """
        
    def update_full(self, brain, rng, dt):
        """Update development with full parameters.
        
        Args:
            brain: Brain object to develop
            rng: RandomGenerator object
            dt: Time step duration
        """
        
    def get_plasticity_modifier(self):
        """Get plasticity modifier for current stage.
        
        Returns:
            float: Plasticity modifier (0.1 to 2.0)
        """
        
    def is_critical_period(self):
        """Check if in critical period.
        
        Returns:
            bool: True if in critical period
        """
        
    def get_critical_period_progress(self):
        """Get critical period progress.
        
        Returns:
            float: Progress (0.0 to 1.0)
        """
        
    def get_developmental_age(self):
        """Get developmental age in simulation time.
        
        Returns:
            float: Age in simulation time units
        """
        
    def set_developmental_age(self, age):
        """Set developmental age.
        
        Args:
            age: Age in simulation time units
        """
```

### 2.2 Plasticity Systems

#### PlasticityRule Base Class
```python
class PlasticityRule:
    """Abstract base for plasticity rules."""
    
    def __init__(self, name):
        """Create plasticity rule.
        
        Args:
            name: Rule name
        """
        
    def get_name(self):
        """Get rule name.
        
        Returns:
            str: Rule name
        """
        
    def should_fire(self, pre_spike, post_spike, synapse):
        """Check if rule should fire.
        
        Args:
            pre_spike: Pre-synaptic spike time
            post_spike: Post-synaptic spike time
            synapse: Synapse object
            
        Returns:
            bool: True if rule should fire
        """
        
    def compute_change(self, pre_spike, post_spike, synapse):
        """Compute synaptic weight change.
        
        Args:
            pre_spike: Pre-synaptic spike time
            post_spike: Post-synaptic spike time
            synapse: Synapse object
            
        Returns:
            float: Weight change
        """
```

#### STDP Class
```python
class STDP(PlasticityRule):
    """Spike-Timing-Dependent Plasticity rule."""
    
    def __init__(self):
        """Create STDP rule."""
        super().__init__("STDP")
        
    def get_learning_rate(self):
        """Get learning rate.
        
        Returns:
            float: Learning rate
        """
        
    def set_learning_rate(self, rate):
        """Set learning rate.
        
        Args:
            rate: Learning rate
        """
        
    def get_time_constant(self):
        """Get time constant.
        
        Returns:
            float: Time constant
        """
```

#### Hebbian Class
```python
class Hebbian(PlasticityRule):
    """Hebbian learning rule (fire together, wire together)."""
    
    def __init__(self):
        """Create Hebbian rule."""
        super().__init__("Hebbian")
        
    def get_learning_rate(self):
        """Get learning rate.
        
        Returns:
            float: Learning rate
        """
        
    def get_coincidence_threshold(self):
        """Get coincidence detection threshold.
        
        Returns:
            float: Threshold
        """
```

#### StructuralPlasticity Class
```python
class StructuralPlasticity:
    """Creates new neural connections (structural changes)."""
    
    def __init__(self):
        """Create structural plasticity system."""
        
    def enable_growth(self, enable):
        """Enable or disable structural growth.
        
        Args:
            enable: True to enable growth
        """
        
    def is_enabled(self):
        """Check if structural growth is enabled.
        
        Returns:
            bool: True if enabled
        """
        
    def grow_connection(self, source_region, target_region):
        """Grow new connection between regions.
        
        Args:
            source_region: RegionId object
            target_region: RegionId object
            
        Returns:
            SynapseId: New synapse ID
        """
        
    def get_connection_count(self):
        """Get number of structural connections.
        
        Returns:
            int: Connection count
        """
        
    def prune_connection(self, synapse_id):
        """Prune structural connection.
        
        Args:
            synapse_id: SynapseId to prune
        """
```

## Phase 3: System Integration

### 3.1 Advanced Configuration System

#### AdvancedConfig Class
```python
class AdvancedConfig:
    """Enhanced configuration system with JSON support and validation."""
    
    def __init__(self, config=None):
        """Create advanced configuration.
        
        Args:
            config: Existing Config object (optional)
        """
        
    @classmethod
    def load_from_file(cls, filepath):
        """Load configuration from JSON file.
        
        Args:
            filepath: Path to JSON configuration file
            
        Returns:
            AdvancedConfig: Loaded configuration
        """
        
    @classmethod
    def load_from_dict(cls, config_dict):
        """Load configuration from dictionary.
        
        Args:
            config_dict: Configuration dictionary
            
        Returns:
            AdvancedConfig: Loaded configuration
        """
        
    def save_to_file(self, filepath, indent=2):
        """Save configuration to JSON file.
        
        Args:
            filepath: Output file path
            indent: JSON indentation
        """
        
    def get(self, key, default=None, data_type=None):
        """Get configuration value with type conversion.
        
        Args:
            key: Configuration key
            default: Default value if key not found
            data_type: Type conversion function
            
        Returns:
            Configuration value with type conversion
        """
        
    def set(self, key, value, config_source=ConfigSource.RUNTIME):
        """Set configuration value.
        
        Args:
            key: Configuration key
            value: Configuration value
            config_source: Source of configuration
        """
        
    def validate(self, schema=None):
        """Validate configuration against schema.
        
        Args:
            schema: JSON schema for validation
            
        Returns:
            list: List of validation errors (empty if valid)
        """
        
    def merge(self, other_config):
        """Merge with another configuration.
        
        Args:
            other_config: Another AdvancedConfig object
        """
        
    def get_config_summary(self):
        """Get configuration summary.
        
        Returns:
            dict: Configuration summary statistics
        """
```

### 3.2 Checkpoint System

#### CheckpointManager Class
```python
class CheckpointManager:
    """Manages brain state checkpointing and recovery."""
    
    def __init__(self, brain):
        """Create checkpoint manager.
        
        Args:
            brain: Brain object to manage checkpoints for
        """
        
    def save_checkpoint(self, filepath, description=""):
        """Save brain state as checkpoint.
        
        Args:
            filepath: Checkpoint file path
            description: Optional checkpoint description
        """
        
    def load_checkpoint(self, filepath):
        """Load brain state from checkpoint.
        
        Args:
            filepath: Checkpoint file path
            
        Returns:
            bool: True if successful
        """
        
    def get_checkpoint_list(self, directory="."):
        """Get list of available checkpoints.
        
        Args:
            directory: Directory to search
            
        Returns:
            list: List of checkpoint file paths
        """
        
    def delete_checkpoint(self, filepath):
        """Delete checkpoint.
        
        Args:
            filepath: Checkpoint file path
        """
        
    def auto_save(self, interval_steps=1000):
        """Enable automatic checkpoint saving.
        
        Args:
            interval_steps: Save every N steps
        """
        
    def get_last_checkpoint_info(self, filepath):
        """Get checkpoint information.
        
        Args:
            filepath: Checkpoint file path
            
        Returns:
            dict: Checkpoint metadata (step, time, description, etc.)
        """
```

### 3.3 Comprehensive System Introspection

#### SystemMonitor Class
```python
class SystemMonitor:
    """Comprehensive system monitoring and introspection."""
    
    def __init__(self, brain):
        """Create system monitor.
        
        Args:
            brain: Brain object to monitor
        """
        
    def get_neural_statistics(self):
        """Get comprehensive neural statistics.
        
        Returns:
            dict: Neural statistics including:
                - neuron_count, synapse_count
                - firing_rate, excitation_inhibition_ratio
                - total_spike_count, active_neuron_count
                - regional_distribution
        """
        
    def get_memory_statistics(self):
        """Get memory system statistics.
        
        Returns:
            dict: Memory system statistics
        """
        
    def get_plasticity_statistics(self):
        """Get plasticity system statistics.
        
        Returns:
            dict: Plasticity system statistics
        """
        
    def get_development_statistics(self):
        """Get development system statistics.
        
        Returns:
            dict: Development system statistics
        """
        
    def get_prediction_statistics(self):
        """Get prediction system statistics.
        
        Returns:
            dict: Prediction system statistics
        """
        
    def get_neuromodulation_statistics(self):
        """Get neuromodulation system statistics.
        
        Returns:
            dict: Neuromodulation system statistics
        """
        
    def get_system_health(self):
        """Get overall system health metrics.
        
        Returns:
            dict: System health metrics
        """
        
    def get_performance_metrics(self):
        """Get performance metrics.
        
        Returns:
            dict: Performance metrics
        """
        
    def export_monitoring_data(self, filepath):
        """Export monitoring data to file.
        
        Args:
            filepath: Output file path
        """
        
    def subscribe_to_updates(self, callback, event_types=None):
        """Subscribe to system updates.
        
        Args:
            callback: Callback function
            event_types: List of event types to subscribe to
        """
```

## Phase 4: User Experience

### 4.1 Command-Line Tools

#### NLM Command Line Interface

```python
# nlm_cli.py - Main CLI implementation

import argparse
import json
from pathlib.com import Path
from pynlm import (
    createBrain, createDefaultConfig, createSimpleWorld, createAgentBrain
)

def create_brain_from_config(config_file=None):
    """Create brain from configuration file or default."""
    if config_file and Path(config_file).exists():
        config = createDefaultConfig()
        # Load config file (would need advanced config support)
        brain = createBrain(config)
    else:
        brain = createBrain(createDefaultConfig())
    brain.initialize()
    return brain

def run_simulation(brain, world, agent, steps, enable_learning=False):
    """Run complete simulation loop."""
    if enable_learning:
        agent.enable_reward_modulation(True)
        agent.enable_curiosity(True)
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain, world, agent

def save_brain_state(brain, filepath):
    """Save brain state to file."""
    brain.save(filepath)
    print(f"Brain state saved to {filepath}")

def load_brain_state(filepath):
    """Load brain state from file."""
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.load(filepath)
    print(f"Brain state loaded from {filepath}")
    return brain

def analyze_brain(brain, output_file=None):
    """Analyze brain state and optionally save results."""
    stats = {
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'firing_count': brain.getFiringNeuronCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'exc_inh_ratio': brain.getExcitationInhibitionRatio(),
        'total_spikes': brain.getTotalSpikeCount(),
        'developmental_stage': brain.getDevelopmentalStage(),
    }
    
    if output_file:
        with open(output_file, 'w') as f:
            json.dump(stats, f, indent=2)
    
    return stats

def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface"
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Command')
    
    # Brain creation
    brain_parser = subparsers.add_parser('create', help='Create brain')
    brain_parser.add_argument('--config', help='Configuration file')
    brain_parser.add_argument('--output', help='Output brain file')
    
    # Simulation
    sim_parser = subparsers.add_parser('simulate', help='Run simulation')
    sim_parser.add_argument('--brain', required=True, help='Brain file')
    sim_parser.add_argument('--world-size', nargs=2, type=int, default=[10, 10],
                          help='World dimensions (width height)')
    sim_parser.add_argument('--steps', type=int, default=1000, help='Number of steps')
    sim_parser.add_argument('--learning', action='store_true',
                          help='Enable learning features')
    sim_parser.add_argument('--output', help='Output brain file')
    
    # Analysis
    analyze_parser = subparsers.add_parser('analyze', help='Analyze brain')
    analyze_parser.add_argument('--brain', required=True, help='Brain file')
    analyze_parser.add_argument('--output', help='Output analysis file')
    
    # Checkpoint management
    checkpoint_parser = subparsers.add_parser('checkpoint', help='Manage checkpoints')
    checkpoint_subparsers = checkpoint_parser.add_subparsers(dest='checkpoint_action')
    
    save_checkpoint = checkpoint_subparsers.add_parser('save', help='Save checkpoint')
    save_checkpoint.add_argument('--brain', required=True, help='Brain file')
    save_checkpoint.add_argument('--output', required=True, help='Checkpoint file')
    save_checkpoint.add_argument('--desc', help='Checkpoint description')
    
    load_checkpoint = checkpoint_subparsers.add_parser('load', help='Load checkpoint')
    load_checkpoint.add_argument('--input', required=True, help='Checkpoint file')
    load_checkpoint.add_argument('--output', required=True, help='Output brain file')
    
    # Configuration management
    config_parser = subparsers.add_parser('config', help='Manage configuration')
    config_subparsers = config_parser.add_subparsers(dest='config_action')
    
    export_config = config_subparsers.add_parser('export', help='Export config')
    export_config.add_argument('--config', required=True, help='Config file')
    export_config.add_argument('--output', help='Output config file')
    
    import_config = config_subparsers.add_parser('import', help='Import config')
    import_config.add_argument('--input', required=True, help='Input config file')
    import_config.add_argument('--output', required=True, help='Output config file')
    
    # Version and info
    info_parser = subparsers.add_parser('info', help='Show information')
    info_parser.add_argument('--brain', help='Brain file for info')
    
    args = parser.parse_args()
    
    if args.command == 'create':
        brain = create_brain_from_config(args.config)
        if args.output:
            brain.save(args.output)
            
    elif args.command == 'simulate':
        # Load brain
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.load(args.brain)
        
        # Create world and agent
        world = createSimpleWorld()
        world.configure(args.world_size[0], args.world_size[1], 8, 8)
        world.reset()
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        brain, world, agent = run_simulation(
            brain, world, agent, args.steps, args.learning
        )
        
        if args.output:
            brain.save(args.output)
            
    elif args.command == 'analyze':
        config = createDefaultConfig()
        brain = createBrain(config)
        brain.load(args.brain)
        stats = analyze_brain(brain, args.output)
        print("Brain Analysis Results:")
        for key, value in stats.items():
            print(f"  {key}: {value}")
            
    elif args.command == 'checkpoint':
        if args.checkpoint_action == 'save':
            config = createDefaultConfig()
            brain = createBrain(config)
            brain.load(args.brain)
            brain.save(args.output)
            
        elif args.checkpoint_action == 'load':
            config = createDefaultConfig()
            brain = createBrain(config)
            brain.load(args.input)
            if args.output:
                brain.save(args.output)
                
    elif args.command == 'config':
        if args.config_action == 'export':
            # Configuration export logic
            pass
            
        elif args.config_action == 'import':
            # Configuration import logic
            pass
            
    elif args.command == 'info':
        if args.brain:
            config = createDefaultConfig()
            brain = createBrain(config)
            brain.load(args.brain)
            analyze_brain(brain)
        else:
            print("NLM - Neural Learning Machine")
            print("Version: 0.1.0")
            print("Python bindings for neural simulation framework")
    
    return 0

if __name__ == '__main__':
    exit(main())
```

### 4.2 Comprehensive Examples

#### Example 1: Working Memory and Attention

```python
import pynlm
import numpy as np

def working_memory_example():
    """Example demonstrating working memory and attention."""
    # Create brain and world
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Create working memory system
    working_memory = pynlm.WorkingMemory()
    
    print("Working Memory Example")
    print("=====================")
    
    # Get some neuron IDs
    neuron1 = None
    neuron2 = None
    # In practice, you would get actual neuron IDs from the brain
    
    # Store information in working memory
    working_memory.store(neuron1, 0.8)
    working_memory.store(neuron2, 0.3)
    
    # Retrieve information
    value1 = working_memory.retrieve(neuron1)
    value2 = working_memory.retrieve(neuron2)
    
    print(f"Stored value for neuron1: {value1}")
    print(f"Stored value for neuron2: {value2}")
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Working memory influences attention
        attention_focus = working_memory.get_current_focus()
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
    
    print(f"Simulation complete. Working memory size: {working_memory.get_current_size()}")
    return brain, world, agent

if __name__ == "__main__":
    working_memory_example()
```

#### Example 2: Neuromodulation and Learning

```python
import pynlm

def neuromodulation_example():
    """Example demonstrating neuromodulation systems."""
    print("Neuromodulation Example")
    print("======================")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create neuromodulation systems
    dopamine = pynlm.Dopamine()
    curiosity = pynlm.Curiosity()
    novelty = pynlm.Novelty()
    prediction_error = pynlm.PredictionError()
    
    # Enable neuromodulation
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Create world and run simulation
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
    world.reset()
    
    print("Running simulation with neuromodulation...")
    
    for step in range(200):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent (triggers neuromodulation)
        agent.process_sensory_input(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Check neuromodulation levels
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Dopamine level: {dopamine.get_level():.3f}")
            print(f"  Curiosity level: {curiosity.get_curiosity_level():.3f}")
            print(f"  Novelty level: {novelty.get_novelty_level():.3f}")
            print(f"  Prediction error: {prediction_error.get_prediction_error():.3f}")
    
    print("Neuromodulation simulation complete!")
    return brain, world, agent, {
        'dopamine': dopamine,
        'curiosity': curiosity,
        'novelty': novelty,
        'prediction_error': prediction_error
    }

if __name__ == "__main__":
    neuromodulation_example()
```

#### Example 3: Prediction and Planning

```python
import pynlm
import numpy as np

def prediction_planning_example():
    """Example demonstrating prediction and planning systems."""
    print("Prediction and Planning Example")
    print("=============================")
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create prediction and planning systems
    prediction_system = pynlm.PredictionSystem()
    planner = pynlm.NeuralPlanner()
    
    # Initialize planning system with brain
    planner.initialize(brain)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print("Running prediction and planning simulation...")
    
    for step in range(300):
        # Get current state
        percept = world.getSensoryPercept()
        
        # Convert to state vector for planning
        current_state = percept.get_vision()
        
        # Plan action
        action = planner.plan_action(current_state)
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Brain step (includes prediction)
        brain.step(step)
        
        # Get action
        decoded_action = agent.decode_motor_command()
        world.apply_motor_command(decoded_action, world.get_simulation_time())
        
        # Check planning metrics
        if step % 100 == 0:
            confidence = planner.get_planning_confidence()
            depth = planner.get_planning_depth()
            print(f"Step {step}:")
            print(f"  Planning confidence: {confidence:.3f}")
            print(f"  Planning depth: {depth}")
            
            # Get prediction error
            pred_error = prediction_system.get_prediction_error()
            print(f"  Prediction error: {pred_error:.3f}")
    
    print("Prediction and planning simulation complete!")
    return brain, world, agent, prediction_system, planner

if __name__ == "__main__":
    prediction_planning_example()
```

### 4.3 Training and Learning APIs

#### TrainingSession Class
```python
class TrainingSession:
    """Manages training and learning scenarios."""
    
    def __init__(self, brain, world, agent):
        """Create training session.
        
        Args:
            brain: Brain object to train
            world: World object for environment
            agent: AgentBrain object for interaction
        """
        self.brain = brain
        self.world = world
        self.agent = agent
        self.step_count = 0
        self.reward_history = []
        self.action_history = []
        self.state_history = []
        
    def add_training_scenario(self, scenario_type, **kwargs):
        """Add a training scenario.
        
        Args:
            scenario_type: Type of scenario ('reward', 'exploration', 'novelty', etc.)
            **kwargs: Scenario parameters
        """
        scenarios = {
            'reward': self._reward_based_training,
            'exploration': self._exploration_training,
            'novelty': self._novelty_training,
            'prediction': self._prediction_training,
            'social': self._social_learning,
        }
        
        if scenario_type in scenarios:
            scenarios[scenario_type](**kwargs)
        else:
            raise ValueError(f"Unknown scenario type: {scenario_type}")
    
    def _reward_based_training(self, reward_schedule, goal_reach=None):
        """Reward-based learning scenario."""
        print(f"Starting reward-based training with goal: {goal_reach}")
        # Implementation would use Dopamine neuromodulation
        
    def _exploration_training(self, exploration_rate=0.3, target_curiosity=0.8):
        """Exploration-driven learning scenario."""
        print(f"Starting exploration training with rate: {exploration_rate}")
        # Implementation would use Curiosity neuromodulation
        
    def _novelty_training(self, novelty_threshold=0.7):
        """Novelty-driven learning scenario."""
        print(f"Starting novelty training with threshold: {novelty_threshold}")
        # Implementation would use Novelty detection
        
    def _prediction_training(self, prediction_error_threshold=0.5):
        """Prediction-based learning scenario."""
        print(f"Starting prediction training with threshold: {prediction_error_threshold}")
        # Implementation would use PredictionError
        
    def _social_learning(self, social_interactions=10):
        """Social learning scenario."""
        print(f"Starting social learning with {social_interactions} interactions")
        # Implementation would use SocialLearning system
    
    def run_training_episode(self, steps, scenario_type='reward'):
        """Run a single training episode.
        
        Args:
            steps: Number of steps to run
            scenario_type: Type of training scenario
            
        Returns:
            dict: Training metrics
        """
        print(f"Running {scenario_type} training episode ({steps} steps)...")
        
        for step in range(steps):
            # World update
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.get_sensory_percept()
            
            # Process with agent
            self.agent.process_sensory_input(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decode_motor_command()
            self.world.apply_motor_command(action, self.world.get_simulation_time())
            
            # Record metrics
            self.action_history.append(action)
            self.state_history.append(percept)
            
            # For simplicity, add some reward
            reward = 1.0 if step % 10 == 0 else 0.1
            self.reward_history.append(reward)
            
            if step % 50 == 0:
                print(f"  Step {step}, Reward: {reward:.3f}")
        
        # Calculate metrics
        metrics = {
            'total_reward': sum(self.reward_history),
            'avg_reward': np.mean(self.reward_history),
            'steps': steps,
            'action_distribution': self._calculate_action_distribution()
        }
        
        print(f"Training episode complete. Total reward: {metrics['total_reward']:.3f}")
        return metrics
    
    def _calculate_action_distribution(self):
        """Calculate action distribution statistics.
        
        Returns:
            dict: Action distribution
        """
        # Implementation would depend on action types
        return {}
    
    def export_training_data(self, filepath):
        """Export training data for analysis.
        
        Args:
            filepath: Output file path
        """
        # Implementation would save training data
        pass
```

#### Training Examples

```python
import pynlm
import numpy as np

def reward_based_learning_example():
    """Example of reward-based learning training."""
    print("Reward-Based Learning Example")
    print("============================")
    
    # Create simulation components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Create training session
    session = pynlm.TrainingSession(brain, world, agent)
    
    # Add training scenarios
    session.add_training_scenario('reward', goal_reach=100)
    session.add_training_scenario('exploration', exploration_rate=0.3)
    
    # Run training episodes
    for episode in range(3):
        print(f"\nEpisode {episode + 1}:")
        metrics = session.run_training_episode(200, 'reward')
        
        print(f"  Episode metrics:")
        print(f"    Total reward: {metrics['total_reward']:.3f}")
        print(f"    Average reward: {metrics['avg_reward']:.3f}")
    
    print("\nReward-based learning training complete!")
    return brain, world, agent

def curiosity_driven_learning_example():
    """Example of curiosity-driven exploration training."""
    print("\nCuriosity-Driven Learning Example")
    print("================================")
    
    # Create simulation components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enable_curiosity(True)
    agent.enable_reward_modulation(True)
    
    # Create curiosity neuromodulator
    curiosity = pynlm.Curiosity()
    
    print("Running curiosity-driven exploration...")
    
    # Run exploration
    for step in range(400):
        world.update(0.1)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process with agent (curiosity affects exploration)
        agent.process_sensory_input(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Check curiosity level periodically
        if step % 100 == 0:
            curiosity_level = curiosity.get_curiosity_level()
            print(f"Step {step}: Curiosity level = {curiosity_level:.3f}")
            
            # Adjust exploration based on curiosity
            if curiosity_level > 0.7:
                print("  High curiosity - encouraging exploration")
            elif curiosity_level < 0.3:
                print("  Low curiosity - encouraging exploitation")
    
    print("\nCuriosity-driven learning complete!")
    return brain, world, agent

if __name__ == "__main__":
    reward_based_learning_example()
    curiosity_driven_learning_example()
```

## Implementation Notes

### 1. Backward Compatibility
- All new classes should be optional imports
- Existing code should continue to work without changes
- New classes should not break existing API contracts

### 2. Integration Strategy
- New C++ bindings should be added to `bindings.cpp`
- Each new system should have corresponding Python class
- Classes should mirror C++ interfaces where possible
- Documentation should be updated alongside code changes

### 3. Testing
- Unit tests for each new Python class
- Integration tests for complex scenarios
- Performance benchmarks for new functionality
- Example tests demonstrating usage patterns

### 4. Documentation
- API documentation for all new classes
- Comprehensive examples in separate files
- Command-line tool documentation
- Training and learning scenario documentation

## Next Steps

1. **Phase 1 Implementation**: Begin with memory systems and neuromodulation
2. **Phase 2 Implementation**: Implement development and plasticity systems
3. **Phase 3 Integration**: Add configuration, checkpoint, and introspection systems
4. **Phase 4 Development**: Create command-line tools and examples
5. **Testing and Validation**: Thoroughly test all new functionality
6. **Documentation**: Complete documentation for all new features

This plan provides a comprehensive roadmap for significantly enhancing the Python API for the NLM project, exposing all Phase 6 integrated systems while maintaining backward compatibility and providing comprehensive user experience features.
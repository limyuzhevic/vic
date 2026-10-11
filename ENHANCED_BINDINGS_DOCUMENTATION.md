# Enhanced NLM Python Bindings Documentation

This document provides comprehensive documentation for the enhanced NLM Python bindings.

## Overview

The enhanced NLM Python bindings provide a complete, Pythonic interface to the NLM (Neural Learning Machine) C++ framework. This version exposes all 12 core systems, including advanced cognitive and memory systems that were previously missing from the Python API.

### Key Improvements

1. **Complete System Coverage**: All 12 NLM systems are now exposed to Python
2. **Pythonic API Design**: Snake_case method names and convenience functions
3. **Comprehensive Error Handling**: Custom exception classes for better error management
4. **Rich Documentation**: Extensive docstrings and examples
5. **Factory Functions**: Easy object creation with `create_*` functions
6. **Memory Management**: Automatic memory management with `shared_ptr`

## System Categories

### Core Systems (4)
- **Config**: Configuration management
- **Brain**: Central neural simulation brain
- **AgentBrain**: Agent brain interface connecting NLM to world
- **SimpleWorld**: Simple 2D world simulation

### Sensory Systems (3)
- **Vision**: Visual sensory input with pixel-level access
- **Audio**: Audio sensory input with duration calculation
- **InternalSignals**: Internal neural signal processing

### Motor Systems (2)
- **Action**: Motor action representation with parameter management
- **AgentBody**: Agent body state with movement and physics

### Advanced Cognitive Systems (6)
- **NeuralPlanner**: Action planning through predictive dynamics
- **ConceptFormation**: Pattern discovery and concept emergence
- **SelfModel**: Internal self-representation and prediction
- **SocialLearning**: Observation learning from other agents
- **SpatialRepresentation**: Spatial reasoning and place cells
- **TemporalRelation**: Sequence learning and temporal prediction

### Memory Systems (3)
- **NeuralWorkingMemory**: Transient active information maintenance
- **NeuralEpisodicMemory**: Experience storage and replay
- **NeuralAssociativeMemory**: Pattern associations and spreading activation

### Prediction System (1)
- **PredictionSystem**: Sensory prediction and error computation (placeholder)

## Pythonic API Features

### Naming Conventions
- Method names use snake_case (e.g., `set_planning_depth`, not `setPlanningDepth`)
- Parameter names are descriptive and Pythonic
- Variable names follow Python conventions

### Convenience Methods
- Factory functions: `create_neural_planner()`, `create_concept_formation()`
- Helper methods: `is_novel()`, `can_imitate()`, `get_system_status()`
- Utility functions: `validate_bindings()`, `get_system_info()`

### Error Handling
Custom exception hierarchy:
- `NLMError`: Base exception for all NLM errors
- `ConfigurationError`: Configuration-related errors
- `MemoryError`: Memory-related errors
- `PlanningError`: Planning-related errors

### Enhanced Enum Support
- Enums have helper methods like `is_excitatory()`, `is_movement()`
- Rich `__repr__()` and `__str__()` methods
- Better documentation

### Factory Functions
Easy object creation:
```python
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
world = pynlm.create_simple_world()
agent = pynlm.create_agent_brain(brain)

# Advanced systems
planner = pynlm.create_neural_planner()
concept_formation = pynlm.create_concept_formation()
self_model = pynlm.create_self_model()
social_learning = pynlm.create_social_learning()
spatial = pynlm.create_spatial_representation()
temporal = pynlm.create_temporal_relation()
working_memory = pynlm.create_neural_working_memory()
episodic_memory = pynlm.create_neural_episodic_memory()
associative_memory = pynlm.create_neural_associative_memory()
prediction_system = pynlm.create_prediction_system()
```

### Complete Agent Setup
```python
# Create a complete agent with all systems
agent = pynlm.create_complete_agent()
world = pynlm.create_simple_world()
config = pynlm.create_default_config()

# Configure the world
world.configure(100, 100, 20, 20)  # width, height, vision_width, vision_height
world.set_agent_start(50, 50)

# Initialize all cognitive systems
planner = pynlm.create_neural_planner()
concept_formation = pynlm.create_concept_formation()
self_model = pynlm.create_self_model()
social_learning = pynlm.create_social_learning()
spatial = pynlm.create_spatial_representation()
temporal = pynlm.create_temporal_relation()
working_memory = pynlm.create_neural_working_memory()
episodic_memory = pynlm.create_neural_episodic_memory()
associative_memory = pynlm.create_neural_associative_memory()
prediction_system = pynlm.create_prediction_system()

# Initialize all systems with the brain
brain = pynlm.create_brain(config)
planner.initialize(brain)
concept_formation.initialize(brain)
self_model.initialize(brain)
social_learning.initialize(brain)
spatial.initialize(brain)
temporal.initialize(brain)
working_memory.initialize(brain)
episodic_memory.initialize(brain)
associative_memory.initialize(brain)
prediction_system.initialize(brain)
```

## Advanced System Usage Examples

### NeuralPlanner Usage
```python
# Create and initialize planner
planner = pynlm.create_neural_planner()
planner.set_planning_depth(5)  # Look ahead 5 steps
planner.set_current_goal([0.5, 0.5, 0.0])  # Target position

# Plan an action
action_type = planner.plan_action(current_state)
action = pynlm.Action(action_type)

# Evaluate sequence
candidate = planner.evaluate_sequence([ActionType.MoveForward, ActionType.TurnRight], start_state)
print(f"Planned sequence quality: {candidate.expected_reward:.3f}, confidence: {candidate.confidence:.3f}")

# Update planning quality based on outcomes
planner.update_plan_quality([ActionType.MoveForward, ActionType.TurnRight], 
                           [ActionType.MoveForward, ActionType.TurnLeft], 0.8)

# Check planning status
if planner.was_recent_plan_successful():
    print("Recent planning was successful")
```

### ConceptFormation Usage
```python
# Create and initialize concept formation
concept_system = pynlm.create_concept_formation()
concept_system.set_formation_threshold(0.8)
concept_system.set_stability_window(10)

# Present experiences
concept_id = concept_system.present_experience(pattern, features, reward, time_step)
if concept_id > 0:
    print(f"New concept discovered: {concept_id}")
else:
    print("Experience matched existing concept")

# Get concept information
concept = concept_system.get_concept(concept_id)
if concept:
    stability = concept_system.get_concept_stability(concept_id)
    instances = concept_system.get_concept_instances(concept_id)
    print(f"Concept stability: {stability:.3f}, instances: {len(instances)}")

# Check for novelty
if concept_system.is_novel(new_pattern):
    print("New pattern is novel")
else:
    print("Pattern belongs to existing concept")

# Generalization ability
if concept_id > 0:
    generalization = concept_system.get_generalization_ability(concept_id)
    print(f"Concept generalization ability: {generalization:.3f}")
```

### Memory Systems Usage
```python
# Working Memory
working_mem = pynlm.create_neural_working_memory()
working_mem.set_capacity(10)
working_mem.set_decay_rate(0.1)

# Store and retrieve information
working_mem.store([0.1, 0.5, 0.9, 0.2])
retrieved = working_mem.retrieve()
print(f"Working memory content: {retrieved}")

# Episodic Memory
episodic_mem = pynlm.create_neural_episodic_memory()
episodic_mem.enable_replay(True)

# Create and store episode
episode = nlm.EpisodicMemoryItem()
episode.sensory_state = [0.1, 0.5, 0.9]
episode.action = pynlm.ActionType.MoveForward
episode.reward = 0.8
episode.position_x = 1.0
episode.position_y = 2.0

episodic_mem.store_episode(episode)

# Retrieve similar episodes
similar_episodes = episodic_mem.retrieve_similar([0.1, 0.6, 0.8])
print(f"Retrieved {len(similar_episodes)} similar episodes")

# Replay episodes
for i, episode_ptr in enumerate(episodic_mem.get_recent_episodes(5)):
    episodic_mem.replay_episode(episode_ptr)
    print(f"Replayed episode {i}")

# Associative Memory
assoc_mem = pynlm.create_neural_associative_memory()

# Create associations
assoc_mem.associate([0.1, 0.2, 0.3], [0.4, 0.5, 0.6])
assoc_mem.associate_from_experience(episode)

# Retrieve associated patterns
patterns = assoc_mem.retrieve([0.1, 0.2, 0.3])
print(f"Retrieved {len(patterns)} associated patterns")

# Spread activation
activated = assoc_mem.spread_activation([0.1, 0.2, 0.3], steps=3)
print(f"Spread activation retrieved {len(activated)} pattern chains")
```

### SelfModel and SocialLearning Usage
```python
# SelfModel
self_model = pynlm.create_self_model()

# Record self-actions to build self-model
self_model.record_self_action(
    pynlm.ActionType.MoveForward,
    [0.0, 0.0, 0.0],  # before state
    [0.1, 0.0, 0.0]   # after state
)

# Predict consequences
predicted_state = self_model.predict_action_consequence(
    pynlm.ActionType.MoveForward,
    [0.0, 0.0, 0.0]
)

# Get self-model confidence
confidence = self_model.get_self_model_confidence(pynlm.ActionType.MoveForward)
print(f"Self-model confidence for movement: {confidence:.3f}")

# SocialLearning
social_learning = pynlm.create_social_learning()

# Observe other agent's actions
social_learning.observe_agent_action(
    pynlm.ActionType.MoveRight,
    [0.0, 0.0, 0.0],  # observer state
    [0.0, 0.1, 0.0]   # resulting state
)

# Can we imitate?
if social_learning.can_imitate(pynlm.ActionType.MoveRight):
    imitation_action = social_learning.get_imitation_action([0.0, 0.0, 0.0])
    print(f"Can imitate action: {imitation_action}")

# Learn communication signals
social_learning.learn_communication_signal([0.5, 0.5, 0.5], 0.9)
signal = social_learning.get_signal_pattern()
meaning = social_learning.get_signal_meaning()
print(f"Signal pattern: {signal}, meaning: {meaning}")
```

### Spatial and Temporal Systems Usage
```python
# SpatialRepresentation
spatial = pynlm.create_spatial_representation()

# Record position experiences
spatial.record_position(1.0, 2.0, [0.1, 0.2, 0.3])
spatial.record_position(1.1, 2.1, [0.2, 0.3, 0.4])

# Get current position
current_x, current_y = spatial.get_current_position()
print(f"Current position: ({current_x:.1f}, {current_y:.1f})")

# Path integration
spatial.integrate_movement(0.5, 0.3)
updated_x, updated_y = spatial.get_predicted_position()
print(f"Position after movement: ({updated_x:.1f}, {updated_y:.1f})")

# Find place neurons
place_neurons = spatial.get_place_neurons_near(1.0, 2.0, 1.0)
print(f"Found {len(place_neurons)} place neurons near (1.0, 2.0)")

# TemporalRelation
temporal = pynlm.create_temporal_relation()

# Record temporal sequences
temporal.record_sequence(
    [0.0, 0.0, 0.0],  # event A
    [0.1, 0.0, 0.0],  # event B
    0.8
)

# Predict next event
predicted_next = temporal.predict_next([0.0, 0.0, 0.0])
print(f"Predicted next event: {predicted_next}")

# Get prediction confidence
confidence = temporal.get_prediction_confidence([0.0, 0.0, 0.0])
print(f"Prediction confidence: {confidence:.3f}")
```

## Utility Functions

### System Validation and Status
```python
# Validate that all bindings work correctly
is_valid, issues = pynlm.validate_bindings()
if is_valid:
    print("All bindings are valid!")
else:
    print(f"Binding issues found: {issues}")

# Get list of all available systems
systems = pynlm.get_available_systems()
print(f"Available systems: {systems}")

# Check status of a specific system
status = pynlm.check_system_status('NeuralPlanner')
print(f"NeuralPlanner status: {status}")

# Get comprehensive system information
info = pynlm.get_system_info()
print(info)
```

### Library Information
```python
# Get library version
version = pynlm.version()
print(f"Library version: {version}")

# Get complete API summary
summary = pynlm.get_api_summary()
print(summary)

# Get Pythonic features
features = pynlm.get_pythonic_features()
print(f"Pythonic features: {features}")

# Get example usage
code_examples = pynlm.example_usage()
print(code_examples)
```

## Error Handling

### Custom Exceptions
```python
import pynlm as nlm

try:
    # Example operations that might raise exceptions
    config = pynlm.create_default_config()
    config.set_int("invalid_key", "not_an_int")  # Will raise ConfigurationError
    
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
    
except nlm.NLMError as e:
    print(f"General NLM error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Common Error Scenarios
- **ConfigurationError**: Invalid configuration keys, type mismatches
- **MemoryError**: Memory allocation failures, invalid memory operations
- **PlanningError**: Invalid planning parameters, sequence evaluation errors
- **NLMError**: General NLM system errors

## Integration Examples

### Complete Simulation Example
```python
"""
Complete example demonstrating all advanced systems working together.
This example simulates an agent learning in a 2D world using all NLM systems.
"""

import pynlm as nlm
import time

def run_complete_simulation():
    print("Starting complete NLM simulation...")
    
    # Setup world and brain
    world = nlm.create_simple_world()
    config = nlm.create_default_config()
    brain = nlm.create_brain(config)
    
    # Configure world
    world.configure(100, 100, 20, 20)
    world.set_agent_start(10, 10)
    
    # Create and initialize all advanced systems
    systems = {
        'NeuralPlanner': nlm.create_neural_planner(),
        'ConceptFormation': nlm.create_concept_formation(),
        'SelfModel': nlm.create_self_model(),
        'SocialLearning': nlm.create_social_learning(),
        'SpatialRepresentation': nlm.create_spatial_representation(),
        'TemporalRelation': nlm.create_temporal_relation(),
        'WorkingMemory': nlm.create_neural_working_memory(),
        'EpisodicMemory': nlm.create_neural_episodic_memory(),
        'AssociativeMemory': nlm.create_neural_associative_memory(),
        'PredictionSystem': nlm.create_prediction_system()
    }
    
    # Initialize all systems with brain reference
    for system_name, system in systems.items():
        system.initialize(brain)
        print(f"Initialized {system_name}")
    
    # Run simulation
    agent = nlm.create_agent_brain(brain)
    agent.initialize(world)
    
    print("Running simulation...")
    
    # Main simulation loop
    for step in range(100):
        # Get current sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input through agent
        agent.process_sensory_input(percept)
        
        # Get action from brain
        action = brain.produce_action()
        
        # Apply action to world
        world.update(1.0)
        
        # Example: Store experience in all memory systems
        pattern = percept.get_vision() if hasattr(percept, 'get_vision') else [0.0]
        
        # Working memory
        systems['WorkingMemory'].store(pattern)
        
        # Episodic memory
        episode = nlm.EpisodicMemoryItem()
        episode.sensory_state = pattern
        episode.action = action.get_type() if hasattr(action, 'get_type') else nlm.ActionType.Wait
        episode.reward = 0.5
        systems['EpisodicMemory'].store_episode(episode)
        
        # Concept formation
        concept_id = systems['ConceptFormation'].present_experience(
            pattern, pattern, 0.5, step
        )
        
        # Spatial representation
        if hasattr(percept, 'get_vision') and percept.get_vision_width() > 0:
            # Extract position from vision (simplified)
            spatial_pos = (10.0, 10.0)  # placeholder
            systems['SpatialRepresentation'].record_position(
                spatial_pos[0], spatial_pos[1], pattern
            )
        
        # Temporal relations
        if step > 0:
            # Example temporal relationship
            systems['TemporalRelation'].record_sequence(
                pattern, pattern, 0.5
            )
        
        if step % 20 == 0:
            # Print system status
            planner_status = systems['NeuralPlanner'].get_planning_summary()
            concept_count = systems['ConceptFormation'].get_concept_count()
            memory_activity = systems['WorkingMemory'].get_memory_activity()
            
            print(f"Step {step}: Planner: {planner_status}, Concepts: {concept_count}, "
                  f"Memory Activity: {memory_activity:.3f}")
    
    print("Simulation completed successfully!")
    
    # Demonstrate system interaction
    print("\nDemonstrating system interactions...")
    
    # Self-model prediction
    self_pred = systems['SelfModel'].predict_action_consequence(
        nlm.ActionType.MoveForward,
        [0.0]
    )
    print(f"Self-model prediction: {self_pred}")
    
    # Social learning imitation
    can_immitate = systems['SocialLearning'].can_immitate(
        nlm.ActionType.MoveForward
    )
    print(f"Can imitate MoveForward: {can_immitate}")
    
    # Spatial navigation
    current_pos = systems['SpatialRepresentation'].get_current_position()
    print(f"Current spatial position: {current_pos}")
    
    # Temporal prediction
    predicted_next = systems['TemporalRelation'].predict_next(
        [0.0]
    )
    print(f"Temporal prediction: {predicted_next}")
    
    return systems

# Run the example
if __name__ == "__main__":
    simulation_systems = run_complete_simulation()
    print("\nAll systems successfully integrated and demonstrated!")
```

## Installation and Setup

### Dependencies
The enhanced NLM Python bindings require:
- C++ compiler with C++11 support
- pybind11 library (included in the project)
- Python development headers

### Building
```bash
# Build the enhanced bindings
cd python
mkdir build
cd build
cmake ..
make

# Install
make install
```

### Python Installation
```bash
# Install the package in development mode
cd /path/to/nlm-python
pip install -e .
```

### Testing
```bash
# Run tests
cd tests
python -m pytest test_bindings.py -v
```

## Troubleshooting

### Common Issues
1. **Import errors**: Ensure pybind11 is properly installed and the module is built.
2. **Missing systems**: Check that the C++ systems are implemented and linked.
3. **Memory issues**: Be aware of automatic memory management with shared_ptr.
4. **Performance**: The enhanced bindings may have slightly higher overhead due to additional safety checks.

### Getting Help
- Check the examples directory for complete working examples
- Use the `validate_bindings()` function to test your setup
- Consult the individual system documentation for specific usage patterns
- Check the issue tracker for known issues and solutions

## Acknowledgements

This enhanced version of the NLM Python bindings was developed to provide a complete, Pythonic interface to all NLM systems. The implementation follows Python best practices and provides comprehensive documentation and examples.

Special thanks to the NLM development team for creating the underlying C++ systems and to the open-source community for the pybind11 library.

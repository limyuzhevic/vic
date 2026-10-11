# NLM Python API Reference

## Core Classes

### NLM
High-level interface to NLM system for common operations.

#### Methods

- `__init__(config_file=None)` - Initialize NLM system
- `create_agent(brain_config=None)` - Create a new agent with specified brain configuration
- `run_simulation(steps=100, save_checkpoint=False)` - Run a simulation for specified number of steps
- `save_checkpoint(name)` - Save current agent state to checkpoint
- `load_checkpoint(name)` - Load agent state from checkpoint
- `evaluate()` - Evaluate current agent performance
- `run_experiment(experiment_config)` - Run automated experiment
- `analyze_results(results)` - Analyze experiment results
- `generate_report(results, report_title, report_format)` - Generate report from results

### BrainNetwork
High-level brain network class for neural simulation.

#### Methods

- `__init__(config=None)` - Initialize a brain network
- `initialize()` - Initialize the brain with current configuration
- `step(dt=0.1)` - Perform a simulation step
- `add_region(name, neuron_count=100, neuron_type="excitatory")` - Add a new neural region
- `get_region_info(region_name)` - Get information about a specific region
- `get_statistics()` - Get brain network statistics
- `save(filepath)` - Save brain network to file
- `load(filepath)` - Load brain network from file
- `reset()` - Reset brain network to initial state

#### Properties

- `config` - Get the brain configuration
- `is_initialized` - Check if the brain is initialized

### EpisodicMemory
Episodic memory with experience encoding and replay.

#### Methods

- `__init__(capacity=1000, consolidation_threshold=0.7)` - Initialize episodic memory
- `encode(experience, timestamp)` - Encode an experience into episodic memory
- `retrieve(episode_id=None, filter_keywords=None)` - Retrieve episodes from episodic memory
- `replay()` - Replay scheduled episodes
- `consolidate(importance_threshold=None)` - Consolidate important episodes
- `analyze_patterns()` - Analyze patterns in episodic memory

### LearningScheduler
Schedule learning sessions for neural networks.

#### Methods

- `__init__(base_interval=1.0, adaptation_rate=0.1)` - Initialize learning scheduler
- `schedule_learning(trigger, performance=None, priority="normal", custom_time=None)` - Schedule a learning session
- `execute_scheduled_sessions()` - Execute all scheduled learning sessions that are due
- `get_schedule()` - Get current learning schedule
- `cancel_session(session_id)` - Cancel a scheduled learning session
- `get_statistics()` - Get learning statistics

### PlasticityController
Control plasticity rules for neural network adaptation.

#### Methods

- `__init__()` - Initialize plasticity controller
- `set_rule(rule, **kwargs)` - Set the current plasticity rule
- `add_plasticity_type(plasticity_type, parameters)` - Add a plasticity type with its parameters
- `enable_adaptive_plasticity(learning_rate=0.01, target_performance=0.8)` - Enable adaptive plasticity based on performance
- `calculate_plasticity(pre_activity, post_activity, weight, time_delta=0.0)` - Calculate plasticity change for a synapse
- `get_current_parameters()` - Get current plasticity parameters

### AdaptiveLearning
Adaptive learning algorithms that adjust based on performance.

#### Methods

- `__init__()` - Initialize adaptive learning system
- `add_algorithm(name, algorithm, parameters)` - Add an adaptive learning algorithm
- `select_algorithm(current_performance)` - Select the best algorithm based on current performance
- `learn(performance, network_state)` - Perform adaptive learning based on current performance
- `add_adaptation_strategy(name, strategy, conditions)` - Add an adaptation strategy
- `check_adaptation_opportunities(network_state)` - Check for opportunities to apply adaptation strategies
- `get_algorithm_statistics()` - Get statistics about algorithm performance

### CuriosityDrivenLearning
Intrinsic motivation and curiosity-driven learning.

#### Methods

- `__init__(exploration_rate=0.1, novelty_threshold=0.5)` - Initialize curiosity-driven learning system
- `process_experience(experience)` - Process an experience through curiosity-driven mechanisms
- `get_curiosity_statistics()` - Get curiosity-driven learning statistics

### DopamineSystem
High-level dopamine modeling for reward prediction and learning.

#### Methods

- `__init__(learning_rate=0.1, prediction_horizon=10)` - Initialize dopamine system
- `update(reward, prediction)` - Update dopamine system with new reward and prediction
- `get_statistics()` - Get dopamine system statistics
- `reset()` - Reset dopamine system to initial state

### RewardEngine
Reward processing and prediction system.

#### Methods

- `__init__(discount_factor=0.9, exploration_bonus=0.1)` - Initialize reward engine
- `process_reward(reward, action=None, state=None)` - Process a reward signal
- `predict_future_reward(state, steps=10)` - Predict future rewards for a given state
- `get_reward_statistics()` - Get reward processing statistics
- `reset()` - Reset reward engine to initial state

### PredictionErrorTracker
Track prediction errors for learning optimization.

#### Methods

- `__init__(time_window=100)` - Initialize prediction error tracker
- `record_error(error, timestamp=None)` - Record a prediction error
- `get_error_analysis()` - Get prediction error analysis
- `reset()` - Reset prediction error tracker

### MotivationSystem
Integrate motivation signals from multiple sources.

#### Methods

- `__init__(motivation_weights=None)` - Initialize motivation system
- `update_motivation(context=None)` - Update motivation based on current context
- `add_goal(goal_id, description, target_value, deadline=None)` - Add a goal to the motivation system
- `update_goal_progress(goal_id, progress)` - Update progress for a goal
- `get_motivation_statistics()` - Get motivation system statistics
- `reset()` - Reset motivation system

### AgentTrainer
Train agents with advanced features.

#### Methods

- `__init__(learning_rate=0.01, batch_size=32, discount_factor=0.99, entropy_coefficient=0.01)` - Initialize agent trainer
- `add_experience(state, action, reward, next_state, done)` - Add experience to replay buffer
- `train_step(agent_brain)` - Perform a training step
- `get_training_statistics()` - Get training statistics
- `reset()` - Reset trainer to initial state

### Evaluator
Evaluate agent performance.

#### Methods

- `__init__(evaluation_metrics=None)` - Initialize evaluator
- `evaluate(agent_brain, world, episodes=10, max_steps=100)` - Evaluate an agent
- `set_baseline(baseline_results)` - Set baseline performance for comparison
- `get_evaluation_statistics()` - Get evaluation statistics
- `generate_report(format="text")` - Generate evaluation report
- `reset()` - Reset evaluator to initial state

### CheckpointManager
Save/load agent states.

#### Methods

- `__init__(checkpoint_dir="checkpoints")` - Initialize checkpoint manager
- `save(brain, memory, world, name, metadata=None)` - Save agent state to checkpoint
- `load(name)` - Load agent state from checkpoint
- `list_checkpoints()` - List all available checkpoints
- `delete_checkpoint(name)` - Delete a checkpoint
- `get_checkpoint(name)` - Get checkpoint information
- `cleanup_old_checkpoints(keep_recent=10)` - Clean up old checkpoints
- `get_checkpoint_history()` - Get checkpoint history
- `reset()` - Reset checkpoint manager

### PerformanceMonitor
Monitor agent performance.

#### Methods

- `__init__(monitoring_interval=1.0)` - Initialize performance monitor
- `start_monitoring()` - Start performance monitoring
- `stop_monitoring()` - Stop performance monitoring
- `update_metrics(metrics)` - Update performance metrics
- `set_threshold(metric, threshold, alert_type="warning")` - Set performance threshold
- `get_performance_stats()` - Get performance statistics
- `get_performance_trend(metric, window=10)` - Get performance trend
- `get_all_trends()` - Get trends for all monitored metrics
- `reset()` - Reset performance monitor

### WorldSimulator
Advanced world simulation for NLM agents.

#### Methods

- `__init__(width=100, height=100, time_step=0.1, physics_enabled=True)` - Initialize world simulator
- `configure(**kwargs)` - Configure world simulator
- `reset()` - Reset world to initial state
- `add_agent(agent)` - Add agent to world
- `remove_agent(agent)` - Remove agent from world
- `add_object(obj)` - Add object to world
- `remove_object(obj)` - Remove object from world
- `update(dt=None)` - Update world simulation
- `get_sensory_percept()` - Get sensory percept from world
- `get_world_state()` - Get current world state
- `save(filepath)` - Save world state to file
- `load(filepath)` - Load world state from file
- `is_valid_position(x, y, radius=0.0)` - Check if position is valid

### EnvironmentFactory
Create environments for NLM simulations.

#### Methods

- `create_simple_environment(width=100, height=100, num_resources=10)` - Create a simple environment
- `create_complex_environment(width=200, height=200, num_hazards=5, num_resources=20)` - Create a complex environment
- `create_procedural_environment(env_type="forest", seed=None)` - Create a procedural environment
- `create_custom_environment(width=100, height=100, objects=None)` - Create a custom environment

### ScenarioManager
Manage experimental scenarios for NLM simulations.

#### Methods

- `__init__()` - Initialize scenario manager
- `create_scenario(name, description, parameters, environment_type="simple")` - Create a new scenario
- `get_scenario(name)` - Get scenario by name
- `list_scenarios()` - List all scenario names
- `update_scenario(name, **kwargs)` - Update scenario parameters
- `delete_scenario(name)` - Delete scenario
- `load_scenario(name)` - Load scenario for use
- `get_current_scenario()` - Get current scenario
- `end_scenario()` - End current scenario
- `get_scenario_statistics()` - Get scenario management statistics
- `reset()` - Reset scenario manager

### ReplaySystem
Replay recorded experiences.

#### Methods

- `__init__(replay_buffer_size=1000)` - Initialize replay system
- `record_experience(experience)` - Record an experience for replay
- `replay_experiences(start_index=None, end_index=None, filter_criteria=None)` - Replay experiences from buffer
- `analyze_replay(analysis_type, time_window=100, filter_criteria=None)` - Analyze replayed experiences
- `get_replay_statistics()` - Get replay system statistics
- `clear_replay_buffer()` - Clear replay buffer
- `reset()` - Reset replay system

### Plotter
Neural activity visualization.

#### Methods

- `__init__(style="default", figure_size=(12, 8))` - Initialize plotter
- `plot_neural_activity(activity_data, title="Neural Activity", save_path=None)` - Plot neural activity over time
- `plot_connectivity_matrix(connectivity_data, title="Connectivity Matrix", save_path=None)` - Plot connectivity matrix
- `plot_training_curve(training_metrics, title="Training Curves", save_path=None)` - Plot training curves
- `plot_activity_heatmap(activity_data, title="Activity Heatmap", save_path=None)` - Plot activity heatmap
- `plot_metric_comparison(metric_data, metric_names=None, title="Metric Comparison", save_path=None)` - Plot metric comparison
- `create_animation(data_sequence, title="Animation", save_path=None)` - Create animation from data sequence
- `save_figure(fig, filepath)` - Save figure to file
- `close_all_figures()` - Close all open figures
- `reset()` - Reset plotter

### ActivityAnalyzer
Analyze neural patterns and activity.

#### Methods

- `__init__(analysis_methods=None)` - Initialize activity analyzer
- `analyze_activity(activity_data, analysis_types=None)` - Analyze neural activity
- `get_analysis_statistics()` - Get analysis statistics
- `reset()` - Reset analyzer

### HeatmapGenerator
Generate neural heatmaps.

#### Methods

- `__init__(colormap="viridis", normalize=True)` - Initialize heatmap generator
- `generate_neural_heatmap(activity_data, title="Neural Activity Heatmap", save_path=None)` - Generate neural activity heatmap
- `generate_spatial_heatmap(positions, values, title="Spatial Heatmap", save_path=None)` - Generate spatial heatmap
- `generate_connectivity_heatmap(connectivity_matrix, title="Connectivity Heatmap", save_path=None)` - Generate connectivity heatmap
- `generate_activity_heatmap_2d(activity_2d, title="2D Activity Heatmap", save_path=None)` - Generate 2D activity heatmap
- `get_heatmap_statistics()` - Get heatmap generation statistics
- `reset()` - Reset heatmap generator

### ActivityStatistics
Compute statistics from neural activity.

#### Methods

- `__init__(statistics_types=None)` - Initialize activity statistics calculator
- `compute_statistics(activity_data, statistic_types=None)` - Compute statistics from neural activity
- `get_statistics_history()` - Get statistics computation history
- `reset()` - Reset statistics calculator

### ExperimentRunner
Run automated experiments for NLM systems.

#### Methods

- `__init__(n_workers=1, parallelization="sequential")` - Initialize experiment runner
- `create_experiment(name, description, parameters, environment_config=None)` - Create a new experiment
- `get_experiment(name)` - Get experiment by name
- `list_experiments()` - List all experiment names
- `update_experiment(name, **kwargs)` - Update experiment parameters
- `delete_experiment(name)` - Delete experiment
- `run_experiment(name, iterations=1, parallel=False)` - Run an experiment
- `run_batch_experiments(experiment_names, iterations_per_experiment=10)` - Run a batch of experiments
- `get_experiment_statistics()` - Get experiment statistics
- `stop_experiment(name)` - Stop a running experiment
- `cleanup()` - Cleanup experiment runner
- `reset()` - Reset experiment runner

### BatchProcessor
Process multiple experiment runs efficiently.

#### Methods

- `__init__(batch_size=10, n_workers=4, parallelization="thread")` - Initialize batch processor
- `create_batch(batch_id, experiment_names, iterations_per_experiment=10, parameter_combinations=None)` - Create a new batch
- `get_batch(batch_id)` - Get batch by ID
- `list_batches()` - List all batch IDs
- `start_batch(batch_id)` - Start processing a batch
- `process_next_task()` - Process the next task in the queue
- `wait_for_batch_completion(batch_id, timeout=None)` - Wait for batch completion
- `get_batch_statistics()` - Get batch processing statistics
- `stop_batch(batch_id)` - Stop a running batch
- `cleanup()` - Cleanup batch processor
- `reset()` - Reset batch processor

### ResultAggregator
Aggregate experiment results.

#### Methods

- `__init__(aggregation_methods=None)` - Initialize result aggregator
- `aggregate_results(results, experiment_names=None)` - Aggregate experiment results
- `get_aggregation_statistics()` - Get result aggregation statistics
- `reset()` - Reset result aggregator

### ReportGenerator
Generate comprehensive reports.

#### Methods

- `__init__(report_format="text", output_directory="reports")` - Initialize report generator
- `create_report(report_id, title, description, data, template=None)` - Create a new report
- `get_report(report_id)` - Get report by ID
- `list_reports()` - List all report IDs
- `generate_report(report_id, output_path=None)` - Generate report
- `get_report_statistics()` - Get report generation statistics
- `cleanup()` - Cleanup report generator
- `reset()` - Reset report generator

### NeuralNetworkTrainer
Train neural networks with advanced optimization techniques.

#### Methods

- `__init__(learning_rate=0.001, batch_size=32, optimizer="adam", loss_function="mse", regularization=None)` - Initialize neural network trainer
- `initialize_model(architecture)` - Initialize neural network model
- `train_model(X_train, y_train, X_val, y_val, epochs=100, verbose=False)` - Train neural network model
- `save_model(filepath)` - Save trained model
- `load_model(filepath)` - Load trained model
- `get_training_statistics()` - Get training statistics
- `reset()` - Reset trainer to initial state

### PolicyGradientLearner
Policy gradient methods for reinforcement learning.

#### Methods

- `__init__(policy_network, learning_rate=0.001, discount_factor=0.99, entropy_coefficient=0.01)` - Initialize policy gradient learner
- `start_episode()` - Start a new episode
- `end_episode()` - End the current episode and compute returns
- `step(state, action, reward, done, log_prob)` - Process a step in the episode
- `get_training_statistics()` - Get training statistics
- `reset()` - Reset learner to initial state

### ReinforcementLearning
Comprehensive reinforcement learning framework.

#### Methods

- `__init__(algorithm="reinforce", env_config=None)` - Initialize reinforcement learning system
- `train(max_episodes=1000, max_steps_per_episode=100, evaluation_interval=10, verbose=False)` - Train reinforcement learning agent
- `evaluate(n_episodes=10)` - Evaluate trained agent
- `save_agent(filepath)` - Save trained agent
- `load_agent(filepath)` - Load trained agent
- `get_training_statistics()` - Get training statistics
- `reset()` - Reset reinforcement learning system

### MetaLearner
Meta-learning capabilities for rapid adaptation.

#### Methods

- `__init__(meta_learner_type="maml", inner_loop_steps=5, outer_loop_steps=100)` - Initialize meta-learner
- `create_task(task_type, task_params)` - Create a new learning task
- `execute_task(task, support_set, query_set=None)` - Execute a learning task
- `get_meta_learning_statistics()` - Get meta-learning statistics
- `save_meta_learner(filepath)` - Save meta-learner
- `load_meta_learner(filepath)` - Load meta-learner
- `reset()` - Reset meta-learner

## Enumerations

### LearningRule
- `STDP` - Spike-Timing-Dependent Plasticity
- `HEBBIAN` - Hebbian learning
- `LTP` - Long-Term Potentiation
- `LTD` - Long-Term Depression
- `Oja` - Oja's rule
- `BCM` - BCM learning rule

### PlasticityType
- `SYNAPTIC` - Synaptic plasticity
- `STRUCTURAL` - Structural plasticity
- `NEURONAL` - Neuronal plasticity

### NeuromodulatorType
- `DOPAMINE` - Dopamine
- `SEROTONIN` - Serotonin
- `NORADRENALIN` - Noradrenaline
- `ACETYLCHOLINE` - Acetylcholine
- `OXYTOCIN` - Oxytocin
- `ENDORPHIN` - Endorphin

### EnvironmentType
- `SIMPLE` - Simple environment
- `COMPLEX` - Complex environment
- `PROCEDURAL` - Procedural environment
- `CUSTOM` - Custom environment

## Function References

### Configuration Management

- `nlm_config()` - Create a high-level configuration manager class
- `load_config_file(filepath, auto_detect=True)` - Load config with automatic format detection
- `save_config(config, filepath, format="auto")` - Save config with format choice
- `validate_config(config, schema=None)` - Validate config against schemas
- `compare_configs(config1, config2, tolerance=0.001)` - Compare two configs

### Brain and Neural Network Management

- `BrainNetwork(config=None)` - High-level brain network class
- `create_network(config=None)` - Create and configure neural networks
- `import_brain(filepath)` - Import pre-trained brain models
- `export_brain(brain, filepath, format="auto")` - Export brain models for sharing
- `simulate_brain(brain, steps, dt=0.1)` - Run brain simulations with results

### Memory System Integration

- `EpisodicMemory(capacity=1000, consolidation_threshold=0.7)` - High-level episodic memory interface
- `ConsolidationManager(episodic_memory)` - Automatic memory consolidation
- `MemoryProfiler(episodic_memory, working_memory)` - Memory usage analysis
- `RetrievalOptimizer(episodic_memory, working_memory)` - Memory retrieval optimization

### Learning and Plasticity Control

- `LearningScheduler(base_interval=1.0, adaptation_rate=0.1)` - Schedule learning sessions
- `PlasticityController()` - Control plasticity rules
- `AdaptiveLearning()` - Adaptive learning algorithms
- `CuriosityDrivenLearning(exploration_rate=0.1, novelty_threshold=0.5)` - Intrinsic motivation integration

### Neuromodulation and Reward Systems

- `DopamineSystem(learning_rate=0.1, prediction_horizon=10)` - High-level dopamine modeling
- `RewardEngine(discount_factor=0.9, exploration_bonus=0.1)` - Reward processing and prediction
- `PredictionErrorTracker(time_window=100)` - Track prediction errors
- `MotivationSystem(motivation_weights=None)` - Integrate motivation signals

### Agent Control and Management

- `AgentTrainer(learning_rate=0.01, batch_size=32, discount_factor=0.99, entropy_coefficient=0.01)` - Train agents with advanced features
- `Evaluator(evaluation_metrics=None)` - Evaluate agent performance
- `CheckpointManager(checkpoint_dir="checkpoints")` - Save/load agent states
- `PerformanceMonitor(monitoring_interval=1.0)` - Monitor agent performance

### Environment and World Management

- `WorldSimulator(width=100, height=100, time_step=0.1, physics_enabled=True)` - Advanced world simulation
- `EnvironmentFactory.create_simple_environment()` - Create environments
- `EnvironmentFactory.create_complex_environment()` - Create environments
- `EnvironmentFactory.create_procedural_environment()` - Create environments
- `ScenarioManager()` - Manage experimental scenarios
- `ReplaySystem(replay_buffer_size=1000)` - Replay recorded experiences

### Visualization and Analysis

- `Plotter(style="default", figure_size=(12, 8))` - Neural activity visualization
- `ActivityAnalyzer(analysis_methods=None)` - Analyze neural patterns
- `HeatmapGenerator(colormap="viridis", normalize=True)` - Generate neural heatmaps
- `ActivityStatistics(statistics_types=None)` - Compute statistics

### Automation and Scripting

- `ExperimentRunner(n_workers=1, parallelization="sequential")` - Run automated experiments
- `BatchProcessor(batch_size=10, n_workers=4, parallelization="thread")` - Process multiple runs
- `ResultAggregator(aggregation_methods=None)` - Aggregate experiment results
- `ReportGenerator(report_format="text", output_directory="reports")` - Generate comprehensive reports

### Advanced Machine Learning Integration

- `NeuralNetworkTrainer(learning_rate=0.001, batch_size=32, optimizer="adam", loss_function="mse", regularization=None)` - Train neural networks
- `PolicyGradientLearner(policy_network, learning_rate=0.001, discount_factor=0.99, entropy_coefficient=0.01)` - Policy gradient methods
- `ReinforcementLearning(algorithm="reinforce", env_config=None)` - RL algorithms
- `MetaLearner(meta_learner_type="maml", inner_loop_steps=5, outer_loop_steps=100)` - Meta-learning capabilities

## Usage Examples

### Basic NLM Usage

```python
import nlm

# Create NLM system
nlm_system = nlm.NLM()

# Create brain with configuration
brain = nlm_system.create_agent({"regions": 5, "neurons_per_region": 100})

# Run simulation
results = nlm_system.run_simulation(steps=500)

# Evaluate performance
evaluation = nlm_system.evaluate()
print(f"Performance: {evaluation.get('overall_score', 0):.2f}")
```

### Advanced Brain Network

```python
import nlm
import numpy as np

# Create sophisticated brain network
brain = nlm.BrainNetwork()
brain.add_region("visual_cortex", 300, "excitatory")
brain.add_region("motor_cortex", 250, "motor")
brain.add_region("hippocampus", 150, "internal")

# Initialize and simulate
brain.initialize()
simulation_results = nlm.simulate_brain(brain, steps=1000, dt=0.05)

# Analyze statistics
stats = brain.get_statistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total regions: {stats['total_regions']}")
print(f"Firing rate: {stats['avg_neurons_per_region']:.1f}")
```

### Memory Management

```python
import nlm

# Create memory systems
episodic_memory = nlm.EpisodicMemory(capacity=1000, consolidation_threshold=0.7)
working_memory = nlm.WorkingMemory(capacity=50, decay_rate=0.02)

# Encode and retrieve experiences
episode_id = episodic_memory.encode({"stimulus": "red", "action": "approach"}, time.time())
retrieved = episodic_memory.retrieve(filter_keywords=["approach"])

# Consolidate and analyze
consolidated = episodic_memory.consolidate()
patterns = episodic_memory.analyze_patterns()
```

### Learning and Plasticity

```python
import nlm

# Configure plasticity
plasticity_controller = nlm.PlasticityController()
plasticity_controller.set_rule(nlm.LearningRule.STDP, learning_rate=0.01)

# Set up adaptive learning
adaptive_learning = nlm.AdaptiveLearning()
adaptive_learning.add_algorithm(
    "td_learning",
    lambda perf, hist, params: perf * 0.1,
    {"discount": 0.99}
)

# Curiosity-driven learning
curiosity_learning = nlm.CuriosityDrivenLearning(exploration_rate=0.1)
```

### Neuromodulation

```python
import nlm

# Create neuromodulation system
dopamine_system = nlm.DopamineSystem(learning_rate=0.1)
reward_engine = nlm.RewardEngine(discount_factor=0.9)

# Simulate experience
for step in range(50):
    reward = np.random.randn() * 10 + 5
    dopamine_result = dopamine_system.update(reward, reward * 0.8)
    reward_result = reward_engine.process_reward(reward)
```

### Agent Training

```python
import nlm

# Create agent trainer
trainer = nlm.AgentTrainer(learning_rate=0.01, batch_size=32)

# Set up evaluator
evaluator = nlm.Evaluator()

# Create checkpoint manager
checkpoint_manager = nlm.CheckpointManager()

# Train and evaluate
for iteration in range(100):
    # Add training experience
    trainer.add_experience(state, action, reward, next_state, done)
    
    # Train agent
    trainer.train_step(agent_brain)

# Evaluate
evaluation = evaluator.evaluate(agent_brain, world)
print(f"Evaluation score: {evaluation['overall_score']}")
```

### Experiment Automation

```python
import nlm

# Create experiment runner
experiment_runner = nlm.ExperimentRunner()

# Create experiments
experiments = [
    {"name": "simple_brain", "parameters": {"regions": 3}},
    {"name": "complex_brain", "parameters": {"regions": 10}},
]

# Run experiments
for exp in experiments:
    results = experiment_runner.run_experiment(exp["name"], iterations=10)

# Aggregate results
aggregated = experiment_runner.analyze_results(list(results.values()))
```

### Machine Learning Integration

```python
import nlm
import numpy as np

# Create neural network trainer
trainer = nlm.NeuralNetworkTrainer(
    learning_rate=0.001,
    batch_size=32,
    optimizer="adam",
    loss_function="mse"
)

# Initialize model
architecture = {
    "layers": [
        {"type": "dense", "units": 128, "activation": "relu"},
        {"type": "dense", "units": 64, "activation": "relu"},
        {"type": "dense", "units": 1, "activation": "linear"}
    ]
}

trainer.initialize_model(architecture)

# Train model
results = trainer.train_model(X_train, y_train, X_val, y_val)
```

### Reinforcement Learning

```python
import nlm

# Create reinforcement learning system
rl_system = nlm.ReinforcementLearning(
    algorithm="reinforce",
    env_config={"width": 100, "height": 100}
)

# Train agent
results = rl_system.train(max_episodes=500)

# Evaluate agent
evaluation = rl_system.evaluate()
print(f"Evaluation reward: {evaluation['average_reward']}")
```

### Visualization

```python
import nlm
import matplotlib.pyplot as plt

# Create neural activity plotter
plotter = nlm.Plotter(style="scientific")

# Generate sample data
activity_data = np.random.randn(100, 1000)

# Plot neural activity
fig = plotter.plot_neural_activity(activity_data, title="Neural Activity")
plt.show()

# Plot connectivity matrix
connectivity = np.random.rand(50, 50)
fig = plotter.plot_connectivity_matrix(connectivity, title="Connectivity Matrix")
plt.show()
```

## Requirements

### Python Dependencies

```
numpy>=1.20
scipy>=1.6
matplotlib>=3.3
seaborn>=0.11
joblib>=0.14
psutil>=5.8
python-dateutil>=2.8
pyyaml>=5.3
jsonschema>=3.2

# For advanced visualization
pillow>=8.0
tqdm>=4.60

# For machine learning
scikit-learn>=0.24
```

### System Requirements

- **Operating System**: Linux, macOS, or Windows
- **Python Version**: 3.8 or higher
- **Memory**: Minimum 8GB RAM, recommended 16GB+
- **Disk Space**: Minimum 1GB for installation
- **Compiler**: C++ compiler for building extensions (if needed)

## Installation

### From PyPI

```bash
pip install nlm
```

### From Source

```bash
# Clone the repository
clone https://github.com/nlm-project/nlm.git

# Install Python dependencies
pip install -r requirements.txt

# Install Python bindings
pip install -e python/

# Build and install C++ core (if needed)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Testing

### Unit Tests

```bash
pytest python/nlm/tests/ -v
```

### Integration Tests

```bash
pytest python/integration_tests/ -v
```

### Performance Tests

```bash
python -m pytest python/performance_tests/ -v
```

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Authors

NLM Authors - Research team at NLM Project

## Contact

For questions, support, or feature requests:
- **GitHub**: https://github.com/nlm-project/nlm
- **Email**: support@nlm.org
- **Documentation**: https://nlm.readthedocs.io

## Acknowledgements

- Special thanks to the NLM core development team
- Appreciation for the open-source community
- Thanks to all contributors and beta testers

## Best Practices

### 1. Configuration Management
- Always validate configuration before use
- Use environment-specific configurations for different deployment scenarios
- Implement configuration versioning for reproducibility
- Consider using configuration templates for common setups

### 2. Memory Management
- Monitor memory usage, especially for long-running simulations
- Implement proper memory consolidation strategies
- Use efficient data structures for large-scale simulations
- Consider memory limits and garbage collection

### 3. Learning Control
- Implement adaptive learning rates based on performance
- Use learning schedules to balance exploration and exploitation
- Monitor for overfitting and apply regularization when needed
- Implement checkpointing for long training sessions

### 4. Neuromodulation
- Understand the interaction between different neuromodulators
- Use prediction errors for learning optimization
- Implement curiosity-driven exploration for open-ended learning
- Balance intrinsic and extrinsic motivation

### 5. Performance Optimization
- Profile your code to identify bottlenecks
- Use parallel processing for large simulations
- Optimize neural network architectures for efficiency
- Consider hardware acceleration where possible

### 6. Error Handling
- Implement comprehensive error handling and recovery mechanisms
- Use try-except blocks for critical operations
- Implement logging for debugging and monitoring
- Provide clear error messages and suggestions

## Core Classes

### NLM
High-level interface to NLM system for common operations.

#### Methods

- `__init__(config_file=None)` - Initialize NLM system
- `create_agent(brain_config=None)` - Create a new agent with specified brain configuration
- `run_simulation(steps=100, save_checkpoint=False)` - Run a simulation for specified number of steps
- `save_checkpoint(name)` - Save current agent state to checkpoint
- `load_checkpoint(name)` - Load agent state from checkpoint
- `evaluate()` - Evaluate current agent performance
- `run_experiment(experiment_config)` - Run automated experiment
- `analyze_results(results)` - Analyze experiment results
- `generate_report(results, report_title, report_format)` - Generate report from results

### BrainNetwork
High-level brain network class for neural simulation.

#### Methods

- `__init__(config=None)` - Initialize a brain network
- `initialize()` - Initialize the brain with current configuration
- `step(dt=0.1)` - Perform a simulation step
- `add_region(name, neuron_count=100, neuron_type="excitatory")` - Add a new neural region
- `get_region_info(region_name)` - Get information about a specific region
- `get_statistics()` - Get brain network statistics
- `save(filepath)` - Save brain network to file
- `load(filepath)` - Load brain network from file
- `reset()` - Reset brain network to initial state

#### Properties

- `config` - Get the brain configuration
- `is_initialized` - Check if the brain is initialized

### EpisodicMemory
Episodic memory with experience encoding and replay.

#### Methods

- `__init__(capacity=1000, consolidation_threshold=0.7)` - Initialize episodic memory
- `encode(experience, timestamp)` - Encode an experience into episodic memory
- `retrieve(episode_id=None, filter_keywords=None)` - Retrieve episodes from episodic memory
- `replay()` - Replay scheduled episodes
- `consolidate(importance_threshold=None)` - Consolidate important episodes
- `analyze_patterns()` - Analyze patterns in episodic memory

### LearningScheduler
Schedule learning sessions for neural networks.

#### Methods

- `__init__(base_interval=1.0, adaptation_rate=0.1)` - Initialize learning scheduler
- `schedule_learning(trigger, performance=None, priority="normal", custom_time=None)` - Schedule a learning session
- `execute_scheduled_sessions()` - Execute all scheduled learning sessions that are due
- `get_schedule()` - Get current learning schedule
- `cancel_session(session_id)` - Cancel a scheduled learning session
- `get_statistics()` - Get learning statistics

### PlasticityController
Control plasticity rules for neural network adaptation.

#### Methods

- `__init__()` - Initialize plasticity controller
- `set_rule(rule, **kwargs)` - Set the current plasticity rule
- `add_plasticity_type(plasticity_type, parameters)` - Add a plasticity type with its parameters
- `enable_adaptive_plasticity(learning_rate=0.01, target_performance=0.8)` - Enable adaptive plasticity based on performance
- `calculate_plasticity(pre_activity, post_activity, weight, time_delta=0.0)` - Calculate plasticity change for a synapse
- `get_current_parameters()` - Get current plasticity parameters

### AdaptiveLearning
Adaptive learning algorithms that adjust based on performance.

#### Methods

- `__init__()` - Initialize adaptive learning system
- `add_algorithm(name, algorithm, parameters)` - Add an adaptive learning algorithm
- `select_algorithm(current_performance)` - Select the best algorithm based on current performance
- `learn(performance, network_state)` - Perform adaptive learning based on current performance
- `add_adaptation_strategy(name, strategy, conditions)` - Add an adaptation strategy
- `check_adaptation_opportunities(network_state)` - Check for opportunities to apply adaptation strategies
- `get_algorithm_statistics()` - Get statistics about algorithm performance

### CuriosityDrivenLearning
Intrinsic motivation and curiosity-driven learning.

#### Methods

- `__init__(exploration_rate=0.1, novelty_threshold=0.5)` - Initialize curiosity-driven learning system
- `process_experience(experience)` - Process an experience through curiosity-driven mechanisms
- `get_curiosity_statistics()` - Get curiosity-driven learning statistics

### DopamineSystem
High-level dopamine modeling for reward prediction and learning.

#### Methods

- `__init__(learning_rate=0.1, prediction_horizon=10)` - Initialize dopamine system
- `update(reward, prediction)` - Update dopamine system with new reward and prediction
- `get_statistics()` - Get dopamine system statistics
- `reset()` - Reset dopamine system to initial state

### RewardEngine
Reward processing and prediction system.

#### Methods

- `__init__(discount_factor=0.9, exploration_bonus=0.1)` - Initialize reward engine
- `process_reward(reward, action=None, state=None)` - Process a reward signal
- `predict_future_reward(state, steps=10)` - Predict future rewards for a given state
- `get_reward_statistics()` - Get reward processing statistics
- `reset()` - Reset reward engine to initial state

### PredictionErrorTracker
Track prediction errors for learning optimization.

#### Methods

- `__init__(time_window=100)` - Initialize prediction error tracker
- `record_error(error, timestamp=None)` - Record a prediction error
- `get_error_analysis()` - Get prediction error analysis
- `reset()` - Reset prediction error tracker

### MotivationSystem
Integrate motivation signals from multiple sources.

#### Methods

- `__init__(motivation_weights=None)` - Initialize motivation system
- `update_motivation(context=None)` - Update motivation based on current context
- `add_goal(goal_id, description, target_value, deadline=None)` - Add a goal to the motivation system
- `update_goal_progress(goal_id, progress)` - Update progress for a goal
- `get_motivation_statistics()` - Get motivation system statistics
- `reset()` - Reset motivation system

### AgentTrainer
Train agents with advanced features.

#### Methods

- `__init__(learning_rate=0.01, batch_size=32, discount_factor=0.99, entropy_coefficient=0.01)` - Initialize agent trainer
- `add_experience(state, action, reward, next_state, done)` - Add experience to replay buffer
- `train_step(agent_brain)` - Perform a training step
- `get_training_statistics()` - Get training statistics
- `reset()` - Reset trainer to initial state

### Evaluator
Evaluate agent performance.

#### Methods

- `__init__(evaluation_metrics=None)` - Initialize evaluator
- `evaluate(agent_brain, world, episodes=10, max_steps=100)` - Evaluate an agent
- `set_baseline(baseline_results)` - Set baseline performance for comparison
- `get_evaluation_statistics()` - Get evaluation statistics
- `generate_report(format="text")` - Generate evaluation report
- `reset()` - Reset evaluator to initial state

### CheckpointManager
Save/load agent states.

#### Methods

- `__init__(checkpoint_dir="checkpoints")` - Initialize checkpoint manager
- `save(brain, memory, world, name, metadata=None)` - Save agent state to checkpoint
- `load(name)` - Load agent state from checkpoint
- `list_checkpoints()` - List all available checkpoints
- `delete_checkpoint(name)` - Delete a checkpoint
- `get_checkpoint(name)` - Get checkpoint information
- `cleanup_old_checkpoints(keep_recent=10)` - Clean up old checkpoints
- `get_checkpoint_history()` - Get checkpoint history
- `reset()` - Reset checkpoint manager

### PerformanceMonitor
Monitor agent performance.

#### Methods

- `__init__(monitoring_interval=1.0)` - Initialize performance monitor
- `start_monitoring()` - Start performance monitoring
- `stop_monitoring()` - Stop performance monitoring
- `update_metrics(metrics)` - Update performance metrics
- `set_threshold(metric, threshold, alert_type="warning")` - Set performance threshold
- `get_performance_stats()` - Get performance statistics
- `get_performance_trend(metric, window=10)` - Get performance trend
- `get_all_trends()` - Get trends for all monitored metrics
- `reset()` - Reset performance monitor

### WorldSimulator
Advanced world simulation for NLM agents.

#### Methods

- `__init__(width=100, height=100, time_step=0.1, physics_enabled=True)` - Initialize world simulator
- `configure(**kwargs)` - Configure world simulator
- `reset()` - Reset world to initial state
- `add_agent(agent)` - Add agent to world
- `remove_agent(agent)` - Remove agent from world
- `add_object(obj)` - Add object to world
- `remove_object(obj)` - Remove object from world
- `update(dt=None)` - Update world simulation
- `get_sensory_percept()` - Get sensory percept from world
- `get_world_state()` - Get current world state
- `save(filepath)` - Save world state to file
- `load(filepath)` - Load world state from file
- `is_valid_position(x, y, radius=0.0)` - Check if position is valid

### EnvironmentFactory
Create environments for NLM simulations.

#### Methods

- `create_simple_environment(width=100, height=100, num_resources=10)` - Create a simple environment
- `create_complex_environment(width=200, height=200, num_hazards=5, num_resources=20)` - Create a complex environment
- `create_procedural_environment(env_type="forest", seed=None)` - Create a procedural environment
- `create_custom_environment(width=100, height=100, objects=None)` - Create a custom environment

### ScenarioManager
Manage experimental scenarios for NLM simulations.

#### Methods

- `__init__()` - Initialize scenario manager
- `create_scenario(name, description, parameters, environment_type="simple")` - Create a new scenario
- `get_scenario(name)` - Get scenario by name
- `list_scenarios()` - List all scenario names
- `update_scenario(name, **kwargs)` - Update scenario parameters
- `delete_scenario(name)` - Delete scenario
- `load_scenario(name)` - Load scenario for use
- `get_current_scenario()` - Get current scenario
- `end_scenario()` - End current scenario
- `get_scenario_statistics()` - Get scenario management statistics
- `reset()` - Reset scenario manager

### ReplaySystem
Replay recorded experiences.

#### Methods

- `__init__(replay_buffer_size=1000)` - Initialize replay system
- `record_experience(experience)` - Record an experience for replay
- `replay_experiences(start_index=None, end_index=None, filter_criteria=None)` - Replay experiences from buffer
- `analyze_replay(analysis_type, time_window=100, filter_criteria=None)` - Analyze replayed experiences
- `get_replay_statistics()` - Get replay system statistics
- `clear_replay_buffer()` - Clear replay buffer
- `reset()` - Reset replay system

### Plotter
Neural activity visualization.

#### Methods

- `__init__(style="default", figure_size=(12, 8))` - Initialize plotter
- `plot_neural_activity(activity_data, title="Neural Activity", save_path=None)` - Plot neural activity over time
- `plot_connectivity_matrix(connectivity_data, title="Connectivity Matrix", save_path=None)` - Plot connectivity matrix
- `plot_training_curve(training_metrics, title="Training Curves", save_path=None)` - Plot training curves
- `plot_activity_heatmap(activity_data, title="Activity Heatmap", save_path=None)` - Plot activity heatmap
- `plot_metric_comparison(metric_data, metric_names=None, title="Metric Comparison", save_path=None)` - Plot metric comparison
- `create_animation(data_sequence, title="Animation", save_path=None)` - Create animation from data sequence
- `save_figure(fig, filepath)` - Save figure to file
- `close_all_figures()` - Close all open figures
- `reset()` - Reset plotter

### ActivityAnalyzer
Analyze neural patterns and activity.

#### Methods

- `__init__(analysis_methods=None)` - Initialize activity analyzer
- `analyze_activity(activity_data, analysis_types=None)` - Analyze neural activity
- `get_analysis_statistics()` - Get analysis statistics
- `reset()` - Reset analyzer

### HeatmapGenerator
Generate neural heatmaps.

#### Methods

- `__init__(colormap="viridis", normalize=True)` - Initialize heatmap generator
- `generate_neural_heatmap(activity_data, title="Neural Activity Heatmap", save_path=None)` - Generate neural activity heatmap
- `generate_spatial_heatmap(positions, values, title="Spatial Heatmap", save_path=None)` - Generate spatial heatmap
- `generate_connectivity_heatmap(connectivity_matrix, title="Connectivity Heatmap", save_path=None)` - Generate connectivity heatmap
- `generate_activity_heatmap_2d(activity_2d, title="2D Activity Heatmap", save_path=None)` - Generate 2D activity heatmap
- `get_heatmap_statistics()` - Get heatmap generation statistics
- `reset()` - Reset heatmap generator

### ActivityStatistics
Compute statistics from neural activity.

#### Methods

- `__init__(statistics_types=None)` - Initialize activity statistics calculator
- `compute_statistics(activity_data, statistic_types=None)` - Compute statistics from neural activity
- `get_statistics_history()` - Get statistics computation history
- `reset()` - Reset statistics calculator

### ExperimentRunner
Run automated experiments for NLM systems.

#### Methods

- `__init__(n_workers=1, parallelization="sequential")` - Initialize experiment runner
- `create_experiment(name, description, parameters, environment_config=None)` - Create a new experiment
- `get_experiment(name)` - Get experiment by name
- `list_experiments()` - List all experiment names
- `update_experiment(name, **kwargs)` - Update experiment parameters
- `delete_experiment(name)` - Delete experiment
- `run_experiment(name, iterations=1, parallel=False)` - Run an experiment
- `run_batch_experiments(experiment_names, iterations_per_experiment=10)` - Run a batch of experiments
- `get_experiment_statistics()` - Get experiment statistics
- `stop_experiment(name)` - Stop a running experiment
- `cleanup()` - Cleanup experiment runner
- `reset()` - Reset experiment runner

### BatchProcessor
Process multiple experiment runs efficiently.

#### Methods

- `__init__(batch_size=10, n_workers=4, parallelization="thread")` - Initialize batch processor
- `create_batch(batch_id, experiment_names, iterations_per_experiment=10, parameter_combinations=None)` - Create a new batch
- `get_batch(batch_id)` - Get batch by ID
- `list_batches()` - List all batch IDs
- `start_batch(batch_id)` - Start processing a batch
- `process_next_task()` - Process the next task in the queue
- `wait_for_batch_completion(batch_id, timeout=None)` - Wait for batch completion
- `get_batch_statistics()` - Get batch processing statistics
- `stop_batch(batch_id)` - Stop a running batch
- `cleanup()` - Cleanup batch processor
- `reset()` - Reset batch processor

### ResultAggregator
Aggregate experiment results.

#### Methods

- `__init__(aggregation_methods=None)` - Initialize result aggregator
- `aggregate_results(results, experiment_names=None)` - Aggregate experiment results
- `get_aggregation_statistics()` - Get result aggregation statistics
- `reset()` - Reset result aggregator

### ReportGenerator
Generate comprehensive reports.

#### Methods

- `__init__(report_format="text", output_directory="reports")` - Initialize report generator
- `create_report(report_id, title, description, data, template=None)` - Create a new report
- `get_report(report_id)` - Get report by ID
- `list_reports()` - List all report IDs
- `generate_report(report_id, output_path=None)` - Generate report
- `get_report_statistics()` - Get report generation statistics
- `cleanup()` - Cleanup report generator
- `reset()` - Reset report generator

### NeuralNetworkTrainer
Train neural networks with advanced optimization techniques.

#### Methods

- `__init__(learning_rate=0.001, batch_size=32, optimizer="adam", loss_function="mse", regularization=None)` - Initialize neural network trainer
- `initialize_model(architecture)` - Initialize neural network model
- `train_model(X_train, y_train, X_val, y_val, epochs=100, verbose=False)` - Train neural network model
- `save_model(filepath)` - Save trained model
- `load_model(filepath)` - Load trained model
- `get_training_statistics()` - Get training statistics
- `reset()` - Reset trainer to initial state

### PolicyGradientLearner
Policy gradient methods for reinforcement learning.

#### Methods

- `__init__(policy_network, learning_rate=0.001, discount_factor=0.99, entropy_coefficient=0.01)` - Initialize policy gradient learner
- `start_episode()` - Start a new episode
- `end_episode()` - End the current episode and compute returns
- `step(state, action, reward, done, log_prob)` - Process a step in the episode
- `get_training_statistics()` - Get training statistics
- `reset()` - Reset learner to initial state

### ReinforcementLearning
Comprehensive reinforcement learning framework.

#### Methods

- `__init__(algorithm="reinforce", env_config=None)` - Initialize reinforcement learning system
- `train(max_episodes=1000, max_steps_per_episode=100, evaluation_interval=10, verbose=False)` - Train reinforcement learning agent
- `evaluate(n_episodes=10)` - Evaluate trained agent
- `save_agent(filepath)` - Save trained agent
- `load_agent(filepath)` - Load trained agent
- `get_training_statistics()` - Get training statistics
- `reset()` - Reset reinforcement learning system

### MetaLearner
Meta-learning capabilities for rapid adaptation.

#### Methods

- `__init__(meta_learner_type="maml", inner_loop_steps=5, outer_loop_steps=100)` - Initialize meta-learner
- `create_task(task_type, task_params)` - Create a new learning task
- `execute_task(task, support_set, query_set=None)` - Execute a learning task
- `get_meta_learning_statistics()` - Get meta-learning statistics
- `save_meta_learner(filepath)` - Save meta-learner
- `load_meta_learner(filepath)` - Load meta-learner
- `reset()` - Reset meta-learner

## Enumerations

### LearningRule
- `STDP` - Spike-Timing-Dependent Plasticity
- `HEBBIAN` - Hebbian learning
- `LTP` - Long-Term Potentiation
- `LTD` - Long-Term Depression
- `Oja` - Oja's rule
- `BCM` - BCM learning rule

### PlasticityType
- `SYNAPTIC` - Synaptic plasticity
- `STRUCTURAL` - Structural plasticity
- `NEURONAL` - Neuronal plasticity

### NeuromodulatorType
- `DOPAMINE` - Dopamine
- `SEROTONIN` - Serotonin
- `NORADRENALIN` - Noradrenaline
- `ACETYLCHOLINE` - Acetylcholine
- `OXYTOCIN` - Oxytocin
- `ENDORPHIN` - Endorphin

### EnvironmentType
- `SIMPLE` - Simple environment
- `COMPLEX` - Complex environment
- `PROCEDURAL` - Procedural environment
- `CUSTOM` - Custom environment

## Function References

### Configuration Management

- `nlm_config()` - Create a high-level configuration manager class
- `load_config_file(filepath, auto_detect=True)` - Load config with automatic format detection
- `save_config(config, filepath, format="auto")` - Save config with format choice
- `validate_config(config, schema=None)` - Validate config against schemas
- `compare_configs(config1, config2, tolerance=0.001)` - Compare two configs

### Brain and Neural Network Management

- `BrainNetwork(config=None)` - High-level brain network class
- `create_network(config=None)` - Create and configure neural networks
- `import_brain(filepath)` - Import pre-trained brain models
- `export_brain(brain, filepath, format="auto")` - Export brain models for sharing
- `simulate_brain(brain, steps, dt=0.1)` - Run brain simulations with results

### Memory System Integration

- `EpisodicMemory(capacity=1000, consolidation_threshold=0.7)` - High-level episodic memory interface
- `ConsolidationManager(episodic_memory)` - Automatic memory consolidation
- `MemoryProfiler(episodic_memory, working_memory)` - Memory usage analysis
- `RetrievalOptimizer(episodic_memory, working_memory)` - Memory retrieval optimization

### Learning and Plasticity Control

- `LearningScheduler(base_interval=1.0, adaptation_rate=0.1)` - Schedule learning sessions
- `PlasticityController()` - Control plasticity rules
- `AdaptiveLearning()` - Adaptive learning algorithms
- `CuriosityDrivenLearning(exploration_rate=0.1, novelty_threshold=0.5)` - Intrinsic motivation integration

### Neuromodulation and Reward Systems

- `DopamineSystem(learning_rate=0.1, prediction_horizon=10)` - High-level dopamine modeling
- `RewardEngine(discount_factor=0.9, exploration_bonus=0.1)` - Reward processing and prediction
- `PredictionErrorTracker(time_window=100)` - Track prediction errors
- `MotivationSystem(motivation_weights=None)` - Integrate motivation signals

### Agent Control and Management

- `AgentTrainer(learning_rate=0.01, batch_size=32, discount_factor=0.99, entropy_coefficient=0.01)` - Train agents with advanced features
- `Evaluator(evaluation_metrics=None)` - Evaluate agent performance
- `CheckpointManager(checkpoint_dir="checkpoints")` - Save/load agent states
- `PerformanceMonitor(monitoring_interval=1.0)` - Monitor agent performance

### Environment and World Management

- `WorldSimulator(width=100, height=100, time_step=0.1, physics_enabled=True)` - Advanced world simulation
- `EnvironmentFactory.create_simple_environment()` - Create environments
- `EnvironmentFactory.create_complex_environment()` - Create environments
- `EnvironmentFactory.create_procedural_environment()` - Create environments
- `ScenarioManager()` - Manage experimental scenarios
- `ReplaySystem(replay_buffer_size=1000)` - Replay recorded experiences

### Visualization and Analysis

- `Plotter(style="default", figure_size=(12, 8))` - Neural activity visualization
- `ActivityAnalyzer(analysis_methods=None)` - Analyze neural patterns
- `HeatmapGenerator(colormap="viridis", normalize=True)` - Generate neural heatmaps
- `ActivityStatistics(statistics_types=None)` - Compute statistics

### Automation and Scripting

- `ExperimentRunner(n_workers=1, parallelization="sequential")` - Run automated experiments
- `BatchProcessor(batch_size=10, n_workers=4, parallelization="thread")` - Process multiple runs
- `ResultAggregator(aggregation_methods=None)` - Aggregate experiment results
- `ReportGenerator(report_format="text", output_directory="reports")` - Generate comprehensive reports

### Advanced Machine Learning Integration

- `NeuralNetworkTrainer(learning_rate=0.001, batch_size=32, optimizer="adam", loss_function="mse", regularization=None)` - Train neural networks
- `PolicyGradientLearner(policy_network, learning_rate=0.001, discount_factor=0.99, entropy_coefficient=0.01)` - Policy gradient methods
- `ReinforcementLearning(algorithm="reinforce", env_config=None)` - RL algorithms
- `MetaLearner(meta_learner_type="maml", inner_loop_steps=5, outer_loop_steps=100)` - Meta-learning capabilities

## Usage Examples

### Basic NLM Usage

```python
import nlm

# Create NLM system
nlm_system = nlm.NLM()

# Create brain with configuration
brain = nlm_system.create_agent({"regions": 5, "neurons_per_region": 100})

# Run simulation
results = nlm_system.run_simulation(steps=500)

# Evaluate performance
evaluation = nlm_system.evaluate()
print(f"Performance: {evaluation.get('overall_score', 0):.2f}")
```

### Advanced Brain Network

```python
import nlm
import numpy as np

# Create sophisticated brain network
brain = nlm.BrainNetwork()
brain.add_region("visual_cortex", 300, "excitatory")
brain.add_region("motor_cortex", 250, "motor")
brain.add_region("hippocampus", 150, "internal")

# Initialize and simulate
brain.initialize()
simulation_results = nlm.simulate_brain(brain, steps=1000, dt=0.05)

# Analyze statistics
stats = brain.get_statistics()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total regions: {stats['total_regions']}")
print(f"Firing rate: {stats['avg_neurons_per_region']:.1f}")
```

### Memory Management

```python
import nlm

# Create memory systems
episodic_memory = nlm.EpisodicMemory(capacity=1000, consolidation_threshold=0.7)
working_memory = nlm.WorkingMemory(capacity=50, decay_rate=0.02)

# Encode and retrieve experiences
episode_id = episodic_memory.encode({"stimulus": "red", "action": "approach"}, time.time())
retrieved = episodic_memory.retrieve(filter_keywords=["approach"])

# Consolidate and analyze
consolidated = episodic_memory.consolidate()
patterns = episodic_memory.analyze_patterns()
```

### Learning and Plasticity

```python
import nlm

# Configure plasticity
plasticity_controller = nlm.PlasticityController()
plasticity_controller.set_rule(nlm.LearningRule.STDP, learning_rate=0.01)

# Set up adaptive learning
adaptive_learning = nlm.AdaptiveLearning()
adaptive_learning.add_algorithm(
    "td_learning",
    lambda perf, hist, params: perf * 0.1,
    {"discount": 0.99}
)

# Curiosity-driven learning
curiosity_learning = nlm.CuriosityDrivenLearning(exploration_rate=0.1)
```

### Neuromodulation

```python
import nlm

# Create neuromodulation system
dopamine_system = nlm.DopamineSystem(learning_rate=0.1)
reward_engine = nlm.RewardEngine(discount_factor=0.9)

# Simulate experience
for step in range(50):
    reward = np.random.randn() * 10 + 5
    dopamine_result = dopamine_system.update(reward, reward * 0.8)
    reward_result = reward_engine.process_reward(reward)
```

### Agent Training

```python
import nlm

# Create agent trainer
trainer = nlm.AgentTrainer(learning_rate=0.01, batch_size=32)

# Set up evaluator
evaluator = nlm.Evaluator()

# Create checkpoint manager
checkpoint_manager = nlm.CheckpointManager()

# Train and evaluate
for iteration in range(100):
    # Add training experience
    trainer.add_experience(state, action, reward, next_state, done)
    
    # Train agent
    trainer.train_step(agent_brain)

# Evaluate
evaluation = evaluator.evaluate(agent_brain, world)
print(f"Evaluation score: {evaluation['overall_score']}")
```

### Experiment Automation

```python
import nlm

# Create experiment runner
experiment_runner = nlm.ExperimentRunner()

# Create experiments
experiments = [
    {"name": "simple_brain", "parameters": {"regions": 3}},
    {"name": "complex_brain", "parameters": {"regions": 10}},
]

# Run experiments
for exp in experiments:
    results = experiment_runner.run_experiment(exp["name"], iterations=10)

# Aggregate results
aggregated = experiment_runner.analyze_results(list(results.values()))
```

### Machine Learning Integration

```python
import nlm
import numpy as np

# Create neural network trainer
trainer = nlm.NeuralNetworkTrainer(
    learning_rate=0.001,
    batch_size=32,
    optimizer="adam",
    loss_function="mse"
)

# Initialize model
architecture = {
    "layers": [
        {"type": "dense", "units": 128, "activation": "relu"},
        {"type": "dense", "units": 64, "activation": "relu"},
        {"type": "dense", "units": 1, "activation": "linear"}
    ]
}

trainer.initialize_model(architecture)

# Train model
results = trainer.train_model(X_train, y_train, X_val, y_val)
```

### Reinforcement Learning

```python
import nlm

# Create reinforcement learning system
rl_system = nlm.ReinforcementLearning(
    algorithm="reinforce",
    env_config={"width": 100, "height": 100}
)

# Train agent
results = rl_system.train(max_episodes=500)

# Evaluate agent
evaluation = rl_system.evaluate()
print(f"Evaluation reward: {evaluation['average_reward']}")
```

### Visualization

```python
import nlm
import matplotlib.pyplot as plt

# Create neural activity plotter
plotter = nlm.Plotter(style="scientific")

# Generate sample data
activity_data = np.random.randn(100, 1000)

# Plot neural activity
fig = plotter.plot_neural_activity(activity_data, title="Neural Activity")
plt.show()

# Plot connectivity matrix
connectivity = np.random.rand(50, 50)
fig = plotter.plot_connectivity_matrix(connectivity, title="Connectivity Matrix")
plt.show()
```

## Requirements

### Python Dependencies

```
numpy>=1.20
scipy>=1.6
matplotlib>=3.3
seaborn>=0.11
joblib>=0.14
psutil>=5.8
python-dateutil>=2.8
pyyaml>=5.3
jsonschema>=3.2

# For advanced visualization
pillow>=8.0
tqdm>=4.60

# For machine learning
scikit-learn>=0.24
```

### System Requirements

- **Operating System**: Linux, macOS, or Windows
- **Python Version**: 3.8 or higher
- **Memory**: Minimum 8GB RAM, recommended 16GB+
- **Disk Space**: Minimum 1GB for installation
- **Compiler**: C++ compiler for building extensions (if needed)

## Installation

### From PyPI

```bash
pip install nlm
```

### From Source

```bash
# Clone the repository
clone https://github.com/nlm-project/nlm.git

# Install Python dependencies
pip install -r requirements.txt

# Install Python bindings
pip install -e python/

# Build and install C++ core (if needed)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Testing

### Unit Tests

```bash
pytest python/nlm/tests/ -v
```

### Integration Tests

```bash
pytest python/integration_tests/ -v
```

### Performance Tests

```bash
python -m pytest python/performance_tests/ -v
```

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Authors

NLM Authors - Research team at NLM Project

## Contact

For questions, support, or feature requests:
- **GitHub**: https://github.com/nlm-project/nlm
- **Email**: support@nlm.org
- **Documentation**: https://nlm.readthedocs.io

## Acknowledgements

- Special thanks to the NLM core development team
- Appreciation for the open-source community
- Thanks to all contributors and beta testers
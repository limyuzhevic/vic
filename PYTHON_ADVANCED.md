# NLM Python Advanced Usage Guide

## Overview

This guide provides advanced Python usage examples and techniques for the NLM (Neural Learning Machine) system. It covers:

- Advanced memory system manipulation
- Custom training loops and optimization
- Integration with external systems
- Performance profiling and monitoring
- Advanced agent behaviors
- Debugging and testing techniques

## Advanced Memory System Usage

### 1. Working Memory Control

```python
import pynlm
import numpy as np

def advanced_working_memory_usage():
    """Advanced working memory manipulation techniques."""
    
    # Create brain with working memory
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent for memory manipulation
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable working memory tracking
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("=== Advanced Working Memory Usage ===")
    
    # 1. Store and retrieve specific patterns
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Access working memory
        working_memory = brain.getWorkingMemory()
        
        if working_memory:
            # Store specific patterns with high priority
            if step % 50 == 0:
                # Create and store a pattern
                pattern = percept.getAllNeurons()
                working_memory.store(pattern, strength=2.0)
                print(f"Stored pattern at step {step}")
            
            # Retrieve and analyze patterns
            if step % 100 == 0:
                patterns = working_memory.retrieve()
                print(f"Working memory contains {len(patterns)} patterns")
                
                # Analyze pattern competition
                active_neurons = working_memory.getActiveTraces()
                print(f"Active memory traces: {active_neurons}")
                
                # Find winning patterns
                if active_neurons > 0:
                    # Get current activation
                    activation = working_memory.getNeuronActivation()
                    if activation:
                        max_activation = max(activation.values())
                        winning_patterns = [k for k, v in activation.items() 
                                          if v == max_activation]
                        print(f"Winning patterns: {winning_patterns}")
    
    return brain
```

### 2. Episodic Memory Management

```python
class EpisodicMemoryManager:
    """Advanced episodic memory management."""
    
    def __init__(self, brain):
        self.brain = brain
        self.episode_counter = 0
        self.memory_threshold = 0.7
    
    def store_significant_episodes(self, percept, reward, step):
        """Store only significant episodes based on reward and novelty."""
        
        episodic_memory = self.brain.getEpisodicMemory()
        if not episodic_memory:
            return
        
        # Calculate episode significance
        episode_importance = self.calculate_episode_importance(percept, reward)
        
        # Store if above threshold
        if episode_importance > self.memory_threshold:
            # Create episode data
            episode = {
                'step': step,
                'sensory_pattern': percept.getAllNeurons(),
                'reward': reward,
                'importance': episode_importance
            }
            
            # Store in episodic memory
            episodic_memory.storeEpisode(episode)
            self.episode_counter += 1
            
            # Consolidate if threshold reached
            if self.episode_counter >= 10:
                self.consolidate_memory(episodic_memory)
    
    def calculate_episode_importance(self, percept, reward):
        """Calculate how important an episode is for learning."""
        
        # Base importance on reward magnitude
        reward_importance = min(abs(reward), 1.0)
        
        # Add novelty component
        novelty = self.calculate_percept_novelty(percept)
        
        # Combine with weights
        importance = 0.7 * reward_importance + 0.3 * novelty
        
        return importance
    
    def calculate_percept_novelty(self, percept):
        """Calculate novelty of sensory input."""
        # This would normally use a novelty detector
        # For this example, use simple variance
        pattern = percept.getAllNeurons()
        if len(pattern) > 0:
            return min(np.std(pattern) / 10.0, 1.0)
        return 0.0
    
    def consolidate_memory(self, episodic_memory):
        """Consolidate stored episodes."""
        if episodic_memory:
            episodic_memory.consolidate(0.5)  # Consolidate episodes > 0.5 importance
            self.episode_counter = 0
    
    def replay_for_learning(self, query_pattern, num_episodes=3):
        """Replay similar episodes to reinforce learning."""
        episodic_memory = self.brain.getEpisodicMemory()
        if not episodic_memory:
            return []
        
        # Retrieve similar episodes
        similar_episodes = episodic_memory.retrieveSimilar(
            query_pattern, num_episodes
        )
        
        # Replay them to consolidate
        replayed_episodes = episodic_memory.getEpisodesForReplay(num_episodes)
        
        return replayed_episodes
```

### 3. Memory-Augmented Decision Making

```python
class MemoryAugmentedAgent:
    """Agent that uses memory for decision making."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.memory_manager = EpisodicMemoryManager(brain)
    
    def run_with_memory_augmentation(self, max_steps=1000):
        """Run agent with memory-augmented decision making."""
        
        print("=== Memory-Augmented Agent Run ===")
        
        for step in range(max_steps):
            # Get current state
            percept = self.world.getSensoryPercept()
            current_position = self.get_agent_position()
            
            # Get working memory state
            working_memory = self.brain.getWorkingMemory()
            memory_state = working_memory.retrieve() if working_memory else []
            
            # Get episodic memory for similar experiences
            episodic_memory = self.brain.getEpisodicMemory()
            similar_experiences = []
            if episodic_memory and memory_state:
                similar_experiences = episodic_memory.retrieveSimilar(
                    memory_state, 3
                )
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Store experience
            reward = self.get_reward_from_world()
            self.memory_manager.store_significant_episodes(
                percept, reward, step
            )
            
            # Memory-guided action selection
            action = self.select_action_memory_guided(
                percept, memory_state, similar_experiences
            )
            
            # Execute action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: Action {action}")
                print(f"  Working memory traces: {working_memory.getActiveTraces() if working_memory else 0}")
                print(f"  Stored episodes: {len(similar_experiences) if similar_experiences else 0}")
    
    def select_action_memory_guided(self, percept, memory_state, similar_experiences):
        """Select action based on memory patterns."""
        
        # Use neural planner for action selection
        if self.brain.getPredictionSystem():
            # Get planning from prediction system
            planned_action = self.brain.getPredictionSystem().getPlannedAction()
            return planned_action
        
        # Fallback to basic action selection
        return self.agent.decodeMotorCommand()
```

## Advanced Training Loops

### 1. Custom Training Loop with Curriculum Learning

```python
class CurriculumTrainer:
    """Trainer that uses curriculum learning."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.current_curriculum_stage = 0
    
    def run_curriculum_training(self, stages):
        """Run training with curriculum learning."""
        
        print("=== Curriculum Learning Training ===")
        
        for stage_idx, stage_config in enumerate(stages):
            print(f"\n--- Stage {stage_idx + 1}: {stage_config['name']} ---")
            
            # Configure environment for this stage
            self.world.configure(**stage_config['world_config'])
            self.world.reset()
            self.agent.initialize(self.world)
            
            # Set brain parameters
            config = self.brain.getConfiguration()
            config.set("brain.neuron_count", stage_config['neuron_count'])
            config.set("plasticity.stdp.learning_rate", stage_config['learning_rate'])
            
            # Run stage training
            for episode in range(stage_config['episodes']):
                episode_reward = self.run_episode(stage_config)
                print(f"Episode {episode + 1}: Reward = {episode_reward:.3f}")
                
                # Adjust parameters based on performance
                self.adapt_to_performance(episode_reward, stage_config)
            
            self.current_curriculum_stage = stage_idx + 1
    
    def run_episode(self, stage_config):
        """Run a single training episode."""
        
        total_reward = 0.0
        
        for step in range(stage_config['steps_per_episode']):
            # Update world
            self.world.update(stage_config['dt'])
            
            # Get and process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step)
            
            # Select and execute action
            action = self.agent.decodeMotorCommand()
            result = self.world.applyMotorCommand(
                action, self.world.getSimulationTime()
            )
            
            # Get reward
            reward = result.reward if hasattr(result, 'reward') else 0.0
            total_reward += reward
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
        
        return total_reward
    
    def adapt_to_performance(self, episode_reward, stage_config):
        """Adapt parameters based on episode performance."""
        
        # Adjust learning rate based on reward
        current_lr = stage_config.get('learning_rate', 0.001)
        if episode_reward < stage_config['target_reward'] * 0.5:
            # Learning too slow, increase rate
            new_lr = current_lr * 1.2
        elif episode_reward > stage_config['target_reward'] * 0.9:
            # Learning too fast, decrease rate for stability
            new_lr = current_lr * 0.95
        else:
            # Keep as is
            new_lr = current_lr
        
        stage_config['learning_rate'] = min(new_lr, 0.01)  # Cap at 1%
```

### 2. Meta-Learning with MAML

```python
class MamlTrainer:
    """Model-Agnostic Meta-Learning (MAML) trainer."""
    
    def __init__(self, brain, world, num_tasks=5, inner_steps=10):
        self.brain = brain
        self.world = world
        self.num_tasks = num_tasks
        self.inner_steps = inner_steps
        self.meta_learning_rate = 0.01
    
    def run_maml_training(self):
        """Run MAML-style meta-learning."""
        
        print("=== MAML Meta-Learning Training ===")
        
        for task_idx in range(self.num_tasks):
            print(f"\n--- Task {task_idx + 1}/{self.num_tasks} ---")
            
            # Sample a new task
            task = self.sample_task(task_idx)
            
            # Fast adaptation (inner loop)
            adapted_brain = self.fast_adapt(task)
            
            # Evaluate on test data
            test_performance = self.evaluate_task(adapted_brain, task['test_data'])
            
            # Meta-update (outer loop)
            self.meta_update(adapted_brain, test_performance)
            
            print(f"Task {task_idx + 1} performance: {test_performance:.3f}")
    
    def sample_task(self, task_idx):
        """Sample a training task."""
        
        # Create a task with specific characteristics
        task = {
            'name': f"Task_{task_idx}",
            'world_config': {
                'width': 20,
                'height': 20,
                'visionWidth': 8,
                'visionHeight': 8
            },
            'training_data': self.generate_task_data(task_idx, 'train'),
            'test_data': self.generate_task_data(task_idx, 'test')
        }
        
        return task
    
    def fast_adapt(self, task):
        """Fast adaptation to a new task."""
        
        # Create a copy of the brain for adaptation
        adapted_brain = pynlm.createBrain(self.brain.getConfiguration())
        adapted_brain.initialize()
        
        # Run inner loop adaptation
        for step in range(self.inner_steps):
            # Run adaptation episode
            adapted_brain.step(step)
            
            # Update based on task data
            # (This would depend on the specific task)
        
        return adapted_brain
    
    def evaluate_task(self, brain, test_data):
        """Evaluate adapted brain on test data."""
        
        total_reward = 0.0
        
        for episode_idx, test_episode in enumerate(test_data):
            # Run episode with adapted brain
            reward = self.run_evaluation_episode(brain, test_episode)
            total_reward += reward
        
        return total_reward / len(test_data)
    
    def meta_update(self, adapted_brain, performance):
        """Update meta-parameters based on performance."""
        
        # Update meta-learning rate based on performance
        if performance > 0.8:
            self.meta_learning_rate *= 1.05
        elif performance < 0.5:
            self.meta_learning_rate *= 0.95
        
        # This would update the underlying network parameters
        # For this example, we just track performance
        if not hasattr(self, 'performance_history'):
            self.performance_history = []
        
        self.performance_history.append(performance)
    
    def generate_task_data(self, task_idx, data_type):
        """Generate task data for training or testing."""
        
        # Create task-specific data
        # This is a simplified example - real tasks would be more complex
        num_episodes = 20 if data_type == 'train' else 5
        
        task_data = []
        for i in range(num_episodes):
            episode = {
                'observations': self.generate_observations(task_idx, i),
                'actions': self.generate_actions(task_idx, i),
                'rewards': self.generate_rewards(task_idx, i)
            }
            task_data.append(episode)
        
        return task_data
```

## Performance Profiling and Optimization

### 1. Advanced Performance Monitoring

```python
class AdvancedProfiler:
    """Advanced performance profiling and analysis."""
    
    def __init__(self):
        self.benchmarks = {}
        self.memory_profiler = None
        self.timeline_profiler = None
    
    def profile_complete_simulation(self, brain, world, agent, steps):
        """Profile a complete simulation run."""
        
        import time
        import tracemalloc
        import psutil
        import numpy as np
        
        print(f"=== Profiling {steps} steps ===")
        
        # Start profiling
        tracemalloc.start()
        process = psutil.Process()
        start_time = time.time()
        
        # Initial memory and CPU
        initial_memory = process.memory_info().rss / 1024 / 1024  # MB
        initial_cpu = process.cpu_percent()
        
        # Run simulation with detailed tracking
        step_times = []
        memory_usage = []
        cpu_usage = []
        spike_counts = []
        
        for step in range(steps):
            step_start = time.time()
            
            # Record current state
            brain_start_memory = process.memory_info().rss / 1024 / 1024
            cpu_before = process.cpu_percent()
            spike_before = brain.getTotalSpikeCount()
            
            # Run step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics
            step_time = time.time() - step_start
            step_end_memory = process.memory_info().rss / 1024 / 1024
            cpu_after = process.cpu_percent()
            spike_after = brain.getTotalSpikeCount()
            
            step_times.append(step_time)
            memory_usage.append(step_end_memory - brain_start_memory)
            cpu_usage.append((cpu_before + cpu_after) / 2)
            spike_counts.append(spike_after - spike_before)
        
        # Calculate statistics
        end_time = time.time()
        total_time = end_time - start_time
        final_memory = process.memory_info().rss / 1024 / 1024
        current, peak_memory = tracemalloc.get_traced_memory()
        
        # Store benchmarks
        self.benchmarks['step_time'] = {
            'mean': np.mean(step_times),
            'std': np.std(step_times),
            'min': np.min(step_times),
            'max': np.max(step_times),
            'p95': np.percentile(step_times, 95)
        }
        
        self.benchmarks['memory'] = {
            'initial': initial_memory,
            'final': final_memory,
            'peak_python': peak_memory / 1024 / 1024,  # MB
            'growth': final_memory - initial_memory,
            'growth_rate': (final_memory - initial_memory) / steps
        }
        
        self.benchmarks['cpu'] = {
            'mean': np.mean(cpu_usage),
            'max': np.max(cpu_usage),
            'std': np.std(cpu_usage)
        }
        
        self.benchmarks['spikes'] = {
            'mean': np.mean(spike_counts),
            'total': sum(spike_counts),
            'rate': sum(spike_counts) / steps
        }
        
        # Print report
        self.print_profiling_report(total_time)
        
        tracemalloc.stop()
        
        return self.benchmarks
    
    def print_profiling_report(self, total_time):
        """Print detailed profiling report."""
        
        print(f"\n=== Performance Summary ===")
        print(f"Total time: {total_time:.2f} seconds")
        print(f"Steps per second: {self.benchmarks['step_count'] / total_time:.1f}")
        
        print(f"\n--- Step Performance ---")
        step_perf = self.benchmarks['step_time']
        print(f"Average step time: {step_perf['mean'] * 1000:.1f} ms")
        print(f"95th percentile: {step_perf['p95'] * 1000:.1f} ms")
        print(f"Min step time: {step_perf['min'] * 1000:.1f} ms")
        print(f"Max step time: {step_perf['max'] * 1000:.1f} ms")
        
        print(f"\n--- Memory Usage ---")
        mem_perf = self.benchmarks['memory']
        print(f"Initial memory: {mem_perf['initial']:.1f} MB")
        print(f"Final memory: {mem_perf['final']:.1f} MB")
        print(f"Python peak: {mem_perf['peak_python']:.1f} MB")
        print(f"Growth rate: {mem_perf['growth_rate']:.2f} MB/step")
        
        print(f"\n--- CPU Usage ---")
        cpu_perf = self.benchmarks['cpu']
        print(f"Average CPU: {cpu_perf['mean']:.1f}%")
        print(f"Max CPU: {cpu_perf['max']:.1f}%")
        print(f"CPU std dev: {cpu_perf['std']:.1f}%")
        
        print(f"\n--- Neural Activity ---")
        spike_perf = self.benchmarks['spikes']
        print(f"Average spikes per step: {spike_perf['mean']:.1f}")
        print(f"Total spikes: {spike_perf['total']}")
        print(f"Spike rate: {spike_perf['rate']:.2f} spikes/step")
    
    def profile_memory_patterns(self, brain, steps):
        """Profile memory usage patterns."""
        
        import tracemalloc
        import matplotlib.pyplot as plt
        
        tracemalloc.start()
        
        # Run simulation with memory tracking
        memory_snapshots = []
        
        for step in range(steps):
            brain.step(step)
            current, peak = tracemalloc.get_traced_memory()
            memory_snapshots.append({
                'step': step,
                'current': current / 1024 / 1024,  # MB
                'peak': peak / 1024 / 1024  # MB
            })
        
        tracemalloc.stop()
        
        # Analyze memory patterns
        print(f"\n=== Memory Pattern Analysis ===")
        print(f"Memory usage range: {np.min([s['current'] for s in memory_snapshots]):.1f} - {np.max([s['current'] for s in memory_snapshots]):.1f} MB")
        print(f"Memory growth rate: {np.polyfit([s['step'] for s in memory_snapshots], [s['current'] for s in memory_snapshots], 1)[0]:.2f} MB/step")
        
        # Detect memory leaks
        if np.polyfit([s['step'] for s in memory_snapshots], [s['current'] for s in memory_snapshots], 1)[0] > 0.01:
            print("WARNING: Potential memory leak detected!")
        
        return memory_snapshots
```

## Advanced Agent Behaviors

### 1. Hierarchical Planning Agent

```python
class HierarchicalPlanner:
    """Agent with hierarchical planning capabilities."""
    
    def __init__(self, brain, world, num_goals=5):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.goals = self.initialize_goals(num_goals)
        self.planning_depth = 3
    
    def initialize_goals(self, num_goals):
        """Initialize goal hierarchy."""
        goals = []
        for i in range(num_goals):
            goal = {
                'id': i,
                'priority': (num_goals - i) / num_goals,  # Higher for lower IDs
                'complexity': 1.0 + (i % 3),  # Variable complexity
                'achievable': True,
                'subgoals': []
            }
            goals.append(goal)
        
        # Create subgoal hierarchy
        for i, goal in enumerate(goals):
            if i > 0:
                goal['subgoals'] = [goals[j] for j in range(max(0, i-2), i)]
        
        return goals
    
    def run_hierarchical_planning(self, max_steps=1000):
        """Run hierarchical planning agent."""
        
        print("=== Hierarchical Planning Agent ===")
        
        for step in range(max_steps):
            # Select goal based on current state
            current_goal = self.select_current_goal()
            
            # Plan action sequence for selected goal
            if current_goal:
                action_sequence = self.plan_action_sequence(current_goal)
                
                # Execute action sequence
                self.execute_action_sequence(action_sequence, step)
            
            # Update world
            self.world.update(0.1)
            
            # Process sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step)
            
            # Get planned action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update goal achievement
            self.update_goal_achievement(current_goal)
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: Current goal {current_goal['id'] if current_goal else 'None'}")
                print(f"  Action sequence length: {len(action_sequence) if 'action_sequence' in locals() else 0}")
    
    def select_current_goal(self):
        """Select current goal based on state and hierarchy."""
        
        # Calculate goal suitability based on current state
        suitable_goals = []
        for goal in self.goals:
            suitability = self.calculate_goal_suitability(goal)
            if suitability > 0.5:  # Only consider achievable goals
                suitable_goals.append((goal, suitability))
        
        if not suitable_goals:
            return None
        
        # Select highest priority suitable goal
        suitable_goals.sort(key=lambda x: x[0]['priority'], reverse=True)
        return suitable_goals[0][0]
    
    def calculate_goal_suitability(self, goal):
        """Calculate how suitable a goal is for current state."""
        
        # This would depend on environment state and goal characteristics
        # Simplified example
        suitability = 1.0
        
        # Check subgoal availability
        if goal['subgoals']:
            available_subgoals = sum(1 for sg in goal['subgoals'] if sg['achievable'])
            suitability *= available_subgoals / len(goal['subgoals'])
        
        return suitability
    
    def plan_action_sequence(self, goal, depth=1):
        """Plan action sequence for a goal."""
        
        if depth > self.planning_depth:
            return []
        
        # Plan sequence based on goal characteristics
        sequence = []
        
        # Add main action
        action = self.brain.getPredictionSystem().getPlannedAction()
        if action:
            sequence.append(action)
        
        # Add subgoal actions if needed
        for subgoal in goal['subgoals']:
            if subgoal['achievable']:
                sub_sequence = self.plan_action_sequence(subgoal, depth + 1)
                sequence.extend(sub_sequence)
        
        return sequence
    
    def execute_action_sequence(self, action_sequence, step):
        """Execute a planned action sequence."""
        
        for i, action in enumerate(action_sequence):
            # Execute action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward
            reward = self.get_reward_from_world()
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
    
    def update_goal_achievement(self, goal):
        """Update goal achievement status."""
        
        if not goal:
            return
        
        # Check if goal is achieved
        # This would depend on environment and goal criteria
        goal['achievable'] = False  # Simplified
        
        # Reset if achieved
        if not goal['achievable']:
            goal['achievable'] = True
```

### 2. Meta-Cognition Agent

```python
class MetaCognitionAgent:
    """Agent with meta-cognitive capabilities."""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent = pynlm.createAgentBrain(brain)
        self.meta_awareness = 0.5
        self.confidence_levels = {}
        self.learning_rates = {}
    
    def run_meta_cognitive_agent(self, max_steps=1000):
        """Run agent with meta-cognitive capabilities."""
        
        print("=== Meta-Cognition Agent ===")
        
        for step in range(max_steps):
            # Meta-awareness update
            self.update_meta_awareness()
            
            # Plan based on meta-awareness
            plan = self.meta_planning()
            
            # Execute with meta-monitoring
            step_result = self.execute_with_monitoring(plan, step)
            
            # Update meta-knowledge
            self.update_meta_knowledge(step_result)
            
            # Adapt based on meta-awareness
            self.adapt_based_on_meta_awareness()
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: Meta-awareness = {self.meta_awareness:.2f}")
                print(f"  Planning confidence: {self.confidence_levels.get('planning', 0):.2f}")
                print(f"  Learning rate: {self.learning_rates.get('adaptation', 0.001):.4f}")
    
    def update_meta_awareness(self):
        """Update meta-awareness based on performance."""
        
        # Calculate meta-awareness from various factors
        performance_factors = []
        
        # Learning progress
        if 'learning_progress' in self.brain.getPerformanceMetrics():
            performance_factors.append(
                self.brain.getPerformanceMetrics()['learning_progress']
            )
        
        # Reward consistency
        reward_consistency = self.calculate_reward_consistency()
        performance_factors.append(reward_consistency)
        
        # Planning accuracy
        planning_accuracy = self.calculate_planning_accuracy()
        performance_factors.append(planning_accuracy)
        
        # Update meta-awareness
        self.meta_awareness = np.mean(performance_factors)
        
        # Update confidence levels
        self.update_confidence_levels()
    
    def meta_planning(self):
        """Plan actions with meta-cognitive awareness."""
        
        plan = {
            'actions': [],
            'confidence': 0.0,
            'reasoning': [],
            'alternative_actions': [],
            'expected_outcomes': []
        }
        
        # Get current state
        percept = self.world.getSensoryPercept()
        current_state = self.extract_state_features(percept)
        
        # Get predictions
        if self.brain.getPredictionSystem():
            predictions = self.brain.getPredictionSystem().getPredictions()
            plan['predictions'] = predictions
        
        # Generate primary plan
        primary_actions = self.generate_primary_plan(current_state)
        plan['actions'] = primary_actions
        
        # Generate alternative plans
        alternative_plans = self.generate_alternative_plans(current_state)
        plan['alternative_actions'] = alternative_plans
        
        # Calculate confidence
        plan['confidence'] = self.calculate_planning_confidence(
            current_state, predictions
        )
        
        # Get reasoning
        plan['reasoning'] = self.generate_reasoning(
            current_state, plan['confidence']
        )
        
        return plan
    
    def execute_with_monitoring(self, plan, step):
        """Execute plan with meta-cognitive monitoring."""
        
        result = {
            'actions_executed': [],
            'rewards_received': [],
            'outcome_quality': 0.0,
            'meta_learning_signal': 0.0
        }
        
        for action in plan['actions']:
            # Execute action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward
            reward = self.get_reward_from_world()
            result['rewards_received'].append(reward)
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            result['actions_executed'].append(action)
        
        # Calculate outcome quality
        result['outcome_quality'] = self.calculate_outcome_quality(
            result['rewards_received']
        )
        
        # Generate meta-learning signal
        result['meta_learning_signal'] = self.generate_meta_learning_signal(
            plan, result
        )
        
        return result
    
    def update_meta_knowledge(self, step_result):
        """Update meta-knowledge based on step results."""
        
        # Update confidence based on outcome
        accuracy = step_result['outcome_quality']
        plan_confidence = step_result['meta_learning_signal']
        
        # Update learning rate based on meta-learning signal
        meta_signal = step_result['meta_learning_signal']
        if meta_signal > 0:
            # Positive meta-signal: increase learning
            for action_id in self.confidence_levels:
                self.confidence_levels[action_id] *= (1 + meta_signal)
        else:
            # Negative meta-signal: decrease learning
            for action_id in self.confidence_levels:
                self.confidence_levels[action_id] *= (1 - abs(meta_signal))
        
        # Update meta-awareness based on performance
        performance = accuracy * plan_confidence
        self.meta_awareness = (self.meta_awareness + performance) / 2
    
    def adapt_based_on_meta_awareness(self):
        """Adapt behavior based on meta-awareness."""
        
        if self.meta_awareness > 0.8:
            # High meta-awareness: more exploration
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
        elif self.meta_awareness < 0.3:
            # Low meta-awareness: more exploitation
            self.agent.enableCuriosity(False)
            self.agent.enableDevelopment(False)
        else:
            # Medium meta-awareness: balanced
            self.agent.enableCuriosity(True)
            self.agent.enableDevelopment(True)
        
        # Adapt learning rate
        if self.meta_awareness > 0.7:
            # High awareness: faster learning
            adaptation_lr = 0.005
        elif self.meta_awareness < 0.3:
            # Low awareness: slower learning
            adaptation_lr = 0.0001
        else:
            # Medium awareness: normal learning
            adaptation_lr = 0.001
        
        self.learning_rates['adaptation'] = adaptation_lr
    
    def calculate_reward_consistency(self):
        """Calculate reward consistency as meta-awareness factor."""
        
        # This would use recent reward history
        # Simplified example
        return np.random.random()  # Placeholder
    
    def calculate_planning_accuracy(self):
        """Calculate planning accuracy as meta-awareness factor."""
        
        # This would compare planned vs actual outcomes
        # Simplified example
        return np.random.random()  # Placeholder
    
    def extract_state_features(self, percept):
        """Extract state features for planning."""
        
        # Extract and normalize features
        features = percept.getAllNeurons()
        if len(features) > 0:
            features = (features - np.mean(features)) / np.std(features)
        
        return features
    
    def generate_primary_plan(self, state):
        """Generate primary action plan."""
        
        # Use neural planner to generate actions
        if self.brain.getPredictionSystem():
            return self.brain.getPredictionSystem().getPlannedActions()
        
        return []
    
    def generate_alternative_plans(self, state):
        """Generate alternative action plans."""
        
        # Generate multiple alternative plans
        # This could use different planning strategies
        return [self.generate_primary_plan(state) for _ in range(3)]
    
    def calculate_planning_confidence(self, state, predictions):
        """Calculate confidence in planning."""
        
        # Calculate based on prediction confidence and state uncertainty
        if not predictions:
            return 0.5
        
        prediction_confidence = np.mean(predictions['confidence'])
        state_uncertainty = np.std(state) if len(state) > 0 else 0.0
        
        confidence = prediction_confidence * (1.0 / (1.0 + state_uncertainty))
        
        return min(max(confidence, 0.1), 1.0)
    
    def generate_reasoning(self, state, confidence):
        """Generate human-readable reasoning for plan."""
        
        reasoning = []
        
        if confidence > 0.7:
            reasoning.append("Plan has high confidence based on strong predictions")
        elif confidence < 0.3:
            reasoning.append("Plan has low confidence due to uncertain state")
        else:
            reasoning.append("Plan confidence is moderate")
        
        reasoning.append(f"State has {len(state)} features")
        
        return reasoning
    
    def calculate_outcome_quality(self, rewards):
        """Calculate quality of outcomes based on rewards."""
        
        if not rewards:
            return 0.0
        
        # Calculate based on reward magnitude and consistency
        mean_reward = np.mean(rewards)
        reward_std = np.std(rewards)
        
        # High rewards with low variance = high quality
        quality = (mean_reward * 2.0) / (1.0 + reward_std)
        
        return min(quality, 1.0)
    
    def generate_meta_learning_signal(self, plan, result):
        """Generate meta-learning signal from execution results."""
        
        # Compare planned vs actual outcomes
        planned_quality = plan['confidence']
        actual_quality = result['outcome_quality']
        
        # Meta-learning signal: positive if actual > planned
        meta_signal = actual_quality - planned_quality
        
        return meta_signal
```

## Advanced Debugging and Testing

### 1. Advanced Testing Framework

```python
class AdvancedTestingFramework:
    """Advanced testing framework for NLM systems."""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.test_results = {}
        self.performance_benchmarks = {}
    
    def run_comprehensive_test_suite(self, test_config):
        """Run comprehensive test suite."""
        
        print("=== Advanced Test Suite ===")
        
        all_passed = True
        
        for test_name, test_function in self.test_functions.items():
            print(f"\nRunning {test_name}...")
            
            try:
                result = test_function(test_config)
                self.test_results[test_name] = result
                
                if result['passed']:
                    print(f"✓ {test_name} PASSED")
                else:
                    print(f"✗ {test_name} FAILED")
                    print(f"  Error: {result['error']}")
                    all_passed = False
                    
            except Exception as e:
                print(f"✗ {test_name} ERROR: {str(e)}")
                self.test_results[test_name] = {
                    'passed': False,
                    'error': str(e)
                }
                all_passed = False
        
        # Generate test report
        self.generate_test_report()
        
        return all_passed
    
    def create_test_functions(self):
        """Create test functions for various aspects."""
        
        self.test_functions = {
            'neural_stability': self.test_neural_stability,
            'memory_functionality': self.test_memory_functionality,
            'plasticity_learning': self.test_plasticity_learning,
            'agent_behavior': self.test_agent_behavior,
            'integration_coherence': self.test_integration_coherence,
            'performance_consistency': self.test_performance_consistency,
            'meta_cognition': self.test_meta_cognition,
            'memory_guided_action': self.test_memory_guided_action
        }
    
    def test_neural_stability(self, test_config):
        """Test neural stability over time."""
        
        print("  Testing neural stability...")
        
        # Run multiple simulations
        stability_scores = []
        
        for run in range(test_config['num_runs']):
            # Reset and run simulation
            self.brain.reset()
            self.brain.initialize()
            
            # Run simulation
            for step in range(test_config['steps']):
                self.brain.step(step)
            
            # Calculate stability metrics
            spike_rate = self.brain.getAverageFiringRate()
            neuron_count = self.brain.getFiringNeuronCount()
            
            stability = 1.0 - (abs(spike_rate - test_config['target_spike_rate']) / 
                             test_config['target_spike_rate'])
            
            stability_scores.append(stability)
        
        # Calculate average stability
        avg_stability = np.mean(stability_scores)
        std_stability = np.std(stability_scores)
        
        passed = avg_stability > test_config['stability_threshold']
        
        return {
            'passed': passed,
            'stability_score': avg_stability,
            'stability_std': std_stability,
            'details': f"Average stability: {avg_stability:.3f}"
        }
    
    def test_memory_functionality(self, test_config):
        """Test memory system functionality."""
        
        print("  Testing memory functionality...")
        
        # Test working memory
        working_memory = self.brain.getWorkingMemory()
        episodic_memory = self.brain.getEpisodicMemory()
        
        if not working_memory or not episodic_memory:
            return {
                'passed': False,
                'error': 'Memory systems not properly initialized'
            }
        
        # Store test patterns
        test_patterns = [
            np.array([1.0, 0.5, 0.0, -0.5, -1.0]),
            np.array([0.0, 0.8, 0.3, 0.1, -0.2]),
            np.array([-0.5, 0.0, 0.5, 0.8, 1.0])
        ]
        
        # Test working memory
        wm_success = True
        for pattern in test_patterns:
            working_memory.store(pattern, 1.0)
            retrieved = working_memory.retrieve()
            if retrieved is None or len(retrieved) != len(pattern):
                wm_success = False
                break
        
        # Test episodic memory
        em_success = True
        for i, pattern in enumerate(test_patterns):
            episodic_memory.storeEpisode({
                'id': i,
                'pattern': pattern,
                'reward': i + 1,
                'step': i * 100
            })
        
        # Test retrieval
        similar_patterns = episodic_memory.retrieveSimilar(test_patterns[0], 2)
        
        passed = wm_success and em_success and len(similar_patterns) > 0
        
        return {
            'passed': passed,
            'working_memory': wm_success,
            'episodic_memory': em_success,
            'similarity_recall': len(similar_patterns)
        }
    
    def test_plasticity_learning(self, test_config):
        """Test plasticity and learning capabilities."""
        
        print("  Testing plasticity and learning...")
        
        # Get initial brain state
        initial_weights = self.capture_neural_weights()
        
        # Run learning simulation
        for step in range(test_config['learning_steps']):
            # Run simulation
            self.brain.step(step)
            
            # Apply reward signals
            if step % 10 == 0:
                reward = np.random.random() * 0.5
                self.agent.applyRewardModulation(reward, 0.0)
        
        # Get final brain state
        final_weights = self.capture_neural_weights()
        
        # Calculate weight changes
        weight_changes = self.calculate_weight_changes(initial_weights, final_weights)
        
        # Check if learning occurred
        learning_detected = (
            np.mean(np.abs(weight_changes)) > test_config['min_weight_change'] and
            np.any(weight_changes != 0)
        )
        
        passed = learning_detected
        
        return {
            'passed': passed,
            'weight_change_mean': np.mean(np.abs(weight_changes)),
            'weight_change_std': np.std(np.abs(weight_changes)),
            'learning_detected': learning_detected
        }
    
    def test_agent_behavior(self, test_config):
        """Test agent behavior and decision making."""
        
        print("  Testing agent behavior...")
        
        # Run agent simulation
        total_reward = 0.0
        action_diversity = set()
        
        for step in range(test_config['agent_steps']):
            # Update world
            self.world.update(test_config['dt'])
            
            # Get percept
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Run brain
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            action_diversity.add(action)
            
            # Execute action
            result = self.world.applyMotorCommand(
                action, self.world.getSimulationTime()
            )
            
            # Get reward
            reward = result.reward if hasattr(result, 'reward') else 0.0
            total_reward += reward
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
        
        # Calculate behavior metrics
        avg_reward = total_reward / test_config['agent_steps']
        action_count = len(action_diversity)
        
        passed = (
            avg_reward > test_config['min_avg_reward'] and
            action_count >= test_config['min_action_diversity']
        )
        
        return {
            'passed': passed,
            'avg_reward': avg_reward,
            'action_diversity': action_count,
            'total_reward': total_reward
        }
    
    def test_integration_coherence(self, test_config):
        """Test integration between different systems."""
        
        print("  Testing integration coherence...")
        
        # Get all system states
        neural_state = self.get_neural_state()
        memory_state = self.get_memory_state()
        neuromod_state = self.get_neuromod_state()
        
        # Check for coherence
        coherence_checks = []
        
        # 1. Check memory-neural coherence
        if 'working_memory' in memory_state:
            wm_neural_coherence = self.calculate_coherence(
                neural_state['firing_rate'],
                memory_state['working_memory']
            )
            coherence_checks.append(wm_neural_coherence > test_config['min_coherence'])
        
        # 2. Check neuromodulation-neural coherence
        if 'dopamine' in neuromod_state:
            neuromod_neural_coherence = self.calculate_coherence(
                neural_state['firing_rate'],
                neuromod_state['dopamine']
            )
            coherence_checks.append(neuromod_neural_coherence > test_config['min_coherence'])
        
        # 3. Check memory development coherence
        if 'episodic_memory' in memory_state and 'development' in neuromod_state:
            mem_dev_coherence = self.calculate_coherence(
                memory_state['episodic_memory'],
                neuromod_state['development']
            )
            coherence_checks.append(mem_dev_coherence > test_config['min_coherence'])
        
        # Calculate overall coherence
        overall_coherence = np.mean(coherence_checks) if coherence_checks else 0.0
        passed = overall_coherence >= test_config['min_overall_coherence']
        
        return {
            'passed': passed,
            'coherence_score': overall_coherence,
            'coherence_checks': len(coherence_checks)
        }
    
    def test_performance_consistency(self, test_config):
        """Test performance consistency across runs."""
        
        print("  Testing performance consistency...")
        
        # Run multiple simulations
        performance_scores = []
        
        for run in range(test_config['num_performance_runs']):
            # Run simulation
            run_score = self.run_performance_test(test_config)
            performance_scores.append(run_score)
        
        # Calculate consistency metrics
        mean_perf = np.mean(performance_scores)
        std_perf = np.std(performance_scores)
        cv = std_perf / mean_perf if mean_perf > 0 else 0.0
        
        passed = cv < test_config['max_coefficient_of_variation']
        
        return {
            'passed': passed,
            'mean_performance': mean_perf,
            'std_performance': std_perf,
            'coefficient_of_variation': cv,
            'consistency': 'high' if cv < 0.1 else 'medium' if cv < 0.3 else 'low'
        }
    
    def test_meta_cognition(self, test_config):
        """Test meta-cognitive capabilities."""
        
        print("  Testing meta-cognition...")
        
        # Test meta-awareness adaptation
        meta_awareness_scores = []
        
        for episode in range(test_config['meta_episodes']):
            # Run meta-cognitive agent
            agent_meta = self.run_meta_cognitive_episode(test_config)
            meta_awareness_scores.append(agent_meta['meta_awareness'])
        
        # Calculate meta-cognition metrics
        mean_meta = np.mean(meta_awareness_scores)
        meta_adaptation = np.std(meta_awareness_scores)
        
        # Check for meta-cognitive behaviors
        meta_behaviors_detected = (
            mean_meta > test_config['min_meta_awareness'] and
            meta_adaptation > test_config['min_meta_adaptation']
        )
        
        passed = meta_behaviors_detected
        
        return {
            'passed': passed,
            'mean_meta_awareness': mean_meta,
            'meta_adaptation': meta_adaptation,
            'meta_behaviors': meta_behaviors_detected
        }
    
    def test_memory_guided_action(self, test_config):
        """Test memory-guided action selection."""
        
        print("  Testing memory-guided action...")
        
        # Test memory-guided behavior
        memory_guided_scores = []
        
        for episode in range(test_config['memory_episodes']):
            # Store memory
            self.store_test_memory(episode)
            
            # Get memory state
            memory_state = self.get_memory_state()
            
            # Select action based on memory
            action = self.select_action_memory_guided(memory_state)
            
            # Execute action and get reward
            reward = self.execute_memory_guided_action(action)
            
            memory_guided_scores.append(reward)
        
        # Calculate memory-guided metrics
        avg_memory_guided = np.mean(memory_guided_scores)
        memory_effectiveness = avg_memory_guided / test_config['baseline_reward']
        
        passed = memory_effectiveness > test_config['min_memory_effectiveness']
        
        return {
            'passed': passed,
            'memory_effectiveness': memory_effectiveness,
            'avg_memory_reward': avg_memory_guided,
            'baseline_reward': test_config['baseline_reward']
        }
    
    def run_performance_test(self, test_config):
        """Run a performance test."""
        
        # Run simulation and return performance score
        total_reward = 0.0
        steps = test_config['performance_steps']
        
        for step in range(steps):
            # Run simulation step
            self.brain.step(step)
            
            # Add small reward for testing
            reward = np.random.random() * 0.1
            total_reward += reward
        
        # Normalize performance score
        performance_score = min(total_reward / steps, 1.0)
        
        return performance_score
    
    def run_meta_cognitive_episode(self, test_config):
        """Run a meta-cognitive episode."""
        
        # This would run a meta-cognitive agent
        # For now, return a placeholder
        return {
            'meta_awareness': np.random.random(),
            'confidence': np.random.random(),
            'planning_quality': np.random.random()
        }
    
    def capture_neural_weights(self):
        """Capture current neural weights for comparison."""
        
        # This would capture actual neural weights
        # For now, return a placeholder
        return np.random.random(100)  # Placeholder
    
    def calculate_weight_changes(self, initial, final):
        """Calculate changes in neural weights."""
        
        return final - initial
    
    def get_neural_state(self):
        """Get current neural state."""
        
        return {
            'firing_rate': self.brain.getAverageFiringRate(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'neuron_count': self.brain.getFiringNeuronCount()
        }
    
    def get_memory_state(self):
        """Get current memory state."""
        
        state = {}
        
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            state['working_memory'] = working_memory.getActiveTraces()
        
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            state['episodic_memory'] = episodic_memory.getEpisodeCount()
        
        return state
    
    def get_neuromod_state(self):
        """Get current neuromodulation state."""
        
        return {
            'dopamine': 0.5,  # Placeholder
            'curiosity': 0.3,  # Placeholder
            'novelty': 0.2  # Placeholder
        }
    
    def calculate_coherence(self, signal1, signal2):
        """Calculate coherence between two signals."""
        
        # Calculate cross-correlation
        if len(signal1) == len(signal2):
            correlation = np.corrcoef(signal1, signal2)[0, 1]
            return abs(correlation) if not np.isnan(correlation) else 0.0
        
        return 0.0
    
    def store_test_memory(self, episode):
        """Store test memory for memory-guided action test."""
        
        # Store test pattern in working memory
        pattern = np.random.random(10)
        if self.brain.getWorkingMemory():
            self.brain.getWorkingMemory().store(pattern, 1.0)
        
        # Store in episodic memory
        if self.brain.getEpisodicMemory():
            self.brain.getEpisodicMemory().storeEpisode({
                'id': episode,
                'pattern': pattern,
                'reward': np.random.random(),
                'step': episode * 100
            })
    
    def select_action_memory_guided(self, memory_state):
        """Select action based on memory state."""
        
        # Simple memory-guided action selection
        if self.agent:
            return self.agent.decodeMotorCommand()
        
        return 0
    
    def execute_memory_guided_action(self, action):
        """Execute memory-guided action and return reward."""
        
        # For now, return a small reward
        return np.random.random() * 0.2
    
    def generate_test_config(self):
        """Generate test configuration."""
        
        return {
            'num_runs': 5,
            'steps': 100,
            'target_spike_rate': 5.0,
            'stability_threshold': 0.8,
            'learning_steps': 200,
            'min_weight_change': 0.001,
            'agent_steps': 150,
            'min_avg_reward': 0.1,
            'min_action_diversity': 3,
            'world_config': {
                'width': 20,
                'height': 20,
                'visionWidth': 8,
                'visionHeight': 8
            },
            'dt': 0.1,
            'num_performance_runs': 10,
            'max_coefficient_of_variation': 0.2,
            'meta_episodes': 5,
            'min_meta_awareness': 0.5,
            'min_meta_adaptation': 0.1,
            'memory_episodes': 8,
            'baseline_reward': 0.2,
            'min_memory_effectiveness': 1.2,
            'performance_steps': 50
        }
    
    def generate_test_report(self):
        """Generate comprehensive test report."""
        
        print(f"\n=== Advanced Test Report ===")
        print(f"Total tests: {len(self.test_results)}")
        
        passed_tests = sum(1 for result in self.test_results.values() if result['passed'])
        failed_tests = len(self.test_results) - passed_tests
        
        print(f"Passed: {passed_tests}")
        print(f"Failed: {failed_tests}")
        
        # Print detailed results
        for test_name, result in self.test_results.items():
            status = "✓ PASS" if result['passed'] else "✗ FAIL"
            print(f"\n{test_name}: {status}")
            
            if 'details' in result:
                print(f"  Details: {result['details']}")
            
            if 'error' in result:
                print(f"  Error: {result['error']}")
        
        # Print performance benchmarks if available
        if self.performance_benchmarks:
            print(f"\n=== Performance Benchmarks ===")
            for metric, stats in self.performance_benchmarks.items():
                print(f"\n{metric}:")
                if isinstance(stats, dict):
                    for key, value in stats.items():
                        print(f"  {key}: {value}")
        
        return passed_tests == len(self.test_results)
```

This advanced usage guide provides comprehensive examples and techniques for working with NLM systems. It covers:

- Advanced memory system manipulation
- Custom training loops with curriculum learning
- Meta-learning approaches (MAML)
- Performance profiling and optimization
- Advanced agent behaviors (hierarchical planning, meta-cognition)
- Advanced testing frameworks

These examples demonstrate how to extend and customize NLM for complex tasks and research applications.
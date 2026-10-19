# Common Patterns

This document provides recurring code patterns and solutions for common NLM programming tasks. It covers pattern-based solutions, optimization techniques, and best practices for efficient NLM code.

## Overview

Common patterns in NLM programming provide reusable solutions to frequently encountered problems. These patterns help developers:

- **Write cleaner code**: Use proven patterns that are easier to understand
- **Avoid common mistakes**: Learn from successful implementations
- **Optimize performance**: Use efficient patterns for common operations
- **Maintain consistency**: Follow established patterns for consistency

## Pattern Categories

### 1. Memory Management Patterns

#### Memory Pool Pattern

**Problem:** Efficiently manage memory allocation for neurons and synapses.

**Solution:** Use memory pools to reduce allocation overhead.

```python
class MemoryPoolBrain:
    def __init__(self, config):
        self.config = config
        self.neuron_pool = self.create_neuron_pool()
        self.synapse_pool = self.create_synapse_pool()
        
    def create_neuron_pool(self):
        """Create memory pool for neurons."""
        neuron_count = self.config.get("neuron_count")
        return [0.0] * neuron_count
        
    def create_synapse_pool(self):
        """Create memory pool for synapses."""
        neuron_count = self.config.get("neuron_count")
        return [(i, (i + 1) % neuron_count) for i in range(neuron_count)]
        
    def allocate_neuron(self):
        """Allocate neuron from pool."""
        if not self.neuron_pool:
            raise RuntimeError("No available neurons in pool")
        return self.neuron_pool.pop()
        
    def free_neuron(self, neuron):
        """Free neuron back to pool."""
        self.neuron_pool.append(neuron)
```

#### Reference Counting Pattern

**Problem:** Manage object lifetimes to avoid memory leaks.

**Solution:** Use reference counting for automatic memory management.

```python
class ReferenceCountedBrain:
    def __init__(self, config):
        self.config = config
        self.ref_count = 0
        self.data = self.initialize_data()
        
    def reference(self):
        """Increment reference count."""
        self.ref_count += 1
        
    def dereference(self):
        """Decrement reference count and cleanup if needed."""
        self.ref_count -= 1
        if self.ref_count <= 0:
            self.cleanup()
            
    def cleanup(self):
        """Cleanup resources."""
        self.data = None
        
    def __enter__(self):
        """Context manager entry."""
        self.reference()
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        self.dereference()
```

### 2. Simulation Control Patterns

#### Event-Driven Simulation Pattern

**Problem:** Manage complex simulation events and timing.

**Solution:** Use event-driven architecture for better control.

```python
class EventDrivenSimulation:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.event_queue = []
        self.time = 0.0
        
    def schedule_event(self, delay, event_type, data):
        """Schedule event for future execution."""
        self.event_queue.append({
            'time': self.time + delay,
            'type': event_type,
            'data': data
        })
        
    def run_simulation(self, duration):
        """Run simulation with event scheduling."""
        while self.time < duration:
            # Execute due events
            self.execute_due_events()
            
            # Update simulation
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.brain.processSensoryInput(percept)
            self.brain.step(int(self.time))
            action = self.brain.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.time)
            
            self.time += 0.1
            
    def execute_due_events(self):
        """Execute events due at current time."""
        due_events = [e for e in self.event_queue if e['time'] <= self.time]
        self.event_queue = [e for e in self.event_queue if e['time'] > self.time]
        
        for event in due_events:
            self.execute_event(event)
            
    def execute_event(self, event):
        """Execute specific event."""
        if event['type'] == 'reward':
            self.handle_reward_event(event['data'])
        elif event['type'] == 'punishment':
            self.handle_punishment_event(event['data'])
```

#### State Management Pattern

**Problem:** Manage simulation state and recovery.

**Solution:** Use state management for simulation control.

```python
class StatefulSimulation:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.state_history = []
        self.current_state = None
        
    def save_state(self):
        """Save current simulation state."""
        state = {
            'brain_state': self.brain.get_state(),
            'world_state': self.world.get_state(),
            'time': self.world.getSimulationTime()
        }
        self.state_history.append(state)
        self.current_state = state
        
    def load_state(self, state_index=-1):
        """Load simulation state."""
        if state_index < 0:
            state_index = len(self.state_history) + state_index
        if state_index < 0 or state_index >= len(self.state_history):
            raise ValueError(f"Invalid state index: {state_index}")
            
        self.current_state = self.state_history[state_index]
        self.brain.set_state(self.current_state['brain_state'])
        self.world.set_state(self.current_state['world_state'])
        
    def snapshot(self):
        """Take snapshot of current state."""
        self.save_state()
        return len(self.state_history) - 1
        
    def restore(self, snapshot_index):
        """Restore to snapshot."""
        self.load_state(snapshot_index)
```

### 3. Memory System Patterns

#### Working Memory Pattern

**Problem:** Manage working memory efficiently.

**Solution:** Use working memory patterns for transient information storage.

```python
class WorkingMemoryPattern:
    def __init__(self, capacity=20):
        self.capacity = capacity
        self.memory_items = {}
        
    def store(self, key, value, neuron_id):
        """Store value with neuron ID."""
        if len(self.memory_items) >= self.capacity:
            self.evict_oldest()
        self.memory_items[key] = {'value': value, 'neuron_id': neuron_id}
        
    def retrieve(self, key):
        """Retrieve value by key."""
        return self.memory_items.get(key, {}).get('value')
        
    def contains(self, key):
        """Check if key exists."""
        return key in self.memory_items
        
    def evict_oldest(self):
        """Evict oldest memory item."""
        if self.memory_items:
            oldest_key = min(self.memory_items.keys())
            del self.memory_items[oldest_key]
            
    def decay(self, decay_rate):
        """Apply decay to all memory items."""
        for item in self.memory_items.values():
            item['value'] *= (1.0 - decay_rate)
            
    def clear(self):
        """Clear all memory items."""
        self.memory_items.clear()
```

#### Episodic Memory Pattern

**Problem:** Manage episodic memory for experience storage.

**Solution:** Use episodic memory patterns for storing experiences.

```python
class EpisodicMemoryPattern:
    def __init__(self, max_episodes=1000):
        self.max_episodes = max_episodes
        self.episodes = []
        
    def store_episode(self, timestamp, neurons, values, metadata):
        """Store episodic memory."""
        if len(self.episodes) >= self.max_episodes:
            self.consolidate()
            
        episode = {
            'timestamp': timestamp,
            'neurons': neurons,
            'values': values,
            'metadata': metadata
        }
        self.episodes.append(episode)
        
    def retrieve_episode(self, timestamp):
        """Retrieve episode by timestamp."""
        for episode in self.episodes:
            if episode['timestamp'] == timestamp:
                return episode
        return None
        
    def get_recent_episodes(self, count):
        """Get recent episodes."""
        return self.episodes[-count:] if count <= len(self.episodes) else self.episodes
        
    def replay_episodes(self, episode_indices):
        """Replay specific episodes."""
        for index in episode_indices:
            if 0 <= index < len(self.episodes):
                yield self.episodes[index]
                
    def consolidate(self):
        """Consolidate memory by removing irrelevant episodes."""
        # Implementation for consolidation logic
        pass
```

#### Associative Memory Pattern

**Problem:** Manage associative relationships between neural representations.

**Solution:** Use associative memory patterns for pattern association.

```python
class AssociativeMemoryPattern:
    def __init__(self):
        self.associations = {}
        self.pattern_index = {}
        
    def add_association(self, pattern_a, pattern_b, strength):
        """Add association between two patterns."""
        # Store associations
        self.associations.setdefault(pattern_a, {}).setdefault(pattern_b, strength)
        self.associations.setdefault(pattern_b, {}).setdefault(pattern_a, strength)
        
        # Update pattern index
        self.update_pattern_index(pattern_a, pattern_b)
        self.update_pattern_index(pattern_b, pattern_a)
        
    def update_pattern_index(self, pattern1, pattern2):
        """Update pattern index for efficient lookup."""
        self.pattern_index.setdefault(pattern1, []).append(pattern2)
        self.pattern_index.setdefault(pattern2, []).append(pattern1)
        
    def retrieve(self, pattern, similarity_threshold):
        """Recall patterns associated with query pattern."""
        if pattern not in self.associations:
            return []
            
        # Filter by similarity threshold
        associated = self.associations[pattern]
        return [p for p, strength in associated.items() 
                if strength >= similarity_threshold]
        
    def get_associations(self, pattern):
        """Get all associations for a pattern."""
        return self.associations.get(pattern, {})
```

### 4. Neuromodulation Patterns

#### Reward-Based Learning Pattern

**Problem:** Implement reward-based learning systems.

**Solution:** Use reward-based learning patterns for reinforcement learning.

```python
class RewardBasedLearningPattern:
    def __init__(self, dopamine_system):
        self.dopamine = dopamine_system
        self.reward_history = []
        self.expected_rewards = {}
        
    def learn_from_reward(self, action, reward, predicted_reward=None):
        """Learn from reward signal."""
        # Update expected reward
        if predicted_reward is None:
            predicted_reward = self.expected_rewards.get(action, 0.0)
            
        # Calculate reward prediction error
        error = reward - predicted_reward
        
        # Update expected reward
        self.expected_rewards[action] = reward
        
        # Signal to dopamine system
        self.dopamine.signalReward(reward)
        self.dopamine.signalRewardPredictionError(error)
        
        # Record learning
        self.reward_history.append({
            'action': action,
            'reward': reward,
            'predicted_reward': predicted_reward,
            'error': error
        })
        
    def get_action_quality(self, action):
        """Get quality of action based on reward."""
        if action not in self.reward_history:
            return 0.0
            
        # Calculate average reward for action
        action_rewards = [r['reward'] for r in self.reward_history if r['action'] == action]
        return sum(action_rewards) / len(action_rewards)
```

#### Curiosity-Driven Learning Pattern

**Problem:** Implement curiosity-driven exploration systems.

**Solution:** Use curiosity-driven learning patterns for intrinsic motivation.

```python
class CuriosityDrivenLearningPattern:
    def __init__(self, curiosity_system):
        self.curiosity = curiosity_system
        self.exploration_history = []
        self.intrinsic_motivation = {}
        
    def explore_environment(self, world, agent, steps):
        """Explore environment using curiosity."""
        for step in range(steps):
            # Update curiosity
            novelty = self.get_novelty(world, agent)
            prediction_error = self.get_prediction_error(world, agent)
            self.curiosity.update(novelty, prediction_error, 0.1)
            
            # Decide exploration action
            curiosity_level = self.curiosity.getCuriosityLevel()
            if curiosity_level > 0.5:
                action = self.explore_action(world, agent)
            else:
                action = self.exploit_action(world, agent)
                
            # Execute action
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain = agent.getBrain()
            brain.step(step)
            decoded_action = agent.decodeMotorCommand()
            world.applyMotorCommand(decoded_action, world.getSimulationTime())
            
            # Record exploration
            self.exploration_history.append({
                'step': step,
                'curiosity_level': curiosity_level,
                'action': action
            })
            
    def get_novelty(self, world, agent):
        """Get novelty measure for environment."""
        # Implementation of novelty detection
        percept = world.getSensoryPercept()
        vision = percept.getVision()
        return self.calculate_vision_novelty(vision)
        
    def get_prediction_error(self, world, agent):
        """Get prediction error measure."""
        # Implementation of prediction error calculation
        return 0.0  # Placeholder
        
    def explore_action(self, world, agent):
        """Take exploratory action."""
        # Take novel action to increase curiosity
        return self.get_random_action(world, agent)
        
    def exploit_action(self, world, agent):
        """Take exploitative action."""
        # Take known good action
        return agent.decodeMotorCommand()
```

#### Novelty Detection Pattern

**Problem:** Detect and respond to novel events.

**Solution:** Use novelty detection patterns for change detection.

```python
class NoveltyDetectionPattern:
    def __init__(self, novelty_system):
        self.novelty = novelty_system
        self.pattern_history = []
        self.novelty_threshold = 0.5
        
    def detect_novelty(self, current_pattern, previous_pattern):
        """Detect novelty from pattern comparison."""
        if previous_pattern is None:
            return 1.0  # First observation is always novel
            
        # Calculate pattern difference
        difference = self.calculate_pattern_difference(current_pattern, previous_pattern)
        
        # Store patterns for future comparison
        self.pattern_history.append(previous_pattern)
        
        # Detect novelty
        novelty_score = min(1.0, difference / self.novelty_threshold)
        self.novelty.setLevel(novelty_score)
        
        return novelty_score
        
    def calculate_pattern_difference(self, pattern1, pattern2):
        """Calculate difference between two patterns."""
        if len(pattern1) != len(pattern2):
            return 1.0  # Different lengths are novel
            
        # Calculate mean squared difference
        differences = [(p1 - p2) ** 2 for p1, p2 in zip(pattern1, pattern2)]
        return (sum(differences) / len(differences)) ** 0.5
```

### 5. Prediction and Planning Patterns

#### Prediction System Pattern

**Problem:** Implement predictive systems for future state estimation.

**Solution:** Use prediction system patterns for state prediction.

```python
class PredictionSystemPattern:
    def __init__(self):
        self.prediction_history = []
        self.prediction_model = self.initialize_prediction_model()
        
    def predict_next_state(self, current_state):
        """Predict next state based on current state."""
        # Get prediction from model
        prediction = self.prediction_model.predict(current_state)
        
        # Store prediction for later comparison
        self.prediction_history.append({
            'input': current_state,
            'prediction': prediction,
            'time': len(self.prediction_history)
        })
        
        return prediction
        
    def update_prediction(self, prediction, actual):
        """Update prediction model with actual observation."""
        # Update prediction model
        self.prediction_model.update(prediction, actual)
        
    def get_prediction_confidence(self):
        """Get confidence in predictions."""
        if not self.prediction_history:
            return 0.0
            
        # Calculate average confidence from prediction history
        confidences = [self.get_prediction_confidence_from_prediction(p) 
                      for p in self.prediction_history]
        return sum(confidences) / len(confidences)
        
    def get_prediction_error(self):
        """Get prediction error."""
        if len(self.prediction_history) < 2:
            return 0.0
            
        # Calculate error between recent predictions
        recent_predictions = self.prediction_history[-10:]
        errors = [abs(p['prediction'] - p['input']) 
                 for p in recent_predictions]
        return sum(errors) / len(errors) if errors else 0.0
```

#### Planning System Pattern

**Problem:** Implement action planning systems for goal achievement.

**Solution:** Use planning system patterns for action sequence planning.

```python
class PlanningSystemPattern:
    def __init__(self, brain):
        self.brain = brain
        self.planning_cache = {}
        self.planning_goals = []
        
    def plan_action(self, current_state, goal):
        """Plan action to achieve goal."""
        # Create planning problem
        planning_problem = self.create_planning_problem(current_state, goal)
        
        # Plan action using cached results if available
        cache_key = self.get_planning_cache_key(current_state, goal)
        if cache_key in self.planning_cache:
            return self.planning_cache[cache_key]
            
        # Perform planning
        action = self.perform_planning(planning_problem)
        
        # Cache result
        self.planning_cache[cache_key] = action
        
        return action
        
    def evaluate_action(self, action, state):
        """Evaluate action for given state."""
        # Evaluate action using planner's evaluation function
        return self.evaluate_planning_action(action, state)
        
    def plan_action_sequence(self, current_state, goals, horizon):
        """Plan sequence of actions to achieve multiple goals."""
        # Plan sequence using hierarchical planning
        action_sequence = self.plan_hierarchical_sequence(current_state, goals, horizon)
        
        # Evaluate sequence quality
        sequence_quality = self.evaluate_action_sequence(action_sequence, current_state)
        
        return action_sequence, sequence_quality
```

### 6. Development Patterns

#### Critical Period Pattern

**Problem:** Implement critical period learning and development.

**Solution:** Use critical period patterns for enhanced learning phases.

```python
class CriticalPeriodPattern:
    def __init__(self, development_system):
        self.development_system = development_system
        self.critical_period_effects = {}
        self.learning_accelerators = []
        
    def enter_critical_period(self):
        """Enter critical period for enhanced learning."""
        self.development_system.setCurrentStage(pynlm.DevelopmentalStage.CriticalPeriod)
        print("Critical period started - enhanced learning enabled")
        
    def exit_critical_period(self):
        """Exit critical period."""
        self.development_system.setCurrentStage(pynlm.DevelopmentalStage.Maturation)
        print("Critical period ended - normal learning resumed")
        
    def accelerate_learning(self, learning_rate):
        """Accelerate learning during critical period."""
        self.learning_accelerators.append(learning_rate)
        # Implementation of learning acceleration
        
    def get_plasticity_modifier(self):
        """Get current plasticity modifier."""
        return self.development_system.getPlasticityModifier()
```

#### Developmental Adaptation Pattern

**Problem:** Implement adaptive development across lifespan.

**Solution:** Use developmental adaptation patterns for lifelong learning.

```python
class DevelopmentalAdaptationPattern:
    def __init__(self, development_system):
        self.development_system = development_system
        self.adaptation_strategies = []
        self.developmental_stages = {}
        
    def setup_development_stages(self):
        """Setup developmental stages."""
        stages = [
            pynlm.DevelopmentalStage.Initial,
            pynlm.DevelopmentalStage.CriticalPeriod,
            pynlm.DevelopmentalStage.Maturation,
            pynlm.DevelopmentalStage.Adult,
            pynlm.DevelopmentalStage.Aging
        ]
        
        for i, stage in enumerate(stages):
            self.developmental_stages[stage] = i
            
    def advance_development(self):
        """Advance to next developmental stage."""
        current_stage = self.development_system.getCurrentStage()
        stage_index = self.developmental_stages[current_stage]
        
        if stage_index < len(pynlm.DevelopmentalStage) - 1:
            next_stage = list(pynlm.DevelopmentalStage)[stage_index + 1]
            self.development_system.setCurrentStage(next_stage)
            print(f"Advanced to {next_stage} stage")
            
        return next_stage if stage_index < len(pynlm.DevelopmentalStage) - 1 else current_stage
```

### 7. Integration Patterns

#### Multi-System Integration Pattern

**Problem:** Integrate multiple neural systems for intelligent behavior.

**Solution:** Use integration patterns for system coordination.

```python
class MultiSystemIntegrationPattern:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.system_coordinators = {}
        
    def initialize_systems(self):
        """Initialize all neural systems."""
        # Initialize brain systems
        self.system_coordinators['working_memory'] = self.brain.getWorkingMemory()
        self.system_coordinators['episodic_memory'] = self.brain.getEpisodicMemory()
        self.system_coordinators['associative_memory'] = self.brain.getAssociativeMemory()
        self.system_coordinators['neuromodulation'] = self.agent.getNeuromodulationLevel()
        self.system_coordinators['prediction'] = self.brain.getPredictionSystem()
        self.system_coordinators['planning'] = self.create_planning_system()
        self.system_coordinators['development'] = self.brain.getDevelopmentSystem()
        
    def coordinate_systems(self, current_state):
        """Coordinate all systems for current state."""
        # Get system outputs
        working_memory_output = self.get_working_memory_output()
        episodic_memory_output = self.get_episodic_memory_output()
        neuromodulation_output = self.get_neuromodulation_output()
        prediction_output = self.get_prediction_output(current_state)
        planning_output = self.get_planning_output(current_state)
        development_output = self.get_development_output()
        
        # Integrate system outputs
        integrated_behavior = self.integrate_system_outputs([
            working_memory_output,
            episodic_memory_output,
            neuromodulation_output,
            prediction_output,
            planning_output,
            development_output
        ])
        
        return integrated_behavior
        
    def integrate_system_outputs(self, system_outputs):
        """Integrate outputs from all systems."""
        # Combine system outputs
        integrated = {
            'working_memory': system_outputs[0],
            'episodic_memory': system_outputs[1],
            'neuromodulation': system_outputs[2],
            'prediction': system_outputs[3],
            'planning': system_outputs[4],
            'development': system_outputs[5]
        }
        
        # Apply integration rules
        integrated_behavior = self.apply_integration_rules(integrated)
        
        return integrated_behavior
```

## Performance Optimization Patterns

### 1. Caching Patterns

#### Read-Through Cache Pattern

**Problem:** Reduce redundant computations through caching.

**Solution:** Use read-through caching for expensive computations.

```python
class ReadThroughCache:
    def __init__(self, cache_size=1000):
        self.cache = {}
        self.cache_size = cache_size
        self.access_order = []
        
    def get(self, key, compute_func):
        """Get value from cache or compute if not cached."""
        if key in self.cache:
            # Move to end of access order
            self.access_order.remove(key)
            self.access_order.append(key)
            return self.cache[key]
            
        # Compute value
        value = compute_func()
        
        # Store in cache
        self.cache[key] = value
        self.access_order.append(key)
        
        # Evict oldest if cache is full
        if len(self.cache) > self.cache_size:
            oldest_key = self.access_order.pop(0)
            del self.cache[oldest_key]
            
        return value
```

#### Write-Behind Cache Pattern

**Problem:** Reduce write overhead by delaying writes.

**Solution:** Use write-behind caching for batch updates.

```python
class WriteBehindCache:
    def __init__(self, flush_interval=1.0):
        self.cache = {}
        self.write_buffer = {}
        self.flush_interval = flush_interval
        self.last_flush = 0.0
        
    def put(self, key, value):
        """Put value in cache with write-behind."""
        self.write_buffer[key] = value
        
    def get(self, key, compute_func=None):
        """Get value from cache or compute."""
        if key in self.cache:
            return self.cache[key]
            
        if compute_func:
            value = compute_func()
            self.cache[key] = value
            return value
            
        raise KeyError(f"Key not found: {key}")
        
    def flush(self):
        """Flush write buffer to cache."""
        self.cache.update(self.write_buffer)
        self.write_buffer.clear()
        self.last_flush = time.time()
```

### 2. Stream Processing Patterns

#### Event Stream Pattern

**Problem:** Process continuous streams of events efficiently.

**Solution:** Use event stream processing for real-time systems.

```python
class EventStreamProcessor:
    def __init__(self):
        self.event_handlers = {}
        self.event_streams = {}
        
    def subscribe(self, event_type, handler):
        """Subscribe to event type."""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append(handler)
        
    def publish(self, event_type, data):
        """Publish event to subscribers."""
        if event_type in self.event_handlers:
            for handler in self.event_handlers[event_type]:
                handler(data)
                
    def create_stream(self, name, event_source):
        """Create event stream from event source."""
        self.event_streams[name] = event_source
        return self.event_streams[name]
```

#### Batch Processing Pattern

**Problem:** Process large datasets in batches for efficiency.

**Solution:** Use batch processing for large-scale computations.

```python
class BatchProcessor:
    def __init__(self, batch_size=1000):
        self.batch_size = batch_size
        self.processors = []
        
    def add_processor(self, processor):
        """Add processing function."""
        self.processors.append(processor)
        
    def process_batch(self, data):
        """Process data in batches."""
        batches = [data[i:i + self.batch_size] 
                  for i in range(0, len(data), self.batch_size)]
        
        results = []
        for batch in batches:
            batch_result = self.process_batch_data(batch)
            results.extend(batch_result)
            
        return results
        
    def process_batch_data(self, batch_data):
        """Process single batch of data."""
        results = []
        for processor in self.processors:
            results.extend(processor(batch_data))
        return results
```

### 3. Concurrency Patterns

#### Thread Pool Pattern

**Problem:** Manage concurrent tasks efficiently.

**Solution:** Use thread pool for managing concurrent tasks.

```python
class ThreadPoolManager:
    def __init__(self, max_workers=10):
        self.max_workers = max_workers
        self.pool = concurrent.futures.ThreadPoolExecutor(max_workers=max_workers)
        
    def submit_task(self, task, *args, **kwargs):
        """Submit task to thread pool."""
        return self.pool.submit(task, *args, **kwargs)
        
    def map_tasks(self, tasks, *args, **kwargs):
        """Map tasks to thread pool."""
        return self.pool.map(tasks, *args, **kwargs)
        
    def shutdown(self):
        """Shutdown thread pool."""
        self.pool.shutdown(wait=True)
```

#### Producer-Consumer Pattern

**Problem:** Coordinate producer and consumer tasks.

**Solution:** Use producer-consumer pattern for task coordination.

```python
class ProducerConsumerManager:
    def __init__(self, queue_size=100):
        self.queue = queue.Queue(maxsize=queue_size)
        self.producers = []
        self.consumers = []
        
    def add_producer(self, producer):
        """Add producer."""
        self.producers.append(producer)
        
    def add_consumer(self, consumer):
        """ adds consumer."""
        self.consumers.append(consumer)
        
    def start(self):
        """Start producers and consumers."""
        # Start consumers
        for consumer in self.consumers:
            consumer_thread = threading.Thread(target=self.run_consumer, args=(consumer,))
            consumer_thread.daemon = True
            consumer_thread.start()
            
    def run_consumer(self, consumer):
        """Run consumer thread."""
        while True:
            try:
                task = self.queue.get(timeout=1.0)
                consumer.process_task(task)
                self.queue.task_done()
            except queue.Empty:
                continue
```

## Design Patterns

### 1. Strategy Pattern

#### Strategy Pattern for Action Selection

**Problem:** Switch between different action selection strategies.

**Solution:** Use strategy pattern for action selection.

```python
class ActionSelectionStrategy:
    def select_action(self, context):
        raise NotImplementedError

class GoalDrivenStrategy(ActionSelectionStrategy):
    def select_action(self, context):
        # Select action based on goals
        return self.plan_action_to_achieve_goals(context)

class RewardDrivenStrategy(ActionSelectionStrategy):
    def select_action(self, context):
        # Select action based on rewards
        return self.select_highest_reward_action(context)

class CuriosityDrivenStrategy(ActionSelectionStrategy):
    def select_action(self, context):
        # Select action based on curiosity
        return self.select_most_novel_action(context)

class ActionSelector:
    def __init__(self, strategy):
        self.strategy = strategy
        
    def set_strategy(self, strategy):
        self.strategy = strategy
        
    def select_action(self, context):
        return self.strategy.select_action(context)
```

### 2. Observer Pattern

#### Observer Pattern for System Monitoring

**Problem:** Notify multiple components when system state changes.

**Solution:** Use observer pattern for system monitoring.

```python
class SystemObserver:
    def update(self, subject, event_data):
        raise NotImplementedError

class StateChangeObserver(SystemObserver):
    def update(self, subject, event_data):
        # Handle state change event
        print(f"State changed: {event_data}")

class PerformanceObserver(SystemObserver):
    def update(self, subject, event_data):
        # Handle performance event
        print(f"Performance updated: {event_data}")

class SystemSubject:
    def __init__(self):
        self.observers = []
        
    def attach(self, observer):
        self.observers.append(observer)
        
    def detach(self, observer):
        self.observers.remove(observer)
        
    def notify(self, event_type, event_data):
        for observer in self.observers:
            observer.update(self, {'type': event_type, 'data': event_data})
```

### 3. Factory Pattern

#### Factory Pattern for System Creation

**Problem:** Create different types of systems based on configuration.

**Solution:** Use factory pattern for system creation.

```python
class SystemFactory:
    @staticmethod
    def create_system(system_type, config):
        if system_type == 'brain':
            return SystemFactory.create_brain(config)
        elif system_type == 'world':
            return SystemFactory.create_world(config)
        elif system_type == 'agent':
            return SystemFactory.create_agent(config)
        elif system_type == 'memory':
            return SystemFactory.create_memory_system(config)
        else:
            raise ValueError(f"Unknown system type: {system_type}")
            
    @staticmethod
    def create_brain(config):
        return Brain(config)
        
    @staticmethod
    def create_world(config):
        return SimpleWorld()
        
    @staticmethod
    def create_agent(config):
        return AgentBrain(config)
        
    @staticmethod
    def create_memory_system(config):
        memory_type = config.get('memory_type', 'working')
        if memory_type == 'working':
            return WorkingMemory()
        elif memory_type == 'episodic':
            return EpisodicMemory()
        elif memory_type == 'associative':
            return AssociativeMemory()
        else:
            raise ValueError(f"Unknown memory type: {memory_type}")
```

### 4. Command Pattern

#### Command Pattern for Action Execution

**Problem:** Execute actions with undo/redo capabilities.

**Solution:** Use command pattern for action execution.

```python
class ActionCommand:
    def execute(self):
        raise NotImplementedError
        
    def undo(self):
        raise NotImplementedError
        
    def redo(self):
        raise NotImplementedError

class MoveActionCommand(ActionCommand):
    def __init__(self, world, action):
        self.world = world
        self.action = action
        self.undone = False
        
    def execute(self):
        self.world.applyMotorCommand(self.action, self.world.getSimulationTime())
        self.undone = False
        
    def undo(self):
        # Implementation of undo
        pass
        
    def redo(self):
        # Implementation of redo
        pass

class CommandManager:
    def __init__(self):
        self.command_history = []
        self.redo_stack = []
        
    def execute_command(self, command):
        command.execute()
        self.command_history.append(command)
        self.redo_stack.clear()
        
    def undo(self):
        if self.command_history:
            command = self.command_history.pop()
            command.undo()
            self.redo_stack.append(command)
            
    def redo(self):
        if self.redo_stack:
            command = self.redo_stack.pop()
            command.redo()
            self.command_history.append(command)
```

### 5. Template Method Pattern

#### Template Method Pattern for Simulation

**Problem:** Define simulation algorithm skeleton with customizable steps.

**Solution:** Use template method pattern for simulation.

```python
class SimulationTemplate:
    def run_simulation(self):
        """Template method for running simulation."""
        self.initialize_simulation()
        self.setup_environment()
        
        for step in range(self.get_simulation_steps()):
            self.before_step(step)
            self.execute_step(step)
            self.after_step(step)
            
        self.cleanup_simulation()
        
    def initialize_simulation(self):
        """Initialize simulation (template method)."""
        self.setup_brain()
        self.setup_world()
        self.setup_agent()
        
    def before_step(self, step):
        """Before step execution (hook method)."""
        pass
        
    def after_step(self, step):
        """After step execution (hook method)."""
        pass
        
    def setup_brain(self):
        """Setup brain (concrete method)."""
        config = self.create_config()
        self.brain = self.create_brain(config)
        self.brain.initialize()
        
    def setup_world(self):
        """Setup world (concrete method)."""
        self.world = self.create_world()
        self.world.configure(
            self.get_world_width(),
            self.get_world_height(),
            self.get_vision_width(),
            self.get_vision_height()
        )
        self.world.reset()
        
    def setup_agent(self):
        """Setup agent (concrete method)."""
        self.agent = self.create_agent(self.brain)
        self.agent.initialize(self.world)
        
    def create_config(self):
        """Create configuration (template method)."""
        return pynlm.createDefaultConfig()
        
    def get_simulation_steps(self):
        """Get number of simulation steps (template method)."""
        return 100
        
    def get_world_width(self):
        """Get world width (template method)."""
        return 20
        
    def get_world_height(self):
        """Get world height (template method)."""
        return 20
        
    def get_vision_width(self):
        """Get vision width (template method)."""
        return 12
        
    def get_vision_height(self):
        """Get vision height (template method)."""
        return 12
```

## Pattern Usage Guidelines

### When to Use Each Pattern

| Pattern | Best Used For | Examples |
|---------|---------------|----------|
| Memory Pool | High-frequency memory allocation | Neuron and synapse management |
| Event-Driven | Complex event handling | Simulation event systems |
| State Management | State tracking and recovery | Simulation state management |
| Caching | Expensive repeated computations | Prediction caching |
| Stream Processing | Continuous data processing | Sensor data processing |
| Thread Pool | Concurrent task management | Parallel processing |
| Producer-Consumer | Task coordination | Task queue systems |
| Strategy | Multiple algorithms | Action selection strategies |
| Observer | Event notification | System monitoring |
| Factory | Object creation based on configuration | Dynamic system creation |
| Command | Undo/redo operations | Action execution |
| Template Method | Algorithm skeleton with customization | Simulation framework |

### Pattern Selection Criteria

1. **Problem Analysis**: Identify the specific problem being solved.
2. **Performance Requirements**: Consider performance requirements.
3. **Complexity**: Evaluate implementation complexity.
4. **Maintainability**: Assess maintenance requirements.
5. **Testability**: Consider testability implications.

### Pattern Implementation Best Practices

1. **Keep patterns simple**: Use patterns only when they add value.
2. **Follow naming conventions**: Use consistent naming.
3. **Document patterns**: Document pattern usage.
4. **Test patterns**: Test pattern implementations.
5. **Optimize patterns**: Optimize pattern performance.

## Common Anti-Patterns

### 1. Over-Engineering

**Problem:** Implementing complex solutions for simple problems.

**Solution:** Use simple solutions when appropriate.

```python
# Anti-pattern: Over-engineering
class ComplexNeuralNetwork:
    def __init__(self):
        self.neural_network = DeepLearningModel()
        self.training_data = self.load_training_data()
        self.hyperparameters = self.optimize_hyperparameters()
        self.validation_strategy = self.setup_cross_validation()
        
    def train(self):
        # Complex training loop
        pass

# Better: Simple implementation
class SimpleNeuralNetwork:
    def __init__(self):
        self.weights = self.initialize_weights()
        
    def train(self):
        # Simple training loop
        pass
```

### 2. Magic Numbers

**Problem:** Hard-coded values without explanation.

**Solution:** Use constants and named values.

```python
# Anti-pattern: Magic numbers
for i in range(100):
    process_data(data[i])

# Better: Named constants
MAX_ITERATIONS = 100
for i in range(MAX_ITERATIONS):
    process_data(data[i])
```

### 3. Global State

**Problem:** Using global variables instead of proper state management.

**Solution:** Use proper state management.

```python
# Anti-pattern: Global state
import global_state

global_state.some_variable = 10

def process_data():
    return global_state.some_variable

# Better: State management
class SystemState:
    def __init__(self):
        self.some_variable = 10
        
    def process_data(self):
        return self.some_variable
```

### 4. Deeply Nested Logic

**Problem:** Complex nested conditional logic.

**Solution:** Use strategy pattern or early returns.

```python
# Anti-pattern: Deeply nested logic
if condition1:
    if condition2:
        if condition3:
            if condition4:
                do_something()
                
# Better: Early returns and strategy pattern
if not condition1:
    return
if not condition2:
    return
if not condition3:
    return
if not condition4:
    return

do_something()
```

### 5. Exception Handling Abuse

**Problem:** Using exceptions for normal control flow.

**Solution:** Use proper error handling.

```python
# Anti-pattern: Exception handling abuse
try:
    result = risky_operation()
except Exception as e:
    # Handle all exceptions
    print(f"Error: {e}")
    
# Better: Specific exception handling
if not can_perform_operation():
    raise OperationError("Cannot perform operation")
    
try:
    result = risky_operation()
except IOError as e:
    raise IOError(f"File error: {e}")
except ValueError as e:
    raise ValueError(f"Value error: {e}")
```

## Pattern Migration Guidelines

### Migrating from Old to New Patterns

1. **Assessment**: Identify patterns in current codebase.
2. **Planning**: Plan migration strategy.
3. **Implementation**: Implement new patterns incrementally.
4. **Testing**: Test pattern implementations.
5. **Documentation**: Update documentation.

### Example: Migration from Global State to State Management

```python
# Before: Global state
import global_state

def process_data(data):
    return global_state.process(data)

# After: State management
class DataProcessor:
    def __init__(self):
        self.state = ProcessorState()
        
    def process_data(self, data):
        return self.state.process(data)
```

## Pattern Reuse Guidelines

### Reusing Existing Patterns

1. **Identify patterns**: Identify patterns in existing codebase.
2. **Adapt patterns**: Adapt patterns for new use cases.
3. **Document patterns**: Document pattern adaptations.
4. **Test patterns**: Test pattern adaptations.
5. **Share patterns**: Share patterns with team.

### Pattern Adaptation Guidelines

1. **Understand original pattern**: Understand original pattern intent.
2. **Analyze new requirements**: Analyze new requirements.
3. **Adapt components**: Adapt pattern components for new requirements.
4. **Test adaptation**: Test pattern adaptation.
5. **Document changes**: Document pattern changes.

## Pattern Performance Considerations

### Performance Optimization

1. **Profile patterns**: Profile pattern performance.
2. **Optimize bottlenecks**: Optimize pattern bottlenecks.
3. **Use efficient data structures**: Use efficient data structures.
4. **Minimize allocations**: Minimize memory allocations.
5. **Use caching**: Use caching for repeated operations.

### Performance Measurement

```python
import time
import memory_profiler

class PatternPerformanceTester:
    def __init__(self, pattern):
        self.pattern = pattern
        
    def measure_time(self, *args, **kwargs):
        """Measure execution time."""
        start_time = time.time()
        result = self.pattern(*args, **kwargs)
        end_time = time.time()
        return result, end_time - start_time
        
    def measure_memory(self, *args, **kwargs):
        """Measure memory usage."""
        mem_usage_before = memory_profiler.memory_usage()[0]
        result = self.pattern(*args, **kwargs)
        mem_usage_after = memory_profiler.memory_usage()[0]
        return result, mem_usage_after - mem_usage_before
        
    def measure_performance(self, *args, **kwargs):
        """Measure comprehensive performance."""
        time_result, time_taken = self.measure_time(*args, **kwargs)
        memory_result, memory_used = self.measure_memory(*args, **kwargs)
        return time_result, time_taken, memory_result, memory_used
```

## Pattern Documentation

### Pattern Documentation Template

```markdown
# Pattern: [Pattern Name]

## Description
[Brief description of the pattern]

## Problem
[Problem being solved]

## Solution
[Solution using the pattern]

## Benefits
[List of benefits]

## When to Use
[Situations where this pattern is appropriate]

## When Not to Use
[Situations where this pattern should not be used]

## Implementation
[Implementation details]

## Variations
[Common variations of the pattern]

## Performance Considerations
[Performance considerations for the pattern]

## Example Code
[Example code using the pattern]

## Related Patterns
[List related patterns]

## References
[References for further reading]
```

## Conclusion

Common patterns provide proven solutions to frequently encountered problems in NLM programming. By following these patterns, developers can write:

- **Cleaner code**: Use established patterns that are easier to understand
- **More maintainable code**: Use patterns that are easier to modify and extend
- **More performant code**: Use efficient patterns for common operations
- **More consistent code**: Follow established patterns for consistency

These patterns enable developers to focus on solving specific problems rather than reinventing common solutions, leading to more efficient and maintainable NLM code.

---

*Common Patterns document provides recurring code patterns and solutions for NLM programming.*
# NLM Python Advanced Usage Examples

This document provides comprehensive examples for advanced usage of the NLM Python API, including real-world scenarios and complex simulation patterns.

## Table of Contents

1. [Advanced Agent Training](#advanced-agent-training)
2. [Memory System Integration](#memory-system-integration)
3. [Neuromodulation Configuration](#neuromodulation-configuration)
4. [Development and Maturation](#development-and-maturation)
5. [Multi-Agent Systems](#multi-agent-systems)
6. [Advanced Configuration Management](#advanced-configuration-management)
7. [Performance Optimization](#performance-optimization)
8. [Error Handling and Robustness](#error-handling-and-robustness)
9. [Experiment Framework](#experiment-framework)
10. [Integration with External Systems](#integration-with-external-systems)

---

## Advanced Agent Training

This example demonstrates advanced training scenarios using curiosity-driven learning and episodic memory.

```python
import pynlm
import numpy as np
import time

class AdvancedAgentTrainer:
    def __init__(self):
        self.training_history = []
        self.performance_metrics = {}
        
    def create_training_environment(self, config_type="experimental"):
        """Create a sophisticated training environment."""
        
        # Create enhanced configuration with advanced settings
        config = pynlm.createPresetConfig(config_type)
        
        # Add custom training-specific settings
        config.set("training.max_episodes", 1000)
        config.set("training.learning_rate", 0.001)
        config.set("training.exploration_rate", 0.2)
        config.set("memory.episodic_capacity", 5000)
        config.set("memory.associative_strength", 0.8)
        
        # Create simulation components
        self.brain = pynlm.createBrain(config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world for training
        self.world.configure(
            width=50,
            height=50,
            visionWidth=16,
            visionHeight=16
        )
        
        # Set agent starting position
        self.world.setAgentStart(25.0, 25.0)
        
        # Initialize all systems
        self.brain.initialize()
        self.agent.initialize(self.world)
        
        # Enable advanced learning capabilities
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        
        return config
        
    def train_episode(self, episode_num, max_steps=200):
        """Train for a single episode with advanced learning strategies."""
        
        # Reset environment
        self.world.reset()
        self.agent.reset()
        self.brain.reset()
        
        episode_metrics = {
            'episode': episode_num,
            'total_reward': 0.0,
            'steps_completed': 0,
            'actions_taken': {},
            'curiosity_history': [],
            'novelty_history': [],
            'energy_history': []
        }
        
        for step in range(max_steps):
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Record curiosity and novelty for learning analysis
            curiosity = self.agent.getCuriosityLevel()
            novelty = self.agent.getNoveltyLevel()
            energy = self.world.getAgentBody().energy
            
            episode_metrics['curiosity_history'].append(curiosity)
            episode_metrics['novelty_history'].append(novelty)
            episode_metrics['energy_history'].append(energy)
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Decode motor command with exploration strategy
            action = self.decode_action_with_exploration(step)
            
            # Apply action to world
            action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = action_result.reward
            predicted_reward = self.agent.getPredictionError()
            self.agent.applyRewardModulation(reward, predicted_reward)
            
            # Track action statistics
            action_key = action.value if hasattr(action, 'value') else str(action)
            episode_metrics['actions_taken'][action_key] = \
                episode_metrics['actions_taken'].get(action_key, 0) + 1
                
            episode_metrics['total_reward'] += reward
            episode_metrics['steps_completed'] += 1
            
            # Update development every 50 steps
            if step % 50 == 0:
                self.agent.updateDevelopment(0.05)
            
            # Print progress for important milestones
            if step % 50 == 0:
                self.print_step_progress(step, episode_metrics)
                
        return episode_metrics
        
    def decode_action_with_exploration(self, step):
        """Decode action with curiosity-driven exploration."""
        
        # Get baseline action from brain
        action = self.agent.decodeMotorCommand()
        
        # Add curiosity-driven exploration
        curiosity = self.agent.getCuriosityLevel()
        
        if curiosity > 0.7 and np.random.random() < 0.3:  # High curiosity -> exploration
            # Random exploration action
            exploration_actions = [
                type('Action', (), {'value': 'MoveForward'})(),
                type('Action', (), {'value': 'MoveBackward'})(),
                type('Action', (), {'value': 'TurnLeft'})(),
                type('Action', (), {'value': 'TurnRight'})(),
                type('Action', (), {'value': 'Interact'})(),
            ]
            action = np.random.choice(exploration_actions)
            
        return action
        
    def print_step_progress(self, step, metrics):
        """Print progress information."""
        
        avg_curiosity = np.mean(metrics['curiosity_history'][-10:])
        avg_novelty = np.mean(metrics['novelty_history'][-10:])
        avg_energy = np.mean(metrics['energy_history'][-10:])
        
        print(f"Step {step}:")
        print(f"  Reward: {metrics['total_reward']:.3f}")
        print(f"  Curiously: {avg_curiosity:.3f}, Novel: {avg_novelty:.3f}")
        print(f"  Energy: {avg_energy:.3f}")
        print(f"  Developmental stage: {self.agent.getDevelopmentalStage()}")
        print(f"  Action counts: {metrics['actions_taken']}")
        
    def train_advanced_agent(self, num_episodes=100):
        """Train advanced agent with sophisticated learning strategies."""
        
        print("=== ADVANCED AGENT TRAINING ===\n")
        
        # Create training environment
        config = self.create_training_environment("experimental")
        
        # Training loop with advanced strategies
        for episode in range(num_episodes):
            print(f"\n--- Episode {episode + 1}/{num_episodes} ---\n")
            
            # Train episode
            episode_metrics = self.train_episode(episode)
            
            # Record metrics
            self.training_history.append(episode_metrics)
            
            # Store performance metrics
            self.performance_metrics[episode] = {
                'final_reward': episode_metrics['total_reward'],
                'final_curiosity': episode_metrics['curiosity_history'][-1],
                'final_energy': episode_metrics['energy_history'][-1],
                'action_distribution': episode_metrics['actions_taken']
            }
            
            # Save model periodically
            if (episode + 1) % 50 == 0:
                checkpoint_file = f"agent_checkpoint_ep{episode + 1}.bin"
                self.save_checkpoint(checkpoint_file)
                print(f"Model saved to {checkpoint_file}")
                
            # Print episode summary
            self.print_episode_summary(episode, episode_metrics)
            
        # Save final model
        self.save_checkpoint("agent_final.bin")
        print("\n=== TRAINING COMPLETE ===")
        
        return self.performance_metrics
        
    def save_checkpoint(self, filename):
        """Save agent state to checkpoint file."""
        return self.brain.save(filename)
        
    def print_episode_summary(self, episode, metrics):
        """Print episode summary with performance analysis."""
        
        avg_reward = metrics['total_reward'] / metrics['steps_completed']
        
        print(f"Episode Summary:")
        print(f"  Total reward: {metrics['total_reward']:.3f}")
        print(f"  Average reward/step: {avg_reward:.3f}")
        print(f"  Steps completed: {metrics['steps_completed']}")
        print(f"  Final curiosity: {metrics['curiosity_history'][-1]:.3f}")
        print(f"  Final energy: {metrics['energy_history'][-1]:.3f}")
        
        # Analyze action patterns
        if metrics['actions_taken']:
            most_common_action = max(metrics['actions_taken'].items(), 
                                   key=lambda x: x[1])
            print(f"  Most common action: {most_common_action[0]} ({most_common_action[1]} times)")

# Usage example
trainer = AdvancedAgentTrainer()
performance = trainer.train_advanced_agent(num_episodes=50)
```

## Memory System Integration

This example demonstrates advanced memory system integration and usage patterns.

```python
import pynlm
import numpy as np

class MemorySystemManager:
    def __init__(self):
        self.memory_stats = {}
        
    def setup_memory_systems(self, config):
        """Setup comprehensive memory systems."""
        
        # Create brain with enhanced configuration
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        # Access memory systems
        self.working_memory = self.brain.getWorkingMemory()
        self.episodic_memory = self.brain.getEpisodicMemory()
        self.associative_memory = self.brain.getAssociativeMemory()
        
        # Configure memory capacities
        self.working_memory.setCapacity(2000)
        self.episodic_memory.setMaxEpisodes(10000)
        
    def demonstrate_working_memory(self, world, agent, steps=100):
        """Demonstrate working memory capabilities."""
        
        print("=== WORKING MEMORY DEMONSTRATION ===\n")
        
        # Create training scenario
        world.reset()
        agent.reset()
        self.brain.reset()
        
        # Store initial state in working memory
        initial_percept = world.getSensoryPercept()
        agent.processSensoryInput(initial_percept)
        
        # Run simulation while monitoring working memory
        memory_trace_history = []
        
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Process new percept
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Record working memory state
            trace_count = self.working_memory.getActiveTraces()
            memory_trace_history.append(trace_count)
            
            if step % 20 == 0:
                print(f"Step {step}: Active traces = {trace_count}")
                
        # Analyze memory traces
        avg_traces = np.mean(memory_trace_history)
        max_traces = np.max(memory_trace_history)
        
        print(f"\nWorking Memory Analysis:")
        print(f"  Average active traces: {avg_traces:.1f}")
        print(f"  Maximum active traces: {max_traces}")
        print(f"  Memory utilization: {avg_traces/self.working_memory.getCapacity()*100:.1f}%")
        
        return memory_trace_history
        
    def demonstrate_episodic_memory(self, world, agent, steps=200):
        """Demonstrate episodic memory capabilities."""
        
        print("\n=== EPISODIC MEMORY DEMONSTRATION ===\n")
        
        # Reset environment
        world.reset()
        agent.reset()
        self.brain.reset()
        
        # Create episodic memory items
        for episode_num in range(20):
            # Create an episodic memory item
            episode = self.create_episodic_item(episode_num, world, agent)
            
            # Store in episodic memory
            self.episodic_memory.storeEpisode(episode)
            
            # Simulate world state
            for step in range(10):
                world.update(0.1)
                
                # Process percept
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                self.brain.step(step)
                
        # Analyze episodic memory
        episode_count = self.episodic_memory.getEpisodeCount()
        avg_reward = self.episodic_memory.getAverageReward()
        
        print(f"Episodic Memory Analysis:")
        print(f"  Total episodes stored: {episode_count}")
        print(f"  Average episode reward: {avg_reward:.3f}")
        
        # Demonstrate retrieval
        print(f"\nRetrieval Demonstration:")
        
        # Get recent episodes
        recent_episodes = self.episodic_memory.getRecentEpisodes(5)
        print(f"  Recent episodes retrieved: {len(recent_episodes)}")
        
        # Get episodes for replay
        replay_episodes = self.episodic_memory.getEpisodesForReplay(3)
        print(f"  Episodes selected for replay: {len(replay_episodes)}")
        
        # Demonstrate replay
        if replay_episodes:
            print(f"  Replaying episodes...")
            episode_ids = [id(ep) for ep in replay_episodes]  # Get episode indices
            self.episodic_memory.replaySequence(episode_ids)
            
    def create_episodic_item(self, episode_num, world, agent):
        """Create an episodic memory item."""
        
        # Get current state
        agent_body = world.getAgentBody()
        percept = world.getSensoryPercept()
        
        # Create episodic memory item
        episode = type('EpisodicMemoryItem', (), {})()
        episode.timestamp = episode_num
        episode.positionX = agent_body.x
        episode.positionY = agent_body.y
        episode.orientation = agent_body.orientation
        episode.action = 'MoveForward'  # Simplified
        episode.reward = 0.5 + np.random.random() * 0.5
        episode.energy = agent_body.energy
        episode.novelty = np.random.random()
        episode.age = 0
        
        return episode
        
    def demonstrate_associative_memory(self):
        """Demonstrate associative memory capabilities."""
        
        print("\n=== ASSOCIATIVE MEMORY DEMONSTRATION ===\n")
        
        # Create pattern associations
        patterns = [
            np.array([0.1, 0.2, 0.3, 0.4, 0.5]),
            np.array([0.2, 0.3, 0.4, 0.5, 0.6]),
            np.array([0.3, 0.4, 0.5, 0.6, 0.7]),
            np.array([0.4, 0.5, 0.6, 0.7, 0.8]),
        ]
        
        # Create associations between patterns
        for i in range(len(patterns)):
            for j in range(i + 1, len(patterns)):
                # Check if patterns are similar
                similarity = self.compute_pattern_similarity(patterns[i], patterns[j])
                
                if similarity > 0.7:  # Strong association
                    self.associative_memory.associate(patterns[i], patterns[j], similarity)
                    print(f"Associated patterns {i} and {j} with strength {similarity:.3f}")
        
        # Demonstrate retrieval
        print(f"\nAssociative Memory Analysis:")
        print(f"  Total associations: {self.associative_memory.getAssociationCount()}")
        
        # Test pattern retrieval
        query_pattern = np.array([0.15, 0.25, 0.35, 0.45, 0.55])
        retrieved_patterns = self.associative_memory.retrieve(query_pattern, max_results=3)
        
        print(f"  Retrieved patterns for query: {len(retrieved_patterns)}")
        for i, pattern in enumerate(retrieved_patterns):
            print(f"    Pattern {i}: {pattern}")
            
    def compute_pattern_similarity(self, pattern_a, pattern_b):
        """Compute similarity between two patterns."""
        if len(pattern_a) != len(pattern_b):
            return 0.0
            
        # Cosine similarity
        dot_product = np.dot(pattern_a, pattern_b)
        norm_a = np.linalg.norm(pattern_a)
        norm_b = np.linalg.norm(pattern_b)
        
        if norm_a < 0.001 or norm_b < 0.001:
            return 0.0
            
        return dot_product / (norm_a * norm_b)
        
    def analyze_memory_system_performance(self):
        """Analyze overall memory system performance."""
        
        print("\n=== MEMORY SYSTEM PERFORMANCE ANALYSIS ===\n")
        
        # Working memory statistics
        working_trace_count = self.working_memory.getActiveTraces()
        working_capacity = self.working_memory.getCapacity()
        
        print(f"Working Memory:")
        print(f"  Active traces: {working_trace_count}")
        print(f"  Capacity: {working_capacity}")
        print(f"  Utilization: {working_trace_count/working_capacity*100:.1f}%")
        
        # Episodic memory statistics
        episodic_count = self.episodic_memory.getEpisodeCount()
        episodic_max = self.episodic_memory.getMaxEpisodes()
        avg_reward = self.episodic_memory.getAverageReward()
        
        print(f"\nEpisodic Memory:")
        print(f"  Stored episodes: {episodic_count}")
        print(f"  Maximum capacity: {episodic_max}")
        print(f"  Memory utilization: {episodic_count/episodic_max*100:.1f}%")
        print(f"  Average reward: {avg_reward:.3f}")
        
        # Associative memory statistics
        assoc_count = self.associative_memory.getAssociationCount()
        print(f"\nAssociative Memory:")
        print(f"  Total associations: {assoc_count}")

# Usage example
memory_manager = MemorySystemManager()
config = pynlm.createPresetConfig("medium")
memory_manager.setup_memory_systems(config)

# Run demonstrations
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(memory_manager.brain)
agent.initialize(world)

working_history = memory_manager.demonstrate_working_memory(world, agent)
memory_manager.demonstrate_episodic_memory(world, agent)
memory_manager.demonstrate_associative_memory()
memory_manager.analyze_memory_system_performance()
```

## Neuromodulation Configuration

This example demonstrates advanced neuromodulation configuration and usage.

```python
import pynlm
import numpy as np

class NeuromodulationController:
    def __init__(self):
        self.neuromodulation_history = {}
        self.dopamine_levels = []
        self.curiosity_levels = []
        self.novelty_levels = []
        
    def setup_advanced_neuromodulation(self, agent):
        """Setup advanced neuromodulation systems."""
        
        # Configure neuromodulators
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Get neuromodulation access
        self.dopamine = agent.getBrain().getDopamine()
        self.curiosity = agent.getCuriosity()
        self.novelty = agent.getCuriosity()  # Novelty uses same access for simplicity
        
        # Configure neuromodulator parameters
        if self.dopamine:
            self.dopamine.setLearningRate(0.01)
            self.dopamine.setScale(1.0)
            
        if self.curiosity:
            self.curiosity.setThreshold(0.3)
            self.curiosity.setDecayRate(0.95)
            
        if self.novelty:
            self.novelty.setThreshold(0.2)
            self.novelty.setInfluence(0.8)
            
    def simulate_neuromodulation_dynamics(self, agent, world, steps=200):
        """Simulate neuromodulation dynamics over time."""
        
        print("=== NEUROMODULATION DYNAMICS SIMULATION ===\n")
        
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain step
            agent.getBrain().step(step)
            
            # Record neuromodulation levels
            if agent.getBrain().getDopamine():
                self.dopamine_levels.append(agent.getBrain().getDopamine().getLevel())
                
            if agent.getCuriosity():
                self.curiosity_levels.append(agent.getCuriosity().getLevel())
                
            if agent.getNovelty():
                # Note: Novelty level access is simplified here
                self.novelty_levels.append(np.random.random())  # Placeholder
            
            # Apply reward modulation every 50 steps
            if step % 50 == 0:
                reward = np.random.random() * 2 - 1  # Random reward
                predicted_reward = agent.getPredictionError()
                agent.applyRewardModulation(reward, predicted_reward)
                
            # Update development
            if step % 100 == 0:
                agent.updateDevelopment(0.1)
                
            # Print progress
            if step % 50 == 0:
                self.print_neuromodulation_status(step)
                
        return {
            'dopamine_levels': self.dopamine_levels,
            'curiosity_levels': self.curiosity_levels,
            'novelty_levels': self.novelty_levels
        }
        
    def print_neuromodulation_status(self, step):
        """Print current neuromodulation status."""
        
        if not self.dopamine_levels or not self.curiosity_levels:
            return
            
        avg_dopamine = np.mean(self.dopamine_levels[-10:])
        avg_curiosity = np.mean(self.curiosity_levels[-10:])
        
        print(f"Step {step}:")
        print(f"  Average dopamine: {avg_dopamine:.3f}")
        print(f"  Average curiosity: {avg_curiosity:.3f}")
        print(f"  Agent developmental stage: {agent.getDevelopmentalStage()}")
        
    def analyze_neuromodulation_patterns(self):
        """Analyze neuromodulation patterns and statistics."""
        
        print("\n=== NEUROMODULATION PATTERNS ANALYSIS ===\n")
        
        if not self.dopamine_levels or not self.curiosity_levels:
            print("No neuromodulation data available.")
            return
            
        # Dopamine analysis
        dopamine_mean = np.mean(self.dopamine_levels)
        dopamine_std = np.std(self.dopamine_levels)
        dopamine_peaks = np.sum(np.array(self.dopamine_levels) > 0.5)
        
        print(f"Dopamine Analysis:")
        print(f"  Mean level: {dopamine_mean:.3f}")
        print(f"  Standard deviation: {dopamine_std:.3f}")
        print(f"  Peak levels (>0.5): {dopamine_peaks} ({dopamine_peaks/len(self.dopamine_levels)*100:.1f}%)")
        
        # Curiosity analysis
        curiosity_mean = np.mean(self.curiosity_levels)
        curiosity_std = np.std(self.curiosity_levels)
        curiosity_high = np.sum(np.array(self.curiosity_levels) > 0.7)
        
        print(f"\nCuriosity Analysis:")
        print(f"  Mean level: {curiosity_mean:.3f}")
        print(f"  Standard deviation: {curiosity_std:.3f}")
        print(f"  High levels (>0.7): {curiosity_high} ({curiosity_high/len(self.curiosity_levels)*100:.1f}%)")
        
        # Novelty analysis (if available)
        if self.novelty_levels:
            novelty_mean = np.mean(self.novelty_levels)
            novelty_std = np.std(self.novelty_levels)
            
            print(f"\nNovelty Analysis:")
            print(f"  Mean level: {novelty_mean:.3f}")
            print(f"  Standard deviation: {novelty_std:.3f}")
            
    def simulate_stress_response(self, agent, world, stress_steps=100):
        """Simulate stress response in neuromodulation system."""
        
        print("\n=== STRESS RESPONSE SIMULATION ===\n")
        
        stress_events = []
        stress_impact = []
        
        for step in range(stress_steps):
            # Apply stress event every 30 steps
            if step % 30 == 0:
                stress_event = {
                    'step': step,
                    'type': 'reward_loss',
                    'magnitude': np.random.random() * 0.5 + 0.5
                }
                stress_events.append(stress_event)
                
                # Apply negative reward
                agent.applyRewardModulation(-stress_event['magnitude'], 0.0)
                
            # Record neuromodulation impact
            current_dopamine = agent.getBrain().getDopamine().getLevel() if agent.getBrain().getDopamine() else 0.0
            current_curiosity = agent.getCuriosity().getLevel() if agent.getCuriosity() else 0.0
            
            stress_impact.append({
                'step': step,
                'dopamine': current_dopamine,
                'curiosity': current_curiosity
            })
            
            # Update world and process
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            agent.getBrain().step(step)
            
        # Analyze stress response
        self.analyze_stress_response(stress_events, stress_impact)
        
    def analyze_stress_response(self, stress_events, stress_impact):
        """Analyze stress response patterns."""
        
        print(f"Stress Events: {len(stress_events)}")
        print(f"Impact periods: {len([impact for impact in stress_impact if impact['dopamine'] < 0.1])}")
        
        if stress_impact:
            # Find dopamine recovery after stress
            dopamine_after_stress = [impact['dopamine'] for impact in stress_impact 
                                   if impact['step'] > 30]
            
            if dopamine_after_stress:
                recovery_time = len(dopamine_after_stress)
                final_dopamine = dopamine_after_stress[-1]
                
                print(f"Dopamine recovery time: {recovery_time} steps")
                print(f"Final dopamine level: {final_dopamine:.3f}")
                
        # Curiosity response to stress
        curiosity_impact = [impact['curiosity'] for impact in stress_impact]
        
        if curiosity_impact:
            avg_curiosity_after_stress = np.mean(curiosity_impact[30:])
            print(f"Average curiosity after stress: {avg_curiosity_after_stress:.3f}")

# Usage example
neuromodulation_controller = NeuromodulationController()

# Create environment
config = pynlm.createPresetConfig("experimental")
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

brain.initialize()
agent.initialize(world)

# Setup neuromodulation
neuromodulation_controller.setup_advanced_neuromodulation(agent)

# Run neuromodulation simulation
results = neuromodulation_controller.simulate_neuromodulation_dynamics(agent, world)

# Analyze neuromodulation patterns
neuromodulation_controller.analyze_neuromodulation_patterns()

# Simulate stress response
neuromodulation_controller.simulate_stress_response(agent, world)
```

---

This document provides comprehensive examples for advanced NLM Python usage. Each example demonstrates real-world scenarios and can be used as a starting point for developing complex NLM applications.

For more information on specific functions and classes, refer to the NLM Python API Reference documentation.

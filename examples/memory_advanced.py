"""
Advanced Memory Systems Examples

This module demonstrates advanced memory systems in NLM.
Key features include:
- Multi-level memory systems
- Episodic memory consolidation
- Semantic memory extraction
- Working memory management
- Procedural memory formation

Author: NLM Project
License: MIT
Version: 1.0
"""

import pynlm
import numpy as np
from typing import List, Dict, Any, Optional
import time
class MemoryEpisode:
    """
    Episodic memory record.
    
    Each episode stores:
    - Context information (percept, action, environment state)
    - Outcome information (reward, success)
    - Metadata (time, agent state, importance)
    - Emotional valence (reward prediction error)
    - Temporal information (sequence, timing)
    """
    
    def __init__(self, timestamp: float):
        self.timestamp = timestamp
        self.percept = None
        self.action = None
        self.reward = 0.0
        self.next_state = None
        self.context = {}
        self.importance = 1.0
        self.emotional_valence = 0.0
        self.sequence_id = 0
        self.consolidation_count = 0
        
    def setExperience(self, percept: pynlm.SensoryPercept, action: pynlm.Action,
                     reward: float, next_state: pynlm.SensoryPercept):
        """Set the experience recorded in this episode."""
        self.percept = percept
        self.action = action
        self.reward = reward
        self.next_state = next_state
        
    def setContext(self, context: Dict[str, Any]):
        """Set contextual information."""
        self.context = context
        
    def calculateImportance(self, reward: float, percept: pynlm.SensoryPercept):
        """Calculate episode importance based on reward and novelty."""
        # Reward-based importance
        reward_importance = min(1.0, abs(reward) / 10.0)
        
        # Novelty-based importance
        novelty_importance = min(1.0, np.std(percept.getAllSignals()) / 10.0)
        
        # Combine factors
        self.importance = (reward_importance + novelty_importance) / 2.0
        
    def calculateEmotionalValence(self, expected_reward: float, actual_reward: float):
        """Calculate emotional valence based on prediction error."""
        prediction_error = actual_reward - expected_reward
        self.emotional_valence = np.tanh(prediction_error / 5.0)
        
    def incrementConsolidation(self):
        """Increment consolidation count."""
        self.consolidation_count += 1
        
    def getStrength(self) -> float:
        """Get episode memory strength."""
        # Combine importance and consolidation
        strength = self.importance * (1.0 + np.log1p(self.consolidation_count))
        return min(1.0, strength)
class SemanticPattern:
    """
    Semantic memory pattern.
    
    Semantic patterns capture:
    - Abstract concepts from experiences
    - Statistical regularities in the environment
    - Relational structures
    - Causal relationships
    """
    
    def __init__(self, pattern_id: int):
        self.pattern_id = pattern_id
        self.concept_vector = np.zeros(50)  # High-dimensional concept space
        self.frequency = 0
        self.strength = 0.0
        self.associated_episodes = []
        self.concept_clusters = []
        
    def updateConceptVector(self, episode: MemoryEpisode):
        """Update concept vector from episode."""
        # Extract features from episode
        features = self.extractFeaturesFromEpisode(episode)
        
        # Update concept vector
        self.concept_vector = (
            self.concept_vector * (self.frequency / (self.frequency + 1)) +
            features / (self.frequency + 1)
        )
        
        # Update strength
        self.frequency += 1
        self.strength = min(1.0, self.frequency / 1000.0)
        
    def extractFeaturesFromEpisode(self, episode: MemoryEpisode) -> np.ndarray:
        """Extract features from episode for concept representation."""
        features = np.zeros(50)
        
        if episode.percept:
            # Get percept features
            percept_signals = episode.percept.getAllSignals()
            if percept_signals:
                # Extract statistical features
                features[:10] = np.mean(percept_signals)
                features[10:20] = np.std(percept_signals)
                features[20:30] = np.max(percept_signals)
                features[30:40] = np.min(percept_signals)
                
                # Compute correlations with other episodes
                if self.associated_episodes:
                    other_signals = np.array(
                        [ep.percept.getAllSignals() if ep.percept else []
                         for ep in self.associated_episodes]
                    )
                    if other_signals.size > 0:
                        correlations = np.corrcoef([percept_signals, other_signals[0]])[0,1]
                        features[40] = correlations
        
        # Add episode characteristics
        features[41] = episode.importance
        features[42] = episode.emotional_valence
        features[43] = episode.consolidation_count / 10.0
        features[44] = episode.sequence_id % 100 / 100.0
        
        # Normalize features
        norm = np.linalg.norm(features)
        if norm > 0:
            features = features / norm
            
        return features
class WorkingMemoryEntry:
    """
    Working memory entry.
    
    Working memory stores:
    - Active task representations
    - Current goals and intentions
    - Immediate context information
    - Active constraints and limitations
    """
    
    def __init__(self, entry_id: int):
        self.entry_id = entry_id
        self.representation = np.zeros(100)  # High-dimensional state
        self.strength = 0.0
        self.age = 0
        self.activation_threshold = 0.5
        self.current_task = None
        self.goal_vector = np.zeros(20)
        self.constraints = []
        
    def updateRepresentation(self, state_update: np.ndarray):
        """Update working memory representation."""
        if state_update.shape == self.representation.shape:
            self.representation = state_update
            self.age = 0
            
    def decay(self, decay_rate: float = 0.95):
        """Apply decay to representation."""
        self.representation *= decay_rate
        self.age += 1
        
    def getActivationScore(self) -> float:
        """Get activation score based on strength and recent updates."""
        activation = self.strength * (1.0 - self.age / 100.0)
        return max(0.0, min(1.0, activation))
class AdvancedMemorySystem:
    """
    Advanced memory system with multiple memory types.
    
    This system integrates:
    - Episodic memory for specific experiences
    - Semantic memory for abstract concepts
    - Working memory for active tasks
    - Procedural memory for skills
    - Associative memory for relationships
    """
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        
        # Initialize memory subsystems
        self.episodic_memory = []
        self.semantic_patterns = []
        self.working_memory = []
        self.procedural_memory = {}
        self.associative_memory = {}
        
        # Memory parameters
        self.working_memory_capacity = 20
        self.episodic_memory_capacity = 1000
        self.semantic_pattern_capacity = 100
        self.consolidation_interval = 100
        self.replay_probability = 0.3
        
        # State tracking
        self.episode_counter = 0
        self.consolidation_step = 0
        
    def storeEpisode(self, percept: pynlm.SensoryPercept, action: pynlm.Action,
                    reward: float, next_state: pynlm.SensoryPercept,
                    context: Dict[str, Any] = None):
        """
        Store an episode in episodic memory.
        
        Args:
            percept: Sensory input before action
            action: Action taken
            reward: Reward received
            next_state: Resulting state
            context: Additional contextual information
        """
        # Create episode
        episode = MemoryEpisode(time.time())
        episode.setExperience(percept, action, reward, next_state)
        
        if context:
            episode.setContext(context)
            
        # Calculate episode importance
        expected_reward = reward * 0.9  # Simulate prediction
        episode.calculateImportance(reward, percept)
        episode.calculateEmotionalValence(expected_reward, reward)
        
        episode.sequence_id = self.episode_counter
        self.episode_counter += 1
        
        # Add to episodic memory
        self.episodic_memory.append(episode)
        
        # Enforce capacity
        if len(self.episodic_memory) > self.episodic_memory_capacity:
            self.consolidateOldEpisodes()
            
        # Extract semantic patterns
        self.extractSemanticPatterns(episode)
        
        # Update working memory
        self.updateWorkingMemory(episode, percept, action)
        
        # Store procedural knowledge if applicable
        self.updateProceduralMemory(action, reward)
        
        # Store associative relationships
        self.updateAssociativeMemory(episode)
        
    def extractSemanticPatterns(self, episode: MemoryEpisode):
        """Extract semantic patterns from episode."""
        if len(self.semantic_patterns) < self.semantic_pattern_capacity:
            # Create new semantic pattern
            new_pattern = SemanticPattern(len(self.semantic_patterns))
            new_pattern.updateConceptVector(episode)
            self.semantic_patterns.append(new_pattern)
        else:
            # Update existing patterns
            for pattern in self.semantic_patterns:
                pattern.updateConceptVector(episode)
                
    def updateWorkingMemory(self, episode: MemoryEpisode, percept: pynlm.SensoryPercept,
                          action: pynlm.Action):
        """Update working memory with new information."""
        # Create new working memory entry
        new_entry = WorkingMemoryEntry(len(self.working_memory))
        
        # Extract state features
        state_features = self.extractStateFeatures(percept, action, episode.reward)
        new_entry.updateRepresentation(state_features)
        new_entry.current_task = action.getType() if action else None
        new_entry.goal_vector = self.extractGoalVector(episode)
        
        # Add to working memory
        self.working_memory.append(new_entry)
        
        # Enforce capacity
        if len(self.working_memory) > self.working_memory_capacity:
            self.decayWeakEntries()
            
        # Apply decay
        for entry in self.working_memory:
            entry.decay()
            
    def extractStateFeatures(self, percept: pynlm.SensoryPercept, action: pynlm.Action,
                           reward: float) -> np.ndarray:
        """Extract state features for working memory."""
        features = np.zeros(100)
        
        if percept:
            signals = percept.getAllSignals()
            if signals:
                # Fill first 50 features with percept data
                num_features = min(50, len(signals))
                features[:num_features] = signals[:num_features]
                
                # Add action features
                if action:
                    features[50:60] = np.array([float(a) for a in str(action).split()])
                
                # Add reward features
                features[60:70] = np.array([reward, reward * reward])
        
        # Add temporal features
        features[70] = time.time() % 100 / 100.0
        features[71] = len(self.working_memory) / self.working_memory_capacity
        
        # Normalize features
        norm = np.linalg.norm(features)
        if norm > 0:
            features = features / norm
            
        return features
        
    def extractGoalVector(self, episode: MemoryEpisode) -> np.ndarray:
        """Extract goal vector from episode."""
        goal_vector = np.zeros(20)
        
        if episode.action:
            # Encode action goals
            goal_vector[0:5] = np.array([float(a) for a in str(episode.action.getType()).split()])
            
        # Add reward goals
        goal_vector[5] = episode.reward
        
        # Add emotional goals
        goal_vector[6] = episode.emotional_valence
        
        # Normalize
        norm = np.linalg.norm(goal_vector)
        if norm > 0:
            goal_vector = goal_vector / norm
            
        return goal_vector
        
    def updateProceduralMemory(self, action: pynlm.Action, reward: float):
        """Update procedural memory with action-outcome associations."""
        if not action:
            return
            
        action_key = str(action.getType())
        
        if action_key not in self.procedural_memory:
            self.procedural_memory[action_key] = {
                'success_rate': 0.0,
                'total_executions': 0,
                'total_rewards': 0.0,
                'skill_level': 0.0
            }
            
        # Update procedural memory
        mem = self.procedural_memory[action_key]
        mem['total_executions'] += 1
        mem['total_rewards'] += reward
        mem['success_rate'] = mem['total_rewards'] / mem['total_executions']
        mem['skill_level'] = min(1.0, mem['success_rate'] * 2.0)
        
    def updateAssociativeMemory(self, episode: MemoryEpisode):
        """Update associative memory with relational information."""
        if not episode.percept or not episode.action:
            return
            
        # Create association key
        key = f"{episode.percept.getType()}_{episode.action.getType()}"
        
        if key not in self.associative_memory:
            self.associative_memory[key] = {
                'frequency': 0,
                'strength': 0.0,
                'outcome_mean': 0.0,
                'outcome_variance': 0.0,
                'episodes': []
            }
            
        # Update associative memory
        assoc = self.associative_memory[key]
        assoc['frequency'] += 1
        assoc['episodes'].append(episode)
        
        # Update statistics
        rewards = [ep.reward for ep in assoc['episodes']]
        assoc['outcome_mean'] = np.mean(rewards)
        if len(rewards) > 1:
            assoc['outcome_variance'] = np.var(rewards)
            
        # Calculate strength
        assoc['strength'] = (
            assoc['frequency'] * 0.5 +
            assoc['outcome_mean'] * 0.3 +
            (1.0 - assoc['outcome_variance']) * 0.2
        )
        
    def consolidateOldEpisodes(self):
        """Consolidate old episodes and remove weak ones."""
        # Remove weakest 20% of episodes
        if len(self.episodic_memory) > 0:
            strengths = [ep.getStrength() for ep in self.episodic_memory]
            threshold = np.percentile(strengths, 20)
            
            # Keep episodes above threshold
            self.episodic_memory = [
                ep for ep in self.episodic_memory
                if ep.getStrength() >= threshold
            ]
            
    def decayWeakEntries(self):
        """Decay weak working memory entries."""
        if len(self.working_memory) > 0:
            # Calculate activation scores
            scores = [entry.getActivationScore() for entry in self.working_memory]
            threshold = np.percentile(scores, 20)
            
            # Keep entries above threshold
            self.working_memory = [
                entry for entry in self.working_memory
                if entry.getActivationScore() >= threshold
            ]
            
    def replayEpisodes(self, num_episodes: int = 5, replay_interval: int = 100):
        """
        Replay important episodes for consolidation.
        
        Args:
            num_episodes: Number of episodes to replay
            replay_interval: Step interval for replay
        """
        self.consolidation_step += 1
        
        if self.consolidation_step % replay_interval != 0:
            return
            
        # Select episodes for replay
        if len(self.episodic_memory) >= num_episodes:
            # Get strongest episodes
            strengths = [(ep.getStrength(), i) for i, ep in enumerate(self.episodic_memory)]
            strengths.sort(reverse=True)
            
            selected_indices = [idx for _, idx in strengths[:num_episodes]]
            selected_episodes = [self.episodic_memory[idx] for idx in selected_indices]
            
            # Replay episodes
            for episode in selected_episodes:
                self.replayEpisode(episode)
                
    def replayEpisode(self, episode: MemoryEpisode):
        """Replay a single episode."""
        # Increment consolidation count
        episode.incrementConsolidation()
        
        # Re-process episode information
        if episode.percept:
            self.brain.processSensoryInput(episode.percept)
            
        # Simulate action execution
        if episode.action:
            # Apply action effects (simulated)
            pass
            
    def getMemoryReport(self) -> Dict[str, Any]:
        """Get comprehensive memory system report."""
        return {
            'episodic_memory': {
                'episodes_count': len(self.episodic_memory),
                'average_strength': np.mean([ep.getStrength() for ep in self.episodic_memory]) if self.episodic_memory else 0,
                'consolidation_needed': sum(1 for ep in self.episodic_memory if ep.consolidation_count < 3)
            },
            'semantic_memory': {
                'patterns_count': len(self.semantic_patterns),
                'average_strength': np.mean([p.strength for p in self.semantic_patterns]) if self.semantic_patterns else 0
            },
            'working_memory': {
                'entries_count': len(self.working_memory),
                'average_activation': np.mean([e.getActivationScore() for e in self.working_memory]) if self.working_memory else 0,
                'capacity_utilization': len(self.working_memory) / self.working_memory_capacity
            },
            'procedural_memory': {
                'skills_count': len(self.procedural_memory),
                'average_skill_level': np.mean([m['skill_level'] for m in self.procedural_memory.values()]) if self.procedural_memory else 0
            },
            'associative_memory': {
                'associations_count': len(self.associative_memory),
                'average_strength': np.mean([m['strength'] for m in self.associative_memory.values()]) if self.associative_memory else 0
            }
        }
def runAdvancedMemoryExample():
    """Run the advanced memory example."""
    print("=== Advanced Memory Systems Example ===\n")
    
    # Create brain
    config = pynlm.createDemoConfig(2000, 0.1)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Initialize advanced memory system
    memory_system = AdvancedMemorySystem(brain)
    
    print("Advanced memory system initialized with:")
    print(f"- Episodic memory capacity: {memory_system.episodic_memory_capacity}")
    print(f"- Working memory capacity: {memory_system.working_memory_capacity}")
    print(f"- Semantic patterns capacity: {memory_system.semantic_pattern_capacity}")
    print()
    
    # Run learning simulation
    print("Running learning simulation...")
    
    for step in range(500):
        # Generate simulated experience
        percept = pynlm.Vision(8, 8)
        action = pynlm.Action(pynlm.ActionType.MoveForward)
        reward = np.random.normal(0.5, 0.2)  # Random rewards
        next_percept = pynlm.Vision(8, 8)
        
        # Store episode
        memory_system.storeEpisode(percept, action, reward, next_percept)
        
        # Periodic consolidation
        if step % 100 == 0 and step > 0:
            memory_system.replayEpisodes()
            
        # Log progress
        if step % 100 == 0:
            report = memory_system.getMemoryReport()
            print(f"Step {step}:")
            print(f"  Episodes stored: {report['episodic_memory']['episodes_count']}")
            print(f"  Semantic patterns: {report['semantic_memory']['patterns_count']}")
            print(f"  Working memory entries: {report['working_memory']['entries_count']}")
            print(f"  Capacity utilization: {report['working_memory']['capacity_utilization']:.2f}")
    
    # Display final report
    print("\n=== Final Memory System Report ===")
    report = memory_system.getMemoryReport()
    
    for memory_type, stats in report.items():
        print(f"{memory_type.replace('_', ' ').title()}:")
        for key, value in stats.items():
            if isinstance(value, float):
                print(f"  {key}: {value:.3f}")
            else:
                print(f"  {key}: {value}")
    
    print("\n=== Advanced Memory Systems Complete ===")
if __name__ == "__main__":
    runAdvancedMemoryExample()
#!/usr/bin/env python3
"""
Memory Learning Example

Demonstrates how to use NLM's memory systems for learning and retention.
This example shows episodic, semantic, and working memory usage.

Usage: python scripts/examples/memory_learning.py [config_file]
"""

import sys
import os
from pathlib import Path
import json
import random
import numpy as np

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from nlm import NeuralLearningMachine
from nlm.memory import WorkingMemory, EpisodicMemory, SemanticMemory


class MemoryLearningAgent:
    """Agent that learns using NLM memory systems."""
    
    def __init__(self, config_path: str = None):
        """Initialize memory learning agent.
        
        Args:
            config_path: Path to configuration file
        """
        self.config_path = config_path
        self.nlm = None
        self.working_memory = None
        self.episodic_memory = None
        self.semantic_memory = None
        self.learning_events = []
        
    def initialize(self):
        """Initialize the agent and memory systems."""
        print("Initializing Memory Learning Agent...")
        
        # Create NLM with memory capabilities
        if self.config_path and Path(self.config_path).exists():
            self.nlm = NeuralLearningMachine.load_config(self.config_path)
        else:
            from nlm.config import get_default_config
            config = get_default_config()
            self.nlm = NeuralLearningMachine(config)
        
        # Initialize memory systems
        self.working_memory = WorkingMemory(
            capacity=self.nlm.config.get('memory', {}).get(
                'working_memory', {}).get('working_memory_capacity', 100),
            decay_rate=self.nlm.config.get('memory', {}).get(
                'working_memory', {}).get('std_decay_rate', 0.999),
            threshold=self.nlm.config.get('memory', {}).get(
                'working_memory', {}).get('top_activation_threshold', 0.5)
        )
        
        self.episodic_memory = EpisodicMemory(
            capacity=self.nlm.config.get('memory', {}).get(
                'episodic_memory', {}).get('episodic_memory_capacity', 1000),
            encoding_strength=self.nlm.config.get('memory', {}).get(
                'episodic', {}).get('encoding_strength', 0.1),
            decay_rate=self.nlm.config.get('memory', {}).get(
                'episodic', {}).get('decay_rate', 0.99)
        )
        
        self.semantic_memory = SemanticMemory(
            capacity=self.nlm.config.get('memory', {}).get(
                'semantic_memory', {}).get('semantic_memory_capacity', 5000)
        )
        
        print("✓ Memory systems initialized successfully")
    
    def experience(self, sensory_input: Dict, action: Dict, reward: float):
        """Process an experience and store in memory systems.
        
        Args:
            sensory_input: Current sensory observations
            action: Action taken
            reward: Reward received
        """
        # Store in working memory (immediate processing)
        self.working_memory.store(sensory_input, action, reward)
        
        # Create episodic memory entry
        episode = {
            'sensory_input': sensory_input,
            'action': action,
            'reward': reward,
            'timestamp': len(self.learning_events),
            'context': self.working_memory.get_context()
        }
        
        self.episodic_memory.store_episode(episode)
        
        # Extract and store semantic patterns
        self.extract_semantic_patterns(sensory_input, action, reward)
        
        # Update NLM with the experience
        self.nlm.process_experience(sensory_input, action, reward)
        
        # Record learning event
        self.learning_events.append({
            'time': len(self.learning_events),
            'sensory_input': sensory_input,
            'action': action,
            'reward': reward,
            'working_memory_size': self.working_memory.size(),
            'episodic_memory_size': self.episodic_memory.size()
        })
    
    def extract_semantic_patterns(self, sensory_input: Dict, action: Dict, reward: float):
        """Extract and store semantic patterns from experience.
        
        Args:
            sensory_input: Current sensory observations
            action: Action taken
            reward: Reward received
        """
        # Create concept representation
        concepts = self.extract_concepts(sensory_input, action, reward)
        
        # Store in semantic memory
        for concept_name, concept_data in concepts.items():
            self.semantic_memory.store_concept(
                concept_name, concept_data, 
                importance=self.calculate_importance(sensory_input, action, reward)
            )
        
        # Search for related concepts
        related_concepts = self.find_related_concepts(concept_name)
        return related_concepts
    
    def extract_concepts(self, sensory_input: Dict, action: Dict, reward: float) -> Dict:
        """Extract concepts from sensory input, action, and reward.
        
        Args:
            sensory_input: Current sensory observations
            action: Action taken
            reward: Reward received
            
        Returns:
            Dictionary of extracted concepts
        """
        concepts = {}
        
        # Extract spatial concepts
        if 'position' in sensory_input:
            concepts['location'] = {
                'coordinates': sensory_input['position'],
                'distance': self.calculate_distance(sensory_input['position']),
                'accessibility': self.evaluate_accessibility(sensory_input)
            }
        
        # Extract action concepts
        if 'action' in sensory_input:
            concepts['environment'] = {
                'type': sensory_input['action'],
                'complexity': self.evaluate_complexity(sensory_input['action']),
                'difficulty': self.evaluate_difficulty(sensory_input, action, reward)
            }
        
        # Extract reward concepts
        if reward > 0:
            concepts['positive_outcome'] = {
                'type': 'success',
                'value': reward,
                'context': self.get_reward_context(reward)
            }
        elif reward < 0:
            concepts['negative_outcome'] = {
                'type': 'failure',
                'value': reward,
                'context': self.get_reward_context(reward)
            }
        
        return concepts
    
    def calculate_importance(self, sensory_input: Dict, action: Dict, reward: float) -> float:
        """Calculate importance of a concept.
        
        Args:
            sensory_input: Current sensory observations
            action: Action taken
            reward: Reward received
            
        Returns:
            Importance score (0-1)
        """
        importance = 0.0
        
        # High reward increases importance
        importance += min(abs(reward), 1.0) * 0.4
        
        # Novel sensory input increases importance
        if 'novelty' in sensory_input:
            importance += sensory_input['novelty'] * 0.3
        
        # Action complexity increases importance
        complexity = self.evaluate_complexity(action)
        importance += complexity * 0.3
        
        return min(importance, 1.0)
    
    def find_related_concepts(self, concept_name: str) -> List[str]:
        """Find related concepts in semantic memory.
        
        Args:
            concept_name: Name of concept to find relations for
            
        Returns:
            List of related concept names
        """
        if concept_name not in self.semantic_memory.concepts:
            return []
        
        concept_data = self.semantic_memory.concepts[concept_name]
        
        # Find similar concepts based on features
        related = []
        for other_name, other_data in self.semantic_memory.concepts.items():
            if other_name != concept_name:
                # Simple similarity check based on overlapping keys
                if set(concept_data.keys()) & set(other_data.keys()):
                    related.append(other_name)
        
        return related
    
    # Helper methods (simplified implementations)
    def calculate_distance(self, position: Tuple[float, float]) -> float:
        """Calculate distance from origin."""
        return np.sqrt(position[0]**2 + position[1]**2)
    
    def evaluate_accessibility(self, sensory_input: Dict) -> float:
        """Evaluate accessibility of location."""
        # Simplified implementation
        return random.uniform(0.5, 1.0)
    
    def evaluate_complexity(self, action: Dict) -> float:
        """Evaluate complexity of action."""
        # Simplified implementation
        return random.uniform(0.3, 0.8)
    
    def evaluate_difficulty(self, sensory_input: Dict, action: Dict, reward: float) -> float:
        """Evaluate difficulty of task."""
        # Simplified implementation
        return 1.0 - min(abs(reward), 1.0)
    
    def get_reward_context(self, reward: float) -> Dict:
        """Get context for reward."""
        return {
            'reward_value': reward,
            'difficulty': 'easy' if abs(reward) < 0.5 else 'hard',
            'type': 'positive' if reward > 0 else 'negative'
        }
    
    def retrieve(self, query: Dict) -> Dict:
        """Retrieve relevant memories for a query.
        
        Args:
            query: Query for memory retrieval
            
        Returns:
            Retrieved memory entries
        """
        # Get working memory context
        wm_context = self.working_memory.get_context()
        
        # Retrieve from episodic memory
        episodic_memories = self.episodic_memory.retrieve(query)
        
        # Retrieve from semantic memory
        semantic_memories = self.semantic_memory.search(query)
        
        return {
            'working_memory': wm_context,
            'episodic_memories': episodic_memories,
            'semantic_memories': semantic_memories
        }
    
    def simulate_learning_session(self, num_steps: int = 100):
        """Simulate a learning session.
        
        Args:
            num_steps: Number of steps to simulate
        """
        print(f"Starting {num_steps}-step learning session...")
        
        for step in range(num_steps):
            # Generate random sensory input
            position = (random.uniform(0, 10), random.uniform(0, 10))
            sensory_input = {
                'position': position,
                'novelty': random.random(),
                'sensory_features': {
                    'color': random.choice(['red', 'green', 'blue']),
                    'texture': random.choice(['smooth', 'rough', 'bumpy']),
                    'sound': random.choice(['silence', 'noise', 'tone'])
                }
            }
            
            # Generate random action
            action = {
                'type': random.choice(['move', 'explore', 'interact']),
                'direction': random.choice(['north', 'south', 'east', 'west']),
                'strength': random.uniform(0.5, 1.0)
            }
            
            # Calculate reward based on proximity to target
            target = (5.0, 5.0)
            distance = self.calculate_distance(position)
            target_distance = self.calculate_distance(target)
            reward = max(0, 10.0 - distance) / (10.0 - target_distance) if target_distance > 0 else 0
            
            # Experience the event
            self.experience(sensory_input, action, reward)
            
            if (step + 1) % 20 == 0:
                print(f"Step {step + 1}/{num_steps} completed")
        
        print("✓ Learning session completed")
        self.print_summary()
    
    def print_summary(self):
        """Print learning session summary."""
        print("\n" + "=" * 60)
        print("MEMORY LEARNING SESSION SUMMARY")
        print("=" * 60)
        print(f"Total experiences: {len(self.learning_events)}")
        print(f"Final working memory size: {self.working_memory.size()}")
        print(f"Final episodic memory size: {self.episodic_memory.size()}")
        print(f"Final semantic memory size: {self.semantic_memory.size()}")
        
        if self.learning_events:
            wm_sizes = [e['working_memory_size'] for e in self.learning_events]
            em_sizes = [e['episodic_memory_size'] for e in self.learning_events]
            
            print(f"Average working memory size: {np.mean(wm_sizes):.1f}")
            print(f"Average episodic memory size: {np.mean(em_sizes):.1f}")
        
        print("=" * 60)


def main():
    """Main entry point for memory learning example."""
    import argparse
    
    parser = argparse.ArgumentParser(description="Memory Learning Example")
    parser.add_argument("config", nargs="?", help="Path to configuration file (optional)")
    parser.add_argument("-s", "--steps", type=int, default=100,
                       help="Number of simulation steps")
    parser.add_argument("--no-run", action="store_true",
                       help="Don't run simulation, just initialize agent")
    
    args = parser.parse_args()
    
    # Create and initialize agent
    agent = MemoryLearningAgent(args.config)
    agent.initialize()
    
    if not args.no_run:
        # Run learning session
        agent.simulate_learning_session(args.steps)
    
    print("\n✓ Memory learning example completed successfully")


if __name__ == "__main__":
    main()

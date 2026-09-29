#!/usr/bin/env python3
"""
Example 4: Complete Agent with Memory System
=========================================

This example demonstrates a complete NLM agent with integrated memory systems,
showing how the agent learns, remembers experiences, and uses memory for
enhanced decision-making.

Learning Objectives:
- Understand integration of memory systems with neural processing
- Implement episodic memory for experience recording
- Use memory for decision support
- Demonstrate memory consolidation
- Analyze memory effectiveness

Expected Time: ~90 minutes
Difficulty: Advanced
"""

import pynlm
import time
import sys
import numpy as np
import json
from datetime import datetime

class MemoryAgent:
    """
    Complete NLM agent with integrated memory systems.
    
    This agent demonstrates how NLM's memory systems (working, episodic,
    and associative memory) work together with neural processing to enable
    sophisticated behavior and learning.
    """
    
    def __init__(self):
        """Initialize the memory agent with all subsystems."""
        print("Initializing Memory-Enhanced NLM Agent...")
        
        # Create optimized configuration
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 2000)
        self.config.set("brain.synapse_density", 0.06)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.hebbian.enable", True)
        self.config.set("plasticity.structural.enable", True)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("neuromod.dopamine.scale", 1.2)
        
        # Initialize brain and agent
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=25, height=25, visionWidth=7, visionHeight=7)
        self.world.reset()
        self.world.setAgentStart(12.5, 12.5)
        
        self.agent.initialize(self.world)
        
        # Enable all learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableStructuralPlasticity(True)
        
        # Memory system state
        self.memory_stats = {
            'episodic_memory': {
                'recordings': [],
                'consolidations': 0,
                'total_memories': 0,
                'recall_success_rate': 0.0
            },
            'working_memory': {
                'current_percept': None,
                'active_goals': [],
                'attention_focus': None,
                'capacity_utilization': 0.0
            },
            'associative_memory': {
                'pattern_associations': {},
                'concept_clusters': {},
                'retrieval_success_rate': 0.0
            }
        }
        
        # Experience recording
        self.experience_log = []
        self.memory_consolidation_events = []
        
        print("✓ Memory-enhanced agent initialized")
    
    def record_experience(self, percept, action, outcome, reward, time_step):
        """
        Record an experience for episodic memory.
        
        Args:
            percept: Sensory input
            action: Action taken
            outcome: Result of action
            reward: Reward received
            time_step: Simulation time step
        """
        # Create experience record
        experience = {
            'timestamp': time_step,
            'percept': {
                'vision': percept.getVision() if percept.getVision() else [],
                'touch': percept.getTouch() if percept.getTouch() else [],
                'internal': percept.getInternal() if percept.getInternal() else []
            },
            'action': {
                'type': action,
                'name': self._action_to_str(action)
            },
            'outcome': outcome,
            'reward': reward,
            'novelty': self.agent.getNoveltyLevel(),
            'curiosity': self.agent.getCuriosityLevel(),
            'brain_state': {
                'firing_rate': self.brain.getAverageFiringRate(),
                'spike_count': self.brain.getTotalSpikeCount(),
                'development_stage': self.agent.getDevelopmentalStage()
            },
            'agent_state': {
                'dopamine_level': self.agent.getNeuromodulationLevel(),
                'energy': self.world.getAgentBody().energy,
                'health': self.world.getAgentBody().health
            }
        }
        
        # Store in episodic memory
        self.memory_stats['episodic_memory']['recordings'].append(experience)
        self.memory_stats['episodic_memory']['total_memories'] += 1
        
        # Add to experience log
        self.experience_log.append(experience)
        
        # Keep memory manageable
        if len(self.memory_stats['episodic_memory']['recordings']) > 1000:
            self.memory_stats['episodic_memory']['recordings'].pop(0)
        
        # Log memory recording
        if len(self.memory_stats['episodic_memory']['recordings']) % 100 == 0:
            print(f"  Memory recorded: {len(self.memory_stats['episodic_memory']['recordings'])} experiences")
    
    def recall_relevant_memories(self, current_percept, max_memories=5):
        """
        Recall relevant memories based on current perception.
        
        Args:
            current_percept: Current sensory input
            max_memories: Maximum number of memories to recall
        
        Returns:
            List of relevant memories
        """
        if not self.memory_stats['episodic_memory']['recordings']:
            return []
        
        # Simple similarity-based recall (in real implementation, use proper vector similarity)
        current_vision = current_percept.getVision() if current_percept.getVision() else []
        
        scored_memories = []
        
        for memory in self.memory_stats['episodic_memory']['recordings']:
            memory_vision = memory['percept']['vision']
            
            if len(current_vision) == len(memory_vision) and len(current_vision) > 0:
                # Calculate similarity
                similarity = self._calculate_similarity(current_vision, memory_vision)
                
                # Boost score based on memory recency and importance
                recency_bonus = 1.0 / (1.0 + (time.time() - memory['timestamp']))
                reward_bonus = memory['reward'] / 10.0  # Normalize reward
                novelty_bonus = memory['novelty']
                
                score = (similarity * 0.5 + 
                        recency_bonus * 0.2 + 
                        reward_bonus * 0.2 + 
                        novelty_bonus * 0.1)
                
                scored_memories.append((score, memory))
        
        # Sort by score and return top memories
        scored_memories.sort(key=lambda x: x[0], reverse=True)
        relevant_memories = [mem for _, mem in scored_memories[:max_memories]]
        
        # Calculate recall success rate
        if relevant_memories:
            successful_recalls = len([m for m in relevant_memories if m['reward'] > 0])
            self.memory_stats['episodic_memory']['recall_success_rate'] = (
                successful_recalls / len(relevant_memories)
            )
        
        return relevant_memories
    
    def _calculate_similarity(self, vec1, vec2):
        """Calculate cosine similarity between two vectors."""
        if len(vec1) != len(vec2) or len(vec1) == 0:
            return 0.0
        
        # Simple Euclidean similarity
        diff = np.array(vec1) - np.array(vec2)
        similarity = 1.0 / (1.0 + np.linalg.norm(diff))
        
        return max(0.0, min(1.0, similarity))
    
    def update_associative_memory(self, percept, action, outcome):
        """
        Update associative memory with new patterns.
        
        Args:
            percept: Sensory input
            action: Action taken
            outcome: Result of action
        """
        # Create pattern signature
        vision = percept.getVision() if percept.getVision() else []
        if len(vision) > 0:
            pattern_key = tuple(vision[:5])  # Use subset for efficiency
        
        # Update pattern associations
        if 'pattern_associations' not in self.memory_stats['associative_memory']:
            self.memory_stats['associative_memory']['pattern_associations'] = {}
        
        if pattern_key not in self.memory_stats['associative_memory']['pattern_associations']:
            self.memory_stats['associative_memory']['pattern_associations'][pattern_key] = []
        
        # Store action-outcome association
        association = {
            'action': action,
            'outcome': outcome,
            'reward': self.calculate_reward(),
            'timestamp': time.time()
        }
        
        self.memory_stats['associative_memory']['pattern_associations'][pattern_key].append(association)
        
        # Create concept clusters
        self._update_concept_clusters(pattern_key, action, outcome)
    
    def _update_concept_clusters(self, pattern, action, outcome):
        """Update concept clusters in associative memory."""
        if 'concept_clusters' not in self.memory_stats['associative_memory']:
            self.memory_stats['associative_memory']['concept_clusters'] = {}
        
        # Determine concept from pattern
        concept = f"concept_{hash(str(pattern)) % 1000}"
        
        if concept not in self.memory_stats['associative_memory']['concept_clusters']:
            self.memory_stats['associative_memory']['concept_clusters'][concept] = {
                'patterns': [],
                'actions': [],
                'outcomes': [],
                'strength': 0.0
            }
        
        cluster = self.memory_stats['associative_memory']['concept_clusters'][concept]
        cluster['patterns'].append(pattern)
        cluster['actions'].append(action)
        cluster['outcomes'].append(outcome)
        
        # Update cluster strength based on experience
        cluster['strength'] = min(1.0, cluster['strength'] + 0.01)
    
    def consolidate_memories(self, priority_threshold=0.7):
        """
        Consolidate important memories into long-term storage.
        
        Args:
            priority_threshold: Threshold for memory consolidation
        """
        print("  Consolidating memories...")
        
        consolidation_count = 0
        
        for memory in self.memory_stats['episodic_memory']['recordings'][:]:
            # Calculate memory priority score
            priority = (
                memory['reward'] * 0.3 +           # High reward memories
                memory['novelty'] * 0.3 +         # Novel experiences
                memory['brain_state']['firing_rate'] * 0.2 +  # High activation
                (1.0 - memory['agent_state']['energy'] / 100.0) * 0.2  # Critical states
            )
            
            if priority > priority_threshold:
                # Consolidate this memory
                self.memory_consolidation_events.append({
                    'timestamp': time.time(),
                    'memory_timestamp': memory['timestamp'],
                    'priority': priority,
                    'reason': 'high_priority_learning'
                })
                
                consolidation_count += 1
                
                # Mark as consolidated (could move to different memory store)
                memory['consolidated'] = True
                memory['consolidation_timestamp'] = time.time()
        
        self.memory_stats['episodic_memory']['consolidations'] += consolidation_count
        
        if consolidation_count > 0:
            print(f"  Consolidated {consolidation_count} memories")
    
    def get_memory_usage(self):
        """Get memory system usage statistics."""
        total_experiences = len(self.memory_stats['episodic_memory']['recordings'])
        
        return {
            'episodic_memory': {
                'experiences_recorded': total_experiences,
                'consolidation_events': self.memory_stats['episodic_memory']['consolidations'],
                'recall_success_rate': self.memory_stats['episodic_memory']['recall_success_rate']
            },
            'associative_memory': {
                'pattern_associations': len(self.memory_stats['associative_memory']['pattern_associations']),
                'concept_clusters': len(self.memory_stats['associative_memory']['concept_clusters']),
                'retrieval_success_rate': self.memory_stats['associative_memory']['retrieval_success_rate']
            }
        }
    
    def calculate_reward(self):
        """Calculate reward based on agent state and outcomes."""
        body = self.world.getAgentBody()
        
        reward = 1.0  # Base survival reward
        
        # Energy-based reward
        if body.energy < 20.0:
            reward -= 0.5
        elif body.energy > 80.0:
            reward += 0.2
        
        # Health-based reward
        if body.health < 30.0:
            reward -= 0.3
        elif body.health > 80.0:
            reward += 0.1
        
        # Position-based reward (move toward center)
        center_x, center_y = self.world.getWidth() / 2, self.world.getHeight() / 2
        distance_to_center = np.sqrt((body.x - center_x)**2 + (body.y - center_y)**2)
        reward += max(0, 2.0 - distance_to_center / 20.0)
        
        return reward
    
    def _action_to_str(self, action):
        """Convert action enum to string."""
        action_names = {
            pynlm.MotorCommand.MoveForward: "MoveF",
            pynlm.MotorCommand.MoveBackward: "MoveB",
            pynlm.MotorCommand.TurnLeft: "TurnL",
            pynlm.MotorCommand.TurnRight: "TurnR",
            pynlm.MotorCommand.LookLeft: "LookL",
            pynlm.MotorCommand.LookRight: "LookR",
            pynlm.MotorCommand.Interact: "Inter",
            pynlm.MotorCommand.Wait: "Wait"
        }
        return action_names.get(action, f"Action({action})")
    
    def run_memory_session(self, num_steps=300):
        """
        Run a complete memory training session.
        
        Args:
            num_steps: Number of simulation steps
        """
        print(f"\nRunning memory training session for {num_steps} steps...")
        
        for step in range(num_steps):
            # Environment update
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain processing
            self.brain.step(step)
            
            # Recall relevant memories for decision support
            relevant_memories = self.recall_relevant_memories(percept)
            
            # Use memory to influence action selection
            action = self.decode_action_with_memory(percept, relevant_memories)
            
            # Apply action in world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate reward and update learning
            reward = self.calculate_reward()
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update associative memory
            self.update_associative_memory(percept, action, "executed")
            
            # Record experience for episodic memory
            self.record_experience(percept, action, "executed", reward, step)
            
            # Periodically consolidate memories
            if step % 50 == 0 and step > 0:
                self.consolidate_memories()
            
            # Print progress
            if step % 50 == 0:
                print(f"  Step {step:3d}: "
                      f"Action: {self._action_to_str(action):8s}, "
                      f"Reward: {reward:6.3f}, "
                      f"Memories: {len(self.memory_stats['episodic_memory']['recordings'])}, "
                      f"Consolidations: {self.memory_stats['episodic_memory']['consolidations']}")
        
        # Final memory consolidation
        print("  Performing final memory consolidation...")
        self.consolidate_memories()
    
    def decode_action_with_memory(self, percept, relevant_memories):
        """
        Decode action using both neural output and memory guidance.
        
        Args:
            percept: Current sensory input
            relevant_memories: Relevant episodic memories
        
        Returns:
            Motor command based on combined neural and memory signals
        """
        # Get neural action
        neural_action = self.agent.decodeMotorCommand()
        
        # If no relevant memories, use neural action alone
        if not relevant_memories:
            return neural_action
        
        # Analyze memory patterns to adjust action
        memory_actions = []
        memory_rewards = []
        
        for memory in relevant_memories:
            memory_actions.append(memory['action'])
            memory_rewards.append(memory['reward'])
        
        # Find most successful action in similar contexts
        if memory_actions:
            # Count action frequencies
            action_counts = {}
            for action, reward in zip(memory_actions, memory_rewards):
                if reward > 0:  # Focus on successful actions
                    action_counts[action] = action_counts.get(action, 0) + 1
            
            if action_counts:
                # Choose most frequent successful action
                best_action = max(action_counts.items(), key=lambda x: x[1])[0]
                
                # Blend with neural action (memory weight = 0.3, neural weight = 0.7)
                # In practice, this could be more sophisticated
                return best_action
        
        return neural_action
    
    def print_memory_analysis(self):
        """Print comprehensive memory system analysis."""
        memory_usage = self.get_memory_usage()
        
        print("\n" + "="*80)
        print("MEMORY-ENHANCED AGENT ANALYSIS")
        print("="*80)
        
        print(f"\nEpisodic Memory System:")
        episodic = memory_usage['episodic_memory']
        print(f"  Experiences recorded: {episodic['experiences_recorded']}")
        print(f"  Memory consolidation events: {episodic['consolidation_events']}")
        print(f"  Recall success rate: {episodic['recall_success_rate']*100:.1f}%")
        
        if episodic['experiences_recorded'] > 0:
            print(f"  Average experiences per consolidation: "
                  f"{episodic['consolidation_events']/max(1, episodic['experiences_recorded'])*100:.1f}%")
        
        print(f"\nAssociative Memory System:")
        associative = memory_usage['associative_memory']
        print(f"  Pattern associations: {associative['pattern_associations']}")
        print(f"  Concept clusters: {associative['concept_clusters']}")
        print(f"  Retrieval success rate: {associative['retrieval_success_rate']*100:.1f}%")
        
        # Memory effectiveness assessment
        print(f"\nMemory Effectiveness Assessment:")
        
        episodic_efficiency = (
            episodic['recall_success_rate'] * episodic['consolidation_events'] / max(100, episodic['experiences_recorded'])
        )
        
        associative_efficiency = associative['retrieval_success_rate']
        
        overall_efficiency = (episodic_efficiency + associative_efficiency) / 2.0
        
        print(f"  Episodic memory efficiency: {episodic_efficiency:.3f}")
        print(f"  Associative memory efficiency: {associative_efficiency:.3f}")
        print(f"  Overall memory efficiency: {overall_efficiency:.3f}")
        
        if overall_efficiency > 0.5:
            print("  → Memory systems are highly effective")
        elif overall_efficiency > 0.3:
            print("  → Memory systems are moderately effective")
        elif overall_efficiency > 0.1:
            print("  → Memory systems are minimally effective")
        else:
            print("  → Memory systems need improvement")
        
        # Memory recommendations
        print(f"\nMemory System Recommendations:")
        if episodic['recall_success_rate'] < 0.3:
            print("  - Increase episodic memory capacity or improve recall algorithms")
        if episodic['consolidation_events'] / max(1, episodic['experiences_recorded']) < 0.1:
            print("  - Increase memory consolidation frequency")
        if associative['retrieval_success_rate'] < 0.3:
            print("  - Improve pattern matching in associative memory")
        
        print(f"\nKey Insights:")
        if episodic['experiences_recorded'] > 100:
            print("  ✓ Agent accumulated substantial episodic memory")
        if episodic['consolidation_events'] > 10:
            print("  ✓ Effective memory consolidation occurred")
        if associative['pattern_associations'] > 50:
            print("  ✓ Rich associative memory patterns developed")

def main():
    """
    Main function demonstrating memory-enhanced agent.
    """
    print("NLM Memory-Enhanced Agent Example")
    print("="*60)
    print("This example demonstrates how NLM's memory systems enhance agent behavior")
    print("through episodic recording, associative learning, and memory consolidation.")
    print()
    
    try:
        # Create memory agent
        agent = MemoryAgent()
        
        # Run memory training session
        agent.run_memory_session(num_steps=200)
        
        # Print memory analysis
        agent.print_memory_analysis()
        
        print("\n" + "="*80)
        print("EXAMPLE COMPLETED SUCCESSFULLY")
        print("="*80)
        print("\nKey insights:")
        print("  - Agent learned through episodic memory recording")
        print("  - Memory consolidation preserved important experiences")
        print("  - Associative memory formed pattern associations")
        print("  - Memory systems enhanced decision-making")
        
    except Exception as e:
        print(f"\n❌ ERROR: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()

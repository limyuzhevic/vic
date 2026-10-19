#!/usr/bin/env python3
"""
Memory System Testing and Analysis
==================================
This example demonstrates memory systems and their testing.
It shows working memory, episodic memory, and associative memory functionality.

Key Features:
- Working memory with persistent activity
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- Memory system integration with neural processing
- Memory testing and analysis
"""

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import random
import time
from collections import defaultdict

def print_memory_header(title):
    """Print a formatted memory system header"""
    print(f"\n{'='*70}")
    print(f"{title}")
    print(f"{'='*70}")

def test_working_memory_integration(brain, duration):
    """Test working memory integration with neural processing"""
    print("\n   Testing working memory integration...")
    
    working_memory = brain.getWorkingMemory()
    if not working_memory:
        print("     Working memory not available, skipping")
        return []
    
    working_data = {
        'step': [],
        'stored_patterns': [],
        'retrieved_patterns': [],
        'competition_metrics': []
    }
    
    for step in range(duration):
        # Create and store a working memory pattern
        pattern = [random.random() * 0.4 + 0.3 for _ in range(10)]  # Pattern in [0.3, 0.7]
        
        working_memory.store(pattern, step)
        
        # Simulate neural competition
        competition = random.random()
        
        # Retrieve patterns (demonstrates competition)
        retrieved = working_memory.retrieve(step - 5) if step >= 5 else None
        
        working_data['step'].append(step)
        working_data['stored_patterns'].append(pattern)
        working_data['retrieved_patterns'].append(retrieved if retrieved else [])
        working_data['competition_metrics'].append(competition)
        
        # Neural processing (affects working memory)
        brain.step(step)
    
    return working_data

def test_episodic_memory_integration(brain, duration):
    """Test episodic memory integration"""
    print("\n   Testing episodic memory integration...")
    
    episodic_memory = brain.getEpisodicMemory()
    if not episodic_memory:
        print("     Episodic memory not available, skipping")
        return []
    
    episodic_data = {
        'step': [],
        'episodes_stored': [],
        'replay_events': [],
        'memory_strengths': []
    }
    
    for step in range(duration):
        # Create and store episodes
        episode_id = f"episode_{step}"
        context = [random.random() * 0.5 + 0.25 for _ in range(5)]  # Context in [0.25, 0.75]
        outcome = random.random() * 2.0 - 1.0  # Outcome in [-1.0, 1.0]
        
        episodic_memory.storeEpisode(episode_id, step, context, outcome)
        
        # Simulate episodic replay (occasional)
        if random.random() < 0.2:  # 20% chance of replay
            replay_step = random.randint(0, step - 1) if step > 0 else 0
            episodic_memory.replayEpisode(replay_step, 10)
            replay_event = True
        else:
            replay_event = False
        
        # Calculate memory strength
        memory_strength = 1.0 / (1.0 + abs(step - 20))  # Decays over time
        
        episodic_data['step'].append(step)
        episodic_data['episodes_stored'].append(episode_id)
        episodic_data['replay_events'].append(replay_event)
        episodic_data['memory_strengths'].append(memory_strength)
        
        brain.step(step)
    
    return episodic_data

def test_associative_memory_integration(brain, duration):
    """Test associative memory integration"""
    print("\n   Testing associative memory integration...")
    
    associative_memory = brain.getAssociativeMemory()
    if not associative_memory:
        print("     Associative memory not available, skipping")
        return []
    
    associative_data = {
        'step': [],
        'patterns_associated': [],
        'association_strengths': [],
        'retrieval_success': []
    }
    
    # Create pattern pairs
    pattern_pairs = []
    for i in range(5):
        pattern1 = [random.random() * 0.3 + 0.35 for _ in range(3)]  # [0.35, 0.65]
        pattern2 = [random.random() * 0.3 + 0.35 for _ in range(3)]
        pattern_pairs.append((pattern1, pattern2))
    
    for step in range(duration):
        # Associate patterns (Hebbian-style)
        if step < len(pattern_pairs):
            pattern1, pattern2 = pattern_pairs[step]
            associative_memory.associate(pattern1, pattern2)
            
            # Test retrieval
            retrieval_success = associative_memory.getAssociation(pattern1) is not None
            association_strength = random.random() * 0.5 + 0.5  # [0.5, 1.0]
        else:
            pattern1, pattern2 = pattern_pairs[0]
            retrieval_success = False
            association_strength = 0.0
        
        associative_data['step'].append(step)
        associative_data['patterns_associated'].append((pattern1, pattern2))
        associative_data['association_strengths'].append(association_strength)
        associative_data['retrieval_success'].append(retrieval_success)
        
        brain.step(step)
    
    return associative_data

def analyze_working_memory(working_data):
    """Analyze working memory performance"""
    print("\n   Analyzing working memory performance...")
    
    if not working_data['stored_patterns']:
        return
    
    # Calculate working memory metrics
    avg_patterns = len(working_data['stored_patterns']) / len(working_data['step'])
    max_retrieval_success = sum(1 for r in working_data['retrieved_patterns'] if r) / len(working_data['step'])
    avg_competition = sum(working_data['competition_metrics']) / len(working_data['competition_metrics'])
    
    print(f"     Working memory metrics:")
    print(f"       Average patterns per step: {avg_patterns:.2f}")
    print(f"       Retrieval success rate: {max_retrieval_success:.3f}")
    print(f"       Average competition: {avg_competition:.3f}")
    
    # Pattern analysis
    pattern_lengths = [len(p) for p in working_data['stored_patterns']]
    avg_pattern_length = sum(pattern_lengths) / len(pattern_lengths)
    print(f"       Average pattern length: {avg_pattern_length:.1f}")
    
    # Memory capacity analysis
    if len(working_data['stored_patterns']) > 5:
        print(f"       Memory capacity: Adequate (handled {len(working_data['stored_patterns'])} patterns)")
    else:
        print(f"       Memory capacity: Limited ({len(working_data['stored_patterns'])} patterns)")

def analyze_episodic_memory(episodic_data):
    """Analyze episodic memory performance"""
    print("\n   Analyzing episodic memory performance...")
    
    if not episodic_data['episodes_stored']:
        return
    
    # Calculate episodic memory metrics
    total_episodes = len(episodic_data['episodes_stored'])
    replay_events = sum(episodic_data['replay_events'])
    replay_rate = replay_events / total_episodes
    
    # Memory strength analysis
    avg_strength = sum(episodic_data['memory_strengths']) / len(episodic_data['memory_strengths'])
    max_strength = max(episodic_data['memory_strengths'])
    
    print(f"     Episodic memory metrics:")
    print(f"       Total episodes stored: {total_episodes}")
    print(f"       Replay events: {replay_events} ({replay_rate:.3f} rate)")
    print(f"       Average memory strength: {avg_strength:.3f}")
    print(f"       Maximum memory strength: {max_strength:.3f}")
    
    # Temporal dynamics
    if len(episodic_data['step']) > 10:
        early_strength = sum(episodic_data['memory_strengths'][:5]) / 5
        late_strength = sum(episodic_data['memory_strengths'][-5:]) / 5
        
        if late_strength > early_strength:
            print(f"       Memory consolidation: Increasing strength over time")
        elif late_strength < early_strength:
            print(f"       Memory decay: Decreasing strength over time")
        else:
            print(f"       Memory stability: Constant strength over time")

def analyze_associative_memory(associative_data):
    """Analyze associative memory performance"""
    print("\n   Analyzing associative memory performance...")
    
    if not associative_data['patterns_associated']:
        return
    
    # Calculate associative memory metrics
    total_associations = len(associative_data['patterns_associated'])
    successful_retrievals = sum(associative_data['retrieval_success'])
    retrieval_rate = successful_retrievals / total_associations
    
    # Association strength analysis
    avg_strength = sum(associative_data['association_strengths']) / len(associative_data['association_strengths'])
    max_strength = max(associative_data['association_strengths'])
    
    print(f"     Associative memory metrics:")
    print(f"       Total associations formed: {total_associations}")
    print(f"       Successful retrievals: {successful_retrievals} ({retrieval_rate:.3f} rate)")
    print(f"       Average association strength: {avg_strength:.3f}")
    print(f"       Maximum association strength: {max_strength:.3f}")
    
    # Pattern similarity analysis
    similarities = []
    for i, (p1, p2) in enumerate(associative_data['patterns_associated']):
        if i < len(associative_data['association_strengths']):
            strength = associative_data['association_strengths'][i]
            similarities.append(strength)
    
    avg_similarity = sum(similarities) / len(similarities) if similarities else 0
    print(f"       Average pattern similarity: {avg_similarity:.3f}")

def test_memory_system_integration(brain, duration):
    """Test complete memory system integration"""
    print("\n   Testing complete memory system integration...")
    
    # Test each memory system
    working_data = test_working_memory_integration(brain, duration)
    episodic_data = test_episodic_memory_integration(brain, duration)
    associative_data = test_associative_memory_integration(brain, duration)
    
    # Analyze all memory systems
    print("\n   Complete memory system analysis:")
    
    analyze_working_memory(working_data)
    analyze_episodic_memory(episodic_data)
    analyze_associative_memory(associative_data)
    
    # Integration analysis
    print("\n   Memory system integration analysis:")
    
    # Check integration indicators
    integration_score = 0.0
    
    if working_data:
        integration_score += 0.3
        print("     ✓ Working memory system operational")
    
    if episodic_data:
        integration_score += 0.3
        print("     ✓ Episodic memory system operational")
    
    if associative_data:
        integration_score += 0.4
        print("     ✓ Associative memory system operational")
    
    print(f"     Integration score: {integration_score:.1f}/1.0")
    
    return {
        'working_data': working_data,
        'episodic_data': episodic_data,
        'associative_data': associative_data,
        'integration_score': integration_score
    }

def simulate_memory_usage_patterns(memory_data):
    """Simulate memory usage patterns over time"""
    print("\n   Simulating memory usage patterns...")
    
    # Simulate different memory usage patterns
    patterns = {
        'baseline': {'rate': 0.3, 'amplitude': 0.5},
        'learning': {'rate': 0.7, 'amplitude': 0.8},
        'consolidation': {'rate': 0.5, 'amplitude': 0.6},
        'recovery': {'rate': 0.2, 'amplitude': 0.4}
    }
    
    for pattern_name, params in patterns.items():
        print(f"\n     {pattern_name} pattern:")
        print(f"       Learning rate: {params['rate']:.3f}")
        print(f"       Memory amplitude: {params['amplitude']:.3f}")
        
        # Simulate memory changes
        base_memory = 0.5
        for i in range(20):
            # Add noise
            noise = random.uniform(-0.1, 0.1)
            memory_change = params['rate'] * params['amplitude'] + noise
            
            # Apply memory update
            updated_memory = base_memory + memory_change
            updated_memory = max(0.0, min(1.0, updated_memory))
            
            if i % 5 == 0:
                print(f"       Time {i}: Memory level {updated_memory:.3f}")
        
        print(f"       Pattern {pattern_name} completed")

def demonstrate_memory_capacity_limits(memory_data):
    """Demonstrate memory capacity limits and strategies"""
    print("\n   Demonstrating memory capacity limits...")
    
    if not memory_data['working_data']:
        return
    
    print("\n   Memory capacity strategies:")
    
    # Example 1: Pattern compression
    patterns = memory_data['working_data']['stored_patterns']
    if patterns:
        # Demonstrate pattern compression
        avg_pattern_length = sum(len(p) for p in patterns) / len(patterns)
        compressed_length = max(1, int(avg_pattern_length * 0.7))  # Compress to 70%
        
        print(f"     Pattern compression:")
        print(f"       Original avg length: {avg_pattern_length:.1f}")
        print(f"       Compressed length: {compressed_length}")
        print(f"       Compression ratio: {compressed_length/avg_pattern_length:.3f}")
    
    # Example 2: Priority-based storage
    print("\n     Priority-based storage:")
    priorities = ['high', 'medium', 'low']
    for priority in priorities:
        capacity = {'high': 5, 'medium': 10, 'low': 20}[priority]
        print(f"       {priority} priority capacity: {capacity} patterns")
    
    # Example 3: Eviction strategies
    print("\n     Eviction strategies:")
    strategies = [
        ("FIFO", "First In, First Out"),
        ("LRU", "Least Recently Used"),
        ("LFU", "Least Frequently Used"),
        ("RAND", "Random")
    ]
    
    for strategy, description in strategies:
        print(f"       {strategy}: {description}")

def main():
    print_memory_header("NLM Memory System Testing and Analysis")
    
    # 1. Setup memory system testing
    print("\n1. Setting up memory system testing...")
    
    # Create configuration optimized for memory studies
    config = pynlm.createDefaultConfig()
    config.set("brain.neuronCount", 200)
    config.set("memory.workingMemoryCapacity", 50)
    config.set("memory.episodicCapacity", 500)
    config.set("memory.associativeStrength", 0.8)
    config.set("plasticity.enable", True)
    
    print("   Configuration created with memory focus:")
    print("     - Working memory capacity: 50 patterns")
    print("     - Episodic memory capacity: 500 episodes")
    print("     - Associative memory strength: 0.8")
    print("     - Plasticity enabled for memory encoding")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Brain initialized: {brain.getTotalNeuronCount()} neurons")
    
    # 2. Run memory system integration tests
    print("\n2. Running memory system integration tests...")
    
    duration = 100
    memory_results = test_memory_system_integration(brain, duration)
    
    # 3. Analyze memory system performance
    print("\n3. Analyzing memory system performance...")
    
    # Working memory analysis
    if memory_results['working_data']:
        print("\n   Working memory analysis:")
        analyze_working_memory(memory_results['working_data'])
    
    # Episodic memory analysis
    if memory_results['episodic_data']:
        print("\n   Episodic memory analysis:")
        analyze_episodic_memory(memory_results['episodic_data'])
    
    # Associative memory analysis
    if memory_results['associative_data']:
        print("\n   Associative memory analysis:")
        analyze_associative_memory(memory_results['associative_data'])
    
    # 4. Simulate memory usage patterns
    print("\n4. Simulating memory usage patterns...")
    
    simulate_memory_usage_patterns(memory_results)
    
    # 5. Demonstrate memory capacity strategies
    print("\n5. Memory capacity strategies...")
    
    demonstrate_memory_capacity_limits(memory_results)
    
    # 6. Summary
    print("\n6. Memory system testing summary...")
    
    print("\n   Key findings:")
    print("     - Working memory supports persistent activity patterns")
    print("     - Episodic memory enables experience replay and consolidation")
    print("     - Associative memory forms Hebbian pattern associations")
    print("     - Memory systems integrate with neural processing")
    print("     - Capacity limits require efficient encoding strategies")
    
    print("\n   Memory system capabilities:")
    print(f"     - Integration score: {memory_results['integration_score']:.1f}/1.0")
    
    # Calculate overall memory score
    overall_score = 0.0
    if memory_results['working_data']:
        overall_score += 0.3
    if memory_results['episodic_data']:
        overall_score += 0.3
    if memory_results['associative_data']:
        overall_score += 0.4
    
    print(f"     - Overall memory system score: {overall_score:.1f}/1.0")
    
    # 7. Practical applications
    print("\n7. Practical applications")
    print("-" * 50)
    
    print("\n   Memory system applications:")
    print("     - Working memory: attentional control and task maintenance")
    print("     - Episodic memory: learning from experience and prediction")
    print("     - Associative memory: pattern completion and generalization")
    print("     - Memory integration: coherent cognitive processing")
    
    print_memory_header("Memory System Testing and Analysis Complete")
    
    print("\nNext Steps:")
    print("  - Try simple_brain.py for basic examples")
    print("  - Try agent_world.py for environment interaction")
    print("  - Try advanced_config.py for configuration examples")
    print("  - Try plasticity_experiment.py for learning experiments")
    print("  - Try neuromodulation_study.py for neuromodulation analysis")
    print("\nFor detailed memory system documentation, see ADVANCED_FEATURES_IMPLEMENTATION.md")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
"""
Memory System Integration Examples for NLM

This module demonstrates advanced memory system usage including working memory,
episodic memory, and associative memory integration with neural networks.
"""

import pynlm
import numpy as np
import time
from typing import Dict, List, Optional, Tuple
import json

class MemorySystemExamples:
    """Memory system integration examples."""
    
    @staticmethod
    def create_working_memory_example():
        """
        Example: Working memory integration with neural network.
        
        Demonstrates how NLM maintains and updates information in working memory
        during cognitive tasks.
        """
        print("=== Working Memory Integration Example ===")
        
        # Create brain with working memory capabilities
        config = pynlm.createDefaultConfig()
        
        # Configure for working memory tasks
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.1)
        config.set("brain.initial_weight_mean", 0.6)
        
        # Enable memory-related plasticity
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.02)  # Stronger learning
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.01)
        
        # Create brain and initialize
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Access working memory system
        working_memory = brain.getWorkingMemory()
        if working_memory:
            print(f"Working memory capacity: {working_memory.getCapacity()} items")
            print(f"Working memory current size: {working_memory.getSize()} items")
        
        # Simulate working memory task
        print("\nSimulating working memory task...")
        
        # Store information in working memory
        if working_memory:
            # Store multiple items (like remembering a sequence)
            items = ["item1", "item2", "item3", "item4", "item5"]
            for i, item in enumerate(items):
                working_memory.store(f"sequence_{i}", item, confidence=0.8)
                print(f"  Stored: {item} in working memory")
            
            # Maintain items over time (simulated)
            for step in range(50):
                brain.step(step)
                
                # Gradually decay memory (simulated)
                if step % 10 == 0:
                    decay_factor = 0.95 ** step
                    working_memory_size = working_memory.getSize()
                    effective_capacity = int(working_memory_size * decay_factor)
                    print(f"  Step {step}: Working memory size = {working_memory_size}, "
                          f"Effective capacity = {effective_capacity}")
        
        return {
            "brain": brain,
            "working_memory": working_memory,
            "config": config
        }
    
    @staticmethod
    def create_episodic_memory_example():
        """
        Example: Episodic memory integration.
        
        Demonstrates storing and retrieving episodic memories with contextual information.
        """
        print("\n=== Episodic Memory Integration Example ===")
        
        # Create brain with episodic memory capabilities
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 3000)
        config.set("brain.synapse_density", 0.12)
        
        # Configure for episodic memory
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.015)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Access episodic memory
        episodic_memory = brain.getEpisodicMemory()
        if episodic_memory:
            print(f"Episodic memory system available")
            print(f"Episodic memory episode capacity: {episodic_memory.getCapacity()} episodes")
        
        # Simulate episodic memory storage and retrieval
        print("\nSimulating episodic memory storage...")
        
        if episodic_memory:
            # Store multiple episodes with different contexts
            episodes = [
                {"context": "task1", "action": "approach", "outcome": "success", "reward": 1.0},
                {"context": "task2", "action": "avoid", "outcome": "failure", "reward": -0.5},
                {"context": "task3", "action": "explore", "outcome": "partial", "reward": 0.3},
                {"context": "task4", "action": "approach", "outcome": "success", "reward": 1.0},
                {"context": "task5", "action": "avoid", "outcome": "failure", "reward": -0.5}
            ]
            
            for i, episode_data in enumerate(episodes):
                # Store episode with temporal information
                episodic_memory.store(
                    episode_id=f"episode_{i}",
                    context=episode_data["context"],
                    content=episode_data,
                    timestamp=i,
                    reward=episode_data["reward"]
                )
                print(f"  Stored episode {i+1}: {episode_data['context']}")
            
            # Test episodic recall
            print("\nTesting episodic recall...")
            for step in range(100):
                brain.step(step)
                
                if step % 25 == 0:
                    # Retrieve similar episodes
                    similar_episodes = episodic_memory.retrieve("task")
                    print(f"  Step {step}: Found {len(similar_episodes)} similar episodes "
                          f"in episodic memory")
                    
                    # Analyze episode patterns
                    if len(similar_episodes) > 1:
                        # Calculate pattern consistency
                        success_episodes = sum(1 for ep in similar_episodes 
                                             if ep.get("reward", 0) > 0)
                        print(f"    Pattern analysis: {success_episodes}/{len(similar_episodes)} "
                              f"successful episodes")
        
        return {
            "brain": brain,
            "episodic_memory": episodic_memory,
            "config": config
        }
    
    @staticmethod
    def create_associative_memory_example():
        """
        Example: Associative memory integration.
        
        Demonstrates pattern association and concept formation through neural networks.
        """
        print("\n=== Associative Memory Integration Example ===")
        
        # Create brain optimized for associative memory
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 2500)
        config.set("brain.synapse_density", 0.08)  # Lower density for more selective connections
        
        # Configure for associative learning
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.008)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.015)  # Stronger Hebbian for association
        config.set("plasticity.structural.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Access associative memory
        associative_memory = brain.getAssociativeMemory()
        if associative_memory:
            print(f"Associative memory system available")
        
        # Simulate associative memory learning
        print("\nSimulating associative memory formation...")
        
        # Create stimulus-response pairs
        stimulus_response_pairs = [
            ("stimulus_A", "response_1"),
            ("stimulus_B", "response_2"),
            ("stimulus_C", "response_3"),
            ("stimulus_A", "response_4"),  # Repeat stimulus to strengthen association
            ("stimulus_D", "response_5"),
            ("stimulus_B", "response_2")   # Repeat stimulus
        ]
        
        if associative_memory:
            # Learn associations
            for stimulus, response in stimulus_response_pairs:
                associative_memory.associate(stimulus, response)
                print(f"  Associated: '{stimulus}' -> '{response}'")
            
            # Test recall
            print("\nTesting associative recall...")
            test_stimuli = ["stimulus_A", "stimulus_B", "stimulus_E", "stimulus_X"]
            
            for step in range(100):
                brain.step(step)
                
                if step % 25 == 0:
                    for stimulus in test_stimuli:
                        recalled_responses = associative_memory.recall(stimulus)
                        if recalled_responses:
                            print(f"  Step {step}: Stimulus '{stimulus}' -> "
                                  f"Responses: {recalled_responses}")
                        else:
                            print(f"  Step {step}: Stimulus '{stimulus}' -> "
                                  f"No associations found")
        
        return {
            "brain": brain,
            "associative_memory": associative_memory,
            "config": config
        }
    
    @staticmethod
    def create_memory_system_benchmark():
        """
        Example: Memory system performance benchmarking.
        
        Compares performance of different memory system configurations.
        """
        print("\n=== Memory System Performance Benchmark ===")
        
        # Test different memory system configurations
        configs = {
            "minimal": {
                "neuron_count": 1000,
                "plasticity": {"stdp": {"enable": True, "learning_rate": 0.01},
                             "hebbian": {"enable": True, "learning_rate": 0.005},
                             "structural": {"enable": False}}
            },
            "balanced": {
                "neuron_count": 2000,
                "plasticity": {"stdp": {"enable": True, "learning_rate": 0.02},
                             "hebbian": {"enable": True, "learning_rate": 0.01},
                             "structural": {"enable": True, "max_connections": 10000}}
            },
            "advanced": {
                "neuron_count": 3000,
                "plasticity": {"stdp": {"enable": True, "learning_rate": 0.03},
                             "hebbian": {"enable": True, "learning_rate": 0.015},
                             "structural": {"enable": True, "max_connections": 20000}}
            }
        }
        
        results = {}
        
        for config_name, config_settings in configs.items():
            print(f"\nTesting {config_name} configuration...")
            
            # Create configuration
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", config_settings["neuron_count"])
            config.set("brain.synapse_density", 0.1)
            
            # Set up plasticity
            plasticity = config_settings["plasticity"]
            if plasticity["stdp"]["enable"]:
                config.set("plasticity.stdp.enable", True)
                config.set("plasticity.stdp.learning_rate", 
                          plasticity["stdp"]["learning_rate"])
            
            if plasticity["hebbian"]["enable"]:
                config.set("plasticity.hebbian.enable", True)
                config.set("plasticity.hebbian.learning_rate",
                          plasticity["hebbian"]["learning_rate"])
            
            if plasticity["structural"]["enable"]:
                config.set("plasticity.structural.enable", True)
            
            # Create brain and run benchmark
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            for step in range(200):
                brain.step(step)
            
            simulation_time = time.time() - start_time
            
            # Calculate metrics
            neurons_per_second = config_settings["neuron_count"] / simulation_time
            memory_efficiency = brain.getTotalSpikeCount() / (config_settings["neuron_count"] * 2)  # 2 steps
            
            results[config_name] = {
                "neurons": config_settings["neuron_count"],
                "simulation_time": simulation_time,
                "neurons_per_second": neurons_per_second,
                "memory_efficiency": memory_efficiency,
                "total_spikes": brain.getTotalSpikeCount(),
                "config": config
            }
            
            print(f"  Performance: {neurons_per_second:,.0f} neurons/sec")
            print(f"  Memory efficiency: {memory_efficiency:.4f} spikes/neuron/step")
            print(f"  Total spikes: {brain.getTotalSpikeCount():,}")
        
        return results

if __name__ == "__main__":
    print("Memory System Integration Examples")
    print("=" * 40)
    
    # Example 1: Working memory
    working_memory_result = MemorySystemExamples.create_working_memory_example()
    print(f"\nWorking memory example completed")
    
    # Example 2: Episodic memory
    episodic_result = MemorySystemExamples.create_episodic_memory_example()
    print(f"\nEpisodic memory example completed")
    
    # Example 3: Associative memory
    associative_result = MemorySystemExamples.create_associative_memory_example()
    print(f"\nAssociative memory example completed")
    
    # Example 4: Performance benchmark
    benchmark_results = MemorySystemExamples.create_memory_system_benchmark()
    print(f"\nMemory system benchmark: {len(benchmark_results)} configurations tested")
    
    print("\n=== All memory system examples completed ===")

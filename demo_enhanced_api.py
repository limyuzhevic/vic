#!/usr/bin/env python3
"""
Demonstration script for NLM Enhanced Python API

This script demonstrates the key features of the enhanced NLM API,
showing how to use the new high-level APIs for simulation management,
memory management, performance profiling, and configuration.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

# Import the enhanced API
import pynlm_enhanced as pynlm

def main():
    """Run the demonstration."""
    print("=== NLM Enhanced Python API Demonstration ===\n")
    
    # Check if we have the C++ library
    try:
        import pynlm as pynlm_c
        print("✓ C++ NLM library is available")
        cpp_available = True
    except ImportError:
        print("✗ C++ NLM library not available - running demo with mock objects")
        cpp_available = False
        
        # Create mock objects for demonstration
        class MockBrain:
            def __init__(self):
                self.total_neurons = 1000
                self.total_spikes = 0
                self.firing_count = 0
                
            def initialize(self):
                pass
                
            def step(self, step):
                self.total_spikes += 1
                if step % 10 == 0:  # Some neurons fire occasionally
                    self.firing_count = 50
                
            def getTotalNeuronCount(self):
                return self.total_neurons
                
            def getTotalSpikeCount(self):
                return self.total_spikes
                
            def getFiringNeuronCount(self):
                return self.firing_count
                
            def getWorkingMemory(self):
                return MockMemoryManager()
                
            def getEpisodicMemory(self):
                return MockMemoryManager()
                
            def getAssociativeMemory(self):
                return MockMemoryManager()
                
            def getConfig(self):
                return MockConfig()
                
        class MockMemoryManager:
            def getActiveTraces(self):
                return 150
                
            def getTraceCapacity(self):
                return 1000
                
            def getCompetitionCount(self):
                return 5
                
            def getEpisodeCount(self):
                return 25
                
            def getMaxEpisodes(self):
                return 1000
                
            def getConsolidationThreshold(self):
                return 0.5
                
            def getAssociationCount(self):
                return 500
                
            def getSimilarityThreshold(self):
                return 0.8
                
            def consolidate(self, threshold):
                pass
                
            def clear(self):
                pass
                
        class MockConfig:
            def getOr(self, key, default):
                return default
                
        # Mock world
        class MockWorld:
            def configure(self, width, height, vision_width, vision_height):
                pass
                
            def reset(self):
                pass
                
            def update(self, timestep):
                pass
                
            def getSensoryPercept(self):
                return MockPercept()
                
        class MockPercept:
            def getVision(self):
                return [0.1] * 256
                
            def getTouch(self):
                return [0.0] * 8
                
            def getInternal(self):
                return [0.5] * 4
                
            def getProprioception(self):
                return [0.0] * 6
                
        # Mock agent
        class MockAgent:
            def __init__(self, brain):
                self.brain = brain
                
            def initialize(self, world):
                pass
                
            def processSensoryInput(self, percept):
                pass
                
            def decodeMotorCommand(self):
                return pynlm.ActionType.MoveForward
                
            def getNoveltyLevel(self):
                return 0.3
                
            def getCuriosityLevel(self):
                return 0.7
                
            def getBrain(self):
                return self.brain
                
        # Mock action
        class MockAction:
            def __init__(self, action_type):
                self.type = action_type
                
        # Mock action types
        class MockActionType:
            MoveForward = "MoveForward"
            TurnLeft = "TurnLeft"
            Interact = "Interact"
            Wait = "Wait"
        
        # Patch the pynlm module
        pynlm.ActionType = MockActionType()
        pynlm.WorldObject = MockAction  # Just for demo
    
    print("\n1. Simple Brain Simulation")
    print("-" * 40)
    
    if cpp_available:
        brain = pynlm.create_brain()
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
            if step % 25 == 0:
                print(f"Step {step:3d}: {brain.getTotalSpikeCount():4d} spikes, "
                      f"{brain.getFiringNeuronCount():2d} firing neurons")
    else:
        brain = MockBrain()
        brain.initialize()
        
        for step in range(100):
            brain.step(step)
            if step % 25 == 0:
                print(f"Step {step:3d}: {brain.getTotalSpikeCount():4d} spikes, "
                      f"{brain.getFiringNeuronCount():2d} firing neurons")
    
    print("\n2. Memory Management")
    print("-" * 40)
    
    if cpp_available:
        memory_manager = pynlm.MemoryManager(brain)
        stats = memory_manager.get_memory_stats()
        
        if stats['working_memory']:
            wm = stats['working_memory']
            print(f"Working Memory: {wm['active_traces']} active traces "
                  f"(capacity: {wm['trace_capacity']})")
        
        if stats['episodic_memory']:
            em = stats['episodic_memory']
            print(f"Episodic Memory: {em['episode_count']} episodes "
                  f"(max: {em['max_episodes']})")
        
        if stats['associative_memory']:
            am = stats['associative_memory']
            print(f"Associative Memory: {am['association_count']} associations "
                  f"(similarity threshold: {am['similarity_threshold']})")
        
        memory_manager.consolidate_memories()
        print("Memory consolidation completed")
    else:
        memory_manager = pynlm.MemoryManager(brain)
        stats = memory_manager.get_memory_stats()
        
        if stats['working_memory']:
            wm = stats['working_memory']
            print(f"Working Memory: {wm['active_traces']} active traces "
                  f"(capacity: {wm['trace_capacity']})")
        
        if stats['episodic_memory']:
            em = stats['episodic_memory']
            print(f"Episodic Memory: {em['episode_count']} episodes "
                  f"(max: {em['max_episodes']})")
        
        memory_manager.consolidate_memories()
        print("Memory consolidation completed")
    
    print("\n3. Performance Profiling")
    print("-" * 40)
    
    if cpp_available:
        profiler = pynlm.PerformanceProfiler(brain)
        profiler.start_profiling(sampling_rate=10.0)
        
        for step in range(100):
            brain.step(step)
            profiler.profile_step(step)
        
        profiler.stop_profiling()
        
        report = profiler.get_performance_report()
        print(f"Average time per step: {report['average_time_per_step']*1000:.2f} ms")
        print(f"Min time per step: {report['min_time_per_step']*1000:.2f} ms")
        print(f"Max time per step: {report['max_time_per_step']*1000:.2f} ms")
        print(f"Total spikes recorded: {report['spike_count']['total']}")
    else:
        print("Mock profiling - brain simulation completed")
    
    print("\n4. Advanced Configuration")
    print("-" * 40)
    
    config = pynlm.ConfigManager()
    
    # Set configuration values
    config.set("brain.neuron_count", 5000)
    config.set("plasticity.stdp_learning_rate", 0.01)
    config.set("neuromod.dopamine_scale", 2.0)
    
    # Validate configuration
    errors = config.validate_config()
    if not errors:
        print("✓ Configuration is valid!")
    else:
        print(f"✗ Validation errors: {errors}")
    
    print(f"Brain neuron count: {config.get('brain.neuron_count')}")
    print(f"Plasticity learning rate: {config.get('plasticity.stdp_learning_rate')}")
    print(f"Dopamine scale: {config.get('neuromod.dopamine_scale')}")
    
    # Save configuration
    config.save("demo_config.json")
    print("Configuration saved to demo_config.json")
    
    # Load configuration
    new_config = pynlm.ConfigManager()
    new_config.load("demo_config.json")
    print("Configuration loaded successfully")
    
    print("\n5. Experiment Management")
    print("-" * 40)
    
    if cpp_available:
        # Create and configure world
        world = pynlm.create_world(width=20, height=20, vision_width=8, vision_height=8)
        world.reset()
        
        # Create agent
        agent = pynlm.create_agent(brain)
        agent.initialize(world)
        
        # Enable learning features
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableDevelopment(True)
        
        print(f"Created agent with brain of {brain.getTotalNeuronCount()} neurons")
        print(f"Agent curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"Agent novelty level: {agent.getNoveltyLevel():.3f}")
        
        # Run a short experiment
        experiment = pynlm.Experiment("Demo Experiment", {
            'steps': 50,
            'timestep': 0.1,
            'enable_memory': True
        })
        
        results = experiment.run(brain, world, agent, steps=50, timestep=0.1)
        
        print(f"Experiment completed in {results['duration']:.2f} seconds")
        print(f"Steps completed: {results['steps_completed']}")
        print(f"Total spikes: {results['total_spikes']}")
        print(f"Average novelty: {results['average_novelty']:.3f}")
        print(f"Average curiosity: {results['average_curiosity']:.3f}")
        
        experiment.save_results("demo_experiment_results.json")
        print("Experiment results saved to demo_experiment_results.json")
    else:
        print("Mock experiment - world, agent, and experiment management demonstrated")
    
    print("\n6. Memory Replay and Consolidation")
    print("-" * 40)
    
    if cpp_available:
        # Show current memory stats
        current_stats = memory_manager.get_memory_stats()
        print("Before consolidation:")
        if current_stats['episodic_memory']:
            print(f"  Episodic episodes: {current_stats['episodic_memory']['episode_count']}")
        
        # Consolidate and replay
        memory_manager.consolidate_memories()
        memory_manager.replay_memories(episode_count=3)
        
        print("Memory consolidation and replay completed")
    else:
        print("Mock memory operations - consolidation and replay demonstrated")
    
    print("\n=== Demonstration Complete ===")
    print("\nKey features demonstrated:")
    print("✓ Simple brain simulation")
    print("✓ Memory management utilities")
    print("✓ Performance profiling and monitoring")
    print("✓ Advanced configuration system")
    print("✓ Experiment management and tracking")
    print("✓ Memory consolidation and replay")
    print("\nThe enhanced NLM API provides:")
    print("• High-level, Pythonic interfaces for complex operations")
    print("• Comprehensive error handling and validation")
    print("• Type hints for better IDE support")
    print("• Experiment tracking and result analysis")
    print("• Performance monitoring and profiling tools")
    print("• Advanced configuration management")
    print("• Memory system utilities and management")


if __name__ == "__main__":
    main()
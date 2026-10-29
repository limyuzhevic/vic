#!/usr/bin/env python3

"""
NLM Enhanced Examples

This script contains practical examples demonstrating the enhanced NLM Python interface
with integrated memory systems, prediction, and cognitive functions.
"""

import sys
import time
from pathlib import Path

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent))

import nlm

def example_basic_usage():
    """Example 1: Basic NLM brain simulation"""
    print("=== Example 1: Basic NLM Brain Simulation ===")
    
    # Create a brain with sensible defaults
    brain = nlm.create_demo_brain()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run simulation
    for step in range(50):
        brain.step(step, step * 0.001)
    
    # Check results
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print()

def example_enhanced_simulation():
    """Example 2: Enhanced simulation with integrated systems"""
    print("=== Example 2: Enhanced Simulation with Integrated Systems ===")
    
    if not nlm.ENHANCED_AVAILABLE:
        print("Enhanced bindings not available, skipping this example")
        print()
        return
    
    from nlm import PyBrainSimulation
    
    # Create enhanced simulation
    sim = PyBrainSimulation(steps=100)
    
    print("Running enhanced simulation with integrated systems...")
    
    # Run simulation
    sim.run()
    
    # Get comprehensive statistics
    stats = sim.getStats()
    memory_stats = sim.getMemoryStats()
    prediction_stats = sim.getPredictionStats()
    cognitive_stats = sim.getCognitiveStats()
    neuromod_stats = sim.getNeuromodulationStats()
    dev_stats = sim.getDevelopmentStats()
    
    print(f"Simulation completed successfully!")
    print(f"Step reached: {stats.get('step', 'N/A')}")
    print(f"Total neurons: {stats.get('total_neurons', 'N/A')}")
    print(f"Total spikes: {stats.get('total_spikes', 'N/A')}")
    print()
    
    # Show memory system status
    if memory_stats:
        print("Memory Systems Status:")
        print(f"  Working Memory: {memory_stats.get('working_memory_size', 'N/A')}/"
              f"{memory_stats.get('working_memory_capacity', 'N/A')}")
        print(f"  Episodic Memory: {memory_stats.get('episodic_memory_count', 'N/A')} episodes")
        if memory_stats.get('has_associative_memory', False):
            print(f"  Associative Memory: Available")
    print()
    
    # Show cognitive systems
    if cognitive_stats:
        print("Cognitive Systems Status:")
        if cognitive_stats.get('has_planner', False):
            print(f"  ✓ Neural Planner: Available")
        if cognitive_stats.get('has_concept_formation', False):
            print(f"  ✓ Concept Formation: Available")
        if cognitive_stats.get('has_attention', False):
            print(f"  ✓ Attention System: Available")
    print()
    
    # Show neuromodulation
    if neuromod_stats:
        print("Neuromodulation Status:")
        if neuromod_stats.get('has_dopamine', False):
            print(f"  ✓ Dopamine System: Available")
        if neuromod_stats.get('has_curiosity', False):
            print(f"  ✓ Curiosity Level: {neuromod_stats.get('curiosity_level', 'N/A')}")
        if neuromod_stats.get('has_prediction_error', False):
            print(f"  ✓ Prediction Error: Available")
    print()

def example_learning_experiment():
    """Example 3: Automated learning experiment"""
    print("=== Example 3: Automated Learning Experiment ===")
    
    if not nlm.ENHANCED_AVAILABLE:
        print("Enhanced bindings not available, using basic brain")
        brain = nlm.create_demo_brain()
        
        for step in range(100):
            brain.step(step, step * 0.001)
        
        print("Basic simulation completed (no integrated learning).")
        print()
        return
    
    from nlm import PyBrainSimulation
    
    # Create enhanced simulation
    sim = PyBrainSimulation(steps=200)
    
    print("Running learning experiment with integrated plasticity...")
    
    # Run simulation
    sim.run()
    
    # Run learning experiment
    learning_results = sim.runLearningExperiment()
    
    print("Learning Experiment Results:")
    print(f"  Learning Factor: {learning_results.get('learning_factor', 'N/A'):.6f}")
    print(f"  Learning Detected: {learning_results.get('learning_detected', 'N/A')}")
    print(f"  Initial Mean Weight: {learning_results.get('initial_mean_weight', 'N/A'):.6f}")
    print(f"  Final Mean Weight: {learning_results.get('final_mean_weight', 'N/A'):.6f}")
    print(f"  Weight Change: {learning_results.get('weight_change', 'N/A'):.6f}")
    print(f"  Initial Synapses: {learning_results.get('initial_synapses', 'N/A')}")
    print(f"  Final Synapses: {learning_results.get('final_synapses', 'N/A')}")
    print()

def example_save_load():
    """Example 4: Save and load brain states"""
    print("=== Example 4: Save and Load Brain States ===")
    
    if not nlm.ENHANCED_AVAILABLE:
        print("Enhanced bindings not available, skipping save/load example")
        print()
        return
    
    from nlm import PyBrainSimulation
    
    # Create and run simulation
    print("Creating and running simulation...")
    sim = PyBrainSimulation(steps=50)
    sim.run()
    
    # Save brain state
    save_file = "example_brain.nml"
    print(f"Saving brain state to {save_file}...")
    success = sim.save(save_file)
    
    if success:
        print("Save successful!")
        
        # Create new simulation and load
        print("Creating new simulation and loading saved state...")
        new_sim = PyBrainSimulation(steps=50)
        load_success = new_sim.load(save_file)
        
        if load_success:
            print("Load successful!")
            
            # Run loaded simulation
            print("Running loaded simulation...")
            new_sim.run()
            
            # Compare results
            old_stats = sim.getStats()
            new_stats = new_sim.getStats()
            
            print(f"Original simulation spike count: {old_stats.get('total_spikes', 'N/A')}")
            print(f"Loaded simulation spike count: {new_stats.get('total_spikes', 'N/A')}")
        else:
            print("Load failed!")
    else:
        print("Save failed!")
    
    # Clean up
    import os
    if os.path.exists(save_file):
        os.remove(save_file)
        print(f"Cleaned up {save_file}")
    print()

def example_batch_processing():
    """Example 5: Batch processing with different configurations"""
    print("=== Example 5: Batch Processing with Different Configurations ===")
    
    if not nlm.ENHANCED_AVAILABLE:
        print("Enhanced bindings not available, using basic simulation")
        
        configurations = [
            {"steps": 50, "description": "Small simulation"},
            {"steps": 100, "description": "Medium simulation"},
            {"steps": 200, "description": "Large simulation"},
        ]
        
        results = []
        for config in configurations:
            print(f"\nRunning {config['description']} ({config['steps']} steps)...")
            brain = nlm.create_demo_brain()
            
            start_time = time.time()
            for step in range(config['steps']):
                brain.step(step, step * 0.001)
            end_time = time.time()
            
            results.append({
                "configuration": config,
                "duration": end_time - start_time,
                "total_spikes": brain.getTotalSpikeCount(),
                "final_spike_rate": brain.getAverageFiringRate()
            })
        
        print(f"\nBatch Processing Results:")
        for result in results:
            config = result['configuration']
            print(f"  {config['description']}: "
                  f"{result['duration']:.2f}s, "
                  f"{result['total_spikes']} spikes, "
                  f"{result['final_spike_rate']:.3f} firing rate")
    
    else:
        from nlm import PyBrainSimulation
        
        configurations = [
            {"steps": 50, "description": "Small simulation"},
            {"steps": 100, "description": "Medium simulation"},
            {"steps": 200, "description": "Large simulation"},
        ]
        
        results = []
        for config in configurations:
            print(f"\nRunning {config['description']} ({config['steps']} steps)...")
            
            sim = PyBrainSimulation(steps=config['steps'])
            start_time = time.time()
            sim.run()
            end_time = time.time()
            
            results.append({
                "configuration": config,
                "duration": end_time - start_time,
                "stats": sim.getStats(),
                "learning": sim.runLearningExperiment()
            })
        
        print(f"\nBatch Processing Results:")
        for result in results:
            config = result['configuration']
            stats = result['stats']
            learning = result['learning']
            print(f"  {config['description']}: "
                  f"{result['duration']:.2f}s, "
                  f"{stats.get('total_spikes', 'N/A')} spikes, "
                  f"learning: {learning.get('learning_factor', 'N/A'):.6f}")
    
    print()

def example_agent_simulation():
    """Example 6: Agent-world simulation"""
    print("=== Example 6: Agent-World Simulation ===")
    
    if not nlm.ENHANCED_AVAILABLE:
        print("Enhanced bindings not available, demonstrating basic agent simulation")
        
        # Basic agent simulation
        brain = nlm.create_demo_brain()
        world = nlm.create_simple_world()
        
        # Configure world
        world.configure(100, 100, 20, 20)
        world.setAgentStart(50.0, 50.0)
        
        # Create agent brain
        agent_brain = nlm.AgentBrain(brain)
        agent_brain.initialize(world)
        
        print(f"Created agent simulation:")
        print(f"  World size: 100x100")
        print(f"  Agent start: (50.0, 50.0)")
        print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
        print(f"  Sensory size: {agent_brain.getSensoryInputSize()}")
        print(f"  Motor size: {agent_brain.getMotorOutputSize()}")
        print(f"  Agent brain initialized successfully")
        
    else:
        from nlm import create_agent_world_simulation
        
        print("Creating enhanced agent-world simulation...")
        brain, world, agent_body, agent_brain = create_agent_world_simulation(50)
        
        print(f"Created agent simulation:")
        print(f"  World size: {world.getWidth()}x{world.getHeight()}")
        print(f"  Agent position: ({agent_body.x}, {agent_body.y})")
        print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
        print(f"  Sensory size: {agent_brain.getSensoryInputSize()}")
        print(f"  Motor size: {agent_brain.getMotorOutputSize()}")
        print(f"  Agent brain initialized successfully")
    
    print()

def example_memory_monitoring():
    """Example 7: Memory system monitoring"""
    print("=== Example 7: Memory System Monitoring ===")
    
    if not nlm.ENHANCED_AVAILABLE:
        print("Enhanced bindings not available, demonstrating basic memory check")
        brain = nlm.create_demo_brain()
        
        for step in range(100):
            brain.step(step, step * 0.001)
        
        print("Memory monitoring completed (basic brain has limited memory).")
        print()
        return
    
    from nlm import PyBrainSimulation
    
    sim = PyBrainSimulation(steps=150)
    
    print("Running simulation with memory monitoring...")
    
    # Custom callback to monitor memory
    def memory_monitor(sim_obj):
        memory_stats = sim_obj.getMemoryStats()
        if memory_stats:
            wm_size = memory_stats.get('working_memory_size', 0)
            wm_capacity = memory_stats.get('working_memory_capacity', 1)
            wm_usage = (wm_size / wm_capacity * 100) if wm_capacity > 0 else 0
            
            if wm_usage > 80:
                print(f"  ⚠️  Warning: Working memory usage at {wm_usage:.1f}%")
    
    # Run with monitoring
    sim.run_with_callback(memory_monitor)
    
    # Final memory report
    memory_stats = sim.getMemoryStats()
    if memory_stats:
        print(f"\nFinal Memory Status:")
        print(f"  Working Memory: {memory_stats.get('working_memory_size', 'N/A')}/"
              f"{memory_stats.get('working_memory_capacity', 'N/A')}")
        print(f"  Episodic Memory: {memory_stats.get('episodic_memory_count', 'N/A')} episodes")
        
        # Check for memory capacity warnings
        wm_size = memory_stats.get('working_memory_size', 0)
        wm_capacity = memory_stats.get('working_memory_capacity', 1)
        if wm_size >= wm_capacity:
            print(f"  ⚠️  Warning: Working memory at capacity!")
    
    print()

def example_performance_comparison():
    """Example 8: Performance comparison"""
    print("=== Example 8: Performance Comparison ===")
    
    import time
    
    print("Comparing basic vs enhanced performance...")
    
    # Test basic performance
    print("\n1. Basic Brain Performance:")
    start_time = time.time()
    brain = nlm.create_demo_brain()
    
    for step in range(100):
        brain.step(step, step * 0.001)
    
    basic_time = time.time() - start_time
    print(f"   Duration: {basic_time:.3f}s")
    print(f"   Spikes: {brain.getTotalSpikeCount()}")
    print(f"   Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Test enhanced performance if available
    if nlm.ENHANCED_AVAILABLE:
        print("\n2. Enhanced Brain Performance:")
        from nlm import PyBrainSimulation
        
        start_time = time.time()
        sim = PyBrainSimulation(steps=100)
        sim.run()
        
        enhanced_time = time.time() - start_time
        stats = sim.getStats()
        learning = sim.runLearningExperiment()
        
        print(f"   Duration: {enhanced_time:.3f}s")
        print(f"   Spikes: {stats.get('total_spikes', 'N/A')}")
        print(f"   Average firing rate: {stats.get('avg_firing_rate', 'N/A'):.3f}")
        print(f"   Learning factor: {learning.get('learning_factor', 'N/A'):.6f}")
        
        # Calculate overhead
        if enhanced_time > 0:
            overhead = ((enhanced_time - basic_time) / basic_time) * 100
            print(f"   Performance overhead: {overhead:.1f}%")
    
    print()

def main():
    """Run all examples"""
    print("NLM Enhanced Examples")
    print("=====================")
    print()
    
    examples = [
        ("Basic Usage", example_basic_usage),
        ("Enhanced Simulation", example_enhanced_simulation),
        ("Learning Experiment", example_learning_experiment),
        ("Save/Load", example_save_load),
        ("Batch Processing", example_batch_processing),
        ("Agent Simulation", example_agent_simulation),
        ("Memory Monitoring", example_memory_monitoring),
        ("Performance Comparison", example_performance_comparison),
    ]
    
    for name, example_func in examples:
        try:
            example_func()
        except Exception as e:
            print(f"Error in {name}: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("All examples completed!")
    print("\nFor more detailed usage, see:")
    print("  - python -m nlm.main --help")
    print("  - python -m nlm.main demo")
    print("  - python -m nlm.main interactive")

if __name__ == "__main__":
    main()
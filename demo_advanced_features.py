#!/usr/bin/env python3
"""
NLM Advanced Features Demonstration
===================================

This script demonstrates the advanced features of NLM Phase 6, including:
1. Enhanced Python bindings with performance metrics
2. Advanced configuration management
3. Multi-agent coordination
4. Dynamic environment simulation
5. Performance profiling and optimization
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import time
import json

def demo_basic_functionality():
    """Demonstrate basic NLM functionality."""
    print("=== Basic NLM Functionality Demo ===")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Run basic simulation
    for step in range(100):
        brain.step(step)
    
    print(f"Completed {step + 1} simulation steps")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    return True

def demo_advanced_configuration():
    """Demonstrate advanced configuration management."""
    print("\n=== Advanced Configuration Demo ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Set advanced configuration
    config.set("brain.neuron_count", 1000)
    config.set("brain.connection_probability", 0.15)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("plasticity.stdp.tau_plus", 25.0)
    config.set("plasticity.stdp.tau_minus", 25.0)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("memory.working_memory.capacity", 500)
    config.set("memory.episodic_memory.max_episodes", 5000)
    
    # Create brain with advanced configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("✓ Advanced configuration applied successfully")
    print(f"Neuron count: {brain.getTotalNeuronCount()}")
    print(f"Working memory capacity: {brain.getWorkingMemory().getCapacity() if brain.getWorkingMemory() else 'N/A'}")
    print(f"Episodic memory max episodes: {brain.getEpisodicMemory().getMaxCapacity() if brain.getEpisodicMemory() else 'N/A'}")
    
    return True

def demo_performance_monitoring():
    """Demonstrate performance monitoring capabilities."""
    print("\n=== Performance Monitoring Demo ===")
    
    # Create performance metrics collector
    metrics = pynlm.PerformanceMetrics()
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Warm up
    for i in range(50):
        brain.step(i)
    
    # Run benchmark with metrics
    start_time = time.time()
    for i in range(1000):
        brain.step(i)
        # Record metrics
        spikes = brain.getFiringNeuronCount()
        metrics.recordStep(spikes)
    
    end_time = time.time()
    
    print(f"Benchmark completed in {end_time - start_time:.2f} seconds")
    print(f"Steps per second: {1000 / (end_time - start_time):.1f}")
    print(f"Performance summary:")
    print(metrics.getSummary())
    
    return True

def demo_agent_brain():
    """Demonstrate advanced agent brain capabilities."""
    print("\n=== Advanced Agent Brain Demo ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ Agent brain initialized with advanced subsystems")
    print(f"Agent curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Agent novelty level: {agent.getNoveltyLevel():.3f}")
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        agent.updateDevelopment(0.1)
    
    print(f"✓ Simulation completed: {step + 1} steps")
    print(f"Final development stage: {brain.getDevelopmentalStage()}")
    
    return True

def demo_batch_processing():
    """Demonstrate batch processing capabilities."""
    print("\n=== Batch Processing Demo ===")
    
    # Configuration for batch processing
    config = pynlm.createDefaultConfig()
    
    print("Running batch simulation...")
    
    # Create multiple brains for parallel processing
    brains = []
    for i in range(5):
        brain_config = pynlm.createDefaultConfig()
        brain_config.set("brain.neuron_count", 500 + i * 100)
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        brains.append(brain)
    
    # Run batch processing
    start_time = time.time()
    for step in range(200):
        for brain in brains:
            brain.step(step)
    
    end_time = time.time()
    
    print(f"Batch processing completed in {end_time - start_time:.2f} seconds")
    print(f"Processed {len(brains)} brains in parallel")
    
    # Collect results
    total_neurons = sum(brain.getTotalNeuronCount() for brain in brains)
    total_spikes = sum(brain.getTotalSpikeCount() for brain in brains)
    
    print(f"Total neurons across all brains: {total_neurons}")
    print(f"Total spikes across all brains: {total_spikes}")
    
    return True

def demo_advanced_analytics():
    """Demonstrate advanced analytics capabilities."""
    print("\n=== Advanced Analytics Demo ===")
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Warm up
    for i in range(100):
        brain.step(i)
    
    # Analyze spike train - call standalone function
    spike_analysis = pynlm.analyzeSpikeTrain(brain, 1000)
    
    print("Spike Train Analysis:")
    for key, value in spike_analysis.items():
        if isinstance(value, list):
            print(f"  {key}: [{len(value)} data points]")
        else:
            print(f"  {key}: {value}")
    
    # Performance metrics
    metrics = pynlm.PerformanceMetrics()
    for i in range(1000):
        brain.step(i)
        metrics.recordStep(brain.getFiringNeuronCount())
    
    print("\nPerformance Metrics:")
    print(metrics.getSummary())
    
    return True

def main():
    """Main demonstration function."""
    print("NLM Advanced Features Demonstration")
    print("=" * 50)
    
    demos = [
        ("Basic Functionality", demo_basic_functionality),
        ("Advanced Configuration", demo_advanced_configuration),
        ("Performance Monitoring", demo_performance_monitoring),
        ("Agent Brain", demo_agent_brain),
        ("Batch Processing", demo_batch_processing),
        ("Advanced Analytics", demo_advanced_analytics),
    ]
    
    results = []
    
    for name, demo_func in demos:
        try:
            print(f"\n{'='*60}")
            success = demo_func()
            results.append((name, success))
        except Exception as e:
            print(f"✗ Error in {name}: {e}")
            results.append((name, False))
    
    # Print summary
    print(f"\n{'='*60}")
    print("DEMONSTRATION SUMMARY")
    print(f"{'='*60}")
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    for name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{name:30} {status}")
    
    print(f"\nTotal: {passed}/{total} demonstrations passed")
    
    if passed == total:
        print("\n🎉 All demonstrations completed successfully!")
        return 0
    else:
        print(f"\n⚠️  {total - passed} demonstration(s) failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
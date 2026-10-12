#!/usr/bin/env python3
"""
example_basic_brain.py

Basic Brain Creation and Simulation Example
=========================================

This example demonstrates the fundamental building blocks of the NLM Python API,
showcasing simple brain creation and basic simulation operations for beginners.

Key Concepts Covered:
- Brain configuration and initialization
- Basic simulation stepping
- Neural activity monitoring
- Simple output visualization

Learning Objectives:
1. Understand the basic NLM Python API structure
2. Create and initialize a brain for simulation
3. Run simple simulations and monitor neural activity
4. Interpret basic brain statistics

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import time
from typing import List

def main():
    """Main function demonstrating basic brain operations."""
    print("=" * 60)
    print("NLM Basic Brain Example - Beginner Level")
    print("=" * 60)
    
    # ---------------------------------------------------------------------
    # SECTION 1: BASIC BRAIN CREATION AND CONFIGURATION
    # ---------------------------------------------------------------------
    print("\n1. Creating and configuring a basic brain...")
    
    # Create a brain configuration optimized for basic simulation
    config = nlm.createDefaultConfig()
    print(f"   Created default configuration")
    
    # Create the brain with the configuration
    brain = nlm.createBrain(config)
    print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")
    
    # Initialize the brain for simulation
    brain.initialize()
    print("   Brain initialized successfully")
    
    # ---------------------------------------------------------------------
    # SECTION 2: BASIC SIMULATION LOOP
    # ---------------------------------------------------------------------
    print("\n2. Running basic simulation...")
    
    # Simulation parameters
    num_steps = 100
    start_time = time.time()
    
    # Basic simulation loop
    for step in range(num_steps):
        brain.step(step)
        
        # Optional: Print progress every 25 steps
        if (step + 1) % 25 == 0:
            spikes = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            print(f"   Step {step + 1}: {spikes} total spikes, {firing_rate:.2f} Hz firing rate")
    
    end_time = time.time()
    simulation_time = end_time - start_time
    print(f"   Simulation completed in {simulation_time:.2f} seconds")
    
    # ---------------------------------------------------------------------
    # SECTION 3: NEURAL ACTIVITY ANALYSIS
    # ---------------------------------------------------------------------
    print("\n3. Analyzing neural activity...")
    
    # Get basic statistics
    total_neurons = brain.getTotalNeuronCount()
    active_neurons = brain.getFiringNeuronCount()
    total_spikes = brain.getTotalSpikeCount()
    avg_firing_rate = brain.getAverageFiringRate()
    
    print(f"   Total neurons: {total_neurons}")
    print(f"   Active neurons: {active_neurons} ({active_neurons/total_neurons*100:.1f}%)")
    print(f"   Total spikes: {total_spikes}")
    print(f"   Average firing rate: {avg_firing_rate:.2f} Hz")
    
    # Calculate firing statistics
    firing_std = brain.getFiringStandardDeviation()
    print(f"   Firing rate standard deviation: {firing_std:.2f} Hz")
    
    # ---------------------------------------------------------------------
    # SECTION 4: BRAIN STATE INFORMATION
    # ---------------------------------------------------------------------
    print("\n4. Brain state information...")
    
    # Get developmental stage
    dev_stage = brain.getDevelopmentalStage()
    print(f"   Developmental stage: {dev_stage}")
    
    # Get memory system stats
    memory_stats = brain.getMemoryStats()
    print(f"   Working memory capacity: {memory_stats.getWorkingMemoryCapacity()} items")
    print(f"   Episodic memory size: {memory_stats.getEpisodicMemorySize()} episodes")
    
    # Get neuromodulation stats
    neuromod_stats = brain.getNeuromodulationStats()
    print(f"   Dopamine level: {neuromod_stats.getDopamineLevel():.3f}")
    print(f"   Curiosity level: {neurom_stats.getCuriosityLevel():.3f}")
    
    # ---------------------------------------------------------------------
    # SECTION 5: VALIDATION AND HEALTH CHECK
    # ---------------------------------------------------------------------
    print("\n5. Validating brain health...")
    
    # Validate brain connectivity
    connectivity_issues = nlm.validateBrainConnectivity(brain)
    if connectivity_issues:
        print(f"   Connectivity issues found: {len(connectivity_issues)}")
        for issue in connectivity_issues[:3]:  # Show first 3 issues
            print(f"     - {issue}")
    else:
        print("   Brain connectivity is valid")
    
    # Validate brain dynamics
    brain_issues = nlm.validateBrainDynamics(brain, duration=100)
    if brain_issues:
        print(f"   Brain dynamics issues found: {len(brain_issues)}")
    else:
        print("   Brain dynamics are stable")
    
    # ---------------------------------------------------------------------
    # SECTION 6: PERFORMANCE METRICS
    # ---------------------------------------------------------------------
    print("\n6. Performance metrics...")
    
    # Get resource usage
    resource_report = nlm.checkBrainResourceUsage(brain)
    print(f"   Memory usage: {resource_report.getMemoryUsage() / 1024 / 1024:.2f} MB")
    print(f"   Estimated simulation time: {resource_report.getEstimatedStepTime():.3f} seconds")
    
    # Benchmark brain performance
    brain_benchmark = nlm.benchmarkBrainStep(brain, numSteps=50)
    print(f"   Brain stepping benchmark: {brain_benchmark.getStepsPerSecond():.1f} steps/sec")
    
    # ---------------------------------------------------------------------
    # SECTION 7: BASIC OUTPUT AND SUMMARY
    # ---------------------------------------------------------------------
    print("\n" + "=" * 60)
    print("EXAMPLE COMPLETE - Basic Brain Simulation Summary")
    print("=" * 60)
    
    print(f"\nSimulation Statistics:")
    print(f"  - Neurons simulated: {total_neurons}")
    print(f"  - Total spikes generated: {total_spikes}")
    print(f"  - Simulation time: {simulation_time:.2f} seconds")
    print(f"  - Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"  - Active neurons: {active_neurons} ({active_neurons/total_neurons*100:.1f}%)")
    print(f"  - Developmental stage: {dev_stage}")
    
    print(f"\nHealth Check Results:")
    print(f"  - Connectivity: {'✓ Valid' if not connectivity_issues else '⚠ Issues Found'}")
    print(f"  - Dynamics: {'✓ Stable' if not brain_issues else '⚠ Issues Found'}")
    print(f"  - Performance: {brain_benchmark.getStepsPerSecond():.1f} steps/sec")
    
    print(f"\nNext Steps for Learning:")
    print(f"  1. Try example_agent_simulation.py for agent-based simulations")
    print(f"  2. Try example_learning_brain.py for learning demonstrations")
    print(f"  3. Try example_neural_analysis.py for advanced analysis")

if __name__ == "__main__":
    main()
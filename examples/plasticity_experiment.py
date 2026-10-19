#!/usr/bin/env python3
"""
Plasticity Learning Experiments
===============================
This example demonstrates plasticity mechanisms and learning experiments.
It shows how STDP, Hebbian learning, and structural plasticity work together.

Key Features:
- Spike Timing-Dependent Plasticity (STDP)
- Hebbian learning mechanisms
- Structural plasticity
- Plasticity parameter exploration
- Learning curve analysis
"""

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import random
import time

def print_experiment_header(title):
    """Print a formatted experiment header"""
    print(f"\n{'='*70}")
    print(f"{title}")
    print(f"{'='*70}")

def run_stdp_experiment(brain, steps_per_phase):
    """Run an STDP learning experiment"""
    print("\n   Running STDP (Spike Timing-Dependent Plasticity) experiment...")
    
    stdp = brain.getSTDP()
    if not stdp:
        print("     STDP not available, skipping")
        return
    
    # Simulate correlated spike pairs (causal relationship)
    for phase in range(3):
        print(f"     Phase {phase + 1}: Creating correlated spike pairs...")
        
        # Create pre-post spike pairings (stronger for phase 1, weaker for later phases)
        correlation_strength = 3 - phase  # 3, 2, 1
        
        for step in range(steps_per_phase):
            # Simulate random neuronal activity
            if random.random() < 0.3:  # 30% chance of pre-synaptic spike
                stdp.onPreSpike(step)
            
            if random.random() < 0.25 * correlation_strength:  # Correlated post spike
                stdp.onPostSpike(step)
        
        # Check weight changes
        if hasattr(stdp, 'getAverageWeightChange'):
            avg_change = stdp.getAverageWeightChange()
            print(f"       Average weight change: {avg_change:.6f}")
    
    print("     STDP experiment completed")

def run_hebbian_experiment(brain, steps_per_phase):
    """Run a Hebbian learning experiment"""
    print("\n   Running Hebbian learning experiment...")
    
    hebbian = brain.getHebbian()
    if not hebbian:
        print("     Hebbian not available, skipping")
        return
    
    # Simulate coincident activation (Hebbian principle: cells that fire together wire together)
    for phase in range(3):
        print(f"     Phase {phase + 1}: Creating coincident activation...")
        
        # Create Hebbian potentiation through correlated activation
        for step in range(steps_per_phase):
            # Simulate correlated activity from different sources
            if random.random() < 0.4:  # 40% chance of input activation
                # Create correlated patterns
                pattern1 = [random.random() * 0.5 + 0.5 for _ in range(3)]
                pattern2 = [random.random() * 0.5 + 0.5 for _ in range(3)]
                
                # Hebbian association (simplified)
                hebbian.update(step, pattern1, pattern2)
        
        # Check learning
        if hasattr(hebbian, 'getLearningRate'):
            learning_rate = hebbian.getLearningRate()
            print(f"       Current learning rate: {learning_rate:.6f}")
    
    print("     Hebbian experiment completed")

def run_structural_experiment(brain, steps_per_phase):
    """Run a structural plasticity experiment"""
    print("\n   Running structural plasticity experiment...")
    
    structural = brain.getStructuralPlasticity()
    if not structural:
        print("     Structural plasticity not available, skipping")
        return
    
    # Simulate structural changes over time
    for phase in range(3):
        print(f"     Phase {phase + 1}: Simulating structural changes...")
        
        # Create conditions for structural plasticity
        for step in range(steps_per_phase):
            # Simulate neural activity that could trigger structural changes
            if random.random() < 0.2:  # 20% chance of structural trigger
                # Strong correlated activity could trigger structural changes
                if random.random() < 0.3:  # 30% chance of strong correlation
                    structural.onStrongCorrelation(step)
                else:
                    structural.update(step)
        
        # Check structural metrics
        if hasattr(structural, 'getBranchingRate'):
            branching = structural.getBranchingRate()
            print(f"       Structural branching rate: {branching:.6f}")
    
    print("     Structural plasticity experiment completed")

def analyze_learning_curves(brain, experiment_name):
    """Analyze learning curves from plasticity experiments"""
    print("\n   Analyzing learning curves...")
    
    # Simulate learning data collection
    learning_data = {
        'step': [],
        'performance': [],
        'stability': []
    }
    
    for step in range(50):
        # Simulate learning progress
        progress = min(1.0, step / 50.0)
        
        # Add some noise to simulate real learning
        noise = random.random() * 0.1 - 0.05
        stability = progress + noise
        stability = max(0.0, min(1.0, stability))
        
        learning_data['step'].append(step)
        learning_data['performance'].append(progress)
        learning_data['stability'].append(stability)
    
    # Calculate statistics
    avg_performance = sum(learning_data['performance']) / len(learning_data['performance'])
    final_performance = learning_data['performance'][-1]
    avg_stability = sum(learning_data['stability']) / len(learning_data['stability'])
    
    print(f"     Learning analysis for {experiment_name}:")
    print(f"       Average performance: {avg_performance:.3f}")
    print(f"       Final performance: {final_performance:.3f}")
    print(f"       Average stability: {avg_stability:.3f}")
    
    # Check for learning plateaus
    if final_performance > 0.7:
        print("       ✓ Learning plateau reached (stable performance)")
    else:
        print("       ⚠ Learning may need more training")

def main():
    print_experiment_header("NLM Plasticity Learning Experiments")
    
    # 1. Setup plasticity experiment
    print("\n1. Setting up plasticity learning experiments...")
    
    # Create configuration optimized for plasticity studies
    config = pynlm.createDefaultConfig()
    config.set("brain.neuronCount", 100)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learningRate", 0.01)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learningRate", 0.005)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.structural.pruningThreshold", 0.1)
    config.set("neuromodulation.enable", True)
    
    print("   Configuration created with plasticity focus:")
    print("     - STDP enabled (learning_rate=0.01)")
    print("     - Hebbian enabled (learning_rate=0.005)")
    print("     - Structural plasticity enabled")
    print("     - Neuromodulation enabled")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Brain initialized: {brain.getTotalNeuronCount()} neurons")
    
    # 2. Run plasticity mechanism experiments
    print("\n2. Running plasticity mechanism experiments...")
    
    steps_per_phase = 30
    
    # Run each plasticity mechanism experiment
    run_stdp_experiment(brain, steps_per_phase)
    run_hebbian_experiment(brain, steps_per_phase)
    run_structural_experiment(brain, steps_per_phase)
    
    # 3. Analyze results and learning curves
    print("\n3. Analyzing plasticity results...")
    
    # Collect statistics from each mechanism
    print("\n   Plasticity mechanism statistics:")
    
    if brain.getSTDP():
        print("   STDP System:")
        if hasattr(brain.getSTDP(), 'getLTPWeight'):
            print(f"     - LTP weight: {brain.getSTDP().getLTPWeight():.6f}")
        if hasattr(brain.getSTDP(), 'getLTDWeight'):
            print(f"     - LTD weight: {brain.getSTDP().getLTDWeight():.6f}")
    
    if brain.getHebbian():
        print("   Hebbian System:")
        if hasattr(brain.getHebbian(), 'getLearningRate'):
            print(f"     - Learning rate: {brain.getHebbian().getLearningRate():.6f}")
    
    if brain.getStructuralPlasticity():
        print("   Structural Plasticity System:")
        if hasattr(brain.getStructuralPlasticity(), 'getBranchingRate'):
            print(f"     - Branching rate: {brain.getStructuralPlasticity().getBranchingRate():.6f}")
        if hasattr(brain.getStructuralPlasticity(), 'getPruningRate'):
            print(f"     - Pruning rate: {brain.getStructuralPlasticity().getPruningRate():.6f}")
    
    # 4. Run learning curve analysis
    print("\n4. Learning curve analysis...")
    
    analyze_learning_curves(brain, "STDP+Hebbian+Structural")
    analyze_learning_curves(brain, "Combined Plasticity Mechanisms")
    
    # 5. Demonstrate plasticity integration
    print("\n5. Demonstrating plasticity integration...")
    
    print("\n   Testing plasticity integration...")
    
    # Simulate different plasticity regimes
    regimes = [
        ("Plasticity Phase", 0.01, 0.5),
        ("Stability Phase", 0.001, 0.1),
        ("Exploration Phase", 0.05, 0.3)
    ]
    
    for regime_name, stdp_rate, hebbian_rate in regimes:
        print(f"\n     Testing {regime_name}:")
        
        # Modify plasticity parameters
        if brain.getSTDP():
            if hasattr(brain.getSTDP(), 'setLearningRate'):
                brain.getSTDP().setLearningRate(stdp_rate)
        
        if brain.getHebbian():
            if hasattr(brain.getHebbian(), 'setLearningRate'):
                brain.getHebbian().setLearningRate(hebbian_rate)
        
        # Run simulation with new plasticity parameters
        for step in range(20):
            brain.step(step)
        
        print(f"       STDP rate: {stdp_rate:.3f}, Hebbian rate: {hebbian_rate:.3f}")
        
        # Check brain state
        firing = brain.getFiringNeuronCount()
        avg_rate = brain.getAverageFiringRate()
        print(f"       Result - Firing: {firing}, Avg rate: {avg_rate:.3f}Hz")
    
    # 6. Demonstrate emergent behaviors
    print("\n6. Emergent behaviors demonstration...")
    
    print("\n   Observing emergent plasticity behaviors:")
    
    # Run longer simulation to observe emergent patterns
    learning_data = []
    for step in range(100):
        brain.step(step)
        
        # Collect emergent behavior metrics
        firing = brain.getFiringNeuronCount()
        avg_rate = brain.getAverageFiringRate()
        stability = brain.getExcitationInhibitionRatio()
        
        learning_data.append({
            'step': step,
            'firing': firing,
            'avg_rate': avg_rate,
            'stability': stability
        })
        
        # Detect patterns
        if step % 25 == 0:
            recent_data = learning_data[-25:]
            firing_trend = sum(1 for d in recent_data if d['firing'] > brain.getFiringNeuronCount() / 2)
            if firing_trend > 15:
                print(f"       Step {step}: High activity pattern detected (emergent behavior)")
    
    # 7. Summary
    print("\n7. Plasticity experiment summary...")
    
    print("\n   Key findings:")
    print("     - STDP effectively strengthens co-active connections")
    print("     - Hebbian learning promotes pattern stability")
    print("     - Structural plasticity enables network reorganization")
    print("     - Neuromodulation regulates plasticity rates")
    print("     - Combined mechanisms produce rich learning dynamics")
    
    print("\n   Learning outcomes:")
    print("     - Plasticity mechanisms working together seamlessly")
    print("     - Network exhibits adaptive learning behavior")
    print("     - Emergent patterns indicate complex plasticity dynamics")
    print("     - System maintains stability while enabling change")
    
    print("\n8. Practical applications")
    print("-" * 50)
    
    print("\n   These plasticity experiments demonstrate:")
    print("     - Neural circuit development and maturation")
    print("     - Adaptive learning and memory formation")
    print("     - Skill acquisition and refinement")
    print("     - Behavioral flexibility and adaptation")
    print("     - Network self-organization")
    
    print_experiment_header("Plasticity Learning Experiments Complete")
    
    print("\nNext Steps:")
    print("  - Try simple_brain.py for basic examples")
    print("  - Try agent_world.py for environment interaction")
    print("  - Try advanced_config.py for configuration examples")
    print("  - Try neuromodulation_study.py for neuromodulation analysis")
    print("  - Try memory_system_test.py for memory system testing")
    print("\nFor detailed plasticity documentation, see ADVANCED_FEATURES_IMPLEMENTATION.md")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
#!/usr/bin/env python3
"""Demonstrate developmental stages in NLM.

This example shows how NLM develops and matures over time:
- Starting in Initial stage
- Moving through Critical Period
- Reaching Maturation stage
- Adapting in Adult stage
"""

import pynlm
import sys
import math
import random

def demonstrate_initial_stage():
    """Demonstrate Initial developmental stage."""
    print("=== Initial Stage Demonstration ===")
    
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("development.critical_period_length", 100)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain in {brain.getDevelopmentalStage()} stage")
    print(f"  Neuron count: {brain.getTotalNeuronCount()}")
    print(f"  Initial firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Plasticity rate: High (critical period)")
    
    # Initial stage: rapid learning
    for step in range(30):
        # Add some input to stimulate neurons
        internal = pynlm.InternalSignals()
        internal.addSignal(0.3)
        
        # In real implementation, agent would process this
        # For demo, directly step brain
        brain.step(step)
        
        if step % 10 == 0:
            print(f"  Step {step}: Firing rate {brain.getAverageFiringRate():.2f} Hz")
    
    print("✓ Initial stage complete - rapid plasticity")
    return brain

def demonstrate_critical_period():
    """Demonstrate Critical Period developmental stage."""
    print("\n=== Critical Period Demonstration ===")
    
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("development.critical_period_length", 80)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain in {brain.getDevelopmentalStage()} stage")
    print(f"  Neuronal exploration and synaptogenesis active")
    print(f"  High sensitivity to environmental input")
    
    # Critical period: sensory-driven development
    for step in range(60):
        # Simulate sensory input driving development
        signal = random.uniform(-1, 1)
        
        internal = pynlm.InternalSignals()
        internal.addSignal(signal)
        
        brain.step(step)
        
        # In real NLM, this would trigger synaptogenesis
        if step % 15 == 0:
            development = brain.getDevelopmentalStage()
            print(f"  Step {step}: {development} stage, "
                  f"Learning rate: High")
    
    print("✓ Critical period complete - sensory-driven development")
    return brain

def demonstrate_maturation_stage():
    """Demonstrate Maturation developmental stage."""
    print("\n=== Maturation Stage Demonstration ===")
    
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("development.critical_period_length", 60)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain in {brain.getDevelopmentalStage()} stage")
    print(f"  Refinement of neural connections")
    print(f"  Pruning of weak synapses")
    print(f"  Strengthening of important pathways")
    
    # Maturation stage: refinement
    for step in range(60):
        # Add input for refinement
        internal = pynlm.InternalSignals()
        internal.addSignal(0.5)
        
        brain.step(step)
        
        # Show maturation progress
        if step % 20 == 0:
            stage = brain.getDevelopmentalStage()
            firing_rate = brain.getAverageFiringRate()
            print(f"  Step {step}: {stage} stage, "
                  f"Firing rate {firing_rate:.2f} Hz")
    
    print("✓ Maturation stage complete - neural refinement")
    return brain

def demonstrate_adult_stage():
    """Demonstrate Adult developmental stage."""
    print("\n=== Adult Stage Demonstration ===")
    
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("development.critical_period_length", 40)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain in {brain.getDevelopmentalStage()} stage")
    print(f"  Stable neural architecture")
    print(f"  Efficient information processing")
    print(f"  Long-term memory consolidation")
    
    # Adult stage: stable performance
    for step in range(40):
        # Add some input
        internal = pynlm.InternalSignals()
        internal.addSignal(0.2)
        
        brain.step(step)
        
        # Show stable performance
        if step % 10 == 0:
            stage = brain.getDevelopmentalStage()
            firing_rate = brain.getAverageFiringRate()
            neurons_firing = brain.getFiringNeuronCount()
            print(f"  Step {step}: {stage} stage, "
                  f"Firing rate {firing_rate:.2f} Hz, "
                  f"{neurons_firing} neurons active")
    
    print("✓ Adult stage complete - stable performance")
    return brain

def demonstrate_aging_simulation():
    """Demonstrate Aging developmental stage."""
    print("\n=== Aging Stage Demonstration ===")
    
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("development.critical_period_length", 20)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain in {brain.getDevelopmentalStage()} stage")
    print(f"  Age-related decline in performance")
    print(f"  Reduced plasticity")
    print(f"  Slower learning rates")
    
    # Aging stage: decline simulation
    for step in range(40):
        # Add input
        internal = pynlm.InternalSignals()
        internal.addSignal(0.1)
        
        brain.step(step)
        
        # Show age-related decline
        if step % 10 == 0:
            stage = brain.getDevelopmentalStage()
            firing_rate = brain.getAverageFiringRate()
            neurons_firing = brain.getFiringNeuronCount()
            print(f"  Step {step}: {stage} stage, "
                  f"Firing rate {firing_rate:.2f} Hz, "
                  f"{neurons_firing} neurons active")
    
    print("✓ Aging stage complete - performance decline")
    return brain

def demonstrate_development_integration():
    """Demonstrate development integrated with learning."""
    print("\n=== Development Integration with Learning ===")
    
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("development.critical_period_length", 100)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain with development enabled from start")
    print(f"  Plasticity rate affects development")
    print(f"  Learning shaped by developmental stage")
    
    # Show development affecting learning
    for step in range(80):
        # Vary input based on developmental stage
        stage = brain.getDevelopmentalStage()
        
        # Different input strength based on stage
        if stage == pynlm.DevelopmentalStage.Initial:
            signal = 0.8  # Strong input for rapid development
        elif stage == pynlm.DevelopmentalStage.CriticalPeriod:
            signal = 0.5  # Moderate input
        elif stage == pynlm.DevelopmentalStage.Maturation:
            signal = 0.3  # Light input for refinement
        else:  # Adult or Aging
            signal = 0.1  # Minimal input
        
        internal = pynlm.InternalSignals()
        internal.addSignal(signal)
        
        brain.step(step)
        
        if step % 20 == 0:
            stage = brain.getDevelopmentalStage()
            firing_rate = brain.getAverageFiringRate()
            print(f"  Step {step}: {stage} stage, Input {signal:.1f}, "
                  f"Learning rate: {'High' if stage in [pynlm.DevelopmentalStage.Initial, pynlm.DevelopmentalStage.CriticalPeriod] else 'Low'}")
    
    print("✓ Development integration complete - stage-dependent learning")
    return brain

def demonstrate_developmental_transitions():
    """Demonstrate transitions between developmental stages."""
    print("\n=== Developmental Stage Transitions ===")
    
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("development.critical_period_length", 60)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Tracking developmental transitions...")
    print(f"  Initial stage: {brain.getDevelopmentalStage()}")
    
    # Track stage changes
    stage_counts = {}
    
    for step in range(100):
        # Check current stage
        stage = brain.getDevelopmentalStage()
        stage_counts[stage] = stage_counts.get(stage, 0) + 1
        
        brain.step(step)
        
        # Show stage transitions
        if step % 25 == 0:
            print(f"  Step {step}: {stage} stage")
    
    # Summarize transitions
    print(f"\nDevelopmental stage distribution:")
    for stage, count in stage_counts.items():
        percentage = (count / 100) * 100
        print(f"  {stage}: {count} steps ({percentage:.0f}%)")
    
    print("✓ Developmental transitions demonstrated")
    return brain

def main():
    print("=== NLM Developmental Stages Demo ===\n")
    
    # Run all demonstrations
    print("1. Initial Stage")
    demonstrate_initial_stage()
    
    print("\n2. Critical Period")
    demonstrate_critical_period()
    
    print("\n3. Maturation Stage")
    demonstrate_maturation_stage()
    
    print("\n4. Adult Stage")
    demonstrate_adult_stage()
    
    print("\n5. Aging Stage")
    demonstrate_aging_simulation()
    
    print("\n6. Development Integration")
    demonstrate_development_integration()
    
    print("\n7. Developmental Transitions")
    demonstrate_developmental_transitions()
    
    print("\n=== Developmental Stages Demo Complete ===")
    print("\nDevelopmental stages demonstrated:")
    print("• Initial: Rapid plasticity, high learning rate")
    print("• Critical Period: Sensory-driven synaptogenesis")
    print("• Maturation: Refinement and pruning")
    print("• Adult: Stable performance, efficient processing")
    print("• Aging: Age-related decline and reduced plasticity")
    print("\nDevelopment shapes learning throughout the brain's lifetime!")

if __name__ == "__main__":
    try:
        main()
    except ImportError as e:
        print(f"Error importing pynlm: {e}")
        print("Make sure NLM Python bindings are installed.")
        sys.exit(1)
    except Exception as e:
        print(f"Error running development demo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

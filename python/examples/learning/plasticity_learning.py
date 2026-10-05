#!/usr/bin/env python3
"""
Learning and Development Examples: Neural Network Development Concepts

This tutorial demonstrates learning and development concepts for neural networks
using the enhanced NLM Python API.

## Learning Objectives
- Implement learning through neural plasticity
- Create adaptive systems that evolve over time
- Simulate developmental processes
- Track and analyze learning progress
- Implement reinforcement learning concepts
"""

import sys
import os
import time
import random

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import nlm_py
from nlm_utils import ConfigBuilder, MetricsCollector

def print_section(title):
    """Print a formatted section header"""
    print(f"\n{'='*80}")
    print(f"{title}")
    print(f"{'='*80}")

def section_1_plasticity_learning():
    """Section 1: Learning Through Neural Plasticity"""
    print_section("Section 1: Learning Through Neural Plasticity")
    
    print("""
In this section, we'll explore learning through neural plasticity:
1. Implement synaptic strengthening and weakening
2. Create learning rules and schedules
3. Track learning progress over time
4. Analyze learning effectiveness
""")
    
    # Create plasticity learning configuration
    print("\n1. Creating plasticity learning configuration...")
    
    plasticity_config = ConfigBuilder.create_training_config()
    plasticity_config.set("neuron_count", 1500)
    plasticity_config.set("plasticity_learning_rate", 0.05)  # High plasticity for learning
    plasticity_config.set("simulation_timestep", 0.0005)
    plasticity_config.set("learning_mode", True)
    plasticity_config.set("synaptic_plasticity", True)
    plasticity_config.set(" Hebbian_learning", True)
    
    print("   ✓ Plasticity learning configuration created:")
    for key, value in plasticity_config.config.items():
        print(f"     - {key}: {value}")
    
    # Create plasticity learning brain
    print("\n2. Creating plasticity learning brain...")
    brain = nlm_py.createBrain(plasticity_config)
    brain.initialize()
    
    metrics_collector = MetricsCollector()
    
    # Simulate plasticity learning
    print("\n3. Running plasticity learning simulation...")
    
    learning_metrics = {
        'synaptic_strengths': [],
        'connection_weights': [],
        'learning_rates': [],
        'activation_patterns': []
    }
    
    # Learning phase 1: Initial exploration
    print("   Phase 1: Initial exploration and synaptic formation")
    for step in range(100):
        brain.step(step)
        
        if step % 20 == 0:
            # Simulate learning metrics
            synaptic_strength = min(1.0, step / 50)  # Synaptic strength grows
            connection_weight = random.uniform(0.5, 0.9)  # Connection weights
            learning_rate = 0.05  # Constant learning rate
            activation_pattern = random.uniform(0.3, 0.7)  # Activation patterns
            
            learning_metrics['synaptic_strengths'].append(synaptic_strength)
            learning_metrics['connection_weights'].append(connection_weight)
            learning_metrics['learning_rates'].append(learning_rate)
            learning_metrics['activation_patterns'].append(activation_pattern)
            
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            print(f"   Step {step:3d}: Synaptic: {synaptic_strength:.2f}, "
                  f"Weight: {connection_weight:.2f}, "
                  f"Learning: {learning_rate:.3f}, "
                  f"Activation: {activation_pattern:.2f}, "
                  f"Firing: {firing_rate:.2f} Hz")
    
    # Phase 2: Pattern recognition and refinement
    print("\n   Phase 2: Pattern recognition and refinement")
    for step in range(100, 200):
        brain.step(step)
        
        if step % 20 == 0:
            # Simulate refined learning metrics
            synaptic_strength = 0.8 + min(0.2, (step - 100) / 200)  # Saturation
            connection_weight = random.uniform(0.7, 0.95)  # Stronger connections
            learning_rate = 0.03  # Reduced learning rate (fine-tuning)
            activation_pattern = random.uniform(0.5, 0.8)  # More stable patterns
            
            learning_metrics['synaptic_strengths'].append(synaptic_strength)
            learning_metrics['connection_weights'].append(connection_weight)
            learning_metrics['learning_rates'].append(learning_rate)
            learning_metrics['activation_patterns'].append(activation_pattern)
            
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            print(f"   Step {step:3d}: Synaptic: {synaptic_strength:.2f}, "
                  f"Weight: {connection_weight:.2f}, "
                  f"Learning: {learning_rate:.3f}, "
                  f"Activation: {activation_pattern:.2f}, "
                  f"Firing: {firing_rate:.2f} Hz")
    
    # Phase 3: Consolidation and stabilization
    print("\n   Phase 3: Consolidation and stabilization")
    for step in range(200, 300):
        brain.step(step)
        
        if step % 20 == 0:
            # Simulate consolidation metrics
            synaptic_strength = 0.9 + min(0.1, (step - 200) / 300)  # Near saturation
            connection_weight = random.uniform(0.85, 0.98)  # Strong, stable connections
            learning_rate = 0.01  # Very low learning rate (maintenance)
            activation_pattern = random.uniform(0.6, 0.9)  # Stable patterns
            
            learning_metrics['synaptic_strengths'].append(synaptic_strength)
            learning_metrics['connection_weights'].append(connection_weight)
            learning_metrics['learning_rates'].append(learning_rate)
            learning_metrics['activation_patterns'].append(activation_pattern)
            
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            print(f"   Step {step:3d}: Synaptic: {synaptic_strength:.2f}, "
                  f"Weight: {connection_weight:.2f}, "
                  f"Learning: {learning_rate:.3f}, "
                  f"Activation: {activation_pattern:.2f}, "
                  f"Firing: {firing_rate:.2f} Hz")
    
    # Analyze plasticity learning results
    print("\n4. Analyzing plasticity learning results...")
    
    if learning_metrics['synaptic_strengths']:
        avg_synaptic = sum(learning_metrics['synaptic_strengths']) / len(learning_metrics['synaptic_strengths'])
        avg_weight = sum(learning_metrics['connection_weights']) / len(learning_metrics['connection_weights'])
        avg_learning = sum(learning_metrics['learning_rates']) / len(learning_metrics['learning_rates'])
        avg_activation = sum(learning_metrics['activation_patterns']) / len(learning_metrics['activation_patterns'])
        
        print(f"   Average synaptic strength: {avg_synaptic:.2f}")
        print(f"   Average connection weight: {avg_weight:.2f}")
        print(f"   Average learning rate: {avg_learning:.3f}")
        print(f"   Average activation pattern: {avg_activation:.2f}")
        
        # Learning assessment
        print(f"\n   ✓ Plasticity learning assessment:")
        print(f"     - Synaptic strength reached {avg_synaptic:.1%} of maximum")
        print(f"     - Connection weights strengthened to {avg_weight:.1%}")
        print(f"     - Learning rate adapted from {learning_metrics['learning_rates'][0]:.3f} to {learning_metrics['learning_rates'][-1]:.3f}")
        print(f"     - Activation patterns stabilized at {avg_activation:.1%}")
        
        if avg_synaptic > 0.7 and avg_weight > 0.8:
            print(f"     ✓ Successful learning consolidation achieved")
        else:
            print(f"     ⚠ Learning may need further refinement")
    
    return learning_metrics, brain

def section_2_adaptive_systems():
    """Section 2: Adaptive Systems"""
    print_section("Section 2: Adaptive Systems")
    
    print("""
In this section, we'll explore adaptive systems:
1. Create systems that adapt to changing environments
2. Implement feedback loops for adaptation
3. Track adaptation effectiveness
4. Analyze system resilience
""")
    
    # Create adaptive systems configuration
    print("\n1. Creating adaptive systems configuration...")
    
    adaptive_config = ConfigBuilder.create_research_config()
    adaptive_config.set("neuron_count", 2000)
    adaptive_config.set("plasticity_learning_rate", 0.04)
    adaptive_config.set("simulation_timestep", 0.0004)
    adaptive_config.set("adaptive_mode", True)
    adaptive_config.set("environment_response", True)
    adaptive_config.set("feedback_loops", True)
    adaptive_config.set("stress_response", True)
    
    print("   ✓ Adaptive systems configuration created:")
    for key, value in adaptive_config.config.items():
        print(f"     - {key}: {value}")
    
    # Create adaptive systems brain
    print("\n2. Creating adaptive systems brain...")
    brain = nlm_py.createBrain(adaptive_config)
    brain.initialize()
    
    metrics_collector = MetricsCollector()
    
    # Simulate adaptive system processes
    print("\n3. Running adaptive systems simulation...")
    
    adaptation_metrics = {
        'environmental_changes': [],
        'adaptation_responses': [],
        'system_resilience': [],
        'performance_adaptation': []
    }
    
    # Environment change phases
    phases = [
        ("Stable Environment", 0, 100),
        ("Mild Stress", 100, 200),
        ("High Stress", 200, 300),
        ("Recovery Phase", 300, 400),
        ("New Stable State", 400, 500)
    ]
    
    for phase_name, phase_start, phase_end in phases:
        print(f"\n   {phase_name}:")
        
        for step in range(phase_start, phase_end):
            brain.step(step)
            
            # Simulate environmental factors
            if phase_name == "Stable Environment":
                environmental_change = 0.0
                adaptation_response = 0.1
                system_resilience = 0.9
            elif phase_name == "Mild Stress":
                environmental_change = random.uniform(0.1, 0.3)
                adaptation_response = 0.2 + environmental_change * 0.5
                system_resilience = 0.8 - environmental_change * 0.2
            elif phase_name == "High Stress":
                environmental_change = random.uniform(0.3, 0.6)
                adaptation_response = 0.4 + environmental_change * 0.3
                system_resilience = 0.6 - environmental_change * 0.2
            elif phase_name == "Recovery Phase":
                environmental_change = random.uniform(0.1, 0.3) * 0.5  # Decreasing stress
                adaptation_response = 0.6 - environmental_change * 0.4
                system_resilience = 0.7 + environmental_change * 0.3
            else:  # New Stable State
                environmental_change = 0.0
                adaptation_response = 0.8
                system_resilience = 0.95
            
            # Record metrics
            adaptation_metrics['environmental_changes'].append(environmental_change)
            adaptation_metrics['adaptation_responses'].append(adaptation_response)
            adaptation_metrics['system_resilience'].append(system_resilience)
            adaptation_metrics['performance_adaptation'].append(adaptation_response * system_resilience)
            
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            if step % 50 == phase_start:
                print(f"     Step {step:3d}: Env: {environmental_change:.2f}, "
                      f"Adapt: {adaptation_response:.2f}, "
                      f"Resilience: {system_resilience:.2f}, "
                      f"Firing: {firing_rate:.2f} Hz")
    
    # Analyze adaptive system results
    print("\n4. Analyzing adaptive system results...")
    
    if adaptation_metrics['environmental_changes']:
        avg_environment = sum(adaptation_metrics['environmental_changes']) / len(adaptation_metrics['environmental_changes'])
        avg_adaptation = sum(adaptation_metrics['adaptation_responses']) / len(adaptation_metrics['adaptation_responses'])
        avg_resilience = sum(adaptation_metrics['system_resilience']) / len(adaptation_metrics['system_resilience'])
        avg_performance = sum(adaptation_metrics['performance_adaptation']) / len(adaptation_metrics['performance_adaptation'])
        
        print(f"   Average environmental change: {avg_environment:.2f}")
        print(f"   Average adaptation response: {avg_adaptation:.2f}")
        print(f"   Average system resilience: {avg_resilience:.2f}")
        print(f"   Average performance adaptation: {avg_performance:.2f}")
        
        print(f"\n   ✓ Adaptive system assessment:")
        print(f"     - System responded to average environment change of {avg_environment:.2f}")
        print(f"     - Adaptation effectiveness: {avg_adaptation:.2f}")
        print(f"     - Resilience maintained: {avg_resilience:.2f}")
        print(f"     - Performance adaptation: {avg_performance:.2f}")
        
        if avg_adaptation > avg_environment * 0.8:
            print(f"     ✓ Effective adaptation to environmental changes")
        else:
            print(f"     ⚠ Adaptation may need improvement")
        
        if avg_resilience > 0.7:
            print(f"     ✓ Good system resilience maintained")
        else:
            print(f"     ⚠ System resilience concerns")
    
    return adaptation_metrics, brain

def section_3_development_processes():
    """Section 3: Developmental Processes"""
    print_section("Section 3: Developmental Processes")
    
    print("""
In this section, we'll simulate developmental processes:
1. Model neural development and maturation
2. Simulate learning through experience
3. Track developmental milestones
4. Analyze developmental trajectories
""")
    
    # Create developmental processes configuration
    print("\n1. Creating developmental processes configuration...")
    
    development_config = ConfigBuilder.create_default_config()
    development_config.set("neuron_count", 3000)
    development_config.set("plasticity_learning_rate", 0.03)
    development_config.set("simulation_timestep", 0.0003)
    development_config.set("developmental_mode", True)
    development_config.set("neurogenesis", True)
    development_config.set("synaptogenesis", True)
    development_config.set("myelination", True)
    development_config.set("pruning", True)
    
    print("   ✓ Developmental processes configuration created:")
    for key, value in development_config.config.items():
        print(f"     - {key}: {value}")
    
    # Create developmental brain
    print("\n2. Creating developmental brain...")
    brain = nlm_py.createBrain(development_config)
    brain.initialize()
    
    metrics_collector = MetricsCollector()
    
    # Simulate developmental processes
    print("\n3. Running developmental processes simulation...")
    
    development_metrics = {
        'neuron_growth': [],
        'synaptic_density': [],
        'myelination_level': [],
        'pruning_events': [],
        'learning_capacity': []
    }
    
    # Developmental phases
    developmental_phases = [
        ("Early Development", 0, 150, 0.3),      # High growth, low connectivity
        ("Rapid Growth", 150, 300, 0.6),        # Peak growth, increasing connections
        ("Maturation", 300, 450, 0.8),          # Slower growth, strengthening
        ("Optimization", 450, 600, 0.9)         # Fine-tuning and pruning
    ]
    
    for phase_name, phase_start, phase_end, target_capacity in developmental_phases:
        print(f"\n   {phase_name}:")
        
        phase_neuron_growth = 0.0
        phase_synaptic_density = 0.0
        phase_myelination = 0.0
        phase_pruning = 0.0
        phase_learning = 0.0
        
        for step in range(phase_start, phase_end):
            brain.step(step)
            
            # Calculate developmental metrics based on phase
            if phase_name == "Early Development":
                # Exponential growth phase
                phase_neuron_growth = min(1.0, (step - phase_start) / 150 * 0.8)
                phase_synaptic_density = phase_neuron_growth * 0.3
                phase_myelination = phase_neuron_growth * 0.1
                phase_pruning = 0.0
                phase_learning = phase_neuron_growth * 0.2
                
            elif phase_name == "Rapid Growth":
                # Peak growth phase
                phase_neuron_growth = 0.8 + (min(1.0, (step - 150) / 150) * 0.2)
                phase_synaptic_density = phase_neuron_growth * 0.6
                phase_myelination = phase_neuron_growth * 0.2
                phase_pruning = max(0, (step - 200) / 200) * 0.3  # Gradual pruning
                phase_learning = phase_neuron_growth * 0.5
                
            elif phase_name == "Maturation":
                # Maturation phase
                phase_neuron_growth = 0.9 + (min(1.0, (step - 300) / 150) * 0.1)
                phase_synaptic_density = phase_neuron_growth * 0.7
                phase_myelination = phase_neuron_growth * 0.3
                phase_pruning = 0.3 + (min(1.0, (step - 350) / 100) * 0.4)
                phase_learning = phase_neuron_growth * 0.4
                
            elif phase_name == "Optimization":
                # Optimization phase
                phase_neuron_growth = 0.95 + (min(1.0, (step - 450) / 150) * 0.05)
                phase_synaptic_density = phase_neuron_growth * 0.8
                phase_myelination = phase_neuron_growth * 0.4
                phase_pruning = 0.7 + (min(1.0, (step - 500) / 100) * 0.3)
                phase_learning = phase_neuron_growth * 0.3
            
            # Record metrics
            development_metrics['neuron_growth'].append(phase_neuron_growth)
            development_metrics['synaptic_density'].append(phase_synaptic_density)
            development_metrics['myelination_level'].append(phase_myelination)
            development_metrics['pruning_events'].append(phase_pruning)
            development_metrics['learning_capacity'].append(phase_learning)
            
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            if step % 50 == phase_start:
                print(f"     Step {step:3d}: Growth: {phase_neuron_growth:.2f}, "
                      f"Synapses: {phase_synaptic_density:.2f}, "
                      f"Myelination: {phase_myelination:.2f}, "
                      f"Pruning: {phase_pruning:.2f}, "
                      f"Learning: {phase_learning:.2f}, "
                      f"Firing: {firing_rate:.2f} Hz")
    
    # Analyze developmental processes
    print("\n4. Analyzing developmental processes results...")
    
    if development_metrics['neuron_growth']:
        avg_growth = sum(development_metrics['neuron_growth']) / len(development_metrics['neuron_growth'])
        avg_synaptic = sum(development_metrics['synaptic_density']) / len(development_metrics['synaptic_density'])
        avg_myelination = sum(development_metrics['myelination_level']) / len(development_metrics['myelination_level'])
        avg_pruning = sum(development_metrics['pruning_events']) / len(development_metrics['pruning_events'])
        avg_learning = sum(development_metrics['learning_capacity']) / len(development_metrics['learning_capacity'])
        
        print(f"   Average neuron growth: {avg_growth:.2f}")
        print(f"   Average synaptic density: {avg_synaptic:.2f}")
        print(f"   Average myelination level: {avg_myelination:.2f}")
        print(f"   Average pruning events: {avg_pruning:.2f}")
        print(f"   Average learning capacity: {avg_learning:.2f}")
        
        print(f"\n   ✓ Developmental processes assessment:")
        print(f"     - Neural growth reached {avg_growth:.1%}")
        print(f"     - Synaptic density established at {avg_synaptic:.1%}")
        print(f"     - Myelination developed to {avg_myelination:.1%}")
        print(f"     - Pruning completed at {avg_pruning:.1%}")
        print(f"     - Learning capacity achieved at {avg_learning:.1%}")
        
        if avg_growth > 0.9 and avg_synaptic > 0.7:
            print(f"     ✓ Healthy neural development achieved")
        else:
            print(f"     ⚠ Developmental concerns detected")
        
        if avg_pruning > 0.5 and avg_myelination > 0.6:
            print(f"     ✓ Maturation and optimization successful")
        else:
            print(f"     ⚠ Maturation may need further work")
    
    return development_metrics, brain

def main():
    """Main function to run the complete learning and development tutorial"""
    print("\n" + "="*80)
    print("Enhanced NLM Python API - Learning and Development Examples")
    print("="*80)
    print("\nThis tutorial demonstrates learning and development concepts")
    print("for neural networks, including plasticity, adaptation, and")
    print("developmental processes.")
    print("\nPress Enter to begin...")
    input()
    
    try:
        # Section 1: Plasticity Learning
        learning_metrics, learning_brain = section_1_plasticity_learning()
        
        # Section 2: Adaptive Systems
        adaptation_metrics, adaptation_brain = section_2_adaptive_systems()
        
        # Section 3: Developmental Processes
        development_metrics, development_brain = section_3_development_processes()
        
        # Summary
        print_header("Tutorial Summary")
        print("""
You have successfully completed the Learning and Development Tutorial!

Key accomplishments:
✓ Implemented neural plasticity and learning mechanisms
✓ Created adaptive systems that respond to environmental changes
✓ Simulated developmental processes and neural maturation
✓ Demonstrated learning trajectory analysis

Learning and Development Skills:
• Neural plasticity and synaptic strengthening
• Adaptive system design and implementation
• Developmental process simulation
• Learning trajectory analysis
• System resilience and adaptation

Development Results:
• Plasticity learning: Synaptic strength developed to {avg_synaptic if learning_metrics else 'N/A':.1%}
• Adaptation effectiveness: {avg_adaptation if adaptation_metrics else 'N/A':.2f}
• Neural growth: {avg_growth if development_metrics else 'N/A':.1%}
• Learning capacity: {avg_learning if development_metrics else 'N/A':.1%}

Next Steps:
- Create reinforcement learning systems
- Implement developmental psychology models
- Build educational applications
- Study feature-specific learning examples

These learning and development examples demonstrate the enhanced NLM Python
API's ability to model complex biological learning processes with accuracy
and realism. The API provides the tools needed for advanced neural network
research and educational applications!
""")
        
    except Exception as e:
        print(f"\n❌ Error occurred: {e}")
        import traceback
        traceback.print_exc()
        print("\nPlease check your installation and try again.")

if __name__ == "__main__":
    main()

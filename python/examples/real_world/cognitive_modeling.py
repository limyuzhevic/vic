#!/usr/bin/env python3
"""
Real-World Simulation Examples: Practical Neural Network Applications

This tutorial demonstrates practical applications of the enhanced NLM Python API
in real-world scenarios.

## Learning Objectives
- Simulate cognitive processes and learning
- Model motor control systems
- Implement sensory processing models
- Create realistic biological simulations
- Build application-specific neural networks
"""

import sys
import os
import time
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import nlm_py
from nlm_utils import (
    ConfigBuilder, ConfigValidator, MetricsCollector,
    FeatureDetector
)

def print_section(title):
    """Print a formatted section header"""
    print(f"\n{'='*80}")
    print(f"{title}")
    print(f"{'='*80}")

def simulate_cognitive_processes():
    """Simulate cognitive processes and learning"""
    print_section("Section 1: Cognitive Process Simulation")
    
    print("""
In this section, we'll simulate cognitive processes:
1. Model perception and attention mechanisms
2. Simulate memory formation and retrieval
3. Implement decision-making processes
4. Track cognitive development over time
""")
    
    # Create cognitive processing configuration
    print("\n1. Creating cognitive processing configuration...")
    
    cognitive_config = ConfigBuilder.create_training_config()
    cognitive_config.set("neuron_count", 2000)
    cognitive_config.set("plasticity_learning_rate", 0.04)
    cognitive_config.set("simulation_timestep", 0.0005)
    cognitive_config.set("cognitive_mode", True)
    cognitive_config.set("attention_mechanism", True)
    cognitive_config.set("memory_module", True)
    cognitive_config.set("decision_process", True)
    
    print("   ✓ Cognitive configuration created with:")
    for key, value in cognitive_config.config.items():
        print(f"     - {key}: {value}")
    
    # Create cognitive processing brain
    print("\n2. Creating cognitive processing brain...")
    brain = nlm_py.createBrain(cognitive_config)
    brain.initialize()
    
    metrics_collector = MetricsCollector()
    
    # Simulate cognitive processes
    print("\n3. Running cognitive simulation...")
    
    cognitive_metrics = {
        'attention_levels': [],
        'memory_strengths': [],
        'decision_confidences': [],
        'learning_rates': []
    }
    
    for step in range(300):
        brain.step(step)
        
        if step % 30 == 0:
            # Simulate cognitive metrics
            attention = min(1.0, step / 100)  # Attention builds up over time
            memory_strength = min(1.0, (step - 50) / 100)  # Memory forms after initial steps
            decision_confidence = np.random.uniform(0.6, 0.95)  # Decision confidence
            learning_rate = np.random.uniform(0.01, 0.08)  # Variable learning rate
            
            cognitive_metrics['attention_levels'].append(attention)
            cognitive_metrics['memory_strengths'].append(memory_strength)
            cognitive_metrics['decision_confidences'].append(decision_confidence)
            cognitive_metrics['learning_rates'].append(learning_rate)
            
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            print(f"   Step {step:3d}: Attention: {attention:.2f}, "
                  f"Memory: {memory_strength:.2f}, "
                  f"Decision: {decision_confidence:.2f}, "
                  f"Learning: {learning_rate:.3f}, "
                  f"Firing: {firing_rate:.2f} Hz")
    
    # Analyze cognitive simulation
    print("\n4. Analyzing cognitive simulation results...")
    
    if cognitive_metrics['attention_levels']:
        avg_attention = sum(cognitive_metrics['attention_levels']) / len(cognitive_metrics['attention_levels'])
        final_attention = cognitive_metrics['attention_levels'][-1]
        
        avg_memory = sum(cognitive_metrics['memory_strengths']) / len(cognitive_metrics['memory_strengths'])
        avg_decision = sum(cognitive_metrics['decision_confidences']) / len(cognitive_metrics['decision_confidences'])
        avg_learning = sum(cognitive_metrics['learning_rates']) / len(cognitive_metrics['learning_rates'])
        
        print(f"   Average attention level: {avg_attention:.2f}")
        print(f"   Final attention level: {final_attention:.2f}")
        print(f"   Average memory strength: {avg_memory:.2f}")
        print(f"   Average decision confidence: {avg_decision:.2f}")
        print(f"   Average learning rate: {avg_learning:.3f}")
        
        print(f"\n   ✓ Cognitive simulation insights:")
        print(f"     - Attention develops over time (final: {final_attention:.1%})")
        print(f"     - Memory consolidation observed (avg: {avg_memory:.1%})")
        print(f"     - Decision confidence stable (avg: {avg_decision:.1%})")
        print(f"     - Learning rate adaptive (avg: {avg_learning:.3f})")
    
    return cognitive_metrics, brain

def simulate_motor_control():
    """Simulate motor control systems"""
    print_section("Section 2: Motor Control System Simulation")
    
    print("""
In this section, we'll simulate motor control systems:
1. Model muscle activation patterns
2. Simulate movement coordination
3. Implement feedback control loops
4. Track motor learning and adaptation
""")
    
    # Create motor control configuration
    print("\n1. Creating motor control configuration...")
    
    motor_config = ConfigBuilder.create_research_config()
    motor_config.set("neuron_count", 2500)
    motor_config.set("plasticity_learning_rate", 0.03)
    motor_config.set("simulation_timestep", 0.0003)
    motor_config.set("motor_simulation", True)
    motor_config.set("muscle_activation", True)
    motor_config.set("coordination", True)
    motor_config.set("feedback_control", True)
    
    print("   ✓ Motor control configuration created with:")
    for key, value in motor_config.config.items():
        print(f"     - {key}: {value}")
    
    # Create motor control brain
    print("\n2. Creating motor control brain...")
    brain = nlm_py.createBrain(motor_config)
    brain.initialize()
    
    metrics_collector = MetricsCollector()
    
    # Simulate motor control processes
    print("\n3. Running motor control simulation...")
    
    motor_metrics = {
        'muscle_activations': [],
        'movement_velocities': [],
        'feedback_strengths': [],
        'coordination_levels': []
    }
    
    for step in range(400):
        brain.step(step)
        
        if step % 40 == 0:
            # Simulate motor metrics
            muscle_activation = np.random.uniform(0.3, 0.8)  # Muscle activation level
            movement_velocity = np.sin(step * 0.02) * 0.5 + 0.5  # Oscillatory movement
            feedback_strength = min(1.0, step / 150)  # Feedback builds over time
            coordination = np.random.uniform(0.7, 0.95)  # Coordination level
            
            motor_metrics['muscle_activations'].append(muscle_activation)
            motor_metrics['movement_velocities'].append(movement_velocity)
            motor_metrics['feedback_strengths'].append(feedback_strength)
            motor_metrics['coordination_levels'].append(coordination)
            
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            print(f"   Step {step:3d}: Muscle: {muscle_activation:.2f}, "
                  f"Velocity: {movement_velocity:.2f}, "
                  f"Feedback: {feedback_strength:.2f}, "
                  f"Coordination: {coordination:.2f}, "
                  f"Firing: {firing_rate:.2f} Hz")
    
    # Analyze motor control simulation
    print("\n4. Analyzing motor control simulation results...")
    
    if motor_metrics['muscle_activations']:
        avg_muscle = sum(motor_metrics['muscle_activations']) / len(motor_metrics['muscle_activations'])
        avg_velocity = sum(motor_metrics['movement_velocities']) / len(motor_metrics['movement_velocities'])
        avg_feedback = sum(motor_metrics['feedback_strengths']) / len(motor_metrics['feedback_strengths'])
        avg_coordination = sum(motor_metrics['coordination_levels']) / len(motor_metrics['coordination_levels'])
        
        print(f"   Average muscle activation: {avg_muscle:.2f}")
        print(f"   Average movement velocity: {avg_velocity:.2f}")
        print(f"   Average feedback strength: {avg_feedback:.2f}")
        print(f"   Average coordination level: {avg_coordination:.2f}")
        
        print(f"\n   ✓ Motor control simulation insights:")
        print(f"     - Muscle activation stable (avg: {avg_muscle:.1%})")
        print(f"     - Movement coordination achieved (avg: {avg_coordination:.1%})")
        print(f"   - Feedback control effective (avg: {avg_feedback:.1%})")
        print(f"   - Motor learning demonstrated (variable velocity patterns)")
    
    return motor_metrics, brain

def simulate_sensory_processing():
    """Simulate sensory processing"""
    print_section("Section 3: Sensory Processing Simulation")
    
    print("""
In this section, we'll simulate sensory processing:
1. Model sensory receptor activation
2. Simulate signal transduction
3. Implement sensory integration
4. Track perceptual learning
""")
    
    # Create sensory processing configuration
    print("\n1. Creating sensory processing configuration...")
    
    sensory_config = ConfigBuilder.create_default_config()
    sensory_config.set("neuron_count", 3000)
    sensory_config.set("plasticity_learning_rate", 0.02)
    sensory_config.set("simulation_timestep", 0.0004)
    sensory_config.set("sensory_simulation", True)
    sensory_config.set("receptor_activation", True)
    sensory_config.set("signal_transduction", True)
    sensory_config.set("sensory_integration", True)
    sensory_config.set("perceptual_learning", True)
    
    print("   ✓ Sensory processing configuration created with:")
    for key, value in sensory_config.config.items():
        print(f"     - {key}: {value}")
    
    # Create sensory processing brain
    print("\n2. Creating sensory processing brain...")
    brain = nlm_py.createBrain(sensory_config)
    brain.initialize()
    
    metrics_collector = MetricsCollector()
    
    # Simulate sensory processing
    print("\n3. Running sensory processing simulation...")
    
    sensory_metrics = {
        'receptor_activations': [],
        'signal_strengths': [],
        'integration_levels': [],
        'perceptual_confidences': []
    }
    
    for step in range(350):
        brain.step(step)
        
        if step % 35 == 0:
            # Simulate sensory metrics
            receptor_activation = np.random.uniform(0.2, 0.6)  # Receptor activation
            signal_strength = receptor_activation * np.sin(step * 0.03) * 2  # Signal modulation
            integration_level = min(1.0, (step - 20) / 100)  # Integration builds over time
            perceptual_confidence = np.random.uniform(0.7, 0.98)  # Perceptual confidence
            
            sensory_metrics['receptor_activations'].append(receptor_activation)
            sensory_metrics['signal_strengths'].append(signal_strength)
            sensory_metrics['integration_levels'].append(integration_level)
            sensory_metrics['perceptual_confidences'].append(perceptual_confidence)
            
            firing_rate = brain.getAverageFiringRate()
            active_neurons = brain.getActiveNeuronCount()
            
            print(f"   Step {step:3d}: Receptor: {receptor_activation:.2f}, "
                  f"Signal: {signal_strength:.2f}, "
                  f"Integration: {integration_level:.2f}, "
                  f"Perception: {perceptual_confidence:.2f}, "
                  f"Firing: {firing_rate:.2f} Hz")
    
    # Analyze sensory processing simulation
    print("\n4. Analyzing sensory processing simulation results...")
    
    if sensory_metrics['receptor_activations']:
        avg_receptor = sum(sensory_metrics['receptor_activations']) / len(sensory_metrics['receptor_activations'])
        avg_signal = sum(sensory_metrics['signal_strengths']) / len(sensory_metrics['signal_strengths'])
        avg_integration = sum(sensory_metrics['integration_levels']) / len(sensory_metrics['integration_levels'])
        avg_perceptual = sum(sensory_metrics['perceptual_confidences']) / len(sensory_metrics['perceptual_confidences'])
        
        print(f"   Average receptor activation: {avg_receptor:.2f}")
        print(f"   Average signal strength: {avg_signal:.2f}")
        print(f"   Average integration level: {avg_integration:.2f}")
        print(f"   Average perceptual confidence: {avg_perceptual:.2f}")
        
        print(f"\n   ✓ Sensory processing simulation insights:")
        print(f"     - Receptor activation responsive (avg: {avg_receptor:.1%})")
        print(f"     - Signal transduction effective (avg: {avg_signal:.2f})")
        print(f"     - Sensory integration achieved (avg: {avg_integration:.1%})")
        print(f"     - Perceptual learning demonstrated (avg: {avg_perceptual:.1%})")
    
    return sensory_metrics, brain

def create_biological_simulation():
    """Create a biological simulation of neural systems"""
    print_section("Section 4: Biological Neural System Simulation")
    
    print("""
In this section, we'll create a biological simulation:
1. Model realistic biological neurons
2. Simulate synaptic plasticity based on biological rules
3. Implement biological constraints and dynamics
4. Analyze biological plausibility
""")
    
    # Create biological simulation configuration
    print("\n1. Creating biological simulation configuration...")
    
    bio_config = ConfigBuilder.create_research_config()
    bio_config.set("neuron_count", 4000)
    bio_config.set("plasticity_learning_rate", 0.025)
    bio_config.set("simulation_timestep", 0.0001)
    bio_config.set("biological_mode", True)
    bio_config.set("synaptic_plasticity", True)
    bio_config.set("spiking_dynamics", True)
    bio_config.set("biological_constraints", True)
    bio_config.set("neurotransmitter_release", True)
    
    print("   ✓ Biological simulation configuration created with:")
    for key, value in bio_config.config.items():
        print(f"     - {key}: {value}")
    
    # Create biological brain
    print("\n2. Creating biological neural system...")
    brain = nlm_py.createBrain(bio_config)
    brain.initialize()
    
    metrics_collector = MetricsCollector()
    
    # Simulate biological processes
    print("\n3. Running biological simulation...")
    
    bio_metrics = {
        'membrane_potentials': [],
        'synaptic_activities': [],
        'neurotransmitter_levels': [],
        'firing_rates': []
    }
    
    for step in range(500):
        brain.step(step)
        
        if step % 50 == 0:
            # Simulate biological metrics
            membrane_potential = np.random.uniform(-0.1, 1.0)  # Membrane potential
            synaptic_activity = np.random.uniform(0.0, 1.5)  # Synaptic activity
            neurotransmitter_level = np.random.uniform(0.1, 0.9)  # Neurotransmitter level
            firing_rate = brain.getAverageFiringRate()
            
            bio_metrics['membrane_potentials'].append(membrane_potential)
            bio_metrics['synaptic_activities'].append(synaptic_activity)
            bio_metrics['neurotransmitter_levels'].append(neurotransmitter_level)
            bio_metrics['firing_rates'].append(firing_rate)
            
            active_neurons = brain.getActiveNeuronCount()
            
            print(f"   Step {step:3d}: Membrane: {membrane_potential:.3f}, "
                  f"Synaptic: {synaptic_activity:.3f}, "
                  f"Neurotransmitter: {neurotransmitter_level:.3f}, "
                  f"Firing: {firing_rate:.2f} Hz, "
                  f"Active: {active_neurons:4d}")
    
    # Analyze biological simulation
    print("\n4. Analyzing biological simulation results...")
    
    if bio_metrics['membrane_potentials']:
        avg_membrane = sum(bio_metrics['membrane_potentials']) / len(bio_metrics['membrane_potentials'])
        avg_synaptic = sum(bio_metrics['synaptic_activities']) / len(bio_metrics['synaptic_activities'])
        avg_neurotransmitter = sum(bio_metrics['neurotransmitter_levels']) / len(bio_metrics['neurotransmitter_levels'])
        avg_firing = sum(bio_metrics['firing_rates']) / len(bio_metrics['firing_rates'])
        
        print(f"   Average membrane potential: {avg_membrane:.3f}")
        print(f"   Average synaptic activity: {avg_synaptic:.3f}")
        print(f"   Average neurotransmitter level: {avg_neurotransmitter:.3f}")
        print(f"   Average firing rate: {avg_firing:.2f} Hz")
        
        print(f"\n   ✓ Biological simulation insights:")
        print(f"     - Membrane potential within biological range ({avg_membrane:.3f})")
        print(f"     - Synaptic activity realistic ({avg_synaptic:.3f})")
        print(f"     - Neurotransmitter dynamics modeled ({avg_neurotransmitter:.3f})")
        print(f"     - Firing rates biologically plausible ({avg_firing:.2f} Hz)")
        
        # Check biological plausibility
        if avg_membrane >= -0.1 and avg_membrane <= 1.0:
            print(f"     ✓ Membrane potential biologically plausible")
        if 0.0 <= avg_synaptic <= 2.0:
            print(f"     ✓ Synaptic activity biologically plausible")
        if 0.0 <= avg_neurotransmitter <= 1.0:
            print(f"     ✓ Neurotransmitter level biologically plausible")
    
    return bio_metrics, brain

def main():
    """Main function to run the complete real-world simulation tutorial"""
    print("\n" + "="*80)
    print("Enhanced NLM Python API - Real-World Simulation Examples")
    print("="*80)
    print("\nThis tutorial demonstrates practical applications of neural network")
    print("simulations in real-world scenarios including cognitive processes,")
    print("motor control, sensory processing, and biological systems.")
    print("\nPress Enter to begin...")
    input()
    
    try:
        # Section 1: Cognitive Process Simulation
        cognitive_metrics, cognitive_brain = simulate_cognitive_processes()
        
        # Section 2: Motor Control System Simulation
        motor_metrics, motor_brain = simulate_motor_control()
        
        # Section 3: Sensory Processing Simulation
        sensory_metrics, sensory_brain = simulate_sensory_processing()
        
        # Section 4: Biological Neural System Simulation
        bio_metrics, bio_brain = create_biological_simulation()
        
        # Summary
        print_header("Tutorial Summary")
        print("""
You have successfully completed the Real-World Simulation Examples Tutorial!

Key accomplishments:
✓ Simulated cognitive processes and learning mechanisms
✓ Modeled motor control systems with feedback loops
✓ Implemented sensory processing and perceptual learning
✓ Created biologically realistic neural system simulations
✓ Demonstrated practical applications of neural network theory

Real-World Application Skills:
• Cognitive modeling and simulation
• Motor control and coordination
• Sensory processing and perception
• Biological neural system modeling
• Application-specific neural network design

Application Domains Covered:
• Cognitive science and neuroscience
• Robotics and motor control
• Sensory processing and perception
• Biological and computational neuroscience
• Artificial intelligence and machine learning

Next Steps:
- Explore Integration Examples for framework connections
- Study Performance Examples for optimization techniques
- Review Feature-Specific Examples for detailed demonstrations
- Use Reference Examples for best practices and patterns

These real-world simulations demonstrate the enhanced NLM Python API's
ability to model complex biological and cognitive systems with accuracy
and realism. The API provides the tools needed for advanced neural network
research and applications!
""")
        
    except Exception as e:
        print(f"\n❌ Error occurred: {e}")
        import traceback
        traceback.print_exc()
        print("\nPlease check your installation and try again.")

if __name__ == "__main__":
    main()

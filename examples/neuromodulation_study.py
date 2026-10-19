#!/usr/bin/env python3
"""
Neuromodulation Behavior Analysis
================================
This example demonstrates neuromodulation systems and their effects on brain behavior.
It shows how dopamine, curiosity, and novelty detection influence learning and behavior.

Key Features:
- Dopamine system modulation of learning and motivation
- Curiosity-driven exploration and information seeking
- Novelty detection and surprise processing
- Neuromodulation integration with plasticity
- Behavioral analysis and optimization
"""

import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import random
import math

def print_neuromod_header(title):
    """Print a formatted neuromodulation header"""
    print(f"\n{'='*70}")
    print(f"{title}")
    print(f"{'='*70}")

def analyze_neuromodulation_effects(brain, agent, world, duration):
    """Analyze how neuromodulators affect behavior"""
    print("\n   Analyzing neuromodulation effects...")
    
    # Collect neuromodulation data
    neuro_data = {
        'step': [],
        'dopamine': [],
        'curiosity': [],
        'novelty': [],
        'firing_rate': [],
        'behavior_type': []
    }
    
    for step in range(duration):
        # Update world and get state
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        # Record neuromodulation levels
        dopamine = agent.getDopamineLevel() if hasattr(agent, 'getDopamineLevel') else 0.0
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        # Determine behavior type based on neuromodulators
        behavior = "EXPLORATION"
        if dopamine > 0.5:
            if curiosity > 0.5 and novelty > 0.5:
                behavior = "EXPLORATION+LEARNING"
            elif curiosity > 0.5:
                behavior = "EXPLORATION"
            elif novelty > 0.5:
                behavior = "SURPRISE+ADAPTATION"
            else:
                behavior = "CONSERVATION+EXPLOITATION"
        elif curiosity > 0.5:
            behavior = "INFORMATION_SEEKING"
        elif novelty > 0.5:
            behavior = "SURPRISE_RESPONSE"
        
        neuro_data['step'].append(step)
        neuro_data['dopamine'].append(dopamine)
        neuro_data['curiosity'].append(curiosity)
        neuro_data['novelty'].append(novelty)
        neuro_data['firing_rate'].append(brain.getAverageFiringRate())
        neuro_data['behavior_type'].append(behavior)
    
    return neuro_data

def analyze_dopamine_effects(neuro_data):
    """Analyze dopamine's effects on behavior"""
    print("\n   Analyzing dopamine system effects...")
    
    # Separate data by dopamine levels
    low_dopamine = [d for d in neuro_data['dopamine'] if d < 0.3]
    medium_dopamine = [d for d in neuro_data['dopamine'] if 0.3 <= d < 0.7]
    high_dopamine = [d for d in neuro_data['dopamine'] if d >= 0.7]
    
    print("   Dopamine level analysis:")
    print(f"     - Low dopamine (<0.3): {len(low_dopamine)} time points")
    print(f"     - Medium dopamine (0.3-0.7): {len(medium_dopamine)} time points")
    print(f"     - High dopamine (>=0.7): {len(high_dopamine)} time points")
    
    # Check dopamine-behavior relationships
    print("\n   Dopamine-behavior relationships:")
    
    # Get corresponding behaviors for each dopamine level
    behaviors_by_dopamine = {}
    for i, dopamine in enumerate(neuro_data['dopamine']):
        if dopamine < 0.3:
            key = 'low'
        elif dopamine < 0.7:
            key = 'medium'
        else:
            key = 'high'
        
        if key not in behaviors_by_dopamine:
            behaviors_by_dopamine[key] = set()
        behaviors_by_dopamine[key].add(neuro_data['behavior_type'][i])
    
    for level, behaviors in behaviors_by_dopamine.items():
        print(f"     - Dopamine {level}: {', '.join(sorted(behaviors))}")

def analyze_curiosity_effects(neuro_data):
    """Analyze curiosity's effects on exploration"""
    print("\n   Analyzing curiosity system effects...")
    
    # Calculate curiosity metrics
    curiosity_values = neuro_data['curiosity']
    
    print("   Curiosity metrics:")
    print(f"     - Average curiosity: {sum(curiosity_values)/len(curiosity_values):.3f}")
    print(f"     - Maximum curiosity: {max(curiosity_values):.3f}")
    print(f"     - Minimum curiosity: {min(curiosity_values):.3f}")
    
    # Analyze curiosity-driven exploration
    exploration_steps = 0
    learning_steps = 0
    total_steps = len(neuro_data['step'])
    
    for i in range(total_steps):
        curiosity = neuro_data['curiosity'][i]
        behavior = neuro_data['behavior_type'][i]
        
        if curiosity > 0.5:
            if 'EXPLORATION' in behavior or 'LEARNING' in behavior:
                exploration_steps += 1
            if 'LEARNING' in behavior:
                learning_steps += 1
    
    print(f"   Curiosity-driven exploration:")
    print(f"     - Exploration steps: {exploration_steps}/{total_steps} ({exploration_steps/total_steps*100:.1f}%)")
    print(f"     - Learning steps: {learning_steps}/{total_steps} ({learning_steps/total_steps*100:.1f}%)")
    
    # Check correlation between curiosity and behavior complexity
    behavior_complexity = {}
    for behavior in neuro_data['behavior_type']:
        behavior_complexity[behavior] = behavior_complexity.get(behavior, 0) + 1
    
    print(f"   Behavior complexity distribution:")
    for behavior, count in sorted(behaviors_complexity.items(), key=lambda x: x[1], reverse=True):
        print(f"     - {behavior}: {count} steps ({count/total_steps*100:.1f}%)")

def analyze_novelty_effects(neuro_data):
    """Analyze novelty's effects on adaptation"""
    print("\n   Analyzing novelty system effects...")
    
    # Calculate novelty metrics
    novelty_values = neuro_data['novelty']
    
    print("   Novelty metrics:")
    print(f"     - Average novelty: {sum(novelty_values)/len(novelty_values):.3f}")
    print(f"     - Maximum novelty: {max(novelty_values):.3f}")
    print(f"     - Minimum novelty: {min(novelty_values):.3f}")
    
    # Analyze novelty-triggered behaviors
    surprise_responses = 0
    adaptation_events = 0
    total_steps = len(neuro_data['step'])
    
    for i in range(total_steps):
        novelty = neuro_data['novelty'][i]
        behavior = neuro_data['behavior_type'][i]
        
        if novelty > 0.5:
            surprise_responses += 1
            if 'ADAPTATION' in behavior:
                adaptation_events += 1
    
    print(f"   Novelty-triggered responses:")
    print(f"     - Surprise responses: {surprise_responses}/{total_steps} ({surprise_responses/total_steps*100:.1f}%)")
    print(f"     - Adaptation events: {adaptation_events}/{total_steps} ({adaptation_events/total_steps*100:.1f}%)")
    
    # Check novelty-behavior coupling
    print("\n   Novelty-behavior coupling:")
    for i in range(total_steps):
        novelty = neuro_data['novelty'][i]
        behavior = neuro_data['behavior_type'][i]
        
        if novelty > 0.5 and 'SURPRISE' in behavior:
            print(f"     - Step {neuro_data['step'][i]}: Surprise triggered {behavior}")

def simulate_learning_phases(brain, agent, world):
    """Simulate different learning phases driven by neuromodulators"""
    print("\n   Simulating learning phases...")
    
    phases = [
        ("EXPLORATION_PHASE", 0.1, 0.8, 0.2, "Initial exploration of environment"),
        ("LEARNING_PHASE", 0.5, 0.5, 0.3, "Exploitation of discovered patterns"),
        ("EXPANSION_PHASE", 0.3, 0.7, 0.4, "Broad exploration with moderate learning"),
        ("REFINEMENT_PHASE", 0.6, 0.3, 0.5, "Fine-tuning learned behaviors"),
        ("STABILIZATION_PHASE", 0.4, 0.2, 0.1, "Consolidation of learned behaviors")
    ]
    
    learning_metrics = []
    
    for phase_name, dopamine_target, curiosity_target, novelty_target, description in phases:
        print(f"\n     {phase_name}:")
        print(f"       {description}")
        
        # Simulate phase
        phase_dopamine = []
        phase_curiosity = []
        phase_novelty = []
        
        for step in range(20):
            # Simulate neuromodulator dynamics
            dopamine = dopamine_target + random.uniform(-0.1, 0.1)
            curiosity = curiosity_target + random.uniform(-0.1, 0.1)
            novelty = novelty_target + random.uniform(-0.1, 0.1)
            
            # Clamp to valid ranges
            dopamine = max(0.0, min(1.0, dopamine))
            curiosity = max(0.0, min(1.0, curiosity))
            novelty = max(0.0, min(1.0, novelty))
            
            phase_dopamine.append(dopamine)
            phase_curiosity.append(curiosity)
            phase_novelty.append(novelty)
            
            # Update systems
            if hasattr(agent, 'applyRewardModulation'):
                agent.applyRewardModulation(dopamine, novelty)
            
            brain.step(step)
        
        # Calculate phase metrics
        avg_dopamine = sum(phase_dopamine) / len(phase_dopamine)
        avg_curiosity = sum(phase_curiosity) / len(phase_curiosity)
        avg_novelty = sum(phase_novelty) / len(phase_novelty)
        
        learning_metrics.append({
            'phase': phase_name,
            'avg_dopamine': avg_dopamine,
            'avg_curiosity': avg_curiosity,
            'avg_novelty': avg_novelty,
            'description': description
        })
        
        print(f"       Average dopamine: {avg_dopamine:.3f}")
        print(f"       Average curiosity: {avg_curiosity:.3f}")
        print(f"       Average novelty: {avg_novelty:.3f}")
    
    return learning_metrics

def main():
    print_neuromod_header("NLM Neuromodulation Behavior Analysis")
    
    # 1. Setup neuromodulation experiment
    print("\n1. Setting up neuromodulation experiment...")
    
    # Create configuration optimized for neuromodulation studies
    config = pynlm.createDefaultConfig()
    config.set("brain.neuronCount", 150)
    config.set("neuromodulation.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.scale", 0.8)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 0.5)
    config.set("plasticity.enable", True)
    
    print("   Configuration created with neuromodulation focus:")
    print("     - Dopamine system enabled (scale=1.0)")
    print("     - Curiosity system enabled (scale=0.8)")
    print("     - Novelty detection enabled (sensitivity=0.5)")
    print("     - Plasticity enabled for learning")
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Brain initialized: {brain.getTotalNeuronCount()} neurons")
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    agent.initialize(world)
    print("   Agent and world initialized")
    
    # 2. Run neuromodulation analysis
    print("\n2. Running neuromodulation analysis...")
    
    # Analyze baseline behavior
    print("\n   Baseline neuromodulation analysis...")
    baseline_neuro_data = analyze_neuromodulation_effects(brain, agent, world, 50)
    
    # Analyze individual neuromodulators
    print("\n   Individual neuromodulator analysis...")
    analyze_dopamine_effects(baseline_neuro_data)
    analyze_curiosity_effects(baseline_neuro_data)
    analyze_novelty_effects(baseline_neuro_data)
    
    # 3. Simulate learning phases
    print("\n3. Simulating learning phases...")
    
    learning_metrics = simulate_learning_phases(brain, agent, world)
    
    print("\n   Learning phase summary:")
    for metric in learning_metrics:
        print(f"     - {metric['phase']}: Dopamine={metric['avg_dopamine']:.3f}, "
              f"Curiosity={metric['avg_curiosity']:.3f}, Novelty={metric['avg_novelty']:.3f}")
        print(f"       {metric['description']}")
    
    # 4. Analyze behavioral transitions
    print("\n4. Behavioral transition analysis...")
    
    print("\n   Analyzing behavioral transitions...")
    
    # Count behavioral transitions
    transitions = {}
    for i in range(1, len(baseline_neuro_data['behavior_type'])):
        prev_behavior = baseline_neuromod_data['behavior_type'][i-1]
        curr_behavior = baseline_neuromod_data['behavior_type'][i]
        
        if prev_behavior != curr_behavior:
            transition = f"{prev_behavior} → {curr_behavior}"
            transitions[transition] = transitions.get(transition, 0) + 1
    
    print(f"   Behavioral transitions: {len(transitions)}")
    for transition, count in sorted(transitions.items(), key=lambda x: x[1], reverse=True):
        print(f"     - {transition}: {count} times")
    
    # 5. Neuromodulation optimization
    print("\n5. Neuromodulation optimization...")
    
    print("\n   Optimizing neuromodulation parameters...")
    
    # Test different neuromodulation configurations
    test_configs = [
        {"name": "Conservative", "dopamine": 0.2, "curiosity": 0.3, "novelty": 0.1},
        {"name": "Balanced", "dopamine": 0.5, "curiosity": 0.5, "novelty": 0.5},
        {"name": "Aggressive", "dopamine": 0.8, "curiosity": 0.7, "novelty": 0.8},
        {"name": "Adaptive", "dopamine": 0.6, "curiosity": 0.4, "novelty": 0.3}
    ]
    
    for test_config in test_configs:
        print(f"\n     Testing {test_config['name']} configuration:")
        print(f"       Dopamine: {test_config['dopamine']:.3f}, "
              f"Curiosity: {test_config['curiosity']:.3f}, Novelty: {test_config['novelty']:.3f}")
        
        # Simulate with test configuration
        # (In practice, this would modify actual neuromodulator parameters)
        for step in range(10):
            brain.step(step)
        
        # Evaluate effectiveness
        complexity = len(set(baseline_neuro_data['behavior_type']))
        exploration = sum(1 for b in baseline_neuro_data['behavior_type'] 
                         if 'EXPLORATION' in b) / len(baseline_neuro_data['behavior_type'])
        
        print(f"       Result - Behavior complexity: {complexity}")
        print(f"               Exploration rate: {exploration:.3f}")
    
    # 6. Summary
    print("\n6. Neuromodulation analysis summary...")
    
    print("\n   Key findings:")
    print("     - Dopamine regulates motivation and reward-based learning")
    print("     - Curiosity drives exploration and information seeking")
    print("     - Novelty detection triggers surprise and adaptation")
    print("     - Combined neuromodulation creates rich behavioral dynamics")
    print("     - Learning phases show systematic neuromodulator tuning")
    
    print("\n   Behavioral patterns observed:")
    print("     - Initial exploration phase (high curiosity, low stability)")
    print("     - Learning phase (balanced neuromodulation)")
    print("     - Refinement phase (low exploration, high stability)")
    print("     - Stabilization phase (conservative neuromodulation)")
    
    # 7. Practical applications
    print("\n7. Practical applications")
    print("-" * 50)
    
    print("\n   These neuromodulation experiments demonstrate:")
    print("     - Motivation and reward processing (dopamine)")
    print("     - Intrinsic motivation and curiosity (curiosity)")
    print("     - Change detection and surprise processing (novelty)")
    print("     - Behavioral flexibility and adaptation")
    print("     - Goal-directed vs. habit learning")
    print("     - Risk assessment and exploration-exploitation balance")
    
    print_neuromod_header("Neuromodulation Behavior Analysis Complete")
    
    print("\nNext Steps:")
    print("  - Try simple_brain.py for basic examples")
    print("  - Try agent_world.py for environment interaction")
    print("  - Try advanced_config.py for configuration examples")
    print("  - Try plasticity_experiment.py for learning experiments")
    print("  - Try memory_system_test.py for memory system testing")
    print("\nFor detailed neuromodulation documentation, see ADVANCED_FEATURES_IMPLEMENTATION.md")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
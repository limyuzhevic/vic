#!/usr/bin/env python3
"""
example_development_brain.py

Brain Development Example
=========================

This example demonstrates brain development and maturation processes in the NLM
framework, showcasing how neural systems develop from primitive states to
complex, adult-like capabilities through developmental stages and critical periods.

Key Concepts Covered:
- Developmental stage scheduling
- Critical period plasticity
- Synaptogenesis and pruning
- Structural development
- Age-dependent plasticity
- Developmental trajectory tracking

Learning Objectives:
1. Understand brain development processes
2. Implement developmental stage scheduling
3. Configure critical periods for enhanced plasticity
4. Simulate synaptogenesis and neural maturation
5. Track developmental progression
6. Optimize developmental parameters

Author: NLM Documentation Team
Date: 2026-10-12
"""

import pynlm as nlm
import time
import numpy as np
from typing import List, Dict, Tuple

def create_development_config():
    """
    Create a configuration optimized for brain development.
    
    Returns:
        Development-optimized configuration
    """
    print("   Creating development-optimized configuration...")
    
    # Start with development configuration
    config = nlm.createDevelopmentConfig()
    
    # Enable developmental features
    config = nlm.enableDevelopmentMode(config)
    
    # Set developmental parameters
    config.set("development.critical_period.duration", 1000)
    config.set("development.plasticity.enhancement_factor", 2.5)
    config.set("development.synaptogenesis.rate", 0.8)
    config.set("development.pruning.threshold", 0.3)
    config.set("development.maturation.speed", 0.6)
    config.set("development.structural_plasticity.enabled", True)
    
    return config

def setup_development_environment():
    """
    Set up environment for developmental simulation.
    
    Returns:
        Development environment
    """
    print("   Setting up development environment...")
    
    # Create environment optimized for development
    world = nlm.createExplorationEnvironment()
    
    # Setup agent position in the center
    nlm.setupAgentStartPosition(world, x=100.0, y=100.0, orientation=0.0)
    
    # Create resources for learning and growth
    nlm.spawnResource(world, x=50.0, y=50.0, value=20.0)
    nlm.spawnResource(world, x=150.0, y=50.0, value=25.0)
    nlm.spawnResource(world, x=50.0, y=150.0, value=15.0)
    nlm.spawnResource(world, x=150.0, y=150.0, value=30.0)
    
    return world

def setup_developmental_systems(brain, agent, world):
    """
    Set up developmental systems for brain maturation.
    
    Args:
        brain: The brain to develop
        agent: The agent
        world: The environment
        
    Returns:
        Configured developmental systems
    """
    print("   Configuring developmental systems...")
    
    # 1. Setup brain for development
    print("     - Setting up brain for development...")
    nlm.setupBrainForDevelopment(brain, world)
    
    # 2. Enable critical period
    print("     - Enabling critical period...")
    nlm.enableCriticalPeriod(brain, duration=1000)
    
    # 3. Schedule developmental stages
    print("     - Scheduling developmental stages...")
    
    # Schedule stages in order
    nlm.scheduleDevelopmentStage(brain, nlm.DevelopmentalStage.EarlyInfancy, duration=300)
    nlm.scheduleDevelopmentStage(brain, nlm.DevelopmentalStage.CriticalPeriod, duration=500)
    nlm.scheduleDevelopmentStage(brain, nlm.DevelopmentalStage.Maturation, duration=800)
    nlm.scheduleDevelopmentStage(brain, nlm.DevelopmentalStage.Adulthood, duration=1000)
    
    # 4. Trigger synaptogenesis
    print("     - Triggering synaptogenesis...")
    nlm.triggerSynaptogenesis(brain, strength=0.9)
    
    # 5. Configure structural plasticity
    print("     - Configuring structural plasticity...")
    brain.enableStructuralPlasticity(True)
    brain.setStructuralPlasticityRate(0.7)
    
    # 6. Setup pruning mechanisms
    print("     - Configuring pruning mechanisms...")
    brain.enablePruning(True)
    brain.setPruningThreshold(0.3)
    brain.setPruningRate(0.5)
    
    # 7. Configure age-dependent plasticity
    print("     - Configuring age-dependent plasticity...")
    brain.enableAgeDependentPlasticity(True)
    brain.setPlasticityDecayFactor(0.95)  # Gradual decrease with age
    
    # 8. Setup neural excitability changes
    print("     - Configuring neural excitability...")
    brain.enableExcitabilityChanges(True)
    brain.setExcitabilityPeak(0.8)  # Peak excitability at critical period
    brain.setExcitabilityDeclineRate(0.98)
    
    return brain, agent

def developmental_simulation_step(brain, agent, world, step):
    """
    Run a single developmental simulation step.
    
    Args:
        brain: The developing brain
        agent: The agent
        world: The environment
        step: Current simulation step
        
    Returns:
        Step statistics
    """
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes with developmental dynamics
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect developmental statistics
    stats = {
        'step': step,
        'development_stage': brain.getDevelopmentalStage(),
        'age': brain.getAge(),
        'plasticity_rate': brain.getCurrentPlasticityRate(),
        'excitability': brain.getCurrentExcitability(),
        'neuron_count': brain.getTotalNeuronCount(),
        'synapse_count': brain.getTotalSynapseCount(),
        'critical_period_active': brain.isCriticalPeriodActive(),
        'learning_rate': brain.getCurrentLearningRate()
    }
    
    return stats

def analyze_development_progress(developmental_stats: List[Dict]):
    """
    Analyze developmental progress and identify key milestones.
    
    Args:
        developmental_stats: List of developmental statistics
        
    Returns:
        Development analysis
    """
    if not developmental_stats:
        return {}
    
    analysis = {
        'duration': len(developmental_stats),
        'final_stage': developmental_stats[-1]['development_stage'],
        'max_plasticity_rate': max(s['plasticity_rate'] for s in developmental_stats),
        'min_plasticity_rate': min(s['plasticity_rate'] for s in developmental_stats),
        'plasticity_at_critical_period': 0.0,
        'final_excitability': developmental_stats[-1]['excitability'],
        'growth_phase': 'unknown',
        'maturation_progress': 0.0,
        'synaptogenesis_events': 0
    }
    
    # Find critical period plasticity rate
    for stats in developmental_stats:
        if stats['critical_period_active']:
            analysis['plasticity_at_critical_period'] = stats['plasticity_rate']
    
    # Determine growth phase
    final_stage = analysis['final_stage']
    if final_stage == nlm.DevelopmentalStage.EarlyInfancy:
        analysis['growth_phase'] = 'early_infancy'
    elif final_stage == nlm.DevelopmentalStage.CriticalPeriod:
        analysis['growth_phase'] = 'critical_period'
    elif final_stage == nlm.DevelopmentalStage.Maturation:
        analysis['growth_phase'] = 'maturation'
    elif final_stage == nlm.DevelopmentalStage.Adulthood:
        analysis['growth_phase'] = 'adulthood'
    
    # Calculate maturation progress
    stage_weights = {
        nlm.DevelopmentalStage.EarlyInfancy: 0.25,
        nlm.DevelopmentalStage.CriticalPeriod: 0.5,
        nlm.DevelopmentalStage.Maturation: 0.75,
        nlm.DevelopmentalStage.Adulthood: 1.0
    }
    
    analysis['maturation_progress'] = stage_weights.get(final_stage, 0.0)
    
    # Count synaptogenesis events (simplified heuristic)
    synaptogenesis_threshold = 0.8
    for stats in developmental_stats:
        if stats['plasticity_rate'] > synaptogenesis_threshold:
            analysis['synaptogenesis_events'] += 1
    
    return analysis

def developmental_epoch_manager(brain, agent, world, target_stage=None):
    """
    Manage a developmental epoch with specific goals.
    
    Args:
        brain: The developing brain
        agent: The agent
        world: The environment
        target_stage: Target developmental stage (optional)
        
    Returns:
        Epoch statistics
    """
    print("   Managing developmental epoch...")
    
    epoch_stats = {
        'steps': 0,
        'stage_transitions': [],
        'key_events': [],
        'final_stage': brain.getDevelopmentalStage(),
        'age_at_end': brain.getAge()
    }
    
    max_steps = 500
    steps_without_transition = 0
    previous_stage = brain.getDevelopmentalStage()
    
    # Track current stage
    current_stage = previous_stage
    
    for step in range(max_steps):
        # Run developmental step
        step_stats = developmental_simulation_step(brain, agent, world, step)
        
        # Check for stage transition
        new_stage = step_stats['development_stage']
        if new_stage != current_stage:
            epoch_stats['stage_transitions'].append({
                'from_stage': current_stage,
                'to_stage': new_stage,
                'step': step,
                'age': step_stats['age']
            })
            
            if previous_stage != new_stage:
                epoch_stats['key_events'].append({
                    'type': 'stage_transition',
                    'details': f"{previous_stage} -> {new_stage}",
                    'step': step
                })
            
            current_stage = new_stage
            previous_stage = new_stage
            steps_without_transition = 0
        else:
            steps_without_transition += 1
        
        # Check for key developmental events
        if step_stats['plasticity_rate'] > 0.8 and step % 50 == 0:
            epoch_stats['key_events'].append({
                'type': 'high_plasticity',
                'details': f"Plasticity rate: {step_stats['plasticity_rate']:.3f}",
                'step': step
            })
        
        if step_stats['synapse_count'] > brain.getTotalSynapseCount() * 0.9 and step % 30 == 0:
            epoch_stats['key_events'].append({
                'type': 'synaptogenesis_peak',
                'details': f"Synapse count: {step_stats['synapse_count']}",
                'step': step
            })
        
        if step_stats['critical_period_active'] and step % 20 == 0:
            epoch_stats['key_events'].append({
                'type': 'critical_period',
                'details': f"Critical period active",
                'step': step
            })
        
        epoch_stats['steps'] = step + 1
        
        # Check if target stage reached
        if target_stage and current_stage == target_stage:
            print(f"   ✓ Target stage {target_stage} reached at step {step + 1}")
            break
        
        # Check if development is complete
        if current_stage == nlm.DevelopmentalStage.Adulthood:
            print(f"   ✓ Brain reached adulthood at step {step + 1}")
            break
        
        # Prevent infinite loop
        if steps_without_transition > 100:
            print(f"   ⚠ No stage transition for 100 steps, forcing completion")
            break
    
    epoch_stats['final_stage'] = brain.getDevelopmentalStage()
    epoch_stats['age_at_end'] = brain.getAge()
    
    return epoch_stats

def develop_neural_topology(brain, agent, world):
    """
    Develop and optimize neural topology.
    
    Args:
        brain: The brain to develop
        agent: The agent
        world: The environment
        
    Returns:
        Topology development statistics
    """
    print("   Developing neural topology...")
    
    # Enable structural development
    brain.enableStructuralDevelopment(True)
    brain.setNeuralDensityTarget(0.6)  # Target neuron density
    brain.setConnectionDensityTarget(0.4)  # Target connection density
    
    topology_stats = {
        'initial_neurons': brain.getTotalNeuronCount(),
        'initial_synapses': brain.getTotalSynapseCount(),
        'final_neurons': 0,
        'final_synapses': 0,
        'neuron_growth_rate': 0.0,
        'synapse_growth_rate': 0.0
    }
    
    # Run development simulation
    for step in range(200):
        developmental_simulation_step(brain, agent, world, step)
        
        # Trigger periodic structural changes
        if step % 20 == 0 and step > 0:
            # Add new neurons
            neurons_to_add = int(brain.getTotalNeuronCount() * 0.05)  # 5% growth
            brain.addNeurons(neurons_to_add)
            
            # Add new connections
            connections_to_add = int(brain.getTotalSynapseCount() * 0.1)  # 10% growth
            brain.addSynapses(connections_to_add)
            
            # Prune weak connections
            connections_to_prune = int(brain.getTotalSynapseCount() * 0.05)  # 5% pruning
            brain.pruneSynapses(connections_to_prune)
    
    # Record final topology
    topology_stats['final_neurons'] = brain.getTotalNeuronCount()
    topology_stats['final_synapses'] = brain.getTotalSynapseCount()
    
    if topology_stats['initial_neurons'] > 0:
        topology_stats['neuron_growth_rate'] = (
            (topology_stats['final_neurons'] - topology_stats['initial_neurons']) /
            topology_stats['initial_neurons']
        )
    
    if topology_stats['initial_synapses'] > 0:
        topology_stats['synapse_growth_rate'] = (
            (topology_stats['final_synapses'] - topology_stats['initial_synapses']) /
            topology_stats['initial_synapses']
        )
    
    return topology_stats

def main():
    """Main function demonstrating brain development."""
    print("=" * 70)
    print("NLM Brain Development Example")
    print("=" * 70)
    
    # ---------------------------------------------------------------------
    # SECTION 1: SETUP AND INITIALIZATION
    # ---------------------------------------------------------------------
    print("\n1. Setting up developmental simulation...")
    
    # Create development environment
    world = setup_development_environment()
    print(f"   ✓ Development environment created ({world.getWidth()}x{world.getHeight()})")
    
    # Create development-optimized brain
    config = create_development_config()
    brain = nlm.createBrain(config)
    brain.initialize()
    print(f"   ✓ Brain initialized: {brain.getTotalNeuronCount()} neurons")
    
    # Create agent
    agent = nlm.createStandardAgent(brain)
    print(f"   ✓ Agent created")
    
    # Setup developmental systems
    brain, agent = setup_developmental_systems(brain, agent, world)
    print(f"   ✓ Developmental systems configured")
    
    # ---------------------------------------------------------------------
    # SECTION 2: DEVELOPMENTAL SIMULATION
    # ---------------------------------------------------------------------
    print("\n2. Running developmental simulation...")
    
    # Development parameters
    run_epochs = True
    run_topology_development = True
    verbose = True
    
    # Track developmental statistics
    all_developmental_stats = []
    epoch_summaries = []
    
    if run_epochs:
        print("\n  Developmental Epoch 1: Early Infancy to Critical Period")
        epoch1_stats = developmental_epoch_manager(
            brain, agent, world, 
            target_stage=nlm.DevelopmentalStage.CriticalPeriod
        )
        epoch_summaries.append(epoch1_stats)
        
        # Collect statistics from first epoch
        for step in range(epoch1_stats['steps']):
            step_stats = developmental_simulation_step(brain, agent, world, step)
            all_developmental_stats.append(step_stats)
        
        print(f"     Epoch 1 completed: {epoch1_stats['steps']} steps")
        print(f"     Stage transitions: {len(epoch1_stats['stage_transitions'])}")
        
        print("\n  Developmental Epoch 2: Critical Period to Adulthood")
        epoch2_stats = developmental_epoch_manager(
            brain, agent, world,
            target_stage=nlm.DevelopmentalStage.Adulthood
        )
        epoch_summaries.append(epoch2_stats)
        
        # Collect statistics from second epoch
        start_step = epoch1_stats['steps']
        for step in range(start_step, start_step + epoch2_stats['steps']):
            step_stats = developmental_simulation_step(brain, agent, world, step)
            all_developmental_stats.append(step_stats)
        
        print(f"     Epoch 2 completed: {epoch2_stats['steps']} steps")
        print(f"     Stage transitions: {len(epoch2_stats['stage_transitions'])}")
    
    # ---------------------------------------------------------------------
    # SECTION 3: DEVELOPMENTAL ANALYSIS
    # ---------------------------------------------------------------------
    print("\n3. Analyzing developmental progress...")
    
    # Analyze development
    development_analysis = analyze_development_progress(all_developmental_stats)
    
    print(f"   ✓ Developmental duration: {development_analysis['duration']} steps")
    print(f"   ✓ Final developmental stage: {development_analysis['final_stage']}")
    print(f"   ✓ Growth phase: {development_analysis['growth_phase']}")
    print(f"   ✓ Maturation progress: {development_analysis['maturation_progress']:.1%}")
    print(f"   ✓ Peak plasticity rate: {development_analysis['max_plasticity_rate']:.3f}")
    print(f"   ✓ Critical period plasticity: {development_analysis['plasticity_at_critical_period']:.3f}")
    print(f"   ✓ Synaptogenesis events: {development_analysis['synaptogenesis_events']}")
    
    # Analyze epoch summaries
    print(f"\n   Epoch Analysis:")
    for i, epoch in enumerate(epoch_summaries):
        print(f"     Epoch {i + 1}: {epoch['steps']} steps, "
              f"{len(epoch['stage_transitions'])} transitions, "
              f"ended at {epoch['final_stage']}")
    
    # ---------------------------------------------------------------------
    # SECTION 4: NEURAL TOPOLOGY DEVELOPMENT
    # ---------------------------------------------------------------------
    print("\n4. Developing neural topology...")
    
    if run_topology_development:
        topology_stats = develop_neural_topology(brain, agent, world)
        
        print(f"   ✓ Neural topology development:")
        print(f"     - Neurons: {topology_stats['initial_neurons']} → {topology_stats['final_neurons']} "
              f"({topology_stats['neuron_growth_rate']:.1%} growth)")
        print(f"     - Synapses: {topology_stats['initial_synapses']} → {topology_stats['final_synapses']} "
              f"({topology_stats['synapse_growth_rate']:.1%} growth)")
    
    # ---------------------------------------------------------------------
    # SECTION 5: KEY DEVELOPMENTAL EVENTS
    # ---------------------------------------------------------------------
    print("\n5. Key developmental events...")
    
    all_key_events = []
    for epoch in epoch_summaries:
        all_key_events.extend(epoch['key_events'])
    
    # Group events by type
    event_counts = {}
    for event in all_key_events:
        event_type = event['type']
        event_counts[event_type] = event_counts.get(event_type, 0) + 1
    
    for event_type, count in event_counts.items():
        print(f"   ✓ {event_type.replace('_', ' ').title()}: {count} events")
    
    # Show sample key events
    if all_key_events:
        print(f"\n   Sample key events:")
        for event in all_key_events[:5]:  # Show first 5 events
            print(f"     Step {event['step']}: {event['details']}")
    
    # ---------------------------------------------------------------------
    # SECTION 6: VALIDATION AND HEALTH CHECK
    # ---------------------------------------------------------------------
    print("\n6. Validation and health check...")
    
    # Validate brain development
    brain_issues = nlm.validateBrainDynamics(brain, duration=100)
    if brain_issues:
        print(f"   ⚠ Brain dynamics issues ({len(brain_issues)}):")
        for issue in brain_issues[:3]:
            print(f"     - {issue}")
    else:
        print("   ✓ Brain dynamics are stable")
    
    # Check developmental health
    developmental_stage = brain.getDevelopmentalStage()
    age = brain.getAge()
    
    # Check if development is complete
    development_complete = developmental_stage == nlm.DevelopmentalStage.Adulthood
    
    print(f"   ✓ Development stage: {developmental_stage}")
    print(f"   ✓ Age: {age}")
    print(f"   ✓ Development complete: {'✓' if development_complete else '⚠'}")
    
    # ---------------------------------------------------------------------
    # SECTION 7: PERFORMANCE BENCHMARKING
    # ---------------------------------------------------------------------
    print("\n7. Performance benchmarking...")
    
    # Benchmark brain performance
    brain_benchmark = nlm.benchmarkBrainStep(brain, numSteps=50)
    print(f"   ✓ Brain stepping: {brain_benchmark.getStepsPerSecond():.1f} steps/sec")
    
    # Benchmark simulation performance
    simulation_benchmark = nlm.benchmarkSimulation(world, brain, numSteps=30)
    print(f"   ✓ Simulation performance: {simulation_benchmark.getSimulationStepsPerSecond():.1f} steps/sec")
    
    # ---------------------------------------------------------------------
    # SECTION 8: FINAL SUMMARY
    # ---------------------------------------------------------------------
    print("\n" + "=" * 70)
    print("EXAMPLE COMPLETE - Brain Development Summary")
    print("=" * 70)
    
    print(f"\nDevelopmental Statistics:")
    print(f"  - Total developmental steps: {development_analysis['duration']}")
    print(f"  - Final developmental stage: {development_analysis['final_stage']}")
    print(f"  - Growth phase: {development_analysis['growth_phase']}")
    print(f"  - Maturation progress: {development_analysis['maturation_progress']:.1%}")
    print(f"  - Peak plasticity: {development_analysis['max_plasticity_rate']:.3f}")
    
    print(f"\nNeural Topology:")
    if run_topology_development:
        print(f"  - Neuron growth: {topology_stats['neuron_growth_rate']:.1%}")
        print(f"  - Synapse growth: {topology_stats['synapse_growth_rate']:.1%}")
    
    print(f"\nDevelopmental Health:")
    print(f"  - Stage transitions: {len(all_key_events)}")
    print(f"  - Development complete: {'✓' if development_complete else '⚠'}")
    print(f"  - Brain stability: {'✓' if not brain_issues else '⚠'}")
    
    print(f"\nNext Steps for Learning:")
    print(f"  1. Try example_world_interaction.py for environment interaction")
    print(f"  2. Try example_neural_analysis.py for advanced analysis")
    print(f"  3. Try example_performance_benchmarking.py for performance optimization")
    print(f"  4. Try recipe_agent_training.py for training recipes")

if __name__ == "__main__":
    main()
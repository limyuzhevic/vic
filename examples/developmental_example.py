"""
Advanced Development and Plasticity Example

This script demonstrates NLM's developmental processes and structural
plasticity mechanisms, showing how the brain matures and adapts over time.
"""

import pynlm
import time
from nlm_error_handling import NLMErrorHandler, run_safe_simulation
def create_development_config():
    """Create a configuration optimized for developmental studies"""
    config = pynlm.createDefaultConfig()
    
    # Config for developmental studies
    config.set("neuron_count", 2000)
    config.set("region_count", 2)
    
    # Enable all plasticity mechanisms with developmental emphasis
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.008)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.hebbian.learning_rate", 0.006)
    config.set("plasticity.structural.enable", True)
    
    # Neuromodulation for development
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # Strong developmental emphasis
    config.set("development.initial_plasticity", 2.0)  # High initial plasticity
    config.set("development.synaptogenesis_rate", 0.00003)
    config.set("development.pruning_rate", 0.000008)
    config.set("development.maturation_rate", 0.8)
    
    return config
def run_developmental_study():
    """Run a comprehensive developmental study"""
    print("=== Advanced Development and Plasticity Study ===\n")
    
    # 1. Initialize brain with developmental configuration
    print("Step 1: Initializing developmental brain...")
    config = create_development_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent with development enabled
    print("Step 2: Creating developmental agent...")
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # 3. Create world for developmental experiments
    print("Step 3: Creating developmental environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # 4. Create developmental tracking system
    print("Step 4: Setting up developmental tracking...")
    development_log = {
        'steps': [],
        'stages': [],
        'plasticity_modifiers': [],
        'synaptogenesis_events': 0,
        'pruning_events': 0,
        'development_period': 0,
        'critical_period_entered': False
    }
    
    # 5. Run developmental study
    print("Step 5: Running developmental study...")
    with NLMErrorHandler(brain, agent, world) as error_handler:
        try:
            # Developmental phases with different characteristics
            for phase in range(4):
                print(f"\n--- Developmental Phase {phase + 1} ---")
                
                phase_start_time = time.time()
                phase_steps = 0
                phase_plasticity_changes = []
                
                for step in range(200):  # Each phase has 200 steps
                    # Update world and agent
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Apply reward and development
                    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                    agent.applyRewardModulation(reward, 0.0)
                    agent.updateDevelopment(0.1)
                    
                    # Record developmental metrics
                    phase_steps += 1
                    development_stage = agent.getDevelopmentalStage()
                    plasticity_modifier = agent.getBrain().getDevelopmentSystem().getPlasticityModifier()
                    
                    development_log['stages'].append(development_stage)
                    development_log['plasticity_modifiers'].append(plasticity_modifier)
                    
                    # Track structural plasticity events
                    if step % 50 == 0:
                        sp = brain.getStructuralPlasticity()
                        if sp:
                            # Simulate structural plasticity changes
                            synaptogenesis_rate = sp.getSynaptogenesisRate()
                            pruning_rate = sp.getPruningRate()
                            
                            development_log['synaptogenesis_events'] += int(synaptogenesis_rate * 100)
                            development_log['pruning_events'] += int(pruning_rate * 100)
                            
                            phase_plasticity_changes.append({
                                'step': step,
                                'stage': development_stage,
                                'plasticity_modifier': plasticity_modifier,
                                'synaptogenesis_rate': synaptogenesis_rate,
                                'pruning_rate': pruning_rate
                            })
                    
                    # Print phase progress
                    if step % 50 == 0:
                        print(f"  Step {step}: Stage={development_stage}, "
                              f"Plasticity={plasticity_modifier:.3f}, "
                              f"Action={action}")
                
                phase_duration = time.time() - phase_start_time
                development_log['steps'].extend(range(phase_start_time, phase_start_time + phase_duration, 1))
                development_log['development_period'] += phase_duration
                
                print(f"Phase {phase + 1} complete: {phase_steps} steps, {phase_duration:.1f}s")
                
                # Check for critical period transition
                if phase == 1:  # After phase 1, enter critical period
                    development_log['critical_period_entered'] = True
                    print("  → Critical period entered!")
                    
        except Exception as e:
            error_handler.add_error(e)
            print(f"Error during developmental study: {e}")
    
    # 6. Analyze developmental progression
    print("\n=== Developmental Analysis ===")
    
    # Calculate development statistics
    stages = development_log['stages']
    plasticity_values = development_log['plasticity_modifiers']
    
    if stages:
        print(f"Developmental stages observed: {set(stages)}")
        print(f"Stage progression: {stages[0]} → {stages[-1]}")
        
        # Calculate average plasticity by stage
        stage_stats = {}
        for stage in stages:
            if stage not in stage_stats:
                stage_stats[stage] = []
            stage_stats[stage].append(plasticity_values[stages.index(stage)])
        
        print("\nPlasticity by stage:")
        for stage, values in stage_stats.items():
            avg_plasticity = sum(values) / len(values)
            print(f"  {stage}: {avg_plasticity:.3f} (±{max(values) - min(values):.3f})")
    
    print(f"\nStructural Plasticity Events:")
    print(f"  Synaptogenesis events: {development_log['synaptogenesis_events']}")
    print(f"  Pruning events: {development_log['pruning_events']}")
    print(f"  Total development time: {development_log['development_period']:.1f}s")
    print(f"  Critical period entered: {development_log['critical_period_entered']}")
    
    # 7. Analyze phase-specific changes
    print("\n=== Phase-Specific Changes ===")
    if phase_plasticity_changes:
        print("Plasticity dynamics by phase:")
        for change in phase_plasticity_changes:
            print(f"  Step {change['step']}: "
                  f"Stage={change['stage']}, "
                  f"Plasticity={change['plasticity_modifier']:.3f}, "
                  f"Synap={change['synaptogenesis_rate']:.6f}, "
                  f"Prune={change['pruning_rate']:.6f}")
    
    # 8. Show development milestones
    print("\n=== Developmental Milestones ===")
    milestones = []
    
    if development_log['critical_period_entered']:
        milestones.append("✓ Critical period entered")
    
    if development_log['synaptogenesis_events'] > 100:
        milestones.append("✓ Significant synaptogenesis occurred")
    
    if development_log['pruning_events'] > 50:
        milestones.append("✓ Active pruning phase reached")
    
    if plasticity_values and max(plasticity_values) < 0.3:
        milestones.append("✓ Reached stable adult stage")
    
    print("Key milestones achieved:")
    for milestone in milestones:
        print(f"  {milestone}")
    
    # 9. Save developmental data
    print("\n=== Saving Developmental Data ===")
    brain.save("developmental_study_brain.bin")
    print("Developmental brain state saved to developmental_study_brain.bin")
    
    # Export detailed developmental metrics
    import json
    developmental_metrics = {
        'total_steps': len(development_log['steps']),
        'total_phases': 4,
        'stages_observed': list(set(stages)),
        'stage_progression': stages,
        'average_plasticity': sum(plasticity_values) / len(plasticity_values) if plasticity_values else 0,
        'plasticity_variance': max(plasticity_values) - min(plasticity_values) if plasticity_values else 0,
        'synaptogenesis_events': development_log['synaptogenesis_events'],
        'pruning_events': development_log['pruning_events'],
        'development_duration': development_log['development_period'],
        'critical_period_entered': development_log['critical_period_entered'],
        'final_development_stage': stages[-1] if stages else None,
        'final_plasticity': plasticity_values[-1] if plasticity_values else 0,
    }
    
    with open("developmental_metrics.json", "w") as f:
        json.dump(developmental_metrics, f, indent=2, default=str)
    
    print("Developmental metrics saved to developmental_metrics.json")
    
    return developmental_metrics, brain, agent, world
def demonstrate_structural_plasticity():
    """Demonstrate structural plasticity mechanisms"""
    print("\n=== Structural Plasticity Demonstration ===\n")
    
    # Create brain for structural plasticity demonstration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    sp = brain.getStructuralPlasticity()
    if not sp:
        print("Structural plasticity system not available")
        return brain
    
    print("1. Synaptogenesis Demonstration:")
    print("   - Initial synapse count:", brain.getTotalSynapseCount())
    
    # Simulate synaptogenesis over time
    initial_synapses = brain.getTotalSynapseCount()
    
    for time_step in range(5):
        sp.update(brain, *brain.getRandomGenerator())
        current_synapses = brain.getTotalSynapseCount()
        new_synapses = current_synapses - initial_synapses
        
        print(f"   Time {time_step + 1}: {current_synapses} synapses (+{new_synapses})")
        
        # Simulate some activity to influence plasticity
        for i in range(10):
            brain.step(i)
    
    print("\n2. Pruning Demonstration:")
    sp.setPruningRate(0.00001)  # Set pruning rate
    
    synapses_before_pruning = brain.getTotalSynapseCount()
    
    # Simulate pruning
    for time_step in range(3):
        sp.update(brain, *brain.getRandomGenerator())
        synapses_after_pruning = brain.getTotalSynapseCount()
        pruned = synapses_before_pruning - synapses_after_pruning
        
        print(f"   Pruning step {time_step + 1}: {pruned} synapses removed")
        
        synapses_before_pruning = synapses_after_pruning
    
    print("\n3. Plasticity Rate Analysis:")
    print(f"   Final synaptogenesis rate: {sp.getSynaptogenesisRate():.6f}")
    print(f"   Final pruning rate: {sp.getPruningRate():.6f}")
    print(f"   Net synapse change: {brain.getTotalSynapseCount() - initial_synapses}")
    
    print("\n=== Structural Plasticity Demonstration Complete ===")
    return brain
def main():
    """Main function to run all demonstrations"""
    print("NLM Advanced Development and Plasticity Example")
    print("=" * 60)
    
    try:
        # Run comprehensive developmental study
        metrics, brain, agent, world = run_developmental_study()
        
        # Demonstrate structural plasticity mechanisms
        plasticity_brain = demonstrate_structural_plasticity()
        
        # Summary
        print("\n" + "=" * 60)
        print("EXAMPLE SUMMARY")
        print("=" * 60)
        print("✓ Developmental configuration created")
        print("✓ 4 developmental phases simulated")
        print("✓ Structural plasticity mechanisms demonstrated")
        print("✓ Critical period dynamics shown")
        print("✓ Synaptogenesis and pruning tracked")
        print("✓ Developmental milestones achieved")
        print("✓ Brain state saved")
        print("✓ Detailed metrics exported")
        print("\nThis example demonstrates NLM's developmental capabilities,")
        print("including:")
        print("• Age-dependent plasticity rates")
        print("• Critical period dynamics")
        print("• Synaptogenesis and pruning")
        print("• Developmental stage transitions")
        print("• Structural plasticity mechanisms")
        print("• Neural maturation over time")
        print("• Experience-dependent development")
        
    except Exception as e:
        print(f"Example failed with error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0
if __name__ == "__main__":
    exit(main())
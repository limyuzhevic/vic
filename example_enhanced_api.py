#!/usr/bin/env python3
"""
Enhanced Python Bindings Example
This script demonstrates the new Pythonic features and advanced functionality
after the improvements to the NLM Python bindings.
"""

import pynlm
import time
import json
from pathlib import Path
from typing import List, Dict, Any, Optional

def main():
    print("=== Enhanced NLM Python API Example ===\n")
    
    # 1. Create a basic brain with configuration
    print("1. Creating and configuring brain...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    
    brain = pynlm.createBrain(config)
    
    # 2. Using context manager for resource management
    print("2. Using context manager...")
    with pynlm.createBrain(config) as managed_brain:
        managed_brain.initialize()
        print(f"   Initialized brain with {managed_brain.getTotalNeuronCount()} neurons")
        # Context manager ensures cleanup
    
    # 3. Property-based access
    print("3. Property-based access...")
    brain.initialize()
    stats = {
        "neurons": brain.getTotalNeuronCount(),
        "synapses": brain.getTotalSynapseCount(),
        "firing": brain.getFiringNeuronCount(),
        "active": brain.getActiveNeuronCount(),
        "excitatory_inhibition_ratio": brain.getExcitationInhibitionRatio(),
        "developmental_stage": str(brain.getDevelopmentalStage()),
        "total_spikes": brain.getTotalSpikeCount()
    }
    print(f"   Brain statistics: {json.dumps(stats, indent=2)}")
    
    # 4. Python-style iteration
    print("4. Iterating over brain regions...")
    for i, region_id in enumerate(brain.getRegionIds()):
        if i < 3:  # Show first 3 regions
            region = brain.getRegion(region_id)
            print(f"   Region {i}: {region_id} (placeholder - no name)")
        else:
            break
    print(f"   Total regions: {brain.getRegionCount()}")
    
    # 5. Advanced simulation with control
    print("5. Advanced simulation control...")
    simulation = pynlm.Simulation(brain)
    simulation.set_mode(pynlm.SimulationMode.EPISODE, num_episodes=10)
    simulation.set_max_steps_per_episode(100)
    simulation.enable_performance_monitoring(True)
    simulation.set_autosave_interval(50)
    
    episode_data = simulation.run()
    print(f"   Completed {len(episode_data)} episodes")
    
    # 6. Replay and history tracking
    print("6. Replay functionality...")
    if episode_data:
        simulation.replay_episode(episode_data[0])
        print("   Replayed first episode successfully")
    
    # 7. Performance monitoring
    print("7. Performance metrics...")
    perf = brain.get_performance_metrics()
    print(f"   Average step time: {perf.get('avg_step_time_ms', 0):.2f}ms")
    print(f"   Memory usage: {perf.get('memory_usage_mb', 0):.2f}MB")
    
    # 8. Enhanced world interaction
    print("8. Enhanced world simulation...")
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=7, visionHeight=7)
    world.setAgentStart(15.0, 15.0)
    
    # Create agent with enhanced capabilities
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    agent.enable_prediction_error(True)
    
    # Simulation loop with enhanced logging
    episode_results = []
    with agent.create_episode(episode_id=1) as episode:
        for step in range(200):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.process_sensory_input(percept)
            
            brain.step(step)
            
            motor_cmd = agent.decode_motor_command()
            result = world.apply_motor_command(motor_cmd, world.get_simulation_time())
            
            reward = percept.get_internal()[0] if percept.get_internal() else 0.0
            agent.apply_reward_modulation(reward, 0.0)
            
            agent.update_development(0.1)
            
            if step % 50 == 0:
                print(f"   Step {step}: Energy={agent.get_brain().get_agent_body().energy:.1f}, "
                      f"Curiosity={agent.get_curiosity_level():.3f}")
            
            # Log step data
            episode.log_step({
                'step': step,
                'reward': reward,
                'action': str(motor_cmd),
                'curiosity': agent.get_curiosity_level(),
                'novelty': agent.get_novelty_level(),
                'prediction_error': agent.get_prediction_error()
            })
    
    episode_results.append(episode.get_data())
    
    # 9. Experiment builder and runner
    print("9. Running experiment...")
    experiment = pynlm.create_experiment(
        name="Learning_Curiosity_Based",
        description="Test curiosity-driven learning",
        brain_config=config,
        world_config={"width": 20, "height": 20},
        parameters={"num_episodes": 50, "curiosity_weight": 0.5}
    )
    
    experiment.set_metric(pynlm.MetricType.CURIOUSITY, pynlm.MetricAggregation.MEAN)
    experiment.set_metric(pynlm.MetricType.NOVELTY, pynlm.MetricAggregation.MAX)
    experiment.set_metric(pynlm.MetricType.PREDICTION_ERROR, pynlm.MetricAggregation.SUM)
    
    experiment_results = experiment.run()
    print(f"   Experiment completed: {experiment_results.get_summary()}")
    
    # 10. Save/load with JSON
    print("10. Save/load with JSON...")
    save_data = {
        "brain_state": brain.save(),
        "config": config.summary(),
        "episodes": episode_results,
        "metrics": experiment_results.get_metrics()
    }
    
    with open("nlm_session.json", "w") as f:
        json.dump(save_data, f, indent=2, default=str)
    print("   Session saved to nlm_session.json")
    
    # 11. Teaching mode with step-by-step debugging
    print("11. Teaching mode with step-by-step debugging...")
    teaching_brain = pynlm.createBrain(config)
    teaching_brain.initialize()
    
    teaching_brain.set_debug_mode(True)
    teaching_brain.enable_step_by_step(True)
    teaching_brain.set_breakpoints([10, 20, 50])
    
    print("   Brain in teaching mode - enabled step-by-step debugging")
    
    # 12. Configuration validation
    print("12. Configuration validation...")
    validator = pynlm.create_config_validator()
    validator.add_rule("brain.neuron_count", pynlm.ValidationRule.RANGE, (100, 10000))
    validator.add_rule("brain.synapse_density", pynlm.ValidationRule.POSITIVE)
    validator.add_rule("plasticity.stdp.enable", pynlm.ValidationRule.BOOLEAN)
    
    config_errors = validator.validate(config)
    if not config_errors:
        print("   Configuration validation passed")
    else:
        print(f"   Validation errors: {config_errors}")
    
    # 13. Advanced agent behaviors
    print("13. Advanced agent behaviors...")
    advanced_agent = pynlm.create_advanced_agent(brain, world)
    advanced_agent.set_exploration_strategy(pynlm.ExplorationStrategy.EPSILON_GREEDY)
    advanced_agent.set_epsilon(0.1)
    advanced_agent.set_memory_capacity(1000)
    advanced_agent.enable_hindsight(True)
    
    print("   Advanced agent configured with epsilon-greedy exploration")
    
    print("\n=== All enhancements demonstrated successfully! ===")

if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""
NLM Phase 6 Enhanced Integration Examples
=========================================

This file contains comprehensive examples demonstrating the Phase 6 integration
of all NLM brain systems. These examples showcase the enhanced Python bindings
and demonstrate advanced usage patterns.

Examples include:
1. Basic Phase 6 integration test
2. Advanced agent-world interaction
3. Performance benchmarking
4. Memory and development tracking
5. Complete simulation with all subsystems enabled
"""

import pynlm
import time
import json
from pathlib import Path

def example_basic_phase6_integration():
    """
    Basic Phase 6 integration test.
    Demonstrates verification of all integrated systems.
    """
    print("=== Example 1: Basic Phase 6 Integration Test ===")
    
    try:
        # Create default configuration
        config = pynlm.createDefaultConfig()
        
        # Create brain with moderate size
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        
        brain = pynlm.createBrain(config)
        print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
        
        # Create experiment
        experiment = pynlm.Phase6IntegratedExperiment()
        print("✓ Created Phase 6 experiment")
        
        # Run integration verification
        verification_result = experiment.verifyIntegration()
        print(f"✓ Integration verification: {'PASSED' if verification_result else 'FAILED'}")
        
        # Run integration tests
        integration_tests = experiment.runIntegrationTests()
        print(f"✓ Integration tests: {'PASSED' if integration_tests.get('success', False) else 'FAILED'}")
        
        if integration_tests.get('success', False):
            print("✓ All Phase 6 systems are properly integrated")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

def example_advanced_agent_world():
    """
    Advanced agent-world interaction with all subsystems enabled.
    Demonstrates the complete brain loop with neuromodulation and development.
    """
    print("\n=== Example 2: Advanced Agent-World Interaction ===")
    
    try:
        # Configure for complex simulation
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.structural.enable", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"✓ Initialized brain with {brain.getTotalNeuronCount()} neurons")
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all learning subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("✓ Enabled all learning subsystems:")
        print(f"  - Reward Modulation: {agent.isRewardModulationEnabled()}")
        print(f"  - Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")
        print(f"  - Development: {agent.isDevelopmentEnabled()}")
        print(f"  - Curiosity: {agent.isCuriosityEnabled()}")
        
        # Run simulation with metrics collection
        episode_data = {
            'steps': 0,
            'total_reward': 0.0,
            'curiosity_level': 0.0,
            'novelty_level': 0.0,
            'dev_stage': 0,
            'spike_counts': [],
            'action_counts': {}
        }
        
        action_types = ['MoveForward', 'MoveBackward', 'TurnLeft', 'TurnRight', 
                       'Look', 'Interact', 'Eat', 'Rest', 'Wait', 'Custom']
        for action_type in action_types:
            episode_data['action_counts'][action_type] = 0
        
        start_time = time.time()
        
        for step in range(200):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process in brain
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get action from brain
            action = agent.decodeMotorCommand()
            
            # Apply action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get current state
            episode_data['steps'] = step + 1
            episode_data['curiosity_level'] = agent.getCuriosityLevel()
            episode_data['novelty_level'] = agent.getNoveltyLevel()
            episode_data['dev_stage'] = brain.getDevelopmentalStage()
            episode_data['spike_counts'].append(brain.getTotalSpikeCount())
            
            # Track action usage
            action_name = action.getName() if hasattr(action, 'getName') else str(action)
            if action_name in episode_data['action_counts']:
                episode_data['action_counts'][action_name] += 1
            
            # Print progress every 50 steps
            if step % 50 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}")
        
        episode_data['total_time'] = time.time() - start_time
        
        print(f"✓ Simulation completed in {episode_data['total_time']:.2f} seconds")
        print(f"✓ Final statistics:")
        print(f"  - Total reward accumulated: {brain.getTotalSpikeCount()}")  # Placeholder
        print(f"  - Average firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  - Most active neurons: {brain.getFiringNeuronCount()}")
        print(f"  - Developmental stage: {brain.getDevelopmentalStage()}")
        
        # Save episode data
        with open('episode_data.json', 'w') as f:
            json.dump(episode_data, f, indent=2)
        
        print("✓ Episode data saved to episode_data.json")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

def example_performance_benchmarking():
    """
    Performance benchmarking of brain operations.
    Demonstrates the enhanced benchmarking capabilities.
    """
    print("\n=== Example 3: Performance Benchmarking ===")
    
    try:
        import pynlm
        
        # Test different brain configurations
        configs = {
            'small': {'neurons': 100, 'name': 'Small Brain'},
            'medium': {'neurons': 500, 'name': 'Medium Brain'},
            'large': {'neurons': 1000, 'name': 'Large Brain'}
        }
        
        benchmark_results = {}
        
        for config_name, config_params in configs.items():
            print(f"\nBenchmarking {config_params['name']}...")
            
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", config_params['neurons'])
            config.set("plasticity.stdp.enable", True)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Benchmark different operations
            op_results = {}
            
            # Step benchmarking
            start_time = time.time()
            for step in range(100):
                brain.step(step)
            step_time = time.time() - start_time
            
            op_results['step_100_time'] = step_time
            op_results['avg_step_time'] = step_time / 100
            
            # Memory benchmarking
            start_time = time.time()
            brain.save('test_checkpoint.bin')
            save_time = time.time() - start_time
            op_results['save_time'] = save_time
            
            # Statistics benchmarking
            start_time = time.time()
            for _ in range(100):
                _ = brain.getTotalNeuronCount()
                _ = brain.getTotalSynapseCount()
                _ = brain.getFiringNeuronCount()
                _ = brain.getAverageFiringRate()
            stats_time = time.time() - start_time
            op_results['stats_time_100'] = stats_time
            op_results['avg_stats_time'] = stats_time / 100
            
            benchmark_results[config_name] = op_results
            
            print(f"  ✓ Step time (100 steps): {op_results['avg_step_time']:.4f}s per step")
            print(f"  ✓ Save time: {op_results['save_time']:.4f}s")
            print(f"  ✓ Stats time (100 calls): {op_results['avg_stats_time']:.4f}s per call")
        
        # Save benchmark results
        with open('benchmark_results.json', 'w') as f:
            json.dump(benchmark_results, f, indent=2)
        
        print(f"\n✓ All benchmark results saved to benchmark_results.json")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

def example_memory_and_development():
    """
    Advanced memory and development tracking.
    Demonstrates Phase 6 memory systems and developmental progression.
    """
    print("\n=== Example 4: Memory and Development Tracking ===")
    
    try:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 800)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("development.enabled", True)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent for episodic memory
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("✓ Initialized brain with memory and development systems")
        
        # Track developmental progression
        developmental_data = {
            'stages': [],
            'plasticity_changes': [],
            'memory_growth': [],
            'curiosity_trajectory': []
        }
        
        # Run simulation with developmental tracking
        for step in range(150):
            # Update world
            world.update(0.1)
            
            # Process and act
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Record developmental data
            developmental_data['stages'].append(brain.getDevelopmentalStage())
            developmental_data['plasticity_changes'].append(agent.getNeuromodulationLevel())
            developmental_data['memory_growth'].append(brain.getTotalSpikeCount())
            developmental_data['curiosity_trajectory'].append(agent.getCuriosityLevel())
            
            if step % 30 == 0:
                print(f"  Step {step}:")
                print(f"    Development Stage: {brain.getDevelopmentalStage()}")
                print(f"    Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
                print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"    Memory Episodes: {brain.getTotalSpikeCount()}")  # Placeholder
        
        # Analyze developmental progression
        print("\n✓ Developmental Analysis:")
        
        stage_transitions = {}
        for stage in developmental_data['stages']:
            stage_key = str(stage)
            stage_transitions[stage_key] = stage_transitions.get(stage_key, 0) + 1
        
        print(f"  Stage distribution:")
        for stage, count in stage_transitions.items():
            percentage = (count / len(developmental_data['stages'])) * 100
            print(f"    {stage}: {count} steps ({percentage:.1f}%)")
        
        # Calculate learning metrics
        initial_curiosity = developmental_data['curiosity_trajectory'][0]
        final_curiosity = developmental_data['curiosity_trajectory'][-1]
        curiosity_growth = final_curiosity - initial_curiosity
        
        print(f"\n✓ Learning Metrics:")
        print(f"  Curiosity Growth: {curiosity_growth:.3f}")
        print(f"  Neuromodulation Range: {min(developmental_data['plasticity_changes']):.3f} - "
              f"{max(developmental_data['plasticity_changes']):.3f}")
        
        # Save developmental data
        with open('developmental_data.json', 'w') as f:
            json.dump(developmental_data, f, indent=2)
        
        print("✓ Developmental data saved to developmental_data.json")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        return False

def example_complete_phase6_simulation():
    """
    Complete Phase 6 simulation with all features enabled.
    Demonstrates the full power of the integrated NLM system.
    """
    print("\n=== Example 5: Complete Phase 6 Simulation ===")
    
    try:
        # Create comprehensive configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1200)
        config.set("brain.synapse_density", 0.12)
        config.set("connection_probability", 0.18)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.001)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.dopamine.scale", 1.2)
        config.set("development.enabled", True)
        config.set("development.critical_period_duration", 100)
        config.set("memory.episodic.capacity", 50)
        config.set("memory.associative.capacity", 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(20.0, 20.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        print("✓ Initialized complete Phase 6 simulation with all subsystems:")
        print(f"  - Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
        print(f"  - Agent: All learning subsystems enabled")
        print(f"  - World: {world.getWidth()}x{world.getHeight()} environment")
        
        # Run extended simulation
        simulation_data = {
            'episodes': 1,
            'steps_per_episode': 300,
            'start_time': time.time(),
            'metrics': [],
            'action_patterns': {},
            'memory_growth': []
        }
        
        action_types = ['MoveForward', 'MoveBackward', 'TurnLeft', 'TurnRight',
                       'Look', 'LookUp', 'LookDown', 'Interact', 'Eat', 'Drink',
                       'Rest', 'Wait', 'Custom']
        for action_type in action_types:
            simulation_data['action_patterns'][action_type] = 0
        
        print(f"\n✓ Running extended simulation ({simulation_data['steps_per_episode']} steps)...")
        
        for step in range(simulation_data['steps_per_episode']):
            # Complete agent-world loop
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.updateDevelopment(0.1)
            
            # Collect metrics
            step_metrics = {
                'step': step,
                'time': time.time() - simulation_data['start_time'],
                'neurons_firing': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'neuromodulation': agent.getNeuromodulationLevel(),
                'dev_stage': brain.getDevelopmentalStage()
            }
            
            simulation_data['metrics'].append(step_metrics)
            simulation_data['memory_growth'].append(brain.getTotalSpikeCount())
            
            # Track actions
            action_name = action.getName() if hasattr(action, 'getName') else str(action)
            if action_name in simulation_data['action_patterns']:
                simulation_data['action_patterns'][action_name] += 1
            
            # Print progress
            if step % 60 == 0:
                print(f"  Step {step}/{simulation_data['steps_per_episode']} "
                      f"({(step/simulation_data['steps_per_episode'])*100:.0f}%)")
        
        # Simulation complete
        total_time = time.time() - simulation_data['start_time']
        
        print(f"\n✓ Simulation Complete!")
        print(f"  Total Time: {total_time:.2f} seconds")
        print(f"  Average Step Time: {total_time/simulation_data['steps_per_episode']:.4f} seconds")
        
        # Final results
        final_metrics = simulation_data['metrics'][-1]
        print(f"\n✓ Final Results:")
        print(f"  Development Stage: {final_metrics['dev_stage']}")
        print(f"  Curiosity Level: {final_metrics['curiosity']:.3f}")
        print(f"  Novelty Level: {final_metrics['novelty']:.3f}")
        print(f"  Neuromodulation Level: {final_metrics['neuromodulation']:.3f}")
        print(f"  Total Spikes: {final_metrics['total_spikes']}")
        print(f"  Average Firing Rate: {final_metrics['avg_firing_rate']:.3f}")
        
        # Action pattern analysis
        print(f"\n✓ Action Pattern Analysis:")
        total_actions = sum(simulation_data['action_patterns'].values())
        for action, count in sorted(simulation_data['action_patterns'].items(), 
                                   key=lambda x: x[1], reverse=True):
            percentage = (count / total_actions) * 100 if total_actions > 0 else 0
            print(f"  {action}: {count} actions ({percentage:.1f}%)")
        
        # Memory growth analysis
        memory_growth = simulation_data['memory_growth']
        if len(memory_growth) >= 2:
            initial_memory = memory_growth[0]
            final_memory = memory_growth[-1]
            memory_growth_rate = (final_memory - initial_memory) / len(memory_growth)
            print(f"\n✓ Memory Growth Analysis:")
            print(f"  Initial Memory: {initial_memory}")
            print(f"  Final Memory: {final_memory}")
            print(f"  Growth Rate: {memory_growth_rate:.2f} spikes per step")
        
        # Save complete simulation data
        with open('complete_simulation_data.json', 'w') as f:
            json.dump(simulation_data, f, indent=2)
        
        print("✓ Complete simulation data saved to complete_simulation_data.json")
        
        # Create summary report
        summary = {
            'simulation_parameters': {
                'brain_size': brain.getTotalNeuronCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'environment_size': f"{world.getWidth()}x{world.getHeight()}",
                'vision_size': f"{world.getVisionWidth()}x{world.getVisionHeight()}",
                'steps': simulation_data['steps_per_episode'],
                'total_time': total_time
            },
            'final_performance': {
                'development_stage': final_metrics['dev_stage'],
                'curiosity_level': final_metrics['curiosity'],
                'novelty_level': final_metrics['novelty'],
                'neuromodulation_level': final_metrics['neuromodulation'],
                'total_spikes': final_metrics['total_spikes'],
                'avg_firing_rate': final_metrics['avg_firing_rate']
            },
            'learning_summary': {
                'action_patterns': simulation_data['action_patterns'],
                'memory_growth': memory_growth
            }
        }
        
        with open('simulation_summary.json', 'w') as f:
            json.dump(summary, f, indent=2)
        
        print("✓ Simulation summary saved to simulation_summary.json")
        
        return True
        
    except Exception as e:
        print(f"✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """
    Run all Phase 6 examples.
    """
    print("NLM Phase 6 Enhanced Integration Examples")
    print("=" * 50)
    
    examples = [
        ("Basic Phase 6 Integration Test", example_basic_phase6_integration),
        ("Advanced Agent-World Interaction", example_advanced_agent_world),
        ("Performance Benchmarking", example_performance_benchmarking),
        ("Memory and Development Tracking", example_memory_and_development),
        ("Complete Phase 6 Simulation", example_complete_phase6_simulation)
    ]
    
    results = []
    
    for example_name, example_func in examples:
        print(f"\n{'='*60}")
        try:
            success = example_func()
            results.append((example_name, success))
            
            if success:
                print(f"✓ {example_name}: PASSED")
            else:
                print(f"✗ {example_name}: FAILED")
                
        except Exception as e:
            print(f"✗ {example_name}: ERROR - {e}")
            results.append((example_name, False))
    
    # Summary
    print(f"\n{'='*60}")
    print("EXAMPLES EXECUTION SUMMARY")
    print(f"{'='*60}")
    
    passed = sum(1 for _, success in results if success)
    total = len(results)
    
    for example_name, success in results:
        status = "✓ PASSED" if success else "✗ FAILED"
        print(f"{status} {example_name}")
    
    print(f"\nTotal: {passed}/{total} examples passed")
    
    if passed == total:
        print("🎉 All examples executed successfully!")
    else:
        print(f"⚠️  {total - passed} example(s) failed. Check output for details.")
    
    return passed == total

if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)
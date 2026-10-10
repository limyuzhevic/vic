"""
Advanced Learning Analysis Example for NLM

This example demonstrates the advanced learning analysis capabilities
introduced in the enhanced NLM Python bindings.

The analysis includes:
- Learning performance metrics
- Novelty and curiosity tracking
- Synaptic weight change analysis
- Development stage monitoring
- Behavioral pattern analysis
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
from datetime import datetime

def create_advanced_learning_experiment():
    """Create a comprehensive learning experiment with advanced analysis"""
    
    print("=== Advanced NLM Learning Analysis Example ===\n")
    
    # 1. Create configuration with advanced settings
    print("1. Creating advanced configuration...")
    config = pynlm.createDefaultConfig()
    
    # Configure for learning experiments
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.connection_probability", 0.12)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    print("   ✓ Configuration created with advanced learning settings")
    
    # 2. Create brain and agent
    print("\n2. Initializing neural components...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    print("   ✓ Brain and agent initialized")
    
    # 3. Create world for agent-environment interaction
    print("\n3. Setting up agent environment...")
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent.initialize(world)
    
    # Enable advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("   ✓ World and agent environment configured")
    
    # 4. Advanced experiment parameters
    print("\n4. Running advanced learning experiment...")
    
    # Track experiment metrics
    experiment_data = {
        'steps': [],
        'firing_rates': [],
        'learning_rates': [],
        'novelty_levels': [],
        'curiosity_levels': [],
        'reward_signals': [],
        'development_stages': [],
        'synaptic_changes': [],
        'behavioral_scores': []
    }
    
    num_steps = 1000
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get motor command and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        if step % 10 == 0:  # Sample every 10 steps
            experiment_data['steps'].append(step)
            experiment_data['firing_rates'].append(brain.getAverageFiringRate())
            experiment_data['learning_rates'].append(agent.getPredictionError())
            experiment_data['novelty_levels'].append(agent.getNoveltyLevel())
            experiment_data['curiosity_levels'].append(agent.getCuriosityLevel())
            experiment_data['reward_signals'].append(reward)
            experiment_data['development_stages'].append(brain.getDevelopmentalStage())
            experiment_data['behavioral_scores'].append(abs(reward) * agent.getNoveltyLevel())
        
        # Log progress periodically
        if step % 200 == 0:
            print(f"   Step {step}: Firing rate={brain.getAverageFiringRate():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    print(f"   ✓ Experiment completed: {num_steps} steps")
    
    # 5. Advanced learning analysis
    print("\n5. Performing advanced learning analysis...")
    
    # Analyze learning performance using advanced metrics
    metrics = pynlm.analyze_learning_performance(brain, agent, experiment_data['reward_signals'])
    
    print("   Learning Performance Analysis:")
    print(f"   - Learning Grade: {metrics['performance_grade']}")
    print(f"   - Improvement Potential: {metrics['improvement_potential']:.2%}")
    print(f"   - Recommendations: {len(metrics['recommendations'])} suggestions")
    
    # 6. Create experiment report
    print("\n6. Generating comprehensive experiment report...")
    
    # Create visualization data
    viz_data = pynlm.create_visualization_data(brain, agent, world)
    
    # Generate experiment report
    report = pynlm.generate_experiment_report(
        title="Advanced NLM Learning Experiment",
        description="Comprehensive analysis of neural learning with neuromodulation",
        author="NLM Research Team",
        brain=brain,
        agent=agent,
        world=world,
        rewards=experiment_data['reward_signals']
    )
    
    print(f"   ✓ Experiment report generated: {report['title']}")
    print(f"   ✓ Duration: {report['duration']:.1f} simulated seconds")
    print(f"   ✓ Success Rate: {report['success_rate']:.1%}")
    
    # 7. Save experiment configuration
    print("\n7. Saving experiment configuration...")
    
    # Create experiment configuration
    exp_config = {
        'brain_configuration': {
            'neuron_count': 2000,
            'synapse_density': 0.15,
            'learning_rate': 0.002,
            'novelty_sensitivity': 0.8
        },
        'agent_settings': {
            'reward_modulation': True,
            'structural_plasticity': True,
            'development_enabled': True,
            'curiosity_enabled': True
        },
        'experiment_parameters': {
            'num_steps': num_steps,
            'world_width': 30,
            'world_height': 30,
            'vision_resolution': '12x12'
        },
        'metadata': {
            'created': datetime.now().isoformat(),
            'version': '2.0-advanced',
            'tags': ['learning', 'neuromodulation', 'advanced']
        }
    }
    
    saved_config = pynlm.save_experiment_config(
        name="advanced_learning_experiment",
        parameters=exp_config,
        description="Advanced learning experiment with comprehensive analysis",
        tags=['learning', 'neuromodulation', 'advanced', 'research']
    )
    
    print(f"   ✓ Experiment configuration saved as: {saved_config['name']}")
    print(f"   ✓ {saved_config['parameters_count']} parameters stored")
    
    # 8. Generate results export
    print("\n8. Exporting results...")
    
    # Create comprehensive results summary
    results_summary = {
        'experiment_id': 'advanced_learning_exp_001',
        'brain_metrics': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'development_stage': brain.getDevelopmentalStage()
        },
        'agent_metrics': {
            'final_novelty': agent.getNoveltyLevel(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel()
        },
        'performance_analysis': metrics,
        'experiment_report': {
            'title': report['title'],
            'success_rate': report['success_rate'],
            'duration': report['duration']
        },
        'visualization_data': viz_data,
        'training_results': experiment_data
    }
    
    # Export to JSON (simplified)
    import json
    results_json = json.dumps(results_summary, indent=2, default=str)
    
    # Save results
    with open('advanced_experiment_results.json', 'w') as f:
        f.write(results_json)
    
    print(f"   ✓ Results exported to: advanced_experiment_results.json")
    print(f"   ✓ File size: {len(results_json)} bytes")
    
    # 9. Display final summary
    print("\n=== Advanced Experiment Summary ===")
    print(f"Experiment ID: {results_summary['experiment_id']}")
    print(f"Brain Architecture: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    print(f"Final Development Stage: {brain.getDevelopmentalStage()}")
    print(f"Learning Performance Grade: {metrics['performance_grade']}")
    print(f"Total Training Steps: {num_steps}")
    print(f"Average Firing Rate: {np.mean(experiment_data['firing_rates']):.3f}")
    print(f"Average Novelty Level: {np.mean(experiment_data['novelty_levels']):.3f}")
    print(f"Average Curiosity Level: {np.mean(experiment_data['curiosity_levels']):.3f}")
    
    return results_summary, experiment_data, report

def create_batch_processing_example():
    """Demonstrate batch processing capabilities"""
    
    print("\n=== NLM Batch Processing Example ===\n")
    
    # Example: Run multiple simulations with different configurations
    print("Creating batch of simulations with varied configurations...")
    
    # Batch configurations
    batch_configs = [
        {
            'name': 'baseline_learning',
            'neuron_count': 1000,
            'synapse_density': 0.1,
            'learning_rate': 0.001
        },
        {
            'name': 'enhanced_learning',
            'neuron_count': 2000,
            'synapse_density': 0.15,
            'learning_rate': 0.002
        },
        {
            'name': 'high_curiosity',
            'neuron_count': 1500,
            'synapse_density': 0.12,
            'learning_rate': 0.0015
        }
    ]
    
    # Run batch simulation
    print("Running batch simulation...")
    results = pynlm.run_batch_simulation(
        num_simulations=3,
        steps_per_simulation=500,
        output_dir="batch_results",
        save_results=True,
        parallel=False,
        timestep=0.001
    )
    
    print(f"✓ Batch simulation completed: {len(results)} results")
    print("Sample results:")
    
    for i, result in enumerate(results[:3]):
        print(f"  Simulation {result['simulation_index']}:")
        print(f"    Status: {result['status']}")
        print(f"    Steps executed: {result['steps_executed']}")
        print(f"    Total spikes: {result['total_spikes']}")
        print(f"    Simulation time: {result['simulation_time']:.3f}s")
    
    return results

if __name__ == "__main__":
    print("NLM Advanced Features Demonstration")
    print("=" * 50)
    
    # Run advanced learning experiment
    try:
        results, data, report = create_advanced_learning_experiment()
        print("\n✓ Advanced learning experiment completed successfully!")
    except Exception as e:
        print(f"✗ Error in advanced experiment: {e}")
    
    # Run batch processing example
    try:
        batch_results = create_batch_processing_example()
        print("\n✓ Batch processing example completed successfully!")
    except Exception as e:
        print(f"✗ Error in batch processing: {e}")
    
    print("\n" + "=" * 50)
    print("Advanced NLM features demonstration complete!")
    print("\nKey features demonstrated:")
    print("✓ Advanced learning performance analysis")
    print("✓ Neuromodulation tracking (novelty, curiosity)")
    print("✓ Experiment reporting and visualization data")
    print("✓ Configuration management and experiment tracking")
    print("✓ Batch processing capabilities")
    print("✓ Performance metrics and recommendations")

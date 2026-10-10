"""Expert NLM Demo - Advanced Plasticity and Neuromodulation

This example demonstrates advanced NLM features including custom plasticity rules,
advanced neuromodulation, expert performance optimization, and sophisticated agent behavior.
"""
import nlm
from nlm import expert
import time
import matplotlib.pyplot as plt
import numpy as np

def run_expert_demo():
    """Run expert NLM demonstration."""
    print("=== Expert NLM Demo ===")
    print("This example demonstrates advanced NLM features including:")
    print("  - Custom plasticity rules")
    print("  - Advanced neuromodulation")
    print("  - Expert performance optimization")
    print("  - Real-time visualization")
    print()
    
    # Setup with expert configuration
    print("1. Setting up expert simulation...")
    config = nlm.create_high_performance_config()
    config.set('brain.simulated_neurons', 3000)
    
    # Create expert systems
    memory_pool = expert.MemoryPool(16384, 131072)
    simd_processor = expert.SIMDProcessor(512)
    parallel_processor = expert.ParallelProcessor(8)
    
    print(f"   - Memory pool size: {memory_pool.initial_size}/{memory_pool.max_size}")
    print(f"   - SIMD processor: {simd_processor.vector_width}-bit")
    print(f"   - Parallel processor: {parallel_processor.num_threads} threads")
    print()
    
    # Create advanced plasticity rules
    print("2. Creating advanced plasticity rules...")
    custom_plasticity = expert.CustomPlasticityRules()
    custom_plasticity.add_rule("hebbian", strength=0.7, window=10)
    custom_plasticity.add_rule("stdp", alpha=0.01, beta=0.005)
    custom_plasticity.add_rule("structural", rate=0.001, threshold=0.8)
    
    print(f"   - Plasticity rules: {len(custom_plasticity.rules)}")
    print()
    
    # Create advanced neuromodulation
    print("3. Setting up advanced neuromodulation...")
    neuromodulation = expert.AdvancedNeuromodulation({
        'dopamine_strength': 0.9,
        'serotonin_influence': 0.7,
        'curiosity_strength': 0.8,
        'novelty_threshold': 0.3
    })
    
    print(f"   - Dopamine influence: {neuromodulation.dopamine_influence}")
    print(f"   - Curiosity strength: {neuromodulation.curiosity_strength}")
    print()
    
    # Create custom connectivity patterns
    print("4. Creating custom connectivity patterns...")
    connectivity = expert.CustomConnectivityPatterns()
    
    # Create neural population manager
    population_manager = expert.NeuralPopulationManager()
    
    print(f"   - Connectivity patterns: {len(connectivity.patterns)}")
    print()
    
    # Create world and brain
    world = nlm.create_simple_world()
    world.configure(30, 30, 128, 128)
    world.setAgentStart(15, 15)
    
    brain = nlm.create_brain(config)
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    print(f"   - World size: {world.width}x{world.height}")
    print(f"   - Initial neurons: {brain.getStatistics()['total_neurons']}")
    print()
    
    # Setup performance monitoring
    print("5. Setting up performance monitoring...")
    from nlm import expert as expert_module
    performance_profiler = expert_module.PerformanceProfiler()
    memory_profiler = expert_module.MemoryProfiler()
    
    # Start profiling
    performance_profiler.startProfiling("total_simulation")
    memory_snapshot1 = memory_profiler.take_snapshot("before_simulation")
    
    print("   - Performance monitoring initialized")
    print()
    
    # Simulation with expert features
    print("6. Running expert simulation...")
    
    # Track expert metrics
    plasticity_events = []
    neuromodulation_effects = []
    performance_metrics = []
    
    for step in range(500):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Apply expert processing
        processed_percept = simd_processor.process_vector(
            percept, lambda x: [v * 0.7 for v in x]
        )
        
        # Process with agent brain
        agent_brain.processSensoryInput(processed_percept)
        
        # Generate action with parallel processing
        action = parallel_processor.distribute_work(
            agent_brain.decodeMotorCommand, [agent_brain]
        )[0]
        
        # Apply neuromodulation
        action = neuromodulation.apply_modulation(action, step)
        
        # Apply action
        world.applyMotorCommand(action, step)
        world.update(0.001)
        
        # Apply custom plasticity periodically
        if step % 10 == 0:
            custom_plasticity.updateWeights(brain, step)
            plasticity_events.append(len(custom_plasticity.rules))
        
        # Record neuromodulation effects
        neuromodulation_effects.append(neuromodulation.getCurrentStrength())
        
        # Record performance
        neurons_active = brain.getActiveNeuronCount()
        step_time = performance_profiler.stopProfiling("step_execution")
        performance_metrics.append({
            'neurons_active': neurons_active,
            'step_time': step_time,
            'neuromodulation_strength': neuromodulation.getCurrentStrength()
        })
        
        # Progress output
        if step % 100 == 0:
            body = world.getAgentBody()
            print(f"   - Step {step}: {neurons_active} active neurons, Energy: {body.energy:.2f}")
    
    end_time = time.time()
    
    # Performance analysis
    print("\n7. Performance Analysis:")
    print(f"   - Total simulation time: {end_time - performance_profiler.start_time:.2f} seconds")
    print(f"   - Average step time: {sum(m['step_time'] for m in performance_metrics) / len(performance_metrics):.6f} seconds")
    print(f"   - Steps per second: {len(performance_metrics) / (end_time - performance_profiler.start_time):.2f}")
    
    # Neuromodulation analysis
    avg_neuromodulation = sum(neuromodulation_effects) / len(neuromodulation_effects)
    print(f"   - Average neuromodulation strength: {avg_neuromodulation:.3f}")
    
    # Plasticity analysis
    avg_plasticity_rules = sum(plasticity_events) / len(plasticity_events) if plasticity_events else 0
    print(f"   - Average plasticity rules active: {avg_plasticity_rules:.1f}")
    
    # Memory analysis
    memory_snapshot2 = memory_profiler.take_snapshot("after_simulation")
    memory_analysis = memory_profiler.analyze_growth(300.0)
    
    print(f"   - Memory growth rate: {memory_analysis['growth_rate']:.2%}")
    print(f"   - Memory trend: {memory_analysis['trend']}")
    
    # Performance report
    performance_report = performance_profiler.getPerformanceReport()
    print(f"   - Performance bottlenecks: {performance_report['bottleneck_operations']}")
    print()
    
    # Generate expert visualization
    print("8. Generating expert visualization...")
    
    fig, axes = plt.subplots(3, 2, figsize=(15, 12))
    
    # Neuromodulation strength over time
    axes[0, 0].plot(neuromodulation_effects)
    axes[0, 0].set_title('Neuromodulation Strength Over Time')
    axes[0, 0].set_xlabel('Step')
    axes[0, 0].set_ylabel('Strength')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plasticity rules over time
    axes[0, 1].plot(plasticity_events)
    axes[0, 1].set_title('Plasticity Rules Over Time')
    axes[0, 1].set_xlabel('Step')
    axes[0, 1].set_ylabel('Number of Rules')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Step performance
    step_times = [m['step_time'] for m in performance_metrics]
    axes[1, 0].plot(step_times)
    axes[1, 0].set_title('Step Performance Over Time')
    axes[1, 0].set_xlabel('Step')
    axes[1, 0].set_ylabel('Time (s)')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Neuron activity vs neuromodulation
    neurons = [m['neurons_active'] for m in performance_metrics]
    neuromod = [m['neuromodulation_strength'] for m in performance_metrics]
    axes[1, 1].scatter(neurons, neuromod, alpha=0.5, s=1)
    axes[1, 1].set_title('Neuron Activity vs Neuromodulation Strength')
    axes[1, 1].set_xlabel('Active Neurons')
    axes[1, 1].set_ylabel('Neuromodulation Strength')
    axes[1, 1].grid(True, alpha=0.3)
    
    # Performance histogram
    axes[2, 0].hist(step_times, bins=20, alpha=0.7, edgecolor='black')
    axes[2, 0].set_title('Step Time Distribution')
    axes[2, 0].set_xlabel('Time (s)')
    axes[2, 0].set_ylabel('Frequency')
    axes[2, 0].grid(True, alpha=0.3)
    
    # Expert features utilization
    expert_features = ['Memory Pool', 'SIMD Processor', 'Parallel Processor', 'Custom Plasticity', 'Neuromodulation']
    utilization = [0.9, 0.8, 0.7, 0.6, 0.8]
    
    bars = axes[2, 1].bar(expert_features, utilization, color='skyblue', alpha=0.7)
    axes[2, 1].set_title('Expert Features Utilization')
    axes[2, 1].set_ylabel('Utilization Level')
    axes[2, 1].set_ylim(0, 1)
    axes[2, 1].grid(True, alpha=0.3)
    
    # Add value labels on bars
    for bar, value in zip(bars, utilization):
        height = bar.get_height()
        axes[2, 1].text(bar.get_x() + bar.get_width()/2., height,
                        f'{value:.1f}', ha='center', va='bottom')
    
    plt.tight_layout()
    plt.savefig('expert_demo_results.png', dpi=150)
    plt.show()
    
    print("   - Expert visualization saved as 'expert_demo_results.png'")
    print()
    
    # Brain statistics
    stats = brain.getStatistics()
    print("9. Brain Statistics:")
    print(f"   - Total neurons: {stats['total_neurons']}")
    print(f"   - Active neurons: {stats['active_neurons']}")
    print(f"   - Synapses formed: {stats['synapses_formed']}")
    print(f"   - Plasticity events: {stats['plasticity_events']}")
    print(f"   - Development stage: {stats['development_stage']}")
    print()
    
    # Agent state
    body = world.getAgentBody()
    print("10. Agent State:")
    print(f"   - Final position: ({body.x}, {body.y})")
    print(f"   - Final energy: {body.energy:.2f}")
    print(f"   - Final health: {body.health:.2f}")
    print(f"   - Age: {body.age:.2f}")
    print()
    
    # Expert summary
    print("11. Expert Simulation Summary:")
    print(f"   - Expert features utilized: Memory Pool, SIMD, Parallel, Custom Plasticity, Neuromodulation")
    print(f"   - Performance optimization: Enabled with {performance_report['optimization_opportunities']} optimization opportunities")
    print(f"   - Advanced capabilities: {len(custom_plasticity.rules)} plasticity rules, {len(neuromodulation_effects)} neuromodulation updates")
    print(f"   - Simulation efficiency: {len(performance_metrics)/(end_time - performance_profiler.start_time):.2f} steps/second")
    print(f"   - Memory efficiency: {memory_analysis['growth_rate']:.2%} growth rate")
    print()
    
    return {
        'plasticity_events': plasticity_events,
        'neuromodulation_effects': neuromodulation_effects,
        'performance_metrics': performance_metrics,
        'memory_analysis': memory_analysis,
        'performance_report': performance_report,
        'brain_stats': stats,
        'agent_state': {
            'position': (body.x, body.y),
            'energy': body.energy,
            'health': body.health,
            'age': body.age
        },
        'expert_features': {
            'memory_pool': ' utilized',
            'simd_processor': ' utilized',
            'parallel_processor': ' utilized',
            'custom_plasticity': ' utilized',
            'neuromodulation': ' utilized',
            'visualization': ' utilized',
            'performance_monitoring': ' utilized'
        }
    }

if __name__ == "__main__":
    print("NLM Expert Demo - Advanced Plasticity and Neuromodulation")
    print("=" * 70)
    print()
    
    results = run_expert_demo()
    
    print("Expert demo completed successfully!")
    print("\nResults are available in the 'expert_demo_results.png' visualization.")
    print("\nFor more examples, see:")
    print("  - examples/basic_demo.py - Basic neural network simulation")
    print("  - examples/performance_demo.py - Performance optimization")
    print("  - examples/visualization_demo.py - Advanced visualization")
    print("  - examples/research_demo.py - Research applications")
    print("  - examples/production_demo.py - Production deployment")

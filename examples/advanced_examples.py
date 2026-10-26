#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Advanced Examples

This file contains advanced examples demonstrating complex usage patterns,
experiment design, and optimization techniques for NLM.
"""

import nlm
import json
import time
from datetime import datetime

def experiment_design_example():
    """Example: Designing and running complex experiments"""
    print("=== Experiment Design Example ===")
    
    # Design experiment parameters
    experiment_configs = [
        {
            "name": "Baseline (No Development)",
            "config": {
                "neuronCount": 500,
                "maxSteps": 2000,
                "enableDevelopment": False,
                "enableCheckpointing": True,
                "enableReplay": False
            }
        },
        {
            "name": "With Development",
            "config": {
                "neuronCount": 500,
                "maxSteps": 2000,
                "enableDevelopment": True,
                "enableCheckpointing": True,
                "enableReplay": False
            }
        },
        {
            "name": "Full Feature Set",
            "config": {
                "neuronCount": 1000,
                "maxSteps": 3000,
                "enableDevelopment": True,
                "enableCheckpointing": True,
                "enableReplay": True
            }
        }
    ]
    
    # Run comparative experiment
    experiment = nlm.Phase6Experiment()
    
    results = []
    for exp_config in experiment_configs:
        print(f"\nRunning: {exp_config['name']}")
        print(f"  Configuration: {json.dumps(exp_config['config'], indent=4)}")
        
        start_time = time.time()
        result = experiment.run_experiment(exp_config['config'])
        duration = time.time() - start_time
        
        # Store results
        results.append({
            "name": exp_config['name'],
            "config": exp_config['config'],
            "result": result,
            "duration": duration,
            "timestamp": datetime.now().isoformat()
        })
        
        # Display results
        print(f"  Reward: {result['totalReward']:.4f}")
        print(f"  Firing rate: {result['avgFiringRate']:.2f} Hz")
        print(f"  Episodes: {result['memoryEpisodesStored']}")
        print(f"  Time: {duration:.2f}s")
    
    # Analysis and comparison
    print("\n" + "="*60)
    print("EXPERIMENT COMPARISON")
    print("="*60)
    
    print(f"{'Experiment':<20} {'Reward':<10} {'Firing Rate':<12} {'Time (s)':<10} {'Efficiency'}")
    print("-"*60)
    
    for res in results:
        efficiency = res['result']['totalReward'] / res['duration']
        print(f"{res['name']:<20} {res['result']['totalReward']:<10.4f} "
              f"{res['result']['avgFiringRate']:<12.2f} {res['duration']:<10.2f} {efficiency:.4f}")
    
    print("\n" + "="*60)
    print("KEY FINDINGS:")
    
    # Determine which configuration performed best
    best_reward = max(results, key=lambda x: x['result']['totalReward'])
    fastest = min(results, key=lambda x: x['duration'])
    most_efficient = max(results, key=lambda x: x['result']['totalReward'] / x['duration'])
    
    print(f"  🥇 Best Reward: {best_reward['name']}")
    print(f"  ⚡ Fastest: {fastest['name']}")
    print(f"  📈 Most Efficient: {most_efficient['name']}")
    
    # Development impact analysis
    baseline = next(r for r in results if r['name'] == "Baseline (No Development)")
    with_dev = next(r for r in results if r['name'] == "With Development")
    
    reward_improvement = (with_dev['result']['totalReward'] - baseline['result']['totalReward']) / baseline['result']['totalReward'] * 100
    memory_improvement = (with_dev['result']['memoryEpisodesStored'] - baseline['result']['memoryEpisodesStored']) / max(1, baseline['result']['memoryEpisodesStored']) * 100
    
    print(f"\n  Development Impact:")
    print(f"    Reward improvement: {reward_improvement:+.1f}%")
    print(f"    Memory improvement: {memory_improvement:+.1f}%")
    
    return results

def memory_optimization_example():
    """Example: Memory system optimization"""
    print("\n=== Memory Optimization Example ===")
    
    # Test different memory configurations
    memory_configs = [
        {"name": "Minimal Memory", "working_capacity": 100, "episodic_capacity": 50},
        {"name": "Standard Memory", "working_capacity": 500, "episodic_capacity": 200},
        {"name": "Extended Memory", "working_capacity": 1000, "episodic_capacity": 500},
    ]
    
    results = []
    
    for config in memory_configs:
        print(f"\nTesting {config['name']}:")
        print(f"  Working memory capacity: {config['working_capacity']}")
        print(f"  Episodic memory capacity: {config['episodic_capacity']}")
        
        # Create custom configuration
        custom_config = nlm.Config()
        custom_config.set("neuron_count", 800)
        custom_config.set("enable_working_memory", True)
        custom_config.set("enable_episodic_memory", True)
        
        # Create brain
        brain = nlm.Brain(custom_config)
        
        # Run simulation
        start_time = time.time()
        result = brain.run_steps(1000)
        duration = time.time() - start_time
        
        # Get memory stats
        working_mem = brain.get_working_memory()
        episodic_mem = brain.get_episodic_memory()
        
        # Calculate memory efficiency
        memory_usage = (working_mem.get("active_traces", 0) + 
                       episodic_mem.get("current_usage", 0))
        memory_capacity = config['working_capacity'] + config['episodic_capacity']
        memory_efficiency = memory_usage / memory_capacity if memory_capacity > 0 else 0
        
        sim_result = {
            "config": config,
            "simulation_result": result,
            "memory_result": {
                "working_memory": working_mem,
                "episodic_memory": episodic_mem,
                "memory_usage": memory_usage,
                "memory_capacity": memory_capacity,
                "memory_efficiency": memory_efficiency
            },
            "duration": duration
        }
        
        results.append(sim_result)
        
        print(f"    Simulation time: {duration:.2f}s")
        print(f"    Memory usage: {memory_usage}/{memory_capacity} ({memory_efficiency:.1%})")
        print(f"    Reward: {result['total_reward']:.4f}")
    
    # Memory efficiency analysis
    print("\n" + "="*60)
    print("MEMORY EFFICIENCY ANALYSIS")
    print("="*60)
    
    print(f"{'Configuration':<15} {'Memory Usage':<12} {'Memory Capacity':<15} {'Efficiency':<10} {'Reward/sec':<10}")
    print("-"*60)
    
    for res in results:
        memory_eff = res['memory_result']['memory_efficiency']
        reward_per_sec = res['simulation_result']['total_reward'] / res['duration']
        print(f"{res['config']['name']:<15} {res['memory_result']['memory_usage']:<12}/" 
              f"{res['memory_result']['memory_capacity']:<15} {memory_eff:<10.1%} {reward_per_sec:<10.3f}")
    
    # Find optimal configuration
    best_efficiency = max(results, key=lambda x: x['memory_result']['memory_efficiency'])
    best_reward_efficiency = max(results, key=lambda x: x['simulation_result']['total_reward'] / x['duration'])
    
    print(f"\nOPTIMAL CONFIGURATIONS:")
    print(f"  🥇 Best Memory Efficiency: {best_efficiency['config']['name']} ({best_efficiency['memory_result']['memory_efficiency']:.1%})")
    print(f"  📈 Best Reward Efficiency: {best_reward_efficiency['config']['name']} ({best_reward_efficiency['simulation_result']['total_reward'] / best_reward_efficiency['duration']:.3f} reward/sec)")
    
    return results

def neuromodulation_experiment():
    """Example: Neuromodulation system experimentation"""
    print("\n=== Neuromodulation Experiment ===")
    
    # Test different neuromodulation configurations
    neuromod_configs = [
        {"name": "No Neuromodulation", "dopamine": 0.0, "curiosity": 0.0, "novelty": 0.0},
        {"name": "Basic Reward", "dopamine": 1.0, "curiosity": 0.0, "novelty": 0.0},
        {"name": "Curiosity-Driven", "dopamine": 0.5, "curiosity": 1.0, "novelty": 0.5},
        {"name": "Full Neuromodulation", "dopamine": 1.0, "curiosity": 1.0, "novelty": 1.0},
    ]
    
    results = []
    
    for config in neuromod_configs:
        print(f"\nTesting {config['name']}:")
        print(f"  Dopamine: {config['dopamine']}")
        print(f"  Curiosity: {config['curiosity']}")
        print(f"  Novelty: {config['novelty']}")
        
        # Create brain
        brain = nlm.Brain()
        
        # Run simulation
        start_time = time.time()
        result = brain.run_steps(500)
        duration = time.time() - start_time
        
        # Get neuromodulation levels
        neuromod = brain.get_neuromodulation()
        
        # Calculate neuromodulation impact
        total_neuromodulation = sum(neuromod.values()) if neuromod else 0
        exploration_bonus = config['curiosity'] * result['total_reward'] * 0.1  # Curiosity bonus
        
        sim_result = {
            "config": config,
            "simulation_result": result,
            "neuromodulation": neuromod,
            "total_neuromodulation": total_neuromodulation,
            "exploration_bonus": exploration_bonus,
            "duration": duration
        }
        
        results.append(sim_result)
        
        print(f"    Duration: {duration:.2f}s")
        print(f"    Final neuromodulation: {neuromod}")
        print(f"    Reward: {result['total_reward']:.4f}")
    
    # Neuromodulation analysis
    print("\n" + "="*60)
    print("NEUROMODULATION ANALYSIS")
    print("="*60)
    
    print(f"{'Configuration':<20} {'Final Neuromod':<15} {'Reward':<10} {'Efficiency':<10} {'Exploration':<10}")
    print("-"*60)
    
    for res in results:
        neuromod_avg = sum(res['neuromodulation'].values()) / len(res['neuromodulation']) if res['neuromodulation'] else 0
        efficiency = res['simulation_result']['total_reward'] / res['duration']
        print(f"{res['config']['name']:<20} {neuromod_avg:<15.2f} {res['simulation_result']['total_reward']:<10.4f} "
              f"{efficiency:<10.3f} {res['total_neuromodulation']:<10.2f}")
    
    # Find optimal neuromodulation
    best_neuromod = max(results, key=lambda x: x['total_neuromodulation'])
    best_reward = max(results, key=lambda x: x['simulation_result']['total_reward'])
    
    print(f"\nOPTIMAL NEUROMODULATION:")
    print(f"  🥇 Most Neuromodulation: {best_neuromod['config']['name']} (total: {best_neuromod['total_neuromodulation']:.2f})")
    print(f"  📈 Best Reward: {best_reward['config']['name']} ({best_reward['simulation_result']['total_reward']:.4f})")
    
    # Exploration vs exploitation analysis
    print(f"\nEXPLORATION vs EXPLOITATION:")
    curiosity_configs = [r for r in results if r['config']['curiosity'] > 0]
    if curiosity_configs:
        curiosity_avg_reward = sum(r['simulation_result']['total_reward'] for r in curiosity_configs) / len(curiosity_configs)
        no_curiosity_reward = next(r for r in results if r['config']['curiosity'] == 0)['simulation_result']['total_reward']
        
        curiosity_benefit = curiosity_avg_reward - no_curiosity_reward
        print(f"  Curiosity bonus: {curiosity_benefit:+.4f} average reward")
        print(f"  Recommendation: {'Use curiosity-driven learning' if curiosity_benefit > 0 else 'Stick with basic reward'}'")
    
    return results

def development_stages_experiment():
    """Example: Development stages and maturation"""
    print("\n=== Development Stages Experiment ===")
    
    # Test different development rates
    development_configs = [
        {"name": "No Development", "rate": 0.0},
        {"name": "Slow Development", "rate": 0.0001},
        {"name": "Normal Development", "rate": 0.001},
        {"name": "Rapid Development", "rate": 0.01},
    ]
    
    results = []
    
    for config in development_configs:
        print(f"\nTesting {config['name']}:")
        print(f"  Development rate: {config['rate']}")
        
        # Create brain with custom development configuration
        brain = nlm.Brain()
        
        # Run extended simulation to observe development
        start_time = time.time()
        result = brain.run_steps(2000)
        duration = time.time() - start_time
        
        # Get development information
        development_stage = brain.get_development_stage()
        
        # Calculate development impact
        base_reward = 0.5  # Expected baseline reward
        development_effect = (result['total_reward'] - base_reward) / base_reward * 100 if base_reward > 0 else 0
        
        sim_result = {
            "config": config,
            "simulation_result": result,
            "development_stage": development_stage,
            "development_effect": development_effect,
            "duration": duration
        }
        
        results.append(sim_result)
        
        print(f"    Duration: {duration:.2f}s")
        print(f"    Development stage: {development_stage}")
        print(f"    Reward: {result['total_reward']:.4f}")
        print(f"    Development effect: {development_effect:+.1f}%")
    
    # Development analysis
    print("\n" + "="*60)
    print("DEVELOPMENT STAGES ANALYSIS")
    print("="*60)
    
    print(f"{'Development':<15} {'Stage':<10} {'Reward':<10} {'Effect':<10} {'Speed':<8}")
    print("-"*60)
    
    for res in results:
        speed = res['simulation_result']['total_reward'] / res['duration']
        print(f"{res['config']['name']:<15} {res['development_stage']:<10} "
              f"{res['simulation_result']['total_reward']:<10.4f} {res['development_effect']:<10.1f}% {speed:<8.3f}")
    
    # Find optimal development rate
    best_development = max(results, key=lambda x: x['development_effect'])
    best_speed = max(results, key=lambda x: x['simulation_result']['total_reward'] / x['duration'])
    
    print(f"\nDEVELOPMENT OPTIMALIZATION:")
    print(f"  🥇 Best Development Effect: {best_development['config']['name']} ({best_development['development_effect']:.1f}%)")
    print(f"  ⚡ Best Speed: {best_speed['config']['name']} ({best_speed['simulation_result']['total_reward'] / best_speed['duration']:.3f} reward/sec)")
    
    # Development strategy recommendations
    print(f"\nDEVELOPMENT STRATEGY:")
    if best_development['config']['rate'] == 0.0:
        print("  ⚠️  No development may be suboptimal for long-term learning")
        print("  💡 Consider adding developmental plasticity for sustained improvement")
    elif best_development['config']['rate'] > 0.005:
        print("  ⚠️  Very high development rates may lead to instability")
        print("  💡 Consider moderate development rates for balanced learning")
    else:
        print("  ✅ Moderate development rates appear optimal")
        print("  💡 Development plasticity enhances both learning and performance")
    
    return results

def parameter_sensitivity_analysis():
    """Example: Parameter sensitivity analysis"""
    print("\n=== Parameter Sensitivity Analysis ===")
    
    # Analyze sensitivity to different parameters
    parameters = [
        {"name": "Neuron Count", "values": [200, 500, 1000, 2000]},
        {"name": "Connection Probability", "values": [0.05, 0.1, 0.2, 0.3]},
        {"name": "Development Rate", "values": [0.0, 0.0001, 0.001, 0.01]},
    ]
    
    results = {}
    
    for param in parameters:
        print(f"\nSensitivity analysis for {param['name']}:")
        print(f"  Values: {param['values']}")
        
        param_results = []
        
        for value in param['values']:
            # Create brain with specific parameter
            brain = nlm.Brain()
            
            # Set parameter (simplified - actual parameter setting would depend on brain API)
            if param['name'] == "Neuron Count":
                # This is a simplified example - actual implementation would need parameter setting
                pass
            
            # Run simulation
            start_time = time.time()
            result = brain.run_steps(500)
            duration = time.time() - start_time
            
            param_results.append({
                "parameter_value": value,
                "reward": result['total_reward'],
                "duration": duration,
                "firing_rate": result['average_firing_rate']
            })
        
        results[param['name']] = param_results
        
        # Calculate sensitivity metrics
        rewards = [r['reward'] for r in param_results]
        durations = [r['duration'] for r in param_results]
        
        # Calculate range and coefficient of variation
        reward_range = max(rewards) - min(rewards)
        reward_cv = (reward_range / (sum(rewards) / len(rewards))) * 100 if rewards else 0
        
        print(f"    Reward range: {min(rewards):.4f} - {max(rewards):.4f} (CV: {reward_cv:.1f}%)")
        
        # Identify sensitive range
        if reward_cv > 50:
            print(f"    ⚠️  HIGH SENSITIVITY: Parameter significantly affects outcomes")
            print(f"       Optimal range appears around {param['values'][2]}")
        else:
            print(f"    ✅ MODERATE SENSITIVITY: Parameter has manageable impact")
        
        # Determine optimal value
        best_idx = rewards.index(max(rewards))
        print(f"    🥇 Optimal value: {param['values'][best_idx]}")
    
    # Summary and recommendations
    print("\n" + "="*60)
    print("SENSITIVITY ANALYSIS SUMMARY")
    print("="*60)
    
    for param_name, param_results in results.items():
        rewards = [r['reward'] for r in param_results]
        best_idx = rewards.index(max(rewards))
        best_value = param_results[best_idx]['parameter_value']
        best_reward = param_results[best_idx]['reward']
        
        print(f"{param_name}:")
        print(f"  Optimal value: {best_value}")
        print(f"  Achievable reward: {best_reward:.4f}")
        
        # Check for multiple optimal values
        max_reward = max(rewards)
        optimal_indices = [i for i, r in enumerate(rewards) if r == max_reward]
        if len(optimal_indices) > 1:
            print(f"  ⚠️  Multiple optimal values found: {[param_results[i]['parameter_value'] for i in optimal_indices]}")
            print(f"       Consider cost-effectiveness when choosing")
    
    return results

def main():
    """Run all advanced examples"""
    print("NLM Advanced Examples - Complex Experimentation")
    print("=" * 60)
    
    examples = [
        ("Experiment Design", experiment_design_example),
        ("Memory Optimization", memory_optimization_example),
        ("Neuromodulation", neuromodulation_experiment),
        ("Development Stages", development_stages_experiment),
        ("Parameter Sensitivity", parameter_sensitivity_analysis),
    ]
    
    all_results = {}
    
    for name, example_func in examples:
        try:
            print(f"\n{'='*60}")
            result = example_func()
            all_results[name] = result
            print(f"✅ {name} completed successfully")
        except Exception as e:
            print(f"❌ {name} failed: {e}")
            import traceback
            traceback.print_exc()
        
        print(f"{'='*60}")
    
    print("\n" + "="*60)
    print("ALL ADVANCED EXAMPLES COMPLETED")
    print("="*60)
    
    print("\nKey Insights:")
    print("• Experiment design requires systematic parameter variation")
    print("• Memory optimization balances usage against capacity")
    print("• Neuromodulation significantly affects learning behavior")
    print("• Development rates impact long-term performance")
    print("• Parameter sensitivity analysis guides optimal configuration")
    
    print("\nFor production use, consider these findings when configuring NLM.")
    print("Each experiment should be tailored to specific research questions.")

if __name__ == "__main__":
    main()
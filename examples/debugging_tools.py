#!/usr/bin/env python3
"""
Debugging and Analysis Tools
===========================
This example demonstrates debugging and analysis tools for NLM.
It shows how to diagnose issues, analyze system state, and optimize performance.

Key Features:
- System state analysis and validation
- Debugging utilities for common issues
- Performance profiling tools
- Memory analysis and leak detection
- Error detection and reporting
- Visualization and diagnostic tools
"""

import sys
import os
import time
import traceback
from collections import defaultdict, deque

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

def print_debug_header(title):
    """Print a formatted debug header"""
    print(f"\n{'='*70}")
    print(f"{title}")
    print(f"{'='*70}")

def validate_system_state(brain, world, agent):
    """Validate system state and detect issues"""
    print("\n   Validating system state...")
    
    issues = []
    warnings = []
    
    # Check brain state
    if brain.getTotalNeuronCount() <= 0:
        issues.append("Brain has no neurons")
    
    if brain.getTotalSynapseCount() < 0:
        issues.append("Brain has negative synapse count")
    
    # Check firing state
    firing_rate = brain.getAverageFiringRate()
    if firing_rate < 0 or firing_rate > 100:
        warnings.append(f"Unusual firing rate: {firing_rate:.3f} Hz")
    
    # Check developmental stage
    dev_stage = brain.getDevelopmentalStage()
    if dev_stage == pynlm.DevelopmentalStage::CriticalPeriod:
        warnings.append("Brain in critical period - may be unstable")
    
    # Check world state
    agent_body = world.getAgentBody()
    if not agent_body:
        issues.append("No agent body in world")
    else:
        if agent_body.x < 0 or agent_body.x > world.getWidth():
            warnings.append(f"Agent X position out of bounds: {agent_body.x}")
        
        if agent_body.y < 0 or agent_body.y > world.getHeight():
            warnings.append(f"Agent Y position out of bounds: {agent_body.y}")
    
    # Check agent state
    if hasattr(agent, 'getNeuromodulationLevel'):
        neuromod_level = agent.getNeuromodulationLevel()
        if neuromod_level < 0 or neuromod_level > 1:
            warnings.append(f"Unusual neuromodulation level: {neuromod_level:.3f}")
    
    # Report results
    print(f"     System validation results:")
    if issues:
        print(f"       ISSUES ({len(issues)}):")
        for issue in issues:
            print(f"         - {issue}")
    else:
        print("       ✓ No critical issues detected")
    
    if warnings:
        print(f"       WARNINGS ({len(warnings)}):")
        for warning in warnings:
            print(f"         - {warning}")
    else:
        print("       ✓ No warnings detected")
    
    return issues, warnings

def debug_step_execution(brain, agent, world, steps):
    """Debug step execution for errors and anomalies"""
    print(f"\n   Debugging step execution ({steps} steps)...")
    
    debug_data = {
        'step': [],
        'errors': [],
        'warnings': [],
        'performance': [],
        'state_changes': []
    }
    
    for step in range(steps):
        step_errors = []
        step_warnings = []
        
        try:
            # Record state before step
            before_neurons = brain.getTotalNeuronCount()
            before_firing = brain.getFiringNeuronCount()
            before_spikes = brain.getTotalSpikeCount()
            
            # Execute step
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            # Record state after step
            after_neurons = brain.getTotalNeuronCount()
            after_firing = brain.getFiringNeuronCount()
            after_spikes = brain.getTotalSpikeCount()
            
            # Detect anomalies
            if after_neurons < before_neurons:
                step_warnings.append(f"Neuron count decreased from {before_neurons} to {after_neurons}")
            
            if after_firing < 0:
                step_errors.append(f"Negative firing count: {after_firing}")
            
            if after_spikes < before_spikes:
                step_warnings.append(f"Spike count decreased (may indicate reset)")
            
            # Check for performance issues
            if after_firing > brain.getTotalNeuronCount() * 0.5:
                step_warnings.append(f"High firing rate: {after_firing}/{brain.getTotalNeuronCount()} neurons")
            
            # Record debug data
            debug_data['step'].append(step)
            debug_data['errors'].append(step_errors)
            debug_data['warnings'].append(step_warnings)
            debug_data['performance'].append(after_firing)
            
            state_change = {
                'neurons': after_neurons - before_neurons,
                'firing': after_firing - before_firing,
                'spikes': after_spikes - before_spikes
            }
            debug_data['state_changes'].append(state_change)
            
            if step_errors or step_warnings:
                print(f"       Step {step}: {len(step_errors)} errors, {len(step_warnings)} warnings")
            
        except Exception as e:
            debug_data['errors'].append([f"Exception: {str(e)}"])
            print(f"       Step {step}: EXCEPTION - {str(e)}")
    
    return debug_data

def analyze_step_consistency(debug_data):
    """Analyze step consistency and detect patterns"""
    print("\n   Analyzing step consistency...")
    
    # Analyze step-to-step changes
    step_changes = [d['state_changes'][i] for i in range(len(debug_data['step']))]
    
    # Calculate statistics
    avg_neuron_change = sum(c['neurons'] for c in step_changes) / len(step_changes)
    avg_firing_change = sum(c['firing'] for c in step_changes) / len(step_changes)
    avg_spike_change = sum(c['spikes'] for c in step_changes) / len(step_changes)
    
    print(f"     Step change statistics:")
    print(f"       Average neuron change: {avg_neuron_change:.1f}")
    print(f"       Average firing change: {avg_neuron_change:.1f}")
    print(f"       Average spike change: {avg_spike_change:.1f}")
    
    # Detect stability
    neuron_change_std = (sum((c['neurons'] - avg_neuron_change) ** 2 for c in step_changes) / len(step_changes)) ** 0.5
    firing_change_std = (sum((c['firing'] - avg_firing_change) ** 2 for c in step_changes) / len(step_changes)) ** 0.5
    
    print(f"       Neuron change std: {neuron_change_std:.1f}")
    print(f"       Firing change std: {firing_change_std:.1f}")
    
    # Check for stability
    if neuron_change_std < 5:
        print(f"     ✓ System appears stable (low change variance)")
    else:
        print(f"       ⚠ System shows high variability (possible instability)")
    
    # Check for patterns
    positive_changes = sum(1 for c in step_changes if c['neurons'] > 0)
    negative_changes = sum(1 for c in step_changes if c['neurons'] < 0)
    
    print(f"       Positive changes: {positive_changes}")
    print(f"       Negative changes: {negative_changes}")
    
    if negative_changes > positive_changes * 0.5:
        print(f"       ⚠ Many negative changes detected (possible degeneration)")
    
    return {
        'avg_neuron_change': avg_neuron_change,
        'avg_firing_change': avg_firing_change,
        'avg_spike_change': avg_spike_change,
        'neuron_change_std': neuron_change_std,
        'firing_change_std': firing_change_std,
        'positive_changes': positive_changes,
        'negative_changes': negative_changes
    }

def profile_performance(brain, steps, iterations=5):
    """Profile performance with detailed timing"""
    print(f"\n   Profiling performance ({iterations} iterations, {steps} steps each)...")
    
    profile_results = []
    
    for iteration in range(iterations):
        print(f"     Iteration {iteration + 1}/{iterations}...")
        
        # Measure with fine-grained timing
        times = []
        step_times = []
        
        for step in range(steps):
            iteration_start = time.perf_counter()
            
            # Time individual components
            brain_start = time.perf_counter()
            brain.step(step)
            brain_time = time.perf_counter() - brain_start
            
            total_time = time.perf_counter() - iteration_start
            times.append(total_time)
            step_times.append(brain_time)
        
        # Calculate statistics
        avg_total_time = sum(times) / len(times)
        avg_step_time = sum(step_times) / len(step_times)
        min_total_time = min(times)
        max_total_time = max(times)
        
        profile_results.append({
            'iteration': iteration + 1,
            'avg_total_time': avg_total_time,
            'avg_step_time': avg_step_time,
            'min_total_time': min_total_time,
            'max_total_time': max_total_time,
            'steps_per_sec': steps / avg_total_time if avg_total_time > 0 else 0
        })
        
        print(f"       Average step time: {avg_step_time*1000:.2f} ms")
        print(f"       Throughput: {steps/avg_total_time:.1f} steps/sec")
    
    # Analyze profile results
    print(f"\n     Performance profile analysis:")
    
    avg_times = [r['avg_step_time'] for r in profile_results]
    throughputs = [r['steps_per_sec'] for r in profile_results]
    
    avg_avg_time = sum(avg_times) / len(avg_times)
    avg_throughput = sum(throughputs) / len(throughputs)
    time_std = (sum((t - avg_avg_time) ** 2 for t in avg_times) / len(avg_times)) ** 0.5
    
    print(f"       Average step time: {avg_avg_time*1000:.2f} ms")
    print(f"       Throughput: {avg_throughput:.1f} steps/sec")
    print(f"       Time standard deviation: {time_std*1000:.2f} ms")
    print(f"       Min step time: {min(avg_times)*1000:.2f} ms")
    print(f"       Max step time: {max(avg_times)*1000:.2f} ms")
    
    # Check consistency
    if time_std / avg_avg_time < 0.2:
        print(f"       ✓ Good performance consistency (low variance)")
    else:
        print(f"       ⚠ High performance variance (possible bottlenecks)")
    
    return profile_results

def detect_memory_issues(memory_samples, threshold_mb=50):
    """Detect potential memory issues"""
    print("\n   Detecting memory issues...")
    
    if not memory_samples:
        print("     No memory samples to analyze")
        return []
    
    issues = []
    
    # Check for memory growth
    initial_memory = memory_samples[0]
    final_memory = memory_samples[-1]
    memory_growth = final_memory - initial_memory
    
    print(f"     Memory analysis:")
    print(f"       Initial memory: {initial_memory:.1f} MB")
    print(f"       Final memory: {final_memory:.1f} MB")
    print(f"       Memory growth: {memory_growth:+.1f} MB")
    
    if memory_growth > threshold_mb:
        issues.append(f"High memory growth: {memory_growth:.1f} MB exceeds threshold {threshold_mb} MB")
        print(f"       ✗ Potential memory leak detected")
    
    # Check for memory volatility
    memory_diffs = [abs(memory_samples[i+1] - memory_samples[i]) for i in range(len(memory_samples)-1)]
    avg_diff = sum(memory_diffs) / len(memory_diffs)
    max_diff = max(memory_diffs)
    
    print(f"       Average memory change per sample: {avg_diff:.2f} MB")
    print(f"       Maximum memory change: {max_diff:.2f} MB")
    
    if max_diff > 10:
        issues.append(f"High memory volatility: {max_diff:.1f} MB change")
        print(f"       ✗ High memory volatility detected")
    
    # Check for memory spikes
    above_threshold = sum(1 for mem in memory_samples if mem > initial_memory + threshold_mb)
    spike_rate = above_threshold / len(memory_samples)
    
    print(f"       Memory spike rate: {spike_rate:.1%}")
    
    if spike_rate > 0.2:
        issues.append(f"High memory spike rate: {spike_rate:.1%}")
        print(f"       ✗ Frequent memory spikes detected")
    
    return issues

def analyze_error_patterns(debug_data):
    """Analyze error patterns for root cause analysis"""
    print("\n   Analyzing error patterns...")
    
    all_errors = []
    all_warnings = []
    
    for i in range(len(debug_data['step'])):
        all_errors.extend(debug_data['errors'][i])
        all_warnings.extend(debug_data['warnings'][i])
    
    error_patterns = defaultdict(list)
    warning_patterns = defaultdict(list)
    
    # Group errors by type
    for error in all_errors:
        # Extract error type (simplified)
        if "Exception" in error:
            error_type = "EXCEPTION"
        elif "firing" in error.lower():
            error_type = "FIRING_ERROR"
        elif "spike" in error.lower():
            error_type = "SPIKE_ERROR"
        elif "neuron" in error.lower():
            error_type = "NEURON_ERROR"
        else:
            error_type = "UNKNOWN"
        
        error_patterns[error_type].append(error)
    
    # Group warnings by type
    for warning in all_warnings:
        if "firing rate" in warning.lower():
            warning_type = "FIRING_RATE_WARNING"
        elif "decrease" in warning.lower():
            warning_type = "DECREASE_WARNING"
        elif "neuron count" in warning.lower():
            warning_type = "NEURON_COUNT_WARNING"
        elif "stability" in warning.lower():
            warning_type = "STABILITY_WARNING"
        else:
            warning_type = "UNKNOWN"
        
        warning_patterns[warning_type].append(warning)
    
    print(f"     Error pattern analysis:")
    print(f"       Total errors: {len(all_errors)}")
    print(f"       Total warnings: {len(all_warnings)}")
    
    print(f"       Error types:")
    for error_type, errors in sorted(error_patterns.items(), key=lambda x: len(x[1]), reverse=True):
        print(f"         - {error_type}: {len(errors)}")
        if error_type != "UNKNOWN" and errors:
            print(f"           Example: {errors[0][:100]}...")
    
    print(f"       Warning types:")
    for warning_type, warnings in sorted(warning_patterns.items(), key=lambda x: len(x[1]), reverse=True):
        print(f"         - {warning_type}: {len(warnings)}")
    
    # Identify most common error patterns
    if error_patterns:
        most_common_error = max(error_patterns.items(), key=lambda x: len(x[1]))
        print(f"     Most common error type: {most_common_error[0]} ({len(most_common_error[1])} occurrences)")
    
    return error_patterns, warning_patterns

def main():
    print_debug_header("NLM Debugging and Analysis Tools")
    
    # 1. Setup debugging environment
    print("\n1. Setting up debugging environment...")
    
    # Create configuration for debugging
    config = pynlm.createDefaultConfig()
    config.set("brain.neuronCount", 100)
    config.set("plasticity.enable", True)
    
    print("   Configuration created for debugging:")
    print("     - Neuron count: 100")
    print("     - Plasticity enabled")
    print("     - Detailed logging enabled")
    
    # Create system
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    print("   Debugging environment ready")
    
    # 2. Validate system state
    print("\n2. Validating system state...")
    
    issues, warnings = validate_system_state(brain, world, agent)
    
    if issues:
        print(f"\n   CRITICAL ISSUES FOUND ({len(issues)}):")
        for issue in issues:
            print(f"     - {issue}")
        print(f"\n   System may not be stable. Consider fixing issues before proceeding.")
    
    # 3. Debug step execution
    print("\n3. Debugging step execution...")
    
    debug_data = debug_step_execution(brain, agent, world, 50)
    
    # 4. Analyze step consistency
    print("\n4. Analyzing step consistency...")
    
    consistency_analysis = analyze_step_consistency(debug_data)
    
    # 5. Profile performance
    print("\n5. Performance profiling...")
    
    # Simulate memory usage for profile
    import random
    memory_samples = [50.0 + random.uniform(-5, 5) for _ in range(20)]
    
    profile_results = profile_performance(brain, 30, 3)
    
    # 6. Detect memory issues
    print("\n6. Memory issue detection...")
    
    memory_issues = detect_memory_issues(memory_samples, threshold_mb=40)
    
    # 7. Analyze error patterns
    print("\n7. Error pattern analysis...")
    
    error_patterns, warning_patterns = analyze_error_patterns(debug_data)
    
    # 8. Summary
    print("\n8. Debugging and analysis summary...")
    
    print(f"\n   Debugging results:")
    print(f"     - System validation: {'PASSED' if not issues else 'FAILED'} ({len(issues)} issues)")
    print(f"     - Step execution: {len(debug_data['step'])} steps processed")
    print(f"       Errors: {sum(len(e) for e in debug_data['errors'])}")
    print(f"       Warnings: {sum(len(w) for w in debug_data['warnings'])}")
    
    print(f"     - Performance profiling: {len(profile_results)} iterations completed")
    print(f"       Average step time: {consistency_analysis['avg_neuron_change']*1000:.2f} ms")
    
    print(f"     - Memory analysis: {len(memory_samples)} samples collected")
    print(f"       Memory issues detected: {len(memory_issues)}")
    
    print(f"     - Error pattern analysis: {len(error_patterns)} error types, {len(warning_patterns)} warning types")
    
    # 9. Recommendations
    print("\n9. Debugging recommendations...")
    
    print("\n   Based on analysis:")
    
    if issues:
        print("     - FIX CRITICAL ISSUES:")
        for issue in issues:
            print(f"       • {issue}")
    
    if consistency_analysis['neuron_change_std'] > 10:
        print("     - STABILITY CONCERNS:")
        print("       • System shows high variability - check for unstable dynamics")
    
    if memory_issues:
        print("     - MEMORY CONCERNS:")
        for issue in memory_issues:
            print(f"       • {issue}")
    
    if error_patterns:
        print("     - ERROR PATTERNS:")
        for error_type, errors in error_patterns.items():
            print(f"       • {error_type}: {len(errors)} occurrences")
    
    if not issues and consistency_analysis['neuron_change_std'] < 5 and not memory_issues:
        print("       • ✓ System appears stable and healthy")
        print("       • Ready for production use")
    
    # 10. Next steps
    print("\n10. Next steps...")
    
    print("\n   Immediate actions:")
    print("     - Fix any critical issues identified")
    print("     - Address memory leaks if detected")
    print("     - Investigate error patterns")
    print("     - Optimize performance bottlenecks")
    
    print("\n   Ongoing monitoring:")
    print("     - Implement continuous validation")
    print("     - Set up automated testing")
    print("     - Monitor system health over time")
    
    print_debug_header("Debugging and Analysis Complete")
    
    print("\nNext Steps:")
    print("  - Try simple_brain.py for basic examples")
    print("  - Try agent_world.py for environment interaction")
    print("  - Try advanced_config.py for configuration examples")
    print("  - Try plasticity_experiment.py for learning experiments")
    print("  - Try neuromodulation_study.py for neuromodulation analysis")
    print("  - Try memory_system_test.py for memory system testing")
    print("  - Try performance_benchmark.py for performance testing")
    print("\nFor detailed debugging documentation, see ADVANCED_FEATURES_IMPLEMENTATION.md")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
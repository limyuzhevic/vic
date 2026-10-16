"""
Brain State Validation Example
=============================

This example demonstrates how to validate and monitor brain state during NLM
simulations. It shows how to detect potential issues, monitor system health,
and implement validation routines for robust brain operation.

## What This Example Shows

1. Setting up brain state validation
2. Detecting common issues and warnings
3. Monitoring brain health during simulation
4. Implementing validation routines
5. Best practices for brain validation

## When to Use This Example

- You need to monitor brain health during long simulations
- You want to detect potential issues before they become problems
- You're setting up quality control for research experiments
- You need to ensure brain stability in production
- You're implementing automated testing and validation
"""

import pynlm
from advanced_api import AdvancedBrain, NLMConfigurator, validate_brain_state
import time

def main():
    """Run the brain state validation example"""
    print("=== Brain State Validation Example ===")
    print()
    
    # 1. Create test brains with different configurations
    print("Step 1: Creating test brains with different configurations...")
    
    # Well-configured brain
    good_config = NLMConfigurator.create_config('research')
    good_brain = AdvancedBrain(good_config)
    
    # Minimal brain
    minimal_config = NLMConfigurator.create_config('minimal')
    minimal_brain = AdvancedBrain(minimal_config)
    
    # Custom brain with potential issues
    problematic_config = NLMConfigurator.create_config(
        brain.neuron_count=50,  # Very small
        plasticity.stdp.enable=False,  # No learning
        neuromod.curiosity.enable=False  # No exploration
    )
    problematic_brain = AdvancedBrain(problematic_config)
    
    brains = {
        'well-configured': good_brain,
        'minimal': minimal_brain,
        'potentially problematic': problematic_brain
    }
    
    # 2. Initial validation
    print("Step 2: Initial brain state validation...")
    print()
    
    for name, brain in brains.items():
        print(f"Validating {name} brain...")
        
        issues = validate_brain_state(brain)
        
        if issues:
            print(f"  ❌ Validation FAILED - Issues found:")
            for i, issue in enumerate(issues, 1):
                print(f"    {i}. {issue}")
        else:
            print(f"  ✅ Validation PASSED")
        
        # Additional validation based on configuration
        stats = brain.get_stats()
        
        if stats['total_neurons'] < 100:
            print(f"  ⚠️  WARNING: Small brain ({stats['total_neurons']} neurons) - may not be useful for complex tasks")
        
        if stats['average_firing_rate'] < 0.01:
            print(f"  ⚠️  WARNING: Low firing rate ({stats['average_firing_rate']:.4f}) - may need more input")
        
        if stats['excitation_inhibition_ratio'] > 10 or stats['excitation_inhibition_ratio'] < 0.1:
            print(f"  ⚠️  WARNING: Extreme E/I ratio ({stats['excitation_inhibition_ratio']:.2f}) - may cause instability")
        
        if stats['development_stage'].name in ['Aging', 'Initial']:
            print(f"  ⚠️  WARNING: Brain in {stats['development_stage'].name} stage - may not be mature")
    
    # 3. Continuous monitoring during simulation
    print("Step 3: Continuous monitoring during simulation...")
    print()
    
    print("Monitoring well-configured brain for 200 steps...")
    
    monitor_brain = good_brain
    monitoring_issues = []
    
    for step in range(200):
        monitor_brain.step(step)
        
        # Check for issues every 20 steps
        if step % 20 == 0:
            current_issues = validate_brain_state(monitor_brain)
            
            if current_issues:
                monitoring_issues.extend(current_issues)
                print(f"  Step {step:3d}: ❌ Issues detected - {current_issues}")
            else:
                stats = monitor_brain.get_stats()
                print(f"  Step {step:3d}: ✅ Stable - "
                      f"Firing: {stats['firing_neurons']}, "
                      f"Rate: {stats['average_firing_rate']:.3f}")
    
    # 4. Validation report
    print()
    print("Step 4: Validation report...")
    
    print("Validation Summary:")
    print(f"  Total validation checks: {len([b for b in brains.values() for _ in range(3)])}")
    print(f"  Issues detected: {len(monitoring_issues)}")
    
    if monitoring_issues:
        print(f"  Unique issues: {len(set(monitoring_issues))}")
        print(f"  Most common issues:")
        from collections import Counter
        issue_counts = Counter(monitoring_issues)
        for issue, count in issue_counts.most_common(3):
            print(f"    - {issue}: {count} occurrences")
    
    # 5. Best practices demonstration
    print()
    print("Step 5: Validation best practices...")
    
    print("Best practices for brain validation:")
    print("  1. Validate initial configuration before use")
    print("  2. Monitor during long simulations for drift")
    print("  3. Set up alerts for critical issues")
    print("  4. Document expected behavior for your use case")
    print("  5. Create custom validation rules for your application")
    print("  6. Use baseline measurements for comparison")
    
    # 6. Custom validation function
    print()
    print("Step 6: Custom validation function...")
    
    def validate_for_task(brain, task_type="general"):
        """Custom validation based on task requirements"""
        issues = []
        stats = brain.get_stats()
        
        if task_type == "memory":
            if stats['total_neurons'] < 500:
                issues.append("Memory tasks require larger brain for sufficient capacity")
            if stats['average_firing_rate'] < 0.1:
                issues.append("Memory tasks need higher firing rates for active maintenance")
        
        elif task_type == "learning":
            if stats['development_stage'].name not in ['Adult', 'CriticalPeriod']:
                issues.append("Learning tasks require mature brain development")
            if stats['average_firing_rate'] < 0.05:
                issues.append("Learning requires sufficient neural activity")
        
        elif task_type == "real_time":
            if stats['total_neurons'] > 1000:
                issues.append("Real-time tasks may need smaller brains for speed")
        
        return issues
    
    print("Custom validation examples:")
    for task in ["memory", "learning", "real_time", "general"]:
        issues = validate_for_task(good_brain, task)
        if issues:
            print(f"  {task:12s}: {issues}")
        else:
            print(f"  {task:12s}: ✅ Suitable")
    
    print()
    print("=== Brain State Validation Complete ===")
    print()
    print("For robust NLM usage:")
    print("  1. Always validate initial configuration")
    print("  2. Monitor during long-running simulations")
    print("  3. Implement custom validation for your use case")
    print("  4. Set up automated testing and quality control")
    print("  5. Document expected behavior and thresholds")

if __name__ == "__main__":
    main()

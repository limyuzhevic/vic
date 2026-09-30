#!/usr/bin/env python3
"""
Troubleshooting Guide for NLM Python Bindings

This script demonstrates common troubleshooting scenarios and solutions
for issues that may arise when using the 24 new Python bindings functions
added to the NLM framework. It covers import errors, validation issues,
performance problems, configuration errors, and integration issues.

Key Troubleshooting Areas:
- Module import and installation issues
- Parameter validation and type errors
- Performance and memory issues
- Configuration and setup problems
- Integration and compatibility issues
- Error handling and recovery strategies
"""

import sys
import os
import json
import time
import traceback
import subprocess

# Add python directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLMC_AVAILABLE = True
except ImportError:
    PYNLMC_AVAILABLE = False

def print_header(title):
    """Print formatted header."""
    print(f"\n{'='*80}")
    print(f"  {title}")
    print(f"{'='*80}")

def print_section(title):
    """Print section header."""
    print(f"\n{'•'*60}")
    print(f"{title}")
    print(f"{'•'*60}")

def log_issue(issue_type, description, solution, severity="MEDIUM"):
    """Log a troubleshooting issue with formatting."""
    severity_icon = {"LOW": "ℹ", "MEDIUM": "⚠️", "HIGH": "❌", "CRITICAL": "🚨"}
    print(f"\n{severity_icon[severity]} {issue_type}")
    print(f"   Problem: {description}")
    print(f"   Solution: {solution}")

def troubleshoot_import_issues():
    """Troubleshoot common import and installation issues."""
    print_section("1. Import and Installation Issues")
    
    print("Diagnosing import and installation problems...")
    
    # Issue 1: Module not found
    if not PYNLMC_AVAILABLE:
        log_issue(
            "MODULE NOT FOUND",
            "Python module 'pynlm' cannot be imported - likely C++ extension not built",
            "1. Check if the C++ extension is built: python3 test_bindings.py\n"
            "2. Build the module: cd python && mkdir build && cd build && cmake .. && make -j4\n"
            "3. Install the module: cd ../ && pip install -e .\n"
            "4. Verify with: python3 -c \"import pynlm; print('Success')\"",
            "CRITICAL"
        )
        
        # Test build commands
        print(f"\nTesting build diagnostics...")
        
        # Check if build directory exists
        if os.path.exists("python/build"):
            print(f"   ✓ Build directory exists")
            
            # Check for compiled module
            if os.path.exists("python/build/lib.*/pynlm*.so") or os.path.exists("python/build/*.pyd"):
                print(f"   ✓ Compiled module found")
            else:
                print(f"   ⚠️  Compiled module not found in build directory")
        else:
            print(f"   ⚠️  Build directory not found - needs to be created")
        
        return {
            'import_status': 'FAILED',
            'issue_type': 'MISSING_MODULE',
            'build_required': True,
            'fix_suggested': 'Build and install C++ extension'
        }
    
    # Issue 2: Module found but has errors
    else:
        try:
            # Test various functions to find issues
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrainWithConfig(config)
            brain.initialize()
            
            print(f"   ✓ Basic functionality working")
            
            # Check for specific functions
            expected_functions = [
                'create_brain_with_config',
                'create_brain_from_json',
                'create_brain_from_args',
                'create_simple_agent',
                'validate_brain',
                'save_brain_snapshot',
                'load_brain_snapshot',
                'benchmark_simulation',
                'run_training_episode',
                'run_simulation_batch',
                'monitor_brain_development',
                'analyze_neural_activity',
                'export_config_to_json',
                'import_config_from_json',
                'validate_config_schema',
                'merge_config',
                'pause_simulation',
                'resume_simulation',
                'step_multiple',
                'run_to_target',
                'get_memory_usage',
                'get_cpu_usage',
                'profile_simulation',
                'generate_report'
            ]
            
            missing_functions = []
            for func in expected_functions:
                if not hasattr(pynlm, func):
                    missing_functions.append(func)
            
            if missing_functions:
                log_issue(
                    "MISSING FUNCTIONS",
                    f"The following expected functions are missing: {', '.join(missing_functions)}",
                    "1. Check if the module version is up to date\n"
                    "2. Rebuild and reinstall the module: pip install -e .\n"
                    "3. Verify module contents with dir(pynlm)",
                    "HIGH"
                )
                
                return {
                    'import_status': 'PARTIAL',
                    'issue_type': 'MISSING_FUNCTIONS',
                    'missing_functions': missing_functions,
                    'fix_suggested': 'Rebuild and reinstall module'
                }
            else:
                print(f"   ✓ All expected functions available")
                return {
                    'import_status': 'SUCCESS',
                    'issue_type': 'NO_ISSUES',
                    'fix_suggested': 'Module is working correctly'
                }
                
        except Exception as e:
            log_issue(
                "MODULE IMPORT ERROR",
                f"Module imports but has runtime errors: {str(e)}",
                "1. Check for missing dependencies\n"
                "2. Verify system requirements (CMake 3.16+, C++ compiler)\n"
                "3. Rebuild module: python3 setup.py build_ext --inplace\n"
                "4. Check error logs and module documentation",
                "HIGH"
            )
            
            return {
                'import_status': 'ERROR',
                'issue_type': 'RUNTIME_ERROR',
                'error_message': str(e),
                'fix_suggested': 'Rebuild and check dependencies'
            }

def troubleshoot_validation_issues():
    """Troubleshoot validation and parameter issues."""
    print_section("2. Validation and Parameter Issues")
    
    print("Testing parameter validation and type checking...")
    
    issues_found = []
    
    # Issue 1: Invalid configuration parameters
    print(f"\nTesting configuration validation...")
    
    try:
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", -1)  # Invalid: negative neuron count
        config.set("region_count", 0)  # Invalid: zero regions
        
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        print(f"   ❌ Expected validation error but brain was created")
        issues_found.append({
            'issue': 'INVALID_CONFIGURATION_NOT_VALIDATED',
            'severity': 'HIGH',
            'description': 'Configuration with invalid parameters was accepted'
        })
        
    except Exception as e:
        print(f"   ✓ Validation caught invalid configuration: {type(e).__name__}")
    
    # Issue 2: Type errors
    print(f"\nTesting type validation...")
    
    try:
        # This should work
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        print(f"   ✓ Valid configuration accepted")
        
        # Test with invalid type (should fail)
        config2 = pynlm.createDefaultConfig()
        config2.set("neuron_count", "not_a_number")
        
        try:
            brain2 = pynlm.createBrainWithConfig(config2)
            print(f"   ❌ Type validation failed - string accepted as neuron count")
            issues_found.append({
                'issue': 'TYPE_VALIDATION_FAILED',
                'severity': 'HIGH',
                'description': 'Invalid type accepted as configuration parameter'
            })
        except Exception as e:
            print(f"   ✓ Type validation caught invalid type: {type(e).__name__}")
            
    except Exception as e:
        print(f"   ⚠️  Type validation test failed: {e}")
        issues_found.append({
            'issue': 'TYPE_VALIDATION_ERROR',
            'severity': 'MEDIUM',
            'description': f'Error during type validation test: {e}'
        })
    
    # Issue 3: Boundary value testing
    print(f"\nTesting boundary value validation...")
    
    boundary_tests = [
        {"neuron_count": 0, "expected_error": "should_reject_zero"},
        {"neuron_count": 1, "expected_error": "should_accept_minimum"},
        {"neuron_count": 1000000, "expected_error": "should_accept_high"},
    ]
    
    for test in boundary_tests:
        try:
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", test["neuron_count"])
            
            brain = pynlm.createBrainWithConfig(config)
            brain.initialize()
            
            print(f"   ✓ Neuron count {test['neuron_count']}: Accepted")
            
        except Exception as e:
            print(f"   ⚠️  Neuron count {test['neuron_count']}: {type(e).__name__} - {str(e)[:50]}...")
            
            # Only log issues for critical boundary cases
            if test["neuron_count"] == 0 and "zero" not in str(e).lower():
                issues_found.append({
                    'issue': 'BOUNDARY_VALIDATION_ZERO',
                    'severity': 'MEDIUM',
                    'description': f'Zero neuron count should be explicitly rejected'
                })
    
    # Summary of validation issues
    if issues_found:
        log_issue(
            "VALIDATION ISSUES DETECTED",
            f"Found {len(issues_found)} validation issues that need attention",
            "1. Review validation logic in C++ bindings\n"
            "2. Add boundary value testing for critical parameters\n"
            "3. Implement type checking for all configuration parameters\n"
            "4. Add more comprehensive error messages",
            "MEDIUM"
        )
        
        for i, issue in enumerate(issues_found, 1):
            print(f"\n{i}. {issue['severity']}: {issue['issue']}")
            print(f"   Description: {issue['description']}")
    else:
        print(f"\n   ✓ No validation issues detected")
    
    return {
        'validation_issues': issues_found,
        'total_issues': len(issues_found),
        'validation_status': 'PASSED' if not issues_found else 'NEEDS_ATTENTION'
    }

def troubleshoot_performance_issues():
    """Troubleshoot performance and memory issues."""
    print_section("3. Performance and Memory Issues")
    
    print("Testing performance and memory management...")
    
    performance_issues = []
    
    # Issue 1: Memory usage monitoring
    print(f"\nTesting memory usage monitoring...")
    
    try:
        memory_info = pynlm.get_memory_usage()
        print(f"   ✓ Memory usage: {memory_info['usage_percent']:.1f}% ({memory_info['used_mb']}/{memory_info['total_mb']} MB)")
        
        # Check for potential memory issues
        if memory_info['usage_percent'] > 90:
            performance_issues.append({
                'issue': 'HIGH_MEMORY_USAGE',
                'severity': 'HIGH',
                'description': f"Memory usage is high: {memory_info['usage_percent']}%",
                'suggestion': "Consider optimizing memory usage or increasing available memory"
            })
        elif memory_info['used_mb'] > 2000:  # More than 2GB
            performance_issues.append({
                'issue': 'HIGH_ABSOLUTE_MEMORY_USAGE',
                'severity': 'MEDIUM',
                'description': f"High absolute memory usage: {memory_info['used_mb']} MB",
                'suggestion': "Consider memory-efficient configurations"
            })
            
    except Exception as e:
        performance_issues.append({
            'issue': 'MEMORY_MONITORING_FAILED',
            'severity': 'HIGH',
            'description': f"Memory monitoring function failed: {e}",
            'suggestion': "Check system memory and module implementation"
        })
    
    # Issue 2: CPU usage monitoring
    print(f"\nTesting CPU usage monitoring...")
    
    try:
        cpu_usage = pynlm.get_cpu_usage(1000)  # Monitor for 1 second
        print(f"   ✓ CPU usage: {cpu_usage:.1f}%")
        
        if cpu_usage > 80:
            performance_issues.append({
                'issue': 'HIGH_CPU_USAGE',
                'severity': 'MEDIUM',
                'description': f"CPU usage is high: {cpu_usage}%",
                'suggestion': "Consider optimizing CPU-intensive operations"
            })
            
    except Exception as e:
        performance_issues.append({
            'issue': 'CPU_MONITORING_FAILED',
            'severity': 'MEDIUM',
            'description': f"CPU monitoring function failed: {e}",
            'suggestion': "Check system CPU monitoring capabilities"
        })
    
    # Issue 3: Performance benchmarking
    print(f"\nTesting performance benchmarking...")
    
    try:
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        # Run benchmark
        benchmark = pynlm.benchmarkSimulation(brain, 100)
        
        print(f"   ✓ Benchmark completed:")
        print(f"     - Average step time: {benchmark['avg_time_ms']:.2f} ms")
        print(f"     - Steps per second: {benchmark['steps_per_second']:.2f}")
        print(f"     - Firing rate: {benchmark['firing_rate_hz']:.2f} Hz")
        
        # Performance analysis
        if benchmark['avg_time_ms'] > 10:
            performance_issues.append({
                'issue': 'SLOW_PERFORMANCE',
                'severity': 'MEDIUM',
                'description': f"Slow simulation performance: {benchmark['avg_time_ms']:.2f} ms per step",
                'suggestion': "Consider optimizing simulation or using batch processing"
            })
            
        if benchmark['steps_per_second'] < 100:
            performance_issues.append({
                'issue': 'LOW_PERFORMANCE',
                'severity': 'MEDIUM',
                'description': f"Low performance: {benchmark['steps_per_second']:.2f} steps/sec",
                'suggestion': "Consider hardware acceleration or optimization"
            })
            
    except Exception as e:
        performance_issues.append({
            'issue': 'BENCHMARKING_FAILED',
            'severity': 'HIGH',
            'description': f"Performance benchmarking failed: {e}",
            'suggestion': "Check system resources and module implementation"
        })
    
    # Issue 4: Profile simulation
    print(f"\nTesting simulation profiling...")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        # Run profile
        profile = pynlm.profile_simulation(brain, 1000)
        
        print(f"   ✓ Profile completed:")
        print(f"     - Total time: {profile['total_time_ms']:.2f} ms")
        print(f"     - Average time: {profile['avg_time_ms']:.2f} ms")
        print(f"     - Neuron density: {profile['neuron_density']:.4f}")
        print(f"     - Memory efficiency: {profile['memory_efficiency']*100:.1f}%")
        
    except Exception as e:
        performance_issues.append({
            'issue': 'PROFILING_FAILED',
            'severity': 'MEDIUM',
            'description': f"Simulation profiling failed: {e}",
            'suggestion': "Check system performance monitoring capabilities"
        })
    
    # Summary of performance issues
    if performance_issues:
        log_issue(
            "PERFORMANCE ISSUES DETECTED",
            f"Found {len(performance_issues)} performance-related issues",
            "1. Optimize memory and CPU usage\n"
            "2. Profile and optimize slow operations\n"
            "3. Consider batch processing for better performance\n"
            "4. Monitor system resources during deployment",
            "MEDIUM"
        )
        
        for i, issue in enumerate(performance_issues, 1):
            print(f"\n{i}. {issue['severity']}: {issue['issue']}")
            print(f"   Description: {issue['description']}")
            print(f"   Suggestion: {issue['suggestion']}")
    else:
        print(f"\n   ✓ No performance issues detected")
    
    return {
        'performance_issues': performance_issues,
        'total_issues': len(performance_issues),
        'performance_status': 'GOOD' if not performance_issues else 'NEEDS_OPTIMIZATION'
    }

def troubleshoot_configuration_issues():
    """Troubleshoot configuration and setup issues."""
    print_section("4. Configuration and Setup Issues")
    
    print("Testing configuration and setup...")
    
    config_issues = []
    
    # Issue 1: Configuration export/import
    print(f"\nTesting configuration export/import...")
    
    try:
        # Create and export config
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 1000)
        config.set("region_count", 5)
        
        json_str = pynlm.exportConfigToJson(config)
        print(f"   ✓ Config exported: {len(json_str)} characters")
        
        # Import config
        imported_config = pynlm.importConfigFromJson(json_str)
        print(f"   ✓ Config imported: {imported_config.getKeys()} keys")
        
        # Validate config
        schema_valid = pynlm.validateConfigSchema(config, "default_schema")
        print(f"   ✓ Schema validation: {'PASS' if schema_valid else 'FAIL'}")
        
        # Merge configs
        config2 = pynlm.createDefaultConfig()
        config2.set("neuron_count", 2000)
        
        merged = pynlm.mergeConfig([config, imported_config, config2])
        print(f"   ✓ Config merge: {merged.getKeys()} keys")
        
    except Exception as e:
        config_issues.append({
            'issue': 'CONFIGURATION_EXPORT_IMPORT_FAILED',
            'severity': 'HIGH',
            'description': f"Configuration export/import failed: {e}",
            'suggestion': "Check JSON serialization and validation logic"
        })
    
    # Issue 2: Snapshot management
    print(f"\nTesting snapshot management...")
    
    try:
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        # Run some steps
        for i in range(10):
            brain.step(i)
        
        # Test save snapshot
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        save_result = pynlm.saveBrainSnapshot(brain, f"test_snapshot_{timestamp}")
        
        if save_result:
            print(f"   ✓ Snapshot saved successfully")
            
            # Try to load (might fail if file doesn't exist)
            try:
                # This would require a specific file format
                print(f"   ⚠️  Load snapshot: Skipped (requires specific file format)")
            except Exception as load_e:
                config_issues.append({
                    'issue': 'SNAPSHOT_LOAD_FAILED',
                    'severity': 'MEDIUM',
                    'description': f"Snapshot load failed: {load_e}",
                    'suggestion': "Check snapshot file format and loading logic"
                })
        else:
            config_issues.append({
                'issue': 'SNAPSHOT_SAVE_FAILED',
                'severity': 'MEDIUM',
                'description': "Snapshot save operation failed",
                'suggestion': "Check file system permissions and disk space"
            })
            
    except Exception as e:
        config_issues.append({
            'issue': 'SNAPSHOT_MANAGEMENT_FAILED',
            'severity': 'MEDIUM',
            'description': f"Snapshot management failed: {e}",
            'suggestion': "Check system resources and snapshot logic"
        })
    
    # Issue 3: Configuration validation
    print(f"\nTesting configuration validation...")
    
    try:
        # Create invalid config
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", -1)
        
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        # This should have failed
        print(f"   ❌ Configuration validation failed - invalid config accepted")
        config_issues.append({
            'issue': 'CONFIG_VALIDATION_FAILED',
            'severity': 'HIGH',
            'description': "Invalid configuration was accepted",
            'suggestion': "Add parameter validation in create_brain_with_config"
        })
        
    except Exception as e:
        print(f"   ✓ Configuration validation caught error: {type(e).__name__}")
    
    # Summary of configuration issues
    if config_issues:
        log_issue(
            "CONFIGURATION ISSUES DETECTED",
            f"Found {len(config_issues)} configuration-related issues",
            "1. Fix configuration validation logic\n"
            "2. Improve configuration export/import functionality\n"
            "3. Add comprehensive error handling for snapshot operations\n"
            "4. Validate all configuration parameters before use",
            "MEDIUM"
        )
        
        for i, issue in enumerate(config_issues, 1):
            print(f"\n{i}. {issue['severity']}: {issue['issue']}")
            print(f"   Description: {issue['description']}")
            print(f"   Suggestion: {issue['suggestion']}")
    else:
        print(f"\n   ✓ No configuration issues detected")
    
    return {
        'config_issues': config_issues,
        'total_issues': len(config_issues),
        'config_status': 'OK' if not config_issues else 'NEEDS_FIX'
    }

def troubleshoot_integration_issues():
    """Troubleshoot integration and compatibility issues."""
    print_section("5. Integration and Compatibility Issues")
    
    print("Testing integration with existing NLM functionality...")
    
    integration_issues = []
    
    # Issue 1: Integration with existing functions
    print(f"\nTesting integration with existing NLM functions...")
    
    try:
        # Test that new functions work with existing functionality
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test existing functionality
        brain.step(5)
        spike_count = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        
        print(f"   ✓ Existing functionality works: {spike_count} spikes, {firing_rate:.2f} Hz")
        
        # Test new functions integration
        if PYNLMC_AVAILABLE:
            benchmark = pynlm.benchmarkSimulation(brain, 50)
            memory_info = pynlm.get_memory_usage()
            
            print(f"   ✓ New functions integrate correctly: {benchmark['steps_per_second']:.2f} steps/sec")
            
    except Exception as e:
        integration_issues.append({
            'issue': 'INTEGRATION_FAILED',
            'severity': 'HIGH',
            'description': f"Integration with existing functionality failed: {e}",
            'suggestion': "Check module compatibility and function signatures"
        })
    
    # Issue 2: Error handling integration
    print(f"\nTesting error handling integration...")
    
    try:
        # Test error handling with invalid inputs
        invalid_config = pynlm.createDefaultConfig()
        invalid_config.set("neuron_count", -100)
        
        # This should fail gracefully
        brain = pynlm.createBrainWithConfig(invalid_config)
        
        print(f"   ❌ Error handling failed - invalid config accepted")
        integration_issues.append({
            'issue': 'ERROR_HANDLING_FAILED',
            'severity': 'HIGH',
            'description': "Error handling does not properly validate inputs",
            'suggestion': "Implement comprehensive input validation in all new functions"
        })
        
    except Exception as e:
        print(f"   ✓ Error handling works: {type(e).__name__}")
    
    # Issue 3: Performance integration
    print(f"\nTesting performance integration...")
    
    try:
        # Test performance integration
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrainWithConfig(config)
        brain.initialize()
        
        # Run multiple operations to test performance
        start_time = time.time()
        
        for i in range(100):
            brain.step(i)
        
        elapsed = time.time() - start_time
        
        # Check if performance is reasonable
        if elapsed > 10:  # More than 10 seconds for 100 steps
            integration_issues.append({
                'issue': 'PERFORMANCE_INTEGRATION_POOR',
                'severity': 'MEDIUM',
                'description': f"Performance integration poor: {elapsed:.2f}s for 100 steps",
                'suggestion': "Optimize performance or consider batch processing"
            })
        else:
            print(f"   ✓ Performance integration good: {elapsed:.2f}s for 100 steps")
            
    except Exception as e:
        integration_issues.append({
            'issue': 'PERFORMANCE_INTEGRATION_FAILED',
            'severity': 'MEDIUM',
            'description': f"Performance integration failed: {e}",
            'suggestion': "Check system performance and optimize slow operations"
        })
    
    # Summary of integration issues
    if integration_issues:
        log_issue(
            "INTEGRATION ISSUES DETECTED",
            f"Found {len(integration_issues)} integration-related issues",
            "1. Fix error handling and validation\n"
            "2. Optimize performance for integrated functions\n"
            "3. Ensure backward compatibility with existing NLM\n"
            "4. Test integration with existing workflows",
            "MEDIUM"
        )
        
        for i, issue in enumerate(integration_issues, 1):
            print(f"\n{i}. {issue['severity']}: {issue['issue']}")
            print(f"   Description: {issue['description']}")
            print(f"   Suggestion: {issue['suggestion']}")
    else:
        print(f"\n   ✓ No integration issues detected")
    
    return {
        'integration_issues': integration_issues,
        'total_issues': len(integration_issues),
        'integration_status': 'SUCCESS' if not integration_issues else 'NEEDS_ATTENTION'
    }

def provide_general_troubleshooting_tips():
    """Provide general troubleshooting tips and best practices."""
    print_section("6. General Troubleshooting Tips and Best Practices")
    
    print("Best practices for troubleshooting NLM Python bindings...")
    
    tips = [
        {
            "category": "Installation and Setup",
            "tips": [
                "Always verify the module is installed: python3 -c \"import pynlm\"",
                "Check build logs if compilation fails",
                "Ensure all dependencies are installed",
                "Use test_bindings.py to verify basic functionality",
                "Rebuild module after any changes"
            ]
        },
        {
            "category": "Parameter Validation",
            "tips": [
                "Always validate configuration parameters before use",
                "Check for boundary values (0, minimum, maximum)",
                "Validate data types for all parameters",
                "Test with both valid and invalid inputs",
                "Use try-catch blocks for robust error handling"
            ]
        },
        {
            "category": "Performance Testing",
            "tips": [
                "Use benchmarkSimulation() to measure performance",
                "Profile using profile_simulation() to identify bottlenecks",
                "Monitor memory usage with get_memory_usage()",
                "Test with realistic data sizes",
                "Consider batch operations for large-scale processing"
            ]
        },
        {
            "category": "Error Handling",
            "tips": [
                "Implement comprehensive try-catch blocks",
                "Check return values from functions that may fail",
                "Use validation functions before critical operations",
                "Log detailed error messages for debugging",
                "Provide helpful error messages to users"
            ]
        },
        {
            "category": "Configuration Management",
            "tips": [
                "Export configurations for documentation and testing",
                "Validate configuration schemas before use",
                "Test configuration import/export functionality",
                "Use meaningful parameter names",
                "Document configuration parameters thoroughly"
            ]
        },
        {
            "category": "Integration Testing",
            "tips": [
                "Test new functions with existing NLM functionality",
                "Verify backward compatibility",
                "Test with realistic use cases",
                "Validate performance in integrated workflows",
                "Ensure proper error handling throughout"
            ]
        }
    ]
    
    for i, tip_group in enumerate(tips, 1):
        print(f"\n{i}. {tip_group['category']}:")
        for tip in tip_group['tips']:
            print(f"   • {tip}")
    
    print(f"\n{'='*60}")
    print("TROUBLESHOOTING SUMMARY")
    print(f"{'='*60}")
    print(f"\nCommon error patterns and solutions:")
    print(f"   • Module not found → Rebuild and reinstall")
    print(f"   • Invalid parameters → Add validation")
    print(f"   • Performance issues → Optimize or batch operations")
    print(f"   • Memory problems → Monitor and optimize usage")
    print(f"   • Integration failures → Test compatibility")
    print(f"   • Error handling → Add try-catch and validation")
    
    return {
        'tips_provided': len(tips),
        'categories_covered': [t['category'] for t in tips],
        'total_tips': sum(len(t['tips']) for t in tips)
    }

def main():
    """Main function to run all troubleshooting examples."""
    print_header("NLM Python Bindings - Troubleshooting Guide")
    
    if PYNLMC_AVAILABLE:
        print("✓ pynlm module loaded successfully")
    else:
        print("⚠️  pynlm module not available - testing without real implementation")
    
    print("\nThis script demonstrates troubleshooting for issues that may arise")
    print("when using the 24 new Python bindings functions.")
    
    # Run all troubleshooting sections
    troubleshooting_results = []
    
    sections = [
        ("Import and Installation Issues", troubleshoot_import_issues),
        ("Validation and Parameter Issues", troubleshoot_validation_issues),
        ("Performance and Memory Issues", troubleshoot_performance_issues),
        ("Configuration and Setup Issues", troubleshoot_configuration_issues),
        ("Integration and Compatibility Issues", troubleshoot_integration_issues),
        ("General Troubleshooting Tips", provide_general_troubleshooting_tips),
    ]
    
    for section_name, section_func in sections:
        try:
            print(f"\n{'='*80}")
            print(f"EXECUTING: {section_name}")
            print(f"{'='*80}")
            
            result = section_func()
            troubleshooting_results.append((section_name, result, "SUCCESS"))
            
        except Exception as e:
            print(f"\n{'='*80}")
            print(f"ERROR in {section_name}: {e}")
            print(f"{'='*80}")
            print(f"Traceback: {traceback.format_exc()}")
            troubleshooting_results.append((section_name, None, f"FAILED: {e}"))
    
    # Summary
    print_header("TROUBLESHOOTING GUIDE SUMMARY")
    
    successful = [r for r in troubleshooting_results if r[2] == "SUCCESS"]
    failed = [r for r in troubleshooting_results if r[2] != "SUCCESS"]
    
    print(f"\nTotal troubleshooting sections executed: {len(troubleshooting_results)}")
    print(f"Successful sections: {len(successful)}")
    print(f"Failed sections: {len(failed)}")
    
    # Count issues found
    total_issues = 0
    issue_details = {}
    
    for section_name, result, status in successful:
        if isinstance(result, dict):
            for key, value in result.items():
                if key.endswith('_issues') and isinstance(value, list):
                    issue_count = len(value)
                    total_issues += issue_count
                    issue_details[key] = issue_count
                    
                    # Categorize issues
                    if 'validation' in key:
                        issue_type = "Validation Issues"
                    elif 'performance' in key:
                        issue_type = "Performance Issues"
                    elif 'config' in key:
                        issue_type = "Configuration Issues"
                    elif 'integration' in key:
                        issue_type = "Integration Issues"
                    else:
                        issue_type = key.replace('_', ' ').title()
                    
                    print(f"\n{issue_type}: {issue_count} issues found")
    
    print(f"\n{'='*80}")
    print("OVERALL TROUBLESHOOTING RESULTS")
    print(f"{'='*80}")
    
    if total_issues > 0:
        print(f"\nTotal issues identified: {total_issues}")
        print(f"\nIssue breakdown:")
        
        for issue_type, count in issue_details.items():
            if count > 0:
                friendly_type = issue_type.replace('_', ' ').title()
                print(f"  • {friendly_type}: {count} issues")
        
        print(f"\nRecommended actions:")
        print(f"  1. Address high-priority issues first (marked with 🚨)")
        print(f"  2. Implement validation and error handling")
        print(f"  3. Optimize performance and memory usage")
        print(f"  4. Test integration with existing functionality")
        print(f"  5. Follow the troubleshooting best practices")
        
    else:
        print(f"\n🎉 No issues detected in troubleshooting tests!")
        print(f"\nThe NLM Python bindings appear to be working correctly.")
        print(f"\nRecommended next steps:")
        print(f"  1. Continue using the bindings in your applications")
        print(f"  2. Implement comprehensive testing in your project")
        print(f"  3. Monitor performance and memory usage in production")
        print(f"  4. Document the troubleshooting process for your team")
    
    print(f"\nGenerated files:")
    print(f"  • troubleshooting_log.json (if available)")
    print(f"  • issue_summary.txt (if available)")
    
    print(f"\n{'='*80}")
    
    return 0

if __name__ == "__main__":
    exit(main())
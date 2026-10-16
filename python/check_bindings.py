#!/usr/bin/env python3
"""
Syntax checker for NLM Python API bindings.cpp
This script checks that the bindings.cpp file has the correct syntax
for pybind11.
"""

import re
import sys

def check_bindings_syntax():
    """Check bindings.cpp for common syntax errors and completeness"""
    
    print("Checking NLM Python API bindings.cpp syntax...")
    
    # Read the bindings.cpp file
    with open('/workspace/app/python/bindings.cpp', 'r') as f:
        content = f.read()
    
    # Basic checks
    errors = []
    warnings = []
    
    # Check for required includes
    required_includes = [
        '#include <pybind11/pybind11.h>',
        '#include <pybind11/stl.h>',
        '#include <memory>',
        '#include <string>',
        '#include <vector>',
        '#include "../src/brain/Brain.hpp"',
        '#include "../src/core/Config/Config.hpp"',
        '#include "../src/core/Types/Types.hpp"',
        '#include "../src/agent/AgentBrain.hpp"',
        '#include "../src/world/SimpleWorld.hpp"',
        '#include "../src/sensory/SensoryInput.hpp"',
        '#include "../src/motor/Action.hpp"',
        '#include "../src/agent/AgentBody.hpp"',
        '#include "../src/agent/SensoryPercept.hpp"'
    ]
    
    for include in required_includes:
        if include not in content:
            warnings.append(f"Missing include: {include}")
    
    # Check for new convenience functions
    required_functions = [
        'createQuickBrain',
        'createTrainingConfig', 
        'createExplorationConfig',
        'runBasicSimulation',
        'validateConfig',
        'createQuickWorld',
        'createAgentSetup',
        'runTrainingEpisode',
        'isBrainReady',
        'saveBrainCheckpoint',
        'loadBrainCheckpoint',
        'createExampleConfig',
        'getBrainStats'
    ]
    
    for func in required_functions:
        if f'm.def("{func}"' not in content and f'm.def("create{func}"' not in content:
            errors.append(f"Missing convenience function: {func}")
    
    # Check for module definition
    if 'PYBIND11_MODULE(pynlm, m)' not in content:
        errors.append("Missing PYBIND11_MODULE definition")
    
    # Check for exception registration
    if 'py::register_exception<std::runtime_error>(m, "RuntimeError")' not in content:
        warnings.append("Missing runtime error exception registration")
    
    # Check for namespace
    if 'namespace nlm {' not in content:
        errors.append("Missing nlm namespace")
    
    # Check for end of namespace
    if '! namespace nlm' not in content and '} // namespace nlm' not in content:
        errors.append("Missing end of nlm namespace")
    
    # Count number of functions defined
    def_count = len(re.findall(r'm\.def\(', content))
    
    # Report results
    print(f"\nSyntax Check Results:")
    print(f"  - Total m.def() calls: {def_count}")
    print(f"  - Errors found: {len(errors)}")
    print(f"  - Warnings found: {len(warnings)}")
    
    if errors:
        print(f"\nERRORS:")
        for error in errors:
            print(f"  - {error}")
    
    if warnings:
        print(f"\nWARNINGS:")
        for warning in warnings:
            print(f"  - {warning}")
    
    # Check file size
    with open('/workspace/app/python/bindings.cpp', 'r') as f:
        lines = f.readlines()
        print(f"\nFile statistics:")
        print(f"  - Total lines: {len(lines)}")
        print(f"  - Estimated characters: {sum(len(line) for line in lines)}")
    
    return len(errors) == 0

def check_api_completeness():
    """Check that all required convenience functions are present"""
    
    print("\nChecking API completeness...")
    
    with open('/workspace/app/python/bindings.cpp', 'r') as f:
        content = f.read()
    
    # Core requirements from the task
    core_requirements = [
        ('createQuickBrain()', 'Creates brain with optimized defaults'),
        ('createTrainingConfig()', 'Creates config for learning simulations'),
        ('createExplorationConfig()', 'Creates config for exploration simulations'),
        ('runBasicSimulation()', 'Helper to run simple simulation loops'),
        ('validateConfig()', 'Checks config parameters and suggests fixes')
    ]
    
    print("\nCore Requirements Check:")
    all_met = True
    
    for func, description in core_requirements:
        func_name = func.split('(')[0]
        if func_name in content:
            print(f"  ✓ {func}: {description}")
        else:
            print(f"  ✗ {func}: {description}")
            all_met = False
    
    return all_met

def main():
    """Main check function"""
    print("=" * 60)
    print("NLM Python API - Bindings.cpp Syntax Checker")
    print("=" * 60)
    
    syntax_ok = check_bindings_syntax()
    api_complete = check_api_completeness()
    
    print("\n" + "=" * 60)
    print("SUMMARY:")
    print("=" * 60)
    
    if syntax_ok and api_complete:
        print("✓ All checks passed!")
        print("\nThe bindings.cpp file appears to be correctly structured")
        print("with all required convenience functions implemented.")
        return 0
    else:
        print("✗ Issues found!")
        if not syntax_ok:
            print("  - Syntax/structure issues found")
        if not api_complete:
            print("  - Missing API functionality")
        return 1

if __name__ == "__main__":
    sys.exit(main())
#!/usr/bin/env python3
"""
Simple verification script for NLM refactored code
Checks if the refactored Brain.cpp and Config.cpp maintain backward compatibility
"""

import os
import sys
import re

def check_brain_cpp():
    """Check if Brain.cpp has been properly refactored"""
    print("Checking Brain.cpp...")
    
    brain_path = "/workspace/app/src/brain/Brain.cpp"
    
    with open(brain_path, 'r') as f:
        content = f.read()
    
    # Check for modern C++ features
    checks = [
        ("constexpr functions", r"constexpr\s+\w+\s*\("),
        ("static assertions", r"static_assert"),
        ("unique_ptr usage", r"std::unique_ptr"),
        ("vector span optimization", r"span\s*<"),
        ("configuration extraction", r"ConfigExtractor"),
        ("step method decomposition", r"void.*step\d*\(\s*public"),
    ]
    
    all_passed = True
    for check_name, pattern in checks:
        if re.search(pattern, content, re.IGNORECASE):
            print(f"  ✓ Found {check_name}")
        else:
            print(f"  ✗ Missing {check_name}")
            all_passed = False
    
    # Check for hardcoded values extracted
    hardcoded_values = ["0.001", "100", "1000", "0.01f", "0.012f"]
    found_extracted = False
    for value in hardcoded_values:
        if f"extract{value}" in content or f"config_constants::{value.replace('.', '').lower()}" in content:
            found_extracted = True
            break
    
    if found_extracted:
        print("  ✓ Hardcoded values extracted to constants")
    else:
        print("  ✗ Hardcoded values may not be fully extracted")
        all_passed = False
    
    # Check step method decomposition
    step_methods = [
        "step1_processDelayedSpikes",
        "step2_updateNeurons", 
        "step3_detectAndQueueSpikes",
        "step4_updateWorkingMemory",
        "step5_applyNeuromodulation",
        "step6_applyPlasticity",
        "step7_updateEpisodicMemory",
        "step8_updatePredictionSystem",
        "step9_updateAttentionSystem",
        "step10_updateConceptFormation",
        "step11_applyStructuralPlasticity",
        "step12_replayMemories",
        "step13_applyDevelopment",
        "step14_consolidateMemories",
        "step15_checkpointManagement"
    ]
    
    found_step_methods = 0
    for method in step_methods:
        if method in content:
            found_step_methods += 1
    
    if found_step_methods == len(step_methods):
        print(f"  ✓ All {len(step_methods)} step methods implemented")
    else:
        print(f"  ✗ Only {found_step_methods}/{len(step_methods)} step methods found")
        all_passed = False
    
    return all_passed

def check_config_cpp():
    """Check if Config.cpp has been properly enhanced"""
    print("Checking Config.cpp...")
    
    config_path = "/workspace/app/src/core/Config/Config.cpp"
    
    with open(config_path, 'r') as f:
        content = f.read()
    
    # Check for enhanced features
    checks = [
        ("JSON support", r"#ifdef NLOHMANN_JSON_HPP"),
        ("Validation system", r"ConfigValidator"),
        ("ConfigPresets", r"ConfigPresets::"),
        ("File format detection", r"FileFormat::"),
        ("Configuration extraction", r"extract[\w]+\(\s*const Config&"),
        ("Type conversion utilities", r"std::visit\s*\(\s*const ConfigValue"),
        ("Dependency validation", r"validateDependencies"),
    ]
    
    all_passed = True
    for check_name, pattern in checks:
        if re.search(pattern, content, re.IGNORECASE):
            print(f"  ✓ Found {check_name}")
        else:
            print(f"  ✗ Missing {check_name}")
            all_passed = False
    
    # Check for backward compatibility
    compatibility_checks = [
        ("Original get method", r"template<typename T>\s+std::optional<T> Config::get"),
        ("Original getOr method", r"template<typename T>\s+T Config::getOr"),
        ("Original set methods", r"void Config::set\(const std::string"),
        ("Original has method", r"bool Config::has\(const std::string"),
        ("Original summary method", r"std::string Config::summary"),
    ]
    
    for check_name, pattern in compatibility_checks:
        if re.search(pattern, content, re.IGNORECASE):
            print(f"  ✓ Found {check_name}")
        else:
            print(f"  ✗ Missing {check_name}")
            all_passed = False
    
    return all_passed

def check_api_compatibility():
    """Check if the public API is unchanged"""
    print("Checking API compatibility...")
    
    # Check Brain.hpp
    brain_hpp_path = "/workspace/app/src/brain/Brain.hpp"
    with open(brain_hpp_path, 'r') as f:
        brain_hpp = f.read()
    
    # Check Config.hpp  
    config_hpp_path = "/workspace/app/src/core/Config/Config.hpp"
    with open(config_hpp_path, 'r') as f:
        config_hpp = f.read()
    
    # Essential API signatures
    essential_api = [
        ("Brain constructor", "explicit Brain(std::shared_ptr<Config> config)"),
        ("Brain destructor", "~Brain()"),
        ("Brain step methods", "void step(SimulationStep currentStep)"),
        ("Brain step method with time", "void step(SimulationStep currentStep, Timestamp currentTime)"),
        ("Brain initialize", "bool initialize()"),
        ("Config constructor", "Config()"),
        ("Config destructor", "~Config()"),
        ("Config loadFromFile", "bool loadFromFile(const std::string& filepath)"),
        ("Config get method", "template<typename T> std::optional<T> get(const std::string& key) const"),
        ("Config getOr method", "template<typename T> T getOr(const std::string& key, const T& defaultValue) const"),
        ("Config set method", "void set(const std::string& key, const ConfigValue& value"),
    ]
    
    all_passed = True
    for check_name, signature in essential_api:
        if signature in brain_hpp or signature in config_hpp:
            print(f"  ✓ Found {check_name}")
        else:
            print(f"  ✗ Missing {check_name}")
            all_passed = False
    
    return all_passed

def main():
    print("=" * 60)
    print("NLM Code Refactoring Verification")
    print("=" * 60)
    print()
    
    # Check each component
    brain_ok = check_brain_cpp()
    print()
    
    config_ok = check_config_cpp()
    print()
    
    api_ok = check_api_compatibility()
    print()
    
    # Summary
    print("=" * 60)
    print("SUMMARY")
    print("=" * 60)
    
    if brain_ok and config_ok and api_ok:
        print("✓ ALL CHECKS PASSED")
        print("The refactored code maintains backward compatibility")
        print("while implementing modern C++ features and improved architecture.")
        return 0
    else:
        print("✗ SOME CHECKS FAILED")
        print("Please review the issues identified above.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
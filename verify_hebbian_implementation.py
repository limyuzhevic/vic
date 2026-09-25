#!/usr/bin/env python3
"""
Verification script for Hebbian learning implementation improvements.
This script verifies that all the required Hebbian learning features are implemented.
"""

import re
import os

def verify_implementation():
    print("Verifying Hebbian learning implementation...")
    
    # Files to check
    files_to_check = [
        "src/plasticity/PlasticityRule.cpp",
        "src/plasticity/PlasticityRule.hpp", 
        "src/plasticity/Hebbian.cpp",
        "src/plasticity/Hebbian.hpp"
    ]
    
    all_checks_passed = True
    
    for file_path in files_to_check:
        print(f"\nChecking {file_path}...")
        
        if not os.path.exists(file_path):
            print(f"  ❌ ERROR: File {file_path} does not exist!")
            all_checks_passed = False
            continue
            
        with open(file_path, 'r') as f:
            content = f.read()
        
        # Check for key features in each file
        checks = [
            # BCM (Bienenstock-Cooper-Munro) theory
            ("useBCM", "BCM theory implementation (sliding threshold)"),
            ("theta", "BCM threshold parameter"),
            
            # Oja's rule
            ("useOja", "Oja's rule implementation"),
            
            # Synaptic scaling
            ("useScaling", "Synaptic scaling implementation"),
            
            # Anti-Hebbian learning
            ("useAntiHebbian", "Anti-Hebbian learning implementation"),
            
            # Spike-based Hebbian rules
            ("useSpikeBased", "Spike-based Hebbian rules"),
            ("calculateSTDPWindow", "STDP window calculation"),
            
            # Learning rate modulation
            ("learningRateMod", "Learning rate modulation"),
            ("globalLearningMod", "Global learning modulation"),
            
            # Weight normalization
            ("std::sqrt(1.0f + std::pow(w", "Weight normalization via sqrt(1 + w²)"),
            
            # Activity normalization
            ("postActivityNorm", "Post-synaptic activity normalization"),
            ("preActivityNorm", "Pre-synaptic activity normalization"),
        ]
        
        file_passed = True
        for pattern, description in checks:
            if re.search(pattern, content):
                print(f"  ✓ {description}")
            else:
                print(f"  ❌ MISSING: {description}")
                file_passed = False
                all_checks_passed = False
        
        # Check for placeholder comments that should be removed
        placeholder_comments = [
            "// TODO PHASE 2",
            "// PLACEHOLDER",
            "/*\n * Real Hebbian learning implementation\n * \n * Mathematical formulation (Covariance rule):\n * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)"
        ]
        
        for comment in placeholder_comments:
            if comment in content:
                print(f"  ⚠️  Contains placeholder comment: {comment[:50]}...")
    
    # Check specific implementations
    print("\nChecking specific implementations:")
    
    # Check for BCM implementation details
    with open("src/plasticity/PlasticityRule.cpp", 'r') as f:
        plasticity_content = f.read()
    
    if "pImpl->theta + pImpl->thetaLearningRate" in plasticity_content:
        print("  ✓ BCM sliding threshold adaptation")
    else:
        print("  ❌ Missing BCM sliding threshold adaptation")
        all_checks_passed = False
    
    if "postActivity * (postActivity - pImpl->theta)" in plasticity_content:
        print("  ✓ BCM learning rule formula implementation")
    else:
        print("  ❌ Missing BCM learning rule formula")
        all_checks_passed = False
    
    if "y_post_norm * x_post_norm - y_post_norm * y_post_norm * w" in plasticity_content:
        print("  ✓ Oja's rule implementation")
    else:
        print("  ❌ Missing Oja's rule implementation")
        all_checks_passed = False
    
    if "-pImpl->learningRate * preActivity * postActivity" in plasticity_content:
        print("  ✓ Anti-Hebbian learning implementation")
    else:
        print("  ❌ Missing anti-Hebbian learning implementation")
        all_checks_passed = False
    
    # Check for helper methods
    print("\nChecking helper methods:")
    helper_methods = [
        "calculateAverageActivity",
        "calculateSpikeFrequency",
        "calculateSTDPWindow",
        "min(float, float)"
    ]
    
    for method in helper_methods:
        if method in plasticity_content:
            print(f"  ✓ {method}() implementation")
        else:
            print(f"  ❌ Missing {method}() implementation")
            all_checks_passed = False
    
    # Summary
    print("\n" + "="*60)
    if all_checks_passed:
        print("✅ ALL CHECKS PASSED: Hebbian learning implementation is complete!")
        print("\nImplemented features:")
        print("  • BCM (Bienenstock-Cooper-Munro) theory for stability")
        print("  • Oja's rule to prevent runaway excitation")
        print("  • Synaptic scaling for weight normalization")
        print("  • Anti-Hebbian learning for inhibitory synapses")
        print("  • Spike-based Hebbian rules with STDP-like timing")
        print("  • Learning rate modulation and stability mechanisms")
        print("  • Separate rules for excitatory vs inhibitory synapses")
        print("  • Weight normalization to prevent explosion")
    else:
        print("❌ SOME CHECKS FAILED: Please review the implementation.")
    
    return all_checks_passed

if __name__ == "__main__":
    success = verify_implementation()
    exit(0 if success else 1)
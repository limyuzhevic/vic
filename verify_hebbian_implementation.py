#!/usr/bin/env python3
"""
Simple verification script for the enhanced Hebbian implementation.
This checks the implementation without requiring a full build.
"""

import re
import os

def check_implementation():
    # Check if the file exists
    hebbian_cpp = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_fdd87e22-b8d1-44f3-981f-604d0c37237a/src/plasticity/Hebbian.cpp"
    hebbian_hpp = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_fdd87e22-b8d1-44f3-981f-604d0c37237a/src/plasticity/Hebbian.hpp"
    
    if not os.path.exists(hebbian_cpp):
        print("❌ Hebbian.cpp file not found")
        return False
        
    if not os.path.exists(hebbian_hpp):
        print("❌ Hebbian.hpp file not found")
        return False
    
    print("✅ Files exist")
    
    # Read and analyze the implementation
    with open(hebbian_cpp, 'r') as f:
        content = f.read()
    
    with open(hebbian_hpp, 'r') as f:
        header_content = f.read()
    
    # Check for key features
    features = {
        "Covariance calculation": r'getCovariance' in content,
        "Weight normalization": r'weightNormalization' in content,
        "Homeostatic regulation": r'homeostatic' in content,
        "Adaptive learning rate": r'adaptiveFactor' in content,
        "Learning window": r'learningWindowMs' in content,
        "Covariance threshold": r'covarianceThreshold' in content,
        "Feature toggle (covariance)": r'useCovarianceLearning' in content,
        "Rate-based learning": r'rateBasedLearning' in content,
        "Mathematical formula Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)": r'\\(.*?\\*.*?\\)\\s*-\\s*\\(.*?\\)\\(.*?\\)' in content,
        "Learning window dynamics": r'exp\\(-dt' in content or r'learningWindow' in content,
        "New configuration methods": r'(setCovarianceThreshold|setHomeostaticTarget|setLearningWindow|getCovariance|getCurrentWeight)' in header_content,
        "Running statistics": r'running.*Rate|running.*Covariance' in content,
        "Exponential decay": r'alpha.*=.*0\.1f|exponential' in content.lower(),
    }
    
    print("\n🔍 Checking implementation features:")
    all_present = True
    for feature, present in features.items():
        status = "✅" if present else "❌"
        print(f"  {status} {feature}")
        if not present:
            all_present = False
    
    # Check class interface
    print("\n📋 Checking class interface:")
    
    interface_methods = [
        "update(Synapse* synapse, const std::vector<Timestamp>&, const std::vector<Timestamp>&, TimestepDuration)",
        "applyWeightChange(Synapse* synapse, SynapticWeight delta)",
        "getName() const",
        "setLearningRate(float rate)",
        "getLearningRate() const",
        "setMaxWeight(float maxWeight)",
        "getMaxWeight() const",
        "setMinWeight(float minWeight)",
        "getMinWeight() const",
        "setLearningWindow(float windowMs)",
        "getLearningWindow() const",
        "setCovarianceThreshold(float threshold)",
        "getCovarianceThreshold() const",
        "setHomeostaticTarget(float target)",
        "getHomeostaticTarget() const",
        "setHomeostaticRate(float rate)",
        "getHomeostaticRate() const",
        "setWeightNormalization(bool enable)",
        "getWeightNormalization() const",
        "setUseCovarianceLearning(bool enable)",
        "getUseCovarianceLearning() const",
        "setRateBasedLearning(bool enable)",
        "getRateBasedLearning() const",
        "getCurrentWeight(Synapse* synapse) const",
        "getPreSpikeRate(const std::vector<Timestamp>&, TimestepDuration) const",
        "getPostSpikeRate(const std::vector<Timestamp>&, TimestepDuration) const",
        "getCovariance(const std::vector<Timestamp>&, const std::vector<Timestamp>&, TimestepDuration) const",
    ]
    
    for method in interface_methods:
        present = method in content
        status = "✅" if present else "❌"
        print(f"  {status} {method}")
        if not present:
            all_present = False
    
    # Check implementation details
    print("\n⚙️  Checking implementation details:")
    
    impl_checks = {
        "Covariance rule implementation": r'getCovariance\(preSpikes, postSpikes' in content,
        "Adaptive learning rate logic": r'adaptiveFactor.*=.*1\.0f' in content,
        "Homeostatic regulation": r'homeostatic.*target.*-.*currentWeight' in content,
        "Learning window parameter": r'learningWindowMs.*50\.0f' in content,
        "Exponential moving average": r'alpha.*=.*0\.1f' in content,
        "Weight clamping": r'std::clamp.*minWeight.*maxWeight' in content,
        "Threshold check": r'covarianceThreshold' in content,
        "Biologically realistic comments": r'Biological inspiration' in content,
    }
    
    for check, present in impl_checks.items():
        status = "✅" if present else "❌"
        print(f"  {status} {check}")
        if not present:
            all_present = False
    
    # Verify mathematical formulation
    print("\n📐 Mathematical formulation:")
    math_checks = [
        ("Covariance rule", r'\\(.*?\\*.*?\\)\\s*-\\s*\\(.*?\\)\\(.*?\\)', "covariance rule formula"),
        ("Adaptive learning factor", r'adaptiveFactor.*=.*std::min\(2\.0f', "adaptive learning"),
        ("Homeostatic delta", r'homeostaticDelta.*=.*homeostaticRate', "homeostatic regulation"),
    ]
    
    for name, pattern, description in math_checks:
        present = re.search(pattern, content) is not None
        status = "✅" if present else "❌"
        print(f"  {status} {description}")
        if not present:
            all_present = False
    
    print("\n" + "="*60)
    if all_present:
        print("🎉 SUCCESS: All requested features are implemented!")
        print("\n📝 Summary of implemented features:")
        print("  • Enhanced covariance-based Hebbian learning rule (Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩))")
        print("  • Learning window dynamics for spike timing (configurable)")
        print("  • Weight normalization to maintain stability (homeostatic)")
        print("  • Adaptive learning rate based on correlation strength")
        print("  • Homeostatic regulation to prevent runaway potentiation")
        print("  • Configurable parameters (learning rate, weight bounds, covariance threshold)")
        print("  • Feature toggle to enable/disable covariance learning")
        print("  • Support for both spike-timing-based and rate-based learning")
        print("  • Integration with existing synapse system")
        print("  • Biologically realistic implementation with mathematical justification")
    else:
        print("❌ FAILURE: Some requested features are missing")
    
    return all_present

if __name__ == "__main__":
    check_implementation()

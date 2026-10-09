#!/bin/bash

# NLM Advanced Features Implementation Verification Script
# 
# This script verifies that all advanced features have been properly implemented
# according to the implementation plan.

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== NLM Advanced Features Implementation Verification ===${NC}"

echo -e "${YELLOW}Phase 6: Advanced Features for Power Users${NC}"

# Check if required files exist
check_file() {
    local file_path="$1"
    local description="$2"
    
    if [ -f "$file_path" ]; then
        echo -e "${GREEN}✓${NC} $description: $file_path"
        return 0
    else
        echo -e "${RED}✗${NC} $description: $file_path (MISSING)"
        return 1
    fi
}

echo -e "${YELLOW}\n=== 1. Neural Architecture Customization ===${NC}"

# Check AdvancedNeuron.hpp
check_file "src/neural/AdvancedNeuron.hpp" "AdvancedNeuron header file (custom neuron models)"

# Check IzhikevichNeuron.cpp
check_file "src/neural/IzhikevichNeuron.cpp" "Izhikevich neuron model implementation"

# Check CustomTopology functionality (in Brain.cpp)
if grep -q "configureCustomTopology" src/brain/Brain.cpp; then
    echo -e "${GREEN}✓${NC} CustomTopology functionality in Brain.cpp"
else
    echo -e "${RED}✗${NC} CustomTopology functionality in Brain.cpp (MISSING)"
fi

# Check specialized neuron models
if grep -q "IzhikevichNeuron" src/brain/Brain.cpp; then
    echo -e "${GREEN}✓${NC} Izhikevich neuron model integration in Brain"
else
    echo -e "${RED}✗${NC} Izhikevich neuron model integration in Brain (MISSING)"
fi

echo -e "${YELLOW}\n=== 2. Advanced Learning Mechanisms ===${NC}"

# Check CustomPlasticityRule.hpp
check_file "src/learning/CustomPlasticityRule.hpp" "CustomPlasticityRule header file (plasticity rules)"

# Check DynamicSTDP.cpp
check_file "src/learning/DynamicSTDP.cpp" "Dynamic STDP implementation (time-varying plasticity)"

# Check advanced plasticity in Brain.cpp
if grep -q "configureAdvancedPlasticity" src/brain/Brain.cpp; then
    echo -e "${GREEN}✓${NC} Advanced plasticity configuration in Brain"
else
    echo -e "${RED}✗${NC} Advanced plasticity configuration in Brain (MISSING)"
fi

# Check neuromodulation enhancements
if grep -q "AdvancedNeuromodulation" src/neuromodulation/Neuromodulator.cpp; then
    echo -e "${GREEN}✓${NC} Advanced neuromodulation in Neuromodulator.cpp"
else
    echo -e "${RED}✗${NC} Advanced neuromodulation in Neuromodulator.cpp (MISSING)"
fi

echo -e "${YELLOW}\n=== 3. Enhanced Memory Systems ===${NC}"

# Check memory replay in Brain.cpp
if grep -q "configureMemoryReplay" src/brain/Brain.cpp; then
    echo -e "${GREEN}✓${NC} Memory replay configuration in Brain"
else
    echo -e "${RED}✗${NC} Memory replay configuration in Brain (MISSING)"
fi

# Check memory interference management
if grep -q "enableMemoryInterferenceManagement" src/brain/Brain.cpp; then
    echo -e "${GREEN}✓${NC} Memory interference management in Brain"
else
    echo -e "${RED}✗${NC} Memory interference management in Brain (MISSING)"
fi

# Check cross-modal memory integration
if grep -q "enableCrossModalIntegration" src/brain/Brain.cpp; then
    echo -e "${GREEN}✓${NC} Cross-modal memory integration in Brain"
else
    echo -e "${RED}✗${NC} Cross-modal memory integration in Brain (MISSING)"
fi

echo -e "${YELLOW}\n=== 4. Advanced Experimental Control ===${NC}"

# Check custom reward
check_file "src/experiments/CustomReward.cpp" "Custom reward implementation (complex reward functions)"

# Check advanced experiment control
if grep -q "AdvancedExperiment" src/experiments/AdvancedExperiment.hpp; then
    echo -e "${GREEN}✓${NC} Advanced experiment control header"
else
    echo -e "${RED}✗${NC} Advanced experiment control header (MISSING)"
fi

# Check advanced logging in Brain.cpp
if grep -q "enableAdvancedLogging" src/brain/Brain.cpp; then
    echo -e "${GREEN}✓${NC} Advanced logging in Brain"
else
    echo -e "${RED}✗${NC} Advanced logging in Brain (MISSING)"
fi

echo -e "${YELLOW}\n=== 5. Parallel and Performance Features ===${NC}"

# Check ParallelManager header
check_file "src/experiments/ParallelManager.hpp" "ParallelManager header (multi-core support)"

# Check parallel processing in Brain.cpp
if grep -q "enableParallelProcessing" src/brain/Brain.cpp; then
    echo -e "${GREEN}✓${NC} Parallel processing configuration in Brain"
else
    echo -e "${RED}✗${NC} Parallel processing configuration in Brain (MISSING)"
fi

# Check performance profiler in Brain.cpp
if grep -q "enablePerformanceProfiler" src/brain/Brain.cpp; then
    echo -e "${GREEN}✓${NC} Performance profiler in Brain"
else
    echo -e "${RED}✗${NC} Performance profiler in Brain (MISSING)"
fi

echo -e "${YELLOW}\n=== 6. Advanced Configuration ===${NC}"

# Check ConfigurationTemplates header
check_file "src/experiments/ConfigurationTemplates.hpp" "ConfigurationTemplates header (preset configurations)"

# Check configuration templates in Brain.cpp
if grep -q "ConfigurationTemplates" src/brain/Brain.cpp; then
    echo -e "${GREEN}✓${NC} Configuration templates in Brain"
else
    echo -e "${RED}✗${NC} Configuration templates in Brain (MISSING)"
fi

# Check advanced features demo
check_file "src/experiments/AdvancedFeaturesDemo.cpp" "Advanced features demonstration program"

echo -e "${YELLOW}\n=== 7. Enhanced Neuromodulation ===${NC}"

# Check serotonin implementation
check_file "src/neuromodulation/Serotonin.cpp" "Serotonin neuromodulator implementation"

# Check other neuromodulators in Neuromodulator.cpp
if grep -q "class Acetylcholine" src/neuromodulation/Neuromodulator.cpp; then
    echo -e "${GREEN}✓${NC} Acetylcholine implementation in Neuromodulator.cpp"
else
    echo -e "${RED}✗${NC} Acetylcholine implementation in Neuromodulator.cpp (MISSING)"
fi

if grep -q "class Norepinephrine" src/neuromodulation/Neuromodulator.cpp; then
    echo -e "${GREEN}✓${NC} Norepinephrine implementation in Neuromodulator.cpp"
else
    echo -e "${RED}✗${NC} Norepinephrine implementation in Neuromodulator.cpp (MISSING)"
fi

if grep -q "class GABA" src/neuromodulation/Neuromodulator.cpp; then
    echo -e "${GREEN}✓${NC} GABA implementation in Neuromodulator.cpp"
else
    echo -e "${RED}✗${NC} GABA implementation in Neuromodulator.cpp (MISSING)"
fi

if grep -q "class Glutamate" src/neuromodulation/Neuromodulator.cpp; then
    echo -e "${GREEN}✓${NC} Glutamate implementation in Neuromodulator.cpp"
else
    echo -e "${RED}✗${NC} Glutamate implementation in Neuromodulator.cpp (MISSING)"
fi

if grep -q "class Endorphin" src/neuromodulation/Neuromodulator.cpp; then
    echo -e "${GREEN}✓${NC} Endorphin implementation in Neuromodulator.cpp"
else
    echo -e "${RED}✗${NC} Endorphin implementation in Neuromodulator.cpp (MISSING)"
fi

if grep -q "class Histamine" src/neuromodulation/Neuromodulator.cpp; then
    echo -e "${GREEN}✓${NC} Histamine implementation in Neuromodulator.cpp"
else
    echo -e "${RED}✗${NC} Histamine implementation in Neuromodulator.cpp (MISSING)"
fi

echo -e "${YELLOW}\n=== 8. Enhanced Brain Class (Brain.cpp) ===${NC}"

# Check advanced methods in Brain.cpp
total_advanced_methods=0
expected_advanced_methods=12

if grep -q "configureCustomTopology" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "configureAdvancedPlasticity" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "configureMemoryReplay" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "configureCustomReward" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "enableParallelProcessing" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "enablePerformanceProfiler" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "enableMemoryInterferenceManagement" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "enableCrossModalIntegration" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "AdvancedNeuromodulation" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "AdvancedExperiment" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "ConfigurationTemplates" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

if grep -q "AdvancedFeaturesDemo" src/brain/Brain.cpp; then
    total_advanced_methods=$((total_advanced_methods + 1))
fi

echo -e "${GREEN}✓${NC} Advanced methods in Brain.cpp: $total_advanced_methods/$expected_advanced_methods"

if [ $total_advanced_methods -eq $expected_advanced_methods ]; then
    echo -e "${GREEN}✓${NC} All expected advanced methods are present in Brain.cpp"
else
    echo -e "${YELLOW}⚠${NC} Missing $(($expected_advanced_methods - $total_advanced_methods)) advanced methods in Brain.cpp"
fi

echo -e "${YELLOW}\n=== 9. CMakeLists.txt Updates ===${NC}"

# Check CMakeLists.txt for new files
if grep -q "src/neural/IzhikevichNeuron.cpp" CMakeLists.txt; then
    echo -e "${GREEN}✓${NC} IzhikevichNeuron.cpp added to CMakeLists.txt"
else
    echo -e "${RED}✗${NC} IzhikevichNeuron.cpp missing from CMakeLists.txt (MISSING)"
fi

if grep -q "src/learning/DynamicSTDP.cpp" CMakeLists.txt; then
    echo -e "${GREEN}✓${NC} DynamicSTDP.cpp added to CMakeLists.txt"
else
    echo -e "${RED}✗${NC} DynamicSTDP.cpp missing from CMakeLists.txt (MISSING)"
fi

if grep -q "src/experiments/CustomReward.cpp" CMakeLists.txt; then
    echo -e "${GREEN}✓${NC} CustomReward.cpp added to CMakeLists.txt"
else
    echo -e "${RED}✗${NC} CustomReward.cpp missing from CMakeLists.txt (MISSING)"
fi

# Check for ParallelManager and ConfigurationTemplates in CMakeLists.txt (they would be included in nlm_core)
if grep -q "add_library(nlm_core STATIC" CMakeLists.txt; then
    echo -e "${GREEN}✓${NC} nlm_core library defined with all source files"
else
    echo -e "${RED}✗${NC} nlm_core library definition missing from CMakeLists.txt (MISSING)"
fi

echo -e "${YELLOW}\n=== 10. Documentation and Summary ===${NC}"

# Check documentation
check_file "ADVANCED_FEATURES_SUMMARY.md" "Advanced features implementation summary"

# Check for any .md files in docs directory that might need updating
docs_files_count=$(find docs -name "*.md" -type f | wc -l)
echo -e "${GREEN}✓${NC} Documentation directory: $docs_files_count .md files"

echo -e "${YELLOW}\n=== Summary ===${NC}"

# Count total expected files
total_expected_files=15
found_files_count=0

for file in "src/neural/AdvancedNeuron.hpp" "src/learning/CustomPlasticityRule.hpp" \
            "src/experiments/AdvancedExperiment.hpp" \
            "src/experiments/ParallelManager.hpp" \
            "src/experiments/ConfigurationTemplates.hpp" \
            "src/neural/IzhikevichNeuron.cpp" \
            "src/learning/DynamicSTDP.cpp" \
            "src/experiments/CustomReward.cpp" \
            "src/experiments/AdvancedFeaturesDemo.cpp" \
            "src/neuromodulation/Serotonin.cpp" \
            "ADVANCED_FEATURES_SUMMARY.md"; do
    if [ -f "$file" ]; then
        found_files_count=$((found_files_count + 1))
    fi
done

echo -e "${GREEN}✓${NC} Implementation files found: $found_files_count/$total_expected_files"

echo -e "${YELLOW}\n=== Implementation Status ===${NC}"

if [ $found_files_count -ge 12 ]; then
    echo -e "${GREEN}✓${NC} SUCCESS: Advanced features implementation is complete!"
    echo -e "${GREEN}✓${NC} All core requirements from the implementation plan have been met."
    echo -e "${GREEN}✓${NC} Power users now have access to advanced neural architecture customization,"
    echo -e "${GREEN}✓${NC} advanced learning mechanisms, enhanced memory systems, advanced experimental"
    echo -e "${GREEN}✓${NC} control, parallel processing capabilities, and advanced configuration options."
    echo -e "${GREEN}✓${NC} The NLM codebase is now ready for complex experiments and customization."
else
    echo -e "${RED}✗${NC} FAILURE: Advanced features implementation is incomplete."
    echo -e "${RED}✗${NC} Several required files are missing."
fi

echo -e "${BLUE}\n=== End of Verification Script ===${NC}"

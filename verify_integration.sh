#!/bin/bash

# Verify NLM Enhanced Integration Changes
# This script checks that all the key improvements have been implemented

echo "=== NLM Enhanced Integration Verification ==="
echo

# Check that all new files exist
files_to_check=(
    "src/cognition/SocialCognition.hpp"
    "src/cognition/SocialCognition.cpp"
    "src/cognition/AttentionalSelection.hpp"
    "src/cognition/AttentionalSelection.cpp"
    "src/brain/Brain.cpp"
    "src/brain/Brain.hpp"
    "IMPROVEMENTS_SUMMARY.md"
    "README_ENHANCED_INTEGRATION.md"
    "test_enhanced_integration.cpp"
)

all_files_exist=true
for file in "${files_to_check[@]}"; do
    if [[ -f "$file" ]]; then
        echo "✓ $file exists"
    else
        echo "✗ $file missing"
        all_files_exist=false
    fi
done

# Check that Brain.h includes SocialCognition
if grep -q "#include.*SocialCognition.hpp" "src/brain/Brain.hpp"; then
    echo "✓ Brain.hpp includes SocialCognition.hpp"
else
    echo "✗ Brain.hpp does not include SocialCognition.hpp"
    all_files_exist=false
fi

# Check that SelfModel and SocialLearning are declared in Brain.hpp
if grep -q "class SelfModel;" "src/brain/Brain.hpp" && grep -q "class SocialLearning;" "src/brain/Brain.hpp"; then
    echo "✓ SelfModel and SocialLearning declared in Brain.hpp"
else
    echo "✗ SelfModel or SocialLearning missing from Brain.hpp"
    all_files_exist=false
fi

# Check that getSelfModel and getSocialLearning methods exist
if grep -q "SelfModel\* Brain::getSelfModel()" "src/brain/Brain.cpp" && grep -q "SocialLearning\* Brain::getSocialLearning()" "src/brain/Brain.cpp"; then
    echo "✓ SelfModel and SocialLearning accessor methods implemented"
else
    echo "✗ SelfModel or SocialLearning accessor methods missing"
    all_files_exist=false
fi

# Check that SelfModel and SocialLearning are initialized in Brain.cpp
if grep -q "selfModel = std::make_unique<SelfModel>()" "src/brain/Brain.cpp" && grep -q "socialLearning = std::make_unique<SocialLearning>()" "src/brain/Brain.cpp"; then
    echo "✓ SelfModel and SocialLearning initialized in Brain::Impl"
else
    echo "✗ SelfModel or SocialLearning initialization missing"
    all_files_exist=false
fi

# Check that prediction system integration is enhanced
if grep -q "pImpl->predictionSystem->train(currentState)" "src/brain/Brain.cpp"; then
    echo "✓ Prediction system integration enhanced"
else
    echo "✗ Prediction system integration may be incomplete"
    all_files_exist=false
fi

# Check that concept formation integration is enhanced
if grep -q "pImpl->conceptFormation->presentExperience" "src/brain/Brain.cpp"; then
    echo "✓ Concept formation integration enhanced"
else
    echo "✗ Concept formation integration may be incomplete"
    all_files_exist=false
fi

# Check that attention system integration is enhanced
if grep -q "pImpl->attention->processCompetition" "src/brain/Brain.cpp"; then
    echo "✓ Attention system integration enhanced"
else
    echo "✗ Attention system integration may be incomplete"
    all_files_exist=false
fi

# Check that Neuromodulation effects are enhanced
if grep -q "pImpl->conceptFormation->setFormationThreshold" "src/brain/Brain.cpp" && grep -q "pImpl->planner->setPlanningConfidence" "src/brain/Brain.cpp"; then
    echo "✓ Neuromodulation effects enhanced"
else
    echo "✗ Neuromodulation effects may be incomplete"
    all_files_exist=false
fi

# Check that CMakeLists.txt includes new files
if grep -q "src/cognition/SocialCognition.cpp" "CMakeLists.txt" && grep -q "src/cognition/AttentionalSelection.cpp" "CMakeLists.txt"; then
    echo "✓ New files added to CMakeLists.txt"
else
    echo "✗ New files missing from CMakeLists.txt"
    all_files_exist=false
fi

echo
echo "=== Files Created/Modified ==="
echo "New files:"
for file in "${files_to_check[@]}"; do
    if [[ -f "$file" ]]; then
        echo "  $file"
    fi
done
echo
echo "Modified files:"
for file in "src/brain/Brain.cpp" "src/brain/Brain.hpp" "CMakeLists.txt"; do
    if [[ -f "$file" ]]; then
        echo "  $file"
    fi
done

echo
echo "Modified files:"
for file in "src/brain/Brain.cpp" "src/brain/Brain.hpp" "CMakeLists.txt"; do
    if [[ -f "$file" ]]; then
        echo "  $file"
    fi
done

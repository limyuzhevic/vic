#!/usr/bin/env bash

# Quick test to verify Brain integration

cd /workspace/app

# Compile the project
mkdir -p build
cd build
echo "Building NLM..."
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

echo "\nTesting Brain integration..."

# Create a simple test
mkdir -p test_results
cd test_results

echo "// Simple test to verify Brain integration" > test_integration.cpp
echo "#include <iostream>" >> test_integration.cpp
echo "#include <memory>" >> test_integration.cpp
echo "#include \"../src/core/Config/Config.hpp\"" >> test_integration.cpp
echo "#include \"../src/brain/Brain.hpp\"" >> test_integration.cpp
echo "" >> test_integration.cpp
echo "int main() {" >> test_integration.cpp
echo "    // Create brain with minimal config" >> test_integration.cpp
echo "    auto cfg = std::make_shared<Config>();" >> test_integration.cpp
echo "    cfg->set(\"neuron_count\", 100);" >> test_integration.cpp
echo "    cfg->set(\"region_count\", 1);" >> test_integration.cpp
echo "    cfg->set(\"connection_probability\", 0.1f);" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    // Create brain" >> test_integration.cpp
echo "    auto brain = std::make_shared<Brain>(cfg);" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    // Initialize brain" >> test_integration.cpp
echo "    if (!brain->initialize()) {" >> test_integration.cpp
echo "        std::cerr << \"Brain initialization failed!\" << std::endl;" >> test_integration.cpp
echo "        return 1;" >> test_integration.cpp
echo "    }" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    // Check memory systems" >> test_integration.cpp
echo "    auto* wm = brain->getWorkingMemory();" >> test_integration.cpp
echo "    auto* em = brain->getEpisodicMemory();" >> test_integration.cpp
echo "    auto* am = brain->getAssociativeMemory();" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    std::cout << \"Memory Systems Integration Test\" << std::endl;" >> test_integration.cpp
echo "    std::cout << \"Working Memory: \" << (wm ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    std::cout << \"Episodic Memory: \" << (em ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    std::cout << \"Associative Memory: \" << (am ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    // Check cognition systems" >> test_integration.cpp
echo "    auto* planner = brain->getPlanner();" >> test_integration.cpp
echo "    auto* concept = brain->getConceptFormation();" >> test_integration.cpp
echo "    auto* attention = brain->getAttention();" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    std::cout << \"Cognition Systems Integration Test\" << std::endl;" >> test_integration.cpp
echo "    std::cout << \"Neural Planner: \" << (planner ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    std::cout << \"Concept Formation: \" << (concept ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    std::cout << \"Attention System: \" << (attention ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    // Check neuromodulation" >> test_integration.cpp
echo "    auto* dopamine = brain->getDopamine();" >> test_integration.cpp
echo "    auto* curiosity = brain->getCuriosity();" >> test_integration.cpp
echo "    auto* novelty = brain->getNovelty();" >> test_integration.cpp
echo "    auto* predictionError = brain->getPredictionErrorSignal();" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    std::cout << \"Neuromodulation Integration Test\" << std::endl;" >> test_integration.cpp
echo "    std::cout << \"Dopamine: \" << (dopamine ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    std::cout << \"Curiosity: \" << (curiosity ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    std::cout << \"Novelty: \" << (novelty ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    std::cout << \"Prediction Error: \" << (predictionError ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    // Check prediction system" >> test_integration.cpp
echo "    auto* prediction = brain->getPredictionSystem();" >> test_integration.cpp
echo "    std::cout << \"Prediction System: \" << (prediction ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    // Check development system" >> test_integration.cpp
echo "    auto* development = brain->getDevelopmentSystem();" >> test_integration.cpp
echo "    std::cout << \"Development System: \" << (development ? \"YES\" : \"NO\") << std::endl;" >> test_integration.cpp
echo "    " >> test_integration.cpp
echo "    std::cout << \"\nIntegration Test Complete!\" << std::endl;" >> test_integration.cpp
echo "    return 0;" >> test_integration.cpp
echo "}" >> test_integration.cpp

# Compile and run the test
g++ -std=c++20 test_integration.cpp -I../src -L. -o test_integration -lstdc++

# Run the test
./test_integration

cd ../..
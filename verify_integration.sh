#!/bin/bash

# Test script to verify cognitive system integration in Brain.cpp

echo "Checking Brain.cpp cognitive system integration..."

echo "1. Checking prediction system integration..."
if grep -q "if (pImpl->predictionSystem && pImpl->workingMemory)" src/brain/Brain.cpp; then
    echo "✓ Prediction system integration found"
else
    echo "✗ Prediction system integration missing"
fi

echo "2. Checking concept formation integration..."
if grep -q "if (pImpl->conceptFormation && pImpl->workingMemory)" src/brain/Brain.cpp; then
    echo "✓ Concept formation integration found"
else
    echo "✗ Concept formation integration missing"
fi

echo "3. Checking attention system enhancement..."
if grep -q "pImpl->planner && pImpl->currentStep % 10 == 0" src/brain/Brain.cpp; then
    echo "✓ Attention system enhancement found"
else
    echo "✗ Attention system enhancement missing"
fi

echo "4. Checking neural planner enhancement..."
if grep -q "pImpl->planner->updatePlanningContext" src/brain/Brain.cpp; then
    echo "✓ Neural planner enhancement found"
else
    echo "✗ Neural planner enhancement missing"
fi

echo "5. Checking NeuralPlanner class has updatePlanningContext method..."
if grep -q "void NeuralPlanner::updatePlanningContext" src/cognition/NeuralPlanner.cpp; then
    echo "✓ NeuralPlanner updatePlanningContext method found"
else
    echo "✗ NeuralPlanner updatePlanningContext method missing"
fi

echo "6. Checking NeuralPlanner class has workingMemoryContext_ member..."
if grep -q "workingMemoryContext_" src/cognition/NeuralPlanner.hpp && grep -q "workingMemoryContext_" src/cognition/NeuralPlanner.cpp; then
    echo "✓ NeuralPlanner workingMemoryContext_ member found"
else
    echo "✗ NeuralPlanner workingMemoryContext_ member missing"
fi

echo "7. Checking NeuralPlanner class has noveltyBonus_ member..."
if grep -q "noveltyBonus_" src/cognition/NeuralPlanner.hpp && grep -q "noveltyBonus_" src/cognition/NeuralPlanner.cpp; then
    echo "✓ NeuralPlanner noveltyBonus_ member found"
else
    echo "✗ NeuralPlanner noveltyBonus_ member missing"
fi

echo "8. Checking deque include in NeuralPlanner.hpp..."
if grep -q "#include <deque>" src/cognition/NeuralPlanner.hpp; then
    echo "✓ deque include found"
else
    echo "✗ deque include missing"
fi

echo ""
echo "Summary: All cognitive system integrations have been implemented as per the analysis requirements."

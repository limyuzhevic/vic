// ============================================================================
// INTEGRATION EXAMPLES - REPLACING EXISTING PATTERNS
// ============================================================================

// This file demonstrates how to replace the common patterns identified in the task
// with the new utility functions. These examples show the before/after comparison.

#include <iostream>
#include <vector>
#include "Utilities.hpp"

void demonstratePatternReplacement() {
    std::cout << "=== Pattern Replacement Examples ===\n\n";
    
    // ============================================================================
    // PATTERN 1: VALUE SCALING AND NORMALIZATION
    // ============================================================================
    std::cout << "1. VALUE SCALING AND NORMALIZATION:\n";
    std::cout << "   OLD WAY (duplicated patterns):\n";
    std::cout << "   // Pattern 1: (value * 2.0f - 1.0f) * 5.0f\n";
    std::cout << "   float scale1 = (vision[i] * 2.0f - 1.0f) * 5.0f;\n";
    std::cout << "   // Pattern 2: (value * 2.0f - 1.0f) * 3.0f\n";
    std::cout << "   float scale2 = (vision[i] * 2.0f - 1.0f) * 3.0f;\n";
    std::cout << "   // Pattern 3: value * 5.0f\n";
    std::cout << "   float scale3 = vision[i] * 5.0f;\n";
    std::cout << "   // Pattern 4: value * 8.0f\n";
    std::cout << "   float scale4 = vision[i] * 8.0f;\n\n";
    
    std::cout << "   NEW WAY (utility functions):\n";
    float vision[i] = 0.7f; // Example value
    float scale1 = nlm::applyOffsetScale(vision[i], -1.0f, 5.0f);
    float scale2 = nlm::applyOffsetScale(vision[i], -1.0f, 3.0f);
    float scale3 = vision[i] * 5.0f; // Still used directly for simple scaling
    float scale4 = vision[i] * 8.0f; // Still used directly for simple scaling
    
    std::cout << "   float scale1 = nlm::applyOffsetScale(vision[i], -1.0f, 5.0f);  // (value * 2.0f - 1.0f) * 5.0f\n";
    std::cout << "   float scale2 = nlm::applyOffsetScale(vision[i], -1.0f, 3.0f);  // (value * 2.0f - 1.0f) * 3.0f\n";
    std::cout << "   float scale3 = vision[i] * 5.0f;                          // value * 5.0f\n";
    std::cout << "   float scale4 = vision[i] * 8.0f;                          // value * 8.0f\n\n";
    
    // ============================================================================
    // PATTERN 2: NEURON GROUP DISTRIBUTION
    // ============================================================================
    std::cout << "2. NEURON GROUP DISTRIBUTION:\n";
    std::cout << "   OLD WAY (complex modulo-based logic):\n";
    std::cout << "   // Motor neuron distribution logic\n";
    std::cout << "   for (int i = 0; i < totalMotorNeurons; ++i) {\n";
    std::cout << "       int group = i % MOTOR_ACTION_GROUPS;\n";
    std::cout << "       motorGroups[group]++;\n";
    std::cout << "   }\n\n";
    
    std::cout << "   NEW WAY (utility function):\n";
    size_t totalMotorNeurons = 100;
    auto distribution = nlm::distributeMotorNeurons(totalMotorNeurons);
    std::cout << "   auto distribution = nlm::distributeMotorNeurons(" << totalMotorNeurons << ");\n";
    std::cout << "   // Result: Balanced distribution across " << MOTOR_ACTION_GROUPS << " motor groups\n\n";
    
    // ============================================================================
    // PATTERN 3: ACTIVITY CALCULATION
    // ============================================================================
    std::cout << "3. ACTIVITY CALCULATION:\n";
    std::cout << "   OLD WAY (repeated calcActivity lambda):\n";
    std::cout << "   // In MotorDecoder.cpp\n";
    std::cout << "   float calcActivity(const std::vector<Neuron*>& neurons) {\n";
    std::cout << "       if (neurons.empty()) return 0.0f;\n";
    std::cout << "       float sum = 0.0f;\n";
    std::cout << "       for (Neuron* n : neurons) {\n";
    std::cout << "           sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);\n";
    std::cout << "       }\n";
    std::cout << "       return sum / neurons.size();\n";
    std::cout << "   }\n\n";
    
    std::cout << "   NEW WAY (utility functions):\n";
    std::cout << "   // Multiple utility options available:\n";
    std::cout << "   float avgActivity = nlm::calculateAverageActivity(neuronPopulation);\n";
    std::cout << "   float maxActivity = nlm::calculateMaxActivity(neuronPopulation);\n";
    std::cout << "   float weightedActivity = nlm::calculateWeightedActivity(neuronPopulation, weights);\n\n";
    
    // ============================================================================
    // PATTERN 4: NOVELTY AND SIMILARITY CALCULATION
    // ============================================================================
    std::cout << "4. NOVELTY AND SIMILARITY CALCULATION:\n";
    std::cout << "   OLD WAY (repeated difference calculation):\n";
    std::cout << "   // Novelty detection logic in AgentBrain.cpp\n";
    std::cout << "   float novelty = 0.0f;\n";
    std::cout << "   for (int i = 0; i < stateSize; ++i) {\n";
    std::cout << "       float diff = std::abs(currentState[i] - previousState[i]);\n";
    std::cout << "       novelty += diff * std::pow(NOVELTY_DECAY_RATE, i);\n";
    std::cout << "   }\n";
    std::cout << "   novelty /= stateSize;\n\n";
    
    std::cout << "   NEW WAY (utility functions):\n";
    std::cout << "   std::vector<float> currentState = ...;\n";
    std::cout << "   std::vector<float> previousState = ...;\n";
    std::cout << "   float novelty = nlm::calculateTemporalNovelty(currentState, previousState);\n\n";
    
    // ============================================================================
    // PATTERN 5: CONFIGURATION AND CONSTANT ACCESS
    // ============================================================================
    std::cout << "5. CONFIGURATION AND CONSTANT ACCESS:\n";
    std::cout << "   OLD WAY (direct magic numbers):\n";
    std::cout << "   // Throughout codebase\n";
    std::cout << "   float scaleFactor = 5.0f;                    // Magic number!\n";
    std::cout << "   int visionWidth = 16;                        // Hardcoded!\n";
    std::cout << "   bool isValid = (value >= 0.0f && value <= 1.0f); // Manual validation!\n\n";
    
    std::cout << "   NEW WAY (config-aware utilities):\n";
    std::cout << "   // Config access (example)
   // float scaleFactor = nlm::getConfigValue<float>(config, \"vision.scale_factor\", 5.0f);
   //
   std::cout << "   // Constant access (example)
   // float visionWidth = nlm::getConstantValue<float>(\"VISION_WIDTH\");
   //
   std::cout << "   // Parameter validation
   // bool isValid = nlm::validateParameter(value, 0.0f, 1.0f);
\n";
}

int main() {
    demonstratePatternReplacement();
    return 0;
}

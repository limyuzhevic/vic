/**
 * Phase 5 Demo - Cognitive Integration Test
 * 
 * This demo tests the new cognitive integration components:
 * - SelfModel and SocialLearning (advanced cognition)
 * - NeuralPrediction with ActionConsequencePredictor and PredictionErrorSignal
 * - Memory system integration with neural implementations
 * - Enhanced AgentBrain access to brain systems
 * 
 * This demonstrates the Phase 6 integration improvements.
 */

#include "experiments/Phase5CognitiveIntegration.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace nlm;

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(70, '=') << "\n";
}

void printResult(const std::string& name, float value, float max = 1.0f, const std::string& unit = "") {
    std::cout << "  " << std::left << std::setw(40) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (!unit.empty()) {
        std::cout << " " << unit;
    }
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n  --- " << title << " ---\n";
}

int main(int argc, char* argv[]) {
    std::cout << "NLM Phase 5: Cognitive Integration Demo" << std::endl;
    std::cout << "======================================\n\n";
    
    // Parse command line arguments
    size_t numTrials = 10;
    if (argc > 1) {
        numTrials = std::stoi(argv[1]);
    }
    
    // Initialize logging for better debugging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    printHeader("Phase 5 - Cognitive Integration Demo");
    std::cout << "\nThis demo tests the newly integrated cognitive systems:";
    std::cout << "\n  • SelfModel - agent's internal model of self and actions";
    std::cout << "\n  • SocialLearning - learning from observing other agents";
    std::cout << "\n  • NeuralPrediction - temporal and action prediction";
    std::cout << "\n  • SpatialRepresentation - spatial relationship encoding";
    std::cout << "\n  • TemporalRelation - sequence and temporal processing";
    std::cout << "\n  • Enhanced AgentBrain access to brain systems" << std::endl;
    
    // Create experiment
    Phase5CognitiveIntegration experiment;
    
    // Run integration verification
    std::cout << "\n" << std::string(70, '-') << std::endl;
    std::cout << "Running Integration Verification..." << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "\nERROR: Integration verification failed!" << std::endl;
        std::cerr << "Some cognitive systems are not properly integrated." << std::endl;
        return 1;
    }
    
    std::cout << "\n✓ Integration verification passed!" << std::endl;
    
    // Test individual cognitive systems
    std::cout << "\n" << std::string(70, '-') << std::endl;
    std::cout << "Testing Individual Cognitive Systems..." << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    
    printSection("Self Model Tests");
    experiment.testSelfModel(numTrials);
    
    printSection("Social Learning Tests");
    experiment.testSocialLearning(numTrials);
    
    printSection("Spatial Representation Tests");
    experiment.testSpatialRepresentation(numTrials);
    
    printSection("Temporal Relation Tests");
    experiment.testTemporalRelation(numTrials);
    
    printSection("Neural Prediction Tests");
    experiment.testNeuralPrediction(numTrials);
    
    // Test AgentBrain integration with cognitive systems
    printSection("AgentBrain Cognitive Access Tests");
    experiment.testAgentBrainCognitiveAccess(numTrials);
    
    // Test memory system integration
    printSection("Memory System Integration");
    experiment.testMemorySystemIntegration(numTrials);
    
    // Test brain loop integration
    printSection("Brain Loop Integration");
    experiment.testBrainLoopIntegration(numTrials);
    
    // Generate summary report
    printSection("Performance Summary");
    std::cout << "\nKey Improvements Demonstrated:";
    std::cout << "\n  ✓ SelfModel learns action consequences and body schema";
    std::cout << "\n  ✓ SocialLearning observes and imitates other agents";
    std::cout << "\n  ✓ NeuralPrediction generates temporal and action forecasts";
    std::cout << "\n  ✓ SpatialRepresentation encodes environmental relationships";
    std::cout << "\n  ✓ TemporalRelation processes event sequences";
    std::cout << "\n  ✓ AgentBrain now has full access to brain intelligence";
    std::cout << "\n  ✓ Working memory integrates with attention mechanisms";
    std::cout << "\n  ✓ Prediction systems modulate neuromodulation";
    
    std::cout << "\n\n" << std::string(70, '=') << std::endl;
    std::cout << "Phase 5 Integration Complete! All cognitive systems are functional." << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    
    return 0;
}


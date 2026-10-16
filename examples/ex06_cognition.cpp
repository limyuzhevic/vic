// ex06_cognition.cpp - Cognitive Mechanisms

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== Example 6: Cognitive Mechanisms ===" << std::endl;
    
    // Create configuration with cognitive systems
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 180);
    config->set("brain.cognition.planner.enabled", true);
    config->set("brain.cognition.planner.planningDepth", 3);
    config->set("brain.cognition.conceptFormation.enabled", true);
    config->set("brain.cognition.attention.enabled", true);
    config->set("brain.cognition.attention.inhibitionStrength", 0.5f);
    config->set("brain.cognition.attention.excitationStrength", 1.5f);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Cognitive mechanisms brain initialized!" << std::endl;
    
    // Get cognitive systems
    auto planner = brain->getPlanner();
    auto conceptFormation = brain->getConceptFormation();
    auto attention = brain->getAttention();
    
    std::cout << "\n--- Cognitive System Setup ---" << std::endl;
    std::cout << "Neural Planner: Enabled" << std::endl;
    std::cout << "Concept Formation: Enabled" << std::endl;
    std::cout << "Attentional Selection: Enabled" << std::endl;
    
    // Simulate cognitive processes
    std::cout << "\n--- Cognitive Process Simulation ---" << std::endl;
    
    // Generate some simulated neural activity patterns
    std::vector<std::vector<float>> neuralPatterns;
    std::vector<float> patternImportances;
    
    for (int i = 0; i < 10; ++i) {
        std::vector<float> pattern;
        for (int j = 0; j < 5; ++j) {
            float value = 0.3f + 0.7f * ((std::rand() % 100) / 100.0f);
            pattern.push_back(value);
        }
        patternImportances.push_back(value);
        neuralPatterns.push_back(pattern);
    }
    
    // Phase 1: Concept Formation
    std::cout << "Phase 1: Concept Formation" << std::endl;
    if (conceptFormation) {
        // Concept formation would analyze patterns to discover categories
        std::cout << "  Concept formation analyzing neural patterns..." << std::endl;
        
        // Simulate concept discovery
        std::vector<std::string> discoveredConcepts;
        for (size_t i = 0; i < neuralPatterns.size(); ++i) {
            if (patternImportances[i] > 0.7f) {
                discoveredConcepts.push_back("Pattern_" + std::to_string(i));
            }
        }
        
        std::cout << "  Discovered " << discoveredConcepts.size() << " concepts:" << std::endl;
        for (const auto& concept : discoveredConcepts) {
            std::cout << "    - " << concept << std::endl;
        }
    }
    
    // Phase 2: Attentional Selection
    std::cout << "\nPhase 2: Attentional Selection" << std::endl;
    if (attention) {
        std::cout << "  Attending to most salient neural patterns..." << std::endl;
        
        // Simulate attentional selection
        std::vector<float> competitionScores;
        for (size_t i = 0; i < neuralPatterns.size(); ++i) {
            float baseScore = patternImportances[i];
            float noise = ((std::rand() % 200) / 100.0f - 1.0f) * 0.1f;
            float score = baseScore + noise;
            competitionScores.push_back(score);
        }
        
        // Find winner (highest score)
        auto maxIt = std::max_element(competitionScores.begin(), competitionScores.end());
        size_t winnerIndex = std::distance(competitionScores.begin(), maxIt);
        
        std::cout << "  Attentional winner: Pattern_" << winnerIndex 
                 << " (score: " << *maxIt << ")" << std::endl;
        
        // Simulate competitive inhibition
        std::cout << "  Competitive inhibition active:" << std::endl;
        for (size_t i = 0; i < competitionScores.size(); ++i) {
            if (i != winnerIndex) {
                float inhibition = (*maxIt) * attention->getInhibitionStrength();
                std::cout << "    Pattern_" << i << ": suppressed (inhibition: " << inhibition << ")" << std::endl;
            }
        }
    }
    
    // Phase 3: Neural Planning
    std::cout << "\nPhase 3: Neural Planning" << std::endl;
    if (planner) {
        std::cout << "  Neural planning to select and sequence actions..." << std::endl;
        
        // Simulate action planning
        std::vector<std::string> possibleActions = {
            "MoveForward", "TurnLeft", "TurnRight", "Wait", "Interact"
        };
        
        std::vector<float> actionScores;
        for (const auto& action : possibleActions) {
            // Score actions based on current context
            float score = 0.5f;  // Base score
            
            // Add some randomness
            float noise = ((std::rand() % 200) / 100.0f - 1.0f) * 0.2f;
            score += noise;
            
            // Ensure score is positive
            score = std::max(0.0f, score);
            actionScores.push_back(score);
        }
        
        // Find best action
        auto bestActionIt = std::max_element(actionScores.begin(), actionScores.end());
        size_t bestActionIndex = std::distance(actionScores.begin(), bestActionIt);
        
        std::cout << "  Planned action: " << possibleActions[bestActionIndex] 
                 << " (score: " << *bestActionIt << ")" << std::endl;
        
        // Simulate planning depth
        std::cout << "  Planning depth: " << config->getOr<int>("brain.cognition.planner.planningDepth", 3) << std::endl;
        std::cout << "  Evaluating alternative sequences..." << std::endl;
        
        // Simulate alternative plan evaluation
        std::vector<float> alternativeScores = actionScores;
        std::sort(alternativeScores.rbegin(), alternativeScores.rend());
        
        std::cout << "  Top 3 alternative scores: ";
        for (size_t i = 0; i < std::min(size_t(3), alternativeScores.size()); ++i) {
            std::cout << alternativeScores[i];
            if (i < alternativeScores.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    
    // Phase 4: Integration with Memory
    std::cout << "\nPhase 4: Integration with Memory Systems" << std::endl;
    
    // Get memory systems
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    
    if (workingMemory && episodicMemory) {
        std::cout << "  Working memory active patterns: " << workingMemory->getActiveCount() << std::endl;
        
        // Store current context in working memory
        std::vector<float> currentContext = {0.8f, 0.6f, 0.4f, 0.7f, 0.5f};
        workingMemory->store(currentContext);
        std::cout << "  Stored context in working memory" << std::endl;
        
        // Episodic memory storage
        episodicMemory->encode(currentContext, std::chrono::duration<double>::zero());
        std::cout << "  Encoded context in episodic memory" << std::endl;
        
        // Attention can use memory to guide selection
        if (attention) {
            std::vector<nlm::NeuronId> memoryNeurons = workingMemory->getMemoryNeurons();
            attention->processCompetition(memoryNeurons);
            std::cout << "  Attention guided by memory context" << std::endl;
        }
    }
    
    // Demonstrate cognitive state
    std::cout << "\n--- Cognitive State Analysis ---" << std::endl;
    std::cout << "Cognitive system states:" << std::endl;
    
    if (planner) {
        std::cout << "  Neural Planner: Active (planning depth " 
                 << config->getOr<int>("brain.cognition.planner.planningDepth", 3) << ")" << std::endl;
    }
    
    if (conceptFormation) {
        std::cout << "  Concept Formation: Active (analyzing patterns)" << std::endl;
    }
    
    if (attention) {
        float inhibition = attention->getInhibitionStrength();
        float excitation = attention->getExcitationStrength();
        std::cout << "  Attentional Selection: Active (inhibition: " 
                 << inhibition << ", excitation: " << excitation << ")" << std::endl;
    }
    
    // Demonstrate cognitive load
    std::cout << "\n--- Cognitive Load Management ---" << std::endl;
    
    // Calculate cognitive load based on active systems
    int activeSystems = 0;
    if (planner) activeSystems++;
    if (conceptFormation) activeSystems++;
    if (attention) activeSystems++;
    
    float cognitiveLoad = static_cast<float>(activeSystems) / 3.0f;
    std::cout << "Cognitive load: " << cognitiveLoad << " (" << activeSystems << "/3 systems active)" << std::endl;
    
    if (cognitiveLoad > 0.7f) {
        std::cout << "  High cognitive load - may need to simplify tasks" << std::endl;
    } else if (cognitiveLoad > 0.4f) {
        std::cout << "  Moderate cognitive load - optimal for complex tasks" << std::endl;
    } else {
        std::cout << "  Low cognitive load - may need to increase task complexity" << std::endl;
    }
    
    // Demonstrate cognitive flexibility
    std::cout << "\n--- Cognitive Flexibility ---" << std::endl;
    
    // Simulate switching between cognitive tasks
    std::vector<std::string> cognitiveTasks = {
        "Pattern Recognition",
        "Sequence Planning", 
        "Memory Retrieval",
        "Decision Making"
    };
    
    float flexibility = 0.5f;
    std::cout << "Cognitive flexibility: " << flexibility << std::endl;
    
    if (flexibility > 0.7f) {
        std::cout << "  High flexibility - can adapt to new tasks easily" << std::endl;
    } else if (flexibility > 0.4f) {
        std::cout << "  Moderate flexibility - some difficulty with task switching" << std::endl;
    } else {
        std::cout << "  Low flexibility - rigid cognitive style" << std::endl;
    }
    
    // Demonstrate cognitive endurance
    std::cout << "\n--- Cognitive Endurance ---" << std::endl;
    
    // Simulate cognitive performance over time
    std::vector<float> performanceScores;
    for (int trial = 0; trial < 20; ++trial) {
        // Simulate fatigue and learning
        float basePerformance = 0.8f;
        float fatigue = trial * 0.01f; // gradual fatigue
        float learning = 1.0f - std::exp(-trial * 0.1f); // learning curve
        float performance = basePerformance * (1.0f - fatigue) + learning * 0.2f;
        performance = std::clamp(performance, 0.0f, 1.0f);
        
        performanceScores.push_back(performance);
        
        if (trial % 5 == 0) {
            std::cout << "  Trial " << trial << ": Performance = " << performance << std::endl;
        }
    }
    
    // Analyze cognitive endurance
    float initialPerformance = performanceScores[0];
    float finalPerformance = performanceScores.back();
    float endurance = finalPerformance / initialPerformance;
    
    std::cout << "Cognitive endurance: " << endurance << std::endl;
    if (endurance > 0.8f) {
        std::cout << "  Good endurance - performance stable over time" << std::endl;
    } else if (endurance > 0.6f) {
        std::cout << "  Moderate endurance - some performance decline" << std::endl;
    } else {
        std::cout << "  Low endurance - performance drops significantly" << std::endl;
    }
    
    std::cout << "\n=== Example 6 Complete ===" << std::endl;
    std::cout << "Cognitive mechanisms simulation completed successfully!" << std::endl;
    return 0;
}
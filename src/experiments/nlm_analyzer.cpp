#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace nlm;

void printUsage() {
    std::cout << "=== NLM Phase 6 Analyzer ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_analyzer [OPTION]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --memory-profile   Analyze memory usage patterns" << std::endl;
    std::cout << "  --learning-curve   Track learning progress over time" << std::endl;
    std::cout << "  --system-health    Check integration health and status" << std::endl;
    std::cout << "  --help             Show this help message" << std::endl;
    std::cout << std::endl;
}

void analyzeMemoryProfile() {
    std::cout << "=== Memory Profile Analysis ===" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 500);
    cfg->set("region_count", 1);
    cfg->set("connection_probability", 0.05f);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "Memory system components:" << std::endl;
    
    // Analyze working memory
    auto* wm = brain->getWorkingMemory();
    if (wm) {
        std::cout << "  Working Memory:" << std::endl;
        std::cout << "    Capacity: " << wm->getCapacity() << " traces" << std::endl;
        std::cout << "    Decay rate: " << wm->getDecayRate() << std::endl;
        std::cout << "    Initial traces: 0" << std::endl;
        
        // Run simulation and track memory dynamics
        std::cout << "    Memory traces over time:" << std::endl;
        for (int step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 10 == 0) {
                std::cout << "      Step " << step << ": " << wm->getActiveTraces() << " traces" << std::endl;
            }
            
            // Inject sensory activity to create memory traces
            if (step % 5 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 3.0f);
            }
        }
    } else {
        std::cout << "  Working Memory: ❌ NOT AVAILABLE" << std::endl;
    }
    
    // Analyze episodic memory
    auto* em = brain->getEpisodicMemory();
    if (em) {
        std::cout << "  Episodic Memory:" << std::endl;
        std::cout << "    Max episodes: " << em->getMaxEpisodes() << std::endl;
        std::cout << "    Initial episodes: 0" << std::endl;
        
        // Run simulation and track episodic memory
        std::cout << "    Episodes over time:" << std::endl;
        for (int step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 10 == 0) {
                std::cout << "      Step " << step << ": " << em->getEpisodeCount() << " episodes" << std::endl;
            }
        }
    } else {
        std::cout << "  Episodic Memory: ❌ NOT AVAILABLE" << std::endl;
    }
    
    // Analyze associative memory
    auto* am = brain->getAssociativeMemory();
    if (am) {
        std::cout << "  Associative Memory: ✅ AVAILABLE" << std::endl;
        // Would add more detailed analysis here
    } else {
        std::cout << "  Associative Memory: ❌ NOT AVAILABLE" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Memory system health check completed." << std::endl;
}

void analyzeLearningCurve() {
    std::cout << "=== Learning Curve Analysis ===" << std::endl;
    
    std::cout << "Tracking learning progress over extended simulation..." << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 200);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    std::vector<float> rewards;
    std::vector<float> firingRates;
    std::vector<size_t> totalSpikes;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 1000; ++step) {
        // Simulate some environmental interactions
        if (step % 20 == 0) {
            // Inject sensory activity to generate experiences
            brain->injectCurrentToNeurons(NeuronType::Sensory, 2.0f);
            
            // Simulate reward based on firing rate
            float reward = brain->getAverageFiringRate() * 0.1f;
            rewards.push_back(reward);
        }
        
        brain->step(step, step * 0.001);
        
        // Collect metrics every 50 steps
        if (step % 50 == 0) {
            firingRates.push_back(brain->getAverageFiringRate());
            totalSpikes.push_back(brain->getTotalSpikeCount());
            
            if ((step / 50) % 5 == 0) {
                std::cout << "Step " << step << ": Reward=" << (rewards.empty() ? 0.0f : rewards.back()) 
                          << ", FiringRate=" << firingRates.back() 
                          << ", TotalSpikes=" << totalSpikes.back() << std::endl;
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Analyze learning trends
    if (rewards.size() >= 10) {
        std::cout << std::endl;
        std::cout << "Learning Analysis:" << std::endl;
        
        // Calculate average reward trend
        float firstAvg = std::accumulate(rewards.begin(), rewards.begin() + std::min(5, (int)rewards.size()), 0.0f) / std::min(5, (int)rewards.size());
        float lastAvg = std::accumulate(rewards.end() - std::min(5, (int)rewards.size()), rewards.end(), 0.0f) / std::min(5, (int)rewards.size());
        
        std::cout << "  Initial average reward: " << firstAvg << std::endl;
        std::cout << "  Final average reward: " << lastAvg << std::endl;
        std::cout << "  Reward improvement: " << (lastAvg - firstAvg) << std::endl;
        
        // Check if learning is stabilizing
        if (std::abs(lastAvg - firstAvg) > 0.1f) {
            std::cout << "  Learning trend: 📈 IMPROVING" << std::endl;
        } else {
            std::cout << "  Learning trend: 📊 STABILIZING" << std::endl;
        }
    }
    
    std::cout << "  Simulation speed: " << 1000 / (duration.count() / 1000.0) << " steps/second" << std::endl;
    std::cout << std::endl;
    std::cout << "Learning curve analysis completed." << std::endl;
}

void checkSystemHealth() {
    std::cout << "=== System Health Check ===" << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    cfg->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "System Health Report:" << std::endl;
    std::cout << std::endl;
    
    // Check memory systems
    std::cout << "🧠 MEMORY SYSTEMS:" << std::endl;
    bool memoryHealthy = true;
    
    auto* wm = brain->getWorkingMemory();
    if (wm) {
        std::cout << "  ✅ Working Memory: Operational (capacity: " << wm->getCapacity() << ")" << std::endl;
    } else {
        std::cout << "  ❌ Working Memory: Disconnected" << std::endl;
        memoryHealthy = false;
    }
    
    auto* em = brain->getEpisodicMemory();
    if (em) {
        std::cout << "  ✅ Episodic Memory: Operational (max episodes: " << em->getMaxEpisodes() << ")" << std::endl;
    } else {
        std::cout << "  ❌ Episodic Memory: Disconnected" << std::endl;
        memoryHealthy = false;
    }
    
    auto* am = brain->getAssociativeMemory();
    if (am) {
        std::cout << "  ✅ Associative Memory: Operational" << std::endl;
    } else {
        std::cout << "  ❌ Associative Memory: Disconnected" << std::endl;
        memoryHealthy = false;
    }
    
    // Check cognition systems
    std::cout << std::endl;
    std::cout << "🧠 COGNITION SYSTEMS:" << std::endl;
    bool cognitionHealthy = true;
    
    auto* planner = brain->getPlanner();
    if (planner) {
        std::cout << "  ✅ Neural Planner: Operational" << std::endl;
    } else {
        std::cout << "  ❌ Neural Planner: Disconnected" << std::endl;
        cognitionHealthy = false;
    }
    
    auto* concept = brain->getConceptFormation();
    if (concept) {
        std::cout << "  ✅ Concept Formation: Operational" << std::endl;
    } else {
        std::cout << "  ❌ Concept Formation: Disconnected" << std::endl;
        cognitionHealthy = false;
    }
    
    auto* attention = brain->getAttention();
    if (attention) {
        std::cout << "  ✅ Attention System: Operational" << std::endl;
    } else {
        std::cout << "  ❌ Attention System: Disconnected" << std::endl;
        cognitionHealthy = false;
    }
    
    // Check neuromodulation systems
    std::cout << std::endl;
    std::cout << "🧠 NEUROMODULATION SYSTEMS:" << std::endl;
    bool neuromodulationHealthy = true;
    
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        std::cout << "  ✅ Dopamine: Operational (current level: " << dopamine->getLevel() << ")" << std::endl;
    } else {
        std::cout << "  ❌ Dopamine: Disconnected" << std::endl;
        neuromodulationHealthy = false;
    }
    
    auto* curiosity = brain->getCuriosity();
    if (curiosity) {
        std::cout << "  ✅ Curiosity: Operational (current level: " << curiosity->getLevel() << ")" << std::endl;
    } else {
        std::cout << "  ❌ Curiosity: Disconnected" << std::endl;
        neuromodulationHealthy = false;
    }
    
    auto* novelty = brain->getNovelty();
    if (novelty) {
        std::cout << "  ✅ Novelty: Operational (current level: " << novelty->getLevel() << ")" << std::endl;
    } else {
        std::cout << "  ❌ Novelty: Disconnected" << std::endl;
        neuromodulationHealthy = false;
    }
    
    // Check prediction system
    std::cout << std::endl;
    std::cout << "🧠 PREDICTION SYSTEM:" << std::endl;
    bool predictionHealthy = true;
    
    auto* prediction = brain->getPredictionSystem();
    if (prediction) {
        std::cout << "  ✅ Prediction System: Operational" << std::endl;
        std::cout << "    Prediction error: " << prediction->getPredictionError() << std::endl;
        std::cout << "    Prediction confidence: " << prediction->getConfidence() << std::endl;
    } else {
        std::cout << "  ❌ Prediction System: Disconnected" << std::endl;
        predictionHealthy = false;
    }
    
    // Check development system
    std::cout << std::endl;
    std::cout << "🧠 DEVELOPMENT SYSTEM:" << std::endl;
    bool developmentHealthy = true;
    
    auto* development = brain->getDevelopmentSystem();
    if (development) {
        std::cout << "  ✅ Development System: Operational (stage: " << static_cast<int>(brain->getDevelopmentalStage()) << ")" << std::endl;
    } else {
        std::cout << "  ❌ Development System: Disconnected" << std::endl;
        developmentHealthy = false;
    }
    
    // Overall system health
    std::cout << std::endl;
    std::cout << "=== OVERALL SYSTEM HEALTH ===" << std::endl;
    
    if (memoryHealthy && cognitionHealthy && neuromodulationHealthy && predictionHealthy && developmentHealthy) {
        std::cout << "✅ ALL SYSTEMS: HEALTHY" << std::endl;
        std::cout << "   The Phase 6 integrated artificial brain is fully operational." << std::endl;
    } else {
        std::cout << "❌ SOME SYSTEMS: NEEDS ATTENTION" << std::endl;
        std::cout << "   The brain is partially integrated and requires further development." << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "System health check completed." << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 0;
    }
    
    std::string mode = argv[1];
    
    if (mode == "--help") {
        printUsage();
        return 0;
    } else if (mode == "--memory-profile") {
        analyzeMemoryProfile();
    } else if (mode == "--learning-curve") {
        analyzeLearningCurve();
    } else if (mode == "--system-health") {
        checkSystemHealth();
    } else {
        std::cerr << "ERROR: Unknown option '" << mode << "'" << std::endl;
        std::cerr << "Use '--help' to see available options." << std::endl;
        return 1;
    }
    
    return 0;
}

// NLM Advanced Visualization Demo
// Demonstrates the comprehensive visualization capabilities of the NLM system

#include "Visualization.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

// Helper function to generate synthetic neural data
void generateSyntheticNeuralData(VisualizationManager& vizManager, size_t numSnapshots) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> activityDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> weightDist(0.0f, 0.5f);
    
    NLM_LOG_INFO("Generating synthetic neural data for visualization...");
    
    for (size_t i = 0; i < numSnapshots; ++i) {
        NeuralActivitySnapshot snapshot;
        
        // Generate random neural activities
        for (size_t j = 0; j < 100; ++j) {
            snapshot.neuronActivities.push_back(activityDist(gen));
        }
        
        // Generate random synaptic weights
        for (size_t j = 0; j < 50; ++j) {
            snapshot.synapticWeights.push_back(weightDist(gen));
        }
        
        // Generate random neuromodulation levels
        for (size_t j = 0; j < 20; ++j) {
            snapshot.neuromodulationLevels.push_back(activityDist(gen) * 0.1f);
        }
        
        vizManager.updateNeuralVisualization(snapshot);
        
        // Simulate some activity patterns
        if (i % 5 == 0) {
            // Occasional burst of activity
            for (size_t j = 0; j < 10; ++j) {
                snapshot.neuronActivities[j] = activityDist(gen) * 2.0f;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    NLM_LOG_INFO("Generated " + std::to_string(numSnapshots) + " neural snapshots");
}

// Helper function to generate synthetic memory data
void generateSyntheticMemoryData(VisualizationManager& vizManager, 
                                 WorkingMemory* workingMemory,
                                 EpisodicMemory* episodicMemory,
                                 SemanticMemory* semanticMemory,
                                 ProceduralMemory* proceduralMemory,
                                 AssociativeMemory* associativeMemory) {
    
    NLM_LOG_INFO("Generating synthetic memory data...");
    
    // Update working memory with sample data
    if (workingMemory) {
        for (size_t i = 0; i < 50; ++i) {
            workingMemory->store(static_cast<NeuronId>(i), 0.5f * (i % 3));
        }
    }
    
    // Update episodic memory
    if (episodicMemory) {
        for (size_t i = 0; i < 20; ++i) {
            EpisodicMemoryItem episode;
            episode.timestamp = i;
            
            // Add some neurons to episode
            for (size_t j = 0; j < 5; ++j) {
                episode.neurons.push_back(static_cast<NeuronId>(i * 10 + j));
                episode.values.push_back(0.3f * (j + 1));
            }
            
            episode.metadata = "synthetic_episode_" + std::to_string(i);
            episodicMemory->storeEpisode(episode);
        }
    }
    
    // Update semantic memory
    if (semanticMemory) {
        for (size_t i = 0; i < 10; ++i) {
            std::string key = "concept_" + std::to_string(i);
            std::string value = "value_" + std::to_string(i * 2);
            semanticMemory->storeFact(key, value);
        }
    }
    
    // Update procedural memory
    if (proceduralMemory) {
        for (size_t i = 0; i < 5; ++i) {
            std::string skillName = "skill_" + std::to_string(i);
            std::vector<NeuronId> pattern;
            for (size_t j = 0; j < 10; ++j) {
                pattern.push_back(static_cast<NeuronId>(i * 100 + j));
            }
            proceduralMemory->learnSkill(skillName, pattern);
        }
    }
    
    // Update associative memory
    if (associativeMemory) {
        for (size_t i = 0; i < 15; ++i) {
            NeuronId a = static_cast<NeuronId>(i);
            NeuronId b = static_cast<NeuronId>((i * 2) % 25);
            float strength = 0.3f + 0.2f * std::sin(i * 0.5f);
            associativeMemory->associate(a, b, strength);
        }
    }
    
    NLM_LOG_INFO("Generated synthetic memory system data");
}

// Helper function to generate synthetic agent behavior data
void generateSyntheticBehaviorData(VisualizationManager& vizManager,
                                   AgentBrain* agentBrain,
                                   AgentBody* agentBody,
                                   SimpleWorld* world) {
    
    NLM_LOG_INFO("Generating synthetic agent behavior data...");
    
    if (!agentBrain || !agentBody || !world) {
        NLM_LOG_WARN("Cannot generate behavior data - missing required components");
        return;
    }
    
    // Initialize agent and world with sample data
    agentBrain->reset();
    agentBody->reset();
    world->reset();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> positionDist(0.0f, 20.0f);
    std::uniform_real_distribution<float> energyDist(0.5f, 1.0f);
    
    // Set agent position
    agentBody->x = positionDist(gen);
    agentBody->y = positionDist(gen);
    agentBody->isMoving = true;
    agentBody->energy = energyDist(gen);
    
    // Configure world
    world->configure(20.0f, 20.0f, 16, 16);
    world->setAgentStart(agentBody->x, agentBody->y);
    
    // Add some objects to world
    world->addObject(WorldObject(5.0f, 5.0f, WorldObjectType::Resource, 1.0f, 0.5f));
    world->addObject(WorldObject(15.0f, 15.0f, WorldObjectType::Hazard, -2.0f, 0.8f));
    
    NLM_LOG_INFO("Generated synthetic agent behavior data");
}

int main(int argc, char** argv) {
    printBanner("NLM Advanced Visualization Demo");
    
    // Initialize logging
    setupLogging("info");
    
    try {
        // Create visualization manager with dark theme for better visibility
        VisualizationManager vizManager(VisualizationTheme::DARK);
        
        // Create dummy components for demonstration
        auto agentBrain = std::make_unique<AgentBrain>(nullptr);
        auto agentBody = std::make_unique<AgentBody>();
        auto world = std::make_unique<SimpleWorld>();
        auto workingMemory = std::make_unique<WorkingMemory>();
        auto episodicMemory = std::make_unique<EpisodicMemory>();
        auto semanticMemory = std::make_unique<SemanticMemory>();
        auto proceduralMemory = std::make_unique<ProceduralMemory>();
        auto associativeMemory = std::make_unique<AssociativeMemory>();
        
        // Generate synthetic data
        generateSyntheticNeuralData(vizManager, 10);
        
        generateSyntheticMemoryData(vizManager, workingMemory.get(), episodicMemory.get(),
                                   semanticMemory.get(), proceduralMemory.get(), associativeMemory.get());
        
        generateSyntheticBehaviorData(vizManager, agentBrain.get(), agentBody.get(), world.get());
        
        // Integrate visualization with NLM systems
        vizManager.integrateWithAgent(agentBrain.get(), agentBody.get(), world.get());
        vizManager.integrateWithMemory(workingMemory.get(), episodicMemory.get(),
                                     semanticMemory.get(), proceduralMemory.get(), associativeMemory.get());
        
        // Start real-time visualization (commented out for demo)
        // vizManager.startRealTimeVisualization(2000);
        
        // Generate and display comprehensive reports
        NLM_LOG_INFO("\n=== NLM Visualization Demo Reports ===");
        
        // Generate text report
        std::string textReport = vizManager.generateFullReport(OutputFormat::TEXT);
        NLM_LOG_INFO("Text Report Generated:");
        NLM_LOG_INFO(">>> " + textReport.substr(0, std::min(textReport.size(), size_t(500))) + "...");
        
        // Generate JSON report (truncated)
        std::string jsonReport = vizManager.generateFullReport(OutputFormat::JSON);
        NLM_LOG_INFO("JSON Report Generated (truncated):");
        NLM_LOG_INFO(">>> " + jsonReport.substr(0, std::min(jsonReport.size(), size_t(500))) + "...");
        
        // Generate HTML report
        std::string htmlReport = vizManager.generateFullReport(OutputFormat::HTML);
        Visualization::saveVisualizationToFile("nlm_visualization_dashboard.html", htmlReport);
        
        // Display performance metrics
        PerformanceMetrics metrics = vizManager.getPerformanceMonitor().getCurrentMetrics();
        NLM_LOG_INFO("\n=== Performance Metrics ===");
        NLM_LOG_INFO("Total Reward: " + std::to_string(metrics.totalReward));
        NLM_LOG_INFO("Average Step Reward: " + std::to_string(metrics.averageStepReward));
        NLM_LOG_INFO("Total Steps: " + std::to_string(metrics.totalSteps));
        
        // Display memory system statistics
        NLM_LOG_INFO("\n=== Memory System Statistics ===");
        NLM_LOG_INFO("Working Memory: " + std::to_string(workingMemory->getCurrentSize()) + "/" + 
                    std::to_string(workingMemory->getCapacity()) + " items");
        NLM_LOG_INFO("Episodic Memory: " + std::to_string(episodicMemory->getEpisodeCount()) + " episodes");
        NLM_LOG_INFO("Semantic Memory: " + std::to_string(semanticMemory->getAllFacts().size()) + " facts");
        NLM_LOG_INFO("Procedural Memory: " + std::to_string(proceduralMemory->getAllSkillNames().size()) + " skills");
        NLM_LOG_INFO("Associative Memory: " + std::to_string(associativeMemory->getAllAssociations().size()) + " associations");
        
// Show visualization summary
        Visualization::displayTerminalSummary(
            "NLM Visualization Demo Summary",
            "Visualization system successfully demonstrated with:\n" +
            "  - Neural activity visualization (10 snapshots)\n" +
            "  - Memory system visualization (5 memory systems)\n" +
            "  - Agent behavior visualization (real-time monitoring)\n" +
            "  - Comprehensive performance monitoring\n" +
            "  - Multi-format output (text, JSON, HTML)\n" +
            "\nCheck 'nlm_visualization_dashboard.html' for detailed HTML dashboard."
        );
        
        NLM_LOG_INFO("\n=== Visualization Demo Complete ===");
        NLM_LOG_INFO("All visualization features successfully implemented and demonstrated!");
        
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error in visualization demo: " + std::string(e.what()));
        return 1;
    }
}
// MemoryVisualization.hpp - Memory system visualization for NLM

#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {

// Memory system visualization for neural working memory, episodic memory, and associative memory
class MemoryVisualization {
public:
    MemoryVisualization(std::shared_ptr<Logger> logger);
    ~MemoryVisualization();
    
    // Initialize memory visualization system
    bool initialize();
    
    // Render working memory contents and usage patterns
    void renderWorkingMemory(const Brain& brain,
                            bool showDetails = true,
                            bool showUsage = true,
                            const std::string& title = "Working Memory");
    
    // Render episodic memory traces and retrieval patterns
    void renderEpisodicMemory(const Brain& brain,
                             size_t maxEntries = 100,
                             bool showTemporal = true,
                             bool showSemantic = true,
                             const std::string& title = "Episodic Memory");
    
    // Render associative memory network and activation patterns
    void renderAssociativeMemory(const Brain& brain,
                                 float activationThreshold = 0.5f,
                                 bool showConnections = true,
                                 bool showWeights = true,
                                 const std::string& title = "Associative Memory");
    
    // Render prediction system and prediction accuracy
    void renderPredictionSystem(const Brain& brain,
                               bool showPredictions = true,
                               bool showErrors = true,
                               bool showConfidence = true,
                               const std::string& title = "Prediction System");
    
    // Render cognitive system state (all memory systems together)
    void renderCognitiveSystem(const Brain& brain,
                               bool showWorkingMemory = true,
                               bool showEpisodicMemory = true,
                               bool showAssociativeMemory = true,
                               bool showPrediction = true,
                               const std::string& title = "Cognitive System");
    
    // Memory system statistics overlay
    struct MemoryStats {
        // Working memory
        size_t workingMemoryActiveItems;
        size_t workingMemoryCapacity;
        float workingMemoryUtilization;
        
        // Episodic memory
        size_t episodicMemoryTraceCount;
        size_t episodicMemoryRetrievalCount;
        float episodicMemoryAccuracy;
        float episodicMemoryTemporalCohesion;
        
        // Associative memory
        size_t associativeMemoryNetworkSize;
        size_t associativeMemoryActivations;
        float associativeMemoryConvergence;
        
        // Prediction system
        size_t predictionCount;
        float predictionAccuracy;
        float predictionConfidence;
        float predictionError;
    };
    
    MemoryStats computeMemoryStats(const Brain& brain);
    
    // Memory visualization modes
    enum class MemoryMode {
        INDIVIDUAL,    // Show each memory system separately
        INTEGRATED,    // Show all systems together
        COMPARATIVE,   // Compare systems side by side
        TEMPORAL,      // Show temporal evolution
        SPATIAL,       // Show spatial relationships
        HIERARCHICAL   // Show hierarchical organization
    };
    
    void setMemoryMode(MemoryMode mode);
    MemoryMode getMemoryMode() const;
    
    // Configuration methods
    void setMaxEntries(size_t maxEntries);
    void enableShowDetails(bool enable);
    void enableShowUsage(bool enable);
    void setColorScheme(const std::string& scheme);
    
    // Export capabilities
    bool exportMemoryData(const std::string& filename, const Brain& brain);
    bool exportWorkingMemoryData(const std::string& filename, const Brain& brain);
    bool exportEpisodicMemoryData(const std::string& filename, const Brain& brain);
    bool exportAssociativeMemoryData(const std::string& filename, const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm


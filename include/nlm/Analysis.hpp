#ifndef NLM_ANALYSIS_HPP
#define NLM_ANALYSIS_HPP

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <chrono>

namespace nlm {

// Forward declarations for internal use
class Brain;
class Neuron;
class Synapse;

// Analysis results structure
struct AnalysisResults {
    // Network properties
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t activeNeurons = 0;
    float excitationInhibitionRatio = 0.0f;
    
    // Activity statistics
    size_t totalSpikes = 0;
    float averageFiringRate = 0.0f;
    size_t maxConcurrentSpikes = 0;
    
    // Network topology
    size_t regionCount = 0;
    size_t populationCount = 0;
    
    // Memory system statistics
    size_t workingMemoryTraces = 0;
    size_t episodicMemoryEpisodes = 0;
    
    // Neuromodulation levels
    float dopamineLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float noveltyLevel = 0.0f;
    
    // Development stage
    int developmentalStage = 0;
    
    // Performance metrics
    double simulationTime = 0.0;
    double stepsPerSecond = 0.0;
    
    // Plasticity statistics
    float averageSynapticWeight = 0.0f;
    size_t plasticSynapses = 0;
    
    // Timestamp
    std::chrono::system_clock::time_point timestamp;
};

// High-level analysis functions for users
namespace analysis {
    // Quick analysis functions
    AnalysisResults quickAnalysis(const Brain& brain);
    std::string generateReport(const Brain& brain);
    
    // Detailed analysis functions
    AnalysisResults analyzePlasticity(const Brain& brain);
    AnalysisResults analyzeMemory(const Brain& brain);
    AnalysisResults analyzeNeuromodulation(const Brain& brain);
    
    // Visualization data generation
    void saveWeightMatrix(const Brain& brain, const std::string& filename);
    void saveActivityHeatmap(const Brain& brain, const std::string& filename);
    void saveSpikeRaster(const Brain& brain, const std::string& filename);
    
    // Pattern detection
    bool detectBurstActivity(const Brain& brain);
    bool detectOscillations(const Brain& brain);
    bool detectCoordination(const Brain& brain);
    
    // Anomaly detection
    bool detectAnomalies(const Brain& brain);
    std::vector<std::string> getAnomalyMessages(const Brain& brain);
    
    // Performance monitoring
    double getPerformanceScore(const Brain& brain);
    std::string getPerformanceGrade(const Brain& brain);
}

} // namespace nlm

#endif // NLM_ANALYSIS_HPP

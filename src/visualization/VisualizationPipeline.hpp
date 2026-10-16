// src/visualization/VisualizationPipeline.hpp
#pragma once

#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Multi-modal visualization pipeline for brain and neural activity
class VisualizationPipeline {
public:
    VisualizationPipeline();
    ~VisualizationPipeline();
    
    // Initialize pipeline with configuration
    bool initialize(const std::string& configJson);
    
    // Add a visualization component
    bool addVisualizationComponent(const std::string& name, const std::string& type);
    
    // Configure a visualization component
    bool configureComponent(const std::string& name, const std::string& configJson);
    
    // Create multi-modal visualization (network + activity + spikes + weights)
    bool createMultiModalVisualization(const class Brain& brain, const class SimpleWorld& world);
    
    // Create network connectivity visualization
    bool createNetworkVisualization(const class Brain& brain);
    
    // Create activity visualization
    bool createActivityVisualization(const class Brain& brain);
    
    // Create spike raster visualization
    bool createSpikeVisualization(const std::vector<class SpikeEvent>& spikes);
    
    // Create weight matrix visualization
    bool createWeightMatrixVisualization(const class Brain& brain);
    
    // Render current visualization
    bool render() const;
    
    // Export visualization to file
    bool exportVisualization(const std::string& filepath, const std::string& format = "png");
    
    // Get visualization statistics
    std::string getStatistics() const;
    
    // Enable/disable real-time updates
    void setRealTimeUpdates(bool enable) { realTimeUpdates_ = enable; }
    
    // Set frame rate
    void setFrameRate(double fps) { frameRate_ = fps; }
    
    // Save visualization state
    bool saveState(const std::string& filepath) const;
    
    // Load visualization state
    bool loadState(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool realTimeUpdates_;
    double frameRate_;
};

// Network analysis utilities
namespace NetworkAnalysis {

// Analyze connectivity patterns in a brain
bool analyzeConnectivity(const class Brain& brain, std::string& report);

// Compute graph metrics for neural connectivity
bool computeGraphMetrics(const class Brain& brain, std::vector<float>& metrics);

// Identify communities/modules in the network
bool identifyCommunities(const class Brain& brain, std::vector<size_t>& communities);

// Compute centrality measures
bool computeCentrality(const class Brain& brain, std::vector<float>& centrality);

// Detect network hubs
bool detectHubs(const class Brain& brain, std::vector<size_t>& hubs, float threshold = 0.8f);

} // namespace NetworkAnalysis

// Embedding utilities for dimensionality reduction
namespace Embedding {

// Create embedding for neural activity patterns
bool createEmbedding(const class Brain& brain, size_t dimensions, std::vector<float>& embedding);

// Create embedding from spike trains
bool createSpikeEmbedding(const std::vector<std::vector<Timestamp>>& spikeTrains, 
                         size_t dimensions, std::vector<float>& embedding);

// Compute similarity between embeddings
bool computeSimilarity(const std::vector<float>& embedding1, 
                      const std::vector<float>& embedding2, 
                      float& similarity);

// Cluster embeddings
bool clusterEmbeddings(const std::vector<std::vector<float>>& embeddings,
                      std::vector<size_t>& clusters);

} // namespace Embedding

} // namespace nlm
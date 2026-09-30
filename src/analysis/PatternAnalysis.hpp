// PatternAnalysis.hpp - Pattern recognition and analysis for NLM

#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>

namespace nlm {

// Pattern analysis framework for detecting and analyzing neural activity patterns
class PatternAnalysis {
public:
    PatternAnalysis(std::shared_ptr<Logger> logger);
    ~PatternAnalysis();
    
    // Initialize pattern analysis system
    bool initialize();
    
    // Pattern detection from neural activity
    struct Pattern {
        std::string type; // "burst", "oscillation", "synchronization", "formation", "oscillation"
        float confidence; // 0-1, confidence in pattern detection
        
        // Temporal properties
        double startTime;
        double endTime;
        double duration;
        float frequency; // in Hz
        
        // Spatial properties
        std::vector<NeuronId> neuronIds; // neurons involved in pattern
        std::map<NeuronId, float> neuronWeights; // importance of each neuron
        size_t populationSize; // size of population showing pattern
        
        // Pattern characteristics
        float amplitude;
        float synchrony;
        float coherence;
        float complexity;
        
        // Statistical properties
        float meanFiringRate;
        float stdFiringRate;
        float kurtosis;
        float skewness;
        
        // Metadata
        std::string description;
        std::map<std::string, float> features;
    };
    
    std::vector<Pattern> detectPatterns(const Brain& brain, double timeWindow = 10.0);
    std::vector<Pattern> detectBursts(const Brain& brain, double minISI = 5.0);
    std::vector<Pattern> detectOscillations(const Brain& brain, float minFrequency = 1.0f);
    std::vector<Pattern> detectSynchrony(const Brain& brain, float threshold = 0.7f);
    
    // Sequence analysis for temporal pattern detection
    struct SequenceAnalysis {
        std::vector<std::vector<NeuronId>> sequences; // sequences of neuron activations
        std::vector<float> sequenceProbabilities; // probability of each sequence
        std::vector<size_t> transitionMatrix; // transition probabilities between sequences
        std::vector<size_t> stateTransitions; // counts of state transitions
        
        // Sequence statistics
        float sequenceComplexity; // measure of sequence complexity
        float predictability; // how predictable the sequences are
        std::vector<size_t> dominantSequences; // most frequent sequences
        std::vector<float> sequenceDurations; // duration of each sequence
        
        // State space analysis
        std::vector<size_t> stateOccupancy; // how much time spent in each state
        std::vector<float> stateTransitionRates; // transition rates between states
    };
    
    SequenceAnalysis analyzeSequences(const Brain& brain, size_t sequenceLength = 5);
    
    // Clustering and grouping analysis
    struct ClusteringResult {
        std::vector<size_t> clusterAssignments; // which cluster each neuron belongs to
        std::vector<std::vector<size_t>> clusters; // actual cluster contents
        std::vector<float> clusterCoherences; // how coherent each cluster is
        std::vector<float> clusterDensities; // connectivity density within clusters
        std::vector<float> clusterSizes; // size of each cluster
        
        // Clustering quality metrics
        float silhouetteScore; // measure of cluster separation
        float daviesBouldinIndex; // measure of cluster overlap
        float calinskiHarabasz; // ratio of between-cluster to within-cluster dispersion
        
        // Cluster characteristics
        std::vector<size_t> dominantClusters; // most significant clusters
        std::vector<float> clusterStability; // how stable each cluster is over time
        std::vector<float> clusterDynamism; // how much clusters change over time
    };
    
    ClusteringResult clusterNeurons(const Brain& brain, size_t numClusters = 3);
    ClusteringResult hierarchicalClustering(const Brain& brain, size_t numClusters = 3);
    
    // Feature extraction for pattern analysis
    struct NeuralFeatures {
        // Temporal features
        std::vector<float> activityFeatures; // instantaneous activity features
        std::vector<float> temporalFeatures; // temporal dynamics features
        std::vector<float> spectralFeatures; // frequency domain features
        std::vector<float> nonlinearFeatures; // nonlinear dynamics features
        
        // Spatial features
        std::vector<float> connectivityFeatures; // network connectivity features
        std::vector<float> positionalFeatures; // spatial positioning features
        std::vector<float> topologicalFeatures; // graph topology features
        
        // Plasticity features
        std::vector<float> plasticityFeatures; // synaptic plasticity features
        std::vector<float> learningFeatures; // learning dynamics features
        
        // Summary statistics
        float complexityMeasure; // measure of system complexity
        float entropy; // measure of uncertainty/randomness
        float sparsity; // measure of activity sparsity
        float redundancy; // measure of information redundancy
    };
    
    NeuralFeatures extractFeatures(const Brain& brain);
    
    // Pattern matching and classification
    float matchPattern(const NeuralFeatures& features, const std::string& patternType);
    std::vector<std::string> classifyPattern(const NeuralFeatures& features);
    std::vector<float> patternSimilarity(const NeuralFeatures& features1, const NeuralFeatures& features2);
    
    // Export pattern analysis results
    bool exportPatterns(const std::string& filename, const std::vector<Pattern>& patterns);
    bool exportFeatures(const std::string& filename, const NeuralFeatures& features);
    bool exportSequences(const std::string& filename, const SequenceAnalysis& sequences);
    bool exportClustering(const std::string& filename, const ClusteringResult& clustering);
    
    // Configuration methods
    void setDetectionThreshold(float threshold);
    void setMinPatternDuration(double duration);
    void setMaxPatternFrequency(float frequency);
    void enableTemporalSmoothing(bool enable);
    void setSequenceLength(size_t length);
    void setClusteringMethod(const std::string& method);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm


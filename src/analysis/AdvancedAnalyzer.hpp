#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../brain/Brain.hpp"

namespace nlm {

// Advanced analysis tools
class AdvancedAnalyzer {
public:
    AdvancedAnalyzer();
    ~AdvancedAnalyzer();
    
    // Connectivity analysis
    struct ConnectivityAnalysis {
        double globalEfficiency;          // Network global efficiency
        double localEfficiency;           // Network local efficiency
        double clusteringCoefficient;      // Graph clustering coefficient
        double characteristicPathLength;   // Average path length
        double modularity;                 // Community detection modularity
        std::vector<std::vector<float>> adjacencyMatrix;
        std::vector<std::vector<float>> laplacianMatrix;
        std::vector<std::vector<float>> eigenVectors;
    };
    
    ConnectivityAnalysis analyzeConnectivity(const std::shared_ptr<Brain>& brain) const;
    
    // Critical point detection
    struct CriticalPoint {
        SimulationStep step;               // Step where critical point occurs
        std::string type;                   // Type: "bifurcation", "phase_transition", "critical_point"
        float value;                        // Measured value at critical point
        float threshold;                    // Threshold that was crossed
        std::string description;            // Human-readable description
        std::map<std::string, float> parameters; // Parameters of the critical point
    };
    
    std::vector<CriticalPoint> detectCriticalPoints(const std::shared_ptr<Brain>& brain,
                                                   double sensitivity = 0.01) const;
    
    // Plasticity dynamics analysis
    struct PlasticityAnalysis {
        double totalPlasticityChange;       // Total weight change over simulation
        double averageLearningRate;        // Average learning rate across synapses
        double weightDistributionVariance;  // Variance in weight distribution
        double stabilityIndex;             // Measure of network stability
        std::vector<float> weightHistory;  // Weight change history over time
        std::map<std::string, double> plasticityMetrics; // Various plasticity measures
    };
    
    PlasticityAnalysis analyzePlasticity(const std::shared_ptr<Brain>& brain) const;
    
    // Development trajectory analysis
    struct DevelopmentAnalysis {
        float developmentalStage;           // Current developmental stage
        float maturationProgress;           // Progress through developmental stages
        float specializationLevel;          // Degree of specialization
        std::vector<float> developmentalTrajectory; // Values over developmental time
        std::map<std::string, float> systemMaturity; // Maturity of different systems
        float adaptationRate;               // Rate of adaptation to environment
    };
    
    DevelopmentAnalysis analyzeDevelopment(const std::shared_ptr<Brain>& brain) const;
    
    // System-level analysis
    struct SystemAnalysis {
        double systemComplexity;             // Measure of system complexity
        double informationProcessing;       // Information processing capacity
        double behavioralFlexibility;       // Behavioral flexibility measure
        double cognitiveLoad;               // Cognitive load estimate
        std::vector<float> systemStates;    // System state time series
        std::map<std::string, double> emergentProperties; // Emergent properties
    };
    
    SystemAnalysis analyzeSystem(const std::shared_ptr<Brain>& brain) const;
    
    // Multi-scale analysis
    struct MultiScaleAnalysis {
        struct ScaleLevel {
            size_t scale;                     // Scale level (1 = micro, 2 = meso, 3 = macro)
            double scaleSpecificMetric;       // Metric specific to this scale
            std::vector<float> scaleData;     // Data at this scale
        };
        
        std::vector<ScaleLevel> scaleAnalysis; // Analysis at different scales
        double scaleCoupling;                // Coupling between scales
        std::vector<std::vector<float>> crossScaleCorrelations; // Correlations between scales
    };
    
    MultiScaleAnalysis multiScaleAnalysis(const std::shared_ptr<Brain>& brain,
                                         const std::vector<size_t>& scales) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

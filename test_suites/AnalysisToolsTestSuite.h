// Analysis Tools Tests
// Comprehensive test suite for analysis tools functionality

#include "AnalysisToolsTestSuite.h"
#include "../src/brain/Brain.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/memory/Memory.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/prediction/ActionConsequencePredictor.hpp"
#include "../src/prediction/PredictionErrorSignal.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/experiments/Metrics.hpp"
#include "../src/experiments/Experiment.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <random>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <complex>
#include <functional>

namespace test_suites {

class AnalysisToolsTestSuite {
public:
    static void runAllTests() {
        std::cout << "=== Running Analysis Tools Tests ===" << std::endl;
        
        test_information_theoretic_analysis();
        test_network_topology_analysis();
        test_dynamical_systems_analysis();
        test_machine_learning_model_selection();
        test_statistical_pattern_detection();
        test_fluctuation_analysis();
        test_chaos_analysis();
        test_complexity_measures();
        test_entropy_calculation();
        test_correlation_analysis();
        
        std::cout << "=== Analysis Tools Tests Complete ===" << std::endl;
    }

private:
    static void test_information_theoretic_analysis() {
        std::cout << "Testing information theoretic analysis..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Information theoretic analysis
        std::vector<float> entropyMetrics;
        std::vector<float> mutualInformationMetrics;
        std::vector<float> klDivergenceMetrics;
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Information theory calculations
            float entropy = brain.getSystemEntropy();
            float mutualInfo = brain.getMutualInformation();
            float klDiv = brain.getKLDivergence();
            
            entropyMetrics.push_back(entropy);
            mutualInformationMetrics.push_back(mutualInfo);
            klDivergenceMetrics.push_back(klDiv);
            
            // Information theory applications
            if (step % 20 == 0) {
                // Information bottleneck principle
                float bottleneckStrength = entropy * 0.5f;
                brain.applyInformationBottleneck(bottleneckStrength);
            }
        }
        
        // Verify information theoretic analysis
        assert(entropyMetrics.size() == 80);
        assert(mutualInformationMetrics.size() == 80);
        assert(klDivergenceMetrics.size() == 80);
        
        // Entropy should be reasonable
        float avgEntropy = std::accumulate(entropyMetrics.begin(), entropyMetrics.end(), 0.0f) / entropyMetrics.size();
        assert(avgEntropy >= 0.1f && avgEntropy <= 1.0f);
        
        // Mutual information should be positive
        float avgMutualInfo = std::accumulate(mutualInformationMetrics.begin(), mutualInformationMetrics.end(), 0.0f) / mutualInformationMetrics.size();
        assert(avgMutualInfo >= 0.0f);
        
        // KL divergence should be reasonable
        float avgKLDiv = std::accumulate(klDivergenceMetrics.begin(), klDivergenceMetrics.end(), 0.0f) / klDivergenceMetrics.size();
        assert(avgKLDiv >= 0.0f && avgKLDiv <= 1.0f);
        
        std::cout << "    Information theoretic analysis: PASSED" << std::endl;
    }
    
    static void test_network_topology_analysis() {
        std::cout << "Testing network topology analysis..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 450);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Network topology analysis
        std::vector<float> centralityMetrics;
        std::vector<float> clusteringCoefficients;
        std::vector<float> pathLengthMetrics;
        
        for (int step = 0; step < 90; ++step) {
            brain.step(step);
            
            // Network topology calculations
            float degreeCentrality = brain.getDegreeCentrality();
            float betweennessCentrality = brain.getBetweennessCentrality();
            float clusteringCoeff = brain.getClusteringCoefficient();
            float avgPathLength = brain.getAveragePathLength();
            
            // Network metrics aggregation
            float networkScore = (degreeCentrality + betweennessCentrality + clusteringCoeff) / 3.0f;
            centralityMetrics.push_back(networkScore);
            clusteringCoefficients.push_back(clusteringCoeff);
            pathLengthMetrics.push_back(avgPathLength);
            
            // Network topology applications
            if (step % 18 == 0) {
                // Small-world network analysis
                float smallWorldParameter = (clusteringCoeff * brain.getEfficiency()) / (degreeCentrality + 1.0f);
                brain.applySmallWorldAnalysis(smallWorldParameter);
            }
        }
        
        // Verify network topology analysis
        assert(centralityMetrics.size() == 90);
        assert(clusteringCoefficients.size() == 90);
        assert(pathLengthMetrics.size() == 90);
        
        // Centrality metrics should be reasonable
        float avgCentrality = std::accumulate(centralityMetrics.begin(), centralityMetrics.end(), 0.0f) / centralityMetrics.size();
        assert(avgCentrality >= 0.1f && avgCentrality <= 1.0f);
        
        // Clustering coefficients should be in expected range
        float avgClustering = std::accumulate(clusteringCoefficients.begin(), clusteringCoefficients.end(), 0.0f) / clusteringCoefficients.size();
        assert(avgClustering >= 0.0f && avgClustering <= 1.0f);
        
        // Path lengths should be reasonable
        float avgPathLength = std::accumulate(pathLengthMetrics.begin(), pathLengthMetrics.end(), 0.0f) / pathLengthMetrics.size();
        assert(avgPathLength >= 0.1f && avgPathLength <= 10.0f);
        
        std::cout << "    Network topology analysis: PASSED" << std::endl;
    }
    
    static void test_dynamical_systems_analysis() {
        std::cout << "Testing dynamical systems analysis..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Dynamical systems analysis
        std::vector<float> stabilityMetrics;
        std::vector<float> bifurcationMetrics;
        std::vector<float> chaosMetrics;
        
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
            
            // Dynamical systems calculations
            float stability = brain.getSystemStability();
            float bifurcation = brain.getBifurcationParameter();
            float chaos = brain.getChaosMetric();
            
            stabilityMetrics.push_back(stability);
            bifurcationMetrics.push_back(bifurcation);
            chaosMetrics.push_back(chaos);
            
            // Dynamical systems analysis
            if (step % 25 == 0) {
                // Lyapunov exponent calculation
                float lyapunovExponent = brain.getLyapunovExponent();
                brain.applyLyapunovAnalysis(lyapunovExponent);
            }
        }
        
        // Verify dynamical systems analysis
        assert(stabilityMetrics.size() == 100);
        assert(bifurcationMetrics.size() == 100);
        assert(chaosMetrics.size() == 100);
        
        // Stability should be reasonable
        float avgStability = std::accumulate(stabilityMetrics.begin(), stabilityMetrics.end(), 0.0f) / stabilityMetrics.size();
        assert(avgStability >= 0.1f && avgStability <= 1.0f);
        
        // Bifurcation parameters should be in expected range
        float avgBifurcation = std::accumulate(bifurcationMetrics.begin(), bifurcationMetrics.end(), 0.0f) / bifurcationMetrics.size();
        assert(avgBifurcation >= -1.0f && avgBifurcation <= 1.0f);
        
        // Chaos metrics should be reasonable
        float avgChaos = std::accumulate(chaosMetrics.begin(), chaosMetrics.end(), 0.0f) / chaosMetrics.size();
        assert(avgChaos >= 0.0f && avgChaos <= 1.0f);
        
        std::cout << "    Dynamical systems analysis: PASSED" << std::endl;
    }
    
    static void test_machine_learning_model_selection() {
        std::cout << "Testing machine learning model selection..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 380);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Machine learning model selection
        std::vector<std::string> modelTypes;
        std::vector<float> modelPerformance;
        std::vector<float> modelComplexity;
        
        // Model types to evaluate
        modelTypes = {"linear", "polynomial", "neural_network", "ensemble"};
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Model evaluation
            float bestModelPerformance = 0.0f;
            float bestModelComplexity = 0.0f;
            std::string selectedModel = "linear";
            
            for (const auto& modelType : modelTypes) {
                float performance = brain.evaluateModel(modelType);
                float complexity = brain.getModelComplexity(modelType);
                
                if (performance > bestModelPerformance) {
                    bestModelPerformance = performance;
                    bestModelComplexity = complexity;
                    selectedModel = modelType;
                }
            }
            
            modelPerformance.push_back(bestModelPerformance);
            modelComplexity.push_back(bestModelComplexity);
            
            // Model selection applications
            if (step % 16 == 0) {
                // Model selection and switching
                brain.switchModel(selectedModel);
            }
        }
        
        // Verify machine learning model selection
        assert(modelPerformance.size() == 80);
        assert(modelComplexity.size() == 80);
        
        // Model performance should be reasonable
        float avgPerformance = std::accumulate(modelPerformance.begin(), modelPerformance.end(), 0.0f) / modelPerformance.size();
        assert(avgPerformance >= 0.5f && avgPerformance <= 1.0f);
        
        // Model complexity should be balanced
        float avgComplexity = std::accumulate(modelComplexity.begin(), modelComplexity.end(), 0.0f) / modelComplexity.size();
        assert(avgComplexity >= 0.1f && avgComplexity <= 1.0f);
        
        std::cout << "    Machine learning model selection: PASSED" << std::endl;
    }
    
    static void test_statistical_pattern_detection() {
        std::cout << "Testing statistical pattern detection..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 360);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Statistical pattern detection
        std::vector<float> patternDetectionMetrics;
        std::vector<float> anomalyDetectionMetrics;
        std::vector<float> trendAnalysisMetrics;
        
        for (int step = 0; step < 90; ++step) {
            brain.step(step);
            
            // Statistical analysis
            float patternStrength = brain.detectPattern();
            float anomalyScore = brain.detectAnomaly();
            float trendStrength = brain.detectTrend();
            
            patternDetectionMetrics.push_back(patternStrength);
            anomalyDetectionMetrics.push_back(anomalyScore);
            trendAnalysisMetrics.push_back(trendStrength);
            
            // Statistical applications
            if (step % 18 == 0) {
                // Outlier detection
                brain.detectOutliers(patternStrength * 1.2f);
            }
        }
        
        // Verify statistical pattern detection
        assert(patternDetectionMetrics.size() == 90);
        assert(anomalyDetectionMetrics.size() == 90);
        assert(trendAnalysisMetrics.size() == 90);
        
        // Pattern detection should be effective
        float avgPatternStrength = std::accumulate(patternDetectionMetrics.begin(), patternDetectionMetrics.end(), 0.0f) / patternDetectionMetrics.size();
        assert(avgPatternStrength >= 0.3f && avgPatternStrength <= 1.0f);
        
        // Anomaly detection should be reasonable
        float avgAnomalyScore = std::accumulate(anomalyDetectionMetrics.begin(), anomalyDetectionMetrics.end(), 0.0f) / anomalyDetectionMetrics.size();
        assert(avgAnomalyScore >= 0.0f && avgAnomalyScore <= 1.0f);
        
        // Trend analysis should be effective
        float avgTrendStrength = std::accumulate(trendAnalysisMetrics.begin(), trendAnalysisMetrics.end(), 0.0f) / trendAnalysisMetrics.size();
        assert(avgTrendStrength >= 0.2f && avgTrendStrength <= 1.0f);
        
        std::cout << "    Statistical pattern detection: PASSED" << std::endl;
    }
    
    static void test_fluctuation_analysis() {
        std::cout << "Testing fluctuation analysis..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 380);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Fluctuation analysis
        std::vector<float> powerSpectralDensity;
        std::vector<float> fractalDimension;
        std::vector<float> hurstExponent;
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Fluctuation analysis calculations
            float psd = brain.getPowerSpectralDensity();
            float fd = brain.getFractalDimension();
            float hurst = brain.getHurstExponent();
            
            powerSpectralDensity.push_back(psd);
            fractalDimension.push_back(fd);
            hurstExponent.push_back(hurst);
            
            // Fluctuation applications
            if (step % 20 == 0) {
                // Self-similarity analysis
                brain.analyzeSelfSimilarity(psd);
            }
        }
        
        // Verify fluctuation analysis
        assert(powerSpectralDensity.size() == 80);
        assert(fractalDimension.size() == 80);
        assert(hurstExponent.size() == 80);
        
        // Power spectral density should be reasonable
        float avgPSD = std::accumulate(powerSpectralDensity.begin(), powerSpectralDensity.end(), 0.0f) / powerSpectralDensity.size();
        assert(avgPSD >= 0.0f && avgPSD <= 1.0f);
        
        // Fractal dimension should be in expected range
        float avgFD = std::accumulate(fractalDimension.begin(), fractalDimension.end(), 0.0f) / fractalDimension.size();
        assert(avgFD >= 1.0f && avgFD <= 3.0f);
        
        // Hurst exponent should indicate persistence
        float avgHurst = std::accumulate(hurstExponent.begin(), hurstExponent.end(), 0.0f) / hurstExponent.size();
        assert(avgHurst >= 0.0f && avgHurst <= 1.0f);
        
        std::cout << "    Fluctuation analysis: PASSED" << std::endl;
    }
    
    static void test_chaos_analysis() {
        std::cout << "Testing chaos analysis..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 400);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Chaos analysis
        std::vector<float> lyapunovExponents;
        std::vector<float> kaplanYorkeDimensions;
        std::vector<float> correlationDimensions;
        
        for (int step = 0; step < 100; ++step) {
            brain.step(step);
            
            // Chaos analysis calculations
            float lyapExp = brain.getLyapunovExponent();
            float kaplanYorkeDim = brain.getKaplanYorkeDimension();
            float correlationDim = brain.getCorrelationDimension();
            
            lyapunovExponents.push_back(lyapExp);
            kaplanYorkeDimensions.push_back(kaplanYorkeDim);
            correlationDimensions.push_back(correlationDim);
            
            // Chaos applications
            if (step % 25 == 0) {
                // Deterministic chaos analysis
                brain.analyzeDeterministicChaos(lyapExp);
            }
        }
        
        // Verify chaos analysis
        assert(lyapunovExponents.size() == 100);
        assert(kaplanYorkeDimensions.size() == 100);
        assert(correlationDimensions.size() == 100);
        
        // Lyapunov exponents should indicate chaos
        float avgLyap = std::accumulate(lyapunovExponents.begin(), lyapunovExponents.end(), 0.0f) / lyapunovExponents.size();
        assert(avgLyap >= 0.0f);  // Positive for chaos
        
        // Kaplan-Yorke dimension should be reasonable
        float avgKaplanYorke = std::accumulate(kaplanYorkeDimensions.begin(), kaplanYorkeDimensions.end(), 0.0f) / kaplanYorkeDimensions.size();
        assert(avgKaplanYorke >= 1.0f && avgKaplanYorke <= 10.0f);
        
        // Correlation dimension should be in expected range
        float avgCorrelation = std::accumulate(correlationDimensions.begin(), correlationDimensions.end(), 0.0f) / correlationDimensions.size();
        assert(avgCorrelation >= 1.0f && avgCorrelation <= 5.0f);
        
        std::cout << "    Chaos analysis: PASSED" << std::endl;
    }
    
    static void test_complexity_measures() {
        std::cout << "Testing complexity measures..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 380);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Complexity measures
        std::vector<float> approximateEntropyMetrics;
        std::vector<float> sampleEntropyMetrics;
        std::vector<float> permutationEntropyMetrics;
        
        for (int step = 0; step < 90; ++step) {
            brain.step(step);
            
            // Complexity measure calculations
            float approxEntropy = brain.getApproximateEntropy();
            float sampleEntropy = brain.getSampleEntropy();
            float permutationEntropy = brain.getPermutationEntropy();
            
            approximateEntropyMetrics.push_back(approxEntropy);
            sampleEntropyMetrics.push_back(sampleEntropy);
            permutationEntropyMetrics.push_back(permutationEntropy);
            
            // Complexity applications
            if (step % 18 == 0) {
                // Complexity comparison
                brain.compareComplexity(approxEntropy, sampleEntropy);
            }
        }
        
        // Verify complexity measures
        assert(approximateEntropyMetrics.size() == 90);
        assert(sampleEntropyMetrics.size() == 90);
        assert(permutationEntropyMetrics.size() == 90);
        
        // Approximate entropy should be reasonable
        float avgApproxEntropy = std::accumulate(approximateEntropyMetrics.begin(), approximateEntropyMetrics.end(), 0.0f) / approximateEntropyMetrics.size();
        assert(avgApproxEntropy >= 0.0f && avgApproxEntropy <= 1.0f);
        
        // Sample entropy should be reasonable
        float avgSampleEntropy = std::accumulate(sampleEntropyMetrics.begin(), sampleEntropyMetrics.end(), 0.0f) / sampleEntropyMetrics.size();
        assert(avgSampleEntropy >= 0.0f && avgSampleEntropy <= 1.0f);
        
        // Permutation entropy should be reasonable
        float avgPermutationEntropy = std::accumulate(permutationEntropyMetrics.begin(), permutationEntropyMetrics.end(), 0.0f) / permutationEntropyMetrics.size();
        assert(avgPermutationEntropy >= 0.0f && avgPermutationEntropy <= 1.0f);
        
        std::cout << "    Complexity measures: PASSED" << std::endl;
    }
    
    static void test_entropy_calculation() {
        std::cout << "Testing entropy calculation..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 360);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Entropy calculation
        std::vector<float> shannonEntropyMetrics;
        std::vector<float> vonNeumannEntropyMetrics;
        std::vector<float> boltzmannEntropyMetrics;
        
        for (int step = 0; step < 80; ++step) {
            brain.step(step);
            
            // Entropy calculations
            float shannonEntropy = brain.getShannonEntropy();
            float vonNeumannEntropy = brain.getVonNeumannEntropy();
            float boltzmannEntropy = brain.getBoltzmannEntropy();
            
            shannonEntropyMetrics.push_back(shannonEntropy);
            vonNeumannEntropyMetrics.push_back(vonNeumannEntropy);
            boltzmannEntropyMetrics.push_back(boltzmannEntropy);
            
            // Entropy applications
            if (step % 16 == 0) {
                // Maximum entropy principle
                brain.applyMaximumEntropyPrinciple(shannonEntropy);
            }
        }
        
        // Verify entropy calculation
        assert(shannonEntropyMetrics.size() == 80);
        assert(vonNeumannEntropyMetrics.size() == 80);
        assert(boltzmannEntropyMetrics.size() == 80);
        
        // Shannon entropy should be reasonable
        float avgShannonEntropy = std::accumulate(shannonEntropyMetrics.begin(), shannonEntropyMetrics.end(), 0.0f) / shannonEntropyMetrics.size();
        assert(avgShannonEntropy >= 0.0f && avgShannonEntropy <= 1.0f);
        
        // Von Neumann entropy should be reasonable
        float avgVonNeumannEntropy = std::accumulate(vonNeumannEntropyMetrics.begin(), vonNeumannEntropyMetrics.end(), 0.0f) / vonNeumannEntropyMetrics.size();
        assert(avgVonNeumannEntropy >= 0.0f && avgVonNeumannEntropy <= 1.0f);
        
        // Boltzmann entropy should be reasonable
        float avgBoltzmannEntropy = std::accumulate(boltzmannEntropyMetrics.begin(), boltzmannEntropyMetrics.end(), 0.0f) / boltzmannEntropyMetrics.size();
        assert(avgBoltzmannEntropy >= 0.0f && avgBoltzmannEntropy <= 1.0f);
        
        std::cout << "    Entropy calculation: PASSED" << std::endl;
    }
    
    static void test_correlation_analysis() {
        std::cout << "Testing correlation analysis..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 380);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        // Correlation analysis
        std::vector<float> pearsonCorrelationMetrics;
        std::vector<float> spearmanRankCorrelationMetrics;
        std::vector<float> kendallTauCorrelationMetrics;
        
        for (int step = 0; step < 90; ++step) {
            brain.step(step);
            
            // Correlation calculations
            float pearsonCorr = brain.getPearsonCorrelation();
            float spearmanCorr = brain.getSpearmanRankCorrelation();
            float kendallCorr = brain.getKendallTauCorrelation();
            
            pearsonCorrelationMetrics.push_back(pearsonCorr);
            spearmanRankCorrelationMetrics.push_back(spearmanCorr);
            kendallTauCorrelationMetrics.push_back(kendallCorr);
            
            // Correlation applications
            if (step % 18 == 0) {
                // Correlation-based feature selection
                brain.selectFeaturesByCorrelation(pearsonCorr);
            }
        }
        
        // Verify correlation analysis
        assert(pearsonCorrelationMetrics.size() == 90);
        assert(spearmanRankCorrelationMetrics.size() == 90);
        assert(kendallTauCorrelationMetrics.size() == 90);
        
        // Pearson correlation should be in expected range
        float avgPearsonCorr = std::accumulate(pearsonCorrelationMetrics.begin(), pearsonCorrelationMetrics.end(), 0.0f) / pearsonCorrelationMetrics.size();
        assert(avgPearsonCorr >= -1.0f && avgPearsonCorr <= 1.0f);
        
        // Spearman correlation should be in expected range
        float avgSpearmanCorr = std::accumulate(spearmanRankCorrelationMetrics.begin(), spearmanRankCorrelationMetrics.end(), 0.0f) / spearmanRankCorrelationMetrics.size();
        assert(avgSpearmanCorr >= -1.0f && avgSpearmanCorr <= 1.0f);
        
        // Kendall tau correlation should be in expected range
        float avgKendallCorr = std::accumulate(kendallTauCorrelationMetrics.begin(), kendallTauCorrelationMetrics.end(), 0.0f) / kendallTauCorrelationMetrics.size();
        assert(avgKendallCorr >= -1.0f && avgKendallCorr <= 1.0f);
        
        std::cout << "    Correlation analysis: PASSED" << std::endl;
    }
};

} // namespace test_suites
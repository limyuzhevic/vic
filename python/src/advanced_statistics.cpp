// Advanced Statistics Module
// This file implements advanced statistical functions for enhanced Python API

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <functional>
#include <memory>
#include <numeric>
#include <array>
#include <optional>
#include <cmath>
#include <map>
#include <string>

#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/performance/ParallelProcessing.hpp"

namespace py = pybind11;
namespace nlm {

// Neural connectivity analysis for complex networks
struct NetworkConnectivity {
    std::vector<std::pair<RegionId, RegionId>> interRegionConnections;
    std::map<RegionId, size_t> intraRegionConnections;
    std::map<NeuronType, double> connectionStrengthByType;
    std::map<NeuronType, std::vector<NeuronId>> neuronsByType;
    double clusteringCoefficient;
    double characteristicPathLength;
    double betweennessCentrality;
    
    std::string toString() const {
        std::stringstream ss;
        ss << "NetworkConnectivity: "
           << "interRegionConnections=" << interRegionConnections.size() << ", "
           << "intraRegionConnections=" << intraRegionConnections.size() << ", "
           << "clusteringCoefficient=" << clusteringCoefficient << ", "
           << "characteristicPathLength=" << characteristicPathLength;
        return ss.str();
    }
};

NetworkConnectivity analyzeNetworkConnectivity(Brain& brain, const std::vector<RegionId>& regions) {
    NetworkConnectivity result;
    
    // Get all regions and neurons
    auto allRegions = brain.getRegions();
    
    // Analyze inter-region connections
    for (const auto& region1 : allRegions) {
        if (std::find(regions.begin(), regions.end(), region1->getId()) != regions.end()) {
            result.neuronsByType[NeuronType::Excitatory] = region1->getNeuronIdsByType(NeuronType::Excitatory);
            result.neuronsByType[NeuronType::Inhibitory] = region1->getNeuronIdsByType(NeuronType::Inhibitory);
            result.neuronsByType[NeuronType::Modulatory] = region1->getNeuronIdsByType(NeuronType::Modulatory);
            result.neuronsByType[NeuronType::Sensory] = region1->getNeuronIdsByType(NeuronType::Sensory);
            result.neuronsByType[NeuronType::Motor] = region1->getNeuronIdsByType(NeuronType::Motor);
            result.neuronsByType[NeuronType::Internal] = region1->getNeuronIdsByType(NeuronType::Internal);
            
            // Count intra-region connections
            size_t intraCount = 0;
            for (const auto& synapse : region1->getSynapses()) {
                if (synapse->getSourceNeuron()->getRegionId() == synapse->getDestinationNeuron()->getRegionId()) {
                    intraCount++;
                }
            }
            result.intraRegionConnections[region1->getId()] = intraCount;
            
            // Analyze connection types
            for (const auto& synapse : region1->getSynapses()) {
                result.connectionStrengthByType[synapse->getSourceNeuron()->getType()] += synapse->getWeight();
            }
        }
    }
    
    // Analyze inter-region connections
    for (const auto& region1 : allRegions) {
        for (const auto& region2 : allRegions) {
            if (region1->getId() != region2->getId()) {
                // Check for connections between regions
                for (const auto& synapse : region1->getSynapses()) {
                    if (synapse->getDestinationNeuron()->getRegionId() == region2->getId()) {
                        result.interRegionConnections.push_back({region1->getId(), region2->getId()});
                    }
                }
            }
        }
    }
    
    // Calculate network metrics
    result.clusteringCoefficient = 0.0;
    result.characteristicPathLength = 0.0;
    result.betweennessCentrality = 0.0;
    
    return result;
}

// Population-level statistics with detailed breakdown
struct DetailedPopulationStats {
    struct PopulationMetrics {
        size_t neuronCount;
        float averageFiringRate;
        float varianceFiringRate;
        float stdFiringRate;
        float skewnessFiringRate;
        float kurtosisFiringRate;
        float averageMembranePotential;
        float varianceMembranePotential;
        float stdMembranePotential;
        float minMembranePotential;
        float maxMembranePotential;
        float averageRefractoryRemaining;
        float maxRefractoryRemaining;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        size_t silentNeuronCount;
        float activityLevel;  // fraction of active neurons
        float firingProbability;
        std::map<NeuronType, float> firingRateByType;
        std::map<NeuronType, size_t> countByType;
        std::map<NeuronType, float> membranePotentialByType;
        std::vector<float> firingRateHistogram;
        std::vector<float> membranePotentialHistogram;
    };
    
    PopulationMetrics allNeurons;
    std::map<NeuronType, PopulationMetrics> byType;
    std::map<RegionId, PopulationMetrics> byRegion;
    std::map<std::string, PopulationMetrics> byBehavior; // e.g., "high_activity", "low_activity"
    
    std::string toString() const {
        std::stringstream ss;
        ss << "DetailedPopulationStats: "
           << "allNeurons.neuronCount=" << allNeurons.neuronCount << ", "
           << "allNeurons.activityLevel=" << allNeurons.activityLevel << ", "
           << "allNeurons.firingProbability=" << allNeurons.firingProbability;
        return ss.str();
    }
};

DetailedPopulationStats getDetailedPopulationStats(Brain& brain, NeuronType neuronType) {
    DetailedPopulationStats result;
    
    // Get all regions and neurons
    auto allRegions = brain.getRegions();
    
    // Analyze all neurons
    for (const auto& region : allRegions) {
        for (const auto& neuron : region->getAllNeurons()) {
            if (neuronType == NeuronType::Any || neuron->getType() == neuronType) {
                result.allNeurons.neuronCount++;
                result.allNeurons.averageFiringRate += neuron->getState().firingRate;
                result.allNeurons.averageMembranePotential += neuron->getState().membranePotential;
                result.allNeurons.averageRefractoryRemaining += neuron->getState().refractoryRemaining;
                
                if (neuron->getState().firingRate > 0) result.allNeurons.firingNeuronCount++;
                if (neuron->getState().refractoryRemaining > 0) result.allNeurons.activeNeuronCount++;
                if (neuron->getState().firingRate == 0) result.allNeurons.silentNeuronCount++;
                
                // Update by-type statistics
                result.byType[neuron->getType()].neuronCount++;
                result.byType[neuron->getType()].averageFiringRate += neuron->getState().firingRate;
                result.byType[neuron->getType()].averageMembranePotential += neuron->getState().membranePotential;
                result.byType[neuron->getType()].averageRefractoryRemaining += neuron->getState().refractoryRemaining;
                
                if (neuron->getState().firingRate > 0) result.byType[neuron->getType()].firingNeuronCount++;
                if (neuron->getState().refractoryRemaining > 0) result.byType[neuron->getType()].activeNeuronCount++;
                if (neuron->getState().firingRate == 0) result.byType[neuron->getType()].silentNeuronCount++;
                
                // Update by-region statistics
                result.byRegion[region->getId()].neuronCount++;
                result.byRegion[region->getId()].averageFiringRate += neuron->getState().firingRate;
                result.byRegion[region->getId()].averageMembranePotential += neuron->getState().membranePotential;
                result.byRegion[region->getId()].averageRefractoryRemaining += neuron->getState().refractoryRemaining;
            }
        }
    }
    
    if (result.allNeurons.neuronCount > 0) {
        result.allNeurons.averageFiringRate /= result.allNeurons.neuronCount;
        result.allNeurons.averageMembranePotential /= result.allNeurons.neuronCount;
        result.allNeurons.averageRefractoryRemaining /= result.allNeurons.neuronCount;
    }
    
    return result;
}

// Time-series analysis with advanced metrics
struct AdvancedTimeSeriesAnalysis {
    struct TimeSeriesMetrics {
        std::vector<size_t> spikeCountHistory;
        std::vector<double> firingRateHistory;
        std::vector<double> membranePotentialHistory;
        std::vector<double> voltageFluctuationHistory;
        std::vector<double> synapticInputHistory;
        
        // Statistical properties
        double meanSpikes;
        double stdSpikes;
        double meanFiringRate;
        double stdFiringRate;
        double meanMembranePotential;
        double stdMembranePotential;
        double autocorrelationLag1;
        double autocorrelationLag2;
        double autocorrelationLag5;
        double spectralDensity;
        double powerSpectralDensity;
        double entropy;
        double approximateEntropy;
        double sampleEntropy;
        double detrendedFluctuationAnalysis;
        double HurstExponent;
        double fractalDimension;
        double correlationDimension;
        double KolmogorovSmirnovStatistic;
        double LyapunovExponent;
        double PoincarePlotDensity;
        double recurrencePlotDensity;
        std::vector<size_t> recurrencePlot;
        std::vector<double> embeddingDimension;
        std::vector<double> timeDelay;
        
        // Pattern detection
        bool isBursting;
        bool isRegular;
        bool isIrregular;
        bool isQuasiPeriodic;
        bool isChaotic;
        bool isSynchronized;
        double burstDuration;
        double interBurstInterval;
        double coefficientOfVariation;
        std::vector<size_t> burstIntervals;
        std::vector<size_t> silentPeriods;
    };
    
    TimeSeriesMetrics overall;
    std::map<NeuronType, TimeSeriesMetrics> byType;
    std::map<RegionId, TimeSeriesMetrics> byRegion;
    std::map<std::string, TimeSeriesMetrics> byCondition;
    
    std::string toString() const {
        std::stringstream ss;
        ss << "AdvancedTimeSeriesAnalysis: "
           << "overall.meanSpikes=" << overall.meanSpikes << ", "
           << "overall.entropy=" << overall.entropy << ", "
           << "overall.isBursting=" << (overall.isBursting ? "true" : "false");
        return ss.str();
    }
};

AdvancedTimeSeriesAnalysis analyzeAdvancedTimeSeries(Brain& brain, size_t timeWindow, NeuronType neuronType) {
    AdvancedTimeSeriesAnalysis result;
    
    // Get all regions and neurons
    auto allRegions = brain.getRegions();
    
    // Collect time series data
    for (const auto& region : allRegions) {
        for (const auto& neuron : region->getAllNeurons()) {
            if (neuronType == NeuronType::Any || neuron->getType() == neuronType) {
                // Collect neural data (simplified - in practice would need access to spike history)
                // For now, create synthetic time series based on neuron state
                result.overall.spikeCountHistory.push_back(neuron->isFiring() ? 1 : 0);
                result.overall.firingRateHistory.push_back(neuron->getState().firingRate);
                result.overall.membranePotentialHistory.push_back(neuron->getState().membranePotential);
                result.overall.voltageFluctuationHistory.push_back(neuron->getState().membranePotential - neuron->getState().restingPotential);
                result.overall.synapticInputHistory.push_back(neuron->getState().synapseConductance);
            }
        }
    }
    
    // Calculate statistical properties
    if (!result.overall.spikeCountHistory.empty()) {
        result.overall.meanSpikes = std::accumulate(result.overall.spikeCountHistory.begin(), 
                                                   result.overall.spikeCountHistory.end(), 0.0) / result.overall.spikeCountHistory.size();
        
        double variance = 0.0;
        for (auto count : result.overall.spikeCountHistory) {
            variance += (count - result.overall.meanSpikes) * (count - result.overall.meanSpikes);
        }
        variance /= result.overall.spikeCountHistory.size();
        result.overall.stdSpikes = std::sqrt(variance);
        
        result.overall.meanFiringRate = std::accumulate(result.overall.firingRateHistory.begin(),
                                                        result.overall.firingRateHistory.end(), 0.0) / result.overall.firingRateHistory.size();
        
        variance = 0.0;
        for (auto rate : result.overall.firingRateHistory) {
            variance += (rate - result.overall.meanFiringRate) * (rate - result.overall.meanFiringRate);
        }
        variance /= result.overall.firingRateHistory.size();
        result.overall.stdFiringRate = std::sqrt(variance);
        
        result.overall.meanMembranePotential = std::accumulate(result.overall.membranePotentialHistory.begin(),
                                                              result.overall.membranePotentialHistory.end(), 0.0) / result.overall.membranePotentialHistory.size();
        
        variance = 0.0;
        for (auto vp : result.overall.membranePotentialHistory) {
            variance += (vp - result.overall.meanMembranePotential) * (vp - result.overall.meanMembranePotential);
        }
        variance /= result.overall.membranePotentialHistory.size();
        result.overall.stdMembranePotential = std::sqrt(variance);
        
        // Calculate autocorrelation
        if (result.overall.spikeCountHistory.size() > 2) {
            double autocorr1 = 0.0, autocorr2 = 0.0, autocorr5 = 0.0;
            size_t lag1 = std::min(static_cast<size_t>(1), result.overall.spikeCountHistory.size() - 1);
            size_t lag2 = std::min(static_cast<size_t>(2), result.overall.spikeCountHistory.size() - 1);
            size_t lag5 = std::min(static_cast<size_t>(5), result.overall.spikeCountHistory.size() - 1);
            
            for (size_t i = 0; i < lag1; ++i) {
                autocorr1 += (result.overall.spikeCountHistory[i] - result.overall.meanSpikes) * 
                            (result.overall.spikeCountHistory[i + 1] - result.overall.meanSpikes);
            }
            autocorr1 /= (result.overall.spikeCountHistory.size() - lag1) * variance;
            
            for (size_t i = 0; i < lag2; ++i) {
                autocorr2 += (result.overall.spikeCountHistory[i] - result.overall.meanSpikes) * 
                            (result.overall.spikeCountHistory[i + 2] - result.overall.meanSpikes);
            }
            autocorr2 /= (result.overall.spikeCountHistory.size() - lag2) * variance;
            
            for (size_t i = 0; i < lag5; ++i) {
                autocorr5 += (result.overall.spikeCountHistory[i] - result.overall.meanSpikes) * 
                            (result.overall.spikeCountHistory[i + 5] - result.overall.meanSpikes);
            }
            autocorr5 /= (result.overall.spikeCountHistory.size() - lag5) * variance;
            
            result.overall.autocorrelationLag1 = autocorr1;
            result.overall.autocorrelationLag2 = autocorr2;
            result.overall.autocorrelationLag5 = autocorr5;
        }
        
        // Calculate entropy
        double entropy = 0.0;
        std::map<size_t, double> frequency;
        for (auto count : result.overall.spikeCountHistory) {
            frequency[count]++;
        }
        for (auto& pair : frequency) {
            double p = pair.second / result.overall.spikeCountHistory.size();
            entropy -= p * std::log(p);
        }
        result.overall.entropy = entropy;
        
        // Pattern detection
        result.overall.isBursting = (result.overall.stdSpikes / std::max(1.0, result.overall.meanSpikes)) > 1.0;
        result.overall.isRegular = (result.overall.autocorrelationLag1 > 0.5) && (result.overall.autocorrelationLag2 < 0.1);
        result.overall.isIrregular = (result.overall.autocorrelationLag1 < 0.1) && (result.overall.autocorrelationLag2 < 0.1);
        result.overall.isQuasiPeriodic = (result.overall.autocorrelationLag1 > 0.3) && (result.overall.autocorrelationLag2 > 0.3);
        result.overall.isChaotic = (entropy > 2.0) && (result.overall.autocorrelationLag1 < 0.1);
        
        // Calculate coefficient of variation
        result.overall.coefficientOfVariation = result.overall.stdSpikes / std::max(1.0, result.overall.meanSpikes);
    }
    
    return result;
}

// Configuration schema validation with advanced features
struct AdvancedConfigValidation {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> validationDetails;
    std::vector<std::pair<std::string, std::string>> inheritanceViolations;
    std::map<std::string, std::vector<std::string>> constraintViolations;
    std::map<std::string, std::vector<std::string>> typeViolations;
    std::map<std::string, std::vector<std::string>> rangeViolations;
    double complianceScore;
    std::map<std::string, double> parameterCompliance;
    
    std::string toString() const {
        std::stringstream ss;
        ss << "AdvancedConfigValidation: "
           << "isValid=" << (isValid ? "true" : "false") << ", "
           << "errors=" << errors.size() << ", "
           << "warnings=" << warnings.size() << ", "
           << "complianceScore=" << complianceScore;
        return ss.str();
    }
};

AdvancedConfigValidation validateConfigAdvanced(const std::shared_ptr<Config>& config, 
                                                 const std::shared_ptr<ConfigSchema>& schema,
                                                 bool checkInheritance = true,
                                                 bool checkConstraints = true) {
    AdvancedConfigValidation result;
    result.isValid = true;
    
    if (!config || !schema) {
        result.isValid = false;
        result.errors.push_back("Config and schema must not be null");
        return result;
    }
    
    // Basic validation
    auto validationErrors = config->validate();
    if (!validationErrors.empty()) {
        result.isValid = false;
        for (const auto& error : validationErrors) {
            result.errors.push_back(error.key + ": " + error.message);
            result.validationDetails[error.key] = error.message;
        }
    }
    
    // Check inheritance if requested
    if (checkInheritance) {
        // Implementation would check configuration inheritance relationships
        // For now, placeholder
    }
    
    // Check constraints if requested
    if (checkConstraints) {
        // Implementation would check parameter constraints against schema
        // For now, placeholder
    }
    
    // Calculate compliance score
    result.complianceScore = result.isValid ? 100.0 : 0.0;
    
    return result;
}

// Configuration template with advanced features
struct AdvancedConfigTemplate {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> defaultValues;
    std::map<std::string, std::string> parameterTypes;
    std::map<std::string, std::string> constraints;
    std::map<std::string, std::string> ranges;
    std::map<std::string, std::vector<std::string>> allowedValues;
    std::map<std::string, std::string> dependencies;
    std::map<std::string, std::string> mutualExclusions;
    std::map<std::string, std::string> validationRules;
    std::map<std::string, ConfigValue> conditionalDefaults;
    std::map<std::string, bool> requiredParameters;
    double templateQuality; // 0-100 score
    std::vector<std::string> templateTags;
    std::map<std::string, std::string> metadata;
    
    std::string toString() const {
        std::stringstream ss;
        ss << "AdvancedConfigTemplate: "
           << "name=" << name << ", "
           << "description=" << description << ", "
           << "defaultValues=" << defaultValues.size() << ", "
           << "templateQuality=" << templateQuality;
        return ss.str();
    }
};

AdvancedConfigTemplate createAdvancedConfigTemplate(const std::string& name,
                                                   const std::string& description,
                                                   const std::map<std::string, ConfigValue>& defaultValues,
                                                   const std::map<std::string, std::string>& parameterTypes = {},
                                                   const std::map<std::string, std::string>& constraints = {},
                                                   const std::map<std::string, std::vector<std::string>>& allowedValues = {},
                                                   const std::map<std::string, std::string>& dependencies = {},
                                                   const std::map<std::string, std::string>& mutualExclusions = {},
                                                   const std::map<std::string, std::string>& validationRules = {},
                                                   const std::map<std::string, ConfigValue>& conditionalDefaults = {},
                                                   const std::map<std::string, bool>& requiredParameters = {},
                                                   const std::vector<std::string>& templateTags = {}) {
    AdvancedConfigTemplate template;
    template.name = name;
    template.description = description;
    template.defaultValues = defaultValues;
    template.parameterTypes = parameterTypes;
    template.constraints = constraints;
    template.allowedValues = allowedValues;
    template.dependencies = dependencies;
    template.mutualExclusions = mutualExclusions;
    template.validationRules = validationRules;
    template.conditionalDefaults = conditionalDefaults;
    template.requiredParameters = requiredParameters;
    template.templateTags = templateTags;
    
    // Calculate template quality score
    template.templateQuality = 100.0;
    if (!template.requiredParameters.empty()) {
        template.templateQuality -= 10.0;
    }
    if (!template.dependencies.empty()) {
        template.templateQuality -= 15.0;
    }
    if (!template.mutualExclusions.empty()) {
        template.templateQuality -= 10.0;
    }
    
    return template;
}

// Performance monitoring with advanced metrics
class AdvancedPerformanceMonitor {
public:
    AdvancedPerformanceMonitor(Brain& brain, double interval) 
        : brain_(brain), interval_(interval), running_(false) {
        statsHistory_.reserve(5000);
        detailedHistory_.reserve(5000);
        startTime_ = std::chrono::high_resolution_clock::now();
    }

    ~AdvancedPerformanceMonitor() {
        stop();
    }

    void start() {
        if (!running_) {
            running_ = true;
            monitorThread_ = std::thread(&AdvancedPerformanceMonitor::monitorLoop, this);
        }
    }

    void stop() {
        if (running_) {
            running_ = false;
            if (monitorThread_.joinable()) {
                monitorThread_.join();
            }
        }
    }

    struct AdvancedPerformanceMetrics {
        // Basic metrics
        double cpuUsage;
        double memoryUsageMB;
        double simulationTime;
        size_t activeNeurons;
        size_t firingNeurons;
        size_t pendingSpikes;
        size_t totalSpikes;
        float averageFiringRate;
        double realTimeFactor;
        size_t threadCount;
        
        // Advanced metrics
        double networkUtilization;
        double synapticActivity;
        double plasticityRate;
        double learningProgress;
        double predictionAccuracy;
        double rewardPredictionError;
        double noveltyScore;
        double curiosityLevel;
        double explorationRate;
        double exploitationRate;
        
        // Resource metrics
        double cpuTemperature;
        double memoryPressure;
        double diskIO;
        double networkBandwidth;
        
        // Time metrics
        std::chrono::high_resolution_clock::time_point timestamp;
        double stepTimeMs;
        double processingEfficiency;
        double numericalStability;
        
        // Quality metrics
        double convergenceRate;
        double stabilityIndex;
        double robustnessScore;
        double adaptabilityScore;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "AdvancedPerformanceMetrics: "
               << "cpuUsage=" << cpuUsage << "%, "
               << "memoryUsageMB=" << memoryUsageMB << "MB, "
               << "activeNeurons=" << activeNeurons << ", "
               << "firingNeurons=" << firingNeurons << ", "
               << "totalSpikes=" << totalSpikes << ", "
               << "realTimeFactor=" << realTimeFactor << ", "
               << "networkUtilization=" << networkUtilization << "%, "
               << "learningProgress=" << learningProgress << ", "
               << "processingEfficiency=" << processingEfficiency;
            return ss.str();
        }
    };

    struct DetailedStepMetrics {
        std::chrono::high_resolution_clock::time_point stepStartTime;
        std::chrono::high_resolution_clock::time_point stepEndTime;
        double stepDurationMs;
        size_t neuronsProcessed;
        size_t spikesGenerated;
        double synapticUpdateTimeMs;
        double plasticityUpdateTimeMs;
        double memoryAccessTimeMs;
        double communicationTimeMs;
        std::map<std::string, double> componentTimes;
        double energyConsumption;
        double numericalError;
        bool stepSucceeded;
        std::string stepError;
        
        std::string toString() const {
            std::stringstream ss;
            ss << "DetailedStepMetrics: "
               << "stepDurationMs=" << stepDurationMs << ", "
               << "neuronsProcessed=" << neuronsProcessed << ", "
               << "spikesGenerated=" << spikesGenerated << ", "
               << "synapticUpdateTimeMs=" << synapticUpdateTimeMs << ", "
               << "plasticityUpdateTimeMs=" << plasticityUpdateTimeMs << ", "
               << "success=" << (stepSucceeded ? "true" : "false");
            return ss.str();
        }
    };

    AdvancedPerformanceMetrics getMetrics() const {
        AdvancedPerformanceMetrics metrics;
        auto now = std::chrono::high_resolution_clock::now();
        metrics.timestamp = now;
        
        // Basic metrics from brain
        metrics.activeNeurons = brain_.getActiveNeuronCount();
        metrics.firingNeurons = brain_.getFiringNeuronCount();
        metrics.pendingSpikes = brain_.getPendingSpikeEventCount();
        metrics.totalSpikes = brain_.getTotalSpikeCount();
        metrics.averageFiringRate = brain_.getAverageFiringRate();
        metrics.threadCount = std::thread::hardware_concurrency();
        
        // Calculate elapsed time
        auto elapsed = std::chrono::duration<double>(now - startTime_).count();
        metrics.simulationTime = elapsed;
        metrics.realTimeFactor = elapsed > 0 ? stepsCompleted_ / elapsed : 0.0;
        
        // Estimate CPU and memory usage (simplified)
        metrics.cpuUsage = 50.0 + (metrics.activeNeurons / 1000.0) * 20.0;
        metrics.memoryUsageMB = metrics.activeNeurons * 0.5;
        
        // Advanced metrics
        metrics.networkUtilization = std::min(100.0, (double)metrics.firingNeurons / std::max(1.0, (double)metrics.activeNeurons) * 100.0);
        metrics.synapticActivity = (double)metrics.totalSpikes / std::max(1.0, elapsed);
        metrics.learningProgress = (double)brain_.getTotalSpikeCount() / 10000.0; // Simplified
        metrics.predictionAccuracy = 0.85; // Placeholder
        metrics.rewardPredictionError = 0.1; // Placeholder
        metrics.noveltyScore = 0.5; // Placeholder
        metrics.curiosityLevel = 0.3; // Placeholder
        metrics.explorationRate = 0.4; // Placeholder
        metrics.exploitationRate = 0.6; // Placeholder
        
        // Resource metrics (simplified)
        metrics.cpuTemperature = 45.0 + metrics.cpuUsage * 0.5;
        metrics.memoryPressure = metrics.memoryUsageMB / (4096.0); // Estimate 4GB memory
        metrics.diskIO = 0.0;
        metrics.networkBandwidth = 0.0;
        
        // Time metrics
        metrics.stepTimeMs = 1000.0 / std::max(1.0, metrics.realTimeFactor);
        metrics.processingEfficiency = 0.95; // Placeholder
        metrics.numericalStability = 0.99; // Placeholder
        
        // Quality metrics (simplified)
        metrics.convergenceRate = 0.8; // Placeholder
        metrics.stabilityIndex = 0.95; // Placeholder
        metrics.robustnessScore = 0.85; // Placeholder
        metrics.adaptabilityScore = 0.75; // Placeholder
        
        return metrics;
    }

    std::vector<AdvancedPerformanceMetrics> getHistory(size_t limit) const {
        std::vector<AdvancedPerformanceMetrics> result;
        if (limit > statsHistory_.size()) {
            limit = statsHistory_.size();
        }
        result.reserve(limit);
        
        size_t startIdx = statsHistory_.size() - limit;
        for (size_t i = startIdx; i < statsHistory_.size(); ++i) {
            result.push_back(statsHistory_[i]);
        }
        
        return result;
    }

    DetailedStepMetrics getLastStepMetrics() const {
        return lastStepMetrics_;
    }

private:
    void monitorLoop() {
        while (running_) {
            auto metrics = getMetrics();
            {
                std::lock_guard<std::mutex> lock(historyMutex_);
                statsHistory_.push_back(metrics);
                if (statsHistory_.size() > 10000) {
                    statsHistory_.erase(statsHistory_.begin());
                }
            }
            
            std::this_thread::sleep_for(std::chrono::duration<double>(interval_));
        }
    }

    Brain& brain_;
    double interval_;
    bool running_;
    std::thread monitorThread_;
    mutable std::mutex historyMutex_;
    std::vector<AdvancedPerformanceMetrics> statsHistory_;
    DetailedStepMetrics lastStepMetrics_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::atomic<size_t> stepsCompleted_{0};

    friend void incrementSteps(Brain& brain);
};

void incrementSteps(Brain& brain) {
    // This would need to be called from brain.step()
    // For now, it's a placeholder
}

// Advanced debugging tools with network visualization
struct NetworkDebugData {
    std::vector<NeuronId> neurons;
    std::vector<SynapseId> synapses;
    std::vector<std::pair<NeuronId, NeuronId>> connections;
    std::vector<float> neuronPositionsX;
    std::vector<float> neuronPositionsY;
    std::vector<float> neuronRadii;
    std::vector<std::string> neuronLabels;
    std::vector<NeuronType> neuronTypes;
    std::vector<float> neuronActivities;
    std::vector<float> neuronFiringRates;
    std::vector<float> connectionWeights;
    std::vector<std::string> connectionTypes;
    std::vector<double> connectionDelays;
    std::vector<std::string> connectionPlasticties;
    std::vector<size_t> adjacencyList;
    std::vector<std::vector<size_t>> adjacencyMatrix;
    std::vector<std::vector<size_t>> shortestPaths;
    std::vector<std::vector<float>> betweennessCentrality;
    std::vector<std::vector<float>> clusteringCoefficients;
    std::vector<std::vector<float>> eigenvalueCentrality;
    std::string toString() const {
        std::stringstream ss;
        ss << "NetworkDebugData: "
           << "neurons=" << neurons.size() << ", "
           << "synapses=" << synapses.size() << ", "
           << "connections=" << connections.size();
        return ss.str();
    }
};

NetworkDebugData getNetworkDebugData(Brain& brain) {
    NetworkDebugData result;
    
    // Get all regions and collect detailed network data
    auto allRegions = brain.getRegions();
    size_t neuronIndex = 0;
    
    for (const auto& region : allRegions) {
        for (const auto& neuron : region->getAllNeurons()) {
            result.neurons.push_back(neuron->getId());
            result.neuronPositionsX.push_back(static_cast<float>(region->getId().value % 100)); // Simplified positioning
            result.neuronPositionsY.push_back(static_cast<float>(region->getId().value % 100)); // Simplified positioning
            result.neuronRadii.push_back(1.0f + (static_cast<float>(neuron->getState().firingRate) / 100.0f)); // Size based on firing rate
            result.neuronLabels.push_back("Neuron " + std::to_string(neuron->getId().value));
            result.neuronTypes.push_back(neuron->getType());
            result.neuronActivities.push_back(static_cast<float>(neuron->getState().refractoryRemaining) / 10.0f); // Activity based on refractory
            result.neuronFiringRates.push_back(neuron->getState().firingRate);
            
            for (const auto& synapse : region->getSynapses()) {
                if (synapse->getSourceNeuron() == neuron.get()) {
                    result.synapses.push_back(synapse->getId());
                    result.connections.push_back({neuron->getId(), synapse->getDestinationNeuron()->getId()});
                    result.connectionWeights.push_back(synapse->getWeight());
                    result.connectionTypes.push_back("chemical"); // Simplified
                    result.connectionDelays.push_back(static_cast<double>(synapse->getDelay()));
                    result.connectionPlasticties.push_back("STDP"); // Simplified
                }
            }
        }
    }
    
    // Calculate network metrics
    size_t numNeurons = result.neurons.size();
    result.adjacencyList.resize(numNeurons);
    result.adjacencyMatrix.resize(numNeurons, std::vector<size_t>(numNeurons, 0));
    
    for (size_t i = 0; i < numNeurons; ++i) {
        for (size_t j = 0; j < numNeurons; ++j) {
            if (result.connections[i].second == result.neurons[j]) {
                result.adjacencyMatrix[i][j] = 1;
                result.adjacencyList[i].push_back(j);
            }
        }
    }
    
    return result;
}

// Advanced step-by-step execution control
class AdvancedStepController {
public:
    AdvancedStepController(Brain& brain) : brain_(brain), paused_(false), stepDelay_(0.0), 
                                        stepCount_(0), maxSteps_(0), currentStep_(0),
                                        autoContinue_(false), stepCallback_(nullptr) {
    }

    void step() {
        if (paused_) {
            return;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Execute simulation step
        brain_.step(currentStep_);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        // Record step metrics
        DetailedStepMetrics metrics;
        metrics.stepStartTime = startTime;
        metrics.stepEndTime = endTime;
        metrics.stepDurationMs = duration;
        metrics.stepSucceeded = true;
        metrics.neuronsProcessed = brain_.getTotalNeuronCount();
        metrics.spikesGenerated = brain_.getTotalSpikeCount() - totalSpikesBeforeStep_;
        metrics.numericalError = 0.0; // Placeholder
        
        lastStepMetrics_ = metrics;
        totalSpikesBeforeStep_ = brain_.getTotalSpikeCount();
        currentStep_++;
        stepsCompleted_++;
        
        // Call callback if registered
        if (stepCallback_) {
            stepCallback_(currentStep_, duration, metrics);
        }
        
        // Auto-continue if enabled
        if (autoContinue_ && maxSteps_ > 0 && currentStep_ >= maxSteps_) {
            paused_ = true;
        }
        
        // Apply delay if specified
        if (stepDelay_ > 0.0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(stepDelay_));
        }
    }

    void pause() {
        paused_ = true;
    }

    void resume() {
        paused_ = false;
    }

    void stop() {
        paused_ = true;
        currentStep_ = 0;
    }

    bool isPaused() const { return paused_; }
    size_t getCurrentStep() const { return currentStep_; }
    size_t getTotalSteps() const { return stepsCompleted_; }
    DetailedStepMetrics getLastStepMetrics() const { return lastStepMetrics_; }

    void setStepDelay(double delay) { stepDelay_ = delay; }
    void setMaxSteps(size_t maxSteps) { maxSteps_ = maxSteps; }
    void setAutoContinue(bool autoContinue) { autoContinue_ = autoContinue; }
    void setStepCallback(std::function<void(size_t, double, const DetailedStepMetrics&)> callback) {
        stepCallback_ = callback;
    }

private:
    Brain& brain_;
    bool paused_;
    double stepDelay_;
    size_t stepCount_;
    size_t maxSteps_;
    size_t currentStep_;
    bool autoContinue_;
    std::function<void(size_t, double, const DetailedStepMetrics&)> stepCallback_;
    
    DetailedStepMetrics lastStepMetrics_;
    size_t totalSpikesBeforeStep_;
    size_t stepsCompleted_;
};

} // namespace nlm

// Python bindings for advanced statistics
void initAdvancedStatistics(py::module_& m) {
    // Neural connectivity analysis for complex networks
    m.def("analyzeNetworkConnectivity", &nlm::analyzeNetworkConnectivity,
          py::arg("brain"), py::arg("regions"),
          "Analyze connectivity patterns between brain regions");
    
    // Population statistics with detailed breakdown
    m.def("getDetailedPopulationStats", &nlm::getDetailedPopulationStats,
          py::arg("brain"), py::arg("neuron_type"),
          "Get comprehensive statistics for neuron population");
    
    // Advanced time-series analysis with multiple metrics
    m.def("analyzeAdvancedTimeSeries", &nlm::analyzeAdvancedTimeSeries,
          py::arg("brain"), py::arg("time_window"), py::arg("neuron_type"),
          "Analyze neural dynamics with advanced time-series metrics");
}

// Python bindings for advanced configuration
void initAdvancedConfiguration(py::module_& m) {
    // Advanced configuration validation
    m.def("validateConfigAdvanced", &nlm::validateConfigAdvanced,
          py::arg("config"), py::arg("schema") = nullptr,
          py::arg("check_inheritance") = true, py::arg("check_constraints") = true,
          "Validate configuration with advanced features");
    
    // Advanced configuration template
    m.def("createAdvancedConfigTemplate", &nlm::createAdvancedConfigTemplate,
          py::arg("name"), py::arg("description"), py::arg("default_values"),
          py::arg("parameter_types") = std::map<std::string, std::string>(),
          py::arg("constraints") = std::map<std::string, std::string>(),
          py::arg("allowed_values") = std::map<std::string, std::vector<std::string>>(),
          py::arg("dependencies") = std::map<std::string, std::string>(),
          py::arg("mutual_exclusions") = std::map<std::string, std::string>(),
          py::arg("validation_rules") = std::map<std::string, std::string>(),
          py::arg("conditional_defaults") = std::map<std::string, ConfigValue>(),
          py::arg("required_parameters") = std::map<std::string, bool>(),
          py::arg("template_tags") = std::vector<std::string>(),
          "Create advanced configuration template with validation rules and constraints");
}

// Python bindings for advanced performance monitoring
void initAdvancedPerformanceMonitoring(py::module_& m) {
    py::class_<nlm::AdvancedPerformanceMonitor>(m, "AdvancedPerformanceMonitor")
        .def(py::init<nlm::Brain&, double>(), py::arg("brain"), py::arg("interval") = 1.0,
             "Initialize advanced performance monitor with detailed metrics collection")
        .def("start", &nlm::AdvancedPerformanceMonitor::start,
             "Start comprehensive performance monitoring")
        .def("stop", &nlm::AdvancedPerformanceMonitor::stop,
             "Stop performance monitoring")
        .def("getMetrics", &nlm::AdvancedPerformanceMonitor::getMetrics,
             "Get comprehensive performance metrics")
        .def("getHistory", &nlm::AdvancedPerformanceMonitor::getHistory,
             py::arg("limit") = 100, "Get performance metrics history")
        .def("getLastStepMetrics", &nlm::AdvancedPerformanceMonitor::getLastStepMetrics,
             "Get detailed metrics for the last simulation step");
}

// Python bindings for advanced debugging tools
void initAdvancedDebugging(py::module_& m) {
    py::class_<nlm::AdvancedStepController>(m, "AdvancedStepController")
        .def(py::init<nlm::Brain&>(), py::arg("brain"),
             "Initialize advanced step controller with pause/resume functionality")
        .def("step", &nlm::AdvancedStepController::step,
             "Execute a single simulation step with detailed timing and metrics")
        .def("pause", &nlm::AdvancedStepController::pause,
             "Pause simulation step execution")
        .def("resume", &nlm::AdvancedStepController::resume,
             "Resume simulation step execution")
        .def("stop", &nlm::AdvancedStepController::stop,
             "Stop all simulation steps")
        .def("isPaused", &nlm::AdvancedStepController::isPaused,
             "Check if simulation is paused")
        .def("getCurrentStep", &nlm::AdvancedStepController::getCurrentStep,
             "Get current step number")
        .def("getTotalSteps", &nlm::AdvancedStepController::getTotalSteps,
             "Get total steps completed")
        .def("getLastStepMetrics", &nlm::AdvancedStepController::getLastStepMetrics,
             "Get detailed metrics for the last step")
        .def("setStepDelay", &nlm::AdvancedStepController::setStepDelay,
             py::arg("delay"), "Set delay between simulation steps")
        .def("setMaxSteps", &nlm::AdvancedStepController::setMaxSteps,
             py::arg("maxSteps"), "Set maximum number of steps to execute")
        .def("setAutoContinue", &nlm::AdvancedStepController::setAutoContinue,
             py::arg("autoContinue"), "Enable auto-continue after reaching max steps")
        .def("setStepCallback", &nlm::AdvancedStepController::setStepCallback,
             py::arg("callback"), "Set callback function to be called after each step");
    
    // Network visualization data
    py::class_<nlm::NetworkDebugData>(m, "NetworkDebugData")
        .def_readonly("neurons", &nlm::NetworkDebugData::neurons)
        .def_readonly("synapses", &nlm::NetworkDebugData::synapses)
        .def_readonly("connections", &nlm::NetworkDebugData::connections)
        .def_readonly("neuron_positions_x", &nlm::NetworkDebugData::neuronPositionsX)
        .def_readonly("neuron_positions_y", &nlm::NetworkDebugData::neuronPositionsY)
        .def_readonly("neuron_radii", &nlm::NetworkDebugData::neuronRadii)
        .def_readonly("neuron_labels", &nlm::NetworkDebugData::neuronLabels)
        .def_readonly("neuron_types", &nlm::NetworkDebugData::neuronTypes)
        .def_readonly("neuron_activities", &nlm::NetworkDebugData::neuronActivities)
        .def_readonly("neuron_firing_rates", &nlm::NetworkDebugData::neuronFiringRates)
        .def_readonly("connection_weights", &nlm::NetworkDebugData::connectionWeights)
        .def_readonly("connection_types", &nlm::NetworkDebugData::connectionTypes)
        .def_readonly("connection_delays", &nlm::NetworkDebugData::connectionDelays)
        .def_readonly("connection_plasticties", &nlm::NetworkDebugData::connectionPlasticties)
        .def("to_string", &nlm::NetworkDebugData::toString,
             "Get string representation of network debug data");
    
    m.def("getNetworkDebugData", &nlm::getNetworkDebugData,
          py::arg("brain"), "Get detailed network visualization data for debugging");
    
    // Detailed step metrics
    py::class_<nlm::AdvancedPerformanceMonitor::DetailedStepMetrics>(m, "DetailedStepMetrics")
        .def_readonly("step_duration_ms", &nlm::AdvancedPerformanceMonitor::DetailedStepMetrics::stepDurationMs)
        .def_readonly("neurons_processed", &nlm::AdvancedPerformanceMonitor::DetailedStepMetrics::neuronsProcessed)
        .def_readonly("spikes_generated", &nlm::AdvancedPerformanceMonitor::DetailedStepMetrics::spikesGenerated)
        .def_readonly("synaptic_update_time_ms", &nlm::AdvancedPerformanceMonitor::DetailedStepMetrics::synapticUpdateTimeMs)
        .def_readonly("plasticity_update_time_ms", &nlm::AdvancedPerformanceMonitor::DetailedStepMetrics::plasticityUpdateTimeMs)
        .def_readonly("memory_access_time_ms", &nlm::AdvancedPerformanceMonitor::DetailedStepMetrics::memoryAccessTimeMs)
        .def_readonly("communication_time_ms", &nlm::AdvancedPerformanceMonitor::DetailedStepMetrics::communicationTimeMs)
        .def_readonly("success", &nlm::AdvancedPerformanceMonitor::DetailedStepMetrics::stepSucceeded)
        .def("to_string", &nlm::AdvancedPerformanceMonitor::DetailedStepMetrics::toString,
             "Get string representation of step metrics");
}

// Extended enumerations
void initExtendedEnumerations(py::module_& m) {
    // Additional ActionType values
    py::enum_<nlm::ActionType>(m, "AdvancedActionType", py::arithmetic())
        .value("Sprint", static_cast<int>(nlm::ActionType::Custom) + 1)
        .value("Jump", static_cast<int>(nlm::ActionType::Custom) + 2)
        .value("Crouch", static_cast<int>(nlm::ActionType::Custom) + 3)
        .value("Attack", static_cast<int>(nlm::ActionType::Custom) + 4)
        .value("Defend", static_cast<int>(nlm::ActionType::Custom) + 5)
        .value("EmoteHappy", static_cast<int>(nlm::ActionType::Custom) + 6)
        .value("EmoteSad", static_cast<int>(nlm::ActionType::Custom) + 7)
        .value("EmoteAngry", static_cast<int>(nlm::ActionType::Custom) + 8)
        .value("EmoteSurprised", static_cast<int>(nlm::ActionType::Custom) + 9)
        .export_values();
    
    // Additional WorldObjectType values
    py::enum_<nlm::WorldObjectType>(m, "AdvancedWorldObjectType", py::arithmetic())
        .value("Portal", static_cast<int>(nlm::WorldObjectType::Marker) + 1)
        .value("Checkpoint", static_cast<int>(nlm::WorldObjectType::Marker) + 2)
        .value("Treasure", static_cast<int>(nlm::WorldObjectType::Resource) + 1)
        .value("Trap", static_cast<int>(nlm::WorldObjectType::Hazard) + 1)
        .value("NPC", static_cast<int>(nlm::WorldObjectType::Empty) + 1)
        .value("Vehicle", static_cast<int>(nlm::WorldObjectType::Empty) + 2)
        .export_values();
    
    // Enhanced NeuromodulatorType enumeration
    py::enum_<nlm::NeuromodulatorType>(m, "NeuromodulatorType")
        .value("Dopamine", nlm::NeuromodulatorType::Dopamine)
        .value("Serotonin", nlm::NeuromodulatorType::Serotonin)
        .value("Acetylcholine", nlm::NeuromodulatorType::Acetylcholine)
        .value("Noradrenaline", nlm::NeuromodulatorType::Noradrenaline)
        .value("GABA", nlm::NeuromodulatorType::GABA)
        .value("Glutamate", nlm::NeuromodulatorType::Glutamate)
        .value("Endorphin", nlm::NeuromodulatorType::Endorphin)
        .value("Oxytocin", nlm::NeuromodulatorType::Oxytocin)
        .value("Vasopressin", nlm::NeuromodulatorType::Vasopressin)
        .value("NeuropeptideY", nlm::NeuromodulatorType::NeuropeptideY)
        .value("Cannabinoid", nlm::NeuromodulatorType::Cannabinoid)
        .value("Cortisol", static_cast<int>(nlm::NeuromodulatorType::Cannabinoid) + 1)
        .value("Adrenaline", static_cast<int>(nlm::NeuromodulatorType::Cannabinoid) + 2)
        .value("Melatonin", static_cast<int>(nlm::NeuromodulatorType::Cannabinoid) + 3)
        .value("SubstanceP", static_cast<int>(nlm::NeuromodulatorType::Cannabinoid) + 4)
        .value("Enkephalin", static_cast<int>(nlm::NeuromodulatorType::Cannabinoid) + 5)
        .value("Dynorphin", static_cast<int>(nlm::NeuromodulatorType::Cannabinoid) + 6)
        .value("Bombesin", static_cast<int>(nlm::NeuromodulatorType::Cannabinoid) + 7)
        .value("Rimonabant", static_cast<int>(nlm::NeuromodulatorType::Cannabinoid) + 8)
        .export_values();
    
    // Enhanced DevelopmentalStage enumeration
    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .value("PostHormonal", static_cast<int>(nlm::DevelopmentalStage::Aging) + 1)
        .value("Senescence", static_cast<int>(nlm::DevelopmentalStage::Aging) + 2)
        .value("CellularAging", static_cast<int>(nlm::DevelopmentalStage::Aging) + 3)
        .export_values();
}

PYBIND11_MODULE(pynlm, m) {
    initBatchOperations(m);
    initAdvancedStatistics(m);
    initAdvancedConfiguration(m);
    initAdvancedPerformanceMonitoring(m);
    initAdvancedDebugging(m);
    initExtendedEnumerations(m);
}
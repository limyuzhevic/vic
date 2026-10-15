// NLM (熙然) - Neural Learning Machine
// Statistics Export Module
// Phase 2: Real Neural Computation

#ifndef NLM_STATISTICS_H
#define NLM_STATISTICS_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <iostream>

namespace nlm {

struct NeuralStatistics {
    // Basic metrics
    uint64_t totalSpikes = 0;
    uint64_t activeNeurons = 0;
    uint64_t firingNeuronCount = 0;
    float averageFiringRate = 0.0f;
    float excitationInhibitionRatio = 0.0f;
    
    // Plasticity metrics
    uint64_t strengthenedSynapses = 0;
    uint64_t weakenedSynapses = 0;
    uint64_t unchangedSynapses = 0;
    float weightChangeMean = 0.0f;
    float weightChangeStdDev = 0.0f;
    
    // Learning metrics
    bool learningOccurred = false;
    std::string learningType;
    float learningScore = 0.0f;
    
    // Memory metrics
    uint64_t memoryCapacity = 0;
    float memoryUtilization = 0.0f;
    
    // Timing metrics
    double simulationTime = 0.0;
    double wallClockTime = 0.0;
    
    // Additional custom metrics
    std::map<std::string, float> customMetrics;
};

class StatisticsExporter {
public:
    StatisticsExporter() = default;
    virtual ~StatisticsExporter() = default;
    
    virtual bool exportToFile(const NeuralStatistics& stats, const std::string& filepath) const;
    virtual bool exportToJSON(const NeuralStatistics& stats, std::ostream& output) const;
    virtual bool exportToCSV(const NeuralStatistics& stats, std::ostream& output) const;
    virtual bool exportToHTML(const NeuralStatistics& stats, const std::string& filepath) const;
    
    virtual bool addCustomMetric(const std::string& name, float value);
    virtual bool removeCustomMetric(const std::string& name);
    virtual float getCustomMetric(const std::string& name) const;
    virtual std::map<std::string, float> getAllCustomMetrics() const;
    
    virtual std::string generateReport(const NeuralStatistics& stats) const;
    virtual std::string generateDetailedReport(const NeuralStatistics& stats) const;
    
protected:
    std::map<std::string, float> customMetrics_;
};

} // namespace nlm

#endif // NLM_STATISTICS_H
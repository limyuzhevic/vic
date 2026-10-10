#ifndef NLM_SPIKEPFROFILER_H
#define NLM_SPIKEPFROFILER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>

namespace nlm {

class SpikeProfiler {
public:
    struct SpikeActivityResult {
        std::string operation;
        size_t spikeCount;
        double averageSpikesPerStep;
        double spikeRateHz;
        size_t maxNeuronsSpiking;
        std::chrono::microseconds processingTime;
    };

    SpikeProfiler() = default;
    ~SpikeProfiler() = default;

    // Record spike activity
    void recordSpikeActivity(const std::string& operationName, size_t spikeCount, 
                           size_t neuronsSpiking, const std::chrono::microseconds& processingTime);

    // Get results for a specific operation
    const SpikeActivityResult* getResult(const std::string& operationName) const;

    // Get all results
    const std::vector<SpikeActivityResult>& getAllResults() const { return results_; }

    // Clear all profiling data
    void clear();

    // Print profiling summary to stdout
    void printSummary() const;

    // Print results for a specific operation
    void printOperationResults(const std::string& operationName) const;

    // Get total spike count
    size_t getTotalSpikeCount() const;

    // Get total processing time
    std::chrono::microseconds getTotalProcessingTime() const;

    // Reset counters
    void reset();

    // Set whether profiling is enabled
    void setEnabled(bool enabled) { enabled_ = enabled; }

    // Check if profiling is enabled
    bool isEnabled() const { return enabled_; }

private:
    struct SpikeActivityData {
        size_t spikeCount{0};
        size_t neuronsSpiking{0};
        std::chrono::microseconds processingTime{0};
        size_t stepCount{0};
    };

    std::unordered_map<std::string, SpikeActivityData> spikeData_;
    std::vector<SpikeActivityResult> results_;
    bool enabled_{true};
};

} // namespace nlm

#endif // NLM_SPIKEPFROFILER_H

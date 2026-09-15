#include "InternalSignals.hpp"
#include "../experiments/Metrics.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct InternalSignalsProcessor::Impl {
    std::vector<float> processedSignals;
    std::vector<float> homeostaticSignals;
    
    // Metrics tracking
    std::shared_ptr<Metrics> metrics;
    
    Impl() : metrics(nullptr) {}
};

InternalSignalsProcessor::InternalSignalsProcessor() : pImpl(new Impl) {}

InternalSignalsProcessor::~InternalSignalsProcessor() = default;

void InternalSignalsProcessor::process(const InternalSignals& signals, std::shared_ptr<Metrics> metrics) {
    // TODO PHASE 2: Implement real internal signal processing
    // PLACEHOLDER: Just pass through
    pImpl->processedSignals = signals.getData();
    
    // Generate some homeostatic signals
    pImpl->homeostaticSignals.clear();
    pImpl->homeostaticSignals.push_back(1.0f);  // Allostatic load
    pImpl->homeostaticSignals.push_back(0.5f);  // Metabolic state
    
    // Record metrics
    pImpl->metrics = metrics;
    if (pImpl->metrics) {
        // Basic metrics about the signals
        pImpl->metrics->record("internal_signal_count", signals.getData().size());
        
        // Process signals metrics
        if (!pImpl->processedSignals.empty()) {
            float sum = std::accumulate(pImpl->processedSignals.begin(), pImpl->processedSignals.end(), 0.0);
            float mean = sum / pImpl->processedSignals.size();
            pImpl->metrics->record("internal_signal_mean", mean);
            
            // Calculate standard deviation
            double sq_sum = 0.0;
            for (float s : pImpl->processedSignals) {
                sq_sum += (s - mean) * (s - mean);
            }
            float std_dev = std::sqrt(sq_sum / pImpl->processedSignals.size());
            pImpl->metrics->record("internal_signal_std", std_dev);
            
            // Calculate min/max
            float min_val = *std::min_element(pImpl->processedSignals.begin(), pImpl->processedSignals.end());
            float max_val = *std::max_element(pImpl->processedSignals.begin(), pImpl->processedSignals.end());
            pImpl->metrics->record("internal_signal_min", min_val);
            pImpl->metrics->record("internal_signal_max", max_val);
        }
        
        // Homeostatic signals metrics
        if (!pImpl->homeostaticSignals.empty()) {
            pImpl->metrics->record("homeostatic_allostatic_load", pImpl->homeostaticSignals[0]);
            pImpl->metrics->record("homeostatic_metabolic_state", pImpl->homeostaticSignals[1]);
        }
    }
}

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

const std::shared_ptr<Metrics>& InternalSignalsProcessor::getMetrics() const {
    return pImpl->metrics;
}

} // namespace nlm

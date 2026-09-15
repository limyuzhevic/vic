#include "Vision.hpp"
#include "../experiments/Metrics.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    // Metrics tracking
    std::shared_ptr<Metrics> metrics;
    
    Impl() : featureDimensions(0), metrics(nullptr) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input, std::shared_ptr<Metrics> metrics) {
    // TODO PHASE 2: Implement real vision processing using NLM's neural machinery
    // PLACEHOLDER: Just pass through raw data
    pImpl->features = input.getData();
    pImpl->featureDimensions = input.getDimensions();
    
    // Record metrics
    pImpl->metrics = metrics;
    if (pImpl->metrics) {
        // Basic metrics about the input
        pImpl->metrics->record("vision_input_width", input.getWidth());
        pImpl->metrics->record("vision_input_height", input.getHeight());
        pImpl->metrics->record("vision_input_channels", input.getChannels());
        pImpl->metrics->record("vision_input_size", input.getData().size());
        
        // Feature statistics
        if (!pImpl->features.empty()) {
            float sum = std::accumulate(pImpl->features.begin(), pImpl->features.end(), 0.0);
            float mean = sum / pImpl->features.size();
            pImpl->metrics->record("vision_feature_mean", mean);
            
            // Calculate standard deviation
            double sq_sum = 0.0;
            for (float f : pImpl->features) {
                sq_sum += (f - mean) * (f - mean);
            }
            float std_dev = std::sqrt(sq_sum / pImpl->features.size());
            pImpl->metrics->record("vision_feature_std", std_dev);
            
            // Calculate min/max
            float min_val = *std::min_element(pImpl->features.begin(), pImpl->features.end());
            float max_val = *std::max_element(pImpl->features.begin(), pImpl->features.end());
            pImpl->metrics->record("vision_feature_min", min_val);
            pImpl->metrics->record("vision_feature_max", max_val);
        }
    }
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

const std::shared_ptr<Metrics>& VisionProcessor::getMetrics() const {
    return pImpl->metrics;
}

} // namespace nlm

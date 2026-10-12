#pragma once

#include <vector>
#include <string>

namespace nlm {

class NoveltyCalculator {
public:
    struct Config {
        float sensoryNoveltyDecay = 0.99f;   // Decay rate for novelty levels
        float visionScaleFactor = 2.0f;     // Scale factor for vision novelty calculation
        float touchScaleFactor = 1.0f;      // Scale factor for touch signals
        float internalScaleFactor = 1.0f;   // Scale factor for internal signals
        float proprioceptionScaleFactor = 1.0f; // Scale factor for proprioception
        bool enableCuriosity = true;         // Enable curiosity system
    };

    NoveltyCalculator(const Config& config = Config()) : config_(config) {}

    struct NoveltyResult {
        float noveltyLevel = 0.0f;
        float curiosityLevel = 0.0f;
        float predictionError = 0.0f;
    };

    NoveltyResult calculateNovelty(const std::vector<float>& vision, const std::vector<float>& previousVision,
                                  float predictionError = 0.0f) {
        NoveltyResult result;

        if (!vision.empty()) {
            float totalDiff = 0.0f;
            for (size_t i = 0; i < vision.size() && i < previousVision.size(); ++i) {
                float diff = std::abs(vision[i] - previousVision[i]);
                totalDiff += diff;
            }

            // Normalize and apply decay
            result.noveltyLevel = totalDiff / std::max<size_t>(vision.size(), 1);
            result.noveltyLevel *= config_.sensoryNoveltyDecay;
        }

        // Calculate curiosity based on novelty and prediction error
        if (config_.enableCuriosity) {
            result.curiosityLevel = result.noveltyLevel * 2.0f + std::abs(predictionError) * 0.5f;
            result.curiosityLevel = std::clamp(result.curiosityLevel, 0.0f, 1.0f);
        }

        result.predictionError = predictionError;

        return result;
    }

    void enableCuriosity(bool enable) { config_.enableCuriosity = enable; }

private:
    Config config_;
};

} // namespace nlm

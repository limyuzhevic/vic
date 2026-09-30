// Neural Activity Calculation System
// Provides unified neural activity measurement across all motor and sensory systems

#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

class NeuralActivityCalculator {
public:
    // Neural activity calculation strategies
    enum class ActivityStrategy {
        MembranePotentialDeviation,  // |V - V_rest|
        AverageFiringRate,           // Average firing rate
        WeightedSum,                 // Weighted sum of inputs
        MaximumPotential,            // Maximum membrane potential
        RMS,                         // Root mean square
        Variance                     // Variance from mean
    };

    // Configuration for activity calculation
    struct ActivityConfig {
        ActivityStrategy strategy = ActivityStrategy::MembranePotentialDeviation;
        float scaleFactor = 1.0f;
        float threshold = 0.0f;
        bool useAbsoluteValue = true;
        bool clampToRange = false;
        std::pair<float, float> clampRange = {-1.0f, 1.0f};
    };

    // Calculate neural activity for a collection of neurons
    static float calculateActivity(const std::vector<float>& values, const ActivityConfig& config = ActivityConfig());

    // Calculate neural activity for a collection of neurons with membrane potentials
    static float calculateActivity(const std::vector<float>& membranePotentials,
                                   const std::vector<float>& restingPotentials,
                                   const ActivityConfig& config = ActivityConfig());

    // Calculate average activity across multiple neurons
    static float averageActivity(const std::vector<float>& activities);

    // Calculate activity with normalization
    static float normalizedActivity(const std::vector<float>& activities, float min = 0.0f, float max = 1.0f);

    // Convert activity to firing rate
    static float toFiringRate(float activity, float maxActivity = 1.0f, float maxFiringRate = 100.0f);

    // Extract common neural activity calculation patterns
    template<typename Iterator>
    static float rangeBasedActivity(Iterator begin, Iterator end, const ActivityConfig& config);

    // Clamp value to biologically plausible range
    static float clampNeurallyPlausible(float value, float min = -100.0f, float max = 50.0f);

    // Calculate deviation from mean
    static float deviationFromMean(const std::vector<float>& values, float mean = 0.0f);

    // Calculate standard deviation
    static float standardDeviation(const std::vector<float>& values, float mean = 0.0f);
};

} // namespace nlm

#include "ExperimentStatistics.hpp"

namespace nlm {

ExperimentStatistics::ExperimentStatistics() {
    // Constructor initialization
}

ExperimentStatistics::~ExperimentStatistics() {
    // Cleanup resources
}

ExperimentStatistics::ExperimentStatistics(ExperimentStatistics&& other) noexcept {
    // Move constructor implementation
}

ExperimentStatistics& ExperimentStatistics::operator=(ExperimentStatistics&& other) noexcept {
    // Move assignment operator implementation
    return *this;
}

// Descriptive statistics implementations
double ExperimentStatistics::calculateMean(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double ExperimentStatistics::calculateStandardDeviation(const std::vector<double>& data, double mean) const {
    if (data.empty()) return 0.0;
    if (mean == 0.0) mean = calculateMean(data);
    double sum = 0.0;
    for (double value : data) {
        sum += (value - mean) * (value - mean);
    }
    return std::sqrt(sum / data.size());
}

double ExperimentStatistics::calculateVariance(const std::vector<double>& data, double mean) const {
    if (data.empty()) return 0.0;
    if (mean == 0.0) mean = calculateMean(data);
    double sum = 0.0;
    for (double value : data) {
        sum += (value - mean) * (value - mean);
    }
    return sum / data.size();
}

double ExperimentStatistics::calculateMedian(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    size_t size = sorted.size();
    if (size % 2 == 0) {
        return (sorted[size / 2 - 1] + sorted[size / 2]) / 2.0;
    } else {
        return sorted[size / 2];
    }
}

double ExperimentStatistics::calculateMode(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    std::map<double, int> frequency;
    for (double value : data) {
        frequency[value]++;
    }
    double mode = data[0];
    int maxCount = 0;
    for (const auto& pair : frequency) {
        if (pair.second > maxCount) {
            mode = pair.first;
            maxCount = pair.second;
        }
    }
    return mode;
}

// ... (additional method implementations can be added as needed)

} // namespace nlm
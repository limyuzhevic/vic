#include "ExperimentResults.hpp"

namespace nlm {

ExperimentResults::ExperimentResults(const std::string& experimentName) {
    // Constructor initialization
}

ExperimentResults::~ExperimentResults() {
    // Cleanup resources
}

ExperimentResults::ExperimentResults(ExperimentResults&& other) noexcept {
    // Move constructor implementation
}

ExperimentResults& ExperimentResults::operator=(ExperimentResults&& other) noexcept {
    // Move assignment operator implementation
    return *this;
}

void ExperimentResults::addMetric(const std::string& name, double value, const std::string& unit) {
    // Base implementation
}

void ExperimentResults::addObservation(const std::string& name, const std::vector<double>& data) {
    // Base implementation
}

void ExperimentResults::addParameter(const std::string& name, const ConfigValue& value) {
    // Base implementation
}

void ExperimentResults::addStatistic(const std::string& name, const ExperimentStatistics& statistic) {
    // Base implementation
}

// ... (additional method implementations can be added as needed)

} // namespace nlm
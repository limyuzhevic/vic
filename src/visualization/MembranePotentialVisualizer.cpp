#include "NeuralActivityVisualizers.hpp"

namespace nlm {

struct MembranePotentialVisualizer::Impl {
    std::vector<int64_t> neuronsToDisplay;
    float thresholdHighlight;
    std::deque<std::tuple<int64_t, double, float>> voltageData;
    double currentTime;
    size_t maxVoltageHistory;
    bool showStatistics;
    bool showThreshold;
    bool showRefractory;
    
    Impl() : thresholdHighlight(0.0f), currentTime(0.0), maxVoltageHistory(1000), 
             showStatistics(true), showThreshold(true), showRefractory(true) {}
};

MembranePotentialVisualizer::MembranePotentialVisualizer() : pImpl(std::make_unique<Impl>()) {}

MembranePotentialVisualizer::~MembranePotentialVisualizer() = default;

MembranePotentialVisualizer::MembranePotentialVisualizer(MembranePotentialVisualizer&&) noexcept = default;
MembranePotentialVisualizer& MembranePotentialVisualizer::operator=(MembranePotentialVisualizer&&) noexcept = default;

bool MembranePotentialVisualizer::initialize(const VisualizationConfig& config) {
    return Visualizer::initialize(config);
}

void MembranePotentialVisualizer::update(const VisualizationData& data) {
    auto voltageDataVec = data.getMembranePotentialData();
    
    for (const auto& voltage : voltageDataVec) {
        pImpl->voltageData.emplace_back(std::get<0>(voltage), std::get<1>(voltage), std::get<2>(voltage));
    }
    
    // Trim history
    while (pImpl->voltageData.size() > pImpl->maxVoltageHistory) {
        pImpl->voltageData.pop_front();
    }
    
    pImpl->currentTime = pImpl->voltageData.empty() ? 0.0 : std::get<1>(pImpl->voltageData.back());
    
    Visualizer::update(data);
}

void MembranePotentialVisualizer::render(bool output) {
    Visualizer::render(output);
}

void MembranePotentialVisualizer::configure(const VisualizationConfig& config) {
    Visualizer::configure(config);
}

VisualizationType MembranePotentialVisualizer::getType() const {
    return VisualizationType::NeuralActivity;
}

std::string MembranePotentialVisualizer::getName() const {
    return "MembranePotentialVisualizer";
}

void MembranePotentialVisualizer::setNeuronsToDisplay(const std::vector<int64_t>& neuronIds) {
    pImpl->neuronsToDisplay = neuronIds;
}

void MembranePotentialVisualizer::setThresholdHighlight(float threshold) {
    pImpl->thresholdHighlight = threshold;
}

std::unordered_map<std::string, double> MembranePotentialVisualizer::getVoltageStatistics() const {
    std::unordered_map<std::string, double> stats;
    
    if (pImpl->voltageData.empty()) {
        return stats;
    }
    
    // Calculate basic statistics
    std::vector<float> voltages;
    for (const auto& voltage : pImpl->voltageData) {
        voltages.push_back(std::get<2>(voltage));
    }
    
    double sum = std::accumulate(voltages.begin(), voltages.end(), 0.0);
    stats["mean_voltage"] = sum / voltages.size();
    
    double variance = 0.0;
    for (float voltage : voltages) {
        double diff = voltage - stats["mean_voltage"];
        variance += diff * diff;
    }
    stats["std_dev_voltage"] = std::sqrt(variance / voltages.size());
    
    stats["max_voltage"] = *std::max_element(voltages.begin(), voltages.end());
    stats["min_voltage"] = *std::min_element(voltages.begin(), voltages.end());
    
    return stats;
}

bool MembranePotentialVisualizer::exportData(const std::string& format, const std::string& filepath) {
    // Implementation would export voltage data in specified format
    return false;
}

} // namespace nlm
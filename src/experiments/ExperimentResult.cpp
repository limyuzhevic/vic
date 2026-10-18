#include "ExperimentResult.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace nlm {

ExperimentResult ExperimentResult::fromExperiment(
    std::shared_ptr<Experiment> experiment,
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Environment> environment
) {
    ExperimentResult result;
    
    if (!experiment) {
        result.setStatus(Status::ERROR);
        result.setExperimentName("INVALID");
        return result;
    }
    
    result.setExperimentName(experiment->getName());
    result.setStatus(Status::SUCCESS);
    result.setTimestamp(std::chrono::system_clock::now());
    
    // Copy metrics from experiment
    auto expMetrics = experiment->getMetrics();
    for (const auto& pair : expMetrics) {
        result.setMetric(pair.first, pair.second);
    }
    
    // Copy step data
    auto expStepData = experiment->getStepData();
    for (const auto& step : expStepData) {
        result.addStepData(step);
    }
    
    // Copy notes
    auto expNotes = experiment->getNotes();
    for (const auto& note : expNotes) {
        result.addNote(note);
    }
    
    // Set duration (placeholder - in real implementation would be calculated)
    result.setDuration(1000.0);
    
    return result;
}

double ExperimentResult::getMetric(const std::string& name) const {
    auto it = metrics_.find(name);
    return (it != metrics_.end()) ? it->second : 0.0;
}

void ExperimentResult::setMetric(const std::string& name, double value) {
    metrics_[name] = value;
}

std::string ExperimentResult::serialize() const {
    std::stringstream ss;
    ss << "ExperimentResult:" << std::endl;
    ss << "  Name: " << experimentName_ << std::endl;
    ss << "  Status: " << static_cast<int>(status_) << std::endl;
    ss << "  Duration: " << durationMs_ << std::endl;
    ss << "  Timestamp: " << std::chrono::system_clock::to_time_t(timestamp_) << std::endl;
    ss << "  Metrics:" << std::endl;
    
    for (const auto& pair : metrics_) {
        ss << "    " << pair.first << ": " << pair.second << std::endl;
    }
    
    ss << "  Notes:" << std::endl;
    for (const auto& note : notes_) {
        ss << "    " << note << std::endl;
    }
    
    ss << "  StepData count: " << stepData_.size() << std::endl;
    
    return ss.str();
}

ExperimentResult ExperimentResult::deserialize(const std::string& serialized) {
    ExperimentResult result;
    // In real implementation, would parse serialized string
    result.setExperimentName("DESERIALIZED");
    result.setStatus(Status::SUCCESS);
    return result;
}

std::string ExperimentResult::getSummary() const {
    std::stringstream ss;
    ss << "ExperimentResult Summary:" << std::endl;
    ss << "  Experiment: " << experimentName_ << std::endl;
    ss << "  Status: " << static_cast<int>(status_) << std::endl;
    ss << "  Duration: " << durationMs_ << "ms" << std::endl;
    ss << "  Metrics:" << std::endl;
    
    for (const auto& pair : metrics_) {
        ss << "    " << pair.first << ": " << pair.second << std::endl;
    }
    
    return ss.str();
}

ExperimentResult::PerformanceMetrics ExperimentResult::calculatePerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    metrics.avgFiringRate = getMetric("avg_firing_rate");
    metrics.totalReward = getMetric("total_reward");
    
    // Calculate efficiency score
    double totalNeurons = getMetric("neuron_count");
    metrics.efficiencyScore = (totalNeurons > 0) ? (metrics.totalReward / totalNeurons) : 0.0;
    
    // Calculate convergence rate
    metrics.convergenceRate = 1.0; // Placeholder
    
    return metrics;
}

bool ExperimentResult::approximatelyEqual(const ExperimentResult& other, double tolerance) const {
    if (experimentName_ != other.experimentName_) return false;
    if (std::abs(durationMs_ - other.durationMs_) > tolerance) return false;
    
    for (const auto& pair : metrics_) {
        auto it = other.metrics_.find(pair.first);
        if (it == other.metrics_.end()) return false;
        if (std::abs(pair.second - it->second) > tolerance) return false;
    }
    
    return true;
}

size_t ExperimentResult::getHash() const {
    std::hash<std::string> hasher;
    size_t hash = hasher(experimentName_);
    
    for (const auto& pair : metrics_) {
        size_t metricHash = hasher(pair.first) ^ std::hash<double>()(pair.second);
        hash ^= metricHash;
    }
    
    return hash;
}

std::vector<ExperimentResult> ExperimentResult::filterByMetric(
    const std::string& metricName,
    double threshold,
    bool greaterThan
) const {
    std::vector<ExperimentResult> filtered;
    double value = getMetric(metricName);
    
    if ((greaterThan && value > threshold) || (!greaterThan && value < threshold)) {
        filtered.push_back(*this);
    }
    
    return filtered;
}

ExperimentResult ExperimentResult::getBestResult(
    const std::vector<ExperimentResult>& results,
    const std::string& metricName
) {
    if (results.empty()) return ExperimentResult();
    
    auto bestIt = std::max_element(results.begin(), results.end(),
        [&metricName](const ExperimentResult& a, const ExperimentResult& b) {
            return a.getMetric(metricName) < b.getMetric(metricName);
        });
    
    return *bestIt;
}

double ExperimentResult::calculateMetric(const std::string& name, AggregationMode mode) const {
    // For single result, just return the metric value
    return getMetric(name);
}

void ExperimentResult::addExperiment(std::shared_ptr<Experiment> experiment) {
    experimentPtrs_.push_back(experiment);
    if (experiment) {
        auto expMetrics = experiment->getMetrics();
        for (const auto& pair : expMetrics) {
            setMetric(pair.first, pair.second);
        }
    }
}

bool ExperimentResult::compareWith(const ExperimentResult& other) const {
    return approximatelyEqual(other);
}

} // namespace nlm
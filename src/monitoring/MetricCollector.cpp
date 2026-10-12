#include "MetricCollector.hpp"
#include <iostream>
#include <chrono>

namespace nlm {

struct MetricCollector::Impl {
    bool monitoring;
    double updateInterval;
    double lastUpdateTime;
    std::map<std::string, std::function<double()>> metrics;
    std::map<std::string, double> collectedValues;
    double aggregationWindow;
    bool rollingAverageEnabled;
    double sampleRate;
    
    Impl() : monitoring(false), updateInterval(1.0), lastUpdateTime(0.0),
             aggregationWindow(60.0), rollingAverageEnabled(false), sampleRate(1.0) {}
};

MetricCollector::MetricCollector() : pImpl(std::make_unique<Impl>()) {}

bool MetricCollector::initialize() {
    pImpl->monitoring = false;
    pImpl->collectedValues.clear();
    return true;
}

void MetricCollector::startMonitoring() {
    pImpl->monitoring = true;
    pImpl->lastUpdateTime = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void MetricCollector::stopMonitoring() {
    pImpl->monitoring = false;
}

void MetricCollector::update() {
    if (!pImpl->monitoring) return;
    
    auto currentTime = std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    pImpl->lastUpdateTime = currentTime;
    
    // Collect metric values
    pImpl->collectedValues.clear();
    for (const auto& metric : pImpl->metrics) {
        try {
            pImpl->collectedValues[metric.first] = metric.second();
        } catch (...) {
            pImpl->collectedValues[metric.first] = 0.0;
        }
    }
}

bool MetricCollector::isMonitoring() const {
    return pImpl->monitoring;
}

void MetricCollector::addCustomMetric(const std::string& name, std::function<double()> collector) {
    pImpl->metrics[name] = collector;
}

void MetricCollector::removeCustomMetric(const std::string& name) {
    pImpl->metrics.erase(name);
}

double MetricCollector::getMetricValue(const std::string& name) const {
    auto it = pImpl->collectedValues.find(name);
    if (it != pImpl->collectedValues.end()) {
        return it->second;
    }
    return 0.0;
}

std::vector<std::string> MetricCollector::getAvailableMetrics() const {
    std::vector<std::string> result;
    for (const auto& metric : pImpl->metrics) {
        result.push_back(metric.first);
    }
    return result;
}

double MetricCollector::getLastUpdateTime() const {
    return pImpl->lastUpdateTime;
}

void MetricCollector::setUpdateInterval(double interval) {
    pImpl->updateInterval = interval;
}

double MetricCollector::getUpdateInterval() const {
    return pImpl->updateInterval;
}

void MetricCollector::setAggregationWindow(double window) {
    pImpl->aggregationWindow = window;
}

void MetricCollector::enableRollingAverage(bool enable) {
    pImpl->rollingAverageEnabled = enable;
}

void MetricCollector::setSampleRate(double rate) {
    pImpl->sampleRate = rate;
}

} // namespace nlm
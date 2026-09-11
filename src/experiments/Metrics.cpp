#include "Metrics.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <cmath>

namespace nlm {

struct Metrics::Impl {
    std::map<std::string, std::vector<double>> metrics;
};

Metrics::Metrics() : pImpl(std::make_unique<Impl>()) {}

Metrics::~Metrics() = default;

void Metrics::record(const std::string& name, double value) {
    pImpl->metrics[name].push_back(value);
}

const std::vector<double>& Metrics::getValues(const std::string& name) const {
    static std::vector<double> empty;
    auto it = pImpl->metrics.find(name);
    if (it != pImpl->metrics.end()) {
        return it->second;
    }
    return empty;
}

double Metrics::getMean(const std::string& name) const {
    auto it = pImpl->metrics.find(name);
    if (it == pImpl->metrics.end() || it->second.empty()) {
        return 0.0;
    }
    double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
    return sum / it->second.size();
}

double Metrics::getStdDev(const std::string& name) const {
    auto it = pImpl->metrics.find(name);
    if (it == pImpl->metrics.end() || it->second.size() < 2) {
        return 0.0;
    }
    double mean = getMean(name);
    double sumSquares = 0.0;
    for (double v : it->second) {
        double diff = v - mean;
        sumSquares += diff * diff;
    }
    return std::sqrt(sumSquares / (it->second.size() - 1));
}

double Metrics::getMin(const std::string& name) const {
    auto it = pImpl->metrics.find(name);
    if (it == pImpl->metrics.end() || it->second.empty()) {
        return 0.0;
    }
    return *std::min_element(it->second.begin(), it->second.end());
}

double Metrics::getMax(const std::string& name) const {
    auto it = pImpl->metrics.find(name);
    if (it == pImpl->metrics.end() || it->second.empty()) {
        return 0.0;
    }
    return *std::max_element(it->second.begin(), it->second.end());
}

std::vector<std::string> Metrics::getMetricNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->metrics) {
        names.push_back(pair.first);
    }
    return names;
}

void Metrics::clear() {
    pImpl->metrics.clear();
}

std::string Metrics::summary() const {
    std::ostringstream oss;
    for (const auto& pair : pImpl->metrics) {
        oss << pair.first << ": mean=" << getMean(pair.first)
            << ", std=" << getStdDev(pair.first)
            << ", min=" << getMin(pair.first)
            << ", max=" << getMax(pair.first) << "\n";
    }
    return oss.str();
}

} // namespace nlm

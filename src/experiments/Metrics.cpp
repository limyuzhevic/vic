// Implementation of Metrics collection and analysis
#include "Metrics.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <cmath>

namespace nlm {

struct Metrics::Impl {
    std::map<std::string, std::vector<double>> data;
};

Metrics::Metrics() : pImpl(new Impl) {}

Metrics::~Metrics() = default;

void Metrics::record(const std::string& name, double value) {
    pImpl->data[name].push_back(value);
}

const std::vector<double>& Metrics::getValues(const std::string& name) const {
    static const std::vector<double> empty;
    auto it = pImpl->data.find(name);
    return it != pImpl->data.end() ? it->second : empty;
}

double Metrics::getMean(const std::string& name) const {
    const auto& values = getValues(name);
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double Metrics::getStdDev(const std::string& name) const {
    const auto& values = getValues(name);
    if (values.size() < 2) return 0.0;
    
    double mean = getMean(name);
    double sumSq = 0.0;
    for (double val : values) {
        sumSq += (val - mean) * (val - mean);
    }
    return std::sqrt(sumSq / (values.size() - 1));
}

double Metrics::getMin(const std::string& name) const {
    const auto& values = getValues(name);
    if (values.empty()) return 0.0;
    return *std::min_element(values.begin(), values.end());
}

double Metrics::getMax(const std::string& name) const {
    const auto& values = getValues(name);
    if (values.empty()) return 0.0;
    return *std::max_element(values.begin(), values.end());
}

std::vector<std::string> Metrics::getMetricNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->data.size());
    for (const auto& pair : pImpl->data) {
        names.push_back(pair.first);
    }
    return names;
}

void Metrics::clear() {
    pImpl->data.clear();
}

std::string Metrics::summary() const {
    std::ostringstream oss;
    oss << "Metrics Summary:" << std::endl;
    
    for (const auto& pair : pImpl->data) {
        const std::string& name = pair.first;
        const auto& values = pair.second;
        
        if (values.empty()) continue;
        
        oss << "  " << name << ": ";
        oss << "count=" << values.size();
        oss << ", mean=" << getMean(name);
        oss << ", stddev=" << getStdDev(name);
        oss << ", min=" << getMin(name);
        oss << ", max=" << getMax(name);
        oss << std::endl;
    }
    
    return oss.str();
}

} // namespace nlm
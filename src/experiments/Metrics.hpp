#pragma once

#include <vector>
#include <string>
#include <map>

namespace nlm {

// Metrics collection and analysis
class Metrics {
public:
    Metrics(nullptr);
    ~Metrics(nullptr);
    
    // Record a metric value
    void record(const std::string& name, double value);
    
    // Get metric values
    const std::vector<double>& getValues(const std::string& name) const;
    
    // Get statistics
    double getMean(const std::string& name) const;
    double getStdDev(const std::string& name) const;
    double getMin(const std::string& name) const;
    double getMax(const std::string& name) const;
    
    // Get all metric names
    std::vector<std::string> getMetricNames(nullptr) const;
    
    // Clear all metrics
    void clear(nullptr);
    
    // Summary report
    std::string summary(nullptr) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <thread>
#include <future>
#include <filesystem>
#include "../core/Config/Config.hpp"

namespace nlm {

class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer();
    
    void setConfig(std::shared_ptr<Config> config);
    
    bool optimizeMemoryUsage(const std::string& outputFile);
    bool optimizeCpuUsage(const std::string& outputFile);
    bool optimizeNeuralPerformance(const std::string& outputFile);
    
    bool applySimdOptimizations(const std::string& outputFile);
    bool applyMemoryPoolOptimizations(const std::string& outputFile);
    bool applyParallelProcessingOptimizations(const std::string& outputFile);
    
    std::map<std::string, double> getOptimizationRecommendations() const;
    std::vector<std::string> getOptimizationWarnings() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void analyzeMemoryUsage();
    void analyzeCpuUsage();
    void analyzeNeuralPerformance();
    
    void recommendMemoryOptimizations();
    void recommendCpuOptimizations();
    void recommendNeuralOptimizations();
    
    bool configureSimdSettings(const std::string& outputFile);
    bool configureMemoryPools(const std::string& outputFile);
    bool configureParallelSettings(const std::string& outputFile);
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentAutomation.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

class ExperimentGeneticAlgorithm {
public:
    ExperimentGeneticAlgorithm();
    ~ExperimentGeneticAlgorithm();
    
    // Disable copying, enable moving
    ExperimentGeneticAlgorithm(const ExperimentGeneticAlgorithm&) = delete;
    ExperimentGeneticAlgorithm& operator=(const ExperimentGeneticAlgorithm&) = delete;
    ExperimentGeneticAlgorithm(ExperimentGeneticAlgorithm&&) noexcept;
    ExperimentGeneticAlgorithm& operator=(ExperimentGeneticAlgorithm&&) noexcept;
    
    // Algorithm management
    bool createGA(const std::string& gaName, const GeneticAlgorithmConfig& config);
    bool removeGA(const std::string& gaName);
    bool updateGA(const std::string& gaName, const GeneticAlgorithmConfig& config);
    GeneticAlgorithmConfig getGAConfig(const std::string& gaName) const;
    std::vector<std::string> getAllGANames() const;
    
    // Algorithm execution
    bool runGA(const std::string& gaName);
    bool runGANow(const std::string& gaName);
    bool pauseGA(const std::string& gaName);
    bool resumeGA(const std::string& gaName);
    bool stopGA(const std::string& gaName);
    
    // Algorithm monitoring
    double getGAPProgress(const std::string& gaName) const;
    std::string getGAStatus(const std::string& gaName) const;
    std::chrono::system_clock::time_point getGAStartTime(const std::string& gaName) const;
    std::chrono::system_clock::time_point getGAEndTime(const std::string& gaName) const;
    std::chrono::seconds getGADuration(const std::string& gaName) const;
    std::string getGAErrorMessage(const std::string& gaName) const;
    
    // Algorithm results
    bool storeGAResults(const std::string& gaName, const Config& optimalParameters, double fitnessValue);
    std::pair<Config, double> getGAResults(const std::string& gaName) const;
    bool exportGAResults(const std::string& gaName, const std::string& filepath) const;
    
    // Algorithm analysis
    double calculateGAConvergence(const std::string& gaName) const;
    std::string analyzeGAPerformance(const std::string& gaName) const;
    std::map<std::string, double> getGAPopulationFitnessHistory(const std::string& gaName) const;
    
    // Algorithm optimization
    bool optimizeGAParameters(const std::string& gaName);
    bool autoTuneGASettings(const std::string& gaName);
    bool enableParallelGA(const std::string& gaName, bool enable);
    bool setGAGenerationLimit(const std::string& gaName, size_t maxGenerations);
    
    // Algorithm cleanup
    bool cleanupGAResults(const std::string& gaName);
    bool cleanupOldGAs(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetGA(const std::string& gaName);
    bool clearAllGAs();
    std::vector<std::string> getAllGANames() const;
    size_t getGACount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
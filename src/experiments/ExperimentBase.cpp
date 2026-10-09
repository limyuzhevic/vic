#include "ExperimentBase.hpp"
#include <cstdarg>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace nlm {

void BaseExperiment::addMetric(const std::string& name, double value) {
    metrics_.emplace_back(name, value, std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
}

double BaseExperiment::getMetric(const std::string& name) const {
    for (const auto& metric : metrics_) {
        if (metric.name == name) {
            return metric.value;
        }
    }
    throw std::runtime_error("Metric not found: " + name);
}

void BaseExperiment::logProgress(const std::string& message) {
    if (logger_) {
        logger_->log(message);
    }
    if (logEnabled_) {
        if (!logFile_.is_open()) {
            logFile_.open("experiment.log", std::ios::app);
        }
        if (logFile_.is_open()) {
            logFile_ << "[" << std::time(nullptr) << "] " << message << std::endl;
        }
    }
    std::cout << "[" << name_ << "] " << message << std::endl;
}

void BaseExperiment::logProgress(const std::string& format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    va_end(args);
    logProgress(std::string(buffer));
}

void BaseExperiment::saveResults(const std::string& filename) {
    // Default implementation - save basic metrics
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    file << "Experiment Results: " << name_ << std::endl;
    file << "Total Metrics: " << metrics_.size() << std::endl;
    file << "Config Keys: " << config_.size() << std::endl;
    
    file << "\nMetrics:\n";
    for (const auto& metric : metrics_) {
        file << metric.name << ": " << metric.value << " (at " << metric.timestamp << ")\n";
    }
    
    file << "\nConfiguration:\n";
    for (const auto& pair : config_) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    file.close();
}

void BaseExperiment::runEpisodes(size_t numEpisodes, std::function<void(size_t, double)> episodeFunc) {
    logProgress("Running %zu episodes", numEpisodes);
    
    for (size_t episode = 0; episode < numEpisodes; ++episode) {
        double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        episodeFunc(episode, startTime);
        
        if (episode % 50 == 0) {
            double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            logProgress("Episode %zu completed in %.2f seconds", episode, endTime - startTime);
        }
    }
}

void BaseExperiment::runSteps(size_t numSteps, std::function<void(size_t, double)> stepFunc) {
    logProgress("Running %zu steps", numSteps);
    
    for (size_t step = 0; step < numSteps; ++step) {
        double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        stepFunc(step, startTime);
        
        if (step % 100 == 0) {
            double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            logProgress("Step %zu completed in %.2f seconds", step, endTime - startTime);
        }
    }
}

} // namespace nlm

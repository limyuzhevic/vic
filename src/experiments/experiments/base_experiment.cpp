#include "base_experiment.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

BaseExperiment::BaseExperiment(const std::string& name, std::shared_ptr<Config> config)
    : state_(State::Created), progress_(0.0), currentStep_(0), totalSteps_(0),
      progressCallback_(nullptr), config_(config) {
    setName(name);
    startTime_ = std::chrono::high_resolution_clock::now();
}

void BaseExperiment::setParameter(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    parameters_[key] = value;
}

std::string BaseExperiment::getParameter(const std::string& key) const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    auto it = parameters_.find(key);
    if (it != parameters_.end()) {
        return it->second;
    }
    return "";
}

std::chrono::duration<double> BaseExperiment::getExecutionTime() const {
    auto end = std::chrono::high_resolution_clock::now();
    return end - startTime_;
}

bool BaseExperiment::saveState(const std::string& filepath) const {
    try {
        json j;
        j["name"] = getName();
        j["type"] = getExperimentType();
        j["parameters"] = parameters_;
        j["state"] = static_cast<int>(state_);
        j["progress"] = progress_;
        j["current_step"] = currentStep_;
        j["total_steps"] = totalSteps_;

        // Save metrics
        auto metrics = getMetrics();
        for (const auto& metric : metrics) {
            j["metrics"][metric.first] = metric.second;
        }

        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving experiment state: " << e.what() << std::endl;
    }
    return false;
}

bool BaseExperiment::loadState(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        json j;
        file >> j;

        setName(j["name"].get<std::string>());
        parameters_ = j["parameters"].get<std::map<std::string, std::string>>();
        state_ = static_cast<State>(j["state"].get<int>());
        progress_ = j["progress"].get<double>();
        currentStep_ = j["current_step"].get<size_t>();
        totalSteps_ = j["total_steps"].get<size_t>();

        // Restore metrics
        clearMetrics(); // Added to Experiment class
        for (const auto& item : j["metrics"].items()) {
            recordMetric(item.key(), item.value().get<double>());
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading experiment state: " << e.what() << std::endl;
    }
    return false;
}

} // namespace nlm
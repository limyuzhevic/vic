#ifndef PYNLM_PYTHON_UTILS_H
#define PYNLM_PYTHON_UTILS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <chrono>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;

// Python utilities namespace
namespace pynlm_utils {
    
    // Simulation statistics collector
    class SimulationStats {
    private:
        struct StepStats {
            int64_t step;
            int64_t firingNeurons;
            double firingRate;
            double curiosity;
            double novelty;
            double predictionError;
            double neuromodulationLevel;
            nlm::DevelopmentalStage developmentalStage;
            std::chrono::duration<double> stepTime;
        };
        
        std::vector<StepStats> stats_;
        double totalTime_;
        int64_t totalSteps_;
        
    public:
        SimulationStats() : totalTime_(0.0), totalSteps_(0) {}
        
        void addStepStats(int64_t step, int64_t firingNeurons, double firingRate,
                         double curiosity, double novelty, double predictionError,
                         double neuromodulationLevel, nlm::DevelopmentalStage stage,
                         double stepTime) {
            StepStats stat;
            stat.step = step;
            stat.firingNeurons = firingNeurons;
            stat.firingRate = firingRate;
            stat.curiosity = curiosity;
            stat.novelty = novelty;
            stat.predictionError = predictionError;
            stat.neuromodulationLevel = neuromodulationLevel;
            stat.developmentalStage = stage;
            stat.stepTime = std::chrono::duration<double>(stepTime);
            stats_.push_back(stat);
            totalSteps_++;
            totalTime_ += stepTime;
        }
        
        // Get statistics
        double getAverageFiringRate() const {
            if (stats_.empty()) return 0.0;
            double sum = 0.0;
            for (const auto& stat : stats_) {
                sum += stat.firingRate;
            }
            return sum / stats_.size();
        }
        
        double getAverageCuriosity() const {
            if (stats_.empty()) return 0.0;
            double sum = 0.0;
            for (const auto& stat : stats_) {
                sum += stat.curiosity;
            }
            return sum / stats_.size();
        }
        
        double getAverageNovelty() const {
            if (stats_.empty()) return 0.0;
            double sum = 0.0;
            for (const auto& stat : stats_) {
                sum += stat.novelty;
            }
            return sum / stats_.size();
        }
        
        double getAveragePredictionError() const {
            if (stats_.empty()) return 0.0;
            double sum = 0.0;
            for (const auto& stat : stats_) {
                sum += stat.predictionError;
            }
            return sum / stats_.size();
        }
        
        double getAverageNeuromodulationLevel() const {
            if (stats_.empty()) return 0.0;
            double sum = 0.0;
            for (const auto& stat : stats_) {
                sum += stat.neuromodulationLevel;
            }
            return sum / stats_.size();
        }
        
        int64_t getMaxFiringNeurons() const {
            if (stats_.empty()) return 0;
            int64_t max = 0;
            for (const auto& stat : stats_) {
                if (stat.firingNeurons > max) {
                    max = stat.firingNeurons;
                }
            }
            return max;
        }
        
        int64_t getMinFiringNeurons() const {
            if (stats_.empty()) return 0;
            int64_t min = INT64_MAX;
            for (const auto& stat : stats_) {
                if (stat.firingNeurons < min) {
                    min = stat.firingNeurons;
                }
            }
            return min;
        }
        
        double getTimePerStep() const {
            return totalSteps_ > 0 ? totalTime_ / totalSteps_ : 0.0;
        }
        
        double getTotalSimulationTime() const {
            return totalTime_;
        }
        
        int64_t getTotalSteps() const {
            return totalSteps_;
        }
        
        // Get stage distribution
        std::map<nlm::DevelopmentalStage, int64_t> getStageDistribution() const {
            std::map<nlm::DevelopmentalStage, int64_t> distribution;
            for (const auto& stat : stats_) {
                distribution[stat.developmentalStage]++;
            }
            return distribution;
        }
        
        // Get recent statistics (last N steps)
        std::vector<StepStats> getRecentStats(int64_t n) const {
            if (n > stats_.size()) {
                n = stats_.size();
            }
            std::vector<StepStats> recent;
            recent.reserve(n);
            for (auto it = stats_.rbegin(); it != stats_.rbegin() + n; ++it) {
                recent.push_back(*it);
            }
            std::reverse(recent.begin(), recent.end());
            return recent;
        }
        
        // Export to Python dictionary
        py::dict to_dict() const {
            py::dict result;
            result["total_steps"] = totalSteps_;
            result["total_time"] = totalTime_;
            result["average_firing_rate"] = getAverageFiringRate();
            result["max_firing_neurons"] = getMaxFiringNeurons();
            result["min_firing_neurons"] = getMinFiringNeurons();
            result["average_curiosity"] = getAverageCuriosity();
            result["average_novelty"] = getAverageNovelty();
            result["average_prediction_error"] = getAveragePredictionError();
            result["average_neuromodulation_level"] = getAverageNeuromodulationLevel();
            result["time_per_step"] = getTimePerStep();
            
            // Stage distribution
            auto stageDist = getStageDistribution();
            py::dict stageDict;
            for (const auto& pair : stageDist) {
                stageDict[py::cast(static_cast<int>(pair.first))] = pair.second;
            }
            result["stage_distribution"] = stageDict;
            
            return result;
        }
        
        // Clear statistics
        void clear() {
            stats_.clear();
            totalTime_ = 0.0;
            totalSteps_ = 0;
        }
    };
    
    // Progress tracking utilities
    class ProgressTracker {
    private:
        int64_t totalSteps_;
        std::chrono::steady_clock::time_point startTime_;
        std::chrono::steady_clock::time_point lastUpdateTime_;
        double lastProgress_;
        std::string message_;
        
    public:
        ProgressTracker(int64_t totalSteps, const std::string& message = "Progress")
            : totalSteps_(totalSteps), message_(message) {
            startTime_ = std::chrono::steady_clock::now();
            lastUpdateTime_ = startTime_;
            lastProgress_ = 0.0;
        }
        
        // Update progress and return percentage (0.0 to 1.0)
        double update(int64_t currentStep, const std::string& additionalInfo = "") {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double>(currentTime - startTime_);
            
            double progress = static_cast<double>(currentStep) / totalSteps_;
            double eta = (1.0 - progress) * elapsed.count() / progress;
            
            if (progress - lastProgress_ > 0.01 || currentStep == totalSteps_) {
                auto stepTime = std::chrono::duration<double>(currentTime - lastUpdateTime_);
                lastUpdateTime_ = currentTime;
                lastProgress_ = progress;
                
                // Print progress bar
                int barWidth = 50;
                int pos = static_cast<int>(barWidth * progress);
                
                std::cout << "\r[" << message_ << "] ";
                std::cout << "[";
                for (int i = 0; i < barWidth; ++i) {
                    if (i < pos) std::cout << "=";
                    else if (i == pos) std::cout << ">";
                    else std::cout << " ";
                }
                std::cout << "] " << static_cast<int>(progress * 100.0) << "% "
                          << currentStep << "/" << totalSteps_;
                if (progress > 0) {
                    std::cout << " ETA: " << eta << "s";
                }
                std::cout << " " << additionalInfo << std::flush;
            }
            
            return progress;
        }
        
        // Finish and print final message
        void finish() {
            auto currentTime = std::chrono::steady_clock::now();
            auto totalElapsed = std::chrono::duration<double>(currentTime - startTime_);
            std::cout << "\r[" << message_ << "] Complete! (" << totalElapsed.count() << "s)" << std::endl;
        }
    };
    
    // Configuration utilities
    namespace config_utils {
        
        // Create configuration from brain statistics
        std::shared_ptr<nlm::Config> createConfigFromStats(
            int64_t neuronCount, double connectionDensity,
            double vThreshold, double vRest, double vReset,
            double tauMem, double tauRef) {
            
            auto config = nlm::createDefaultConfig();
            
            // Set configuration values
            config->set("brain.neuron_count", neuronCount);
            config->set("brain.synapse_density", connectionDensity);
            config->set("brain.connection_probability", connectionDensity * 0.1);
            config->set("brain.v_thresh", vThreshold);
            config->set("brain.v_rest", vRest);
            config->set("brain.v_reset", vReset);
            config->set("brain.tau_mem", tauMem);
            config->set("brain.tau_ref", tauRef);
            
            return config;
        }
        
        // Validate configuration
        bool validateConfig(const std::shared_ptr<nlm::Config>& config) {
            if (!config) {
                return false;
            }
            
            // Basic validation
            if (config->has("brain.neuron_count")) {
                int neuronCount = config->getInt("brain.neuron_count");
                if (neuronCount <= 0) {
                    return false;
                }
            }
            
            if (config->has("brain.synapse_density")) {
                double density = config->getDouble("brain.synapse_density");
                if (density < 0.0 || density > 1.0) {
                    return false;
                }
            }
            
            return true;
        }
        
        // Get configuration as Python dictionary
        py::dict configToDict(const std::shared_ptr<nlm::Config>& config) {
            py::dict result;
            if (!config) {
                return result;
            }
            
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                // Try to get value as string (simplified)
                result[key] = "";
            }
            
            return result;
        }
        
        // Load configuration from JSON string
        std::shared_ptr<nlm::Config> loadConfigFromJson(const std::string& json) {
            // Note: This is a simplified implementation
            // In practice, you would use a proper JSON parser
            auto config = nlm::createDefaultConfig();
            
            // Try to parse simple key-value pairs
            size_t pos = 0;
            while ((pos = json.find('"', pos)) != std::string::npos) {
                size_t keyEnd = json.find('"', pos + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = json.substr(pos + 1, keyEnd - pos - 1);
                pos = json.find(':', keyEnd);
                if (pos == std::string::npos) break;
                
                size_t valueStart = json.find_first_not_of(' ', pos + 1);
                if (valueStart == std::string::npos) break;
                
                size_t valueEnd = json.find('"', valueStart);
                if (valueEnd == std::string::npos) break;
                
                std::string value = json.substr(valueStart, valueEnd - valueStart);
                
                // Set configuration value (simplified)
                if (key == "brain.neuron_count") {
                    config->setInt("brain.neuron_count", std::stoi(value));
                } else if (key == "brain.synapse_density") {
                    config->setDouble("brain.synapse_density", std::stod(value));
                }
                
                pos = valueEnd + 1;
            }
            
            return config;
        }
    }
    
    // World utilities
    namespace world_utils {
        
        // Create a world with resources and hazards
        std::shared_ptr<nlm::SimpleWorld> createWorldWithEnvironment(
            size_t width, size_t height,
            const std::vector<std::pair<double, double>>& resources,
            const std::vector<std::pair<double, double>>& hazards,
            double visionWidth, double visionHeight) {
            
            auto world = nlm::createSimpleWorld();
            world->configure(width, height, visionWidth, visionHeight);
            world->reset();
            
            // Add resources
            for (const auto& pos : resources) {
                auto resource = std::make_shared<nlm::WorldObject>(
                    pos.first, pos.second,
                    nlm::WorldObjectType::Resource, 1.0, 0.5
                );
                world->addObject(resource);
            }
            
            // Add hazards
            for (const auto& pos : hazards) {
                auto hazard = std::make_shared<nlm::WorldObject>(
                    pos.first, pos.second,
                    nlm::WorldObjectType::Hazard, 0.5, 0.5
                );
                world->addObject(hazard);
            }
            
            // Set agent start position
            world->setAgentStart(width / 2.0, height / 2.0);
            
            return world;
        }
        
        // Add obstacles to world
        void addObstacles(std::shared_ptr<nlm::SimpleWorld> world,
                         const std::vector<std::pair<double, double>>& obstaclePositions,
                         double obstacleRadius = 0.5) {
            for (const auto& pos : obstaclePositions) {
                auto wall = std::make_shared<nlm::WorldObject>(
                    pos.first, pos.second,
                    nlm::WorldObjectType::Wall, 0.0, obstacleRadius
                );
                world->addObject(wall);
            }
        }
        
        // Get nearest object to agent
        std::pair<std::shared_ptr<nlm::WorldObject>, double> getNearestObject(
            std::shared_ptr<nlm::SimpleWorld> world,
            const std::shared_ptr<nlm::AgentBody>& agentBody) {
            
            auto percept = world->getSensoryPercept();
            if (!percept) {
                return {nullptr, std::numeric_limits<double>::max()};
            }
            
            // Note: This is a simplified implementation
            // In practice, you would search through all world objects
            return {nullptr, std::numeric_limits<double>::max()};
        }
    }
    
    // Visualization utilities (simplified, requires matplotlib)
    namespace viz_utils {
        
        // Create simple firing rate visualization data
        std::vector<std::pair<double, double>> createFiringRateData(
            const std::vector<double>& firingRates,
            const std::vector<double>& timestamps) {
            
            std::vector<std::pair<double, double>> data;
            data.reserve(firingRates.size());
            
            for (size_t i = 0; i < firingRates.size(); ++i) {
                data.emplace_back(timestamps[i], firingRates[i]);
            }
            
            return data;
        }
        
        // Create developmental stage timeline
        std::vector<std::pair<nlm::DevelopmentalStage, double>> createStageTimeline(
            const std::vector<nlm::DevelopmentalStage>& stages,
            const std::vector<double>& timestamps) {
            
            std::vector<std::pair<nlm::DevelopmentalStage, double>> timeline;
            timeline.reserve(stages.size());
            
            for (size_t i = 0; i < stages.size(); ++i) {
                timeline.emplace_back(stages[i], timestamps[i]);
            }
            
            return timeline;
        }
    }
}

// Python bindings for utilities
PYBIND11_MODULE(pynlm_utils, m) {
    m.doc() = R"pbdoc(
        NLM Python Utilities
        --------------------
        Utility functions for NLM simulations.
        
        Features:
        - Simulation statistics collection
        - Progress tracking
        - Configuration utilities
        - World environment setup
        - Visualization data preparation
    )pbdoc";
    
    // SimulationStats class
    py::class_<pynlm_utils::SimulationStats>(m, "SimulationStats")
        .def(py::init<>())
        .def("get_average_firing_rate", &pynlm_utils::SimulationStats::getAverageFiringRate,
             "Get average firing rate")
        .def("get_average_curiosity", &pynlm_utils::SimulationStats::getAverageCuriosity,
             "Get average curiosity level")
        .def("get_average_novelty", &pynlm_utils::SimulationStats::getAverageNovelty,
             "Get average novelty level")
        .def("get_average_prediction_error", &pynlm_utils::SimulationStats::getAveragePredictionError,
             "Get average prediction error")
        .def("get_average_neuromodulation_level", &pynlm_utils::SimulationStats::getAverageNeuromodulationLevel,
             "Get average neuromodulation level")
        .def("get_max_firing_neurons", &pynlm_utils::SimulationStats::getMaxFiringNeurons,
             "Get maximum firing neurons")
        .def("get_min_firing_neurons", &pynlm_utils::SimulationStats::getMinFiringNeurons,
             "Get minimum firing neurons")
        .def("get_time_per_step", &pynlm_utils::SimulationStats::getTimePerStep,
             "Get average time per step")
        .def("get_total_simulation_time", &pynlm_utils::SimulationStats::getTotalSimulationTime,
             "Get total simulation time")
        .def("get_total_steps", &pynlm_utils::SimulationStats::getTotalSteps,
             "Get total number of steps")
        .def("to_dict", &pynlm_utils::SimulationStats::to_dict,
             "Export statistics to Python dictionary")
        .def("clear", &pynlm_utils::SimulationStats::clear,
             "Clear all statistics");
    
    // ProgressTracker class
    py::class_<pynlm_utils::ProgressTracker>(m, "ProgressTracker")
        .def(py::init<int64_t, std::string>(),
             py::arg("total_steps"), py::arg("message"),
             "Create a progress tracker")
        .def("update", &pynlm_utils::ProgressTracker::update,
             py::arg("current_step"), py::arg("additional_info"),
             "Update progress and return completion percentage")
        .def("finish", &pynlm_utils::ProgressTracker::finish,
             "Finish progress tracking");
    
    // Config utilities namespace
    m.def_submodule("config", "Configuration utilities")
        .def("create_from_stats", &pynlm_utils::config_utils::createConfigFromStats,
             py::arg("neuron_count"), py::arg("connection_density"),
             py::arg("v_threshold"), py::arg("v_rest"), py::arg("v_reset"),
             py::arg("tau_mem"), py::arg("tau_ref"),
             "Create configuration from statistics")
        .def("validate", &pynlm_utils::config_utils::validateConfig,
             py::arg("config"), "Validate configuration")
        .def("to_dict", &pynlm_utils::config_utils::configToDict,
             py::arg("config"), "Convert configuration to dictionary")
        .def("load_from_json", &pynlm_utils::config_utils::loadConfigFromJson,
             py::arg("json"), "Load configuration from JSON string");
    
    // World utilities namespace
    m.def_submodule("world", "World utilities")
        .def("create_with_environment", &pynlm_utils::world_utils::createWorldWithEnvironment,
             py::arg("width"), py::arg("height"),
             py::arg("resources"), py::arg("hazards"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Create world with resources and hazards")
        .def("add_obstacles", &pynlm_utils::world_utils::addObstacles,
             py::arg("world"), py::arg("obstacle_positions"),
             py::arg("obstacle_radius") = 0.5,
             "Add obstacles to world");
    
    // Visualization utilities namespace
    m.def_submodule("viz", "Visualization utilities")
        .def("create_firing_rate_data", &pynlm_utils::viz_utils::createFiringRateData,
             py::arg("firing_rates"), py::arg("timestamps"),
             "Create firing rate visualization data")
        .def("create_stage_timeline", &pynlm_utils::viz_utils::createStageTimeline,
             py::arg("stages"), py::arg("timestamps"),
             "Create developmental stage timeline");
    
    // Main utility functions
    m.def("run_simulation_with_stats", [](std::shared_ptr<nlm::Brain> brain,
                                        std::shared_ptr<nlm::SimpleWorld> world,
                                        std::shared_ptr<nlm::AgentBrain> agent,
                                        int64_t num_steps,
                                        std::shared_ptr<pynlm_utils::SimulationStats> stats) {
        if (!brain->isInitialized()) {
            brain->initialize();
        }
        world->reset();
        agent->initialize(world);
        
        pynlm_utils::ProgressTracker tracker(num_steps, "NLM Simulation");
        
        for (int64_t step = 0; step < num_steps; ++step) {
            auto startTime = std::chrono::steady_clock::now();
            
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto endTime = std::chrono::steady_clock::now();
            double stepTime = std::chrono::duration<double>(endTime - startTime).count();
            
            if (stats) {
                stats->addStepStats(
                    step,
                    brain->getFiringNeuronCount(),
                    brain->getAverageFiringRate(),
                    agent->getCuriosityLevel(),
                    agent->getNoveltyLevel(),
                    agent->getPredictionError(),
                    agent->getNeuromodulationLevel(),
                    brain->getDevelopmentalStage(),
                    stepTime
                );
            }
            
            tracker.update(step, "Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                              " Curiosity: " + std::to_string(agent->getCuriosityLevel()));
        }
        
        tracker.finish();
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
           py::arg("num_steps"), py::arg("stats"),
           "Run simulation with statistics collection");
    
    m.def("run_simulation_with_stats_and_timestep", [](std::shared_ptr<nlm::Brain> brain,
                                                     std::shared_ptr<nlm::SimpleWorld> world,
                                                     std::shared_ptr<nlm::AgentBrain> agent,
                                                     int64_t num_steps,
                                                     double timestep,
                                                     std::shared_ptr<pynlm_utils::SimulationStats> stats) {
        if (!brain->isInitialized()) {
            brain->initialize();
        }
        world->reset();
        agent->initialize(world);
        
        pynlm_utils::ProgressTracker tracker(num_steps, "NLM Simulation");
        
        for (int64_t step = 0; step < num_steps; ++step) {
            auto startTime = std::chrono::steady_clock::now();
            
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto endTime = std::chrono::steady_clock::now();
            double stepTime = std::chrono::duration<double>(endTime - startTime).count();
            
            if (stats) {
                stats->addStepStats(
                    step,
                    brain->getFiringNeuronCount(),
                    brain->getAverageFiringRate(),
                    agent->getCuriosityLevel(),
                    agent->getNoveltyLevel(),
                    agent->getPredictionError(),
                    agent->getNeuromodulationLevel(),
                    brain->getDevelopmentalStage(),
                    stepTime
                );
            }
            
            tracker.update(step, "Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                              " Curiosity: " + std::to_string(agent->getCuriosityLevel()));
        }
        
        tracker.finish();
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
           py::arg("num_steps"), py::arg("timestep"), py::arg("stats"),
           "Run simulation with statistics collection and custom timestep");
}

#endif // PYNLM_PYTHON_UTILS_H

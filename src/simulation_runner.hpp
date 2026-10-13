// SimulationRunner class header
// This file provides the header declaration for the SimulationRunner class

#ifndef SIMULATION_RUNNER_HPP
#define SIMULATION_RUNNER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

// NLM includes
#include "../src/advanced_api.hpp"

/**
 * @brief Configurable simulation runner for multiple agents and scenarios
 */
class SimulationRunner {
private:
    std::vector<std::shared_ptr<SimpleAgent>> agents_;
    std::shared_ptr<SimpleWorld> world_;
    size_t numSteps_;
    float dt_;
    bool renderEnabled_;
    std::string outputDirectory_;
    bool isRunning_ = false;
    std::function<void(size_t)> stepCallback_;
    std::function<void(const ExperimentResult&)> episodeCallback_;
    size_t experimentId_;
    
public:
    SimulationRunner(size_t numSteps = 1000, float dt = 1.0f);
    
    ~SimulationRunner() = default;
    
    // Agent management
    void addAgent(std::shared_ptr<SimpleAgent> agent);
    void removeAgent(const std::string& agentId);
    std::shared_ptr<SimpleAgent> getAgent(const std::string& agentId) const;
    std::vector<std::shared_ptr<SimpleAgent>> getAllAgents() const;
    
    // World setup
    void setWorld(std::shared_ptr<SimpleWorld> world);
    std::shared_ptr<SimpleWorld> getWorld() const;
    
    // Configuration
    void configure(const StringMap& config);
    StringMap getConfig() const;
    
    // Simulation control
    void start();
    void stop();
    bool isRunning() const;
    void run();
    void step();
    
    // Output and visualization
    void enableRendering(bool enable);
    bool isRenderingEnabled() const;
    void setOutputDirectory(const std::string& directory);
    void saveResults(const std::string& prefix = "simulation");
    
    // Statistics and analysis
    std::map<std::string, double> getStatistics() const;
    FloatMatrix getLearningCurve() const;
    std::map<std::string, FloatArray> getAgentMetrics() const;
    
    // Callbacks
    void setStepCallback(std::function<void(size_t)> callback);
    void setEpisodeCallback(std::function<void(const ExperimentResult&)> callback);
};

#endif // SIMULATION_RUNNER_HPP

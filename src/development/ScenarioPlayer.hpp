#pragma once

#include "Brain.hpp"
#include "BrainDebugger.hpp"
#include "BrainSimulator.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <deque>
#include <algorithm>

namespace nlm {

class ScenarioPlayer {
public:
    ScenarioPlayer(std::shared_ptr<Brain> brain, std::shared_ptr<BrainDebugger> debugger,
                  std::shared_ptr<BrainSimulator> simulator);
    ~ScenarioPlayer();
    
    // Disable copying, enable moving
    ScenarioPlayer(const ScenarioPlayer&) = delete;
    ScenarioPlayer& operator=(const ScenarioPlayer&) = delete;
    ScenarioPlayer(ScenarioPlayer&&) noexcept = default;
    ScenarioPlayer& operator=(ScenarioPlayer&&) noexcept = default;
    
    // Initialization and shutdown
    bool initialize();
    void shutdown();
    
    // Scenario management
    struct Scenario {
        std::string name;
        std::string description;
        std::string author;
        std::string version;
        double duration;
        size_t maxSteps;
        double timestep;
        std::string startCondition;
        std::string endCondition;
        std::vector<SimulationStep> initialState;
        std::vector<SimulationVariable> variables;
        std::vector<ScenarioEvent> events;
        std::vector<ScenarioAction> actions;
        std::vector<ScenarioCondition> conditions;
        
        Scenario()
            : name("Unnamed Scenario"), description(""), author(""), version("1.0"),
              duration(0.0), maxSteps(1000), timestep(0.01),
              startCondition(""), endCondition("") {}
    };
    
    // Scenario API
    bool loadScenario(const std::string& filename);
    bool saveScenario(const std::string& filename) const;
    bool createScenario(const Scenario& scenario);
    bool createScenario(const std::string& name, const std::string& description);
    bool createStandardScenario(const std::string& type);
    bool createLearningScenario(const std::string& learningType);
    bool createMemoryScenario(const std::string& memoryType);
    bool createPlasticityScenario(const std::string& plasticityType);
    
    // Scenario execution
    bool playScenario(const std::string& name);
    bool playScenario(const Scenario& scenario);
    bool pauseScenario();
    bool resumeScenario();
    bool stopScenario();
    bool stepScenario();
    bool stepScenario(double timestep);
    bool resetScenario();
    
    // Scenario status
    bool isScenarioPlaying() const;
    bool isScenarioPaused() const;
    bool isScenarioStopped() const;
    std::string getCurrentScenarioName() const;
    Scenario getCurrentScenario() const;
    ScenarioState getCurrentScenarioState() const;
    
    // Scenario data management
    bool saveScenarioData(const std::string& filename);
    bool loadScenarioData(const std::string& filename);
    std::vector<ScenarioData> getScenarioHistory() const;
    bool deleteScenario(const std::string& name);
    
    // Scenario analysis
    void analyzeScenario();
    void analyzeScenarioPerformance();
    void analyzeScenarioLearning();
    void analyzeScenarioMemory();
    void analyzeScenarioPlasticity();
    
    // Scenario comparison
    void compareScenarios(const std::vector<std::string>& scenarioNames);
    void compareScenarioResults(const std::vector<ScenarioResult>& results);
    
    // Scenario configuration
    bool addScenarioVariable(const SimulationVariable& variable);
    bool modifyScenarioVariable(const std::string& name, float value);
    bool removeScenarioVariable(const std::string& name);
    std::vector<SimulationVariable> getScenarioVariables() const;
    
    bool addScenarioEvent(const ScenarioEvent& event);
    bool modifyScenarioEvent(size_t eventId, const ScenarioEvent& newEvent);
    bool removeScenarioEvent(size_t eventId);
    std::vector<ScenarioEvent> getScenarioEvents() const;
    
    // Scenario validation
    bool validateScenario(const Scenario& scenario) const;
    bool validateScenarioVariables() const;
    bool validateScenarioEvents() const;
    bool validateScenarioPerformance() const;
    std::vector<std::string> getScenarioValidationErrors() const;
    
    // Scenario replication
    bool replicateScenario(const std::string& sourceScenario, const std::string& targetScenario);
    bool cloneScenario(const std::string& sourceScenario);
    bool modifyScenario(const std::string& scenarioName, const Scenario& newScenario);
    
    // Scenario metadata
    bool setScenarioMetadata(const std::string& key, const std::string& value);
    std::string getScenarioMetadata(const std::string& key) const;
    std::unordered_map<std::string, std::string> getScenarioMetadata() const;
    
    // Scenario statistics
    struct ScenarioStatistics {
        std::string scenarioName;
        double duration;
        size_t totalSteps;
        double finalAccuracy;
        double finalLoss;
        double convergenceTime;
        float finalLearningRate;
        float totalReward;
        size_t memoryUsage;
        double cpuUsage;
        
        ScenarioStatistics()
            : scenarioName(""), duration(0.0), totalSteps(0), finalAccuracy(0.0),
              finalLoss(1.0), convergenceTime(0.0), finalLearningRate(0.01f),
              totalReward(0.0f), memoryUsage(0), cpuUsage(0.0) {}
    };
    
    ScenarioStatistics getScenarioStatistics(const std::string& scenarioName) const;
    std::vector<ScenarioStatistics> getAllScenarioStatistics() const;
    
    // Configuration
    void loadScenarioConfiguration(const std::string& configFile);
    void saveScenarioConfiguration() const;
    void resetScenarioConfiguration();
    Scenario getCurrentScenarioConfiguration() const;
    
    // Thread safety
    void lockScenarioPlayer() const;
    void unlockScenarioPlayer() const;
    bool tryLockScenarioPlayer() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<BrainDebugger> debugger_;
    std::shared_ptr<BrainSimulator> simulator_;
    std::shared_ptr<Config> config_;
    
    mutable std::mutex playerMutex_;
    std::atomic<bool> isPlaying_;
    std::atomic<bool> isPaused_;
    std::atomic<bool> isStopped_;
    std::thread scenarioThread_;
    
    // Scenario state
    std::string currentScenarioName_;
    Scenario currentScenario_;
    ScenarioState currentScenarioState_;
    
    // Scenario data storage
    std::unordered_map<std::string, Scenario> scenarioConfigs_;
    std::unordered_map<std::string, ScenarioResult> scenarioResults_;
    std::unordered_map<std::string, std::vector<ScenarioData>> scenarioHistory_;
    
    // Scenario variables and events
    std::vector<SimulationVariable> scenarioVariables_;
    std::vector<ScenarioEvent> scenarioEvents_;
    
    // Scenario metadata
    std::unordered_map<std::string, std::string> scenarioMetadata_;
    
    // Internal helper methods
    void cleanup();
    void playScenarioLoop(const Scenario& scenario);
    void executeScenarioStep(double timestep);
    void updateScenarioState();
    void processScenarioEvents();
    void executeScenarioEvent(const ScenarioEvent& event);
    void evaluateScenarioConditions();
    ScenarioState captureScenarioState() const;
    void restoreScenarioState(const ScenarioState& state);
    void updateScenarioStatistics();
    void logScenarioStep();
    void exportScenarioData() const;
    void generateScenarioReports() const;
    void validateScenarioSetup() const;
    void setupScenarioEnvironment();
    void cleanupScenarioEnvironment();
    void handleScenarioError(const std::string& error);
    void syncWithDebugger();
};

} // namespace nlm
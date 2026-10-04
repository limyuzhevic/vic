#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <atomic>
#include <map>
#include <functional>
#include <unordered_map>

// Include all NLM headers
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
namespace nlm {

// Python-friendly type aliases
using ConfigPtr = std::shared_ptr<Config>;
using BrainPtr = std::shared_ptr<Brain>;
using AgentBrainPtr = std::shared_ptr<AgentBrain>;
using SimpleWorldPtr = std::shared_ptr<SimpleWorld>;
using VisionPtr = std::shared_ptr<Vision>;
using AudioPtr = std::shared_ptr<Audio>;
using InternalSignalsPtr = std::shared_ptr<InternalSignals>;
using ActionPtr = std::shared_ptr<Action>;
using WorldObjectPtr = std::shared_ptr<WorldObject>;
using AgentBodyPtr = std::shared_ptr<AgentBody>;
using SensoryPerceptPtr = std::shared_ptr<SensoryPercept>;

// Advanced Python API exception classes
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation Error: " + msg) {}
};

class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& msg) : NLMException("Memory Error: " + msg) {}
};

class PluginError : public NLMException {
public:
    explicit PluginError(const std::string& msg) : NLMException("Plugin Error: " + msg) {}
};

// Advanced Simulation Controller with Event System
class AdvancedSimulationController {
public:
    AdvancedSimulationController(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : brain_(brain), config_(config), running_(false), timestep_(0.001), 
          maxSteps_(0), currentStep_(0), currentTime_(0.0), speed_(1.0f),
          paused_(false), eventCount_(0) {
        
        // Setup default event handlers
        setupDefaultEventHandlers();
        
        // Setup performance monitoring
        setupPerformanceMonitoring();
        
        // Setup memory monitoring
        setupMemoryMonitoring();
    }
    
    void start() {
        if (!brain_ || !brain_->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
        
        running_ = true;
        paused_ = false;
        currentStep_ = 0;
        currentTime_ = 0.0;
        
        // Record start event
        recordEvent(SimulationEvent::Type::SimulationStarted,
                   currentTime_, "Simulation started");
        
        // Start background performance monitoring
        startPerformanceMonitoring();
    }
    
    void stop() {
        if (!running_) return;
        
        running_ = false;
        paused_ = true;
        
        // Record stop event
        recordEvent(SimulationEvent::Type::SimulationStopped,
                   currentTime_, "Simulation stopped");
        
        stopPerformanceMonitoring();
    }
    
    bool isRunning() const { return running_ && !paused_; }
    bool isPaused() const { return paused_; }
    
    void pause() {
        if (running_) {
            paused_ = true;
            recordEvent(SimulationEvent::Type::SimulationPaused,
                       currentTime_, "Simulation paused");
        }
    }
    
    void resume() {
        if (paused_) {
            paused_ = false;
            recordEvent(SimulationEvent::Type::SimulationResumed,
                       currentTime_, "Simulation resumed");
        }
    }
    
    void step() {
        if (!running_ || paused_) {
            throw SimulationError("Simulation not running or paused");
        }
        
        // Record event before step
        recordEvent(SimulationEvent::Type::StepStarting,
                   currentTime_, "Starting simulation step " + std::to_string(currentStep_));
        
        // Execute simulation step
        brain_->step(currentStep_, currentTime_);
        
        // Update time
        currentTime_ += timestep_;
        currentStep_++;
        
        // Record completion event
        recordEvent(SimulationEvent::Type::StepCompleted,
                   currentTime_, "Step " + std::to_string(currentStep_ - 1) + " completed");
        
        // Check for learning events
        checkForLearningEvents();
        
        // Check for memory events
        checkForMemoryEvents();
        
        // Check for developmental events
        checkForDevelopmentalEvents();
        
        // Update performance metrics
        updatePerformanceMetrics();
    }
    
    void runSteps(int steps) {
        if (!running_ || paused_) {
            throw SimulationError("Simulation not running or paused");
        }
        
        int originalSteps = steps;
        while (steps > 0 && running_ && !paused_) {
            step();
            steps--;
            
            // Check for early termination
            if (shouldTerminate()) {
                break;
            }
        }
        
        if (originalSteps - steps > 0) {
            recordEvent(SimulationEvent::Type::SimulationStopped,
                       currentTime_, "Simulation stopped after " + std::to_string(originalSteps - steps) + " steps");
        }
    }
    
    void runForTime(double duration) {
        int steps = static_cast<int>(duration / timestep_);
        runSteps(steps);
    }
    
    void runUntilCondition(std::function<bool()> condition) {
        while (running_ && !paused_ && !condition()) {
            step();
            
            // Check for timeout
            if (currentTime_ > maxRuntime_) {
                recordEvent(SimulationEvent::Type::SimulationTimeout,
                           currentTime_, "Simulation timeout reached");
                break;
            }
        }
    }
    
    void setTimestep(double timestep) {
        timestep_ = timestep;
        config_->set("simulation_timestep", timestep, ConfigSource::Runtime);
    }
    
    double getTimestep() const { return timestep_; }
    
    int getCurrentStep() const { return currentStep_; }
    double getCurrentTime() const { return currentTime_; }
    
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    void setMaxSteps(int maxSteps) {
        maxSteps_ = maxSteps;
    }
    
    int getMaxSteps() const { return maxSteps_; }
    
    void setMaxRuntime(double maxRuntime) {
        maxRuntime_ = maxRuntime;
    }
    
    double getMaxRuntime() const { return maxRuntime_; }
    
    void setSpeed(float speed) {
        speed_ = std::max(0.1f, std::min(speed, 10.0f)); // Clamp between 0.1x and 10x
    }
    
    float getSpeed() const { return speed_; }
    
    double getSimulationTime() const { return currentTime_; }
    
    // Neuromodulation control
    void enableNeuromodulation(bool enable) {
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(brain_);
        if (agentBrain) {
            agentBrain->enableRewardModulation(enable);
            agentBrain->enableCuriosity(enable);
            agentBrain->enableNovelty(enable);
        }
    }
    
    void setNeuromodulationParameters(float dopamine, float curiosity, float novelty) {
        config_->set("dopamine_strength", dopamine, ConfigSource::Runtime);
        config_->set("curiosity_threshold", curiosity, ConfigSource::Runtime);
        config_->set("novelty_decay", novelty, ConfigSource::Runtime);
    }
    
    // Development control
    void enableDevelopment(bool enable) {
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(brain_);
        if (agentBrain) {
            agentBrain->enableDevelopment(enable);
        }
    }
    
    void setDevelopmentalStage(DevelopmentalStage stage) {
        brain_->setDevelopmentalStage(stage);
    }
    
    // Memory management
    void enableReplay(bool enabled) {
        config_->set("replay_enabled", enabled, ConfigSource::Runtime);
    }
    
    bool isReplayEnabled() const {
        return config_->getOr<bool>("replay_enabled", true);
    }
    
    void setReplayFrequency(float frequency) {
        config_->set("replay_frequency", frequency, ConfigSource::Runtime);
    }
    
    void enableMemoryConsolidation(bool enabled) {
        config_->set("memory_consolidation_enabled", enabled, ConfigSource::Runtime);
    }
    
    // Checkpoint persistence
    void saveCheckpoint(const std::string& filepath) {
        if (!brain_->save(filepath)) {
            throw SimulationError("Failed to save checkpoint to " + filepath);
        }
        
        recordEvent(SimulationEvent::Type::CheckpointSaved,
                   currentTime_, "Checkpoint saved to " + filepath);
    }
    
    void loadCheckpoint(const std::string& filepath) {
        if (!brain_->load(filepath)) {
            throw SimulationError("Failed to load checkpoint from " + filepath);
        }
        
        // Reset controller state after loading
        currentStep_ = 0;
        currentTime_ = 0.0;
        
        recordEvent(SimulationEvent::Type::CheckpointLoaded,
                   currentTime_, "Checkpoint loaded from " + filepath);
    }
    
    // Event system
    void addEventHandler(SimulationEvent::Type eventType, std::function<void(const SimulationEvent&)> handler) {
        eventHandlers_[eventType].push_back(handler);
    }
    
    void removeEventHandler(SimulationEvent::Type eventType, std::function<void(const SimulationEvent&)> handler) {
        auto it = eventHandlers_[eventType].begin();
        while (it != eventHandlers_[eventType].end()) {
            if (*it == handler) {
                it = eventHandlers_[eventType].erase(it);
            } else {
                ++it;
            }
        }
    }
    
    std::vector<SimulationEvent> getEvents(SimulationEvent::Type eventType) const {
        std::vector<SimulationEvent> filteredEvents;
        for (const auto& event : recentEvents_) {
            if (event.getType() == eventType) {
                filteredEvents.push_back(event);
            }
        }
        return filteredEvents;
    }
    
    std::vector<SimulationEvent> getAllEvents() const {
        return recentEvents_;
    }
    
    void clearEvents() {
        recentEvents_.clear();
    }
    
    void setEventFilter(SimulationEvent::Type eventType, bool enabled) {
        eventFilter_[eventType] = enabled;
    }
    
    bool isEventEnabled(SimulationEvent::Type eventType) const {
        return eventFilter_.find(eventType) == eventFilter_.end() || eventFilter_.at(eventType);
    }
    
    // Memory manager integration
    class MemoryManager {
    public:
        MemoryManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
        
        size_t getWorkingMemorySize() const {
            return brain_->getWorkingMemory()->getCapacity();
        }
        
        void setWorkingMemorySize(size_t capacity) {
            brain_->getWorkingMemory()->setCapacity(capacity);
        }
        
        size_t getEpisodicMemorySize() const {
            return brain_->getEpisodicMemory()->getBufferSize();
        }
        
        void setEpisodicMemorySize(size_t capacity) {
            brain_->getEpisodicMemory()->setBufferSize(capacity);
        }
        
        std::vector<float> getWorkingMemory() const {
            return brain_->getWorkingMemory()->getContents();
        }
        
        void setWorkingMemory(const std::vector<float>& contents) {
            brain_->getWorkingMemory()->setContents(contents);
        }
        
        void clearMemory() {
            brain_->getWorkingMemory()->clear();
            brain_->getEpisodicMemory()->clear();
        }
        
        double getMemoryUsage() const {
            return brain_->getWorkingMemory()->getUsage() + brain_->getEpisodicMemory()->getUsage();
        }
        
    private:
        std::shared_ptr<Brain> brain_;
    };
    
    MemoryManager getMemoryManager() {
        return MemoryManager(brain_);
    }
    
    // Performance monitoring
    struct PerformanceMetrics {
        double cpuUsage{0.0};
        double memoryUsage{0.0};
        double simulationTime{0.0};
        int stepsPerSecond{0};
        double learningRate{0.0};
        double noveltyLevel{0.0};
        double neuromodulationActivity{0.0};
    };
    
    PerformanceMetrics getPerformanceMetrics() const {
        return currentMetrics_;
    }
    
    std::string getPerformanceReport() const {
        std::stringstream ss;
        ss << "Performance Report:\n";
        ss << "  CPU Usage: " << (currentMetrics_.cpuUsage * 100) << "%\n";
        ss << "  Memory Usage: " << (currentMetrics_.memoryUsage * 100) << "%\n";
        ss << "  Simulation Time: " << currentMetrics_.simulationTime << "s\n";
        ss << "  Steps/Second: " << currentMetrics_.stepsPerSecond << "\n";
        ss << "  Learning Rate: " << currentMetrics_.learningRate << "\n";
        ss << "  Novelty Level: " << currentMetrics_.noveltyLevel << "\n";
        ss << "  Neuromodulation Activity: " << currentMetrics_.neuromodulationActivity << "\n";
        return ss.str();
    }
    
    // Advanced configuration
    void validateConfiguration() {
        // Validate configuration parameters
        if (timestep_ <= 0.0) {
            throw ConfigurationError("Timestep must be positive");
        }
        
        if (maxSteps_ < 0) {
            throw ConfigurationError("Max steps cannot be negative");
        }
        
        if (maxRuntime_ <= 0.0) {
            throw ConfigurationError("Max runtime must be positive");
        }
        
        if (speed_ <= 0.0) {
            throw ConfigurationError("Speed must be positive");
        }
    }
    
    std::string getConfigurationReport() const {
        std::stringstream ss;
        ss << "Configuration Report:\n";
        ss << "  Simulation Timestep: " << timestep_ << "s\n";
        ss << "  Current Step: " << currentStep_ << "\n";
        ss << "  Current Time: " << currentTime_ << "s\n";
        ss << "  Max Steps: " << maxSteps_ << "\n";
        ss << "  Max Runtime: " << maxRuntime_ << "s\n";
        ss << "  Speed: " << speed_ << "x\n";
        ss << "  Running: " << (running_ ? "Yes" : "No") << "\n";
        ss << "  Paused: " << (paused_ ? "Yes" : "No") << "\n";
        ss << "  Event Count: " << eventCount_ << "\n";
        ss << "  Memory Usage: " << getMemoryManager().getMemoryUsage() << "\n";
        return ss.str();
    }
    
    // Plugin system
    class Plugin {
    public:
        virtual ~Plugin() = default;
        virtual std::string getName() const = 0;
        virtual std::string getVersion() const = 0;
        virtual void initialize(std::shared_ptr<AdvancedSimulationController> controller) = 0;
        virtual void shutdown() = 0;
        virtual void onEvent(const SimulationEvent& event) = 0;
        virtual bool onStep() = 0;
        virtual void onConfigurationChanged() = 0;
    };
    
    void loadPlugin(std::shared_ptr<Plugin> plugin) {
        if (!plugin) {
            throw PluginError("Plugin cannot be null");
        }
        
        try {
            plugin->initialize(shared_from_this());
            loadedPlugins_.push_back(plugin);
            recordEvent(SimulationEvent::Type::PluginLoaded,
                       currentTime_, "Plugin loaded: " + plugin->getName() + " v" + plugin->getVersion());
        } catch (...) {
            throw PluginError("Failed to load plugin: " + plugin->getName());
        }
    }
    
    void unloadPlugin(std::shared_ptr<Plugin> plugin) {
        auto it = std::find(loadedPlugins_.begin(), loadedPlugins_.end(), plugin);
        if (it != loadedPlugins_.end()) {
            try {
                (*it)->shutdown();
                loadedPlugins_.erase(it);
                recordEvent(SimulationEvent::Type::PluginUnloaded,
                           currentTime_, "Plugin unloaded: " + (*it)->getName());
            } catch (...) {
                throw PluginError("Failed to unload plugin: " + (*it)->getName());
            }
        }
    }
    
    std::vector<std::shared_ptr<Plugin>> getLoadedPlugins() const {
        return loadedPlugins_;
    }
    
protected:
    void recordEvent(SimulationEvent::Type type, double timestamp, const std::string& message) {
        SimulationEvent event(type, timestamp, message);
        recentEvents_.push_back(event);
        eventCount_++;
        
        // Notify registered handlers
        if (eventFilter_.find(type) == eventFilter_.end() || eventFilter_.at(type)) {
            for (const auto& handler : eventHandlers_[type]) {
                try {
                    handler(event);
                } catch (const std::exception& e) {
                    // Log error but don't crash
                    std::cerr << "Error in event handler: " << e.what() << std::endl;
                }
            }
        }
        
        // Notify loaded plugins
        for (const auto& plugin : loadedPlugins_) {
            try {
                plugin->onEvent(event);
            } catch (const std::exception& e) {
                std::cerr << "Error in plugin event handler: " << e.what() << std::endl;
            }
        }
    }
    
    void setupDefaultEventHandlers() {
        // Add default event handlers
        addEventHandler(SimulationEvent::Type::StepCompleted, 
                       [this](const SimulationEvent& event) {
                           this->onStepCompleted(event);
                       });
        
        addEventHandler(SimulationEvent::Type::LearningDetected,
                       [this](const SimulationEvent& event) {
                           this->onLearningDetected(event);
                       });
        
        addEventHandler(SimulationEvent::Type::MemoryUpdated,
                       [this](const SimulationEvent& event) {
                           this->onMemoryUpdated(event);
                       });
    }
    
    void setupPerformanceMonitoring() {
        performanceTimer_.start();
    }
    
    void startPerformanceMonitoring() {
        performanceTimer_.start();
    }
    
    void stopPerformanceMonitoring() {
        performanceTimer_.stop();
        currentMetrics_.simulationTime = performanceTimer_.getElapsedTime();
    }
    
    void updatePerformanceMetrics() {
        currentMetrics_.cpuUsage = performanceTimer_.getCpuUsage();
        currentMetrics_.simulationTime = performanceTimer_.getElapsedTime();
        currentMetrics_.stepsPerSecond = currentStep_ / std::max(1.0, currentMetrics_.simulationTime);
        
        // Update memory usage
        currentMetrics_.memoryUsage = getMemoryManager().getMemoryUsage();
        
        // Update neuromodulation activity
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(brain_);
        if (agentBrain) {
            currentMetrics_.neuromodulationActivity = agentBrain->getNeuromodulationLevel();
            currentMetrics_.noveltyLevel = agentBrain->getNoveltyLevel();
            currentMetrics_.learningRate = agentBrain->getCuriosityLevel();
        }
    }
    
    void setupMemoryMonitoring() {
        memoryMonitor_.start();
    }
    
    void checkForLearningEvents() {
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(brain_);
        if (agentBrain) {
            float curiosityLevel = agentBrain->getCuriosityLevel();
            float predictionError = agentBrain->getPredictionError();
            
            if (curiosityLevel > 0.5f || std::abs(predictionError) > 0.3f) {
                recordEvent(SimulationEvent::Type::LearningDetected,
                           currentTime_, "Learning detected - curiosity: " + std::to_string(curiosityLevel) +
                               ", prediction error: " + std::to_string(predictionError));
            }
        }
    }
    
    void checkForMemoryEvents() {
        auto memoryManager = getMemoryManager();
        double memoryUsage = memoryManager.getMemoryUsage();
        
        if (memoryUsage > 0.8) {
            recordEvent(SimulationEvent::Type::MemoryUpdated,
                       currentTime_, "High memory usage detected: " + std::to_string(memoryUsage));
        }
        
        if (memoryUsage < 0.2) {
            recordEvent(SimulationEvent::Type::MemoryUpdated,
                       currentTime_, "Low memory usage detected: " + std::to_string(memoryUsage));
        }
    }
    
    void checkForDevelopmentalEvents() {
        auto agentBrain = std::dynamic_pointer_cast<AgentBrain>(brain_);
        if (agentBrain) {
            DevelopmentalStage currentStage = agentBrain->getDevelopmentalStage();
            float developmentalAge = agentBrain->getBrain()->getDevelopmentalAge();
            
            if (developmentalAge > 60.0 && developmentalAge < 120.0) {
                recordEvent(SimulationEvent::Type::DevelopmentStageChanged,
                           currentTime_, "Developmental stage change: " + std::to_string(static_cast<int>(currentStage)) +
                               " at age " + std::to_string(developmentalAge));
            }
        }
    }
    
    void onStepCompleted(const SimulationEvent& event) {
        // Check if step execution should continue
        if (shouldTerminate()) {
            stop();
        }
    }
    
    void onLearningDetected(const SimulationEvent& event) {
        // Record learning event
    }
    
    void onMemoryUpdated(const SimulationEvent& event) {
        // Record memory update event
    }
    
    bool shouldTerminate() const {
        return currentStep_ >= maxSteps_ || currentTime_ >= maxRuntime_;
    }
    
    bool isPluginLoaded(const std::string& pluginName) const {
        for (const auto& plugin : loadedPlugins_) {
            if (plugin->getName() == pluginName) {
                return true;
            }
        }
        return false;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    bool running_;
    bool paused_;
    double timestep_;
    int maxSteps_;
    double currentStep_;
    double currentTime_;
    double maxSteps_;
    double maxRuntime_;
    float speed_;
    int eventCount_;
    
    std::vector<SimulationEvent> recentEvents_;
    std::map<SimulationEvent::Type, std::list<std::function<void(const SimulationEvent&)>>> eventHandlers_;
    std::map<SimulationEvent::Type, bool> eventFilter_;
    
    PerformanceMetrics currentMetrics_;
    Timer performanceTimer_;
    MemoryMonitor memoryMonitor_;
    
    std::vector<std::shared_ptr<Plugin>> loadedPlugins_;
    
    friend class Timer;
    friend class MemoryMonitor;
};

// Timer for performance monitoring
class Timer {
public:
    Timer() : startTime_(0), elapsedTime_(0), running_(false) {}
    
    void start() {
        if (!running_) {
            startTime_ = std::chrono::high_resolution_clock::now();
            running_ = true;
        }
    }
    
    void stop() {
        if (running_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            elapsedTime_ += std::chrono::duration<double>(endTime - startTime_).count();
            running_ = false;
        }
    }
    
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
        elapsedTime_ = 0.0;
        running_ = false;
    }
    
    double getElapsedTime() const {
        if (running_) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            return elapsedTime_ + std::chrono::duration<double>(currentTime - startTime_).count();
        }
        return elapsedTime_;
    }
    
    double getCpuUsage() const {
        // Simplified CPU usage estimation
        return 0.5; // Placeholder
    }
    
    bool isRunning() const { return running_; }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    double elapsedTime_;
    bool running_;
};

// Memory monitor for memory tracking
class MemoryMonitor {
public:
    MemoryMonitor() : tracking_(false) {}
    
    void start() {
        if (!tracking_) {
            tracking_ = true;
        }
    }
    
    void stop() {
        tracking_ = false;
    }
    
    double getMemoryUsage() const {
        // Simplified memory usage estimation
        return 0.3; // Placeholder
    }
    
    bool isTracking() const { return tracking_; }
    
private:
    bool tracking_;
};

// Advanced Python API functions for simulation control
py::class_<AdvancedSimulationController>(m, "AdvancedSimulationController", R"pbdoc(Advanced simulation controller with event system)")
    .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<Config>>(), py::arg("brain"), py::arg("config"))
    .def("start", &AdvancedSimulationController::start, "Start the simulation")
    .def("stop", &AdvancedSimulationController::stop, "Stop the simulation")
    .def("pause", &AdvancedSimulationController::pause, "Pause the simulation")
    .def("resume", &AdvancedSimulationController::resume, "Resume the simulation")
    .def("isRunning", &AdvancedSimulationController::isRunning, "Check if simulation is running")
    .def("isPaused", &AdvancedSimulationController::isPaused, "Check if simulation is paused")
    .def("step", &AdvancedSimulationController::step, "Advance simulation by one step")
    .def("runSteps", &AdvancedSimulationController::runSteps, py::arg("steps"), "Run simulation for specified number of steps")
    .def("runForTime", &AdvancedSimulationController::runForTime, py::arg("duration"), "Run simulation for specified duration")
    .def("runUntilCondition", &AdvancedSimulationController::runUntilCondition, py::arg("condition"), "Run simulation until condition is met")
    .def("setTimestep", &AdvancedSimulationController::setTimestep, py::arg("timestep"), "Set simulation timestep")
    .def("getTimestep", &AdvancedSimulationController::getTimestep, "Get current timestep")
    .def("getCurrentStep", &AdvancedSimulationController::getCurrentStep, "Get current step")
    .def("getCurrentTime", &AdvancedSimulationController::getCurrentTime, "Get current time")
    .def("getSimulationTime", &AdvancedSimulationController::getSimulationTime, "Get current simulation time")
    .def("setMaxSteps", &AdvancedSimulationController::setMaxSteps, py::arg("maxSteps"), "Set maximum steps")
    .def("getMaxSteps", &AdvancedSimulationController::getMaxSteps, "Get maximum steps")
    .def("setMaxRuntime", &AdvancedSimulationController::setMaxRuntime, py::arg("maxRuntime"), "Set maximum runtime")
    .def("getMaxRuntime", &AdvancedSimulationController::getMaxRuntime, "Get maximum runtime")
    .def("setSpeed", &AdvancedSimulationController::setSpeed, py::arg("speed"), "Set simulation speed")
    .def("getSpeed", &AdvancedSimulationController::getSpeed, "Get simulation speed")
    .def("saveCheckpoint", &AdvancedSimulationController::saveCheckpoint, py::arg("filepath"), "Save checkpoint to file")
    .def("loadCheckpoint", &AdvancedSimulationController::loadCheckpoint, py::arg("filepath"), "Load checkpoint from file")
    .def("getBrain", &AdvancedSimulationController::getBrain, "Get brain instance")
    .def("getConfig", &AdvancedSimulationController::getConfig, "Get configuration")
    .def("enableNeuromodulation", &AdvancedSimulationController::enableNeuromodulation, py::arg("enable"), "Enable neuromodulation")
    .def("setNeuromodulationParameters", &AdvancedSimulationController::setNeuromodulationParameters,
         py::arg("dopamine"), py::arg("curiosity"), py::arg("novelty"),
         "Set neuromodulation parameters")
    .def("enableDevelopment", &AdvancedSimulationController::enableDevelopment, py::arg("enable"), "Enable development")
    .def("setDevelopmentalStage", &AdvancedSimulationController::setDevelopmentalStage,
         py::arg("stage"), "Set developmental stage")
    .def("enableReplay", &AdvancedSimulationController::enableReplay, py::arg("enabled"), "Enable replay")
    .def("isReplayEnabled", &AdvancedSimulationController::isReplayEnabled, "Check if replay is enabled")
    .def("setReplayFrequency", &AdvancedSimulationController::setReplayFrequency, py::arg("frequency"), "Set replay frequency")
    .def("enableMemoryConsolidation", &AdvancedSimulationController::enableMemoryConsolidation,
         py::arg("enabled"), "Enable memory consolidation")
    .def("getMemoryManager", &AdvancedSimulationController::getMemoryManager, "Get memory manager")
    .def("getPerformanceMetrics", &AdvancedSimulationController::getPerformanceMetrics, "Get performance metrics")
    .def("getPerformanceReport", &AdvancedSimulationController::getPerformanceReport, "Get performance report")
    .def("getConfigurationReport", &AdvancedSimulationController::getConfigurationReport, "Get configuration report")
    .def("validateConfiguration", &AdvancedSimulationController::validateConfiguration, "Validate configuration")
    .def("addEventHandler", &AdvancedSimulationController::addEventHandler,
         py::arg("eventType"), py::arg("handler"), "Add event handler")
    .def("removeEventHandler", &AdvancedSimulationController::removeEventHandler,
         py::arg("eventType"), py::arg("handler"), "Remove event handler")
    .def("getEvents", &AdvancedSimulationController::getEvents, py::arg("eventType"), "Get events of specific type")
    .def("getAllEvents", &AdvancedSimulationController::getAllEvents, "Get all events")
    .def("clearEvents", &AdvancedSimulationController::clearEvents, "Clear all events")
    .def("setEventFilter", &AdvancedSimulationController::setEventFilter,
         py::arg("eventType"), py::arg("enabled"), "Set event filter")
    .def("isEventEnabled", &AdvancedSimulationController::isEventEnabled,
         py::arg("eventType"), "Check if event is enabled")
    .def("loadPlugin", &AdvancedSimulationController::loadPlugin, py::arg("plugin"), "Load plugin")
    .def("unloadPlugin", &AdvancedSimulationController::unloadPlugin, py::arg("plugin"), "Unload plugin")
    .def("getLoadedPlugins", &AdvancedSimulationController::getLoadedPlugins, "Get loaded plugins");

// Event system
py::class_<SimulationEvent>(m, "SimulationEvent", R"pbdoc(Event in simulation timeline)")
    .def(py::init<SimulationEvent::Type, double, std::string>(), 
         py::arg("type"), py::arg("timestamp"), py::arg("message") = "")
    .def("getType", &SimulationEvent::getType, "Get event type")
    .def("getTimestamp", &SimulationEvent::getTimestamp, "Get event timestamp")
    .def("getMessage", &SimulationEvent::getMessage, "Get event message");

// Plugin system
py::class_<AdvancedSimulationController::Plugin>(m, "Plugin", R"pbdoc(Base class for NLM plugins)")
    .def("getName", &AdvancedSimulationController::Plugin::getName, "Get plugin name")
    .def("getVersion", &AdvancedSimulationController::Plugin::getVersion, "Get plugin version")
    .def("onEvent", &AdvancedSimulationController::Plugin::onEvent, "Handle event")
    .def("onStep", &AdvancedSimulationController::Plugin::onStep, "Handle step event")
    .def("onConfigurationChanged", &AdvancedSimulationController::Plugin::onConfigurationChanged, "Handle configuration change");

// Memory management helper
py::class_<AdvancedSimulationController::MemoryManager>(m, "MemoryManager", R"pbdoc(Manages brain memory systems)")
    .def("getWorkingMemorySize", &AdvancedSimulationController::MemoryManager::getWorkingMemorySize, "Get working memory capacity")
    .def("setWorkingMemorySize", &AdvancedSimulationController::MemoryManager::setWorkingMemorySize, "Set working memory capacity")
    .def("getEpisodicMemorySize", &AdvancedSimulationController::MemoryManager::getEpisodicMemorySize, "Get episodic memory capacity")
    .def("setEpisodicMemorySize", &AdvancedSimulationController::MemoryManager::setEpisodicMemorySize, "Set episodic memory capacity")
    .def("clearMemory", &AdvancedSimulationController::MemoryManager::clearMemory, "Clear all memory")
    .def("getMemoryUsage", &AdvancedSimulationController::MemoryManager::getMemoryUsage, "Get memory usage");

// Simulation controller alias for backward compatibility
py::class_<AdvancedSimulationController>(m, "SimulationController", R"pbdoc(Simulation controller for NLM)")
    .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<Config>>(), py::arg("brain"), py::arg("config"));

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Advanced Version
        ------------------------------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This version provides:
        - Complete integration of all NLM systems (neuromodulation, memory, prediction, development)
        - Advanced simulation control with event-driven architecture
        - Memory management APIs
        - Event-driven callback system
        - Checkpoint persistence
        - Advanced configuration options
        - Comprehensive error handling and validation
        - Plugin system for extensibility
        - Performance monitoring
        - Memory monitoring
        
        Key features:
        • Real-time simulation control (start/stop/step/run)
        • Event-driven architecture with callback system
        • Memory system management (working/episodic memory)
        • Neuromodulation control (dopamine, curiosity, novelty)
        • Development system control
        • Checkpoint persistence for long-running simulations
        • Advanced debugging and monitoring tools
        • Plugin system for extensibility
        • Performance monitoring
        • Memory monitoring
        • Event filtering and handling
        • Configuration validation
        
        Perfect for researchers and developers needing advanced control over neural simulations.
    )pbdoc";
    
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<PluginError>(m, "PluginError");
    py::register_exception<NLMException>(m, "NLMError");
    
    // Register all the advanced classes
    py::enum_<SimulationEvent::Type>(m, "SimulationEventType")
        .value("StepCompleted", SimulationEvent::Type::StepCompleted)
        .value("LearningDetected", SimulationEvent::Type::LearningDetected)
        .value("MemoryUpdated", SimulationEvent::Type::MemoryUpdated)
        .value("NeuromodulationChanged", SimulationEvent::Type::NeuromodulationChanged)
        .value("DevelopmentStageChanged", SimulationEvent::Type::DevelopmentStageChanged)
        .value("ErrorOccurred", SimulationEvent::Type::ErrorOccurred)
        .value("CheckpointSaved", SimulationEvent::Type::CheckpointSaved)
        .value("SimulationStarted", SimulationEvent::Type::SimulationStarted)
        .value("SimulationStopped", SimulationEvent::Type::SimulationStopped)
        .value("SimulationPaused", SimulationEvent::Type::SimulationPaused)
        .value("SimulationResumed", SimulationEvent::Type::SimulationResumed)
        .value("StepStarting", SimulationEvent::Type::StepStarting)
        .value("PluginLoaded", SimulationEvent::Type::PluginLoaded)
        .value("PluginUnloaded", SimulationEvent::Type::PluginUnloaded)
        .value("CheckpointLoaded", SimulationEvent::Type::CheckpointLoaded)
        .value("SimulationTimeout", SimulationEvent::Type::SimulationTimeout);
    
    // Register factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Register constant values
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Register exception classes
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<PluginError>(m, "PluginError");
    py::register_exception<NLMException>(m, "NLMError");
}

} // namespace nlm


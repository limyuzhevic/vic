#pragma once

/**
 * Advanced debugging and profiling tools for NLM
 * Provides comprehensive debugging, profiling, and scripting capabilities
 * for power users working with the NLM brain simulation
 */

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <fstream>
#include <functional>
#include <iomanip>
#include <thread>

namespace nlm {

/**
 * Advanced debugger for NLM
 * Provides detailed inspection and debugging capabilities
 */
class NLMAdvancedDebugger {
public:
    /**
     * Debug state capture point
     */
    struct DebugState {
        uint64_t step;
        double time;
        std::string description;
        
        // Brain state
        size_t neuronCount;
        size_t synapseCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        
        // Memory state
        size_t workingMemoryTraces;
        size_t episodicEpisodes;
        
        // Agent state
        float neuromodulationLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionError;
        float learnedPreference;
        
        // World state
        float agentEnergy;
        float agentHealth;
        int resourcesConsumed;
        int hazardsHit;
        
        // Timing
        double stepTime;
        
        DebugState() : step(0), time(0.0), description(""),
                      neuronCount(0), synapseCount(0), firingNeuronCount(0),
                      averageFiringRate(0.0f), workingMemoryTraces(0),
                      episodicEpisodes(0), neuromodulationLevel(0.0f),
                      curiosityLevel(0.0f), noveltyLevel(0.0f),
                      predictionError(0.0f), learnedPreference(0.0f),
                      agentEnergy(0.0f), agentHealth(0.0f),
                      resourcesConsumed(0), hazardsHit(0), stepTime(0.0) {}
    };
    
    /**
     * Debugging session
     */
    class DebugSession {
    public:
        std::string sessionName;
        uint64_t startStep;
        double startTime;
        std::vector<DebugState> snapshots;
        std::map<std::string, std::string> variables;
        bool recording = true;
        bool autoSave = false;
        std::string savePath;
        
        DebugSession(const std::string& name) : sessionName(name), startStep(0), startTime(0.0) {}
    };
    
private:
    std::vector<DebugSession> sessions_;
    std::unordered_map<std::string, std::function<void(const DebugState&)>> callbacks_;
    std::string currentSessionName_;
    
public:
    /**
     * Start a new debugging session
     */
    void startSession(const std::string& name) {
        currentSessionName_ = name;
        DebugSession session(name);
        sessions_.push_back(session);
    }
    
    /**
     * Capture current state
     */
    void captureState(const std::shared_ptr<Brain>& brain, 
                     const std::shared_ptr<AgentBrain>& agent,
                     const std::shared_ptr<SimpleWorld>& world,
                     uint64_t step, double time,
                     const std::string& description = "") {
        DebugState state;
        state.step = step;
        state.time = time;
        state.description = description;
        
        // Capture brain state
        state.neuronCount = brain->getTotalNeuronCount();
        state.synapseCount = brain->getTotalSynapseCount();
        state.firingNeuronCount = brain->getFiringNeuronCount();
        state.averageFiringRate = brain->getAverageFiringRate();
        
        // Capture memory state
        if (brain->getWorkingMemory()) {
            state.workingMemoryTraces = brain->getWorkingMemory()->getActiveTraces();
        }
        if (brain->getEpisodicMemory()) {
            state.episodicEpisodes = brain->getEpisodicMemory()->getEpisodeCount();
        }
        
        // Capture agent state
        state.neuromodulationLevel = agent->getNeuromodulationLevel();
        state.curiosityLevel = agent->getCuriosityLevel();
        state.noveltyLevel = agent->getNoveltyLevel();
        state.predictionError = agent->getPredictionError();
        state.learnedPreference = agent->getLearnedPreference();
        
        // Capture world state
        const auto& body = world->getAgentBody();
        state.agentEnergy = body.energy;
        state.agentHealth = body.health;
        
        // Store in current session
        for (auto& session : sessions_) {
            if (session.sessionName == currentSessionName_) {
                session.snapshots.push_back(state);
                break;
            }
        }
        
        // Call registered callbacks
        for (const auto& pair : callbacks_) {
            pair.second(state);
        }
    }
    
    /**
     * Add debug callback
     */
    void addCallback(const std::string& name, std::function<void(const DebugState&)> callback) {
        callbacks_[name] = callback;
    }
    
    /**
     * Save debug session to file
     */
    bool saveSession(const std::string& sessionName, const std::string& filepath) {
        for (auto& session : sessions_) {
            if (session.sessionName == sessionName) {
                std::ofstream file(filepath);
                if (!file.is_open()) return false;
                
                // Write header
                file << "NLM Debug Session: " << sessionName << std::endl;
                file << "Format: CSV" << std::endl;
                file << "Columns: step,time,description,neuron_count,synapse_count," << std::endl;
                file << "        firing_neurons,avg_firing_rate,wm_traces,episodic_episodes," << std::endl;
                file << "        neuromodulation,curiosity,novelty,prediction_error,learned_preference," << std::endl;
                file << "        agent_energy,agent_health,resources,hazards,step_time" << std::endl;
                
                // Write data
                for (const auto& state : session.snapshots) {
                    file << state.step << "," << state.time << "," << state.description << ",";
                    file << state.neuronCount << "," << state.synapseCount << ",";
                    file << state.firingNeuronCount << "," << state.averageFiringRate << ",";
                    file << state.workingMemoryTraces << "," << state.episodicEpisodes << ",";
                    file << state.neuromodulationLevel << "," << state.curiosityLevel << ",";
                    file << state.noveltyLevel << "," << state.predictionError << ",";
                    file << state.learnedPreference << ",";
                    file << state.agentEnergy << "," << state.agentHealth << ",";
                    file << state.resourcesConsumed << "," << state.hazardsHit << ",";
                    file << state.stepTime << std::endl;
                }
                
                return true;
            }
        }
        return false;
    }
    
    /**
     * Get debug session
     */
    DebugSession* getSession(const std::string& name) {
        for (auto& session : sessions_) {
            if (session.sessionName == name) {
                return &session;
            }
        }
        return nullptr;
    }
    
    /**
     * Compare two debug sessions
     */
    std::string compareSessions(const std::string& session1, const std::string& session2) {
        auto* s1 = getSession(session1);
        auto* s2 = getSession(session2);
        
        if (!s1 || !s2) {
            return "Session not found";
        }
        
        std::stringstream result;
        result << "Comparison between " << session1 << " and " << session2 << ":" << std::endl;
        
        // Compare key metrics
        if (s1->snapshots.size() != s2->snapshots.size()) {
            result << "Warning: Different number of snapshots" << std::endl;
        }
        
        for (size_t i = 0; i < std::min(s1->snapshots.size(), s2->snapshots.size()); ++i) {
            const auto& state1 = s1->snapshots[i];
            const auto& state2 = s2->snapshots[i];
            
            result << "Snapshot " << i << " (step " << state1.step << "):" << std::endl;
            
            if (state1.neuronCount != state2.neuronCount) {
                result << "  Neuron count difference: " << state1.neuronCount - state2.neuronCount << std::endl;
            }
            if (state1.synapseCount != state2.synapseCount) {
                result << "  Synapse count difference: " << state1.synapseCount - state2.synapseCount << std::endl;
            }
            if (fabs(state1.averageFiringRate - state2.averageFiringRate) > 0.1) {
                result << "  Average firing rate difference: " << (state1.averageFiringRate - state2.averageFiringRate) << std::endl;
            }
        }
        
        return result.str();
    }
};

/**
 * Performance profiler for NLM
 * Comprehensive performance measurement and analysis
 */
class NLMProfiler {
public:
    /**
     * Profiling data point
     */
    struct ProfileData {
        std::string name;
        double duration;  // milliseconds
        uint64_t steps;
        size_t memoryUsage;
        float cpuUsage;
        std::map<std::string, double> metrics;
        
        ProfileData(const std::string& n, double dur, uint64_t steps = 0) 
            : name(n), duration(dur), steps(steps), memoryUsage(0), cpuUsage(0.0) {}
    };
    
    /**
     * Profiling session
     */
    class ProfileSession {
    public:
        std::string sessionName;
        double startTime;
        std::vector<ProfileData> measurements;
        std::map<std::string, std::vector<double>> timeSeries;
        bool recording = true;
        
        ProfileSession(const std::string& name) : sessionName(name), startTime(0.0) {}
    };
    
private:
    std::vector<ProfileSession> sessions_;
    std::chrono::high_resolution_clock::time_point sessionStart_;
    std::string currentSessionName_;
    
public:
    /**
     * Start profiling session
     */
    void startSession(const std::string& name) {
        currentSessionName_ = name;
        ProfileSession session(name);
        sessions_.push_back(session);
        sessionStart_ = std::chrono::high_resolution_clock::now();
    }
    
    /**
     * Stop profiling session
     */
    ProfileSession stopSession() {
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - sessionStart_).count();
        
        for (auto& session : sessions_) {
            if (session.sessionName == currentSessionName_) {
                session.startTime = duration;
                return session;
            }
        }
        return ProfileSession("");
    }
    
    /**
     * Record measurement
     */
    void recordMeasurement(const std::string& sessionName, const ProfileData& data) {
        for (auto& session : sessions_) {
            if (session.sessionName == sessionName) {
                session.measurements.push_back(data);
                break;
            }
        }
    }
    
    /**
     * Record time series data
     */
    void recordTimeSeries(const std::string& sessionName, const std::string& metricName, 
                         double value, uint64_t step) {
        for (auto& session : sessions_) {
            if (session.sessionName == sessionName) {
                session.timeSeries[metricName].push_back(value);
                break;
            }
        }
    }
    
    /**
     * Get profiling session
     */n inline ProfileSession* getSession(const std::string& name) {
        for (auto& session : sessions_) {
            if (session.sessionName == name) {
                return &session;
            }
        }
        return nullptr;
    }
    
    /**
     * Generate performance report
     */
    std::string generateReport(const std::string& sessionName) {
        auto* session = getSession(sessionName);
        if (!session) {
            return "Session not found";
        }
        
        std::stringstream report;
        report << "=== NLM Performance Profile: " << sessionName << " ===" << std::endl;
        report << "Total duration: " << session->startTime << " ms" << std::endl;
        report << "Number of measurements: " << session->measurements.size() << std::endl;
        
        if (!session->measurements.empty()) {
            double totalDuration = 0.0;
            uint64_t totalSteps = 0;
            
            for (const auto& m : session->measurements) {
                totalDuration += m.duration;
                totalSteps += m.steps;
            }
            
            report << "Average measurement duration: " << (totalDuration / session->measurements.size()) << " ms" << std::endl;
            report << "Total steps processed: " << totalSteps << std::endl;
            report << "Measurements per second: " << (session->measurements.size() / (session->startTime / 1000.0)) << std::endl;
        }
        
        report << std::endl;
        
        // Report time series analysis
        for (const auto& pair : session->timeSeries) {
            const auto& values = pair.second;
            if (values.empty()) continue;
            
            double avg = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
            double min = *std::min_element(values.begin(), values.end());
            double max = *std::max_element(values.begin(), values.end());
            
            report << "Metric: " << pair.first << std::endl;
            report << "  Samples: " << values.size() << std::endl;
            report << "  Average: " << avg << std::endl;
            report << "  Min: " << min << std::endl;
            report << "  Max: " << max << std::endl;
            report << "  Range: " << (max - min) << std::endl;
            report << std::endl;
        }
        
        return report.str();
    }
    
    /**
     * Compare two profiling sessions
     */
    std::string compareProfiles(const std::string& session1, const std::string& session2) {
        auto* s1 = getSession(session1);
        auto* s2 = getSession(session2);
        
        if (!s1 || !s2) {
            return "One or both sessions not found";
        }
        
        std::stringstream result;
        result << "=== Profile Comparison ===" << std::endl;
        result << "Session 1: " << session1 << std::endl;
        result << "Session 2: " << session2 << std::endl;
        result << std::endl;
        
        // Compare measurement counts
        result << "Measurement counts:" << std::endl;
        result << "  " << session1 << ": " << s1->measurements.size() << std::endl;
        result << "  " << session2 << ": " << s2->measurements.size() << std::endl;
        
        // Compare average durations
        double avgDur1 = 0.0, avgDur2 = 0.0;
        for (const auto& m : s1->measurements) avgDur1 += m.duration;
        for (const auto& m : s2->measurements) avgDur2 += m.duration;
        if (s1->measurements.size() > 0) avgDur1 /= s1->measurements.size();
        if (s2->measurements.size() > 0) avgDur2 /= s2->measurements.size();
        
        result << std::endl;
        result << "Average measurement duration:" << std::endl;
        result << "  " << session1 << ": " << avgDur1 << " ms" << std::endl;
        result << "  " << session2 << ": " << avgDur2 << " ms" << std::endl;
        
        if (avgDur1 > 0.0) {
            result << "  Difference: " << (avgDur1 - avgDur2) << " ms (" 
                   << ((avgDur2 - avgDur1) / avgDur1 * 100.0) << "%)" << std::endl;
        }
        
        return result.str();
    }
};

/**
 * Scripting engine for NLM
 * Python-style scripting for NLM control and automation
 */
class NLMScriptingEngine {
public:
    /**
     * Scripting context
     */
    class ScriptingContext {
    public:
        std::shared_ptr<Brain> brain;
        std::shared_ptr<AgentBrain> agent;
        std::shared_ptr<SimpleWorld> world;
        uint64_t currentStep;
        double currentTime;
        std::map<std::string, std::string> variables;
        std::vector<std::function<bool()>> conditions;
        
        ScriptingContext() : currentStep(0), currentTime(0.0) {}
    };
    
private:
    std::vector<ScriptingContext> contexts_;
    std::map<std::string, std::function<bool(const ScriptingContext&)>> scripts_;
    std::vector<std::string> commandHistory_;
    
public:
    /**
     * Create new scripting context
     */
    ScriptingContext createContext() {
        ScriptingContext context;
        contexts_.push_back(context);
        return context;
    }
    
    /**
     * Set context components
     */
    void setContextComponents(ScriptingContext& context, 
                             const std::shared_ptr<Brain>& brain,
                             const std::shared_ptr<AgentBrain>& agent,
                             const std::shared_ptr<SimpleWorld>& world) {
        context.brain = brain;
        context.agent = agent;
        context.world = world;
    }
    
    /**
     * Add script
     */
    void addScript(const std::string& name, std::function<bool(const ScriptingContext&)> script) {
        scripts_[name] = script;
    }
    
    /**
     * Execute script
     */
    bool executeScript(const std::string& name, ScriptingContext& context) {
        auto it = scripts_.find(name);
        if (it == scripts_.end()) {
            std::cerr << "Script not found: " << name << std::endl;
            return false;
        }
        
        bool result = it->second(context);
        commandHistory_.push_back("exec " + name);
        return result;
    }
    
    /**
     * Define variable
     */
    void defineVariable(ScriptingContext& context, const std::string& name, const std::string& value) {
        context.variables[name] = value;
        commandHistory_.push_back("var " + name + " = " + value);
    }
    
    /**
     * Get variable value
     */
    std::string getVariable(const ScriptingContext& context, const std::string& name) {
        auto it = context.variables.find(name);
        if (it != context.variables.end()) {
            return it->second;
        }
        return "";
    }
    
    /**
     * Run batch script
     */
    bool runBatch(const std::vector<std::string>& scriptNames, ScriptingContext& context) {
        for (const auto& name : scriptNames) {
            if (!executeScript(name, context)) {
                return false;
            }
        }
        return true;
    }
    
    /**
     * Get command history
     */
    std::vector<std::string> getCommandHistory() const {
        return commandHistory_;
    }
    
    /**
     * Clear command history
     */
    void clearCommandHistory() {
        commandHistory_.clear();
    }
    
    /**
     * Save command history to file
     */
    bool saveCommandHistory(const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) return false;
        
        for (const auto& command : commandHistory_) {
            file << command << std::endl;
        }
        
        return true;
    }
    
    /**
     * Load command history from file
     */
    bool loadCommandHistory(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        std::string line;
        while (std::getline(file, line)) {
            commandHistory_.push_back(line);
        }
        
        return true;
    }
};

/**
 * NLM automation engine
 * Automated control and execution of NLM demonstrations
 */
class NLMAutomationEngine {
public:
    /**
     * Automation task
     */
    struct AutomationTask {
        std::string name;
        std::function<void()> setup;
        std::function<void()> execute;
        std::function<void()> cleanup;
        bool enabled = true;
        std::map<std::string, std::string> parameters;
    };
    
    /**
     * Automation sequence
     */
    class AutomationSequence {
    public:
        std::string name;
        std::vector<AutomationTask> tasks;
        double interval;  // seconds between tasks
        bool repeat = false;
        
        AutomationSequence(const std::string& n, double interval = 1.0) 
            : name(n), interval(interval) {}
    };
    
private:
    std::vector<AutomationTask> tasks_;
    std::vector<AutomationSequence> sequences_;
    bool running = false;
    
public:
    /**
     * Add task
     */
    void addTask(const AutomationTask& task) {
        tasks_.push_back(task);
    }
    
    /**
     * Add sequence
     */
    void addSequence(const AutomationSequence& sequence) {
        sequences_.push_back(sequence);
    }
    
    /**
     * Run all tasks
     */
    void runAllTasks() {
        for (auto& task : tasks_) {
            if (task.enabled) {
                if (task.setup) task.setup();
                if (task.execute) task.execute();
                if (task.cleanup) task.cleanup();
            }
        }
    }
    
    /**
     * Run automation sequence
     */
    void runSequence(const std::string& sequenceName) {
        for (auto& sequence : sequences_) {
            if (sequence.name == sequenceName) {
                running = true;
                for (auto& task : sequence.tasks) {
                    if (task.setup) task.setup();
                    if (task.execute) task.execute();
                    
                    if (running) {
                        if (sequence.interval > 0.0) {
                            // Simulate interval delay
                            std::this_thread::sleep_for(std::chrono::milliseconds(
                                static_cast<int>(sequence.interval * 1000.0)));
                        }
                    } else {
                        break;
                    }
                }
                running = false;
                break;
            }
        }
    }
    
    /**
     * Stop all automation
     */
    void stop() {
        running = false;
    }
};

/**
 * Enhanced NLM configuration with debugging support
 */
class EnhancedConfig {
public:
    class DebugOptions {
    public:
        bool enableDebugLogging = false;
        bool enableProfiling = false;
        bool enableDetailedStateCapture = false;
        bool enableMemoryTracking = false;
        std::string debugLogFile;
        uint64_t debugCaptureInterval = 1000;  // steps
        bool autoSaveDebug = false;
        
        DebugOptions() {}
    };
    
    DebugOptions debug;
    std::shared_ptr<Config> config;
    
    EnhancedConfig() {
        config = std::make_shared<Config>();
    }
    
    EnhancedConfig(const std::shared_ptr<Config>& cfg) : config(cfg) {}
    
    /**
     * Set debug options
     */
    void setDebugOptions(const DebugOptions& opts) {
        debug = opts;
    }
    
    /**
     * Add debug configuration parameters
     */
    void addDebugParameters() {
        if (debug.enableDebugLogging) {
            config->set("debug.logging_enabled", "true");
        }
        if (debug.enableProfiling) {
            config->set("debug.profiling_enabled", "true");
        }
        if (debug.enableDetailedStateCapture) {
            config->set("debug.state_capture_interval", std::to_string(debug.debugCaptureInterval));
        }
        if (!debug.debugLogFile.empty()) {
            config->set("debug.log_file", debug.debugLogFile);
        }
    }
    
    /**
     * Create enhanced config from debug options
     */
    static EnhancedConfig createEnhanced(const DebugOptions& debugOpts, 
                                        const std::shared_ptr<Config>& baseConfig = nullptr) {
        EnhancedConfig enhanced;
        enhanced.setDebugOptions(debugOpts);
        
        if (baseConfig) {
            enhanced.config = baseConfig;
        }
        
        enhanced.addDebugParameters();
        return enhanced;
    }
};

/**
 * Main advanced tools manager
 * Integrates all advanced debugging, profiling, and scripting capabilities
 */
class NLMAdvancedTools {
private:
    NLMAdvancedDebugger debugger_;
    NLMProfiler profiler_;
    NLMScriptingEngine scriptingEngine_;
    NLMAutomationEngine automationEngine_;
    EnhancedConfig enhancedConfig_;
    
public:
    /**
     * Get debugger
     */
    NLMAdvancedDebugger& getDebugger() {
        return debugger_;
    }
    
    /**
     * Get profiler
     */
    NLMProfiler& getProfiler() {
        return profiler_;
    }
    
    /**
     * Get scripting engine
     */
    NLMScriptingEngine& getScriptingEngine() {
        return scriptingEngine_;
    }
    
    /**
     * Get automation engine
     */
    NLMAutomationEngine& getAutomationEngine() {
        return automationEngine_;
    }
    
    /**
     * Get enhanced config
     */
    EnhancedConfig& getEnhancedConfig() {
        return enhancedConfig_;
    }
    
    /**
     * Create comprehensive demo setup with all advanced tools
     */
    static std::tuple<std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>>
    createAdvancedDemoSetup(const EnhancedConfig& config) {
        auto brain = std::make_shared<Brain>(config.config);
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(config.debug.enableDebugLogging ? 12345 : 42);
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        
        // Configure based on debug options
        if (config.debug.enableDebugLogging) {
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
        }
        
        return std::make_tuple(brain, agent, std::make_shared<SimpleWorld>(world));
    }
    
    /**
     * Run demonstration with advanced tools
     */
    bool runAdvancedDemo(const std::string& demoName, EnhancedConfig config) {
        std::cout << "=== Advanced NLM Demo: " << demoName << " ===" << std::endl;
        
        // Create enhanced setup
        auto [brain, agent, world] = createAdvancedDemoSetup(config);
        
        // Start debugging if enabled
        if (config.debug.enableDebugLogging) {
            debugger_.startSession(demoName + "_debug");
        }
        
        // Start profiling if enabled
        if (config.debug.enableProfiling) {
            profiler_.startSession(demoName + "_profile");
        }
        
        // Run the demo (simplified implementation)
        std::cout << "Running " << demoName << " demonstration..." << std::endl;
        
        // Simulate some demo steps
        for (uint64_t step = 0; step < 100; ++step) {
            // Capture state if debugging
            if (config.debug.enableDebugLogging && step % config.debug.debugCaptureInterval == 0) {
                debugger_.captureState(brain, agent, world, step, static_cast<double>(step) * 0.001, 
                                    "Step " + std::to_string(step));
            }
            
            // Record time series data if profiling
            if (config.debug.enableProfiling) {
                profiler_.recordTimeSeries(demoName + "_profile", "firing_rate", 
                                         brain->getAverageFiringRate(), step);
                profiler_.recordTimeSeries(demoName + "_profile", "reward", 
                                         0.0f, step); // Would track actual reward
            }
            
            // Simple demo loop (would be replaced with actual demo logic)
            const SensoryPercept& percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, static_cast<double>(step) * 0.001);
            
            if (step % 50 == 0 && config.debug.enableDebugLogging) {
                std::cout << "Step " << step << ": " << brain->getAverageFiringRate() << " Hz" << std::endl;
            }
        }
        
        // Stop sessions
        if (config.debug.enableDebugLogging) {
            debugger_.getSession(demoName + "_debug");
        }
        if (config.debug.enableProfiling) {
            profiler_.stopSession();
        }
        
        // Generate reports if enabled
        if (config.debug.enableDebugLogging) {
            debugger_.saveSession(demoName + "_debug", demoName + "_debug.log");
            std::cout << "Debug data saved to " << demoName << "_debug.log" << std::endl;
        }
        
        if (config.debug.enableProfiling) {
            std::cout << profiler_.generateReport(demoName + "_profile") << std::endl;
        }
        
        std::cout << "=== Demo " << demoName << " Completed ===" << std::endl;
        return true;
    }
};

} // namespace nlm

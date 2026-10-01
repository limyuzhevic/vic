// NLM Profiler Tool
// Performance monitoring and profiling tools for the NLM brain

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <map>
#include <vector>

namespace nlm {

class Profiler {
public:
    Profiler(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world);
    ~Profiler();
    
    // Command-line interface
    void run(const std::vector<std::string>& args);
    
    // Performance measurement
    void startMeasurement(const std::string& name);
    void stopMeasurement(const std::string& name);
    void resetMeasurements();
    
    // System resource monitoring
    void monitorCPU() const;
    void monitorMemory() const;
    void monitorNetwork() const;
    
    // Brain performance metrics
    void profileBrain() const;
    void profileMemorySystems() const;
    void profileNeuromodulation() const;
    void profileCognition() const;
    void profileDevelopment() const;
    
    // Agent performance metrics
    void profileAgent() const;
    void profileSensoryProcessing() const;
    void profileMotorDecoding() const;
    
    // World performance metrics
    void profileWorld() const;
    
    // Command processing
    bool processCommand(const std::string& cmd, const std::vector<std::string>& args);
    
    // Help and usage
    void printHelp() const;
    void printProfilingCommands() const;
    void printMonitoringCommands() const;
    void printBenchmarkCommands() const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    std::shared_ptr<SimpleWorld> world_;
    
    // Performance data
    struct Measurement {
        double startTime;
        double endTime;
        double duration;
        std::string description;
        size_t callCount;
    };
    
    std::map<std::string, Measurement> measurements_;
    std::chrono::system_clock::time_point sessionStartTime_;
    
    // Command handlers
    bool handleProfileCommand(const std::vector<std::string>& args);
    bool handleMonitorCommand(const std::vector<std::string>& args);
    bool handleBenchmarkCommand(const std::vector<std::string>& args);
    bool handleMeasureCommand(const std::vector<std::string>& args);
    bool handleReportCommand(const std::vector<std::string>& args);
    bool handleResetCommand(const std::vector<std::string>& args);
    
    // Profiling methods
    void profileBrainStep() const;
    void profileMemoryUsage() const;
    void profileSpikeProcessing() const;
    void profilePlasticityUpdate() const;
    void profileSensoryProcessing() const;
    void profileMotorOutput() const;
    void profileWorldSimulation() const;
    
    // Reporting methods
    void generatePerformanceReport() const;
    void generateBenchmarkReport() const;
    void generateComparisonReport() const;
    
    // Formatting helpers
    void printMeasurement(const std::string& name, const Measurement& meas) const;
    void printSummary() const;
    std::string formatTime(double seconds) const;
    std::string formatBytes(size_t bytes) const;
};

Profiler::Profiler(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world)
    : brain_(brain), agent_(agent), world_(world) {
    sessionStartTime_ = std::chrono::system_clock::now();
    resetMeasurements();
}

Profiler::~Profiler() = default;

void Profiler::run(const std::vector<std::string>& args) {
    if (args.empty()) {
        printHelp();
        return;
    }
    
    std::string command = args[0];
    std::vector<std::string> commandArgs(args.begin() + 1, args.end());
    
    if (!processCommand(command, commandArgs)) {
        std::cerr << "Unknown command: " << command << std::endl;
        std::cerr << "Type 'help' for available commands." << std::endl;
    }
}

bool Profiler::processCommand(const std::string& cmd, const std::vector<std::string>& args) {
    if (cmd == "help" || cmd == "?") {
        handleHelpCommand(args);
        return true;
    } else if (cmd == "profile") {
        return handleProfileCommand(args);
    } else if (cmd == "monitor") {
        return handleMonitorCommand(args);
    } else if (cmd == "benchmark") {
        return handleBenchmarkCommand(args);
    } else if (cmd == "measure") {
        return handleMeasureCommand(args);
    } else if (cmd == "report") {
        return handleReportCommand(args);
    } else if (cmd == "reset") {
        return handleResetCommand(args);
    } else {
        return false;
    }
}

void Profiler::printHelp() const {
    std::cout << "=== NLM Profiler Help ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Profiling Commands:" << std::endl;
    std::cout << "  profile brain <duration>      - Profile brain processing" << std::endl;
    std::cout << "  profile memory <duration>     - Profile memory systems" << std::endl;
    std::cout << "  profile neuromod <duration>    - Profile neuromodulation" << std::endl;
    std::cout << "  profile cognition <duration>  - Profile cognitive systems" << std::endl;
    std::cout << "  profile agent <duration>      - Profile agent behavior" << std::endl;
    std::cout << "  profile world <duration>      - Profile world simulation" << std::endl;
    std::cout << "  profile all <duration>        - Profile all systems" << std::endl;
    std::cout << std::endl;
    
    printMonitoringCommands();
    printBenchmarkCommands();
    
    std::cout << std::endl;
    std::cout << "Measurement Commands:" << std::endl;
    std::cout << "  measure <name>               - Start timing a measurement" << std::endl;
    std::cout << "  report <name>                - Generate report for measurement" << std::endl;
    std::cout << "  reset                        - Reset all measurements" << std::endl;
    std::cout << std::endl;
    std::cout << "Session info:" << std::endl;
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStartTime_).count();
    std::cout << "  Session started: " << duration << " seconds ago" << std::endl;
}

void Profiler::printProfilingCommands() const {
    std::cout << "Profiling Options:" << std::endl;
    std::cout << "  <duration> is in simulation steps" << std::endl;
    std::cout << "  Use 0 for "unlimited" or until interrupted" << std::endl;
    std::cout << "  Add --detailed for verbose output" << std::endl;
    std::cout << "  Add --csv <file> to export to CSV" << std::endl;
    std::cout << std::endl;
}

void Profiler::printMonitoringCommands() const {
    std::cout << "Monitoring Commands:" << std::endl;
    std::cout << "  monitor cpu                   - Monitor CPU usage" << std::endl;
    std::cout << "  monitor memory                - Monitor memory usage" << std::endl;
    std::cout << "  monitor network               - Monitor network activity" << std::endl;
    std::cout << std::endl;
}

void Profiler::printBenchmarkCommands() const {
    std::cout << "Benchmark Commands:" << std::endl;
    std::cout << "  benchmark brain               - Run brain benchmark" << std::endl;
    std::cout << "  benchmark memory              - Run memory benchmark" << std::endl;
    std::cout << "  benchmark all                 - Run all benchmarks" << std::endl;
    std::cout << std::endl;
}

bool Profiler::handleProfileCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Please specify what to profile and duration." << std::endl;
        std::cerr << "Use 'help profile' for options." << std::endl;
        return false;
    }
    
    std::string target = args[0];
    int duration = 100; // Default duration
    
    if (args.size() > 1) {
        try {
            duration = std::stoi(args[1]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid duration: " << args[1] << std::endl;
            return false;
        }
    }
    
    if (target == "brain") {
        profileBrain();
        std::cout << "Profiling brain for " << duration << " steps..." << std::endl;
        // Would actually run simulation here
        profileBrainStep();
    } else if (target == "memory") {
        profileMemorySystems();
        std::cout << "Profiling memory systems for " << duration << " steps..." << std::endl;
    } else if (target == "neuromod") {
        profileNeuromodulation();
        std::cout << "Profiling neuromodulation for " << duration << " steps..." << std::endl;
    } else if (target == "cognition") {
        profileCognition();
        std::cout << "Profiling cognition systems for " << duration << " steps..." << std::endl;
    } else if (target == "agent") {
        profileAgent();
        std::cout << "Profiling agent for " << duration << " steps..." << std::endl;
    } else if (target == "world") {
        profileWorld();
        std::cout << "Profiling world simulation for " << duration << " steps..." << std::endl;
    } else if (target == "all") {
        std::cout << "Profiling all systems for " << duration << " steps..." << std::endl;
        profileBrain();
        profileMemorySystems();
        profileNeuromodulation();
        profileCognition();
        profileAgent();
        profileWorld();
    } else {
        std::cerr << "Unknown profiling target: " << target << std::endl;
        return false;
    }
    
    return true;
}

void Profiler::profileBrain() const {
    std::cout << "=== Brain Performance Profile ===" << std::endl;
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    brain_->logStatus();
}

void Profiler::profileMemorySystems() const {
    std::cout << "=== Memory Systems Profile ===" << std::endl;
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    auto* wm = brain_->getWorkingMemory();
    auto* em = brain_->getEpisodicMemory();
    auto* am = brain_->getAssociativeMemory();
    
    std::cout << "Working Memory:" << std::endl;
    if (wm) {
        std::cout << "  Active traces: " << wm->getActiveTraces() << std::endl;
    } else {
        std::cout << "  Not available" << std::endl;
    }
    
    std::cout << "Episodic Memory:" << std::endl;
    if (em) {
        std::cout << "  Episodes: " << em->getEpisodeCount() << std::endl;
    } else {
        std::cout << "  Not available" << std::endl;
    }
    
    std::cout << "Associative Memory:" << std::endl;
    if (am) {
        std::cout << "  Available" << std::endl;
    } else {
        std::cout << "  Not available" << std::endl;
    }
}

void Profiler::profileNeuromodulation() const {
    std::cout << "=== Neuromodulation Profile ===" << std::endl;
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    auto* dopamine = brain_->getDopamine();
    auto* curiosity = brain_->getCuriosity();
    auto* novelty = brain_->getNovelty();
    
    std::cout << "Dopamine System:" << std::endl;
    if (dopamine) {
        std::cout << "  Available" << std::endl;
    } else {
        std::cout << "  Not available" << std::endl;
    }
    
    std::cout << "Curiosity System:" << std::endl;
    if (curiosity) {
        std::cout << "  Available" << std::endl;
    } else {
        std::cout << "  Not available" << std::endl;
    }
    
    std::cout << "Novelty System:" << std::endl;
    if (novelty) {
        std::cout << "  Available" << std::endl;
    } else {
        std::cout << "  Not available" << std::endl;
    }
}

void Profiler::profileCognition() const {
    std::cout << "=== Cognition System Profile ===" << std::endl;
    if (!brain_) {
        std::cout << "Brain not available" << std::endl;
        return;
    }
    
    auto* planner = brain_->getPlanner();
    auto* conceptFormation = brain_->getConceptFormation();
    auto* attention = brain_->getAttention();
    
    std::cout << "Neural Planner:" << std::endl;
    if (planner) {
        std::cout << "  Available" << std::endl;
    } else {
        std::cout << "  Not available" << std::endl;
    }
    
    std::cout << "Concept Formation:" << std::endl;
    if (conceptFormation) {
        std::cout << "  Available" << std::endl;
    } else {
        std::cout << "  Not available" << std::endl;
    }
    
    std::cout << "Attentional Selection:" << std::endl;
    if (attention) {
        std::cout << "  Available" << std::endl;
    } else {
        std::cout << "  Not available" << std::endl;
    }
}

void Profiler::profileAgent() const {
    std::cout << "=== Agent Profile ===" << std::endl;
    if (!agent_) {
        std::cout << "Agent not available" << std::endl;
        return;
    }
    
    std::cout << "Agent system is available" << std::endl;
    // Would add agent-specific profiling here
}

void Profiler::profileWorld() const {
    std::cout << "=== World Profile ===" << std::endl;
    if (!world_) {
        std::cout << "World not available" << std::endl;
        return;
    }
    
    std::cout << "World simulation is available" << std::endl;
    // Would add world-specific profiling here
}

void Profiler::printSummary() const {
    std::cout << "=== Performance Summary ===" << std::endl;
    std::cout << "Session duration: " << formatTime(
        std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::system_clock::now() - sessionStartTime_
        ).count()) << std::endl;
    
    std::cout << "Measurements completed:" << std::endl;
    for (const auto& pair : measurements_) {
        printMeasurement(pair.first, pair.second);
    }
}

std::string Profiler::formatTime(double seconds) const {
    if (seconds < 60) {
        return std::to_string(seconds) + " seconds";
    } else if (seconds < 3600) {
        return std::to_string(seconds / 60) + " minutes";
    } else {
        return std::to_string(seconds / 3600) + " hours";
    }
}

std::string Profiler::formatBytes(size_t bytes) const {
    if (bytes < 1024) {
        return std::to_string(bytes) + " bytes";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024) + " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    } else {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    }
}

} // namespace nlm
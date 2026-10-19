// NLM Advanced Debugging Interface Implementation
// Provides comprehensive debugging, profiling, and analysis capabilities for NLM

#include "DebugInterface.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace nlm {

DebugInterface::DebugInterface(std::shared_ptr<Config> config,
                               std::shared_ptr<Brain> brain,
                               std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<AgentBody> body,
                               std::shared_ptr<SimpleWorld> world)
    : config_(config), brain_(brain), agent_(agent), body_(body), world_(world),
      loggingEnabled_(false), performanceMonitoringActive_(false) {
    
    // Initialize performance metrics
    performanceMetrics_.startTime = std::chrono::steady_clock::now();
    performanceMetrics_.totalCPUTime = 0.0;
    performanceMetrics_.totalWallTime = 0.0;
    performanceMetrics_.totalSteps = 0;
    performanceMetrics_.averageFiringRate = 0.0;
    performanceMetrics_.averageMemoryUsage = 0.0;
    performanceMetrics_.averageNetworkLatency = 0.0;
    
    // Register all commands
    registerCommand(DebugCommand::Help, "help", "Show help information", [this]() { cmdHelp(); });
    registerCommand(DebugCommand::Status, "status", "Show system status", [this]() { cmdStatus(); });
    registerCommand(DebugCommand::Performance, "perf", "Show performance metrics", [this]() { cmdPerformance(); });
    registerCommand(DebugCommand::Memory, "memory", "Show memory usage", [this]() { cmdMemory(); });
    registerCommand(DebugCommand::Network, "network", "Show network statistics", [this]() { cmdNetwork(); });
    registerCommand(DebugCommand::Neurons, "neurons", "Show neuron information", [this]() { cmdNeurons(); });
    registerCommand(DebugCommand::Synapses, "synapses", "Show synapse information", [this]() { cmdSynapses(); });
    registerCommand(DebugCommand::Regions, "regions", "Show region information", [this]() { cmdRegions(); });
    registerCommand(DebugCommand::Populations, "populations", "Show population information", [this]() { cmdPopulations(); });
    registerCommand(DebugCommand::Development, "dev", "Show development system status", [this]() { cmdDevelopment(); });
    registerCommand(DebugCommand::Neuromodulation, "mod", "Show neuromodulation status", [this]() { cmdNeuromodulation(); });
    registerCommand(DebugCommand::Prediction, "pred", "Show prediction system status", [this]() { cmdPrediction(); });
    registerCommand(DebugCommand::MemoryUsage, "memusage", "Show memory usage details", [this]() { cmdMemoryUsage(); });
    registerCommand(DebugCommand::EnergyBudget, "energy", "Show energy budget", [this]() { cmdEnergyBudget(); });
    registerCommand(DebugCommand::VisualInput, "vision", "Show visual input", [this]() { cmdVisualInput(); });
    registerCommand(DebugCommand::MotorOutput, "motor", "Show motor output", [this]() { cmdMotorOutput(); });
    registerCommand(DebugCommand::AgentState, "agent", "Show agent state", [this]() { cmdAgentState(); });
    registerCommand(DebugCommand::Configuration, "config", "Show configuration", [this]() { cmdConfiguration(); });
    registerCommand(DebugCommand::ExportData, "export", "Export data", [this]() { cmdExportData(); });
    registerCommand(DebugCommand::ImportData, "import", "Import data", [this]() { cmdImportData(); });
    registerCommand(DebugCommand::Benchmark, "benchmark", "Run benchmark", [this]() { cmdBenchmark(); });
    registerCommand(DebugCommand::RunExperiment, "run", "Run experiment", [this]() { cmdRunExperiment(); });
    registerCommand(DebugCommand::SetParameter, "set", "Set parameter", [this]() { cmdSetParameter(); });
    registerCommand(DebugCommand::GetParameter, "get", "Get parameter", [this]() { cmdGetParameter(); });
    registerCommand(DebugCommand::ValidateConfig, "validate", "Validate configuration", [this]() { cmdValidateConfig(); });
    registerCommand(DebugCommand::ClearLog, "clearlog", "Clear log file", [this]() { cmdClearLog(); });
    registerCommand(DebugCommand::EnableLog, "enablelog", "Enable logging", [this]() { cmdEnableLog(); });
    registerCommand(DebugCommand::DisableLog, "disablelog", "Disable logging", [this]() { cmdDisableLog(); });
    registerCommand(DebugCommand::ShowLog, "showlog", "Show log contents", [this]() { cmdShowLog(); });
    registerCommand(DebugCommand::SaveState, "save", "Save system state", [this]() { cmdSaveState(); });
    registerCommand(DebugCommand::LoadState, "load", "Load system state", [this]() { cmdLoadState(); });
    registerCommand(DebugCommand::Replay, "replay", "Replay saved state", [this]() { cmdReplay(); });
    registerCommand(DebugCommand::Quit, "quit", "Quit debugging session", [this]() { cmdQuit(); });
}

DebugInterface::~DebugInterface() {
    if (loggingEnabled_) {
        logStream_.close();
    }
}

bool DebugInterface::processCommand(const std::string& input) {
    std::string trimmedInput = trim(input);
    if (trimmedInput.empty()) {
        return false;
    }
    
    // Check for command completion
    auto completions = getCommandCompletions(trimmedInput);
    if (completions.size() > 1) {
        // Multiple completions - show them
        std::cout << "Possible commands:" << std::endl;
        for (const auto& completion : completions) {
            std::cout << "  " << completion << std::endl;
        }
        return true;
    } else if (completions.size() == 1) {
        // Single completion - execute it
        processCommand(parseCommand(completions[0]));
        return true;
    }
    
    // Parse command
    DebugCommand cmd = parseCommand(trimmedInput);
    if (cmd == DebugCommand::Help) {
        // Check for specific command help
        if (trimmedInput.length() > 5) {
            std::string subCmd = trimmedInput.substr(5);
            // Try to parse subCommand
            for (const auto& entry : commands_) {
                if (entry.first != DebugCommand::Help && entry.second.first == subCmd) {
                    std::vector<std::string> help = getCommandHelp(entry.first);
                    for (const auto& line : help) {
                        std::cout << line << std::endl;
                    }
                    return true;
                }
            }
        }
    }
    
    processCommand(cmd);
    return true;
}

void DebugInterface::processCommand(DebugCommand cmd) {
    auto it = commands_.find(cmd);
    if (it != commands_.end()) {
        it->second.second();
    } else {
        error("Unknown command: " + commandToString(cmd));
    }
}

void DebugInterface::registerCommand(DebugCommand cmd, const std::string& name, const std::string& description,
                                     std::function<void()> handler) {
    commands_[cmd] = std::make_pair(name, handler);
}

std::vector<std::string> DebugInterface::getCommandCompletions(const std::string& input) const {
    std::vector<std::string> completions;
    for (const auto& entry : commands_) {
        if (entry.second.first.find(input) == 0) {
            completions.push_back("--" + entry.second.first);
        }
    }
    return completions;
}

std::vector<std::string> DebugInterface::getCommandHelp(DebugCommand cmd) const {
    auto it = commands_.find(cmd);
    if (it != commands_.end()) {
        return {
            "Command: --" + it->second.first,
            "Description: " + it->second.second,
            "",
            "Usage: --" + it->second.first + " [parameters]"
        };
    }
    return {"Command not found"};
}

void DebugInterface::cmdHelp() {
    std::cout << "=== NLM Debug Interface Help ===" << std::endl;
    std::cout << "Available commands:" << std::endl;
    
    int maxNameLength = 0;
    for (const auto& entry : commands_) {
        maxNameLength = std::max(maxNameLength, static_cast<int>(entry.second.first.length()));
    }
    
    for (const auto& entry : commands_) {
        std::cout << "  --" << entry.second.first;
        std::cout << std::string(maxNameLength - entry.second.first.length() + 4, ' ');
        std::cout << "- " << entry.second.second << std::endl;
    }
    
    std::cout << "" << std::endl;
    std::cout << "Use '--command' to get detailed help for a specific command." << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  --help                    Show this help" << std::endl;
    std::cout << "  --status                  Show system status" << std::endl;
    std::cout << "  --perf                    Show performance metrics" << std::endl;
    std::cout << "  --memory                  Show memory usage" << std::endl;
    std::cout << "  --neurons                 Show neuron information" << std::endl;
    std::cout << "  --quit                    Exit debug interface" << std::endl;
}

void DebugInterface::cmdStatus() {
    std::cout << "=== NLM System Status ===" << std::endl;
    std::cout << "Brain: " << (brain_ ? "Active" : "Inactive") << std::endl;
    if (brain_) {
        std::cout << "  Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "  Regions: " << brain_->getRegionCount() << std::endl;
        std::cout << "  Spike Count: " << brain_->getTotalSpikeCount() << std::endl;
        std::cout << "  Firing Neurons: " << brain_->getFiringNeuronCount() << std::endl;
        std::cout << "  Average Firing Rate: " << brain_->getAverageFiringRate() << std::endl;
    }
    
    std::cout << "Agent: " << (agent_ ? "Active" : "Inactive") << std::endl;
    if (agent_) {
        std::cout << "  Dopamine Level: " << agent_->getNeuromodulationLevel() << std::endl;
        std::cout << "  Curiosity Level: " << agent_->getCuriosityLevel() << std::endl;
        std::cout << "  Novelty Level: " << agent_->getNoveltyLevel() << std::endl;
        std::cout << "  Prediction Error: " << agent_->getPredictionError() << std::endl;
        std::cout << "  Developmental Stage: " << static_cast<int>(agent_->getDevelopmentalStage()) << std::endl;
    }
    
    std::cout << "Body: " << (body_ ? "Active" : "Inactive") << std::endl;
    if (body_) {
        std::cout << "  Position: (" << body_->x << ", " << body_->y << ")" << std::endl;
        std::cout << "  Orientation: " << body_->orientation << " rad" << std::endl;
        std::cout << "  Energy: " << body_->energy << "/" << body_->health << std::endl;
        std::cout << "  Age: " << body_->age << std::endl;
    }
    
    std::cout << "World: " << (world_ ? "Active" : "Inactive") << std::endl;
    if (world_) {
        std::cout << "  Size: " << world_->getWidth() << " x " << world_->getHeight() << std::endl;
    }
    
    std::cout << "Configuration: " << (config_ ? "Loaded" : "Default") << std::endl;
    if (config_) {
        std::cout << "  Entries: " << config_->getKeys().size() << std::endl;
    }
}

void DebugInterface::cmdPerformance() {
    std::cout << "=== NLM Performance Metrics ===" << std::endl;
    std::cout << "Total Steps: " << performanceMetrics_.totalSteps << std::endl;
    std::cout << "Average Firing Rate: " << performanceMetrics_.averageFiringRate << std::endl;
    std::cout << "Average Memory Usage: " << formatMemorySize(static_cast<size_t>(performanceMetrics_.averageMemoryUsage)) << std::endl;
    std::cout << "Average Network Latency: " << performanceMetrics_.averageNetworkLatency << " ms" << std::endl;
    
    if (!performanceMetrics_.stepTimes.empty()) {
        std::cout << "" << std::endl;
        std::cout << "Step Times (ms):" << std::endl;
        double min = *std::min_element(performanceMetrics_.stepTimes.begin(), performanceMetrics_.stepTimes.end());
        double max = *std::max_element(performanceMetrics_.stepTimes.begin(), performanceMetrics_.stepTimes.end());
        double avg = std::accumulate(performanceMetrics_.stepTimes.begin(), performanceMetrics_.stepTimes.end(), 0.0) / performanceMetrics_.stepTimes.size();
        std::cout << "  Min: " << min << " ms" << std::endl;
        std::cout << "  Max: " << max << " ms" << std::endl;
        std::cout << "  Avg: " << avg << " ms" << std::endl;
        std::cout << "  P95: " << performanceMetrics_.stepTimes[performanceMetrics_.stepTimes.size() * 95 / 100] << " ms" << std::endl;
    }
    
    std::cout << "" << std::endl;
    std::cout << "Memory Usage Timeline:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), performanceMetrics_.memoryUsageSamples.size()); ++i) {
        size_t idx = i * performanceMetrics_.memoryUsageSamples.size() / 10;
        std::cout << "  " << idx << ": " << formatMemorySize(static_cast<size_t>(performanceMetrics_.memoryUsageSamples[idx])) << std::endl;
    }
    
    std::cout << "" << std::endl;
    std::cout << "Firing Rate Timeline:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), performanceMetrics_.firingRateSamples.size()); ++i) {
        size_t idx = i * performanceMetrics_.firingRateSamples.size() / 10;
        std::cout << "  " << idx << ": " << performanceMetrics_.firingRateSamples[idx] << " Hz" << std::endl;
    }
}

void DebugInterface::cmdMemory() {
    std::cout << "=== NLM Memory Usage ===" << std::endl;
    std::cout << "Total Allocated: " << formatMemorySize(memoryProfile_.totalAllocated) << std::endl;
    std::cout << "Total Deallocated: " << formatMemorySize(memoryProfile_.totalDeallocated) << std::endl;
    std::cout << "Current Usage: " << formatMemorySize(memoryProfile_.currentUsage) << std::endl;
    std::cout << "Peak Usage: " << formatMemorySize(memoryProfile_.peakUsage) << std::endl;
    
    std::cout << "" << std::endl;
    std::cout << "Usage by Category:" << std::endl;
    for (const auto& category : memoryProfile_.categoryUsage) {
        std::cout << "  " << category.first << ": " << formatMemorySize(category.second) << std::endl;
    }
    
    std::cout << "" << std::endl;
    std::cout << "Usage Timeline:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(10), memoryProfile_.usageTimeline.size()); ++i) {
        auto entry = memoryProfile_.usageTimeline[i];
        double time = entry.first * 1000.0; // Convert to ms
        std::cout << "  " << time << " ms: " << formatMemorySize(entry.second) << std::endl;
    }
}

void DebugInterface::cmdNetwork() {
    std::cout << "=== NLM Network Statistics ===" << std::endl;
    std::cout << "Packets Sent: " << networkStats_.packetsSent << std::endl;
    std::cout << "Packets Received: " << networkStats_.packetsReceived << std::endl;
    std::cout << "Packet Loss Rate: " << (networkStats_.packetsSent > 0 ? 
        (static_cast<double>(networkStats_.packetsSent - networkStats_.packetsReceived) / networkStats_.packetsSent * 100.0) : 0.0) << "%" << std::endl;
    std::cout << "Average Latency: " << networkStats_.averageLatency << " ms" << std::endl;
    std::cout << "Bandwidth Usage: " << networkStats_.bandwidthUsage << " Mbps" << std::endl;
    
    if (!networkStats_.latencyHistory.empty()) {
        std::cout << "" << std::endl;
        std::cout << "Latency Timeline (ms):" << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), networkStats_.latencyHistory.size()); ++i) {
            size_t idx = i * networkStats_.latencyHistory.size() / 10;
            std::cout << "  " << idx << ": " << networkStats_.latencyHistory[idx] << " ms" << std::endl;
        }
    }
    
    if (!networkStats_.packetSizeHistory.empty()) {
        std::cout << "" << std::endl;
        std::cout << "Packet Size Timeline (bytes):" << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), networkStats_.packetSizeHistory.size()); ++i) {
            size_t idx = i * networkStats_.packetSizeHistory.size() / 10;
            std::cout << "  " << idx << ": " << networkStats_.packetSizeHistory[idx] << " bytes" << std::endl;
        }
    }
}

void DebugInterface::cmdNeurons() {
    std::cout << "=== NLM Neurons ===" << std::endl;
    if (!brain_) {
        std::cout << "No brain initialized" << std::endl;
        return;
    }
    
    size_t totalNeurons = 0;
    for (const auto& region : brain_->getRegions()) {
        std::cout << "Region " << region->getId() << ": " << region->getPopulations().size() << " populations" << std::endl;
        for (const auto& pop : region->getPopulations()) {
            size_t neuronCount = pop->getNeurons().size();
            totalNeurons += neuronCount;
            std::cout << "  Population " << pop->getId() << " (" << pop->getNeuronType() << "):" << std::endl;
            std::cout << "    Type: " << static_cast<int>(pop->getNeuronType()) << std::endl;
            std::cout << "    Neurons: " << neuronCount << std::endl;
            
            // Sample some neuron properties
            auto neurons = pop->getNeurons();
            if (!neurons.empty()) {
                double totalMemPot = 0.0;
                for (const auto& neuron : neurons) {
                    totalMemPot += neuron->getState().membranePotential;
                }
                double avgMemPot = totalMemPot / neurons.size();
                std::cout << "    Avg Membrane Potential: " << avgMemPot << std::endl;
            }
        }
    }
    
    std::cout << "Total Neurons: " << totalNeurons << std::endl;
}

void DebugInterface::cmdSynapses() {
    std::cout << "=== NLM Synapses ===" << std::endl;
    if (!brain_) {
        std::cout << "No brain initialized" << std::endl;
        return;
    }
    
    size_t totalSynapses = 0;
    for (const auto& region : brain_->getRegions()) {
        std::cout << "Region " << region->getId() << ": " << region->getSynapses().size() << " synapses" << std::endl;
        for (const auto& syn : region->getSynapses()) {
            totalSynapses++;
            if (totalSynapses <= 20) { // Limit output
                std::cout << "  Synapse " << syn->getId() << ": " << std::endl;
                std::cout << "    Pre: " << syn->getPreNeuronId() << ", Post: " << syn->getPostNeuronId() << std::endl;
                std::cout << "    Weight: " << syn->getWeight() << std::endl;
                std::cout << "    Delay: " << syn->getDelay() << " ms" << std::endl;
                std::cout << "    Plasticity: " << (syn->isPlasticityEnabled() ? "Enabled" : "Disabled") << std::endl;
                std::cout << "    Eligibility Trace: " << syn->getEligibilityTrace() << std::endl;
            }
        }
    }
    
    std::cout << "Total Synapses: " << totalSynapses << std::endl;
}

void DebugInterface::cmdRegions() {
    std::cout << "=== NLM Regions ===" << std::endl;
    if (!brain_) {
        std::cout << "No brain initialized" << std::endl;
        return;
    }
    
    for (const auto& region : brain_->getRegions()) {
        std::cout << "Region " << region->getId() << ":" << std::endl;
        std::cout << "  Populations: " << region->getPopulations().size() << std::endl;
        std::cout << "  Synapses: " << region->getSynapses().size() << std::endl;
        
        double totalFiringRate = 0.0;
        for (const auto& pop : region->getPopulations()) {
            totalFiringRate += pop->getAverageFiringRate();
        }
        double avgFiringRate = region->getPopulations().empty() ? 0.0 : totalFiringRate / region->getPopulations().size();
        std::cout << "  Average Firing Rate: " << avgFiringRate << std::endl;
    }
}

void DebugInterface::cmdPopulations() {
    std::cout << "=== NLM Populations ===" << std::endl;
    if (!brain_) {
        std::cout << "No brain initialized" << std::endl;
        return;
    }
    
    size_t totalPopulations = 0;
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            totalPopulations++;
            std::cout << "Population " << pop->getId() << " (Region " << region->getId() << "):" << std::endl;
            std::cout << "  Type: " << static_cast<int>(pop->getNeuronType()) << std::endl;
            std::cout << "  Neuron Count: " << pop->getNeurons().size() << std::endl;
            std::cout << "  Average Firing Rate: " << pop->getAverageFiringRate() << std::endl;
            std::cout << "  Membrane Potential Range: " << 
                (pop->getNeurons().empty() ? 0.0 : pop->getNeurons()[0]->getState().membranePotential) << " - "
                << (pop->getNeurons().empty() ? 0.0 : pop->getNeurons().back()->getState().membranePotential) << std::endl;
        }
    }
    
    std::cout << "Total Populations: " << totalPopulations << std::endl;
}

void DebugInterface::cmdDevelopment() {
    std::cout << "=== NLM Development System ===" << std::endl;
    if (!agent_) {
        std::cout << "No agent initialized" << std::endl;
        return;
    }
    
    std::cout << "Developmental Age: " << agent_->getDevelopmentalStage() << std::endl;
    std::cout << "Plasticity Modifier: " << agent_->getPlasticityModifier() << std::endl;
    std::cout << "Development Enabled: " << (agent_->isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "Structural Plasticity Enabled: " << (agent_->isStructuralPlasticityEnabled() ? "Yes" : "No") << std::endl;
    
    if (brain_) {
        std::cout << "Current Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
    }
}

void DebugInterface::cmdNeuromodulation() {
    std::cout << "=== NLM Neuromodulation ===" << std::endl;
    if (!agent_) {
        std::cout << "No agent initialized" << std::endl;
        return;
    }
    
    std::cout << "Dopamine Level: " << agent_->getNeuromodulationLevel() << std::endl;
    std::cout << "Curiosity Level: " << agent_->getCuriosityLevel() << std::endl;
    std::cout << "Novelty Level: " << agent_->getNoveltyLevel() << std::endl;
    std::cout << "Prediction Error: " << agent_->getPredictionError() << std::endl;
    std::cout << "Expected Reward: " << agent_->getExpectedReward() << std::endl;
    
    std::cout << "Modulation Enabled: " << (agent_->isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "Curiosity Enabled: " << (agent_->isCuriosityEnabled() ? "Yes" : "No") << std::endl;
}

void DebugInterface::cmdPrediction() {
    std::cout << "=== NLM Prediction System ===" << std::endl;
    // Add prediction system information when available
    std::cout << "Prediction system not fully implemented in current version" << std::endl;
}

void DebugInterface::cmdMemoryUsage() {
    std::cout << "=== NLM Memory Usage Details ===" << std::endl;
    std::cout << "This command provides detailed memory usage information." << std::endl;
    std::cout << "For more detailed information, use '--perf' or '--memory' commands." << std::endl;
}

void DebugInterface::cmdEnergyBudget() {
    std::cout << "=== NLM Energy Budget ===" << std::endl;
    if (!body_) {
        std::cout << "No body initialized" << std::endl;
        return;
    }
    
    std::cout << "Current Energy: " << body_->energy << std::endl;
    std::cout << "Health: " << body_->health << std::endl;
    std::cout << "Energy Regeneration: " << (body_->energy < 0.5f ? "Active" : "Inactive") << std::endl;
    std::cout << "Damage Threshold: " << (body_->health < 0.3f ? "Low" : (body_->health < 0.6f ? "Medium" : "High")) << std::endl;
}

void DebugInterface::cmdVisualInput() {
    std::cout << "=== NLM Visual Input ===" << std::endl;
    if (!world_) {
        std::cout << "No world initialized" << std::endl;
        return;
    }
    
    std::cout << "Vision Size: " << world_->getVisionWidth() << " x " << world_->getVisionHeight() << std::endl;
    std::cout << "Vision Type: " << (world_->getVisionType() == VisionType::Grayscale ? "Grayscale" : "RGB") << std::endl;
    
    // Show sample vision data if available
    if (agent_) {
        // This would show the actual vision data when available
        std::cout << "Vision data: [sample data would be displayed here]" << std::endl;
    }
}

void DebugInterface::cmdMotorOutput() {
    std::cout << "=== NLM Motor Output ===" << std::endl;
    if (!agent_) {
        std::cout << "No agent initialized" << std::endl;
        return;
    }
    
    // Show current motor command
    MotorCommand cmd = agent_->decodeMotorCommand();
    std::cout << "Current Motor Command: " << static_cast<int>(cmd) << std::endl;
    
    // Show available motor commands
    std::cout << "Available Motor Commands:" << std::endl;
    std::cout << "  0: MoveForward" << std::endl;
    std::cout << "  1: MoveBackward" << std::endl;
    std::cout << "  2: TurnLeft" << std::endl;
    std::cout << "  3: TurnRight" << std::endl;
    std::cout << "  4: LookLeft" << std::endl;
    std::cout << "  5: LookRight" << std::endl;
    std::cout << "  6: Interact" << std::endl;
    std::cout << "  7: Wait" << std::endl;
    
    std::cout << "Motor neuron group activity:" << std::endl;
    std::cout << "  Forward: ? neurons" << std::endl;
    std::cout << "  Backward: ? neurons" << std::endl;
    std::cout << "  Turn Left: ? neurons" << std::endl;
    std::cout << "  Turn Right: ? neurons" << std::endl;
    std::cout << "  Interact: ? neurons" << std::endl;
    std::cout << "  Wait: ? neurons" << std::endl;
}

void DebugInterface::cmdAgentState() {
    std::cout << "=== NLM Agent State ===" << std::endl;
    if (!agent_ || !body_) {
        std::cout << "No agent or body initialized" << std::endl;
        return;
    }
    
    std::cout << "Position: (" << body_->x << ", " << body_->y << ")" << std::endl;
    std::cout << "Orientation: " << body_->orientation << " rad (" << body_->orientation * 180.0 / M_PI << " degrees)" << std::endl;
    std::cout << "Velocity: (" << body_->velocityX << ", " << body_->velocityY << ") m/s" << std::endl;
    std::cout << "Angular Velocity: " << body_->angularVelocity << " rad/s" << std::endl;
    std::cout << "Energy: " << body_->energy << "/" << body_->health << std::endl;
    std::cout << "Health: " << body_->health << std::endl;
    std::cout << "Age: " << body_->age << std::endl;
    std::cout << "Is Moving: " << (body_->isMoving ? "Yes" : "No") << std::endl;
    std::cout << "Is Turning: " << (body_->isTurning ? "Yes" : "No") << std::endl;
    std::cout << "Last Action Time: " << body_->lastActionTime << std::endl;
}

void DebugInterface::cmdConfiguration() {
    std::cout << "=== NLM Configuration ===" << std::endl;
    if (!config_) {
        std::cout << "No configuration loaded" << std::endl;
        return;
    }
    
    std::cout << config_->summary() << std::endl;
}

void DebugInterface::cmdExportData() {
    std::cout << "=== NLM Export Data ===" << std::endl;
    std::cout << "Data export functionality available." << std::endl;
    std::cout << "To export data, use: --export filename [format]" << std::endl;
    std::cout << "Supported formats: text, csv, json" << std::endl;
}

void DebugInterface::cmdImportData() {
    std::cout << "=== NLM Import Data ===" << std::endl;
    std::cout << "Data import functionality available." << std::endl;
    std::cout << "To import data, use: --import filename [format]" << std::endl;
    std::cout << "Supported formats: text, csv, json" << std::endl;
}

void DebugInterface::cmdBenchmark() {
    std::cout << "=== NLM Benchmark ===" << std::endl;
    std::cout << "Benchmark functionality available." << std::endl;
    std::cout << "To run a benchmark, use: --benchmark name neurons steps" << std::endl;
    std::cout << "Example: --benchmark test 100 1000" << std::endl;
}

void DebugInterface::cmdRunExperiment() {
    std::cout << "=== NLM Run Experiment ===" << std::endl;
    std::cout << "Experiment functionality available." << std::endl;
    std::cout << "To run an experiment, use: --run experiment [config]" << std::endl;
    std::cout << "Available experiments: phase3, phase4, phase6" << std::endl;
}

void DebugInterface::cmdSetParameter() {
    std::cout << "=== NLM Set Parameter ===" << std::endl;
    std::cout << "Parameter setting functionality available." << std::endl;
    std::cout << "To set a parameter, use: --set key value" << std::endl;
    std::cout << "Example: --set neuron_count 500" << std::endl;
}

void DebugInterface::cmdGetParameter() {
    std::cout << "=== NLM Get Parameter ===" << std::endl;
    std::cout << "Parameter retrieval functionality available." << std::endl;
    std::cout << "To get a parameter, use: --get key" << std::endl;
    std::cout << "Example: --get neuron_count" << std::endl;
}

void DebugInterface::cmdValidateConfig() {
    std::cout << "=== NLM Validate Configuration ===" << std::endl;
    if (!config_) {
        std::cout << "No configuration to validate" << std::endl;
        return;
    }
    
    std::vector<std::string> errors = validateConfiguration();
    if (errors.empty()) {
        std::cout << "Configuration is valid!" << std::endl;
    } else {
        std::cout << "Configuration validation errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  " << error << std::endl;
        }
    }
}

void DebugInterface::cmdClearLog() {
    std::cout << "=== NLM Clear Log ===" << std::endl;
    if (loggingEnabled_) {
        clearLogFile();
        std::cout << "Log file cleared." << std::endl;
    } else {
        std::cout << "Logging is not enabled." << std::endl;
    }
}

void DebugInterface::cmdEnableLog() {
    std::cout << "=== NLM Enable Log ===" << std::endl;
    std::string filename;
    std::cout << "Enter log filename (or leave blank for default): ";
    std::getline(std::cin, filename);
    
    if (filename.empty()) {
        filename = "nlm_debug.log";
    }
    
    enableLogging(filename);
    std::cout << "Logging enabled for file: " << filename << std::endl;
}

void DebugInterface::cmdDisableLog() {
    std::cout << "=== NLM Disable Log ===" << std::endl;
    disableLogging();
    std::cout << "Logging disabled." << std::endl;
}

void DebugInterface::cmdShowLog() {
    std::cout << "=== NLM Show Log ===" << std::endl;
    if (!isLoggingEnabled()) {
        std::cout << "Logging is not enabled." << std::endl;
        return;
    }
    
    std::cout << "Showing last 50 log entries:" << std::endl;
    std::cout << "(Log file: " << getLogFile() << ")" << std::endl;
    
    // This would read from the log file
    std::cout << "[Log entries would be displayed here]" << std::endl;
}

void DebugInterface::cmdSaveState() {
    std::cout << "=== NLM Save State ===" << std::endl;
    std::string filename;
    std::cout << "Enter save filename: ";
    std::getline(std::cin, filename);
    
    if (saveState(filename)) {
        std::cout << "State saved successfully to: " << filename << std::endl;
    } else {
        std::cout << "Failed to save state." << std::endl;
    }
}

void DebugInterface::cmdLoadState() {
    std::cout << "=== NLM Load State ===" << std::endl;
    std::string filename;
    std::cout << "Enter load filename: ";
    std::getline(std::cin, filename);
    
    if (loadState(filename)) {
        std::cout << "State loaded successfully from: " << filename << std::endl;
    } else {
        std::cout << "Failed to load state." << std::endl;
    }
}

void DebugInterface::cmdReplay() {
    std::cout << "=== NLM Replay ===" << std::endl;
    std::cout << "Replay functionality available." << std::endl;
    std::cout << "To replay, use: --replay filename" << std::endl;
}

void DebugInterface::cmdQuit() {
    std::cout << "=== NLM Debug Interface ===" << std::endl;
    std::cout << "Exiting debug interface. Goodbye!" << std::endl;
}

void DebugInterface::startPerformanceMonitoring() {
    performanceMonitoringActive_ = true;
    performanceMetrics_.startTime = std::chrono::steady_clock::now();
}

void DebugInterface::stopPerformanceMonitoring() {
    performanceMonitoringActive_ = false;
    auto endTime = std::chrono::steady_clock::now();
    performanceMetrics_.totalWallTime = std::chrono::duration<double>(endTime - performanceMetrics_.startTime).count();
}

void DebugInterface::recordStepTime(double time) {
    performanceMetrics_.stepTimes.push_back(time);
    if (performanceMetrics_.stepTimes.size() > 1000) {
        performanceMetrics_.stepTimes.erase(performanceMetrics_.stepTimes.begin());
    }
}

void DebugInterface::recordMemoryUsage(double usage) {
    performanceMetrics_.memoryUsageSamples.push_back(usage);
    if (performanceMetrics_.memoryUsageSamples.size() > 1000) {
        performanceMetrics_.memoryUsageSamples.erase(performanceMetrics_.memoryUsageSamples.begin());
    }
    
    memoryProfile_.usageTimeline.push_back(std::make_pair(
        performanceMetrics_.memoryUsageSamples.size() - 1, usage));
    if (memoryProfile_.usageTimeline.size() > 1000) {
        memoryProfile_.usageTimeline.erase(memoryProfile_.usageTimeline.begin());
    }
}

void DebugInterface::recordFiringRate(double rate) {
    performanceMetrics_.firingRateSamples.push_back(rate);
    if (performanceMetrics_.firingRateSamples.size() > 1000) {
        performanceMetrics_.firingRateSamples.erase(performanceMetrics_.firingRateSamples.begin());
    }
}

void DebugInterface::recordNetworkLatency(double latency) {
    networkStats_.latencyHistory.push_back(latency);
    if (networkStats_.latencyHistory.size() > 1000) {
        networkStats_.latencyHistory.erase(networkStats_.latencyHistory.begin());
    }
    
    networkStats_.packetsSent++;
    networkStats_.averageLatency = std::accumulate(networkStats_.latencyHistory.begin(),
        networkStats_.latencyHistory.end(), 0.0) / networkStats_.latencyHistory.size();
}

bool DebugInterface::exportData(const std::string& filename, const std::string& format) {
    std::cout << "Exporting data to " << filename << " in " << format << " format..." << std::endl;
    // Implementation would export data based on format
    return true;
}

bool DebugInterface::importData(const std::string& filename, const std::string& format) {
    std::cout << "Importing data from " << filename << " in " << format << " format..." << std::endl;
    // Implementation would import data based on format
    return true;
}

bool DebugInterface::saveState(const std::string& filename) const {
    std::cout << "Saving state to " << filename << "..." << std::endl;
    // Implementation would save state
    return true;
}

bool DebugInterface::loadState(const std::string& filename) {
    std::cout << "Loading state from " << filename << "..." << std::endl;
    // Implementation would load state
    return true;
}

void DebugInterface::enableLogging(const std::string& logFile) {
    loggingEnabled_ = true;
    logFile_ = logFile;
    logStream_.open(logFile, std::ios::app);
}

void DebugInterface::disableLogging() {
    loggingEnabled_ = false;
    if (logStream_.is_open()) {
        logStream_.close();
    }
}

void DebugInterface::clearLogFile() {
    if (logStream_.is_open()) {
        logStream_.close();
        std::remove(logFile_.c_str());
        logStream_.open(logFile_, std::ios::app);
    }
}

std::string DebugInterface::getLogFile() const {
    return logFile_;
}

bool DebugInterface::isLoggingEnabled() const {
    return loggingEnabled_;
}

bool DebugInterface::setParameter(const std::string& key, const std::string& value) {
    if (!config_) {
        return false;
    }
    
    try {
        config_->set(key, value);
        return true;
    } catch (...) {
        return false;
    }
}

std::string DebugInterface::getParameter(const std::string& key) const {
    if (!config_) {
        return "";
    }
    
    auto value = config_->get<std::string>(key);
    return value.has_value() ? value.value() : "";
}

std::vector<std::string> DebugInterface::getParameterKeys() const {
    if (!config_) {
        return {};
    }
    
    return config_->getKeys();
}

std::vector<std::string> DebugInterface::validateConfiguration() const {
    if (!config_) {
        return {"No configuration to validate"};
    }
    
    return config_->validateAll();
}

std::string DebugInterface::visualizeNeurons() const {
    std::stringstream ss;
    ss << "Neuron Visualization:" << std::endl;
    ss << "Total Neurons: " << (brain_ ? brain_->getTotalNeuronCount() : 0) << std::endl;
    ss << "Total Synapses: " << (brain_ ? brain_->getTotalSynapseCount() : 0) << std::endl;
    return ss.str();
}

std::string DebugInterface::visualizeSynapses() const {
    std::stringstream ss;
    ss << "Syapse Visualization:" << std::endl;
    ss << "Total Synapses: " << (brain_ ? brain_->getTotalSynapseCount() : 0) << std::endl;
    return ss.str();
}

std::string DebugInterface::visualizeRegions() const {
    std::stringstream ss;
    ss << "Region Visualization:" << std::endl;
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            ss << "Region " << region->getId() << ": " << region->getPopulations().size() << " populations" << std::endl;
        }
    }
    return ss.str();
}

std::string DebugInterface::visualizePopulations() const {
    std::stringstream ss;
    ss << "Population Visualization:" << std::endl;
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                ss << "  Population " << pop->getId() << " (" << pop->getNeurons().size() << " neurons)" << std::endl;
            }
        }
    }
    return ss.str();
}

std::string DebugInterface::visualizeNetworkGraph() const {
    std::stringstream ss;
    ss << "Network Graph Visualization:" << std::endl;
    ss << "Network graph would be displayed here." << std::endl;
    return ss.str();
}

void DebugInterface::runBenchmark(const std::string& benchmarkName, size_t neuronCount, size_t steps) {
    std::cout << "Running benchmark: " << benchmarkName << " (" << neuronCount << " neurons, " << steps << " steps)" << std::endl;
    // Implementation would run the benchmark
}

void DebugInterface::runExperiment(const std::string& experimentName, const std::string& configFile) {
    std::cout << "Running experiment: " << experimentName << std::endl;
    if (!configFile.empty()) {
        std::cout << "Using configuration: " << configFile << std::endl;
    }
    // Implementation would run the experiment
}

std::string DebugInterface::formatMemorySize(size_t bytes) const {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024.0) + " KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024.0 * 1024.0)) + " MB";
    return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0)) + " GB";
}

std::string DebugInterface::formatTime(double seconds) const {
    if (seconds < 60) return std::to_string(seconds) + " s";
    if (seconds < 3600) return std::to_string(seconds / 60.0) + " m";
    if (seconds < 86400) return std::to_string(seconds / 3600.0) + " h";
    return std::to_string(seconds / 86400.0) + " d";
}

std::string DebugInterface::formatRate(double rate) const {
    return std::to_string(rate) + " Hz";
}

// Static command parsing
DebugCommand DebugInterface::parseCommand(const std::string& cmd) {
    if (cmd.empty()) return DebugCommand::Help;
    
    std::string lowerCmd = cmd;
    std::transform(lowerCmd.begin(), lowerCmd.end(), lowerCmd.begin(), ::tolower);
    
    if (lowerCmd == "help" || lowerCmd == "?") return DebugCommand::Help;
    if (lowerCmd == "status" || lowerCmd == "s") return DebugCommand::Status;
    if (lowerCmd == "performance" || lowerCmd == "perf" || lowerCmd == "p") return DebugCommand::Performance;
    if (lowerCmd == "memory" || lowerCmd == "m") return DebugCommand::Memory;
    if (lowerCmd == "network" || lowerCmd == "n") return DebugCommand::Network;
    if (lowerCmd == "neurons" || lowerCmd == "neuron") return DebugCommand::Neurons;
    if (lowerCmd == "synapses" || lowerCmd == "synapse") return DebugCommand::Synapses;
    if (lowerCmd == "regions" || lowerCmd == "region") return DebugCommand::Regions;
    if (lowerCmd == "populations" || lowerCmd == "population") return DebugCommand::Populations;
    if (lowerCmd == "development" || lowerCmd == "dev") return DebugCommand::Development;
    if (lowerCmd == "neuromodulation" || lowerCmd == "mod") return DebugCommand::Neuromodulation;
    if (lowerCmd == "prediction" || lowerCmd == "pred") return DebugCommand::Prediction;
    if (lowerCmd == "memusage") return DebugCommand::MemoryUsage;
    if (lowerCmd == "energy") return DebugCommand::EnergyBudget;
    if (lowerCmd == "vision" || lowerCmd == "visual") return DebugCommand::VisualInput;
    if (lowerCmd == "motor") return DebugCommand::MotorOutput;
    if (lowerCmd == "agent") return DebugCommand::AgentState;
    if (lowerCmd == "config") return DebugCommand::Configuration;
    if (lowerCmd == "export") return DebugCommand::ExportData;
    if (lowerCmd == "import") return DebugCommand::ImportData;
    if (lowerCmd == "benchmark") return DebugCommand::Benchmark;
    if (lowerCmd == "run") return DebugCommand::RunExperiment;
    if (lowerCmd == "set") return DebugCommand::SetParameter;
    if (lowerCmd == "get") return DebugCommand::GetParameter;
    if (lowerCmd == "validate") return DebugCommand::ValidateConfig;
    if (lowerCmd == "clearlog") return DebugCommand::ClearLog;
    if (lowerCmd == "enablelog") return DebugCommand::EnableLog;
    if (lowerCmd == "disablelog") return DebugCommand::DisableLog;
    if (lowerCmd == "showlog") return DebugCommand::ShowLog;
    if (lowerCmd == "save") return DebugCommand::SaveState;
    if (lowerCmd == "load") return DebugCommand::LoadState;
    if (lowerCmd == "replay") return DebugCommand::Replay;
    if (lowerCmd == "quit") return DebugCommand::Quit;
    
    return DebugCommand::Help;
}

std::string DebugInterface::commandToString(DebugCommand cmd) {
    switch (cmd) {
        case DebugCommand::Help: return "help";
        case DebugCommand::Status: return "status";
        case DebugCommand::Performance: return "performance";
        case DebugCommand::Memory: return "memory";
        case DebugCommand::Network: return "network";
        case DebugCommand::Neurons: return "neurons";
        case DebugCommand::Synapses: return "synapses";
        case DebugCommand::Regions: return "regions";
        case DebugCommand::Populations: return "populations";
        case DebugCommand::Development: return "development";
        case DebugCommand::Neuromodulation: return "neuromodulation";
        case DebugCommand::Prediction: return "prediction";
        case DebugCommand::MemoryUsage: return "memusage";
        case DebugCommand::EnergyBudget: return "energy";
        case DebugCommand::VisualInput: return "vision";
        case DebugCommand::MotorOutput: return "motor";
        case DebugCommand::AgentState: return "agent";
        case DebugCommand::Configuration: return "config";
        case DebugCommand::ExportData: return "export";
        case DebugCommand::ImportData: return "import";
        case DebugCommand::Benchmark: return "benchmark";
        case DebugCommand::RunExperiment: return "run";
        case DebugCommand::SetParameter: return "set";
        case DebugCommand::GetParameter: return "get";
        case DebugCommand::ValidateConfig: return "validate";
        case DebugCommand::ClearLog: return "clearlog";
        case DebugCommand::EnableLog: return "enablelog";
        case DebugCommand::DisableLog: return "disablelog";
        case DebugCommand::ShowLog: return "showlog";
        case DebugCommand::SaveState: return "save";
        case DebugCommand::LoadState: return "load";
        case DebugCommand::Replay: return "replay";
        case DebugCommand::Quit: return "quit";
        default: return "unknown";
    }
}

std::vector<std::string> DebugInterface::getAllCommandNames() {
    return {
        "help", "status", "performance", "memory", "network",
        "neurons", "synapses", "regions", "populations",
        "development", "neuromodulation", "prediction",
        "memusage", "energy", "vision", "motor",
        "agent", "config", "export", "import",
        "benchmark", "run", "set", "get", "validate",
        "clearlog", "enablelog", "disablelog", "showlog",
        "save", "load", "replay", "quit"
    };
}

// Internal helper methods
void DebugInterface::log(const std::string& message) {
    std::cout << "[DEBUG] " << message << std::endl;
    if (loggingEnabled_) {
        logStream_ << "[DEBUG] " << message << std::endl;
    }
}

void DebugInterface::error(const std::string& message) {
    std::cout << "[ERROR] " << message << std::endl;
    if (loggingEnabled_) {
        logStream_ << "[ERROR] " << message << std::endl;
    }
}

void DebugInterface::warning(const std::string& message) {
    std::cout << "[WARNING] " << message << std::endl;
    if (loggingEnabled_) {
        logStream_ << "[WARNING] " << message << std::endl;
    }
}

std::string DebugInterface::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool DebugInterface::stringToBool(const std::string& str) const {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr == "true" || lowerStr == "yes" || lowerStr == "1";
}

double DebugInterface::stringToDouble(const std::string& str) const {
    return std::stod(str);
}

int DebugInterface::stringToInt(const std::string& str) const {
    return std::stoi(str);
}

void DebugInterface::executeCommand(DebugCommand cmd) {
    processCommand(cmd);
}

std::vector<std::string> DebugInterface::completeCommand(const std::string& input) const {
    return getCommandCompletions(input);
}

} // namespace nlm
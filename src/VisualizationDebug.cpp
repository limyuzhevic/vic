#include "VisualizationDebug.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <random>
#include <map>

namespace nlm {

BrainVisualizer::BrainVisualizer(std::shared_ptr<Brain> brain) : brain_(brain) {}

BrainVisualizer::~BrainVisualizer() {}

std::string BrainVisualizer::visualize(const VisualizationOptions& options) {
    switch (options.format) {
        case OutputFormat::ASCII:
            return generateASCIIArt(options);
        case OutputFormat::JSON:
            return generateJSON(options);
        case OutputFormat::CSV:
            return generateCSV(options);
        case OutputFormat::TEXT:
            return generateText(options);
        case OutputFormat::HTML:
            return generateHTML(options);
        case OutputFormat::SVG:
            return "SVG not implemented yet";
        case OutputFormat::PNG:
            return "PNG not implemented yet";
        case OutputFormat::PDF:
            return "PDF not implemented yet";
        default:
            return generateASCIIArt(options);
    }
}

std::string BrainVisualizer::visualizeNetwork(const VisualizationOptions& options) {
    std::stringstream ss;
    ss << "=== NLM Brain Network Visualization ===" << std::endl << std::endl;
    
    if (!brain_) {
        ss << "Brain not initialized" << std::endl;
        return ss.str();
    }
    
    ss << "Brain Statistics:" << std::endl;
    ss << "  - Regions: " << brain_->getRegionCount() << std::endl;
    ss << "  - Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
    ss << "  - Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
    ss << "  - Active Neurons: " << brain_->getFiringNeuronCount() << std::endl;
    ss << "  - Average Firing Rate: " << std::fixed << std::setprecision(2) << brain_->getAverageFiringRate() << std::endl;
    ss << "  - Total Spikes: " << brain_->getTotalSpikeCount() << std::endl;
    ss << std::endl;
    
    if (options.showPopulations) {
        ss << "Neural Populations:" << std::endl;
        for (const auto& regionId : brain_->getRegionIds()) {
            auto* region = brain_->getRegion(regionId);
            if (region) {
                ss << "  Region " << regionId << ": " << region->getPopulationCount() << " populations" << std::endl;
                for (const auto& popId : region->getPopulationIds()) {
                    auto* pop = region->getPopulation(popId);
                    if (pop) {
                        ss << "    Population " << popId << ": " << pop->getNeuronCount() << " neurons" << std::endl;
                        ss << "      Type: " << static_cast<int>(pop->getNeuronType()) << std::endl;
                    }
                }
            }
        }
        ss << std::endl;
    }
    
    if (options.showConnections) {
        ss << "Synaptic Connections:" << std::endl;
        uint64_t totalConnections = 0;
        double totalWeight = 0.0;
        
        for (const auto& regionId : brain_->getRegionIds()) {
            auto* region = brain_->getRegion(regionId);
            if (region) {
                for (auto* syn : region->getSynapses()) {
                    totalConnections++;
                    totalWeight += syn->getWeight();
                }
            }
        }
        
        ss << "  - Total connections: " << totalConnections << std::endl;
        ss << "  - Average connection weight: " << (totalConnections > 0 ? totalWeight / totalConnections : 0.0) << std::endl;
        ss << "  - Connection density: " << (brain_->getTotalNeuronCount() > 0 ? 
                                        static_cast<double>(brain_->getTotalSynapseCount()) / brain_->getTotalNeuronCount() : 0.0) << " synapses/neuron" << std::endl;
    }
    
    return ss.str();
}

std::string BrainVisualizer::visualizeConnections(const VisualizationOptions& options) {
    std::stringstream ss;
    ss << "=== Synaptic Connections Visualization ===" << std::endl << std::endl;
    
    if (!brain_) {
        ss << "Brain not initialized" << std::endl;
        return ss.str();
    }
    
    ss << "Connection Summary:" << std::endl;
    ss << "  - Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
    ss << "  - Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
    ss << std::endl;
    
    if (options.showConnections) {
        ss << "Top 10 strongest connections:" << std::endl;
        std::vector<std::tuple<float, uint64_t, uint64_t>> connections; // weight, source, target
        
        for (const auto& regionId : brain_->getRegionIds()) {
            auto* region = brain_->getRegion(regionId);
            if (region) {
                for (auto* syn : region->getSynapses()) {
                    connections.emplace_back(syn->getWeight(), 
                                           syn->getSourceNeuron().index(), 
                                           syn->getDestinationNeuron().index());
                }
            }
        }
        
        // Sort by weight (descending)
        std::sort(connections.begin(), connections.end(), 
                  [](const auto& a, const auto& b) { return std::get<0>(a) > std::get<0>(b); });
        
        size_t count = std::min(connections.size(), size_t(10));
        for (size_t i = 0; i < count; ++i) {
            auto [weight, source, target] = connections[i];
            ss << "  Connection " << i + 1 << ": " << source << " -> " << target 
               << " (weight: " << weight << ")" << std::endl;
        }
    }
    
    return ss.str();
}

std::string BrainVisualizer::visualizeActivity(const VisualizationOptions& options) {
    std::stringstream ss;
    ss << "=== Activity Visualization ===" << std::endl << std::endl;
    
    if (!brain_) {
        ss << "Brain not initialized" << std::endl;
        return ss.str();
    }
    
    ss << "Activity Statistics:" << std::endl;
    ss << "  - Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
    ss << "  - Active neurons: " << brain_->getFiringNeuronCount() << std::endl;
    ss << "  - Average firing rate: " << std::fixed << std::setprecision(3) << brain_->getAverageFiringRate() << std::endl;
    ss << "  - Activity level: " << (brain_->getFiringNeuronCount() > 0 ? "HIGH" : "LOW") << std::endl;
    ss << std::endl;
    
    if (options.showActivity) {
        ss << "Activity heat map:" << std::endl;
        // Create a simple representation of activity
        for (const auto& regionId : brain_->getRegionIds()) {
            auto* region = brain_->getRegion(regionId);
            if (region) {
                ss << "  Region " << regionId << ":" << std::endl;
                for (auto* pop : region->getPopulations()) {
                    uint64_t activeInPop = 0;
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron && neuron->getFiringState() == FiringState::Firing) {
                            activeInPop++;
                        }
                    }
                    ss << "    Population " << pop->getId() << ": " 
                       << activeInPop << "/" << pop->getNeuronCount() 
                       << " active neurons" << std::endl;
                }
            }
        }
    }
    
    return ss.str();
}

std::string BrainVisualizer::visualizePopulations(const VisualizationOptions& options) {
    std::stringstream ss;
    ss << "=== Neural Populations Visualization ===" << std::endl << std::endl;
    
    if (!brain_) {
        ss << "Brain not initialized" << std::endl;
        return ss.str();
    }
    
    ss << "Population Distribution:" << std::endl;
    
    // Count neurons by type
    std::map<NeuronType, uint64_t> typeCounts;
    for (const auto& regionId : brain_->getRegionIds()) {
        auto* region = brain_->getRegion(regionId);
        if (region) {
            for (auto* pop : region->getPopulations()) {
                typeCounts[pop->getNeuronType()] += pop->getNeuronCount();
            }
        }
    }
    
    for (const auto& pair : typeCounts) {
        ss << "  " << static_cast<int>(pair.first) << ": " << pair.second << " neurons" << std::endl;
    }
    
    ss << std::endl;
    
    if (options.showPopulations) {
        ss << "Detailed populations:" << std::endl;
        for (const auto& regionId : brain_->getRegionIds()) {
            auto* region = brain_->getRegion(regionId);
            if (region) {
                ss << "  Region " << regionId << ":" << std::endl;
                for (auto* pop : region->getPopulations()) {
                    ss << "    Population " << pop->getId() << 
                       " (type: " << static_cast<int>(pop->getNeuronType()) << ", "
                       << pop->getNeuronCount() << " neurons)" << std::endl;
                }
            }
        }
    }
    
    return ss.str();
}

std::string BrainVisualizer::visualizeRegions(const VisualizationOptions& options) {
    std::stringstream ss;
    ss << "=== Brain Regions Visualization ===" << std::endl << std::endl;
    
    if (!brain_) {
        ss << "Brain not initialized" << std::endl;
        return ss.str();
    }
    
    ss << "Region Information:" << std::endl;
    
    for (const auto& regionId : brain_->getRegionIds()) {
        auto* region = brain_->getRegion(regionId);
        if (region) {
            ss << "  Region " << regionId << ":" << std::endl;
            ss << "    Neurons: " << region->getNeuronCount() << std::endl;
            ss << "    Populations: " << region->getPopulationCount() << std::endl;
            ss << "    Connections: " << region->getConnectionCount() << std::endl;
            ss << "    Volume: " << region->getVolume() << std::endl;
        }
    }
    
    return ss.str();
}

bool BrainVisualizer::exportVisualization(const std::string& filename, const VisualizationOptions& options) {
    try {
        std::string content = visualize(options);
        
        std::ofstream file(filename, std::ios::out);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string BrainVisualizer::generateASCIIArt(const VisualizationOptions& options) const {
    std::stringstream ss;
    ss << "=== NLM Brain ASCII Visualization ===" << std::endl << std::endl;
    
    if (!brain_) {
        ss << "Brain not initialized" << std::endl;
        return ss.str();
    }
    
    // Create a simple ASCII representation
    ss << "Brain Structure:" << std::endl;
    ss << "  +-----------+" << std::endl;
    ss << "  |   Brain   |" << std::endl;
    ss << "  +-----------+" << std::endl;
    ss << "     |" << std::endl;
    
    // Add regions
    for (const auto& regionId : brain_->getRegionIds()) {
        ss << "  +-----------+" << std::endl;
        ss << "  | Region " << regionId << " |" << std::endl;
        ss << "  +-----------+" << std::endl;
        ss << "     |" << std::endl;
    }
    
    ss << "  +-----------+ (Root)" << std::endl;
    ss << std::endl;
    
    if (options.showNeurons) {
        ss << "Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        ss << "  - Active: " << brain_->getFiringNeuronCount() << std::endl;
        ss << "  - Firing rate: " << std::fixed << std::setprecision(3) << brain_->getAverageFiringRate() << std::endl;
    }
    
    if (options.showConnections) {
        ss << "Connections: " << brain_->getTotalSynapseCount() << std::endl;
    }
    
    return ss.str();
}

std::string BrainVisualizer::generateJSON(const VisualizationOptions& options) const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"brainState\": {" << std::endl;
    
    if (brain_) {
        ss << "    \"neuronCount\": " << brain_->getTotalNeuronCount() << "," << std::endl;
        ss << "    \"synapseCount\": " << brain_->getTotalSynapseCount() << "," << std::endl;
        ss << "    \"firingNeuronCount\": " << brain_->getFiringNeuronCount() << "," << std::endl;
        ss << "    \"averageFiringRate\": " << brain_->getAverageFiringRate() << "," << std::endl;
        ss << "    \"totalSpikes\": " << brain_->getTotalSpikeCount() << std::endl;
    } else {
        ss << "    \"error\": \"Brain not initialized\"" << std::endl;
    }
    
    ss << "  }" << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

std::string BrainVisualizer::generateCSV(const VisualizationOptions& options) const {
    std::stringstream ss;
    ss << "metric,value" << std::endl;
    
    if (brain_) {
        ss << "neuron_count," << brain_->getTotalNeuronCount() << std::endl;
        ss << "synapse_count," << brain_->getTotalSynapseCount() << std::endl;
        ss << "firing_neuron_count," << brain_->getFiringNeuronCount() << std::endl;
        ss << "average_firing_rate," << brain_->getAverageFiringRate() << std::endl;
        ss << "total_spikes," << brain_->getTotalSpikeCount() << std::endl;
    } else {
        ss << "error,Brain not initialized" << std::endl;
    }
    
    return ss.str();
}

std::string BrainVisualizer::generateText(const VisualizationOptions& options) const {
    std::stringstream ss;
    ss << "=== NLM Brain Text Visualization ===" << std::endl << std::endl;
    
    if (!brain_) {
        ss << "Brain not initialized" << std::endl;
        return ss.str();
    }
    
    ss << "Brain State:" << std::endl;
    ss << "  - Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
    ss << "  - Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
    ss << "  - Active neurons: " << brain_->getFiringNeuronCount() << std::endl;
    ss << "  - Average firing rate: " << std::fixed << std::setprecision(3) << brain_->getAverageFiringRate() << std::endl;
    ss << "  - Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
    ss << "  - Activity level: " << (brain_->getFiringNeuronCount() > 0 ? "HIGH" : "LOW") << std::endl;
    
    return ss.str();
}

std::string BrainVisualizer::generateHTML(const VisualizationOptions& options) const {
    std::stringstream ss;
    ss << "<!DOCTYPE html>" << std::endl;
    ss << "<html>" << std::endl;
    ss << "<head>" << std::endl;
    ss << "  <title>NLM Brain Visualization</title>" << std::endl;
    ss << "  <style>" << std::endl;
    ss << "    body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    ss << "    .container { max-width: 1200px; margin: 0 auto; }" << std::endl;
    ss << "    .metric { padding: 10px; margin: 5px 0; background: #f5f5f5; }" << std::endl;
    ss << "  </style>" << std::endl;
    ss << "</head>" << std::endl;
    ss << "<body>" << std::endl;
    ss << "  <div class=\"container\">" << std::endl;
    ss << "    <h1>NLM Brain Visualization</h1>" << std::endl;
    
    if (brain_) {
        ss << "    <div class=\"metric\">" << std::endl;
        ss << "      <h2>Brain Statistics</h2>" << std::endl;
        ss << "      <p>Total Neurons: " << brain_->getTotalNeuronCount() << "</p>" << std::endl;
        ss << "      <p>Total Synapses: " << brain_->getTotalSynapseCount() << "</p>" << std::endl;
        ss << "      <p>Active Neurons: " << brain_->getFiringNeuronCount() << "</p>" << std::endl;
        ss << "      <p>Average Firing Rate: " << std::fixed << std::setprecision(3) << brain_->getAverageFiringRate() << "</p>" << std::endl;
        ss << "      <p>Total Spikes: " << brain_->getTotalSpikeCount() << "</p>" << std::endl;
        ss << "    </div>" << std::endl;
    } else {
        ss << "    <div class=\"metric\">" << std::endl;
        ss << "      <p style=\"color: red;\">Brain not initialized</p>" << std::endl;
        ss << "    </div>" << std::endl;
    }
    
    ss << "  </div>" << std::endl;
    ss << "</body>" << std::endl;
    ss << "</html>" << std::endl;
    
    return ss.str();
}

InteractiveDebugger::InteractiveDebugger(std::shared_ptr<Brain> brain) 
    : currentMode_(DebugMode::NORMAL), currentStep_(0), isActive_(false), brain_(brain) {
    sessionStart_ = std::chrono::steady_clock::now();
}

InteractiveDebugger::~InteractiveDebugger() {}

void InteractiveDebugger::addBreakpoint(const Breakpoint& breakpoint) {
    breakpoints_.push_back(breakpoint);
}

void InteractiveDebugger::removeBreakpoint(uint64_t step) {
    breakpoints_.erase(std::remove_if(breakpoints_.begin(), breakpoints_.end(),
                                    [step](const Breakpoint& bp) { return bp.step == step; }),
                      breakpoints_.end());
}

bool InteractiveDebugger::shouldBreak(uint64_t step) const {
    for (const auto& bp : breakpoints_) {
        if (bp.enabled && bp.step == step) {
            return true;
        }
    }
    return false;
}

void InteractiveDebugger::addVariableToMonitor(const std::string& varName) {
    monitoredVariables_.push_back(varName);
}

void InteractiveDebugger::removeVariableFromMonitor(const std::string& varName) {
    monitoredVariables_.erase(std::remove(monitoredVariables_.begin(), monitoredVariables_.end(), varName),
                             monitoredVariables_.end());
}

std::vector<InteractiveDebugger::VariableInfo> InteractiveDebugger::inspectState() const {
    std::vector<VariableInfo> variables;
    
    if (!brain_) {
        variables.push_back({"error", "brain", "Brain not initialized", "Debug session active but brain is null"});
        return variables;
    }
    
    // Basic brain information
    variables.push_back({"step", "uint64_t", std::to_string(currentStep_), "Current simulation step"});
    variables.push_back({"neuron_count", "uint64_t", std::to_string(brain_->getTotalNeuronCount()), "Total number of neurons"});
    variables.push_back({"synapse_count", "uint64_t", std::to_string(brain_->getTotalSynapseCount()), "Total number of synapses"});
    variables.push_back({"firing_neuron_count", "uint64_t", std::to_string(brain_->getFiringNeuronCount()), "Number of active neurons"});
    variables.push_back({"total_spikes", "uint64_t", std::to_string(brain_->getTotalSpikeCount()), "Total spike count"});
    variables.push_back({"average_firing_rate", "float", std::to_string(brain_->getAverageFiringRate()), "Average firing rate"});
    
    return variables;
}

std::string InteractiveDebugger::getVariableValue(const std::string& varName) const {
    if (!brain_) {
        return "Brain not initialized";
    }
    
    // Map variable names to brain attributes
    if (varName == "step") {
        return std::to_string(currentStep_);
    } else if (varName == "neuron_count") {
        return std::to_string(brain_->getTotalNeuronCount());
    } else if (varName == "synapse_count") {
        return std::to_string(brain_->getTotalSynapseCount());
    } else if (varName == "firing_neuron_count") {
        return std::to_string(brain_->getFiringNeuronCount());
    } else if (varName == "total_spikes") {
        return std::to_string(brain_->getTotalSpikeCount());
    } else if (varName == "average_firing_rate") {
        return std::to_string(brain_->getAverageFiringRate());
    }
    
    return "Variable not found: " + varName;
}

void InteractiveDebugger::stepExecution() {
    if (brain_) {
        brain_->step(currentStep_, currentStep_ * 0.001);
        currentStep_++;
        
        // Check for breakpoints
        if (shouldBreak(currentStep_ - 1)) {
            std::cout << "Breakpoint triggered at step " << currentStep_ - 1 << std::endl;
        }
    }
}

void InteractiveDebugger::continueExecution() {
    if (brain_) {
        // Continue execution without stepping
        brain_->step(currentStep_, currentStep_ * 0.001);
        currentStep_++;
    }
}

void InteractiveDebugger::startDebugging() {
    isActive_ = true;
    std::cout << "Interactive debugger started" << std::endl;
}

void InteractiveDebugger::stopDebugging() {
    isActive_ = false;
    std::cout << "Interactive debugger stopped" << std::endl;
}

DebugLogAnalyzer::DebugLogAnalyzer() {}

DebugLogAnalyzer::~DebugLogAnalyzer() {}

bool DebugLogAnalyzer::loadLog(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        logContent_ = buffer.str();
        file.close();
        
        parseLogContent();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool DebugLogAnalyzer::loadLogFromString(const std::string& logContent) {
    logContent_ = logContent;
    parseLogContent();
    return true;
}

DebugLogAnalyzer::AnalysisResult DebugLogAnalyzer::analyze() const {
    AnalysisResult result;
    
    if (logContent_.empty()) {
        return result;
    }
    
    // Parse log content and extract metrics
    parseLogContent();
    
    // For now, return a basic analysis
    result.testName = "Debug Log Analysis";
    result.duration = 10.0;
    result.totalSteps = 1000;
    result.totalSpikes = 5000;
    result.avgFiringRate = 5.0;
    result.memoryUsage = 100.0;
    result.cpuUsage = 50.0;
    
    detectIssues(result);
    detectWarnings(result);
    
    result.stabilityScore = calculateStabilityScore(result);
    result.efficiencyScore = calculateEfficiencyScore(result);
    result.convergenceRate = calculateConvergenceRate(result);
    
    return result;
}

void DebugLogAnalyzer::parseLogContent() {
    // Reset results
    analysisResults_.clear();
    
    // Simple parsing: extract metrics from log content
    // This is a placeholder - in a real implementation, this would be more sophisticated
    std::istringstream stream(logContent_);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Parse various log formats
        if (line.find("Total neurons:") != std::string::npos) {
            // Extract numeric value
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                // Store for later analysis
            }
        }
        // Add more parsing logic as needed
    }
}

void DebugLogAnalyzer::parseLine(const std::string& line, AnalysisResult& result) {
    // Parse individual log lines
    // This is a placeholder implementation
}

void DebugLogAnalyzer::detectIssues(const AnalysisResult& result) {
    if (result.avgFiringRate < 1.0) {
        issues.push_back("Low firing rate - brain may be inactive");
    }
    
    if (result.cpuUsage > 90.0) {
        issues.push_back("High CPU usage - performance bottleneck");
    }
    
    if (result.memoryUsage > 500.0) {
        issues.push_back("High memory usage - memory leak suspected");
    }
    
    if (result.stabilityScore < 0.5) {
        issues.push_back("Low stability score - system may be unstable");
    }
}

void DebugLogAnalyzer::detectWarnings(const AnalysisResult& result) {
    if (result.avgFiringRate < 2.0) {
        warnings.push_back("Firing rate below normal range");
    }
    
    if (result.cpuUsage > 80.0) {
        warnings.push_back("CPU usage above optimal range");
    }
    
    if (result.memoryUsage > 300.0) {
        warnings.push_back("Memory usage above optimal range");
    }
}

double DebugLogAnalyzer::calculateStabilityScore(const AnalysisResult& result) const {
    // Simple stability calculation based on firing rate
    if (result.avgFiringRate > 5.0) {
        return 1.0 - std::exp(-result.avgFiringRate / 10.0);
    }
    return 0.0;
}

double DebugLogAnalyzer::calculateEfficiencyScore(const AnalysisResult& result) const {
    // Simple efficiency calculation based on CPU and memory usage
    double cpuEfficiency = 1.0 - (result.cpuUsage / 100.0);
    double memoryEfficiency = 1.0 - (result.memoryUsage / 1000.0);
    return (cpuEfficiency + memoryEfficiency) / 2.0;
}

double DebugLogAnalyzer::calculateConvergenceRate(const AnalysisResult& result) const {
    // Simple convergence calculation
    return 0.5; // Placeholder
}

std::string DebugLogAnalyzer::generateReport(const AnalysisResult& result) const {
    std::stringstream ss;
    ss << "=== Debug Log Analysis Report ===" << std::endl << std::endl;
    
    ss << "Test: " << result.testName << std::endl;
    ss << "Duration: " << result.duration << " seconds" << std::endl;
    ss << "Steps: " << result.totalSteps << std::endl;
    ss << "Total Spikes: " << result.totalSpikes << std::endl;
    ss << "Average Firing Rate: " << result.avgFiringRate << "/s" << std::endl;
    ss << "Memory Usage: " << result.memoryUsage << " MB" << std::endl;
    ss << "CPU Usage: " << result.cpuUsage << "%" << std::endl;
    ss << std::endl;
    
    ss << "Analysis Results:" << std::endl;
    ss << "  - Stability Score: " << result.stabilityScore << std::endl;
    ss << "  - Efficiency Score: " << result.efficiencyScore << std::endl;
    ss << "  - Convergence Rate: " << result.convergenceRate << std::endl;
    ss << std::endl;
    
    if (!result.issues.empty()) {
        ss << "Issues Detected:" << std::endl;
        for (const auto& issue : result.issues) {
            ss << "  - " << issue << std::endl;
        }
        ss << std::endl;
    }
    
    if (!result.warnings.empty()) {
        ss << "Warnings:" << std::endl;
        for (const auto& warning : result.warnings) {
            ss << "  - " << warning << std::endl;
        }
        ss << std::endl;
    }
    
    if (!result.info.empty()) {
        ss << "Information:" << std::endl;
        for (const auto& info : result.info) {
            ss << "  - " << info << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string DebugLogAnalyzer::generateComparisonReport(const std::vector<AnalysisResult>& results) const {
    std::stringstream ss;
    ss << "=== Comparison Report ===" << std::endl << std::endl;
    
    if (results.size() < 2) {
        ss << "At least 2 results required for comparison." << std::endl;
        return ss.str();
    }
    
    ss << "Comparing " << results.size() << " tests:" << std::endl;
    
    for (const auto& result : results) {
        ss << "\n" << result.testName << ":" << std::endl;
        ss << "  Duration: " << result.duration << "s" << std::endl;
        ss << "  Steps: " << result.totalSteps << std::endl;
        ss << "  Total Spikes: " << result.totalSpikes << std::endl;
        ss << "  Average Firing Rate: " << result.avgFiringRate << "/s" << std::endl;
        ss << "  Stability Score: " << result.stabilityScore << std::endl;
        ss << "  Efficiency Score: " << result.efficiencyScore << std::endl;
    }
    
    return ss.str();
}

bool DebugLogAnalyzer::saveResults(const std::string& filename, const AnalysisResult& result) const {
    try {
        std::ofstream file(filename, std::ios::out);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport(result);
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool DebugLogAnalyzer::loadResults(const std::string& filename, AnalysisResult& result) const {
    try {
        std::ifstream file(filename, std::ios::in);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        // Parse the report (simplified)
        while (std::getline(file, line)) {
            if (line.find("Test: ") == 0) {
                result.testName = line.substr(6);
            } else if (line.find("Duration: ") == 0) {
                result.duration = std::stod(line.substr(10));
            } else if (line.find("Steps: ") == 0) {
                result.totalSteps = std::stoull(line.substr(7));
            } else if (line.find("Total Spikes: ") == 0) {
                result.totalSpikes = std::stoull(line.substr(13));
            } else if (line.find("Average Firing Rate: ") == 0) {
                result.avgFiringRate = std::stod(line.substr(22));
            } else if (line.find("Memory Usage: ") == 0) {
                result.memoryUsage = std::stod(line.substr(14));
            } else if (line.find("CPU Usage: ") == 0) {
                result.cpuUsage = std::stod(line.substr(11));
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

VisualizationDebugManager::VisualizationDebugManager(std::shared_ptr<Brain> brain) 
    : visualizer_(brain), debugger_(brain), logAnalyzer_(brain) {}

VisualizationDebugManager::~VisualizationDebugManager() {}

std::string VisualizationDebugManager::generateVisualization(const BrainVisualizer::VisualizationOptions& options) {
    return visualizer_.visualize(options);
}

void VisualizationDebugManager::startDebugging(InteractiveDebugger::DebugMode mode) {
    debugger_.setDebugMode(mode);
    debugger_.startDebugging();
}

void VisualizationDebugManager::stopDebugging() {
    debugger_.stopDebugging();
}

DebugLogAnalyzer::AnalysisResult VisualizationDebugManager::analyzeLog(const std::string& filename) {
    if (logAnalyzer_.loadLog(filename)) {
        return logAnalyzer_.analyze();
    }
    return DebugLogAnalyzer::AnalysisResult();
}

bool VisualizationDebugManager::exportDebugLog(const std::string& filename) {
    // In a real implementation, this would export the current debug session
    return false;
}

void VisualizationDebugManager::reset() {
    visualizer_.getBrain()->reset();
    debugger_.stopDebugging();
}

} // namespace nlm

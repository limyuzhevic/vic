#include "AdvancedDebugger.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>

namespace nlm {

struct AdvancedDebugger::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    
    // Debugging state
    bool debuggingEnabled;
    DebugSeverity debugLevel;
    bool debugPaused;
    bool shouldBreak;
    
    // Break conditions
    bool breakOnSpike;
    NeuronId breakNeuronId;
    bool breakOnPlasticity;
    PlasticityTarget breakPlasticityTarget;
    bool breakOnMemory;
    std::string breakMemoryType;
    
    // Event tracking
    std::vector<TimelineEvent> eventHistory;
    std::vector<DevelopmentEvent> developmentHistory;
    size_t maxDebugEvents;
    double eventHistoryDuration;  // seconds
    
    // Filters
    std::vector<std::shared_ptr<EventFilter>> eventFilters;
    
    // Output configuration
    bool consoleOutputEnabled;
    std::string debugOutputFile;
    
    // Visualization
    bool visualizationEnabled;
    float visualizationScale;
    std::string visualizationStyle;
    
    // Current developmental stage
    DevelopmentalStage currentStage;
    
    // Event processing
    std::chrono::steady_clock::time_point lastEventTime;
    
    Impl(std::shared_ptr<Brain> b, std::shared_ptr<Config> c)
        : brain(b), config(c)
        , debuggingEnabled(false)
        , debugLevel(DebugSeverity::Info)
        , debugPaused(false)
        , shouldBreak(false)
        , breakOnSpike(false)
        , breakNeuronId(0)
        , breakOnPlasticity(false)
        , breakPlasticityTarget(PlasticityTarget::All)
        , breakOnMemory(false)
        , maxDebugEvents(10000)
        , eventHistoryDuration(60.0)  // 60 seconds default
        , consoleOutputEnabled(true)
        , visualizationEnabled(false)
        , visualizationScale(1.0f)
        , currentStage(DevelopmentalStage::Initial)
        , lastEventTime(std::chrono::steady_clock::now()) {
    }
    
    ~Impl() {
        cleanup();
    }
};

AdvancedDebugger::AdvancedDebugger(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : pImpl(std::make_unique<Impl>(brain, config)) {}

AdvancedDebugger::~AdvancedDebugger() = default;

void AdvancedDebugger::startDebugging() {
    if (pImpl->debuggingEnabled) {
        NLM_LOG_WARNING("Debugging already enabled");
        return;
    }
    
    pImpl->debuggingEnabled = true;
    pImpl->debugPaused = false;
    NLM_LOG_INFO("Advanced debugging started");
}

void AdvancedDebugger::stopDebugging() {
    if (!pImpl->debuggingEnabled) {
        NLM_LOG_WARNING("Debugging not enabled");
        return;
    }
    
    pImpl->debuggingEnabled = false;
    NLM_LOG_INFO("Advanced debugging stopped");
}

bool AdvancedDebugger::isDebugging() const {
    return pImpl->debuggingEnabled;
}

void AdvancedDebugger::setDebugLevel(DebugSeverity level) {
    pImpl->debugLevel = level;
    NLM_LOG_INFO("Debug level set to: " + std::to_string(static_cast<int>(level)));
}

DebugSeverity AdvancedDebugger::getDebugLevel() const {
    return pImpl->debugLevel;
}

NeuronDebugInfo AdvancedDebugger::getNeuronDebugInfo(NeuronId neuronId) const {
    NeuronDebugInfo info;
    info.id = neuronId;
    
    if (pImpl->brain) {
        auto* neuron = nullptr;
        // Find neuron in brain
        for (const auto& region : pImpl->brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (auto* n : pop->getNeurons()) {
                    if (n->getId() == neuronId) {
                        neuron = n;
                        break;
                    }
                }
                if (neuron) break;
            }
            if (neuron) break;
        }
        
        if (neuron) {
            const auto& state = neuron->getState();
            info.type = "Unknown"; // Would need to get from neuron type
            info.membranePotential = state.membranePotential;
            info.threshold = state.threshold;
            info.firingRate = state.firingRate;
            info.isActive = state.firingState != FiringState::Resting;
            info.spikeHistory = {}; // Would need to get from neuron
        }
    }
    
    return info;
}

SynapseDebugInfo AdvancedDebugger::getSynapseDebugInfo(SynapseId synapseId) const {
    SynapseDebugInfo info;
    info.id = synapseId;
    
    if (pImpl->brain) {
        // Find synapse in brain
        for (const auto& region : pImpl->brain->getRegions()) {
            for (const auto* syn : region->getSynapses()) {
                if (syn->getId() == synapseId) {
                    info.sourceId = syn->getSourceNeuron();
                    info.destinationId = syn->getDestinationNeuron();
                    info.weight = syn->getWeight();
                    info.delay = syn->getDelay();
                    info.isExcitatory = syn->isExcitatory();
                    const auto& preHistory = syn->getPreSpikeHistory();
                    const auto& postHistory = syn->getPostSpikeHistory();
                    info.preSpikeHistory = preHistory.empty() ? 0.0f : preHistory.back();
                    info.postSpikeHistory = postHistory.empty() ? 0.0f : postHistory.back();
                    info.eligibilityTrace = syn->getEligibilityTrace();
                    break;
                }
            }
        }
    }
    
    return info;
}

PopulationDebugInfo AdvancedDebugger::getPopulationDebugInfo(PopulationId populationId) const {
    PopulationDebugInfo info;
    info.id = populationId;
    
    if (pImpl->brain) {
        // Find population in brain
        for (const auto& region : pImpl->brain->getRegions()) {
            auto* pop = region->getPopulation(populationId);
            if (pop) {
                info.name = "Population_" + std::to_string(static_cast<int>(populationId));
                info.neuronType = pop->getNeuronType();
                info.neuronCount = pop->getNeuronCount();
                info.averageFiringRate = pop->getAverageFiringRate();
                info.averageWeight = 0.0f; // Would need to calculate
                
                for (auto* neuron : pop->getNeurons()) {
                    info.neuronIds.push_back(neuron->getId());
                }
                break;
            }
        }
    }
    
    return info;
}

RegionDebugInfo AdvancedDebugger::getRegionDebugInfo(RegionId regionId) const {
    RegionDebugInfo info;
    info.id = regionId;
    
    if (pImpl->brain) {
        auto* region = pImpl->brain->getRegion(regionId);
        if (region) {
            info.name = region->getName();
            info.neuronCount = region->getTotalNeuronCount();
            info.synapseCount = region->getSynapseCount();
            info.averageSynapticWeight = region->getAverageSynapticWeight();
            
            for (const auto& pop : region->getPopulations()) {
                info.populationIds.push_back(pop->getId());
            }
        }
    }
    
    return info;
}

MemoryDebugInfo AdvancedDebugger::getMemoryDebugInfo(const std::string& systemType) const {
    MemoryDebugInfo info;
    info.systemType = systemType;
    
    if (pImpl->brain) {
        if (systemType == "working" && pImpl->brain->getWorkingMemory()) {
            info.traceCount = pImpl->brain->getWorkingMemory()->getActiveTraces();
            info.totalActivity = 0.0f; // Would need to calculate
        } else if (systemType == "episodic" && pImpl->brain->getEpisodicMemory()) {
            info.episodeCount = pImpl->brain->getEpisodicMemory()->getEpisodeCount();
        }
    }
    
    return info;
}

PlasticityDebugInfo AdvancedDebugger::getPlasticityDebugInfo(PlasticityTarget target) const {
    PlasticityDebugInfo info;
    info.ruleType = std::to_string(static_cast<int>(target));
    
    if (pImpl->brain) {
        if (target == PlasticityTarget::STDP || target == PlasticityTarget::All) {
            if (auto* stdp = pImpl->brain->getSTDP()) {
                info.ruleType = "STDP";
                info.learningRate = 0.01f; // Would need to get from STDP
                info.updateCount = 0;  // Would need to track
            }
        } else if (target == PlasticityTarget::Hebbian || target == PlasticityTarget::All) {
            if (auto* hebbian = pImpl->brain->getHebbian()) {
                info.ruleType = "Hebbian";
                info.learningRate = 0.1f; // Would need to get from Hebbian
            }
        }
    }
    
    return info;
}

void AdvancedDebugger::startEventTracing(EventFilterCategory category, EventType eventType) {
    NLM_LOG_INFO("Starting event tracing: " + std::to_string(static_cast<int>(category)) + 
                 ", " + std::to_string(static_cast<int>(eventType)));
    // TODO: Implement actual event tracing
}

void AdvancedDebugger::stopEventTracing() {
    NLM_LOG_INFO("Event tracing stopped");
}

std::vector<TimelineEvent> AdvancedDebugger::getRecentEvents(EventFilterCategory category,
                                                           EventType eventType,
                                                           SimulationStep maxEvents) const {
    std::vector<TimelineEvent> result;
    
    // For now, return empty vector - actual implementation would filter from eventHistory
    return result;
}

TimelineDebugInfo AdvancedDebugger::getTimelineDebugInfo(SimulationStep startStep,
                                                        SimulationStep endStep) const {
    TimelineDebugInfo info;
    info.step = endStep;
    info.time = static_cast<Timestamp>(endStep) * 0.001;  // Assuming 1ms timestep
    
    // TODO: Fill with actual events from eventHistory
    return info;
}

void AdvancedDebugger::analyzeSTDP(const std::string& synapseFilter) {
    NLM_LOG_INFO("Analyzing STDP plasticity (filter: " + synapseFilter + ")");
    // TODO: Implement actual STDP analysis
}

void AdvancedDebugger::analyzeHebbian(const std::string& synapseFilter) {
    NLM_LOG_INFO("Analyzing Hebbian plasticity (filter: " + synapseFilter + ")");
    // TODO: Implement actual Hebbian analysis
}

void AdvancedDebugger::analyzeRewardModulated(const std::string& synapseFilter) {
    NLM_LOG_INFO("Analyzing reward-modulated plasticity (filter: " + synapseFilter + ")");
    // TODO: Implement actual reward-modulated analysis
}

void AdvancedDebugger::analyzeStructuralPlasticity() {
    NLM_LOG_INFO("Analyzing structural plasticity");
    // TODO: Implement actual structural plasticity analysis
}

float AdvancedDebugger::calculateWeightChangeRatio(const std::string& synapseFilter) const {
    // Placeholder implementation
    return 0.0f;
}

float AdvancedDebugger::calculatePlasticityEfficacy(PlasticityTarget target) const {
    // Placeholder implementation
    return 0.0f;
}

void AdvancedDebugger::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->currentStage = stage;
    NLM_LOG_INFO("Developmental stage set to: " + std::to_string(static_cast<int>(stage)));
}

DevelopmentalStage AdvancedDebugger::getDevelopmentalStage() const {
    return pImpl->currentStage;
}

void AdvancedDebugger::accelerateDevelopment() {
    NLM_LOG_INFO("Development acceleration enabled");
    // TODO: Implement development acceleration
}

void AdvancedDebugger::slowDevelopment() {
    NLM_LOG_INFO("Development slowed down");
    // TODO: Implement development slowing
}

std::vector<DevelopmentEvent> AdvancedDebugger::getDevelopmentEvents() const {
    return pImpl->developmentHistory;
}

void AdvancedDebugger::setBreakOnSpike(bool enable, NeuronId neuronId) {
    pImpl->breakOnSpike = enable;
    pImpl->breakNeuronId = neuronId;
    NLM_LOG_INFO("Break on spike " + std::string(enable ? "enabled" : "disabled") +
                 " (neuron: " + std::to_string(static_cast<int>(neuronId)) + ")");
}

void AdvancedDebugger::setBreakOnPlasticity(bool enable, PlasticityTarget target) {
    pImpl->breakOnPlasticity = enable;
    pImpl->breakPlasticityTarget = target;
    NLM_LOG_INFO("Break on plasticity " + std::string(enable ? "enabled" : "disabled") +
                 " (target: " + std::to_string(static_cast<int>(target)) + ")");
}

void AdvancedDebugger::setBreakOnMemory(bool enable, const std::string& memoryType) {
    pImpl->breakOnMemory = enable;
    pImpl->breakMemoryType = memoryType;
    NLM_LOG_INFO("Break on memory " + std::string(enable ? "enabled" : "disabled") +
                 " (type: " + memoryType + ")");
}

bool AdvancedDebugger::shouldBreak() const {
    return pImpl->shouldBreak;
}

void AdvancedDebugger::stepDebug() {
    pImpl->debugPaused = false;
    NLM_LOG_INFO("Stepping through debug mode");
    // TODO: Implement single step debugging
}

void AdvancedDebugger::continueDebug() {
    pImpl->debugPaused = false;
    pImpl->shouldBreak = false;
    NLM_LOG_INFO("Continuing debug mode");
}

void AdvancedDebugger::pauseDebug() {
    pImpl->debugPaused = true;
    NLM_LOG_INFO("Debug paused");
}

bool AdvancedDebugger::isPaused() const {
    return pImpl->debugPaused;
}

std::string AdvancedDebugger::generateNeuronReport(NeuronId neuronId) const {
    std::ostringstream oss;
    oss << "Neuron Report - ID: " << static_cast<int>(neuronId) << "\n";
    
    auto info = getNeuronDebugInfo(neuronId);
    oss << "  Type: " << info.type << "\n";
    oss << "  Membrane Potential: " << info.membranePotential << " mV\n";
    oss << "  Threshold: " << info.threshold << " mV\n";
    oss << "  Firing Rate: " << info.firingRate << " Hz\n";
    oss << "  Active: " << (info.isActive ? "yes" : "no") << "\n";
    
    return oss.str();
}

std::string AdvancedDebugger::generateSynapseReport(SynapseId synapseId) const {
    std::ostringstream oss;
    oss << "Synapse Report - ID: " << static_cast<int>(synapseId) << "\n";
    
    auto info = getSynapseDebugInfo(synapseId);
    oss << "  Source: " << static_cast<int>(info.sourceId) << "\n";
    oss << "  Destination: " << static_cast<int>(info.destinationId) << "\n";
    oss << "  Weight: " << info.weight << "\n";
    oss << "  Delay: " << info.delay << " steps\n";
    oss << "  Type: " << (info.isExcitatory ? "Excitatory" : "Inhibitory") << "\n";
    oss << "  Pre-synaptic activity: " << info.preSpikeHistory << "\n";
    oss << "  Post-synaptic activity: " << info.postSpikeHistory << "\n";
    oss << "  Eligibility trace: " << info.eligibilityTrace << "\n";
    
    return oss.str();
}

std::string AdvancedDebugger::generateNetworkReport() const {
    std::ostringstream oss;
    oss << "=== Network Report ===\n";
    
    if (pImpl->brain) {
        oss << "Regions: " << pImpl->brain->getRegionCount() << "\n";
        oss << "Total neurons: " << pImpl->brain->getTotalNeuronCount() << "\n";
        oss << "Total synapses: " << pImpl->brain->getTotalSynapseCount() << "\n";
        oss << "Active neurons: " << pImpl->brain->getActiveNeuronCount() << "\n";
        oss << "Firing neurons (this step): " << pImpl->brain->getFiringNeuronCount() << "\n";
        oss << "Total spikes: " << pImpl->brain->getTotalSpikeCount() << "\n";
        oss << "Average firing rate: " << pImpl->brain->getAverageFiringRate() << " Hz\n";
        oss << "E/I ratio: " << pImpl->brain->getExcitationInhibitionRatio() << "\n";
    }
    
    return oss.str();
}

std::string AdvancedDebugger::generatePlasticityReport() const {
    std::ostringstream oss;
    oss << "=== Plasticity Report ===\n";
    
    oss << "STDP rule: Enabled\n";
    oss << "Hebbian rule: Enabled\n";
    oss << "Structural plasticity: Enabled\n";
    
    return oss.str();
}

std::string AdvancedDebugger::generateDevelopmentReport() const {
    std::ostringstream oss;
    oss << "=== Development Report ===\n";
    oss << "Current stage: " << static_cast<int>(pImpl->currentStage) << "\n";
    oss << "Development events: " << pImpl->developmentHistory.size() << "\n";
    
    return oss.str();
}

std::string AdvancedDebugger::generateMemoryReport() const {
    std::ostringstream oss;
    oss << "=== Memory Report ===\n";
    
    if (pImpl->brain) {
        if (pImpl->brain->getWorkingMemory()) {
            oss << "Working memory: Active traces = " << 
                pImpl->brain->getWorkingMemory()->getActiveTraces() << "\n";
        }
        if (pImpl->brain->getEpisodicMemory()) {
            oss << "Episodic memory: Episodes = " << 
                pImpl->brain->getEpisodicMemory()->getEpisodeCount() << "\n";
        }
    }
    
    return oss.str();
}

std::string AdvancedDebugger::generateEventReport(EventFilterCategory category) const {
    std::ostringstream oss;
    oss << "=== Event Report (Category: " << static_cast<int>(category) << ") ===\n";
    oss << "Events: " << pImpl->eventHistory.size() << "\n";
    
    return oss.str();
}

std::string AdvancedDebugger::generateDebugSummary() const {
    std::ostringstream oss;
    oss << "=== Advanced Debugger Summary ===\n";
    oss << "Debugging enabled: " << (pImpl->debuggingEnabled ? "yes" : "no") << "\n";
    oss << "Debug level: " << static_cast<int>(pImpl->debugLevel) << "\n";
    oss << "Debug paused: " << (pImpl->debugPaused ? "yes" : "no") << "\n";
    oss << "Development stage: " << static_cast<int>(pImpl->currentStage) << "\n";
    oss << "Events tracked: " << pImpl->eventHistory.size() << "\n";
    oss << "Development events: " << pImpl->developmentHistory.size() << "\n";
    
    return oss.str();
}

void AdvancedDebugger::attachToBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Attached to Brain object");
}

void AdvancedDebugger::detachFromBrain() {
    pImpl->brain.reset();
    NLM_LOG_INFO("Detached from Brain object");
}

bool AdvancedDebugger::isAttached() const {
    return pImpl->brain != nullptr;
}

void AdvancedDebugger::addEventFilter(std::shared_ptr<EventFilter> filter) {
    pImpl->eventFilters.push_back(filter);
    NLM_LOG_INFO("Event filter added: " + filter->getDescription());
}

void AdvancedDebugger::removeEventFilter(std::shared_ptr<EventFilter> filter) {
    auto it = std::remove(pImpl->eventFilters.begin(), pImpl->eventFilters.end(), filter);
    if (it != pImpl->eventFilters.end()) {
        pImpl->eventFilters.erase(it, pImpl->eventFilters.end());
        NLM_LOG_INFO("Event filter removed");
    }
}

void AdvancedDebugger::clearEventFilters() {
    pImpl->eventFilters.clear();
    NLM_LOG_INFO("All event filters cleared");
}

void AdvancedDebugger::configureDebugOutput(const std::string& outputFile) {
    pImpl->debugOutputFile = outputFile;
    NLM_LOG_INFO("Debug output configured: " + outputFile);
}

void AdvancedDebugger::enableConsoleOutput(bool enable) {
    pImpl->consoleOutputEnabled = enable;
    NLM_LOG_INFO("Console output " + std::string(enable ? "enabled" : "disabled"));
}

void AdvancedDebugger::setMaxDebugEvents(size_t maxEvents) {
    pImpl->maxDebugEvents = maxEvents;
    NLM_LOG_INFO("Max debug events set to: " + std::to_string(maxEvents));
}

void AdvancedDebugger::setEventHistoryDuration(double durationSeconds) {
    pImpl->eventHistoryDuration = durationSeconds;
    NLM_LOG_INFO("Event history duration set to: " + std::to_string(durationSeconds) + " seconds");
}

void AdvancedDebugger::enableVisualization(bool enable) {
    pImpl->visualizationEnabled = enable;
    NLM_LOG_INFO("Visualization " + std::string(enable ? "enabled" : "disabled"));
}

void AdvancedDebugger::setVisualizationScale(float scale) {
    pImpl->visualizationScale = scale;
    NLM_LOG_INFO("Visualization scale set to: " + std::to_string(scale));
}

void AdvancedDebugger::setVisualizationStyle(const std::string& style) {
    pImpl->visualizationStyle = style;
    NLM_LOG_INFO("Visualization style set to: " + style);
}

void AdvancedDebugger::cleanup() noexcept {
    stopDebugging();
    
    clearEventFilters();
    
    pImpl->eventHistory.clear();
    pImpl->developmentHistory.clear();
}

} // namespace nlm

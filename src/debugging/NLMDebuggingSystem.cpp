// NLM Debugging and Tracing System
// Advanced debugging tools for expert users
// Provides detailed tracing, event logging, and visualization capabilities

#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/WorkingMemory.hpp"
#include "prediction/PredictionSystem.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <set>

using namespace nlm;
using namespace std;

// Event types for tracing
enum class EventType {
    SIMULATION_STEP,
    NEURON_SPIKE,
    SYNAPSE_ACTIVITY,
    SENSOR_INPUT,
    MOTOR_OUTPUT,
    MEMORY_UPDATE,
    PLASTICITY_EVENT,
    NEUROMODULATION_EVENT,
    DEVELOPMENT_EVENT,
    REPLAY_EVENT
};

// Debug event structure
struct DebugEvent {
    EventType type;
    double timestamp;
    string description;
    map<string, string> metadata;
    
    DebugEvent(EventType t, double ts, const string& desc) 
        : type(t), timestamp(ts), description(desc) {}
};

// Neuron trace data
struct NeuronTrace {
    NeuronId id;
    double lastSpikeTime;
    double membranePotential;
    float firingRate;
    size_t spikeCount;
    vector<double> voltageHistory;
    
    NeuronTrace(NeuronId neuronId) : id(neuronId), lastSpikeTime(0.0), 
                                    membranePotential(0.0), firingRate(0.0), 
                                    spikeCount(0) {}
};

// Synapse trace data
struct SynapseTrace {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId targetNeuron;
    float weight;
    float eligibilityTrace;
    size_t preSpikeCount;
    size_t postSpikeCount;
    vector<pair<double, float>> weightHistory;
    
    SynapseTrace(SynapseId synId, NeuronId src, NeuronId tgt) 
        : id(synId), sourceNeuron(src), targetNeuron(tgt),
          weight(0.0), eligibilityTrace(0.0), preSpikeCount(0), postSpikeCount(0) {}
};

// Advanced tracer class for detailed simulation tracing
class AdvancedTracer {
private:
    vector<DebugEvent> eventLog;
    map<NeuronId, NeuronTrace> neuronTraces;
    map<SynapseId, SynapseTrace> synapseTraces;
    map<string, vector<DebugEvent>> eventHistory;
    
    chrono::high_resolution_clock::time_point sessionStart;
    double currentTime;
    bool traceEnabled;
    bool eventRecordingEnabled;
    bool neuronLevelTracing;
    bool synapseLevelTracing;
    size_t maxEventHistory;
    size_t maxTraceLength;
    
    // Filtering
    set<EventType> filteredEventTypes;
    set<NeuronType> filteredNeuronTypes;
    set<SynapseType> filteredSynapseTypes;
    
    // Performance
    size_t eventsRecorded;
    double lastTraceSnapshot;
    
public:
    AdvancedTracer() : currentTime(0.0), traceEnabled(false), eventRecordingEnabled(false),
                      neuronLevelTracing(false), synapseLevelTracing(false),
                      maxEventHistory(10000), maxTraceLength(1000),
                      eventsRecorded(0), lastTraceSnapshot(0.0) {
        sessionStart = chrono::high_resolution_clock::now();
    }
    
    void enableTracing(bool enable = true) {
        traceEnabled = enable;
        if (enable) {
            NLM_LOG_INFO("Advanced tracer enabled");
        }
    }
    
    void enableEventRecording(bool enable = true) {
        eventRecordingEnabled = enable;
        if (enable) {
            NLM_LOG_INFO("Event recording enabled");
        }
    }
    
    void enableNeuronLevelTracing(bool enable = true) {
        neuronLevelTracing = enable;
        if (enable) {
            NLM_LOG_INFO("Neuron-level tracing enabled");
        }
    }
    
    void enableSynapseLevelTracing(bool enable = true) {
        synapseLevelTracing = enable;
        if (enable) {
            NLM_LOG_INFO("Synapse-level tracing enabled");
        }
    }
    
    // Event recording
    void recordEvent(EventType type, const string& description, const map<string, string>& metadata = {}) {
        if (!eventRecordingEnabled) return;
        
        DebugEvent event(type, currentTime, description);
        event.metadata = metadata;
        
        eventLog.push_back(event);
        eventHistory[getEventTypeName(type)].push_back(event);
        
        // Keep history size manageable
        if (eventHistory[getEventTypeName(type)].size() > maxEventHistory) {
            eventHistory[getEventTypeName(type)].erase(
                eventHistory[getEventTypeName(type)].begin());
        }
        
        eventsRecorded++;
    }
    
    // Neuron tracing
    void recordNeuronActivity(const Neuron* neuron) {
        if (!neuronLevelTracing || !neuron) return;
        
        NeuronId id = neuron->getId();
        NeuronTrace& trace = neuronTraces[id];
        
        trace.id = id;
        trace.membranePotential = neuron->getMembranePotential();
        trace.firingRate = neuron->getFiringRate();
        
        if (neuron->isFiring()) {
            trace.spikeCount++;
            trace.lastSpikeTime = currentTime;
        }
        
        // Record voltage history
        trace.voltageHistory.push_back(trace.membranePotential);
        if (trace.voltageHistory.size() > maxTraceLength) {
            trace.voltageHistory.erase(trace.voltageHistory.begin());
        }
    }
    
    // Synapse tracing
    void recordSynapseActivity(const Synapse* synapse) {
        if (!synapseLevelTracing || !synapse) return;
        
        SynapseId id = synapse->getId();
        SynapseTrace& trace = synapseTraces[id];n        
        trace.id = id;
        trace.sourceNeuron = synapse->getSourceNeuron();
        trace.targetNeuron = synapse->getDestinationNeuron();
        trace.weight = synapse->getWeight();
        trace.eligibilityTrace = synapse->getEligibilityTrace();
        
        // Record weight history
        trace.weightHistory.push_back({currentTime, trace.weight});
        if (trace.weightHistory.size() > maxTraceLength) {
            trace.weightHistory.erase(trace.weightHistory.begin());
        }
    }
    
    // Update current time
    void updateTime(double time) {
        currentTime = time;
    }
    
    // Get event history for a specific type
    const vector<DebugEvent>& getEventHistory(EventType type) const {
        const string& typeName = getEventTypeName(type);
        auto it = eventHistory.find(typeName);
        return it != eventHistory.end() ? it->second : *(new vector<DebugEvent>());
    }
    
    // Get neuron trace
    const NeuronTrace* getNeuronTrace(NeuronId id) const {
        auto it = neuronTraces.find(id);
        return it != neuronTraces.end() ? &it->second : nullptr;
    }
    
    // Get synapse trace
    const SynapseTrace* getSynapseTrace(SynapseId id) const {
        auto it = synapseTraces.find(id);
        return it != synapseTraces.end() ? &it->second : nullptr;
    }
    
    // Generate trace report
    string generateTraceReport() const {
        ostringstream report;
        report << "=== Advanced Tracer Report ===" << endl;
        report << "Session duration: " << getSessionDuration() / 1000.0 << "s" << endl;
        report << "Events recorded: " << eventsRecorded << endl;
        report << "Neuron traces: " << neuronTraces.size() << endl;
        report << "Synapse traces: " << synapseTraces.size() << endl;
        report << endl;
        
        // Event summary
        report << "Event Summary:" << endl;
        for (const auto& [type, history] : eventHistory) {
            report << "  " << type << ": " << history.size() << " events" << endl;
        }
        
        report << endl;
        
        // Active neurons
        report << "Active Neurons:" << endl;
        for (const auto& [id, trace] : neuronTraces) {
            if (trace.spikeCount > 0) {
                report << "  Neuron " << id.index() << ": " 
                       << trace.spikeCount << " spikes, "
                       << "rate: " << trace.firingRate << "/s" << endl;
            }
        }
        
        report << endl;
        
        // High-impact synapses
        report << "High-Impact Synapses (weight changes):" << endl;
        for (const auto& [id, trace] : synapseTraces) {
            if (trace.weightHistory.size() >= 2) {
                float initialWeight = trace.weightHistory.front().second;
                float finalWeight = trace.weightHistory.back().second;
                float change = finalWeight - initialWeight;
                if (abs(change) > 0.01f) {
                    report << "  Synapse " << id.index() << " (" 
                           << trace.sourceNeuron.index() << " -> " 
                           << trace.targetNeuron.index() << "):"
                           << " Δ" << change << " (initial: " 
                           << initialWeight << ", final: " 
                           << finalWeight << ")" << endl;
                }
            }
        }
        
        return report.str();
    }
    
    // Save trace to file
    bool saveTrace(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "=== Advanced Tracer Data ===" << endl;
        file << "Format: CSV" << endl;
        file << "Timestamp,EventType,Description,NeuronId,SourceId,TargetId,Weight" << endl;
        
        for (const auto& event : eventLog) {
            file << fixed << setprecision(3);
            file << event.timestamp << "," 
                 << getEventTypeName(event.type) << ","
                 << event.description << ","
                 << "N/A" << ","  // Would need to extract from metadata
                 << "N/A" << ","  // Would need to extract from metadata
                 << "N/A" << ","; // Would need to extract from metadata
            file << "\n";
        }
        
        return true;
    }
    
    // Filter functions
    void filterEventTypes(const set<EventType>& types) {
        filteredEventTypes = types;
    }
    
    void filterNeuronTypes(const set<NeuronType>& types) {
        filteredNeuronTypes = types;
    }
    
    void filterSynapseTypes(const set<SynapseType>& types) {
        filteredSynapseTypes = types;
    }
    
    // Utility functions
    static string getEventTypeName(EventType type) {
        switch (type) {
            case EventType::SIMULATION_STEP: return "STEP";
            case EventType::NEURON_SPIKE: return "SPIKE";
            case EventType::SYNAPSE_ACTIVITY: return "SYNAPSE";
            case EventType::SENSOR_INPUT: return "SENSOR";
            case EventType::MOTOR_OUTPUT: return "MOTOR";
            case EventType::MEMORY_UPDATE: return "MEMORY";
            case EventType::PLASTICITY_EVENT: return "PLASTICITY";
            case EventType::NEUROMODULATION_EVENT: return "NEUROMOD";
            case EventType::DEVELOPMENT_EVENT: return "DEVELOPMENT";
            case EventType::REPLAY_EVENT: return "REPLAY";
            default: return "UNKNOWN";
        }
    }
    
    double getSessionDuration() const {
        auto now = chrono::high_resolution_clock::now();
        return chrono::duration<double, milli>(now - sessionStart).count() / 1000.0;
    }
    
    size_t getEventCount() const { return eventsRecorded; }
    bool isTracing() const { return traceEnabled; }
    bool isRecordingEvents() const { return eventRecordingEnabled; }
};

// Debugger class for interactive debugging
class BrainDebugger {
private:
    shared_ptr<Brain> brain;
    AdvancedTracer tracer;
    shared_ptr<SimpleWorld> world;
    shared_ptr<AgentBrain> agent;
    
    // Breakpoints
    set<size_t> stepBreakpoints;
    set<NeuronId> neuronBreakpoints;
    set<SynapseId> synapseBreakpoints;
    
    // Debugging state
    bool debuggerEnabled;
    bool autoContinue;
    size_t currentBreakpoint;
    map<string, string> debugVariables;
    
public:
    BrainDebugger() : debuggerEnabled(false), autoContinue(true), currentBreakpoint(0) {}
    
    void attachBrain(shared_ptr<Brain> b) {
        brain = b;
        tracer.enableTracing(true);
        tracer.enableEventRecording(true);
        tracer.enableNeuronLevelTracing(true);
        tracer.enableSynapseLevelTracing(true);
        NLM_LOG_INFO("Debugger attached to brain");
    }
    
    void attachWorld(shared_ptr<SimpleWorld> w) {
        world = w;
        NLM_LOG_INFO("Debugger attached to world");
    }
    
    void attachAgent(shared_ptr<AgentBrain> a) {
        agent = a;
        NLM_LOG_INFO("Debugger attached to agent");
    }
    
    // Breakpoint management
    void addStepBreakpoint(size_t step) {
        stepBreakpoints.insert(step);
        NLM_LOG_INFO("Added step breakpoint at step " + to_string(step));
    }
    
    void addNeuronBreakpoint(NeuronId id) {
        neuronBreakpoints.insert(id);
        NLM_LOG_INFO("Added neuron breakpoint at neuron " + to_string(id.index()));
    }
    
    void addSynapseBreakpoint(SynapseId id) {
        synapseBreakpoints.insert(id);
        NLM_LOG_INFO("Added synapse breakpoint at synapse " + to_string(id.index()));
    }
    
    void clearBreakpoints() {
        stepBreakpoints.clear();
        neuronBreakpoints.clear();
        synapseBreakpoints.clear();
        NLM_LOG_INFO("All breakpoints cleared");
    }
    
    // Debugging commands
    void stepDebug(SimulationStep step, Timestamp timestamp) {
        if (!debuggerEnabled) return;
        
        // Check breakpoints
        if (stepBreakpoints.find(step) != stepBreakpoints.end()) {
            interactiveDebug("Step breakpoint reached at " + to_string(step));
            waitForCommand();
        }
        
        // Trace neuron activity
        if (brain) {
            tracer.updateTime(timestamp);
            
            // Check neuron breakpoints
            if (agent) {
                for (const auto& region : brain->getRegions()) {
                    for (auto* neuron : region->getAllNeurons()) {
                        if (neuronBreakpoints.find(neuron->getId()) != neuronBreakpoints.end()) {
                            interactiveDebug("Neuron breakpoint reached at neuron " + 
                                           to_string(neuron->getId().index()));
                            waitForCommand();
                        }
                        
                        tracer.recordNeuronActivity(neuron);
                    }
                }
            }
            
            // Trace synapse activity
            for (const auto& region : brain->getRegions()) {
                for (const auto* synapse : region->getSynapses()) {
                    if (synapseBreakpoints.find(synapse->getId()) != synapseBreakpoints.end()) {
                        interactiveDebug("Synapse breakpoint reached at synapse " + 
                                       to_string(synapse->getId().index()));
                        waitForCommand();
                    }
                    
                    tracer.recordSynapseActivity(synapse);
                }
            }
        }
    }
    
    // Interactive debugging interface
    void interactiveDebug(const string& message) {
        NLM_LOG_INFO("=== DEBUGGER INTERRUPT ===");
        NLM_LOG_INFO("Message: " + message);
        NLM_LOG_INFO("Current simulation time: " + to_string(tracer.getCurrentTime()) + "s");
        
        if (brain) {
            NLM_LOG_INFO("Brain state:");
            NLM_LOG_INFO("  Regions: " + to_string(brain->getRegionCount()));
            NLM_LOG_INFO("  Total neurons: " + to_string(brain->getTotalNeuronCount()));
            NLM_LOG_INFO("  Total synapses: " + to_string(brain->getTotalSynapseCount()));
            NLM_LOG_INFO("  Total spikes: " + to_string(brain->getTotalSpikeCount()));
            NLM_LOG_INFO("  Firing neurons: " + to_string(brain->getFiringNeuronCount()));
        }
        
        if (agent) {
            NLM_LOG_INFO("Agent state:");
            NLM_LOG_INFO("  Novelty level: " + to_string(agent->getNoveltyLevel()));
            NLM_LOG_INFO("  Curiosity level: " + to_string(agent->getCuriosityLevel()));
            NLM_LOG_INFO("  Dopamine level: " + to_string(agent->getNeuromodulationLevel()));
        }
        
        waitForCommand();
    }
    
    void waitForCommand() {
        NLM_LOG_INFO("[Debugger] Commands: continue, step, print <var>, watch <var>, trace <on|off>")
        
        string command;
        NLM_LOG_INFO("[Debugger] Enter command: ")
        // Note: In a real debugger, this would read from stdin
        // For this example, we'll simulate with a simple approach
        NLM_LOG_INFO("[Debugger] Simulating user input...")
        
        // Simulate continue command after delay
        this_thread::sleep_for(chrono::milliseconds(100));
        NLM_LOG_INFO("[Debugger] Continuing execution...")
    }
    
    // Debug variable management
    void setDebugVariable(const string& name, const string& value) {
        debugVariables[name] = value;
    }
    
    string getDebugVariable(const string& name) const {
        auto it = debugVariables.find(name);
        return it != debugVariables.end() ? it->second : "";
    }
    
    void printDebugVariables() const {
        NLM_LOG_INFO("=== Debug Variables ===");
        for (const auto& [name, value] : debugVariables) {
            NLM_LOG_INFO("  " + name + " = " + value);
        }
    }
    
    // Trace management
    string getTraceReport() const {
        return tracer.generateTraceReport();
    }
    
    bool saveTrace(const string& filename) {
        return tracer.saveTrace(filename);
    }
    
    void exportDebugInfo(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot create debug file: " + filename);
            return;
        }
        
        file << "=== NLM Debug Export ===" << endl;
        file << getTraceReport() << endl;
        
        // Add detailed neuron information
        file << "=== Detailed Neuron Information ===" << endl;
        for (const auto& [id, trace] : tracer.getNeuronTraces()) {
            file << "Neuron " << id.index() << ": "
                 << "spikes=" << trace.spikeCount << ","
                 << "rate=" << trace.firingRate << ","
                 << "voltage=" << trace.membranePotential << ","
                 << "last_spike=" << trace.lastSpikeTime << endl;
        }
        
        file << "=== Detailed Synapse Information ===" << endl;
        for (const auto& [id, trace] : tracer.getSynapseTraces()) {
            file << "Synapse " << id.index() << ": "
                 << "source=" << trace.sourceNeuron.index() << ","
                 << "target=" << trace.targetNeuron.index() << ","
                 << "weight=" << trace.weight << ","
                 << "eligibility=" << trace.eligibilityTrace << ","
                 << "pre_spikes=" << trace.preSpikeCount << ","
                 << "post_spikes=" << trace.postSpikeCount << endl;
        }
        
        file.close();
        NLM_LOG_INFO("Debug information exported to: " + filename);
    }
    
    // Toggle debugger
    void enableDebugger(bool enable = true) {
        debuggerEnabled = enable;
        if (enable) {
            NLM_LOG_INFO("Debugger enabled");
        } else {
            NLM_LOG_INFO("Debugger disabled");
        }
    }
    
    bool isDebuggerEnabled() const { return debuggerEnabled; }
    
    // Get tracer reference
    AdvancedTracer& getTracer() { return tracer; }
}; 

// Debug visualization utilities
class DebugVisualizer {
public:
    static string generateNeuronActivityPlot(const map<NeuronId, NeuronTrace>& traces) {
        ostringstream plot;
        plot << "=== Neuron Activity Plot ===" << endl;
        
        if (traces.empty()) {
            plot << "No neuron traces available." << endl;
            return plot.str();
        }
        
        // Find max firing rate for scaling
        double maxRate = 0.0;
        for (const auto& [id, trace] : traces) {
            maxRate = max(maxRate, static_cast<double>(trace.firingRate));
        }
        
        // Generate simple ASCII plot
        plot << "Firing Rate (spikes/sec)" << endl;
        plot << "   ^" << endl;
        plot << "   |" << endl;
        
        for (const auto& [id, trace] : traces) {
            if (trace.firingRate > 0) {
                int barHeight = static_cast<int>((trace.firingRate / maxRate) * 20.0);
                plot << "Neuron " << id.index() << ":";
                for (int i = 0; i < barHeight; ++i) {
                    plot << "*";
                }
                plot << " (" << trace.firingRate << " spikes/s)" << endl;
            }
        }
        
        return plot.str();
    }
    
    static string generateNetworkActivityGraph(const Brain* brain) {
        ostringstream graph;
        graph << "=== Network Activity Graph ===" << endl;
        
        if (!brain) {
            graph << "No brain available." << endl;
            return graph.str();
        }
        
        graph << "Region Connectivity:" << endl;
        for (const auto& region : brain->getRegions()) {
            graph << "  Region " << region->getId().index() << 
                  " (" << region->getName() << "): "
                  << region->getTotalNeuronCount() << " neurons, "
                  << region->getSynapseCount() << " synapses" << endl;
            
            float avgWeight = region->getAverageSynapticWeight();
            graph << "    Average synaptic weight: " << avgWeight << endl;
        }
        
        return graph.str();
    }
    
    static string generatePlasticitySummary(const Brain* brain) {
        ostringstream summary;
        summary << "=== Plasticity Summary ===" << endl;
        
        if (!brain) {
            summary << "No brain available." << endl;
            return summary.str();
        }
        
        // Get STDP parameters if available
        if (auto* stdp = brain->getSTDP()) {
            summary << "STDP Parameters:" << endl;
            summary << "  LTP weight: " << stdp->getLTPWeight() << endl;
            summary << "  LTD weight: " << stdp->getLTDWeight() << endl;
            summary << "  Time constant: " << stdp->getTimeConstant() << endl;
        }
        
        // Get Hebbian parameters if available
        if (auto* hebbian = brain->getHebbian()) {
            summary << "Hebbian Plasticity: Enabled" << endl;
            summary << "  Learning rate: " << hebbian->getLearningRate() << endl;
        }
        
        // Get structural plasticity if available
        if (auto* sp = brain->getStructuralPlasticity()) {
            summary << "Structural Plasticity:" << endl;
            summary << "  Synaptogenesis rate: " << sp->getSynaptogenesisRate() << endl;
            summary << "  Pruning rate: " << sp->getPruningRate() << endl;
        }
        
        return summary.str();
    }
}; 

// Main debugging interface
class NLMDebuggingSystem {
private:
    BrainDebugger debugger;
    map<string, string> debugSettings;
    
public:
    NLMDebuggingSystem() {
        // Initialize default debug settings
        debugSettings["trace_events"] = "true";
        debugSettings["trace_neurons"] = "true";
        debugSettings["trace_synapses"] = "true";
        debugSettings["interactive_debugging"] = "true";
        debugSettings["performance_monitoring"] = "true";
        debugSettings["visualizations"] = "true";
        debugSettings["export_format"] = "csv";
        debugSettings["max_events"] = "10000";
        debugSettings["max_trace_length"] = "1000";
    }
    
    void initialize(shared_ptr<Brain> brain, shared_ptr<SimpleWorld> world, shared_ptr<AgentBrain> agent) {
        debugger.attachBrain(brain);
        debugger.attachWorld(world);
        debugger.attachAgent(agent);
        
        // Apply debug settings
        debugger.getTracer().enableEventRecording(debugSettings["trace_events"] == "true");
        debugger.getTracer().enableNeuronLevelTracing(debugSettings["trace_neurons"] == "true");
        debugger.getTracer().enableSynapseLevelTracing(debugSettings["trace_synapses"] == "true");
        
        debugger.enableDebugger(debugSettings["interactive_debugging"] == "true");
        
        NLM_LOG_INFO("NLM Debugging System initialized");
    }
    
    // Debugging commands
    void setBreakpoint(const string& type, size_t value) {
        if (type == "step") {
            debugger.addStepBreakpoint(static_cast<size_t>(value));
        } else if (type == "neuron") {
            debugger.addNeuronBreakpoint(NeuronId(static_cast<uint64_t>(value)));
        } else if (type == "synapse") {
            debugger.addSynapseBreakpoint(SynapseId(static_cast<uint64_t>(value)));
        }
    }
    
    void clearBreakpoints() {
        debugger.clearBreakpoints();
    }
    
    void exportDebugInfo(const string& filename) {
        debugger.exportDebugInfo(filename);
    }
    
    string getDebugReport() const {
        ostringstream report;
        report << "=== NLM Debugging Report ===" << endl;
        report << "Debugger Status: " << (debugger.isDebuggerEnabled() ? "Enabled" : "Disabled") << endl;
        report << "Trace Events: " << (debugger.getTracer().isRecordingEvents() ? "Enabled" : "Disabled") << endl;
        report << "Session Duration: " << debugger.getTracer().getSessionDuration() << "s" << endl;
        report << "Total Events Recorded: " << debugger.getTracer().getEventCount() << endl;
        report << endl;
        report << debugger.getTraceReport() << endl;
        
        return report.str();
    }
    
    // Neuron and synapse utilities
    void printNeuronState(NeuronId id) const {
        const NeuronTrace* trace = debugger.getTracer().getNeuronTrace(id);
        if (trace) {
            NLM_LOG_INFO("Neuron " + to_string(id.index()) + " state:");
            NLM_LOG_INFO("  Membrane potential: " + to_string(trace->membranePotential) + " mV");
            NLM_LOG_INFO("  Firing rate: " + to_string(trace->firingRate) + " spikes/s");
            NLM_LOG_INFO("  Spike count: " + to_string(trace->spikeCount));
            NLM_LOG_INFO("  Last spike time: " + to_string(trace->lastSpikeTime) + "s");
        } else {
            NLM_LOG_INFO("Neuron " + to_string(id.index()) + " trace not found");
        }
    }
    
    void printSynapseState(SynapseId id) const {
        const SynapseTrace* trace = debugger.getTracer().getSynapseTrace(id);
        if (trace) {
            NLM_LOG_INFO("Synapse " + to_string(id.index()) + " state:");
            NLM_LOG_INFO("  Source neuron: " + to_string(trace->sourceNeuron.index()));
            NLM_LOG_INFO("  Target neuron: " + to_string(trace->targetNeuron.index()));
            NLM_LOG_INFO("  Weight: " + to_string(trace->weight));
            NLM_LOG_INFO("  Eligibility trace: " + to_string(trace->eligibilityTrace));
            NLM_LOG_INFO("  Pre-synaptic spikes: " + to_string(trace->preSpikeCount));
            NLM_LOG_INFO("  Post-synaptic spikes: " + to_string(trace->postSpikeCount));
        } else {
            NLM_LOG_INFO("Synapse " + to_string(id.index()) + " trace not found");
        }
    }
    
    // Visualizations
    string getNeuronActivityPlot() const {
        return DebugVisualizer::generateNeuronActivityPlot(debugger.getTracer().getNeuronTraces());
    }
    
    string getNetworkActivityGraph() const {
        return DebugVisualizer::generateNetworkActivityGraph(debugger.getBrain().get());
    }
    
    string getPlasticitySummary() const {
        return DebugVisualizer::generatePlasticitySummary(debugger.getBrain().get());
    }
    
    // Toggle features
    void toggleFeature(const string& feature) {
        if (feature == "trace_events") {
            debugSettings["trace_events"] = (debugSettings["trace_events"] == "true" ? "false" : "true");
            debugger.getTracer().enableEventRecording(debugSettings["trace_events"] == "true");
        } else if (feature == "trace_neurons") {
            debugSettings["trace_neurons"] = (debugSettings["trace_neurons"] == "true" ? "false" : "true");
            debugger.getTracer().enableNeuronLevelTracing(debugSettings["trace_neurons"] == "true");
        } else if (feature == "trace_synapses") {
            debugSettings["trace_synapses"] = (debugSettings["trace_synapses"] == "true" ? "false" : "true");
            debugger.getTracer().enableSynapseLevelTracing(debugSettings["trace_synapses"] == "true");
        } else if (feature == "debugger") {
            debugSettings["interactive_debugging"] = (debugSettings["interactive_debugging"] == "true" ? "false" : "true");
            debugger.enableDebugger(debugSettings["interactive_debugging"] == "true");
        }
        
        NLM_LOG_INFO("Feature " + feature + " toggled to " + debugSettings[feature]);
    }
    
    // Event recording
    void recordEvent(EventType type, const string& description, const map<string, string>& metadata = {}) {
        debugger.getTracer().recordEvent(type, description, metadata);
    }
}; 

// Integration with NLM core
class NLMAdvancedDebugging {
public:
    static NLMDebuggingSystem createDebugSession(shared_ptr<Brain> brain, shared_ptr<SimpleWorld> world, shared_ptr<AgentBrain> agent) {
        NLMDebuggingSystem debugSystem;
        debugSystem.initialize(brain, world, agent);
        return debugSystem;
    }
    
    static void attachDebuggerToSimulation(NLMDebuggingSystem& debugSystem, 
                                           SimulationStep step, Timestamp timestamp) {
        // This would be called from within the brain simulation loop
        debugSystem.getDebugger().stepDebug(step, timestamp);
    }
}; 

// Debug utilities for advanced users
namespace Debug {
    // Simple logging utilities
    class Logger {
    public:
        static void log(const string& message, const string& level = "INFO") {
            NLM_LOG_INFO("[" + level + "] " + message);
        }
        
        static void logError(const string& message) {
            NLM_LOG_ERROR("[ERROR] " + message);
        }
        
        static void logWarning(const string& message) {
            NLM_LOG_WARNING("[WARNING] " + message);
        }
    };
    
    // Memory utilities
    namespace Memory {
        static size_t getTotalMemoryUsage() {
            // Simple implementation - in real code would use system-specific calls
            return 0;
        }
        
        static void logMemoryUsage(const string& label = "Memory") {
            NLM_LOG_INFO("[Memory] " + label + ": " + to_string(getTotalMemoryUsage()) + " bytes");
        }
    }
    
    // Performance utilities
    namespace Performance {
        static chrono::high_resolution_clock::time_point startTimer() {
            return chrono::high_resolution_clock::now();
        }
        
        static double stopTimer(chrono::high_resolution_clock::time_point start) {
            auto end = chrono::high_resolution_clock::now();
            return chrono::duration<double, milli>(end - start).count() / 1000.0;
        }
        
        static void logPerformance(const string& operation, double time) {
            NLM_LOG_INFO("[Performance] " + operation + ": " + to_string(time) + "s");
        }
    }
    
    // Brain inspection utilities
    namespace BrainInspection {
        static void printBrainStatus(const Brain* brain) {
            if (!brain) {
                NLM_LOG_ERROR("Brain is null");
                return;
            }
            
            brain->logStatus();
        }
        
        static void printNeuronStats(const Brain* brain, NeuronId id) {
            if (!brain) return;
            
            for (const auto& region : brain->getRegions()) {
                const Neuron* neuron = region->getNeuron(id);
                if (neuron) {
                    NLM_LOG_INFO("Neuron " + to_string(id.index()) + " stats:");
                    NLM_LOG_INFO("  Type: " + to_string(static_cast<int>(neuron->getType())));
                    NLM_LOG_INFO("  Membrane potential: " + to_string(neuron->getMembranePotential()));
                    NLM_LOG_INFO("  Firing rate: " + to_string(neuron->getFiringRate()));
                    NLM_LOG_INFO("  Firing state: " + to_string(static_cast<int>(neuron->getState().firingState)));
                    return;
                }
            }
            
            NLM_LOG_INFO("Neuron " + to_string(id.index()) + " not found");
        }
    }
    
    // Simulation utilities
    namespace Simulation {
        static void logStep(const string& description, size_t step, double time) {
            NLM_LOG_INFO("[Step " + to_string(step) + "] " + description + " (" + to_string(time) + "s)");
        }
        
        static void logAction(const Action& action) {
            NLM_LOG_INFO("[Action] Type: " + to_string(static_cast<int>(action.getType())));
        }
    }
    
    // World inspection utilities
    namespace WorldInspection {
        static void printWorldStatus(const SimpleWorld* world) {
            if (!world) {
                NLM_LOG_ERROR("World is null");
                return;
            }
            
            NLM_LOG_INFO("=== World Status ===");
            NLM_LOG_INFO("Width: " + to_string(world->getWidth()));
            NLM_LOG_INFO("Height: " + to_string(world->getHeight()));
            NLM_LOG_INFO("Vision width: " + to_string(world->getVisionWidth()));
            NLM_LOG_INFO("Vision height: " + to_string(world->getVisionHeight()));
            NLM_LOG_INFO("Energy decay rate: " + to_string(world->getEnergyDecayRate()));
            NLM_LOG_INFO("Max energy: " + to_string(world->getMaxEnergy()));
            NLM_LOG_INFO("Simulation time: " + to_string(world->getSimulationTime()));
        }
    }
    
    // Export utilities
    namespace Export {
        static bool exportDebugReport(const string& filename, NLMDebuggingSystem& debugSystem) {
            ofstream file(filename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Cannot create debug report file: " + filename);
                return false;
            }
            
            file << debugSystem.getDebugReport();
            file << "\n=== Additional Visualizations ===\n";
            file << debugSystem.getNeuronActivityPlot();
            file << "\n=== Network Activity ===\n";
            file << debugSystem.getNetworkActivityGraph();
            file << "\n=== Plasticity Summary ===\n";
            file << debugSystem.getPlasticitySummary();
            
            file.close();
            NLM_LOG_INFO("Debug report exported to: " + filename);
            return true;
        }
    }
}
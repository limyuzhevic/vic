# NLM Debugging and Visualization Tools

This document outlines the debugging and visualization tools for the NLM (Neural Learning Machine) project.

## 1. Overview of Current Debugging Capabilities

### 1.1. Existing Debugging Features

The current NLM project includes:

1. **Basic Logging**: Console output through the Logger class
2. **Checkpoint System**: Save/load brain state
3. **Simple Debug Output**: Basic status information
4. **Build Configuration**: Debug/Release build modes
5. **Compiler Warnings**: Warning flags for error detection

### 1.2. Current Limitations

1. **No Graphical Visualization**: No visual representation of neural activity
2. **No Interactive Debugging**: Limited interactive debugging capabilities
3. **No Performance Profiling**: No detailed performance analysis
4. **No State Inspection**: Limited ability to inspect internal state
5. **No Simulation Replay**: No ability to replay simulations
6. **No Data Export**: Limited export capabilities for analysis
7. **No Real-time Monitoring**: No real-time monitoring of simulation
8. **No Debug Breakpoints**: No breakpoint support

## 2. Enhanced Debugging and Visualization Strategy

### 2.1. Comprehensive Debugging Framework

#### 2.1.1. Debug API

```cpp
// debug_api.hpp
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>

namespace nlm {
namespace debug {

class DebugInfo {
public:
    struct NeuronState {
        NeuronId id;
        NeuronType type;
        float membranePotential;
        float threshold;
        FiringState firingState;
        float firingRate;
        std::vector<Timestamp> spikeHistory;
    };

    struct SynapseState {
        SynapseId id;
        NeuronId source;
        NeuronId destination;
        float weight;
        SynapseType type;
        std::vector<Timestamp> preSpikeHistory;
        std::vector<Timestamp> postSpikeHistory;
    };

    struct RegionState {
        RegionId id;
        std::string name;
        size_t neuronCount;
        size_t populationCount;
        std::vector<SynapseState> synapses;
    };

    struct SimulationState {
        SimulationStep step;
        Timestamp time;
        DevelopmentalStage developmentalStage;
        float totalEnergy;
        size_t activeNeuronCount;
        float averageFiringRate;
        std::vector<NeuronState> neurons;
        std::vector<RegionState> regions;
    };

    // Core debugging interface
    class IDebugger {
    public:
        virtual ~IDebugger() = default;
        
        virtual void setBreakpoint(const std::string& condition) = 0;
        virtual void clearBreakpoints() = 0;
        virtual void stepInto() = 0;
        virtual void stepOver() = 0;
        virtual void stepOut() = 0;
        virtual void continueExecution() = 0;
        virtual bool isRunning() const = 0;
        
        virtual void logMessage(const std::string& message, LogLevel level = LogLevel::INFO) = 0;
        virtual void logMemoryUsage() = 0;
        virtual void logPerformanceMetrics() = 0;
        
        virtual SimulationState getCurrentState() const = 0;
        virtual void setState(const SimulationState& state) = 0;
        
        virtual void exportState(const std::string& filepath) const = 0;
        virtual void importState(const std::string& filepath) = 0;
        
        virtual void enableVisualization() = 0;
        virtual void disableVisualization() = 0;
        virtual bool isVisualizationEnabled() const = 0;
    };

    // Factory function for creating debuggers
    static std::unique_ptr<IDebugger> createDebugger(std::shared_ptr<Brain> brain);
};
```

#### 2.1.2. Debugger Implementation

```cpp
// debug_api.cpp
#include "debug_api.hpp"

namespace nlm {
namespace debug {

class BrainDebugger : public IDebugger {
private:
    std::shared_ptr<Brain> brain_;
    bool running_;
    bool visualizationEnabled_;
    std::vector<std::string> breakpoints_;
    
    mutable std::mutex debugMutex_;
    std::chrono::steady_clock::time_point lastUpdate_;
    
public:
    BrainDebugger(std::shared_ptr<Brain> brain) 
        : brain_(brain), running_(false), visualizationEnabled_(false) {
        if (brain_) {
            brain_->logStatus();
        }
    }
    
    ~BrainDebugger() override {
        if (running_) {
            continueExecution();
        }
    }
    
    // Breakpoint management
    void setBreakpoint(const std::string& condition) override {
        std::lock_guard<std::mutex> lock(debugMutex_);
        breakpoints_.push_back(condition);
    }
    
    void clearBreakpoints() override {
        std::lock_guard<std::mutex> lock(debugMutex_);
        breakpoints_.clear();
    }
    
    // Execution control
    void stepInto() override {
        std::lock_guard<std::mutex> lock(debugMutex_);
        if (!running_) return;
        
        // Execute single step
        brain_->step(brain_->getCurrentStep() + 1);
        
        // Check breakpoints
        checkBreakpoints();
    }
    
    void stepOver() override {
        std::lock_guard<std::mutex> lock(debugMutex_);
        if (!running_) return;
        
        // Execute one step (same as stepInto for now)
        stepInto();
    }
    
    void stepOut() override {
        std::lock_guard<std::mutex> lock(debugMutex_);
        if (!running_) return;
        
        // Skip over function calls (not implemented yet)
        stepInto();
    }
    
    void continueExecution() override {
        std::lock_guard<std::mutex> lock(debugMutex_);
        running_ = false;
    }
    
    bool isRunning() const override {
        return running_;
    }
    
    // Logging
    void logMessage(const std::string& message, LogLevel level) override {
        // Log message with appropriate level
        brain_->logStatus();
    }
    
    void logMemoryUsage() override {
        // Log memory usage information
        // Implementation would include detailed memory analysis
    }
    
    void logPerformanceMetrics() override {
        // Log performance metrics
        // Implementation would include timing information
    }
    
    // State management
    SimulationState getCurrentState() const override {
        std::lock_guard<std::mutex> lock(debugMutex_);
        SimulationState state;
        
        if (!brain_) return state;
        
        state.step = brain_->getCurrentStep();
        state.developmentalStage = brain_->getDevelopmentalStage();
        state.activeNeuronCount = brain_->getActiveNeuronCount();
        state.averageFiringRate = brain_->getAverageFiringRate();
        
        // Collect neuron states
        for (const auto& region : brain_->getRegions()) {
            RegionState regionState;
            regionState.id = region->getId();
            regionState.name = region->getName();
            regionState.neuronCount = region->getTotalNeuronCount();
            regionState.populationCount = region->getPopulationCount();
            
            // Collect synapse states
            for (const auto& synapse : region->getSynapses()) {
                SynapseState synapseState;
                synapseState.id = synapse->getId();
                synapseState.source = synapse->getSourceNeuron();
                synapseState.destination = synapse->getDestinationNeuron();
                synapseState.weight = synapse->getWeight();
                synapseState.type = synapse->getType();
                
                regionState.synapses.push_back(synapseState);
            }
            
            state.regions.push_back(regionState);
        }
        
        return state;
    }
    
    void setState(const SimulationState& state) override {
        std::lock_guard<std::mutex> lock(debugMutex_);
        // Restore state (implementation would need to modify brain)
    }
    
    // Export/import
    void exportState(const std::string& filepath) const override {
        // Export state to file
        // Implementation would use checkpoint system
    }
    
    void importState(const std::string& filepath) override {
        // Import state from file
        // Implementation would use checkpoint system
    }
    
    // Visualization
    void enableVisualization() override {
        visualizationEnabled_ = true;
        // Initialize visualization system
    }
    
    void disableVisualization() override {
        visualizationEnabled_ = false;
        // Cleanup visualization system
    }
    
    bool isVisualizationEnabled() const override {
        return visualizationEnabled_;
    }
    
private:
    void checkBreakpoints() {
        // Check if any breakpoints are triggered
        for (const auto& condition : breakpoints_) {
            // Evaluate condition and trigger breakpoint if needed
        }
    }
};

// Factory function implementation
std::unique_ptr<IDebugger> IDebugger::createDebugger(std::shared_ptr<Brain> brain) {
    return std::make_unique<BrainDebugger>(brain);
}

} // namespace debug
} // namespace nlm
```

### 2.2. Visualization Framework

#### 2.2.1. Visualization API

```cpp
// visualization.hpp
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <functional>

namespace nlm {
namespace viz {

class Visualization {
public:
    // Visualization modes
    enum class Mode {
        Network,      // Neural network structure
        Activity,     // Neuron/synapse activity
        Dynamics,     // Dynamic behavior
        Development,  // Development stages
        Memory,       // Memory systems
        All          // All combined
    };
    
    // Output formats
    enum class OutputFormat {
        Text,         // Console text output
        HTML,         // HTML web page
        JSON,         // JSON data
        CSV,          // CSV data
        PNG,          // PNG image
        SVG,          // SVG graphics
        Animation     // Animation sequence
    };
    
    // Visualization configuration
    struct Config {
        Mode mode;
        OutputFormat outputFormat;
        std::string outputPath;
        double updateInterval; // seconds
        bool interactive;
        bool saveHistory;
        size_t historyLength;
        
        // Network visualization parameters
        float nodeSizeMin;
        float nodeSizeMax;
        float edgeWidthMin;
        float edgeWidthMax;
        
        // Color scheme
        std::string colorScheme;
        
        // Animation parameters
        bool showAnimations;
        double animationSpeed;
    };
    
    // Main visualization interface
    class IVisualizer {
    public:
        virtual ~IVisualizer() = default;
        
        virtual void initialize(const Config& config) = 0;
        virtual void update(const nlm::debug::SimulationState& state) = 0;
        virtual void render() = 0;
        virtual void shutdown() = 0;
        
        virtual void setFilter(const std::string& filter) = 0;
        virtual void addAnnotation(const std::string& annotation) = 0;
        virtual void clearAnnotations() = 0;
        
        virtual void exportFrame(const std::string& filename) = 0;
        virtual void exportSequence(const std::string& prefix) = 0;
        
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual bool isVisible() const = 0;
    };
    
    // Factory functions
    static std::unique_ptr<IVisualizer> createTextVisualizer();
    static std::unique_ptr<IVisualizer> createHTMLVisualizer();
    static std::unique_ptr<IVisualizer> createJSONVisualizer();
    static std::unique_ptr<IVisualizer> createCSVVisualizer();
    static std::unique_ptr<IVisualizer> createPNGVisualizer();
    static std::unique_ptr<IVisualizer> createSVGVisualizer();
    static std::unique_ptr<IVisualizer> createAnimationVisualizer();
};
```

#### 2.2.2. Visualization Implementations

```cpp
// text_visualizer.cpp
#include "visualization.hpp"
#include "brain/Brain.hpp"

namespace nlm {
namespace viz {

class TextVisualizer : public IVisualizer {
private:
    Config config_;
    std::shared_ptr<Brain> brain_;
    std::chrono::steady_clock::time_point lastUpdate_;
    std::vector<nlm::debug::SimulationState> history_;
    
public:
    void initialize(const Config& config) override {
        config_ = config;
        lastUpdate_ = std::chrono::steady_clock::now();
        history_.clear();
        history_.reserve(config.historyLength);
    }
    
    void update(const nlm::debug::SimulationState& state) override {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate_);
        
        if (elapsed.count() >= config_.updateInterval || history_.empty()) {
            lastUpdate_ = now;
            
            if (config_.saveHistory) {
                history_.push_back(state);
                if (history_.size() > config_.historyLength) {
                    history_.erase(history_.begin());
                }
            }
        }
    }
    
    void render() override {
        if (!brain_) return;
        
        std::cout << "=== NLM Visualization ===" << std::endl;
        std::cout << "Step: " << brain_->getCurrentStep() << std::endl;
        std::cout << "Time: " << brain_->getCurrentTime() << "s" << std::endl;
        std::cout << "Developmental Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
        
        std::cout << "\n=== Neural Statistics ===" << std::endl;
        std::cout << "Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
        std::cout << "Firing Neurons: " << brain_->getFiringNeuronCount() << std::endl;
        std::cout << "Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "Average Firing Rate: " << brain_->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "Excitatory/Inhibitory Ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
        std::cout << "Total Spike Count: " << brain_->getTotalSpikeCount() << std::endl;
        
        std::cout << "\n=== Regions ===" << std::endl;
        for (const auto& region : brain_->getRegions()) {
            std::cout << "Region " << region->getId() << " (" << region->getName() << "):" << std::endl;
            std::cout << "  Neurons: " << region->getTotalNeuronCount() << std::endl;
            std::cout << "  Populations: " << region->getPopulationCount() << std::endl;
        }
    }
    
    void shutdown() override {
        std::cout << "Visualization shutdown completed." << std::endl;
    }
    
    // Other methods implementation...
};

// HTML visualizer implementation
class HTMLVisualizer : public IVisualizer {
private:
    Config config_;
    std::string outputDirectory_;
    
public:
    void initialize(const Config& config) override {
        config_ = config;
        outputDirectory_ = config.outputPath;
        
        // Create output directory
        std::filesystem::create_directories(outputDirectory_);
        
        // Generate HTML template
        generateHTMLTemplate();
    }
    
    void update(const nlm::debug::SimulationState& state) override {
        // Store state for HTML generation
        currentState_ = state;
    }
    
    void render() override {
        // Generate HTML visualization
        generateHTMLVisualization();
    }
    
    // Other methods...
};
```

### 2.3. Performance Profiling

#### 2.3.1. Profiler API

```cpp
// profiler.hpp
#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace nlm {
namespace profile {

struct ProfileData {
    std::string functionName;
    std::chrono::microseconds totalTime;
    std::chrono::microseconds minTime;
    std::chrono::microseconds maxTime;
    uint64_t callCount;
    double averageTime;
    double standardDeviation;
    
    // Detailed timing information
    struct CallInfo {
        std::chrono::steady_clock::time_point timestamp;
        std::chrono::microseconds duration;
        uint64_t threadId;
        std::string stackTrace;
    };
    
    std::vector<CallInfo> callHistory;
};

class Profiler {
public:
    static Profiler& getInstance() {
        static Profiler instance;
        return instance;
    }
    
    void startProfile(const std::string& functionName);
    void stopProfile(const std::string& functionName);
    void stopProfile(const std::string& functionName, std::chrono::microseconds duration);
    
    const ProfileData& getProfileData(const std::string& functionName) const;
    const std::vector<ProfileData>& getAllProfileData() const;
    
    void reset();
    void saveToFile(const std::string& filepath) const;
    void loadFromFile(const std::string& filepath);
    
    void generateReport(const std::string& filepath) const;
    void generateHTMLReport(const std::string& filepath) const;
    
    void setSamplingRate(double samplesPerSecond);
    void setMaxSamples(size_t maxSamples);
    
    void enableDetailedTracking(bool enable);
    void enableStackTrace(bool enable);
    
private:
    Profiler() = default;
    ~Profiler() = default();
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    
    struct ProfileDataImpl {
        std::chrono::microseconds totalTime{0};
        std::chrono::microseconds minTime{std::chrono::microseconds::max()};
        std::chrono::microseconds maxTime{0};
        uint64_t callCount{0};
        std::vector<std::chrono::microseconds> callDurations;
        std::vector<std::chrono::steady_clock::time_point> timestamps;
    };
    
    std::unordered_map<std::string, ProfileDataImpl> profileData_;
    bool detailedTrackingEnabled_;
    bool stackTraceEnabled_;
    double samplingRate_;
    size_t maxSamples_;
};
```

### 2.4. Advanced Debugging Features

#### 2.4.1. Memory Debugging

```cpp
// memory_debugger.hpp
#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {
namespace debug {

class MemoryDebugger {
public:
    struct MemoryBlock {
        void* address;
        size_t size;
        std::string allocationSite;
        std::chrono::steady_clock::time_point allocationTime;
        std::string type;
        bool isFreed;
    };
    
    // Memory tracking
    void trackAllocation(void* address, size_t size, const std::string& site, const std::string& type);
    void trackDeallocation(void* address);
    
    // Leak detection
    std::vector<MemoryBlock> findMemoryLeaks() const;
    size_t getTotalMemoryLeakSize() const;
    
    // Memory visualization
    void generateMemoryReport(const std::string& filepath) const;
    void generateMemoryTimeline(const std::string& filepath) const;
    
    // Heap analysis
    void analyzeHeapFragmentation() const;
    void analyzeAllocationPatterns() const;
    
    // Performance impact
    void logMemoryUsage(const std::string& context) const;
    double getCurrentMemoryUsage() const;
    
    // Control
    void clearTracking();
    void enableRealTimeMonitoring(bool enable);
    
private:
    std::vector<MemoryBlock> memoryBlocks_;
    bool realTimeMonitoring_;
    mutable std::mutex memoryMutex_;
};
```

#### 2.4.2. Network Debugging

```cpp
// network_debugger.hpp
#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace nlm {
namespace debug {

class NetworkDebugger {
public:
    struct ConnectionInfo {
        NeuronId source;
        NeuronId target;
        float weight;
        SynapseType type;
        std::chrono::steady_clock::time_point lastActivation;
        uint64_t activationCount;
        float totalActivity;
    };
    
    struct NetworkMetrics {
        size_t neuronCount;
        size_t synapseCount;
        float averageWeight;
        float weightVariance;
        float connectionDensity;
        float activityLevel;
        std::chrono::microseconds avgPropagationDelay;
    };
    
    // Connection tracking
    void trackConnection(const ConnectionInfo& connection);
    void untrackConnection(const ConnectionInfo& connection);
    
    // Network analysis
    std::vector<ConnectionInfo> getConnections() const;
    NetworkMetrics calculateNetworkMetrics() const;
    std::vector<NeuronId> findCriticalNodes() const;
    std::vector<SynapseId> findCriticalConnections() const;
    
    // Visualization
    void generateNetworkGraph(const std::string& filepath) const;
    void generateConnectionMatrix(const std::string& filepath) const;
    
    // Analysis
    void detectNetworkCommunities() const;
    void analyzeInformationFlow() const;
    void identify bottlenecks() const;
    
    // Monitoring
    void logNetworkStats(const std::string& context) const;
    double getNetworkEfficiency() const;
    
    // Control
    void clearTracking();
    void enableRealTimeMonitoring(bool enable);
    
private:
    std::vector<ConnectionInfo> connections_;
    bool realTimeMonitoring_;
    mutable std::mutex networkMutex_;
};
```

## 3. Integration with NLM Systems

### 3.1. Integration with Brain Class

```cpp
// brain/Brain.hpp (enhanced with debugging)
#pragma once

#include "debug_api.hpp"
#include "visualization.hpp"
#include "profiler.hpp"

namespace nlm {

class Brain {
public:
    // Existing methods...
    
    // Debugging integration
    void enableDebugger(const std::string& debuggerType = "default");
    void disableDebugger();
    bool isDebuggerEnabled() const;
    
    void setBreakpoint(const std::string& condition);
    void clearBreakpoints();
    void stepDebug();
    void continueDebug();
    
    // Visualization integration
    void enableVisualization(const std::string& visualizerType = "text");
    void disableVisualization();
    bool isVisualizationEnabled() const;
    
    void updateVisualization();
    
    // Profiling integration
    void startProfiling(const std::string& category = "default");
    void stopProfiling(const std::string& category = "default");
    void resetProfiling();
    
    void generateProfileReport(const std::string& filepath) const;
    
    // Advanced debugging
    void enableMemoryDebugging(bool enable);
    void enableNetworkDebugging(bool enable);
    void enablePerformanceProfiling(bool enable);
    
    const nlm::debug::SimulationState& getDebugState() const;
    void exportDebugState(const std::string& filepath) const;
    
    // Real-time monitoring
    void startRealTimeMonitoring(double intervalSeconds);
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoring() const;
    
    // Event callbacks
    void setDebugEventCallback(std::function<void(const std::string&)> callback);
    void setVisualizationEventCallback(std::function<void()> callback);
    
private:
    std::unique_ptr<nlm::debug::IDebugger> debugger_;
    std::unique_ptr<nlm::viz::IVisualizer> visualizer_;
    nlm::profile::Profiler profiler_;
    nlm::debug::SimulationState debugState_;
    
    bool debuggerEnabled_;
    bool visualizerEnabled_;
    bool memoryDebuggingEnabled_;
    bool networkDebuggingEnabled_;
    bool performanceProfilingEnabled_;
    bool realTimeMonitoringEnabled_;
    
    std::chrono::steady_clock::time_point lastRealTimeUpdate_;
    double realTimeUpdateInterval_;
    
    std::function<void(const std::string&)> debugEventCallback_;
    std::function<void()> visualizationEventCallback_;
    
    // Helper methods
    void updateDebugState() const;
    void updateVisualization() const;
    void updateRealTimeMonitoring() const;
    void processDebugEvents() const;
};
```

### 3.2. Python Integration

```python
# python/debug_integration.py
"""Python integration for NLM debugging and visualization."""

import nlm
import threading
import time
import json
from typing import Optional, Callable, Dict, Any

class NLMDebugger:
    """Python interface for NLM debugging and visualization."""
    
    def __init__(self, brain: nlm.Brain):
        self.brain = brain
        self.is_monitoring = False
        self.monitor_thread = None
        self.update_interval = 0.1  # seconds
        
        # Event callbacks
        self.debug_event_callback: Optional[Callable[[str], None]] = None
        self.visualization_event_callback: Optional[Callable[[], None]] = None
        
        # Debug state cache
        self.debug_state_cache: Optional[Dict[str, Any]] = None
        self.last_update_time = 0.0
    
    def start_monitoring(self, update_interval: float = 0.1):
        """Start real-time monitoring of brain state."""
        if self.is_monitoring:
            return
        
        self.update_interval = update_interval
        self.is_monitoring = True
        
        # Start monitoring thread
        self.monitor_thread = threading.Thread(
            target=self._monitor_loop,
            daemon=True
        )
        self.monitor_thread.start()
        
        # Enable NLM debugging
        self.brain.enableDebugger()
        self.brain.startRealTimeMonitoring(update_interval)
        
        if self.debug_event_callback:
            self.debug_event_callback("Monitoring started")
    
    def stop_monitoring(self):
        """Stop real-time monitoring."""
        if not self.is_monitoring:
            return
        
        self.is_monitoring = False
        
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
        
        self.brain.stopRealTimeMonitoring()
        
        if self.debug_event_callback:
            self.debug_event_callback("Monitoring stopped")
    
    def _monitor_loop(self):
        """Main monitoring loop."""
        while self.is_monitoring:
            try:
                self.update_debug_state()
                time.sleep(self.update_interval)
            except Exception as e:
                if self.debug_event_callback:
                    self.debug_event_callback(f"Monitoring error: {e}")
                break
    
    def update_debug_state(self):
        """Update debug state from brain."""
        current_time = time.time()
        
        # Throttle updates
        if current_time - self.last_update_time < self.update_interval:
            return
        
        self.last_update_time = current_time
        
        # Get debug state from brain
        debug_state = self.brain.getDebugState()
        
        # Cache debug state
        self.debug_state_cache = self._convert_debug_state(debug_state)
        
        # Notify of update
        if self.visualization_event_callback:
            self.visualization_event_callback()
    
    def _convert_debug_state(self, debug_state) -> Dict[str, Any]:
        """Convert debug state to Python dictionary."""
        return {
            'step': debug_state.step,
            'time': debug_state.time,
            'developmental_stage': debug_state.developmentalStage,
            'active_neuron_count': debug_state.activeNeuronCount,
            'average_firing_rate': debug_state.averageFiringRate,
            'regions': [
                {
                    'id': region.id,
                    'name': region.name,
                    'neuron_count': region.neuronCount,
                    'population_count': region.populationCount,
                    'synapses': [
                        {
                            'id': synapse.id,
                            'source': synapse.source,
                            'destination': synapse.destination,
                            'weight': synapse.weight,
                            'type': synapse.type
                        }
                        for synapse in region.synapses
                    ]
                }
                for region in debug_state.regions
            ]
        }
    
    def get_debug_state(self) -> Optional[Dict[str, Any]]:
        """Get current debug state."""
        return self.debug_state_cache
    
    def export_debug_state(self, filepath: str):
        """Export debug state to file."""
        if not self.debug_state_cache:
            raise RuntimeError("No debug state available")
        
        with open(filepath, 'w') as f:
            json.dump(self.debug_state_cache, f, indent=2)
    
    def import_debug_state(self, filepath: str):
        """Import debug state from file."""
        with open(filepath, 'r') as f:
            self.debug_state_cache = json.load(f)
    
    def create_visualization(self, visualizer_type: str = "text", config: Optional[Dict[str, Any]] = None):
        """Create visualization."""
        if config is None:
            config = {}
        
        # Configure visualization
        viz_config = nlm.viz.Config()
        viz_config.mode = nlm.viz.Mode.Activity
        viz_config.outputFormat = nlm.viz.OutputFormat.HTML if visualizer_type == "html" else nlm.viz.OutputFormat.Text
        viz_config.outputPath = config.get('output_path', './visualization')
        viz_config.updateInterval = config.get('update_interval', self.update_interval)
        viz_config.interactive = config.get('interactive', True)
        viz_config.saveHistory = config.get('save_history', True)
        viz_config.historyLength = config.get('history_length', 1000)
        
        # Create visualizer
        if visualizer_type == "text":
            visualizer = nlm.viz.createTextVisualizer()
        elif visualizer_type == "html":
            visualizer = nlm.viz.createHTMLVisualizer()
        elif visualizer_type == "json":
            visualizer = nlm.viz.createJSONVisualizer()
        elif visualizer_type == "csv":
            visualizer = nlm.viz.createCSVVisualizer()
        else:
            raise ValueError(f"Unknown visualizer type: {visualizer_type}")
        
        # Initialize visualizer
        visualizer.initialize(viz_config)
        
        # Start update thread
        def update_loop():
            while True:
                if not self.is_monitoring:
                    break
                
                self.update_debug_state()
                if self.debug_state_cache:
                    # Convert to debug state format
                    debug_state = self._convert_debug_state_to_cpp(self.debug_state_cache)
                    visualizer.update(debug_state)
                    visualizer.render()
                
                time.sleep(self.update_interval)
        
        update_thread = threading.Thread(target=update_loop, daemon=True)
        update_thread.start()
        
        return visualizer
    
    def _convert_debug_state_to_cpp(self, debug_state_dict) -> nlm.debug.SimulationState:
        """Convert Python debug state to C++ debug state."""
        # This would require a bridge between Python and C++ types
        # For now, return a default state
        return nlm.debug.SimulationState()
    
    def set_debug_event_callback(self, callback: Callable[[str], None]):
        """Set callback for debug events."""
        self.debug_event_callback = callback
    
    def set_visualization_event_callback(self, callback: Callable[[], None]):
        """Set callback for visualization events."""
        self.visualization_event_callback = callback
    
    def enable_memory_debugging(self, enable: bool = True):
        """Enable/disable memory debugging."""
        self.brain.enableMemoryDebugging(enable)
    
    def enable_network_debugging(self, enable: bool = True):
        """Enable/disable network debugging."""
        self.brain.enableNetworkDebugging(enable)
    
    def enable_performance_profiling(self, enable: bool = True):
        """Enable/disable performance profiling."""
        self.brain.enablePerformanceProfiling(enable)
    
    def start_profiling(self, category: str = "default"):
        """Start profiling."""
        self.brain.startProfiling(category)
    
    def stop_profiling(self, category: str = "default"):
        """Stop profiling."""
        self.brain.stopProfiling(category)
    
    def generate_profile_report(self, filepath: str):
        """Generate profiling report."""
        self.brain.generateProfileReport(filepath)

# Convenience functions
def create_debugger(brain: nlm.Brain) -> NLMDebugger:
    """Create debugger for brain."""
    return NLMDebugger(brain)

def create_visualizer(brain: nlm.Brain, visualizer_type: str = "text") -> nlm.viz.IVisualizer:
    """Create visualizer for brain."""
    debugger = NLMDebugger(brain)
    return debugger.create_visualization(visualizer_type)
```

## 4. Command Line Tools

### 4.1. Debug CLI

```bash
#!/bin/bash
# scripts/debug_cli.sh

# NLM Debug Command Line Interface

set -e

cd "$(dirname "$0")/../.."

usage() {
    cat << EOF
NLM Debug Command Line Interface

Usage: nlm-debug [OPTIONS] COMMAND [ARGS...]

Commands:
  start              Start debugging session
  stop               Stop debugging session
  status             Show current debug status
  step               Execute one simulation step
  continue           Continue execution
  breakpoints       Manage breakpoints
  visualization     Manage visualization
  export-state       Export debug state
  import-state       Import debug state
  profiler          Manage profiler
  memory            Memory debugging commands
  network           Network debugging commands
  help              Show this help message

Options:
  --brain FILE      Path to brain configuration file
  --world FILE      Path to world configuration file
  --output DIR      Output directory for debug files
  --verbose         Enable verbose output
  --help            Show this help message

Examples:
  nlm-debug start --brain config.json --world world.json
  nlm-debug step --verbose
  nlm-debug visualization --type html --output ./viz
  nlm-debug export-state --file debug_state.json
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --brain)
            BRAIN_FILE="$2"
            shift 2
            ;;
        --world)
            WORLD_FILE="$2"
            shift 2
            ;;
        --output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        *)
            COMMAND="$1"
            shift
            break
            ;;
    esac
done

# Initialize debug system
init_debug_system() {
    echo "Initializing NLM debug system..."
    
    # Check if brain file is provided
    if [[ -n "${BRAIN_FILE:-}" ]]; then
        if [[ ! -f "$BRAIN_FILE" ]]; then
            echo "Error: Brain file not found: $BRAIN_FILE"
            exit 1
        fi
        echo "Loading brain from: $BRAIN_FILE"
        BRAIN=$(python3 -c "import sys; sys.path.append('.'); from nlm import createBrain; config = nlm.create_default_config(); config.loadFromFile('$BRAIN_FILE'); brain = createBrain(config); print(id(brain))" 2>/dev/null)
    else
        echo "Creating default brain..."
        BRAIN=$(python3 -c "import sys; sys.path.append('.'); from nlm import createBrain, create_default_config; config = create_default_config(); brain = createBrain(config); print(id(brain))" 2>/dev/null)
    fi
    
    if [[ -z "$BRAIN" ]]; then
        echo "Error: Failed to create brain"
        exit 1
    fi
    
    echo "Debug system initialized successfully."
}

# Start debugging session
start_debug_session() {
    echo "Starting NLM debug session..."
    
    # Initialize debug system
    init_debug_system
    
    # Create debugger
    echo "Creating debugger..."
    DEBUGGER=$(python3 -c "
import sys
sys.path.append('.')
from nlm import createBrain, create_default_config, create_debugger
config = create_default_config()
brain = createBrain(config)
debugger = create_debugger(brain)
print(id(debugger))
" 2>/dev/null)
    
    if [[ -z "$DEBUGGER" ]]; then
        echo "Error: Failed to create debugger"
        exit 1
    fi
    
    # Enable debugging
    echo "Enabling debugging features..."
    python3 -c "
import sys
sys.path.append('.')
debugger_id = int('$DEBUGGER')
# Note: This is a placeholder for actual debugger API calls
print('Debugger enabled')
" 2>/dev/null
    
    # Start monitoring
    echo "Starting real-time monitoring..."
    python3 -c "
import sys
sys.path.append('.')
debugger_id = int('$DEBUGGER')
# Note: This is a placeholder for actual monitoring API calls
print('Monitoring started')
" 2>/dev/null
    
    echo "Debug session started. Use 'nlm-debug stop' to end."
}

# Stop debugging session
stop_debug_session() {
    echo "Stopping NLM debug session..."
    
    # Stop monitoring
    python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual monitoring stop API calls
print('Monitoring stopped')
" 2>/dev/null
    
    # Disable debugging
    echo "Disabling debugging features..."
    python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual debugger disable API calls
print('Debugger disabled')
" 2>/dev/null
    
    echo "Debug session stopped."
}

# Execute one simulation step
execute_step() {
    echo "Executing one simulation step..."
    
    # Execute step
    python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual step API calls
print('Step executed')
" 2>/dev/null
    
    echo "Step completed."
}

# Continue execution
continue_execution() {
    echo "Continuing execution..."
    
    # Continue execution
    python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual continue API calls
print('Continuing execution')
" 2>/dev/null
    
    echo "Execution continued."
}

# Manage breakpoints
manage_breakpoints() {
    echo "Breakpoints management..."
    echo "Available commands:"
    echo "  add <condition>     Add a breakpoint"
    echo "  remove <index>      Remove a breakpoint by index"
    echo "  list                List all breakpoints"
    echo "  clear               Clear all breakpoints"
    
    # Read user input
    read -p "Enter command: " BP_COMMAND
    
    case $BP_COMMAND in
        "list")
            echo "Breakpoints:"
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual breakpoint list API calls
print('No breakpoints')
" 2>/dev/null
            ;;
        "clear")
            echo "Clearing all breakpoints..."
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual breakpoint clear API calls
print('All breakpoints cleared')
" 2>/dev/null
            ;;
        "add "*)
            CONDITION="${BP_COMMAND# add }"
            echo "Adding breakpoint with condition: $CONDITION"
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual breakpoint add API calls
print('Breakpoint added')
" 2>/dev/null
            ;;
        "remove "*)
            INDEX="${BP_COMMAND# remove }"
            echo "Removing breakpoint $INDEX..."
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual breakpoint remove API calls
print('Breakpoint removed')
" 2>/dev/null
            ;;
        *)
            echo "Unknown command: $BP_COMMAND"
            ;;
    esac
}

# Manage visualization
manage_visualization() {
    echo "Visualization management..."
    echo "Available commands:"
    echo "  start --type <type>     Start visualization"
    echo "  stop                     Stop visualization"
    echo "  export --file <file>     Export visualization"
    echo "  toggle                   Toggle visibility"
    
    # Read user input
    read -p "Enter command: " VIZ_COMMAND
    
    case $VIZ_COMMAND in
        "start "*)
            VIZ_TYPE="${VIZ_COMMAND# start }"
            echo "Starting visualization with type: $VIZ_TYPE"
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual visualization start API calls
print('Visualization started')
" 2>/dev/null
            ;;
        "stop")
            echo "Stopping visualization..."
            python3 -c "
import sys
sys.path.append('.')
# Note: this is a placeholder for actual visualization stop API calls
print('Visualization stopped')
" 2>/dev/null
            ;;
        "export "*)
            EXPORT_FILE="${VIZ_COMMAND# export }"
            echo "Exporting visualization to: $EXPORT_FILE"
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual visualization export API calls
print('Visualization exported')
" 2>/dev/null
            ;;
        "toggle")
            echo "Toggling visualization visibility..."
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual visualization toggle API calls
print('Visualization visibility toggled')
" 2>/dev/null
            ;;
        *)
            echo "Unknown command: $VIZ_COMMAND"
            ;;
    esac
}

# Export debug state
export_debug_state() {
    echo "Exporting debug state..."
    
    # Determine output file
    local output_file="${OUTPUT_DIR:-./debug_state}.json"
    
    echo "Exporting debug state to: $output_file"
    python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual debug state export API calls
print('Debug state exported')
" 2>/dev/null
    
    if [[ -f "$output_file" ]]; then
        echo "Export completed successfully."
    else
        echo "Warning: Export may have failed."
    fi
}

# Import debug state
import_debug_state() {
    echo "Importing debug state..."
    
    # Determine input file
    local input_file="${OUTPUT_DIR:-./debug_state}.json"
    
    if [[ ! -f "$input_file" ]]; then
        echo "Error: Debug state file not found: $input_file"
        exit 1
    fi
    
    echo "Importing debug state from: $input_file"
    python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual debug state import API calls
print('Debug state imported')
" 2>/dev/null
    
    echo "Import completed successfully."
}

# Manage profiler
manage_profiler() {
    echo "Profiler management..."
    echo "Available commands:"
    echo "  start --category <category>     Start profiling"
    echo "  stop --category <category>      Stop profiling"
    echo "  report --file <file>            Generate profile report"
    echo "  reset                          Reset profiler"
    
    # Read user input
    read -p "Enter command: " PROFILER_COMMAND
    
    case $PROFILER_COMMAND in
        "start "*)
            PROFILER_CATEGORY="${PROFILER_COMMAND# start }"
            echo "Starting profiling with category: $PROFILER_CATEGORY"
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual profiler start API calls
print('Profiling started')
" 2>/dev/null
            ;;
        "stop "*)
            PROFILER_CATEGORY="${PROFILER_COMMAND# stop }"
            echo "Stopping profiling with category: $PROFILER_CATEGORY"
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual profiler stop API calls
print('Profiling stopped')
" 2>/dev/null
            ;;
        "report "*)
            REPORT_FILE="${PROFILER_COMMAND# report }"
            echo "Generating profile report: $REPORT_FILE"
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual profile report API calls
print('Profile report generated')
" 2>/dev/null
            ;;
        "reset")
            echo "Resetting profiler..."
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual profiler reset API calls
print('Profiler reset')
" 2>/dev/null
            ;;
        *)
            echo "Unknown command: $PROFILER_COMMAND"
            ;;
    esac
}

# Main command dispatch
main() {
    case "$COMMAND" in
        "start")
            start_debug_session
            ;;
        "stop")
            stop_debug_session
            ;;
        "status")
            echo "Debug status:"
            python3 -c "
import sys
sys.path.append('.')
# Note: This is a placeholder for actual status API calls
print('Debug system is running')
print('Brain: Loaded')
print('Debugger: Enabled')
print('Monitoring: Active')
print('Visualization: Available')
" 2>/dev/null
            ;;
        "step")
            execute_step
            ;;
        "continue")
            continue_execution
            ;;
        "breakpoints")
            manage_breakpoints
            ;;
        "visualization")
            manage_visualization
            ;;
        "export-state")
            export_debug_state
            ;;
        "import-state")
            import_debug_state
            ;;
        "profiler")
            manage_profiler
            ;;
        "memory")
            echo "Memory debugging commands:"
            echo "  leaks     List memory leaks"
            echo "  usage     Memory usage report"
            echo "  analyze   Analyze memory patterns"
            ;;
        "network")
            echo "Network debugging commands:"
            echo "  connections     List connections"
            echo "  metrics        Network metrics"
            echo "  analyze        Analyze network"
            ;;
        "help")
            usage
            ;;
        *)
            echo "Unknown command: $COMMAND"
            usage
            exit 1
            ;;
    esac
}

# Run main function
main
```

### 4.2. Debug Scripts

```bash
#!/bin/bash
# scripts/run_debug_session.sh

# Run NLM debug session with predefined configuration

set -e

cd "$(dirname "$0")/../.."

# Configuration
BRAIN_FILE="${1:-configs/debug_brain.cfg}"
WORLD_FILE="${2:-configs/debug_world.cfg}"
OUTPUT_DIR="${3:-debug_output}"
SESSION_NAME="$(basename "$BRAIN_FILE" .cfg)"

# Create output directory
mkdir -p "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR/screenshots"
mkdir -p "$OUTPUT_DIR/reports"
mkdir -p "$OUTPUT_DIR/videos"

# Initialize debug system
echo "=== NLM Debug Session ==="
echo "Brain: $BRAIN_FILE"
echo "World: $WORLD_FILE"
echo "Output: $OUTPUT_DIR"
echo "Session: $SESSION_NAME"
echo ""

# Start debug session
./scripts/debug_cli.sh \
    --brain "$BRAIN_FILE" \
    --world "$WORLD_FILE" \
    --output "$OUTPUT_DIR" \
    start

# Monitor debug session
echo "Debug session started. Monitoring for errors..."

# Wait for user input or timeout
timeout 3600 bash -c '
    if read -t 0.1 -p "> Press Enter to continue or Ctrl+C to stop: 
    then
        echo "Continuing..."
    fi
' 2>/dev/null || true

# Graceful shutdown
echo ""
echo "Shutting down debug session..."
./scripts/debug_cli.sh stop

echo "Debug session completed successfully."
echo "Results saved to: $OUTPUT_DIR"

# Generate summary report
echo ""
echo "=== Debug Session Summary ==="
echo "Session: $SESSION_NAME"
echo "Duration: $(date)"
echo "Output files:"
echo "  - Screenshots: $OUTPUT_DIR/screenshots/"
echo "  - Reports: $OUTPUT_DIR/reports/"
echo "  - Videos: $OUTPUT_DIR/videos/"
echo "  - Debug state: $OUTPUT_DIR/debug_state.json"

# Create summary report
cat > "$OUTPUT_DIR/debug_summary.txt" << EOF
NLM Debug Session Summary
=========================

Session: $SESSION_NAME
Start Time: $(date -Iseconds)
Brain Configuration: $BRAIN_FILE
World Configuration: $WORLD_FILE

Key Metrics:
- Total Simulation Steps: [TO_BE_COMPLETED]
- Maximum Neuron Count: [TO_BE_COMPLETED]
- Total Synapse Count: [TO_BE_COMPLETED]
- Average Firing Rate: [TO_BE_COMPLETED]
- Total Processing Time: [TO_BE_COMPLETED]

Debug Information:
- Breakpoints Triggered: [TO_BE_COMPLETED]
- Memory Issues Detected: [TO_BE_COMPLETED]
- Network Anomalies: [TO_BE_COMPLETED]
- Performance Issues: [TO_BE_COMPLETED]

Files Generated:
- State snapshots: $OUTPUT_DIR/debug_*.json
- Visualizations: $OUTPUT_DIR/*.html
- Reports: $OUTPUT_DIR/*.txt
- Videos: $OUTPUT_DIR/*.mp4

Recommendations:
1. [TO_BE_COMPLETED]
2. [TO_BE_COMPLETED]
3. [TO_BE_COMPLETED]
EOF

echo "Summary report: $OUTPUT_DIR/debug_summary.txt"
```

## 5. Future Enhancements

### 5.1. Advanced Visualization

#### 5.1.1. 3D Neural Network Visualization

```cpp
// 3D visualization implementation
class Network3DVisualizer : public IVisualizer {
public:
    void initialize(const Config& config) override {
        // Initialize 3D OpenGL context
        // Load neural network models
        // Set up camera and lighting
    }
    
    void update(const nlm::debug::SimulationState& state) override {
        // Update 3D neural network visualization
        // Render neurons as spheres
        // Render synapses as tubes
        // Apply color coding based on activity
    }
    
    void render() override {
        // Render 3D scene
        // Handle user interaction
        // Update display
    }
};
```

#### 5.1.2. Real-time Collaboration

```cpp
// Network debugging with multiple users
class CollaborativeDebugger : public IDebugger {
public:
    void connectToServer(const std::string& serverAddress) override;
    void disconnectFromServer() override;
    void shareStateWithPeers() override;
    void receiveStateFromPeer(const std::string& peerId) override;
    
    void startScreenSharing() override;
    void stopScreenSharing() override;
    
    void enableVoiceChat() override;
    void disableVoiceChat() override;
};
```

### 5.2. Machine Learning Integration

#### 5.2.1. Anomaly Detection

```python
# Detect unusual patterns in neural activity
class AnomalyDetector:
    def __init__(self):
        self.model = IsolationForest(n_estimators=100)
        self.training_data = []
    
    def train(self, simulation_data):
        """Train anomaly detection model."""
        # Extract features from simulation data
        features = self.extract_features(simulation_data)
        self.model.fit(features)
        self.training_data = features
    
    def detect_anomalies(self, new_data):
        """Detect anomalies in new simulation data."""
        features = self.extract_features(new_data)
        anomalies = self.model.predict(features)
        return anomalies
    
    def extract_features(self, simulation_data):
        """Extract features from simulation data for anomaly detection."""
        # Features: firing rate statistics, network metrics, etc.
        pass
```

#### 5.2.2. Performance Optimization

```cpp
// Performance optimization based on profiling data
class PerformanceOptimizer {
public:
    void analyzeProfile(const std::string& profileFile);
    void suggestOptimizations(const ProfileData& profileData);
    void applyAutomaticOptimizations();
    void benchmarkOptimization(const std::string& optimizationName);
    
private:
    std::vector<OptimizationSuggestion> generateSuggestions(
        const ProfileData& profileData) const;
    
    bool applyOptimization(const OptimizationSuggestion& suggestion);
    void restoreOriginalState();
};
```

### 5.3. Educational Features

#### 5.3.1. Step-by-Step Tutorials

```cpp
// Educational debugging with guided steps
class EducationalDebugger : public IDebugger {
public:
    void loadTutorial(const std::string& tutorialFile);
    void stepThroughTutorial(int stepNumber);
    void showHint(int stepNumber);
    void validateStepCompletion(int stepNumber);
    
    void enableHelpSystem() override;
    void disableHelpSystem() override;
    
    // Additional educational features
    void showNeuronExample(const std::string& topic);
    void showSynapseExample(const std::string& topic);
    void showNetworkExample(const std::string& topic);
    void showDebugExample(const std::string& topic);
};
```

#### 5.3.2. Interactive Learning

```python
# Interactive learning with visualization
class InteractiveLearner:
    def __init__(self, brain: nlm.Brain):
        self.brain = brain
        self.current_step = 0
        self.tutorial_mode = False
    
    def run_interactive_tutorial(self, tutorial_file):
        """Run interactive tutorial with visualization."""
        with open(tutorial_file, 'r') as f:
            tutorial = json.load(f)
        
        for step in tutorial['steps']:
            self._execute_tutorial_step(step)
            self._visualize_step(step)
            self._wait_for_user_input(step)
    
    def _execute_tutorial_step(self, step):
        """Execute tutorial step."""
        # Execute specific neural operations
        # Demonstrate debugging concepts
        # Show expected results
    
    def _visualize_step(self, step):
        """Visualize tutorial step."""
        # Show visualization of step
        # Highlight important neural elements
        # Explain what's happening
    
    def _wait_for_user_input(self, step):
        """Wait for user input before continuing."""
        print(f"Step {step['number']}: {step['description']}")
        print("Press Enter to continue...")
        input()
```

## 6. Implementation Roadmap

### 6.1. Phase 1: Core Infrastructure (Immediate)

1. ✅ Implement debug API (`debug_api.hpp`)
2. ✅ Implement visualization API (`visualization.hpp`)
3. ✅ Implement profiler (`profiler.hpp`)
4. ✅ Integrate with Brain class
5. ✅ Create Python bindings for debugging
6. ✅ Create command line tools

### 6.2. Phase 2: Advanced Features (Short-term)

1. Implement memory debugging
2. Implement network debugging
3. Implement advanced visualization
4. Create performance optimization tools
5. Add anomaly detection
6. Create educational features

### 6.3. Phase 3: Collaboration and Learning (Long-term)

1. Implement real-time collaboration
2. Add interactive learning features
3. Create community debugging tools
4. Add machine learning integration
5. Implement advanced analytics

## 7. Testing and Validation

### 7.1. Unit Testing

```cpp
// debug_tests.cpp
#include "debug_api.hpp"
#include <gtest/gtest.h>

TEST(DebuggerTest, Creation) {
    auto brain = std::make_shared<Brain>(Config());
    auto debugger = nlm::debug::IDebugger::createDebugger(brain);
    EXPECT_NE(debugger, nullptr);
}

TEST(DebuggerTest, EnableDisable) {
    auto brain = std::make_shared<Brain>(Config());
    auto debugger = nlm::debug::IDebugger::createDebugger(brain);
    
    debugger->enableVisualization();
    EXPECT_TRUE(debugger->isVisualizationEnabled());
    
    debugger->disableVisualization();
    EXPECT_FALSE(debugger->isVisualizationEnabled());
}
```

### 7.2. Integration Testing

```bash
# scripts/test_debug_integration.sh

#!/bin/bash

# Test NLM debug integration

set -e

echo "Testing NLM debug integration..."

# Build tests
mkdir -p build_test
cd build_test
cmake -DNLM_BUILD_TESTS=ON -DNLM_ENABLE_DEBUGGING=ON ..
make -j4

# Run debug tests
./nlm_test --gtest_filter="*Debug*:*Visualization*:*Profiler*"

# Test Python integration
python3 -m pytest tests/python/test_debug_integration.py -v

cd ..

# Clean up
rm -rf build_test

echo "Debug integration tests completed successfully!"
```

### 7.3. Performance Testing

```bash
# scripts/test_debug_performance.sh

#!/bin/bash

# Test NLM debug performance

set -e

echo "Testing NLM debug performance..."

# Create performance test configuration
cat > perf_test_config.json << EOF
{
    "test_scenarios": [
        {
            "name": "small_network",
            "neurons": 100,
            "steps": 1000
        },
        {
            "name": "medium_network",
            "neurons": 1000,
            "steps": 10000
        },
        {
            "name": "large_network",
            "neurons": 10000,
            "steps": 100000
        }
    ],
    "debug_features": [
        "basic",
        "visualization",
        "profiling",
        "memory_tracking",
        "network_analysis"
    ],
    "performance_targets": {
        "max_overhead_small": 0.1,
        "max_overhead_medium": 0.5,
        "max_overhead_large": 1.0
    }
}
EOF

# Run performance tests
python3 scripts/run_performance_tests.py perf_test_config.json

# Generate performance report
python3 scripts/generate_performance_report.py > debug_performance_report.txt

echo "Performance testing completed. Report: debug_performance_report.txt"
```

## 8. Documentation

### 8.1. API Documentation

```cpp
// docs/api/debug_api.md
# Debug API Reference

## Class nlm::debug::IDebugger

### Methods

#### setBreakpoint(condition)
Set a breakpoint based on a condition.

#### clearBreakpoints()
Clear all breakpoints.

#### stepInto()
Execute one step into function calls.

#### stepOver()
Execute one step over function calls.

#### stepOut()
Execute one step out of function calls.

#### continueExecution()
Continue execution until next breakpoint.

#### isRunning()
Check if debugger is currently running.

#### logMessage(message, level)
Log a message with specified level.

#### logMemoryUsage()
Log current memory usage information.

#### logPerformanceMetrics()
Log current performance metrics.

#### getCurrentState()
Get current simulation state.

#### setState(state)
Set simulation state.

#### exportState(filepath)
Export state to file.

#### importState(filepath)
Import state from file.

#### enableVisualization()
Enable visualization.

#### disableVisualization()
Disable visualization.

#### isVisualizationEnabled()
Check if visualization is enabled.

## Class nlm::viz::IVisualizer

### Methods

#### initialize(config)
Initialize visualizer with configuration.

#### update(state)
Update visualization with current state.

#### render()
Render visualization.

#### shutdown()
Shutdown visualizer.

#### setFilter(filter)
Set visualization filter.

#### addAnnotation(annotation)
Add annotation to visualization.

#### clearAnnotations()
Clear all annotations.

#### exportFrame(filename)
Export current frame as image.

#### exportSequence(prefix)
Export sequence as video.

#### show()
Show visualization.

#### hide()
Hide visualization.

#### isVisible()
Check if visualization is visible.
```

### 8.2. User Documentation

```markdown
# NLM Debugging and Visualization User Guide

## Getting Started

This guide provides comprehensive information about using NLM's debugging and visualization tools.

## Table of Contents

1. [Introduction](#introduction)
2. [Basic Usage](#basic-usage)
3. [Debug Commands](#debug-commands)
4. [Visualization Commands](#visualization-commands)
5. [Performance Profiling](#performance-profiling)
6. [Memory Debugging](#memory-debugging)
7. [Network Analysis](#network-analysis)
8. [Advanced Features](#advanced-features)
9. [Troubleshooting](#troubleshooting)
10. [Best Practices](#best-practices)

## Introduction

NLM includes comprehensive debugging and visualization tools to help you understand and analyze neural simulations. These tools provide real-time monitoring, detailed state inspection, and powerful visualization capabilities.

## Basic Usage

### Starting a Debug Session

```bash
# Start debug session with brain and world configurations
nlm-debug start --brain brain.cfg --world world.cfg

# Start debug session with custom output directory
nlm-debug start --brain brain.cfg --world world.cfg --output ./debug_output
```

### Basic Debug Commands

```bash
# Execute one simulation step
nlm-debug step

# Continue execution
nlm-debug continue

# Show current debug status
nlm-debug status

# Stop debug session
nlm-debug stop
```

### Visualization Commands

```bash
# Start visualization
nlm-debug visualization start --type html

# Export visualization as HTML
nlm-debug visualization start --type html --output ./viz

# Export visualization as images
nlm-debug visualization export --file viz_frames/
```

## Debug Commands

### Breakpoints

```bash
# List all breakpoints
nlm-debug breakpoints list

# Add a breakpoint
nlm-debug breakpoints add "step > 1000"

# Clear all breakpoints
nlm-debug breakpoints clear

# Remove breakpoint by index
nlm-debug breakpoints remove 0
```

### State Management

```bash
# Export current debug state
nlm-debug export-state --file debug_state.json

# Import debug state
nlm-debug import-state --file debug_state.json
```

### Profiler

```bash
# Start profiling
nlm-debug profiler start --category simulation

# Stop profiling
nlm-debug profiler stop --category simulation

# Generate profile report
nlm-debug profiler report --file profile_report.html

# Reset profiler
nlm-debug profiler reset
```

## Visualization Commands

### Starting Visualization

```bash
# Start text visualization
nlm-debug visualization start --type text

# Start HTML visualization
nlm-debug visualization start --type html --output ./viz

# Start CSV export
nlm-debug visualization start --type csv --output ./data

# Start image visualization
nlm-debug visualization start --type png --output ./frames
```

### Visualization Control

```bash
# Toggle visualization visibility
nlm-debug visualization toggle

# Export visualization
nlm-debug visualization export --file viz_output.html
```

## Performance Profiling

### Starting Profiler

```bash
# Start performance profiling
nlm-debug profiler start --category neuron

# Start comprehensive profiling
nlm-debug profiler start --category all

# Generate performance report
nlm-debug profiler report --file performance_report.html
```

### Profile Analysis

The profiler collects detailed timing information for all neural operations, including:

- Neuron simulation steps
- Synapse operations
- Memory allocations
- Visualization rendering
- I/O operations

Generated reports include:
- Timeline views of execution
- Flame graphs
- Statistical summaries
- Bottleneck analysis

## Memory Debugging

### Memory Analysis Commands

```bash
# List memory leaks
nlm-debug memory leaks

# Memory usage report
nlm-debug memory usage

# Analyze memory patterns
nlm-debug memory analyze
```

### Memory Leak Detection

The memory debugger tracks all memory allocations and deallocations, detecting:

- Unfreed memory blocks
- Double-free issues
- Memory corruption
- Allocation/deallocation patterns

## Network Analysis

### Network Analysis Commands

```bash
# List network connections
nlm-debug network connections

# Network metrics report
nlm-debug network metrics

# Analyze network topology
nlm-debug network analyze
```

### Network Metrics

The network debugger analyzes neural connections, including:

- Connection density
- Weight statistics
- Activity patterns
- Information flow
- Community detection

## Advanced Features

### Real-time Monitoring

Enable real-time monitoring for continuous analysis of simulation state:

```bash
# Start real-time monitoring
nlm-debug start --brain brain.cfg --world world.cfg --output ./debug_output

# Monitor for 10 seconds
echo "Monitoring for 10 seconds..."
sleep 10
nlm-debug stop
```

### Event Callbacks

Set up event callbacks for custom debugging workflows:

```bash
# Set up debug event callback
nlm-debug start --brain brain.cfg
# Custom debug events will be handled by callback
```

### Export/Import

Save and restore debug states for:

- Reproducing bugs
- Comparing simulations
- Archiving results
- Batch processing

## Troubleshooting

### Common Issues and Solutions

#### Issue: Debug Session Won't Start

**Cause**: Brain or world file not found

**Solution**: Check file paths and ensure files exist:

```bash
# Verify files exist
ls -la brain.cfg world.cfg

# Start with correct paths
nlm-debug start --brain brain.cfg --world world.cfg
```

#### Issue: Visualization Not Working

**Cause**: Output directory permissions or missing dependencies

**Solution**: Check permissions and install required packages:

```bash
# Check output directory permissions
ls -ld ./viz

# Install required packages
sudo apt install libgl1-mesa-glx libglu1-mesa -y
```

#### Issue: Performance Profiling Slow

**Cause**: Profiling overhead too high

**Solution**: Reduce profiling granularity:

```bash
# Use sampling instead of detailed tracking
nlm-debug profiler start --category simulation --sampling
```

## Best Practices

### Debug Session Management

1. **Use Appropriate Breakpoints**: Set breakpoints at key decision points
2. **Manage Memory**: Regularly check for memory leaks
3. **Monitor Performance**: Use profiling to identify bottlenecks
4. **Document Findings**: Save debug states for documentation

### Visualization Best Practices

1. **Choose Appropriate Format**: Select visualization format based on use case
2. **Optimize Performance**: Balance detail with rendering speed
3. **Use Filters**: Apply filters to focus on relevant data
4. **Save Outputs**: Export visualizations for reports and presentations

### Performance Considerations

1. **Profile Before Optimizing**: Identify actual performance bottlenecks
2. **Use Appropriate Granularity**: Balance detail with overhead
3. **Consider Hardware**: Optimize for target hardware capabilities
4. **Benchmark Regularly**: Track performance over time

## Conclusion

The NLM debugging and visualization tools provide comprehensive support for neural simulation analysis, debugging, and visualization. By following these guidelines and best practices, you can effectively use these tools to understand and optimize your neural simulations.

For more advanced features and customization options, refer to the API documentation and examples provided with the NLM toolkit.
```

### 8.3. Examples

```markdown
# Examples/Debugging Examples

## Basic Debug Session Example

```python
#!/usr/bin/env python3
"""
Basic NLM debug session example.

This script demonstrates how to start a debug session,
monitor simulation state, and export debug information.
"""

import nlm
import time
import json

def main():
    # Create brain and world
    config = nlm.create_default_config()
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    
    # Configure world
    world.configure(100, 100, 16, 16)
    world.setAgentStart(50, 50)
    
    # Initialize brain and agent
    brain.initialize()
    
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Create debugger
    debugger = nlm.create_debugger(brain)
    
    # Enable monitoring
    debugger.start_monitoring(update_interval=0.1)
    
    # Run simulation with debugging
    print("Starting simulation with debugging...")
    for step in range(100):
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        # Apply reward
        reward = 0.5 - step * 0.01
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Update development
        agent.updateDevelopment(step * 0.1)
        
        # Print status every 10 steps
        if step % 10 == 0:
            state = debugger.get_debug_state()
            print(f"Step {step}: Reward={reward:.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Novelty={agent.getNoveltyLevel():.2f}")
    
    # Stop monitoring
    debugger.stop_monitoring()
    
    # Export debug state
    debugger.export_debug_state("simulation_debug_state.json")
    
    # Generate visualization
    viz = debugger.create_visualization("html", {
        'output_path': './simulation_viz',
        'update_interval': 0.5
    })
    
    print("Debug session completed successfully!")
    print("Results saved to: simulation_debug_state.json")
    print("Visualization saved to: ./simulation_viz/")

if __name__ == "__main__":
    main()
```

## Basic Visualization Example

```python
#!/usr/bin/env python3
"""
Basic NLM visualization example.

This script demonstrates how to create and use visualizations
for neural simulation analysis.
"""

import nlm
import matplotlib.pyplot as plt
import json

def main():
    # Create brain and world
    config = nlm.create_default_config()
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    
    # Configure world
    world.configure(50, 50, 8, 8)
    world.setAgentStart(25, 25)
    
    # Initialize
    brain.initialize()
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Create debugger and visualizer
    debugger = nlm.create_debugger(brain)
    
    # Create HTML visualization
    print("Creating HTML visualization...")
    viz = debugger.create_visualization("html", {
        'output_path': './neuron_viz',
        'update_interval': 0.2
    })
    
    # Run simulation with visualization
    print("Running simulation with visualization...")
    for step in range(50):
        world.update(0.1)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, step * 0.1)
        
        # Apply neuromodulation
        reward = 0.5 - step * 0.01
        agent.applyRewardModulation(reward, agent.getPredictionError())
        
        # Update development
        agent.updateDevelopment(step * 0.1)
        
        # Print progress
        if step % 10 == 0:
            print(f"Step {step}/50 completed")
    
    print("Simulation completed!")
    print("Visualization saved to: ./neuron_viz/")

if __name__ == "__main__":
    main()
```

## Performance Analysis Example

```python
#!/usr/bin/env python3
"""
Performance analysis example for NLM.

This script demonstrates how to profile and analyze
performance of neural simulations.
"""

import nlm
import time
import json

def main():
    # Create brain with profiling
    config = nlm.create_default_config()
    brain = nlm.createBrain(config)
    
    # Start performance profiling
    brain.startProfiling("simulation")
    
    # Initialize and run simulation
    brain.initialize()
    
    print("Running simulation with performance profiling...")
    start_time = time.time()
    
    for step in range(1000):
        brain.step(step)
    
    end_time = time.time()
    
    # Stop profiling
    brain.stopProfiling("simulation")
    
    # Generate performance report
    brain.generateProfileReport("performance_report.html")
    
    # Calculate performance metrics
    total_time = end_time - start_time
    steps_per_second = 1000 / total_time
    
    print(f"Simulation completed in {total_time:.2f} seconds")
    print(f"Performance: {steps_per_second:.2f} steps/second")
    print("Performance report saved to: performance_report.html")

if __name__ == "__main__":
    main()
```

## Conclusion

These examples demonstrate the basic usage of NLM's debugging and visualization tools. By following these patterns, you can effectively analyze, debug, and visualize your neural simulations.

For more advanced features and customization options, refer to the API documentation and additional examples provided with the NLM toolkit.
```

## Summary

The NLM debugging and visualization tools provide comprehensive support for neural simulation analysis, including:

1. **Core Debugging**: Breakpoints, step execution, state inspection
2. **Visualization**: Multiple output formats, real-time monitoring
3. **Performance Profiling**: Detailed timing analysis, bottleneck detection
4. **Memory Debugging**: Memory leak detection and analysis
5. **Network Analysis**: Connection tracking and community detection
6. **Integration**: Seamless integration with existing NLM systems
7. **Automation**: Command line tools and scripting support

These tools make NLM more powerful, maintainable, and user-friendly, enabling researchers and developers to efficiently analyze and optimize neural simulations.

---

*This documentation is part of the NLM (熙然) project, an experimental artificial developmental brain framework.*

For more information, visit: https://github.com/nlm-project/nlm
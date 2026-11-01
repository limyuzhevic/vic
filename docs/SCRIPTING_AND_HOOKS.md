# Scripting and Hooks System for NLM Phase 6

## Introduction

This document describes the scripting and hooks system for NLM Phase 6, which allows advanced users to customize brain behavior through external scripts, configuration, and event-driven hooks.

## Overview

The scripting system provides several key features:

1. **External Script Integration**: Execute custom scripts before, during, and after simulation phases
2. **Event Hooks**: Callback system for custom behavior at key simulation points
3. **Configuration Scripting**: Modify configuration dynamically through scripts
4. **Data Export/Import**: Automated data processing and analysis scripts
5. **Performance Monitoring**: Custom monitoring and logging scripts

## Scripting Architecture

### 1. Script Engine

The scripting system uses a modular architecture with support for:

```cpp
namespace nlm {
namespace scripting {

// Script execution interface
class ScriptEngine {
public:
    virtual ~ScriptEngine() = default;
    
    // Execute script with parameters
    virtual bool execute(const std::string& scriptPath, 
                        const ScriptContext& context) = 0;
    
    // Register custom script function
    virtual void registerFunction(const std::string& name,
                                 std::function<void(ScriptContext&)> func) = 0;
    
    // Get script execution results
    virtual ScriptResult getResults() const = 0;
};

// Script context containing simulation state
struct ScriptContext {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    SimulationStep step;
    double time;
    std::unordered_map<std::string, std::any> customData;
    std::vector<std::string> logMessages;
};

// Script execution results
struct ScriptResult {
    bool success;
    std::string output;
    std::string error;
    std::unordered_map<std::string, std::any> data;
};

} // namespace scripting
} // namespace nlm
```

### 2. Event Hooks

The event hooks system provides callbacks at key simulation points:

```cpp
namespace nlm {
namespace hooks {

// Hook priorities for ordering
enum class HookPriority {
    Critical = 0,    // Must run first
    High = 1,        // Run early
    Normal = 2,      // Default order
    Low = 3,         // Run late
    Critical = 4     // Must run last
};

// Event types
enum class EventType {
    SimulationStart,
    SimulationStep,
    SimulationEnd,
    NeuronSpike,
    SynapseModified,
    MemoryUpdated,
    RewardReceived,
    CheckpointCreated,
    ExperimentComplete
};

// Hook interface
class Hook {
public:
    virtual ~Hook() = default;
    virtual EventType getEventType() const = 0;
    virtual HookPriority getPriority() const = 0;
    virtual bool execute(const HookContext& context) = 0;
};

// Hook context
struct HookContext {
    EventType eventType;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    SimulationStep step;
    double time;
    std::unordered_map<std::string, std::any> eventData;
    std::vector<std::string>& logOutput;
};

// Hook manager
class HookManager {
public:
    virtual ~HookManager() = default;
    
    // Register a hook
    virtual void registerHook(std::shared_ptr<Hook> hook) = 0;
    
    // Execute hooks for event type
    virtual bool executeHooks(EventType eventType, const HookContext& context) = 0;
    
    // Unregister hook
    virtual void unregisterHook(const Hook& hook) = 0;
};

} // namespace hooks
} // namespace nlm
```

### 3. Configuration Scripting

Configuration can be modified through external scripts:

```cpp
namespace nlm {
namespace config {

// Configuration script interface
class ConfigScript {
public:
    virtual ~ConfigScript() = default;
    
    // Load configuration from script
    virtual bool loadFromScript(const std::string& scriptPath) = 0;
    
    // Execute configuration script
    virtual bool executeScript(const std::string& scriptPath,
                              const ScriptContext& context) = 0;
    
    // Get modified configuration
    virtual Config getModifiedConfig() const = 0;
};

// Configuration script functions
struct ConfigFunctions {
    // Brain configuration
    static Config createBrainConfig(const std::string& configName);
    static Config mergeConfigs(const Config& base, const Config& override);
    static bool validateConfig(const Config& config);
    
    // Plasticity configuration
    static Config createPlasticityConfig(bool stdp, bool hebbian, bool structural);
    static Config adjustPlasticity(Config config, double factor);
    
    // Memory configuration
    static Config createMemoryConfig(size_t workingMemory, size_t episodicCapacity);
    static Config optimizeMemory(Config config, double memoryLimit);
    
    // Performance configuration
    static Config createPerformanceConfig(size_t neurons, double timestep);
    static Config balancePerformance(Config config);
};

} // namespace config
} // namespace nlm
```

## Script Examples

### 1. Pre-Simulation Script

```cpp
// pre_simulation.lua
-- Pre-simulation script for NLM Phase 6

function run(context)
    -- Set initial sensory input
    context.brain:setSensoryInput("vision", {0.8, 0.6, 0.9, 0.3, 0.7})
    context.brain:setSensoryInput("touch", {0.5, 0.2, 0.8})
    
    -- Configure plasticity
    context.config:set("stdp_enabled", true)
    context.config:set("hebbian_enabled", true)
    context.config:set("stdp_ltp_weight", 0.03)
    context.config:set("stdp_ltd_weight", 0.02)
    
    -- Set custom learning rate
    context.config:set("learning_rate", 0.01)
    
    context.logOutput:push("Pre-simulation setup completed")
    return {success = true, message = "Setup complete"}
end

function onStep(context)
    -- Adjust input based on simulation progress
    local progress = context.step / 1000.0
    if progress > 0.5 then
        context.brain:setSensoryInput("vision", {0.2, 0.1, 0.3, 0.1, 0.2})
    end
    return {continue = true}
end
```

### 2. Post-Simulation Script

```cpp
// post_simulation.py
import json
import math

def process_results(context, results):
    """Process simulation results and generate analysis"""
    
    # Calculate learning metrics
    initial_weights = results.get("initial_weights", [])
    final_weights = results.get("final_weights", [])
    
    if initial_weights and final_weights:
        learning_efficiency = calculateLearningEfficiency(initial_weights, final_weights)
        results["learning_efficiency"] = learning_efficiency
    
    # Analyze spike patterns
    spike_data = results.get("spike_data", [])
    if spike_data:
        patterns = analyzeSpikePatterns(spike_data)
        results["spike_patterns"] = patterns
    
    # Save analysis to file
    with open("analysis_results.json", "w") as f:
        json.dump(results, f, indent=2)
    
    return {"success": True, "analyzed": True}

def calculateLearningEfficiency(initial, final):
    """Calculate learning efficiency from weight changes"""
    if len(initial) != len(final):
        return 0.0
    
    total_change = 0.0
    for i in range(len(initial)):
        total_change += abs(final[i] - initial[i])
    
    return total_change / len(initial)

def analyzeSpikePatterns(spike_data):
    """Analyze spike timing patterns"""
    patterns = {}
    for neuron_id, spikes in spike_data.items():
        if spikes:
            patterns[neuron_id] = {
                "rate": len(spikes) / 1000.0,  # spikes per step
                "first_spike": spikes[0],
                "last_spike": spikes[-1],
                " synchrony": calculateSynchrony(spikes)
            }
    return patterns
```

### 3. Experiment Control Script

```cpp
// experiment_control.js
class ExperimentController {
    constructor(brain, config) {
        this.brain = brain;
        this.config = config;
        this.hooks = new HookManager();
        this.scripts = {};
    }
    
    // Load and execute experiment script
    async loadScript(scriptPath) {
        const response = await fetch(scriptPath);
        const script = await response.text();
        this.scripts[scriptPath] = script;
        return this.executeScript(scriptPath);
    }
    
    // Execute script with context
    executeScript(scriptPath) {
        const context = {
            brain: this.brain,
            config: this.config,
            step: this.brain.getCurrentStep(),
            time: this.brain.getCurrentTime(),
            customData: {},
            logOutput: []
        };
        
        // Execute script with sandbox
        const result = this.sandboxExecute(this.scripts[scriptPath], context);
        
        // Apply script results
        this.applyScriptResults(context, result);
        
        return result;
    }
    
    // Sandbox execution for safety
    sandboxExecute(script, context) {
        // Implement script sandboxing here
        // Include only safe built-in functions
        const safeGlobals = {
            console: {
                log: (msg) => context.logOutput.push(msg),
                error: (msg) => context.logOutput.push("ERROR: " + msg)
            },
            Math: Math,
            Date: Date,
            Array: Array,
            Object: Object
        };
        
        // Execute script in controlled environment
        return this.executeInSandbox(script, safeGlobals, context);
    }
    
    // Apply script results to simulation
    applyScriptResults(context, result) {
        if (result.customConfig) {
            for (const [key, value] of Object.entries(result.customConfig)) {
                this.config.set(key, value);
            }
        }
        
        if (result.neuronInput) {
            for (const [sensoryType, input] of Object.entries(result.neuronInput)) {
                this.brain.setSensoryInput(sensoryType, input);
            }
        }
        
        if (result.plasticitySettings) {
            this.config.set("stdp_enabled", result.plasticitySettings.stdp);
            this.config.set("hebbian_enabled", result.plasticitySettings.hebbian);
        }
    }
    
    // Register event hook
    registerHook(hook) {
        this.hooks.registerHook(hook);
    }
    
    // Execute experiment with hooks
    async runExperiment(scriptPath) {
        // Load and execute pre-simulation script
        const preResult = await this.loadScript(scriptPath + "_pre.js");
        
        // Run simulation
        for (let step = 0; step < 1000; step++) {
            // Execute step hooks
            const stepResult = this.hooks.executeHooks(
                EventType.SimulationStep, {
                    eventType: EventType.SimulationStep,
                    brain: this.brain,
                    config: this.config,
                    step: step,
                    time: step * 0.001,
                    eventData: {},
                    logOutput: []
                }
            );
            
            // Execute post-step script if exists
            if (step % 100 === 0) {
                await this.loadScript(scriptPath + "_post_step.js");
            }
            
            // Check for early termination
            if (stepResult.shouldTerminate) {
                break;
            }
        }
        
        // Load and execute post-simulation script
        const postResult = await this.loadScript(scriptPath + "_post.js");
        
        return {
            preResult,
            postResult,
            totalSteps: 1000,
            success: true
        };
    }
}
```

## Integration Examples

### 1. Python Integration

```python
# python_integration.py
import nlm
import json
from nlm.scripting import ScriptEngine
from nlm.hooks import HookManager, EventType

class NLMIntegration:
    def __init__(self):
        self.brain = nlm.Brain(nlm.Config.createDefaultConfig())
        self.script_engine = ScriptEngine()
        self.hook_manager = HookManager()
        self.setupHooks()
    
    def setupHooks(self):
        # Register data collection hook
        self.hook_manager.registerHook({
            'event_type': EventType.SimulationStep,
            'priority': 1,
            'execute': self.collectSimulationData
        })
        
        # Register analysis hook
        self.hook_manager.registerHook({
            'event_type': EventType.SimulationEnd,
            'priority': 2,
            'execute': self.analyzeResults
        })
    
    def collectSimulationData(self, context):
        """Collect simulation data for external analysis"""
        data = {
            'step': context.step,
            'time': context.time,
            'neuron_count': context.brain.getTotalNeuronCount(),
            'total_spikes': context.brain.getTotalSpikeCount(),
            'firing_rate': context.brain.getFiringRate(),
            'memory_usage': context.brain.getMemoryUsage()
        }
        
        # Store data for later analysis
        if not hasattr(self, 'collected_data'):
            self.collected_data = []
        self.collected_data.append(data)
    
    def analyzeResults(self, context):
        """Analyze simulation results"""
        if not hasattr(self, 'collected_data'):
            return
        
        # Calculate statistics
        total_spikes = sum(d['total_spikes'] for d in self.collected_data)
        avg_firing_rate = sum(d['firing_rate'] for d in self.collected_data) / len(self.collected_data)
        
        # Generate analysis report
        analysis = {
            'total_spikes': total_spikes,
            'average_firing_rate': avg_firing_rate,
            'duration': context.time,
            'efficiency': total_spikes / context.time if context.time > 0 else 0
        }
        
        # Save analysis
        with open('analysis_report.json', 'w') as f:
            json.dump(analysis, f, indent=2)
        
        print(f"Analysis complete: {analysis}")
    
    def runScriptedSimulation(self, script_path):
        """Run simulation with external script"""
        # Load configuration script
        self.script_engine.loadScript(script_path + '_config.py')
        
        # Load behavior script
        self.script_engine.loadScript(script_path + '_behavior.py')
        
        # Run simulation
        for step in range(1000):
            # Execute hooks
            self.hook_manager.executeHooks(EventType.SimulationStep, {
                'step': step,
                'brain': self.brain,
                'config': self.config,
                'time': step * 0.001
            })
            
            # Execute simulation step
            self.brain.step(step, step * 0.001)
        
        # Execute final hooks
        self.hook_manager.executeHooks(EventType.SimulationEnd, {
            'brain': self.brain,
            'config': self.config,
            'time': 1.0
        })
        
        return self.collected_data

# Usage example
if __name__ == "__main__":
    integration = NLMIntegration()
    results = integration.runScriptedSimulation("experiments/my_experiment")
    print("Simulation completed successfully")
```

### 2. C++ Integration

```cpp
// cpp_integration.cpp
#include <nlm/scripting/ScriptEngine.h>
#include <nlm/hooks/HookManager.h>
#include <nlohmann/json.hpp>

class CustomNLMSystem {
public:
    CustomNLMSystem() {
        // Initialize brain with custom configuration
        auto config = nlm::Config::createResearchConfig();
        config.set("neuron_count", 2000);
        config.set("enable_plasticity", true);
        
        brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Setup script engine
        scriptEngine = std::make_unique<nlm::scripting::ScriptEngine>();
        
        // Setup hook manager
        hookManager = std::make_unique<nlm::hooks::HookManager>();
        
        registerCustomHooks();
    }
    
    void registerCustomHooks() {
        // Register performance monitoring hook
        hookManager->registerHook(std::make_shared<nlm::hooks::Hook>([
            eventType = nlm::hooks::EventType::SimulationStep,
            priority = nlm::hooks::HookPriority::High
        ](const nlm::hooks::HookContext& context) {
            return this->monitorPerformance(context);
        }));
        
        // Register data collection hook
        hookManager->registerHook(std::make_shared<nlm::hooks::Hook>([
            eventType = nlm::hooks::EventType::NeuronSpike,
            priority = nlm::hooks::HookPriority::Normal
        ](const nlm::hooks::HookContext& context) {
            return this->collectNeuralData(context);
        }));
    }
    
    bool monitorPerformance(const nlm::hooks::HookContext& context) {
        // Custom performance monitoring
        double cpuUsage = getSystemCPUUsage();
        double memoryUsage = getSystemMemoryUsage();
        
        // Log performance data
        std::cout << "Step " << context.step 
                  << ": CPU=" << cpuUsage << "%, "
                  << "Memory=" << memoryUsage << "MB" << std::endl;
        
        // Store in performance log
        performanceLog.push_back({
            'step': context.step,
            'cpu_usage': cpuUsage,
            'memory_usage': memoryUsage,
            'simulation_time': context.time
        });
        
        return true;
    }
    
    bool collectNeuralData(const nlm::hooks::HookContext& context) {
        // Custom neural data collection
        const auto& spikeData = context.eventData.at("spike_data");
        
        // Analyze spike patterns
        analyzeSpikePatterns(spikeData);
        
        // Update statistics
        updateLearningStatistics(context);
        
        return true;
    }
    
    void analyzeSpikePatterns(const nlohmann::json& spikeData) {
        // Custom spike pattern analysis
        std::unordered_map<std::string, double> patternStats;
        
        for (const auto& [neuronId, spikes] : spikeData.items()) {
            std::vector<double> spikeTimes = spikes.get<std::vector<double>>();
            
            if (spikeTimes.size() > 1) {
                double avgISI = calculateAverageISI(spikeTimes);
                double synchrony = calculateSynchrony(spikeTimes);
                
                patternStats[neuronId] = {
                    "avg_isi": avgISI,
                    "synchrony": synchrony,
                    "firing_rate": spikeTimes.size() / 1.0
                };
            }
        }
        
        // Store pattern analysis
        spikePatternAnalysis.push_back(patternStats);
    }
    
    void updateLearningStatistics(const nlm::hooks::HookContext& context) {
        // Update learning-related statistics
        double learningProgress = calculateLearningProgress(context);
        
        learningStats.push_back({
            'step': context.step,
            'learning_progress': learningProgress,
            'plasticity_effect': context.eventData.at("plasticity_effect"),
            'reward_signal': context.eventData.at("reward_signal")
        });
    }
    
    void saveAnalysisResults() {
        // Save all collected analysis data
        nlohmann::json analysis;
        
        analysis["performance_log"] = performanceLog;
        analysis["spike_pattern_analysis"] = spikePatternAnalysis;
        analysis["learning_stats"] = learningStats;
        
        // Save to file
        std::ofstream file("custom_analysis.json");
        file << analysis.dump(2);
        
        std::cout << "Analysis results saved to custom_analysis.json" << std::endl;
    }
    
    void runScriptedSimulation(const std::string& scriptPath) {
        // Load custom scripts
        scriptEngine->loadScript(scriptPath + "/behavior.js");
        scriptEngine->loadScript(scriptPath + "/config.js");
        
        // Run simulation with custom hooks
        for (int step = 0; step < 1000; step++) {
            // Execute performance monitoring
            hookManager->executeHooks(nlm::hooks::EventType::SimulationStep, {
                eventType: nlm::hooks::EventType::SimulationStep,
                brain: brain,
                config: brain->getConfig(),
                step: step,
                time: step * 0.001,
                eventData: {},
                logOutput: {}
            });
            
            // Execute simulation step
            brain->step(step, step * 0.001);
            
            // Check for external triggers
            scriptEngine->executeHooks(nlm::scripting::EventType::SimulationStep, {
                eventType: nlm::scripting::EventType::SimulationStep,
                brain: brain,
                config: brain->getConfig(),
                step: step,
                time: step * 0.001,
                eventData: {},
                logOutput: {}
            });
        }
        
        // Save analysis results
        saveAnalysisResults();
    }
    
private:
    std::shared_ptr<nlm::Brain> brain;
    std::unique_ptr<nlm::scripting::ScriptEngine> scriptEngine;
    std::unique_ptr<nlm::hooks::HookManager> hookManager;
    
    // Analysis data storage
    std::vector<nlohmann::json> performanceLog;
    std::vector<nlohmann::json> spikePatternAnalysis;
    std::vector<nlohmann::json> learningStats;
};

int main() {
    // Create custom NLM system
    auto customSystem = std::make_unique<CustomNLMSystem>();
    
    // Run scripted simulation
    customSystem->runScriptedSimulation("experiments/custom_experiment");
    
    std::cout << "Custom simulation completed successfully" << std::endl;
    return 0;
}
```

## Script File Formats

### JavaScript (.js)
```javascript
// JavaScript script example
function setup(context) {
    context.brain.setSensoryInput("vision", [0.8, 0.6, 0.9]);
    context.config.set("plasticity_enabled", true);
}

function onStep(context) {
    // Custom behavior per step
    return {continue: true};
}

function onEnd(context) {
    // Cleanup and analysis
    console.log("Simulation completed");
}
```

### Python (.py)
```python
# Python script example
def setup(context):
    context.brain.set_sensory_input("vision", [0.8, 0.6, 0.9])
    context.config.set("plasticity_enabled", True)

def on_step(context):
    # Custom behavior per step
    return {"continue": True}

def on_end(context):
    # Cleanup and analysis
    print("Simulation completed")
```

### Lua (.lua)
```lua
-- Lua script example
function setup(context)
    context.brain:set_sensory_input("vision", {0.8, 0.6, 0.9})
    context.config:set("plasticity_enabled", true)
end

function on_step(context)
    -- Custom behavior per step
    return {continue = true}
end

function on_end(context)
    -- Cleanup and analysis
    print("Simulation completed")
end
```

## Safety and Security

### Script Sandboxing

Scripts are executed in a controlled sandbox environment with:

1. **Restricted API Access**: Only safe built-in functions and custom functions
2. **Memory Limits**: Maximum memory allocation per script
3. **Time Limits**: Maximum execution time per script
4. **Input Validation**: Validate all script inputs
5. **Output Validation**: Sanitize script outputs

### Security Measures

1. **Script Validation**: Validate script syntax before execution
2. **Permission System**: Control script permissions and access rights
3. **Isolation**: Execute scripts in isolated processes
4. **Logging**: Log all script execution activities
5. **Error Handling**: Graceful error handling and recovery

## Performance Considerations

### Script Optimization

1. **Cache Scripts**: Load and cache scripts for repeated execution
2. **Lazy Loading**: Load scripts only when needed
3. **Script Compilation**: Pre-compile scripts for better performance
4. **Parallel Execution**: Execute independent scripts in parallel

### Hook Optimization

1. **Hook Prioritization**: Prioritize critical hooks for fast execution
2. **Hook Batching**: Batch similar hooks for efficiency
3. **Conditional Execution**: Only execute hooks when needed
4. **Hook Removal**: Remove unnecessary hooks dynamically

## Migration Guide

### From Basic to Advanced Usage

1. **Enable Scripting**: Configure script engine in your application
2. **Define Custom Hooks**: Register custom event hooks for your use case
3. **Create Scripts**: Develop custom scripts for your specific requirements
4. **Test and Optimize**: Test scripts with hooks and optimize performance
5. **Deploy**: Deploy in production environment with proper monitoring

### Script Conversion Checklist

- [ ] Convert configuration to JSON format
- [ ] Define custom hooks for data collection
- [ ] Create analysis scripts for results processing
- [ ] Implement error handling and recovery
- [ ] Add performance monitoring
- [ ] Test with small-scale simulations
- [ ] Validate output formats
- [ ] Optimize for your specific use case

## Conclusion

The scripting and hooks system provides a powerful framework for customizing NLM Phase 6 behavior through external scripts and event-driven hooks. This system enables:

1. **Flexible Customization**: Modify brain behavior through scripts
2. **Advanced Analysis**: Collect and analyze simulation data
3. **Performance Optimization**: Monitor and optimize simulation performance
4. **Research Automation**: Automate complex experimental protocols
5. **Integration**: Integrate NLM with external systems and data sources

This system makes NLM Phase 6 suitable for advanced research, custom applications, and large-scale simulations while maintaining safety and security through proper sandboxing and validation.

## References

1. Original NLM Documentation: See `docs/` directory for comprehensive documentation
2. Script Engine API: `nlm/scripting/` directory for script engine documentation
3. Hook System API: `nlm/hooks/` directory for hook system documentation
4. Script Examples: See examples in `examples/scripting/` for usage examples
5. Integration Guides: See integration examples for Python and C++ integration

## Quick Start Guide

For rapid scripting with NLM Phase 6:

1. **Create Basic Script**:
   ```bash
   cat > my_script.js << 'EOF'
   function setup(context) {
       context.brain.setSensoryInput("vision", [0.8, 0.6, 0.9]);
   }
   function onStep(context) {
       return {continue: true};
   }
   EOF
   ```

2. **Load and Run Script**:
   ```cpp
   scriptEngine->loadScript("my_script.js");
   customSystem->runScriptedSimulation("experiments/my_experiment");
   ```

3. **Add Custom Hooks**:
   ```cpp
   hookManager->registerHook(std::make_shared<CustomHook>());
   ```

4. **Run with Monitoring**:
   ```bash
   ./nlm_phase6_demo --neurons 2000 --steps 500 --output results.json
   ```

This guide provides comprehensive coverage of the scripting and hooks system for NLM Phase 6, including implementation details, examples, integration guides, and performance considerations for advanced users and developers.

---

*This document is part of the NLM Phase 6 Advanced Features Documentation Suite.*
*For more information, see related documentation in the `docs/` directory.*

---
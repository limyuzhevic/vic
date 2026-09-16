#include "AdvancedTools.hpp"
#include "brain_analysis/BrainAnalysisTools.hpp"
#include "brain_management/BrainManagement.hpp"
#include "configuration_management/AdvancedConfigManager.hpp"
#include "performance/AdvancedPerformanceProfiler.hpp"
#include "simulation/AdvancedSimulationController.hpp"
#include "memory/AdvancedMemoryManager.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <mutex>

namespace nlm {

struct AdvancedToolkit::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Environment> environment;
    std::shared_ptr<Logger> logger;
    
    std::unique_ptr<BrainAnalysisTools> brainAnalyzer;
    std::unique_ptr<BrainController> brainController;
    std::unique_ptr<AdvancedConfigManager> configManager;
    std::unique_ptr<AdvancedPerformanceProfiler> performanceProfiler;
    std::unique_ptr<AdvancedSimulationController> simulationController;
    std::unique_ptr<AdvancedMemoryManager> memoryManager;
    
    bool initialized;
    std::chrono::system_clock::time_point creationTime;
    std::map<std::string, bool> toolStatus;
    
    Impl(std::shared_ptr<Brain> br, std::shared_ptr<Environment> env, std::shared_ptr<Logger> log)
        : brain(br), environment(env), logger(log), initialized(false),
          creationTime(std::chrono::system_clock::now()) {
    }
};

AdvancedToolkit::AdvancedToolkit(std::shared_ptr<Brain> brain,
                                 std::shared_ptr<Environment> environment,
                                 std::shared_ptr<Logger> logger)
    : pImpl(std::make_unique<Impl>(brain, environment, logger)) {
}

AdvancedToolkit::~AdvancedToolkit() = default;

// Factory function implementations

BrainStateAnalyzer* createBrainAnalysisTools(std::shared_ptr<Logger> logger) {
    return new BrainStateAnalyzer(logger);
}

BrainController* createBrainController(std::shared_ptr<Brain> brain,
                                      std::shared_ptr<Logger> logger) {
    return new BrainController(brain, logger);
}

AdvancedConfigManager* createConfigManager(std::shared_ptr<Logger> logger) {
    return new AdvancedConfigManager(logger);
}

AdvancedPerformanceProfiler* createPerformanceProfiler(std::shared_ptr<Logger> logger) {
    return new AdvancedPerformanceProfiler(logger);
}

AdvancedSimulationController* createSimulationController(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Environment> environment,
    std::shared_ptr<Logger> logger) {
    return new AdvancedSimulationController(brain, environment, logger);
}

AdvancedMemoryManager* createMemoryManager(std::shared_ptr<Logger> logger) {
    return new AdvancedMemoryManager(logger);
}

AdvancedToolkit* createAdvancedToolkit(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Environment> environment,
    std::shared_ptr<Logger> logger) {
    
    AdvancedToolkit* toolkit = new AdvancedToolkit(brain, environment, logger);
    toolkit->initialize();
    return toolkit;
}

namespace {
    std::shared_ptr<AdvancedToolkit> g_currentToolkit;
    std::mutex g_toolkitMutex;
}

AdvancedToolkit* getAdvancedTools() {
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    return g_currentToolkit.get();
}

void setAdvancedTools(std::shared_ptr<AdvancedToolkit> toolkit) {
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    g_currentToolkit = toolkit;
}

std::shared_ptr<AdvancedToolkit> initializeAdvancedTools(std::shared_ptr<Logger> logger) {
    if (!logger) {
        logger = Logger::getGlobal();
    }
    
    std::shared_ptr<AdvancedToolkit> toolkit = std::make_shared<AdvancedToolkit>(nullptr, nullptr, logger);
    
    if (toolkit) {
        toolkit->initialize();
        
        if (logger) {
            logger->info("Advanced NLM Tools initialized successfully");
        }
    }
    
    return toolkit;
}

void shutdownAdvancedTools() {
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    g_currentToolkit.reset();
    
    if (Logger::getGlobal()) {
        Logger::getGlobal()->info("Advanced NLM Tools shutdown completed");
    }
}

std::string getAdvancedToolsStatistics() {
    std::ostringstream oss;
    oss << "=== ADVANCED NLM TOOLS STATISTICS ===\n";
    oss << "Generated: " << std::ctime(&std::time(nullptr));
    oss << "\n\n";
    
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    
    if (g_currentToolkit) {
        oss << "Current toolkit: " << typeid(*g_currentToolkit).name() << "\n";
        
        if (g_currentToolkit->pImpl->logger) {
            oss << "Brain present: " << (g_currentToolkit->pImpl->brain ? "yes" : "no") << "\n";
            oss << "Environment present: " << (g_currentToolkit->pImpl->environment ? "yes" : "no") << "\n";
            oss << "Logger present: yes\n";
        }
        
        oss << "Initialized: " << (g_currentToolkit->isInitialized() ? "yes" : "no") << "\n";
        
        auto age = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - g_currentToolkit->pImpl->creationTime);
        oss << "Age: " << age.count() << " seconds\n";
    } else {
        oss << "No toolkit currently loaded\n";
    }
    
    oss << "\n=== END STATISTICS ===\n";
    return oss.str();
}

bool exportAdvancedToolsConfig(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    
    if (g_currentToolkit) {
        // Export basic toolkit state
        file << "Advanced NLM Tools Configuration\n";
        file << "Generated: " << std::ctime(&std::time(nullptr));
        file << "\n\n";
        file << "Tool Count: " << g_currentToolkit->pImpl->toolStatus.size() << "\n";
        
        for (const auto& pair : g_currentToolkit->pImpl->toolStatus) {
            file << pair.first << ": " << (pair.second ? "active" : "inactive") << "\n";
        }
    }
    
    file.close();
    return true;
}

bool importAdvancedToolsConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Placeholder for config import
    file.close();
    return true;
}

bool areAdvancedToolsInitialized() {
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    return g_currentToolkit && g_currentToolkit->isInitialized();
}

size_t getAdvancedToolsCount() {
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    return g_currentToolkit ? g_currentToolkit->pImpl->toolStatus.size() : 0;
}

std::vector<std::string> getAdvancedToolsList() {
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    std::vector<std::string> tools;
    
    if (g_currentToolkit) {
        for (const auto& pair : g_currentToolkit->pImpl->toolStatus) {
            tools.push_back(pair.first);
        }
    }
    
    return tools;
}

bool isAdvancedToolLoaded(const std::string& toolName) {
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    
    if (!g_currentToolkit) {
        return false;
    }
    
    return g_currentToolkit->pImpl->toolStatus.count(toolName) > 0;
}

void enableAdvancedToolsLogging(bool enable) {
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    
    if (g_currentToolkit && g_currentToolkit->pImpl->logger) {
        g_currentToolkit->pImpl->logger->setLevel(enable ? LogLevel::Debug : LogLevel::Error);
    }
}

std::shared_ptr<Logger> getAdvancedToolsLogger() {
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    
    if (g_currentToolkit) {
        return g_currentToolkit->pImpl->logger;
    }
    
    return Logger::getGlobal();
}

void setAdvancedToolsLogger(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(g_toolkitMutex);
    
    if (g_currentToolkit) {
        g_currentToolkit->pImpl->logger = logger;
    }
}

std::shared_ptr<AdvancedToolkit> createSimpleAdvancedTools(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Environment> environment) {
    
    auto logger = Logger::getGlobal();
    
    std::shared_ptr<AdvancedToolkit> toolkit = std::make_shared<AdvancedToolkit>(brain, environment, logger);
    
    if (toolkit) {
        toolkit->initialize();
    }
    
    return toolkit;
}

std::shared_ptr<AdvancedToolkit> createResearchAdvancedTools(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Environment> environment,
    std::shared_ptr<Logger> logger) {
    
    if (!logger) {
        logger = Logger::getGlobal();
    }
    
    std::shared_ptr<AdvancedToolkit> toolkit = std::make_shared<AdvancedToolkit>(brain, environment, logger);
    
    if (toolkit) {
        toolkit->initialize();
        
        // Enable comprehensive data collection for research
        if (auto* profiler = toolkit->getPerformanceProfiler()) {
            profiler->enableMemoryProfiling(true);
            profiler->enableSpikeProfiling(true);
            profiler->enablePlasticityProfiling(true);
        }
        
        if (auto* configManager = toolkit->getConfigManager()) {
            configManager->enableHistory(true);
        }
        
        if (auto* memoryManager = toolkit->getMemoryManager()) {
            memoryManager->enableReplay(true);
            memoryManager->setConsolidationThreshold(0.8f);
        }
        
        logger->info("Created research-optimized AdvancedToolkit");
    }
    
    return toolkit;
}

std::shared_ptr<AdvancedToolkit> createProductionAdvancedTools(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Environment> environment,
    std::shared_ptr<Logger> logger) {
    
    if (!logger) {
        logger = Logger::getGlobal();
    }
    
    std::shared_ptr<AdvancedToolkit> toolkit = std::make_shared<AdvancedToolkit>(brain, environment, logger);
    
    if (toolkit) {
        toolkit->initialize();
        
        // Configure for production use
        if (auto* profiler = toolkit->getPerformanceProfiler()) {
            profiler->enableRealTimeMonitoring(true);
            profiler->enableMemoryProfiling(true);
        }
        
        if (auto* simulationController = toolkit->getSimulationController()) {
            simulationController->enableMonitoring(true);
        }
        
        if (auto* configManager = toolkit->getConfigManager()) {
            configManager->enableMonitoring(true);
        }
        
        logger->info("Created production-optimized AdvancedToolkit");
    }
    
    return toolkit;
}

std::string getAdvancedToolsHelp(const std::string& topic) {
    if (topic.empty()) {
        return "=== ADVANCED NLM TOOLS HELP ===\n\n"
               "The Advanced NLM Tools module provides comprehensive utilities for:\n"
               "- Brain analysis and metrics collection\n"
               "- Brain manipulation and debugging\n"
               "- Configuration management with validation\n"
               "- Performance profiling and optimization\n"
               "- Simulation control and parameter management\n"
               "- Memory management for episodic storage and replay\n\n"
               "Use the factory functions to create toolkits:\n"
               "- createSimpleAdvancedTools() - Basic toolkit\n"
               "- createResearchAdvancedTools() - Research-optimized toolkit\n"
               "- createProductionAdvancedTools() - Production-optimized toolkit\n";
    }
    
    std::map<std::string, std::string> helpTopics {
        {"brain_analysis", "Brain analysis tools provide detailed metrics and statistics about brain state, performance, and development."},
        {"brain_management", "Brain management tools provide precise control over brain manipulation, testing, and debugging."},
        {"config_management", "Configuration management tools provide advanced config with validation, schemas, and optimization."},
        {"performance_profiling", "Performance profiling tools provide comprehensive profiling and optimization capabilities."},
        {"simulation_control", "Simulation control tools provide advanced simulation parameter management and control."},
        {"memory_management", "Memory management tools provide episodic memory, replay, and consolidation capabilities."}
    };
    
    auto it = helpTopics.find(topic);
    if (it != helpTopics.end()) {
        return "=== ADVANCED NLM TOOLS HELP: " + topic + " ===\n\n" + it->second + "\n\n";
    }
    
    return "Unknown topic: " + topic + "\n";
}

void printAdvancedToolsInfo(bool includeDetails) {
    std::string help = getAdvancedToolsHelp();
    std::cout << help << std::endl;
    
    if (includeDetails) {
        std::cout << "\n=== DETAILED INFORMATION ===\n";
        std::cout << "Current tools loaded: " << getAdvancedToolsCount() << "\n";
        std::cout << "Tools: ";
        auto tools = getAdvancedToolsList();
        for (size_t i = 0; i < tools.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << tools[i];
        }
        std::cout << "\n\n";
    }
}

std::string getAdvancedToolsVersion() {
    return "Advanced NLM Tools v1.0.0 - Neural Learning Machine Framework Extension";
}

bool checkAdvancedToolsUpdates() {
    // Placeholder for update checking
    return false;
}

} // namespace nlm

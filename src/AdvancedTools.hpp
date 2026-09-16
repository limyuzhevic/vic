#pragma once

/**
 * Advanced NLM Commands and Utilities
 * 
 * Comprehensive set of advanced tools for the Neural Learning Machine (NLM) framework,
 * designed to empower expert users with deep control, analysis, and optimization capabilities.
 * 
 * This module provides:
 * 1. Advanced Brain Analysis Tools - Deep inspection and metrics collection
 * 2. Brain Management Utilities - Precise manipulation and debugging
 * 3. Configuration Management Tools - Advanced config with validation and optimization
 * 4. Performance Optimization Tools - Comprehensive profiling and optimization
 * 5. Simulation Control Tools - Advanced simulation parameter management
 * 6. Memory Management Utilities - Episodic memory, replay, and consolidation
 * 
 * These tools are designed to work seamlessly with the existing NLM framework while
 * providing powerful capabilities for advanced users, researchers, and developers.
 */

#include "brain_analysis/BrainAnalysisTools.hpp"
#include "brain_management/BrainManagement.hpp"
#include "configuration_management/AdvancedConfigManager.hpp"
#include "performance/AdvancedPerformanceProfiler.hpp"
#include "simulation/AdvancedSimulationController.hpp"
#include "memory/AdvancedMemoryManager.hpp"

// Convenience factory functions for easy access to advanced tools

namespace nlm {

// Global factory functions for creating advanced tools with proper dependencies

/**
 * Create a comprehensive Advanced Brain Analysis Tools instance
 * 
 * @param logger Logger instance for tool operations
 * @return Shared pointer to BrainStateAnalyzer
 */nlm::BrainStateAnalyzer* createBrainAnalysisTools(std::shared_ptr<Logger> logger);

/**
 * Create an Advanced Brain Controller instance
 * 
 * @param brain Shared pointer to Brain instance
 * @param logger Logger instance for operations
 * @return Shared pointer to BrainController
 */nlm::BrainController* createBrainController(std::shared_ptr<Brain> brain,
                                          std::shared_ptr<Logger> logger);

/**
 * Create an Advanced Configuration Manager instance
 * 
 * @param logger Logger instance for operations
 * @return Shared pointer to AdvancedConfigManager
 */nlm::AdvancedConfigManager* createConfigManager(std::shared_ptr<Logger> logger);

/**
 * Create an Advanced Performance Profiler instance
 * 
 * @param logger Logger instance for operations
 * @return Shared pointer to AdvancedPerformanceProfiler
 */nlm::AdvancedPerformanceProfiler* createPerformanceProfiler(std::shared_ptr<Logger> logger);

/**
 * Create an Advanced Simulation Controller instance
 * 
 * @param brain Shared pointer to Brain instance
 * @param environment Shared pointer to Environment instance
 * @param logger Logger instance for operations
 * @return Shared pointer to AdvancedSimulationController
 */nlm::AdvancedSimulationController* createSimulationController(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<Environment> environment,
    std::shared_ptr<Logger> logger);

/**
 * Create an Advanced Memory Manager instance
 * 
 * @param logger Logger instance for operations
 * @return Shared pointer to AdvancedMemoryManager
 */nlm::AdvancedMemoryManager* createMemoryManager(std::shared_ptr<Logger> logger);

/**
 * Create a complete toolkit with all advanced utilities
 * 
 * This function creates and initializes all advanced tools with proper
 * dependency injection, returning a comprehensive toolkit.
 * 
 * @param brain Optional shared pointer to Brain instance
 * @param environment Optional shared pointer to Environment instance
 * @param logger Logger instance for all tool operations
 * @return Shared pointer to AdvancedToolkit containing all tools
 */nlm::AdvancedToolkit* createAdvancedToolkit(
    std::shared_ptr<Brain> brain = nullptr,
    std::shared_ptr<Environment> environment = nullptr,
    std::shared_ptr<Logger> logger = nullptr);

/**
 * Get the current set of advanced tools (singleton pattern)
 * 
 * @return Shared pointer to the current AdvancedToolkit
 */nlm::AdvancedToolkit* getAdvancedTools();

/**
 * Set the current advanced tools (singleton pattern)
 * 
 * @param toolkit Shared pointer to the toolkit to set as current
 */void setAdvancedTools(std::shared_ptr<AdvancedToolkit> toolkit);

/**
 * Initialize all advanced tools with default configuration
 * 
 * This function creates a complete set of advanced tools with sensible defaults
 * and initializes them for immediate use.
 * 
 * @param logger Logger instance for all tool operations
 * @return Shared pointer to the initialized AdvancedToolkit
 */std::shared_ptr<AdvancedToolkit> initializeAdvancedTools(
    std::shared_ptr<Logger> logger = Logger::getGlobal());

/**
 * Shutdown and cleanup all advanced tools
 * 
 * This function properly shuts down all advanced tools and cleans up resources.
 */void shutdownAdvancedTools();

/**
 * Get system-wide advanced tool statistics
 * 
 * @return String containing comprehensive statistics about all loaded tools
 */std::string getAdvancedToolsStatistics();

/**
 * Export advanced tool configuration to file
 * 
 * @param filepath Path to save the configuration
 * @return true if export succeeded, false otherwise
 */bool exportAdvancedToolsConfig(const std::string& filepath);

/**
 * Import advanced tool configuration from file
 * 
 * @param filepath Path to the configuration file
 * @return true if import succeeded, false otherwise
 */bool importAdvancedToolsConfig(const std::string& filepath);

/**
 * Check if all advanced tools are properly initialized
 * 
 * @return true if all tools are initialized and ready, false otherwise
 */bool areAdvancedToolsInitialized();

/**
 * Get the number of currently loaded advanced tools
 * 
 * @return Number of initialized advanced tools
 */size_t getAdvancedToolsCount();

/**
 * Get list of loaded advanced tool names
 * 
 * @return Vector of loaded tool names
 */std::vector<std::string> getAdvancedToolsList();

/**
 * Check if a specific advanced tool is loaded
 * 
 * @param toolName Name of the tool to check
 * @return true if the tool is loaded, false otherwise
 */bool isAdvancedToolLoaded(const std::string& toolName);

/**
 * Enable or disable logging for all advanced tools
 * 
 * @param enable true to enable logging, false to disable
 */void enableAdvancedToolsLogging(bool enable);

/**
 * Get the main logger used by advanced tools
 * 
 * @return Shared pointer to the main logger
 */std::shared_ptr<Logger> getAdvancedToolsLogger();

/**
 * Set the main logger used by advanced tools
 * 
 * @param logger Shared pointer to the logger to set
 */void setAdvancedToolsLogger(std::shared_ptr<Logger> logger);

/**
 * Create a simple advanced tools instance for basic usage
 * 
 * This convenience function creates a minimal set of essential advanced tools
 * suitable for users who want some advanced capabilities without full complexity.
 * 
 * @param brain Optional brain instance
 * @param environment Optional environment instance
 * @return Shared pointer to simple AdvancedToolkit
 */std::shared_ptr<AdvancedToolkit> createSimpleAdvancedTools(
    std::shared_ptr<Brain> brain = nullptr,
    std::shared_ptr<Environment> environment = nullptr);

/**
 * Create an advanced tools instance optimized for research
 * 
 * This function creates advanced tools optimized for research scenarios,
 * with emphasis on data collection, analysis, and reproducibility.
 * 
 * @param brain Optional brain instance
 * @param environment Optional environment instance
 * @param logger Optional logger instance
 * @return Shared pointer to research-optimized AdvancedToolkit
 */std::shared_ptr<AdvancedToolkit> createResearchAdvancedTools(
    std::shared_ptr<Brain> brain = nullptr,
    std::shared_ptr<Environment> environment = nullptr,
    std::shared_ptr<Logger> logger = nullptr);

/**
 * Create an advanced tools instance optimized for production
 * 
 * This function creates advanced tools optimized for production use,
 * with emphasis on stability, performance, and monitoring.
 * 
 * @param brain Optional brain instance
 * @param environment Optional environment instance
 * @param logger Optional logger instance
 * @return Shared pointer to production-optimized AdvancedToolkit
 */std::shared_ptr<AdvancedToolkit> createProductionAdvancedTools(
    std::shared_ptr<Brain> brain = nullptr,
    std::shared_ptr<Environment> environment = nullptr,
    std::shared_ptr<Logger> logger = nullptr);

/**
 * Get advanced tools help and documentation
 * 
 * @param topic Optional topic for specific documentation (empty for general help)
 * @return String containing help documentation
 */std::string getAdvancedToolsHelp(const std::string& topic = "");

/**
 * Print advanced tools information to console
 * 
 * @param includeDetails true to include detailed information, false for summary
 */void printAdvancedToolsInfo(bool includeDetails = false);

/**
 * Get version information for advanced tools
 * 
 * @return String containing version information
 */std::string getAdvancedToolsVersion();

/**
 * Check for updates to advanced tools
 * 
 * @return true if updates are available, false otherwise
 */bool checkAdvancedToolsUpdates();

} // namespace nlm

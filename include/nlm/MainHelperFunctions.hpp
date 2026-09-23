// NLM Main Application Helper Functions
// Phase 2: Real Neural Computation
//
// This file provides helper function declarations for main.cpp refactoring
// to improve code organization and API design

#pragma once

#include <memory>
#include <string>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

namespace nlm {

/**
 * @brief Print application banner and welcome message
 * 
 * Displays the NLM (Neural Learning Machine) Phase 2 banner with version information
 * and a brief description of the implementation.
 */
void printBanner();

/**
 * @brief Initialize application logging system
 * 
 * Sets up the global logger with console output at Info level
 * and configures the logging system for the application.
 */
void initializeLogging();

/**
 * @brief Load and configure application configuration
 * 
 * Handles command-line argument parsing, configuration file loading,
 * and application-specific configuration defaults. Performs validation
 * and returns a fully configured configuration object.
 * 
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Fully configured configuration object
 * 
 * @throws std::runtime_error if configuration loading fails
 */
std::shared_ptr<Config> loadConfiguration(int argc, char** argv);

/**
 * @brief Initialize and configure the brain for simulation
 * 
 * Creates a Brain instance with the provided configuration, performs
 * all necessary initialization steps including region setup,
 * population creation, connectivity establishment, and system
 * configuration. Handles errors and provides detailed status reporting.
 * 
 * @param config Configuration for brain initialization
 * @return Initialized brain instance
 * 
 * @throws std::runtime_error if brain initialization fails
 */
std::shared_ptr<Brain> setupBrain(const std::shared_ptr<Config>& config);

/**
 * @brief Run demonstration tests on the brain
 * 
 * Executes the three main demonstration tests:
 * 1. Basic connectivity test (Test 1)
 * 2. Plasticity learning experiment (Test 2)
 * 3. STDP verification (Test 3)
 * 
 * Each test resets and reinitializes the brain to ensure
 * independent test execution and clean state between tests.
 * 
 * @param brain Initialized brain instance to test
 */
void runTests(const std::shared_ptr<Brain>& brain);

/**
 * @brief Display final brain statistics and results
 * 
 * Shows comprehensive statistics including brain status,
 * learning experiment results, and performance metrics.
 * Provides a summary of Phase 2 objectives completion.
 * 
 * @param brain Brain instance to display results for
 */
void displayResults(const std::shared_ptr<Brain>& brain);

/**
 * @brief Handle graceful application shutdown
 * 
 * Performs cleanup operations and displays final status
 * before application exit. Ensures proper resource cleanup
 * and provides user feedback.
 * 
 * @param success Exit status (true for success, false for failure)
 */
void shutdownApplication(bool success);

} // namespace nlm

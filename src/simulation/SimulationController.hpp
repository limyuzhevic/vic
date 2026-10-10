#pragma once

/**
 * Advanced Simulation Controls for NLM
 * 
 * Comprehensive simulation control system for NLM (Neural Learning Machine)
 * 
 * Features:
 * - Pause/Resume simulation
 * - Step-by-step execution
 * - Fast-forward and rewind
 * - Time scaling and speed control
 * - Conditional execution
 * - Event-driven simulation
 * - State management
 * - Checkpoint management
 * - Branch simulation
 * - Parallel simulation
 * - Analysis and visualization
 * 
 * Usage:
 * ```cpp
 * #include <nlm/simulation/SimulationController.hpp>
 * 
 * int main() {
 *     auto controller = nlm::createSimulationController();
 *     controller->loadConfiguration("simulation_config.json");
 *     controller->initialize();
 *     
 *     while (controller->isRunning()) {
 *         if (controller->shouldPause()) {
 *             controller->pause();
 *         }
 *         
 *         if (controller->shouldStep()) {
 *             controller->step();
 *         } else {
 *             controller->update();
 *         }
 *     }
 * }
 * ```
 */

#include <nlm/simulation/SimulationController.hpp>
#include <nlm/simulation/SimulationState.hpp>
#include <nlm/simulation/SimulationParameters.hpp>
#include <nlm/simulation/SimulationManager.hpp>
#include <nlm/simulation/SimulationRegistry.hpp>
#include <nlm/simulation/BranchSimulation.hpp>
#include <nlm/simulation/ParallelSimulation.hpp>
#include <nlm/analysis/PerformanceAnalyzer.hpp>
#include <nlm/analysis/StateAnalyzer.hpp>
#include <nlm/analysis/BehavioralAnalyzer.hpp>
#include <nlm/visualization/RealTimeVisualizer.hpp>
#include <nlm/visualization/AnimationController.hpp>
#include <nlm/visualization/DataVisualizer.hpp>
#include <nlm/visualization/ComparisonVisualizer.hpp>

// Include implementation files
#include <nlm/simulation/SimulationController.cpp>
#include <nlm/simulation/SimulationState.cpp>
#include <nlm/simulation/SimulationParameters.cpp>
#include <nlm/simulation/SimulationManager.cpp>
#include <nlm/simulation/SimulationRegistry.cpp>
#include <nlm/simulation/BranchSimulation.cpp>
#include <nlm/simulation/ParallelSimulation.cpp>
#include <nlm/analysis/PerformanceAnalyzer.cpp>
#include <nlm/analysis/StateAnalyzer.cpp>
#include <nlm/analysis/BehavioralAnalyzer.cpp>
#include <nlm/visualization/RealTimeVisualizer.cpp>
#include <nlm/visualization/AnimationController.cpp>
#include <nlm/visualization/DataVisualizer.cpp>
#include <nlm/visualization/ComparisonVisualizer.cpp>

// Factory functions for easy access
namespace nlm {

/**
 * Create a simulation controller with default settings
 */
std::unique_ptr<SimulationController> createSimulationController() {
    return std::make_unique<SimulationController>();
}

/**
 * Create a simulation controller with custom parameters
 */
std::unique_ptr<SimulationController> createSimulationController(
    const SimulationParameters& params) {
    return std::make_unique<SimulationController>(params);
}

/**
 * Create a simulation manager
 */
std::unique_ptr<SimulationManager> createSimulationManager() {
    return std::make_unique<SimulationManager>();
}

/**
 * Create a simulation registry
 */
std::unique_ptr<SimulationRegistry> createSimulationRegistry() {
    return std::make_unique<SimulationRegistry>();
}

/**
 * Create a branch simulator
 */
std::unique_ptr<BranchSimulation> createBranchSimulator() {
    return std::make_unique<BranchSimulation>();
}

/**
 * Create a parallel simulator
 */
std::unique_ptr<ParallelSimulation> createParallelSimulator() {
    return std::make_unique<ParallelSimulation>();
}

/**
 * Create a performance analyzer
 */
std::unique_ptr<PerformanceAnalyzer> createPerformanceAnalyzer() {
    return std::make_unique<PerformanceAnalyzer>();
}

/**
 * Create a state analyzer
 */
std::unique_ptr<StateAnalyzer> createStateAnalyzer() {
    return std::make_unique<StateAnalyzer>();
}

/**
 * Create a behavioral analyzer
 */
std::unique_ptr<BehavioralAnalyzer> createBehavioralAnalyzer() {
    return std::make_unique<BehavioralAnalyzer>();
}

/**
 * Create a real-time visualizer
 */
std::unique_ptr<RealTimeVisualizer> createRealTimeVisualizer() {
    return std::make_unique<RealTimeVisualizer>();
}

/**
 * Create an animation controller
 */
std::unique_ptr<AnimationController> createAnimationController() {
    return std::make_unique<AnimationController>();
}

/**
 * Create a data visualizer
 */
std::unique_ptr<DataVisualizer> createDataVisualizer() {
    return std::make_unique<DataVisualizer>();
}

/**
 * Create a comparison visualizer
 */
std::unique_ptr<ComparisonVisualizer> createComparisonVisualizer() {
    return std::make_unique<ComparisonVisualizer>();
}

} // namespace nlm

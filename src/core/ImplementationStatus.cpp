// Implementation Status Tracker - Implementation
// Phase 6 Final Integration Status Report - Implementation

#include "ImplementationStatus.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <regex>
#include <iostream>

namespace nlm {

void ImplementationStatusTracker::addComponent(const std::string& id, const ComponentStatus& status) {
    component_statuses[id] = status;
    updateScore();
}

ComponentStatus ImplementationStatusTracker::getComponent(const std::string& id) const {
    auto it = component_statuses.find(id);
    if (it != component_statuses.end()) {
        return it->second;
    }
    return ComponentStatus{"Unknown", "", ImplementationStatus::NOT_IMPLEMENTED, 
                          "Not found", 0.0, "Not tracked", false, false, {}};
}

void ImplementationStatusTracker::updateScore() {
    achieved_points = 0;
    for (const auto& pair : component_statuses) {
        achieved_points += static_cast<int>(pair.second.completion_percentage);
    }
}

void ImplementationStatusTracker::analyzeCodebase(const std::string& source_dir) {
    // Analyze header files for TODO comments
    std::regex todo_regex(R"(TODO|PPLACEHOLDER)");
    std::regex phase_regex(R"(PHASE [0-9])");
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator(source_dir)) {
        if (entry.path().extension() == ".hpp" || entry.path().extension() == ".cpp") {
            std::ifstream file(entry.path());
            if (!file) continue;
            
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            // Count TODO comments
            std::smatch matches;
            std::string::const_iterator search_start(content.cbegin());
            int todo_count = 0;
            
            while (std::regex_search(search_start, content.cend(), matches, todo_regex)) {
                todo_count++;
                search_start = matches.suffix().first;
            }
            
            if (todo_count > 10) {
                // Update relevant component status
                if (entry.path().string().find("Brain.hpp") != std::string::npos) {
                    auto& brain_status = component_statuses["NEURAL_CORE"];
                    brain_status.implementation_notes += " (" + std::to_string(todo_count) + " TODOs)";
                }
            }
        }
    }
}

void ImplementationStatusTracker::analyzeTestCoverage(const std::string& test_dir) {
    for (const auto& entry : std::filesystem::directory_iterator(test_dir)) {
        if (entry.path().extension() == ".cpp") {
            std::ifstream file(entry.path());
            if (!file) continue;
            
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            // Check for TEST_F macros (Google Test)
            if (content.find("TEST_F") != std::string::npos) {
                size_t test_count = std::count(content.begin(), content.end(), 'T') + 
                                 std::count(content.begin(), content.end(), 't');
                
                if (entry.path().string().find("test_brain") != std::string::npos) {
                    auto& brain_status = component_statuses["NEURAL_CORE"];
                    brain_status.tested = true;
                    brain_status.integration_status += " (Google Test coverage)";
                }
            }
        }
    }
}

void ImplementationStatusTracker::analyzeBrainLoopIntegration() {
    // Key components that should be in Brain::step()
    std::vector<std::string> brain_loop_components = {
        "WORKING_MEMORY",
        "EPISODIC_MEMORY", 
        "SEMANTIC_MEMORY",
        "PROCEDURAL_MEMORY",
        "NEURAL_PLANNER",
        "CONCEPT_FORMATION",
        "PREDICTION_SYSTEM",
        "DEVELOPMENT_SYSTEM",
        "DOPAMINE"
    };
    
    for (const auto& id : brain_loop_components) {
        auto it = component_statuses.find(id);
        if (it != component_statuses.end()) {
            // Check if connected to brain loop
            if (it->second.status == ImplementationStatus::DISCONNECTED || 
                it->second.status == ImplementationStatus::PARTIAL) {
                it->second.connected_to_brain_loop = false;
            }
        }
    }
}

void ImplementationStatusTracker::analyzeMemorySystems() {
    std::vector<std::string> memory_components = {
        "WORKING_MEMORY",
        "EPISODIC_MEMORY",
        "SEMANTIC_MEMORY", 
        "PROCEDURAL_MEMORY",
        "ASSOCIATIVE_MEMORY"
    };
    
    for (const auto& id : memory_components) {
        auto it = component_statuses.find(id);
        if (it != component_statuses.end()) {
            // Check integration with brain
            if (!it->second.connected_to_brain_loop) {
                it->second.integration_status += "; Memory integration needed";
            }
        }
    }
}

void ImplementationStatusTracker::analyzeCognitionSystems() {
    std::vector<std::string> cognition_components = {
        "NEURAL_PLANNER",
        "CONCEPT_FORMATION",
        "SELF_MODEL",
        "SOCIAL_LEARNING",
        "SPATIAL_REPRESENTATION",
        "TEMPORAL_RELATION"
    };
    
    for (const auto& id : cognition_components) {
        auto it = component_statuses.find(id);
        if (it != component_statuses.end()) {
            // Check integration with perception/action
            if (!it->second.connected_to_brain_loop) {
                it->second.integration_status += "; Cognition integration needed";
            }
        }
    }
}

void ImplementationStatusTracker::analyzeNeuromodulation() {
    // Check neuromodulation components
    std::vector<std::string> neuromod_components = {
        "DOPAMINE",
        "ACETYLCHOLINE",
        "NOREPINEPHRINE", 
        "SEROTONIN",
        "CURIOUSITY",
        "NOVELTY_DETECTION",
        "PREDICTION_ERROR"
    };
    
    for (const auto& id : neuromod_components) {
        auto it = component_statuses.find(id);
        if (it != component_statuses.end()) {
            // Check integration with plasticity and learning
            if (it->second.status == ImplementationStatus::PLACEHOLDER) {
                it->second.integration_status += "; Neuromodulation integration needed";
            }
        }
    }
}

void ImplementationStatusTracker::analyzePredictionSystem() {
    auto it = component_statuses.find("PREDICTION_SYSTEM");
    if (it != component_statuses.end()) {
        // Check connection to sensory processing
        if (!it->second.connected_to_brain_loop) {
            it->second.integration_status += "; Prediction integration needed";
        }
    }
}

void ImplementationStatusTracker::analyzeDevelopmentSystem() {
    auto it = component_statuses.find("DEVELOPMENT_SYSTEM");
    if (it != component_statuses.end()) {
        // Check comprehensive development effects
        if (it->second.completion_percentage < 50.0) {
            it->second.integration_status += "; Development needs expansion";
        }
    }
}

void ImplementationStatusTracker::analyzePersistence() {
    auto it = component_statuses.find("CHECKPOINT_SYSTEM");
    if (it != component_statuses.end()) {
        // Check integration with Brain save/load
        if (it->second.status == ImplementationStatus::DISCONNECTED) {
            it->second.integration_status += "; Checkpoint integration needed";
        }
    }
}

void ImplementationStatusTracker::analyzeEmbodiment() {
    // Check embodied systems integration
    std::vector<std::string> embodied_components = {
        "AGENT_BRAIN",
        "SIMPLE_WORLD",
        "SENSORY_INPUT",
        "ACTION_SYSTEM"
    };
    
    for (const auto& id : embodied_components) {
        auto it = component_statuses.find(id);
        if (it != component_statuses.end()) {
            // Check working integration
            if (it->second.connected_to_brain_loop) {
                it->second.integration_status += "; Embodied system working";
            }
        }
    }
}

void ImplementationStatusTracker::analyzeExperiments() {
    auto it = component_statuses.find("TEST_FRAMEWORK");
    if (it != component_statuses.end()) {
        // Check test coverage
        if (!it->second.tested) {
            it->second.integration_status += "; Test framework needs improvement";
        }
    }
}

void ImplementationStatusTracker::generateAuditReport() const {
    std::cout << "========================================" << std::endl;
    std::cout << "NLM/熙然 - Phase 6 Final Audit Report" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Date: " << audit_date << std::endl;
    std::cout << "Phase: 6 - Final Integration, Maturation & Artificial Brain" << std::endl;
    std::cout << std::endl;
    
    std::cout << "OVERALL STATUS: " << achieved_points << "/" << total_points << " points ("
              << std::fixed << std::setprecision(1) << getOverallCompletionRate() << "%)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "COMPONENT STATUS SUMMARY:" << std::endl;
    std::cout << "------------------------" << std::endl;
    
    for (const auto& pair : component_statuses) {
        std::cout << std::left << std::setw(25) << pair.first << " : "
                  << std::setw(15) << getStatusString(pair.second.status)
                  << " : " << std::setw(3) << pair.second.completion_percentage << "%"
                  << " : " << pair.second.integration_status << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "CRITICAL GAPS:" << std::endl;
    std::cout << "---------------" << std::endl;
    
    // Identify critical gaps
    std::vector<std::string> critical_gaps;
    for (const auto& pair : component_statuses) {
        if (pair.second.status == ImplementationStatus::DISCONNECTED ||
            pair.second.status == ImplementationStatus::PLACEHOLDER) {
            critical_gaps.push_back(pair.first);
        }
    }
    
    if (critical_gaps.empty()) {
        std::cout << "No critical gaps identified." << std::endl;
    } else {
        for (const auto& gap : critical_gaps) {
            std::cout << "- " << gap << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "RECOMMENDATIONS:" << std::endl;
    std::cout << "----------------" << std::endl;
    std::cout << "1. Connect Memory Systems to Brain Loop" << std::endl;
    std::cout << "2. Integrate Cognition Systems into Perception/Action" << std::endl;
    std::cout << "3. Implement Full Neuromodulation Effects" << std::endl;
    std::cout << "4. Connect Prediction System to Sensory Processing" << std::endl;
    std::cout << "5. Expand Development System Integration" << std::endl;
    std::cout << "6. Connect Checkpoint System to Brain Save/Load" << std::endl;
    std::cout << std::endl;
}

void ImplementationStatusTracker::generateIntegrationRoadmap() const {
    std::cout << "========================================" << std::endl;
    std::cout << "INTEGRATION ROADMAP" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 1: CONNECT EXISTING SYSTEMS (Priority)" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "1. WorkingMemory ↔ SensoryInput Processing" << std::endl;
    std::cout << "   - Update Brain::getWorkingMemory() to return working instance" << std::endl;
    std::cout << "   - Integrate with sensory data streams" << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. EpisodicMemory ↔ Experience Logging" << std::endl;
    std::cout << "   - Implement episodic memory storage" << std::endl;
    std::cout << "   - Connect to sensory and action sequences" << std::endl;
    std::cout << std::endl;
    
    std::cout << "3. PredictionSystem ↔ Sensory Processing" << std::endl;
    std::cout << "   - Integrate prediction into brain loop" << std::endl;
    std::cout << "   - Use predictions for attention and planning" << std::endl;
    std::cout << std::endl;
    
    std::cout << "4. NeuralPlanner ↔ Action Selection" << std::endl;
    std::cout << "   - Instantiate NeuralPlanner in AgentBrain" << std::endl;
    std::cout << "   - Connect to action production system" << std::endl;
    std::cout << std::endl;
    
    std::cout << "5. ConceptFormation ↔ Experience Processing" << std::endl;
    std::cout << "   - Instantiate ConceptFormation" << std::endl;
    std::cout << "   - Connect to episodic memory" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 2: IMPLEMENT MISSING MECHANISMS" << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "1. Brain::save() and Brain::load()" << std::endl;
    std::cout << "   - Implement using CheckpointSystem" << std::endl;
    std::cout << "   - Connect memory and state persistence" << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. Full Neuromodulation" << std::endl;
    std::cout << "   - Implement ACh effects on attention" << std::endl;
    std::cout << "   - Implement NE effects on arousal" << std::endl;
    std::cout << "   - Implement 5-HT effects on mood" << std::endl;
    std::cout << std::endl;
    
    std::cout << "3. Sleep/Rest Cycle" << std::endl;
    std::cout << "   - Implement consolidation mechanisms" << std::endl;
    std::cout << "   - Add replay during rest" << std::endl;
    std::cout << std::endl;
    
    std::cout << "4. Replay Mechanisms" << std::endl;
    std::cout << "   - Implement memory replay" << std::endl;
    std::cout << "   - Connect to development stages" << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 3: VALIDATE INTEGRATION" << std::endl;
    std::cout << "----------------------------" << std::endl;
    std::cout << "1. Test Memory Retention" << std::endl;
    std::cout << "   - Longitudinal memory tests" << std::endl;
    std::cout << "   - Catastrophic forgetting tests" << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. Test Prediction Accuracy" << std::endl;
    std::cout << "   - Prediction improvement measurements" << std::endl;
    std::cout << "   - Confidence calibration tests" << std::endl;
    std::cout << std::endl;
    
    std::cout << "3. Test Development" << std::endl;
    std::cout << "   - Developmental stage progression" << std::endl;
    std::cout << "   - Multi-system interaction" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE 4: OPTIMIZE" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "1. Performance Profiling" << std::endl;
    std::cout << "   - Profile hot paths" << std::endl;
    std::cout << "   - Optimize critical operations" << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. Integrate Performance Infrastructure" << std::endl;
    std::cout << "   - Memory pools in brain loop" << std::endl;
    std::cout << "   - Event queue integration" << std::endl;
    std::cout << std::endl;
    
    std::cout << "3. Enable Safe Parallelization" << std::endl;
    std::cout << "   - Thread-safe component access" << std::endl;
    std::cout << "   - Parallel processing where safe" << std::endl;
    std::cout << std::endl;
}

void ImplementationStatusTracker::updateDocumentation() const {
    std::cout << "========================================" << std::endl;
    std::cout << "DOCUMENTATION UPDATE REQUIRED" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "The following documentation needs updating:" << std::endl;
    std::cout << std::endl;
    
    // Check which documentation files exist
    std::vector<std::string> doc_files = {
        "docs/README.md",
        "docs/PHASE6_FINAL_AUDIT.md",
        "docs/ARCHITECTURE.md",
        "docs/PHASE6_FINAL_RESULTS.md"
    };
    
    for (const auto& doc_file : doc_files) {
        std::ifstream file(doc_file);
        if (file.good()) {
            std::cout << "✓ " << doc_file << " - EXISTS" << std::endl;
            
            // Check for specific claims that need updating
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            // Look for claims about completeness
            if (content.find("Phase 6 complete") != std::string::npos ||
                content.find("fully implemented") != std::string::npos) {
                std::cout << "  ⚠️  Contains claims needing verification" << std::endl;
            }
        } else {
            std::cout << "✗ " << doc_file << " - MISSING" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "REQUIRED UPDATES:" << std::endl;
    std::cout << std::endl;
    std::cout << "1. Update README.md to reflect actual implementation status" << std::endl;
    std::cout << "2. Update PHASE6_FINAL_AUDIT.md with current findings" << std::endl;
    std::cout << "3. Update ARCHITECTURE.md with integration status" << std::endl;
    std::cout << "4. Add PHASE6_FINAL_RESULTS.md with test results" << std::endl;
    std::cout << "5. Create IMPLEMENTATION_STATUS_REPORT.md with detailed analysis" << std::endl;
    std::cout << "6. Update all documentation to remove false claims of completion" << std::endl;
}

double ImplementationStatusTracker::getOverallCompletionRate() const {
    return static_cast<double>(achieved_points) / total_points * 100.0;
}

const char* ImplementationStatusTracker::getStatusString(ImplementationStatus status) const {
    switch (status) {
        case ImplementationStatus::NOT_IMPLEMENTED: return "NOT IMPLEMENTED";
        case ImplementationStatus::PLACEHOLDER: return "PLACEHOLDER";
        case ImplementationStatus::PARTIAL: return "PARTIAL";
        case ImplementationStatus::WORKING: return "WORKING";
        case ImplementationStatus::DISCONNECTED: return "DISCONNECTED";
        case ImplementationStatus::COMPLETED: return "COMPLETED";
        default: return "UNKNOWN";
    }
}

void ImplementationStatusTracker::printStatusSummary() const {
    std::cout << "========================================" << std::endl;
    std::cout << "IMPLEMENTATION STATUS SUMMARY" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Total Components: " << component_statuses.size() << std::endl;
    std::cout << "Overall Score: " << achieved_points << "/" << total_points << " ("
              << std::fixed << std::setprecision(1) << getOverallCompletionRate() << "%)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Status Breakdown:" << std::endl;
    std::cout << "- Working: " << countComponentsByStatus(ImplementationStatus::WORKING) << std::endl;
    std::cout << "- Partial: " << countComponentsByStatus(ImplementationStatus::PARTIAL) << std::endl;
    std::cout << "- Disconnected: " << countComponentsByStatus(ImplementationStatus::DISCONNECTED) << std::endl;
    std::cout << "- Placeholder: " << countComponentsByStatus(ImplementationStatus::PLACEHOLDER) << std::endl;
    std::cout << "- Not Implemented: " << countComponentsByStatus(ImplementationStatus::NOT_IMPLEMENTED) << std::endl;
    std::cout << "- Completed: " << countComponentsByStatus(ImplementationStatus::COMPLETED) << std::endl;
}

int ImplementationStatusTracker::countComponentsByStatus(ImplementationStatus status) const {
    int count = 0;
    for (const auto& pair : component_statuses) {
        if (pair.second.status == status) {
            count++;
        }
    }
    return count;
}

} // namespace nlm

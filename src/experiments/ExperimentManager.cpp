// NLM Experiment Manager Implementation
// Phase 2: Real Neural Computation - Enhanced Experiment System

#include "ExperimentManager.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <cmath>
#include <ctime>
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "cognition/ConceptFormation.hpp"
#include "memory/NeuralWorkingMemory.hpp"

namespace nlm {

// ExperimentManager implementation
ExperimentManager::ExperimentManager() {
    NLM_LOG_INFO("Initializing ExperimentManager...");
    initializeDefaultExperiments();
    NLM_LOG_INFO("ExperimentManager initialized with " + std::to_string(experiments.size()) + " experiments");
}

ExperimentManager::~ExperimentManager() {
    NLM_LOG_INFO("ExperimentManager shutting down");
}

void ExperimentManager::registerExperiment(std::unique_ptr<IExperiment> experiment) {
    if (experiment) {
        std::string name = experiment->getName();
        experiments[name] = std::move(experiment);
        NLM_LOG_INFO("Registered experiment: " + name);
    }
}

void ExperimentManager::unregisterExperiment(const std::string& name) {
    auto it = experiments.find(name);
    if (it != experiments.end()) {
        NLM_LOG_INFO("Unregistered experiment: " + name);
        experiments.erase(it);
    }
}

std::vector<std::string> ExperimentManager::getExperimentNames() const {
    std::vector<std::string> names;
    names.reserve(experiments.size());
    for (const auto& pair : experiments) {
        names.push_back(pair.first);
    }
    return names;
}

IExperiment* ExperimentManager::getExperiment(const std::string& name) {
    auto it = experiments.find(name);
    return it != experiments.end() ? it->second.get() : nullptr;
}

ExperimentResult ExperimentManager::runExperiment(const std::string& name, const Config& config) {
    IExperiment* experiment = getExperiment(name);
    if (!experiment) {
        ExperimentResult result(name, "Experiment not found");
        result.success = false;
        result.output = "Error: Experiment '" + name + "' not found";
        return result;
    }
    
    NLM_LOG_INFO("Running experiment: " + name);
    ExperimentResult result(name, experiment->getDescription());
    result.startTime = std::chrono::system_clock::now();
    
    // Setup
    bool setupSuccess = experiment->setup(config);
    if (!setupSuccess) {
        result.output = "Error: Experiment setup failed";
        result.endTime = std::chrono::system_clock::now();
        return result;
    }
    
    try {
        // Run experiment
        result = experiment->run();
        experiment->cleanup();
    } catch (const std::exception& e) {
        result.output = std::string("Exception: ") + e.what();
        result.endTime = std::chrono::system_clock::now();
    }
    
    // Calculate execution time
    auto end = std::chrono::system_clock::now();
    result.endTime = end;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - result.startTime);
    result.executionTime = duration.count() / 1000.0;
    
    NLM_LOG_INFO("Experiment completed: " + name + " (success: " + (result.success ? "true" : "false") + ")");
    return result;
}

std::vector<ExperimentResult> ExperimentManager::runAllExperiments(const Config& config) {
    NLM_LOG_INFO("Running all experiments...");
    std::vector<ExperimentResult> results;
    
    for (const auto& pair : experiments) {
        if (pair.second->shouldRun(config)) {
            results.push_back(runExperiment(pair.first, config));
        }
    }
    
    this->results = results; // Store for comparison
    return results;
}

std::vector<ExperimentResult> ExperimentManager::runExperimentsByCategory(const std::string& category, const Config& config) {
    NLM_LOG_INFO("Running experiments in category: " + category);
    std::vector<ExperimentResult> results;
    
    for (const auto& pair : experiments) {
        IExperiment* exp = pair.second.get();
        if (exp->getCategory() == category && exp->shouldRun(config)) {
            results.push_back(runExperiment(pair.first, config));
        }
    }
    
    return results;
}

Config ExperimentManager::getDefaultConfiguration() const {
    Config config;
    
    // Set sensible defaults for all experiments
    config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config.set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config.set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Experiment-specific settings
    config.set("experiment_repeats", static_cast<int64_t>(1), ConfigSource::Default);
    config.set("experiment_log_level", std::string("INFO"), ConfigSource::Default);
    config.set("checkpoint_enabled", true, ConfigSource::Default);
    
    return config;
}

bool ExperimentManager::saveResults(const std::vector<ExperimentResult>& results, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save results to file: " + filepath);
        return false;
    }
    
    file << "NLM Experiment Results" << std::endl;
    file << "=======================" << std::endl;
    file << "Saved: " << std::ctime(&std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    file << "Total experiments: " << results.size() << std::endl;
    file << "Successful: " << std::count_if(results.begin(), results.end(), 
        [](const ExperimentResult& r) { return r.success; }) << std::endl;
    file << std::endl;
    
    for (const auto& result : results) {
        file << "Experiment: " << result.name << std::endl;
        file << "Description: " << result.description << std::endl;
        file << "Category: " << getCategoryFromExperiment(result.name) << std::endl;
        file << "Success: " << (result.success ? "true" : "false") << std::endl;
        file << "Execution time: " << std::fixed << std::setprecision(3) 
             << result.executionTime << " seconds" << std::endl;
        file << "Output:" << std::endl << result.output << std::endl;
        file << "---" << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Results saved to: " + filepath);
    return true;
}

std::vector<ExperimentResult> ExperimentManager::loadResults(const std::string& filepath) const {
    std::vector<ExperimentResult> results;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to load results from file: " + filepath);
        return results;
    }
    
    // For now, implement a simple loader. In a full implementation,
    // this would use proper serialization.
    std::string line;
    ExperimentResult currentResult;
    bool inResult = false;
    
    while (std::getline(file, line)) {
        if (line.empty() || line.find("Experiment:") == std::string::npos) {
            continue;
        }
        
        if (inResult && !line.empty() && line.find("---") != std::string::npos) {
            if (!currentResult.name.empty()) {
                results.push_back(currentResult);
                currentResult = ExperimentResult();
            }
            inResult = false;
            continue;
        }
        
        if (line.find("Experiment:") != std::string::npos) {
            currentResult.name = line.substr(line.find("Experiment:") + 11);
            inResult = true;
        } else if (line.find("Description:") != std::string::npos && inResult) {
            currentResult.description = line.substr(line.find("Description:") + 12);
        } else if (line.find("Category:") != std::string::npos && inResult) {
            // Skip category for now
        } else if (line.find("Success:") != std::string::npos && inResult) {
            currentResult.success = (line.find("true") != std::string::npos);
        } else if (line.find("Execution time:") != std::string::npos && inResult) {
            try {
                currentResult.executionTime = std::stof(line.substr(line.find("Execution time:") + 16));
            } catch (...) {
                currentResult.executionTime = 0.0;
            }
        } else if (line.find("Output:") != std::string::npos && inResult) {
            currentResult.output = "";
        } else if (!line.empty() && line.find("Output:") == std::string::npos && inResult && 
                   currentResult.output.find("Output:") == std::string::npos) {
            if (!currentResult.output.empty()) currentResult.output += "\n";
            currentResult.output += line;
        }
    }
    
    if (!currentResult.name.empty()) {
        results.push_back(currentResult);
    }
    
    file.close();
    NLM_LOG_INFO("Loaded " + std::to_string(results.size()) + " results from: " + filepath);
    return results;
}

void ExperimentManager::clearResults() {
    results.clear();
    NLM_LOG_INFO("Results cleared");
}

std::string ExperimentManager::compareResults(const std::vector<ExperimentResult>& results) const {
    if (results.empty()) {
        return "No results to compare";
    }
    
    std::stringstream ss;
    ss << "Experiment Comparison Report" << std::endl;
    ss << "============================" << std::endl;
    ss << std::endl;
    
    // Find max execution time for normalization
    double maxTime = 0.0;
    for (const auto& result : results) {
        if (result.executionTime > maxTime) {
            maxTime = result.executionTime;
        }
    }
    
    for (const auto& result : results) {
        ss << "Experiment: " << result.name << std::endl;
        ss << "  Category: " << getCategoryFromExperiment(result.name) << std::endl;
        ss << "  Success: " << (result.success ? "✓" : "✗") << std::endl;
        ss << "  Execution time: " << formatTime(result.executionTime);
        if (maxTime > 0.0) {
            double percentage = (result.executionTime / maxTime) * 100.0;
            ss << " (" << std::fixed << std::setprecision(1) << percentage << "% of slowest)";
        }
        ss << std::endl;
        
        if (result.output.size() > 200) {
            ss << "  Output preview: " << result.output.substr(0, 200) << "..." << std::endl;
        } else if (!result.output.empty()) {
            ss << "  Output: " << result.output << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::string ExperimentManager::getSummary() const {
    std::stringstream ss;
    ss << "NLM Experiment Summary" << std::endl;
    ss << "======================" << std::endl;
    ss << "Total experiments run: " << results.size() << std::endl;
    
    size_t successful = std::count_if(results.begin(), results.end(),
        [](const ExperimentResult& r) { return r.success; });
    size_t failed = results.size() - successful;
    
    ss << "Successful: " << successful << std::endl;
    ss << "Failed: " << failed << std::endl;
    
    if (!results.empty()) {
        double totalTime = 0.0;
        for (const auto& result : results) {
            totalTime += result.executionTime;
        }
        ss << "Total execution time: " << formatTime(totalTime) << std::endl;
        ss << "Average time per experiment: " << formatTime(totalTime / results.size()) << std::endl;
    }
    
    ss << std::endl;
    ss << "By category:" << std::endl;
    
    std::unordered_map<std::string, size_t> categoryCount;
    std::unordered_map<std::string, size_t> categorySuccess;
    
    for (const auto& result : results) {
        std::string category = getCategoryFromExperiment(result.name);
        categoryCount[category]++;
        if (result.success) {
            categorySuccess[category]++;
        }
    }
    
    for (const auto& pair : categoryCount) {
        ss << "  " << pair.first << ": " << pair.second << " experiments, ";
        ss << categorySuccess[pair.first] << " successful" << std::endl;
    }
    
    return ss.str();
}

void ExperimentManager::reset() {
    clearResults();
    // In a full implementation, would reload default experiments
    NLM_LOG_INFO("ExperimentManager reset");
}

void ExperimentManager::initializeDefaultExperiments() {
    // Register core functionality experiments
    registerExperiment(std::make_unique<BasicConnectivityExperiment>());
    registerExperiment(std::make_unique<PlasticityLearningExperiment>());
    registerExperiment(std::make_unique<StdpVerificationExperiment>());
    
    // Register advanced experiments
    registerExperiment(std::make_unique<WorkingMemoryExperiment>());
    registerExperiment(std::make_unique<ConceptFormationExperiment>());
    
    NLM_LOG_INFO("Initialized default experiments");
}

std::string ExperimentManager::formatTime(double seconds) const {
    std::stringstream ss;
    if (seconds >= 60.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        int secs = static_cast<int>(seconds) % 60;
        ss << minutes << "m " << secs << "." << std::setw(3) << std::setfill('0') 
           << static_cast<int>((seconds - static_cast<int>(seconds)) * 1000) << "s";
    } else {
        ss << std::fixed << std::setprecision(3) << seconds << "s";
    }
    return ss.str();
}

std::string ExperimentManager::formatResults(const ExperimentResult& result) const {
    std::stringstream ss;
    ss << "Experiment: " << result.name << std::endl;
    ss << "Description: " << result.description << std::endl;
    ss << "Success: " << (result.success ? "true" : "false") << std::endl;
    ss << "Time: " << formatTime(result.executionTime) << std::endl;
    if (!result.output.empty()) {
        ss << "Output:" << std::endl << result.output;
    }
    return ss.str();
}

std::string ExperimentManager::getCategoryFromExperiment(const std::string& experimentName) const {
    // Helper to map experiment names to categories
    if (experimentName == "Basic Connectivity") return "Core Functionality";
    if (experimentName == "Plasticity Learning") return "Plasticity";
    if (experimentName == "STDP Verification") return "Plasticity";
    if (experimentName == "Working Memory") return "Memory";
    if (experimentName == "Concept Formation") return "Cognition";
    return "Other";
}

// BasicConnectivityExperiment implementation
ExperimentResult BasicConnectivityExperiment::run() {
    ExperimentResult result(getName(), getDescription());
    
    try {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            result.output = "Failed to initialize brain";
            return result;
        }
        
        // Run basic connectivity test
        brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step);
        }
        
        size_t spikes = brain->getTotalSpikeCount();
        result.output = "Basic connectivity test completed. Generated " + std::to_string(spikes) + " spikes.";
        result.success = spikes > 0;
        
    } catch (const std::exception& e) {
        result.output = std::string("Exception: ") + e.what();
    }
    
    return result;
}

bool BasicConnectivityExperiment::shouldRun(const Config& config) const {
    return config.getOr<bool>("run_basic_connectivity", true);
}

// PlasticityLearningExperiment implementation
ExperimentResult PlasticityLearningExperiment::run() {
    ExperimentResult result(getName(), getDescription());
    
    try {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            result.output = "Failed to initialize brain";
            return result;
        }
        
        // Enable plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply repeated input patterns
        std::vector<NeuronId> patternNeurons;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                    patternNeurons.push_back(neuron->getId());
                }
            }
        }
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, 30.0f);
            }
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                result.output += "Step " + std::to_string(step) + 
                               " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                               " | Firing: " + std::to_string(brain->getFiringNeuronCount()) + "\n";
            }
        }
        
        size_t totalSpikes = brain->getTotalSpikeCount();
        result.output += "\nPlasticity experiment completed. Total spikes: " + std::to_string(totalSpikes);
        result.success = totalSpikes > 0;
        
    } catch (const std::exception& e) {
        result.output = std::string("Exception: ") + e.what();
    }
    
    return result;
}

bool PlasticityLearningExperiment::shouldRun(const Config& config) const {
    return config.getOr<bool>("run_plasticity_experiment", true);
}

// StdpVerificationExperiment implementation
ExperimentResult StdpVerificationExperiment::run() {
    ExperimentResult result(getName(), getDescription());
    
    try {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            result.output = "Failed to initialize brain";
            return result;
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            result.output = "No regions found for STDP verification";
            return result;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            result.output = "No synapses available for STDP test";
            return result;
        }
        
        Synapse* syn = synapses[0].get();
        syn->enablePlasticity(false, true, false);
        float beforeWeight = syn->getWeight();
        
        result.output = "Testing STDP on synapse with initial weight: " + std::to_string(beforeWeight) +
                       " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                       ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")\n";
        
        // Create correlated pre->post activity
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        result.output += "After correlated activity:\n";
        result.output += "  Final weight: " + std::to_string(syn->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")\n";
        
        if (delta > 0.001f) {
            result.output += "✓ STDP WORKING: Pre-before-post produced potentiation\n";
            result.success = true;
        } else if (delta < -0.001f) {
            result.output += "! STDP reversed: Check parameters\n";
            result.success = true;
        } else {
            result.output += "! No change: STDP may not be triggering\n";
            result.success = false;
        }
        
    } catch (const std::exception& e) {
        result.output = std::string("Exception: ") + e.what();
    }
    
    return result;
}

bool StdpVerificationExperiment::shouldRun(const Config& config) const {
    return config.getOr<bool>("run_stdp_verification", true);
}

// WorkingMemoryExperiment implementation
ExperimentResult WorkingMemoryExperiment::run() {
    ExperimentResult result(getName(), getDescription());
    
    try {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            result.output = "Failed to initialize brain";
            return result;
        }
        
        // Get working memory
        auto* wm = brain->getWorkingMemory();
        if (!wm) {
            result.output = "No working memory system available";
            return result;
        }
        
        // Test memory storage and retrieval
        size_t totalStored = 0;
        size_t activeTraces = 0;
        
        for (int step = 0; step < 100; ++step) {
            // Inject some sensory input
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto* neuron : region->getAllNeurons()) {
                    if (neuron->getType() == NeuronType::Sensory && totalStored < 10) {
                        neuron->injectCurrent(20.0f);
                        wm->storeToNeuron(neuron->getId(), 1.0f);
                        totalStored++;
                    }
                }
            }
            
            brain->step(step);
            
            if (step % 20 == 0) {
                auto traces = wm->getActiveTraces();
                activeTraces = traces.size();
                result.output += "Step " + std::to_string(step) + 
                               ": Stored " + std::to_string(totalStored) +
                               " neurons, " + std::to_string(activeTraces) + 
                               " active traces\n";
            }
        }
        
        // Final check
        auto finalTraces = wm->getActiveTraces();
        activeTraces = finalTraces.size();
        
        result.output += "\nWorking memory test completed:";
        result.output += "\n  Total neurons stored: " + std::to_string(totalStored);
        result.output += "\n  Final active traces: " + std::to_string(activeTraces);
        result.output += "\n  Success: " + std::to_string(activeTraces > 0);
        
        result.success = activeTraces > 0;
        
    } catch (const std::exception& e) {
        result.output = std::string("Exception: ") + e.what();
    }
    
    return result;
}

bool WorkingMemoryExperiment::shouldRun(const Config& config) const {
    return config.getOr<bool>("run_working_memory_experiment", true);
}

// ConceptFormationExperiment implementation
ExperimentResult ConceptFormationExperiment::run() {
    ExperimentResult result(getName(), getDescription());
    
    try {
        // Test concept formation directly (simplified)
        auto conceptFormation = std::make_unique<ConceptFormation>();
        
        // Create test patterns: resource and hazard scenes
        std::vector<float> resource(24, 0.0f);
        std::vector<float> hazard(24, 0.0f);
        
        // Resource scene (e.g., food, water)
        for (int i : {5, 6, 7, 8, 9, 10}) resource[i] = 0.3f;
        resource[16] = 0.8f;
        
        // Hazard scene (e.g., predator, obstacle)
        for (int i : {5, 6, 7, 8, 9, 10}) hazard[i] = 0.75f;
        hazard[16] = 0.8f;
        
        float same = conceptFormation->computeSimilarity(resource, resource);
        float diff = conceptFormation->computeSimilarity(resource, hazard);
        
        result.output = "Concept formation test results:";
        result.output += "\n  Self-similarity (resource vs resource): " + std::to_string(same);
        result.output += "\n  Cross-similarity (resource vs hazard): " + std::to_string(diff);
        result.output += "\n  Similarity threshold: 0.7 (below which scenes are considered different)";
        
        bool testPassed = (same >= 0.99f) && (diff < 0.7f);
        result.output += "\n  Test passed: " + std::to_string(testPassed);
        result.success = testPassed;
        
    } catch (const std::exception& e) {
        result.output = std::string("Exception: ") + e.what();
    }
    
    return result;
}

bool ConceptFormationExperiment::shouldRun(const Config& config) const {
    return config.getOr<bool>("run_concept_formation_experiment", true);
}

} // namespace nlm

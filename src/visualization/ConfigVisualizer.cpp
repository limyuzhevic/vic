#include "ConfigVisualizer.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <numeric>
#include <map>

namespace nlm {

struct ConfigVisualizer::Impl {
    bool active;
    double updateRate;
    std::vector<std::string> filteredCategories;
    std::vector<std::string> filteredSources;
    std::vector<ConfigParameter> tempParameters;
    std::unordered_map<std::string, double> paramImpactCache;
    
    Impl() : active(false), updateRate(1.0) {
        // Initialize with common categories
        filteredCategories = {"performance", "stability", "accuracy", "behavior"};
        
        // Initialize with common sources
        filteredSources = {"default", "config_file", "command_line", "runtime"};
    }
};

ConfigVisualizer::ConfigVisualizer() : active_(false), updateRate_(1.0) {
    pImpl = std::make_unique<Impl>();
    initialize();
}

ConfigVisualizer::~ConfigVisualizer() = default;

bool ConfigVisualizer::initialize() {
    active_ = true;
    NLM_LOG_INFO("ConfigVisualizer initialized");
    return true;
}

void ConfigVisualizer::update(std::shared_ptr<Config> config) {
    if (!active_ || !config) return;
    
    // Collect configuration parameters
    collectConfigParameters(config);
    
    // Build inheritance tree
    buildInheritanceTree();
    
    // Analyze parameter impacts
    analyzeParameterImpacts();
    
    // Collect default values
    collectDefaultValues();
    
    // Update dependency relationships
    updateDependencyRelationships();
}

void ConfigVisualizer::renderConfig(const ConfigOptions& options) const {
    std::cout << "=== Configuration Visualizer ===" << std::endl;
    
    if (currentConfigParameters_.empty()) {
        std::cout << "No configuration parameters available." << std::endl;
        return;
    }
    
    std::cout << "Configuration Parameters: " << currentConfigParameters_.size() << std::endl;
    
    // Filter parameters based on options
    std::vector<ConfigParameter> filteredParams = currentConfigParameters_;
    
    if (options.showOnlyOverrides) {
        filteredParams.erase(std::remove_if(filteredParams.begin(), filteredParams.end(),
                                            [](const ConfigParameter& param) { return !param.isOverridden; }),
                            filteredParams.end());
    }
    
    // Filter by categories if specified
    if (options.showDetailedInfo && !pImpl->filteredCategories.empty()) {
        // Note: In a real implementation, parameters would have categories
        // For now, we'll show all parameters
    }
    
    // Render based on visualization type
    if (options.visualizationType == "tree") {
        renderTreeView(filteredParams, options);
    } else if (options.visualizationType == "flat") {
        renderFlatView(filteredParams, options);
    } else if (options.visualizationType == "grouped") {
        renderGroupedView(filteredParams, options);
    }
    
    // Show impact analysis if requested
    if (options.showImpactAnalysis && !currentParameterImpacts_.empty()) {
        renderParameterImpact();
    }
}

void ConfigVisualizer::renderInheritanceTree() const {
    std::cout << "\n=== Configuration Inheritance Tree ===" << std::endl;
    
    renderInheritanceNode(currentInheritance_, 0);
}

void ConfigVisualizer::renderParameterImpact() const {
    std::cout << "\n=== Parameter Impact Analysis ===" << std::endl;
    
    if (currentParameterImpacts_.empty()) {
        std::cout << "No impact analysis available." << std::endl;
        return;
    }
    
    std::cout << "Parameter Impact Scores (Higher = More Impact):" << std::endl;
    
    // Sort by total impact
    std::vector<ParameterImpact> sortedImpacts = currentParameterImpacts_;
    std::sort(sortedImpacts.begin(), sortedImpacts.end(),
              [](const ParameterImpact& a, const ParameterImpact& b) {
                  return (a.impactOnPerformance + a.impactOnStability + a.impactOnAccuracy) >
                         (b.impactOnPerformance + b.impactOnStability + b.impactOnAccuracy);
              });
    
    for (const auto& impact : sortedImpacts) {
        double totalImpact = impact.impactOnPerformance + impact.impactOnStability + impact.impactOnAccuracy;
        std::string impactType = getImpactColor(totalImpact);
        
        std::cout << "  " << impactType << impact.parameterName << ": " << std::fixed << std::setprecision(2)
                  << totalImpact * 100.0 << "%" << std::endl;
        
        if (!impact.affectedComponents.empty()) {
            std::cout << "    Affects: ";
            for (size_t i = 0; i < impact.affectedComponents.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << impact.affectedComponents[i];
            }
            std::cout << std::endl;
        }
    }
}

void ConfigVisualizer::renderDefaultValues() const {
    std::cout << "\n=== Default Values Viewer ===" << std::endl;
    
    if (currentDefaultValues_.empty()) {
        std::cout << "No default values available." << std::endl;
        return;
    }
    
    // Group by category
    std::unordered_map<std::string, std::vector<DefaultValueEntry>> groupedDefaults;
    for (const auto& entry : currentDefaultValues_) {
        groupedDefaults[entry.category].push_back(entry);
    }
    
    for (const auto& group : groupedDefaults) {
        std::cout << "\n" << group.first << ":" << std::endl;
        
        // Sort by parameter name
        std::vector<DefaultValueEntry> sortedEntries = group.second;
        std::sort(sortedEntries.begin(), sortedEntries.end(),
                  [](const DefaultValueEntry& a, const DefaultValueEntry& b) {
                      return a.parameterName < b.parameterName;
                  });
        
        for (const auto& entry : sortedEntries) {
            std::cout << "  " << std::left << std::setw(25) << entry.parameterName << " = "
                      << entry.defaultValue << " (" << entry.parameterType << ")";
            if (!entry.description.empty()) {
                std::cout << " - " << entry.description;
            }
            std::cout << std::endl;
        }
    }
}

void ConfigVisualizer::renderParameterDependencies() const {
    std::cout << "\n=== Parameter Dependencies ===" << std::endl;
    
    // Build dependency graph
    std::unordered_map<std::string, std::vector<std::string>> graph;
    std::unordered_map<std::string, std::vector<std::string>> reverseGraph;
    
    for (const auto& param : currentConfigParameters_) {
        for (const auto& dep : param.dependencies) {
            graph[dep].push_back(param.name);
            reverseGraph[param.name].push_back(dep);
        }
    }
    
    // Render dependency graph
    std::cout << "Dependency Graph:" << std::endl;
    
    // Find roots (parameters with no incoming dependencies)
    std::unordered_set<std::string> roots;
    for (const auto& param : currentConfigParameters_) {
        if (reverseGraph.find(param.name) == reverseGraph.end() || reverseGraph[param.name].empty()) {
            roots.insert(param.name);
        }
    }
    
    // Simple rendering - just show connections
    for (const auto& root : roots) {
        renderDependencyNode(root, reverseGraph, graph, 0);
    }
}

void ConfigVisualizer::generateConfigReport(const std::string& filename) const {
    std::cout << "Generating configuration report to " << filename << std::endl;
    
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to create configuration report file: " << filename << std::endl;
        return;
    }
    
    file << "=== Phase 6 Configuration Report ===" << std::endl;
    file << "Generated at: " << std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count() << "s ago" << std::endl;
    file << "\n=== Summary ===" << std::endl;
    file << "Total Parameters: " << currentConfigParameters_.size() << std::endl;
    file << "Overridden Parameters: " << countOverriddenParameters() << std::endl;
    file << "Parameter Categories: " << getUniqueCategories().size() << std::endl;
    
    file << "\n=== Configuration Parameters ===" << std::endl;
    for (const auto& param : currentConfigParameters_) {
        file << "- " << param.name << " (" << param.type << ")" << std::endl;
        file << "  Value: " << param.value << std::endl;
        file << "  Default: " << param.defaultValue << std::endl;
        file << "  Source: " << param.source << std::endl;
        file << "  Overridden: " << (param.isOverridden ? "Yes" : "No") << std::endl;
        if (!param.description.empty()) {
            file << "  Description: " << param.description << std::endl;
        }
        if (!param.dependencies.empty()) {
            file << "  Dependencies: ";
            for (size_t i = 0; i < param.dependencies.size(); ++i) {
                if (i > 0) file << ", ";
                file << param.dependencies[i];
            }
            file << std::endl;
        }
        file << std::endl;
    }
    
    file << "\n=== Inheritance Hierarchy ===" << std::endl;
    renderInheritanceToFile(file, currentInheritance_);
    
    file << "\n=== Parameter Impacts ===" << std::endl;
    for (const auto& impact : currentParameterImpacts_) {
        file << impact.parameterName << ": " << std::fixed << std::setprecision(3)
             << impact.impactOnPerformance << " (performance), "
             << impact.impactOnStability << " (stability), "
             << impact.impactOnAccuracy << " (accuracy)" << std::endl;
    }
    
    file << "\n=== End Configuration Report ===" << std::endl;
    file.close();
}

void ConfigVisualizer::generateConfigComparison(const ComparisonOptions& options) const {
    std::cout << "Generating configuration comparison report" << std::endl;
    
    if (options.baselineFile.empty() || options.comparisonFile.empty()) {
        std::cout << "Missing configuration files for comparison." << std::endl;
        return;
    }
    
    std::cout << "Comparing: " << options.baselineFile << " vs " << options.comparisonFile << std::endl;
    std::cout << "Comparison type: " << options.comparisonType << std::endl;
    std::cout << "Show differences: " << (options.showDifferences ? "Yes" : "No") << std::endl;
    std::cout << "Show similarities: " << (options.showSimilarities ? "Yes" : "No") << std::endl;
    
    // Note: In a real implementation, this would load the comparison files
    // and perform actual comparisons. For this prototype, we'll just output
    // a placeholder.
    
    std::cout << "\n[Note: Full comparison functionality would require loading configuration files]" << std::endl;
}

bool ConfigVisualizer::exportConfig(const std::string& filename, const std::string& format) const {
    std::cout << "Exporting configuration to " << filename << " (format: " << format << ")" << std::endl;
    
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Failed to create export file: " << filename << std::endl;
        return false;
    }
    
    if (format == "text") {
        file << "=== Exported Configuration ===" << std::endl;
        file << "Generated at: " << std::chrono::duration<double>(
            std::chrono::steady_clock::now().time_since_epoch()).count() << "s ago" << std::endl;
        file << "\nParameters:" << std::endl;
        
        for (const auto& param : currentConfigParameters_) {
            file << param.name << " = " << param.value << " # " << param.type << std::endl;
        }
    } else if (format == "json") {
        file << "{\n  \"parameters\": [\n";
        
        for (size_t i = 0; i < currentConfigParameters_.size(); ++i) {
            const auto& param = currentConfigParameters_[i];
            file << "    {\"name\": \"" << param.name << "\", \"type\": \"" << param.type 
                 << "\", \"value\": \"" << param.value << "\", \"source\": \"" << param.source
                 << "\"}";
            
            if (i < currentConfigParameters_.size() - 1) {
                file << ",";
            }
            file << "\n";
        }
        
        file << "  ]\n}";
    } else {
        std::cerr << "Unsupported export format: " << format << std::endl;
        return false;
    }
    
    file.close();
    return true;
}

void ConfigVisualizer::collectConfigParameters(std::shared_ptr<Config> config) {
    if (!config) return;
    
    currentConfigParameters_.clear();
    
    // Common configuration parameters based on NLM design
    std::vector<std::pair<std::string, std::string>> standardParams = {
        {"neuron_count", "int64"},
        {"region_count", "int64"},
        {"connection_probability", "float"},
        {"random_seed", "int64"},
        {"simulation_timestep", "double"},
        {"working_memory_size", "int64"},
        {"memory_consolidation_interval", "double"},
        {"neuromodulation_level", "float"},
        {"development_stage", "string"},
        {"prediction_window", "double"},
        {"replay_checkpoint_mode", "string"},
        {"memory_span", "int64"},
        {"stdp_ltp_weight", "float"},
        {"stdp_ltd_weight", "float"},
        {"stdp_tau", "float"},
        {"synaptogenesis_rate", "float"},
        {"pruning_rate", "float"},
        {"enable_checkpointing", "bool"},
        {"enable_replay", "bool"},
        {"enable_development", "bool"},
        {"debug_level", "int"},
        {"metrics_interval", "int"}
    };
    
    for (const auto& pair : standardParams) {
        ConfigParameter param;
        param.name = pair.first;
        param.type = pair.second;
        param.defaultValue = getDefaultValue(param.name, param.type);
        
        // Get current value from config
        if (config->has(param.name)) {
            if (param.type == "int64") {
                param.value = std::to_string(config->getOr<int64_t>(param.name, 
                                   std::stoll(param.defaultValue)));
            } else if (param.type == "double") {
                param.value = std::to_string(config->getOr<double>(param.name, 
                                   std::stod(param.defaultValue)));
            } else if (param.type == "float") {
                param.value = std::to_string(config->getOr<float>(param.name, 
                                   std::stof(param.defaultValue)));
            } else if (param.type == "bool") {
                param.value = config->getOr<bool>(param.name, 
                                        std::stoi(param.defaultValue)) ? "true" : "false";
            } else if (param.type == "string") {
                param.value = config->getOr<std::string>(param.name, param.defaultValue);
            }
            
            param.isOverridden = true;
            param.source = getConfigSource(param.name);
        } else {
            param.value = param.defaultValue;
            param.isOverridden = false;
            param.source = "default";
        }
        
        param.description = getParameterDescription(param.name);
        currentConfigParameters_.push_back(param);
    }
}

void ConfigVisualizer::buildInheritanceTree() {
    currentInheritance_.name = "root";
    currentInheritance_.type = "section";
    currentInheritance_.value = "NLM Configuration";
    currentInheritance_.defaultValue = "NLM Configuration";
    currentInheritance_.source = "system";
    currentInheritance_.inheritanceLevel = 0;
    currentInheritance_.isOverridden = false;
    
    // Build sections
    std::vector<std::string> sections = {"core", "memory", "neuromodulation", "plasticity", "cognition", "behavior"};
    
    for (const auto& section : sections) {
        InheritanceNode sectionNode;
        sectionNode.name = section;
        sectionNode.type = "section";
        sectionNode.value = "Section: " + section;
        sectionNode.defaultValue = "Default " + section + " settings";
        sectionNode.source = "system";
        sectionNode.inheritanceLevel = 1;
        sectionNode.isOverridden = false;
        
        currentInheritance_.children.push_back(sectionNode);
    }
}

void ConfigVisualizer::analyzeParameterImpacts() {
    currentParameterImpacts_.clear();
    
    for (const auto& param : currentConfigParameters_) {
        ParameterImpact impact;
        impact.parameterName = param.name;
        impact.currentValue = std::stod(param.value);
        impact.defaultValue = std::stod(param.defaultValue);
        
        // Calculate impacts based on parameter type and value
        calculateParameterImpact(param, impact);
        
        currentParameterImpacts_.push_back(impact);
    }
}

void ConfigVisualizer::collectDefaultValues() {
    currentDefaultValues_.clear();
    
    // Define default values for common parameters
    std::vector<std::pair<std::string, std::pair<std::string, std::string>>> defaultValues = {
        {"neuron_count", {"int64", "500"}},
        {"region_count", {"int64", "1"}},
        {"connection_probability", {"float", "0.15"}},
        {"random_seed", {"int64", "42"}},
        {"simulation_timestep", {"double", "0.001"}},
        {"working_memory_size", {"int64", "100"}},
        {"memory_consolidation_interval", {"double", "60.0"}},
        {"neuromodulation_level", {"float", "0.5"}},
        {"development_stage", {"string", "initial"}},
        {"prediction_window", {"double", "0.1"}},
        {"replay_checkpoint_mode", {"string", "periodic"}},
        {"memory_span", {"int64", "1000"}},
        {"stdp_ltp_weight", {"float", "0.02"}},
        {"stdp_ltd_weight", {"float", "0.015"}},
        {"stdp_tau", {"float", "20.0"}},
        {"synaptogenesis_rate", {"float", "0.0001"}},
        {"pruning_rate", {"float", "0.00001"}},
        {"enable_checkpointing", {"bool", "true"}},
        {"enable_replay", {"bool", "true"}},
        {"enable_development", {"bool", "true"}},
        {"debug_level", {"int", "3"}},
        {"metrics_interval", {"int", "100"}}
    };
    
    for (const auto& pair : defaultValues) {
        DefaultValueEntry entry;
        entry.parameterName = pair.first;
        entry.parameterType = pair.second.first;
        entry.defaultValue = pair.second.second;
        entry.description = getDefaultValueDescription(entry.parameterName);
        entry.category = getParameterCategory(entry.parameterName);
        
        currentDefaultValues_.push_back(entry);
    }
}

void ConfigVisualizer::updateDependencyRelationships() {
    // Build dependency graph
    std::unordered_map<std::string, std::vector<std::string>> dependencyMap;
    std::unordered_map<std::string, std::vector<std::string>> reverseDependencyMap;
    
    for (const auto& param : currentConfigParameters_) {
        // Simple dependency rules based on parameter name
        if (param.name.find("memory_") != std::string::npos) {
            dependencyMap[param.name].push_back("working_memory_size");
            reverseDependencyMap["working_memory_size"].push_back(param.name);
        }
        
        if (param.name.find("neuromodulation") != std::string::npos) {
            dependencyMap[param.name].push_back("neuromodulation_level");
            reverseDependencyMap["neuromodulation_level"].push_back(param.name);
        }
        
        if (param.name.find("development") != std::string::npos) {
            dependencyMap[param.name].push_back("development_stage");
            reverseDependencyMap["development_stage"].push_back(param.name);
        }
        
        if (param.name.find("prediction") != std::string::npos) {
            dependencyMap[param.name].push_back("prediction_window");
            reverseDependencyMap["prediction_window"].push_back(param.name);
        }
        
        if (param.name.find("stdp") != std::string::npos) {
            dependencyMap[param.name].push_back("stdp_ltp_weight");
            reverseDependencyMap["stdp_ltp_weight"].push_back(param.name);
        }
    }
    
    // Update parameter dependencies
    for (auto& param : currentConfigParameters_) {
        if (dependencyMap.find(param.name) != dependencyMap.end()) {
            param.dependencies = dependencyMap[param.name];
        }
        
        if (reverseDependencyMap.find(param.name) != reverseDependencyMap.end()) {
            param.dependents = reverseDependencyMap[param.name];
        }
    }
}

void ConfigVisualizer::calculateParameterImpact(const ConfigParameter& param, ParameterImpact& impact) {
    // Simple impact calculation based on parameter name and value
    impact.impactOnPerformance = 0.0;
    impact.impactOnStability = 0.0;
    impact.impactOnAccuracy = 0.0;
    
    // Determine impact based on parameter type
    if (param.name.find("memory_") != std::string::npos) {
        impact.impactOnPerformance = 0.3;
        impact.impactOnStability = 0.2;
        impact.impactOnAccuracy = 0.1;
        impact.affectedComponents = {"working_memory", "episodic_memory"};
        impact.impactType = "positive";
    } else if (param.name.find("neuromodulation") != std::string::npos) {
        impact.impactOnPerformance = 0.2;
        impact.impactOnStability = 0.3;
        impact.impactOnAccuracy = 0.2;
        impact.affectedComponents = {"curiosity", "novelty", "dopamine"};
        impact.impactType = "positive";
    } else if (param.name.find("development") != std::string::npos) {
        impact.impactOnPerformance = 0.1;
        impact.impactOnStability = 0.2;
        impact.impactOnAccuracy = 0.3;
        impact.affectedComponents = {"plasticity", "structural_plasticity"};
        impact.impactType = "positive";
    } else if (param.name.find("prediction") != std::string::npos) {
        impact.impactOnPerformance = 0.2;
        impact.impactOnStability = 0.1;
        impact.impactOnAccuracy = 0.4;
        impact.affectedComponents = {"prediction_system", "error_correction"};
        impact.impactType = "positive";
    } else if (param.name.find("stdp") != std::string::npos) {
        impact.impactOnPerformance = 0.2;
        impact.impactOnStability = 0.2;
        impact.impactOnAccuracy = 0.3;
        impact.affectedComponents = {"learning", "adaptation"};
        impact.impactType = "positive";
    } else if (param.name.find("neuron_count") != std::string::npos) {
        impact.impactOnPerformance = 0.4;
        impact.impactOnStability = 0.1;
        impact.impactOnAccuracy = 0.2;
        impact.affectedComponents = {"brain", "processing"};
        impact.impactType = "positive";
    } else if (param.name.find("working_memory_size") != std::string::npos) {
        impact.impactOnPerformance = 0.3;
        impact.impactOnStability = 0.2;
        impact.impactOnAccuracy = 0.2;
        impact.affectedComponents = {"memory_system", "information_processing"};
        impact.impactType = "positive";
    } else {
        impact.impactOnPerformance = 0.1;
        impact.impactOnStability = 0.1;
        impact.impactOnAccuracy = 0.1;
        impact.affectedComponents = {"general_system"};
        impact.impactType = "neutral";
    }
    
    // Adjust based on how far from default value
    double deviation = std::abs(impact.currentValue - impact.defaultValue);
    double maxDeviation = std::max(std::abs(impact.defaultValue), 1.0);
    double normalizedDeviation = deviation / maxDeviation;
    
    if (impact.impactType == "positive") {
        impact.impactOnPerformance *= (1.0 + normalizedDeviation);
        impact.impactOnStability *= (1.0 - normalizedDeviation * 0.5);
        impact.impactOnAccuracy *= (1.0 + normalizedDeviation * 0.5);
    }
}

std::string ConfigVisualizer::getDefaultValue(const std::string& name, const std::string& type) {
    // Return default value based on parameter name and type
    static std::unordered_map<std::string, std::string> defaults = {
        {"neuron_count", "500"},
        {"region_count", "1"},
        {"connection_probability", "0.15"},
        {"random_seed", "42"},
        {"simulation_timestep", "0.001"},
        {"working_memory_size", "100"},
        {"memory_consolidation_interval", "60.0"},
        {"neuromodulation_level", "0.5"},
        {"development_stage", "initial"},
        {"prediction_window", "0.1"},
        {"replay_checkpoint_mode", "periodic"},
        {"memory_span", "1000"},
        {"stdp_ltp_weight", "0.02"},
        {"stdp_ltd_weight", "0.015"},
        {"stdp_tau", "20.0"},
        {"synaptogenesis_rate", "0.0001"},
        {"pruning_rate", "0.00001"},
        {"enable_checkpointing", "true"},
        {"enable_replay", "true"},
        {"enable_development", "true"},
        {"debug_level", "3"},
        {"metrics_interval", "100"}
    };
    
    auto it = defaults.find(name);
    if (it != defaults.end()) {
        return it->second;
    }
    
    // Default fallback based on type
    if (type == "int64" || type == "int") return "0";
    if (type == "double" || type == "float") return "0.0";
    if (type == "bool") return "false";
    if (type == "string") return "";
    
    return "";
}

std::string ConfigVisualizer::getConfigSource(const std::string& name) {
    // Determine source based on parameter name
    if (name.find("command_line") != std::string::npos) {
        return "command_line";
    } else if (name.find("config_file") != std::string::npos) {
        return "config_file";
    } else if (name.find("runtime") != std::string::npos) {
        return "runtime";
    } else {
        return "config_file";
    }
}

std::string ConfigVisualizer::getParameterDescription(const std::string& name) {
    // Return description based on parameter name
    static std::unordered_map<std::string, std::string> descriptions = {
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of neural regions"},
        {"connection_probability", "Probability of creating connections between neurons"},
        {"random_seed", "Seed for random number generation"},
        {"simulation_timestep", "Time step for simulation updates"},
        {"working_memory_size", "Capacity of working memory in items"},
        {"memory_consolidation_interval", "Interval for memory consolidation in seconds"},
        {"neuromodulation_level", "Level of neuromodulation (0.0-1.0)"},
        {"development_stage", "Current developmental stage"},
        {"prediction_window", "Time window for prediction system"},
        {"replay_checkpoint_mode", "Mode for replay/checkpoint functionality"},
        {"memory_span", "Memory span in simulation steps"},
        {"stdp_ltp_weight", "Weight for long-term potentiation"},
        {"stdp_ltd_weight", "Weight for long-term depression"},
        {"stdp_tau", "Time constant for STDP"},
        {"synaptogenesis_rate", "Rate of new synapse formation"},
        {"pruning_rate", "Rate of synapse pruning"},
        {"enable_checkpointing", "Enable checkpoint functionality"},
        {"enable_replay", "Enable replay functionality"},
        {"enable_development", "Enable developmental processes"},
        {"debug_level", "Debug logging level (0-5)"},
        {"metrics_interval", "Interval for metrics collection in steps"}
    };
    
    auto it = descriptions.find(name);
    if (it != descriptions.end()) {
        return it->second;
    }
    
    return "Configuration parameter";
}

std::string ConfigVisualizer::getDefaultValueDescription(const std::string& name) {
    // Return default value description based on parameter name
    static std::unordered_map<std::string, std::string> descriptions = {
        {"neuron_count", "Number of neurons to initialize in the brain"},
        {"region_count", "Number of neural regions to create"},
        {"connection_probability", "Base probability for neural connections"},
        {"random_seed", "Seed for reproducible random number generation"},
        {"simulation_timestep", "Time step size for simulation updates"},
        {"working_memory_size", "Default working memory capacity"},
        {"memory_consolidation_interval", "Default memory consolidation interval"},
        {"neuromodulation_level", "Default neuromodulation level (0=off, 1=on)"},
        {"development_stage", "Starting developmental stage"},
        {"prediction_window", "Default prediction time window"},
        {"replay_checkpoint_mode", "Default checkpoint/replay mode"},
        {"memory_span", "Default memory span duration"},
        {"stdp_ltp_weight", "Default LTP weight for STDP"},
        {"stdp_ltd_weight", "Default LTD weight for STDP"},
        {"stdp_tau", "Default STDP time constant"},
        {"synaptogenesis_rate", "Default synaptogenesis rate"},
        {"pruning_rate", "Default pruning rate"},
        {"enable_checkpointing", "Enable checkpointing by default"},
        {"enable_replay", "Enable replay by default"},
        {"enable_development", "Enable development by default"},
        {"debug_level", "Default debug logging level"},
        {"metrics_interval", "Default metrics collection interval"}
    };
    
    auto it = descriptions.find(name);
    if (it != descriptions.end()) {
        return it->second;
    }
    
    return "Default configuration value";
}

std::string ConfigVisualizer::getParameterCategory(const std::string& name) {
    // Determine category based on parameter name
    if (name.find("memory_") != std::string::npos || 
        name.find("neuron_") != std::string::npos ||
        name.find("working_memory") != std::string::npos) {
        return "performance";
    } else if (name.find("neuromodulation") != std::string::npos) {
        return "behavior";
    } else if (name.find("development") != std::string::npos ||
               name.find("stdp") != std::string::npos) {
        return "stability";
    } else if (name.find("prediction") != std::string::npos) {
        return "accuracy";
    } else {
        return "general";
    }
}

void ConfigVisualizer::renderTreeView(const std::vector<ConfigParameter>& parameters, const ConfigOptions& options) const {
    std::cout << "Configuration Tree View:" << std::endl;
    
    // Group parameters by category
    std::unordered_map<std::string, std::vector<ConfigParameter>> grouped;
    
    for (const auto& param : parameters) {
        std::string category = getParameterCategory(param.name);
        grouped[category].push_back(param);
    }
    
    for (const auto& group : grouped) {
        std::cout << "\n" << group.first << ":" << std::endl;
        
        for (const auto& param : group.second) {
            std::string sourceColor = getSourceColor(param.source);
            std::string overrideMarker = param.isOverridden ? " [OVERRIDDEN]" : "";
            
            std::cout << "  " << sourceColor << param.name << ": " << param.value << 
                      " (source: " << param.source << ")" << overrideMarker << std::endl;
            
            if (options.showDetailedInfo && !param.description.empty()) {
                std::cout << "    " << param.description << std::endl;
            }
            
            if (options.showDependencies && !param.dependencies.empty()) {
                std::cout << "    Dependencies: ";
                for (size_t i = 0; i < param.dependencies.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << param.dependencies[i];
                }
                std::cout << std::endl;
            }
        }
    }
}

void ConfigVisualizer::renderFlatView(const std::vector<ConfigParameter>& parameters, const ConfigOptions& options) const {
    std::cout << "Configuration Flat View:" << std::endl;
    
    // Sort by name for consistent ordering
    std::vector<ConfigParameter> sortedParams = parameters;
    std::sort(sortedParams.begin(), sortedParams.end(),
              [](const ConfigParameter& a, const ConfigParameter& b) {
                  return a.name < b.name;
              });
    
    std::cout << std::left << std::setw(30) << "Parameter" << std::setw(12) << "Type" 
              << std::setw(12) << "Value" << std::setw(15) << "Source" 
              << std::setw(10) << "Overridden" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    for (const auto& param : sortedParams) {
        std::string type = param.type;
        std::string value = param.value;
        std::string source = param.source;
        std::string overridden = param.isOverridden ? "Yes" : "No";
        
        // Truncate long values
        if (value.length() > 20) {
            value = value.substr(0, 17) + "...";
        }
        
        std::cout << std::left << std::setw(30) << param.name 
                  << std::setw(12) << type 
                  << std::setw(12) << value 
                  << std::setw(15) << source 
                  << std::setw(10) << overridden << std::endl;
        
        if (options.showDetailedInfo && !param.description.empty()) {
            std::cout << "  " << param.description << std::endl;
        }
    }
}

void ConfigVisualizer::renderGroupedView(const std::vector<ConfigParameter>& parameters, const ConfigOptions& options) const {
    std::cout << "Configuration Grouped View:" << std::endl;
    
    // Group by source
    std::unordered_map<std::string, std::vector<ConfigParameter>> sourceGroups;
    
    for (const auto& param : parameters) {
        sourceGroups[param.source].push_back(param);
    }
    
    for (const auto& group : sourceGroups) {
        std::cout << "\nBy Source - " << group.first << ":" << std::endl;
        
        for (const auto& param : group.second) {
            std::cout << "  " << param.name << ": " << param.value << std::endl;
        }
    }
    
    // Group by type
    std::cout << "\nBy Type:" << std::endl;
    std::unordered_map<std::string, std::vector<ConfigParameter>> typeGroups;
    
    for (const auto& param : parameters) {
        typeGroups[param.type].push_back(param);
    }
    
    for (const auto& group : typeGroups) {
        std::cout << "  " << group.first << " (" << group.second.size() << "):" << std::endl;
        for (const auto& param : group.second) {
            std::cout << "    " << param.name << ": " << param.value << std::endl;
        }
    }
}

void ConfigVisualizer::renderInheritanceNode(const InheritanceNode& node, int level) const {
    std::string indent(level * 2, ' ');
    
    std::string nodeColor = node.isOverridden ? "🔴" : "🟢";
    
    std::cout << indent << nodeColor << " " << node.name 
              << " (level " << node.inheritanceLevel << ")";
    
    if (!node.value.empty() && node.value != node.name) {
        std::cout << " = " << node.value;
    }
    
    std::cout << " [source: " << node.source << "]";
    
    if (node.isOverridden) {
        std::cout << " [OVERRIDDEN]";
    }
    
    std::cout << std::endl;
    
    for (const auto& child : node.children) {
        renderInheritanceNode(child, level + 1);
    }
}

void ConfigVisualizer::renderDependencyNode(const std::string& name,
                                           const std::unordered_map<std::string, std::vector<std::string>>& reverseGraph,
                                           const std::unordered_map<std::string, std::vector<std::string>>& graph,
                                           int level) const {
    std::string indent(level * 2, ' ');
    
    // Find parameter info
    const ConfigParameter* param = nullptr;
    for (const auto& p : currentConfigParameters_) {
        if (p.name == name) {
            param = &p;
            break;
        }
    }
    
    std::cout << indent;
    
    if (param) {
        std::cout << "● " << param->name << " (" << param->type << ")";
        
        if (param->isOverridden) {
            std::cout << " [OVERRIDDEN]";
        }
        
        std::cout << " = " << param->value;
        
        if (!param->dependencies.empty()) {
            std::cout << " <-- depends on: ";
            for (size_t i = 0; i < param->dependencies.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << param->dependencies[i];
            }
        }
    } else {
        std::cout << name << " (unknown parameter)";
    }
    
    std::cout << std::endl;
    
    // Recursively render dependents
    if (graph.find(name) != graph.end()) {
        for (const auto& dependent : graph.at(name)) {
            renderDependencyNode(dependent, reverseGraph, graph, level + 1);
        }
    }
}

int ConfigVisualizer::countOverriddenParameters() const {
    int count = 0;
    for (const auto& param : currentConfigParameters_) {
        if (param.isOverridden) {
            count++;
        }
    }
    return count;
}

std::set<std::string> ConfigVisualizer::getUniqueCategories() const {
    std::set<std::string> categories;
    for (const auto& param : currentConfigParameters_) {
        categories.insert(getParameterCategory(param.name));
    }
    return categories;
}

void ConfigVisualizer::renderInheritanceToFile(std::ofstream& file, const InheritanceNode& node) const {
    file << "  " << std::string(node.inheritanceLevel * 2, ' ') << "├─ " << node.name;
    
    if (!node.value.empty() && node.value != node.name) {
        file << " = " << node.value;
    }
    
    file << " [level: " << node.inheritanceLevel << "]";
    
    if (node.isOverridden) {
        file << " [OVERRIDDEN]";
    }
    
    file << " [source: " << node.source << "]" << std::endl;
    
    for (size_t i = 0; i < node.children.size(); ++i) {
        if (i == node.children.size() - 1) {
            file << "  " << std::string((node.inheritanceLevel + 1) * 2, ' ') << "└─ ";
        } else {
            file << "  " << std::string((node.inheritanceLevel + 1) * 2, ' ') << "├─ ";
        }
        
        renderInheritanceToFile(file, node.children[i]);
    }
}

std::string ConfigVisualizer::getImpactColor(double impact) const {
    if (impact > 0.7) return "🔴";
    else if (impact > 0.4) return "🟠";
    else if (impact > 0.2) return "🟡";
    else return "🟢";
}

} // namespace nlm

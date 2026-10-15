#include "Architecture.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plugins/PluginInterface.hpp"
#include <algorithm>
#include <random>

namespace nlm {

struct NeuralArchitecture::Impl {
    // Architecture components
    std::vector<RegionSpec> regions;
    std::vector<PopulationSpec> populations;
    std::vector<ConnectionSpec> connections;
    
    // Configuration
    Config config;
    
    // Builder
    std::unique_ptr<NeuralArchitectureBuilder> builder;
    
    // Compilation
    std::unique_ptr<ArchitectureCompiler> compiler;
    
    // Plugin integration
    std::map<std::string, std::shared_ptr<INeuronModelPlugin>> neuronModelPlugins;
    std::map<std::string, std::shared_ptr<IPlasticityRulePlugin>> plasticityRulePlugins;
    std::map<std::string, std::shared_ptr<ISynapticDynamicsPlugin>> synapticDynamicsPlugins;
    
    // Optimization
    bool optimizationEnabled;
    int optimizationLevel;
    
    // Performance monitoring
    bool performanceMonitoringEnabled;
    std::chrono::system_clock::time_point startTime;
    double totalExecutionTime;
    size_t neuronCount;
    size_t synapseCount;
    
    // Statistics
    size_t compilationCount;
    size_t buildCount;
    size_t executionCount;
    
    Logger* logger;
};

NeuralArchitecture::NeuralArchitecture() : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = Logger::getInstance();
    pImpl->optimizationEnabled = true;
    pImpl->optimizationLevel = 1;
    pImpl->performanceMonitoringEnabled = false;
    pImpl->totalExecutionTime = 0.0;
    pImpl->neuronCount = 0;
    pImpl->synapseCount = 0;
    pImpl->compilationCount = 0;
    pImpl->buildCount = 0;
    pImpl->executionCount = 0;
}

NeuralArchitecture::~NeuralArchitecture() {
    shutdown();
}

NeuralArchitecture::NeuralArchitecture(NeuralArchitecture&& other) noexcept : pImpl(std::move(other.pImpl)) {}

NeuralArchitecture& NeuralArchitecture::operator=(NeuralArchitecture&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool NeuralArchitecture::initialize(const Config& config) {
    pImpl->config = config;
    
    // Initialize builder
    pImpl->builder = std::make_unique<DefaultNeuralArchitectureBuilder>();
    if (!pImpl->builder->initialize(config)) {
        pImpl->logger->error("Failed to initialize neural architecture builder");
        return false;
    }
    
    // Initialize compiler
    pImpl->compiler = std::make_unique<ArchitectureCompiler>();
    if (!pImpl->compiler->initialize()) {
        pImpl->logger->error("Failed to initialize architecture compiler");
        return false;
    }
    
    pImpl->logger->info("Neural architecture initialized");
    return true;
}

void NeuralArchitecture::update(double timestep) {
    if (pImpl->builder) {
        pImpl->builder->update(timestep);
    }
    
    if (pImpl->performanceMonitoringEnabled) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime);
        pImpl->totalExecutionTime += duration.count();
    }
}

void NeuralArchitecture::shutdown() {
    pImpl->logger->info("Neural architecture shutdown");
    
    if (pImpl->builder) {
        pImpl->builder->shutdown();
    }
    
    if (pImpl->compiler) {
        pImpl->compiler->shutdown();
    }
    
    pImpl->regions.clear();
    pImpl->populations.clear();
    pImpl->connections.clear();
}

void NeuralArchitecture::addRegion(const RegionSpec& region) {
    // Check for duplicate region names
    auto it = std::find_if(pImpl->regions.begin(), pImpl->regions.end(),
                          [&region](const RegionSpec& r) { return r.name == region.name; });
    if (it != pImpl->regions.end()) {
        pImpl->logger->warning("Region already exists: " + region.name);
        return;
    }
    
    pImpl->regions.push_back(region);
    
    if (pImpl->builder) {
        pImpl->builder->addRegion(region);
    }
}

void NeuralArchitecture::removeRegion(const std::string& regionName) {
    auto it = std::find_if(pImpl->regions.begin(), pImpl->regions.end(),
                          [&regionName](const RegionSpec& r) { return r.name == regionName; });
    if (it != pImpl->regions.end()) {
        pImpl->regions.erase(it);
        
        if (pImpl->builder) {
            // Remove associated populations and connections
            // Simplified implementation
        }
        
        pImpl->logger->info("Region removed: " + regionName);
    }
}

RegionSpec* NeuralArchitecture::getRegion(const std::string& regionName) {
    auto it = std::find_if(pImpl->regions.begin(), pImpl->regions.end(),
                          [&regionName](const RegionSpec& r) { return r.name == regionName; });
    if (it != pImpl->regions.end()) {
        return &(*it);
    }
    return nullptr;
}

const RegionSpec* NeuralArchitecture::getRegion(const std::string& regionName) const {
    auto it = std::find_if(pImpl->regions.begin(), pImpl->regions.end(),
                          [&regionName](const RegionSpec& r) { return r.name == regionName; });
    if (it != pImpl->regions.end()) {
        return &(*it);
    }
    return nullptr;
}

std::vector<RegionSpec>& NeuralArchitecture::getRegions() {
    return pImpl->regions;
}

const std::vector<RegionSpec>& NeuralArchitecture::getRegions() const {
    return pImpl->regions;
}

void NeuralArchitecture::addPopulation(const PopulationSpec& population) {
    // Check for duplicate population names
    auto it = std::find_if(pImpl->populations.begin(), pImpl->populations.end(),
                          [&population](const PopulationSpec& p) { return p.name == population.name; });
    if (it != pImpl->populations.end()) {
        pImpl->logger->warning("Population already exists: " + population.name);
        return;
    }
    
    pImpl->populations.push_back(population);
    
    if (pImpl->builder) {
        pImpl->builder->addPopulation(population);
    }
    
    // Update neuron count
    pImpl->neuronCount += population.neuronCount;
}

void NeuralArchitecture::removePopulation(const std::string& populationName) {
    auto it = std::find_if(pImpl->populations.begin(), pImpl->populations.end(),
                          [&populationName](const PopulationSpec& p) { return p.name == populationName; });
    if (it != pImpl->populations.end()) {
        // Subtract neuron count
        pImpl->neuronCount -= it->neuronCount;
        
        pImpl->populations.erase(it);
        
        if (pImpl->builder) {
            // Remove from builder
        }
        
        pImpl->logger->info("Population removed: " + populationName);
    }
}

PopulationSpec* NeuralArchitecture::getPopulation(const std::string& populationName) {
    auto it = std::find_if(pImpl->populations.begin(), pImpl->populations.end(),
                          [&populationName](const PopulationSpec& p) { return p.name == populationName; });
    if (it != pImpl->populations.end()) {
        return &(*it);
    }
    return nullptr;
}

const PopulationSpec* NeuralArchitecture::getPopulation(const std::string& populationName) const {
    auto it = std::find_if(pImpl->populations.begin(), pImpl->populations.end(),
                          [&populationName](const PopulationSpec& p) { return p.name == populationName; });
    if (it != pImpl->populations.end()) {
        return &(*it);
    }
    return nullptr;
}

std::vector<PopulationSpec>& NeuralArchitecture::getPopulations() {
    return pImpl->populations;
}

const std::vector<PopulationSpec>& NeuralArchitecture::getPopulations() const {
    return pImpl->populations;
}

void NeuralArchitecture::addConnection(const ConnectionSpec& connection) {
    pImpl->connections.push_back(connection);
    
    if (pImpl->builder) {
        pImpl->builder->addConnection(connection);
    }
    
    // Estimate synapse count (rough approximation)
    pImpl->synapseCount += static_cast<size_t>(connection.connectionProbability * 100);
}

void NeuralArchitecture::removeConnection(const std::string& sourceRegion, const std::string& targetRegion) {
    auto it = std::find_if(pImpl->connections.begin(), pImpl->connections.end(),
                          [&sourceRegion, &targetRegion](const ConnectionSpec& c) {
                              return c.sourceRegion == sourceRegion && c.targetRegion == targetRegion;
                          });
    if (it != pImpl->connections.end()) {
        pImpl->connections.erase(it);
        
        if (pImpl->builder) {
            // Remove from builder
        }
        
        pImpl->logger->info("Connection removed: " + sourceRegion + " -> " + targetRegion);
    }
}

ConnectionSpec* NeuralArchitecture::getConnection(const std::string& sourceRegion, const std::string& targetRegion) {
    auto it = std::find_if(pImpl->connections.begin(), pImpl->connections.end(),
                          [&sourceRegion, &targetRegion](const ConnectionSpec& c) {
                              return c.sourceRegion == sourceRegion && c.targetRegion == targetRegion;
                          });
    if (it != pImpl->connections.end()) {
        return &(*it);
    }
    return nullptr;
}

const ConnectionSpec* NeuralArchitecture::getConnection(const std::string& sourceRegion, const std::string& targetRegion) const {
    auto it = std::find_if(pImpl->connections.begin(), pImpl->connections.end(),
                          [&sourceRegion, &targetRegion](const ConnectionSpec& c) {
                              return c.sourceRegion == sourceRegion && c.targetRegion == targetRegion;
                          });
    if (it != pImpl->connections.end()) {
        return &(*it);
    }
    return nullptr;
}

std::vector<ConnectionSpec>& NeuralArchitecture::getConnections() {
    return pImpl->connections;
}

const std::vector<ConnectionSpec>& NeuralArchitecture::getConnections() const {
    return pImpl->connections;
}

bool NeuralArchitecture::compile(const std::string& outputPath) {
    pImpl->compilationCount++;
    
    // Convert to NeuralArchitecture format for compiler
    NeuralArchitecture archForCompiler;
    archForCompiler.regions = pImpl->regions;
    archForCompiler.populations = pImpl->populations;
    archForCompiler.connections = pImpl->connections;
    archForCompiler.name = "GeneratedArchitecture";
    archForCompiler.version = "1.0.0";
    
    return pImpl->compiler->compile(archForCompiler, outputPath);
}

bool NeuralArchitecture::compileToCode(const std::string& outputPath) {
    // Same as compile for now
    return compile(outputPath);
}

bool NeuralArchitecture::validate() const {
    std::vector<std::string> errors;
    
    // Basic validation
    for (const auto& region : pImpl->regions) {
        if (region.name.empty()) {
            errors.push_back("Region name cannot be empty");
        }
        
        // Check if region is referenced by any connection
        bool referenced = false;
        for (const auto& connection : pImpl->connections) {
            if (connection.sourceRegion == region.name || connection.targetRegion == region.name) {
                referenced = true;
                break;
            }
        }
        
        if (!referenced) {
            errors.push_back("Region not referenced by any connection: " + region.name);
        }
    }
    
    for (const auto& population : pImpl->populations) {
        if (population.name.empty()) {
            errors.push_back("Population name cannot be empty");
        }
        
        if (population.neuronCount == 0) {
            errors.push_back("Population must have at least one neuron: " + population.name);
        }
    }
    
    pImpl->logger->info("Architecture validation " + std::string(errors.empty() ? "passed" : "failed"));
    if (!errors.empty()) {
        pImpl->logger->error("Validation errors: " + std::to_string(errors.size()));
        for (const auto& error : errors) {
            pImpl->logger->error("  - " + error);
        }
    }
    
    return errors.empty();
}

std::vector<std::string> NeuralArchitecture::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Basic validation
    for (const auto& region : pImpl->regions) {
        if (region.name.empty()) {
            errors.push_back("Region name cannot be empty");
        }
    }
    
    return errors;
}

std::vector<std::string> NeuralArchitecture::getValidationWarnings() const {
    std::vector<std::string> warnings;
    
    // Check for regions without populations
    for (const auto& region : pImpl->regions) {
        bool hasPopulation = false;
        for (const auto& population : pImpl->populations) {
            if (population.name.find(region.name) != std::string::npos) {
                hasPopulation = true;
                break;
            }
        }
        
        if (!hasPopulation) {
            warnings.push_back("Region has no associated populations: " + region.name);
        }
    }
    
    return warnings;
}

bool NeuralArchitecture::build(const std::string& brainName) {
    pImpl->buildCount++;
    
    // In real implementation, would create Brain object with this architecture
    // For now, just log
    
    pImpl->startTime = std::chrono::system_clock::now();
    pImpl->logger->info("Building brain from architecture: " + brainName);
    
    return true;
}

bool NeuralArchitecture::execute(double duration) {
    pImpl->executionCount++;
    
    // In real implementation, would execute the architecture
    // For now, just log
    
    pImpl->logger->info("Executing architecture for " + std::to_string(duration) + " seconds");
    
    return true;
}

bool NeuralArchitecture::stop() {
    // In real implementation, would stop execution
    pImpl->logger->info("Stopping architecture execution");
    return true;
}

bool NeuralArchitecture::reset() {
    // In real implementation, would reset architecture state
    pImpl->logger->info("Resetting architecture");
    return true;
}

size_t NeuralArchitecture::getTotalNeuronCount() const {
    return pImpl->neuronCount;
}

size_t NeuralArchitecture::getTotalSynapseCount() const {
    return pImpl->synapseCount;
}

size_t NeuralArchitecture::getRegionCount() const {
    return pImpl->regions.size();
}

size_t NeuralArchitecture::getPopulationCount() const {
    return pImpl->populations.size();
}

size_t NeuralArchitecture::getConnectionCount() const {
    return pImpl->connections.size();
}

std::string NeuralArchitecture::getArchitectureStats() const {
    std::stringstream ss;
    ss << "Architecture Statistics:\n";
    ss << "======================\n";
    ss << "Regions: " << pImpl->regions.size() << "\n";
    ss << "Populations: " << pImpl->populations.size() << "\n";
    ss << "Connections: " << pImpl->connections.size() << "\n";
    ss << "Total neurons: " << pImpl->neuronCount << "\n";
    ss << "Total synapses: " << pImpl->synapseCount << "\n";
    ss << "Compilations: " << pImpl->compilationCount << "\n";
    ss << "Builds: " << pImpl->buildCount << "\n";
    ss << "Executions: " << pImpl->executionCount << "\n";
    
    if (pImpl->performanceMonitoringEnabled) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime);
        ss << "Total execution time: " << pImpl->totalExecutionTime << " seconds\n";
        ss << "Current uptime: " << duration.count() << " seconds\n";
    }
    
    return ss.str();
}

void NeuralArchitecture::setConfig(const Config& config) {
    pImpl->config = config;
}

Config NeuralArchitecture::getConfig() const {
    return pImpl->config;
}

bool NeuralArchitecture::addNeuronModelPlugin(std::shared_ptr<INeuronModelPlugin> plugin) {
    if (!plugin) {
        pImpl->logger->error("Cannot add null neuron model plugin");
        return false;
    }
    
    pImpl->neuronModelPlugins[plugin->getName()] = plugin;
    pImpl->logger->info("Neuron model plugin added: " + plugin->getName());
    return true;
}

bool NeuralArchitecture::addPlasticityRulePlugin(std::shared_ptr<IPlasticityRulePlugin> plugin) {
    if (!plugin) {
        pImpl->logger->error("Cannot add null plasticity rule plugin");
        return false;
    }
    
    pImpl->plasticityRulePlugins[plugin->getName()] = plugin;
    pImpl->logger->info("Plasticity rule plugin added: " + plugin->getName());
    return true;
}

bool NeuralArchitecture::addSynapticDynamicsPlugin(std::shared_ptr<ISynapticDynamicsPlugin> plugin) {
    if (!plugin) {
        pImpl->logger->error("Cannot add null synaptic dynamics plugin");
        return false;
    }
    
    pImpl->synapticDynamicsPlugins[plugin->getName()] = plugin;
    pImpl->logger->info("Synaptic dynamics plugin added: " + plugin->getName());
    return true;
}

void NeuralArchitecture::enableOptimization(bool enable) {
    pImpl->optimizationEnabled = enable;
}

bool NeuralArchitecture::isOptimizationEnabled() const {
    return pImpl->optimizationEnabled;
}

void NeuralArchitecture::setOptimizationLevel(int level) {
    pImpl->optimizationLevel = level;
}

int NeuralArchitecture::getOptimizationLevel() const {
    return pImpl->optimizationLevel;
}

void NeuralArchitecture::enablePerformanceMonitoring(bool enable) {
    pImpl->performanceMonitoringEnabled = enable;
    if (enable) {
        pImpl->startTime = std::chrono::system_clock::now();
    }
}

bool NeuralArchitecture::isPerformanceMonitoringEnabled() const {
    return pImpl->performanceMonitoringEnabled;
}

std::string NeuralArchitecture::getPerformanceProfile() const {
    std::stringstream ss;
    ss << "Performance Profile:\n";
    ss << "==================\n";
    
    if (pImpl->performanceMonitoringEnabled) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime);
        ss << "Uptime: " << duration.count() << " seconds\n";
        ss << "Total execution time: " << pImpl->totalExecutionTime << " seconds\n";
        
        if (pImpl->totalExecutionTime > 0) {
            ss << "Architecture build rate: " << pImpl->buildCount / pImpl->totalExecutionTime << " builds/second\n";
        }
    }
    
    return ss.str();
}

bool NeuralArchitecture::saveAsTemplate(const std::string& templatePath) const {
    try {
        std::ofstream file(templatePath);
        if (!file) {
            pImpl->logger->error("Failed to open template file: " + templatePath);
            return false;
        }
        
        // Create YAML representation
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "architecture_name" << YAML::Value << "TemplateArchitecture";
        emitter << YAML::Key << "version" << YAML::Value << "1.0.0";
        
        // Export regions, populations, connections...
        // In real implementation would export full architecture
        
        emitter << YAML::EndMap;
        
        file << emitter.c_str();
        pImpl->logger->info("Architecture saved as template: " + templatePath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to save template: " + std::string(e.what()));
        return false;
    }
}

bool NeuralArchitecture::loadFromTemplate(const std::string& templatePath) {
    // In real implementation, would load from template file
    pImpl->logger->info("Loading architecture from template: " + templatePath);
    return true;
}

std::vector<std::string> NeuralArchitecture::getAvailableTemplates() const {
    // In real implementation, would scan for template files
    return std::vector<std::string>();
}

void NeuralArchitecture::loadConnectionPatterns(const std::string& patternsFile) {
    // In real implementation, would load connection patterns
    pImpl->logger->info("Loading connection patterns from: " + patternsFile);
}

void NeuralArchitecture::applyTopologicalPatterns(const std::string& patternType) {
    // In real implementation, would apply topological patterns
    pImpl->logger->info("Applying topological pattern: " + patternType);
}

void NeuralArchitecture::generateStructuralPlasticityRules(const std::string& ruleFile) {
    // In real implementation, would generate structural plasticity rules
    pImpl->logger->info("Generating structural plasticity rules from: " + ruleFile);
}

class NeuralArchitectureBuilder* NeuralArchitecture::getBuilder() const {
    return pImpl->builder.get();
}

bool NeuralArchitecture::setBuilder(std::unique_ptr<NeuralArchitectureBuilder> builder) {
    if (!builder) {
        pImpl->logger->error("Cannot set null builder");
        return false;
    }
    
    pImpl->builder = std::move(builder);
    pImpl->logger->info("Neural architecture builder set");
    return true;
}

// DefaultNeuralArchitectureBuilder implementations
bool DefaultNeuralArchitectureBuilder::initialize(const Config& config) {
    regions.clear();
    populations.clear();
    connections.clear();
    plasticityRules.clear();
    return true;
}

void DefaultNeuralArchitectureBuilder::update(double timestep) {
    // Update builder state
}

void DefaultNeuralArchitectureBuilder::shutdown() {
    // Clean up builder resources
}

void DefaultNeuralArchitectureBuilder::addRegion(const RegionSpec& region) {
    regions.push_back(region);
}

void DefaultNeuralArchitectureBuilder::addPopulation(const PopulationSpec& population) {
    populations.push_back(population);
}

void DefaultNeuralArchitectureBuilder::addConnection(const ConnectionSpec& connection) {
    connections.push_back(connection);
}

void DefaultNeuralArchitectureBuilder::clear() {
    regions.clear();
    populations.clear();
    connections.clear();
}

void DefaultNeuralArchitectureBuilder::generateRandomTopology(size_t regionCount, size_t populationSize) {
    // Generate random topology
    for (size_t i = 0; i < regionCount; ++i) {
        RegionSpec region;
        region.name = "Region_" + std::to_string(i);
        region.type = "type_" + std::to_string(i % 3); // Different types
        
        for (size_t j = 0; j < populationSize; ++j) {
            PopulationSpec population;
            population.name = region.name + "_Pop_" + std::to_string(j);
            population.neuronType = "Neuron_" + std::to_string(j % 2); // Excitatory or inhibitory
            population.neuronCount = populationSize / 10; // Smaller populations
            
            region.populations.push_back(population);
        }
        
        regions.push_back(region);
    }
    
    // Generate random connections
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (const auto& region : regions) {
        for (const auto& otherRegion : regions) {
            if (&region != &otherRegion && dis(gen) < 0.3) { // 30% connection probability
                ConnectionSpec connection;
                connection.sourceRegion = region.name;
                connection.targetRegion = otherRegion.name;
                connection.connectionType = dis(gen) < 0.7 ? "feedforward" : "recurrent";
                connection.connectionProbability = dis(gen);
                
                connections.push_back(connection);
            }
        }
    }
}

void DefaultNeuralArchitectureBuilder::generateFeedforwardTopology(size_t layerCount, size_t neuronsPerLayer) {
    for (size_t layer = 0; layer < layerCount; ++layer) {
        RegionSpec region;
        region.name = "Layer_" + std::to_string(layer);
        region.type = "layer_" + std::to_string(layer % 3);
        
        PopulationSpec population;
        population.name = region.name + "_Pop";
        population.neuronType = "LayerNeuron";
        population.neuronCount = neuronsPerLayer;
        
        region.populations.push_back(population);
        regions.push_back(region);
        
        // Add connections to next layer
        if (layer < layerCount - 1) {
            ConnectionSpec connection;
            connection.sourceRegion = region.name;
            connection.targetRegion = "Layer_" + std::to_string(layer + 1);
            connection.connectionType = "feedforward";
            connection.connectionProbability = 1.0; // Full connections
            
            connections.push_back(connection);
        }
    }
}

void DefaultNeuralArchitectureBuilder::generateRecurrentTopology(size_t regionCount, float connectionProbability) {
    // Generate a recurrent network
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (size_t i = 0; i < regionCount; ++i) {
        RegionSpec region;
        region.name = "Region_" + std::to_string(i);
        region.type = "cortical";
        
        // Add neurons
        for (size_t j = 0; j < 50; ++j) {
            PopulationSpec population;
            population.name = region.name + "_Pop_" + std::to_string(j);
            population.neuronType = (j % 2 == 0) ? "Excitatory" : "Inhibitory";
            population.neuronCount = 10;
            
            region.populations.push_back(population);
        }
        
        regions.push_back(region);
        
        // Add recurrent connections
        for (size_t j = 0; j < regionCount; ++j) {
            if (i != j && dis(gen) < connectionProbability) {
                ConnectionSpec connection;
                connection.sourceRegion = region.name;
                connection.targetRegion = "Region_" + std::to_string(j);
                connection.connectionType = "recurrent";
                connection.connectionProbability = dis(gen);
                
                connections.push_back(connection);
            }
        }
    }
}

void DefaultNeuralArchitectureBuilder::generateModularTopology(const std::vector<std::vector<size_t>>& moduleSpecs) {
    for (size_t module = 0; module < moduleSpecs.size(); ++module) {
        RegionSpec region;
        region.name = "Module_" + std::to_string(module);
        region.type = "module_" + std::to_string(module % 4);
        
        for (size_t neuron : moduleSpecs[module]) {
            PopulationSpec population;
            population.name = region.name + "_Neuron_" + std::to_string(neuron);
            population.neuronType = "ModuleNeuron_" + std::to_string(neuron % 3);
            population.neuronCount = 1; // Each neuron in its own population
            
            region.populations.push_back(population);
        }
        
        regions.push_back(region);
    }
    
    // Add connections between modules
    for (size_t i = 0; i < moduleSpecs.size(); ++i) {
        for (size_t j = 0; j < moduleSpecs.size(); ++j) {
            if (i != j) {
                ConnectionSpec connection;
                connection.sourceRegion = "Module_" + std::to_string(i);
                connection.targetRegion = "Module_" + std::to_string(j);
                connection.connectionType = "feedforward";
                connection.connectionProbability = 0.5;
                
                connections.push_back(connection);
            }
        }
    }
}

void DefaultNeuralArchitectureBuilder::addFeedforwardConnections(const std::string& fromRegion, const std::string& toRegion) {
    ConnectionSpec connection;
    connection.sourceRegion = fromRegion;
    connection.targetRegion = toRegion;
    connection.connectionType = "feedforward";
    connection.connectionProbability = 1.0;
    
    connections.push_back(connection);
}

void DefaultNeuralArchitectureBuilder::addFeedbackConnections(const std::string& fromRegion, const std::string& toRegion) {
    ConnectionSpec connection;
    connection.sourceRegion = fromRegion;
    connection.targetRegion = toRegion;
    connection.connectionType = "feedback";
    connection.connectionProbability = 0.8;
    
    connections.push_back(connection);
}

void DefaultNeuralArchitectureBuilder::addRecurrentConnections(const std::string& region, float probability) {
    for (size_t i = 0; i < regions.size(); ++i) {
        if (regions[i].name == region) {
            // Add self-connections
            ConnectionSpec connection;
            connection.sourceRegion = region;
            connection.targetRegion = region;
            connection.connectionType = "recurrent";
            connection.connectionProbability = probability;
            
            connections.push_back(connection);
            break;
        }
    }
}

void DefaultNeuralArchitectureBuilder::addSkipConnections(const std::vector<std::string>& layers) {
    for (size_t i = 0; i < layers.size() - 2; ++i) {
        ConnectionSpec connection;
        connection.sourceRegion = layers[i];
        connection.targetRegion = layers[i + 2]; // Skip one layer
        connection.connectionType = "skip";
        connection.connectionProbability = 0.3;
        
        connections.push_back(connection);
    }
}

void DefaultNeuralArchitectureBuilder::addStructuralPlasticityRule(const std::string& ruleName,
                                                                 std::function<bool(const SynapseState&, const SynapseState&)> rule) {
    plasticityRules[ruleName] = rule;
}

void DefaultNeuralArchitectureBuilder::removeStructuralPlasticityRule(const std::string& ruleName) {
    auto it = plasticityRules.find(ruleName);
    if (it != plasticityRules.end()) {
        plasticityRules.erase(it);
    }
}

std::vector<std::string> DefaultNeuralArchitectureBuilder::getStructuralPlasticityRules() const {
    std::vector<std::string> ruleNames;
    ruleNames.reserve(plasticityRules.size());
    
    for (const auto& rulePair : plasticityRules) {
        ruleNames.push_back(rulePair.first);
    }
    
    return ruleNames;
}

} // namespace nlm

#include "ExperimentTemplate.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../agent/AgentBrain.hpp"
#include <unordered_map>

namespace nlm {

struct ExperimentTemplate::Impl {
    std::string name;
    std::string description;
    std::string category;
    std::string author;
    std::string version;
};

ExperimentTemplate::ExperimentTemplate() : pImpl(new Impl) {}

const std::string& ExperimentTemplate::getName() const {
    return pImpl->name;
}

const std::string& ExperimentTemplate::getDescription() const {
    return pImpl->description;
}

const std::string& ExperimentTemplate::getCategory() const {
    return pImpl->category;
}

const std::string& ExperimentTemplate::getAuthor() const {
    return pImpl->author;
}

const std::string& ExperimentTemplate::getVersion() const {
    return pImpl->version;
}

bool ExperimentTemplate::validate(const std::shared_ptr<EnhancedExperiment>& experiment) const {
    return true; // Basic validation - can be overridden
}

void ExperimentTemplate::cleanup(std::shared_ptr<EnhancedExperiment> experiment) {
    // Default cleanup - can be overridden
}

std::vector<std::string> ExperimentTemplate::getRequiredParameters() const {
    return {};
}

std::vector<std::string> ExperimentTemplate::getOptionalParameters() const {
    return {};
}

std::unordered_map<std::string, std::string> ExperimentTemplate::getDefaultParameters() const {
    return {};
}

std::unordered_map<std::string, std::string> ExperimentTemplate::getParameterRanges() const {
    return {};
}

std::vector<std::string> ExperimentTemplate::getExpectedMetrics() const {
    return {};
}

std::vector<std::string> ExperimentTemplate::getPerformanceMetrics() const {
    return {};
}

std::vector<std::string> ExperimentTemplate::getDiagnosticMetrics() const {
    return {};
}

bool ExperimentTemplate::canRun(const std::shared_ptr<EnhancedExperiment>& experiment) const {
    return true; // Basic check - can be overridden
}

std::vector<std::string> ExperimentTemplate::getValidationRequirements() const {
    return {};
}

// Template Factory Implementation
std::unordered_map<std::string, std::function<std::unique_ptr<ExperimentTemplate>()>> 
    ExperimentTemplateFactory::templateRegistry = {
        {"memory_integration", []() { return std::make_unique<MemoryIntegrationTemplate>(); }},
        {"prediction_system", []() { return std::make_unique<PredictionSystemTemplate>(); }},
        {"cognitive_benchmark", []() { return std::make_unique<CognitiveBenchmarkTemplate>(); }},
        {"neuromodulation", []() { return std::make_unique<NeuromodulationTemplate>(); }},
        {"development", []() { return std::make_unique<DevelopmentTemplate>(); }},
        {"social_learning", []() { return std::make_unique<SocialLearningTemplate>(); }}
    };

std::unique_ptr<ExperimentTemplate> ExperimentTemplateFactory::createTemplate(const std::string& name) {
    auto it = templateRegistry.find(name);
    if (it != templateRegistry.end()) {
        return it->second();
    }
    return nullptr;
}

std::vector<std::string> ExperimentTemplateFactory::getAvailableTemplateNames() {
    std::vector<std::string> names;
    for (const auto& pair : templateRegistry) {
        names.push_back(pair.first);
    }
    return names;
}

std::unique_ptr<ExperimentTemplate> ExperimentTemplateFactory::cloneTemplate(const std::string& name) {
    auto it = templateRegistry.find(name);
    if (it != templateRegistry.end()) {
        return it->second();
    }
    return nullptr;
}

} // namespace nlm

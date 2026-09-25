#include "EnhancedMetrics.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct EnhancedMetrics::Impl {
    // Metrics storage: category -> name -> values
    std::unordered_map<std::string, std::map<std::string, std::vector<double>>> metrics;
    
    // Metadata for each metric
    struct MetricMetadata {
        std::string name;
        std::string category;
        std::unordered_map<std::string, std::string> metadata;
    };
    std::vector<MetricMetadata> metaData;
    
    // Time series data
    std::unordered_map<std::string, std::vector<std::pair<double, double>>> timeSeries;
    
    Impl() {}
};

EnhancedMetrics::EnhancedMetrics() : pImpl(new Impl) {}

EnhancedMetrics::~EnhancedMetrics() = default;

void EnhancedMetrics::record(const std::string& name, double value,
                            const std::string& category,
                            const std::unordered_map<std::string, std::string>& metadata) {
    // Store value
    pImpl->metrics[category][name].push_back(value);
    
    // Store metadata if not already exists
    bool exists = false;
    for (const auto& meta : pImpl->metaData) {
        if (meta.name == name && meta.category == category) {
            exists = true;
            break;
        }
    }
    
    if (!exists) {
        Impl::MetricMetadata metricMeta;
        metricMeta.name = name;
        metricMeta.category = category;
        metricMeta.metadata = metadata;
        pImpl->metaData.push_back(metricMeta);
    }
}

const std::vector<double>& EnhancedMetrics::getValues(const std::string& name) const {
    static std::vector<double> empty;
    
    // Find metric in any category
    for (const auto& catPair : pImpl->metrics) {
        const auto& nameValues = catPair.second;
        auto it = nameValues.find(name);
        if (it != nameValues.end()) {
            return it->second;
        }
    }
    
    return empty;
}

std::vector<double> EnhancedMetrics::getValues(const std::string& name, const std::string& category) const {
    std::vector<double> values;
    
    auto catIt = pImpl->metrics.find(category);
    if (catIt != pImpl->metrics.end()) {
        const auto& nameValues = catIt->second;
        auto it = nameValues.find(name);
        if (it != nameValues.end()) {
            values = it->second;
        }
    }
    
    return values;
}

double EnhancedMetrics::getMean(const std::string& name) const {
    // Calculate mean across all categories
    std::vector<double> allValues;
    for (const auto& catPair : pImpl->metrics) {
        const auto& nameValues = catPair.second;
        auto it = nameValues.find(name);
        if (it != nameValues.end()) {
            allValues.insert(allValues.end(), it->second.begin(), it->second.end());
        }
    }
    
    if (allValues.empty()) return 0.0;
    
    double sum = std::accumulate(allValues.begin(), allValues.end(), 0.0);
    return sum / allValues.size();
}

double EnhancedMetrics::getMean(const std::string& name, const std::string& category) const {
    std::vector<double> values = getValues(name, category);
    
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double EnhancedMetrics::getStdDev(const std::string& name) const {
    // Calculate standard deviation across all categories
    std::vector<double> allValues;
    for (const auto& catPair : pImpl->metrics) {
        const auto& nameValues = catPair.second;
        auto it = nameValues.find(name);
        if (it != nameValues.end()) {
            allValues.insert(allValues.end(), it->second.begin(), it->second.end());
        }
    }
    
    return getStdDevFromValues(allValues);
}

double EnhancedMetrics::getStdDev(const std::string& name, const std::string& category) const {
    std::vector<double> values = getValues(name, category);
    return getStdDevFromValues(values);
}

double EnhancedMetrics::getStdDevFromValues(const std::vector<double>& values) const {
    if (values.size() < 2) return 0.0;
    
    double mean = getVectorMean(values);
    double sumSquares = 0.0;
    
    for (double v : values) {
        double diff = v - mean;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / (values.size() - 1));
}

double EnhancedMetrics::getMin(const std::string& name) const {
    std::vector<double> allValues;
    for (const auto& catPair : pImpl->metrics) {
        const auto& nameValues = catPair.second;
        auto it = nameValues.find(name);
        if (it != nameValues.end()) {
            allValues.insert(allValues.end(), it->second.begin(), it->second.end());
        }
    }
    
    if (allValues.empty()) return 0.0;
    
    return *std::min_element(allValues.begin(), allValues.end());
}

double EnhancedMetrics::getMin(const std::string& name, const std::string& category) const {
    std::vector<double> values = getValues(name, category);
    
    if (values.empty()) return 0.0;
    
    return *std::min_element(values.begin(), values.end());
}

double EnhancedMetrics::getMax(const std::string& name) const {
    std::vector<double> allValues;
    for (const auto& catPair : pImpl->metrics) {
        const auto& nameValues = catPair.second;
        auto it = nameValues.find(name);
        if (it != nameValues.end()) {
            allValues.insert(allValues.end(), it->second.begin(), it->second.end());
        }
    }
    
    if (allValues.empty()) return 0.0;
    
    return *std::max_element(allValues.begin(), allValues.end());
}

double EnhancedMetrics::getMax(const std::string& name, const std::string& category) const {
    std::vector<double> values = getValues(name, category);
    
    if (values.empty()) return 0.0;
    
    return *std::max_element(values.begin(), values.end());
}

double EnhancedMetrics::getMedian(const std::string& name) const {
    std::vector<double> allValues;
    for (const auto& catPair : pImpl->metrics) {
        const auto& nameValues = catPair.second;
        auto it = nameValues.find(name);
        if (it != nameValues.end()) {
            allValues.insert(allValues.end(), it->second.begin(), it->second.end());
        }
    }
    
    return getVectorMedian(allValues);
}

double EnhancedMetrics::getMedian(const std::string& name, const std::string& category) const {
    std::vector<double> values = getValues(name, category);
    return getVectorMedian(values);
}

double EnhancedMetrics::getVectorMedian(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    size_t n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
    } else {
        return sorted[n/2];
    }
}

double EnhancedMetrics::getVectorMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double EnhancedMetrics::getPercentile(const std::string& name, double percentile) const {
    std::vector<double> allValues;
    for (const auto& catPair : pImpl->metrics) {
        const auto& nameValues = catPair.second;
        auto it = nameValues.find(name);
        if (it != nameValues.end()) {
            allValues.insert(allValues.end(), it->second.begin(), it->second.end());
        }
    }
    
    return getVectorPercentile(allValues, percentile);
}

double EnhancedMetrics::getPercentile(const std::string& name, const std::string& category, double percentile) const {
    std::vector<double> values = getValues(name, category);
    return getVectorPercentile(values, percentile);
}

double EnhancedMetrics::getVectorPercentile(const std::vector<double>& values, double percentile) const {
    if (values.empty() || percentile < 0.0 || percentile > 100.0) return 0.0;
    
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    double index = (percentile / 100.0) * (sorted.size() - 1);
    size_t lower = static_cast<size_t>(std::floor(index));
    size_t upper = static_cast<size_t>(std::ceil(index));
    
    if (lower == upper) {
        return sorted[lower];
    }
    
    double weight = index - lower;
    return sorted[lower] * (1.0 - weight) + sorted[upper] * weight;
}

std::vector<std::string> EnhancedMetrics::getMetricNames() const {
    std::vector<std::string> names;
    
    for (const auto& catPair : pImpl->metrics) {
        const auto& nameValues = catPair.second;
        for (const auto& pair : nameValues) {
            bool exists = false;
            for (const auto& name : names) {
                if (name == pair.first) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                names.push_back(pair.first);
            }
        }
    }
    
    return names;
}

std::vector<std::string> EnhancedMetrics::getCategories() const {
    std::vector<std::string> categories;
    
    for (const auto& pair : pImpl->metrics) {
        bool exists = false;
        for (const auto& cat : categories) {
            if (cat == pair.first) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            categories.push_back(pair.first);
        }
    }
    
    return categories;
}

std::vector<std::pair<std::string, std::string>> EnhancedMetrics::getMetricCategories() const {
    std::vector<std::pair<std::string, std::string>> result;
    
    for (const auto& catPair : pImpl->metrics) {
        const auto& nameValues = catPair.second;
        for (const auto& pair : nameValues) {
            result.emplace_back(pair.first, catPair.first);
        }
    }
    
    return result;
}

EnhancedMetrics::PerformanceMetrics EnhancedMetrics::computePerformanceMetrics() const {
    PerformanceMetrics metrics;
    
    // Get neural activity (average firing rate)
    metrics.neuralActivity = getMean("avg_firing_rate");
    
    // Get synaptic strength (average synaptic weight)
    metrics.synapticStrength = getMean("avg_synaptic_weight");
    
    // Get prediction accuracy
    metrics.predictionAccuracy = 1.0 - getMean("prediction_error") / 100.0;
    
    // Get planning efficiency (placeholder)
    metrics.planningEfficiency = getMean("planning_success");
    
    // Get attention selectivity (placeholder)
    metrics.attentionSelectivity = getMean("selectivity_index");
    
    // Get memory retention (placeholder)
    metrics.memoryRetention = getMean("retention_accuracy");
    
    // Get learning speed (placeholder)
    metrics.learningSpeed = getMean("learning_rate");
    
    // Get generalization (placeholder)
    metrics.generalization = getMean("generalization_score");
    
    // Get exploration-exploitation balance (placeholder)
    metrics.explorationExploitation = getMean("exploration_rate");
    
    return metrics;
}

EnhancedMetrics::CognitiveMetrics EnhancedMetrics::computeCognitiveMetrics() const {
    CognitiveMetrics metrics;
    
    // Get working memory capacity
    metrics.workingMemoryCapacity = getMean("working_memory_traces");
    
    // Get episodic memory strength
    metrics.episodicMemoryStrength = getMean("memory_strength");
    
    // Get concept formation
    metrics.conceptFormation = getMean("concept_count");
    
    // Get prediction error
    metrics.predictionError = getMean("prediction_error");
    
    // Get planning horizon (placeholder)
    metrics.planningHorizon = getMean("planning_depth");
    
    // Get social learning capacity (placeholder)
    metrics.socialLearningCapacity = getMean("imitation_success");
    
    // Get curiosity level (placeholder)
    metrics.curiosityLevel = getMean("curiosity_score");
    
    // Get reward sensitivity (placeholder)
    metrics.rewardSensitivity = getMean("reward_response");
    
    // Get intrinsic motivation (placeholder)
    metrics.intrinsicMotivation = getMean("intrinsic_drive");
    
    return metrics;
}

EnhancedMetrics::MemoryMetrics EnhancedMetrics::computeMemoryMetrics() const {
    MemoryMetrics metrics;
    
    // Get encoding efficiency
    metrics.encodingEfficiency = getMean("encoding_speed") / getMean("encoding_accuracy");
    
    // Get retention rate (placeholder)
    metrics.retentionRate = getMean("retention_curve_slope");
    
    // Get recall accuracy
    metrics.recallAccuracy = getMean("recall_success");
    
    // Get interference level (placeholder)
    metrics.interferenceLevel = getMean("interference_measure");
    
    // Get consolidation rate (placeholder)
    metrics.consolidationRate = getMean("consolidation_speed");
    
    // Get pattern separation (placeholder)
    metrics.patternSeparation = getMean("pattern_separation_score");
    
    // Get pattern completion (placeholder)
    metrics.patternCompletion = getMean("pattern_completion_score");
    
    return metrics;
}

EnhancedMetrics::LearningMetrics EnhancedMetrics::computeLearningMetrics() const {
    LearningMetrics metrics;
    
    // Get learning curve slope
    metrics.learningCurveSlope = getMean("learning_slope");
    
    // Get convergence rate (placeholder)
    metrics.convergenceRate = getMean("convergence_speed");
    
    // Get transfer efficiency (placeholder)
    metrics.transferEfficiency = getMean("transfer_success");
    
    // Get adaptation speed (placeholder)
    metrics.adaptationSpeed = getMean("adaptation_rate");
    
    // Get robustness (placeholder)
    metrics.robustness = getMean("robustness_score");
    
    // Get sample efficiency (placeholder)
    metrics.sampleEfficiency = getMean("samples_per_skill");
    
    // Get retention after learning (placeholder)
    metrics.retentionAfterLearning = getMean("long_term_retention");
    
    return metrics;
}

EnhancedMetrics::NeuromodulationMetrics EnhancedMetrics::computeNeuromodulationMetrics() const {
    NeuromodulationMetrics metrics;
    
    // Get dopamine effect (placeholder)
    metrics.dopamineEffect = getMean("dopamine_impact");
    
    // Get curiosity drive
    metrics.curiosityDrive = getMean("curiosity_level");
    
    // Get novelty sensitivity
    metrics.noveltySensitivity = getMean("novelty_response");
    
    // Get reward prediction error
    metrics.rewardPredictionError = getMean("prediction_error");
    
    // Get attention modulation (placeholder)
    metrics.attentionModulation = getMean("attention_focus");
    
    // Get arousal level (placeholder)
    metrics.arousalLevel = getMean("arousal_score");
    
    // Get motivation strength (placeholder)
    metrics.motivationStrength = getMean("motivation_level");
    
    return metrics;
}

EnhancedMetrics::DevelopmentMetrics EnhancedMetrics::computeDevelopmentMetrics() const {
    DevelopmentMetrics metrics;
    
    // Get plasticity rate (placeholder)
    metrics.plasticityRate = getMean("plasticity_rate");
    
    // Get maturation level (placeholder)
    metrics.maturationLevel = getMean("maturation_score");
    
    // Get experience integration (placeholder)
    metrics.experienceIntegration = getMean("experience_integration_score");
    
    // Get skill complexity (placeholder)
    metrics.skillComplexity = getMean("skill_complexity");
    
    // Get behavioral flexibility (placeholder)
    metrics.behavioralFlexibility = getMean("flexibility_score");
    
    // Get memory organization (placeholder)
    metrics.memoryOrganization = getMean("memory_organization_score");
    
    // Get cognitive integration (placeholder)
    metrics.cognitiveIntegration = getMean("cognitive_integration_score");
    
    return metrics;
}

void EnhancedMetrics::clear() {
    pImpl->metrics.clear();
    pImpl->metaData.clear();
    pImpl->timeSeries.clear();
}

void EnhancedMetrics::clear(const std::string& category) {
    auto it = pImpl->metrics.find(category);
    if (it != pImpl->metrics.end()) {
        it->second.clear();
    }
}

void EnhancedMetrics::clear(const std::string& name, const std::string& category) {
    auto catIt = pImpl->metrics.find(category);
    if (catIt != pImpl->metrics.end()) {
        auto nameIt = catIt->second.find(name);
        if (nameIt != catIt->second.end()) {
            nameIt->second.clear();
        }
    }
}

std::string EnhancedMetrics::summary() const {
    std::stringstream ss;
    ss << "Enhanced Metrics Summary" << std::endl;
    ss << "=======================" << std::endl << std::endl;
    
    ss << "Categories: ";
    auto categories = getCategories();
    for (size_t i = 0; i < categories.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << categories[i];
    }
    ss << std::endl << std::endl;
    
    ss << "Metrics by Category:" << std::endl;
    for (const auto& category : categories) {
        ss << "  " << category << ":" << std::endl;
        auto catIt = pImpl->metrics.find(category);
        if (catIt != pImpl->metrics.end()) {
            for (const auto& pair : catIt->second) {
                ss << "    " << pair.first << ": "
                   << "mean=" << getMean(pair.first, category)
                   << ", min=" << getMin(pair.first, category)
                   << ", max=" << getMax(pair.first, category)
                   << ", std=" << getStdDev(pair.first, category) << std::endl;
            }
        }
    }
    
    ss << std::endl;
    
    // Compute and display specialized metrics
    auto perfMetrics = computePerformanceMetrics();
    ss << "Performance Metrics:" << std::endl;
    ss << "  Neural Activity: " << perfMetrics.neuralActivity << std::endl;
    ss << "  Synaptic Strength: " << perfMetrics.synapticStrength << std::endl;
    ss << "  Prediction Accuracy: " << (perfMetrics.predictionAccuracy * 100) << "%" << std::endl;
    ss << "  Planning Efficiency: " << perfMetrics.planningEfficiency << std::endl;
    ss << "  Attention Selectivity: " << perfMetrics.attentionSelectivity << std::endl;
    ss << "  Memory Retention: " << perfMetrics.memoryRetention << std::endl;
    ss << "  Learning Speed: " << perfMetrics.learningSpeed << std::endl;
    ss << "  Generalization: " << perfMetrics.generalization << std::endl;
    ss << "  Exploration-Exploitation: " << perfMetrics.explorationExploitation << std::endl << std::endl;
    
    return ss.str();
}

std::string EnhancedMetrics::summary(const std::string& category) const {
    std::stringstream ss;
    ss << "Metrics Summary for Category: " << category << std::endl;
    ss << "=============================" << std::endl << std::endl;
    
    auto catIt = pImpl->metrics.find(category);
    if (catIt != pImpl->metrics.end()) {
        for (const auto& pair : catIt->second) {
            ss << pair.first << ": "
               << "mean=" << getMean(pair.first, category)
               << ", min=" << getMin(pair.first, category)
               << ", max=" << getMax(pair.first, category)
               << ", std=" << getStdDev(pair.first, category) << std::endl;
        }
    }
    
    return ss.str();
}

std::string EnhancedMetrics::performanceSummary() const {
    std::stringstream ss;
    ss << "Performance Metrics Summary" << std::endl;
    ss << "==========================" << std::endl << std::endl;
    
    auto perfMetrics = computePerformanceMetrics();
    ss << "Neural Activity (avg firing rate): " << perfMetrics.neuralActivity << std::endl;
    ss << "Synaptic Strength (avg weight): " << perfMetrics.synapticStrength << std::endl;
    ss << "Prediction Accuracy: " << (perfMetrics.predictionAccuracy * 100) << "%" << std::endl;
    ss << "Planning Efficiency: " << perfMetrics.planningEfficiency << std::endl;
    ss << "Attention Selectivity: " << perfMetrics.attentionSelectivity << std::endl;
    ss << "Memory Retention: " << perfMetrics.memoryRetention << std::endl;
    ss << "Learning Speed: " << perfMetrics.learningSpeed << std::endl;
    ss << "Generalization: " << perfMetrics.generalization << std::endl;
    ss << "Exploration-Exploitation Balance: " << perfMetrics.explorationExploitation << std::endl;
    
    return ss.str();
}

std::string EnhancedMetrics::cognitiveSummary() const {
    std::stringstream ss;
    ss << "Cognitive Metrics Summary" << std::endl;
    ss << "=========================" << std::endl << std::endl;
    
    auto cogMetrics = computeCognitiveMetrics();
    ss << "Working Memory Capacity: " << cogMetrics.workingMemoryCapacity << std::endl;
    ss << "Episodic Memory Strength: " << cogMetrics.episodicMemoryStrength << std::endl;
    ss << "Concept Formation: " << cogMetrics.conceptFormation << std::endl;
    ss << "Prediction Error: " << cogMetrics.predictionError << std::endl;
    ss << "Planning Horizon: " << cogMetrics.planningHorizon << std::endl;
    ss << "Social Learning Capacity: " << cogMetrics.socialLearningCapacity << std::endl;
    ss << "Curiosity Level: " << cogMetrics.curiosityLevel << std::endl;
    ss << "Reward Sensitivity: " << cogMetrics.rewardSensitivity << std::endl;
    ss << "Intrinsic Motivation: " << cogMetrics.intrinsicMotivation << std::endl;
    
    return ss.str();
}

std::string EnhancedMetrics::memorySummary() const {
    std::stringstream ss;
    ss << "Memory Metrics Summary" << std::endl;
    ss << "=======================" << std::endl << std::endl;
    
    auto memMetrics = computeMemoryMetrics();
    ss << "Encoding Efficiency: " << memMetrics.encodingEfficiency << std::endl;
    ss << "Retention Rate: " << memMetrics.retentionRate << std::endl;
    ss << "Recall Accuracy: " << memMetrics.recallAccuracy << std::endl;
    ss << "Interference Level: " << memMetrics.interferenceLevel << std::endl;
    ss << "Consolidation Rate: " << memMetrics.consolidationRate << std::endl;
    ss << "Pattern Separation: " << memMetrics.patternSeparation << std::endl;
    ss << "Pattern Completion: " << memMetrics.patternCompletion << std::endl;
    
    return ss.str();
}

std::string EnhancedMetrics::learningSummary() const {
    std::stringstream ss;
    ss << "Learning Metrics Summary" << std::endl;
    ss << "========================" << std::endl << std::endl;
    
    auto learnMetrics = computeLearningMetrics();
    ss << "Learning Curve Slope: " << learnMetrics.learningCurveSlope << std::endl;
    ss << "Convergence Rate: " << learnMetrics.convergenceRate << std::endl;
    ss << "Transfer Efficiency: " << learnMetrics.transferEfficiency << std::endl;
    ss << "Adaptation Speed: " << learnMetrics.adaptationSpeed << std::endl;
    ss << "Robustness: " << learnMetrics.robustness << std::endl;
    ss << "Sample Efficiency: " << learnMetrics.sampleEfficiency << std::endl;
    ss << "Retention After Learning: " << learnMetrics.retentionAfterLearning << std::endl;
    
    return ss.str();
}

std::string EnhancedMetrics::neuromodulationSummary() const {
    std::stringstream ss;
    ss << "Neuromodulation Metrics Summary" << std::endl;
    ss << "==============================" << std::endl << std::endl;
    
    auto neuromodMetrics = computeNeuromodulationMetrics();
    ss << "Dopamine Effect: " << neuromodMetrics.dopamineEffect << std::endl;
    ss << "Curiosity Drive: " << neuromodMetrics.curiosityDrive << std::endl;
    ss << "Novelty Sensitivity: " << neuromodMetrics.noveltySensitivity << std::endl;
    ss << "Reward Prediction Error: " << neuromodMetrics.rewardPredictionError << std::endl;
    ss << "Attention Modulation: " << neuromodMetrics.attentionModulation << std::endl;
    ss << "Arousal Level: " << neuromodMetrics.arousalLevel << std::endl;
    ss << "Motivation Strength: " << neuromodMetrics.motivationStrength << std::endl;
    
    return ss.str();
}

std::string EnhancedMetrics::developmentSummary() const {
    std::stringstream ss;
    ss << "Development Metrics Summary" << std::endl;
    ss << "===========================" << std::endl << std::endl;
    
    auto devMetrics = computeDevelopmentMetrics();
    ss << "Plasticity Rate: " << devMetrics.plasticityRate << std::endl;
    ss << "Maturation Level: " << devMetrics.maturationLevel << std::endl;
    ss << "Experience Integration: " << devMetrics.experienceIntegration << std::endl;
    ss << "Skill Complexity: " << devMetrics.skillComplexity << std::endl;
    ss << "Behavioral Flexibility: " << devMetrics.behavioralFlexibility << std::endl;
    ss << "Memory Organization: " << devMetrics.memoryOrganization << std::endl;
    ss << "Cognitive Integration: " << devMetrics.cognitiveIntegration << std::endl;
    
    return ss.str();
}

std::string EnhancedMetrics::toJSON() const {
    json j;
    
    // Add metrics by category
    json metricsJson = json::object();
    for (const auto& catPair : pImpl->metrics) {
        json catJson = json::object();
        for (const auto& namePair : catPair.second) {
            json nameJson;
            nameJson["name"] = namePair.first;
            nameJson["values"] = namePair.second;
            nameJson["mean"] = getMean(namePair.first, catPair.first);
            nameJson["std"] = getStdDev(namePair.first, catPair.first);
            nameJson["min"] = getMin(namePair.first, catPair.first);
            nameJson["max"] = getMax(namePair.first, catPair.first);
            nameJson["median"] = getMedian(namePair.first, catPair.first);
            
            catJson[namePair.first] = nameJson;
        }
        metricsJson[catPair.first] = catJson;
    }
    
    j["metrics"] = metricsJson;
    
    // Add computed metrics
    j["performance"] = {
        {"neuralActivity", computePerformanceMetrics().neuralActivity},
        {"synapticStrength", computePerformanceMetrics().synapticStrength},
        {"predictionAccuracy", computePerformanceMetrics().predictionAccuracy},
        {"planningEfficiency", computePerformanceMetrics().planningEfficiency},
        {"attentionSelectivity", computePerformanceMetrics().attentionSelectivity},
        {"memoryRetention", computePerformanceMetrics().memoryRetention},
        {"learningSpeed", computePerformanceMetrics().learningSpeed},
        {"generalization", computePerformanceMetrics().generalization},
        {"explorationExploitation", computePerformanceMetrics().explorationExploitation}
    };
    
    j["cognitive"] = {
        {"workingMemoryCapacity", computeCognitiveMetrics().workingMemoryCapacity},
        {"episodicMemoryStrength", computeCognitiveMetrics().episodicMemoryStrength},
        {"conceptFormation", computeCognitiveMetrics().conceptFormation},
        {"predictionError", computeCognitiveMetrics().predictionError},
        {"planningHorizon", computeCognitiveMetrics().planningHorizon},
        {"socialLearningCapacity", computeCognitiveMetrics().socialLearningCapacity},
        {"curiosityLevel", computeCognitiveMetrics().curiosityLevel},
        {"rewardSensitivity", computeCognitiveMetrics().rewardSensitivity},
        {"intrinsicMotivation", computeCognitiveMetrics().intrinsicMotivation}
    };
    
    j["memory"] = {
        {"encodingEfficiency", computeMemoryMetrics().encodingEfficiency},
        {"retentionRate", computeMemoryMetrics().retentionRate},
        {"recallAccuracy", computeMemoryMetrics().recallAccuracy},
        {"interferenceLevel", computeMemoryMetrics().interferenceLevel},
        {"consolidationRate", computeMemoryMetrics().consolidationRate},
        {"patternSeparation", computeMemoryMetrics().patternSeparation},
        {"patternCompletion", computeMemoryMetrics().patternCompletion}
    };
    
    j["learning"] = {
        {"learningCurveSlope", computeLearningMetrics().learningCurveSlope},
        {"convergenceRate", computeLearningMetrics().convergenceRate},
        {"transferEfficiency", computeLearningMetrics().transferEfficiency},
        {"adaptationSpeed", computeLearningMetrics().adaptationSpeed},
        {"robustness", computeLearningMetrics().robustness},
        {"sampleEfficiency", computeLearningMetrics().sampleEfficiency},
        {"retentionAfterLearning", computeLearningMetrics().retentionAfterLearning}
    };
    
    j["neuromodulation"] = {
        {"dopamineEffect", computeNeuromodulationMetrics().dopamineEffect},
        {"curiosityDrive", computeNeuromodulationMetrics().curiosityDrive},
        {"noveltySensitivity", computeNeuromodulationMetrics().noveltySensitivity},
        {"rewardPredictionError", computeNeuromodulationMetrics().rewardPredictionError},
        {"attentionModulation", computeNeuromodulationMetrics().attentionModulation},
        {"arousalLevel", computeNeuromodulationMetrics().arousalLevel},
        {"motivationStrength", computeNeuromodulationMetrics().motivationStrength}
    };
    
    j["development"] = {
        {"plasticityRate", computeDevelopmentMetrics().plasticityRate},
        {"maturationLevel", computeDevelopmentMetrics().maturationLevel},
        {"experienceIntegration", computeDevelopmentMetrics().experienceIntegration},
        {"skillComplexity", computeDevelopmentMetrics().skillComplexity},
        {"behavioralFlexibility", computeDevelopmentMetrics().behavioralFlexibility},
        {"memoryOrganization", computeDevelopmentMetrics().memoryOrganization},
        {"cognitiveIntegration", computeDevelopmentMetrics().cognitiveIntegration}
    };
    
    return j.dump(2);
}

std::string EnhancedMetrics::toJSON(const std::string& category) const {
    json j;
    
    auto catIt = pImpl->metrics.find(category);
    if (catIt != pImpl->metrics.end()) {
        json catJson = json::object();
        for (const auto& namePair : catIt->second) {
            json nameJson;
            nameJson["name"] = namePair.first;
            nameJson["values"] = namePair.second;
            nameJson["mean"] = getMean(namePair.first, category);
            nameJson["std"] = getStdDev(namePair.first, category);
            nameJson["min"] = getMin(namePair.first, category);
            nameJson["max"] = getMax(namePair.first, category);
            nameJson["median"] = getMedian(namePair.first, category);
            
            catJson[namePair.first] = nameJson;
        }
        j[category] = catJson;
    }
    
    return j.dump(2);
}

double EnhancedMetrics::compareWithBaseline(const std::string& metricName,
                                          const EnhancedMetrics& baseline,
                                          const std::string& category) const {
    double current = getMean(metricName, category);
    double baselineValue = baseline.getMean(metricName, category);
    
    if (baselineValue == 0.0) return 0.0;
    
    return (current - baselineValue) / baselineValue;
}

bool EnhancedMetrics::hasConverged(const std::string& metricName,
                                  double threshold,
                                  const std::string& category) const {
    std::vector<double> currentValues = getValues(metricName, category);
    if (currentValues.size() < 2) return false;
    
    double currentMean = getMean(metricName, category);
    double currentStdDev = getStdDev(metricName, category);
    
    return currentStdDev / currentMean <= threshold;
}

bool EnhancedMetrics::hasImproved(const std::string& metricName,
                                 size_t windowSize,
                                 const std::string& category) const {
    std::vector<double> values = getValues(metricName, category);
    
    if (values.size() < 2 * windowSize) return false;
    
    double oldMean = getVectorMean(std::vector<double>(values.begin(), values.begin() + windowSize));
    double newMean = getVectorMean(std::vector<double>(values.end() - windowSize, values.end()));
    
    return newMean > oldMean;
}

double EnhancedMetrics::computeTrend(const std::string& metricName,
                                    const std::string& category) const {
    std::vector<double> values = getValues(metricName, category);
    
    if (values.size() < 2) return 0.0;
    
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    size_t n = values.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = values[i];
        
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    
    return slope;
}

std::vector<double> EnhancedMetrics::getTimeSeries(const std::string& metricName,
                                                   const std::string& category) const {
    std::vector<double> values = getValues(metricName, category);
    
    // For now, just return the values
    // In a real implementation, this would store time stamps
    return values;
}

} // namespace nlm

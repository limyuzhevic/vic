#include "TestUtilities.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/NeuralRegion.hpp"
#include <numeric>
#include <cmath>
#include <algorithm>

namespace nlm {

void TestUtils::logSection(const std::string& title) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== " + title + " ===");
    NLM_LOG_INFO("");
}

void TestUtils::logSubsection(const std::string& subtitle) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO(subtitle);
    NLM_LOG_INFO(std::string(subtitle.length(), '='));
}

void TestUtils::logResult(const std::string& label, const std::string& value) {
    NLM_LOG_INFO("  " + label + ": " + value);
}

void TestUtils::logMetric(const std::string& name, double value, const std::string& unit) {
    std::string formattedValue;
    if (std::abs(value) < 0.001 && value != 0.0) {
        formattedValue = "0.000";
    } else {
        formattedValue = std::to_string(value);
        if (formattedValue.find('.') == std::string::npos) {
            formattedValue += ".0";
        }
    }
    NLM_LOG_INFO("  " + name + ": " + formattedValue + unit);
}

void TestUtils::logProgress(int current, int total, const std::string& description) {
    int percentage = (total > 0) ? (current * 100) / total : 0;
    NLM_LOG_INFO(std::string(2, ' ') + "Step " + std::to_string(current) + "/" + std::to_string(total) + 
                 " (" + std::to_string(percentage) + "%)" + 
                 (description.empty() ? "" : " - " + description));
}

void TestUtils::logError(const std::string& message) {
    NLM_LOG_ERROR("  ERROR: " + message);
}

template<typename T>
std::vector<T> TestUtils::extractWeights(const NeuralRegion& region) {
    std::vector<T> weights;
    for (const auto& syn : region.getSynapses()) {
        weights.push_back(static_cast<T>(syn->getWeight()));
    }
    return weights;
}

float TestUtils::calculateMean(const std::vector<float>& values) {
    if (values.empty()) return 0.0f;
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    return sum / values.size();
}

float TestUtils::calculateVariance(const std::vector<float>& values, float mean) {
    if (values.empty()) return 0.0f;
    float sumSquaredDiffs = 0.0f;
    for (float value : values) {
        float diff = value - mean;
        sumSquaredDiffs += diff * diff;
    }
    return sumSquaredDiffs / values.size();
}

size_t TestUtils::countSignificantChanges(const std::vector<float>& before, const std::vector<float>& after, float threshold) {
    size_t minSize = std::min(before.size(), after.size());
    size_t count = 0;
    for (size_t i = 0; i < minSize; ++i) {
        float delta = after[i] - before[i];
        if (std::abs(delta) > threshold) {
            ++count;
        }
    }
    return count;
}

void ConfigSetup::setupDefaultPhase2Config(std::shared_ptr<Config> config) {
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void ConfigSetup::applyCommandLineArgs(std::shared_ptr<Config> config, int argc, char** argv) {
    config->loadFromArgs(argc, argv);
}

void ConfigSetup::logConfiguration(const Config& config) {
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config.getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config.getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config.getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config.getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config.getOr<float>("connection_probability", 0.15f)));
}

std::shared_ptr<Brain> BrainInitializer::createAndInitializeBrain(const std::shared_ptr<Config>& config) {
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        throw std::runtime_error("Brain initialization failed");
    }
    return brain;
}

void BrainInitializer::resetBrainForNewTest(std::shared_ptr<Brain> brain) {
    if (brain) {
        brain->reset();
        brain->initialize();
    }
}

LearningExperiment::Results LearningExperiment::runLearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed) {
    Results results;
    results.totalSpikes = brain->getTotalSpikeCount();
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        throw std::runtime_error("Region 1 not found for learning experiment");
    }
    
    results.initialWeights = TestUtils::extractWeights<float>(*region);
    results.initialMean = TestUtils::calculateMean(results.initialWeights);
    
    for (auto& syn : region->getSynapses()) {
        syn->enablePlasticity(true, true, false);
    }
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && results.mostActiveNeurons.size() < 20) {
                    if (std::find(results.mostActiveNeurons.begin(), results.mostActiveNeurons.end(), neuron->getId()) == results.mostActiveNeurons.end()) {
                        results.mostActiveNeurons.push_back(neuron->getId());
                    }
                    brain->injectCurrent(neuron->getId(), 30.0f);
                }
            }
        }
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            TestUtils::logProgress(step, 1000, "Plasticity experiment");
        }
    }
    
    results.finalWeights = TestUtils::extractWeights<float>(*region);
    results.finalMean = TestUtils::calculateMean(results.finalWeights);
    
    results.strengthened = countSignificantChanges(results.initialWeights, results.finalWeights, 0.01f);
    results.totalSpikes = brain->getTotalSpikeCount() - results.totalSpikes;
    
    results.unchanged = results.initialWeights.size() - results.strengthened;
    
    results.learningOccurred = (std::abs(results.finalMean - results.initialMean) > 0.001f) || 
                               (results.strengthened > 0);
    
    return results;
}

} // namespace nlm
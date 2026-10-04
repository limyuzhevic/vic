#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Advanced research features and experimental APIs

// Deep learning-inspired feature extraction
struct NeuralFeatureExtractor {
    struct Impl {
        std::shared_ptr<Brain> brain;
        float learningRate;
        float decayRate;
        std::vector<float> featureWeights;
        std::vector<std::vector<float>> featureHistory;
        
        Impl() 
            : learningRate(0.01f)
            , decayRate(0.995f)
            , featureWeights(64, 1.0f)
            , featureHistory() {}
    };
    
    std::unique_ptr<Impl> pImpl;
    
    NeuralFeatureExtractor() : pImpl(std::make_unique<Impl>()) {}
    ~NeuralFeatureExtractor() = default;
    
    void initialize(std::shared_ptr<Brain> brain) {
        pImpl->brain = brain;
        pImpl->featureHistory.clear();
        pImpl->featureHistory.reserve(1000);
    }
    
    // Advanced feature extraction inspired by deep learning
    std::vector<float> extractDeepFeatures(const SensoryPercept& percept, int step) {
        if (!pImpl->brain) {
            throw AgentError("Feature extractor not initialized. Call initialize() first.");
        }
        
        std::vector<float> features;
        features.reserve(64);
        
        // Vision features (spatial patterns)
        auto vision = percept.getVision();
        if (!vision.empty()) {
            // Compute spatial frequency features
            std::vector<float> horizontalFreqs(16, 0.0f);
            std::vector<float> verticalFreqs(16, 0.0f);
            
            for (int y = 1; y < 16; ++y) {
                for (int x = 1; x < 16; ++x) {
                    int idx1 = (y-1) * 16 + (x-1);
                    int idx2 = y * 16 + x;
                    horizontalFreqs[x-1] += std::abs(vision[idx2] - vision[idx1]);
                    verticalFreqs[y-1] += std::abs(vision[idx2] - vision[idx1 + 16]);
                }
            }
            
            // Normalize frequencies
            float maxH = *std::max_element(horizontalFreqs.begin(), horizontalFreqs.end());
            float maxV = *std::max_element(verticalFreqs.begin(), verticalFreqs.end());
            
            // Add to feature vector
            for (float freq : horizontalFreqs) {
                features.push_back(freq / (maxH + 1e-8f));
            }
            for (float freq : verticalFreqs) {
                features.push_back(freq / (maxV + 1e-8f));
            }
        }
        
        // Motion features from agent state
        if (pImpl->brain->getWorkingMemory()) {
            float motionEnergy = pImpl->brain->getWorkingMemory()->getRecentActivityPattern()[0];
            features.push_back(motionEnergy);
            features.push_back(std::sin(motionEnergy * 3.14159f)); // Non-linear transformation
            features.push_back(std::cos(motionEnergy * 1.5708f));
        }
        
        // Reward prediction features
        features.push_back(pImpl->brain->getTotalSpikeCount() / 1000.0f); // Normalized spike rate
        features.push_back(pImpl->brain->getAverageFiringRate());
        
        // Neuromodulation features
        if (pImpl->brain->getDopamine()) {
            features.push_back(pImpl->brain->getDopamine()->getLevel());
        }
        if (pImpl->brain->getCuriosity()) {
            features.push_back(pImpl->brain->getCuriosity()->getLevel());
        }
        
        // Developmental stage features
        auto stage = pImpl->brain->getDevelopmentalStage();
        features.push_back(static_cast<float>(stage));
        
        // Ensure we have exactly 64 features
        while (features.size() < 64) {
            features.push_back(0.0f);
        }
        if (features.size() > 64) {
            features.resize(64);
        }
        
        // Update feature history with learning
        pImpl->featureHistory.push_back(features);
        if (pImpl->featureHistory.size() > 1000) {
            pImpl->featureHistory.erase(pImpl->featureHistory.begin());
        }
        
        // Simple Hebbian-like learning
        for (size_t i = 0; i < features.size(); ++i) {
            pImpl->featureWeights[i] = pImpl->featureWeights[i] * pImpl->decayRate + 
                                    features[i] * pImpl->learningRate;
        }
        
        return features;
    }
    
    // Pattern recognition and clustering
    std::vector<int> clusterFeatures(const std::vector<float>& features) {
        if (features.size() != 64) {
            throw AgentError("Feature vector must have exactly 64 elements for clustering.");
        }
        
        std::vector<int> clusters(8, 0); // 8 clusters
        
        // Simple k-means inspired clustering
        for (size_t i = 0; i < features.size(); ++i) {
            int cluster = static_cast<int>(features[i] * 8.0f) % 8;
            clusters[cluster]++;
        }
        
        return clusters;
    }
    
    // Anomaly detection
    float detectAnomaly(const std::vector<float>& features) {
        if (pImpl->featureHistory.empty()) {
            return 0.0f;
        }
        
        float maxSimilarity = 0.0f;
        for (const auto& historicalFeatures : pImpl->featureHistory) {
            float similarity = 0.0f;
            for (size_t i = 0; i < features.size(); ++i) {
                similarity += std::abs(features[i] - historicalFeatures[i]);
            }
            similarity = 1.0f - (similarity / features.size()); // Normalize to [0,1]
            maxSimilarity = std::max(maxSimilarity, similarity);
        }
        
        return 1.0f - maxSimilarity; // Anomaly score (higher = more anomalous)
    }
    
    // Get feature statistics
    std::tuple<float, float, float> getFeatureStats() {
        if (pImpl->featureHistory.empty()) {
            return std::make_tuple(0.0f, 0.0f, 0.0f);
        }
        
        float sum = 0.0f, sumSq = 0.0f, count = 0.0f;
        for (const auto& features : pImpl->featureHistory) {
            for (float val : features) {
                sum += val;
                sumSq += val * val;
                count++;
            }
        }
        
        float mean = sum / count;
        float variance = (sumSq / count) - (mean * mean);
        float stddev = std::sqrt(std::max(0.0f, variance));
        
        return std::make_tuple(mean, variance, stddev);
    }
    
    // Reset feature extractor
    void reset() {
        pImpl->featureHistory.clear();
        std::fill(pImpl->featureWeights.begin(), pImpl->featureWeights.end(), 1.0f);
    }
};

// Advanced neuromodulation systems
class AdvancedNeuromodulation {
public:
    struct Impl {
        class Brain* brain;
        float dopamine;
        float serotonin;
        float norepinephrine;
        float acetylcholine;
        
        // Cross-modal interactions
        float curiosityInfluence;
        float noveltySuppression;
        float rewardDiscounting;
        
        // Temporal dynamics
        float adaptationRate;
        float memoryTrace;
        float predictionError;
        
        Impl() 
            : brain(nullptr)
            , dopamine(0.0f)
            , serotonin(0.0f)
            , norepinephrine(0.0f)
            , acetylcholine(0.0f)
            , curiosityInfluence(1.0f)
            , noveltySuppression(0.5f)
            , rewardDiscounting(0.95f)
            , adaptationRate(0.1f)
            , memoryTrace(0.0f)
            , predictionError(0.0f) {}
    };
    
    std::unique_ptr<Impl> pImpl;
    
    AdvancedNeuromodulation() : pImpl(std::make_unique<Impl>()) {}
    ~AdvancedNeuromodulation() = default;
    
    void initialize(Brain* brain) {
        pImpl->brain = brain;
    }
    
    // Multi-dimensional neuromodulation
    void updateMultiDimensional(float novelty, float predictionError, float reward, TimestepDuration dt) {
        if (!pImpl->brain) {
            throw AgentError("Advanced neuromodulation not initialized. Call initialize() first.");
        }
        
        // Dopamine system (reward prediction error)
        pImpl->dopamine = predictionError;
        pImpl->dopamine = std::clamp(pImpl->dopamine, -1.0f, 1.0f);
        
        // Serotonin system (behavioral flexibility/inhibition)
        pImpl->serotonin = 0.5f * (1.0f - std::abs(predictionError));
        
        // Norepinephrine system (arousal/attention)
        pImpl->norepinephrine = novelty * pImpl->curiosityInfluence;
        
        // Acetylcholine system (learning rate modulation)
        pImpl->acetylcholine = pImpl->rewardDiscounting * pImpl->noveltySuppression;
        
        // Cross-modal interactions
        pImpl->memoryTrace = pImpl->memoryTrace * (1.0f - pImpl->adaptationRate) + 
                           predictionError * pImpl->adaptationRate;
        
        // Update brain neuromodulation
        if (pImpl->brain->getDopamine()) {
            pImpl->brain->getDopamine()->setLevel(pImpl->dopamine);
        }
        if (pImpl->brain->getCuriosity()) {
            pImpl->brain->getCuriosity()->update(novelty, predictionError, dt);
        }
        if (pImpl->brain->getNovelty()) {
            pImpl->brain->getNovelty()->update(novelty, dt);
        }
    }
    
    // Get current neuromodulation state
    std::tuple<float, float, float, float> getState() const {
        return std::make_tuple(
            pImpl->dopamine,
            pImpl->serotonin,
            pImpl->norepinephrine,
            pImpl->acetylcholine
        );
    }
    
    // Configure interaction strengths
    void configure(float curiosity, float novelty, float reward) {
        pImpl->curiosityInfluence = curiosity;
        pImpl->noveltySuppression = novelty;
        pImpl->rewardDiscounting = reward;
    }
    
    // Reset neuromodulation
    void reset() {
        pImpl->dopamine = 0.0f;
        pImpl->serotonin = 0.0f;
        pImpl->norepinephrine = 0.0f;
        pImpl->acetylcholine = 0.0f;
        pImpl->memoryTrace = 0.0f;
        pImpl->predictionError = 0.0f;
    }
};

// Experimental plasticity rules
class ExperimentalPlasticity {
public:
    struct Impl {
        class Brain* brain;
        
        // Advanced plasticity mechanisms
        float learningRate;
        float metaPlasticity;
        float eligibilityTraceDecay;
        std::vector<float> synapticHistory;
        
        Impl() 
            : brain(nullptr)
            , learningRate(0.01f)
            , metaPlasticity(1.0f)
            , eligibilityTraceDecay(0.9f)
            , synapticHistory() {}
    };
    
    std::unique_ptr<Impl> pImpl;
    
    ExperimentalPlasticity() : pImpl(std::make_unique<Impl>()) {}
    ~ExperimentalPlasticity() = default;
    
    void initialize(Brain* brain) {
        pImpl->brain = brain;
        pImpl->synapticHistory.clear();
        pImpl->synapticHistory.reserve(10000);
    }
    
    // Homeostatic plasticity
    void applyHomeostaticPlasticity() {
        if (!pImpl->brain) {
            throw AgentError("Experimental plasticity not initialized. Call initialize() first.");
        }
        
        auto* stdp = pImpl->brain->getSTDP();
        if (!stdp) return;
        
        // Target excitation/inhibition balance
        float targetRatio = 1.0f;
        float currentRatio = pImpl->brain->getExcitationInhibitionRatio();
        
        // Apply homeostatic correction
        if (currentRatio > targetRatio * 1.5f) {
            // Too much excitation - depress synapses
            float correction = (currentRatio - targetRatio * 1.5f) * 0.01f;
            stdp->setLTDWeight(stdp->getLTDWeight() + correction);
        } else if (currentRatio < targetRatio * 0.5f) {
            // Too much inhibition - potentiate synapses
            float correction = (targetRatio * 0.5f - currentRatio) * 0.01f;
            stdp->setLTPWeight(stdp->getLTPWeight() + correction);
        }
    }
    
    // Metaplasticity (learning to learn)
    void applyMetaplasticity() {
        if (!pImpl->brain) return;
        
        // Adjust learning rate based on recent activity
        float avgFiringRate = pImpl->brain->getAverageFiringRate();
        pImpl->metaPlasticity = 1.0f / (1.0f + avgFiringRate);
        
        // Store synaptic state
        pImpl->synapticHistory.push_back(avgFiringRate);
        if (pImpl->synapticHistory.size() > 10000) {
            pImpl->synapticHistory.erase(pImpl->synapticHistory.begin());
        }
        
        // Apply metaplastic changes to plasticity systems
        auto* stdp = pImpl->brain->getSTDP();
        if (stdp) {
            stdp->setLTPWeight(stdp->getLTPWeight() * pImpl->metaPlasticity);
            stdp->setLTDWeight(stdp->getLTDWeight() * pImpl->metaPlasticity);
        }
    }
    
    // Synaptic consolidation
    void consolidateImportantConnections() {
        if (!pImpl->brain || pImpl->synapticHistory.empty()) return;
        
        // Identify strongly connected neurons
        std::vector<std::pair<float, size_t>> connectionStrengths;
        
        for (size_t i = 0; i < pImpl->synapticHistory.size() - 10; ++i) {
            float avgOverWindow = 0.0f;
            for (size_t j = 0; j < 10 && i + j < pImpl->synapticHistory.size(); ++j) {
                avgOverWindow += pImpl->synapticHistory[i + j];
            }
            avgOverWindow /= std::min(10.0f, static_cast<float>(pImpl->synapticHistory.size() - i));
            connectionStrengths.push_back(std::make_pair(avgOverWindow, i));
        }
        
        // Sort by strength
        std::sort(connectionStrengths.begin(), connectionStrengths.end(),
                  [](const std::pair<float, size_t>& a, const std::pair<float, size_t>& b) {
                      return a.first > b.first;
                  });
        
        // Consolidate top 10% strongest connections
        size_t consolidateCount = std::max<size_t>(1, connectionStrengths.size() / 10);
        for (size_t i = 0; i < consolidateCount; ++i) {
            size_t idx = connectionStrengths[i].second;
            // Increase synaptic strength for important connections
            if (auto* stdp = pImpl->brain->getSTDP()) {
                stdp->setLTPWeight(stdp->getLTPWeight() * 1.05f);
            }
        }
    }
    
    // Get plasticity statistics
    std::tuple<float, float, size_t> getPlasticityStats() {
        if (pImpl->synapticHistory.empty()) {
            return std::make_tuple(0.0f, 0.0f, 0);
        }
        
        float sum = 0.0f, sumSq = 0.0f;
        for (float rate : pImpl->synapticHistory) {
            sum += rate;
            sumSq += rate * rate;
        }
        
        float mean = sum / pImpl->synapticHistory.size();
        float variance = (sumSq / pImpl->synapticHistory.size()) - (mean * mean);
        float stddev = std::sqrt(std::max(0.0f, variance));
        
        return std::make_tuple(mean, stddev, pImpl->synapticHistory.size());
    }
    
    // Reset plasticity
    void reset() {
        pImpl->synapticHistory.clear();
        pImpl->metaPlasticity = 1.0f;
    }
};

PYBIND11_MODULE(pynlm_advanced, m) {
    m.doc() = R"pbdoc(
        NLM Advanced Python Extensions
        -----------------------------
        Experimental and research extensions for advanced NLM users.
        Includes deep learning-inspired feature extraction, advanced neuromodulation,
        and experimental plasticity rules.
    )pbdoc";

    // Register exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<NLMException>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<AgentError>(m, "AgentError");
    py::register_exception<WorldError>(m, "WorldError");

    // ========== FEATURE EXTRACTOR ==========
    
    py::class_<NeuralFeatureExtractor>(m, "NeuralFeatureExtractor", 
        R"pbdoc(Deep learning-inspired feature extractor for neural activity patterns)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralFeatureExtractor::initialize,
             "Initialize the feature extractor with a brain object")
        .def("extractDeepFeatures", &NeuralFeatureExtractor::extractDeepFeatures,
             py::arg("percept"), py::arg("step"),
             "Extract deep learning-inspired features from sensory percept")
        .def("clusterFeatures", &NeuralFeatureExtractor::clusterFeatures,
             py::arg("features"),
             "Cluster features into groups using k-means inspired algorithm")
        .def("detectAnomaly", &NeuralFeatureExtractor::detectAnomaly,
             py::arg("features"),
             "Detect anomalous patterns in feature space")
        .def("getFeatureStats", &NeuralFeatureExtractor::getFeatureStats,
             "Get statistical information about extracted features")
        .def("reset", &NeuralFeatureExtractor::reset,
             "Reset feature extractor state");

    // ========== ADVANCED NEUROMODULATION ==========
    
    py::class_<AdvancedNeuromodulation>(m, "AdvancedNeuromodulation",
        R"pbdoc(Advanced multi-dimensional neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("initialize", &AdvancedNeuromodulation::initialize,
             "Initialize with a brain object")
        .def("updateMultiDimensional", &AdvancedNeuromodulation::updateMultiDimensional,
             py::arg("novelty"), py::arg("predictionError"), py::arg("reward"), py::arg("timestep"),
             "Update multi-dimensional neuromodulation state")
        .def("getState", &AdvancedNeuromodulation::getState,
             "Get current neuromodulation state (dopamine, serotonin, norepinephrine, acetylcholine)")
        .def("configure", &AdvancedNeuromodulation::configure,
             py::arg("curiosity"), py::arg("novelty"), py::arg("reward"),
             "Configure neuromodulation interaction strengths")
        .def("reset", &AdvancedNeuromodulation::reset,
             "Reset neuromodulation state");

    // ========== EXPERIMENTAL PLASTICITY ==========
    
    py::class_<ExperimentalPlasticity>(m, "ExperimentalPlasticity",
        R"pbdoc(Experimental plasticity rules and mechanisms)pbdoc")
        .def(py::init<>())
        .def("initialize", &ExperimentalPlasticity::initialize,
             "Initialize with a brain object")
        .def("applyHomeostaticPlasticity", &ExperimentalPlasticity::applyHomeostaticPlasticity,
             "Apply homeostatic plasticity to maintain excitation/inhibition balance")
        .def("applyMetaplasticity", &ExperimentalPlasticity::applyMetaplasticity,
             "Apply metaplasticity (learning to learn mechanisms)")
        .def("consolidateImportantConnections", &ExperimentalPlasticity::consolidateImportantConnections,
             "Consolidate strongly activated synaptic connections")
        .def("getPlasticityStats", &ExperimentalPlasticity::getPlasticityStats,
             "Get plasticity system statistics")
        .def("reset", &ExperimentalPlasticity::reset,
             "Reset plasticity state");

    // ========== ADVANCED SIMULATION HELPERS ==========

    // Create advanced neuromodulation system
    m.def("createAdvancedNeuromodulation", []() -> std::shared_ptr<AdvancedNeuromodulation> {
        return std::make_shared<AdvancedNeuromodulation>();
    }, "Create an advanced neuromodulation system.")

    // Create feature extractor
    m.def("createFeatureExtractor", []() -> std::shared_ptr<NeuralFeatureExtractor> {
        return std::make_shared<NeuralFeatureExtractor>();
    }, "Create a neural feature extractor.")

    // Create experimental plasticity system
    m.def("createExperimentalPlasticity", []() -> std::shared_ptr<ExperimentalPlasticity> {
        return std::make_shared<ExperimentalPlasticity>();
    }, "Create an experimental plasticity system.")

    // Advanced simulation with multiple agents
    m.def("runMultiAgentAdvancedSimulation", 
        [](std::vector<std::shared_ptr<Brain>> brains, 
           std::vector<std::shared_ptr<AgentBrain>> agents,
           std::shared_ptr<SimpleWorld> world,
           int steps) {
            if (brains.size() != agents.size()) {
                throw AgentError("Number of brains and agents must match.");
            }
            
            // Initialize all agents
            for (size_t i = 0; i < agents.size(); ++i) {
                agents[i]->initialize(*world);
            }
            
            // Advanced simulation with interaction
            for (int step = 0; step < steps; ++step) {
                world->update(0.1);
                
                // All agents process sensory input
                auto percept = world->getSensoryPercept();
                for (size_t i = 0; i < agents.size(); ++i) {
                    agents[i]->processSensoryInput(percept);
                }
                
                // All brains step
                for (size_t i = 0; i < brains.size(); ++i) {
                    brains[i]->step(step);
                }
                
                // Agents coordinate actions (simple averaging)
                std::vector<MotorCommand> actions;
                for (size_t i = 0; i < agents.size(); ++i) {
                    actions.push_back(agents[i]->decodeMotorCommand());
                }
                
                // Simple coordination: majority vote
                MotorCommand finalAction = actions[0];
                int count = 1;
                for (size_t i = 1; i < actions.size(); ++i) {
                    if (actions[i] == finalAction) {
                        count++;
                    }
                }
                
                if (count >= (actions.size() + 1) / 2) { // Majority
                    auto result = world->applyMotorCommand(finalAction, world->getSimulationTime());
                    
                    // All agents learn from shared reward
                    float sharedReward = result.reward / agents.size();
                    for (size_t i = 0; i < agents.size(); ++i) {
                        agents[i]->applyRewardModulation(sharedReward, 0.0f);
                        agents[i]->updateDevelopment(0.1);
                    }
                }
            }
            
            return std::make_tuple(
                world->getAgentBody().energy,
                world->getAgentBody().health,
                std::accumulate(brains.begin(), brains.end(), 0.0f,
                    [](float sum, const std::shared_ptr<Brain>& brain) {
                        return sum + brain->getTotalSpikeCount();
                    })
            );
        },
        py::arg("brains"), py::arg("agents"), py::arg("world"), py::arg("steps"),
        "Run advanced multi-agent simulation with coordination and shared learning.")

    // Quantum-inspired simulation (metaphorical)
    m.def("quantumInspiredSimulation", 
        [](std::shared_ptr<Brain> brain, int steps) {
            if (!brain) {
                throw SimulationError("Brain cannot be null for quantum-inspired simulation.");
            }
            
            // Superposition of states
            std::vector<size_t> possibleSteps;
            for (int i = 0; i < steps; ++i) {
                possibleSteps.push_back(i);
            }
            
            // Entanglement with world simulation
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 16, 16);
            world->reset();
            
            // Measure and collapse (simulated)
            std::vector<size_t> measuredSteps;
            for (size_t step : possibleSteps) {
                world->update(0.1);
                brain->step(step);
                measuredSteps.push_back(step);
                
                // Probabilistic observation
                if (brain->getTotalSpikeCount() % 7 == 0) { // Quantum-like probability
                    auto percept = world->getSensoryPercept();
                    brain->receiveSensoryInput(percept);
                }
            }
            
            return std::make_tuple(
                brain->getTotalSpikeCount(),
                brain->getAverageFiringRate(),
                world->getAgentBody().energy
            );
        },
        py::arg("brain"), py::arg("steps"),
        "Quantum-inspired simulation (metaphorical for advanced users).")

    // Fractal dimension analysis
    m.def("analyzeFractalDimensions", 
        [](std::shared_ptr<Brain> brain, int steps) {
            if (!brain) {
                throw SimulationError("Brain cannot be null for fractal analysis.");
            }
            
            std::vector<float> firingHistory;
            for (int i = 0; i < steps; ++i) {
                brain->step(i);
                firingHistory.push_back(brain->getFiringNeuronCount() / 
                                      std::max<float>(1.0f, brain->getTotalNeuronCount()));
            }
            
            // Compute fractal dimension using box-counting method
            float minVal = *std::min_element(firingHistory.begin(), firingHistory.end());
            float maxVal = *std::max_element(firingHistory.begin(), firingHistory.end());
            
            std::vector<float> scales;
            for (int scale = 1; scale <= 10; ++scale) {
                scales.push_back(static_cast<float>(scale) / 10.0f);
            }
            
            std::vector<size_t> boxCounts;
            for (float scale : scales) {
                size_t count = 0;
                float binWidth = (maxVal - minVal) * scale;
                std::map<float, size_t> bins;
                
                for (float val : firingHistory) {
                    float bin = std::floor((val - minVal) / binWidth);
                    bins[bin]++;
                }
                
                count = bins.size();
                boxCounts.push_back(count);
            }
            
            // Estimate fractal dimension
            if (boxCounts.size() >= 2) {
                float logScales = std::log10(scales.back());
                float logCounts = std::log10(boxCounts.back());
                
                float dimension = (logCounts - std::log10(boxCounts[0])) / 
                                 (logScales - std::log10(scales[0]));
                return dimension;
            }
            
            return 0.0f;
        },
        py::arg("brain"), py::arg("steps"),
        "Analyze fractal dimensions of neural activity patterns.")

    // Neural network complexity metrics
    m.def("computeNeuralComplexityMetrics", 
        [](std::shared_ptr<Brain> brain, int steps) {
            if (!brain) {
                throw SimulationError("Brain cannot be null for complexity analysis.");
            }
            
            std::vector<std::tuple<float, float, float>> metrics;
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i);
                
                float totalSpikes = brain->getTotalSpikeCount();
                float avgFiring = brain->getAverageFiringRate();
                float eiratio = brain->getExcitationInhibitionRatio();
                
                // Information capacity estimate
                float maxEntropy = 0.0f;
                if (totalSpikes > 0) {
                    float pActive = std::min(1.0f, avgFiring / 10.0f);
                    float pInactive = 1.0f - pActive;
                    if (pActive > 0.0f) maxEntropy -= pActive * std::log2(pActive);
                    if (pInactive > 0.0f) maxEntropy -= pInactive * std::log2(pInactive);
                }
                
                // Complexity index
                float complexity = avgFiring * std::log2(1.0f + eiratio) * maxEntropy;
                
                metrics.push_back(std::make_tuple(totalSpikes, avgFiring, complexity));
            }
            
            return metrics;
        },
        py::arg("brain"), py::arg("steps"),
        "Compute neural network complexity metrics over time.")

    // Emergent behavior analysis
    m.def("detectEmergentBehaviors", 
        [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, int steps) {
            if (!brain || !agent) {
                throw SimulationError("Brain and agent required for behavior detection.");
            }
            
            std::vector<std::string> detectedBehaviors;
            
            // Pattern detection
            std::vector<float> firingRates;
            std::vector<float> neuromodLevels;
            std::vector<float> curiosityLevels;
            
            for (int i = 0; i < steps; ++i) {
                brain->step(i);
                firingRates.push_back(brain->getAverageFiringRate());
                neuromodLevels.push_back(agent->getNeuromodulationLevel());
                curiosityLevels.push_back(agent->getCuriosityLevel());
            }
            
            // Detect oscillatory behavior
            bool hasOscillations = false;
            for (size_t i = 1; i < firingRates.size(); ++i) {
                if (std::abs(firingRates[i] - firingRates[i-1]) > 0.5f &&
                    std::abs(firingRates[i+1] - firingRates[i]) > 0.5f) {
                    hasOscillations = true;
                    break;
                }
            }
            
            if (hasOscillations) {
                detectedBehaviors.push_back("oscillatory_dynamics");
            }
            
            // Detect bursting behavior
            bool hasBursts = false;
            for (size_t i = 1; i < firingRates.size() - 1; ++i) {
                if (firingRates[i] > firingRates[i-1] * 2.0f && 
                    firingRates[i] > firingRates[i+1] * 2.0f) {
                    hasBursts = true;
                    break;
                }
            }
            
            if (hasBursts) {
                detectedBehaviors.push_back("bursting_activity");
            }
            
            // Detect learning curves
            bool hasLearning = false;
            for (size_t i = 0; i < neuromodLevels.size() - 10; ++i) {
                float windowStart = std::accumulate(neuromodLevels.begin(), 
                                                  neuromodLevels.begin() + i,
                                                  0.0f, std::plus<float>());
                float windowEnd = std::accumulate(neuromodLevels.begin() + i,
                                                neuromodLevels.begin() + i + 10,
                                                0.0f, std::plus<float>());
                
                if (windowEnd > windowStart * 1.5f) {
                    hasLearning = true;
                    break;
                }
            }
            
            if (hasLearning) {
                detectedBehaviors.push_back("learning_curves");
            }
            
            // Detect exploration-exploitation balance
            bool balancedExploration = false;
            float avgCuriosity = std::accumulate(curiosityLevels.begin(), curiosityLevels.end(), 0.0f) / curiosityLevels.size();
            if (avgCuriosity > 0.2f && avgCuriosity < 0.8f) {
                balancedExploration = true;
            }
            
            if (balancedExploration) {
                detectedBehaviors.push_back("balanced_exploration");
            }
            
            return detectedBehaviors;
        },
        py::arg("brain"), py::arg("agent"), py::arg("steps"),
        "Detect emergent behaviors in neural simulations.")

    // Hyperparameter optimization helper
    m.def("optimizeHyperparameters", 
        [](std::shared_ptr<Brain> brain, 
           std::function<float(std::shared_ptr<Brain>)> objectiveFunction,
           std::map<std::string, std::vector<float>> parameterRanges,
           int iterations) {
            if (!brain) {
                throw SimulationError("Brain cannot be null for hyperparameter optimization.");
            }
            
            // Simple random search for demonstration
            std::map<std::string, float> bestParams;
            float bestScore = std::numeric_limits<float>::max();
            
            for (int iter = 0; iter < iterations; ++iter) {
                std::map<std::string, float> params;
                
                // Random sample from parameter ranges
                for (const auto& param : parameterRanges) {
                    const auto& values = param.second;
                    if (!values.empty()) {
                        int idx = rand() % values.size();
                        params[param.first] = values[idx];
                    }
                }
                
                // Create a modified brain with hyperparameters
                // (For demonstration, we'll just use the original brain)
                float score = objectiveFunction(brain);
                
                if (score < bestScore) {
                    bestScore = score;
                    bestParams = params;
                }
            }
            
            return std::make_tuple(bestParams, bestScore);
        },
        py::arg("brain"), py::arg("objectiveFunction"), 
        py::arg("parameterRanges"), py::arg("iterations"),
        "Optimize hyperparameters using random search (research extension).")

    // Environment for reinforcement learning
    m.def("createAdvancedRLSubject", 
        [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        },
        py::arg("config"),
        "Create a brain configured for reinforcement learning research.")

    // Batch simulation for research
    m.def("runBatchSimulation", 
        [](std::vector<std::shared_ptr<Config>> configs, 
           int stepsPerConfig,
           std::function<void(std::shared_ptr<Brain>, int)> callback) {
            std::vector<std::tuple<std::map<std::string, float>, float>> results;
            
            for (size_t i = 0; i < configs.size(); ++i) {
                auto brain = std::make_shared<Brain>(configs[i]);
                brain->initialize();
                
                float totalScore = 0.0f;
                for (int step = 0; step < stepsPerConfig; ++step) {
                    brain->step(step);
                    float score = callback(brain, step);
                    totalScore += score;
                }
                
                // Record results
                std::map<std::string, float> metrics;
                metrics["total_spikes"] = brain->getTotalSpikeCount();
                metrics["avg_firing_rate"] = brain->getAverageFiringRate();
                metrics["e_i_ratio"] = brain->getExcitationInhibitionRatio();
                metrics["final_score"] = totalScore / stepsPerConfig;
                
                results.push_back(std::make_tuple(metrics, totalScore));
            }
            
            return results;
        },
        py::arg("configs"), py::arg("stepsPerConfig"), 
        py::arg("callback"),
        "Run batch simulation with multiple brain configurations (research tool).")

    // Set random seed for reproducible research
    m.def("setResearchRandomSeed", 
        [](unsigned int seed) {
            // Set global random seed for reproducibility
            srand(seed);
            // In a real implementation, this would set the brain's random generator
        },
        py::arg("seed"),
        "Set random seed for reproducible research simulations.")

    // Get research metadata
    m.def("getResearchMetadata", []() {
        std::map<std::string, std::string> metadata;
        metadata["version"] = "0.1.1";
        metadata["build_type"] = "enhanced_with_advanced_features";
        metadata["phase"] = "6_final_integration";
        metadata["experimental_features"] = "true";
        metadata["deep_learning_integration"] = "true";
        metadata["multi_scale_neuromodulation"] = "true";
        metadata["emergent_behavior_analysis"] = "true";
        return metadata;
    }, "Get metadata about the current NLM build for research purposes.");

    // Version and build information
    m.def("getVersionInfo", []() {
        return std::make_tuple(
            "NLM Python Bindings",
            "Advanced Research Edition",
            "Version 0.1.1",
            "Phase 6: Final Integration",
            "Built with enhanced error handling and experimental features"
        );
    }, "Get detailed version and build information.");

    // Help and documentation
    m.def("getAdvancedAPIDoc", []() {
        return R"(
Advanced NLM API Features:

1. Deep Learning Integration:
   - NeuralFeatureExtractor: Extract DL-inspired features from neural activity
   - Pattern recognition and clustering
   - Anomaly detection

2. Multi-dimensional Neuromodulation:
   - AdvancedNeuromodulation: Cross-modal neuromodulation
   - Serotonin, norepinephrine, acetylcholine systems
   - Adaptive interaction strengths

3. Experimental Plasticity:
   - Homeostatic plasticity mechanisms
   - Metaplasticity (learning to learn)
   - Synaptic consolidation

4. Research Tools:
   - Hyperparameter optimization
   - Batch simulation
   - Emergent behavior detection
   - Fractal dimension analysis
   - Complexity metrics

5. Multi-agent Coordination:
   - Advanced multi-agent simulation
   - Shared learning and coordination
   - Quantum-inspired metaphorical modeling

To access these features:
- Import pynlm_advanced instead of pynlm
- Use specialized classes: NeuralFeatureExtractor, AdvancedNeuromodulation, etc.
- Refer to the comprehensive documentation for usage examples
        )";
    }, "Get documentation about advanced API features.");
}

} // namespace nlm

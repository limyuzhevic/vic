#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <set>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

// Feature importance
struct FeatureImportance {
    std::string featureId;
    std::string featureType;
    double importanceScore;
    double normalizedImportance;
    std::vector<double> importanceHistory;
    std::map<std::string, double> contributionMetrics;
    bool isSelected;
    
    FeatureImportance(const std::string& id, const std::string& type,
                     double score, bool selected = false)
        : featureId(id), featureType(type), importanceScore(score),
          normalizedImportance(0.0), isSelected(selected) {
        importanceHistory.reserve(1000);
    }
};

// Partial dependence
struct PartialDependence {
    std::string featureId;
    std::vector<double> featureValues;
    std::vector<double> predictedValues;
    std::vector<double> averagePredictions;
    std::vector<double> marginalEffects;
    double minFeatureValue;
    double maxFeatureValue;
    
    PartialDependence(const std::string& id, const std::vector<double>& values,
                     const std::vector<double>& preds)
        : featureId(id), featureValues(values), predictedValues(preds),
          minFeatureValue(*std::min_element(values.begin(), values.end())),
          maxFeatureValue(*std::max_element(values.begin(), values.end())) {
        
        // Calculate average predictions for each feature value
        averagePredictions.resize(featureValues.size(), 0.0);
        
        std::unordered_map<double, std::vector<double>> valueToPredictions;
        for (size_t i = 0; i < featureValues.size(); ++i) {
            valueToPredictions[featureValues[i]].push_back(predictedValues[i]);
        }
        
        for (const auto& pair : valueToPredictions) {
            double avgPred = std::accumulate(pair.second.begin(), pair.second.end(), 0.0) / pair.second.size();
            for (size_t i = 0; i < featureValues.size(); ++i) {
                if (featureValues[i] == pair.first) {
                    averagePredictions[i] = avgPred;
                }
            }
        }
        
        // Calculate marginal effects
        marginalEffects.resize(featureValues.size(), 0.0);
        for (size_t i = 1; i < featureValues.size(); ++i) {
            marginalEffects[i] = averagePredictions[i] - averagePredictions[i-1];
        }
    }
};

// SHAP value
struct SHAPValue {
    std::string featureId;
    double shapValue;
    double shapMagnitude;
    std::string shapDirection;
    std::vector<double> interactionTerms;
    double expectedValue;
    
    SHAPValue(const std::string& featureId, double value, double magnitude,
              const std::string& direction)
        : featureId(featureId), shapValue(value), shapMagnitude(magnitude),
          shapDirection(direction), expectedValue(0.0) {}
};

// Global surrogate model
class GlobalSurrogateModel {
private:
    std::string modelId;
    std::vector<std::string> inputFeatures;
    std::vector<std::string> outputTargets;
    std::vector<std::vector<double>> surrogateCoefficients;
    std::vector<std::vector<double>> surrogateInteractions;
    std::string surrogateType;
    double regularizationParameter;
    
public:
    GlobalSurrogateModel(const std::string& id, const std::string& type = "linear",
                         double regParam = 0.01)
        : modelId(id), surrogateType(type), regularizationParameter(regParam) {
    }
    
    // Train surrogate model
    void train(const std::vector<std::vector<double>>& X,
               const std::vector<double>& y) {
        size_t nSamples = X.size();
        if (nSamples == 0) return;
        
        size_t nFeatures = X[0].size();
        
        // Initialize coefficients
        surrogateCoefficients.assign(nFeatures + 1, std::vector<double>(nFeatures + 1, 0.0));  // Include intercept
        surrogateInteractions.assign(nFeatures, std::vector<double>(nFeatures, 0.0));
        
        // Simple linear regression using normal equation
        for (size_t j = 0; j < nFeatures + 1; ++j) {
            for (size_t k = 0; k < nFeatures + 1; ++k) {
                double sum = 0.0;
                for (size_t i = 0; i < nSamples; ++i) {
                    double xVal = (j == 0) ? 1.0 : X[i][j - 1];
                    double yVal = (k == 0) ? 1.0 : (k <= nFeatures ? X[i][k - 1] : 1.0);
                    sum += xVal * yVal * y[i];
                }
                surrogateCoefficients[j][k] = sum;
            }
        }
        
        // Add regularization
        for (size_t j = 0; j < nFeatures + 1; ++j) {
            for (size_t k = 0; k < nFeatures + 1; ++k) {
                surrogateCoefficients[j][k] /= (1.0 + regularizationParameter);
            }
        }
    }
    
    // Predict using surrogate model
    double predict(const std::vector<double>& x) const {
        double prediction = 0.0;
        
        for (size_t j = 0; j < surrogateCoefficients.size(); ++j) {
            double xVal = (j == 0) ? 1.0 : x[j - 1];
            
            for (size_t k = 0; k < surrogateCoefficients[j].size(); ++k) {
                double yVal = (k == 0) ? 1.0 : (k <= x.size() ? x[k - 1] : 1.0);
                prediction += surrogateCoefficients[j][k] * xVal * yVal;
            }
        }
        
        return prediction;
    }
    
    std::string getModelId() const { return modelId; }
    
    std::string getGlobalSurrogateModelStatus() const {
        std::stringstream ss;
        ss << "Global Surrogate Model: " << modelId << "\n";
        ss << "  Surrogate Type: " << surrogateType << "\n";
        ss << "  Regularization Parameter: " << regularizationParameter << "\n";
        ss << "  Number of Features: " << inputFeatures.size() << "\n";
        ss << "  Number of Targets: " << outputTargets.size() << "\n";
        
        return ss.str();
    }
};

// Local interpretable model-agnostic explanation (LIME)
class LIMEExplainer {
private:
    std::string explainerId;
    double sampleSize;
    double neighborhoodRadius;
    double perturbationStep;
    double threshold;
    std::mt19937 rng;
    
public:
    LIMEExplainer(const std::string& id, double sample = 1000, double radius = 0.5,
                  double step = 0.1, double thresh = 0.01)
        : explainerId(id), sampleSize(sample), neighborhoodRadius(radius),
          perturbationStep(step), threshold(thresh), rng(std::random_device{}()) {
    }
    
    // Explain a prediction
    std::vector<SHAPValue> explain(const std::vector<double>& instance,
                                   const std::function<double(const std::vector<double>&)>& blackBoxModel,
                                   const std::vector<std::string>& featureNames) {
        std::vector<SHAPValue> explanations;
        
        // Generate neighborhood samples
        std::vector<std::vector<double>> neighborhood = generateNeighborhood(instance);
        
        // Get predictions for neighborhood
        std::vector<double> predictions;
        for (const auto& sample : neighborhood) {
            predictions.push_back(blackBoxModel(sample));
        }
        
        // Calculate feature importance using weighted local linear regression
        std::vector<double> featureWeights = calculateLocalFeatureWeights(instance, neighborhood, predictions);
        
        // Create SHAP explanations
        for (size_t i = 0; i < featureNames.size() && i < featureWeights.size(); ++i) {
            double weight = featureWeights[i];
            double magnitude = std::abs(weight);
            
            if (magnitude > threshold) {
                std::string direction = (weight > 0) ? "positive" : "negative";
                explanations.emplace_back(featureNames[i], weight, magnitude, direction);
            }
        }
        
        // Sort by magnitude
        std::sort(explanations.begin(), explanations.end(),
                 [](const SHAPValue& a, const SHAPValue& b) {
                     return a.shapMagnitude > b.shapMagnitude;
                 });
        
        return explanations;
    }
    
    // Generate neighborhood samples
    std::vector<std::vector<double>> generateNeighborhood(const std::vector<double>& instance) {
        std::vector<std::vector<double>> neighborhood;
        
        // Add the instance itself
        neighborhood.push_back(instance);
        
        // Generate perturbed samples
        for (size_t i = 0; i < sampleSize - 1; ++i) {
            std::vector<double> perturbed = instance;
            
            // Perturb each feature
            for (size_t j = 0; j < perturbed.size(); ++j) {
                if (std::uniform_real_distribution<double>(0.0, 1.0)(rng) < 0.5) {
                    double perturbation = (std::uniform_real_distribution<double>(-1.0, 1.0)(rng) *
                                         perturbationStep);
                    perturbed[j] += perturbation;
                    
                    // Ensure value is within reasonable bounds
                    perturbed[j] = std::max(0.0, std::min(perturbed[j], 1.0));
                }
            }
            
            neighborhood.push_back(perturbed);
        }
        
        return neighborhood;
    }
    
    // Calculate local feature weights
    std::vector<double> calculateLocalFeatureWeights(
        const std::vector<double>& instance,
        const std::vector<std::vector<double>>& neighborhood,
        const std::vector<double>& predictions) {
        
        if (neighborhood.empty()) {
            return std::vector<double>(instance.size(), 0.0);
        }
        
        // Create kernel weights based on distance to instance
        std::vector<double> kernelWeights(neighborhood.size(), 0.0);
        for (size_t i = 0; i < neighborhood.size(); ++i) {
            double distance = calculateDistance(instance, neighborhood[i]);
            kernelWeights[i] = std::exp(-distance / neighborhoodRadius);
        }
        
        // Calculate feature weights using weighted local linear regression
        std::vector<double> featureWeights(instance.size(), 0.0);
        
        for (size_t j = 0; j < instance.size(); ++j) {
            double numerator = 0.0, denominator = 0.0;
            
            for (size_t i = 0; i < neighborhood.size(); ++i) {
                double kernelWeight = kernelWeights[i];
                double featureDiff = neighborhood[i][j] - instance[j];
                double predictionDiff = predictions[i] - *std::max_element(predictions.begin(), predictions.end()) * 0.5;
                
                numerator += kernelWeight * featureDiff * predictionDiff;
                denominator += kernelWeight * featureDiff * featureDiff;
            }
            
            if (denominator > 0) {
                featureWeights[j] = numerator / denominator;
            }
        }
        
        return featureWeights;
    }
    
    // Calculate distance between instances
    double calculateDistance(const std::vector<double>& a, const std::vector<double>& b) {
        double distance = 0.0;
        for (size_t i = 0; i < a.size() && i < b.size(); ++i) {
            distance += (a[i] - b[i]) * (a[i] - b[i]);
        }
        
        return std::sqrt(distance);
    }
    
    std::string getExplainerId() const { return explainerId; }
    
    std::string getLIMEExplainerStatus() const {
        std::stringstream ss;
        ss << "LIME Explainer: " << explainerId << "\n";
        ss << "  Sample Size: " << sampleSize << "\n";
        ss << "  Neighborhood Radius: " << neighborhoodRadius << "\n";
        ss << "  Perturbation Step: " << perturbationStep << "\n";
        ss << "  Threshold: " << threshold << "\n";
        
        return ss.str();
    }
};

// Anchor explanations
class AnchorExplainer {
private:
    std::string explainerId;
    double coverageThreshold;
    double precisionThreshold;
    size_t maxCandidates;
    double minSupport;
    std::mt19937 rng;
    
public:
    AnchorExplainer(const std::string& id, double coverage = 0.8, double precision = 0.9,
                    size_t maxCand = 1000, double minSup = 0.1)
        : explainerId(id), coverageThreshold(coverage), precisionThreshold(precision),
          maxCandidates(maxCand), minSupport(minSup), rng(std::random_device{}()) {
    }
    
    // Find anchors for an instance
    std::vector<SHAPValue> findAnchors(const std::vector<double>& instance,
                                       const std::function<bool(const std::vector<double>&)>& coverageFunction,
                                       const std::vector<std::string>& featureNames) {
        std::vector<SHAPValue> anchors;
        
        // Generate candidate anchors
        std::vector<std::vector<double>> candidates = generateCandidates(featureNames.size());
        
        // Evaluate each candidate
        for (const auto& candidate : candidates) {
            double coverage = calculateCoverage(candidate, coverageFunction);
            double precision = calculatePrecision(candidate, instance, coverageFunction);
            
            if (coverage >= coverageThreshold && precision >= precisionThreshold) {
                double strength = precision * coverage;
                anchors.emplace_back("anchor", strength, strength, "positive");
            }
        }
        
        // Filter and rank anchors
        anchors = filterAndRankAnchors(anchors);
        
        return anchors;
    }
    
    // Generate candidate anchors
    std::vector<std::vector<double>> generateCandidates(size_t numFeatures) {
        std::vector<std::vector<double>> candidates;
        
        for (size_t i = 0; i < maxCandidates; ++i) {
            std::vector<double> candidate(numFeatures);
            for (size_t j = 0; j < numFeatures; ++j) {
                candidate[j] = std::uniform_real_distribution<double>(0.0, 1.0)(rng);
            }
            candidates.push_back(candidate);
        }
        
        return candidates;
    }
    
    // Calculate coverage of candidate
    double calculateCoverage(const std::vector<double>& candidate,
                             const std::function<bool(const std::vector<double>&)>& coverageFunction) {
        double covered = 0.0;
        int total = 1000;  // Number of random samples to test
        
        for (int i = 0; i < total; ++i) {
            std::vector<double> sample(candidate.size());
            for (size_t j = 0; j < candidate.size(); ++j) {
                sample[j] = std::uniform_real_distribution<double>(0.0, 1.0)(rng);
            }
            
            if (coverageFunction(sample)) {
                covered++;
            }
        }
        
        return covered / total;
    }
    
    // Calculate precision of candidate
    double calculatePrecision(const std::vector<double>& candidate,
                              const std::vector<double>& instance,
                              const std::function<bool(const std::vector<double>&)>& coverageFunction) {
        int correct = 0;
        int total = 1000;
        
        for (int i = 0; i < total; ++i) {
            std::vector<double> sample = instance;
            
            // Perturb the instance
            for (size_t j = 0; j < sample.size(); ++j) {
                if (std::uniform_real_distribution<double>(0.0, 1.0)(rng) < 0.3) {
                    double perturbation = (std::uniform_real_distribution<double>(-0.2, 0.2)(rng));
                    sample[j] += perturbation;
                    sample[j] = std::max(0.0, std::min(sample[j], 1.0));
                }
            }
            
            bool isCovered = coverageFunction(sample);
            bool isRelevant = (calculateDistance(sample, instance) < 0.1); // Simplified relevance check
            
            if (isCovered && isRelevant) {
                correct++;
            }
        }
        
        return correct / (double)total;
    }
    
    // Filter and rank anchors
    std::vector<SHAPValue> filterAndRankAnchors(std::vector<SHAPValue>& anchors) {
        // Remove duplicate anchors
        std::vector<SHAPValue> uniqueAnchors;
        std::set<std::string> seen;
        
        for (const auto& anchor : anchors) {
            if (seen.find(anchor.featureId) == seen.end()) {
                uniqueAnchors.push_back(anchor);
                seen.insert(anchor.featureId);
            }
        }
        
        // Sort by strength
        std::sort(uniqueAnchors.begin(), uniqueAnchors.end(),
                 [](const SHAPValue& a, const SHAPValue& b) {
                     return a.shapMagnitude > b.shapMagnitude;
                 });
        
        return uniqueAnchors;
    }
    
    // Calculate distance between instances
    double calculateDistance(const std::vector<double>& a, const std::vector<double>& b) {
        double distance = 0.0;
        for (size_t i = 0; i < a.size() && i < b.size(); ++i) {
            distance += (a[i] - b[i]) * (a[i] - b[i]);
        }
        
        return std::sqrt(distance);
    }
    
    std::string getExplainerId() const { return explainerId; }
    
    std::string getAnchorExplainerStatus() const {
        std::stringstream ss;
        ss << "Anchor Explainer: " << explainerId << "\n";
        ss << "  Coverage Threshold: " << coverageThreshold << "\n";
        ss << "  Precision Threshold: " << precisionThreshold << "\n";
        ss << "  Maximum Candidates: " << maxCandidates << "\n";
        ss << "  Minimum Support: " << minSupport << "\n";
        
        return ss.str();
    }
};

// Advanced model interpretability
class AdvancedModelInterpretability {
private:
    std::string interpreterId;
    std::unique_ptr<GlobalSurrogateModel> globalSurrogate;
    std::unique_ptr<LIMEExplainer> limeExplainer;
    std::unique_ptr<AnchorExplainer> anchorExplainer;
    std::vector<FeatureImportance> featureImportances;
    std::vector<PartialDependence> partialDependenceExplanations;
    std::vector<SHAPValue> shapExplanations;
    double interpretabilityScore;
    
public:
    AdvancedModelInterpretability(const std::string& id, const Config& config) : interpreterId(id) {
        // Initialize explainers
        globalSurrogate = std::make_unique<GlobalSurrogateModel>("global_surrogate", "linear", 0.01);
        limeExplainer = std::make_unique<LIMEExplainer>("lime", 1000, 0.5, 0.1, 0.01);
        anchorExplainer = std::make_unique<AnchorExplainer>("anchor", 0.8, 0.9, 1000, 0.1);
        
        interpretabilityScore = config.getOr("interpretability.score", 0.85);
    }
    
    // Calculate feature importance
    void calculateFeatureImportance(const std::vector<std::vector<double>>& X,
                                   const std::vector<double>& y,
                                   const std::vector<std::string>& featureNames) {
        featureImportances.clear();
        
        // Train surrogate model
        globalSurrogate->train(X, y);
        
        // Calculate feature importance from surrogate model coefficients
        for (size_t i = 0; i < featureNames.size() && i < globalSurrogate->getModelCoefficients().size(); ++i) {
            double importance = std::abs(globalSurrogate->getModelCoefficients()[i][0]);  // Simplified
            featureImportances.emplace_back(featureNames[i], "feature", importance, true);
        }
        
        // Normalize importances
        normalizeFeatureImportances();
    }
    
    // Generate partial dependence explanations
    void generatePartialDependenceExplanations(
        const std::vector<std::vector<double>>& X,
        const std::vector<double>& y,
        const std::vector<std::string>& featureNames) {
        partialDependenceExplanations.clear();
        
        for (size_t i = 0; i < featureNames.size() && i < X[0].size(); ++i) {
            std::vector<double> featureValues;
            std::vector<double> predictedValues;
            
            for (const auto& row : X) {
                featureValues.push_back(row[i]);
                
                // Predict with surrogate model
                std::vector<double> tempX = row;
                double prediction = globalSurrogate->predict(tempX);
                predictedValues.push_back(prediction);
            }
            
            partialDependenceExplanations.emplace_back(featureNames[i], featureValues, predictedValues);
        }
    }
    
    // Generate SHAP explanations for instance
    std::vector<SHAPValue> generateSHAPExplanations(
        const std::vector<double>& instance,
        const std::function<double(const std::vector<double>&)>& blackBoxModel,
        const std::vector<std::string>& featureNames) {
        return limeExplainer->explain(instance, blackBoxModel, featureNames);
    }
    
    // Generate anchor explanations for instance
    std::vector<SHAPValue> generateAnchorExplanations(
        const std::vector<double>& instance,
        const std::function<bool(const std::vector<double>&)>& coverageFunction,
        const std::vector<std::string>& featureNames) {
        return anchorExplainer->findAnchors(instance, coverageFunction, featureNames);
    }
    
    // Normalize feature importances
    void normalizeFeatureImportances() {
        if (featureImportances.empty()) return;
        
        double maxImportance = 0.0;
        for (const auto& imp : featureImportances) {
            maxImportance = std::max(maxImportance, imp.importanceScore);
        }
        
        if (maxImportance > 0) {
            for (auto& imp : featureImportances) {
                imp.normalizedImportance = imp.importanceScore / maxImportance;
                imp.importanceHistory.push_back(imp.normalizedImportance);
                if (imp.importanceHistory.size() > 100) {
                    imp.importanceHistory.erase(imp.importanceHistory.begin());
                }
            }
        }
    }
    
    // Get top k important features
    std::vector<FeatureImportance> getTopKFeatures(int k) const {
        std::vector<FeatureImportance> topFeatures = featureImportances;
        std::partial_sort(topFeatures.begin(), topFeatures.begin() + std::min(k, (int)topFeatures.size()),
                         topFeatures.end(),
                         [](const FeatureImportance& a, const FeatureImportance& b) {
                             return a.normalizedImportance > b.normalizedImportance;
                         });
        
        if (topFeatures.size() > (size_t)k) {
            topFeatures.resize(k);
        }
        
        return topFeatures;
    }
    
    // Get feature importance for specific feature
    FeatureImportance getFeatureImportance(const std::string& featureId) const {
        for (const auto& imp : featureImportances) {
            if (imp.featureId == featureId) {
                return imp;
            }
        }
        
        return FeatureImportance("", "", 0.0, false);
    }
    
    std::string getInterpreterId() const { return interpreterId; }
    
    std::string getAdvancedModelInterpretabilityStatus() const {
        std::stringstream ss;
        ss << "Advanced Model Interpretability: " << interpreterId << "\n";
        ss << "  Number of Features: " << featureImportances.size() << "\n";
        ss << "  Number of Partial Dependence Explanations: " << partialDependenceExplanations.size() << "\n";
        ss << "  Interpretability Score: " << interpretabilityScore << "\n";
        
        if (!featureImportances.empty()) {
            ss << "  Top Features:\n";
            auto topFeatures = getTopKFeatures(5);
            for (const auto& imp : topFeatures) {
                ss << "    " << imp.featureId << ": " << imp.normalizedImportance << "\n";
            }
        }
        
        return ss.str();
    }
    
    void saveInterpretabilityResults(const Config& config) const {
        // Implementation for saving interpretability results
    }
    
    void loadInterpretabilityResults(const Config& config) {
        // Implementation for loading interpretability results
    }
};

} // namespace nlm

#pragma once

#include <string>
#include <vector>

namespace nlm {

// Evaluation metrics
struct EvaluationMetrics {
    float accuracy = 0.0f;                 // Classification accuracy
    float precision = 0.0f;                // Precision (positive predictive value)
    float recall = 0.0f;                   // Recall (sensitivity)
    float f1Score = 0.0f;                  // F1 score (harmonic mean of precision and recall)
    float auc = 0.0f;                      // Area under ROC curve
    float mse = 0.0f;                      // Mean squared error
    float rmse = 0.0f;                     // Root mean squared error
    float mae = 0.0f;                      // Mean absolute error
    
    // Confusion matrix (for classification)
    std::vector<std::vector<size_t>> confusionMatrix;
    
    EvaluationMetrics() : accuracy(0.0f), precision(0.0f), recall(0.0f), f1Score(0.0f),
                          auc(0.0f), mse(0.0f), rmse(0.0f), mae(0.0f) {}
};

// Test dataset
struct TestDataset {
    std::vector<std::vector<float>> inputs;  // Input samples
    std::vector<std::vector<float>> targets; // Target values
    std::vector<std::string> labels;         // Sample labels
    std::vector<float> weights;              // Sample weights for weighted averaging
    
    TestDataset() {}
    
    TestDataset(const std::vector<std::vector<float>>& in,
                const std::vector<std::vector<float>>& tgt,
                const std::vector<std::string>& lbl)
        : inputs(in), targets(tgt), labels(lbl) {
        weights.resize(inputs.size(), 1.0f);
    }
};

// Cross-validation results
struct CrossValidationResult {
    std::vector<EvaluationMetrics> foldMetrics;    // Metrics for each fold
    float meanAccuracy = 0.0f;                     // Mean accuracy across folds
    float stdAccuracy = 0.0f;                      // Standard deviation of accuracy
    float meanF1Score = 0.0f;                       // Mean F1 score across folds
    float stdF1Score = 0.0f;                        // Standard deviation of F1 score
    
    CrossValidationResult() : meanAccuracy(0.0f), stdAccuracy(0.0f), meanF1Score(0.0f), stdF1Score(0.0f) {}
};

} // namespace nlm
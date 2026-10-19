#include "VisualizationSystem.hpp"

namespace nlm {

struct PredictionVisualizer::Impl {
    bool showPredictionErrors;
    bool showConfidenceIntervals;
    bool showPlanningTrees;
    size_t planningTreeDepth;
    double errorTimeWindow;
    
    std::vector<PredictionError> errors;
    std::vector<ConfidenceInterval> intervals;
    std::vector<PlanningTree> trees;
    
    // Simulation state
    double currentPredictionTime;
};

PredictionVisualizer::PredictionVisualizer() : pImpl(std::make_unique<Impl>()) {
    pImpl->showPredictionErrors = true;
    pImpl->showConfidenceIntervals = true;
    pImpl->showPlanningTrees = true;
    pImpl->planningTreeDepth = 3;
    pImpl->errorTimeWindow = 300.0; // 5 minutes
    pImpl->currentPredictionTime = 0.0;
}

PredictionVisualizer::~PredictionVisualizer() = default;

void PredictionVisualizer::setShowPredictionErrors(bool show) {
    pImpl->showPredictionErrors = show;
}

void PredictionVisualizer::setShowConfidenceIntervals(bool show) {
    pImpl->showConfidenceIntervals = show;
}

void PredictionVisualizer::setShowPlanningTrees(bool show) {
    pImpl->showPlanningTrees = show;
}

void PredictionVisualizer::setPlanningTreeDepth(size_t depth) {
    pImpl->planningTreeDepth = depth;
}

void PredictionVisualizer::setErrorTimeWindow(double timeWindow) {
    pImpl->errorTimeWindow = timeWindow;
}

void PredictionVisualizer::updatePredictionErrors(const std::vector<PredictionError>& errors) {
    pImpl->errors = errors;
}

void PredictionVisualizer::updateConfidenceIntervals(const std::vector<ConfidenceInterval>& intervals) {
    pImpl->intervals = intervals;
}

void PredictionVisualizer::updatePlanningTrees(const std::vector<PlanningTree>& trees) {
    pImpl->trees = trees;
}

float PredictionVisualizer::calculatePredictionAccuracy() const {
    // Calculate prediction accuracy from confidence intervals
    if (pImpl->intervals.empty()) return 0.0f;
    
    float totalAccuracy = 0.0f;
    for (const auto& interval : pImpl->intervals) {
        // Accuracy based on interval width (narrower = more confident/accurate)
        float width = interval.upperBound - interval.lowerBound;
        totalAccuracy += (1.0f - std::min(1.0f, width)) * interval.confidence;
    }
    
    return totalAccuracy / pImpl->intervals.size();
}

float PredictionVisualizer::calculatePredictionUncertainty() const {
    // Calculate average uncertainty from confidence intervals
    if (pImpl->intervals.empty()) return 0.0f;
    
    float totalWidth = 0.0f;
    for (const auto& interval : pImpl->intervals) {
        totalWidth += interval.upperBound - interval.lowerBound;
    }
    
    return totalWidth / pImpl->intervals.size();
}

float PredictionVisualizer::calculatePlanningEfficiency() const {
    // Calculate planning efficiency from planning trees
    if (pImpl->trees.empty()) return 0.0f;
    
    float totalValue = 0.0f;
    float totalUncertainty = 0.0f;
    
    for (const auto& tree : pImpl->trees) {
        totalValue += tree.value * (1.0f - tree.uncertainty);
        totalUncertainty += tree.uncertainty;
    }
    
    return (totalValue / pImpl->trees.size()) * (1.0f - (totalUncertainty / pImpl->trees.size()));
}

std::vector<std::string> PredictionVisualizer::detectPredictionPatterns() const {
    // Detect patterns in prediction errors
    std::vector<std::string> patterns;
    
    for (const auto& error : pImpl->errors) {
        if (error.errorMagnitude > 0.8f) {
            patterns.push_back(error.errorType + "_high_error");
        } else if (error.errorMagnitude < 0.1f) {
            patterns.push_back(error.errorType + "_low_error");
        }
    }
    
    return patterns;
}

void PredictionVisualizer::simulatePredictionUpdates(double timeStep) {
    pImpl->currentPredictionTime += timeStep;
    
    // Update prediction errors over time
    for (auto& error : pImpl->errors) {
        // Decay errors over time
        error.errorMagnitude *= 0.95f;
        error.confidence *= 0.98f;
    }
}

void PredictionVisualizer::animatePlanningTrees(double timeStep) {
    // Animate planning tree visualization
    for (auto& tree : pImpl->trees) {
        // Update tree position over time
        tree.x += static_cast<float>(timeStep * 0.01f);
        if (tree.x > 100.0f) {
            tree.x -= 100.0f;
        }
        
        // Update children
        for (size_t childId : tree.children) {
            // Find child and update its position
        }
    }
}

bool PredictionVisualizer::exportPredictionData(const std::string& filepath) {
    // Export prediction data to file
    return true;
}

bool PredictionVisualizer::exportPlanningHistory(const std::string& filepath) {
    // Export planning history to file
    return true;
}

} // namespace nlm

#include "VisualizationSystem.hpp"

namespace nlm {

struct PlasticityVisualizer::Impl {
    bool showSTDPTraces;
    bool showHebbianCurves;
    bool showPlasticityRules;
    double learningCurveTimeWindow;
    bool traceDecayVisualization;
    
    std::vector<STDPTrace> stdpTraces;
    std::vector<HebbianLearningCurve> hebbianCurves;
    std::vector<PlasticityRuleViz> rules;
    
    // Simulation state
    double currentSTDPTime;
};

PlasticityVisualizer::PlasticityVisualizer() : pImpl(std::make_unique<Impl>()) {
    pImpl->showSTDPTraces = true;
    pImpl->showHebbianCurves = true;
    pImpl->showPlasticityRules = true;
    pImpl->learningCurveTimeWindow = 300.0; // 5 minutes
    pImpl->traceDecayVisualization = true;
    pImpl->currentSTDPTime = 0.0;
}

PlasticityVisualizer::~PlasticityVisualizer() = default;

void PlasticityVisualizer::setShowSTDPTraces(bool show) {
    pImpl->showSTDPTraces = show;
}

void PlasticityVisualizer::setShowHebbianCurves(bool show) {
    pImpl->showHebbianCurves = show;
}

void PlasticityVisualizer::setShowPlasticityRules(bool show) {
    pImpl->showPlasticityRules = show;
}

void PlasticityVisualizer::setLearningCurveTimeWindow(double timeWindow) {
    pImpl->learningCurveTimeWindow = timeWindow;
}

void PlasticityVisualizer::setTraceDecayVisualization(bool showDecay) {
    pImpl->traceDecayVisualization = showDecay;
}

void PlasticityVisualizer::updateSTDPTraces(const std::vector<STDPTrace>& traces) {
    pImpl->stdpTraces = traces;
}

void PlasticityVisualizer::updateHebbianCurves(const std::vector<HebbianLearningCurve>& curves) {
    pImpl->hebbianCurves = curves;
}

void PlasticityVisualizer::updatePlasticityRules(const std::vector<PlasticityRuleViz>& rules) {
    pImpl->rules = rules;
}

float PlasticityVisualizer::calculatePlasticityBalance() const {
    // Calculate balance between LTP and LTD
    float ltpSum = 0.0f;
    float ltdSum = 0.0f;
    
    for (const auto& trace : pImpl->stdpTraces) {
        if (trace.isPositive) {
            ltpSum += trace.traceValue;
        } else {
            ltdSum += trace.traceValue;
        }
    }
    
    float total = ltpSum + ltdSum;
    if (total == 0.0f) return 1.0f;
    
    return ltpSum / total;
}

float PlasticityVisualizer::calculateLearningEfficiency() const {
    // Calculate overall learning efficiency
    if (pImpl->hebbianCurves.empty()) return 0.0f;
    
    float totalLearning = 0.0f;
    for (const auto& curve : pImpl->hebbianCurves) {
        totalLearning += curve.learningRate;
    }
    
    return totalLearning / pImpl->hebbianCurves.size();
}

float PlasticityVisualizer::calculateAdaptationSpeed() const {
    // Calculate adaptation speed
    if (pImpl->rules.empty()) return 0.0f;
    
    float totalAdaptation = 0.0f;
    for (const auto& rule : pImpl->rules) {
        totalAdaptation += rule.adaptationRate;
    }
    
    return totalAdaptation / pImpl->rules.size();
}

std::vector<std::string> PlasticityVisualizer::detectPlasticityPatterns() const {
    // Detect plasticity patterns
    std::vector<std::string> patterns;
    
    for (const auto& trace : pImpl->stdpTraces) {
        if (trace.traceValue > 0.1f && trace.isPositive) {
            patterns.push_back("LTP_dominant");
        } else if (trace.traceValue < -0.1f && !trace.isPositive) {
            patterns.push_back("LTD_dominant");
        }
    }
    
    return patterns;
}

void PlasticityVisualizer::simulateTraceDecay(double timeStep) {
    if (!pImpl->traceDecayVisualization) return;
    
    pImpl->currentSTDPTime += timeStep;
    
    // Apply exponential decay to traces
    for (auto& trace : pImpl->stdpTraces) {
        trace.traceValue *= std::exp(-trace.decayRate * static_cast<float>(timeStep));
        trace.opacity *= 0.95f;
    }
}

void PlasticityVisualizer::animateLearningCurves(double timeStep) {
    // Animate learning curves
    for (auto& curve : pImpl->hebbianCurves) {
        // Update curve position over time
        curve.x += static_cast<float>(timeStep * 0.01f);
        if (curve.x > pImpl->learningCurveTimeWindow) {
            curve.x -= pImpl->learningCurveTimeWindow;
        }
    }
}

bool PlasticityVisualizer::exportPlasticityData(const std::string& filepath) {
    // Export plasticity data to file
    return true;
}

bool PlasticityVisualizer::exportLearningHistory(const std::string& filepath) {
    // Export learning history to file
    return true;
}

} // namespace nlm

// ==============================================================================
// Plasticity Visualizer
// ==============================================================================
#ifndef PLASTICITYVISUALIZER_HPP
#define PLASTICITYVISUALIZER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

namespace nlm {
namespace debug {

/**
 * @class PlasticityVisualizer
 * @brief Plasticity visualization tool for NLM
 * 
 * Provides tools for visualizing plasticity rules including STDP, Hebbian,
 * structural plasticity, and rule interactions.
 */
class PlasticityVisualizer {
public:
    /**
     * @enum VisualizationMode
     * @brief Visualization mode
     */
    enum class VisualizationMode {
        WEIGHT_CHANGE,
        STDP_TRACE,
        HEBBIAN_TRACE,
        STRUCTURAL_CHANGES,
        RULE_INTERACTIONS,
        TEMPERATURE_EFFECTS,
        TIME_DEPENDENCE,
        SPACE_DEPENDENCE
    };

    /**
     * @enum PlasticityRuleType
     * @brief Type of plasticity rule
     */
    enum class PlasticityRuleType {
        STDP,
        HEBBIAN,
        STRUCTURAL,
        COMPOUND,
        ADAPTIVE,
        HOMEOSTATIC
    };

    /**
     * @struct PlasticityData
     * @brief Plasticity data
     */
    struct PlasticityData {
        double time;
        double weightChange;
        double tracePre;
        double tracePost;
        double eligibilityTrace;
        double synapticStrength;
        double connectionProbability;
        std::map<PlasticityRuleType, double> ruleContributions;
    };

    PlasticityVisualizer();
    ~PlasticityVisualizer();

    /**
     * @brief Visualize plasticity rule
     * @param rule Plasticity rule reference
     * @param filename Output file path
     * @param mode Visualization mode
     * @return true if visualization was created
     */
    bool visualize(const PlasticityRule& rule, const std::string& filename, VisualizationMode mode = VisualizationMode::WEIGHT_CHANGE);

    /**
     * @brief Visualize STDP rule
     * @param stdp STDP rule reference
     * @param filename Output file path
     * @param mode Visualization mode
     * @return true if visualization was created
     */
    bool visualize(const STDP& stdp, const std::string& filename, VisualizationMode mode = VisualizationMode::STDP_TRACE);

    /**
     * @brief Visualize Hebbian rule
     * @param hebbian Hebbian rule reference
     * @param filename Output file path
     * @param mode Visualization mode
     * @return true if visualization was created
     */
    bool visualize(const Hebbian& hebbian, const std::string& filename, VisualizationMode mode = VisualizationMode::HEBBIAN_TRACE);

    /**
     * @brief Visualize structural plasticity
     * @param structural Plasticity rule reference
     * @param filename Output file path
     * @param mode Visualization mode
     * @return true if visualization was created
     */
    bool visualize(const StructuralPlasticity& structural, const std::string& filename, VisualizationMode mode = VisualizationMode::STRUCTURAL_CHANGES);

    /**
     * @brief Generate plasticity trace
     * @param rule Plasticity rule reference
     * @param duration Duration
     * @param timeStep Time step
     * @return Plasticity data
     */
    std::vector<PlasticityData> generatePlasticityTrace(const PlasticityRule& rule, double duration, double timeStep = 0.001);

    /**
     * @brief Generate STDP trace
     * @param stdp STDP rule reference
     * @param preSpikeTime Pre-synaptic spike time
     * @param postSpikeTime Post-synaptic spike time
     * @param duration Duration
     * @param timeStep Time step
     * @return STDP trace data
     */
    std::vector<PlasticityData> generateSTDPTrace(const STDP& stdp, double preSpikeTime, double postSpikeTime, double duration, double timeStep = 0.001);

    /**
     * @brief Generate Hebbian trace
     * @param hebbian Hebbian rule reference
     * @param activityPre Pre-synaptic activity
     * @param activityPost Post-synaptic activity
     * @param duration Duration
     * @param timeStep Time step
     * @return Hebbian trace data
     */
    std::vector<PlasticityData> generateHebbianTrace(const Hebbian& hebbian, const std::vector<double>& activityPre, const std::vector<double>& activityPost, double duration, double timeStep = 0.001);

    /**
     * @brief Generate structural plasticity trace
     * @param structural Plasticity rule reference
     * @param timePoints Time points
     * @return Structural plasticity trace
     */
    std::vector<PlasticityData> generateStructuralPlasticityTrace(const StructuralPlasticity& structural, const std::vector<double>& timePoints);

    /**
     * @brief Analyze plasticity rule
     * @param rule Plasticity rule reference
     * @return Analysis result
     */
    struct AnalysisResult {
        double totalWeightChange;
        double averageWeightChange;
        double maxWeightChange;
        double minWeightChange;
        double weightChangeStability;
        double plasticityEfficiency;
        double ruleStability;
        std::map<std::string, double> ruleParameters;
    };

    AnalysisResult analyze(const PlasticityRule& rule);

    /**
     * @brief Compare plasticity rules
     * @param rule1 First plasticity rule
     * @param rule2 Second plasticity rule
     * @return Comparison result
     */
    struct ComparisonResult {
        double weightChangeDifference;
        double efficiencyDifference;
        double stabilityDifference;
        double convergenceRateDifference;
        double computationalCostDifference;
        std::string betterRule;
    };

    ComparisonResult compare(const PlasticityRule& rule1, const PlasticityRule& rule2);

    /**
     * @brief Generate plasticity visualization
     * @param rule Plasticity rule reference
     * @param filename Output file path
     */
    void generatePlasticityVisualization(const PlasticityRule& rule, const std::string& filename);

    /**
     * @brief Generate STDP visualization
     * @param stdp STDP rule reference
     * @param filename Output file path
     */
    void generateSTDPVisualization(const STDP& stdp, const std::string& filename);

    /**
     * @brief Generate Hebbian visualization
     * @param hebbian Hebbian rule reference
     * @param filename Output file path
     */
    void generateHebbianVisualization(const Hebbian& hebbian, const std::string& filename);

    /**
     * @brief Get plasticity rule types
     * @return Vector of plasticity rule types
     */
    std::vector<PlasticityRuleType> getAvailableRuleTypes() const;

    /**
     * @brief Get visualization modes
     * @return Vector of visualization modes
     */
    std::vector<VisualizationMode> getAvailableVisualizationModes() const;

    /**
     * @brief Format plasticity data for display
     * @param data Plasticity data
     * @return Formatted string
     */
    std::string formatPlasticityData(const PlasticityData& data) const;

    /**
     * @brief Format analysis result for display
     * @param result Analysis result
     * @return Formatted string
     */
    std::string formatAnalysisResult(const AnalysisResult& result) const;

    /**
     * @brief Format comparison result for display
     * @param result Comparison result
     * @return Formatted string
     */
    std::string formatComparisonResult(const ComparisonResult& result) const;

protected:
    /**
     * @brief Create STDP visualization
     * @param stdp STDP rule reference
     * @param filename Output file path
     */
    void createSTDPVisualization(const STDP& stdp, const std::string& filename);

    /**
     * @brief Create Hebbian visualization
     * @param hebbian Hebbian rule reference
     * @param filename Output file path
     */
    void createHebbianVisualization(const Hebbian& hebbian, const std::string& filename);

    /**
     * @brief Create structural visualization
     * @param structural Plasticity rule reference
     * @param filename Output file path
     */
    void createStructuralVisualization(const StructuralPlasticity& structural, const std::string& filename);

    /**
     * @brief Create weight change visualization
     * @param rule Plasticity rule reference
     * @param filename Output file path
     */
    void createWeightChangeVisualization(const PlasticityRule& rule, const std::string& filename);

    /**
     * @brief Create trace visualization
     * @param data Plasticity data
     * @param filename Output file path
     */
    void createTraceVisualization(const std::vector<PlasticityData>& data, const std::string& filename);

    /**
     * @brief Create 3D visualization
     * @param rule Plasticity rule reference
     * @param filename Output file path
     */
    void create3DVisualization(const PlasticityRule& rule, const std::string& filename);

    /**
     * @brief Calculate plasticity efficiency
     * @param rule Plasticity rule reference
     * @return Plasticity efficiency
     */
    double calculatePlasticityEfficiency(const PlasticityRule& rule) const;

    /**
     * @brief Calculate stability
     * @param rule Plasticity rule reference
     * @return Stability
     */
    double calculateStability(const PlasticityRule& rule) const;

    /**
     * @brief Calculate convergence rate
     * @param rule Plasticity rule reference
     * @return Convergence rate
     */
    double calculateConvergenceRate(const PlasticityRule& rule) const;

    /**
     * @brief Generate color map
     * @param value Value to map
     * @return Color
     */
    std::string generateColorMap(double value) const;

    /**
     * @brief Generate size map
     * @param value Value to map
     * @return Size
     */
    double generateSizeMap(double value) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace debug
} // namespace nlm

#endif // PLASTICITYVISUALIZER_HPP

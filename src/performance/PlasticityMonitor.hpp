#pragma once

/**
 * Plasticity Monitor
 * 
 * Plasticity rule performance tracking for the NLM brain simulation project.
 * 
 * Features:
 * - Plasticity rule execution time monitoring
 * - Plasticity rule efficiency analysis
 * - Long-term plasticity monitoring
 * - Short-term plasticity monitoring
 * - Synaptic weight change tracking
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class PlasticityMonitor {
public:
    /**
     * Plasticity rule execution record
     */
    struct PlasticityExecution {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string rule_type; // "Hebbian", "STDP", "structural"
        int pre_synaptic_neuron_id;
        int post_synaptic_neuron_id;
        double weight_change;
        double execution_time;
        double efficiency_score; // 0.0 to 1.0
        int thread_id;
        std::string plasticity_type; // "LTP", "LTD", "metaplasticity"
    };

    /**
     * Plasticity system metrics
     */
    struct PlasticityMetrics {
        double total_weight_change;
        double average_efficiency;
        double rule_execution_frequency; // rules per second
        double average_execution_time;
        int hebbian_executions;
        int stdp_executions;
        int structural_executions;
    };

    PlasticityMonitor();
    ~PlasticityMonitor();
    
    /** Record plasticity rule execution */
    void recordPlasticityExecution(const PlasticityExecution& execution);
    
    /** Get all plasticity executions */
    const std::vector<PlasticityExecution>& getPlasticityExecutions() const { return executions_; }
    
    /** Get plasticity metrics */
    const PlasticityMetrics& getPlasticityMetrics() const { return metrics_; }
    
    /** Calculate plasticity efficiency */
    double calculatePlasticityEfficiency() const;
    
    /** Get executions by rule type */
    std::unordered_map<std::string, int> getExecutionsByRuleType() const;
    
    /** Get executions by plasticity type */
    std::unordered_map<std::string, int> getExecutionsByPlasticityType() const;
    
    /** Calculate weight stabilization */
    double calculateWeightStabilization() const;
    
    /** Clear executions */
    void clear();
    
    /** Enable/disable long-term plasticity tracking */
    void enableLTPTracking(bool enable) { track_ltp_ = enable; }
    
    /** Enable/disable short-term plasticity tracking */
    void enableSTPTracking(bool enable) { track_stdp_ = enable; }
    
private:
    std::vector<PlasticityExecution> executions_;
    PlasticityMetrics metrics_;
    bool track_ltp_;
    bool track_stdp_;
    size_t max_executions_;
};

} // namespace nlm

#include "PlasticityAnalyzer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {
namespace analysis {

// PlasticityAnalyzer implementation
bool PlasticityAnalyzer::addPlasticityTrace(const SynapticPlasticityTrace& trace) {
    if (trace.events.empty()) {
        return false;
    }

    // Validate trace
    for (size_t i = 1; i < trace.events.size(); ++i) {
        if (trace.events[i].time < trace.events[i-1].time) {
            return false; // Time should be monotonic
        }
    }

    plasticity_traces_[trace.synapse_id] = trace;
    return true;
}

bool PlasticityAnalyzer::addSTDPProfile(const STDPProfile& profile) {
    if (profile.weight_changes.empty()) {
        return false;
    }

    stdp_profiles_[profile.synapse_id] = profile;
    return true;
}

bool PlasticityAnalyzer::addHomeostaticTrace(const HomeostaticPlasticity& trace) {
    if (trace.weight_deviation.empty()) {
        return false;
    }

    homeostatic_traces_[trace.synapse_id] = trace;
    return true;
}

std::vector<SynapticPlasticityTrace> PlasticityAnalyzer::getPlasticityTraces() const {
    std::vector<SynapticPlasticityTrace> traces;
    for (const auto& [synapse_id, trace] : plasticity_traces_) {
        traces.push_back(trace);
    }
    return traces;
}

double PlasticityAnalyzer::computeAveragePlasticityRate(const std::string& synapse_id) const {
    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        return 0.0;
    }

    double total_change = 0.0;
    double total_time = 0.0;

    for (size_t i = 1; i < it->second.events.size(); ++i) {
        total_change += std::abs(it->second.events[i].weight_change);
        total_time += (it->second.events[i].time - it->second.events[i-1].time);
    }

    return total_time > 0.0 ? total_change / total_time : 0.0;
}

double PlasticityAnalyzer::computePlasticityVariance(const std::string& synapse_id) const {
    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        return 0.0;
    }

    std::vector<double> weight_changes;
    for (const auto& event : it->second.events) {
        weight_changes.push_back(event.weight_change);
    }

    if (weight_changes.size() <= 1) {
        return 0.0;
    }

    double mean = std::accumulate(weight_changes.begin(), weight_changes.end(), 0.0) / weight_changes.size();
    double variance = 0.0;

    for (double change : weight_changes) {
        variance += (change - mean) * (change - mean);
    }

    return variance / weight_changes.size();
}

double PlasticityAnalyzer::computeTotalPlasticityChange(const std::string& synapse_id) const {
    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        return 0.0;
    }

    double total_change = 0.0;
    for (const auto& event : it->second.events) {
        total_change += event.weight_change;
    }

    return total_change;
}

std::vector<double> PlasticityAnalyzer::computePlasticityHistogram(const std::string& synapse_id,
                                                                   size_t bins) const {
    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        return std::vector<double>(bins, 0.0);
    }

    std::vector<double> weight_changes;
    for (const auto& event : it->second.events) {
        weight_changes.push_back(event.weight_change);
    }

    if (weight_changes.empty()) {
        return std::vector<double>(bins, 0.0);
    }

    double min_val = *std::min_element(weight_changes.begin(), weight_changes.end());
    double max_val = *std::max_element(weight_changes.begin(), weight_changes.end());
    double range = max_val - min_val;

    if (range == 0.0) {
        std::vector<double> histogram(bins, 0.0);
        histogram[0] = weight_changes.size();
        return histogram;
    }

    std::vector<double> histogram(bins, 0.0);

    for (double change : weight_changes) {
        size_t bin_index = static_cast<size_t>((change - min_val) / range * (bins - 1));
        if (bin_index >= bins) bin_index = bins - 1;
        histogram[bin_index]++;
    }

    // Normalize to probabilities
    double total = std::accumulate(histogram.begin(), histogram.end(), 0.0);
    if (total > 0.0) {
        for (auto& count : histogram) {
            count /= total;
        }
    }

    return histogram;
}

std::map<std::string, double> PlasticityAnalyzer::analyzeSTDPSymmetry(const std::string& synapse_id) const {
    auto it = stdp_profiles_.find(synapse_id);
    if (it == stdp_profiles_.end()) {
        return std::map<std::string, double>();
    }

    const STDPProfile& profile = it->second;

    double pre_post_sum = 0.0;
    double post_pre_sum = 0.0;
    size_t count = 0;

    // Analyze weight changes for symmetry
    for (double change : profile.weight_changes) {
        if (change > 0.0) {
            post_pre_sum += change;
        } else {
            pre_post_sum += std::abs(change);
        }
        count++;
    }

    std::map<std::string, double> symmetry;
    if (count > 0) {
        symmetry["post_pre_ratio"] = post_pre_sum / count;
        symmetry["pre_post_ratio"] = pre_post_sum / count;
        symmetry["net_change"] = (post_pre_sum - pre_post_sum) / count;
    }

    return symmetry;
}

std::map<std::string, double> PlasticityAnalyzer::analyzeSTDPTemporalProfile(const std::string& synapse_id) const {
    auto it = stdp_profiles_.find(synapse_id);
    if (it == stdp_profiles_.end()) {
        return std::map<std::string, double>();
    }

    const STDPProfile& profile = it->second;

    std::map<std::string, double> temporal;
    temporal["max_weight_change"] = 0.0;
    temporal["min_weight_change"] = 0.0;
    temporal["time_window"] = profile.time_window;

    if (!profile.weight_changes.empty()) {
        temporal["max_weight_change"] = *std::max_element(profile.weight_changes.begin(), profile.weight_changes.end());
        temporal["min_weight_change"] = *std::min_element(profile.weight_changes.begin(), profile.weight_changes.end());
    }

    return temporal;
}

std::map<std::string, double> PlasticityAnalyzer::analyzeSTDPConvergence(const std::string& synapse_id) const {
    auto it = stdp_profiles_.find(synapse_id);
    if (it == stdp_profiles_.end()) {
        return std::map<std::string, double>();
    }

    const STDPProfile& profile = it->second;

    // Check convergence by looking at changes in weight changes
    std::map<std::string, double> convergence;

    if (profile.weight_changes.size() >= 3) {
        // Compute variance of last few weight changes
        size_t tail_size = std::min(static_cast<size_t>(5), profile.weight_changes.size());
        std::vector<double> tail_changes(profile.weight_changes.end() - tail_size, profile.weight_changes.end());

        double tail_variance = 0.0;
        double tail_mean = std::accumulate(tail_changes.begin(), tail_changes.end(), 0.0) / tail_changes.size();

        for (double change : tail_changes) {
            tail_variance += (change - tail_mean) * (change - tail_mean);
        }

        tail_variance /= tail_changes.size();
        convergence["convergence_variance"] = tail_variance;
        convergence["is_converged"] = tail_variance < 0.01; // Threshold for convergence
    } else {
        convergence["convergence_variance"] = 0.0;
        convergence["is_converged"] = false;
    }

    return convergence;
}

std::map<std::string, double> PlasticityAnalyzer::analyzeSTDPLearningRate(const std::string& synapse_id) const {
    std::map<std::string, double> learning_rate;

    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        return learning_rate;
    }

    // Compute average learning rate over time
    double total_change = 0.0;
    double total_time = 0.0;

    for (size_t i = 1; i < it->second.events.size(); ++i) {
        total_change += std::abs(it->second.events[i].weight_change);
        total_time += (it->second.events[i].time - it->second.events[i-1].time);
    }

    if (total_time > 0.0) {
        learning_rate["average_rate"] = total_change / total_time;
    }

    // Compute learning rate variance
    std::vector<double> changes;
    for (const auto& event : it->second.events) {
        changes.push_back(event.weight_change);
    }

    if (changes.size() >= 2) {
        double mean = std::accumulate(changes.begin(), changes.end(), 0.0) / changes.size();
        double variance = 0.0;

        for (double change : changes) {
            variance += (change - mean) * (change - mean);
        }

        learning_rate["rate_variance"] = variance / changes.size();
    }

    return learning_rate;
}

std::map<std::string, double> PlasticityAnalyzer::analyzeHomeostaticPlasticity(const std::string& synapse_id) const {
    auto it = homeostatic_traces_.find(synapse_id);
    if (it == homeostatic_traces_.end()) {
        return std::map<std::string, double>();
    }

    const HomeostaticPlasticity& trace = it->second;

    std::map<std::string, double> homeostasis;

    if (!trace.weight_deviation.empty()) {
        double avg_deviation = std::accumulate(trace.weight_deviation.begin(), trace.weight_deviation.end(), 0.0) / trace.weight_deviation.size();
        double max_deviation = *std::max_element(trace.weight_deviation.begin(), trace.weight_deviation.end());
        double min_deviation = *std::min_element(trace.weight_deviation.begin(), trace.weight_deviation.end());

        homeostasis["average_deviation"] = avg_deviation;
        homeostasis["max_deviation"] = max_deviation;
        homeostasis["min_deviation"] = min_deviation;
        homeostasis["correction_efficiency"] = (max_deviation - min_deviation) / std::max(std::abs(avg_deviation), 0.1);
    }

    return homeostasis;
}

std::map<std::string, double> PlasticityAnalyzer::analyzeMetaplasticChanges(const std::string& synapse_id) const {
    std::map<std::string, double> metaplastic;

    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        return metaplastic;
    }

    const SynapticPlasticityTrace& trace = it->second;

    // Detect metaplastic events (large, unexpected weight changes)
    std::vector<double> weight_changes;
    for (const auto& event : trace.events) {
        weight_changes.push_back(event.weight_change);
    }

    if (weight_changes.empty()) {
        return metaplastic;
    }

    double mean_change = std::accumulate(weight_changes.begin(), weight_changes.end(), 0.0) / weight_changes.size();
    double std_dev = 0.0;

    for (double change : weight_changes) {
        std_dev += (change - mean_change) * (change - mean_change);
    }
    std_dev = std::sqrt(std_dev / weight_changes.size());

    // Identify metaplastic events (changes > 3 standard deviations)
    double threshold = 3.0 * std_dev;
    int metaplastic_count = 0;

    for (double change : weight_changes) {
        if (std::abs(change - mean_change) > threshold) {
            metaplastic_count++;
        }
    }

    metaplastic["metaplastic_event_count"] = metaplastic_count;
    metaplastic["metaplastic_probability"] = static_cast<double>(metaplastic_count) / weight_changes.size();

    return metaplastic;
}

std::map<std::string, double> PlasticityAnalyzer::detectPlasticityRegimes(const std::string& synapse_id) const {
    std::map<std::string, double> regimes;

    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        return regimes;
    }

    const SynapticPlasticityTrace& trace = it->second;

    // Classify regime based on weight change patterns
    std::vector<double> weight_changes;
    for (const auto& event : trace.events) {
        weight_changes.push_back(event.weight_change);
    }

    if (weight_changes.empty()) {
        return regimes;
    }

    // Compute statistics
    double mean_change = std::accumulate(weight_changes.begin(), weight_changes.end(), 0.0) / weight_changes.size();
    double positive_changes = 0;
    double negative_changes = 0;
    double large_changes = 0;

    for (double change : weight_changes) {
        if (change > 0) {
            positive_changes++;
        } else if (change < 0) {
            negative_changes++;
        }
        if (std::abs(change) > 0.1) {
            large_changes++;
        }
    }

    regimes["mean_change"] = mean_change;
    regimes["positive_change_ratio"] = positive_changes / weight_changes.size();
    regimes["negative_change_ratio"] = negative_changes / weight_changes.size();
    regimes["large_change_ratio"] = large_changes / weight_changes.size();

    // Determine regime type
    if (regimes["large_change_ratio"] > 0.3) {
        regimes["regime"] = "unstable";
    } else if (regimes["positive_change_ratio"] > 0.7) {
        regimes["regime"] = "potentiation";
    } else if (regimes["negative_change_ratio"] > 0.7) {
        regimes["regime"] = "depression";
    } else {
        regimes["regime"] = "balanced";
    }

    return regimes;
}

std::map<std::string, double> PlasticityAnalyzer::identifyPlasticityBounds(const std::string& synapse_id) const {
    std::map<std::string, double> bounds;

    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        return bounds;
    }

    const SynapticPlasticityTrace& trace = it->second;

    // Find bounds (min and max observed weight changes)
    double min_bound = std::numeric_limits<double>::max();
    double max_bound = std::numeric_limits<double>::lowest();

    for (const auto& event : trace.events) {
        min_bound = std::min(min_bound, event.weight_change);
        max_bound = std::max(max_bound, event.weight_change);
    }

    bounds["min_bound"] = min_bound;
    bounds["max_bound"] = max_bound;
    bounds["bound_range"] = max_bound - min_bound;

    // Check for saturation (approaching bounds)
    double current_weight = 0.0;
    if (!trace.weight_history.empty()) {
        current_weight = trace.weight_history.back();
    }

    bounds["weight_saturation"] = std::min(std::abs(current_weight - min_bound), 
                                          std::abs(current_weight - max_bound));

    return bounds;
}

std::map<std::string, double> PlasticityAnalyzer::analyzePlasticityCriticality(const std::string& synapse_id) const {
    std::map<std::string, double> criticality;

    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        return criticality;
    }

    const SynapticPlasticityTrace& trace = it->second;

    // Compute criticality metrics
    std::vector<double> weight_changes;
    for (const auto& event : trace.events) {
        weight_changes.push_back(event.weight_change);
    }

    if (weight_changes.empty()) {
        return criticality;
    }

    // Compute Lyapunov exponent approximation
    double total_change = 0.0;
    for (double change : weight_changes) {
        total_change += std::abs(change);
    }

    criticality["total_change_magnitude"] = total_change;
    criticality["event_count"] = trace.events.size();

    // Estimate critical state (based on change magnitude)
    double criticality_score = std::min(total_change / 10.0, 1.0); // Normalize
    criticality["criticality_score"] = criticality_score;

    return criticality;
}

double PlasticityAnalyzer::computeNetworkPlasticityDynamism() const {
    // Compute overall network plasticity dynamism
    double total_dynamism = 0.0;
    size_t synapse_count = 0;

    for (const auto& [synapse_id, trace] : plasticity_traces_) {
        if (!trace.events.empty()) {
            total_dynamism += computeAveragePlasticityRate(synapse_id);
            synapse_count++;
        }
    }

    return synapse_count > 0 ? total_dynamism / synapse_count : 0.0;
}

std::map<std::string, double> PlasticityAnalyzer::analyzePlasticitySpread(const std::string& source_synapse) const {
    // In real implementation, would analyze how plasticity spreads through network
    // For now, return placeholder values
    std::map<std::string, double> spread;
    spread["source_weight_change"] = computeTotalPlasticityChange(source_synapse);
    spread["average_spread_rate"] = computeNetworkPlasticityDynamism() / 10.0; // Placeholder
    return spread;
}

std::map<std::string, double> PlasticityAnalyzer::computePlasticityCorrelation(const std::string& synapse_id1,
                                                                const std::string& synapse_id2) const {
    // In real implementation, would compute correlation between plasticity traces
    // For now, return placeholder values
    std::map<std::string, double> correlation;
    correlation["correlation_coefficient"] = 0.5; // Placeholder
    correlation["correlation_strength"] = "moderate"; // Placeholder
    return correlation;
}

bool PlasticityAnalyzer::validatePlasticityModel(const std::string& synapse_id) const {
    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end()) {
        return false;
    }

    // Basic validation: check for reasonable weight changes
    for (const auto& event : it->second.events) {
        if (std::abs(event.weight_change) > 1.0) { // Weight changes should be reasonable
            return false;
        }
    }

    return true;
}

std::map<std::string, double> PlasticityAnalyzer::performPlasticityGoodnessOfFit(const std::string& synapse_id) const {
    std::map<std::string, double> goodness_of_fit;

    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        return goodness_of_fit;
    }

    // Compute goodness of fit metrics
    std::vector<double> weight_changes;
    for (const auto& event : it->second.events) {
        weight_changes.push_back(event.weight_change);
    }

    if (weight_changes.empty()) {
        return goodness_of_fit;
    }

    double mean_change = std::accumulate(weight_changes.begin(), weight_changes.end(), 0.0) / weight_changes.size();
    double sum_squared_errors = 0.0;

    for (double change : weight_changes) {
        sum_squared_errors += (change - mean_change) * (change - mean_change);
    }

    double variance = sum_squared_errors / weight_changes.size();
    double r_squared = 1.0 - (sum_squared_errors / (weight_changes.size() * variance));

    goodness_of_fit["r_squared"] = r_squared;
    goodness_of_fit["model_fit"] = (r_squared > 0.5) ? "good" : "poor";

    return goodness_of_fit;
}

std::vector<std::string> PlasticityAnalyzer::getPlasticityModelValidationErrors(const std::string& synapse_id) const {
    std::vector<std::string> errors;

    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end()) {
        errors.push_back("Plasticity trace not found for synapse: " + synapse_id);
        return errors;
    }

    // Validate plasticity trace
    const SynapticPlasticityTrace& trace = it->second;

    if (trace.events.empty()) {
        errors.push_back("No plasticity events recorded for synapse: " + synapse_id);
    }

    for (const auto& event : trace.events) {
        if (event.time < 0.0) {
            errors.push_back("Invalid time in plasticity event for synapse: " + synapse_id);
        }
        if (std::isnan(event.weight_change) || std::isinf(event.weight_change)) {
            errors.push_back("Invalid weight change in plasticity event for synapse: " + synapse_id);
        }
    }

    return errors;
}

std::string PlasticityAnalyzer::generatePlasticityTracePlot(const std::string& synapse_id) const {
    std::stringstream plot;
    plot << "Plasticity Trace Plot for Synapse: " << synapse_id << std::endl;
    plot << "============================================" << std::endl;

    auto it = plasticity_traces_.find(synapse_id);
    if (it == plasticity_traces_.end() || it->second.events.empty()) {
        plot << "No data available for this synapse." << std::endl;
        return plot.str();
    }

    const SynapticPlasticityTrace& trace = it->second;

    plot << "Number of events: " << trace.events.size() << std::endl;
    plot << "Average plasticity rate: " << computeAveragePlasticityRate(synapse_id) << std::endl;
    plot << "Total weight change: " << computeTotalPlasticityChange(synapse_id) << std::endl;

    plot << std::endl << "Event Details:" << std::endl;
    for (size_t i = 0; i < trace.events.size() && i < 10; ++i) {
        const auto& event = trace.events[i];
        plot << "  Event " << i << ": Time=" << event.time 
             << ", Change=" << event.weight_change 
             << ", Rule=" << event.rule_type << std::endl;
    }

    if (trace.events.size() > 10) {
        plot << "  ... and " << (trace.events.size() - 10) << " more events" << std::endl;
    }

    return plot.str();
}

std::string PlasticityAnalyzer::generatePlasticityHeatmap() const {
    std::stringstream heatmap;
    heatmap << "Plasticity Heatmap" << std::endl;
    heatmap << "===================" << std::endl;
    heatmap << "(In real implementation, would generate interactive heatmap)" << std::endl;
    heatmap << "Showing plasticity dynamics across multiple synapses." << std::endl;

    // In real implementation, would generate actual heatmap
    heatmap << "[Visualization data would be displayed here]" << std::endl;

    return heatmap.str();
}

std::string PlasticityAnalyzer::generatePlasticityTimeSeries() const {
    std::stringstream time_series;
    time_series << "Plasticity Time Series" << std::endl;
    time_series << "======================" << std::endl;
    time_series << "(In real implementation, would generate time series plots)" << std::endl;

    return time_series.str();
}

bool PlasticityAnalyzer::exportPlasticityData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        return false;
    }

    try {
        file << "Plasticity Data Export" << std::endl;
        file << "======================" << std::endl;
        file << "Total synapses: " << plasticity_traces_.size() << std::endl;
        file << std::endl;

        for (const auto& [synapse_id, trace] : plasticity_traces_) {
            file << "Synapse: " << synapse_id << std::endl;
            file << "  Number of events: " << trace.events.size() << std::endl;
            file << "  Average rate: " << computeAveragePlasticityRate(synapse_id) << std::endl;
            file << "  Total change: " << computeTotalPlasticityChange(synapse_id) << std::endl;
            file << std::endl;
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting plasticity data: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

std::unique_ptr<PlasticityAnalyzer> PlasticityAnalyzer::importPlasticityData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        return nullptr;
    }

    // In real implementation, would parse plasticity data from file
    file.close();
    return std::make_unique<PlasticityAnalyzer>();
}

} // namespace analysis
} // namespace nlm
// src/debug/SpikeProfiler.cpp
#include "SpikeProfiler.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

namespace nlm {
namespace debug {

SpikeProfiler::SpikeProfiler() : is_profiling_(false) {
    session_start_ = std::chrono::high_resolution_clock::now();
    session_id_ = generateSessionId();
}

SpikeProfiler::~SpikeProfiler() {
    if (is_profiling_) {
        stopProfiling();
    }
}

void SpikeProfiler::recordSpike(NeuronId neuronId, Timestamp time, SimulationStep step, SpikeType type) {
    SpikeEvent event;
    event.neuron_id = neuronId;
    event.time = time;
    event.step = step;
    event.type = type;
    
    spike_events_.push_back(event);
}

void SpikeProfiler::recordPreSpike(SynapseId synapseId, NeuronId neuronId, Timestamp time) {
    PrePostPair pair;
    pair.synapse_id = synapseId;
    pair.pre_neuron_id = neuronId;
    pair.pre_time = time;
    pair.time_difference = 0.0; // Will be calculated when post spike recorded
    
    pre_post_pairs_.push_back(pair);
}

void SpikeProfiler::recordPostSpike(SynapseId synapseId, NeuronId neuronId, Timestamp time) {
    // Find matching pre spike for this synapse and neuron
    for (auto& pair : pre_post_pairs_) {
        if (pair.synapse_id == synapseId && pair.pre_neuron_id == neuronId && pair.pre_time > 0) {
            pair.post_neuron_id = neuronId;
            pair.post_time = time;
            pair.time_difference = time - pair.pre_time;
            return;
        }
    }
}

void SpikeProfiler::generateSTDPReport(const std::string& output_file) const {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        return;
    }
    
    file << "STDP Analysis Report" << std::endl;
    file << "=======================" << std::endl;
    file << "Session ID: " << session_id_ << std::endl;
    file << "Session duration: " << getSessionDuration() << " ms" << std::endl;
    file << "Total spikes recorded: " << spike_events_.size() << std::endl;
    file << "Total pre-post pairs: " << pre_post_pairs_.size() << std::endl;
    
    // Analyze STDP timing
    double max_time_diff = 0.0;
    double min_time_diff = 0.0;
    double sum_time_diff = 0.0;
    int valid_pairs = 0;
    
    for (const auto& pair : pre_post_pairs_) {
        if (pair.post_time > 0 && pair.pre_time > 0) {
            double diff = pair.time_difference;
            if (valid_pairs == 0) {
                max_time_diff = min_time_diff = diff;
            } else {
                max_time_diff = std::max(max_time_diff, diff);
                min_time_diff = std::min(min_time_diff, diff);
            }
            sum_time_diff += diff;
            valid_pairs++;
        }
    }
    
    file << "\nSTDP Timing Analysis:" << std::endl;
    if (valid_pairs > 0) {
        file << "  Valid pre-post pairs: " << valid_pairs << std::endl;
        file << "  Average time difference: " << (sum_time_diff / valid_pairs) << " ms" << std::endl;
        file << "  Min time difference: " << min_time_diff << " ms" << std::endl;
        file << "  Max time difference: " << max_time_diff << " ms" << std::endl;
        
        // Categorize timing for STDP
        int early_ltp = 0, late_ltp = 0, early_ltd = 0, late_ltd = 0;
        
        for (const auto& pair : pre_post_pairs_) {
            if (pair.post_time > 0 && pair.pre_time > 0) {
                double diff = pair.time_difference;
                if (diff < 10.0) {  // Early post (within 10ms)
                    if (diff < 5.0) early_ltp++;  // Strong LTP
                    else late_ltp++;           // Weak LTP
                } else if (diff < 50.0) {
                    early_ltd++;              // Early LTD
                } else {
                    late_ltd++;               // Late LTD
                }
            }
        }
        
        file << "\nSTDP Pairing Types:" << std::endl;
        file << "  Pre-before-post (LTP): " << (early_ltp + late_ltp) << " pairs" << std::endl;
        file << "    Early LTP (<5ms): " << early_ltp << std::endl;
        file << "    Late LTP (5-10ms): " << late_ltp << std::endl;
        file << "  Pre-after-post (LTD): " << (early_ltd + late_ltd) << " pairs" << std::endl;
        file << "    Early LTD (10-50ms): " << early_ltd << std::endl;
        file << "    Late LTD (>50ms): " << late_ltd << std::endl;
    } else {
        file << "  No valid pre-post pairs for STDP analysis" << std::endl;
    }
    
    file.close();
}

void SpikeProfiler::generateSpikeActivityReport(const std::string& output_file) const {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        return;
    }
    
    file << "Spike Activity Report" << std::endl;
    file << "======================" << std::endl;
    file << "Session ID: " << session_id_ << std::endl;
    file << "Session duration: " << getSessionDuration() << " ms" << std::endl;
    file << "Total spikes: " << spike_events_.size() << std::endl;
    
    // Group spikes by neuron
    std::unordered_map<NeuronId, int> spike_count_by_neuron;
    std::unordered_map<NeuronId, double> firing_rate_by_neuron;
    
    for (const auto& event : spike_events_) {
        spike_count_by_neuron[event.neuron_id]++;
    }
    
    double session_duration_ms = getSessionDuration();
    double total_time_seconds = session_duration_ms / 1000.0;
    
    file << "\nSpike Count by Neuron:" << std::endl;
    for (const auto& pair : spike_count_by_neuron) {
        double firing_rate = (pair.second / total_time_seconds) * 1000.0; // Hz
        firing_rate_by_neuron[pair.first] = firing_rate;
        
        file << "  Neuron " << pair.first << ": " << pair.second << " spikes ("
             << firing_rate << " Hz)" << std::endl;
    }
    
    // Find most active neurons
    file << "\nTop 10 Most Active Neurons:" << std::endl;
    std::vector<std::pair<NeuronId, int>> neuron_spikes(spike_count_by_neuron.begin(), spike_count_by_neuron.end());
    std::sort(neuron_spikes.begin(), neuron_spikes.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < std::min(size_t(10), neuron_spikes.size()); ++i) {
        file << "  " << (i + 1) << ". Neuron " << neuron_spikes[i].first
             << ": " << neuron_spikes[i].second << " spikes"
             << " (" << firing_rate_by_neuron[neuron_spikes[i].first] << " Hz)" << std::endl;
    }
    
    file.close();
}

void SpikeProfiler::generatePerformanceReport(const std::string& output_file) const {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        return;
    }
    
    double session_duration_ms = getSessionDuration();
    
    file << "Performance Report" << std::endl;
    file << "===================" << std::endl;
    file << "Session ID: " << session_id_ << std::endl;
    file << "Session duration: " << session_duration_ms << " ms" << std::endl;
    file << "Total spikes: " << spike_events_.size() << std::endl;
    file << "Average spike rate: " << (spike_events_.size() / session_duration_ms) << " spikes/ms" << std::endl;
    file << "\nSpike Type Distribution:" << std::endl;
    
    std::unordered_map<SpikeType, int> type_counts;
    for (const auto& event : spike_events_) {
        type_counts[event.type]++;
    }
    
    for (const auto& pair : type_counts) {
        std::string type_str;
        switch (pair.first) {
            case SpikeType::Regular: type_str = "Regular"; break;
            case SpikeType::Burst: type_str = "Burst"; break;
            case SpikeType::Adaptive: type_str = "Adaptive"; break;
            case SpikeType::Modulated: type_str = "Modulated"; break;
        }
        file << "  " << type_str << ": " << pair.second << " spikes" << std::endl;
    }
    
    file.close();
}

void SpikeProfiler::startProfiling() {
    if (!is_profiling_) {
        is_profiling_ = true;
        session_start_ = std::chrono::high_resolution_clock::now();
        session_id_ = generateSessionId();
    }
}

void SpikeProfiler::stopProfiling() {
    if (is_profiling_) {
        is_profiling_ = false;
    }
}

double SpikeProfiler::getSessionDuration() const {
    if (!is_profiling_) {
        // Calculate from recorded spike times
        if (spike_events_.empty()) return 0.0;
        double min_time = spike_events_[0].time;
        double max_time = spike_events_[0].time;
        
        for (const auto& event : spike_events_) {
            min_time = std::min(min_time, event.time);
            max_time = std::max(max_time, event.time);
        }
        return max_time - min_time;
    } else {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(now - session_start_).count();
    }
}

std::string SpikeProfiler::generateSessionId() {
    auto now = std::chrono::high_resolution_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return "session_" + std::to_string(now_ms.count());
}

} // namespace debug
} // namespace nlm

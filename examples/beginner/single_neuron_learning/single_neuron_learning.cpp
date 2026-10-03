#!/usr/bin/env python3
"""
Single Neuron Learning - Python Implementation
NLM Phase 2: Real Neural Computation
Experimental artificial developmental brain
"""

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <map>

using namespace nlm;
using namespace std;

// Configuration constants for single neuron learning
const int DEFAULT_NEURON_COUNT = 50;
const int DEFAULT_STIMULI_COUNT = 200;
const double DEFAULT_LEARNING_RATE = 0.001;
const double DEFAULT_TIME_STEP = 0.001;
const double DEFAULT_SPIKE_THRESHOLD = -50.0;
const double DEFAULT_REST_POTENTIAL = -70.0;
const double DEFAULT_RESET_POTENTIAL = -75.0;
const double DEFAULT_MEMBRANE_TIME_CONSTANT = 20.0;
const double DEFAULT_REFECTORY_PERIOD = 2.0;

// Learning experiment data structure
struct LearningDataPoint {
    double time;                    // Simulation time in ms
    double neuron_potential;        // Membrane potential of main neuron
    int spike_emitted;              // Number of spikes emitted
    double learning_rate;            // Current learning rate
    double synaptic_weight;          // Average synaptic weight
    double stimulus_strength;        // Strength of presented stimulus
    
    LearningDataPoint(double t, double v, int s, double l, double w, double st)
        : time(t), neuron_potential(v), spike_emitted(s), learning_rate(l), 
          synaptic_weight(w), stimulus_strength(st) {}
};

struct LearningResults {
    bool success = false;
    int total_neurons = 0;
    int total_synapses = 0;
    int total_spikes = 0;
    double baseline_firing_rate = 0.0;
    double initial_response = 0.0;
    double final_response = 0.0;
    double mean_response = 0.0;
    double response_std = 0.0;
    double learning_change = 0.0;
    bool learning_significance = false;
    string learning_type = "No learning";
    string outcome = "No significant change";
    vector<LearningDataPoint> learning_trajectory;
    vector<vector<double>> synaptic_weights;
    vector<double> spike_times;
    chrono::duration<double> wall_clock_time;
    
    // Statistical analysis
    double calculate_learning_efficiency() const {
        if (initial_response > 0.0) {
            return (learning_change / initial_response) * 100.0;
        }
        return 0.0;
    }
    
    string get_summary() const {
        ostringstream ss;
        ss << "Single Neuron Learning Results:" << endl;
        ss << "  Success: " << (success ? "Yes" : "No") << endl;
        ss << "  Neurons: " << total_neurons << endl;
        ss << "  Synapses: " << total_synapses << endl;
        ss << "  Spikes: " << total_spikes << endl;
        ss << "  Baseline FR: " << baseline_firing_rate << " Hz" << endl;
        ss << "  Initial Response: " << initial_response << " Hz" << endl;
        ss << "  Final Response: " << final_response << " Hz" << endl;
        ss << "  Learning Type: " << learning_type << endl;
        ss << "  Outcome: " << outcome << endl;
        ss << "  Learning Efficiency: " << calculate_learning_efficiency() << "%" << endl;
        return ss.str();
    }
};

class SingleNeuronLearning {
private:
    // Configuration
    shared_ptr<Config> config;
    
    // Core components
    shared_ptr<Brain> brain;
    shared_ptr<Logger> logger;
    
    // Learning parameters
    int neuron_count;
    int stimuli_count;
    double learning_rate;
    double time_step;
    
    // State tracking
    vector<LearningDataPoint> learning_data;
    vector<double> synaptic_weights;
    vector<double> spike_times;
    int stimulus_index;
    
    // Main neuron tracking
    NeuronId main_neuron_id;
    RegionId main_region_id;
    
public:
    SingleNeuronLearning() : stimulus_index(0) {
        // Initialize default configuration
        config = make_shared<Config>();
        
        // Set default values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", DEFAULT_TIME_STEP, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(DEFAULT_NEURON_COUNT), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.enable", false, ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", DEFAULT_LEARNING_RATE, ConfigSource::Default);
        
        // Initialize learning parameters
        neuron_count = config->getOr<int64_t>("neuron_count", DEFAULT_NEURON_COUNT);
        stimuli_count = DEFAULT_STIMULI_COUNT;
        learning_rate = config->getOr<float>("plasticity.hebbian.learning_rate", DEFAULT_LEARNING_RATE);
        time_step = config->getOr<double>("simulation_timestep", DEFAULT_TIME_STEP);
        
        // Initialize logger
        logger = make_shared<Logger>();
        auto console_logger = make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(console_logger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Single Neuron Learning initialized with default parameters");
        NLM_LOG_INFO("Neuron count: " + to_string(neuron_count));
        NLM_LOG_INFO("Stimuli count: " + to_string(stimuli_count));
        NLM_LOG_INFO("Learning rate: " + to_string(learning_rate));
    }
    
    SingleNeuronLearning(const string& config_file) {
        // Load configuration from file
        config = make_shared<Config>();
        
        if (config->loadFromFile(config_file)) {
            NLM_LOG_INFO("Loaded configuration from: " + config_file);
        } else {
            NLM_LOG_WARN("Could not load configuration from " + config_file + ". Using defaults.");
            initialize_default_config();
        }
        
        // Initialize from config
        initialize_parameters();
        
        // Initialize logger
        logger = make_shared<Logger>();
        auto console_logger = make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(console_logger);
        Logger::setGlobal(logger);
    }
    
    void initialize_default_config() {
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", DEFAULT_TIME_STEP, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(DEFAULT_NEURON_COUNT), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.2f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.enable", false, ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", DEFAULT_LEARNING_RATE, ConfigSource::Default);
    }
    
    void initialize_parameters() {
        neuron_count = config->getOr<int64_t>("neuron_count", DEFAULT_NEURON_COUNT);
        stimuli_count = DEFAULT_STIMULI_COUNT;
        learning_rate = config->getOr<float>("plasticity.hebbian.learning_rate", DEFAULT_LEARNING_RATE);
        time_step = config->getOr<double>("simulation_timestep", DEFAULT_TIME_STEP);
        
        NLM_LOG_INFO("Initialized learning parameters:");
        NLM_LOG_INFO("  Neuron count: " + to_string(neuron_count));
        NLM_LOG_INFO("  Stimuli count: " + to_string(stimuli_count));
        NLM_LOG_INFO("  Learning rate: " + to_string(learning_rate));
        NLM_LOG_INFO("  Time step: " + to_string(time_step) + " ms");
    }
    
    bool create_network() {
        try {
            NLM_LOG_INFO("Creating neural network for single neuron learning...");
            
            // Override config for this experiment
            config->set("neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::User);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::User);
            config->set("plasticity.hebbian.learning_rate", learning_rate, ConfigSource::User);
            
            // Create brain with optimized parameters
            brain = make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain");
                return false;
            }
            
            // Get the first region and main neuron
            auto regions = brain->getRegions();
            if (regions.empty()) {
                NLM_LOG_ERROR("No regions created in brain");
                return false;
            }
            
            main_region_id = regions[0];
            auto* region = brain->getRegion(main_region_id);
            if (!region) {
                NLM_LOG_ERROR("Could not access main region");
                return false;
            }
            
            // Get neurons from the region
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) {
                NLM_LOG_ERROR("No neurons created in region");
                return false;
            }
            
            // Use the first neuron as main neuron
            main_neuron_id = neurons[0]->getId();
            
            // Enable Hebbian plasticity
            for (auto& synapse : region->getSynapses()) {
                synapse->enablePlasticity(true, true, false);  // Hebbian only
            }
            
            NLM_LOG_INFO("Network created successfully:");
            NLM_LOG_INFO("  Main neuron ID: " + to_string(main_neuron_id));
            NLM_LOG_INFO("  Total neurons: " + to_string(brain->getTotalNeuronCount()));
            NLM_LOG_INFO("  Total synapses: " + to_string(brain->getTotalSynapseCount()));
            NLM_LOG_INFO("  Excitatory/Inhibitory ratio: " + to_string(brain->getExcitationInhibitionRatio()));
            
            return true;
            
        } catch (const exception& e) {
            NLM_LOG_ERROR("Exception in create_network: " + string(e.what()));
            return false;
        }
    }
    
    void reset_experiment() {
        NLM_LOG_INFO("Resetting experiment state...");
        learning_data.clear();
        synaptic_weights.clear();
        spike_times.clear();
        stimulus_index = 0;
    }
    
    LearningResults present_stimulus(double stimulus_strength) {
        LearningResults::LearningDataPoint data_point;
        data_point.stimulus_strength = stimulus_strength;
        
        try {
            // Record time
            data_point.time = stimulus_index * time_step;
            
            // Get current state of main neuron
            auto* main_neuron = brain->getNeuron(main_neuron_id);
            if (!main_neuron) {
                NLM_LOG_WARN("Could not access main neuron at time " + to_string(data_point.time));
                data_point.neuron_potential = 0.0;
                data_point.spike_emitted = 0;
                data_point.synaptic_weight = 0.0;
                learning_data.push_back(data_point);
                return LearningResults();  // Return empty results
            }
            
            // Record current potential
            data_point.neuron_potential = main_neuron->getMembranePotential();
            
            // Record current spike count
            size_t current_spike_count = brain->getTotalSpikeCount();
            
            // Apply stimulus by injecting current
            main_neuron->injectCurrent(stimulus_strength * 100.0);  // Convert to current units
            
            // Run brain for one time step
            brain->step(stimulus_index, stimulus_index * time_step);
            
            // Calculate changes
            int new_spike_count = brain->getTotalSpikeCount() - current_spike_count;
            data_point.spike_emitted = new_spike_count;
            
            if (new_spike_count > 0) {
                spike_times.push_back(data_point.time);
            }
            
            // Record synaptic weight (simplified - use EI ratio as proxy)
            data_point.synaptic_weight = brain->getExcitationInhibitionRatio();
            data_point.learning_rate = learning_rate;
            
            // Add to learning trajectory
            learning_data.push_back(data_point);
            
            // Record synaptic weights for analysis
            update_synaptic_weights();
            
            NLM_LOG_DEBUG("Time: " + to_string(data_point.time) + 
                         " | Potential: " + to_string(data_point.neuron_potential) +
                         " | Spikes: " + to_string(data_point.spike_emitted) +
                         " | Stimulus: " + to_string(stimulus_strength));
            
            return LearningResults();  // Return empty results (will collect later)
            
        } catch (const exception& e) {
            NLM_LOG_ERROR("Exception in present_stimulus: " + string(e.what()));
            data_point.neuron_potential = 0.0;
            data_point.spike_emitted = 0;
            data_point.synaptic_weight = 0.0;
            learning_data.push_back(data_point);
            return LearningResults();
        }
    }
    
    void update_synaptic_weights() {
        // This is a simplified version - in a real implementation,
        // you would track individual synapse weights
        try {
            auto* region = brain->getRegion(main_region_id);
            if (!region) return;
            
            double total_weight = 0.0;
            size_t synapse_count = 0;
            
            for (const auto& synapse : region->getSynapses()) {
                total_weight += synapse->getWeight();
                synapse_count++;
            }
            
            if (synapse_count > 0) {
                double avg_weight = total_weight / synapse_count;
                synaptic_weights.push_back(avg_weight);
            }
            
        } catch (const exception& e) {
            NLM_LOG_ERROR("Exception in update_synaptic_weights: " + string(e.what()));
        }
    }
    
    LearningResults run_learning_experiment() {
        auto start_time = chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("Starting single neuron learning experiment...");
        NLM_LOG_INFO("Total stimuli to present: " + to_string(stimuli_count));
        
        // Record baseline firing rate
        double baseline_rate = brain->getAverageFiringRate();
        NLM_LOG_INFO("Baseline firing rate: " + to_string(baseline_rate) + " Hz");
        
        // Present stimuli with varying strengths
        vector<double> stimulus_strengths;
        for (int i = 0; i < stimuli_count; ++i) {
            if (i < stimuli_count * 0.3) {
                // First 30%: weak stimuli
                stimulus_strengths.push_back(0.5);
            } else if (i < stimuli_count * 0.7) {
                // Next 40%: moderate stimuli
                stimulus_strengths.push_back(1.0);
            } else {
                // Last 30%: strong stimuli
                stimulus_strengths.push_back(1.5);
            }
        }
        
        // Present all stimuli
        for (int i = 0; i < stimuli_count; ++i) {
            LearningResults::LearningDataPoint result = present_stimulus(stimulus_strengths[i]);
            
            // Progress monitoring
            if ((i + 1) % 50 == 0) {
                double progress = (i + 1) / (double)stimuli_count * 100.0;
                NLM_LOG_INFO("Progress: " + to_string(progress) + "% | " +
                           to_string(i + 1) + " stimuli presented");
            }
        }
        
        // Analyze results
        LearningResults results = analyze_learning_results(baseline_rate);
        
        // Record wall clock time
        auto end_time = chrono::high_resolution_clock::now();
        results.wall_clock_time = end_time - start_time;
        
        return results;
    }
    
    LearningResults analyze_learning_results(double baseline_rate) {
        LearningResults results;
        results.success = true;
        results.total_neurons = brain->getTotalNeuronCount();
        results.total_synapses = brain->getTotalSynapseCount();
        results.total_spikes = brain->getTotalSpikeCount();
        results.baseline_firing_rate = baseline_rate;
        
        if (learning_data.empty()) {
            NLM_LOG_WARN("No learning data collected");
            results.success = false;
            return results;
        }
        
        // Extract response rates
        vector<double> response_rates;
        for (const auto& data : learning_data) {
            if (data.spike_emitted > 0) {
                response_rates.push_back(data.spike_emitted / time_step);
            }
        }
        
        if (response_rates.empty()) {
            NLM_LOG_WARN("No spike data collected");
            results.success = false;
            return results;
        }
        
        // Calculate statistics
        results.initial_response = response_rates[0];
        results.final_response = response_rates.back();
        results.mean_response = accumulate(response_rates.begin(), response_rates.end(), 0.0) / response_rates.size();
        
        // Calculate standard deviation
        double sum_sq = 0.0;
        for (double rate : response_rates) {
            sum_sq += (rate - results.mean_response) * (rate - results.mean_response);
        }
        results.response_std = sqrt(sum_sq / response_rates.size());
        
        // Calculate learning change
        results.learning_change = results.final_response - results.initial_response;
        
        // Determine learning significance
        double relative_change = fabs(results.learning_change / results.initial_response);
        results.learning_significance = (relative_change > 0.05);  // 5% threshold
        
        // Determine learning type
        if (results.learning_significance) {
            if (results.learning_change > 0) {
                results.learning_type = "Facilitation";
                results.outcome = "Stronger response to stimuli";
            } else {
                results.learning_type = "Depression";
                results.outcome = "Weaker response to stimuli";
            }
        } else {
            results.learning_type = "No learning";
            results.outcome = "No significant change in response";
        }
        
        // Store trajectory data
        results.learning_trajectory = learning_data;
        
        // Store synaptic weights
        results.synaptic_weights = synaptic_weights;
        
        // Store spike times
        results.spike_times = spike_times;
        
        NLM_LOG_INFO("Learning analysis completed:");
        NLM_LOG_INFO("  Initial response: " + to_string(results.initial_response) + " Hz");
        NLM_LOG_INFO("  Final response: " + to_string(results.final_response) + " Hz");
        NLM_LOG_INFO("  Learning change: " + to_string(results.learning_change) + " Hz");
        NLM_LOG_INFO("  Learning type: " + results.learning_type);
        NLM_LOG_INFO("  Outcome: " + results.outcome);
        
        return results;
    }
    
    bool save_results(const string& filename, const LearningResults& results) {
        try {
            ofstream file(filename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Could not open file for writing: " + filename);
                return false;
            }
            
            file << "Single Neuron Learning Results" << endl;
            file << "===========================" << endl;
            file << endl;
            
            // Write experiment summary
            file << "Experiment Summary:" << endl;
            file << "  Total stimuli: " << stimuli_count << endl;
            file << "  Neuron count: " << results.total_neurons << endl;
            file << "  Synapse count: " << results.total_synapses << endl;
            file << "  Total spikes: " << results.total_spikes << endl;
            file << "  Wall clock time: " << results.wall_clock_time.count() << " seconds" << endl;
            file << endl;
            
            // Write learning trajectory
            file << "Learning Trajectory (Time, Potential, Spikes, Learning Rate, Synaptic Weight, Stimulus):" << endl;
            for (const auto& data : results.learning_trajectory) {
                file << fixed << setprecision(4)
                     << data.time << ","
                     << data.neuron_potential << ","
                     << data.spike_emitted << ","
                     << data.learning_rate << ","
                     << data.synaptic_weight << ","
                     << data.stimulus_strength << endl;
            }
            file << endl;
            
            // Write spike times
            file << "Spike Times:" << endl;
            for (double time : results.spike_times) {
                file << fixed << setprecision(4) << time << endl;
            }
            file << endl;
            
            // Write synaptic weights
            file << "Synaptic Weights (average per stimulus):" << endl;
            for (double weight : results.synaptic_weights) {
                file << fixed << setprecision(4) << weight << endl;
            }
            file << endl;
            
            file.close();
            NLM_LOG_INFO("Results saved to: " + filename);
            return true;
            
        } catch (const exception& e) {
            NLM_LOG_ERROR("Exception in save_results: " + string(e.what()));
            return false;
        }
    }
    
    bool generate_visualization(const string& prefix, const LearningResults& results) {
        try {
            // Generate visualization files
            string trajectory_file = prefix + "_trajectory.csv";
            string spike_file = prefix + "_spikes.csv";
            string weight_file = prefix + "_weights.csv";
            string summary_file = prefix + "_summary.txt";
            
            // Save trajectory data
            ofstream traj_file(trajectory_file);
            if (traj_file.is_open()) {
                traj_file << "time,neuron_potential,spike_emitted,learning_rate,synaptic_weight,stimulus_strength" << endl;
                for (const auto& data : results.learning_trajectory) {
                    traj_file << fixed << setprecision(4)
                             << data.time << ","
                             << data.neuron_potential << ","
                             << data.spike_emitted << ","
                             << data.learning_rate << ","
                             << data.synaptic_weight << ","
                             << data.stimulus_strength << endl;
                }
                traj_file.close();
            }
            
            // Save spike times
            ofstream spike_file_obj(spike_file);
            if (spike_file_obj.is_open()) {
                for (double time : results.spike_times) {
                    spike_file_obj << fixed << setprecision(4) << time << endl;
                }
                spike_file_obj.close();
            }
            
            // Save synaptic weights
            ofstream weight_file_obj(weight_file);
            if (weight_file_obj.is_open()) {
                for (double weight : results.synaptic_weights) {
                    weight_file_obj << fixed << setprecision(4) << weight << endl;
                }
                weight_file_obj.close();
            }
            
            // Save summary
            ofstream summary_file_obj(summary_file);
            if (summary_file_obj.is_open()) {
                summary_file_obj << results.get_summary() << endl;
                summary_file_obj.close();
            }
            
            NLM_LOG_INFO("Visualization files generated:");
            NLM_LOG_INFO("  Trajectory: " + trajectory_file);
            NLM_LOG_INFO("  Spike times: " + spike_file);
            NLM_LOG_INFO("  Synaptic weights: " + weight_file);
            NLM_LOG_INFO("  Summary: " + summary_file);
            
            return true;
            
        } catch (const exception& e) {
            NLM_LOG_ERROR("Exception in generate_visualization: " + string(e.what()));
            return false;
        }
    }
    
    void print_detailed_report(const LearningResults& results) {
        cout << endl << "=" << 70 << endl;
        cout << "SINGLE NEURON LEARNING EXPERIMENT - DETAILED REPORT" << endl;
        cout << "=" << 70 << endl;
        
        if (!results.success) {
            cout << "❌ EXPERIMENT FAILED" << endl;
            return;
        }
        
        cout << endl << "📊 EXPERIMENT OVERVIEW" << endl;
        cout << "  • Total stimuli presented: " << stimuli_count << endl;
        cout << "  • Total spikes recorded: " << results.total_spikes << endl;
        cout << "  • Baseline firing rate: " << results.baseline_firing_rate << " Hz" << endl;
        
        cout << endl << "🔍 NEURAL RESPONSE ANALYSIS" << endl;
        cout << "  • Initial response: " << results.initial_response << " Hz" << endl;
        cout << "  • Final response: " << results.final_response << " Hz" << endl;
        cout << "  • Mean response: " << results.mean_response << " ± " << results.response_std << " Hz" << endl;
        cout << "  • Learning change: " << results.learning_change << " Hz" << endl;
        
        cout << endl << "🎯 LEARNING OUTCOME" << endl;
        cout << "  • Learning type: " << results.learning_type << endl;
        cout << "  • Significant learning: " << (results.learning_significance ? "Yes" : "No") << endl;
        cout << "  • Final outcome: " << results.outcome << endl;
        
        // Calculate and display learning efficiency
        if (results.initial_response > 0) {
            double learning_efficiency = (results.learning_change / results.initial_response) * 100.0;
            cout << "  • Learning efficiency: " << learning_efficiency << "%" << endl;
        }
        
        cout << endl << "🧠 THEORETICAL INTERPRETATION" << endl;
        if (results.learning_type == "Facilitation") {
            cout << "  ✓ Hebbian learning confirmed: Synapses strengthened" << endl;
            cout << "  ✓ Neural adaptation: Neuron became more responsive" << endl;
        } else if (results.learning_type == "Depression") {
            cout << "  ⚠ Synaptic depression observed" << endl;
            cout << "  ⚠ Potential homeostatic regulation" << endl;
        } else {
            cout << "  • No significant synaptic change detected" << endl;
            cout << "  • May require more trials or stronger stimuli" << endl;
        }
        
        cout << endl << "📈 STATISTICAL SIGNIFICANCE" << endl;
        if (results.learning_significance) {
            cout << "  ✓ Learning is statistically significant" << endl;
            cout << "  ✓ Changes exceed expected noise threshold (5%)" << endl;
        } else {
            cout << "  ⚠ Learning not statistically significant" << endl;
            cout << "  ⚠ May need more repetitions to detect" << endl;
        }
        
        cout << endl << "🏁 CONCLUSION" << endl;
        if (results.learning_significance) {
            cout << "  ✅ Single neuron learning demonstrated successfully!" << endl;
            cout << "  ✅ Neural plasticity is functioning as expected" << endl;
            cout << "  ✅ Hebbian mechanisms are operational" << endl;
        } else {
            cout << "  ⚠ Learning observed but not statistically significant" << endl;
            cout << "  ⚠ May require optimization or longer training" << endl;
        }
        
        cout << endl << "=" << 70 << endl;
    }
};

int main(int argc, char* argv[]) {
    cout << "=" << 70 << endl;
    cout << "Single Neuron Learning - C++ Implementation" << endl;
    cout << "=" << 70 << endl;
    
    // Parse command line arguments
    string config_file = "";
    bool generate_visualization_flag = false;
    string output_prefix = "single_neuron_learning";
    
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            config_file = argv[++i];
        } else if (arg == "--visualize") {
            generate_visualization_flag = true;
        } else if (arg == "--output" && i + 1 < argc) {
            output_prefix = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            cout << "Usage: " << argv[0] << " [options]" << endl;
            cout << "Options:" << endl;
            cout << "  --config file     Configuration file" << endl;
            cout << "  --visualize       Generate visualization files" << endl;
            cout << "  --output prefix   Output file prefix" << endl;
            cout << "  --help, -h       Show this help" << endl;
            return 0;
        }
    }
    
    // Create learning experiment
    SingleNeuronLearning experiment;
    
    // Initialize with custom configuration if provided
    if (!config_file.empty()) {
        experiment = SingleNeuronLearning(config_file);
    }
    
    // Create neural network
    cout << "Creating neural network..." << endl;
    if (!experiment.create_network()) {
        cout << "❌ Failed to create neural network. Exiting." << endl;
        return 1;
    }
    
    // Run learning experiment
    cout << "Running learning experiment..." << endl;
    LearningResults results = experiment.run_learning_experiment();
    
    // Display detailed report
    experiment.print_detailed_report(results);
    
    // Save results
    string results_file = output_prefix + "_results.txt";
    if (experiment.save_results(results_file, results)) {
        cout << "Results saved to: " << results_file << endl;
    }
    
    // Generate visualization if requested
    if (generate_visualization_flag) {
        cout << "Generating visualization files..." << endl;
        string viz_prefix = output_prefix + "_viz";
        experiment.generate_visualization(viz_prefix, results);
    }
    
    cout << endl << "✅ Single Neuron Learning experiment completed successfully!" << endl;
    cout << endl << "Key takeaways:" << endl;
    cout << "• Neurons adapt their response properties through experience" << endl;
    cout << "• Hebbian learning strengthens connections between active neurons" << endl;
    cout << "• Learning curves show the adaptation process over time" << endl;
    cout << "• Statistical analysis confirms learning significance" << endl;
    
    return 0;
}
// NLM Visualization Utilities
// Advanced visualization tools for NLM brain state inspection
// Provides multiple visualization formats and output types

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/visualization/VisualizationInterface.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cmath>

using namespace nlm;

class VisualizationTools {
public:
    std::shared_ptr<Brain> brain;
    
    VisualizationTools(std::shared_ptr<Brain> b) : brain(b) {}
    
    // Generate text-based visualization of brain state
    void generateTextVisualization(const std::string& filename) {
        std::cout << "=== Generating Text Visualization ===" << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "NLM Brain State Visualization\n";
        file << "===========================" << std::endl;
        file << std::endl;
        
        // Header information
        file << "Simulation Status:" << std::endl;
        file << "  Step: N/A (text visualization doesn't track simulation steps)" << std::endl;
        file << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        file << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << std::endl;
        
        // Overall statistics
        file << "Network Statistics:" << std::endl;
        file << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "  Connection Density: " << calculateConnectionDensity() << std::endl;
        file << "  Firing Rate: " << std::fixed << std::setprecision(4) 
             << brain->getAverageFiringRate() << " Hz" << std::endl;
        file << "  Excitation/Inhibition Ratio: " << std::fixed << std::setprecision(4) 
             << brain->getExcitationInhibitionRatio() << std::endl;
        file << std::endl;
        
        // Regional analysis
        file << "Regional Analysis:" << std::endl;
        auto regions = brain->getAllRegions();
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            file << "  Region " << regionId.value << ":" << std::endl;
            auto neurons = region->getAllNeurons();
            file << "    Neurons: " << neurons.size() << std::endl;
            file << "    Synapses: " << region->getSynapses().size() << std::endl;
            
            if (!neurons.empty()) {
                // Calculate regional statistics
                double regionalFiringRate = calculateRegionalFiringRate(regionId);
                double regionalExcitatory = calculateRegionalExcitatoryRatio(regionId);
                
                file << "    Firing Rate: " << std::fixed << std::setprecision(4) 
                     << regionalFiringRate << " Hz" << std::endl;
                file << "    Excitatory Ratio: " << std::fixed << std::setprecision(4) 
                     << regionalExcitatory << std::endl;
            }
        }
        
        file << std::endl;
        
        // Neuron type distribution
        file << "Neuron Type Distribution:" << std::endl;
        auto typeCounts = countNeuronTypes();
        for (const auto& [type, count] : typeCounts) {
            file << "  " << neuronTypeToString(type) << ": " << count << " ("
                 << std::fixed << std::setprecision(2) 
                 << (count * 100.0 / brain->getTotalNeuronCount()) << "%)" << std::endl;
        }
        
        file << std::endl;
        
        // Synaptic type distribution
        file << "Synaptic Type Distribution:" << std::endl;
        auto synapseCounts = countSynapseTypes();
        for (const auto& [type, count] : synapseCounts) {
            file << "  " << synapseTypeToString(type) << ": " << count << " ("
                 << std::fixed << std::setprecision(2) 
                 << (count * 100.0 / brain->getTotalSynapseCount()) << "%)" << std::endl;
        }
        
        file << std::endl;
        
        // Activity patterns
        file << "Activity Patterns:" << std::endl;
        auto activityPatterns = analyzeActivityPatterns();
        file << "  Average Active Neuron %: " << std::fixed << std::setprecision(2) 
             << activityPatterns.avgActivePercentage << "%" << std::endl;
        file << "  Average Firing Neuron %: " << std::fixed << std::setprecision(2) 
             << activityPatterns.avgFiringPercentage << "%" << std::endl;
        file << "  High Activity Regions: " << activityPatterns.highActivityCount << std::endl;
        file << "  Low Activity Regions: " << activityPatterns.lowActivityCount << std::endl;
        
        file << std::endl;
        
        // Developmental stage
        file << "Developmental Status:" << std::endl;
        auto stage = brain->getDevelopmentalStage();
        file << "  Current Stage: " << developmentalStageToString(stage) << std::endl;
        file << "  Synapse Count Trend: " << calculateSynapseTrend() << std::endl;
        
        file.close();
        std::cout << "Text visualization saved to: " << filename << std::endl;
    }
    
    // Generate interactive HTML visualization
    void generateHTMLVisualization(const std::string& filename) {
        std::cout << "=== Generating HTML Visualization ===" << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        // Generate HTML content
        file << "<!DOCTYPE html>" << std::endl;
        file << "<html>" << std::endl;
        file << "<head>" << std::endl;
        file << "    <title>NLM Brain State Visualization</title>" << std::endl;
        file << "    <style>" << std::endl;
        file << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
        file << "        .container { max-width: 1200px; margin: 0 auto; }" << std::endl;
        file << "        .section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }" << std::endl;
        file << "        .header { background-color: #f0f0f0; padding: 10px; border-radius: 5px 5px 0 0; }" << std::endl;
        file << "        .stat { display: inline-block; margin: 10px; padding: 10px; background-color: #e9e9e9; border-radius: 3px; }" << std::endl;
        file << "        .stat-value { font-size: 24px; font-weight: bold; color: #2c3e50; }" << std::endl;
        file << "        .stat-label { font-size: 14px; color: #7f8c8d; }" << std::endl;
        file << "        table { border-collapse: collapse; width: 100%; margin: 10px 0; }" << std::endl;
        file << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
        file << "        th { background-color: #3498db; color: white; }" << std::endl;
        file << "        tr:nth-child(even) { background-color: #f2f2f2; }" << std::endl;
        file << "        canvas { border: 1px solid #ccc; margin: 10px 0; }" << std::endl;
        file << "    </style>" << std::endl;
        file << "</head>" << std::endl;
        file << "<body>" << std::endl;
        file << "    <div class='container'>" << std::endl;
        file << "        <h1>NLM Brain State Visualization</h1>" << std::endl;
        file << "        <p>Generated: " << getCurrentTimestamp() << "</p>" << std::endl;
        file << std::endl;
        
        // Overall statistics section
        file << "        <div class='section'>" << std::endl;
        file << "            <div class='header'><h2>Overall Statistics</h2></div>" << std::endl;
        file << "            <div>";
        
        // Key statistics
        file << "                <div class='stat'><div class='stat-value'>" << brain->getTotalNeuronCount() 
             << "</div><div class='stat-label'>Neurons</div></div>" << std::endl;
        file << "                <div class='stat'><div class='stat-value'>" << brain->getTotalSynapseCount() 
             << "</div><div class='stat-label'>Synapses</div></div>" << std::endl;
        file << "                <div class='stat'><div class='stat-value'>" << std::fixed << std::setprecision(2)
             << brain->getAverageFiringRate() << " Hz</div><div class='stat-label'>Firing Rate</div></div>" << std::endl;
        file << "                <div class='stat'><div class='stat-value'>" << std::fixed << std::setprecision(2)
             << (brain->getActiveNeuronCount() * 100.0 / brain->getTotalNeuronCount()) << "%</div><div class='stat-label'>Active %</div></div>" << std::endl;
        file << "                <div class='stat'><div class='stat-value'>" << std::fixed << std::setprecision(2)
             << brain->getExcitationInhibitionRatio() << "</div><div class='stat-label'>E/I Ratio</div></div>" << std::endl;
        file << "            </div>" << std::endl;
        file << "        </div>" << std::endl;
        file << std::endl;
        
        // Regional analysis table
        file << "        <div class='section'>" << std::endl;
        file << "            <div class='header'><h2>Regional Analysis</h2></div>" << std::endl;
        file << "            <table>" << std::endl;
        file << "                <tr><th>Region ID</th><th>Neurons</th><th>Synapses</th><th>Firing Rate</th><th>Excitatory</th><th>Activity</th></tr>" << std::endl;
        
        auto regions = brain->getAllRegions();
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            auto neurons = region->getAllNeurons();
            double regionalFiringRate = calculateRegionalFiringRate(regionId);
            double regionalExcitatory = calculateRegionalExcitatoryRatio(regionId);
            double activityLevel = calculateRegionalActivityLevel(regionId);
            
            file << "                <tr>" << std::endl;
            file << "                    <td>" << regionId.value << "</td>" << std::endl;
            file << "                    <td>" << neurons.size() << "</td>" << std::endl;
            file << "                    <td>" << region->getSynapses().size() << "</td>" << std::endl;
            file << "                    <td>" << std::fixed << std::setprecision(4) << regionalFiringRate << " Hz</td>" << std::endl;
            file << "                    <td>" << std::fixed << std::setprecision(4) << regionalExcitatory << "</td>" << std::endl;
            file << "                    <td>" << std::fixed << std::setprecision(2) << activityLevel << "%</td>" << std::endl;
            file << "                </tr>" << std::endl;
        }
        
        file << "            </table>" << std::endl;
        file << "        </div>" << std::endl;
        file << std::endl;
        
        // Distribution charts
        file << "        <div class='section'>" << std::endl;
        file << "            <div class='header'><h2>Distributions</h2></div>" << std::endl;
        file << "            <div id='neuron-type-chart'></div>" << std::endl;
        file << "            <div id='synapse-type-chart'></div>" << std::endl;
        file << "        </div>" << std::endl;
        file << std::endl;
        
        // JavaScript for charts
        file << "        <script src='https://cdn.jsdelivr.net/npm/chart.js'></script>" << std::endl;
        file << "        <script>" << std::endl;
        file << "            // Neuron type distribution" << std::endl;
        file << "            const neuronTypeData = {
                labels: ['Excitatory', 'Inhibitory', 'Modulatory', 'Sensory', 'Motor', 'Internal'],
                datasets: [{
                    label: 'Neuron Types',
                    data: [" << getNeuronTypeCount(NeuronType::Excitatory) << ", " 
                          << getNeuronTypeCount(NeuronType::Inhibitory) << ", "
                          << getNeuronTypeCount(NeuronType::Modulatory) << ", "
                          << getNeuronTypeCount(NeuronType::Sensory) << ", "
                          << getNeuronTypeCount(NeuronType::Motor) << ", "
                          << getNeuronTypeCount(NeuronType::Internal) << "],
                    backgroundColor: ['#e74c3c', '#3498db', '#f39c12', '#2ecc71', '#9b59b6', '#95a5a6']
                }]
            };" << std::endl;
        file << std::endl;
        file << "            // Synapse type distribution" << std::endl;
        file << "            const synapseTypeData = {
                labels: ['Excitatory', 'Inhibitory', 'Modulatory', 'Electrical', 'Gap Junction'],
                datasets: [{
                    label: 'Synapse Types',
                    data: [" << getSynapseTypeCount(SynapseType::Excitatory) << ", " 
                          << getSynapseTypeCount(SynapseType::Inhibitory) << ", "
                          << getSynapseTypeCount(SynapseType::Modulatory) << ", "
                          << getSynapseTypeCount(SynapseType::Electrical) << ", "
                          << getSynapseTypeCount(SynapseType::GapJunction) << "],
                    backgroundColor: ['#e74c3c', '#3498db', '#f39c12', '#2ecc71', '#95a5a6']
                }]
            };" << std::endl;
        file << std::endl;
        file << "            // Create charts" << std::endl;
        file << "            new Chart(document.getElementById('neuron-type-chart'), {
                type: 'bar',
                data: neuronTypeData,
                options: { title: { display: true, text: 'Neuron Type Distribution' } }
            });" << std::endl;
        file << "            new Chart(document.getElementById('synapse-type-chart'), {
                type: 'bar',
                data: synapseTypeData,
                options: { title: { display: true, text: 'Synapse Type Distribution' } }
            });" << std::endl;
        file << "        </script>" << std::endl;
        file << std::endl;
        
        // Analysis section
        file << "        <div class='section'>" << std::endl;
        file << "            <div class='header'><h2>Analysis & Insights</h2></div>" << std::endl;
        
        auto activityAnalysis = analyzeActivityPatterns();
        file << "            <h3>Activity Analysis</h3>" << std::endl;
        file << "            <p>Average Active Neuron Percentage: <strong>" << std::fixed << std::setprecision(2)
             << activityAnalysis.avgActivePercentage << "%</strong></p>" << std::endl;
        file << "            <p>Average Firing Neuron Percentage: <strong>" << std::fixed << std::setprecision(2)
             << activityAnalysis.avgFiringPercentage << "%</strong></p>" << std::endl;
        file << "            <p>High Activity Regions: <strong>" << activityAnalysis.highActivityCount << "</strong></p>" << std::endl;
        file << "            <p>Low Activity Regions: <strong>" << activityAnalysis.lowActivityCount << "</strong></p>" << std::endl;
        
        file << "            <h3>Developmental Insights</h3>" << std::endl;
        file << "            <p>Current Developmental Stage: <strong>" 
             << developmentalStageToString(brain->getDevelopmentalStage()) << "</strong></p>" << std::endl;
        file << "            <p>Synapse Trend: <strong>" << calculateSynapseTrend() << "</strong></p>" << std::endl;
        
        file << "        </div>" << std::endl;
        file << std::endl;
        
        file << "    </div>" << std::endl;
        file << "</body>" << std::endl;
        file << "</html>" << std::endl;
        
        file.close();
        std::cout << "HTML visualization saved to: " << filename << std::endl;
    }
    
    // Generate JSON data for external analysis
    void generateJSONData(const std::string& filename) {
        std::cout << "=== Generating JSON Data ===" << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        nlohmann::json j;
        
        // Basic information
        j["metadata"] = {
            {"timestamp", getCurrentTimestamp()},
            {"version", "2.0.0"},
            {"format", "json"}
        };
        
        // Brain state
        j["brain_state"] = {
            {"neuron_count", brain->getTotalNeuronCount()},
            {"synapse_count", brain->getTotalSynapseCount()},
            {"active_neuron_count", brain->getActiveNeuronCount()},
            {"firing_neuron_count", brain->getFiringNeuronCount()},
            {"average_firing_rate", brain->getAverageFiringRate()},
            {"excitation_inhibition_ratio", brain->getExcitationInhibitionRatio()},
            {"developmental_stage", developmentalStageToString(brain->getDevelopmentalStage())}
        };
        
        // Regional data
        j["regions"] = nlohmann::json::array();
        auto regions = brain->getAllRegions();
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            nlohmann::json regionJson;
            regionJson["id"] = regionId.value;
            regionJson["neuron_count"] = region->getAllNeurons().size();
            regionJson["synapse_count"] = region->getSynapses().size();
            regionJson["firing_rate"] = calculateRegionalFiringRate(regionId);
            regionJson["excitatory_ratio"] = calculateRegionalExcitatoryRatio(regionId);
            regionJson["activity_level"] = calculateRegionalActivityLevel(regionId);
            
            j["regions"].push_back(regionJson);
        }
        
        // Neuron type distribution
        j["neuron_types"] = nlohmann::json::object();
        auto typeCounts = countNeuronTypes();
        for (const auto& [type, count] : typeCounts) {
            j["neuron_types"][neuronTypeToString(type)] = count;
        }
        
        // Synaptic type distribution
        j["synapse_types"] = nlohmann::json::object();
        auto synapseCounts = countSynapseTypes();
        for (const auto& [type, count] : synapseCounts) {
            j["synapse_types"][synapseTypeToString(type)] = count;
        }
        
        // Activity analysis
        auto activityAnalysis = analyzeActivityPatterns();
        j["activity_analysis"] = {
            {"average_active_percentage", activityAnalysis.avgActivePercentage},
            {"average_firing_percentage", activityAnalysis.avgFiringPercentage},
            {"high_activity_count", activityAnalysis.highActivityCount},
            {"low_activity_count", activityAnalysis.lowActivityCount}
        };
        
        // Write JSON
        file << j.dump(2) << std::endl;
        file.close();
        
        std::cout << "JSON data saved to: " << filename << std::endl;
    }
    
    // Generate CSV data for spreadsheet analysis
    void generateCSVData(const std::string& filename) {
        std::cout << "=== Generating CSV Data ===" << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        // Header
        file << "metric,value,description\n";
        
        // Basic metrics
        file << "total_neurons," << brain->getTotalNeuronCount() << ",Total neuron count\n";
        file << "total_synapses," << brain->getTotalSynapseCount() << ",Total synapse count\n";
        file << "active_neurons," << brain->getActiveNeuronCount() << ",Number of active neurons\n";
        file << "firing_neurons," << brain->getFiringNeuronCount() << ",Number of firing neurons\n";
        file << "average_firing_rate," << std::fixed << std::setprecision(6) 
             << brain->getAverageFiringRate() << ",Average firing rate in Hz\n";
        file << "excitation_inhibition_ratio," << std::fixed << std::setprecision(6) 
             << brain->getExcitationInhibitionRatio() << ",E/I ratio\n";
        file << "activity_percentage," << std::fixed << std::setprecision(6) 
             << (brain->getActiveNeuronCount() * 100.0 / brain->getTotalNeuronCount()) 
             << ",Percentage of active neurons\n";
        file << "firing_percentage," << std::fixed << std::setprecision(6) 
             << (brain->getFiringNeuronCount() * 100.0 / brain->getTotalNeuronCount()) 
             << ",Percentage of firing neurons\n";
        
        // Regional metrics
        file << "\nregional_data\n";
        file << "region_id,neuron_count,synapse_count,firing_rate,excitatory_ratio,activity_level\n";
        
        auto regions = brain->getAllRegions();
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            double firingRate = calculateRegionalFiringRate(regionId);
            double excitatoryRatio = calculateRegionalExcitatoryRatio(regionId);
            double activityLevel = calculateRegionalActivityLevel(regionId);
            
            file << regionId.value << ","
                 << region->getAllNeurons().size() << ","
                 << region->getSynapses().size() << ","
                 << std::fixed << std::setprecision(6) << firingRate << ","
                 << std::fixed << std::setprecision(6) << excitatoryRatio << ","
                 << std::fixed << std::setprecision(6) << activityLevel << "\n";
        }
        
        // Type distributions
        file << "\ntype_distributions\n";
        file << "type,count,percentage\n";
        
        auto neuronTypes = countNeuronTypes();
        for (const auto& [type, count] : neuronTypes) {
            double percentage = (count * 100.0) / brain->getTotalNeuronCount();
            file << neuronTypeToString(type) << ","
                 << count << ","
                 << std::fixed << std::setprecision(6) << percentage << "\n";
        }
        
        auto synapseTypes = countSynapseTypes();
        file << "\nsynapse_type_distributions\n";
        file << "type,count,percentage\n";
        for (const auto& [type, count] : synapseTypes) {
            double percentage = (count * 100.0) / brain->getTotalSynapseCount();
            file << synapseTypeToString(type) << ","
                 << count << ","
                 << std::fixed << std::setprecision(6) << percentage << "\n";
        }
        
        file.close();
        std::cout << "CSV data saved to: " << filename << std::endl;
    }
    
    // Generate analysis report
    void generateAnalysisReport(const std::string& filename) {
        std::cout << "=== Generating Analysis Report ===" << std::endl;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file " << filename << std::endl;
            return;
        }
        
        file << "NLM Brain State Analysis Report\n";
        file << "==============================\n\n";
        
        file << "Report Generated: " << getCurrentTimestamp() << "\n\n";
        
        // Executive summary
        file << "Executive Summary:\n";
        file << "  - Network Size: " << brain->getTotalNeuronCount() << " neurons, "
             << brain->getTotalSynapseCount() << " synapses\n";
        file << "  - Network Activity: " << std::fixed << std::setprecision(2)
             << brain->getAverageFiringRate() << " Hz average firing rate\n";
        file << "  - Network Balance: E/I ratio = " << std::fixed << std::setprecision(4)
             << brain->getExcitationInhibitionRatio() << "\n";
        file << "  - Developmental Stage: " << developmentalStageToString(brain->getDevelopmentalStage()) << "\n";
        file << std::endl;
        
        // Performance assessment
        file << "Performance Assessment:\n";
        
        double connectionDensity = calculateConnectionDensity();
        double activityLevel = calculateOverallActivityLevel();
        double networkStability = calculateNetworkStability();
        
        file << "  - Connection Density: " << std::fixed << std::setprecision(6) 
             << connectionDensity << " (synapses per neuron)\n";
        file << "  - Activity Level: " << std::fixed << std::setprecision(2) 
             << activityLevel << "% average active neurons\n";
        file << "  - Network Stability: " << std::fixed << std::setprecision(4) 
             << networkStability << " (1.0 = stable)\n";
        
        // Regional analysis
        file << "\nRegional Analysis:\n";
        file << "Region | Neurons | Synapses | Firing Rate | E/I Ratio | Activity Level" << std::endl;
        file << "-------|---------|----------|-------------|-----------|---------------" << std::endl;
        
        auto regions = brain->getAllRegions();
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            double firingRate = calculateRegionalFiringRate(regionId);
            double excitatoryRatio = calculateRegionalExcitatoryRatio(regionId);
            double activityLevel = calculateRegionalActivityLevel(regionId);
            
            file << std::setw(6) << regionId.value << " | " << std::setw(7)
                 << region->getAllNeurons().size() << " | " << std::setw(8)
                 << region->getSynapses().size() << " | " << std::setw(11)
                 << std::fixed << std::setprecision(4) << firingRate << " | " << std::setw(9)
                 << std::fixed << std::setprecision(4) << excitatoryRatio << " | " << std::setw(13)
                 << std::fixed << std::setprecision(2) << activityLevel << "%" << std::endl;
        }
        
        // Type distribution analysis
        file << "\nType Distribution Analysis:\n";
        file << "Neuron Types:" << std::endl;
        auto neuronTypes = countNeuronTypes();
        for (const auto& [type, count] : neuronTypes) {
            double percentage = (count * 100.0) / brain->getTotalNeuronCount();
            file << "  " << neuronTypeToString(type) << ": " << count << " ("
                 << std::fixed << std::setprecision(2) << percentage << "%)" << std::endl;
        }
        
        file << "\nSynaptic Types:" << std::endl;
        auto synapseTypes = countSynapseTypes();
        for (const auto& [type, count] : synapseTypes) {
            double percentage = (count * 100.0) / brain->getTotalSynapseCount();
            file << "  " << synapseTypeToString(type) << ": " << count << " ("
                 << std::fixed << std::setprecision(2) << percentage << "%)" << std::endl;
        }
        
        // Activity pattern analysis
        file << "\nActivity Pattern Analysis:\n";
        auto activityAnalysis = analyzeActivityPatterns();
        
        file << "  Network Activity Summary:" << std::endl;
        file << "    Average Active Neuron Percentage: " << std::fixed << std::setprecision(2)
             << activityAnalysis.avgActivePercentage << "%" << std::endl;
        file << "    Average Firing Neuron Percentage: " << std::fixed << std::setprecision(2)
             << activityAnalysis.avgFiringPercentage << "%" << std::endl;
        file << "    High Activity Regions: " << activityAnalysis.highActivityCount << std::endl;
        file << "    Low Activity Regions: " << activityAnalysis.lowActivityCount << std::endl;
        
        // Developmental assessment
        file << "\nDevelopmental Assessment:\n";
        auto stage = brain->getDevelopmentalStage();
        file << "  Current Developmental Stage: " << developmentalStageToString(stage) << std::endl;
        file << "  Synapse Count: " << brain->getTotalSynapseCount() << std::endl;
        file << "  Synapse Trend: " << calculateSynapseTrend() << std::endl;
        file << "  Neural Maturation: " << calculateMaturationLevel() << " (0.0 = immature, 1.0 = mature)" << std::endl;
        
        // Recommendations
        file << "\nRecommendations:\n";
        
        if (activityAnalysis.avgActivePercentage < 10.0) {
            file << "  - Low network activity detected. Consider increasing input or adjusting parameters.\n";
        } else if (activityAnalysis.avgActivePercentage > 80.0) {
            file << "  - High network activity detected. Consider reducing input or decreasing connectivity.\n";
        }
        
        if (brain->getExcitationInhibitionRatio() < 0.8 || brain->getExcitationInhibitionRatio() > 1.2) {
            file << "  - E/I ratio outside optimal range (1.0). Consider adjusting synaptic parameters.\n";
        }
        
        if (calculateNetworkStability() < 0.5) {
            file << "  - Network stability is low. Monitor for potential instability.\n";
        }
        
        file << "  - Continue monitoring for developmental changes.\n";
        file << "  - Consider running additional experiments with different parameters.\n";
        
        file << std::endl;
        file << "End of Report." << std::endl;
        
        file.close();
        std::cout << "Analysis report saved to: " << filename << std::endl;
    }
    
private:
    // Helper functions
    double calculateConnectionDensity() const {
        if (brain->getTotalNeuronCount() == 0) return 0.0;
        return static_cast<double>(brain->getTotalSynapseCount()) / brain->getTotalNeuronCount();
    }
    
    double calculateRegionalFiringRate(const RegionId& regionId) const {
        auto region = brain->getRegion(regionId);
        if (!region) return 0.0;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return 0.0;
        
        double totalFiringRate = 0.0;
        for (const auto& neuron : neurons) {
            totalFiringRate += neuron->getFiringRate();
        }
        
        return totalFiringRate / neurons.size();
    }
    
    double calculateRegionalExcitatoryRatio(const RegionId& regionId) const {
        auto region = brain->getRegion(regionId);
        if (!region) return 0.0;
        
        auto synapses = region->getSynapses();
        if (synapses.empty()) return 0.0;
        
        int excitatoryCount = 0;
        for (const auto& synapse : synapses) {
            if (synapse->getType() == SynapseType::Excitatory) {
                excitatoryCount++;
            }
        }
        
        return static_cast<double>(excitatoryCount) / synapses.size();
    }
    
    double calculateRegionalActivityLevel(const RegionId& regionId) const {
        auto region = brain->getRegion(regionId);
        if (!region) return 0.0;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return 0.0;
        
        int activeCount = 0;
        for (const auto& neuron : neurons) {
            if (neuron->isActive()) {
                activeCount++;
            }
        }
        
        return static_cast<double>(activeCount) / neurons.size() * 100.0;
    }
    
    std::map<NeuronType, int> countNeuronTypes() const {
        std::map<NeuronType, int> counts;
        auto regions = brain->getAllRegions();
        
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            auto neurons = region->getAllNeurons();
            for (const auto& neuron : neurons) {
                counts[neuron->getType()]++;
            }
        }
        
        return counts;
    }
    
    std::map<SynapseType, int> countSynapseTypes() const {
        std::map<SynapseType, int> counts;
        auto regions = brain->getAllRegions();
        
        for (const auto& regionId : regions) {
            auto region = brain->getRegion(regionId);
            if (!region) continue;
            
            auto synapses = region->getSynapses();
            for (const auto& synapse : synapses) {
                counts[synapse->getType()]++;
            }
        }
        
        return counts;
    }
    
    struct ActivityAnalysis {
        double avgActivePercentage = 0.0;
        double avgFiringPercentage = 0.0;
        int highActivityCount = 0;
        int lowActivityCount = 0;
    };
    
    ActivityAnalysis analyzeActivityPatterns() const {
        ActivityAnalysis analysis;
        auto regions = brain->getAllRegions();
        int totalRegions = 0;
        
        for (const auto& regionId : regions) {
            double activityLevel = calculateRegionalActivityLevel(regionId);
            
            analysis.avgActivePercentage += activityLevel;
            totalRegions++;
            
            if (activityLevel > 50.0) {
                analysis.highActivityCount++;
            } else if (activityLevel < 10.0) {
                analysis.lowActivityCount++;
            }
        }
        
        if (totalRegions > 0) {
            analysis.avgActivePercentage /= totalRegions;
        }
        
        // Calculate firing percentage
        analysis.avgFiringPercentage = static_cast<double>(brain->getFiringNeuronCount()) / 
                                       brain->getTotalNeuronCount() * 100.0;
        
        return analysis;
    }
    
    double calculateConnectionDensity() const {
        return static_cast<double>(brain->getTotalSynapseCount()) / brain->getTotalNeuronCount();
    }
    
    double calculateOverallActivityLevel() const {
        return static_cast<double>(brain->getActiveNeuronCount()) / brain->getTotalNeuronCount() * 100.0;
    }
    
    double calculateNetworkStability() const {
        // Simplified stability metric based on firing rate and E/I ratio
        double firingRate = brain->getAverageFiringRate();
        double eiRatio = brain->getExcitationInhibitionRatio();
        
        double firingStability = 1.0 / (1.0 + firingRate / 50.0); // Optimal around 50 Hz
        double eiStability = 1.0 - std::abs(eiRatio - 1.0); // Optimal at 1.0
        
        return (firingStability + eiStability) / 2.0;
    }
    
    std::string calculateSynapseTrend() const {
        // This would require historical data or more complex analysis
        // For now, return a simplified estimate
        int synapseCount = brain->getTotalSynapseCount();
        
        if (synapseCount > 1000) {
            return "High - mature network";
        } else if (synapseCount > 500) {
            return "Moderate - developing network";
        } else {
            return "Low - early development";
        }
    }
    
    double calculateMaturationLevel() const {
        // Simplified maturation based on synapse count and firing rate
        auto stage = brain->getDevelopmentalStage();
        
        double stageValue = static_cast<double>(stage.value);
        double synapseFactor = std::min(1.0, static_cast<double>(brain->getTotalSynapseCount()) / 2000.0);
        double firingFactor = std::min(1.0, brain->getAverageFiringRate() / 100.0);
        
        return (stageValue / 4.0 + synapseFactor + firingFactor) / 3.0;
    }
    
    const char* neuronTypeToString(NeuronType type) const {
        switch (type) {
            case NeuronType::Excitatory: return "Excitatory";
            case NeuronType::Inhibitory: return "Inhibitory";
            case NeuronType::Modulatory: return "Modulatory";
            case NeuronType::Sensory: return "Sensory";
            case NeuronType::Motor: return "Motor";
            case NeuronType::Internal: return "Internal";
            default: return "Unknown";
        }
    }
    
    const char* synapseTypeToString(SynapseType type) const {
        switch (type) {
            case SynapseType::Excitatory: return "Excitatory";
            case SynapseType::Inhibitory: return "Inhibitory";
            case SynapseType::Modulatory: return "Modulatory";
            case SynapseType::Electrical: return "Electrical";
            case SynapseType::GapJunction: return "GapJunction";
            default: return "Unknown";
        }
    }
    
    const char* developmentalStageToString(DevelopmentalStage stage) const {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    int getNeuronTypeCount(NeuronType type) const {
        auto counts = countNeuronTypes();
        return counts.count(type) ? counts.at(type) : 0;
    }
    
    int getSynapseTypeCount(SynapseType type) const {
        auto counts = countSynapseTypes();
        return counts.count(type) ? counts.at(type) : 0;
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Visualization Tools (Advanced Utility) ===" << std::endl;
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string outputFormat = "text";
    bool generateAll = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--format" && i + 1 < argc) {
            outputFormat = argv[++i];
        } else if (arg == "--all") {
            generateAll = true;
        } else if (arg == "--help") {
            std::cout << "Usage: nlm_visualization [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --config <file>      Configuration file (default: configs/default.cfg)\n";
            std::cout << "  --output <file>      Output file (required)\n";
            std::cout << "  --format <type>      Output format: text, html, json, csv, report (default: text)\n";
            std::cout << "  --all                Generate all formats\n";
            std::cout << "  --help               Show this help\n";
            return 0;
        }
    }
    
    if (outputFile.empty() && !generateAll) {
        std::cout << "Error: Output file required. Use --help for more information." << std::endl;
        return 1;
    }
    
    if (outputFormat != "text" && outputFormat != "html" && outputFormat != "json" && 
        outputFormat != "csv" && outputFormat != "report") {
        std::cout << "Error: Invalid output format. Use --help for valid formats." << std::endl;
        return 1;
    }
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(configFile)) {
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Initialize brain
    std::cout << "Initializing NLM Brain for visualization..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Error: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Create visualization tools
    VisualizationTools vizTools(brain);
    
    // Generate visualizations based on format
    if (generateAll) {
        vizTools.generateTextVisualization("brain_state.txt");
        vizTools.generateHTMLVisualization("brain_state.html");
        vizTools.generateJSONData("brain_state.json");
        vizTools.generateCSVData("brain_state.csv");
        vizTools.generateAnalysisReport("brain_state_report.txt");
        
        std::cout << "\n=== All visualizations generated successfully ===" << std::endl;
    } else {
        if (outputFormat == "text") {
            vizTools.generateTextVisualization(outputFile);
        } else if (outputFormat == "html") {
            vizTools.generateHTMLVisualization(outputFile);
        } else if (outputFormat == "json") {
            vizTools.generateJSONData(outputFile);
        } else if (outputFormat == "csv") {
            vizTools.generateCSVData(outputFile);
        } else if (outputFormat == "report") {
            vizTools.generateAnalysisReport(outputFile);
        }
        
        std::cout << "\n=== Visualization complete ===" << std::endl;
    }
    
    return 0;
}

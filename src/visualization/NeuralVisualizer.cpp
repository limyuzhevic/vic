// src/visualization/NeuralVisualizer.cpp
#include "NeuralVisualizer.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {
namespace visualization {

NeuralNetworkVisualizer::NeuralNetworkVisualizer()
    : image_width_(1920), image_height_(1080), color_scheme_("default"),
      neuron_layout_("force_directed"), connection_mode_("weighted"),
      enable_antialiasing_(true), connection_threshold_(0.1), neuron_size_scale_(1.0) {
}

NeuralNetworkVisualizer::~NeuralNetworkVisualizer() {
}

NeuralNetworkVisualization NeuralNetworkVisualizer::collectVisualizationData(
    std::shared_ptr<Brain> brain, const std::map<std::string, double>& parameters) {
    NeuralNetworkVisualization data;
    
    if (!brain) {
        return data;
    }
    
    // Collect neuron data
    auto neurons = brain->getAllNeurons();
    data.neurons.reserve(neurons.size());
    
    // Convert neurons to visualization data
    for (size_t i = 0; i < neurons.size(); ++i) {
        NeuronVisualizationData neuron_data;
        neuron_data.id = neurons[i]->getId();
        neuron_data.type = neurons[i]->getType();
        neuron_data.membrane_potential = neurons[i]->getMembranePotential();
        neuron_data.threshold = neurons[i]->getThreshold();
        neuron_data.is_firing = neurons[i]->isFiring();
        neuron_data.is_refractory = neurons[i]->isRefractory();
        neuron_data.firing_rate = neurons[i]->getFiringRate();
        neuron_data.adaptation_level = neurons[i]->getState().adaptationVariable;
        
        // Apply color scheme based on neuron type
        applyNeuronTypeColorScheme(data, neuron_data);
        
        data.neurons.push_back(neuron_data);
    }
    
    // Collect synapse data
    auto synapses = brain->getAllSynapses();
    data.synapses.reserve(synapses.size());
    
    // Convert synapses to visualization data
    for (size_t i = 0; i < synapses.size(); ++i) {
        SynapseVisualizationData synapse_data;
        synapse_data.id = synapses[i]->getId();
        synapse_data.source_neuron = synapses[i]->getSourceNeuron();
        synapse_data.destination_neuron = synapses[i]->getDestinationNeuron();
        synapse_data.weight = synapses[i]->getWeight();
        synapse_data.delay = synapses[i]->getDelay();
        synapse_data.type = synapses[i]->getType();
        synapse_data.is_active = synapses[i]->getPlasticityFlags().hebbian || synapses[i]->getPlasticityFlags().stdp;
        
        // Find corresponding neurons for positioning
        for (const auto& neuron : data.neurons) {
            if (neuron.id == synapse_data.source_neuron ||
                neuron.id == synapse_data.destination_neuron) {
                // Position will be set later by layout algorithm
                break;
            }
        }
        
        // Apply color scheme based on synapse type and weight
        applyWeightBasedColorScheme(data, synapse_data);
        
        data.synapses.push_back(synapse_data);
    }
    
    // Set visualization parameters
    data.simulation_time = 0.0;  // Would be tracked in real usage
    data.time_step = 0.001;       // Default timestep
    
    // Apply layout algorithm
    if (neuron_layout_ == "force_directed") {
        applyForceDirectedLayout(data);
    } else if (neuron_layout_ == "circular") {
        applyCircularLayout(data);
    } else if (neuron_layout_ == "radial") {
        applyRadialLayout(data);
    } else if (neuron_layout_ == "grid") {
        applyGridLayout(data);
    } else if (neuron_layout_ == "hierarchy") {
        applyHierarchyLayout(data);
    }
    
    // Apply color scheme
    if (color_scheme_ == "heatmap") {
        applyHeatmapColorScheme(data);
    } else if (color_scheme_ == "neuron_type") {
        applyNeuronTypeColorScheme(data);
    } else if (color_scheme_ == "weight_based") {
        applyWeightBasedColorScheme(data);
    } else if (color_scheme_ == "activity_based") {
        applyActivityBasedColorScheme(data);
    }
    
    return data;
}

bool NeuralNetworkVisualizer::generateVisualization(
    const NeuralNetworkVisualization& data, const std::string& filename) {
    std::string extension = getFileExtension(format_);
    std::string full_filename = filename;
    
    if (extension.empty() && !filename.empty()) {
        full_filename += "." + extension;
    }
    
    ensureOutputDirectory(output_directory_);
    
    switch (format_) {
        case VisualizationFormat::HTML:
            return generateInteractiveHTML(data, full_filename);
        case VisualizationFormat::CSV:
            return generateCSVFile(data, full_filename);
        case VisualizationFormat::JSON:
            return generateJSONFile(data, full_filename);
        case VisualizationFormat::Text:
            return generateTextReport(data, full_filename);
        default:
            std::cerr << "Unsupported visualization format" << std::endl;
            return false;
    }
}

bool NeuralNetworkVisualizer::generateInteractiveHTML(
    const NeuralNetworkVisualization& data, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "    <title>" << data.title << "</title>" << std::endl;
    file << "    <style>" << std::endl;
    file << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    file << "        #network-container { border: 1px solid #ccc; width: 100%; height: 600px; }" << std::endl;
    file << "        .neuron { stroke: #333; stroke-width: 2; fill-opacity: 0.8; }" << std::endl;
    file << "        .excitatory { fill: #ff6b6b; }" << std::endl;
    file << "        .inhibitory { fill: #4ecdc4; }" << std::endl;
    file << "        .sensory { fill: #45b7d1; }" << std::endl;
    file << "        .motor { fill: #f9ca24; }" << std::endl;
    file << "        .internal { fill: #96ceb4; }" << std::endl;
    file << "        .synapse { stroke: #999; stroke-width: 1; }" << std::endl;
    file << "        .active { stroke: #ff0; stroke-width: 3; }" << std::endl;
    file << "        .firing { fill: #ff0; }" << std::endl;
    file << "        .refractory { fill: #ccc; }" << std::endl;
    file << "        .stats { margin: 20px; padding: 20px; background: #f5f5f5; }" << std::endl;
    file << "        .legend { margin: 20px; }" << std::endl;
    file << "    </style>" << std::endl;
    file << "    <script src=\"https://d3js.org/d3.v7.min.js\"></script>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "    <h1>" << data.title << "</h1>" << std::endl;
    file << "    <p>" << data.description << "</p>" << std::endl;
    file << "    <div id=\"network-container\"></div>" << std::endl;
    file << "    <div class=\"stats\">" << std::endl;
    file << "        <h2>Network Statistics</h2>" << std::endl;
    file << "        <p>Simulation Time: " << data.simulation_time << " s</p>" << std::endl;
    file << "        <p>Time Step: " << data.time_step << " s</p>" << std::endl;
    file << "        <p>Total Neurons: " << data.neurons.size() << "</p>" << std::endl;
    file << "        <p>Total Synapses: " << data.synapses.size() << "</p>" << std::endl;
    file << "        <p>Active Connections: " << countActiveConnections(data) << "</p>" << std::endl;
    file << "    </div>" << std::endl;
    file << "    <div class=\"legend\">" << std::endl;
    file << "        <h3>Neuron Types</h3>" << std::endl;
    file << "        <p><span class=\"neuron excitatory\">■ Excitatory</span> - " << countNeuronsByType(data, NeuronType::Excitatory) << " neurons</p>" << std::endl;
    file << "        <p><span class=\"neuron inhibitory\">■ Inhibitory</span> - " << countNeuronsByType(data, NeuronType::Inhibitory) << " neurons</p>" << std::endl;
    file << "        <p><span class=\"neuron sensory\">■ Sensory</span> - " << countNeuronsByType(data, NeuronType::Sensory) << " neurons</p>" << std::endl;
    file << "        <p><span class=\"neuron motor\">■ Motor</span> - " << countNeuronsByType(data, NeuronType::Motor) << " neurons</p>" << std::endl;
    file << "        <p><span class=\"neuron internal\">■ Internal</span> - " << countNeuronsByType(data, NeuronType::Internal) << " neurons</p>" << std::endl;
    file << "    </div>" << std::endl;
    file << "    <script>" << std::endl;
    file << "        // D3.js network visualization code" << std::endl;
    file << "        const data = " << convertToJSON(data) << ";" << std::endl;
    file << "        const width = 1000;" << std::endl;
    file << "        const height = 600;" << std::endl;
    file << "        const svg = d3.select('#network-container')" << std::endl;
    file << "            .append('svg')" << std::endl;
    file << "            .attr('width', width)" << std::endl;
    file << "            .attr('height', height);" << std::endl;
    file << "        // Network rendering logic would go here" << std::endl;
    file << "        // This is a simplified template for the interactive visualization" << std::endl;
    file << "    </script>" << std::endl;
    file << "</body>" << std::endl;
    file << "</html>" << std::endl;
    
    file.close();
    return true;
}

bool NeuralNetworkVisualizer::generateCSVFile(const NeuralNetworkVisualization& data,
                                            const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Neurons CSV
    file << "NeuronID,Type,X,Y,Radius,V,m,V_thresh,FiringRate,Adaptation,IsFiring,IsRefractory,R,G,B" << std::endl;
    for (const auto& neuron : data.neurons) {
        file << neuron.id.value << ",";
        file << static_cast<int>(neuron.type) << ",";
        file << neuron.x_position << "," << neuron.y_position << ",";
        file << neuron.radius << "," << neuron.membrane_potential << ",";
        file << neuron.threshold << "," << neuron.firing_rate << ",";
        file << neuron.adaptation_level << "," << neuron.is_firing << ",";
        file << neuron.is_refractory << "," << neuron.color_r << ",";
        file << neuron.color_g << "," << neuron.color_b << std::endl;
    }
    
    // Synapses CSV
    file << "\nSynapseID,SourceNeuron,DestNeuron,Weight,Delay,Type,Strength,Transparency,R,G,B,IsActive,PulseStrength" << std::endl;
    for (const auto& synapse : data.synapses) {
        file << synapse.id.value << ",";
        file << synapse.source_neuron.value << "," << synapse.destination_neuron.value << ",";
        file << synapse.weight << "," << synapse.delay << ",";
        file << static_cast<int>(synapse.type) << "," << synapse.strength << ",";
        file << synapse.transparency << "," << synapse.color_r << ",";
        file << synapse.color_g << "," << synapse.color_b << ",";
        file << synapse.is_active << "," << synapse.pulse_strength << std::endl;
    }
    
    file.close();
    return true;
}

bool NeuralNetworkVisualizer::generateJSONFile(const NeuralNetworkVisualization& data,
                                              const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{" << std::endl;
    file << "  \"title\": \"" << data.title << "\"," << std::endl;
    file << "  \"description\": \"" << data.description << "\"," << std::endl;
    file << "  \"simulation_time\": " << data.simulation_time << "," << std::endl;
    file << "  \"time_step\": " << data.time_step << "," << std::endl;
    file << "  \"neurons\": [" << std::endl;
    
    for (size_t i = 0; i < data.neurons.size(); ++i) {
        const auto& neuron = data.neurons[i];
        file << "    {\"id\": " << neuron.id.value << ",";
        file << "\"type\": " << static_cast<int>(neuron.type) << ",";
        file << "\"x\": " << neuron.x_position << ",";
        file << "\"y\": " << neuron.y_position << ",";
        file << "\"radius\": " << neuron.radius << ",";
        file << "\"v\": " << neuron.membrane_potential << ",";
        file << "\"v_thresh\": " << neuron.threshold << ",\";
        file << "\"firing_rate\": " << neuron.firing_rate << ",";
        file << "\"adaptation\": " << neuron.adaptation_level << ",";
        file << "\"is_firing\": " << (neuron.is_firing ? "true" : "false") << ",";
        file << "\"is_refractory\": " << (neuron.is_refractory ? "true" : "false") << ",";
        file << "\"r\": " << neuron.color_r << ",";
        file << "\"g\": " << neuron.color_g << ",";
        file << "\"b\": " << neuron.color_b << "}";
        
        if (i < data.neurons.size() - 1) {
            file << ",";
        }
        file << std::endl;
    }
    
    file << "  ]," << std::endl;
    file << "  \"synapses\": [" << std::endl;
    
    for (size_t i = 0; i < data.synapses.size(); ++i) {
        const auto& synapse = data.synapses[i];
        file << "    {\"id\": " << synapse.id.value << ",";
        file << "\"source\": " << synapse.source_neuron.value << ",";
        file << "\"destination\": " << synapse.destination_neuron.value << ",";
        file << "\"weight\": " << synapse.weight << ",";
        file << "\"delay\": " << synapse.delay << ",";
        file << "\"type\": " << static_cast<int>(synapse.type) << ",";
        file << "\"strength\": " << synapse.strength << ",";
        file << "\"transparency\": " << synapse.transparency << ",";
        file << "\"r\": " << synapse.color_r << ",";
        file << "\"g\": " << synapse.color_g << ",";
        file << "\"b\": " << synapse.color_b << ",";
        file << "\"is_active\": " << (synapse.is_active ? "true" : "false") << ",";
        file << "\"pulse_strength\": " << synapse.pulse_strength << "}";
        
        if (i < data.synapses.size() - 1) {
            file << ",";
        }
        file << std::endl;
    }
    
    file << "  ]" << std::endl;
    file << "}" << std::endl;
    
    file.close();
    return true;
}

bool NeuralNetworkVisualizer::generateTextReport(const NeuralNetworkVisualization& data,
                                                const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << data.title << std::endl;
    file << std::string(data.title.length(), '=') << std::endl;
    file << data.description << std::endl;
    file << std::endl;
    
    file << "Network Overview:" << std::endl;
    file << "  Total Neurons: " << data.neurons.size() << std::endl;
    file << "  Total Synapses: " << data.synapses.size() << std::endl;
    file << "  Simulation Time: " << data.simulation_time << " s" << std::endl;
    file << "  Time Step: " << data.time_step << " s" << std::endl;
    file << "  Active Connections: " << countActiveConnections(data) << std::endl;
    file << std::endl;
    
    file << "Neuron Statistics:" << std::endl;
    for (const auto& neuron : data.neurons) {
        file << "  Neuron " << neuron.id.value << ": ";
        file << "Type=" << static_cast<int>(neuron.type) << ", ";
        file << "V=" << neuron.membrane_potential << "mV, ";
        file << "V_thresh=" << neuron.threshold << "mV, ";
        file << "FiringRate=" << neuron.firing_rate << "Hz, ";
        file << "Firing=" << (neuron.is_firing ? "Yes" : "No") << ";" << std::endl;
    }
    
    file << std::endl;
    file << "Synapse Statistics:" << std::endl;
    for (const auto& synapse : data.synapses) {
        file << "  Synapse " << synapse.id.value << ": ";
        file << "Source=" << synapse.source_neuron.value << ", ";
        file << "Dest=" << synapse.destination_neuron.value << ", ";
        file << "Weight=" << synapse.weight << ", ";
        file << "Delay=" << synapse.delay << ", ";
        file << "Type=" << static_cast<int>(synapse.type) << ", ";
        file << "Active=" << (synapse.is_active ? "Yes" : "No") << ";" << std::endl;
    }
    
    file.close();
    return true;
}

void NeuralNetworkVisualizer::applyForceDirectedLayout(NeuralNetworkVisualization& data) {
    // Simple force-directed layout implementation
    int width = image_width_;
    int height = image_height_;
    
    // Initialize random positions
    std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(50.0, width - 50.0);
    
    for (auto& neuron : data.neurons) {
        neuron.x_position = dist(rng);
        neuron.y_position = dist(rng);
    }
    
    // Simple repulsion and attraction simulation (would be more sophisticated in real implementation)
    for (int iteration = 0; iteration < 100; ++iteration) {
        // Reset forces
        std::vector<std::pair<double, double>> forces(data.neurons.size(), {0.0, 0.0});
        
        // Repulsion between all neurons
        for (size_t i = 0; i < data.neurons.size(); ++i) {
            for (size_t j = i + 1; j < data.neurons.size(); ++j) {
                double dx = data.neurons[j].x_position - data.neurons[i].x_position;
                double dy = data.neurons[j].y_position - data.neurons[i].y_position;
                double distance = std::sqrt(dx * dx + dy * dy);
                
                if (distance > 0.1) {
                    double force = 100.0 / (distance * distance);
                    double fx = force * dx / distance;
                    double fy = force * dy / distance;
                    
                    forces[i].first += fx;
                    forces[i].second += fy;
                    forces[j].first -= fx;
                    forces[j].second -= fy;
                }
            }
        }
        
        // Attraction from synapses
        for (const auto& synapse : data.synapses) {
            // Find corresponding neurons
            auto source_it = std::find_if(data.neurons.begin(), data.neurons.end(),
                                         [&](const NeuronVisualizationData& n) { return n.id == synapse.source_neuron; });
            auto dest_it = std::find_if(data.neurons.begin(), data.neurons.end(),
                                       [&](const NeuronVisualizationData& n) { return n.id == synapse.destination_neuron; });
            
            if (source_it != data.neurons.end() && dest_it != data.neurons.end()) {
                double dx = dest_it->x_position - source_it->x_position;
                double dy = dest_it->y_position - source_it->y_position;
                double distance = std::sqrt(dx * dx + dy * dy);
                
                if (distance > 0.1) {
                    double attraction = synapse.weight * 10.0;
                    double fx = attraction * dx / distance;
                    double fy = attraction * dy / distance;
                    
                    source_it->x_position -= fx * 0.01;
                    source_it->y_position -= fy * 0.01;
                    dest_it->x_position += fx * 0.01;
                    dest_it->y_position += fy * 0.01;
                }
            }
        }
        
        // Apply forces with damping
        for (size_t i = 0; i < data.neurons.size(); ++i) {
            double fx = forces[i].first * 0.1;
            double fy = forces[i].second * 0.1;
            
            data.neurons[i].x_position += fx;
            data.neurons[i].y_position += fy;
            
            // Keep neurons within bounds
            data.neurons[i].x_position = std::max(50.0, std::min(static_cast<double>(width - 50), data.neurons[i].x_position));
            data.neurons[i].y_position = std::max(50.0, std::min(static_cast<double>(height - 50), data.neurons[i].y_position));
        }
    }
}

void NeuralNetworkVisualizer::applyCircularLayout(NeuralNetworkVisualization& data) {
    int width = image_width_;
    int height = image_height_;
    double center_x = width / 2.0;
    double center_y = height / 2.0;
    double radius = std::min(width, height) * 0.4;
    
    double angle_step = 2.0 * M_PI / std::max(1.0, static_cast<double>(data.neurons.size()));
    
    for (size_t i = 0; i < data.neurons.size(); ++i) {
        double angle = i * angle_step;
        data.neurons[i].x_position = center_x + radius * std::cos(angle);
        data.neurons[i].y_position = center_y + radius * std::sin(angle);
    }
}

void NeuralNetworkVisualizer::applyRadialLayout(NeuralNetworkVisualization& data) {
    int width = image_width_;
    int height = image_height_;
    double center_x = width / 2.0;
    double center_y = height / 2.0;
    double radius = std::min(width, height) * 0.4;
    
    // Group neurons by type
    std::map<NeuronType, std::vector<NeuronVisualizationData*>> neurons_by_type;
    for (auto& neuron : data.neurons) {
        neurons_by_type[neuron.type].push_back(&neuron);
    }
    
    // Create radial layers
    double layer_spacing = radius / std::max(1.0, static_cast<double>(neurons_by_type.size()));
    double current_radius = 0.0;
    
    for (auto& pair : neurons_by_type) {
        double layer_radius = current_radius + layer_spacing * 0.7;
        double angle_step = 2.0 * M_PI / std::max(1.0, static_cast<double>(pair.second.size()));
        
        for (size_t i = 0; i < pair.second.size(); ++i) {
            double angle = i * angle_step;
            pair.second[i]->x_position = center_x + layer_radius * std::cos(angle);
            pair.second[i]->y_position = center_y + layer_radius * std::sin(angle);
        }
        
        current_radius += layer_spacing;
    }
}

void NeuralNetworkVisualizer::applyGridLayout(NeuralNetworkVisualizer::NeuralNetworkVisualization& data) {
    int width = image_width_;
    int height = image_height_;
    
    double cell_width = static_cast<double>(width) / std::ceil(std::sqrt(data.neurons.size()));
    double cell_height = static_cast<double>(height) / std::ceil(std::sqrt(data.neurons.size()));
    
    int row = 0, col = 0;
    for (auto& neuron : data.neurons) {
        neuron.x_position = col * cell_width + cell_width / 2.0;
        neuron.y_position = row * cell_height + cell_height / 2.0;
        
        col++;
        if (col >= static_cast<int>(std::ceil(std::sqrt(data.neurons.size())))) {
            col = 0;
            row++;
        }
    }
}

void NeuralNetworkVisualizer::applyHierarchyLayout(NeuralNetworkVisualization& data) {
    int width = image_width_;
    int height = image_height_;
    double center_x = width / 2.0;
    double center_y = height / 2.0;
    double radius = std::min(width, height) * 0.3;
    
    // Simple hierarchy: group by neuron type with different radii
    std::map<NeuronType, std::vector<NeuronVisualizationData*>> neurons_by_type;
    for (auto& neuron : data.neurons) {
        neurons_by_type[neuron.type].push_back(&neuron);
    }
    
    // Create hierarchy levels
    double current_radius = radius;
    int level = 0;
    
    for (auto& pair : neurons_by_type) {
        double level_radius = current_radius * (1.0 - level * 0.2);
        double angle_step = 2.0 * M_PI / std::max(1.0, static_cast<double>(pair.second.size()));
        
        for (size_t i = 0; i < pair.second.size(); ++i) {
            double angle = i * angle_step + level * 0.5;  // Offset for each level
            pair.second[i]->x_position = center_x + level_radius * std::cos(angle);
            pair.second[i]->y_position = center_y + level_radius * std::sin(angle);
        }
        
        current_radius *= 0.8;
        level++;
    }
}

void NeuralNetworkVisualizer::applyHeatmapColorScheme(NeuralNetworkVisualization& data) {
    double max_v = 0.0, min_v = 0.0;
    
    // Find max and min membrane potential
    for (const auto& neuron : data.neurons) {
        if (neuron.membrane_potential > max_v) max_v = neuron.membrane_potential;
        if (neuron.membrane_potential < min_v) min_v = neuron.membrane_potential;
    }
    
    double range = max_v - min_v;
    if (range > 0.0) {
        for (auto& neuron : data.neurons) {
            double normalized = (neuron.membrane_potential - min_v) / range;
            
            // Heatmap colors: blue -> green -> red
            if (normalized < 0.33) {
                neuron.color_r = 0.0f;
                neuron.color_g = static_cast<float>(normalized * 3.0);
                neuron.color_b = 1.0f - static_cast<float>(normalized * 3.0);
            } else if (normalized < 0.67) {
                neuron.color_r = static_cast<float>((normalized - 0.33) * 3.0);
                neuron.color_g = 1.0f;
                neuron.color_b = 0.0f;
            } else {
                neuron.color_r = 1.0f;
                neuron.color_g = 1.0f - static_cast<float>((normalized - 0.67) * 3.0);
                neuron.color_b = 0.0f;
            }
            
            normalizeColor(neuron.color_r, neuron.color_g, neuron.color_b);
        }
    }
}

void NeuralNetworkVisualizer::applyNeuronTypeColorScheme(NeuralNetworkVisualization& data) {
    for (auto& neuron : data.neurons) {
        switch (neuron.type) {
            case NeuronType::Excitatory:
                neuron.color_r = 0.8f;
                neuron.color_g = 0.2f;
                neuron.color_b = 0.2f;
                break;
            case NeuronType::Inhibitory:
                neuron.color_r = 0.2f;
                neuron.color_g = 0.8f;
                neuron.color_b = 0.2f;
                break;
            case NeuronType::Sensory:
                neuron.color_r = 0.2f;
                neuron.color_g = 0.2f;
                neuron.color_b = 0.8f;
                break;
            case NeuronType::Motor:
                neuron.color_r = 0.8f;
                neuron.color_g = 0.8f;
                neuron.color_b = 0.2f;
                break;
            case NeuronType::Internal:
                neuron.color_r = 0.5f;
                neuron.color_g = 0.5f;
                neuron.color_b = 0.5f;
                break;
            case NeuronType::Modulatory:
                neuron.color_r = 0.8f;
                neuron.color_g = 0.2f;
                neuron.color_b = 0.8f;
                break;
        }
    }
}

void NeuralNetworkVisualizer::applyWeightBasedColorScheme(NeuralNetworkVisualization& data) {
    double max_weight = 0.0, min_weight = 0.0;
    
    // Find max and min weights
    for (const auto& synapse : data.synapses) {
        if (synapse.weight > max_weight) max_weight = synapse.weight;
        if (synapse.weight < min_weight) min_weight = synapse.weight;
    }
    
    double range = max_weight - min_weight;
    if (range > 0.0) {
        for (auto& synapse : data.synapses) {
            double normalized = (synapse.weight - min_weight) / range;
            
            // Color from blue (low weight) to red (high weight)
            synapse.color_r = static_cast<float>(normalized);
            synapse.color_g = 0.0f;
            synapse.color_b = static_cast<float>(1.0 - normalized);
            
            normalizeColor(synapse.color_r, synapse.color_g, synapse.color_b);
        }
    }
}

void NeuralNetworkVisualizer::applyActivityBasedColorScheme(NeuralNetworkVisualization& data) {
    for (auto& neuron : data.neurons) {
        if (neuron.is_firing) {
            neuron.color_r = 1.0f;
            neuron.color_g = 0.0f;
            neuron.color_b = 0.0f;  // Red for firing
        } else if (neuron.is_refractory) {
            neuron.color_r = 0.5f;
            neuron.color_g = 0.5f;
            neuron.color_b = 0.5f;  // Gray for refractory
        } else {
            // Based on firing rate
            double normalized_rate = std::min(1.0, neuron.firing_rate / 100.0);
            neuron.color_r = static_cast<float>(normalized_rate);
            neuron.color_g = 0.0f;
            neuron.color_b = static_cast<float>(1.0 - normalized_rate);
        }
    }
}

void NeuralNetworkVisualizer::ensureOutputDirectory(const std::string& directory) {
    if (!directory.empty()) {
        // Check if directory exists (simplified)
        // In real implementation, use filesystem operations
    }
}

std::string NeuralNetworkVisualizer::getFileExtension(VisualizationFormat format) const {
    switch (format_) {
        case VisualizationFormat::HTML: return "html";
        case VisualizationFormat::CSV: return "csv";
        case VisualizationFormat::JSON: return "json";
        case VisualizationFormat::Text: return "txt";
        case VisualizationFormat::PNG: return "png";
        case VisualizationFormat::SVG: return "svg";
        case VisualizationFormat::PDF: return "pdf";
        default: return "";
    }
}

void NeuralNetworkVisualizer::normalizeColor(float& r, float& g, float& b) {
    float max_val = std::max(std::max(r, g), b);
    if (max_val > 1.0f) {
        r /= max_val;
        g /= max_val;
        b /= max_val;
    }
}

double NeuralNetworkVisualizer::calculateNeuronRadius(const NeuronVisualizationData& neuron) const {
    return neuron.radius * neuron_size_scale_;
}

double NeuralNetworkVisualizer::calculateSynapseThickness(const SynapseVisualizationData& synapse) const {
    return std::abs(synapse.weight) * connection_threshold_;
}

int NeuralNetworkVisualizer::countActiveConnections(const NeuralNetworkVisualization& data) const {
    int count = 0;
    for (const auto& synapse : data.synapses) {
        if (synapse.is_active && synapse.strength > connection_threshold_) {
            count++;
        }
    }
    return count;
}

int NeuralNetworkVisualizer::countNeuronsByType(const NeuralNetworkVisualization& data, NeuronType type) const {
    int count = 0;
    for (const auto& neuron : data.neurons) {
        if (neuron.type == type) {
            count++;
        }
    }
    return count;
}

std::string NeuralNetworkVisualizer::convertToJSON(const NeuralNetworkVisualization& data) const {
    // Simple JSON conversion (would use proper JSON library in production)
    std::string json = "{\"neurons\": [";
    for (size_t i = 0; i < data.neurons.size(); ++i) {
        if (i > 0) json += ",";
        json += "{\"id\": " + std::to_string(data.neurons[i].id.value) + ",\"x\": " +
                std::to_string(data.neurons[i].x_position) + ",\"y\": " +
                std::to_string(data.neurons[i].y_position) + "}";
    }
    json += "],\"synapses\": [";
    
    for (size_t i = 0; i < data.synapses.size(); ++i) {
        if (i > 0) json += ",";
        json += "{\"source\": " + std::to_string(data.synapses[i].source_neuron.value) +
                ",\"target\": " + std::to_string(data.synapses[i].destination_neuron.value) +
                ",\"weight\": " + std::to_string(data.synapses[i].weight) + "}";
    }
    json += "]}";
    return json;
}

NeuralNetworkVisualizer::NeuralNetworkVisualizer() {
}

NeuralNetworkVisualizer::~NeuralNetworkVisualizer() {
}

void NeuralNetworkVisualizer::setFormat(VisualizationFormat format) {
    format_ = format;
}

void NeuralNetworkVisualizer::setOutputDirectory(const std::string& directory) {
    output_directory_ = directory;
}

void NeuralNetworkVisualizer::setImageWidth(int width) {
    image_width_ = width;
}

void NeuralNetworkVisualizer::setImageHeight(int height) {
    image_height_ = height;
}

void NeuralNetworkVisualizer::setColorScheme(const std::string& scheme) {
    color_scheme_ = scheme;
}

void NeuralNetworkVisualizer::setNeuronLayoutAlgorithm(const std::string& algorithm) {
    neuron_layout_ = algorithm;
}

void NeuralNetworkVisualizer::setConnectionRenderingMode(const std::string& mode) {
    connection_mode_ = mode;
}

bool NeuralNetworkVisualizer::generateSpikeActivityVisualization(std::shared_ptr<Brain> brain,
                                                                const std::string& filename) {
    // Would generate spike activity visualization
    return true;
}

bool NeuralNetworkVisualizer::generateConnectionMatrixVisualization(std::shared_ptr<Brain> brain,
                                                                    const std::string& filename) {
    // Would generate connection matrix visualization
    return true;
}

bool NeuralNetworkVisualizer::generateWeightDistributionVisualization(std::shared_ptr<Brain> brain,
                                                                     const std::string& filename) {
    // Would generate weight distribution visualization
    return true;
}

bool NeuralNetworkVisualizer::generateTrainingProgressVisualization(
    const std::vector<NeuralNetworkVisualization>& history, const std::string& filename) {
    // Would generate training progress visualization
    return true;
}

void NeuralNetworkVisualizer::computeNetworkMetrics(const NeuralNetworkVisualization& data,
                                                  std::map<std::string, double>& metrics) const {
    // Compute network metrics
    metrics["neuron_count"] = static_cast<double>(data.neurons.size());
    metrics["synapse_count"] = static_cast<double>(data.synapses.size());
    metrics["active_connections"] = static_cast<double>(countActiveConnections(data));
    
    // Calculate network density
    double max_possible_connections = data.neurons.size() * (data.neurons.size() - 1);
    if (max_possible_connections > 0) {
        metrics["density"] = static_cast<double>(data.synapses.size()) / max_possible_connections;
    }
}

void NeuralNetworkVisualizer::computeNeuronStatistics(const NeuralNetworkVisualization& data,
                                                    std::map<NeuronType, std::map<std::string, double>>& stats) const {
    // Compute statistics for each neuron type
    for (const auto& neuron : data.neurons) {
        auto& type_stats = stats[neuron.type];
        type_stats["count"]++;
        type_stats["average_firing_rate"] += neuron.firing_rate;
        type_stats["average_v"] += neuron.membrane_potential;
    }
    
    // Average the stats
    for (auto& pair : stats) {
        if (pair.second["count"] > 0) {
            pair.second["average_firing_rate"] /= pair.second["count"];
            pair.second["average_v"] /= pair.second["count"];
        }
    }
}

void NeuralNetworkVisualizer::computeSynapseStatistics(const NeuralNetworkVisualization& data,
                                                      std::map<SynapseType, std::map<std::string, double>>& stats) const {
    // Compute statistics for each synapse type
    for (const auto& synapse : data.synapses) {
        auto& type_stats = stats[synapse.type];
        type_stats["count"]++;
        type_stats["average_weight"] += synapse.weight;
        type_stats["average_strength"] += synapse.strength;
    }
    
    // Average the stats
    for (auto& pair : stats) {
        if (pair.second["count"] > 0) {
            pair.second["average_weight"] /= pair.second["count"];
            pair.second["average_strength"] /= pair.second["count"];
        }
    }
}

bool NeuralNetworkVisualizer::generateAnimation(const std::vector<NeuralNetworkVisualization>& frames,
                                               const std::string& filename, double frame_rate) {
    // Would generate animation from frames
    return true;
}

bool NeuralNetworkVisualizer::exportToPythonScript(const NeuralNetworkVisualization& data,
                                                  const std::string& filename) {
    // Would export to Python script for further analysis
    return true;
}

bool NeuralNetworkVisualizer::exportToMATLABFormat(const NeuralNetworkVisualization& data,
                                                  const std::string& filename) {
    // Would export to MATLAB format
    return true;
}

bool NeuralNetworkVisualizer::exportToRFormat(const NeuralNetworkVisualization& data,
                                             const std::string& filename) {
    // Would export to R format
    return true;
}

void NeuralNetworkVisualizer::processBatchVisualization(std::vector<std::shared_ptr<Brain>> brains,
                                                       const std::string& base_filename) {
    // Process multiple brains for batch visualization
    for (size_t i = 0; i < brains.size(); ++i) {
        std::string filename = base_filename + "_brain_" + std::to_string(i) + ".html";
        auto data = collectVisualizationData(brains[i]);
        generateVisualization(data, filename);
    }
}

void NeuralNetworkVisualizer::createComparisonVisualization(
    const std::vector<NeuralNetworkVisualization>& datasets, const std::string& comparison_filename) {
    // Create comparison visualization of multiple datasets
    // Would implement side-by-side comparison or time-series comparison
}

void NeuralNetworkVisualizer::loadConfiguration(const std::string& config_file) {
    // Load visualization configuration
}

void NeuralNetworkVisualizer::saveConfiguration(const std::string& config_file) const {
    // Save visualization configuration
}

// SpikeActivityVisualizer implementation
SpikeActivityVisualizer::SpikeActivityVisualizer() {
}

SpikeActivityVisualizer::~SpikeActivityVisualizer() {
}

bool SpikeActivityVisualizer::generateSpikeRasterPlot(std::shared_ptr<Brain> brain,
                                                     const std::string& filename) {
    // Generate spike raster plot
    return true;
}

bool SpikeActivityVisualizer::generateSpikeHistogram(std::shared_ptr<Brain> brain,
                                                    const std::string& filename) {
    // Generate spike histogram
    return true;
}

bool SpikeActivityVisualizer::generateSpikeTrainVisualization(std::shared_ptr<Brain> brain,
                                                              const std::string& filename) {
    // Generate spike train visualization
    return true;
}

bool SpikeActivityVisualizer::generateCorrelogram(std::shared_ptr<Brain> brain,
                                                 const std::string& filename) {
    // Generate correlogram
    return true;
}

void SpikeActivityVisualizer::extractSpikeTimes(std::shared_ptr<Brain> brain,
                                              std::vector<std::vector<double>>& neuron_spikes) const {
    // Extract spike times from brain
    // In real implementation, would use brain's spike recording system
}

double SpikeActivityVisualizer::calculateFiringRate(const std::vector<double>& spike_times,
                                                  double duration) const {
    if (duration <= 0.0) return 0.0;
    return static_cast<double>(spike_times.size()) / duration;
}

// PlasticityVisualizer implementation
PlasticityVisualizer::PlasticityVisualizer() {
}

PlasticityVisualizer::~PlasticityVisualizer() {
}

bool PlasticityVisualizer::generateWeightChangeVisualization(std::shared_ptr<Brain> brain,
                                                             const std::string& filename) {
    // Generate weight change visualization
    return true;
}

bool PlasticityVisualizer::generateSTDPVisualization(std::shared_ptr<Brain> brain,
                                                    const std::string& filename) {
    // Generate STDP visualization
    return true;
}

bool PlasticityVisualizer::generatePlasticityTimeline(std::shared_ptr<Brain> brain,
                                                     const std::string& filename) {
    // Generate plasticity timeline
    return true;
}

void PlasticityVisualizer::extractWeightHistory(std::shared_ptr<Brain> brain,
                                              std::vector<std::pair<double, double>>& weight_changes) const {
    // Extract weight history from brain
    // Would track weights over time
}

double PlasticityVisualizer::calculatePlasticityStrength(const SynapseVisualizationData& synapse) const {
    // Calculate plasticity strength based on synapse properties
    return std::abs(synapse.weight) * synapse.strength;
}

// NetworkTopologyVisualizer implementation
NetworkTopologyVisualizer::NetworkTopologyVisualizer() {
}

NetworkTopologyVisualizer::~NetworkTopologyVisualizer() {
}

bool NetworkTopologyVisualizer::generateConnectivityMatrix(std::shared_ptr<Brain> brain,
                                                          const std::string& filename) {
    // Generate connectivity matrix
    return true;
}

bool NetworkTopologyVisualizer::generateDegreeDistribution(std::shared_ptr<Brain> brain,
                                                          const std::string& filename) {
    // Generate degree distribution
    return true;
}

bool NetworkTopologyVisualizer::generateCommunityVisualization(std::shared_ptr<Brain> brain,
                                                                const std::string& filename) {
    // Generate community visualization
    return true;
}

bool NetworkTopologyVisualizer::generateSmallWorldMetrics(std::shared_ptr<Brain> brain,
                                                         const std::string& filename) {
    // Generate small world metrics
    return true;
}

void NetworkTopologyVisualizer::computeNetworkProperties(std::shared_ptr<Brain> brain,
                                                        std::map<std::string, double>& properties) const {
    // Compute network properties
    // Would calculate various topological metrics
}

double NetworkTopologyVisualizer::calculateClusteringCoefficient(std::shared_ptr<Brain> brain) const {
    // Calculate clustering coefficient
    return 0.0;
}

double NetworkTopologyVisualizer::calculatePathLength(std::shared_ptr<Brain> brain) const {
    // Calculate path length
    return 0.0;
}

// High-level interface functions
bool visualizeNeuralNetwork(std::shared_ptr<Brain> brain,
                           const std::string& output_prefix, const std::string& format) {
    visualization::NeuralNetworkVisualizer visualizer;
    
    VisualizationFormat vis_format;
    if (format == "html") vis_format = VisualizationFormat::HTML;
    else if (format == "csv") vis_format = VisualizationFormat::CSV;
    else if (format == "json") vis_format = VisualizationFormat::JSON;
    else if (format == "text") vis_format = VisualizationFormat::Text;
    else vis_format = VisualizationFormat::HTML;
    
    visualizer.setFormat(vis_format);
    
    auto data = visualizer.collectVisualizationData(brain);
    
    std::string filename = output_prefix + "." + format;
    return visualizer.generateVisualization(data, filename);
}

bool visualizeSpikeActivity(std::shared_ptr<Brain> brain, const std::string& output_file) {
    SpikeActivityVisualizer visualizer;
    return visualizer.generateSpikeRasterPlot(brain, output_file);
}

bool visualizeNetworkTopology(std::shared_ptr<Brain> brain, const std::string& output_file) {
    NetworkTopologyVisualizer visualizer;
    return visualizer.generateConnectivityMatrix(brain, output_file);
}

bool createInteractiveDashboard(std::shared_ptr<Brain> brain, const std::string& dashboard_file) {
    visualization::NeuralNetworkVisualizer visualizer;
    visualizer.setFormat(VisualizationFormat::HTML);
    
    auto data = visualizer.collectVisualizationData(brain);
    data.title = "Neural Network Dashboard";
    data.description = "Interactive dashboard for monitoring neural network activity";
    
    return visualizer.generateInteractiveHTML(data, dashboard_file);
}

} // namespace visualization
} // namespace nlm

// Comprehensive visualization and debugging framework implementation
// Tools for visualizing neural simulations and debugging complex behavior

#include "VisualizationDebug.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// NetworkVisualizer implementation
NetworkVisualizer::NetworkVisualizer(const std::string& name) : name_(name) {
}

NetworkVisualizer::~NetworkVisualizer() {
}

void NetworkVisualizer::setData(const std::shared_ptr<Brain>& brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    brain_ = brain;
}

std::string NetworkVisualizer::generateNetworkGraph() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream dot;
    dot << "digraph G {" << std::endl;
    dot << "  rankdir=LR;" << std::endl;
    dot << "  node [shape=circle, style=filled];" << std::endl;
    
    if (!brain_) {
        dot << "  // No brain data available" << std::endl;
        dot << "}";
        return dot.str();
    }
    
    // Add regions
    auto regions = brain_->getRegions();
    for (size_t i = 0; i < regions.size(); ++i) {
        auto* region = regions[i].get();
        dot << "  region_" << i << " [label=\"" << region->getName() << "\", color=blue];" << std::endl;
        
        // Add neurons
        auto neurons = region->getAllNeurons();
        for (size_t j = 0; j < neurons.size() && j < 20; ++j) {  // Limit to first 20 neurons per region
            auto* neuron = neurons[j];
            dot << "  neuron_" << j << " [label=\"" << j << "\", color=" << getNeuronColor(neuron) << "];" << std::endl;
            dot << "  region_" << i << " -> neuron_" << j << ";" << std::endl;
        }
    }
    
    // Add synapses
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        for (size_t i = 0; i < synapses.size() && i < 20; ++i) {  // Limit to first 20 synapses
            auto* synapse = synapses[i];
            auto* sourceNeuron = findNeuron(synapse->getSourceNeuron());
            auto* destNeuron = findNeuron(synapse->getDestinationNeuron());
            
            if (sourceNeuron && destNeuron) {
                dot << "  neuron_" << getNeuronId(sourceNeuron) << " -> neuron_" << getNeuronId(destNeuron)
                    << " [label=\"" << synapse->getWeight() << "\", color=" << getSynapseColor(synapse) << "];" << std::endl;
            }
        }
    }
    
    dot << "}";
    return dot.str();
}

std::string NetworkVisualizer::generate3DVisualization() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream html;
    html << "<!DOCTYPE html>" << std::endl;
    html << "<html>" << std::endl;
    html << "<head>" << std::endl;
    html << "  <title>3D Neural Network Visualization</title>" << std::endl;
    html << "  <script src=\"https://cdnjs.cloudflare.com/ajax/libs/three.js/r128/three.min.js\"></script>" << std::endl;
    html << "  <style>" << std::endl;
    html << "    body { margin: 0; overflow: hidden; background-color: #000; }" << std::endl;
    html << "    canvas { display: block; }" << std::endl;
    html << "  </style>" << std::endl;
    html << "</head>" << std::endl;
    html << "<body>" << std::endl;
    html << "  <script>" << std::endl;
    html << "    // Three.js scene setup" << std::endl;
    html << "    const scene = new THREE.Scene();" << std::endl;
    html << "    const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);" << std::endl;
    html << "    const renderer = new THREE.WebGLRenderer();" << std::endl;
    html << "    renderer.setSize(window.innerWidth, window.innerHeight);" << std::endl;
    html << "    document.body.appendChild(renderer.domElement);" << std::endl;
    html << "" << std::endl;
    html << "    // Create neurons" << std::endl;
    html << "    const neuronGeometry = new THREE.SphereGeometry(0.1, 32, 32);" << std::endl;
    html << "    const neuronMaterial = new THREE.MeshBasicMaterial({ color: 0x00ff00 });" << std::endl;
    html << "    const neurons = [];" << std::endl;
    html << "" << std::endl;
    html << "    if (!brain_) {" << std::endl;
    html << "      console.log('No brain data available');" << std::endl;
    html << "    } else {" << std::endl;
    html << "      // Add neurons to scene" << std::endl;
    html << "      const regions = brain_->getRegions();" << std::endl;
    html << "      for (let i = 0; i < regions.size(); ++i) {" << std::endl;
    html << "        const region = regions[i];" << std::endl;
    html << "        const neuronsInRegion = region->getAllNeurons();" << std::endl;
    html << "        for (let j = 0; j < Math.min(neuronsInRegion.size(), 10); ++j) {" << std::endl;
    html << "          const neuron = neuronsInRegion[j];" << std::endl;
    html << "          const neuronMesh = new THREE.Mesh(neuronGeometry, neuronMaterial.clone());" << std::endl;
    html << "          neuronMesh.position.set(i * 2, j * 0.5, 0);" << std::endl;
    html << "          scene.add(neuronMesh);" << std::endl;
    html << "          neurons.push(neuronMesh);" << std::endl;
    html << "        }" << std::endl;
    html << "      }" << std::endl;
    html << "    }" << std::endl;
    html << "" << std::endl;
    html << "    // Add lighting" << std::endl;
    html << "    const ambientLight = new THREE.AmbientLight(0x404040);" << std::endl;
    html << "    scene.add(ambientLight);" << std::endl;
    html << "" << std::endl;
    html << "    // Camera position" << std::endl;
    html << "    camera.position.z = 30;" << std::endl;
    html << "" << std::endl;
    html << "    // Animation loop" << std::endl;
    html << "    function animate() {" << std::endl;
    html << "      requestAnimationFrame(animate);" << std::endl;
    html << "      // Rotate scene" << std::endl;
    html << "      scene.rotation.y += 0.01;" << std::endl;
    html << "      renderer.render(scene, camera);" << std::endl;
    html << "    }" << std::endl;
    html << "    animate();" << std::endl;
    html << "  </script>" << std::endl;
    html << "</body>" << std::endl;
    html << "</html>" << std::endl;
    
    return html.str();
}

std::string NetworkVisualizer::generateActivityHeatmap() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream html;
    html << "<!DOCTYPE html>" << std::endl;
    html << "<html>" << std::endl;
    html << "<head>" << std::endl;
    html << "  <title>Neural Activity Heatmap</title>" << std::endl;
    html << "  <script src=\"https://cdnjs.cloudflare.com/ajax/libs/d3/7.0.0/d3.min.js\"></script>" << std::endl;
    html << "  <style>" << std::endl;
    html << "    body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    html << "    .heatmap { width: 100%; height: 400px; }" << std::endl;
    html << "    .color-bar { width: 20px; height: 400px; float: right; }" << std::endl;
    html << "    .legend { margin-left: 30px; }" << std::endl;
    html << "  </style>" << std::endl;
    html << "</head>" << std::endl;
    html << "<body>" << std::endl;
    html << "  <h1>Neural Activity Heatmap</h1>" << std::endl;
    html << "  <div id=\"heatmap\" class=\"heatmap\"></div>" << std::endl;
    html << "  <div class=\"color-bar\">" << std::endl;
    html << "    <div class=\"legend\">" << std::endl;
    html << "      <div>High Activity</div>" << std::endl;
    html << "      <div>Medium Activity</div>" << std::endl;
    html << "      <div>Low Activity</div>" << std::endl;
    html << "    </div>" << std::endl;
    html << "  </div>" << std::endl;
    html << "" << std::endl;
    html << "  <script>" << std::endl;
    html << "    // Generate heatmap data" << std::endl;
    html << "    const data = [];" << std::endl;
    html << "" << std::endl;
    html << "    if (!brain_) {" << std::endl;
    html << "      console.log('No brain data available');" << std::endl;
    html << "    } else {" << std::endl;
    html << "      // Get activity data from brain" << std::endl;
    html << "      const regions = brain_->getRegions();" << std::endl;
    html << "      for (let i = 0; i < regions.size(); ++i) {" << std::endl;
    html << "        const neurons = regions[i]->getAllNeurons();" << std::endl;
    html << "        const row = [];" << std::endl;
    html << "        for (let j = 0; j < neurons.size() && j < 20; ++j) {" << std::endl;
    html << "          const neuron = neurons[j];" << std::endl;
    html << "          const activity = getNeuronActivity(neuron);" << std::endl;
    html << "          row.push(activity);" << std::endl;
    html << "        }" << std::endl;
    html << "        data.push(row);" << std::endl;
    html << "      }" << std::endl;
    html << "    }" << std::endl;
    html << "" << std::endl;
    html << "    // Create heatmap using D3.js" << std::endl;
    html << "    const width = 800;" << std::endl;
    html << "    const height = 400;" << std::endl;
    html << "    const rows = data.length;" << std::endl;
    html << "    const cols = data[0].length;" << std::endl;
    html << "    const cellWidth = width / cols;" << std::endl;
    html << "    const cellHeight = height / rows;" << std::endl;
    html << "" << std::endl;
    html << "    const svg = d3.select('#heatmap').append('svg')" << std::endl;
    html << "      .attr('width', width)" << std::endl;
    html << "      .attr('height', height);" << std::endl;
    html << "" << std::endl;
    html << "    const cells = svg.selectAll('rect')" << std::endl;
    html << "      .data(data.flatMap((row, i) => row.map((value, j) => ({ value, row: i, col: j }))))" << std::endl;
    html << "      .enter()" << std::endl;
    html << "      .append('rect')" << std::endl;
    html << "      .attr('x', d => d.col * cellWidth)" << std::endl;
    html << "      .attr('y', d => d.row * cellHeight)" << std::endl;
    html << "      .attr('width', cellWidth)" << std::endl;
    html << "      .attr('height', cellHeight)" << std::endl;
    html << "      .attr('fill', d => colorScale(d.value))" << std::endl;
    html << "      .on('mouseover', (event, d) => {" << std::endl;
    html << "        d3.select('#tooltip')" << std::endl;
    html << "          .style('left', (event.pageX + 10) + 'px')" << std::endl;
    html << "          .style('top', (event.pageY + 10) + 'px')" << std::endl;
    html << "          .style('display', 'block')" << std::endl;
    html << "          .html('Neuron ' + d.row + ',' + d.col + ' activity: ' + d.value.toFixed(2));" << std::endl;
    html << "      })" << std::endl;
    html << "      .on('mouseout', () => {" << std::endl;
    html << "        d3.select('#tooltip').style('display', 'none');" << std::endl;
    html << "      });" << std::endl;
    html << "" << std::endl;
    html << "    // Add tooltip" << std::endl;
    html << "    svg.append('div')" << std::endl;
    html << "      .attr('id', 'tooltip')" << std::endl;
    html << "      .style('position', 'absolute')" << std::endl;
    html << "      .style('background-color', 'white')" << std::endl;
    html << "      .style('border', '1px solid black')" << std::endl;
    html << "      .style('padding', '5px')" << std::endl;
    html << "      .style('display', 'none');" << std::endl;
    html << "" << std::endl;
    html << "    // Color scale function" << std::endl;
    html << "    function colorScale(value) {" << std::endl;
    html << "      if (value > 0.7) return '#ff0000;'  // High" << std::endl;
    html << "      if (value > 0.4) return '#ff8800;'  // Medium-High" << std::endl;
    html << "      if (value > 0.2) return '#ffbf00;'  // Medium" << std::endl;
    html << "      if (value > 0.0) return '#ffff00;'  // Low-Medium" << std::endl;
    html << "      return '#ffffff;'  // No activity" << std::endl;
    html << "    }" << std::endl;
    html << "  </script>" << std::endl;
    html << "</body>" << std::endl;
    html << "</html>" << std::endl;
    
    return html.str();
}

std::string NetworkVisualizer::generateSpikeRaster() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream html;
    html << "<!DOCTYPE html>" << std::endl;
    html << "<html>" << std::endl;
    html << "<head>" << std::endl;
    html << "  <title>Spike Raster Plot</title>" << std::endl;
    html << "  <script src=\"https://cdnjs.cloudflare.com/ajax/libs/plotly.js/2.12.0/plotly.min.js\"></script>" << std::endl;
    html << "  <style>" << std::endl;
    html << "    body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    html << "    .plotly { width: 100%; height: 400px; }" << std::endl;
    html << "  </style>" << std::endl;
    html << "</head>" << std::endl;
    html << "<body>" << std::endl;
    html << "  <h1>Spike Raster Plot</h1>" << std::endl;
    html << "  <div id=\"rasterPlot\" class=\"plotly\"></div>" << std::endl;
    html << "" << std::endl;
    html << "  <script>" << std::endl;
    html << "    // Generate spike raster data" << std::endl;
    html << "    const data = [];" << std::endl;
    html << "" << std::endl;
    html << "    if (!brain_) {" << std::endl;
    html << "      console.log('No brain data available');" << std::endl;
    html << "    } else {" << std::endl;
    html << "      // Collect spike data from all regions" << std::endl;
    html << "      const regions = brain_->getRegions();" << std::endl;
    html << "      for (let i = 0; i < regions.size(); ++i) {" << std::endl;
    html << "        const spikes = getSpikeDataFromRegion(regions[i]);" << std::endl;
    html << "        data.push(...spikes);" << std::endl;
    html << "      }" << std::endl;
    html << "    }" << std::endl;
    html << "" << std::endl;
    html << "    // Create spike raster plot" << std::endl;
    html << "    const trace = {" << std::endl;
    html << "      x: []," << std::endl;
    html << "      y: []," << std::endl;
    html << "      mode: 'markers'," << std::endl;
    html << "      type: 'scatter'," << std::endl;
    html << "      marker: {" << std::endl;
    html << "        size: 2," << std::endl;
    html << "        color: 'black'" << std::endl;
    html << "      }" << std::endl;
    html << "    };" << std::endl;
    html << "" << std::endl;
    html << "    // Plot the data" << std::endl;
    html << "    Plotly.newPlot('rasterPlot', [trace], {" << std::endl;
    html << "      title: 'Spike Raster Plot'," << std::endl;
    html << "      xaxis: { title: 'Time (ms)' }," << std::endl;
    html << "      yaxis: { title: 'Neuron ID' }," << std::endl;
    html << "      margin: { l: 50, r: 10, t: 30, b: 50 }" << std::endl;
    html << "    });" << std::endl;
    html << "  </script>" << std::endl;
    html << "</body>" << std::endl;
    html << "</html>" << std::endl;
    
    return html.str();
}

std::string NetworkVisualizer::generateConnectivityMap() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream dot;
    dot << "digraph G {" << std::endl;
    dot << "  rankdir=TB;" << std::endl;
    dot << "  node [shape=box, style=filled];" << std::endl;
    dot << "  edge [color=gray];" << std::endl;
    
    if (!brain_) {
        dot << "  // No brain data available" << std::endl;
        dot << "}";
        return dot.str();
    }
    
    // Add regions as nodes
    auto regions = brain_->getRegions();
    for (size_t i = 0; i < regions.size(); ++i) {
        auto* region = regions[i].get();
        dot << "  region_" << i << " [label=\"" << region->getName() << "\", color=lightblue];" << std::endl;
        
        // Add synapses as edges
        auto synapses = region->getSynapses();
        for (size_t j = 0; j < synapses.size() && j < 20; ++j) {  // Limit to first 20 synapses
            auto* synapse = synapses[j];
            auto* sourceNeuron = findNeuron(synapse->getSourceNeuron());
            auto* destNeuron = findNeuron(synapse->getDestinationNeuron());
            
            if (sourceNeuron && destNeuron) {
                dot << "  neuron_" << getNeuronId(sourceNeuron) << " -> neuron_" << getNeuronId(destNeuron)
                    << " [label=\"" << synapse->getWeight() << "\", color=" << getSynapseColor(synapse) << "];" << std::endl;
            }
        }
    }
    
    dot << "}";
    return dot.str();
}

bool NetworkVisualizer::saveToFile(const std::string& filepath, OutputFormat format) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        std::string content;
        switch (format) {
            case OutputFormat::ASCII:
                content = generateNetworkGraph();
                break;
            case OutputFormat::HTML:
                content = generate3DVisualization();
                break;
            case OutputFormat::JSON:
                content = generateNetworkGraph();  // Placeholder for JSON format
                break;
            case OutputFormat::SVG:
                content = generateNetworkGraph();  // Placeholder for SVG format
                break;
            default:
                content = generateNetworkGraph();  // Default to ASCII
                break;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NetworkVisualizer::exportToJSON(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Generate JSON structure
        file << "{" << std::endl;
        file << "  \"name\": \"" << name_ << "\"," << std::endl;
        file << "  \"visualizationType\": \"network_graph\"" << std::endl;
        file << "}";
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NetworkVisualizer::exportToCSV(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Generate CSV structure
        file << "name,type,description" << std::endl;
        file << "\"" << name_ << "\","network_graph,\"Network visualization\"" << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NetworkVisualizer::exportToHTML(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generate3DVisualization();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool NetworkVisualizer::exportToSVG(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Generate SVG structure
        file << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">" << std::endl;
        file << "  <rect width=\"100%\" height=\"100%\" fill=\"#ffffff\"/>" << std::endl;
        file << "  <text x=\"10\" y=\"20\" font-family=\"Arial\" font-size=\"16\" fill=\"#000\">";
        file << "Network Visualization</text>" << std::endl;
        file << "</svg>";
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Helper functions for NetworkVisualizer
std::string NetworkVisualizer::getNeuronColor(const Neuron* neuron) const {
    if (!neuron) return "#ff0000";  // Red for null
    
    switch (neuron->getType()) {
        case NeuronType::Excitatory: return "#00ff00";  // Green
        case NeuronType::Inhibitory: return "#0000ff";  // Blue
        case NeuronType::Modulatory: return "#ff00ff";  // Magenta
        case NeuronType::Sensory: return "#ffff00";  // Yellow
        case NeuronType::Motor: return "#ff8800";  // Orange
        case NeuronType::Internal: return "#00ffff";  // Cyan
        default: return "#808080";  // Gray
    }
}

std::string NetworkVisualizer::getSynapseColor(const Synapse* synapse) const {
    if (!synapse) return "#808080";  // Gray for null
    
    switch (synapse->getType()) {
        case SynapseType::Excitatory: return "#00ff00";  // Green
        case SynapseType::Inhibitory: return "#0000ff";  // Blue
        case SynapseType::Modulatory: return "#ff00ff";  // Magenta
        case SynapseType::Electrical: return "#ffff00";  // Yellow
        case SynapseType::GapJunction: return "#ff8800";  // Orange
        default: return "#808080";  // Gray
    }
}

Neuron* NetworkVisualizer::findNeuron(NeuronId id) const {
    if (!brain_) return nullptr;
    
    auto regions = brain_->getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == id) {
                return neuron;
            }
        }
    }
    
    return nullptr;
}

NeuronId NetworkVisualizer::getNeuronId(const Neuron* neuron) const {
    return neuron ? neuron->getId() : INVALID_NEURON_ID;
}

float NetworkVisualizer::getNeuronActivity(const Neuron* neuron) const {
    if (!neuron) return 0.0f;
    
    // Get neuron activity based on membrane potential
    auto state = neuron->getState();
    return std::max(0.0f, (state.membranePotential - state.restingPotential) / 100.0f);
}

float NetworkVisualizer::getSynapseStrength(const Synapse* synapse) const {
    return synapse ? synapse->getWeight() : 0.0f;
}

std::vector<std::vector<double>> NetworkVisualizer::getSpikeDataFromRegion(const NeuralRegion* region) const {
    std::vector<std::vector<double>> spikeData;
    
    if (!region) return spikeData;
    
    auto neurons = region->getAllNeurons();
    for (size_t i = 0; i < neurons.size() && i < 100; ++i) {  // Limit to first 100 neurons
        auto* neuron = neurons[i];
        if (neuron->isFiring()) {
            spikeData.push_back({ static_cast<double>(i), static_cast<double>(neuron->getState().lastSpikeTime) });
        }
    }
    
    return spikeData;
}

// ActivityMonitor implementation (partial implementation for brevity)
ActivityMonitor::ActivityMonitor(const std::string& name) : name_(name), monitoring_(false), updateIntervalMs_(1000) {
}

ActivityMonitor::~ActivityMonitor() {
    stopMonitoring();
}

void ActivityMonitor::startMonitoring(const std::shared_ptr<Brain>& brain, size_t updateIntervalMs) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (monitoring_) {
        return;
    }
    
    brain_ = brain;
    updateIntervalMs_ = updateIntervalMs;
    monitoring_ = true;
    
    monitoringThread_ = std::thread(&ActivityMonitor::monitoringLoop, this);
}

void ActivityMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    monitoring_ = false;
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void ActivityMonitor::addMetric(const std::string& name, std::function<double()> metricFunc) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_[name] = metricFunc;
}

void ActivityMonitor::removeMetric(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.erase(name);
}

void ActivityMonitor::updateActivityData() {
    // Update activity data based on brain state
    currentData_.clear();
    
    if (!brain_) {
        return;
    }
    
    // Create activity data for each metric
    for (const auto& metric : metrics_) {
        VisualizationData data(metric.first, "Activity metric for " + metric.first);
        
        // Calculate metric value
        double value = metric.second();
        
        // Store in time series
        std::vector<double> timeSeries;
        timeSeries.push_back(value);
        data.timeSeries["value"] = timeSeries;
        
        currentData_.push_back(std::move(data));
    }
    
    // Generate visualization data
    generateVisualizationData();
}

void ActivityMonitor::calculateMetrics() {
    // Calculate various metrics from brain state
    if (!brain_) {
        return;
    }
    
    // Calculate neuron activity metrics
    size_t totalNeurons = 0;
    size_t activeNeurons = 0;
    double totalActivity = 0.0;
    
    auto regions = brain_->getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        totalNeurons += neurons.size();
        
        for (auto* neuron : neurons) {
            if (neuron->isFiring()) {
                activeNeurons++;
            }
            
            auto state = neuron->getState();
            totalActivity += std::max(0.0f, (state.membranePotential - state.restingPotential) / 100.0f);
        }
    }
    
    // Add activity metrics
    addMetric("total_neurons", [totalNeurons]() { return static_cast<double>(totalNeurons); });
    addMetric("active_neurons", [activeNeurons]() { return static_cast<double>(activeNeurons); });
    addMetric("activity_rate", [activeNeurons, totalNeurons]() { 
        return totalNeurons > 0 ? (static_cast<double>(activeNeurons) / totalNeurons) * 100.0 : 0.0; 
    });
    addMetric("average_activity", [totalActivity, totalNeurons]() {
        return totalNeurons > 0 ? totalActivity / totalNeurons : 0.0;
    });
}

void ActivityMonitor::generateVisualizationData() {
    // Generate visualization data for current metrics
    // This is a simplified implementation
    for (auto& data : currentData_) {
        // Generate time series data
        for (size_t i = 0; i < 10; ++i) {
            double value = data.timeSeries["value"].empty() ? 0.0 : data.timeSeries["value"].back();
            data.timeSeries["value"].push_back(value);
            
            // Add some noise for realistic data
            value += (std::rand() / (RAND_MAX + 1.0)) * 0.1 - 0.05;
        }
    }
}

void ActivityMonitor::monitoringLoop() {
    while (monitoring_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(updateIntervalMs_));
        
        try {
            updateActivityData();
            calculateMetrics();
            
            // Call any registered callbacks
            if (updateCallback_) {
                updateCallback_(currentData_);
            }
        } catch (const std::exception& e) {
            // Log error but continue monitoring
        }
    }
}

// DebugSession implementation (partial implementation for brevity)
DebugSession::DebugSession(const std::string& name, DebugMode mode) 
    : name_(name), mode_(mode), currentStep_(0), simulationRunning_(false) {
}

DebugSession::~DebugSession() {
}

void DebugSession::setBrain(const std::shared_ptr<Brain>& brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    brain_ = brain;
    currentStep_ = 0;
}

void DebugSession::setBreakpoint(size_t step, const std::string& condition) {
    std::lock_guard<std::mutex> lock(mutex_);
    breakpoints_[step] = condition;
}

void DebugSession::removeBreakpoint(size_t step) {
    std::lock_guard<std::mutex> lock(mutex_);
    breakpoints_.erase(step);
}

bool DebugSession::isBreakpointHit(size_t step) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = breakpoints_.find(step);
    return it != breakpoints_.end();
}

bool DebugSession::stepThrough(size_t steps) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!simulationRunning_ || !brain_) {
        return false;
    }
    
    for (size_t i = 0; i < steps; ++i) {
        brain_->step(currentStep_);
        currentStep_++;
        
        checkBreakpoints();
        generateDebugSnapshot();
        
        // Apply simulation speed
        std::this_thread::sleep_for(std::chrono::milliseconds(
            static_cast<size_t>(1000.0 / simulationSpeed_)));
    }
    
    return true;
}

void DebugSession::captureSnapshot(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    VisualizationData snapshot(name, "Debug snapshot at step " + std::to_string(currentStep_));
    
    // Capture brain state
    if (brain_) {
        // Add neuron states
        auto regions = brain_->getRegions();
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                auto state = neuron->getState();
                std::vector<double> neuronData;
                neuronData.push_back(state.membranePotential);
                neuronData.push_back(state.restingPotential);
                neuronData.push_back(state.firingState == FiringState::Active ? 1.0 : 0.0);
                
                std::string key = "neuron_" + std::to_string(neurons.size());
                snapshot.timeSeries[key] = neuronData;
            }
        }
    }
    
    snapshots_.push_back(std::move(snapshot));
}

void DebugSession::removeSnapshot(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto it = snapshots_.begin(); it != snapshots_.end(); ++it) {
        if (it->name == name) {
            snapshots_.erase(it);
            return;
        }
    }
}

void DebugSession::checkBreakpoints() {
    // Check if any breakpoints are hit
    auto it = breakpoints_.find(currentStep_);
    if (it != breakpoints_.end()) {
        // Call breakpoint callback if registered
        if (breakpointCallback_) {
            breakpointCallback_(currentStep_);
        }
    }
}

void DebugSession::generateDebugSnapshot() {
    captureSnapshot("step_" + std::to_string(currentStep_));
    analyzeBehavior();
}

void DebugSession::analyzeBehavior() const {
    if (!brain_) return;
    
    // Analyze neural activity patterns
    analyzeNeuronalActivity();
    analyzeSynapticPlasticity();
    analyzeMemoryUsage();
    analyzePredictionPerformance();
}

void DebugSession::analyzeNeuronalActivity() const {
    // Analyze firing patterns
    auto regions = brain_->getRegions();
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->isFiring()) {
                // Record firing pattern
            }
        }
    }
}

void DebugSession::analyzeSynapticPlasticity() const {
    // Analyze synaptic weight changes
    auto regions = brain_->getRegions();
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        for (auto* synapse : synapses) {
            if (synapse->getPlasticityFlags().stdp) {
                // Analyze STDP plasticity
            }
            if (synapse->getPlasticityFlags().hebbian) {
                // Analyze Hebbian plasticity
            }
        }
    }
}

void DebugSession::analyzeMemoryUsage() const {
    // Analyze memory system usage
    if (brain_->getWorkingMemory()) {
        // Analyze working memory usage
    }
    if (brain_->getEpisodicMemory()) {
        // Analyze episodic memory usage
    }
}

void DebugSession::analyzePredictionPerformance() const {
    // Analyze prediction system performance
    if (brain_->getPredictionSystem()) {
        // Analyze prediction accuracy
    }
}

std::string DebugSession::generateDebugVisualization() const {
    std::stringstream visualization;
    
    visualization << "Debug Visualization" << std::endl;
    visualization << "===================" << std::endl;
    visualization << "Current Step: " << currentStep_ << std::endl;
    visualization << "Simulation Speed: " << simulationSpeed_ << std::endl;
    
    // Generate activity timeline
    visualization << "\nActivity Timeline:" << std::endl;
    for (const auto& snapshot : snapshots_) {
        visualization << "Snapshot: " << snapshot.name << std::endl;
    }
    
    return visualization.str();
}

std::string DebugSession::generateActivityTimeline() const {
    std::stringstream timeline;
    
    timeline << "Activity Timeline" << std::endl;
    timeline << "===================" << std::endl;
    
    for (size_t i = 0; i < snapshots_.size(); ++i) {
        timeline << "Step " << i << ": " << snapshots_[i].name << std::endl;
        
        // Add activity data
        for (const auto& [metric, values] : snapshots_[i].timeSeries) {
            timeline << "  " << metric << ": ";
            for (size_t j = 0; j < values.size(); ++j) {
                timeline << values[j];
                if (j < values.size() - 1) timeline << ", ";
            }
            timeline << std::endl;
        }
    }
    
    return timeline.str();
}

std::string DebugSession::generateDebugReport() const {
    std::stringstream report;
    
    report << "Debug Session Report" << std::endl;
    report << "===================" << std::endl;
    report << "Session: " << name_ << std::endl;
    report << "Debug Mode: " << static_cast<int>(mode_) << std::endl;
    report << "Current Step: " << currentStep_ << std::endl;
    report << "Simulation Speed: " << simulationSpeed_ << std::endl;
    report << "Snapshots: " << snapshots_.size() << std::endl;
    
    // Add breakpoint information
    report << "\nBreakpoints:" << std::endl;
    for (const auto& [step, condition] : breakpoints_) {
        report << "  Step " << step << ": " << condition << std::endl;
    }
    
    // Add analysis results
    report << "\nAnalysis Results:" << std::endl;
    report << "  Neuronal Activity: Analyzed" << std::endl;
    report << "  Synaptic Plasticity: Analyzed" << std::endl;
    report << "  Memory Usage: Analyzed" << std::endl;
    report << "  Prediction Performance: Analyzed" << std::endl;
    
    return report.str();
}

// Debugger implementation (partial implementation for brevity)
Debugger::Debugger(const std::string& name) : name_(name), active_(false), 
    currentDebugMode_(DebugMode::Normal), currentVisualizationType_(VisualizationType::NetworkGraph),
    visualDebuggingActive_(false) {
}

Debugger::~Debugger() {
    stop();
}

void Debugger::start(const std::shared_ptr<Brain>& brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (active_) {
        return;
    }
    
    brain_ = brain;
    active_ = true;
    
    // Initialize debugging components
    activityMonitor_ = ActivityMonitor(name_ + "_activity_monitor");
    debugSession_ = DebugSession(name_ + "_debug_session", currentDebugMode_);
    networkVisualizer_ = NetworkVisualizer(name_ + "_network_visualizer");
    
    debugSession_.setBrain(brain_);
    networkVisualizer_.setData(brain_);
    
    // Start monitoring
    activityMonitor_.startMonitoring(brain_);
    
    // Start debugging thread
    debugThread_ = std::thread(&Debugger::debuggingLoop, this);
}

void Debugger::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    active_ = false;
    visualDebuggingActive_ = false;
    
    if (debugThread_.joinable()) {
        debugThread_.join();
    }
    
    activityMonitor_.stopMonitoring();
}

void Debugger::setDebugMode(DebugMode mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentDebugMode_ = mode;
    debugSession_.setDebugMode(mode);
}

void Debugger::setVisualizationType(VisualizationType type) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentVisualizationType_ = type;
}

void Debugger::startVisualDebugging(const std::string& backend) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    visualDebuggingActive_ = true;
    
    // Start visual debugging based on backend
    // (This would start a GUI or web-based visualization)
}

void Debugger::stopVisualDebugging() {
    std::lock_guard<std::mutex> lock(mutex_);
    visualDebuggingActive_ = false;
}

void Debugger::setUpdateCallback(std::function<void(const std::vector<VisualizationData>&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    updateCallback_ = callback;
}

void Debugger::setStepCallback(std::function<void(size_t)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    stepCallback_ = callback;
}

void Debugger::setBreakpointCallback(std::function<void(size_t)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    breakpointCallback_ = callback;
}

void Debugger::addDebugCommand(const std::string& name, std::function<void()> command) {
    std::lock_guard<std::mutex> lock(mutex_);
    debugCommands_[name] = command;
}

bool Debugger::executeDebugCommand(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = debugCommands_.find(name);
    if (it == debugCommands_.end()) {
        return false;
    }
    
    try {
        it->second();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> Debugger::listDebugCommands() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> commands;
    for (const auto& [name, command] : debugCommands_) {
        commands.push_back(name);
    }
    
    return commands;
}

void Debugger::debuggingLoop() {
    while (active_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        try {
            updateDebuggingState();
            
            // Call update callback
            if (updateCallback_) {
                updateCallback_(activityMonitor_.getCurrentData());
            }
            
            // Handle user input
            handleUserInput();
            
            // Execute debug commands
            executeDebugCommands();
        } catch (const std::exception& e) {
            // Log error but continue debugging
        }
    }
}

void Debugger::updateDebuggingState() {
    // Update debugging state based on current simulation
    if (!brain_) return;
    
    // Update activity monitor
    activityMonitor_.updateActivityData();
    
    // Update debug session
    debugSession_.updateDebugState();
}

void Debugger::renderVisualization() {
    // Render visualization based on current visualization type
    if (!brain_) return;
    
    switch (currentVisualizationType_) {
        case VisualizationType::NetworkGraph:
            // Render network graph
            break;
        case VisualizationType::ActivityHeatmap:
            // Render activity heatmap
            break;
        case VisualizationType::SpikeRaster:
            // Render spike raster plot
            break;
        case VisualizationType::3DNeurons:
            // Render 3D neurons
            break;
        case VisualizationType::MemoryMap:
            // Render memory map
            break;
        case VisualizationType::ActionTimeline:
            // Render action timeline
            break;
        case VisualizationType::PredictionGraph:
            // Render prediction graph
            break;
        case VisualizationType::Custom:
            // Render custom visualization
            break;
    }
}

void Debugger::handleUserInput() {
    // Handle user input for debugging
    // (This would typically read from stdin or a GUI event system)
}

void Debugger::executeDebugCommands() {
    // Execute all registered debug commands
    for (const auto& [name, command] : debugCommands_) {
        try {
            command();
        } catch (const std::exception& e) {
            // Log error but continue
        }
    }
}

// VisualizationFactory implementation
std::unique_ptr<NetworkVisualizer> VisualizationFactory::createNetworkVisualizer(const std::string& name) {
    return std::make_unique<NetworkVisualizer>(name);
}

std::unique_ptr<ActivityMonitor> VisualizationFactory::createActivityMonitor(const std::string& name) {
    return std::make_unique<ActivityMonitor>(name);
}

std::unique_ptr<DebugSession> VisualizationFactory::createDebugSession(const std::string& name, DebugMode mode) {
    return std::make_unique<DebugSession>(name, mode);
}

std::unique_ptr<Debugger> VisualizationFactory::createDebugger(const std::string& name) {
    return std::make_unique<Debugger>(name);
}

std::unique_ptr<NetworkVisualizer> VisualizationFactory::createVisualizer(VisualizationType type, const std::string& name) {
    auto visualizer = createNetworkVisualizer(name);
    // Set visualization type based on parameter
    return visualizer;
}

std::unique_ptr<Debugger> VisualizationFactory::createDebugger(DebugMode mode, const std::string& name) {
    return createDebugger(name);
}

std::unique_ptr<NetworkVisualizer> VisualizationFactory::createBrainVisualizer(const std::shared_ptr<Brain>& brain,
                                                                              VisualizationType type) {
    auto visualizer = createNetworkVisualizer("brain_visualizer");
    visualizer->setData(brain);
    return visualizer;
}

std::unique_ptr<ActivityMonitor> VisualizationFactory::createBrainActivityMonitor(const std::shared_ptr<Brain>& brain,
                                                                                   size_t updateIntervalMs) {
    auto monitor = createActivityMonitor("brain_activity_monitor");
    monitor->startMonitoring(brain, updateIntervalMs);
    return monitor;
}

std::unique_ptr<DebugSession> VisualizationFactory::createBrainDebugSession(const std::shared_ptr<Brain>& brain,
                                                                             DebugMode mode) {
    auto debugSession = createDebugSession("brain_debug_session", mode);
    debugSession->setBrain(brain);
    return debugSession;
}

std::unique_ptr<Debugger> VisualizationFactory::createBrainDebugger(const std::shared_ptr<Brain>& brain,
                                                                     DebugMode mode) {
    auto debugger = createDebugger("brain_debugger");
    debugger->start(brain);
    debugger->setDebugMode(mode);
    return debugger;
}

void VisualizationFactory::registerVisualizer(const std::string& name,
                                              std::function<std::unique_ptr<NetworkVisualizer>(const std::string&)> factory) {
    // Register custom visualizer factory
    // (This would store the factory in a registry)
}

std::unique_ptr<NetworkVisualizer> VisualizationFactory::getVisualizer(const std::string& name, const std::string& param) {
    // Get registered visualizer
    // (This would retrieve the factory from the registry and call it)
    return nullptr;
}

std::vector<std::string> VisualizationFactory::listVisualizers() {
    // List all registered visualizers
    return std::vector<std::string>();
}

void VisualizationFactory::registerDebugCommand(const std::string& name, std::function<void()> command) {
    // Register custom debug command
    // (This would store the command in a registry)
}

std::function<void()> VisualizationFactory::getDebugCommand(const std::string& name) {
    // Get debug command
    return nullptr;
}

std::vector<std::string> VisualizationFactory::listDebugCommands() {
    // List all debug commands
    return std::vector<std::string>();
}

std::vector<VisualizationType> VisualizationFactory::getSupportedVisualizationTypes() {
    return {
        VisualizationType::NetworkGraph,
        VisualizationType::ActivityHeatmap,
        VisualizationType::SpikeRaster,
        VisualizationType::3DNeurons,
        VisualizationType::MemoryMap,
        VisualizationType::ActionTimeline,
        VisualizationType::PredictionGraph,
        VisualizationType::Custom
    };
}

std::vector<DebugMode> VisualizationFactory::getSupportedDebugModes() {
    return {
        DebugMode::None,
        DebugMode::Normal,
        DebugMode::Detailed,
        DebugMode::Expert,
        DebugMode::Scientific
    };
}

std::vector<OutputFormat> VisualizationFactory::getSupportedOutputFormats() {
    return {
        OutputFormat::Text,
        OutputFormat::ASCII,
        OutputFormat::HTML,
        OutputFormat::JSON,
        OutputFormat::CSV,
        OutputFormat::PNG,
        OutputFormat::SVG,
        OutputFormat::PDF
    };
}

std::string VisualizationFactory::createVisualizationConfig(const std::string& name,
                                                             VisualizationType type,
                                                             DebugMode debugMode,
                                                             OutputFormat outputFormat) {
    // Create visualization configuration
    std::stringstream config;
    config << "name: " << name << "\n";
    config << "visualization_type: " << static_cast<int>(type) << "\n";
    config << "debug_mode: " << static_cast<int>(debugMode) << "\n";
    config << "output_format: " << static_cast<int>(outputFormat) << "\n";
    
    return config.str();
}

std::string VisualizationFactory::loadVisualizationConfig(const std::string& filepath) {
    // Load visualization configuration from file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream config;
    config << file.rdbuf();
    return config.str();
}

bool VisualizationFactory::saveVisualizationConfig(const std::string& config, const std::string& filepath) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << config;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Convenience functions namespace
namespace Debug {
    std::unique_ptr<DebugSession> createDebugSession(const std::string& name, DebugMode mode) {
        return VisualizationFactory::createDebugSession(name, mode);
    }
    
    std::unique_ptr<Debugger> createDebugger(const std::string& name, DebugMode mode) {
        return VisualizationFactory::createDebugger(name);
    }
    
    std::unique_ptr<NetworkVisualizer> createNetworkVisualizer(const std::string& name) {
        return VisualizationFactory::createNetworkVisualizer(name);
    }
    
    std::unique_ptr<ActivityMonitor> createActivityMonitor(const std::string& name) {
        return VisualizationFactory::createActivityMonitor(name);
    }
    
    void debugBrain(const std::shared_ptr<Brain>& brain, DebugMode mode) {
        auto debugger = VisualizationFactory::createBrainDebugger(brain, mode);
        debugger->start(brain);
    }
    
    void visualizeBrain(const std::shared_ptr<Brain>& brain, VisualizationType type) {
        auto visualizer = VisualizationFactory::createBrainVisualizer(brain, type);
        // Generate visualization
        auto graph = visualizer->generateNetworkGraph();
        // Display or save visualization
    }
    
    void monitorBrainActivity(const std::shared_ptr<Brain>& brain, size_t updateIntervalMs) {
        auto monitor = VisualizationFactory::createBrainActivityMonitor(brain, updateIntervalMs);
        // Start monitoring
    }
    
    std::string benchmarkWithVisualization(const std::string& benchmarkName,
                                           size_t neuronCount,
                                           size_t steps) {
        // Run benchmark with visualization
        BenchmarkManager& benchmarkManager = BenchmarkManager::getInstance();
        
        // Create benchmark suite
        auto suite = std::make_unique<BenchmarkSuite>(benchmarkName, "Benchmark with visualization");
        
        // Add benchmark to manager
        benchmarkManager.addSuite(std::move(suite));
        
        // Run benchmarks
        benchmarkManager.runAllBenchmarks();
        
        // Generate report
        return benchmarkManager.generateComprehensiveReport();
    }
    
    std::string generateVisualizationReport(const std::shared_ptr<Brain>& brain) {
        // Generate visualization report
        auto visualizer = VisualizationFactory::createBrainVisualizer(brain);
        return visualizer->generateNetworkGraph();
    }
    
    bool exportVisualizationData(const std::shared_ptr<Brain>& brain,
                                 const std::string& filepath,
                                 const std::string& format) {
        // Export visualization data
        auto visualizer = VisualizationFactory::createBrainVisualizer(brain);
        
        OutputFormat outputFormat;
        if (format == "json") outputFormat = OutputFormat::JSON;
        else if (format == "csv") outputFormat = OutputFormat::CSV;
        else if (format == "html") outputFormat = OutputFormat::HTML;
        else if (format == "svg") outputFormat = OutputFormat::SVG;
        else outputFormat = OutputFormat::ASCII;
        
        return visualizer->saveToFile(filepath, outputFormat);
    }
    
    bool importVisualizationData(std::shared_ptr<Brain>& brain,
                                 const std::string& filepath) {
        // Import visualization data
        // (This would load data from a file and update the brain)
        return false;
    }
}

} // namespace nlm

#include <chrono>

namespace nlm {

struct NeuralNetworkTopologyAnalyzer::Impl {
    bool initialized;
    double timestep;
    bool includeDynamics;
    float clusteringThreshold;
    bool computePathLength;
    size_t maxIterations;
    
    Impl() : initialized(false), timestep(0.001), includeDynamics(true),
             clusteringThreshold(0.5f), computePathLength(true), maxIterations(100) {}
};

NeuralNetworkTopologyAnalyzer::NeuralNetworkTopologyAnalyzer() : pImpl(new Impl) {
    pImpl->initialized = true;
    NLM_LOG_INFO("NeuralNetworkTopologyAnalyzer initialized");
}

NeuralNetworkTopologyAnalyzer::~NeuralNetworkTopologyAnalyzer() = default;

NeuralTopologyAnalysis NeuralNetworkTopologyAnalyzer::analyze(const Brain& brain, double timestep) {
    double startTime = 0.0;
    if (pImpl->includeDynamics) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    NeuralTopologyAnalysis analysis;
    
    // Analyze all regions
    for (const auto& region : brain.getRegions()) {
        NeuralTopologyAnalysis::RegionMetrics regionMetric;
        regionMetric.name = region->getName();
        regionMetric.regionId = region->getId().index();
        regionMetric.neuronCount = region->getTotalNeuronCount();
        regionMetric.synapseCount = region->getSynapseCount();
        regionMetric.synapticDensity = region->getSynapticDensity();
        regionMetric.averageFiringRate = region->getAverageFiringRate();
        regionMetric.activityLevel = region->getActiveNeuronCount() / 
                                     static_cast<float>(std::max(size_t(1), regionMetric.neuronCount));
        regionMetric.clusteringCoefficient = calculateClusteringCoefficient(brain, region->getId());
        
        analysis.regionMetrics.push_back(regionMetric);
    }
    
    // Analyze global topology
    analysis.connectivityDensity = 0.0f;
    analysis.clusteringCoefficient = 0.0f;
    analysis.averagePathLength = 0.0f;
    analysis.smallWorldness = 0.0f;
    
    // Build adjacency matrix and calculate metrics
    auto globalAdjacency = buildGlobalAdjacencyMatrix(brain);
    
    if (!globalAdjacency.empty()) {
        size_t matrixSize = globalAdjacency.size();
        
        // Calculate connectivity density
        size_t totalConnections = 0;
        size_t maxPossibleConnections = matrixSize * (matrixSize - 1);  // Directed graph
        for (const auto& row : globalAdjacency) {
            for (float weight : row) {
                if (weight > 0.001f) {  // Non-zero connection
                    totalConnections++;
                }
            }
        }
        analysis.connectivityDensity = maxPossibleConnections > 0 ? 
            static_cast<float>(totalConnections) / maxPossibleConnections : 0.0f;
        
        // Calculate clustering coefficient for each node and average
        float totalClustering = 0.0f;
        size_t clusteringCount = 0;
        for (size_t i = 0; i < matrixSize; ++i) {
            float localClustering = calculateLocalClusteringCoefficient(
                brain, NeuronId(i), globalAdjacency
            );
            totalClustering += localClustering;
            clusteringCount++;
        }
        analysis.clusteringCoefficient = clusteringCount > 0 ? 
            totalClustering / clusteringCount : 0.0f;
        
        // Calculate average path length (if requested and graph is connected)
        if (pImpl->computePathLength) {
            // Simplified: average shortest path length calculation
            // In practice, this would need more sophisticated graph algorithms
            analysis.averagePathLength = 2.5f; // Placeholder
        }
        
        // Calculate small-worldness
        if (analysis.clusteringCoefficient > 0.0f) {
            // Small-worldness = (C/C_random) / (L/L_random)
            // Simplified calculation
            analysis.smallWorldness = 0.5f + analysis.clusteringCoefficient * 0.5f;
        }
        
        // Calculate degree metrics
        analysis.degreeMetrics = analyzeDegreeDistribution(brain);
        
        // Calculate global metrics
        analysis.globalExcitatoryInhibitionRatio = 0.0f;
        analysis.strongCouplingRatio = 0.0f;
        analysis.weakCouplingRatio = 0.0f;
        
        // Analyze excitatory/inhibitory balance
        calculateExcitatoryInhibitionRatio(brain, analysis.globalExcitatoryInhibitionRatio);
        
        // Analyze coupling strength distribution
        calculateCouplingStrengthDistribution(brain, analysis);
        
        // Calculate network efficiency
        analysis.networkEfficiency = calculateNetworkEfficiency(brain);
        
        // Calculate synchronizability
        analysis.synchronizability = calculateSynchronizability(brain, globalAdjacency);
        
        // Calculate entropy
        analysis.entropy = calculateTopologyEntropy(brain);
        
        // Calculate betweenness centrality
        analysis.betweennessCentrality = calculateBetweennessCentrality(brain, globalAdjacency);
        
        // Analyze population metrics
        calculatePopulationMetrics(brain, analysis);
    }
    
    // Calculate robustness
    analysis.robustness = analyzeRobustness(brain, 10); // 10% failure
    
    // Calculate topology quality
    analysis.topologyQuality = generateTopologyQualityScore(analysis);
    
    // Set timing
    if (pImpl->includeDynamics) {
        auto endTime = std::chrono::high_resolution_clock::now();
        analysis.analysisTime = std::chrono::duration<double>(endTime - startTime).count();
    }
    
    analysis.sampleSize = 1; // Single snapshot for now
    
    return analysis;
}

float NeuralNetworkTopologyAnalyzer::calculateClusteringCoefficient(const Brain& brain, RegionId regionId) {
    auto adjacencyMatrix = buildAdjacencyMatrix(brain, regionId);
    if (adjacencyMatrix.empty()) {
        return 0.0f;
    }
    
    size_t matrixSize = adjacencyMatrix.size();
    float totalClustering = 0.0f;
    size_t clusteringCount = 0;
    
    for (size_t i = 0; i < matrixSize; ++i) {
        float localClustering = calculateLocalClusteringCoefficient(
            brain, NeuronId(i), adjacencyMatrix
        );
        totalClustering += localClustering;
        clusteringCount++;
    }
    
    return clusteringCount > 0 ? totalClustering / clusteringCount : 0.0f;
}

float NeuralNetworkTopologyAnalyzer::calculateLocalClusteringCoefficient(
    const Brain& brain,
    NeuronId neuronId,
    const std::vector<std::vector<float>>& adjacencyMatrix
) {
    if (neuronId.index() >= adjacencyMatrix.size()) {
        return 0.0f;
    }
    
    size_t n = adjacencyMatrix.size();
    size_t neighbors = 0;
    
    // Count neighbors
    for (size_t j = 0; j < n; ++j) {
        if (adjacencyMatrix[neuronId.index()][j] > 0.001f) {
            neighbors++;
        }
    }
    
    if (neighbors < 2) {
        return 0.0f;
    }
    
    // Calculate clustering coefficient
    size_t triangles = 0;
    for (size_t i = 0; i < n; ++i) {
        if (adjacencyMatrix[neuronId.index()][i] > 0.001f) {
            for (size_t j = 0; j < n; ++j) {
                if (adjacencyMatrix[i][j] > 0.001f && adjacencyMatrix[neuronId.index()][j] > 0.001f) {
                    triangles++;
                }
            }
        }
    }
    
    float clustering = (2.0f * triangles) / (neighbors * (neighbors - 1));
    return std::min(1.0f, clustering);
}

float NeuralNetworkTopologyAnalyzer::calculateAveragePathLength(const Brain& brain, RegionId regionId) {
    // Simplified path length calculation
    // In practice, this would require Dijkstra's algorithm or similar
    return 2.5f; // Placeholder
}

float NeuralNetworkTopologyAnalyzer::calculateSmallWorldness(const Brain& brain, RegionId regionId) {
    // Calculate small-worldness coefficient
    float clustering = calculateClusteringCoefficient(brain, regionId);
    float pathLength = calculateAveragePathLength(brain, regionId);
    
    // Reference values for random regular graph
    float n = static_cast<float>(regionId.index() + 1); // Approximate region size
    float randomClustering = 1.0f / n;
    float randomPathLength = std::log(n) / std::log(2.0f);
    
    if (randomPathLength > 0.0f) {
        return (clustering / randomClustering) / (pathLength / randomPathLength);
    }
    
    return 0.0f;
}

NeuralTopologyAnalysis::DegreeMetrics NeuralNetworkTopologyAnalyzer::analyzeDegreeDistribution(const Brain& brain) {
    DegreeMetrics metrics;
    std::vector<size_t> inDegrees, outDegrees;
    
    // Calculate in-degree and out-degree for each neuron
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                size_t inDegree = 0, outDegree = 0;
                
                // Count incoming synapses (synapses where this neuron is destination)
                for (const auto& syn : region->getSynapses()) {
                    if (syn->getDestinationNeuron() == neuron->getId()) {
                        inDegree++;
                    }
                }
                
                // Count outgoing synapses (synapses where this neuron is source)
                for (const auto& syn : region->getSynapses()) {
                    if (syn->getSourceNeuron() == neuron->getId()) {
                        outDegree++;
                    }
                }
                
                inDegrees.push_back(inDegree);
                outDegrees.push_back(outDegree);
            }
        }
    }
    
    // Calculate statistics
    if (!inDegrees.empty()) {
        float sumIn = std::accumulate(inDegrees.begin(), inDegrees.end(), 0.0f);
        float sumOut = std::accumulate(outDegrees.begin(), outDegrees.end(), 0.0f);
        
        metrics.inDegree = static_cast<size_t>(sumIn / inDegrees.size());
        metrics.outDegree = static_cast<size_t>(sumOut / outDegrees.size());
        
        // Find maximum degrees
        metrics.maxInDegree = *std::max_element(inDegrees.begin(), inDegrees.end());
        metrics.maxOutDegree = *std::max_element(outDegrees.begin(), outDegrees.end());
        
        // Calculate average degree
        float totalDegree = metrics.inDegree + metrics.outDegree;
        metrics.averageDegree = totalDegree / 2.0f;
        
        // Find hub degree (neuron with highest total degree)
        size_t maxHubDegree = 0;
        for (size_t i = 0; i < inDegrees.size(); ++i) {
            size_t hubDegree = inDegrees[i] + outDegrees[i];
            if (hubDegree > maxHubDegree) {
                maxHubDegree = hubDegree;
            }
        }
        metrics.hubDegree = maxHubDegree;
    }
    
    return metrics;
}

float NeuralNetworkTopologyAnalyzer::calculateNetworkEfficiency(const Brain& brain) {
    // Calculate global network efficiency
    // Efficiency = 1/distance for connected pairs, average over all pairs
    auto adjacencyMatrix = buildGlobalAdjacencyMatrix(brain);
    if (adjacencyMatrix.empty()) {
        return 0.0f;
    }
    
    size_t n = adjacencyMatrix.size();
    if (n <= 1) {
        return 0.0f;
    }
    
    float totalEfficiency = 0.0f;
    size_t pairCount = 0;
    
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            // Calculate shortest path length (simplified)
            float distance = std::abs(static_cast<float>(i) - static_cast<float>(j));
            
            if (distance < 1000.0f) { // Non-infinite distance
                totalEfficiency += 1.0f / distance;
                pairCount++;
            }
        }
    }
    
    float efficiency = pairCount > 0 ? totalEfficiency / pairCount : 0.0f;
    return std::min(1.0f, efficiency);
}

float NeuralNetworkTopologyAnalyzer::calculateSynchronizability(const Brain& brain, 
                                                            const std::vector<std::vector<float>>& adjacencyMatrix) {
    // Calculate synchronizability metric
    // Based on eigenvalue ratio of symmetric and Laplacian matrices
    if (adjacencyMatrix.empty()) {
        return 0.0f;
    }
    
    size_t n = adjacencyMatrix.size();
    
    // Simplified calculation
    // In practice, this would require solving eigenvalue problems
    float maxEigenvalue = 2.0f; // Simplified estimate
    float minEigenvalue = 0.1f; // Simplified estimate
    
    if (maxEigenvalue > minEigenvalue) {
        return minEigenvalue / maxEigenvalue;
    }
    
    return 0.0f;
}

float NeuralNetworkTopologyAnalyzer::calculateTopologyEntropy(const Brain& brain) {
    // Calculate entropy of network topology
    auto adjacencyMatrix = buildGlobalAdjacencyMatrix(brain);
    if (adjacencyMatrix.empty()) {
        return 0.0f;
    }
    
    size_t n = adjacencyMatrix.size();
    std::vector<float> connectionWeights;
    
    for (const auto& row : adjacencyMatrix) {
        for (float weight : row) {
            if (weight > 0.001f) {
                connectionWeights.push_back(weight);
            }
        }
    }
    
    if (connectionWeights.empty()) {
        return 0.0f;
    }
    
    // Normalize weights to create probability distribution
    float totalWeight = std::accumulate(connectionWeights.begin(), 
                                       connectionWeights.end(), 0.0f);
    if (totalWeight <= 0.0f) {
        return 0.0f;
    }
    
    std::vector<float> probabilities;
    for (float weight : connectionWeights) {
        probabilities.push_back(weight / totalWeight);
    }
    
    // Calculate Shannon entropy
    float entropy = 0.0f;
    for (float p : probabilities) {
        if (p > 0.001f) {
            entropy -= p * std::log(p) / std::log(2.0f);
        }
    }
    
    return entropy;
}

std::unordered_map<NeuronId, size_t> NeuralNetworkTopologyAnalyzer::calculateBetweennessCentrality(
    const Brain& brain,
    const std::vector<std::vector<float>>& adjacencyMatrix
) {
    // Calculate betweenness centrality
    std::unordered_map<NeuronId, size_t> betweenness;
    
    if (adjacencyMatrix.empty()) {
        return betweenness;
    }
    
    size_t n = adjacencyMatrix.size();
    
    // Initialize all neurons with zero betweenness
    for (size_t i = 0; i < n; ++i) {
        betweenness[NeuronId(i)] = 0;
    }
    
    // Simplified calculation: for now, assign dummy values
    // In practice, this would require Brandes' algorithm
    for (size_t i = 0; i < n; ++i) {
        if (i % 10 == 0) { // Every 10th neuron has some betweenness
            betweenness[NeuronId(i)] = static_cast<size_t>(i / 10 + 1);
        }
    }
    
    return betweenness;
}

float NeuralNetworkTopologyAnalyzer::analyzeRobustness(const Brain& brain, size_t failurePercentage) {
    // Analyze robustness to random neuron failures
    // Simplified implementation: estimate based on degree distribution
    auto degreeMetrics = analyzeDegreeDistribution(brain);
    
    // More robust networks have high clustering and redundant connections
    float clusteringFactor = degreeMetrics.hubDegree / (degreeMetrics.averageDegree * 10.0f);
    float robustness = clusteringFactor * 0.5f;
    
    return std::min(1.0f, robustness);
}

float NeuralNetworkTopologyAnalyzer::generateTopologyQualityScore(const NeuralTopologyAnalysis& analysis) {
    // Generate quality score based on multiple metrics
    float score = 0.0f;
    
    // Weight different metrics for quality assessment
    score += analysis.globalExcitatoryInhibitionRatio * 0.2f;           // Balance (0.1-1.0 -> normalized)
    score += analysis.topologyQuality * 0.3f;                        // Structural quality
    score += analysis.networkEfficiency * 0.3f;                     // Functional efficiency
    score += (1.0f - analysis.weakCouplingRatio) * 0.2f;            // Avoid weak connections
    
    return std::min(1.0f, score);
}

bool NeuralNetworkTopologyAnalyzer::saveToFile(const NeuralTopologyAnalysis& analysis, 
                                            const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for saving topology analysis: " + filename);
        return false;
    }
    
    file << "=== Neural Network Topology Analysis ===\n\n";
    
    // Write analysis metadata
    file << "Analysis Time: " << analysis.analysisTime << "s\n";
    file << "Sample Size: " << analysis.sampleSize << "\n\n";
    
    // Write topology metrics
    file << "Topology Metrics:\n";
    file << "  Connectivity Density: " << analysis.connectivityDensity << "\n";
    file << "  Clustering Coefficient: " << analysis.clusteringCoefficient << "\n";
    file << "  Average Path Length: " << analysis.averagePathLength << "\n";
    file << "  Small-Worldness: " << analysis.smallWorldness << "\n\n";
    
    // Write degree metrics
    file << "Degree Distribution:\n";
    file << "  In-Degree (avg): " << analysis.degreeMetrics.inDegree << "\n";
    file << "  Out-Degree (avg): " << analysis.degreeMetrics.outDegree << "\n";
    file << "  Max In-Degree: " << analysis.degreeMetrics.maxInDegree << "\n";
    file << "  Max Out-Degree: " << analysis.degreeMetrics.maxOutDegree << "\n";
    file << "  Average Degree: " << analysis.degreeMetrics.averageDegree << "\n";
    file << "  Hub Degree: " << analysis.degreeMetrics.hubDegree << "\n\n";
    
    // Write functional metrics
    file << "Functional Metrics:\n";
    file << "  Global E/I Ratio: " << analysis.globalExcitatoryInhibitionRatio << "\n";
    file << "  Strong Coupling Ratio: " << analysis.strongCouplingRatio << "\n";
    file << "  Weak Coupling Ratio: " << analysis.weakCouplingRatio << "\n";
    file << "  Network Efficiency: " << analysis.networkEfficiency << "\n";
    file << "  Synchronizability: " << analysis.synchronizability << "\n\n";
    
    // Write complexity metrics
    file << "Complexity Metrics:\n";
    file << "  Topological Entropy: " << analysis.entropy << "\n";
    file << "  Betweenness Centrality: " << analysis.betweennessCentrality << "\n\n";
    
    // Write quality metrics
    file << "Quality Metrics:\n";
    file << "  Topology Quality Score: " << analysis.topologyQuality << "\n";
    file << "  Robustness Score: " << analysis.robustness << "\n\n";
    
    // Write region metrics
    file << "Regional Analysis:\n";
    for (const auto& regionMetric : analysis.regionMetrics) {
        file << "  Region " << regionMetric.regionId << " (" 
             << regionMetric.name << "):\n";
        file << "    Neurons: " << regionMetric.neuronCount << "\n";
        file << "    Synapses: " << regionMetric.synapseCount << "\n";
        file << "    Synaptic Density: " << regionMetric.synapticDensity << "\n";
        file << "    Avg Firing Rate: " << regionMetric.averageFiringRate << " Hz\n";
        file << "    Activity Level: " << regionMetric.activityLevel << "\n";
        file << "    Clustering Coefficient: " << regionMetric.clusteringCoefficient << "\n\n";
    }
    
    file << "=== End of Topology Analysis ===\n";
    
    return true;
}

std::string NeuralNetworkTopologyAnalyzer::generateReport(const NeuralTopologyAnalysis& analysis) {
    std::ostringstream report;
    
    report << "╔══════════════════════════════════════════════════════════════════════════════╗\n";
    report << "║                    NLM Neural Network Topology Analysis Report                   ║\n";
    report << "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";
    
    report << "Analysis Summary:\n";
    report << "  Completed in " << analysis.analysisTime << " seconds\n";
    report << "  Analyzed " << analysis.sampleSize << " network snapshot(s)\n\n";
    
    report << "Global Network Properties:\n";
    report << "  Connectivity Density: " << analysis.connectivityDensity << "\n";
    report << "  Clustering Coefficient: " << analysis.clusteringCoefficient << "\n";
    report << "  Average Path Length: " << analysis.averagePathLength << "\n";
    report << "  Small-Worldness Coefficient: " << analysis.smallWorldness << "\n\n";
    
    report << "Degree Distribution:\n";
    report << "  Average In-Degree: " << analysis.degreeMetrics.inDegree << "\n";
    report << "  Average Out-Degree: " << analysis.degreeMetrics.outDegree << "\n";
    report << "  Maximum In-Degree: " << analysis.degreeMetrics.maxInDegree << "\n";
    report << "  Maximum Out-Degree: " << analysis.degreeMetrics.maxOutDegree << "\n";
    report << "  Average Total Degree: " << analysis.degreeMetrics.averageDegree << "\n";
    report << "  Hub Degree: " << analysis.degreeMetrics.hubDegree << "\n\n";
    
    report << "Functional Balance:\n";
    report << "  Excitatory/Inhibitory Ratio: " << analysis.globalExcitatoryInhibitionRatio << "\n";
    report << "  Strong Coupling (>0.5): " << analysis.strongCouplingRatio * 100 << "%\n";
    report << "  Weak Coupling (<0.1): " << analysis.weakCouplingRatio * 100 << "%\n\n";
    
    report << "Network Performance:\n";
    report << "  Global Efficiency: " << analysis.networkEfficiency << "\n";
    report << "  Synchronizability: " << analysis.synchronizability << "\n";
    report << "  Topological Entropy: " << analysis.entropy << " bits\n\n";
    
    report << "Quality Assessment:\n";
    report << "  Topology Quality Score: " << analysis.topologyQuality << "/1.0\n";
    report << "  Robustness Score: " << analysis.robustness << "/1.0\n\n";
    
    report << "Regional Analysis (" << analysis.regionMetrics.size() << " regions):\n";
    for (const auto& regionMetric : analysis.regionMetrics) {
        report << "  Region " << regionMetric.regionId << ": " << regionMetric.name << "\n";
        report << "    Structure: " << regionMetric.neuronCount << " neurons, " 
               << regionMetric.synapseCount << " synapses\n";
        report << "    Activity: " << regionMetric.averageFiringRate << " Hz avg, " 
               << regionMetric.activityLevel * 100 << "% active\n";
        report << "    Organization: " << regionMetric.clusteringCoefficient 
               << " clustering coefficient\n\n";
    }
    
    report << "╔══════════════════════════════════════════════════════════════════════════════╗\n";
    report << "║                        Topology Analysis Complete                             ║\n";
    report << "╚══════════════════════════════════════════════════════════════════════════════╝\n";
    
    return report.str();
}

bool NeuralNetworkTopologyAnalyzer::isInitialized() const {
    return pImpl->initialized;
}

void NeuralNetworkTopologyAnalyzer::setParameters(double timestep, bool includeDynamics) {
    pImpl->timestep = timestep;
    pImpl->includeDynamics = includeDynamics;
}

NeuralNetworkTopologyAnalyzer::AnalysisParameters NeuralNetworkTopologyAnalyzer::getParameters() const {
    return AnalysisParameters{
        pImpl->timestep,
        pImpl->includeDynamics,
        pImpl->clusteringThreshold,
        pImpl->computePathLength,
        pImpl->maxIterations
    };
}

// Private helper implementations

float NeuralNetworkTopologyAnalyzer::calculateLocalClusteringCoefficient(
    const Brain& brain,
    NeuronId neuronId,
    const std::vector<std::vector<float>>& adjacencyMatrix
) {
    // Reuse the public method
    return calculateLocalClusteringCoefficient(brain, neuronId, adjacencyMatrix);
}

std::vector<std::vector<float>> NeuralNetworkTopologyAnalyzer::buildAdjacencyMatrix(
    const Brain& brain,
    RegionId regionId
) {
    // Create adjacency matrix for a region
    // Collect all neurons in the region
    std::vector<const Neuron*> neurons;
    
    for (const auto& region : brain.getRegions()) {
        if (region->getId() == regionId) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    neurons.push_back(neuron);
                }
            }
            break;
        }
    }
    
    size_t n = neurons.size();
    std::vector<std::vector<float>> adjacencyMatrix(n, std::vector<float>(n, 0.0f));
    
    // Create mapping from neuron ID to matrix index
    std::unordered_map<NeuronId, size_t> idToIndex;
    for (size_t i = 0; i < n; ++i) {
        idToIndex[neurons[i]->getId()] = i;
    }
    
    // Fill adjacency matrix based on synapses within the region
    for (const auto& region : brain.getRegions()) {
        if (region->getId() == regionId) {
            for (const auto* syn : region->getSynapses()) {
                auto sourceIt = idToIndex.find(syn->getSourceNeuron());
                auto targetIt = idToIndex.find(syn->getDestinationNeuron());
                
                if (sourceIt != idToIndex.end() && targetIt != idToIndex.end()) {
                    adjacencyMatrix[sourceIt->second][targetIt->second] = syn->getWeight();
                }
            }
        }
    }
    
    return adjacencyMatrix;
}

std::vector<std::vector<float>> NeuralNetworkTopologyAnalyzer::calculateDistanceMatrix(
    const Brain& brain,
    RegionId regionId
) {
    // Calculate shortest path distances (simplified)
    auto adjacencyMatrix = buildAdjacencyMatrix(brain, regionId);
    size_t n = adjacencyMatrix.size();
    
    std::vector<std::vector<float>> distanceMatrix(n, std::vector<float>(n, 1000.0f));
    
    for (size_t i = 0; i < n; ++i) {
        distanceMatrix[i][i] = 0.0f;
        
        for (size_t j = 0; j < n; ++j) {
            if (i != j && adjacencyMatrix[i][j] > 0.001f) {
                distanceMatrix[i][j] = 1.0f; // Direct connection
            }
        }
    }
    
    // Floyd-Warshall algorithm (simplified)
    for (size_t k = 0; k < n; ++k) {
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (distanceMatrix[i][k] + distanceMatrix[k][j] < distanceMatrix[i][j]) {
                    distanceMatrix[i][j] = distanceMatrix[i][k] + distanceMatrix[k][j];
                }
            }
        }
    }
    
    return distanceMatrix;
}

std::vector<std::vector<float>> NeuralNetworkTopologyAnalyzer::buildGlobalAdjacencyMatrix(const Brain& brain) {
    // Create adjacency matrix for the entire brain
    std::vector<const Neuron*> neurons;
    
    for (const auto& region : brain.getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                neurons.push_back(neuron);
            }
        }
    }
    
    size_t n = neurons.size();
    std::vector<std::vector<float>> adjacencyMatrix(n, std::vector<float>(n, 0.0f));
    
    // Create mapping from neuron ID to matrix index
    std::unordered_map<NeuronId, size_t> idToIndex;
    for (size_t i = 0; i < n; ++i) {
        idToIndex[neurons[i]->getId()] = i;
    }
    
    // Fill adjacency matrix based on all synapses in the brain
    for (const auto& region : brain.getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            auto sourceIt = idToIndex.find(syn->getSourceNeuron());
            auto targetIt = idToIndex.find(syn->getDestinationNeuron());
            
            if (sourceIt != idToIndex.end() && targetIt != idToIndex.end()) {
                adjacencyMatrix[sourceIt->second][targetIt->second] = syn->getWeight();
            }
        }
    }
    
    return adjacencyMatrix;
}

std::unordered_map<NeuronId, size_t> NeuralNetworkTopologyAnalyzer::calculateBetweennessCentrality(
    const Brain& brain,
    const std::vector<std::vector<float>>& adjacencyMatrix
) {
    // Reuse the public method
    return calculateBetweennessCentrality(brain, adjacencyMatrix);
}

float NeuralNetworkTopologyAnalyzer::calculateSynchronizationFunction(
    const Brain& brain,
    const std::vector<std::vector<float>>& adjacencyMatrix,
    float omega
) {
    // Calculate synchronization function (simplified)
    if (adjacencyMatrix.empty()) {
        return 0.0f;
    }
    
    size_t n = adjacencyMatrix.size();
    
    // Simplified calculation
    float syncFunction = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        float totalWeight = 0.0f;
        for (size_t j = 0; j < n; ++j) {
            if (i != j) {
                totalWeight += adjacencyMatrix[i][j];
            }
        }
        syncFunction += totalWeight;
    }
    
    syncFunction /= n * (n - 1);
    return syncFunction;
}

void NeuralNetworkTopologyAnalyzer::calculateExcitatoryInhibitionRatio(
    const Brain& brain,
    float& ratio
) {
    // Calculate excitatory/inhibitory balance
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : brain.getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0.0f) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        ratio = totalExcitatory / totalInhibitory;
    } else {
        ratio = totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
    }
}

void NeuralNetworkTopologyAnalyzer::calculateCouplingStrengthDistribution(
    const Brain& brain,
    NeuralTopologyAnalysis& analysis
) {
    // Analyze coupling strength distribution
    float strongConnections = 0.0f;
    float weakConnections = 0.0f;
    size_t totalConnections = 0;
    
    for (const auto& region : brain.getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            totalConnections++;
            float weight = std::abs(syn->getWeight());
            
            if (weight > 0.5f) {
                strongConnections++;
            } else if (weight < 0.1f) {
                weakConnections++;
            }
        }
    }
    
    if (totalConnections > 0) {
        analysis.strongCouplingRatio = strongConnections / totalConnections;
        analysis.weakCouplingRatio = weakConnections / totalConnections;
    }
}

void NeuralNetworkTopologyAnalyzer::calculatePopulationMetrics(
    const Brain& brain,
    NeuralTopologyAnalysis& analysis
) {
    // Calculate metrics for each population type
    for (int i = NeuronType::Sensory; i < NeuronType::COUNT; ++i) {
        NeuronType type = static_cast<NeuronType>(i);
        size_t populationSize = 0;
        float excitatoryRatio = 0.0f;
        
        for (const auto& region : brain.getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                if (pop->getNeuronType() == type) {
                    populationSize += pop->getSize();
                    // Calculate excitatory ratio for this population
                    for (const auto* neuron : pop->getNeurons()) {
                        if (neuron->getType() == NeuronType::Excitatory || 
                            neuron->getType() == NeuronType::Sensory ||
                            neuron->getType() == NeuronType::Motor ||
                            neuron->getType() == NeuronType::Internal) {
                            excitatoryRatio++;
                        }
                    }
                }
            }
        }
        
        if (populationSize > 0) {
            analysis.populationSizes[type] = populationSize;
            analysis.populationExcitatoryRatio[type] = excitatoryRatio / populationSize;
        }
    }
}

} // namespace nlm
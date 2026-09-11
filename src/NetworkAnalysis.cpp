// NLM Network Analysis implementation
// Graph theory-based network connectivity analysis

#include "NetworkAnalysis.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

namespace nlm {

struct NetworkAnalyzer::Impl {
    std::shared_ptr<Logger> logger;
    
    Impl() {
        logger = std::make_shared<Logger>();
        logger->setLevel(LogLevel::INFO);
    }
    
    // Floyd-Warshall algorithm for all-pairs shortest paths
    std::vector<std::vector<float>> floydWarshall(const std::vector<GraphNode>& graph) {
        size_t n = graph.size();
        std::vector<std::vector<float>> dist(n, std::vector<float>(n, std::numeric_limits<float>::max()));
        
        // Initialize distance matrix
        for (size_t i = 0; i < n; ++i) {
            dist[i][i] = 0.0f;
            for (const auto& conn : graph[i].connections) {
                size_t j = conn.first;
                if (j < n) {
                    dist[i][j] = conn.second;
                }
            }
        }
        
        // Floyd-Warshall algorithm
        for (size_t k = 0; k < n; ++k) {
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < n; ++j) {
                    if (dist[i][k] < std::numeric_limits<float>::max() && 
                        dist[k][j] < std::numeric_limits<float>::max()) {
                        dist[i][j] = std::min(dist[i][j], dist[i][k] + dist[k][j]);
                    }
                }
            }
        }
        
        return dist;
    }
    
    // Calculate average shortest path length
    float calculateAveragePathLength(const std::vector<std::vector<float>>& dist) {
        float total = 0.0f;
        int count = 0;
        
        for (const auto& row : dist) {
            for (float d : row) {
                if (d < std::numeric_limits<float>::max() && d > 0) {
                    total += d;
                    count++;
                }
            }
        }
        
        return (count > 0) ? total / count : 0.0f;
    }
    
    // Calculate network diameter
    float calculateDiameter(const std::vector<std::vector<float>>& dist) {
        float diameter = 0.0f;
        
        for (const auto& row : dist) {
            for (float d : row) {
                if (d < std::numeric_limits<float>::max()) {
                    diameter = std::max(diameter, d);
                }
            }
        }
        
        return diameter;
    }
    
    // Calculate global efficiency
    float calculateGlobalEfficiency(const std::vector<std::vector<float>>& dist) {
        float totalEfficiency = 0.0f;
        int count = 0;
        
        for (const auto& row : dist) {
            for (float d : row) {
                if (d < std::numeric_limits<float>::max() && d > 0) {
                    totalEfficiency += 1.0f / d;
                    count++;
                }
            }
        }
        
        return (count > 0) ? totalEfficiency / (count * count) : 0.0f;
    }
};

// NetworkAnalyzer implementation

NetworkAnalyzer::NetworkAnalyzer() : pImpl(std::make_unique<Impl>()) {}

NetworkAnalyzer::~NetworkAnalyzer() = default;

void NetworkAnalyzer::initialize() {
    pImpl->logger->setLevel(LogLevel::INFO);
    pImpl->logger->log("Network analyzer initialized", LogLevel::INFO);
}

std::vector<GraphNode> NetworkAnalyzer::extractGraph(std::shared_ptr<Brain> brain) {
    std::vector<GraphNode> graph;
    
    if (!brain) {
        return graph;
    }
    
    // Get all regions from brain
    auto regionIds = brain->getRegionIds();
    graph.reserve(regionIds.size());
    
    // Create nodes for each region
    for (size_t i = 0; i < regionIds.size(); ++i) {
        GraphNode node;
        node.id = i;
        node.type = "region";
        node.x = static_cast<float>(i); // Placeholder position
        node.y = 0.0f;
        node.z = 0.0f;
        
        // Add connections to other regions
        for (size_t j = 0; j < regionIds.size(); ++j) {
            if (i != j) {
                auto conn = brain->getInterRegionConnection(regionIds[i], regionIds[j]);
                if (conn > 0) {
                    node.connections[j] = conn;
                    node.incomingConnections.push_back(j);
                }
            }
        }
        
        graph.push_back(node);
    }
    
    // If no regions, create nodes from neurons
    if (graph.empty() && brain->getTotalNeuronCount() > 0) {
        size_t neuronCount = std::min<size_t>(1000, brain->getTotalNeuronCount()); // Limit for performance
        graph.reserve(neuronCount);
        
        for (size_t i = 0; i < neuronCount; ++i) {
            GraphNode node;
            node.id = i;
            node.type = "neuron";
            node.x = static_cast<float>(i % 50); // Grid layout
            node.y = static_cast<float>((i / 50) % 10);
            node.z = 0.0f;
            
            // Simple connectivity model based on neuron indices
            if (i > 0) {
                node.connections[i - 1] = 1.0f / (i + 1); // Weight decreases with distance
                node.incomingConnections.push_back(i - 1);
            }
            
            if (i < neuronCount - 1) {
                node.connections[i + 1] = 1.0f / (i + 2);
            }
            
            graph.push_back(node);
        }
    }
    
    pImpl->logger->log("Extracted network graph with " + std::to_string(graph.size()) + " nodes", LogLevel::INFO);
    return graph;
}

NetworkMetrics NetworkAnalyzer::computeMetrics(const std::vector<GraphNode>& graph) {
    NetworkMetrics metrics;
    metrics.nodeCount = graph.size();
    
    // Count edges
    metrics.edgeCount = 0;
    for (const auto& node : graph) {
        metrics.edgeCount += node.connections.size();
    }
    
    // Calculate density
    if (metrics.nodeCount > 1) {
        float maxEdges = static_cast<float>(metrics.nodeCount * (metrics.nodeCount - 1));
        metrics.density = metrics.edgeCount / maxEdges;
    }
    
    // Calculate average degree
    metrics.averageDegree = (metrics.nodeCount > 0) ? 
        static_cast<float>(metrics.edgeCount) / metrics.nodeCount : 0.0f;
    
    // Calculate clustering coefficient (simplified)
    float totalClustering = 0.0f;
    int clusteringCount = 0;
    
    for (const auto& node : graph) {
        if (node.connections.size() < 2) continue;
        
        // Count triangles (shared connections among neighbors)
        int triangles = 0;
        for (const auto& neighbor1 : node.connections) {
            auto it1 = find(node.incomingConnections.begin(), node.incomingConnections.end(), neighbor1.first);
            if (it1 == node.incomingConnections.end()) continue;
            
            for (const auto& neighbor2 : node.connections) {
                if (neighbor1.first == neighbor2.first) continue;
                
                // Check if neighbor1 and neighbor2 are connected
                bool connected = false;
                for (const auto& conn : graph[neighbor1.first].connections) {
                    if (conn.first == neighbor2.first) {
                        connected = true;
                        break;
                    }
                }
                
                if (connected) triangles++;
            }
        }
        
        int possibleTriangles = node.connections.size() * (node.connections.size() - 1) / 2;
        if (possibleTriangles > 0) {
            totalClustering += static_cast<float>(triangles) / possibleTriangles;
            clusteringCount++;
        }
    }
    
    metrics.clusteringCoefficient = (clusteringCount > 0) ? totalClustering / clusteringCount : 0.0f;
    
    // Compute shortest paths
    auto dist = pImpl->floydWarshall(graph);
    
    // Calculate small-world coefficient (placeholder)
    metrics.smallWorldCoefficient = metrics.clusteringCoefficient / metrics.averageDegree; // Simplified
    
    // Calculate average path length
    metrics.averagePathLength = pImpl->calculateAveragePathLength(dist);
    
    // Calculate diameter
    metrics.diameter = pImpl->calculateDiameter(dist);
    
    // Calculate global efficiency
    metrics.efficiency = pImpl->calculateGlobalEfficiency(dist);
    
    // Calculate modularity (placeholder)
    metrics.modularity = 0.5f; // Placeholder
    
    // Calculate transitivity (clustering coefficient again)
    metrics.transitivity = metrics.clusteringCoefficient;
    
    // Calculate centralization (placeholder)
    metrics.centralization = 0.3f; // Placeholder
    
    return metrics;
}

std::vector<CommunityResult> NetworkAnalyzer::detectCommunities(const std::vector<GraphNode>& graph) {
    std::vector<CommunityResult> communities;
    
    if (graph.empty()) {
        return communities;
    }
    
    // Simple greedy community detection (placeholder)
    size_t numNodes = graph.size();
    std::vector<bool> visited(numNodes, false);
    size_t communityId = 0;
    
    for (size_t i = 0; i < numNodes; ++i) {
        if (!visited[i]) {
            CommunityResult community;
            community.communityId = communityId++;
            
            // Simple connected component detection
            std::queue<size_t> queue;
            queue.push(i);
            visited[i] = true;
            
            while (!queue.empty()) {
                size_t current = queue.front();
                queue.pop();
                
                community.memberNodes.push_back(current);
                
                // Add neighbors
                for (const auto& conn : graph[current].connections) {
                    if (!visited[conn.first]) {
                        visited[conn.first] = true;
                        queue.push(conn.first);
                    }
                }
                
                for (size_t incoming : graph[current].incomingConnections) {
                    if (!visited[incoming]) {
                        visited[incoming] = true;
                        queue.push(incoming);
                    }
                }
            }
            
            // Calculate community metrics
            community.density = static_cast<float>(community.memberNodes.size()) / numNodes;
            community.cohesion = community.density * (static_cast<float>(community.memberNodes.size()) / 2.0f);
            
            communities.push_back(community);
        }
    }
    
    return communities;
}

std::vector<std::vector<float>> NetworkAnalyzer::computeShortestPaths(const std::vector<GraphNode>& graph) {
    return pImpl->floydWarshall(graph);
}

std::string NetworkAnalyzer::visualizeNetwork(const std::vector<GraphNode>& graph, 
                                             size_t maxNodes) const {
    std::stringstream ss;
    
    ss << "Network Visualization (showing " << std::min(graph.size(), maxNodes) << " nodes):" << std::endl;
    ss << "=================================" << std::endl;
    
    size_t displayCount = std::min(graph.size(), maxNodes);
    
    for (size_t i = 0; i < displayCount; ++i) {
        const auto& node = graph[i];
        ss << "Node " << i << " (type: " << node.type << ") connections:";
        
        for (const auto& conn : node.connections) {
            ss << " -> " << conn.first << "(" << conn.second << ")";
        }
        
        ss << std::endl;
    }
    
    return ss.str();
}

std::string NetworkAnalyzer::generateReport(const std::vector<GraphNode>& graph,
                                           const NetworkMetrics& metrics) const {
    std::stringstream ss;
    
    ss << "Network Analysis Report" << std::endl;
    ss << "=======================" << std::endl;
    ss << "Nodes: " << metrics.nodeCount << std::endl;
    ss << "Edges: " << metrics.edgeCount << std::endl;
    ss << "Density: " << metrics.density << std::endl;
    ss << "Average Degree: " << metrics.averageDegree << std::endl;
    ss << "Clustering Coefficient: " << metrics.clusteringCoefficient << std::endl;
    ss << "Small-World Coefficient: " << metrics.smallWorldCoefficient << std::endl;
    ss << "Average Path Length: " << metrics.averagePathLength << std::endl;
    ss << "Network Diameter: " << metrics.diameter << std::endl;
    ss << "Global Efficiency: " << metrics.efficiency << std::endl;
    ss << "Modularity: " << metrics.modularity << std::endl;
    ss << "Transitivity: " << metrics.transitivity << std::endl;
    ss << "Centralization: " << metrics.centralization << std::endl;
    
    return ss.str();
}

std::string NetworkAnalyzer::getNetworkStatistics(const std::vector<GraphNode>& graph) const {
    return visualizeNetwork(graph, 100);
}

std::map<std::string, float> NetworkAnalyzer::calculateCentrality(const std::vector<GraphNode>& graph) {
    std::map<std::string, float> centralityMetrics;
    
    if (graph.empty()) {
        return centralityMetrics;
    }
    
    size_t n = graph.size();
    
    // Degree centrality
    float maxDegree = 0.0f;
    float totalDegree = 0.0f;
    
    for (const auto& node : graph) {
        float degree = node.connections.size() + node.incomingConnections.size();
        totalDegree += degree;
        maxDegree = std::max(maxDegree, degree);
    }
    
    centralityMetrics["degree_centrality"] = (maxDegree > 0) ? totalDegree / maxDegree : 0.0f;
    
    // Betweenness centrality (simplified)
    centralityMetrics["betweenness_centrality"] = 0.5f; // Placeholder
    
    // Closeness centrality (simplified)
    centralityMetrics["closeness_centrality"] = 0.7f; // Placeholder
    
    // Eigenvector centrality (simplified)
    centralityMetrics["eigenvector_centrality"] = 0.6f; // Placeholder
    
    return centralityMetrics;
}

std::map<std::string, float> NetworkAnalyzer::calculateCommunityMetrics(const std::vector<CommunityResult>& communities) {
    std::map<std::string, float> metrics;
    
    if (communities.empty()) {
        return metrics;
    }
    
    float totalCohesion = 0.0f;
    float totalDensity = 0.0f;
    
    for (const auto& community : communities) {
        totalCohesion += community.cohesion;
        totalDensity += community.density;
    }
    
    metrics["average_cohesion"] = totalCohesion / communities.size();
    metrics["average_density"] = totalDensity / communities.size();
    metrics["number_of_communities"] = static_cast<float>(communities.size());
    
    return metrics;
}

std::vector<std::vector<size_t>> NetworkAnalyzer::detectMotifs(const std::vector<GraphNode>& graph) {
    std::vector<std::vector<size_t>> motifs;
    
    if (graph.size() < 3) {
        return motifs;
    }
    
    // Detect triangle motifs (simple)
    std::unordered_map<size_t, std::unordered_set<size_t>> adjList;
    
    for (size_t i = 0; i < graph.size(); ++i) {
        for (const auto& conn : graph[i].connections) {
            adjList[i].insert(conn.first);
        }
    }
    
    for (size_t i = 0; i < graph.size(); ++i) {
        for (size_t j : adjList[i]) {
            if (j <= i) continue;
            
            std::vector<size_t> triangle;
            triangle.push_back(i);
            triangle.push_back(j);
            
            for (size_t k : adjList[i]) {
                if (k != j && adjList[j].find(k) != adjList[j].end()) {
                    triangle.push_back(k);
                    motifs.push_back(triangle);
                    triangle.pop_back();
                }
            }
        }
    }
    
    return motifs;
}

std::vector<std::string> NetworkAnalyzer::compareNetworks(const std::vector<GraphNode>& graph1,
                                                         const std::vector<GraphNode>& graph2) {
    std::vector<std::string> differences;
    
    if (graph1.size() != graph2.size()) {
        differences.push_back("Different number of nodes: " + std::to_string(graph1.size()) + " vs " + std::to_string(graph2.size()));
    }
    
    // Compare average degrees
    float avgDegree1 = 0.0f, avgDegree2 = 0.0f;
    
    for (const auto& node : graph1) {
        avgDegree1 += node.connections.size() + node.incomingConnections.size();
    }
    
    for (const auto& node : graph2) {
        avgDegree2 += node.connections.size() + node.incomingConnections.size();
    }
    
    if (graph1.size() > 0 && graph2.size() > 0) {
        avgDegree1 /= graph1.size();
        avgDegree2 /= graph2.size();
        
        if (std::abs(avgDegree1 - avgDegree2) > 0.1f) {
            differences.push_back("Different average degrees: " + std::to_string(avgDegree1) + " vs " + std::to_string(avgDegree2));
        }
    }
    
    return differences;
}

bool NetworkAnalyzer::exportNetwork(const std::vector<GraphNode>& graph, 
                                   const std::string& filepath,
                                   const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "graphml") {
        // Simple GraphML format
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        file << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\">" << std::endl;
        file << "  <graph id=\"0\">" << std::endl;
        
        for (size_t i = 0; i < graph.size(); ++i) {
            file << "    <node id=\"n" << i << "/>" << std::endl;
        }
        
        for (size_t i = 0; i < graph.size(); ++i) {
            for (const auto& conn : graph[i].connections) {
                file << "    <edge source=\"n" << i << "\" target=\"n" << conn.first << " weight=\"" 
                     << conn.second << "/>" << std::endl;
            }
        }
        
        file << "  </graph>" << std::endl;
        file << "</graphml>" << std::endl;
    } else {
        // Simple text format
        file << "Network Graph" << std::endl;
        file << "=============" << std::endl;
        file << "Nodes: " << graph.size() << std::endl;
        file << "Edges:" << std::endl;
        
        for (size_t i = 0; i < graph.size(); ++i) {
            for (const auto& conn : graph[i].connections) {
                file << "  " << i << " -> " << conn.first << " (weight: " << conn.second << ")" << std::endl;
            }
        }
    }
    
    file.close();
    pImpl->logger->log("Exported network to: " + filepath, LogLevel::INFO);
    return true;
}

bool NetworkAnalyzer::importNetwork(std::vector<GraphNode>& graph,
                                    const std::string& filepath,
                                    const std::string& format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple import (placeholder)
    file.close();
    pImpl->logger->log("Imported network from: " + filepath, LogLevel::INFO);
    return true;
}

void NetworkAnalyzer::reset() {
    pImpl->logger->setLevel(LogLevel::INFO);
    pImpl->logger->log("Network analyzer reset", LogLevel::INFO);
}

void NetworkAnalyzer::shutdown() {
    reset();
    pImpl->logger->log("Network analyzer shutdown", LogLevel::INFO);
}

} // namespace nlm

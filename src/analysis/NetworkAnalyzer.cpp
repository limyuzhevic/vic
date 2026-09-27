#include "NetworkAnalyzer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_set>

namespace nlm {
namespace analysis {

// NetworkTopologyAnalyzer implementation
bool NetworkTopologyAnalyzer::addConnection(const NeuralConnection& connection) {
    // Validate connection
    if (connection.connections.empty()) {
        return false;
    }

    // Check for duplicate connections
    for (const auto& conn : connections_) {
        if (conn.source_layer == connection.source_layer && 
            conn.target_layer == connection.target_layer) {
            return false; // Connection already exists
        }
    }

    connections_.push_back(connection);
    return true;
}

bool NetworkTopologyAnalyzer::addTopology(const NetworkTopology& topology) {
    // Validate topology
    if (topology.node_ids.empty()) {
        return false;
    }

    // Check for duplicate topology
    for (const auto& existing : topologies_) {
        if (existing.network_id == topology.network_id) {
            return false; // Topology already exists
        }
    }

    topologies_[topology.network_id] = topology;
    return true;
}

double NetworkTopologyAnalyzer::computeNodeDegree(const std::string& node_id) const {
    double degree = 0.0;
    
    // Count incoming and outgoing connections
    for (const auto& conn : connections_) {
        // Check if node is in source neurons of target connections
        for (const auto& source : conn.source_neurons) {
            if (source == node_id) {
                degree += 1.0;
            }
        }
        
        for (const auto& target : conn.target_neurons) {
            if (target == node_id) {
                degree += 1.0;
            }
        }
    }
    
    return degree;
}

double NetworkTopologyAnalyzer::computeOutDegree(const std::string& node_id) const {
    double out_degree = 0.0;
    
    for (const auto& conn : connections_) {
        for (const auto& source : conn.source_neurons) {
            if (source == node_id) {
                out_degree += conn.connections.size();
            }
        }
    }
    
    return out_degree;
}

double NetworkTopologyAnalyzer::computeInDegree(const std::string& node_id) const {
    double in_degree = 0.0;
    
    for (const auto& conn : connections_) {
        for (const auto& target : conn.target_neurons) {
            if (target == node_id) {
                in_degree += conn.connections.size();
            }
        }
    }
    
    return in_degree;
}

double NetworkTopologyAnalyzer::computeClusteringCoefficient(const std::string& node_id) const {
    std::unordered_set<std::string> neighbors;
    
    // Find neighbors of the node
    for (const auto& conn : connections_) {
        for (const auto& source : conn.source_neurons) {
            if (source == node_id) {
                for (const auto& target : conn.target_neurons) {
                    neighbors.insert(target);
                }
            }
        }
        
        for (const auto& target : conn.target_neurons) {
            if (target == node_id) {
                for (const auto& source : conn.source_neurons) {
                    neighbors.insert(source);
                }
            }
        }
    }
    
    if (neighbors.size() < 2) {
        return 0.0; // Not enough neighbors for clustering
    }
    
    // Count triangles (simplified)
    int triangle_count = 0;
    std::vector<std::string> neighbor_list(neighbors.begin(), neighbors.end());
    
    for (size_t i = 0; i < neighbor_list.size(); ++i) {
        for (size_t j = i + 1; j < neighbor_list.size(); ++j) {
            std::string n1 = neighbor_list[i];
            std::string n2 = neighbor_list[j];
            
            // Check if n1 and n2 are connected
            bool connected = false;
            for (const auto& conn : connections_) {
                for (const auto& source : conn.source_neurons) {
                    for (const auto& target : conn.target_neurons) {
                        if ((source == n1 && target == n2) || 
                            (source == n2 && target == n1)) {
                            connected = true;
                            break;
                        }
                    }
                    if (connected) break;
                }
                if (connected) break;
            }
            
            if (connected) {
                triangle_count++;
            }
        }
    }
    
    // Maximum possible triangles
    int max_triangles = neighbor_list.size() * (neighbor_list.size() - 1) / 2;
    
    return static_cast<double>(triangle_count) / max_triangles;
}

double NetworkTopologyAnalyzer::computeGlobalClusteringCoefficient() const {
    if (connections_.empty()) {
        return 0.0;
    }
    
    double total_clustering = 0.0;
    int node_count = 0;
    
    // Collect all unique node IDs
    std::unordered_set<std::string> all_nodes;
    for (const auto& conn : connections_) {
        for (const auto& source : conn.source_neurons) {
            all_nodes.insert(source);
        }
        for (const auto& target : conn.target_neurons) {
            all_nodes.insert(target);
        }
    }
    
    node_count = all_nodes.size();
    if (node_count == 0) {
        return 0.0;
    }
    
    // Compute clustering for each node
    for (const auto& node : all_nodes) {
        total_clustering += computeClusteringCoefficient(node);
    }
    
    return total_clustering / node_count;
}

double NetworkTopologyAnalyzer::computeCharacteristicPathLength() const {
    // Compute shortest paths between all pairs of nodes
    std::vector<std::string> nodes;
    std::unordered_set<std::string> node_set;
    
    for (const auto& conn : connections_) {
        for (const auto& source : conn.source_neurons) {
            if (node_set.find(source) == node_set.end()) {
                node_set.insert(source);
                nodes.push_back(source);
            }
        }
        for (const auto& target : conn.target_neurons) {
            if (node_set.find(target) == node_set.end()) {
                node_set.insert(target);
                nodes.push_back(target);
            }
        }
    }
    
    if (nodes.size() < 2) {
        return 0.0;
    }
    
    // Build adjacency matrix
    std::map<std::string, std::map<std::string, double>> adj_matrix;
    for (const auto& node1 : nodes) {
        adj_matrix[node1] = std::map<std::string, double>();
        for (const auto& node2 : nodes) {
            adj_matrix[node1][node2] = (node1 == node2) ? 0.0 : std::numeric_limits<double>::max();
        }
    }
    
    // Fill adjacency matrix with connection weights
    for (const auto& conn : connections_) {
        for (const auto& source : conn.source_neurons) {
            for (const auto& target : conn.target_neurons) {
                double weight = std::find(conn.connections.begin(), conn.connections.end(),
                                         std::make_pair(source, target) != conn.connections.end()) ? 1.0 : 0.0;
                adj_matrix[source][target] = weight;
            }
        }
    }
    
    // Compute shortest path lengths using Floyd-Warshall algorithm
    double total_path_length = 0.0;
    int valid_paths = 0;
    
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = 0; j < nodes.size(); ++j) {
            if (i != j) {
                double path_length = std::numeric_limits<double>::max();
                
                // Check if there's a path
                bool has_path = false;
                for (const auto& conn : connections_) {
                    for (const auto& pair : conn.connections) {
                        if (pair.first == nodes[i] && pair.second == nodes[j]) {
                            has_path = true;
                            path_length = 1.0; // Default path length
                            break;
                        }
                    }
                    if (has_path) break;
                }
                
                if (has_path) {
                    total_path_length += path_length;
                    valid_paths++;
                }
            }
        }
    }
    
    if (valid_paths == 0) {
        return 0.0;
    }
    
    return total_path_length / valid_paths;
}

double NetworkTopologyAnalyzer::computeAveragePathLength() const {
    return computeCharacteristicPathLength();
}

double NetworkTopologyAnalyzer::computeSmallWorldness() const {
    double clustering = computeGlobalClusteringCoefficient();
    double path_length = computeAveragePathLength();
    
    // For a random network with same number of nodes and edges
    size_t node_count = 0;
    for (const auto& conn : connections_) {
        node_count = std::max(node_count, static_cast<size_t>(conn.source_neurons.size()));
        node_count = std::max(node_count, static_cast<size_t>(conn.target_neurons.size()));
    }
    
    // Placeholder for random network calculation
    double random_clustering = 0.01 * node_count; // Approximate
    double random_path_length = std::log(node_count) / std::log(2); // Approximate
    
    if (random_path_length == 0.0) {
        return 0.0;
    }
    
    double small_worldness = (clustering / random_clustering) / (path_length / random_path_length);
    return small_worldness;
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeBetweennessCentrality() const {
    std::map<std::string, double> centrality;
    
    // Collect all nodes
    std::unordered_set<std::string> all_nodes;
    for (const auto& conn : connections_) {
        for (const auto& source : conn.source_neurons) {
            all_nodes.insert(source);
        }
        for (const auto& target : conn.target_neurons) {
            all_nodes.insert(target);
        }
    }
    
    for (const auto& node : all_nodes) {
        centrality[node] = 0.0;
    }
    
    // For now, return simple degree-based centrality (placeholder)
    for (const auto& conn : connections_) {
        for (const auto& source : conn.source_neurons) {
            centrality[source] += static_cast<double>(conn.connections.size());
        }
        for (const auto& target : conn.target_neurons) {
            centrality[target] += static_cast<double>(conn.connections.size());
        }
    }
    
    return centrality;
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeClosenessCentrality() const {
    std::map<std::string, double> centrality;
    
    // Collect all nodes
    std::unordered_set<std::string> all_nodes;
    for (const auto& conn : connections_) {
        for (const auto& source : conn.source_neurons) {
            all_nodes.insert(source);
        }
        for (const auto& target : conn.target_neurons) {
            all_nodes.insert(target);
        }
    }
    
    if (all_nodes.size() <= 1) {
        return centrality;
    }
    
    // Compute average shortest path length to all other nodes
    for (const auto& node : all_nodes) {
        double total_distance = 0.0;
        int reachable_nodes = 0;
        
        // Simplified: assume all nodes are connected with distance 1
        reachable_nodes = static_cast<int>(all_nodes.size()) - 1;
        total_distance = static_cast<double>(reachable_nodes);
        
        centrality[node] = reachable_nodes > 0 ? 1.0 / total_distance : 0.0;
    }
    
    return centrality;
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeEigenvectorCentrality() const {
    // In real implementation, would compute eigenvector centrality
    // For now, return degree-based centrality
    return computeBetweennessCentrality();
}

std::map<std::string, double> NetworkTopologyAnalyzer::computePageRank() const {
    // In real implementation, would compute PageRank
    // For now, return eigenvector centrality as approximation
    return computeEigenvectorCentrality();
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeLRCentrality() const {
    // In real implementation, would compute local reachability centrality
    // For now, return betweenness centrality as approximation
    return computeBetweennessCentrality();
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeStressCentrality() const {
    // In real implementation, would compute stress centrality
    // For now, return betweenness centrality as approximation
    return computeBetweennessCentrality();
}

std::vector<std::vector<std::string>> NetworkTopologyAnalyzer::detectCommunities() const {
    // In real implementation, would use community detection algorithms
    // For now, return single community containing all nodes
    std::vector<std::vector<std::string>> communities;
    
    std::unordered_set<std::string> all_nodes;
    for (const auto& conn : connections_) {
        for (const auto& source : conn.source_neurons) {
            all_nodes.insert(source);
        }
        for (const auto& target : conn.target_neurons) {
            all_nodes.insert(target);
        }
    }
    
    if (!all_nodes.empty()) {
        communities.push_back(std::vector<std::string>(all_nodes.begin(), all_nodes.end()));
    }
    
    return communities;
}

std::map<std::string, std::vector<std::string>> NetworkTopologyAnalyzer::detectModularityCommunities() const {
    // In real implementation, would use modularity-based community detection
    // For now, return single community
    std::map<std::string, std::vector<std::string>> communities;
    
    std::unordered_set<std::string> all_nodes;
    for (const auto& conn : connections_) {
        for (const auto& source : conn.source_neurons) {
            all_nodes.insert(source);
        }
        for (const auto& target : conn.target_neurons) {
            all_nodes.insert(target);
        }
    }
    
    if (!all_nodes.empty()) {
        communities["community_0"] = std::vector<std::string>(all_nodes.begin(), all_nodes.end());
    }
    
    return communities;
}

double NetworkTopologyAnalyzer::computeModularity() const {
    // In real implementation, would compute network modularity
    // For now, return a placeholder value
    return 0.5;
}

double NetworkTopologyAnalyzer::computeModularity(const std::vector<std::vector<std::string>>& communities) const {
    return computeModularity();
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeDynamicalStability() const {
    // In real implementation, would compute dynamical stability metrics
    // For now, return placeholder values
    std::map<std::string, double> stability;
    stability["node_stability"] = 0.8;
    stability["network_stability"] = 0.7;
    stability["oscillation_stability"] = 0.6;
    return stability;
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeSynchronizationProbability() const {
    // In real implementation, would compute synchronization probability
    // For now, return a placeholder value
    return std::map<std::string, double>{{"synchronization_probability", 0.9}};
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeInformationFlow() const {
    // In real implementation, would compute information flow metrics
    // For now, return a placeholder value
    return std::map<std::string, double>{{"information_flow", 0.8}};
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeNetworkCapacity() const {
    // In real implementation, would compute network capacity
    // For now, return a placeholder value
    return std::map<std::string, double>{{"capacity", 1.0}};
}

double NetworkTopologyAnalyzer::computeRobustnessToNodeRemoval() const {
    // In real implementation, would compute robustness to node removal
    // For now, return a placeholder value
    return 0.8;
}

double NetworkTopologyAnalyzer::computeRobustnessToEdgeRemoval() const {
    // In real implementation, would compute robustness to edge removal
    // For now, return a placeholder value
    return 0.9;
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeAttackTolerance() const {
    // In real implementation, would compute attack tolerance
    // For now, return placeholder values
    std::map<std::string, double> tolerance;
    tolerance["random_attack"] = 0.7;
    tolerance["targeted_attack"] = 0.5;
    tolerance["heirarchical_attack"] = 0.6;
    return tolerance;
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeRecoveryCapability() const {
    // In real implementation, would compute recovery capability
    // For now, return a placeholder value
    return std::map<std::string, double>{{"recovery_capability", 0.8}};
}

double NetworkTopologyAnalyzer::computeGlobalEfficiency() const {
    // In real implementation, would compute global efficiency
    // For now, return a placeholder value
    return 0.8;
}

double NetworkTopologyAnalyzer::computeLocalEfficiency(const std::string& node_id) const {
    // In real implementation, would compute local efficiency
    // For now, return a placeholder value
    return 0.7;
}

double NetworkTopologyAnalyzer::computeNetworkDiameter() const {
    // In real implementation, would compute network diameter
    // For now, return a placeholder value
    return 5.0;
}

std::vector<double> NetworkTopologyAnalyzer::computeDiameterDistribution() const {
    // In real implementation, would compute diameter distribution
    // For now, return a placeholder vector
    return std::vector<double>{1.0, 2.0, 3.0, 4.0, 5.0};
}

double NetworkTopologyAnalyzer::computeNetworkEntropy() const {
    // In real implementation, would compute network entropy
    // For now, return a placeholder value
    return 0.5;
}

double NetworkTopologyAnalyzer::computeDegreeDistributionEntropy() const {
    // In real implementation, would compute degree distribution entropy
    // For now, return a placeholder value
    return 0.6;
}

double NetworkTopologyAnalyzer::computeBetweennessDistributionEntropy() const {
    // In real implementation, would compute betweenness distribution entropy
    // For now, return a placeholder value
    return 0.7;
}

std::map<std::string, double> NetworkTopologyAnalyzer::analyzeDynamicTopology(const std::string& time_window) const {
    // In real implementation, would analyze dynamic topology
    // For now, return placeholder values
    std::map<std::string, double> analysis;
    analysis["stability"] = 0.8;
    analysis["connectivity"] = 0.9;
    analysis["robustness"] = 0.7;
    return analysis;
}

std::vector<double> NetworkTopologyAnalyzer::computeTopologyEvolution() const {
    // In real implementation, would compute topology evolution
    // For now, return a placeholder vector
    return std::vector<double>{1.0, 1.1, 1.2, 1.3, 1.4};
}

std::map<std::string, double> NetworkTopologyAnalyzer::computeTopologyVariability() const {
    // In real implementation, would compute topology variability
    // For now, return placeholder values
    std::map<std::string, double> variability;
    variability["structural_variability"] = 0.2;
    variability["functional_variability"] = 0.3;
    return variability;
}

std::string NetworkTopologyAnalyzer::generateAdjacencyMatrix() const {
    std::stringstream matrix;
    matrix << "Adjacency Matrix:" << std::endl;
    matrix << "(In real implementation, would show actual adjacency matrix)" << std::endl;
    return matrix.str();
}

std::string NetworkTopologyAnalyzer::generateCommunityPlot() const {
    return "Community Plot Visualization:" << std::endl << "(In real implementation, would generate visualization)";
}

std::string NetworkTopologyAnalyzer::generateCentralityHeatmap() const {
    return "Centrality Heatmap:" << std::endl << "(In real implementation, would generate heatmap)";
}

std::string NetworkTopologyAnalyzer::generateNetworkFlowChart() const {
    return "Network Flow Chart:" << std::endl << "(In real implementation, would generate flow chart)";
}

bool NetworkTopologyAnalyzer::exportNetwork(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        return false;
    }

    try {
        file << "Network Topology Export:" << std::endl;
        file << "========================" << std::endl;
        file << "Number of connections: " << connections_.size() << std::endl;
        file << "Number of topologies: " << topologies_.size() << std::endl;
        file << std::endl;

        file << "Connections:" << std::endl;
        for (size_t i = 0; i < connections_.size(); ++i) {
            file << "  " << i << ": " << connections_[i].source_layer << " -> " 
                 << connections_[i].target_layer << std::endl;
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting network: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

std::unique_ptr<NetworkTopologyAnalyzer> NetworkTopologyAnalyzer::importNetwork(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        return nullptr;
    }

    // In real implementation, would parse network from file
    file.close();
    return std::make_unique<NetworkTopologyAnalyzer>();
}

size_t NetworkTopologyAnalyzer::getEdgeCount() const {
    size_t edge_count = 0;
    for (const auto& conn : connections_) {
        edge_count += conn.connections.size();
    }
    return edge_count;
}

} // namespace analysis
} // namespace nlm
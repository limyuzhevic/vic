#include "NetworkIOProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <queue>
#include <set>
#include <functional>
#include <atomic>

namespace nlm {

struct NetworkIOProfiler::Impl {
    // Network data storage
    std::vector<NetworkPacket> packets;
    mutable std::mutex packetsMutex;
    
    std::unordered_map<uint64_t, NetworkConnection> connections;
    mutable std::mutex connectionsMutex;
    
    std::vector<NetworkThroughput> throughputs;
    mutable std::mutex throughputsMutex;
    
    std::vector<NetworkCongestion> congestions;
    mutable std::mutex congestionsMutex;
    
    std::vector<NetworkError> errors;
    mutable std::mutex errorsMutex;
    
    // Packet capture thread
    std::thread packetCaptureThread;
    bool packetCaptureActive;
    std::chrono::milliseconds packetCaptureInterval;
    
    // Connection monitoring thread
    std::thread connectionMonitorThread;
    bool connectionMonitorActive;
    std::chrono::milliseconds connectionMonitorInterval;
    
    // Performance monitoring thread
    std::thread performanceMonitorThread;
    bool performanceMonitorActive;
    std::chrono::milliseconds performanceMonitorInterval;
    
    // Configuration
    NetworkProfilerConfig config;
    
    // Packet ID generator
    std::atomic<uint64_t> nextPacketId{0};
    std::atomic<uint64_t> nextConnectionId{1};
    
    // Statistics
    uint64_t totalPacketsCaptured;
    uint64_t totalConnectionsEstablished;
    uint64_t totalConnectionsClosed;
    uint64_t totalBytesTransferred;
    double totalDuration;
    
    Impl() : packetCaptureActive(false), connectionMonitorActive(false),
             performanceMonitorActive(false), packetCaptureInterval(100),
             connectionMonitorInterval(1000), performanceMonitorInterval(500),
             totalPacketsCaptured(0), totalConnectionsEstablished(0),
             totalConnectionsClosed(0), totalBytesTransferred(0), totalDuration(0.0) {}
    
    ~Impl() {
        if (packetCaptureActive) {
            packetCaptureActive = false;
            if (packetCaptureThread.joinable()) {
                packetCaptureThread.join();
            }
        }
        
        if (connectionMonitorActive) {
            connectionMonitorActive = false;
            if (connectionMonitorThread.joinable()) {
                connectionMonitorThread.join();
            }
        }
        
        if (performanceMonitorActive) {
            performanceMonitorActive = false;
            if (performanceMonitorThread.joinable()) {
                performanceMonitorThread.join();
            }
        }
    }
    
    void startPacketCapture() {
        packetCaptureActive = true;
        packetCaptureThread = std::thread([this]() {
            while (packetCaptureActive) {
                capturePackets();
                std::this_thread::sleep_for(packetCaptureInterval);
            }
        });
    }
    
    void startConnectionMonitoring() {
        connectionMonitorActive = true;
        connectionMonitorThread = std::thread([this]() {
            while (connectionMonitorActive) {
                monitorConnections();
                std::this_thread::sleep_for(connectionMonitorInterval);
            }
        });
    }
    
    void startPerformanceMonitoring() {
        performanceMonitorActive = true;
        performanceMonitorThread = std::thread([this]() {
            while (performanceMonitorActive) {
                monitorPerformance();
                std::this_thread::sleep_for(performanceMonitorInterval);
            }
        });
    }
    
    void capturePackets() {
        // Simplified packet capture - in real implementation would use system APIs
        // or packet capture libraries
        NetworkPacket packet;
        packet.packetId = ++nextPacketId;
        packet.timestamp = std::chrono::high_resolution_clock::now();
        packet.protocol = NetworkProtocol::TCP;
        packet.connectionType = ConnectionType::CLIENT;
        packet.sourceAddress = "192.168.1.100";
        packet.destinationAddress = "10.0.0.1";
        packet.sourcePort = 12345;
        packet.destinationPort = 8080;
        packet.packetSize = 1500 + (rand() % 1500);
        packet.latency = static_cast<double>(rand()) / RAND_MAX * 100.0;
        packet.jitter = static_cast<double>(rand()) / RAND_MAX * 10.0;
        packet.operation = "GET";
        packet.endpoint = "/api/data";
        packet.packetSequenceNumber = rand() % 1000;
        packet.isAck = (rand() % 10 == 0);
        packet.isData = !packet.isAck;
        packet.isControl = false;
        
        recordPacket(packet);
    }
    
    void monitorConnections() {
        std::lock_guard<std::mutex> lock(connectionsMutex);
        
        auto now = std::chrono::high_resolution_clock::now();
        for (auto& pair : connections) {
            auto& connection = pair.second;
            
            // Check for timeout
            double duration = std::chrono::duration<double>(now - connection.lastActivityTime).count();
            if (duration > config.connectionTimeout && connection.isActive) {
                connection.isActive = false;
                connection.closeTime = now;
                totalConnectionsClosed++;
                
                // Record connection error
                NetworkError error;
                error.timestamp = now;
                error.errorType = "timeout";
                error.sourceAddress = connection.localAddress;
                error.destinationAddress = connection.remoteAddress;
                error.protocol = connection.protocol;
                error.errorMessage = "Connection timed out after " + std::to_string(config.connectionTimeout) + " seconds";
                error.bytesTransferred = connection.totalBytesTransferred;
                error.duration = duration;
                error.isRecoverable = false;
                error.errorCode = 0;
                
                recordError(error);
            }
            
            // Update throughput
            if (connection.lastActivityTime.time_since_epoch().count() > 0) {
                double connDuration = std::chrono::duration<double>(now - connection.lastActivityTime).count();
                if (connDuration > 0) {
                    connection.bytesPerSecond = connection.totalBytesTransferred / connDuration;
                    connection.packetsInFlight = static_cast<uint64_t>(connection.bytesPerSecond / 1500.0); // approximate
                }
            }
        }
    }
    
    void monitorPerformance() {
        // Calculate overall performance metrics
        NetworkThroughput throughput;
        throughput.timestamp = std::chrono::high_resolution_clock::now();
        
        // Calculate overall throughput based on all packets
        std::lock_guard<std::mutex> lock(packetsMutex);
        double totalReadSpeed = 0.0;
        double totalWriteSpeed = 0.0;
        uint64_t totalReadBytes = 0;
        uint64_t totalWriteBytes = 0;
        
        for (const auto& packet : packets) {
            if (packet.isData) {
                totalReadBytes += packet.packetSize;
                totalReadSpeed += packet.packetSize;
            } else {
                totalWriteBytes += packet.packetSize;
                totalWriteSpeed += packet.packetSize;
            }
        }
        
        // Calculate speeds (simplified - would use actual timing)
        double sampleInterval = std::chrono::duration<double>(performanceMonitorInterval).count();
        if (sampleInterval > 0) {
            throughput.readSpeed = totalReadBytes / sampleInterval;
            throughput.writeSpeed = totalWriteBytes / sampleInterval;
            throughput.throughput = throughput.readSpeed + throughput.writeSpeed;
            throughput.readBytes = totalReadBytes;
            throughput.writeBytes = totalWriteBytes;
        }
        
        recordThroughput(throughput);
        
        // Clean up old data
        cleanupOldData();
    }
    
    void cleanupOldData() {
        auto now = std::chrono::high_resolution_clock::now();
        
        // Remove old packets (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(packetsMutex);
            auto threshold = now - std::chrono::hours(1);
            packets.erase(std::remove_if(packets.begin(), packets.end(),
                                       [threshold](const NetworkPacket& p) {
                                           return p.timestamp < threshold;
                                       }),
                         packets.end());
        }
        
        // Close inactive connections (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(connectionsMutex);
            auto threshold = now - std::chrono::hours(1);
            for (auto it = connections.begin(); it != connections.end();) {
                if (!it->second.isActive && it->second.closeTime.time_since_epoch().count() > 0) {
                    auto closeThreshold = now - std::chrono::hours(24);
                    if (it->second.closeTime < closeThreshold) {
                        it = connections.erase(it);
                        continue;
                    }
                }
                ++it;
            }
        }
        
        // Remove old throughput samples (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(throughputsMutex);
            auto threshold = now - std::chrono::hours(1);
            throughputs.erase(std::remove_if(throughputs.begin(), throughputs.end(),
                                           [threshold](const NetworkThroughput& t) {
                                               return t.timestamp < threshold;
                                           }),
                            throughputs.end());
        }
        
        // Remove old congestion events (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(congestionsMutex);
            auto threshold = now - std::chrono::hours(1);
            congestions.erase(std::remove_if(congestions.begin(), congestions.end(),
                                           [threshold](const NetworkCongestion& c) {
                                               return c.timestamp < threshold;
                                           }),
                            congestions.end());
        }
        
        // Remove old errors (older than 1 hour)
        {
            std::lock_guard<std::mutex> lock(errorsMutex);
            auto threshold = now - std::chrono::hours(1);
            errors.erase(std::remove_if(errors.begin(), errors.end(),
                                       [threshold](const NetworkError& e) {
                                           return e.timestamp < threshold;
                                       }),
                        errors.end());
        }
    }
    
    double calculateThroughput(const std::vector<NetworkPacket>& packets) const {
        if (packets.empty()) return 0.0;
        
        // Calculate average throughput based on packet sizes and timestamps
        double totalBytes = 0.0;
        double totalTime = 0.0;
        
        for (size_t i = 1; i < packets.size(); ++i) {
            totalBytes += packets[i].packetSize;
            double timeDiff = std::chrono::duration<double>(packets[i].timestamp - packets[i-1].timestamp).count();
            totalTime += timeDiff;
        }
        
        if (totalTime == 0.0) return 0.0;
        return totalBytes / totalTime;
    }
    
    NetworkCongestion detectCongestion(const std::vector<NetworkPacket>& packets) const {
        NetworkCongestion congestion;
        
        if (packets.empty()) return congestion;
        
        // Simple congestion detection based on packet loss and latency
        double totalPackets = 0.0;
        double lostPackets = 0.0;
        double totalLatency = 0.0;
        
        for (const auto& packet : packets) {
            totalPackets++;
            totalLatency += packet.latency;
            
            // Detect packet loss (simplified heuristic)
            if (packet.isData && packet.packetSequenceNumber % 10 == 0) {
                lostPackets++;
            }
        }
        
        if (totalPackets > 0) {
            congestion.packetLossRate = lostPackets / totalPackets;
            congestion.detectedLatency = totalLatency / totalPackets;
            congestion.averageRTT = congestion.detectedLatency * 2.0; // approximate RTT
            congestion.jitter = 0.0; // would need more complex calculation
            
            // Determine if congestion is significant
            if (congestion.packetLossRate > config.congestionDetectionThreshold) {
                congestion.severity = congestion.packetLossRate / config.congestionDetectionThreshold;
                congestion.congestionCause = "packet_loss";
                congestion.isResolved = false;
            } else {
                congestion.severity = 0.0;
                congestion.isResolved = true;
            }
        }
        
        return congestion;
    }
    
    NetworkError detectError(const NetworkPacket& packet) const {
        NetworkError error;
        error.timestamp = packet.timestamp;
        
        // Simple error detection based on packet characteristics
        if (packet.packetSize == 0) {
            error.errorType = "zero_size_packet";
            error.errorMessage = "Packet size is zero";
            error.isRecoverable = false;
            error.errorCode = 1;
        } else if (packet.latency > 1000.0) { // 1 second threshold
            error.errorType = "high_latency";
            error.errorMessage = "Packet latency exceeds threshold";
            error.isRecoverable = true;
            error.errorCode = 2;
        } else {
            return error; // No error detected
        }
        
        error.sourceAddress = packet.sourceAddress;
        error.destinationAddress = packet.destinationAddress;
        error.protocol = packet.protocol;
        error.bytesTransferred = packet.packetSize;
        error.duration = 0.0; // Would need connection duration
        
        return error;
    }
};

NetworkIOProfiler::NetworkIOProfiler() : pImpl(std::make_unique<Impl>()) {
    initialize();
}

NetworkIOProfiler::~NetworkIOProfiler() {
    shutdown();
}

bool NetworkIOProfiler::initialize() {
    try {
        // Configure based on settings
        pImpl->config = config_;
        
        // Start monitoring threads if enabled
        if (config_.enablePacketCapture) {
            pImpl->startPacketCapture();
        }
        
        if (config_.enableConnectionTracking) {
            pImpl->startConnectionMonitoring();
        }
        
        if (config_.enableThroughputMonitoring) {
            pImpl->startPerformanceMonitoring();
        }
        
        NLM_LOG_INFO("Network I/O profiler initialized successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to initialize network I/O profiler: ") + e.what());
        return false;
    }
}

void NetworkIOProfiler::shutdown() {
    try {
        // Stop all monitoring threads
        pImpl->packetCaptureActive = false;
        pImpl->connectionMonitorActive = false;
        pImpl->performanceMonitorActive = false;
        
        // Join threads
        if (pImpl->packetCaptureThread.joinable()) {
            pImpl->packetCaptureThread.join();
        }
        
        if (pImpl->connectionMonitorThread.joinable()) {
            pImpl->connectionMonitorThread.join();
        }
        
        if (pImpl->performanceMonitorThread.joinable()) {
            pImpl->performanceMonitorThread.join();
        }
        
        NLM_LOG_INFO("Network I/O profiler shutdown complete");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error during network I/O profiler shutdown: ") + e.what());
    }
}

void NetworkIOProfiler::recordPacket(const NetworkPacket& packet) {
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    pImpl->packets.push_back(packet);
    
    // Update statistics
    pImpl->totalPacketsCaptured++;
    pImpl->totalBytesTransferred += packet.packetSize;
    
    // Detect and record errors
    NetworkError error = pImpl->detectError(packet);
    if (!error.errorMessage.empty()) {
        recordError(error);
    }
}

uint64_t NetworkIOProfiler::createConnection(const NetworkConnection& connection) {
    std::lock_guard<std::mutex> lock(pImpl->connectionsMutex);
    uint64_t connectionId = ++pImpl->nextConnectionId;
    
    NetworkConnection conn = connection;
    conn.connectionId = connectionId;
    conn.establishTime = std::chrono::high_resolution_clock::now();
    conn.isActive = true;
    
    pImpl->connections[connectionId] = conn;
    pImpl->totalConnectionsEstablished++;
    
    return connectionId;
}

void NetworkIOProfiler::updateConnection(uint64_t connectionId, const std::string& activity) {
    std::lock_guard<std::mutex> lock(pImpl->connectionsMutex);
    auto it = pImpl->connections.find(connectionId);
    if (it != pImpl->connections.end()) {
        it->second.lastActivityTime = std::chrono::high_resolution_clock::now();
        it->second.totalBytesTransferred += 1500; // approximate bytes per activity
    }
}

void NetworkIOProfiler::closeConnection(uint64_t connectionId) {
    std::lock_guard<std::mutex> lock(pImpl->connectionsMutex);
    auto it = pImpl->connections.find(connectionId);
    if (it != pImpl->connections.end()) {
        it->second.isActive = false;
        it->second.closeTime = std::chrono::high_resolution_clock::now();
        pImpl->totalConnectionsClosed++;
    }
}

void NetworkIOProfiler::recordThroughput(const NetworkThroughput& throughput) {
    std::lock_guard<std::mutex> lock(pImpl->throughputsMutex);
    pImpl->throughputs.push_back(throughput);
}

void NetworkIOProfiler::recordCongestion(const NetworkCongestion& congestion) {
    std::lock_guard<std::mutex> lock(pImpl->congestionsMutex);
    pImpl->congestions.push_back(congestion);
}

void NetworkIOProfiler::recordError(const NetworkError& error) {
    std::lock_guard<std::mutex> lock(pImpl->errorsMutex);
    pImpl->errors.push_back(error);
}

std::vector<NetworkPacket> NetworkIOProfiler::getPackets() const {
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    return pImpl->packets;
}

std::vector<NetworkConnection> NetworkIOProfiler::getConnections(bool activeOnly) const {
    std::vector<NetworkConnection> connections;
    
    std::lock_guard<std::mutex> lock(pImpl->connectionsMutex);
    for (const auto& pair : pImpl->connections) {
        if (!activeOnly || pair.second.isActive) {
            connections.push_back(pair.second);
        }
    }
    
    return connections;
}

NetworkConnection NetworkIOProfiler::getConnection(uint64_t connectionId) const {
    std::lock_guard<std::mutex> lock(pImpl->connectionsMutex);
    auto it = pImpl->connections.find(connectionId);
    if (it != pImpl->connections.end()) {
        return it->second;
    }
    
    return NetworkConnection();
}

std::vector<NetworkThroughput> NetworkIOProfiler::getThroughput() const {
    std::lock_guard<std::mutex> lock(pImpl->throughputsMutex);
    return pImpl->throughputs;
}

std::vector<NetworkCongestion> NetworkIOProfiler::getCongestionEvents() const {
    std::lock_guard<std::mutex> lock(pImpl->congestionsMutex);
    return pImpl->congestions;
}

std::vector<NetworkError> NetworkIOProfiler::getErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->errorsMutex);
    return pImpl->errors;
}

std::unordered_map<std::string, double> NetworkIOProfiler::analyzeNetworkPerformance() const {
    std::unordered_map<std::string, double> metrics;
    
    // Calculate performance metrics based on collected data
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    
    if (!pImpl->packets.empty()) {
        double totalBytes = 0.0;
        double totalPackets = 0.0;
        double totalLatency = 0.0;
        
        for (const auto& packet : pImpl->packets) {
            totalBytes += packet.packetSize;
            totalPackets++;
            totalLatency += packet.latency;
        }
        
        metrics["average_packet_size"] = totalBytes / totalPackets;
        metrics["total_bytes_transferred"] = totalBytes;
        metrics["total_packets_transferred"] = totalPackets;
        metrics["average_latency_ms"] = totalLatency / totalPackets;
        
        // Calculate throughput
        double timeSpan = std::chrono::duration<double>(
            pImpl->packets.back().timestamp - pImpl->packets.front().timestamp).count();
        
        if (timeSpan > 0) {
            metrics["overall_throughput_bps"] = totalBytes / timeSpan;
            metrics["overall_throughput_mbps"] = (totalBytes / timeSpan) / (1024.0 * 1024.0);
        }
    }
    
    return metrics;
}

std::string NetworkIOProfiler::generateThroughputReport() const {
    std::ostringstream oss;
    oss << "=== NLM Network Throughput Report ===\n\n";
    oss << "Network Performance Statistics:\n";
    oss << "  Total Packets Captured: " << pImpl->totalPacketsCaptured << "\n";
    oss << "  Total Bytes Transferred: " << pImpl->totalBytesTransferred << "\n";
    oss << "  Total Connections Established: " << pImpl->totalConnectionsEstablished << "\n";
    oss << "  Total Connections Closed: " << pImpl->totalConnectionsClosed << "\n";
    oss << "  Current Active Connections: " << getConnections(true).size() << "\n\n";
    
    // Generate throughput report based on samples
    std::lock_guard<std::mutex> lock(pImpl->throughputsMutex);
    if (!pImpl->throughputs.empty()) {
        oss << "Recent Throughput Samples:\n";
        for (size_t i = 0; i < std::min(size_t(10), pImpl->throughputs.size()); ++i) {
            const auto& throughput = pImpl->throughputs[pImpl->throughputs.size() - 1 - i];
            oss << "  Sample " << (i + 1) << ": "
                << (throughput.throughput / (1024.0 * 1024.0)) << " Mbps "
                << "(Read: " << (throughput.readSpeed / (1024.0 * 1024.0)) << " Mbps, "
                << "Write: " << (throughput.writeSpeed / (1024.0 * 1024.0)) << " Mbps)\n";
        }
    }
    
    return oss.str();
}

std::string NetworkIOProfiler::generateCongestionReport() const {
    std::ostringstream oss;
    oss << "=== NLM Network Congestion Report ===\n\n";
    
    std::lock_guard<std::mutex> lock(pImpl->congestionsMutex);
    
    if (pImpl->congestions.empty()) {
        oss << "No congestion events detected.\n";
        return oss.str();
    }
    
    oss << "Congestion Events Detected:\n";
    for (size_t i = 0; i < pImpl->congestions.size(); ++i) {
        const auto& congestion = pImpl->congestions[i];
        oss << "  Event " << (i + 1) << ": "
            << congestion.congestionCause << " (Severity: " << congestion.severity << ")\n"
            << "    Source: " << congestion.sourceAddress << " -> " << congestion.destinationAddress << "\n"
            << "    Latency: " << congestion.detectedLatency << " ms\n"
            << "    Packet Loss Rate: " << (congestion.packetLossRate * 100.0) << "%\n"
            << "    Timestamp: " << congestion.timestamp.time_since_epoch().count() << "\n"
            << "    Resolved: " << (congestion.isResolved ? "Yes" : "No") << "\n\n";
    }
    
    return oss.str();
}

std::string NetworkIOProfiler::generateErrorReport() const {
    std::ostringstream oss;
    oss << "=== NLM Network Error Report ===\n\n";
    
    std::lock_guard<std::mutex> lock(pImpl->errorsMutex);
    
    if (pImpl->errors.empty()) {
        oss << "No network errors detected.\n";
        return oss.str();
    }
    
    oss << "Network Errors Detected:\n";
    for (size_t i = 0; i < pImpl->errors.size(); ++i) {
        const auto& error = pImpl->errors[i];
        oss << "  Error " << (i + 1) << ": " << error.errorType << "\n"
            << "    Message: " << error.errorMessage << "\n"
            << "    Source: " << error.sourceAddress << " -> " << error.destinationAddress << "\n"
            << "    Protocol: " << static_cast<int>(error.protocol) << "\n"
            << "    Bytes Transferred: " << error.bytesTransferred << "\n"
            << "    Duration: " << error.duration << " seconds\n"
            << "    Recoverable: " << (error.isRecoverable ? "Yes" : "No") << "\n"
            << "    Timestamp: " << error.timestamp.time_since_epoch().count() << "\n\n";
    }
    
    return oss.str();
}

std::string NetworkIOProfiler::generateConnectionReport() const {
    std::ostringstream oss;
    oss << "=== NLM Network Connection Report ===\n\n";
    
    auto connections = getConnections(true);
    oss << "Active Connections: " << connections.size() << "\n";
    
    if (connections.empty()) {
        oss << "No active connections.\n";
        return oss.str();
    }
    
    oss << "Connection Details:\n";
    for (size_t i = 0; i < connections.size() && i < 10; ++i) {
        const auto& conn = connections[i];
        oss << "  Connection " << (i + 1) << ": "
            << conn.localAddress << ":" << conn.localPort << " -> "
            << conn.remoteAddress << ":" << conn.remotePort << "\n"
            << "    Protocol: " << static_cast<int>(conn.protocol) << "\n"
            << "    Connection Type: " << static_cast<int>(conn.connectionType) << "\n"
            << "    Bytes Transferred: " << conn.totalBytesTransferred << "\n"
            << "    Average Latency: " << conn.averageLatency << " ms\n"
            << "    Packets In Flight: " << conn.packetsInFlight << "\n"
            << "    Established: " << conn.establishTime.time_since_epoch().count() << "\n\n";
    }
    
    return oss.str();
}

std::string NetworkIOProfiler::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    oss << "  \"config\": {\n";
    oss << "    \"enablePacketCapture\": " << (config_.enablePacketCapture ? "true" : "false") << ",\n";
    oss << "    \"enableConnectionTracking\": " << (config_.enableConnectionTracking ? "true" : "false") << ",\n";
    oss << "    \"enableThroughputMonitoring\": " << (config_.enableThroughputMonitoring ? "true" : "false") << ",\n";
    oss << "    \"enableCongestionDetection\": " << (config_.enableCongestionDetection ? "true" : "false") << ",\n";
    oss << "    \"enableErrorTracking\": " << (config_.enableErrorTracking ? "true" : "false") << ",\n";
    oss << "    \"enableLatencyMeasurement\": " << (config_.enableLatencyMeasurement ? "true" : "false") << ",\n";
    oss << "    \"enableJitterMeasurement\": " << (config_.enableJitterMeasurement ? "true" : "false") << "\n";
    oss << "  },\n";
    
    oss << "  \"statistics\": {\n";
    oss << "    \"totalPacketsCaptured\": " << pImpl->totalPacketsCaptured << ",\n";
    oss << "    \"totalConnectionsEstablished\": " << pImpl->totalConnectionsEstablished << ",\n";
    oss << "    \"totalConnectionsClosed\": " << pImpl->totalConnectionsClosed << ",\n";
    oss << "    \"totalBytesTransferred\": " << pImpl->totalBytesTransferred << ",\n";
    oss << "    \"totalDuration\": " << pImpl->totalDuration << "\n";
    oss << "  },\n";
    
    // Export packets
    oss << "  \"packets\": [\n";
    bool firstPacket = true;
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    for (const auto& packet : pImpl->packets) {
        if (!firstPacket) oss << ",\n";
        firstPacket = false;
        
        oss << "    {\n";
        oss << "      \"packetId\": " << packet.packetId << ",\n";
        oss << "      \"timestamp\": \"" << packet.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"protocol\": " << static_cast<int>(packet.protocol) << ",\n";
        oss << "      \"connectionType\": " << static_cast<int>(packet.connectionType) << ",\n";
        oss << "      \"sourceAddress\": \"" << packet.sourceAddress << "\",\n";
        oss << "      \"destinationAddress\": \"" << packet.destinationAddress << "\",\n";
        oss << "      \"sourcePort\": " << packet.sourcePort << ",\n";
        oss << "      \"destinationPort\": " << packet.destinationPort << ",\n";
        oss << "      \"packetSize\": " << packet.packetSize << ",\n";
        oss << "      \"latency\": " << packet.latency << ",\n";
        oss << "      \"jitter\": " << packet.jitter << ",\n";
        oss << "      \"operation\": \"" << packet.operation << "\",\n";
        oss << "      \"endpoint\": \"" << packet.endpoint << "\",\n";
        oss << "      \"packetSequenceNumber\": " << packet.packetSequenceNumber << ",\n";
        oss << "      \"isAck\": " << (packet.isAck ? "true" : "false") << ",\n";
        oss << "      \"isData\": " << (packet.isData ? "true" : "false") << ",\n";
        oss << "      \"isControl\": " << (packet.isControl ? "true" : "false") << "\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export connections
    oss << "  \"connections\": [\n";
    bool firstConnection = true;
    std::lock_guard<std::mutex> lock2(pImpl->connectionsMutex);
    for (const auto& pair : pImpl->connections) {
        if (!firstConnection) oss << ",\n";
        firstConnection = false;
        
        const auto& conn = pair.second;
        oss << "    {\n";
        oss << "      \"connectionId\": " << conn.connectionId << ",\n";
        oss << "      \"establishTime\": \"" << conn.establishTime.time_since_epoch().count() << "\",\n";
        oss << "      \"lastActivityTime\": \"" << conn.lastActivityTime.time_since_epoch().count() << "\",\n";
        oss << "      \"closeTime\": \"" << (conn.closeTime.time_since_epoch().count() == 0 ? "null" : std::to_string(conn.closeTime.time_since_epoch().count())) << "\",\n";
        oss << "      \"protocol\": " << static_cast<int>(conn.protocol) << ",\n";
        oss << "      \"connectionType\": " << static_cast<int>(conn.connectionType) << ",\n";
        oss << "      \"localAddress\": \"" << conn.localAddress << "\",\n";
        oss << "      \"localPort\": " << conn.localPort << ",\n";
        oss << "      \"remoteAddress\": \"" << conn.remoteAddress << "\",\n";
        oss << "      \"remotePort\": " << conn.remotePort << ",\n";
        oss << "      \"endpoint\": \"" << conn.endpoint << "\",\n";
        oss << "      \"totalBytesTransferred\": " << conn.totalBytesTransferred << ",\n";
        oss << "      \"totalPacketsTransferred\": " << conn.totalPacketsTransferred << ",\n";
        oss << "      \"bytesPerSecond\": " << conn.bytesPerSecond << ",\n";
        oss << "      \"averageLatency\": " << conn.averageLatency << ",\n";
        oss << "      \"latencyVariation\": " << conn.latencyVariation << ",\n";
        oss << "      \"packetsInFlight\": " << conn.packetsInFlight << ",\n";
        oss << "      \"isActive\": " << (conn.isActive ? "true" : "false") << "\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export throughput samples
    oss << "  \"throughputs\": [\n";
    bool firstThroughput = true;
    std::lock_guard<std::mutex> lock3(pImpl->throughputsMutex);
    for (const auto& throughput : pImpl->throughputs) {
        if (!firstThroughput) oss << ",\n";
        firstThroughput = false;
        
        oss << "    {\n";
        oss << "      \"timestamp\": \"" << throughput.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"readSpeed\": " << throughput.readSpeed << ",\n";
        oss << "      \"writeSpeed\": " << throughput.writeSpeed << ",\n";
        oss << "      \"throughput\": " << throughput.throughput << ",\n";
        oss << "      \"readBytes\": " << throughput.readBytes << ",\n";
        oss << "      \"writeBytes\": " << throughput.writeBytes << ",\n";
        oss << "      \"readPackets\": " << throughput.readPackets << ",\n";
        oss << "      \"writePackets\": " << throughput.writePackets << ",\n";
        oss << "      \"readUtilization\": " << throughput.readUtilization << ",\n";
        oss << "      \"writeUtilization\": " << throughput.writeUtilization << ",\n";
        oss << "      \"interface\": \"" << throughput.interface << "\"\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export congestion events
    oss << "  \"congestionEvents\": [\n";
    bool firstCongestion = true;
    std::lock_guard<std::mutex> lock4(pImpl->congestionsMutex);
    for (const auto& congestion : pImpl->congestions) {
        if (!firstCongestion) oss << ",\n";
        firstCongestion = false;
        
        oss << "    {\n";
        oss << "      \"timestamp\": \"" << congestion.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"sourceAddress\": \"" << congestion.sourceAddress << "\",\n";
        oss << "      \"destinationAddress\": \"" << congestion.destinationAddress << "\",\n";
        oss << "      \"protocol\": " << static_cast<int>(congestion.protocol) << ",\n";
        oss << "      \"detectedLatency\": " << congestion.detectedLatency << ",\n";
        oss << "      \"receivedPacketLoss\": " << congestion.receivedPacketLoss << ",\n";
        oss << "      \"totalPacketsSent\": " << congestion.totalPacketsSent << ",\n";
        oss << "      \"packetLossRate\": " << congestion.packetLossRate << ",\n";
        oss << "      \"averageRTT\": " << congestion.averageRTT << ",\n";
        oss << "      \"jitter\": " << congestion.jitter << ",\n";
        oss << "      \"congestionCause\": \"" << congestion.congestionCause << "\",\n";
        oss << "      \"severity\": " << congestion.severity << ",\n";
        oss << "      \"isResolved\": " << (congestion.isResolved ? "true" : "false") << "\n";
        oss << "    }";
    }
    oss << "\n  ],\n";
    
    // Export errors
    oss << "  \"errors\": [\n";
    bool firstError = true;
    std::lock_guard<std::mutex> lock5(pImpl->errorsMutex);
    for (const auto& error : pImpl->errors) {
        if (!firstError) oss << ",\n";
        firstError = false;
        
        oss << "    {\n";
        oss << "      \"timestamp\": \"" << error.timestamp.time_since_epoch().count() << "\",\n";
        oss << "      \"errorType\": \"" << error.errorType << "\",\n";
        oss << "      \"sourceAddress\": \"" << error.sourceAddress << "\",\n";
        oss << "      \"destinationAddress\": \"" << error.destinationAddress << "\",\n";
        oss << "      \"protocol\": " << static_cast<int>(error.protocol) << ",\n";
        oss << "      \"errorMessage\": \"" << error.errorMessage << "\",\n";
        oss << "      \"bytesTransferred\": " << error.bytesTransferred << ",\n";
        oss << "      \"duration\": " << error.duration << ",\n";
        oss << "      \"isRecoverable\": " << (error.isRecoverable ? "true" : "false") << ",\n";
        oss << "      \"errorCode\": " << error.errorCode << ",\n";
        oss << "      \"stackTrace\": \"" << error.stackTrace << "\",\n";
        oss << "    }";
    }
    oss << "\n  ]\n";
    
    oss << "}\n";
    
    return oss.str();
}

std::string NetworkIOProfiler::exportToCSV() const {
    std::ostringstream oss;
    oss << "PacketID,Timestamp,SourceAddress,DestinationAddress,Protocol,PacketSize,Latency,Operation,Endpoint\n";
    
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    for (const auto& packet : pImpl->packets) {
        oss << packet.packetId << ","
            << packet.timestamp.time_since_epoch().count() << ","
            << packet.sourceAddress << ","
            << packet.destinationAddress << ","
            << static_cast<int>(packet.protocol) << ","
            << packet.packetSize << ","
            << packet.latency << ","
            << packet.operation << ","
            << packet.endpoint << "\n";
    }
    
    return oss.str();
}

std::string NetworkIOProfiler::generateReport() const {
    std::ostringstream oss;
    oss << "=== NLM Network I/O Profiler Report ===\n\n";
    oss << "Network Profiling Statistics:\n";
    oss << "  Total Packets Captured: " << pImpl->totalPacketsCaptured << "\n";
    oss << "  Total Bytes Transferred: " << pImpl->totalBytesTransferred << "\n";
    oss << "  Total Connections Established: " << pImpl->totalConnectionsEstablished << "\n";
    oss << "  Total Connections Closed: " << pImpl->totalConnectionsClosed << "\n";
    oss << "  Current Active Connections: " << getConnections(true).size() << "\n\n";
    
    // Generate performance metrics
    auto metrics = analyzeNetworkPerformance();
    oss << "Performance Metrics:\n";
    for (const auto& pair : metrics) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    oss << "\n";
    
    // Add throughput report
    oss << generateThroughputReport();
    oss << "\n";
    
    // Add congestion report
    oss << generateCongestionReport();
    oss << "\n";
    
    // Add error report
    oss << generateErrorReport();
    oss << "\n";
    
    // Add connection report
    oss << generateConnectionReport();
    
    return oss.str();
}

void NetworkIOProfiler::reset() {
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    pImpl->packets.clear();
    
    std::lock_guard<std::mutex> lock2(pImpl->connectionsMutex);
    pImpl->connections.clear();
    pImpl->totalConnectionsEstablished = 0;
    pImpl->totalConnectionsClosed = 0;
    
    std::lock_guard<std::mutex> lock3(pImpl->throughputsMutex);
    pImpl->throughputs.clear();
    
    std::lock_guard<std::mutex> lock4(pImpl->congestionsMutex);
    pImpl->congestions.clear();
    
    std::lock_guard<std::mutex> lock5(pImpl->errorsMutex);
    pImpl->errors.clear();
}

NetworkIOProfiler& NetworkIOProfiler::getInstance() {
    static NetworkIOProfiler instance;
    return instance;
}

std::vector<std::pair<std::string, uint64_t>> NetworkIOProfiler::getTopEndpoints(size_t limit) const {
    std::unordered_map<std::string, uint64_t> endpointTraffic;
    
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    for (const auto& packet : pImpl->packets) {
        endpointTraffic[packet.endpoint] += packet.packetSize;
    }
    
    // Convert to vector and sort
    std::vector<std::pair<std::string, uint64_t>> topEndpoints;
    for (const auto& pair : endpointTraffic) {
        topEndpoints.push_back(pair);
    }
    
    std::sort(topEndpoints.begin(), topEndpoints.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    if (topEndpoints.size() > limit) {
        topEndpoints.resize(limit);
    }
    
    return topEndpoints;
}

std::unordered_map<std::string, uint64_t> NetworkIOProfiler::getTrafficByProtocol() const {
    std::unordered_map<std::string, uint64_t> protocolTraffic;
    
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    for (const auto& packet : pImpl->packets) {
        std::string protocolStr;
        switch (packet.protocol) {
            case NetworkProtocol::TCP: protocolStr = "TCP"; break;
            case NetworkProtocol::UDP: protocolStr = "UDP"; break;
            case NetworkProtocol::HTTP: protocolStr = "HTTP"; break;
            case NetworkProtocol::HTTPS: protocolStr = "HTTPS"; break;
            case NetworkProtocol::WEBSOCKET: protocolStr = "WEBSOCKET"; break;
            case NetworkProtocol::GRPC: protocolStr = "GRPC"; break;
            case NetworkProtocol::FTP: protocolStr = "FTP"; break;
            case NetworkProtocol::SFTP: protocolStr = "SFTP"; break;
            case NetworkProtocol::NFS: protocolStr = "NFS"; break;
            case NetworkProtocol::SMB: protocolStr = "SMB"; break;
            case NetworkProtocol::MQTT: protocolStr = "MQTT"; break;
            case NetworkProtocol::COAP: protocolStr = "COAP"; break;
            default: protocolStr = "UNKNOWN";
        }
        
        protocolTraffic[protocolStr] += packet.packetSize;
    }
    
    return protocolTraffic;
}

std::unordered_map<std::string, uint64_t> NetworkIOProfiler::getTrafficByConnectionType() const {
    std::unordered_map<std::string, uint64_t> connectionTypeTraffic;
    
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    for (const auto& packet : pImpl->packets) {
        std::string connectionTypeStr;
        switch (packet.connectionType) {
            case ConnectionType::CLIENT: connectionTypeStr = "CLIENT"; break;
            case ConnectionType::SERVER: connectionTypeStr = "SERVER"; break;
            case ConnectionType::PEERING: connectionTypeStr = "PEERING"; break;
            case ConnectionType::REPLICATION: connectionTypeStr = "REPLICATION"; break;
            case ConnectionType::BACKUP: connectionTypeStr = "BACKUP"; break;
            case ConnectionType::STREAMING: connectionTypeStr = "STREAMING"; break;
            case ConnectionType::REAL_TIME: connectionTypeStr = "REAL_TIME"; break;
            default: connectionTypeStr = "UNKNOWN";
        }
        
        connectionTypeTraffic[connectionTypeStr] += packet.packetSize;
    }
    
    return connectionTypeTraffic;
}

	double NetworkIOProfiler::getAverageLatency() const {
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    
    if (pImpl->packets.empty()) {
        return 0.0;
    }
    
    double totalLatency = 0.0;
    for (const auto& packet : pImpl->packets) {
        totalLatency += packet.latency;
    }
    
    return totalLatency / pImpl->packets.size();
}

double NetworkIOProfiler::getPacketLossRate() const {
    // Calculate packet loss rate based on packet sequence numbers
    std::lock_guard<std::mutex> lock(pImpl->packetsMutex);
    
    if (pImpl->packets.empty()) {
        return 0.0;
    }
    
    std::set<uint32_t> sequenceNumbers;
    for (const auto& packet : pImpl->packets) {
        if (packet.isData) {
            sequenceNumbers.insert(packet.packetSequenceNumber);
        }
    }
    
    double totalDataPackets = 0.0;
    for (const auto& packet : pImpl->packets) {
        if (packet.isData) {
            totalDataPackets++;
        }
    }
    
    if (totalDataPackets == 0) {
        return 0.0;
    }
    
    double estimatedLosses = totalDataPackets - sequenceNumbers.size();
    return estimatedLosses / totalDataPackets;
}

std::unordered_map<std::string, double> NetworkIOProfiler::getThroughputByInterface() const {
    std::unordered_map<std::string, double> throughputByInterface;
    
    // For now, return empty map - would need actual network interface monitoring
    // to provide accurate throughput by interface
    
    return throughputByInterface;
}

} // namespace nlm
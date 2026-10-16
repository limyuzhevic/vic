#pragma once

/**
 * NetworkIOProfiler - Network I/O profiling for experiment data
 * 
 * This class provides detailed network I/O profiling capabilities for:
 * - Network bandwidth tracking and analysis
 * - Latency measurement and monitoring
 * - Throughput analysis
 * - Connection tracking
 * - Network congestion detection
 * - Network optimization recommendations
 * 
 * Designed for profiling experiment data transfer and
 * remote communication in the NLM framework
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <queue>
#include <set>

namespace nlm {

/**
 * Network protocol type
 */
enum class NetworkProtocol {
    TCP,
    UDP,
    HTTP,
    HTTPS,
    WEBSOCKET,
    GRPC,
    FTP,
    SFTP,
    NFS,
    SMB,
    MQTT,
    COAP,
    UNKNOWN
};

/**
 * HTTP method type
 */
enum class HTTPMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS,
    CONNECT,
    TRACE,
    UNKNOWN
};

/**
 * Connection type
 */
enum class ConnectionType {
    CLIENT,
    SERVER,
    PEERING,
    REPLICATION,
    BACKUP,
    STREAMING,
    REAL_TIME
};

/**
 * Network packet record
 */
struct NetworkPacket {
    uint64_t packetId;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    NetworkProtocol protocol;
    ConnectionType connectionType;
    std::string sourceAddress;
    std::string destinationAddress;
    uint64_t sourcePort;
    uint64_t destinationPort;
    uint64_t packetSize;
    double latency;  // milliseconds
    double jitter;   // milliseconds
    std::string operation;  // HTTP method or custom operation
    std::string endpoint;   // HTTP URL or endpoint path
    uint32_t packetSequenceNumber;
    bool isAck;      // Acknowledgment packet
    bool isData;     // Data packet
    bool isControl;  // Control packet
    std::unordered_map<std::string, std::string> metadata;
    
    NetworkPacket() : packetId(0), sourcePort(0), destinationPort(0), packetSize(0),
                     packetSequenceNumber(0), isAck(false), isData(false), isControl(false) {}
};

/**
 * Connection record
 */
struct NetworkConnection {
    uint64_t connectionId;
    std::chrono::time_point<std::chrono::high_resolution_clock> establishTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastActivityTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> closeTime;
    NetworkProtocol protocol;
    ConnectionType connectionType;
    std::string localAddress;
    uint64_t localPort;
    std::string remoteAddress;
    uint64_t remotePort;
    std::string endpoint;
    uint64_t totalBytesTransferred;
    uint64_t totalPacketsTransferred;
    uint64_t bytesPerSecond;
    double averageLatency;
    double latencyVariation;
    uint64_t packetsInFlight;
    bool isActive;
    std::string httpMethod;  // For HTTP/HTTPS connections
    std::string httpPath;    // For HTTP/HTTPS connections
    std::string httpStatus;  // For HTTP/HTTPS connections
    std::string userAgent;   // For HTTP/HTTPS connections
    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> queryParameters;
    
    NetworkConnection() : connectionId(0), localPort(0), remotePort(0),
                         totalBytesTransferred(0), totalPacketsTransferred(0),
                         bytesPerSecond(0), averageLatency(0.0), latencyVariation(0.0),
                         packetsInFlight(0), isActive(true) {}
    
    double getConnectionDuration() const {
        auto endTime = closeTime.time_since_epoch().count() == 0 ? 
            std::chrono::high_resolution_clock::now() : closeTime;
        return std::chrono::duration<double>(endTime - establishTime).count();
    }
};

/**
 * Network throughput measurement
 */
struct NetworkThroughput {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    double readSpeed;      // bytes per second
    double writeSpeed;     // bytes per second
    double throughput;     // total throughput
    uint64_t readBytes;    // bytes read
    uint64_t writeBytes;   // bytes written
    uint64_t readPackets;  // packets read
    uint64_t writePackets; // packets written
    double readUtilization;  // link utilization (0-100%)
    double writeUtilization; // link utilization (0-100%)
    std::string interface;  // network interface name
    
    NetworkThroughput() : readSpeed(0.0), writeSpeed(0.0), throughput(0.0),
                         readBytes(0), writeBytes(0), readPackets(0), writePackets(0),
                         readUtilization(0.0), writeUtilization(0.0) {}
};

/**
 * Network congestion event
 */
struct NetworkCongestion {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::string sourceAddress;
    std::string destinationAddress;
    NetworkProtocol protocol;
    double detectedLatency;      // milliseconds
    uint64_t receivedPacketLoss; // packets lost
    uint64_t totalPacketsSent;   // total packets sent
    double packetLossRate;       // fraction of packets lost
    double averageRTT;          // average round-trip time
    double jitter;              // packet delay variation
    std::string congestionCause; // detected cause (e.g., "bufferbloat", "bandwidth", "queue")
    double severity;            // 0.0 to 1.0 severity level
    bool isResolved;
    
    NetworkCongestion() : detectedLatency(0.0), receivedPacketLoss(0), totalPacketsSent(0),
                         packetLossRate(0.0), averageRTT(0.0), jitter(0.0), severity(0.0), isResolved(false) {}
};

/**
 * Network error
 */
struct NetworkError {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::string errorType;         // e.g., "connection_refused", "timeout", "protocol_error"
    std::string sourceAddress;     // For connection errors
    std::string destinationAddress; // For connection errors
    NetworkProtocol protocol;
    std::string errorMessage;
    uint64_t bytesTransferred;    // bytes transferred before error
    double duration;              // connection duration before error (seconds)
    bool isRecoverable;           // whether connection can be recovered
    uint32_t errorCode;           // protocol-specific error code
    std::string stackTrace;       // call stack where error occurred
    std::unordered_map<std::string, std::string> errorContext;
    
    NetworkError() : errorCode(0), bytesTransferred(0), duration(0.0), isRecoverable(false) {}
};

/**
 * Network profiler configuration
 */
struct NetworkProfilerConfig {
    bool enablePacketCapture;           // Enable packet capture
    bool enableConnectionTracking;      // Enable connection tracking
    bool enableThroughputMonitoring;    // Enable throughput monitoring
    bool enableCongestionDetection;     // Enable congestion detection
    bool enableErrorTracking;           // Enable error tracking
    bool enableLatencyMeasurement;      // Enable latency measurement
    bool enableJitterMeasurement;       // Enable jitter measurement
    double packetCaptureInterval;      // packet capture interval (seconds)
    double connectionTimeout;          // connection timeout (seconds)
    double congestionDetectionThreshold;  // threshold for congestion detection
    size_t maxPacketsStored;           // maximum packets to store
    size_t maxConnectionsTracked;      // maximum connections to track
    size_t maxCongestionEvents;        // maximum congestion events to store
    size_t maxErrorsRecorded;          // maximum errors to record
    std::string networkInterface;      // network interface to monitor
    std::string outputDirectory;       // output directory for results
    bool exportToJSON;                 // export results to JSON
    bool exportToCSV;                  // export results to CSV
    
    NetworkProfilerConfig() : enablePacketCapture(true), enableConnectionTracking(true),
                             enableThroughputMonitoring(true), enableCongestionDetection(true),
                             enableErrorTracking(true), enableLatencyMeasurement(true),
                             enableJitterMeasurement(true), packetCaptureInterval(0.1),
                             connectionTimeout(30.0), congestionDetectionThreshold(0.1),
                             maxPacketsStored(100000), maxConnectionsTracked(10000),
                             maxCongestionEvents(1000), maxErrorsRecorded(1000),
                             networkInterface(""), outputDirectory("./network_profiling"),
                             exportToJSON(true), exportToCSV(true) {}
};

/**
 * Network profiler for comprehensive network I/O tracking and analysis
 */
class NetworkIOProfiler {
public:
    NetworkIOProfiler();
    ~NetworkIOProfiler();
    
    // Disable copying
    NetworkIOProfiler(const NetworkIOProfiler&) = delete;
    NetworkIOProfiler& operator=(const NetworkIOProfiler&) = delete;
    
    /**
     * Initialize the network profiler
     */
    bool initialize();
    
    /**
     * Shutdown the network profiler
     */
    void shutdown();
    
    /**
     * Record network packet
     */
    void recordPacket(const NetworkPacket& packet);
    
    /**
     * Create new network connection
     */
    uint64_t createConnection(const NetworkConnection& connection);
    
    /**
     * Update connection activity
     */
    void updateConnection(uint64_t connectionId, const std::string& activity);
    
    /**
     * Close network connection
     */
    void closeConnection(uint64_t connectionId);
    
    /**
     * Record network throughput
     */
    void recordThroughput(const NetworkThroughput& throughput);
    
    /**
     * Record network congestion event
     */
    void recordCongestion(const NetworkCongestion& congestion);
    
    /**
     * Record network error
     */
    void recordError(const NetworkError& error);
    
    /**
     * Get all packets
     */
    std::vector<NetworkPacket> getPackets() const;
    
    /**
     * Get connections by status
     */
    std::vector<NetworkConnection> getConnections(bool activeOnly = true) const;
    
    /**
     * Get connection by ID
     */
    NetworkConnection getConnection(uint64_t connectionId) const;
    
    /**
     * Get network throughput
     */
    std::vector<NetworkThroughput> getThroughput() const;
    
    /**
     * Get congestion events
     */
    std::vector<NetworkCongestion> getCongestionEvents() const;
    
    /**
     * Get network errors
     */
    std::vector<NetworkError> getErrors() const;
    
    /**
     * Analyze network performance
     */
    std::unordered_map<std::string, double> analyzeNetworkPerformance() const;
    
    /**
     * Generate throughput report
     */
    std::string generateThroughputReport() const;
    
    /**
     * Generate congestion report
     */
    std::string generateCongestionReport() const;
    
    /**
     * Generate error report
     */
    std::string generateErrorReport() const;
    
    /**
     * Generate connection report
     */
    std::string generateConnectionReport() const;
    
    /**
     * Export network data to JSON
     */
    std::string exportToJSON() const;
    
    /**
     * Export network data to CSV
     */
    std::string exportToCSV() const;
    
    /**
     * Generate comprehensive network report
     */
    std::string generateReport() const;
    
    /**
     * Reset all network profiling data
     */
    void reset();
    
    /**
     * Get profiler instance (singleton)
     */
    static NetworkIOProfiler& getInstance();
    
    /**
     * Configure network profiler
     */
    void configure(const NetworkProfilerConfig& config) {
        config_ = config;
    }
    
    /**
     * Get current configuration
     */
    NetworkProfilerConfig getConfig() const { return config_; }
    
    /**
     * Get top endpoints by traffic
     */
    std::vector<std::pair<std::string, uint64_t>> getTopEndpoints(size_t limit = 10) const;
    
    /**
     * Get network traffic by protocol
     */
    std::unordered_map<std::string, uint64_t> getTrafficByProtocol() const;
    
    /**
     * Get network traffic by connection type
     */
    std::unordered_map<std::string, uint64_t> getTrafficByConnectionType() const;
    
    /**
     * Get average latency
     */
    double getAverageLatency() const;
    
    /**
     * Get packet loss rate
     */
    double getPacketLossRate() const;
    
    /**
     * Get throughput by interface
     */
    std::unordered_map<std::string, double> getThroughputByInterface() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    NetworkProfilerConfig config_;
    
    /**
     * Clean up old packets and connections
     */
    void cleanupOldData();
    
    /**
     * Calculate throughput based on packets
     */
    double calculateThroughput(const std::vector<NetworkPacket>& packets) const;
    
    /**
     * Detect congestion based on packet data
     */
    NetworkCongestion detectCongestion(const std::vector<NetworkPacket>& packets) const;
    
    /**
     * Detect errors based on packets
     */
    NetworkError detectError(const NetworkPacket& packet) const;
};

} // namespace nlm

#endif // NLM_NETWORK_IO_PROFILER_HPP
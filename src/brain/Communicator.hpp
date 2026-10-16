#pragma once

#include <mpi.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <chrono>

namespace nlm {

// Forward declarations
class DistributedBrain;

// MPI communication tag constants
namespace MPITags {
    constexpr int NEURAL_DATA = 1001;
    constexpr int SPIKE_EVENTS = 1002;
    constexpr int HEARTBEAT = 1003;
    constexpr int NODE_INFO = 1004;
    constexpr int CHECKPOINT_DATA = 1005;
    constexpr int FAULT_REPORT = 1006;
    constexpr int LOAD_BALANCE = 1007;
    constexpr int TERMINATE = 1008;
}

// Communication message structures
struct NeuralDataMessage {
    int sourceNode;
    int messageSize;
    std::vector<float> data;
    uint64_t timestamp;
};

struct SpikeEventMessage {
    int sourceNode;
    std::vector<SpikeEvent> events;
    uint64_t timestamp;
};

struct HeartbeatMessage {
    int nodeId;
    uint64_t timestamp;
    float cpuUsage;
    float memoryUsage;
    int activeNeurons;
};

struct NodeInfoMessage {
    int nodeId;
    std::string host;
    int port;
    bool active;
};

struct CheckpointMessage {
    int sourceNode;
    std::string checkpointPath;
    std::vector<char> data;
    uint64_t timestamp;
};

struct FaultReportMessage {
    int sourceNode;
    int faultType;
    std::string description;
    uint64_t timestamp;
};

// MPI communicator for distributed neural computing
class Communicator {
public:
    // Create communicator with configuration
    explicit Communicator(const DistributedConfig& config);
    
    ~Communicator();
    
    // Disable copying, enable moving
    Communicator(const Communicator&) = delete;
    Communicator& operator=(const Communicator&) = delete;
    Communicator(Communicator&&) noexcept;
    Communicator& operator=(Communicator&&) noexcept;
    
    // Initialize MPI environment
    bool initialize();
    
    // Finalize MPI environment
    void finalize();
    
    // Synchronize all processes
    void barrier() const;
    
    // Broadcast data from root to all processes
    template<typename T>
    void broadcast(T* data, int count, int root) const;
    
    // Gather data from all processes to root
    template<typename T>
    void gather(const T* sendData, T* recvData, int count, int root) const;
    
    // Scatter data from root to all processes
    template<typename T>
    void scatter(const T* sendData, T* recvData, int count, int root) const;
    
    // All-to-all communication
    template<typename T>
    void allToAll(const T* sendData, T* recvData, int count) const;
    
    // Send neural data to specific node
    bool sendNeuralData(int targetNode, const std::vector<float>& data);
    
    // Receive neural data from specific node
    bool receiveNeuralData(int sourceNode, std::vector<float>& data);
    
    // Broadcast spike events to all nodes
    bool broadcastSpikeEvents(const std::vector<SpikeEvent>& events);
    
    // Receive spike events from all nodes
    bool receiveSpikeEvents(std::vector<std::vector<SpikeEvent>>& events);
    
    // Send heartbeat to all nodes
    void sendHeartbeat(const HeartbeatMessage& heartbeat);
    
    // Receive heartbeat from all nodes
    bool receiveHeartbeat(HeartbeatMessage& heartbeat);
    
    // Exchange node information
    void exchangeNodeInfo(std::unordered_map<int, NodeInfoMessage>& nodeInfo);
    
    // Send checkpoint data to specific node
    bool sendCheckpoint(int targetNode, const CheckpointMessage& checkpoint);
    
    // Receive checkpoint data from specific node
    bool receiveCheckpoint(CheckpointMessage& checkpoint);
    
    // Report fault to all nodes
    void reportFault(const FaultReportMessage& faultReport);
    
    // Receive fault reports
    bool receiveFaultReport(FaultReportMessage& faultReport);
    
    // Load balancing communication
    bool exchangeLoadBalancingInfo(const LoadBalanceInfo& info, std::vector<LoadBalanceInfo>& results);
    
    // Get MPI rank (node ID)
    int getRank() const { return rank; }
    
    // Get total number of processes
    int getSize() const { return size; }
    
    // Check if this is the root process
    bool isRoot() const { return rank == 0; }
    
    // Get MPI communicator
    MPI_Comm getCommunicator() const { return comm; }
    
    // Get communication statistics
    float getAverageBandwidth() const;
    float getAverageLatency() const;
    int getMessagesSent() const { return messagesSent; }
    int getMessagesReceived() const { return messagesReceived; }
    
    // Check if MPI is initialized
    bool isInitialized() const { return isInitialized; }
    
    // Get hostname
    static std::string getHostname();
    
    // Get network interfaces
    static std::vector<std::string> getNetworkInterfaces();
    
    // Test MPI connectivity
    static bool testConnectivity(int timeout = 5000);  // milliseconds
    
    // Get error message for MPI error code
    static std::string getErrorMessage(int errorCode);

private:
    struct Impl;
    Impl* pImpl;
    
    bool isInitialized;
    int rank;
    int size;
    MPI_Comm comm;
    mutable std::mutex commMutex;
    
    // Communication statistics
    std::atomic<int> messagesSent;
    std::atomic<int> messagesReceived;
    std::vector<float> bandwidthHistory;
    std::vector<float> latencyHistory;
    mutable std::mutex statsMutex;
    
    // Message queues
    std::queue<NeuralDataMessage> neuralDataQueue;
    std::queue<SpikeEventMessage> spikeEventQueue;
    std::queue<HeartbeatMessage> heartbeatQueue;
    std::queue<CheckpointMessage> checkpointQueue;
    std::queue<FaultReportMessage> faultReportQueue;
    std::mutex queueMutex;
    
    // Progress thread
    std::thread progressThread;
    std::atomic<bool> progressThreadRunning;
    
    // Callbacks
    std::function<void(const NeuralDataMessage&)> neuralDataCallback;
    std::function<void(const SpikeEventMessage&)> spikeEventCallback;
    std::function<void(const HeartbeatMessage&)> heartbeatCallback;
    std::function<void(const FaultReportMessage&)> faultReportCallback;
};

// Load balancing information structure
struct LoadBalanceInfo {
    int nodeId;
    int neuronCount;
    int activeNeuronCount;
    int spikeCount;
    float memoryUsage;
    float cpuUsage;
    float loadScore;
    uint64_t timestamp;
};

// Abstract base class for communication protocols
class CommunicationProtocol {
public:
    virtual ~CommunicationProtocol() = default;
    
    virtual bool initialize() = 0;
    virtual void finalize() = 0;
    
    virtual bool send(int target, const void* data, size_t size) = 0;
    virtual bool receive(int source, void* data, size_t size) = 0;
    
    virtual bool isAvailable() const = 0;
    virtual std::string getName() const = 0;
};

// MPI communication protocol implementation
class MPIProtocol : public CommunicationProtocol {
public:
    explicit MPIProtocol(int rank, int size, MPI_Comm comm);
    ~MPIProtocol() override;
    
    bool initialize() override;
    void finalize() override;
    
    bool send(int target, const void* data, size_t size) override;
    bool receive(int source, void* data, size_t size) override;
    
    bool isAvailable() const override { return isAvailable; }
    std::string getName() const override { return "MPI"; }
    
    int getRank() const { return rank; }
    int getSize() const { return size; }

private:
    bool isAvailable;
    int rank;
    int size;
    MPI_Comm comm;
    std::mutex sendMutex;
    std::mutex receiveMutex;
};

} // namespace nlm

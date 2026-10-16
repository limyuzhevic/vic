#include "Communicator.hpp"
#include "../core/Logger/Logger.hpp"
#include <mpi.h>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ifaddrs.h>

namespace nlm {

struct Communicator::Impl {
    int rank;
    int size;
    MPI_Comm comm;
    bool isInitialized;
    DistributedConfig config;
    
    // Communication buffers
    std::vector<float> neuralDataBuffer;
    std::vector<SpikeEvent> spikeEventBuffer;
    std::vector<HeartbeatMessage> heartbeatBuffer;
    std::vector<CheckpointMessage> checkpointBuffer;
    std::vector<FaultReportMessage> faultReportBuffer;
    
    // Progress thread control
    std::atomic<bool> progressThreadRunning;
    std::thread progressThread;
    
    // Callback handlers
    std::function<void(const NeuralDataMessage&)> neuralDataCallback;
    std::function<void(const SpikeEventMessage&)> spikeEventCallback;
    std::function<void(const HeartbeatMessage&)> heartbeatCallback;
    std::function<void(const FaultReportMessage&)> faultReportCallback;
    
    // Message tracking
    std::atomic<int> messagesSent{0};
    std::atomic<int> messagesReceived{0};
    std::mutex messageMutex;
    
    // Network diagnostics
    float currentBandwidth;
    float currentLatency;
    std::chrono::high_resolution_clock::time_point lastBandwidthMeasure;
    
    Impl(const DistributedConfig& cfg) 
        : config(cfg), isInitialized(false), rank(0), size(1), currentBandwidth(0.0f), 
          currentLatency(0.0f), lastBandwidthMeasure(std::chrono::high_resolution_clock::now()) {
    }
};

Communicator::Communicator(const DistributedConfig& config) : pImpl(new Impl(config)) {
    isInitialized = false;
    rank = 0;
    size = 1;
    pImpl->isInitialized = false;
}

Communicator::~Communicator() {
    if (pImpl->progressThreadRunning) {
        pImpl->progressThreadRunning = false;
        if (pImpl->progressThread.joinable()) {
            pImpl->progressThread.join();
        }
    }
    
    if (isInitialized) {
        MPI_Finalize();
    }
}

Communicator::Communicator(Communicator&& other) noexcept : pImpl(other.pImpl) {
    isInitialized = other.isInitialized;
    rank = other.rank;
    size = other.size;
    other.pImpl = nullptr;
    other.isInitialized = false;
    other.rank = 0;
    other.size = 1;
}

Communicator& Communicator::operator=(Communicator&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        isInitialized = other.isInitialized;
        rank = other.rank;
        size = other.size;
        other.pImpl = nullptr;
        other.isInitialized = false;
        other.rank = 0;
        other.size = 1;
    }
    return *this;
}

bool Communicator::initialize() {
    if (isInitialized) {
        return true;
    }
    
    try {
        int provided;
        MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &provided);
        
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        
        comm = MPI_COMM_WORLD;
        isInitialized = true;
        pImpl->isInitialized = true;
        pImpl->rank = rank;
        pImpl->size = size;
        
        // Set up communication buffers
        pImpl->neuralDataBuffer.reserve(1000000);  // 1M floats
        pImpl->spikeEventBuffer.reserve(100000);
        
        // Start progress thread
        pImpl->progressThreadRunning = true;
        pImpl->progressThread = std::thread(&Communicator::progressLoop, this);
        
        NLM_LOG_INFO("MPI initialized successfully (Rank " + std::to_string(rank) + "/" + 
                    std::to_string(size) + ")");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to initialize MPI: ") + e.what());
        return false;
    }
}

void Communicator::finalize() {
    if (!isInitialized) return;
    
    pImpl->progressThreadRunning = false;
    if (pImpl->progressThread.joinable()) {
        pImpl->progressThread.join();
    }
    
    MPI_Finalize();
    isInitialized = false;
    pImpl->isInitialized = false;
    
    NLM_LOG_INFO("MPI finalized");
}

void Communicator::barrier() const {
    MPI_Barrier(comm);
}

template<typename T>
void Communicator::broadcast(T* data, int count, int root) const {
    MPI_Bcast(data, count, MPI_T, root, comm);
}

template<typename T>
void Communicator::gather(const T* sendData, T* recvData, int count, int root) const {
    MPI_Gather(sendData, count, MPI_T, recvData, count, MPI_T, root, comm);
}

template<typename T>
void Communicator::scatter(const T* sendData, T* recvData, int count, int root) const {
    MPI_Scatter(sendData, count, MPI_T, recvData, count, MPI_T, root, comm);
}

template<typename T>
void Communicator::allToAll(const T* sendData, T* recvData, int count) const {
    MPI_Alltoall(sendData, count, MPI_T, recvData, count, MPI_T, comm);
}

bool Communicator::sendNeuralData(int targetNode, const std::vector<float>& data) {
    if (!isInitialized || targetNode < 0 || targetNode >= size) {
        return false;
    }
    
    NeuralDataMessage message;
    message.sourceNode = rank;
    message.messageSize = data.size();
    message.data = data;
    message.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Pack and send
    std::vector<char> buffer(sizeof(NeuralDataMessage) + message.messageSize * sizeof(float));
    char* ptr = buffer.data();
    std::memcpy(ptr, &message, sizeof(NeuralDataMessage));
    ptr += sizeof(NeuralDataMessage);
    std::memcpy(ptr, message.data.data(), message.messageSize * sizeof(float));
    
    MPI_Status status;
    MPI_Send(buffer.data(), buffer.size(), MPI_CHAR, targetNode, MPITags::NEURAL_DATA, comm);
    
    pImpl->messagesSent++;
    
    return true;
}

bool Communicator::receiveNeuralData(int sourceNode, std::vector<float>& data) {
    if (!isInitialized) {
        return false;
    }
    
    int source = sourceNode >= 0 ? sourceNode : MPI_ANY_SOURCE;
    
    // Receive header first
    NeuralDataMessage header;
    MPI_Status status;
    MPI_Recv(&header, sizeof(NeuralDataMessage), MPI_CHAR, source, MPITags::NEURAL_DATA, comm, &status);
    
    pImpl->messagesReceived++;
    
    // Receive data
    data.resize(header.messageSize);
    if (header.messageSize > 0) {
        MPI_Recv(data.data(), header.messageSize, MPI_FLOAT, status.MPI_SOURCE, 
                MPITags::NEURAL_DATA, comm, MPI_STATUS_IGNORE);
    }
    
    return true;
}

bool Communicator::broadcastSpikeEvents(const std::vector<SpikeEvent>& events) {
    if (!isInitialized) return false;
    
    SpikeEventMessage message;
    message.sourceNode = rank;
    message.events = events;
    message.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Broadcast to all nodes
    int bufferSize = sizeof(SpikeEventMessage) + message.events.size() * sizeof(SpikeEvent);
    std::vector<char> buffer(bufferSize);
    char* ptr = buffer.data();
    std::memcpy(ptr, &message, sizeof(SpikeEventMessage));
    ptr += sizeof(SpikeEventMessage);
    std::memcpy(ptr, message.events.data(), message.events.size() * sizeof(SpikeEvent));
    
    MPI_Bcast(buffer.data(), bufferSize, MPI_CHAR, 0, comm);
    
    pImpl->messagesSent++;
    
    // Parse received message (all nodes)
    NeuralDataMessage header;
    ptr = buffer.data();
    std::memcpy(&header, ptr, sizeof(NeuralDataMessage));
    
    if (pImpl->spikeEventCallback) {
        SpikeEventMessage spikeMsg;
        spikeMsg.sourceNode = header.sourceNode;
        spikeMsg.events = message.events;  // Same as sent message
        spikeMsg.timestamp = message.timestamp;
        pImpl->spikeEventCallback(spikeMsg);
    }
    
    return true;
}

bool Communicator::receiveSpikeEvents(std::vector<std::vector<SpikeEvent>>& events) {
    if (!isInitialized) return false;
    
    SpikeEventMessage message;
    MPI_Status status;
    
    MPI_Recv(&message, sizeof(SpikeEventMessage), MPI_CHAR, MPI_ANY_SOURCE, 
            MPITags::SPIKE_EVENTS, comm, &status);
    
    pImpl->messagesReceived++;
    
    events.push_back(message.events);
    
    if (pImpl->spikeEventCallback) {
        pImpl->spikeEventCallback(message);
    }
    
    return true;
}

void Communicator::sendHeartbeat(const HeartbeatMessage& heartbeat) {
    if (!isInitialized) return;
    
    HeartbeatMessage message = heartbeat;
    message.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Send to all nodes
    MPI_Bcast(&message, sizeof(HeartbeatMessage), MPI_CHAR, 0, comm);
    
    pImpl->messagesSent++;
    
    if (pImpl->heartbeatCallback) {
        pImpl->heartbeatCallback(message);
    }
}

bool Communicator::receiveHeartbeat(HeartbeatMessage& heartbeat) {
    if (!isInitialized) return false;
    
    MPI_Status status;
    MPI_Recv(&heartbeat, sizeof(HeartbeatMessage), MPI_CHAR, MPI_ANY_SOURCE,
            MPITags::HEARTBEAT, comm, &status);
    
    pImpl->messagesReceived++;
    
    if (pImpl->heartbeatCallback) {
        pImpl->heartbeatCallback(heartbeat);
    }
    
    return true;
}

void Communicator::exchangeNodeInfo(std::unordered_map<int, NodeInfoMessage>& nodeInfo) {
    if (!isInitialized) return;
    
    // Create node info message for this node
    NodeInfoMessage myInfo;
    myInfo.nodeId = rank;
    myInfo.host = getHostname();
    myInfo.port = pImpl->config.mpiPort.empty() ? 12345 : std::stoi(pImpl->config.mpiPort);
    myInfo.active = true;
    
    // Gather all node info to root
    std::vector<NodeInfoMessage> allInfo(pImpl->size);
    gather(&myInfo, allInfo.data(), sizeof(NodeInfoMessage), 0);
    
    if (isRoot()) {
        // Parse gathered info
        for (int i = 0; i < pImpl->size; ++i) {
            NodeInfoMessage& info = allInfo[i];
            nodeInfo[info.nodeId] = info;
        }
    }
}

bool Communicator::sendCheckpoint(int targetNode, const CheckpointMessage& checkpoint) {
    if (!isInitialized || targetNode < 0 || targetNode >= size) {
        return false;
    }
    
    // Pack checkpoint message
    int bufferSize = sizeof(CheckpointMessage) + checkpoint.data.size();
    std::vector<char> buffer(bufferSize);
    char* ptr = buffer.data();
    std::memcpy(ptr, &checkpoint, sizeof(CheckpointMessage));
    ptr += sizeof(CheckpointMessage);
    std::memcpy(ptr, checkpoint.data.data(), checkpoint.data.size());
    
    MPI_Send(buffer.data(), bufferSize, MPI_CHAR, targetNode, MPITags::CHECKPOINT_DATA, comm);
    
    pImpl->messagesSent++;
    
    return true;
}

bool Communicator::receiveCheckpoint(CheckpointMessage& checkpoint) {
    if (!isInitialized) return false;
    
    MPI_Status status;
    MPI_Recv(&checkpoint, sizeof(CheckpointMessage), MPI_CHAR, MPI_ANY_SOURCE,
            MPITags::CHECKPOINT_DATA, comm, &status);
    
    pImpl->messagesReceived++;
    
    // Receive data
    checkpoint.data.resize(checkpoint.data.size());
    MPI_Recv(checkpoint.data.data(), checkpoint.data.size(), MPI_CHAR, status.MPI_SOURCE,
            MPITags::CHECKPOINT_DATA, comm, MPI_STATUS_IGNORE);
    
    return true;
}

void Communicator::reportFault(const FaultReportMessage& faultReport) {
    if (!isInitialized) return;
    
    FaultReportMessage message = faultReport;
    message.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Send to all nodes
    MPI_Bcast(&message, sizeof(FaultReportMessage), MPI_CHAR, 0, comm);
    
    pImpl->messagesSent++;
    
    if (pImpl->faultReportCallback) {
        pImpl->faultReportCallback(message);
    }
}

bool Communicator::receiveFaultReport(FaultReportMessage& faultReport) {
    if (!isInitialized) return false;
    
    MPI_Status status;
    MPI_Recv(&faultReport, sizeof(FaultReportMessage), MPI_CHAR, MPI_ANY_SOURCE,
            MPITags::FAULT_REPORT, comm, &status);
    
    pImpl->messagesReceived++;
    
    if (pImpl->faultReportCallback) {
        pImpl->faultReportCallback(faultReport);
    }
    
    return true;
}

bool Communicator::exchangeLoadBalancingInfo(const LoadBalanceInfo& info, std::vector<LoadBalanceInfo>& results) {
    if (!isInitialized) return false;
    
    // All-gather load balancing info
    std::vector<LoadBalanceInfo> allInfo(pImpl->size);
    allToAll(&info, allInfo.data(), sizeof(LoadBalanceInfo));
    
    results = std::move(allInfo);
    
    return true;
}

float Communicator::getAverageBandwidth() const {
    std::lock_guard<std::mutex> lock(pImpl->messageMutex);
    return pImpl->currentBandwidth;
}

float Communicator::getAverageLatency() const {
    std::lock_guard<std::mutex> lock(pImpl->messageMutex);
    return pImpl->currentLatency;
}

std::string Communicator::getHostname() {
    char hostname[256];
    hostname[255] = '\0';
    if (gethostname(hostname, 255) != 0) {
        return "unknown";
    }
    return std::string(hostname);
}

std::vector<std::string> Communicator::getNetworkInterfaces() {
    std::vector<std::string> interfaces;
    
    struct ifaddrs *ifa = nullptr, *ifap = nullptr;
    if (getifaddrs(&ifap) == 0) {
        for (ifa = ifap; ifa != nullptr; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
                char buf[INET_ADDRSTRLEN];
                if (inet_ntop(AF_INET, &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr, 
                             buf, INET_ADDRSTRLEN) != nullptr) {
                    interfaces.push_back(std::string(buf));
                }
            }
        }
        freeifaddrs(ifap);
    }
    
    return interfaces;
}

bool Communicator::testConnectivity(int timeout) {
    MPI_Comm testComm;
    MPI_Comm_dup(MPI_COMM_WORLD, &testComm);
    
    // Try to perform a simple MPI operation
    int rank, size;
    MPI_Comm_rank(testComm, &rank);
    MPI_Comm_size(testComm, &size);
    
    int sum = 0;
    MPI_Reduce(&rank, &sum, 1, MPI_INT, MPI_SUM, 0, testComm);
    
    MPI_Comm_free(&testComm);
    
    return true;
}

std::string Communicator::getErrorMessage(int errorCode) {
    char errorString[MPI_MAX_ERROR_STRING];
    int resultLen;
    MPI_Error_string(errorCode, errorString, &resultLen);
    return std::string(errorString);
}

void Communicator::progressLoop() {
    while (pImpl->progressThreadRunning) {
        // Progress MPI communication
        MPI_PROGRESS(MPI_COMM_WORLD);
        
        // Measure network performance periodically
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastBandwidthMeasure);
        
        if (duration.count() >= 10) {  // Every 10 seconds
            // Simple bandwidth estimation based on message count
            double currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            double timeDiff = (currentTime - pImpl->lastBandwidthMeasure.time_since_epoch().count()) / 1000.0;
            
            if (timeDiff > 0) {
                pImpl->currentBandwidth = pImpl->messagesSent * sizeof(float) / timeDiff / 1024.0f;  // KB/s
            }
            
            pImpl->lastBandwidthMeasure = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

MPIProtocol::MPIProtocol(int r, int s, MPI_Comm c) 
    : rank(r), size(s), comm(c), isAvailable(false) {
}

MPIProtocol::~MPIProtocol() {
    if (isAvailable) {
        MPI_Finalize();
    }
}

bool MPIProtocol::initialize() {
    int provided;
    MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &provided);
    
    isAvailable = (provided != MPI_THREAD_SINGLE);
    return isAvailable;
}

void MPIProtocol::finalize() {
    if (isAvailable) {
        MPI_Finalize();
        isAvailable = false;
    }
}

bool MPIProtocol::send(int target, const void* data, size_t size) {
    if (!isAvailable) return false;
    
    MPI_Send(const_cast<void*>(data), static_cast<int>(size), MPI_CHAR, target, 0, comm);
    return true;
}

bool MPIProtocol::receive(int source, void* data, size_t size) {
    if (!isAvailable) return false;
    
    MPI_Recv(data, static_cast<int>(size), MPI_CHAR, source, 0, comm, MPI_STATUS_IGNORE);
    return true;
}

} // namespace nlm

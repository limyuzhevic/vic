#pragma once

#include "Service.hpp"
#include "ServiceImpl.hpp"

namespace nlm {

class ServiceImpl : public Service {
public:
    explicit ServiceImpl(std::unique_ptr<Service> serviceImpl);
    ~ServiceImpl() override;
    
    const ServiceInfo& getInfo() const override;
    bool initialize() override;
    void shutdown() override;
    bool isInitialized() const override;
    bool handleRequest(const ServiceRequest& request, ServiceResponse& response) override;
    std::string getName() const override;
    ServiceType getType() const override;
    
private:
    std::unique_ptr<Service> serviceImpl_;
    ServiceState state_;
    bool initialized_;
};

} // namespace nlm

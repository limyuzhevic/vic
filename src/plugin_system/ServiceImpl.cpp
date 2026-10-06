#pragma once

#include "ServiceImpl.hpp"
#include "PluginSystem_impl.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

ServiceImpl::ServiceImpl(std::unique_ptr<Service> serviceImpl)
    : serviceImpl_(std::move(serviceImpl)), state_(ServiceState::Uninitialized), initialized_(false) {
}

ServiceImpl::~ServiceImpl() {
    try {
        shutdown();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("ServiceImpl destructor exception: ") + e.what());
    }
}

const ServiceInfo& ServiceImpl::getInfo() const {
    return serviceImpl_->getInfo();
}

bool ServiceImpl::initialize() {
    if (state_ == ServiceState::Initializing || state_ == ServiceState::Initialized) {
        return true;
    }
    
    state_ = ServiceState::Initializing;
    
    try {
        if (serviceImpl_->initialize()) {
            initialized_ = true;
            state_ = ServiceState::Initialized;
            NLM_LOG_INFO(std::string("ServiceImpl initialized: ") + getName());
            return true;
        }
        state_ = ServiceState::Error;
        NLM_LOG_ERROR(std::string("ServiceImpl failed to initialize: ") + getName());
        return false;
    } catch (const std::exception& e) {
        state_ = ServiceState::Error;
        NLM_LOG_ERROR(std::string("ServiceImpl initialization exception for ") + getName() + ": " + e.what());
        return false;
    }
}

void ServiceImpl::shutdown() {
    if (state_ == ServiceState::Shutdown || state_ == ServiceState::Uninitialized) {
        return;
    }
    
    try {
        serviceImpl_->shutdown();
        initialized_ = false;
        state_ = ServiceState::Shutdown;
        NLM_LOG_INFO(std::string("ServiceImpl shutdown: ") + getName());
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("ServiceImpl shutdown exception for ") + getName() + ": " + e.what());
    }
}

bool ServiceImpl::isInitialized() const {
    return initialized_;
}

bool ServiceImpl::handleRequest(const ServiceRequest& request, ServiceResponse& response) {
    try {
        return serviceImpl_->handleRequest(request, response);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("ServiceImpl handleRequest exception for ") + getName() + ": " + e.what());
        response.success = false;
        response.errorMessage = e.what();
        return false;
    }
}

std::string ServiceImpl::getName() const {
    return serviceImpl_->getName();
}

ServiceType ServiceImpl::getType() const {
    return serviceImpl_->getType();
}

} // namespace nlm

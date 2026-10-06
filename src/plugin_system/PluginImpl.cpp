#pragma once

#include "PluginImpl.hpp"
#include "PluginSystem_impl.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

PluginImpl::PluginImpl(std::unique_ptr<Plugin> pluginImpl)
    : pluginImpl_(std::move(pluginImpl)), state_(PluginState::Unloaded), initialized_(false) {
}

PluginImpl::~PluginImpl() {
    try {
        shutdown();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("PluginImpl destructor exception: ") + e.what());
    }
}

const PluginMetadata& PluginImpl::getMetadata() const {
    return pluginImpl_->getMetadata();
}

bool PluginImpl::initialize() {
    if (state_ == PluginState::Initializing || state_ == PluginState::Initialized) {
        return true;
    }
    
    state_ = PluginState::Initializing;
    
    try {
        if (pluginImpl_->initialize()) {
            initialized_ = true;
            state_ = PluginState::Initialized;
            NLM_LOG_INFO(std::string("PluginImpl initialized: ") + getName() + " v" + getVersion());
            return true;
        }
        state_ = PluginState::Error;
        NLM_LOG_ERROR(std::string("PluginImpl failed to initialize: ") + getName());
        return false;
    } catch (const std::exception& e) {
        state_ = PluginState::Error;
        NLM_LOG_ERROR(std::string("PluginImpl initialization exception for ") + getName() + ": " + e.what());
        return false;
    }
}

void PluginImpl::shutdown() {
    if (state_ == PluginState::Shutdown || state_ == PluginState::Unloaded) {
        return;
    }
    
    try {
        pluginImpl_->shutdown();
        initialized_ = false;
        state_ = PluginState::Shutdown;
        NLM_LOG_INFO(std::string("PluginImpl shutdown: ") + getName());
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("PluginImpl shutdown exception for ") + getName() + ": " + e.what());
    }
}

bool PluginImpl::isInitialized() const {
    return initialized_;
}

std::vector<ServiceInfo> PluginImpl::getProvidedServices() const {
    return pluginImpl_->getProvidedServices();
}

bool PluginImpl::onServiceRequest(const ServiceRequest& request, ServiceResponse& response) {
    return pluginImpl_->onServiceRequest(request, response);
}

bool PluginImpl::onPluginEvent(const PluginEvent& event) {
    return pluginImpl_->onPluginEvent(event);
}

bool PluginImpl::execute(const ExecutionContext& context) {
    try {
        return pluginImpl_->execute(context);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("PluginImpl execution exception for ") + getName() + ": " + e.what());
        return false;
    }
}

void PluginImpl::onBrainUpdate(const BrainUpdateEvent& event) {
    pluginImpl_->onBrainUpdate(event);
}

std::string PluginImpl::getName() const {
    return pluginImpl_->getName();
}

std::string PluginImpl::getVersion() const {
    return pluginImpl_->getVersion();
}

} // namespace nlm

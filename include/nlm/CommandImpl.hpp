#pragma once

#include "Command.hpp"
#include <stdexcept>

namespace nlm {

void SimulationCommandImpl::execute() {
    if (!executed) {
        executed = true;
        // Execute simulation
    } else if (executed && !undone) {
        // Already executed, can't execute again
        throw std::runtime_error("Command already executed");
    }
}

void SimulationCommandImpl::undo() {
    if (executed && !undone) {
        undone = true;
        // Undo simulation
    } else if (executed && undone) {
        // Already undone, can't undo again
        throw std::runtime_error("Command already undone");
    } else {
        // Not executed or already redone
        throw std::runtime_error("Cannot undo command");
    }
}

void SimulationCommandImpl::redo() {
    if (executed && undone) {
        undone = false;
        // Redo simulation
    } else if (executed && !undone) {
        // Already in redo state, can't redo again
        throw std::runtime_error("Command already in redo state");
    } else {
        // Not executed or already executed
        throw std::runtime_error("Cannot redo command");
    }
}

void SimulationCommandImpl::start() {
    // Start simulation
}

void SimulationCommandImpl::stop() {
    // Stop simulation
}

void SimulationCommandImpl::step() {
    // Step simulation
}

void SimulationCommandImpl::pause() {
    // Pause simulation
}
}
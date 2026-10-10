#include "InputHandler.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <ctime>

namespace nlm {

InputHandler::InputHandler()
    : currentInputMode_(InputType::Keyboard), historyEnabled_(true), autoCompleteEnabled_(true),
      gesturesEnabled_(true), maxHistorySize_(100), commandPrefix_("~"),
      touchActive_(false), gestureStartX_(0), gestureStartY_(0),
      mouseButtonStateMap_({{MouseButton::Left, false}, {MouseButton::Middle, false}, {MouseButton::Right, false}}) {
    initializeCommands();
}

InputHandler::~InputHandler() {
}

void InputHandler::initializeCommands() {
    commands_ = {
        {"~help", "Show help information", [](const std::vector<std::string>& args) {
            std::cout << "Available commands:\n";
            // Would list available commands
            return true;
        }},
        {"~clear", "Clear the screen", [](const std::vector<std::string>& args) {
            std::cout << "\033[2J\033[H";
            return true;
        }},
        {"~status", "Show system status", [](const std::vector<std::string>& args) {
            std::cout << "System status: OK\n";
            return true;
        }},
        {"~zoom", "Zoom visualization", [](const std::vector<std::string>& args) {
            if (args.size() > 0) std::cout << "Zoom: " << args[0] << "\n";
            return true;
        }},
        {"~mode", "Change display mode", [](const std::vector<std::string>& args) {
            if (args.size() > 0) std::cout << "Mode: " << args[0] << "\n";
            return true;
        }},
        {"~save", "Save visualization state", [](const std::vector<std::string>& args) {
            std::cout << "Saving visualization...\n";
            return true;
        }},
        {"~export", "Export visualization", [](const std::vector<std::string>& args) {
            if (args.size() > 0) std::cout << "Exporting as: " << args[0] << "\n";
            return true;
        }},
        {"~toggle", "Toggle visualization elements", [](const std::vector<std::string>& args) {
            if (args.size() > 0) std::cout << "Toggled: " << args[0] << "\n";
            return true;
        }},
        {"~reset", "Reset visualization", [](const std::vector<std::string>& args) {
            std::cout << "Reset visualization\n";
            return true;
        }},
    };
}

bool InputHandler::processInput(const std::string& input) {
    InputEvent event;
    event.type = InputType::Keyboard;
    event.timestamp = std::chrono::steady_clock::now();
    event.rawInput = input;
    
    // Parse keyboard input
    if (input.empty()) {
        return false;
    }
    
    char firstChar = input[0];
    if (firstChar == commandPrefix_[0]) {
        event.type = InputType::Command;
        return processInput(event);
    }
    
    // Simple key detection
    if (input.length() == 1) {
        event.key = static_cast<Key>(input[0]);
    } else if (input == "\033[A") { // Up arrow
        event.key = Key::Up;
    } else if (input == "\033[B") { // Down arrow
        event.key = Key::Down;
    } else if (input == "\033[C") { // Right arrow
        event.key = Key::Right;
    } else if (input == "\033[D") { // Left arrow
        event.key = Key::Left;
    } else if (input == "\033[H") { // Home
        event.key = Key::Home;
    } else if (input == "\033[F") { // End
        event.key = Key::End;
    }
    
    addToHistory(event);
    return processInput(event);
}

bool InputHandler::processInput(const InputEvent& event) {
    bool handled = false;
    
    switch (event.type) {
        case InputType::Keyboard:
            handled = onKeyboardInput(event);
            break;
        case InputType::Mouse:
            handled = onMouseInput(event);
            break;
        case InputType::Command:
            handled = onCommand(event.rawInput);
            break;
        default:
            break;
    }
    
    if (historyEnabled_) {
        addToHistory(event);
    }
    
    return handled;
}

void InputHandler::onKeyPress(Key key, bool ctrl, bool alt, bool shift, bool command) {
    InputEvent event;
    event.type = InputType::Keyboard;
    event.key = key;
    event.ctrlPressed = ctrl;
    event.altPressed = alt;
    event.shiftPressed = shift;
    event.commandPressed = command;
    event.timestamp = std::chrono::steady_clock::now();
    
    addToHistory(event);
    processInput(event);
}

void InputHandler::onKeyRelease(Key key) {
    InputEvent event;
    event.type = InputType::Keyboard;
    event.key = key;
    event.timestamp = std::chrono::steady_clock::now();
    
    addToHistory(event);
}

void InputHandler::onMouseMove(int x, int y) {
    InputEvent event;
    event.type = InputType::Mouse;
    event.mouseX = x;
    event.mouseY = y;
    event.timestamp = std::chrono::steady_clock::now();
    
    addToHistory(event);
    processInput(event);
}

void InputHandler::onMouseButtonPress(MouseButton button, int x, int y) {
    InputEvent event;
    event.type = InputType::Mouse;
    event.button = button;
    event.mouseX = x;
    event.mouseY = y;
    event.timestamp = std::chrono::steady_clock::now();
    
    if (button == MouseButton::Left) {
        mouseButtonStateMap_[MouseButton::Left] = true;
        onTouchStart(x, y);
    } else if (button == MouseButton::Right) {
        mouseButtonStateMap_[MouseButton::Right] = true;
    } else if (button == MouseButton::Middle) {
        mouseButtonStateMap_[MouseButton::Middle] = true;
    }
    
    addToHistory(event);
    processInput(event);
}

void InputHandler::onMouseButtonRelease(MouseButton button, int x, int y) {
    InputEvent event;
    event.type = InputType::Mouse;
    event.button = button;
    event.mouseX = x;
    event.mouseY = y;
    event.timestamp = std::chrono::steady_clock::now();
    
    if (button == MouseButton::Left) {
        mouseButtonStateMap_[MouseButton::Left] = false;
        onTouchEnd(x, y);
    } else if (button == MouseButton::Right) {
        mouseButtonStateMap_[MouseButton::Right] = false;
    } else if (button == MouseButton::Middle) {
        mouseButtonStateMap_[MouseButton::Middle] = false;
    }
    
    addToHistory(event);
}

void InputHandler::onMouseScroll(float deltaX, float deltaY) {
    InputEvent event;
    event.type = InputType::Mouse;
    event.deltaX = deltaX;
    event.deltaY = deltaY;
    event.timestamp = std::chrono::steady_clock::now();
    
    addToHistory(event);
    processInput(event);
}

void InputHandler::onTouchStart(int x, int y) {
    touchActive_ = true;
    gestureStartX_ = x;
    gestureStartY_ = y;
    touchStartTime_ = std::chrono::steady_clock::now();
}

void InputHandler::onTouchEnd(int x, int y) {
    if (!touchActive_) return;
    
    touchActive_ = false;
    
    auto gesture = detectGesture();
    if (gesturesEnabled_) {
        onGesture(gesture);
    }
}

void InputHandler::onTouchMove(int x, int y) {
    if (touchActive_) {
        InputEvent event;
        event.type = InputType::Touch;
        event.mouseX = x;
        event.mouseY = y;
        event.timestamp = std::chrono::steady_clock::now();
        
        addToHistory(event);
    }
}

std::string InputHandler::parseCommand(const std::string& input) const {
    if (input.empty() || input[0] != commandPrefix_[0]) {
        return input;
    }
    
    // Remove command prefix
    std::string command = input.substr(commandPrefix_.length());
    
    // Find matching command
    for (const auto& cmd : commands_) {
        if (cmd.name == command) {
            return command;
        }
    }
    
    return "unknown";
}

std::vector<std::string> InputHandler::getAvailableCommands() const {
    std::vector<std::string> availableCommands;
    for (const auto& cmd : commands_) {
        availableCommands.push_back(cmd.name + " - " + cmd.description);
    }
    return availableCommands;
}

void InputHandler::addToHistory(const InputEvent& event) {
    inputHistory_.push_back(event);
    
    if (inputHistory_.size() > maxHistorySize_) {
        inputHistory_.erase(inputHistory_.begin());
    }
}

InputHandler::Gesture InputHandler::detectGesture() const {
    Gesture gesture;
    
    if (!touchActive_) return gesture;
    
    int deltaX = gestureStartX_ - gestureStartX_;
    int deltaY = gestureStartY_ - gestureStartY_;
    
    float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - touchStartTime_);
    
    if (distance < 5) return gesture; // Too small to be a gesture
    
    if (duration.count() > 500) { // Long press
        gesture.type = Gesture::Type::LongPress;
    } else if (std::abs(deltaX) > std::abs(deltaY) * 2) { // Horizontal swipe
        if (deltaX > 0) {
            gesture.type = Gesture::Type::SwipeRight;
        } else {
            gesture.type = Gesture::Type::SwipeLeft;
        }
    } else if (std::abs(deltaY) > std::abs(deltaX) * 2) { // Vertical swipe
        if (deltaY > 0) {
            gesture.type = Gesture::Type::SwipeDown;
        } else {
            gesture.type = Gesture::Type::SwipeUp;
        }
    }
    
    return gesture;
}

std::string InputHandler::generateAutoComplete(const std::string& partialCommand) const {
    std::string result;
    size_t matchCount = 0;
    
    for (const auto& cmd : commands_) {
        if (cmd.name.find(partialCommand) == 0) {
            if (matchCount > 0) result += ", ";
            result += cmd.name;
            matchCount++;
            if (matchCount >= 5) break; // Limit suggestions
        }
    }
    
    return result;
}

bool InputHandler::matchesCommandPattern(const std::string& input, const std::string& pattern) const {
    return input.find(pattern) != std::string::npos;
}

std::string InputHandler::getSessionSummary() const {
    std::stringstream ss;
    ss << "Session Summary:\n";
    ss << "Total inputs: " << inputHistory_.size() << "\n";
    ss << "Commands executed: ";
    
    int commandCount = 0;
    for (const auto& event : inputHistory_) {
        if (event.type == InputType::Command) commandCount++;
    }
    
    ss << commandCount << "\n";
    ss << "Current command: " << currentCommand_ << "\n";
    
    return ss.str();
}

} // namespace nlm

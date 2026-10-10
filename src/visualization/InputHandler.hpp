#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <memory>

namespace nlm {

class InputHandler {
public:
    enum class InputType {
        Keyboard,
        Mouse,
        Touch,
        Gesture,
        Command
    };
    
    enum class MouseButton {
        Left,
        Right,
        Middle,
        None
    };
    
    enum class Key {
        Unknown,
        Escape,
        Enter,
        Space,
        Tab,
        Backspace,
        Delete,
        Up,
        Down,
        Left,
        Right,
        Home,
        End,
        PageUp,
        PageDown,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
        F11, F12,
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
        Numpad5, Numpad6, Numpad7, Numpad8, Numpad9
    };
    
    struct InputEvent {
        InputType type;
        std::chrono::steady_clock::time_point timestamp;
        std::string rawInput;
        Key key;
        MouseButton button;
        int mouseX;
        int mouseY;
        float deltaX;
        float deltaY;
        bool ctrlPressed;
        bool altPressed;
        bool shiftPressed;
        bool commandPressed;
        
        InputEvent() : type(InputType::Keyboard), key(Key::Unknown), button(MouseButton::None),
                      mouseX(0), mouseY(0), deltaX(0.0f), deltaY(0.0f),
                      ctrlPressed(false), altPressed(false), shiftPressed(false), commandPressed(false) {}
    };
    
    struct Gesture {
        enum class Type {
            None,
            SwipeLeft,
            SwipeRight,
            SwipeUp,
            SwipeDown,
            Pinch,
            Expand,
            Tap,
            DoubleTap,
            LongPress
        };
        
        Type type;
        int startX;
        int startY;
        int endX;
        int endY;
        std::chrono::steady_clock::time_point startTime;
        
        Gesture() : type(Type::None), startX(0), startY(0), endX(0), endY(0) {}
    };
    
    class InputListener {
    public:
        virtual ~InputListener() = default;
        virtual bool onKeyboardInput(const InputEvent& event) { return false; }
        virtual bool onMouseInput(const InputEvent& event) { return false; }
        virtual bool onGesture(const Gesture& gesture) { return false; }
        virtual bool onCommand(const std::string& command) { return false; }
    };
    
    InputHandler();
    ~InputHandler();
    
    void setInputMode(InputType mode) { currentInputMode_ = mode; }
    InputType getInputMode() const { return currentInputMode_; }
    
    void addListener(std::shared_ptr<InputListener> listener) {
        listeners_.push_back(listener);
    }
    
    void removeListener(std::shared_ptr<InputListener> listener) {
        listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
    }
    
    bool processInput(const std::string& input);
    bool processInput(const InputEvent& event);
    
    // Keyboard input
    void onKeyPress(Key key, bool ctrl = false, bool alt = false, bool shift = false, bool command = false);
    void onKeyRelease(Key key);
    
    // Mouse input
    void onMouseMove(int x, int y);
    void onMouseButtonPress(MouseButton button, int x, int y);
    void onMouseButtonRelease(MouseButton button, int x, int y);
    void onMouseScroll(float deltaX, float deltaY);
    
    // Gesture recognition
    void onTouchStart(int x, int y);
    void onTouchEnd(int x, int y);
    void onTouchMove(int x, int y);
    
    // Command processing
    std::string parseCommand(const std::string& input) const;
    std::vector<std::string> getAvailableCommands() const;
    
    // Input history and analysis
    void enableInputHistory(bool enable) { historyEnabled_ = enable; }
    void setMaxHistorySize(size_t size) { maxHistorySize_ = size; }
    const std::vector<InputEvent>& getInputHistory() const { return inputHistory_; }
    
    // Configuration
    void setCommandPrefix(const std::string& prefix) { commandPrefix_ = prefix; }
    void setAutoComplete(bool enable) { autoCompleteEnabled_ = enable; }
    void setEnableGestures(bool enable) { gesturesEnabled_ = enable; }
    
    // Session management
    void resetSession() {
        inputHistory_.clear();
        currentCommand_.clear();
        gestureStartPos_ = {0, 0};
    }
    
    std::string getSessionSummary() const;
    
private:
    InputType currentInputMode_;
    bool historyEnabled_;
    bool autoCompleteEnabled_;
    bool gesturesEnabled_;
    size_t maxHistorySize_;
    std::string commandPrefix_;
    
    std::vector<std::shared_ptr<InputListener>> listeners_;
    std::vector<InputEvent> inputHistory_;
    std::string currentCommand_;
    
    // Gesture recognition state
    bool touchActive_;
    int gestureStartX_, gestureStartY_;
    std::chrono::steady_clock::time_point touchStartTime_;
    
    std::map<Key, bool> keyStateMap_;
    std::map<MouseButton, bool> mouseButtonStateMap_;
    
    void addToHistory(const InputEvent& event);
    Gesture detectGesture() const;
    std::string generateAutoComplete(const std::string& partialCommand) const;
    bool matchesCommandPattern(const std::string& input, const std::string& pattern) const;
    
    // Command definitions
    struct Command {
        std::string name;
        std::string description;
        std::function<bool(const std::vector<std::string>&)> handler;
    };
    
    std::vector<Command> commands_;
    
    void initializeCommands();
};

} // namespace nlm

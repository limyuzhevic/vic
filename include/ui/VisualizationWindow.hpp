#pragma once

#include <string>
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class ControlPanel;
class ViewManager;
class LayoutManager;
class ViewExporter;
class InteractionManager;

// Main visualization window
class VisualizationWindow {
public:
    VisualizationWindow(const std::string& title);
    ~VisualizationWindow();
    
    // Window properties
    void setTitle(const std::string& title);
    std::string getTitle() const;
    
    void setSize(int width, int height);
    void getSize(int& width, int& height) const;
    
    void setPosition(int x, int y);
    void getPosition(int& x, int& y) const;
    
    // Window state
    void show();
    void hide();
    void close();
    bool isVisible() const;
    
    // Content management
    void setContent(std::shared_ptr<ControlPanel> panel);
    void addView(std::shared_ptr<ControlPanel> view);
    void removeView(std::shared_ptr<ControlPanel> view);
    
    // Event handling
    void processEvents();
    void setEventHandler(std::function<void(const std::string&)> handler);
    
    // Rendering
    void render();
    void update();
    
    // State persistence
    void saveState(const std::string& filename) const;
    void loadState(const std::string& filename);
    
    // Fullscreen and window management
    void setFullscreen(bool fullscreen);
    bool isFullscreen() const;
    void minimize();
    void maximize();
    void restore();
    
    // Mouse and keyboard input
    void setMouseCursor(bool show);
    void setKeyboardInput(bool enable);
    
    // Theme and appearance
    void setTheme(const std::string& theme);
    void setFontSize(int size);
    void setColors(const std::vector<std::string>& colors);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Interactive control panel
class ControlPanel {
public:
    ControlPanel(const std::string& title);
    virtual ~ControlPanel() = default;
    
    // Panel properties
    void setTitle(const std::string& title);
    std::string getTitle() const;
    
    void setSize(int width, int height);
    void getSize(int& width, int& height) const;
    
    // Panel state
    void show();
    void hide();
    bool isVisible() const;
    
    // Content
    virtual void render() = 0;
    virtual void update() = 0;
    
    // Controls
    void addButton(const std::string& label, std::function<void()> callback);
    void addSlider(const std::string& label, double min, double max, double value);
    void addComboBox(const std::string& label, const std::vector<std::string>& options);
    void addCheckbox(const std::string& label, bool value);
    void addTextField(const std::string& label, const std::string& value);
    
    // Callbacks
    void setCallback(std::function<void(const std::string&, const std::string&)> callback);
    
    // Theme
    void setTheme(const std::string& theme);
    
    // Docking
    void setDockPosition(const std::string& position); // "top", "bottom", "left", "right"
    void setAutoHide(bool autoHide);

protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// View manager - manages multiple visualization views
class ViewManager {
public:
    ViewManager();
    ~ViewManager();
    
    // View management
    void addView(std::shared_ptr<ControlPanel> view);
    void removeView(std::shared_ptr<ControlPanel> view);
    void clearViews();
    
    // View properties
    void setActiveView(std::shared_ptr<ControlPanel> view);
    std::shared_ptr<ControlPanel> getActiveView() const;
    
    // Layout management
    void setLayout(const std::string& layout); // "tiled", "cascaded", "tabbed", "floating"
    void setLayoutMargin(int margin);
    void setLayoutSpacing(int spacing);
    
    // View state
    void showAllViews();
    void hideAllViews();
    void maximizeView(std::shared_ptr<ControlPanel> view);
    void restoreView(std::shared_ptr<ControlPanel> view);
    
    // Rendering
    void renderAllViews();
    void updateAllViews();
    
    // Configuration
    void setViewTitle(std::shared_ptr<ControlPanel> view, const std::string& title);
    void setViewTheme(std::shared_ptr<ControlPanel> view, const std::string& theme);
    void setViewDockPosition(std::shared_ptr<ControlPanel> view, const std::string& position);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Layout manager - handles window layouts
class LayoutManager {
public:
    LayoutManager();
    ~LayoutManager();
    
    // Layout configuration
    void setLayoutType(const std::string& type); // "grid", "column", "row", "freeform"
    void setGridColumns(size_t columns);
    void setGridRows(size_t rows);
    void setGridSpacing(int spacing);
    void setGridMargin(int margin);
    
    // Window placement
    void addWindow(const std::string& windowId, int x, int y, int width, int height);
    void removeWindow(const std::string& windowId);
    void updateWindowPosition(const std::string& windowId, int x, int y);
    void updateWindowSize(const std::string& windowId, int width, int height);
    
    // Layout calculation
    void calculateLayout();
    void resizeWindows();
    
    // Docking support
    void addDock(const std::string& dockId, const std::string& position); // "top", "bottom", "left", "right"
    void removeDock(const std::string& dockId);
    void splitDock(const std::string& dockId, const std::string& direction);
    
    // Configuration
    void saveLayout(const std::string& filename) const;
    void loadLayout(const std::string& filename);
    void resetLayout();
    
    // State
    void setMaximizedWindow(const std::string& windowId, bool maximized);
    std::string getMaximizedWindow() const;
    
    // Rendering
    void render();
    void update();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// View exporter - export visualization configurations
class ViewExporter {
public:
    ViewExporter();
    ~ViewExporter();
    
    // Export formats
    enum class Format {
        XML,
        JSON,
        TXT,
        CSV,
        PNG,
        PDF
    };
    
    // Export configuration
    bool exportLayout(const std::string& filename, Format format);
    bool exportView(const std::string& filename, std::shared_ptr<ControlPanel> view, Format format);
    
    // Import configuration
    bool importLayout(const std::string& filename, Format format);
    bool importView(const std::string& filename, Format format);
    
    // Configuration management
    void setExportPath(const std::string& path);
    std::string getExportPath() const;
    
    // Preset configurations
    void addPreset(const std::string& name, const std::string& configuration);
    void removePreset(const std::string& name);
    void applyPreset(const std::string& name);
    std::vector<std::string> getPresetNames() const;
    
    // File filters
    void setFileFilter(const std::string& extension);
    std::string getFileFilter() const;
    
    // Progress reporting
    void setProgressCallback(std::function<void(double)> callback);
    void setErrorCallback(std::function<void(const std::string&)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Interaction manager - handles user interactions
class InteractionManager {
public:
    InteractionManager();
    ~InteractionManager();
    
    // Mouse input
    void setMouseCallback(std::function<void(int, int, int)> callback);
    void setMouseMotionCallback(std::function<void(int, int)> callback);
    void setMouseWheelCallback(std::function<void(int, int)> callback);
    
    // Keyboard input
    void setKeyCallback(std::function<void(int, int, int)> callback);
    void setCharCallback(std::function<void(unsigned int)> callback);
    
    // Touch input (for touchscreens)
    void setTouchBeginCallback(std::function<void(int, int)> callback);
    void setTouchEndCallback(std::function<void(int, int)> callback);
    void setTouchMotionCallback(std::function<void(int, int, int, int)> callback);
    
    // Window events
    void setWindowCloseCallback(std::function<void()> callback);
    void setWindowFocusCallback(std::function<void(bool)> callback);
    void setWindowResizeCallback(std::function<void(int, int)> callback);
    
    // Drag and drop
    void setDragDropCallback(std::function<void(const std::string&)> callback);
    
    // Selection
    void setSelectionCallback(std::function<void(const std::string&)> callback);
    void clearSelection();
    
    // Input state
    void captureMouse(bool capture);
    void captureKeyboard(bool capture);
    bool isMouseCaptured() const;
    bool isKeyboardCaptured() const;
    
    // Configuration
    void setInputScale(float scale);
    void setRepeatDelay(int delay);
    void setRepeatRate(int rate);
    
    // Debug
    void enableDebugMode(bool enable);
    bool isDebugMode() const;
    
    // Calibration (for touchscreens)
    void calibrateTouchScreen();
    void resetInputState();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
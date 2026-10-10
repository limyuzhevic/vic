#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace nlm {

class Widget {
public:
    enum class Type {
        Panel,
        Label,
        Text,
        Chart,
        Graph,
        List,
        Tree,
        Heatmap,
        Timeline,
        Status,
        Progress
    };
    
    struct Position {
        int x = 0;
        int y = 0;
        
        Position() = default;
        Position(int x, int y) : x(x), y(y) {}
    };
    
    struct Size {
        int width = 0;
        int height = 0;
        
        Size() = default;
        Size(int width, int height) : width(width), height(height) {}
    };
    
    struct Constraint {
        enum class Type {
            None,
            Left,
            Right,
            Top,
            Bottom,
            Width,
            Height,
            CenterX,
            CenterY,
            VStretch,
            HStretch
        };
        
        Type type;
        float value;
        float secondaryValue = 0.0f;
        
        Constraint(Type t, float v, float sv = 0.0f) : type(t), value(v), secondaryValue(sv) {}
    };
    
    Widget(Type type, const std::string& id = "", const Position& pos = Position(), const Size& size = Size());
    virtual ~Widget() = default;
    
    virtual void render() = 0;
    virtual void update() {}
    virtual bool handleInput(const std::string& input) { return false; }
    
    const std::string& getId() const { return id_; }
    Type getType() const { return type_; }
    const Position& getPosition() const { return position_; }
    const Size& getSize() const { return size_; }
    bool isVisible() const { return visible_; }
    
    void setPosition(const Position& pos) { position_ = pos; }
    void setSize(const Size& size) { size_ = size; }
    void setVisible(bool visible) { visible_ = visible; }
    void setParent(Widget* parent) { parent_ = parent; }
    
    Widget* getParent() const { return parent_; }
    int getScreenX() const;
    int getScreenY() const;
    
    virtual void addChild(std::unique_ptr<Widget> child);
    Widget* getChild(const std::string& id) const;
    const std::vector<std::unique_ptr<Widget>>& getChildren() const { return children_; }
    
    virtual void applyLayout(const std::map<std::string, Constraint>& constraints);
    virtual void calculateGeometry();
    
protected:
    std::string id_;
    Type type_;
    Position position_;
    Size size_;
    bool visible_;
    Widget* parent_;
    std::vector<std::unique_ptr<Widget>> children_;
    
    virtual void onPositionChanged() {}
    virtual void onSizeChanged() {}
};

class Panel : public Widget {
public:
    enum class BorderStyle {
        None,
        Single,
        Double,
        Box,
        Rounded
    };
    
    Panel(const std::string& id = "", const Position& pos = Position(), const Size& size = Size(), 
          BorderStyle border = BorderStyle::Single);
    
    void setTitle(const std::string& title) { title_ = title; }
    void setBorderStyle(BorderStyle style) { borderStyle_ = style; }
    void setBackgroundColor(const std::string& color) { backgroundColor_ = color; }
    
    void render() override;
    
private:
    std::string title_;
    BorderStyle borderStyle_;
    std::string backgroundColor_;
    std::vector<std::string> borderChars_;
};

class Label : public Widget {
public:
    Label(const std::string& text, const std::string& id = "", const Position& pos = Position(), 
          const Size& size = Size());
    
    void setText(const std::string& text) { text_ = text; }
    void setColor(const std::string& color) { color_ = color; }
    void setAlignment(int align) { alignment_ = align; } // 0=left, 1=center, 2=right
    
    void render() override;
    
private:
    std::string text_;
    std::string color_;
    int alignment_;
};

class Chart : public Widget {
public:
    Chart(const std::string& id = "", const Position& pos = Position(), const Size& size = Size());
    
    void setData(const std::vector<float>& data) { data_ = data; }
    void setLabels(const std::vector<std::string>& labels) { labels_ = labels; }
    void setColorScheme(const std::string& scheme) { colorScheme_ = scheme; }
    
    void render() override;
    
private:
    std::vector<float> data_;
    std::vector<std::string> labels_;
    std::string colorScheme_;
    std::vector<std::string> colors_;
};

class Graph : public Widget {
public:
    Graph(const std::string& id = "", const Position& pos = Position(), const Size& size = Size());
    
    void setSeries(const std::vector<std::vector<float>>& series) { series_ = series; }
    void setXLabel(const std::string& label) { xLabel_ = label; }
    void setYLabel(const std::string& label) { yLabel_ = label; }
    
    void render() override;
    
private:
    std::vector<std::vector<float>> series_;
    std::string xLabel_;
    std::string yLabel_;
};

class ListWidget : public Widget {
public:
    ListWidget(const std::string& id = "", const Position& pos = Position(), const Size& size = Size());
    
    void setItems(const std::vector<std::string>& items) { items_ = items; }
    void setItemHeight(int height) { itemHeight_ = height; }
    
    void render() override;
    bool handleInput(const std::string& input) override;
    
    void setSelectedIndex(int index) { selectedIndex_ = index; }
    int getSelectedIndex() const { return selectedIndex_; }
    
private:
    std::vector<std::string> items_;
    int itemHeight_;
    int selectedIndex_;
    int scrollOffset_;
};

class LayoutManager {
public:
    struct ScreenInfo {
        int width = 80;
        int height = 40;
        int availableWidth = 78;
        int availableHeight = 38;
    };
    
    LayoutManager();
    ~LayoutManager();
    
    void setScreenSize(int width, int height);
    void setRootWidget(std::unique_ptr<Widget> root) { rootWidget_ = std::move(root); }
    
    void addWidget(std::unique_ptr<Widget> widget);
    void removeWidget(const std::string& id);
    Widget* getWidget(const std::string& id) const;
    
    void updateLayout();
    void render() const;
    bool handleInput(const std::string& input);
    
    void setTheme(const std::string& theme);
    void setColorScheme(const std::string& scheme);
    
    void setPadding(int left, int top, int right, int bottom) {
        paddingLeft_ = left;
        paddingTop_ = top;
        paddingRight_ = right;
        paddingBottom_ = bottom;
    }
    
    void setMargin(int left, int top, int right, int bottom) {
        marginLeft_ = left;
        marginTop_ = top;
        marginRight_ = right;
        marginBottom_ = bottom;
    }
    
    void setSpacing(int hSpacing, int vSpacing) {
        horizontalSpacing_ = hSpacing;
        verticalSpacing_ = vSpacing;
    }
    
    void enableAutoLayout(bool enable) { autoLayoutEnabled_ = enable; }
    
    const ScreenInfo& getScreenInfo() const { return screenInfo_; }
    
private:
    std::unique_ptr<Widget> rootWidget_;
    ScreenInfo screenInfo_;
    
    int paddingLeft_, paddingTop_, paddingRight_, paddingBottom_;
    int marginLeft_, marginTop_, marginRight_, marginBottom_;
    int horizontalSpacing_, verticalSpacing_;
    bool autoLayoutEnabled_;
    std::string theme_;
    std::string colorScheme_;
    
    std::map<std::string, std::unique_ptr<Widget>> widgetMap_;
};

} // namespace nlm

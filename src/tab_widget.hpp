#ifndef TAB_WIDGET_HPP
#define TAB_WIDGET_HPP

#include "widget.hpp"
#include "sunken_box.hpp"

class Label;

struct TabWidgetTab {
    Widget *widget;
    Label *label;
    int left;
    int right;
    SunkenBox bg;
    glm::mat4 label_model;
    glm::mat4 left_line_model;
    glm::mat4 right_line_model;
    glm::mat4 top_line_model;
    bool is_current;
    int index;
};

class TabWidget : public Widget {
public:
    TabWidget(GuiWindow *window);
    ~TabWidget() override;

    void draw(const glm::mat4 &projection) override;

    void on_resize() override {
        update_model();
    }

    // if true, hide the tab bar when there are less than 2 tabs
    void set_auto_hide(bool value) {
        auto_hide = value;
        update_model();
    }

    void select_index(int index);
    void select_widget(Widget *widget);

    void on_mouse_move(const MouseEvent *event) override;
    void on_drag(const DragEvent *event) override;
    bool on_key_event(const KeyEvent *event) override;

    void add_widget(Widget *widget, const String &title);

    void move_tab(int source_index, int dest_index);
    void remove_tab(int index);
    void insert_tab(Widget *widget, const String &title, int insert_index);

    int tab_count() const {
        return tabs.length();
    }

    List<TabWidgetTab *> tabs;
    int current_index;
    TabWidgetTab *current_tab;
    glm::mat4 tab_bottom_line_model;
    glm::vec4 tab_border_color;
    int tab_height;
    glm::vec4 bg_color;
    glm::vec4 tab_bg_color;
    glm::mat4 tab_bg_model;
    glm::vec4 tab_text_color;
    glm::vec4 tab_selected_bg_color;
    int padding_left;
    int padding_right;
    int padding_top;
    int padding_bottom;
    int tab_spacing;
    int title_padding_left;
    int title_padding_right;
    int widget_top;
    bool auto_hide;
    bool show_tab_bar;
    void (*on_drag_tab)(TabWidgetTab *tab, TabWidget *tab_widget, const MouseEvent *event);
    void (*on_drag_event)(TabWidget *tab_widget, const DragEvent *event);

    void update_model();
    TabWidgetTab *get_tab_at(int x, int y);
    int get_insert_index_at(int x, int y);
    void change_current_index(int direction);
    int get_widget_index(Widget *widget);
    void get_tab_pos(int index, int *x, int *y);
    void clamp_current_index();
};

#endif

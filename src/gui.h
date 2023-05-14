/*
 * gui.h
 * This file is part of GoL Lab, a simulator of Conway's game of life.
 *
 * Copyright (C) 2022 - Cyprien Lacassagne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <gtkmm.h>
#include "simulation.h"

enum Action { DRAW, SELECT, DRAG };

struct Frame {
    double xMin;
    unsigned xMax;
    double yMin;
    unsigned yMax;
    double asp, height, width;
};

struct Point {
	double x;
	double y;
};

constexpr unsigned refresh_min(5);
constexpr unsigned refresh_max(500);
constexpr unsigned zoom_min(100);
constexpr unsigned zoom_max(200);

//===========================================

class MyArea : public Gtk::DrawingArea {
public:
    MyArea();
    virtual ~MyArea();
    // ------ Member functions ------
    void clear();
    void draw();
    void setFrame(Frame x);
    void adjustFrame();
    void refresh();
    void flip_pattern_left_right();
    void flip_pattern_up_down();
    void rotate_pattern();
    std::vector<Coordinates> rebase_coords(std::vector<Coordinates> abs_coords);

    void set_pattern(std::vector<Coordinates> data) { pattern = data; }
    void set_selection(std::vector<Coordinates> sel) { selection = sel; }
    void set_clipboard(std::vector<Coordinates> clip) { clipboard = clip; }

    std::vector<Coordinates> get_pattern() const { return pattern; }
    std::vector<Coordinates> get_selection() const { return selection; }
    std::vector<Coordinates> get_clipboard() const { return clipboard; }

    // ------ Signal handlers ------   
    bool on_enter_notify_event(GdkEventCrossing * crossing_event);
    bool on_leave_notify_event(GdkEventCrossing * crossing_event);
protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    void draw_frame(const Cairo::RefPtr<Cairo::Context>& cr);
private:
    unsigned pattern_width();
    unsigned pattern_height();
    Frame frame;
    Point p1, p2;
    bool empty;
    std::vector<Coordinates> pattern, selection, clipboard;
    enum Rotate {
        NORTH,
        EAST,
        SOUTH,
        WEST
    };
};

class ModelColumns : public Gtk::TreeModel::ColumnRecord {
public:
    ModelColumns() { add(m_col_id); add(m_col_name); }

    Gtk::TreeModelColumn<int> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
};

//===========================================

class SimulationWindow : public Gtk::Window {
public:
    SimulationWindow(Glib::RefPtr<Gtk::Application>, std::string __filename, int result);
    virtual ~SimulationWindow();
    // Display an error message if the file passed as argument in CLI is unexploitable
    void parse_file_error(int parsing_result);
protected:
    // ------ Member functions ------
    void set_default_frame();
    void create_action_groups(Glib::RefPtr<Gtk::Application> app);
    void instantiate_menubar_from_glade();
    void instantiate_toolbar_from_glade();
    // Create accelerators (shortcuts) for MenuBar items
    void create_refresh_scale();
    void create_control_buttons();
    void create_StatusBar();
    void create_ComboBoxes();
    void create_patterns_ComboBox();

    void zoom_frame();
    void updt_statusbar();
    void file_modified();
    bool save_changes_dialog();
    // Error dialog relative to file opening or reading
    void error_dialog_open(Glib::ustring error_message, Glib::ustring details);

    void read_settings();
    void write_settings();

    void update_cursor();
    void drag_frame();
    void update_selection();
    void draw(unsigned x, unsigned y);

    // ------ Signal handlers ------
    // File actions
    void on_action_new();
    void on_action_open();
    void on_action_save();
    void on_action_saveas();
    void on_action_quit();
    // Edit actions
    void on_action_cut();
    void on_action_copy();
    void on_action_clear();
    void on_action_paste();
    void on_action_select_all();
    void on_action_rotate();
    void on_action_flip_vertically();
    void on_action_flip_horizontally();
    void on_action_cursor_draw();
    void on_action_cursor_drag();
    void on_action_cursor_select();
    // Toggle Stabillity Detection
    void on_action_experiment();
    void on_action_zoom_in();
    void on_action_zoom_out();
    void on_action_reset_zoom();
    void on_button_increase_size_clicked();
    void on_button_decrease_size_clicked();
    void on_action_insert_pattern();
    void on_action_help();
    void on_button_about_clicked();

    void on_button_start_clicked();
    void on_button_step_clicked();
    void on_button_reset_clicked();
    void on_button_slower_clicked();
    void on_button_faster_clicked();
    void on_action_random();

    void on_checkbutton_dark_checked();
    void on_checkbutton_grid_checked();
    void on_checkbutton_fade_checked();
    void on_button_colorscheme_clicked();

    void on_event_add_timer();
    void on_event_delete_timer();

    // Translate the frame to the left
    void pan_frame_left(unsigned offset=1);
    // Translate the frame to the right
    void pan_frame_right(unsigned offset=1);
    // Translate the frame to the top
    void pan_frame_up(unsigned offset=1);
    // Translate the frame to the bottom
    void pan_frame_down(unsigned offset=1);

    bool on_timeout();
    // Called when any conventional key is pressed
    bool on_key_press_event(GdkEventKey * key_event) override;
    // Called when any mouse button is pressed
    bool on_button_press_event(GdkEventButton * event) override;
    // Set a pencil as cursor for the mouse once any mouse button is released
    bool on_button_release_event(GdkEventButton * event) override;
    // Called when one moves and holds the mouse over the drawing area
    bool on_motion_notify_event(GdkEventMotion * event) override;
    bool on_scroll_event(GdkEventScroll * event) override;
    bool on_delete_event(GdkEventAny * any_event) override;

    void on_combo_light_changed();
    void on_combo_dark_changed();
    void on_combo_pattern_changed();

    // -----Child widgets-----
    Gtk::MenuBar* m_MenuBar;
    Gtk::MenuItem fileMi;
    Gtk::MenuItem editMi;
    Gtk::MenuItem viewMi;
    Gtk::MenuItem toolsMi;
    Gtk::MenuItem m_helpMi;
    Gtk::Menu m_FileMenu;
    Gtk::Menu m_EditMenu;
    Gtk::Menu m_ViewMenu;
    Gtk::Menu m_ToolsMenu;
    Gtk::Menu m_HelpMenu;
    Gtk::MenuItem newMi;
    Gtk::MenuItem openMi;
    Gtk::MenuItem* saveMi;
    Gtk::MenuItem saveasMi;
    Gtk::MenuItem quitMi;
    Gtk::MenuItem* cutMi;
    Gtk::MenuItem* copyMi;
    Gtk::MenuItem* clearMi;
    Gtk::MenuItem* pasteMi;
    Gtk::MenuItem selectallMi;
    Gtk::MenuItem* randomMi;
    Gtk::MenuItem* rotateMi;
    Gtk::MenuItem* flipverMi;
    Gtk::MenuItem* fliphorMi;
    Gtk::MenuItem cursormodeMi;
    Gtk::Menu m_CursormodeMenu;
    Gtk::MenuItem drawMi;
    Gtk::MenuItem dragMi;
    Gtk::MenuItem selectMi;
    Gtk::MenuItem* zoominMi;
    Gtk::MenuItem* zoomoutMi;
    Gtk::MenuItem* resetzoomMi;
    Gtk::CheckMenuItem showToolbarMi;
    Gtk::CheckMenuItem showStatusBarMi;
    Gtk::CheckMenuItem* showgridMi;
    Gtk::CheckMenuItem* fadeMi;
    Gtk::CheckMenuItem* darkMi;
    Gtk::MenuItem colorschemeMi;
    Gtk::CheckMenuItem* experimentMi;
    Gtk::MenuItem simsizeMi;
    Gtk::Menu m_SimsizeMenu;
    Gtk::MenuItem* incrsizeMi;
    Gtk::MenuItem* decrsizeMi;
    Gtk::MenuItem patternMi;
    Gtk::MenuItem helpMi;
    Gtk::MenuItem aboutMi;
    Gtk::SeparatorMenuItem file_sepMi;
    Gtk::SeparatorMenuItem view_sepMi, view_sepMi2;

    Gtk::Toolbar* m_ToolBar;
    Gtk::ToolButton* toolbutton_new;
    Gtk::ToolButton* toolbutton_save;
    Gtk::ToolButton* toolbutton_cut;
    Gtk::ToolButton* toolbutton_copy;
    Gtk::ToolButton* toolbutton_paste;
    Gtk::ToolButton* toolbutton_zoomin;
    Gtk::ToolButton* toolbutton_zoomout;
    Gtk::ToolButton* toolbutton_resetzoom;
    Gtk::ToggleToolButton* toggletoolbutton_experiment;

    Gtk::Separator m_Separator;
    Gtk::Separator m_Sep;

    Gtk::Box m_SuperBox;
    Gtk::Box m_Box, m_Box_General, m_ButtonBox, m_RefreshBox;
    Gtk::ButtonBox m_RefreshButtonBox;
    Gtk::Frame m_Frame_Speed;
    Gtk::Button m_Button_Start, m_Button_Step, m_Button_Reset,
                m_Button_Slower, m_Button_Faster;
    // Gtk::Scale m_Scale;
    Gtk::Label m_Label_Refresh;

    Gtk::Statusbar m_StatusBar;

    Gtk::ComboBox m_ComboLight, m_ComboDark, m_ComboPatt;
    Gtk::CellRendererText m_cell;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel, m_pattTreeModel;

    MyArea m_Area;
    ModelColumns m_Columns;

    Glib::RefPtr<Gio::SimpleActionGroup> m_refActionGroup;
    Glib::RefPtr<Gtk::Builder> m_refBuilder;

    // -----Attributes-----
    enum ButtonType { NONE, LEFT, RIGHT, MIDDLE };

    bool timer_added;
    bool disconnect;
    bool experiment;

    int timeout_value;

    unsigned zoom;
    unsigned default_zoom;
    double frame_surface;
    std::string filename;
    std::string x, y;

    ButtonType button_type;
};

#endif

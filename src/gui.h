
/*
 *  gui.h -- GoL Lab -- GUI with various options and view controls
 *  Copyright (C) 2022 Cyprien Lacassagne

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <gtkmm.h>
#include "simulation.h"

// Parse what action has to be done by "on_motion_notify_event"
// Because it does not have access to the "event->button" information
enum ButtonType {NONE, LEFT, RIGHT, MIDDLE};

enum Action {DRAW, SELECT, DRAG};

const std::string ERR_OPEN_FILE("Failed opening the file\nMake sure the file exists");
const std::string ERR_CONFIG_FILE("Failed configuring the simulation due to invalid values\n");
const std::string ERR_EXT_FILE("The file has to be a plain text file (.txt)");

struct Frame{
    double xMin;
    unsigned xMax;
    double yMin;
    unsigned yMax;
    double asp, height, width;
};

struct Point
{
	double x;
	double y;
};

// Adjust the view range according to the world dimensions
void set_default_frame();

class MyArea : public Gtk::DrawingArea {
public:
    MyArea();
    virtual ~MyArea();
    void clear();
    void draw();
    void setFrame(Frame x);
    void adjustFrame();
    void refresh();
    void set_pattern(std::vector<Pos> data) { pattern = data; }
    void set_selection(std::vector<Pos> sel) { selection = sel; }
    void set_clipboard(std::vector<Pos> clip) { clipboard = clip; }
    std::vector<Pos> get_pattern() { return pattern; }
    std::vector<Pos> get_selection() { return selection; }
    std::vector<Pos> get_clipboard() { return clipboard; }
    void flip_pattern_left_right();
    void flip_pattern_up_down();
    void rotate_pattern();
    std::vector<Pos> rebase_coords(std::vector<Pos> abs_coords);

    // --- Signal handlers ---    
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
    std::vector<Pos> pattern, selection, clipboard;
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
    SimulationWindow(Glib::RefPtr<Gtk::Application>, std::string __filename);
    virtual ~SimulationWindow();
    // Display an error message if the file passed as argument in CLI is unexploitable
    void parse_file_error();
protected:
    void create_main_action_group(Glib::RefPtr<Gtk::Application> app);
    void instanciate_menubar_from_glade();
    void instanciante_toolbar_from_glade();
    // Create accelerators (shortcuts) for MenuBar items
    void create_refresh_scale();
    void create_control_buttons();
    void create_StatusBar();
    void create_ComboBoxes();
    void create_patterns_ComboBox();
    void zoom_frame();
    void updt_statusbar_coord();
    void file_modified();
    // Error dialog relative to file opening or reading
    void error_dialog_open(std::string error_message);

    void read_settings();
    void write_settings();

    void update_cursor();

    void update_selection();
    void drag_frame();

    // -----Signal handlers-----
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
    // Increase the zoom level and decrease the view angle (z ratio)
    void on_action_zoom_in();
    // Decrease the zoom level and increase the view angle (z ratio)
    void on_action_zoom_out();
    // Reset zoom values and the zoom label
    void on_action_reset_zoom();
    void on_button_increase_size_clicked();
    void on_button_decrease_size_clicked();
    void on_action_insert_pattern();
    void on_action_help();
    void on_button_about_clicked();

    void on_button_start_clicked();
    void on_button_step_clicked();
    void on_button_reset_clicked();
    void on_action_random();

    void on_checkbutton_show_toolbar_checked();
    void on_checkbutton_show_statusbar_checked();
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
    bool on_key_press_event(GdkEventKey * key_event);
    // Called when any mouse button is pressed
    bool on_button_press_event(GdkEventButton * event);
    // Set a pencil as cursor for the mouse once any mouse button is released
    bool on_button_release_event(GdkEventButton * event);
    // Called when one moves and holds the mouse over the drawing area
    bool on_motion_notify_event(GdkEventMotion * event);

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
    Gtk::CheckMenuItem* showToolbarMi;
    Gtk::CheckMenuItem* showStatusBarMi;
    Gtk::CheckMenuItem* showgridMi;
    Gtk::CheckMenuItem* fadeMi;
    Gtk::CheckMenuItem* darkmode;
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
    Gtk::ToolButton* toolbutton_zoomin;
    Gtk::ToolButton* toolbutton_zoomout;
    Gtk::ToolButton* toolbutton_resetzoom;

    Gtk::Separator m_Separator;
    Gtk::Separator m_Sep;

    Gtk::Box m_SuperBox;
    Gtk::Box m_Box, m_Box_General, m_ButtonBox;
    Gtk::Frame m_Frame_Speed;
    Gtk::Button m_Button_Start, m_Button_Step, m_Button_Reset, m_Button_Clear, m_Button_Random;
    Gtk::Scale m_Scale;
    Gtk::Label m_Label_Info, m_Label_Test, m_Label_Theme, m_LabelSize, m_LabelZoom, m_LabelCoordinates, m_Label_Population;

    Gtk::Statusbar m_StatusBar;

    Gtk::ComboBox m_ComboLight, m_ComboDark, m_ComboPatt;
    Gtk::CellRendererText m_cell;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel, m_pattTreeModel;

    MyArea m_Area;
    ModelColumns m_Columns;

    Glib::RefPtr<Gio::SimpleActionGroup> m_refActionGroup;
    Glib::RefPtr<Gtk::Builder> m_refBuilder;

    // -----Attributes-----
    bool timer_added;
    bool disconnect;
    bool experiment;
    int timeout_value;
    double frame_surface;
    std::string filename;
    std::string x, y;
    ButtonType button_type;
};

#endif

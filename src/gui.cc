/*
 * gui.cc 
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

#include <iostream>
#include <fstream>
#include <string>
#include <cairomm/context.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "gui.h"
#include "graphic_gui.h"
#include "config.h"
#include "prefs.h"

static Frame default_frame = {-1, World::get_x_max(), -1, World::get_y_max(),
                              window_width/window_height, window_height, window_width};

static void orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr,
                                    Frame frame);

static unsigned val(0);

static bool dark_theme_on(false);
static bool show_grid(false);

static bool inserting_pattern(false);
static bool dragging_frame(false);
static bool in_MyArea_window(false);
static CursorMode current_mode(DRAW);
// Mouse coordinates in the drawing area
static unsigned x_mouse, y_mouse, x_0_sel_mouse, y_0_sel_mouse;

//===========================================

MainArea::MainArea()
:   empty(false) {
    this->signal_enter_notify_event().connect(sigc::mem_fun(*this,
            &MainArea::on_enter_notify_event));
    this->signal_leave_notify_event().connect(sigc::mem_fun(*this,
            &MainArea::on_leave_notify_event));
    this->set_events(Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK | Gdk::SCROLL_MASK);
    set_size_request(window_width, window_height);
}

MainArea::~MainArea() {
    setFrame(default_frame);
}

void MainArea::clear() {
    empty = true;
    refresh();
}

void MainArea::draw() {
    empty = false;
    refresh();
}

void MainArea::refresh() {
    auto win = get_window();
    if (win) {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                               get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
}

void MainArea::setFrame(Frame f) {
	if((f.xMin <= f.xMax) and (f.yMin <= f.yMax) and (f.height > 0)) {
		f.asp = f.width/f.height;
		frame = f;
	}
	else {
		std::cout << "incorrect Model framing or window parameters\n";
    }
}

void MainArea::adjustFrame() {
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();

	frame.width  = width;
	frame.height = height;

    double new_aspect_ratio((double)width/height);
    if (new_aspect_ratio > default_frame.asp) {
	    frame.yMax = default_frame.yMax;
	    frame.yMin = default_frame.yMin;
	  
	    double delta(default_frame.xMax - default_frame.xMin);
	    double mid((default_frame.xMax + default_frame.xMin)/2);
        
	    frame.xMax = mid + 0.5*(new_aspect_ratio/default_frame.asp)*delta;
	    frame.xMin = mid - 0.5*(new_aspect_ratio/default_frame.asp)*delta;
    }else {
	    frame.xMax = default_frame.xMax;
	    frame.xMin = default_frame.xMin;

 	    double delta(default_frame.yMax - default_frame.yMin);
	    double mid((default_frame.yMax + default_frame.yMin)/2);

	    frame.yMax = mid + 0.5*(default_frame.asp/new_aspect_ratio)*delta;
	    frame.yMin = mid - 0.5*(default_frame.asp/new_aspect_ratio)*delta;
    }
}

static void orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr,
                                                                Frame frame) {
	cr->translate(frame.width/2, frame.height/2);

	cr->scale(frame.width/(frame.xMax - frame.xMin),
             -frame.height/(frame.yMax - frame.yMin));

	cr->translate(-(frame.xMin + frame.xMax)/2, -(frame.yMin + frame.yMax)/2);
}

void MainArea::draw_frame(const Cairo::RefPtr<Cairo::Context>& cr) {

    if (!dark_theme_on) {
        cr->set_source_rgb(gray1.r, gray1.g, gray1.b);
    }else {
        cr->set_source_rgb(0.1, 0.1, 0.1);
    }
    cr->paint();
    cr->stroke();
}

bool MainArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    if (!empty) {
        graphic_set_context(cr);

        draw_frame(cr);
        setFrame(default_frame);
        adjustFrame();
        orthographic_projection(cr, frame);

        graphic_draw_world(World::get_x_max(), World::get_y_max(), dark_theme_on, show_grid, default_frame.xMax-default_frame.xMin);
        simulation::draw_cells(dark_theme_on);

        if (inserting_pattern) {
            graphic_ghost_pattern(x_mouse, y_mouse, pattern, dark_theme_on);
            return true;
        }
        if (dragging_frame)
            return true;
        if (current_mode == SELECT) {
            graphic_draw_select_rec(x_0_sel_mouse, y_0_sel_mouse, x_mouse, y_mouse, dark_theme_on);
            graphic_highlight_selected_cells(selection, dark_theme_on);
        }
    }
    return true;
}

void MainArea::flip_pattern_left_right() {
    unsigned width(pattern_width());
        for (auto& e : pattern) {
            e.x = width - e.x;
        }
}

void MainArea::flip_pattern_up_down() {
    unsigned height(pattern_height());
    for (auto& e : pattern) {
        e.y = height - e.y;
    }
}

void MainArea::rotate_pattern() {
    unsigned tmp;
    unsigned height(pattern_height());
    for (auto& e : pattern) {
        tmp = e.x;
        e.x = height - e.y;
        e.y = tmp;
    }
}

std::vector<Coordinates> MainArea::rebase_coords(std::vector<Coordinates> abs_coords) {
    unsigned x_min(World::get_x_max());
    unsigned y_min(World::get_y_max());

    for (auto& e : abs_coords) {
        if (e.x < x_min)
            x_min = e.x;
        if (e.y < y_min)
            y_min = e.y;
    }
    for (auto& e : abs_coords) {
        e.x -= x_min;
        e.y -= y_min;
    }

    return abs_coords;
}

unsigned MainArea::pattern_width() {
    unsigned max(0);
    unsigned min(World::get_x_max());

    for (unsigned i(0); i < pattern.size(); ++i) {
        if (pattern[i].x > max)
            max = pattern[i].x;
        if (pattern[i].x < min)
            min = pattern[i].x;
    }

    return max-min;
}

unsigned MainArea::pattern_height() {
    unsigned max(0);
    unsigned min(World::get_y_max());

    for (unsigned i(0); i < pattern.size(); ++i) {
        if (pattern[i].y > max)
            max = pattern[i].y;
        if (pattern[i].y < min)
            min = pattern[i].y;
    }

    return max-min;
}

bool MainArea::on_enter_notify_event(GdkEventCrossing * crossing_event) {
    in_MyArea_window = (crossing_event->type == GDK_ENTER_NOTIFY);
    return in_MyArea_window;
}

bool MainArea::on_leave_notify_event(GdkEventCrossing * crossing_event) {
    in_MyArea_window = !(crossing_event->type == GDK_LEAVE_NOTIFY);
    return in_MyArea_window;
}

/**  
 *  =============================================================
 *  SimulationWindow constructor: TopLevel window initialization.
 *  Manages all signal handlers and events.
 *  =============================================================
 */
SimulationWindow::SimulationWindow(Glib::RefPtr<Gtk::Application> app,
                                   std::string __filename,
                                   int result)
:   timer_added(false),
    disconnect(false),
    experiment(false),
    timeout_value(startup_timeout_value),
    frame_surface(default_frame.xMax*default_frame.yMax),
    filename(__filename),
    x(std::to_string(default_frame.xMax / 2)),
    y(std::to_string(default_frame.yMax / 2)),
    n_selected(0),
    button_type(NONE),
    cmd_index(-1),
    disconnect_release_event(false),
    m_SuperBox(Gtk::ORIENTATION_VERTICAL),
    m_Box(Gtk::ORIENTATION_HORIZONTAL),
    m_Box_General(Gtk::ORIENTATION_VERTICAL, 15),
    m_ButtonBox(Gtk::ORIENTATION_VERTICAL, 5),
    m_RefreshButtonBox(Gtk::ORIENTATION_VERTICAL),
    m_Frame_Speed("Speed"),
    m_Button_Start("Start"),
    m_Button_Step("Step"),
    m_Button_Reset("Reset"),
    m_Button_Slower("Slower"),
    m_Button_Faster("Faster"),
    m_Label_Refresh("Refresh : " + std::to_string(startup_timeout_value) + " ms")   
{
    maximize();
    set_position(Gtk::WIN_POS_CENTER);
    set_default_icon_from_file(working_dir() + "share/icons/gol_lab.ico");
    set_resizable();
    if (result != 0)
        parse_file_error(result);
    if (filename.empty())
        set_title(PROGRAM_NAME);
    else 
        set_title(simulation::remove_filepath(filename) + " - " + PROGRAM_NAME);

    m_Box.set_margin_top(2);
    m_Box.set_margin_right(2);
    m_Box_General.set_margin_left(5);
    m_Box_General.set_margin_right(5);
    m_Box_General.set_size_request(130);

    this->set_events(Gdk::POINTER_MOTION_MASK | Gdk::SCROLL_MASK | Gdk::ENTER_NOTIFY_MASK);

    add(m_SuperBox);

    create_ComboBoxes();
    read_settings();
    create_action_groups(app);
    instantiate_menubar_from_glade();
    instantiate_toolbar_from_glade();

    m_SuperBox.pack_start(m_Separator, false, false);
    m_SuperBox.pack_start(m_Box);
    m_SuperBox.pack_start(m_StatusBar, false, false);
    m_Box.pack_start(m_Box_General, false, false);
    m_Box.pack_start(m_Area, Gtk::PACK_EXPAND_WIDGET);
    m_Box_General.pack_start(m_ButtonBox, false, false);
    m_Box_General.pack_start(m_Frame_Speed, false, false);
    m_Frame_Speed.add(m_RefreshButtonBox);
    m_Frame_Speed.set_shadow_type(Gtk::SHADOW_ETCHED_OUT);

    m_Separator.set_valign(Gtk::ALIGN_START);
    m_Separator.set_vexpand(false);

    create_control_buttons();
    create_StatusBar();

    m_ToolBar->set_visible();
    m_StatusBar.set_visible();

    zoom_frame();

    show_all_children();
    m_Area.refresh();
    preserve_aspect_ratio();
}

SimulationWindow::~SimulationWindow() {
    write_settings();
}

void SimulationWindow::parse_file_error(int reading_result) {
    simulation::init();
    const Glib::ustring file_not_found("Can't open \"" + simulation::remove_filepath(filename)
                                       + "\" because it wasn't found");
    switch (reading_result)
    {
    case 1:
        error_dialog_open(file_not_found, message::file_does_not_exist(filename));
        filename.clear();
        break;
    case 2:
        error_dialog_open("File format not supported", message::invalid_file_format(filename));
        filename.clear();
        break;
    case 3:
        error_dialog_open("The configuration format Life 1.06 is not respected",
                          message::unusable_file_data(filename));
        filename.clear();
        break;
    case 4:
        warning_dialog_open("The initial configuration specified by the file spreads out of "
                            "the world boundaries", message::invalid_cell_coordinate(filename));
        filename.clear();
        break;
    }
}

void SimulationWindow::set_default_frame() {
    default_frame.xMin = -1;
    default_frame.xMax = World::get_x_max();
    default_frame.yMin = -1;
    default_frame.yMax = World::get_y_max();
}

void SimulationWindow::zoom_frame() {
    unsigned zoom_diff(zoom_max-zoom);
    // Reset zoom level
    if (zoom_diff == 100) {
        set_default_frame();
        m_Area.refresh();
        return;
    }
    if (zoom_diff == 0)
        zoom_diff = 5;

    // Calculate the target dimensions of the frame
    double target_width(World::get_x_max() * (zoom_diff / 100.));
    double target_height(World::get_y_max() * (zoom_diff / 100.));
    double delta_x(default_frame.xMax - default_frame.xMin);
    double delta_y(default_frame.yMax - default_frame.yMin);

    double xMaxNew(default_frame.xMax - (delta_x - target_width) / 2);
    double xMinNew(default_frame.xMin + (delta_x - target_width) / 2);
    double yMaxNew(default_frame.yMax - (delta_y - target_height) / 2);
    double yMinNew(default_frame.yMin + (delta_y - target_height) / 2);

    // The four corners of the frame must stay inside the initial frame
    if (xMaxNew > World::get_x_max()) {
        xMinNew += (delta_x - target_width);
        xMaxNew = default_frame.xMax;
    }
    if (xMinNew < -1) {
        xMaxNew -= delta_x - target_width;
        xMinNew = default_frame.xMin;
    }
    if (yMaxNew > World::get_y_max()) {
        yMinNew += (delta_y - target_height);
        yMaxNew = default_frame.yMax;
    }
    if (yMinNew < -1) {
        yMaxNew -= delta_y - target_height;
        yMinNew = default_frame.yMin;
    }
    // Update the position of the four corners
    default_frame.xMax = xMaxNew;
    default_frame.xMin = xMinNew;
    default_frame.yMax = yMaxNew;
    default_frame.yMin = yMinNew;

    m_Area.refresh();
}

void SimulationWindow::preserve_aspect_ratio() {
    float ratio((float)m_Area.get_width() / m_Area.get_height());
    int prev_width(m_Area.get_width());
    int new_width(2 * prev_width / ratio);
    m_Area.set_size_request(0, 0);
    int offset(prev_width - new_width);

    if (this->is_maximized())
        m_ButtonBox.set_size_request(m_ButtonBox.get_width() + offset);
    else {
        if (offset >= 0)
            this->resize(this->get_width() - offset, this->get_height());
        else
            this->resize(this->get_width(), this->get_height() + offset/2);
    }
}

void SimulationWindow::updt_statusbar() {
    // const Glib::ustring index("Index: " + std::to_string(cmd_index) + "\t\t");
    const Glib::ustring generation("Generation: " + std::to_string(val) + "\t\t");
    const Glib::ustring population("Population: " + std::to_string(simulation::get_population()) + "\t\t");
    Glib::ustring mouse_coord("x: " + std::to_string(x_mouse) + ", y: " + std::to_string(y_mouse));
    if (n_selected != 0)
        mouse_coord += " (" + std::to_string(n_selected) + " selected)";
    const Glib::ustring zoom_level("\t\t" + std::to_string(zoom) + "%\t\t");
    const Glib::ustring dim(std::to_string(World::get_x_max()) + " x " + std::to_string(World::get_y_max()));
    Glib::ustring status(generation + population + mouse_coord + zoom_level + dim);
    if (experiment)
        status = "Stability detection enabled\t\t" + status;
    m_StatusBar.pop();
    m_StatusBar.push(status);
}

void SimulationWindow::stabilize_history() {
    ++cmd_index;
    unsigned threshold(cmd_history.size() - cmd_index);
    unsigned n_cmd(cmd_history.size());
    for (unsigned i(0); i < threshold; ++i) {
        delete cmd_history[n_cmd - 1 - i];
        cmd_history.pop_back();
    }
    undoMi->set_sensitive();
    if (cmd_index == cmd_history.size())
        redoMi->set_sensitive(false);
}

// The asterisk shows that change are occuring, but the file
// is not overwritten yet. Click on 'Save' to overwrite the file
void SimulationWindow::file_modified() {
    if (filename != "" && !saveMi->get_sensitive()) {
        saveMi->set_sensitive(true);
        toolbutton_save->set_sensitive(true);
        m_Button_Reset.set_sensitive(true);
        this->set_title("*" + this->get_title());
    }
}

bool SimulationWindow::save_changes_dialog() {
    const Glib::ustring file(simulation::remove_filepath(filename));
    const Glib::ustring msg("Do you want to save changes to \"" + file + "\"?");
    Gtk::MessageDialog dialog(msg, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE, true);
    dialog.add_button("_Save", Gtk::RESPONSE_YES);
    dialog.add_button("_Don't Save", Gtk::RESPONSE_NO);
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.set_transient_for(*this);
    int result = dialog.run();
    switch (result) {
        case Gtk::RESPONSE_YES:
            on_action_save();
            return false;
            break;
        case Gtk::RESPONSE_NO:
            return false;
        case Gtk::RESPONSE_CANCEL:
            break;
        default: 
            break;
    }
    return true;
}

void SimulationWindow::error_dialog_open(Glib::ustring error_message, Glib::ustring details) {
    Gtk::MessageDialog error_dialog(error_message, false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);

    error_dialog.set_secondary_text(details, true);
    error_dialog.set_position(Gtk::WIN_POS_CENTER);
    error_dialog.set_title("Error - upload failed");          
    error_dialog.set_transient_for(*this);
    error_dialog.run();

    this->set_title(PROGRAM_NAME);
}

void SimulationWindow::warning_dialog_open(Glib::ustring warning_message, Glib::ustring details) {
    Gtk::MessageDialog warning_dialog(warning_message, false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK, true);

    warning_dialog.set_secondary_text(details, true);
    warning_dialog.set_position(Gtk::WIN_POS_CENTER);
    warning_dialog.set_title("Warning - configuration");          
    warning_dialog.set_transient_for(*this);
    warning_dialog.run();

    this->set_title(PROGRAM_NAME);
}

void SimulationWindow::read_settings() {
    Glib::KeyFile keyFile;
    std::string settings_filepath(working_dir() + SETTINGS_INI_FILE);
    keyFile.load_from_file(settings_filepath, Glib::KEY_FILE_KEEP_COMMENTS);

    dark_theme_on = keyFile.get_boolean(prefs::settings_group, prefs::dark_theme_key);
    simulation::adjust_bool_grid();
    default_zoom = keyFile.get_uint64(prefs::preferences_group, prefs::default_zoom_key);
    zoom = default_zoom;
    show_grid = keyFile.get_boolean(prefs::preferences_group, prefs::show_grid_key);
    m_ComboLight.set_active(keyFile.get_integer(prefs::preferences_group, prefs::light_scheme_key));
    m_ComboDark.set_active(keyFile.get_integer(prefs::preferences_group, prefs::dark_scheme_key));
}

void SimulationWindow::write_settings() {

    const std::string light_s_new_keyval(std::to_string(m_ComboLight.get_active_row_number()));
    const std::string dark_s_new_keyval(std::to_string(m_ComboDark.get_active_row_number()));
    std::ifstream settings_file(working_dir() + SETTINGS_INI_FILE);
    std::ostringstream ss;
    ss << settings_file.rdbuf();
    std::string str(ss.str());

    if (!settings_file.fail()) {
        size_t pos(str.find(prefs::dark_theme_key));
        str.replace(pos + prefs::dark_theme_key.length() + 1, 1, std::to_string(dark_theme_on));
        pos = str.find(prefs::light_scheme_key);
        str.replace(pos + prefs::light_scheme_key.length() + 1, 1, light_s_new_keyval);
        pos = str.find(prefs::dark_scheme_key);
        str.replace(pos + prefs::dark_scheme_key.length() + 1, 1, dark_s_new_keyval);
        pos = str.find(prefs::default_zoom_key);
        str.replace(pos + prefs::default_zoom_key.length() + 1, 3, std::to_string(zoom));
        pos = str.find(prefs::show_grid_key);
        str.replace(pos + prefs::show_grid_key.length() + 1, 1, std::to_string(show_grid));
    }

    settings_file.close();
    std::ofstream edited_settings(working_dir() + SETTINGS_INI_FILE);
    edited_settings << str;
}

void SimulationWindow::update_cursor() {
    Glib::RefPtr<Gdk::Window> ref_window(m_Area.get_window());
    Glib::RefPtr<Gdk::Display> display(m_Area.get_display());

    auto pencil(Gdk::Cursor::create(display, Gdk::CursorType::PENCIL));
    auto hand(Gdk::Cursor::create(Gdk::CursorType::HAND1));
    auto cross(Gdk::Cursor::create(Gdk::CursorType::CROSS));

    if (inserting_pattern || dragging_frame) {
        ref_window->set_cursor(hand);
        return;
    }
    switch (current_mode) {
        case DRAW:
            ref_window->set_cursor(pencil);
            break;
        case DRAG:
            ref_window->set_cursor(hand);
            break;
        case SELECT:
            ref_window->set_cursor(cross);
            break;
    }
}

void SimulationWindow::drag_frame() {
    bool positive_x_dir(x_0_sel_mouse < x_mouse);
    bool positive_y_dir(y_0_sel_mouse < y_mouse);

    unsigned delta_x((1-2*(!positive_x_dir))*(x_mouse-x_0_sel_mouse));
    unsigned delta_y((1-2*(!positive_y_dir))*(y_mouse-y_0_sel_mouse));

    if (positive_x_dir) {
        if (delta_x > x_0_sel_mouse)
            delta_x = x_0_sel_mouse;
    }else if (delta_x > default_frame.xMax-x_0_sel_mouse)
            delta_x = default_frame.xMax-x_0_sel_mouse;
    if (positive_y_dir) {
        if (delta_y > y_0_sel_mouse)
            delta_y = y_0_sel_mouse;
    }else if (delta_y > default_frame.yMax-y_0_sel_mouse)
            delta_y = default_frame.yMax-y_0_sel_mouse;

    positive_x_dir ? pan_frame_left(delta_x) : pan_frame_right(delta_x);
    positive_y_dir ? pan_frame_down(delta_y) : pan_frame_up(delta_y);
}

void SimulationWindow::update_selection() {
    unsigned x_min = (x_0_sel_mouse <= x_mouse) ? x_0_sel_mouse : x_mouse;
    unsigned x_max = (x_0_sel_mouse > x_mouse) ? x_0_sel_mouse : x_mouse;
    unsigned y_min = (y_0_sel_mouse <= y_mouse) ? y_0_sel_mouse : y_mouse;
    unsigned y_max = (y_0_sel_mouse > y_mouse) ? y_0_sel_mouse : y_mouse;

    std::vector<Coordinates> new_sel(simulation::get_live_cells_in_area(x_min, x_max, y_min, y_max));
    n_selected = new_sel.size();
    m_Area.set_selection(new_sel);
}

void SimulationWindow::draw(unsigned x, unsigned y) {
    simulation::new_birth(x, y);
    // Stuff might be coming soon
}

void SimulationWindow::pan_frame_left(unsigned offset) {
    if (zoom == 100 || default_frame.xMin == -1)
        return;
    if (offset <= default_frame.xMin) {
        default_frame.xMin -= offset;
        default_frame.xMax -= offset;
    }else {
        default_frame.xMax -= default_frame.xMin;
        default_frame.xMin = -1;
    }
    updt_statusbar();
    m_Area.refresh();
}

void SimulationWindow::pan_frame_right(unsigned offset) {
    if (zoom == 100 || default_frame.xMax == World::get_x_max())
        return;
    if (offset <= World::get_x_max() - default_frame.xMax) {
        default_frame.xMin += offset;
        default_frame.xMax += offset;
    }else {
        default_frame.xMin += World::get_x_max()-default_frame.xMax;
        default_frame.xMax = World::get_x_max();
    }
    updt_statusbar();
    m_Area.refresh();
}

void SimulationWindow::pan_frame_up(unsigned offset) {
    if (zoom == 100 || default_frame.yMax == World::get_y_max())
        return;
    if (offset <= World::get_y_max() - default_frame.yMax) {
        default_frame.yMin += offset;
        default_frame.yMax += offset;
    }else {
        default_frame.yMin += World::get_y_max()-default_frame.yMax;
        default_frame.yMax = World::get_y_max();
    }
    updt_statusbar();
    m_Area.refresh();
}

void SimulationWindow::pan_frame_down(unsigned offset) {
    if (zoom == 100 || default_frame.yMin == -1)
        return;
    if (offset <= default_frame.yMin) {
        default_frame.yMin -= offset;
        default_frame.yMax -= offset;
    }else {
        default_frame.yMax -= default_frame.yMin;
        default_frame.yMin = -1;
    }
    updt_statusbar();
    m_Area.refresh();
}

void SimulationWindow::add_timer() {
    if (!timer_added) {
        Glib::signal_timeout().connect(sigc::mem_fun(*this,
                    &SimulationWindow::on_timeout), timeout_value);
        timer_added = true;
    }
}

void SimulationWindow::delete_timer() {
    if (timer_added) {
        disconnect = true;
        timer_added = false;
    }
}

// ---------------
// Signal handlers
// ---------------

void SimulationWindow::on_combo_light_changed() {
    const auto iter = m_ComboLight.get_active();
    if (iter) {
        const auto row = *iter;
        if (row) {
            int id = row[m_Columns.m_col_id];
            Glib::ustring name = row[m_Columns.m_col_name];
            graphic_change_light_color_scheme(id-1);
        }
    }
    m_Area.refresh();
}

void SimulationWindow::on_combo_dark_changed() {
    const auto iter = m_ComboDark.get_active();
    if (iter) {
        const auto row = *iter;
        if (row) {
            int id = row[m_Columns.m_col_id];
            Glib::ustring name = row[m_Columns.m_col_name];
            graphic_change_dark_color_scheme(id-1);
        }
    }
    m_Area.refresh();
}

void SimulationWindow::on_action_cursor_mode_changed(int parameter)  {
    m_refCursorMode->change_state(parameter);
    if (parameter == 1)
        current_mode = DRAW;
    else if (parameter == 2)
        current_mode = DRAG;
    else if (parameter == 3)
        current_mode = SELECT;
    update_cursor();
}

bool SimulationWindow::on_timeout() {
    if (disconnect) {
        disconnect = false;
        return false;
    }

    if (experiment) {
        if (simulation::update(simulation::EXPERIMENTAL)) {
            // Reset the properties of the start button
            m_Button_Start.set_label("Start");
            Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
            css_provider->load_from_data("button {background-image: image(green);}");
            m_Button_Start.get_style_context()->add_provider(css_provider,
                                                             GTK_STYLE_PROVIDER_PRIORITY_USER);
            if (val < 4)
                val = 0;
            else
                val -= 4;
            // Stop the timer
            delete_timer();
        }
    }else {
        simulation::update();
    }
    if (simulation::get_population() == 0 && m_Button_Start.get_label() == "Stop") {
        on_button_start_clicked();
        --val;
    }
    ++val;
    updt_statusbar();
    if (current_mode == SELECT)
        update_selection();
    file_modified();
    m_Area.refresh();
    return true;
}

bool SimulationWindow::on_key_press_event(GdkEventKey * key_event) {
    if (key_event->type == GDK_KEY_PRESS) {

        if (key_event->keyval == GDK_KEY_space) {
            SimulationWindow::on_button_start_clicked();
            return false;
        }
        if (key_event->keyval == GDK_KEY_Escape && inserting_pattern) {
            inserting_pattern = false;
            rotateMi->set_sensitive(false);
            flipverMi->set_sensitive(false);
            fliphorMi->set_sensitive(false);
            update_cursor();
            m_Area.refresh();
            return Gtk::Window::on_key_press_event(key_event);
        }

        switch (key_event->keyval) {
            case GDK_KEY_F2:
                SimulationWindow::on_action_cursor_mode_changed(1);
                break;
            case GDK_KEY_F3:
                SimulationWindow::on_action_cursor_mode_changed(2);
                break;
            case GDK_KEY_F4:
                SimulationWindow::on_action_cursor_mode_changed(3);
                break;
        }
            
        switch(gdk_keyval_to_unicode(key_event->keyval)) {
            case '1':
                SimulationWindow::on_button_step_clicked();
                break;
            case 'h':
                SimulationWindow::pan_frame_left();
                break;
            case 'j':
                SimulationWindow::pan_frame_down();
                break;
            case 'k':
                SimulationWindow::pan_frame_up();
                break;
            case 'l':
                SimulationWindow::pan_frame_right();
                break;
            case 'm':
                m_Area.set_size_request(m_Area.get_width() - 10);
                m_ButtonBox.set_size_request(m_ButtonBox.get_width() + 10);
                break;
            case 'M':
                m_ButtonBox.set_size_request(m_ButtonBox.get_width() - 10);
                m_Area.set_size_request(m_Area.get_width() + 10);
                break;
            default:
                break;
        }
    }
    return Gtk::Window::on_key_press_event(key_event);
}

bool SimulationWindow::on_button_press_event(GdkEventButton * event)
{
    update_cursor();
	if(event->type == GDK_BUTTON_PRESS)
	{
		// raw mouse coordinates in the window frame
		double clic_x = event->x ;
		double clic_y = event->y ;

		// origin of the drawing area
		double origin_x = m_Area.get_allocation().get_x();
		double origin_y = m_Area.get_allocation().get_y();

		// get width and height of drawing area
		double width = m_Area.get_allocation().get_width();
		double height= m_Area.get_allocation().get_height();

		// retain only mouse events located within the drawing area
		if(clic_x >= origin_x && clic_x <= origin_x + width &&
		   clic_y >= origin_y && clic_y <= origin_y + height) { 
			// Point that we are allowed to use expressed with drawing area coord.
			Point p({clic_x - origin_x, clic_y - origin_y});
            int x(round((p.x*(default_frame.xMax-default_frame.xMin)/width) + default_frame.xMin));
            int y(round(default_frame.yMax - (p.y*(default_frame.yMax-default_frame.yMin)/height)));

            if (x < (int)World::get_x_max() && y < (int)World::get_y_max()) {
                x_mouse = x;
                y_mouse = y;
                switch (event->button)
                {
                case 1:
                    button_type = LEFT;
                    if (inserting_pattern) {
                        stabilize_history();
                        cmd_history.push_back(new InsertPatternCommand(x, y, simulation::get_state(),
                                                                m_Area.get_pattern()));
                        cmd_history[cmd_index]->execute();
                        file_modified();
                        // Done
                        rotateMi->set_sensitive(false);
                        flipverMi->set_sensitive(false);
                        fliphorMi->set_sensitive(false);
                        break;
                    }
                    if (current_mode == DRAW && !simulation::is_alive(x, y)) {
                        current_drawing.prev_state_screenshot = simulation::get_state();
                        current_drawing.pattern.push_back({x, y});
                        simulation::new_birth(x, y);
                        file_modified();
                    }
                    break;
                case 2:
                    button_type = MIDDLE;
                    dragging_frame = true;
                    drag_frame();
                    break;
                case 3:
                    button_type = RIGHT;
                    if (current_mode == DRAW && simulation::is_alive(x, y)) {
                        stabilize_history();
                        cmd_history.push_back(new RemoveCellCommand(x ,y));
                        cmd_history[cmd_index]->execute();
                        file_modified();
                    }
                    break;
                default:
                    break;
                }
                updt_statusbar();
                m_Area.refresh();
            }
		}
	}
	return true;
}

bool SimulationWindow::on_button_release_event(GdkEventButton * event) {
    preserve_aspect_ratio();

    if (!in_MyArea_window || disconnect_release_event)
        return false;

    if (current_mode == SELECT && !inserting_pattern && !dragging_frame)
        update_selection();

    bool selection_empty(m_Area.get_selection().empty());
    cutMi->set_sensitive(!selection_empty);
    toolbutton_cut->set_sensitive(!selection_empty);
    copyMi->set_sensitive(!selection_empty);
    toolbutton_copy->set_sensitive(!selection_empty);
    clearMi->set_sensitive(!selection_empty);

    if (current_mode == DRAW && !inserting_pattern && !dragging_frame) {
        stabilize_history();
        cmd_history.push_back(new InsertPatternCommand(0, 0, current_drawing.prev_state_screenshot,
                                                      current_drawing.pattern));
        current_drawing.prev_state_screenshot.clear();
        current_drawing.pattern.clear();
    }
    
    inserting_pattern = false;
    m_Area.set_pattern(m_Area.rebase_coords(m_Area.get_clipboard()));
    dragging_frame = false;
    update_cursor();
    button_type = NONE;
    x_0_sel_mouse = x_mouse;
    y_0_sel_mouse = y_mouse;
    updt_statusbar();
    m_Area.refresh();
    return true;
}

bool SimulationWindow::on_motion_notify_event(GdkEventMotion * event) {
    disconnect_release_event = false; // Reconnect the handler
    update_cursor();
    // raw mouse coordinates in the window frame
    double clic_x = event->x ;
    double clic_y = event->y ;

    // origin of the drawing area
    double origin_x = m_Area.get_allocation().get_x();
    double origin_y = m_Area.get_allocation().get_y();

    // get width and height of drawing area
    double width = m_Area.get_allocation().get_width();
    double height= m_Area.get_allocation().get_height();

    // retain only mouse events located within the drawing area
    if (clic_x >= origin_x && clic_x <= origin_x + width &&
        clic_y >= origin_y && clic_y <= origin_y + height) { 
        // Point that we are allowed to use expressed with drawing area coord.
        Point p({clic_x - origin_x, clic_y -origin_y});
        // Coord. calculated taking in consideration the size of the frame, to adapt when 
        // the user zooms in and translates the view frame
        int x(round((p.x*(default_frame.xMax-default_frame.xMin)/width) + default_frame.xMin));
        int y(round(default_frame.yMax - (p.y*(default_frame.yMax-default_frame.yMin)/height)));

        if (x < (int)World::get_x_max() && y < (int)World::get_y_max() && x >= 0 && y >= 0) {
            x_mouse = x;
            y_mouse = y;
            switch (button_type)
            {
            case LEFT:
                if (current_mode == DRAW && !simulation::is_alive(x, y)) {
                    simulation::new_birth(x, y);
                    current_drawing.pattern.push_back({x, y});
                }
                else if (current_mode == DRAG)
                    drag_frame();
                else if (current_mode == SELECT)
                    update_selection();
                break;
            case RIGHT:
                if (current_mode == DRAW && simulation::is_alive(x, y)) {
                    stabilize_history();
                    cmd_history.push_back(new RemoveCellCommand(x ,y));
                    cmd_history[cmd_index]->execute();
                }
                break;
            case MIDDLE:
                    drag_frame();
                break;
            case NONE:
                x_0_sel_mouse = x_mouse;
                y_0_sel_mouse = y_mouse;
                break;
            }
            updt_statusbar();
            m_Area.refresh();
        }
    }
    return true;
}

bool SimulationWindow::on_scroll_event(GdkEventScroll * scroll_event) {
    switch (scroll_event->direction) {
        case GDK_SCROLL_UP:
            on_action_zoom_in();
            break;
        case GDK_SCROLL_DOWN:
            on_action_zoom_out();
            break;
        default:
            break;
    }
    return Gtk::Window::on_scroll_event(scroll_event);
}

bool SimulationWindow::on_enter_notify_event(GdkEventCrossing * crossing_event) {
    if (crossing_event->type == GDK_ENTER_NOTIFY) {
        preserve_aspect_ratio();
        return true;
    }
    return false;
}

bool SimulationWindow::on_delete_event(GdkEventAny * any_event) {
    if (m_Button_Reset.get_sensitive()) {
        if (save_changes_dialog())
            return true;
        return false;
    }
    return false;
}

// -------
// Actions
// -------

void SimulationWindow::on_action_new() {
    val = 0;
    simulation::init();
    this->set_title(PROGRAM_NAME);

    updt_statusbar();
    m_Button_Reset.set_sensitive(false);
    saveMi->set_sensitive(false);
    toolbutton_save->set_sensitive(false);
    filename = "";

    updt_statusbar();
    m_Area.refresh();
}

void SimulationWindow::on_action_open() {
    auto dialog = Gtk::FileChooserNative::create("Open a file", Gtk::FILE_CHOOSER_ACTION_OPEN,
                                                 "_Open", "_Cancel");
    auto filter = Gtk::FileFilter::create();
    // Show Life 1.06 files only
    filter->set_name("Life 1.06");
    filter->add_pattern("*.lif");
    filter->add_pattern("*.life");
    dialog->add_filter(filter);

    int response = dialog->run();
    dialog->hide();

    if (response == Gtk::RESPONSE_ACCEPT) {
        filename = dialog->get_filename();
        // Clear the grids of booleans
        simulation::init();
        // Read data and check for any error
        int reading_result(simulation::read_file(filename));
        if (reading_result == 0) {

            on_action_reset_zoom();
            updt_statusbar();
            val = 0;

            decrsizeMi->set_sensitive(World::get_x_max() != world_size_min);
            incrsizeMi->set_sensitive(World::get_x_max() != world_size_max);
            decrsizeMi->set_sensitive(World::get_x_max() > world_size_min);
            incrsizeMi->set_sensitive(World::get_x_max() < world_size_max);
            randomMi->set_sensitive(World::get_x_max() <= 300);

            this->set_title(simulation::remove_filepath(filename) + "  -  " + PROGRAM_NAME);
        }else
            parse_file_error(reading_result);
    }
    saveMi->set_sensitive(false);
    toolbutton_save->set_sensitive(false);
    m_Button_Reset.set_sensitive(false);
    m_Area.refresh();
}

void SimulationWindow::on_action_save() {
    if (!saveMi->get_sensitive())
        return;

    simulation::save_file(filename);

    saveMi->set_sensitive(false);
    toolbutton_save->set_sensitive(false);
    m_Button_Reset.set_sensitive(false);
    this->set_title(this->get_title().replace(0, 1, ""));
}

void SimulationWindow::on_action_saveas() {
    auto dialog = Gtk::FileChooserNative::create("Save as", Gtk::FILE_CHOOSER_ACTION_SAVE, "_Save", "_Cancel");
    auto filter = Gtk::FileFilter::create();
    // Show only Life 1.06 files
    filter->add_pattern("*.lif");
    dialog->add_filter(filter);

    int response = dialog->run();
    
    if (response == Gtk::RESPONSE_ACCEPT) {
        filename = dialog->get_filename();
        simulation::save_file(filename);
        set_title(simulation::remove_filepath(filename) + " - " + this->get_title());
    }    
}

void SimulationWindow::on_action_quit() {
    close();
}

void SimulationWindow::on_action_undo() {
    if (!undoMi->get_sensitive())
        return;
    cmd_history[cmd_index]->undo();
    --cmd_index;
    redoMi->set_sensitive();
    if (cmd_index < 0)
        undoMi->set_sensitive(false);
    m_Area.refresh();
}

void SimulationWindow::on_action_redo() {
    if (!redoMi->get_sensitive())
        return;
    ++cmd_index;
    undoMi->set_sensitive();
    if (cmd_index >= cmd_history.size() - 1)
        redoMi->set_sensitive(false);
    cmd_history[cmd_index]->execute();
    m_Area.refresh();
}

void SimulationWindow::on_action_cut() {
    if (!cutMi->get_sensitive())
        return;

    stabilize_history();
    cmd_history.push_back(new CutCommand(&m_Area, m_Area.get_selection(), cutMi, toolbutton_cut,
                            copyMi, toolbutton_copy, clearMi, pasteMi, toolbutton_paste));
    cmd_history[cmd_index]->execute();

    m_Area.refresh();
    file_modified();
}

void SimulationWindow::on_action_copy() {
    if (!copyMi->get_sensitive())
        return;

    m_Area.set_clipboard(m_Area.get_selection());
    m_Area.set_pattern(m_Area.rebase_coords(m_Area.get_clipboard()));

    cutMi->set_sensitive(false);
    toolbutton_cut->set_sensitive(false);
    copyMi->set_sensitive(false);
    toolbutton_copy->set_sensitive(false);
    clearMi->set_sensitive(false);
    pasteMi->set_sensitive();
    toolbutton_paste->set_sensitive();
}

void SimulationWindow::on_action_clear() {
    if (!clearMi->get_sensitive())
        return;

    if (!m_Area.get_selection().empty()) {
        simulation::del_pattern(0, 0, m_Area.get_selection());
        m_Area.set_selection(simulation::get_live_cells_in_area(0, 0, 0, 0));
    }

    cutMi->set_sensitive(false);
    toolbutton_cut->set_sensitive(false);
    copyMi->set_sensitive(false);
    toolbutton_copy->set_sensitive(false);
    clearMi->set_sensitive(false);
    file_modified();
    updt_statusbar();
    m_Area.refresh();
}

void SimulationWindow::on_action_paste() {
    stabilize_history();
    cmd_history.push_back(new PasteCommand(rotateMi, flipverMi, fliphorMi, &inserting_pattern));
    cmd_history[cmd_index]->execute();
    update_cursor();
}

void SimulationWindow::on_action_select_all() {
    on_action_cursor_select();
    std::vector<Coordinates> sel_all(simulation::get_live_cells_in_area(0, World::get_x_max()-1,
                                                                        0, World::get_y_max()-1));
    n_selected = sel_all.size();
    m_Area.set_selection(sel_all);
    cutMi->set_sensitive();
    toolbutton_cut->set_sensitive();
    copyMi->set_sensitive();
    toolbutton_copy->set_sensitive();
    clearMi->set_sensitive();
    m_Area.refresh();
}

void SimulationWindow::on_action_insert_pattern() {
    auto dialog = Gtk::FileChooserNative::create("Insert a pattern", Gtk::FILE_CHOOSER_ACTION_OPEN,
                                                 "_Open", "_Cancel");
    auto filter = Gtk::FileFilter::create();
    filter->add_pattern("*.rle");
    dialog->add_filter(filter);
    dialog->set_current_folder(working_dir() + PATTERNS_DIR);

    disconnect_release_event = true; // Prevents the trigger of the on_release_button_event
    int response = dialog->run();
    std::string pattern_filename;
    if (response == Gtk::RESPONSE_ACCEPT) {
        pattern_filename = dialog->get_filename();
        // Read data and check for any error
        m_Area.set_pattern(simulation::get_rle_data(pattern_filename));
        inserting_pattern = true;
        update_cursor();
        m_Area.refresh();
    }
}

void SimulationWindow::on_action_random() {
    if (!randomMi->get_sensitive())
        return;

    stabilize_history();
    cmd_history.push_back(new RandomSeedCommand(simulation::get_state()));
    cmd_history[cmd_index]->execute();

    file_modified();
    updt_statusbar();
    m_Area.refresh();
}

void SimulationWindow::on_action_rotate() {
    if (inserting_pattern)
        m_Area.rotate_pattern();
    m_Area.refresh();
}

void SimulationWindow::on_action_flip_vertically() {
    if (inserting_pattern)
        m_Area.flip_pattern_up_down();
    m_Area.refresh();
}

void SimulationWindow::on_action_flip_horizontally() {
    if (inserting_pattern)
        m_Area.flip_pattern_left_right();
    m_Area.refresh();
}

void SimulationWindow::on_action_cursor_draw() {
    current_mode = DRAW;
    update_cursor();
}

void SimulationWindow::on_action_cursor_drag() {
    current_mode = DRAG;
    update_cursor();
}

void SimulationWindow::on_action_cursor_select() {
    current_mode = SELECT;
    update_cursor();
}

void SimulationWindow::on_action_zoom_in() {
    if (!zoominMi->get_sensitive())
        return;
    zoom += 10;
    zoom_frame();

    if (zoom == zoom_max) {
        zoominMi->set_sensitive(false);
        toolbutton_zoomin->set_sensitive(false);
    }
    updt_statusbar();
    zoomoutMi->set_sensitive(true);
    toolbutton_zoomout->set_sensitive(true);
    resetzoomMi->set_sensitive();
    toolbutton_resetzoom->set_sensitive();
}

void SimulationWindow::on_action_zoom_out() {
    if (!zoomoutMi->get_sensitive())
        return;
    zoom -= 10;
    zoom_frame();

    if (zoom == 100) {
        zoomoutMi->set_sensitive(false);
        toolbutton_zoomout->set_sensitive(false);
    }
    updt_statusbar();
    zoominMi->set_sensitive(true);
    toolbutton_zoomin->set_sensitive(true);
    resetzoomMi->set_sensitive();
    toolbutton_resetzoom->set_sensitive();
}

void SimulationWindow::on_action_reset_zoom() {
    zoom = default_zoom;
    zoom_frame();
    if (zoom > 100) {
        zoominMi->set_sensitive();
        zoomoutMi->set_sensitive();
        toolbutton_zoomin->set_sensitive();
        toolbutton_zoomout->set_sensitive();
    }else if (zoom == 100) {
        zoomoutMi->set_sensitive(false);
        toolbutton_zoomout->set_sensitive(false);
    }else if (zoom == zoom_max) {
        zoominMi->set_sensitive(false);
        toolbutton_zoomin->set_sensitive(false);
    }
    updt_statusbar();
}

void SimulationWindow::on_checkbutton_grid_checked() {
    bool active(false);
    m_refActionGrid->get_state(active);
    active = !active;
    m_refActionGrid->change_state(active);
    show_grid = active;
    m_Area.refresh();
}

void SimulationWindow::on_checkbutton_fade_checked() {
    simulation::toggle_fade_effect();
}

void SimulationWindow::on_checkbutton_dark_checked() {
    bool active(false);
    m_refActionDark->get_state(active);
    active = !active;
    m_refActionDark->change_state(active);
    dark_theme_on = active;

    auto settings = Gtk::Settings::get_default();
    settings->property_gtk_application_prefer_dark_theme().set_value(dark_theme_on);
    m_Area.refresh();
}

void SimulationWindow::on_button_colorscheme_clicked() {
    Gtk::Dialog color_dial("Select Color Scheme", *this, Gtk::DIALOG_MODAL);
    auto carea = color_dial.get_content_area();

    // Main horizontal box
    Gtk::HBox hbox(false, 10);
    hbox.set_border_width(10);
    hbox.set_valign(Gtk::ALIGN_CENTER);
    // Frames
    Gtk::Frame light("Light Theme");
    light.set_valign(Gtk::ALIGN_CENTER);
    Gtk::Frame dark("Dark Theme");
    dark.set_valign(Gtk::ALIGN_CENTER);

    // ComboBox adjustments
    m_ComboLight.set_border_width(2);
    m_ComboLight.set_valign(Gtk::ALIGN_CENTER);
    m_ComboLight.set_halign(Gtk::ALIGN_START);
    m_ComboDark.set_border_width(2);
    m_ComboDark.set_valign(Gtk::ALIGN_CENTER);
    m_ComboDark.set_halign(Gtk::ALIGN_START);

    light.add(m_ComboLight);
    dark.add(m_ComboDark);

    hbox.pack_start(light);
    hbox.pack_start(dark);
    carea->pack_start(hbox);
    carea->show_all_children();

    color_dial.set_transient_for(*this);
    color_dial.run();
}

void SimulationWindow::on_action_experiment() {
    bool active(false);
    m_refActionExperiment->get_state(active);
    active = !active;
    m_refActionExperiment->change_state(active);

    experiment = active;
    updt_statusbar();
}

void SimulationWindow::on_button_increase_size_clicked() {
    stabilize_history();
    cmd_history.push_back(new EnlargeWorldCommand(decrsizeMi, incrsizeMi, randomMi));
    cmd_history[cmd_index]->execute();

    set_default_frame();
    simulation::adjust_bool_grid();
    updt_statusbar();
    m_Area.refresh();
}

void SimulationWindow::on_button_decrease_size_clicked() {
    stabilize_history();
    cmd_history.push_back(new ShrinkWorldCommand(decrsizeMi, incrsizeMi, randomMi));
    cmd_history[cmd_index]->execute();

    set_default_frame();
    simulation::adjust_bool_grid();
    updt_statusbar();
    m_Area.refresh();
}

void SimulationWindow::on_action_help() {
    // Puts the file "Help.txt" into the "help" string
    std::ifstream f(working_dir() + HELP_FILE);
    std::string help;
    std::stringstream ss;
    ss << f.rdbuf();
    help = ss.str();
    // Creates a pop-up message dialog
    Gtk::MessageDialog help_dial("");
    Gtk::ScrolledWindow scrolled_win;
    Gtk::Label label_help;
    label_help.set_use_markup(true);
#ifdef _WIN32
    label_help.set_label("<span face='Consolas'>" + help + "</span>");
    // label_help.set_label(help);
#elif defined __linux__
    label_help.set_label("<span face='Dejavu Sans Mono'>" + help + "</span>");
#elif defined __APPLE__
    label_help.set_label("<span face='SF Mono'>" + help + "</span>");
#endif
    scrolled_win.add(label_help);
    scrolled_win.set_size_request(1000, 500);
    // Adds the scrollable window to the message area
    Gtk::Box* marea = help_dial.get_message_area();
    marea->pack_start(scrolled_win);

    help_dial.set_title("Help");
    help_dial.set_transient_for(*this);
    help_dial.show_all_children();
    help_dial.run();
}

void SimulationWindow::on_button_about_clicked() {
    Gtk::AboutDialog about_dial;
    const std::vector<Glib::ustring> authors(1, "Cyprien Lacassagne");
    
    about_dial.set_program_name(PROGRAM_NAME);
    about_dial.set_authors(authors);
    about_dial.set_copyright("Copyright \u00A9 2022-2023 Cyprien Lacassagne");
    about_dial.set_website("https://github.com/clacassa/Game-of-Life");
    about_dial.set_website_label("Github page");
    about_dial.set_license_type(Gtk::LICENSE_GPL_3_0);
    about_dial.set_version(VERSION);
    about_dial.set_hexpand(false);
    about_dial.set_comments("Game of Life laboratory");

    about_dial.set_transient_for(*this);
    about_dial.run();
}

void SimulationWindow::on_button_start_clicked() {
    if (m_Button_Start.get_label() == "Stop") {
        m_Button_Start.set_label("Start");

        // Change the button background color to green
        Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
        css_provider->load_from_data("button {background-image: image(green);}");
        m_Button_Start.get_style_context()->add_provider(css_provider,
            GTK_STYLE_PROVIDER_PRIORITY_USER);
        
        // Update the timeout value and stop the timer
        delete_timer();
    }else {
        m_Button_Start.set_label("Stop");

        // Change the button background color to dark red
        Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
        css_provider->load_from_data("button {background-image: image(brown);}");
        m_Button_Start.get_style_context()->add_provider(css_provider,
            GTK_STYLE_PROVIDER_PRIORITY_USER);

        // Update the timeout value and start the timer
        add_timer();
    }
}

void SimulationWindow::on_button_step_clicked() {
    if (m_Button_Start.get_label() == "Start") {
        // Do a single simulation step
        on_timeout();
    }
}

void SimulationWindow::on_button_reset_clicked() {
    val = 0;
    m_Button_Reset.set_sensitive(false);
    saveMi->set_sensitive(false);
    toolbutton_save->set_sensitive(false);
    simulation::init();
    simulation::read_file(filename);
    set_default_frame();
    zoom_frame();
    this->set_title(this->get_title().replace(0, 1, ""));
    updt_statusbar();
    m_Area.refresh();
}

void SimulationWindow::on_button_slower_clicked() {
    switch (timeout_value) {
        case refresh_min:
            timeout_value = 10;
            m_Button_Faster.set_sensitive();
            break;
        case 10:
            timeout_value = 20;
            break;
        case 20:
            timeout_value = 50;
            break;
        case 50:
            timeout_value = 100;
            break;
        case 100:
            timeout_value = refresh_max;
            m_Button_Slower.set_sensitive(false);
            break;
        default:
            break;
    }
    m_Label_Refresh.set_text("Refresh : " + std::to_string(timeout_value) + " ms");
    if (timer_added) {
        delete_timer();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        add_timer();
    }
}

void SimulationWindow::on_button_faster_clicked() {
    switch (timeout_value) {
        case refresh_max:
            timeout_value = 100;
            m_Button_Slower.set_sensitive();
            break;
        case 100:
            timeout_value = 50;
            break;
        case 50:
            timeout_value = 20;
            break;
        case 20:
            timeout_value = 10;
            break;
        case 10:
            timeout_value = refresh_min;
            m_Button_Faster.set_sensitive(false);
            break;
        default:
            break;
    }
    m_Label_Refresh.set_text("Refresh : " + std::to_string(timeout_value) + " ms");
    if (timer_added) {
        delete_timer();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        add_timer();
    }
}

/* 
 * ===================================================================================
 * Widgets layout, accelerators, menubar, toolbar and statusbar setup
 * ===================================================================================
 */ 

void SimulationWindow::create_action_groups(Glib::RefPtr<Gtk::Application> app) {
    auto m_refFileActionGroup = Gio::SimpleActionGroup::create();
    m_refFileActionGroup->add_action("new", sigc::mem_fun(*this,
            &SimulationWindow::on_action_new));

    m_refFileActionGroup->add_action("open", sigc::mem_fun(*this,
                &SimulationWindow::on_action_open));

    m_refFileActionGroup->add_action("save", sigc::mem_fun(*this,
            &SimulationWindow::on_action_save));

    m_refFileActionGroup->add_action("saveas", sigc::mem_fun(*this,
            &SimulationWindow::on_action_saveas));

    m_refFileActionGroup->add_action("quit", sigc::mem_fun(*this,
            &SimulationWindow::on_action_quit));
        
    auto m_refEditActionGroup = Gio::SimpleActionGroup::create();
    m_refEditActionGroup->add_action("undo", sigc::mem_fun(*this,
            &SimulationWindow::on_action_undo));
    m_refEditActionGroup->add_action("redo", sigc::mem_fun(*this,
            &SimulationWindow::on_action_redo));
    m_refEditActionGroup->add_action("cut", sigc::mem_fun(*this,
            &SimulationWindow::on_action_cut));

    m_refEditActionGroup->add_action("copy", sigc::mem_fun(*this,
            &SimulationWindow::on_action_copy));

    m_refEditActionGroup->add_action("clear", sigc::mem_fun(*this,
            &SimulationWindow::on_action_clear));

    m_refEditActionGroup->add_action("paste", sigc::mem_fun(*this,
            &SimulationWindow::on_action_paste));

    m_refEditActionGroup->add_action("selectall", sigc::mem_fun(*this,
            &SimulationWindow::on_action_select_all));

    m_refEditActionGroup->add_action("random", sigc::mem_fun(*this,
            &SimulationWindow::on_action_random));

    m_refEditActionGroup->add_action("rotate", sigc::mem_fun(*this,
            &SimulationWindow::on_action_rotate));

    m_refEditActionGroup->add_action("flipver", sigc::mem_fun(*this,
            &SimulationWindow::on_action_flip_vertically));

    m_refEditActionGroup->add_action("fliphor", sigc::mem_fun(*this,
            &SimulationWindow::on_action_flip_horizontally));

    auto m_refViewActionGroup = Gio::SimpleActionGroup::create();
    m_refViewActionGroup->add_action("zoomin", sigc::mem_fun(*this,
            &SimulationWindow::on_action_zoom_in));

    m_refViewActionGroup->add_action("zoomout", sigc::mem_fun(*this,
            &SimulationWindow::on_action_zoom_out));

    m_refViewActionGroup->add_action("resetzoom", sigc::mem_fun(*this,
            &SimulationWindow::on_action_reset_zoom));

    m_refViewActionGroup->add_action("fade", sigc::mem_fun(*this,
            &SimulationWindow::on_checkbutton_fade_checked));

    m_refViewActionGroup->add_action("scheme", sigc::mem_fun(*this,
            &SimulationWindow::on_button_colorscheme_clicked));

    auto m_refToolsActionGroup = Gio::SimpleActionGroup::create();
    m_refToolsActionGroup->add_action("experiment", sigc::mem_fun(*this,
            &SimulationWindow::on_action_experiment));

    m_refToolsActionGroup->add_action("increasesize", sigc::mem_fun(*this,
            &SimulationWindow::on_button_increase_size_clicked));

    m_refToolsActionGroup->add_action("decreasesize", sigc::mem_fun(*this,
            &SimulationWindow::on_button_decrease_size_clicked));

    m_refToolsActionGroup->add_action("pattern", sigc::mem_fun(*this,
            &SimulationWindow::on_action_insert_pattern));

    auto m_refHelpActionGroup = Gio::SimpleActionGroup::create();
    m_refHelpActionGroup->add_action("help", sigc::mem_fun(*this,
            &SimulationWindow::on_action_help));

    m_refHelpActionGroup->add_action("about", sigc::mem_fun(*this,
            &SimulationWindow::on_button_about_clicked));

    m_refToggleActionGroup = Gio::SimpleActionGroup::create();
    m_refActionGrid = m_refToggleActionGroup->add_action_bool("grid",
        sigc::mem_fun(*this, &SimulationWindow::on_checkbutton_grid_checked), show_grid);

    m_refActionDark = m_refToggleActionGroup->add_action_bool("dark",
        sigc::mem_fun(*this, &SimulationWindow::on_checkbutton_dark_checked), dark_theme_on);

    m_refActionExperiment = m_refToggleActionGroup->add_action_bool("experiment",
        sigc::mem_fun(*this, &SimulationWindow::on_action_experiment), false);

    m_refCursorMode = m_refToggleActionGroup->add_action_radio_integer("mode",
        sigc::mem_fun(*this, &SimulationWindow::on_action_cursor_mode_changed), 1);

    insert_action_group("file", m_refFileActionGroup);
    app->set_accel_for_action("file.new", "<Primary>n");
    app->set_accel_for_action("file.open", "<Primary>o");
    app->set_accel_for_action("file.save", "<Primary>s");
    app->set_accel_for_action("file.saveas", "<Primary><Shift>s");
    app->set_accel_for_action("file.quit", "<Primary>q");
    insert_action_group("edit", m_refEditActionGroup);
    app->set_accel_for_action("edit.undo", "<Primary>z");
    app->set_accel_for_action("edit.redo", "<Primary><Shift>z");
    app->set_accel_for_action("edit.cut", "<Primary>x");
    app->set_accel_for_action("edit.copy", "<Primary>c");
    app->set_accel_for_action("edit.paste", "<Primary>v");
    app->set_accel_for_action("edit.selectall", "<Primary>a");
    app->set_accel_for_action("edit.clear", "Delete");
    app->set_accel_for_action("edit.random", "<Primary>r");
    app->set_accel_for_action("edit.rotate", "r");
    app->set_accel_for_action("edit.flipver", "y");
    app->set_accel_for_action("edit.fliphor", "x");
    insert_action_group("view", m_refViewActionGroup);
    app->set_accel_for_action("view.zoomin", "<Primary>plus");
    app->set_accel_for_action("view.zoomout", "<Primary>minus");
    app->set_accel_for_action("view.resetzoom", "0");
    insert_action_group("tools", m_refToolsActionGroup);
    app->set_accel_for_action("tools.increasesize", "<Primary>Page_Up");
    app->set_accel_for_action("tools.decreasesize", "<Primary>Page_Down");
    app->set_accel_for_action("tools.pattern", "<Primary>p");
    insert_action_group("help", m_refHelpActionGroup);
    app->set_accel_for_action("help.help", "F1");
    insert_action_group("toggle", m_refToggleActionGroup);
    app->set_accel_for_action("toggle.grid", "<Primary>g");
    app->set_accel_for_action("toggle.dark", "<Primary>d");
    app->set_accel_for_action("toggle.experiment", "<Primary>t");
}

void SimulationWindow::instantiate_menubar_from_glade() {
    m_refBuilder = Gtk::Builder::create();

    try {
        m_refBuilder->add_from_file(working_dir() + "src/actions_ui.glade");
    }
    catch(const Glib::Error& ex) {
        std::cerr << "Building menus and toolbar failed: " << ex.what();
    }

    m_MenuBar = nullptr;
    m_refBuilder->get_widget("menubar", m_MenuBar);
    if (!m_MenuBar)
        g_warning("GtkMenuBar not found");
    else
        m_SuperBox.pack_start(*m_MenuBar, Gtk::PACK_SHRINK);

    saveMi = nullptr;
    m_refBuilder->get_widget("saveMi", saveMi);
    if (!saveMi)
        g_warning("GtkMenuItem not found: saveMi");
    saveMi->set_sensitive(false);

    undoMi = nullptr;
    m_refBuilder->get_widget("undoMi", undoMi);
    if (!undoMi)
        g_warning("GtkMenuItem not found");
    undoMi->set_sensitive(false);

    redoMi = nullptr;
    m_refBuilder->get_widget("redoMi", redoMi);
    if (!redoMi)
        g_warning("GtkMenuItem not found");
    redoMi->set_sensitive(false);

    cutMi = nullptr;
    m_refBuilder->get_widget("cutMi", cutMi);
    if (!cutMi)
        g_warning("GtkMenuItem not found");
    cutMi->set_sensitive(false);

    copyMi = nullptr;
    m_refBuilder->get_widget("copyMi", copyMi);
    if (!copyMi)
        g_warning("GtkMenuItem not found: copyMi");
    copyMi->set_sensitive(false);

    clearMi = nullptr;
    m_refBuilder->get_widget("clearMi", clearMi);
    if (!clearMi)
        g_warning("GtkMenuItem not found: clearMi");
    clearMi->set_sensitive(false);

    pasteMi = nullptr;
    m_refBuilder->get_widget("pasteMi", pasteMi);
    if (!pasteMi)
        g_warning("GtkMenuItem not found: pasteMi");
    pasteMi->set_sensitive(false);

    randomMi = nullptr;
    m_refBuilder->get_widget("randomMi", randomMi);
    if (!randomMi)
        g_warning("GtkMenuItem not found: randomMi");
    randomMi->set_sensitive(World::get_x_max() <= 1000);

    rotateMi = nullptr;
    m_refBuilder->get_widget("rotateMi", rotateMi);
    if (!rotateMi)
        g_warning("GtkMenuItem not found: rotateMi");
    rotateMi->set_sensitive(false);

    flipverMi = nullptr;
    m_refBuilder->get_widget("flipverMi", flipverMi);
    if (!flipverMi)
        g_warning("GtkMenuItem not found: flipverMi");
    flipverMi->set_sensitive(false);

    fliphorMi = nullptr;
    m_refBuilder->get_widget("fliphorMi", fliphorMi);
    if (!fliphorMi)
        g_warning("GtkMenuItem not found: fliphorMi");
    fliphorMi->set_sensitive(false);

    showgridMi = nullptr;
    m_refBuilder->get_widget("showgridMi", showgridMi);
    if (!showgridMi)
        g_warning("GtkCheckButton not found: showgridMi");

    fadeMi = nullptr;
    m_refBuilder->get_widget("fadeMi", fadeMi);
    if (!fadeMi)
        g_warning("GtkCheckButton not found: fadeMi");

    darkMi = nullptr;
    m_refBuilder->get_widget("darkMi", darkMi);
    if (!darkMi)
        g_warning("GtkCheckButton not found: darkMi");

    experimentMi = nullptr;
    m_refBuilder->get_widget("experimentMi", experimentMi);
    if (!experimentMi)
        g_warning("GtkCheckButton not found: experimentMi");
    experimentMi->set_sensitive(true);
    
    incrsizeMi = nullptr;
    m_refBuilder->get_widget("increasesizeMi", incrsizeMi);
    if (!incrsizeMi)
        g_warning("GtkMenuItem not found: increasesizeMi");
    if (World::get_x_max() >= world_size_max)
        incrsizeMi->set_sensitive(false);

    decrsizeMi = nullptr;
    m_refBuilder->get_widget("decreasesizeMi", decrsizeMi);
    if (!decrsizeMi)
        g_warning("GtkMenuItem not found: decreasesizeMi");
    if (World::get_x_max() <= world_size_min)
        decrsizeMi->set_sensitive(false);

    zoominMi = nullptr;
    m_refBuilder->get_widget("zoominMi", zoominMi);
    if (!zoominMi)
        g_warning("GtkMenuItem not found: zoominMi");
    zoominMi->set_sensitive(zoom < zoom_max);
    
    zoomoutMi = nullptr;
    m_refBuilder->get_widget("zoomoutMi", zoomoutMi);
    if (!zoomoutMi)
        g_warning("GtkMenuItem not found: zoomoutMi");
    zoomoutMi->set_sensitive(zoom > zoom_min);

    resetzoomMi = nullptr;
    m_refBuilder->get_widget("resetzoomMi", resetzoomMi);
    if (!resetzoomMi)
        g_warning("GtkMenuItem not found: resetzoomMi");
    resetzoomMi->set_sensitive(false);
}

void SimulationWindow::instantiate_toolbar_from_glade() {
    m_ToolBar = nullptr;
    m_refBuilder->get_widget("toolbar", m_ToolBar);
    if (!m_ToolBar)
        g_warning("GtkToolbar not found: toolbar");
    else
        m_SuperBox.pack_start(*m_ToolBar, Gtk::PACK_SHRINK);
    m_ToolBar->set_can_focus(false);

    toolbutton_new = nullptr;
    m_refBuilder->get_widget("toolbutton_new", toolbutton_new);
    if (!toolbutton_new)
        g_warning("GtkToolButton not found: toolbutton_new");
    toolbutton_new->set_can_focus(false);

    toolbutton_save = nullptr;
    m_refBuilder->get_widget("toolbutton_save", toolbutton_save);
    if (!toolbutton_save)
        g_warning("GtkToolButton not found: toolbutton_save");
    toolbutton_save->set_sensitive(false);

    toolbutton_cut = nullptr;
    m_refBuilder->get_widget("toolbutton_cut", toolbutton_cut);
    if (!toolbutton_cut)
        g_warning("GtkToolButton not found: toolbutton_cut");
    toolbutton_cut->set_sensitive(false);

    toolbutton_copy = nullptr;
    m_refBuilder->get_widget("toolbutton_copy", toolbutton_copy);
    if (!toolbutton_copy)
        g_warning("GtkToolButton not found: toolbutton_copy");
    toolbutton_copy->set_sensitive(false);

    toolbutton_paste = nullptr;
    m_refBuilder->get_widget("toolbutton_paste", toolbutton_paste);
    if (!toolbutton_paste)
        g_warning("GtkToolButton not found: toolbutton_paste");
    toolbutton_paste->set_sensitive(false);

    toolbutton_zoomin = nullptr;
    m_refBuilder->get_widget("toolbutton_zoomin", toolbutton_zoomin);
    if (!toolbutton_zoomin)
        g_warning("GtkToolButton not found: toolbutton_zoomin");
    toolbutton_zoomin->set_sensitive(zoom < zoom_max);
    
    toolbutton_zoomout = nullptr;
    m_refBuilder->get_widget("toolbutton_zoomout", toolbutton_zoomout);
    if (!toolbutton_zoomout)
        g_warning("GtkToolButton not found: toolbutton_zoomout");
    toolbutton_zoomout->set_sensitive(zoom > zoom_min);

    toolbutton_resetzoom = nullptr;
    m_refBuilder->get_widget("toolbutton_resetzoom", toolbutton_resetzoom);
    if (!toolbutton_resetzoom)
        g_warning("GtkToolButton not found: toolbutton_resetzoom");

    toggletoolbutton_experiment = nullptr;
    m_refBuilder->get_widget("toggletoolbutton_experiment", toggletoolbutton_experiment);
    if (!toggletoolbutton_experiment)
        g_warning("GtkToggleToolButton not found: toggletoolbutton_experiment");
}

void SimulationWindow::create_control_buttons() {
    // Create Start button
    m_ButtonBox.pack_start(m_Button_Start);
    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
    css_provider->load_from_data("button {background-image: image(green);}");
    m_Button_Start.get_style_context()->add_provider(css_provider,
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    m_Button_Start.override_color(Gdk::RGBA("white"));

    m_Button_Start.set_property("can-focus", false);
    m_Button_Start.signal_clicked().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_start_clicked));

    // Create Step button
    m_ButtonBox.pack_start(m_Button_Step);
    m_Button_Step.set_property("can-focus", false);
    m_Button_Step.signal_clicked().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_step_clicked));

    // Create Reset button
    m_ButtonBox.pack_start(m_Button_Reset);
    m_Button_Reset.set_property("can-focus", false);
    m_Button_Reset.set_sensitive(false);
    m_Button_Reset.signal_clicked().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_reset_clicked));

    // Create a separator
    m_ButtonBox.pack_start(m_Separator2);
    m_Separator2.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

    // Create speed control buttons
    m_RefreshButtonBox.pack_start(m_Button_Faster, false, false);
    m_RefreshButtonBox.pack_start(m_Button_Slower, false, false);
    m_RefreshButtonBox.pack_start(m_Label_Refresh);

    m_Button_Slower.set_property("can-focus", false);
    m_Button_Slower.set_hexpand(false);
    m_Button_Slower.signal_clicked().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_slower_clicked));

    m_Button_Faster.set_property("can-focus", false);
    m_Button_Faster.set_hexpand(false);
    m_Button_Faster.signal_clicked().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_faster_clicked));
}

void SimulationWindow::create_StatusBar() {
    m_StatusBar.set_valign(Gtk::ALIGN_END);
    m_StatusBar.set_vexpand(false);
    m_StatusBar.set_halign(Gtk::ALIGN_END);
    m_StatusBar.set_hexpand();
    updt_statusbar();
}

void SimulationWindow::create_ComboBoxes() {
    m_refTreeModel = Gtk::ListStore::create(m_Columns);
    m_ComboLight.set_model(m_refTreeModel);
    m_ComboDark.set_model(m_refTreeModel);

    // Fill combo tree model
    auto iter = m_refTreeModel->append();
    auto row = *iter;
    
    row[m_Columns.m_col_id] = 1;
    row[m_Columns.m_col_name] = "DefaultLight";
    m_ComboLight.set_active(iter);

    iter = m_refTreeModel->append();
    row = *iter;
    row[m_Columns.m_col_id] = 2;
    row[m_Columns.m_col_name] = "DefaultDark";
    m_ComboDark.set_active(iter);

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_id] = 3;
    row[m_Columns.m_col_name] = "Jade";

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_id] = 4;
    row[m_Columns.m_col_name] = "Basalt";

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_id] = 5;
    row[m_Columns.m_col_name] = "Asphalt";

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_id] = 6;
    row[m_Columns.m_col_name] = "Albaster";

    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_id] = 7;
    row[m_Columns.m_col_name] = "Deep Blue";
    
    // Add the model columns to combo
    m_ComboLight.pack_start(m_Columns.m_col_id);
    m_ComboLight.pack_start(m_Columns.m_col_name);

    m_ComboLight.pack_start(m_cell);

    m_ComboLight.signal_changed().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_combo_light_changed));

    m_ComboDark.pack_start(m_Columns.m_col_id);
    m_ComboDark.pack_start(m_Columns.m_col_name);

    m_ComboDark.pack_start(m_cell);

    m_ComboDark.signal_changed().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_combo_dark_changed));
}

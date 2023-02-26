/*
 *  gui.cc -- GoL Lab -- GUI with various options and view controls
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

#include <iostream>
#include <fstream>
#include <string>
#include <cairomm/context.h>
#include <cstdlib>
#include <filesystem>
#include "gui.h"
#include "graphic_gui.h"
#include "config.h"

namespace fs = std::filesystem;

static Frame default_frame = {-1, Conf::world_size, -1, Conf::world_y_max,
                                                       1, window_size, window_size};

static void orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr,
                                                                        Frame frame);

static unsigned val(0);
static unsigned zoom(100);
static unsigned z_ratio(100);

static bool dark_theme_on(false);
static bool show_grid(false);

static bool inserting_pattern(false);
static bool dragging_frame(false);
static bool in_MyArea_window(false);
static Action current_action(DRAW);
// Mouse coordinates in the drawing area
static unsigned x_mouse, y_mouse, x_0_sel_mouse, y_0_sel_mouse;

void set_default_frame() {
    default_frame.xMin = -1;
    default_frame.xMax = Conf::world_size;
    default_frame.yMin = -1;
    default_frame.yMax = Conf::world_y_max;
}

//===========================================

MyArea::MyArea()
:   empty(false) {
    this->signal_enter_notify_event().connect(sigc::mem_fun(*this,
            &MyArea::on_enter_notify_event));
    this->signal_leave_notify_event().connect(sigc::mem_fun(*this,
            &MyArea::on_leave_notify_event));
    this->set_events(Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK);
}

MyArea::~MyArea() {
    setFrame(default_frame);
}

void MyArea::clear() {
    empty = true;
    refresh();
}

void MyArea::draw() {
    empty = false;
    refresh();
}

void MyArea::refresh() {
    auto win = get_window();
    if (win) {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                               get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
}

void MyArea::setFrame(Frame f)
{
	if((f.xMin <= f.xMax) and (f.yMin <= f.yMax) and (f.height > 0)) {
		f.asp = f.width/f.height;
		frame = f;
	}
	else {
		std::cout << "incorrect Model framing or window parameters\n";
    }
}

void MyArea::adjustFrame()
{
	Gtk::Allocation allocation = get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();

	
	frame.width  = width;
	frame.height = height;

    double new_aspect_ratio((double)width/height);
    if( new_aspect_ratio > default_frame.asp)
    {
	    frame.yMax = default_frame.yMax ;
	    frame.yMin = default_frame.yMin ;
	  
	    double delta(default_frame.xMax - default_frame.xMin);
	    double mid((default_frame.xMax + default_frame.xMin)/2);
        
	    frame.xMax = mid + 0.5*(new_aspect_ratio/default_frame.asp)*delta ;
	    frame.xMin = mid - 0.5*(new_aspect_ratio/default_frame.asp)*delta ;
    }
    else
    {
	    frame.xMax = default_frame.xMax ;
	    frame.xMin = default_frame.xMin ;

 	    double delta(default_frame.yMax - default_frame.yMin);
	    double mid((default_frame.yMax + default_frame.yMin)/2);

	    frame.yMax = mid + 0.5*(default_frame.asp/new_aspect_ratio)*delta ;
	    frame.yMin = mid - 0.5*(default_frame.asp/new_aspect_ratio)*delta ;
    }
}

static void orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr,
                                                                Frame frame) {
	cr->translate(frame.width/2, frame.height/2);

	cr->scale(frame.width/(frame.xMax - frame.xMin),
             -frame.height/(frame.yMax - frame.yMin));

	cr->translate(-(frame.xMin + frame.xMax)/2, -(frame.yMin + frame.yMax)/2);
}

void MyArea::draw_frame(const Cairo::RefPtr<Cairo::Context>& cr) {

    if (!dark_theme_on) {
        cr->set_source_rgb(gray1.r, gray1.g, gray1.b);
    }else {
        cr->set_source_rgb(0.1, 0.1, 0.1);
    }
    cr->paint();
    cr->stroke();
}

bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    if (!empty) {
        graphic_set_context(cr);

        draw_frame(cr);
        adjustFrame();
        orthographic_projection(cr, frame);

        graphic_draw_world(Conf::world_size, Conf::world_y_max, dark_theme_on, show_grid, default_frame.xMax-default_frame.xMin);
        draw_cells(dark_theme_on);

        if (inserting_pattern) {
            graphic_ghost_pattern(x_mouse, y_mouse, pattern, dark_theme_on);
            refresh();
            return true;
        }
        if (dragging_frame)
            return true;
        if (current_action == SELECT) {
            graphic_draw_select_rec(x_0_sel_mouse, y_0_sel_mouse, x_mouse, y_mouse, dark_theme_on);
            graphic_highlight_selected_cells(selection, dark_theme_on);
        }

        refresh();
    }
    return true;
}

void MyArea::flip_pattern_left_right() {
    unsigned width(pattern_width());
        for (auto& e : pattern) {
            e.x = width - e.x;
        }
}

void MyArea::flip_pattern_up_down() {
    unsigned height(pattern_height());
    for (auto& e : pattern) {
        e.y = height - e.y;
    }
}

void MyArea::rotate_pattern() {
    unsigned tmp;
    for (auto& e : pattern) {
        tmp = e.x;
        e.x = e.y;
        e.y = tmp;
    }
}

std::vector<Pos> MyArea::rebase_coords(std::vector<Pos> abs_coords) {
    unsigned x_min(Conf::world_size);
    unsigned y_min(Conf::world_y_max);
    for (auto& e : abs_coords) {
        if (e.x < x_min) x_min = e.x;
        if (e.y < y_min) y_min = e.y;
    }
    for (auto& e : abs_coords) {
        e.x -= x_min;
        e.y -= y_min;
    }
    return abs_coords;
}

unsigned MyArea::pattern_width() {
    unsigned max(0);
    unsigned min(Conf::world_size);
    for (unsigned i(0); i < pattern.size(); ++i) {
        if (pattern[i].x > max) max = pattern[i].x;
        if (pattern[i].x < min) min = pattern[i].x;
    }
    return max-min;
}

unsigned MyArea::pattern_height() {
    unsigned max(0);
    unsigned min(Conf::world_y_max);
    for (unsigned i(0); i < pattern.size(); ++i) {
        if (pattern[i].y > max) max = pattern[i].y;
        if (pattern[i].y < min) min = pattern[i].y;
    }
    return max-min;
}

bool MyArea::on_enter_notify_event(GdkEventCrossing * crossing_event) {
    if (crossing_event->type == GDK_ENTER_NOTIFY) {
        in_MyArea_window = true;
        return true;
    }
    return false;
}

bool MyArea::on_leave_notify_event(GdkEventCrossing * crossing_event) {
    if (crossing_event->type == GDK_LEAVE_NOTIFY) {
        in_MyArea_window = false;
        return true;
    }
    return false;
}

/**  
 *  ===========================================
 *  SimulationWindow constructor: TopLevel window initialization.
 *  Manages all signal handlers and events.
 *  ===========================================
*/
SimulationWindow::SimulationWindow(Glib::RefPtr<Gtk::Application> app, std::string __filename)
:   m_SuperBox(Gtk::ORIENTATION_VERTICAL),
    m_Box(Gtk::ORIENTATION_HORIZONTAL),
    m_Box_General(Gtk::ORIENTATION_VERTICAL, 15),
    m_ButtonBox(Gtk::ORIENTATION_VERTICAL, 5),
    m_Frame_Speed("Refresh (ms)"),
    m_Button_Start("Start"),
    m_Button_Step("Step"),
    m_Button_Reset("Reset"),
    m_Button_Clear("Clear"),
    m_Button_Random("Random"),
    m_Scale(Gtk::ORIENTATION_HORIZONTAL),
    m_Label_Theme("dark"),
    timer_added(false),
    disconnect(false),
    experiment(false),
    timeout_value(startup_timeout_value),
    frame_surface(default_frame.xMax*default_frame.yMax),
    filename(__filename),
    x(std::to_string(default_frame.xMax / 2)),
    y(std::to_string(default_frame.yMax / 2)),
    button_type(NONE)
{
    set_default_size(window_size*1.07, window_size);
    set_position(Gtk::WIN_POS_CENTER);
    set_default_icon_from_file(PROGRAM_ICON);
    set_resizable(true);
    parse_file_error();

    m_Box.set_margin_top(2);
    m_Box.set_margin_right(2);
    m_Box_General.set_margin_left(5);
    m_Box_General.set_margin_right(5);
    m_Box_General.set_size_request(130);

    this->set_events(Gdk::POINTER_MOTION_MASK);

    add(m_SuperBox);

    create_main_action_group(app);
    instanciate_menubar_from_glade();
    instanciante_toolbar_from_glade();

    m_SuperBox.pack_start(m_Separator, false, false);
    m_SuperBox.pack_start(m_Box);
    m_SuperBox.pack_start(m_StatusBar, false, false);
    m_Box.pack_start(m_Box_General, false, false);
    m_Box.pack_start(m_Area);
    m_Box_General.pack_start(m_ButtonBox, false, false);
    m_Box_General.pack_start(m_Frame_Speed, false, false);
    m_Frame_Speed.add(m_Scale);
    m_Frame_Speed.set_shadow_type(Gtk::SHADOW_ETCHED_OUT);

    m_Separator.set_valign(Gtk::ALIGN_START);
    m_Separator.set_vexpand(false);

    create_refresh_scale();
    create_control_buttons();
    create_StatusBar();
    create_ComboBoxes();
    create_patterns_ComboBox();

    if (showgridMi->get_active()) {
        show_grid = true;
    }
    showToolbarMi->set_active();
    showStatusBarMi->set_active();
    m_ToolBar->set_visible();
    m_StatusBar.set_visible();

    read_settings();

    show_all_children();
}

void SimulationWindow::zoom_frame() {
    if (z_ratio == 0) {
        default_frame.xMax -= 10;
        default_frame.yMax -= 10;
        return;
    }

    // Calculate the target frame's dimensions
    unsigned target_surface(Conf::world_size * Conf::world_y_max * z_ratio / 100);
    unsigned target_side(sqrt(target_surface));
    double xMaxNew = default_frame.xMax + (target_side - (default_frame.xMax - default_frame.xMin)) / 2;
    double xMinNew = default_frame.xMin - (target_side - (default_frame.xMax - default_frame.xMin)) / 2;
    double yMaxNew = default_frame.yMax + (target_side - (default_frame.yMax - default_frame.yMin)) / 2;
    double yMinNew = default_frame.yMin - (target_side - (default_frame.yMax - default_frame.yMin)) / 2;

    // Reset zoom level
    if (z_ratio == 100) {
        set_default_frame();
        return;
    }

    // The four corners of the frame must stay inside the initial frame
    if (xMaxNew > Conf::world_size) {
        xMinNew -= (target_side - (default_frame.xMax - default_frame.xMin));
        xMaxNew = default_frame.xMax;
    }
    if (xMinNew < -1) {
        xMaxNew += target_side - (default_frame.xMax - default_frame.xMin);
        xMinNew = default_frame.xMin;
    }
    if (yMaxNew > Conf::world_y_max) {
        yMinNew -= (target_side - (default_frame.yMax - default_frame.yMin));
        yMaxNew = default_frame.yMax;
    }
    if (yMinNew < -1) {
        yMaxNew += target_side - (default_frame.yMax - default_frame.yMin);
        yMinNew = default_frame.yMin;
    }
    // Update the position of the four corners
    default_frame.xMax = xMaxNew;
    default_frame.xMin = xMinNew;
    default_frame.yMax = yMaxNew;
    default_frame.yMin = yMinNew;
}

void SimulationWindow::updt_statusbar_coord() {
    x = std::to_string((default_frame.xMax + (int)default_frame.xMin) / 2 + 1);
    y = std::to_string((default_frame.yMax + (int)default_frame.yMin) / 2 + 1);
    m_LabelCoordinates.set_text("x=" + x + "    y=" + y);
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

void SimulationWindow::on_action_quit() {
    hide();
}

void SimulationWindow::on_button_start_clicked() {
    if (m_Button_Start.get_label() == "Stop") 
    {
        m_Button_Start.set_label("Start");

        // Change the button background color to green
        Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
        css_provider->load_from_data("button {background-image: image(green);}");
        m_Button_Start.get_style_context()->add_provider(css_provider,
            GTK_STYLE_PROVIDER_PRIORITY_USER);
        
        // Update the timeout value and stop the timer
        timeout_value = m_Scale.get_value();
        on_event_delete_timer();
    }
    else 
    {
        m_Button_Start.set_label("Stop");

        // Change the button background color to dark red
        Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
        css_provider->load_from_data("button {background-image: image(brown);}");
        m_Button_Start.get_style_context()->add_provider(css_provider,
            GTK_STYLE_PROVIDER_PRIORITY_USER);

        // Update the timeout value and start the timer
        timeout_value = m_Scale.get_value();
        on_event_add_timer();
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
    init();
    read_file(filename);
    set_default_frame();
    this->set_title(this->get_title().replace(0, 1, ""));
    m_Label_Info.set_label("<b>Generation=" + std::to_string(val) + "</b>");
    m_Label_Population.set_label("<b>Population=" + std::to_string(get_alive()) + "</b>");
    m_LabelSize.set_text(std::to_string(Conf::world_size) + "x" + std::to_string(Conf::world_y_max));
}

void SimulationWindow::on_action_random() {
    init();
    file_modified();
    if (experiment) {
        m_Label_Test.set_visible(true);
        m_Label_Test.set_label("<span foreground='blue'>S.D. ON</span>");
        m_Label_Population.set_label("<b>Population=" + std::to_string(get_alive()) + "</b>");
    }else {
        m_Label_Test.set_visible(false);
    }
    
    // Randomly fill half of the grid
    unsigned rand_x, rand_y;
    for (unsigned index(0); index < (Conf::world_size*Conf::world_y_max)/2; ++index) {
        rand_x = rand() % Conf::world_size;
        rand_y = rand() % Conf::world_y_max;
        new_birth(rand_x, rand_y);
    }
    m_Area.refresh();
}

void SimulationWindow::on_action_open() {

    auto dialog = Gtk::FileChooserNative::create("Open a file", Gtk::FILE_CHOOSER_ACTION_OPEN, "_Open", "_Cancel");
    auto filter = Gtk::FileFilter::create();
    // Show text files only
    filter->add_pattern("*.txt");
    dialog->add_filter(filter);
    int response = dialog->run();
    dialog->hide();

    if (response == Gtk::RESPONSE_ACCEPT) {
        filename = dialog->get_filename();
        // Clear the grids of booleans
        init();
        // Read data and check for any error
        if (read_file(filename)) {

            on_action_reset_zoom();
            m_Label_Info.set_label("<span weight='bold' > Generation : " + std::to_string(val) + "</span>");
            m_Label_Population.set_label("<b>Population=" + std::to_string(get_alive()) + "</b>");
            m_LabelSize.set_text(std::to_string(Conf::world_size) + " x "+ std::to_string(Conf::world_y_max));
            if (Conf::world_size == world_size_min) decrsizeMi->set_sensitive(false);
            if (Conf::world_size == world_size_max) incrsizeMi->set_sensitive(false);
            if (Conf::world_size > world_size_min) decrsizeMi->set_sensitive(true);
            if (Conf::world_size < world_size_max) incrsizeMi->set_sensitive(true);

            unsigned pos = filename.find_last_of('\\');
            std::string flnm = filename;
            this->set_title(flnm.replace(0, pos + 1, "") + "  -  " + PROGRAM_NAME);
        }else {
            parse_file_error();
        }
    }
    saveMi->set_sensitive(false);
    toolbutton_save->set_sensitive(false);
    m_Button_Reset.set_sensitive(false);
    m_Area.refresh();
}

void SimulationWindow::parse_file_error() {
    switch (get_error_id())
    {
    case 1:
        error_dialog_open(ERR_OPEN_FILE);
        break;
    case 3:
        error_dialog_open(ERR_CONFIG_FILE);
        break;
    case 4:
        error_dialog_open(ERR_EXT_FILE);
        break;
    default:
        break;
    }
}

void SimulationWindow::error_dialog_open(std::string error_message) {
    Gtk::MessageDialog error_dialog(error_message, true, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
    // // Font options
    // if (!dark_theme_on) {
    //     error_dialog.set_message("<span size='large' foreground='#000099' letter_spacing='1024'>"
    //                     + error_message + "</span>", true);
    // }else {
    //     error_dialog.set_message("<span size='large' foreground='#C0C0C0' letter_spacing='1024'>"
    //                     + error_message + "</span>", true);
    // }
    error_dialog.set_message(error_message);
    std::vector<Gtk::Widget*> children = error_dialog.get_action_area()->get_children();
    children[0]->set_margin_left(dialog_button_margin);              
    error_dialog.set_icon_from_file("share/icons/notifications.ico");
    error_dialog.set_transient_for(*this);
    error_dialog.run();
    filename = "";
    this->set_title(PROGRAM_NAME);
}

void SimulationWindow::on_action_save() {
    if (!saveMi->get_sensitive()) return;
    save_file(filename);
    saveMi->set_sensitive(false);
    toolbutton_save->set_sensitive(false);
    m_Button_Reset.set_sensitive(false);
    this->set_title(this->get_title().replace(0, 1, ""));
}

void SimulationWindow::on_action_saveas() {
    auto dialog = Gtk::FileChooserNative::create("Save as", Gtk::FILE_CHOOSER_ACTION_SAVE, "_Save", "_Cancel");
    auto filter = Gtk::FileFilter::create();
    // Show only text files
    filter->add_pattern("*.txt");
    dialog->add_filter(filter);
    int response = dialog->run();
    
    if (response == Gtk::RESPONSE_ACCEPT) {
        filename = dialog->get_filename();
        save_file(filename);
    }    
}

void SimulationWindow::on_action_experiment() {
    if (experiment) {
        experiment = false;
        m_Label_Test.set_visible(false);
    }else {
        experiment = true;
        m_Label_Test.set_visible(true);
        m_Label_Test.set_label("<span foreground='blue'>S.D. ON</span>");
    }
    // if (experimentMi->get_active())
    //     experimentMi->set_active(false);
    // else
    //     experimentMi->set_active(true);    
}

void SimulationWindow::on_action_zoom_in() {
    if (!zoominMi->get_sensitive()) return;
    zoom += 10;
    z_ratio -= 10;
    zoom_frame();
    m_LabelZoom.set_text(std::to_string(zoom) + "%");
    updt_statusbar_coord();

    if (z_ratio == 0) {
        zoominMi->set_sensitive(false);
        toolbutton_zoomin->set_sensitive(false);
    }
    zoomoutMi->set_sensitive(true);
    resetzoomMi->set_sensitive(true);
    toolbutton_zoomout->set_sensitive(true);
    toolbutton_resetzoom->set_sensitive(true);
}

void SimulationWindow::on_action_zoom_out() {
    if (!zoomoutMi->get_sensitive()) return;
    zoom -= 10;
    z_ratio += 10;
    zoom_frame();
    m_LabelZoom.set_text(std::to_string(zoom) + "%");
    updt_statusbar_coord();

    if (zoom == 100) {
        zoomoutMi->set_sensitive(false);
        resetzoomMi->set_sensitive(false);
        toolbutton_zoomout->set_sensitive(false);
    toolbutton_resetzoom->set_sensitive(false);
    }
    zoominMi->set_sensitive(true);
    toolbutton_zoomin->set_sensitive(true);
}

void SimulationWindow::on_action_reset_zoom() {
    zoom = 100;
    z_ratio = 100;
    m_LabelZoom.set_text(std::to_string(zoom) + "%");
    zoominMi->set_sensitive(true);
    zoomoutMi->set_sensitive(false);
    resetzoomMi->set_sensitive(false);
    toolbutton_zoomin->set_sensitive(true);
    toolbutton_zoomout->set_sensitive(false);
    toolbutton_resetzoom->set_sensitive(false);

    set_default_frame();
    updt_statusbar_coord();
}

void SimulationWindow::pan_frame_left(unsigned offset) {
    if (zoom > 100 && default_frame.xMin > -1) {
        default_frame.xMin -= offset;
        default_frame.xMax -= offset;
        updt_statusbar_coord();
        m_Area.refresh();
    }
}

void SimulationWindow::pan_frame_right(unsigned offset) {
    if (zoom > 100 && default_frame.xMax < Conf::world_size) {
        default_frame.xMin += offset;
        default_frame.xMax += offset;

        updt_statusbar_coord();
        m_Area.refresh();
    }
}

void SimulationWindow::pan_frame_up(unsigned offset) {
    if (zoom > 100 && default_frame.yMax < Conf::world_y_max) {
        default_frame.yMin += offset;
        default_frame.yMax += offset;

        updt_statusbar_coord();
        m_Area.refresh();
    }
}

void SimulationWindow::pan_frame_down(unsigned offset) {
    if (zoom > 100 && default_frame.yMin > -1) {
        default_frame.yMin -= offset;
        default_frame.yMax -= offset;

        updt_statusbar_coord();
        m_Area.refresh();
    }
}

void SimulationWindow::on_button_increase_size_clicked() {
    if (world_size_max - Conf::world_size < 50) {
        Conf::set_world_size(world_size_max);
    }else {
        Conf::set_world_size(Conf::world_size + 50);
    }
    if (Conf::world_size == world_size_max) {
        incrsizeMi->set_sensitive(false);
    }
    if (Conf::world_size > 300) {
        m_Button_Random.set_sensitive(false);
    }
    decrsizeMi->set_sensitive(true);
    set_default_frame();
    on_action_reset_zoom();
    adjust_bool_grid();
    m_LabelSize.set_text(std::to_string(Conf::world_size) + " x "
                       + std::to_string(Conf::world_y_max));
}

void SimulationWindow::on_button_decrease_size_clicked() {
    if (Conf::world_size - world_size_min < 50) {
        Conf::set_world_size(world_size_min);
    }else {
        Conf::set_world_size(Conf::world_size - 50);
    }
    if (Conf::world_size == world_size_min)  {
        decrsizeMi->set_sensitive(false);
    }
    if (Conf::world_size <= 300) {
        m_Button_Random.set_sensitive(true);
    }
    incrsizeMi->set_sensitive(true);
    set_default_frame();
    on_action_reset_zoom();
    adjust_bool_grid();
    m_LabelSize.set_text(std::to_string(Conf::world_size) + " x "
                       + std::to_string(Conf::world_y_max));
}

void SimulationWindow::on_checkbutton_dark_checked() {
    dark_theme_on = 1-dark_theme_on;
    if (!darkmode->get_active()) {
        auto settings = Gtk::Settings::get_default();
        settings->property_gtk_application_prefer_dark_theme().set_value(false);
        dark_theme_on = false;
    }else {
        auto settings = Gtk::Settings::get_default();
        settings->property_gtk_application_prefer_dark_theme().set_value(true);
        dark_theme_on = true;
    }
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
}

void SimulationWindow::on_combo_pattern_changed() {
    const auto iter = m_ComboPatt.get_active();
    if (iter) {
        const auto row = *iter;
        if (row) {
            Glib::ustring name = row[m_Columns.m_col_name];
        }
    }
}

void SimulationWindow::on_checkbutton_show_toolbar_checked() {
    if (m_ToolBar->get_visible())
        m_ToolBar->set_visible(false);
    else
        m_ToolBar->set_visible(true);
}

void SimulationWindow::on_checkbutton_show_statusbar_checked() {
    if (m_StatusBar.get_visible())
        m_StatusBar.set_visible(false);
    else
        m_StatusBar.set_visible(true);
}

void SimulationWindow::on_checkbutton_fade_checked() {
    toggle_fade_effect();
}

void SimulationWindow::on_checkbutton_grid_checked() {
    if (showgridMi->get_active()) {
        show_grid = true;
    }else {
        show_grid = false;
    }
}

void SimulationWindow::on_action_insert_pattern() {
    Gtk::MessageDialog patternDial("Select a pattern to insert.", false,
                Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK_CANCEL);
    Gtk::Box* marea = patternDial.get_message_area();
    marea->pack_start(m_ComboPatt);
    Gtk::Box* carea = patternDial.get_content_area();
    // Give focus to the "OK" button
    carea->set_focus_child(*(carea->get_children()[1]));
    patternDial.set_transient_for(*this);
    patternDial.show_all_children();
    int result = patternDial.run();

    if (result == GTK_RESPONSE_OK) {
        const auto iter = m_ComboPatt.get_active();
        if (iter) {
            const auto row = *iter;
            if (row) {
                Glib::ustring name = row[m_Columns.m_col_name];
                m_Area.set_pattern(get_rle_data(name));
                inserting_pattern = true;
                update_cursor();
            }
        }
    }
}

void SimulationWindow::on_action_help() {
    // Puts the file "Help.txt" into the "help" string
    std::ifstream f("Help.txt");
    std::string help;
    std::ostringstream ss;
    ss << f.rdbuf();
    help = ss.str();
    // Create a pop-up message dialog
    Gtk::MessageDialog help_dial("");
    Gtk::ScrolledWindow sc_win;
    Gtk::Label Help;
    Help.set_use_markup(true);
    Help.set_label(help);
    sc_win.add(Help);
    sc_win.set_size_request(250, 250);
    // Add the scrollable window to the message area
    Gtk::Box* marea = help_dial.get_message_area();
    marea->pack_start(sc_win);

    help_dial.set_title("Help");
    help_dial.set_transient_for(*this);
    help_dial.show_all_children();
    help_dial.run();
}

void SimulationWindow::on_button_about_clicked() {

    Gtk::AboutDialog about_dial;
    std::vector<Glib::ustring> authors(1, PROGRAM_AUTHOR);

    about_dial.set_program_name(PROGRAM_NAME);
    about_dial.set_authors(authors);
    about_dial.set_copyright("Copyright (C) 2022 Cyprien Lacassagne");
    about_dial.set_website("https://github.com/clacassa/Game-of-Life");
    // Hide the license button
    about_dial.unset_license();
    // Set GNU General License v3 as license
    about_dial.set_license_type(Gtk::LICENSE_GPL_3_0);
    // Pre-release version
    about_dial.set_version(PROGRAM_VERSION);
    about_dial.set_hexpand(false);
    about_dial.set_comments("Based on John Conway's game of life");
    about_dial.set_wrap_license(true);

    about_dial.show_all_children();
    about_dial.set_transient_for(*this);
    about_dial.run();
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
			Point p({clic_x - origin_x, clic_y -origin_y});
            int x(round((p.x*(default_frame.xMax-default_frame.xMin)/width) + default_frame.xMin));
            int y(round(default_frame.yMax - (p.y*(default_frame.yMax-default_frame.yMin)/height)));

            if (x < (int)Conf::world_size && y < (int)Conf::world_y_max) {
                x_mouse = x;
                y_mouse = y;
                m_LabelCoordinates.set_text("x=" + std::to_string(x_mouse) + "    y=" + std::to_string(y_mouse));
                switch (event->button)
                {
                case 1:
                    button_type = LEFT;
                    if (inserting_pattern) {
                        new_pattern(x, y, m_Area.get_pattern());
                        break;
                    }
                    if (current_action == DRAW)
                        new_birth(x, y);
                    break;
                case 2:
                    button_type = MIDDLE;
                    dragging_frame = true;
                    drag_frame();
                    break;
                case 3:
                    button_type = RIGHT;
                    if (inserting_pattern) {
                        new_pattern(x, y, m_Area.get_pattern());
                        break;
                    }
                    if (current_action == DRAW)
                        new_death(x, y);
                    break;
                default:
                    break;
                }
                m_Area.refresh();
                file_modified();
                m_Label_Population.set_label("<b>Population=" + std::to_string(get_alive()) + "</b>");
            }
		}
	}
	return Gtk::Window::on_button_press_event(event);
}

bool SimulationWindow::on_button_release_event(GdkEventButton * event) {

    if (!in_MyArea_window)
        return false;

    if (current_action == SELECT && !inserting_pattern)
        update_selection();

    if (button_type != MIDDLE) {
        inserting_pattern = false;
        rotateMi->set_sensitive(false);
        flipverMi->set_sensitive(false);
        fliphorMi->set_sensitive(false);
    }
        
    dragging_frame = false;
    update_cursor();
    button_type = NONE;
    x_0_sel_mouse = x_mouse;
    y_0_sel_mouse = y_mouse;
    m_Label_Population.set_label("<b>Population=" + std::to_string(get_alive()) + "</b>");
    return true;
}

bool SimulationWindow::on_motion_notify_event(GdkEventMotion * event) {
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
    if(clic_x >= origin_x && clic_x <= origin_x + width &&
        clic_y >= origin_y && clic_y <= origin_y + height) { 
        // Point that we are allowed to use expressed with drawing area coord.
        Point p({clic_x - origin_x, clic_y -origin_y});
        // Coord. calculated taking in consideration the size of the frame, to adapt when 
        // the user zooms in and translates the view frame
        int x(round((p.x*(default_frame.xMax-default_frame.xMin)/width) + default_frame.xMin));
        int y(round(default_frame.yMax - (p.y*(default_frame.yMax-default_frame.yMin)/height)));

        if (x < (int)Conf::world_size && y < (int)Conf::world_y_max && x >= 0 && y >= 0) {
            x_mouse = x;
            y_mouse = y;
            m_LabelCoordinates.set_text("x=" + std::to_string(x_mouse) + "    y=" + std::to_string(y_mouse));
            switch (button_type)
            {
            case LEFT:
                if (current_action == DRAW)
                    new_birth(x, y);
                else if (current_action == DRAG)
                    drag_frame();
                else if (current_action == SELECT)
                    update_selection();
                break;
            case RIGHT:
                if (current_action == DRAW)
                    new_death(x, y);
                break;
            case MIDDLE:
                    drag_frame();
                break;
            case NONE:
                x_0_sel_mouse = x_mouse;
                y_0_sel_mouse = y_mouse;
                break;
            }
            m_Area.refresh();
            m_Label_Population.set_label("<b>Population=" + std::to_string(get_alive()) + "</b>");
        }
    }
    return true;
}

void SimulationWindow::drag_frame() {
    bool positive_x_dir = (x_0_sel_mouse <= x_mouse);
    bool positive_y_dir = (y_0_sel_mouse <= y_mouse);
    if (positive_x_dir)
        pan_frame_left(2);
    else
        pan_frame_right(2);
    if (positive_y_dir)
        pan_frame_down(2);
    else
        pan_frame_up(2);
}

void SimulationWindow::update_selection() {
    unsigned x_min = (x_0_sel_mouse <= x_mouse) ? x_0_sel_mouse : x_mouse;
    unsigned x_max = (x_0_sel_mouse > x_mouse) ? x_0_sel_mouse : x_mouse;
    unsigned y_min = (y_0_sel_mouse <= y_mouse) ? y_0_sel_mouse : y_mouse;
    unsigned y_max = (y_0_sel_mouse > y_mouse) ? y_0_sel_mouse : y_mouse;
    std::vector<Pos> new_sel(get_live_cells_in_area(x_min, x_max-1, y_min, y_max-1));
    m_Area.set_selection(new_sel);
    cutMi->set_sensitive(true);
    copyMi->set_sensitive(true);
    clearMi->set_sensitive(true);
}

void SimulationWindow::on_action_new() {
    val = 0;
    init();
    this->set_title(PROGRAM_NAME);
    m_Label_Info.set_label("<b>Generation=" + std::to_string(val) + "</b>");
    m_Label_Population.set_label("<b>Population=" + std::to_string(get_alive()) + "</b>");
    m_Button_Reset.set_sensitive(false);
    saveMi->set_sensitive(false);
    toolbutton_save->set_sensitive(false);
    filename = "";
    if (experiment) {
        m_Label_Test.set_visible(true);
        m_Label_Test.set_label("<span foreground='blue'>S.D. ON</span>");
    }else {
        m_Label_Test.set_visible(false);
    }
}

void SimulationWindow::on_action_cut() {
    if (!cutMi->get_sensitive()) return;
    on_action_copy();
    if (!m_Area.get_selection().empty()) {
        del_pattern(0, 0, m_Area.get_selection());
        m_Area.set_selection(get_live_cells_in_area(0, 0, 0, 0));
    }
    cutMi->set_sensitive(false);
    copyMi->set_sensitive(false);
    clearMi->set_sensitive(false);
}

void SimulationWindow::on_action_copy() {
    if (!copyMi->get_sensitive()) return;
    m_Area.set_clipboard(m_Area.get_selection());
    m_Area.set_pattern(m_Area.rebase_coords(m_Area.get_selection()));
    cutMi->set_sensitive(false);
    copyMi->set_sensitive(false);
    clearMi->set_sensitive(false);
    pasteMi->set_sensitive(true);
}

void SimulationWindow::on_action_clear() {
    if (!clearMi->get_sensitive()) return;

    if (!m_Area.get_selection().empty()) {
        del_pattern(0, 0, m_Area.get_selection());
        m_Area.set_selection(get_live_cells_in_area(0, 0, 0, 0));
    }
    cutMi->set_sensitive(false);
    copyMi->set_sensitive(false);
    clearMi->set_sensitive(false);
}

void SimulationWindow::on_action_paste() {
    rotateMi->set_sensitive(true);
    flipverMi->set_sensitive(true);
    fliphorMi->set_sensitive(true);
    inserting_pattern = true;
    update_cursor();
}

void SimulationWindow::on_action_select_all() {
    std::vector<Pos> sel_all(get_live_cells_in_area(0, Conf::world_size-2, 0, Conf::world_y_max-2));
    m_Area.set_selection(sel_all);
    cutMi->set_sensitive(true);
    copyMi->set_sensitive(true);
    clearMi->set_sensitive(true);
}

void SimulationWindow::on_action_rotate() {
    if (inserting_pattern)
        m_Area.rotate_pattern();
}

void SimulationWindow::on_action_flip_vertically() {
    if (inserting_pattern)
        m_Area.flip_pattern_up_down();
}

void SimulationWindow::on_action_flip_horizontally() {
    if (inserting_pattern)
        m_Area.flip_pattern_left_right();
}

void SimulationWindow::on_action_cursor_draw() {
    current_action = DRAW;
    update_cursor();
}

void SimulationWindow::on_action_cursor_drag() {
    current_action = DRAG;
    update_cursor();
}

void SimulationWindow::on_action_cursor_select() {
    current_action = SELECT;
    update_cursor();
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
    switch (current_action) {
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

void SimulationWindow::on_event_add_timer() {
    if (!timer_added) {
        Glib::signal_timeout().connect(sigc::mem_fun(*this,
                    &SimulationWindow::on_timeout), timeout_value);
        timer_added = true;
    }
}

void SimulationWindow::on_event_delete_timer() {
    if (timer_added) {
        disconnect = true;
        timer_added = false;
    }
}

bool SimulationWindow::on_timeout() {
    if (disconnect) {
        disconnect = false;
        return false;
    }

    if (experiment) {
        if (update(EXPERIMENTAL)) {
            // Update the stability detection status label
            m_Label_Test.set_label("<span foreground='green'>Stable</span>");

            // Reset the properties of the start button
            m_Button_Start.set_label("Start");
            Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
            css_provider->load_from_data("button {background-image: image(green);}");
            m_Button_Start.get_style_context()->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
            m_Label_Population.set_label("<b>Population=" + std::to_string(get_alive()) + "</b>");
            if (val < 4)
                m_Label_Info.set_label("<b>Generation=" + std::to_string(0) + "</b>");
            else
                m_Label_Info.set_label("<b>Generation=" + std::to_string(val-4) + "</b>");
            // Stop the timer
            on_event_delete_timer();          
        }else {
            m_Label_Test.set_visible(true);
            m_Label_Test.set_label("<span foreground='blue'>S.D. ON</span>");
        }
    }else {
        update();
        m_Label_Test.set_visible(false);
    }
    if (get_alive() == 0 && m_Button_Start.get_label() == "Stop") {
        on_button_start_clicked();
        --val;
    }
    ++val;
    m_Label_Info.set_label("<b>Generation=" + std::to_string(val) + "</b>");
    m_Label_Population.set_label("<b>Population=" + std::to_string(get_alive()) + "</b>");
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
            return Gtk::Window::on_key_press_event(key_event);
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
            default:
                break;
        }
    }
    return Gtk::Window::on_key_press_event(key_event);
}

void SimulationWindow::create_main_action_group(Glib::RefPtr<Gtk::Application> app) {
    m_refActionGroup = Gio::SimpleActionGroup::create();

    m_refActionGroup->add_action("new", sigc::mem_fun(*this,
            &SimulationWindow::on_action_new));

    m_refActionGroup->add_action("open", sigc::mem_fun(*this, 
                &SimulationWindow::on_action_open));

    m_refActionGroup->add_action("save", sigc::mem_fun(*this,
            &SimulationWindow::on_action_save));

    m_refActionGroup->add_action("saveas", sigc::mem_fun(*this,
            &SimulationWindow::on_action_saveas));

    m_refActionGroup->add_action("quit", sigc::mem_fun(*this,
            &SimulationWindow::on_action_quit));

    m_refActionGroup->add_action("cut", sigc::mem_fun(*this,
            &SimulationWindow::on_action_cut));

    m_refActionGroup->add_action("copy", sigc::mem_fun(*this,
            &SimulationWindow::on_action_copy));

    m_refActionGroup->add_action("clear", sigc::mem_fun(*this,
            &SimulationWindow::on_action_clear));

    m_refActionGroup->add_action("paste", sigc::mem_fun(*this,
            &SimulationWindow::on_action_paste));

    m_refActionGroup->add_action("selectall", sigc::mem_fun(*this,
            &SimulationWindow::on_action_select_all));

    m_refActionGroup->add_action("random", sigc::mem_fun(*this,
            &SimulationWindow::on_action_random));

    m_refActionGroup->add_action("rotate", sigc::mem_fun(*this,
            &SimulationWindow::on_action_rotate));

    m_refActionGroup->add_action("flipver", sigc::mem_fun(*this,
            &SimulationWindow::on_action_flip_vertically));

    m_refActionGroup->add_action("fliphor", sigc::mem_fun(*this,
            &SimulationWindow::on_action_flip_horizontally));

    m_refActionGroup->add_action("draw", sigc::mem_fun(*this,
            &SimulationWindow::on_action_cursor_draw));
        
    m_refActionGroup->add_action("drag", sigc::mem_fun(*this,
            &SimulationWindow::on_action_cursor_drag));

    m_refActionGroup->add_action("select", sigc::mem_fun(*this,
            &SimulationWindow::on_action_cursor_select));

    m_refActionGroup->add_action("zoomin", sigc::mem_fun(*this,
            &SimulationWindow::on_action_zoom_in));

    m_refActionGroup->add_action("zoomout", sigc::mem_fun(*this,
            &SimulationWindow::on_action_zoom_out));

    m_refActionGroup->add_action("resetzoom", sigc::mem_fun(*this,
            &SimulationWindow::on_action_reset_zoom));

    m_refActionGroup->add_action("showtoolbar", sigc::mem_fun(*this,
            &SimulationWindow::on_checkbutton_show_toolbar_checked));

    m_refActionGroup->add_action("showstatusbar", sigc::mem_fun(*this,
            &SimulationWindow::on_checkbutton_show_statusbar_checked));

    m_refActionGroup->add_action("showgrid", sigc::mem_fun(*this,
            &SimulationWindow::on_checkbutton_grid_checked));

    m_refActionGroup->add_action("fade", sigc::mem_fun(*this,
            &SimulationWindow::on_checkbutton_fade_checked));

    m_refActionGroup->add_action("dark", sigc::mem_fun(*this,
            &SimulationWindow::on_checkbutton_dark_checked));

    m_refActionGroup->add_action("scheme", sigc::mem_fun(*this,
            &SimulationWindow::on_button_colorscheme_clicked));

    m_refActionGroup->add_action("experiment", sigc::mem_fun(*this,
            &SimulationWindow::on_action_experiment));

    m_refActionGroup->add_action("increasesize", sigc::mem_fun(*this,
            &SimulationWindow::on_button_increase_size_clicked));

    m_refActionGroup->add_action("decreasesize", sigc::mem_fun(*this,
            &SimulationWindow::on_button_decrease_size_clicked));

    m_refActionGroup->add_action("pattern", sigc::mem_fun(*this,
            &SimulationWindow::on_action_insert_pattern));

    m_refActionGroup->add_action("help", sigc::mem_fun(*this,
            &SimulationWindow::on_action_help));

    m_refActionGroup->add_action("about", sigc::mem_fun(*this,
            &SimulationWindow::on_button_about_clicked));

    insert_action_group("main", m_refActionGroup);
    app->set_accel_for_action("main.new", "<Primary>n");
    app->set_accel_for_action("main.open", "<Primary>o");
    app->set_accel_for_action("main.save", "<Primary>s");
    app->set_accel_for_action("main.saveas", "<Primary><Shift>s");
    app->set_accel_for_action("main.quit", "<Primary>q");
    app->set_accel_for_action("main.cut", "<Primary>x");
    app->set_accel_for_action("main.copy", "<Primary>c");
    app->set_accel_for_action("main.paste", "<Primary>v");
    app->set_accel_for_action("main.selectall", "<Primary>a");
    app->set_accel_for_action("main.clear", "Delete");
    app->set_accel_for_action("main.random", "<Primary>r");
    app->set_accel_for_action("main.rotate", "r");
    app->set_accel_for_action("main.flipver", "y");
    app->set_accel_for_action("main.fliphor", "x");
    app->set_accel_for_action("main.draw", "F2");
    app->set_accel_for_action("main.drag", "F3");
    app->set_accel_for_action("main.select", "F4");
    app->set_accel_for_action("main.zoomin", "<Primary>plus");
    app->set_accel_for_action("main.zoomout", "<Primary>minus");
    app->set_accel_for_action("main.resetzoom", "0");
    app->set_accel_for_action("main.increasesize", "<Primary>Page_Up");
    app->set_accel_for_action("main.decreasesize", "<Primary>Page_Down");
    app->set_accel_for_action("main.pattern", "<Primary>p");
    app->set_accel_for_action("main.help", "F1");
}

void SimulationWindow::instanciate_menubar_from_glade() {
    m_refBuilder = Gtk::Builder::create();

    try {
        m_refBuilder->add_from_file("src/toolbar.glade");
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

    showToolbarMi = nullptr;
    m_refBuilder->get_widget("showtoolbarMi", showToolbarMi);
    if (!showToolbarMi)
        g_warning("GtkCheckButton not found: showtoolbarMi");
    
    showStatusBarMi = nullptr;
    m_refBuilder->get_widget("showstatbarMi", showStatusBarMi);
    if (!showStatusBarMi)
        g_warning("GtkCheckButton not found: showstatbarMi");

    showgridMi = nullptr;
    m_refBuilder->get_widget("showgridMi", showgridMi);
    if (!showgridMi)
        g_warning("GtkCheckButton not found: showgridMi");
    showgridMi->set_active(true);
    
    fadeMi = nullptr;
    m_refBuilder->get_widget("fadeMi", fadeMi);
    if (!fadeMi)
        g_warning("GtkCheckButton not found: fadeMi");

    darkmode = nullptr;
    m_refBuilder->get_widget("darkMi", darkmode);
    if (!darkmode)
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
    if (Conf::world_size >= world_size_max)
        incrsizeMi->set_sensitive(false);

    decrsizeMi = nullptr;
    m_refBuilder->get_widget("decreasesizeMi", decrsizeMi);
    if (!decrsizeMi)
        g_warning("GtkMenuItem not found: decreasesizeMi");
    if (Conf::world_size <= world_size_min)
        decrsizeMi->set_sensitive(false);

    zoominMi = nullptr;
    m_refBuilder->get_widget("zoominMi", zoominMi);
    if (!zoominMi)
        g_warning("GtkMenuItem not found: zoominMi");
    zoominMi->set_sensitive(true);

    zoomoutMi = nullptr;
    m_refBuilder->get_widget("zoomoutMi", zoomoutMi);
    if (!zoomoutMi)
        g_warning("GtkMenuItem not found: zoomoutMi");
    zoomoutMi->set_sensitive(false);

    resetzoomMi = nullptr;
    m_refBuilder->get_widget("resetzoomMi", resetzoomMi);
    if (!resetzoomMi)
        g_warning("GtkMenuItem not found: resetzoomMi");
    resetzoomMi->set_sensitive(false);
}

void SimulationWindow::instanciante_toolbar_from_glade() {
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
        g_warning("GtkToolButton not found: toolbutton_wew");
    toolbutton_new->set_can_focus(false);

    toolbutton_save = nullptr;
    m_refBuilder->get_widget("toolbutton_save", toolbutton_save);
    if (!toolbutton_save)
        g_warning("GtkToolButton not found: toolbutton_save");
    toolbutton_save->set_sensitive(false);

    toolbutton_zoomin = nullptr;
    m_refBuilder->get_widget("toolbutton_zoomin", toolbutton_zoomin);
    if (!toolbutton_zoomin)
        g_warning("GtkToolButton not found: toolbutton_zoomin");
    
    toolbutton_zoomout = nullptr;
    m_refBuilder->get_widget("toolbutton_zoomout", toolbutton_zoomout);
    if (!toolbutton_zoomout)
        g_warning("GtkToolButton not found: toolbutton_zoomout");
    toolbutton_zoomout->set_sensitive(false);

    toolbutton_resetzoom = nullptr;
    m_refBuilder->get_widget("toolbutton_resetzoom", toolbutton_resetzoom);
    if (!toolbutton_resetzoom)
        g_warning("GtkToolButton not found: toolbutton_resetzoom");
    toolbutton_resetzoom->set_sensitive(false);
}

void SimulationWindow::create_refresh_scale() {
    m_Scale.set_range(10, 50);
    m_Scale.set_value(startup_timeout_value);
    m_Scale.add_mark(10, Gtk::POS_BOTTOM, "10");
    m_Scale.add_mark(20, Gtk::POS_BOTTOM, "20");
    m_Scale.add_mark(30, Gtk::POS_BOTTOM, "30");
    m_Scale.add_mark(40, Gtk::POS_BOTTOM, "40");
    m_Scale.add_mark(50, Gtk::POS_BOTTOM, "50");
    m_Scale.set_value_pos(Gtk::POS_TOP);
    m_Scale.set_show_fill_level(true);
    m_Scale.set_has_origin(false);
    m_Scale.set_digits(0);
    m_Scale.set_round_digits(0);
    m_Scale.set_inverted(true);
    m_Scale.set_property("can-focus", false);
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

    // Create Step button
    m_ButtonBox.pack_start(m_Button_Step);
    m_Button_Step.set_property("can-focus", false);

    // Create Clear button
    m_ButtonBox.pack_start(m_Button_Clear);
    m_Button_Clear.set_property("can-focus", false);

    // Create Random button
    m_ButtonBox.pack_start(m_Button_Random);
    m_Button_Random.set_property("can-focus", false);

    // Create Reset button
    m_ButtonBox.pack_start(m_Button_Reset);
    m_Button_Reset.set_property("can-focus", false);
    m_Button_Reset.set_sensitive(false);

    // Create a separator
    m_ButtonBox.pack_start(m_Sep);
    m_Sep.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
}

void SimulationWindow::create_StatusBar() {
    // - for the simulation size
    m_LabelSize.set_text(m_LabelSize.get_text() + std::to_string(Conf::world_size)
                        + " x " + std::to_string(Conf::world_y_max));
    m_LabelSize.set_halign(Gtk::ALIGN_END);
    m_LabelSize.set_hexpand(false);
    // - for the zoom level
    m_LabelZoom.set_text(std::to_string(zoom) + "%");
    m_LabelZoom.set_halign(Gtk::ALIGN_END);
    m_LabelZoom.set_hexpand(false);
    // - for the coordinates
    m_LabelCoordinates.set_text("x=" + x + "    y=" +y);
    m_LabelCoordinates.set_halign(Gtk::ALIGN_END);
    m_LabelCoordinates.set_hexpand(false);
    // - for the sim. info
    m_Label_Info.set_label("<b>Generation=" + std::to_string(val) + "</b>");
    m_Label_Info.set_halign(Gtk::ALIGN_END);
    m_Label_Info.set_hexpand(false);
    m_Label_Info.set_use_markup(true);
    m_Label_Population.set_label("<b>Population=" + std::to_string(get_alive()) + "</b>");
    m_Label_Population.set_halign(Gtk::ALIGN_END);
    m_Label_Population.set_hexpand(false);
    m_Label_Population.set_use_markup(true);

    m_Label_Test.set_visible(false);
    m_Label_Test.set_use_markup(true);

    // Create StatusBar
    m_StatusBar.pack_end(m_LabelSize);
    m_StatusBar.pack_end(m_LabelZoom);
    m_StatusBar.pack_end(m_LabelCoordinates);
    m_StatusBar.pack_end(m_Label_Info);
    m_StatusBar.pack_end(m_Label_Population);
    m_StatusBar.pack_end(m_Label_Test);
    m_StatusBar.set_valign(Gtk::ALIGN_END);
    m_StatusBar.set_vexpand(false);
    m_StatusBar.set_halign(Gtk::ALIGN_BASELINE);
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

void SimulationWindow::create_patterns_ComboBox() {
    m_pattTreeModel = Gtk::ListStore::create(m_Columns);
    m_ComboPatt.set_model(m_pattTreeModel);

    Gtk::TreeRow row;
    const size_t len(PATTERNS_DIR.length());

    for (const auto& entry : fs::directory_iterator(PATTERNS_DIR)) {
        row = *(m_pattTreeModel->append());
        std::string pattern_name(entry.path().string().replace(0, len, ""));
        row[m_Columns.m_col_name] = pattern_name;
    }
    m_ComboPatt.pack_start(m_Columns.m_col_name);
    m_ComboPatt.pack_start(m_cell);
}

void SimulationWindow::read_settings() {
    Glib::KeyFile keyFile;
    const Glib::ustring settings_grp("Settings");
    const Glib::ustring pref_group("Preferences");
    const Glib::ustring light_s_key("color-scheme-light");
    const Glib::ustring dark_s_key("color-scheme-dark");
    const Glib::ustring dark_theme_key("gtk-application-prefer-dark-theme");
    unsigned light_s_keyval, dark_s_keyval;
    bool dark_theme_keyval;

    keyFile.load_from_file(SETTINGS_INI_FILE, Glib::KEY_FILE_KEEP_COMMENTS);
    light_s_keyval = keyFile.get_integer(pref_group, light_s_key);
    dark_s_keyval = keyFile.get_integer(pref_group, dark_s_key);
    dark_theme_keyval = keyFile.get_boolean(settings_grp, dark_theme_key);

    m_ComboLight.set_active(light_s_keyval);
    m_ComboDark.set_active(dark_s_keyval);
    if (dark_theme_keyval) {
        dark_theme_on = true;
        darkmode->set_active(true);
    }
}

void SimulationWindow::write_settings() {
    const std::string light_s_key("color-scheme-light");
    const std::string dark_s_key("color-scheme-dark");
    const std::string dark_theme_key("gtk-application-prefer-dark-theme");
    const std::string light_s_new_keyval(std::to_string(m_ComboLight.get_active_row_number()));
    const std::string dark_s_new_keyval(std::to_string(m_ComboDark.get_active_row_number()));
    std::string dark_theme_new_keyval;
    if (dark_theme_on) dark_theme_new_keyval = "1";
    else dark_theme_new_keyval = "0";

    std::ifstream settings(SETTINGS_INI_FILE);
    std::ostringstream ss;
    ss << settings.rdbuf();
    std::string str(ss.str());

    if (!settings.fail()) {
        size_t pos(str.find(light_s_key));
        str.replace(pos + light_s_key.length() + 1, 1, light_s_new_keyval);
        pos = str.find(dark_s_key);
        str.replace(pos + dark_s_key.length() + 1, 1, dark_s_new_keyval);
        pos = str.find(dark_theme_key);
        str.replace(pos + dark_theme_key.length() + 1, 1, dark_theme_new_keyval);
    }

    settings.close();
    std::ofstream edited_settings(SETTINGS_INI_FILE);
    edited_settings << str;
}

SimulationWindow::~SimulationWindow() {
    write_settings();
}

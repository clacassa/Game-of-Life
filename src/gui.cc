/*
 *  gui.cc -- GameofLife -- GUI with various options and view controls
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

#include "gui.h"
#include "graphic_gui.h"
#include "simulation.h"
#include "config.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cairomm/context.h>
#include <cstdlib>

static Frame default_frame = {-1, Conf::world_size, -1, Conf::world_size,
                                                       1, window_size, window_size};

static void orthographic_projection(const Cairo::RefPtr<Cairo::Context>& cr,
                                                                        Frame frame);

static unsigned val(0);
static unsigned zoom(100);
static unsigned z_ratio(100);
// The background theme: 1 for light, 0 for dark
static unsigned _theme_(1);

static bool show_grid(false);

void set_default_frame() {
    default_frame.xMin = -1;
    default_frame.xMax = Conf::world_size;
    default_frame.yMin = -1;
    default_frame.yMax = Conf::world_size;
}

//===========================================

MyArea::MyArea()
:   empty(false) {}

MyArea::~MyArea() {}

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

    if (_theme_) {
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

        graphic_draw_world(Conf::world_size, Conf::world_size, _theme_, show_grid);
        draw_cells(1-_theme_);
        refresh();
    }
    return true;
}

/*  ===========================================
    SimulationWindow constructor: TopLevel window initialization
    Manages all signal handlers and events
    ===========================================
*/
SimulationWindow::SimulationWindow(std::string __filename)
:   fileMi("File"),
    viewMi("View"),
    toolsMi("Tools"),
    m_helpMi("Help"),
    openMi("Open..."),
    saveMi("Save"),
    saveasMi("Save as..."),
    quitMi("Quit"),
    zoominMi("Zoom in"),
    zoomoutMi("Zoom out"),
    resetzoomMi("Reset zoom"),
    showgridMi("Show grid"),
    fadeMi("Fade effect"),
    darkmode("Dark theme"),
    experimentMi("Stability Detection (S.D.)"),
    simsizeMi("World size"),
    incrsizeMi("Extend"),
    decrsizeMi("Shrink"),
    helpMi("Help"),
    aboutMi("About"),
    m_SuperBox(Gtk::ORIENTATION_VERTICAL),
    m_Box(Gtk::ORIENTATION_HORIZONTAL),
    m_Box_General(Gtk::ORIENTATION_VERTICAL, 15),
    m_ButtonBox(Gtk::ORIENTATION_VERTICAL, 5),
    m_Frame_Speed("Refresh (ms)"),
    m_Button_Start("Start"),
    m_Button_Step("Step"),
    m_Button_Reset("Reset"),
    m_Button_Clear("Clear"),
    m_Button_Random("Random"),
    m_Button_Canon("Glider gun"),
    m_Button_Max("Spacefiller"),
    m_Scale(Gtk::ORIENTATION_HORIZONTAL),
    m_Label_Theme("dark"),
    timer_added(false),
    disconnect(false),
    experiment(false),
    timeout_value(startup_timeout_value),
    frame_surface(default_frame.xMax*default_frame.xMax),
    filename(__filename),
    x(std::to_string(default_frame.xMax / 2)),
    y(std::to_string(default_frame.yMax / 2)),
    button_type(NONE)
    {
        m_Box.set_margin_top(2);
        m_Box.set_margin_right(2);
        m_Box_General.set_margin_left(5);
        m_Box_General.set_margin_right(5);
        m_Box_General.set_size_request(130);

        add(m_SuperBox);
        m_SuperBox.pack_start(m_MenuBar, false, false);
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

        create_MenuBar();
        MenuBar_signals_hdl();
        MenuBar_accelerators();

        create_refresh_scale();
        create_control_buttons();
        create_StatusBar();

        if (!_theme_) {
            darkmode.set_active(true);
        }

        show_all_children();
    }

void SimulationWindow::zoom_frame() {
    if (z_ratio == 0) {
        default_frame.xMax -= 10;
        default_frame.yMax -= 10;
        return;
    }

    // Calculate the target frame's dimensions
    unsigned target_surface(Conf::world_size * Conf::world_size * z_ratio / 100);
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
    if (yMaxNew > Conf::world_size) {
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
    m_LabelCoordinates.set_text("x : " + x + "    y : " + y);
}

void SimulationWindow::reset_max_canon() {
    m_Button_Canon.set_active(false);
    m_Button_Canon.set_sensitive(true);
    m_Button_Max.set_active(false);
    m_Button_Max.set_sensitive(true);
    val = 0;
}

// The asterisk shows that change are occuring, but the file
// is not overwritten yet. Click on 'Save' to overwrite the file
void SimulationWindow::file_modified() {
    if (filename != "" && !saveMi.get_sensitive()) {
        saveMi.set_sensitive(true);
        m_Button_Reset.set_sensitive(true);
        this->set_title("*" + this->get_title());
    }
}

void set_dark_theme_on() {
    _theme_ = 0;
}

void SimulationWindow::on_button_quit_clicked() {
    exit(EXIT_SUCCESS);
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
        file_modified();
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
        file_modified();
    }
}

void SimulationWindow::on_button_reset_clicked() {
    val = 0;
    m_Button_Reset.set_sensitive(false);
    saveMi.set_sensitive(false);
    init();
    read_file(filename);
    this->set_title(this->get_title().replace(0, 1, ""));
    m_Label_Info.set_label("<b>Generation : " + std::to_string(val) + "</b>");
    reset_max_canon();
}

void SimulationWindow::on_button_clear_clicked() {
    val = 0;
    init();
    this->set_title("Game of Life");
    m_Label_Info.set_label("<b>Generation : " + std::to_string(val) + "</b>");
    m_Button_Reset.set_sensitive(false);
    saveMi.set_sensitive(false);
    filename = "";
    if (experiment) {
        m_Label_Test.set_label("<span foreground='blue'>S.D. ON</span>");
    }else {
        m_Label_Test.set_label("S.D. OFF");
    }
    reset_max_canon();
}

void SimulationWindow::on_button_random_clicked() {
    init();
    reset_max_canon();
    file_modified();
    if (experiment) {
        m_Label_Test.set_label("<span foreground='blue'>S.D. ON</span>");
    }else {
        m_Label_Test.set_label("S.D. OFF");
    }
    
    // Randomly fill half of the grid
    unsigned rand_x, rand_y;
    for (unsigned index(0); index < (Conf::world_size*Conf::world_size)/2; ++index) {
        rand_x = rand() % Conf::world_size;
        rand_y = rand() % Conf::world_size;
        new_birth(rand_x, rand_y);
    }
    m_Area.refresh();
}

void SimulationWindow::on_button_open_clicked() {

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
        read_file(filename);

        set_default_frame();
        updt_statusbar_coord();
        m_Label_Info.set_label("<span weight='bold' > Generation : " + std::to_string(val) + "</span>");
        m_LabelSize.set_text(std::to_string(Conf::world_size) + " x "+ std::to_string(Conf::world_size));

        unsigned pos = filename.find_last_of('\\');
        std::string flnm = filename;
        this->set_title(flnm.replace(0, pos + 1, "") + "  -  Game of Life");
        file_error_dialog();
    }
    m_Area.refresh();
}

void SimulationWindow::file_error_dialog() {
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
    // Font options
    if (_theme_) {
        error_dialog.set_message("<span size='large' foreground='#000099' letter_spacing='1024'>"
                        + error_message + "</span>", true);
    }else {
        error_dialog.set_message("<span size='large' foreground='#C0C0C0' letter_spacing='1024'>"
                        + error_message + "</span>", true);
    }
    std::vector<Gtk::Widget*> children = error_dialog.get_action_area()->get_children();
    children[0]->set_margin_left(dialog_button_margin);              
    error_dialog.set_icon_from_file("share/icons/notifications.ico");
    error_dialog.set_transient_for(*this);
    error_dialog.run();
    this->set_title("Game of Life");
}

void SimulationWindow::on_button_save_clicked() {
    save_file(filename);
    saveMi.set_sensitive(false);
    m_Button_Reset.set_sensitive(false);
    this->set_title(this->get_title().replace(0, 1, ""));
}

void SimulationWindow::on_button_saveas_clicked() {
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

void SimulationWindow::on_button_test_clicked() {
    if (experiment) {
        experiment = false;
        m_Label_Test.set_label("S.D. OFF");
    }else {
        experiment = true;
        m_Label_Test.set_label("<span foreground='blue'>S.D. ON</span>");
    }
}

void SimulationWindow::on_button_zoom_in_clicked() {
    zoom += 10;
    z_ratio -= 10;
    zoom_frame();
    m_LabelZoom.set_text(std::to_string(zoom) + "%");
    updt_statusbar_coord();

    if (z_ratio == 0) {
        zoominMi.set_sensitive(false);
    }
    zoomoutMi.set_sensitive(true);
    resetzoomMi.set_sensitive(true);
}

void SimulationWindow::on_button_zoom_out_clicked() {
    zoom -= 10;
    z_ratio += 10;
    zoom_frame();
    m_LabelZoom.set_text(std::to_string(zoom) + "%");
    updt_statusbar_coord();

    if (zoom == 100) {
        zoomoutMi.set_sensitive(false);
        resetzoomMi.set_sensitive(false);
    }
    zoominMi.set_sensitive(true);
}

void SimulationWindow::on_button_reset_zoom_clicked() {
    zoom = 100;
    z_ratio = 100;
    m_LabelZoom.set_text(std::to_string(zoom) + "%");
    zoomoutMi.set_sensitive(false);
    resetzoomMi.set_sensitive(false);
    zoominMi.set_sensitive(true);

    set_default_frame();
    updt_statusbar_coord();
}

void SimulationWindow::on_leftarrow_pressed() {
    if (zoom > 100 && default_frame.xMin > -1) {
        default_frame.xMin -= 1;
        default_frame.xMax -= 1;
        updt_statusbar_coord();
        m_Area.refresh();
    }
}

void SimulationWindow::on_rightarrow_pressed() {
    if (zoom > 100 && default_frame.xMax < Conf::world_size) {
        default_frame.xMin += 1;
        default_frame.xMax += 1;

        updt_statusbar_coord();
        m_Area.refresh();
    }
}

void SimulationWindow::on_uparrow_pressed() {
    if (zoom > 100 && default_frame.yMax < Conf::world_size) {
        default_frame.yMin += 1;
        default_frame.yMax += 1;

        updt_statusbar_coord();
        m_Area.refresh();
    }
}

void SimulationWindow::on_downarrow_pressed() {
    if (zoom > 100 && default_frame.yMin > -1) {
        default_frame.yMin -= 1;
        default_frame.yMax -= 1;

        updt_statusbar_coord();
        m_Area.refresh();
    }
}

void SimulationWindow::on_button_increase_size_clicked() {
    Conf::set_world_size(Conf::world_size + 50);
    if (Conf::world_size == world_size_max) {
        incrsizeMi.set_sensitive(false);
    }
    decrsizeMi.set_sensitive(true);
    set_default_frame();
    on_button_reset_zoom_clicked();
    adjust_bool_grid();
    m_LabelSize.set_text(std::to_string(Conf::world_size) + " x "
                       + std::to_string(Conf::world_size));
}

void SimulationWindow::on_button_decrease_size_clicked() {
    Conf::set_world_size(Conf::world_size - 50);
    if (Conf::world_size == world_size_min)  {
        decrsizeMi.set_sensitive(false);
    }
    incrsizeMi.set_sensitive(true);
    set_default_frame();
    on_button_reset_zoom_clicked();
    adjust_bool_grid();
    m_LabelSize.set_text(std::to_string(Conf::world_size) + " x "
                       + std::to_string(Conf::world_size));
}

void SimulationWindow::on_checkbutton_dark_checked() {
    _theme_ = 1-_theme_;
    if (!darkmode.get_active()) {
        g_object_set(gtk_settings_get_default(),
    			"gtk-application-prefer-dark-theme", FALSE, NULL);
        _theme_ = 1;
    }else {
        g_object_set(gtk_settings_get_default(),
    			"gtk-application-prefer-dark-theme", TRUE, NULL);
        _theme_ = 0;
    }
}

void SimulationWindow::on_checkbutton_fade_checked() {
    toggle_fade_effect();
}

void SimulationWindow::on_checkbutton_grid_checked() {
    if (showgridMi.get_active()) {
        show_grid = true;
    }else {
        show_grid = false;
    }
}

void SimulationWindow::on_button_help_clicked() {
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
    sc_win.set_size_request(600, 300);
    // Add the scrollable window to the message area
    Gtk::Box* marea = help_dial.get_message_area();
    marea->pack_start(sc_win);

    help_dial.set_title("Features Documentation");
    help_dial.set_transient_for(*this);
    help_dial.show_all_children();
    help_dial.run();
}

void SimulationWindow::on_button_about_clicked() {

    Gtk::AboutDialog about_dial(true);
    std::vector<Glib::ustring> authors(1, "Cyprien Lacassagne");
    // Julian contributed to this project by sharing me his
    // feedback about the GUI during the development, thanks!
    std::vector<Glib::ustring> contributors(1, "Julian Ruiz Rodriguez");

    about_dial.set_program_name("Game of Life");
    about_dial.set_authors(authors);
    about_dial.set_copyright("Copyright (C) 2022 Cyprien Lacassagne");
    // Hide the license button
    about_dial.unset_license();
    // Set GNU General License v3 as license
    about_dial.set_license_type(Gtk::LICENSE_GPL_3_0);
    // Pre-release version
    about_dial.set_version("0.2.0");
    about_dial.set_hexpand(false);
    about_dial.add_credit_section("Contributor", contributors);
    about_dial.set_comments("Based on John Conway's game of life");
    about_dial.set_wrap_license(true);

    about_dial.show_all_children();
    about_dial.set_transient_for(*this);
    about_dial.run();
}

void SimulationWindow::on_button_gosper_clicked() {
    if (m_Button_Canon.get_active()) {
        draw_canon_planeur(0, 0);
    }else {
        erase_canon_planeur(0, 0);
    }
    file_modified();
    m_Area.refresh();
}

void SimulationWindow::on_button_max_clicked() {
    if (m_Button_Max.get_active()) {
        draw_spacefiller(Conf::world_size/2 - 13, Conf::world_size/2 - 13);
    }else {
        erase_spacefiller(Conf::world_size/2 - 13, Conf::world_size/2 - 13);
    }
    file_modified();
    m_Area.refresh();
}

bool SimulationWindow::on_button_press_event(GdkEventButton * event)
{
    Glib::RefPtr<Gdk::Window> ref_win(m_Area.get_window());
    auto scroll = Gdk::Cursor::create(Gdk::CursorType::DOUBLE_ARROW);
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

            int delta_x = default_frame.xMax-default_frame.xMin;
            int delta_y = default_frame.yMax-default_frame.yMin;

            if (x < (int)Conf::world_size && y < (int)Conf::world_size) {
                switch (event->button)
                {
                case 1:
                    // A left click gives birth to a cell
                    new_birth(x, y);
                    button_type = LEFT;
                    break;
                case 2:
                    // A middle click translates the view frame
                    if (x < (delta_x)/4 + default_frame.xMin) {
                        on_leftarrow_pressed();
                    }else if (x >= (delta_x*3/4) + default_frame.xMin) {
                        on_rightarrow_pressed();
                    }
                    if (y < delta_y/4 + default_frame.yMin) {
                        on_downarrow_pressed();
                    }else if (y >= delta_y*3/4 + default_frame.yMin) {
                        on_uparrow_pressed();
                    }
                    button_type = SCROLL;
                    ref_win->set_cursor(scroll);
                    break;
                case 3:
                    // A right click kills a cell
                    new_death(x, y);
                    button_type = RIGHT;
                    break;
                default:
                    break;
                }
                m_Area.refresh();
                file_modified();
            }
		}
	}
	return Gtk::Window::on_button_press_event(event);
}

bool SimulationWindow::on_button_release_event(GdkEventButton * event) {
    Glib::RefPtr<Gdk::Window> ref_win(m_Area.get_window());
    auto pencil = Gdk::Cursor::create(Gdk::CursorType::PENCIL);
    ref_win->set_cursor(pencil);
    button_type = NONE;
    return true;
}

bool SimulationWindow::on_motion_notify_event(GdkEventMotion * event) {
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

        int delta_x = default_frame.xMax-default_frame.xMin;
        int delta_y = default_frame.yMax-default_frame.yMin;

        if (x < (int)Conf::world_size && y < (int)Conf::world_size && x >= 0 && y >= 0) {
            switch (button_type)
            {
            case LEFT:
                new_birth(x, y);
                break;
            case RIGHT:
                new_death(x, y);
                break;
            case SCROLL:
                if (x < (delta_x)/4 + default_frame.xMin) {
                    on_leftarrow_pressed();
                }else if (x >= (delta_x*3/4) + default_frame.xMin) {
                    on_rightarrow_pressed();
                }
                if (y < delta_y/4 + default_frame.yMin) {
                    on_downarrow_pressed();
                }else if (y >= delta_y*3/4 + default_frame.yMin) {
                    on_uparrow_pressed();
                }
                break;
            default:
                button_type = NONE;
                break;
            }
            m_Area.refresh();
        }
    }
    return true;
}

bool SimulationWindow::on_enter_notify_event(GdkEventCrossing * crossing_event) {
    Glib::RefPtr<Gdk::Window> ref_window(m_Area.get_window());
    if (crossing_event->type == GDK_ENTER_NOTIFY) {
        ref_window->set_cursor(Gdk::Cursor::create(Gdk::CursorType::PENCIL));
    }
    else if (crossing_event->type == GDK_LEAVE_NOTIFY) {
        ref_window->set_cursor(Gdk::Cursor::create(Gdk::CursorType::ARROW));
    }
    return true;
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

bool SimulationWindow::on_idle() {
    return true;
}

bool SimulationWindow::on_timeout() {
    if (disconnect) {
        disconnect = false;
        return false;
    }

    if (m_Button_Canon.get_active()) m_Button_Canon.set_sensitive(false);
    if (m_Button_Max.get_active()) m_Button_Max.set_sensitive(false);

    if (experiment) {
        if (update(EXPERIMENTAL)) {
            // Update the stability detection status label
            m_Label_Test.set_label("<span foreground='green'>Stable</span>");

            // Reset the properties of the start button
            m_Button_Start.set_label("Start");
            Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
            css_provider->load_from_data("button {background-image: image(green);}");
            m_Button_Start.get_style_context()->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
            
            // Stop the timer
            on_event_delete_timer();            
        }else {
            m_Label_Test.set_label("<span foreground='blue'>S.D. ON</span>");
            ++val;
        }
    }else {
        update();
        m_Label_Test.set_label("S.D. OFF");
        ++val;
    }
    m_Label_Info.set_label("<b>Generation : " + std::to_string(val) + "</b>");
    m_Area.refresh();
    return true;
}

bool SimulationWindow::on_key_press_event(GdkEventKey * key_event) {
    if (key_event->type == GDK_KEY_PRESS) {

        if (key_event->keyval == GDK_KEY_space) {
            SimulationWindow::on_button_start_clicked();
            return Gtk::Window::on_key_press_event(key_event);
        }

        switch(gdk_keyval_to_unicode(key_event->keyval)) {
            case '1':
                SimulationWindow::on_button_step_clicked();
                break;
            case '2':
                SimulationWindow::on_button_clear_clicked();
                break;
            case '3':
                SimulationWindow::on_button_random_clicked();
                break;
            case '4':
                if (m_Button_Canon.get_active()) m_Button_Canon.set_active(false);
                else m_Button_Canon.set_active(true);
                SimulationWindow::on_button_gosper_clicked();
                break;
            case '5':
                if (m_Button_Max.get_active()) m_Button_Max.set_active(false);
                else m_Button_Max.set_active(true);
                SimulationWindow::on_button_max_clicked();
                break;
            case 'h':
                SimulationWindow::on_leftarrow_pressed();
                break;
            case 'j':
                SimulationWindow::on_downarrow_pressed();
                break;
            case 'k':
                SimulationWindow::on_uparrow_pressed();
                break;
            case 'l':
                SimulationWindow::on_rightarrow_pressed();
                break;
            default:
                break;
        }
    }
    return Gtk::Window::on_key_press_event(key_event);
}

void SimulationWindow::create_MenuBar() {
    m_MenuBar.append(fileMi);
    fileMi.set_submenu(m_FileMenu);
    m_FileMenu.append(openMi);
    m_FileMenu.append(saveMi);
    m_FileMenu.append(saveasMi);
    m_FileMenu.append(file_sepMi);
    m_FileMenu.append(quitMi);
    m_MenuBar.append(viewMi);
    viewMi.set_submenu(m_ViewMenu);
    m_ViewMenu.append(zoominMi);
    m_ViewMenu.append(zoomoutMi);
    m_ViewMenu.append(resetzoomMi);
    m_ViewMenu.append(view_sepMi);
    m_ViewMenu.append(showgridMi);
    m_ViewMenu.append(fadeMi);
    m_ViewMenu.append(darkmode);
    m_MenuBar.append(toolsMi);
    toolsMi.set_submenu(m_ToolsMenu);
    m_ToolsMenu.append(experimentMi);
    m_ToolsMenu.append(simsizeMi);
    simsizeMi.set_submenu(m_SimsizeMenu);
    m_SimsizeMenu.append(incrsizeMi);
    m_SimsizeMenu.append(decrsizeMi);
    m_MenuBar.append(m_helpMi);
    m_helpMi.set_submenu(m_HelpMenu);
    m_HelpMenu.append(helpMi);
    m_HelpMenu.append(aboutMi);

    m_MenuBar.set_valign(Gtk::ALIGN_START);
    m_MenuBar.set_vexpand(false);
    experimentMi.set_tooltip_text("stop the simulation when stable");
}

void SimulationWindow::MenuBar_signals_hdl() {
    openMi.signal_activate().connect(sigc::mem_fun(*this, 
                &SimulationWindow::on_button_open_clicked));

    saveMi.set_sensitive(false);
    saveMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_save_clicked));

    saveasMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_saveas_clicked));

    quitMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_quit_clicked));

    zoominMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_zoom_in_clicked));

    zoomoutMi.set_sensitive(false);
    zoomoutMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_zoom_out_clicked));

    resetzoomMi.set_sensitive(false);
    resetzoomMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_reset_zoom_clicked));

    showgridMi.set_active(false);
    showgridMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_checkbutton_grid_checked));

    fadeMi.set_active(false);
    fadeMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_checkbutton_fade_checked));

    darkmode.set_active(false);
    darkmode.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_checkbutton_dark_checked));

    experimentMi.set_active(false);
    experimentMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_test_clicked));

    incrsizeMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_increase_size_clicked));
    if (Conf::world_size == world_size_max) {
        incrsizeMi.set_sensitive(false);
    }

    decrsizeMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_decrease_size_clicked));
    if (Conf::world_size == world_size_min) {
        decrsizeMi.set_sensitive(false);
    }

    helpMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_help_clicked));

    aboutMi.signal_activate().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_about_clicked));
}

void SimulationWindow::MenuBar_accelerators() {
    Glib::RefPtr<Gtk::AccelGroup> accel_group = Gtk::AccelGroup::create();
    this->add_accel_group(accel_group);
    openMi.add_accelerator("activate", accel_group, GDK_KEY_o,
                            Gdk::CONTROL_MASK, Gtk::ACCEL_VISIBLE);
    saveMi.add_accelerator("activate", accel_group, GDK_KEY_s,
                            Gdk::CONTROL_MASK, Gtk::ACCEL_VISIBLE);
    saveasMi.add_accelerator("activate", accel_group, GDK_KEY_s,
                            Gdk::SHIFT_MASK, Gtk::ACCEL_VISIBLE);
    quitMi.add_accelerator("activate", accel_group, GDK_KEY_q,
                            Gdk::CONTROL_MASK, Gtk::ACCEL_VISIBLE);
    zoominMi.add_accelerator("activate", accel_group, GDK_KEY_plus,
                            Gdk::CONTROL_MASK, Gtk::ACCEL_VISIBLE);
    zoomoutMi.add_accelerator("activate", accel_group, GDK_KEY_minus,
                            Gdk::CONTROL_MASK, Gtk::ACCEL_VISIBLE);
    resetzoomMi.add_accelerator("activate", accel_group, GDK_KEY_0,
                            Gdk::CONTROL_MASK, Gtk::ACCEL_VISIBLE);
    incrsizeMi.add_accelerator("activate", accel_group, GDK_KEY_Page_Up,
                            Gdk::CONTROL_MASK, Gtk::ACCEL_VISIBLE);
    decrsizeMi.add_accelerator("activate", accel_group, GDK_KEY_Page_Down,
                            Gdk::CONTROL_MASK, Gtk::ACCEL_VISIBLE);
    experimentMi.add_accelerator("activate", accel_group, GDK_KEY_t,
                            Gdk::CONTROL_MASK, Gtk::ACCEL_VISIBLE);
    helpMi.add_accelerator("activate", accel_group, GDK_KEY_F1,
                            Gdk::RELEASE_MASK, Gtk::ACCEL_VISIBLE);
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
    m_Button_Start.signal_clicked().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_start_clicked));
    m_Button_Start.set_property("can-focus", false);

    // Create Step button
    m_ButtonBox.pack_start(m_Button_Step);
    m_Button_Step.signal_clicked().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_step_clicked));
    m_Button_Step.set_property("can-focus", false);

    // Create Clear button
    m_ButtonBox.pack_start(m_Button_Clear);
    m_Button_Clear.signal_clicked().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_clear_clicked));
    m_Button_Clear.set_property("can-focus", false);

    // Create Random button
    m_ButtonBox.pack_start(m_Button_Random);
    m_Button_Random.signal_clicked().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_random_clicked));
    m_Button_Random.set_property("can-focus", false);

    // Create Reset button
    m_ButtonBox.pack_start(m_Button_Reset);
    m_Button_Reset.signal_clicked().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_reset_clicked));
    m_Button_Reset.set_property("can-focus", false);
    m_Button_Reset.set_sensitive(false);

    // Create a separator
    m_ButtonBox.pack_start(m_Sep);
    m_Sep.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

    // Create GosperCanon button
    m_ButtonBox.pack_start(m_Button_Canon);
    m_Button_Canon.signal_toggled().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_gosper_clicked));
    m_Button_Canon.set_property("can-focus", false);

    // Create SpaceFiller button
    m_ButtonBox.pack_start(m_Button_Max);
    m_Button_Max.signal_toggled().connect(sigc::mem_fun(*this,
            &SimulationWindow::on_button_max_clicked));
    m_Button_Max.set_property("can-focus", false);
}

void SimulationWindow::create_StatusBar() {
    // - for the simulation size
    m_LabelSize.set_text(m_LabelSize.get_text() + std::to_string(Conf::world_size)
                        + " x " + std::to_string(Conf::world_size));
    m_LabelSize.set_halign(Gtk::ALIGN_END);
    m_LabelSize.set_hexpand(false);
    // - for the zoom level
    m_LabelZoom.set_text(std::to_string(zoom) + "%");
    m_LabelZoom.set_halign(Gtk::ALIGN_END);
    m_LabelZoom.set_hexpand(false);
    // - for the coordinates
    m_LabelCoordinates.set_text("x : " + x + "    y : " +y);
    m_LabelCoordinates.set_halign(Gtk::ALIGN_END);
    m_LabelCoordinates.set_hexpand(false);
    // - for the sim. info
    m_Label_Info.set_label("<b>Generation : " + std::to_string(val) + "</b>");
    m_Label_Info.set_halign(Gtk::ALIGN_END);
    m_Label_Info.set_hexpand(false);
    m_Label_Info.set_use_markup(true);

    m_Label_Test.set_label("S.D. OFF");
    m_Label_Test.set_use_markup(true);

    // Create StatusBar
    m_StatusBar.pack_end(m_LabelSize);
    m_StatusBar.pack_end(m_LabelZoom);
    m_StatusBar.pack_end(m_LabelCoordinates);
    m_StatusBar.pack_end(m_Label_Info);
    m_StatusBar.pack_end(m_Label_Test);
    m_StatusBar.set_valign(Gtk::ALIGN_END);
    m_StatusBar.set_vexpand(false);
    m_StatusBar.set_halign(Gtk::ALIGN_BASELINE);
}

/************************************************************************

*	Game of Life -- GUI with various options and view controls
*	Copyright (C) 2022 Cyprien Lacassagne

*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.

*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.

*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <https://www.gnu.org/licenses/>.

*************************************************************************/

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

static unsigned val(1);  // The simulation starts at generation number 1
static unsigned zoom(100);
static unsigned z_ratio(100);
static unsigned _theme_(1);
static bool grid_shown(false);
static bool accept_show_grid(false);
static bool first_zoom(true);

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
        cr->set_source_rgb(white.r, white.g, white.b);
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

        graphic_draw_world(Conf::world_size, Conf::world_size, _theme_);
        draw_world(1-_theme_);
        refresh();
    }
    return true;
}

//===========================================

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
    darkmode("Dark theme"),
    experimentMi("Experiment"),
    simsizeMi("World size"),
    incrsizeMi("Extend"),
    decrsizeMi("Shrink"),
    helpMi("Help"),
    aboutMi("About"),
    m_SuperBox(Gtk::ORIENTATION_VERTICAL),
    m_HeaderBox(Gtk::ORIENTATION_HORIZONTAL),
    m_Box(Gtk::ORIENTATION_HORIZONTAL),
    m_GuiBox(Gtk::ORIENTATION_VERTICAL),
    m_GraphicBox(Gtk::ORIENTATION_HORIZONTAL, 10),
    m_ButtonBox(Gtk::ORIENTATION_VERTICAL, 10),
    m_Box_General(Gtk::ORIENTATION_VERTICAL, 10),
    m_Appearance_Box(Gtk::ORIENTATION_VERTICAL, 10),
    m_Frame_General("General"),
    m_Frame_Speed("Refresh rate (ms)"),
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
    timeout_value(50),
    filename(__filename),
    frame_surface(default_frame.xMax*default_frame.xMax),
    x(std::to_string(default_frame.xMax / 2)),
    y(std::to_string(default_frame.yMax / 2))
    {
        m_Box.set_border_width(10);
        m_GuiBox.set_border_width(5);
        m_GraphicBox.set_border_width(5);
        m_ButtonBox.set_border_width(0);
        m_ButtonBox.set_size_request(150);

        add(m_SuperBox);
        m_SuperBox.pack_start(m_MenuBar, false, false);
        m_SuperBox.pack_start(m_Separator, false, false);
        m_SuperBox.pack_start(m_Box);
        m_SuperBox.pack_start(m_StatusBar, false, false);

        m_Separator.set_valign(Gtk::ALIGN_START);
        m_Separator.set_vexpand(false);

        // Create MenuBar
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

        // Set some properties
        m_MenuBar.set_valign(Gtk::ALIGN_START);
        m_MenuBar.set_vexpand(false);

        experimentMi.set_tooltip_text("stop the simulation when stable");

        // Link the buttons to their signal handler
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

        darkmode.set_active(false);
        darkmode.signal_activate().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_checkbutton_dark_checked));

        experimentMi.signal_activate().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_test_clicked));

        incrsizeMi.signal_activate().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_increase_size_clicked));

        decrsizeMi.signal_activate().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_decrease_size_clicked));

        helpMi.signal_activate().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_help_clicked));

        aboutMi.signal_activate().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_about_clicked));

        // Create accelerators (shortcuts) for MenuBar items
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

        // Put the containers into their parent box
        m_Box.pack_start(m_GuiBox, false, false);
        m_Box.pack_start(m_GraphicBox);
        m_GuiBox.pack_start(m_ButtonBox, false, false);
        m_GraphicBox.pack_start(m_Area);
        m_ButtonBox.pack_start(m_Box_General);
        // m_Frame_General.add(m_Box_General);
        m_ButtonBox.pack_start(m_Frame_Speed);
        m_Frame_Speed.add(m_Scale);

        // Create Refresh Scale
        m_Scale.set_range(10, 100);
        m_Scale.set_round_digits(0);
        m_Scale.set_value(50);
        m_Scale.set_inverted(true);
        m_Scale.add_mark(10, Gtk::POS_BOTTOM, "10");
	    m_Scale.add_mark(20, Gtk::POS_TOP, "20");
        m_Scale.add_mark(30, Gtk::POS_BOTTOM, "30");
	    m_Scale.add_mark(40, Gtk::POS_TOP, "40");
        m_Scale.add_mark(50, Gtk::POS_BOTTOM, "50");
	    m_Scale.add_mark(60, Gtk::POS_TOP, "60");
	    m_Scale.add_mark(70, Gtk::POS_BOTTOM, "70");
	    m_Scale.add_mark(80, Gtk::POS_TOP, "80");
	    m_Scale.add_mark(90, Gtk::POS_BOTTOM, "90");
        m_Scale.add_mark(100, Gtk::POS_TOP, "100");
        m_Scale.set_draw_value(false);
        m_Scale.set_show_fill_level(true);
        m_Scale.set_has_origin(false);
        m_Scale.set_increments(20, 0);
        m_Scale.set_property("can-focus", false);

        // Create Start button
        m_Box_General.pack_start(m_Button_Start);
        Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
        css_provider->load_from_data("button {background-image: image(green);}");
        m_Button_Start.get_style_context()->add_provider(css_provider,
            GTK_STYLE_PROVIDER_PRIORITY_USER);
        m_Button_Start.override_color(Gdk::RGBA("white"));
        m_Button_Start.signal_clicked().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_start_clicked));
        m_Button_Start.set_property("can-focus", false);

        // Create Step button
        m_Box_General.pack_start(m_Button_Step);
        m_Button_Step.signal_clicked().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_step_clicked));
        m_Button_Step.set_property("can-focus", false);

        // Create Clear button
        m_Box_General.pack_start(m_Button_Clear);
        m_Button_Clear.signal_clicked().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_clear_clicked));
        m_Button_Clear.set_property("can-focus", false);

        // Create Random button
        m_Box_General.pack_start(m_Button_Random);
        m_Button_Random.signal_clicked().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_random_clicked));
        m_Button_Random.set_property("can-focus", false);

        // Create Reset button
        m_Box_General.pack_start(m_Button_Reset);
        m_Button_Reset.signal_clicked().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_reset_clicked));
        m_Button_Reset.set_property("can-focus", false);
        m_Button_Reset.set_sensitive(false);

        // Create a separator
        m_Box_General.pack_start(m_Sep);
        m_Sep.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

        // Create GosperCanon button
        m_Box_General.pack_start(m_Button_Canon);
        m_Button_Canon.signal_toggled().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_gosper_clicked));
        m_Button_Canon.set_property("can-focus", false);

        // Create SpaceFiller button
        m_Box_General.pack_start(m_Button_Max);
        m_Button_Max.signal_toggled().connect(sigc::mem_fun(*this,
                &SimulationWindow::on_button_max_clicked));
        m_Button_Max.set_property("can-focus", false);

        // Create Status labels
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
        m_Label_Info.set_text("Generation : ");
        m_Label_Info.set_halign(Gtk::ALIGN_END);
        m_Label_Info.set_hexpand(false);

        // Create StatusBar
        m_StatusBar.pack_end(m_LabelSize);
        m_StatusBar.pack_end(m_LabelZoom);
        m_StatusBar.pack_end(m_LabelCoordinates);
        m_StatusBar.pack_end(m_Label_Info);
        m_StatusBar.pack_end(m_Label_Test);
        m_StatusBar.set_valign(Gtk::ALIGN_END);
        m_StatusBar.set_vexpand(false);
        m_StatusBar.set_halign(Gtk::ALIGN_BASELINE);

        if (!_theme_) {
            darkmode.set_active(true);
        }

///////////////        Glib::signal_idle().connect(sigc::mem_fun(*this, &SimulationWindow::on_idle)); ///////////////////////
        show_all_children();

        // Check for any file reading error when a file is passed as argument
        file_error_dialog();
    }

// Display an error message in case of bad configuration or uncompatible file
void SimulationWindow::file_error_dialog() {
    if (get_error_id() == 3) {
        std::string m1("Failed configuring the simulation (invalid values)\n");
        Gtk::MessageDialog error_dialog(m1, true, 
                                        Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, false);
        
        // Font options
        if (_theme_) {
            error_dialog.set_message("<span size='large' foreground='#000099' letter_spacing='1024'>"
                            + m1 + "</span>", true);
        }else {
            error_dialog.set_message("<span size='large' foreground='#C0C0C0' letter_spacing='1024'>"
                            + m1 + "</span>", true);
        }

        std::vector<Gtk::Widget*> children = error_dialog.get_action_area()->get_children();
        children[0]->set_margin_left(400);              
        error_dialog.set_icon_from_file("share/icons/notifications.ico");
        error_dialog.set_transient_for(*this);
        this->set_title("Game of Life");

        int result = error_dialog.run();

        switch(result) {
            case(Gtk::RESPONSE_OK):
                break;
            default:
                break;
        }
    }
    if (get_error_id() == 1) {
        std::string m1("Failed opening the file\nMake sure the file exists");
        Gtk::MessageDialog error_dialog(m1, true,
                                        Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        // Font options
        if (_theme_) {
            error_dialog.set_message("<span size='large' foreground='#000099' letter_spacing='1024'>"
                            + m1 + "</span>", true);
        }else {
            error_dialog.set_message("<span size='large' foreground='#C0C0C0' letter_spacing='1024'>"
                            + m1 + "</span>", true);
        }

        error_dialog.set_transient_for(*this);
        error_dialog.run();
        this->set_title("Game of Life");
    }
}

void SimulationWindow::set_compliant_theme() {
    #ifdef _WIN32
    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
    Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
    Glib::RefPtr<Gio::File> css_file = Gio::File::create_for_path("share/themes/Windows102/gtk-3.0/gtk.css");
    css_provider->load_from_file(css_file);
    this->get_style_context()->add_provider_for_screen(screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    #elif __linux__
    Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
    Glib::RefPtr<Gdk::Screen> screen = Gdk::Screen::get_default();
    Glib::RefPtr<Gio::File> css_file = Gio::File::create_for_path("share/themes/Windows10Dark/gtk-3.0/gtk.css");
    css_provider->load_from_file(css_file);
    this->get_style_context()->add_provider_for_screen(screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    // #elif __APPLE__
    #endif
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

void SimulationWindow::updt_statusbar_labels() {
    x = std::to_string((default_frame.xMax + (int)default_frame.xMin) / 2 + 1);
    y = std::to_string((default_frame.yMax + (int)default_frame.yMin) / 2 + 1);
    m_LabelCoordinates.set_text("x : " + x + "    y : " + y);
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
        if (filename != "" && !saveMi.get_sensitive()) {
            saveMi.set_sensitive(true);
            m_Button_Reset.set_sensitive(true);
            this->set_title("*" + this->get_title());
        }
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
        if (filename != "" && !saveMi.get_sensitive()) {
            saveMi.set_sensitive(true);
            m_Button_Reset.set_sensitive(true);
            this->set_title("*" + this->get_title());
        }
    }
}

void SimulationWindow::on_button_reset_clicked() {
    m_Button_Reset.set_sensitive(false);
    init();
    read_file(filename);
    this->set_title(this->get_title().replace(0, 1, ""));

    // Reset labels and buttons
    m_Label_Info.set_text("Generation: ");
    m_Button_Canon.set_active(false);
    m_Button_Canon.set_sensitive(true);
    m_Button_Max.set_active(false);
    m_Button_Max.set_sensitive(true);
    saveMi.set_sensitive(false);
    val = 1;
}

void SimulationWindow::on_button_clear_clicked() {
    init();
    m_Area.refresh();
    this->set_title("Game of Life");
    m_Label_Info.set_text("Generation: ");
    filename = "";
    if (experiment) {
        m_Label_Test.set_text("Experimental");
    }

    // Reset the state of both buttons below
    m_Button_Canon.set_active(false);
    m_Button_Canon.set_sensitive(true);
    m_Button_Max.set_active(false);
    m_Button_Max.set_sensitive(true);
    saveMi.set_sensitive(false);
    val = 1;
}

void SimulationWindow::on_button_random_clicked() {
    on_button_clear_clicked();

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
    Gtk::FileChooserDialog dialog("Open", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);
    dialog.add_button("_Open", Gtk::RESPONSE_OK);
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);

    int result = dialog.run();

    switch(result) {
        case(Gtk::RESPONSE_OK): {
            dialog.hide();

            // Initialize the simulation with the chosen document
            init();
            filename = (dialog.get_filename());
            read_file(filename);

            // Display an error message in case of bad configuration or uncompatible file
            if (get_error_id() == 1) {
                std::string err_mess("Failed opening the file\nMake sure the file exists");
                error_dialog_open(err_mess);
                return;
            }

            if (get_error_id() == 3) {
                std::string err_mess("Failed configuring the simulation due to invalid values\n");
                error_dialog_open(err_mess);
                return;
            }

            if (get_error_id() == 4) {
                std::string err_mess("The file has to be a plain text file (.txt)");
                error_dialog_open(err_mess);
                return;
            }

            set_default_frame();
            accept_show_grid = false;
            updt_statusbar_labels();

            m_LabelSize.set_text(std::to_string(Conf::world_size) + " x "
                       + std::to_string(Conf::world_size));

            unsigned pos = filename.find_last_of('\\');
            this->set_title(filename.replace(0, pos + 1, "") + "  -  Game of Life");

            if (Conf::world_size >= 300 && showgridMi.get_active()) {
                disable_show_grid();
                showgridMi.set_active(false);
            }
            m_Area.refresh();
            break;
        }
        case(Gtk::RESPONSE_CANCEL): {
            break;
        }
        default: {
            break;
        }
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
    Gtk::FileChooserDialog dialog("Save as", Gtk::FILE_CHOOSER_ACTION_SAVE);
    
    dialog.set_transient_for(*this);
    dialog.add_button("_Save", Gtk::RESPONSE_OK);
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);

    int result = dialog.run();

    switch(result) {
        case(Gtk::RESPONSE_OK): {
            filename = dialog.get_filename();
            save_file(filename);
            break;
        }
        case(Gtk::RESPONSE_CANCEL): {
            break;
        }
        default: {
            break;
        }
    }
}

void SimulationWindow::on_button_test_clicked() {
    if (experiment) {
        experiment = false;
        m_Label_Test.set_text("");
    }else {
        experiment = true;
        m_Label_Test.set_text("Experimental");
    }
}

void SimulationWindow::on_button_zoom_in_clicked() {
    // If first time the user zooms in, show how to navigate throughout 
    // the world
    if (first_zoom) {
        std::string msg("To move the frame, " \
                        "you can use 4 differents keys, for the four directions:\n\n" \
                        "'H' to go left\n'J' to go down\n'K' to go up, and\n'L' to go right");
        Gtk::MessageDialog nav_com_dial(msg, true, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, false);
        // Font options
        if (_theme_) {
            nav_com_dial.set_message("<span size='large' foreground='#000099' letter_spacing='1024'>"
                            + msg + "</span>", true);
        }else {
            nav_com_dial.set_message("<span size='large' foreground='#C0C0C0' letter_spacing='1024'>"
                            + msg + "</span>", true);
        }
        std::vector<Gtk::Widget*> children = nav_com_dial.get_action_area()->get_children();
        children[0]->set_margin_left(dialog_button_margin);
        nav_com_dial.set_transient_for(*this);
        nav_com_dial.run();
        first_zoom = false;
    }

    // Increase the zoom level and decrease the view angle (z ratio)
    zoom += 10;
    z_ratio -= 10;
    zoom_frame();
    m_LabelZoom.set_text(std::to_string(zoom) + "%");
    updt_statusbar_labels();

    if (z_ratio == 0) {
        zoominMi.set_sensitive(false);
    }
    zoomoutMi.set_sensitive(true);
    resetzoomMi.set_sensitive(true);
}

void SimulationWindow::on_button_zoom_out_clicked() {

    // Decrease the zoom level and increase the view angle (z ratio)
    zoom -= 10;
    z_ratio += 10;
    zoom_frame();
    m_LabelZoom.set_text(std::to_string(zoom) + "%");
    updt_statusbar_labels();

    if (zoom == 100) {
        zoomoutMi.set_sensitive(false);
    }
    zoominMi.set_sensitive(true);
}

void SimulationWindow::on_button_reset_zoom_clicked() {
    // Reset zoom values and the zoom label
    zoom = 100;
    z_ratio = 100;
    m_LabelZoom.set_text(std::to_string(zoom) + "%");

    // Reset the zoom level of the frame
    set_default_frame();
    updt_statusbar_labels();
}

void SimulationWindow::on_leftarrow_pressed() {
    if (zoom > 100 && default_frame.xMin > -1) {
        
        // Translate the frame to the left
        default_frame.xMin -= 1;
        default_frame.xMax -= 1;
        updt_statusbar_labels();
        m_Area.refresh();
    }
}

void SimulationWindow::on_rightarrow_pressed() {
    if (zoom > 100 && default_frame.xMax < Conf::world_size) {

        // Translate the frame to the right
        default_frame.xMin += 1;
        default_frame.xMax += 1;

        updt_statusbar_labels();
        m_Area.refresh();
    }
}

void SimulationWindow::on_uparrow_pressed() {
    if (zoom > 100 && default_frame.yMax < Conf::world_size) {

        // Translate the frame to the top
        default_frame.yMin += 1;
        default_frame.yMax += 1;

        updt_statusbar_labels();
        m_Area.refresh();
    }
}

void SimulationWindow::on_downarrow_pressed() {
    if (zoom > 100 && default_frame.yMin > -1) {

        // Translate the frame to the bottom
        default_frame.yMin -= 1;
        default_frame.yMax -= 1;

        updt_statusbar_labels();
        m_Area.refresh();
    }
}

void SimulationWindow::on_button_increase_size_clicked() {
    Conf::set_world_size(Conf::world_size + 50);
    if (Conf::world_size == 500) {
        incrsizeMi.set_sensitive(false);
    }
    decrsizeMi.set_sensitive(true);
    set_default_frame();
    adjust_zoom();
    m_LabelSize.set_text(std::to_string(Conf::world_size) + " x "
                       + std::to_string(Conf::world_size));
}

void SimulationWindow::on_button_decrease_size_clicked() {
    Conf::set_world_size(Conf::world_size - 50);
    if (Conf::world_size == 50) {
        decrsizeMi.set_sensitive(false);
    }
    incrsizeMi.set_sensitive(true);
    set_default_frame();
    adjust_zoom();
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

void SimulationWindow::on_checkbutton_grid_checked() {
    if (showgridMi.get_active()) {
        if (Conf::world_size > 300 && !accept_show_grid) {
            
            // Create "don't show again" button and connect its signal handler
            Gtk::CheckButton dontshowagain("don't show again");
            dontshowagain.signal_clicked().connect(sigc::mem_fun(*this,
                    &SimulationWindow::on_dontshowagain_checked));

            // Create the message dialog
            std::string m1("Displaying the grid might reduce the performances for a resolution above 300x300\n\n");
            std::string m2("Beyond this size, the grid becomes also barely seeable");
            std::string message(m1 + m2);
            Gtk::MessageDialog dialog(message, true, Gtk::MESSAGE_INFO, Gtk::BUTTONS_NONE, false);

            // Set font/icon options
            if (_theme_) {
                dialog.set_message("<span size='large' foreground='#000099' letter_spacing='1024'>"
                                    + message + "</span>", true);
            }else {
                dialog.set_message("<span size='large' foreground='#C0C0C0' letter_spacing='1024'>"
                                    + message + "</span>", true);
            }
            Glib::RefPtr<Gdk::Pixbuf> Info_ptr = Gdk::Pixbuf::create_from_file("share/icons/cs-notifications.png");
            dialog.set_icon(Info_ptr);

            // Add two response buttons
            dialog.add_button("_Don't show the grid", 0);
            dialog.add_button("_Show anyway", 1);

            // // Set a white background for the message area
            // Gtk::Box *marea(dialog.get_message_area());
            // marea->set_halign(Gtk::ALIGN_FILL);
            // Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
            // css_provider->load_from_data("box {background-image: image(white);}");
            // marea->get_style_context()->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

            // Add an info icon and the "don't show again" button to the content area
            Gtk::Box *carea(dialog.get_content_area());
            carea->set_border_width(0);
            carea->pack_start(dontshowagain);
            dontshowagain.set_margin_left(50);

            // Set properties to the response buttons
            std::vector<Gtk::Widget*> children = dialog.get_action_area()->get_children();
            children[0]->set_margin_left(5);
            children[0]->set_halign(Gtk::ALIGN_START);
            children[0]->set_size_request(200);
            children[0]->grab_focus();
            children[1]->set_margin_right(5);
            children[1]->set_size_request(150);
            children[1]->set_halign(Gtk::ALIGN_END);

            dialog.show_all_children();
            dialog.set_transient_for(*this);

            int result = dialog.run();

            switch(result) {
                case(1):
                    enable_show_grid();
                    break;
                case(0):
                    showgridMi.set_active(false);
                    break;
                default:
                    showgridMi.set_active(false);
                    break;
            }
        }else {
            enable_show_grid();
        }
        grid_shown = true;
    }else {
        disable_show_grid();
    }
}

void SimulationWindow::on_dontshowagain_checked() {
    accept_show_grid = true;
}

void SimulationWindow::on_button_help_clicked() {
    std::ifstream f("README.txt");
    std::string readme;
    std::ostringstream ss;
    ss << f.rdbuf();
    readme = ss.str();
    Gtk::MessageDialog help_dial(readme, true);
    Gtk::Box* marea = help_dial.get_message_area();
    Gtk::Scrollbar scrollbar;
    scrollbar.set_orientation(Gtk::ORIENTATION_VERTICAL);
    marea->set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    marea->pack_start(scrollbar);
    help_dial.set_transient_for(*this);
    help_dial.show_all_children();
    help_dial.run();
}

void SimulationWindow::on_button_about_clicked() {

    // GdkPixbuf *example_logo = gdk_pixbuf_new_from_file ("./beetle.png", NULL);
    // gtk_show_about_dialog (NULL,
    //                    "program-name", "ExampleCode",
    //                    "logo", example_logo,
    //                    "title", "About ExampleCode",
    //                    NULL);   

    Gtk::AboutDialog about_dial;
    std::ifstream f("license.txt");
    std::string license;
    std::ostringstream ss;
    ss << f.rdbuf();
    license = ss.str();
    // std::string license(
    //     "This program is free software: you can redistribute it and/or modify\n"
    //     "it under the terms of the GNU General Public License as published by\n"
    //     "the Free Software Foundation, either version 3 of the License, or\n"
    //     "(at your option) any later version.\n\n" 

    //     "This program is distributed in the hope that it will be useful,\n" 
    //     "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" 
    //     "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    //     "GNU General Public License for more details.\n\n"

    //     "You should have received a copy of the GNU General Public License\n" 
    //     "along with this program.  If not, see <https://www.gnu.org/licenses/>.);"
    // );

    std::vector<Glib::ustring> authors(1, "Cyprien Lacassagne");
    std::vector<Glib::ustring> contributors(1, "Julian Ruiz Rodriguez");

    about_dial.set_program_name("Game of Life");
    about_dial.set_authors(authors);
    about_dial.set_copyright("Copyright (C) 2022 Cyprien Lacassagne");
    // about_dial.set_license_type(Gtk::LICENSE_GPL_3_0);
    about_dial.set_license(license);
    about_dial.set_wrap_license(true);
    about_dial.set_version("0.1.0");
    // about_dial.set_website("https://www.geany.org/");
    about_dial.set_hexpand(false);
    about_dial.add_credit_section("Contributor", contributors);
    about_dial.set_comments("Inspired by John Conway's game of life");

    about_dial.show_all_children();
    about_dial.set_transient_for(*this);
    about_dial.run();
}

void SimulationWindow::on_button_gosper_clicked() {
    if (m_Button_Canon.get_active()) {
        draw_canon_planeur(0, 0);
        if (filename != "" && !saveMi.get_sensitive()) {
            saveMi.set_sensitive(true);
            m_Button_Reset.set_sensitive(true);
            this->set_title("*" + this->get_title());
        }
    }else {
        erase_canon_planeur(0, 0);
        if (filename != "" && !saveMi.get_sensitive()) {
            saveMi.set_sensitive(true);
            m_Button_Reset.set_sensitive(true);
            this->set_title("*" + this->get_title());
        }
    }
    m_Area.refresh();
}

void SimulationWindow::on_button_max_clicked() {
    if (m_Button_Max.get_active()) {
        draw_spacefiller(Conf::world_size/2 - 13, Conf::world_size/2 - 13);
    }else {
        erase_spacefiller(Conf::world_size/2 - 13, Conf::world_size/2 - 13);
    }
    m_Area.refresh();
}

bool SimulationWindow::on_button_press_event(GdkEventButton * event)
{
	if(event->type == GDK_BUTTON_PRESS)
	{
		// raw mouse coordinates in the window frame
		double clic_x = event->x ;
		double clic_y = event->y ;
        std::cout << "raw : " << clic_x << " " << clic_y << "\n";

		// origin of the drawing area
		double origin_x = m_Area.get_allocation().get_x();
		double origin_y = m_Area.get_allocation().get_y();
        std::cout << "origin : " << origin_x << " " << origin_y << "\n";

		// get width and height of drawing area
		double width = m_Area.get_allocation().get_width();
		double height= m_Area.get_allocation().get_height();
        std::cout << "area dim : " << width << " " << height << "\n";

		// retain only mouse events located within the drawing area
		if(clic_x >= origin_x && clic_x <= origin_x + width &&
		   clic_y >= origin_y && clic_y <= origin_y + height) { 
			// Point that we are allowed to use expressed with drawing area coord.
			Point p({clic_x - origin_x, clic_y -origin_y});
            int x(round(p.x*Conf::world_size/width));
            int y(round(Conf::world_size - (p.y*Conf::world_size/height)));

            std::cout << x << "    " << y << "\n";

            if (x < (int)Conf::world_size && y < (int)Conf::world_size) {

                // A left click gives birth to a new cell
                if (event->button == 1) {
                    new_birth(x, y);
                    m_Area.refresh();

                // A right click kills an alive cell
                }else if (event->button == 3) {
                    new_death(x, y);
                    m_Area.refresh();
                }
                if (filename != "" && !saveMi.get_sensitive()) {
                    // Show to the user that the file has been modified
                    saveMi.set_sensitive(true);
                    m_Button_Reset.set_sensitive(true);
                    this->set_title("*" + this->get_title());
                }
            }
		}
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

    // Do not allow the user to interact with these special buttons
    // while a simulation is running
    if (m_Button_Canon.get_active()) m_Button_Canon.set_sensitive(false);
    if (m_Button_Max.get_active()) m_Button_Max.set_sensitive(false);

    if (experiment) {
        if (update(EXPERIMENTAL)) {
            Gtk::Label m_val(std::to_string(val-4));

            // Reset the properties of the start button
            m_Button_Start.set_label("Start");
            Glib::RefPtr<Gtk::CssProvider> css_provider = Gtk::CssProvider::create();
            css_provider->load_from_data("button {background-image: image(green);}");
            m_Button_Start.get_style_context()->add_provider(css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
            
            // Stop the timer
            on_event_delete_timer();            
        }else {
            m_Label_Test.set_text("Experimental");
            ++val;
        }
    }else {
        update();
        m_Label_Test.set_text("");
        ++val;
    }
    m_Label_Info.set_text("Generation : " + std::to_string(val));
    m_Area.refresh();
    return true;
}

bool SimulationWindow::on_key_press_event(GdkEventKey * key_event) {
    if (key_event->type == GDK_KEY_PRESS) {
        switch(gdk_keyval_to_unicode(key_event->keyval)) {
            case '1':
                SimulationWindow::on_button_start_clicked();
                break;
            case '2':
                SimulationWindow::on_button_step_clicked();
                break;
            case '3':
                SimulationWindow::on_button_clear_clicked();
                break;
            case '4':
                SimulationWindow::on_button_random_clicked();
                break;
            case '5':
                SimulationWindow::on_button_gosper_clicked();
                break;
            case '6':
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

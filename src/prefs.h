#ifndef PREFS_H
#define PREFS_H

#include <glibmm/ustring.h>

/**
 * Settings/Preferences keys loaded at startup and saved on exit
 */
namespace prefs {

    const Glib::ustring settings_group("Settings");
    const Glib::ustring dark_theme_key("gtk-application-prefer-dark-theme");

    const Glib::ustring preferences_group("Preferences");
    const Glib::ustring default_zoom_key("default-zoom-value");
    const Glib::ustring show_grid_key("show-grid");
    const Glib::ustring light_scheme_key("color-scheme-light");
    const Glib::ustring dark_scheme_key("color-scheme-dark");

} /* namespace prefs */

#endif
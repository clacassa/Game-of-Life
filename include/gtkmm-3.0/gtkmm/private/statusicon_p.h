// Generated by gmmproc 2.66.0 -- DO NOT MODIFY!
#ifndef _GTKMM_STATUSICON_P_H
#define _GTKMM_STATUSICON_P_H
#ifndef GTKMM_DISABLE_DEPRECATED


#include <glibmm/private/object_p.h>

#include <glibmm/class.h>

namespace Gtk
{

class GTKMM_API StatusIcon_Class : public Glib::Class
{
public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  using CppObjectType = StatusIcon;
  using BaseObjectType = GtkStatusIcon;
  using BaseClassType = GtkStatusIconClass;
  using CppClassParent = Glib::Object_Class;
  using BaseClassParent = GObjectClass;

  friend class StatusIcon;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  const Glib::Class& init();


  static void class_init_function(void* g_class, void* class_data);

  static Glib::ObjectBase* wrap_new(GObject*);

protected:

  //Callbacks (default signal handlers):
  //These will call the *_impl member methods, which will then call the existing default signal callbacks, if any.
  //You could prevent the original default signal handlers being called by overriding the *_impl method.
  static gboolean size_changed_callback(GtkStatusIcon* self, gint p0);
  static void activate_callback(GtkStatusIcon* self);
  static void popup_menu_callback(GtkStatusIcon* self, guint p0, guint32 p1);
  static gboolean button_press_event_callback(GtkStatusIcon* self, GdkEventButton* p0);
  static gboolean button_release_event_callback(GtkStatusIcon* self, GdkEventButton* p0);
  static gboolean scroll_event_callback(GtkStatusIcon* self, GdkEventScroll* p0);
  static gboolean query_tooltip_callback(GtkStatusIcon* self, gint p0, gint p1, gboolean p2, GtkTooltip* p3);

  //Callbacks (virtual functions):
};


} // namespace Gtk

#endif // GTKMM_DISABLE_DEPRECATED


#endif /* _GTKMM_STATUSICON_P_H */


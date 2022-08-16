// Generated by gmmproc 2.66.0 -- DO NOT MODIFY!
#ifndef _ATKMM_COMPONENT_P_H
#define _ATKMM_COMPONENT_P_H


#include <atk/atkcomponent.h>

#include <glibmm/private/interface_p.h>

namespace Atk
{

class ATKMM_API Component_Class : public Glib::Interface_Class
{
public:
  using CppObjectType = Component;
  using BaseObjectType = AtkComponent;
  using BaseClassType = AtkComponentIface;
  using CppClassParent = Glib::Interface_Class;

  friend class Component;

  const Glib::Interface_Class& init();

  static void iface_init_function(void* g_iface, void* iface_data);

  static Glib::ObjectBase* wrap_new(GObject*);

protected:

  //Callbacks (default signal handlers):
  //These will call the *_impl member methods, which will then call the existing default signal callbacks, if any.
  //You could prevent the original default signal handlers being called by overriding the *_impl method.

  //Callbacks (virtual functions):
  static guint add_focus_handler_vfunc_callback(AtkComponent* self, AtkFocusHandler handler);
  static gboolean contains_vfunc_callback(AtkComponent* self, gint x, gint y, AtkCoordType coord_type);
  static AtkObject* ref_accessible_at_point_vfunc_callback(AtkComponent* self, gint x, gint y, AtkCoordType coord_type);
  static void get_extents_vfunc_callback(AtkComponent* self, gint* x, gint* y, gint* width, gint* height, AtkCoordType coord_type);
  static void get_position_vfunc_callback(AtkComponent* self, gint* x, gint* y, AtkCoordType coord_type);
  static void get_size_vfunc_callback(AtkComponent* self, gint* width, gint* height);
  static AtkLayer get_layer_vfunc_callback(AtkComponent* self);
  static gint get_mdi_zorder_vfunc_callback(AtkComponent* self);
  static gboolean grab_focus_vfunc_callback(AtkComponent* self);
  static void remove_focus_handler_vfunc_callback(AtkComponent* self, guint handler_id);
  static gboolean set_extents_vfunc_callback(AtkComponent* self, gint x, gint y, gint width, gint height, AtkCoordType coord_type);
  static gboolean set_position_vfunc_callback(AtkComponent* self, gint x, gint y, AtkCoordType coord_type);
  static gboolean set_size_vfunc_callback(AtkComponent* self, gint width, gint height);
};


} // namespace Atk


#endif /* _ATKMM_COMPONENT_P_H */


// Generated by gmmproc 2.66.0 -- DO NOT MODIFY!
#ifndef _GTKMM_LAYOUT_H
#define _GTKMM_LAYOUT_H


#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

/*
 * Copyright (C) 2002 The gtkmm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <gtkmm/container.h>
#include <gtkmm/scrollable.h>
#include <gdkmm/window.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
using GtkLayout = struct _GtkLayout;
using GtkLayoutClass = struct _GtkLayoutClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Gtk
{ class GTKMM_API Layout_Class; } // namespace Gtk
#endif //DOXYGEN_SHOULD_SKIP_THIS

namespace Gtk
{

/** Infinite scrollable area containing child widgets and/or custom drawing.
 *
 * Gtk::Layout is similar to Gtk::DrawingArea in that it's a "blank slate"
 * and doesn't do anything but paint a blank background by default. It's
 * different in that it supports scrolling natively (You can add it directly
 * to a Gtk::ScrolledWindow), and it can contain child widgets, since it's a
 * Gtk::Container.  However if you're just going to draw, a Gtk::DrawingArea
 * is a better choice since it has lower overhead.
 *
 * @ingroup Widgets
 * @ingroup Containers
 */

class GTKMM_API Layout
 : public Container,
   public Scrollable
{
  public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef Layout CppObjectType;
  typedef Layout_Class CppClassType;
  typedef GtkLayout BaseObjectType;
  typedef GtkLayoutClass BaseClassType;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  Layout(Layout&& src) noexcept;
  Layout& operator=(Layout&& src) noexcept;

  // noncopyable
  Layout(const Layout&) = delete;
  Layout& operator=(const Layout&) = delete;

  ~Layout() noexcept override;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

private:
  friend class GTKMM_API Layout_Class;
  static CppClassType layout_class_;

protected:
  explicit Layout(const Glib::ConstructParams& construct_params);
  explicit Layout(GtkLayout* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:

  /** Get the GType for this class, for use with the underlying GObject type system.
   */
  static GType get_type()      G_GNUC_CONST;

#ifndef DOXYGEN_SHOULD_SKIP_THIS


  static GType get_base_type() G_GNUC_CONST;
#endif

  /// Provides access to the underlying C GObject.
  GtkLayout*       gobj()       { return reinterpret_cast<GtkLayout*>(gobject_); }

  /// Provides access to the underlying C GObject.
  const GtkLayout* gobj() const { return reinterpret_cast<GtkLayout*>(gobject_); }


public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::


private:

  
public:
  Layout();
    explicit Layout(const Glib::RefPtr<Adjustment>& hadjustment, const Glib::RefPtr<Adjustment>& vadjustment);


  /** Retrieve the bin window of the layout used for drawing operations.
   * 
   * @newin{2,14}
   * 
   * @return A Gdk::Window.
   */
  Glib::RefPtr<Gdk::Window> get_bin_window();
  
  /** Retrieve the bin window of the layout used for drawing operations.
   * 
   * @newin{2,14}
   * 
   * @return A Gdk::Window.
   */
  Glib::RefPtr<const Gdk::Window> get_bin_window() const;

  
  /** Adds @a child_widget to @a layout, at position ( @a x, @a y).
   *  @a layout becomes the new parent container of @a child_widget.
   * 
   * @param child_widget Child widget.
   * @param x X position of child widget.
   * @param y Y position of child widget.
   */
  void put(Widget& child_widget, int x, int y);
  
  /** Moves a current child of @a layout to a new position.
   * 
   * @param child_widget A current child of @a layout.
   * @param x X position to move to.
   * @param y Y position to move to.
   */
  void move(Widget& child_widget, int x, int y);
  
  /** Sets the size of the scrollable area of the layout.
   * 
   * @param width Width of entire scrollable area.
   * @param height Height of entire scrollable area.
   */
  void set_size(guint width, guint height);
  
  /** Gets the size that has been set on the layout, and that determines
   * the total extents of the layout’s scrollbar area. See
   * set_size().
   * 
   * @param width Location to store the width set on
   *  @a layout, or <tt>nullptr</tt>.
   * @param height Location to store the height set on
   *  @a layout, or <tt>nullptr</tt>.
   */
  void get_size(guint& width, guint& height) const;

   //deprecated

  /** The width of the layout.
   *
   * Default value: 100
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< guint > property_width() ;

/** The width of the layout.
   *
   * Default value: 100
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< guint > property_width() const;

  /** The height of the layout.
   *
   * Default value: 100
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< guint > property_height() ;

/** The height of the layout.
   *
   * Default value: 100
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< guint > property_height() const;


  /** X position of child widget.
   *
   * Default value: 0
   *
   * @return A ChildPropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Gtk::ChildPropertyProxy< int > child_property_x(Gtk::Widget& child) ;

/** X position of child widget.
   *
   * Default value: 0
   *
   * @return A ChildPropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Gtk::ChildPropertyProxy_ReadOnly< int > child_property_x(const Gtk::Widget& child) const;

  /** Y position of child widget.
   *
   * Default value: 0
   *
   * @return A ChildPropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Gtk::ChildPropertyProxy< int > child_property_y(Gtk::Widget& child) ;

/** Y position of child widget.
   *
   * Default value: 0
   *
   * @return A ChildPropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Gtk::ChildPropertyProxy_ReadOnly< int > child_property_y(const Gtk::Widget& child) const;


};

} //namespace Gtk


namespace Glib
{
  /** A Glib::wrap() method for this object.
   *
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates Gtk::Layout
   */
  GTKMM_API
  Gtk::Layout* wrap(GtkLayout* object, bool take_copy = false);
} //namespace Glib


#endif /* _GTKMM_LAYOUT_H */


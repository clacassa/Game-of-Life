// Generated by gmmproc 2.66.0 -- DO NOT MODIFY!
#ifndef _GTKMM_FRAME_H
#define _GTKMM_FRAME_H


#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

/*
 * Copyright (C) 1998-2002 The gtkmm Development Team
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


#include <gtkmm/bin.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
using GtkFrame = struct _GtkFrame;
using GtkFrameClass = struct _GtkFrameClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Gtk
{ class GTKMM_API Frame_Class; } // namespace Gtk
#endif //DOXYGEN_SHOULD_SKIP_THIS

namespace Gtk
{

/** A Gtk::Bin with a decorative frame and optional label.
 *
 * The Frame widget surrounds its single child with a decorative frame and
 * an optional label.  If present, the label is drawn in a gap in the top
 * side of the frame. The position of the label can be controlled with
 * set_label_align().
 *
 * A Frame widget looks like this:
 * @image html frame1.png
 *
 * @ingroup Widgets
 * @ingroup Containers
 */

class GTKMM_API Frame : public Bin
{
  public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef Frame CppObjectType;
  typedef Frame_Class CppClassType;
  typedef GtkFrame BaseObjectType;
  typedef GtkFrameClass BaseClassType;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

  Frame(Frame&& src) noexcept;
  Frame& operator=(Frame&& src) noexcept;

  // noncopyable
  Frame(const Frame&) = delete;
  Frame& operator=(const Frame&) = delete;

  ~Frame() noexcept override;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

private:
  friend class GTKMM_API Frame_Class;
  static CppClassType frame_class_;

protected:
  explicit Frame(const Glib::ConstructParams& construct_params);
  explicit Frame(GtkFrame* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:

  /** Get the GType for this class, for use with the underlying GObject type system.
   */
  static GType get_type()      G_GNUC_CONST;

#ifndef DOXYGEN_SHOULD_SKIP_THIS


  static GType get_base_type() G_GNUC_CONST;
#endif

  /// Provides access to the underlying C GObject.
  GtkFrame*       gobj()       { return reinterpret_cast<GtkFrame*>(gobject_); }

  /// Provides access to the underlying C GObject.
  const GtkFrame* gobj() const { return reinterpret_cast<GtkFrame*>(gobject_); }


public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::


private:

public:

  Frame();
    explicit Frame(const Glib::ustring& label);


  //TODO: Add a bool use_markup arg to set_label() as a convenience - it would have to use set_label_widget().

  
  /** Set the label to appear in the top edge of the frame.
   * Label alignment defaults to the upper left corner of the frame.
   */
  void set_label(const Glib::ustring& label);
  void unset_label();
  
  /** If the frame’s label widget is a Gtk::Label, returns the
   * text in the label widget. (The frame will have a Gtk::Label
   * for the label widget if a non-<tt>nullptr</tt> argument was passed
   * to new().)
   * 
   * @return The text in the label, or <tt>nullptr</tt> if there
   * was no label widget or the lable widget was not
   * a Gtk::Label. This string is owned by GTK+ and
   * must not be modified or freed.
   */
  Glib::ustring get_label() const;

  
  /** Sets the Gtk::Frame::property_label_widget() for the frame. This is the widget that
   * will appear embedded in the top edge of the frame as a title.
   * 
   * @param label_widget The new label widget.
   */
  void set_label_widget(Widget& label_widget);
  
  /** Retrieves the label widget for the frame. See
   * set_label_widget().
   * 
   * @return The label widget, or <tt>nullptr</tt> if
   * there is none.
   */
  Widget* get_label_widget();
  
  /** Retrieves the label widget for the frame. See
   * set_label_widget().
   * 
   * @return The label widget, or <tt>nullptr</tt> if
   * there is none.
   */
  const Widget* get_label_widget() const;

  
  /** Sets the alignment of the frame widget’s label. The
   * default values for a newly created frame are 0.0 and 0.5.
   * 
   * @param xalign The position of the label along the top edge
   * of the widget. A value of 0.0 represents left alignment;
   * 1.0 represents right alignment.
   * @param yalign The y alignment of the label. A value of 0.0 aligns under 
   * the frame; 1.0 aligns above the frame. If the values are exactly
   * 0.0 or 1.0 the gap in the frame won’t be painted because the label
   * will be completely above or below the frame.
   */
  void set_label_align(float xalign =  0.0, float yalign =  0.5);
  
  /** Sets the alignment of the frame widget’s label. The
   * default values for a newly created frame are Gtk::ALIGN_START and Gtk::ALIGN_CENTER.
   *
   * @param xalign The position of the label along the top edge
   * of the widget. A value of Gtk::ALIGN_START represents left alignment;
   * Gtk::ALIGN_END represents right alignment.
   * @param yalign The y alignment of the label. A value of Gtk::ALIGN_START aligns under
   * the frame; Gtk::ALIGN_END aligns above the frame. If the values are
   * Gtk::ALIGN_START or Gtk::ALIGN_END the gap in the frame won’t be painted because the label
   * will be completely above or below the frame.
   */
  void set_label_align(Align xalign, Align yalign =  Gtk::ALIGN_CENTER);

  
  /** Retrieves the X and Y alignment of the frame’s label. See
   * set_label_align().
   * 
   * @param xalign Location to store X alignment of
   * frame’s label, or <tt>nullptr</tt>.
   * @param yalign Location to store X alignment of
   * frame’s label, or <tt>nullptr</tt>.
   */
  void get_label_align(float& xalign, float& yalign) const;

  
  /** Sets shadow type of the frame.
   */
  void set_shadow_type(ShadowType type);
  
  /** Retrieves the shadow type of the frame. See
   * set_shadow_type().
   * 
   * @return The current shadow type of the frame.
   */
  ShadowType get_shadow_type() const;

  /** Text of the frame's label.
   *
   * Default value: ""
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::ustring > property_label() ;

/** Text of the frame's label.
   *
   * Default value: ""
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::ustring > property_label() const;

  /** The horizontal alignment of the label.
   *
   * Default value: 0
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< double > property_label_xalign() ;

/** The horizontal alignment of the label.
   *
   * Default value: 0
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< double > property_label_xalign() const;

  /** The vertical alignment of the label.
   *
   * Default value: 0.5
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< double > property_label_yalign() ;

/** The vertical alignment of the label.
   *
   * Default value: 0.5
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< double > property_label_yalign() const;

  /** Appearance of the frame border.
   *
   * Default value: Gtk::SHADOW_ETCHED_IN
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< ShadowType > property_shadow_type() ;

/** Appearance of the frame border.
   *
   * Default value: Gtk::SHADOW_ETCHED_IN
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< ShadowType > property_shadow_type() const;

  /** A widget to display in place of the usual frame label.
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Widget* > property_label_widget() ;

/** A widget to display in place of the usual frame label.
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Widget* > property_label_widget() const;


protected:
 

    virtual void compute_child_allocation_vfunc(Allocation& allocation);


};

} // namespace Gtk


namespace Glib
{
  /** A Glib::wrap() method for this object.
   *
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates Gtk::Frame
   */
  GTKMM_API
  Gtk::Frame* wrap(GtkFrame* object, bool take_copy = false);
} //namespace Glib


#endif /* _GTKMM_FRAME_H */


// Generated by gmmproc 2.66.0 -- DO NOT MODIFY!
#ifndef _ATKMM_STREAMABLECONTENT_H
#define _ATKMM_STREAMABLECONTENT_H

#include <atkmmconfig.h>


#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

/* $Id: streamablecontent.hg,v 1.3 2004/01/19 19:48:36 murrayc Exp $ */

/* Copyright (C) 2003 The gtkmm Development Team
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
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <glibmm/interface.h>
#include <glibmm/iochannel.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
extern "C"
{
  typedef struct _AtkStreamableContentIface AtkStreamableContentIface;
  typedef struct _AtkStreamableContent      AtkStreamableContent;
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


#ifndef DOXYGEN_SHOULD_SKIP_THIS
using AtkStreamableContent = struct _AtkStreamableContent;
using AtkStreamableContentClass = struct _AtkStreamableContentClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Atk
{ class ATKMM_API StreamableContent_Class; } // namespace Atk
#endif // DOXYGEN_SHOULD_SKIP_THIS

namespace Atk
{

class ATKMM_API Object;

/** The ATK interface which provides access to streamable content.
 */

class ATKMM_API StreamableContent : public Glib::Interface
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  using CppObjectType = StreamableContent;
  using CppClassType = StreamableContent_Class;
  using BaseObjectType = AtkStreamableContent;
  using BaseClassType = AtkStreamableContentIface;

  // noncopyable
  StreamableContent(const StreamableContent&) = delete;
  StreamableContent& operator=(const StreamableContent&) = delete;

private:
  friend class StreamableContent_Class;
  static CppClassType streamablecontent_class_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
protected:
  /**
   * You should derive from this class to use it.
   */
  StreamableContent();

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  /** Called by constructors of derived classes. Provide the result of
   * the Class init() function to ensure that it is properly
   * initialized.
   *
   * @param interface_class The Class object for the derived type.
   */
  explicit StreamableContent(const Glib::Interface_Class& interface_class);

public:
  // This is public so that C++ wrapper instances can be
  // created for C instances of unwrapped types.
  // For instance, if an unexpected C type implements the C interface.
  explicit StreamableContent(AtkStreamableContent* castitem);

protected:
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:

  StreamableContent(StreamableContent&& src) noexcept;
  StreamableContent& operator=(StreamableContent&& src) noexcept;

  ~StreamableContent() noexcept override;

  static void add_interface(GType gtype_implementer);

  /** Get the GType for this class, for use with the underlying GObject type system.
   */
  static GType get_type()      G_GNUC_CONST;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  AtkStreamableContent*       gobj()       { return reinterpret_cast<AtkStreamableContent*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const AtkStreamableContent* gobj() const { return reinterpret_cast<AtkStreamableContent*>(gobject_); }

private:

public:

  
  /** Gets the number of mime types supported by this object.
   * 
   * @return A <tt>int</tt> which is the number of mime types supported by the object.
   */
  int get_n_mime_types() const;
  
  /** Gets the character string of the specified mime type. The first mime
   * type is at position 0, the second at position 1, and so on.
   * 
   * @param i A <tt>int</tt> representing the position of the mime type starting from 0.
   * @return A gchar* representing the specified mime type; the caller
   * should not free the character string.
   */
  Glib::ustring get_mime_type(int i) const;
  
  /** Gets the content in the specified mime type.
   * 
   * @param mime_type A gchar* representing the mime type.
   * @return A IOChannel which contains the content in the
   * specified mime type.
   */
  Glib::RefPtr<Glib::IOChannel> get_stream(const Glib::ustring& mime_type);

protected:
    virtual int get_n_mime_types_vfunc() const;

    virtual const gchar* get_mime_type_vfunc(int i) const;


    virtual GIOChannel* get_stream_vfunc(const Glib::ustring& mime_type);
                                                   

public:

public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::


};

} // namespace Atk


namespace Glib
{
  /** A Glib::wrap() method for this object.
   *
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates Atk::StreamableContent
   */
  ATKMM_API
  Glib::RefPtr<Atk::StreamableContent> wrap(AtkStreamableContent* object, bool take_copy = false);

} // namespace Glib


#endif /* _ATKMM_STREAMABLECONTENT_H */


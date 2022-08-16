// Generated by gmmproc 2.66.0 -- DO NOT MODIFY!
#ifndef _GTKMM_CSSSECTION_H
#define _GTKMM_CSSSECTION_H

#include <gtkmmconfig.h>


#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

/* Copyright (C) 2014 The gtkmm Development Team
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
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */


#include <glibmm/refptr.h>
#include <giomm/file.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef struct _GtkCssSection GtkCssSection;
#endif

namespace Gtk
{
/** @addtogroup gtkmmEnums gtkmm Enums and Flags */

/** 
 *  @var CssSectionType CSS_SECTION_DOCUMENT
 * The section describes a complete document.
 * This section time is the only one where Gtk::CssSection::get_parent()
 * might return <tt>nullptr</tt>.
 * 
 *  @var CssSectionType CSS_SECTION_IMPORT
 * The section defines an import rule.
 * 
 *  @var CssSectionType CSS_SECTION_COLOR_DEFINITION
 * The section defines a color. This
 * is a GTK extension to CSS.
 * 
 *  @var CssSectionType CSS_SECTION_BINDING_SET
 * The section defines a binding set. This
 * is a GTK extension to CSS.
 * 
 *  @var CssSectionType CSS_SECTION_RULESET
 * The section defines a CSS ruleset.
 * 
 *  @var CssSectionType CSS_SECTION_SELECTOR
 * The section defines a CSS selector.
 * 
 *  @var CssSectionType CSS_SECTION_DECLARATION
 * The section defines the declaration of
 * a CSS variable.
 * 
 *  @var CssSectionType CSS_SECTION_VALUE
 * The section defines the value of a CSS declaration.
 * 
 *  @var CssSectionType CSS_SECTION_KEYFRAMES
 * The section defines keyframes. See [CSS
 * Animations](http://dev.w3.org/csswg/css3-animations/#keyframes) for details. @newin{3,6}
 * 
 *  @enum CssSectionType
 * 
 * The different types of sections indicate parts of a CSS document as
 * parsed by GTK’s CSS parser. They are oriented towards the
 * [CSS Grammar](http://www.w3.org/TR/CSS21/grammar.html),
 * but may contain extensions.
 * 
 * More types might be added in the future as the parser incorporates
 * more features.
 * 
 * @newin{3,2}
 *
 * @ingroup gtkmmEnums
 */
enum CssSectionType
{
  CSS_SECTION_DOCUMENT,
  CSS_SECTION_IMPORT,
  CSS_SECTION_COLOR_DEFINITION,
  CSS_SECTION_BINDING_SET,
  CSS_SECTION_RULESET,
  CSS_SECTION_SELECTOR,
  CSS_SECTION_DECLARATION,
  CSS_SECTION_VALUE,
  CSS_SECTION_KEYFRAMES
};

} // namespace Gtk

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class GTKMM_API Value<Gtk::CssSectionType> : public Glib::Value_Enum<Gtk::CssSectionType>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace Gtk
{


/** Defines a part of a CSS document.
 * Because sections are nested into one another, you can use get_parent()
 * to get the containing region.
 *
 * There is no public method that creates a CssSection, but it is input data to
 * a Gtk::CssProvider::signal_parsing_error() handler.
 *
 * @newin{3,16}
 */
class GTKMM_API CssSection final
{
  public:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  using CppObjectType = CssSection;
  using BaseObjectType = GtkCssSection;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


  /** Increment the reference count for this object.
   * You should never need to do this manually - use the object via a RefPtr instead.
   */
  void reference()   const;

  /** Decrement the reference count for this object.
   * You should never need to do this manually - use the object via a RefPtr instead.
   */
  void unreference() const;

  ///Provides access to the underlying C instance.
  GtkCssSection*       gobj();

  ///Provides access to the underlying C instance.
  const GtkCssSection* gobj() const;

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GtkCssSection* gobj_copy() const;

  CssSection() = delete;

  // noncopyable
  CssSection(const CssSection&) = delete;
  CssSection& operator=(const CssSection&) = delete;

protected:
  // Do not derive this.  Gtk::CssSection can neither be constructed nor deleted.

  void operator delete(void*, std::size_t);

private:

  
public:
  
  /** Gets the type of information that @a section describes.
   * 
   * @newin{3,2}
   * 
   * @return The type of @a section.
   */
  CssSectionType get_section_type() const;
  
  /** Gets the parent section for the given @a section. The parent section is
   * the section that contains this @a section. A special case are sections of
   * type Gtk::CSS_SECTION_DOCUMENT. Their parent will either be <tt>nullptr</tt>
   * if they are the original CSS document that was loaded by
   * Gtk::CssProvider::load_from_file() or a section of type
   * Gtk::CSS_SECTION_IMPORT if it was loaded with an import rule from
   * a different file.
   * 
   * @newin{3,2}
   * 
   * @return The parent section or <tt>nullptr</tt> if none.
   */
  Glib::RefPtr<CssSection> get_parent();
  
  /** Gets the parent section for the given @a section. The parent section is
   * the section that contains this @a section. A special case are sections of
   * type Gtk::CSS_SECTION_DOCUMENT. Their parent will either be <tt>nullptr</tt>
   * if they are the original CSS document that was loaded by
   * Gtk::CssProvider::load_from_file() or a section of type
   * Gtk::CSS_SECTION_IMPORT if it was loaded with an import rule from
   * a different file.
   * 
   * @newin{3,2}
   * 
   * @return The parent section or <tt>nullptr</tt> if none.
   */
  Glib::RefPtr<const CssSection> get_parent() const;
  
  /** Gets the file that @a section was parsed from. If no such file exists,
   * for example because the CSS was loaded via
   *  @a Gtk::CssProvider::load_from_data(), then <tt>nullptr</tt> is returned.
   * 
   * @newin{3,2}
   * 
   * @return The Gio::File that @a section was parsed from
   * or <tt>nullptr</tt> if @a section was parsed from other data.
   */
  Glib::RefPtr<Gio::File> get_file();
  
  /** Gets the file that @a section was parsed from. If no such file exists,
   * for example because the CSS was loaded via
   *  @a Gtk::CssProvider::load_from_data(), then <tt>nullptr</tt> is returned.
   * 
   * @newin{3,2}
   * 
   * @return The Gio::File that @a section was parsed from
   * or <tt>nullptr</tt> if @a section was parsed from other data.
   */
  Glib::RefPtr<const Gio::File> get_file() const;
  
  /** Returns the line in the CSS document where this section starts.
   * The line number is 0-indexed, so the first line of the document
   * will return 0.
   * 
   * @newin{3,2}
   * 
   * @return The line number.
   */
  unsigned int get_start_line() const;
  
  /** Returns the offset in bytes from the start of the current line
   * returned via get_start_line().
   * 
   * @newin{3,2}
   * 
   * @return The offset in bytes from the start of the line.
   */
  unsigned int get_start_position() const;
  
  /** Returns the line in the CSS document where this section end.
   * The line number is 0-indexed, so the first line of the document
   * will return 0.
   * This value may change in future invocations of this function if
   *  @a section is not yet parsed completely. This will for example 
   * happen in the GtkCssProvider::parsing-error signal.
   * The end position and line may be identical to the start
   * position and line for sections which failed to parse anything
   * successfully.
   * 
   * @newin{3,2}
   * 
   * @return The line number.
   */
  unsigned int get_end_line() const;
  
  /** Returns the offset in bytes from the start of the current line
   * returned via get_end_line().
   * This value may change in future invocations of this function if
   *  @a section is not yet parsed completely. This will for example
   * happen in the GtkCssProvider::parsing-error signal.
   * The end position and line may be identical to the start
   * position and line for sections which failed to parse anything
   * successfully.
   * 
   * @newin{3,2}
   * 
   * @return The offset in bytes from the start of the line.
   */
  unsigned int get_end_position() const;


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
 * @relates Gtk::CssSection
 */
GTKMM_API
Glib::RefPtr<Gtk::CssSection> wrap(GtkCssSection* object, bool take_copy = false);

} // namespace Glib


#endif /* _GTKMM_CSSSECTION_H */


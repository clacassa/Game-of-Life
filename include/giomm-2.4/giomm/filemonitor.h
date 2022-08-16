// Generated by gmmproc 2.66.2 -- DO NOT MODIFY!
#ifndef _GIOMM_FILEMONITOR_H
#define _GIOMM_FILEMONITOR_H

#include <giommconfig.h>


#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

/* Copyright (C) 2007 The gtkmm Development Team
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
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


//#include <giomm/file.h>
#include <glibmm/object.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
using GFileMonitor = struct _GFileMonitor;
using GFileMonitorClass = struct _GFileMonitorClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Gio
{ class GIOMM_API FileMonitor_Class; } // namespace Gio
#endif //DOXYGEN_SHOULD_SKIP_THIS

namespace Gio
{

/** @addtogroup giommEnums giomm Enums and Flags */

/** 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_CHANGED
 * A file changed.
 * 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_CHANGES_DONE_HINT
 * A hint that this was probably the last change in a set of changes.
 * 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_DELETED
 * A file was deleted.
 * 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_CREATED
 * A file was created.
 * 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED
 * A file attribute was changed.
 * 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_PRE_UNMOUNT
 * The file location will soon be unmounted.
 * 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_UNMOUNTED
 * The file location was unmounted.
 * 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_MOVED
 * The file was moved -- only sent if the
 * (deprecated) FILE_MONITOR_SEND_MOVED flag is set.
 * 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_RENAMED
 * The file was renamed within the
 * current directory -- only sent if the FILE_MONITOR_WATCH_MOVES
 * flag is set.  @newin{2,46}
 * 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_MOVED_IN
 * The file was moved into the
 * monitored directory from another location -- only sent if the
 * FILE_MONITOR_WATCH_MOVES flag is set.  @newin{2,46}
 * 
 *  @var FileMonitorEvent FILE_MONITOR_EVENT_MOVED_OUT
 * The file was moved out of the
 * monitored directory to another location -- only sent if the
 * FILE_MONITOR_WATCH_MOVES flag is set.  @newin{2,46}
 * 
 *  @enum FileMonitorEvent
 * 
 * Specifies what type of event a monitor event is.
 *
 * @ingroup giommEnums
 */
enum FileMonitorEvent
{
  FILE_MONITOR_EVENT_CHANGED,
  FILE_MONITOR_EVENT_CHANGES_DONE_HINT,
  FILE_MONITOR_EVENT_DELETED,
  FILE_MONITOR_EVENT_CREATED,
  FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED,
  FILE_MONITOR_EVENT_PRE_UNMOUNT,
  FILE_MONITOR_EVENT_UNMOUNTED,
  FILE_MONITOR_EVENT_MOVED,
  FILE_MONITOR_EVENT_RENAMED,
  FILE_MONITOR_EVENT_MOVED_IN,
  FILE_MONITOR_EVENT_MOVED_OUT
};


class GIOMM_API File;

/** Monitors a file or directory for changes.
 * To obtain a FileMonitor for a file or directory, use File::monitor_file() or
 * File::monitor_directory().
 *
 * To get informed about changes to the file or directory you are monitoring,
 * connect to signal_changed().
 *
 * @newin{2,16}
 */

class GIOMM_API FileMonitor : public Glib::Object
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  using CppObjectType = FileMonitor;
  using CppClassType = FileMonitor_Class;
  using BaseObjectType = GFileMonitor;
  using BaseClassType = GFileMonitorClass;

  // noncopyable
  FileMonitor(const FileMonitor&) = delete;
  FileMonitor& operator=(const FileMonitor&) = delete;

private:  friend class FileMonitor_Class;
  static CppClassType filemonitor_class_;

protected:
  explicit FileMonitor(const Glib::ConstructParams& construct_params);
  explicit FileMonitor(GFileMonitor* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:

  FileMonitor(FileMonitor&& src) noexcept;
  FileMonitor& operator=(FileMonitor&& src) noexcept;

  ~FileMonitor() noexcept override;

  /** Get the GType for this class, for use with the underlying GObject type system.
   */
  static GType get_type()      G_GNUC_CONST;

#ifndef DOXYGEN_SHOULD_SKIP_THIS


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GFileMonitor*       gobj()       { return reinterpret_cast<GFileMonitor*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GFileMonitor* gobj() const { return reinterpret_cast<GFileMonitor*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GFileMonitor* gobj_copy();

private:

protected:

public:

  
  /** Cancels a file monitor.
   * 
   * @return Always <tt>true</tt>.
   */
  bool cancel();
  
  /** Returns whether the monitor is canceled.
   * 
   * @return <tt>true</tt> if monitor is canceled. <tt>false</tt> otherwise.
   */
  bool is_cancelled() const;
  
  /** Sets the rate limit to which the @a monitor will report
   * consecutive change events to the same file.
   * 
   * @param limit_msecs A non-negative integer with the limit in milliseconds
   * to poll for changes.
   */
  void set_rate_limit(int limit_msecs);

  //g_file_monitor_emit_event is for implementations.
  

  /**
   * @par Slot Prototype:
   * <tt>void on_my_%changed(const Glib::RefPtr<File>& file, const Glib::RefPtr<File>& other_file, FileMonitorEvent event_type)</tt>
   *
   * Flags: Run Last
   *
   * Emitted when @a file has been changed.
   * 
   * If using FILE_MONITOR_WATCH_MOVES on a directory monitor, and
   * the information is available (and if supported by the backend),
   *  @a event_type may be FILE_MONITOR_EVENT_RENAMED,
   * FILE_MONITOR_EVENT_MOVED_IN or FILE_MONITOR_EVENT_MOVED_OUT.
   * 
   * In all cases @a file will be a child of the monitored directory.  For
   * renames, @a file will be the old name and @a other_file is the new
   * name.  For "moved in" events, @a file is the name of the file that
   * appeared and @a other_file is the old name that it was moved from (in
   * another directory).  For "moved out" events, @a file is the name of
   * the file that used to be in this directory and @a other_file is the
   * name of the file at its new location.
   * 
   * It makes sense to treat FILE_MONITOR_EVENT_MOVED_IN as
   * equivalent to FILE_MONITOR_EVENT_CREATED and
   * FILE_MONITOR_EVENT_MOVED_OUT as equivalent to
   * FILE_MONITOR_EVENT_DELETED, with extra information.
   * FILE_MONITOR_EVENT_RENAMED is equivalent to a delete/create
   * pair.  This is exactly how the events will be reported in the case
   * that the FILE_MONITOR_WATCH_MOVES flag is not in use.
   * 
   * If using the deprecated flag FILE_MONITOR_SEND_MOVED flag and @a event_type is
   * FILE_MONITOR_EVENT_MOVED, @a file will be set to a File containing the
   * old path, and @a other_file will be set to a File containing the new path.
   * 
   * In all the other cases, @a other_file will be set to #<tt>nullptr</tt>.
   * 
   * @param file A File.
   * @param other_file A File or #<tt>nullptr</tt>.
   * @param event_type A FileMonitorEvent.
   */

  Glib::SignalProxy< void,const Glib::RefPtr<File>&,const Glib::RefPtr<File>&,FileMonitorEvent > signal_changed();


  //_WRAP_VFUNC(bool cancel(), cancel);

  /** The limit of the monitor to watch for changes, in milliseconds.
   *
   * Default value: 800
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< int > property_rate_limit() ;

/** The limit of the monitor to watch for changes, in milliseconds.
   *
   * Default value: 800
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< int > property_rate_limit() const;

  /** Whether the monitor has been cancelled.
   *
   * Default value: <tt>false</tt>
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< bool > property_cancelled() const;


public:

public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::
  /// This is a default handler for the signal signal_changed().
  virtual void on_changed(const Glib::RefPtr<File>& file, const Glib::RefPtr<File>& other_file, FileMonitorEvent event_type);


};

} // namespace Gio


namespace Glib
{
  /** A Glib::wrap() method for this object.
   *
   * @param object The C instance.
   * @param take_copy False if the result should take ownership of the C instance. True if it should take a new copy or ref.
   * @result A C++ instance that wraps this C instance.
   *
   * @relates Gio::FileMonitor
   */
  GIOMM_API
  Glib::RefPtr<Gio::FileMonitor> wrap(GFileMonitor* object, bool take_copy = false);
}


#endif /* _GIOMM_FILEMONITOR_H */


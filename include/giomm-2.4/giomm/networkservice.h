// Generated by gmmproc 2.66.2 -- DO NOT MODIFY!
#ifndef _GIOMM_NETWORKSERVICE_H
#define _GIOMM_NETWORKSERVICE_H


#include <glibmm/ustring.h>
#include <sigc++/sigc++.h>

/* Copyright (C) 2007 The giomm Development Team
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

#include <glibmm/object.h>
#include <giomm/socketconnectable.h>


#ifndef DOXYGEN_SHOULD_SKIP_THIS
using GNetworkService = struct _GNetworkService;
using GNetworkServiceClass = struct _GNetworkServiceClass;
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Gio
{ class GIOMM_API NetworkService_Class; } // namespace Gio
#endif //DOXYGEN_SHOULD_SKIP_THIS

namespace Gio
{

/** A GSocketConnectable for resolving SRV records
 *
 * Like NetworkAddress does with hostnames, NetworkService provides an easy
 * way to resolve a SRV record, and then attempt to connect to one of the hosts
 * that implements that service, handling service priority/weighting, multiple
 * IP addresses, and multiple address families.
 *
 * See SrvTarget for more information about SRV records, and see
 * SocketConnectable for and example of using the connectable interface.
 *
 * @newin{2,24}
 * @ingroup NetworkIO
 */

class GIOMM_API NetworkService : public Glib::Object,
    public SocketConnectable
{
  
#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:
  using CppObjectType = NetworkService;
  using CppClassType = NetworkService_Class;
  using BaseObjectType = GNetworkService;
  using BaseClassType = GNetworkServiceClass;

  // noncopyable
  NetworkService(const NetworkService&) = delete;
  NetworkService& operator=(const NetworkService&) = delete;

private:  friend class NetworkService_Class;
  static CppClassType networkservice_class_;

protected:
  explicit NetworkService(const Glib::ConstructParams& construct_params);
  explicit NetworkService(GNetworkService* castitem);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

public:

  NetworkService(NetworkService&& src) noexcept;
  NetworkService& operator=(NetworkService&& src) noexcept;

  ~NetworkService() noexcept override;

  /** Get the GType for this class, for use with the underlying GObject type system.
   */
  static GType get_type()      G_GNUC_CONST;

#ifndef DOXYGEN_SHOULD_SKIP_THIS


  static GType get_base_type() G_GNUC_CONST;
#endif

  ///Provides access to the underlying C GObject.
  GNetworkService*       gobj()       { return reinterpret_cast<GNetworkService*>(gobject_); }

  ///Provides access to the underlying C GObject.
  const GNetworkService* gobj() const { return reinterpret_cast<GNetworkService*>(gobject_); }

  ///Provides access to the underlying C instance. The caller is responsible for unrefing it. Use when directly setting fields in structs.
  GNetworkService* gobj_copy();

private:

  
    explicit NetworkService(const Glib::ustring& service, const Glib::ustring& protocol, const Glib::ustring& domain);


public:
  
  static Glib::RefPtr<NetworkService> create(const Glib::ustring& service, const Glib::ustring& protocol, const Glib::ustring& domain);


  /** Gets @a srv's service name (eg, "ldap").
   * 
   * @newin{2,22}
   * 
   * @return  @a srv's service name.
   */
  Glib::ustring get_service() const;
  
  /** Gets @a srv's protocol name (eg, "tcp").
   * 
   * @newin{2,22}
   * 
   * @return  @a srv's protocol name.
   */
  Glib::ustring get_protocol() const;
  
  /** Gets the domain that @a srv serves. This might be either UTF-8 or
   * ASCII-encoded, depending on what @a srv was created with.
   * 
   * @newin{2,22}
   * 
   * @return  @a srv's domain name.
   */
  Glib::ustring get_domain() const;

  
  /** Gets the URI scheme used to resolve proxies. By default, the service name
   * is used as scheme.
   * 
   * @newin{2,26}
   * 
   * @return  @a srv's scheme name.
   */
  Glib::ustring get_scheme() const;
  
  /** Set's the URI scheme used to resolve proxies. By default, the service name
   * is used as scheme.
   * 
   * @newin{2,26}
   * 
   * @param scheme A URI scheme.
   */
  void set_scheme(const Glib::ustring& scheme);

  /** Network domain, eg, 'example.com'.
   *
   * Default value: ""
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::ustring > property_domain() const;


  /** Network protocol, eg 'tcp'.
   *
   * Default value: ""
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::ustring > property_protocol() const;


  /** Network scheme (default is to use service).
   *
   * Default value: ""
   *
   * @return A PropertyProxy that allows you to get or set the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy< Glib::ustring > property_scheme() ;

/** Network scheme (default is to use service).
   *
   * Default value: ""
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::ustring > property_scheme() const;

  /** Service name, eg 'ldap'.
   *
   * Default value: ""
   *
   * @return A PropertyProxy_ReadOnly that allows you to get the value of the property,
   * or receive notification when the value of the property changes.
   */
  Glib::PropertyProxy_ReadOnly< Glib::ustring > property_service() const;


public:

public:
  //C++ methods used to invoke GTK+ virtual functions:

protected:
  //GTK+ Virtual Functions (override these to change behaviour):

  //Default Signal Handlers::


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
   * @relates Gio::NetworkService
   */
  GIOMM_API
  Glib::RefPtr<Gio::NetworkService> wrap(GNetworkService* object, bool take_copy = false);
}


#endif /* _GIOMM_NETWORKSERVICE_H */


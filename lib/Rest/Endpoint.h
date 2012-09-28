////////////////////////////////////////////////////////////////////////////////
/// @brief connection endpoints
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2011 triagens GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Jan Steemann
/// @author Copyright 2012, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef TRIAGENS_FYN_REST_ENDPOINT_H
#define TRIAGENS_FYN_REST_ENDPOINT_H 1

#include <Basics/Common.h>
#include <Basics/StringUtils.h>



#ifdef TRI_HAVE_LINUX_SOCKETS
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/file.h>
#endif 


#ifdef TRI_HAVE_WINSOCK2_H
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif


// -----------------------------------------------------------------------------
// --SECTION--                                                          Endpoint
// -----------------------------------------------------------------------------

namespace triagens {
  namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint specification
////////////////////////////////////////////////////////////////////////////////

    class Endpoint {

// -----------------------------------------------------------------------------
// --SECTION--                                                          typedefs
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
      
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint types
////////////////////////////////////////////////////////////////////////////////

        enum EndpointType {
          ENDPOINT_SERVER,
          ENDPOINT_CLIENT 
        };

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint types
////////////////////////////////////////////////////////////////////////////////

        enum DomainType {
          DOMAIN_UNKNOWN = 0,
          DOMAIN_UNIX,
          DOMAIN_IPV4,
          DOMAIN_IPV6
        };

////////////////////////////////////////////////////////////////////////////////
/// @brief protocols used for endpoints
////////////////////////////////////////////////////////////////////////////////

        enum ProtocolType {
          PROTOCOL_UNKNOWN,
          PROTOCOL_HTTP
        };

////////////////////////////////////////////////////////////////////////////////
/// @brief encryption used when talking to endpoint
////////////////////////////////////////////////////////////////////////////////
        
        enum EncryptionType {
          ENCRYPTION_NONE = 0, 
          ENCRYPTION_SSL
        };

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint
////////////////////////////////////////////////////////////////////////////////

        Endpoint (const EndpointType, 
                  const DomainType, 
                  const ProtocolType, 
                  const EncryptionType,
                  const std::string&,
                  int);

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an endpoint
////////////////////////////////////////////////////////////////////////////////
      
        virtual ~Endpoint ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a server endpoint from a string value
////////////////////////////////////////////////////////////////////////////////

        static Endpoint* serverFactory (const std::string&, int = 10);

////////////////////////////////////////////////////////////////////////////////
/// @brief creates a client endpoint from a string value
////////////////////////////////////////////////////////////////////////////////

        static Endpoint* clientFactory (const std::string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint from a string value
////////////////////////////////////////////////////////////////////////////////

        static Endpoint* factory (const EndpointType type, 
                                  const std::string&,
                                  int);

////////////////////////////////////////////////////////////////////////////////
/// @brief compare two endpoints
////////////////////////////////////////////////////////////////////////////////

        bool operator== (Endpoint const &) const;

////////////////////////////////////////////////////////////////////////////////
/// @brief return the default endpoint
////////////////////////////////////////////////////////////////////////////////

        static const std::string getDefaultEndpoint ();

////////////////////////////////////////////////////////////////////////////////
/// @brief connect the endpoint
////////////////////////////////////////////////////////////////////////////////
        
        virtual int connect (double, double) = 0; 

////////////////////////////////////////////////////////////////////////////////
/// @brief disconnect the endpoint
////////////////////////////////////////////////////////////////////////////////
        
        virtual void disconnect () = 0; 

////////////////////////////////////////////////////////////////////////////////
/// @brief init an incoming connection
////////////////////////////////////////////////////////////////////////////////

        virtual bool initIncoming (socket_t) = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief set socket timeout
////////////////////////////////////////////////////////////////////////////////
        
        virtual void setTimeout (socket_t, double); 

////////////////////////////////////////////////////////////////////////////////
/// @brief initialise socket flags
////////////////////////////////////////////////////////////////////////////////
        
        virtual bool setSocketFlags (socket_t); 

////////////////////////////////////////////////////////////////////////////////
/// @brief return whether the endpoint is connected
////////////////////////////////////////////////////////////////////////////////

        bool isConnected () const {
          return _connected;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the type of an endpoint
////////////////////////////////////////////////////////////////////////////////

        EndpointType getType () const {
          return _type;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the domain type of an endpoint
////////////////////////////////////////////////////////////////////////////////

        DomainType getDomainType () const {
          return _domainType;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the protocol of an endpoint
////////////////////////////////////////////////////////////////////////////////

        ProtocolType getProtocol () const {
          return _protocol;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the encryption used
////////////////////////////////////////////////////////////////////////////////

        EncryptionType getEncryption () const {
          return _encryption;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get the original endpoint specification
////////////////////////////////////////////////////////////////////////////////

        std::string getSpecification () const {
          return _specification;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get endpoint domain
////////////////////////////////////////////////////////////////////////////////

        virtual int getDomain () const = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief get port number
////////////////////////////////////////////////////////////////////////////////

        virtual int getPort () const = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief get host name
////////////////////////////////////////////////////////////////////////////////

        virtual std::string getHost () const = 0;

////////////////////////////////////////////////////////////////////////////////
/// @brief get address
////////////////////////////////////////////////////////////////////////////////

        virtual std::string getHostString () const = 0;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                               protected variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief whether or not the endpoint is connected
////////////////////////////////////////////////////////////////////////////////

        bool _connected;

////////////////////////////////////////////////////////////////////////////////
/// @brief the actual socket
////////////////////////////////////////////////////////////////////////////////

        socket_t _socket;

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint type
////////////////////////////////////////////////////////////////////////////////
      
        EndpointType _type;

////////////////////////////////////////////////////////////////////////////////
/// @brief endpoint domain type
////////////////////////////////////////////////////////////////////////////////
      
        DomainType _domainType;

////////////////////////////////////////////////////////////////////////////////
/// @brief protocol used
////////////////////////////////////////////////////////////////////////////////

        ProtocolType _protocol;

////////////////////////////////////////////////////////////////////////////////
/// @brief encryption used
////////////////////////////////////////////////////////////////////////////////

        EncryptionType _encryption;

////////////////////////////////////////////////////////////////////////////////
/// @brief original endpoint specification
////////////////////////////////////////////////////////////////////////////////

        std::string _specification;

////////////////////////////////////////////////////////////////////////////////
/// @brief listen backlog size
////////////////////////////////////////////////////////////////////////////////

        int _listenBacklog;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

#ifdef TRI_HAVE_LINUX_SOCKETS

// -----------------------------------------------------------------------------
// --SECTION--                                                      EndpointUnix
// -----------------------------------------------------------------------------

    class EndpointUnix : public Endpoint {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint
////////////////////////////////////////////////////////////////////////////////

        EndpointUnix (const EndpointType, 
                      const ProtocolType,
                      const std::string&, 
                      int,
                      const std::string&);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an endpoint
////////////////////////////////////////////////////////////////////////////////
      
        virtual ~EndpointUnix ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief connect the endpoint
////////////////////////////////////////////////////////////////////////////////

        socket_t connect (double, double);

////////////////////////////////////////////////////////////////////////////////
/// @brief disconnect the endpoint
////////////////////////////////////////////////////////////////////////////////
        
        virtual void disconnect ();

////////////////////////////////////////////////////////////////////////////////
/// @brief init an incoming connection
////////////////////////////////////////////////////////////////////////////////

        virtual bool initIncoming (socket_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief get endpoint domain
////////////////////////////////////////////////////////////////////////////////
        
        int getDomain () const {
          return AF_UNIX;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get port
////////////////////////////////////////////////////////////////////////////////

        int getPort () const {
          return 0;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get host name
////////////////////////////////////////////////////////////////////////////////

        string getHost () const {
          return "localhost";
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get host string for HTTP requests
////////////////////////////////////////////////////////////////////////////////

        string getHostString () const {
          return "localhost";
        }

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief socket file
////////////////////////////////////////////////////////////////////////////////

        string _path;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

#endif

// -----------------------------------------------------------------------------
// --SECTION--                                                        EndpointIp
// -----------------------------------------------------------------------------

    class EndpointIp : public Endpoint {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      protected:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint
////////////////////////////////////////////////////////////////////////////////

        EndpointIp (const EndpointType, 
                    const DomainType, 
                    const ProtocolType,
                    const EncryptionType,
                    const std::string&, 
                    int,
                    const std::string&, 
                    const uint16_t);

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an endpoint
////////////////////////////////////////////////////////////////////////////////
      
        ~EndpointIp ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                  public variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief default port number if none specified
////////////////////////////////////////////////////////////////////////////////

        static const uint16_t _defaultPort;

////////////////////////////////////////////////////////////////////////////////
/// @brief default host if none specified
////////////////////////////////////////////////////////////////////////////////

        static const string _defaultHost;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// --SECTION--                                                   private methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
      
      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief connect the socket
////////////////////////////////////////////////////////////////////////////////
        
        socket_t connectSocket (const struct addrinfo*, double, double);

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief connect the endpoint
////////////////////////////////////////////////////////////////////////////////

        socket_t connect (double, double);

////////////////////////////////////////////////////////////////////////////////
/// @brief disconnect the endpoint
////////////////////////////////////////////////////////////////////////////////
        
        virtual void disconnect ();

////////////////////////////////////////////////////////////////////////////////
/// @brief init an incoming connection
////////////////////////////////////////////////////////////////////////////////

        virtual bool initIncoming (socket_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief get port
////////////////////////////////////////////////////////////////////////////////

        int getPort () const {
          return _port;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get host 
////////////////////////////////////////////////////////////////////////////////

        std::string getHost () const {
          return _host;
        }

////////////////////////////////////////////////////////////////////////////////
/// @brief get host strin for HTTP requests
////////////////////////////////////////////////////////////////////////////////

        std::string getHostString  () const {
          return _host + ':' + triagens::basics::StringUtils::itoa(_port);
        }

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                 private variables
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      private:

////////////////////////////////////////////////////////////////////////////////
/// @brief host name / address (IPv4 or IPv6)
////////////////////////////////////////////////////////////////////////////////

        std::string _host;

////////////////////////////////////////////////////////////////////////////////
/// @brief port number
////////////////////////////////////////////////////////////////////////////////

        uint16_t _port;

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

// -----------------------------------------------------------------------------
// --SECTION--                                                      EndpointIpV4
// -----------------------------------------------------------------------------

    class EndpointIpV4 : public EndpointIp {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint
////////////////////////////////////////////////////////////////////////////////

        EndpointIpV4 (const EndpointType, 
                      const ProtocolType,
                      const EncryptionType,
                      const std::string&, 
                      int,
                      const std::string&, 
                      const uint16_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an endpoint
////////////////////////////////////////////////////////////////////////////////
      
        ~EndpointIpV4 ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief get endpoint domain
////////////////////////////////////////////////////////////////////////////////
        
        int getDomain () const {
          return AF_INET;
        }

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

// -----------------------------------------------------------------------------
// --SECTION--                                                      EndpointIpV6
// -----------------------------------------------------------------------------

    class EndpointIpV6 : public EndpointIp {

// -----------------------------------------------------------------------------
// --SECTION--                                        constructors / destructors
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////

      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief creates an endpoint
////////////////////////////////////////////////////////////////////////////////

        EndpointIpV6 (const EndpointType,
                      const ProtocolType, 
                      const EncryptionType,
                      const std::string&, 
                      int,
                      const std::string&, 
                      const uint16_t);

////////////////////////////////////////////////////////////////////////////////
/// @brief destroys an endpoint
////////////////////////////////////////////////////////////////////////////////
      
        ~EndpointIpV6 ();

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// --SECTION--                                                    public methods
// -----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup Rest
/// @{
////////////////////////////////////////////////////////////////////////////////
    
      public:

////////////////////////////////////////////////////////////////////////////////
/// @brief get endpoint domain
////////////////////////////////////////////////////////////////////////////////
        
        int getDomain () const {
          return AF_INET6;
        }

////////////////////////////////////////////////////////////////////////////////
/// @}
////////////////////////////////////////////////////////////////////////////////

    };

  }
}

#endif

// Local Variables:
// mode: outline-minor
// outline-regexp: "^\\(/// @brief\\|/// {@inheritDoc}\\|/// @addtogroup\\|// --SECTION--\\|/// @\\}\\)"
// End:

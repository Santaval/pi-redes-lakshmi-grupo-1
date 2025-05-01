#ifndef SSLSocket_hpp
#define SSLSocket_hpp

#include "VSocket.hpp"

/**
 * @brief Class to manage SSL connections
 */
class SSLSocket : public VSocket {

   public:
      SSLSocket( bool IPv6 = false );				// Not possible to create with UDP, client constructor
      SSLSocket( char *, char *, bool = false );		// For server connections
      SSLSocket( int );
      ~SSLSocket();
      int Connect( const char *, int );
      int Connect( const char *, const char * );
      size_t Write( const char * );
      size_t Write( const void *, size_t );
      size_t Read( void *, size_t );
      void ShowCerts();
      const char * GetCipher();

      // Se colocan porque daban problemas de abstraccion
      int MakeConnection( const char *, int ) override;
      int MakeConnection( const char *, const char * ) override;

   private:
      void Init( bool = false );		// Defaults to create a client context, true if server context needed
      void InitContext( bool );
      void LoadCertificates( const char *, const char * );

// Instance variables      
      void * SSLContext;				// SSL context
      void * SSLStruct;					// SSL BIO (Basic Input/Output)

};

#endif


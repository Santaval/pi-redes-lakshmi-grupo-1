#ifndef Socket_h
#define Socket_h
#include "VSocket.h"

/**
 * @brief Class to handle a socket
 */
class Socket : public VSocket {

   public:
      Socket( char, bool = false );
      ~Socket();
      int MakeConnection( const char *, int );
      int MakeConnection( const char *, const char * );
      size_t Read( void *, size_t );
      size_t Write( const void *, size_t );
      size_t Write( const char * );

   protected:

};

#endif


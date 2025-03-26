#ifndef VSocket_h
#define VSocket_h
 
/**
 * @brief Virtual socket class
 * This class is the base class for all socket classes.
 */
class VSocket {
   public:
       void BuildSocket( char, bool = false );
      ~VSocket();

      void Close();
      int EstablishConnection( const char *, int );
      int EstablishConnection( const char *, const char * );
      virtual int MakeConnection( const char *, int ) = 0;
      virtual int MakeConnection( const char *, const char * ) = 0;

      virtual size_t Read( void *, size_t ) = 0;
      virtual size_t Write( const void *, size_t ) = 0;
      virtual size_t Write( const char * ) = 0;

      int Bind( int );

      size_t sendTo( const void *, size_t, void * );
      size_t recvFrom( void *, size_t, void * );

   protected:
      int idSocket;   // Socket identifier
      bool IPv6;      // Is IPv6 socket?
      int port;       // Socket associated port
      int domain;
      char type;      // Socket type (datagram, stream, etc.)
        
};

#endif // VSocket_h

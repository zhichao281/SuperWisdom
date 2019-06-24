/*
 *  Copyright (c) 2004-2017 by Jakob Schröter <js@camaya.net>
 *  This file is part of the gloox library. http://camaya.net/gloox
 *
 *  This software is distributed under a license. The full license
 *  agreement can be found in the file LICENSE in this distribution.
 *  This software may not be copied, modified, sold or distributed
 *  other than expressed in the named license agreement.
 *
 *  This software is distributed without any warranty.
 */

#include "../../connectiontcpserver.h"
#include "../../connectiontcpclient.h"
#include "../../connectiontcpbase.h"
#include "../../connectiondatahandler.h"
#include "../../connectionhandler.h"
#include "../../logsink.h"
#include "../../loghandler.h"
#include "../../gloox.h"
using namespace gloox;

#include <stdio.h>
#include <locale.h>
#include <string>
#include <cstdio> // [s]print[f]
class TestHandler : public gloox::ConnectionHandler, public gloox::LogHandler, public gloox::ConnectionDataHandler
{
  public:
    TestHandler() : m_test( 0 ) {}

    virtual void handleReceivedData( const ConnectionBase* /*connection*/, const std::string& data )
    {
//       printf( "handleReceivedData: %s\n", data.c_str() );
    }

    virtual void handleConnect( const ConnectionBase* connection )
    {
//       printf( "handleConnect: %s:%d\n", connection->server().c_str(), connection->port() );
    }

    virtual void handleDisconnect( const ConnectionBase* /*connection*/, ConnectionError reason )
    {
//       printf( "handleDisconnect(): %d\n", reason );
    }

    virtual void handleIncomingConnection( ConnectionBase* server, ConnectionBase* connection )
    {
      switch( m_test )
      {
        case 3:
//           printf( "Incoming connection from %s:%d to %s:%d\n", connection->server().c_str(), connection->port(),
//                                                                server->server().c_str(), server->port() );
          break;
        default:
          break;
      }
    }

    virtual void handleLog( LogLevel /*level*/, LogArea /*area*/, const std::string& message )
    {
//       printf( "Test %d says: %s\n", m_test, message.c_str() );
    }

    void setTest( int test ) { m_test = test; }

  private:
    int m_test;

};

int main( int /*argc*/, char** /*argv*/ )
{
  int fail = 0;

  TestHandler* h = new TestHandler();
  LogSink log;
  log.registerLogHandler( LogLevelDebug, LogAreaAll, h );
  ConnectionTCPServer server( h, log, gloox::EmptyString, 54321 );

  std::string name = "listen";
  h->setTest( 1 );
  ConnectionError ret = server.connect();

  if( ret != ConnNoError )
  {
    ++fail;
    fprintf( stderr, "test '%s' failed\n", name.c_str() );
    printf( "connect() returned: %d\n", ret );
  }
  // -------

  name = "connect";
  h->setTest( 2 );
  ConnectionTCPClient c( h, log, "127.0.0.1", 54321 );
  ret = c.connect();
  if( ret != ConnNoError )
  {
    ++fail;
    fprintf( stderr, "test '%s' failed\n", name.c_str() );
    printf( "connect() returned: %d\n", ret );
  }
  // -------

  name = "accept";
  h->setTest( 3 );
  ret = server.recv();
  if( ret != ConnNoError )
  {
    ++fail;
    fprintf( stderr, "test '%s' failed\n", name.c_str() );
    printf( "connect() returned: %d\n", ret );
  }
  // -------

  if( fail == 0 )
  {
    printf( "ConnectionTCPServer: OK\n" );
    return 0;
  }
  else
  {
    fprintf( stderr, "ConnectionTCPServer: %d test(s) failed\n", fail );
    return 1;
  }

}

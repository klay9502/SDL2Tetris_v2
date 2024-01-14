#include "stdafx.h"
#include "ServerSocket.h"


CServerSocket::CServerSocket(void)
{
	m_SocketInfoIDCount = 0;
}


CServerSocket::~CServerSocket(void)
{
}

bool CServerSocket::Init( void )
{
	if( WSAStartup( MAKEWORD( 2, 2 ), &m_WSAData ) != 0)
	{
		fprintf( stderr, "ERROR::CServerSocket::Init() - WSAStartup()\n" );
		return false;
	}

	m_ListenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( m_ListenSocket == INVALID_SOCKET )
	{
		fprintf( stderr, "ERROR::CServerSocket::Init() - socket()\n" );
		return false;
	}

	ZeroMemory( &m_ServerAddr, sizeof( m_ServerAddr ) );
	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	m_ServerAddr.sin_port = htons( PORT_NUMBER );

	if( bind( m_ListenSocket, ( struct sockaddr* )&m_ServerAddr, sizeof(m_ServerAddr) ) == SOCKET_ERROR )
	{
		fprintf( stderr, "ERROR::CServerSocket::Init() - bind()\n" );
		return false;
	}

	if( listen( m_ListenSocket, SOMAXCONN ) == SOCKET_ERROR )
	{
		fprintf( stderr, "ERROR::CServerSocket::Init() - listen()\n" );
		return false;
	}

	FD_ZERO( &m_ReadSet );
	FD_ZERO( &m_TempSet );

	FD_SET( m_ListenSocket, &m_ReadSet );

	return true;
}

bool CServerSocket::Loop( void )
{
	m_TempSet = m_ReadSet;
	// 소켓 상태 변화 감지
	m_FD_SocketCount = select( 0, &m_TempSet, nullptr, nullptr, nullptr );
	if( m_FD_SocketCount == SOCKET_ERROR )
	{
 		fprintf( stderr, "ERROR::CServerSocket::Loop() - select()\n" );
		return false;
	}

	// 변경된 소켓에 대한 처리
	for( int i = 0; i < m_FD_SocketCount; i++ )
	{
		SOCKET socket = m_TempSet.fd_array[i];

		// 새로운 클라이언트 연결이 있을 경우
		if( socket == m_ListenSocket )
		{
			this->AddClient();
		}
		// 기존 클라이언트 소켓에서 데이터를 수신했을 경우
		else
		{
			for( int j = 0; j < m_ClientList.size(); j++ )
			{
				if( socket != m_ClientList[j].socket )
				{
					continue;
				}

				char buf[MAX_BUFSIZE] = {};

				if( recv( m_ClientList[j].socket, buf, MAX_BUFSIZE, 0) <= 0 )
				{
					this->DelClient( j );
				}
				else
				{
					fprintf( stdout, "INFO::ServerSocket::Loop() - [Client No.%d] Input Data.\n", m_ClientList[j].id);
				}
			}
		}
	}

	return true;
}

void CServerSocket::Release( void )
{
	closesocket( m_ListenSocket );
	WSACleanup();
	
	return;
}

void CServerSocket::AddClient( void )
{
	SOCKADDR_IN addr = {};
	int lenAddr = sizeof( addr );
	char buf[MAX_BUFSIZE] = {};

	ZeroMemory( &addr, lenAddr );

	SOCKET_INFO newClient = {};

	newClient.id = m_SocketInfoIDCount;
	newClient.socket = accept( m_ListenSocket, ( struct sockaddr* )&addr, &lenAddr );
	recv( newClient.socket, buf, MAX_BUFSIZE, 0 );
	newClient.nickname = std::string( buf );

	fprintf( stdout, "INFO::ServerSocket::AddClient() - New Client(No.%d) Detected, Nickname: %s, IP: %s\n", newClient.id, newClient.nickname.c_str(), inet_ntoa( addr.sin_addr ) );
	m_ClientList.push_back( newClient );	// 배열에 클라이언트 추가
	FD_SET( m_ClientList.back().socket, &m_ReadSet );

	m_SocketInfoIDCount++;

	return;
}

void CServerSocket::DelClient( int _index )
{
	fprintf( stdout, "INFO::ServerSocket::DelClient() - Client No.%d ( Nickname: %s ) is Disconnected.\n", m_ClientList[_index].id, m_ClientList[_index].nickname.c_str() );

	closesocket( m_ClientList[_index].socket );
	FD_CLR( m_ClientList[_index].socket, &m_ReadSet );
	m_ClientList.erase( m_ClientList.begin() + _index );

	return;
}
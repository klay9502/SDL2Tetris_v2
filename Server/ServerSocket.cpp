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

bool CServerSocket::Loop( CSQLManager* pSQLManager )
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
			pSQLManager->InitClientDBData( &m_ClientList.back() );

			if( m_ClientList.size() < 3 )
			{
				GANE_INFO gameInfoBuf = {STATE_WAIT, NULL};
				fprintf( stdout, "INFO::ServerSocket::Loop() - [Client No.%d] Waiting another client.\n", m_ClientList.back().id);
				send( m_ClientList.back().socket, ( char* )&gameInfoBuf, MAX_BUFSIZE, 0 );
			}

			if( m_ClientList.size() == 2 )
			{
				GANE_INFO gameInfoBuf = {STATE_READY, NULL};
				for( int j = 0; j < m_ClientList.size(); j++ )
				{
					send( m_ClientList[j].socket, ( char* )&gameInfoBuf, MAX_BUFSIZE, 0 );
				}
			}
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

				// 클라이언트 닫혔을 때 처리
				if( recv( m_ClientList[j].socket, buf, MAX_BUFSIZE, 0) <= 0 )
				{
					this->DelClient( j );
				}
				else
				{
					// 그 외 데이터 수신의 경우 처리

					std::string str(buf);
					
					// 준비 신호
 					if( str == "READY" )
					{
						m_ClientList[j].state = STATE_READY;
						strcpy_s( buf, "S_READY" );
						send( m_ClientList[j].socket, buf, MAX_BUFSIZE, 0 );
						fprintf( stdout, "INFO::ServerSocket::Loop() - [Client No.%d] Player is Ready.\n", m_ClientList[j].id);
					}

					// 플레이어 모두가 준비 했을 때 처리
					// 이는 개선이 필요한 코드임
					if( m_ClientList[0].state == STATE_READY &&  m_ClientList[1].state == STATE_READY )
					{
						GANE_INFO gameInfoBuf = {STATE_PLAY, NULL};
						m_ClientList[0].state = STATE_PLAY;
						send( m_ClientList[0].socket, ( char* )&gameInfoBuf, MAX_BUFSIZE, 0 );
						m_ClientList[1].state = STATE_PLAY;
						send( m_ClientList[1].socket, ( char* )&gameInfoBuf, MAX_BUFSIZE, 0 );
						fprintf( stdout, "INFO::ServerSocket::Loop() - GAME START!\n" );
					}

					// 게임 오버 신호
					if( str == "GAMEOVER" )
					{
						m_ClientList[j].state = STATE_GAMEOVER;
						fprintf( stdout, "INFO::ServerSocket::Loop() - [Client No.%d] GAME OVER.\n", m_ClientList[j].id);

						if( m_ClientList[0].state == STATE_GAMEOVER )
						{
							GANE_INFO gameInfoBuf = {STATE_GAMEOVER, NULL};
							send( m_ClientList[0].socket, ( char* )&gameInfoBuf, MAX_BUFSIZE, 0 );
							m_ClientList[0].loseCount += 1;
							gameInfoBuf.gameState = STATE_WINNER;
							send( m_ClientList[1].socket, ( char* )&gameInfoBuf, MAX_BUFSIZE, 0 );
							m_ClientList[1].winCount += 1;
						}
						else
						{
							GANE_INFO gameInfoBuf = {STATE_WINNER, NULL};
							send( m_ClientList[0].socket, ( char* )&gameInfoBuf, MAX_BUFSIZE, 0 );
							m_ClientList[0].winCount += 1;
							gameInfoBuf.gameState = STATE_GAMEOVER;
							send( m_ClientList[1].socket, ( char* )&gameInfoBuf, MAX_BUFSIZE, 0 );
							m_ClientList[1].loseCount += 1;
						}

						recv( m_ClientList[0].socket, reinterpret_cast<char*>(&m_ClientList[0].hiScore), sizeof( m_ClientList[0].hiScore ), 0 );
						recv( m_ClientList[1].socket, reinterpret_cast<char*>(&m_ClientList[1].hiScore), sizeof( m_ClientList[1].hiScore ), 0 );

						// printf("%d - %d\n", 0, m_ClientList[0].hiScore);
						// printf("%d - %d\n", 1, m_ClientList[1].hiScore);
						
						pSQLManager->UpdateClientDBData( &m_ClientList[0] );
						pSQLManager->UpdateClientDBData( &m_ClientList[1] );
					}

					if ( str == "RANK" )
					{
						strcpy_s( buf, "DATA_RANK" );
						send( m_ClientList[j].socket, buf, MAX_BUFSIZE, 0 );

						int printSize = pSQLManager->getPrintSize();

						for( int i = 1; i <= printSize; i++ )
						{
							std::string temp = pSQLManager->getRankString( i );
							send( m_ClientList[j].socket, temp.c_str(), temp.length(), 0 );
						}

						strcpy_s( buf, "S_RANK" );
						send( m_ClientList[j].socket, buf, MAX_BUFSIZE, 0 );
					}
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
	newClient.winCount = 0;
	newClient.loseCount = 0;
	newClient.hiScore = 0;
	newClient.state = STATE_WAIT;

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
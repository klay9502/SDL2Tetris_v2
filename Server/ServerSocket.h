#pragma once

typedef struct _SOCKET_INFO
{
	int			id;
	SOCKET		socket;
	std::string nickname;
} SOCKET_INFO;

class CServerSocket
{
public:
	CServerSocket(void);
	~CServerSocket(void);

	bool	Init( void );
	bool	Loop( void );
	void	Release( void );

	void	AddClient( void );
	void	DelClient( int _index );

private:
	WSAData			m_WSAData;
	SOCKET			m_ListenSocket;		// 서버 소켓
	SOCKADDR_IN		m_ServerAddr;
	fd_set			m_ReadSet, m_TempSet;
	int				m_FD_SocketCount;
	int				m_SocketInfoIDCount;

	// 연결된 클라이언트 소켓들을 저장하는 배열
	std::vector<SOCKET_INFO> m_ClientList;
};


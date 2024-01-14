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
	SOCKET			m_ListenSocket;		// ���� ����
	SOCKADDR_IN		m_ServerAddr;
	fd_set			m_ReadSet, m_TempSet;
	int				m_FD_SocketCount;
	int				m_SocketInfoIDCount;

	// ����� Ŭ���̾�Ʈ ���ϵ��� �����ϴ� �迭
	std::vector<SOCKET_INFO> m_ClientList;
};


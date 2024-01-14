#pragma once

class CApp
{
public:
	CApp(void);
	~CApp(void);

	bool	Init( void );
	void	Loop( void );
	void	Release( void );

private:
	CServerSocket* m_pServerSocket;
};


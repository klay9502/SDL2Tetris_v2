#pragma once

class CSQLManager
{
public:
	CSQLManager(void);
	~CSQLManager(void);

	bool		Init( void );
	void		Release( void );

	void		InitClientDBData( SOCKET_INFO* client );
	void		UpdateClientDBData( SOCKET_INFO* client );
	std::string getRankString( int _index );
	int			getPrintSize( void );
	
	bool		duplicateCheck( std::string _nickname );
private:
	SQLHENV			m_Henv;
	SQLHDBC			m_Hdbc;
	SQLWCHAR*		m_ConnectionString;
	SQLHSTMT		m_Hstmt;

};


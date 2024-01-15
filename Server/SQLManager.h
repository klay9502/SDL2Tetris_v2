#pragma once

class CSQLManager
{
public:
	CSQLManager(void);
	~CSQLManager(void);

	bool	Init( void );
	void	Release( void );

	void	InsertPlayerScore( std::string _nickname );

private:
	SQLHENV			m_Henv;
	SQLHDBC			m_Hdbc;
	SQLWCHAR*		m_ConnectionString;
	SQLHSTMT		m_Hstmt;

};


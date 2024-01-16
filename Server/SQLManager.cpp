#include "stdafx.h"
#include "SQLManager.h"

std::wstring SQLWCHARToWstring(SQLWCHAR* sqlWcharArray) {
    size_t length = 0;
    while (sqlWcharArray[length] != L'\0') {
        length++;
    }
    
    wchar_t* wcharArray = reinterpret_cast<wchar_t*>(sqlWcharArray);
    std::wstring wstr(wcharArray, length);
    
    return wstr;
}

CSQLManager::CSQLManager(void)
{
}

CSQLManager::~CSQLManager(void)
{
}

bool CSQLManager::Init( void )
{
	// ODBC 환경 핸들 생성
	SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_Henv );
	if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		fprintf( stderr, "ERROR::CSQLManager::Init() - SQLAllocHandle()::ODBC\n" );
		return false;
	}

	// ODBC 버전 설정
	ret = SQLSetEnvAttr( m_Henv, SQL_ATTR_ODBC_VERSION, ( void* )SQL_OV_ODBC3, 0 );
	if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		fprintf( stderr, "ERROR::CSQLManager::Init() - SQLSetEnvAttr()::SQL_INVALID_HANDLE\n" );
		return false;
	}

	// DB 연결 핸들 생성
	ret = SQLAllocHandle( SQL_HANDLE_DBC, m_Henv, &m_Hdbc );
	if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		fprintf( stderr, "ERROR::CSQLManager::Init() - SQLAllocHandle()::DB\n" );
		return false;
	}

	// 연결 문자열 생성
	m_ConnectionString = ( SQLWCHAR* )L"DRIVER={SQL Server};SERVER=DESKTOP-II3RSPQ;DATABASE=TetrisDB;UID=;PWD=;";

	// DB에 연결
	ret = SQLDriverConnectW( m_Hdbc, NULL, m_ConnectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE );
	if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		fprintf( stderr, "ERROR::CSQLManager::Init() - SQLDriverConnectW()\n" );
		return false;
	}

	ret = SQLAllocHandle( SQL_HANDLE_STMT, m_Hdbc, &m_Hstmt );
	if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		fprintf( stderr, "ERROR::CSQLManager::Init() - SQLAllocHandle()::HSTMT\n" );
		return false;
	}

	return true;
}

void CSQLManager::Release( void )
{
	SQLFreeHandle( SQL_HANDLE_STMT, m_Hstmt );
	SQLDisconnect( m_Hdbc );
	SQLFreeHandle( SQL_HANDLE_DBC, m_Hdbc );
	SQLFreeHandle( SQL_HANDLE_ENV, m_Henv );

	return;
}

void CSQLManager::InitClientDBData( SOCKET_INFO* client )
{
	if( this->duplicateCheck( client->nickname ) )
	{
		std::string temp = "SELECT * FROM PlayerTBL WHERE nickname = \'" + client->nickname + "\';";
		std::wstring wTemp;
		wTemp.assign( temp.begin(), temp.end() );

		SQLWCHAR* sql = ( SQLWCHAR* ) wTemp.c_str();
		SQLINTEGER winCount;
		SQLINTEGER loseCount;
		SQLINTEGER hiScore;

		SQLRETURN ret = SQLExecDirectW(m_Hstmt, sql, SQL_NTS);

		if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
		{	
			while( SQLFetch( m_Hstmt ) == SQL_SUCCESS )
			{
				SQLGetData( m_Hstmt, 2, SQL_C_LONG, &winCount, sizeof( winCount ), NULL );
				SQLGetData( m_Hstmt, 3, SQL_C_LONG, &loseCount, sizeof( loseCount ), NULL );
				SQLGetData( m_Hstmt, 4, SQL_C_LONG, &hiScore, sizeof( hiScore ), NULL );

				client->winCount = static_cast<int>( winCount );
				client->loseCount = static_cast<int>( loseCount );
				client->hiScore = static_cast<int>( hiScore );
			}
		}
		else
		{
			fprintf( stderr, "ERROR::CSQLManager::duplicateCheck() - SQLExecDirectW()\n" );
		}

		SQLFreeStmt(m_Hstmt, SQL_CLOSE);
		SQLFreeStmt(m_Hstmt, SQL_UNBIND);
		SQLFreeStmt(m_Hstmt, SQL_RESET_PARAMS);

		return;
	}
	else
	{
		std::string temp = "INSERT INTO PlayerTBL VALUES ( \'" + client->nickname + "\', 0, 0, 0 );";
		std::wstring wTemp;
		wTemp.assign( temp.begin(), temp.end() );

		SQLWCHAR* sql = ( SQLWCHAR* ) wTemp.c_str();

		SQLRETURN ret = SQLExecDirectW(m_Hstmt, sql, SQL_NTS);
		if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
		{
			fprintf( stderr, "ERROR::CSQLManager::InitClientDBData() - SQLExecDirectW()\n" );
			return;
		}

		client->winCount = 0;
		client->loseCount = 0;
		client->hiScore = 0;

		fprintf( stderr, "INFO::CSQLManager::InitClientDBData() - Insert player data in DB.\n" );

		SQLFreeStmt(m_Hstmt, SQL_CLOSE);
		SQLFreeStmt(m_Hstmt, SQL_UNBIND);
		SQLFreeStmt(m_Hstmt, SQL_RESET_PARAMS);

		return;
	}
}

void CSQLManager::UpdateClientDBData( SOCKET_INFO* client )
{
	std::string temp = "SELECT highscore FROM PlayerTBL WHERE nickname = \'" + client->nickname + "\';";
	std::wstring wTemp;
	wTemp.assign( temp.begin(), temp.end() );

	SQLWCHAR* sql = ( SQLWCHAR* ) wTemp.c_str();
	SQLINTEGER hiScore;

	SQLRETURN ret = SQLExecDirectW(m_Hstmt, sql, SQL_NTS);

	if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
	{	
		while( SQLFetch( m_Hstmt ) == SQL_SUCCESS )
		{
			SQLGetData( m_Hstmt, 1, SQL_C_LONG, &hiScore, sizeof( hiScore ), NULL );

			if( client->hiScore < static_cast<int>( hiScore ) )
			{
				client->hiScore = static_cast<int>( hiScore );
			}
		}
	}
	else
	{
		fprintf( stderr, "ERROR::CSQLManager::UpdateClientDBData() - SQLExecDirectW()\n" );
	}

	SQLFreeStmt(m_Hstmt, SQL_CLOSE);
	SQLFreeStmt(m_Hstmt, SQL_UNBIND);
	SQLFreeStmt(m_Hstmt, SQL_RESET_PARAMS);

	temp = "UPDATE PlayerTBL SET [win_count] = " + std::to_string( client->winCount ) +
		" , [lose_count] = " + std::to_string( client->loseCount ) + " , [highscore] = " + std::to_string( client->hiScore ) +
		" WHERE nickname = \'" + client->nickname + "\';";

	wTemp.assign( temp.begin(), temp.end() );

	sql = ( SQLWCHAR* ) wTemp.c_str();

	ret = SQLExecDirectW(m_Hstmt, sql, SQL_NTS);
	if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		fprintf( stderr, "ERROR::CSQLManager::UpdateClientDBData() - SQLExecDirectW()\n" );
		return;
	}

	fprintf( stderr, "INFO::CSQLManager::UpdateClientDBData() - Update player data in DB.\n" );

	SQLFreeStmt(m_Hstmt, SQL_CLOSE);
	SQLFreeStmt(m_Hstmt, SQL_UNBIND);
	SQLFreeStmt(m_Hstmt, SQL_RESET_PARAMS);
}

std::string CSQLManager::getRankString( int _index )
{
	std::wstring wTemp = L"SELECT TOP(1) * FROM ( SELECT TOP(" + std::to_wstring( _index ) +
		L") * FROM PlayerTBL ORDER BY highscore DESC ) AS SubQuery ORDER BY highscore ASC;";

	SQLWCHAR* sql = ( SQLWCHAR* ) wTemp.c_str();

	SQLRETURN ret = SQLExecDirectW(m_Hstmt, sql, SQL_NTS);

	std::wstring wstr;
	std::string str;

	if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
	{
		SQLWCHAR nickname[20];
		SQLINTEGER winCount;
		SQLINTEGER loseCount;
		SQLINTEGER highScore;
		
		while( SQLFetch( m_Hstmt ) == SQL_SUCCESS )
		{
			SQLGetData( m_Hstmt, 1, SQL_C_WCHAR, nickname, 10, NULL );
			SQLGetData( m_Hstmt, 2, SQL_C_LONG, &winCount, sizeof( winCount ), NULL );
			SQLGetData( m_Hstmt, 3, SQL_C_LONG, &loseCount, sizeof( loseCount ), NULL );
			SQLGetData( m_Hstmt, 4, SQL_C_LONG, &highScore, sizeof( highScore ), NULL );

			wstr = SQLWCHARToWstring(nickname) + L" " + 
				std::to_wstring( winCount ) + L" " +
				std::to_wstring( loseCount ) + L" " + 
				std::to_wstring( highScore ) + L" ";
		}
	}
	else
	{
		fprintf( stderr, "ERROR::CSQLManager::getRankString() - SQLExecDirectW()\n" );
	}

	SQLFreeStmt(m_Hstmt, SQL_CLOSE);
	SQLFreeStmt(m_Hstmt, SQL_UNBIND);
	SQLFreeStmt(m_Hstmt, SQL_RESET_PARAMS);

	return str.assign( wstr.begin(), wstr.end() ).c_str();
}

int CSQLManager::getPrintSize( void )
{
	std::wstring wTemp = L"SELECT COUNT(*) FROM PlayerTBL;";

	SQLWCHAR* sql = ( SQLWCHAR* ) wTemp.c_str();
	SQLRETURN ret = SQLExecDirectW(m_Hstmt, sql, SQL_NTS);

	SQLINTEGER totalRows;

	if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
	{	
		while( SQLFetch( m_Hstmt ) == SQL_SUCCESS )
		{
			SQLGetData( m_Hstmt, 1, SQL_C_LONG, &totalRows, sizeof( totalRows ), NULL );
		}
	}
	else
	{
		fprintf( stderr, "ERROR::CSQLManager::getRankString() - SQLExecDirectW()\n" );
	}

	SQLFreeStmt(m_Hstmt, SQL_CLOSE);
	SQLFreeStmt(m_Hstmt, SQL_UNBIND);
	SQLFreeStmt(m_Hstmt, SQL_RESET_PARAMS);

	if( static_cast<int>(totalRows) > 10 )
	{
		return 10;
	}
	else
	{
		return static_cast<int>(totalRows);
	}
}

bool CSQLManager::duplicateCheck( std::string _nickname )
{
	std::string temp = "SELECT Count(*) FROM PlayerTBL WHERE nickname = \'" + _nickname + "\';";
	std::wstring wTemp;
	wTemp.assign( temp.begin(), temp.end() );

	SQLWCHAR* sql = ( SQLWCHAR* ) wTemp.c_str();
	SQLINTEGER check;

	SQLRETURN ret = SQLExecDirectW(m_Hstmt, sql, SQL_NTS);

	if( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO )
	{	
		while( SQLFetch( m_Hstmt ) == SQL_SUCCESS )
		{
			SQLGetData( m_Hstmt, 1, SQL_C_LONG, &check, sizeof( check ), NULL );
		}
	}
	else
	{
		fprintf( stderr, "ERROR::CSQLManager::duplicateCheck() - SQLExecDirectW()\n" );
	}

	SQLFreeStmt(m_Hstmt, SQL_CLOSE);
	SQLFreeStmt(m_Hstmt, SQL_UNBIND);
	SQLFreeStmt(m_Hstmt, SQL_RESET_PARAMS);

	if( static_cast<int>(check) )
	{
		fprintf( stderr, "INFO::CSQLManager::duplicateCheck() - The DB already has that nickname.\n" );

		return true;
	}
	else
	{
		return false;
	}
}
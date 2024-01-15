#include "stdafx.h"
#include "SQLManager.h"


CSQLManager::CSQLManager(void)
{
}

CSQLManager::~CSQLManager(void)
{
}

bool CSQLManager::Init( void )
{
	// ODBC ȯ�� �ڵ� ����
	SQLRETURN ret = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_Henv );
	if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		fprintf( stderr, "ERROR::CSQLManager::Init() - SQLAllocHandle()::ODBC\n" );
		return false;
	}

	// ODBC ���� ����
	ret = SQLSetEnvAttr( m_Henv, SQL_ATTR_ODBC_VERSION, ( void* )SQL_OV_ODBC3, 0 );
	if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		fprintf( stderr, "ERROR::CSQLManager::Init() - SQLSetEnvAttr()::SQL_INVALID_HANDLE\n" );
		return false;
	}

	// DB ���� �ڵ� ����
	ret = SQLAllocHandle( SQL_HANDLE_DBC, m_Henv, &m_Hdbc );
	if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		fprintf( stderr, "ERROR::CSQLManager::Init() - SQLAllocHandle()::DB\n" );
		return false;
	}

	// ���� ���ڿ� ����
	m_ConnectionString = ( SQLWCHAR* )L"DRIVER={SQL Server};SERVER=DESKTOP-II3RSPQ;DATABASE=TetrisDB;UID=;PWD=;";

	// DB�� ����
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

void CSQLManager::InsertPlayerScore( std::string _nickname )
{
	std::string temp = "INSERT INTO PlayerTBL VALUES ( \'" + _nickname + "\', 1, 2 )";
	SQLWCHAR* sql = ( SQLWCHAR* ) temp.c_str();

	SQLRETURN ret = SQLExecDirectW(m_Hdbc, sql, SQL_NTS);
	if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
	{
		fprintf( stderr, "ERROR::CSQLManager::InsertPlayerScore() - SQLExecDirectW()\n" );
		return;
	}

	fprintf( stderr, "INFO::CSQLManager::InsertPlayerScore() - Insert player data in DB.\n" );
}
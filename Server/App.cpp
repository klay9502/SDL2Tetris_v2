#include "stdafx.h"
#include "App.h"


CApp::CApp(void)
	: m_pServerSocket( nullptr )
	, m_pSQLManager( nullptr )
{
}


CApp::~CApp(void)
{
	if( m_pSQLManager != nullptr )
	{
		this->Release();
	}

	if( m_pServerSocket != nullptr )
	{
		this->Release();
	}
}

bool CApp::Init( void )
{
	m_pServerSocket = new CServerSocket();

	if( m_pServerSocket->Init() != true )
	{
		fprintf( stderr, "ERROR::CApp::Init() - m_pServerSocket->Init()\n" );
		return false;
	}

	fprintf( stdout, "SUCCESS::CApp::Init() - m_pServerSocket->Init()\n" );

	m_pSQLManager = new CSQLManager();

	if( m_pSQLManager->Init() != true )
	{
		fprintf( stderr, "ERROR::CApp::Init() - m_pSQLManager->Init()\n" );
		return false;
	}

	fprintf( stdout, "SUCCESS::CApp::Init() - m_pSQLManager->Init()\n" );

	return true;
}

void CApp::Loop( void )
{
	fprintf( stdout, "SUCCESS::CApp::Loop() - Server running...\n" );

	while( true )
	{
		if( !m_pServerSocket->Loop( m_pSQLManager ) )
		{
			break;
		}
	}
}

void CApp::Release( void )
{
	m_pSQLManager->Release();
	delete m_pSQLManager;
	m_pSQLManager = nullptr;

	m_pServerSocket->Release();
	delete m_pServerSocket;
	m_pServerSocket = nullptr;
}
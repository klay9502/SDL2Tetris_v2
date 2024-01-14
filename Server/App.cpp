#include "stdafx.h"
#include "App.h"


CApp::CApp(void)
	: m_pServerSocket( nullptr )
{
}


CApp::~CApp(void)
{
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

	return true;
}

void CApp::Loop( void )
{
	fprintf( stdout, "SUCCESS::CApp::Loop() - Server running...\n" );

	while( true )
	{
		if( !m_pServerSocket->Loop() )
		{
			break;
		}
	}
}

void CApp::Release( void )
{
	m_pServerSocket->Release();
	delete m_pServerSocket;
	m_pServerSocket = nullptr;
}
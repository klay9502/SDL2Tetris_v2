// main.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "App.h"

int main( void )
{
	CApp app;

	if( !app.Init() )
	{
		fprintf( stderr, "����::main.cpp - app Ŭ���� �ʱ�ȭ ����.\n" );
		app.Release();
		return 1;
	}

	app.Loop();

	app.Release();

    return 0;
}

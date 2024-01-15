// main.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "App.h"

int main( void )
{
	CApp app;

	if( !app.Init() )
	{
		fprintf( stderr, "ERROR::main.cpp - Init()\n" );
		app.Release();
		return 1;
	}

	app.Loop();

	app.Release();

    return 0;
}

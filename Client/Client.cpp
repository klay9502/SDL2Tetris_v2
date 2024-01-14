// Client.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

typedef struct _KeyEventValue
{
	int SDLKValue;
} KeyEventValue;


int _tmain(int argc, _TCHAR* argv[])
{
	if( SDL_Init( SDL_INIT_EVERYTHING ) != 0)
	{
		fprintf( stderr, "SDL failed to initialize: %s\n",SDL_GetError() );
		return false;
	}

	fprintf( stdout, "SUCCESS::SDL Initialized\n" );

	SDL_Window* m_pWindow = SDL_CreateWindow( "title",
								  SDL_WINDOWPOS_UNDEFINED,
								  SDL_WINDOWPOS_UNDEFINED,
								  1280,
								  720,
								  SDL_WINDOW_SHOWN );

	if( !m_pWindow )
	{
		fprintf( stderr, "SDL failed to initialize: %s\n",SDL_GetError() );
		SDL_Quit();
		return false;
	}

	// SDL2_TTF

	if( TTF_Init() == -1 )
	{
		fprintf( stderr, "SDL2_TTL failed to initialize: %s\n",TTF_GetError() );
		return false;
	}

	fprintf( stdout, "SUCCESS::SDL2_TTF Initialized.\n" );

	SOCKET server_socket;
	WSADATA wsadata;
	SOCKADDR_IN server_address = { 0 };
 
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        printf("WSAStartup 에러\n");
        return -1;
    }
 
    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        puts("socket 에러.");
        return -1;
    }
 
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(9000);
 
    if ((connect(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))) < 0)
    {
        puts("connect 에러.");
        return -1;
    
	}

	char buf[512] = "NICKNAME";

	send(server_socket, buf, 512, 0);

	printf("Hello Server!\n");


	int quit = 0;
	KeyEventValue keyEventBuf;

    while (!quit)
	{
		SDL_Event event;
        while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
			case SDL_QUIT:
				quit = 1;
				break;
				
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym )
				{
				case SDLK_ESCAPE:
					quit = 1;
					break;
				case SDLK_RIGHT:
				case SDLK_LEFT:
				case SDLK_UP:
				case SDLK_DOWN:
					keyEventBuf.SDLKValue = event.key.keysym.sym;
					if( send( server_socket, ( char* )&keyEventBuf, sizeof( keyEventBuf ), 0 ) == SOCKET_ERROR )
					{
						fprintf( stderr, "ERROR::SDLWindows - send() Error. Code: %d\n", WSAGetLastError() );
					}
					break;
				default:
					break;
				}
				break;

			default:
				break;
			}
        }
    }
}


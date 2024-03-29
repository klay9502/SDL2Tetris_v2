// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define PORT_NUMBER	9000
#define MAX_BUFSIZE	512
#define MAX_CLIENT	2

#include "targetver.h"

#include <stdio.h>
#include <WinSock2.h>
#include <process.h>
#include <string>
#include <vector>
#include <thread>
#include <algorithm>

#include "ServerSocket.h"

// SQL
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include "SQLManager.h"

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

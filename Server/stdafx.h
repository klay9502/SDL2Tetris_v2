// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
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

#include "ServerSocket.h"

// SQL
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include "SQLManager.h"

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

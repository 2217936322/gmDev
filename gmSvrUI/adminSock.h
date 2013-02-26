#ifndef _ADMIN_SOCK_H
#define _ADMIN_SOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <WinSock2.h>
#include <Windows.h>
#include <time.h>
#include <process.h>

#include <ptrList.h>
#include <mxml.h>

// -----------------------------------------------------------------------------------------
// ͬ����Ϣ����/�����߳�
// -----------------------------------------------------------------------------------------

// ������������
SOCKET CmdSocket_CreateAndConnect(const CHAR *ip, WORD port);
// ���緢��
BOOL CmdSocket_Send(SOCKET cmdSock, const UCHAR *data, int dataLen);
// �������
BOOL CmdSocket_Receive(SOCKET cmdSock, UCHAR **recvBuf, int *recvLen);
// �ر�����
void CmdSocket_Close(SOCKET cmdSock);


// -----------------------------------------------------------------------------------------
// �첽��Ϣ�����߳�
// �������ὫһЩ֪ͨ��Ϣͨ���˽ӿڷ��͸��ͻ��ˣ���ʽΪXML
// -----------------------------------------------------------------------------------------

// �ص�����
typedef void (CALLBACK MSGSOCKET_CB)(UCHAR *msgData, int msgLen);

// ��ʼ�߳�
BOOL BeginMsgSocketThread(MSGSOCKET_CB *msgSockCB,
                          const CHAR *svrIp, WORD svrPort,
                          const CHAR *svrPwd);
// �����߳�
void StopMsgSocketThread();


#ifdef __cplusplus
}
#endif

#endif

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

// -------------------------------------------------------------------------------------
// kv
struct kv
{
    char *k;
    char *v;
};
struct kv *make_kv(const CHAR *k, const CHAR *v);
void free_kv(void *kv);
CHAR *find_kv(struct ptrList *keyVals, const CHAR *k);
CHAR *KeyValueToXml(struct ptrList *keyVals);
void KeyValueFromXml(const CHAR *szXml, struct ptrList **keyVals);

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
typedef void (CALLBACK MSGSOCKET_CB)(CHAR *msgData, int msgLen);

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

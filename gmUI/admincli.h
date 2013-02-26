#ifndef _ADMIN_H
#define _ADMIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <WinSock2.h>
#include <Windows.h>
#include <time.h>
#include <process.h>

#include "idx.h"
#include "mxml.h"
#include "zlib.h"
#include "adminSock.h"

#define STR_REGISTER_SOCKET         "register message socket"
#define STR_TEST_SOCKET             "test message socket"
#define STR_GENERAL_INFO            "general info"

#define STR_GET_OPTIONS             "get options"
#define STR_SET_OPTIONS             "set options"

#define STR_GET_DOWNLOADING_TASKS   "get downloading tasks" // ��ȡ�������ػ�����б�
#define STR_GET_WAITING_TASKS       "get waiting tasks"     // ��ȡ���ڵȴ��б�
#define STR_GET_AUTO_UPDATE_TASKS   "get auto update tasks" // ��ȡ��Ҫ�Զ����µ���Դ�б�
#define STR_GET_SEEDING_TASKS       "get seeding tasks"     // ��ȡ���������б�
#define STR_GET_UPLOADING_TASKS     "get uploading tasks"   // ��ȡ�����ϴ��б�
#define STR_GET_LOCAL_IDX_LIST      "get local idx list"    // ������Դ
#define STR_GET_NET_IDX_LIST        "get net idx list"      // ������Դ

#define STR_ADD_TASKS               "add tasks"             // ������񵽵ȴ��б�
#define STR_SET_DOWNLOADING_PRIORITY "set downloading priority"// �������ȼ�,�������ƣ�-1000�ƶ�������
#define STR_SET_WAITING_PRIORITY    "set waiting priority"  // �������ȼ�,�������ƣ�-1000�ƶ�������
#define STR_SUSPEND_TASKS           "suspend tasks"         // ��ͣ���ڽ��е�����
#define STR_RESUME_TASKS            "resume tasks"          // ������ͣ������
#define STR_REMOVE_DOWNLOADING_TASKS "remove downloading tasks"
#define STR_REMOVE_WAITING_TASKS    "remove waiting tasks"
#define STR_REMOVE_SEEDING_TASKS    "remove seeding tasks"
#define STR_REMOVE_UPLOADING_TASKS  "remove uploading tasks"
#define STR_REMOVE_LOCAL_IDX        "remove local idx"
#define STR_CHECK_TASKS             "check tasks"           // ������ݣ������ִ�����ת�����
#define STR_SET_AUTO_UPDATE_TASKS   "set auto update tasks" // ��������Ϊ�Զ�����(��ȡ���Զ�����)

#define STR_UPLOAD_RESOURCE         "upload resource"       // �ϴ����µ���Դ
#define STR_DELETE_RESOURCE         "delete resource"       // ɾ����������Դ

#define STR_GET_PEER_INFO           "get peer info"         // ��ȡ�ڵ���Ϣ

#define STR_STOP_SERVICE            "stop service"          // ֹͣ����

struct options
{
    CHAR PID[MAX_PID_LEN];          // �û�PID: 32�ֽڵ��ַ���
    CHAR svrAddr[256];              // server�������ṩ�����б������ļ�����/�ϴ����ڵ�����, ��ʽ: 192.168.0.48:8080;
    WORD portNum;                   // �˿ںţ�����TCP��UDP����:26500���˲�������ʹ�������³�ʼ��
    UCHAR updateMode;               // ���ø���ģʽ��0(Ĭ��): ʹ���ļ����壬1: ֱ��д����ϷĿ¼
    WCHAR tmpDir[MAX_PATH];         // ���ڸ��µ���ʱĿ¼�������ڸ����ļ�����ʱ�洢
    UCHAR dirMode;                  // 0:��Ϸ���+��Ϸ����,1:��Ϸ����,0ΪĬ��ֵ
    WCHAR dir[MAX_PATH];            // ��Ϸ���Ŀ¼
    UCHAR userPrvc;                 // �û�������ʡ�ݴ���
    UCHAR userType;                 // 0: ����û�, 1: �����û�, 3: vip
    UCHAR userAttr;                 // �ɲ��ֱ�־��
    UCHAR lineType;                 // ��·����: 0: ����, 1: ��ͨ, ......
    UCHAR lineSpeed;                // ��·�ٶ�: ��λ100K
    UCHAR priorityMode;             // ���ȼ�ģʽ, 0:��, 1:70%,20%..., 2:50%,%25,%15..., 2:40%,%30,%20..., ����������ʱ����Ч
    DWORD downLimit;                // ��������ٶ�(KB/s)
    DWORD upLimit;                  // ����ϴ��ٶ�(KB/s)
    DWORD maxConcurrentTasks;       // ���ͬʱ������
    DWORD minDownloadSpeed;         // ���ڴ��ٶȵĽڵ㽫��ɾ��
    DWORD maxDownPeersPerTask;      // �������������
    DWORD maxUpPeersPerTask;        // ����ϴ�������
    DWORD maxCachesPerTask;         // ÿ��������󻺴�
    DWORD seedMinutes;              // ��������Ժ�����ʱ��
    DWORD diskSpaceReserve;         // ���̱����ռ䣬����ʱ�������ʣ��ռ䣬MB
};

BOOL CmdSocket_GetOptions(SOCKET, struct options *o);
BOOL CmdSocket_SetOptions(SOCKET, struct options *o);

// �б�
BOOL CmdSocket_GetDownloadingTasks(SOCKET);
BOOL CmdSocket_GetWaitingTasks(SOCKET);
BOOL CmdSocket_GetSeedingTasks(SOCKET);
BOOL CmdSocket_GetUploadingTasks(SOCKET);
BOOL CmdSocket_GetAutoUpdateTasks(SOCKET);
BOOL CmdSocket_GetLocalIdxList(SOCKET);
BOOL CmdSocket_GetNetIdxList(SOCKET);

void MsgSocket_OnDownloadingTasks(CHAR *msgData, int msgLen, struct ptrList *keyVals);
void MsgSocket_OnWaitingTasks(CHAR *msgData, int msgLen, struct ptrList *keyVals);
void MsgSocket_OnSeedingTasks(CHAR *msgData, int msgLen, struct ptrList *keyVals);
void MsgSocket_OnUploadingTasks(CHAR *msgData, int msgLen, struct ptrList *keyVals);
void MsgSocket_OnAutoUpdateTasks(CHAR *msgData, int msgLen, struct ptrList *keyVals);
void MsgSocket_OnLocalIdxList(CHAR *msgData, int msgLen, struct ptrList *keyVals);
void MsgSocket_OnNetIdxList(CHAR *msgData, int msgLen, struct ptrList *keyVals);

// �������"�ȴ�����"�б�,����ʼ�����Ƚ���ȴ��б�
BOOL CmdSocket_AddTasks(SOCKET, struct ptrList *);

// ����upDown: �������ơ��������ƣ�С�ڵ���-1000�ö������ڵ���1000�õ�
BOOL CmdSocket_SetDownloadingPriority(SOCKET cmdSock, struct ptrList *ids, int upDown);
BOOL CmdSocket_SetWaitingPriority(SOCKET cmdSock, struct ptrList *ids, int upDown);

// ��ͣ����
BOOL CmdSocket_SuspendTasks(SOCKET, struct ptrList *);
// ������ͣ������
BOOL CmdSocket_ResumeTasks(SOCKET, struct ptrList *);

// ɾ������
BOOL CmdSocket_RemoveDownloadingTasks(SOCKET cmdSock, struct ptrList *ids);
BOOL CmdSocket_RemoveWaitingTasks(SOCKET cmdSock, struct ptrList *ids);
BOOL CmdSocket_RemoveSeedingTasks(SOCKET cmdSock, struct ptrList *ids, BOOL deleteFiles);
BOOL CmdSocket_RemoveUploadingTasks(SOCKET cmdSock, struct ptrList *ids);
BOOL CmdSocket_RemoveLocalIdx(SOCKET cmdSock, struct ptrList *ids);

// ������ݣ�������ִ�����ת�����
BOOL CmdSocket_CheckTasks(SOCKET, struct ptrList *);

// ��������Ϊ�Զ�����(��ȡ���Զ�����)
BOOL CmdSocket_SetAutoUpdateTasks(SOCKET, struct ptrList *, BOOL autoUpdate);

// �ϴ���Դ
BOOL CmdSocket_UploadResource(SOCKET, const CHAR *id, const WCHAR *dir,
                              const WCHAR *cate, const CHAR *pwd,
                              const CHAR *notifyPeers);
// ɾ����������Դ
BOOL CmdSocket_DeleteResource(SOCKET, const CHAR *id, const CHAR *pwd);

// ��ȡ�ڵ���Ϣ
BOOL CmdSocket_GetPeerInfo(SOCKET, const CHAR *id, struct ptrList **peers);

// ֹͣ���غ��ķ���
BOOL CmdSocket_StopService(SOCKET cmdSock);

struct downloading_progress
{
    CHAR id[MAX_ID_LEN];
    CHAR action[16];
    INT64 total, completed;
    int upSpeed, dnSpeed;
    int seederCnt, peerCnt;
};

struct tracker_info
{
    CHAR id[MAX_ID_LEN];
    int seeders, peers;
    int incoming, outgoing;
};

struct peer_info
{
    CHAR pid[MAX_PID_LEN];
    CHAR ipport[32];
    UINT32 piecesHave;
    UINT32 dnSpeed, upSpeed;
    BOOL isOutgoing, isConnected;
};


extern CHAR g_seedListVer[];
extern struct ptrList *g_downloading;
extern struct ptrList *g_waiting;
extern struct ptrList *g_uploading;
extern struct ptrArray g_seeding;
extern struct ptrArray g_autoUpdate;
extern struct ptrArray g_localIdx;
extern struct ptrArray g_netIdx;
extern struct ptrArray g_searchRes;

extern struct ptrArray g_progress;

extern struct ptrArray g_categories;

extern struct ptrArray g_trackerInfo;

extern int g_downloadingCnt;
extern int g_waitingCnt;
extern int g_uploadingCnt;
extern int g_seedingCnt;

extern int g_maxId;

#define TS_ERROR            0x00000001  // ����, ����顱���Կɼ�������
#define TS_PAUSED           0x00000002  // ��ͣ��־
#define TS_UPLOADING        0x00000008  // ���ڴ������ϴ�����
#define TS_CONTINUING       0x00000010  // ���ڼ���(ϵͳ�������������)
#define TS_PREPARING        0x00000020  // ����׼��
#define TS_CHECKING         0x00000040  // ���ڼ��
#define TS_DELETING         0x00000080  // ����ɾ��
#define TS_DOWNLOADING      0x00000100  // ��������
#define TS_UPDATING         0x00000200  // ���ڸ���
#define TS_SEEDING          0x00001000  // �Ѿ���������ػ�����˸��²�ת��, ��������
#define TS_TRANSFERING      0x00002000  // ����ת��(�������Ҫ���²��ֵ�����)


#define STR_NET_IDX_ADDED           "net_idx_added"
#define STR_NET_IDX_DELETED         "net_idx_deleted"

#define STR_LOCAL_IDX_ADDED         "local_idx_added"
#define STR_LOCAL_IDX_DELETED       "local_idx_deleted"

#define STR_UPLOADING_ADDED         "uploading_added"
#define STR_UPLOADING_DELETED       "uploading_deleted"
#define STR_UPLOADING_PROGRESS      "uploading_progress"
#define STR_UPLOADING_ERROR         "uploading_error"

#define STR_SEEDING_ADDED           "seeding_added"
#define STR_SEEDING_DELETED         "seeding_deleted"
#define STR_SEEDING_TIME_OUT        "seeding_time_out"

#define STR_AUTOUPDATE_ADDED        "autoupdate_added"
#define STR_AUTOUPDATE_DELETED      "autoupdate_deleted"

#define STR_WAITING_ADDED           "waiting_added"
#define STR_WAITING_DELETED         "waiting_deleted"

#define STR_DOWNLOADING_PAUSED      "downloading_paused"
#define STR_DOWNLOADING_RESUMED     "downloading_resumed"

#define STR_DOWNLOADING_ADDED       "downloading_added"
#define STR_DOWNLOADING_DELETED     "downloading_deleted"
#define STR_DOWNLOADING_COMPLETED   "downloading_completed"
#define STR_DOWNLOADING_UPDATED     "downloading_updated"
#define STR_DOWNLOADING_PROGRESS    "downloading_progress"
#define STR_DOWNLOADING_CHANGED     "downloading_changed"
#define STR_DOWNLOADING_ERROR       "downloading_error"

#define STR_TRANSFER_BEGIN          "transfer_begin"
#define STR_TRANSFER_ERROR          "transfer_error"

#define STR_TRACKER_INFO            "tracker_info"


#define ERR_SUCCESS         0  // �޴�
#define ERR_IDX             1  // �����ļ����ļ�δ�ҵ����ļ���ʽ���ԡ��ļ���id�����������id��һ��
#define ERR_NEW_IDX         2  // ���������ļ�����
#define ERR_DISK_SPACE      3  // ���̿ռ䲻��
#define ERR_FILE_READ       4  // �ļ���д��
#define ERR_FILE_WRITE      5  // �ļ���д��
#define ERR_FILES           6  // �ļ���, ����ȫ���
#define ERR_TMP_FILE        7  // ��ʱ�ļ���д��
#define ERR_NET_IDX         8  // ���������ļ�����
#define ERR_NET_IDX2        9  // �ϴ������ļ�����
#define ERR_CONTINUE        10 // ��������ʱ���ɵļ�¼�д�

#ifdef __cplusplus
}
#endif

#endif

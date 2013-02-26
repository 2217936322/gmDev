#ifndef _CORE_H
#define _CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <winsock2.h>
#include <windows.h>
#include <Ws2ipdef.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#include "ptrList.h"
#include "ptrArray.h"

#include "tcp.h"
#include "debugf.h"
#include "NtSvc.h"
#include "helper.h"
#include "sha1.h"
#include "getMacIp.h"

#include "base64.h"
#include "bitset.h"
#include "buffer.h"
#include "rateCtrl.h"
#include "speed.h"

#include "idx.h"

#include "zlib.h"

#pragma comment (lib, "wsock32")
#pragma comment (lib, "comctl32")
#pragma comment (lib, "comdlg32")
#pragma comment (lib, "shell32")
#pragma comment (lib, "shlwapi")
#pragma comment (lib, "Iphlpapi")
#pragma comment (lib, "Wininet")

struct options
{
    CHAR myIp[MAX_IP_LEN];          // ����IP��ַ������lsd
    CHAR myMac[20];                 // ����MAC��ַ
    CHAR myId[MAX_PID_LEN];         // �û�PID: 32�ֽڵ��ַ���
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
    UCHAR isDedicatedServer;        // ר�ŵķ�����
};

extern WCHAR g_workDir[MAX_PATH];
extern struct options g_options;

#define DEFAULT_PORT_NUM    18900
#define VERSION             10

// ip:port
struct ip_port
{
    UINT32 ip;
    UINT16 port;
};

// download/upload speed, bytes/s
struct speed
{
    UINT32 down;
    UINT32 up;
};

// �ڵ�״̬
#define PEER_CONNECTED                  0x00000001
#define PEER_HANDSHAKED                 0x00000002
#define PEER_BITFIELDED                 0x00000004

// peer request
struct request
{
    struct file *file;      // file info
    UINT32 piece;           // piece index in file
    UINT32 offset;          // offset in piece
    UINT32 length;          // request length
    time_t requestTime;     // tick count
    UCHAR *data;
    UINT32 dataLen;
};

#define MAX_CACHE_IDLE  30
// piece cahce
struct piece
{
    struct file *file;
    UINT32 piece;           // index in file
    UCHAR *data;
    int dataLen;
    time_t lastRefTime;
};

// peer
struct peer
{
    CHAR pid[MAX_PID_LEN];
    CHAR ip[MAX_IP_LEN];
    UINT16 port;
    UINT32 downLimit;
    UINT32 upLimit;
    UCHAR userPrvc;
    UCHAR userType;
    UCHAR userAttr;
    UCHAR lineType;
    UCHAR lineSpeed;

    struct task *task;
    int sock;
    int isOutgoing;
    UINT32 status;
    time_t connectTime;
    time_t lastReceiveTime;

    struct bitset bitset;
    UINT32 havePieces;
    BOOL amChokingToPeer;
    BOOL amInterestingToPeer;
    BOOL isPeerChokingToMe;
    BOOL isPeerInterestingToMe;

    // ������������һ���ļ�
    struct file *downloadingFile;
    UINT32 downloadingPiece;

    struct rate_control rcDown;
    struct rate_control rcUp;
    struct speed_data speedDown;
    struct speed_data speedUp;

    UINT32 recommendedPieces[10];
    struct ptrList *requests;
    struct ptrList *peerRequests;
    struct ptrList *readCache;

    CHAR szDesc[128];
};

// tcp custom events
#define TCP_CUSTOM_TASK_IO_READ     1
#define TCP_CUSTOM_TASK_IO_WRITE    2

#define TCP_CUSTOM_TASK_UPLOAD      3
#define TCP_CUSTOM_TASK_CONTINUE    4
#define TCP_CUSTOM_TASK_PREPARE     5
#define TCP_CUSTOM_TASK_CHECK       6
#define TCP_CUSTOM_TASK_DELETE      7
#define TCP_CUSTOM_TASK_TRANSFER    8

#define TCP_CUSTOM_TASK_PEERS       9
#define TCP_CUSTOM_IDX_LIST         10

#define TCP_CUSTOM_LOCAL_PEER       11

struct tcp_custom
{
    INT ioType;
};
struct tcp_custom_task_io
{
    INT ioType;
    int iThread, bOperated;
    CHAR taskHash[MAX_HASH_LEN];
    CHAR peerId[MAX_PID_LEN];
    struct task *task;
    struct peer *peer;
    struct file *file;
    UINT32 piece;
    DWORD pieceLen;
    UCHAR *pieceData;
    BOOL success;
};
struct tcp_custom_task_upload
{
    INT ioType;
    struct task *task;
    CHAR hashOld[MAX_HASH_LEN];
    BOOL success;
};
struct tcp_custom_task_continue
{
    INT ioType;
    struct task *task;
    BOOL success;
};
struct tcp_custom_task_prepare
{
    INT ioType;
    struct task *task;
    CHAR hashOld[MAX_HASH_LEN];
    BOOL success;
};
struct tcp_custom_task_check
{
    INT ioType;
    struct task *task;
    CHAR hashOld[MAX_HASH_LEN];
    BOOL success;
};
struct tcp_custom_task_delete
{
    INT ioType;
    struct task *task;
    BOOL success;
    BOOL deleteFiles;
};
struct tcp_custom_task_transfer
{
    INT ioType;
    struct task *task;
    BOOL success;
};
struct tcp_custom_task_peers
{
    INT ioType;
    CHAR *data;
    INT dataLen;
};
struct tcp_custom_idx_list
{
    INT ioType;
    struct ptrArray netIdxList1, netIdxList2;
};
struct tcp_custom_local_peer
{
    INT ioType;
    CHAR method[16];
    CHAR hash[MAX_HASH_LEN];
    CHAR pid[MAX_HASH_LEN];
    CHAR ip[MAX_IP_LEN];
    WORD port;
};


// Task Status
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

#define TS_STOP_DOWNLOAD    0x000030FF
#define TS_STOP_UPLOAD      0x000020FF

// task error codes
#define ERR_SUCCESS         0  // �޴�
#define ERR_IDX             1  // �����ļ����ļ�δ�ҵ����ļ���ʽ���ԡ��ļ���id�����������id��һ��
#define ERR_NEW_IDX         2  // ���������ļ�����
#define ERR_DISK_SPACE      3  // ���̿ռ䲻��
#define ERR_FILE_READ       4  // �ļ���д��
#define ERR_FILE_WRITE      5  // �ļ���д��
#define ERR_FILES           6  // �ļ���, ����ȫ���
#define ERR_TMP_FILE        7  // ��ʱ�ļ���д��
#define ERR_NET_IDX         8  // �ӷ��������������ļ�����
#define ERR_NET_IDX2        9  // ��������ϴ������ļ�����
#define ERR_CONTINUE        10 // ��������ʱ���ɵļ�¼�д�

#define MAX_PENDING_READ    512         //
#define MAX_PENDING_WRITE   1024        //

struct task_aux
{
    UINT32 status;
    UINT32 total;
    UINT32 completed;
    HANDLE stopEvent;
    HANDLE stoppedEvent;
    time_t lastActionTime;
};

struct file_tmp
{
    UINT64 bytesDownloaded;
    UINT32 piecesDownloaded;
};
struct task_tmp
{
    WCHAR fileName[MAX_PATH];
    HANDLE hFile;
    UINT64 bytesDownloaded;
    UINT32 piecesDownloaded;
    UINT32 *pieceTable;
    struct bitset bitset;
    struct file_tmp *files;
};

struct task
{
    struct idx idx;

    WCHAR dir[MAX_PATH];
    struct bitset bitset;
    UINT32 action;
    UINT64 bytesDownloaded;
    UINT32 piecesDownloaded;
    UINT64 bytesToDownload;
    UINT32 piecesToDownload;
    UINT64 bytesUploaded;
    UINT32 downLimit;
    UINT32 upLimit;

    HANDLE hFileStat;

    struct bitset bitsetRequested;

    struct rate_control rcRequest;
    struct rate_control rcDown;
    struct rate_control rcUp;

    // ����ʱ�����ص���ʱ�ļ���ȫ����ɺ��д
    struct task_tmp tmp;

    int totalPeers, totalSeeders;
    time_t lastGetPeersTime, lastLocalSearchTime;
    struct ptrList *peersCandidate;
    struct ptrList *peersOutgoing;
    struct ptrList *peersIncoming;
    int unchokedPeerCount;
    time_t startSeedingTime;
    time_t lastSeedingTime;

    // read cache
    struct ptrList *readCache;

    // upload progress and status
    struct task_aux upload;
    CHAR uploadPwd[MAX_PWD_LEN];
    CHAR *uploadNotifyPeers;

    // continue progress and status
    struct task_aux cont;

    // prepare progress and status
    struct task_aux prepare;

    // check progress and status
    struct task_aux check;

    // transfer
    struct task_aux transfer;

    // OnTcpTimer
    time_t lastSentProgressTime;
    time_t lastCheckIdleFileTime;
    time_t lastCheckIdleCacheTime;
    time_t lastCheckPeersTime;
    time_t lastCheckPeerSpeedTime;
    time_t lastCheckTransferTime;

    UINT32 errorCode;
};

extern struct ptrArray g_netIdxSortedById;
extern struct ptrArray g_netIdxSortedByHash;

extern struct ptrArray g_localIdxSortedById;
extern struct ptrArray g_localIdxSortedByHash;

extern struct ptrArray g_tasksAutoUpdate;
extern struct ptrArray g_tasksSeedingSI;
extern struct ptrArray g_tasksSeedingSH;
extern struct ptrList *g_tasksDownloading;
extern struct ptrList *g_tasksUploading;
extern struct ptrList *g_tasksWaiting;

CHAR *task_isWaiting(const CHAR *id);
CHAR *task_isAutoUpdate(const CHAR *id);
struct task *task_isSeeding(const CHAR *id);
struct task *task_isDownloading(const CHAR *id);
struct task *task_isUploading(const CHAR *id);
struct task *task_findHash(const CHAR *hash);

void task_remove(struct task *task);
BOOL task_removeAutoUpdate(const CHAR *id);
BOOL task_removeWaiting(const CHAR *id);
BOOL task_removeUploading(const CHAR *id);
BOOL task_removeDownloading(const CHAR *id);
BOOL task_removeSeeding(const CHAR *id);
BOOL task_removeLocalIdx(const CHAR *id, BOOL deleteDir);
BOOL task_removeWorking(const CHAR *id, BOOL deleteDir);

extern int g_listenSock;

extern struct rate_control g_rcRequest;
extern struct rate_control g_rcDown;
extern struct rate_control g_rcUp;

extern int g_adminSocket;
extern CRITICAL_SECTION g_csAdminMsg;
extern struct ptrArray g_adminSockets;


void SetOptionsDefault();
void SetOptions(struct options *);
void ReadOptions();
BOOL SaveOptions();


void svr_restart();
void svr_cleanup();
BOOL svr_startup();

// task
struct task *task_new();
void task_delete(struct task *task);
void task_onPieceRead(struct tcp_custom_task_io *);
void task_onPieceWritten(struct tcp_custom_task_io *);
void task_setError(struct task *task, UINT32 errCode);
BOOL task_getDestDir(const CHAR *id, const WCHAR *name, WCHAR *destDir);
UINT32 task_getPieceLength(struct task *task, struct file *fi, UINT32 piece);
struct file *task_getFileInfo(struct task *task, UINT32 piece);
BOOL task_openFile(struct task *task, struct file *file, BOOL forceWrite, DWORD *errCode);
void task_closeFile(struct task *task, struct file *file);
void task_closeAllFiles(struct task *task);

BOOL task_allocTmpFile(struct task *task);
void task_resetTmpData(struct task *task);
void task_discardTmpData(struct task *task);
BOOL task_openTmpFile(struct task *task);

BOOL task_createDirectories(struct task *task);
BOOL task_allocFiles(struct task *task, DWORD *errCode);
void task_copySeedFile(struct task *task);
void task_correcrAllFileTime(struct task *task);

void task_getSpeed(struct task *task, struct speed *speed);

void task_checkConnectablePeers(struct task *task, time_t currTime);
void task_checkCandidatePeers(struct task *task, time_t currTime);
void task_checkIncomingPeers(struct task *task, time_t currTime);

void task_closeAllPeers(struct task *task);

BOOL task_setPieceData(struct task *task, struct file *file, UINT32 piece, const UCHAR *data, int dataLen);
struct piece *task_getPieceData(struct task *task, struct file *file, UINT32 piece, struct peer *peer);
void task_addReadCache(struct task *task, struct file *file, UINT32 piece, UCHAR *data, int dataLen);

void task_cancelAllIo(struct task *task);


BOOL taskIo_startup();
void taskIo_cleanup();


void task_arrangePriorities();


BOOL taskContinue_begin(struct task *task);
void taskContinue_end(struct task *task);

BOOL taskPrepare_begin(struct task *task);
void taskPrepare_end(struct task *task);

BOOL taskCheck_begin(struct task *task);
void taskCheck_end(struct task *task);

BOOL taskTransfer_begin(struct task *task);
void taskTransfer_end(struct task *task);

BOOL taskUpload_begin(struct task *task);
void taskUpload_end(struct task *task);

void task_onTimer(struct task *task, time_t currTime);
void task_checkServer(struct task *task, BOOL force, BOOL exiting, time_t currTime);


void admin_onNeedSeed(CHAR *hash);
void admin_onNeedUpdate(CHAR *id);
BOOL admin_processCmd(int sockIdx, const UCHAR *data, int dataLen);
void admin_sendMsg(const CHAR *cmd, const CHAR *id);
void admin_sendAllLists();
void admin_sendDownloadingProgress(struct task *task);
void admin_sendUploadingProgress(struct task *task, const CHAR *action, int done, int total);
void admin_sendCheckingProgress(struct task *task, int done, int total);
void admin_sendTrackerInfo(struct task *task);

// tak manager
BOOL task_startup();
void task_cleanup();


// ɾ����ԴĿ¼�����ļ�
BOOL deleteDir_begin(const WCHAR *dir);
void deleteDir_end(const WCHAR *dir);
BOOL deleteDir_startup();
void deleteDir_cleanup();

// piece
struct piece *piece_new(struct file *file, UINT32 piece, UCHAR *data, int dataLen);
struct piece *piece_dup(struct piece *pc);
void piece_free(struct piece *pc);


// peer
struct peer *peer_new();
void peer_delete(struct peer *peer);
BOOL peer_connect(struct peer *peer);
void peer_sendUnchoke(struct peer *peer);
void peer_sendAlive(struct peer *peer);
void peer_sendRecommend(struct peer *peer, UINT32 piece);
void peer_sendLimitSpeed(struct peer *peer);
void peer_doSendPiece(struct peer *peer);
void peer_doPieceRequest(struct peer *peer);
void peer_onCloseCancelRequests(struct peer *peer);
void peer_tryDownloadPiece(struct peer *peer, struct file *file, UINT32 pieces);
void peer_sendHaveToPeers(struct task *task, UINT32 piece);
void peer_getSpeed(struct peer *peer, struct speed *speed);

struct piece *peer_addReadCache(struct peer *peer, struct file *file,
                                UINT32 piece, UCHAR *data, int dataLen);
void peer_releaseReadCache(struct peer *peer, struct piece *pc);

// peer tcp
int CALLBACK peer_OnTcpEvent(int sockIdx, int msgCode, UCHAR *msgData, int msgLen);
void task_OnTcpTimer(); // 100ms timer of tcp thread
void task_OnTcpCustom(struct tcp_custom *); // custom event

// ----------------------------------------------------------------------------------------
// server: idxList, idx:get/add/set/del, peers
// ----------------------------------------------------------------------------------------
extern BOOL g_idxListInitialized;

extern int g_svr;
extern int g_svrInteval;
extern UCHAR g_amConnectable;

BOOL svr_startup();
void svr_cleanup();
void svr_reset();
void svr_sendExitRequest(int iSvr);
void svr_sendPeersRequest(int iSvr, struct task *task, BOOL exiting, int peersWant);
void svr_onPeers(UCHAR *msgData, int msgLen);
void svr_sendIdxListRequest(int iSvr, time_t currTime);
void svr_onIdxList(struct ptrArray *, struct ptrArray *);

BOOL svr_GetIdx(int iSvr, const CHAR *id);
BOOL svr_SetIdx(int iSvr, const CHAR *id, const CHAR *pwd);
BOOL svr_DelIdx(int iSvr, const CHAR *id, const CHAR *pwd);


BOOL lsd_startup();
void lsd_cleanup();
BOOL lsd_sendAnnounce(struct task *task, const CHAR *method);
void lsd_onAnnounce(struct tcp_custom_local_peer *tcpCustom);

// ----------------------------------------------------------------------------------------
// files: AutoUpdate,Completed,Working, Task Status,Bitset,FileTime
// ----------------------------------------------------------------------------------------
// AutoUpdate.txt
BOOL ReadAutoUpdateTaskList();
BOOL SaveAutoUpdateTaskList();

// Waiting.txt
BOOL ReadWaitingTaskList();
BOOL SaveWaitingTaskList();

// Waiting.txt
BOOL ReadSeedingTaskList();
BOOL SaveSeedingTaskList();

// LocalIdx.txt
BOOL ReadLocalIdxList();
BOOL SaveLocalIdxList();
struct idx_local *AddLocalIdx(struct idx *idx, const WCHAR *dir);
BOOL RemoveLocalIdx(const CHAR *id);

// Downloading.txt
BOOL ReadDownloadingTaskList();
BOOL SaveDownloadingTaskList();

// Uploading.txt
BOOL ReadUploadingTaskList();
BOOL SaveUploadingTaskList();

// task.working.status
BOOL task_readStatus(struct task *task);
BOOL task_saveStatus(struct task *task);
void task_delStatus(struct task *task);

// task.working.bits
BOOL task_readBitset(struct task *task);
BOOL task_saveBitset(struct task *task);
void task_delBitset(struct task *task);

// task.working.filetime(stop/continue)
BOOL task_readFileTime(struct task *task, UINT32 *timeCount, UINT64 **times);
BOOL task_saveFileTime(struct task *task);
void task_delFileTime(struct task *task);


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

#ifdef __cplusplus
}
#endif

#endif


#ifndef _IDX_FILE_H
#define _IDX_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <winsock2.h>
#include <windows.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ptrList.h"
#include "helper.h"
#include "base64.h"
#include "sha1.h"
#include "arc4.h"
#include "zlib.h"

#define IDX_EXTNAME                     L".gmIdx"

    // cmd
#define CMD_HANDSHAKE                   10
#define CMD_BITFIELD                    11
#define CMD_CHOKE                       12
#define CMD_UNCHOKE                     13
#define CMD_INTERESTED                  14
#define CMD_NOTINTERESTED               15
#define CMD_HAVE                        16  // �����������ܿ�������
#define CMD_REQUEST                     17
#define CMD_PIECE                       18
#define CMD_CANCEL                      19
#define CMD_RECOMMEND                   20
#define CMD_LIMITSPEED                  21
#define CMD_PING                        22

#define CMD_ADMIN                       30

#define GM_GET_IDX_LIST                 1   // �����ļ��б�
#define GM_GET_IDX                      2   // �����ļ�
#define GM_SET_IDX                      3   // �޸��ļ�
#define GM_DEL_IDX                      4   // ɾ���ļ�
#define GM_GET_PEERS                    5   // ����Դ
#define GM_NEED_SEED                    6   // ��Ҫ���֣����������ͻ���
#define GM_NEED_UPDATE                  7   // ��Ҫ���֣����������ͻ���
#define GM_GET_IDX_LIST_RESP            101
#define GM_GET_IDX_RESP                 102
#define GM_SET_IDX_RESP                 103
#define GM_DEL_IDX_RESP                 104
#define GM_GET_PEERS_RESP               105
#define GM_NEED_SEED_RESP               106
#define GM_NEED_UPDATE_RESP             107

#define GM_GET_SVR_OPTIONS              81
#define GM_SET_SVR_OPTIONS              82
#define GM_GET_SVR_STATUS               83
#define GM_GET_SVR_PEERS                84
#define GM_GET_SVR_IDX_LIST             85
#define GM_REG_SVR_SOCK                 86
#define GM_SVR_IDX_CHANGED              87
#define GM_GET_SVR_OPTIONS_RESP         181
#define GM_SET_SVR_OPTIONS_RESP         182
#define GM_GET_SVR_STATUS_RESP          183
#define GM_GET_SVR_PEERS_RESP           184
#define GM_GET_SVR_IDX_LIST_RESP        185
#define GM_REG_SVR_SOCK_RESP            186
#define GM_SVR_IDX_CHANGED_RESP         187

#define MAX_VERSION_LEN                 24
#define MAX_PID_LEN                     30
#define MAX_IP_LEN                      20
#define MIN_ID_LEN                      4
#define MAX_ID_LEN                      12
#define MAX_HASH_LEN                    30
#define MAX_PWD_LEN                     30
#define MAX_CATEGORY_LEN                16
#define MAX_NAME_LEN                    128
#define MAX_EXTRA_LEN                   512

#define MAX_IDXFILE_SIZE                (10*1024*1024)

// �ص�����, SetEvent(cs->hEventStop); ������ֹ����
typedef void (CALLBACK IDXCREATECB)(struct create_idx *cs);

struct create_idx
{
    CHAR id[MAX_ID_LEN];                // ����ID���������ļ���һ�£��63�ֽ�
    WCHAR dir[MAX_PATH];                // �ļ�·������Ŀ¼������Ŀ¼�µ������ļ�����������ȥ
    WCHAR category[MAX_CATEGORY_LEN];   // ���
    DWORD pieceLength;                  // ���С��Ӧ����256*1024��512*1024��1024*1024����ֵ�����ֽ�Ϊ��λ��0��ʾ�Զ�ƥ��
    time_t creationDate;                // 1970��1��1����������������0����Ĭ�ϵ�ǰʱ��
    CHAR extraInfo[MAX_EXTRA_LEN];      // ������Ϣ

    IDXCREATECB *createCB;              // �ص�����
    void *cbParam;                      // �ص������Ĳ���
    HANDLE hEventStop;                  // ֹͣ����

    DWORD status;                       // 1:����ɨ��Ŀ¼��2:���ڼ���sha1��3:�Ѿ����
    DWORD totalPieces;                  // ���ڴ���ܿ���, MAXDWORD��ʾ���ڳ�ʼ��(�����ļ�)
    DWORD completedPieces;              // ���ڴ���Ѿ���ɿ���
};

// ���������ļ���TRUE��ʾ�ɹ�
// idxFileCreateCB����FALSEʱ���жϲ���
// �����ļ��������浽��sourceDir\ID.IDXFILE_EXTNAME
int createIdx(struct create_idx *);
#define ERR_CREATE_IDX_SUCCESS          0
#define ERR_CREATE_IDX_USER_BREAK       1
#define ERR_CREATE_IDX_PARAM            2
#define ERR_CREATE_IDX_WRITE            3
#define ERR_CREATE_IDX_READ             4

// -------------------------------------------------------------------------------------------------
struct idx
{
    CHAR id[MAX_ID_LEN];                // ����ID�������ļ���xxxxxx.idx��xxxxxxΪID����������ĸ������
    CHAR hash[MAX_HASH_LEN];            // �����ӵ�hash��20�ֽڵ�base64
    WCHAR name[MAX_NAME_LEN];           // ����
    WCHAR category[MAX_CATEGORY_LEN];   // ���
    UINT64 creationDate;                // ���Ӵ���ʱ��
    UINT32 pieceLength;                 // ���С���ֽ�
    UINT32 pieceCount;                  // �����ļ��ܿ���
    UINT64 bytes;                       // �����ļ��ܳ��ȣ��ֽ�
    UINT32 fileCount;                   // �ļ���
    struct file **files;                // �ļ���Ϣ
    UINT32 directoryCount;              // Ŀ¼��
    WCHAR **directories;                // Ŀ¼��Ϣ
    CHAR extraInfo[MAX_EXTRA_LEN];      // ������Ϣ,�ɴ洢������Ϣ
};

struct file
{
    WCHAR fileName[MAX_PATH];           // �ļ���
    UINT64 bytes;                       // �ļ����ȣ��ֽ�
    UINT32 pieceOffset;                 // ��ʼ��
    UINT32 pieceCount;                  // �ļ������Ŀ���
    UINT32 fileAttr;                    // �ļ�����: 0x0001-ֻ����0x0002-ϵͳ��0x0004-����
    UINT64 fileTime;                    // �ļ�������޸�ʱ��
    UCHAR *hash;                        // �ļ�hash������Ϊÿ��20�ֽ�

    INT idxInFiles;                     // �������ļ��б��е����
    HANDLE hFile;                       // �ļ����
    UINT64 bytesDownloaded;             // �Ѿ������ֽ���
    UINT32 piecesDownloaded;            // �Ѿ����ؿ���
    UINT32 accessMode;                  // GENERIC_READ/GENERIC_WRITE
    time_t lastAccessTime;              // ������ʱ��
};

//----------------------------------------------------------------------------------------
BOOL idx_save(WCHAR *fileName, UCHAR *data, int dataLen);

/* ���������ļ�(������ȡ�����ļ���Ϣ���������ϴ������޹�)
  ����:
fileName: �ļ���
si: ָ��struct idx_file_info�ṹ��ָ��
  ��ע:
���������л����һЩ�ڴ棬���ʹ����Ϻ���Ҫ����IdxFile_freeInfo���ͷ��ڴ� */
BOOL idx_open(const WCHAR *idxFileName, struct idx *); // from file
BOOL idx_load(UCHAR *fileData, int fileDataLen, struct idx *); // from memory

/* ��ɷ��������ļ�
����:
ti: ָ��struct idx_file_info�ṹ��ָ�� */
void idx_free(struct idx *si);

//----------------------------------------------------------------------------------------
// ���Ŀ¼���ļ��Ĵ�С��ʱ���Ƿ��������ļ����
BOOL idx_checkFilesTimeAndSize(struct idx *idx, const WCHAR *dir);

//----------------------------------------------------------------------------------------
// ɾ��������ļ���Ŀ¼
void idx_cleanDirectory(struct idx *idx, WCHAR *dir);

struct idx_net
{
    CHAR id[MAX_ID_LEN];
    CHAR hash[MAX_HASH_LEN];
    WCHAR name[MAX_NAME_LEN];
    WCHAR category[MAX_CATEGORY_LEN];
    CHAR extraInfo[MAX_EXTRA_LEN];
    UINT32 pieceLen, pieceCnt;
    INT64 size;
    time_t lastUpdateTime;
};

struct idx_local
{
    CHAR id[MAX_ID_LEN];
    CHAR hash[MAX_HASH_LEN];
    WCHAR name[MAX_NAME_LEN];
    WCHAR category[MAX_CATEGORY_LEN];
    CHAR extraInfo[MAX_EXTRA_LEN];
    INT64 size;
    time_t completeTime;
    WCHAR dir[MAX_PATH];
};

struct idx_downloading
{
    CHAR id[MAX_ID_LEN];
    CHAR hash[MAX_HASH_LEN];
    WCHAR dir[MAX_PATH];
    UINT32 action;
    UINT32 err;
};

static int IdxToUtf8String(struct idx *pIdx, CHAR *buf, int bufSize)
{
    CHAR name[256], category[64];

    return sprintf_s(buf, bufSize,
        "%s\t"
        "%s\t"
        "%s\t"
        "%s\t"
        "%s\t"
        "%u\t"
        "%u\t"
        "%I64d\t"
        "%I64d\r\n",
        pIdx->id,
        pIdx->hash,
        UnicodeToUtf8(pIdx->name, name, 256),
        UnicodeToUtf8(pIdx->category, category, 64),
        pIdx->extraInfo,
        pIdx->pieceLength,
        pIdx->pieceCount,
        pIdx->bytes,
        pIdx->creationDate);
}

static int IdxNetToUtf8String(struct idx_net *pIdxn, CHAR *buf, int bufSize)
{
    CHAR name[256], category[64];

    return sprintf_s(buf, bufSize,
        "%s\t"
        "%s\t"
        "%s\t"
        "%s\t"
        "%s\t"
        "%u\t"
        "%u\t"
        "%I64d\t"
        "%I64d\r\n",
        pIdxn->id,
        pIdxn->hash,
        UnicodeToUtf8(pIdxn->name, name, 256),
        UnicodeToUtf8(pIdxn->category, category, 64),
        pIdxn->extraInfo,
        pIdxn->pieceLen,
        pIdxn->pieceCnt,
        pIdxn->size,
        pIdxn->lastUpdateTime);
}

static BOOL IdxNetFromUtf8String(struct idx_net *idxn, CHAR *id)
{
    CHAR *hash, *name, *cate, *extra, *pieceLen, *pieceCnt, *size, *lastUpd, *pCrlf;

    pCrlf = strstr(id, "\r\n"); if (pCrlf) *pCrlf = 0;
    hash = strchr(id, '\t'); if (!hash) return FALSE; *hash = 0; hash ++;
    name = strchr(hash, '\t'); if (!name) return FALSE; *name = 0; name ++;
    cate = strchr(name, '\t'); if (!cate) return FALSE; *cate = 0; cate ++;
    extra = strchr(cate, '\t'); if (!extra) return FALSE; *extra = 0; extra ++;
    pieceLen = strchr(extra, '\t'); if (!pieceLen) return FALSE; *pieceLen = 0; pieceLen ++;
    pieceCnt = strchr(pieceLen, '\t'); if (!pieceCnt) return FALSE; *pieceCnt = 0; pieceCnt ++;
    size = strchr(pieceCnt, '\t'); if (!size) return FALSE; *size = 0; size ++;
    lastUpd = strchr(size, '\t'); if (!lastUpd) return FALSE; *lastUpd = 0; lastUpd ++;

    strcpy_s(idxn->id ,MAX_ID_LEN, id);
    strcpy_s(idxn->hash, MAX_HASH_LEN, hash);
    Utf8ToUnicode(name, idxn->name, MAX_NAME_LEN);
    Utf8ToUnicode(cate, idxn->category, MAX_CATEGORY_LEN);
    strcpy_s(idxn->extraInfo, MAX_EXTRA_LEN, extra);
    idxn->pieceLen = (UINT32)atoi(pieceLen);
    idxn->pieceCnt = (UINT32)atoi(pieceCnt);
    idxn->size = (INT64)_atoi64(size);
    idxn->lastUpdateTime = (time_t)_atoi64(lastUpd);

    return TRUE;
}

static int IdxLocalToUtf8String(struct idx_local *pIdxl, CHAR *buf, int bufSize)
{
    CHAR name[256], category[64], szDir[384];

    return sprintf_s(buf, bufSize,
        "%s\t"
        "%s\t"
        "%s\t"
        "%s\t"
        "%s\t"
        "%I64d\t"
        "%I64d\t"
        "%s\r\n",
        pIdxl->id,
        pIdxl->hash,
        UnicodeToUtf8(pIdxl->name, name, 256),
        UnicodeToUtf8(pIdxl->category, category, 64),
        pIdxl->extraInfo,
        pIdxl->size,
        pIdxl->completeTime,
        UnicodeToUtf8(pIdxl->dir, szDir, 384));
}

static int IdxLocalToMbcsString(struct idx_local *pIdxl, CHAR *buf, int bufSize)
{
    CHAR name[256], category[64], szDir[384];

    return sprintf_s(buf, bufSize,
        "%s\t"
        "%s\t"
        "%s\t"
        "%s\t"
        "%s\t"
        "%I64d\t"
        "%I64d\t"
        "%s\r\n",
        pIdxl->id,
        pIdxl->hash,
        UnicodeToMbcs(pIdxl->name, name, 256),
        UnicodeToMbcs(pIdxl->category, category, 64),
        pIdxl->extraInfo,
        pIdxl->size,
        pIdxl->completeTime,
        UnicodeToMbcs(pIdxl->dir, szDir, 384));
}

static BOOL IdxLocalFromUtf8String(struct idx_local *idxl, CHAR *pId)
{
    CHAR *pHash, *pCate, *pName, *pExtra, *pSize, *pTime, *pDir, *pCrlf;

    pCrlf = strstr(pId, "\r\n"); if (pCrlf) *pCrlf = 0;
    pHash = strchr(pId, '\t'); if (!pHash) return FALSE; *pHash = 0; pHash ++;
    pName = strchr(pHash, '\t'); if (!pName) return FALSE; *pName = 0; pName ++;
    pCate = strchr(pName, '\t'); if (!pCate) return FALSE; *pCate = 0; pCate ++;
    pExtra = strchr(pCate, '\t'); if (!pExtra) return FALSE; *pExtra = 0; pExtra ++;
    pSize = strchr(pExtra, '\t'); if (!pSize) return FALSE; *pSize = 0; pSize ++;
    pTime = strchr(pSize, '\t'); if (!pTime) return FALSE; *pTime = 0; pTime ++;
    pDir = strchr(pTime, '\t'); if (!pDir) return FALSE; *pDir = 0; pDir ++;

    memset(idxl, 0, sizeof(struct idx_local));
    strcpy_s(idxl->id, MAX_ID_LEN, pId);
    strcpy_s(idxl->hash, MAX_HASH_LEN, pHash);
    Utf8ToUnicode(pName, idxl->name, MAX_NAME_LEN);
    Utf8ToUnicode(pCate, idxl->category, MAX_CATEGORY_LEN);
    strcpy_s(idxl->extraInfo, MAX_EXTRA_LEN, pExtra);
    idxl->size = (UINT64)_atoi64(pSize);
    idxl->completeTime = (UINT64)_atoi64(pTime);
    Utf8ToUnicode(pDir, idxl->dir, MAX_PATH);

    return TRUE;
}

static BOOL IdxLocalFromMbcsString(struct idx_local *idxl, CHAR *pId)
{
    CHAR *pHash, *pCate, *pName, *pExtra, *pSize, *pTime, *pDir, *pCrlf;
    WCHAR wszCate[MAX_CATEGORY_LEN], wszName[MAX_NAME_LEN], wszDir[MAX_PATH];

    pCrlf = strstr(pId, "\r\n"); if (pCrlf) *pCrlf = 0;
    pHash = strchr(pId, '\t'); if (!pHash) return FALSE; *pHash = 0; pHash ++;
    pName = strchr(pHash, '\t'); if (!pName) return FALSE; *pName = 0; pName ++;
    pCate = strchr(pName, '\t'); if (!pCate) return FALSE; *pCate = 0; pCate ++;
    pExtra = strchr(pCate, '\t'); if (!pExtra) return FALSE; *pExtra = 0; pExtra ++;
    pSize = strchr(pExtra, '\t'); if (!pSize) return FALSE; *pSize = 0; pSize ++;
    pTime = strchr(pSize, '\t'); if (!pTime) return FALSE; *pTime = 0; pTime ++;
    pDir = strchr(pTime, '\t'); if (!pDir) return FALSE; *pDir = 0; pDir ++;

    MbcsToUnicode(pName, wszName, MAX_NAME_LEN);
    MbcsToUnicode(pCate, wszCate, MAX_CATEGORY_LEN);
    MbcsToUnicode(pDir, wszDir, MAX_PATH);

    if (strlen(pId)>=MAX_ID_LEN ||
        strlen(pHash)>=MAX_HASH_LEN ||
        wcslen(wszName)>=MAX_NAME_LEN ||
        wcslen(wszCate)>=MAX_CATEGORY_LEN ||
        strlen(pExtra)>=MAX_EXTRA_LEN ||
        wcslen(wszDir)>=MAX_PATH)
        return FALSE;

    memset(idxl, 0, sizeof(struct idx_local));
    strcpy_s(idxl->id, MAX_ID_LEN, pId);
    strcpy_s(idxl->hash, MAX_HASH_LEN, pHash);
    wcscpy_s(idxl->name, MAX_NAME_LEN, wszName);
    wcscpy_s(idxl->category, MAX_CATEGORY_LEN, wszCate);
    strcpy_s(idxl->extraInfo, MAX_EXTRA_LEN, pExtra);
    idxl->size = (UINT64)_atoi64(pSize);
    idxl->completeTime = (UINT64)_atoi64(pTime);
    wcscpy_s(idxl->dir, MAX_PATH, wszDir);

    return TRUE;
}

#ifdef __cplusplus
}
#endif

#endif

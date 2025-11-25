#ifndef _GOODIX_LOG_H_
#define _GOODIX_LOG_H_
#include "goodix_type.h"

typedef int32_t(*CallbackPrintfUser)(const char *, ...);
typedef int32_t(*CallbackSnprintfUser)(char *str, size_t size, const char *format, ...);
typedef void(*Callback_GH3X2X_Log)(GCHAR *log_string);

DRVDLL_API void goodix_set_sprintFunc(CallbackPrintfUser g_pPrintfUserIn, CallbackSnprintfUser g_pSnprintfUserIn, Callback_GH3X2X_Log GH3X2X_Log_In);

//log print
//extern CallbackPrintfUser g_pPrintfUser;
//extern CallbackSnprintfUser g_pSnprintfUser;
//extern Callback_GH3X2X_Log GH3X2X_PlatformLog;

///// log support len
//#define GH3X2X_LOG_DEBUG_SUP_LEN     (128)

#if 0
#define GH3X2X_INFO_LOG_PARAM(...) do {\
if(g_pSnprintfUser)\
{\
GCHAR chDebugStr[GH3X2X_LOG_DEBUG_SUP_LEN]={0};\
g_pSnprintfUser(chDebugStr, GH3X2X_LOG_DEBUG_SUP_LEN, \
"alg:"__VA_ARGS__);\
GH3X2X_PlatformLog(chDebugStr);\
}\
if(g_pPrintfUser)g_pPrintfUser("alg:"__VA_ARGS__);\
} while (0)//bp log
#endif

// need to init goodix_set_sprintFunc() first
/* LOG example:
#if DEBUG_LOG
#define LOG_PRINT	 GH3X2X_INFO_LOG_PARAM
#else
#define LOG_PRINT
#endif //#if DEBUG_LOG
*/

#endif // !_GOODIX_LOG_H_

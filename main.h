#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <commctrl.h>

#define MSGERROR(_s_) MessageBox(NULL,_s_,"Error", MB_ICONERROR|MB_SYSTEMMODAL|MB_OK)
#define MSGINFO(_s_)  MessageBox(NULL,_s_,"Info", MB_ICONINFORMATION|MB_SYSTEMMODAL|MB_OK)


static RECT MainRect   = {0, 0, 547, 347};  //main window size
static RECT LoggerRect = {4, 48, 530, 272}; 


HINSTANCE ins;
HWND hLogger, hStatusBar;


typedef struct INDEX_
{
   UCHAR *index;
}INDEX;



DWORD CountTheLines(UCHAR *buffer,DWORD len);
INDEX *CreateTheIndexes(UCHAR *buffer,DWORD len,DWORD numElem);
UCHAR *ReadTheFile(UCHAR *name,DWORD *length);

typedef BOOL (* RUNFUNC)(UCHAR*, UCHAR*, DWORD);
BOOL findNoCaseSensitive(UCHAR *data,UCHAR *str,DWORD len);
BOOL findCaseSensitive(UCHAR *data,UCHAR *str,DWORD len);
BOOL find(UCHAR *data,UCHAR *str,DWORD len,RUNFUNC run);


#include "main.h"

char szClassName[ ] = "WindowsSearchApp";

#define SMS(_x_,_s_) SNDMSG(hStatusBar, SB_SETTEXT, _x_, (LPARAM)_s_);

#define MIN_H 200
#define MIN_W 400

HWND hWnd;
HWND hSearch,hCaseSensitive,hGo;

void CenterOnScreen(HWND hnd);
void add(unsigned char *sms,unsigned char *after);

HANDLE AT_WORK;  //event to start and stop
BOOL I_AM_LOADING = FALSE; //flag for DragDrop

UCHAR *AllData = NULL; //file buffer
DWORD DataLen=0;       //file buffer length
INDEX *indexPointers = NULL; // line indexes
DWORD indexCount=0;          //line indexes length

UCHAR dropped[MAX_PATH]; //url file to open,and later stores the string to search
UCHAR temp[MAX_PATH];   //temp buffer for output

static inline BOOL GetOut()
{
     return (WaitForSingleObject(AT_WORK,0)==0);      
}

void EnableItems(BOOL go,BOOL other)
{
    EnableWindow(hGo,go);    //the GO button
    EnableWindow(hSearch,other);     //the Edit field
    EnableWindow(hCaseSensitive,other);        //the checkbox                   
}

void LoadInitStuff()
{
   _sleep(500); //wait,warmup
   AT_WORK = CreateEvent(NULL, TRUE, FALSE, "TTTS");
   if(AT_WORK == NULL)
   {
        MSGERROR("#Error: CreateEvent");
        SNDMSG(hWnd,WM_DESTROY,0,0);
        return;
   }
   ResetEvent(AT_WORK);
}

void BeginScan()
{
     DWORD len=0;
     DWORD i;
     DWORD found=0;
     RUNFUNC run=findNoCaseSensitive;
     memset(dropped,0,MAX_PATH);
     len =GetWindowText(hSearch,dropped,MAX_PATH);
     if(len == 0)
     {
        EnableItems(TRUE,TRUE); //enable all views
        MSGERROR("String To Search is Empty");  
        return;
     }
     if((SendMessage(hCaseSensitive,BM_GETCHECK,0,0))==BST_CHECKED)
     {
         run = findCaseSensitive;
     }

     EnableItems(TRUE,FALSE);     //ready,disable views
     SetWindowText(hGo,"Stop");     //change GO button text
     SetEvent(AT_WORK);     //signal
     SMS(1,"Status: Scan");
     
     for(i=0;i<indexCount;i++) //do search
     {
         if(GetOut() == FALSE)
           break;   
         sprintf(temp,"Status: Scan %d Line\0",i); 
         SMS(1,temp);
         if(find(indexPointers[i].index,dropped,len,run) == TRUE)
         {
             sprintf(temp,"[Line %d] \0",i+1); 
             add(temp,indexPointers[i].index);
             add("\r\n",NULL);
             found++;
         }
     }
     SetWindowText(hGo,"Start");
     ResetEvent(AT_WORK);
     sprintf(temp,"Status: DONE -> FOUND %d Lines \0",found); 
            
     SMS(1,temp);
     EnableItems(TRUE,TRUE);
}

void Reset()
{
   SMS(0,"File:");
   SMS(1,"Status:");
   SetWindowText(hLogger,"");
   if(AllData != NULL)
      free(AllData);
   if(indexPointers!=NULL)
      free(indexPointers);
   
   DataLen = 0;
   indexCount=0;
}

void LoadTheFile()
{
    DWORD result;
    DWORD i; UCHAR *p;
    EnableItems(FALSE,FALSE);
    I_AM_LOADING = TRUE;
    Reset();
    SetWindowText(hLogger,"");
   
    AllData = ReadTheFile(dropped,&DataLen);
    if(AllData == NULL)
    {
       I_AM_LOADING = FALSE;
      return;
    }
    SMS(1,"Status: Counting Lines...");
    result=CountTheLines(AllData,DataLen);
    
    SMS(1,"Status: Storing Lines...");
    indexPointers = CreateTheIndexes(AllData,DataLen,result);
    if(indexPointers==NULL)
    {
       I_AM_LOADING = FALSE;
       MSGERROR("Can't allocate Memory For the Index Table!!!");
       free(AllData);
       return;
    }
    indexCount = result;
    SMS(1,"Status: OK"); 
    sprintf(temp,"File: Contains %d Lines\0",indexCount); 
    SMS(0,temp); 
    EnableItems(TRUE,TRUE);
    I_AM_LOADING = FALSE;
}
     
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_CREATE:
        {
            InitCommonControls();
            hWnd = hwnd;
            HWND  s1;
            HFONT hFont;
            hLogger=CreateWindow("EDIT","",WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL| WS_HSCROLL | ES_MULTILINE,4,48,570,218,hwnd,(HMENU)0,ins,NULL);
            SendMessage(hLogger,EM_SETLIMITTEXT,(WPARAM)0xFFFFFFFF,(LPARAM)0); 
            s1 = CreateWindow("BUTTON","String To Search",WS_CHILD|WS_VISIBLE|BS_CENTER| BS_GROUPBOX,5,2,317,40,hwnd,NULL,ins,NULL);
            hSearch=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL,12,20,200,18,hwnd,NULL,ins,NULL);
            hCaseSensitive=CreateWindow("BUTTON","CaseSensitive",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,224,20,92,18,hwnd,NULL,ins,NULL);
            hGo=CreateWindow("BUTTON","Start",WS_CHILD|WS_VISIBLE,327,11,60,28,hwnd,(HMENU)2222,ins,NULL);
 
            hFont = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
            SendMessage(s1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
            SendMessage(hCaseSensitive, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(1, 0));
        
            EnableItems(FALSE,FALSE);   
            CreateStatusbar(hwnd);
            CenterOnScreen(hwnd);
            CreateThread(0,0,(LPTHREAD_START_ROUTINE)LoadInitStuff,0,0,0); 
        }break;  
        case WM_DROPFILES:
		{
             if(GetOut() != FALSE) //not running
             {
                 MSGINFO("Stop the Search and then DragDrop a File.");
                 break;
             }
             if(I_AM_LOADING == TRUE)
             {
                 MSGINFO("Loading the File is initialized,you have to wait.");
                 break;
             }
            HDROP hDrop;  			
			memset(dropped,0,MAX_PATH);
			hDrop=(HDROP)wParam;
			DragQueryFile(hDrop,0,dropped,MAX_PATH);
			DragFinish(hDrop);
			if((GetFileAttributes(dropped) & FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY)
			{
               UCHAR *p=(UCHAR*)strrchr(dropped,'/');
               if(p == NULL)
               {
                  p=(UCHAR*)strrchr(dropped,'\\');
               }
               if(p != NULL){ SetWindowText(hwnd,p); }
               SetForegroundWindow(hwnd);
               SetFocus(hwnd);
               CreateThread(0,0,(LPTHREAD_START_ROUTINE)LoadTheFile,0,0,0); 
               break;
            }
            MSGINFO("DragDrop a File not a Folder");
        }break;
        case WM_SIZE:
        {
             if(wParam != SIZE_MINIMIZED)
             {
                int x = LOWORD (lParam);
                int y = HIWORD (lParam);
                ResizeElements( x - MainRect.right, y - MainRect.bottom,x);
             }
         }
         break;
          case WM_GETMINMAXINFO:
          {
             MINMAXINFO * m=(MINMAXINFO*)lParam;
             m->ptMinTrackSize.x=MIN_W;
             m->ptMinTrackSize.y=MIN_H;  
          }break;
       case WM_COMMAND:
       {
            if(LOWORD(wParam) == 2222)
            {
                if(GetOut() == FALSE) //not running
                { 
                    EnableItems(FALSE,FALSE); //disable all views
                    SetWindowText(hLogger,""); //clean the logger
                    CreateThread(0,0,(LPTHREAD_START_ROUTINE)BeginScan,0,0,0); 
                    break;
                }
                ResetEvent(AT_WORK); 
            }   
       }
       break;
        case WM_DESTROY:
        {
            CloseHandle(AT_WORK);
            PostQuitMessage (0);
        }break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain (HINSTANCE hThis,HINSTANCE hPrev, LPSTR lpszA, int nF)

{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    ins=hThis;
    wincl.hInstance = hThis;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (ins,"A");
    wincl.hIconSm = LoadIcon (ins,"A");
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL; 
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx (
           WS_EX_TOPMOST | WS_EX_ACCEPTFILES  | WS_EX_TOOLWINDOW	,
           szClassName,"SearchTool",WS_OVERLAPPEDWINDOW,
           0,0,400,200,HWND_DESKTOP,NULL,hThis,NULL);

    ShowWindow (hwnd, nF);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

void CenterOnScreen(HWND hnd)
{
     RECT rcClient, rcDesktop;
	 int nX, nY;
     SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);
     GetWindowRect(hnd, &rcClient);
	 nX=((rcDesktop.right - rcDesktop.left) / 2) -((rcClient.right - rcClient.left) / 2);
	 nY=((rcDesktop.bottom - rcDesktop.top) / 2) -((rcClient.bottom - rcClient.top) / 2);
     SetWindowPos(hnd, NULL, nX, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void add(unsigned char *sms,unsigned char *after)
{
     DWORD len=GetWindowTextLength(hLogger);
     if(len>0)  SendMessage(hLogger,EM_SETSEL,(WPARAM)len,(LPARAM)len);
     if(sms) SendMessage(hLogger,EM_REPLACESEL,(WPARAM)0,(LPARAM)sms);
     if(after) SendMessage(hLogger,EM_REPLACESEL,(WPARAM)0,(LPARAM)after);
     SendMessage(hLogger,WM_VSCROLL,(WPARAM)SB_ENDSCROLL,(LPARAM)0);
     return;
}

#include "main.h"


void CreateStatusbar(HWND hwnd)
{
    RECT rc = { 0, 0, 0, 0 };
    hStatusBar = CreateWindow(STATUSCLASSNAME, 0, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 
        rc.left, rc.top, rc.right, rc.bottom, hwnd, (HMENU)0, ins, 0);
        
    GetClientRect(hwnd, &rc);
    int nParts[2] = { rc.right / 2, -1 };
    SNDMSG(hStatusBar, SB_SETPARTS, 2, (LPARAM)&nParts);
    
    SNDMSG(hStatusBar, SB_SETTEXT, 0, (LPARAM)"File: ");
    SNDMSG(hStatusBar, SB_SETTEXT, 1, (LPARAM)"Status: ");
}

void MovePos(HWND target,int x,int y,int w,int h)
{
   RECT	rc;
   if(!MoveWindow(target, x, y, w, h,1))
   {
      SetWindowPos(target, NULL, x, y, w, h, SWP_NOZORDER );
   }
   GetClientRect(target,&rc);
   InvalidateRect(target,&rc,1);
}

void ResizeElements(int difx, int dify, int z)
{
    int nParts[2] = { z / 2, -1 };
    SNDMSG(hStatusBar, SB_SETPARTS, 2, (LPARAM)&nParts);
    
    SNDMSG(hStatusBar, WM_SIZE, 0, 0);  
    MovePos(hLogger, LoggerRect.left, LoggerRect.top, LoggerRect.right+ difx+11, LoggerRect.bottom + dify+6);              
}  


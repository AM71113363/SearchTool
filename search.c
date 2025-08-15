#include "main.h"


BOOL findNoCaseSensitive(UCHAR *data,UCHAR *str,DWORD len)
{
     DWORD d=(DWORD)strlen(data);
     DWORD i;
     DWORD n=0;
     BOOL ret=FALSE;   
     if(d < len)
       return FALSE;

     for(i=0;i<d;i++)
     {
       if(tolower(data[i]) == tolower(str[n])){ n++; }else{ n=0; }            
       if(n==len){ ret = TRUE; break; }
     }
 return ret;    
}

BOOL findCaseSensitive(UCHAR *data,UCHAR *str,DWORD len)
{
     DWORD d=(DWORD)strlen(data);
     if(d < len)
      return FALSE;
     if(strstr(data,str) != NULL)
      return TRUE;
 return FALSE;    
}

BOOL find(UCHAR *data,UCHAR *str,DWORD len,RUNFUNC run)
{
     return run(data,str,len);  
}

#include "main.h"

UCHAR *ReadTheFile(UCHAR *name,DWORD *length)
{
    UCHAR *buffer;
    HANDLE fd;
    DWORD result,len;
    fd = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
    if(fd == INVALID_HANDLE_VALUE)
    {
        MSGERROR("Can't open the File!!!");
		return NULL;
    }
    len = GetFileSize(fd,NULL);
    if(len == 0xFFFFFFFF || len == 0)
    {
       CloseHandle(fd);       
       MSGERROR("The File is Empty or the File is >4GB!!!");
       return NULL;
    }  
    buffer = (UCHAR*) malloc(len+1);
    if(buffer == NULL)
    {
        CloseHandle(fd);        
        MSGERROR("Can't allocate Memory!!!");
 		return NULL;
    }
    memset(buffer,0,len+1);

    if(ReadFile(fd, buffer, len, &result, NULL) == FALSE)
    {
        free(buffer);
        CloseHandle(fd);
        MSGERROR("Can't ReadFile!!!");
 		return NULL;
    }
    if(result != len)
    {
        free(buffer);
        CloseHandle(fd);
        MSGERROR("File is Corrupted!!!");
 		return;
    }
    CloseHandle(fd);
    *length = len;
  return buffer;    
}


#include "main.h"

DWORD CountTheLines(UCHAR *buffer,DWORD len)
{
    DWORD i;
    DWORD ret=0;
    UCHAR *p;
    
    for(i=0;i<len;i++)
    {
       if(buffer[i] == '\n')
       {
           ret++;
           p=buffer + i+1;
       }  
    }
    if(p[0] != 0)
    {
        if(strlen(p)>0)
        {
             ret++;          
        }    
    }
  return ret;    
}

INDEX *CreateTheIndexes(UCHAR *buffer,DWORD len,DWORD numElem)
{
    DWORD i;
    DWORD count=0;
    UCHAR *p;
    INDEX *idx = (INDEX*)malloc((numElem+1)*sizeof(INDEX));  
    if(idx != NULL)  
    {
        p=buffer;
        for(i=0;i<len;i++)
        {
            if(buffer[i] == '\n')
            {
                if(count > numElem)
                  break;
                if(i>0 && buffer[i-1] == '\r'){ buffer[i-1]=0; }
                buffer[i]=0;
                idx[count].index = p;
                count++;
                p=buffer + i+1; 
            }  
         }
         if(p[0] != 0 && count < numElem)
         {
             if(strlen(p)>0){ idx[count].index = p; }    
         } 
    }   
  return idx;    
}

#include <expat.h>
#include <stdio.h>
#include <string.h>
#include "resultparse.h"

Item* tree;

int main()
{
  FILE * pFile;
    char buf[1024*1024];
  pFile = fopen ("./net-protokol/get_messages.xml","r");
//  pFile = fopen ("./1msg.xml","r");
  if (pFile!=NULL)
  {
    memset(buf,0,sizeof(buf));
    size_t n = fread ((char*)&buf,1,sizeof(buf),pFile);
//    buf[strlen(buf)-1] = '\0'; //003 -> 0

//printf("%s",buf);
    stripinvalidtag((char*)&buf, strlen(buf));
    tree = xmlparse((char*)&buf, strlen(buf));
printf("%s",tree->tostring().c_str());
    fclose (pFile);
  }
  return 0;

}
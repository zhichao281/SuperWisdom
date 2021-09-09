#include <stdio.h>

long filesize(FILE *stream)
{
 long curpos, length;
 curpos = ftell(stream);
 fseek(stream, 0L, SEEK_END);
 length = ftell(stream);
 fseek(stream, curpos, SEEK_SET);
 return length;
}
int findStartCode(unsigned char *buf, int zeros_in_startcode)
{
 int info;
 int i;

 info = 1;
 for (i = 0; i < zeros_in_startcode; i++)
  if (buf[i] != 0)
   info = 0;

 if (buf[i] != 1)
  info = 0;
 return info;
}
//此处的NALU包括StartCode  
int getNextNalu(FILE* inpf, unsigned char* buf)
{
 int pos = 0;
 int startCodeFound = 0;
 int info2 = 0;
 int info3 = 0;

 while (!feof(inpf) && (buf[pos++] = fgetc(inpf)) == 0);

 while (!startCodeFound)
 {
  if (feof(inpf))
  {
   return pos - 1;
  }
  buf[pos++] = fgetc(inpf);
  info3 = findStartCode(&buf[pos - 4], 3);
  startCodeFound = (info3 == 1);
  if (info3 != 1)
   info2 = findStartCode(&buf[pos - 3], 2);
  startCodeFound = (info2 == 1 || info3 == 1);
 }
 if (info2)
 {
  fseek(inpf, -3, SEEK_CUR);
  return pos - 3;
 }
 if (info3)
 {
  fseek(inpf, -4, SEEK_CUR);
  return pos - 4;
 }
}
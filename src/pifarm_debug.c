#include "pifarm_debug.h"

void debug_assert(const char *p_fct_name, const char *p_filename, int line, const char *assertion, int res)
{
#ifdef DBG_ASSERTION
   if( res == 1)
   {
     printf("ASSERTION ERROR file:%s line:%d function:%s() => %s\n", p_filename, line, p_fct_name, assertion);
     abort();
   }
#endif
   return ;
}

void debug_dump(char *p_info, char *p_mem, uint32_t lenght)
{
   #define  DUMP_CHARS_NB 16

   static char      t_T[] = "0123456789abcdef";
   static char      t_A[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~.";

   static char      t_dump[(DUMP_CHARS_NB*3)+2+DUMP_CHARS_NB+2];
   static char *    p_dump = t_dump;
   static char *    p_ascii_dump = t_dump+(DUMP_CHARS_NB*3)+2;
   static uint32_t  iter;

   *(p_dump+sizeof(t_dump)-2) = '\n';
   *(p_dump+sizeof(t_dump)-1) = '\0';

   memset(p_dump, ' ', sizeof(t_dump)-2);

   printf("---- %s ----\n", p_info);

   for(iter = 0; iter < lenght; iter++)
     {
      *(p_dump + 0) = t_T[(unsigned char)p_mem[iter]/16];
      *(p_dump + 1) = t_T[(unsigned char)p_mem[iter]%16];

      p_dump += 3;

      if( (unsigned char)p_mem[iter] >= 32 && (unsigned char)p_mem[iter] <= 127 )
      {
          *(p_ascii_dump+(iter % DUMP_CHARS_NB)) = t_A[p_mem[iter]-32];
      }
      else
      {
          *(p_ascii_dump+(iter % DUMP_CHARS_NB)) = '.';
      }

      if( p_dump-t_dump == DUMP_CHARS_NB * 3 )
        {
            printf("%s", t_dump);
            p_dump    = t_dump;
            memset(p_dump, ' ', sizeof(t_dump)-2);
        }
     }

   if( p_dump - t_dump != 0) printf("%s", t_dump);
}

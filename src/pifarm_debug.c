/*
    piFarm
    Copyright (C) 2020 Erwan STIPON (F4FTX)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pifarm_debug.h"

void debug_print(char *p_fmt_message, ...)
{
    va_list x_args;
    va_start(x_args, p_fmt_message);
    vprintf(p_fmt_message, x_args);
    printf("\n");
}

void debug_assert(const char *p_fct_name, const char *p_filename, int line, const char *assertion, int res)
{
   if( res == 1)
   {
     DEBUG_PRINT("ASSERTION ERROR file:%s line:%d function:%s() => %s", p_filename, line, p_fct_name, assertion);
     abort();
   }
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

   printf("---- %s ----", p_info);

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

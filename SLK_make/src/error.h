/*
SLK_make - a build-system

Written in 2021 by Lukas Holzbeierlein (Captain4LK) email: captain4lk [at] tutanota [dot] com

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.

You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>. 
*/

#ifndef _ERROR_H_

#define _ERROR_H_

extern int line_on;
extern char *file_on;
extern char *file_contents;
extern int mk_debug_on;

void mk_error(char *format,...);
void mk_debug(char *format,...);

#define assert(cond,mess) if (!(cond)) mk_error("%s:%d - %s\n",__FILE__,__LINE__,mess)

#endif

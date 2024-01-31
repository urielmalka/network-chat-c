#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include <stdarg.h>

// Define ANSI color codes
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5 
#define CYAN 6
#define WHITE 0 
#define RESET 0

// Bold text colors
#define BOLD_RED 10  
#define BOLD_GREEN 20  
#define BOLD_YELLOW 30
#define BOLD_BLUE 50    
#define BOLD_MAGENTA 60 
#define BOLD_CYAN 70    
#define BOLD_WHITE 0   

// Italic text colors
#define ITALIC_RED 100  
#define ITALIC_GREEN 200 
#define ITALIC_YELLOW 300  
#define ITALIC_BLUE 400
#define ITALIC_MAGENTA 500 
#define ITALIC_CYAN 600    
#define ITALIC_WHITE 0  

void printc(int colorCode, const char *format, ...);
void printcb(int colorCode, const char *format, ...);
void printci(int colorCode, const char *format, ...);
void printcu(int colorCode, const char *format, ...);
void printcd(int colorCode, const char *format, ...);
void printcm(int colorCode, const char *format, ...);

char *get_color(int colorCode);
char *get_bold_color(int colorCode);
char *get_italic_color(int colorCode);
char *get_underline_color(int colorCode);
char *get_disco_color(int colorCode);
char *get_mark_color(int colorCode);

#endif


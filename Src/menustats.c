#ifdef _WIN32
#include "menu.h"
#include <urlmon.h>
#define ALLPINGERS "allpingers.txt"
#define URL "http://www.noreply.org/allpingers/allpingers.txt"
#pragma comment(lib,"urlmon.lib")

static const char *files[]={"mlist","rlist","mixring","pgpring","type2list"};
#define NUMFILES sizeof(files)/sizeof(*files)

/* Download all the needed files from the specified source */
static BOOL download(char *source, char *allpingers) {
	const char *localfiles[]={"mlist.txt","rlist.txt","pubring.mix","pgpring.asc","type2.list"};
	char buffer[1024],path[PATHMAX];
	int i,ret = TRUE;
	
	clear();
	standout();
	GetPrivateProfileString(source,"base","",buffer,sizeof(buffer),allpingers);
	if (buffer[0]!='\0')
		printw("%s",buffer);
	standend();
	
	for (i = 0;i < NUMFILES;i++) {
		GetPrivateProfileString(source,files[i],"",buffer,sizeof(buffer),allpingers);
		if (buffer[0]=='\0')
			break;
		mixfile(path, localfiles[i]);
		mvprintw(i+3,0,"downloading %s...",localfiles[i]);
		refresh();
		if (URLDownloadToFile(NULL,buffer,path,BINDF_GETNEWESTVERSION,NULL) != S_OK) {
			printw("failed to download.\n\rTry using another stats source.");
			ret = FALSE;
			break;
		}
		printw("done");
	}
	
	printw("\n\n\rPress any key to continue");
	getch();
	clear();
	return ret;
}
/* Checks whether the stats source has all the required files */
static BOOL good_source(char *source,char *allpingers) {
	char buffer[1024],*ptr;
	int i;
	
	GetPrivateProfileString(source,NULL,"",buffer,sizeof(buffer),allpingers);
	
	for (i = 0;i < NUMFILES;i++) {
		ptr = buffer;
		while (*ptr != '\0') {
			if (!strcmp(ptr,files[i]))
				break;
			ptr+=strlen(ptr)+1;
		}
		if (*ptr == '\0')
			return FALSE;
	}
	
	return TRUE;
}
/* Download allpingers.txt */
static BOOL download_list(char *allpingers) {
	clear();
	standout();
	printw(URL);
	standend();
	
	mvprintw(3,0,"downloading %s...", ALLPINGERS);
	refresh();
	if (URLDownloadToFile(NULL,URL,allpingers,BINDF_GETNEWESTVERSION,NULL) != S_OK) {
		printw("failed to download.\n\rTry again later.");
		printw("\n\n\rPress any key to continue");
		getch();
		return FALSE;
	}
	return TRUE;
}
/* Displays the choice of stats sources */
void update_stats(void) {
	char buffer[1024],*ptr,*stats[MAXREM];
	int i=0,x,y,num=0;
	char path[PATHMAX],c;
	mixfile(path, ALLPINGERS);
	
	while (1) {
		x = 0;
		clear();
		standout();
		printw("Select stats source:\n\n");
		standend();
		if (num == 0) {
			GetPrivateProfileSectionNames(buffer,sizeof(buffer),path);
			ptr = buffer;
			while ((*ptr != '\0') && (num < MAXREM)) {
				if (good_source(ptr,path) == TRUE)
					stats[num++] = ptr;
				ptr+=strlen(ptr)+1;
			}
		}
		
		for (i = 0;i < num;i++) {
			y = i;
			if (y >= LINES - 6)
				y -= LINES - 6, x = 40;
			mvprintw(y + 2, x, "%c", i < 26 ? i + 'a' : i - 26 + 'A');
			mvprintw(y + 2, x + 2, "%s", stats[i]);
		}
		y = i + 3;
		if (y > LINES - 4)
			y = LINES - 4;
		mvprintw(y, 0, "*  update list of pingers");
		c = getch();
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
			if (c >= 'a')
				c -= 'a';
			else
				c = c - 'A' + 26;
			if (c < num) {
				if (download(stats[c],path) == TRUE)
					break;
			}
		}
		else if (c == '*') {
			download_list(path);
		}
		else break;
	}
	clear();
}

#endif /* _WIN32 */

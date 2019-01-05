#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>

#include <termios.h>
#include <pthread.h>
#include <curses.h>

extern unsigned char val[256];

extern char	qthfile[50], tlefile[50], dbfile[50], temp[80], output[25],
	serial_port[15], resave, reload_tle, netport[7],
	once_per_second, ephem[5], sat_sun_status, findsun,
	calc_squint, database, xterm, io_lat, io_lon;

extern char *predictpath;

extern int	indx, antfd, iaz, iel, ma256, isplat, isplong, socket_flag,
  Flags;

extern struct	{  char line1[70];
	   char line2[70];
	   char name[25];
 	   long catnum;
	   long setnum;
	   char designator[10];
 	   int year;
	   double refepoch;
	   double incl;
	   double raan;
	   double eccn;
	   double argper;
	   double meanan;
	   double meanmo;
	   double drag;
	   double nddot6;
  	   double bstar;
	   long orbitnum;
	}  sat[24];

char CopyFile(char *source, char *destination);
char ReadDataFiles();
int AutoUpdate(char *string);
int QuickFind(char *string, char *outputfile);
int QuickPredict(char *string, char *outputfile);
int QuickDoppler100(char *string, char *outputfile);
void NewUser();
void QthEdit();
void bailout(char *string);
void socket_server(char *predict_name);
void MultiTrack();
void MainMenu();
int Print(char *string, char *mode);
int PrintVisible(char *string);
int Select();
char Decayed(int x, double time);
void Predict(char mode);
void PredictMoon();
void PredictSun();
void KepEdit();
void ShowOrbitData();
void SingleTrack(int x, char speak);
void ProgramInfo();
void db_edit();
void Illumination();

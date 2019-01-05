/***************************************************************************\
*          PREDICT: A satellite tracking/orbital prediction program         *
*          Project started 26-May-1991 by John A. Magliacane, KD2BD         *
*                        Last update: 04-May-2018                           *
*****************************************************************************
*                                                                           *
* This program is free software; you can redistribute it and/or modify it   *
* under the terms of the GNU General Public License as published by the     *
* Free Software Foundation; either version 2 of the License or any later    *
* version.                                                                  *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
* General Public License for more details.                                  *
*                                                                           *
*****************************************************************************
*          See the "CREDITS" file for the names of those who have           *
*   generously contributed their time, talent, and effort to this project.  *
\***************************************************************************/

#include "libpredict.h"

int main(argc,argv)
char argc, *argv[];
{
	int x, y, z, key=0;
	char updatefile[80], quickfind=0, quickpredict=0,
	     quickstring[40], outputfile[42], quickdoppler100=0,
	     tle_cli[50], qth_cli[50], interactive=0;
	struct termios oldtty, newtty;
	pthread_t thread;
	char *env=NULL;
	FILE *db;

	/* Set up translation table for computing TLE checksums */

	for (x=0; x<=255; val[x]=0, x++);
	for (x='0'; x<='9'; val[x]=x-'0', x++);

	val['-']=1;

	updatefile[0]=0;
	outputfile[0]=0;
	temp[0]=0;
	tle_cli[0]=0;
	qth_cli[0]=0;
	dbfile[0]=0;
	netport[0]=0;
	serial_port[0]=0;
	once_per_second=0;
		
	y=argc-1;
	antfd=-1;

	/* Make sure entire "quickstring" array is initialized before use */

	for (x=0; x<40; quickstring[x]=0, x++);

	/* Scan command-line arguments */

	for (x=1; x<=y; x++)
	{
		if (strcmp(argv[x],"-f")==0)
		{
			quickfind=1;
			z=x+1;
			while (z<=y && argv[z][0] && argv[z][0]!='-')
			{
				if ((strlen(quickstring)+strlen(argv[z]))<37)
				{
					strncat(quickstring,argv[z],15);
					strcat(quickstring,"\n");
					z++;
				}
			}
			z--;
		}

		if (strcmp(argv[x],"-p")==0)
		{
			quickpredict=1;
			z=x+1;

			while (z<=y && argv[z][0] && argv[z][0]!='-')
			{
				if ((strlen(quickstring)+strlen(argv[z]))<37)
				{
					strncat(quickstring,argv[z],15);
					strcat(quickstring,"\n");
					z++;
				}
			}
			z--;
		}

		if (strcmp(argv[x],"-dp")==0)
		{
			quickdoppler100=1;
			z=x+1;

			while (z<=y && argv[z][0] && argv[z][0]!='-')
			{
				if ((strlen(quickstring)+strlen(argv[z]))<37)
				{
					strncat(quickstring,argv[z],15);
					strcat(quickstring,"\n");
					z++;
				}
			}
			z--;
		}

		if (strcmp(argv[x],"-u")==0)
		{
			z=x+1;
			while (z<=y && argv[z][0] && argv[z][0]!='-')
			{
				if ((strlen(updatefile)+strlen(argv[z]))<75)
				{
					strncat(updatefile,argv[z],75);
					strcat(updatefile,"\n");
					z++;
				}
			}
			z--;	
		}


		if (strcmp(argv[x],"-t")==0)
		{
			z=x+1;
			if (z<=y && argv[z][0] && argv[z][0]!='-')
				strncpy(tle_cli,argv[z],48);
		}

		if (strcmp(argv[x],"-q")==0)
		{
			z=x+1;
			if (z<=y && argv[z][0] && argv[z][0]!='-')
				strncpy(qth_cli,argv[z],48);
		}

		if (strcmp(argv[x],"-a")==0)
		{
			z=x+1;
			if (z<=y && argv[z][0] && argv[z][0]!='-')
				strncpy(serial_port,argv[z],13);
		}

		if (strcmp(argv[x],"-a1")==0)
		{
			z=x+1;
			if (z<=y && argv[z][0] && argv[z][0]!='-')
				strncpy(serial_port,argv[z],13);
			once_per_second=1;
		}

		if (strcmp(argv[x],"-o")==0)
		{
			z=x+1;
			if (z<=y && argv[z][0] && argv[z][0]!='-')
				strncpy(outputfile,argv[z],40);
		}

		if (strcmp(argv[x],"-n")==0)
		{
			z=x+1;
			if (z<=y && argv[z][0] && argv[z][0]!='-')
				strncpy(netport,argv[z],5);
		}

		if (strcmp(argv[x],"-s")==0)
			socket_flag=1;

		if (strcmp(argv[x],"-north")==0) /* Default */
			io_lat='N';

		if (strcmp(argv[x],"-south")==0)
			io_lat='S';

		if (strcmp(argv[x],"-west")==0)  /* Default */
			io_lon='W';

		if (strcmp(argv[x],"-east")==0)
			io_lon='E';
	}

	/* We're done scanning command-line arguments */

	/* If no command-line (-t or -q) arguments have been passed
	   to PREDICT, create qth and tle filenames based on the
	   default ($HOME) directory. */

	env=getenv("HOME");

	if (qth_cli[0]==0)
		sprintf(qthfile,"%s/.predict/predict.qth",env);
	else
		/* sprintf(qthfile,"%s%c",qth_cli,0); */
		sprintf(qthfile,"%s",qth_cli);

	if (tle_cli[0]==0)
		sprintf(tlefile,"%s/.predict/predict.tle",env);
	else
		/* sprintf(tlefile,"%s%c",tle_cli,0); */
		sprintf(tlefile,"%s",tle_cli);

	/* Test for interactive/non-interactive mode of operation
	   based on command-line arguments given to PREDICT. */

	if (updatefile[0] || quickfind || quickpredict || quickdoppler100)
		interactive=0;
	else
		interactive=1;

	if (interactive)
	{
		sprintf(dbfile,"%s/.predict/predict.db",env);

		/* If the transponder database file doesn't already
		   exist under $HOME/.predict, and a working environment
		   is available, place a default copy from the PREDICT
		   distribution under $HOME/.predict. */

		db=fopen(dbfile,"r");

		if (db==NULL)
		{
			sprintf(temp,"%sdefault/predict.db",predictpath);
			CopyFile(temp,dbfile);
		}

		else
			fclose(db);
	}

	x=ReadDataFiles();

	if (x>1)  /* TLE file was loaded successfully */
	{
		if (updatefile[0])  /* -u was passed to PREDICT */
		{
			y=0;
			z=0;
			temp[0]=0;

			while (updatefile[y]!=0)
			{
				while (updatefile[y]!='\n' && updatefile[y]!=0 && y<79)
				{
					temp[z]=updatefile[y];
					z++;
					y++;
				}

				temp[z]=0;

				if (temp[0])
				{
					AutoUpdate(temp);
					temp[0]=0;
					z=0;
					y++;
				}
			}

			exit(0);
		}
	}

	if (x==3)  /* Both TLE and QTH files were loaded successfully */
	{
		if (quickfind)  /* -f was passed to PREDICT */
			exit(QuickFind(quickstring,outputfile));

		if (quickpredict)  /* -p was passed to PREDICT */
			exit(QuickPredict(quickstring,outputfile));

		if (quickdoppler100)  /* -dp was passed to PREDICT */
			exit(QuickDoppler100(quickstring,outputfile));
	}

	else
	{
		if (tle_cli[0] || qth_cli[0])
		{
			/* "Houston, we have a problem..." */

			printf("\n%c",7);

			if (x^1)
				printf("*** ERROR!  Your QTH file \"%s\" could not be loaded!\n",qthfile);

			if (x^2)
				printf("*** ERROR!  Your TLE file \"%s\" could not be loaded!\n",tlefile);

			printf("\n");

			exit(-1);
		}
	}

	if (interactive)
	{
		/* We're in interactive mode.  Prepare the screen */

		/* Are we running under an xterm or equivalent? */

		env=getenv("TERM");

		if (env!=NULL && strncmp(env,"xterm",5)==0)
			xterm=1;
		else
			xterm=0; 

		/* Start ncurses */

		initscr();
		start_color();
		cbreak();
		noecho();
		scrollok(stdscr,TRUE);
		curs_set(0);

		init_pair(1,COLOR_WHITE,COLOR_BLACK);
		init_pair(2,COLOR_WHITE,COLOR_BLUE);
		init_pair(3,COLOR_YELLOW,COLOR_BLUE);
		init_pair(4,COLOR_CYAN,COLOR_BLUE);
		init_pair(5,COLOR_WHITE,COLOR_RED);
		init_pair(6,COLOR_RED,COLOR_WHITE);
		init_pair(7,COLOR_CYAN,COLOR_RED);

		if (x<3)
		{
			/* A problem occurred reading the
			   default QTH and TLE files, and
			   no -t or -q options were
			   provided on the command-line. */

			NewUser();
			x=ReadDataFiles();
			QthEdit();
		}
	}

	if (x==3)
	{
		/* Open serial port to send data to
		   the antenna tracker if present. */

		if (serial_port[0]!=0)
		{
			/* Make sure there's no trailing '/' */

			x=strlen(serial_port);

			if (serial_port[x-1]=='/')
				serial_port[x-1]=0;

			antfd=open(serial_port, O_WRONLY|O_NOCTTY);

			if (antfd!=-1)
			{
				/* Set up serial port */

				tcgetattr(antfd, &oldtty);
				memset(&newtty, 0, sizeof(newtty));

				/* 9600 baud, 8-bits, no parity,
				   1-stop bit, no handshaking */

				newtty.c_cflag=B9600|CS8|CLOCAL;
				newtty.c_iflag=IGNPAR;
				newtty.c_oflag=0;
				newtty.c_lflag=0;

				tcflush(antfd, TCIFLUSH);
				tcsetattr(antfd, TCSANOW, &newtty);
			}

			else
			{
				bailout("Unable To Open Antenna Port");
				exit(-1);
			}
		}
	
		/* Socket activated here.  Remember that
		   the socket data is updated only when
		   running in the real-time tracking modes. */

		if (socket_flag)
		{
			pthread_create(&thread,NULL,(void *)socket_server,(void *)argv[0]);
			bkgdset(COLOR_PAIR(3));
			MultiTrack();
		}

		MainMenu();

		do
		{	
			key=getch();

			if (key!='T')
				key=tolower(key);

			switch (key)
			{
				case 'p':
				case 'v':
					Print("",0);
					PrintVisible("");
					indx=Select();

					if (indx!=-1 && sat[indx].meanmo!=0.0 && Decayed(indx,0.0)==0)
						Predict(key);

					MainMenu();
					break;

				case 'l':
					Print("",0);
					PredictMoon();
					MainMenu();
					break;

				case 'o':
					Print("",0);
					PredictSun();
					MainMenu();
					break;

				case 'u':
					AutoUpdate("");
					MainMenu();
					break;

				case 'e':
					KepEdit();
					MainMenu();
					break;

				case 'd':
					ShowOrbitData();
					MainMenu();
					break;

				case 'g':
					QthEdit();
					MainMenu();
					break;

				case 't':
				case 'T':
					indx=Select();

					if (indx!=-1 && sat[indx].meanmo!=0.0 && Decayed(indx,0.0)==0)
						SingleTrack(indx,key);

					MainMenu();
					break;

				case 'm':
					MultiTrack();
					MainMenu();
					break;

				case 'i':
					ProgramInfo();
					MainMenu();
					break;

				case 'b':
					db_edit();
					MainMenu();
					break;

				case 's':
					indx=Select();
					if (indx!=-1 && sat[indx].meanmo!=0.0 && Decayed(indx,0.0)==0)
					{
						Print("",0);
						Illumination();
					}
					MainMenu();
					break;
			}

		} while (key!='q' && key!=27);

		if (antfd!=-1)
		{
			tcsetattr(antfd,TCSANOW,&oldtty);
			close(antfd);
		}

		curs_set(1);	
		bkgdset(COLOR_PAIR(1));
		clear();
		refresh();
		endwin();
	}

	exit(0);
}


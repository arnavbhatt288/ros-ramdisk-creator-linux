/*
 * PROJECT:     ReactOS FreeLoader installer for Linux
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     Main functions
 * COPYRIGHT:   Copyright 2020 Arnav Bhatt (arnavbhatt2004@gmail.com)
 */

#include "copy.h"
#include "install.h"
#include "ini.h"
#include "volume.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int opt;
	bool dFlag = false;
	bool uFlag = false;
	bool fsFlag = false;
	bool bFlag = false;
	bool lFlag = false;
	bool sFlag = false;
	bool iFlag = false;
	bool processFlag = false;

	char* drive;
	char* fstype;
	char* dir;
	char* bsource;
	char* lsource;
	char* ssource;
	char bdest[512];
	char ldest[512];
	char sdest[512];
	char inisource[512];

	while((opt = getopt(argc, argv, "d:u:b:l:s:f:i::")) != -1)
	{
		switch(opt)
		{
			case 'd':
				drive = optarg;
				dFlag = true;
				break;

			case 'u':
				dir = optarg;
				uFlag = true;
				break;

			case 'f':
				fstype = optarg;
				fsFlag = true;
				break;

			case 'b':
				bsource = optarg;
				bFlag = true;
				break;

			case 'l':
				lsource = optarg;
				lFlag = true;
				break;

			case 's':
				ssource = optarg;
				sFlag = true;
				break;

			case 'i':
				iFlag = true;
				break;
		}
	}

	if(dFlag == true)
	{
		if(fsFlag == false)
		{
			printf("Missing argument: '-f'\n");
			return 0;
		}
		if (!OpenVolume(drive))
		{
			printf("Exiting program...\n");
			return -1;
		}

		if (!InstallBootSector(fstype))
		{
			printf("Exiting program...\n");
			return -1;
		}

		return 0;

	}
	else if(uFlag == true)
	{
		if(bFlag == true)
		{
			sprintf(bdest, "%s/bootcd.iso", dir);
			if(!CopyFile(bdest, bsource))
			{
				printf("Exiting program...\n");
				return -1;
			}
		}

		else if(lFlag == true)
		{
			sprintf(ldest, "%s/livecd.iso", dir);
			if(!CopyFile(ldest, lsource))
			{
				printf("Exiting program...\n");
				return -1;
			}
		}

		else if(sFlag == true)
		{
			sprintf(sdest, "%s/freeldr.sys", dir);
			if(!CopyFile(sdest, ssource))
			{
				printf("Exiting program...\n");
				return -1;
			}
		}
		else
		{
			printf("Missing argument: either '-b', '-l' or '-s'\n");
			return 0;
		}

		if (iFlag == true)
		{
			sprintf(inisource, "%s/freeldr.ini", dir);
			processFlag = CreateINI(inisource, "FREELOADER", "DefaultOS", "Setup");
			processFlag = CreateINI(inisource, "FREELOADER", "TimeOut", "5");
			processFlag = CreateINI(inisource, "Display", "TitleText", "ReactOS Setup (RAM Disk)");
			processFlag = CreateINI(inisource, "Display", "StatusBarColor", "Cyan");
			processFlag = CreateINI(inisource, "Display", "StatusBarTextColor", "Black");
			processFlag = CreateINI(inisource, "Display", "BackdropTextColor", "White");
			processFlag = CreateINI(inisource, "Display", "BackdropColor", "Blue");
			processFlag = CreateINI(inisource, "Display", "BackdropFillStyle", "Medium");
			processFlag = CreateINI(inisource, "Display", "TitleBoxTextColor", "White");
			processFlag = CreateINI(inisource, "Display", "TitleBoxColor", "Red");
			processFlag = CreateINI(inisource, "Display", "MessageBoxTextColor", "White");
			processFlag = CreateINI(inisource, "Display", "MessageBoxColor", "Blue");
			processFlag = CreateINI(inisource, "Display", "MenuTextColor", "Gray");
			processFlag = CreateINI(inisource, "Display", "MenuColor", "Black");
			processFlag = CreateINI(inisource, "Display", "TextColor", "Gray");
			processFlag = CreateINI(inisource, "Display", "SelectedTextColor", "Black");
			processFlag = CreateINI(inisource, "Display", "SelectedColor", "Gray");
			processFlag = CreateINI(inisource, "Display", "ShowTime", "No");
			processFlag = CreateINI(inisource, "Display", "MenuBox", "No");
			processFlag = CreateINI(inisource, "Display", "CenterMenu", "No");
			processFlag = CreateINI(inisource, "Display", "MinimalUI", "Yes");
			processFlag = CreateINI(inisource, "Display", "TimeText", "Seconds until highlighted choice will be started automatically:");

			if (bFlag == true)
			{
				processFlag = CreateINI(inisource, "Operating Systems", "Setup", "\"Setup\"");
				processFlag = CreateINI(inisource, "Setup", "BootType", "ReactOSSetup");
				processFlag = CreateINI(inisource, "Setup", "SystemPath", "ramdisk(0)");
				processFlag = CreateINI(inisource, "Setup", "Options", "/RDPATH=bootcd.iso");
			}

			if (lFlag == true)
			{
				processFlag = CreateINI(inisource, "Operating Systems", "LiveCD", "\"LiveCD\"");
				processFlag = CreateINI(inisource, "LiveCD", "BootType", "Windows2003");
				processFlag = CreateINI(inisource, "LiveCD", "SystemPath", "ramdisk(0)\\reactos");
				processFlag = CreateINI(inisource, "LiveCD", "Options", "/MININT /RDPATH=livecd.iso /RDEXPORTASCD");
			}

			if (processFlag == false)
			{
				printf("Exiting program...\n");
				return -1;
			}

			return 0;
		}
	}

	else
	{
		printf("\nReactOS Ramdisk Creator V1.0\nCopyright 2019-2020 Arnav Bhatt\n\n");
		printf("Usage:\n./name-of-program (provide necessary args)\n\n");
		printf("Arguments for freeldr boot code installation:\n");
		printf("-d - Provide partition of your drive\n");
		printf("-f - Provide your drive's filesystem\n\n");
		printf("Arguments for file copying:\n");
		printf("-u - Provide your drive's directory\n");
		printf("-b - Provide your bootcd's directory\n");
		printf("-l - Provide your livecd's directory\n");
		printf("-s - Provide your freeldr.sys directory\n\n");
		printf("Optional arguments:\n");
		printf("-i - Generates freeldr.ini (use it with either -b or -l)\n");
		return 0;
	}
}

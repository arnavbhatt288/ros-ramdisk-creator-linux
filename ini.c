/*
 * PROJECT:     ReactOS FreeLoader installer for Linux
 * LICENSE:     GPL-2.0-or-later (https://spdx.org/licenses/GPL-2.0-or-later)
 * PURPOSE:     INI functions
 * COPYRIGHT:   Copyright 2020 Arnav Bhatt (arnavbhatt2004@gmail.com)
 */

#include "ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool CreateINI(char *pacPath, char *pacTopic, char *pacItem, char *pacValue)
{
    int                 iFoundTopic;
    int                 iFoundItem;
    int                 iError;
	int                 iItemLength;
    int                 iValueLength;
    long                lTopicFilePos;
    char                acTopicHeading[80];
    char                acLastTopicHeading[80];
    char                acItemHeading[80];
    char                acIniLine[160];
    char                acIniPath[160];
    char                acTempPath[160];
    FILE                *pFIniFile;
    FILE                *pFTempIni;

    iError = true;
    acLastTopicHeading[0] = '\0';

    strcpy(acIniPath, pacPath);

    strcpy(acTempPath, pacPath);
    strcat(acTempPath, "temp");

    // add brackets to topic
    strcpy(acTopicHeading, "[");
    strcat(acTopicHeading, pacTopic);
    strcat(acTopicHeading, "]\n");

    strcpy(acItemHeading, pacItem);
    strcat(acItemHeading, "=");

    iItemLength = strlen(acItemHeading);
    iValueLength = strlen(pacValue);

    iFoundTopic = 0;
    iFoundItem = 0;

    if ((pFTempIni = fopen (acTempPath, "w")) == NULL)
	{
        printf ("Could not open temp ini file to write settings\n");
        iError = false;
        return(iError);
    }

    // try to open current config file for reading
    if ((pFIniFile = fopen (acIniPath, "r")) != NULL)
	{
        // read a line from the config file until EOF
        while (fgets(acIniLine, 159, pFIniFile) != NULL)
		{
            // the item has been found so continue reading file to end
            if (iFoundItem == 1)
			{
                fputs (acIniLine, pFTempIni);
                continue;
            }
            // topic has not been found yet
            if (iFoundTopic == 0)
			{
                if (strcmp(acTopicHeading, acIniLine) == 0)
				{
                    // found the topic
                    iFoundTopic = 1;
                }
                fputs (acIniLine, pFTempIni);
                continue;
            }
            // the item has not been found yet
            if ((iFoundItem == 0) && (iFoundTopic == 1)) {
                if ( strncmp (acItemHeading, acIniLine, iItemLength) == 0) {
                    // found the item
                    iFoundItem = 1;
                    if (iValueLength > 0) {
                        fputs (acItemHeading, pFTempIni);
                        fputs (pacValue, pFTempIni);
                        fputs ("\n", pFTempIni);
                    }
                    continue;
                }
                // if newline or [, the end of the topic has been reached
                // so add the item to the topic
                if ((acIniLine[0] == '\n') || (acIniLine[0] == '[')) {
                    iFoundItem = 1;
                    if ( iValueLength > 0) {
                        fputs(acItemHeading, pFTempIni);
                        fputs(pacValue, pFTempIni);
                        fputs("\n", pFTempIni);
                    }
                    fputs("\n", pFTempIni);
                    if(acIniLine[0] == '[') {
                        fputs(acIniLine, pFTempIni);
                    }
                    continue;
                }
                // if the item has not been found, write line to temp file
                if(iFoundItem == 0) {
                    fputs (acIniLine, pFTempIni);
                    continue;
                }
            }
        }
        fclose(pFIniFile);
    }
    // still did not find the item after reading the config file
    if (iFoundItem == 0) {
        // config file does not exist
        // or topic does not exist so write to temp file
        if (iValueLength > 0) {
            if (iFoundTopic == 0) {
                fputs(acTopicHeading, pFTempIni);
            }
            fputs(acItemHeading, pFTempIni);
            fputs(pacValue, pFTempIni);
            fputs("\n\n", pFTempIni);
        }
    }

    fclose(pFTempIni);

    //delete the ini file
    remove(acIniPath);

    // rename the temp file to ini file
    rename(acTempPath, acIniPath);

    return(iError);
}
   
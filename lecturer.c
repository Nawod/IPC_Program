#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void share(char *exFile/*existing file*/, char *newFile);//function for copy file(share)

int main (int argument, char *getaction[])
{
//Check the command line actions
	if (argument != 2)
		printf("Please provide an action with command line\n\t* view\n\t* upload\n\t* download\n\t* delete\n\n");
	else
	{
		char *action = getaction[1];
		struct dirent *sDirent;//create dirent structure
		key_t key = 5555;//unique key(ftok)

		int shmid = shmget(key,1024,0666|IPC_CREAT);//key,size,permission

		char *Spath = (char*) shmat(shmid,(void*)0,0);//add a shared location
		Spath = "sharedMemory/";//mentioned shared directory path

		DIR *dirpath = opendir (Spath);
		if (dirpath == NULL)
		{
			printf("\nERROR! Shared memory can not be opened\n\n");
			exit(1);
		}

//View action
		if (strcmp(action,"view") == 0)
		{
			while (sDirent = readdir (dirpath))
				printf("%s\n",sDirent->d_name);
			closedir(dirpath);
		}

//Upload action
		else if (strcmp(action,"upload") == 0)
		{
			char exFile[50];
            //take file name to upload
			printf("Enter the file name that want to upload\n");
			gets(exFile);

			char *newFile;
			newFile = malloc(strlen(Spath)+strlen(exFile));
			strcpy(newFile, Spath);
			strcat(newFile, exFile);

			share(exFile, newFile);//call share function
			printf("\n**File upload successfully**\n\n");
		}

//Download action
		else if (strcmp(action,"download") == 0)
		{
			char *currentPath, *downloadPath;
			char exFile[50], newPath[500];

			//Display files in the shared memory
			while (sDirent = readdir (dirpath))
				printf("%s\n",sDirent->d_name);
			closedir(dirpath);
           
            //select file
			printf("Enter the file name that want to download\n");
			gets(exFile);
			currentPath = malloc(strlen(Spath)+strlen(exFile));
			strcpy(currentPath, Spath);
			strcat(currentPath, exFile);
            
            //get download path
			printf("Enter the path to download (press ENTER to download current directory or type new path)\n");
			gets(newPath);
			downloadPath = malloc(strlen(newPath)+strlen(exFile));
			strcpy(downloadPath, newPath);
			strcat(downloadPath, exFile);

			share(currentPath, downloadPath);//call share function
			printf("\n**File successfully downloaded**\n\n");
		}

//Delete action
		else if (strcmp(action,"delete") == 0)
		{
			int status;
			char fileName[50];
			char *deleteFile;

			//Display files in shared memory
			while (sDirent = readdir (dirpath))
				printf("%s\n",sDirent->d_name);
			closedir(dirpath);

            //select a file
			printf("Enter the file name that want to delete\n");
			gets(fileName);

            //delete the file
			deleteFile = malloc(strlen(Spath)+strlen(fileName));
			strcpy(deleteFile, Spath);
			strcat(deleteFile, fileName);

			status = remove(deleteFile);

			if (status == 0)
				printf("\n**%s is successfully deleted**\n\n", fileName);
			else
			{
				printf("ERROR! File could not delete\n\n");
				exit(1);
			}
		}
		else
			printf("Invalid action provided\n\n");
			exit(1);

		
		shmdt(Spath);
	}
	return 0;
}

//share function
void share(char *exFile, char *newFile)
{
	char ch;
	FILE *exfile, *newfile;

	exfile = fopen(exFile, "r");
	if (exfile == NULL)
	{
		printf("\nERROR! can not open selected file\n\n");
		exit(1);
	}

	newfile = fopen(newFile, "w");
	if (newfile == NULL)
	{
		printf("\nERROR!\n file can not be uploaded/downloaded\n\n");
		exit(1);
	}
	while ((ch = fgetc(exfile)) != EOF)
		fputc(ch, newfile);

	fclose(exfile);
	fclose(newfile);
}
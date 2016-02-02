//
//  Platform.cpp
//  ModelStudio
//
//  Created by howe on 15/6/25.
//
//

#include "Platform.h"
#include <iostream>
#include <fstream>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <io.h>
#include <ShlObj.h>
#include <direct.h>
#else
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

using namespace std;


int Platform::create_multi_dir(const char *path)
{

    size_t i, len;
    
    len = strlen(path);
    char dir_path[1024] = "\0";
    
 //   strncpy(dir_path, path, len);
    char mode = '/';
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    mode = '\\';
#endif
//    for (i=0; i<len; i++)
//    {
//
//        if (dir_path[i] == mode && i > 0)
//        {
//            dir_path[i]='\0';
           // if (Platform::DirIsExist(dir_path) < 0)
            {
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 
                std::string dosCmd = "md ";
                dosCmd += path;
                system(dosCmd.c_str());
#else
              if (mkdir(dir_path, 0755) < 0)
              {
                  printf("mkdir=%s:msg=%s\n", dir_path, strerror(errno));
                  return -1;
              }
#endif
                
                
            }
//        }
//    }

    return 0;
}

std::vector<std::string> fileArray;
void _loopFiles(const std::string &folderPath,int type)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    _finddata_t FileInfo;
    std::string strfind = folderPath + "\\*";
    long Handle = _findfirst(strfind.c_str(), &FileInfo);
    if (Handle == -1L)
    {
		std::string errorlog = "Canot open dir!" + folderPath;
        fprintf(stderr,"Canot open dir: %s\n", errorlog.c_str());
		return;
    }
    do{
        if (FileInfo.attrib & _A_SUBDIR)
        {
            if( (strcmp(FileInfo.name,".") != 0 ) &&(strcmp(FileInfo.name,"..") != 0))
            {
                std::string newPath = folderPath + "\\" + FileInfo.name;
                if (type == 1)
                {
                    fileArray.push_back(newPath);
                    continue;
                }
                _loopFiles(newPath,type);
            }
        }
        else
        {
            if (type == 0)
            {
                std::string filename = (folderPath + "\\" + FileInfo.name);
                fileArray.push_back(filename);
            }
        }
    }while (_findnext(Handle, &FileInfo) == 0);
    _findclose(Handle);
#else
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(folderPath.c_str())) == NULL)
    {
        fprintf(stderr,"cannot open directory: %s\n", folderPath.c_str());
        return;
    }
    chdir(folderPath.c_str());
    while((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode))
        {
            if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
            {
                continue;
            }
            if (type == 1)
            {
                //dir
                fileArray.push_back(folderPath + "/" + entry->d_name);
                continue;
            }
            _loopFiles(folderPath + "/" + entry->d_name,type);
        }
        else
        {
            if (type == 0)
            {
                std::string filename = entry->d_name;
                fileArray.push_back(folderPath + "/" + filename);
            }
        }
    }
    chdir("..");
    closedir(dp);
#endif
}

std::vector<std::string> Platform::loopFiles(const std::string &folderPath)
{
    fileArray.clear();
    _loopFiles(folderPath,0);
    return fileArray;
}

std::vector<std::string> Platform::loopDirectories(const std::string &folderPath)
{
    fileArray.clear();
    _loopFiles(folderPath,1);
    return fileArray;
}

int Platform::DirIsExist( const char* path ) {
    //    return access( path, F_OK );
    //#define	F_OK		0	/* test for existence of file */
    //    return access( path, 0 );
    //Each of these functions returns 0 if the file has the given mode. The function returns –1 if the named file does not exist or is not accessible in the given mode; in this case, errno is set as follows:
   
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)

	WIN32_FIND_DATA  wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(path, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;
	}
	FindClose(hFind);
	return rValue;


#endif
	std::fstream _file;
	std::string filePath = path;
    _file.open( path, std::ios::in );
    if (!_file) {
        // not exist
        return -1;
    } else {
        // exist
        return 0;
    }
    return -1;
}

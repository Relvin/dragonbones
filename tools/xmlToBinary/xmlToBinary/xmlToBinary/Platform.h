//
//  Platform.h
//  ModelStudio
//
//  Created by howe on 15/6/25.
//
//

#ifndef __ModelStudio__Platform__
#define __ModelStudio__Platform__

#include <stdio.h>
#include <vector>
#include <map>

#define CC_PLATFORM_UNKNOWN            0
#define CC_PLATFORM_IOS                1
#define CC_PLATFORM_ANDROID            2
#define CC_PLATFORM_WIN32              3
#define CC_PLATFORM_MARMALADE          4
#define CC_PLATFORM_LINUX              5
#define CC_PLATFORM_BADA               6
#define CC_PLATFORM_BLACKBERRY         7
#define CC_PLATFORM_MAC                8
#define CC_PLATFORM_NACL               9
#define CC_PLATFORM_EMSCRIPTEN        10
#define CC_PLATFORM_TIZEN             11
#define CC_PLATFORM_QT5               12
#define CC_PLATFORM_WP8               13
#define CC_PLATFORM_WINRT             14


#if defined(CC_TARGET_OS_WIN)
#undef CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_WIN32
#endif



class Platform
{
public:
    static std::string getDesktopDirectory();

    static std::vector<std::string> findPaths(const std::string &dir_path,const std::string &partten);

    static std::vector<std::string> loopFiles(const std::string &dir_path);

    static std::vector<std::string> loopDirectories(const std::string &dir_path);
    static int DirIsExist( const char* path );
    static int create_multi_dir(const char *path);

};
#endif /* defined(__ModelStudio__Platform__) */

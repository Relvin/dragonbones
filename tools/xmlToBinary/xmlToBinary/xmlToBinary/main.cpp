//
//  main.cpp
//  xmlToBinary
//
//  Created by Relvin on 15/12/3.
//  Copyright © 2015年 Relvin. All rights reserved.
//

#include <iostream>
#include "xmlToBinary.hpp"
#include "Platform.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

int main(int argc, const char * argv[]) {
    // insert code here...
    
    std::string extFilePath = "";
    std::string exeName = "";
	std::string project_path = "";

	if (argc < 3)
	{
        return -1;
	}
    
    project_path = argv[1];
    extFilePath = argv[2];
	
    char buf[1024] = "\0";
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
     _getcwd( buf, sizeof(buf));
#else
    getcwd(buf,sizeof(buf));
#endif
	project_path = buf + project_path;
	extFilePath = buf + extFilePath;
	
    
//    printf("project_path = %s \n",project_path.c_str());
    std::vector<std::string> fileArray = Platform::loopFiles(project_path.c_str());
    for (int idx = 0;idx < fileArray.size();idx++)
    {
		xmlToBinary::getInstance()->converBoneToBinaryByFile(fileArray.at(idx), extFilePath);
    }
    
//    std::string filePath = "/Users/dengqinghong/Desktop/1-boss.xml";
    
    
    std::cout << " ===>SUCCESS!<===\n";

    return 0;
}

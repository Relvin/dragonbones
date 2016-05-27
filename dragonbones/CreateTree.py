#encoding:utf-8

import sys
import os

searchedPath = []
pathTree = []
def createTree(rootPath,startPath):

    delthPathArr = rootPath[len(startPath):].split('/')
    deltaPath = ""
    for value in delthPathArr:
        deltaPath += value
        searchedPath.append(deltaPath)
        deltaPath += "/"
    searchedPath.reverse()
    searchedPath.append("")
    print pathTree
    for (dirpath,dirnames,filenames) in os.walk(startPath):
        curPath = ""
        for value in searchedPath:
            if dirpath.find(os.path.join(startPath,value)) != -1:
                fullPath = os.path.join(startPath,value)
                curPath += dirpath[len(fullPath):]
                if len(dirpath) != len(fullPath):
                    curPath += "/"
                break
            curPath += "../"
        for filename in filenames:
            if filename.endswith("cpp"):
                pathTree.append(curPath + filename)

    for value in pathTree:
        print "%s" %value
    print "createTree"

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "arguments error!"
    else:
        rootPath = sys.argv[1]
        startPath = sys.argv[2]
        if rootPath.find(startPath) == -1:
            print "arguments error!"
        else:
            createTree(rootPath,startPath)
#encoding=utf8

import sys,os

def getXml2XmlbRuntime():
    if(os.name == 'posix'):
        return './mac/xmlToBinary '
    else:
        return './win32/xmlToBinary.exe '

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print "USAGE:python xml2xmlb.py [path/xxx.xml] [outpath:option]"
    else:
        outpath = "./"
        value_0 = sys.argv[1]
        if len(sys.argv) >= 3:
            outpath = sys.argv[2]
        if not os.path.isabs(outpath):
            currentPath = value_0
            if not os.path.isdir(value_0):
                currentPath = os.path.dirname(value_0)
            outpath = os.path.join(currentPath,outpath)

        command = getXml2XmlbRuntime()
        for(dirpath, dirnames, filenames) in os.walk(value_0):
                deltaPath = dirpath[len(value_0):]
                if deltaPath != "":
                    deltaPath = deltaPath.replace('\\','/')
                    deltaPath = deltaPath[1 if deltaPath[0] == '/' else 0 :]
                exportPath = os.path.join(outpath,deltaPath)
                if not os.path.isdir(exportPath):
                    os.makedirs(exportPath)
                # print "deltaPath = ",deltaPath
                for filename in filenames:
                    if filename.endswith(".xml"):
                        os.system(command + os.path.join(dirpath,filename) + " " + os.path.join(exportPath,filename))
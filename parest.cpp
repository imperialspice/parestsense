//
// Created by middleton on 2/22/23.
//
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>

int main(int argc, char** argv){
    for(int i = 0; i < argc; i++){
        std::cout << std::string(argv[i]) << std::endl;
    }

    std::ifstream openFile;
    std::ofstream writeFile;
    openFile.open("graph.example");
    char * fileNameOut = new char[256];
    snprintf(fileNameOut, 256, "%s", argv[argc-1]);
    writeFile.open(fileNameOut);
    std::string str;
    std::string fileOutput;
    while(std::getline(openFile, str)){
        fileOutput += str;
        fileOutput.push_back('\n');
    }

    writeFile.write(fileOutput.c_str(), fileOutput.length());

}
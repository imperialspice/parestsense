//
// Created by middleton on 2/23/23.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::vector<std::vector<double>> readTo (std::string fileName){
    std::string dividerTab = "\t";
    std::string dividerSpace = " ";

    std::ifstream example(fileName);
    std::string str;
    std::vector<std::vector<double>> data;
    size_t pos;
    int insertRounds = 0;
    int newLine = -1;
    while(std::getline(example, str)){
        // are we at the start?
        newLine++;
        // remove the space after time
        str.erase(0, str.find(dividerSpace)+dividerSpace.length());

        // subdivide the rest of the data
        while(((pos = str.find(dividerTab)) != std::string::npos )){
            std::string token = str.substr(0, pos);

            // only add columns at the start of the document.
            if(newLine == 0) {
                std::vector<double> tmp = {std::stod(token)};
                data.push_back(std::move(tmp));
            }
            // if not the first round then no more rows should be added so data.size is immutable
            else data.at(insertRounds % data.size()).push_back(std::stod(token));

            str.erase(0, pos+dividerTab.length());
            insertRounds++;
        }

    }
    example.close();
    return data;

}

// get grad of 


int main(int argc, char** argv){
    auto data = readTo("graph.example");

    for(int row = 0; row < data.at(0).size(); row++){
        for(auto i : data){
            std::cout << i.at(row) << " ";
        }
        std::cout << "\n\n" << std::flush;
    }


}
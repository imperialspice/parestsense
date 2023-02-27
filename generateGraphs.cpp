//
// Created by middleton on 2/27/23.
//
#include <sciplot/sciplot.hpp>
#include <filesystem>
#include <iostream>
#include <toml.hpp>

struct legend{
    double value;
    std::string currentParameter;
};

double getFloating(toml::value value){
    if(value.is_integer()){
        return (double) value.as_integer();
    }
    else{
        return value.as_floating();
    }



}

int main(){

    // hard code dir
    std::vector<std::string> tomlFiles;
    std::string tomlDir = "./toml";
    for (const auto & entry : std::filesystem::directory_iterator(tomlDir)){
        tomlFiles.push_back(entry.path());
    }

    std::map<double, std::vector<double>> mean;
    std::map<double, std::vector<double>> stddiv;
    std::vector<legend> legends;




    for(const auto& fileName : tomlFiles){
        auto file = toml::parse(fileName);
        auto investigation = toml::find(file, "investigation");
        auto cpi = toml::find(investigation, "current_parameter_investigated");
        auto cpi_value  = toml::find(investigation, "current_parameter_value");
        double cpi_value_cast = 0.0;
        if(cpi_value.is_floating()){
            cpi_value_cast = cpi_value.as_floating();
        }
        else if (cpi_value.is_integer()){
            cpi_value_cast = (double) cpi_value.as_integer();
        }



        legend L{
            cpi_value_cast,
            std::to_string(cpi.as_integer())
        };

        legends.push_back(L);

        auto results = toml::find(file, "results");
        for(auto &i : results.as_table()){
            std::string name(i.first);
            name.replace(0, name.find("_")+1, "");
            int number = std::stoi(name);
            if(i.first.find("mean") != std::variant_npos){
                if(mean.find(number) == mean.end()){
                    mean.insert_or_assign(number, std::vector<double>{getFloating(i.second)});
                }
                else{
                    mean.at(number).push_back(getFloating(i.second));
                }

            }
            else{
                if(stddiv.find(number) == stddiv.end()){
                    stddiv.insert_or_assign(number, std::vector<double>{getFloating(i.second)});
                }
                else{
                    stddiv.at(number).push_back(getFloating(i.second));
                }

            }


            std::cout << "First: " << name << std::endl;
            std::cout << "second: " << i.second << std::endl;
        }
//        for(int i = 0; i < results.size(); i++){
//            auto result = results.(i);
//            if(i % 2 == 0){
//                mean.at(i).push_back(result.as_floating());
//            }
//            else{
//                stddiv.at(i).push_back(result.as_floating());
//            }
//        }

    }

    std::cout << "done" << std::endl;

    sciplot::Plot2D testPlot;
    int x = 0;
    testPlot.fontSize(10);
    testPlot.drawDots(mean.at(x), stddiv.at(x));

    sciplot::Figure fig = {{testPlot}};
    fig.title("All test");
    sciplot::Canvas canvas = {{fig}};
    canvas.size(1000, 1000);
    canvas.save("test.svg");





}
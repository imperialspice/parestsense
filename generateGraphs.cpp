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

struct graph{
    std::string title;
    sciplot::Plot2D plot;
    std::string fileName;
};

double getFloating(toml::value value){
    if(value.is_integer()){
        return (double) value.as_integer();
    }
    else{
        return value.as_floating();
    }



}

int main(int argv, char** argc){
    if(argv != 4){
        std::cout << "Please use the following format to produce graphs.\n";
        std::cout << "./generateGraphs dataDir graphDir species\n";
        std::cout << "dataDir - Directory containing the generated toml files.\n";
        std::cout << "graphDir - Directory to save the produced graphs.\n";
        std::cout << "species - the species, identified by number to produce graphs for\n0 - all species, with any other number only printing that specific species.\n";
        std::cout << std::flush;
        exit(0);
    }

    std::string dataDir = std::string(argc[1]);
    std::string graphDir = std::string(argc[2]);
    std::string species = std::string(argc[3]);

    std::cout << dataDir << graphDir << species << std::endl;

    // hard code dir
    std::vector<std::string> tomlFiles;
    std::string tomlDir = dataDir;
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

    std::cout << "plotting" << std::endl;

    int species_int = std::stoi(species);
    if(species_int > mean.size()){std::cout << "Specific species value too large."; exit(0);}
    int startingInt = species_int > 0 ? species_int:0;
    int endInt = species_int > 0 ? species_int+1:mean.size();


    for(int sp = startingInt; sp < endInt; sp++){
        // get current mean, stddiv and legend vector
        auto currentMean = mean.at(sp);
        auto currentSD = stddiv.at(sp);
        std::vector<double> currentLegend;
        for(auto &i : legends){
            currentLegend.push_back(i.value);
        }

        sciplot::Plot3D currentPlot;
        currentPlot.xlabel("mean");
        currentPlot.ylabel("standard deviation");
        currentPlot.zlabel("species " +legends.at(sp).currentParameter);
        currentPlot.border().clear();
        currentPlot.border().bottomLeftFront();
        currentPlot.border().bottomRightFront();
        currentPlot.border().leftVertical();

        currentPlot.drawDots(currentMean, currentSD, currentLegend);

        sciplot::Figure fig = {{currentPlot}};
        sciplot::Canvas can = {{fig}};
        std::string graphName = graphDir+"/figure_param_"+legends.at(sp).currentParameter+".svg";
        can.save(graphName);

    }
}
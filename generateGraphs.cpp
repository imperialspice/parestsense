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


    std::vector<double> xVector;
    std::vector<double> yVector;

    for(int sp = startingInt; sp < endInt; sp++){
        graph currentGraph = {"", sciplot::Plot2D(), ""};
        for(int x = 0; x < mean.at(sp).size(); x++){
            std::cout << mean.at(sp).at(x) << std::endl;
            xVector.push_back(mean.at(sp).at(x));
            yVector.push_back(stddiv.at(sp).at(x));
            currentGraph.plot.drawDots(xVector, yVector).label(
                    std::to_string(legends.at(sp).value));
            if(currentGraph.title.empty()) currentGraph.title = legends.at(sp).currentParameter;
            xVector.clear(); yVector.clear();
        }
        currentGraph.plot.xlabel("mean");
        currentGraph.plot.ylabel("standard deviation");
        currentGraph.plot.fontSize(14);
        currentGraph.plot.palette("parula");

        currentGraph.fileName = currentGraph.title;

        sciplot::Figure fig = {{currentGraph.plot}};
        fig.title(currentGraph.title);

        sciplot::Canvas canvas = {{fig}};
        canvas.size(1000, 1000);
        std::string fileName = "/output"+(currentGraph.fileName)+"_"+std::to_string(sp)+"_"+".svg";
        canvas.save(graphDir+fileName);

    }





//
//    for(int x = startingInt; x < endInt; x++) {
//        sciplot::Plot2D testPlot;
//        testPlot.xlabel("mean");
//        testPlot.ylabel("standard deviation");
//        testPlot.fontSize(10);
//        testPlot.palette("parula");
//        sciplot::Figure fig = {{testPlot}};
//        // since we are plotting each individial point as a new series we need to iterate over it
//        for(int y = 0; y < mean.at(x).size(); y++){
//            testPlot.drawDots(std::vector<double>{mean.at(x).at(y)},std::vector<double>{stddiv.at(x).at(y)}).label(std::to_string(legends.at(y).value));
//            fig.title(legends.at(y).currentParameter);
//        }
//        //testPlot.drawDots(mean.at(x), stddiv.at(x)).label(std::to_string(legends.at(x).value));
//
//
//        sciplot::Canvas canvas = {{fig}};
//        canvas.size(1000, 1000);
//        std::string fileName = "/output"+ std::to_string(x)+".svg";
//        canvas.save(graphDir+fileName);
//    }




}
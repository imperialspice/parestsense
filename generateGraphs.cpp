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

void plot(std::string species, std::map<double,std::vector<double>> mean, std::map<double, std::vector<double>> stddiv, std::vector<legend> legends, std::string graphDir, std::string currentSpecies){

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
        std::string zlabel;
        zlabel = "species_"+std::to_string(sp);
        currentPlot.zlabel(zlabel);
        currentPlot.border().clear();
        currentPlot.border().bottomLeftFront();
        currentPlot.border().bottomRightFront();
        currentPlot.border().leftVertical();

        currentPlot.drawDots(currentMean, currentSD, currentLegend);
        currentPlot.palette("parula");
        sciplot::Figure fig = {{currentPlot}};
        fig.title(legends.at(sp).currentParameter);

        sciplot::Canvas can = {{fig}};
        can.size(1000, 1000);
        std::string graphName = graphDir+"/figure_param_"+legends.at(sp).currentParameter+"_species_"+std::to_string(sp)+".pdf";
        can.save(graphName);

    }
}

void writeData(std::string species, std::map<double, std::vector<double>> mean, std::map<double, std::vector<double>> stddiv, std::vector<legend> legends, std::string graphDir, std::string currentSpecies){
    std::cout << "Outputing CSV Files." << std::endl;

    int species_int = std::stoi(species);
    if(species_int > mean.size()){std::cout << "Specific species value too large."; exit(0);}
    int startingInt = species_int > 0 ? species_int:0;
    int endInt = species_int > 0 ? species_int+1:mean.size();


    for(int sp = startingInt; sp < endInt; sp++) {
        // get current mean, stddiv and legend vector
        auto currentMean = mean.at(sp);
        auto currentSD = stddiv.at(sp);
        std::vector<double> currentLegend;
        for (auto &i: legends) {
            currentLegend.push_back(i.value);
        }
        std::string graphName = graphDir+"/figure_param_"+legends.at(0).currentParameter+"_species_"+std::to_string(sp)+".csv";
        std::cout << "Outputting: " << graphName << std::endl;
        std::ofstream outputFile(graphName);

        // outputing to csv file from here.
        outputFile << "species\t" << "species value\t"<< "mean\t" << "stddev\n";

        if(currentMean.size() != currentSD.size()) return;


        int size = currentMean.size();
        outputFile.precision(10);

        for(int i = 0; i < size; i++){
            outputFile << legends.at(0).currentParameter << "\t" << currentLegend.at(i) << "\t";
            outputFile << currentMean.at(i) << "\t" << currentSD.at(i) << "\n";
        }
        outputFile << std::flush;
        outputFile.close();
    }

}


int main(int argv, char** argc){
    if(argv != 5){
        std::cout << "Please use the following format to produce graphs.\n";
        std::cout << "./generateGraphs dataDir graphDir species mode\n";
        std::cout << "dataDir - Directory containing the generated toml files.\n";
        std::cout << "graphDir - Directory to save the produced graphs.\n";
        std::cout << "species - the species, identified by number to produce graphs for\n0 - all species, with any other number only printing that specific species.\n";
        std::cout << "mode - either a 0 - svg mode or 1 to output data to csv files.\n";
        std::cout << std::flush;
        exit(0);
    }

    std::string dataDir = std::string(argc[1]);
    std::string graphDir = std::string(argc[2]);
    std::string species = std::string(argc[3]);
    int mode = std::stoi(argc[4]);

    std::cout << dataDir << graphDir << species << std::endl;

    // hard code dir
    std::vector<std::string> tomlFiles;
    std::string tomlDir = dataDir;
    for (const auto & entry : std::filesystem::directory_iterator(tomlDir)){
        tomlFiles.push_back(entry.path());
    }


//    std::map<double, std::vector<double>> mean;
//    std::map<double, std::vector<double>> stddiv;

    std::map<int,std::map<double, std::vector<double>>> meanList;
    std::map<int,std::map<double, std::vector<double>>> stddivList;

    std::map<int,std::vector<legend>> legendList;




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



        if(legendList.find(cpi.as_integer()) == legendList.end()){
            legendList.insert_or_assign(cpi.as_integer(), std::vector<legend>{});
        }
        auto legends = &legendList.find(cpi.as_integer())->second;
        legend L{
                cpi_value_cast,
                std::to_string(cpi.as_integer())
        };
        legends->push_back(L);

        auto results = toml::find(file, "results");
        for(auto &i : results.as_table()){
            std::string name(i.first);
            name.replace(0, name.find("_")+1, "");
            int number = std::stoi(name);



            if(i.first.find("mean") != std::variant_npos){

                if(meanList.find(cpi.as_integer()) == meanList.end()){
                    meanList.insert_or_assign(cpi.as_integer(), std::map<double, std::vector<double>>{});
                }
                auto mean = &meanList.find(cpi.as_integer())->second;


                if(mean->find(number) == mean->end()){
                    mean->insert_or_assign(number, std::vector<double>{getFloating(i.second)});
                }
                else{
                    mean->at(number).push_back(getFloating(i.second));
                }

            }
            else{

                if(stddivList.find(cpi.as_integer()) == stddivList.end()){
                    stddivList.insert_or_assign(cpi.as_integer(), std::map<double, std::vector<double>>{});
                }
                auto stddiv = &stddivList.find(cpi.as_integer())->second;


                if(stddiv->find(number) == stddiv->end()){
                    stddiv->insert_or_assign(number, std::vector<double>{getFloating(i.second)});
                }
                else{
                    stddiv->at(number).push_back(getFloating(i.second));
                }

            }
        }




        //


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
    // TODO: Split lists so that the parameters for one file are always the same.
    // use the key from mean to lookup everywhere else
    for(auto it = meanList.begin(); it != meanList.end(); it++){
        auto mean = it->second;
        auto stddiv = stddivList.find(it->first);
        auto legends = legendList.find(it->first);
        std::string paramName = "speices_" + std::to_string(it->first);

        if(mode == 0){
            plot(species, mean,  stddiv->second, legends->second, graphDir, paramName);
        }
        else{
            writeData(species, mean, stddiv->second, legends->second, graphDir, paramName);
        }



    }


}
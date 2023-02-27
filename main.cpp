#include <iostream>
#include <string>
#include <map>
#include <random>
#define UUID_SYSTEM_GENERATOR 1 // enable system random generator

#include <uuid.h>
#include <cstdlib>
#include <toml.hpp>



//
double stepSize = 0.1;
std::string optionLinear = "linear";
std::string optionallCompare = "every";

struct Options{
  int mode = 0; // default linear, 0 linear, 1 every
  std::string finalDir = "";
  std::string outputDir = "";
  std::string paramDir = "";
  std::string tomlDir = "";
};
Options options;

std::map<std::string, std::vector<double>> globalData;


// return the number of simulations to get a full map
int countItterations(const std::map<std::string, std::vector<double>>& data){
    int steps = 1;

    // work out the number of steps
    for(auto i : data){
        if(i.second.at(1) == 0 || i.second.at(2) == 0){
            steps += 0;

        }
        else{
            if(options.mode == 0){
                steps = steps + i.second.at(3);
            }
            else if(options.mode == 1){

                steps = steps * i.second.at(3);
            }


        }
    }
    return steps;
}

void writeConfig(std::string fileName, std::vector<double> output){
    // there needs to be the number of param values and three zeros a the top of the file
    double size = output.size();
    std::ofstream paramDat("t/"+fileName);
    paramDat << size << "\n" << "0\n" << "0\n" << "0\n" << std::flush;
    for(auto number : output){
        paramDat << std::setprecision(5) << std::scientific << number << "\n";
    }
    paramDat.close();
}

int execute(std::string fileParam, std::string fileResults){
    char * buffer;
    buffer = new char[256];
    snprintf(buffer, 256, "./parest -p %s -d ddat.in -z %s", fileParam.c_str(), fileResults.c_str());
    int status = system(buffer);
    std::cout << "Status Returned: " << status << std::endl;
   return status;

}

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

std::vector<double> mean(std::vector<std::vector<double>> data){
    std::vector<double> rtn;
    for(auto i : data){
        double sum = std::accumulate(i.begin(), i.end(), 0.0);
        rtn.push_back((sum/i.size()));
    }
    return rtn;
}


std::vector<double> std_div(std::vector<std::vector<double>> data){
    std::vector<double> acc;
    acc.assign(data.size(), 0); // reserve
    auto means = mean(data);
    for(int i = 0; i < data.size(); i++){
        std::for_each(data.at(i).begin(), data.at(i).end(), [&](const double &d){
            acc[i] += (d - means[i]) * (d - means[i]);
        });
        acc[i] = sqrt(acc[i] / (data[i].size() -1));
    }
    return acc;
}

void writeToTOML(std::string fileName, std::vector<double> &params, std::vector<double> mean, std::vector<double> stddiv, int currentParameter){
    // output to a file based on the key of the param list, with the mean and stddiv data too

    std::ofstream tomlOutput(fileName);
    // write the param data and fileName internally
    fileName.replace(fileName.find('/'), 1, "_");
    tomlOutput.precision(10);
    tomlOutput << "[" << fileName << "]" << "\n" << std::flush;
    tomlOutput << "[investigation]" << "\n" << std::flush;
    tomlOutput << "current_parameter_investigated = " << currentParameter << std::endl;
    tomlOutput << "current_parameter_value = " << (double)params.at(currentParameter) << std::endl;
    tomlOutput << "[params]\n" << std::flush;
    for(int i = 0; i < params.size(); i++){
        tomlOutput << "param_" << i << " = " << params.at(i) << std::endl;
    }
    tomlOutput << "[results]\n" << std::flush;
    for(int i = 0; i < mean.size(); i++){
        tomlOutput << "mean_" << i << " = " << mean.at(i) << std::endl;
        tomlOutput << "stddiv_" << i << " = " << stddiv.at(i) << std::endl;
    }
    tomlOutput.close();

};

void readResults(std::string fileOutput, std::string fileResults, std::string tomlResults, std::vector<double> &params, int currentParameter){

    auto results = readTo(fileResults);
    std::ofstream finalOutput(fileOutput);
    for(auto i : params){
        finalOutput << i << "\t";
    }
    finalOutput << "\n" << std::flush;
    for(int row = 0; row < results.at(0).size(); row++){
        for(auto i : results){
            finalOutput << i.at(row) << "\t";
        }
        finalOutput << "\n" << std::flush;
    }
    // add data representations here like the sd and mean calculations
    auto means = mean(results);
    for(auto i : means){
        finalOutput << i << "\t";
    }
    finalOutput << "\n";
    auto stddev = std_div(results);
    for(auto i : stddev){
        finalOutput << i << "\t";
    }
    finalOutput << "\n";


    finalOutput.close();

    writeToTOML(tomlResults, params, means, stddev, currentParameter);

}





void linear(const std::map<std::string, std::vector<double>> data, int currentCount, int range, int totalCount){

    int count = currentCount;
    double position = 0;
    double step;
    double lLimit;
    double hLimit;
    int listPosition = 0;
    int currentParameter = 0;
    for(auto i : data){
        listPosition++;
        if((count - i.second.at(3)) >= 0) {count = count - i.second.at(3); currentParameter++; continue;}
        position = i.second.at(0) + i.second.at(2)*count;
        lLimit = i.second.at(0);
        hLimit = i.second.at(1);
        step = i.second.at(2);
        break;
    }

    std::cout << "Current Parameter Being Checked: " << currentParameter << std::endl;

    // only reading from data currently so it would be safe to iterate over to find and replace the values

    // check if currentCount or range is higher then use the higher lowest one to ensure that is isn't run more than required.
    int currentTarget = totalCount > range ? range : totalCount;

    for(int x = 0; x < currentTarget; x++){
        double currentValue = position + x*step;
        if(currentValue >= hLimit){
            linear(data, count+x, currentTarget-x, totalCount);
            break;
        }

        std::vector<double> writeList;
        int localPosition = 0;
        for(auto i : data){
            localPosition++;
            if(localPosition == listPosition){
                writeList.insert(writeList.end(), currentValue);
                continue;
            }
            writeList.insert(writeList.end(), i.second.at(0));
        }

        // generate uuid;
        uuids::uuid const fileID = uuids::uuid_system_generator{}();
        writeConfig(uuids::to_string(fileID), writeList);
        std::string paramFile = options.paramDir+"/"+uuids::to_string(fileID);
        std::string outputFile = options.outputDir+"/"+uuids::to_string(fileID);
        std::string finalFile = options.finalDir+"/"+uuids::to_string(fileID);
        std::string tomlFile = options.tomlDir+"/"+uuids::to_string(fileID);

        execute(paramFile, outputFile);
        readResults(finalFile, outputFile, tomlFile, writeList, currentParameter);
    }

}



// data then convert
void run(const std::map<std::string, std::vector<double>>& data, int currentCount, int range, int totalCount){
    // linear position should be somewhere in the list of values;
    if(options.mode == 0) linear(data, currentCount, range, totalCount);


}

void setupOptions(const toml::table& textOptions){
    if(textOptions.empty()) return;
    if(textOptions.at("method").as_string() == optionallCompare){
        options.mode = 1;
    }
    if(textOptions.at("output_dir").is_string()){
        options.outputDir = textOptions.at("output_dir").as_string();
    }
    if(textOptions.at("param_dir").is_string()){
        options.paramDir = textOptions.at("param_dir").as_string();
    }
    if(textOptions.at("final_dir").is_string()){
        options.finalDir = textOptions.at("final_dir").as_string();
    }
    if(textOptions.at("toml_dir").is_string()){
        options.tomlDir = textOptions.at("toml_dir").as_string();
    }


}


int main() {
    const auto data = toml::parse("test.toml");


    const auto configFrame = toml::find<toml::table>(data, "options");
    const auto dataFrame = toml::find<toml::table>(data, "spec");

    // configure options frame
    setupOptions(configFrame);


    for(auto i : dataFrame){
        //std::cout << "title: " << i.first << ", " << i.second << std::endl;
        std::vector<double> tmp;

        if(i.second.is_floating()){
            tmp = { (i.second.as_floating()), 0, 0, 0};

        }
        if(i.second.is_array()){
            if(i.second.as_array().size()==2){ // 2 so standard step
                auto t = i.second.as_array();
                /* t {
                    *      starting value
                    *      largest value
                    * }
                    */
                tmp = { ( t.at(0).as_floating()),
                        ( t.at(1).as_floating()), stepSize,
                       (( ( t.at(1).as_floating())-  ( t.at(0).as_floating()))/stepSize),
                        ( t.at(0).as_floating())};
                /* tmp {
                *      smallest value
                *      largest value
                *      stepsize
                *      range
                *      inital value
                *  }
                */
            }
            else if(i.second.as_array().size()==3){ // 3 with step value
                auto t = i.second.as_array();
                /* t {
                    *      starting value
                    *      largest value
                    *      step size
                    * }
                    */
                tmp = { ( t.at(0).as_floating()),
                        ( t.at(1).as_floating()),
                        ( t.at(2).as_floating()),
                       ( ( t.at(1).as_floating()) -  ( t.at(0).as_floating()))/( ( t.at(2).as_floating())),
                        ( t.at(0).as_floating())};
                /* tmp {
                    *      smallest value
                    *      largest value
                    *      stepsize
                    *      range
                    *      inital value
                    *  }
                    */
            }
            else if(i.second.as_array().size()==4){
                auto t = i.second.as_array();
                /* t {
                 *      starting value
                 *      reduction value
                 *      increase value
                 *      step size
                 * }
                 */
                tmp = { ( t.at(0).as_floating()) -  ( t.at(1).as_floating()),
                        ( t.at(0).as_floating()) +  ( t.at(2).as_floating()),
                        ( t.at(3).as_floating()),
                        (((t.at(0).as_floating() + (t.at(2).as_floating())) - (t.at(0).as_floating() - t.at(1).as_floating()))/t.at(3).as_floating()),
                        (t.at(0).as_floating())};
                /* tmp {
                 *      smallest value
                 *      largest value
                 *      stepsize
                 *      range
                 *      inital value
                 *  }
                 */

            }
            else{
                std::cout << "Invalid Config File" << std::endl;
                exit(-1);
            }

        }
        globalData.insert_or_assign(i.first.data(), tmp);
    }

    for(auto i : globalData){
        std::cout << "Name: " << i.first<< std::endl;
        std::cout << "I: " << i.second.at(0) << " E: " << i.second.at(1) << " STEP: " << i.second.at(2) << std::endl;
    }

    // sensitivity
    int count = countItterations(globalData);
    std::cout << "Count: " << count << std::endl;
    run(globalData, 0, 1000, count);
    return 0;
}

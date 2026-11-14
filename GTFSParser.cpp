#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <format>

int loadFile(std::ifstream& stream,std::string file);
void searchStopTimes(std::string stop_id);

std::ifstream stop_file;
std::ifstream stop_times_file;

int main(int argc, char* argv[])
{
    std::chrono::time_point now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);

    std::cout << __cplusplus << std::endl;
    std::string busStop;
    busStop = "PLAZA ELÍPTICA 4";

    if (argc != 2) 
    { 
        std::cerr << "Please add only the file as a parameter";
    }
    
    if(loadFile(stop_file ,std::string(argv[1]) + "/stops.txt") == 1){return 1;}
    if(loadFile(stop_times_file ,std::string(argv[1]) + "/stop_times.txt") == 1){return 1;}

        std::string s;
        while (std::getline(stop_file, s))
        {
            if(s.find(busStop) != std::string::npos) // Finds the stop 
            {
            std::cout << s << std::endl;
            std::string stop_id = s.substr(0,s.find_first_of(',') + 1);
            searchStopTimes(stop_id);
            }
        } 

        stop_file.close();
        stop_times_file.close();
        
        return 0;
    }


int loadFile(std::ifstream& stream,std::string file)
{
    stream.open(file);
    if (!stream.is_open())
    {
        std::cerr << "Error while opening the file" << file;
        return 1;
    }
    else{return 0;}
}

void searchStopTimes(std::string stop_id)
{
        std::string search;
        while (std::getline(stop_times_file, search))
        {
            if(search.find(stop_id) != std::string::npos) // Finds the stop 
            {
            //std::cout << search << std::endl;
            }
        } 
}
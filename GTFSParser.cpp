#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <format>

int loadFile(std::ifstream& stream,std::string file);
void searchStopTimes(std::string stop_id);

std::ifstream stop_file;
std::ifstream stop_times_file;


    int hour = 0;

int main(int argc, char* argv[])
{
   // std::chrono::time_point now = std::chrono::system_clock::now();
   // std::chrono::hh_mm_ss time_of_day{now - std::chrono::floor<std::chrono::days>(now)}; 
   // std::cout << std::format("{:%T}", time_of_day) << std::endl;

    auto now = std::chrono::system_clock::now(); 
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm tm = *std::localtime(&time);

    std::stringstream ss;

    ss << std::put_time(&tm, "%H"); 


    std::string date_str = ss.str();
    std::string busStop;
    busStop = "PLAZA ELÍPTICA 4";
    hour = stoi(date_str);
    std::cout << hour << std::endl;
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
            std::string stop_id = s.substr(0,s.find_first_of(',')); //Find_first_of due to stop_id being the primary key
            std::cout << stop_id << std::endl;
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
            if(search.find(stop_id) != std::string::npos) 
            {
                int lowerLimit = 0;
                int upperLimit = 0;
            
            for(int i = 0; i <= 1; i++) //Arrival_time is the 2nd field
            { 
                lowerLimit = upperLimit;
                upperLimit = search.find_first_of(',', upperLimit + 1);
            } 
            std::string arrival_time = search.substr(lowerLimit + 1,(upperLimit - lowerLimit) - 1);
            if(stoi(arrival_time.substr(0,2)) >= hour && stoi(arrival_time.substr(0,2)) < hour + 1){ std::cout << arrival_time << std::endl;}
            }
        } 
}
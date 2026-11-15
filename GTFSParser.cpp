#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <format>
#include <vector>
#include <algorithm>

int loadFile(std::ifstream& stream,std::string file);
void searchStopTimes(std::string stop_id);

std::ifstream stop_file;
std::ifstream stop_times_file;

    std::vector<time_t> time_vector;
    std::string date_str;
    int hour = 0;
    int min = 0;
    int sec = 0;

int main(int argc, char* argv[])
{


    std::time_t time_now = std::time(nullptr);

    std::tm tm = *std::localtime(&time_now);

    std::stringstream ss;

    ss << std::put_time(&tm, "%H:%M:%S"); 


    date_str = ss.str();
    std::string busStop;
    busStop = "PLAZA ELÍPTICA 4";
    std::cout << date_str << std::endl;
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
            
            for(int i = 0; i <= 2; i++) //Arrival_time is the 2nd field
            { 
                lowerLimit = upperLimit;
                upperLimit = search.find_first_of(',', upperLimit + 1);
            } 
            std::string departure_string = search.substr(lowerLimit + 1,(upperLimit - lowerLimit) - 1);

            struct std::tm tm1;
            std::istringstream ss1(departure_string);
            ss1 >> std::get_time(&tm1, "%H:%M:%S");
            std::time_t departure_time = mktime(&tm1);

            struct std::tm tm2;
            std::istringstream ss2(date_str);
            ss2 >> std::get_time(&tm2, "%H:%M:%S");
            std::time_t date_time = mktime(&tm2);
            
            //std::cout<<date_time << "/" << departure_time<< " = " << difftime(departure_time,date_time) << std::endl;
            if(difftime(departure_time,date_time) > 0)
            {
                std::string travel_id = search.substr(0,search.find_first_of(',')); 
                time_vector.push_back(departure_time);
            }
            }
        } 

        sort(time_vector.begin(), time_vector.end());

            for(int i = 0; i < time_vector.size(); i++)
            {
                std::tm tm = *std::localtime(&time_vector[i]);
                std::cout << std::put_time(&tm, "%H:%M:%S") << std::endl;
            }
}
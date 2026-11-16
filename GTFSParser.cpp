#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <format>
#include <vector>
#include <algorithm>
#include <chrono>
using namespace std;
using namespace std::chrono;

int loadFile(std::ifstream& stream,std::string file);
void searchStopTimes(std::string stop_id);

std::ifstream stop_file;
std::ifstream stop_times_file;

    std::vector<seconds> time_vector;
    std::string date_str;
    std::chrono::time_point time_now = std::chrono::system_clock::now();
    
    int hour = 0;
    int min = 0;
    int sec = 0;

int main(int argc, char* argv[])
{

    const auto* tz = std::chrono::locate_zone("Europe/Madrid");
    
    // Convert system_clock time_point to zoned_time (respects DST)
    std::chrono::zoned_time local_time{tz, time_now};

     date_str = std::format("{:%H:%M:%S}", time_now);
    std::cout << date_str << '\n';

    std::string busStop = "PLAZA ELÍPTICA 4";
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
                //std::cout << search << std::endl;
                int lowerLimit = 0;
                int upperLimit = 0;
            
            for(int i = 0; i <= 2; i++) //Arrival_time is the 2nd field
            { 
                lowerLimit = upperLimit;
                upperLimit = search.find_first_of(',', upperLimit + 1);
            } 
            std::string departure_string = search.substr(lowerLimit + 1,(upperLimit - lowerLimit) - 1);

            std::istringstream stream (departure_string);
            std::chrono::seconds departure_time;
            std::chrono::from_stream (stream, "%H:%M:%S", departure_time);
            

            std::istringstream stream_now (date_str);
            std::chrono::seconds seconds_now;
            std::chrono::from_stream (stream_now, "%H:%M:%S", seconds_now); 
            
            //std::cout << seconds_now << "/" << departure_time <<std::endl;
            if( seconds_now <= departure_time)
            {
                std::string travel_id = search.substr(0,search.find_first_of(',')); 
                //std::cout << "Voy a insertar: " << std::format("{:%H:%M:%S}", departure_time) << std::endl;
                time_vector.push_back(departure_time);
            }
            }
        } 
        std::cout << time_vector.size() << std::endl;
        sort(time_vector.begin(), time_vector.end());

        std::string first_string = std::format("{:%H:%M:%S}", time_vector[0]);
        std::cout << first_string << '\n';

       std::string second_string = std::format("{:%H:%M:%S}", time_vector[1]);
        std::cout << second_string << '\n';

        std::string third_string = std::format("{:%H:%M:%S}", time_vector[2]);
        std::cout << third_string << '\n';
        
        //std::string first_string, second_string, third_string= "prueba";
        std::cout << first_string << " " << second_string << " " << third_string << std::endl;
        
       stop_times_file.clear();
       stop_times_file.seekg(5,std::ios::beg); //Returns pointer to start of file.
        std::string search2;
       while (std::getline(stop_times_file, search2))
        {
            if(search2.find(stop_id) != std::string::npos && (search2.find(first_string) != std::string::npos || search2.find(second_string) != std::string::npos ||  search2.find(third_string) != std::string::npos)) 
            {
                //std::cout << search2 << std::endl;
            }
        }
            
}
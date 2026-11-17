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

struct departure
{   
    string trip_id;
    std::chrono::seconds departure_time;
    string route;
    string desc;
};

int loadFile(std::ifstream& stream,std::string file);
void searchStopTimes(std::string stop_id);
bool departure_sorter(departure const& lhs, departure const& rhs);
departure searchDepartureInfo(departure departure);

std::ifstream stop_file;
std::ifstream stop_times_file;
std::ifstream trips_file;


std::string date_str;
std::chrono::zoned_time time_now{std::chrono::current_zone(), std::chrono::system_clock::now()};

int hour = 0;
int min = 0;
int sec = 0;

    std::vector<departure> time_vector;

int main(int argc, char* argv[])
{
    date_str = std::format("{:%H:%M:%S}", time_now);

    std::string busStop = "PLAZA ELÍPTICA 4";
    std::cout << date_str << std::endl;
    if (argc != 2) 
    { 
        std::cerr << "Please add only the file as a parameter";
    }
    
    if(loadFile(stop_file ,std::string(argv[1]) + "/stops.txt") == 1){return 1;}
    if(loadFile(stop_times_file ,std::string(argv[1]) + "/stop_times.txt") == 1){return 1;}
    if(loadFile(trips_file ,std::string(argv[1]) + "/trips.txt") == 1){return 1;}

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

        departure first_departure = time_vector[0];
        departure second_departure = time_vector[1];
        departure third_departure = time_vector[2];

        first_departure = searchDepartureInfo(first_departure);
        second_departure = searchDepartureInfo(second_departure);
        third_departure = searchDepartureInfo(third_departure);

        std::cout << first_departure.route << "/" << first_departure.trip_id << "/" << first_departure.desc << "/" << std::format("{:%H:%M:%S}", first_departure.departure_time) << std::endl;
        std::cout << second_departure.route << "/" << second_departure.trip_id << "/" << second_departure.desc << "/" << std::format("{:%H:%M:%S}", second_departure.departure_time) << std::endl;
        std::cout << third_departure.route << "/"<< third_departure.trip_id << "/" << third_departure.desc << "/" << std::format("{:%H:%M:%S}", third_departure.departure_time) << std::endl;


        stop_file.close();
        stop_times_file.close();
        trips_file.close();
        
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
            if(search.find(stop_id + ',') != std::string::npos) 
            {
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

            if( seconds_now <= departure_time)
            {
                departure departure_from_line(search.substr(0,search.find_first_of(',')),departure_time,"","");
                time_vector.push_back(departure_from_line);
            }
            }
        } 

        sort(time_vector.begin(),time_vector.end(),&departure_sorter);    
}

bool departure_sorter(departure const& lhs, departure const& rhs) {return lhs.departure_time < rhs.departure_time;}

departure searchDepartureInfo(departure departure)
{
        trips_file.clear();
        trips_file.seekg(5,std::ios::beg); //Returns pointer to start of file.
        std::string search;
        while (std::getline(trips_file, search))
        {
            if(search.find( departure.trip_id + ',') != std::string::npos)
            {
               departure.route = search.substr(0,search.find_first_of(','));

                int lowerLimit = 0;
                int upperLimit = 0;

            for(int i = 0; i <= 3; i++) 
            { 
                lowerLimit = upperLimit;
                upperLimit = search.find_first_of(',', upperLimit + 1);
            } 

            departure.desc = search.substr(lowerLimit + 1,(upperLimit - lowerLimit) - 1);

            }
        }
        //std::cout << departure.route << "/" <<departure.desc << "/" << std::format("{:%H:%M:%S}", departure.departure_time) << std::endl;
        return departure;
}
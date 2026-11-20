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

int loadFile(std::ifstream &stream, std::string file);
void searchStopTimes(std::string stop_id);
bool departure_sorter(departure const &lhs, departure const &rhs);
departure searchDepartureInfo(departure departure);
bool checkIfCorrectDay(string trip_id);
bool checkExceptionDate(string service_id, int dayToCheck);

std::ifstream stop_file;
std::ifstream stop_times_file;
std::ifstream trips_file;
std::ifstream routes_file;
std::ifstream calendar_file;
std::ifstream calendar_dates_file;

std::string date_str;
std::chrono::zoned_time time_now{std::chrono::current_zone(), std::chrono::system_clock::now()};
int dayOfWeek = stoi(std::format("{:%u}", time_now));
string date_now = std::format("{:%Y%m%d}", time_now);

int hour = 0;
int min = 0;
int sec = 0;

std::vector<departure> time_vector;

int main(int argc, char *argv[])
{
    date_str = std::format("{:%X}", time_now);
    std::string busStop = "PLAZA ELÍPTICA 4";
    std::cout << "Current Date: " << std::format("{:%F:%T}", time_now) << std::endl;
    std::cout << "Bus stop: " << busStop << std::endl;
    std::cout << std::endl;
    if (argc != 2)
    {
        std::cerr << "Please add only one directory as a parameter";
    }

    // We load every file we are going to need
    if (loadFile(stop_file, std::string(argv[1]) + "/stops.txt") == 1)
    {return 1;}
    if (loadFile(stop_times_file, std::string(argv[1]) + "/stop_times.txt") == 1)
    {return 1;}
    if (loadFile(trips_file, std::string(argv[1]) + "/trips.txt") == 1)
    {return 1;}
    if (loadFile(routes_file, std::string(argv[1]) + "/routes.txt") == 1)
    {return 1;}
    if (loadFile(calendar_file, std::string(argv[1]) + "/calendar.txt") == 1)
    {return 1;}
    if (loadFile(calendar_dates_file, std::string(argv[1]) + "/calendar_dates.txt") == 1)
    {return 1;}

    std::string s;
    while (std::getline(stop_file, s)) //Read all of stop.txt, line by line, to find the bus stop which matches the text
    {
        if (s.find(busStop) != std::string::npos) 
        {
            std::string stop_id = s.substr(0, s.find_first_of(',')); // Find_first_of only once due to stop_id being the primary key
            searchStopTimes(stop_id);  // Function call to search of the next departures on the found bus stop
        }
    }

    //This gets the first 3 departures, sorted in the time_vector
    departure first_departure = time_vector[0];
    departure second_departure = time_vector[1];
    departure third_departure = time_vector[2];

    //Function call to get the related info
    first_departure = searchDepartureInfo(first_departure);
    second_departure = searchDepartureInfo(second_departure);
    third_departure = searchDepartureInfo(third_departure);

    //This transform the current time to seconds so it may display how many minutes/seconds until arrival.
    std::istringstream stream_now(date_str);
    std::chrono::seconds seconds_now;
    std::chrono::from_stream(stream_now, "%H:%M:%S", seconds_now);

    //TUI
    std::cout << "Next Departures: " << std::endl;
    std::cout << first_departure.route << " / " << first_departure.desc << " / " << std::format("{:%H:%M:%S}", first_departure.departure_time) << ". Time until arrival: " << std::format("{:%Mm :%Ss}", first_departure.departure_time - seconds_now) << std::endl;
    std::cout << second_departure.route << " / " << second_departure.desc << " / " << std::format("{:%H:%M:%S}", second_departure.departure_time) << ". Time until arrival: " << std::format("{:%Mm :%Ss}", second_departure.departure_time - seconds_now) << std::endl;
    std::cout << third_departure.route << " / " << third_departure.desc << " / " << std::format("{:%H:%M:%S}", third_departure.departure_time) << ". Time until arrival: " << std::format("{:%Mm :%Ss}", third_departure.departure_time - seconds_now) << std::endl;

    //We close every file.
    stop_file.close();
    stop_times_file.close();
    trips_file.close();
    routes_file.close();
    calendar_file.close();
    calendar_dates_file.close();
    return 0;
}

int loadFile(std::ifstream &stream, std::string file) //Function loads file to read them through
{
    stream.open(file);
    if (!stream.is_open())
    {
        std::cerr << "Error while opening the file" << file;
        return 1;
    }
    else
    {
        return 0;
    }
}

void searchStopTimes(std::string stop_id) // Function searches every next departure time and sorts them into time_vector
{
    std::string search;
    while (std::getline(stop_times_file, search)) //Read stop_times.txt line by line to find the departure times.
    {
        if (search.find(stop_id + ',') != std::string::npos)
        {
            int lowerLimit = 0;
            int upperLimit = 0;

            for (int i = 0; i <= 2; i++) // Arrival_time is the 2nd field so we use find_first_of two times to move the pointer.
            {
                lowerLimit = upperLimit;
                upperLimit = search.find_first_of(',', upperLimit + 1);
            }
            std::string departure_string = search.substr(lowerLimit + 1, (upperLimit - lowerLimit) - 1);

            //This converts the departure_time and the current system time to chrono::seconds so we may compare it.
            std::istringstream stream(departure_string);
            std::chrono::seconds departure_time;
            std::chrono::from_stream(stream, "%H:%M:%S", departure_time);

            std::istringstream stream_now(date_str);
            std::chrono::seconds seconds_now;
            std::chrono::from_stream(stream_now, "%H:%M:%S", seconds_now);

            string trip_id = search.substr(0, search.find_first_of(','));

            if (seconds_now <= departure_time && checkIfCorrectDay(trip_id)) //If the departure time is after the current time and the departure time it comes from is from a line working today.
            {
                departure departure_from_line(trip_id, departure_time, "", ""); //This create a departure struct so we can sort them out later without loosing trip_id
                time_vector.push_back(departure_from_line); 
            }
        }
    }

    sort(time_vector.begin(), time_vector.end(), &departure_sorter); //This sorts the time_vector using the departure_sorter function.
}

bool departure_sorter(departure const &lhs, departure const &rhs) { return lhs.departure_time < rhs.departure_time; }  //Predicate function used to sort time_vector

departure searchDepartureInfo(departure departure)
{
    trips_file.clear();
    trips_file.seekg(0, std::ios::beg); // Returns pointer to start of file, needed if we call the function multiple times
    std::string search;
    while (std::getline(trips_file, search))  //Read trips.txt line by line and find the route related to the trip_id
    {
        if (search.find(departure.trip_id + ',') != std::string::npos)
        {
            departure.route = search.substr(0, search.find_first_of(','));
        }
    }
    routes_file.clear();
    routes_file.seekg(0, std::ios::beg);// Returns pointer to start of file, needed if we call the function multiple times
    std::string search2;
    while (std::getline(routes_file, search2)) //Now that we have the route, we can search routes.txt to find the related info about the departure.
    {
         if ((search2.substr(0, search2.find_first_of(','))).compare(departure.route) == 0)
        {
            int lowerLimit = 0;
            int upperLimit = 0;

            for (int i = 0; i <= 2; i++)
            {
                lowerLimit = upperLimit;
                upperLimit = search2.find_first_of(',', upperLimit + 1);
            }
            departure.route = search2.substr(lowerLimit + 1, (upperLimit - lowerLimit) - 1);

            lowerLimit = upperLimit;
            upperLimit = search2.find_first_of(',', upperLimit + 1);
            departure.desc = search2.substr(lowerLimit + 1, (upperLimit - lowerLimit) - 1); //These two fields are side by side, it doesn't need a second loop.
        }
    }
    
    return departure;
}

bool checkIfCorrectDay(string trip_id) //This function checks what services are operative.
{
    string service_id;
    trips_file.clear();
    trips_file.seekg(0, std::ios::beg); // Returns pointer to start of file.
    std::string search;
    while (std::getline(trips_file, search)) //First, we check trips.txt to find the related service_id.
    {
        if (search.find(trip_id + ',') != std::string::npos)
        {
            int lowerLimit = 0;
            int upperLimit = 0;

            for (int i = 0; i < 2; i++) 
            {
                lowerLimit = upperLimit;
                upperLimit = search.find_first_of(',', upperLimit + 1);
            }
            service_id = search.substr(lowerLimit + 1, (upperLimit - lowerLimit) - 1);
        }
    }

    int dayToCheck = 0;

    calendar_file.clear();
    calendar_file.seekg(0, std::ios::beg); // Returns pointer to start of file.
    std::string search2;
    while (std::getline(calendar_file, search2)) //With  the service_id we can check if the service is available or not.
    {
        if (search2.find(service_id + ',') != std::string::npos)
        {
            int lowerLimit = 0;
            int upperLimit = 0;

            for (int i = 0; i < dayOfWeek + 1; i++) // Due to being categorized in day-by-day fields, we need to use which day of the week is on a scale of 1 to 7.
            {
                lowerLimit = upperLimit;
                upperLimit = search2.find_first_of(',', upperLimit + 1);
            }
            dayToCheck = stoi(search2.substr(lowerLimit + 1, (upperLimit - lowerLimit) - 1));
        }
    }
    return checkExceptionDate(service_id, dayToCheck); //Finally, this calls to check for any special days, such as holidays.
}

bool checkExceptionDate(string service_id, int dayToCheck) //This function checks if the service is operative, keeping in mind exception days.
{
    trips_file.clear();
    trips_file.seekg(0, std::ios::beg); // Returns pointer to start of file.
    std::string search;
    while (std::getline(calendar_dates_file, search)) //It searches calendar_dates.txt to find if there is a match of service_id and today's date.
    {
        if (search.find(service_id + ',') != std::string::npos && search.find(date_now + ',') != std::string::npos)
        {
             int exceptionService = stoi(search.substr(search.find_last_of(',') + 1,search.size()-1));
            if (exceptionService == 1) //It uses .back() due to the exception field being always the last character.
            {
                return dayToCheck != 1; //There exist an added service.
            }
            else
                return dayToCheck != 0; //There exist a removed service.
        }
    }
    return dayToCheck == 1; //Return if the service is available.
}

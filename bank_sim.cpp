/*
 * Description: Simulation of bank queue using queue data structures *
 */

// includes
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <queue>

using namespace std;

struct Event {
    char type;
    int time;
    int length;
};

struct timeCompare {
    bool operator()(const Event& e1, const Event& e2)
    {
        return e1.time > e2.time;
    }
};

struct EventList {
    void fill(istream& is);
    void simulate();
    void proccessArrival(Event newEvent, queue<Event>& bankQueue);
    void processDeparture(Event newEvent, queue<Event>& bankQueue);
    int currentTime;
    int totalWait;
    bool tellerAvailable;
    priority_queue<Event, vector<Event>, timeCompare> eventListPQueue;

};

int main(int argc, char** argv) // argc is the # of arguments in our cmd line; argv is the point to the array of pointers
{
    EventList eventList;
    // command-line parameter 
    // also fills the event list with the input data

    char* progname = argv[0];

    switch (argc)
    {
        case 1:
            eventList.fill(cin);
            break;
        case 2:
        {
            ifstream ifs(argv[1]);
            if (!ifs)
            {
               cerr << progname << ": couldn't open " << argv[1] << endl;
               return 1;
            }
            eventList.fill(ifs);
            break;
        }
        default:
            cerr << "Usage: " << progname << " [datafile]\n";
            return 2;
    }

    eventList.simulate();
}

// EventList::fill
void EventList::fill(istream& is)
{
    int time; // time of arrival
    int length; // length of transaction

    // helps catch errors in the data file
    int prevTime = -1;
    int person = 1;

    // reading the data
    while( is >> time >> length)
    {
        // if elements in the data file are out of order
        if (time < prevTime)
        {

            cerr <<  "Customer #" << person << " is out of order\n" <<
            "Time: " << time << " Previous Time: " << prevTime << endl;
            exit(-1); // exits out of the program
        }

        // creating arrival events
        Event newArrivalEvent = {'A', time, length};

        // pushing to priortiy queue
        eventListPQueue.push(newArrivalEvent);

        prevTime = time;
        person++;
    }
}

// EventList::simulate
void EventList::simulate()
{
    queue<Event> bankQueue; // holds the people who are waiting in line while the teller is not available

    int totalPeople = 0;
    totalWait = 0; // declared in EventList
    float average;

    tellerAvailable = true; // declared in tellerAvailable

    while(!eventListPQueue.empty())
    {
        Event newEvent = eventListPQueue.top();
        currentTime = newEvent.time;
        // if event is an arrival event
        if (newEvent.type == 'A')
        {
            proccessArrival(newEvent, bankQueue);
            // counting the # of people
            totalPeople++;
        }
        // if event is a departure event
        else
            processDeparture(newEvent, bankQueue);
    }

    // stats
    average = totalWait / static_cast<float>(totalPeople); // average
    cout << endl << "Final Statistics:" << endl;
    cout << "Total number of people processed: " << totalPeople << endl;
    cout << setprecision(2) << fixed;
    cout << "Average amount of time spent waiting: " << average << endl;
}

// EventList::proccessArrival
void EventList::proccessArrival(Event arrivalEvent, queue<Event>& bankQueue)
{
    Event customer = arrivalEvent; // creating customer event
    int departureTime;

    cout << "Processing arrival time at: " << setw(4) << right << currentTime << endl;

    eventListPQueue.pop(); // popping off from priority queue
    if (bankQueue.empty() && tellerAvailable)
    {
        departureTime = currentTime + customer.length;
        Event newDepartureEvent = {'D', departureTime, '/'}; // creating a new departur event
        eventListPQueue.push(newDepartureEvent); // pushign to priority queue
        tellerAvailable = false; // teller is occupied

        totalWait = totalWait + (currentTime - arrivalEvent.time); // caculating the total wait time
    }
    else
        bankQueue.push(customer); // pushing to bankQueue

}

// EventList::processDeparture
void EventList::processDeparture(Event departureEvent, queue<Event>& bankQueue)
{
    int departureTime;

    cout << "Processing departure time at: " << currentTime << endl;

    eventListPQueue.pop(); // popping off from priority queue
    if (!bankQueue.empty())
    {
        Event customer = bankQueue.front(); // creating customer event
        bankQueue.pop(); // removing from bankQueue
        departureTime = currentTime + customer.length; // calculating departure time
        Event newDepartureEvent = {'D', departureTime, '/'}; // creating a new departure event
        eventListPQueue.push(newDepartureEvent); // pushing to priority queue

        totalWait = totalWait + (currentTime - customer.time); // calculating the total wait time
    }
    else
        tellerAvailable = true; // teller is free
}

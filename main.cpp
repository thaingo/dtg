#include <iostream>
#include <thread>
#include <chrono>

#include "dataguard.hpp"

// variables to monitor

namespace levels {
    double f1_v1 = 15000, f2_v2 = 3000;
}

// dataguard monitoring sample

int main()
{
    // configuration

    dataguard dtg;

    dtg.chiefs = {"chief1@nova.com", "chief2@nova.com"};
    dtg.engineers = {"engineer1@nova.com", "engineer2@nova.com", "engineer3@nova.com"};
    dtg.machine = {"automatic-subsystem@nova.com" };

    dtg.ongood = { []( dataguard &dtg, const std::string &issue ) {
        for( auto &who : dtg.machine )
            std::cout << '\t' << who << " says: " << issue << std::endl;
    }};
    dtg.onwarn = { []( dataguard &dtg, const std::string &warning ) {
        for( auto &who : dtg.engineers )
            std::cout << '\t' << who << " says: " << warning << std::endl;
    }};
    dtg.onfail = { []( dataguard &dtg, const std::string &failure ) {
        for( auto &who : dtg.chiefs )
            std::cout << '\t' << who << " says: " << failure << std::endl;
        for( auto &who : dtg.engineers )
            std::cout << '\t' << who << " says: " << failure << std::endl;
    }};

    auto action = []( dataguard &dtg, const std::string &unused ) {
        std::cout << "watching value of field1:" << levels::f1_v1 << std::endl;
        std::cout << "watching value of field2:" << levels::f2_v2 << std::endl;

        /*
        * feel free to implement your business logic here
        */
        if( levels::f2_v2 > 2000 )
            dtg.good("nominal");
        else
            if( levels::f2_v2 >  200 )
                dtg.warn("there might be a data issue with field2!");
            else
                dtg.fail("PROBLEM!!");

        /*
        * other logics or so
        */
        if( levels::f1_v1  >= 3000 )
            dtg.good("nominal");
        else
            if( levels::f1_v1  >=  100 )
                dtg.warn("there might be a data issue with field1!");
            else
                dtg.fail("the data is lost!");
    };

    // patrol every 2 seconds
    dtg.patrol( action, 0, 2 );

    // queue as many patrols as you like
    // dtg.patrol( ... );

    // configuration is done at this point
    // dataguard is working in background right now
    // you can modify all monitored data in real-time

    // simulate a cpu load and let dataguard reacts
    std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
    levels::f1_v1 /= 30; levels::f2_v2 /= 3;

    std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
    levels::f1_v1 /= 10; levels::f2_v2 /= 20;

    std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
    return 0;
}

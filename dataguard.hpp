#ifndef DATAGUARD_HPP_INCLUDED
#define DATAGUARD_HPP_INCLUDED

#include <chrono>
#include <deque>
#include <string>
#include <thread>

class dataguard
{
    public:

    typedef std::string issue_type;
    typedef std::string receiver_type;

    typedef void (*todo)( dataguard &self, const issue_type &fn );

    // feel free to submit data to these structs
    std::deque<receiver_type> chiefs;
    std::deque<receiver_type> engineers;
    std::deque<receiver_type> machine;

    std::deque<todo> onfail;
    std::deque<todo> onwarn;
    std::deque<todo> ongood;

    // then command some patrols to do custom work
    void patrol( todo fn, double initial_delay, double repeat_delay ) {
        is_running = true;
        todos.push_back( std::thread( [=]() {
            sleep( initial_delay, is_running );
            for(;;) {
                if( !is_running ) return;
                fn( *this, issue_type() );
                sleep( repeat_delay, is_running );
            }
        } ));
    }

    // these functions are called from lambda fn in patrol() call to signal issues
    void good( const issue_type &issue ) {
        for( auto &fn : ongood )
            fn( *this, issue );
    }

    void fail( const issue_type &issue ) {
        for( auto &fn : onfail )
            fn( *this, issue );
    }

    void warn( const issue_type &issue ) {
        for( auto &fn : onwarn )
            fn( *this, issue );
    }

    // that is all. really
    dataguard() : is_running(true)
    {}

    ~dataguard() {
        is_running = false;
        while( !todos.empty() ) {
            todos.back().join();
            todos.pop_back();
        }
    }

    protected:

    volatile bool is_running;
    std::deque<std::thread> todos;

    void sleep( double seconds, volatile const bool &is_running ) {
        auto sleep = []( double seconds ) {
            std::chrono::microseconds duration( (int)(seconds * 1000000) );
            std::this_thread::sleep_for( duration );
        };
        while( is_running && seconds >= 0.5 ) sleep((seconds -= 0.5, 0.5));
        while( is_running && seconds >  0.0 ) sleep( seconds );
    }
};

#endif // DATAGUARD_HPP_INCLUDED

// Copyright (c) 2022-2023 Robert A. Alfieri
// 
// WARNING: THIS SOFTWARE CONTAINS TRADE SECRET INFORMATION OF ROBERT A. ALFIERI.
// UNAUTHORIZED DISCLOSURE IS STRICTLY PROHIBITED AND MAY RESULT IN SERIOUS LEGAL CONSEQUENCES.
//
// cpp_eg.cpp - example C++ program
//
#include "sys.h"                // common utility functions

int main( int argc, const char * argv[] )
{
    // set defaults here
    uint32_t seed       = 0xcafebabe;

    //--------------------------------------------------------------
    // Parse Arguments
    //--------------------------------------------------------------
    for( int i = 1; i < argc; i++ )
    {
        std::string arg = argv[i];
        if ( arg == "-debug" ) {                                __debug = std::stoi( argv[++i] ); // in sys.h
        } else if ( arg == "-seed" ) {                          seed = std::stoi( argv[++i] );
        } else {                                                die( "unknown option: " + arg ); }
    }
    rand_thread_seed( seed );

    //--------------------------------------------------------------
    // Fill a 1,000,000,000 long array with random byte values (uint8_t).
    //--------------------------------------------------------------
    const uint32_t byte_cnt = 1000000000;
    std::cout << "Initializing " << byte_cnt << " bytes with random data...\n";
    uint8_t * bytes = new uint8_t[byte_cnt];
    for( uint32_t i = 0; i < byte_cnt; i++ )
    {
        bytes[i] = rand_n( 256 );       // returns random int between 0 and 255
    }

    //--------------------------------------------------------------
    // Add up the bytes using one thread.
    //--------------------------------------------------------------
    std::cout << "Adding bytes using one thread...\n";
    real64 begin = clock_time();
    uint64_t sum = 0;
    for( uint32_t i = 0; i < byte_cnt; i++ )
    {
        sum += bytes[i];
    }
    real64 end = clock_time();
    real64 elapsed = end - begin;
    //std::cout << "begin=" << begin << " end=" << end << " elapsed=" << elapsed << "\n";
    std::cout << "sum=" << sum << " elapsed=" << elapsed << "\n";

    return 0;
}

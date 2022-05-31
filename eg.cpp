// Copyright (c) 2022-2023 Robert A. Alfieri
// 
// WARNING: THIS SOFTWARE CONTAINS TRADE SECRET INFORMATION OF ROBERT A. ALFIERI.
// UNAUTHORIZED DISCLOSURE IS STRICTLY PROHIBITED AND MAY RESULT IN SERIOUS LEGAL CONSEQUENCES.
//
// eg.cpp - example C++ program
//
#include "sys.h"                // common utility functions

int main( int argc, const char * argv[] )
{
    // set defaults here

    //--------------------------------------------------------------
    // Parse Arguments
    //--------------------------------------------------------------
    for( int i = 1; i < argc; i++ )
    {
        std::string arg = argv[i];
        if ( arg == "-debug" ) {                                __debug = std::stoi( argv[++i] );
        } else if ( arg == "-foo" ) {                           // etc.
        } else {                                                die( "unknown option: " + arg ); }
    }

    return 0;
}

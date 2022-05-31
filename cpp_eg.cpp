// Copyright (c) 2022-2023 Robert A. Alfieri
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// cpp_eg.cpp - example C++ program
//
#include "sys.h"                // common utility functions

struct State
{
    uint64_t            byte_cnt;
    uint8_t *           bytes;
    uint64_t            partial_sum[THREAD_CNT_MAX];
};

// this gets executed by each thread; it is called by thread_parallelize()
//     tid     == my thread id (0 .. thr_cnt-1)
//     thr_cnt == thread count
//     arg     == opaque pointer to State struct (thread_parallelize() requires an opaque ptr to be general)
//
void sumThread( uint32_t tid, uint32_t thr_cnt, void * arg )
{
    State * s = reinterpret_cast<State *>( arg );  // get a non-opaque pointer

    // figure out range of bytes this thread should add up
    //
    uint64_t bytes_per_thread = s->byte_cnt / thr_cnt;
    uint64_t start_i = bytes_per_thread * tid;          
    uint64_t end_i   = (tid == (thr_cnt-1)) ? (s->byte_cnt-1) : (start_i + bytes_per_thread-1);

    uint64_t psum = 0;
    uint8_t * bytes = s->bytes;
    for( uint64_t i = start_i; i <= end_i; i++ )
    {
        psum += bytes[i];
    }
    s->partial_sum[tid] = psum;                          
}

int main( int argc, const char * argv[] )
{
    // set defaults here
    uint32_t seed               = 0xcafebabe;
    uint32_t thread_cnt         = thread_hardware_thread_cnt();   // actual number of CPU HW threads

    //--------------------------------------------------------------
    // Parse Arguments
    //--------------------------------------------------------------
    for( int i = 1; i < argc; i++ )
    {
        std::string arg = argv[i];
        if ( arg == "-debug" ) {                                __debug = std::stoi( argv[++i] ); // in sys.h
        } else if ( arg == "-seed" ) {                          seed = std::stoi( argv[++i] );
        } else if ( arg == "-thread_cnt" ) {                    thread_cnt = std::stoi( argv[++i] );
        } else {                                                die( "unknown option: " + arg ); }
    }
    rand_thread_seed( seed );   // needed only if random numbers are used (currently not)

    //--------------------------------------------------------------
    // Fill a 1,000,000,000 long array with increasing byte values (uint8_t).
    //--------------------------------------------------------------
    const uint64_t byte_cnt = 1000000000;
    std::cout << "Initializing " << byte_cnt << " bytes...\n";
    uint8_t * bytes = new uint8_t[byte_cnt];
    for( uint64_t i = 0; i < byte_cnt; i++ )
    {
        bytes[i] = i & 0xff;    // could also call rand_n( 256 ) for a random byte, but slower
    }

    //--------------------------------------------------------------
    // Add up the bytes using one thread.
    //--------------------------------------------------------------
    std::cout << "\nAdding bytes using 1 thread...\n";
    real64 begin = clock_time();
    uint64_t sum = 0;
    for( uint64_t i = 0; i < byte_cnt; i++ )
    {
        sum += bytes[i];
    }
    real64 end = clock_time();
    real64 elapsed = end - begin;
    std::cout << "sum=" << sum << " elapsed=" << elapsed << " secs\n";

    //--------------------------------------------------------------
    // Add up the bytes using thread_cnt threads.
    //--------------------------------------------------------------
    std::cout << "\nAdding bytes using " << thread_cnt << " threads...\n";
    begin = clock_time();
    State s;
    s.byte_cnt = byte_cnt;
    s.bytes = bytes;

    // this will not return until all thread_cnt threads have finished executing sumThread()
    // The pointer to s will get passed into sumThread as an opaque pointer
    // (that's so that thread_parallelize() doesn't need to know about State).
    //
    thread_parallelize( thread_cnt, sumThread, &s ); 

    // add up partial sums
    uint64_t tsum = 0;
    for( uint32_t i = 0; i < thread_cnt; i++ )
    {
        tsum += s.partial_sum[i];
    }
    end = clock_time();
    real64 telapsed = end - begin;
    std::cout << "sum=" << tsum << " elapsed=" << telapsed << " secs\n";
    dassert( tsum == sum, "threaded sum != non-threaded sum" );

    std::cout << "\nSpeedup=" << (elapsed/telapsed) << "x\n";

    return 0;
}

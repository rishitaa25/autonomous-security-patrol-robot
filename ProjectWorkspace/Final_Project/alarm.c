#include "open_interface.h"
#include "Timer.h"

#include <stdio.h>
#include <stdlib.h>

void alarm() {
    unsigned char notes[] = {
        84,88,84,88,84,88,84,88
    };

    unsigned char durations[] = {
        6,6,6,6,6,6,6,12
    };

    oi_loadSong(0, 8, notes, durations);
    oi_play_song(0);
}

//void batmanSong() {
//    unsigned char notes[] = {
//        83,83,82,82,81,81,82,82,
//        83,83,82,82,81,81,82,82,
//        83,0,83
//    };
//
//    unsigned char durations[] = {
//        16,16,16,16,16,16,16,16,
//        16,16,16,16,16,16,16,16,
//        32,16,32
//    };
//
//    oi_loadSong(1, 19, notes, durations);
//    oi_play_song(1);
//}
//
//void missionSong() {
//    unsigned char notes[] = {
//        82,79,86,
//        82,79,85,
//        82,79,84,
//        82,84
//    };
//
//    unsigned char durations[] = {
//        12,12,48,   // da-da-DUM
//        12,12,48,   // da-da-DUM
//        12,12,48,   // da-da-DUM
//        8,24       // ending phrase, slightly stretched
//    };
//
//    oi_loadSong(2, 11, notes, durations);
//    oi_play_song(2);
//}

void alertBelow(){
    unsigned char notes[] = {
               80, 75, 80
           };

           unsigned char durations[] = {
               6, 6, 6
           };

           oi_loadSong(1, 3, notes, durations);
           oi_play_song(1);
}

void alertStart() {
    unsigned char notes[] = {
        80
    };

    unsigned char durations[] = {
        6
    };

    oi_loadSong(2, 1, notes, durations);
    oi_play_song(2);
}

void alertEnd(){
    unsigned char notes[] = {
           75
       };

       unsigned char durations[] = {
           6
       };

       oi_loadSong(3, 1, notes, durations);
       oi_play_song(3);
}

//void alertTape(){
//    unsigned char notes[] = {
//               50, 50
//           };
//
//           unsigned char durations[] = {
//               6, 6
//           };
//
//           oi_loadSong(1, 2, notes, durations);
//           oi_play_song(3);
//}



//void alertHole(){
//    unsigned char notes[] = {
//                   1
//               };
//
//               unsigned char durations[] = {
//                   10
//               };
//
//               oi_loadSong(6, 1, notes, durations);
//               oi_play_song(6);
//}

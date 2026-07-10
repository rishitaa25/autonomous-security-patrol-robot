/**
 * lab6_template.c
 *
 *
 */

#include "Timer.h"
#include "lcd.h"
#include "scan.h"  // For scan sensors
#include "uart.h"
#include "alarm.h"
#include <stdio.h>
#include <math.h>
#include "adc.h"

// Uncomment or add any include directives that are needed
#include "open_interface.h"
#include "movement.h"
//#include "button.h"

//#warning "Possible unimplemented functions"
#define MAX_DIST 150
#define IR_THRESHOLD 1000
#define THRESHOLD 200
#define TEST 750

//----DOWNSTAIRS----
//#define TEST 750

//----CYBOT21----
#define LARGEST 700
#define OTHERS 750


void cyBot_sendString(char *send){

    int i = 0;
    //sends each character of the string
    while(send[i] != '\0'){
        uart_sendChar(send[i]);
        i++;
    }
}

void cyBOT_scanRange(int range){

    int angle;
    cyBOT_Scan_t scan;
    char data[50];
    int i = 0;

    for(angle = 0; angle <= range; angle += 2) {
        for(i = 0; i < 5; i++){
            cyBOT_Scan(angle, &scan);   // take measurement at angle
            sprintf(data, "Angle: %d\t Distance: %.2f cm\t IR Value: %d\r\n", angle, scan.sound_dist, scan.IR_raw_val);  //format data
            uart_sendStr(data);
        }
    }
}

void calibrationTest(){

    cyBOT_Scan_t scan;
    int i;
    cyBOT_Scan(0, &scan);

    for(i=0; i <=180; i+=2){
        cyBOT_Scan(i, &scan);
    }
}

void print_objects(object_t *obj){

    char numObj[50];
    char data[75];
    char space[25];

    sprintf(numObj, "\rObject detected: %d\r\n", obj->num);
    uart_sendStr(numObj);

    sprintf(data, "OBJ %.3f %.2f %.3f %s\n",
                    obj->distance,
                    (obj->start_angle + obj->end_angle) / 2.0,
                    obj->linear_width,
                    obj->obj_type == OBJ_OTHERS ? "OTHERS" : "LARGEST");
    uart_sendStr(data);

    sprintf(space, "\r-----------------------------\r\n");
    uart_sendStr(space);

    // --- ALARM SECTION ---
    // Keep ONLY ONE of the two blocks below depending on which you want:

    // Option A: alarm when object hits OTHERS threshold (IR >= 1500)
//    if (obj->obj_type == OBJ_OTHERS) {
//        alarm();
//    }

    // Option B: alarm when object hits LARGEST threshold (IR >= 1000 but < 1500)
    // Alarm only for LARGEST type AND wide enough
//    if (obj->obj_type == OBJ_LARGEST && obj->linear_width > 0.20) {
//        alarm();
//    }
}

//
//void print_objects(object_t *obj){
//
//    char numObj[50];
//    char data[150];
//    char space[60];
//
//    sprintf(numObj, "\rObject detected: %d\r\n", obj->num);
//    uart_sendStr(numObj);
//
//    sprintf(data, "Start Angle: %d, End Angle: %d, Radial Width: %d, Linear Width: %f, Distance: %f\r\n\n", obj->start_angle, obj->end_angle, obj->radial_width, obj->linear_width, obj->distance);
//    uart_sendStr(data);
//
//    sprintf(space, "\r-----------------------------\r\n");
//    uart_sendStr(space);
//}

int bumped_sensor(oi_t*sensor_data){

    int bumped = 0;
    oi_update(sensor_data);

     if(sensor_data->bumpLeft || sensor_data->bumpRight){
          move_backward(sensor_data, 70);
          bumped = 1;
    }
      return bumped;
}

void turnAngle(oi_t*sensor_data, object_t minObj){

    int smallMidAngle;
    smallMidAngle = (minObj.end_angle + minObj.start_angle)/2;

    if(smallMidAngle < 90){
       turn_right(sensor_data, 90 - smallMidAngle);

   }else if (smallMidAngle > 90){
       turn_left(sensor_data, smallMidAngle - 90);

   }

}
object_t scan_Objects(int scanAngle1, int scanAngle2)
{
    uart_sendStr("START\n");  // GUI needs this to begin collecting points

    int angle;
    cyBOT_Scan_t scan;
    object_t obj;
    object_t maxObj;

    #define MAX_OBJECTS 16
    object_t all_objs[MAX_OBJECTS];
    int obj_count = 0;

    int stop     = 0;
    int index    = 0;
    int i        = 0;
    int avgIRVal = 0;
    float avgPingDist = 0;

    obj.start_angle  = 0;
    obj.end_angle    = 0;
    obj.distance     = 0.0;
    obj.linear_width = 0.0;
    obj.obj_type     = OBJ_LARGEST;
    maxObj.linear_width = 0;

    for (angle = scanAngle1; angle <= scanAngle2; angle += 4)
    {
        avgIRVal    = 0;
        avgPingDist = 0.0;

        cyBOT_Scan(angle, &scan);
        avgIRVal    = scan.IR_raw_val;
        avgPingDist = scan.sound_dist;

        // Classify BEFORE deciding whether to re-sample
        int isOthers  = (avgIRVal >= OTHERS);
        int isLargest = (avgIRVal >= LARGEST && !isOthers);

        // If something detected, take 2 more samples and average all 3
        if (isLargest || isOthers) {
            int confirm_ir      = avgIRVal;
            float confirm_dist  = avgPingDist;
            int k;
            for (k = 0; k < 2; k++) {
                cyBOT_Scan(angle, &scan);
                confirm_ir   += scan.IR_raw_val;
                confirm_dist += scan.sound_dist;
            }
            avgIRVal    = confirm_ir   / 3;
            avgPingDist = confirm_dist / 3;

//            // Re-classify with the averaged values
//            isOthers  = (avgIRVal >= OTHERS);
//            isLargest = (avgIRVal >= LARGEST && !isOthers);

            // Only detect objects within 1 metre
            int inRange   = (avgPingDist <= 100.0f && avgPingDist > 0.0f);
            isOthers  = inRange && (avgIRVal >= OTHERS);
            isLargest = inRange && (avgIRVal >= LARGEST && !isOthers);
        }

        float objDist;
        if (avgPingDist < 50.0f) {
            objDist = get_distance(adc_read());  // ADC more accurate close up
            lcd_printf("ADC dist: %df", get_distance(adc_read()));
        } else {
            objDist = avgPingDist;
        }

        // Send scan line to GUI — format: "<angle> <dist_m> <ir>\r\n"
        char data1[50];
        sprintf(data1, "%d %.4f %d\r\n", angle, objDist / 100.0f, avgIRVal);
        uart_sendStr(data1);

        if ((isLargest || isOthers) && stop == 0 && scan.sound_dist < MAX_DIST) {
            obj.start_angle = angle;
            obj.end_angle   = angle;
            obj.distance    = objDist;
            obj.IRVal       = avgIRVal;
            obj.obj_type    = isOthers ? OBJ_OTHERS : OBJ_LARGEST;
            stop = 1;
        }
        else if (stop == 1) {
            if (isLargest || isOthers) {
                if (isOthers) obj.obj_type = OBJ_OTHERS;
                obj.end_angle = angle;
                obj.distance  = objDist;
                obj.IRVal = avgIRVal;
            } else {
                stop = 0;
                obj.linear_width = obj.distance * ((obj.end_angle - obj.start_angle  * (M_PI / 180.0)));
                obj.radial_width = obj.end_angle - obj.start_angle;

                if (obj.radial_width >= 4) {
                    obj.num = ++index;
                    maxObj = obj;
                    print_objects(&obj);
                    if (obj_count < MAX_OBJECTS)
                        all_objs[obj_count++] = obj;
                }

                obj.start_angle  = 0;
                obj.end_angle    = 0;
                obj.distance     = 0.0;
                obj.IRVal        = 0;
                obj.linear_width = 0.0;
                obj.obj_type     = OBJ_LARGEST;
            }
        }
    }

    // Close any object still open at end of scan
    if (stop == 1) {
        obj.linear_width = obj.distance
                           * ((obj.end_angle - obj.start_angle)
                              * (M_PI / 180.0));
        obj.radial_width = obj.end_angle - obj.start_angle;
        if (obj.radial_width >= 4) {
            obj.num = ++index;
//            if (obj.linear_width > maxObj.linear_width)
            maxObj = obj;
            print_objects(&obj);
            if (obj_count < MAX_OBJECTS)
                all_objs[obj_count++] = obj;
        }
    }

    uart_sendStr("END\n");  // GUI needs this to stop collecting points

    // Send OBJ lines to GUI after END
    int j;
    for (j = 0; j < obj_count; j++) {
        float edge_dist_mm = all_objs[j].distance * 1000.0f;
        float centre_angle = (all_objs[j].start_angle + all_objs[j].end_angle) / 2.0f;
        float rel_angle    = centre_angle - 90.0f;
        float diam_mm      = all_objs[j].linear_width * 1000.0f;

        char obj_line[64];
        sprintf(obj_line, "OBJ %.1f %.2f %.1f\n",
                edge_dist_mm, rel_angle, diam_mm);
        uart_sendStr(obj_line);
    }

//    // Alarm on the largest object found
//    if (maxObj.linear_width > 0 && maxObj.obj_type == OBJ_LARGEST) {
//       // alarm();
//    }

    int largeMidAngle = (maxObj.end_angle + maxObj.start_angle) / 2;
    cyBOT_Scan(largeMidAngle, &scan);

    return maxObj;
}

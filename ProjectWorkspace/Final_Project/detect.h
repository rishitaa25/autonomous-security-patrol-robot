

#ifndef DETECT_H_
#define DETECT_H_

#include "Timer.h"
#include "lcd.h"
#include <string.h>
#include "movement.h"

#include "open_interface.h"
#include "uart.h"
#include "scan.h"

typedef enum { OBJ_OTHERS, OBJ_LARGEST } obj_type_e;

typedef struct {
    int start_angle;
    int end_angle;
    int radial_width;
    float linear_width;
    float distance;
    int IRVal;
    int num;
    obj_type_e obj_type;   // <-- add this
} object_t;


void cyBot_sendString(char *send);
void cyBOT_scanRange(int range);
void calibrationTest(void);
void print_objects(object_t *obj);
object_t scan_Objects(int scanAngle1, int scanAngle2);
void move(oi_t*sensor_data, object_t minObj);
int bumped_sensor(oi_t*sensor_data);
void turnAngle(oi_t*sensor_data, object_t minObj);

#endif /* DETECT_H_ */

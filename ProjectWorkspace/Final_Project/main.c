#include "Timer.h"
#include "lcd.h"
#include "servo.h"
#include "button.h"
#include "scan.h"
#include "uart.h"
#include "movement.h"
#include "detect.h"
#include "alarm.h"
#include "open_interface.h"
#include <stdio.h>
#include <string.h>
#include "imu.h"
#include "i2c.h"
#include "adc.h"

static void read_line(char *buf, int maxlen);

static float angleDiff(float a, float b) {
    float diff = a - b;

    while (diff > 180.0f) diff -= 360.0f;
    while (diff < -180.0f) diff += 360.0f;

    if (diff < 0) diff = -diff;
    return diff;
}

static const char* getHardcodedDirection(float heading) {
    // Based on values for bot 23
    // N  = 254
    // NE = halfway between N and E = 299
    // E  = 344
    // SE = halfway between E and S = 30
    // S  = 76
    // SW = halfway between S and W = 127
    // W  = 177
    // NW = halfway between W and N = 216

    float centers[8] = {
        294.0f, 31.0f, 42.0f, 129.0f,
        130.0f, 209.0f, 218.0f, 300.0f
    };

    const char* dirs[8] = {
        "N", "NE", "E", "SE",
        "S", "SW", "W", "NW"
    };

    int bestIndex = 0;
    float bestDiff = angleDiff(heading, centers[0]);

    int i;
    for (i = 1; i < 8; i++) {
        float diff = angleDiff(heading, centers[i]);
        if (diff < bestDiff) {
            bestDiff = diff;
            bestIndex = i;
        }
    }

    return dirs[bestIndex];
}


// -------------------- MAIN --------------------
int main(void) {
    timer_init();
    lcd_init();
    servo_init();
    button_init();
    uart_init();
    imu_init();
    imu_setDefaultUnits();
    imu_setMode(NDOF);
    imu_setMode(COMPASS);
    imu_setMode(MAGONLY);
    adc_init();


    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

   cyBOT_init_Scan(7);

// cyBOT_SERVO_cal_t cal = cyBOT_SERVO_cal();

    // ------ CYBOT 17 ------
//    right_calibration_value = 7300;
//    left_calibration_value  = 33200;

    // ------ CYBOT 24 ------
// right_calibration_value = cal.right;
// left_calibration_value  = cal.left;
//
//    right_calibration_value = 8800;
//    left_calibration_value  = 36200;

    // ------ CYBOT 06 ------
//    right_calibration_value = 8000;
//    left_calibration_value  = 35100;

     // ------ CYBOT 21 ------
//     right_calibration_value = 9300;
//     left_calibration_value  = 38200;

     // ------ CYBOT 20 ------
     right_calibration_value = 6900;
     left_calibration_value  = 34800;

    lcd_printf("READY");
    uart_sendStr("CyBot Ready\r\n");

//    int val;
//    object_t minObj;
//    val = uart_receive();
//       if(val == 'm') {            // start scan when m is pressed
//          minObj =  scan_Objects(45, 140);
//       }

    char cmd_buf[62];
    while(1){
        mag_t* mag = imu_getMag();
        read_line(cmd_buf, sizeof(cmd_buf));

        char debug[80];
        sprintf(debug, "GOT:[%s]\n", cmd_buf);
        uart_sendStr(debug);

//        char imu[70];
//        sprintf(imu, "Heading: %.1f\n X: %d, Y: %d, Z: %d", mag->heading, mag->x, mag->y, mag->z);
//        uart_sendStr(imu);

//        lcd_printf("Heading: %.1f\n X: %d, Y: %d, Z: %d", mag->heading, mag->x, mag->y, mag->z);

        if(strcmp(cmd_buf, "M") == 0){  // scan
            alertStart();
            scan_Objects(0, 180);
            alertEnd();
        } else if(strcmp(cmd_buf, "F") == 0){ // forward
            alertStart();
            sprintf(cmd_buf, "MOVE %.3f 0.00\n", move_forward(sensor_data, 300));
//            turn_right(sensor_data, 14);
            uart_sendStr(cmd_buf);
            alertEnd();
        } else if(strcmp(cmd_buf, "B") == 0){ // back
            alertStart();
            sprintf(cmd_buf, "MOVE %.3f 0.00\n", move_backward(sensor_data, 300));
//            turn_left(sensor_data, 14);
            uart_sendStr(cmd_buf);
            alertEnd();
        } else if(strcmp(cmd_buf, "L") == 0){ // left
            alertStart();
            sprintf(cmd_buf, "MOVE 0.000 %.2f\n", turn_left(sensor_data, 30));
            uart_sendStr(cmd_buf);
            alertEnd();
        } else if(strcmp(cmd_buf, "R") == 0){ // right
            alertStart();
            sprintf(cmd_buf, "MOVE 0.000 %.2f\n", turn_right(sensor_data, 30));
            uart_sendStr(cmd_buf);
            alertEnd();
        } else if(strcmp(cmd_buf, "A") == 0){ // alert
            alarm();
            //batmanSong();
        } else if(strcmp(cmd_buf, "C") == 0){ // creep forward
            alertStart();
            sprintf(cmd_buf, "MOVE %.3f 0.00\n", move_forward(sensor_data, 30));
            uart_sendStr(cmd_buf);
            alertEnd();
        } else if(strcmp(cmd_buf, "E") == 0){ // creep backward
            alertStart();
            sprintf(cmd_buf, "MOVE %.3f 0.00\n", move_backward(sensor_data, 30));
//            turn_left(sensor_data, 2);
            uart_sendStr(cmd_buf);
            alertEnd();
        }else if(strcmp(cmd_buf, "0") == 0){ // left 90
            alertStart();
            sprintf(cmd_buf, "MOVE 0.000 %.2f\n", turn_left(sensor_data, 90));
            uart_sendStr(cmd_buf);
            alertEnd();
        } else if(strcmp(cmd_buf, "9") == 0){ // right 90
            alertStart();
            sprintf(cmd_buf, "MOVE 0.000 %.2f\n", turn_right(sensor_data, 90));
            uart_sendStr(cmd_buf);
            alertEnd();
        }else if(strcmp(cmd_buf, "5") == 0){ // left 15
            alertStart();
            sprintf(cmd_buf, "MOVE 0.000 %.2f\n", turn_left(sensor_data, 15));
            uart_sendStr(cmd_buf);
            alertEnd();
        } else if(strcmp(cmd_buf, "1") == 0){ // right 15
            alertStart();
            sprintf(cmd_buf, "MOVE 0.000 %.2f\n", turn_right(sensor_data, 15));
            uart_sendStr(cmd_buf);
            alertEnd();
        }
//        else if(strcmp(cmd_buf, "H") == 0){ // imu
//                char imu[100];
//                sprintf(imu, "Heading: %.1f\n X: %d, Y: %d, Z: %d", mag->heading, mag->x, mag->y, mag->z);
//                uart_sendStr(imu);
//                free(mag);
//        }
        else if(strcmp(cmd_buf, "H") == 0) {
                    float heading = imu_getEulerHeading();
                    const char* dir = getHardcodedDirection(heading);

                    char imu_msg[100];
                    sprintf(imu_msg, "Heading: %.2f Direction: %s\r\n", heading, dir);
                    uart_sendStr(imu_msg);

                    lcd_printf("Head: %.1f\nDir: %s", heading, dir);
                }


    }

    //run_security_patrol(sensor_data);

//    lcd_printf("L: %d, FL: %d\nR: %d, FR: %d" ,
//                           sensor_data -> cliffLeftSignal, sensor_data -> cliffFrontLeftSignal, sensor_data -> cliffRightSignal, sensor_data -> cliffFrontRightSignal);

    oi_free(sensor_data);
    return 0;
}

static void read_line(char *buf, int maxlen)
{
    int i = 0;
    char c;
    while (1)
    {
        c = uart_receive();
        if (c == '\n' || c == '\r')
        {
            if (i > 0) break;
            continue;
        }
        if (i < maxlen - 1)
            buf[i++] = c;
    }
    buf[i] = '\0';
}

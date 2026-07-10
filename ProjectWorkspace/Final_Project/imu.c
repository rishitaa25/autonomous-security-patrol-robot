

#include <stdlib.h>
#include <math.h>
#include "i2c.h"
#include "imu.h"
#include "timer.h"
#include <inc/tm4c123gh6pm.h>

#define DEG_PER_RAD (180.0f / M_PI)

#define MODE_SWITCH_DELAY_MS 20

/******************************************************************************
 * Static helpers
 ******************************************************************************/

static void addr_set(uint8_t addr);

static float vectorMag(float x, float y, float z);

static int16_t bytesToInt16(uint8_t lsb, uint8_t msb) {
    return (int16_t)((msb << 8) | lsb);
}

/******************************************************************************
 * Address select
 ******************************************************************************/

static void addr_set(uint8_t addr) {
    switch (addr) {
        case 0x28:
            GPIO_PORTB_DATA_R &= ~0x80;
            break;
        case 0x29:
        default:
            GPIO_PORTB_DATA_R |= 0x80;
            break;
    }
}

/******************************************************************************
 * Init
 ******************************************************************************/

void imu_init() {
    i2c_init();
    timer_init();

    SYSCTL_RCGCGPIO_R |= 0x2;
    while ((SYSCTL_PRGPIO_R & 0x2) == 0);

    GPIO_PORTB_DEN_R |= 0xC0;
    GPIO_PORTB_DIR_R |= 0xC0;
    GPIO_PORTB_AFSEL_R &= ~0xC0;

    GPIO_PORTB_DATA_R |= 0x40;

    addr_set(BNO055_ADDRESS_B);
}

/******************************************************************************
 * Mode control
 ******************************************************************************/

void imu_setMode(imu_mode_t mode) {
    imu_writeReg(IMU_OPR_MODE, mode);

    timer_waitMillis(MODE_SWITCH_DELAY_MS);
}

imu_mode_t imu_getMode() {
    return (imu_mode_t)imu_readRegByte(IMU_OPR_MODE);
}

/******************************************************************************
 * Magnetometer
 ******************************************************************************/

mag_t* imu_getMag() {
    mag_t* mag = malloc(sizeof(mag_t));

    uint8_t* data = imu_readRegBytes(IMU_MAG_DATAX_LSB, 6);

    int16_t x = bytesToInt16(data[0], data[1]);
    int16_t y = bytesToInt16(data[2], data[3]);
    int16_t z = bytesToInt16(data[4], data[5]);

    mag->x = x;
    mag->y = y;
    mag->z = z;

    mag->heading = atan2f((float)y, (float)x) * DEG_PER_RAD;
    if (mag->heading < 0) {
        mag->heading += 360.0f;
    }

    free(data);
    return mag;
}

/******************************************************************************
 * Accelerometer
 ******************************************************************************/

acc_t* imu_getAcc() {
    acc_t* acc = malloc(sizeof(acc_t));

    uint8_t* data = imu_readRegBytes(IMU_LIA_DATAX_LSB, 6);

    int16_t x = bytesToInt16(data[0], data[1]);
    int16_t y = bytesToInt16(data[2], data[3]);
    int16_t z = bytesToInt16(data[4], data[5]);

    acc->x = x * 0.01f;
    acc->y = y * 0.01f;
    acc->z = z * 0.01f;

    acc->magnitude = vectorMag(acc->x, acc->y, acc->z);

    free(data);
    return acc;
}

/******************************************************************************
 * Linear accel (alias safe implementation)
 ******************************************************************************/

acc_t* imu_getLinAcc() {
    return imu_getAcc();
}

/******************************************************************************
 * Units
 ******************************************************************************/

void imu_setDefaultUnits() {
    imu_mode_t mode = imu_getMode();

    imu_setMode(CONFIG);

    uint8_t units = 0b00010000;
    imu_writeReg(IMU_UNIT_SEL, units);

    imu_setMode(mode);
}

/******************************************************************************
 * Status / Info
 ******************************************************************************/

bool imu_connected() {
    return imu_readRegByte(IMU_CHIP_ID) == BNO055_ID;
}

uint8_t imu_getStatus() {
    return imu_readRegByte(IMU_SYS_STATUS);
}

int imu_getTemp() {
    return imu_readRegByte(IMU_TEMP);
}

imu_info_t* imu_getChipInfo() {
    imu_info_t* info = malloc(sizeof(imu_info_t));

    uint8_t* data = imu_readRegBytes(IMU_CHIP_ID, 7);

    info->chipID = data[0];
    info->accID  = data[1];
    info->magID  = data[2];
    info->gyrID  = data[3];

    info->swRevID = data[4] | (data[5] << 8);
    info->blRevID = data[6];

    free(data);
    return info;
}

/******************************************************************************
 * Reset
 ******************************************************************************/

void imu_reset() {
    GPIO_PORTB_DATA_R &= ~0x40;
    asm(" NOP");
    asm(" NOP");
    GPIO_PORTB_DATA_R |= 0x40;
}

/******************************************************************************
 * I2C helpers (FIXED — no leaks)
 ******************************************************************************/

void imu_writeReg(uint8_t regAddr, uint8_t val) {
    uint8_t data[2] = { regAddr, val };
    i2c_sendBytes(BNO055_ADDRESS_B, data, 2);
}

uint8_t imu_readRegByte(uint8_t regAddr) {
    i2c_requestByte(BNO055_ADDRESS_B, regAddr);
    return i2c_recByte(BNO055_ADDRESS_B);
}

uint8_t* imu_readRegBytes(uint8_t regAddr, size_t len) {

    i2c_requestByte(BNO055_ADDRESS_B, regAddr);
    return i2c_recBytes(BNO055_ADDRESS_B, len);

}

/******************************************************************************
 * Math helper
 ******************************************************************************/

static float vectorMag(float x, float y, float z) {
    return sqrtf(x*x + y*y + z*z);
}

float imu_getEulerHeading() {
    uint8_t* data = imu_readRegBytes(IMU_EUL_HEAD_LSB, 2);
    int16_t raw = (int16_t)((data[1] << 8) | data[0]);
    free(data);

    return raw / 16.0f;
}


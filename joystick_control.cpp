#include "joystick_control.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <fcntl.h>
#include <iostream>
#include <cmath>
#include <unistd.h>
#include <atomic>
#include "globals.h"



// GPIO引脚配置
const int ENA = 16;
const int IN1 = 20;
const int IN2 = 21;

const int ENA2 = 23;
const int IN3 = 24;
const int IN4 = 25;

const int ENA3 = 13;
const int IN5 = 19;
const int IN6 = 26;

const int ENA4 = 22;
const int IN7 = 5;
const int IN8 = 6;

// 数值映射函数，将一个范围的值映射到另一个范围
int mapValue(int value, int in_min, int in_max, int out_min, int out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// 控制电机以移动小车
void controlCar(int x, int y, const std::string& rotate) {
    // 将手柄的X和Y值从[0, 255]映射到[-100, 100]
    int xSpeed = mapValue(x, 0, 255, -100, 100);
    int ySpeed = mapValue(y, 0, 255, 100, -100);

    // 计算电机速度
    int leftFrontSpeed = ySpeed + xSpeed;
    int rightFrontSpeed = ySpeed - xSpeed;
    int leftRearSpeed = ySpeed - xSpeed;
    int rightRearSpeed = ySpeed + xSpeed;

    // 根据旋转指令调整速度
    if (rotate == "left") {
        leftFrontSpeed -= 50;
        rightFrontSpeed += 50;
        leftRearSpeed -= 50;
        rightRearSpeed += 50;
    } else if (rotate == "right") {
        leftFrontSpeed += 50;
        rightFrontSpeed -= 50;
        leftRearSpeed += 50;
        rightRearSpeed -= 50;
    }

    // 限制电机速度在-100到100之间
    leftFrontSpeed = std::max(std::min(leftFrontSpeed, 100), -100);
    rightFrontSpeed = std::max(std::min(rightFrontSpeed, 100), -100);
    leftRearSpeed = std::max(std::min(leftRearSpeed, 100), -100);
    rightRearSpeed = std::max(std::min(rightRearSpeed, 100), -100);

    // 设置电机方向和速度（使用analogWrite设置PWM占空比）
    digitalWrite(IN1, leftFrontSpeed >= 0 ? HIGH : LOW);
    digitalWrite(IN2, leftFrontSpeed < 0 ? HIGH : LOW);
    digitalWrite(IN3, rightFrontSpeed >= 0 ? HIGH : LOW);
    digitalWrite(IN4, rightFrontSpeed < 0 ? HIGH : LOW);
    digitalWrite(IN5, leftRearSpeed >= 0 ? HIGH : LOW);
    digitalWrite(IN6, leftRearSpeed < 0 ? HIGH : LOW);
    digitalWrite(IN7, rightRearSpeed >= 0 ? HIGH : LOW);
    digitalWrite(IN8, rightRearSpeed < 0 ? HIGH : LOW);

    softPwmWrite(ENA, abs(leftFrontSpeed));
    softPwmWrite(ENA2, abs(rightFrontSpeed));
    softPwmWrite(ENA3, abs(leftRearSpeed));
    softPwmWrite(ENA4, abs(rightRearSpeed));

    std::cout << "X轴: " << x << ", Y轴: " << y
              << ", 左前轮: " << leftFrontSpeed
              << ", 右前轮: " << rightFrontSpeed
              << ", 左后轮: " << leftRearSpeed
              << ", 右后轮: " << rightRearSpeed
              << ", 转向: " << rotate << std::endl;
}

void controlJoystick() {
    // 初始化WiringPi用于GPIO控制
    if (wiringPiSetupGpio() == -1) {
        std::cerr << "WiringPi初始化失败." << std::endl;
        return;
    }

    // 设置GPIO引脚模式
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA3, OUTPUT);
    pinMode(IN5, OUTPUT);
    pinMode(IN6, OUTPUT);
    pinMode(ENA4, OUTPUT);
    pinMode(IN7, OUTPUT);
    pinMode(IN8, OUTPUT);

    // 初始化软件PWM以控制电机
    softPwmCreate(ENA, 0, 100);
    softPwmCreate(ENA2, 0, 100);
    softPwmCreate(ENA3, 0, 100);
    softPwmCreate(ENA4, 0, 100);

    // 使用evdev设置手柄输入
    struct libevdev *dev = NULL;
    int fd = open("/dev/input/event4", O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "打开输入设备失败." << std::endl;
        return;
    }

    int rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        std::cerr << "libevdev初始化失败." << std::endl;
        return;
    }

    int xAxisValue = 127; // 手柄初始位置
    int yAxisValue = 127;
    std::string rotateState = "";

    try {
        struct input_event ev;
        while (!stop_joysProgram) {
            rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
            if (rc == 0) {
                if (ev.type == EV_ABS) {
                    if (ev.code == ABS_X) {
                        xAxisValue = ev.value;
                    } else if (ev.code == ABS_Y) {
                        yAxisValue = ev.value;
                    }
                    controlCar(xAxisValue, yAxisValue, rotateState);
                } else if (ev.type == EV_KEY) {
                    if (ev.code == BTN_TOP) { // 左转
                        rotateState = (ev.value == 1) ? "left" : "";
                    } else if (ev.code == BTN_THUMB) { // 右转
                        rotateState = (ev.value == 1) ? "right" : "";
                    }
                    controlCar(xAxisValue, yAxisValue, rotateState);
                }
            }
        }
    } catch (...) {
        std::cerr << "发生错误." << std::endl;
    }

    // 释放资源
    libevdev_free(dev);
    close(fd);
    std::cout << "程序结束，GPIO资源已释放." << std::endl;
}

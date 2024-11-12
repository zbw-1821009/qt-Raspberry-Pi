#include <wiringPi.h>
#include <softPwm.h>
#include <wiringSerial.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstring>
#include <algorithm> // 确保包含algorithm
#include <stdexcept> // 包含stdexcept处理错误
#include "globals.h"

#define ENA 16  // 左前侧电机 EN 引脚
#define IN1 20  // 左前侧电机正转控制引脚
#define IN2 21  // 左前侧电机反转控制引脚
#define ENA2 23 // 右前侧电机 EN 引脚
#define IN3 24  // 右前侧电机正转控制引脚
#define IN4 25  // 右前侧电机反转控制引脚
#define ENA3 13 // 左后侧电机 EN 引脚
#define IN5 19  // 左后侧电机正转控制引脚
#define IN6 26  // 左后侧电机反转控制引脚
#define ENA4 22 // 右后侧电机 EN 引脚
#define IN7 5   // 右后侧电机正转控制引脚
#define IN8 6   // 右后侧电机反转控制引脚
#define TRIG 4  // 超声波传感器触发引脚
#define ECHO 17 // 超声波传感器回声引脚



// 初始化 GPIO 设置
void setupGPIO() {
    wiringPiSetupGpio();

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
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
std::cerr << "pinmMode 初始化！" << std::endl;
    // 创建 PWM 信号
    softPwmCreate(ENA, 0, 100);
    softPwmCreate(ENA2, 0, 100);
    softPwmCreate(ENA3, 0, 100);
    softPwmCreate(ENA4, 0, 100);
}

// 控制电机的函数
void controlMotor(int in1, int in2, int pwm, int dutyCycle) {
    digitalWrite(in1, HIGH); // 设置正转
    digitalWrite(in2, LOW);  // 设置反转为低电平
    softPwmWrite(pwm, dutyCycle); // 设置 PWM 占空比
}

// 停止所有电机
void stopMotor() {
    std::cout << "stopMoter" << std::endl;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    digitalWrite(IN5, LOW);
    digitalWrite(IN6, LOW);
    digitalWrite(IN7, LOW);
    digitalWrite(IN8, LOW);
    softPwmWrite(ENA, 0);
    softPwmWrite(ENA2, 0);
    softPwmWrite(ENA3, 0);
    softPwmWrite(ENA4, 0);
}

// 蓝牙串口控制函数
void blue() {
    try {
        int fd = serialOpen("/dev/serial0", 9600); // 打开串口设备
        if (fd == -1) {
            std::cerr << "Failed to open serial port" << std::endl;
            return;
        }
        std::cout << "Bluetooth serial started." << std::endl;

        while (!stop_program) {
            if (serialDataAvail(fd)) { // 检查是否有数据可用
                char command = serialGetchar(fd); // 读取一个字符命令
                serialFlush(fd);

                switch (command) {
                case 'f': // 前进
                    controlMotor(IN1, IN2, ENA, 70);
                    controlMotor(IN3, IN4, ENA2, 70);
                    controlMotor(IN5, IN6, ENA3, 70);
                    controlMotor(IN7, IN8, ENA4, 70);
                    serialPuts(fd, "Forward\n");
                    std::cout << "Forward" << std::endl;
                    break;
                case 's': // 停止
                    stopMotor();
                    serialPuts(fd, "Stop\n");
                    std::cout << "Stop" << std::endl;
                    break;
                case 'b': // 后退
                    controlMotor(IN2, IN1, ENA, 50);
                    controlMotor(IN4, IN3, ENA2, 50);
                    controlMotor(IN6, IN5, ENA3, 50);
                    controlMotor(IN8, IN7, ENA4, 50);
                    serialPuts(fd, "Backward\n");
                    std::cout << "Backward" << std::endl;
                    break;
                case 'l': // 左平移
                    digitalWrite(IN1, LOW);
                    digitalWrite(IN2, HIGH);
                    digitalWrite(IN3, HIGH);
                    digitalWrite(IN4, LOW);
                    digitalWrite(IN5, HIGH);
                    digitalWrite(IN6, LOW);
                    digitalWrite(IN7, LOW);
                    digitalWrite(IN8, HIGH);
                    softPwmWrite(ENA, 100);
                    softPwmWrite(ENA2, 100);
                    softPwmWrite(ENA3, 100);
                    softPwmWrite(ENA4, 100);
                    serialPuts(fd, "Left Move\n");
                    std::cout << "左平移" << std::endl;
                    break;
                case 'r': // 右平移
                    digitalWrite(IN1, HIGH);
                    digitalWrite(IN2, LOW);
                    digitalWrite(IN3, LOW);
                    digitalWrite(IN4, HIGH);
                    digitalWrite(IN5, LOW);
                    digitalWrite(IN6, HIGH);
                    digitalWrite(IN7, HIGH);
                    digitalWrite(IN8, LOW);
                    softPwmWrite(ENA, 100);
                    softPwmWrite(ENA2, 100);
                    softPwmWrite(ENA3, 100);
                    softPwmWrite(ENA4, 100);
                    serialPuts(fd, "Right Move\n");
                    std::cout << "右平移" << std::endl;
                    break;
//                case 'lf': // 左前斜向移动
//                    digitalWrite(IN1, HIGH);
//                    digitalWrite(IN2, LOW);
//                    digitalWrite(IN3, LOW);
//                    digitalWrite(IN4, LOW);
//                    digitalWrite(IN5, HIGH);
//                    digitalWrite(IN6, LOW);
//                    digitalWrite(IN7, LOW);
//                    digitalWrite(IN8, HIGH);
//                    softPwmWrite(ENA, 70);
//                    softPwmWrite(ENA2, 70);
//                    softPwmWrite(ENA3, 70);
//                    softPwmWrite(ENA4, 70);
//                    serialPuts(fd, "Left Forward\n");
//                    std::cout << "左前斜向移动" << std::endl;
//                    break;
//                case 'rf': // 右前斜向移动
//                    digitalWrite(IN1, HIGH);
//                    digitalWrite(IN2, LOW);
//                    digitalWrite(IN3, HIGH);
//                    digitalWrite(IN4, LOW);
//                    digitalWrite(IN5, LOW);
//                    digitalWrite(IN6, HIGH);
//                    digitalWrite(IN7, HIGH);
//                    digitalWrite(IN8, LOW);
//                    softPwmWrite(ENA, 70);
//                    softPwmWrite(ENA2, 70);
//                    softPwmWrite(ENA3, 70);
//                    softPwmWrite(ENA4, 70);
//                    serialPuts(fd, "Right Forward\n");
//                    std::cout << "右前斜向移动" << std::endl;
//                    break;
//                case 'lb': // 左后斜向移动
//                    digitalWrite(IN1, LOW);
//                    digitalWrite(IN2, HIGH);
//                    digitalWrite(IN3, LOW);
//                    digitalWrite(IN4, LOW);
//                    digitalWrite(IN5, LOW);
//                    digitalWrite(IN6, HIGH);
//                    digitalWrite(IN7, LOW);
//                    digitalWrite(IN8, HIGH);
//                    softPwmWrite(ENA, 70);
//                    softPwmWrite(ENA2, 70);
//                    softPwmWrite(ENA3, 70);
//                    softPwmWrite(ENA4, 70);
//                    serialPuts(fd, "Left Backward\n");
//                    std::cout << "左后斜向移动" << std::endl;
//                    break;
//                case 'rb': // 右后斜向移动
//                    digitalWrite(IN1, LOW);
//                    digitalWrite(IN2, HIGH);
//                    digitalWrite(IN3, LOW);
//                    digitalWrite(IN4, HIGH);
//                    digitalWrite(IN5, LOW);
//                    digitalWrite(IN6, LOW);
//                    digitalWrite(IN7, LOW);
//                    digitalWrite(IN8, HIGH);
//                    softPwmWrite(ENA, 70);
//                    softPwmWrite(ENA2, 70);
//                    softPwmWrite(ENA3, 70);
//                    softPwmWrite(ENA4, 70);
//                    serialPuts(fd, "Right Backward\n");
//                    std::cout << "右后斜向移动" << std::endl;
//                    break;
//                case 'tl': // 左旋转
//                    digitalWrite(IN1, HIGH);
//                    digitalWrite(IN2, LOW);
//                    digitalWrite(IN3, LOW);
//                    digitalWrite(IN4, HIGH);
//                    digitalWrite(IN5, HIGH);
//                    digitalWrite(IN6, LOW);
//                    digitalWrite(IN7, LOW);
//                    digitalWrite(IN8, HIGH);
//                    softPwmWrite(ENA, 100);
//                    softPwmWrite(ENA2, 100);
//                    softPwmWrite(ENA3, 100);
//                    softPwmWrite(ENA4, 100);
//                    serialPuts(fd, "Turn Left\n");
//                    std::cout << "左旋转" << std::endl;
//                    break;
//                case 'tr': // 右旋转
//                    digitalWrite(IN1, LOW);
//                    digitalWrite(IN2, HIGH);
//                    digitalWrite(IN3, HIGH);
//                    digitalWrite(IN4, LOW);
//                    digitalWrite(IN5, LOW);
//                    digitalWrite(IN6, HIGH);
//                    digitalWrite(IN7, HIGH);
//                    digitalWrite(IN8, LOW);
//                    softPwmWrite(ENA, 100);
//                    softPwmWrite(ENA2, 100);
//                    softPwmWrite(ENA3, 100);
//                    softPwmWrite(ENA4, 100);
//                    serialPuts(fd, "Turn Right\n");
//                    std::cout << "右旋转" << std::endl;
//                    break;
                default:
                    std::cout << "Unknown command" << std::endl;
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 延时 100 毫秒
        }
        serialClose(fd); // 关闭串口
    } catch (...) {
        std::cerr << "Error in Bluetooth communication." << std::endl;
    }
}

// 获取距离的函数（超声波测距）
float getDistance() {
    float distances[3];
    for (int i = 0; i < 3; ++i) {
        digitalWrite(TRIG, HIGH);
        std::this_thread::sleep_for(std::chrono::microseconds(10)); // 触发高电平 10 微秒
        digitalWrite(TRIG, LOW);

        while (digitalRead(ECHO) == LOW) {} // 等待回声开始
        auto start = std::chrono::high_resolution_clock::now(); // 记录开始时间
        while (digitalRead(ECHO) == HIGH) {} // 等待回声结束
        auto end = std::chrono::high_resolution_clock::now(); // 记录结束时间

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); // 计算回声持续时间
        distances[i] = (duration * 0.0343) / 2; // 计算距离，单位为 cm
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 延时 100 毫秒
    }
    return *std::max_element(distances, distances + 3); // 返回最大距离
}

// 避障程序
void obstacleAvoidance() {
    try {
        while (!stop_program) {
            float distance = getDistance(); // 获取当前距离
            std::cout << "Current distance: " << distance << " cm" << std::endl;std::cout << "Current distance: " << distance << " cm" << std::endl;

            if (distance < 15) { // 如果距离小于 15 cm
                std::cout << "Obstacle detected, moving backward!" << std::endl;
                controlMotor(IN2, IN1, ENA, 50);
                controlMotor(IN4, IN3, ENA2, 50);
                controlMotor(IN6, IN5, ENA3, 50);
                controlMotor(IN8, IN7, ENA4, 50);
                std::this_thread::sleep_for(std::chrono::seconds(1)); // 后退 1 秒
                stopMotor(); // 停止电机
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // 延时 500 毫秒
        }
    } catch (...) {
        std::cerr << "Error in obstacle avoidance." << std::endl;
    }
}

// 主程序入口
void blueMain() {
    setupGPIO(); // 初始化 GPIO
    std::thread blueThread(blue); // 创建蓝牙线程
    blueThread.join(); // 等待蓝牙线程结束
    stopMotor(); // 停止所有电机

}

#pragma once
#ifndef FUNC_H
#define FUNC_H

#include<easyx.h>
#include<vector>
#include<time.h>
using namespace std;

// 工具函数
void drawAlpha(IMAGE* img, int x, int y);//透明化图片
bool timerArrive(int id, int ms); // 冷却时间控制

#endif
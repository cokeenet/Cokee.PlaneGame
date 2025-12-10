#pragma once
#include <vector>
#include <iostream>
#include <easyx.h>
#include "func.h"
#include "Game.h"

#ifndef PLANEGAME_H
#define PLANEGAME_H

// --- 游戏常量定义 ---
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 700
#define ENEMY_NUM 20
#define BULLET_NUM 15
#define BULLET_SPEED 5
#define ENEMY_SPEED 0.5
#define PLAYER_MAX_HP 100
#define PLAYER_DAMAGE_PER_COLLISION 20
#define ENEMY_HP 3 // 默认敌机血量，但在 CreateEnemy 中会根据类型重置

// 敌人类型枚举
enum EnemyType
{
	ENEMY_SMALL = 1,
	ENEMY_MEDIUM = 2,
	ENEMY_LARGE = 3
};

using namespace std;

struct GameObject
{
	int x, y;
	int width, height;
	bool alive;
	int speed;
	int type;
	int hp;
	int animationFrame; // 当前动画帧
	DWORD lastAnimTime; // 上次动画更新时间
};

class PlaneGame
{
protected:
	GameObject player;
	vector<GameObject> enemy;
	vector<GameObject> bullet;

	void PlayerMove();
	void CreateBullet();
	void BulletMove();
	void CreateEnemy();
	void EnemyMove();
	void EnemyDead();
	void PlayerDead();

public:
	IMAGE imgBackground;
	IMAGE imgPlayer;
	IMAGE imgEnemy1;		// 小敌机
	IMAGE imgEnemy1Down[4]; // 小敌机爆炸动画
	IMAGE imgEnemy2;		// 中敌机
	IMAGE imgEnemy2Down[4]; // 中敌机爆炸动画
	IMAGE imgEnemy3;		// 大敌机
	IMAGE imgEnemy3Down[6]; // 大敌机爆炸动画
	IMAGE imgBullet;
	double BackgroundY = 0;

	// 构造函数：用于初始化 vector 大小
	PlaneGame() : enemy(ENEMY_NUM), bullet(BULLET_NUM) {}

	void InitializeGame(); // 初始化
	void Run();
	void Draw();
	void DrawEnemy(int index);
	void DrawHealthBar(int x, int y, int width, int height, int currentHP, int maxHP);
};
#endif
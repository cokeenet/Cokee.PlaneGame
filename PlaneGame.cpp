#include "func.h"
#include "PlaneGame.h"
#include <easyx.h>
#include <tchar.h>
#include <Windows.h>
#include <time.h>

void PlaneGame::InitializeGame()
{
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
	player.x = WINDOW_WIDTH / 2 - 40;
	player.y = WINDOW_HEIGHT - 100;
	player.speed = 5;
	player.alive = true;
	player.width = 102;
	player.height = 126;
	player.hp = PLAYER_MAX_HP; // 初始化玩家血量

	loadimage(&imgBackground, _T("./images/background.png"), WINDOW_WIDTH, WINDOW_HEIGHT);
	loadimage(&imgPlayer, _T("./images/me.png"), player.width, player.height);
	loadimage(&imgBullet, _T("./images/bullet.png"), 5, 11);

	// 加载不同类型敌人的图片
	loadimage(&imgEnemy1, _T("./images/enemy1.png"), 46, 35);
	loadimage(&imgEnemy1Down[0], _T("./images/enemy1_down1.png"), 46, 35);
	loadimage(&imgEnemy1Down[1], _T("./images/enemy1_down2.png"), 46, 35);
	loadimage(&imgEnemy1Down[2], _T("./images/enemy1_down3.png"), 46, 35);
	loadimage(&imgEnemy1Down[3], _T("./images/enemy1_down4.png"), 46, 35);

	loadimage(&imgEnemy2, _T("./images/enemy2.png"), 69, 99);
	loadimage(&imgEnemy2Down[0], _T("./images/enemy2_down1.png"), 69, 99);
	loadimage(&imgEnemy2Down[1], _T("./images/enemy2_down2.png"), 69, 99);
	loadimage(&imgEnemy2Down[2], _T("./images/enemy2_down3.png"), 69, 99);
	loadimage(&imgEnemy2Down[3], _T("./images/enemy2_down4.png"), 69, 99);

	loadimage(&imgEnemy3, _T("./images/enemy3.png"), 165, 261);
	loadimage(&imgEnemy3Down[0], _T("./images/enemy3_down1.png"), 165, 261);
	loadimage(&imgEnemy3Down[1], _T("./images/enemy3_down2.png"), 165, 261);
	loadimage(&imgEnemy3Down[2], _T("./images/enemy3_down3.png"), 165, 261);
	loadimage(&imgEnemy3Down[3], _T("./images/enemy3_down4.png"), 165, 261);
	loadimage(&imgEnemy3Down[4], _T("./images/enemy3_down5.png"), 165, 261);
	loadimage(&imgEnemy3Down[5], _T("./images/enemy3_down6.png"), 165, 261);

	srand((unsigned int)time(NULL));
	HWND hWnd = GetHWnd();
	SetWindowTextA(hWnd, "飞机大战游戏");
}

// 绘制血条函数
void PlaneGame::DrawHealthBar(int x, int y, int width, int height, int currentHP, int maxHP)
{
	// 血条参数
	int barWidth = width;
	int barHeight = height;
	int barX = x;
	int barY = y;

	// 绘制背景（灰色）
	setfillcolor(RGB(100, 100, 100));
	setlinecolor(RGB(50, 50, 50));
	setbkmode(TRANSPARENT);
	solidroundrect(barX, barY, barX + barWidth, barY + barHeight, 2, 2);

	// 绘制血量（根据当前血量比例填充绿色到红色）
	int hpWidth = (int)((float)currentHP / maxHP * barWidth);
	if (hpWidth > 0)
	{
		// 根据血量比例计算颜色（绿色到红色）
		int red = (int)(255 * (1.0f - (float)currentHP / maxHP));
		int green = (int)(255 * ((float)currentHP / maxHP));
		setfillcolor(RGB(red, green, 0));
		setlinecolor(RGB(red / 2, green / 2, 0));
		solidroundrect(barX, barY, barX + hpWidth, barY + barHeight, 2, 2);
	}
}

// 新增函数：根据敌人类型绘制敌人
void PlaneGame::DrawEnemy(int index)
{
	if (index < 0 || index >= ENEMY_NUM)
		return;

	GameObject &e = enemy[index];
	// 只绘制存活的敌人
	if (!e.alive)
		return;

	// 如果敌人正在爆炸，绘制爆炸动画
	if (e.hp <= 0)
	{
		switch (e.type)
		{
		case ENEMY_SMALL:
			if (e.animationFrame < 4)
				drawAlpha(&imgEnemy1Down[e.animationFrame], e.x, e.y);
			break;
		case ENEMY_MEDIUM:
			if (e.animationFrame < 4)
				drawAlpha(&imgEnemy2Down[e.animationFrame], e.x, e.y);
			break;
		case ENEMY_LARGE:
			if (e.animationFrame < 6)
				drawAlpha(&imgEnemy3Down[e.animationFrame], e.x, e.y);
			break;
		}
	}
	else
	{
		// 正常绘制敌人
		switch (e.type)
		{
		case ENEMY_SMALL:
			drawAlpha(&imgEnemy1, e.x, e.y);
			break;
		case ENEMY_MEDIUM:
			drawAlpha(&imgEnemy2, e.x, e.y);
			break;
		case ENEMY_LARGE:
			drawAlpha(&imgEnemy3, e.x, e.y);
			break;
		default:
			break;
		}

		// 绘制敌机血条（仅对有血量的敌人）
		if (e.hp > 0)
		{
			// 根据敌人类型确定血条尺寸
			int barWidth = e.width;
			int barHeight = 6;
			int barX = e.x;
			int barY = e.y - 10; // 血条在敌人上方

			DrawHealthBar(barX, barY, barWidth, barHeight, e.hp, ENEMY_HP);
		}
	}
}

void PlaneGame::Draw()
{
	// 实现背景滚动效果
	BackgroundY += 0.2; // 滚动速度
	if (BackgroundY >= WINDOW_HEIGHT)
	{
		BackgroundY = 0;
	}

	// 绘制背景图片两次以实现无缝滚动
	putimage(0, BackgroundY, &imgBackground);
	putimage(0, BackgroundY - WINDOW_HEIGHT, &imgBackground);

	// 绘制玩家血条
	DrawHealthBar(10, 10, 200, 20, player.hp, PLAYER_MAX_HP);

	// 绘制玩家
	drawAlpha(&imgPlayer, player.x, player.y);

	// 绘制敌人
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		DrawEnemy(i);
	}

	// 绘制子弹
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].alive)
			drawAlpha(&imgBullet, bullet[i].x, bullet[i].y);
	}
}
void PlaneGame::BulletMove()
{
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].alive)
		{
			bullet[i].y = bullet[i].y - BULLET_SPEED;
			if (bullet[i].y <= 0)
			{
				bullet[i].alive = false;
			}
		}
	}
}
void PlaneGame::EnemyMove()
{
	DWORD currentTime = clock();

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].alive)
		{
			enemy[i].y += enemy[i].speed;

			// 更新爆炸动画帧
			if (enemy[i].hp <= 0)
			{
				if (currentTime - enemy[i].lastAnimTime > 100) // 每100毫秒切换一帧
				{
					enemy[i].animationFrame++;
					enemy[i].lastAnimTime = currentTime;

					// 动画播放完毕后销毁敌人
					bool animFinished = false;
					switch (enemy[i].type)
					{
					case ENEMY_SMALL:
						animFinished = (enemy[i].animationFrame >= 4);
						break;
					case ENEMY_MEDIUM:
						animFinished = (enemy[i].animationFrame >= 4);
						break;
					case ENEMY_LARGE:
						animFinished = (enemy[i].animationFrame >= 6);
						break;
					}

					if (animFinished)
					{
						// 完全重置敌机状态
						enemy[i].alive = false;
						enemy[i].hp = 0;
						enemy[i].animationFrame = 0;
						enemy[i].x = 0;
						enemy[i].y = 0;
					}
				}
			}
			// 敌人飞出屏幕底部则销毁
			else if (enemy[i].y > WINDOW_HEIGHT)
			{
				// 完全重置敌机状态
				enemy[i].alive = false;
				enemy[i].hp = 0;
				enemy[i].animationFrame = 0;
				enemy[i].x = 0;
				enemy[i].y = 0;
			}
		}
	}
}

void PlaneGame::EnemyDead()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].alive && enemy[i].hp > 0) // 只检测未被击毁的敌人
		{
			for (int j = 0; j < BULLET_NUM; j++)
			{
				if (bullet[j].alive)
				{
					// 简单的碰撞检测
					if (bullet[j].x >= enemy[i].x &&
						bullet[j].x <= enemy[i].x + enemy[i].width &&
						bullet[j].y >= enemy[i].y &&
						bullet[j].y <= enemy[i].y + enemy[i].height)
					{
						// 减少敌机血量（每次命中减少1点血量）
						enemy[i].hp--;
						bullet[j].alive = false;

						// 敌人生命值降到0时开始爆炸动画
						if (enemy[i].hp <= 0)
						{
							enemy[i].animationFrame = 0;
							enemy[i].lastAnimTime = clock();
							// 注意：这里不立即设置alive=false，因为敌机需要播放爆炸动画
						}
						break;
					}
				}
			}
		}
	}
}

void PlaneGame::PlayerDead()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].alive)
		{
			// 更精确的碰撞检测（矩形相交检测）
			if (player.x + player.width > enemy[i].x &&
				player.x < enemy[i].x + enemy[i].width &&
				player.y + player.height > enemy[i].y &&
				player.y < enemy[i].y + enemy[i].height)
			{
				// 碰撞发生，扣除玩家血量
				player.hp -= PLAYER_DAMAGE_PER_COLLISION;
				if (player.hp <= 0)
				{
					player.hp = 0;
					player.alive = false;
				}

				// 敌机也被摧毁，进入爆炸状态
				enemy[i].hp = 0;
				enemy[i].animationFrame = 0;
				enemy[i].lastAnimTime = clock();
				// 注意：这里不立即设置alive=false，因为敌机需要播放爆炸动画
			}
		}
	}
}

void PlaneGame::CreateEnemy()
{
	static DWORD lastCreateTime = 0;
	DWORD currentTime = clock();

	// 控制敌人生成频率
	if (currentTime - lastCreateTime < 100) // 100毫秒生成一次敌人批次
		return;

	lastCreateTime = currentTime;

	// 尝试创建多个敌人（最多3个）
	int enemiesToCreate = 1 + rand() % 3; // 随机创建1-3个敌人

	for (int i = 0; i < ENEMY_NUM && enemiesToCreate > 0; i++)
	{
		// 只在敌人完全不存在时（不仅alive为false，而且不在爆炸动画中）才创建新敌人
		if (!enemy[i].alive && enemy[i].hp <= 0)
		{
			// 随机生成敌人类型（按要求调整概率分布）
			int enemyType = rand() % 100;

			if (enemyType < 40) // 40%概率生成小敌机
			{
				enemy[i].type = ENEMY_SMALL;
				enemy[i].width = 46;
				enemy[i].height = 35;
				enemy[i].hp = 3; // 设置为3点血量，三发子弹打爆
				enemy[i].speed = ENEMY_SPEED;
			}
			else if (enemyType < 80) // 40%概率生成中敌机
			{
				enemy[i].type = ENEMY_MEDIUM;
				enemy[i].width = 69;
				enemy[i].height = 99;
				enemy[i].hp = 3; // 设置为3点血量，三发子弹打爆
				enemy[i].speed = ENEMY_SPEED * 0.7;
			}
			else // 20%概率生成大敌机
			{
				enemy[i].type = ENEMY_LARGE;
				enemy[i].width = 165;
				enemy[i].height = 261;
				enemy[i].hp = 3; // 设置为3点血量，三发子弹打爆
				enemy[i].speed = ENEMY_SPEED * 0.5;
			}

			enemy[i].x = rand() % (WINDOW_WIDTH - enemy[i].width);
			enemy[i].y = 0 - enemy[i].height;
			enemy[i].alive = true;
			enemy[i].animationFrame = 0;
			enemy[i].lastAnimTime = currentTime;

			enemiesToCreate--;
		}
	}
}

void PlaneGame::CreateBullet()
{
	if (GetAsyncKeyState(VK_SPACE))
		for (int i = 0; i < BULLET_NUM; i++)
		{
			if (!bullet[i].alive)
			{
				bullet[i].x = player.x + player.width / 2;
				bullet[i].y = player.y;
				bullet[i].alive = true;
				break;
			}
		}
}

void PlaneGame::PlayerMove()
{
	if (GetAsyncKeyState(VK_UP))
	{
		player.y = player.y - 5;
		if (player.y < 0)
			player.y = 0;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		player.y = player.y + 5;
		if (player.y > WINDOW_HEIGHT - 80)
			player.y = WINDOW_HEIGHT - 80;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		player.x = player.x - 5;
		if (player.x < 0)
			player.x = 0;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		player.x = player.x + 5;
		if (player.x > WINDOW_WIDTH - 63)
			player.x = WINDOW_WIDTH - 63;
	}
}

void PlaneGame::Run()
{

	BeginBatchDraw(); // 开启批量绘制
	while (1)
	{
		Draw();
		// 循环判断有没有按键，移动player
		if (timerArrive(0, 10))
			PlayerMove();
		if (timerArrive(1, 200))
			CreateBullet();
		if (timerArrive(2, 100)) // 更频繁地尝试生成敌人
			CreateEnemy();
		EnemyMove();
		EnemyDead();
		PlayerDead();
		BulletMove();

		FlushBatchDraw();
		// 有没有ESC键按下
		if (GetAsyncKeyState(VK_ESCAPE))
			break;
	}
	closegraph();
}

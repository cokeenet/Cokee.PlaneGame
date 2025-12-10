#include "func.h"
#include "PlaneGame.h"
#include <easyx.h>
#include <tchar.h>

// 修复宏冲突的关键：阻止 Windows.h 定义 min/max 宏
#define NOMINMAX

#include <Windows.h>
#include <time.h>
#include <algorithm> // 用于 std::min 和 std::max

using namespace std;

// 计时器ID定义（用于 timerArrive 函数的第一个参数）
// 范围 0-99 (对应 vector<DWORD> t(100))
#define TIMER_ID_ENEMY_CREATE 1 // 敌人生成计时器
#define TIMER_ID_BULLET_SHOT 2	// 子弹发射计时器

// 辅助函数：限制玩家移动范围
void ClampPlayerPosition(GameObject &player)
{
	player.x = min(player.x, WINDOW_WIDTH - player.width);
	player.y = min(player.y, WINDOW_HEIGHT - player.height);
}

void PlaneGame::InitializeGame()
{
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);

	// 确保向量有足够的大小
	if (enemy.empty())
		enemy.resize(ENEMY_NUM);
	if (bullet.empty())
		bullet.resize(BULLET_NUM);

	// 手动初始化所有敌机和子弹为“死亡”状态，确保干净启动
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemy[i].alive = false;
		enemy[i].hp = 0;
		enemy[i].x = 0;
		enemy[i].y = 0;
	}
	for (int i = 0; i < BULLET_NUM; i++)
	{
		bullet[i].alive = false;
	}

	// 统一初始化玩家属性
	player.width = 102;
	player.height = 126;
	player.speed = 5;
	player.x = WINDOW_WIDTH / 2 - player.width / 2;
	player.y = WINDOW_HEIGHT - 100 - player.height / 2;
	player.alive = true;
	player.hp = PLAYER_MAX_HP;

	loadimage(&imgBackground, _T("./images/background.png"), WINDOW_WIDTH, WINDOW_HEIGHT);
	loadimage(&imgPlayer, _T("./images/me.png"), player.width, player.height);
	loadimage(&imgBullet, _T("./images/bullet.png"), 5, 11);

	// 加载敌机图片
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

// 绘制血条函数 - 保持不变
void PlaneGame::DrawHealthBar(int x, int y, int width, int height, int currentHP, int maxHP)
{
	int barWidth = width;
	int barHeight = height;
	int barX = x;
	int barY = y;

	setfillcolor(RGB(100, 100, 100));
	setlinecolor(RGB(50, 50, 50));
	setbkmode(TRANSPARENT);
	solidroundrect(barX, barY, barX + barWidth, barY + barHeight, 2, 2);

	int hpWidth = (int)((float)currentHP / maxHP * barWidth);
	if (hpWidth > 0)
	{
		int red = (int)(255 * (1.0f - (float)currentHP / maxHP));
		int green = (int)(255 * ((float)currentHP / maxHP));
		setfillcolor(RGB(red, green, 0));
		setlinecolor(RGB(red / 2, green / 2, 0));
		solidroundrect(barX, barY, barX + hpWidth, barY + barHeight, 2, 2);
	}
}

// 新增函数：根据敌人类型绘制敌人 - 保持不变
void PlaneGame::DrawEnemy(int index)
{
	if (index < 0 || index >= ENEMY_NUM)
		return;

	GameObject &e = enemy[index];
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
			int barWidth = e.width;
			int barHeight = 6;
			int barX = e.x;
			int barY = e.y - 10;

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
	putimage(0, (int)BackgroundY, &imgBackground);
	putimage(0, (int)BackgroundY - WINDOW_HEIGHT, &imgBackground);

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
			bullet[i].y -= BULLET_SPEED;
			if (bullet[i].y <= -11)
			{
				bullet[i].alive = false;
			}
		}
	}
}

void PlaneGame::EnemyMove()
{
	// 注意：这里仍然使用 clock() 来控制动画的帧率，因为它与运动逻辑分开，且与游戏主循环无关。
	DWORD currentTime = clock();
	const int FRAME_DURATION_SMALL_MEDIUM = 100;
	const int FRAME_DURATION_LARGE = 80;

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		GameObject &e = enemy[i];

		if (e.alive)
		{
			e.y += e.speed;

			// 敌人飞出屏幕底部则销毁（仅对存活且未爆炸的敌人）
			if (e.hp > 0 && e.y > WINDOW_HEIGHT)
			{
				e.alive = false;
				e.hp = 0;
				e.animationFrame = 0;
				e.x = 0;
				e.y = 0;
			}
			// 更新爆炸动画帧
			else if (e.hp <= 0)
			{
				int duration = (e.type == ENEMY_LARGE) ? FRAME_DURATION_LARGE : FRAME_DURATION_SMALL_MEDIUM;

				if ((currentTime - e.lastAnimTime) > duration)
				{
					e.animationFrame++;
					e.lastAnimTime = currentTime;

					bool animFinished = false;
					switch (e.type)
					{
					case ENEMY_SMALL:
					case ENEMY_MEDIUM:
						animFinished = (e.animationFrame >= 4);
						break;
					case ENEMY_LARGE:
						animFinished = (e.animationFrame >= 6);
						break;
					}

					if (animFinished)
					{
						e.alive = false;
						e.hp = 0;
						e.animationFrame = 0;
						e.x = 0;
						e.y = 0;
					}
				}
			}
		}
	}
}

void PlaneGame::EnemyDead()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		GameObject &e = enemy[i];
		if (e.alive && e.hp > 0)
		{
			for (int j = 0; j < BULLET_NUM; j++)
			{
				GameObject &b = bullet[j];
				if (b.alive)
				{
					// 简单的碰撞检测
					if (b.x >= e.x &&
						b.x <= e.x + e.width &&
						b.y >= e.y &&
						b.y <= e.y + e.height)
					{
						e.hp--;
						b.alive = false; // 子弹销毁

						if (e.hp <= 0)
						{
							// 敌人生命值降到0时开始爆炸动画
							e.animationFrame = 0;
							e.lastAnimTime = clock();
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
		GameObject &e = enemy[i];

		if (e.alive && e.hp > 0)
		{
			// 精确的矩形相交检测
			if (player.x < e.x + e.width &&
				player.x + player.width > e.x &&
				player.y < e.y + e.height &&
				player.y + player.height > e.y)
			{
				player.hp -= PLAYER_DAMAGE_PER_COLLISION;
				player.hp = std::max(0, player.hp); // 确保血量不为负

				if (player.hp <= 0)
				{
					player.alive = false;
				}

				// 敌机也被摧毁，进入爆炸状态
				e.hp = 0;
				e.animationFrame = 0;
				e.lastAnimTime = clock();
			}
		}
	}
}

void PlaneGame::CreateEnemy()
{
	// >>> 替换：移除静态计时逻辑，由 Run 函数中的 timerArrive 控制 <<<
	// const DWORD CREATE_INTERVAL = 1000; // 1秒生成一批敌人
	const int MAX_BATCH_SIZE = 4;

	// Note: 现在的 CreateEnemy 每次被 Run() 调用时，如果 timerArrive(1, 1000) 返回 true，
	// 就会执行一次生成批次。

	int enemiesToCreate = 1 + rand() % MAX_BATCH_SIZE;

	for (int i = 0; i < ENEMY_NUM && enemiesToCreate > 0; i++)
	{
		GameObject &e = enemy[i];

		// 只在敌机槽位完全空闲时创建新敌机
		if (!e.alive && e.hp <= 0)
		{
			int enemyType = rand() % 100;

			int hpValue;
			int widthValue;
			int heightValue;
			float speedFactor;

			if (enemyType < 40) // 40% 小敌机
			{
				e.type = ENEMY_SMALL;
				hpValue = 1;
				widthValue = 46;
				heightValue = 35;
				speedFactor = 1.0f;
			}
			else if (enemyType < 80) // 40% 中敌机
			{
				e.type = ENEMY_MEDIUM;
				hpValue = 3;
				widthValue = 69;
				heightValue = 99;
				speedFactor = 0.7f;
			}
			else // 20% 大敌机
			{
				e.type = ENEMY_LARGE;
				hpValue = 8;
				widthValue = 165;
				heightValue = 261;
				speedFactor = 0.5f;
			}

			e.width = widthValue;
			e.height = heightValue;
			e.hp = hpValue;
			e.speed = ENEMY_SPEED * speedFactor;

			e.x = rand() % (WINDOW_WIDTH - e.width);
			e.y = 0 - e.height;
			e.alive = true;
			e.animationFrame = 0;
			e.lastAnimTime = clock(); // 爆炸计时仍使用 clock()
			enemiesToCreate--;
		}
	}
}

void PlaneGame::CreateBullet()
{
	// >>> 替换：移除静态计时逻辑，由 Run 函数中的 timerArrive 控制 <<<
	// 仅在玩家存活时尝试发射子弹
	if (player.alive && GetAsyncKeyState(VK_SPACE))
	{
		// Note: 射速现在由 Run() 中的 timerArrive(2, 150) 控制

		for (int i = 0; i < BULLET_NUM; i++)
		{
			if (!bullet[i].alive)
			{
				bullet[i].x = player.x + player.width / 2 - 2;
				bullet[i].y = player.y;
				bullet[i].alive = true;
				// lastShotTime = currentTime; // 移除静态时间记录
				break;
			}
		}
	}
}

void PlaneGame::PlayerMove()
{
	int dx = 0;
	int dy = 0;

	if (GetAsyncKeyState(VK_UP))
		dy -= player.speed;
	if (GetAsyncKeyState(VK_DOWN))
		dy += player.speed;
	if (GetAsyncKeyState(VK_LEFT))
		dx -= player.speed;
	if (GetAsyncKeyState(VK_RIGHT))
		dx += player.speed;

	player.x += dx;
	player.y += dy;

	ClampPlayerPosition(player);
}

void PlaneGame::Run()
{
	BeginBatchDraw();
	while (player.alive)
	{
		Draw();

		PlayerMove();

		// >>> 核心修改：使用 timerArrive 控制子弹发射和敌人生成 <<<

		// 子弹发射：每 150 毫秒尝试发射一次 (使用 TIMER_ID_BULLET_SHOT = 2)
		if (timerArrive(TIMER_ID_BULLET_SHOT, 150))
		{
			CreateBullet();
		}

		// 敌人生成：每 1000 毫秒（1秒）尝试生成一批敌人 (使用 TIMER_ID_ENEMY_CREATE = 1)
		if (timerArrive(TIMER_ID_ENEMY_CREATE, 1000))
		{
			CreateEnemy();
		}
		// -------------------------------------------------------------

		BulletMove();
		EnemyMove();
		EnemyDead();
		PlayerDead();

		FlushBatchDraw();

		if (GetAsyncKeyState(VK_ESCAPE))
			break;
	}

	// 游戏结束画面（可选）
	if (!player.alive)
	{
		settextcolor(RED);
		settextstyle(50, 0, _T("微软雅黑"));
		RECT r = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
		drawtext(_T("G A M E   O V E R"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		FlushBatchDraw();
		Sleep(3000);
	}

	EndBatchDraw();
	closegraph();
}
#pragma once

/**
 * 抽象游戏类，定义所有游戏都应该有的基本功能
 */
class Game
{
protected:
    int windowWidth;
    int windowHeight;

public:
    /**
     * 构造函数
     */
    Game(int width, int height) : windowWidth(width), windowHeight(height) {}

    /**
     * 虚析构函数，确保派生类能正确析构
     */
    virtual ~Game() = default;

    /**
     * 初始化游戏
     */
    virtual void InitializeGame() = 0;

    /**
     * 游戏主循环
     */
    virtual void Run() = 0;

    /**
     * 绘制游戏画面
     */
    virtual void Draw() = 0;

    /**
     * 获取窗口宽度
     */
    int GetWindowWidth() const { return windowWidth; }

    /**
     * 获取窗口高度
     */
    int GetWindowHeight() const { return windowHeight; }
};
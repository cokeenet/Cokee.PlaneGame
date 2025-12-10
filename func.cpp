#include "func.h"

// 绘制透明图片
void drawAlpha(IMAGE *img, int x, int y)
{
    // 初始化
    DWORD *dst = GetImageBuffer();
    DWORD *draw = GetImageBuffer();
    DWORD *src = GetImageBuffer(img);      // 获取picture源指针
    int picture_width = img->getwidth();   // 获取图片宽度
    int picture_height = img->getheight(); // 获取图片高度
    int graphWidth = getwidth();           // 获取屏幕宽度
    int graphHeight = getheight();         // 获取屏幕高度
    int dstX = 0;                          // 源像素的索引

    for (int iy = 0; iy < picture_height; iy++)
    {
        for (int ix = 0; ix < picture_width; ix++)
        {
            int srcX = ix + iy * picture_width;
            int sa = ((src[srcX] & 0xff000000) >> 24);
            int sr = ((src[srcX] & 0xff0000) >> 16); // 获取RGB中的R
            int sg = ((src[srcX] & 0xff00) >> 8);    // G
            int sb = src[srcX] & 0xff;               // B

            // 检查目标坐标是否在有效范围内
            if (ix + x >= 0 && ix + x < graphWidth && iy + y >= 0 && iy + y < graphHeight)
            {
                dstX = (ix + x) + (iy + y) * graphWidth;
                int dr = ((dst[dstX] & 0xff0000) >> 16);
                int dg = ((dst[dstX] & 0xff00) >> 8);
                int db = dst[dstX] & 0xff;

                // 执行透明度混合计算并绘制
                draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16) | ((sg * sa / 255 + dg * (255 - sa) / 255) << 8) | (sb * sa / 255 + db * (255 - sa) / 255);
            }
        }
    }
}

// 等待时间到达
bool timerArrive(int id, int ms)
{
    static vector<DWORD> t(100);
    if (clock() - t[id] > ms)
    {
        t[id] = clock();
        return true;
    }
    return false;
}

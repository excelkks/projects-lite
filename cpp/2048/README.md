# 2048 游戏的终端实现

## TODO
- 目前只能在空白出随机出现2，应当根据实际情况随机出现2/4/8...等，且概率分布要重新设定
 
## 终端键盘事件
为了不需要在每次按下键盘后都要使用enter键来驱动运行，设计了键盘事件监听函数`void setBufferedInput(bool enable)`，参考[https://zhuanlan.zhihu.com/p/328417142](https://zhuanlan.zhihu.com/p/328417142)

```c++
void setBufferedInput(bool enable) {
  static bool enabled = true;
  static struct termios old;
  struct termios new;

  if (enable && !enabled) {
    // restore the former settings
    tcsetattr(STDIN_FILENO,TCSANOW,&old);
    // set the new state
    enabled = true;
  } else if (!enable && enabled) {
    // get the terminal settings for standard input
    tcgetattr(STDIN_FILENO,&new);
    // we want to keep the old setting to restore them at the end
    old = new;
    // disable canonical mode (buffered i/o) and local echo
    new.c_lflag &=(~ICANON & ~ECHO);
    // set the new settings immediately
    tcsetattr(STDIN_FILENO,TCSANOW,&new);
    // set the new state
    enabled = false;
  }
}
```

## 终端显示
```c++
#include <unistd.h>  
  
// 清除屏幕  
printf("\033[2J")  
  
// 上移光标  
printf("\033[%dA", (x))  
  
// 下移光标  
printf("\033[%dB", (x))  
  
// 左移光标  
printf("\033[%dD", (y))  
  
// 右移光标  
printf("\033[%dC",(y))  
  
// 定位光标  
printf("\033[%d;%dH", (x), (y))  
  
// 光标复位  
printf("\033[H")  
  
// 隐藏光标  
printf("\033[?25l")  
  
// 显示光标  
printf("\033[?25h")  
  
//反显  
printf("\033[7m")  

// 恢复反显
printf("\033[27m")  
```


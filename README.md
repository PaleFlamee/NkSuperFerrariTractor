# 1.赛题
      A   B  
     (     )  
      D   C  
1. A->B  B.
2. A->B  B~>A  A.
3. A->B  B=>C  C->D  D=>A  A.
4. 把(3)做两遍

其中，->表示空白线，~>表示空白线倒车，=>表示一般巡线，.表示停车  
每个动作之间都要有声光提示

# 2.实现
### 1.核心架构
由于PID进程不可停止，除`setup()`以外的代码均采用 **定时器-中断** 结构以实现类似多线程的效果  
`ATmega168`拥有`Timer0` `Timer1` `Timer2`三个硬件定时器  

`Timer0`提供的`millis()`用于创建软件定时器
`Timer1`拥有最高的精度，用于PID和IMU/LTM数据的更新  
`Timer2`用于产生PWM信号  

以上均通过全局变量沟通

### 2.运行逻辑
1. `setup()`函数
    1. 各项功能初始化
    2. 通过**2P拨码开关**判断并标记小车所处的题目，
2. `loop()`函数
    1. 通过软件定时器a检测小车运行进度，在需要时切换PID运行模式，开启/关闭声光模块开关  
    2. 通过软件定时器b调节声光模块  
3. `Timer1_ISR()`函数
    1. 执行PID  

### 3.代码
`.h`头文件与`.cpp`源文件成对出现，`.h`文件包含仅在相应的`.cpp`文件中出现的宏/函数/类型声明，`.cpp`文件包含具体的定义  
`NkSuperFerrariTractor.ino`是主要的源文件，包含`setup()` `loop()` `Timer1_ISR()`三个核心函数和`IMUData` `LTMData` `Title` `currentMode` `currentStage`全局变量  
`NkSuperFerrariTractor.h`是主要的头文件，包含以上的声明和编译开关

所有其他源文件都应该包含对应的头文件和`NkSuperFerrariTractor.h`  
|文件|作用|
|---|---|
|LightNSound||
|Motor||
|FetchData||
|PID||
|||

# RISC-V-Emulator
简单的RISC-V指令模拟器，实现了绝大多非扩展指令的模拟工作。

本模拟器可以跨平台执行。

命名规则：功能+作用对象（对象名称中的空格用下划线代替）

设计思路（模拟器架构）：
1. 设定模拟硬件环境
2. 解析ELF文件，将ELF文件中所有有用信息都提取出来并放到模拟硬件环境中
3. 执行在硬件环境中存储的二进制代码

模拟硬件环境：
1. 4e5字节内存
2. 共32个通用寄存器，0号寄存器常为0，2号寄存器为SP寄存器，3号寄存器为GP寄存器
3. 设置堆栈大小0x1000
4. PC寄存器单独存在。

ELF解析过程：
1. 读取ELF头，得到基础信息
2. 读取所有SECTION HEADER，并将所有信息存入模拟内存中。其中将.text的地址存入PC寄存器中作为代码执行初始位置

二进制执行代码过程：
1. 读取二进制代码
2. 解析其opcode，依据opcode执行不同指令函数
3. 不断执行此过程直至代码执行完成（遇到nop指令停止）

编译：
windows环境下需要配置GCC环境并执行run.bat即可，生成emulator.exe可执行程序
linux环境下需要配置GCC环境并执行run.sh即可，生成emulator可执行程序

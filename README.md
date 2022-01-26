# 30天自制操作系统

## 开发流程

1. 准备一块软盘
2. 写启动区
   1. 软盘内容装到内存：顺序：扇区S(1-18)->正反H(0,1)->柱面C(0-79)
   2. 设置显示模式作为测试
3. 内存写入，屏幕显示
   1. 显示窗口
   2. 设定调色板
   3. 尝试绘制图形
   4. 绘制文字
4. 分割编译与中断处理
   1. 分段分页
   2. 初始化PIC
   3. 信号中断
   4. 键盘鼠标的控制，键盘中断
5. 设置FIFO和栈
   1. 设置fifo
   2. 激活鼠标：鼠标fifo，鼠标控制电路，鼠标中断，鼠标控制
6. 内存管理
   1. 检查内存

## 执行流程

1. 重新创建GDT，IDT
2. 初始化PIC
3. 执行 io_sti()
4. 初始化调色板和画面

## CPU

- 只能处理 0 1 两个电信号
- 四位二进制构成一位十六进制
- cpu访问内存的速度要比访问寄存器慢很多
- `0xf0000`附近存放着BIOS程序，用户不能使用
- `0x00007c00-0x00007dff`启动区内容装载地址
- CPU和设备相连，out信号向设备发信息，in信号接收设备信息
- CPU进行模式转换时一定要屏蔽中断

## 汇编指令

- `;`：注释
- `DB`：写字符串
- `DW`：define world
- `DD`：define double world，4字节，32位
- `$`：1.输出之前字节数。2.将要读入的内存地址。
- `RESB	0x1fe-$`：填写0x00直到0x001fe
- `ORG`：指明程序的装载地址
- `JMP`：跳转
- `entry`：标签的声明，用于指定`JMP`的目的地
- `MOV`：复制赋值
- `[]`：表示内存地址  `MOV AL,[SI]`表示把SI地址的一字节内容读到AL中
- `ADD`：加法
- `CMP`：判断是否相等
- `JE`：条件跳转，如果比较结果相等跳转

~~~ARM
CMP AL,0
JE fin	;如果AL==0，跳转到fin
~~~

- `JB`：条件跳转，如果小于，就跳转
- `INT`：中断跳转
- `HLT`：让CPU停止动作，进入待机状态，等待外部发生变化CPU被唤醒。可用于防止CPU空转
- `EQU`：#define `CYLS EQU 10` == `#define CYLS 10`
- `PUSHFD`：将标志位的值按照双字长压入栈
- `POPFD`：将标志位的值按照双字长从栈弹出
- `NOP`：让CPU休息一个时钟长的时间

## 启动区

- 软盘第一个扇区称为启动区，如果最后两个字节不是`0x55 AA`计算机会认为这张盘没有所需的启动程序
- 计算机以512个字节位一个单位进行读写，称为一个扇区
- 由于操作系统往往很大，在启动区只放一个IPL（启动程序加载器）

## 寄存器

- `AX`：累加寄存器`AH AL`
- `CX`：计数寄存器`CH CL`可用作柱面号，扇区号
- `DX`：数据寄存器`DH DL`可用作磁头号，驱动器号

- `BX`：基址寄存器`BH BL`
- `SP`：栈指针寄存器
- `BP`：基址指针寄存器
- `SI`：源变址寄存器
- `DI`：目的变址寄存器
- 在这些16位寄存器前加E，就成了32位寄存器的表示
- 段寄存器
  - `ES`：附加段寄存器
  - `CS`：代码段寄存器
  - `SS`：栈段寄存器
  - `DS`：数据段寄存器,默认段寄存器
  - `FS  GS`：没有名称
- 标志寄存器`FLAGS`
  - `JC`：进位标志位是1，就跳转
  - `JNC`：进位标志位是0，就跳转

## 软盘

- 一张软盘有80个柱面，18个扇区，两个磁头
- 一个扇区有512个字节

$$
一张软盘的容量 = 80\times2\times18\times512=1474560Byte=1440KB
$$

- 含有ILP启动区，位于C0-H0-S1 ，CX代表柱面X号，H0代表正面,H1代表背面 ，SX表示扇区X号

<img src="README.assets/image-20220126073700384.png" alt="image-20220126073700384" style="zoom:80%;" />

- 读取下一个扇区

~~~ARM
MOV AX,ES	
ADD AX,0X0020	; == 512/16
MOV ES,AX	;前三步让内存地址ES后移512/16
ADD CL,1
CMP CL,18	；一共18个柱面，不能超了
~~~

- 向一个空软盘保存文件时，文件名会写在0x002600以后的地方，文件内容会写在0x004200以后的地方

## 高级语言编译过程

.c -> .gas

.gas -> .nas

.nas -> .obj(目标文件)

.obj -> .bim（二进制映像文件）

.bim -> .hrb

.bin + .hrb -> .sys

## 屏幕显示测试demo

~~~ARM
;写在源代码【画面モードを設定】后
		MOV		BX,DS
		MOV		AX,0xa000
		MOV		DS,AX
		MOV		SI,0	;初始化[DS:SI]  0xa0000 
draw:
		MOV		[SI],BYTE 15	;白色
		ADD		SI,1
		CMP		SI,320*100	;320*100窗口上半区
		JBE		draw
		MOV		DS,BX
~~~

<img src="README.assets/image-20220126090808559.png" alt="image-20220126090808559" style="zoom:50%;" />

## 指针的底层

~~~cpp
char *p;
p = (char*)i;
*p = i&0x0f;
~~~

~~~ARM
;equal to
	MOV ECX,i	;寄存器
	MOV [ECX],(i&0x0f)	;内存地址
~~~

~~~cpp
a[2];
2[a];
*(a+2);
*(2+a);
//等价

(*stu).val;
stu->val;
//等价
~~~

## 分段

- 按照自定义方式将内存分成很多块，每一块的起始地址都当作0处理，这样的块称为段
- 分段使用的是段寄存器
- 段的信息
  - 段的大小
  - 段的起始地址
  - 段的属性管理（禁止写入，禁止执行，系统专用等）
    - 本身12位和段上限的4位一起构成16位段属性

<img src="README.assets/image-20220126172928361.png" alt="image-20220126172928361" style="zoom: 80%;" />

<img src="README.assets/482E061E1CE4BAE9F51BE8E68991BF3D.png" alt="img" style="zoom:50%;" />

- GDT（全局段号记录表）放在内存中，记录段的起始地址
- 在CPU的GDTR存储内存的起始地址和有效设定个数
  - 低16位表示段上限，高32位表示表示GDT的开始地址
- 步骤 
  - `_load_gdtr`：限定段上限和地址值赋值给GDTR的48位寄存器，用到指令`LGDT`

![image-20220126174043598](README.assets/image-20220126174043598.png)

## 分页

- 段的上限是32位（4GB），存段信息时，如果直接存（本身4字节，基址4字节），已经占满32位，没地方管理属性了
- 有12位的段属性，所以段上限只能用20位，即1MB

- 在段的属性里设立一个Gbit标志位，当他是1的时候，limit的单位不解释为字节，而是页（一页是4KB）
- 因此 4KB * 1MB = 4GB

## 分割编译

- 函数分割，将函数放在不同.c文件，方便修改查找；但造成makefile复杂
- 用一般规则简化makefile，但如果写了特定的生成规则，那将优先，类似函数模板

~~~C
%.gas : %.c Makefile
    $(CC1) -o $*.gas $*.c
    
%.nas : %.gas Makefile
    $(GAS2NASK) -o $*.gas $*.nas
~~~

- 整理头文件，将重复的东西放到头文件并包含之

## 中断

- 轮询的话，有些应急事件无法及时响应，所以需要中断。而且CPU可以不把精力用在查询上，集中在处理任务上

- IDT（中断记录表），CPU遇到某些事件切换过去执行
- IDT记录了 （0~255）的中断号码和调用函数的对应关系

### 中断标志

中断标志为1表示立即处理中断

中断标志为0表示忽略中断请求

- `CLI`：将中断标志置0
- `STI`：将中断标志置1

### PIC

- 可编程中断控制器
- 将8个中断信号集合成一个中断信号，8个信号按照或的方式通知cpu
- 分类
  - 主PIC：负责0-7号中断信号
  - 从PIC：负责8-15号中断信号
  - 如果主pic不通知CPU，从pic的信号就不能传达给CPU

<img src="README.assets/BC1DAF8BCE30D2CDDB861A55E40CA672.png" alt="img" style="zoom:50%;" />

- PIC的寄存器
  - IMR：中断屏蔽寄存器，中断的时候屏蔽其他中断防止混乱
  - ICW：初始化控制数据，有四个ICW，但可支配的只有ICW2，它决定了IQR按照那种中断通知CPU
  - CPU收到中断信号，命令PIC发送两个字节数据，`0xcd 0xXX`等价于`INT 0xXX`，CPU执行对应的INT指令

### 中断处理程序

- 执行某指令后，IF（中断许可标志位）变为1，CPU接受来自外部设备的中断，执行对应的**中断处理程序**

- 中断函数执行完要重启对于中断的监视

### 加快中断处理

- 如果中断处理的太慢，就会影响接收其他中断
- 处理方法：将按键的编码接受下来，存到变量里，由程序偶尔查看一下这个变量，如果有数据就把他显示出来
- 一个变量还是慢，需要用一个队列存储

### FIFO

~~~c
struct KEYBUF{
    unsigned char data[32];
    int next_r,next_w,len;
}
//用一个队列存储输入的信息
//当指针到达队尾，重设为0
~~~

## 内存

### 分布图

![img](README.assets/1B9E7C468C5C6629DC5A8FF2E62CAE90.png)

### 高速缓冲存储器（cache memory）

- 在CPU里的存储器，容量小，速度快，价格贵
- 访问内存时，将内存的地址和内容存到cache里，下次用这个地址的信息，马上就能从cache拿到
- 往内存写数据时，先将数据写到cache里，缓存控制点路再配合内存速度慢慢写到内存里
- 遇到循环，再cache里计算后将结果存入内存，而非多次存入覆盖。

### 检查内存

- 写入内存一个值再读出来是否一致
- 此时需要关闭cache

### 内存管理

- 标记为1表示正在使用，标记为0表示空着

  - 内存分配：比如需要100KB的空间，以字节（4KB）为单位管理只要找到连续的25个0即可，并设为1

  - 内存释放：归还的时候用地址除以单位内存，将其设为0
  - 占用内存较多，存储着每块内存的状态。大块内存分配需要遍历很久

- 空间使用记录表
  - 从xxx开始的yyy字节的空间是空着的。
  - 内存分配：遍历找到合适大小的可用空间，将其从表中删除，如果这块空间大小变为0，就删除这条信息
  - 内存释放：表里增加一条可用空间信息，检查这段空间能否和相邻空间合并，能合并，将他们归纳为1条
  - 占用内存较小，大块内存分配释放迅速
  - 管理较为复杂，当空间管理表用完，先割舍小块内存，等到表有空余再放入；或者切换回上一种方法

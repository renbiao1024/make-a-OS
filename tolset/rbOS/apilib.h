void api_putchar(int c);//显示字符
void api_putstr0(char *s);//显示字符串
void api_putstr1(char *s, int l);//显示定长的字符串
void api_end(void);//退出
int api_openwin(char *buf, int xsiz, int ysiz, int col_inv, char *title);//打开窗口
void api_putstrwin(int win, int x, int y, int col, int len, char *str);//在窗口显示字符串
void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);//盒体
void api_initmalloc(void);//初始化大小
char *api_malloc(int size);
void api_free(char *addr, int size);//释放
void api_point(int win, int x, int y, int col);//点
void api_refreshwin(int win, int x0, int y0, int x1, int y1);//刷新
void api_linewin(int win, int x0, int y0, int x1, int y1, int col);//画线
void api_closewin(int win);//关闭窗口
int api_getkey(int mode);//获得按键
int api_alloctimer(void);//定时器
void api_inittimer(int timer, int data);
void api_settimer(int timer, int time);
void api_freetimer(int timer);
void api_beep(int tone);//蜂鸣器
int api_fopen(char *fname);//文件打开
void api_fclose(int fhandle);//文件关闭
void api_fseek(int fhandle, int offset, int mode);//文件查找
int api_fsize(int fhandle, int mode);//文件大小
int api_fread(char *buf, int maxsize, int fhandle);//读取文件
int api_cmdline(char *buf, int maxsize);//命令行
int api_getlang(void);//获取语言

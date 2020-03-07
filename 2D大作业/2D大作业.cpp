#include "stdafx.h"
#include <gl/glut.h>
#include <math.h>
#include <string>
#include <vector>
#include <time.h>
#include <stack>

#define Width 700  //屏幕宽
#define Height 700 //屏幕高
#define MAX_CHAR 128 //字符集总数
using namespace std;

typedef struct
{
	bool flag;
	int y;
	int x;
}pointInfo;
typedef struct
{
	int y;
	int x;
}edgeInfo;
class Maze
{
private:
	int width, height;
	int **maze;
	vector<pointInfo> pointSet;
	vector<edgeInfo> edgeSet;
	enum Type
	{
		road, //路
		wall, //墙
		start,//起点
		end,  //终点
		path  //DFS寻找的路径
	}type;

public:
	Maze(int w, int h)
	{
		width = 2 * w + 1;
		height = 2 * h + 1;
		//为迷宫数组动态分配内存
		maze = new int*[height];
		for (int i = 0; i < height; i++)
		{
			maze[i] = new int[width];
		}
		//初始化迷宫数组
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				maze[i][j] = wall;
				if (!(i == 0 || i == height - 1 || j == 0 || j == width - 1))
				{
					if (i % 2 == 1)
					{
						if (j % 2 == 1)
						{
							maze[i][j] = road;
							pointInfo point{ false, i, j };
							pointSet.push_back(point);
						}
					}
				}
			}
		}
		maze[1][0] = start;//起点
		maze[height - 2][width - 1] = end;//终点
	}

	//从点集合中随机选取一个点
	pointInfo choiceRandomPoint()
	{
		srand((unsigned)time(NULL));
		int index = rand() % pointSet.size();
		pointSet[index].flag = true;
		return pointSet[index];
	}

	//查看点的状态
	bool checkPointState(int x, int y)
	{
		for (int i = 0; i < pointSet.size(); i++)
			if (pointSet[i].x == x && pointSet[i].y == y)
				return pointSet[i].flag;
	}

	//从边集合中随机选取一条边
	edgeInfo choiceRandomEdge()
	{
		srand((unsigned)time(NULL));
		int index = rand() % edgeSet.size();
		edgeInfo edge = edgeSet[index];
		edgeSet.erase(edgeSet.begin() + index);
		return edge;
	}

	//查看与点相连的边
	void checkConnectedEdge(pointInfo point)
	{
		int x = point.x, y = point.y;
		edgeInfo edge;

		//判断是否在边界内
		if (x - 1 >= 1)
		{
			edge.x = x - 1;
			edge.y = y;
			edgeSet.push_back(edge);
		}
		if (x + 1 <= width - 2)
		{
			edge.x = x + 1;
			edge.y = y;
			edgeSet.push_back(edge);
		}
		if (y - 1 >= 1)
		{
			edge.x = x;
			edge.y = y - 1;
			edgeSet.push_back(edge);
		}
		if (y + 1 <= height - 2)
		{
			edge.x = x;
			edge.y = y + 1;
			edgeSet.push_back(edge);
		}
	}

	//查看是否所有的点都被访问过了
	bool checkAllPoint()
	{
		bool flag = true;
		for (int i = 0; i < pointSet.size(); i++)
			if (pointSet[i].flag == false)
				flag = false;
		return flag;
	}

	//设置点的状态
	void SetPointState(pointInfo point)
	{
		for (int i = 0; i < pointSet.size(); i++)
			if (pointSet[i].x == point.x && pointSet[i].y == point.y)
				pointSet[i].flag = point.flag;
	}

	void process(edgeInfo edge)
	{
		int x = edge.x, y = edge.y;
		//横边，查看左右两个点
		if (y % 2 == 1)
		{
			//一个点被访问，一个点没被访问的话，打通两点之间的墙
			if (checkPointState(x - 1, y) ^ checkPointState(x + 1, y))
			{
				maze[y][x] = road;//打通墙
				pointInfo point;

				//点没被访问的话
				if (!checkPointState(x - 1, y))
				{
					point.x = x - 1;
					point.y = y;
					point.flag = true;
					
					//将点设为已被访问
					SetPointState(point);
					//将和点相连的边加入边的集合
					checkConnectedEdge(point);
				}

				if (!checkPointState(x + 1, y))
				{
					point.x = x + 1;
					point.y = y;
					point.flag = true;

					SetPointState(point);
					checkConnectedEdge(point);
				}
			}
		}
		//竖边，查看上下两个点
		else
		{
			if (checkPointState(x, y - 1) ^ checkPointState(x, y + 1))
			{
				maze[y][x] = 0;
				pointInfo point;
				if (!checkPointState(x, y - 1))
				{
					point.x = x;
					point.y = y - 1;
					point.flag = true;

					SetPointState(point);
					checkConnectedEdge(point);
				}
				if (!checkPointState(x, y + 1))
				{
					point.x = x;
					point.y = y + 1;
					point.flag = true;

					SetPointState(point);
					checkConnectedEdge(point);
				}
			}
		}
	}

	//生成随机迷宫
	void createMaze()
	{
		checkConnectedEdge(choiceRandomPoint());
		while (!checkAllPoint())
		{
			process(choiceRandomEdge());
		}
	}

	//显示迷宫
	void showMaze(bool flag)
	{
		//设置比例，使得无论宽、高多少的迷宫都能正常显示
		double size = Width * 0.75 / max(width, height);
		
		//迷宫小方块左下角坐标
		double x = (Width - width * size) / 2;
		double y = Height - (Height - height * size) / 2 - size;

		for (int i = 0; i < height; i++, y -= size)
		{
			x = (Width - width * size) / 2;
			for (int j = 0; j < width; j++, x += size)
			{
				switch (maze[i][j])
				{
				case road://路
					break;
				case wall://墙
					glColor3d(0.9, 0.9, 0.9);
					glRectd(x, y, x + size, y + size);
					break;
				case start://起点
					glColor3d(1, 0, 0);
					glRectd(x, y, x + size, y + size);
					break;
				case end://终点
					glColor3d(0, 1, 0);
					glRectd(x, y, x + size, y + size);
					break;
				case path://DFS寻找的路径
					if (flag)
					{
						glColor3d(0.6, 0.2, 0.8);
						glRectd(x, y, x + size, y + size);
					}
					break;
				default:
					break;
				}
			}
		}
	}

	//判断当前的点是否是通路
	bool Accessible(int x, int y)
	{
		if (1 <= x && x <= width - 2 && 1 <= y && y <= height - 2 || y == height - 2 && x == width - 1)
			if (maze[y][x] == road || maze[y][x] == end)
				return true;
		return false;
	}

	//深度优先搜索
	bool DFS(int x, int y)
	{
		pointInfo point;
		point.flag = false;
		point.x = x;
		point.y = y;
		stack<pointInfo> pointStack;
		pointStack.push(point);
		bool flag = true;

		while (!pointStack.empty())
		{
			if (flag)
				point = pointStack.top();
			else
			{
				point = pointStack.top();
				//回溯时将点设为被访问但不是通路
				maze[point.y][point.x] = -1;
				pointStack.pop();

				point = pointStack.top();
			}

			int x = point.x, y = point.y;
			point.flag = true;

			//找到终点
			if (y == height - 2 && x == width - 1)
			{
				maze[y][x] = end;
				return true;
			}

			//查看周围右、下、左、上是否有通路
			if (Accessible(x + 1, y))//若有通路便从新的位置寻找
			{
				maze[y][x + 1] = path;
				point.x = x + 1;
				point.y = y;
				point.flag = true;
				flag = true;

				pointStack.push(point);
				continue;
			}
			if (Accessible(x, y - 1))
			{
				maze[y - 1][x] = path;
				point.x = x;
				point.y = y - 1;
				point.flag = true;
				flag = true;

				pointStack.push(point);
				continue;
			}
			if (Accessible(x - 1, y))
			{
				maze[y][x - 1] = path;
				point.x = x - 1;
				point.y = y;
				point.flag = true;
				flag = true;

				pointStack.push(point);
				continue;
			}
			if (Accessible(x, y + 1))
			{
				maze[y + 1][x] = path;
				point.x = x;
				point.y = y + 1;
				point.flag = true;
				flag = true;

				pointStack.push(point);
				continue;
			}
			flag = false;
		}
		return false;
	}

	~Maze()
	{
		for (int i = 0; i < height; i++)
		{
			delete[]maze[i];
		}
		delete[]maze;
	}
};

void Myinit(void);
void Keyboard(unsigned char key, int x, int y);
void Display(void);
void Reshape(GLsizei w, GLsizei h);

Maze *maze = NULL;
int mazeX = 15, mazeY = 15;
bool show_path;

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	char text1[] = "2D大作业";
	char text2[] = "盛逶";

	char *argv[] = { text1, text2 };
	int argc = 2;

	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(Width, Height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);//双缓存模式
	glutCreateWindow("Maze - 盛逶");

	Myinit();  //初始设置

	glutDisplayFunc(Display);  //注册绘制响应回调函数
	glutKeyboardFunc(Keyboard);  //注册键盘响应回调函数
	glutReshapeFunc(Reshape);    //注册窗口改变回调函数
	glutMainLoop();

	return 0;
}

void Myinit(void)
{
	glClearColor(0, 0, 0, 0);  //背景色
	maze = new Maze(mazeX, mazeY);
	maze->createMaze();
}

//屏幕显示字体函数
void drawString(string str)
{
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall) {
		isFirstCall = 0;
		// 申请MAX_CHAR个连续的显示列表编号
		lists = glGenLists(MAX_CHAR);
		// 把每个字符的绘制命令都装到对应的显示列表中
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	// 调用每个字符对应的显示列表，绘制每个字符
	for (int i = 0; i < str.length(); i++) {
		glCallList(lists + str[i]);
	}
}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	//显示迷宫的宽和高
	glColor3f(1, 1, 1);	 
	glRasterPos2i(30, Height - 30);
	drawString("width:   " + to_string(2 * mazeX - 1));
	glRasterPos2i(30, Height - 50);
	drawString("height: " + to_string(2 * mazeY - 1));

	//显示用户指导
	glColor3f(0, 1, 0);
	glRasterPos2i(30, 60);
	drawString("User instruction:");
	glRasterPos2i(30, 40);
	drawString("w: height+=2   s: height-=2   d: width+=2   a: width-=2");
	glRasterPos2i(30, 20);
	drawString("f: refresh   p: find path (press again to hide the path)");

	maze->showMaze(show_path);
	glutSwapBuffers();
}

int num = 0;
//键盘互动
void Keyboard(unsigned char key, int x, int y)
{
	bool flag = false;

	switch (key)
	{
	case 'w':
		mazeY += 1;
		flag = true;
		break;
	case 's':
		if (mazeY >= 2)
			mazeY -= 1;
		flag = true;
		break;
	case 'd':
		mazeX += 1;
		flag = true;
		break;
	case 'a':
		if (mazeX >= 2)
			mazeX -= 1;
		flag = true;
		break;
	case 'f':
		flag = true;
		break;
	case 'p':
		num++;
		if (num == 1)
			maze->DFS(1, 0);
		//按一次显示路线，再按一次隐藏路线
		if (num % 2 == 1)
			show_path = true;
		else
			show_path = false;
		goto end;
		break;
	default:
		break;
	}

	if (flag)
	{
		num = 0;
		delete maze;
		maze = new Maze(mazeX, mazeY);
		maze->createMaze();
	}
	
end:
	glutPostRedisplay();
}

void Reshape(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);  //投影矩阵模式
	glLoadIdentity();  //矩阵堆栈清空
	gluOrtho2D(0, Width, 0, Height);  //设置裁剪窗口大小
	glViewport(0, 0, w, h); //设置视区大小
	glMatrixMode(GL_MODELVIEW);  //模型矩阵模式 
}

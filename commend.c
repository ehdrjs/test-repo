#include<ncurses.h>
#include<ctype.h>
#include<stdlib.h>
#include"main.h"
#include"commend.h"
#include"edit.h"

void isPrintI(WINDOW *editWin,WINDOW *comWin,int *prev,int ch,int *flag) // insertmode변경 
{
	if(*flag == MODE_INS)
		insert(editWin,comWin,prev,ch,flag);
	else
		*flag = MODE_INS;
}

void commendDisplay(WINDOW *editWin,WINDOW *comWin) // 모든 입력 명령 세이브 추렭담당
{
	int editY,editX,comY,comX;
	int maxy,maxx;
	getyx(editWin,editY,editX);
	getyx(comWin,comY,comX);
	getmaxyx(comWin,maxy,maxx);
	wmove(comWin,0,0);
	wclrtobot(comWin);
	wrefresh(comWin);
	mvwprintw(comWin,0,maxx-20,"(%d,%d)",(editX + 1),(editY + 1));
	wrefresh(comWin);
	wmove(editWin,editY,editX);
}

void delX(WINDOW *editWin,int *flag,int *buf) // 삭제 글자하나
{
	if(*flag == MODE_COM)
	{
		*buf = winch(editWin);
		if(lineIsEmpty(editWin))
			wdeleteln(editWin);
		else
			wdelch(editWin);
	}
}

void delWord(WINDOW *editWin,int *prev,int *flag,int *buf) // 삭제 단어
{
	int i = 0;
	int y,x;
	getyx(editWin,y,x);
	if(*flag == MODE_COM)
	{
		if(*prev == 'd')
		{
			if(lineIsEmpty(editWin))
				wdeleteln(editWin);
			else
			{
				while(!isspace(winch(editWin)) && x > 0)
					wmove(editWin,y,--x);
				if(x > 0)
				wmove(editWin,y,++x);
				while(!isspace(winch(editWin)))
				{
					buf[i++] = winch(editWin); 
					wdelch(editWin);
				}
				buf[i] = '\0';
				if(isspace(winch(editWin)))
					wdelch(editWin);
			}
			*prev = EOF;
		}
	}
}

void delLine(WINDOW *editWin,int *prev,int *flag,int *buf) //삭제 라인
{
	int y,x,i = 0;
	int ch;
	if(*flag == MODE_COM && *prev == 'd')
	{
		getyx(editWin,y,x);
		wmove(editWin,y,0);
		while(i < COLS - 1)
		{ 
			buf[i] = winch(editWin);
			wmove(editWin,y,++i);
		}
		buf[i] = winch(editWin);
		buf[i + 1] = '\0';
		wdeleteln(editWin);
		wmove(editWin,y,0);
		*prev = EOF;
	}
}

void copyWord(WINDOW *editWin,int *prev,int *flag,int buf[]) // 복사 단어
{
	int count = 0;
	int tempCh,tempX;
	int y,x;
	getyx(editWin,y,x);
	tempX = x;
	if(*flag == MODE_COM && *prev == 'c')
	{
		while(!isspace(tempCh = winch(editWin)))
		{
			buf[count++] = tempCh;
			wmove(editWin,y,++tempX);
		}
		buf[count] = '\0';
		wmove(editWin,y,x);
		*prev = EOF;
	}
}

void copyLine(WINDOW *editWin,int *prev,int *flag,int buf[]) // 복사 라인
{
	int count = 0,i = 0;
	int y,x,tempX,temp = COLS;
	getyx(editWin,y,x);
	tempX = x;
	if(*flag == MODE_COM && *prev == 'c')
	{
		tempX = 0;
		wmove(editWin,y,tempX);
		while(tempX < COLS)
		{
			buf[count++] = winch(editWin);
			wmove(editWin,y,++tempX);
		}
		buf[count] = '\0';
		wmove(editWin,y,x);
		*prev = EOF;
	}
}

void pasteWord(WINDOW *editWin,int *prev,int *flag,int buf[]) // 붙여넣기 단어
{
	int i = 0;
	int prevY,prevX;
	getyx(editWin,prevY,prevX);
	if(*flag == MODE_COM && *prev == 'p')
	{
		while(buf[i] != '\0')
		{
			if(isspace(winch(editWin)))
			{
				waddch(editWin,buf[i]);
				wmove(editWin,prevY,++prevX);
			}else{
				winsch(editWin,buf[i]);
				wmove(editWin,prevY,++prevX);
			}
			i++;
		}
		*prev = EOF;
	}
}

void pasteLine(WINDOW *editWin,int *prev,int *flag,int buf[]) //붙여넣기 라인
{
	int i = 0;
	int y,x;
	if(*flag == MODE_COM && *prev == 'p')
	{
		winsertln(editWin);
		getyx(editWin,y,x);
		wmove(editWin,y,0);
		while(buf[i] != '\0')
			waddch(editWin,buf[i++]);
		*prev = EOF;
	}
}

void replaceChar(WINDOW *editWin,int *flag) //글자하나 바꾸기
{
	if(*flag == MODE_COM)
		waddch(editWin,wgetch(editWin));
}

void replaceLine(WINDOW *editWin,int *flag) // 라인 바구기
{
	int ch;
	if(*flag == MODE_COM)
	{
		while((ch = wgetch(editWin)) != CH_ESCAPE)
			waddch(editWin,ch);
	}
}

void backspaceDel(WINDOW *win,int *flag) //입력모드일때 삭제
{
	int ch;
	int count;
	int y,x;	
	int maxy,maxx;
	int buf[LINE_BUF_NUM];
	getmaxyx(win,maxy,maxx);
	getyx(win,y,x);
	if(*flag == MODE_COM)
	{
		if(x > 0)
			wmove(win,y,--x);
		else{
			if(y - 1 >= 0)
			{
				wmove(win,--y,maxx - 1);
				getyx(win,y,x);
				while(isspace(winch(win)) && x > 0 )
					wmove(win,y,--x);		
			}else
				wmove(win,0,0);
		
		}
	}else if(*flag == MODE_INS)
	{
		if( x > 0)
		{
			wmove(win,y,x - 1);
			wdelch(win);
			wmove(win,y,--x);
		}else{
			if(y - 1 >= 0)
			{
				wmove(win,y,maxx - 1);
				getyx(win,y,x);
				while(isspace(winch(win)) && x > 0)
					wmove(win,y,--x);
				if(!isspace(winch(win)))
				{
					count = x;
					while(x >= 0)
					{
						buf[x] = winch(win);
						wmove(win,y,--x);
					}
					buf[count + 1] = '\0';
				}

				wmove(win,--y,maxx - 1);
				getyx(win,y,x);
				while(isspace(winch(win)) && x > 0)
					wmove(win,y,--x);
				if(!isspace(winch(win)))
				wmove(win,y,++x);
				if(count > 0 && count < maxx - x)
				{ 
					int i = 0;
					while(buf[i] != '\0')
					{
						waddch(win,buf[i]);
						i++;
					}
					getyx(win,y,x);
					wmove(win,y + 1,0);
					wdeleteln(win);
					wmove(win,y,x - count - 1);
				}

			}else
				wmove(win,0,0);
		}
	}else{
		wdelch(win);
		if(x > 0)
			wmove(win,y,--x);
		else
			*flag = MODE_COM;
	}
}

void enterKey(WINDOW *win,int *flag) // 엔터키
{
	int buf[LINE_BUF_NUM];
	int y,x;
	getyx(win,y,x);
	if(*flag == MODE_COM)
		wmove(win,++y,0);
	else if(*flag == MODE_INS) 
	{
		int i = 0,j = 0,count,maxy,maxx;
		getmaxyx(win,maxy,maxx);
		count = maxx - x;
		while(count > 0)
		{
			buf[i++] = winch(win);
			wdelch(win);
			count--;
		}
		buf[i] = '\0';
		wmove(win,++y,0);
		winsertln(win);
		while(buf[j] != '\0')
		{
			waddch(win,buf[j]);
			j++;
		}
		wmove(win,y,0);
	}
	
}

void tabKey(WINDOW *editWin,int *flag) //템키
{
	int i;
	int y,x;
	getyx(editWin,y,x);
	if(*flag == MODE_INS)
		for(i = 0;i < 4;i++)
		{ 
			winsch(editWin,' ');
			wmove(editWin,y,++x);
		}
}
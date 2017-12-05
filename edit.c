#include<ncurses.h>
#include<ctype.h>
#include<stdlib.h>
#include"main.h"
#include"edit.h"

void insert(WINDOW *editWin,WINDOW *comWin,int *prev,int ch,int *flag) 
{

	int y,x;
	getyx(editWin,y,x); // 현재 커서위치 저장

	if(*flag == MODE_INS && isprint(ch)) { // 입력모드, 출력가능문자 검사
		winsch(editWin,ch); //editWin 윈도우에 ch삽입
		wmove(editWin,y,++x); // 커서위치 변경
	}

	if(*flag == MODE_COM) { // 명령모드
		if(isdigit(ch) && isdigit(*prev)) {
			*prev =	(*prev - '0') * 10 + ( ch - '0');
			mvprintw(20,30,"%d",*prev);
		}else{
			*prev = ch;
		}
	}

	if(ch == CH_ESCAPE && *flag == MODE_INS) { // 명령모드 변경 ESC눌렀을때 
		*flag = MODE_COM;
	}
}

int lineIsEmpty(WINDOW *editWin) //커서줄이 비어있는지 확인, 빈행을 삭제
{
	int y,x,i = 0;
	getyx(editWin,y,x); // 현재 커서위치 저장

	if(x == 0) { //커서줄이 비어있을 경우 
		while(i < COLS && isspace(winch(editWin)))
			wmove(editWin,y,++i);
		if(i >= COLS) {
			wmove(editWin,y,x);
			return 1;
		}
	}
	wmove(editWin,y,x);

	return 0;
}
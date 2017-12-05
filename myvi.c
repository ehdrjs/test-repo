#include<stdlib.h>
#include<ncurses.h>
#include<ctype.h>
#include"main.h"
#include"edit.h"
#include"commend.h"
#include"save.h"

int main(int argc,char *argv[])
{
	if(argc < 2) {
		fprintf(stderr,"파일 이름을 입력하시오.\n");
		exit(1);
	}else {
		FILE *fp;
		fp = fopen(argv[1],"r+");

		if(fp == NULL) // 파일이 없던 경우
		{
			char create;
			fprintf(stdout,"파일을 생성하시겠습니까?（Y/N");
			fscanf(stdin,"%c",&create);

			if(create == 'Y' || create == 'y') {
				fp = fopen(argv[1],"w");
				fclose(fp);
			}

			if(fp == NULL) {
				fprintf(stderr,"fopen error\n");
				exit(1);
			}else {
				fprintf(stdout,"fopen success\n");
			}
		}else {
			int y,x,maxy,maxx;
			int ch,prev = EOF;
			int quit = false;
			int charBuf;
			int lastLine;
			WINDOW *editWin,*comWin; // 명령모드 입력모드 윈도우 생성
			int flag = MODE_COM;
			int wordBuf[WORD_BUF_NUM],lineBuf[LINE_BUF_NUM];

			initscr(); // 커서모드 초기화
			//start_color(); 색상모드
			cbreak(); // cbreak모드 컨트롤 문자를 다른 문자로 변경해줌
			noecho(); // 터미널에 입력한 문자를 제어

			editWin = derwin(stdscr,LINES- 1,COLS,0,0);
			comWin = derwin(stdscr,1,COLS,LINES - 1,0);

			scrollok(editWin,TRUE); // 스크롤 사용가능
			refresh(); // 화면 갱신

			keypad(stdscr,TRUE); // 방향키 사용가능
			keypad(editWin,TRUE); // 방향키 사용가능 
			keypad(comWin,TRUE); // 방향키 사용가능

			while((ch = getc(fp)) != EOF) {
				waddch(editWin,ch);
			}

			getmaxyx(editWin,maxy,maxx); // 행렬 최댓값 리턴
			lastLine = getLastLine(editWin,maxy,maxx);
			wmove(editWin,0,0);	
			commendDisplay(editWin,comWin);

			while((ch = wgetch(editWin)) != EOF && quit == false)
			{
				switch(ch)
				{
					case KEY_UP :
						getyx(editWin,y,x);
						wmove(editWin,--y,x);
						commendDisplay(editWin,comWin);
						break;
					case KEY_DOWN : 
						getyx(editWin,y,x);
						wmove(editWin,++y,x);
						commendDisplay(editWin,comWin);
						break;
					case KEY_RIGHT : 
						getyx(editWin,y,x);
						wmove(editWin,y,++x);
						commendDisplay(editWin,comWin);
						break;
					case KEY_LEFT : 
						getyx(editWin,y,x);
						wmove(editWin,y,--x);
						commendDisplay(editWin,comWin);
						break;
					case CH_ESCAPE :
						flag = MODE_COM;
						break;
					case CH_EDIT :
						quit = isPrint(editWin,comWin,fp,&prev,ch,&flag,lastLine);
						insert(editWin,comWin,&prev,ch,&flag);
						break; 
					case 'i' :
						isPrintI(editWin,comWin,&prev,ch,&flag);
						commendDisplay(editWin,comWin);
						break;
					case 'x' :
						delX(editWin,&flag,&charBuf);
						insert(editWin,comWin,&prev,ch,&flag);
						commendDisplay(editWin,comWin);
						break;
					case 'w' : 
						delWord(editWin,&prev,&flag,wordBuf);
						copyWord(editWin,&prev,&flag,wordBuf);
						pasteWord(editWin,&prev,&flag,wordBuf);
						insert(editWin,comWin,&prev,ch,&flag);
						commendDisplay(editWin,comWin);
						break;
					case 'l' : 
						delLine(editWin,&prev,&flag,lineBuf);
						copyLine(editWin,&prev,&flag,lineBuf);
						pasteLine(editWin,&prev,&flag,lineBuf);
						insert(editWin,comWin,&prev,ch,&flag);
						commendDisplay(editWin,comWin);
						break;
					case 'r' : 
						replaceChar(editWin,&flag);
						insert(editWin,comWin,&prev,ch,&flag);
						commendDisplay(editWin,comWin);
						break;
					case 'R' : 
						replaceLine(editWin,&flag);
						insert(editWin,comWin,&prev,ch,&flag);
						commendDisplay(editWin,comWin);
						break;
					case CH_ENTER : 
						enterKey(editWin,&flag);
						commendDisplay(editWin,comWin);
						break;
					case KEY_BACKSPACE : 
						backspaceDel(editWin,&flag);
						commendDisplay(editWin,comWin);
						break;
					case CH_TAB : 
						tabKey(editWin,&flag);
						commendDisplay(editWin,comWin);
						break;
					default : 
						insert(editWin,comWin,&prev,ch,&flag);
						commendDisplay(editWin,comWin);
						break; 
				}
				
			}
			endwin();
		}
	}
	return 0;
}

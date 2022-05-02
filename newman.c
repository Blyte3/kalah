#include <stdio.h>

#define HOLECOUNT 12
#define MARB_PER_HOLE 4

typedef struct{
	
	unsigned char board[HOLECOUNT];
	unsigned short p1points;
	unsigned short p2points;
	
}gameboard;

typedef struct{
	
	int pathlen;
	int eval;
	char path[HOLECOUNT*MARB_PER_HOLE];
	gameboard board;
	
}branchinfo;

void printboard(gameboard board){
	
	printf("\n");
	
	int r;
	
	if(board.p2points<10) printf(" ");
	printf("%d ",board.p2points);
	
	for(r=HOLECOUNT-1;r>(HOLECOUNT/2-1);r--){
		
		if(board.board[r]<10) printf(" ");
		printf("%d ",board.board[r]);
	}
	
	printf("\n   ");
	
	for(r=0;r<HOLECOUNT/2;r++){
		
		if(board.board[r]<10) printf(" ");
		printf("%d ",board.board[r]);
	}
	
	if(board.p1points<10) printf(" ");
	printf("%d ",board.p1points);
}

void gameend(branchinfo* board){
	
	int r;
	
	for(r=0;r<(HOLECOUNT/2);r++){
		
		board->board.p1points+=board->board.board[r];
		board->board.p2points+=board->board.board[r+(HOLECOUNT/2)];
		board->board.board[r]=0;
		board->board.board[r+(HOLECOUNT/2)]=0;
	}
}

void gameend_main(gameboard* board){
	
	int r;
	
	for(r=0;r<(HOLECOUNT/2);r++){
		
		board->p1points+=board->board[r];
		board->p2points+=board->board[r+(HOLECOUNT/2)];
		board->board[r]=0;
		board->board[r+(HOLECOUNT/2)]=0;
	}
}

branchinfo findmax(branchinfo* list,int itemcount){
	
	branchinfo highest;
	int r;
	
	highest=list[0];
	
	for(r=1;r<itemcount;r++){
		
		if(list[r].eval>highest.eval) highest=list[r];
	}
	
	return highest;
}

branchinfo findmin(branchinfo* list,int itemcount){
	
	branchinfo lowest;
	int r;
	
	lowest=list[0];
	
	for(r=1;r<itemcount;r++){
		
		if(list[r].eval<lowest.eval) lowest=list[r];
	}
	
	return lowest;
}

int makemove(gameboard *board,int space,int player){
	
	int count;
	int points;
	int endonpoint;
	
	//points=0;
	count=board->board[space];
	board->board[space]=0;
	
	if(player==1){
		
		points=(count+space+(HOLECOUNT/2+1))/(HOLECOUNT+1);
		endonpoint=(count+space+(HOLECOUNT/2+1))%(HOLECOUNT+1);
	}
	else{
		
		points=(count+space+1)/(HOLECOUNT+1);
		endonpoint=(count+space+1)%(HOLECOUNT+1);
	}
	
	if(player==1) board->p1points+=points;
	else board->p2points+=points;
	
	count-=points;
	
	int r;
	
	for(r=0;r<count;r++){
		
		space++;
		
		if(space==HOLECOUNT) space=0;
		
		board->board[space]++;
	}
	
	if(endonpoint==0) return 1;

	if((board->board[space]==1) && (board->board[(HOLECOUNT-1)-space]!=0)){
		
		if((player==1) && (space<(HOLECOUNT/2))){
			
			board->p1points+=board->board[space]+board->board[(HOLECOUNT-1)-space];
			board->board[space]=0;
			board->board[(HOLECOUNT-1)-space]=0;
		}
		else if((player==-1) && (space>(HOLECOUNT/2)-1)){
		
			board->p2points+=board->board[space]+board->board[(HOLECOUNT-1)-space];
			board->board[space]=0;
			board->board[(HOLECOUNT-1)-space]=0;
		}
	}
	
	return 0;
}

//void cleantable(tableentry

branchinfo branch(branchinfo board,int alpha,int beta,int player,int depth,int pathlen){
		
	if(depth==0){
		
		board.eval=board.board.p1points-board.board.p2points;
		board.pathlen=pathlen;
		return board;
	}
	
	int index;
	char moves[HOLECOUNT/2];
	int movecount;
	
	int r;
	movecount=0;
	
	if(player==1) index=0;
	else index=HOLECOUNT/2;
	
	for(r=index;r<index+(HOLECOUNT/2);r++){
		
		if(board.board.board[r]!=0){
			
			moves[movecount]=r;
			movecount++;
		}
	}
	
	if(movecount==0){
		
		gameend(&board);
		board.eval=board.board.p1points-board.board.p2points;
		board.pathlen=pathlen;
		return board;
	}
	
	int resultcount;
	int eval;
	int tempplayer;
	branchinfo results[HOLECOUNT/2];
	resultcount=0;
	
	branchinfo temp;
	
	if(player==1){
		
		for(r=0;r<movecount;r++){
			
			temp=board;
			
			if(!makemove(&temp.board,moves[r],player)) tempplayer=~player+1;
			
			temp.path[pathlen]=moves[r];
			
			results[r]=branch(temp,alpha,beta,tempplayer,depth-1,pathlen+1);
			
			resultcount++;
			
			if(results[r].eval>alpha) alpha=results[r].eval;
			if(alpha>=beta) break;
		}
		
		return findmax(results,resultcount);
	}
	else{
		
		for(r=0;r<movecount;r++){
			
			temp=board;
			
			if(!makemove(&temp.board,moves[r],player)) tempplayer=~player+1;
			
			temp.path[pathlen]=moves[r];
			
			results[r]=branch(temp,alpha,beta,tempplayer,depth-1,pathlen+1);
			
			resultcount++;
			
			if(results[r].eval<beta) beta=results[r].eval;
			if(alpha>=beta) break;
		}
		
		return findmin(results,resultcount);
	}
}

int aimove(gameboard board,int player){
	
	branchinfo result;
	
	result.board=board;
	
	result=branch(result,-100000,100000,player,13,0);
	
	return result.path[0];
}

void main(){
	
	int player;
	gameboard board;
	int input;
	int repeat;
	int index;
	int r;
	
	for(r=0;r<HOLECOUNT;r++) board.board[r]=MARB_PER_HOLE;
	
	board.p1points=0;
	board.p2points=0;
	
	int array[12]={0,0,0,0,1,0,0,3,4,13,11,9};
	
	board.p1points=6;
	board.p2points=1;
	
	for(r=0;r<12;r++){
		
		board.board[r]=array[r];
	}
	
	player=1;
	
	printboard(board);
	
	while(1){
		
		if(player==1){
			
			scanf("%d",&input);
			repeat=makemove(&board,input,player);
			//repeat=makemove(&board,aimove(board,player),player);
		}
		else{
			
			repeat=makemove(&board,aimove(board,player),player);
		}
		
		if(player==1) index=0;
		else index=HOLECOUNT/2;
		
		for(r=0;r<(HOLECOUNT/2);r++) if(board.board[r+index]!=0) break;
		if(r==HOLECOUNT/2){
			
			gameend_main(&board);
			break;
		}
		
		if(repeat==0) player=(~player)+1;
		
		if(player==1) printf("\nplayer 1 to move\n");
		else printf("\nplayer 2 to move\n");
		
		printboard(board);
	}
	
	printboard(board);
	
	if(board.p1points==board.p2points) printf("\ndraw\n");
	if(board.p1points>board.p2points) printf("\nplayer 1 wins\n");
	if(board.p2points>board.p1points) printf("\nplayer 2 wins\n");
}

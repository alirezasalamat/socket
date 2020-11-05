#include <stdio.h> 
#include <stdlib.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <signal.h>

int checker(char input[],char check[]);
int end(int state[5][5], int n);
int tekrar(int rasta, int x, int y, int state[5][5]);
int check_square(int rasta, int x, int y, int state[5][5], int emtiaz[4][4], int turn);
int get_command(int rasta, int x, int y, int state[5][5], int n);
int player_turn(int state[5][5], int emtiaz[4][4], char* buff, int turn, int n);
void show_map(int state[5][5], int emtiaz[4][4], int n);
int game(char* num, char* port, int n);
char* win(int count[4], int n);
void handler(int signum);
int change_turn(int turn, int n);
void show_turn(int turn);
void show_end_msg(int count[4], int n, int state[5][5], int emtiaz[4][4]);
int checker(char input[],char check[]);


int main(int argc, char const *argv[]) 
{   
    siginterrupt(SIGALRM, 1);
    int sock, valread, sendval; 
    struct sockaddr_in serv_addr; 
    char buffer[1024] = {0}; 
    char buffer2[1024] = {0};  
   
    //tcp
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
    {  
        write(1, "socket failed\n", 14);
        exit(EXIT_FAILURE);
    } 

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(atoi(argv[1])); 
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        write(1, "connect failed\n", 16);
        exit(EXIT_FAILURE); 
    } 
    bzero(buffer, sizeof(buffer));
    if(valread = recv(sock, buffer, 1024 , 0) < 0)
    {
        write(1, "error read\n", 12);
    }
    write(1, buffer, 1024);


    char* q1 = "please enter from 2 or 3 or 4 player game:\n";
    write(1, q1, strlen(q1));

    bzero(buffer2, sizeof(buffer2));
    if((valread = read(0, buffer2, 1024)) <= 0)
    {
        write(1, "error read\n",4);
    }
    
    sendval = send(sock, buffer2, strlen(buffer2) - 1 , 0);
    char num[10];
   
    strcpy(num, buffer2);
   
    if(sendval  < 0)
    {
        write(1, "send error\n" , 8);
    }
    
    bzero(buffer, sizeof(buffer));
    if(valread = recv(sock, buffer, 21, 0) < 0 )
    {
            write(1, "recv erroe\n", 10);
    }
    if(write(1, buffer, 21) < 0)
    {
            write(1,"error write\n", 11);
    }
    
    bzero(buffer, sizeof(buffer));
    char port[10];
    bzero(port, 10);
    if(valread = recv(sock, port, 4, 0) < 0 )
    {
            write(1, "recv error\n", 11);
    }
    
    if(valread = recv(sock, buffer, 1, 0) < 0 )
    {
            write(1, "recv error\n", 11);
    }

    game(buffer, port, atoi(num));

}


struct sockaddr_in my_addr; 
int game(char* num, char* port, int n)
{
    int a;
    int len = sizeof(struct sockaddr_in);

    if ((a = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    { 
        write(1, "connect failed\n", 16);
        exit(EXIT_FAILURE); 
    } 
    int broadcast = 1;
    if(setsockopt(a,SOL_SOCKET,SO_REUSEPORT,&broadcast,sizeof(broadcast)) < 0)
    {
        write(1, "setsockopt error\n", 18); 
		exit(EXIT_FAILURE);
    }

    if(setsockopt(a,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast)) < 0)
    {
        write(1, "setsockopt error\n", 18); 
		exit(EXIT_FAILURE);
    }
     
    my_addr.sin_family = AF_INET;        
    my_addr.sin_port = htons(atoi(port));   
    my_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    if(bind(a,(struct sockaddr*)&my_addr, sizeof (my_addr)) < 0)
    {
        write(1, "error binding\n", 14);
        close(a);
		exit(EXIT_FAILURE);
    }

    char buff[1024];
    int count[4] = {0} ,flag = 0;
	int state[5][5] = {0}, x, y, rasta, emtiaz[4][4];
    int turn = 0;
    int id = atoi(num);
    int valread, valsend, valrecv;
    int time = 0;
    memset(emtiaz, -1, sizeof(emtiaz));

    while(1)
    {
        show_map(state, emtiaz, n);
        show_turn(turn);
        if(turn == id  && time != 1)
        {   
            bzero(buff, 1024);
            strcpy(buff, "0 0 0");
            signal(SIGALRM, handler);
            alarm(20);
            if((valread = read(0, buff, 1024)) < 0)
            {   
                write(1, "No Input !!\n",13);
            }
            alarm(0); 
            if(sendto(a, buff, 1024, 0, (struct sockaddr *)&my_addr, sizeof(my_addr)) <= 0)
            {
                write(1, "error in send\n", 14);
            }
        }
        bzero(buff, 1024);
        if( time != 1)
        {
            if(recv(a, buff , 1024, 0) < 0)
            {
                write(1, "recv error\n", 11);
            }
        }
        buff[6] = '\0';
        int flag = player_turn(state, emtiaz, buff, turn, n);
        count[turn] += flag;
        if(end(state, n) == 1)
        {
            show_end_msg(count, n, state, emtiaz);
        }
        turn = change_turn(turn, n);
    }
}

void handler(int signum)
{   
    write(1, "timeout !!\n", 12);
}

void show_end_msg(int count[4], int n, int state[5][5], int emtiaz[4][4])
{
    char* winner = win(count, n);
    show_map(state, emtiaz, n);
    write(1, "Player ", 7);
    write(1, winner, 2);
    write(1, "WON !!!!\n", 9);
    free(winner);
    exit(1);
}

void show_turn(int turn)
{
    write(1,"turn:  ", 8);
    if(turn == 0)
        write(1, "A\n", 2);
    else if(turn == 1)
        write(1, "B\n", 2);
    else if(turn == 2)
        write(1, "C\n", 2);
    else if(turn == 3)
        write(1, "D\n", 2);
}

int change_turn(int turn, int n)
{
    if(n == 2)
    {
        if(turn == 0)
        {
            return 1;
        }
        else if(turn == 1)
        {
            return 0;
        }
    }
    else if(n == 3)
    {
         if(turn == 0)
        {
            return 1;
        }
        else if(turn == 1)
        {
            return 2 ;
        }
        else if(turn == 2)
        {
            return 0;
        }
    }
    else if(n == 4)
    {
         if(turn == 0)
        {
            return 1;
        }
        else if(turn == 1)
        {
            return 2 ;
        }
        else if(turn == 2)
        {
            return 3;
        }
        else if(turn == 3)
        {
            return 0;
        }
    }
}


int player_turn(int state[5][5], int emtiaz[4][4], char* buff, int turn, int n)
{   
    if(strcmp(buff, "") == 0)
    {
        return 0;
    }
    int x, y, rasta;
    int flag = 0;
    rasta = buff[0] - '0';
    x = buff[2] - '0';
    y = buff[4] - '0';
    if(rasta == 0 && x == 0 && y == 0)
    {
        return 0;
    }
    
    x--;
    y--;
    if(get_command(rasta, x, y, state , n) == 0)
    {
        return 0;
    }
	flag = check_square(rasta, x, y, state, emtiaz, turn);                                  
    return flag;
}


int get_command(int rasta, int x, int y, int state[5][5], int n) 
{

	if ((x < 0 || x>n || y < 0 || y > n || rasta > 1 || rasta < 0) || (rasta == 0 && y == n) || (rasta == 1 && x == n)) 
	{
		write(1, "Your number is Wrong! your turn lost!\n\n", 40);    
		return 0;
	}
	else if (tekrar(rasta, x, y, state) == 1)                             
	{
		write(1, "This line exists ! your turn lost!\n\n", 37);
		return 0;
	}

	if (rasta == 0)          
	{
		state[x][y] |= 1;
	}
	else
    {
		state[x][y] |= 2;
    }
	return 1;
}



int check_square(int rasta, int x, int y, int state[5][5], int emtiaz[4][4], int turn)        
{
	int full_square = 0;
	if (state[x][y] == 3 && (state[x + 1][y] == 1 || state[x + 1][y] == 3) && (state[x][y + 1] == 2 || state[x][y + 1] == 3))          
	{                                                                                                                                
		emtiaz[x][y] = turn;
		full_square++;
	}
		if (rasta == 1)                                                                                                                 
			y--; 
		else
			x--;
		if (x >= 0 && y >= 0 && state[x][y] == 3 && (state[x + 1][y] == 1 || state[x + 1][y] == 3) && (state[x][y + 1] == 2 || state[x][y + 1] == 3))
		{
			emtiaz[x][y] = turn;
			full_square++;
		}
	return full_square;
}



int tekrar(int rasta, int x, int y, int state[5][5])            
{
	if (rasta == 0)
	{
		if (state[x][y] == 1 || state[x][y] == 3)             
			return 1;
	}
	else
	{
		if (state[x][y] == 2 || state[x][y] == 3)
		{
			return 1;
		}
	}
	return 0;        
}



int end(int state[5][5], int n)           
{
	int i, j;
	for (i = 0; i < n + 1; i++)
	{
		for (j = 0; j < n + 1; j++)                                
		{
			if (i < n && j < n && state[i][j] != 3)               
				return 0;
			else if (i == n && j < n && state[i][j] != 1)
				return 0;
			else if (j == n && i < n && state[i][j] != 2)
				return 0;
		}
	}
	return 1;
}


void show_map(int state[5][5], int emtiaz[4][4], int n)    
{
	int i, j;
	for (i = 0; i < n + 1; i++)
	{
		for (j = 0; j < n + 1; j++)
		{

			if (state[i][j] == 1 || state[i][j] == 3)   
				write(1, "*--", 3);
			else
				write(1, "*  ", 3);
		}
		write(1,"\n" ,1);
		for (j = 0; j < n + 1; j++)
		{
			if (state[i][j] == 2 || state[i][j] == 3)
				write(1, "|", 2);
			else
				write(1, " ", 1);
			if (i == n || j == n || emtiaz[i][j] == -1)  
			{
				write(1, "  ", 2);                         
			}
			else if (emtiaz[i][j] == 0)
			{
				write(1, "A ", 2);                         
			}
			else if(emtiaz[i][j] == 1)
			{
				write(1, "B ", 2);                          
			}
            else if(emtiaz[i][j] == 2)
			{
				write(1, "C ", 2);                          
			}
            else if(emtiaz[i][j] == 3)
			{
				write(1, "D ", 2);                          
			}
		}
		write(1, "\n", 1);
	}
}

char* win(int count[4], int n)
{
    int i;
    char* winner = (char*)malloc(3 * sizeof(char));
    int max = -1;
    int max_index = -1;
    for(i = 0 ; i < 4 ; i++)
    {
        if(count[i] > max)
        {
            max = count[i];
            max_index = i;
        }
    }
    if(max_index == 0)
    {
        strcpy(winner , "A ");
    }
    else if(max_index == 1)
    {
        strcpy(winner , "B ");
    }
    else if(max_index == 2)
    {
        strcpy(winner , "C ");
    }
    else if(max_index == 3)
    {
        strcpy(winner , "D ");
    }
    return winner;
}

int checker(char input[],char check[])
{
    int i,result=1;
    for(i=0; input[i]!='\0' || check[i]!='\0'; i++) {
        if(input[i] != check[i]) {
            result=0;
            break;
        }
    }
    return result;
}




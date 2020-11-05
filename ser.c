#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> 
	
#define TRUE 1 
#define FALSE 0 


int digit_count(int l);
char* str(int l, int dc);
int checker(char input[],char check[]);
int ckecker_two(char* s);
int ckecker_three(char* s);
int ckecker_four(char* s);
int port_counter();
void msg2(int a, int b);
void msg3(int a, int b, int c);
void msg4(int a, int b, int c, int d);

struct two_player{
    int first;
    int second;
    struct two_player* next;
};

struct three_player{
    int first;
    int second;
    int third;
    struct three_player* next;
};

struct four_player{
    int first;
    int second;
    int third;
    int fourth;
    struct four_player* next;
};



int main(int argc , char *argv[]) 
{ 
	int opt = TRUE; 
	int master_socket , addrlen , new_socket , client_socket[30] , 
		max_clients = 30 , activity, i , valread , sd; 
	int max_sd; 
	struct sockaddr_in address; 
	char buffer[1025]; 

	fd_set readfds; 
	char *message = "Welcome to Dots and Boxes game !! \r\n"; 
	
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
		
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		write(1, "socket failed\n", 14); 
		exit(EXIT_FAILURE); 
	} 

	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
		sizeof(opt)) < 0 ) 
	{ 
		write(1, "setsockopt error\n", 18); 
		exit(EXIT_FAILURE); 
	} 
	
	//char port[] = argv[1];
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(atoi(argv[1])); 
		
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		write(1, "bind failed\n", 13); 
		exit(EXIT_FAILURE); 
	} 

	if (listen(master_socket, 3) < 0) 
	{ 
		write(1, "listen error\n", 14); 
		exit(EXIT_FAILURE); 
	} 
		
	addrlen = sizeof(address); 
	write(1, "Waiting for connections ...\n" ,29); 
    struct two_player* two = (struct two_player*)malloc(sizeof(struct two_player));
    struct two_player* head2 = two;
    head2->first = -1;
    head2->second = -1;
	head2->next = NULL;
	

    struct three_player* three = (struct three_player*)malloc(sizeof(struct three_player));
    struct three_player* head3 = three;
    head3->first = -1;
    head3->second = -1;
    head3->third = -1;
	head3->next = NULL;

    struct four_player* four = (struct four_player*)malloc(sizeof(struct four_player));
    struct four_player* head4 = four;
    head4->first = -1;
    head4->second = -1;
    head4->third = -1;
    head4->fourth = -1;
	head4->next = NULL;

	while(TRUE) 
	{ 
		FD_ZERO(&readfds); 
		FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			sd = client_socket[i]; 
			if(sd > 0) 
				FD_SET( sd , &readfds); 
			if(sd > max_sd) 
				max_sd = sd; 
		} 

		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			write(1, "select error\n", 14); 
		} 

		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			{ 
				write(1, "accept error\n", 14); 
				exit(EXIT_FAILURE); 
			} 

			if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
			{ 
				write(1, "send error\n", 12); 
			} 
				
			for (i = 0; i < max_clients; i++) 
			{  
				if( client_socket[i] == 0 ) 
				{ 
					client_socket[i] = new_socket; 
					write(1, "Player with id ", 16);
					write(1, str(i, digit_count(i)), 2);
					write(1, " joined.\n", 10 );
					break; 
				} 
			} 
		} 

		for (i = 0; i < max_clients; i++) 
		{   
			sd = client_socket[i]; 
			if (FD_ISSET( sd , &readfds)) 
			{ 
				if ((valread = read( sd , buffer, 1024)) == 0) 
				{ 
					getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen); 
					write(1, "Player with id ", 16);
					write(1, str(i, digit_count(i)), 2);
					write(1, " disconnected.\n", 16);
					close(sd); 
					client_socket[i] = 0; 
				} 

				else
				{ 
					buffer[valread] = '\0'; 
                    if(ckecker_two(buffer))
				    {	
						struct two_player* temp = head2;
						while(temp->next != NULL)
						{
							temp = temp->next;
						}
					    if(temp->first == -1)
					    {
						    temp->first = client_socket[i];
					    }
					    else if(temp->first != -1 && temp->second == -1)
					    {
						    temp->second = client_socket[i];
						    msg2(temp->first, temp->second);
                            struct two_player* new = (struct two_player*)malloc(sizeof(struct two_player));
   							new->first = -1;
    						new->second = -1;
							new->next = NULL;
							temp->next = new;
					    }
				    } 

                    else if(ckecker_three(buffer))
                    {
						struct three_player* temp = head3;
						while(temp->next != NULL)
						{
							temp = temp->next;
						}
                        if(temp->first == -1)
					    {
						    temp->first = client_socket[i];
					    }
					    else if(temp->first != -1 && temp->second == -1)
					    {
						    temp->second = client_socket[i];

					    }
                        else if(temp->first != -1 && temp->second != -1 && temp->third == -1)
                        {
                        	temp->third = client_socket[i];
							msg3(temp->first, temp->second, temp->third);
                            struct three_player* new = (struct three_player*)malloc(sizeof(struct three_player));
   							new->first = -1;
    						new->second = -1;
							new->third = -1;
							new->next = NULL;
							temp->next = new;
                        }
                    }
                    else if(ckecker_four(buffer))
                    {
						struct four_player* temp = head4;
						while(temp->next != NULL)
						{
							temp = temp->next;
						}
                        if(head4->first == -1)
					    {
						    head4->first = client_socket[i];
					    }
					    else if(head4->first != -1 && head4->second == -1)
					    {
						    head4->second = client_socket[i];
					    }
                        else if(head4->first != -1 && head4->second != -1 && head4->third == -1)
                        {
                            head4->third = client_socket[i];
                        }
                        else if(head4->first != -1 && head4->second != -1 && head4->third != -1 && head4->fourth == -1)
                        {
                            head4->fourth= client_socket[i];
							msg4(temp->first, temp->second, temp->third, temp->fourth);
                            struct four_player* new = (struct four_player*)malloc(sizeof(struct four_player));
   							new->first = -1;
    						new->second = -1;
							new->third = -1;
							new->fourth = -1;
							new->next = NULL;
							temp->next = new;
                        }
                    }
			    } 
		    }
	}   }
		
	return 0; 
} 



int digit_count(int l) {
    if (l == 0)
        return 1;
    
    int res = 0;
    for (int i = 0; i < 10; i++) {
        if (l == 0)
            break;
        
        l /= 10;
        res++;
    }

    return res;
}

char* str(int l, int dc) {    
    char* l_str = (char*) malloc((dc + 1) * sizeof(char));
    
    for (int i = 0; i < dc; i++) {
        int digit = l % 10;
        l /= 10;
        l_str[dc - i - 1] = '0' + digit;
    }

    return l_str;
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
	
int ckecker_two(char* s)
{   
    return checker(s, "2");
}


int ckecker_three(char* s)
{
    return checker(s, "3");
}


int ckecker_four(char* s)
{
    return checker(s, "4");
}

int port_counter()
{
	static int port = 6000;
	port ++;
	return port - 1;
}
void msg2(int a, int b)
{
    send(a, "you joined the game!\n", 21, 0);
    send(b, "you joined the game!\n", 21, 0);
	int port = port_counter();
	send(a, str(port, digit_count(port)), 4, 0);
	send(b, str(port, digit_count(port)), 4, 0);
	send(a, "0", 1, 0);
	send(b, "1", 1, 0);
}

void msg3(int a, int b, int c)
{
    send(a, "you joined the game!\n", 21, 0);
    send(b, "you joined the game!\n", 21, 0);
	send(c, "you joined the game!\n", 21, 0);
	int port = port_counter();
	send(a, str(port, digit_count(port)), 4, 0);
	send(b, str(port, digit_count(port)), 4, 0);
	send(c, str(port, digit_count(port)), 4, 0);
	send(a, "0", 1, 0);
	send(b, "1", 1, 0);
	send(c, "2", 1, 0);
}

void msg4(int a, int b, int c, int d)
{
    send(a, "you joined the game!\n", 21, 0);
    send(b, "you joined the game!\n", 21, 0);
	send(c, "you joined the game!\n", 21, 0);
	send(d, "you joined the game!\n", 21, 0);
	int port = port_counter();
	send(a, str(port, digit_count(port)), 4, 0);
	send(b, str(port, digit_count(port)), 4, 0);
	send(c, str(port, digit_count(port)), 4, 0);
	send(d, str(port, digit_count(port)), 4, 0);
	send(a, "0", 1, 0);
	send(b, "1", 1, 0);
	send(c, "2", 1, 0);
	send(d, "3", 1, 0);
}





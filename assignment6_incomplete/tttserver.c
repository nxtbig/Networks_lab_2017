#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<sys/sem.h>
#include<signal.h>

#define P(s) semop(s,&pop,1);
#define V(s) semop(s,&vop,1);

void func(int x){
	exit(0);
}

int checkRow(char**arr,int rownum,int board_size){
	int i;
	int flago=0,flagx=0;
	for(i=0;i<board_size;i++){

		if(arr[rownum][i]=='O'&&flagx==0){
			flago=1;
		}
		else if(arr[rownum][i]=='O'&&flagx==1){
			return 0;
		}
		else if(arr[rownum][i]=='X'&&flago==0){
			flagx=1;
		}
		else if(arr[rownum][i]=='X'&&flago==1){
			return 0;
		}
		else if(arr[rownum][i]=='.'){
			return 0;
		}

	}
	return 1;
}


int checkColumn(char**arr,int colnum,int board_size){
	int i;
	int flago=0,flagx=0;
	for(i=0;i<board_size;i++){
		if(arr[i][colnum]=='O'&&flagx==0){
			flago=1;
		}
		else if(arr[i][colnum]=='O'&&flagx==1){
			return 0;
		}
		else if(arr[i][colnum]=='X'&&flago==0){
			flagx=1;
		}
		else if(arr[i][colnum]=='X'&&flago==1){
			return 0;
		}
		else if(arr[i][colnum]=='.'){
			return 0;
		}
	}
	return 1;
}


int checkRightDiagonal(char**arr,int board_size){
	int i;
	int flago=0,flagx=0;
	for(i=0;i<board_size;i++){
		if(arr[i][i]=='O'&&flagx==0){
			flago=1;
		}
		else if(arr[i][i]=='O'&&flagx==1){
			return 0;
		}
		else if(arr[i][i]=='X'&&flago==0){
			flagx=1;
		}
		else if(arr[i][i]=='X'&&flago==1){
			return 0;
		}
		else if(arr[i][i]=='.'){
			return 0;
		}
	}
	return 1;
}


int checkLeftDiagonal(char**arr,int board_size){
	int i;
	int flago=0,flagx=0;
	for(i=0;i<board_size;i++){
		if(arr[i][board_size-1-i]=='O'&&flagx==0){
			flago=1;
		}
		else if(arr[i][board_size-1-i]=='O'&&flagx==1){
			return 0;
		}
		else if(arr[i][board_size-1-i]=='X'&&flago==0){
			flagx=1;
		}
		else if(arr[i][board_size-1-i]=='X'&&flago==1){
			return 0;
		}
		else if(arr[i][board_size-1-i]=='.'){
			return 0;
		}
	}
	return 1;
}


int checkGameOver(char**arr,int board_size){
	int i,j;
	//checking row winner///////////
	for(i=0;i<board_size;i++){
		if(checkRow(arr,i,board_size)==1){
			return 1;
		}
	}
	//checking column winner////////
	for(i=0;i<board_size;i++){
		if(checkColumn(arr,i,board_size)==1){
			return 1;
		}
	}
	//checking diagonal winner////////
	if(checkRightDiagonal(arr,board_size)||checkLeftDiagonal(arr,board_size)){
		return 1;
	}

	return 0;
}

int checkBoardFull(char**arr,int board_size){
	int flag=0;
	int i,j;
  for(i=0;i<board_size;i++){
    for(j=0;j<board_size;j++){
      if(arr[i][j]=='.'){
        flag=1;
        break;
      }
    }
  }
  if(flag==0){
    return 1;
  }
	return 0;
}




int main(void){

	int k,i,status,j;
	//char board[3][3];
	int shmid;
	char *a,*b,*c;
	//////////////initializing semaphores//////////////////////////
	int sigma1,sigma2;
	struct sembuf pop,vop;

//	sigma1=semget(IPC_PRIVATE,1,0777|IPC_CREAT);
//	sigma2=semget(IPC_PRIVATE,1,0777|IPC_CREAT);

	//semctl(sigma1,0,SETVAL,1);
	//semctl(sigma2,0,SETVAL,0);

	pop.sem_num=vop.sem_num=0;
	pop.sem_flg=vop.sem_flg=0;
	pop.sem_op=-1;
	vop.sem_op=1;
///////////////setting up the server socket////////////////////////
	int sockfd,newsockfd,newsockfdd;
	int clilen;
	struct sockaddr_in serv_addr,cli_addr;

	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		printf("Unable to create socket\n");
		exit(0);
	}

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	serv_addr.sin_port=htons(6000);


	if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
		printf("Unable to bind\n");
		exit(0);
	}

	listen(sockfd,2);//at max two clients


///////the server waits for a new game all the time///////////////////

	while(1){

		sigma1=semget(IPC_PRIVATE,1,0777|IPC_CREAT);
		sigma2=semget(IPC_PRIVATE,1,0777|IPC_CREAT);

		semctl(sigma1,0,SETVAL,1);
		semctl(sigma2,0,SETVAL,0);

		printf("\n\n***********************************************************\n\n");
		printf("Enter the board size for new game: ");
		scanf("%d",&k);
		shmid=shmget(IPC_PRIVATE,k*k*sizeof(char),0777|IPC_CREAT);
		c=shmat(shmid,0,0);

/////////////////////initializing the board for a new game ////////////////////
		for(i=0;i<=k*k-1;i++){
			c[i]='.';
		}

////////////////////////printing the new board in the server//////////////
	printf("\n\nThe new board in the server: \n\n");
	for(i=0;i<k;i++){
		for(j=0;j<k;j++){
			printf("%c  ",c[k*i+j]);
		}
	printf("\n\n");
	}
///////////////////////////////////////////////////////////////////////////
//semctl(sigma1,0,SETVAL,1);
//semctl(sigma2,0,SETVAL,0);
///////////////////////////////////////////////////////////////////////////
clilen = sizeof(cli_addr);
//printf("\n\nNow waiting to accept client 1\n\n");
newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
&clilen) ;
if (newsockfd < 0) {
		printf("Accept error\n");
		exit(0);
	}
/////if connection established////////////////
	pid_t pid1=fork();
	if(pid1==0){
		//in process S1
		a=shmat(shmid,0,0);
		signal(SIGKILL,func);
		while(1){

			P(sigma1);
			sleep(4);
			printf("\n\n******************************************************\n\n");


		//	printf("\n\nInside process S1\n\n");

			///////////////////////CONSTRUCT THE SERVERMSG ////////////////////////
			char servermsg[1000];
		/*	for(i=0; i < 1000; i++) {
				servermsg[i] = '\0';
			}*/
			for(i=0;i<k*k;i++){
				servermsg[i]=a[i];
			}
			servermsg[i]='\0';

			//printf("\nprinting the msg which server sends\n");
			//printf("%s\n",servermsg);

			send(newsockfd,servermsg,strlen(servermsg)+1,0);
			/////////////////////CHECK WHETHER GAME OVER OR NOT//////////////////////////


			char**temp=(char**)malloc(k*sizeof(char*));
			for(i=0;i<k;i++){
				temp[i]=(char*)malloc(k*sizeof(char));
			}

			for(i=0;i<k;i++){
				for(j=0;j<k;j++){
					temp[i][j]=a[k*i+j];
				}
			}
			///////////////////IF GAME OVER then TERMINATE this process/////////////////

			if(checkGameOver(temp,k)){
				close(newsockfd);
				printf("\n\n**************CLIENT 2 WINS!!!!*********************\n\n");
				shmdt(a);
				V(sigma2);
				exit(0);
			}
			else if(checkBoardFull(temp,k)){
				close(newsockfd);
				printf("\n\n**************THE GAME ENDS IN A DRAW!!!*************\n\n");
				shmdt(a);
				V(sigma2);
				exit(0);
			}
			////////freeing the temporary storage/////////////
			for(i=0;i<k;i++){
				free(temp[i]);
			}
			free(temp);
			//////////receiving C1's move//////////////////////////
			char buff[1000];
			for(i=0; i < 1000; i++) {
				buff[i] = '\0';
			}
			recv(newsockfd, buff, 1000, 0);
			for(i=0;i<k*k;i++){
				if(buff[i]=='q'){
					a[i]='X';
					continue;
				}
				a[i]=buff[i];
			}

			printf("\n\nThe current board in the server: \n");
			for(i=0;i<k;i++){
				for(j=0;j<k;j++){
					printf("%c  ",a[k*i+j]);
				}
			printf("\n\n");
			}

			V(sigma2);

		}//end of while(1) in S1

	}//end of S1

	else{
		//in process S
		clilen = sizeof(cli_addr);
		//printf("\n\nNow waiting to accept client 2\n\n");
		newsockfdd = accept(sockfd, (struct sockaddr *) &cli_addr,
		&clilen) ;
		if (newsockfdd < 0) {
				printf("Accept error\n");
				exit(0);
			}
			/////if connection established//////////////
			pid_t pid2=fork();
			if(pid2==0){
				//in process S2

				b=shmat(shmid,0,0);
				signal(SIGKILL,func);
				while(1){

					P(sigma2);
					sleep(4);

					printf("\n\n****************************************************\n\n");

				//	printf("\n\nInside process S2\n\n");

					/////////////////////CONSTRUCT THE SERVERMSG ////////////////////
					char servermsg[1000];
					for(i=0;i<k*k;i++){
						servermsg[i]=b[i];
					}
					servermsg[i]='\0';

					for(i=0;i<k;i++){
						servermsg[i]=b[i];
					}
					send(newsockfdd,servermsg,strlen(servermsg)+1,0);
					////////////////////CHECK WHETHER GAME OVER OR NOT////////////////////


					char**temp=(char**)malloc(k*sizeof(char*));
					for(i=0;i<k;i++){
						temp[i]=(char*)malloc(k*sizeof(char));
					}

					for(i=0;i<k;i++){
						for(j=0;j<k;j++){
							temp[i][j]=b[k*i+j];
						}
					}
					///////////////////IF GAME OVER then TERMINATE this process//////////

					if(checkGameOver(temp,k)){
						close(newsockfdd);
						printf("\n\n****************CLIENT 1 WINS!!!!****************\n\n");
						shmdt(b);
						V(sigma1);
						exit(0);
					}
					else if(checkBoardFull(temp,k)){
						close(newsockfd);
						printf("\n\n**************THE GAME ENDS IN A DRAW!!!!*********\n\n");
						shmdt(a);
						V(sigma2);
						exit(0);
					}
					////////freeing the temporary storage/////////////
					for(i=0;i<k;i++){
						free(temp[i]);
					}
					free(temp);
					////////////////////////////////////////////////
					char buff[1000];
					for(i=0; i < 1000; i++) {
						buff[i] = '\0';
					}
					recv(newsockfdd, buff, 1000, 0);
					for(i=0;i<k*k;i++){
						if(buff[i]=='q'){
							b[i]='O';
							continue;
						}
						b[i]=buff[i];
					}

					printf("\n\nThe current board in the server: \n");
					for(i=0;i<k;i++){
						for(j=0;j<k;j++){
							printf("%c  ",b[k*i+j]);
						}
					printf("\n\n");
					}


					V(sigma1);

				}//end of while(1) in S2


			}//end of S2

		/////wait for both child processes to end////////////

		pid_t temp=wait(&status);
		if(temp==pid1){
			kill(pid2,SIGKILL);
		}
		else{
			kill(pid1,SIGKILL);
		}
		//wait(&status);

		close(newsockfd);
		shmctl(shmid, IPC_RMID, 0);
		semctl(sigma1,0,IPC_RMID,0);
		semctl(sigma2,0,IPC_RMID,0);

	}//end of process S

}//end of while(1)

}//end of main


/*******
	Carlos Augusto Lana de Mello
	Yuri Carvalho Pains
	Sistemas de Informação

	Executar o make na pasta onde se encontram os arquivos
		Chamar o servidor, na forma ./servidorFTP porta tam_buffer
		Chamar o cliente, na forma ./clienteFTP host_do_servidor porta_servidor nome_arquivo tam_buffer tam_janela
*******/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>
#include "tp_socket.h"


void error(const char *msg)
{//Exibe erro e aborta o programa
    perror(msg);
    exit(1);
}

char *geraId(int a){
  int comprimento = floor(log10(abs(a))) + 1;
  char *aux = malloc(7);
  switch(comprimento)
  {
    case 1:
      sprintf(aux, "00000%d", a);
      return aux;
      break;
    case 2:
      sprintf(aux, "0000%d", a);
      return aux;
      break;
    case 3:
      sprintf(aux, "000%d", a);
      return aux;
      break;
    case 4:
      sprintf(aux, "00%d", a);
      return aux;
      break;
    case 5:
      sprintf(aux, "0%d", a);
      return aux;
      break;
  }
  return "erro";
}

int main(int argc, char *argv[])
{
     int sockfd, porta, b_size, n, aux,i, TAM_JANELA;	//sockfd - file descriptor - guarda informações do socket
     socklen_t clilen;
     FILE * rFile;
     so_addr sv_addr, cli_addr;
     char fname[9999];
     char janela[TAM_JANELA];

     b_size = atoi(argv[2]);
     char *buffer;
     buffer = (char*) malloc(sizeof(char)*b_size+1);

     if (argc < 3) {						//Erro se nao for passada a porta na qual o server esta operando
         fprintf(stderr,"Porta inexistente\n");
         exit(1);
     }

     bzero((char *) &sv_addr, sizeof(sv_addr));			//bzero zera os valores em um buffer
     porta = atoi(argv[1]);
     bzero(buffer,b_size);

     tp_init();
     sockfd = tp_socket(porta);					//Inicia o servidor e "amarra" na porta passada como argumento

     n = tp_recvfrom(sockfd, buffer, b_size, &cli_addr); // Recebe o nome do arquivo
     strcpy(fname, buffer);
     bzero(buffer,b_size);
     n = tp_recvfrom(sockfd, buffer, b_size, &cli_addr); // Recebe o tamanho da janela
     TAM_JANELA = atoi(buffer);
     rFile = fopen(fname,"r");					//Abre o arquivo em modo leitura
     int size = ftell(rFile);
     int frames = ceil(size/b_size);
     if (rFile!=NULL)
     {
     	int ack = 0, j = 1, i, copiou = 0, m;
	    aux = 1;
	  	struct timeval tv;
	  	tv.tv_sec = 1;  /* 1 Secs Timeout */
      	tv.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
        
	  	char bufferBkp[9999][b_size];
	  	while(aux)
	  	{
			char oi[b_size-7];
          	char result[b_size];
			int sair=1;
			for(i=ack; i<ack+TAM_JANELA; i++)
			{
				if(copiou==0)
				{
					char *oi2 = geraId(i);
					bzero(result,b_size);
					n = fread(oi, b_size-7, 1, rFile); //le bloco no arquivo do tamanho do buffer
					fseek(rFile, n-1, SEEK_CUR);
					strcpy(result, oi2);
					strcat(result, oi);
					strcpy(bufferBkp[i], result);

					m = tp_sendto(sockfd,result,b_size, &cli_addr); //Caso contrário manda um bloco

					if(n==0)
					{		//Se chegou no fim do arquivo, envia avisando para o cliente
						tp_sendto(sockfd,bufferBkp[i],b_size, &cli_addr);
						tp_sendto(sockfd,"FYN",strlen("FYN"), &cli_addr);
						aux=0;
						break;
					}
				}
				else
				{
					for(i=ack; i<ack+TAM_JANELA; i++)
				    {
				    	if(!strcmp(bufferBkp[i],"FYN")==0)
				     	{
					    	m = tp_sendto(sockfd,bufferBkp[i],b_size, &cli_addr); //Caso contrário manda um bloco
				     	}
				    }
				}
           	}
		    bzero(buffer,b_size);
		    if(tp_recvfrom(sockfd, buffer, b_size, &cli_addr)>0)
		    {
		    	strcpy(bufferBkp[atoi(buffer)],"FYN");
		    }
		    sair = 1;
		    for(i=ack; i<ack+TAM_JANELA; i++)
		    {
		    	if(!strcmp(bufferBkp[i],"FYN")==0)
		      	{
		      		sair = 0;
		      	}
		    }
		    if(sair==1)
		    {
		    	ack = ack + TAM_JANELA;
		    	copiou = 0;
		    }
		    else
		    {
		    	copiou = 1;
		    }
		}
     }
     //Finaliza as conexões e fecha os arquivos
     free(buffer);
     fclose(rFile);
     close(sockfd);

     return 0;
}

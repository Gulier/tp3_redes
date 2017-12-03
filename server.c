
/*******
	Carlos Augusto Lana de Mello
	2013062740
	Sistemas de Informação

	Executar o make na pasta onde se encontram os arquivos
		Chamar o servidor, na forma ./servidorFTP porta tam_buffer
		Chamar o cliente, na forma ./clienteFTP host_do_servidor porta_servidor nome_arquivo tam_buffer
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

#define TAM_JANELA 5

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
     int sockfd, porta, b_size, n, aux,i;	//sockfd - file descriptor - guarda informações do socket
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

     n = tp_recvfrom(sockfd, buffer, b_size, &cli_addr);

     strcpy(fname, buffer);
     rFile = fopen(fname,"r");					//Abre o arquivo em modo leitura
     //fseek(rFile, 0, SEEK_END);
     int size = ftell(rFile);
     int frames = ceil(size/b_size);
     if (rFile!=NULL)
     {
     		int ack = 0, j= 1;
	    	aux = 1;
	    	while(aux==1)
	    	{
		      	bzero(buffer,b_size);			//zera buffer
          		char oi[b_size-10];
          		char *oi2 = geraId(j);
		      	//fprintf(stderr, "%s", oi2);
          		char result[b_size];
          		bzero(result,b_size);
		      	//fprintf(stderr, "\n%s\n", result);
		      	n = fread(oi, b_size-10, 1, rFile);	//le bloco no arquivo do tamanho do buffer
		      	strcpy(result, oi2);
         		strcat(result, oi);
		      	//fprintf(stderr, "\n%s\n", result);
		      	fseek(rFile, n-1, SEEK_CUR);

		      	if(n==0)
		      	{						//Se chegou no fim do arquivo, envia avisando para o cliente
		      		//tp_sendto(sockfd,result,b_size, &cli_addr);
		      		tp_sendto(sockfd,"FYN",strlen("FYN"), &cli_addr);
				aux=0;
				break;
		      	}

		      	n = tp_sendto(sockfd,result,b_size, &cli_addr); //Caso contrário manda um bloco

		      	while(1){					//Espera algum sinal do cliente antes de enviar o próximo pacote
		      		bzero(buffer,b_size);
		      		if(tp_recvfrom(sockfd, buffer, b_size, &cli_addr)>0){
		      			fprintf(stderr, "%s", buffer);
              				break;				//Quando recebe, quebra o loop e volta ao procedimento
		      		}
		      	}
		      	if (n < 0)
      			 	error("ERROR in sendto");
	     		j++;
        	}
     }/*
     if (rFile!=NULL)
     {
     		int last_ack = 0;
	    	while(n!=0)
	    	{
			for(i=1; i<=frames; i++)					//de 1 até o total de frames necessários
			{
				if(i%TAM_JANELA==0)
				{							//se deu o tamanho da janela, envia e espera o ack
					tp_sendto(sockfd,buffer,b_size, &cli_addr);
					bzero(buffer,b_size);
					tp_recvfrom(sockfd, buffer, b_size, &cli_addr);
				}
				else
				{
					tp_sendto(sockfd,buffer,b_size, &cli_addr);	//caso contrario só envia
				}
		      	}
	     	}
     }*/
     //Finaliza as conexões e fecha os arquivos
     free(buffer);
     fclose(rFile);
     close(sockfd);

     return 0;
}


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
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "tp_socket.h"

void error(const char *msg)
{//Exibe erro e aborta o programa
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, porta, b_size, n, aux;	//sockfd - file descriptor - guarda informações do socket
     socklen_t clilen;
     FILE * rFile;
     so_addr sv_addr, cli_addr;
     char fname[9999];
     
     
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
	     
     if (rFile!=NULL)
     {
     		int ack = 0;
	    	aux = 1;
	    	while(aux==1)
	    	{
		      bzero(buffer,b_size);			//zera buffer
		      n = fread(buffer, b_size, 1, rFile);	//le bloco no arquivo do tamanho do buffer
		      fseek(rFile, n-1, SEEK_CUR);

		      if(n==0)
		      {						//Se chegou no fim do arquivo, envia avisando para o cliente
		      	tp_sendto(sockfd,buffer,b_size, &cli_addr);
		      	tp_sendto(sockfd,"FYN",strlen("FYN"), &cli_addr);
			aux=0;
			break;
		      }
		      n = tp_sendto(sockfd,buffer,b_size, &cli_addr); //Caso contrário manda um bloco
		      	      
		      while(1){					//Espera algum sinal do cliente antes de enviar o próximo pacote
		      	bzero(buffer,b_size);
		      	if(tp_recvfrom(sockfd, buffer, b_size, &cli_addr)>0){
		      		break;				//Quando recebe, quebra o loop e volta ao procedimento
		      	}
		      }
		      if (n < 0) 
      			 error("ERROR in sendto");
	     	}     	
     }
     //Finaliza as conexões e fecha os arquivos
     free(buffer);
     fclose(rFile);
     close(sockfd);
     
     return 0; 
} 

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
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "tp_socket.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, porta, n, b_size;		//sockfd- file descriptor - guarda informações do socket
    char fname[9999], host[9999];
    FILE * wFile;
    int bytes, cnt, TAM_JANELA;
    so_addr sv_addr;
    struct timeval start, end;
    struct hostent *server;

	TAM_JANELA = atoi(argv[5]);
    b_size = atoi(argv[4]);
    strcpy(host, argv[1]);
    char buffer[b_size+1];
    strcpy(fname, argv[3]);
    porta = atoi(argv[2]);

    if (argc < 5)
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    gettimeofday(&start, NULL);						//Inicia marcador de tempo

    tp_init();    							//Funções da biblioteca do tp para inicialização
    tp_build_addr(&sv_addr, host, porta);
    sockfd = tp_socket(0);
    bzero(buffer,b_size);

    n = tp_sendto(sockfd,fname,strlen(fname), &sv_addr);		//Envia o nome do arquivo para o servidor
    n = tp_sendto(sockfd,argv[5],strlen(argv[5]), &sv_addr);
    char bufferBkp[9999][b_size];
    wFile = fopen("Output.txt","w+");					//Abre arquivo de saída no modo escrita
    if (wFile!=NULL)
    {
        bytes = 0;
        int ack = 0, fynEnviado;

        while(1)
        {
        	n = tp_recvfrom(sockfd,buffer,b_size, &sv_addr);	//Recebe info do servidor
        	int i,j, copiar = 0;
        	if(strcmp(buffer,"FYN")==0){						//Se recebe FYN, termina execução
              	fynEnviado = 1;
        	}
        	else
        	{
        		char result[11];
		        strncpy(result, buffer, 6);
		  	    strncpy(bufferBkp[atoi(result)], buffer+6, strlen(buffer)-6);
		  	    tp_sendto(sockfd,result,strlen(result), &sv_addr);	//Confirma que recebeu a msg
        	}
          copiar = 1;
        	for(i=ack; i<ack+TAM_JANELA; i++)
		      {
			      if(strcmp(bufferBkp[i],"")==0 && !fynEnviado)
			      {
				      copiar = 0;
			      }

		      }

		      if(copiar== 1)
		      {
			      for(i=ack; i<ack+TAM_JANELA; i++)
			      {
              			if(strcmp(bufferBkp[i], "FYN")==0)
              			{
                			break;
              			}
              			else
              			{
              				bytes+=strlen(bufferBkp[i]);
                			fwrite(bufferBkp[i], 1, strlen(bufferBkp[i]), wFile);		//Escreve msg recebida no arquivo
			        	}
            		}
			      	copiar = 0;
            		ack+=TAM_JANELA;
		      	}
          		if (fynEnviado){
            	break;
          	  	}
    			//bytes+=strlen(buffer);

    			if (n < 1) error("Erro ao ler do socket");
    		  	bzero(buffer,b_size);
        }
    }
    else
    {
        printf("Erro leitura arquivo\n");
    }

    fclose (wFile);
    close(sockfd);

    gettimeofday(&end, NULL);					//Finaliza contagem de tempo e exibe informações da conexão
    double elapsed = (end.tv_sec - start.tv_sec) +
              ((end.tv_usec - start.tv_usec)/1000000.0);
    printf("%lf seconds\n", elapsed);
    printf("Buffer = \%5u byte(s), \%10.2lf kbps (\%u bytes em \%.6lfs)\n", bytes,
           ((bytes/1000)/elapsed), bytes, elapsed);

    return 0;
}

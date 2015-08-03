/** 
 *   Modulo: dumb
 *   @file dumb.c
 *   Veja dumb.h para mais informações.
 ******************************************************************************/

/*******************************************************************************
 *                             MODULOS UTILIZADOS							   *
 ******************************************************************************/

/*
 * Inclusão de arquivos de cabeçalho da ferramenta de desenvolvimento.
 * Por exemplo: '#include <stdlib.h>'.
 */
#include <stdint.h>   /* Para as definições de uint8_t/uint16_t */
#include <stdbool.h>  /* Para as definições de true/false */ 
#include "stdio.h"
#include "string.h"

/*
 * Inclusão de arquivos de cabeçalho sem um arquivo ".c" correspondente.
 * Por exemplo: 
 * #include "stddefs.h" 
 * #include "template_header.h"
 */

/*
 * Inclusão de arquivos de cabeçalho de outros módulos utilizados por este.
 * Por exemplo: '#include "serial.h"'.
 */
#include "lwip/api.h"

/*
 * Inclusão dos arquivos ".tab.h" deste módulo.
 * Por exemplo: 
 * #include "barcode.tab.h"
 * #include "template.tab.h"
 */

/*
 * Inclusão do arquivo de cabeçalho deste módulo.
 */
#include "dumb.h"

/*******************************************************************************
 *                     CONSTANTES E DEFINICOES DE MACRO						   *
 ******************************************************************************/
#if LWIP_NETCONN 			/* don't build if not configured for use in lwipopts.h */

/*******************************************************************************
 *                      ESTRUTURAS E DEFINIÇÕES DE TIPOS					   *
 ******************************************************************************/
struct command
{
	struct command *conn;
	s8_t (* exec)(struct command *);
	u8_t nargs;
	char *args[10];
}com;

/*******************************************************************************
 *                        VARIÁVEIS PUBLICAS (Globais)						   *
 ******************************************************************************/

/*******************************************************************************
 *                  DECLARACOES DE VARIAVEIS PRIVADAS (static)				   *
 ******************************************************************************/
static TaskHandle_t dumbhandle = NULL;
static u8_t buffer[DUMBBUFFERSIZE];

/*******************************************************************************
 *                   PROTOTIPOS DAS FUNCOES PRIVADAS (static)				   *
 ******************************************************************************/
static void dumbTask(void *pvParameters);
static void dumbProcessing(struct netconn *dumbconn);
static s8_t dumbProcessingCommand(struct command *com, u32_t len);
static s8_t comloutd(struct command *com);
static s8_t comsoutd(struct command *com);
static s8_t comcoutd(struct command *com);

/*******************************************************************************
 *                      IMPLEMENTACAO DAS FUNCOES PUBLICAS					   *
 ******************************************************************************/
/*
 * A documentação destas funções é realizada no arquivo ".h" deste módulo.
 */
static void dumbTask(void *pvParameters)
{
	struct netconn *dumbserv, *newconn;
	err_t err;

	LWIP_UNUSED_ARG(pvParameters);

	dumbserv = netconn_new(NETCONN_TCP);	/* Create a new connection identifier. */
	if (dumbserv != NULL)
	{
		/* Bind connection. */
		err = netconn_bind(dumbserv, NULL, DUMBPORT);
		if (err == ERR_OK)
		{
			/* Tell connection to go into listening mode. */
			netconn_listen(dumbserv);
			do
			{
				/* Grab new connection. */
				err = netconn_accept(dumbserv, &newconn);
				if (err == ERR_OK)
				{
					dumbProcessing(newconn);	/* Processing DUMB Protocol. */
					netconn_close(newconn);		/* Close connection. */
					netconn_delete(newconn);	/* Discard connection identifier. */
				}
			} while(err == ERR_OK);
		}
		else
		{
			printf("Can not bind TCP netconn.");
		}
		netconn_close(dumbserv);
		netconn_delete(dumbserv);
	}
	else
	{
		printf("Can not create TCP netconn.");
	}
}

/*****************************************************************************/
TaskHandle_t dumbGetHandle(void)
{
	return (dumbhandle);
}

/*****************************************************************************/
void dumbSetHandle(TaskHandle_t handle)
{
	dumbhandle = handle;
}

/*****************************************************************************/
static void dumbProcessing(struct netconn *dumbconn)
{
	struct pbuf *p;
	u16_t len = 0, cur_len;
	struct command com;
	s8_t err;
	err_t ret;

	do
	{
		ret = netconn_recv_tcp_pbuf(dumbconn, &p);
		if (ret == ERR_OK)
		{
			pbuf_copy_partial(p, &buffer[len], DUMBBUFFERSIZE - len, 0);
			cur_len = p->tot_len;
			len += cur_len;
			pbuf_free(p);
			if (((len > 0) && ((buffer[len-1] == '\r') || (buffer[len-1] == '\n'))) || (len >= DUMBBUFFERSIZE))
			{
				if (buffer[0] != 0xff && buffer[1] != 0xfe)
				{
					err = dumbProcessingCommand(&com, len);
					if (err == DUMBSUCCESS)
					{
						com.conn = dumbconn;
						err = com.exec(&com);
					}
					if (err == DUMBCLOSED)
					{
						netconn_write(dumbconn, (void *)DUMBERRORFLAG, strlen(DUMBERRORFLAG), NETCONN_NOCOPY);
						printf("Closed.");
					}
					if (err != DUMBSUCCESS)
					{
						netconn_write(dumbconn, (void *)DUMBERRORFLAG, strlen(DUMBERRORFLAG), NETCONN_NOCOPY);
					}
				}
				len = 0;
			}
		}
	} while (ret == ERR_OK);
}

/*-----------------------------------------------------------------------------------*/
static s8_t comloutd(struct command *com)
{
	u8_t i;
	s8_t output[20];

	for(i = OUTPUTPORTMINIOPIN; i <= OUTPUTPORTMAXIOPIN; i++)
	{
		if(GPIO_ReadValue(OUTPUTPORTIO) & (1 << i))
		{
			sprintf(output, "\nOutput p%d.%d: %s", OUTPUTPORTIO, i, DUMBONMSG);
		}
		else
		{
			sprintf(output, "\nOutput p%d.%d: %s", OUTPUTPORTIO, i, DUMBOFFMSG);
		}
		netconn_write(com->conn, (void *)output, strlen(output), NETCONN_NOCOPY);
	}
	return (DUMBSUCCESS);
}

/*-----------------------------------------------------------------------------------*/
static s8_t comsoutd(struct command *com)
{
	u8_t i;

	i = strtol(com->args[0], NULL, 10);
	GPIO_SetValue(OUTPUTPORTIO, (1 << i));
	netconn_write(com->conn, (void *)DUMBONMSG, strlen(DUMBONMSG), NETCONN_NOCOPY);

	return (DUMBSUCCESS);
}

/*-----------------------------------------------------------------------------------*/
static s8_t comcoutd(struct command *com)
{
	u8_t i;

	i = strtol(com->args[0], NULL, 10);
	GPIO_ClearValue(OUTPUTPORTIO, (1 << i));
	netconn_write(com->conn, (void *)DUMBOFFMSG, strlen(DUMBOFFMSG), NETCONN_NOCOPY);

	return (DUMBSUCCESS);
}

/*-----------------------------------------------------------------------------------*/
static s8_t dumbProcessingCommand(struct command *com, u32_t len)
{
	u16_t i;
	u16_t bufp;

	if (strncmp((const char *)buffer, "CMDSCLK", 7) == 0)
	{
	}
	else if (strncmp((const char *)buffer, "CMDWRCFG", 8) == 0)
	{
	}
	else if (strncmp((const char *)buffer, "CMDRDCFG", 8) == 0)
	{
	}
	else if (strncmp((const char *)buffer, "CMDSOUTD", 8) == 0)
	{
		com->exec = comsoutd;
		com->nargs = 1;
	}
	else if (strncmp((const char *)buffer, "CMDLOUTD", 8) == 0)
	{
		com->exec = comloutd;
		com->nargs = 0;
	}
	else if (strncmp((const char *)buffer, "CMDCOUTD", 8) == 0)
	{
		com->exec = comcoutd;
		com->nargs = 1;
	}
	else if (strncmp((const char *)buffer, "CMDRINPA", 8) == 0)
	{
	}
	else if (strncmp((const char *)buffer, "CMDLINPA", 8) == 0)
	{
	}
	else
	{
		return (DUMBSYNTAX);
	}

	if (com->nargs == 0)
	{
		return (DUMBSUCCESS);
	}

	bufp = 0;
	for(; bufp < len && buffer[bufp] != ' '; bufp++);
	for(i = 0; i < 10; i++)
	{
		for(; bufp < len && buffer[bufp] == ' '; bufp++);
		if (buffer[bufp] == '\r' || buffer[bufp] == '\n')
		{
			buffer[bufp] = 0;
			if (i < com->nargs - 1)
			{
				return (DUMBTOOFEW);
			}
			if (i > com->nargs - 1)
			{
				return (DUMBTOOMANY);
			}
			break;
		}
		if (bufp > len)
		{
			return (DUMBTOOFEW);
		}
		com->args[i] = (char *)&buffer[bufp];
		for(; bufp < len && buffer[bufp] != ' ' && buffer[bufp] != '\r' && buffer[bufp] != '\n'; bufp++)
		{
			if (buffer[bufp] == '\\')
			{
				buffer[bufp] = ' ';
			}
		}
		if (bufp > len)
		{
			return (DUMBSYNTAX);
		}
		buffer[bufp] = 0;
		bufp++;
		if (i == com->nargs - 1)
		{
			break;
		}
	}
	return (DUMBSUCCESS);
}

/*****************************************************************************/
TaskHandle_t dumbInit(void)
{
	dumbhandle = sys_thread_new("dumbs", dumbTask, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
	return(dumbhandle);
}

/******************************************************************************
 *                    IMPLEMENTACAO DAS FUNCOES PRIVADAS					  *
 *****************************************************************************/

/******************************************************************************	
 *                                    EOF                                     *
 *****************************************************************************/
#endif

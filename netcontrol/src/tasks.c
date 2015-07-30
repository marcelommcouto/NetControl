/** 
 *   Modulo: tasks
 *   @file tasks.c
 *   Veja tasks.h para mais informações.
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
#include <stdio.h>
#include <string.h>

/*
 * Inclusão de arquivos de cabeçalho sem um arquivo ".c" correspondente.
 * Por exemplo: 
 * #include "stddefs.h" 
 * #include "template_header.h"
 */
#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
/*
 * Inclusão de arquivos de cabeçalho de outros módulos utilizados por este.
 * Por exemplo: '#include "serial.h"'.
 */
#include "task.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "arch/lpc17xx_40xx_emac.h"
#include "arch/lpc_phy_smsc87x0.h"
#include "arch/enet_17xx_40xx.h"

/*
 * Inclusão dos arquivos ".tab.h" deste módulo.
 * Por exemplo: 
 * #include "barcode.tab.h"
 * #include "template.tab.h"
 */

/*
 * Inclusão do arquivo de cabeçalho deste módulo.
 */
#include "tasks.h"

/*******************************************************************************
 *                     CONSTANTES E DEFINICOES DE MACRO						   *
 ******************************************************************************/

/*******************************************************************************
 *                      ESTRUTURAS E DEFINIÇÕES DE TIPOS					   *
 ******************************************************************************/

/*******************************************************************************
 *                        VARIÁVEIS PUBLICAS (Globais)						   *
 ******************************************************************************/

/*******************************************************************************
 *                  DECLARACOES DE VARIAVEIS PRIVADAS (static)				   *
 ******************************************************************************/
static struct netif lpc17xx_netif;

/*******************************************************************************
 *                   PROTOTIPOS DAS FUNCOES PRIVADAS (static)				   *
 ******************************************************************************/

/*******************************************************************************
 *                      IMPLEMENTACAO DAS FUNCOES PUBLICAS					   *
 ******************************************************************************/
/*
 * A documentação destas funções é realizada no arquivo ".h" deste módulo.
 */
void blinktask(void *pvParameters)
{
	/* No caso, esse será o led que piscará... */
	LPC_GPIO0->FIODIR |= (1<<4);

	while(1)
	{
		LPC_GPIO0->FIOSET = (1<<4);
		vTaskDelay(500/portTICK_RATE_MS);
		LPC_GPIO0->FIOCLR = (1<<4);
		vTaskDelay(500/portTICK_RATE_MS);
	}
}

// TODO: insert other definitions and declarations here
void ethernettask(void *pvParameters)
{
	uint32_t physts;
	struct ip_addr ipaddr, netmask, gw;
	uint8_t prt_ip = 0;

	/* Inicializando Pilha TCP/IP. */
	tcpip_init(NULL, NULL);

	/* Configurando o IP. Se o DHCP será ativado, podemos deixar esse campo zerado. */
	IP4_ADDR(&gw, 0, 0, 0, 0);
	IP4_ADDR(&ipaddr, 0, 0, 0, 0);
	IP4_ADDR(&netmask, 0, 0, 0, 0);

	/* Configurando os parametros da pilha TCP/IP. */
	memset(&lpc17xx_netif, 0, sizeof(lpc17xx_netif));
	netif_add(&lpc17xx_netif, &ipaddr, &netmask, &gw, NULL, lpc_enetif_init, tcpip_input);
	netif_set_default(&lpc17xx_netif);
	netif_set_up(&lpc17xx_netif);

	/* Enable MAC interrupts only after LWIP is ready */
	NVIC_SetPriority(ENET_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
	NVIC_EnableIRQ(ENET_IRQn);

	dhcp_start(&lpc17xx_netif);

	/* This loop monitors the PHY link and will handle cable events via the PHY driver. */
	while (1)
	{
		/* Call the PHY status update state machine once in a while to keep the link status up-to-date */
		physts = lpcPHYStsPoll();

		/* Only check for connection state when the PHY status has changed */
		if (physts & PHY_LINK_CHANGED)
		{
			if (physts & PHY_LINK_CONNECTED)
			{
				prt_ip = 0;

				/* Set interface speed and duplex */
				if (physts & PHY_LINK_SPEED100)
				{
					Chip_ENET_Set100Mbps(LPC_EMAC);
					NETIF_INIT_SNMP(&lpc_netif, snmp_ifType_ethernet_csmacd, 100000000);
				}
				else
				{
					Chip_ENET_Set10Mbps(LPC_EMAC);
					NETIF_INIT_SNMP(&lpc_netif, snmp_ifType_ethernet_csmacd, 10000000);
				}
				if (physts & PHY_LINK_FULLDUPLX)
				{
					Chip_ENET_SetFullDuplex(LPC_EMAC);
				}
				else
				{
					Chip_ENET_SetHalfDuplex(LPC_EMAC);
				}
				tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_up, (void *) &lpc17xx_netif, 1);
				printf("\nCabo colocado.");
			}
			else
			{
				tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_down, (void *) &lpc17xx_netif, 1);
				printf("\nCabo retirado.");
			}
			vTaskDelay(configTICK_RATE_HZ / 2);		/* Delay for link detection (500mS) */
		}

		/* Print IP address info */
		if (!prt_ip)
		{
			if (lpc17xx_netif.ip_addr.addr)
			{
				static char tmp_buff[16];
				prt_ip = 1;

				printf("\nIP_ADDR 	: %s", ipaddr_ntoa_r((const ip_addr_t *) &lpc17xx_netif.ip_addr, tmp_buff, 16));
				printf("\nNET_MASK  : %s", ipaddr_ntoa_r((const ip_addr_t *) &lpc17xx_netif.netmask, tmp_buff, 16));
				printf("\nGATEWAY_IP: %s", ipaddr_ntoa_r((const ip_addr_t *) &lpc17xx_netif.gw, tmp_buff, 16));
			}
		}
	}
}

/******************************************************************************
 *                    IMPLEMENTACAO DAS FUNCOES PRIVADAS					  *
 *****************************************************************************/

/******************************************************************************	
 *                                    EOF                                     *
 *****************************************************************************/

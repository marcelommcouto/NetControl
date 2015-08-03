#ifndef DUMB_H_
#define DUMB_H_

/** ************************************************************************
 * Modulo: dumb 
 * @file dumb.h
 * @headerfile dumb.h
 * @author Marcelo Martins Maia do Couto - Email: marcelo.m.maia@gmail.com
 * @date Jul 24, 2015
 *
 * @brief Substitua este texto por uma descrição breve deste módulo.
 *
 * Substitua este texto pela descrição completa deste módulo.
 * Este módulo é um modelo para a criação de novos módulos. Ele contém
 * toda a estrutura que um módulo deve conter, sendo composto pelos arquivos:
 *   - dumb.c;
 *   - dumb.h.
 *
 * @copyright Copyright 2015 M3C Tecnologia
 * @copyright Todos os direitos reservados.
 *
 * @note
 *  - Não sobrescreva os arquivos de template do módulo. Implemente um novo
 *    módulo sobre uma cópia do template.
 *  - Os padrões de comentário que começam com "/** ", como este, devem ser
 *    compilados com a ferramenta Doxygen (comando:
 *    "doxygen.exe doxygen.cfg").
 *  - Leia a documentação do @b Doxygen para maiores informações sobre o
 *    funcionamento dos recursos de documentação de código.
 *
 * @warning
 *  - É altamente recomendado manter todos os arquivos de template como
 *    somente-leitura, evitando assim que eles sejam sobrescritos ao serem
 *    utilizados.
 *
 * @attention
 *  - A descrição de cada módulo como um todo deve ser realizada no arquivo
 *    ".h" do mesmo.
 *  - Cada módulo de um projeto de software deve conter, pelo menos, um
 *    arquivo ".h" e um ".c".
 * @pre 
 *   Coloque algum pré-requisito para utilizar este módulo aqui.
 *
 ******************************************************************************/

/*
 * Inclusão de arquivos de cabeçalho da ferramenta de desenvolvimento.
 * Por exemplo: '#include <stdlib.h>'.
 */

/*
 * Inclusão de arquivos de cabeçalho sem um arquivo ".c" correspondente.
 * Por exemplo: '#include "stddefs.h"'.
 */

/*
 * Inclusão de arquivos de cabeçalho de outros módulos utilizados por este.
 * Por exemplo: '#include "serial.h"'.
 */
#include "lpc17xx_gpio.h"

/*******************************************************************************
 *                           DEFINICOES E MACROS							   *
 ******************************************************************************/
#define DUMBPORT		23
#define DUMBBUFFERSIZE	50

#define OUTPUTPORTIO		0	/* Port usado pelas saídas. */
#define OUTPUTPORTMINIOPIN	5
#define OUTPUTPORTMAXIOPIN	11

#define DUMBERRORFLAG	"\nERROR!"
#define DUMBONMSG		"On.\n"
#define DUMBOFFMSG		"Off.\n"

#define DUMBSUCCESS 	0
#define DUMBSYNTAX 		-1
#define DUMBTOOFEW 		-2
#define DUMBTOOMANY 	-3
#define DUMBCLOSED 		-4

/*******************************************************************************
 *                     ESTRUTURAS E DEFINICOES DE TIPOS						   *	
 ******************************************************************************/

/*******************************************************************************
 *                       VARIAVEIS PUBLICAS (Globais)						   *
 ******************************************************************************/

/*******************************************************************************
 *                      PROTOTIPOS DAS FUNCOES PUBLICAS						   *
 ******************************************************************************/

/**
 * Inicialização deste módulo.
 *
 * @return void
 *
 * @pre Descreva a pré-condição para esta função
 *
 * @post Descreva a pós-condição para esta função
 *
 * @invariant Descreva o que não pode variar quando acabar a execução da função 
 *
 * @note
 *  Esta função deve ser chamada durante a inicialização do software caso este
 *  módulo seja utilizado.
 *
 * <b> Exemplo de uso: </b>
 * @code
 *    template_init();
 * @endcode
 ******************************************************************************/
TaskHandle_t dumbInit(void);
TaskHandle_t dumbGetHandle(void);
void dumbSetHandle(TaskHandle_t handle);

/*******************************************************************************
 *                                   EOF									   *	
 ******************************************************************************/
#endif

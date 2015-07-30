#ifndef STACK_SIZE
#define STACK_SIZE  (0x400)
#endif

/* Top of stack that comes from linker script */
extern void _vStackTop(void);

// *****************************************************************************
//
// Heap overflow check function required by REDLib_V2 library
// Without the fix Redlib's malloc calls from inside freeRtos tasks
// will always return NULL.
//
// *****************************************************************************
extern unsigned int *_pvHeapStart;
unsigned int __check_heap_overflow (void * new_end_of_heap)
{
	unsigned long stackend = ((unsigned long) &_vStackTop) - STACK_SIZE;
	return ((unsigned long)new_end_of_heap >= stackend);
}

#ifdef LWIP_DEBUG

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Displays an error message on assertion */
void assert_printf(char *msg, int line, char *file)
{
	if (msg) {
		LWIP_DEBUGF(LWIP_DBG_ON, ("%s:%d in file %s\n", msg, line, file));
	}
	while (1) {}
}

#else
/* LWIP optimized assertion loop (no LWIP_DEBUG) */
void assert_loop(void)
{
//	while (1) {}
}

 #endif /* LWIP_DEBUG */

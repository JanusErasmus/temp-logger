#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef _DEBUG
#define diag_printf(_x) printf _x
#else
#define diag_printf(_x)
#endif

#endif /* DEBUG_H_ */

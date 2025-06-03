#ifndef HANDLER_H_
#define HANDLER_H_

#include "..\base.h"

#define DIGISIZE (4)
#define NUMBASE  (10) // Basis des Zahlensystems kann zwischen 2 und 16 gewählt werden

EXTERN inline Void Handler_init(Void);
EXTERN Void Button_Handler(Void);
EXTERN Void Number_Handler(Void);
EXTERN Void Display_Handler(Void);

#endif /* HANDLER_H_ */

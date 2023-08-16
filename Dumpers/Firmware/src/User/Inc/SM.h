#ifndef __SM_H
#define __SM_H

#ifdef __cplusplus
 extern "C" {
#endif

void SM_GPIO_Init(void);
void SM_Dump(void);
void SM_Prog(void);
void SM_Veri(void);
void SM_Test(void);

#ifdef __cplusplus
}
#endif

#endif /* __SM_H */

#ifndef __CV_H
#define __CV_H

#ifdef __cplusplus
 extern "C" {
#endif

void CV_GPIO_Init(void);
void CV_Dump(void);
void CV_Prog(void);
void CV_Veri(void);
void CV_Test(void);

#ifdef __cplusplus
}
#endif

#endif /* __CV_H */

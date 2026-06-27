/*
 * APS.h
 *
 *  Created on: Jun 27, 2026
 *      Author: yagiz20Z
 */

#ifndef INC_kalman_H_
#define INC_kalman_H_

#define R 9.0f      // Sensöre ne kadar güveniyorum   // yüksek olursa hızlı olur ama stabil olmaz 
#define Q 0.05f     //sistemin değişim hızıdır
#define k  // Yeni ölçümü ne kadar dikkate alacağım
#define P  // Tahminimin ne kadar hatalı olabileceği

float x;  // tahmin edilen değer




#endif
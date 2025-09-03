/*
 * fuzzy.h
 *
 * Created: 3/9/2025 17:19:37
 *  Author: ADMIN
 */ 


#ifndef FUZZY_H
#define FUZZY_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

// ---------------- Macros ----------------
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define X1_SIZE 5
#define X2_SIZE 5
#define N 5

// ---------------- Variables globales ----------------
extern float alfa;
extern float x1_val[X1_SIZE];
extern float x2_val[X2_SIZE];

extern float mat_centro[N][N];
extern float mat_estimula[N][N];

// ---------------- Prototipos ----------------
float trapmf(float x, float params[4]);
void generarEstimula(void);
void generarMatCentro(float alfa);
float defuzzy(float mat_estimula[N][N], float mat_centro[N][N]);
float fuzzy(float x, float x2);

#endif

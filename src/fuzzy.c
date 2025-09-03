/*
 * fuzzy.c
 *
 * Created: 3/9/2025 17:19:50
 *  Author: ADMIN
 */ 

#include "fuzzy.h"

// ---------------- Variables ----------------
float alfa = 250.0;
float x1_val[X1_SIZE];
float x2_val[X2_SIZE];

float mat_centro[N][N];
float mat_estimula[N][N];

// ---------------- Tablas en PROGMEM ----------------
const int16_t con_dif_1[5][4] PROGMEM = {
	{-2, -1, 0, 6},
	{0, 6, 6, 12},
	{6, 12, 12, 18},
	{12, 18, 18, 24},
	{18, 30, 31, 32}
};

const int16_t con_dif_2[5][4] PROGMEM = {
	{-32, -31, -30, -15},
	{-30, -15, -15, 0},
	{-15, 0, 0, 15},
	{0, 15, 15, 30},
	{15, 30, 31, 32}
};

const int16_t mat_centro_2[5][4] PROGMEM = {
	{-2, -1, 0, 62},
	{0, 62, 62, 125},
	{62, 125, 125, 187},
	{125, 187, 187, 250},
	{187, 250, 251, 252}
};

// ---------------- Funciones ----------------
float trapmf(float x, float params[4]) {
	float val1, val2, val3;

	if (params[1] == params[0])
	val1 = 0;
	else
	val1 = (x - params[0]) / (params[1] - params[0]);

	if (params[3] == params[2])
	val3 = 0;
	else
	val3 = (params[3] - x) / (params[3] - params[2]);

	val2 = 1.0;

	return MAX(MIN(val1, MIN(val2, val3)), 0.0);
}

void generarEstimula(void) {
	for (uint8_t i = 0; i < X1_SIZE; i++) {
		for (uint8_t j = 0; j < X2_SIZE; j++) {
			mat_estimula[i][j] = (x1_val[i] < x2_val[j]) ? x1_val[i] : x2_val[j];
		}
	}
}

void generarMatCentro(float alfa) {
	float paso = alfa / (N - 1);
	for (uint8_t i = 0; i < N; i++) {
		for (uint8_t j = 0; j < N; j++) {
			mat_centro[i][j] = alfa - (i + j) * paso;
		}
	}
}

float defuzzy(float mat_estimula[N][N], float mat_centro[N][N]) {
	float sum_mu = 0.0;
	float sum_mu_c = 0.0;
	for (uint8_t i = 0; i < N; i++) {
		for (uint8_t j = 0; j < N; j++) {
			sum_mu_c += mat_estimula[i][j] * mat_centro[i][j];
			sum_mu += mat_estimula[i][j];
		}
	}
	return (sum_mu == 0) ? 0 : sum_mu_c / sum_mu;
}

float fuzzy(float x, float x2) {
	float params[4];

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 4; j++) {
			params[j] = (float)pgm_read_word(&(con_dif_1[i][j]));
		}
		x1_val[i] = trapmf(x, params);

		for (int j = 0; j < 4; j++) {
			params[j] = (float)pgm_read_word(&(con_dif_2[i][j]));
		}
		x2_val[i] = trapmf(x2, params);
	}

	generarEstimula();
	generarMatCentro(alfa);
	return defuzzy(mat_estimula, mat_centro);
}

#include "registradores.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ---- estrutura interna (definida APENAS no .c) ---- */

typedef struct {
    double x;
    double y;
    bool   ocupado;
} RegSlot;

typedef struct {
    RegSlot slots[REG_TOTAL];
} RegistradoresImpl;

/* ---- ciclo de vida ---- */

Registradores registradores_criar(void) {
    RegistradoresImpl *r = calloc(1, sizeof(RegistradoresImpl));
    /* calloc zera: todos os slots começam livres (ocupado = false) */
    return r;
}

void registradores_destruir(Registradores regs) {
    free(regs);
}

/* ---- índice a partir do nome "R0".."R10" ---- */

int reg_indice(const char *nome) {
    if (!nome || (nome[0] != 'R' && nome[0] != 'r')) return -1;

    const char *digitos = nome + 1;
    if (*digitos == '\0') return -1;

    int valor = 0;
    for (const char *p = digitos; *p; p++) {
        if (!isdigit((unsigned char)*p)) return -1;
        valor = valor * 10 + (*p - '0');
        if (valor >= REG_TOTAL) {
            /* pode ainda ser exatamente REG_TOTAL-1 no último dígito; checa no fim */
            if (valor > REG_TOTAL) return -1;
        }
    }
    if (valor < 0 || valor >= REG_TOTAL) return -1;
    return valor;
}

/* ---- armazenar / obter ---- */

bool reg_armazenar(Registradores regs, int idx, double x, double y) {
    if (!regs || idx < 0 || idx >= REG_TOTAL) return false;
    RegistradoresImpl *r = regs;
    r->slots[idx].x = x;
    r->slots[idx].y = y;
    r->slots[idx].ocupado = true;
    return true;
}

bool reg_obter(Registradores regs, int idx, double *x_out, double *y_out) {
    if (!regs || idx < 0 || idx >= REG_TOTAL) return false;
    RegistradoresImpl *r = regs;
    if (!r->slots[idx].ocupado) return false;
    if (x_out) *x_out = r->slots[idx].x;
    if (y_out) *y_out = r->slots[idx].y;
    return true;
}

bool reg_ocupado(Registradores regs, int idx) {
    if (!regs || idx < 0 || idx >= REG_TOTAL) return false;
    RegistradoresImpl *r = regs;
    return r->slots[idx].ocupado;
}

/* ---- cálculo de coordenada por endereço ---- */

bool reg_coordenada_endereco(double qx, double qy, double qw, double qh,
                             char face, double num,
                             double *x_out, double *y_out) {
    if (!x_out || !y_out) return false;

    /* Canto sudeste (ancoragem) = (qx + qw, qy + qh) */
    double se_x = qx + qw;
    double se_y = qy + qh;

    switch (toupper((unsigned char)face)) {
        case 'N': /* topo: y fixo no topo; anda para oeste a partir do SE projetado */
            *x_out = se_x - num;
            *y_out = qy;
            return true;
        case 'S': /* base: y fixo na base; anda para oeste a partir do SE */
            *x_out = se_x - num;
            *y_out = se_y;
            return true;
        case 'L': /* direita: x fixo à direita; anda para norte a partir do SE */
            *x_out = se_x;
            *y_out = se_y - num;
            return true;
        case 'O': /* esquerda: x fixo à esquerda; anda para norte a partir do SE projetado */
            *x_out = qx;
            *y_out = se_y - num;
            return true;
        default:
            return false;
    }
}

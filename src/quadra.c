#include "quadra.h"

#include <stdlib.h>
#include <string.h>

struct quadra
{
  char *cep;
  double x;
  double y;
  double largura;
  double altura;
  char *cor_preenchimento;
  char *cor_borda;
  double espessura_borda;
};

static char *quadra_copiar_texto(const char *texto)
{
  size_t tamanho;
  char *copia;

  if (texto == NULL || texto[0] == '\0')
  {
    return NULL;
  }

  tamanho = strlen(texto) + 1u;
  copia = (char *)malloc(tamanho);
  if (copia == NULL)
  {
    return NULL;
  }

  memcpy(copia, texto, tamanho);
  return copia;
}

static int quadra_copiar_campo(char *destino, size_t capacidade,
                               const char *origem)
{
  size_t tamanho;

  if (destino == NULL || origem == NULL)
  {
    return 0;
  }

  tamanho = strlen(origem);
  if (tamanho >= capacidade)
  {
    return 0;
  }

  memset(destino, 0, capacidade);
  memcpy(destino, origem, tamanho);
  return 1;
}

static int quadra_argumentos_validos(const char *cep, double largura,
                                     double altura,
                                     const char *cor_preenchimento,
                                     const char *cor_borda,
                                     double espessura_borda)
{
  return cep != NULL && cep[0] != '\0' && largura > 0.0 && altura > 0.0 &&
         cor_preenchimento != NULL && cor_preenchimento[0] != '\0' &&
         cor_borda != NULL && cor_borda[0] != '\0' &&
         espessura_borda >= 0.0;
}

quadra_t *quadra_criar(const char *cep, double x, double y, double largura,
                       double altura, const char *cor_preenchimento,
                       const char *cor_borda, double espessura_borda)
{
  quadra_t *quadra;

  if (!quadra_argumentos_validos(cep, largura, altura, cor_preenchimento,
                                 cor_borda, espessura_borda))
  {
    return NULL;
  }

  quadra = (quadra_t *)calloc(1u, sizeof(*quadra));
  if (quadra == NULL)
  {
    return NULL;
  }

  quadra->cep = quadra_copiar_texto(cep);
  quadra->cor_preenchimento = quadra_copiar_texto(cor_preenchimento);
  quadra->cor_borda = quadra_copiar_texto(cor_borda);

  if (quadra->cep == NULL || quadra->cor_preenchimento == NULL ||
      quadra->cor_borda == NULL)
  {
    quadra_destruir(quadra);
    return NULL;
  }

  quadra->x = x;
  quadra->y = y;
  quadra->largura = largura;
  quadra->altura = altura;
  quadra->espessura_borda = espessura_borda;

  return quadra;
}

void quadra_destruir(quadra_t *quadra)
{
  if (quadra == NULL)
  {
    return;
  }

  free(quadra->cep);
  free(quadra->cor_preenchimento);
  free(quadra->cor_borda);
  free(quadra);
}

const char *quadra_obter_cep(const quadra_t *quadra)
{
  return quadra == NULL ? NULL : quadra->cep;
}

double quadra_obter_x(const quadra_t *quadra)
{
  return quadra == NULL ? 0.0 : quadra->x;
}

double quadra_obter_y(const quadra_t *quadra)
{
  return quadra == NULL ? 0.0 : quadra->y;
}

double quadra_obter_largura(const quadra_t *quadra)
{
  return quadra == NULL ? 0.0 : quadra->largura;
}

double quadra_obter_altura(const quadra_t *quadra)
{
  return quadra == NULL ? 0.0 : quadra->altura;
}

const char *quadra_obter_cor_preenchimento(const quadra_t *quadra)
{
  return quadra == NULL ? NULL : quadra->cor_preenchimento;
}

const char *quadra_obter_cor_borda(const quadra_t *quadra)
{
  return quadra == NULL ? NULL : quadra->cor_borda;
}

double quadra_obter_espessura_borda(const quadra_t *quadra)
{
  return quadra == NULL ? 0.0 : quadra->espessura_borda;
}

/* ---- Registro serializável (layout privado) ---- */

typedef struct
{
  char   cep[QUADRA_CEP_MAX + 1u];
  double x;
  double y;
  double largura;
  double altura;
  char   cor_preenchimento[QUADRA_COR_MAX + 1u];
  char   cor_borda[QUADRA_COR_MAX + 1u];
  double espessura_borda;
} quadra_registro_s;

size_t quadra_registro_size(void)
{
  return sizeof(quadra_registro_s);
}

void quadra_registro_preencher(quadra_registro_t *registro, const char *cep,
                               double x, double y, double largura, double altura,
                               const char *cor_preenchimento,
                               const char *cor_borda, double espessura_borda)
{
  quadra_registro_s *r = (quadra_registro_s *)registro;

  if (r == NULL)
  {
    return;
  }

  memset(r, 0, sizeof(*r));
  if (cep != NULL)
    quadra_copiar_campo(r->cep, sizeof(r->cep), cep);
  if (cor_preenchimento != NULL)
    quadra_copiar_campo(r->cor_preenchimento, sizeof(r->cor_preenchimento),
                        cor_preenchimento);
  if (cor_borda != NULL)
    quadra_copiar_campo(r->cor_borda, sizeof(r->cor_borda), cor_borda);
  r->x = x;
  r->y = y;
  r->largura = largura;
  r->altura = altura;
  r->espessura_borda = espessura_borda;
}

const char *quadra_registro_cep(const quadra_registro_t *registro)
{
  return registro == NULL ? NULL : ((const quadra_registro_s *)registro)->cep;
}

double quadra_registro_x(const quadra_registro_t *registro)
{
  return registro == NULL ? 0.0 : ((const quadra_registro_s *)registro)->x;
}

double quadra_registro_y(const quadra_registro_t *registro)
{
  return registro == NULL ? 0.0 : ((const quadra_registro_s *)registro)->y;
}

double quadra_registro_largura(const quadra_registro_t *registro)
{
  return registro == NULL ? 0.0 : ((const quadra_registro_s *)registro)->largura;
}

double quadra_registro_altura(const quadra_registro_t *registro)
{
  return registro == NULL ? 0.0 : ((const quadra_registro_s *)registro)->altura;
}

const char *quadra_registro_cor_preenchimento(const quadra_registro_t *registro)
{
  return registro == NULL
             ? NULL
             : ((const quadra_registro_s *)registro)->cor_preenchimento;
}

const char *quadra_registro_cor_borda(const quadra_registro_t *registro)
{
  return registro == NULL
             ? NULL
             : ((const quadra_registro_s *)registro)->cor_borda;
}

double quadra_registro_espessura_borda(const quadra_registro_t *registro)
{
  return registro == NULL
             ? 0.0
             : ((const quadra_registro_s *)registro)->espessura_borda;
}

int quadra_para_registro(const quadra_t *quadra, quadra_registro_t *registro)
{
  if (quadra == NULL || registro == NULL)
  {
    return 0;
  }

  quadra_registro_preencher(registro, quadra->cep, quadra->x, quadra->y,
                            quadra->largura, quadra->altura,
                            quadra->cor_preenchimento, quadra->cor_borda,
                            quadra->espessura_borda);
  return 1;
}

quadra_t *quadra_criar_de_registro(const quadra_registro_t *registro)
{
  if (registro == NULL)
  {
    return NULL;
  }

  return quadra_criar(quadra_registro_cep(registro),
                      quadra_registro_x(registro), quadra_registro_y(registro),
                      quadra_registro_largura(registro),
                      quadra_registro_altura(registro),
                      quadra_registro_cor_preenchimento(registro),
                      quadra_registro_cor_borda(registro),
                      quadra_registro_espessura_borda(registro));
}

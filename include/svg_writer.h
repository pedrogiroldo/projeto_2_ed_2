/**
 * @file svg_writer.h
 * @brief Gerador incremental de arquivos SVG.
 *
 * Crie com svg_writer_criar(), adicione elementos com as funções de desenho e
 * finalize com svg_writer_finalizar() para fechar o tag raiz e salvar.
 * svg_writer_destruir() libera a memória independentemente de ter finalizado.
 *
 * O viewBox final é calculado dinamicamente a partir dos elementos desenhados,
 * com uma margem fixa. Os parâmetros @c largura e @c altura passados a
 * svg_writer_criar() são usados apenas como fallback quando nenhum elemento
 * é desenhado.
 */

#ifndef SVG_WRITER_H
#define SVG_WRITER_H

/** Tipo opaco para o writer SVG. */
typedef struct svg_writer svg_writer_t;

/**
 * @brief Cria um novo writer SVG e abre @p output_path para escrita.
 *
 * @param output_path Caminho do arquivo SVG de saída (não-NULL).
 * @param largura     Largura fallback usada quando nenhum elemento é desenhado.
 * @param altura      Altura fallback usada quando nenhum elemento é desenhado.
 * @return Nova instância ou NULL se @p output_path for NULL ou o arquivo
 *         não puder ser aberto.
 */
svg_writer_t *svg_writer_criar(const char *output_path,
                               double largura, double altura);

/**
 * @brief Fecha o elemento raiz @c \<svg\> e o arquivo.
 *
 * Deve ser chamado antes de svg_writer_destruir() para produzir um SVG válido.
 * Aceita NULL com segurança.
 *
 * @param sw Writer a finalizar.
 */
void svg_writer_finalizar(svg_writer_t *sw);

/**
 * @brief Libera a memória do writer. Não chama finalizar internamente.
 *
 * Chame svg_writer_finalizar() antes desta função para fechar o SVG corretamente.
 * Aceita NULL com segurança.
 *
 * @param sw Writer a destruir.
 */
void svg_writer_destruir(svg_writer_t *sw);

/**
 * @brief Desenha um retângulo na camada de marcações (sobre as quadras).
 *
 * @param sw           Writer de destino.
 * @param x            Coordenada X do canto superior esquerdo.
 * @param y            Coordenada Y do canto superior esquerdo.
 * @param w            Largura.
 * @param h            Altura.
 * @param fill         Cor de preenchimento (string CSS).
 * @param stroke       Cor da borda (string CSS).
 * @param stroke_width Espessura da borda.
 */
void svg_writer_retangulo(svg_writer_t *sw,
                          double x, double y, double w, double h,
                          const char *fill, const char *stroke,
                          double stroke_width);

/**
 * @brief Desenha um retângulo na camada base (abaixo das marcações).
 *
 * Use para as quadras do mapa, pois esta camada é escrita antes das marcações.
 *
 * @param sw           Writer de destino.
 * @param x            Coordenada X do canto superior esquerdo.
 * @param y            Coordenada Y do canto superior esquerdo.
 * @param w            Largura.
 * @param h            Altura.
 * @param fill         Cor de preenchimento (string CSS).
 * @param stroke       Cor da borda (string CSS).
 * @param stroke_width Espessura da borda.
 */
void svg_writer_retangulo_base(svg_writer_t *sw,
                               double x, double y, double w, double h,
                               const char *fill, const char *stroke,
                               double stroke_width);

/**
 * @brief Escreve texto na posição (@p x, @p y).
 *
 * @param sw        Writer de destino.
 * @param x         Coordenada X da âncora do texto.
 * @param y         Coordenada Y da âncora do texto.
 * @param texto     Conteúdo textual a escrever.
 * @param font_size Tamanho da fonte (ex.: "12px").
 * @param fill      Cor do texto (string CSS).
 */
void svg_writer_texto(svg_writer_t *sw, double x, double y,
                      const char *texto, const char *font_size,
                      const char *fill);

/**
 * @brief Marca uma posição com um X vermelho de comprimento @p tamanho.
 *
 * @param sw      Writer de destino.
 * @param x       Coordenada X do centro do X.
 * @param y       Coordenada Y do centro do X.
 * @param tamanho Metade do comprimento de cada linha diagonal.
 */
void svg_writer_x_vermelho(svg_writer_t *sw,
                           double x, double y, double tamanho);

/**
 * @brief Marca uma quadra removida com diagonais ligando vértices opostos.
 *
 * @param sw Writer de destino.
 * @param x  Coordenada X do canto superior esquerdo da quadra.
 * @param y  Coordenada Y do canto superior esquerdo da quadra.
 * @param w  Largura da quadra.
 * @param h  Altura da quadra.
 */
void svg_writer_x_quadra_removida(svg_writer_t *sw,
                                  double x, double y, double w, double h);

/**
 * @brief Desenha uma cruz vermelha (+) centrada em (@p x, @p y).
 *
 * @param sw      Writer de destino.
 * @param x       Coordenada X do centro da cruz.
 * @param y       Coordenada Y do centro da cruz.
 * @param tamanho Metade do comprimento de cada braço da cruz.
 */
void svg_writer_cruz_vermelha(svg_writer_t *sw,
                              double x, double y, double tamanho);

/**
 * @brief Desenha um círculo preto preenchido centrado em (@p x, @p y).
 *
 * @param sw   Writer de destino.
 * @param x    Coordenada X do centro.
 * @param y    Coordenada Y do centro.
 * @param raio Raio do círculo.
 */
void svg_writer_circulo_preto(svg_writer_t *sw,
                              double x, double y, double raio);

/**
 * @brief Desenha um quadrado vermelho com o CPF escrito dentro.
 *
 * @param sw   Writer de destino.
 * @param x    Coordenada X do canto superior esquerdo do quadrado.
 * @param y    Coordenada Y do canto superior esquerdo do quadrado.
 * @param lado Comprimento do lado do quadrado.
 * @param cpf  CPF a exibir dentro do quadrado.
 */
void svg_writer_quadrado_cpf(svg_writer_t *sw,
                             double x, double y, double lado,
                             const char *cpf);

/* ======================================================================== */
/* Extensões do Projeto 2 — grafo viário, registradores, componentes e MST  */
/* ======================================================================== */

/**
 * @brief Desenha um vértice do grafo como pequeno círculo lilás em (x, y).
 *
 * @param sw Writer de destino.
 * @param x  Coordenada X do vértice.
 * @param y  Coordenada Y do vértice.
 */
void svg_writer_vertice(svg_writer_t *sw, double x, double y);

/**
 * @brief Desenha uma aresta direcionada de (x1,y1) para (x2,y2) com seta no
 *        destino indicando o sentido do tráfego; opcionalmente rotula com @p nome.
 *
 * @param sw   Writer de destino.
 * @param x1   X da origem.
 * @param y1   Y da origem.
 * @param x2   X do destino (onde fica a ponta da seta).
 * @param y2   Y do destino.
 * @param nome Nome da rua a exibir no meio da aresta (pode ser NULL/"").
 */
void svg_writer_aresta(svg_writer_t *sw,
                       double x1, double y1, double x2, double y2,
                       const char *nome);

/**
 * @brief Desenha uma linha vertical pontilhada vermelha na posição @p x,
 *        cobrindo a altura atual do desenho, com @p label no topo.
 *
 * Usada pelo comando @c \@o? para marcar a coordenada de um endereço.
 *
 * @param sw    Writer de destino.
 * @param x     Posição X da linha.
 * @param label Texto exibido no topo (ex.: nome do registrador). Pode ser NULL.
 */
void svg_writer_linha_pontilhada_vertical(svg_writer_t *sw, double x,
                                          const char *label);

/**
 * @brief Desenha um retângulo semitransparente (bounding box) para o comando
 *        @c regs, delimitando uma componente conexa.
 *
 * @param sw        Writer de destino.
 * @param x         X do canto superior esquerdo.
 * @param y         Y do canto superior esquerdo.
 * @param w         Largura.
 * @param h         Altura.
 * @param cor       Cor de preenchimento (string CSS).
 * @param opacidade Opacidade do preenchimento em [0,1] (ex.: 0.5 = 50%).
 */
void svg_writer_bounding_box(svg_writer_t *sw,
                             double x, double y, double w, double h,
                             const char *cor, double opacidade);

/**
 * @brief Desenha uma aresta grossa (traço espesso) na cor @p cor, usada pelo
 *        comando @c exp para destacar arestas da AGM.
 *
 * @param sw  Writer de destino.
 * @param x1  X da origem.
 * @param y1  Y da origem.
 * @param x2  X do destino.
 * @param y2  Y do destino.
 * @param cor Cor do traço (string CSS; ex.: "red").
 */
void svg_writer_aresta_grossa(svg_writer_t *sw,
                              double x1, double y1, double x2, double y2,
                              const char *cor);

/**
 * @brief Desenha um percurso animado (comando @c p?): traça o caminho pelos
 *        pontos, anima um marcador ao longo dele com @c \<animateMotion\>, e
 *        rotula os extremos com "I" (início) e "F" (fim).
 *
 * @param sw      Writer de destino.
 * @param pontos  Array de coordenadas intercaladas [x0,y0,x1,y1,...] com 2*n valores.
 * @param n       Número de pontos (>= 2 para desenhar).
 * @param cor     Cor do traço e do marcador (string CSS).
 * @param id_path Identificador único do @c \<path\> (referenciado pelo @c \<mpath\>).
 */
void svg_writer_percurso_animado(svg_writer_t *sw,
                                 const double *pontos, int n,
                                 const char *cor, const char *id_path);

#endif // SVG_WRITER_H

# TileMap.cpp

`TileMap.cpp` define as funções declaradas e não definidas em
`TileMap.hpp`. Enquanto o `Dungeon2D` guarda a geometria (salas e
corredores), o `TileMap` é quem transforma essa geometria em uma
**matriz de tiles** de verdade, além de cuidar da persistência em CSV e
da tradução entre coordenadas do mundo e índices de célula.

A matriz é guardada como um `std::vector<int>` em ordem **row-major**,
ou seja, o índice de uma célula `(col, row)` é `row * width + col`, assim
como está combinado no `Global.hpp`. Cada célula é `EMPTY_TILE` (livre)
ou `WALL_TILE` (parede).

## carveRect e carveLeaves (auxiliares)

Antes das funções membro, há dois auxiliares dentro de um namespace
anônimo:

- `carveRect` escava um retângulo, ou seja, troca as células dele para
  `EMPTY_TILE`. Ele faz o *clamp* dos limites na matriz, então mesmo que
  um corredor passe um pouco da borda, nada é escrito fora do vetor.
- `carveLeaves` percorre a árvore BSP e, para cada folha, escava a sala
  dela usando o `carveRect`.

## TileMap::loadFromDungeon

Essa é a função que monta a matriz a partir de um `Dungeon2D`. O
processo é:

1. ajusta o tamanho da matriz para o tamanho da masmorra;
2. preenche **tudo** com `WALL_TILE`;
3. escava as salas (percorrendo as folhas da árvore com `carveLeaves`);
4. escava cada corredor da lista de corredores.

Ou seja, a lógica é "começa tudo parede e vai abrindo os espaços livres".

## Persistência em CSV

### TileMap::saveCSV

Salva a matriz em um arquivo, uma linha por linha da matriz, com os
inteiros separados por vírgula **sem vírgula no final**, na ordem
row-major. Retorna `true` se deu certo. Se o arquivo não abrir, retorna
`false` logo no começo.

### TileMap::loadCSV

Lê um arquivo no mesmo formato do `saveCSV`. Um cuidado importante aqui é
que a função lê tudo primeiro para uma estrutura temporária e só aplica
na matriz de verdade se o arquivo estiver todo correto. Assim, se der
algum erro no meio, o mapa continua como estava (isso é o que a
declaração promete).

Os erros que fazem a função rejeitar o arquivo são: campo que não é
número inteiro, arquivo vazio e matriz "irregular" (linhas com
quantidades diferentes de colunas).

## Consultas

As funções `width` e `height` só devolvem as dimensões guardadas.

A `at` devolve o valor do tile em `(col, row)`, usando um `assert` para
garantir que os índices estão dentro do intervalo, como as outras
pré-condições do projeto.

A `isWall` só reaproveita a `at` e compara o resultado com `WALL_TILE`.

A `data` devolve o ponteiro cru para a matriz (útil, por exemplo, para
quem for desenhar o mapa na tela).

## Tradução entre mundo e célula

### TileMap::toIndices

Converte uma posição contínua do mundo (em pixels) para o índice da
célula que a contém, usando `floor(coordenada / TILE_SIZE)` em cada eixo,
exatamente como está combinado no `Global.hpp`.

### TileMap::toWorld

Faz o caminho contrário: dado um índice de célula, devolve a coordenada
do **centro** daquela célula no mundo, que é
`((col + 0.5) * TILE_SIZE, (row + 0.5) * TILE_SIZE)`.

## TileMap::randomEmptyCell

Devolve os índices de uma célula `EMPTY_TILE` sorteada. Primeiro é
montada uma lista com os índices de todas as células livres, com um
`assert` para garantir que existe pelo menos uma (que é a pré-condição),
e depois uma posição dessa lista é sorteada. No fim o índice linear é
convertido de volta para `col` e `row` com resto e divisão por `width`.

# Dungeon2D.cpp

`Dungeon2D.cpp` define as funções que foram declaradas e não definidas
em `Dungeon2D.hpp`. A ideia da classe é gerar a **geometria** de uma
masmorra usando Particionamento Binário do Espaço (BSP), guardando o
resultado em uma árvore de salas e uma lista de corredores.

É importante lembrar que aqui tudo é medido em **tiles** (índices
inteiros de célula), não em pixels. A tarefa de transformar essa
geometria em uma matriz de tiles é do `TileMap`, não desse arquivo.

As restrições que a geração precisa respeitar são:

- corredores têm 3 tiles de largura;
- a menor sala possível é 5 x 5 tiles;
- todas as salas ficam conectadas entre si pelos corredores.

## Constantes

Para não espalhar números mágicos pelo código, algumas constantes são
definidas no começo do arquivo (dentro de um namespace anônimo, para ter 
linkagem interna, já que só interessam aqui):

- `ROOM_MIN` = 5, o menor lado de uma sala;
- `CORRIDOR_W` = 3, a largura do corredor;
- `LEAF_MIN` = `ROOM_MIN + 2` = 7, o menor tamanho de uma região que
  ainda vale a pena dividir. O `+ 2` existe para deixar uma margem de 1
  tile de cada lado da sala, assim a sala nunca encosta na borda da
  região;
- `MAX_DEPTH` = 5, um limite na profundidade da árvore para as salas não
  ficarem todas no tamanho mínimo.

## Construtor

O construtor só guarda a largura e a altura da masmorra (em tiles) e usa
um `assert` para garantir que as dimensões são positivas, igual é feito
nos outros arquivos do projeto para as pré-condições.

## Dungeon2D::generate

Essa é a função pública que faz todo o trabalho. Ela recebe uma `seed`
para o gerador de números pseudoaleatórios (`std::mt19937`), o que
garante que a **mesma seed sempre gera a mesma masmorra**.

O processo é dividido em três etapas, cada uma com uma função auxiliar:

1. `buildTree` — constrói a árvore BSP dividindo o espaço;
2. `carveRooms` — coloca uma sala dentro de cada folha da árvore;
3. `connect` — liga as salas com corredores.

Antes disso, a raiz da árvore é criada com a região cobrindo a masmorra
inteira: de `(0, 0)` até `(width - 1, height - 1)`.

### splitNode e buildTree

A `splitNode` é quem corta uma região em duas. Primeiro ela verifica se
dá para cortar na vertical ou na horizontal: só dá se o lado tiver pelo
menos `2 * LEAF_MIN`, porque assim as duas metades ainda ficam com no
mínimo `LEAF_MIN`. Se não der para nenhum dos dois, a função retorna
`false` e aquela região vira uma folha.

Quando dá para cortar nos dois sentidos, a função prefere cortar o lado
mais comprido (comparando largura e altura com uma folga de 25%). Isso
evita regiões muito espremidas. Se os lados forem parecidos, o sorteio
decide.

Escolhido o sentido, um ponto de corte é sorteado de forma que as duas
metades fiquem com pelo menos `LEAF_MIN`, e os dois filhos são criados
com suas respectivas regiões.

A `buildTree` só chama a `splitNode` recursivamente nos dois filhos,
parando quando a profundidade chega em `MAX_DEPTH` ou quando a região
não pode mais ser dividida.

### carveRooms

Essa função percorre a árvore e, para cada **folha**, desenha uma sala
dentro da região dela. Primeiro a região é encolhida em 1 tile de cada
lado (quando há espaço) para deixar a margem. Depois a largura e a altura
da sala são sorteadas, sempre entre `ROOM_MIN` e o espaço disponível, e
por fim a posição da sala dentro da região também é sorteada.

Como toda folha tem no mínimo `LEAF_MIN` (= 7), mesmo tirando a margem
sobra espaço de sobra para uma sala 5 x 5.

### roomCentre e addCorridor

A `roomCentre` devolve o centro (em tiles) de uma sala "representante" de
uma subárvore, sempre descendo pelo filho da esquerda até chegar numa
folha. Serve para saber a qual ponto ligar o corredor.

A `addCorridor` cria o caminho em formato de **L** entre dois pontos. O
corredor tem `CORRIDOR_W` de largura, então é usado um deslocamento de 1
tile (`CORRIDOR_W / 2`) para cada lado do eixo do corredor. São dois
retângulos: uma perna horizontal e uma perna vertical, que se sobrepõem
no ponto onde se encontram, garantindo que o caminho fique contínuo.

### connect

A `connect` percorre a árvore de baixo para cima e, para **cada nó
interno**, liga a sala representante do filho da esquerda com a do filho
da direita usando a `addCorridor`.

O detalhe importante é que isso garante a conectividade: como cada nó
interno conecta as suas duas metades, e isso acontece em todos os níveis
da árvore, no fim toda a masmorra vira um único espaço conectado.

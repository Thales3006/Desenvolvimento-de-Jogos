# Collision2D

Esse arquivo contém dois tipos de dados definidos: AABB e Collision2D.

## AABB

Essa struct representa uma das formas mais clássicas de bounding box: um
retângulo (definido por 2 pontos) alinhado aos eixos X e Y (não rotacionado).

### AABB::intersects(const AABB& other)

Esse método verifica se outra bounding box AABB intercepta `this`.
Para isso, a função verifica, para cada eixo, se os "intervalos" das AABBs
têm uma interseção.

Considerando A e B bounding boxes, temos alguns casos possíveis:

- {A------A}   {B======B} (A antes de B)
- {A--{B==A}==B}          (interseção)
- {B=={A--B}--A}          (interseção)

Vemos que o máximo de A precisa passar o mínimo de B para ser um candidato
à interseção. Para confirmá-la, o mínimo de A deve ser menor que o máximo de B.
Desse modo, podemos garantir que há uma interseção nesse eixo.

Fazemos isso para X e Y.

## Collision2D

Collision2D é uma struct que, pela sua definição no header, guarda o tamanho da
caixa de colisão sem posição, ou seja, pressupõe que a entidade dona da colisão
é dinâmica e muda de posição de forma constante.

### Collision2D::bounds(const Vector2D& position)

Por esse motivo, o método bounds necessita da posição como parâmetro.

A posição representa o ponto central da Collision2D, sendo o tamanho armazenado
(halfExtents) metade da extensão da largura e altura (óbvio pelo nome).
Ao subtrair e somar `halfExtents` com `position` em `Collision2D::bounds`,
criamos uma AABB com `position` exatamente no ponto central, como esperado.

 V- position-halfExtents
 #----------|
 |          |
 |    . position
 |          |
 |__________# <- position+halfExtents

Assim, o método retorna uma AABB que envolve a posição com
`2 * halfExtents` como largura e altura.

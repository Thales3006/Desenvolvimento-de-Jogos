# Transform2D.cpp

`Transform2D.cpp` define as funções de membro que foram declaradas e 
não definidas em `Transform.hpp`.

## Factories

Existem 3 funções que servem como factories (constroem objetos com razões 
mais específicas). Sendo elas: translation, rotation e scale.

### Transform2D::translation

A função membro `translation` serve como uma factory para criar
matrizes de translação que movem vetores por uma 
quantidade (`tx`, `ty`) de unidades, sendo a matriz resultante:

|    |    |   |
| -- | -- | - |
| 1  | 0  | 0 |
| 0  | 1  | 0 |
| tx | ty | 1 |

Isso porque matrizes NxN não permitem translações para N dimensões.
Ao adicionar uma terceira dimensão (para um espaço de 2 dimensões), 
é possível supor que os vetores têm um terceiro componente igual a 1 (constante),
ou seja, `(a, b, 1)` para qualquer vetor. 

Assim, a multiplicação da matriz com o vetor resultará em 
`(a + tx, b + ty, 1)`, transladando o vetor.

### Transform2D::rotation

A matriz rotação rotaciona um vetor **em torno do centro** por um ângulo x, 
sua fórmula é:

|        |       |   |
| ------ | ----- | - |
| cos x  | sin x | 0 |
| -sin x | cos x | 0 |
| 0      | 0     | 1 |

Assim, sendo `angle_rad` o ângulo de entrada e tendo o valor de `cos(angle_rad)` e `sin(angle_rad)`, basta atribuir os valores à matriz para criação. 

### Transform2D::scale

A função membro `scale` serve como uma factory de uma matriz de
transformação que escala um vetor nas suas 2 dimensões de
forma independente **em torno do centro**. Sendo a matriz:

|    |    |   |
| -- | -- | - |
| sx | 0  | 0 |
| 0  | sy | 0 |
| 0  | 0  | 1 |

Isso escala um vetor qualquer `(a, b)` para `(a * sx, b * sy)`.

## Operadores e métodos

Por mais que matrizes tenham mais operadores na matemática, para
computação gráfica e jogos as operações que envolvem multiplicação 
bastam.

### Transform2D::operator*

O overloading de `*` para a multiplicação de matrizes segue o mesmo
modo que a matemática, onde para cada item (i,j) na matriz resultante
é resultado da soma dos produtos dos componentes da linha i 
com a coluna j.

Isso é feito para cada (i, j) da nova matriz (2 for loops iniciais),
iterando sobre a linha i e coluna j para multiplicar os componentes 
(for loop mais interno) e os acumulando no componente (i,j) da 
matriz com `+=`.

Essa operação é muito importante para encadear transformações de matrizes.
Também é importante denotar que, do modo que foi implementado, a 
transformação da matriz à esquerda será aplicada primeiro para um vetor,
visto que a multiplicação de matrizes não é comutativa.  

### Transform2D::operator*=

O operador `*=` usa do `*`. 
Porém, ao invés de retornar um novo valor, ele atribui por cópia para
a matriz original, mudando seu conteúdo.

### Transform2D::transform_point

Aqui aplica-se a transformação com a translação no vetor `point`.
A multiplicação segue a mesma regra da multiplicação de matrizes,
mas por serem poucos componentes é possível escrever sem loops de iteração.

### Transform2D::transform_vector

Assim como na `transform_point` multiplicamos o vetor pela transformação,
porém não aplicamos a transformação (ignoramos a terceira dimensão hipotética).
Isso se torna útil para mudanças em direções (como o nome do parâmetro 
`const Vector2D& direction` sugere)  porque ajuda na performance por reduzir a quantidade de cálculos e instruções em momentos mais críticos de processamento.
Por exemplo: um carro que vira para a *direita* pode usar a mesma matriz de transformação para a posição do modelo do carro e sua direção.
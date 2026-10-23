# Vector2D.cpp

A implementação das funções do arquivo `Vector2D.cpp`
foi feita com auxílio de IA (Claude) para as funções de membro 
e operadores que foram apenas declaradas no header, 
porém que não foram definidas nele.

Sendo estas funções:

- Vector2D::normalized
- Vector2D::normalize
- Vector2D::equals
- Vector2D::operator+
- Vector2D::operator-
- Vector2D::operator* (vec * scalar)
- Vector2D::operator* (scalar * vec)
- Vector2D::operator+=
- Vector2D::operator-=
- Vector2D::operator*=
- Vector2D::operator/=

## Vector2D::normalized

O método normalized, como o nome diz, normaliza o vetor para ter
módulo = 1. Para tal, o vetor é dividido pela sua própria distância
(sendo a divisão definida posteriormente).
Ele retorna por valor uma cópia do vetor normalizado, ou seja,
não modifica o vetor original.

Assim como está escrito no comentário da declaração do membro,
é usado o `assert` para garantir que o vetor não é nulo 
(por se tratar de floats, usamos uma constante `EPSILON` ao invés 
de `!= 0` por causa do erro de aproximação dos pontos flutuantes).

## Vector2D::normalize

Esse método faz o mesmo que o `Vector2D::normalized`, porém ao invés
de retornar um novo vetor, ele modifica o próprio vetor que chamou o método.

## Vector2D::equals

Esse método compara se dois vetores são iguais com uma margem de 
erro/tolerância (tol), isso significa que se a diferença entre todos os
componentes do vetor estiver dentro do intervalo ]-tol, tol[ 
os vetores são considerados iguais.

## Vector2D::operator+ e Vector2D::operator-

O overloading dos operadores é feito para que a classe Vector2D 
aja como os vetores da álgebra linear. 

Dessa forma, a soma e subtração apenas somam e subtraem cada componente
de forma respectiva:
 
- soma: v + u = (v.x + u.x, v.y + u.y)
- subtração: v - u = (v.x - u.x, v.y - u.y)

Logicamente, vetores só podem ser somados com outros vetores.
Essas operações retornam um novo vetor, não alteram o original.

## Vector2D::operator* (vec * scalar) e Vector2D::operator/

A multiplicação e divisão seguem a mesma lógica, porém temos que
o vetor será multiplicado e dividido por um escalar. Esse escalar
multiplica/divide cada um dos componentes do vetor.

A ordem de multiplicação e divisão, por causa do objeto implícito
nas funções de membro, apenas permite `vector * scalar` e `vector / scaler`, sendo o vetor
sempre o primeiro na ordem.

## operator* (scalar * vec)

Como a multiplicação por escalar é comutativa, temos que definir o overloading fora da classe para permitir inverter essa ordem.
Assim, podemos usar o overloading previamente definido 
(`Vector2D::operator*`) para definir a multiplicação com a ordem dos
parâmetros invertida: `scalar * Vector`.

## Vector2D::operator+=, Vector2D::operator-=, Vector2D::operator*= e Vector2D::operator/=

Esses operadores modificam o vetor original aplicando as operações de soma,
subtração, multiplicação e divisão respectivamente que, por mais que não 
usem os operadores definidos anteriormente, se comportam de forma semelhante.

Sendo a única diferença a alteração direta do vetor e que não retornam por valor,
mas por referência.








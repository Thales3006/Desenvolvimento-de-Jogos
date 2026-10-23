# RigidBody2D

A struct `RigidBody2D` define um corpo (rígido) que compõe um ponto em
um espaço 2D com velocidade e aceleração.

### RigidBody2D::integrate(float dt)

Esse método apenas aplica a aceleração à velocidade e a velocidade à posição.
Isso pode ser visto como um tipo de integração, porque soma à velocidade
e posição de forma linear a cada chamada de função.

O mais interessante é o parâmetro `dt`, que tem como precondição que deve ser
maior que 0. O `dt` representa o tempo entre cada iteração da simulação
(`delta time`). Isso permite que programas rodando em máquinas ou em cenários
diferentes, que por consequência da diferença aumentam ou diminuem a frequência
em que cada iteração da simulação é executada, não afetem a execução.

Por exemplo: sejam uma máquina A com 60 FPS e uma máquina B com 30 FPS,
e um programa que simula um movimento de um corpo rígido em que a cada iteração
o `dt` é constantemente 1. A máquina A terminará na metade do tempo de B.
Com `dt` como parâmetro é possível passar um tempo mais aproximado do que
realmente passou.

Isso é feito de forma semi-implícita, em que a velocidade é atualizada
antes da posição, o que torna as simulações mais estáveis. Isso evita que objetos
atinjam velocidades exorbitantes por algum outlier (como um travamento no
computador).

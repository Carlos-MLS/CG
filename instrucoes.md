# Instruções - CG Trabalho Fase 1

## Compilar

```bash
cd build
cmake .. -B .
make
```

## Gerar primitivas

### Plano
```bash
./generator plane <comprimento> <divisões> <ficheiro>
```
Exemplo: plano 5x5 com 10 divisões por eixo
```bash
./generator plane 5 10 plane.3d
```

### Box
```bash
./generator box <largura> <altura> <profundidade> <divisões> <ficheiro>
```
Exemplo: caixa 2x3x4 com 5 divisões por aresta
```bash
./generator box 2 3 4 5 box.3d
```

### Esfera
```bash
./generator sphere <raio> <slices> <stacks> <ficheiro>
```
Exemplo: esfera de raio 1 com 20 slices e 20 stacks
```bash
./generator sphere 1 20 20 sphere.3d
```

### Cone
```bash
./generator cone <raio_base> <altura> <slices> <stacks> <ficheiro>
```
Exemplo: cone de raio 1, altura 2, com 20 slices e 10 stacks
```bash
./generator cone 1 2 20 10 cone.3d
```

## Visualizar com o engine

O engine lê um ficheiro XML que diz que modelos carregar e como posicionar a câmera.

```bash
./engine config.xml
```

### Controlos do engine

| Tecla | O que faz |
|-------|-----------|
| W/S | Rodar câmera cima/baixo |
| A/D | Rodar câmera esquerda/direita |
| Q/E | Zoom in / zoom out |
| C | Alternar câmera fixa/orbital |
| 1 | Modo sólido |
| 2 | Modo wireframe |
| 3 | Modo pontos |
| ESC | Sair |

### Exemplo de config.xml

```xml
<world>
    <window width="512" height="512" />
    <camera>
        <position x="3" y="2" z="1" />
        <lookAt x="0" y="0" z="0" />
        <up x="0" y="1" z="0" />
        <projection fov="60" near="1" far="1000" />
    </camera>
    <group>
        <models>
            <model file="sphere.3d" />
        </models>
    </group>
</world>
```

Para ver vários modelos ao mesmo tempo basta adicionar mais linhas `<model file="..." />` dentro de `<models>`.

## Testar tudo de uma vez

```bash
cd build
cmake .. -B . && make
./generator plane 5 10 plane.3d
./generator box 2 3 4 5 box.3d
./generator sphere 1 20 20 sphere.3d
./generator cone 1 2 20 10 cone.3d
./engine config.xml
```

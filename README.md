# Autor: Pedro Henrique Soares Rodrigues

# Model Loader

Um visualizador de modelos 3D em formato OBJ que permite manipular múltiplos objetos simultaneamente.

## Requisitos

- OpenGL 3.3 ou superior
- GLFW
- GLAD
- GLM
- Compilador C++ com suporte a C++11 ou superior

## Compilação

```bash
mkdir build
cd build
cmake ..
make
cd ..
```

## Uso

O programa aceita um ou mais arquivos OBJ como argumentos:

```bash
./build/src/ModelLoader <modelo1.obj> [modelo2.obj] [modelo3.obj] ...
```

### Controles

- **TAB**: Alternar entre objetos
- **T**: Modo de translação
  - Setas: Mover no plano XY
  - Page Up/Down: Mover no eixo Z
- **R**: Modo de rotação
  - X: Rotacionar no eixo X
  - Y: Rotacionar no eixo Y
  - Z: Rotacionar no eixo Z
- **S**: Modo de escala
  - Seta para cima: Aumentar escala
  - Seta para baixo: Diminuir escala
- **W**: Alternar modo wireframe
- **ESC**: Sair

### Exemplo

```bash
./build/src/ModelLoader ./assets/Modelos3D/Suzanne.obj ./assets/Modelos3D/Cube.obj
```

# Three Point Lighting

Um visualizador de modelos 3D em formato OBJ com três pontos de luz

## Requisitos

- OpenGL 3.3 ou superior
- GLFW
- GLAD
- GLM
- Compilador C++ com suporte a C++11 ou superior

## Compilação

```bash
mkdir build
cd build
cmake ..
make
cd ..
```

## Uso

O programa aceita um ou mais arquivos OBJ como argumentos:

```bash
./build/src/ThreePointLighting <modelo1.obj> [modelo2.obj] [modelo3.obj] ...
```

### Controles

- **TAB**: Alternar entre objetos
- **T**: Modo de translação
  - Setas: Mover no plano XY
  - Page Up/Down: Mover no eixo Z
- **R**: Modo de rotação
  - X: Rotacionar no eixo X
  - Y: Rotacionar no eixo Y
  - Z: Rotacionar no eixo Z
- **S**: Modo de escala
  - Seta para cima: Aumentar escala
  - Seta para baixo: Diminuir escala
- **W**: Alternar modo wireframe
- Controle dos 3 pontos de luz:

  - 1: Ligar/desligar Luz principal
  - 2: Ligar/desligar Luz de preenchimento
  - 3: Ligar/desligar Luz de fundo

- **ESC**: Sair

### Exemplo

```bash
./build/src/ThreePointLighting ./assets/Modelos3D/Suzanne.obj
```

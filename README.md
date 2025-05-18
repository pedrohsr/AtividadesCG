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
```

## Uso

O programa aceita um ou mais arquivos OBJ como argumentos:

```bash
./src/modelLoader <modelo1.obj> [modelo2.obj] [modelo3.obj] ...
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
./src/modelLoader assets/Modelos3D/Suzanne.obj assets/Modelos3D/Cube.obj
```

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

# Trabalho final

Um visualizador de modelos 3D

## Requisitos

- OpenGL 3.3 ou superior
- nlohmann/json
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
./build/src/SceneViewer scene_config.json
```

### Controles

**Controle de Câmera:**
- **WASD**: Mover câmera
- **Mouse**: Olhar ao redor
- **Space/Ctrl**: Mover para cima/baixo

**Controle de Objetos:**
- **TAB**: Alternar entre objetos
- **T/R/G**: Modo de translação/rotação/escala
- **Z/X/Y**: Rotacionar objeto selecionado
- **Setas**: Transformar objeto selecionado
- **Page Up/Down**: Mover objeto para frente/trás
- **C**: Alternar modo de adicionar pontos de controle
- **SPACE**: Adicionar ponto de controle (quando no modo de adicionar)
- **M**: Alternar movimento de trajetória
- **X**: Limpar trajetória
- **O**: Teste de rotação (45° eixo Y)

**Controles de Luz:**
- **L**: Alternar entre luzes
- **Numpad 4/6**: Mover luz esquerda/direita
- **Numpad 2/8**: Mover luz baixo/cima
- **Numpad 7/9**: Mover luz frente/trás
- **Numpad +/-**: Ajustar intensidade da luz

**Controles de Visualização:**
- **F**: Alternar modo wireframe
- **V**: Alternar visualização de trajetórias

**Operações de Arquivos:**
- **F1**: Salvar configuração da cena (JSON)
- **F2**: Carregar configuração da cena (JSON)
- **ESC**: Sair

### Exemplo

```bash
./build/src/ThreePointLighting ./assets/Modelos3D/Suzanne.obj
```
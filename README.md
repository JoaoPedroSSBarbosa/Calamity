# Calamity - Dungeon Crawler em C com raylib

Este projeto agora � um jogo de explora��o de masmorra implementado com **raylib** em C.

## Recursos implementados

- `structs` (`Player`, `Enemy`, `Item`, `GameState`)
- `malloc` / `free` para mapa e listas encadeadas
- listas encadeadas para inimigos e itens
- matriz de caracteres para o mapa da masmorra
- anima��o e movimento com raylib
- colis�o com paredes, armadilhas e inimigos
- interação com itens: tesouro, chave, alavanca, poção de vida e melhoria de ataque
- leitura e escrita de arquivo para salvar o high score

## Como compilar

### Linux / MacOS

```bash
gcc main.c game.c -o calamity -lraylib
```

### Windows (MSYS2 MinGW)

Instale raylib com o pacman e compile:

```bash
pacman -S mingw-w64-x86_64-raylib
gcc main.c game.c -o calamity.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

Se voc� usa `pkg-config`:

```bash
gcc main.c game.c -o calamity $(pkg-config --cflags --libs raylib)
```

## Como jogar

Execute o bin�rio gerado:

```bash
./calamity
```

Controles:
- `W` / `Seta para cima`: subir
- `S` / `Seta para baixo`: descer
- `A` / `Seta para esquerda`: mover para esquerda
- `D` / `Seta para direita`: mover para direita
- `Espa�o`: atacar inimigo adjacente
- `R`: reiniciar ap�s game over

## Observa��es

- O jogo exibe o mapa da masmorra com inimigos, itens e portas.
- O `highscore` � salvo em `highscore.txt`.

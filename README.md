# Calamity - Dungeon Crawler em C

Esse projeto é um jogo do gênero Dungeon Crawler feito apenas em C que roda pelo terminal!

## Recursos implementados

- structs (`Player`, `Enemy`, `Item`, `GameState`)
- ponteiros
- alocação dinâmina de memória
- listas encadeadas para inimigos e itens
- matriz de caracteres para o mapa da masmorra
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

## Como jogar

Execute o binário gerado:

```bash
./calamity
```

Controles:
- `W` / `Seta para cima`: subir
- `S` / `Seta para baixo`: descer
- `A` / `Seta para esquerda`: mover para esquerda
- `D` / `Seta para direita`: mover para direita
- `Espaço`: atacar inimigo adjacente
- `R`: reiniciar após game over

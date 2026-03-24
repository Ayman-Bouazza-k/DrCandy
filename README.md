# Práctica de Programación 2 - UAB, Grado Ingeniería Informática, 107890

# Ayuda primera ejecución

Instrucciones para una primera prueba de humo. Sistemas testados:

* Linux
* Windows
* MacOS

Todas las rutas relativas a la carpeta raíz del proyecto.

## Windows/Mac

1. Instalar Visual Studio community.
2. Abrir el fichero `visual_studio/0.\ Windows\ Desktop/P2_Lab.sln` con Visual Studio.
5. Pulsar el boton play de la barra superior (también disponible el script `run.bat` una vez compilado el proyecto).

El ejecutable se genera en `visual_studio\0. Windows Desktop\Program\P2_Lab.exe`.
Se recomiendo usar PowerShell para ejecutarlo y usar la entrada/salida por línea de comandos (`std::cin`/`std::cout`).

## Ubuntu/Debian

1. `sudo apt install build-essential cmake libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-image-dev libsdl2-gfx-dev libpng-dev libfreetype-dev`
2. make clean; make
3. `bash run.sh`

El binario se genera en `./build/p2_project` y puede ejecutarse directamente.

## Fedora

1. `sudo dnf install gcc gcc-c++ make cmake libsdl2-devel libsdl2-ttf-devel libsdl2-mixer-devel libsdl2-image-devel libsdl2-gfx-devel libpng-devel libfreetype-devel`
2. make clean; make
3. `bash run.sh`

El binario se genera en `./build/p2_project` y puede ejecutarse directamente.

## MacOS

### Guía rápida: compilar el proyecto en macOS con SDL2

Esta guía explica cómo configurar el proyecto para que compile en macOS usando Homebrew y SDL2 (y extensiones) con CMake o, opcionalmente, con un Makefile simple.

---

### 1. Instalar las librerías con Homebrew

En macOS (Apple Silicon o Intel) instala las dependencias con:

```bash
brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer libpng pkg-config
```

Comprobaciones rápidas (opcionales):

```bash
ls /opt/homebrew/include/SDL2        # headers de SDL2
ls /opt/homebrew/lib | grep SDL2    # libs de SDL2 y extensiones
```

En Intel es posible que Homebrew use `/usr/local` en lugar de `/opt/homebrew`. Ajusta las rutas si es tu caso.

---

### 2. Configurar CMake para macOS + Homebrew

Un `CMakeLists.txt` mínimo compatible con macOS/Homebrew podría tener esta estructura:

```cmake
cmake_minimum_required(VERSION 3.10)

project(p2_project)

set(CMAKE_CXX_STANDARD 20)

add_executable(p2_project)

# Compilación con warnings y debug
target_compile_options(p2_project PRIVATE -Wextra -g3 -O0)

# Directorios de include del proyecto
include_directories("src/")
include_directories("src/private/sdl_wrapper")
include_directories("src/private/extlibs")
include_directories("test/")

# Headers de Homebrew (SDL2, SDL2_image, SDL2_ttf, libpng...)
include_directories(/opt/homebrew/include)

# Fuentes del proyecto
file(GLOB_RECURSE files
        "src/*.cpp"
        "test/*.cpp"
)
target_sources(p2_project PRIVATE ${files})

# SDL2 core (usa el config de CMake/Homebrew)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Directorios de librerías de Homebrew (dylibs en /opt/homebrew/lib)
target_link_directories(p2_project PRIVATE /opt/homebrew/lib)

# Enlazar contra SDL2 + extensiones + png
target_link_libraries(p2_project PRIVATE
        ${SDL2_LIBRARIES}
        SDL2_image
        SDL2_ttf
        SDL2_mixer
        SDL2main
        png
        png16
)
```

Puntos clave:

- **Headers en macOS (Homebrew)**:
  - `#include <SDL2/SDL.h>`
  - `#include <SDL2/SDL_image.h>`
  - `#include <SDL2/SDL_ttf.h>`
- **Librerías** en `/opt/homebrew/lib`:
  - `libSDL2.dylib`, `libSDL2_image.dylib`, `libSDL2_ttf.dylib`, `libSDL2_mixer.dylib`, etc.
- `find_package(SDL2 REQUIRED)` proporciona:
  - `SDL2_INCLUDE_DIRS`
  - `SDL2_LIBRARIES`
- `target_link_directories(p2_project PRIVATE /opt/homebrew/lib)` indica al linker dónde buscar `-lSDL2_image`, `-lSDL2_ttf`, etc.

Si tu `brew --prefix` no es `/opt/homebrew`, sustituye esa ruta por la que devuelva:

```bash
brew --prefix
```

---

### 3. Ajustar includes en el código para macOS

Para que el mismo código funcione en Windows, Linux y macOS, es útil usar directivas de preprocesador:

```cpp
#if defined(__APPLE__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#else
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#endif
```

Así:

- En macOS/Homebrew se usarán `<SDL2/...>`.
- En otros sistemas donde SDL se instala como `<SDL.h>` funciona sin cambios.

Haz lo mismo en cualquier fichero que incluya SDL (por ejemplo `keyboard.h`, `sprites.h`, `video.cpp`, `graphics.cpp`, etc.).

---

### 4. Alternativa: Makefile sencillo con `pkg-config`

Si quieres compilar sin CMake, puedes usar un `Makefile` apoyado en `pkg-config`:

```make
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g

# Flags de SDL2, SDL2_image y SDL2_ttf obtenidos con pkg-config
CXXFLAGS += $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)
LDFLAGS  = $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf) -lpng

SRC = $(wildcard src/*.cpp src/private/sdl_wrapper/*.cpp src/private/sdl_wrapper/NFont/*.cpp test/*.cpp)
OBJ = $(SRC:.cpp=.o)

p2_project: $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) p2_project
```

Requisitos previos:

```bash
brew install pkg-config
```

`pkg-config` se encarga de devolver los flags adecuados (`-I` y `-L -l...`) en función de dónde Homebrew haya instalado las librerías.

---

### 5. Resumen rápido

- **Instala** con Homebrew: `sdl2`, `sdl2_image`, `sdl2_ttf`, `sdl2_mixer`, `libpng`, `pkg-config`.
- **Includes**:
  - macOS: `<SDL2/...>` y `<png.h>` (vía `/opt/homebrew/include`).
- **CMake**:
  - `include_directories(/opt/homebrew/include)`
  - `target_link_directories(p2_project PRIVATE /opt/homebrew/lib)`
  - `target_link_libraries(... SDL2_image SDL2_ttf SDL2_mixer SDL2main png png16)`
- **Código**:
  - Usa `#if defined(__APPLE__)` para seleccionar `<SDL2/...>` en macOS y `<SDL.h>` en el resto.

Con esto el proyecto queda preparado para compilar y ejecutarse correctamente en macOS usando la instalación estándar de SDL2 con Homebrew.


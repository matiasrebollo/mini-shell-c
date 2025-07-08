# Mini Shell en C

Este proyecto es una implementación de una **shell mínima** desarrollada en **C11** bajo el estándar **POSIX.1-2008**, como parte del trabajo práctico de la materia **Sistemas Operativos** (FIUBA, 2C2024 – cátedra Méndez-Fresia).

La shell soporta:

- Ejecución de comandos externos con argumentos
- Redirección de entrada/salida estándar y de errores
- Pipes simples y múltiples
- Variables de entorno y pseudo-variables (`$?`)
- Variables de entorno temporarias
- Comandos built-in (`cd`, `pwd`, `exit`)
- Ejecución de procesos en segundo plano

---

## Compilación

```bash
make
```

## Ejecución

```bash
./shell
```
Se mostrará un prompt desde donde se pueden ingresar comandos como en Bash.

---

## Funcionalidades implementadas

### Parte 1: Invocación de comandos
- Búsqueda en `$PATH`
- Comandos con o sin argumentos

### Parte 2: Redirecciones y Pipes
- Redirección de entrada: `< archivo.txt`
- Redirección de salida: `> archivo.txt`
- Redirección de errores: `2> error.txt`
- Redirección combinada: `2>&1`
- Pipes entre comandos: `cmd1 | cmd2 | cmd3`

### Parte 3: Variables
- Expansión de variables de entorno (`$HOME`, `$PATH`)
- Variables que no existen → reemplazo por string vacío
- Variables de entorno temporarias: `VAR=valor comando`
- Pseudo-variable `$?` que almacena el último código de salida

### Parte 4: Built-ins
- `cd`: cambia el directorio actual
- `pwd`: imprime el directorio actual
- `exit`: finaliza la shell

### Parte 5: Procesos en segundo plano
- Comandos terminados en `&` se ejecutan en background
- Se informa al usuario cuando terminan
- Manejo correcto de `wait` y señales

---

## Estándares y herramientas utilizadas

- **Lenguaje:** C11  
- **Estándar:** POSIX.1-2008  
- **Compilación:** `gcc` y `make`

Uso intensivo de:

- `fork()`, `execvp()`, `waitpid()`
- `dup2()`, `pipe()`, `open()`
- Manejo de señales: `SIGCHLD`, `SIGINT`
- Parsing manual con `strtok` y manipulación de strings

Trabajo práctico grupal realizado en la materia Sistemas Operativos (FIUBA), Cátedra Méndez-Fresia.

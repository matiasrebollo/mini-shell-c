# Mini Shell en C

Este proyecto es una implementación de una **shell mínima** desarrollada en **C11** bajo el estándar **POSIX.1-2008**.
Trabajo práctico de la materia **Sistemas Operativos** (FIUBA) – Grupo 25 – Cátedra Méndez-Fresia.

Integrantes:

- Agustín García Dresch
- Matías Gabriel Rebollo
- Marcos García Neira
- Sabrina García Lucentini

---

La shell soporta:

- Ejecución de comandos externos con argumentos
- Redirección de entrada/salida estándar y de errores
- Pipes simples y múltiples
- Variables de entorno y pseudo-variables (`$?`)
- Variables de entorno temporarias
- Comandos built-in (`cd`, `pwd`, `exit`)
- Ejecución de procesos en segundo plano

---

## Compilar

```bash
make
```

## Pruebas

- Ejecutar todas las pruebas

```bash
make test
```

- Ejecutar una **única** prueba

```bash
make test-TEST_NAME
```

Por ejemplo:

```bash
make test-env_empty_variable
```

Cada identificador de una prueba se muestra entre paréntesis `(TEST_NAME)` al lado de cada _test_ cuando se ejecutan todas las pruebas.

```
=== Temporary files will be stored in: /tmp/tmp0l10br1k-shell-test ===

PASS 1/26: cd . and cd .. work correctly by checking pwd (no prompt) (cd_back)
PASS 2/26: cd works correctly by checking pwd (no prompt) (cd_basic)
PASS 3/26: cd with no arguments takes you home (/proc/sys :D) (cd_home)
PASS 4/26: empty variables are not substituted (env_empty_variable)
...
```

## Ejecutar

```bash
./sh
```

## Linter

```bash
make format
```

Para efectivamente subir los cambios producidos por `make format`, hay que hacer `git add .` y `git commit`.

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

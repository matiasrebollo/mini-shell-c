# shell

### Búsqueda en $PATH

La familia exec(3) consiste en varias funciones de alto nivel que envuelven a la syscall execve(). La syscall execve(2) toma tres argumentos: el pathname del programa a ejecutar, un array de argumentos (argv[]) y un array de variables de entorno (envp[]). Las otras funciones de la familia exec(3) permiten especificar el programa usando solo el nombre del archivo buscandolo en el PATH. También ofrecen variaciones en la forma en que se pasan los argumentos, como una lista de argumentos (execl, execlp, execle), un array de punteros a strings terminados en NULL (execv, execvp), o permitiendo especificar un entorno personalizado para el nuevo programa (execle, execvpe).

**¿Puede la llamada a exec(3) fallar? ¿Cómo se comporta la implementación de la shell en ese caso?**
Si, la llamada puede fallar por errores en el path del archivo, por que el archivo ya esta siendo usado por otro poceso o por espacio insuficiente para los argumentos. Cuando exec(3) falla devuelve -1, la shell imprime un mensaje de error, cambia el status a failure, interrumpe el proceso hijo actual y se queda esperando un nuevo comando.

### Procesos en segundo plano

**Los procesos en segundo plano (background processes)** permiten la ejecución de comandos sin que el usuario tenga que esperar a que finalicen, devolviendo el prompt de la Shell inmediatamente para continuar con otras tareas. Para gestionar estos procesos, es necesario capturar la señal **SIGCHLD**, que es generada cada vez que un proceso hijo termina. Para capturar dicha señal, se utiliza un handler, configurado con la syscall *sigaction()*.

Sin embargo, **todos** los procesos hijos generan la señal, no solo los que se ejecutan en segundo plano, por lo que es necesario implementar un mecanismo que diferencie entre procesos de primer plano y de segundo plano. Esto se puede lograr asegurando que todos los procesos en segundo plano compartan el mismo **Process Group ID (PGID)**. Luego, dentro del handler, el proceso padre puede hacer *waitpid()* solo por los procesos en segundo plano.

**Explicación detallada:**

1) **Configuración del handler:** Se configura un stack alternativo con la syscall *sigaltstack()*, donde se ejecutará el handler. Esto es crucial, ya que mejora la seguridad y estabilidad al manejar operaciones que podrían ser interrumpidas por señales. Posteriormente, se configura el handler personalizado con *sigaction()* para capturar la señal **SIGCHLD**.

2) **Uso del handler:** El handler se activa cuando un proceso hijo termina y emite la señal SIGCHLD. Dentro del handler, se utiliza *waitpid(0, &status, WNOHANG)* para recoger todos los procesos hijos (de segundo plano) que hayan terminado. El primer argumento 0 en *waitpid()* indica que solo se espera por los procesos que comparten el mismo PGID que el del proceso que invoca *waitpid()* (en este caso, la Shell). Esto asegura que se recojan únicamente los procesos en segundo plano. Finalmente, se imprime el **PID** y el estado de salida del proceso.

3) **Ejecución de comandos:** Al ejecutar un comando, primero se determina si debe ejecutarse en primer o segundo plano. Si el comando es un proceso en primer plano, se cambia el **Process Group ID** del proceso hijo a su propio **PID** utilizando *setpgid(0, 0)*. Esto garantiza que los procesos de primer plano tengan un **PGID** distinto. En cambio, todos los procesos en segundo plano mantienen el mismo **PGID** que la Shell, permitiendo que el *waitpid()* descrito en el punto 2 funcione correctamente. Una vez configurado, el comando en segundo plano se ejecuta sin esperar su finalización, permitiendo continuar con otras tareas.

# **¿Por qué es necesario el uso de señales?**

1) **Comunicación entre procesos:** Las señales permiten la comunicación indirecta entre procesos. Por ejemplo, cuando un proceso hijo finaliza, envía una señal **SIGCHLD** al proceso padre para notificarle que debe recoger su estado. Además, un proceso padre puede enviar señales a un hijo para controlarlo, como suspender su ejecución con **SIGSTOP** o finalizarlo con **SIGKILL**.

2) **Interrupción de procesos:** Los usuarios pueden interrumpir la ejecución de un proceso con señales como **SIGINT**, enviadas al presionar **Ctrl+C**, lo que permite detener comandos de forma inmediata.

3) **Manejo de eventos asincrónicos:** Las señales permiten que los procesos respondan a eventos inesperados sin necesidad de tener que ser ellos quienes consulten repetidamente para verificar si ha ocurrido, evitando así el consumo innecesario de recursos. Por ejemplo, cuando llegan nuevos datos, el sistema puede enviar una señal para que el proceso gestione el evento inmediatamente.

4) **Seguridad y estabilidad**: Las señales pueden ayudar a que los procesos se comporten de manera segura y estable. Por ejemplo, el manejo de señales puede prevenir la ejecución de operaciones no deseadas si un proceso se encuentra en un estado inesperado. También permite al sistema gestionar que los procesos no se queden bloqueados o en estado de espera indefinida.


### Flujo estándar

**Investigar el significado de 2>&1, explicar cómo funciona su forma general**

Lo que hace 2>&1 es redirigir la salida de errores al mismo lugar que la salida estándar del programa. Está escrito de esta forma pues, 2> es como se redirecciona normalmente la salida de errores del programa, y si queremos que vaya al mismo lugar que la salida estándar, podemos pensar que alcanza con poner 2>1. Pero de esta forma, lo que se está haciendo es redirigir la salida de errores a un archivo llamado "1", entonces agregamos el & para indicar que nos referimos a el file descriptor de la salida estándar y no a un archivo llamado 1.

**Mostrar qué sucede con la salida de cat out.txt en el ejemplo.**
$ ls -C /home /noexiste >out.txt 2>&1

$ cat out.txt
---????---

En este caso, la salida resultante es:

ls: /noexiste: No such file or directory
/home:

La primera línea corresponde al error del directorio no existente, mientras que la segunda linea es la salida estándar hasta el error. Este comportamiento es igual en la shell implementada por nosotros y en una shell bash.

Si invertimos el orden de las redirecciones, en la shell implementada por nosotros no cambia el comportamiento, mientras que en una shell bash, no se realiza la redirección de errores, pues cuando se realiza el primer comando, por pantalla se imprime la linea:

ls: /noexiste: No such file or directory

Mientras que luego del comando: 'cat out.txt' se imprime lo siguiente:

/home:

Esto implica que no se redirigió la salida de errores al archivo out.txt.

### Tuberías múltiples

Cuando se ejecuta un pipe, cada proceso ejecutado reporta a la shell su exit code. Estos códigos se guardan en una variable de entorno que en el caso de bash se llama PIPESTATUS. Esta variable es una array cuyo índice comienza en 0 y están almacenados los exit code de los comandos ejecutados en el orden en el que fueron escritos en la ejecución del pipe. Por ejemplo:

$ false | true

Si queremos ver los exit code de los comandos, ingresamos el siguiente comando:

$ echo "${PIPESTATUS[0]} ${PIPESTATUS[1]}"

Lo que nos devuelve:

1 0

Los códigos reportados por cada comando no influyen en el proceso de ejecución de los otros comandos (influirán cuando un comando espera input de otro comando fallido, pero no fallará la ejecución del comando como tal).

Debido a que bash ejecuta cada comando del pipeline en su propia "subshell", el pipe entero puede ser ejecutado aunque uno de los comandos falle o no exista. Bash no chequea que todos los comandos puedan ser ejecutados, eso se lo delega a las subshells. Es por eso que ejecuciones como:

% noexiste | echo 2

Va a devolver:

-bash: noexiste: command not found
2

El comando no existente devuelve error pero echo se ejecutará igual. Otro ejemplo:

% noexiste|wc
-bash: noexiste : command not found
       0       0       0

El comando wc se ejecutará pero sin ningún input, por lo que cuenta 0 bytes.
Por último, probamos el pipeline:

% ls -l | noexiste | wc

El cual devuelve:

-bash: noexiste: command not found
       0       0       0

Lo que sucede es que el no se ve por pantalla la salida del primer comando, pues fue enviada a la entrada estándar de un proceso que ejecutó el comando que no existe, pero el comando ls -l fue ejecutado. Y nuevamente wc cuenta 0 bytes pues no recibe ningún input.

Ahora vamos a comparar los mismos ejemplos con nuestra implementación.

% noexiste | echo 2

Va a devolver:

Error ejecutando execvp 
: No such file or directory
2

El funcionaimento es el mismo, devuelve el mensaje de error para el comando no existente y ejecuta el comando que si existe.

Ahora probamos con el pipe:

% noexiste|wc
Error ejecutando execvp 
: No such file or directory
       0       0       0

Lo mismo de vuelta, se muestra el mensaje de error del comando no existente y se ejecuta el comando wc sin ningún imput, resultando en 0 bytes contados.

Por último:

% ls -l | noexiste | wc

Error ejecutando execvp 
: No such file or directory
       0       0       0

Volvió a suceder lo que pasó anteriormente. Se ejecuta el primer comando y su salida se manda a un proceso que intenta ejecutar un comando inexistente, el cual devuelve un error mientras que wc se ejecuta sin ningún input.

### Variables de entorno temporarias

**¿Por qué es necesario hacerlo luego de la llamada a fork(2)?**

Es necesario hacerlo luego de la llamada a fork(2) ya que queremos que las variables de entorno solo se definan en el entorno del proceso hijo y no en el del padre. Si se hiciera antes de la llamada a fork(2), las variables se definirían en el proceso padre y el luego el proceso hijo las heredaría, pero esto no es deseable en nuestro caso dado que las variables existirían en el proceso de la shell y no serían temporarias.

En algunos de los wrappers de la familia de funciones de exec(3) (las que finalizan con la letra e), se les puede pasar un tercer argumento (o una lista de argumentos dependiendo del caso), con nuevas variables de entorno para la ejecución de ese proceso. Supongamos, entonces, que en vez de utilizar setenv(3) por cada una de las variables, se guardan en un arreglo y se lo coloca en el tercer argumento de una de las funciones de exec(3). **¿El comportamiento resultante es el mismo que en el primer caso?**

No, el comportamiento resultante no es exactamente el mismo. En el primer caso, utilizando setenv(3) por cada una de las variables, se setean las nuevas variables al entorno adicionalmente a las ya existentes. En cambio, en el segundo caso, se ejecuta el proceso con un entorno nuevo y se le agregan las variables pasadas por parámetro.

**Descripción breve de una posible implementación para que el comportamiento sea el mismo**

Para que el comportamiento sea el mismo, se podría hacer el exec(3) (de la familia con e) pasandole como tercer argumento un arreglo de punteros char con las variables de entorno preexistentes junto con las nuevas variables. Estos punteros deberán apuntar a una cadena de caracteres con el formtato "key=value", y el arreglo deberá terminar en NULL.

### Pseudo-variables

La variable mágica ? devuelve el termination status del último comando ejecutado. Este valdrá 0 si el comando ejecutado fue exitoso, y diferente a 0 en caso de haya habido algún error.

**Otras variables mágicas estándar**

* $$ Devuelve el pid del proceso actual

>$ echo $$ \
>   10118

* $_ Devuelve el último argumento del último comando que se ejecutó

>$ echo Hola Mundo \
>$ echo $_ \
>   Mundo

* $0 Devuelve el nombre del script o comando que se está ejecutando

>$ echo $0 \
>   bash

### Comandos built-in

Los comandos built-in son especiales porque se ejecutan directamente dentro del proceso de la shell, lo que le da la habilidad de ejecutarse rapidamente y de poder acceder al estado de la shell. 
El comando cd necesariamente tiene que ser built-in para poder acceder al directorio actual y actualizarlo, si se ejecutara dentro de un proceso hijo no podria actualizar el estado de la shell y no funcionaria correctamente.
En cambio el comando pwd no tiene que hacer ningun cambio en el directorio actual, solo imprimirlo por pantalla, por lo que se podria implementarlo sin ser built-in por fuera del proceso de la shell. Igualmente la razon por la que se implementa ese comando como built-in es para facilitar el rapido acceso al estado de la shell 

### Historial

---

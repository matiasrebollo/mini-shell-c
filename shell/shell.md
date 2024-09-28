# shell

### Búsqueda en $PATH

---

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

---

### Tuberías múltiples

---

### Variables de entorno temporarias

---

### Pseudo-variables

---

### Comandos built-in

---

### Historial

---

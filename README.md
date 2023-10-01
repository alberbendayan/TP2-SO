<h1 align="center">Trabajo Práctico Especial</h1>
<h3 align="center">Arquitectura de Computadoras - 2023</h3>

<p align="center">
  <a href="https://bitbucket.org/RowDaBoat/x64barebones/">
    <img src="./Extras/x64BareBones.png" style="width:15%" alt="x64BareBones Logo"></img><br><br>
  </a>
  <a href="https://bitbucket.org/RowDaBoat/x64barebones/">x64BareBones</a>
</p>

## Tabla de contenidos

* [Requisitos](#requisitos)
* [Preparando el entorno](#preparando-el-entorno)
  * [Agregar mi usuario al grupo de docker](#agregar-mi-usuario-al-grupo-de-docker)
  * [Iniciar el daemon de docker](#iniciar-el-daemon-de-docker)
* [Compilando](#compilando)
* [Debugging](#debugging)

## Requisitos

Los requisitos necesarios para correr el proyecto son los siguientes:

* `docker`: para emular la compilación en una distribución de Ubuntu/Debian
* `qemu`: permite tomar la imagen compilada y emular un sistema

Desde _Arch Linux_ el procedimiento de instalación es sencillo:

```bash
sudo pacman -S qemu-system-x86 docker
```

## Preparando el entorno

Una vez instalados los requisitos se deben realizar unas tareas antes de poder compilar el proyecto.

### Agregar mi usuario al grupo de docker

Esto permite correr los contenedores, sin necesidad de tener privilegios de administrador.

```bash
sudo groupadd docker
sudo usermod -aG docker $USER
```

### Iniciar el daemon de docker

Con permisos de administrador correr el siguiente comando:

```bash
sudo setsid -f dockerd >/dev/null 2>&1
```

Con docker andando ya se pueden correr todos los comandos relacionados con docker. El comando `./compile.sh` permitirá realizar el resto del trabajo.

## Compilando

Listo! El entorno ya está preparado para compilar el proyecto y correrlo.

El comando `./compile.sh` realizará las siguientes tareas:

* Descargará la imágen de `agodio/itba-so:1.0` de docker donde se compilará el proyecto.
* Creará un nuevo contenedor preparado para compilar el proyecto como si fuéramos nosotros porque crea un usuario con nuestro mismo **uid** y **gid**.
* Iniciará el contenedor para que siempre esté corriendo.
* Enviará los comandos necesarios para limpiar y compilar el proyecto completo.

Esa es la funcionalidad base del comando `./compile.sh`. A su vez, cuenta con dos flags que pueden ser enviados:

* `-d`: Compila el proyecto con información de debugging para poder utilizar gdb y ejecutar paso a paso el código.
* `-r`: Una vez compilado el proyecto completo, si no ocurrió ningún error durante el proceso, se ejecuta el comando `./run.sh`, es decir, una vez compilado el proyecto, lo ejecuta.

> **Nota:** Si se corre el comando con ambos flags en simultáneo, es decir, `./compile.sh -d -r`, el proyecto se compilará y ejecutará en modo debugging. Si solo se utiliza el flag `-r`, se compilará en modo normal y se ejecutará en modo normal, sin información de debugging.

## Debugging

Para poder ejecutar el código paso a paso debe utilizarse el programa `gdb`.

Para esto hay un archivo de configuración `.gdbinit` que debe copiarse el home del usuario para poder configurarlo apropiadamente.

```sh
cp .gdbinit $HOME
```

Una vez realizado esto, para poder funcionarlo se utilizarán dos terminales:

1. En una se ejecutará el programa de compilado y ejecución: `./compile.sh -d -r`
2. En la otra se ejecutará el `gdb` que se conectará al `qemu` y podrá debuguearse el código.

El `gdb` estará configurado con dos subcomandos para facilitar el debugueo de código en **ASM** y en **C**:

* `src-prof` configura el `gdb` en modo de debugueo para código **C**.
* `asm-prof` lo mismo pero para código en **ASM**.

> **Nota:** Se puede intercalar en los modos en cualquier momento.

Una vez que está corriendo el `qemu` este se quedará esperando a que se conecte el `gdb`.

Para eso correr el comando `gdb`. Una vez abierto si se corre `c` o `continue` correrá el programa de forma normal.

Si se quiere hacer un debugueo _step by step_ puede ponerse un breakpoint con `b` o `br` en alguna parte del código y ejecutar `c` para que salte hasta ese punto.

```gdb
> b main
> c
```

Si quiere realizarse un debugueo también _step by step_ desde el principio, una vez abierto el `gdb` comenzar a ejecutar `s` o `si` (no significan lo mismo **OJO**).

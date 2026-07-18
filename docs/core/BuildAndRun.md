# Build do Aether

Este documento descreve como configurar o ambiente de desenvolvimento e compilar o Aether utilizando o CLion com um servidor Linux remoto.

---

# Requisitos

## Servidor Linux

O servidor deve possuir os seguintes pacotes instalados:

- GCC
- G++
- CMake 3.20 ou superior
- GNU Make
- GDB
- Git

### Debian

```bash
sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    gdb \
    git
```

---

# Clonando o projeto

```bash
git clone https://github.com/<usuario>/Aether.git

cd Aether
```

---

# Compilação Manual

Criar o diretório de build:

```bash
mkdir -p build
cd build
```

Gerar os arquivos do CMake:

```bash
cmake ..
```

Compilar:

```bash
cmake --build .
```

ou

```bash
make -j$(nproc)
```

---

# Executando

Exemplo:

```bash
.aether-core/apps/aetherd/aetherd
```

ou

```bash
.aether-core/apps/aether_cli/aether_cli
```

*(Os caminhos podem variar conforme os executáveis configurados no projeto.)*

---

# Build utilizando o CLion

## Configuração do Toolchain

Acessar:

```
Settings
→ Build, Execution, Deployment
→ Toolchains
```

Criar um novo Toolchain:

```
Type:
Remote Host
```

Configurar:

```
Name:
Remote Host

Credentials:
SSH

CMake:
Remote Host CMake

Build Tool:
Detected: gmake

C Compiler:
Detected: cc

C++ Compiler:
Detected: c++

Debugger:
Remote Host GDB
```

Todos os componentes devem aparecer em verde.

---

## Configuração do CMake

Acessar:

```
Settings
→ Build, Execution, Deployment
→ CMake
```

Criar um Profile:

```
Name:
Debug

Toolchain:
Remote Host

Build Type:
Debug

Generation Path:
cmake-build-debug
```

---

## Abrindo o projeto

Abrir a pasta raiz do projeto:

```
Aether/
│
├── CMakeLists.txt
├── aether-core
├──── core/
├──── modules/
└── ...
```

O CLion irá detectar automaticamente o projeto CMake.

Caso isso não ocorra:

```
File
→ Reload CMake Project
```

ou

```
Tools
→ CMake
→ Reset Cache and Reload Project
```

---

## Compilando

Selecionar o Target desejado.

Exemplos:

```
aetherd (Ponto inicial do Software)

aether_cli
```

Depois:

```
Build
→ Build Project
```

ou

```
Ctrl + F9
```

---

## Executando

Criar uma nova Run Configuration do tipo:

```
CMake Application
```

Selecionar:

```
Target:
aetherd
```

ou

```
Target:
aether_cli
```

Executar normalmente pelo CLion.

---

# Problemas conhecidos

## O projeto não é reconhecido como CMake

Caso apareça a mensagem:

```
Not a plain CMake project
```

Remova a pasta:

```
.idea/
```

e abra novamente a pasta raiz do projeto.

O CLion recriará automaticamente as configurações.

---

## Limpando o build

```bash
rm -rf build
```

ou

```bash
rm -rf cmake-build-debug
```

Depois execute novamente:

```bash
mkdir build
cd build

cmake ..
cmake --build .
```
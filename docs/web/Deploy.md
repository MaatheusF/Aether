# Ambiente Web

Este documento descreve como configurar o ambiente de desenvolvimento da interface Web do Aether.

---

# Requisitos

## PHP

Versão mínima:

```
PHP 8.3
```

Extensões:

```
ctype
iconv
json
mbstring
openssl
pdo
pgsql
```

Verificar:

```bash
php -v
php -m
```

---

## Composer

Verificar:

```bash
composer --version
```

---

## NodeJS

Versão recomendada:

```
22 LTS
```

Verificar:

```bash
node -v
npm -v
```

---

# Instalando dependências

Entrar na pasta:

```bash
cd web
```

Instalar dependências PHP:

```bash
composer install
```

Instalar dependências Node:

```bash
npm install
```

---

# Tailwind CSS

Gerar o CSS:

```bash
npm run build
```

Modo desenvolvimento:

```bash
npm run watch
```

---

# Assets

Os arquivos CSS ficam em:

```
assets/
```

Após o build:

```
public/build/
```

Nunca editar diretamente:

```
public/build/app.css
```

---

# Banco de Dados

Criar o banco:

```bash
php bin/console doctrine:database:create
```

Executar migrations:

```bash
php bin/console doctrine:migrations:migrate
```

---

# Executando

Servidor Symfony:

```bash
symfony server:start
```

ou

```bash
php -S localhost:8000 -t public
```

---

# Produção

Instalar dependências:

```bash
composer install --no-dev
```

Gerar assets:

```bash
npm run build
```

Limpar cache:

```bash
php bin/console cache:clear
```

---

# Estrutura

```
web/

assets/
config/
public/
src/
templates/
vendor/
```

---

# Comandos úteis

Instalar dependências:

```bash
composer install
npm install
```

Atualizar dependências:

```bash
composer update
npm update
```

Build CSS:

```bash
npm run build
```

Modo desenvolvimento:

```bash
npm run watch
```

Limpar cache:

```bash
php bin/console cache:clear
```

Listar rotas:

```bash
php bin/console debug:router
```

Listar serviços:

```bash
php bin/console debug:container
```

## Compilação do CSS usando PostCSS
#### Produção:
- npx postcss assets/styles/app.css -o public/build/app.css

#### Desenvolvimento:
- npx postcss assets/styles/app.css -o public/build/app.css --watch
- npm run css:watch

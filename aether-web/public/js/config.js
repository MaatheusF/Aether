/* public/js/config.js
   Configuração compartilhada de acesso ao Core, usada por todos os
   serviços em public/js/services/*.js — evita repetir o IP/porta do
   Core em cada arquivo (o que já teria acontecido ao criar
   poseidon_status.js se essa constante não existisse).

   TODO: mover para variável de ambiente injetada pelo Symfony
   (ex: via um <meta> no <head> lido daqui) quando o IP do Core deixar
   de ser fixo na rede local.
*/
window.AETHER_CORE_BASE_URL = 'http://192.168.0.133:9001';

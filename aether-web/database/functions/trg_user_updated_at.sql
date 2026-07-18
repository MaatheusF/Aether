/* TRIGGER: user_updated_at */
/* DESCRIPTION: Update na coluna updated_at na tabela user_user adicionando a data e hora atual que o registro foi alterado */
CREATE TRIGGER trg_user_updated_at
BEFORE UPDATE ON aether_core.user_user
FOR EACH ROW
EXECUTE FUNCTION aether_core.touch_updated_at();

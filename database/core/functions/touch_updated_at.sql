/* FUNCTION: touch_updated_at */
/* DESCRIPTION: Atualiza a coluna updated_at para a data/hora atual quando um registro é alterado */
CREATE OR REPLACE FUNCTION aether_core.touch_updated_at()
RETURNS TRIGGER
LANGUAGE plpgsql
AS $$
BEGIN
	IF NEW IS DISTINCT FROM OLD THEN
	    NEW.updated_at := CURRENT_TIMESTAMP;
END IF;
RETURN NEW;
END;
$$;

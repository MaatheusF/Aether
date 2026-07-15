/* Trigger para dar update na coluna updated_at quando o registor for alterado */
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

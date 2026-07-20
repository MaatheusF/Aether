CREATE SCHEMA IF NOT EXISTS aether_core;

CREATE TABLE aether_core.user_user
(
    id           BIGSERIAL PRIMARY KEY,
    username     VARCHAR(100) NOT NULL,
    password     VARCHAR(255) NOT NULL,
    roles        JSONB NOT NULL DEFAULT '["ROLE_USER"]',
    enabled      BOOLEAN NOT NULL DEFAULT TRUE,
    created_at   TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at   TIMESTAMP NULL,
    last_login   TIMESTAMP NULL,
    CONSTRAINT uk_user_username UNIQUE (username)
);

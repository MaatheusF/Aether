#pragma once

#include <fstream>
#include <iostream>

/** @class AetherCoreLogger
 * @brief Classe responsável pelo registro de logs do sistema AetherCore.
 *
 * Esta classe fornece funcionalidades para inicializar o sistema de log,
 * registrar mensagens de log e finalizar o sistema de log.
 */
class AetherCoreLogger
{
public:
    /**
     * @brief Inicializa o sistema de log com o arquivo especificado.
     *
     * @param filename Nome do arquivo onde os logs serão armazenados.
     */
    static void Initialize(const std::string &filename)
    {
        logFile.open(filename, std::ios::out | std::ios::app);
        if (!logFile.is_open())
        {
            std::cerr << "Falha ao abrir aquivo de log: " << filename << std::endl;
            return;
        }

        static TeeBuffer coutTee(std::cout.rdbuf(), logFile.rdbuf());           /// Redireciona std::cout
        static TeeBuffer cerrTee(std::cerr.rdbuf(), logFile.rdbuf());           /// Redireciona std::cerr

        std::cout.rdbuf(&coutTee);  /// Aplica o redirecionamento
        std::cerr.rdbuf(&cerrTee);  /// Aplica o redirecionamento
    }

    /**
     * @brief Registra uma mensagem de log no arquivo.
     *
     * @param message Mensagem a ser registrada.
     */
    static void Log(const std::string& message)
    {
        if (logFile.is_open())
        {
            logFile << returnCurrentTimeStamp() << " " << message << std::endl;
        }
    }

    /**
     * @brief Finaliza o sistema de log, fechando o arquivo de log.
     */
    static void Shutdown()
    {
        if (logFile.is_open())
        {
            logFile.close();
        }
    }

    /**
     * @brief Retorna o timestamp atual no formato "YYYY-MM-DD HH:MM:SS".
     * @return std::string Timestamp formatado.
     */
    std::string static returnCurrentTimeStamp();

private:
    /**
     * @class TeeBuffer
     * @brief Classe auxiliar para redirecionar saídas para múltiplos buffers.
     */
    class TeeBuffer : public std::streambuf
    {
    public:
        /**
         * @brief Construtor da classe TeeBuffer.
         * @param sb1 Primeiro buffer de saída.
         * @param sb2 Segundo buffer de saída.
         */
        TeeBuffer(std::streambuf* sb1, std::streambuf* sb2) : sb1(sb1), sb2(sb2), atLineStart(true) {}

    protected:
        /**
         * @brief Sobrescreve a função overflow para redirecionar a saída.
         * @param c Caractere a ser escrito.
         * @return Retorna o caractere escrito ou EOF em caso de erro.
         */
        int overflow(int c) override
        {
            if (c == EOF) return !EOF;

            // Console
            if (sb1)
            {
                if (atLineStart)
                {
                    std::string ts = returnCurrentTimeStamp() + " | ";
                    sb1->sputn(ts.c_str(), ts.size());
                    atLineStart = false;
                }

                sb1->sputc(c);  // Escreve o caractere no arquivo de log

                if (c == '\n')
                    atLineStart = true; // próxima linha começa, adiciona timestamp
            }

            // Arquivo de Log
            if (sb2)
            {
                if (atLineStart)
                {
                    std::string ts = returnCurrentTimeStamp() + " | ";
                    sb2->sputn(ts.c_str(), ts.size());
                    atLineStart = false;
                }

                sb2->sputc(c);  // Escreve o caractere no arquivo de log

                if (c == '\n')
                    atLineStart = true; // próxima linha começa, adiciona timestamp
            }

            return c;
        }

        /**
         * @brief Sobrescreve a função sync para sincronizar os buffers.
         * @return Retorna 0 em caso de sucesso, -1 em caso de erro.
         */
        int sync() override
        {
            int r1 = sb1 ? sb1->pubsync() : 0;          /// Sincroniza o primeiro buffer
            int r2 = sb2 ? sb2->pubsync() : 0;          /// Sincroniza o segundo buffer
            return (r1 == 0 && r2 == 0) ? 0 : -1;       /// Retorna 0 se ambos sincronizaram com sucesso
        }

    private:
        std::streambuf* sb1;        /// Primeiro buffer de saída
        std::streambuf* sb2;        /// Segundo buffer de saída
        bool atLineStart = false;   /// indica se está no início de uma linha
    };

    static std::ofstream logFile;       /// Arquivo de log
};
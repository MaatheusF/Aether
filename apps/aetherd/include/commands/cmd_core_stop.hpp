#include <condition_variable>
#include <mutex>
#include <set>

inline std::mutex stopMutex;
inline std::condition_variable stopCv;
inline std::set<std::string> stoppedModules;

/**
 * @brief Função que verifica e aquarda receber um comando "MODULE_STOPPED" de todos
 * os modulos inscritos para continuar
 * @param modules lista de modulos inscritos
 */
inline void waitForAllModulesToStop(const std::vector<IModule*>& modules) {
    std::unique_lock<std::mutex> lock(stopMutex);
    stoppedModules.clear();

    // Marca os modulos ja parados
    for (auto* m : modules)
    {
        if (!m->isRunning())
        {
            stoppedModules.insert(m->name()); //Adiciona o modulo na lista de modulos parados
        }
    }

    /// Publica o evento de parada para todos os módulos que estão rodando
    for (auto* m : modules)
    {
        if (m->isRunning())
        {
            EventBus::getInstance().publish(
                Event("CLI", m->name(), Events::CORE_STOP, {})
            );
            std::cout << "[CORE_STOP] Enviando comando de parada para o módulo: " << m->name() << std::endl;
        }
    }

    // Espera até todos os módulos confirmarem parada
    stopCv.wait(lock, [&modules]() {
        return stoppedModules.size() == modules.size();
    });
}

/**
 * @brief Classe que registra um callback para monitorar MODULE_STOPPED
 */
class StopListener : public IModule {
public:
    void onEvent(const Event& event) override {
        if (event.type == Events::MODULE_STOPPED) {
            std::lock_guard<std::mutex> lock(stopMutex);
            stoppedModules.insert(event.source);
            stopCv.notify_all();
        }
    }
    std::string name() const override { return "StopListener"; }
};

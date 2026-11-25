#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <SFML/System/Vector3.hpp>
#include <memory>
#include <filesystem>
#include <cstdio>
#include <glm.hpp>

class EfectosAmbiente {
public:
    bool Inicializar();
    bool Cargado() const { return cargado; }

    void ActualizarListener(const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up);
    void ActualizarMotorNave(bool activo, const glm::vec3& pos);
    void ActualizarMotorMain(bool activo, const glm::vec3& pos);
    void ReproducirPuerta(const glm::vec3& pos);
    void ReproducirPortal(const glm::vec3& pos);
    void ReproducirLlegada(const glm::vec3& pos);

    bool ReproduciendoMotorNave() const { return Reproduciendo(motorNave); }
    bool ReproduciendoMotorMain() const { return Reproduciendo(motorMain); }
    bool ReproduciendoPuerta() const { return Reproduciendo(puerta); }
    bool ReproduciendoPortal() const { return Reproduciendo(portal); }
    bool ReproduciendoLlegada() const { return Reproduciendo(llegada); }
    void DetenerTodo();

private:
    static sf::Vector3f ASfml(const glm::vec3& v) {
        return sf::Vector3f(v.x, v.y, v.z);
    }
    static bool Reproduciendo(const std::unique_ptr<sf::Sound>& sonido) {
        return sonido && sonido->getStatus() == sf::SoundSource::Status::Playing;
    }
    static void ConfigurarLoop(sf::Sound& sonido, float volumen);
    static void ConfigurarUnaVez(sf::Sound& sonido, float volumen);
    static void ActualizarLoop(std::unique_ptr<sf::Sound>& sonido, bool activo, const glm::vec3& pos);
    static void Disparar(std::unique_ptr<sf::Sound>& sonido, const glm::vec3& pos, const char* nombre);
    static bool Cargar(sf::SoundBuffer& buffer, const char* ruta);

    sf::SoundBuffer bufMotorNave;
    sf::SoundBuffer bufMotorMain;
    sf::SoundBuffer bufPuerta;
    sf::SoundBuffer bufPortal;
    sf::SoundBuffer bufLlegada;
    std::unique_ptr<sf::Sound> motorNave;
    std::unique_ptr<sf::Sound> motorMain;
    std::unique_ptr<sf::Sound> puerta;
    std::unique_ptr<sf::Sound> portal;
    std::unique_ptr<sf::Sound> llegada;
    bool cargado = false;
    bool intentado = false;
};

inline bool EfectosAmbiente::Cargar(sf::SoundBuffer& buffer, const char* ruta) {
    if (!buffer.loadFromFile(std::filesystem::path(ruta))) {
        printf("[Efectos] No se pudo cargar %s\n", ruta);
        fflush(stdout);
        return false;
    }
    return true;
}

inline void EfectosAmbiente::ConfigurarLoop(sf::Sound& sonido, float volumen) {
    sonido.setRelativeToListener(false);
    sonido.setLooping(true);
    sonido.setMinDistance(5.0f);
    sonido.setMaxDistance(160.0f);
    sonido.setAttenuation(1.0f);
    sonido.setVolume(volumen);
}

inline void EfectosAmbiente::ConfigurarUnaVez(sf::Sound& sonido, float volumen) {
    sonido.setRelativeToListener(false);
    sonido.setLooping(false);
    sonido.setMinDistance(5.0f);
    sonido.setMaxDistance(160.0f);
    sonido.setAttenuation(1.0f);
    sonido.setVolume(volumen);
}

inline bool EfectosAmbiente::Inicializar() {
    if (intentado) return cargado;
    intentado = true;

    const bool ok =
        Cargar(bufMotorNave, "audio/e_motor_nave_mono.wav") &&
        Cargar(bufMotorMain, "audio/e_motor_nave_main_mono.wav") &&
        Cargar(bufPuerta, "audio/e_puerta_estacion_mono.wav") &&
        Cargar(bufPortal, "audio/e_portal_mono.wav") &&
        Cargar(bufLlegada, "audio/reksai_mono.wav");
    if (!ok) return false;

    motorNave = std::make_unique<sf::Sound>(bufMotorNave);
    motorMain = std::make_unique<sf::Sound>(bufMotorMain);
    puerta = std::make_unique<sf::Sound>(bufPuerta);
    portal = std::make_unique<sf::Sound>(bufPortal);
    llegada = std::make_unique<sf::Sound>(bufLlegada);
    ConfigurarLoop(*motorNave, 70.0f);
    ConfigurarLoop(*motorMain, 70.0f);
    ConfigurarUnaVez(*puerta, 100.0f);
    ConfigurarUnaVez(*portal, 100.0f);
    ConfigurarUnaVez(*llegada, 100.0f);

    sf::Listener::setPosition(ASfml(glm::vec3(0.0f)));
    sf::Listener::setDirection(ASfml(glm::vec3(0.0f, 0.0f, 1.0f)));
    sf::Listener::setUpVector(ASfml(glm::vec3(0.0f, 1.0f, 0.0f)));
    sf::Listener::setVelocity(ASfml(glm::vec3(0.0f)));

    cargado = true;
    printf("[Efectos] Motores de nave y efectos de ambiente listos\n");
    fflush(stdout);
    return true;
}

inline void EfectosAmbiente::ActualizarListener(const glm::vec3& pos, const glm::vec3& forward,
    const glm::vec3& up) {
    sf::Listener::setPosition(ASfml(pos));
    sf::Listener::setDirection(ASfml(forward));
    sf::Listener::setUpVector(ASfml(up));
    sf::Listener::setVelocity(ASfml(glm::vec3(0.0f)));
}

inline void EfectosAmbiente::ActualizarLoop(std::unique_ptr<sf::Sound>& sonido, bool activo,
    const glm::vec3& pos) {
    if (!sonido) return;
    sonido->setPosition(ASfml(pos));
    const sf::SoundSource::Status estado = sonido->getStatus();
    if (activo) {
        if (estado != sf::SoundSource::Status::Playing) sonido->play();
    } else if (estado != sf::SoundSource::Status::Stopped) {
        sonido->stop();
    }
}

inline void EfectosAmbiente::ActualizarMotorNave(bool activo, const glm::vec3& pos) {
    if (!cargado) return;
    ActualizarLoop(motorNave, activo, pos);
}

inline void EfectosAmbiente::ActualizarMotorMain(bool activo, const glm::vec3& pos) {
    if (!cargado) return;
    ActualizarLoop(motorMain, activo, pos);
}

inline void EfectosAmbiente::Disparar(std::unique_ptr<sf::Sound>& sonido, const glm::vec3& pos,
    const char* nombre) {
    if (!sonido) return;
    sonido->stop();
    sonido->setPosition(ASfml(pos));
    sonido->play();
    printf("[Efecto] %s\n", nombre);
    fflush(stdout);
}

inline void EfectosAmbiente::ReproducirPuerta(const glm::vec3& pos) {
    if (!cargado) return;
    Disparar(puerta, pos, "puerta");
}

inline void EfectosAmbiente::ReproducirPortal(const glm::vec3& pos) {
    if (!cargado) return;
    Disparar(portal, pos, "portal");
}

inline void EfectosAmbiente::ReproducirLlegada(const glm::vec3& pos) {
    if (!cargado) return;
    Disparar(llegada, pos, "llegada");
}

inline void EfectosAmbiente::DetenerTodo() {
    for (auto* sonido : { &motorNave, &motorMain, &puerta, &portal, &llegada })
        if (*sonido) (*sonido)->stop();
}

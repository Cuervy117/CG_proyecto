#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <SFML/System/Vector3.hpp>
#include <array>
#include <memory>
#include <string>
#include <filesystem>
#include <cstdio>
#include <glm.hpp>

class SonidoCohetes {
public:
    static constexpr int EMISORES = 4;

    bool Inicializar();
    bool Cargado() const { return cargado; }
    void ActualizarListener(const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up);
    void Actualizar(int i, bool activo, const glm::vec3& pos);
    bool Reproduciendo(int i) const;
    void DetenerTodo();

private:
    static sf::Vector3f ASfml(const glm::vec3& v) {
        return sf::Vector3f(v.x, v.y, v.z);
    }

    sf::SoundBuffer buffer;
    std::array<std::unique_ptr<sf::Sound>, EMISORES> sonidos;
    bool cargado = false;
    bool intentado = false;
};

inline bool SonidoCohetes::Inicializar() {
    if (intentado) return cargado;
    intentado = true;

    if (!buffer.loadFromFile(std::filesystem::path("audio/e_cohete_mono.wav"))) {
        printf("[Audio] No se pudo cargar audio/e_cohete_mono.wav\n");
        fflush(stdout);
        return false;
    }
    if (buffer.getChannelCount() != 1) {
        printf("[Audio] e_cohete_mono.wav no es mono (%u canales); el 3D puede ignorarse\n",
            buffer.getChannelCount());
    }

    sf::Listener::setPosition(ASfml(glm::vec3(0.0f)));
    sf::Listener::setDirection(ASfml(glm::vec3(0.0f, 0.0f, 1.0f)));
    sf::Listener::setUpVector(ASfml(glm::vec3(0.0f, 1.0f, 0.0f)));
    sf::Listener::setVelocity(ASfml(glm::vec3(0.0f)));

    for (int i = 0; i < EMISORES; ++i) {
        sonidos[i] = std::make_unique<sf::Sound>(buffer);
        sonidos[i]->setRelativeToListener(false);
        sonidos[i]->setLooping(true);
        sonidos[i]->setMinDistance(8.0f);
        sonidos[i]->setMaxDistance(140.0f);
        sonidos[i]->setAttenuation(1.0f);
        sonidos[i]->setVolume(100.0f);
    }

    cargado = true;
    printf("[Audio] SFML %d.%d.%d | e_cohete_mono.wav | %d emisores\n",
        SFML_VERSION_MAJOR, SFML_VERSION_MINOR, SFML_VERSION_PATCH, EMISORES);
    fflush(stdout);
    return true;
}

inline void SonidoCohetes::ActualizarListener(const glm::vec3& pos, const glm::vec3& forward,
    const glm::vec3& up) {
    sf::Listener::setPosition(ASfml(pos));
    sf::Listener::setDirection(ASfml(forward));
    sf::Listener::setUpVector(ASfml(up));
    sf::Listener::setVelocity(ASfml(glm::vec3(0.0f)));
}

inline void SonidoCohetes::Actualizar(int i, bool activo, const glm::vec3& pos) {
    if (!cargado || i < 0 || i >= EMISORES || !sonidos[i]) return;
    sonidos[i]->setPosition(ASfml(pos));
    const sf::SoundSource::Status estado = sonidos[i]->getStatus();
    if (activo) {
        if (estado != sf::SoundSource::Status::Playing) sonidos[i]->play();
    } else if (estado != sf::SoundSource::Status::Stopped) {
        sonidos[i]->stop();
    }
}

inline bool SonidoCohetes::Reproduciendo(int i) const {
    if (!cargado || i < 0 || i >= EMISORES || !sonidos[i]) return false;
    return sonidos[i]->getStatus() == sf::SoundSource::Status::Playing;
}

inline void SonidoCohetes::DetenerTodo() {
    for (auto& sonido : sonidos)
        if (sonido) sonido->stop();
}

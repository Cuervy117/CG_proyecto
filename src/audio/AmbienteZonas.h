#pragma once
#include <SFML/Audio.hpp>
#include <memory>
#include <filesystem>
#include <cstdio>

class AmbienteZonas {
public:
    bool Inicializar();
    bool Cargado() const { return cargado; }

    void ActualizarZona(int zona);
    void ActualizarReksai(float dt, bool activo);

    bool ReproduciendoZona() const {
        return Reproduciendo(pajaros) || Reproduciendo(insectos) || Reproduciendo(olas);
    }
    bool ReproduciendoReksai() const { return Reproduciendo(reksai); }
    const char* ZonaActual() const;
    void Detener();

private:
    static bool Reproduciendo(const std::unique_ptr<sf::Sound>& sonido) {
        return sonido && sonido->getStatus() == sf::SoundSource::Status::Playing;
    }
    static bool Cargar(sf::SoundBuffer& buffer, const char* ruta);
    static void ConfigurarLoop(sf::Sound& sonido, float volumen);

    sf::SoundBuffer bufPajaros;
    sf::SoundBuffer bufInsectos;
    sf::SoundBuffer bufOlas;
    sf::SoundBuffer bufReksai;
    std::unique_ptr<sf::Sound> pajaros;
    std::unique_ptr<sf::Sound> insectos;
    std::unique_ptr<sf::Sound> olas;
    std::unique_ptr<sf::Sound> reksai;
    int zonaActual = -1;
    float temporizadorReksai = 0.0f;
    bool cargado = false;
    bool intentado = false;
};

inline bool AmbienteZonas::Cargar(sf::SoundBuffer& buffer, const char* ruta) {
    if (!buffer.loadFromFile(std::filesystem::path(ruta))) {
        printf("[Ambiente] No se pudo cargar %s\n", ruta);
        fflush(stdout);
        return false;
    }
    return true;
}

inline void AmbienteZonas::ConfigurarLoop(sf::Sound& sonido, float volumen) {
    sonido.setSpatializationEnabled(false);
    sonido.setRelativeToListener(true);
    sonido.setLooping(true);
    sonido.setVolume(volumen);
}

inline bool AmbienteZonas::Inicializar() {
    if (intentado) return cargado;
    intentado = true;
    const bool ok =
        Cargar(bufPajaros, "audio/a_pajaros.wav") &&
        Cargar(bufInsectos, "audio/a_insectos.wav") &&
        Cargar(bufOlas, "audio/a_olas.wav") &&
        Cargar(bufReksai, "audio/a_reksai.wav");
    if (!ok) return false;

    pajaros = std::make_unique<sf::Sound>(bufPajaros);
    insectos = std::make_unique<sf::Sound>(bufInsectos);
    olas = std::make_unique<sf::Sound>(bufOlas);
    reksai = std::make_unique<sf::Sound>(bufReksai);
    ConfigurarLoop(*pajaros, 45.0f);
    ConfigurarLoop(*insectos, 45.0f);
    ConfigurarLoop(*olas, 45.0f);
    reksai->setSpatializationEnabled(false);
    reksai->setRelativeToListener(true);
    reksai->setLooping(false);
    reksai->setVolume(60.0f);

    cargado = true;
    printf("[Ambiente] a_pajaros, a_insectos, a_olas y a_reksai listos\n");
    fflush(stdout);
    return true;
}

inline const char* AmbienteZonas::ZonaActual() const {
    switch (zonaActual) {
    case 0: return "a_pajaros";
    case 1: return "a_insectos";
    case 2: return "a_olas";
    default: return "silencio";
    }
}

inline void AmbienteZonas::ActualizarZona(int zona) {
    if (!cargado) return;
    const int deseada = (zona == 2) ? 0 : (zona == 1) ? 1 : (zona == 3) ? 2 : -1;
    if (deseada == zonaActual) {
        sf::Sound* actual = deseada == 0 ? pajaros.get() : deseada == 1 ? insectos.get() : deseada == 2 ? olas.get() : nullptr;
        if (actual && actual->getStatus() != sf::SoundSource::Status::Playing) actual->play();
        return;
    }

    if (pajaros) pajaros->stop();
    if (insectos) insectos->stop();
    if (olas) olas->stop();
    zonaActual = deseada;
    if (deseada == 0 && pajaros) pajaros->play();
    else if (deseada == 1 && insectos) insectos->play();
    else if (deseada == 2 && olas) olas->play();
    printf("[Ambiente] %s\n", ZonaActual());
    fflush(stdout);
}

inline void AmbienteZonas::ActualizarReksai(float dt, bool activo) {
    if (!cargado || !reksai) return;
    if (!activo) {
        temporizadorReksai = 0.0f;
        if (Reproduciendo(reksai)) reksai->stop();
        return;
    }
    temporizadorReksai += dt;
    if (temporizadorReksai >= 15.0f) {
        temporizadorReksai = 0.0f;
        reksai->stop();
        reksai->play();
        printf("[Ambiente] a_reksai\n");
        fflush(stdout);
    }
}

inline void AmbienteZonas::Detener() {
    for (auto* sonido : { &pajaros, &insectos, &olas, &reksai })
        if (*sonido) (*sonido)->stop();
    temporizadorReksai = 0.0f;
    zonaActual = -1;
}

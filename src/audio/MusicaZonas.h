#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <filesystem>
#include <cstdio>

class MusicaZonas {
public:
    bool Inicializar();
    bool Cargado() const { return cargado; }
    void Actualizar(int zona, bool carrera);
    const std::string& PistaActual() const { return actual; }
    bool Reproduciendo() const {
        return musica.getStatus() == sf::SoundSource::Status::Playing;
    }
    bool NoEspacializada() const { return !musica.isSpatializationEnabled(); }
    bool RelativaAlOyente() const { return musica.isRelativeToListener(); }
    float Volumen() const { return musica.getVolume(); }
    void Detener() { musica.stop(); }

    static const char* PistaParaZona(int zona, bool carrera) {
        if (carrera) return "audio/st_carrera.mp3";
        switch (zona) {
        case 0: return "audio/st_espacio.mp3";
        case 1: return "audio/st_reksai.mp3";
        case 2: return "audio/st_shinobi.mp3";
        case 3: return "audio/st_agua.mp3";
        default: return "audio/st_espacio.mp3";
        }
    }

private:
    sf::Music musica;
    std::string actual;
    bool cargado = false;
    bool intentado = false;
};

inline bool MusicaZonas::Inicializar() {
    if (intentado) return cargado;
    intentado = true;
    musica.setSpatializationEnabled(false);
    musica.setRelativeToListener(true);
    musica.setVolume(65.0f);
    cargado = true;
    printf("[Musica] Pistas por zona listas | no espacializada | volumen 65\n");
    fflush(stdout);
    return true;
}

inline void MusicaZonas::Actualizar(int zona, bool carrera) {
    if (!cargado) return;
    const std::string deseada = PistaParaZona(zona, carrera);
    if (deseada == actual) {
        if (musica.getStatus() != sf::SoundSource::Status::Playing) musica.play();
        return;
    }

    musica.stop();
    if (!musica.openFromFile(std::filesystem::path(deseada))) {
        printf("[Musica] No se pudo cargar %s\n", deseada.c_str());
        fflush(stdout);
        actual.clear();
        return;
    }
    musica.setLooping(true);
    musica.setSpatializationEnabled(false);
    musica.setRelativeToListener(true);
    musica.setVolume(65.0f);
    musica.play();
    actual = deseada;
    printf("[Musica] %s\n", actual.c_str());
    fflush(stdout);
}

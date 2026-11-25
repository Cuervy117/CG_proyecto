#pragma once
#include <array>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Model.h"

class CoheteDavid {
public:
    static constexpr int PIEZAS = 6;

    void Cargar();
    void Actualizar(float dt, bool iniciar, bool reiniciar);
    void Dibujar(GLuint uniformModel, GLuint uniformColor, GLuint uniformEmision,
        MeshModel* llama, Texture& fuego);

    bool Cargado() const { return cargado; }
    bool Encendido() const { return cargado && (fase == 1 || fase == 2); }
    glm::vec3 Posicion() const { return origen; }
    glm::vec3 PosicionTobera() const;
    const char* TextoEstado() const;

private:
    struct Pose { float x = 0.0f, y = 0.0f, giro = 0.0f; };
    struct Frame { float t = 0.0f; std::array<Pose, PIEZAS> piezas{}; };

    std::array<Model, PIEZAS> modelos;
    std::array<std::vector<Frame>, 5> secuencias;
    std::array<Pose, PIEZAS> pose{};
    std::array<Pose, PIEZAS> poseFinalCuarta{};
    Pose ajusteOrbita{};
    glm::vec3 origen = glm::vec3(-47.0f, 0.0f, 7.0f);
    float tiempo = 0.0f;
    float reloj = 0.0f;
    int fase = 0;
    bool reproduciendo = false;
    bool cargado = false;
    bool intentado = false;

    static const char* NombrePieza(int i);
    static glm::vec3 Pivote(int i);
    static float MinY(int i);
    static glm::vec3 Montaje(int i);
    static std::array<int, PIEZAS> Padres(int f);
    static std::array<Pose, PIEZAS> Interpolar(const std::vector<Frame>& frames, float t);
    bool LeerArchivo(const char* ruta, std::array<std::vector<Frame>, 5>& destino);
    void Evaluar(float t);
    std::array<glm::mat4, PIEZAS> Matrices() const;
};

inline const char* CoheteDavid::NombrePieza(int i) {
    static const char* nombres[PIEZAS] = {
        "primera_etapa", "anillo_separacion", "segunda_etapa",
        "cofia_izquierda", "cofia_derecha", "capsula"
    };
    return (i >= 0 && i < PIEZAS) ? nombres[i] : "";
}

inline glm::vec3 CoheteDavid::Pivote(int i) {
    static const glm::vec3 pivotes[PIEZAS] = {
        glm::vec3(0.0f, 5.3f, -0.0029445f),
        glm::vec3(-0.198643f, 10.9f, 0.1915745f),
        glm::vec3(-0.3171725f, 15.8f, 0.2614895f),
        glm::vec3(-0.913754763f, 21.95f, 0.288221472f),
        glm::vec3(0.556366894f, 21.95f, 0.254760922f),
        glm::vec3(-0.4030205f, 25.1886975f, 0.300873f)
    };
    return (i >= 0 && i < PIEZAS) ? pivotes[i] : glm::vec3(0.0f);
}

inline float CoheteDavid::MinY(int i) {
    static const float minimos[PIEZAS] = { 0.0f, 10.6f, 11.2f, 20.4f, 20.4f, 23.5f };
    return (i >= 0 && i < PIEZAS) ? minimos[i] : 0.0f;
}

inline glm::vec3 CoheteDavid::Montaje(int i) {
    static const glm::vec3 montaje[PIEZAS] = {
        glm::vec3(0.0f, 1.59f, -0.00088335f),
        glm::vec3(-0.0595929f, 3.27f, 0.05747235f),
        glm::vec3(-0.09515175f, 4.74f, 0.07844685f),
        glm::vec3(-0.274126429f, 6.585f, 0.086466442f),
        glm::vec3(0.166910068f, 6.585f, 0.076428277f),
        glm::vec3(-0.12090615f, 7.55660925f, 0.0902619f)
    };
    return (i >= 0 && i < PIEZAS) ? montaje[i] : glm::vec3(0.0f);
}

inline std::array<int, CoheteDavid::PIEZAS> CoheteDavid::Padres(int f) {
    if (f <= 1) return { -1, 0, 1, 5, 5, 2 };
    if (f == 2) return { -1, -1, -1, 5, 5, 2 };
    if (f == 3) return { -1, -1, -1, 5, 5, -1 };
    return { -1, -1, -1, -1, -1, -1 };
}

inline bool CoheteDavid::LeerArchivo(const char* ruta, std::array<std::vector<Frame>, 5>& destino) {
    std::ifstream in(ruta);
    if (!in) return false;
    std::string marca;
    int version = 0;
    if (!(in >> marca >> version) || marca != "COHETE_ETAPAS" || version != 1) return false;
    for (int e = 0; e < 5; ++e) {
        int id = 0, n = 0;
        if (!(in >> id >> n) || id != e + 1 || n < 2 || n > 1000) return false;
        destino[e].clear();
        for (int k = 0; k < n; ++k) {
            Frame f;
            if (!(in >> f.t) || !std::isfinite(f.t)) return false;
            if (k == 0 && f.t != 0.0f) return false;
            if (k > 0 && f.t <= destino[e].back().t) return false;
            for (auto& p : f.piezas) {
                if (!(in >> p.x >> p.y >> p.giro)) return false;
                if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.giro)) return false;
            }
            destino[e].push_back(f);
        }
    }
    return true;
}

inline std::array<CoheteDavid::Pose, CoheteDavid::PIEZAS> CoheteDavid::Interpolar(
    const std::vector<Frame>& frames, float t) {
    std::array<Pose, PIEZAS> salida{};
    if (frames.empty()) return salida;
    if (t <= frames.front().t) return frames.front().piezas;
    if (t >= frames.back().t) return frames.back().piezas;
    size_t i = 0;
    while (i + 1 < frames.size() - 1 && frames[i + 1].t < t) ++i;
    const Frame& a = frames[i];
    const Frame& b = frames[i + 1];
    const float u = (b.t > a.t) ? (t - a.t) / (b.t - a.t) : 0.0f;
    for (int j = 0; j < PIEZAS; ++j) {
        salida[j].x = a.piezas[j].x + (b.piezas[j].x - a.piezas[j].x) * u;
        salida[j].y = a.piezas[j].y + (b.piezas[j].y - a.piezas[j].y) * u;
        salida[j].giro = a.piezas[j].giro + (b.piezas[j].giro - a.piezas[j].giro) * u;
    }
    return salida;
}

inline void CoheteDavid::Cargar() {
    if (intentado) return;
    intentado = true;

    std::array<std::vector<Frame>, 5> separacion;
    std::array<std::vector<Frame>, 5> orbita;
    if (!LeerArchivo("Models/rocket/keyframes_rocket_etapas.txt", separacion)) {
        printf("[David] Keyframes de separacion invalidos\n");
        return;
    }
    if (!LeerArchivo("Models/rocket/keyframes_cohete_etapas.txt", orbita)) {
        printf("[David] Keyframes de orbita invalidos\n");
        return;
    }

    for (int i = 0; i < 4; ++i) secuencias[i] = separacion[i];
    secuencias[4] = orbita[4];
    poseFinalCuarta = secuencias[3].back().piezas;
    const Pose inicioOrbita = secuencias[4].front().piezas[5];
    ajusteOrbita.x = poseFinalCuarta[5].x - inicioOrbita.x;
    ajusteOrbita.y = poseFinalCuarta[5].y - inicioOrbita.y;
    ajusteOrbita.giro = poseFinalCuarta[5].giro - inicioOrbita.giro;

    for (int i = 0; i < PIEZAS; ++i) {
        std::string ruta = std::string("Models/rocket/piezas/") + NombrePieza(i) + ".obj";
        modelos[i].LoadModel(ruta);
        if (!modelos[i].estaCargado()) {
            printf("[David] Falta la pieza %s\n", NombrePieza(i));
            return;
        }
    }

    pose = {};
    fase = 0;
    tiempo = 0.0f;
    reloj = 0.0f;
    reproduciendo = false;
    cargado = true;
    printf("[David] Listo: 6 piezas, 5 fases; 8 reproducir, 9 reiniciar\n");
    fflush(stdout);
}

inline void CoheteDavid::Evaluar(float t) {
    if (fase <= 0) {
        pose = {};
        return;
    }
    if (fase >= 1 && fase <= 4) {
        pose = Interpolar(secuencias[fase - 1], t);
        return;
    }
    if (fase == 5) {
        const auto orbita = Interpolar(secuencias[4], t);
        for (int j = 0; j < PIEZAS - 1; ++j) pose[j] = poseFinalCuarta[j];
        pose[5].x = orbita[5].x + ajusteOrbita.x;
        pose[5].y = orbita[5].y + ajusteOrbita.y;
        pose[5].giro = orbita[5].giro + ajusteOrbita.giro;
    }
}

inline void CoheteDavid::Actualizar(float dt, bool iniciar, bool reiniciar) {
    if (!cargado) return;
    const float paso = std::max(dt, 0.0f);
    reloj += paso;

    if (reiniciar) {
        fase = 0;
        tiempo = 0.0f;
        reproduciendo = false;
        pose = {};
        return;
    }
    if (iniciar && !reproduciendo && fase == 0) {
        fase = 1;
        tiempo = 0.0f;
        reproduciendo = true;
    }
    if (!reproduciendo) {
        Evaluar(tiempo);
        return;
    }

    tiempo += paso;
    while (reproduciendo) {
        if (fase < 1 || fase > 5) {
            reproduciendo = false;
            break;
        }
        const std::vector<Frame>& tramo = secuencias[fase - 1];
        if (tramo.size() < 2) {
            reproduciendo = false;
            break;
        }
        const float duracion = tramo.back().t;
        if (fase == 5) {
            if (duracion > 0.0f) tiempo = std::fmod(tiempo, duracion);
            break;
        }
        if (tiempo < duracion) break;
        tiempo -= duracion;
        ++fase;
    }
    Evaluar(tiempo);
}

inline std::array<glm::mat4, CoheteDavid::PIEZAS> CoheteDavid::Matrices() const {
    std::array<glm::mat4, PIEZAS> mundo;
    const auto padres = Padres(fase);
    static const int orden[PIEZAS] = { 0, 1, 2, 5, 3, 4 };
    const glm::mat4 base = glm::translate(glm::mat4(1.0f), origen);
    for (int j : orden) {
        const int padre = padres[j];
        glm::vec3 local = Montaje(j);
        if (padre >= 0) local -= Montaje(padre);
        local.x += pose[j].x;
        local.y += pose[j].y;
        const glm::mat4 padreMundo = (padre >= 0) ? mundo[padre] : base;
        mundo[j] = glm::translate(padreMundo, local);
        mundo[j] = glm::rotate(mundo[j], glm::radians(pose[j].giro), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    return mundo;
}

inline void CoheteDavid::Dibujar(GLuint uniformModel, GLuint uniformColor, GLuint uniformEmision,
    MeshModel* llama, Texture& fuego) {
    if (!cargado) return;
    const auto mundo = Matrices();
    glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
    for (int j = 0; j < PIEZAS; ++j) {
        glm::mat4 m = glm::scale(mundo[j], glm::vec3(0.3f));
        m = glm::translate(m, -Pivote(j));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
        modelos[j].RenderModel(uniformColor, glm::vec3(1.0f));
    }

    const int motor = (fase == 1) ? 0 : (fase == 2 ? 2 : -1);
    if (motor >= 0 && llama != nullptr) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        const float pulso = 1.0f + 0.15f * std::sin(reloj * 30.0f);
        glm::mat4 m = glm::scale(mundo[motor], glm::vec3(0.3f));
        m = glm::translate(m, glm::vec3(0.0f, MinY(motor) - Pivote(motor).y + 0.25f, 0.0f));
        m = glm::scale(m, glm::vec3(1.8f, 4.0f * pulso, 1.8f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
        glUniform3f(uniformEmision, 1.0f, 0.45f, 0.10f);
        fuego.UseTexture();
        llama->RenderMeshModel();
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
    }
    glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
}

inline glm::vec3 CoheteDavid::PosicionTobera() const {
    const auto mundo = Matrices();
    const int motor = (fase == 2) ? 2 : 0;
    const glm::mat4 m = glm::scale(mundo[motor], glm::vec3(0.3f));
    const glm::vec4 p = m * glm::vec4(0.0f, MinY(motor) - Pivote(motor).y, 0.0f, 1.0f);
    return glm::vec3(p.x, p.y, p.z);
}

inline const char* CoheteDavid::TextoEstado() const {
    if (!cargado) return "DAVID SIN CARGAR";
    switch (fase) {
    case 0: return "DAVID LISTO";
    case 1: return "DAVID DESPEGUE";
    case 2: return "DAVID SEPARACION 1";
    case 3: return "DAVID SEPARACION 2";
    case 4: return "DAVID COFIAS";
    case 5: return "DAVID ORBITA";
    default: return "DAVID";
    }
}

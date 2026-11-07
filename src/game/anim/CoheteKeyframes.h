#pragma once
#include <array>
#include <fstream>
#include <sstream>
#include <cmath>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Model.h"

// Keyframes de Jona: duraciones por segmento y veinte canales, en orden del TXT.
class CoheteKeyframes {
    struct Frame { float t; std::array<float,20> v; };
    std::vector<Frame> frames;
    Model modelo;
    std::array<float,20> v{};
    float tiempo = 0;
    bool cargado = false, intentado = false, reproduciendo = false;
public:
    void Cargar() {
        if (intentado) return;
        intentado = true;
        std::ifstream file("Models/jona/keyframes_cohete.txt");
        std::string line;
        float acumulado = 0;
        while (std::getline(file,line)) {
            if (line.find_first_not_of(" \t\r") == std::string::npos || line[line.find_first_not_of(" \t\r")] == '#') continue;
            std::istringstream row(line);
            float duracion;
            Frame f{};
            bool ok = bool(row >> duracion) && std::isfinite(duracion) && duracion >= 0;
            for (float& x : f.v) ok = bool(row >> x) && std::isfinite(x) && ok;
            if (!ok || (!frames.empty() && duracion <= 0)) { frames.clear(); break; }
            acumulado += duracion; f.t = acumulado; frames.push_back(f);
        }
        if (frames.size() < 2 || frames.front().t != 0) { printf("[Jona] Keyframes invalidos\n"); return; }
        modelo.LoadModel("Models/jona/cohete_jona.obj");
        for (const char* part : {"plataforma", "primera_etapa", "anillo_separacion", "segunda_etapa", "cofia_izquierda", "cofia_derecha", "capsula"})
            if (!modelo.HasPart(part)) { printf("[Jona] Falta pieza %s\n", part); return; }
        cargado = true; v = frames.front().v;
        printf("[Jona] Listo: %zu keyframes, %.1f segundos; 6 reproducir, 7 reiniciar\n",frames.size(),frames.back().t);
    }
    void Actualizar(float dt, bool iniciar, bool reiniciar) {
        if (!cargado) return;
        if (reiniciar) { tiempo = 0; reproduciendo = false; v = frames.front().v; return; }
        if (iniciar && !reproduciendo) { tiempo = 0; reproduciendo = true; }
        if (!reproduciendo) return;
        tiempo = fminf(tiempo + fmaxf(dt,0), frames.back().t);
        size_t i = 1;
        while (i + 1 < frames.size() && frames[i].t < tiempo) ++i;
        float alpha = (tiempo - frames[i-1].t)/(frames[i].t-frames[i-1].t);
        for (size_t k=0;k<v.size();++k) v[k] = frames[i-1].v[k]*(1-alpha)+frames[i].v[k]*alpha;
        if (tiempo >= frames.back().t) reproduciendo = false;
    }
    void Dibujar(GLuint um, GLuint uc, GLuint ue, MeshModel* llama, Texture& fuego) {
        if (!cargado) return;
        // A la derecha del punto de llegada, con altura inicial de 21 unidades.
        glm::mat4 base = glm::translate(glm::mat4(1),glm::vec3(22,0,14));
        base = glm::scale(base,glm::vec3(.30f));
        glUniform3f(uc,1,1,1); glUniform3f(ue,0,0,0);
        auto pieza = [&](const char* nombre, glm::mat4 m) {
            glUniformMatrix4fv(um,1,GL_FALSE,glm::value_ptr(m)); modelo.RenderPart(nombre);
        };
        auto girar = [](glm::mat4 m, glm::vec3 pivote, float angulo) {
            m=glm::translate(m,pivote); m=glm::rotate(m,glm::radians(angulo),glm::vec3(0,0,1));
            return glm::translate(m,-pivote);
        };
        pieza("plataforma",base);
        glm::mat4 vuelo = glm::translate(base,glm::vec3(0,v[0],0));
        vuelo = girar(vuelo,glm::vec3(0,7.12f,0),v[1]);
        pieza("primera_etapa",girar(glm::translate(vuelo,glm::vec3(-v[7],-v[6],0)),glm::vec3(0,27.38f,0),v[8]));
        pieza("anillo_separacion",girar(glm::translate(vuelo,glm::vec3(v[10],-v[9],0)),glm::vec3(0,48.36f,0),v[11]));
        pieza("segunda_etapa",girar(glm::translate(vuelo,glm::vec3(v[13],-v[12],0)),glm::vec3(0,53.35f,0),v[14]));
        pieza("cofia_izquierda",girar(glm::translate(vuelo,glm::vec3(-v[5],-v[4],0)),glm::vec3(0,59,0),v[3]));
        pieza("cofia_derecha",girar(glm::translate(vuelo,glm::vec3(v[5],-v[4],0)),glm::vec3(0,59,0),-v[3]));
        float orbita=glm::radians(v[18]);
        glm::mat4 capsula=glm::translate(vuelo,glm::vec3(v[17]*cosf(orbita),v[16],v[17]*sinf(orbita)));
        pieza("capsula",girar(capsula,glm::vec3(0,61.8f,0),v[19]));
        // Los canales de fuego también se interpolan desde el archivo.
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE); glDepthMask(GL_FALSE);
        for (int etapa=0;etapa<2;++etapa) {
            float fuerza=etapa ? v[15] : v[2];
            if (fuerza <= .001f) continue;
            glm::mat4 m=glm::translate(vuelo,glm::vec3(0,(etapa ? 47.6f : 7.1f)-2.7f*fuerza,0));
            m=glm::scale(m,glm::vec3(3,6*fuerza,3));
            glUniformMatrix4fv(um,1,GL_FALSE,glm::value_ptr(m));
            glUniform3f(ue,fuerza,.4f*fuerza,.05f*fuerza); fuego.UseTexture(); llama->RenderMeshModel();
        }
        glDepthMask(GL_TRUE); glDisable(GL_BLEND); glUniform3f(ue,0,0,0); glUniform3f(uc,1,1,1);
    }
};

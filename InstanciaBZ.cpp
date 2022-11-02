//
//  InstanciaBZ.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#include "InstanciaBZ.h"

// ***********************************************************
//  void InstanciaPonto(Ponto3D *p, Ponto3D *out)
//  Esta funcao calcula as coordenadas de um ponto no
//  sistema de referencia do universo (SRU), ou seja,
//  aplica as rotacoes, escalas e translacoes a um
//  ponto no sistema de referencia do objeto (SRO).
// ***********************************************************
void InstanciaPonto(Ponto &p, Ponto &out)
{
    GLfloat ponto_novo[4];
    GLfloat matriz_gl[4][4];
    int  i;
    
    glGetFloatv(GL_MODELVIEW_MATRIX,&matriz_gl[0][0]);
    
    for(i=0;i<4;i++)
    {
        ponto_novo[i]= matriz_gl[0][i] * p.x+
        matriz_gl[1][i] * p.y+
        matriz_gl[2][i] * p.z+
        matriz_gl[3][i];
    }
    out.x=ponto_novo[0];
    out.y=ponto_novo[1];
    out.z=ponto_novo[2];
}

Ponto InstanciaPonto(Ponto P)
{
    Ponto temp;
    InstanciaPonto(P, temp);
    return temp;
}

InstanciaBZ::InstanciaBZ()
{
    
    Rotacao = 0;
    Posicao = Ponto(0,0,0);
    Escala = Ponto(1,1,1);
    
    nroDaCurva = 0;
    proxCurva = -1;
    tAtual = 0.0;
    direcao = 1;
    metadeCurva = false;

}
InstanciaBZ::InstanciaBZ(Bezier *C)
{
    Rotacao = 0;
    Posicao = Ponto(0,0,0);
    Escala = Ponto(1,1,1);
    
    Curva = C;
    tAtual = 0;
    direcao = 1;
    metadeCurva = false;
}

void InstanciaBZ::desenha()
{
    // Escala.imprime("Escala: ");
    // cout << endl;
    // Aplica as transformacoes geometricas no modelo
    glPushMatrix();
        glTranslatef(Posicao.x, Posicao.y, 0);
        glRotatef(Rotacao, 0, 0, 1);
        glScalef(Escala.x, Escala.y, Escala.z);
        
        (*modelo)(); // desenha a instancia
        
    glPopMatrix();
}
Ponto InstanciaBZ::ObtemPosicao()
{
    // aplica as transformacoes geometricas no modelo
    // desenha a geometria do objeto
    
    glPushMatrix();
        glTranslatef(Posicao.x, Posicao.y, 0);
        glRotatef(Rotacao, 0, 0, 1);
        Ponto PosicaoDoPersonagem;
        Ponto Origem (0,0,0);
        InstanciaPonto(Origem, PosicaoDoPersonagem);
        //PosicaoDoPersonagem.imprime(); cout << endl;
    glPopMatrix();
    return PosicaoDoPersonagem;
}

bool InstanciaBZ::calculaColisao(Ponto p){
    double distancia = calculaDistancia(Posicao, p);
    if(distancia <= 0.2){
        return true;
    }
    return false;
}

void InstanciaBZ::AtualizaPosicao(float tempoDecorrido)
{

    Ponto vetorHorizontal = Ponto(1,0,0);

    Ponto ultimaPosicao = Posicao;

    float deslocamento = Velocidade * tempoDecorrido;
    float comprimentoCurva = Curva->ComprimentoTotalDaCurva;
    float deltaT = deslocamento/comprimentoCurva;
    if(direcao == 1){
        tAtual += deltaT;
    } else {
        tAtual -= deltaT;
    }
    Posicao = Curva->Calcula(tAtual);

    Ponto vetorResultado = Posicao - ultimaPosicao;

    vetorResultado.versor();
    double prodEscalar = ProdEscalar(vetorHorizontal, vetorResultado);

    double cosDelta = prodEscalar;
    double radianos = acos(cosDelta);
    double pi = 2 * acos(0.0);
    Rotacao = radianos * (180/pi);
    Rotacao += 90;
    if(vetorResultado.y <= 0){
        Rotacao = Rotacao * (-1);
        Rotacao -= 180;
    }

}

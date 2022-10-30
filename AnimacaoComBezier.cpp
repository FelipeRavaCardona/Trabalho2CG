// **********************************************************************
// PUCRS/Escola Polit�cnica
// COMPUTA��O GR�FICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <fstream>

using namespace std;

#ifdef WIN32
#include <windows.h>
#include <GL/glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include "Ponto.h"
#include "Poligono.h"
#include "InstanciaBZ.h"

#include "Temporizador.h"
#include "ListaDeCoresRGB.h"

Temporizador T;
double AccumDeltaT = 0;
Temporizador T2;

// Limites l�gicos da �rea de desenho
Ponto Min, Max;

bool desenha = false;

int nInstancias = 0;

float angulo = 0.0;

double nFrames = 0;
double TempoTotal = 0;

// Variaveis criadas
Ponto pontos[20];
unsigned int nPontos;

Bezier curvas[40];
unsigned int nCurvas;

Poligono personagem;
InstanciaBZ personagens[11];

// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0 / 30) // fixa a atualiza��o da tela em 30
    {
        AccumDeltaT = 0;
        angulo += 2;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: " << TempoTotal << " segundos. ";
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames / TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
// **********************************************************************
void reshape(int w, int h)
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x, Max.x, Min.y, Max.y, -10, +10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
// **********************************************************************
void DesenhaEixos()
{
    Ponto Meio;
    Meio.x = (Max.x + Min.x) / 2;
    Meio.y = (Max.y + Min.y) / 2;
    Meio.z = (Max.z + Min.z) / 2;

    glBegin(GL_LINES);
    //  eixo horizontal
    glVertex2f(Min.x, Meio.y);
    glVertex2f(Max.x, Meio.y);
    //  eixo vertical
    glVertex2f(Meio.x, Min.y);
    glVertex2f(Meio.x, Max.y);
    glEnd();
}

// **********************************************************************
void desenhaPersonagemPrincipal(){
    glLineWidth(3);
    glPushMatrix();
    defineCor(BrightGold);
    personagem.desenhaPoligono();
    glPopMatrix();
}
void desenhaPersonagemInimigo(){
    glLineWidth(3);
    glPushMatrix();
    defineCor(Green);
    personagem.desenhaPoligono();
    glPopMatrix();
}
// **********************************************************************
// Esta fun��o deve instanciar todos os personagens do cen�rio
// **********************************************************************
void CriaInstancias(int numInstancias)
{
    nInstancias = numInstancias;

    personagens[0].Rotacao = 0;
    personagens[0].cor = BrightGold;
    personagens[0].modelo = desenhaPersonagemPrincipal;
    personagens[0].Curva = &curvas[0];
    personagens[0].proxCurva = -1;
    personagens[0].nroDaCurva = 0;
    personagens[0].Velocidade = 1;
    personagens[0].direcao = 1;

    for(int i = 1; i <= nInstancias; i++){
        personagens[i].Rotacao = 0;
        personagens[i].cor = Green;
        personagens[i].modelo = desenhaPersonagemInimigo;
        personagens[i].Curva = &curvas[i];
        personagens[i].proxCurva = -1;
        personagens[i].nroDaCurva = i;
        personagens[i].Velocidade = 1;
        personagens[i].direcao = 1;
    }
}
// **********************************************************************
//
// **********************************************************************
void CarregaModelos()
{
    personagem.LePoligono("Personagem.txt");
}

void CarregaPontos(const char *nome){
    ifstream input;            // ofstream arq;
    input.open(nome, ios::in); //arq.open(nome, ios::out);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Lendo arquivo " << nome << "...";
    string S;
    //int nLinha = 0;
    
    input >> nPontos;  // arq << qtdVertices

    for (int i=0; i< nPontos; i++)
    {
        double x,y;
        // Le cada elemento da linha
        input >> x >> y; // arq << x  << " " << y << endl
        if(!input)
            break;
        //nLinha++;
        pontos[i] = Ponto(x,y,0);
    }
    cout << "Pontos lidos com sucesso!" << endl;
}

void CarregaCurvas(const char *nome){

    ifstream input;            // ofstream arq;
    input.open(nome, ios::in); //arq.open(nome, ios::out);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Lendo arquivo " << nome << "...";
    string S;
    //int nLinha = 0;
    
    input >> nCurvas;  // arq << qtdVertices

    for (int i=0; i< nCurvas; i++)
    {
        double x,y,z;
        // Le cada elemento da linha
        input >> x >> y >> z; // arq << x  << " " << y << endl
        if(!input)
            break;
        //nLinha++;
        // insereVertice(Ponto(x,y));
        int xInt = int (x);
        int yInt = int (y);
        int zInt = int (z);
        curvas[i] = Bezier(pontos[xInt], pontos[yInt], pontos[zInt]);
        curvas[i].cor = Blue;
    }
    cout << "Curvas lidas com sucesso!" << endl;

}
// **********************************************************************
//
// **********************************************************************
void init()
{
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    CarregaPontos("PontosDeTeste.txt");
    CarregaCurvas("CurvasDeTeste.txt");

    CarregaModelos();
    CriaInstancias(3);

    float d = 5;
    Min = Ponto(-d, -d);
    Max = Ponto(d, d);
}

// **********************************************************************
void trocaProximaCurvaPersonagemPrincipal(InstanciaBZ *jogador){
    jogador->metadeCurva = true;
    Ponto pontoFinal;
    int possiveisCurvas[40];
    int contador = 0;
    if(jogador->direcao == 1){
        pontoFinal = jogador->Curva->getPC(2);
    } else {
        pontoFinal = jogador->Curva->getPC(0);
    }
    for(int i = 0; i < nCurvas; i++){
        if(jogador->nroDaCurva != i){
            if(curvas[i].getPC(0).isSame(pontoFinal)){
                possiveisCurvas[contador++] = i;
            }
            if(curvas[i].getPC(2).isSame(pontoFinal)){
                possiveisCurvas[contador++] = i;
            }
        }
    }

    int range = (contador - 1) - 0 + 1;
    int num = rand() % range + 0;
    jogador->proxCurva = possiveisCurvas[num];
    curvas[jogador->proxCurva].cor = Red;
}

void trocaCurvaAtualPersonagemPrincipal(InstanciaBZ *jogador){
    jogador->metadeCurva = false;
    if(curvas[jogador->proxCurva].getPC(0).isSame(jogador->Curva->getPC(2))){
        jogador->direcao = 1;
    } else if(curvas[jogador->proxCurva].getPC(2).isSame(jogador->Curva->getPC(2))){
        jogador->direcao = 0;
    } else if(curvas[jogador->proxCurva].getPC(0).isSame(jogador->Curva->getPC(0))){
        jogador->direcao = 1;
        cout << "trocou direção pra 1 no caso da curva 0" << endl;
    }
    jogador->Curva = &curvas[jogador->proxCurva];
    jogador->nroDaCurva = jogador->proxCurva;
    jogador->proxCurva = -1;
    if(jogador->direcao == 1){
        jogador->tAtual = 0;
    } else {
        jogador->tAtual = 1;
    }
    curvas[jogador->nroDaCurva].cor = Blue;
}

// **********************************************************************
void trocaProximaCurva(InstanciaBZ *jogador){
        jogador->metadeCurva = true;
    Ponto pontoFinal;
    int possiveisCurvas[40];
    int contador = 0;
    if(jogador->direcao == 1){
        pontoFinal = jogador->Curva->getPC(2);
    } else {
        pontoFinal = jogador->Curva->getPC(0);
    }
    for(int i = 0; i < nCurvas; i++){
        if(jogador->nroDaCurva != i){
            if(curvas[i].getPC(0).isSame(pontoFinal)){
                possiveisCurvas[contador++] = i;
            }
            if(curvas[i].getPC(2).isSame(pontoFinal)){
                possiveisCurvas[contador++] = i;
            }
        }
    }

    int range = (contador - 1) - 0 + 1;
    int num = rand() % range + 0;
    jogador->proxCurva = possiveisCurvas[num];
}

void trocaCurvaAtual(InstanciaBZ *jogador){
    jogador->metadeCurva = false;
    if(curvas[jogador->proxCurva].getPC(0).isSame(jogador->Curva->getPC(2))){
        jogador->direcao = 1;
    } else if(curvas[jogador->proxCurva].getPC(2).isSame(jogador->Curva->getPC(2))){
        jogador->direcao = 0;
    } else if(curvas[jogador->proxCurva].getPC(0).isSame(jogador->Curva->getPC(0))){
        jogador->direcao = 1;
    }
    jogador->Curva = &curvas[jogador->proxCurva];
    jogador->nroDaCurva = jogador->proxCurva;
    jogador->proxCurva = -1;
    if(jogador->direcao == 1){
        jogador->tAtual = 0;
    } else {
        jogador->tAtual = 1;
    }
}

// **********************************************************************
void DesenhaPersonagens(float tempoDecorrido)
{
    if(desenha){
        personagens[0].AtualizaPosicao(tempoDecorrido);
    }
    personagens[0].desenha();
    if(personagens[0].direcao == 1){
        if(personagens[0].tAtual >= 0.5 && personagens[0].metadeCurva == false){
            trocaProximaCurvaPersonagemPrincipal(&personagens[0]);
        }
        if(personagens[0].tAtual >= 1){
            trocaCurvaAtualPersonagemPrincipal(&personagens[0]);
        }
    }

    else{
        if(personagens[0].tAtual <= 0.5 && personagens[0].metadeCurva == false){
            trocaProximaCurvaPersonagemPrincipal(&personagens[0]);
        }
        if(personagens[0].tAtual <= 0){
            trocaCurvaAtualPersonagemPrincipal(&personagens[0]);
        }
    }
    for(int i = 1; i < nInstancias; i++){
        personagens[i].AtualizaPosicao(tempoDecorrido);
        personagens[i].desenha();
        if(personagens[i].direcao == 1){
            if(personagens[i].tAtual >= 0.5 && personagens[i].metadeCurva == false){
                trocaProximaCurva(&personagens[i]);
            }
            if(personagens[i].tAtual >= 1){
                trocaCurvaAtual(&personagens[i]);
            }
        } else {
            if(personagens[i].tAtual <= 0.5 && personagens[i].metadeCurva == false){
                trocaProximaCurva(&personagens[i]);
            }
            if(personagens[i].tAtual <= 0){
                trocaCurvaAtual(&personagens[i]);
            }
        }
    }
}

// **********************************************************************
//
// **********************************************************************
void DesenhaCurvas()
{
    for (int i = 0; i < nCurvas; i++)
    {
        glPushMatrix();
        defineCor(curvas[i].cor);
        curvas[i].Traca();
        glPopMatrix();
    }
}
// **********************************************************************
//  void display( void )
// **********************************************************************
void display(void)
{

    // Limpa a tela coma cor de fundo
    glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites l�gicos da �rea OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // Coloque aqui as chamadas das rotinas que desenham os objetos
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

    glLineWidth(1);
    glColor3f(1, 1, 1); // R, G, B  [0..1]

    DesenhaEixos();

    DesenhaCurvas();
    DesenhaPersonagens(T2.getDeltaT());
    
    glutSwapBuffers();
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo n�mero de segundos e informa quanto frames
// se passaram neste per�odo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while (true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }
}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
// **********************************************************************
void keyboard(unsigned char key, int x, int y)
{

    switch (key)
    {
    case 27:     // Termina o programa qdo
        exit(0); // a tecla ESC for pressionada
        break;
    case 't':
        ContaTempo(3);
        break;
    case ' ':
        desenha = !desenha;
        break;
    case 'f':
        personagens[0].direcao = 1 - personagens[0].direcao;
        break;
    default:
        break;
    }
}
// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
// **********************************************************************
void arrow_keys(int a_keys, int x, int y)
{
    switch (a_keys)
    {
    case GLUT_KEY_LEFT:
        // personagens[0].Posicao.x -= 0.5;
        personagens[0].Rotacao++;
        break;
    case GLUT_KEY_RIGHT:
        personagens[0].Rotacao--;
        break;
    case GLUT_KEY_UP:     // Se pressionar UP
        glutFullScreen(); // Vai para Full Screen
        break;
    case GLUT_KEY_DOWN: // Se pressionar UP
                        // Reposiciona a janela
        glutPositionWindow(50, 50);
        glutReshapeWindow(700, 500);
        break;
    default:
        break;
    }
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int main(int argc, char **argv)
{
    cout << "Programa OpenGL" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize(650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de t�tulo da janela.
    glutCreateWindow("Animacao com Bezier");

    // executa algumas inicializa��es
    init();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser� chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc(display);

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser� chamada automaticamente quando
    // o usu�rio alterar o tamanho da janela
    glutReshapeFunc(reshape);

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser� chamada automaticamente sempre
    // o usu�rio pressionar uma tecla comum
    glutKeyboardFunc(keyboard);

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser� chamada
    // automaticamente sempre o usu�rio
    // pressionar uma tecla especial
    glutSpecialFunc(arrow_keys);

    // inicia o tratamento dos eventos
    glutMainLoop();

    return 0;
}

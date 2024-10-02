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

InstanciaBZ Personagens[10];

Bezier Curvas[20];
unsigned int nCurvas;

// Limites l�gicos da �rea de desenho
Ponto Min, Max;

bool desenha = false;

Poligono Mapa, MeiaSeta, Mastro;
int nInstancias = 0;

float angulo = 0.0;

double nFrames = 0;
double TempoTotal = 0;


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
        //Personagens[1].Rotacao++;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
   /*      cout << "Tempo Acumulado: " << TempoTotal << " segundos. ";
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames / TempoTotal << endl; */
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

void DesenhaPersonagem()
{
    defineCor(YellowGreen);
    glTranslatef(53,33,0);
    Mapa.desenhaPoligono();
}
// **********************************************************************
// void DesenhaTriangulo()
// **********************************************************************
void DesenhaTriangulo()
{
    glBegin(GL_TRIANGLES);
        glVertex2f(-2,-2);
        glVertex2f(0, 2);
        glVertex2f(2,-2);
    glEnd();
}

void DesenhaFanstasma()
{
    
}
// **********************************************************************
// Esta funcao deve instanciar todos os personagens do cenario
// **********************************************************************
void CriaInstancias()
{
    Personagens[0].posicao = Ponto(0, 0);
    Personagens[0].Rotacao = 0;
    defineCor(BlueViolet);
    Personagens[0].cor = Maroon;
    Personagens[0].modelo = []() {
        defineCor(Personagens[0].cor);
        glBegin(GL_TRIANGLES);
        glVertex2f(-2,-2);
        glVertex2f(0, 2);
        glVertex2f(2,-2);
        glEnd();
    };
    Personagens[0].Escala = Ponto(1, 1, 1);
    
    
    Personagens[1].posicao = Ponto(5, 5);
    Personagens[1].Rotacao = 0;
    Personagens[1].cor = Gold;
    Personagens[1].modelo = []() {
        defineCor(Personagens[1].cor);
        glBegin(GL_TRIANGLES);
        glVertex2f(-2,-2);
        glVertex2f(0, 2);
        glVertex2f(2,-2);
        glEnd();
    };;
    Personagens[1].Escala = Ponto(1, 1, 1);
    
    nInstancias = 1;

}
// **********************************************************************
//  Carrega os modelos que poderão representar os personagens
// **********************************************************************
void CarregaModelos()
{
    Mapa.LePoligono("EstadoRS.txt");
    MeiaSeta.LePoligono("MeiaSeta.txt");
    Mastro.LePoligono("Mastro.txt");
    
    Ponto A, B;
    Mapa.obtemLimites(A,B);
    cout << "Limites do Mapa" << endl;
    A.imprime();
    cout << endl;
    B.imprime();
    cout << endl;
}
// **********************************************************************
//  Este metodo deve ser alterado para ler as curvas de um arquivo texto
// **********************************************************************
void CriaCurvas()
{
    //Curvas[0] = Bezier(Ponto(-5, -5), Ponto(0, 6), Ponto(5, -5));
    //Curvas[1] = Bezier(Ponto(5, -5), Ponto(15, 0), Ponto(12, 12));
    Curvas[0] = Bezier(Ponto(-10, -5), Ponto(-15, 15), Ponto(12, 12));
    Curvas[1] = Bezier(Ponto(12, 12), Ponto(15, -15), Ponto(-12, -12));

    nCurvas = 2;
}
// **********************************************************************
//
// **********************************************************************
void AssociaPersonagemComCurva(int p, int c, int direcao)
{
    Personagens[p].Curva = Curvas[c];
    Personagens[p].nroDaCurva = c;
    Personagens[p].tAtual = 0.0;
    Personagens[p].direcao = direcao;
    
}
// **********************************************************************
//
// **********************************************************************
void init()
{
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

    // carrega os modelos armazenados em arquivos
    CarregaModelos();
    
    // cria instancias do modelos
    CriaInstancias();

    // carrega as curvas que farao parte do cenario
    CriaCurvas();
    
    AssociaPersonagemComCurva(0, 0,1);
    //AssociaPersonagemComCurva(1, 1);

    // define is limites da área de desenho
    float d = 15;
    Min = Ponto(-d, -d);
    Max = Ponto(d, d);
}

bool isDiferentCurve(Bezier b1, Bezier b2)
{
    if (b1.Coords[0] == b2.Coords[0]) return false;
    if (b1.Coords[1] == b2.Coords[1]) return false;
    if (b1.Coords[2] == b2.Coords[2]) return false;
    return true;

}

void TrocarCurva(int indicePersonagem) 
{
 
    InstanciaBZ personagem = Personagens[indicePersonagem];
    Ponto posicao =  personagem.posicao;
    
    // se chegou ao final/inicio da curva atual
    if (personagem.tAtual == 0.0 || personagem.tAtual == 1) {
        for (int i = 0; i < nCurvas; i ++) {
            // se nao for a curva que ele ja esta
            if (i != personagem.nroDaCurva) {

                // se outra curva comeca/termina naquele ponto
                if (posicao == Curvas[i].Coords[0] || posicao == Curvas[i].Coords[2]) {
                        AssociaPersonagemComCurva(indicePersonagem, i, 1);
                        Personagens[indicePersonagem].proxCurva = 0;
                    
                } else {
                    if (Personagens[indicePersonagem].proxCurva != -1) {
                         cout << "terminei o fluxo" << endl;
                         Personagens[indicePersonagem].proxCurva = -1;
                         Personagens[indicePersonagem].direcao = -1;

                    }
                }
                
            }
        }
    } 
}

// **********************************************************************
void DesenhaPersonagens(float tempoDecorrido)
{
    for (int i = 0; i < nInstancias; i++)
    {
        TrocarCurva(i);
        Personagens[i].AtualizaPosicao(tempoDecorrido);
        Personagens[i].desenha(); 
    }
}

void DesenhaPoligonoDeControle(int curva)
{
    Ponto P;
    glBegin(GL_LINE_STRIP);
    for (int v=0;v<3;v++)
    {
        P = Curvas[curva].getPC(v);
        glVertex2d(P.x, P.y);
    }
    glEnd();
}
// **********************************************************************
//
// **********************************************************************
void DesenhaCurvas()
{
    for (int i = 0; i < nCurvas; i++)
    {
        defineCor(Personagens[i].cor);
        glLineWidth(4);
        Curvas[i].Traca();
     
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

    //AtualizaCenario();
    DesenhaCurvas();
    
    // Desenha os personagens no tempo T2.getDeltaT()
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
    Ponto P;
    switch (key)
    {
        case 'a':
                
                P = Curvas[1].Calcula(0);
                P.imprime("Pra t==0 :");
                break;

    case 27:     // Termina o programa qdo
        exit(0); // a tecla ESC for pressionada
        break;
    case 't':
        ContaTempo(3);
        break;
    case ' ':
        desenha = !desenha;
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
            Personagens[1].Rotacao+=2;
        break;
    case GLUT_KEY_RIGHT:
        Personagens[1].Rotacao-=2;
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

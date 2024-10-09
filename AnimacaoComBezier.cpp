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
#include <string>
#include <time.h>



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

InstanciaBZ Personagens[11];
int nInstancias = 11; // 10 inimigos + principal

Bezier Curvas[20];
unsigned int nCurvas;

// Limites l�gicos da �rea de desenho
Ponto Min, Max;

bool desenha = false;

Poligono Mapa;

double nFrames = 0;
double TempoTotal = 0;
char teclaDigitada = 'w';
time_t momentoColisao;


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
        //Personagens[1].Rotacao++;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
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
       glVertex2f(-0.2,-0.2);
        glVertex2f(0, 0.2);
        glVertex2f(0.2,-0.2);
    glEnd();
}

void AssociaPersonagemComCurva(int p, int c, bool isInicio)
{
    Personagens[p].Curva = Curvas[c];
    Personagens[p].nroDaCurva = c;
    Personagens[p].tAtual = isInicio ? 0.0 : 1;
    Personagens[p].direcao = isInicio ? 1 : -1;
}
 
// **********************************************************************
//  Carrega os modelos que poderão representar os personagens
// **********************************************************************
void CarregaModelos()
{
    // le as curvas do arquivo
    string fileNamePontos = "example1.txt";
    string fileNameCurvas = "example1-curves.txt";
    nCurvas = Mapa.obterCurvas(fileNamePontos, Curvas);
    Ponto A, B;
    Mapa.obtemLimites(A,B);
    cout << "Limites do Mapa" << endl;
    A.imprime();
    cout << endl;
    B.imprime();
    cout << endl;
}

void desenharPersonagens()
{  
    for (int i = 0; i < nInstancias; i++) {
        int numeroRand = (rand() % (nInstancias)) + 1;
        // Personagens[0] == protagonista
        if (i == 0) {
            // protagonista sempre tera cor dourada
            Personagens[0].cor = Gold;
            Personagens[0].modelo = [](int cor) {
               // muda a cor por um periodo se colidiu
               time_t tempoAtual; 
               if (difftime(time(&tempoAtual), momentoColisao) > 0.5) {
                    defineCor(cor);
                } else {
                    defineCor(White);
                }
               DesenhaTriangulo();
            };
            AssociaPersonagemComCurva(0, 0, 1);
        } else {
            Personagens[i].cor = numeroRand + 10 * i/2;
            Personagens[i].modelo = [](int cor) {
            defineCor(cor);
            DesenhaTriangulo();
            };
            AssociaPersonagemComCurva(i, numeroRand, 1);
            // inimigos tem spaw no meio da curva
            Personagens[i].tAtual = 0.5;
            // cada metade em sentido oposto
            if (i >= 5) {
                Personagens[i].direcao = -1;
            } else {
                Personagens[i].direcao = 1;
            }
        }
    } 

}

void init()
{
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

    // carrega os modelos armazenados em arquivos
    CarregaModelos();
    
    // popula os triangulos
    desenharPersonagens();
   

    // define is limites da área de desenho
    float d = 5;
    Min = Ponto(-d, -d);
    Max = Ponto(d, d);
}

void trocarCurva(int indicePersonagem) 
{   
    // curvas candidatas ao personagem seguir
    int curvasEncontradas[20];
    int nCurvasEncontradas = 0;
    // sinaliza se o ponto encontrado da curva é final ou inicial
    bool direcoesCurvas[20];

    // salva a curva que o personagem teve spawn
    if (Personagens[indicePersonagem].curvaInicial == -1) {
        Personagens[indicePersonagem].curvaInicial = Personagens[indicePersonagem].nroDaCurva;
    }

    InstanciaBZ personagem = Personagens[indicePersonagem];
    Ponto posicao =  personagem.posicao;
    
    // se chegou ao final/inicio da curva atual
    if (personagem.tAtual == 0.0 || personagem.tAtual == 1) {
        for (int i = 0; i < nCurvas; i ++) {
            // se nao for a curva que ele ja esta
            if (i != personagem.nroDaCurva) {

                // se outra curva comeca/termina naquele ponto
                if (posicao == Curvas[i].Coords[0] || posicao == Curvas[i].Coords[2]) {
                       bool isInicio = false;
                       isInicio = posicao == Curvas[i].Coords[0];
                       curvasEncontradas[nCurvasEncontradas] = i;
                       direcoesCurvas[nCurvasEncontradas] = isInicio;
                       nCurvasEncontradas++;
                }
            } 
        }

        // escolhe aleatoriamente uma curva entre as candidatas
        if (nCurvasEncontradas) {
            int numRand = rand() % nCurvasEncontradas;
            AssociaPersonagemComCurva(indicePersonagem, curvasEncontradas[numRand], direcoesCurvas[numRand]);
            return;
        } 
        
        // caso esteja em alguma extremidade e não haja curva a se direcionar
        if (Personagens[indicePersonagem].tAtual == 0) {
            Personagens[indicePersonagem].direcao = 1;
        } else {
            Personagens[indicePersonagem].direcao = -1;
        }
    } 
}

bool houveColisao(int indiceColisor) {
    if (Personagens[indiceColisor].nroDaCurva == Personagens[0].nroDaCurva) {
        Ponto p = Personagens[0].posicao - Personagens[indiceColisor].posicao;
        // arredonda para considerar pontas do triangulo
        p.x = round(p.x);
        p.y = round(p.y);
        return p.x == 0 && p.y == 0;
    }
    return 0;
}

// **********************************************************************
void DesenhaPersonagens(float tempoDecorrido)
{
    for (int i = 0; i < nInstancias; i++)
    {
        trocarCurva(i);
        Personagens[i].Curva.calculaComprimentoDaCurva();
        Personagens[i].AtualizaPosicao(tempoDecorrido);        
        Personagens[i].desenha(); 
        if (i != 0) {
            if (houveColisao(i)) {
                cout << "colidiu com o personagem num =" << i << endl;
                time(&momentoColisao);
            }
        }

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

void DesenhaCurvas()
{
    for (int i = 0; i < nCurvas; i++)
    {
        defineCor(Curvas[i].cor);
        glLineWidth(4);
        Curvas[i].Traca();
     
    }
}

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
        Personagens[0].tAtual= 0.0;
        AssociaPersonagemComCurva(0, Personagens[0].curvaInicial, 1);
        desenha = !desenha;
        break;
    case 's':
        if (teclaDigitada != 's') {
           Personagens[0].direcao *= -1;
           teclaDigitada = 's';
           Personagens[0].Rotacao+=180;
        }  
        break;
    case 'w':
        if (teclaDigitada != 'w') {
        Personagens[0].direcao *= -1;
        teclaDigitada = 'w';
        Personagens[0].Rotacao+=180;
        }
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
    srand(time(0));
    cout << "Programa OpenGL" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize(1920, 1080);

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

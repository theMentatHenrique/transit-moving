//
//  Poligono.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 18/08/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//
#include <iostream>
#include <fstream>
using namespace std;

#include "Poligono.h"

Poligono::Poligono()
{
    
}

void Poligono::insereVertice(Ponto p)
{
    Vertices.push_back(p);
}

void Poligono::insereVertice(Ponto p, int pos)
{
    if ((pos < 0) || (pos>Vertices.size()))
    {
        cout << "Metodo " << __FUNCTION__ << ". Posicao Invalida. Vertice nao inserido." << endl;
        return;
    }
    Vertices.insert(Vertices.begin()+pos, p);
}

Ponto Poligono::getVertice(int i)
{
    return Vertices[i];
}

void Poligono::pintaPoligono()
{
    glBegin(GL_POLYGON);
    for (int i=0; i<Vertices.size(); i++)
        glVertex3f(Vertices[i].x,Vertices[i].y,Vertices[i].z);
    glEnd();
}

void Poligono::desenhaPoligono()
{
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<Vertices.size(); i++)
        glVertex3f(Vertices[i].x,Vertices[i].y,Vertices[i].z);
    glEnd();
}
void Poligono::desenhaVertices()
{
    glBegin(GL_POINTS);
    for (int i=0; i<Vertices.size(); i++)
        glVertex3f(Vertices[i].x,Vertices[i].y,Vertices[i].z);
    glEnd();
}
void Poligono::imprime()
{
    for (int i=0; i<Vertices.size(); i++)
        Vertices[i].imprime();
}
unsigned long Poligono::getNVertices()
{
    return Vertices.size();
}

void Poligono::obtemLimites(Ponto &Min, Ponto &Max)
{
    Max = Min = Vertices[0];
    
    for (int i=0; i<Vertices.size(); i++)
    {
        Min = ObtemMinimo (Vertices[i], Min);
        Max = ObtemMaximo (Vertices[i], Max);
    }
}

// **********************************************************************
//
// **********************************************************************
void Poligono::LePoligono(const char *nome)
{
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
    unsigned int qtdVertices;
    
    input >> qtdVertices;  // arq << qtdVertices

    for (int i=0; i< qtdVertices; i++)
    {
        double x,y;
        // Le cada elemento da linha
        input >> x >> y; // arq << x  << " " << y << endl
        if(!input)
            break;
        //nLinha++;
        insereVertice(Ponto(x,y));
    }
    cout << "Poligono lido com sucesso!" << endl;

}

int Poligono ::obterCurvas(string fileNamePontos,string fileNameCurvas, Bezier curvas[]) {
    ifstream input;           
    input.open(fileNamePontos, ios::in); 
    if (!input)
    {
        cout << "Erro ao abrir " << fileNamePontos << ". " << endl;
        exit(0);
    }
    cout << "Lendo arquivo " << fileNamePontos << "...";
    string S;
    unsigned int qtdPontos;
    
    input >> qtdPontos; 
    Ponto pontos[qtdPontos];
    for (int i=0; i< qtdPontos; i++)
    {
        double x,y;
        input >> x >> y; 

        pontos[i].x = x;
        pontos[i].y = y;
        if(!input)
            break;

        insereVertice(pontos[i]);

    }
    input.close();


    input.open(fileNameCurvas, ios::in); //arq.open(nome, ios::out);
    if (!input)
    {
        cout << "Erro ao ler " << fileNameCurvas << ". " << endl;
        exit(0);
    }
    cout << "Lendo arquivo " << fileNameCurvas << "...";
    //int nLinha = 0;
    unsigned int qtdCurvas;
    
    input >> qtdCurvas; 
    srand(time(NULL));
    for (int i= 0; i< qtdCurvas; i++)
    {
        char a,b, c;
        input >> a >>  b >> c; 
        curvas[i] = Bezier(pontos[a -'0'], pontos[b -'0'], pontos[c -'0']);


        // Gera um número randômico entre 0 e 95
        int numero = rand() % 96;
        curvas[i].cor = numero;
        if(!input)
            break;

    } 
    
    return qtdCurvas;

}

void Poligono::getAresta(int n, Ponto &P1, Ponto &P2)
{
    P1 = Vertices[n];
    int n1 = (n+1) % Vertices.size();
    P2 = Vertices[n1];
}

void Poligono::desenhaAresta(int n)
{
    glBegin(GL_LINES);
        glVertex3f(Vertices[n].x,Vertices[n].y,Vertices[n].z);
        int n1 = (n+1) % Vertices.size();
        glVertex3f(Vertices[n1].x,Vertices[n1].y,Vertices[n1].z);
    glEnd();
}

void Poligono::alteraVertice(int i, Ponto P)
{
    Vertices[i] = P;
}

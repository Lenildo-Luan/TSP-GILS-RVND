#include "readData.h"
#include <random>
#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

typedef struct{
  int pos;
  int vertice;
  int custo;
} tInsercao;
typedef struct{
  int posVertice;
  int posInsercao;
  int vertice;
} tReinsercao;

typedef struct{
  int pos1;
  int vertice1;
  int pos2;
  int vertice2;
} tSwap;

// Pega da instâncias
double ** matrizAdj; // matriz de adjacencia
int dimension; // quantidade total de vertices

// Untils
void printData(); // Mostra matriz de adjacencia
void printSolucao(vector<int> &solucao, int tamanhoArray); // Mostra a solução inicial gerada pel algorítimo escolhido
void custoSolucao(int *custoTotal, vector<int> solucao, int tamanhoArray); // Mostra o custo da solução gerada
bool compareByCost(const tInsercao &data1, const tInsercao &data2);

//GILS-RVND
void construtivo(vector<int> &solucao, int coleta, int deposito);
void rvnd(vector<int> &solucao);

// Vizinhanças
void reinsertion(vector<int> &solucao, int blocoSize);
void swap(vector<int> &solucao);
void twoOptN(vector<int> &solucao){}

//MAIN
int main(int argc, char** argv){
    readData(argc, argv, &dimension, &matrizAdj);
    //printData();

    vector<int> solucao;
    int custoTotal, coleta, deposito;

    coleta = 1;
    deposito = 1;

    construtivo(solucao, coleta, deposito);
    //reinsertion(solucao, 1);
    swap(solucao);
   
    printSolucao(solucao, dimension);
    custoSolucao(&custoTotal, solucao, dimension);

    cout <<  "Custo: " << custoTotal << endl;

    return 0;
}

//GILS-RVND
void construtivo(vector<int> &solucao, int coleta, int deposito){
  // Inicia variáveis
  vector<int> verticesList; // Lista de vertices faltando
  vector<tInsercao> bestVerticesList; // Lista dos X melhores verticesList

  mt19937 mt(42); // Gerador de números aleatórios
  uniform_real_distribution<float> linear_f(0.0, 0.5); // Distribuição linear de reais para gerar alpha

  float alpha = 0.0;
  int rBest;
  int randomVertice1, randomVertice2, randomVertice3;
  int tamanhoSolucao;
  int verticesRestantes;
  int distanciaVertice;
  tInsercao insercao;
  //Adiciona coleta ao vector
  solucao.push_back(coleta);

  // Gera lista de vertices faltantes
  for (size_t i = 1; i <= dimension; i++) {
    if(i == coleta || i == deposito) continue;
    verticesList.push_back(i);
  }

  // Escolhe três vertices de forma aleatória
  for (size_t i = 0; i < 3; i++) {
    uniform_int_distribution<int> linear_i(0, verticesList.size()-1); // Distribuição linear de inteiros para escolher vertice inicial
    randomVertice1 = linear_i(mt);

    solucao.push_back(verticesList[randomVertice1]);
    verticesList.erase(verticesList.begin() + randomVertice1);
  }

  // Adiciona deposito ao vector
  solucao.push_back(deposito);

  // Gera solução
  while(1) {
    tamanhoSolucao = solucao.size();
    verticesRestantes = verticesList.size();
    distanciaVertice = 0;

    // Gera lista de custo de vertices
    for (size_t i = 0; i < verticesRestantes; i++) { // Itera sobre vértices restantes
      for (size_t j = 1; j < tamanhoSolucao; j++) { // Itera sobre a solução
        insercao.vertice = verticesList[i];
        insercao.pos = j;
        insercao.custo = (matrizAdj[solucao[j-1]][verticesList[i]] + matrizAdj[verticesList[i]][solucao[j]]) - matrizAdj[solucao[j-1]][solucao[j]];
        bestVerticesList.push_back(insercao);
      }
    }

    //Arruma lista pelo valor do custo
    sort(bestVerticesList.begin(), bestVerticesList.end(), compareByCost);

    // Gera Alfa
    alpha = linear_f(mt);

    // Adiciona novo vertice à solução conforme alpha
    uniform_int_distribution<int> linear_i_alpha(0, int((bestVerticesList.size()-1)*alpha));
    rBest = linear_i_alpha(mt);
    solucao.emplace(solucao.begin() + bestVerticesList[rBest].pos, bestVerticesList[rBest].vertice);
    
    // Reseta Vectors
    for (size_t i = 0; i < verticesRestantes; i++) {
      if(bestVerticesList[rBest].vertice == verticesList[i]) {
        verticesList.erase(verticesList.begin() + i);
        break;
      }
    }
    bestVerticesList.clear();

    // Se não tiverem mais candidatos o loop acaba
    if(verticesRestantes == 1) {
      break;
    }
  }

  // // Mostra solução
  // for (size_t i = 0; i < solucao.size(); i++){
  //   cout << solucao[i] << " ";
  //   //cout << solucaoInicial[i] << " ";
  // }

}
void rvnd(vector<int> &solucao){

}

// Vizinhanças
void reinsertion(vector<int> &solucao, int blocoSize){
  // Inicia variáveis
  int deltaCusto = 0;
  int custoRetirada = 0;
  int custoInsercao = 0;
  int custoDaSolucao = 0;
  bool flag = false;
  
  tReinsercao insercao;

  custoSolucao(&custoDaSolucao, solucao, dimension);

  while(1){
    //Procura local de melhor reinserção
    for(size_t i = 1; i < solucao.size() - 1 - blocoSize; i++){
      custoRetirada = matrizAdj[solucao[i-1]][solucao[i+blocoSize]] - (matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[i+blocoSize-1]][solucao[i+blocoSize]]); 
      
      for(size_t y = i+blocoSize; y < solucao.size() - blocoSize; y++){
        custoInsercao = (matrizAdj[solucao[y]][solucao[i]] + matrizAdj[solucao[i+blocoSize-1]][solucao[y+1]]) - matrizAdj[solucao[y]][solucao[y+1]]; 

        if((custoRetirada + custoInsercao) < deltaCusto){
          flag = true;

          deltaCusto = custoInsercao + custoRetirada;
          insercao.posVertice = i;
          insercao.posInsercao = y+1;
          insercao.vertice = solucao[i];
        }
      }
    }

    //Aplica reinserção
    if(flag){
      custoDaSolucao = custoDaSolucao + deltaCusto;

      if(blocoSize == 1){
        solucao.insert(solucao.begin() + insercao.posInsercao, solucao[insercao.posVertice]);
        solucao.erase(solucao.begin() + insercao.posVertice);
      } else {
        solucao.insert(solucao.begin() + insercao.posInsercao, solucao.begin() + insercao.posVertice, solucao.begin() + insercao.posVertice + blocoSize);
        //printSolucao(solucao, solucao.size());
        solucao.erase(solucao.begin() + insercao.posVertice, solucao.begin() + insercao.posVertice + blocoSize);        
      }

      flag = false;
      deltaCusto = 0;

    } else {
      break;
    }
  }  

}

void swap(vector<int> &solucao){
  //Inicia variáveis
  int deltaCusto = 0;
  int custoRetirada = 0;
  int custoInsercao = 0;
  int custoDaSolucao = 0;
  bool flag = false;

  tSwap swap;

  custoSolucao(&custoDaSolucao, solucao, dimension);

  while(1){
    //Aplica reinserção
    for(size_t i = 1; i < solucao.size() - 3; i++){      
      for(size_t y = i+2; y < solucao.size()-1; y++){
        custoRetirada = matrizAdj[solucao[i-1]][solucao[i]] + matrizAdj[solucao[i]][solucao[i+1]] +
                        matrizAdj[solucao[y-1]][solucao[y]] + matrizAdj[solucao[y]][solucao[y+1]]; 

        custoInsercao = matrizAdj[solucao[i-1]][solucao[y]] + matrizAdj[solucao[y]][solucao[i+1]] +
                        matrizAdj[solucao[y-1]][solucao[i]] + matrizAdj[solucao[i]][solucao[y+1]]; 

        if((custoInsercao - custoRetirada) < deltaCusto){
          flag = true;

          deltaCusto = custoInsercao - custoRetirada;
          swap.pos1 = i;
          swap.vertice1 = solucao[i];
          swap.pos2 = y;
          swap.vertice2 = solucao[y];
        }
      }
    }

    if(flag){
      custoDaSolucao = custoDaSolucao + deltaCusto;

      solucao.erase(solucao.begin() + swap.pos2);
      solucao.emplace(solucao.begin() + swap.pos2, swap.vertice1);

      solucao.erase(solucao.begin() + swap.pos1);
      solucao.emplace(solucao.begin() + swap.pos1, swap.vertice2);

      flag = false;
      deltaCusto = 0;

    } else {
      break;

    }
  }
}

void twoOptN(vector<int> &solucao){
  //Inicia variáveis
  int deltaCusto = 0;
  int custoRetirada = 0;
  int custoInsercao = 0;
  int custoDaSolucao = 0;
  bool flag = false;

  tSwap swap;

  custoSolucao(&custoDaSolucao, solucao, dimension);

  while(1){
    for(size_t i = 1; i < solucao.size() - 3; i++){
      for(size_t y = i + 2; y < solucao.size(); i++){
        custoRetirada = matrizAdj[solucao[i]][solucao[i+1]] + matrizAdj[solucao[y-1]][solucao[y]];
        custoInsercao = matrizAdj[solucao[i]][solucao[y-1]] + matrizAdj[solucao[i+1]][solucao[y]];

        if((custoInsercao - custoRetirada) < deltaCusto){
          deltaCusto = custoInsercao - custoRetirada;

          
        }
      }
    }
  }
}

// Untils
void printData() {
  cout << endl << "dimension: " << dimension << endl;
  for (size_t i = 1; i <= dimension; i++) {
    for (size_t j = 1; j <= dimension; j++) {
      cout << matrizAdj[i][j] << " ";
    }
    cout << endl;
  }
}

void printSolucao(vector<int> &solucao, int tamanhoArray){
  cout << endl << "Solucao: [ ";

  for(size_t i = 0; i < solucao.size(); i++){
    cout << solucao[i] << " ";
  }

  cout << "]" << endl;
}

void custoSolucao(int *custoTotal, vector<int> solucao, int tamanhoArray) {
  *custoTotal = 0;

  for(size_t i = 0; i < solucao.size()-1; i++){
    *custoTotal += matrizAdj[solucao[i]][solucao[i + 1]];
  }
}

bool compareByCost(const tInsercao &data1, const tInsercao &data2){
  return data1.custo < data2.custo;
}

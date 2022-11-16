#!/bin/bash
echo "Algoritmo para gerar um cenario em 3D e navegar pelo cenario atraves de um drone(ficticio): --------------------------------------"
echo ""

g++ cenario_Igor_Capeletti.cpp -o executavel -lGL -lGLU -lglut -lm      #compilacao do codigo em C++

#Se quiser informar todos os dados pelo terminal, executar: (cada arquivo.obj sera normalizado e limpado as "//" automaticamente)
#./executavel

#Passa como argumento um arquivo com os dados dos objetos: (ira normalizar os arquivos.obj e limpar as "//" automaticamente)
./executavel lista_objetos.txt normal
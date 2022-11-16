#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
//Compilar:
//g++ cenario_Igor_Capeletti.cpp -o executavel -lGL -lGLU -lglut -lm

//Executar:
//./executavel lista_objetos.txt normal
//ou 
//./executavel lista_objetos.txt
//ou
//./executavel

/*
Autor: Igor Ferrazza Capeletti
Disciplina: Computacao Grafica
Ano: 2021.2
Professor: Marcelo Thielo
*/

class Obj{
    private:
        int num_faces;
        float escala;
        float pos_x;
        float pos_y;
        float pos_z;
        float red;
        float green;
        float blue;
        string nome_obj;
        
        vector <float> vx;
        vector <float> vy;
        vector <float> vz;
        vector <int> f_v1;
        vector <int> f_v2;
        vector <int> f_v3;
        vector <float> normal1;
        vector <float> normal2;
        vector <float> normal3;

    public:
        Obj();
        ~Obj();
        void carrega_objeto_3d(string opcao_obj, string nome_arquivo);
        void printa_objeto_3d();
        void desenha_objeto_tela(int opcao);
        void seta_escala_objeto(float nova_escala);
        void seta_cor_objeto(int r, int g, int b);
        void seta_posicao_objeto(int x, int y, int z);
        void rotaciona_objeto_3d(float thetax, float thetay, float thetaz);
        string retorna_nome_objeto();
};
//metodo construtor do objeto
Obj::Obj(){

}

//metodo destrutor do objeto
Obj::~Obj(){
    vx.clear();
    vy.clear();
    vz.clear();
    f_v1.clear();
    f_v2.clear();
    f_v3.clear();
    normal1.clear();
    normal2.clear();
    normal3.clear();
}


//------------ variaveis globais ------------
int modo_camera;        //0 = modo de drone sem cabeca, 1= modo orientado para onde a camera aponta
vector <Obj*> lista_objetos;

//variaveis para utilizar na camera e tambem para rotacao e translacao dos objetos no cenario
float x_pos_atual, y_pos_atual, z_pos_atual, x_ap_atual, y_ap_atual, z_ap_atual;
float x_pos_cam, y_pos_cam, z_pos_cam;
float x_ap_cam, y_ap_cam, z_ap_cam;
float alpha, theta, angulox;

//variaveis da iluminacao ambiente
int opcao_luz_amb;
float red_amb, green_amb, blue_amb;
float x_luz_amb, y_luz_amb, z_luz_amb;  //posicao da luz ambiente
GLfloat light0_ambient[] = {red_amb, green_amb, blue_amb, 0.2};    //intensidade das cores da luz0 ambiente
GLfloat light0_position[] = {x_luz_amb, y_luz_amb, z_luz_amb, 1.0 };
GLfloat globalAmbient[] = {0.4, 0.4, 0.4, 0.4};

//variaveis da iluminacao difusa
int opcao_luz_dif;
float red_dif, green_dif, blue_dif;
float x_luz_dif, y_luz_dif, z_luz_dif;  //posicao da luz difusa
GLfloat light2_diffuse[] = {red_dif, green_dif, blue_dif, 1.0};
GLfloat light2_position[] = {x_luz_dif, y_luz_dif, z_luz_dif , 1.0 }; //{0.0, 0.0, -50.0, 11.0};

//variaveis da iluminacao especular
int opcao_luz_esp;
float red_esp, green_esp, blue_esp;
float x_luz_esp, y_luz_esp, z_luz_esp;  //posicao da luz especular
GLfloat light1_specular[] = {red_esp, green_esp, blue_esp, 1.0};
GLfloat light1_position[] = {x_luz_esp, y_luz_esp, z_luz_esp, 1.0 };

//------------------------------------------------


void Obj::seta_escala_objeto(float nova_escala){
    escala= nova_escala;
}

void Obj::seta_posicao_objeto(int x, int y, int z){
    pos_x= x;
    pos_y= y;
    pos_z= z;
}

void Obj::seta_cor_objeto(int r, int g, int b){
    red= r;
    green= g;
    blue= b;
}


//metodo para girar objeto 3D em torno dos eixos X, Y e Z de acordo com valores recebidos (em radianos)
void Obj::rotaciona_objeto_3d(float thetax, float thetay, float thetaz){
    int i=0;
    float xt, yt, zt;
    if(thetay > 0.0){
        for(i=0; i < vx.size(); i++){   //rotaciona no eixo y
            xt= vx[i]*cos(thetay)-vz[i]*sin(thetay);
            zt= vx[i]*sin(thetay)+vz[i]*cos(thetay);
            vx[i]= xt;
            vz[i]= zt;
        }
    }
    if(thetax > 0.0){
        for(i=0; i < vx.size(); i++){   //rotaciona no eixo x
            yt= vy[i]*cos(thetax)-vz[i]*sin(thetax);
            zt= vy[i]*sin(thetax)+vz[i]*cos(thetax);
            vy[i]= yt;
            vz[i]= zt;
        }
    }
    if(thetaz > 0.0){
        for(i=0; i < vx.size(); i++){   //rotaciona no eixo z
            xt= vx[i]*cos(thetaz)-vy[i]*sin(thetaz);
            yt= vx[i]*sin(thetaz)+vy[i]*cos(thetaz);
            vx[i]= xt;
            vy[i]= yt;
        }
    }
}

//metodo para ler uma imagem 3D (.obj)
void Obj::carrega_objeto_3d(string opcao_obj, string nome_arquivo){
    int i;
    num_faces=0;
    ifstream arq_input(nome_arquivo.c_str());
    string line;
    stringstream ss;

    if(arq_input.good()){
        nome_obj= opcao_obj;
        while(getline(arq_input, line,'\n').good()){
            ss.str("");
            ss.clear();
            ss.str(line);
            string subs;
            if(line[0] == 'v'){   // o primeiro caractere da linha indica um vertice
                getline(ss,subs,' ');// pula o caractere
                getline(ss,subs,' ');
                vx.push_back(stof(subs));
                getline(ss,subs,' ');
                vy.push_back(stof(subs));
                getline(ss,subs,' ');
                vz.push_back(stof(subs));
            }
            if(line[0] == 'f'){ // o primeiro caractere da linha indica uma face
                num_faces++;
                getline(ss,subs,' ');  // pula o caractere
                getline(ss,subs,' ');
                f_v1.push_back(stoi(subs)-1);   
                getline(ss,subs,' ');
                f_v2.push_back(stoi(subs)-1);
                getline(ss,subs,' ');
                f_v3.push_back(stoi(subs)-1);
            }
        }
        arq_input.close();
        for (i=0; i<num_faces; i++){
            float Px=vx[f_v2[i]]-vx[f_v1[i]];
            float Py=vy[f_v2[i]]-vy[f_v1[i]];
            float Pz=vz[f_v2[i]]-vz[f_v1[i]];

            float Qx=vx[f_v3[i]]-vx[f_v2[i]];
            float Qy=vy[f_v3[i]]-vy[f_v2[i]];
            float Qz=vz[f_v3[i]]-vz[f_v2[i]];

            float Ci = Py*Qz - Pz*Qy;
            float Cj = Px*Qz - Pz*Qx;
            float Ck = Px*Qy - Py*Qx;

            float modulo = sqrt(Ci*Ci+Cj*Cj+Ck*Ck);
            normal1.push_back(Ci/modulo);
            normal2.push_back(Cj/modulo);
            normal3.push_back(Ck/modulo);
        }
    }else{
        cout << "Erro de leitura!" << endl;
    }
}

string Obj::retorna_nome_objeto(){
    return nome_obj;
}

//metodo para printar na tela todos os dados lidos de uma imagem 3D (.obj)
void Obj::printa_objeto_3d(){
    int i=0;
    for(i=0; i < vx.size(); i++){
        cout << i << "\tv " << vx[i] << ' ' << vy[i] << ' ' << vz[i] << "\n";
    }
    for(i=0; i < f_v1.size(); i++){
        cout << i << "\tf " << f_v1[i]+1 << ' ' << f_v2[i]+1 << ' ' << f_v3[i]+1 << "\n";
    }
}

void carrega_todos_objetos(int argc, char **argv){
    string resp = "s";
    string nome_objeto;
    string nome_objeto2;
    string comando;
    float nova_escala, x, y, z, r, g, b, rx, ry, rz;
    x_pos_atual=0.0, y_pos_atual= 0.0, z_pos_atual= 0.0;
    //x_pos_cam= 0.0, y_pos_cam= 100.0, z_pos_cam= 50.0;
    x_ap_cam= 0.0, y_ap_cam= 0.0, z_ap_cam= 0.0;
    theta= 0, angulox= 0;

    opcao_luz_amb= 0, red_amb= 0.1, green_amb= 0.1, blue_amb= 0.1, x_luz_amb= 1000.0, y_luz_amb= -1000.0, z_luz_amb= -100.0; 
    opcao_luz_dif= 0, red_dif= 1.0, green_dif= 1.0, blue_dif= 1.0, x_luz_dif= 1000.0, y_luz_dif= 1000.0, z_luz_dif= -100.0;  
    opcao_luz_esp= 0, red_esp= 1.0, green_esp= 1.0, blue_esp= 1.0, x_luz_esp= 1000.0, y_luz_esp= 1000.0, z_luz_esp= -100.0;  

    cout << "Informacoes importantes: \n" << "\tBotoes: \t=\tacao\n";
    cout << "\tw \t\t=\tinclina camera para cima\n" << "\ts \t\t=\tinclina camera para baixo\n";
    cout << "\ta \t\t=\tgira camera para esquerda\n" << "\td \t\t=\tgira camera para direita\n\n";
    cout << "\tfrente \t\t=\tanda para frente\n" << "\tatras \t\t=\tanda para atras\n";
    cout << "\tesquerda \t=\tanda para esquerda\n" << "\tdireita \t=\tanda para direita\n\n";
    cout << "\tscroll(frente) \t=\tdesce com a camera\n" << "\tscroll(atras) \t=\tsobe com a camera\n\n";
    cout << "\t1 \t\t=\tliga/desliga luz ambiente\n" << "\t2 \t\t=\tliga/desliga luz especular\n";
    cout << "\t3 \t\t=\tliga/desliga luz difusa\n\n" << "\tc \t\t=\talterna os dois modo de camera (sem cabeca ou normal)\n";

    cout << "Escolha o modo da camera (0= sem cabeca ou 1= orientado para onde a camera aponta): ";
    cin >> modo_camera;
    if(modo_camera < 0 || modo_camera > 1) modo_camera= 0;

    
    if(modo_camera == 0){
        cout << "\n\nEscolha a seguir os mesmos valores para Pos_X e Aponta_X!\n\n";
        cout << "Escolha opcao da camera: Pos_X Pos_Y Pos_Z Aponta_X Aponta_Y Aponta_Z\n";
        cin >> x_pos_atual >> y_pos_atual >> z_pos_atual >> x_ap_cam >> y_ap_cam >> z_ap_cam;
    }else if(modo_camera == 1){
        do{
            cout << "\n\nEscolha a seguir os mesmos valores para Pos_X e Aponta_X!\n\n";
            cout << "Escolha opcao da camera: Pos_X Pos_Y Pos_Z Aponta_X Aponta_Y Aponta_Z\n";
            cin >> x_pos_atual >> y_pos_atual >> z_pos_atual >> x_ap_atual >> y_ap_atual >> z_ap_atual;
        }while(x_pos_atual != x_ap_atual);
    }    
    
    cout << "Deseja ligar luz ambiente? ";
    cin >> resp;
    if(!(resp == "n" || resp == "nao" || resp == "não")){
        opcao_luz_amb= 1;
        cout << "Escolha as propriedades da luz ambiente:\nPos_X Pos_Y Pos_Z intensidade_Vermelho intensidade_Verde intensidade_Azul\n";
        cin >> x_luz_amb >> y_luz_amb >> z_luz_amb >> red_amb >> green_amb >> blue_amb;
    }

    cout << "Deseja ligar luz difusa? ";
    cin >> resp;
    if(!(resp == "n" || resp == "nao" || resp == "não")){
        opcao_luz_dif= 1;
        cout << "Escolha as propriedades da luz difusa:\nPos_X Pos_Y Pos_Z intensidade_Vermelho intensidade_Verde intensidade_Azul\n";
        cin >> x_luz_dif >> y_luz_dif >> z_luz_dif >> red_dif >> green_dif >> blue_dif;
    }
    cout << "Deseja ligar luz especular? ";
    cin >> resp;
    if(!(resp == "n" || resp == "nao" || resp == "não")){
        opcao_luz_esp= 1;
        cout << "Escolha as propriedades da luz especular:\nPos_X Pos_Y Pos_Z intensidade_Vermelho intensidade_Verde intensidade_Azul\n";
        cin >> x_luz_esp >> y_luz_esp >> z_luz_esp >> red_esp >> green_esp >> blue_esp;
    }

    //-------------------- usuario vai escolher manualmente pelo terminal: --------------------
    if(argc == 1){
        cout << "Exemplo de parametros para adicionar objetos: nome_objeto.obj escala pos_x pos_y pos_z red green blue\n";
        while(!(resp == "n" || resp == "nao" || resp == "não")){
            if(!(resp == "n" || resp == "nao" || resp == "não")){
                Obj *novo_objeto = new Obj;
                cout << "parametros: ";
                cin >> nome_objeto >> nova_escala >> x >> y >> z >> r >> g >> b >> rx >> ry >> rz;
                comando = ("ctmconv "+nome_objeto+" buffer.obj --no-normals --no-texcoords");
                system(comando.c_str());
                comando = ("python3 limpa_lixo_arquivo_obj.py buffer.obj sufixo.obj");
                system(comando.c_str());
                comando = ("cp sufixo.obj "+nome_objeto);
                system(comando.c_str());

                novo_objeto->carrega_objeto_3d(nome_objeto, nome_objeto);
                novo_objeto->seta_escala_objeto(nova_escala);
                novo_objeto->seta_posicao_objeto(x, y, z);
                novo_objeto->seta_cor_objeto(r, g, b);
                novo_objeto->rotaciona_objeto_3d(rx, ry, rz);
                cout << "Adicionar mais objetos? ";
                cin >> resp;
            }
        }
    //-------------------- usuario escolher propriedades dos objetos por um arquivo: --------------------
    }else if(argc >= 2){
        ifstream arq_input(argv[1]);
        string line;
        stringstream ss;
        string subs;

        if(arq_input.good()){
            cout << "\n\nCarregando Objetos do arquivo " << argv[1] << "...\n";
            while(getline(arq_input, line,'\n').good()){
                ss.str("");
                ss.clear();
                ss.str(line);
                if(!(line[0] == ' '|| line[0] == '#' || line[0] == '/')){
                    Obj *novo_objeto = new Obj;
                    vector <float> posicao_escolhida;
                    vector <float> cores_escolhidas;
                    vector <float> rotacao_escolhida;
                    //cout << line << "\n";
                    
                    getline(ss,subs,' ');
                    if(argc == 3){
                        comando = ("ctmconv "+subs+" buffer.obj --no-normals --no-texcoords");
                        system(comando.c_str());
                        comando = ("python3 limpa_lixo_arquivo_obj.py buffer.obj sufixo.obj");
                        system(comando.c_str());
                        comando = ("cp sufixo.obj "+subs);
                        system(comando.c_str());
                    }
                    novo_objeto->carrega_objeto_3d(subs, subs);
                    getline(ss,subs,' ');
                    nova_escala= stof(subs);
                    getline(ss,subs,' ');
                    x= stof(subs);
                    getline(ss,subs,' ');
                    y= stof(subs);
                    getline(ss,subs,' ');
                    z= stof(subs);
                    novo_objeto->seta_escala_objeto(nova_escala);
                    novo_objeto->seta_posicao_objeto(x, y, z);

                    getline(ss,subs,' ');
                    r= stof(subs);
                    getline(ss,subs,' ');
                    g= stof(subs);
                    getline(ss,subs,' ');
                    b= stof(subs);
                    novo_objeto->seta_cor_objeto(r, g, b);

                    getline(ss,subs,' ');
                    x= stof(subs);
                    getline(ss,subs,' ');
                    y= stof(subs);
                    getline(ss,subs,' ');
                    z= stof(subs);
                    novo_objeto->rotaciona_objeto_3d(x, y, z);

                    lista_objetos.push_back(novo_objeto);
                }
            }
            arq_input.close();
        }
    }
    //for(j=0; j < lista_objetos.size(); j++){
    //    lista_objetos[j]->printa_objeto_3d();
    //}
}


//-------------------------- FUNCOES DA GLUT --------------------------
void timer(int value){
    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}

void desenha_grama(float tam, float x, float y, float z){
    glColor4f (0.0, 0.6, 0.0,1.0); 
    GLfloat cor[]={0.0, 0.6, 0.0, 0.0};     //cor da face atual
    glMaterialfv(GL_FRONT, GL_AMBIENT, cor);    //seta cor na face atual
    glEnable(GL_COLOR_MATERIAL);
    
    glPushMatrix();
    glBegin (GL_POLYGON);
        glVertex3f(-tam+x, y, -tam+z);
        glVertex3f(-tam+x, y, tam+z);
        glVertex3f(tam+x, y, tam+z);
        glVertex3f(tam+x, y, -tam+z);
    glEnd();
    glPopMatrix();
}

void desenha_rua(float tam, float x, float y, float z){
    glColor4f (0.0, 0.1, 0.3, 0.5);
    GLfloat cor[]={0.0, 0.1, 0.3, 0.0};     //cor da face atual
    glMaterialfv(GL_FRONT, GL_AMBIENT, cor);    //seta cor na face atual
    glEnable(GL_COLOR_MATERIAL);
    
    glPushMatrix();
    glBegin (GL_POLYGON);
        glVertex3f(-tam+x, y, -tam+z);
        glVertex3f(-tam+x, y, tam+z);
        glVertex3f(tam+x, y, tam+z);
        glVertex3f(tam+x, y, -tam+z);
    glEnd();
    glPopMatrix();
}

void Obj::desenha_objeto_tela(int opcao){
    glEnable(GL_COLOR_MATERIAL);
    glColor4f (0.0, 0.0, 0.0, 1.0);
    GLfloat cor[]={1.0, 0.0, 0.0, 1.0};    //cor da face atual

    glPushMatrix();
    for(int i=0; i<num_faces; i++){  
            if(red > green){
            if(green > blue){   //red > green > blue
                cor[0]= (float)i/(float)num_faces;
                cor[1]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                cor[2]= (float)i/(float)num_faces;
            }else{
                if(red > blue){     //red > blue > green
                    cor[0]= (float)i/(float)num_faces;
                    cor[2]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                    cor[1]= (float)i/(float)num_faces;
                }else{      //blue > red > green
                    cor[2]= (float)i/(float)num_faces;
                    cor[0]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                    cor[1]= (float)i/(float)num_faces;
                }
            }
        }else{ 
            if(green > blue){ 
                if(red > blue){     //green > red > blue
                    cor[1]= (float)i/(float)num_faces;
                    cor[0]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                    cor[2]= (float)i/(float)num_faces;
                }else{      //green > blue > red
                    cor[1]= (float)i/(float)num_faces;
                    cor[2]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                    cor[0]= (float)i/(float)num_faces;
                }
            }else{      //blue > green > red
                cor[2]= (float)i/(float)num_faces;
                cor[1]= ((float)i/(float)green)<0.5?(2.0*(float)i/(float)green):2.0-2.0*((float)i/(float)green);
                cor[0]= (float)i/(float)num_faces;
            }
        }

        glNormal3f(normal1[i],normal2[i],normal3[i]);
        glMaterialfv(GL_FRONT, GL_AMBIENT, cor);    //seta cor ambiente na face atual
        glBegin (GL_TRIANGLES);
            glVertex3f(pos_x+escala*vx[f_v1[i]], pos_y+escala*vy[f_v1[i]], pos_z+escala*vz[f_v1[i]]);
            glVertex3f(pos_x+escala*vx[f_v2[i]], pos_y+escala*vy[f_v2[i]], pos_z+escala*vz[f_v2[i]]);
            glVertex3f(pos_x+escala*vx[f_v3[i]], pos_y+escala*vy[f_v3[i]], pos_z+escala*vz[f_v3[i]]);
        glEnd();
    }
    glPopMatrix();
}

void display(void){
    int i;
    float e;
    GLfloat rot_x=0.0, rot_y=0.0;
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
	alpha=(theta/180.0)*3.14159265;


    //---------------- ILUMINACOES ----------------
    glDisable(GL_COLOR_MATERIAL);   //desativa a cor dos objetos
        
    //luz ambiente (difundida igualmente em todas as direcoes)
    if(opcao_luz_amb == 1){
        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);       //atribui a luz LIGHT0, com o atributo ambiente, com as cores light0_ambient
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);     //atribui posicao da luz ambiente
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
        glColorMaterial(GL_FRONT, GL_AMBIENT);                  //ativa luz ambiente nos objetos, nas faces frontais
        glEnable(GL_LIGHT0);                                    //ativa luz ambiente
    }else if(opcao_luz_amb == 0){
        glDisable(GL_LIGHT0);
    }

    //luz especular (reflete para uma direcao)
    if(opcao_luz_esp == 1){
        glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);     //atribui a luz LIGHT1, com o atributo especular, com as cores light1_specular
        glLightfv(GL_LIGHT1, GL_POSITION, light1_position);     //atribui posicao da luz especular
        glColorMaterial(GL_FRONT, GL_SPECULAR);                 //ativa luz especular nos objetos, nas faces frontais
        glEnable(GL_LIGHT1);                                    //ativa luz especular
    }else if(opcao_luz_esp == 0){ 
        glDisable(GL_LIGHT1);
    }

    //luz diffusa (reflexiva - reflete para todas as posicoes)
    if(opcao_luz_dif == 1){
        glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);       //atribui a luz LIGHT2, com o atributo diffuse, com as cores light2_diffuse
        glLightfv(GL_LIGHT2, GL_POSITION, light2_position);     //atribui posicao da luz difusa
        glColorMaterial(GL_FRONT, GL_DIFFUSE);                  //ativa luz difusa nos objetos, nas faces frontais
        glEnable(GL_LIGHT2);                                    //ativa luz difusa
    }else if(opcao_luz_dif == 0){
        glDisable(GL_LIGHT2);
    }
    glEnable(GL_COLOR_MATERIAL);    //ativa cor dos objetos
    glEnable(GL_LIGHTING);          //liga todas as iluminacoes


    //---------------- CAMERA ----------------
    if(modo_camera == 0){   //OK-modo sem cabeca
        gluLookAt(x_pos_cam+x_pos_atual+0.0, y_pos_cam+y_pos_atual+0.0, z_pos_cam+z_pos_atual+0.0, x_pos_cam+x_ap_cam+0.0, y_pos_cam+y_ap_cam+0.0, z_pos_cam+z_ap_cam+0.0, 0.0, 1.0, 0.0);
    }else if(modo_camera == 1){     //OK-modo orientado para onde a camera aponta
        gluLookAt(x_pos_atual+0.0, y_pos_atual+0.0, z_pos_atual+0.0, x_ap_atual+0.0, y_ap_cam+y_ap_atual+0.0, z_ap_atual+0.0, 0.0, 1.0, 0.0);
        glTranslated(x_pos_cam, y_pos_cam, z_pos_cam);      //andar para lado, altura, profundidade
        glRotatef(theta,0.0,1.0,0.0);       //angulo p/ rotacionar, x , y , z
    }
    glScalef(1.0, 1.0, 1.0);

    //desenha todos os objetos da lista na tela
    for(i=0; i<lista_objetos.size(); i++){
        lista_objetos[i]->desenha_objeto_tela(1);
    }
    
    desenha_grama(110, 106, 0, -100);
    desenha_grama(110, 106, 0, -211);
    for(i=-10; i < 260; i= i+30){
        desenha_rua(15, i, 1, 25);
        desenha_rua(15, i, 1, -60);
        desenha_rua(15, i, 1, -145);
        desenha_rua(15, i, 1, -240);
        desenha_rua(15, i, 1, -335);
    }
    for(i=25; i > -350; i= i-30){
        desenha_rua(15, -17, 1, i);
        desenha_rua(15, 230, 1, i);
    }

    glFlush();
    glutSwapBuffers();
    glDisable(GL_TEXTURE_2D);
}


void myReshape(int w, int h){
    float aspectRatio = (float)w/(float)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-50,50,-50,50,-50,50);
    gluPerspective(45,aspectRatio,-20,20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void myInit(void){
    glClearColor (0.0, 0.0, 0.0, 0.0);
    //glShadeModel (GL_FLAT);
    glShadeModel(GL_SMOOTH);
    glFrontFace(GL_CW);    
    glCullFace(GL_FRONT);    //Estas tres fazem o culling funcionar
    glEnable(GL_CULL_FACE); 

    glClearDepth(100.0f);
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);
    glDepthRange(200,-200);   
}

//comandos do teclado para andar com a camera
void processNormalKeys(unsigned char key, int x, int y){
    float xt, yt, zt;
    if(key == 27){ //tecla Escape - fechar a tela de visualizacao
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        exit(0);
    }
    if(key == 'w' || key == 'W'){ //sobe com a camera
        y_ap_cam++;
    }
    if(key == 's' || key == 'S'){ //desce com a camera
        y_ap_cam--;
    }
    if(key == 'a' || key == 'A'){ //gira camera em seu eixo para esquerda
        theta--;
        angulox--;
        if(modo_camera == 0){
            if(angulox > 360){
                angulox= 0.0;
            }else if(angulox < 0.0){
                angulox= 359;
            }
            if(angulox >= 0 && angulox < 90){
                x_ap_cam-= 1;
                z_ap_cam-= 1;
            }else if(angulox >= 90 && angulox < 180){
                x_ap_cam+= 1;
                z_ap_cam-= 1;
            }else if(angulox >= 180  && angulox < 270){
                x_ap_cam+= 1;
                z_ap_cam+= 1;
            }else if(angulox >= 270  && angulox <= 360){
                x_ap_cam-= 1;
                z_ap_cam+= 1;
            }
        }
    }
    if(key == 'd' || key == 'D'){ //gira camera em seu eixo para direita
        theta++;
        angulox++;
        if(modo_camera == 0){
            if(angulox > 360){
                angulox= 0.0;
            }else if(angulox < 0.0){
                angulox= 359;
            }
            if(angulox >= 0 && angulox < 90){
                x_ap_cam+= 1;
                z_ap_cam+= 1;
            }else if(angulox >= 90 && angulox < 180){
                x_ap_cam-= 1;
                z_ap_cam+= 1;
            }else if(angulox >= 180  && angulox < 270){
                x_ap_cam-= 1;
                z_ap_cam-= 1;
            }else if(angulox >= 270  && angulox <= 360){
                x_ap_cam+= 1;
                z_ap_cam-= 1;
            }
        }
    }
    if(key == 49){      //botao 1 do teclado - liga/desliga luz ambiente
        if(opcao_luz_amb == 1){
            opcao_luz_amb= 0;
        }else if(opcao_luz_amb == 0){
            opcao_luz_amb= 1;
        }
    }
    if(key == 50){      //botao 2 do teclado - liga/desliga luz especular
        if(opcao_luz_esp == 1){
            opcao_luz_esp= 0;
        }else if(opcao_luz_esp == 0){
            opcao_luz_esp= 1;
        }
    }
    if(key == 51){      //botao 3 do teclado - liga/desliga luz difusa
        if(opcao_luz_dif == 1){
            opcao_luz_dif= 0;
        }else if(opcao_luz_dif == 0){
            opcao_luz_dif= 1;
        }
    }
    glutPostRedisplay();
}

//comandos especiais do teclado para andar com a camera
void processSpecialKeys(int key, int x, int y){
    if(key == GLUT_KEY_UP){     //tecla seta para cima, vai para frente no cenario
        if(modo_camera == 0){
            z_pos_cam--;
        }else{
            z_pos_cam++;
        }
    }
    if(key == GLUT_KEY_DOWN){   //tecla seta para baixo, vai para tras no cenario
        if(modo_camera == 0){
            z_pos_cam++;
        }else{
            z_pos_cam--;
        }
    }
    if(key == GLUT_KEY_RIGHT){  //tecla seta para direita, vai para direita no cenario
        if(modo_camera == 0){
            x_pos_cam++;
        }else{
            x_pos_cam--;
        } 
    }
    if(key == GLUT_KEY_LEFT){   //tecla com seta para esquerda, vai para esquerda no cenario
        if(modo_camera == 0){
            x_pos_cam--;
        }else{
            x_pos_cam++;
        }
    }
}

//comandos do mouse para descer e subir a camera
void Mouse(int button, int state, int x, int y){
    if (button==3){     //rolar o scroll do mouse para tras, camera sobe
        if(modo_camera == 0){
            y_pos_cam--;
        }else{
            y_pos_cam++;
        }
    }
    if (button==4){     //rolar o scroll do mouse para frente, camera desce
        if(modo_camera == 0){
            y_pos_cam++;
        }else{
            y_pos_cam--;
        }
    }
}


int main(int argc, char **argv){
    int i= argc;
    char **argumentos= argv;
    angulox= 0.0;
    theta= 0.0;

    carrega_todos_objetos(i, argumentos);

    //--------------------- chamadas das funcoes da GLUT ---------------------
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (1000, 1000);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Trabalho 2 - Igor Capeletti - Computacao Grafica");
    glEnable (GL_BLEND | GL_DEPTH_TEST);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    myInit();
    glutDisplayFunc(display);
    glutReshapeFunc(myReshape);
    glutTimerFunc(10, timer, 0);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(Mouse);

    glutMainLoop();

    //------------------------------------------------------------------------------------
    //limpar vectores dentro de cada objeto e limpa o vetor de objetos
    for(int i=0; i < lista_objetos.size(); i++){
        lista_objetos[i]->~Obj();
    }
    lista_objetos.clear();
    return 0;
}

//Autor: Nedeljko Tesanovic
//Opis: Primjer upotrebe tekstura

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//stb_image.h je header-only biblioteka za ucitavanje tekstura.
//Potrebno je definisati STB_IMAGE_IMPLEMENTATION prije njenog ukljucivanja
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath); //Ucitavanje teksture, izdvojeno u funkciju

int main(void)
{

    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 1280;
    unsigned int wHeight = 1000;
    const char wTitle[] = "[Generic title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }



    unsigned VAO[19]; //0 - body, 1 - membrana, 2 - mrezica, 3 - powerbutton, 4 - powerbuttonON, 5 - onBulb, 6 - amfmmode, 
    //7 - amfm scale, 8 - pointer, 9 - credentials, 10 - antena, 11 - antena2, 12 - antena3, 13 - sliderLine, 14 - slider pointer
    // 15 - pozadina za progress bar, 16 - kockica za volume, 17 - volume muted, 18 - volume max
    glGenVertexArrays(19, VAO);
    unsigned VBO[19];
    glGenBuffers(19, VBO);


    //shaderi
    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int zvucnikShader = createShader("basic.vert", "zvucnik.frag");
    unsigned int mrezicaShader = createShader("mrezica.vert", "mrezica.frag");
    unsigned int pointerShader = createShader("pointer.vert", "pointer.frag");
    unsigned int antenaShader = createShader("antena.vert", "antena.frag");
    unsigned int volumeSliderShader = createShader("sliderPoint.vert", "sliderPoint.frag");
    unsigned int volumeTickShader = createShader("volumeTick.vert", "volumeTick.frag");
    unsigned int unifiedShader = createShader("mrezica.vert", "mrezica.frag");
    unsigned int onBulbShader = createShader("onBulb.vert", "onBulb.frag");
    unsigned uTexLoc = glGetUniformLocation(unifiedShader, "uTex");

    
    
    //radio body
    float rectangleVertices[] = {
        -0.7, -0.55,       0.6, 0.4, 0.2, 0.5, //dole levo
        -0.7, 0.55,       0.6, 0.4, 0.2, 0.5, // gore levo
        0.7, -0.55,       0.6, 0.4, 0.2, 0.5, // dole desno
        0.7, 0.55,       0.6, 0.4, 0.2, 0.5, // gore desno
    };

    unsigned int stride = 6 * sizeof(float);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    //membrana zvucnika
    float centerX = -0.35f;
    float centerY = 0.0f;
    float radius = 0.25f;

    const int segments = 100; // Broj segmenata kruga
    float* circleVertices = new float[segments * 2];

    for (int i = 0; i < segments; ++i)
    {
        float angle = 2.0f * 3.1415926 * i / segments;
        circleVertices[2 * i] = centerX + radius * std::cos(angle);
        circleVertices[2 * i + 1] = centerY + radius * std::sin(angle);
    }

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * segments * 2, circleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //mrezica zvucnika
    

    float mrezicaVertices[] = {
        -0.61, -0.26,       0.0, 0.0, //donji levi
        -0.61, 0.26,       0.0, 1.0, //gornji levi
        -0.09, -0.26,       1.0, 0.0, //donji desni
        -0.09, 0.26,       1.0, 1.0, //gornji desni
    };

    unsigned int stride1 = 4 * sizeof(float);

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mrezicaVertices), mrezicaVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride1, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride1, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    //dugme za paljenje radija
    float powerButtonVertices[] = {
        0.5, 0.55,    1.0, 0.0, 0.0, 1.0,
        0.5, 0.65,    1.0, 0.0, 0.0, 1.0,
        0.6, 0.55,    1.0, 0.0, 0.0, 1.0,
        0.6, 0.65,    1.0, 0.0, 0.0, 1.0,
    };

    unsigned int stride2 = 6 * sizeof(float);

    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(powerButtonVertices), powerButtonVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride2, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride2, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //upaljeno dugme za paljenje
    float newPowerButtonVertices[] = {
        0.5, 0.55,    1.0, 0.0, 0.0, 1.0,
        0.5, 0.59,    1.0, 0.0, 0.0, 1.0,
        0.6, 0.55,    1.0, 0.0, 0.0, 1.0,
        0.6, 0.59,    1.0, 0.0, 0.0, 1.0,
    };

    unsigned int stride3 = 6 * sizeof(float);

    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(newPowerButtonVertices), newPowerButtonVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride3, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride3, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //onBulb a i offBulb
    float onBulbVertices[] = {
        -0.65, 0.52,       0.0, 0.0, //donji levi
        -0.65, 0.7,       0.0, 1.0, //gornji levi
        -0.45, 0.52,       1.0, 0.0, //donji desni
        -0.45, 0.7,       1.0, 1.0, //gornji desni
    };

    unsigned int stride4 = 4 * sizeof(float);

    glBindVertexArray(VAO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(onBulbVertices), onBulbVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride4, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride4, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // AM-FM Mode
    float amfmVertices[] = {
        0.5, 0.4,       0.0, 0.0, //donji levi
        0.5, 0.5,       0.0, 1.0, //gornji levi
        0.6, 0.4,       1.0, 0.0, //donji desni
        0.6, 0.5,       1.0, 1.0, //gornji desni
    };

    unsigned int stride5 = 4 * sizeof(float);

    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(amfmVertices), amfmVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride5, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride5, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //AM-FM SCALE
    float amfmScaleVertices[] = {
        0.0, 0.1,       0.0, 0.0, //donji levi
        0.0, 0.2,       0.0, 1.0, //gornji levi
        0.6, 0.1,       1.0, 0.0, //donji desni
        0.6, 0.2,       1.0, 1.0, //gornji desni
    };

    unsigned int stride6 = 4 * sizeof(float);

    glBindVertexArray(VAO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(amfmScaleVertices), amfmScaleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride6, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride6, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //POINTER
    float pointerVertices[] = {
        0.04, 0.1,
        0.04, 0.2,
    };

    unsigned int stride7 = 2 * sizeof(float);

    glBindVertexArray(VAO[8]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[8]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointerVertices), pointerVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride7, (void*)0);
    glEnableVertexAttribArray(0);

    //CREDENTIALS
    float credentialsVertices[] = {
        0.4, 0.8,       0.0, 0.0, //donji levi
        0.4, 0.9,       0.0, 1.0, //gornji levi
        0.95, 0.8,       1.0, 0.0, //donji desni
        0.95, 0.9,       1.0, 1.0, //gornji desni
    };

    unsigned int stride8 = 4 * sizeof(float);

    glBindVertexArray(VAO[9]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[9]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(credentialsVertices), credentialsVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride8, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride8, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //ANTENA
    float antenaVertices[] = {
        -0.67, 0.55,
        -0.67, 0.65,
    };

    unsigned int stride9 = 2 * sizeof(float);

    glBindVertexArray(VAO[10]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[10]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(antenaVertices), antenaVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride9, (void*)0);
    glEnableVertexAttribArray(0);

    //ANTENA2
    float antena2Vertices[] = {
        -0.67, 0.65,
        -0.67, 0.80,
    };

    unsigned int stride10 = 2 * sizeof(float);

    glBindVertexArray(VAO[11]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[11]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(antena2Vertices), antena2Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride10, (void*)0);
    glEnableVertexAttribArray(0);

    //ANTENA3
    float antena3Vertices[] = {
        -0.67, 0.80,
        -0.67, 0.97,
    };

    unsigned int stride11 = 2 * sizeof(float);

    glBindVertexArray(VAO[12]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[12]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(antena3Vertices), antena3Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride11, (void*)0);
    glEnableVertexAttribArray(0);


    //SLIDER LINE
    float sliderLineVertices[] = {
        0.0, -0.15,   1.0, 1.0, 1.0, 1.0,
        0.6, -0.15,   1.0, 1.0, 1.0, 1.0,
    };

    unsigned int stride12 = 6 * sizeof(float);

    glBindVertexArray(VAO[13]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[13]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sliderLineVertices), sliderLineVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride12, (void*)0);
    glEnableVertexAttribArray(0);

    //SLIDER POINT
    float sliderPointVertices[] = {
        0.0, -0.15
    };

    unsigned int stride13 = 2 * sizeof(float);

    glBindVertexArray(VAO[14]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[14]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sliderPointVertices), sliderPointVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride13, (void*)0);
    glEnableVertexAttribArray(0);

    //PROGRESS BAR BG
    float progressBarBgVertices[] = {
        0.0, -0.35,       1.0, 1.0, 1.0, 1.0,
        0.0, -0.27,       1.0, 1.0, 1.0, 1.0,
        0.6, -0.35,       1.0, 1.0, 1.0, 1.0,
        0.6, -0.27,       1.0, 1.0, 1.0, 1.0,
    };

    unsigned int stride14 = 6 * sizeof(float);

    glBindVertexArray(VAO[15]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[15]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(progressBarBgVertices), progressBarBgVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride14, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride14, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    //PROGRESS BAR TICK
    float progressBarTickVertices[] = {
        0.03, -0.31
    };

    unsigned int stride15 = 2 * sizeof(float);

    glBindVertexArray(VAO[16]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[16]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(progressBarTickVertices), progressBarTickVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride15, (void*)0);
    glEnableVertexAttribArray(0);

    //VOLUME MUTED
    float volumeMutedVertices[] = {
        -0.12, -0.2,       0.0, 0.0, //donji levi
        -0.12, -0.1,       0.0, 1.0, //gornji levi
        -0.03, -0.2,       1.0, 0.0, //donji desni
        -0.03, -0.1,       1.0, 1.0, //gornji desni
    };

    unsigned int stride16 = 4 * sizeof(float);

    glBindVertexArray(VAO[17]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[17]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(volumeMutedVertices), volumeMutedVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride16, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride16, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //VOLUME MAX
    float volumeMaxVertices[] = {
        0.615, -0.2,       0.0, 0.0, //donji levi
        0.615, -0.1,       0.0, 1.0, //gornji levi
        0.70, -0.2,       1.0, 0.0, //donji desni
        0.70, -0.1,       1.0, 1.0, //gornji desni
    };

    unsigned int stride17 = 4 * sizeof(float);

    glBindVertexArray(VAO[18]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[18]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(volumeMaxVertices), volumeMaxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride17, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride17, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //ovo posle svega
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //ovo posle svega



    //tekstura upaljene lampice
    unsigned onBulbTexture = loadImageToTexture("res/bulbon.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, onBulbTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)

    //tekstura ugasene lampice
    unsigned offBulbTexture = loadImageToTexture("res/bulboff.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, offBulbTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)


    //tekstura mrezica
    unsigned checkerTexture = loadImageToTexture("res/mreza.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, checkerTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)


    //tekstura AM
    unsigned amTexture = loadImageToTexture("res/am.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, amTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)


    //tekstura FM
    unsigned fmTexture = loadImageToTexture("res/fm.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, fmTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)


    //tekstura FM/AM Scale 
    unsigned amfmScaleTexture = loadImageToTexture("res/amfmscale.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, amfmScaleTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)

    
    //tekstura CREDENTIALS
    unsigned credentialstexture = loadImageToTexture("res/credentials.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, credentialstexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)

    //teksura volume muted
    unsigned volumeMutedTexture = loadImageToTexture("res/volumeMuted.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, volumeMutedTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)
    
    //tekstura volume MAX
    unsigned volumeMaxTexture = loadImageToTexture("res/volumeMax.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, volumeMaxTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)



    double lastRadiusUpdateTime = glfwGetTime();
    double radiusUpdateInterval = 0.014;
    bool increaseRadius = true;
    bool isOn = false;
    bool isAM = true;
    float x = 0;
    float y = 0;
    float volumeX = 0;
    float volumeY = 0;
    float offsetX = 0.01;
    float  currentTime3 = 0.0;
    glfwSetTime(0.0);
    unsigned int uPosLoc = glGetUniformLocation(basicShader, "uPos");
    unsigned int uPosLoc1 = glGetUniformLocation(volumeTickShader, "uPos");
    unsigned int uColorLoc = glGetUniformLocation(volumeTickShader, "uColor");


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            isOn = true; // za paljenje
            currentTime3 = glfwGetTime();
        }

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) 
        {
            isOn = false; // za gasenje
        }

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            if (isOn) {
                isAM = true; // za AM mode
            }
        }

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        {
            if (isOn) {
                isAM = false; // za FM mode
            }
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            x -= 0.00003;
            if (x < 0.0)
                x = 0.0;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            x += 0.00003;
            if (x > 0.51)
                x = 0.51;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            if (isOn) {
                volumeX -= 0.00009;
                if (volumeX < 0.0)
                    volumeX = 0.0;
            }
            
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            if (isOn) {
                volumeX += 0.00009;
                if (volumeX > 0.6)
                    volumeX = 0.6;
            }
            
        }

        
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        //radio body
        glUseProgram(basicShader);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(rectangleVertices) / stride);

        //VIBRIRANJE MEMBRANE
        double currentTime = glfwGetTime();
        if (isOn) {
            if (currentTime - lastRadiusUpdateTime >= radiusUpdateInterval)
            {

                if (increaseRadius)
                    radius += 0.01;
                else
                    radius -= 0.01;

                // Ažuriranje smera promene za sledeću iteraciju
                increaseRadius = !increaseRadius;
                lastRadiusUpdateTime = currentTime;

                // Ponovno postavljanje podataka za krug
                for (int i = 0; i < segments; ++i)
                {
                    float angle = 2.0f * 3.1415926 * i / segments;
                    circleVertices[2 * i] = centerX + radius * std::cos(angle);
                    circleVertices[2 * i + 1] = centerY + radius * std::sin(angle);
                }

                glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * segments * 2, circleVertices, GL_STATIC_DRAW);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
        }
        


        //membrana
        glUseProgram(zvucnikShader);
        glBindVertexArray(VAO[1]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW); // Ažuriranje podataka u VBO
        glDrawArrays(GL_TRIANGLE_FAN, 0, segments);


        //mrezica
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(unifiedShader);
        glBindVertexArray(VAO[2]);

        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, checkerTexture);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(mrezicaVertices)/stride1);
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);


        
        if (!isOn) {
            //POWERBUTTON
            glUseProgram(basicShader);
            glBindVertexArray(VAO[3]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(powerButtonVertices) / stride2);

            //OFFBULB
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glUseProgram(onBulbShader);
            glBindVertexArray(VAO[5]);

            glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
            glBindTexture(GL_TEXTURE_2D, offBulbTexture);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(onBulbVertices) / stride4);
            glDisable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else {
            //POWERBUTTON
            glUseProgram(basicShader);
            glBindVertexArray(VAO[4]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(newPowerButtonVertices) / stride3);

            //ONBULB
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glUseProgram(onBulbShader);
            glBindVertexArray(VAO[5]);

            glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
            glBindTexture(GL_TEXTURE_2D, onBulbTexture);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(onBulbVertices) / stride4);
            glDisable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        

        if (isAM) {
            //am MODE
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glUseProgram(onBulbShader);
            glBindVertexArray(VAO[6]);

            glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
            glBindTexture(GL_TEXTURE_2D, amTexture);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(amfmVertices) / stride5);
            glDisable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else {
            //fm MODE
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glUseProgram(onBulbShader);
            glBindVertexArray(VAO[6]);

            glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
            glBindTexture(GL_TEXTURE_2D, fmTexture);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(amfmVertices) / stride5);
            glDisable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        //AM-FM Scale
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(onBulbShader);
        glBindVertexArray(VAO[7]);

        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, amfmScaleTexture);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(amfmScaleVertices) / stride6);
        //glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);

        //POINTER
        glLineWidth(2.0);
        glUseProgram(pointerShader);
        glBindVertexArray(VAO[8]);
        glUniform2f(uPosLoc, x, y);
        glDrawArrays(GL_LINE_STRIP, 0, sizeof(pointerVertices) / stride7);
        

        //CREDENTIALS
        glUseProgram(onBulbShader);
        glBindVertexArray(VAO[9]);
        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, credentialstexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(credentialsVertices) / stride8);
        glBindTexture(GL_TEXTURE_2D, 0);

        double currentTime2 = glfwGetTime() - currentTime3;
        if (isOn) {
            if (currentTime2 >= 1.5) {
                //ANTENA2
                glLineWidth(25.0);
                glUseProgram(antenaShader);
                glBindVertexArray(VAO[11]);
                glDrawArrays(GL_LINE_STRIP, 0, sizeof(antena2Vertices) / stride10);
            }
            if (currentTime2 >= 3.0) {
                //ANTENA3
                glLineWidth(25.0);
                glUseProgram(antenaShader);
                glBindVertexArray(VAO[12]);
                glDrawArrays(GL_LINE_STRIP, 0, sizeof(antena3Vertices) / stride11);
            }
        }

        //ANTENA
        glLineWidth(25.0);
        glUseProgram(antenaShader);
        glBindVertexArray(VAO[10]);
        glDrawArrays(GL_LINE_STRIP, 0, sizeof(antenaVertices) / stride9);
        
        //SLIDER LINE
        glLineWidth(15.0);
        glUseProgram(basicShader);
        glBindVertexArray(VAO[13]);
        glDrawArrays(GL_LINE_STRIP, 0, sizeof(sliderLineVertices) / stride12);

        //SLIDER POINT
        glPointSize(15.0);
        glUseProgram(volumeSliderShader);
        glBindVertexArray(VAO[14]);
        glUniform2f(uPosLoc, volumeX, volumeY);
        glDrawArrays(GL_POINTS, 0, sizeof(sliderPointVertices) / stride13);

        // od 0.0 do 0.6
        
        //PROGRESS BAR BG
        glUseProgram(zvucnikShader);
        glBindVertexArray(VAO[15]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(progressBarBgVertices) / stride14);

        //VOLUME TICK
        glPointSize(35.0);
        glUseProgram(volumeTickShader);
        glBindVertexArray(VAO[16]);

        if (volumeX > 0.0 && volumeX < 0.06) {
            // volumeX je u intervalu 0.0 - 0.06
            // 1 kockica
            for (int i = 0; i < 1; ++i) {
                // Postavite odgovarajuće uniformne promenljive, na primer, pomerajte tačku po x-osi za svaku iteraciju
                glUniform2f(uPosLoc1, 0.0 + i*0.06, volumeY);
                glUniform4f(uColorLoc, 1.0, 1.0 - i*0.1, 0.0, 1.0);
                // Iscrtajte tačku
                glDrawArrays(GL_POINTS, 0, sizeof(progressBarTickVertices) / stride15);
            }
        }
        else if (volumeX >= 0.06 && volumeX < 0.12) {
            // volumeX je u intervalu 0.06 - 0.12
            // 2 kockice
            for (int i = 0; i < 2; ++i) {
                // Postavite odgovarajuće uniformne promenljive, na primer, pomerajte tačku po x-osi za svaku iteraciju
                glUniform2f(uPosLoc1, 0.0 + i * 0.06, volumeY);
                glUniform4f(uColorLoc, 1.0, 1.0 - i * 0.1, 0.0, 1.0);
                // Iscrtajte tačku
                glDrawArrays(GL_POINTS, 0, sizeof(progressBarTickVertices) / stride15);
            }
        }
        else if (volumeX >= 0.12 && volumeX < 0.18) {
            // volumeX je u intervalu 0.12 - 0.18
            // 3 kockice
            for (int i = 0; i < 3; ++i) {
                // Postavite odgovarajuće uniformne promenljive, na primer, pomerajte tačku po x-osi za svaku iteraciju
                glUniform2f(uPosLoc1, 0.0 + i * 0.06, volumeY);
                glUniform4f(uColorLoc, 1.0, 1.0 - i * 0.1, 0.0, 1.0);
                // Iscrtajte tačku
                glDrawArrays(GL_POINTS, 0, sizeof(progressBarTickVertices) / stride15);
            }
        }
        else if (volumeX >= 0.18 && volumeX < 0.24) {
            // volumeX je u intervalu 0.18 - 0.24
            // 4 kockice
            for (int i = 0; i < 4; ++i) {
                // Postavite odgovarajuće uniformne promenljive, na primer, pomerajte tačku po x-osi za svaku iteraciju
                glUniform2f(uPosLoc1, 0.0 + i * 0.06, volumeY);
                glUniform4f(uColorLoc, 1.0, 1.0 - i * 0.1, 0.0, 1.0);
                // Iscrtajte tačku
                glDrawArrays(GL_POINTS, 0, sizeof(progressBarTickVertices) / stride15);
            }
        }
        else if (volumeX >= 0.24 && volumeX < 0.3) {
            // volumeX je u intervalu 0.24 - 0.3
            // 5 kockica
            for (int i = 0; i < 5; ++i) {
                // Postavite odgovarajuće uniformne promenljive, na primer, pomerajte tačku po x-osi za svaku iteraciju
                glUniform2f(uPosLoc1, 0.0 + i * 0.06, volumeY);
                glUniform4f(uColorLoc, 1.0, 1.0 - i * 0.1, 0.0, 1.0);
                // Iscrtajte tačku
                glDrawArrays(GL_POINTS, 0, sizeof(progressBarTickVertices) / stride15);
            }
        }
        else if (volumeX >= 0.3 && volumeX < 0.36) {
            // volumeX je u intervalu 0.3 - 0.36
            // 6 kockica
            for (int i = 0; i < 6; ++i) {
                // Postavite odgovarajuće uniformne promenljive, na primer, pomerajte tačku po x-osi za svaku iteraciju
                glUniform2f(uPosLoc1, 0.0 + i * 0.06, volumeY);
                glUniform4f(uColorLoc, 1.0, 1.0 - i * 0.1, 0.0, 1.0);
                // Iscrtajte tačku
                glDrawArrays(GL_POINTS, 0, sizeof(progressBarTickVertices) / stride15);
            }
        }
        else if (volumeX >= 0.36 && volumeX < 0.42) {
            // volumeX je u intervalu 0.36 - 0.42
            // 7 kockica
            for (int i = 0; i < 7; ++i) {
                // Postavite odgovarajuće uniformne promenljive, na primer, pomerajte tačku po x-osi za svaku iteraciju
                glUniform2f(uPosLoc1, 0.0 + i * 0.06, volumeY);
                glUniform4f(uColorLoc, 1.0, 1.0 - i * 0.1, 0.0, 1.0);
                // Iscrtajte tačku
                glDrawArrays(GL_POINTS, 0, sizeof(progressBarTickVertices) / stride15);
            }
        }
        else if (volumeX >= 0.42 && volumeX < 0.48) {
            // volumeX je u intervalu 0.42 - 0.48
            // 8 kockica
            for (int i = 0; i < 8; ++i) {
                // Postavite odgovarajuće uniformne promenljive, na primer, pomerajte tačku po x-osi za svaku iteraciju
                glUniform2f(uPosLoc1, 0.0 + i * 0.06, volumeY);
                glUniform4f(uColorLoc, 1.0, 1.0 - i * 0.1, 0.0, 1.0);
                // Iscrtajte tačku
                glDrawArrays(GL_POINTS, 0, sizeof(progressBarTickVertices) / stride15);
            }
        }
        else if (volumeX >= 0.48 && volumeX < 0.54) {
            // volumeX je u intervalu 0.48 - 0.54
            // 9 kockica
            for (int i = 0; i < 9; ++i) {
                // Postavite odgovarajuće uniformne promenljive, na primer, pomerajte tačku po x-osi za svaku iteraciju
                glUniform2f(uPosLoc1, 0.0 + i * 0.06, volumeY);
                glUniform4f(uColorLoc, 1.0, 1.0 - i * 0.1, 0.0, 1.0);
                // Iscrtajte tačku
                glDrawArrays(GL_POINTS, 0, sizeof(progressBarTickVertices) / stride15);
            }
        }
        else if (volumeX >= 0.54) {
            // volumeX je u intervalu 0.54 - 0.6
            // 10 kockica
            for (int i = 0; i < 10; ++i) {
                // Postavite odgovarajuće uniformne promenljive, na primer, pomerajte tačku po x-osi za svaku iteraciju
                glUniform2f(uPosLoc1, 0.0 + i * 0.06, volumeY);
                glUniform4f(uColorLoc, 1.0, 1.0 - i * 0.1, 0.0, 1.0);
                // Iscrtajte tačku
                glDrawArrays(GL_POINTS, 0, sizeof(progressBarTickVertices) / stride15);
            }
        }


        //VOLUME MUTED
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(onBulbShader);
        glBindVertexArray(VAO[17]);

        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, volumeMutedTexture);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(volumeMutedVertices) / stride16);
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);

        //VOLUME MAX
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(onBulbShader);
        glBindVertexArray(VAO[18]);

        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, volumeMaxTexture);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(volumeMaxVertices) / stride17);
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);


        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &checkerTexture); //PODSETNIK DA NE ZABORAVIM: ne zaboravi da obrises i ostale teksture 
    glDeleteBuffers(19, VBO);
    glDeleteVertexArrays(19, VAO);
    glDeleteProgram(basicShader);
    glDeleteProgram(unifiedShader);
    glDeleteProgram(zvucnikShader);
    glDeleteProgram(onBulbShader);
    glDeleteProgram(antenaShader);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}
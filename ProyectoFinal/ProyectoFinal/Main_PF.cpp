#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 1600, HEIGHT = 1200;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(25.0f, -10.0f, 0.0f));//50 -10 0
GLfloat lastX = WIDTH ;
GLfloat lastY = HEIGHT ;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 0.0f;


// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(0.0f, 1.0f, 0.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

bool activeLuz1 = false;
bool activeLuz2 = false;
bool activeLuz3 = false;
bool activeLuz4 = false;
bool activeVent1 = false;
bool activeVent2 = false;
float rotVent1 = 0.0;
float rotVent2 = 0.0;
bool velocidad1 = false;
bool velocidad2 = false;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX = PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0;

#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
    //Variables para GUARDAR Key Frames
    float posX;		//Variable para PosicionX
    float posY;		//Variable para PosicionY
    float posZ;		//Variable para PosicionZ
    float incX;		//Variable para IncrementoX
    float incY;		//Variable para IncrementoY
    float incZ;		//Variable para IncrementoZ
    float rotRodIzq;
    float rotInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(7.0f, -8.5f, 9.0f),
    glm::vec3(-5.0f, -8.5f, 9.0f),
    glm::vec3(8.0f, -6.0f, -9.0f),
    glm::vec3(-8.0f, -6.0f, -6.0f)
};

glm::vec3 LightP1;
glm::vec3 LightP2;
glm::vec3 LightP3;
glm::vec3 LightP4;

glm::vec3 posIniP(-13.0f, -18.0f, -9.0f);
glm::vec3 posIniC(10.0f, -10.0f, -20.0f);

float movPizzaX = 0.0f;
float movPizzaZ = 0.0f;
float movPizzaY = 0.0f;

bool circuitoP = false;
bool recorridoP1 = true;
bool recorridoP2 = false;
bool recorridoP3 = false;
bool recorridoP4 = false;
bool recorridoP5 = false;

bool circuitoC = false;
bool recorridoC1 = true;
bool recorridoC2 = false;
bool recorridoC3 = false;
bool recorridoC4 = false;
bool recorridoC5 = false;

float movCX = 0.0f;
float movCZ = 0.0f;
float movCY = 0.0f;
float rotC  = 0.0f;

void saveFrame(void)
{

    printf("frameindex %d\n", FrameIndex);

    KeyFrame[FrameIndex].posX = posX;
    KeyFrame[FrameIndex].posY = posY;
    KeyFrame[FrameIndex].posZ = posZ;

    KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;


    FrameIndex++;
}

void resetElements(void)
{
    posX = KeyFrame[0].posX;
    posY = KeyFrame[0].posY;
    posZ = KeyFrame[0].posZ;

    rotRodIzq = KeyFrame[0].rotRodIzq;

}

void interpolation(void)
{

    KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
    KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
    KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

    KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;

}



int main( )
{
    // Init GLFW
    glfwInit( );
    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow( WIDTH, HEIGHT, "Proyecto Final", nullptr, nullptr );
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent( window );
    
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    
    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    
    // GLFW Options
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // OpenGL options
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
    Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
    Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");
    // Setup and compile our shaders
    Shader shader( "Shaders/modelLoading.vs", "Shaders/modelLoading.frag" );
    
    // Load models
    
    Model Television((char*)"Models/television/Old Tv Set nr1.obj");
    Model Mesa((char*)"Models/Table/Table_office.obj");
    Model Casa((char*)"Models/Casa/Casa.obj");
    Model Piso((char*)"Models/Piso/Piso.obj");
    Model Coche((char*)"Models/Coche/Coche.obj");
    Model Silla((char*)"Models/Silla/Silla.obj");
    Model Ventilador((char*)"Models/Lampara/VentiladorLampara.obj");
    Model Cama((char*)"Models/Cama/Cama.obj");
    Model Estufa((char*)"Models/Estufa/Estufa.obj");
    Model Lampara((char*)"Models/Lampara/Lampara.obj");
    Model Refri((char*)"Models/Refri/Refri.obj");
    Model Mueble((char*)"Models/MueblesX/Mueble.obj");
    Model Sofa((char*)"Models/Sofa/Sofa.obj");
    Model LamparaTecho((char*)"Models/Lampara/LamparaTecho.obj");
    Model Pizza((char*)"Models/Pizza/pizza.obj");
    Model Escoba((char*)"Models/Escoba/Escoba.obj");

    glm::mat4 projection = glm::perspective( camera.GetZoom( ), ( float )SCREEN_WIDTH/( float )SCREEN_HEIGHT, 0.1f, 100.0f );

    // First, set the container's VAO (and VBO)
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Texture Coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    // Game loop
    while (!glfwWindowShouldClose(window))
    {

        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        DoMovement();
        animacion();


        // Clear the colorbuffer
        glClearColor(0.164f, 0.862f, 0.937f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use cooresponding shader when setting uniforms/drawing objects
        lightingShader.Use();
        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        // Set material properties
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 1.0f);
        // == ==========================
        // Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
        // the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
        // by defining light types as classes and set their values in there, or by using a more efficient uniform approach
        // by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
        // == ==========================
        // Directional light
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 1.0f, 1.0f, 1.0f);


        // Point light 1
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.1f, 0.1f, 0.1f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), LightP1.x, LightP1.y, LightP1.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), LightP1.x, LightP1.y, LightP1.z);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);



        // Point light 2
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.01f, 0.01f, 0.01f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), LightP2.x, LightP2.y, LightP2.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), LightP2.x, LightP2.y, LightP2.z);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

        // Point light 3
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.01f, 0.01f, 0.01f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), LightP3.x, LightP3.y, LightP3.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), LightP3.x, LightP3.y, LightP3.z);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

        // Point light 4
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.01f, 0.01f, 0.01f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), LightP4.x, LightP4.y, LightP4.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), LightP4.x, LightP4.y, LightP4.z);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

        // SpotLight
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), 0.0f, -10.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(80.0f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(90.0f)));

        // Set material properties
        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 1.0f);

        // Create camera transformations
        glm::mat4 view;
        view = camera.GetViewMatrix();


        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glm::mat4 tmp = glm::mat4(1.0f); //Temp
        glm::mat4 model(1);

        //Carga de modelo 
        //Personaje
        view = camera.GetViewMatrix();

        model = glm::mat4(1);
        /*model = glm::scale(model, glm::vec3(1.0f));*/
        model = glm::translate(model, glm::vec3(0.0f, -20.3f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Piso.Draw(lightingShader);

        //Comedor
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-0.5f, 0.01f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Mesa.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-2.0f, -20.3f, 9.0f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Silla.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-2.0f, -20.3f, 14.0f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Silla.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-12.0f, -20.3f, 3.5f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Silla.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-12.0f, -20.3f, 8.5f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Silla.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-5.0f, -7.9f, 9.0f));
        model = glm::rotate(model, rotVent2, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Ventilador.Draw(lightingShader);

        //Sala
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(13.0f, -17.18f, 15.0f));
        model = glm::rotate(model, glm::radians(235.0f), glm::vec3(0.0f, 1.0f, 0.0));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Television.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(13.0f, -20.3, 15.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Mueble.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(13.9f, -19.3f, 3.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Lampara.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(13.9f, -20.3f, 3.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Mueble.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(7.0f, -7.9f, 9.0f));
        model = glm::rotate(model, rotVent1, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Ventilador.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(4.0f, -20.3f, 7.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Sofa.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(7.0f, -20.3f, 15.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Sofa.Draw(lightingShader);

        //Cochera
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(2.0f, -20.3f, -23.0f));
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Coche.Draw(lightingShader); 

        model = glm::mat4(1);
        model = glm::translate(model, posIniC + glm::vec3(movCX, movCY, movCZ));
        model = glm::rotate(model, glm::radians(rotC), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Escoba.Draw(lightingShader);

        //Cuarto
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(12.0f, -20.0f, -11.3f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Cama.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(8.0f, -5.8f, -9.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        LamparaTecho.Draw(lightingShader);

        //Cocina
        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-13.0f, -20.0f, -9.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Estufa.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-13.0f, -20.0f, -2.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Refri.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(-8.0f, -5.8f, -6.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        LamparaTecho.Draw(lightingShader);

        model = glm::mat4(1);
        model = glm::translate(model, posIniP+glm::vec3(movPizzaX,movPizzaY,movPizzaZ));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Pizza.Draw(lightingShader);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        model = glm::mat4(1);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        Casa.Draw(lightingShader);
        glEnable(GL_BLEND);
        glBindVertexArray(0);

        

        // Also draw the lamp object, again binding the appropriate shader
        lampShader.Use();
        // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
        modelLoc = glGetUniformLocation(lampShader.Program, "model");
        viewLoc = glGetUniformLocation(lampShader.Program, "view");
        projLoc = glGetUniformLocation(lampShader.Program, "projection");

        // Set matrices
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4(1);
        model = glm::translate(model, lightPos);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // Draw the light object (using light's vertex attributes)
        glBindVertexArray(lightVAO);
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate( );
    return 0;
}

// Moves/alters the camera positions based on user input
void animacion()
{

    //Movimiento del personaje

    if (play)
    {
        if (i_curr_steps >= i_max_steps) //end of animation between frames?
        {
            playIndex++;
            if (playIndex > FrameIndex - 2)	//end of total animation?
            {
                printf("termina anim\n");
                playIndex = 0;
                play = false;
            }
            else //Next frame interpolations
            {
                i_curr_steps = 0; //Reset counter
                                  //Interpolation
                interpolation();
            }
        }
        else
        {
            //Draw animation
            posX += KeyFrame[playIndex].incX;
            posY += KeyFrame[playIndex].incY;
            posZ += KeyFrame[playIndex].incZ;

            rotRodIzq += KeyFrame[playIndex].rotInc;

            i_curr_steps++;
        }

    }

    if (activeVent1) {
        if (velocidad1)
            rotVent1 += 0.05;
        else
            rotVent1 += 0.1;
    }
    if (activeVent2) {
        if (velocidad2)
            rotVent2 += 0.05;
        else
            rotVent2 += 0.1;
    }

    if (circuitoP)
    {
        if (recorridoP1)
        {
            movPizzaX += 0.1f;
            if (movPizzaX > 6)
            {
                recorridoP1 = false;
                recorridoP2 = true;
            }
        }

        if (recorridoP2)
        {
            
            movPizzaY += 0.1f;
            if (movPizzaY > 4)
            {
                recorridoP2 = false;
                recorridoP3 = true;
            }
        }

        if (recorridoP3)
        {
            movPizzaZ += 0.1f;
            if (movPizzaZ > 17.0f)
            {
                recorridoP3 = false;
                recorridoP4 = true;

            }
        }

        

        if (recorridoP4)
        {
            movPizzaY -= 0.1f;
            if (movPizzaY < 0.3f)
            {
                recorridoP4 = false;
                recorridoP5 = true;
            }
        }
    }

    if (circuitoC)
    {
        if (recorridoC1)
        {
            movCZ -= 0.1f;
            movCY = 10 * glm::sin(0.5 * movCZ);
            if (movCZ < -30.0f)
            {
                recorridoC1 = false;
                recorridoC2 = true;
            }
        }
        if (recorridoC2)
        {
            rotC = 90;
            movCX -= 0.1f;
            movCY = 10 * glm::sin(0.5 * movCX);
            if (movCX <-30.0f)
            {
                recorridoC2 = false;
                recorridoC3 = true;

            }
        }

        if (recorridoC3)
        {
            rotC = 180;
            movCZ += 0.1f;
            movCY = 10 * glm::sin(0.5 * movCZ);
            if (movCZ > 0)
            {
                recorridoC3 = false;
                recorridoC4 = true;
            }
        }

        if (recorridoC4)
        {
            rotC = 270;
            movCX += 0.1f;
            movCY = 10 * glm::sin(0.5 * movCX);
            if (movCX > 0)
            {
                recorridoC4 = false;
                recorridoC5 = true;
            }
        }
        if (recorridoC5)
        {
            rotC = 0;
            movCZ += 0.1f;
            if (movCZ > 0)
            {
                recorridoC5 = false;
                recorridoC1 = true;
            }
        }

    }
    
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (keys[GLFW_KEY_L])
    {
        if (play == false && (FrameIndex > 1))
        {

            resetElements();
            //First Interpolation				
            interpolation();

            play = true;
            playIndex = 0;
            i_curr_steps = 0;
        }
        else
        {
            play = false;
        }

    }

    if (keys[GLFW_KEY_K])
    {
        if (FrameIndex < MAX_FRAMES)
        {
            saveFrame();
        }

    }


    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }


    if (keys[GLFW_KEY_1])
    {
        activeLuz1 = !activeLuz1;
        if (activeLuz1) {
            LightP1 = glm::vec3(0.984f, 0.992f, 0.427f); 
        } 
        else {
            LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
        }
            
    }

    if (keys[GLFW_KEY_2])
    {
        activeLuz2 = !activeLuz2;
        if (activeLuz2) {
            LightP2 = glm::vec3(0.984f, 0.992f, 0.427f);
        }
            
        else {
            LightP2 = glm::vec3(0.0f, 0.0f, 0.0f);
        }         
    }

    if (keys[GLFW_KEY_3])
    {
        activeLuz3 = !activeLuz3;
        if (activeLuz3) {
            LightP3 = glm::vec3(0.984f, 0.992f, 0.427f);
        }

        else {
            LightP3 = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }

    if (keys[GLFW_KEY_4])
    {
        activeLuz4 = !activeLuz4;
        if (activeLuz4) {
            LightP4 = glm::vec3(0.984f, 0.992f, 0.427f);
        }

        else {
            LightP4 = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }

    if (keys[GLFW_KEY_5])
        activeVent1 = !activeVent1;

    if (keys[GLFW_KEY_6])
        activeVent2 = !activeVent2;

    if (keys[GLFW_KEY_KP_ADD]) {
        velocidad1 = !velocidad1;
    }

    if (keys[GLFW_KEY_KP_SUBTRACT]) {
        velocidad2 = !velocidad2;
    }

}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
    // Camera controls
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);

    }

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);


    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
    {
        camera.ProcessKeyboard(LEFT, deltaTime);


    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (keys[GLFW_KEY_Q])
        circuitoP = !circuitoP;
    if (keys[GLFW_KEY_E])
        circuitoC = !circuitoC;
}


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define  GLEW_STATIC
#include <GL/glew.h>
#include <pthread.h>

#include <GLFW/glfw3.h>

#define STATUS_BUFFER_LEN 512
#define SHADER_SRC_ARRAY_LEN 512

int loadShader(const char* filename, GLchar* loadBuffer);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

float vertices[] = {
  0.0f,  0.5f, 0.0f, // Vertex 1 (X, Y)
  0.5f, -0.5f, 0.0f, // Vertex 2 (X, Y)
  -0.5f, -0.5f, 0.0f  // Vertex 3 (X, Y)
};

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /*
     * Configure window options here if you so desire
     *
     * i.e.
     */
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
     //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
     
     //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
     

    //the fourth parameter of glfwCreateWindow should be NULL for windowed mode and 
    //glfGetPrimaryMonitor() for full screen mode
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /*
     * Initialize glew here
     */

    glewExperimental = GL_TRUE;
    glewInit();

    //Create your vertices. Each vertex is a point on the screen related to our primitive.
    //(SEE ABOVE)

    //Create a vertex buffer object (VBO)
    GLuint vbo;
    printf("%s", "Generating vertex buffer object... \n");
    glGenBuffers(1, &vbo); // Generate a vbo with ID 1
    printf("VBO #%u has been created\n", vbo);

    //Bind all of your vertex objects to a vertex object array 
    //(this will save all the attributes from the vbo so that, the next time you activate the desired 
    //program which contains the desired shaders, you won't have to reload all the attributes from 
    //the desired vertex object)
    GLuint vao;
    printf("Generating vertex array object\n");
    glGenVertexArrays(1, &vao);
    printf("Binding vertex array object\n");
    glBindVertexArray(vao);

    //Bind your vertex buffer object to the active buffer array
    //printf("Binding vbo #%u to active buffer array\n", vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo); 
    //printf("Vbo #%u has been boung to active buffer array\n", vbo);
      
    //Upload vertices to the active buffer array (which is bound to the VBO you created earlier)
    printf("Uploading vertices to the active buffer array...\n");
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    printf("Vertices uploaded successfully\n");

    //Create or Load your vertex and fragment shaders
   
    printf("Loading shaders...\n"); 
    int ret;
    GLchar vertexSource[SHADER_SRC_ARRAY_LEN]; 
    const char* vertex_filename = "vertex_shader.cl";
    if((ret = loadShader(vertex_filename, vertexSource)) != 0){
      return ret; 
    } 
    printf("Vertex shader loaded successfully\n");

    GLchar fragmentSource[SHADER_SRC_ARRAY_LEN]; 
    const char* frag_filename = "fragment_shader.cl";
    if((ret = loadShader(frag_filename, fragmentSource)) != 0){
      return ret; 
    }
    printf("Fragment shader loaded successfully\n");
    
    //Assign your vertexSource pointer to a pointer array for glShaderSource
    printf("Attaching shaders to array\n");
    const GLchar* vertexArray[1] = {vertexSource};
    const GLchar* fragmentArray[1] = {fragmentSource};

    //Create your shader objects
    printf("Creating shader objects\n");
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);


    //Load data into shader object
    printf("Loading GLSL scripts into shaders\n");
    printf("Loading VertexSource"); 
    glShaderSource(vertexShader, 1, vertexArray, NULL);
    printf("Loading FragmentSource"); 
    glShaderSource(fragmentShader, 1, fragmentArray, NULL);

    //Compile your shaders
    printf("Compiling shaders...\n");
    glCompileShader(vertexShader);      
    glCompileShader(fragmentShader);

    //get the compile logs
    printf("Getting shader compilation logs...\n");
    GLint vertex_status, fragment_status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertex_status);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragment_status);


    //free memory allocated to vertexSource
    if(vertex_status != GL_TRUE){
      // print shader logs then exit
       char vertex_stat_buff[STATUS_BUFFER_LEN];
       glGetShaderInfoLog(vertexShader, STATUS_BUFFER_LEN, NULL, vertex_stat_buff);
       printf("%s", "\n\nATTN: VERTEX SHADER COMPILATION FAILED \
                         PLEASE SEE LOG AS FOLLOWS:\n\n");
       printf("%s", vertex_stat_buff);
       return -1;
    } else if (fragment_status != GL_TRUE){
       char fragment_stat_buff[STATUS_BUFFER_LEN];
       glGetShaderInfoLog(fragmentShader, STATUS_BUFFER_LEN, NULL, fragment_stat_buff);
       printf("%s", "\n\nATTN: VERTEX SHADER COMPILATION FAILED \
                         PLEASE SEE LOG AS FOLLOWS:\n\n");
       printf("%s", fragment_stat_buff);
       return -1;
    } else {
      printf("Vertex shader compiled successfully\n");
      printf("Fragment shader compiled successsfully\n");
      //free(vertexSource);
      //free(fragmentSource);
    }

    //Create a program
    printf("Creating program...\n");
    GLuint program = glCreateProgram();
    
    //Attach desired shaders to the program
    printf("Attaching shaders to program...\n");
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
     
    //Link the program
    printf("Linking program...\n");
    glLinkProgram(program);
    printf("Program linked successfully...\n");

    GLint linker_status;
    glGetProgramiv(program, GL_LINK_STATUS, &linker_status);
    
    char infoLog[STATUS_BUFFER_LEN];
    if(!linker_status){
      glGetProgramInfoLog(program, 512, NULL, infoLog);
      printf("%s", "\n\nATTN: PROGRAM LINKING FAILED \
                         PLEASE SEE LOG AS FOLLOWS:\n\n");
      printf("%s", infoLog);
      return -1;
    }
   
    //Use the program
    glUseProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //Retrieve the attributes from your vertex object and send it to the vertex shader
    
    printf("Retrieving vertex attributes...\n");
    //GLint posAttrib = glGetAttribLocation(program, "pos"); //"pos" is declared in vertex shader
    
    printf("Vertex attribute retrieved\n");
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  
    
    printf("Enabling vertex attribute\n");
    glEnableVertexAttribArray(0);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);

    //Bind your desired output buffer to the fragment shader
    //printf("Binding fragment output to buffer...\n");
    glBindFragDataLocation(program, 0, "outColor"); 
   
    printf("Launching OpenGL context and drawing primitive\n");

    /* Loop until the user closes the window */
       while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glUseProgram(program);
        //glBindVertexArray(vao);
        
        //Draw the shape
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
           glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}

unsigned long getFileLen(const char* filename){

  printf("Opening file: %s\n", filename);
  FILE* fp = fopen(filename, "r");

  if(fp == NULL){
    return 1;
  }

  unsigned long pos = 0;
  char ch;
  printf("Getting file length of file: %s\n", filename);
  //printf("Shader contents is as follows:\n\n");
  while((ch = fgetc(fp)) != EOF){
    //printf("%c", ch);
    ++pos; 
  };

  printf("\nClosing file\n");
  fclose(fp);
  return pos;
}


int loadShader(const char* filename, GLchar* loadBuffer){

  int len = getFileLen(filename);

  if(len == 1){
    return -1;
  } else {
    printf("File length: %d characters\n", len);
  }

  printf("Allocating memory for reading shader...\n"); 
  //*loadBuffer = (GLchar*)malloc(len * sizeof(GLchar));

  if(loadBuffer == 0){
    printf("Memory unable to be allocated\n");
    return -3;
  }
  printf("Memory allocated successfully\n");

  //*loadBuffer[len -1] = '\0';

  printf("Opening file for read\n");
  FILE* fp = fopen(filename, "r");

  printf("Reading file: %s...\n", filename);
  
  //fgets(*loadBuffer, len -1, fp);
 
  printf("Shader contents is as follows:\n\n");
  int i = 0;
  char ch; 
  while((ch = fgetc(fp)) != EOF){
    loadBuffer[i] = ch;
    printf("%c", loadBuffer[i]);
    ++i;
  };

  loadBuffer[len-1] = 0;


  //printf("Shader contents is as follows:\n\n");
  //for(int i = 0; i < len -1; i++){
  //  printf("%c", loadBuffer[i]);
  //}

  /* 
  for(int i = 0; i < len -1; i++){
    printf("char #%d\n", i);
    *loadBuffer[i] = fgetc(fp); 
    if(*loadBuffer[i] != EOF){
      continue;
    } else {
      *loadBuffer[i] = 0;
    }
  }
  */

  printf("File read successfully... \nClosing file...\n");
  fclose(fp);

  return 0; 
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}





/*

unsigned long getFileLength(ifstream& file)
{
    if(!file.good()) return 0;
    
    unsigned long pos=file.tellg();
    file.seekg(0,ios::end);
    unsigned long len = file.tellg();
    file.seekg(ios::beg);
    
    return len;
}

int loadshader(char* filename, GLchar** ShaderSource, unsigned long* len)
{
   ifstream file;
   file.open(filename, ios::in); // opens as ASCII!
   if(!file) return -1;
    
   len = getFileLength(file);
    
   if (len==0) return -2;   // Error: Empty File 
    
   *ShaderSource = (GLubyte*) new char[len+1];
   if (*ShaderSource == 0) return -3;   // can't reserve memory
   
    // len isn't always strlen cause some characters are stripped in ascii read...
    // it is important to 0-terminate the real length later, len is just max possible value... 
   *ShaderSource[len] = 0; 

   unsigned int i=0;
   while (file.good())
   {
       *ShaderSource[i] = file.get();       // get character from file.
       if (!file.eof())
        i++;
   }
    
   *ShaderSource[i] = 0;  // 0-terminate it at the correct position
    
   file.close();
      
   return 0; // No Error
}


int unloadshader(GLubyte** ShaderSource)
{
   if (*ShaderSource != 0)
     delete[] *ShaderSource;
   *ShaderSource = 0;
}

*/


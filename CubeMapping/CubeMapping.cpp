#include "CubeMapping.h"

//------------------------------------------------------------------
//
// Camera Class
//
//------------------------------------------------------------------

CameraGL::CameraGL() : 
   CameraGL(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f))
{   
}

CameraGL::CameraGL(
   const vec3& cam_position,
   const vec3& view_reference_position,
   const vec3& view_up_vector,
   float fov,
   float near_plane,
   float far_plane
) : 
   ZOOM_SENSITIVITY( 2.0f ), MOVE_SENSITIVITY( 5.0f ), ROTATE_SENSITIVITY( 0.01f ), IsMoving( false ),
   AspectRatio( 0.0f ), InitFOV( fov ), NearPlane( near_plane ), FarPlane( far_plane ), 
   InitCamPos( cam_position ), InitRefPos( view_reference_position ), InitUpVec( view_up_vector ), 
   FOV( fov ), CamPos( cam_position ), RefPos( view_reference_position ), UpVec( view_up_vector ),
   ViewMatrix( lookAt( CamPos, RefPos, UpVec ) ), ProjectionMatrix( mat4(1.0f) )
{
}

bool CameraGL::getMovingState() const
{
   return IsMoving;
}

void CameraGL::setMovingState(bool is_moving)
{
   IsMoving = is_moving;
}

void CameraGL::pitch(int angle)
{
   const vec3 u_axis(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
   ViewMatrix = glm::rotate( ViewMatrix, static_cast<float>(angle) * ROTATE_SENSITIVITY, u_axis );
}

void CameraGL::yaw(int angle)
{
   const vec3 v_axis(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);
   ViewMatrix = glm::rotate( ViewMatrix, static_cast<float>(angle) * ROTATE_SENSITIVITY, v_axis );
}

void CameraGL::moveForward()
{
   ViewMatrix = translate( ViewMatrix, MOVE_SENSITIVITY * vec3( 0.0f, 0.0f, -1.0f ) );
}

void CameraGL::moveBackward()
{
   ViewMatrix = translate( ViewMatrix, MOVE_SENSITIVITY * vec3( 0.0f, 0.0f, 1.0f ) );
}

void CameraGL::moveLeft()
{
   ViewMatrix = translate( ViewMatrix, MOVE_SENSITIVITY * vec3( 1.0f, 0.0f, 0.0f ) );
}

void CameraGL::moveRight()
{
   ViewMatrix = translate( ViewMatrix, MOVE_SENSITIVITY * vec3( -1.0f, 0.0f, 0.0f ) );
}

void CameraGL::moveUp()
{
   ViewMatrix = translate( ViewMatrix, MOVE_SENSITIVITY * vec3( 0.0f, -1.0f, 0.0f ) );
}

void CameraGL::moveDown()
{
   ViewMatrix = translate( ViewMatrix, MOVE_SENSITIVITY * vec3( 0.0f, 1.0f, 0.0f ) );
}

void CameraGL::zoomIn()
{
   if (FOV > 0.0f) {
      FOV -= ZOOM_SENSITIVITY;
      ProjectionMatrix = perspective( radians( FOV ), AspectRatio, NearPlane, FarPlane );
   }
}

void CameraGL::zoomOut()
{
   if (FOV < 90.0f) {
      FOV += ZOOM_SENSITIVITY;
      ProjectionMatrix = perspective( radians( FOV ), AspectRatio, NearPlane, FarPlane );
   }
}

void CameraGL::resetCamera()
{
   ViewMatrix = lookAt( InitCamPos, InitRefPos, InitUpVec );
   ProjectionMatrix = perspective( radians( InitFOV ), AspectRatio, NearPlane, FarPlane );
}

void CameraGL::updateWindowSize(int width, int height)
{
   AspectRatio = static_cast<float>(width) / height;
   ProjectionMatrix = perspective( radians( FOV ), AspectRatio, NearPlane, FarPlane );
}


//------------------------------------------------------------------
//
// Object Class
//
//------------------------------------------------------------------

ObjectGL::ObjectGL() : ObjVAO( 0 ), ObjVBO( 0 ), DrawMode( 0 ), TextureID( 0 ), VerticesCount( 0 ), Colors{}
{
}

void ObjectGL::prepareCubeTextures(const vector<Mat>& cube_images)
{
   glGenTextures( 1, &TextureID );
   glBindTexture( GL_TEXTURE_CUBE_MAP, TextureID );

   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );    
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0 ); 
   glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0 ); 

   glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, cube_images[0].cols, cube_images[0].rows, 0, GL_BGR, GL_UNSIGNED_BYTE, cube_images[0].data );
   glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, cube_images[1].cols, cube_images[1].rows, 0, GL_BGR, GL_UNSIGNED_BYTE, cube_images[1].data );
   glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, cube_images[2].cols, cube_images[2].rows, 0, GL_BGR, GL_UNSIGNED_BYTE, cube_images[2].data );
   glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, cube_images[3].cols, cube_images[3].rows, 0, GL_BGR, GL_UNSIGNED_BYTE, cube_images[3].data );
   glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, cube_images[4].cols, cube_images[4].rows, 0, GL_BGR, GL_UNSIGNED_BYTE, cube_images[4].data );
   glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, cube_images[5].cols, cube_images[5].rows, 0, GL_BGR, GL_UNSIGNED_BYTE, cube_images[5].data );

   glGenerateMipmap( GL_TEXTURE_CUBE_MAP );
}

void ObjectGL::prepareVertexBuffer(const int& n_bytes_per_vertex)
{
   glGenBuffers( 1, &ObjVBO );
   glBindBuffer( GL_ARRAY_BUFFER, ObjVBO );
   glBufferData( GL_ARRAY_BUFFER, sizeof(GLfloat) * DataBuffer.size(), DataBuffer.data(), GL_STATIC_DRAW );
   glBindBuffer( GL_ARRAY_BUFFER, 0 );

   glGenVertexArrays( 1, &ObjVAO );
   glBindVertexArray( ObjVAO );
   glBindBuffer( GL_ARRAY_BUFFER, ObjVBO );
   glVertexAttribPointer( VertexLoc, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, bufferOffset( 0 ) );
   glEnableVertexAttribArray( VertexLoc );

   glBindBuffer( GL_ARRAY_BUFFER, 0 );
   glBindVertexArray( 0 );
}

void ObjectGL::setObject(
   GLenum draw_mode, 
   const vec3& color, 
   const vector<vec3>& vertices,
   const string& texture_directory_path
)
{
   DrawMode = draw_mode;
   Colors = { color.r, color.g, color.b };
   for (const auto& vertex : vertices) {
      DataBuffer.push_back( vertex.x );
      DataBuffer.push_back( vertex.y );
      DataBuffer.push_back( vertex.z );
      VerticesCount++;
   }
   const int n_bytes_per_vertex = 3 * sizeof(GLfloat);
   prepareVertexBuffer( n_bytes_per_vertex );

   const vector<Mat> cube_images = {
      imread( texture_directory_path + "/right.jpg" ),
      imread( texture_directory_path + "/left.jpg" ),
      imread( texture_directory_path + "/top.jpg" ),
      imread( texture_directory_path + "/bottom.jpg" ),
      imread( texture_directory_path + "/back.jpg" ),
      imread( texture_directory_path + "/front.jpg" )
   };
   prepareCubeTextures( cube_images );
}

void ObjectGL::setVideoObject(
   GLenum draw_mode, 
   const vec3& color, 
   const vector<glm::vec3>& vertices,
   const string& texture_video_path
)
{
   DrawMode = draw_mode;
   Colors = { color.r, color.g, color.b };
   for (const auto& vertex : vertices) {
      DataBuffer.push_back( vertex.x );
      DataBuffer.push_back( vertex.y );
      DataBuffer.push_back( vertex.z );
      VerticesCount++;
   }
   const int n_bytes_per_vertex = 3 * sizeof(GLfloat);
   prepareVertexBuffer( n_bytes_per_vertex );

   Videos = {
      VideoCapture(texture_video_path + "/right.avi"),
      VideoCapture(texture_video_path + "/left.avi"),
      VideoCapture(texture_video_path + "/top.avi"),
      VideoCapture(texture_video_path + "/bottom.avi"),
      VideoCapture(texture_video_path + "/back.avi"),
      VideoCapture(texture_video_path + "/front.avi")
   };
   
   vector<Mat> cube_images(6);
   for (uint i = 0; i < cube_images.size(); ++i) {
      Videos[i] >> cube_images[i];
   }
   prepareCubeTextures( cube_images );
}

void ObjectGL::updateTextures()
{
   for (uint i = 0; i < Videos.size(); ++i) {
      Mat frame;
      Videos[i] >> frame;
      if (frame.empty()) continue;
      glTexSubImage2D(
         GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
         0, 0, 0, frame.cols, frame.rows, 
         GL_BGR, GL_UNSIGNED_BYTE, frame.data
      );
      
   }
}


//------------------------------------------------------------------
//
// Shader Class
//
//------------------------------------------------------------------

ShaderGL::ShaderGL() : 
   ShaderProgram( 0 ), MVPLocation( 0 ), ColorLocation( 0 ), TextureLocation( 0 )
{
}

void ShaderGL::setShader(const GLchar* const vertex_source, const GLchar* const fragment_source)
{
   const GLuint vertex_shader = glCreateShader( GL_VERTEX_SHADER );
   const GLuint fragment_shader = glCreateShader( GL_FRAGMENT_SHADER );

   glShaderSource( vertex_shader, 1, &vertex_source, nullptr );
   glShaderSource( fragment_shader, 1, &fragment_source, nullptr );
   glCompileShader( vertex_shader );
   glCompileShader( fragment_shader );

   ShaderProgram = glCreateProgram();
   glAttachShader( ShaderProgram, vertex_shader );
   glAttachShader( ShaderProgram, fragment_shader );
   glLinkProgram( ShaderProgram );

   MVPLocation = glGetUniformLocation( ShaderProgram, "ModelViewProjectionMatrix" );
   ColorLocation = glGetUniformLocation( ShaderProgram, "PrimitiveColor" );
   TextureLocation = glGetUniformLocation( ShaderProgram, "CubeTexture" );

   glDeleteShader( vertex_shader );
   glDeleteShader( fragment_shader );
}


//------------------------------------------------------------------
//
// Renderer Class
//
//------------------------------------------------------------------

CubeMapping* CubeMapping::Renderer = nullptr;
CubeMapping::CubeMapping(const bool& is_video) : Window( nullptr ), IsVideo( is_video )
{
   Renderer = this;

   initialize();
   printOpenGLInformation();
}

CubeMapping::~CubeMapping()
{
   glfwTerminate();
}

void CubeMapping::printOpenGLInformation() const
{
   cout << "****************************************************************" << endl;
   cout << " - GLFW version supported: " << glfwGetVersionString() << endl;
   cout << " - GLEW version supported: " << glewGetString( GLEW_VERSION ) << endl;
   cout << " - OpenGL renderer: " << glGetString( GL_RENDERER ) << endl;
   cout << " - OpenGL version supported: " << glGetString( GL_VERSION ) << endl;
   cout << " - OpenGL shader version supported: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << endl  ;
   cout << "****************************************************************" << endl << endl;
}

void CubeMapping::setCubeShader()
{
   const GLchar* const vertex_source = {
      "#version 330                                             \n"
      "uniform mat4 ModelViewProjectionMatrix;                  \n"
      "uniform vec3 PrimitiveColor;                             \n"
      "layout (location = 0) in vec4 v_position;                \n"
      "out vec4 color;                                          \n"
      "out vec3 tex_coord;                                      \n"
      "void main(void) {                                        \n"
      "	color = vec4( PrimitiveColor, 1.0f );                  \n"
      "	tex_coord = normalize( v_position.xyz );               \n"
      "	gl_Position =  ModelViewProjectionMatrix * v_position; \n"
      "}                                                        \n"
   };
   const GLchar* const fragment_source = {
      "#version 330                                       \n"
      "uniform samplerCube CubeTexture;                   \n"
      "in vec4 color;                                     \n"
      "in vec3 tex_coord;                                 \n"
      "layout (location = 0) out vec4 final_color;        \n"
      "void main(void) {                                  \n"
      "	final_color = texture( CubeTexture, tex_coord ); \n"
      "}                                                  \n"
   };

   CubeShader.setShader( vertex_source, fragment_source );
}

void CubeMapping::setCubeObject(const float& length)
{
   const vec3 cube_color = { 0.5f, 0.125f, 0.9f };
   const vector<vec3> cube_vertices{
      { -length, length, -length },
      { -length, -length, -length },
      { length, -length, -length },
      { length, -length, -length },
      { length, length, -length },
      { -length, length, -length },

      { -length, -length, length },
      { -length, -length, -length },
      { -length, length, -length },
      { -length, length, -length },
      { -length, length, length },
      { -length, -length, length },

      { length, -length, -length },
      { length, -length, length },
      { length, length, length },
      { length, length, length },
      { length, length, -length },
      { length, -length, -length },

      { -length, -length, length },
      { -length, length, length },
      { length, length, length },
      { length, length, length },
      { length, -length, length },
      { -length, -length, length },

      { -length, length, -length },
      { length, length, -length },
      { length, length, length },
      { length, length, length },
      { -length, length, length },
      { -length, length, -length },

      { -length, -length, -length },
      { -length, -length, length },
      { length, -length, -length },
      { length, -length, -length },
      { -length, -length, length },
      { length, -length, length }
   };

   if (IsVideo) {
      CubeObject.setVideoObject( GL_TRIANGLES, cube_color, cube_vertices, "Textures/dynamic/" );
   }
   else {
      CubeObject.setObject( GL_TRIANGLES, cube_color, cube_vertices, "Textures/static/sample1/" );
   }
}

void CubeMapping::initialize()
{
   const int width = 1920;
   const int height = 1080;
   initializeOpenGL( width, height );
   registerCallbacks();

   setCubeShader();
   setCubeObject( 5.0f );
}

void CubeMapping::initializeOpenGL(const int& width, const int& height)
{
   if (!glfwInit()) {
      cout << "Cannot Initialize OpenGL..." << endl;
      return;
   }
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
   glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

   Window = glfwCreateWindow( width, height, "Cube Mapping", nullptr, nullptr );
   glfwMakeContextCurrent( Window );
   glewInit();
   
   glEnable( GL_DEPTH_TEST );
   glEnable( GL_TEXTURE_CUBE_MAP );
   glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

   MainCamera.updateWindowSize( width, height );
}

void CubeMapping::error(int error, const char* description) const
{
   puts( description );
}

void CubeMapping::errorWrapper(int error, const char* description)
{
   Renderer->error( error, description );
}

void CubeMapping::cleanup(GLFWwindow* window)
{
   glDeleteProgram( CubeShader.ShaderProgram );
   glDeleteVertexArrays( 1, &CubeObject.ObjVAO );
   glDeleteBuffers( 1, &CubeObject.ObjVBO );
   
   glfwSetWindowShouldClose( window, GLFW_TRUE );
}

void CubeMapping::cleanupWrapper(GLFWwindow* window)
{
   Renderer->cleanup( window );
}

void CubeMapping::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   switch (key) {
      case GLFW_KEY_UP:
         MainCamera.moveForward();
         break;
      case GLFW_KEY_DOWN:
         MainCamera.moveBackward();
         break;
      case GLFW_KEY_LEFT:
         MainCamera.moveLeft();
         break;
      case GLFW_KEY_RIGHT:
         MainCamera.moveRight();
         break;
      case GLFW_KEY_W:
         MainCamera.moveUp();
         break;
      case GLFW_KEY_S:
         MainCamera.moveDown();
         break;
      case GLFW_KEY_I:
         MainCamera.resetCamera();
         break;
      case GLFW_KEY_Q:
      case GLFW_KEY_ESCAPE:
         cleanupWrapper( window );
         break;
      default:
         return;
   }
}

void CubeMapping::keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   Renderer->keyboard( window, key, scancode, action, mods );
}

void CubeMapping::cursor(GLFWwindow* window, double xpos, double ypos)
{
   if (MainCamera.getMovingState()) {
      const int dx = static_cast<int>(round( xpos )) - ClickedPoint.x;
      const int dy = static_cast<int>(round( ypos )) - ClickedPoint.y;
      MainCamera.pitch( dy );
      MainCamera.yaw( dx );
   }
}

void CubeMapping::cursorWrapper(GLFWwindow* window, double xpos, double ypos)
{
   Renderer->cursor( window, xpos, ypos );
}

void CubeMapping::mouse(GLFWwindow* window, int button, int action, int mods)
{
   if (button == GLFW_MOUSE_BUTTON_LEFT) {
      const bool moving_state = action == GLFW_PRESS;
      if (moving_state) {
         double x, y;
         glfwGetCursorPos( window, &x, &y );
         ClickedPoint = Point(static_cast<int>(round( x )), static_cast<int>(round( y )));
      }
      MainCamera.setMovingState( moving_state );
   }
}

void CubeMapping::mouseWrapper(GLFWwindow* window, int button, int action, int mods)
{
   Renderer->mouse( window, button, action, mods );
}

void CubeMapping::mousewheel(GLFWwindow* window, double xoffset, double yoffset)
{
   if (yoffset >= 0.0) {
      MainCamera.zoomIn();
   }
   else {
      MainCamera.zoomOut();
   }
}

void CubeMapping::mousewheelWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
   Renderer->mousewheel( window, xoffset, yoffset );
}

void CubeMapping::reshape(GLFWwindow* window, int width, int height)
{
   MainCamera.updateWindowSize( width, height );
   glViewport( 0, 0, width, height );
}

void CubeMapping::reshapeWrapper(GLFWwindow* window, int width, int height)
{
   Renderer->reshape( window, width, height );
}

void CubeMapping::registerCallbacks() const
{
   glfwSetErrorCallback( errorWrapper );
   glfwSetWindowCloseCallback( Window, cleanupWrapper );
   glfwSetKeyCallback( Window, keyboardWrapper );
   glfwSetCursorPosCallback( Window, cursorWrapper );
   glfwSetMouseButtonCallback( Window, mouseWrapper );
   glfwSetScrollCallback( Window, mousewheelWrapper );
   glfwSetFramebufferSizeCallback( Window, reshapeWrapper );
}

void CubeMapping::render()
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   const mat4 model_view_projection = MainCamera.ProjectionMatrix * MainCamera.ViewMatrix;
   
   glUseProgram( CubeShader.ShaderProgram );
   glUniformMatrix4fv( CubeShader.MVPLocation, 1, GL_FALSE, &model_view_projection[0][0] );
   
   glBindVertexArray( CubeObject.ObjVAO );
   glUniform3fv( CubeShader.ColorLocation, 1, value_ptr( CubeObject.Colors ) );
   if (IsVideo) CubeObject.updateTextures();
   glBindTexture( GL_TEXTURE_CUBE_MAP, CubeObject.TextureID );
   glDrawArrays( CubeObject.DrawMode, 0, CubeObject.VerticesCount );
   glBindVertexArray( 0 );
   glUseProgram( 0 );
}

void CubeMapping::play()
{
   if (glfwWindowShouldClose( Window )) initialize();
   
   while (!glfwWindowShouldClose( Window )) {
      render();
      
      glfwSwapBuffers( Window );
      glfwPollEvents();
   }
   glfwDestroyWindow( Window );
}
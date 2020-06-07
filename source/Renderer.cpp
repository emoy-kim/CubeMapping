#include "Renderer.h"

RendererGL::RendererGL() : 
   Window( nullptr ), FrameWidth( 1920 ), FrameHeight( 1080 ), IsVideo( false ), ClickedPoint( -1, -1 ),
   MainCamera( std::make_unique<CameraGL>() ), ObjectShader( std::make_unique<ShaderGL>() ),
   CubeObject( std::make_unique<ObjectGL>() )
{
   Renderer = this;

   initialize();
   printOpenGLInformation();
}

RendererGL::~RendererGL()
{
   glfwTerminate();
}

void RendererGL::printOpenGLInformation() const
{
   std::cout << "****************************************************************\n";
   std::cout << " - GLFW version supported: " << glfwGetVersionString() << "\n";
   std::cout << " - OpenGL renderer: " << glGetString( GL_RENDERER ) << "\n";
   std::cout << " - OpenGL version supported: " << glGetString( GL_VERSION ) << "\n";
   std::cout << " - OpenGL shader version supported: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n";
   std::cout << "****************************************************************\n\n";
}

void RendererGL::initialize()
{
   if (!glfwInit()) {
      std::cout << "Cannot Initialize OpenGL...\n";
      return;
   }
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
   glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

   Window = glfwCreateWindow( FrameWidth, FrameHeight, "Main Camera", nullptr, nullptr );
   glfwMakeContextCurrent( Window );

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return;
   }
   
   registerCallbacks();
   
   glEnable( GL_DEPTH_TEST );
   glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

   MainCamera->updateWindowSize( FrameWidth, FrameHeight );

   const std::string shader_directory_path = std::string(CMAKE_SOURCE_DIR) + "/shaders";
   ObjectShader->setShader(
      std::string(shader_directory_path + "/BasicPipeline.vert").c_str(),
      std::string(shader_directory_path + "/BasicPipeline.frag").c_str()
   );
}

void RendererGL::error(int error, const char* description) const
{
   puts( description );
}

void RendererGL::errorWrapper(int error, const char* description)
{
   Renderer->error( error, description );
}

void RendererGL::cleanup(GLFWwindow* window)
{
   glfwSetWindowShouldClose( window, GLFW_TRUE );
}

void RendererGL::cleanupWrapper(GLFWwindow* window)
{
   Renderer->cleanup( window );
}

void RendererGL::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   switch (key) {
      case GLFW_KEY_UP:
         MainCamera->moveForward();
         break;
      case GLFW_KEY_DOWN:
         MainCamera->moveBackward();
         break;
      case GLFW_KEY_LEFT:
         MainCamera->moveLeft();
         break;
      case GLFW_KEY_RIGHT:
         MainCamera->moveRight();
         break;
      case GLFW_KEY_W:
         MainCamera->moveUp();
         break;
      case GLFW_KEY_S:
         MainCamera->moveDown();
         break;
      case GLFW_KEY_I:
         MainCamera->resetCamera();
         break;
      case GLFW_KEY_P: {
         const glm::vec3 pos = MainCamera->getCameraPosition();
         std::cout << "Camera Position: " << pos.x << ", " << pos.y << ", " << pos.z << "\n";
      } break;
      case GLFW_KEY_Q:
      case GLFW_KEY_ESCAPE:
         cleanupWrapper( window );
         break;
      default:
         return;
   }
}

void RendererGL::keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   Renderer->keyboard( window, key, scancode, action, mods );
}

void RendererGL::cursor(GLFWwindow* window, double xpos, double ypos)
{
   if (MainCamera->getMovingState()) {
      const auto x = static_cast<int>(round( xpos ));
      const auto y = static_cast<int>(round( ypos ));
      const int dx = x - ClickedPoint.x;
      const int dy = y - ClickedPoint.y;
      MainCamera->moveForward( -dy );
      MainCamera->rotateAroundWorldY( -dx );

      if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS) {
         MainCamera->pitch( -dy );
      }

      ClickedPoint.x = x;
      ClickedPoint.y = y;
   }
}

void RendererGL::cursorWrapper(GLFWwindow* window, double xpos, double ypos)
{
   Renderer->cursor( window, xpos, ypos );
}

void RendererGL::mouse(GLFWwindow* window, int button, int action, int mods)
{
   if (button == GLFW_MOUSE_BUTTON_LEFT) {
      const bool moving_state = action == GLFW_PRESS;
      if (moving_state) {
         double x, y;
         glfwGetCursorPos( window, &x, &y );
         ClickedPoint.x = static_cast<int>(round( x ));
         ClickedPoint.y = static_cast<int>(round( y ));
      }
      MainCamera->setMovingState( moving_state );
   }
}

void RendererGL::mouseWrapper(GLFWwindow* window, int button, int action, int mods)
{
   Renderer->mouse( window, button, action, mods );
}

void RendererGL::mousewheel(GLFWwindow* window, double xoffset, double yoffset) const
{
   if (yoffset >= 0.0) MainCamera->zoomIn();
   else MainCamera->zoomOut();
}

void RendererGL::mousewheelWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
   Renderer->mousewheel( window, xoffset, yoffset );
}

void RendererGL::reshape(GLFWwindow* window, int width, int height) const
{
   MainCamera->updateWindowSize( width, height );
   glViewport( 0, 0, width, height );
}

void RendererGL::reshapeWrapper(GLFWwindow* window, int width, int height)
{
   Renderer->reshape( window, width, height );
}

void RendererGL::registerCallbacks() const
{
   glfwSetErrorCallback( errorWrapper );
   glfwSetWindowCloseCallback( Window, cleanupWrapper );
   glfwSetKeyCallback( Window, keyboardWrapper );
   glfwSetCursorPosCallback( Window, cursorWrapper );
   glfwSetMouseButtonCallback( Window, mouseWrapper );
   glfwSetScrollCallback( Window, mousewheelWrapper );
   glfwSetFramebufferSizeCallback( Window, reshapeWrapper );
}

void RendererGL::setCubeObject(float length) const
{
   const std::vector<glm::vec3> cube_vertices{
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

   const std::string sample_directory_path = std::string(CMAKE_SOURCE_DIR) + "/samples";
   std::vector<std::string> texture_set;
   if (IsVideo) {
      const std::string texture_set_path = std::string(sample_directory_path + "/dynamic");
      texture_set = {
         std::string(texture_set_path + "/right.avi"),
         std::string(texture_set_path + "/left.avi"),
         std::string(texture_set_path + "/top.avi"),
         std::string(texture_set_path + "/bottom.avi"),
         std::string(texture_set_path + "/back.avi"),
         std::string(texture_set_path + "/front.avi")
      };
      CubeObject->setVideoObject( GL_TRIANGLES, cube_vertices, texture_set );
   }
   else {
      const std::string texture_set_path = std::string(sample_directory_path + "/static/sample1");
      texture_set = {
         std::string(texture_set_path + "/right.jpg"),
         std::string(texture_set_path + "/left.jpg"),
         std::string(texture_set_path + "/top.jpg"),
         std::string(texture_set_path + "/bottom.jpg"),
         std::string(texture_set_path + "/back.jpg"),
         std::string(texture_set_path + "/front.jpg")
      };
      CubeObject->setCubeObject( GL_TRIANGLES, cube_vertices, texture_set );
   }
   CubeObject->setDiffuseReflectionColor( { 1.0f, 1.0f, 1.0f, 1.0f } );
}


void RendererGL::drawCubeObject() const
{
   MainCamera->updateWindowSize( FrameWidth, FrameHeight );
   glViewport( 0, 0, FrameWidth, FrameHeight );

   glBindFramebuffer( GL_FRAMEBUFFER, 0 );
   glUseProgram( ObjectShader->getShaderProgram() );
   
   glUseProgram( ObjectShader->getShaderProgram() );
   ObjectShader->transferBasicTransformationUniforms( glm::mat4(1.0f), MainCamera.get(), true );

   if (IsVideo) CubeObject->updateVideoCubeTextures();
   CubeObject->transferUniformsToShader( ObjectShader.get() );

   glBindTextureUnit( 0, CubeObject->getTextureID( 0 ) );
   glBindVertexArray( CubeObject->getVAO() );
   glDrawArrays( CubeObject->getDrawMode(), 0, CubeObject->getVertexNum() );
}

void RendererGL::render() const
{
   glClear( OPENGL_COLOR_BUFFER_BIT | OPENGL_DEPTH_BUFFER_BIT );

   drawCubeObject();

   glBindVertexArray( 0 );
   glUseProgram( 0 );
}

void RendererGL::play()
{
   if (glfwWindowShouldClose( Window )) initialize();

   setCubeObject( 5.0f );
   ObjectShader->setUniformLocations( 0 );

   while (!glfwWindowShouldClose( Window )) {
      render();

      glfwSwapBuffers( Window );
      glfwPollEvents();
   }
   glfwDestroyWindow( Window );
}
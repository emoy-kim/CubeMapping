#pragma once

#include <OpenCVLinker.h>
#include <OpenGLLinker.h>

using namespace std;
using namespace cv;
using namespace glm;

class CameraGL
{
   const float ZoomSensitivity;
   const float MoveSensitivity;
   const float RotationSensitivity;
   bool IsMoving;
   float AspectRatio;
   float InitFOV;
   float NearPlane, FarPlane;
   vec3 InitCamPos, InitRefPos, InitUpVec;

public:
   float FOV;
   vec3 CamPos;
   mat4 ViewMatrix, ProjectionMatrix;

   CameraGL();
   CameraGL(
      const vec3& cam_position,
      const vec3& view_reference_position,
      const vec3& view_up_vector,
      float fov = 30.0f,
      float near_plane = 1.0f,
      float far_plane = 10000.0f
   );

   bool getMovingState() const;
   void setMovingState(bool is_moving);
   void updateCamera();
   void pitch(int angle);
   void yaw(int angle);
   void moveForward();
   void moveBackward();
   void moveLeft();
   void moveRight();
   void moveUp();
   void moveDown();
   void zoomIn();
   void zoomOut();
   void resetCamera();
   void updateWindowSize(int width, int height);
};

class ObjectGL
{
   vector<GLfloat> DataBuffer; // 3 for vertex, 3 for normal, and 2 for texture
   vector<VideoCapture> Videos;

   void prepareCubeTextures(const vector<Mat>& cube_images);
   void prepareVertexBuffer(const int& n_bytes_per_vertex);
   GLvoid* bufferOffset(int offset) const { return reinterpret_cast<GLvoid *>(offset); }

public:
   enum LayoutLocation { VertexLoc=0, NormalLoc, TextureLoc };

   GLuint ObjVAO, ObjVBO;
   GLenum DrawMode;
   GLuint TextureID;
   GLsizei VerticesCount;
   vec3 Colors;

   ObjectGL();

   void setObject(
      GLenum draw_mode, 
      const vec3& color,
      const vector<vec3>& vertices,
      const string& texture_directory_path
   );

   void setVideoObject(
      GLenum draw_mode, 
      const vec3& color, 
      const vector<glm::vec3>& vertices,
      const string& texture_video_path
   );

   void updateTextures();
};

class ShaderGL
{
public:
   GLuint ShaderProgram;
   GLint MVPLocation, ColorLocation, TextureLocation;
   
   ShaderGL();

   void setShader(const GLchar* const vertex_source, const GLchar* const fragment_source);
};

class CubeMapping
{
   static CubeMapping* Renderer;
   GLFWwindow* Window;

   bool IsVideo;
   Point ClickedPoint;

   CameraGL MainCamera;
   ShaderGL CubeShader;
   ObjectGL CubeObject;

   void registerCallbacks() const;
   void initializeOpenGL(const int& width, const int& height);
   void setCubeShader();
   void setCubeObject(const float& length = 1.0f);
   void initialize();

   void printOpenGLInformation() const;

   void error(int error, const char* description) const;
   void cleanup(GLFWwindow* window);
   void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
   void cursor(GLFWwindow* window, double xpos, double ypos);
   void mouse(GLFWwindow* window, int button, int action, int mods);
   void mousewheel(GLFWwindow* window, double xoffset, double yoffset);
   void reshape(GLFWwindow* window, int width, int height);
   static void errorWrapper(int error, const char* description);
   static void cleanupWrapper(GLFWwindow* window);
   static void keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
   static void cursorWrapper(GLFWwindow* window, double xpos, double ypos);
   static void mouseWrapper(GLFWwindow* window, int button, int action, int mods);
   static void mousewheelWrapper(GLFWwindow* window, double xoffset, double yoffset);
   static void reshapeWrapper(GLFWwindow* window, int width, int height);

   void render();


public:
   CubeMapping(const bool& is_video = false);
   ~CubeMapping();

   void play();
};
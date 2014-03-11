#include "Angel.h"
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};

// RGBA colors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};

// Parameters controlling the size of the Robot's arm
const GLfloat BASE_HEIGHT      = 2.0;
const GLfloat BASE_WIDTH       = 5.0;
const GLfloat LOWER_ARM_HEIGHT = 5.0;
const GLfloat LOWER_ARM_WIDTH  = 0.5;
const GLfloat UPPER_ARM_HEIGHT = 5.0;
const GLfloat UPPER_ARM_WIDTH  = 0.5;
const GLfloat BALL_RADIUS      = 0.25;

// Shader transformation matrices
mat4  model_view;
GLuint ModelView, Projection;

// Array of rotation angles (in degrees) for each rotation axis
enum { Base = 0, LowerArm = 1, UpperArm = 2, NumAngles = 3 };
const int TopView = 3;
int      Axis = Base;
GLfloat  Theta[NumAngles] = { 0.0 };

// Menu option values
const int  Quit = 4;

bool isTopView = false;
bool FetchMode = false;
bool withBall = false;
bool finishFetch = false;
point4 old_position;
point4 new_position;
point4 cur_position;
GLfloat oldTheta[NumAngles] = { 0.0 };
GLfloat newTheta[NumAngles] = { 0.0 };
GLfloat moveTime[NumAngles] = { 0.0 };
timeval startTime;
double speed = 360 / 5000; //360 per 5000ms(5s)

//----------------------------------------------------------------------------

int Index = 0;

void
quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}

void
colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

int
elapsed()
{
    timeval current;
    gettimeofday(&current, NULL);
    double elapsedTime;
    elapsedTime = (current.tv_sec - startTime.tv_sec) * 1000.0;
    elapsedTime += (current.tv_usec - startTime.tv_usec) / 1000.0;
    return int(elapsedTime); // ms
}

//----------------------------------------------------------------------------

/* Define the three parts */
/* Note use of push/pop to return modelview matrix
to its state before functions were entered and use
rotation, translation, and scaling to create instances
of symbols (cube and cylinder */

void
base()
{
    mat4 instance = ( Translate( 0.0, 0.5 * BASE_HEIGHT, 0.0 ) *
		 Scale( BASE_WIDTH,
			BASE_HEIGHT,
			BASE_WIDTH ) );

    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );

    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void
upper_arm()
{
    mat4 instance = ( Translate( 0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0 ) *
		      Scale( UPPER_ARM_WIDTH,
			     UPPER_ARM_HEIGHT,
			     UPPER_ARM_WIDTH ) );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void
lower_arm()
{
    mat4 instance = ( Translate( 0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0 ) *
		      Scale( LOWER_ARM_WIDTH,
			     LOWER_ARM_HEIGHT,
			     LOWER_ARM_WIDTH ) );
    
    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void
ball()
{
    double ball_y = cur_position.y;
    if (withBall) {
        ball_y -= BASE_HEIGHT + LOWER_ARM_HEIGHT;
    }
    mat4 instance = ( Translate( cur_position.x, ball_y, cur_position.z ) *
                     Scale( BALL_RADIUS * 2,
                           BALL_RADIUS * 2,
                           BALL_RADIUS * 2 ) );

    glUniformMatrix4fv( ModelView, 1, GL_TRUE, model_view * instance );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------

void
idle()
{
    glutPostRedisplay();
}

void
setWithBall()
{
    withBall = true;
    gettimeofday(&startTime, NULL);
# warning 换targetTheta，换moveTime，换startTheta
}

void
setFinishFetch()
{
    withBall = false;
    finishFetch = true;
}

int
sign(double x)
{
    if (x < 0) {
        return -1;
    }
    else {
        return 1;
    }
}

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // Accumulate ModelView Matrix as we traverse the tree
    if (isTopView) {
        model_view = ( Translate(0, BASE_WIDTH, 0) *
                      RotateX(270.0) );
    }
    else {
        model_view = ( RotateX(0) );
    }

    if (FetchMode) {
# warning 合并!withBall和withBall
        if (!withBall) { // Initial theta to oldTheta
            ball();
            bool moved = false;
# warning 合并成move函数
            if (elapsed() < moveTime[Base]) {
# warning 加startTheta
                Theta[Base] = elapsed() * speed * sign(oldTheta[Base]);
                if (Theta[Base] < 0.0) {
                    Theta[Base] += 360.0;
                }
                moved = true;
            }
            model_view *= RotateY(Theta[Base]);
            base();

            if (elapsed() < moveTime[LowerArm]) {
                Theta[LowerArm] = elapsed() * speed * sign(oldTheta[LowerArm]);
                if (Theta[LowerArm] < 0.0) {
                    Theta[LowerArm] += 360.0;
                }
                moved = true;
            }
            model_view *= ( Translate(0.0, BASE_HEIGHT, 0.0) *
                           RotateZ(Theta[LowerArm]) );
            lower_arm();

            if (elapsed() < moveTime[UpperArm]) {
                Theta[UpperArm] = elapsed() * speed * sign(oldTheta[UpperArm]);
                if (Theta[UpperArm] < 0.0) {
                    Theta[UpperArm] += 360.0;
                }
                moved = true;
            }
            model_view *= ( Translate(0.0, LOWER_ARM_HEIGHT, 0.0) *
                           RotateZ(Theta[UpperArm]) );
            upper_arm();
            if (!moved) {
                setWithBall();
            }
        }
        else if (withBall) { // oldTheta to newTheta
            model_view *= RotateY(Theta[Base]);
            base();
            ball();
            model_view *= ( Translate(0.0, BASE_HEIGHT, 0.0) *
                           RotateZ(Theta[LowerArm]) );
            lower_arm();

            model_view *= ( Translate(0.0, LOWER_ARM_HEIGHT, 0.0) *
                           RotateZ(Theta[UpperArm]) );
            upper_arm();
            if (Theta[Base] == newTheta[Base] && Theta[LowerArm] == newTheta[LowerArm] && Theta[UpperArm] == newTheta[UpperArm]) {
                setFinishFetch();
            }
        }

    }
    else {
        if (finishFetch) {
            ball();
        }
        model_view *= RotateY(Theta[Base]);
        base();
        model_view *= ( Translate(0.0, BASE_HEIGHT, 0.0) *
                RotateZ(Theta[LowerArm]) );
        lower_arm();

        model_view *= ( Translate(0.0, LOWER_ARM_HEIGHT, 0.0) *
                RotateZ(Theta[UpperArm]) );
        upper_arm();
    }

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
init( void )
{
    colorcube();
    
    // Create a vertex array object
    GLuint vao;
    glGenVertexArraysAPPLE( 1, &vao );
    glBindVertexArrayAPPLE( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );
    
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader81.glsl", "fshader81.glsl" );
    glUseProgram( program );
    
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

    ModelView = glGetUniformLocation( program, "ModelView" );
    Projection = glGetUniformLocation( program, "Projection" );

    glEnable( GL_DEPTH );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); 
}

//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{

    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
	// Incrase the joint angle
	Theta[Axis] += 5.0;
	if ( Theta[Axis] > 360.0 ) { Theta[Axis] -= 360.0; }
    }

    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ) {
	// Decrase the joint angle
	Theta[Axis] -= 5.0;
	if ( Theta[Axis] < 0.0 ) { Theta[Axis] += 360.0; }
    }

    glutPostRedisplay();
}

//----------------------------------------------------------------------------

void
menu( int option )
{
    if ( option == Quit ) {
	exit( EXIT_SUCCESS );
    }
    else if (option == TopView) {
        isTopView = !isTopView;
        if (isTopView) {
            glutChangeToMenuEntry(TopView+1, "side view", TopView);
        }
        else {
            glutChangeToMenuEntry(TopView+1, "top view", TopView);
        }
    }
    else {
	Axis = option;
    }
}

//----------------------------------------------------------------------------

void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

    GLfloat  left = -10.0, right = 10.0;
    GLfloat  bottom = -5.0, top = 15.0;
    GLfloat  zNear = -10.0, zFar = 10.0;

    GLfloat aspect = GLfloat(width)/height;

    if ( aspect > 1.0 ) {
	left *= aspect;
	right *= aspect;
    }
    else {
	bottom /= aspect;
	top /= aspect;
    }

    mat4 projection = Ortho( left, right, bottom, top, zNear, zFar );
    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );

    model_view = mat4( 1.0 );  // An Identity matrix
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033: // Escape Key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    }
}

//----------------------------------------------------------------------------

double
cosLaw(double a, double b, double c)
{
    return acos((pow(a, 2) + pow(b, 2) - pow(c, 2)) / (2 * a * b)) * 180 / M_PI;
}

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    if (argc > 1) {
        FetchMode = true;
        old_position = point4(atof(argv[1]), atof(argv[2]), atof(argv[3]), 1.0);
        new_position = point4(atof(argv[4]), atof(argv[5]), atof(argv[6]), 1.0);
        cur_position = old_position;
        if (strcmp(argv[7], "-tv") == 0) {
            isTopView = true;
        }

        old_position.y -= BASE_HEIGHT;
        double oldTopTheta = cosLaw(LOWER_ARM_HEIGHT, UPPER_ARM_HEIGHT+BALL_RADIUS, length(vec2(old_position.x, old_position.y)));
        double oldBottomTheta = cosLaw(LOWER_ARM_HEIGHT, length(vec2(old_position.x, old_position.y)), UPPER_ARM_HEIGHT+BALL_RADIUS);
        double oldBallBottomTheta = atan(old_position.x / old_position.y) * 180 / M_PI;
        oldTheta[Base] = atan(old_position.z / old_position.x) * 180 / M_PI;
        if (old_position.x < 0) {
            oldTheta[LowerArm] = oldBallBottomTheta - oldBottomTheta;
            oldTheta[UpperArm] = 180 - oldTopTheta;
        }
        else {
            oldTheta[LowerArm] = oldBottomTheta - oldBallBottomTheta;
            oldTheta[UpperArm] = oldTopTheta - 180;
        }
        old_position.y += BASE_HEIGHT;
        moveTime[Base] = abs(oldTheta[Base]) / speed;
        moveTime[LowerArm] = abs(oldTheta[LowerArm]) / speed;
        moveTime[UpperArm] = abs(oldTheta[UpperArm]) / speed;

#warning 挪到setWithBall里去，把oldTheta改成targetTheta
        new_position.y -= BASE_HEIGHT;
        double newTopTheta = cosLaw(LOWER_ARM_HEIGHT, UPPER_ARM_HEIGHT+BALL_RADIUS, length(vec2(new_position.x, new_position.y)));
        double newBottomTheta = cosLaw(LOWER_ARM_HEIGHT, length(vec2(new_position.x, new_position.y)), UPPER_ARM_HEIGHT+BALL_RADIUS);
        double newBallBottomTheta = atan(new_position.x / new_position.y) * 180 / M_PI;
        newTheta[Base] = atan(new_position.z / new_position.x) * 180 / M_PI;
        if (new_position.x < 0) {
            newTheta[LowerArm] = newBallBottomTheta - newBottomTheta;
            newTheta[UpperArm] = 180 - newTopTheta;
        }
        else {
            newTheta[LowerArm] = newBottomTheta - newBallBottomTheta;
            newTheta[UpperArm] = newTopTheta - 180;
        }
        new_position.y += BASE_HEIGHT;
        newTheta[Base] = newTheta[Base] - oldTheta[Base];
        newTheta[LowerArm] = newTheta[LowerArm] - oldTheta[LowerArm];
        newTheta[UpperArm] = newTheta[UpperArm] - oldTheta[UpperArm];
    }
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "robot" );
    gettimeofday(&startTime, NULL);
    init();

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutMouseFunc( mouse );
    glutIdleFunc( idle );

    glutCreateMenu( menu );
    // Set the menu values to the relevant rotation axis values (or Quit)
    glutAddMenuEntry( "base", Base );
    glutAddMenuEntry( "lower arm", LowerArm );
    glutAddMenuEntry( "upper arm", UpperArm );
    if (isTopView) {
    glutAddMenuEntry( "side view", TopView);
    }
    else {
    glutAddMenuEntry( "top view", TopView);
    }
    glutAddMenuEntry( "quit", Quit );
    glutAttachMenu( GLUT_MIDDLE_BUTTON );

    glutMainLoop();
    return 0;
}
